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
    struct set *new_set = (struct set *)malloc(sizeof(struct set));
    if (!new_set) {
        return NULL;
    }
    new_set->tree = tree_init(turbo);
    new_set->turbo = turbo;
    return new_set;
}

int set_insert(struct set *s, int num) {
    if (!s) return -1;
    return tree_insert(s->tree, num);
}

int set_find(struct set *s, int num) {
    if (!s) return -1;
    return tree_find(s->tree, num);
}

int set_remove(struct set *s, int num) {
    if (!s) return -1;
    return tree_remove(s->tree, num);
}

void set_cleanup(struct set *s) {
    if (!s) return;
    tree_cleanup(s->tree);
    free(s);
}

void set_print(const struct set *s) {
    if (!s || !s->tree) return;
    tree_print(s->tree);

}

int set_verify(const struct set *s) {
    if (!s || !s->tree) return -1;
    return tree_check(s->tree);
}
