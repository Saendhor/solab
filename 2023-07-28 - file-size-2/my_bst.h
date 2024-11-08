#pragma once
#include <stdio.h>
#include <stdlib.h>


typedef struct node {
    int key;
    struct node* left;
    struct node* right;

} node_t;

//Create a new BST node
node_t* new_node (int key) {
    node_t* temp = (node_t*) malloc(sizeof(node_t));
    
    if (temp != NULL) {
        temp->key = key;
        temp->left = temp->right = NULL;
    }
    return temp;
}

int insert_key (node_t** rootptr, int key) {
    node_t* root = *rootptr;

    if (root == NULL){
        //Tree empty
        (*rootptr) = new_node(key);
        return 1;
    }

    if (key == root->key) {
        //do nothing
        return 0;
    }

    if (key < root->key) {
        //Check left
        return insert_key(&(root->left), key);
    } else {
        //Check right
        return insert_key(&(root->right), key);
    }
}

int find_key (node_t* root, int key) {
    if (root == NULL) {
        return 0;
    }

    if (root->key == key){
        return 1;
    }

    if (key < root->key){
        return find_key(root->left, key);

    } else {
        return find_key(root->right, key);

    }
}

void print_tabs (int num_tabs) {
    for (int i = 0; i < num_tabs; i++) {
        printf ("\t");
    }
}

void print_tree_rec (node_t* root, int lvl) {
    if (root == NULL) {
        print_tabs(lvl);
        printf("---<empty>--\n");
        return;
    }

    print_tabs(lvl);
    printf("key = %d\n", root->key);
    print_tabs(lvl);
    printf("left\n");

    print_tree_rec(root->left, lvl+1);
    print_tabs(lvl);
    printf("right\n");

    print_tree_rec(root->right, lvl+1);

    print_tabs(lvl);
    printf("done!\n");

}

void print_tree (node_t* root) {
    print_tree_rec(root, 0);
}

/* EXAMPLE
    node_t* root = NULL;

    insert_key(&root, 15);
    insert_key(&root, 11);
    insert_key(&root, 24);
    insert_key(&root, 5);
    insert_key(&root, 19);
    insert_key(&root, 16);

    print_tree(root);

    printf("%d (%d)\n", 16 , find_key(root, 16));
    printf("%d (%d)\n", 15 , find_key(root, 15));
    printf("%d (%d)\n", 5 , find_key(root, 5));
    printf("%d (%d)\n", 115 , find_key(root, 115));
    printf("%d (%d)\n", 1 , find_key(root, 1));
    printf("%d (%d)\n", 7 , find_key(root, 7));

*/