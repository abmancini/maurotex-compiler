#define VERSIONSTRING "versione 0.1b3 del 10 maggio 2019"

/*
  
file "m2tei.c"
PROGRAMMA PRINCIPALE

m2tei: traduttore da M-TeX a TEI XML basato mvisit

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
	
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
	
#include "mparse.h"
#include "mparsey.h"
#include "mvisit.h"
	
#define XMLEST  ".xml"
	
// STRUTTURA DATI PARAMETRI
	class PARAMETRIm2xml: public PARAMETRI {
	
	public:
	short nOM;
	bool  justify;
	bool  greek;
	char  greek_buffer;
	
	PARAMETRIm2xml(): PARAMETRI() {
		
		this->nOM     = 0;
		this->justify = true;
		this->greek   = false;
	}
};

// VARIABILI GLOBALI
//static char * latexFileName;
static char * xmlFileName;
static short  nSpaces;
static const char * content;
static char * figExtension;

// FUNZIONI PER OPZIONI GESTITE ESTERNAMENTE
bool verboseWarning() {
	return false;
}

// FUNZIONI SPECIFICHE DEL TRADUTTORE LATEX


// TRASFORMA UNA VARIANTE BANALE IN UNA SERIA SE RICHIESTO
bool opzioneBanale() {
	return false;
}

// RITORNA IL TESTIMONE PER VEDERENE L'ESTRATTO SE RICHIESTO
char * opzioneTestEx() {
	return NULL;
}

// FUNZIONE DI STAMPA DELLE MANI
void printManus(struct testo *testo, PARAMETRI* param, struct testo *sigle) {}

// FUNZIONE CHE STAMPA IL PROLOGO DI UNA TABELLA
void printTableStart(int key, struct testo * testo, PARAMETRI * parametri, struct testo * sigle) {
  visitTesto(testo, parametri, sigle);
}

// FUNZIONE CHE STAMPA L'EPILOGO DI UNA TABELLA
void printTableFinish(int key, struct testo * testo, PARAMETRI * parametri, struct testo * sigle) {
  visitTesto(testo, parametri, sigle);
}

// FUNZIONE CHE STAMPA LO HEADER DI RIGA DI UNA TABELLA
void printRowHeader(short key, PARAMETRI * parametri) {
  if (key == HLINEKEY) {
    visitKey(key, 0, NULL, parametri, NULL);
  } else {
    fprintf(parametri->outFile, "<tr>\n");
  }
}

// FUNZIONE CHE STAMPA IL FOOTER DI RIGA DI UNA TABELLA
void printRowFooter(short key, PARAMETRI * parametri) {
  if (key != HLINEKEY) {
    fprintf(parametri->outFile, "</tr>\n");
  }
}

// FUNZIONE CHE STAMPA LO HEADER DI UNA TABELLA
void printTableHeader(int key, const char * colonne, PARAMETRI * parametri) {
  fprintf(parametri->outFile, "\n<tabula>\n");
}

// FUNZIONE CHE STAMPA IL FOOTER DI UNA TABELLA
void printTableFooter(int key, PARAMETRI * parametri) {
  fprintf(parametri->outFile, "</tabula>\n");  
}

// FUNZIONE CHE STAMPA UN CAMPO DI UNA TABELLA
void printCampo(short          nRow,
		short          nCol,
		const char   * colonne,
		struct testo * campo,
		PARAMETRI    * parametri,
		struct testo * sigle) {
	
	if (campo != NULL) {
		
		fprintf(parametri->outFile, "<td");
		
		// GESTIONE INIZIALE MULTIROW E MULTICOL
		switch (campo->key) {
			case MULTICOLKEY:
				fprintf(parametri->outFile, " colspan=\"%d\"", atoi(campo->lexema));
				break;
			case MULTIROWKEY:      
				fprintf(parametri->outFile, " rowspan=\"%d\"", atoi(campo->lexema));
				break;
		}
		fprintf(parametri->outFile, ">");
		
		switch (campo->key) {
			case CLINEKEY:
			case LEFTGRATABKEY:
			case RIGHTGRATABKEY:
			case LEFTANGTABKEY:
			case RIGHTANGTABKEY:
				visitKey(campo->key, 0, NULL, parametri, sigle);
				break;
			default:
				visitTesto(campo, parametri, sigle);
				break;
		}
		fprintf(parametri->outFile, "</td>\n");
	}

}

// ANALISI TESTO DEI CASES
void printCasi(struct testo * testo, PARAMETRI * parametri, struct testo * sigle) {
	if (testo != NULL) {
		fprintf(parametri->outFile, "\n<case>");
		visitTesto(testo->testo1, parametri, sigle);
		fprintf(parametri->outFile, "</case>");

		printCasi(testo->testo2, parametri, sigle);
	}
}

// FUNZIONE DI STAMPA DEI CASES
void printCases(short key, struct testo * testo, PARAMETRI * parametri, struct testo * sigle) {
  
	fprintf(parametri->outFile, "<cases ");
  
	if (strcmp(testo->lexema, "\\Rcases")  == 0) {
		fprintf(parametri->outFile, " type = \"R\" frame = \"bracket\"");
	}
  
	if (strcmp(testo->lexema, "\\RLcases") == 0) {
		fprintf(parametri->outFile, " type = \"RL\" frame = \"bracket\"");
	}
  
	if (strcmp(testo->lexema, "\\Lcases") == 0) {
		fprintf(parametri->outFile, " type = \"L\" frame = \"bracket\"");
	}
  
	if (strcmp(testo->lexema, "\\Lbracecases") == 0) {
		fprintf(parametri->outFile, " type = \"L\" frame = \"brace\"");
	}
  
	if (strcmp(testo->lexema, "\\Voidcases") == 0) {
		fprintf(parametri->outFile, " type = \"VOID\" frame = \"void\"");
	}
  
	if (strcmp(testo->lexema, "\\Rbracecases") == 0) {
		fprintf(parametri->outFile, " type = \"R\" frame = \"brace\"");
	}

	if (strcmp(testo->lexema, "\\RLbracecases") == 0) {
		fprintf(parametri->outFile, " type = \"RL\" frame = \"brace\"");
	}
  
	fprintf(parametri->outFile, ">");
  
	printCasi(testo->testo2, parametri, sigle);
  
	fprintf(parametri->outFile, "\n</cases>\n");
}

// STAMPA I TITOLI DELLE OPERE
void printTitleWork(struct testo *testo, PARAMETRI * parametri, struct testo *sigle){
  fprintf(parametri->outFile, "<title>");
  visitTesto(testo->testo1, parametri, sigle);
  fprintf(parametri->outFile, "</title>");
  fprintf(parametri->outFile, "<subtitle>");
  if (testo->testo2 != NULL) {
	visitTesto(testo->testo2, parametri, sigle);
  }
  fprintf(parametri->outFile, "</subtitle>");
}

// FUNZIONE DI STAMPA DI ALIAMANUS
void printAliaManus(struct testo * testo,
					PARAMETRI * parametri,
					struct testo * sigle) {
  visitTesto(testo, parametri, sigle);
}

// FUNZIONE DI STAMPA DELLE FRAZIONI
void printFrac(struct testo *testo, PARAMETRI * parametri, struct testo *sigle){
  
/*
	fprintf(parametri->outFile, "<frac>");
  
	fprintf(parametri->outFile, "<num>");
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "</num>");
  
	fprintf(parametri->outFile, "<denom>");
	visitTesto(testo->testo2, parametri, sigle);
	fprintf(parametri->outFile, "</denom>");

	fprintf(parametri->outFile, "</frac>");
*/
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "/");
	visitTesto(testo->testo2, parametri, sigle);
}

