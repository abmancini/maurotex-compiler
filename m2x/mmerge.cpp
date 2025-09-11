#define VERSIONSTRING "versione 1.10.a4 del 19 marzo 2005"

/*

  file "mmerge.c"
  PROGRAMMA PRINCIPALE

  mmerge: merge di collazioni M-TeX sullo stesso testo base

  eLabor scrl
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

#include <string.h>

#include "mparse.h"
#include "mparsey.hpp"

// VARIABILI LOCALI
static struct testo * rifer = NULL;
static struct testo * merge;
static bool  errore = false;
static short nTokens;
static struct testo * testimoni1;
static struct testo * testimoni2;

// FUNZIONI PER OPZIONI GESTITE ESTERNAMENTE
bool verboseWarning() {
  return false;
}

// PROTOTIPI FUNZIONI ESTERNE
struct testo * makeTesto(long           ln,
			 long           key,
			 const char   * lexema,
			 struct testo * testo1,
			 struct testo * testo2);

// PROTOTIPI FUNZIONI INTERNE
bool mergeDocumento(struct testo *  t1, struct testo *  t2,
		    struct testo ** n1, struct testo ** n2,
		    bool            swap,
		    struct testo ** testimoni1,
		    struct testo ** testimoni2,
		    struct testo ** merge);

// STAMPA DI UNA REGOLA
void printTesto(FILE * outFile, struct testo * testo);

// STAMPA DEL FILE DI OUTPUT
void printMTeX(const char * outFileName, struct testo * testo);
void printSigle(FILE * outFile, struct testo * sigle);

// STAMPA DEI TESTIMONI COLLAZIONATI
bool printTestimoni(FILE * outFile, struct testo * t, bool sep);

// FUNZIONI DI DEBUG
void printDebug(struct testo * testo, short livello) {

  short i;
  
  for (i = 0; i < livello; i++) {
    fprintf(stderr, "  ");
  }
  if (testo) {
    fprintf(stderr, "%ld(", testo->key);
    if (testo->lexema) fprintf(stderr, "%s", testo->lexema);
    fprintf(stderr, ",\n");
    printDebug(testo->testo1, livello+1);
    fprintf(stderr, ",\n");
    printDebug(testo->testo2, livello+1);
    fprintf(stderr, ")");
  }
}

void printCheckDebug(struct testo * t1, struct testo * t2) {
  fprintf(stderr, "\n<---------\n");
  printDebug(t1, 0);
  fprintf(stderr, "\n----------\n");
  printDebug(t2, 0);
  fprintf(stderr, "\n--------->\n");
}

void printCheck(struct testo * t1, struct testo * t2, bool swap) {

  struct testo * testo1;
  struct testo * testo2;

  if (swap) {
    testo1 = t2;
    testo2 = t1;
  } else {
    testo1 = t1;
    testo2 = t2;
  }

  nTokens = 20;
  fprintf(stderr, "\ndocumento di riferimento: -->\n");
  printMTeX(NULL, testo1);

  nTokens = 20;
  fprintf(stderr, "\n%s.tex:%ld: -->\n", fileName,
	 (testo2 == NULL) ? testo1->ln : testo2->ln);
  printMTeX(NULL, testo2);
  fprintf(stderr, "\n---------\n");
  nTokens = 0;
}

// STAMPA DEL FILE DI OUTPUT
void printMTeX(const char * outFileName, struct testo * testo) {

  FILE * outFile;

  if (outFileName != NULL) {
    outFile = fopen(outFileName, "w");
  } else {
    outFile = stderr;
  }

  if (outFile) {
    printTesto(outFile, testo);
  } else {
    fprintf(stderr, "impossibile aprire il file %s\n", outFileName ? outFileName : "NULL");
  }

  if (outFileName) {
    fclose(outFile);
  }

}

// CONTROLLO TIPO DI NODO
bool checkKey(struct testo * testo, short key) {
  return testo != NULL && testo->key == key;
}

// CONTROLLO CONSISTENZA DI UN NODO
bool checkNodo(struct testo * t1, struct testo * t2) {

  bool ok = true;

  // CONTROLLO TIPO DI NODO
  if (t1->key != t2->key) {
    ok = false;
  } else {

    // CONTROLLO LEXEMA
    if ((t1->lexema == NULL) != (t2->lexema == NULL)) {
      ok = false;
    } else {

      // CONTROLLO ESISTENZA LEXEMA
      if (t1->lexema) {

	// CONTROLLO UGUAGLIANZA LEXEMA
	ok = (strcmp(t1->lexema, t2->lexema) == 0);
      }
    }
  }

  return ok;
}

bool checkTesto(struct testo * t1, struct testo * t2) {

  bool ok = true;

  if ((t1 == NULL) == (t2 == NULL)) {

    if (t1 != NULL) {
      if (t1->key == COMMKEY || t1->key == ADNOTKEY) {
	ok = checkTesto(t1->testo2, t2);
      } else if (t2->key == COMMKEY || t2->key == ADNOTKEY) {
	ok = checkTesto(t1, t2->testo2);
      } else {
	ok =
	  checkNodo(t1,t2) &&
	  checkTesto(t1->testo1, t2->testo1) &&
	  checkTesto(t1->testo2, t2->testo2);
      }
    }
  } else {
    ok = false;
  }

  return ok;
}

// CONTROLLA CHE IL TESTO SIA FINITO (A MENO DI SPAZI)
bool checkEnd(struct testo * testo) {

  while(testo != NULL && testo->key == SPACES) {
    testo = testo->testo2;
  }

  return (testo == NULL);

}
    

// CONCATENAZIONE DI DUE LISTE
struct testo * concatenaListe(struct testo * l1, struct testo * l2) {
  
  struct testo *l = NULL, *t;
  
  if (l1 == NULL) {
    l = l2;
  } else if (l2 == NULL) {
    l = l1;
  } else {
    
    // SCANSIONE PRIMA LISTA
    l = makeTesto(l1->ln, l1->key, l1->lexema, l1->testo1, NULL);
    l1 = l1->testo2;
    t = l;
    while (l1) {
      t->testo2 = makeTesto(l1->ln, l1->key, l1->lexema, l1->testo1, NULL);
      l1 = l1->testo2;
      t = t->testo2;
    }
    while (l2) {
      t->testo2 = makeTesto(l2->ln, l2->key, l2->lexema, l2->testo1, NULL);
      l2 = l2->testo2;
      t = t->testo2;
    }
  }

  return l;

}

// NORMALIZZAZIONE DI UNA LISTA DI SIGLE
void normalizzaSigle(struct testo * s) {

  struct testo *t, *u;

  // SCANSIONE PRINCIPALE LISTA
  while (s != NULL) {
    
    // INIZIALIZZAZIONE ELEMENTO PRECEDENTE
    u = s;
    
    // INIZIALIZZAZIONE ELEMENTO CORRENTE
    t = s->testo2;
    
    // SCANSIONE PARTE RIMANENTE LISTA
    while (t != NULL) {
      if (strcmp(s->lexema, t->lexema) == 0) {
	
	// ELIMINAZIONE ELEMENTO RIPETUTO
	u->testo2 = t->testo2;
	
      } else {
	
	// AVANZAMENTO ELEMENTO PRECEDENTE
	u = t;
      }
      
      // AVANZAMENTO ELEMENTO CORRENTE
      t = t->testo2;
    }
    
    // AVANZAMENTO ELEMENTO CORRENTE DELLA SCANZIONE PRINCIPALE
    s = s->testo2;
  }
}

// UNIONE DI DUE LISTE DI TESTIMONI
struct testo * unioneSigle(struct testo * s1,
			   struct testo * s2) {

  struct testo *s = concatenaListe(s1,s2);
  
  // ELIMINAZIONE SIGLE RIPETUTE
  normalizzaSigle(s);

  // RESTITUZIONE DELLA LISTA
  return s;
}

// NORMALIZZAZIONE DI UNA LISTA DI VARIANTI
void normalizzaVarianti(struct testo * v) {
  
  struct testo *t, *u;
  
  // SCANSIONE PRINCIPALE LISTA
  while (v != NULL) {
    
    // INIZIALIZZAZIONE ELEMENTO PRECEDENTE
    u = v;
    
    // INIZIALIZZAZIONE ELEMENTO CORRENTE
    t = v->testo2;
    
    // SCANSIONE PARTE RIMANENTE LISTA
    while (t != NULL) {
      if (checkTesto(v->testo1->testo2->testo1, t->testo1->testo2->testo1) &&
	  t->testo1->testo2->testo2 == NULL) {
	
	// ELIMINAZIONE ELEMENTO RIPETUTO
	u->testo2 = t->testo2;
	v->testo1->testo1 = unioneSigle(v->testo1->testo1,
					t->testo1->testo1);
	
      } else {
	
	// AVANZAMENTO ELEMENTO PRECEDENTE
	u = t;
      }
      
      // AVANZAMENTO ELEMENTO CORRENTE
      t = t->testo2;
    }
    
    // AVANZAMENTO ELEMENTO CORRENTE DELLA SCANZIONE PRINCIPALE
    v = v->testo2;
  }
}

struct testo * testimoniVarianti(struct testo * varianti) {

  struct testo * testimoni;

  if (varianti == NULL) {
    testimoni = NULL;;
  } else {
    testimoni = concatenaListe(varianti->testo1->testo1,
			       testimoniVarianti(varianti->testo2));
  }

  return testimoni;
}

struct testo * estraiTestimoni(struct testo * testimoni,
			       struct testo * testo);

struct testo * matchTestimone(struct testo * t1,
			      struct testo * t2) {

  struct testo * m = NULL;

  if (t1 != NULL && t2 != NULL) {
    short l1 = strlen(t1->lexema);
    short l2 = strlen(t2->lexema);
    if (strncmp(t1->lexema, t2->lexema, l1) == 0 &&
	(l1 == l2 ||
	 (l1+1 == l2 &&
	  strspn(&t2->lexema[l1], "+123456789") == 1)
	 )
	) {
      m = makeTesto(0, NOKEY, t2->lexema, NULL, NULL);
    } else {
      m = matchTestimone(t1, t2->testo2);
    }
  }

  return m;
}

struct testo * matchTestimoni(struct testo * t1,
			      struct testo * t2) {

  struct testo * m1 = NULL;
  struct testo * m2;

  if (t1 != NULL && t2 != NULL) {
    m1 = matchTestimone(t1,t2);
    m2 = matchTestimoni(t1->testo2, t2);
    if (m1 == NULL) {
      m1 = m2;
    } else {
      m1->testo2 = m2;
    }
  }

  return m1;
}

struct testo * estraiTestimoniLezione(struct testo * testimoni,
				      struct testo * testo) {
  
  struct testo * e = NULL;
  
  if (testo != NULL) {
    e = makeTesto(testo->ln, testo->key, testo->lexema,
		  estraiTestimoni(testimoni, testo->testo1),
		  testo->testo2);
  }

  return e;
}

struct testo * estraiTestimoniCampo(struct testo * testimoni,
				    struct testo * testo) {
  
  struct testo * e = NULL;
  struct testo * testimoniCampo;
  
  if (testo != NULL) {
    testimoniCampo = matchTestimoni(testimoni, testo->testo1);
    if (testimoniCampo != NULL) {
      e = makeTesto(testo->ln, testo->key, testo->lexema,
		    testimoniCampo,
		    estraiTestimoniLezione(testimoni, testo->testo2));
    }
  }

  return e;

}

struct testo * estraiTestimoniListaCampi(struct testo * testimoni,
					 struct testo * testo) {
  
  struct testo * e = NULL;
  struct testo * testimoniCampo;

  if (testo != NULL) {
    testimoniCampo = estraiTestimoniCampo(testimoni, testo->testo1);
    if (testimoniCampo == NULL) {
      e = estraiTestimoniListaCampi(testimoni, testo->testo2);
    } else {
      e = makeTesto(testo->ln, testo->key, testo->lexema,
		    testimoniCampo,
		    estraiTestimoniListaCampi(testimoni, testo->testo2));
    }
  }

  return e;
}

struct testo * estraiTestimoni(struct testo * testimoni,
			       struct testo * testo) {

  struct testo * e = NULL;
  struct testo * testimoniListaCampi;

  if (testo != NULL) {
    if (testo->key == VVKEY) {
      testimoniListaCampi =
	estraiTestimoniListaCampi(testimoni, testo->testo1);
      if (testimoniListaCampi == NULL) {
	e = estraiTestimoni(testimoni, testo->testo2);
      } else {
	e = makeTesto(testo->ln,
		      testo->key,
		      testo->lexema,
		      testimoniListaCampi,
		      estraiTestimoni(testimoni, testo->testo2));
      }
    } else {
      e = makeTesto(testo->ln,
		    testo->key,
		    testo->lexema,
		    estraiTestimoni(testimoni, testo->testo1),
		    estraiTestimoni(testimoni, testo->testo2));
    }
  }

  return e;

}

void concatenaTestoVarianti(struct testo * v, struct testo * t) {

  while (v != NULL) {
    v->testo1->testo2->testo1 =
      concatenaListe(v->testo1->testo2->testo1,
		     estraiTestimoni(v->testo1->testo1, t));
    v = v->testo2;
  }
}

// UNIONE DI VARIANTI
struct testo * unioneVarianti(struct testo * v1,
			      struct testo * v2) {

  struct testo * v = concatenaListe(v1,v2);
  
  // ELIMINAZIONE VARIANTI RIPETUTE
  normalizzaVarianti(v);

  return v;
}

// MERGE DI DUE NODI VARIANTI
bool mergeSimm(struct testo *  t1, struct testo *  t2,
	       struct testo ** n1, struct testo ** n2,
	       bool            swap,
	       struct testo ** testimoni1,
	       struct testo ** testimoni2,
	       struct testo ** merge) {
  
  struct testo *m1, *m3, *m4, *lezione = NULL;
  struct testo * testimoniVV1 = t1->testo1->testo1->testo1;
  struct testo * testimoniVV2 = t2->testo1->testo1->testo1;
  bool ok;

  // MERGE PRIMA VARIANTE
  ok = mergeDocumento(t1->testo1->testo1->testo2->testo1,
		      t2->testo1->testo1->testo2->testo1,
		      n1, n2, swap, &testimoniVV1, &testimoniVV2, &m1);

  if (ok) {
    
    // MERGE EVENTUALE PARTE RIMANENTE DELLA VARIANTE
    if (*n1 == NULL) {
      ok = mergeDocumento(t1->testo2, *n2,
			  n1, n2, swap, testimoni1, &testimoniVV2, &m3);
      t1->testo2 = *n1;
      concatenaTestoVarianti(t1->testo1->testo2, m3);
      
      // PREPARAZIONE PRIMA LEZIONE
      lezione = makeTesto
	(t1->ln, NOKEY, NULL,
	 concatenaListe(m1, estraiTestimoni
			(unioneSigle(testimoniVV1, testimoniVV2), m3)),
	 NULL);

    } else {
      ok = mergeDocumento(t2->testo2, *n1,
			  n2, n1, !swap, testimoni2, &testimoniVV1, &m3);
      t2->testo2 = *n2;
      concatenaTestoVarianti(t2->testo1->testo2, m3);
      
      // PREPARAZIONE PRIMA LEZIONE
      lezione = makeTesto
	(t1->ln, NOKEY, NULL,
	 concatenaListe(m1, estraiTestimoni
			(unioneSigle(testimoniVV1, testimoniVV2), m3)),
	 NULL);
      
    }
    
    // MERGE PARTE RIMANENTE
    ok = mergeDocumento(t1->testo2, t2->testo2, n1, n2, swap,
			testimoni1, testimoni2, &m4);
    
    if (ok) {
      
      // CREAZIONE STRUTTURA MERGE
      *merge =
	makeTesto(t1->ln,
		  t1->key, // NODO VARIANTE
		  t1->lexema,
		  makeTesto(t1->ln,
			    NOKEY, // LISTA VARIANTI
			    NULL,
			    makeTesto(t1->ln,
				      NOKEY, // PRIMA VARIANTE
				      NULL,
				      unioneSigle(testimoniVV1, testimoniVV2),
				      lezione),
			    unioneVarianti(t1->testo1->testo2,
					   t2->testo1->testo2)),
		  m4); // PARTE RIMANENTE
    }
    
  } else {
    fprintf(stderr, "\ntesti base diversi nelle varianti:\n");
    printCheck(t1->testo1->testo1->testo2->testo1,
	       t2->testo1->testo1->testo2->testo1,
	       swap);
  }
  
  return ok;
  
}

// MERGE DI UN NODO VARIANTE ED UNO NO
bool mergeAsimm(struct testo *  t1, struct testo *  t2,
		struct testo ** n1, struct testo ** n2,
		bool            swap,
		struct testo ** testimoni1,
		struct testo ** testimoni2,
		struct testo ** merge) {
  
  struct testo *m1, *m3;
  struct testo * testimoniVV1 = t1->testo1->testo1->testo1;
  bool ok;

  // PRIMO NODO VARIANTE E SECONDO NO
  ok = mergeDocumento(t1->testo1->testo1->testo2->testo1, t2,
		      n1, n2, swap, &testimoniVV1, testimoni2, &m1);
  
  if (ok) {
    
    // GESTIONE EVENTUALE PARTE RIMANENTE DELLA VARIANTE
    if (*n1 == NULL) {
      
      // RIPRENDE IL MERGE DAL PUNTO DOVE E` ARRIVATO
      ok = mergeDocumento(t1->testo2, *n2, n1, n2, swap,
			  testimoni1, testimoni2, &m3);
      
    } else {
      
      // SCORPORA LA PARTE RIMANENTE DELLA VARIANTE
      m3 = NULL;
      *n1 =
	makeTesto((*n1)->ln,
		  VVKEY,
		  NULL,
		  makeTesto((*n1)->ln,
			    NOKEY,
			    NULL,
			    makeTesto((*n1)->ln,
				      NOKEY,
				      NULL,
				      t1->testo1->testo1->testo1,
				      makeTesto((*n1)->ln,
						NOKEY,
						NULL,
						*n1, NULL)),
			    //(t1->testo1->testo2 == NULL) ?
			    //NULL :
			    makeTesto((*n1)->ln,
				      NOKEY,
				      NULL,
				      makeTesto((*n1)->ln,
						NOKEY,
						NULL,
						testimoniVarianti
						(t1->testo1->testo2),
						makeTesto((*n1)->ln,
							  NOKEY,
							  NULL,
							  makeTesto((*n1)->ln,
								    OMKEY,
								    NULL,
								    NULL,
								    NULL),
							  NULL)),
				      NULL)),
		  t1->testo2);
    }
  }
  
  if (ok) {
    
    // CREAZIONE NUOVO NODO VARIANTE
    *merge =
      makeTesto(t1->ln,
		t1->key, // NODO VARIANTE
		t1->lexema,
		makeTesto(t1->ln,
			  NOKEY,
			  NULL,
			  makeTesto(t1->ln,
				    NOKEY,
				    NULL,
				    unioneSigle(testimoniVV1, *testimoni2),
				    makeTesto(t1->ln, NOKEY, NULL, m1, NULL)),
			  t1->testo1->testo2),
		m3); // PARTE RIMANENTE
  }
  
  return ok;
  
}

// MERGE DI DUE NODI GENERICI
bool mergeDocumento(struct testo * t1,  struct testo * t2,
		    struct testo ** n1, struct testo ** n2,
		    bool   swap,
		    struct testo ** testimoni1,
		    struct testo ** testimoni2,
		    struct testo ** merge) {
  
  bool ok = true;
  struct testo * m1;
  struct testo * m2;
  
  // INIZIALIZZAZIONE ALBERO DI MERGE
  *merge = NULL;
  
  // INIZIALIZZAZIONE DEI PUNTATORI AI SEGUITI
  *n1 = NULL;
  *n2 = NULL;
  
  if (t1 != NULL || t2 != NULL) {
    
    // I TESTI NON SONO ENTRAMBI FINITI
    if (checkKey(t1, TESTIMONIKEY)) {
      *testimoni1 = t1->testo1;
      ok = mergeDocumento(t1->testo2, t2,
			  n1, n2, swap, testimoni1, testimoni2, &m1);
    } else if (checkKey(t2, TESTIMONIKEY)) {
      *testimoni2 = t2->testo1;
      ok = mergeDocumento(t1, t2->testo2,
			  n1, n2, swap, testimoni1, testimoni2, &m1);
    } else if (t1 == NULL || t2 == NULL) {
      
      // UN TESTO E` FINITO E L'ALTRO NO
      *n1 = t1;
      *n2 = t2;
      
    } else {
      
      // ENTRAMBI I TESTI CONTINUANO
      if (checkKey(t1, COMMKEY) || checkKey(t1, ADNOTKEY)) {
	ok = mergeDocumento(t1->testo2, t2,
			    n1, n2, swap, testimoni1, testimoni2, &m1);
	if (ok) {
	  *merge = makeTesto(t1->ln, t1->key, t1->lexema, t1->testo1, m1);
	}
      } else if (checkKey(t2, COMMKEY) || checkKey(t2, ADNOTKEY)) {
	ok = mergeDocumento(t1, t2->testo2,
			    n1, n2, swap, testimoni1, testimoni2, &m1);
	if (ok) {
	  *merge = makeTesto(t2->ln, t2->key, t2->lexema, t2->testo1, m1);
	}
      } else if (checkKey(t1, SPACES)) {
	if (checkKey(t2, SPACES)) {
	  t2 = t2->testo2;
	}
	ok = mergeDocumento(t1->testo2, t2,
			    n1, n2, swap, testimoni1, testimoni2, &m1);  
	if (ok) {
	  *merge = makeTesto(t1->ln, t1->key, t1->lexema, t1->testo1, m1);
	}
      } else if (checkKey(t2, SPACES)) {
	ok = mergeDocumento(t1, t2->testo2,
			    n1, n2, swap, testimoni1, testimoni2, &m1);  
	if (ok) {
	  *merge = makeTesto(t2->ln, t2->key, t2->lexema, t2->testo1, m1);
	}
      } else if (checkKey(t1, OMKEY)) {
	ok = mergeDocumento(t1->testo2, t2,
			    n1, n2, swap, testimoni1, testimoni2, &m1);
      } else if (checkKey(t2, OMKEY)) {
	ok = mergeDocumento(t1, t2->testo2,
			    n1, n2, swap, testimoni1, testimoni2, &m1);
      } else {
	
	// CONTROLLO DELLE VARIANTI
	if (checkKey(t1, VVKEY) && checkKey(t2, VVKEY)) {
	  
	  // ENTRAMBI I NODI SONO VARIANTI
	  ok = mergeSimm(t1, t2, n1, n2, swap, testimoni1, testimoni2, merge);
	  
	} else if (checkKey(t1, VVKEY) && !checkKey(t2, VVKEY)) {
	  
	  // PRIMO NODO VARIANTE E SECONDO NO
	  ok = mergeAsimm(t1, t2, n1, n2, swap, testimoni1, testimoni2, merge);
	  
	} else if (!checkKey(t1, VVKEY) && checkKey(t2, VVKEY)) {
	  
	  // SECONDO NODO VARIANTE E PRIMO NO
	  ok = mergeAsimm(t2, t1, n2, n1, !swap, testimoni2, testimoni1, merge);
	  
	} else {
	  
	  // NESSUN NODO VARIANTE
	  ok = checkNodo(t1, t2);
	  if (ok) {
	    
	    // CONTROLLO DEI RAMI
	    ok = mergeDocumento(t1->testo1, t2->testo1,
				n1, n2, swap, testimoni1, testimoni2, &m1);
	    if (ok) {
	      ok = mergeDocumento(t1->testo2, t2->testo2, n1, n2, swap,
				  testimoni1, testimoni2, &m2);
	      
	      // CREAZIONE NUOVO NODO
	      *merge = makeTesto(t1->ln, t1->key, t1->lexema, m1, m2);
	    }
	  } else {
	    fprintf(stderr, "\ntesti base diversi:\n");
	    printCheck(t1, t2, swap);
	  }
	}
      }
    }
  }
  
  // VALORE DI RITORNO
  return ok;

}

// FUNZIONE DI POST-ELABORAZIONE DELL'ALBERO SINTATTICO
void printDocumento(struct testo * testo) {

  struct testo *n1, *n2;
  //bool         ok = true;

  testimoni1 = concatenaListe(testimoni1, testimoni2);
  testimoni2 = NULL;

  if (testo) {
    if (rifer) {
      if (mergeDocumento(rifer, testo, &n1, &n2, false,
			 &testimoni1, &testimoni2, &merge)) {
	if (n1 == NULL && n2 == NULL) {
	  fprintf(stderr, "struttura %s.tex corretta\n", fileName);
	  rifer = merge;
	} else {
	  //ok = false;
	  fprintf(stderr, "testi di lunghezza diversa:\n");
	  printCheck(n1, n2, false);
	}
      } else {
	fprintf(stderr,
		"struttura %s.tex diversa -> documento scartato\n",
		fileName);
      }
    } else {
      fprintf(stderr, "memorizzazione struttura di riferimento\n");
      rifer = testo;
    }
  } else {
    errore = true;
  }
}

// STAMPA DI UN LEXEMA
void printLexema(FILE * outFile, const char * lexema) {
  if (lexema) {
    fprintf(outFile, "%s", lexema);
  }
}

// STAMPA DEL PROLOGO
void printPrologo(FILE * outFile, struct testo * prologo) {
}

// STAMPA DI UNA LISTA DI PAROLE
void printLista(FILE * outFile, struct testo * lista) {
  if (lista) {
    fprintf(outFile, "%s", lista->lexema);
    lista = lista->testo2;
    if (lista) {
      fprintf(outFile, ", ");
    }
    printLista(outFile, lista);
  }
}

// STAMPA DI UNA LISTA DI OPZIONI
void printOpzioni(FILE * outFile, struct testo * opzioni) {
  fprintf(outFile, "[");
  printLista(outFile, opzioni);
  fprintf(outFile, "]");
}

// STAMPA DEI TESTIMONI COLLAZIONATI
bool printTestimoni(FILE * outFile, struct testo * t, bool sep) {
  
  while (t != NULL) {
    if (sep) {
      fprintf(outFile, "/");
    }
    fprintf(outFile, "%s", t->lexema);
    sep = true;
    t = t->testo2;
  }

  return sep;
}

// STAMPA DEL CORPO DI UN DOCUMENTO
void printCorpo(FILE * outFile, struct testo * corpo) {
  fprintf(outFile, "\n");
  printTesto(outFile, corpo->testo1->testo1);
  if (testimoni1 != NULL || testimoni2 != NULL) {
    fprintf(outFile, "\n\\Testimoni{");
    printTestimoni(outFile, testimoni1,
		   printTestimoni(outFile, testimoni2, false));
    fprintf(outFile, "}\n");
  }
  fprintf(outFile, "\n\\begin{document}\n");
  printTesto(outFile, corpo->testo1->testo2);
  fprintf(outFile, "\n\n\\end{document}\n");
}

// STAMPA DELLA CLASSE DEL DOCUMENTO
void printClass(FILE * outFile,
		const char * classe,
		struct testo * opzioni,
		struct testo * corpo) {

  fprintf(outFile, "\\documentclass");
  printOpzioni(outFile, opzioni);
  fprintf(outFile, "{%s}\n", classe);
  printCorpo(outFile, corpo);
}

// STAMPA DEI PACKAGE
void printPackage(FILE * outFile,
		  struct testo * opzioni,
		  struct testo * package) {
  fprintf(outFile, "\\usepackage");
  printOpzioni(outFile, opzioni);
  fprintf(outFile, "{");
  printLista(outFile, package);
  fprintf(outFile, "}\n");
}

// STAMPA LE SIGLE DEI TESTIMONI
void printSigle(FILE * outFile, struct testo * sigle) {
  while (sigle) {
    fprintf(outFile, "%s", sigle->lexema);
    sigle = sigle->testo2;
    if (sigle) {
      fprintf(outFile, "/");
    }
  }
  fprintf(outFile, ":");
}

void printSottocampo(FILE * outFile, struct testo * sottocampo) {
  if (sottocampo != NULL) {
    switch (sottocampo->key) {
    case TRKEY:
      fprintf(outFile, "\\DES{");
      printTesto(outFile, sottocampo->testo1);
      fprintf(outFile, "}");
      break;
    case EDKEY:
      fprintf(outFile, "\\ED{");
      printTesto(outFile, sottocampo->testo1);
      fprintf(outFile, "}");
      break;
    case POSTSCRIPTKEY:
      fprintf(outFile, "\\POSTSCRIPT");
      break;
    case EXKEY:
      fprintf(outFile, "\\EX{");
      printTesto(outFile, sottocampo->testo1);
      fprintf(outFile, "}");
      break;
    case INTERLKEY:
      fprintf(outFile, "\\INTERL");
      break;
    case SUPRAKEY:
      fprintf(outFile, "\\SUPRA");
      break;
    case MARGKEY:
      fprintf(outFile, "\\MARG");
      break;
    case PCKEY:
      fprintf(outFile, "\\PC");
      break;
    case MARGSIGNKEY:
      fprintf(outFile, "\\MARGSIGN");
      break;
    case ANTEDELKEY:
      fprintf(outFile, "\\ANTEDEL{");
      printTesto(outFile, sottocampo->testo1);
      fprintf(outFile, "}");
      break;
    case POSTDELKEY:
      fprintf(outFile, "\\POSTDEL{");
      printTesto(outFile, sottocampo->testo1);
      fprintf(outFile, "}");
      break;
    case LECKEY:
      fprintf(outFile, "\\LEC{");
      printTesto(outFile, sottocampo->testo1);
      fprintf(outFile, "}");
      break;
    default:
      fprintf(outFile, "\\UNKNOWN{}");
    }
  }
}

void printSottocampi(FILE * outFile, struct testo * sottocampi) {
  if (sottocampi != NULL) {
    printSottocampo(outFile, sottocampi->testo1);
    printSottocampi(outFile, sottocampi->testo2);
  }
}

// STAMPA I SOTTOCAMPI
void printListaSottocampi(FILE * outFile, struct testo * sottocampi) {
  if (sottocampi) {
    printSottocampi(outFile, sottocampi);
    fprintf(outFile, ":");
  }
}

// STAMPA DI UNA VARIANTE
void printVariante(FILE * outFile, struct testo * variante) {
  fprintf(outFile, "{");
  if (variante != NULL) {
    printSigle(outFile, variante->testo1);
    printListaSottocampi(outFile, variante->testo2->testo2);
    printTesto(outFile, variante->testo2->testo1);
  }
  fprintf(outFile, "}\n");
}

// STAMPA DI UNA LISTA DI VARIANTI
void printVarianti(FILE * outFile, struct testo * vv) {

  while (vv != NULL) {
    printVariante(outFile, vv->testo1);
    vv = vv->testo2;
  }

}

// STAMPA DI UNA VV
void printVV(FILE * outFile, struct testo * vv) {
  fprintf(outFile, "\\VV");
  if (vv->lexema) {
    fprintf(outFile, "[%s]", vv->lexema);
  }
  fprintf(outFile, "{\n");
  printVarianti(outFile, vv);
  fprintf(outFile, "}");
}

// STAMPA DI UN TESTO CON DELIMITATORI
void printTestoDel(FILE * outFile, const char * lDel, const char * rDel, struct testo * testo) {
  fprintf(outFile, "%s", lDel);
  printTesto(outFile, testo);
  fprintf(outFile, "%s", rDel);
}

// STAMPA DI UN TESTO TRA DELIMITATORI - SOLO SE IL TESTO NON E` NULLO
void printTestoDelNoNull(FILE * outFile, const char * lDel, const char * rDel, struct testo * testo) {
  if (testo) {
    printTestoDel(outFile, lDel, rDel, testo);
  }
}

// STAMPA DI UN TESTO CENTRATO
void printEnv(FILE * outFile, const char * env, struct testo * testo) {
  fprintf(outFile, "\n\\begin{%s}\n", env);
  printTesto(outFile, testo);
  fprintf(outFile, "\n\\end{%s}\n", env);
}

// STAMPA DI UNA MACRO CON ARGOMENTO OPZIONALE
void printKeyOpt(FILE * outFile, const char * macro, struct testo * testo) {
  fprintf(outFile, "\\%s[", macro);
  printTesto(outFile, testo);
  fprintf(outFile, "]");
}

// STAMPA DI UNA MACRO SENZA ARGOMENTO
void printKeyNoArg(FILE * outFile, const char * macro) {
  fprintf(outFile, "\\%s", macro);
}

// STAMPA DI UNA MACRO
void printKey(FILE * outFile, const char * macro, struct testo * testo) {
  fprintf(outFile, "\\%s{", macro);
  printTesto(outFile, testo);
  fprintf(outFile, "}");
}

// STAMPA DI UNA FRAZIONE
void printFrazione(FILE * outFile, struct testo * frazione) {
  fprintf(outFile, "\\frac");
  printTestoDel(outFile, "{", "}", frazione->testo1);
  printTestoDel(outFile, "{", "}", frazione->testo2);
}

// STAMPA UNA ETICHETTA DI UNIT
void printUnit(FILE * outFile, const char * lexema) {
  fprintf(outFile, "\\Unit");
  if (lexema) {
    fprintf(outFile, "[%s]", lexema);
  }
}

// STAMPA UN FOLIUM
void printFolium(FILE * outFile, struct testo * folium) {
  fprintf(outFile, "\\Folium{%s:", folium->lexema);
  printTesto(outFile, folium->testo2);
  fprintf(outFile, "}");
}

// STAMPA DI UNA REGOLA
void printTesto(FILE * outFile, struct testo * testo) {
  
  // CONTROLLO ESISTENZA TESTO
  while (testo != NULL) {
    
    // CONTROLLO DELLA REGOLA DA APPLICARE
    switch((testo->key > 0x00FF) ? testo->key : testo->key & 0xFF00) {
    case 0: // TUTTE LE CHIAVI INFERIORI A 256
      fprintf(outFile, "%c", int(testo->key));
      break;
    case TILDE:
    case GRAVE:
    case ACUTE:
    case CIRC:
    case UML:
    case MACRO:
      printLexema(outFile, testo->lexema);
      break;
    case COMMENTKEY:
      printKeyNoArg(outFile, "Commenti");
      break;
    case ANNOTAZKEY:
      printKeyNoArg(outFile, "Annotazioni");
      break;
    case CLASSKEY:
      printClass(outFile, testo->lexema, testo->testo1, testo->testo2);
      testo = NULL;
      break;
    case USEPACKAGE:
      printPackage(outFile, testo->testo1->testo1, testo->testo1->testo2);
      break;
    case SPACES:
      printLexema(outFile, " ");
      break;
    case VVKEY:
      printVV(outFile, testo->testo1);
      break;
    case NOKEY:
      printLexema(outFile, testo->lexema);
      printTestoDelNoNull(outFile, "{", "}", testo->testo1);
      break;
    case PARAGRAPH:
      fprintf(outFile, "\n\\par\n\n");
      break;
    case CENTERKEY:
      printEnv(outFile, "center", testo->testo1);
      break;
    case CRUXKEY:
      printKey(outFile, "CRUX", testo->testo1);
      break;
    case EXPUKEY:
      printKey(outFile, "EXPU", testo->testo1);
      break;
    case INTEKEY:
      printKey(outFile, "INTE", testo->testo1);
      break;
    case TITKEY:
      printKey(outFile, "Tit", testo->testo1);
      break;
    case BEGINMATHKEY:
      printTestoDel(outFile, "\\(", "\\)", testo->testo1);
      break;
    case BEGINEQNKEY:
      printTestoDel(outFile, "\\[", "\\]", testo->testo1);
      break;
    case TITLEKEY:
      printKey(outFile, "title", testo->testo1);
      break;
    case AUTHORKEY:
      printKey(outFile, "author", testo->testo1);
      break;
    case MAKETITLEKEY:
      fprintf(outFile, "\\maketitle\n");
      break;
    case LEFTPAR:
      fprintf(outFile, "(");
      break;
    case RIGHTPAR:
      fprintf(outFile, ")");
      break;
    case OMKEY:
      fprintf(outFile, "\\OM");
      break;
    case OMLACKEY:
      fprintf(outFile, "\\OMLAC");
      break;
    case LACCKEY:
      fprintf(outFile, "\\LACc");
      break;
    case LACMKEY:
      fprintf(outFile, "\\LACm");
      break;
    case LACSKEY:
      fprintf(outFile, "\\LACs");
      break;
    case LEFTANG:
      printTestoDel(outFile, "<", ">", testo->testo1);
      break;
    case SECTIONKEY:
      printTestoDel(outFile, "\n\\section{", "}", testo->testo1);
      break;
    case SUBSECTIONKEY:
      printTestoDel(outFile, "\n\\subsection{", "}", testo->testo1);
      break;
    case SSSECTIONKEY:
      printTestoDel(outFile, "\n\\subsubsection{", "}", testo->testo1);
      break;
    case FRACKEY:
      printFrazione(outFile, testo->testo1);
      break;
    case ENUNCIATIO:
    case PROTASIS:
    case TABULAR:
    case GENERICENV:
      printEnv(outFile, testo->testo1->lexema, testo->testo1->testo2);
      break;
    case SYMBOL:
      fprintf(outFile, "%s", testo->lexema);
      break;
    case UNITKEY:
      printUnit(outFile, testo->lexema);
      break;
    case FOLIUMKEY:
      printFolium(outFile, testo->testo1);
      break;
    case COMMKEY:
      printKey(outFile, "Comm", testo->testo1);
      break;
    case CONGKEY:
      printKey(outFile, "Cong", testo->testo1);
      break;
    case ADNOTKEY:
      printKey(outFile, "Adnotatio", testo->testo1);
      break;
    case NOTEKEY:
      printKey(outFile, "note", testo->testo1);
      break;
    case HTMLCUTKEY:
      printKeyOpt(outFile, "htmlcut", testo->testo1);
      break;
    }
    
    // SE IL TESTO SUCCESSIVO NON E` GIA` STATO TRATTATO
    if (nTokens == 1) {
      testo = NULL;
      fprintf(outFile, "...");
    } else {
      if (nTokens > 1) {
	nTokens--;
      }
    }
    if (testo) {
      testo = testo->testo2;
    }

  }
}

// PROGRAMMA PRINCIPALE
int main (int npar, char ** vpar) {

  const char * outFileName = "mmerge.tex";
  short i;
  bool help = false;
  testimoni1 = NULL;
  testimoni2 = NULL;

  if (npar > 1) {
    
    // VERIFICA OPZIONE DI HELP
    if (strcmp(vpar[1], "-h") == 0) {
      help = true;
    }
  } else {
    help = true;
  }
  
  if (help) {

    // STAMPA L'HELP
    printf("merge collazioni MauroTeX (%s)\n", VERSIONSTRING);
    printf("uso: %s -h\n", vpar[0]);
    printf("     %s <file> [<file> ...]\n", vpar[0]);
    if (help) {
      printf("opzioni:\n");
      printf("  -h             stampa questo messaggio\n");
      printf("  -o <file>      usa <file> come file di uscita\n");
    }
  } else {
    
    // SCANSIONE DEI FILE DI INPUT
    for (i = 1; !help && !errore && i < npar; i++) {
      if (strcmp(vpar[i], "-o") == 0) {
	i++;
	if (i < npar) {
	  outFileName = vpar[i];
	} else {
	  fprintf(stderr, "inserire il nome del file di uscita\n");
	}
      } else {
	parser(vpar[i]);
      }
    }
    
    if (!errore) {

      // STAMPA FILE MTEX
      printMTeX(outFileName, rifer);
    }
  }
}
