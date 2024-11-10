#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct node {
    int key;
    struct node* parent;
    struct node* left;
    struct node* right;

} node_t;

//Create a new BST node
node_t* new_node (int key) {
    node_t* temp = (node_t*) malloc(sizeof(node_t));
    
    if (temp != NULL) {
        temp->key = key;
        temp->parent = temp->left = temp->right = NULL;
    }
    return temp;
}

int insert_key (node_t** rootptr, int key, node_t* prev) {
    node_t* root = *rootptr;

    if (root == NULL){
        //Tree empty
        (*rootptr) = new_node(key);
        (*rootptr)->parent = prev;
        return 0;
    }

    if (key <= root->key) {
        //Check left
        return insert_key(&(root->left), key, root);
    } else {
        //Check right
        return insert_key(&(root->right), key, root);
    }
}

int find_key (node_t* root, int key) {
    if (root == NULL) {
        perror("Selected root node is NULL");
        exit(EXIT_FAILURE);
    }

    if (root->key == key){
        printf("[BST] Given key found in tree!\n");
        return 1;
    }

    if (key < root->key){
        printf("[BST] Checking left subtree...\n");
        return find_key(root->left, key);

    } else {
        printf("[BST] Checking right subtree...\n");
        return find_key(root->right, key);
    }
}

int delete_key (node_t* root, int key) {
    //Check if given node is NULL
    if (root == NULL) {
        perror("Selected root node is NULL");
        exit(EXIT_FAILURE);
    }
    
    if (root->key == key) {
        printf("[BST] Key found! Deleting node...\n");
        //root is root (parent == NULL)

        //root has parent
            //root is leaf (root->left == NULL && root->right == NULL)
            //free(root);
            //root has only right subtree (root->left == NULL)
            //root->right->parent = root->parent;
            
        root->left->left = root->left;
        root->left->right = root->right;
        free (root);
        return 0;
    }

    if (key < root->key){
        printf("[BST] Checking left subtree...\n");
        delete_key(root->left, key);

    } else {
        printf("[BST] Checking right subtree...\n");
        delete_key(root->right, key);
    }

}

int get_max_key (node_t* root) {
    // Check if  given node is NULL
    if (root == NULL) {
        perror("Selected root node is NULL");
        exit(EXIT_FAILURE);
    }

    //Check if right node is null and returns value
    if (root->right == NULL) {
        printf("[BST] Right subtree is NULL. Returning selected root key...\n");
        return root->key;
    }

    //Ricursively calls get_max_key
    get_max_key(root->right);
}

int get_min_key (node_t* root) {
    // Check if  given node is NULL
    if (root == NULL) {
        printf("[BST] Selected root node is NULL\n");
        return 0;
    }

    //Check if right node is null and returns value
    if (root->left == NULL) {
        printf("[BST] Left subtree is NULL. Returning selected root key...\n");
        return root->key;
    }

    //Ricursively calls get_max_key
    get_min_key(root->right);
}

void deallocate_tree (node_t* root) {
    //Checks if given tree was already deallocated or simply NULL
    if (root == NULL){
        printf("[BST] Given tree was already NULL\n");
        return;
    }

    //Checks for right subtree recursively
    if (root->right != NULL) {
        //printf("[BST] Deallocating right node with key %d\n", root->right->key);
        deallocate_tree(root->right);
    }
    
    //Checks for left subtree recursively
    if (root->left != NULL) {
        //printf("[BST] Deallocating left node with key %d\n", root->left->key);
        deallocate_tree(root->left);
    }

    //If node does not have right and left nodes then it gets deallocated from memory
    printf("[BST] Deallocating node with key %d\n", root->key);
    free(root);
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