#include "my_bst.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PATHSIZE 64

typedef struct shared_data {
    //Shared data structure
    node_t* number_set;

    //Semaphore(s)
    //sem_t full_sem, empty_sem;

    //Mutex(es)
    pthread_mutex_t shared_data_mutex;

} shared_data_t;

typedef struct thread_data {
    unsigned short id;
    char path[PATHSIZE];

} thread_data_t;


int main (int argc, char* argv[]) {
    shared_data_t* shared_data = (shared_data_t*) malloc (sizeof(shared_data_t));
    shared_data->number_set = (node_t*) malloc(sizeof(node_t));

    //Initialization
    shared_data->number_set = NULL;
/*    
    if (sem_init(&shared_data->full_sem, 0, 0) != 0) {
        perror("Error while initializating full_sem");
        exit(EXIT_FAILURE);
    }

    if (sem_init(&shared_data->empty_sem, 0, N) != 0) {
        perror("Error while initializating empty_sem");
        exit(EXIT_FAILURE);
    }
*/
    if (pthread_mutex_init(&shared_data->shared_data_mutex, NULL)) {
        perror("Error while trying to initialize shared_data mutex");
        exit(EXIT_FAILURE);
    }

    if (argc < 2) {
        perror("Error with inputed parameters");
        exit(EXIT_FAILURE);
    }


    
    return 0;
}