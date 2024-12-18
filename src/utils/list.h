#ifndef _DYNAMIC_LIST
#define _DYNAMIC_LIST

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define _LIST_DEFAULT_CAPACITY 10
#define _LIST_SCALE_FACTOR 	   2

typedef struct {
    void *list;
    size_t _size;
    size_t _capacity;
    size_t length;
	char* (*_to_string)(void *); // return should be dynamically allocated char*
} List;

/**
 * Allocates and initialises a List into list
 * 
 * @param list the list to initialise
 * @param size the size (in bytes) of the elements in the list
 * @param capacity the initial capacity of the list (0 sets it to the default capacity)
 * 
 * @return capacity of the list or 0
 */
size_t init_list(List *list, size_t size, size_t capacity, char*(*to_string)(void*));

size_t list_increase_capacity(List *);

size_t list_add_element(List *list, void *element);

void *list_get_index(List *list, size_t index);

int print_list(List *list);

#endif // _DYNAMIC_LIST