// FUNZIONE DI STAMPA DELL'ESPONENTE E DEL DEPONENTE
void printSupSub(struct testo *testo, PARAMETRI * parametri, struct testo *sigle){
  
	fprintf(parametri->outFile, "<expdep>");
  
	fprintf(parametri->outFile, "<exp>");
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "</exp>");
  
	fprintf(parametri->outFile, "<dep>");
	visitTesto(testo->testo2, parametri, sigle);
	fprintf(parametri->outFile, "</dep>");
  
	fprintf(parametri->outFile, "</expdep>");
}

// STAMPA GLI ACCENTI
void printAccento(const char * lexema, const char * accento, FILE * outFile) {

	int i; // POSIZIONE DEL CARATTERE ACCENTATO

	// IGNORA L'EVENTUALE PARENTESI O BACKSLASH
	if (lexema[2] == '{' || lexema[2] == '\\') i = 3; else i = 2;

	// STAMPA IL CARATTERE COL SUO ACCENTO
	switch (lexema[i]) {
		case 'a':
			fprintf(outFile, "à");
			break;
		case 'e':
			fprintf(outFile, "è");
			break;
		case 'i':
			fprintf(outFile, "ì");
			break;
		case 'o':
			fprintf(outFile, "ò");
			break;
		case 'u':
			fprintf(outFile, "ù");
			break;
		case 'A':
			fprintf(outFile, "À");
			break;
		case 'E':
			fprintf(outFile, "È");
			break;
		case 'I':
			fprintf(outFile, "Ì");
			break;
		case 'O':
			fprintf(outFile, "Ò");
			break;
		case 'U':
			fprintf(outFile, "Ù");
			break;
		default:
			fprintf(outFile, "&%c%s;", lexema[i], accento);
			break;
	}
}

// STAMPA LA SIGLA DI UN TESTIMONE
void printSigla(const char * sigla, FILE * outFile) {
}

// STAMPA UNA STRINGA DI NOTA DELL'EDITORE
void printNota(const char * nota, FILE * outFile) {
}

// STAMPA IL CORPO DI UNA VARIANTE
short printCorpoVariante(struct testo * lezione,
						 struct testo * note,
						 struct testo * sigle,
						 PARAMETRI    * param,
						 bool           duplex,
						 bool           multiplex,
						 bool           nested,
						 int            nVar) {
  
	short nTest = 0;
	return nTest;
}

// STAMPA I POSTSCRIPT DI UNA VARIANTE
void printPostscript(struct testo * sigle, struct testo * nota, PARAMETRI * param) {
}


// FUNZIONI GENERICHE DEFINITE IN MVISIT

void printFolium(const char * testimone, const char * pagina, PARAMETRI* parametri, struct testo *sigle) {
  fprintf(parametri->outFile, "<folium page=\"%s\">", pagina);
  fprintf(parametri->outFile, "<ids><id id=\"%s\"/></ids></folium>", testimone);
}

// FUNZIONE DI STAMPA DELLE DATE
void printDate(struct testo *testo, PARAMETRI* param, struct testo *sigle) {
  
	// RECUPERA I PARAMETRI
	//PARAMETRIm2xml * parametri = (PARAMETRIm2xml *) param;
  
	//fprintf(parametri->outFile, "\n<date>");
	//fprintf(parametri->outFile, "<reading>");
	visitTesto(testo->testo1, param, sigle);
/*
	fprintf(parametri->outFile, "</reading>");
	if (testo->testo2) {
		if (testo->testo2->testo1) {
			fprintf(parametri->outFile, "<standard>");
			visitTesto(testo->testo2->testo1, param, sigle);
			fprintf(parametri->outFile, "</standard>");
		}
		if (testo->testo2->testo2) {
			fprintf(parametri->outFile, "<comment>");
			visitTesto(testo->testo2->testo2, param, sigle);
			fprintf(parametri->outFile, "</comment>");
		}
	}
	fprintf(parametri->outFile, "</date>\n");
*/
}

// FUNZIONE DI STAMPA DELLE CITAZIONI
void printCit(struct testo *testo, PARAMETRI* param, struct testo *sigle){
  
  /*
	// RECUPERA I PARAMETRI
	PARAMETRIm2xml * parametri = (PARAMETRIm2xml *) param;
  
	fprintf(parametri->outFile, "<cit type=\"%s\">", (testo->lexema == NULL) ? "normall" : testo->lexema);
	fprintf(parametri->outFile, "<reading>");
  */
	visitTesto(testo->testo1, param, sigle);
	/*
	fprintf(parametri->outFile, "</reading>");
	if (testo->testo2) {
		if (testo->testo2->testo1) {
			fprintf(parametri->outFile, "<standard>");
			visitTesto(testo->testo2->testo1, param, sigle);
			fprintf(parametri->outFile, "</standard>");
		}
		if (testo->testo2->testo2) {
			fprintf(parametri->outFile, "<comment>");
			visitTesto(testo->testo2->testo2, param, sigle);
			fprintf(parametri->outFile, "</comment>");
		}
	}
	fprintf(parametri->outFile, "</cit>");
	*/

}

void printUTF8(short g1, short g2, PARAMETRIm2xml * parametri) {
  fprintf(parametri->outFile, "%c%c", g1, g2);
}

