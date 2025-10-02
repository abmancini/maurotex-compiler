# Test funzionali

Questa sezione contiene il sistema di test che permette di verificare le funzionalità del sistema `MauroTeX`.

Questa è l'organizzazione di questa sezione:

* `src`: sorgenti del sistema di test
* `test`: definizione dei test
* `bin` (non rilasciata nel repository): compilatori `MauroTeX` (è possibile in questo modo testare diverse versioni dei compilatori)

Ogni test è contenuto in una cartella il cui nome identifica il test e contiene:

* `input`: cartella contenente tutti i file sorgenti necessari (in molti casi sarà un solo file `MauroTeX`, ma in alcuni potrebbero esserci dei file accessori)
* `<nome-compilatore>`: cartella contenente,  per ogni compilatore supportato (quindi, `m2hv`, `m2lv`, `m2ledmac`, `m2web`, ... e probabilmente altri), i file di output attesi dalla compilazione dei file presenti nella cartella `input`. Essendo i compilatori diversi, c'è una cartella per ognuno di essi: vengono testati solo i compilatori per cui è presente la corrispondente cartella.

È possibile organizzare i test in sottoinsiemi anche ricorsivamente: il sistema esegue un test quando trova una cartella che si chiama col nome del compilatore e se non trova la cartella `input` allo stesso livello segnala un errore.

Per eseguire un insieme di test bisogna dare dalla cartella principale del sistema di test il comando:

```bash
    python src/functional-test.py <insieme>
```

* dove `<insieme>` è una _espressione regolare bash` che indica l'insieme di test che si vuole eseguire; ad esempio:
  * `test`: esegue tutti i test contenuti nella cartella `test`
  * `test/*vv*`: esegue tutti i test che si trovano nella cartella `test` e che hanno `vv` nel nome
  * `test/*cit*/m2hv`: esegue tutti test che si trovano nella cartella `test`, hanno `cit` nel nome ed utilizzano il compilatore `m2hv`
  * se si omette il parametro, viene considerato il valore di default `test`
