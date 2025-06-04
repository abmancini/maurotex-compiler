/*

  file "mvisit.c"
  IMPLEMENTAZIONE DI MVIST

  mvisit: visitatore generalizzato di documenti MTeX
  versione 1.13b14 del 21 aprile 2018

  eLabor sc
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

#include "mparse.h"
#include "mparsey.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "mvisit.h"

#define MAXCONF 1000
#define CONFEST  ".cnf"

// VARIABILI GLOBALI
static Configurazione configurazioni[MAXCONF];
static short nConf = 0;         // NUMERO DELLE CONFIGURAZIONI

// STAMPA L'ALBERO SINTATTICO PER DEBUG
void printDebug(FILE * outFile, struct testo * testo) {
  if (testo) {
    fprintf(outFile, "(%ld, %s, ", testo->key, (testo->lexema) ? testo->lexema : "");
    printDebug(outFile, testo->testo1);
    fprintf(outFile, ", ");
    printDebug(outFile, testo->testo2);
    fprintf(outFile, ") ");
  }
}

// RITORNA IL NUMERO DELLE CONFIGURAZIONI
short getNConf() {
  return nConf;
}

// FUNZIONE DI LETTURA DEL FILE DI CONFIGURAZIONE
void leggiConfigurazione(char * confFileName) {

  short iConf;
  
  // GENERAZIONE DEL NOME DEL FILE
  if (confFileName == NULL) {
    confFileName = new char[strlen(fileName)+10];
    sprintf(confFileName, "%s%s", fileName, CONFEST);
  }
  
  // APERTURA DEL FILE
  FILE * confFile = fopen(confFileName, "r");
  if (confFile) {
    
    short nRiga = 0;
    
    // LETTURA DELLE RIGHE DEL FILE
    printf("analisi del file di configurazione (%s)\n", confFileName);
    while (!feof(confFile)) {
      
      // CONTROLLO DEL NUMERO DI CONFIGURAZIONI
      if (nConf < MAXCONF) {
		
		char riga[1000];
		char *r = fgets(riga, sizeof(riga), confFile);
		nRiga++;
		
		if (r != NULL && strlen(riga) > 5 && *riga != '#') {
		  char * tok = strtok(riga, "\t\n\r");
		  if (tok) {
			configurazioni[nConf].setSglaut(tok);
			tok = strtok(NULL, "\t\n\r");
		  }
		  if (tok) {
			configurazioni[nConf].setSglope(tok);
			tok = strtok(NULL, "\t\n\r");
		  }
		  if (tok) {
			configurazioni[nConf].setNomaut(tok);
			tok = strtok(NULL, "\t\n\r");
		  }
		  if (tok) {
			configurazioni[nConf].setTitope(tok);
			tok = strtok(NULL, "\t\n\r");
		  }
		  if (tok) {
			configurazioni[nConf].setTxtOpeFileName(tok);
			tok = strtok(NULL, "\t\n\r");
		  }
		  if (tok) {
			configurazioni[nConf].setHtmlOpeFileName(tok);

			// VERIFICA NON ESISTENZA CONFIGURAZIONE
			iConf = trovaConfigurazione(configurazioni[nConf].sglaut, configurazioni[nConf].sglope);
			if (iConf < 0) {
			  nConf++;
			} else {
			  fprintf
				(stderr,
				 "la riga %d e` stata scartata (%s/%s gia` presente)\n",
				 nRiga, configurazioni[nConf].sglaut, configurazioni[nConf].sglope);
			}
		  } else {
			fprintf
			  (stderr,
			   "la riga %d non contiene tutti i campi richiesti (verificare l'uso degli spazi)\n",
			   nRiga);
		  }
		}
      } else {
		fprintf(stderr, "il numero delle righe di configurazione eccede %d\n", MAXCONF);
      }
    }
    
    // CHIUSURA DEL FILE
    fclose(confFile);
    
    // STAMPA DI PROVA
    printf("opere da citare:\n");
    for (iConf = 0; iConf < nConf; iConf++) {
      configurazioni[iConf].print();
    }
    
  } else {
    if (verboseWarning()) {
      fprintf(stderr, "\nERRORE: impossibile leggere il file %s\n\n", confFileName);
    }
  }
}

// RITORNA LA CONFIGURAZIONE
Configurazione * getConfigurazione(short iConf) {
  return &configurazioni[iConf];
}

// STAMPA IL RIFERIMENTO DELLE CITAZIONI
void printRiferimento(PARAMETRI * parametri,
		      short       iConf,
		      const char      * libro,
		      const char      * prop) {
  
  // STAMPA DEL NOME AUTORE DEL TITOLO DELL'OPERA
  fprintf(parametri->outFile, "%s %s ",
	  configurazioni[iConf].nomaut, configurazioni[iConf].titope);
  
  // STAMPA DEL LIBRO
  if (libro) {
    fprintf(parametri->outFile, "%s", libro);
  }
  
  // STAMPA DELLA PROPOSIZIONE
  if (prop) {
    fprintf(parametri->outFile, ".%s", prop);
  }
}

// RICERCA DLL'INDICE DELLE SIGLE (AUTORE E OPERA) NEL FILE DI CONFIGURAZIONE
short trovaConfigurazione(const char * sglaut, const char * sglope) {
  
  short iConf = 0;
  
  // CICLO PER TROVARE L'INDICE DELLA CONFIGURAZIONE
  while (iConf < nConf) {
    
    Configurazione * c = &configurazioni[iConf];
    
    // SE LE SIGLE SONO UGUALI HO TROVATO L'INDICE
    if (strcmp(c->sglaut, sglaut) == 0 && strcmp(c->sglope, sglope) == 0) break;
    
    iConf++;
  }
  
  // VALORE DI RITORNO (-1 -> NON TROVATO)
  return (iConf >= nConf) ? -1 : iConf;

}

// RICOSTRUISCE IL CAMPO PROPOSIZIONE DI UNA CITAZIONE
char * extractProp(struct testo * testo) {

  char * retval = new(char[100]);

  retval[0] = 0;
  
  // printDebug(stderr, testo);
  // fprintf(stderr, "\n");
  for (struct testo * t = testo; t != NULL; t = t->testo2) {
	if (t->lexema == NULL) {
	  switch (t->key) {
	  case LEFTPAR:
		strcat(retval, "(");
		break;
	  case RIGHTPAR:
		strcat(retval, ")");
		break;
	  case SPACES:
		strcat(retval, " ");
		break;
	  }
	} else {
	  if (strlen(retval) + strlen(t->lexema) < 99) {
		strcat(retval, t->lexema);
	  }
	}
  }

  return retval;

}

// TROVA IL RIFERIMENTO DI UNA CITAZIONE
void trovaRiferimento(struct testo * testo,
		      short *iConf, const char ** libro, const char ** prop) {
  
  *iConf = -1;
  *libro = NULL;
  *prop  = NULL;
  
  // IL NODO ESISTE
  if (testo) {
    
    // MEMORIZZAZIONE DELLA SIGLA AUTORE TROVATA NEL TESTO
    const char *sglaut = testo->lexema;
    
    // NON CONOSCO ANCORA LA SIGLA DELL'OPERA
    const char *sglope = NULL;
    
    // ESISTE IL NODO SUCCESSIVO
    if (testo->testo2) {
      
      // CONTROLLO LA PRESENZA DELLA BARRA NEL TESTO
      if (testo->testo2->key == SLASH) {
		// ESISTE IL NODO SUCCESSIVO
		if (testo->testo2->testo2) {
		  // MEMORIZZAZIONE DELLA SIGLA OPERA TROVATA NEL TESTO
		  sglope = testo->testo2->testo2->lexema;
		  
		  // RICERCA DELL'INDICE DELLA CONFIGURAZIONE
		  *iConf = trovaConfigurazione(sglaut, sglope);
		  
		  // HO TROVATO L'INDICE
		  if (*iConf >= 0) {
			
			// ESISTE IL NODO SUCCESSIVO
			if (testo->testo2->testo2->testo2) {
			  // CONTROLLO LA PRESENZA DELLA BARRA NEL TESTO
			  if (testo->testo2->testo2->testo2->key == SLASH) {
				
				// ESISTE IL NODO SUCCESSIVO
				if (testo->testo2->testo2->testo2->testo2) {
				  
				  // MEMORIZZAZIONE DEL LIBRO
				  *libro = testo->testo2->testo2->testo2->testo2->lexema;
				  
				  // ESISTE IL NODO SUCCESSIVO
				  if (testo->testo2->testo2->testo2->testo2->testo2) {
					
					// ESISTE IL NODO SUCCESSIVO
					if (testo->testo2->testo2->testo2->testo2->testo2->testo2) {
					  
					  // MEMORIZZAZIONE DELLA PROPOSIZIONE
					  *prop = extractProp(testo->testo2->testo2->testo2->testo2->testo2->testo2);
					  //fprintf(stderr, "prop: <%s>\n", *prop);
					  /*
					   *prop = testo->testo2->testo2->testo2->testo2->testo2->testo2->lexema;
					   printDebug(stderr, testo->testo2->testo2->testo2->testo2->testo2->testo2);
					   fprintf(stderr, "\n");
					  */
					}
				  }
				}
			  }
			}
		  }
		}
      }
    }
  }
}