void printGreekChar(char c, PARAMETRIm2xml * parametri) {
  
  // a -> alpha
  // b -> beta
  // c -> csi
  // d -> delta
  // e -> epsilon
  // f -> phi
  // g -> gamma
  // h -> eta
  // i -> iota
  // j -> 
  // k -> kappa
  // l -> lambda
  // m -> mu
  // n -> nu
  // o -> omicron
  // p -> pi
  // q -> theta
  // r -> rho
  // s -> sigma
  // t -> tau
  // u -> 
  // v -> psi
  // w -> omega
  // x -> chi
  // y -> ypsilon
  // z -> zeta
  
  switch (c) {
  case 'A':
	printUTF8(206, 145, parametri);
	break;
  case 'B':
	printUTF8(206, 146, parametri);
	break;
  case 'G':
	printUTF8(206, 147, parametri);
	break;
  case 'D':
	printUTF8(206, 148, parametri);
	break;
  case 'E':
	printUTF8(206, 149, parametri);
	break;
  case 'Z':
	printUTF8(206, 150, parametri);
	break;
  case 'H':
	printUTF8(206, 151, parametri);
	break;
  case 'Q':
	printUTF8(206, 152, parametri);
	break;
  case 'I':
	printUTF8(206, 153, parametri);
	break;
  case 'K':
	printUTF8(206, 154, parametri);
	break;
  case 'L':
	printUTF8(206, 155, parametri);
	break;
  case 'M':
	printUTF8(206, 156, parametri);
	break;
  case 'N':
	printUTF8(206, 157, parametri);
	break;
  case 'C':
	printUTF8(206, 158, parametri);
	break;
  case 'O':
	printUTF8(206, 159, parametri);
	break;
  case 'P':
	printUTF8(206, 160, parametri);
	break;
  case 'R':
	printUTF8(206, 161, parametri);
	break;
  case 'S':
	printUTF8(206, 163, parametri);
	break;
  case 'T':
	printUTF8(206, 164, parametri);
	break;
  case 'Y':
	printUTF8(206, 165, parametri);
	break;
  case 'F':
	printUTF8(206, 166, parametri);
	break;
  case 'X':
	printUTF8(206, 167, parametri);
	break;
  case 'V':
	printUTF8(206, 168, parametri);
	break;
  case 'W':
	printUTF8(206, 169, parametri);
	break;
  case 'a':
	printUTF8(206, 177, parametri);
	break;
  case 'b':
	printUTF8(206, 178, parametri);
	break;
  case 'g':
	printUTF8(206, 179, parametri);
	break;
  case 'd':
	printUTF8(206, 180, parametri);
	break;
  case 'e':
	printUTF8(206, 181, parametri);
	break;
  case 'z':
	printUTF8(206, 182, parametri);
	break;
  case 'h':
	printUTF8(206, 183, parametri);
	break;
  case 'q':
	printUTF8(206, 184, parametri);
	break;
  case 'i':
	printUTF8(206, 185, parametri);
	break;
  case 'k':
	printUTF8(206, 186, parametri);
	break;
  case 'l':
	printUTF8(206, 187, parametri);
	break;
  case 'm':
	printUTF8(206, 188, parametri);
	break;
  case 'n':
	printUTF8(206, 189, parametri);
	break;
  case 'c':
	printUTF8(206, 190, parametri);
	break;
  case 'o':
	printUTF8(206, 191, parametri);
	break;
  case 'p':
	printUTF8(207, 128, parametri);
	break;
  case 'r':
	printUTF8(207, 129, parametri);
	break;
  case 's':
	printUTF8(207, 130, parametri);
	break;
  case 't':
	printUTF8(207, 132, parametri);
	break;
  case 'y':
	printUTF8(207, 133, parametri);
	break;
  case 'f':
	printUTF8(207, 134, parametri);
	break;
  case 'x':
	printUTF8(207, 135, parametri);
	break;
  case 'v':
	printUTF8(207, 136, parametri);
	break;
  case 'w':
	printUTF8(207, 137, parametri);
	break;
  default:
	fprintf(parametri->outFile, "%c", c);
	break;
  }
}

void flushGreekBuffer(PARAMETRIm2xml * parametri) {
  if (parametri->greek_buffer != 0) {
	printGreekChar(parametri->greek_buffer, parametri);
	parametri->greek_buffer = 0;
  }
}

// FUNZIONE DI STAMPA DI UN CARATTERE GRECO
void printGreco(unsigned char c, PARAMETRIm2xml * parametri) {
  
  if (parametri->greek_buffer == 0) {
	
	// NESSUN CARATTERE IN SOSPESO
	switch (c) {
	case 'A':
	case 'E':
	case 'H':
	case 'I':
	case 'O':
	case 'Y':
	case 'W':
	case 'a':
	case 'e':
	case 'h':
	case 'i':
	case 'o':
	case 'y':
	case 'w':
	  
	  // CARATTERE DA SOSPENDERE
	  parametri->greek_buffer = c;
	  break;
	  
	default:
	  // CARATTERE DA STAMPARE DIRETTAMENTE
	  printGreekChar(c, parametri);
	  break;
	}
	
  } else {
	
	// C'E` UN CARATTERE IN SOSPESO
	switch (c) {
	  
	case '\'':
	  
	  // ACCENTO
	  switch (parametri->greek_buffer) {
	  case 'A':
		printUTF8(206, 134, parametri);
		break;
	  case 'E':
		printUTF8(206, 136, parametri);
		break;
	  case 'H':
		printUTF8(206, 137, parametri);
		break;
	  case 'I':
		printUTF8(206, 138, parametri);
		break;
	  case 'O':
		printUTF8(206, 140, parametri);
		break;
	  case 'Y':
		printUTF8(206, 142, parametri);
		break;
	  case 'W':
		printUTF8(206, 143, parametri);
		break;
	  case 'a':
		printUTF8(206, 172, parametri);
		break;
	  case 'e':
		printUTF8(206, 173, parametri);
		break;
	  case 'h':
		printUTF8(206, 174, parametri);
		break;
	  case 'i':
		printUTF8(206, 175, parametri);
		break;
	  case 'o':
		printUTF8(207, 140, parametri);
		break;
	  case 'y':
		printUTF8(207, 141, parametri);
		break;
	  case 'w':
		printUTF8(207, 142, parametri);
		break;
	  }
	  
	  // CARATTERE IN SOSPESO TRATTATO
	  parametri->greek_buffer = 0;
	  
	  break;
	  
	case '=':
	  
	  // DIERESI
	  switch (parametri->greek_buffer) {
	  case 'I':
		printUTF8(206, 170, parametri);
		break;
	  case 'Y':
		printUTF8(206, 171, parametri);
		break;
	  case 'i':
		printUTF8(207, 138, parametri);
		break;
	  case 'y':
		printUTF8(207, 139, parametri);
		break;
	  }
	  
	  // CARATTERE IN SOSPESO TRATTATO
	  parametri->greek_buffer = 0;
	  
	  break;
	  
	case ')':
	  
	  // SPIRITO?
	  switch (parametri->greek_buffer) {
	  case 'A':
		printUTF8(206, 144, parametri);
		break;
	  case 'a':
		printUTF8(206, 177, parametri);
		break;
	  }
	  
	  // CARATTERE IN SOSPESO TRATTATO
	  parametri->greek_buffer = 0;
	  
	  break;
	  
	default:
	  
	  // NON C'E` COMPOSIZIONE
	  flushGreekBuffer(parametri);
	  //printGreekChar(c, parametri);
	  parametri->greek_buffer = c;
	}
  }
}

