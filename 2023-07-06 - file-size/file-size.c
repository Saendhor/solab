// file-size <dir-1> <dir-2> ... <dir-n>
/*  
Creare un programma file-size.c in linguaggio C che accetti invocazioni sulla riga di
comando del tipo:
    file-size <dir-1> <dir-2> ... <dir-n>

Il programma dovrà determinare la dimensione totale in byte dei file regolari
direttamente contenuti all'interno delle cartelle indicate (senza ricorsione).

Al suo avvio il programma creerà n+1 thread:
    • n thread DIR-i che si occuperanno di scansionare la cartella assegnata alla ricerca
    di file regolari direttamente contenuti in essa (no ricorsione);
    • un thread STAT che si occuperà di determinare la dimensione di ogni file regolare
    individuato.

Gli n thread DIR-i agiranno in parallelo e inseriranno, per ogni file regolare incontrato, il
pathname dello stesso all'interno di un buffer condiviso di capienza prefissata (10
pathname). Il thread STAT estrarrà, uno alla volta, i pathname da tale buffer e
determinerà la dimensione in byte del file associato. La coppia di informazioni
(pathname, dimensione) sarà passata, attravenso un'altra struttura dati, al thread
principale MAIN che si occuperà di mantenere un totale globale.

I thread si dovranno coordinare opportunamente tramite mutex e semafori numerici
POSIX: il numero (minimo) e la modalità di impiego sono da determinare da parte dello
studente. Si dovrà inoltre rispettare la struttura dell'output riportato nell'esempio a
seguire.

I thread dovranno terminare spontaneamente al termine dei lavori.

*/

#include <dirent.h>
#include <linux/limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define STACKSIZE 10

//Shared data structure for thread(s) DIR and STAT
typedef struct shared_thread_data {
    char* stack[STACKSIZE];
    short index;

    sem_t stack_sem;
    pthread_mutex_t stack_mutex;
} shared_thread_data_t;

typedef struct thread_dir_data {
    short id;
    char* pathname;
    shared_thread_data_t* shared_thread_data;

} thread_dir_data_t;

//type for the data structure in STAT and MAIN shared data
typedef struct stat_data {
    char pathname;
    int filesize;

} stat_data_t;

//Shared data structure for thread STAT and MAIN
typedef struct shared_data {
    stat_data_t stat_stack[STACKSIZE];

    //sem_t name
    //pthread_mutex name

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
    int iteration = 0;

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
        iteration += 1;
        printf ("[%s] Iteration n. %d\n", myname, iteration);
        //printf("[%s] Reading file '%s'\n", myname, entry->d_name);
        
        //Manually creating path
        char path[PATH_MAX];
        snprintf(path,PATH_MAX,"%s/%s",dt->pathname,entry->d_name);
        
        //printf("[%s] Passing path: '%s'\n", myname, path);
        if (lstat(path, &statbuf) == -1) {
            perror("Error while trying to read entry name");
            exit(EXIT_FAILURE);
        }
        
        if (S_ISREG(statbuf.st_mode)) {

            //Add string to shared data stack
            pthread_mutex_lock(&dt->shared_thread_data->stack_mutex);

            dt->shared_thread_data->stack[dt->shared_thread_data->index] = path;
            //strcpy(dt->shared_thread_data->stack[dt->shared_thread_data->index], path);
            dt->shared_thread_data->index++;
            
            sem_wait(&dt->shared_thread_data->stack_sem);

            printf("[%s] Element %s inserted in stack at index %d\n", myname, dt->shared_thread_data->stack[dt->shared_thread_data->index], dt->shared_thread_data->index);
            pthread_mutex_unlock(&dt->shared_thread_data->stack_mutex);
            printf("[%s] File '%s' added to buffer\n", myname, entry->d_name);
        }

    }
    



    closedir(dp);
    return NULL;
}

void* thread_stat_funct (void* args) {
    thread_stat_data_t* dt = (thread_stat_data_t*) args;
    
    //Defining thread name
    char myname[4] = "STAT";

    printf("[%s] Ready to consume!\n", myname);


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

    //Instantiate semaphore(s) or mutex(es)
    if ((sem_init(&shared_thread_data->stack_sem, 0, STACKSIZE)) != 0){
        perror("Error while initializing stack semaphore");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&shared_thread_data->stack_mutex, NULL) != 0) {
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
    pthread_t thread_stat = (pthread_t) malloc ( sizeof(pthread_t));
    //Memory allocation for the data that will be used by the thread
    thread_stat_data_t* thread_stat_data = (thread_stat_data_t*) malloc(sizeof(thread_stat_data_t));
    if (pthread_create(&thread_stat, NULL, &thread_stat_funct, &thread_stat_data) != 0) {
        perror("Error while creating thread");
        exit(EXIT_FAILURE);
    }

    //// DO  STUFF ////




    //// END STUFF ////
    printf("[MAIN] Job done! Terminating execution gracefully...\n");

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

    //Free memory
    free(thread_stat_data);
    free(thread_dir_data);
    free(thread_dir);

    free(shared_thread_data);
    free(shared_data);
    printf("[MAIN] Memory successfully cleared!\n");


    return 0;
}

/*

ESEMPIO

$ ./file-size /usr/bin /usr/include/
[D-1] scansione della cartella '/usr/bin'...
[D-2] scansione della cartella '/usr/include/'...
[D-2] trovato il file 'aio.h' in '/usr/include/'
[D-2] trovato il file 'aliases.h' in '/usr/include/'
[STAT] il file '/usr/include/aio.h' ha dimensione 7457 byte
[D-1] trovato il file '411toppm' in '/usr/bin'
[STAT] il file '/usr/include/aliases.h' ha dimensione 2032 byte
[MAIN] con il file '/usr/include/aio.h' il totale parziale è di 7457 byte
[MAIN] con il file '/usr/include/aliases.h' il totale parziale è di 9489 byte
[D-1] trovato il file 'add-apt-repository' in '/usr/bin'
[STAT] il file '/usr/bin/411toppm' ha dimensione 18504 byte
...
[MAIN] il totale finale è di 166389312 byte

*/