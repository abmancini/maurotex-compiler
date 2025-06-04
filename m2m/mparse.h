/*

  file "mparse.h"
  DEFINIZIONI PER L'UTILIZZAZIONE ESTERNA

  mparse: controllore sintattico e generatore albero sintattico per M-TeX
  versione 1.12.b8 del 23 dicembre 2011

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

#ifndef __MPARSE_H__

#define __MPARSE_H__

#include <stdio.h>
#include "comp.h"

// NODO DELL'ALBERO SINTATTICO

struct testo {
  long           ln;
  struct testo * testo1;
  long           key;
  const char   * lexema;
  struct testo * testo2;
};

extern char * fileName; // ARGOMENTO DEL PROGRAMMA

// FUNZIONI PER EMISSIONE DI UN MESSAGGIO (DEFINITE INTERNAMENTE)

void yyemessage (int ln, const char * message);
void yywmessage (int ln, const char * message);
bool verboseWarning(void);

// FUNZIONI DI RITROVAMENTO DELLE ETICHETTE

typedef struct rif {
  const char   * label;
  int            nUnit;
  struct testo * testo;
  rif          * next;
} rif;

typedef struct labelKey {
  const char   * label;
  int            sect;
  int            ssect;
  int            sssect;
  labelKey     * next;
} labelKey;

labelKey * findLabelKey(const char * label);

rif * findLabel(const char * label); // PER ETICHETTE COMANDO LB

int findUnit(const char * label);    // PER ETICHETTE COMANDO Unit

// FUNZIONE CHE ESEGUE L'ANALISI SINTATTICA DEL FILE
// DEFINITA IN (MPARSE.Y)

void parser (char * fileName);

// FUNZIONE DI POST-ELABORAZIONE DELL'ALBERO SINTATTICO (DA DEFINIRE)

void printDocumento(struct testo * testo);

// FUNZIONE DI STAMPA DELLA TABELLA DEI SIMBOLI

void dumpsymtable(FILE * ok, FILE * ko);

#endif