// FUNZIONE DI STAMPA DI UNA MACRO GENERICA
void printKey(bool           inizio,
			  short          key,
			  long           ln,
			  struct testo * testo,
			  PARAMETRI    * param,
			  struct testo * sigle) {
  
  // RECUPERA I PARAMETRI
  PARAMETRIm2xml * parametri = (PARAMETRIm2xml *) param;
  
  if (parametri->label == NULL || parametri->nota) {
    
	// IDENTIFICAZIONE DELLA MACRO
	switch (key) {
	default:
	  break;
	case HLINEKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<linea />");
	  }
	  break;
	case CLINEKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<regula />");
	  }
	  break;
	case LEFTGRATABKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<leftgratab />");
	  }
	  break;
	case RIGHTGRATABKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<rightgratab />");
	  }
	  break;
	case LEFTANGTABKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<leftangtab />");
	  }
	  break;
	case RIGHTANGTABKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "rightangtab /");
	  }
	  break;
	case CONGKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<cong>");
	  } else {
		fprintf(parametri->outFile, "</cong>");
	  }
	  break;
	case LBKEY:
	  /*
		if (inizio) {
		fprintf(parametri->outFile, "<label");
		fprintf(parametri->outFile, " label=\"%s\">", testo->lexema);
		} else {
		fprintf(parametri->outFile, "</label>");
		}
	  */
	  break;
	case COMMENTKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "<commenti />");
	  }
	  break;
	case ANNOTAZKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "<annotazioni />");
	  }
	  break;
	case PROPKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<prop>");
		visitTesto(testo, parametri, sigle);
	  } else {
		fprintf(parametri->outFile, "</prop>");
	  }
	  break;
	case OPERAKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<opera>");
		visitTesto(testo, parametri, sigle);
	  } else {
		fprintf(parametri->outFile, "</opera>");
	  }
	  break;
	case ARGKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<arg>");
		visitTesto(testo, parametri, sigle);
	  } else {
		fprintf(parametri->outFile, "</arg>");
	  }
	  break;
	case LIBROKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<book>");
		visitTesto(testo, parametri, sigle);
	  } else {
		fprintf(parametri->outFile, "</book>");
	  }
	  break;
      
	case BEGINNMKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<margnote>");
	  }
	  break;
      
	case ENDNMKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "</margnote>");
	  }
	  break;
      
	case MARKKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<markup>");
		fprintf(parametri->outFile, "<des>");
		visitTesto(testo->testo1, parametri, sigle);
		fprintf(parametri->outFile, "</des>");
		fprintf(parametri->outFile, "<reading><content>");
		visitTesto(testo->testo2, parametri, sigle);
		fprintf(parametri->outFile, "</content></reading>");
	  } else {
		fprintf(parametri->outFile, "</markup>");
	  }
	  break;
	case COMMKEY:
	  if (inizio) {
	    //fprintf(parametri->outFile, "<comment content=\"");
	    //visitTesto(testo, parametri, sigle);
	    //fprintf(parametri->outFile, "\"/>");
	  }
	  break;
	case GREEKKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<foreign xml:lang=\"greek\">");
		parametri->greek = true;
		parametri->greek_buffer = 0;
	  } else {
		flushGreekBuffer(parametri);
		fprintf(parametri->outFile, "</foreign>");
		parametri->greek = false;
	  }
	  break;
	case ABBRKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "<abbr>");
	  } else {
		//fprintf(parametri->outFile, "</abbr>");
	  }
	  break;
	case SETLENGTH:
	  if (inizio) {
		//fprintf(parametri->outFile, "\\setlength");
		//fprintf(parametri->outFile, "<setlength />\n");
	  }
	  break;
	case MAKETITLEKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "\\maketitle\n\n");
		//fprintf(parametri->outFile, "<maketitle />\n");
	  }
	  break;
	case PLKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "<pl>");
	  } else {
		//fprintf(parametri->outFile, "</pl>");
	  }
	  break;
	case CLASSKEY:
	  if (inizio) {
	    fprintf(parametri->outFile, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	  }
	  break;
	case DOCENVKEY:
	  if (inizio) {
	    visitTesto(testo->testo1, parametri, sigle);
	    fprintf(parametri->outFile, "<TEI.2>\n");
	    fprintf(parametri->outFile, "<teiHeader>\n");
	    fprintf(parametri->outFile, "<fileDesc>\n");
	    fprintf(parametri->outFile, "<sourceDesc>\n");
	    fprintf(parametri->outFile, "<bibl>\n");
	    fprintf(parametri->outFile, "<author ref=\"http://viaf.org/viaf/34537888\">Maurolycus, Franciscus<date>1494-1575</date></author>\n");
	    fprintf(parametri->outFile, "<imprint>\n");
	    fprintf(parametri->outFile, "<pubPlace>Pisa</pubPlace>\n");
	    fprintf(parametri->outFile, "<publisher>online</publisher>\n");
	    fprintf(parametri->outFile, "</imprint>\n");
	    fprintf(parametri->outFile, "<edition>critical edition</edition>\n");
	    fprintf(parametri->outFile, "</bibl>\n");
	    fprintf(parametri->outFile, "</sourceDesc>\n");
	    fprintf(parametri->outFile, "</fileDesc>\n");
	    fprintf(parametri->outFile, "</teiHeader>\n");
	    fprintf(parametri->outFile, "<content>");
	    visitTesto(testo->testo2, parametri, sigle);
	    fprintf(parametri->outFile, "</content>\n");
	    fprintf(parametri->outFile, "</TEI.2>\n");
	  }
	  break;
	case UNITKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<unit");
		if (testo->lexema != NULL) {
		  fprintf(parametri->outFile, " label=\"%s\"", testo->lexema);
		}
		fprintf(parametri->outFile, " />");
	  }
	  break;
	case CAPITOLOKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "\n<capitolo>");
	  } else {
		fprintf(parametri->outFile, "</capitolo>\n");
	  }
	  break;
	case TEOREMAKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "\n<teorema>");
	  } else {
		fprintf(parametri->outFile, "</teorema>\n");
	  }
	  break;
	case SECTIONKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "\n\n<section>");
	  } else {
		fprintf(parametri->outFile, "</section>\n\n");
	  }
	  break;
	case SUBSECTIONKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "\n\n<subsection>");
	  } else {
		fprintf(parametri->outFile, "</subsection>\n\n");
	  }
	  break;
	case SSSECTIONKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "\n\n<subsubsection>");
	  } else {
		fprintf(parametri->outFile, "</subsubsection>\n\n");
	  }
	  break;
	case ':':
	  if (inizio) {
		fprintf(parametri->outFile, "%c", ':');
	  }
	  break;
	case OMKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "omisit");
	  }
	  break;
	case DELKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "delevit");
	  }
	  break;
	case EXPKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<exp />");
	  }
	  break;
	case NIHILKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "<nihil />");
	  }
	  break;
	case OMLACKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<omlac />");
	  }
	  
	  break;
	case NLKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<nl />");
	  }
	  break;
	case LACMKEY:
	  if (inizio) {
		if (!parametri->nota) {
		  fprintf(parametri->outFile, "<lacm />");
		}
	  }
	  break;
	case LACSKEY:
	  if (inizio) {
		if (!parametri->nota) {
		  fprintf(parametri->outFile, "<lacs />");
		}
	  }
	  break;
	case LACCKEY:
	  if (inizio) {
		if (!parametri->nota) {
		  fprintf(parametri->outFile, "<lacc />");
		}
	  }
	  break;
	case BEGINEQNKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<eqn>");
	  } else {
		fprintf(parametri->outFile, "</eqn>");
	  }
	  break;
	case BEGINMATHKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<math><content>");
	  } else {
		fprintf(parametri->outFile, "</content></math>");
	  }
	  break;
	case SLANTED:
	  if (inizio) {
		fprintf(parametri->outFile, "<slanted>");
	  } else {
		fprintf(parametri->outFile, "</slanted>");
	  }
	  break;
	case EMPHASIS:
	  if (inizio) {
		fprintf(parametri->outFile, "<emph>");
	  } else {
		fprintf(parametri->outFile, "</emph>");
	  }
	  break;
	case TITKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "<tit>");
	  } else {
		//fprintf(parametri->outFile, "</tit>");
	  }
	  break;
	case ITALIC:
	  if (inizio) {
		fprintf(parametri->outFile, "<i>");
	  } else {
		fprintf(parametri->outFile, "</i>");
	  }
	  break;
	case STRIKEKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<strike>");
	  } else {
		fprintf(parametri->outFile, "</strike>");
	  }
	  break;
	case POWERKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<power>");
	  } else {
		fprintf(parametri->outFile, "</power>");
	  }
	  break;
	case INDEXKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<underline>");
	  } else {
		fprintf(parametri->outFile, "</underline>");
	  }
	  break;
	case SUPKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "<sup>");
	  } else {
		//fprintf(parametri->outFile, "</sup>");
	  }
	  break;
	case SUBKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<sub>");
	  } else {
		fprintf(parametri->outFile, "</sub>");
	  }
	  break;
	case TILDEKEY:
	  if (inizio) {
		//fprintf(parametri->outFile,"&nbsp;");
	  }
	  break;
	case DOTSKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<dots />");
	  }
	  break;
	case DSKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<db />");
	  }
	  break;
	case SSKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<sb />");
	  }
	  break;
	case NBSPACEKEY:
	  if (inizio) {
		fprintf(parametri->outFile," ");
	  }
	  break;
	case FLATKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<flat />");
	  }
	  break;
	case NATURALKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<natural />");
	  }
	  break;
	case CDOTKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<cdot />");
	  }
	case VERTKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<vert />");
	  }
	  break;
	case LBRACKETKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<lbracket />");
	  }
	  break;
	case RBRACKETKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<rbracket />");
	  }
	  break;
	case SKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<S />");
	  }
	  break;
	case SLASH:
	  if (inizio) {
		fprintf(parametri->outFile, "<slash />");
	  }
	  break;
	case GENERICENV:
	  if (inizio) {
		fprintf(parametri->outFile, "<%s>\n", testo->lexema);
	  } else {
		fprintf(parametri->outFile, "</%s>\n", testo->lexema);
	  }
	  break;	
	case ENUNCIATIO:
	  if (inizio) {
		fprintf(parametri->outFile, "<enunciatio><content>");
	  } else {
		fprintf(parametri->outFile, "</content></enunciatio>");
	  }
	  break;
	case PROTASIS:
	  if (inizio) {
		//fprintf(parametri->outFile, "<protasis>");
	  } else {
		//fprintf(parametri->outFile, "</protasis>\n");
	  }
	  break;
	case CENTERKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<center>");
	  } else {
		fprintf(parametri->outFile, "</center>\n");
	  }
	  break;
	case PARAGRAPH:
	  if (inizio) {
		fprintf(parametri->outFile, "\n");
	  }
	  break;
	case TITLEKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<title>");
	  } else {
		fprintf(parametri->outFile, "</title>");
	  }
	  break;
	case AUTHORKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<author>");
	  } else {
		fprintf(parametri->outFile, "</author>");
	  }
	  break;
	case ROMAN:
	  if (inizio) {
		fprintf(parametri->outFile, "<roman>");
	  } else {
		fprintf(parametri->outFile, "</roman>");
	  }
	  break;
	case BOLDFACE:
	  if (inizio) {
		fprintf(parametri->outFile, "<b>");
	  } else {
		fprintf(parametri->outFile, "</b>");
	  }
	  break;
	case TYPEFACE:
	  if (inizio) {
		fprintf(parametri->outFile, "<tt>");
	  } else {
		fprintf(parametri->outFile, "</tt>");
	  }
	  break;
	case SPACES:
	case NEWLINE:
	  if (inizio) {
		if (parametri->greek) {
		  flushGreekBuffer(parametri);
		}
		fprintf(parametri->outFile, " ");
		/*
		  if (nSpaces < 10) {
		  nSpaces++;
		  fprintf(parametri->outFile, " ");
		  } else {
		  nSpaces = 0;
		  fprintf(parametri->outFile, "\n");
		  }
		*/
	  }
	  break;
	case MAUROTEXKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<mauroteX />");
	  }
	  break;
	case MTEXKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<mtex />");
	  }
	  break;
	case PERKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<per />");
	  }
	  break;
	case CONKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<con />");
	  }
	  break;
	case TEXKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<tex />");
	  }
	  break;
	case LATEXKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<latex />");
	  }
	  break;
	case ADKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "---");
	  }
	  break;
	case VECTORKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "<vector>");
	  } else {
		//fprintf(parametri->outFile, "</vector>");
	  }
	  break;
	case CONTRKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "<contr>");
	  } else {
		//fprintf(parametri->outFile, "</contr>");
	  }
	  break;
	case CRUXKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "<crux>");
	  } else {
		//fprintf(parametri->outFile, "</crux>");
	  }
	  break;
	case EXPUKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "<expu>");
	  } else {
		//fprintf(parametri->outFile, "</expu>");
	  }
	  break;
	case INTEKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "<inte>\n");
	  } else {
		//fprintf(parametri->outFile, "</inte>\n");
	  }
	  break;
	case LEFTANG:
	  if (inizio) {
		fprintf(parametri->outFile, "<leftang>\n");
	  } else {
		fprintf(parametri->outFile, "</leftang>\n");
	  }
	  break;
	case CITMARGKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<citmarg>");
	  } else {
		fprintf(parametri->outFile, "</citmarg>");
	  }
	  break;
	case CITMARGSIGNKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<citmargsign>");
	  } else {
		fprintf(parametri->outFile, "</citmargsign>");
	  }
	  break;
	case UNDERLINE:
	  if (inizio) {
		fprintf(parametri->outFile, "<underline>");
	  } else {
		fprintf(parametri->outFile, "</underline>");
	  }
	  break;
	case LEFTSQUARE:
	  if (inizio) {
		fprintf(parametri->outFile, "[");
	  } else {
		fprintf(parametri->outFile, "]");
	  }
	  break;
	case LEFTPAR:
	  if (inizio) {
		fprintf(parametri->outFile, "(");
	  }
	  break;
	case RIGHTPAR:
	  if (inizio) {
		if (parametri->greek) {
		  printGreco(')', parametri);
		} else {
		  fprintf(parametri->outFile, ")");
		}
	  }
	  break;
	case DAGKEY:
	  if (inizio) {
		  fprintf(parametri->outFile, "<dag>");
		  visitTesto(testo->testo2, parametri, sigle);
		  fprintf(parametri->outFile, "</dag>");
	  }
	  break;
	case HREFKEY:
	  if (inizio) {
		visitTesto(testo->testo2, parametri, sigle);
	  }
	  break;
	  // case NOTAMACRO:
	case OUTRANGE:
	  if (inizio) {
	    //fprintf(parametri->outFile, "??");
	  }
	  break;
	  /*
		case NOKEY:
		if (testo) {
		if (inizio) {
		//fprintf(parametri->outFile, "{");
		fprintf(parametri->outFile, "{");
		} else {
		//fprintf(parametri->outFile, "}");
		fprintf(parametri->outFile, "}");
		}
		}
		break;
	  */
	}
	//fprintf(parametri->outFile, "</macro>");
  }
}

