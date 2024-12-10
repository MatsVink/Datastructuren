/* Name: Mats Vink
 * UvAnetID: 15874648
 * Program: BSc Informatics
 * 
 * Description:
 * This file implements a binary search tree (BST) with optional AVL balancing. 
 * It provides functions for inserting, finding, and removing elements, as well 
 * as for printing and cleaning up the tree.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

struct tree {
    struct node *root;
    int turbo; // 0 = Regular BST, 1 = AVL
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

// Helper functions:

/* Create a new node with the given data.
 * Returns a pointer to the newly created node, or NULL on failure.
 */
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

//Get the height of a node.
static int height(node *n) {
    return n ? n->height : 0;
}

//Update the height of a node based on its children.
static void update_height(node *n) {
    if (n) {
        n->height = 1 + (height(n->lhs) > height(n->rhs) ? height(n->lhs) : height(n->rhs));
    }
}

//Compute the balance factor of a node.
static int balance_factor(node *n) {
    return n ? height(n->lhs) - height(n->rhs) : 0;
}

//Perform a right rotation on the subtree rooted at y.
static node *rotate_right(node *y) {
    node *x = y->lhs;
    node *T2 = x->rhs;

    x->rhs = y;
    y->lhs = T2;

    update_height(y);
    update_height(x);

    return x;
}

//Perform a left rotation on the subtree rooted at x.
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

/* Initialize a new tree.
 * 
 * turbo: Whether to use AVL balancing (1) or not (0).
 * 
 * Returns:
 * A pointer to the newly initialized tree, or NULL on failure.
 */
struct tree *tree_init(int turbo) {
    struct tree *new_tree = (struct tree *)malloc(sizeof(struct tree));
    if (!new_tree) {
        return NULL;
    }
    new_tree->root = NULL;
    new_tree->turbo = turbo;
    return new_tree;
}

/* Insert Node (BST and AVL) */

/* Insert a new value into the subtree rooted at n.
 * 
 * Returns:
 * The new root of the subtree after insertion.
 */
static node *insert_node(node *n, int data, int turbo, int *status) {
    if (!n) {
        *status = 0;
        return make_node(data);
    }

    if (data < n->data) {
        n->lhs = insert_node(n->lhs, data, turbo, status);
    } else if (data > n->data) {
        n->rhs = insert_node(n->rhs, data, turbo, status);
    } else {
        *status = 1;
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

/* Insert a value into the tree.
 * 
 * tree: A pointer to the tree structure.
 * data: The value to insert.
 * 
 * Returns:
 * 0 if the insertion was successful, 1 if the value is a duplicate, -1 on failure.
 */
int tree_insert(struct tree *tree, int data) {
    if (!tree) return -1;

    int status = 0;
    tree->root = insert_node(tree->root, data, tree->turbo, &status);
    return status;
}

/* Find a value in the tree.
 * 
 * tree: A pointer to the tree structure.
 * data: The value to search for.
 * 
 * Returns:
 * 1 if the value is found, 0 otherwise.
 */
int tree_find(struct tree *tree, int data) {
    if (!tree) return 0;

    node *current = tree->root;
    while (current) {
        if (data == current->data) {
            return 1;
        }
        current = (data < current->data) ? current->lhs : current->rhs;
    }
    return 0;
}

/* Remove a value from the subtree rooted at n.
 * 
 * Returns:
 * The new root of the subtree after removal.
 */
static node *remove_node(node *n, int data, int turbo) {
    if (!n) return NULL;

    if (data < n->data) {
        n->lhs = remove_node(n->lhs, data, turbo);
    } else if (data > n->data) {
        n->rhs = remove_node(n->rhs, data, turbo);
    } else {
        if (!n->lhs || !n->rhs) {
            /* Case 1: No child or one child */
            node *temp = n->lhs ? n->lhs : n->rhs;
            free(n);
            return temp;
        }

        /* Case 2: Two children */
        node *temp = n->rhs;
        while (temp->lhs) {
            temp = temp->lhs;
        }
        n->data = temp->data;
        n->rhs = remove_node(n->rhs, temp->data, turbo);
    }

    if (turbo) {
        /* Balance the tree if AVL mode is enabled */
        update_height(n);
        int bf = balance_factor(n);

        if (bf > 1 && balance_factor(n->lhs) >= 0) {
            return rotate_right(n);
        }
        if (bf > 1 && balance_factor(n->lhs) < 0) {
            n->lhs = rotate_left(n->lhs);
            return rotate_right(n);
        }
        if (bf < -1 && balance_factor(n->rhs) <= 0) {
            return rotate_left(n);
        }
        if (bf < -1 && balance_factor(n->rhs) > 0) {
            n->rhs = rotate_right(n->rhs);
            return rotate_left(n);
        }
    }

    return n;
}

/* Remove a value from the tree.
 * 
 * tree: A pointer to the tree structure.
 * data: The value to remove.
 * 
 * Returns:
 * 0 if the value is successfully removed, 1 if the value is not found.
 */
int tree_remove(struct tree *tree, int data) {
    if (!tree) return 1;

    if (!tree_find(tree, data)) {
        return 1;
    }

    tree->root = remove_node(tree->root, data, tree->turbo);
    return 0;
}

/* Perform an in-order traversal of the subtree rooted at n.
 * 
 * n: A pointer to the root of the subtree.
 */
static void print_in_order(node *n) {
    if (!n) return;
    print_in_order(n->lhs);
    printf("%d\n", n->data);
    print_in_order(n->rhs);
}

/* Print the contents of the tree in sorted order.
 * Each value is printed on a new line.
 * 
 * tree: A pointer to the tree structure.
 */
void tree_print(const struct tree *tree) {
    if (!tree) return;
    print_in_order(tree->root);
}

/* Recursively free all nodes in the subtree rooted at n.
 * 
 * n: A pointer to the root of the subtree.
 */
static void free_tree(node *n) {
    if (!n) return;
    free_tree(n->lhs);
    free_tree(n->rhs);
    free(n);
}
/* Free all memory associated with the tree.
 * 
 * tree: A pointer to the tree structure.
 */
void tree_cleanup(struct tree *tree) {
    if (!tree) return;
    free_tree(tree->root);
    free(tree);
}
