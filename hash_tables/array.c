/* Name: Mats Vink
 * UvAnetID: 15874648
 * Program: BSc Informatics
 * 
 * Description:
 * This file implements a dynamic array data structure for storing integers.
 * The array supports initialization, cleanup, element retrieval and appending 
 * elements. The array dynamically resizes as needed.
 */

#include <stdlib.h>
#include "array.h"

struct array {
    int *data;
    size_t used;
    size_t capacity;
};

/* 
 * Initialize a dynamic array.
 * -initial_capacity: The initial capacity of the array.
 * Returns apointer to the initialized array, or NULL.
 */
struct array *array_init(unsigned long initial_capacity) {
    struct array *a = malloc(sizeof(struct array));
    if (a == NULL) {
        return NULL;
    }

    a->data = malloc(initial_capacity * sizeof(int));
    if (a->data == NULL) {
        free(a);
        return NULL;
    }

    a->capacity = initial_capacity;
    a->used = 0;
    return a;
}

/* 
 * Free the memory of a dynamic array.
 */
void array_cleanup(struct array *a) {
    if (a == NULL) {
        return;
    }
    free(a->data);
    free(a);
}

/* 
 * Retrieve the element at a specific index in the array.
 * 
 * a: The array to access.
 * index: The index of the desired element.
 * 
 * Returns the element at the given index, or -1 if the index is out of bounds.
 */
int array_get(const struct array *a, unsigned long index) {
    if (a == NULL || index >= a->used) {
        return -1;
    }
    return a->data[index];
}

/* 
 * Append an element to the end of the array.
 * 
 * a: The array to modify.
 * elem: The element to append.
 * 
 * Returns 0 if the operation is successful, 1 if not.
 */
int array_append(struct array *a, int elem) {
    if (a == NULL) {
        return 1;
    }

    if (a->used >= a->capacity) {
        size_t new_capacity = a->capacity * 2;
        int *new_data = realloc(a->data, new_capacity * sizeof(int));
        if (new_data == NULL) {
            return 1;
        }
        a->data = new_data;
        a->capacity = new_capacity;
    }

    a->data[a->used] = elem;
    a->used++;

    return 0;
}

/* 
 * Get the number of elements currently stored in the array.
 * 
 * a: The array.
 * 
 * Returns the number of elements in the array. If a is NULL, the return value is 1.
 */
unsigned long array_size(const struct array *a) {
    if (a == NULL) {
        return 1;
    }

    return a->used;
}
