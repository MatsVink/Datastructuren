/* Name: Mats Vink
 * UvAnetID: 15874648
 * Program: BSc Informatics
 * 
 * Description:
 * This file implements the main program for interacting with a Set Abstract Data Type (ADT).
 * The program reads commands from the user to perform operations on a set, including insert, 
 * remove, find, and print. It uses a binary search tree (BST) or an AVL tree (if turbo mode is enabled).
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set.h"

#define BUF_SIZE 256

void cleanup_and_fail(char *buf, struct set *s) {
    if (s) {
        set_cleanup(s);
    }
    free(buf);
    exit(EXIT_FAILURE);
}

/* Main function to process user commands.
 * 
 * Returns:
 * EXIT_SUCCESS on successful execution, or EXIT_FAILURE on failure.
 */
int main(void) {
    char *buf = malloc(BUF_SIZE);
    if (!buf) {
        perror("Could not allocate input buffer");
        return EXIT_FAILURE;
    }

    /* Initialize the set with turbo mode off (0 = regular BST). */
    struct set *s = set_init(0);
    if (!s) {
        perror("Failed to initialize set");
        free(buf);
        return EXIT_FAILURE;
    }

    while (fgets(buf, BUF_SIZE, stdin)) {
        char *endptr;
        char *command;
        char *num_str;
        int num = 0;

        /* Parse the command */
        command = strtok(buf, " ");\
        if (!command) {
            printf("Invalid input. Expected a command (+, -, ?, p).\n");
            continue;\
        }

        /* Parse the operand if needed */
        if (strchr("+-?", *command)) { \
            num_str = strtok(NULL, "\n");
            if (!num_str) {
                printf("Set operation '%c' requires an integer operand\n", *command);
                cleanup_and_fail(buf, s);
            }
            num = (int)strtol(num_str, &endptr, 10);
            if (*endptr != '\0') {
                printf("Error converting '%s' to an integer\n", num_str);
                cleanup_and_fail(buf, s);
            }
        }

        /* Handle commands */
        switch (*command) {
            case '+':
                set_insert(s, num);
                break;

            case '-': /* Remove */
                set_remove(s, num);
                break;

            case '?':
                if (set_find(s, num)) {
                    printf("found: %d\n", num);
                } else {
                    printf("not found: %d\n", num);
                }
                break;

            case 'p':
                set_print(s);
                break;

            default:
                printf("Unknown command '%c'. Valid commands are +, -, ?, p.\n", *command);
                break;
        }
    }

    if (set_verify(s)) {
        fprintf(stderr, "Set implementation failed verification!\n");
    }

    /* Cleanup */
    free(buf);
    set_cleanup(s);

    return EXIT_SUCCESS;
}
