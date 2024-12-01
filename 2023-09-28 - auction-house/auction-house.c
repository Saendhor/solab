#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFERSIZE 1024

typedef struct shared_data {
    //

    //Mutex(es)

    //Semaphore(s)

} shared_data_t;


int main (int argc, char* argv[]) {

    if (argc != 3) {
        perror("Error with inputed parameters");
        exit(EXIT_FAILURE);
    }





    return 0;
}