/* Name: Mats Vink
 * UvAnetID: 15874648
 * Program: BSc Informatics
 */

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/* Queue structure holding data, positions, and usage statistics */
struct queue {
    int *data;
    size_t front;          // Position for the next element to be dequeued
    size_t back;           // Position for the next element to be enqueued
    size_t total_push;     // Total number of enqueue operations
    size_t total_pop;      // Total number of dequeue operations
    size_t max_elements;   // Maximum elements held at once
    size_t capacity;       // Queue's maximum capacity
    size_t size;           // Current number of elements in the queue
};

/* Initializes a queue with a specified capacity.
 * capacity: Maximum number of elements the queue can hold
 * Returns: Pointer to the queue if successful, NULL if memory allocation fails
 */
struct queue *queue_init(size_t capacity) {
    struct queue *q = malloc(sizeof(struct queue));
    if (q == NULL) {
        return NULL;
    }
    q->data = malloc(capacity * sizeof(int));
    if (q->data == NULL) {
        free(q);
        return NULL;
    }
    q->front = 0;
    q->back = 0;
    q->total_pop = 0;
    q->total_push = 0;
    q->max_elements = 0;
    q->capacity = capacity;
    q->size = 0;
    return q;
}

/* Frees all memory associated with the queue */
void queue_cleanup(struct queue *q) {
    if (q == NULL) {
        return;
    }
    free(q->data);
    free(q);
}

/* Prints queue statistics to stderr in format: 'stats' total_push total_pop max_elements
 * q: Pointer to the queue
 */
void queue_stats(const struct queue *q) {
    if (q == NULL) {
        return;
    }
    fprintf(stderr, "stats %zu %zu %zu\n", q->total_push, q->total_pop, q->max_elements);
}

/* Enqueues an item to the back of the queue
 * q: Pointer to the queue
 * e: Item to be enqueued
 * Returns: 0 if successful, 1 if queue is full or NULL
 */
int queue_push(struct queue *q, int e) {
    if (q == NULL || q->size == q->capacity) {
        return 1;
    }

    q->data[q->front] = e;
    q->front = (q->front + 1) % q->capacity;
    q->total_push++;
    q->size++;

    if (q->max_elements < q->size) {
        q->max_elements = q->size;
    }
    return 0;
}

/* Dequeues the front item from the queue
 * q: Pointer to the queue
 * Returns: Front item if successful, -1 if queue is empty or NULL
 */
int queue_pop(struct queue *q) {
    if (q == NULL || q->size == 0) {
        return -1;
    }

    int item = q->data[q->back];
    q->back = (q->back + 1) % q->capacity;
    q->total_pop++;
    q->size--;

    return item;
}

/* Returns the front item of the queue without removing it
 * q: Pointer to the queue
 * Returns: Front item if successful, -1 if queue is empty or NULL
 */
int queue_peek(const struct queue *q) {
    if (q == NULL || q->size == 0) {
        return -1;
    }
    return q->data[q->back];
}

/* Checks if the queue is empty
 * q: Pointer to the queue
 * Returns: 1 if queue is empty, 0 if it has elements, -1 if NULL
 */
int queue_empty(const struct queue *q) {
    if (q == NULL) {
        return -1;
    }
    return q->size == 0 ? 1 : 0;
}

/* Returns the number of elements stored in the queue
 * q: Pointer to the queue
 * Returns: Number of elements if successful, 0 if NULL
 */
size_t queue_size(const struct queue *q) {
    if (q == NULL) {
        return 0;
    }
    return q->size;
}
