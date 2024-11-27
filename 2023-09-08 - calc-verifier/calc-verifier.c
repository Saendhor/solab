#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFERSIZE 32
#define CLOSE -1
#define NAMESIZE 16
#define PATHSIZE 32

 enum operation {ADD= 0, SUB, MUL, NUM_OPERATIONS} ;

typedef struct shared_data {
    long long operand_1;
    long long operand_2;
    long long result;
    short operation;
    unsigned short requester;

    //Mutex(es)
    pthread_mutex_t shared_data_mutex;

    //Semaphore(s)
    sem_t* calc_thread_sem;

} shared_data_t;

typedef struct thread_data {
    unsigned short id;
    char filepath [PATHSIZE];
    shared_data_t* shared_data;

} thread_data_t;

//Obs: the id of the operation threads is the respective operation


void* fthread_calc (void* args) {
    thread_data_t* dt = (thread_data_t*) args;
    FILE* fp;
    char buffer [BUFFERSIZE];
    char operation [2];
    long long total;
    static short exit_message = 0;

    //Cleansing the buffer
    memset(buffer, 0, BUFFERSIZE);

    //Defining the name
    char myname [NAMESIZE];
    snprintf(myname, NAMESIZE, "CALC-%d", dt->id);

    //lock
    if (pthread_mutex_lock(&dt->shared_data->shared_data_mutex) != 0) {
        perror("Error while performing mutex_lock on shared_data_mutex");
        exit(EXIT_FAILURE);
    }

    if (dt->id > exit_message) {
        printf("[%s] my id %u / exit_message %u\n", myname, dt->id ,exit_message);
        exit_message = dt->id -1;
    }

    //unlock
    if (pthread_mutex_unlock(&dt->shared_data->shared_data_mutex) != 0) {
        perror("Error while performing mutex_unlock on shared_data_mutex");
        exit(EXIT_FAILURE);
    }

    printf("[%s] Working with path '%s'\n", myname, dt->filepath);
    if ((fp = fopen(dt->filepath, "r")) == NULL) {
        perror("Error while trying to open given file");
        exit(EXIT_FAILURE);
    }
    
    //Get the first line of the file
    if (fgets(buffer, BUFFERSIZE, fp) == NULL) {
        perror("Error while getting first line of file");
        exit(EXIT_FAILURE);
    }

    //Corrects the string to prevent any error
    if (buffer[BUFFERSIZE-1] == '\n') {
        buffer[BUFFERSIZE-1] == '\0';
    }

    printf("[%s] First line content is '%s' \n", myname, buffer);
    total = atoll(buffer);

    printf ("EXIT MESSAGE %u\n", exit_message);
    while (!feof(fp)) {
        
        //Cleans the buffer
        memset(buffer, 0, BUFFERSIZE);

        //Checks the contents of buffer not to be NULL
        if (fgets(buffer, BUFFERSIZE, fp) == NULL) {
            printf("[%s] Analyzed line is empty. Closing the works...\n", myname);
            break;
        }

        //Corrects the string to prevent any error
        if (buffer[BUFFERSIZE-1] == '\n') {
            buffer[BUFFERSIZE-1] == '\0';
        }

        if (buffer[1] == ' ') {
            operation[0] = buffer[0];
            operation[1] = '\0';
            buffer[0] = ' ';

            printf("[%s] Value in operation '%s'\n", myname, operation);
            printf("[%s] Value in buffer: %lld\n", myname, atoll(buffer));

            //lock
            if (pthread_mutex_lock(&dt->shared_data->shared_data_mutex) != 0) {
                perror("Error while performing mutex_lock on shared_data_mutex");
                exit(EXIT_FAILURE);
            }

            //Insert new items into shared data in order to be processed by thread_operation
            dt->shared_data->operand_1 = total;
            dt->shared_data->operand_2 = atoll(buffer);
            dt->shared_data->requester = dt->id;

            if (operation[0] == '+') {
                dt->shared_data->operation = ADD;

            } else if (operation[0] == '-') {
                dt->shared_data->operation = SUB;

            } else if (operation[0] == 'x') {
                dt->shared_data->operation = MUL;

            } else {
                perror("Inputed operation is unknown");
                exit(EXIT_FAILURE);
            }

            //unlock
            if (pthread_mutex_unlock(&dt->shared_data->shared_data_mutex) != 0) {
                perror("Error while performing mutex_lock on shared_data_mutex");
                exit(EXIT_FAILURE);
            }


            // sem_wait on myself whereas sem_post by requested op_thread knowing calc_thread_sem is a vector
            if (sem_wait(&dt->shared_data->calc_thread_sem[dt->id -1]) != 0) {
                perror("Error while performing sem_wait on calc_thread_sem[dt->id -1]");
                exit(EXIT_FAILURE);
            }

            //lock
            if (pthread_mutex_lock(&dt->shared_data->shared_data_mutex) != 0) {
                perror("Error while performing mutex_lock on shared_data_mutex");
                exit(EXIT_FAILURE);
            }

            //Update total sum
            //no need to check if the request is complied by me because it can already have been overwritten by another
            //thread calc that acquired lock
            total = dt->shared_data->result;

            //unlock
            if (pthread_mutex_unlock(&dt->shared_data->shared_data_mutex) != 0) {
                perror("Error while performing mutex_lock on shared_data_mutex");
                exit(EXIT_FAILURE);
            }
            printf("[%s] Updated total is now '%lld'\n", myname, total);


        } else {
            printf("[%s] Last value detected is '%lld'\n", myname, atoll(buffer));

            if (atoll(buffer) == total){
                printf("[%s] Check completed successfully (%lld, %lld)\n", myname, total, atoll(buffer));

            } else {
                printf("[%s] Last line reached with total (%lld, %lld)\n", myname, total, atoll(buffer));

            }

            //lock
            if (pthread_mutex_lock(&dt->shared_data->shared_data_mutex) != 0) {
                perror("Error while performing mutex_lock on shared_data_mutex");
                exit(EXIT_FAILURE);
            }

            exit_message -= 1;
            if (exit_message == CLOSE) {
                dt->shared_data->operation = CLOSE;
            }
            printf("[%s] Updated exit message (local) %u / (shared) %u\n", myname, exit_message, dt->shared_data->operation);

            //unlock
            if (pthread_mutex_unlock(&dt->shared_data->shared_data_mutex) != 0) {
                perror("Error while performing mutex_unlock on shared_data_mutex");
                exit(EXIT_FAILURE);
            }
            break;
        }
    }

    printf("[%s] Work finished. Closing thread...\n", myname);

    fclose(fp);
    return NULL;
}

