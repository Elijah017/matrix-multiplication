#include "list.h"

size_t init_list(List *list, size_t size, size_t capacity, char*(*to_string)(void*), void(*free_val)(void*)) {
	capacity = capacity == 0 ? _LIST_DEFAULT_CAPACITY : capacity;

	*list = (List){
		.list = calloc(capacity, size),
		._capacity = capacity,
		._size = size,
		.length = 0,
		._to_string = to_string,
		._free_val = free_val
	};

	if (list->list == NULL) { return 0; }
	return list->_capacity;
}

size_t list_increase_capacity(List *list) {
	size_t new_cap = list->_capacity * _LIST_SCALE_FACTOR;
	if (new_cap <= list->_capacity) {
		errno = EOVERFLOW;
		return -2;
	}

	list->list = realloc(list->list, new_cap);
	if (list->list == NULL) { return -1; }

	list->_capacity = new_cap;
	return list->_capacity;
}

size_t list_add_element(List *list, const void *element) {
	if (list->_capacity == list->length) {
		if (list_increase_capacity(list) <= 0) { return 0; }
	}

	memcpy(list->list + (list->length * list->_size), element, list->_size);
	list->length++;

	return list->length;
}

void *list_get_index(List *list, size_t index) {
	if (index >= list->length) { return NULL; }
	return list->list + (index * list->_size);
}

void print_list(List *list) {
	printf("[ ");

	for (size_t i = 0; i < list->length; i++) {
		void *value = list_get_index(list, i);
		if (i == list->length - 1) {
			if (list->_to_string == NULL) {
				printf("%p", value);
			} else {
				printf("%s", list->_to_string(value));
			}
		} else {
			if (list->_to_string == NULL) {
				printf("%p, ", value);
			} else {
				printf("%s, ", list->_to_string(value));
			}
		}
		free(value);
	}

	printf(" ]\n");
}

void *list_set_index(List *list, size_t index, void *element) {
	if (index >= list->length) { return NULL; }

	memcpy(list->list + (index * list->_size), element, list->_size);
	return list_get_index(list, index);
}

size_t list_write_cache(List *list, FILE *cache) {
	size_t length = fwrite(&list->length, sizeof(size_t), 1, cache);
	if (length == 0) { return 0; }
	size_t bytes = fwrite(list->list, list->_size, list->length, cache);
	if (bytes == 0) { return 0; }
	return (sizeof(size_t) * length) + (bytes * list->_size);
}

size_t list_read_cache(List *list, FILE *cache) {
	size_t length;
	size_t lbytes = fread(&length, sizeof(size_t), 1, cache);
	if (lbytes == 0) { return 0; }

	list->list = realloc(list->list, length);
	if (list->list == NULL) {
		errno = ENOMEM;
		return 0;
	}
	list->_capacity = length;
	list->length = length; 

	size_t vbytes = fread(list->list, list->_size, length, cache);
	if (vbytes == 0) { return 0; }
	return (sizeof(size_t) * lbytes) + (vbytes * list->_size);
}

void delete_list(void *lst) {
	List *list = lst;
	if (list->list == NULL) { return; }

	if (list->_free_val != NULL) {
		for (size_t i = 0; i < list->length; i++) {
			list->_free_val(list_get_index(list, i));
		}
	}

	free(list->list);
	list->list = NULL;
}