// FUNZIONE DI STAMPA DI UNA FIGURA
void printFig(short key, struct testo * testo, PARAMETRI * parametri) {
  fprintf(parametri->outFile, "<figure>");
  if (figExtension == NULL) {
	figExtension = new char[4];
	sprintf(figExtension, "%s", "jpg");
  }
  switch (key) {
  case FIGURAKEY:
	fprintf(parametri->outFile,"<graphic url=\"");
	visitTesto(testo->testo2, parametri, NULL);
	fprintf(parametri->outFile, ".%s\" />", figExtension);
	break;
  case FIGSKIPKEY:
	//fprintf(parametri->outFile, "<figskip skip=\"");
	//visitTesto(testo->testo2, parametri, NULL);
	//fprintf(parametri->outFile, "\" />");
	break;
  case FMARGKEY:
	fprintf(parametri->outFile, "<fmarg>");
	visitTesto(testo->testo2, parametri, NULL);
	fprintf(parametri->outFile, "</fmarg>");
	break;
  case FORMULAKEY:
	fprintf(parametri->outFile, "<formula> ");
	visitTesto(testo->testo2, parametri, NULL);
	fprintf(parametri->outFile, "</formula>");
	break;
  case TAVOLAKEY:
	fprintf(parametri->outFile, "<table>");
	visitTesto(testo->testo2, parametri, NULL);
	fprintf(parametri->outFile, "</table>");
	break;
  }
  if (testo->testo1) {
	fprintf(parametri->outFile, "<figDesc>");
	visitTesto(testo->testo1, parametri, NULL);
	fprintf(parametri->outFile, "</figDesc>");
  }
  fprintf(parametri->outFile, "</figure>");
}

