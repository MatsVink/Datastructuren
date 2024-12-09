#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

/* C files can be modified anywhere.
 * So you can change or remove these structs to suit your needs. */
struct tree {
    struct node *root;
    int turbo;
};

struct node {
    int data;
    struct node *lhs;
    struct node *rhs;
    int height;
};

typedef struct node node;

/* Unique id's for numbering nodes in dot format. */
static int global_node_counter = 0;

/* Helper function: Allocate a new tree node and initialise it with
 * the given parameters. Return a pointer to the new node or NULL on
 * failure. */
static node *make_node(int data) {
    node *new_node = (node *)malloc(sizeof(node));
    if (!new_node) {
        return NULL;
    }
    new_node->data = data;
    new_node->lhs = NULL;
    new_node->rhs = NULL;
    new_node->height = 1;
    return new_node;
}

static int height(node *n) {
    return n ? n->height : 0;
}

static void update_height(node *n) {
    if (n) {
        n->height = 1 + (height(n->lhs) > height(n->rhs) ? height(n->lhs) : height(n->rhs));
    }
}

static int balance_factor(node *n) {
    return height(n->lhs) - height(n->rhs);
}

static node *rotate_right(node *y) {
    node *x = y->lhs;
    node *T2 = x->rhs;
    x->rhs = y;
    y->lhs = T2;
    update_height(y);
    update_height(x);
    return x;
}

static node *rotate_left(node *x) {
    node *y = x->rhs;
    node *T2 = y->lhs;
    y->lhs = x;
    x->rhs = T2;
    update_height(x);
    update_height(y);
    return y;
}

static int print_tree_dot_r(node *root, FILE *dotf) {
    int left_id, right_id, my_id = global_node_counter++;

    if (root == NULL) {
        fprintf(dotf, "    %d [shape=point];\n", my_id);
        return my_id;
    }

    fprintf(dotf, "    %d [color=%s label=\"%d\"]\n", my_id, "black", root->data);

    left_id = print_tree_dot_r(root->lhs, dotf);
    fprintf(dotf, "    %d -> %d [label=\"l\"]\n", my_id, left_id);

    right_id = print_tree_dot_r(root->rhs, dotf);
    fprintf(dotf, "    %d -> %d [label=\"r\"]\n", my_id, right_id);

    return my_id;
}

void tree_dot(const struct tree *tree, const char *filename) {
    node *root = tree->root;
    global_node_counter = 0;
    FILE *dotf = fopen(filename, "w");
    if (!dotf) {
        printf("error opening file: %s\n", filename);
        exit(1);
    }
    fprintf(dotf, "digraph {\n");
    if (root) {
        print_tree_dot_r(root, dotf);
    }
    fprintf(dotf, "}\n");
    fclose(dotf);
}

int tree_check(const struct tree *tree) {
    /* ... OPTIONALLY IMPLEMENT TREE CHECKING HERE ... */
    return 0;
}

struct tree *tree_init(int turbo) {
    struct tree *new_tree = (struct tree *)malloc(sizeof(struct tree));
    if (!new_tree) {
        return NULL;
    }
    new_tree->root = NULL;
    new_tree->turbo = turbo;
    return new_tree;
}

static node *insert_node(node *n, int data, int *status) {
    if (!n) {
        *status = 0; // Succesvolle invoeging
        return make_node(data);
    }

    if (data < n->data) {
        n->lhs = insert_node(n->lhs, data, status);
    } else if (data > n->data) {
        n->rhs = insert_node(n->rhs, data, status);
    } else {
        *status = 1; // Dubbele waarde gevonden
        return n;    // Geen invoeging nodig
    }

    update_height(n);

    // Balanceren (alleen nodig als AVL wordt ondersteund)
    int bf = balance_factor(n);
    if (bf > 1 && data < n->lhs->data) return rotate_right(n);
    if (bf > 1 && data > n->lhs->data) {
        n->lhs = rotate_left(n->lhs);
        return rotate_right(n);
    }
    if (bf < -1 && data > n->rhs->data) return rotate_left(n);
    if (bf < -1 && data < n->rhs->data) {
        n->rhs = rotate_right(n->rhs);
        return rotate_left(n);
    }

    return n;
}

int tree_insert(struct tree *tree, int data) {
    if (!tree) return -1;

    int status = 0; // 0 = succesvol, 1 = duplicaat
    tree->root = insert_node(tree->root, data, &status);

    return status;
}

int tree_find(struct tree *tree, int data) {
    node *current = tree->root;
    while (current) {
        if (data == current->data) return 1;
        current = (data < current->data) ? current->lhs : current->rhs;
    }
    return 0;
}

static node *remove_node(node *n, int data) {
    if (!n) return NULL;

    if (data < n->data) {
        n->lhs = remove_node(n->lhs, data);
    } else if (data > n->data) {
        n->rhs = remove_node(n->rhs, data);
    } else {
        if (!n->lhs || !n->rhs) {
            node *temp = n->lhs ? n->lhs : n->rhs;
            free(n);
            return temp;
        }

        node *temp = n->rhs;
        while (temp->lhs) temp = temp->lhs;
        n->data = temp->data;
        n->rhs = remove_node(n->rhs, temp->data);
    }

    update_height(n);

    int bf = balance_factor(n);

    // Balance tree
    if (bf > 1 && balance_factor(n->lhs) >= 0) return rotate_right(n);
    if (bf > 1 && balance_factor(n->lhs) < 0) {
        n->lhs = rotate_left(n->lhs);
        return rotate_right(n);
    }

    if (bf < -1 && balance_factor(n->rhs) <= 0) return rotate_left(n);
    if (bf < -1 && balance_factor(n->rhs) > 0) {
        n->rhs = rotate_right(n->rhs);
        return rotate_left(n);
    }

    return n;
}

int tree_remove(struct tree *tree, int data) {
    if (!tree_find(tree, data)) return 1;
    if (!tree) return 1;
    tree->root = remove_node(tree->root, data);
    return 0;
}

static void print_in_order(node *n) {
    if (!n) return;
    print_in_order(n->lhs);
    printf("%d\n", n->data);
    print_in_order(n->rhs);
}

void tree_print(const struct tree *tree) {
    if (!tree) return;
    print_in_order(tree->root);
}

static void free_tree(node *n) {
    if (!n) return;
    free_tree(n->lhs);
    free_tree(n->rhs);
    free(n);
}

void tree_cleanup(struct tree *tree) {
    if (!tree) return;
    free_tree(tree->root);
    free(tree);
}
