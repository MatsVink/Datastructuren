
int main(int argc, char *argv[]) {
    struct config cfg;
    if (parse_options(&cfg, argc, argv) != 0) {
        return 1;
    }

    // Roep mysort aan met de opgegeven configuratie
    mysort(cfg);
    return 0;
}

// sorts an array a of size n, using insertion sort
void insertionsort(int *a, int n) {
    for (int i=1; i<n; i++) {
        percolate(a, i);
    }       
}
// moves item at index k down to its
// correct position in array a
void percolate(int *a, int i) {
    int item = a[i];
    int j = i;
    while (j>0 && a[j-1] > item) {
        a[j] = a[j-1];
        j--;
    }
    a[j] = item;
}

#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define BUF_SIZE 1024
char buf[BUF_SIZE];

// Voeg een nummer toe in de juiste positie om een gesorteerde lijst te behouden
int list_insert_sorted(struct list *l, struct node *n) {
     if (l == NULL || n == NULL) return 1;

    // Als lijst leeg is of de eerste node groter is (bij oplopende volgorde), voeg dan vooraan in
    if (l->head == NULL || l->head->value) {
        return list_add_front(l, n);
    }

    struct node *current = l->head;
    struct node *previous = NULL;

    // Zoek de positie om het nieuwe element in te voegen
    while (current && current->value < n->value) {
        previous = current;
        current = current->next;
    }

    // Invoegen aan het begin
    if (previous == NULL) {
        n->next = l->head;
        l->head = n;
    } else {
        // Invoegen in het midden of einde
        previous->next = n;
        n->next = current;
    }

    // Update de tail indien nodig
    if (n->next == NULL) {
        l->tail = n;
    }
    l->length++;
    return 0;
}
// Voeg node N in gesorteerde volgorde in in list L.
// Retourneert 0 bij succes en 1 bij een fout.
int list_insert_sorted(struct list *l, struct node *n, int descending_order) {
    if (l == NULL || n == NULL) return 1;

    // Als lijst leeg is of de eerste node groter is (bij oplopende volgorde), voeg dan vooraan in
    if (l->head == NULL || (descending_order ? n->value > l->head->value : n->value < l->head->value)) {
        return list_add_front(l, n);
    }

    // Zoek de juiste positie voor de nieuwe node
    struct node *current = l->head;
    while (current->next != NULL && 
           (descending_order ? current->next->value > n->value : current->next->value < n->value)) {
        current = current->next;
    }

    // Voeg de node in na de juiste gevonden positie
    n->next = current->next;
    current->next = n;
    if (current == l->tail) {
        l->tail = n;
    }
    l->length++;
    return 0;
}

// mysort functie die taken uitvoert
void mysort(struct config cfg) {
    struct list *lst = list_init();  // Initialiseert een lege linked list
    if (!lst) {
        fprintf(stderr, "Failed to initialize list\n");
        exit(EXIT_FAILURE);
    }

    // Lees invoer van stdin, split en voeg elke waarde gesorteerd in
    while (fgets(buf, BUF_SIZE, stdin)) {
        char *token = strtok(buf, " \t\n");
        while (token != NULL) {
            int num = atoi(token);
            struct node *new_node = list_new_node(num);
            if (!new_node || list_insert_sorted(lst, new_node)) {
                fprintf(stderr, "Failed to add node\n");
                list_cleanup(lst);
                exit(EXIT_FAILURE);
            }
            token = strtok(NULL, " \t\n");
        }
    }

    // Pas configuratieopties toe op de lijst
    if (cfg.descending_order) {
        // Voeg logica toe voor aflopende volgorde
    }
    if (cfg.combine) {
        // Voeg logica toe voor combineren van paren
    }
    if (cfg.remove_odd) {
        // Voeg logica toe voor het verwijderen van oneven nummers
    }
    if (cfg.zip_alternating) {
        // Voeg logica toe voor zip alternating
    }

    // Print de gesorteerde lijst
    struct node *current = list_head(lst);
    while (current) {
        printf("%d\n", current->value);
        current = list_next(current);
    }

    list_cleanup(lst);
}

int main(int argc, char *argv[]) {
    struct config cfg;
    if (parse_options(&cfg, argc, argv) != 0) {
        return 1;
    }

    // Roep mysort aan met de configuratie
    mysort(cfg);
    return 0;
}

