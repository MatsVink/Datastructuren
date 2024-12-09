/* Name: Mats Vink
 * UvAnetID: 15874648
 * Program: BSc Informatics
 * 
 * Description:
 * This program implements a sorting application using a linked list. The program reads numbers 
 * from standard input, sorts them using insertion sort, and supports several command-line options:
 * -d: Sort in descending order.
 * -c: Combine every two consecutive numbers.
 * -o: Remove odd numbers.
 * -z: Zip the list by alternating two halves.
 */

#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#define BUF_SIZE 1024

char buf[BUF_SIZE];

struct config {
    /* You can ignore these options until you implement the
       extra command-line arguments. */

    /* Set to 1 if -d is specified, 0 otherwise. */
    int descending_order;

    // Set to 1 if -c is specified, 0 otherwise.
    int combine;

    // Set to 1 if -o is specified, 0 otherwise.
    int remove_odd;

    /* Set to 1 if -z is specified, 0 otherwise. */
    int zip_alternating;
};

int parse_options(struct config *cfg, int argc, char *argv[]) {
    memset(cfg, 0, sizeof(struct config));
    int c;
    while ((c = getopt(argc, argv, "dcoz")) != -1) {
        switch (c) {
        case 'd':
            cfg->descending_order = 1;
            break;
        case 'c':
            cfg->combine = 1;
            break;
        case 'o':
            cfg->remove_odd = 1;
            break;
        case 'z':
            cfg->zip_alternating = 1;
            break;
        default:
            fprintf(stderr, "invalid option: -%c\n", optopt);
            return 1;
        }
    }
    return 0;
}

/* Sorts a linked list using insertion sort.
 * 
 * list: Pointer to the list to be sorted.
 */
void insertionsort_list(struct list *list) {
    if (list == NULL || list_head(list) == NULL) return;

    struct list *sorted_list = list_init();
    struct node *current = list_head(list);

    while (current != NULL) {
        struct node *next = list_next(current);
        list_unlink_node(list, current);

        struct node *sorted_curr = list_head(sorted_list);
        struct node *prev = NULL;

        while (sorted_curr != NULL && list_node_get_value(sorted_curr) < list_node_get_value(current)) {
            prev = sorted_curr;
            sorted_curr = list_next(sorted_curr);
        }
        if (prev == NULL) {
            list_add_front(sorted_list, current);
        } else {
            list_insert_after(sorted_list, current, prev);
        }
        current = next;
    }
    //copy the sorted list to the list
    current = list_head(sorted_list);
    while (current != NULL) {
        struct node *next = list_next(current);
        list_add_back(list, list_new_node(list_node_get_value(current)));
        list_unlink_node(sorted_list, current);
        list_free_node(current);
        current = next;
    }
    list_cleanup(sorted_list);
}

/* Reverses the order of a linked list.
 * 
 * lst: Pointer to the list to be reversed.
 */
void reverse_list(struct list *lst) {
    struct node *current = list_head(lst);
    while (current != NULL) {
        struct node *next = list_next(current);
        list_unlink_node(lst, current);
        list_add_front(lst, current);
        current = next;
    }
}

/* Combines every two consecutive numbers in a linked list.
 * 
 * lst: Pointer to the list.
 */
void combine_pairs(struct list *lst) {
    struct node *current = list_head(lst);
    while (current != NULL && list_next(current) != NULL) {
        struct node *next = list_next(current);
        int combined_value = list_node_get_value(current) + list_node_get_value(next);
        list_node_set_value(current, combined_value);
        list_unlink_node(lst, next);
        list_free_node(next);
        current = list_next(current);
    }
}

/* Removes all odd numbers from a linked list.
 * 
 * lst: Pointer to the list.
 */
void remove_odd_numbers(struct list *lst) {
    struct node *current = list_head(lst);
    while (current != NULL) {
        struct node *next = list_next(current);
        if (list_node_get_value(current) % 2 != 0) {
            list_unlink_node(lst, current);
            list_free_node(current);
        }
        current = next;
    }
}

/* Zips the list by alternating elements from two halves.
 * 
 * lst: Pointer to the list.
 */
void zip_alternating(struct list *lst) {
    size_t len = list_length(lst);
    if (len < 2) return;

    // split in half
    struct node *middle = list_get_ith(lst, (len + 1) / 2 - 1);
    struct list *second_half = list_cut_after(lst, middle);

    struct node *first = list_head(lst);
    struct node *second = list_head(second_half);

    // switch the nodes 1 by 1
    while (second != NULL) {
        struct node *next_first = list_next(first);
        struct node *next_second = list_next(second);

        list_unlink_node(second_half, second);
        list_insert_after(lst, second, first);

        first = next_first;
        second = next_second;
    }
    list_cleanup(second_half);
}

int main(int argc, char *argv[]) {
    struct config cfg;
    if (parse_options(&cfg, argc, argv) != 0) {
        return 1;
    }
    
    struct list *list = list_init();
    while (fgets(buf, BUF_SIZE, stdin)) {
        char *token = strtok(buf, " \t\n");
        while (token != NULL) {
            char *endptr;
            long val = strtol(token, &endptr, 10);
            if (*endptr == '\0') {
                int num = (int)val;
                struct node *new_node = list_new_node(num);
                if (list_add_back(list, new_node) != 0) {
                    fprintf(stderr, "Failed to add node\n");
                    list_cleanup(list);
                    return 1;
                }
            } else {
                fprintf(stderr, "Invalid input: %s\n", token);
            }
            token = strtok(NULL, " \t\n");
        }
    }

    insertionsort_list(list);
    if (cfg.descending_order) {
    reverse_list(list);
    }
    if (cfg.combine) {
    combine_pairs(list);
    }
    if (cfg.remove_odd) {
    remove_odd_numbers(list);
    }
    if (cfg.zip_alternating) {
    zip_alternating(list);
    }
    // Print list
    struct node *current = list_head(list);
    while (current) {
        printf("%d\n", list_node_get_value(current));
        current = list_next(current);
    }
    list_cleanup(list);
    return 0;
}