// FUNZIONE DI STAMPA DI UN RIFERIMENTO AD UNA UNIT
void printUnitRef(const char * opera, const char * etichetta, PARAMETRI * parametri) {
  short nUnit = findUnit(etichetta);
  printUnitRef(nUnit, parametri);
}

void printUnitRef(short nUnit, PARAMETRI * parametri) {
	fprintf(parametri->outFile, "<unitref>%d</unitref>\n", nUnit);
}

// FUNZIONE DI STAMPA DI UNA PAROLA
void printLexema(const short key, const char * lexema, PARAMETRI * param) {
  
  PARAMETRIm2xml * parametri = (PARAMETRIm2xml *) param;
  
  if (lexema != NULL) {
    
	// CONTROLLO INIBIZIONE DELLA STAMPA
	if (parametri->label == NULL || parametri->nota) {
      
	  switch (key) {
	
		// DOCUMENTCLASS
	  case EXTERNALDOCUMENT:
	  case CLASSKEY:
		//fprintf(parametri->outFile, "{%s}\n\n", lexema);
		break;

	  case UNITKEY:
		break;
		// ACCENTI
	  case GRAVE:
		printAccento(lexema, "grave", parametri->outFile);
		break;
	  case ACUTE:
		printAccento(lexema, "acute", parametri->outFile);
		break;
	  case UML:
		printAccento(lexema, "uml", parametri->outFile);
		break;
	  case CIRC:
		printAccento(lexema, "circ", parametri->outFile);
		break;
	  case TILDE:
		printAccento(lexema, "tilde", parametri->outFile);
	
		// SIMBOLI E MACRO
	  case PARSKIP:
		//fprintf(parametri->outFile, "<parskip=%s\n />", lexema);
		break;
	  case PARINDENT:
		//fprintf(parametri->outFile, "<parindent=%s\n />", lexema);
		break;
	  case SYMBOL:
		fprintf(parametri->outFile, "<%s />", &lexema[1]);
		break;
	  case MACRO:
		if (strcmp(lexema, "\\ss")    == 0)       fprintf(parametri->outFile,"&#224;");
		if (strcmp(lexema, "\\ae")    == 0)       fprintf(parametri->outFile,"ae");
		if (strcmp(lexema, "\\oe")    == 0)       fprintf(parametri->outFile,"&#339;");
		if (strcmp(lexema, "\\OE")    == 0)       fprintf(parametri->outFile,"&#338;");
		if (strcmp(lexema, "\\AE")    == 0)       fprintf(parametri->outFile,"AE");
		if (strcmp(lexema, "\\c e")   == 0)       fprintf(parametri->outFile,"e(ced)");
		if (strcmp(lexema, "\\c p")   == 0)       fprintf(parametri->outFile,"<strike>p</strike>");
		if (strcmp(lexema, "\\c c")   == 0)       fprintf(parametri->outFile,"&ccedil;");
		if (strcmp(lexema, "\\c C")   == 0)       fprintf(parametri->outFile,"&Ccedil;");
		if (strcmp(lexema, "\\&")     == 0)       fprintf(parametri->outFile,"&amp;");
		if (strcmp(lexema, "\\it\\&") == 0)       fprintf(parametri->outFile,"<emph><b>&amp;</b></emph>");
		if (strcmp(lexema, "\\.o")    == 0)       fprintf(parametri->outFile,"<strike>o</strike>");
		if (strcmp(lexema, "\\v o")   == 0)       fprintf(parametri->outFile,"<strike>o</strike>");
		if (strcmp(lexema, "\\Alpha") == 0)       fprintf(parametri->outFile,"\\Alpha;");
		if (strcmp(lexema, "\\Beta") == 0)        fprintf(parametri->outFile,"\\Beta;");
		if (strcmp(lexema, "\\Gamma") == 0)       fprintf(parametri->outFile,"\\Gamma;");
		if (strcmp(lexema, "\\Delta") == 0)       fprintf(parametri->outFile,"\\Delta;");
		if (strcmp(lexema, "\\Epsilon") == 0)     fprintf(parametri->outFile,"\\Epsilon;");
		if (strcmp(lexema, "\\Zeta") == 0)        fprintf(parametri->outFile,"\\Zeta;");
		if (strcmp(lexema, "\\Eta") == 0)         fprintf(parametri->outFile,"\\Eta;");
		if (strcmp(lexema, "\\Theta") == 0)       fprintf(parametri->outFile,"\\Theta;");
		if (strcmp(lexema, "\\Iota") == 0)        fprintf(parametri->outFile,"\\Iota;");
		if (strcmp(lexema, "\\Kappa") == 0)       fprintf(parametri->outFile,"\\Kappa;");
		if (strcmp(lexema, "\\Lambda") == 0)      fprintf(parametri->outFile,"\\Lambda;");
		if (strcmp(lexema, "\\Mu") == 0)          fprintf(parametri->outFile,"\\Mu;");
		if (strcmp(lexema, "\\Nu") == 0)          fprintf(parametri->outFile,"\\Nu;");
		if (strcmp(lexema, "\\Xi") == 0)          fprintf(parametri->outFile,"\\Xi;");
		if (strcmp(lexema, "\\Omicron") == 0)     fprintf(parametri->outFile,"\\Omicron;");
		if (strcmp(lexema, "\\Pi") == 0)          fprintf(parametri->outFile,"\\Pi;");
		if (strcmp(lexema, "\\Rho") == 0)         fprintf(parametri->outFile,"\\Rho;");
		if (strcmp(lexema, "\\Sigma") == 0)       fprintf(parametri->outFile,"\\Sigma;");
		if (strcmp(lexema, "\\Tau") == 0)         fprintf(parametri->outFile,"\\Tau;");
		if (strcmp(lexema, "\\Upsilon") == 0)     fprintf(parametri->outFile,"\\Upsilon;");
		if (strcmp(lexema, "\\Phi") == 0)         fprintf(parametri->outFile,"\\Phi;");
		if (strcmp(lexema, "\\Chi") == 0)         fprintf(parametri->outFile,"\\Chi;");
		if (strcmp(lexema, "\\Psi") == 0)         fprintf(parametri->outFile,"\\Psi;");
		if (strcmp(lexema, "\\Omega") == 0)       fprintf(parametri->outFile,"\\Omega;");
		if (strcmp(lexema, "\\alpha") == 0)       fprintf(parametri->outFile,"\\alpha;");
		if (strcmp(lexema, "\\beta") == 0)        fprintf(parametri->outFile,"\\beta;");
		if (strcmp(lexema, "\\gamma") == 0)       fprintf(parametri->outFile,"\\gamma;");
		if (strcmp(lexema, "\\delta") == 0)       fprintf(parametri->outFile,"\\delta;");
		if (strcmp(lexema, "\\varepsilon") == 0)  fprintf(parametri->outFile,"\\varepsilon;");
		if (strcmp(lexema, "\\epsilon") == 0)     fprintf(parametri->outFile,"\\epsilon;");
		if (strcmp(lexema, "\\zeta") == 0)        fprintf(parametri->outFile,"\\zeta;");
		if (strcmp(lexema, "\\eta") == 0)         fprintf(parametri->outFile,"\\eta;");
		if (strcmp(lexema, "\\theta") == 0)       fprintf(parametri->outFile,"\\theta;");
		if (strcmp(lexema, "\\iota") == 0)        fprintf(parametri->outFile,"\\iota;");
		if (strcmp(lexema, "\\kappa") == 0)       fprintf(parametri->outFile,"\\kappa;");
		if (strcmp(lexema, "\\lambda") == 0)      fprintf(parametri->outFile,"\\lambda;");
		if (strcmp(lexema, "\\mu") == 0)          fprintf(parametri->outFile,"\\mu;");
		if (strcmp(lexema, "\\nu") == 0)          fprintf(parametri->outFile,"\\nu;");
		if (strcmp(lexema, "\\xi") == 0)          fprintf(parametri->outFile,"\\xi;");
		if (strcmp(lexema, "\\omicron") == 0)     fprintf(parametri->outFile,"\\omicron;");
		if (strcmp(lexema, "\\pi") == 0)          fprintf(parametri->outFile,"\\pi;");
		if (strcmp(lexema, "\\rho") == 0)         fprintf(parametri->outFile,"\\rho;");
		if (strcmp(lexema, "\\varsigma") == 0)    fprintf(parametri->outFile,"\\sigmaf;");
		if (strcmp(lexema, "\\sigma") == 0)       fprintf(parametri->outFile,"\\sigma;");
		if (strcmp(lexema, "\\tau") == 0)         fprintf(parametri->outFile,"\\tau;");
		if (strcmp(lexema, "\\upsilon") == 0)     fprintf(parametri->outFile,"\\upsilon;");
		if (strcmp(lexema, "\\varphi") == 0)      fprintf(parametri->outFile,"\\varphi;");
		if (strcmp(lexema, "\\phi") == 0)         fprintf(parametri->outFile,"\\phi;");
		if (strcmp(lexema, "\\chi") == 0)         fprintf(parametri->outFile,"\\chi;");
		if (strcmp(lexema, "\\psi") == 0)         fprintf(parametri->outFile,"\\psi;");
		if (strcmp(lexema, "\\omega") == 0)       fprintf(parametri->outFile,"\\omega;");
		break;
	
	  default:
		// PAROLA GENERICA
		if (parametri->greek) {
		  for (unsigned int i = 0; i < strlen(lexema); i++) {
			printGreco(lexema[i], parametri);
		  }
		} else {
		  fprintf(parametri->outFile, "%s", lexema);
		}
		break;
	  }
	}
  }
  fflush(parametri->outFile);
}

