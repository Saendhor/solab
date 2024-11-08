#include <pthread.h>
#include "my_bst.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main (int argc, char* argv[]) {
    node_t* root = new_node(15);
    insert_key(&root, 66);
    
    if (argc < 2) {
        perror("Error with inputed parameters");
        exit(EXIT_FAILURE);
    }

    print_tree_rec(root, 0);

    
    return 0;
}