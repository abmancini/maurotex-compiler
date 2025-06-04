/*

  file "mvisit.h"
  HEADER DI MVIST

  mvisit: visitatore generalizzato di documenti MTeX
  versione 1.13.b14 del 11 novembre 2013

  eLabor scrl
  (ex. Laboratorio Informatico del Consorzio Sociale "Polis")
  via G. Garibaldi 33, 56127 Pisa - Italia
  <http://elabor.homelinux.org>
  <info@elabor.homelinux.org>

  responsabile sviluppo:
  Paolo Mascellani (paolo@elabor.homelinux.org)

  coordinatore del progetto "Maurolico":
  Pier Daniele Napolitani (napolita@dm.unipi.it)
  
  Questo programma e` software libero. Lei puo` redistribuirlo
  e/o modificarlo nei termini della "GNU General Public License",
  come pubblicata dalla "Free Software Foundation", versione 2 del
  giugno 1991.
  
  Questo programma e` distribuito nella speranza che possa essere
  utile, ma SENZA ALCUNA GARANZIA, implicita o espressa. Vedere la 
  "GNU General Public License" per maggiori dettagli.
  
  Lei, assieme al programma, dovrebbe aver ricevuto anche una copia
  della "GNU General Public License", contenuta nel file "LICENZA";
  se no, scriva alla "Free Software Foundation, Inc., 675 Mass Ave,
  Cambridge, MA 02139, USA".

*/

#ifndef __MVISIT__
#define __MVISIT__

#include <stdio.h>
#include <string.h>

// STRUTTURA DATI DI CONFIGURAZIONE
class Configurazione {
  
 public:
  
  char * sglaut;          // SIGLA AUTORE DA TROVARE
  char * sglope;          // SIGLA OPERA DA TROVARE
  char * nomaut;          // NOME AUTORE DA RIPORTARE NEL FILE DELLE CITAZIONI
  char * titope;          // TITOLO OPERA DA RIPORTARE NEL FILE DELLE CITAZIONI
  char * txtOpeFileName;  // NOME DEL FILE TXT CONTENENTE LE CITAZIONI DELL'OPERA
  char * htmlOpeFileName; // NOME DEL FILE HTML CONTENENTE LE CITAZIONI DELL'OPERA
  
  Configurazione() {
    this->sglaut          = NULL;
    this->sglope          = NULL;
    this->nomaut          = NULL;
    this->titope          = NULL;
    this->txtOpeFileName  = NULL;
    this->htmlOpeFileName = NULL;
  }
  
  // IMPOSTAZIONE DELLE VARIABILI DI CONFIGURAZIONE
  void setSglaut(char * sglaut) {
    this->sglaut = this->clone(sglaut);
  }
  
  void setSglope(char * sglope) {
    this->sglope = this->clone(sglope);
  }
  
  void setNomaut(char * nomaut) {
    this->nomaut = this->clone(nomaut);
  }
  
  void setTitope(char * titope) {
    this->titope = this->clone(titope);
  }
  
  void setTxtOpeFileName(char * txtOpeFileName) {
    this->txtOpeFileName = this->clone(txtOpeFileName);
  }
  
  char * getTxtOpeFileName() {
    return this->txtOpeFileName;
  }
  
  void setHtmlOpeFileName(char * htmlOpeFileName) {
    this->htmlOpeFileName = this->clone(htmlOpeFileName);
  }
  
  char * getHtmlOpeFileName() {
    return this->htmlOpeFileName;
  }
  
  // STAMPA DELLE VARIABILI DI CONFIGURAZIONE
  void print() {
    printf("%s\t%s\t%s\t%s\t%s\t%s\n", this->sglaut, this->sglope, this->nomaut, this->titope,
	   this->txtOpeFileName, this->htmlOpeFileName);
  }
  
 private:
  // GENERAZIONE COPIA STRINGA
  char * clone(char * s) {
    
    char * d = NULL;
    
    if (s == NULL) {
      fprintf(stderr, "Errore interno: clone di stringa vuota\n");
    } else {
      d = new char[strlen(s)+1];
      strcpy(d, s);
    }
    
    return d;
  }
  
};

// PARAMETRI DELLA VISITA
class PARAMETRI {
  