// VISITA DI UNA LISTA DI CAMPI DI UNA RIGA DI UNA TABELLA
short visitCampi(short          nRow,
		 const char   * colonne,
		 struct testo * campi,
		 PARAMETRI    * parametri,
		 struct testo * sigle) {
    
    short nCol = 0;
    short nColCampo = 1;
    
    if (campi) {
		nCol = visitCampi(nRow, colonne, campi->testo1, parametri, sigle);
		printCampo(nRow, nCol, colonne, campi->testo2, parametri, sigle);
		if (campi->testo2) {
			if (campi->testo2->key == MULTICOLKEY) {
				nColCampo = atoi(campi->testo2->lexema);
			}
		}
    }
    
    return nCol+nColCampo;
}

// VISITA DI UNA RIGA DI UNA TABELLA
void visitRiga(short          nRow,
	       const char   * colonne,
	       struct testo * riga,
	       PARAMETRI    * parametri,
	       struct testo * sigle) {
	printRowHeader(riga->key, parametri);
	visitCampi(nRow, colonne, riga, parametri, sigle);
	printRowFooter(riga->key, parametri);
}

// VISITA DI UNA LISTA DI RIGHE DI UNA TABELLA
short visitRighe(const char   * colonne,
		 struct testo * righe,
		 PARAMETRI    * parametri,
		 struct testo * sigle) {
	
	short nRow = 0;
	
	if (righe) {
		nRow = visitRighe(colonne, righe->testo1, parametri, sigle);
		visitRiga(nRow, colonne, righe->testo2, parametri, sigle);
	}
	
	return nRow+1;
}