// FUNZIONE DI STAMPA DEL FRONTESPIZIO
void printFrontespizio(struct testo * testo,
					   PARAMETRI * parametri,
					   struct testo * sigle) {
}

// FUNZIONE DI STAMPA DI UNA LISTA DI AUTORI
void printAuthors(struct testo * testo,
				  PARAMETRI * parametri,
				  struct testo * sigle) {
}


void printTextGreek(struct testo * testo,
					PARAMETRI * parametri,
					struct testo * sigle) {
  fprintf(parametri->outFile, "<foreign xml:lang=\"greek\">\n");
  visitTesto(testo, parametri, sigle);
  fprintf(parametri->outFile, "</foreign>\n");
}

// FUNZIONE DI STAMPA DI UNA NOTA A PIEDE DI PAGINA
void printFootnote(struct testo * testo,
				   PARAMETRI * parametri,
				   struct testo * sigle) {
}

// FUNZIONE DI STAMPA DI UN COMMENTO
void printCommento(struct testo * testo,
		PARAMETRI * param,
		struct testo * sigle) {
}

// FUNZIONE DI STAMPA DI UNA ANNOTAZIONE
void printAdnotatio(struct testo * testo,
					PARAMETRI * parametri,
					struct testo * sigle) {
	//fprintf(parametri->outFile, "<adnotatio>\n");
	//visitTesto(testo, parametri, sigle);
	//fprintf(parametri->outFile, "</adnotatio>\n");
}

// FUNZIONE DI MARCATURA DEL TESTO CRITICO
void printInizioTestoCritico(short key, PARAMETRI * parametri) {
}

void printFineTestoCritico(short key, PARAMETRI * parametri) {
}

// FUNZIONE DI MARCATURA DELLE VARIANTI
void printInizioVarianti(PARAMETRI * parametri) {
}

void printSepVarianti(PARAMETRI * parametri) {
}

void printFineVarianti(PARAMETRI * parametri) {
}

