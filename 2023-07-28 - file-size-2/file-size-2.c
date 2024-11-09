#include "my_bst.h"

#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define NAMESIZE 16
#define NUM_ADD_THR 2
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
    char assigned_dir[PATHSIZE];

    shared_data_t* shared_data;

} thread_data_t;


void* thread_dir (void* args) {
    /*
        - scansionare la cartella assegnata alla ricerca
        di file regolari direttamente contenuti in essa (no ricorsione)
        - per ogni file incontrato ne determinerà la dimensione in byte
        e inserirà il numero in una struttura dati number_set condivisa;
    */
    thread_data_t* dt = (thread_data_t*) args;
    DIR* dp;
    struct dirent* entry;
    struct stat statbuf;
    
    //Conveniently defining name
    char myname[NAMESIZE];
    sprintf(myname, "DIR-%d", dt->id);

    printf("[%s] Assigned directory: '%s'\n", myname, dt->assigned_dir);
    if ((dp = opendir(dt->assigned_dir)) == NULL) {
        perror("Error while trying to open directory at given path");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dp)) != NULL) {
        //Excluding '.' and '..' directories to printf
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
            printf("[%s] Reading item '%s'\n", myname, entry->d_name);
        }
        
    }

    //Closing
    printf("[%s] Job completed. Closing thread\n", myname);
    return NULL;
}

void* thread_add (void* args) {
    /*
        in presenza di almeno due numeri in number_set
        estrarre il minimo e il massimo attuale e di re-inserire la loro somma
    */
    thread_data_t* dt = (thread_data_t*) args;
    
    //Conveniently defining name
    char myname[NAMESIZE];
    sprintf(myname, "ADD-%d", dt->id);


    printf("[%s] Assigned directory: '%s'\n", myname, dt->assigned_dir);



    //Closing
    printf("[%s] Job completed. Closing thread\n", myname);
    return NULL;

}


int main (int argc, char* argv[]) {
    int num_dir = argc -1;
    
    //Declaration
    //Shared Data
    shared_data_t* shared_data = (shared_data_t*) malloc (sizeof(shared_data_t));
    shared_data->number_set = (node_t*) malloc(sizeof(node_t));

    //Shared Thread Data
    pthread_t* pthread_dir = (pthread_t*) malloc (sizeof(pthread_t) * num_dir);
    thread_data_t* thread_dir_data = (thread_data_t*) malloc (sizeof(thread_data_t) * num_dir);
    pthread_t* pthread_add = (pthread_t*) malloc (sizeof(pthread_t) * NUM_ADD_THR);
    thread_data_t* thread_add_data = (thread_data_t*) malloc (sizeof(thread_data_t) * NUM_ADD_THR);

    //Initialization
    //Shared Data
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

    //Create thread(s) DIR
    printf("[MAIN] Creating %d thread(s) DIR\n", num_dir);
    for (int i = 0; i < num_dir; i++) {
        //Filling thread_dir_data parameters
        thread_dir_data[i].id = i+1;
        strncpy(thread_dir_data[i].assigned_dir, argv[i+1], PATHSIZE);
        thread_dir_data[i].shared_data = shared_data;
        //Initializing thread itself
        if (pthread_create(&pthread_dir[i], NULL, &thread_dir, &thread_dir_data[i]) != 0) {
            perror("Error while creating new DIR thread");
            exit(EXIT_FAILURE);
        }
    }

    //Create thread(s) ADD
    printf("[MAIN] Creating %d thread(s) ADD\n", NUM_ADD_THR);
    for (int j = 0; j < NUM_ADD_THR; j++) {
        //Filling thread_add_data parameters
        thread_add_data[j].id = j+1;
        memset(thread_add_data[j].assigned_dir, 0, PATHSIZE);
        thread_add_data[j].shared_data = shared_data;
        //Initializing thread itself
        if (pthread_create(&pthread_add[j], NULL, &thread_add, &thread_add_data[j]) != 0) {
            perror("Error while creating new ADD thread");
            exit(EXIT_FAILURE);
        }
    }

    //MAIN STUFF
    sleep(1);

    //Closing thread DIR(s)
    printf("[MAIN] Closing %d thread(s) DIR\n", num_dir);
    for (int i = 0; i < num_dir; i++) {
        if (pthread_join(pthread_dir[i], NULL) != 0) {
            perror("Error while performing join on thread(s) DIR");
            exit(EXIT_FAILURE);
        }
    }

    //Closing thread ADD(s)
    printf("[MAIN] Closing %d thread(s) DIR\n", NUM_ADD_THR);
    for (int j = 0; j < NUM_ADD_THR; j++) {
        if (pthread_join(pthread_add[j], NULL) != 0) {
            perror("Error while performing join on thread(s) ADD");
            exit(EXIT_FAILURE);
        }
    }

    //Destroying semaphore(s) and mutex(es)
    if (pthread_mutex_destroy(&shared_data->shared_data_mutex) != 0) {
        perror("Error while destroying shared_data mutex");
        exit(EXIT_FAILURE);
    }

    //Dynamic memory
    deallocate_tree (shared_data->number_set);
    free(thread_dir_data);
    free (pthread_dir);
    free(shared_data);
    
    return 0;
}