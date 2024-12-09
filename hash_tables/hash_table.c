/* Name: Mats Vink
 * UvAnetID: 15874648
 * Program: BSc Informatics
 * 
 * Description:
 * This file implements a hash table that uses separate chaining for collision
 * resolution. Each key is associated with an array of integer values, which dynamically
 * resizes as needed. The hash table supports insertion, lookup, deletion, resizing, 
 * and cleanup.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "hash_table.h"

struct table {
    /* The (simple) array used to index the table */
    struct node **array;
    /* The function used for computing the hash values in this table */
    unsigned long (*hash_func)(const unsigned char *);
    /* Maximum load factor after which the table array should be resized */
    double max_load_factor;
    /* Capacity of the array used to index the table */
    unsigned long capacity;
    /* Current number of elements stored in the table */
    unsigned long load;
};

/* Note: This struct should be a *strong* hint to a specific type of hash table
 * You may implement other options, if you can build them in such a way they
 * pass all tests. However, the other options are generally harder to code. */
struct node {
    /* The string of characters that is the key for this node */
    char *key;
    /* A resizing array, containing the all the integer values for this key */
    struct array *value;

    /* Next pointer */
    struct node *next;
};

/* 
 * Resize the hash table to twice its current capacity.
 * 
 * t: The hash table to resize.
 * 
 * Returns 0 on success, 1 on failure.
 */
int table_resize(struct table *t) {
    unsigned long new_capacity = t->capacity * 2;
    struct node **new_array = calloc(new_capacity, sizeof(struct node *));
    if (new_array == NULL) {
        return 1;
    }

    for (unsigned long i = 0; i < t->capacity; i++) {
        struct node *current = t->array[i];
        while (current != NULL) {
            struct node *next = current->next;

            unsigned long new_index = t->hash_func((const unsigned char *)current->key) % new_capacity;
            current->next = new_array[new_index];
            new_array[new_index] = current;

            current = next;
        }
    }
    free(t->array);
    t->array = new_array;
    t->capacity = new_capacity;

    return 0;
}

/* 
 * Initialize a hash table.
 * 
 * capacity: Initial capacity of the hash table.
 * max_load_factor: Maximum load factor before resizing.
 * hash_func: Pointer to the hash function to use.
 * 
 * Returnsa pointer to the initialized hash table, or NULL on failure.
 */
struct table *table_init(unsigned long capacity,
                         double max_load_factor,
                         unsigned long (*hash_func)(const unsigned char *)) {
    if (capacity == 0 || max_load_factor <= 0 || hash_func == NULL) {
        return NULL;
    }

    struct table *t = malloc(sizeof(struct table));
    if (t == NULL) {
        return NULL;
    }

    t->array = calloc(capacity, sizeof(struct node *));
    if (t->array == NULL) {
        free(t);
        return NULL;
    }
    t->hash_func = hash_func;
    t->max_load_factor = max_load_factor;
    t->capacity = capacity;
    t->load = 0;

    return t;
}

/* 
 * Copies and inserts a key into the hash table, along with its value.
 * If the key already exists, the value is appended to the existing array.
 * 
 * t: The hash table.
 * key: The key to insert.
 * value: The value to associate with the key.
 * 
 * Returns 0 on success, 1 on failure.
 */
int table_insert(struct table *t, const char *key, int value) {
    if (t == NULL || key == NULL) {
        return 1;
    }

    unsigned long index = t->hash_func((const unsigned char *)key) % t->capacity;
    struct node *current = t->array[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (array_append(current->value, value) != 0) {
                return 1;
            }
            return 0;
        }
        current = current->next;
    }
    struct node *new_node = malloc(sizeof(struct node));
    if (new_node == NULL) {
        return 1;
    }
    new_node->key = malloc(strlen(key) + 1);
    if (new_node->key == NULL) {
        free(new_node);
        return 1;
    }
    strcpy(new_node->key, key);

    new_node->value = array_init(4);
    if (new_node->value == NULL) {
        free(new_node->key);
        free(new_node);
        return 1;
    }

    if (array_append(new_node->value, value) != 0) {
        array_cleanup(new_node->value);
        free(new_node->key);
        free(new_node);
        return 1;
    }
    new_node->next = t->array[index];
    t->array[index] = new_node;
    t->load++;

    if ((double)t->load / t->capacity > t->max_load_factor) {
        if (table_resize(t) != 0) {
            return 1;
        }
    }
    return 0;
}

/* 
 * Look up a key in the hash table and return its associated array of values.
 * 
 * t: The hash table.
 * key: The key to look up.
 * 
 * Returns a pointer to the array of values, or NULL if the key is not found or t is NULL.
 */
struct array *table_lookup(const struct table *t, const char *key) {
    if (t == NULL || key == NULL) {
        return NULL;
    }

    unsigned long index = t->hash_func((const unsigned char *)key) % t->capacity;
    struct node *current = t->array[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }

    return NULL;
}

/* 
 * Calculate the load factor of the hash table.
 * The load factor is defined as: number of elements stored / table capacity.
 * 
 * t: The hash table.
 * 
 * Returns:
 * The load factor as a double. Returns -1.0 if the hash table is NULL or its 
 * capacity is zero.
 */
double table_load_factor(const struct table *t) {
    if (t == NULL || t->capacity == 0) {
        return -1.0;
    }
    return (double)t->load / t->capacity;
}

/* 
 * Remove the specified key and its associated values from the hash table.
 * 
 * t: The hash table.
 * key: The key to remove.
 * 
 * Returns:
 * 0 if the key was successfully removed.
 * 1 if the key was not found in the table.
 * -1 if the hash table or key is NULL.
 */
int table_delete(struct table *t, const char *key) {
    if (t == NULL || key == NULL) {
        return -1;
    }

    unsigned long index = t->hash_func((const unsigned char *)key) % t->capacity;
    struct node *current = t->array[index];
    struct node *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                t->array[index] = current->next;
            } else {
                prev->next = current->next;
            }

            array_cleanup(current->value);
            free(current->key);
            free(current);
            t->load--;
            return 0;
        }

        prev = current;
        current = current->next;
    }

    return 1;
}

/* 
 * Clean up the hash table and free all allocated memory.
 * 
 * t: The hash table to clean up.
 */
void table_cleanup(struct table *t) {
    if (t == NULL) {
        return;
    }

    for (unsigned long i = 0; i < t->capacity; i++) {
        struct node *current = t->array[i];
        while (current != NULL) {
            struct node *temp = current;
            current = current->next;
            array_cleanup(temp->value);
            free(temp->key);
            free(temp);
        }
    }

    free(t->array);
    free(t);
}
