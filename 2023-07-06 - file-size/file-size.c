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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char* argv[]) {
    
    if (argc < 2) {
        perror("Error with inputed parameters");
        exit(EXIT_FAILURE);
    }

    printf("[MAIN] Welcome to the jungle!\n");

    return 0;
}