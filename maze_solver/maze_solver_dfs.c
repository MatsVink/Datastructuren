/* Name: Mats Vink
 * UvAnetID: 15874648
 * Program: BSc Informatics
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "maze.h"
#include "stack.h"

#define NOT_FOUND -1
#define ERROR -2

/* Solves the maze using a depth-first search (DFS) algorithm.
 * m: Pointer to the maze structure
 * Returns: The length of the path if a path is found, NOT_FOUND if no path exists,
 *          and ERROR if an error occurs.
 */
int dfs_solve(struct maze *m) {
    struct stack *s = stack_init(100000);
    if (!s) {
        return ERROR;
    }

    int row, col;
    maze_start(m, &row, &col);
    int start_index = maze_index(m, row, col);
    stack_push(s, start_index);
    maze_set(m, row, col, VISITED);
    int length = 0;

    int size = maze_size(m) * maze_size(m);
    int *predecessor = malloc((size_t)size * sizeof(int));
    if (!predecessor) {
        stack_cleanup(s);
        return ERROR;
    }

    while (!stack_empty(s)) {
        int current = stack_pop(s);
        row = maze_row(m, current);
        col = maze_col(m, current);

        if (maze_at_destination(m, row, col)) {
            // Mark the path from destination back to the start
            int path_index = current;
            while (path_index != start_index) {
                maze_set(m, maze_row(m, path_index), maze_col(m, path_index), PATH);
                path_index = predecessor[path_index];
                length++;
            }
            free(predecessor);
            stack_cleanup(s);
            return length;
        }

        maze_set(m, row, col, VISITED);

        for (int i = 0; i < N_MOVES; i++) {
            int new_row = row + m_offsets[i][0];
            int new_col = col + m_offsets[i][1];
            int neighbor_index = maze_index(m, new_row, new_col);

            if (maze_valid_move(m, new_row, new_col) && maze_get(m, new_row, new_col) == FLOOR) {
                stack_push(s, neighbor_index);
                maze_set(m, new_row, new_col, TO_VISIT);
                predecessor[neighbor_index] = current;
            }
        }
    }
    free(predecessor);
    stack_cleanup(s);
    return NOT_FOUND;
}

int main(void) {
    /* read maze */
    struct maze *m = maze_read();
    if (!m) {
        printf("Error reading maze\n");
        return 1;
    }

    /* solve maze */
    int path_length = dfs_solve(m);
    if (path_length == ERROR) {
        printf("dfs failed\n");
        maze_cleanup(m);
        return 1;
    } else if (path_length == NOT_FOUND) {
        printf("no path found from start to destination\n");
        maze_cleanup(m);
        return 1;
    }
    printf("dfs found a path of length: %d\n", path_length);

    /* print maze */
    maze_print(m, false);
    maze_output_ppm(m, "out.ppm");

    maze_cleanup(m);
    return 0;
}
