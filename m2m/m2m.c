/*

  METADATI

*/
/*

  file "m2m.c"
  PROGRAMMA PRINCIPALE

  m2m: traduttore da Mauro-TeX a M-TeX basato mvisit

  eLabor sc
  (ex. Laboratorio Informatico del Consorzio Sociale "Polis")
  via G. Garibaldi 33, 56127 Pisa - Italia
  <http://elabor.homelinux.org>
  <info@elabor.homelinux.org>

  responsabile sviluppo:
  Paolo Mascellani (paolo@elabor.homelinux.org)

  coordinatore dei progetti "Maurolico" e "Fibonacci2021":
  Pier Daniele Napolitani (napolita@dm.unipi.it)

*/
/*

  LICENZA

*/
/*

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
/*

  DEFINIZIONI C

*/
#ifndef __M2M__
/*              \
\\   define    //
 \            */
#define __M2M__
//define precede_un nome_di_macro seguito da ''token'' di cui e' abbreviazione.
#define             FILEEXTENSION            ".二.tex"
//define crea____un sinonimo_____ di________ ''un       frammento  di      codice''
#define             VERSIONSTRING            "versione BRANCH del 26 Novembre 2020" 
// (see: https://gcc.gnu.org/onlinedocs/cpp/Object-like-Macros.html#index-_0023define).
/*

  DIRETTIVE DI PREPROCESSAMENTO C

*/
// Declarations and macro definitions 
// to be shared between 
// several source files
// (see: https://gcc.gnu.org/onlinedocs/cpp/Header-Files.html#Header-Files).
// The preprocessor looks for 
// header files included 
// by the quote form 
// of the directive ‘#include‘ 
#include <stdio.h>
#include "mparse.h"
#include "mparsey.h"
// first relative to the directory 
// of the current file
//, and then in a preconfigured list 
// of standard system directories;
// for the angle-bracket form 
#include <stdlib.h>
#include <string.h>
// preprocessor’s defsault behavior 
// is to look only 
// in the standard 
// system directories.
// (see: https://gcc.gnu.org/onlinedocs/cpp/Search-Path.html)
/*

  PARAMETRI   ZZAZIONE DELLA RICERCA

*/
// 
// ALIAS view
//
class PARAMETRI {
  public        :
  long   lastKey;
  
  bool           XY;
  bool           YX;
  long           XX;
  long           YY;
  const char   * ZZ;
  short          CY;
  short          CX;
  short          CZ;

  short nProp;       // NUMERO DI PROPOSIZIONE
  short nUnit;       // NUMERO DI UNIT
  short nSec;        // NUMERO DI SEZIONE
  short nSubsec;     // NUMERO DI SOTTOSEZIONE
  short nSSubsec;    // NUMERO DI SOTTOSOTTOSEZIONE
  short nAdnot;      // NUMERO DI ANNOTAZIONE
  short nComm;       // NUMERO DI COMMENTO
  short nFootnote;   // NUMERO DI NOTA A PIEDE PAGINA
  short nVV;         // NUMERO DI NOTA
  short nFig;        // NUMERO DI FIGURA
  bool  nota;        // TESTO INTERNO AD UNA NOTA
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
  this->lastKey = -1;
  
    this->XX = 1           ;
    this->YY =   0         ;
    this->ZZ =     ""      ;
    this->XY =        0    ;
    this->YX =          0  ;
    this->CX =            0;
    this->CY =            0;
    this->CZ =            0;
    
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
    this->banale    = false;
    this->testimone = "";
    this->outFile   = NULL;
    this->label     = NULL;
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
/*

  Funzione: ESPLORAZIONE DI QUALCOSA USANDO STRUMENTI CHE GENERANO INFORMAZIONI
  Sintassi: INFORMAZIONI esplorazioneQUALCOSA(STRUMENTI)
  Figurato: IL METODO OPPORTUNO DI TRATTARE IL NODO CORRENTE
  Metafora: QUALSIASI PEZZO DEL PUZZLE CHE TROVO, SO IMMEDIATAMENTE DOVE METTERLO

*/
// esplorazione UN ELEMENTO DI TESTO PER STAMPARE
void node2node(
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle);

// esplorazione LA KEY DI UN ELEMENTO DI TESTO
void esplorazioneKey(
  short          key,
  long           ln,
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle);

// esplorazione UNA VV
void esplorazioneVV(
  short        key,
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle);

// esplorazione DI UNA LISTA DI VARIANTI E RITORNA IL NUMERO DI TESTIMONI
short esplorazioneVarianti(
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle,
  bool           nested,
  bool           duplex,
  bool           multiplex,
  int            numero);

// esplorazione UNA VARIANTE E RITORNA UNA CONFERMA DI ESISTENZA DELLA VARIANTE
bool esplorazioneVariante(
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle,
  bool           nested,
  bool           duplex,
  bool           multiplex,
  int            numero,
  short        * nTest);

// esplorazione LE SIGLE DI UNA VARIANTE
short esplorazioneSigle(struct testo * sigle, bool noNota, FILE * outFile);
// esplorazione I POSTSCRIPT DI UNA VARIANTE
void esplorazionePostscript(struct testo * sigle,
		     struct testo * nota,
		     PARAMETRI    * view,
		     bool           postscript);
// FUNZIONE DI STAMPA DI UN RIFERIMENTO AD UNA UNIT
void printUnitRef(const char * opera, const char * etichetta, PARAMETRI * view);
void printUnitRef(short nUnit, PARAMETRI * view);
// STAMPA I POSTSCRIPT DI UNA VARIANTE
void printPostscript(struct testo * sigle, struct testo * nota, PARAMETRI * view);
// RITORNA UN int DI VERIFICA DI ESISTENZA DEL TESTIMONE DI RIFERIMENTO NELLE SIGLE - no manus alterae
bool trovaTestimoneNMA(struct testo * sigle, char * testRif);
/*

  VARIABILI GLOBALI

*/
// DI SALVATAGGIO DEL NOME FILE
static char    * latexFileName;
// DI SALVATAGGIO DEL NOME TESTIMONE
// OTTENUTO DA <testimone> 
// PER L'OPZIONE -t
static char    * testEx ;
// PER ENUMERAZIONE DI SPAZIATURE
static short   * nSpaces;
/*

  FUNZIONI DI STAMPA DIRETTA
  (FILE <-- ELEMENTO C)

*/ 
// IDENTIFICATE DAL TIPO DI N-gramma
// (See: https://it.wikipedia.org/wiki/N-gramma)
// DIPENDENTI DA:
//- PARAMETRI  *
//- const char *
void stringa2outfile(    // è una funzione locale
  PARAMETRI    * view,// è un elemento globale
  const char   * C_str // è una stringa generica
  ){ // Elemento C = N-gramma composto da String
  if (C_str != NULL ) { fprintf(view->outFile, "%s", C_str); }
}
// DIPENDENTI DA:
//- PARAMETRI  *
//- int
void nongint2outfile(     // è una funzione locale
  PARAMETRI    * view,// è un elemento globale
  int            C_dgt   // è un intero generico
  ){ // Elemento C = N-gramma composto da Digit
  if (C_dgt         ) { fprintf(view->outFile, "%d", C_dgt); }
}
// DIPENDENTI DA:
//- PARAMETRI  *
//- long int
void longint2outfile(    // è una funzione locale
  PARAMETRI    * view,// è un elemento globale
  long int       C_lnt // è un 'long int'
  ){ // Elemento C = N-gramma composto da long int
  if (C_lnt         ) { fprintf(view->outFile, "%ld", C_lnt); }
}
/*

  FUNZIONE DI CONVERSIONE 
  DA STAMPA DIRETTA A INDIRETTA
  IDENTIFICATE DAL TIPO DI CARATTERE
  (ELEMENTO C <-- MORFEMA)

*/
/*

  FUNZIONI DI STAMPA INDIRETTA
  IDENTIFICATE DAL TIPO DI CARATTERE
  (ELEMENTO C <-- MORFEMA)

*/
// DIPENDENTI DA:
//- PARAMETRI  *
void spaziatura2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // L'inserimento di un spazio
  stringa2outfile(view, " ");
}
void aperturatonda2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // L'apertura di una parentesi tonda
  stringa2outfile(view, "(");
}
void chiusuratonda2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La chiusura di una parentesi tonda
  stringa2outfile(view, ")");
}
void aperturaquadra2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // L'apertura di una parentesi quadra
  stringa2outfile(view, "[");
}
void chiusuraquadra2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La chiusura di una parentesi quadra
  stringa2outfile(view, "]");
}
void aperturagraffa2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // L'apertura di una parentesi graffa
  stringa2outfile(view, "{");
}
void chiusuragraffa2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La chiusura di una parentesi graffa
  stringa2outfile(view, "}");
}
void accentuazione2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La grafìa di un accento circonflesso
  stringa2outfile(view, "^");
}
void trattino2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La grafìa di un tratto d'unione
  stringa2outfile(view, "-");
}
void legatura2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La grafìa di una E commerciale
  stringa2outfile(view, "&");
}
void tilde2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // Il carattere tilde
  stringa2outfile(view, "~");
}
void esplicazione2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // I due punti
  stringa2outfile(view, ":");
}
void minoranza2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // Il simbolo di minore
  stringa2outfile(view, "<");
}
void maggioranza2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // Il simbolo di maggiore
  stringa2outfile(view, ">");
}
void pausamedia2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // Il simbolo del punto e virgola
  stringa2outfile(view, ";");
}
void pausabreve2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // Il simbolo della virgola
  stringa2outfile(view, ",");
}
void backtick2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La grafìa di un accento grave
  stringa2outfile(view, "`");
}
void slash2stringa(
  PARAMETRI * view
  ){ // N-GRAMMA
    // La grafìa di una barra obliqua
  stringa2outfile(view, "/");
}
// INFINE I CARATTERI MAG1CI
void acapo2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // L'andare accapo
  stringa2outfile(view, "\n");
}
void elusione2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA :: ESCAPE
    // La grafìa di una barra inversa
  stringa2outfile(view, "\\");
}
void apicazione2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La grafìa di un apice singolo
  stringa2outfile(view, "\'");
}
void quotazione2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La grafìa di un apice doppio
  stringa2outfile(view, "\"");
}
//////////////////////////////////////
void dueapicazione2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La grafìa di due apici singolo
  apicazione2stringa(view);
  apicazione2stringa(view);
}
void duebacktick2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La grafìa di due barre inverse
  backtick2stringa(view);
  backtick2stringa(view);
}
void duetrattino2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La grafìa di due trattini
  trattino2stringa(view);
  trattino2stringa(view);
}
void dueslash2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La grafìa di due barre inverse
  elusione2stringa(view);
  elusione2stringa(view);
}