// STAMPO LE SIGLE
void stampaSigle(struct testo * testo, PARAMETRIm2xml * parametri){
  while (testo != NULL) {
	fprintf(parametri->outFile, " %s",
			(testo->lexema==NULL ? parametri->testimone : testo->lexema));
	testo = testo->testo2;
  }
}

// STAMPO LA LEZIONE
void stampaLezione(struct testo * testo, PARAMETRIm2xml * parametri, struct testo * sigle){
	visitTesto(testo, parametri, sigle);
}

// STAMPO LA NOTE
void stampaNote(struct testo   * testimoni,
				struct testo   * testo,
				PARAMETRIm2xml * parametri,
				struct testo   * sigle){
  
  // MEMORIZZO LE NOTE DELLA VV
  struct testo * n = testo->testo2;
  
  if (testimoni != NULL &&
	  (testimoni->lexema == NULL || strcmp(testimoni->lexema, "*") != 0)) {
	stampaSigle(testimoni, parametri);
	while (n != NULL) {
	  if (n->testo1) {
		switch (n->testo1->key) {
		case CRKEY:
		  break;
		case ANTEDELKEY:
		  fprintf(parametri->outFile, " ante ~ delevit ");
		  visitTesto(n->testo1, parametri, sigle);
		  break;
		case POSTDELKEY:
		  fprintf(parametri->outFile, " post ~ delevit ");
		  visitTesto(n->testo1, parametri, sigle);
		  break;
		case LECKEY:
		  fprintf(parametri->outFile, " ");
		  visitTesto(n->testo1, parametri, sigle);
		  break;
		case TRKEY:
		  fprintf(parametri->outFile, " ");
		  visitTesto(n->testo1, parametri, sigle);
		  break;
		case EDKEY:
		  fprintf(parametri->outFile, " ");
		  visitTesto(n->testo1, parametri, sigle);
		  break;
		case POSTSCRIPTKEY:
		  fprintf(parametri->outFile, " postscript ");
		  break;
		case BISKEY:
		  fprintf(parametri->outFile, " bis ");
		  break;
		case SUPRAKEY:
		case INTERLKEY:
		  fprintf(parametri->outFile, " supra lineam ");
		  break;
		case MARGKEY:
		  fprintf(parametri->outFile, " in margine ");
		  break;
		case MARGSIGNKEY:
		  fprintf(parametri->outFile, " signo posito in margine ");
		  break;
		case PCKEY:
		  fprintf(parametri->outFile, " post correctionem ");
		  break;
		case EXKEY:
		  fprintf(parametri->outFile, " correxit ex ");
		  visitTesto(n->testo1, parametri, sigle);
		  fprintf(parametri->outFile, "\n");
		  break;
		case EDCOMPLKEY:
		case DESCOMPLKEY:
		  fprintf(parametri->outFile, " ");
		  visitTesto(n->testo1->testo1->testo1, parametri, sigle);
		  fprintf(parametri->outFile, " ~ ");
		  visitTesto(n->testo1->testo1->testo2->testo1, parametri, sigle);
		  fprintf(parametri->outFile, " ");
		  visitTesto(n->testo1->testo1->testo2->testo2, parametri, sigle);
		  fprintf(parametri->outFile, "\n");
		  break;
		}
	  }
	  n = n->testo2;
	}
	fprintf(parametri->outFile, "\n");
  }
}

// FUNZIONE PER DISABILITARE LA GESTIONE DELLA VV DI MVISIT
bool checkVV(short          key,
	     struct testo * testo,
	     PARAMETRI    * param,
	     struct testo * sigle) {
  PARAMETRIm2xml * parametri = (PARAMETRIm2xml *) param;
  if (key == VBKEY) {
    struct testo * restoVV = testo->testo1->testo2;
    if (restoVV) {
      if (restoVV->testo1) {
	visitTesto(restoVV->testo1, parametri, sigle);
      }
    }
  } else {
    fprintf(parametri->outFile, "<variant>");
    struct testo * t = testo;
    if (t != NULL) {
      fprintf(parametri->outFile, "<reading>\n");
      struct testo * restoVV = t->testo1->testo2;
      stampaLezione(restoVV->testo1, parametri, sigle);
      fprintf(parametri->outFile, "<app>\n");
      stampaNote(t->testo1->testo1, restoVV, parametri, sigle);
      t = t->testo2;
      while (t != NULL) {
	if (t->testo1) {
	  struct testo * restoVV = t->testo1->testo2;
	  if (restoVV) {
	    fprintf(parametri->outFile, "<i>");
	    if (restoVV->testo1) {
	      stampaLezione(restoVV->testo1, parametri, sigle);
	    }
	    fprintf(parametri->outFile, "</i>\n");
	    stampaNote(t->testo1->testo1, restoVV, parametri, sigle);
	  }
	}
	t = t->testo2;
      }
      fprintf(parametri->outFile, "</app>\n");
      fprintf(parametri->outFile, "</reading>\n");
    }
    fprintf(parametri->outFile, "</variant>\n");
  }
  return false;
}

// FUNZIONE DI STAMPA DI UNA NOTA
void tisivVV(short          key,
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

// FUNZIONE DI STAMPA DI UNA NOTA
void printVV(short          key,
			 struct testo * testo,
			 PARAMETRI    * param,
			 struct testo * sigle) {
}

// FUNZIONE DI STAMPA DELL'ALBERO SINTATTICO
void printDocumento(struct testo * testo) {
	PARAMETRIm2xml * parametri;
	if (testo) {
		xmlFileName = new char[strlen(fileName)+10];
		sprintf(xmlFileName, "%s%s", fileName, XMLEST);
		sprintf(xmlFileName, "%s%s", fileName, XMLEST);
		parametri = new PARAMETRIm2xml();
		parametri->setFileName(xmlFileName);
		parametri->banale = true;
		visitTesto(testo, parametri, NULL);
		delete(parametri);
	}
}

// PROGRAMMA PRINCIPALE
int main (int argn, char ** argv) {
  
	char * fileName = NULL;
	bool ok         = true;
	bool help       = false;
	nSpaces         = 0;
	content         = "";
  
	for (int i = 1; i < argn; i++) {
	  
	  // CONTROLLO DEGLI ARGOMENTI
	  switch (argv[i][0]) {
		
		// CONTROLLO LA PRESENZA DI OPZIONI
	  case '-' :
		switch (argv[i][1]) {
		  // MOSTRA L'HELP
		case 'h' :
		  help = true;
		  break;
		case 'c':
		  i++;
		  content = argv[i];
		  break;
		}
		
		break;
		
	  default :
		// NOME  DEL FILE DA ANALIZZARE
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
	  
	  // INVOCAZIONE DELL'ANALIZZATORE SINTATTICO
	  parser(fileName);
	} else {
	  
	  // STAMPA L'HELP
	  printf("traduttore MauroTeX -> XML (%s)\n", VERSIONSTRING);
	  printf("uso: %s [-h] <nome_file>\n", argv[0]);
	  if (help) {
		printf("opzioni:\n");
		printf("  -h             stampa questo messaggio\n");
	  }
	}
}
