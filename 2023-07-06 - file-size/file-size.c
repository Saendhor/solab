#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define STACKSIZE 10
#define PATHSIZE 64

//Shared data structure for thread(s) DIR and STAT
typedef struct shared_thread_data {
    //Stack infos
    char stack [STACKSIZE] [PATHSIZE];
    unsigned short index;
    unsigned int total_inserted_items;

    //Thread status
    unsigned short exit_status;
    unsigned short thr_dir_num;

    //Semaphore(s)
    sem_t full_stack_sem;
    sem_t empty_stack_sem;

    //Mutex(es)
    pthread_mutex_t stack_mutex;
    pthread_mutex_t exit_status_mutex;

} shared_thread_data_t;

typedef struct thread_dir_data {
    short id;
    char* pathname;
    shared_thread_data_t* shared_thread_data;

} thread_dir_data_t;

//type for the data structure in STAT and MAIN shared data
typedef struct stat_data {
    char pathname[PATHSIZE];
    int filesize;

} stat_data_t;

//Shared data structure for thread STAT and MAIN
typedef struct shared_data {
    //Stack infos
    stat_data_t stat_stack[STACKSIZE];
    unsigned short index;
    unsigned int total_extracted_items;

    //Thread status
    unsigned short exit_status;

    //Semaphore(s)
    sem_t full_stat_sem;
    sem_t empty_stat_sem;

    //Mutex(es)
    pthread_mutex_t stat_mutex;
    pthread_mutex_t exit_status_mutex;

} shared_data_t;

typedef struct thread_stat_data {
    shared_thread_data_t* shared_thread_data;
    shared_data_t* shared_data;
    
} thread_stat_data_t;

void* thread_dir_funct (void* args) {
    thread_dir_data_t* dt = (thread_dir_data_t*) args;
    DIR* dp;
    struct dirent* entry;
    struct stat statbuf;
    char path[PATHSIZE];

    //Defining thread name
    char myname[6];
    sprintf(myname, "DIR-%d", dt->id);

    printf("[%s] Directory to scan: '%s'\n", myname, dt->pathname);
    //Opens directory pointer
    if ((dp = opendir(dt->pathname)) == NULL ){
        perror("Error while trying to open specified directory");
        exit(EXIT_FAILURE);
    }
    
    //Loops to read all entries
    printf("[%s] Scanning directory's list\n", myname);
    while ((entry = readdir(dp)) != NULL){

        //Excluding '.' and '..' directories to printf
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
            printf("[%s] Reading item '%s'\n", myname, entry->d_name);
            //Manually creating path
            snprintf(path, PATHSIZE, "%s/%s", dt->pathname, entry->d_name);
            
            if (lstat(path, &statbuf) == -1) {
                perror("Error while trying to read entry name");
                exit(EXIT_FAILURE);
            }
        }
        
        //Add string to shared data stack
        if (S_ISREG(statbuf.st_mode)) {
            //down (empty)
            if (sem_wait(&dt->shared_thread_data->empty_stack_sem) != 0) {
                perror("Error while performing sem wait to empty_stack_sem");
                exit(EXIT_FAILURE);
            }
            //down (mutex)
            if (pthread_mutex_lock(&dt->shared_thread_data->stack_mutex) != 0) {
                perror("Error while performing lock stack mutex");
                exit(EXIT_FAILURE);
            }

            //Insert item into stack
            strcpy(dt->shared_thread_data->stack[dt->shared_thread_data->index], path);
            printf("[%s] Element '%s' inserted in stack at index %u\n", myname, dt->shared_thread_data->stack[dt->shared_thread_data->index], dt->shared_thread_data->index);
            
            //Increase indexes
            dt->shared_thread_data->index = (dt->shared_thread_data->index +1) % STACKSIZE;
            dt->shared_thread_data->total_inserted_items += 1;

            //up (mutex)
            if (pthread_mutex_unlock(&dt->shared_thread_data->stack_mutex) != 0) {
                perror("Error while performing unlock to stack mutex");
                exit(EXIT_FAILURE);
            }
            //up (full)
            if (sem_post(&dt->shared_thread_data->full_stack_sem) != 0) {
                perror("Error while performing sem wait to empty_stack_sem");
                exit(EXIT_FAILURE);
            }
        }
    }
    //Increase exit_status from shared_data
    if (pthread_mutex_lock(&dt->shared_thread_data->exit_status_mutex) != 0) {
        perror("Error while performing lock exit status mutex");
        exit(EXIT_FAILURE);
    }

    dt->shared_thread_data->exit_status += 1;
    printf("[%s] Exit status incremented. Current value: %u/%u\n", myname, dt->shared_thread_data->exit_status, dt->shared_thread_data->thr_dir_num);

    if (pthread_mutex_unlock(&dt->shared_thread_data->exit_status_mutex) != 0) {
        perror("Error while performing unlock exit_status mutex");
        exit(EXIT_FAILURE);
    }

    printf("[%s] Job completed. Closing...\n", myname);
    closedir(dp);
    return NULL;
}

