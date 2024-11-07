# ruota-fortuna (10 / 07 / 2024)
Creare un programma ruota-fortuna.c in linguaggio C che accetti invocazioni sulla riga di comando del tipo:
```bash
ruota-fortuna <n-numero-giocatori> <m-numero-partite> <file-con-frasi>
```
Il programma dovrà gestire m partite del gioco "Ruota della Fortuna" tra n giocatori rappresentati da altrettanti thread Gi.
Il gioco sarà gestito e coordinato dal thread principale M (detto Mike).
Questo dovrà leggere le frasi da usare dal file indicato e creare i thread secondari.
Tutti i thread condivideranno una struttura dati comune della Tabellone che conterrà una rappresentazione delle seguenti informazioni:
- frase da scoprire: inizialmente tutte le lettere alfabetiche (a-z, A-Z) saranno sostituite con dei simboli #; man mano che i giocatori indovineranno le lettere presenti, queste verranno sostituite con le effettive occorrenze;
- lettere già chiamate: l'insieme delle lettere già chiamate durante la partita corrente;
- i punteggi numerici attuali dei giocatori;
- la lettere prescelta dal giocatore attuale;
- eventuale flag di fine partita.

Il thread Mike ad ogni nuova partita dovrà scegliere dal file una frase a caso, avendo cura di non scegliere più volte la stessa frase in una sequenza di partite.
Caricata la "frase offuscata" sul Tabellone, Mike risveglierà un giocatore alla volta per concedere la possibilità di indovinare una tra le lettere ancora non scoperte:
se il giocatore indovina riceverà un punteggio aggiuntivo casuale tra 100, 200, 300 e 400 moltiplicato per il numero di occorrenze trovate e manterrà il turno;
se sbaglia, il turno passerà al giocatore successivo.
Ogni giocatore Gi nel suo turno dovrà indicare la scelta effettuata avendo cura di non scegliere le lettere già chiamate:
segnalata la disponibilità della propria scelta a Mike, questo si occuperà di verificare le occorrenze, svelare le lettere corrispondenti e aggiornare i punteggi.
La scelta può essere semplicemente casuale o prevedere qualche strategia diversa, a scelta dello studente, anche diversa tra i giocatori.

Alla fine di ogni partita il thread Mike riporta i punteggi attuali e, alla fine dell'ultima partita, il giocatore vincitore.
Tutti i thread dovranno terminare correttamente e spontaneamente.

Tutti i thread useranno unicamente variabili condizione e mutex per coordinarsi tra di loro nella misura (minima) e nelle modalità ritenute necessarie dallo studente.

Verificare all'avvio che il numero di frasi nel file passato sia sufficiente a gestire il numero di partite richieste.
Nella frase segreta solo le lettere alfabetiche semplici (a-z, A-Z) dovranno essere oscurate: tutti gli altri caratteri (spazi, interpunzione, ....) sono da lasciare invariati e non sono da indovinare.
La verifica è case-insensitive.

Nota: possono risultare utili, ma non indispensabili, le seguenti funzioni standard per stringhe/caratteri: tolower, toupper, isalpha, strcmp, strcasecmp.

Tempo: 2 ore e 15 minuti

## Esempio di output
Un file con le frasi da indovinare potrebbe essere il seguente
```
Una notte al Museo
Qualcuno volo' sul nido del cuculo
Il senso di Smilla per la neve
Il mistero delle pagine perdute
La maledizione della prima luna
Balla coi lupi
L'Impero colpisce ancora
Kramer contro Kramer
Donne sull'orlo di una crisi di nervi
```
L'output atteso di una esecuzione potrebbe essere il seguente:
```
$ ./ruota-fortuna 3 4 frasi.txt
[M] lette 9 possibili frasi da indovinare per 4 partite
[G1] avviato e pronto
[G2] avviato e pronto
[G3] avviato e pronto
[M] scelta la frase "L'Impero colpisce ancora" per la partita n.1
[M] tabellone: #'###### ######## ######
[M] adesso è il turno di G1
[G1] scelgo la lettera 'b'
[M] nessuna occorrenza per 'b'
[M] tabellone: #'###### ######## ######
[M] adesso è il turno di G2
[G2] scelgo la lettera 'i'
[M] ci sono 2 occorrenze di 'i'; assegnati 200x2=400 punti a G2
[M] tabellone: #'I##### ####i### ######
[M] adesso è ancora il turno di G2
[G2] scelgo la lettera 'e'
[M] ci sono 2 occorrenze di 'e'; assegnati 400x2=800 punti a G2
...
[M] adesso è il turno di G1
[G2] scelgo la lettera 'p'
[M] ci sono 1 occorrenze di 'p'; assegnati 100x1=100 punti a G1
[M] tabellone: L'Impero colpisce ancora
[M] frase completata; punteggi attuali: G1:5100 G2:4800 G3:2400
[M] scelta la frase "Una notte al Museo" per la partita n.2
...
[M] frase completata; punteggi attuali: G1:10300 G2:20200 G3:15300
[M] questa era l'ultima partita: il vincitore è G2
```