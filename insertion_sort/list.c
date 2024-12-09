/* Name: Mats Vink
 * UvAnetID: 15874648
 * Program: BSc Informatics
 * 
 * Description:
 * This file implements a singly linked list. 
 * It provides functions for creating, modifying, and managing linked 
 * lists and their nodes. The list supports basic operations such as 
 * insertion and deletion.
 */

#include "list.h"
#include <stdio.h>
#include <stdlib.h>

/* List node structure declaration. */
struct node {
    int value;
    struct node *next;
};

/* Linked list data structure declaration. */
struct list {
    struct node *head;
    struct node *tail;
    size_t length;
};

/* Creates a new linked list and returns a pointer to it.
 * Returns NULL on failure. */
struct list *list_init(void) {
    struct list *l = malloc(sizeof(struct list));
    if (l == NULL) {
        return NULL;
    }
    l->head = NULL;
    l->tail = NULL;
    l->length = 0;
    return l;
}

/* Creates a new node that contains the number num and returns a pointer to
 * it.  Returns NULL on failure. */
struct node *list_new_node(int num) {
    struct node *n = malloc(sizeof(struct node));
    if (n == NULL) {
        return NULL;
    }
    n->value = num;
    n->next = NULL;
    return n;
}

/* Returns a pointer to the first node of the list L or NULL if list is empty. */
struct node *list_head(const struct list *l) {
    return (l == NULL) ? NULL : l->head;
}

/* Returns a pointer to the node after node N.
 * Return NULL if N is NULL or if N is the last node in the list. */
struct node *list_next(const struct node *n) {
    if (n == NULL || n->next == NULL) {
        return NULL;
    }
    return n->next;
}

/* Inserts node N at the front of list L.
 * Returns 0 if N was successfully inserted, 1 otherwise. */
int list_add_front(struct list *l, struct node *n) {
    if (l == NULL || n == NULL) {
        return 1;
    }
    n->next = l->head;
    l->head = n;
    if (l->tail == NULL) {
        l->tail = n; // Lijst was eerst leeg
    }
    l->length++;
    return 0;
}

/* Returns a pointer to the last node of the list L or NULL if list is empty. */
struct node *list_tail(const struct list *l) {
    return (l == NULL) ? NULL : l->tail;
}

/* Returns a pointer to the node before node N in the list L, or returns NULL
 * if N is the first node in the list or if N is not in the list at all. */
struct node *list_prev(const struct list *l, const struct node *n) {
    if (l == NULL || n == NULL || l->head == n) {
        return NULL;
    }
    struct node *current = l->head;
    while (current != NULL && current->next != n) {
        current = current->next;
    }
    return current;
}

/* Appends node N at the back of list L.
 * Returns 0 if N was successfully appended, 1 otherwise. */
int list_add_back(struct list *l, struct node *n) {
    if (l == NULL || n == NULL) {
        return 1;
    }
    n->next = NULL;
    if (l->tail != NULL) {
        l->tail->next = n;
    } else {
        l->head = n;
    }
    l->tail = n;
    l->length++;
    return 0;
}

/* Returns the value stored in the list node N. If N is NULL the return
 * value is not defined. */
int list_node_get_value(const struct node *n) {
    return (n == NULL) ? 0 : n->value;
}

/* Set the value of node N to VALUE.
 * Returns 0 if successful, 1 otherwise. */
int list_node_set_value(struct node *n, int value) {
    if (n == NULL) {
        return 1;
    }
    n->value = value;
    return 0;
}

/* Unlink node N from list L. After unlinking, the list L contains no pointers
 * to the node N and N contains no pointers to nodes in L.
 * Returns 0 if N was successfully unlinked from list L, or 1 otherwise */
int list_unlink_node(struct list *l, struct node *n) {
    if (l == NULL || n == NULL) {
        return 1;
    }
    if (l->head == n) {
        l->head = n->next;
        if (l->tail == n) {
            l->tail = NULL;
        }
    } else {
        struct node *prev = list_prev(l, n);
        if (prev == NULL) {
            return 1;
        }
        prev->next = n->next;
        if (l->tail == n) {
            l->tail = prev;
        }
    }
    n->next = NULL;
    l->length--;
    return 0;
}

/* Frees node N. */
void list_free_node(struct node *n) {
    if (n != NULL) {
        free(n);
    }
}

/* Cleans up entire list L data structure.
 * Returns 0 if successful, 1 otherwise. */
int list_cleanup(struct list *l) {
    if (l == NULL) {
        return 1;
    }
    struct node *current = l->head;
    while (current != NULL) {
        struct node *temp = current;
        current = current->next;
        list_free_node(temp);
    }
    free(l);
    return 0;
}

/* Returns 1 if node N is present in list L and 0 if N is not present
 * in L. Returns -1 if an error occured. */
int list_node_present(const struct list *l, const struct node *n) {
    if (l == NULL || n == NULL) {
        return -1;
    }
    struct node *current = l->head;
    while (current != NULL) {
        if (current == n) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

/* Inserts node N after node M in list L.
 * Fails if node M is not in the list L or if node N is already in list L.
 * Returns 0 if N was successfully inserted, or 1 otherwise. */
int list_insert_after(struct list *l, struct node *n, struct node *m) {
    if (l == NULL || n == NULL || m == NULL || list_node_present(l, m) == 0 || list_node_present(l, n) == 1) {
        return 1;
    }
    n->next = m->next;
    m->next = n;
    if (l->tail == m) {
        l->tail = n;
    }
    l->length++;
    return 0;
}

/* Inserts node N before node M in list L.
 * Fails if node M is not in the list L or if node N is already in list L.
 * Returns 0 if N was successfully inserted, or 1 otherwise. */
int list_insert_before(struct list *l, struct node *n, struct node *m) {
    if (l == NULL || n == NULL || m == NULL || list_node_present(l, m) == 0) {
        return 1;
    }
    struct node *prev = list_prev(l, m);
    if (prev == NULL) {
        return list_add_front(l, n);
    } else {
        prev->next = n;
        n->next = m;
        l->length++;
        return 0;
    }
}
/* Returns the length of list L, or 0 if L is the NULL pointer */
size_t list_length(const struct list *l) {
    return (l == NULL) ? 0 : l->length;
}

/* Returns a pointer to the i^th node of list L or NULL if there is no i^th
 * element in list L. */
struct node *list_get_ith(const struct list *l, size_t i) {
    if (l == NULL || i >= l->length) {
        return NULL;
    }
    struct node *current = l->head;
    for (size_t j = 0; j < i; j++) {
        current = current->next;
    }
    return current;
}

/* Cuts list L into 2 lists, with node N being the last node in the first half
 * and all nodes after nodes N are part to the second half, in the same
 * order they were in in the original list.  Modifies list L to only contain
 * the first half and creates a new list for the second half. Returns a
 * pointer to the second half if successfully cut and NULL otherwise. */
struct list *list_cut_after(struct list *l, struct node *n) {
    if (l == NULL || n == NULL || list_node_present(l, n) == 0) {
        return NULL;
    }
    struct list *new_list = list_init();
    if (new_list == NULL) {
        return NULL;
    }
    new_list->head = n->next;
    new_list->tail = l->tail;
    new_list->length = l->length - 1;
    
    l->tail = n;
    n->next = NULL;
    l->length = 1;
    return new_list;
}