void* thread_stat_funct (void* args) {
    thread_stat_data_t* dt = (thread_stat_data_t*) args;
    unsigned int iteration = 0;
    char path [PATHSIZE];
    struct stat statbuf;
    unsigned short exit_status = 0;
    unsigned int total_inserted_items = 1;
    unsigned int total_extracted_items = 0;

    
    //Defining thread name
    char myname[5] = "STAT\0";
    printf("[%s] Ready to consume.\n", myname);

    while ((exit_status < dt->shared_thread_data->thr_dir_num) && (total_extracted_items < total_inserted_items)) {
        //Defining number of iterations
        iteration += 1;
        //Getting exit status
        if (pthread_mutex_lock(&dt->shared_thread_data->exit_status_mutex) != 0) {
            perror("Error while performing lock exit status mutex");
            exit(EXIT_FAILURE);
        }

        exit_status = dt->shared_thread_data->exit_status;
        total_inserted_items = dt->shared_thread_data->total_inserted_items;

        if (pthread_mutex_unlock(&dt->shared_thread_data->exit_status_mutex) != 0) {
            perror("Error while performing unlock exit_status mutex");
            exit(EXIT_FAILURE);
        }
        printf("[%s] Iteration n.%u with exit status %u/%u\n", myname, iteration, exit_status, dt->shared_thread_data->thr_dir_num);

        // THREAD DIR
        //down (full)
        if (sem_wait(&dt->shared_thread_data->full_stack_sem) != 0) {
            perror("Error while performing sem wait to empty_stat_sem");
            exit(EXIT_FAILURE);
        }
        //down (mutex)
        if (pthread_mutex_lock(&dt->shared_thread_data->stack_mutex) != 0) {
            perror("Error while performing lock stack mutex");
            exit(EXIT_FAILURE);
        }

        //Decrease index number
        dt->shared_thread_data->index = (dt->shared_thread_data->index -1) % STACKSIZE;

        if (dt->shared_thread_data->stack[dt->shared_thread_data->index] != NULL) {
            
            //Extract form stack
            strcpy(path, dt->shared_thread_data->stack[dt->shared_thread_data->index]);
            printf("[%s] Item '%s' extracted from the stack at index %u\n", myname, path, dt->shared_thread_data->index);
            
            //Fill selected stat slot with zero(s)
            memset(dt->shared_thread_data->stack[dt->shared_thread_data->index], 0, sizeof(path));

        }
        
        //up (mutex)
        if (pthread_mutex_unlock(&dt->shared_thread_data->stack_mutex) != 0) {
            perror("Error while performing unlock stack mutex");
            exit(EXIT_FAILURE);
        }
        //up (empty)
        if (sem_post(&dt->shared_thread_data->empty_stack_sem) != 0) {
            perror("Error while performing sem wait to empty_stack_sem");
            exit(EXIT_FAILURE);
        }

        //Determine size using lstat
        printf("[%s] Reading '%s' stats\n", myname, path);
        if (lstat(path, &statbuf) == -1) {
            perror("Error while trying to read entry name");
            exit(EXIT_FAILURE);
        }

        // THREAD MAIN        
        //down (empty)
        if (sem_wait(&dt->shared_data->empty_stat_sem) != 0) {
            perror("Error while performing sem wait to empty_stack_sem");
            exit(EXIT_FAILURE);
        }
        
        //down (mutex)
        if (pthread_mutex_lock(&dt->shared_data->stat_mutex) != 0) {
            perror("Error while performing lock stack mutex");
            exit(EXIT_FAILURE);
        }

        //Insert item (path / size) in stat stack
        strcpy(dt->shared_data->stat_stack[dt->shared_data->index].pathname, path);
        dt->shared_data->stat_stack[dt->shared_data->index].filesize = statbuf.st_size;
        printf("[%s] New item (%s, %d) inserted in stack at index %u\n", myname, dt->shared_data->stat_stack[dt->shared_data->index].pathname, dt->shared_data->stat_stack[dt->shared_data->index].filesize, dt->shared_data->index);
        
        //Increase counter
        dt->shared_data->index = (dt->shared_data->index +1) % STACKSIZE;
        dt->shared_data->total_extracted_items += 1;
        total_extracted_items = dt->shared_data->total_extracted_items;

        //up (mutex)
        if (pthread_mutex_unlock(&dt->shared_data->stat_mutex) != 0) {
            perror("Error while performing lock stack mutex");
            exit(EXIT_FAILURE);
        }

        //up (full)
        if (sem_post(&dt->shared_data->full_stat_sem) != 0) {
            perror("Error while performing sem post to full_stack_sem");
            exit(EXIT_FAILURE);
        }       
    }

    //down (mutex)
    if (pthread_mutex_lock(&dt->shared_data->exit_status_mutex) != 0) {
        perror("Error while performing lock stack mutex");
        exit(EXIT_FAILURE);
    }

    //Signal exit to main
    dt->shared_data->exit_status += 1;

    //up (mutex)
    if (pthread_mutex_unlock(&dt->shared_data->exit_status_mutex) != 0) {
        perror("Error while performing lock stack mutex");
        exit(EXIT_FAILURE);
    }

    printf("[%s] Job completed. Closing...\n", myname);
    return NULL;
}