 public:
  short nProp;       // NUMERO DI PROPOSIZIONE
  short nUnit;       // NUMERO DI UNIT
  short nSec;        // NUMERO DI SEZIONE
  short nSubsec;     // NUMERO DI SOTTOSEZIONE
  short nSSubsec;    // NUMERO DI SOTTOSOTTOSEZIONE
  short nAdnot;      // NUMERO DI ANNOTAZIONE
  short nComm;      // NUMERO DI COMMENTO
  short nFootnote;   // NUMERO DI NOTA A PIEDE PAGINA
  short nVV;         // NUMERO DI NOTA
  short nFig;        // NUMERO DI FIGURA
  bool  nota;        // TESTO INTERNO AD UNA NOTA
  bool  greco;       // TESTO GRECO
  bool  math;        // AMBIENTE MATEMATICO
  bool  banale;      // VARIANTE BANALE
  const char  *testimone;  // TESTIMONE DI DEFAULT (PER LE MACRO DI SEMPLIFICAZIONE)
  FILE  *outFile;    // FILE DI OUTPUT
  const char *label; // PAROLA DI CHIUSURA DELLLA VARIANTE LUNGA
  struct testo * mitIntest;
  struct testo * desIntest;
  struct testo * lmiIntest;
  struct testo * datIntest;
  struct testo * comIntest;
  short listEnv;
  bool itemOpen;
  //char ** tabSnap;
  
  // COSTRUTTORE DELLA CLASSE
  PARAMETRI () {
    this->nProp     = 0;
    this->nUnit     = 0;
    this->nSec      = 0;
    this->nSubsec   = 0;
    this->nSSubsec  = 0;
    this->nAdnot    = 0;
    this->nComm     = 0;
    this->nFootnote = 0;
    this->nVV       = 0;
    this->nFig      = 0;
    this->nota      = false;
    this->greco     = false;
    this->math      = false;
    this->banale    = false;
    this->testimone = "";
    this->outFile   = NULL;
    this->label     = NULL;
    //this->tabSnap   = NULL;
    this->mitIntest = NULL;
    this->desIntest = NULL;
    this->lmiIntest = NULL;
    this->datIntest = NULL;
    this->comIntest = NULL;
	this->listEnv = 0;
	this->itemOpen = false;
  }
  
  // DISTRUTTORE DELLA CLASSE
  ~PARAMETRI (void) {
    if (this->outFile) {
      fclose(this->outFile);
      this->outFile = 0;
    }
  }
  
  // IMPOSTAZIONE DEL FILE DI OUTPUT
  void setFileName(char * fileName) {
    if (this->outFile) {
      fclose(this->outFile);
    }
    this->outFile = fopen(fileName, "w");
  }
};

#endif

// PROTOTIPO DELLE FUNZIONI DEFINITE IN MVISIT

// ESTRAZIONE DELLA VARIANTE CHE SI RIFERISCE AL TESTIMONE RICHIESTO
void estrazioneVariante(struct testo * testo,
			PARAMETRI    * parametri,
			struct testo * sigle);

// CONTROLLA SE LA VARIANTE E` SIGNIFICATIVA RISPETTO AL TESTIMONE
bool checkVariante(struct testo * testo, char * testRif);

// CONTROLLA SE LE VARIANTI SONO SIGNIFICATIVE RISPETTO AL TESTIMONE
bool checkVarianti(struct testo * testo, char * testRif);

// VISITA UN ELEMENTO DI TESTO
void visitTesto(struct testo * testo,
		PARAMETRI    * parametri,
		struct testo * sigle);

// VISITA LA KEY DI UN ELEMENTO DI TESTO
void visitKey(short          key,
	      long           ln,
	      struct testo * testo,
	      PARAMETRI    * parametri,
	      struct testo * sigle);

// VISITA UNA VV
void visitVV(short          key,
	     struct testo * testo,
	     PARAMETRI    * parametri,
	     struct testo * sigle);

// VISITA DI UNA LISTA DI VARIANTI E RITORNA IL NUMERO DI TESTIMONI
short visitVarianti(struct testo * testo,
		    PARAMETRI    * parametri,
		    struct testo * sigle,
		    bool           nested,
		    bool           duplex,
		    bool           multiplex,
		    int            numero);

// VISITA UNA VARIANTE E RITORNA UNA CONFERMA DI ESISTENZA DELLA VARIANTE
bool visitVariante(struct testo * testo,
		   PARAMETRI    * parametri,
		   struct testo * sigle,
		   bool           nested,
		   bool           duplex,
		   bool           multiplex,
		   int            numero,
		   short        * nTest);

// VISITA LE SIGLE DI UNA VARIANTE
short visitSigle(struct testo * sigle, bool noNota, FILE * outFile);

// STAMPA MACRO SCONOSCIUTA
void printUnknow(short          key,
				 const char   * lexema,
				 struct testo * testo,
				 int            ln,
				 PARAMETRI    * parametri,
				 struct testo * sigle);
				 
