#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

/* Struct Definitions */
struct tree {
    struct node *root;
    int turbo; // 0 = BST, 1 = AVL
};

struct node {
    int data;
    struct node *lhs;
    struct node *rhs;
    int height;
};

typedef struct node node;

/* Unique IDs for graphviz dot format */
static int global_node_counter = 0;

/* Helper Function: Create a new node */
static node *make_node(int data) {
    node *new_node = (node *)malloc(sizeof(node));
    if (!new_node) {
        return NULL;
    }
    new_node->data = data;
    new_node->lhs = NULL;
    new_node->rhs = NULL;
    new_node->height = 1; // Default height for a new node
    return new_node;
}

/* Utility Functions */
static int height(node *n) {
    return n ? n->height : 0;
}

static void update_height(node *n) {
    if (n) {
        n->height = 1 + (height(n->lhs) > height(n->rhs) ? height(n->lhs) : height(n->rhs));
    }
}

static int balance_factor(node *n) {
    return n ? height(n->lhs) - height(n->rhs) : 0;
}

/* Tree Rotations */
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

/* Insert Node (BST and AVL) */
static node *insert_node(node *n, int data, int *status, int turbo) {
    if (!n) {
        *status = 0; // Success
        return make_node(data);
    }

    if (data < n->data) {
        n->lhs = insert_node(n->lhs, data, status, turbo);
    } else if (data > n->data) {
        n->rhs = insert_node(n->rhs, data, status, turbo);
    } else {
        *status = 1; // Duplicate
        return n;
    }

    if (turbo) {
        update_height(n);
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
    }

    return n;
}

int tree_insert(struct tree *tree, int data) {
    if (!tree) return -1;
    int status = 0; // 0 = success, 1 = duplicate
    tree->root = insert_node(tree->root, data, &status, tree->turbo);
    return status;
}

/* Find Node */
int tree_find(struct tree *tree, int data) {
    if (!tree) return 0;
    node *current = tree->root;
    while (current) {
        if (data == current->data) return 1;
        current = (data < current->data) ? current->lhs : current->rhs;
    }
    return 0;
}

/* Remove Node (BST and AVL) */
static node *remove_node(node *n, int data, int turbo) {
    if (!n) return NULL;

    if (data < n->data) {
        n->lhs = remove_node(n->lhs, data, turbo);
    } else if (data > n->data) {
        n->rhs = remove_node(n->rhs, data, turbo);
    } else {
        if (!n->lhs || !n->rhs) {
            node *temp = n->lhs ? n->lhs : n->rhs;
            free(n);
            return temp;
        }

        node *temp = n->rhs;
        while (temp->lhs) temp = temp->lhs;
        n->data = temp->data;
        n->rhs = remove_node(n->rhs, temp->data, turbo);
    }

    if (turbo) {
        update_height(n);
        int bf = balance_factor(n);
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
    }

    return n;
}

int tree_remove(struct tree *tree, int data) {
    if (!tree_find(tree, data)) return 1;
    tree->root = remove_node(tree->root, data, tree->turbo);
    return 0;
}


/* Print Tree (In-Order Traversal) */
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

/* Free Tree */
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

/* Initialize Tree */
struct tree *tree_init(int turbo) {
    struct tree *new_tree = (struct tree *)malloc(sizeof(struct tree));
    if (!new_tree) return NULL;
    new_tree->root = NULL;
    new_tree->turbo = turbo;
    return new_tree;
}

/* Debugging: Generate Graphviz Dot File */
static int print_tree_dot_r(node *root, FILE *dotf) {
    int left_id, right_id, my_id = global_node_counter++;

    if (!root) {
        fprintf(dotf, "    %d [shape=point];\n", my_id);
        return my_id;
    }

    fprintf(dotf, "    %d [label=\"%d\"];\n", my_id, root->data);

    left_id = print_tree_dot_r(root->lhs, dotf);
    fprintf(dotf, "    %d -> %d [label=\"l\"];\n", my_id, left_id);

    right_id = print_tree_dot_r(root->rhs, dotf);
    fprintf(dotf, "    %d -> %d [label=\"r\"];\n", my_id, right_id);

    return my_id;
}

void tree_dot(const struct tree *tree, const char *filename) {
    if (!tree) return;
    FILE *dotf = fopen(filename, "w");
    if (!dotf) {
        perror("Error opening file for dot output");
        return;
    }
    fprintf(dotf, "digraph BST {\n");
    global_node_counter = 0;
    print_tree_dot_r(tree->root, dotf);
    fprintf(dotf, "}\n");
    fclose(dotf);
}

int tree_check(const struct tree *tree) {
    /* Optional: Implement tree checks for debugging */
    return 0;
}