int main (int argc, char* argv[]) {
    int num_dir = argc -1;
    shared_thread_data_t* shared_thread_data = (shared_thread_data_t*) malloc (sizeof(shared_thread_data_t));
    shared_data_t* shared_data = (shared_data_t*) malloc (sizeof(shared_data_t));
    
    if (argc < 2) {
        perror("Error with inputed parameters");
        exit(EXIT_FAILURE);
    }
    
    //Initializing
    shared_thread_data->index = 0;
    shared_thread_data->exit_status = 0;
    shared_thread_data->thr_dir_num = num_dir;
    shared_thread_data->total_inserted_items = 0;

    shared_data->index = 0;
    shared_data->exit_status = 0;
    shared_data->total_extracted_items = 0;

    //Instantiate semaphore(s) or mutex(es)
    // SHARED THREAD DATA
    if ((sem_init(&shared_thread_data->empty_stack_sem, 0, STACKSIZE)) != 0){
        perror("Error while initializing empty stack semaphore");
        exit(EXIT_FAILURE);
    }

    if ((sem_init(&shared_thread_data->full_stack_sem, 0, 0)) != 0){
        perror("Error while initializing full stack semaphore");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&shared_thread_data->stack_mutex, NULL) != 0) {
        perror("Error while initializing stack mutex");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&shared_thread_data->exit_status_mutex, NULL) != 0) {
        perror("Error while initializing stack mutex");
        exit(EXIT_FAILURE);
    }

    // SHARED DATA
    if ((sem_init(&shared_data->empty_stat_sem, 0, STACKSIZE)) != 0){
        perror("Error while initializing empty stack semaphore");
        exit(EXIT_FAILURE);
    }

    if ((sem_init(&shared_data->full_stat_sem, 0, 0)) != 0){
        perror("Error while initializing full stack semaphore");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&shared_data->stat_mutex, NULL) != 0) {
        perror("Error while initializing stack mutex");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&shared_data->exit_status_mutex, NULL) != 0) {
        perror("Error while initializing stack mutex");
        exit(EXIT_FAILURE);
    }


    //Create DIR-i threads
    printf("[MAIN] Instantiating %d thread(s) DIR\n", num_dir);
    //Memory allocation for the existance of the thread
    pthread_t* thread_dir = (pthread_t*) malloc ( sizeof(pthread_t) * num_dir);
    //Memory allocation for the data that will be used by the thread
    thread_dir_data_t* thread_dir_data = (thread_dir_data_t*) malloc(sizeof(thread_dir_data_t) * num_dir);

    for (int i = 0; i < num_dir; i++) {
        //Determines the id of the thread_dir
        thread_dir_data[i].id = i+1;
        //Determine associated folder
        thread_dir_data[i].pathname = argv[i+1];
        //Gives the shared data defined in the main to the single thread
        thread_dir_data[i].shared_thread_data = shared_thread_data;
        if (pthread_create(&thread_dir[i], NULL, &thread_dir_funct, &thread_dir_data[i]) != 0) {
            perror("Error while creating thread DIR");
            exit(EXIT_FAILURE);
        }
    }
    printf("[MAIN] Thread(s)_dir created successfully!\n");

    //Create STAT thread
    printf("[MAIN] Instantiating the thread STAT\n");
    //Memory allocation for the existance of the thread
    pthread_t thread_stat = (pthread_t) malloc (sizeof(pthread_t));
    //Memory allocation for the data that will be used by the thread
    thread_stat_data_t* thread_stat_data = (thread_stat_data_t*) malloc(sizeof(thread_stat_data_t));
    //Initialization of given data structures mentioned above
    thread_stat_data->shared_thread_data = shared_thread_data;
    thread_stat_data->shared_data = shared_data;
    if (pthread_create(&thread_stat, NULL, &thread_stat_funct, thread_stat_data) != 0) {
        perror("Error while creating thread");
        exit(EXIT_FAILURE);
    }

    printf("[MAIN] Starting total size count\n");
    unsigned short exit_status = 0;
    unsigned int total_filesize = 0;
    unsigned int total_read_entries = 0;
    unsigned int iteration = 0;
    unsigned int stat_items = 1;
    while (exit_status == 0 && total_read_entries < stat_items) {
        iteration += 1;
        printf("[MAIN] Total file size at iteration %u is: %u\n", iteration, total_filesize);

        //down (mutex)
        if (pthread_mutex_lock(&shared_data->exit_status_mutex) != 0) {
            perror("Error while performing lock stack mutex");
            exit(EXIT_FAILURE);
        }

        //Check for exit status in shared data
        exit_status = shared_data->exit_status;
        stat_items = shared_data->total_extracted_items;
        if (stat_items == 0){
            stat_items += 1;
        }

        //up (mutex)
        if (pthread_mutex_unlock(&shared_data->exit_status_mutex) != 0) {
            perror("Error while performing lock stack mutex");
            exit(EXIT_FAILURE);
        }
        printf("[MAIN] Exit status from STAT: %u\n", exit_status);

        //down (full)
        if (sem_wait(&shared_data->full_stat_sem) != 0) {
            perror("Error while performing sem wait to empty_stat_sem");
            exit(EXIT_FAILURE);
        }
        //down (mutex)
        if (pthread_mutex_lock(&shared_data->stat_mutex) != 0) {
            perror("Error while performing lock stack mutex");
            exit(EXIT_FAILURE);
        }

        //Decrease index number
        shared_data->index = (shared_data->index -1) % STACKSIZE;

        if (shared_data->stat_stack[shared_data->index].filesize != 0) {
            
            //Extract form stack
            printf("[MAIN] Item '(%s/%u)' extracted from the stack at index %u\n", shared_data->stat_stack[shared_data->index].pathname, shared_data->stat_stack[shared_data->index].filesize, shared_data->index);
            
            //Increase total
            total_filesize += shared_data->stat_stack[shared_data->index].filesize;
            printf("[MAIN] Total size of file read: %u\n", total_filesize);

            //Fill selected stat slot with zero(s)
            memset(shared_data->stat_stack[shared_data->index].pathname, 0, sizeof(shared_data->stat_stack[shared_data->index].pathname));
            shared_data->stat_stack[shared_data->index].filesize = 0;

            //Increase total read entries
            total_read_entries += 1;

        }
        
        //up (mutex)
        if (pthread_mutex_unlock(&shared_data->stat_mutex) != 0) {
            perror("Error while performing unlock stack mutex");
            exit(EXIT_FAILURE);
        }
        //up (empty)
        if (sem_post(&shared_data->empty_stat_sem) != 0) {
            perror("Error while performing sem wait to empty_stack_sem");
            exit(EXIT_FAILURE);
        }
    }
    printf("[MAIN] Job done! Total size of %u files read %u\n", total_read_entries, total_filesize);

    printf("[MAIN] Terminating execution gracefully...\n");
    //Close thread(s) DIR
    for (int i = 0; i < num_dir; i++) {
        if (pthread_join(thread_dir[i], NULL) != 0){
            perror("Error while closing thread DIR");
            exit(EXIT_FAILURE);
        }
    }
    printf("[MAIN] Thread(s) DIR successfully closed!\n");

    //Close thread STAT
    if (pthread_join(thread_stat, NULL) != 0) {
        perror("Error while closing thread STAT");
        exit(EXIT_FAILURE);
    }
    printf("[MAIN] Thread STAT successfully closed!\n");

    //Close semaphore(s) and mutex(es)
    if ((sem_destroy(&shared_thread_data->empty_stack_sem)) != 0){
        perror("Error while destroying full stack semaphore");
        exit(EXIT_FAILURE);
    }
    
    if ((sem_destroy(&shared_thread_data->full_stack_sem)) != 0){
        perror("Error while destroying full stack semaphore");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_destroy(&shared_thread_data->stack_mutex) != 0) {
        perror("Error while destroying stack mutex");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_mutex_destroy(&shared_thread_data->exit_status_mutex) != 0) {
        perror("Error while destroying stack mutex");
        exit(EXIT_FAILURE);
    }

    printf("[MAIN] All semaphores successfully destroyed!\n");

    //Free memory
    free(thread_stat_data);
    free(thread_dir_data);
    free(thread_dir);

    free(shared_thread_data);
    free(shared_data);

    printf("[MAIN] Memory successfully cleared!\n");


    return 0;
}