void* fthread_operation (void* args) {
    thread_data_t* dt = (thread_data_t*) args;
    char myname [NAMESIZE];

    //Determine name for string "myname"
    if (dt->id == ADD) {
        snprintf(myname, NAMESIZE, "ADD");

    } else if (dt->id == SUB) {
        snprintf(myname, NAMESIZE, "SUB");

    } else if (dt->id == MUL) {
        snprintf(myname, NAMESIZE, "MUL");

    } else {
        perror("Error while determining name");
        exit(EXIT_FAILURE);
    }
    printf("[%s] Working with path '%s'\n", myname, dt->filepath);

    while (1) {
        //lock
        if (pthread_mutex_lock(&dt->shared_data->shared_data_mutex) != 0) {
            perror("Error while performing mutex_lock on shared_data_mutex");
            exit(EXIT_FAILURE);
        }

        if (dt->shared_data->operation >= 0 && dt->shared_data->operation < NUM_OPERATIONS){
            //Returns result
            if (dt->id == ADD) {
                dt->shared_data->result = dt->shared_data->operand_1 + dt->shared_data->operand_2;
                //printf("[%s] '%lld' + '%lld' = '%lld'\n", myname, dt->shared_data->operand_1, dt->shared_data->operand_2, dt->shared_data->result);

            } else if (dt->id == SUB) {
                dt->shared_data->result = dt->shared_data->operand_1 - dt->shared_data->operand_2;
                //printf("[%s] '%lld' - '%lld' = '%lld'\n", myname, dt->shared_data->operand_1, dt->shared_data->operand_2, dt->shared_data->result);

            } else if (dt->id == MUL) {
                dt->shared_data->result = dt->shared_data->operand_1 * dt->shared_data->operand_2;
                //printf("[%s] '%lld' x '%lld' = '%lld'\n", myname, dt->shared_data->operand_1, dt->shared_data->operand_2, dt->shared_data->result);

            } else {
                printf("[%s] Unrecognized operation\n", myname);
            }

            //sem_post on requester
            if (sem_post(&dt->shared_data->calc_thread_sem[dt->shared_data->requester]) != 0) {
                perror("Error while performing sem_wait on calc_thread_sem[dt->id -1]");
                exit(EXIT_FAILURE);
            }
        }

        if (dt->shared_data->operation == CLOSE){
            //unlock
            if (pthread_mutex_unlock(&dt->shared_data->shared_data_mutex) != 0) {
                perror("Error while performing mutex_lock on shared_data_mutex");
                exit(EXIT_FAILURE);
            }
            break;
        }

        //unlock
        if (pthread_mutex_unlock(&dt->shared_data->shared_data_mutex) != 0) {
            perror("Error while performing mutex_lock on shared_data_mutex");
            exit(EXIT_FAILURE);
        }

    }

    printf("[%s] Job completed. Closing...\n", myname);
    return NULL;
}