// VISITA I POSTSCRIPT DI UNA VARIANTE
void visitPostscript(struct testo * sigle,
		     struct testo * nota,
		     PARAMETRI    * parametri,
		     bool           postscript);

void printDebug(FILE * outFile, struct testo * testo);

void stampaCitazioneOpera(PARAMETRI * parametri,
			  const char * libro,
			  const char * prop);

// PROTOTIPO DELLE FUNZIONI DA DEFINIRE PER I PROGRAMMI CHE USANO MVISIT

// FUNZIONE DI LETTURA DEL FILE DI CONFIGURAZIONE
void leggiConfigurazione(char * confFileName);

// RITORNA IL NUMERO DELLE CONFIGURAZIONI
short getNConf();

// RITORNA LA CONFIGURAZIONE
Configurazione * getConfigurazione(short iConf);

// STAMPA IL RIFERIMENTO DELLE CITAZIONI
void printRiferimento(PARAMETRI * parametri,
		      short iConf,
		      const char * libro,
		      const char * prop);

// RICERCA DLL'INDICE DELLE SIGLE (AUTORE E OPERA) NEL FILE DI CONFIGURAZIONE
short trovaConfigurazione(const char * sglaut, const char * sglope);

// TROVA IL RIFERIMENTO DI UNA CITAZIONE
void trovaRiferimento(struct testo * testo,
		      short *iConf, const char ** libro, const char ** prop);

// FUNZIONE CHE CONTROLLA SE LA VV DEVE ESSERE GESTITA DA MVISIT
bool checkVV(short          key,
	     struct testo * testo,
	     PARAMETRI    * parametri,
	     struct testo * sigle);

// FUNZIONE CHE STAMPA UNA VV
void printVV(short          key,
	     struct testo * testo,
	     PARAMETRI    * parametri,
	     struct testo * sigle);

// FUNZIONE CHE STAMPA UNo SCHOLIUM
void printSCHOL(short          key,
	     struct testo * testo,
	     PARAMETRI    * parametri,
	     struct testo * sigle);

// STAMPA LA SIGLA DI UN TESTIMONE
void printSigla(const char * sigla, FILE * outFile);

// STAMPA I POSTSCRIPT DI UNA VARIANTE
void printPostscript(struct testo * sigle, struct testo * nota, PARAMETRI * parametri);

// FUNZIONE CHE STAMPA IL PROLOGO DI UNA TABELLA
void printTableStart(int key, struct testo * testo, PARAMETRI * parametri, struct testo * sigle);

// FUNZIONE CHE STAMPA L'EPILOGO DI UNA TABELLA
void printTableFinish(int key, struct testo * testo, PARAMETRI * parametri, struct testo * sigle);

// FUNZIONE CHE STAMPA LO HEADER DI RIGA DI UNA TABELLA
void printRowHeader(short key, PARAMETRI * parametri);

// FUNZIONE CHE STAMPA IL FOOTER DI RIGA DI UNA TABELLA
void printRowFooter(short key, PARAMETRI * parametri);

// FUNZIONE CHE STAMPA LO HEADER DI UNA TABELLA
void printTableHeader(int key, const char * colonne, PARAMETRI * parametri);

// FUNZIONE CHE STAMPA IL FOOTER DI UNA TABELLA
void printTableFooter(int key, PARAMETRI * parametri);

// FUNZIONE CHE STAMPA UN CAMPO DI UNA TABELLA
void printCampo(short          nRow,
		short          nCol,
		const char   * colonne,
		struct testo * campo,
		PARAMETRI    * parametri,
		struct testo * sigle);

// FUNZIONE DI STAMPA DI UNA MACRO GENERICA
void printKey(bool           inizio,
	      short          key,
	      long          ln,
	      struct testo * testo,
	      PARAMETRI    * parametri,
	      struct testo * sigle);

// FUNZIONE DI STAMPA DI UNA FRAZIONE
void printFrac(struct testo * testo, PARAMETRI * parametri, struct testo * sigle);

// STAMPA I TITOLI DI UN OPERA
void printTitleWork(struct testo * testo, PARAMETRI * parametri, struct testo * sigle);

// FUNZIONE DI STAMPA DI UN ESPONENTE E DI UN DEPONENTE
void printSupSub(struct testo * testo, PARAMETRI * parametri, struct testo * sigle);

// FUNZIONE DI STAMPA DEI CASES
void printCases(short key, struct testo * testo, PARAMETRI * parametri, struct testo * sigle);