short columnCount(char * colonne) {
	
	short nCol = 0;
	unsigned short i;
	
	for (i = 0; i < strlen(colonne); i++) {
		if (colonne[i] != '|') {
			nCol++;
		}
	}
	
	return nCol;
}

// VISITA DI UNA TABELLA
void visitTable(int key,
		struct testo * tabella,
		PARAMETRI    * parametri,
		struct testo * sigle) {
  printTableStart(key, tabella->testo1->testo1, parametri, sigle);
  printTableHeader(key, tabella->lexema, parametri);
  visitRighe(tabella->lexema, tabella->testo2, parametri, sigle);
  printTableFooter(key, parametri);
  printTableFinish(key, tabella->testo1->testo2, parametri, sigle);
}

// VISITA DI UNA TABELLA
void visitLongTable(int key,
		    struct testo * tabella,
		    PARAMETRI    * parametri,
		    struct testo * sigle) {
  printTableHeader(key, tabella->lexema, parametri);
  visitRighe(tabella->lexema, tabella, parametri, sigle);
  printTableFooter(key, parametri);
}

// VISITA DI UNA MACRO "CASES"
void visitCases(short        key,
	      struct testo * testo,
	      PARAMETRI    * parametri,
	      struct testo * sigle) {
  printCases(key, testo, parametri, sigle);
}

// VERIFICA CHE NELLE SIGLE NON ESISTANO TESTIMONI DIVERSI DA QUELLO DI RIFERIMENTO
bool unicTestRif(struct testo * sigle, char * testRif) {

  bool ok = true;
  
  if (testRif) { // SE IL TESTIMONE DI RIFERIMENTO E` STATO IMPOSTATO
    
    if (sigle) { // SE CI SONO ALTRE SIGLE NELLA LISTA
      
      if (sigle->lexema) {
	if (strncmp(sigle->lexema, testRif, strlen(testRif)) != 0) {
	  ok = false;
	} else {
	  ok = unicTestRif(sigle->testo2, testRif);
	}
      }
    }
  }

  return ok;
}

// VERIFICA CHE ESISTA IL TESTIMONE DI RIFERIMENTO NELLE SIGLE
bool findTestRif(struct testo * sigle, char * testRif) {
  bool ok = false;
  if (testRif) { // SE IL TESTIMONE DI RIFERIMENTO E` STATO IMPOSTATO
    if (sigle) { // SE CI SONO ALTRE SIGLE NELLA LISTA
      if (sigle->lexema) {
		if (strncmp(sigle->lexema, testRif, strlen(testRif)) == 0) {
		  // SE LA SIGLA DEL TESTIMONE CORRISPONDE AL TESTIMONE DI RIFERIMENTO
		  ok = true;
		} else {
		  // ALTRIMENTI CERCA NELLE SIGLE RIMANENTI
		  ok = findTestRif(sigle->testo2, testRif);
		}
      }
    }
  }
  return ok;
}

