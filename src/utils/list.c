#include "list.h"

size_t init_list(List *list, size_t size, size_t capacity, char*(*to_string)(void*)) {
	capacity = capacity == 0 ? _LIST_DEFAULT_CAPACITY : capacity;

	*list = (List){
		.list = malloc(size * capacity),
		._capacity = capacity,
		._size = size,
		.length = 0,
		._to_string = to_string
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

size_t list_add_element(List *list, void *element) {
	int err;
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

int print_list(List *list) {
	printf("[ ");

	for (size_t i = 0; i < list->length; i++) {
		char* value = list->_to_string(list_get_index(list, i));
		if (i == list->length - 1) {
			printf("%s", value);
		} else {
			printf("%s, ", value);
		}
		free(value);
	}

	printf(" ]\n");
}
