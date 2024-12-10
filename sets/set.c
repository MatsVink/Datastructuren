/* Name: Mats Vink
 * UvAnetID: 15874648
 * Program: BSc Informatics
 * 
 * Description:
 * This file implements a Set Abstract Data Type (ADT) using a binary search tree (BST) or 
 * an AVL tree (if turbo mode is enabled). The set supports operations such as insert, 
 * remove, find, print, and cleanup.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set.h"
#include "tree.h"

/* Struct Definition */
struct set {
    struct tree *tree;
    int turbo;
};

/* Initialize a new set.
 * 
 * Returns:
 * A pointer to the newly initialized set, or NULL on failure.
 */
struct set *set_init(int turbo) {
    struct set *new_set = (struct set *)malloc(sizeof(struct set));
    if (!new_set) {
        return NULL;
    }
    new_set->tree = tree_init(turbo);
    new_set->turbo = turbo;
    return new_set;
}

/* Insert a value into the set.
 * 
 * Returns:
 * 0 if the insertion was successful, 1 if the value is a duplicate, -1 on failure.
 */
int set_insert(struct set *s, int num) {
    if (!s) return -1;
    return tree_insert(s->tree, num);
}

/* Find a value in the set.
 * 
 * Returns:
 * 1 if the value is found, 0 otherwise, -1 if the set is NULL.
 */
int set_find(struct set *s, int num) {
    if (!s) return -1;
    return tree_find(s->tree, num);
}

/* Remove a value from the set.
 * 
 * Returns:
 * 0 if the value is successfully removed, 1 if the value is not found, -1 if the set is NULL.
 */
int set_remove(struct set *s, int num) {
    if (!s) return -1;
    return tree_remove(s->tree, num);
}

/* Free all memory associated with the set.
 * 
 * s: A pointer to the set.
 */
void set_cleanup(struct set *s) {
    if (!s) return;
    tree_cleanup(s->tree);
    free(s);
}

/* Print all values in the set in sorted order.
 * Each value is printed on a new line.
 * 
 * s: A pointer to the set.
 */
void set_print(const struct set *s) {
    if (!s || !s->tree) return;
    tree_print(s->tree);
}

int set_verify(const struct set *s) {
    if (!s || !s->tree) return -1;
    return tree_check(s->tree);
}