void tretrattino2stringa(
  PARAMETRI    * view
  ){ // N-GRAMMA
    // La grafìa di un accento grave
  trattino2stringa(view);
  trattino2stringa(view);
  trattino2stringa(view);
}
/*

  FUNZIONI DI STAMPA COMPOSITA
  (INDIRETTA E/O DIRETTA)
  IDENTIFICATE DAL TIPO DI ENTITA'
  (ELEMENTO C <-- MORFEMA)

*/
// DIPENDENTI DA:
//- PARAMETRI  *
//- const char *
void macroelusione2stringa(
  PARAMETRI    * view, 
  const char   * macro
  ){ // N-GRAMMA
    // \macro
  elusione2stringa(view);
  stringa2outfile(view, macro);
}
void ingraffamento2stringa(
  PARAMETRI    * view, 
  const char   * token
  ){ // N-GRAMMA
    // {token}
  aperturagraffa2stringa(view);
  stringa2outfile(view, token);
  chiusuragraffa2stringa(view);
}
void inquadramento2stringa(
  PARAMETRI    * view, 
  const char   * token
  ){ // N-GRAMMA
    // [token]
  aperturaquadra2stringa(view);
  stringa2outfile(view, token);
  chiusuraquadra2stringa(view);
}
// DIPENDENTI DA:
//- PARAMETRI  *
//- const char *
//- const char *
void macroingraffamento2stringa(
  PARAMETRI    * view, 
  const char   * macro, 
  const char   * token
  ){ // N-GRAMMA
    // \macro{token}
  macroelusione2stringa(view, macro);
  ingraffamento2stringa(view, token);
}
void macroinquadramento2stringa(
  PARAMETRI    * view, 
  const char   * macro, 
  const char   * token
  ){ // N-GRAMMA
    // \macro[token]
  macroelusione2stringa(view, macro);
  inquadramento2stringa(view, token);
}
void macroinquadrabile2stringa(
  PARAMETRI    * view,
  const char   * macro,
  const char   * token
  ){ // N-GRAMMA
    // \macro[token]
   // \macro
  if (macro != NULL) {macroelusione2stringa(view, macro);}
  if (token != NULL) {inquadramento2stringa(view, token);}
}
/*

  FUNZIONI DI STAMPA RICORSIVA
  IDENTIFICATE DAL TIPO DI STRUTTURA
  (ELEMENTO C <-- MORFEMA)

*/
// DIPENDENTI DA:
//- testo      *
//- PARAMETRI  *
//- const char *
void argomentazioni2stringa(
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle
  ){ // N-GRAMMA
    // {node->testo1}{node->testo2->testo1}..
   // {node->testo1}argomentazioni2stringa(node->testo2,view,sigle)
  if (node != NULL)
  {
   aperturagraffa2stringa(view);
    if (node->testo1)
    {
      //
      node2node(node->testo1, view, sigle);
    }
    chiusuragraffa2stringa(view);
    if (node->testo2) {
      argomentazioni2stringa(node->testo2, view, sigle);
    }
  }
}
// DIPENDENTI DA:
//- PARAMETRI  *
//- testo      *
void macrogiaelusa2stringa(
  PARAMETRI    * view, 
  struct testo * node
  ){ // N-GRAMMA
    // node->lexema
  if (node->lexema) { stringa2outfile(view, node->lexema); }
}
// DIPENDENTI DA:
//- PARAMETRI  *
//- const char *
//- testo      *
void macrowordsquared2stringa(
  PARAMETRI    * view, 
  const char   * macro,
  struct testo * node
  ){ // N-GRAMMA
    // \macro[node->lexema]
  if( macro && node->lexema ){ macroinquadrabile2stringa( view , macro , node->lexema ); }
}
// DIPENDENTI DA:
//- const char *
//- testo      *
//- PARAMETRI  *
//- const char *
void macroargomentauno2stringa(
  const char   * macro,
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle
  ){ // N-GRAMMA
    // macro{node->testo1}  
  macrogiaelusa2stringa(view, node);
  aperturagraffa2stringa(view); 
  if (node  != NULL)                    { 
  if (node->testo1 )                    { 
  node2node(node->testo1, view, sigle); } 
  chiusuragraffa2stringa(view);         }
}
void macroargomentaunoodue2stringa(
  const char   * macro,
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle
  ){ // N-GRAMMA
    // macro{node->testo1->testo1}{<-node->testo1->testo2->}  
  macrogiaelusa2stringa(view, node);
  aperturagraffa2stringa(view); 
  if (node  != NULL)                    { 
  if (node->testo1 )                    { 
  node2node(node->testo1, view, sigle); } 
  chiusuragraffa2stringa(view);         }
}
// DIPENDENTI DA:
//- testo      *
//- PARAMETRI  *
//- testo      *
void macrosimple2stringa(
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle
  ){ // N-GRAMMA
    // \node->lexema{{node->testo1}{node->testo2->testo1}..}
   // \node->lexema{argomentazioni2stringa(node,view,sigle)}
  macroelusione2stringa(view, node->lexema);
  aperturagraffa2stringa(view);
  //
  argomentazioni2stringa(node->testo2, view, sigle);
  chiusuragraffa2stringa(view);
}
// DIPENDENTI DA:
//- const char *
//- const char *
//- PARAMETRI  *
//- testo      *
void macrowordbra2stringa(
  PARAMETRI    * view,
  const char   * macro,
  const char   * token,
  struct testo * sigle
  ){ // N-GRAMMA
    // \macro{token}
  if (macro != NULL) {macroelusione2stringa(view, macro);}
  if (token != NULL) {ingraffamento2stringa(view, token);}
}
// DIPENDENTI DA:
//- testo      *
//- PARAMETRI  *
//- testo      *
//- const char *
void macromain2stringa(
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle,
  const char   * macro
  ){ // N-GRAMMA
    // \macro[node->lexema]{{node->testo1}{node->testo2->testo1}..}
   // \macro[node->lexema]{argomentazioni2stringa(node,view,sigle)}
  if (macro != NULL && node != NULL) {
    macroelusione2stringa(view, macro);
    // UN token RAPPRESENTANTE
    // UN'OPZIONE OPZIONALE
    if (node->lexema != NULL) {
      inquadramento2stringa(view, node->lexema);
    }
    // UN TESTO RAPPRESENTANTE
    // IL CONTENUTO DELL'ARGOMENTAZIONE PRINCIPALE,
    aperturagraffa2stringa(view);
    //
    argomentazioni2stringa(node, view, sigle);
    // OVVERO UNA SERIE DI POSSIBILI ARGOMENTAZIONI
    // DI CUI A PRIORI NON CONOSCO LA PROFONDITA'
    // MA ALMENO UNA CI ''DOVREBBE'' ESSERE
    chiusuragraffa2stringa(view);
  }
}
// DIPENDENTI DA:
//- testo      *
//- PARAMETRI  *
//- testo      *
//- const char *
void macrorefered2stringa(
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle,
  const char   * macro
  ){ // N-GRAMMA
    // \macro[node->lexema]{node->testo1}
   // \macro[node->lexema]{argomentazioni2stringa(node,view,sigle)}
  if (macro != NULL && node != NULL) {
    macroelusione2stringa(view, macro);
    // UN token RAPPRESENTANTE
    // UN'OPZIONE OPZIONALE
    if (node->lexema != NULL) {
      inquadramento2stringa(view, node->lexema);
    }
    // UN TESTO RAPPRESENTANTE
    // IL CONTENUTO DELL'ARGOMENTAZIONE PRINCIPALE,
    if (node->testo1 != NULL) {
      aperturagraffa2stringa(view);
      //
      node2node(node->testo1, view, sigle);
      chiusuragraffa2stringa(view);
    }
    // OVVERO UNA SERIE DI POSSIBILI ARGOMENTAZIONI
    // DI CUI A PRIORI NON CONOSCO LA PROFONDITA'
    // MA ALMENO UNA CI ''DOVREBBE'' ESSERE
  }
}
// DIPENDENTI DA:
//- PARAMETRI  *
//- const char *
//- testo      *
void macrodiffered2stringa(
  PARAMETRI    * view,
  const char   * macro,
  struct testo * lexem
  ){ // N-GRAMMA
    // \macro[lexem]{token}
  if (macro != NULL) {macroelusione2stringa(view, macro);}
  if (lexem->lexema) {inquadramento2stringa(view, lexem->lexema);}
}
// DIPENDENTI DA:
//- PARAMETRI  *
//- const char *
//- const char *
//- const char *
void macrodefered2stringa(
  PARAMETRI    * view,
  const char   * macro,
  const char   * lexem,
  const char   * token 
  ){ // N-GRAMMA
    // \macro[lexem]{token}
  if (macro != NULL) {macroelusione2stringa(view, macro);}
  if (lexem != NULL) {inquadramento2stringa(view, lexem);}
  if (token != NULL) {ingraffamento2stringa(view, token);}
}
// DIPENDENTI DA:
//- testo      *
//- PARAMETRI  *
//- testo      *
//- const char *
void macroessenza2stringa(
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle,
  const char   * macro
  ){ // N-GRAMMA
    // \macro[node->lexema]{node->testo1}
   // \macro[node->lexema]{argomentazioni2stringa(node,view,sigle)}
  if (macro != NULL && node != NULL) {
    macroelusione2stringa(view, macro);
    // UN token RAPPRESENTANTE
    // UN'OPZIONE OPZIONALE
    if (node->lexema != NULL) {
      inquadramento2stringa(view, node->lexema);
    }
    // UN TESTO RAPPRESENTANTE
    // IL CONTENUTO DELL'ARGOMENTAZIONE PRINCIPALE,
    if (node->testo1 != NULL) {
      aperturagraffa2stringa(view);
      //
      node2node(node->testo2, view, sigle);
      chiusuragraffa2stringa(view);
    }
    // OVVERO UNA SERIE DI POSSIBILI ARGOMENTAZIONI
    // DI CUI A PRIORI NON CONOSCO LA PROFONDITA'
    // MA ALMENO UNA CI ''DOVREBBE'' ESSERE
  }
}
// DIPENDENTI DA:
//- PARAMETRI  *
//- const char *
//- const char *
//- testo      *
//- testo      *
void macrowordbratestobra2stringa(
  PARAMETRI    * view,
  const char   * macro,
  const char   * token,
  struct testo * node,
  struct testo * sigle
  ){ // N-GRAMMA
    // \macro{lexem}{node2node}
  macroelusione2stringa(view, macro);
  ingraffamento2stringa(view, token); 
  aperturagraffa2stringa(view);
  //
  node2node(node, view, sigle);
  chiusuragraffa2stringa(view);
}
void macroopzionaunoargomentadue2stringa(
  PARAMETRI    * P ,
  const char   * M ,
  struct testo * A ,
  struct testo * D )
  { /*\M                   {                 }
   //|  [A->testo1->testo1]                   
   //|                      A->testo1->testo2 
    */
  if( A->testo1 ){
    macroelusione2stringa( P, M );
    if( A->testo1->testo1 ){
      aperturaquadra2stringa( P );
      //
      node2node( A->testo1->testo1 , P, D);
      chiusuraquadra2stringa( P );
    } else {
    /*TODO*/
    }
    if ( A->testo1->testo2 )
      {
      aperturagraffa2stringa(P);
      //
      node2node(A->testo1->testo2, P, D);
      chiusuragraffa2stringa(P);
    } else {
    /*TODO*/
    }
  } else {
  /*TODO*/
  }
}
void macrovariante2stringa ( PARAMETRI * P , const char * M , struct testo * Z , struct testo * D )
/* \M       {                 }
//|  [At1l]                    
//|          At1t1t1->/At1t1t1t2t1/
//|        {                   {A->testo1->testo2}}
*/
{ struct testo * A = Z;
  bool questavarianterigurdailtestimonesceltoconlopzionet = true;
  // isNodeFeaturenotNULL
  bool An = false;
  bool At1l = false;    // Opzione
  bool At1 = false;    //
  bool At1t1 = false;   // Siglae:
  bool At1t1t1 = false; // sigla1=node->lexema
  bool At1t1t2 = false; // sigla2=node->lexema
//     At1t1t1t2        // sigla3=node->lexema se At1t1t1->lexema = NULL
//     At1t1t1t1        // sigla3=node->lexema altrimenti (se NULL, fine)

//bool At1t1t2t1 = false;
//bool At1t1t2t1t1 = false;
  bool At1t1t2t2 = false;
  bool At1t2 = false;
  bool At1t2t1 = false; // Lectio
  bool At1t2t2 = false;
  bool At1t2t1t1 = false;
  bool At1t2t1t2 = false;
  bool At1t1t2t1 = false;
  bool At1t1t1t2 = false;
//bool At2l = false;
  bool At2 = false;    //
  bool At2t1 = false;
  bool At2t1t1 = false;
  bool At2t1t2 = false;
  bool At2t2 = false;
  bool At2t2t1 = false;
//bool At2t2t2 = false;
  
  if ( A )
  { if ( A->testo1 )
    { At1 = true;
      if ( A->testo1->lexema ) 
      {   At1l    = true;
      } else { }
      if ( A->testo1->testo1 ) 
      {   At1t1   = true;
        if ( A->testo1->testo1->testo1 ) 
        { At1t1t1 = true;
          /*if ( A->testo1->testo1->testo1->testo1 ) 
          { At1t1t1t1 = true;
          } else { }*/
          if ( A->testo1->testo1->testo1->testo2 ) 
          { At1t1t1t2 = true;
          } else { }
        } else { }
        if ( A->testo1->testo1->testo2 ) 
        { At1t1t2 = true;
          if ( A->testo1->testo1->testo2->testo1 ) 
          { At1t1t2t1 = true;
          } else { }
          /*if ( A->testo1->testo1->testo2->testo1->testo1 ) 
          { At1t1t2t1t1 = true;
          } else { }*/
          if ( A->testo1->testo1->testo2->testo2 ) 
          { At1t1t2t2 = true;
          } else { }
        } else { }
      } else { }
      if ( A->testo1->testo2 )
      { At1t2 = true;
        if ( A->testo1->testo2->testo1)
        { At1t2t1 = true;
          if (A->testo1->testo2->testo1->testo1)
          { At1t2t1t1 = true;
          } else { }
          if (A->testo1->testo2->testo1->testo2)
          { At1t2t1t2 = true;
          } else { }
        } else { }
        if ( A->testo1->testo2->testo2)
        { At1t2t2 = true;
        } else { }
      } else { }
    } else { }
    if ( A->testo2 )
    { /* if ( A->testo2->lexema ) 
      { At2l      = true;
      } else { } */
      At2 = true; 
      if ( A->testo2->testo1 ) 
      { At2t1     = true;
        if ( A->testo2->testo1->testo1 ) 
        { At2t1t1     = true;
        } else { At2t1t1     = false; }
        if ( A->testo2->testo1->testo2 ) 
        { At2t1t2     = true;
        } else { At2t1t2     = false; }
      } else { }
      if ( A->testo2->testo2 ) 
      { At2t2  = true;
        if ( A->testo2->testo2->testo1 ) 
        { At2t2t1  = true;
        } else { }
      } else { }
    } else { At1 = false; }
  } else { }
  
  

  
  
  if ( questavarianterigurdailtestimonesceltoconlopzionet ) { 
  /* MACRO */
  macroelusione2stringa( P, M ) ;
  /* OPZIONE */
  if ( At1l ) { inquadramento2stringa( P, A->testo1->lexema  ) ; }
  /* VARIANTE (START) */
  aperturagraffa2stringa( P ) ;
  
  bool An = false;
  bool At1l = false;    // Opzione
  bool At1 = false;    //
//bool At2l = false;
  bool At2 = false;    //
  bool At2t1 = false;
  bool At2t1t1 = false;
  bool At2t1t2 = false;
  bool At2t2 = false;
  bool At2t2t1 = false;
//bool At2t2t2 = false
  if ( A )
  { An = true;
    if ( A->testo1 )
    { At1 = true;
      if ( A->testo1->lexema ) 
      {   At1l    = true;
      } else { At1l    = false; }
    } else { At2 = false; }
    if ( A->testo2 )
    { /* if ( A->testo2->lexema ) 
      { At2l      = true;
      } else { } */
      At2 = true;
      if ( A->testo2->testo1 ) 
      { At2t1     = true;
        if ( A->testo2->testo1->testo1 ) 
        { At2t1t1     = true;
        } else { At2t1t1     = false; }
        if ( A->testo2->testo1->testo2 ) 
        { At2t1t2     = true;
        } else { At2t1t2     = false; }
      } else { At2t1     = false; }
      if ( A->testo2->testo2 ) 
      { At2t2  = true;
        if ( A->testo2->testo2->testo1 ) 
        { At2t2t1  = true;
        } else { At2t2t1  = false; }
      } else { At2t2  = false; }
    } else { At2 = false; }
  } else { An = false; }
  if ( At1 ) { A=Z->testo1; } else { A=NULL; };
  // ITERAZIONE SUI CAMPI
  while (A) {
  /* ELEMENTI AUSILIARI */
  struct testo * temporarynode; // Un nodo temporaneo per iterare le sigle
  temporarynode = A->testo1->testo1->testo1; // inizializzato al primo nodo con siglum
  bool hoappenainiziatoilwhile = true; // il while sta per iniziare
  bool questonodohalexemanullo = true; // il primo lexema è il primo siglum
  bool questavarianterigurdailtestimonesceltoconlopzionet = true;
  // isNodeFeaturenotNULL
  bool At1t1 = false;   // Siglae:
  bool At1t1t1 = false; // sigla1=node->lexema
  bool At1t1t2 = false; // sigla2=node->lexema
//     At1t1t1t2        // sigla3=node->lexema se At1t1t1->lexema = NULL
//     At1t1t1t1        // sigla3=node->lexema altrimenti (se NULL, fine)

//bool At1t1t2t1 = false;
//bool At1t1t2t1t1 = false;
  bool At1t1t2t2 = false;
  bool At1t2 = false;
  bool At1t2t1 = false; // Lectio
  bool At1t2t1t1 = false;
  bool At1t2t1t1t1 = false;
  bool At1t2t1t1t2 = false;
  bool At1t2t1t2t1 = false;
  bool At1t2t1t2t2 = false;
  bool At1t2t1t2t2t1 = false;
  bool At1t2t1t2t2t1t2 = false;
  bool At1t2t2t2t1t2 = false;
  bool At1t2t1t2 = false;
  bool At1t2t2 = false;
  bool At1t2t2t2 = false;
  bool At1t2t2t2t1 = false;
  bool At1t2t2t2t2 = false;
  bool At1t2t2t1 = false;
  bool At1t2t2t1t1 = false;
  bool At1t2t2t1t2 = false;
  bool At1t1t2t1 = false;
  bool At1t1t1t2 = false;;

      if ( A->testo1 ) 
      {   At1   = true;
        if ( A->testo1->testo1 ) 
        { At1t1 = true;
          /*if ( A->testo1->testo1->testo1 ) 
          { At1t1t1t1 = true;
          } else { }*/
          if ( A->testo1->testo1->testo2 ) 
          { At1t1t2 = true;
          } else { At1t1t2 = false; }
        } else { At1t1 = false; }
        if ( A->testo1->testo2 ) 
        { At1t2 = true;
          if ( A->testo1->testo2->testo1 ) 
          { At1t2t1 = true;
            if ( A->testo1->testo2->testo1->testo1 ) 
            { At1t2t1t1 = true;
              if ( A->testo1->testo2->testo1->testo1->testo1 ) 
              { At1t2t1t1t1 = true;
              } else { }
              if ( A->testo1->testo2->testo1->testo1->testo2 ) 
              { At1t2t1t1t2 = true;
              } else { }
            } else { }
            if ( A->testo1->testo2->testo1->testo2 ) 
            { At1t2t1t2 = true;
              if ( A->testo1->testo2->testo1->testo2->testo1 ) 
              { At1t2t1t2t1 = true;
              } else { }
              if ( A->testo1->testo2->testo1->testo2->testo2 ) 
              { At1t2t1t2t2 = true;
                if ( A->testo1->testo2->testo1->testo2->testo2->testo1 ) 
                { At1t2t1t2t2t1 = true;
                } else { }
              } else { }
            } else { }
          } else { At1t2t2 = false; }
          if ( A->testo1->testo2->testo2 ) 
          { At1t2t2 = true;
            if ( A->testo1->testo2->testo2->testo1 ) 
            { At1t2t2t1 = true;
              if ( A->testo1->testo2->testo2->testo1->testo1 ) 
              { At1t2t2t1t1 = true;
              } else { }
              if ( A->testo1->testo2->testo2->testo1->testo2 ) 
              { At1t2t2t1t2 = true;
              } else { }
            } else { }
            if ( A->testo1->testo2->testo2->testo2 ) 
            { At1t2t2t2 = true;
              if ( A->testo1->testo2->testo2->testo2->testo1 ) 
              { At1t2t2t2t1 = true;
                if ( A->testo1->testo2->testo1->testo2->testo2->testo1->testo2 ) 
                { At1t2t1t2t2t1t2 = true;
                } else { }
              } else { }
              if ( A->testo1->testo2->testo2->testo2->testo2 ) 
              { At1t2t2t2t2 = true;
              } else { }
            } else { }
          } else { At1t2t2 = false; }
        } else { At1t2 = false; }
      } else { At1   = false; }
      if ( A->testo2 )
      { At2 = true;
        if ( A->testo2->testo1)
        { At2t1 = true;
          if (A->testo2->testo1->testo1)
          { At2t1t1 = true;
          } else { At2t1t1 = false; }
          if (A->testo2->testo1->testo2)
          { At2t1t2 = true;
          } else { At2t1t2 = false; }
        } else { At2t1 = false; }
        if ( A->testo2->testo2)
        { At2t2 = true;
        } else { At2t2 = false; }
      } else { At2 = false; }

  /* CAMPO (START) */
  // POSSO CONTROLLARE QUI SE C'E' UN ALTRO CAMPO
  //if ( At2t1t1 ) { aperturagraffa2stringa( P ) ; }
  // ESEMPIO DI UTILIZZO
  // IL COMANDO PRECEDENTE PERMETTE DI RACCHIUDERE
  // in una sola parentesi
  // sigla:opzione:lezione
  // nel caso in cui ci sia un solo campo
  // in questa variante
  // (vedi dopo quando la chiudo)
  aperturagraffa2stringa( P ) ;
  // CI SONO I SIGLAE
  if ( At1t1 ) { temporarynode = A->testo1->testo1; 
  while ( temporarynode ) {
  if ( hoappenainiziatoilwhile ) { hoappenainiziatoilwhile = false; } else { /* SEPARATORE DI SIGLAE (SOLO TRA DUE SIGLA, NON PRIMA UN SIGLA NE DOPO UN SIGLA) */ slash2stringa(P); };
  // SIGLA N.1,2,3,...N
  if ( temporarynode->lexema ) { stringa2outfile(P, temporarynode->lexema); } else { questonodohalexemanullo = true; }
  // COME PROSEGUO PER TROVARE GLI ALTRI SIGLA (USO IL while)
  if ( questonodohalexemanullo ) { if (temporarynode->testo2) { temporarynode = temporarynode->testo2; } else { temporarynode = NULL; }; } else { if (temporarynode->testo1) { temporarynode = temporarynode->testo1; } else { temporarynode = NULL; }; }; 
  }
  // DUEPUNTI
  esplicazione2stringa( P );
  // DUEPUNTI
  if ( At1t2t2t1 ) { node2node( A->testo1->testo2->testo2->testo1 , P, D );
  /* DUEPUNTI */ esplicazione2stringa( P ); }
  // LEZIONE
  if ( At1t2t1 ) { node2node( A->testo1->testo2->testo1 , P, D ); }
  }
  /* CAMPO (END) */
  // POSSO CONTROLLARE QUI SE C'E' UN ALTRO CAMPO
  //if ( At2t1t1 ) { chiusuragraffa2stringa( P ) ; }
  // ESEMPIO DI UTILIZZO
  // IL COMANDO PRECEDENTE PERMETTE DI RACCHIUDERE
  // in una sola parentesi
  // sigla:opzione:lezione
  // nel caso in cui ci sia un solo campo
  // in questa variante
  chiusuragraffa2stringa( P ) ;
  // PROSSIMO CAMPO
  if ( At2 ) { A=A->testo2; } else { A=NULL; }
  //hoappenainiziatoilwhile = true; // un altro giro di while sta per iniziare
  //questonodohalexemanullo = true; // il primo lexema sara' con il primo siglum
  }
  /* VARIANTE (END) */
  chiusuragraffa2stringa(P); 
  //}
  }
}
void macrotripla2stringa(
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle,
  const char   * macro
  ){ // N-GRAMMA
    // \macro{node->testo1}{node->testo2->testo1}{node->testo2->testo2}
  macroelusione2stringa(view, macro);
  aperturagraffa2stringa(view);
  //
  node2node(node->testo1, view, sigle);
  chiusuragraffa2stringa(view);
  aperturagraffa2stringa(view);
  //
  node2node(node->testo2->testo1, view, sigle);
  chiusuragraffa2stringa(view);
  aperturagraffa2stringa(view);
  //
  node2node(node->testo2->testo2, view, sigle);
  chiusuragraffa2stringa(view);
}
void macrotestobra2stringa(
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle,
  const char   * macro
  ){ // N-GRAMMA
    // \macro{node->testo1}
  macroelusione2stringa(view, macro);
  aperturagraffa2stringa(view);
  //
  node2node(node, view, sigle);
  chiusuragraffa2stringa(view);
}
void macroambiente2stringa(
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle,
  const char   * macro
  ){ // N-GRAMMA
    // \macro{node->testo1}
  macroelusione2stringa(view, macro);
  aperturagraffa2stringa(view);
  //
  node2node(node, view, sigle);
  chiusuragraffa2stringa(view);
  node2node(node->testo2, view, sigle);
}
//////////////////////////////////////////////////////////////////////////////////////
/*

  FUNZIONI PER OPZIONI GESTITE ESTERNAMENTE

*/
bool opzioneBanale(
){
  return false;
}
bool verboseWarning(
){
  return false;
}
void printNota(
  const char * C_String,
  FILE * outFile
  ){
  fprintf(outFile, "%s", C_String);
}
// FUNZIONE DI STAMPA DELLE CITAZIONI
void printCit(struct testo * node, PARAMETRI    * view, struct testo * sigle)
{//macromain2stringa(node, view, sigle, "Cit");
}
// FUNZIONE DI STAMPA DEI CASES
void printCases(
  short key,
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle
  ){
  //macrosimple2stringa(node, view, sigle);
}

