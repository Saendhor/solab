#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct shared_data {
    
    unsigned int number_set;

} shared_data_t;



int main (int argc, char* argv[]) {

    if (argc < 2) {
        perror("Error with inputed parameters");
        exit(EXIT_FAILURE);

    }


    return 0;
}