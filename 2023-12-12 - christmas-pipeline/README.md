# christmas-pipeline (12 / 12 / 2023)
Creare un programma christmas-pipeline.c in linguaggio C che accetti invocazioni sulla riga di comando del tipo:
```bash
christmas-pipeline <presents-file> <goods-bads-file> <letters-file-1> [... letters-file-n]
```
Il programma dovrà gestire la catena di gestione dei regali di Babbo Natale a partire dai file dati in input:
- letters-file(s): uno o più file di testo che in ogni riga del tipo "nome;regalo" riporta il nome (univoco) del bambino e il regalo desiderato;
- goods-bads-file: un file di testo che in ogni riga del tipo "nome;comportamento" riporta se quel bambino è stato "buono" o "cattivo" nell'ultimo anno;
- presents-file: un file di testo che in ogni riga del tipo "regalo;costo" riporta il costo di produzione che Babbo Natale dovrà sostenere per il regalo prescelto.

Il programma, una volta lanciato creerà i seguenti thread ausiliari:
- uno o più thread Elfo-Smistatore [ES] (uno per ogni letters-file passato sulla riga di comando);
si occuperò di leggere, riga per riga, il rispettivo file con leletterine: dovrà segnalare al thread Babbo-Natale il nome del bambino e il regalo desiderato;
tutti i thread EP dovranno agire in parallelo;
- un thread Babbo-Natale [BN] che per ogni bambino segnalato dovrà attivare il thread Elfo-Indagatore che riporterà lo stato di "buono o cattivo" del bambino in questione;
se il bambino è stato buono, Babbo-Natale segnalerà all'Elfo-Produttore il nominativo e il nome del regalo da creare;
se invece il bambino è risultato cattivo, egli si limiterà a segnalare, ai fini statistici, il fatto all'Elfo-Contabile;
- un thread Elfo-Indagatore [EI] che preliminarmente leggerà il file goods-bads-file e ne immagazinerà in memoria il contenuto per consultarlo successivamente per rispondere alle varie richieste di Babbo-Natale;
la struttura dati prescelta dovrà essere dimensionata dinamicamente a runtime in base al contenuto del file: non è accettabile una dimensione fissa prestabilita;
il thread cercherà il nominativi di volta in volta segnalati da Babbo-Natale e fornirà lo stato ritrovato: buono o cattivo;
- un thread Elfo-Produttore [EP] che preliminarmente leggerà il file presents-file in una struttura dati apposita (con i medesimi vincoli esposti prima);
per ogni bimbo buono segnalato da Babbo-Natale provvederà a cercarne il costo e a segnalarlo all'Elfo-Contabile;
- un thread Elfo-Contabile [EC] che terrà delle statistiche su: numero di lettere ricevute, numero di bambini buoni, numero di bambini cattivi e costo totale di produzione dei regali;
tali contatori saranno man mano aggiornati in base alle segnalazioni ricevute direttamente da Babbo-Natale e dall'Elfo-Produttore.

Tutti i thread si coordineranno utilizzando un singolo mutex e delle variabili condizione (nel modo e nella quantità minimale ritenuta più opportuna da parte dello studente) e avranno accesso (visibilità) ad una unica collezione condivisa di dati:
- nome bambino (stringa);
- nome regalo (stringa);
- comportamento: buono o cattivo;
- costo regalo;
- eventuali altri flag ausiliari.
I thread non dovranno fare uso di alcuna variabile o struttura con visibilità globale.

Esaurite le richieste da gestire, tutti i thread ausiliari dovranno terminare correttamente e spontaneamente. L'output del programma dovrà rispettare quello dell'esempio di sotto riportato.

Tempo: 2 ore e 30 minuti

## Esempio di output
```
$ ./christmas-pipeline regali.txt buoni-cattivi.txt letterine-1.txt letterine-2.txt letterine-3.txt
[ES1] leggo le letterine dal file 'letterine-1.txt'
[ES1] il bambino 'Elio' desidera per Natale desidera 'Pentola'
[ES2] leggo le letterine dal file 'letterine-2.txt'
[ES3] leggo le letterine dal file 'letterine-3.txt'
[ES2] il bambino 'Livio' desidera per Natale desidera 'Forno'
[BN] come si è comportato il bambino 'Elio'?
[EI] il bambino 'Elio' è stato buono quest'anno
[BN] il bambino 'Elio' riceverà il suo regalo 'Pentola'
[EP] creo il regalo 'Pentola' per il bambino 'Elio' al costo di 16 €
[EC] aggiornate le statistiche dei bambini buoni (1) e dei costi totali (16 €)
[BN] come si è comportato il bambino 'Livio'?
[EI] il bambino 'Livio' è stato cattivo quest'anno
[BN] il bambino 'Livio' non riceverà alcun regalo quest'anno!
[EC] aggiornate le statistiche dei bambini cattivi (1)
...
[ES2] non ho più letterine da consegnare
[ES3] non ho più letterine da consegnare
[BN] non ci sono più bambini da esaminare
[EC] quest'anno abbiamo ricevuto 560 richieste da 355 bambini buoni e da 205 cattivi con un costo totale
di produzione di 4073 €
```

I file dell'esempio sono forniti a corredo: letterine-1.txt, letterine-2.txt, letterine-3.txt, regali.txt, buoni-cattivi.txt.