bool checkLexema(
  const char   * lex1, 
  const char   * lex2
  ){
  bool value;
  if (lex1 == lex2) {
    value = true;
  } else {
    if (lex1 == NULL || lex2 == NULL) {
      value = false;
    } else {
      value = (strcmp(lex1, lex2) == 0);
    }
  }
  return value;
}

bool checkTesto(
  struct testo * testo1, 
  struct testo * testo2
  ){
  bool value;
  if (testo1 == testo2) {
    value = true;
  } else {
    if (testo1 == NULL || testo2 == NULL) {
      value = false;
    } else {
      if (checkLexema(testo1->lexema, testo2->lexema)) {
        value = checkTesto(testo1->testo1, testo2->testo1) && 
                checkTesto(testo1->testo2, testo2->testo2);
      } else {
        value = false;
      }
    }
  }
  return value;
}

class Descrizione {

 public:
  struct testo * descrizione;
  Descrizione  * next;

};

bool checkDescrizione(struct testo * descrizione, Descrizione * descrizioni) {
  bool value;
  if (descrizioni == NULL) {
  value = false;
  } else {
  if (checkTesto(descrizione, descrizioni->descrizione)) {
    value = true;
  } else {
    value = checkDescrizione(descrizione, descrizioni->next);
  }
  }
  return value;
}

static Descrizione * descrizioni;

void addDescrizione(struct testo * descrizione) {
  Descrizione * nuova = new Descrizione();
  nuova->descrizione = descrizione;
  nuova->next = descrizioni;
  descrizioni = nuova;
}

void disposeDescrizioni(Descrizione * descrizioni) {
  if (descrizioni != NULL) {
    disposeDescrizioni(descrizioni->next);
    delete(descrizioni);
  }
}

void resetDescrizioni() {
  disposeDescrizioni(descrizioni);
  descrizioni = NULL;
}

bool checkDescrizione(struct testo * descrizione) {
  return checkDescrizione(descrizione, descrizioni);
}

// RITORNA IL TESTIMONE PER VEDERNE L'ESTRATTO SE RICHIESTO
char * opzioneTestEx() {
  return testEx;
}

// FUNZIONE CHE STAMPA LO HEADER DI RIGA DI UNA TABELLA
void printRowHeader(
  short key,
  PARAMETRI * view
  ){
  if (key == HLINEKEY) {
    esplorazioneKey(key, 0, NULL, view, NULL);
  }
}

// FUNZIONE CHE STAMPA IL FOOTER DI RIGA DI UNA TABELLA
void printRowFooter(
  short key,
  PARAMETRI * view
  ){
  if (key != HLINEKEY) {
    // Print
    elusione2stringa(view);
    elusione2stringa(view);
  }
    acapo2stringa(view);
}

// FUNZIONE CHE STAMPA LO HEADER DI UNA TABELLA
void printTableHeader(
  int key, 
  const char * colonne, 
  PARAMETRI * view
  ){
  
  
}

// FUNZIONE CHE STAMPA IL FOOTER DI UNA TABELLA
void printTableFooter(int key,PARAMETRI * view){}
// FUNZIONE CHE STAMPA IL PROLOGO DI UNA TABELLA
void printTableStart(int key,struct testo * node,PARAMETRI * view,struct testo * sigle){}
// FUNZIONE CHE STAMPA L'EPILOGO DI UNA TABELLA
void printTableFinish(int key,struct testo * node,PARAMETRI * view,struct testo * sigle){}

// FUNZIONE CHE STAMPA UN CAMPO DI UNA TABELLA
void printCampo(short          nRow,
        short          nCol,
        const char   * colonne,
        struct testo * campo,
        PARAMETRI    * view,
        struct testo * sigle) {
  
  if (nCol > 1) {
  legatura2stringa(view);
  }
  
  if (campo != NULL) {
  
    // GESTIONE INIZIALE MULTIROW E MULTICOL
    switch (campo->key) {
      case MULTICOLKEY:
        // Entità Mauroliciane come Caratteri Tipografici 1
        const char * EMcCT_0 = "mcol";
        int          EMcCT_1 = atoi(campo->lexema);
        // Print
        elusione2stringa(view);
        stringa2outfile(view, EMcCT_0);
        aperturagraffa2stringa(view);
        nongint2outfile(view, EMcCT_1);
        chiusuragraffa2stringa(view);
        //
        break;
    }
    
    switch (campo->key) {
      case CLINEKEY:
      case LEFTGRATABKEY:
      case RIGHTGRATABKEY:
      case LEFTANGTABKEY:
      case RIGHTANGTABKEY:
        esplorazioneKey(campo->key, 0, NULL, view, sigle);
        break;
      default:
        node2node(campo, view, sigle);
      break;
    }

  }
}

// STAMPA I TITOLI DELLE OPERE
void printTitleWork(struct testo *node, PARAMETRI * view, struct testo *sigle){
  fprintf(view->outFile, "\\Title{");
  node2node(node->testo1, view, sigle);
  fprintf(view->outFile, "}");
  fprintf(view->outFile, "{");
  if (node->testo2 != NULL) {
  node2node(node->testo2, view, sigle);
  }
  fprintf(view->outFile, "}");
}

// FUNZIONE DI STAMPA DELLE FRAZIONI
void printFrac(struct testo *node, PARAMETRI * view, struct testo *sigle){
  fprintf(view->outFile, "\\frac{");
  node2node(node->testo1, view, sigle);
  fprintf(view->outFile, "}{");
  node2node(node->testo2, view, sigle);
  fprintf(view->outFile, "}");
}

// FUNZIONE DI STAMPA DELL'ESPONENTE E DEL DEPONENTE
void printSupSub(struct testo *node, PARAMETRI * view, struct testo *sigle){
  fprintf(view->outFile, "\\hbox{$^{\\rm ");
  node2node(node->testo1, view, sigle);
  fprintf(view->outFile, "}_{\\rm ");
  node2node(node->testo2, view, sigle);
  fprintf(view->outFile, "}$}");
}

// STAMPA GLI ACCENTI
void printAccento(const char * lexema, const char * accento, FILE * outFile) {
  
  int i; // POSIZIONE DEL CARATTERE ACCENTATO
  
  // IGNORA L'EVENTUALE PARENTESI O BACKSLASH
  if (lexema[2] == '{' || lexema[2] == '\\') i = 3; else i = 2;
  
  // STAMPA IL CARATTERE COL SUO ACCENTO
  fprintf(outFile, "%s%c", accento, lexema[i]);
}

// STAMPA I LINK ALLE IMMAGINI
void printImage(char * path, char * alt, FILE * outFile) {
  fprintf(outFile, "<img src=\"%s\" alt=\"%s\">", path, alt);
}

// STAMPA LA SIGLA DI UN TESTIMONE
void printSigla(const char * sigla, FILE * outFile) {
  
  char str[100], c;
  
  if (strlen(sigla) > 0) {
    size_t len = strlen(sigla);
    char *tmp = new char[len];
    strncpy(tmp, sigla, len-1);
    tmp[len-1] = 0;
    c = sigla[strlen(sigla)-1];
    switch (c) {
      case 'm':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      // APICI
      sprintf(str, "%s%c", tmp, c);
      break;
      case '+':
      // CARATTERI NASCOSTI
      //strcpy(str, tmp);
      strcpy(str, sigla);
      break;
    default:
      // STANDARD
      strcpy(str, sigla);
  }
  delete tmp;
  // STAMPA DELLA SIGLA FORMATTATA
  printNota(str, outFile);
  }
}

void printTextGreek(struct testo * node, PARAMETRI * view, struct testo * sigle) {
  node2node(node, view, sigle);
}

// FUNZIONE DI STAMPA DELLE MANI
void printManus(
  struct testo *node, 
  PARAMETRI   * view, 
  struct testo *sigle
  ){
  macroingraffamento2stringa(view,"Manus", node->lexema);
  aperturagraffa2stringa(view);
  //
  node2node(node->testo1->testo1, view, sigle);
  chiusuragraffa2stringa(view);
  aperturagraffa2stringa(view);
  //
  node2node(node->testo1->testo2, view, sigle);
  chiusuragraffa2stringa(view);
  struct testo * style = node->testo2;
  if (style == NULL) {
    aperturagraffa2stringa(view);
    chiusuragraffa2stringa(view);
  } else {
    if (strlen(style->lexema) == 0) {
      aperturagraffa2stringa(view);
      chiusuragraffa2stringa(view);
    } else {
      ingraffamento2stringa(view, style->lexema);
    }
  }
  acapo2stringa(view);
}

// FUNZIONE DI STAMPA DI ALIAMANUS
void printAliaManus(
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle
  ){
  //macroopzionaunoargomentadue2stringa(view, "AliaManus", node, sigle);
}