// FUNZIONE DI STAMPA DI UN RIFERIMENTO AD UNA UNIT
void printUnitRef(const char * opera, const char * etichetta, PARAMETRI * parametri);
void printUnitRef(short nUnit, PARAMETRI * parametri);

// FUNZIONE DI STAMPA DEI FOLIA
void printFolium(const char * testimone, const char * pagina, PARAMETRI* param, struct testo *sigle);

// FUNZIONE DI STAMPA DELLE CITAZIONI
void printCit(struct testo *testo, PARAMETRI* param, struct testo *sigle);

// FUNZIONE DI STAMPA DELLE DATE
void printDate(struct testo *testo, PARAMETRI* param, struct testo *sigle);

// FUNZIONE DI STAMPA DELLE MANI
void printManus(struct testo *testo, PARAMETRI* param, struct testo *sigle);

// FUNZIONE DI STAMPA DI UNA FIGURA
void printFig(short key, struct testo * testo, PARAMETRI * parametri);

// FUNZIONE DI STAMPA DI UNA PAROLA
void printLexema(short key, const char * lexema, PARAMETRI * parametri);

// FUNZIONE DI STAMPA DI UNA ANNOTAZIONE
void printAdnotatio(struct testo * testo, PARAMETRI * parametri, struct testo * sigle);

// FUNZIONE DI STAMPA DI UN COMMENTO
void printCommento(struct testo * testo, PARAMETRI * parametri, struct testo * sigle);

// FUNZIONE DI STAMPA DI UNA NOTA A PIEDE DI PAGINA
void printFootnote(struct testo * testo, PARAMETRI * parametri, struct testo * sigle);

// FUNZIONE DI STAMPA DI UNA LISTA DI AUTORI
void printAuthors(struct testo * testo, PARAMETRI * parametri, struct testo * sigle);

void printTextGreek(struct testo * testo, PARAMETRI * parametri, struct testo * sigle);

// FUNZIONE DI STAMPA DEL FRONTESPIZIO
void printFrontespizio(struct testo * testo, PARAMETRI * parametri, struct testo * sigle);

// FUNZIONE DI STAMPA ALIAMANUS
void printAliaManus(struct testo * testo, PARAMETRI * parametri, struct testo * sigle);

// FUNZIONE DI MARCATURA DELLE VARIANTI
void printInizioVarianti(PARAMETRI * parametri);
void printSepVarianti(PARAMETRI * parametri);
void printFineVarianti(PARAMETRI * parametri);

// FUNZIONE DI MARCATURA DEL TESTO CRITICO
void printInizioTestoCritico(short key, PARAMETRI * parametri);
void printFineTestoCritico(short key, PARAMETRI * parametri);

// FUNZIONE DI STAMPA DI UNA STRINGA DI NOTA DELL'EDITORE
void printNota(const char * nota, FILE * outFile);

// STAMPA IL CORPO DI UNA VARIANTE E RITORNA IL NUMERO DI TESTIMONI DI RIFERIMENTO
short printCorpoVariante(struct testo * lezione,
			 struct testo * note,
			 struct testo * sigle,
			 PARAMETRI    * parametri,
			 bool           duplex,
			 bool           multiplex,
			 bool           nested,
			 int            nVar);

// FUNZIONE PER TRASFORMARE LA VARIANTE DA BANALE IN SERIA
bool opzioneBanale();

// FUNZIONE PER VEDERE L'ESTRATTO DI UN TESTIMONE
char * opzioneTestEx();

// VERIFICA CHE ESISTA IL TESTIMONE DI RIFERIMENTO NELLE SIGLE
bool findTestRif(struct testo * sigle, char * testRif);

// VERIFICA CHE ESISTA IL TESTIMONE DI RIFERIMENTO NELLE SIGLE - no manus alterae
bool findTestRifNOMANUSALTERAE(struct testo * sigle, char * testRif);

// RITORNA UN int DI VERIFICA DI ESISTENZA DEL TESTIMONE DI RIFERIMENTO NELLE SIGLE - no manus alterae
int findTestIdNOMANUSALTERAE(struct testo * sigle, char * testRif);

// CERCA LA SIGLA CORRISPONDENTE AL TESTIMONE RICHIESTO
bool findSigle(struct testo * sigle, char * testRif);

// VERIFICA CHE NELLE SIGLE NON ESISTANO TESTIMONI DIVERSI DA QUELLO DI RIFERIMENTO
bool unicTestRif(struct testo * sigle, char * testRif);

// VISITA L'ESTRATTO DI UN TESTIMONE
//void testoEx(struct testo * testo, PARAMETRI * parametri, struct testo * sigle);
