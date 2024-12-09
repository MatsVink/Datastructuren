/* Name: Mats Vink
 * UvAnetID: 15874648
 * Program: BSc Informatics
 * 
 * Description:
 * This file implements a heap-based priority queue. The heap maintains the 
 * smallest element at the root, allowing for efficient insertion and removal 
 * of elements based on a comparison function. The heap is dynamically resized 
 * using an array-based implementation. 
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "array.h"
#include "prioq.h"

/* 
 * Initialize a new heap structure.
 * compare: Comparison function for ordering elements in the heap.
 * Returns a pointer to the initialized heap, or NULL on failure.
 */
static struct heap *heap_init(int (*compare)(const void *, const void *)) {
    struct heap *h = malloc(sizeof(struct heap));
    if (!h) {
        return NULL;
    }

    h->array = array_init(100);
    if (!h->array) {
        free(h);
        return NULL;
    }

    h->compare = compare;
    return h;
}

/* 
 * Initialize a priority queue.
 * compare: Comparison function for ordering elements.
 * Returns a pointer to the initialized priority queue, or NULL on failure.
 */
prioq *prioq_init(int (*compare)(const void *, const void *)) {
    return heap_init(compare);
}

/* 
 * Get the size of the priority queue.
 * q: Pointer to the priority queue.
 * Returns the number of elements in the queue, or -1 on error.
 */
long int prioq_size(const prioq *q) {
    if (!q || !q->array) {
        return -1;
    }
    return array_size(q->array);
}

/* 
 * Clean up the heap and free its resources.
 * h: Pointer to the heap.
 * free_func: Function to free individual elements.
 * Returns 0 on success, -1 on error.
 */
static int heap_cleanup(struct heap *h, void (*free_func)(void *)) {
    if (!h) {
        return -1;
    }

    array_cleanup(h->array, free_func);
    free(h);
    return 0;
}

/* 
 * Clean up the priority queue and free its resources.
 * q: Pointer to the priority queue.
 * free_func: Function to free individual elements.
 * Returns 0 on success, -1 on error.
 */
int prioq_cleanup(prioq *q, void (*free_func)(void *)) {
    if (!q) {
        return -1;
    }
    return heap_cleanup(q, free_func);
}

/* 
 * Insert an element into the heap.
 * h: Pointer to the heap.
 * p: Pointer to the element to insert.
 * Returns 0 on success, -1 on error.
 */
static int heap_insert(struct heap *h, void *p) {
    if (!h || !p) {
        return -1;
    }

    if (array_append(h->array, p) == -1) {
        return -1;
    }

    long index = array_size(h->array) - 1;
    while (index > 0) {
        long parent = (index - 1) / 2;
        void *current = array_get(h->array, index);
        void *parent_elem = array_get(h->array, parent);

        if (h->compare(current, parent_elem) < 0) {
            array_set(h->array, index, parent_elem);
            array_set(h->array, parent, current);
            index = parent;
        } else {
            break;
        }
    }

    return 0;
}

/* 
 * Insert an element into the priority queue.
 * q: Pointer to the priority queue.
 * p: Pointer to the element to insert.
 * Returns 0 on success, -1 on error.
 */
int prioq_insert(prioq *q, void *p) {
    return heap_insert(q, p);
}

/* 
 * Remove and return the smallest element from the heap.
 * h: Pointer to the heap.
 * Returns a pointer to the removed element, or NULL if the heap is empty.
 */
static void *heap_pop(struct heap *h) {
    if (!h || array_size(h->array) == 0) {
        return NULL;
    }

    long size = array_size(h->array);
    void *top = array_get(h->array, 0);
    void *last = array_get(h->array, size - 1);

    array_set(h->array, 0, last);
    array_set(h->array, size - 1, NULL);
    array_pop(h->array);

    long index = 0;
    while (1) {
        long left = 2 * index + 1;
        long right = 2 * index + 2;
        long smallest = index;

        void *current = array_get(h->array, index);
        void *left_elem = (left < array_size(h->array)) ? array_get(h->array, left) : NULL;
        void *right_elem = (right < array_size(h->array)) ? array_get(h->array, right) : NULL;

        if (left_elem && h->compare(left_elem, current) < 0) {
            smallest = left;
        }
        if (right_elem && h->compare(right_elem, array_get(h->array, smallest)) < 0) {
            smallest = right;
        }

        if (smallest != index) {
            void *smallest_elem = array_get(h->array, smallest);
            array_set(h->array, index, smallest_elem);
            array_set(h->array, smallest, current);
            index = smallest;
        } else {
            break;
        }
    }

    return top;
}

/* 
 * Remove and return the smallest element from the priority queue.
 * q: Pointer to the priority queue.
 * Returns a pointer to the removed element, or NULL if the queue is empty.
 */
void *prioq_pop(prioq *q) {
    return heap_pop(q);
}
