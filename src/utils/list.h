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
	void (*_free_val)(void *);
} List;

/**
 * Allocates and initialises a List into list
 * 
 * @param list the list to initialise
 * @param size the size (in bytes) of the elements in the list
 * @param capacity the initial capacity of the list (0 sets it to the default capacity)
 * @param to_string a function that takes a value (from the list) and returns a heap
 * allocated string representing the value
 * 
 * @return capacity of the list or 0
 */
size_t init_list(List *list, size_t size, size_t capacity, char*(*to_string)(void*), void(*free_val)(void*));

/**
 * Increases list capacity by default scale factor
 * 
 * @return The new size of the list or 0
 */
size_t list_increase_capacity(List *);

/**
 * Append element to list, increasing size if necessary
 * 
 * @return The new size of the list or 0
 */
size_t list_add_element(List *list, const void *element);

/**
 * Gets the element of the list at the given index
 * 
 * @return the value of the element at the index or NULL
 */
void *list_get_index(List *list, size_t index);

/**
 * prints the contents of the list to stdout
 */
void print_list(List *list);

/**
 * caches a list to disk. Expects the read/write head to be positioned correctly and may
 * require `seek`-ing before use
 * 
 * @param list List to be cached
 * @param cache Pointer to cache file
 * 
 * @return number of bytes written to cache, 0 on error
 */
size_t list_write_cache(List *list, FILE *cache);

/**
 * loads list values from cache. Expects read/write head to be positioned correctly and
 * may require `seek`-ing before use
 * 
 * @param list Pointer to initialised List
 * @param cache Pointer to cache file
 * 
 * @return number of bytes read from cache, 0 on error
 * 
 * @note Sets errno to ENOMEM if realloc of List.list fails
 */
size_t list_read_cache(List *list, FILE *cache);

void delete_list(void *list);

#endif // _DYNAMIC_LIST