// VERIFICA CHE ESISTA IL TESTIMONE DI RIFERIMENTO NELLE SIGLE - no manus alterae
bool findTestRifNOMANUSALTERAE(struct testo * sigle, char * testRif) {
  bool ok = false;
  if (testRif) { // SE IL TESTIMONE DI RIFERIMENTO E` STATO IMPOSTATO
    if (sigle) { // SE CI SONO ALTRE SIGLE NELLA LISTA
      if (sigle->lexema) {
		if (strcmp(sigle->lexema, testRif) == 0) {
		  // SE LA SIGLA DEL TESTIMONE CORRISPONDE AL TESTIMONE DI RIFERIMENTO
		  ok = true;
	     /*printf("[%ld *** %s]", sigle->ln, sigle->lexema);
	     if ( sigle->testo1 ) { 
	     if ( sigle->testo1->lexema ) {
	     printf(" %s ", sigle->testo1->lexema); 
	     }
	     }*/
		} else {
		  // ALTRIMENTI CERCA NELLE SIGLE RIMANENTI
		  //printf(" %ld *** %s ", sigle->ln, sigle->lexema);
		  ok = findTestRifNOMANUSALTERAE(sigle->testo2, testRif);
		}
      }
    }
  }
  return ok;
}

// RITORNA UN int DI VERIFICA DI ESISTENZA DEL TESTIMONE DI RIFERIMENTO NELLE SIGLE - no manus alterae
int findTestIdNOMANUSALTERAE(struct testo * sigle, char * testRif) {
  int LV = 1;
  if (testRif) { // SE IL TESTIMONE DI RIFERIMENTO E` STATO IMPOSTATO
    if (sigle) { // SE CI SONO ALTRE SIGLE NELLA LISTA
      if (sigle->lexema) {
		if (strcmp(sigle->lexema, testRif) == 0) {
		  // SE LA SIGLA DEL TESTIMONE CORRISPONDE AL TESTIMONE DI RIFERIMENTO
		  LV++;
		} else {
		  // ALTRIMENTI CERCA NELLE SIGLE RIMANENTI
		  LV = findTestRifNOMANUSALTERAE(sigle->testo2, testRif);
		}
      }
    }
  }
  return LV;
}

// CERCA LA SIGLA CORRISPONDENTE AL TESTIMONE RICHIESTO
bool findSigle(struct testo * testo, char * testimone) {
  
  bool sigla = false;
  
  // SE ESISTE IL TESTO
  if (testo) {
    
    // RITROVA IL TESTIMONE DI RIFERIMENTO NELLA LISTA DELLE SIGLE
    if (findTestRif(testo->testo1, testimone)) {
      sigla = true;
    }
  }

  // RITORNA LA RISPOSTA
  return sigla;
}

// CERCA LA VARIANTE CHE SI RIFERISCE AL TESTIMONE RICHIESTO
struct testo * findVariante(struct testo * testo, char * testimone) {
  
  struct testo * variante = NULL;
  
  // SE IL TESTO ESISTE
  if (testo != NULL && testimone != NULL) {

    // CERCO LA SIGLA DEL TESTIMONE RICHIESTO
    if (findSigle(testo->testo1, testimone)) {
      if (testo->testo1->testo2) {
	variante = testo->testo1->testo2;
      }
    } else {

      // CERCO LA SIGLA DEL TESTIMONE RICHIESTO NEL TESTO SUCCESSIVO
      variante = findVariante(testo->testo2, testimone);
    }
  }

  // RESTITUZIONE DELLA VARIANTE RELATIVA AL TESTIMONE
  return variante; 
}

// CERCA LE VARIANTI LUNGHE PER IL TESTO DEL TESTIMONE CERCATO
const char * findCR(struct testo * testo) {
  
  const char * label = NULL;
  
  // SE ESISTE IL TESTO
  if (testo) {
	
    // SE ESISTE LA NOTA
    if (testo->testo1) {
	  
      // SE ESISTE LA CRKEY
      if (testo->testo1->key == CRKEY) {
		
		// SE ESISTE IL LEXEMA
		if (testo->testo1->lexema) {
		  
		  // MEMORIZZO L'ETICHETTA CHE CHIUDE LA VARIANTE LUNGA
		  label = testo->testo1->lexema;
		}
      } else {
		// CERCO CRKEY NELLE NOTE SUCCESSIVE
		label = findCR(testo->testo2);
      }
    }
  }
  
  // RITORNA LA RISPOSTA
  return label;
}

// CONTROLLA SE CI SONO NOTE SIGNIFICATIVE NELLA VARIANTE
bool checkNote(struct testo * note) {
  
  bool retVal = false;
  
  if (note) {
	
    if (note->testo1) {
	  
      if (note->testo1->key != CRKEY) {
		retVal = true;
      }
    }
    if (!retVal) {
      retVal = checkNote(note->testo2);
    }
  }
  
  return retVal;
}

// CONTROLLA SE LA VARIANTE E` SIGNIFICATIVA RISPETTO AL TESTIMONE
bool checkVariante(struct testo * testo, char * testRif) {

  bool retVal = false;
  
  if (testRif) {
    if (findTestRif(testo->testo1, testRif)) {
      if (checkNote(testo->testo2->testo2)) {
	retVal = true;
      }
    }
  } else {
    retVal = true;
  }

  return retVal;
}