int main (int argc, char* argv[]) {
    printf ("[MAIN] Entering program...\n");
    int num_files = argc -1;
    
    if (argc < 2) {
        perror("Error with imputed parameters");
        exit(EXIT_FAILURE);
    }

    //Initialization
    //Shared data across the code
    shared_data_t* shared_data = (shared_data_t*) malloc (sizeof (shared_data_t));

    //Calc(s)
    thread_data_t* calc_thread_data = (thread_data_t*) malloc (sizeof(thread_data_t) * num_files);
    pthread_t* thread_calc = (pthread_t*) malloc (sizeof(pthread_t) * num_files);

    //Operation(s)
    thread_data_t* operation_thread_data = (thread_data_t*) malloc (sizeof(thread_data_t) * NUM_OPERATIONS); //0 1 2
    pthread_t* thread_operation = (pthread_t*) malloc (sizeof(pthread_t) * NUM_OPERATIONS);
    
    
    //Defining
    //Shared data across the code
    shared_data->operand_1 = 0;
    shared_data->operand_2 = 0;
    shared_data->result = 0;
    shared_data->operation = 0; // 0 Default value
    shared_data->requester = 0; // Error value (must be != 0)
    shared_data->calc_thread_sem = (sem_t*) malloc (sizeof(sem_t) * num_files);

    //Mutex
    if (pthread_mutex_init(&shared_data->shared_data_mutex, NULL) != 0) {
        perror("Error while performing pthread_mutex_init on shared_data_mutex");
        exit(EXIT_FAILURE);
    }

    //Sem
    for (int i = 0; i < num_files; i++) {
        if (sem_init(&shared_data->calc_thread_sem[i], 0, 1) != 0) {
            perror("Error while performing sem_init on calc_thread_sem");
            exit(EXIT_FAILURE);
        }
    }

    //Calc(s)
    for (int i = 0; i < num_files; i++) {
        calc_thread_data[i].id = i+1;
        snprintf(calc_thread_data[i].filepath, PATHSIZE, "%s", argv[i+1]);
        calc_thread_data[i].shared_data = shared_data;

        if (pthread_create(&thread_calc[i], NULL, fthread_calc, &calc_thread_data[i]) != 0){
            perror("Error while performing pthread_create for thread_calc");
            exit(EXIT_FAILURE);
        }
    }

    //Operation(s)
    // j and operations in operation coincide
    for (int j = 0; j < NUM_OPERATIONS; j++) {
        operation_thread_data[j].id = j;
        memset(operation_thread_data[j].filepath, 0, PATHSIZE);
        operation_thread_data[j].shared_data = shared_data;
        if (pthread_create(&thread_operation[j], NULL, fthread_operation, &operation_thread_data[j]) != 0){
            perror("Error while performing pthread_create for thread_calc");
            exit(EXIT_FAILURE);
        }
    }

    //Closing
    printf("[MAIN] Joining threads...\n");

    //Calc
    for (int i = 0; i < num_files; i++) {
        if (pthread_join(thread_calc[i], NULL) != 0) {
            perror("Error while performing pthread_join for thread_calc");
            exit(EXIT_FAILURE);
        }
    }

    //Operation(s)
    for (int j = 0; j < NUM_OPERATIONS; j++) {
        if (pthread_join(thread_operation[j], NULL) != 0) {
            perror("Error while performing pthread_join for thread_calc");
            exit(EXIT_FAILURE);
        }
    }

    //Free
    free(operation_thread_data);
    free(calc_thread_data);
    free(thread_calc);
    free(thread_operation);
    free(shared_data);
    printf("[MAIN] Dynamicly allocated memory successfully freed!\n");

    return 0;
}