// esplorazione UNA LISTA DI SIGLE DI TESTIMONI
short testimoni2stringa(
  struct testo * sigle, 
  bool           noNota, 
  FILE         * outFile
  ){
  short nTest = 0;
  if (sigle) {
    if (strcmp(sigle->lexema, "*") == 0) {
      if (noNota) {
	     printNota("*", outFile);
      }
    } else {
      printSigla(sigle->lexema, outFile);
      if (sigle->testo2) {
        printNota("/", outFile);
        nTest = 1 + testimoni2stringa(sigle->testo2, noNota, outFile);
      }
    }
  }
  return nTest;
}

// STAMPA UN TESTO DI NOTA DELL'EDITORE
void printTestoNota(
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle
  ){
  node2node(node, view, sigle);
}

// STAMPA IL CORPO DI UNA VARIANTE
bool printNotaVariante(
  struct testo * lezione,
  struct testo * nota,
  struct testo * sigle,
  PARAMETRI    * view,
  bool           duplex,
  bool           multiplex
  ){
  bool noNota = false; // INDICATORE DI NOTA VUOTA
  // SE LA NOTA (SECONDO SOTTOCAMPO) ESISTE
  if (nota) {
    esplicazione2stringa(view); //spostare nei singoli casi, serve sapere quando NON metterla
    // VERIFICA DEL TIPO DI NOTA
    switch (nota->key) {
      case ANTEDELKEY    : macrotestobra2stringa(nota->testo1, view, sigle, "ANTEDEL"); break;
      case POSTDELKEY    : macrotestobra2stringa(nota->testo1, view, sigle, "POSTDEL"); break;
      case DESCOMPLKEY   : macrotripla2stringa(nota->testo1, view, sigle, "DESCOMPL"); break; 
      case EDCOMPLKEY    : macrotripla2stringa(nota->testo1, view, sigle, "EDCOMPL"); break;
      case SEXKEY        : macrotripla2stringa(nota->testo1, view, sigle, "Sex"); break; 
      case CRKEY         : macroingraffamento2stringa(view, "CR", nota->lexema); break;
      case TRKEY         : macrotestobra2stringa(nota, view, sigle, "DES"); break;
      case EDKEY         : macrotestobra2stringa(nota, view, sigle, "ED"); break;
      case EXKEY         : macrotestobra2stringa(nota->testo1, view, sigle, "EX"); break;
      case LECKEY        : macrotestobra2stringa(nota->testo1, view, sigle, "LEC"); break;
      case POSTSCRIPTKEY : macroelusione2stringa(view, "POSTSCRIPT"); break;
      case INTERLKEY     : macroelusione2stringa(view, "INTERL"); break;
      case SUPRAKEY      : macroelusione2stringa(view, "SUPRA"); break;
      case MARGKEY       : macroelusione2stringa(view, "MARG"); break;
      case MARGSIGNKEY   : macroelusione2stringa(view, "MARGSIGN"); break;
      case PCKEY         : macroelusione2stringa(view, "PC"); break;
      //case REPKEY        : macroinquadramento2stringa(view, "REP", nota->lexema); break;
      
      default            : //
                           node2node(nota->testo1, view, sigle); break;
      }
    } else {
	 noNota = true;
  }
  // RITORNA L'INDICATORE DI NOTA VUOTA
  return noNota;
}

bool printNoteVariante(
  struct testo * lezione,
  struct testo * note,
  struct testo * sigle,
  PARAMETRI    * view,
  bool           duplex,
  bool           multiplex
  ){
  bool noNota = true;
  if (note) {
    noNota = printNotaVariante(lezione, note->testo1, sigle, view, duplex, multiplex);
    if (note->testo1->key != POSTSCRIPTKEY) {
      noNota = printNoteVariante(NULL, note->testo2, sigle, view, duplex, multiplex) && noNota;
    }
  }
  return noNota;
}

// esplorazione I POSTSCRIPT DI UNA VARIANTE
void esplorazionePostscript(struct testo * sigle, struct testo * note, PARAMETRI * view, bool postscript) {
  if (note) {
    if (note->testo1->key == POSTSCRIPTKEY) postscript = true;
    if (postscript) {
      printPostscript(sigle, note->testo1, view);
    }
    esplorazionePostscript(sigle, note->testo2, view, postscript);
  }
}

// STAMPA IL CORPO DI UNA VARIANTE
short printCorpoVariante(
  struct testo * lezione,
  struct testo * note,
  struct testo * sigle,
  PARAMETRI    * view,
  bool           duplex,
  bool           multiplex,
  bool           nested,
  int            nVar
  ){
  // INDICATORE DI NOTA VUOTA
  bool noNota = true;
  // NUMERO DI TESTIMONI IN QUESTO CORPO
  short nTest;
  // MANDO IN STAMPA LA LISTA DI SIGLE DI TESTIMONI
  nTest = testimoni2stringa(sigle, noNota, view->outFile);
  // CONTROLLO 
  if (note) {
	noNota = printNoteVariante(lezione, note, sigle, view, duplex, multiplex);
  }
  esplicazione2stringa(view);
  //
  node2node(lezione, view, sigle);
  //
  esplorazionePostscript(sigle, note, view, false);
  // RITORNA IL NUMERO DI TESTIMONI
  return nTest;
}

// STAMPA I POSTSCRIPT DI UNA VARIANTE
void printPostscript(struct testo * sigle, struct testo * nota, PARAMETRI * view) {
}

// STAMPA IL TESTO CRITICO
void printTestoCritico (struct testo * node,
            PARAMETRI * view,
            struct testo * sigle) {
  bool banale = view->banale;
  view->banale = true;
  //node2node(node->testo1->testo2->testo1, view, sigle);
  view->banale = banale;
}

void printOptions(struct testo * node, FILE * outFile, bool sep) {
  if (node) {
    printNota(node->lexema, outFile);
    printOptions(node->testo2, outFile, node->lexema[0] != '=');
  }
}

// FUNZIONI GENERICHE DEFINITE IN MVISIT

// FUNZIONE DI STAMPA DEI FOLIA
void printFoliumCommon(const char * testimone, const char * pagina, PARAMETRI   * view) {
}

void printFolium(const char * testimone, const char * pagina, PARAMETRI   * view, struct testo *sigle) {
}

// FUNZIONE DI STAMPA DELLE DATE
void printDate(struct testo *node, PARAMETRI   * view, struct testo *sigle) {
  node2node(node->testo1, view, sigle);
}

void printEnv(PARAMETRI * view, const char * env, bool inizio) {
  if (inizio) {
    acapo2stringa(view);
    macroingraffamento2stringa(view, "begin", env);
  } else {
    acapo2stringa(view);
    macroingraffamento2stringa(view, "end", env);
    acapo2stringa(view);
  }
}

void printMarkup(
  const char * macro,
  const char * style,
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle
  ){
  struct testo * lezione = node->testo1;
  struct testo * descrizione = node->testo2;
  /*
  if (style != NULL) {
    fprintf(view->outFile, "{\\%s}", style);
  }
  if (style != NULL) {
    fprintf(view->outFile, "}");
  }
  */
  if (!checkDescrizione(descrizione)) {
    addDescrizione(descrizione);
    macroelusione2stringa(view, macro);
    aperturagraffa2stringa(view);
    aperturagraffa2stringa(view);
    //
    node2node(lezione, view, sigle);
    chiusuragraffa2stringa(view);
    //
    node2node(descrizione, view, sigle);
    chiusuragraffa2stringa(view);
  }
}