// CONTROLLA SE LE VARIANTI SONO SIGNIFICATIVE RISPETTO AL TESTIMONE
bool checkVarianti(struct testo * testo, char * testRif) {

  bool retVal;

  retVal = checkVariante(testo->testo1, testRif);
  if (testo->testo2 && !retVal) {
    retVal = checkVarianti(testo->testo2, testRif);
  }

  return retVal;
}

// ESTRAZIONE DELLA VARIANTE CHE SI RIFERISCE AL TESTIMONE RICHIESTO
void estrazioneVariante(struct testo * testo,
			PARAMETRI    * parametri,
			struct testo * sigle) {

  struct testo * variante = NULL;
  const char * label = NULL;
  
  // CERCO LA VARIANTE DEL TESTIMONE RICHIESTO
  variante = findVariante(testo, opzioneTestEx());

  // SE NON C'E`, ALLORA PRENDO LA PRIMA VARIANTE
  if (variante == NULL) {
    variante = testo->testo1->testo2;
  }
  
  // CONTROLLO SE ESISTE LA VARIANTE DEL TESTIMONE 
  if (variante->testo1) {
    
    // STAMPO LA LEZIONE DELLA VARIANTE
    visitTesto(variante->testo1, parametri, sigle);
    
    // SE LA VARIANTE NON E` LA PRIMA 
    if (variante != testo->testo1->testo2) {
      
      // CONTROLLO CHE CI SIA LA PRIMA VARIANTE
      if (testo->testo1->testo2) {
	
	// CERCO SE C'E' UNA VARIANTE LUNGA E MEMORIZZO L'ETICHETTA FINALE
	label = findCR(testo->testo1->testo2->testo2);
	
	// INIBIZIONE DELLA STAMPA FINO ALLA CORRISPONDENTE LB
	if (parametri->label == NULL) {
	  parametri->label = label;
	}
      }
    }
  }
}

// FUNZIONE DI STAMPA DI UNA NOTA
void visitVV(short          key,
			 struct testo * testo,
			 PARAMETRI    * parametri,
			 struct testo * sigle) {
  
  // CONTROLLO GESTIONE ESTERNA VARIANTI
  if (checkVV(key, testo, parametri, sigle)) {
	
    // INDIVIDUAZIONE DEL TIPO DI NOTA
    if (key != SCHOLKEY) {
      if (key != NOTEKEY) {
	if (key != NMARGKEY &&
	    ((key == VBKEY && !opzioneBanale()) ||
	     (testo->lexema==NULL || (strcmp(testo->lexema, "unit") != 0)))) {
	  
	  // TESTO CRITICO
	  printInizioTestoCritico(key, parametri);
	  bool banale = parametri->banale;
	  parametri->banale = (opzioneTestEx() == NULL);
	  estrazioneVariante(testo, parametri, sigle);
	  parametri->banale = banale;
	  printFineTestoCritico(key, parametri);
	}
      }
    }
    
    // GESTIONE DELLA NOTA
    if (key != SCHOLKEY) {
      if ((key != VBKEY || opzioneBanale()) && !parametri->banale) {
		
		// VERIFICA CHE ESISTANO DELLA VARIANTI DA STAMPARE
		if (checkVarianti(testo, opzioneTestEx())) {
		  
		  // STAMPA LE VARIANTI
		  if (!parametri->nota) {
			printInizioVarianti(parametri);
		  }
		  printVV(key, testo, parametri, sigle);
		  if (!parametri->nota) {
			printFineVarianti(parametri);
		  }
		}
      }
    } else {
      // VERIFICA CHE ESISTANO DELLA VARIANTI DA STAMPARE
      if (checkVarianti(testo, opzioneTestEx())) {
		
		// STAMPA LE VARIANTI
		printVV(key, testo, parametri, sigle);
      }
    }
  }
}

// VISITA DI UNA LISTA DI VARIANTI E RITORNA IL NUMERO DI TESTIMONI
short visitVarianti(struct testo * testo,
		    PARAMETRI    * parametri,
		    struct testo * sigle,
		    bool           nested,
		    bool           duplex,
		    bool           multiplex,
		    int            numero) {

  short nTest = 0;
  
  // SE CI SONO ALTRE VARIANTI NELLA LISTA
  if (testo) {
    
    // STAMPA LA PRIMA VARIANTE ED UNO SPAZIO SE QUESTA NON E` VUOTA
    if (visitVariante(testo->testo1, parametri, sigle,
		      nested, duplex, multiplex, numero, &nTest) && 
	testo->testo2) {
      printSepVarianti(parametri);
    }
    
    // VERIFICA SE SI TRATTA DI UNA TRASPOSIZIONE "DUPLEX"
    if (testo->lexema) {
      if (strcmp(testo->lexema,"duplex") == 0) duplex = true;
    }
    
    // VERIFICA SE SI TRATTA DI UNA TRASPOSIZIONE "MULTIPLEX"
    if (testo->lexema) {
      if (strcmp(testo->lexema,"multiplex") == 0) multiplex = true;
    }
    
    // STAMPA LE ALTRE VARIANTI
    nTest += visitVarianti(testo->testo2, parametri, sigle,
			   false, duplex, multiplex, numero+1);
  }
  
  // RITORNA IL NUMERO DI TESTIMONI GESTITI
  return nTest;
}

