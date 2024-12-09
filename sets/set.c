#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set.h"
#include "tree.h"

struct set {
    struct tree *tree;
    int turbo;
};

struct set *set_init(int turbo) {
    struct set *new_set = (struct set *)malloc(sizeof(struct set));;
    if (!new_set) {
        return NULL;
    }
    new_set->tree = tree_init(turbo);
    new_set->turbo = turbo;
    return new_set;
}

/* Insert num into set s. Return 0 for success and -1 for failure.
 * When item is already in the set s, do nothing and return 1. */
int set_insert(struct set *s, int num) {
    if (!s) return -1;
    return tree_insert(s->tree, num);
}

int set_find(struct set *s, int num) {
    /* ... SOME CODE MISSING HERE ... */
}

int set_remove(struct set *s, int num) {
    /* ... SOME CODE MISSING HERE ... */
}

void set_cleanup(struct set *s) {
    /* ... SOME CODE MISSING HERE ... */
}

void set_print(const struct set *s) {
    /* ... SOME CODE MISSING HERE ... */
}

int set_verify(const struct set *s) {
    /* ... OPTIONALLY IMPLEMENT SET CHECKING HERE ... */
    return 0;
}
