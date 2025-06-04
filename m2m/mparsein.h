/*

  file "mparsein.h"
  DEFINIZIONI COMUNI ALL'ANALIZZATORE SINTATTICO E A QUELLO LESSICALE

  mparse: controllore sintattico e generatore albero sintattico per M-TeX
  versione 1.10.b4 del 19 febbraio 2005

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

#include <stdio.h>

// TIPO DI DATO DELL'ANALIZZATORE LESSICALE E SINTATTICO
#define YYSTYPE yystype

typedef struct yystype {
  const char        *lexema;
  struct testo      *testo;
  int               ln;
} yystype;

// FILE SORGENTE
extern FILE * yyin;

// NUMERO DELLA LINEA ATTUALE DEL SORGENTE
void resetnumbers (void);
int getlinenumber (void);
int getunitnumber (void);