// FUNZIONE DI STAMPA DI UNA MACRO GENERICA
void printKey(
  bool inizio,
  short          key,
  long           ln,
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle
  ){
  if (view->label == NULL || view->nota) {
    // IDENTIFICAZIONE DELLA MACRO
    switch (key) {
      default:
        break;
      case LEFTANG:
        if (inizio) {
          minoranza2stringa(view);
        } else {
          maggioranza2stringa(view);
        }
      break;
  case LUOGOKEY:
    //if (inizio) {printMarkup("Luogo", NULL, node, view, sigle);}
    break;
  case OPERAKEY:
    //if(inizio){printMarkup("Opera", NULL, node, view, sigle);}
    break;
  case NOMEKEY:
    //if (inizio) { printMarkup("Nome", NULL, node, view, sigle); }
    break;
  /*
  case EXTERNALDOCUMENT:
    if (inizio) {
      aperturagraffa2stringa(view);
      //
      node2node(node, view, sigle);
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case HLINEKEY:
    if (inizio) {
      macroelusione2stringa(view, "linea");
    }
    break;
  case CLINEKEY:
    if (inizio) {
      macroelusione2stringa(view, "regula");
    }
    break;
  case LEFTGRATABKEY:
    if (inizio) {
      macroelusione2stringa(view, "lgra");
    }
    break;
  case RIGHTGRATABKEY:
    if (inizio) {
      macroelusione2stringa(view, "rgra");
    }
    break;
  case LEFTANGTABKEY:
    if (inizio) {
      macroelusione2stringa(view, "lang");
    }
    break;
  case RIGHTANGTABKEY:
    if (inizio) {
      macroelusione2stringa(view, "rang");
    }
    break;
  */
  case CONGKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "Cong");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case COMMENTKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "Commenti");
      acapo2stringa(view);
    }
    break;
  case ANNOTAZKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "Annotazioni");
      acapo2stringa(view);
    }
    break;
  case COMMKEY:
    if (inizio) {
      macroelusione2stringa(view, "Comm");
      aperturagraffa2stringa(view);
      //
      node2node(node, view, sigle);
      chiusuragraffa2stringa(view);
    }
    break;
  case GREEKKEY:
    if (inizio) {
      macroelusione2stringa(view, "GG");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case SETLENGTH:
    //if (inizio) { macroelusione2stringa(view, "setlength"); acapo2stringa(view); }
    break;
  case MAKETITLEKEY:
    //if (inizio) { macroelusione2stringa(view, "maketitle"); acapo2stringa(view); }
    break;
  case PLKEY:
    if (inizio) {
      macroelusione2stringa(view, "POSTDEL");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case CLASSKEY:
    if (inizio) {
      macroelusione2stringa(view, "documentclass");
      aperturaquadra2stringa(view);
      printOptions(node, view->outFile, false);
      chiusuraquadra2stringa(view);
    }
    break;
  case DOCENVKEY:
    if (inizio) {
      // CERCO IL PROLOGO
      node2node(node->testo1, view, sigle);
      macroingraffamento2stringa(view, "begin", "document");
      acapo2stringa(view);
      // CERCO IL CORPO DEL document
      node2node(node->testo2, view, sigle);
      macroingraffamento2stringa(view, "end", "document");
      acapo2stringa(view);
    }
    break;
  case USEPACKAGE:
    if (inizio) {
      macroelusione2stringa(view, "usepackage");
      if (node->testo1) {
        aperturaquadra2stringa(view);
        printOptions(node->testo1, view->outFile, false);
        chiusuraquadra2stringa(view);
      }
      aperturagraffa2stringa(view);
      printOptions(node->testo2, view->outFile, false);
      chiusuragraffa2stringa(view);
    }
    acapo2stringa(view);
    break;
  case GEOMETRY:
    if (inizio) {
      macroelusione2stringa(view, "geometry");
      aperturagraffa2stringa(view);
      printOptions(node, view->outFile, false);
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case MANUSKEY:
    break;
  case FOLIUMMARGOPT:
    //if (inizio) {macroelusione2stringa(view, "FoliumInMargine");}
    break;
  case FOLIUMTESTOOPT:
    //if (inizio) {macroelusione2stringa(view, "FoliumInTesto");}
    break;
  case UNITKEY:
    //if (inizio) {printUnitRef(view->nUnit, view);}
    break;
  case LBKEY:
    if (!inizio) {
      macroingraffamento2stringa(view, "LB", node->lexema);
    }
    break;
  case CAPITOLOKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "Capitolo");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case SOTTOCAPITOLOKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "Sottocapitolo");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case TEOREMAKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "Teorema");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case ALITERKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "Aliter");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case LEMMAKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "Lemma");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case COROLLARIOKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "Corollario");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case SCOLIOKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "Scolio");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case ADDITIOKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "Additio");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case SECTIONKEY:
    if (inizio) {
      acapo2stringa(view);
      descrizioni = NULL;
      macroelusione2stringa(view, "section");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case SUBSECTIONKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "subsection");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case SSSECTIONKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "subsubsection");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case PARAGRAPHKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "paragraph");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
      acapo2stringa(view);
    }
    break;
  case LABELKEY:
    //if (inizio) {macroingraffamento2stringa(view, "label", node->lexema);}
    break;
  case LETTERLABELKEY:
    if (inizio) {
      macroingraffamento2stringa(view, "letterlabel", node->lexema);
      ingraffamento2stringa(view, node->testo1->lexema);
    }
    break;
  case CITEKEY:
    //if (inizio) {macroingraffamento2stringa(view, "cite", node->lexema);}
    break;
  case MCITEKEY:
    //if (inizio) {macroingraffamento2stringa(view, "maurocite", node->lexema); aperturagraffa2stringa(view); node2node(node, view, sigle); } else { chiusuragraffa2stringa(view); }
    break;
  case VSPACEKEY:
    if (inizio) {
      macroelusione2stringa(view, "vspace");
      aperturagraffa2stringa(view);
      //
      node2node(node, view, sigle);
      chiusuragraffa2stringa(view);
    }
    break;
  case REFKEY:
    if (inizio) {
      macroelusione2stringa(view, "ref");
      aperturagraffa2stringa(view);
      //
      node2node(node, view, sigle);
      chiusuragraffa2stringa(view);
    }
    break;
  case PAGEREFKEY:
    if (inizio) {
      macroelusione2stringa(view, "pageref");
      aperturagraffa2stringa(view);
      //
      node2node(node, view, sigle);
      chiusuragraffa2stringa(view);
    }
    break;
  case ':':
    if (inizio) {
      esplicazione2stringa(view);
    }
    break;
  case OMKEY:
    if (inizio) {
      macroelusione2stringa(view, "OM");
    }
    break;
  case DELKEY:
    if (inizio) {
      macroelusione2stringa(view, "DEL");
    }
    break;
  case EXPKEY:
    if (inizio) {
      macroelusione2stringa(view, "EXP");
    }
    break;
  case OMLACKEY:
    if (inizio) {
      macroelusione2stringa(view, "OMLAC");
    }
    break;
  case NLKEY:
    if (inizio) {
      macroelusione2stringa(view, "NL");
    }
    break;
  case LACMKEY:
    if (inizio) {
      macroelusione2stringa(view, "LACm");
    }
  case LACSKEY:
    if (inizio) {
      macroelusione2stringa(view, "LACs");
    }
    break;
  case LACCKEY:
    if (inizio) {
      macroelusione2stringa(view, "LACc");
    }
    break;
  case BEGINEQNKEY:
    acapo2stringa(view);
    if (inizio) {
      macroelusione2stringa(view, "[");
    } else {
      macroelusione2stringa(view, "]");
    }
    acapo2stringa(view);
    break;
  case BEGINMATHKEY:
    if (inizio) {
      macroelusione2stringa(view, "(");
    } else {
      macroelusione2stringa(view, ")");
    }
    break;
  case SLANTED:
    if (inizio) {
      macroelusione2stringa(view, "sl");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case EMPHASIS:
    if (inizio) {
      macroelusione2stringa(view, "em");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case TITKEY:
    if (inizio) {
      macroelusione2stringa(view, "Tit");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case ITALIC:
    if (inizio) {
      macroelusione2stringa(view, "it");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case CORRKEY:
    if (inizio) {
      macroelusione2stringa(view, "CORR");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case PERSKEY:
    if (inizio) {
      macroelusione2stringa(view, "Personaggio");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case STRIKEKEY:
    if (inizio) {
      macroelusione2stringa(view, "strike");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case POWERKEY:
    if (inizio) {
      accentuazione2stringa(view);
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case INDEXKEY:
    if (inizio) { macroelusione2stringa(view, "_"); aperturagraffa2stringa(view); } else { chiusuragraffa2stringa(view); }
    break;
  case SUPKEY:
    if (inizio) {
      macroelusione2stringa(view, "Sup");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case SUBKEY:
    if (inizio) {
      macroelusione2stringa(view, "Sub");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case TILDEKEY:
    //if (inizio) {stringa2outfile(view, "~");}
    break;
  case DOTSKEY:
    if (inizio) {
      macroelusione2stringa(view, "dots");
    }
    break;
  case TIMESKEY:
    if (inizio) {
      macroelusione2stringa(view, "times");
    }
    break;
  case DSKEY:
    if (inizio) {
      macroelusione2stringa(view, "DB");
    }
    break;
  case SSKEY:
    if (inizio) {
      macroelusione2stringa(view, "SB");
    }
    break;
  case NBSPACEKEY:
    if (inizio) {
      macroelusione2stringa(view, ",");
    }
    break;
  case FLATKEY:
    if (inizio) {
      macroelusione2stringa(view, "flat");
    }
    break;
  case NATURALKEY:
    if (inizio) {
      macroelusione2stringa(view, "natural");
    }
    break;
  case CDOTKEY:
    if (inizio) {
      macroelusione2stringa(view, "cdot");
    }
    break;
  case VERTKEY:
    if (inizio) {      
      macroelusione2stringa(view, "vert");
    }
    break;
  case LBRACKETKEY:
    if (inizio) {
      macroelusione2stringa(view, "lbracket");
    }
    break;
  case RBRACKETKEY:
    if (inizio) {
      macroelusione2stringa(view, "rbracket");
    }
    break;
  case SKEY:
    if (inizio) {
      macroelusione2stringa(view, "S");
    }
    break;
  case SLASH:
    //if (inizio) {stringa2outfile(view, "/");}
    break;
  case GENERICENV:
    if (inizio) {
      macroingraffamento2stringa(view, "begin", node->lexema);
    if (node->testo1 != NULL) {
      inquadramento2stringa(view, node->testo1->lexema);
    }
    } else {
      macroingraffamento2stringa(view, "end", node->lexema);
    }
    break;
  case NOINDENTKEY:
    if (inizio) {
      macroelusione2stringa(view, "noindent");
    }
    break;
  case NEWPAGEKEY:
    if (inizio) {
      macroelusione2stringa(view, "newpage");
    }
    break;
  case BEGINEPISTOLAKEY:
    if (inizio) {
      macroelusione2stringa(view, "inizioepistola");
    }
    break;
  case ENDEPISTOLAKEY:
    if (inizio) {
      macroelusione2stringa(view, "fineepistola");
    }
    break;
  case HYPHENKEY:
    if (inizio) {
      macroelusione2stringa(view, "-");
    }
    break;
  case TITMARG:
    printEnv(view, "titmarg", inizio);
    break;
  case TEXTGREEK:
    printEnv(view, "textgreek", inizio);
    break;
  case ENUNCIATIO:
    printEnv(view, "Enunciatio", inizio);
  case ITEMKEY:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "item");
      if (node != NULL) {
        aperturaquadra2stringa(view);
        //
        node2node(node, view, sigle);
        chiusuraquadra2stringa(view);
      }
      spaziatura2stringa(view);
    }
    break;
  case QUOTE:
    printEnv(view, "quote", inizio);
    break;
  case ITEMIZE:
    printEnv(view, "itemize", inizio);
    break;
  case ENUMERATE:
    printEnv(view, "enumerate", inizio);
    break;
  case DESCRIPTION:
    printEnv(view, "description", inizio);
    break;
  case PROTASIS:
    printEnv(view, "Protasis", inizio);
  case CENTERKEY:
    //if (inizio) { macroelusione2stringa(view, "center"); acapo2stringa(view); }
    break;
  case PARAGRAPH:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "par");
      acapo2stringa(view);
    }
    break;
  case PARAGRAPHEND:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "pstart");
      acapo2stringa(view);
    }
    break;
  case PARAGRAPHSTART:
    if (inizio) {
      acapo2stringa(view);
      macroelusione2stringa(view, "pstart");
      acapo2stringa(view);
    }
    break;
  case TITLEKEY:
    if (inizio) {
      macroelusione2stringa(view, "title");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case TITLEDATEKEY:
    if (inizio) {
      macroelusione2stringa(view, "date");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case ROMAN:
    if (inizio) {
      macroelusione2stringa(view, "tt");
    }
    break;
  case BOLDFACE:
    if (inizio) {
      macroelusione2stringa(view, "bf");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case SMALLCAPSKEY:
    if (inizio) {
      macroelusione2stringa(view, "sc");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case TYPEFACE:
    if (inizio) {
      macroelusione2stringa(view, "tt");
    }
    break;
  case SPACES:
  
    if (nSpaces == 0) {
      spaziatura2stringa(view);
      nSpaces++;
    }
    nSpaces++;
    break;
  case NEWLINE:
    nSpaces++;
    acapo2stringa(view);
    /*
    if (inizio) {
      if (nSpaces < 10) {
        nSpaces++;
        fprintf(view->outFile, " ");
      } else {
        nSpaces = 0;
        fprintf(view->outFile, " ");
      }
    }
    */
    break;
  case MAUROTEXKEY:
    //if (inizio) {macroelusione2stringa(view, "MauroTeX");}
    break;
  case MTEXKEY:
    //if (inizio) {macroelusione2stringa(view, "MTeX");}
    break;
  case PERKEY:
    //if (inizio) {macroelusione2stringa(view, "PER");}
    break;
  case CONKEY:
    //if (inizio) {macroelusione2stringa(view, "CON");}
    break;
  case TEXKEY:
    //if (inizio) {macroelusione2stringa(view, "TeX");}
    break;
  case LATEXKEY:
    //if (inizio) {macroelusione2stringa(view, "LaTeX");}
    break;
  case SLQUOTEKEY:
    //if (inizio) {stringa2outfile(view, "`");}
    break;
  case SRQUOTEKEY:
    //if (inizio) {stringa2outfile(view, "'");}
    break;
  case DLQUOTEKEY:
    //if (inizio) {stringa2outfile(view, "``");}
    break;
  case DRQUOTEKEY:
    //if (inizio) {stringa2outfile(view, "''");}
    break;
  case ADKEY:
    //if (inizio) {stringa2outfile(view, "---");}
    break;
  case LONGKEY:
    //if (inizio) {stringa2outfile(view, "--");}
    break;
  case VECTORKEY:
    //if (inizio) {elusione2stringa(view);stringa2outfile(view, "=");}
    break;
  case CONTRKEY:
    if (inizio) {
      macroelusione2stringa(view, "CONTR");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case CRUXKEY:
    if (inizio) {
      macroelusione2stringa(view, "CRUX");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case EXPUKEY:
    if (inizio) {
      macroelusione2stringa(view, "EXPU");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case INTEKEY:
    if (inizio) {
      macroelusione2stringa(view, "INTE");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case BIBRIFKEY:
    if (inizio) {
      macroelusione2stringa(view, "Bibrif");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case CITMARGKEY:
    if (inizio) {
      macroelusione2stringa(view, "CitMarg");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case CITMARGSIGNKEY:
    if (inizio) {
      macroelusione2stringa(view, "CitMargSign");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case UNDERLINE:
    if (inizio) {
      macroelusione2stringa(view, "underline");
      aperturagraffa2stringa(view);
    } else {
      chiusuragraffa2stringa(view);
    }
    break;
  case LEFTSQUARE:
    if (inizio) {
      aperturaquadra2stringa(view);
    } else {
      chiusuraquadra2stringa(view);
    }
    break;
  case LEFTPAR:
    //if (inizio) {fprintf(view->outFile, "(");}
    break;
  case RIGHTPAR:
    //if (inizio) {fprintf(view->outFile, ")");}
    break;
  case HREFKEY:
    if (inizio) {
    //
    node2node(node->testo2, view, sigle);
    }
    break;
  case DAGKEY:
    //if (inizio) {elusione2stringa(view); aperturagraffa2stringa(view); node2node(node->testo2, view, sigle); elusione2stringa(view); chiusuragraffa2stringa(view); }
    break;
//case NOTAMACRO:
  case OUTRANGE:
    if (inizio) {
    stringa2outfile(view, "??");
    }
    break;
  case NOKEY:
    if (node) {
      if (inizio) {
        //aperturagraffa2stringa(view);
      } else {
        //chiusuragraffa2stringa(view);
      }
    }
    break;
  }
  }
}

// FUNZIONE DI STAMPA DI UNA FIGURA
void printFig(short key, struct testo * node, PARAMETRI * view) {}

// FUNZIONE DI STAMPA DI UN RIFERIMENTO AD UNA UNIT
void printUnitRef(
  const char * opera, 
  const char * etichetta, 
  PARAMETRI * view
  ){
  //elusione2stringa(view);
  // Print
  //stringa2outfile(view, "ref");
  //aperturagraffa2stringa(view);
  //if (opera != NULL && strlen(opera)>0) {
  // Print
  //  stringa2outfile(view, opera);
  //  stringa2outfile(view, "_");
  //}
  // Print
  //stringa2outfile(view, etichetta);
  //chiusuragraffa2stringa(view);
}

/* void printUnitRef(short nUnit, PARAMETRI * view) { */
void printUnitRef(
  short nUnit, 
  PARAMETRI * view
  ){
  /* fprintf(view->outFile, "\n\\textlatin{\\small\\textbf{%d}}", nUnit); */
  //acapo2stringa(view);
  //macroelusione2stringa(view, "Unit");
  //acapo2stringa(view);
}

// FUNZIONE DI STAMPA DI UNA PAROLA
void printLexema(
  short key, 
  const char * lexema, 
  PARAMETRI * view
  ){

  if (lexema != NULL) {

    // CONTROLLO INIBIZIONE DELLA STAMPA
    if (view->label == NULL || view->nota) {
    
      switch (key) {
        case TEXTCIRCLEDKEY:
          macroingraffamento2stringa(view, "textcircled", lexema);
          break;
     
        // DOCUMENTCLASS
        case CLASSKEY:
          ingraffamento2stringa(view, lexema);
          acapo2stringa(view);
          break;

        // ACCENTI
        case GRAVE:
          //printAccento(lexema, "\\`", view->outFile);
          break;
        case ACUTE:
          //printAccento(lexema, "\\'", view->outFile);
          break;
        case UML:
          //printAccento(lexema, "\\\"", view->outFile);
          break;
        case CIRC:
          //printAccento(lexema, "\\^", view->outFile);
          break;

        // SIMBOLI E MACRO
        case PARSKIP: //macroingraffamento2stringa(view, "parskip", lexema); acapo2stringa(view);
          break;
        case EXTERNALDOCUMENT:
          acapo2stringa(view);
          macroinquadramento2stringa(view, "externaldocument", lexema);
          break;
        case UNITKEY:
          //acapo2stringa(view);
          //macroinquadramento2stringa(view, "Unit", lexema);
          //acapo2stringa(view);
          break;
        case PARINDENT:
          macroingraffamento2stringa(view, "parindent", lexema);
          acapo2stringa(view);
          break;
        case PRELOADKEY:
          macroingraffamento2stringa(view, "PreloadUnicodePage", lexema);
          acapo2stringa(view);
          break;
        case SYMBOL:
        case MACRO:
          //macrogiaelusa2stringa(view, lexema);
          break;

        // PAROLA GENERICA
        default:
          // Print
          stringa2outfile(view, lexema);
          break;
      }
    }
  }
  nSpaces = 0;
  fflush(view->outFile);
}

// FUNZIONI DI STAMPA DI UNA LISTA DI AUTORI
void internalPrintAuthors(struct testo * node,
              PARAMETRI * view,
              struct testo * sigle) {
  node2node(node->testo1, view, sigle);
  if (node->testo2 != NULL) {
     // Print
     stringa2outfile(view, "#");
     //
     internalPrintAuthors(node->testo2, view, sigle);
  }
}

void printAuthors(struct testo * node,
          PARAMETRI * view,
          struct testo * sigle) {
  macroelusione2stringa(view, "author");
  aperturagraffa2stringa(view);
  //
  internalPrintAuthors(node, view, sigle);
  // Print
  chiusuragraffa2stringa(view);
  acapo2stringa(view);
}

// FUNZIONE DI STAMPA DEL FRONTESPIZIO
void printFrontespizio(struct testo * node,
             PARAMETRI * view,
             struct testo * sigle) {
  macroelusione2stringa(view, "frontespizio");
  aperturagraffa2stringa(view);
  //
  node2node(node->testo1, view, sigle);
  chiusuragraffa2stringa(view);
  aperturagraffa2stringa(view);
  //
  node2node(node->testo2->testo1, view, sigle);
  chiusuragraffa2stringa(view);
  aperturagraffa2stringa(view);
  //
  internalPrintAuthors(node->testo2->testo2, view, sigle);
  chiusuragraffa2stringa(view);
  acapo2stringa(view);
}

// FUNZIONE DI STAMPA DI UNA ANNOTAZIONE
void printFootnote(struct testo * node,
           PARAMETRI * view,
           struct testo * sigle) {
  macroelusione2stringa(view, "footnote");
  aperturagraffa2stringa(view);
  //
  node2node(node, view, sigle);
  chiusuragraffa2stringa(view);
}

// FUNZIONE DI STAMPA DI UN COMMENTO
void printCommento(struct testo * node,
    PARAMETRI * view,
    struct testo * sigle) {
}

// FUNZIONE DI STAMPA DI UNA ANNOTAZIONE
void printAdnotatio(struct testo * node,
          PARAMETRI * view,
          struct testo * sigle) {
  macroelusione2stringa(view, "Adnotatio");
  aperturagraffa2stringa(view);
  //
  node2node(node, view, sigle);
  // Print
  chiusuragraffa2stringa(view);
}

// FUNZIONE DI MARCATURA DEL TESTO CRITICO
void printInizioTestoCritico(short key, PARAMETRI * view) {
}

void printFineTestoCritico(short key, PARAMETRI * view) {
}

// FUNZIONE DI MARCATURA DELLE VARIANTI
void printInizioVarianti(PARAMETRI * view) {
  macroelusione2stringa(view, "VV");
  //aperturagraffa2stringa(view);
  //aperturagraffa2stringa(view);
}

void printSepVarianti(
  PARAMETRI * view
  ){
  //chiusuragraffa2stringa(view);
  //aperturagraffa2stringa(view);
}

void printFineVarianti(
  PARAMETRI * view
  ){
  //chiusuragraffa2stringa(view);
  //chiusuragraffa2stringa(view);
}

// FUNZIONE PER ABILITARE LA GESTIONE DELLA VV DI MVISIT
bool checkVV(short          key,
       struct testo * node,
       PARAMETRI    * view,
       struct testo * sigle) {
  return true;
}

// STAMPA UNA NOTA PRINCIPALE (NON SOTTONOTA)
void printVVPrincipale(
  short key,
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle
  ){           
  bool nota = view->nota;
  if (key == SCHOLKEY) {} else {
    view->nota = true;
  }
  if (key == NOTEKEY) {
    node2node(node, view, sigle);
  } else {
    esplorazioneVarianti(node, view, sigle, false, false, false, 1);
  }
  view->nota = nota;
}

// VERIFICA CHE ESISTA IL TESTIMONE DI RIFERIMENTO NELLE SIGLE - no manus alterae
bool trovaTestimoneNMA(struct testo * sigle, char * testRif) {
  bool ok = false;
  if (testRif) { // SE IL TESTIMONE DI RIFERIMENTO E` STATO IMPOSTATO
    if (sigle) { // SE CI SONO ALTRE SIGLE NELLA LISTA
      if (sigle->lexema) {
		if (strcmp(sigle->lexema, testRif) == 0) {
		  // SE LA SIGLA DEL TESTIMONE CORRISPONDE AL TESTIMONE DI RIFERIMENTO
		  ok = true;
		} else {
		  // ALTRIMENTI CERCA NELLE SIGLE RIMANENTI
		  ok = trovaTestimoneNMA(sigle->testo2, testRif);
		}
      }
    }
  }
  return ok;
}

// esplorazione UNA VARIANTE
bool esplorazioneVariante(struct testo * node,
		   PARAMETRI    * view,
		   struct testo * sigle,
		   bool           nested,
		   bool           duplex,
		   bool           multiplex,
		   int            numero,
		   short        * nTest) {
  
  bool stampato = false; // INDICATORE DI VARIANTE NON VUOTA
  char * testimone;      // TESTIMONE DA ESTRARRE
  
  // SE IL TESTO (1) NON E` VUOTO E (2) NON E` CONTRASSEGNATO DA BANALEKEY
  if (node && node->key != BANALEKEY) {
    
    // SE LA PRIMA SIGLA E` VUOTA, LA RIMPIAZZA CON IL TESTIMONE DI DEFAULT
    // (PER LE MACRO DI SEMPLIFICAZIONE)
    if (node->testo1->lexema == NULL) {
      
      node->testo1->lexema = view->testimone;
      
      // SE LA SIGLA DEL TESTIMONE DI DEFAULT NON E' STATA IMPOSTATA
      // EMETTE UN MESSAGGIO DI ERRORE
      if (strlen (view->testimone) == 0) {
	//printf("La sigla del testimone di default non e` stata impostata (utilizzare la macro NomeTestimone)\n");
      }
    }
    
    // VERIFICA CHE LA VARIANTE VADA STAMPATA
    testimone = opzioneTestEx();
    if (testimone == NULL || trovaTestimoneNMA(node->testo1, testimone)) {
      
      // SE SI TRATTA DI UNA TRASPOSIZIONE "DUPLEX", STAMPA "POST"
      if (duplex) {
	     //printNota("post ", view->outFile);
      }
      // SE LA NOTA NON E` INNESTATA OPPURE SE IL TESTIMONE E` IL PRIMO ED E` *
      if (!nested || (numero == 1 && (strcmp(node->testo1->lexema, "*") == 0) )) {
	*nTest = printCorpoVariante(node->testo2->testo1,
				    node->testo2->testo2,
				    node->testo1,
				    view,
				    duplex,
				    multiplex,
				    nested,
				    numero);
	
	stampato = true;
      }
    }
  }
  
  // RITORNA LA RISPOSTA
  return stampato;
}

// esplorazione DI UNA LISTA DI VARIANTI E RITORNA IL NUMERO DI TESTIMONI
short esplorazioneVarianti(struct testo * node,
		    PARAMETRI    * view,
		    struct testo * sigle,
		    bool           nested,
		    bool           duplex,
		    bool           multiplex,
		    int            numero) {

  short nTest = 0;
  
  // SE CI SONO ALTRE VARIANTI NELLA LISTA
  if (node) {
    
    // STAMPA LA PRIMA VARIANTE ED UNO SPAZIO SE QUESTA NON E` VUOTA
    if (esplorazioneVariante(node->testo1, view, sigle,
		      nested, duplex, multiplex, numero, &nTest) && 
	node->testo2) {
      printSepVarianti(view);
    }
    
    // VERIFICA SE SI TRATTA DI UNA TRASPOSIZIONE "DUPLEX"
    if (node->lexema) {
      if (strcmp(node->lexema,"duplex") == 0) duplex = true;
    }
    
    // VERIFICA SE SI TRATTA DI UNA TRASPOSIZIONE "MULTIPLEX"
    if (node->lexema) {
      if (strcmp(node->lexema,"multiplex") == 0) multiplex = true;
    }
    
    // STAMPA LE ALTRE VARIANTI
    nTest += esplorazioneVarianti(node->testo2, view, sigle,
			   false, duplex, multiplex, numero+1);
  }
  
  // RITORNA IL NUMERO DI TESTIMONI GESTITI
  return nTest;
}


// FUNZIONE DI STAMPA DI UNA VARIANTE
void printVV(
  short          key,
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle
  ){
  // LV = VERO  ==> PRIMA VARIANTE AL LIVELLO TESTO
  // LV = FALSO ==> VARIANTE NON AL LIVELLO TESTO
  bool LV = view->nota;
  if (LV) {
    //
    esplorazioneVarianti(node, view, sigle, true, false, false, 1);
  } else {
    printVVPrincipale(key, node, view, sigle);
  }
  view->nota = LV;
}

// esplorazione DI UNA LISTA DI CAMPI DI UNA RIGA DI UNA TABELLA
short esplorazioneCampi(short          nRow,
		 const char   * colonne,
		 struct testo * campi,
		 PARAMETRI    * view,
		 struct testo * sigle) {
    
    short nCol = 0;
    short nColCampo = 1;
    
    if (campi) {
		nCol = esplorazioneCampi(nRow, colonne, campi->testo1, view, sigle);
		printCampo(nRow, nCol, colonne, campi->testo2, view, sigle);
		if (campi->testo2) {
			if (campi->testo2->key == MULTICOLKEY) {
				nColCampo = atoi(campi->testo2->lexema);
			}
		}
    }
    
    return nCol+nColCampo;
}

// esplorazione DI UNA RIGA DI UNA TABELLA
void esplorazioneRiga(short          nRow,
	       const char   * colonne,
	       struct testo * riga,
	       PARAMETRI    * view,
	       struct testo * sigle) {
	printRowHeader(riga->key, view);
	esplorazioneCampi(nRow, colonne, riga, view, sigle);
	printRowFooter(riga->key, view);
}

// esplorazione DI UNA LISTA DI RIGHE DI UNA TABELLA
short esplorazioneRighe(const char   * colonne,
		 struct testo * righe,
		 PARAMETRI    * view,
		 struct testo * sigle) {
	
	short nRow = 0;
	
	if (righe) {
		nRow = esplorazioneRighe(colonne, righe->testo1, view, sigle);
		esplorazioneRiga(nRow, colonne, righe->testo2, view, sigle);
	}
	
	return nRow+1;
}

// esplorazione DI UNA TABELLA
void esplorazioneTable(int key,
		struct testo * tabella,
		PARAMETRI    * view,
		struct testo * sigle) {
  //printTableStart(key, tabella->testo1->testo1, view, sigle);
  //printTableHeader(key, tabella->lexema, view);
  esplorazioneRighe(tabella->lexema, tabella->testo2, view, sigle);
  //printTableFooter(key, view);
  //printTableFinish(key, tabella->testo1->testo2, view, sigle);
}

// esplorazione DI UNA TABELLA
void esplorazioneLongTable(int key,
		    struct testo * tabella,
		    PARAMETRI    * view,
		    struct testo * sigle) {
  printTableHeader(key, tabella->lexema, view);
  esplorazioneRighe(tabella->lexema, tabella, view, sigle);
  printTableFooter(key, view);
}

// esplorazione DI UNA MACRO "CASES"
void esplorazioneCases(short        key,
	      struct testo * node,
	      PARAMETRI    * view,
	      struct testo * sigle) {
  //printCases(key, node, view, sigle);
}

// esplorazione DI UNA MACRO GENERICA
void esplorazioneKey(short          key,
			  long           ln,
			  struct testo * node,
			  PARAMETRI    * view,
			  struct testo * sigle) {
  // SALVATAGGIO DELL'AMBIENTE A LISTA ATTUALE
  short saveListEnv;
  bool saveItemOpen;

  // GESTIONE DELLA PARTE INIZIALE DELLA MACRO
  printKey(true, key, ln, node, view, sigle);
  
  // esplorazione INTERNA DELLA MACRO
  switch (key) {
  case CLASSKEY:
  case DOCENVKEY:
  case GEOMETRY:
  case USEPACKAGE:
  case COMMKEY:
  case PROPKEY:
  case ARGKEY:
  case LIBROKEY:
  case MARKKEY:
  case HREFKEY:
  case DAGKEY:
  case TESTIMONIKEY:
  case ELENCOTESTIMONIKEY:
  case LABELKEY:
  case LETTERLABELKEY:
  case CITEKEY:
  case REFKEY:
  case PAGEREFKEY:
  case EXTERNALDOCUMENT:
  case OPERAKEY:
  case LUOGOKEY:
  case NOMEKEY:
    break;
  case MANUSKEY:
	printManus(node, view, sigle);
	break;
  case FOLIUMKEY:
    // CONTROLLO ESTRAZIONE TESTIMONE
    if( node->lexema ) {
      if (opzioneTestEx() == NULL ||
  		strcmp(opzioneTestEx(), node->lexema) == 0) {
        printFolium(node->lexema, node->testo2->lexema, view, sigle);
      }
    }
    break;
  case CITKEY:
    //macromain2stringa(node, view, sigle, "Cit");
    break;
  case DATEKEY:
    //node2node(node->testo1, view, sigle);
    printDate(node, view, sigle);
    break;
  case TABLEKEY:
    //esplorazioneTable(key, node, view, sigle);
    break;
  case LONGTABLEKEY:
    esplorazioneLongTable(key, node, view, sigle);
    break;
  case CASES:
    //esplorazioneCases(key, node, view, sigle);
    break;
  case FRACKEY:
    printFrac(node, view, sigle);
    break;
  case TITLEWORKKEY:
    printTitleWork(node, view, sigle);
    break;
  case SUPSUBKEY:
    printSupSub(node, view, sigle);
    break;
  case LBKEY:
    node2node(node->testo2, view, sigle);
    if (node->lexema != NULL && view->label != NULL) {
      // CONTROLLO CORRISPONDENZA TRA LE ETICHETTE
      if (strcmp(node->lexema, view->label) == 0) {
    	view->label = NULL;
      }
    }
    break;
  case MCITEKEY:
    //node2node(node->testo2, view, sigle);
    break;
  case DESCRIPTION:
  case ITEMIZE:
  case ENUMERATE:
	saveListEnv = view->listEnv;
	saveItemOpen = view->itemOpen;
	view->listEnv = key;
	view->itemOpen = false;
	node2node(node->testo2, view, sigle);
	view->listEnv = saveListEnv;
	view->itemOpen = saveItemOpen;
	break;
  case ITEMKEY:
	view->itemOpen = true;
	break;
  case TEXTGREEK:
	printTextGreek(node->testo2, view, sigle);
	break;
  case VSPACEKEY:
  case QUOTE:
  case ENUNCIATIO:
  case PROTASIS:
  case TABULAR:
  case TITMARG:
  case GENERICENV:
    node2node(node->testo2, view, sigle);
    break;
  case UNITKEY:
    break;
  case AUTHORKEY:
	printAuthors(node, view, sigle);
	break;
  case FRONTESPIZIOKEY:
	printFrontespizio(node, view, sigle);
	break;
  case ALIAMANUSKEY:
	//printAliaManus(node, view, sigle);
	break;
  default:
    node2node(node, view, sigle);
    break;
  }

  // GESTIONE DELLA PARTE FINALE DELLA MACRO
  printKey(false, key, ln, node, view, sigle);
}

// FUNZIONE DI STAMPA DI UNA NOTA
void esplorazioneVV(
  short          key,
  struct testo * node,
  PARAMETRI    * view,
  struct testo * sigle
  ){ 
  printInizioVarianti(view);
  printVV(key, node, view, sigle);
  printFineVarianti(view);
}

// CONTROLLORE:
// esplorazione DI UN TESTO COME NODO DI UN GRAFO
// CERCA node->key
//       node->key    ABILITA O MENO UN SOTTOPROCESSO SU node, node->testo1, etc.
//       node->testo1 VIENE (FORSE) GESTITO IN UN SOTTOPROCESSO (così come altri)
//       node->testo2 VIENE GESTITO DOPO IL CONTROLLO DI node->key
//  ____
// |node|<-----------------------.----.
//  ^^^^                         |    |
// |                            (?)   |
// |   ___       ____________    |    |
//  \_(key)---->|node->testo1|___/    |
// |   ^^^       ^^^^^^^^^^^^         |
// |   ______    ____________         |
//  \_|testo2|->|node->testo2|________/
//     ^^^^^^    ^^^^^^^^^^^^          
void node2node          (// testo2macro
  struct testo * node   ,
  PARAMETRI    * view   ,
  struct testo * sigle  ){
  while (node!=NULL     ){
  if    (node->key      ){
  switch(node->key      ){
  // TRADUZIONI DIRETTE: KEY --> N-GRAMMA UNARIO
  case  NEWLINE          : acapo2stringa(view); break;          /*
*/
  case  SPACES           : spaziatura2stringa(view); break;     /* */
  case  SLQUOTEKEY       : backtick2stringa(view); break;       /*\*/
  case  SRQUOTEKEY       : apicazione2stringa(view); break;     /*'*/
  case  LEFTPAR          : aperturatonda2stringa(view); break;  /*(*/
  case  RIGHTPAR         : chiusuratonda2stringa(view); break;  /*)*/
  case  LEFTSQUARE       : aperturaquadra2stringa(view); break; /*[*/
  case  RIGHTSQUARE      : chiusuraquadra2stringa(view); break; /*]*/
  case  LEFTBRA          : aperturagraffa2stringa(view); break; /*{*/
  case  RIGHTBRA         : chiusuragraffa2stringa(view); break; /*}*/
  case  TILDEKEY         : tilde2stringa(view); break;          /*~*/
  case  ':'              : esplicazione2stringa(view); break;   /*:*/
  case  SLASH            : slash2stringa(view); break;          /*\*/
  // TRADUZIONI DIRETTE: KEY --> N-GRAMMA BINARIO
  case  DLQUOTEKEY       : duebacktick2stringa(view); break;    /*``*/
  case  DRQUOTEKEY       : dueapicazione2stringa(view); break;  /*''*/
  case  LONGKEY          : duetrattino2stringa(view); break;    /*--*/
  case  '\\'             : dueslash2stringa(view); break;
  // TRADUZIONI DIRETTE: KEY --> N-GRAMMA TERNARIO
  case  ADKEY            : tretrattino2stringa(view); break;    /*---*/
  // TRADUZIONI SEMIDIRETTE: KEY + TOKEN --> N-GRAMMA ENNARIO
  case  POWERKEY         : macroelusione2stringa(view, "^"              ); break;
  case  INDEXKEY         : macroelusione2stringa(view, "_"              ); break;
  case  MAUROTEXKEY      : macroelusione2stringa(view, "MauroTeX"       ); break;
  case  MTEXKEY          : macroelusione2stringa(view, "MTeX"           ); break;
  case  PERKEY           : macroelusione2stringa(view, "PER"            ); break;
  case  CONKEY           : macroelusione2stringa(view, "CON"            ); break;
  case  INTESTAKEY       : macroelusione2stringa(view, "intestazione"   ); break;
  case  FOLIUMMARGOPT    : macroelusione2stringa(view, "FoliumInMargine"); break;
  case  FOLIUMTESTOOPT   : macroelusione2stringa(view, "FoliumInTesto"  ); break;
  case  CLINEKEY         : macroelusione2stringa(view, "regula"         ); break;
  case  LEFTGRATABKEY    : macroelusione2stringa(view, "lgra"           ); break;
  case  RIGHTGRATABKEY   : macroelusione2stringa(view, "rgra"           ); break;
  case  LEFTANGTABKEY    : macroelusione2stringa(view, "lang"           ); break;
  case  RIGHTANGTABKEY   : macroelusione2stringa(view, "rang"           ); break;
  case  SETLENGTH        : macroelusione2stringa(view, "setlength"      ); break;
  case  MAKETITLEKEY     : macroelusione2stringa(view, "maketitle"      ); break;
  case  POSTSCRIPTKEY    : macroelusione2stringa(view, "POSTSCRIPT"     ); break;
  case  INTERLKEY        : macroelusione2stringa(view, "INTERL"         ); break;
  case  SUPRAKEY         : macroelusione2stringa(view, "SUPRA"          ); break;
  case  MARGKEY          : macroelusione2stringa(view, "MARG"           ); break;
  case  MARGSIGNKEY      : macroelusione2stringa(view, "MARGSIGN"       ); break;
  case  PCKEY            : macroelusione2stringa(view, "PC"             ); break; 
  case  PARAGRAPHSTART   : macroelusione2stringa(view, "pstart"         ); break;
  case  PARAGRAPHEND     : macroelusione2stringa(view, "pend"           ); break;
  case  ANNOTAZKEY       : macroelusione2stringa(view, "Annotazioni"    ); break;
  case  COMMENTKEY       : macroelusione2stringa(view, "Commenti"       ); break;
  case  CONGKEY          : macroelusione2stringa(view, "Cong"           ); break;
  case  TBKEY            : macroelusione2stringa(view, "TB"             ); break;
  case  NIHILKEY         : macroelusione2stringa(view, "NIHIL"          ); break;
  case  DELKEY           : macroelusione2stringa(view, "DEL"            ); break;
  case  EXPKEY           : macroelusione2stringa(view, "EXP"            ); break;
  case  OMLACKEY         : macroelusione2stringa(view, "OMLAC"          ); break;
  case  NLKEY            : macroelusione2stringa(view, "NL"             ); break;
  case  OMKEY            : macroelusione2stringa(view, "OM"             ); break;
  case  LACMKEY          : macroelusione2stringa(view, "LACm"           ); break;
  case  LACCKEY          : macroelusione2stringa(view, "LACc"           ); break;
  case  LACSKEY          : macroelusione2stringa(view, "LACs"           ); break;
  case  DSKEY            : macroelusione2stringa(view, "DB"             ); break;
  case  SSKEY            : macroelusione2stringa(view, "SB"             ); break;
  case  ENDMATHKEY       : macroelusione2stringa(view, ")"              ); break;
  case  NBSPACEKEY       : macroelusione2stringa(view, ","              ); break;
  case  FLATKEY          : macroelusione2stringa(view, "flat"           ); break;
  case  NATURALKEY       : macroelusione2stringa(view, "natural"        ); break;
  case  CDOTKEY          : macroelusione2stringa(view, "cdot"           ); break;
  case  VERTKEY          : macroelusione2stringa(view, "vert"           ); break;
  case  LBRACKETKEY      : macroelusione2stringa(view, "lbracket"       ); break;
  case  RBRACKETKEY      : macroelusione2stringa(view, "rbracket"       ); break;
  case  SKEY             : macroelusione2stringa(view, "S"              ); break;
  case  DOTSKEY          : macroelusione2stringa(view, "dots"           ); break;
  case  TIMESKEY         : macroelusione2stringa(view, "times"          ); break;
  // TRADUZIONI INDIRETTE: (KEY,NODE) --> N-GRAMMA
  case  GRAVE            : macrogiaelusa2stringa(view, node             ); break;
  case  ACUTE            : macrogiaelusa2stringa(view, node             ); break;
  case  UML              : macrogiaelusa2stringa(view, node             ); break;
  case  TILDE            : macrogiaelusa2stringa(view, node             ); break;
  case  CIRC             : macrogiaelusa2stringa(view, node             ); break;
  case  MACRO            : macrogiaelusa2stringa(view, node             ); break;
  case  SYMBOL           : macrogiaelusa2stringa(view, node             ); break;
  case  WORD             : macrogiaelusa2stringa(view, node             ); break;
  // TRADUZIONI SEMIDIRETTE: KEY + NODO --> N-GRAMMA ENNARIO
  case REPKEY            : macrowordsquared2stringa(view, "REP", node); break;
  // TRADUZIONI INDIRETTE RICORSIVE: (KEY,NODE) + TOKEN --> N-GRAMMA
  case  FIGSKIPKEY       : macroopzionaunoargomentadue2stringa(view, "Figskip", node, sigle); break;
  case  FIGURAKEY        : macroopzionaunoargomentadue2stringa(view, "Figura" , node, sigle); break;
  case  FMARGKEY         : macroopzionaunoargomentadue2stringa(view, "Marg"   , node, sigle); break;
  case  FORMULAKEY       : macroopzionaunoargomentadue2stringa(view, "Formula", node, sigle); break;
  case  TAVOLAKEY        : macroopzionaunoargomentadue2stringa(view, "Tavola" , node, sigle); break;
      
  case  CASES            : macroargomentaunoodue2stringa("Cases", node, view, sigle); break;
//case  BANALEKEY        : macroelusione2stringa(view, "banale"); break;
  case  VECTORKEY        : macroelusione2stringa(view, "="); node2node(node->testo1, view, sigle); break;
  case  EXTERNALDOCUMENT : macrorefered2stringa(node, view, sigle, "externaldocument"); break;
                         //esplorazioneKey(node->key, node->ln, node->testo1, view, sigle); break;
//case LEMMAKEY          :
//case DATEKEY           :
  case CITKEY           : if(node->testo1){ macromain2stringa(node->testo1, view, sigle, "Cit");} break;
  case LUOGOKEY         : macromain2stringa(node->testo1, view, node->testo2, "Luogo"); break;
  case NOMEKEY          : macromain2stringa(node->testo1, view, node->testo2, "Nome"); break;
  case OPERAKEY         : macromain2stringa(node->testo1, view, node->testo2, "Opera"); break;
  case HTMLCUTKEY       : if(node->testo1){ macrodiffered2stringa(view, "htmlcut", node->testo1); } else{ macroelusione2stringa(view, "htmlcut");} break;
  case HREFKEY          : macrorefered2stringa(node, view, sigle, "href"); break;
  case VOIDHREFKEY      : macrorefered2stringa(node, view, sigle, "voidhref"); break;
/*da sistemare-->*/
  case TABLEKEY         : macroingraffamento2stringa(view, "begin", "tabula"); ingraffamento2stringa(view, node->testo1->lexema); esplorazioneRighe(node->testo1->lexema, node->testo1->testo2, view, sigle);  macroingraffamento2stringa(view, "end", "tabula"); break;
  case HLINEKEY         : macroelusione2stringa(view, node->lexema); break;
  case MULTICOLKEY      : break;
  case MULTIROWKEY      : break;
  case BEGINTABKEY      : macrotestobra2stringa(node->testo1, view, sigle, "inizio"); break;
  case LABELKEY         : macrotestobra2stringa(node->testo1, view, sigle, "label"); break;
  case ENDTABKEY        : macrotestobra2stringa(node->testo1, view, sigle, "fine"); break;
  case CITEKEY          : macrotestobra2stringa(node->testo1, view, sigle, "cite"); break;
//case RIGHTANG         : non arriva mai (per definizione: non viene mai salvato da in mparse.y con un makeTesto)
/*<--da sistemare*/
  case LEFTANG          : minoranza2stringa(view); if (node->testo1) { node2node(node->testo1, view, sigle); } maggioranza2stringa(view); break;
  case CENTERKEY        : macroelusione2stringa(view, "begin{center}"); if (node->testo1) { node2node(node->testo1, view, sigle); }macroelusione2stringa(view, "end{center}"); break;
  case MCITEKEY         : if (node->testo1) { if (node->testo1->lexema) { if (node->testo1->testo2) { macrowordbratestobra2stringa(view, "maurocite", node->testo1->lexema, node->testo1->testo2, sigle); }}} break;
  case PARAGRAPH        : macroelusione2stringa(view, "par"); break;
  //case USEPACKAGE       : macrotestobra2stringa(node->testo1, view, sigle, "Dag"); break;
  case DAGKEY           : macrotestobra2stringa(node->testo1, view, sigle, "Dag"); break;
  case PAGEREFKEY       : macrotestobra2stringa(node->testo1, view, sigle, "pageref"); break;
  case REFKEY           : macrotestobra2stringa(node->testo1, view, sigle, "ref"); break;
  case UNDERLINE        : macrotestobra2stringa(node->testo1, view, sigle, "underline"); break;
  case CONTRKEY         : macrotestobra2stringa(node->testo1, view, sigle, "CONTR"); break;
  case TITKEY           : macrotestobra2stringa(node->testo1, view, sigle, "Tit"); break;
  case PERSKEY          : macrotestobra2stringa(node->testo1, view, sigle, "Personaggio"); break;
  case RUBRKEY          : macrotestobra2stringa(node->testo1, view, sigle, "RUBR"); break;
  case STRIKEKEY        : macrotestobra2stringa(node->testo1, view, sigle, "STRIKE"); break;
  case ABBRKEY          : macrotestobra2stringa(node->testo1, view, sigle, "ABBR"); break;
  case TEOREMAKEY       : macrotestobra2stringa(node->testo1, view, sigle, "Teorema"); break;
  case CAPITOLOKEY      : macrotestobra2stringa(node->testo1, view, sigle, "Capitolo"); break;
  case ALITERKEY        : macrotestobra2stringa(node->testo1, view, sigle, "Aliter"); break;
  case COROLLARIOKEY    : macrotestobra2stringa(node->testo1, view, sigle, "Corollario"); break;
  case SCOLIOKEY        : macrotestobra2stringa(node->testo1, view, sigle, "Scolio"); break;
  case ADDITIOKEY       : macrotestobra2stringa(node->testo1, view, sigle, "Additio "); break;
  case SOTTOCAPITOLOKEY : macrotestobra2stringa(node->testo1, view, sigle, "Sottocapitolo"); break;
  case SUPKEY           : macrotestobra2stringa(node->testo1, view, sigle, "Sup"); break;
  case SUBKEY           : macrotestobra2stringa(node->testo1, view, sigle, "Sub"); break;
  case TITLEKEY         : macrotestobra2stringa(node->testo1, view, sigle, "Title"); break;
  case AUTHORKEY        : macrotestobra2stringa(node->testo1, view, sigle, "Author"); break;
  case PARAGRAPHKEY     : macrotestobra2stringa(node->testo1, view, sigle, "paragraph"); break;
  case PARSKIP          : macroingraffamento2stringa(view, "parskip", node->lexema); break;
  /*
  case DESCOMPLKEY   : macrotripla2stringa(node->testo1, view, sigle, "DESCOMPL"); break;
  case EDCOMPLKEY    : macrotripla2stringa(node->testo1, view, sigle, "EDCOMPL"); break;
  case ANTEDELKEY    : macrotestobra2stringa(node->testo1, view, sigle, "ANTEDEL"); break;
  case POSTDELKEY    : macrotestobra2stringa(node->testo1, view, sigle, "POSTDEL"); break;
  case CRKEY         : macroingraffamento2stringa(view, "CR", node->lexema); break; 
  case TRKEY         : macrotestobra2stringa(nota, view, sigle, "DES"); break;
  case EDKEY         : macrotestobra2stringa(nota, view, sigle, "ED"); break;
  case EXKEY         : macrotestobra2stringa(node->testo1, view, sigle, "EX"); break;
  case LECKEY        : macrotestobra2stringa(node->testo1, view, sigle, "LEC"); break;
*/

case ALIAMANUSKEY                    : macroopzionaunoargomentadue2stringa(view, "AliaManus", node, sigle);

/*
case ELENCOTESTIMONIKEY              : \ElencoTestimoni ELENCOTESTIMONIKEY LEFTBRA allowedtestimoni RIGHTBRA
case TESTIMONIKEY                    : \Testimoni LEFTBRA testimoni RIGHTBRA optspaces prologo
case USEPACKAGE                      : \usepackage opzioni LEFTBRA listaopt RIGHTBRA optspaces prologo 
case GEOMETRY                        : \geometry LEFTBRA listaopt RIGHTBRA optspaces prologo
case FOLIUMTESTOOPT                  : \FoliumInTesto optspaces prologo; break;
case BEGINKEY                        : NON PASSA
case ENDKEY                          : NON PASSA

case PLKEY                           : | LEFTBRA \PL *** testobra RIGHTBRA; break;
case BISKEY                          : \BIS | *********SBISKEY*********** mano testobra 
case CRUXKEY                         : CRUX       : \CRUX        testobra; break;
case EXPUKEY                         : EXPU       : \EXPU        testobra; break;
case INTEKEY                         : INTE       : \INTE        testobra; break;
case CORRKEY                         : CORR       : \CORR        testobra; break;
case CITMARGKEY                      : citmarg    : \CitMarg     testobra; break;
case CITMARGSIGNKEY                  : citmargsign: \CitMargSign testobra; break;
case LBKEY                           : lb: \LB     *** wordbra testobra; break;
case MATHKEY                         : | \Math     *** testobra; break;
case VSPACEKEY                       : | \vspace   *** testobra; break;
case BIBRIFKEY                       : | \Bibrif   *** testobra; break;
case EMPHASIS                        : | \em       *** space testo; break;
case ITALIC                          : | \it       *** space testo; break;
case SLANTED                         : | \sl       *** space testo; break;
case BOLDFACE                        : | \bf       *** space testo; break;
case TYPEFACE                        : | \tt       *** space testo; break;
case ROMAN                           : | \rm       *** space testo; break;
case UNITEMPTYKEY                    : unitempty: \UnitEmpty; break;
case LIBROKEY                        : \Libro          *** testobra; break;
case ARGKEY                          : \Arg            *** testobra; break;
case SMALLCAPSKEY                    : \textsc *** testobra; break;
case BOLDFACEKEY                     : \textbf *** testobra; break;
case ITALICKEY                       : \textit *** testobra; break;
case FOLIUMKEY                       : \Folium LEFTBRA testimone ':' WORD RIGHTBRA | \Folium LEFTBRA testimone ':' WORD RIGHTBRA |  LEFTBRA error
case LABELKEY                        : \label          *** wordbra; break;
case LETTERLABELKEY                  : \label          *** wordbra; wordbra; break;
case TITLEDATEKEY                    : \author  nameslistbra; break;
case MANUSKEY                        : manus: \Manus *** wordbra testobra testobra wordbra | \Manus *** wordbra testobra testobra; break;
case ITEMKEY                         : \item *** opttestosquare testonosquare; break;
case NOINDENTKEY                     : \noindent "); break;
case NEWPAGEKEY                      : \newpage  "); break;
case BEGINEPISTOLAKEY                : \inizioepistola "); break;
case ENDEPISTOLAKEY                  : \fineepistola "); break;
case HYPHENKEY                       : \- testo; break;
case FRONTESPIZIOKEY                 : frontespizio: FRONTESPIZIOKEY testobra testobra nameslistbra; break;
case LISTSEPKEY                      : testo *** # *** nameslist; break;
case ENDEQNKEY                       : equazione: BEGINEQNKEY testo \\]; break;
case DOCENVKEY                       : optspaces prologo htmlcut testonosquare BEGINKEYoENDKEY LEFTBRA *** document *** RIGHTBRA optspaces error; break;
case LONGTABLEKEY                    : ltabula *** BEGINKEY LEFTBRA LONGTABLEKEY RIGHTBRA wordbra optspaces righe ENDKEY LEFTBRA LONGTABLEKEY RIGHTBRA; break;
case MARKKEY                         : \Markup       *** testobra testobra; break;
case FRACKEY                         : \frac         *** testobra testobra; break;
case ALIAMANUSKEY                    : \AliaManus    *** testobra testobra; break;
case SUPSUBKEY                       : \SupSub       *** testobra testobra; break;
case TITLEWORKKEY                    : \Title        *** testobra testobra; break;
*/ // NB che in alcuni casi makeTesto viene sostituito da altro, come: makeSemplCorr
      /*case SEPCOLKEY       : legatura2stringa(view); spaziatura2stringa(view); node2node(node->testo1, view, NULL); break;
      case SEPROWKEY       : elusione2stringa(view); elusione2stringa(view); node2node(node->testo1, view, NULL); break;*/
      case BEGINNMKEY      : macroelusione2stringa(view, "BeginNM"); node2node(node->testo1, view, NULL); break;
      case ENDNMKEY        : macroelusione2stringa(view, "EndNM"); node2node(node->testo1, view, NULL); break;
      case TEXTCIRCLEDKEY  : printLexema(node->key, node->testo1->lexema, view); break;
      case BEGINMATHKEY    :
      case BEGINEQNKEY     : esplorazioneKey(node->key, node->ln, node->testo1, view, sigle); break;
      case GREEKKEY        : esplorazioneKey(node->key, node->ln, node->testo1, view, sigle); break;
      case CLASSKEY        : esplorazioneKey(node->key, node->ln, node->testo1, view, sigle); printLexema(node->key, node->lexema, view); break;
      default              : esplorazioneKey(node->key, node->ln, node->testo1, view, sigle); break;

      case PRELOADKEY      : macroinquadrabile2stringa(view, "PreloadUnicodePage", node->testo1->lexema); break;
      case UNITKEY         : macroinquadrabile2stringa(view, "Unit", node->testo1->lexema); break;
      
      case UNITREFKEY      : macrodefered2stringa(view, "UN", node->testo1->testo1->lexema, node->testo1->testo2->lexema); break;
      case PROPKEY         : if (node->testo1) { if (node->testo1->lexema) {macroingraffamento2stringa(view, "Prop", node->testo1->lexema);} } else {macroingraffamento2stringa(view, "Prop", "");} break;
      case PARINDENT       : macrowordbra2stringa(view, "parindent", node->lexema, sigle); break;
      case NUMINTESTKEY    : macrotestobra2stringa(node->testo1, view, sigle, "numero"); break;
      case LEMMAKEY        : macrotestobra2stringa(node->testo1, view, sigle, "Lemma"); break;
      case SECTIONKEY      : macrotestobra2stringa(node->testo1, view, sigle, "section"); break;
      case SUBSECTIONKEY   : macrotestobra2stringa(node->testo1, view, sigle, "subsection"); break;
      case SSSECTIONKEY    : macrotestobra2stringa(node->testo1, view, sigle, "subsubsection"); break;
      case NOTEKEY         : macrotestobra2stringa(node->testo1, view, sigle, "note"); break;
      case ADNOTKEY        : macrotestobra2stringa(node->testo1, view, sigle, "Adnotatio"); break;
      case FOOTNOTEKEY     : macrotestobra2stringa(node->testo1, view, sigle, "footnote"); break;
      case NOMTESTKEY      : macrotestobra2stringa(node->testo1, view, sigle, "NomeTestimone"); break;
      case MITINTESTKEY    : macrotestobra2stringa(node->testo1, view, sigle, "mittente"); break;
      case DESINTESTKEY    : macrotestobra2stringa(node->testo1, view, sigle, "destinatario"); break;
      case LMIINTESTKEY    : macrotestobra2stringa(node->testo1, view, sigle, "luogomitt"); break;
      case COMINTESTKEY    : macrotestobra2stringa(node->testo1, view, sigle, "commento"); break;
      case DATINTESTKEY    : macrotestobra2stringa(node->testo1, view, sigle, "data"); break;
      case COMMKEY         : macrotestobra2stringa(node->testo1, view, sigle, "Comm"); break;
  case  VVKEY           : macrovariante2stringa(view, "VV", node, sigle); break;
  /* TODO: MACRO DI SEMPLIFICAZIONE
  // vengono "nascoste" da VVKEY
  // vedi mparse.y
  // SISTEMARE!
  case SMARGKEY                       : Smarg         *** mano testobra; break; 
  case SMARGSKEY                       : Smargsign     *** mano testobra; break;
  case SINTERLKEY                      : Sinterl       *** mano testobra; break;
  case SSUPRAKEY                       : Ssupra        *** mano testobra; break;
  case SPCKEY                          : Spc           *** mano testobra; break;
  case SBISKEY                         : Sbis          *** mano testobra; break;
  case SMCORRKEY                       : Smargcorr     *** mano testobra testobra; break;
  case SMSCORRKEY                      : Smargsigncorr *** mano testobra testobra; break;
  case SICORRKEY                       : Sintcorr      *** mano testobra testobra; break;
  case SANTEDELKEY                     : Spostedel     *** mano testobra testobra; break;
  case SPOSTDELKEY                     : Smargsigncorr *** mano testobra testobra; break;
  case SEXKEY                          : Sex           *** mano testobra testobra; break;
  */
  case  VBKEY           : macrovariante2stringa(view, "VB"   , node, sigle); break;
  case  TVKEY           : macrovariante2stringa(view, "TV"   , node, sigle); break;
  /* TODO: SCHOL
  // manca la sua documentazione
  // sia in maurpro.tex
  // sia in man8.1.tex
  */
  case  SCHOLKEY        : macrovariante2stringa(view, "SCHOL", node, sigle); break;
  case  NMARGKEY        : macrovariante2stringa(view, "NOTAMARG"   , node, sigle); break;
  break;
      case NOKEY           : printLexema(node->key, node->lexema, view); esplorazioneKey(node->key, node->ln, node->testo1, view, sigle); break;
    }}
    // SWITCH
    node = node->testo2;
  }
}
// FUNZIONE DI STAMPA DELL'ALBERO SINTATTICO PER DEBUG
void node2view(
  PARAMETRI    * view, 
  struct testo * node
  ){ // naviga l'albero sintattico passando da nodo a nodo;
  // il passaggio avviene con un cammino che parte da un primo nodo
  // che puo' avere 
  //   zero (nel caso in cui il documento sia composto da un unico termine), 
  //   uno (nel caso in cui il secondo nodo non sia necessario per il primo termine)
  // o due figli (caso frequente in un grafo bidirezionale).
  if(view){                         /* Get Parametri View */ // se view esiste
  long int       XX = (view->XX    )?view->XX    :1        ; // XX ricorda ln
  long int       YY = (view->YY    )?view->YY    :  0      ; // YY ricorda key
  const char   * ZZ = (view->ZZ    )?view->ZZ    :    ""   ; // ZZ ricorda lexema
  //bool           XY = (view->XY    )?view->XY    :      0  ; // XY ricorda isChild1
  //bool           YX = (view->YX    )?view->YX    :        0; // YX ricorda isChild2
  short          CX = (view->CX    )?view->CX    :        0; // CX ricorda lexPosInRow
  // Alla prima chiamata i parametri sono impostati come di default 
  // oppure come descritto qui sopra (uguale al valore di default)
  // Ad ogni chiamata
  // Dovrei aver modificato i parametri in modo da 
  // posso confrontarli con i valori del nodo corrente.
  // Il confronto avviene per esistenza; 
  // (ho controllato se esistono i parametri, 
  // ora controllo se esiste il nodo corrente)
  // in questo passaggio ottengo una serie di informazioni:
  // TR, la riga in cui l'analizzatore lessicale ha trovato il token corrente
  // TK, la chiave riferita al token corrente
  // TL, la parola riferita al token corrente
  // T1, la possibilità di proseguire nel ramo dx dell'albero sintattico
  // T2, la possibilità di proseguire nel ramo sx dell'albero sintattico
  if(node){                         /* Get Parametri Node */ // se node esiste
  long int       TR = (node->ln    )?node->ln    :       XX; // trovo o ricordo TR
  long int       TK = (node->key   )?node->key   :       YY; // trovo o ricordo TK
  const char   * TL = (node->lexema)?node->lexema:       ZZ; // trovo o ricordo TL
  bool           T1 = (node->testo1)?1           :        0; // trovo o ricordo T1
  bool           T2 = (node->testo2)?1           :        0; // trovo o ricordo T2
  // Adesso i valori correnti sono: 
  // o nuovi 
  // o uguali a quelli di uno dei nodi precedenti.
                                    /* Check nodeVSparent */
  bool           xx = (XX<TR      )?1           :        0;// TR e' aumentato
  //bool           yy = (YY!=TK      )?1           :        0;// TK cambiato
  //bool           zz = (ZZ!=TL      )?0           :        1;
  // A capo
  if(xx){
  if(TR-XX>10)
  {
  spaziatura2stringa(view);
  }
  else
  if(T1||T2)
  {
  for(int ii=0;ii<TR-XX;ii++)
  {
  acapo2stringa(view);
  }
  }
  CX=0;
  }
  aperturaquadra2stringa(view);
  aperturagraffa2stringa(view);
  longint2outfile(       view, XX);
  pausabreve2stringa(    view);
  longint2outfile(       view, TR);
  pausabreve2stringa(    view);
  longint2outfile(       view, T1);
  pausabreve2stringa(    view);
  longint2outfile(       view, T2);
  chiusuragraffa2stringa(view);
  pausabreve2stringa(    view);
  longint2outfile(       view, CX++);
  pausabreve2stringa(    view);
  quotazione2stringa(    view);
  stringa2outfile(       view, TL);
  quotazione2stringa(    view);
  pausabreve2stringa(    view);
  aperturagraffa2stringa(view);
  longint2outfile(       view, YY);
  pausabreve2stringa(    view);
  longint2outfile(       view, TK);
  chiusuragraffa2stringa(view);
  chiusuraquadra2stringa(view);
                                     view->XX=TR ;           // registro la modifica
                                     view->YY=TK ;           // registro la modifica
                                     view->ZZ=TL ;           // registro la modifica
                                     view->CX=CX ;           // registro la modifica
  CX++;
  //aperturagraffa2stringa(view);
  /*
  if(            yy){
  quotazione2stringa(view);
  stringa2outfile(view,"y");
  quotazione2stringa(view);
  esplicazione2stringa(view);
  quotazione2stringa(view);
  longint2outfile(view,TK);
  quotazione2stringa(view);
  pausamedia2stringa(view);
  }
  if(            zz){
  quotazione2stringa(view);
  stringa2outfile(view,"z");
  quotazione2stringa(view);
  esplicazione2stringa(view);
  quotazione2stringa(view);
  stringa2outfile(view,TL);
  quotazione2stringa(view);
  pausamedia2stringa(view);
  }
  */
  if(T1){
    node2view(view,node->testo1);
    view->XY=1;
    view->YX=0;
  }
  if(T2){
    node2view(view,node->testo2);
    view->XY=0;
    view->YX=1;
  }
  } // se non esiste il nodo non faccio alcunche'
  } // ma gia' prima: se non esistono i parametri non faccio alcunche'
}
/*

  AFTER-PARSING

*/
// parser2stampa
// FUNZIONE DI STAMPA DELL'ALBERO SINTATTICO
void printDocumento(
  struct testo * node
  ){
  PARAMETRI * view;
  latexFileName = new char[strlen(fileName)+10];
  if (opzioneTestEx()) {
    sprintf(latexFileName, "%s-%s%s", fileName, opzioneTestEx(), FILEEXTENSION);
  } else {
    sprintf(latexFileName, "%s%s", fileName, FILEEXTENSION);
  }
  view = new PARAMETRI   ();
  view->setFileName(latexFileName);
  if (node) {
    node2node(node, view, NULL);
  }
  delete(view);
}
/*

  PROGRAMMA PRINCIPALE

*/
// m2m___2parser
// Every program requires at least one function, called ‘main’. 
// This is where the program begins executing.
int main (int argn, char ** argv) {
  // To accept command line parameters, 
  // you need to have two parameters in the main function, 
  // int argc followed by char *argv[]. 
  char * fileName = NULL;
  bool ok         = true;
  bool help       = false;
  testEx          = NULL;
  nSpaces         = 0;
  // ITERAZIONE PER GESTIRE LA RIGA DI COMANDO
  for (int i = 1; i < argn; i++) {
    // CONTROLLO DEGLI ARGOMENTI
    switch (argv[i][0]) {
      // CONTROLLO LA PRESENZA DI OPZIONI
      case '-' :
        switch (argv[i][1]) {
          break;
          // MOSTRA L'HELP
          case 'h':
            help = true;
            break;
          // ESTRATTO DI UN TESTIMONE
          case 't':
            i++;
            testEx = argv[i];
            break;
        }
        break;
      default:
        // NOME DEL FILE DA ANALIZZARE
        if (fileName == NULL) {
          fileName = argv[i];
        } else {
          ok = false;
        }
        break;
      }
    }
  // VERIFICA CHIAMATA CON NOME FILE
  if (fileName == NULL) {
    ok = false;
  }
  // AVVIO DELL'ANALISI DEL FILE SE NON E' STATO RICHIESTO L'HELP
  if (ok && !help) {
    // INVOCAZIONE DELL'ANALIZZATORE SINTATTICO // file2parser
    parser(fileName);
  } else {
    // STAMPA L'HELP
    printf("traduttore MauroTeX -> M-TeX (%s)\n", VERSIONSTRING);
    printf("uso: %s [-h] [-t <testimone>] [<texfile>]\n", argv[0]);
    if (help) {
      printf("opzioni:\n");
      printf("  -h                       stampa questo messaggio\n");
      printf("  -t <testimone> <texfile> genera il file del testo di <testimone>\n");
      printf("                 <texfile> traduce il documento <texfile>\n");
      printf("\n");
    }
  }
}
// (See: https://www.gnu.org/software/gnu-c-manual/gnu-c-manual.html#The-main-Function)