// VISITA UNA VARIANTE
bool visitVariante(struct testo * testo,
		   PARAMETRI    * parametri,
		   struct testo * sigle,
		   bool           nested,
		   bool           duplex,
		   bool           multiplex,
		   int            numero,
		   short        * nTest) {
  
  bool stampato = false; // INDICATORE DI VARIANTE NON VUOTA
  char * testimone;      // TESTIMONE DA ESTRARRE
  
  // SE IL TESTO (1) NON E` VUOTO E (2) NON E` CONTRASSEGNATO DA BANALEKEY
  if (testo && testo->key != BANALEKEY) {
    
    // SE LA PRIMA SIGLA E` VUOTA, LA RIMPIAZZA CON IL TESTIMONE DI DEFAULT
    // (PER LE MACRO DI SEMPLIFICAZIONE)
    if (testo->testo1->lexema == NULL) {
      
      testo->testo1->lexema = parametri->testimone;
      
      // SE LA SIGLA DEL TESTIMONE DI DEFAULT NON E' STATA IMPOSTATA
      // EMETTE UN MESSAGGIO DI ERRORE
      if (strlen (parametri->testimone) == 0) {
	printf
	  ("La sigla del testimone di default non e` stata impostata (utilizzare la macro NomeTestimone)\n");
      }
    }
    
    // VERIFICA CHE LA VARIANTE VADA STAMPATA
    testimone = opzioneTestEx();
    if (testimone == NULL || findTestRif(testo->testo1, testimone)) {
      
      // SE SI TRATTA DI UNA TRASPOSIZIONE "DUPLEX", STAMPA "POST"
      if (duplex) {
	     //printNota("post ", parametri->outFile);
      }
      // SE LA NOTA NON E` INNESTATA OPPURE SE IL TESTIMONE E` IL PRIMO ED E` *
      if (!nested || (numero == 1 && (strcmp(testo->testo1->lexema, "*") == 0) )) {
	*nTest = printCorpoVariante(testo->testo2->testo1, // lezione
				    testo->testo2->testo2, // note
				    testo->testo1, // sigle
				    parametri,
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

// VISITA UNA LISTA DI SIGLE DI TESTIMONI
short visitSigle(struct testo * sigle, bool noNota, FILE * outFile) {

  short nTest = 0;

  if (sigle) {
    if (strcmp(sigle->lexema, "*") == 0) {
      if (noNota) {
	     printNota(":", outFile);
      }
    } else {
      printSigla(sigle->lexema, outFile);
      nTest = 1 + visitSigle(sigle->testo2, noNota, outFile);
    }
  }
  return nTest;
}

// VISITA I POSTSCRIPT DI UNA VARIANTE
void visitPostscript(struct testo * sigle, struct testo * note, PARAMETRI * parametri, bool postscript) {
  if (note) {
    if (note->testo1->key == POSTSCRIPTKEY) postscript = true;
    if (postscript) {
      printPostscript(sigle, note->testo1, parametri);
    }
    visitPostscript(sigle, note->testo2, parametri, postscript);
  }
}

// VISITA DI UNA MACRO GENERICA
void visitKey(short          key,
			  long           ln,
			  struct testo * testo,
			  PARAMETRI    * parametri,
			  struct testo * sigle) {
  // SALVATAGGIO DELL'AMBIENTE A LISTA ATTUALE
  short saveListEnv;
  bool saveItemOpen;

  // GESTIONE DELLA PARTE INIZIALE DELLA MACRO
  printKey(true, key, ln, testo, parametri, sigle);
  
  // VISITA INTERNA DELLA MACRO
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
  case HTMLCUTKEY:
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
	printManus(testo, parametri, sigle);
	break;
  case FOLIUMKEY:
    // CONTROLLO ESTRAZIONE TESTIMONE
    if( testo->lexema ) {
      if (opzioneTestEx() == NULL ||
  		strcmp(opzioneTestEx(), testo->lexema) == 0) {
        printFolium(testo->lexema, testo->testo2->lexema, parametri, sigle);
      }
    }
    break;
  case CITKEY:
    printCit(testo, parametri, sigle);
    break;
  case DATEKEY:
    // visitTesto(testo->testo1, parametri, sigle);
    printDate(testo, parametri, sigle);
    break;
  case TABLEKEY:
    visitTable(key, testo, parametri, sigle);
    break;
  case LONGTABLEKEY:
    visitLongTable(key, testo, parametri, sigle);
    break;
  case CASES:
    visitCases(key, testo, parametri, sigle);
    break;
  case FRACKEY:
    printFrac(testo, parametri, sigle);
    break;
  case TITLEWORKKEY:
    printTitleWork(testo, parametri, sigle);
    break;
  case SUPSUBKEY:
    printSupSub(testo, parametri, sigle);
    break;
  case LBKEY:
    visitTesto(testo->testo2, parametri, sigle);
    if (testo->lexema != NULL && parametri->label != NULL) {
      // CONTROLLO CORRISPONDENZA TRA LE ETICHETTE
      if (strcmp(testo->lexema, parametri->label) == 0) {
    	parametri->label = NULL;
      }
    }
    break;
  case MCITEKEY:
    visitTesto(testo->testo2, parametri, sigle);
    break;
  case DESCRIPTION:
  case ITEMIZE:
  case ENUMERATE:
	saveListEnv = parametri->listEnv;
	saveItemOpen = parametri->itemOpen;
	parametri->listEnv = key;
	parametri->itemOpen = false;
	visitTesto(testo->testo2, parametri, sigle);
	parametri->listEnv = saveListEnv;
	parametri->itemOpen = saveItemOpen;
	break;
  case ITEMKEY:
	parametri->itemOpen = true;
	break;
  case TEXTGREEK:
	printTextGreek(testo->testo2, parametri, sigle);
	break;
  case VSPACEKEY:
  case QUOTE:
  case ENUNCIATIO:
  case PROTASIS:
  case TABULAR:
  case TITMARG:
  case GENERICENV:
    visitTesto(testo->testo2, parametri, sigle);
    break;
  case UNITKEY:
    break;
  case AUTHORKEY:
	printAuthors(testo, parametri, sigle);
	break;
  case FRONTESPIZIOKEY:
	printFrontespizio(testo, parametri, sigle);
	break;
  case ALIAMANUSKEY:
	printAliaManus(testo, parametri, sigle);
	break;
  default:
    visitTesto(testo, parametri, sigle);
    break;
  }

  // GESTIONE DELLA PARTE FINALE DELLA MACRO
  printKey(false, key, ln, testo, parametri, sigle);
}

// STAMPA LA CITAZIONE DELL'OPERA
void stampaCitazioneOpera(PARAMETRI * parametri,
			  const char * libro,
			  const char * prop) {
  
  short i;
  char rif[1000];
  
  // VA BENE COSI'?
  if (libro) {
    if (prop) {
      sprintf(rif, "%s.%s", libro, prop);
    } else {
      sprintf(rif, "%s", libro);
    }
  }
  
  // NUMERO DELLE CONFIGURAZIONI
  for (i = 0; i < getNConf(); i++) {
    
    // PRENDO LA CONFIGURAZIONE CHE INTERESSA
    Configurazione * conf = getConfigurazione(i);
    
    // CONTATORE DELLA RIGA
    short nRiga = 0;
    
    // APERTURA DEL FILE DELLE CITAZIONI DELL'OPERA
    FILE * txtOpeFile = fopen(conf->getTxtOpeFileName(), "r");
    
    // SE IL FILE DELLE CONFIGURAZIONI ESISTE
    if (txtOpeFile) {
      
      // FINCHE' DURA IL FILE DELLE CONFIGURAZIONI
      while (!feof(txtOpeFile)) {
	
		char riga[10000];
	
		// MEMORIZZAZIONE DELLA RIGA
		char * r = riga;
		*r = 0;
		r = fgets(riga, sizeof(riga), txtOpeFile);
	
		// INCREMENTO DEL CONTATORE DELLE RIGHE
		nRiga++;
	
		// CARATTERE INIZIALE
		if (strlen(riga) > 0 && *riga != '#') {
	  
		  // MEMORIZZAZIONE DELLA RIGA FINO A  \t \n \r
		  char * tok = strtok(riga, "\t\n\r");
	  
		  // SE ESISTE
		  if (tok) {
	    
			if (strcmp(tok, rif) == 0) {
	      
			  // MEMORIZZO LA PARTE SUCCESSIVA DELLA RIGA FINO A \t \n \r
			  tok = strtok(NULL, "\t\n\r");
	      
			  // SE ESISTE
			  if (tok) {
		
				// STAMPO IL TESTO DELLA CITAZIONE DELL'OPERA
				fprintf(parametri->outFile, " %s", tok);
			  }
			}
		  }
		}
      }
      
      // CHIUSURA DEL FILE HTML DELL'OPERA
      fclose(txtOpeFile);
      
      // IL FILE DELL'OPERA NON ESISTE
    } else {
      fprintf(stderr, "\nERRORE: impossibile leggere il file %s\n\n", conf->getTxtOpeFileName());
    }
  }
}

// VISITA DI UN TESTO
void visitTesto(
  struct testo * testo,
  PARAMETRI    * parametri,
  struct testo * sigle
  ){

  bool math;
  bool greco;

  while (testo != NULL) { // SE IL TESTO NON E` NULL
    
    // GESTIONE DEI TAG
	// fprintf(parametri->outFile, "*** %ld\n", testo->key);
    switch (testo->key) {
    default:
      visitKey(testo->key, testo->ln, testo->testo1, parametri, sigle);
      break;
  case TEXTCIRCLEDKEY:
      printLexema(testo->key, testo->testo1->lexema, parametri);
      break;
	case BEGINMATHKEY:
    case BEGINEQNKEY:
      math = parametri->math;
      parametri->math = true;
      visitKey(testo->key, testo->ln, testo->testo1, parametri, sigle);
      parametri->math = math;
      break;
    case GREEKKEY:
      greco = parametri->greco;
      parametri->greco = true;
      visitKey(testo->key, testo->ln, testo->testo1, parametri, sigle);
      parametri->greco = greco;
      break;
    case CLASSKEY:
      visitKey(testo->key, testo->ln, testo->testo1, parametri, sigle);
      printLexema(testo->key, testo->lexema, parametri);
      break;
    case EXTERNALDOCUMENT:
    case SYMBOL:
    case MACRO:
    case NOTAMACRO:
    case NOKEY:
    case GRAVE:
    case ACUTE:
    case UML:
    case TILDE:
    case CIRC:
    case PARSKIP:
    case PARINDENT:
    case PRELOADKEY:
      printLexema(testo->key, testo->lexema, parametri);
      visitKey(testo->key, testo->ln, testo->testo1, parametri, sigle);
      break;
    case UNITREFKEY:
	   printUnitRef(testo->testo1->testo1->lexema, testo->testo1->testo2->lexema, parametri);
      break;
    case FIGSKIPKEY:
    case FIGURAKEY:
    case FMARGKEY:
    case FORMULAKEY:
    case TAVOLAKEY:
      parametri->nFig++;
      printFig(testo->key, testo->testo1, parametri);
      break;
    case PROPKEY:
      parametri->nProp++;
      visitKey(testo->key, testo->ln, testo->testo1, parametri, sigle);
      break;
    case UNITKEY:
      parametri->nUnit++;
      visitKey(testo->key, testo->ln, testo->testo1, parametri, sigle);
	  if (testo->testo1->lexema != NULL) {
		printLexema(testo->key, testo->testo1->lexema, parametri);
	  }
      break;
    case SECTIONKEY:
      parametri->nSec++;
      parametri->nSubsec = 0;
      parametri->nSSubsec = 0;
      visitKey(testo->key, testo->ln, testo->testo1, parametri, sigle);
      break;
    case SUBSECTIONKEY:
      parametri->nSubsec++;
      parametri->nSSubsec = 0;
      visitKey(testo->key, testo->ln, testo->testo1, parametri, sigle);
      break;
    case SSSECTIONKEY:
      parametri->nSSubsec++;
      visitKey(testo->key, testo->ln, testo->testo1, parametri, sigle);
      break;
    case ADNOTKEY:
      parametri->nAdnot++;
      if (!parametri->nota) {
		printAdnotatio(testo->testo1, parametri, sigle);
      }
      break;
    case FOOTNOTEKEY:
      parametri->nFootnote++;
      if (!parametri->nota) {
		printFootnote(testo->testo1, parametri, sigle);
      }
      break;
    case VBKEY:
      //if (opzioneBanale()) {
      //	testo->key = VVKEY;
      //}
    case SCHOLKEY:
    case VVKEY:
    case TVKEY:
    case NMARGKEY:
    case NOTEKEY:
      if ((testo->key != VBKEY || opzioneBanale()) && !parametri->nota && !parametri->banale) {
		parametri->nVV++;
      }
      visitVV(testo->key, testo->testo1, parametri, sigle);
      break;
    case NOMTESTKEY:
      parametri->testimone = testo->lexema;
      break;
    case MITINTESTKEY:
      parametri->mitIntest = testo->testo1;
      break;
    case DESINTESTKEY:
      parametri->desIntest = testo->testo1;
      break;
    case LMIINTESTKEY:
      parametri->lmiIntest = testo->testo1;
      break;
    case COMINTESTKEY:
      parametri->comIntest = testo->testo1;
      break;
    case DATINTESTKEY:
      parametri->datIntest = testo->testo1;
      break;
    case COMMKEY:
      parametri->nComm++;
      if (!parametri->nota) {
      printCommento(testo->testo1, parametri, sigle);
      }
      break;
    }
    
    // GESTIONE DELLA SECONDA PARTE DI TESTO
    testo = testo->testo2;
  }
}
