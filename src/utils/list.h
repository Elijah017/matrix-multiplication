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
 * @param to_string a function that takes a value (from the list) and returns a heap
 * allocated string representing the value
 * 
 * @return capacity of the list or 0
 */
size_t init_list(List *list, size_t size, size_t capacity, char*(*to_string)(void*));

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
size_t list_add_element(List *list, void *element);

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
 * Dumps the binary contents of the list into the provided file. The formatting of the
 * files assumes the user knows the type of the list when it is required
 * 
 * @param list The list being cached
 * @param cache The file storing the list
 */
void list_write_context_cache(List *list, FILE *cache);

/**
 * Reads the contents of a binary list previously cached context assumed list. The
 * context expected being the size of the type of value stored by the list and the
 * to_string function.
 * 
 * @param list List that will be initialised and store the cached contents
 * @param size Size (in bytes) of the lists datatype
 * @param to_string Function that converts a value to a string
 * @param cache Cache to be read from with read/write head possitioned to the appropiate
 * element (may require use of seek prior to use of this function)
 */
void list_read_context_cache(List *list, size_t size, char*(*to_string)(void*), FILE *cache);

#endif // _DYNAMIC_LIST