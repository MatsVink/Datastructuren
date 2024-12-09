/* Name: Mats Vink
 * UvAnetID: 15874648
 * Program: BSc Informatics
 *
 * Description:
 * This program manages a priority queue of patients for a doctor's office. Patients can be sorted by name
 * or age depending on the provided command-line arguments. Additionally, each patient has a treatment duration,
 * during which no new patients can be treated. At the end of the day, all remaining patients are removed.
 */

#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "prioq.h"

#define BUF_SIZE 1024

static char buf[BUF_SIZE];

/* Struct to store configuration options */
struct config {
    int year; // If set to 1, patients are sorted by age; otherwise, by name
};

/* Struct to represent a patient */
typedef struct {
    char *name;
    int age;
    int duration;
    int remaining_time;
} patient_t;

/* Function decleration */
static int parse_options(struct config *cfg, int argc, char *argv[]);
static patient_t *create_patient(char *input);
static void process_patient(patient_t **current_patient);
static void finalize_day(prioq *queue, patient_t *current_patient);

/* 
 * Compare patients by name for priority queue.
 * Returns a negative, zero, or positive value depending on lexicographical order.
 */
static int compare_patient_name(const void *a, const void *b) {
    return strcmp(((const patient_t *)a)->name, ((const patient_t *)b)->name);
}

/* 
 * Compare patients by age for priority queue. If ages are equal, compare by name.
 * Returns a negative, zero, or positive value depending on order.
 */
static int compare_patient_age(const void *a, const void *b) {
    const patient_t *pa = (const patient_t *)a;
    const patient_t *pb = (const patient_t *)b;

    if (pa->age != pb->age) {
        return pa->age - pb->age;
    }
    return strcmp(pa->name, pb->name);
}

/* 
 * Parses options, initializes the queue, and processes 
 * patients until the end of the day.
 */
int main(int argc, char *argv[]) {
    prioq *queue;
    struct config cfg;
    patient_t *current_patient = NULL;

    if (parse_options(&cfg, argc, argv) != 0) {
        return EXIT_FAILURE;
    }

    // Initialize queue with the comparison function
    queue = cfg.year ? prioq_init(&compare_patient_age) : prioq_init(&compare_patient_name);

    for (int iterations = 0;;) {
        // Read and process patients until "." is encountered
        while (1) {
            char *s = fgets(buf, BUF_SIZE, stdin);
            if (s == NULL) {
                fprintf(stderr, "Unexpected end of file. Exiting.\n");
                prioq_cleanup(queue, free);
                return EXIT_FAILURE;
            }

            if (strcmp(buf, ".\n") == 0) break;

            patient_t *new_patient = create_patient(buf);
            if (!new_patient) continue;

            if (prioq_insert(queue, new_patient) != 0) {
                fprintf(stderr, "Failed to insert patient into queue.\n");
                free(new_patient->name);
                free(new_patient);
            }
        }

        // Assign a new patient if the doctor is free
        if (!current_patient && prioq_size(queue) > 0) {
            current_patient = prioq_pop(queue);
        }

        // Process the current patient
        process_patient(&current_patient);

        // Print time marker
        printf(".\n");

        // Check if the end of the day has been reached
        if (++iterations == 10) {
            finalize_day(queue, current_patient);
            break;
        }
    }

    prioq_cleanup(queue, free);
    return EXIT_SUCCESS;
}

static int parse_options(struct config *cfg, int argc, char *argv[]) {
    memset(cfg, 0, sizeof(struct config)); // Initialize config with zeros
    int c;
    while ((c = getopt(argc, argv, "y")) != -1) {
        switch (c) {
        case 'y':
            cfg->year = 1; // Sort by age if -y is specified
            break;
        default:
            fprintf(stderr, "Invalid option: -%c\n", optopt);
            return 1;
        }
    }
    return 0;
}

/* 
 * Creates a new patient based on input data.
 * input: Input string containing patient details.
 * Returns a pointer to a newly allocated patient_t struct, or NULL on error.
 */
static patient_t *create_patient(char *input) {
    char *token = strtok(input, " ");
    if (!token) return NULL;

    size_t name_len = strlen(token);
    char *name_cpy = malloc(name_len + 1);
    if (!name_cpy) {
        perror("Failed to allocate memory for name");
        return NULL;
    }
    strcpy(name_cpy, token);

    patient_t *patient = malloc(sizeof(patient_t));
    if (!patient) {
        perror("Failed to allocate memory for patient");
        free(name_cpy);
        return NULL;
    }

    patient->name = name_cpy;

    token = strtok(NULL, " ");
    if (!token) {
        fprintf(stderr, "Invalid input format\n");
        free(patient->name);
        free(patient);
        return NULL;
    }
    patient->age = atoi(token);

    token = strtok(NULL, " ");
    patient->duration = token ? atoi(token) : 1;
    patient->remaining_time = patient->duration;

    return patient;
}

/* 
 * Processes the current patient, reducing their remaining time.
 * current_patient: Pointer to the pointer of the current patient.
 * Frees the patient if their treatment is complete.
 */
static void process_patient(patient_t **current_patient) {
    if (*current_patient) {
        (*current_patient)->remaining_time--;

        if ((*current_patient)->remaining_time == 0) {
            printf("%s\n", (*current_patient)->name);
            free((*current_patient)->name);
            free(*current_patient);
            *current_patient = NULL;
        }
    }
}

/* 
 * Finalizes the day by removing and printing all remaining patients.
 * queue: Priority queue of patients.
 * current_patient: The patient currently being treated.
 */
static void finalize_day(prioq *queue, patient_t *current_patient) {
    if (current_patient) {
        printf("%s\n", current_patient->name);
        free(current_patient->name);
        free(current_patient);
    }

    long size = prioq_size(queue);
    for (long i = 0; i < size; i++) {
        patient_t *patient = prioq_pop(queue);
        if (patient) {
            printf("%s\n", patient->name);
            free(patient->name);
            free(patient);
        }
    }
}
