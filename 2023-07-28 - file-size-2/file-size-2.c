#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct node {
    unsigned int key;
    struct node* prev_pointer;
    struct node* next_pointer;

} node_t;

typedef struct list {
    node_t* head;

} list_t;

typedef struct shared_data {
    list_t number_set;

} shared_data_t;

void initialize_list (list_t list, unsigned int key) {
    list.head = (node_t*) malloc (sizeof(node_t));
    //Initializing default values for head
    list.head->key = key;
    list.head->prev_pointer = NULL;
    list.head->next_pointer = NULL;
}

void list_insert_head (list_t* list, unsigned int key) {
    node_t* new_node = (node_t*) malloc (sizeof(node_t));
    //new_node initialization
    new_node->key = key;
    new_node->next_pointer = list->head;
    new_node->prev_pointer = NULL;
    //old head is new "next node"
    list->head->prev_pointer = new_node;
    list->head->next_pointer = NULL;
    //Setting new head
    list->head = new_node;

}


int main (int argc, char* argv[]) {

    if (argc < 2) {
        perror("Error with passed parameters");
        exit(EXIT_FAILURE);
    }


    return 0;
}