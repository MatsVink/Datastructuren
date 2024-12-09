/* Name: Mats Vink
 * UvAnetID: 15874648
 * Program: BSc Informatics
 */

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

/* Stack structure holding data, capacity, and usage statistics */
struct stack {
    int *data;
    size_t used;          // Number of elements currently in the stack
    size_t total_push;    // Total number of push operations
    size_t total_pop;     // Total number of pop operations
    size_t max_numbers;   // Maximum number of elements held in stack at once
    size_t capacity;      // Stack's maximum capacity
};

/* Initializes a stack with a specified capacity.
 * capacity: Maximum number of elements the stack can hold
 * Returns: Pointer to the stack if successful, NULL if memory allocation fails
 */
struct stack *stack_init(size_t capacity) {
    struct stack *sp = malloc(sizeof(struct stack));
    if (sp == NULL) {
        return NULL;
    }
    sp->data = malloc(capacity * sizeof(int));
    if (sp->data == NULL) {
        free(sp);
        return NULL;
    }
    sp->used = 0;
    sp->total_pop = 0;
    sp->total_push = 0;
    sp->max_numbers = 0;
    sp->capacity = capacity;
    return sp;
}

/* Frees all memory associated with the stack */
void stack_cleanup(struct stack *s) {
    if (s == NULL) {
        return;
    }
    free(s->data);
    free(s);
}

/* Prints stack statistics to stderr in format: 'stats' total_push total_pop max_numbers
 * s: Pointer to the stack
 */
void stack_stats(const struct stack *s) {
    if (s == NULL) {
        return;
    }
    fprintf(stderr, "stats %zu %zu %zu\n", s->total_push, s->total_pop, s->max_numbers);
}

/* Pushes an item onto the stack
 * s: Pointer to the stack
 * c: Item to be pushed
 * Returns: 0 if successful, 1 if stack is full or NULL
 */
int stack_push(struct stack *s, int c) {
    if (s == NULL || s->used >= s->capacity) {
        return 1;
    }

    s->data[s->used++] = c;
    s->total_push++;
    if (s->max_numbers < s->used) {
        s->max_numbers = s->used;
    }
    return 0;
}

/* Pops the top item from the stack
 * s: Pointer to the stack
 * Returns: Top item if successful, -1 if stack is empty or NULL
 */
int stack_pop(struct stack *s) {
    if (s == NULL || s->used == 0) {
        return -1;
    }
    s->total_pop++;
    return s->data[--s->used];
}

/* Returns the top item of the stack without removing it
 * s: Pointer to the stack
 * Returns: Top item if successful, -1 if stack is empty or NULL
 */
int stack_peek(const struct stack *s) {
    if (s == NULL || s->used == 0) {
        return -1;
    }
    return s->data[s->used - 1];
}

/* Checks if the stack is empty
 * s: Pointer to the stack
 * Returns: 1 if stack is empty, 0 if it has elements, -1 if NULL
 */
int stack_empty(const struct stack *s) {
    if (s == NULL) {
        return -1;
    }
    return s->used == 0 ? 1 : 0;
}

/* Returns the number of elements stored in the stack
 * s: Pointer to the stack
 * Returns: Number of elements if successful, 0 if NULL
 */
size_t stack_size(const struct stack *s) {
    if (s == NULL) {
        return 0;
    }
    return s->used;
}
