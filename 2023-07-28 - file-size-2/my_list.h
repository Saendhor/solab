#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct node {
    int key;
    struct node* next;

} node_t;

node_t* create_node (int key) {
    node_t* temp = (node_t*) malloc(sizeof(node_t));
    if (temp != NULL) {
        temp->key = key;
        temp->next = NULL;
    }
    return temp;
}

int insert_key (node_t* head, int key) {
    if (head == NULL) {
        printf("[LIST] Given node parameter is NULL\n");
        head = create_node(key);
        return 0;
    }
    // Checks in the memory area pointed by head
    return insert_key(head->next, key);
}

int get_max_key (node_t* head) {
    if (head == NULL) {
        printf("[LIST] Selected root node is NULL\n");
        // Error code
        return -1;
    }

    node_t* temp = head;
    int current_max = head->key;
    while (temp != NULL) {
        temp = temp->next;
        // only ">" because otherwise it would make a useless set of operations
        if (temp->key > current_max) {
            current_max = temp->key;
        }
        printf("[LIST] Currently max item key: %d\n", current_max);
    }
    return current_max;
}

int get_min_key (node_t *head) {
    if (head == NULL) {
        printf("[LIST] Selected root node is NULL\n");
        // Error code
        return -1;
    }

    node_t* temp = head;
    int current_min = head->key;
    while (temp != NULL) {
        temp = temp->next;
        // only "<" because otherwise it would make a useless set of operations
        if (temp->key < current_min) {
            current_min = temp->key;
        }
        printf("[LIST] Currently min item key: %d\n", current_min);
    }

    return current_min;
}

int delete_key (node_t* head, int key) {
    if (head == NULL) {
        printf("Given node is NULL\n");
        // Error code
        return 1;
    }

    if (head->key == key) {
        node_t* temp = head;
        head = head->next;
        free(temp);
        return 0;
    }

    while (head->next != NULL) {
        if (head->next->key == key) {
            node_t* temp = head->next;
            head->next = head->next->next;
            free(temp);
            return 0;
        }
    }

    //Error code
    return 1;
}

int deallocate_list(node_t* head) {
    node_t* temp = NULL;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
    printf("[LIST] Completed deallocating list!\n");
    return 0;
}