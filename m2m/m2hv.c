#define VERSIONSTRING "versione 1.13b19 del 12 Novembre 2020"

/*

  file "m2hv.c"
  PROGRAMMA PRINCIPALE

  m2h: traduttore da M-TeX a HTML basato mvisit

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

#include "comp.h"

#include "mparse.h"
#include "mparsey.h"
#include "mvisit.h"

#define HTMLEST  ".htm"
#define ADNOTEST "a.htm"
#define COMMEST  "z.htm"
#define VVEST    "v.htm"
#define OMEST    "o.htm"
#define LSEST    "s.htm"
#define EREST    "e.htm"
#define COEST    "d.htm"
#define CITEST   "c.htm"
#define FRAMEEST "f.htm"
#define INDEST   "ind.htm"

#define MAXCONF 100


typedef struct externalLabel {
  const char  * label;
  externalLabel * next;
} externalLabel;

static externalLabel * externalLabels = NULL;

const char * findExternalLabel(const char * label) {
  
  externalLabel * lab = externalLabels;
  
  while (lab != NULL && strncmp(lab->label, label, strlen(lab->label) != 0)) {
	lab = lab->next;
  }

  return (lab == NULL) ? NULL : lab->label;
}

bool addExternalLabel (const char * label) {
  
  bool inserito = (findExternalLabel(label) == NULL);
  
  if (inserito) {
    externalLabel * l = new externalLabel;
    l->label = label;
    l->next  = externalLabels;
    externalLabels = l;
  }
  return inserito;
}

// STRUTTURA DATI PARAMETRI
class PARAMETRIm2h: public PARAMETRI {

public:
	short nER;
	short nCO;
	short nCO1;
	short nCO2;
	short nCIT;
	bool  justify;
	short mRow, mCol;
	short nPag;
	short idPag;

	PARAMETRIm2h(): PARAMETRI() {
		this->nER     = 0;
		this->nCO     = 0;
		this->nCO1    = 0;
		this->nCO2    = 0;
		this->nCIT    = 0;
		this->justify = true;
		this->mRow    = 1;
		this->mCol    = 1;
		this->nPag    = 0;
		this->idPag   = 0;
	}
};

// VARIABILI GLOBALI
#define MAXTEST 10

static short  nTestLs;         // NUMERO DI LEZIONI SINGOLARI
static short  nTestOm;         // NUMERO DI TESTIMONI DI RIFERIMENTO PER LE OMISSIONI
static short  nTestEr;         // NUMERO DI TESTIMONI DI RIFERIMENTO PER GLI ERRORI
static short  nUnitRef;        // INDICE UNIT DI RIFERIMENTO
static char * testOm[MAXTEST]; // TESTIMONI PER LE OMISSIONI
static char * testLs[MAXTEST]; // TESTIMONI PER LEZIONE SINGOLARE
static char * testEr[MAXTEST]; // TESTIMONI PER ERRORI
static char * testEx;          // TESTIMONE PER ESTRAZIONE TESTO

static char * testD1;          // PRIMO argv PER CONCORDANZE
static char * testD2;          // SECONDO argv PER CONCORDANZE
const  char * coSEP = ",";     // SEPARATORE DI TESTIMONI NEI DEU GRUPPI INPUT
static char *test_1_L[MAXTEST];// LISTA 1 DI TESTIMONI PER CONCORDANZE
static char *test_2_L[MAXTEST];// LISTA 2 DI TESTIMONI PER CONCORDANZE
static char * coT1DLName;      // SEQUENZA DI TESTIMONI GRUPPO 1
static char * coT2DLName;      // SEQUENZA DI TESTIMONI GRUPPO 2

static char * htmlFileName;
static char * vvFileName;
static char * indFileName;
static char * erFileName[MAXTEST];
static char * coFileName;
static char * omFileName[MAXTEST];
static char * lsFileName[MAXTEST];
static char * confFileName;
static char * unitFileName;
static char * adFileName;
static char * commFileName;
static char * citFileName;
static char * frameFileName;
static bool   vbTovv;          // VARIANTI BANALI COME EFFETTIVE
static bool   graffe;          // PARENTESI GRAFFE INVECE DI BARRETTE
static bool   exclude;         // ESCLUDE STAMPA CITAZIONE COMPLETA
static bool   numtest;         // STAMPA NUMERO TESTIMONI DI UNA VARIANTE
static bool   analisi;         // ESEGUE SOLO L'ANALISI DEL SORGENTE
static bool   commenti;        // STAMPA I COMMENTI
static bool   livello;         // NON STAMPA GLI APPARATI
static bool   verbose;         // STAMPA I MESSAGGI DI WARNING
static bool   secNumber;       // STAMPA I NUMERI DI SEZIONE, SOTTOSEZIONE, ...

static FILE *indFile;          // DICHIARAZIONE DEL FILE INDICE
static FILE *unitFile;         // DICHIARAZIONE FILE UNIT DI RIFERIMENTO

static const char *greekChar[26] = {  // NOME CARATTERI GRECI
		"alpha",
		"beta",
		"csi",
		"delta",
		"epsilon",
		"phi",
		"gamma",
		"eta",
		"iota",
		"",
		"kappa",
		"lambda",
		"mu",
		"nu",
		"omicron",
		"pi",
		"theta",
		"ro",
		"sigma",
		"tau",
		"upsilon",
		"",
		"omega",
		"chi",
		"psi",
		"zeta"
};

// FUNZIONI PER OPZIONI GESTITE ESTERNAMENTE
bool verboseWarning() {
	return verbose;
}

// FUNZIONI SPECIFICHE DEL TRADUTTORE HTML

// RITORNA IL TESTIMONE PER VEDERE IL SUO TESTO SE IMPOSTATO
char * opzioneTestEx() {
	return testEx;
}

// INSERISCE IL TAG DI STILE DI UNA NOTA
void printTagNota(FILE * outFile, bool inizio) {
	if (inizio) {
		fprintf(outFile, "<font color=green><em>");
	} else {
		fprintf(outFile, "</em></font>");
	}
}

// FUNZIONE DI STAMPA DELLE MANI
void printManus(struct testo *testo, PARAMETRI* param, struct testo *sigle) {}

// FUNZIONE CHE STAMPA IL PROLOGO DI UNA TABELLA
void printTableStart(int key, struct testo * testo, PARAMETRI * parametri, struct testo * sigle) {
	fprintf(parametri->outFile, "<table cellpadding=\"0\" cellspacing=\"4\"><tr><td>\n\n");
	visitTesto(testo, parametri, sigle);
	fprintf(parametri->outFile, "</td><td>\n\n");
}

// FUNZIONE CHE STAMPA L'EPILOGO DI UNA TABELLA
void printTableFinish(int key, struct testo * testo, PARAMETRI * parametri, struct testo * sigle) {
	fprintf(parametri->outFile, "</td><td>\n\n");
	visitTesto(testo, parametri, sigle);
	fprintf(parametri->outFile, "</td></tr></table>\n\n");
}

// FUNZIONE CHE STAMPA LO HEADER DI RIGA DI UNA TABELLA
void printRowHeader(short key, PARAMETRI * parametri) {
	if (key != HLINEKEY) {
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
  if (strchr(colonne, '|') == NULL) {
    fprintf(parametri->outFile, "<table>\n");
  } else {
    fprintf(parametri->outFile, "<table border=1>\n");
  }
}

// FUNZIONE CHE STAMPA IL FOOTER DI UNA TABELLA
void printTableFooter(int key, PARAMETRI * parametri) {
  fprintf(parametri->outFile, "</table>\n");
}

void printTextGreek(struct testo * testo,
					PARAMETRI * parametri,
					struct testo * sigle) {
  visitTesto(testo, parametri, sigle);
}

// FUNZIONE CHE STAMPA UN CAMPO DI UNA TABELLA
void printCampo(short          nRow,
		short          nCol,
		const char   * colonne,
		struct testo * campo,
		PARAMETRI    * param,
		struct testo * sigle) {

	PARAMETRIm2h * parametri = (PARAMETRIm2h *) param;

	short i, j;
	const char * attr;

	// CERCA L'ALLINEAMENTO DELLA COLONNA
	i = 0;
	j = 0;
	do {
		while (colonne[j] == '|') {
			j++;
		}
		i++;
		j++;
	} while (i < nCol && colonne[j] != 0);

	// DEFINIZIONE DELL'ATTRIBUTO DI ALLINEAMENTO
	switch (colonne[j-1]) {
	case 'r':
		attr = "right";
		break;
	case 'c':
		attr = "center";
		break;
	case 'l':
	default:
		attr = "left";
		break;
	}

	fprintf(parametri->outFile, "<td nowrap ");

	if (campo == NULL) {
		fprintf(parametri->outFile, "> &nbsp;");
	} else {

		// STAMPA IL CAMPO
		char cols[50];
		const char *bgColor = "";

		// SALVATAGGIO PARAMETRI
		short mRow = parametri->mRow;
		short mCol = parametri->mCol;

		// GESTIONE INIZIALE MULTIROW E MULTICOL
		switch (campo->key) {
		case MULTICOLKEY:
			sprintf(cols, " colspan=\"%d\"", atoi(campo->lexema));
			parametri->mCol = atoi(campo->lexema);
			break;
		case MULTIROWKEY:
			sprintf(cols, " rowspan=\"%d\"", atoi(campo->lexema));
			parametri->mRow = atoi(campo->lexema);
			break;
		default:
			strcpy(cols, "");
		}

		// CONTROLLO GESTIONE GRAFFE
		if (!graffe) {
			switch (campo->key) {
			case LEFTGRATABKEY:
			case RIGHTGRATABKEY:
			case LEFTANGTABKEY:
			case RIGHTANGTABKEY:
				bgColor = " bgcolor=\"black\"";
				break;
			default:
				if (campo->testo1) {
					switch (campo->testo1->key) {
					case LEFTGRATABKEY:
					case RIGHTGRATABKEY:
					case LEFTANGTABKEY:
					case RIGHTANGTABKEY:
						bgColor = " bgcolor=\"black\"";
						break;
					}
				}
			}
		}

		// ATTRIBUTI CAMPO
		fprintf(parametri->outFile, "align=\"%s\"%s%s>\n", attr, cols, bgColor);

		// CONTENUTO CAMPO
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

		// RIPRISTINO PARAMETRI
		parametri->mCol = mCol;
		parametri->mRow = mRow;

	}

	// FINE CAMPO
	fprintf(parametri->outFile, "</td>\n");

}

// STAMPA I TITOLI DELLE OPERE
void printTitleWork(struct testo *testo, PARAMETRI * parametri, struct testo *sigle){
  fprintf(parametri->outFile, "<center><h1>\n");
  visitTesto(testo->testo1, parametri, sigle);
  fprintf(parametri->outFile, "</h1></center>\n");
	fprintf(parametri->outFile, "<center><h2>\n");
  if (testo->testo2 != NULL) {
	visitTesto(testo->testo2, parametri, sigle);
  }
	fprintf(parametri->outFile, "</h2></center>\n");
}

// FUNZIONE DI STAMPA DI ALIAMANUS
void printAliaManus(struct testo * testo,
		    PARAMETRI * parametri,
		    struct testo * sigle) {
  visitTesto(testo->testo2, parametri, sigle);
}

// FUNZIONE DI STAMPA DELLE FRAZIONI
void printFrac(struct testo *testo, PARAMETRI * parametri, struct testo *sigle){
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "/");
	visitTesto(testo->testo2, parametri, sigle);
}

// FUNZIONE DI STAMPA DELL'ESPONENTE E DEL DEPONENTE
void printSupSub(struct testo *testo, PARAMETRI * parametri, struct testo *sigle){
	fprintf(parametri->outFile, "<sup>");
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "</sup><sub>");
	visitTesto(testo->testo2, parametri, sigle);
	fprintf(parametri->outFile, "</sub>");
}

// STAMPA UNA STRINGA DI NOTA DELL'EDITORE
void printNota(const char * nota, FILE * outFile) {
	printTagNota(outFile, true);
	fprintf(outFile, "%s", nota);
	printTagNota(outFile, false);
}

// STAMPA GLI ACCENTI
void printAccento(const char * lexema, const char * accento, FILE * outFile) {

	int i; // POSIZIONE DEL CARATTERE ACCENTATO

	// IGNORA L'EVENTUALE PARENTESI O BACKSLASH
	if (lexema[2] == '{' || lexema[2] == '\\') i = 3; else i = 2;

	// STAMPA IL CARATTERE COL SUO ACCENTO
	fprintf(outFile, "&%c%s;", lexema[i], accento);
}

// STAMPA TESTO "CIRCLED"
void printCircled(const char * lexema, FILE * outFile) {
	// ENCLOSED ALPHANUMERICS (da 1 a 20)
	if      (strcmp(lexema, "1" ) == 0) { fprintf(outFile, "&#9312;"); }
	else if (strcmp(lexema, "2" ) == 0) { fprintf(outFile, "&#9313;"); }
	else if (strcmp(lexema, "3" ) == 0) { fprintf(outFile, "&#9314;"); }
	else if (strcmp(lexema, "4" ) == 0) { fprintf(outFile, "&#9315;"); }
	else if (strcmp(lexema, "5" ) == 0) { fprintf(outFile, "&#9316;"); }
	else if (strcmp(lexema, "6" ) == 0) { fprintf(outFile, "&#9317;"); }
	else if (strcmp(lexema, "7" ) == 0) { fprintf(outFile, "&#9318;"); }
	else if (strcmp(lexema, "8" ) == 0) { fprintf(outFile, "&#9319;"); }
	else if (strcmp(lexema, "9" ) == 0) { fprintf(outFile, "&#9320;"); }
	else if (strcmp(lexema, "10") == 0) { fprintf(outFile, "&#9321;"); }
	else if (strcmp(lexema, "11") == 0) { fprintf(outFile, "&#9322;"); }
	else if (strcmp(lexema, "12") == 0) { fprintf(outFile, "&#9323;"); }
	else if (strcmp(lexema, "13") == 0) { fprintf(outFile, "&#9324;"); }
	else if (strcmp(lexema, "14") == 0) { fprintf(outFile, "&#9325;"); }
	else if (strcmp(lexema, "15") == 0) { fprintf(outFile, "&#9326;"); }
	else if (strcmp(lexema, "16") == 0) { fprintf(outFile, "&#9327;"); }
	else if (strcmp(lexema, "17") == 0) { fprintf(outFile, "&#9328;"); }
	else if (strcmp(lexema, "18") == 0) { fprintf(outFile, "&#9329;"); }
	else if (strcmp(lexema, "19") == 0) { fprintf(outFile, "&#9330;"); }
	else if (strcmp(lexema, "20") == 0) { fprintf(outFile, "&#9331;"); }
	// ALTRO
	else {
		fprintf(outFile, "<span style=\"border:1px solid black;border-radius:50%%;\">");
	    fprintf(outFile, "%s", lexema);
		fprintf(outFile, "</span>");
	}
}

// STAMPA LE ANCORE DEGLI OMISSIS
void printOM(const char * nota, PARAMETRIm2h * parametri, struct testo * sigle) {

	short i;

	if (!livello) {

		// LE OMISSIONI SONO INCOMPATIBILI CON L'ESTRAZIONE
		if (testEx == NULL) {

			// RICERCA DEI TESTIMONI RICHIESTI
			for (i = 0; i < nTestOm; i++) {

				// VERIFICA PRESENZA OMISSIONE
				if (findTestRif(sigle, testOm[i])) {

					// APERTURA DEL FILE DELLE OMISSIONI
					FILE * omFile = fopen(omFileName[i], "a");

					if (omFile) {

						// INSERIMENTO DEL LINK ALLA NOTA NEL FILE DELLE OMISSIONI
						fprintf(omFile, "<a href=\"%s#VV%d\" target=\"variante\">%d</a>\n",
								vvFileName, parametri->nVV, parametri->nVV);

						// CHIUSURA DEL FILE DELLE OMISSIONI
						fclose(omFile);

					} else {
						fprintf(stderr, "impossibile aprire il file %s\n", omFileName[i]);
					}
				}
			}
		}
	}

	printNota(nota, parametri->outFile);
}

// STAMPA I LINK ALLE IMMAGINI
void printImage(const char * path, const char * alt, FILE * outFile) {
	fprintf(outFile, "<img src=\"%s\" alt=\"%s\" align=\"middle\">",
			path, alt);
}

// STAMPA I SIMBOLI MATEMATICI ED ASTRONOMICI
void printSymbol(const char * lexema, FILE * outFile) {
	if (strcmp(lexema, "\\RDX")  == 0)
		printImage("micons/RDX.gif", "Radix", outFile);
	if (strcmp(lexema, "\\mtilde")  == 0)
		printImage("micons/mtilde.gif", "mtilde", outFile);
	if (strcmp(lexema, "\\ptilde")  == 0)
		printImage("micons/ptilde.gif", "ptilde", outFile);
	if (strcmp(lexema, "\\rdx")  == 0)
		printImage("micons/rdxs.gif", "radix", outFile);
	if (strcmp(lexema, "\\SOL")  == 0)
		printImage("micons/astro/SOL.gif", "Sole", outFile);
	if (strcmp(lexema, "\\LUN")  == 0)
		printImage("micons/astro/LUN.gif", "Luna", outFile);
	if (strcmp(lexema, "\\TER")  == 0)
		printImage("micons/astro/TER.gif", "Terra", outFile);
	if (strcmp(lexema, "\\MER")  == 0)
		printImage("micons/astro/MER.gif", "Mercurio", outFile);
	if (strcmp(lexema, "\\VEN")  == 0)
		printImage("micons/astro/VEN.gif", "Venere", outFile);
	if (strcmp(lexema, "\\MAR")  == 0)
		printImage("micons/astro/MAR.gif", "Marte", outFile);
	if (strcmp(lexema, "\\GIO")  == 0)
		printImage("micons/astro/GIO.gif", "Giove", outFile);
	if (strcmp(lexema, "\\SAT")  == 0)
		printImage("micons/astro/SAT.gif", "Saturno", outFile);
	if (strcmp(lexema, "\\ARS")  == 0)
		printImage("micons/astro/ARS.gif", "Ariete", outFile);
	if (strcmp(lexema, "\\TRS")  == 0)
		printImage("micons/astro/TRS.gif", "Toro", outFile);
	if (strcmp(lexema, "\\GMN")  == 0)
		printImage("micons/astro/GMN.gif", "Gemelli", outFile);
	if (strcmp(lexema, "\\CNC")  == 0)
		printImage("micons/astro/CNC.gif", "Cancro", outFile);
	if (strcmp(lexema, "\\LEO")  == 0)
		printImage("micons/astro/LEO.gif", "Leone", outFile);
	if (strcmp(lexema, "\\VRG")  == 0)
		printImage("micons/astro/VRG.gif", "Vergine", outFile);
	if (strcmp(lexema, "\\LBR")  == 0)
		printImage("micons/astro/LBR.gif", "Bilancia", outFile);
	if (strcmp(lexema, "\\SCR")  == 0)
		printImage("micons/astro/SCR.gif", "Scorpione", outFile);
	if (strcmp(lexema, "\\SGT")  == 0)
		printImage("micons/astro/SGT.gif", "Sagittario", outFile);
	if (strcmp(lexema, "\\CPR")  == 0)
		printImage("micons/astro/CPR.gif", "Capricorno", outFile);
	if (strcmp(lexema, "\\AQR")  == 0)
		printImage("micons/astro/AQR.gif", "Acquario", outFile);
	if (strcmp(lexema, "\\PSC")  == 0)
		printImage("micons/astro/PSC.gif", "Pesci", outFile);
	if (strcmp(lexema, "\\PRL")  == 0 || strcmp(lexema, "\\PG") == 0)
		printImage("micons/PRL.gif", "parallelogramma", outFile);
	if (strcmp(lexema, "\\TRN")  == 0 || strcmp(lexema, "\\T")  == 0)
		printImage("micons/TRN.gif", "triangolo", outFile);
	if (strcmp(lexema, "\\QDR")  == 0 || strcmp(lexema, "\\Q")  == 0)
		printImage("micons/QDR.gif", "quadrato", outFile);
	if (strcmp(lexema, "\\RTT")  == 0 || strcmp(lexema, "\\R")  == 0)
		printImage("micons/RTT.gif", "rettangolo", outFile);
	if (strcmp(lexema, "\\DRTT")  == 0 || strcmp(lexema, "\\R")  == 0)
		printImage("micons/DRTT.gif", "doppiorettangolo", outFile);
	if (strcmp(lexema, "\\CUB")  == 0)
		printImage("micons/CUB.gif", "cubo", outFile);
	if (strcmp(lexema, "\\PYR")  == 0)
		printImage("micons/PYR.gif", "piramide", outFile);
	if (strcmp(lexema, "\\PPD")  == 0)
		printImage("micons/PPD.gif", "parallelepipedo", outFile);
	if (strcmp(lexema, "\\PEN")  == 0)
		printImage("micons/PEN.gif", "pentagono", outFile);
	if (strcmp(lexema, "\\HEX")  == 0)
		printImage("micons/HEX.gif", "esagono", outFile);
	if (strcmp(lexema, "\\HEXC")  == 0)
		printImage("micons/HEXC.gif", "esagono centrale", outFile);
	if (strcmp(lexema, "\\TRP")  == 0 || strcmp(lexema, "\\TP") == 0)
		printImage("micons/TRP.gif", "trapezio", outFile);
	if (strcmp(lexema, "\\CNJ")  == 0)
		printImage("micons/astro/CNJ.gif", "congiunzione", outFile);
	if (strcmp(lexema, "\\OPP")  == 0)
		printImage("micons/astro/OPP.gif", "opposizione", outFile);
}

// STAMPA I CARATTERI SPECIALI
void printMacro(const char * lexema, FILE * outFile) {
	if (strcmp(lexema, "\\ss")    == 0)       fprintf(outFile,"&szlig;");
	if (strcmp(lexema, "\\ae")    == 0)       fprintf(outFile,"&aelig;");
	if (strcmp(lexema, "\\oe")    == 0)       fprintf(outFile,"&#339;");
	if (strcmp(lexema, "\\OE")    == 0)       fprintf(outFile,"&#338;");
	if (strcmp(lexema, "\\AE")    == 0)       fprintf(outFile,"&AElig;");
	if (strcmp(lexema, "\\c e")   == 0)       fprintf(outFile,"<strike>e</strike>");
	if (strcmp(lexema, "\\c p")   == 0)       fprintf(outFile,"<strike>p</strike>");
	if (strcmp(lexema, "\\c c")   == 0)       fprintf(outFile,"&ccedil;");
	if (strcmp(lexema, "\\c C")   == 0)       fprintf(outFile,"&Ccedil;");
	if (strcmp(lexema, "\\&")     == 0)       fprintf(outFile,"&amp;");
	if (strcmp(lexema, "\\it\\&") == 0)       fprintf(outFile,"<em><strong>&amp;</strong></em>");
	if (strcmp(lexema, "\\.o")    == 0)       fprintf(outFile,"<strike>o</strike>");
	if (strcmp(lexema, "\\v o")   == 0)       fprintf(outFile,"<strike>o</strike>");
	if (strcmp(lexema, "\\Alpha") == 0)       fprintf(outFile,"&Alpha;");
	if (strcmp(lexema, "\\Beta") == 0)        fprintf(outFile,"&Beta;");
	if (strcmp(lexema, "\\Gamma") == 0)       fprintf(outFile,"&Gamma;");
	if (strcmp(lexema, "\\Delta") == 0)       fprintf(outFile,"&Delta;");
	if (strcmp(lexema, "\\Epsilon") == 0)     fprintf(outFile,"&Epsilon;");
	if (strcmp(lexema, "\\Zeta") == 0)        fprintf(outFile,"&Zeta;");
	if (strcmp(lexema, "\\Eta") == 0)         fprintf(outFile,"&Eta;");
	if (strcmp(lexema, "\\Theta") == 0)       fprintf(outFile,"&Theta;");
	if (strcmp(lexema, "\\Iota") == 0)        fprintf(outFile,"&Iota;");
	if (strcmp(lexema, "\\Kappa") == 0)       fprintf(outFile,"&Kappa;");
	if (strcmp(lexema, "\\Lambda") == 0)      fprintf(outFile,"&Lambda;");
	if (strcmp(lexema, "\\Mu") == 0)          fprintf(outFile,"&Mu;");
	if (strcmp(lexema, "\\Nu") == 0)          fprintf(outFile,"&Nu;");
	if (strcmp(lexema, "\\Xi") == 0)          fprintf(outFile,"&Xi;");
	if (strcmp(lexema, "\\Omicron") == 0)     fprintf(outFile,"&Omicron;");
	if (strcmp(lexema, "\\Pi") == 0)          fprintf(outFile,"&Pi;");
	if (strcmp(lexema, "\\Rho") == 0)         fprintf(outFile,"&Rho;");
	if (strcmp(lexema, "\\Sigma") == 0)       fprintf(outFile,"&Sigma;");
	if (strcmp(lexema, "\\Tau") == 0)         fprintf(outFile,"&Tau;");
	if (strcmp(lexema, "\\Upsilon") == 0)     fprintf(outFile,"&Upsilon;");
	if (strcmp(lexema, "\\Phi") == 0)         fprintf(outFile,"&Phi;");
	if (strcmp(lexema, "\\Chi") == 0)         fprintf(outFile,"&Chi;");
	if (strcmp(lexema, "\\Psi") == 0)         fprintf(outFile,"&Psi;");
	if (strcmp(lexema, "\\Omega") == 0)       fprintf(outFile,"&Omega;");
	if (strcmp(lexema, "\\alpha") == 0)       fprintf(outFile,"&alpha;");
	if (strcmp(lexema, "\\beta") == 0)        fprintf(outFile,"&beta;");
	if (strcmp(lexema, "\\gamma") == 0)       fprintf(outFile,"&gamma;");
	if (strcmp(lexema, "\\delta") == 0)       fprintf(outFile,"&delta;");
	if (strcmp(lexema, "\\varepsilon") == 0)  fprintf(outFile,"&epsilon;");
	if (strcmp(lexema, "\\epsilon") == 0)     fprintf(outFile,"&epsilon;");
	if (strcmp(lexema, "\\zeta") == 0)        fprintf(outFile,"&zeta;");
	if (strcmp(lexema, "\\eta") == 0)         fprintf(outFile,"&eta;");
	if (strcmp(lexema, "\\theta") == 0)       fprintf(outFile,"&theta;");
	if (strcmp(lexema, "\\iota") == 0)        fprintf(outFile,"&iota;");
	if (strcmp(lexema, "\\kappa") == 0)       fprintf(outFile,"&kappa;");
	if (strcmp(lexema, "\\lambda") == 0)      fprintf(outFile,"&lambda;");
	if (strcmp(lexema, "\\mu") == 0)          fprintf(outFile,"&mu;");
	if (strcmp(lexema, "\\nu") == 0)          fprintf(outFile,"&nu;");
	if (strcmp(lexema, "\\xi") == 0)          fprintf(outFile,"&xi;");
	if (strcmp(lexema, "\\omicron") == 0)     fprintf(outFile,"&omicron;");
	if (strcmp(lexema, "\\pi") == 0)          fprintf(outFile,"&pi;");
	if (strcmp(lexema, "\\rho") == 0)         fprintf(outFile,"&rho;");
	if (strcmp(lexema, "\\varsigma") == 0)    fprintf(outFile,"&sigmaf;");
	if (strcmp(lexema, "\\sigma") == 0)       fprintf(outFile,"&sigma;");
	if (strcmp(lexema, "\\tau") == 0)         fprintf(outFile,"&tau;");
	if (strcmp(lexema, "\\upsilon") == 0)     fprintf(outFile,"&upsilon;");
	if (strcmp(lexema, "\\varphi") == 0)      fprintf(outFile,"&phi;");
	if (strcmp(lexema, "\\phi") == 0)         fprintf(outFile,"&phi;");
	if (strcmp(lexema, "\\chi") == 0)         fprintf(outFile,"&chi;");
	if (strcmp(lexema, "\\psi") == 0)         fprintf(outFile,"&psi;");
	if (strcmp(lexema, "\\omega") == 0)       fprintf(outFile,"&omega;");
}

// CONTATORE DELLE RIGHE SULLE QUALI SI ESTENDE IL CASES
int contaCasi(struct testo * testo) {

	int nrighe = 1;
	struct testo * caso = testo;

	while (caso != NULL) {
		nrighe++;
		caso = caso->testo2;
	}

	return nrighe;
}


// STAMPA UN BLOCCO DI TESTO CONTENUTO NEI CASES
void printCasi(struct testo * testo, PARAMETRI * parametri, struct testo * sigle, bool primo, const char * lexema) {
  
  int nrighe = 0;
  
  if (testo) {
	fprintf(parametri->outFile, "<tr>");
	// SE E' IL PRIMO CASE
	if (primo) {
	  // CONTO LE RIGHE INTERESSATE DAL CASE
	  nrighe = contaCasi(testo->testo2);
	  
	  // SE CI SONO PARENTESI APERTE
	  if (strcmp(lexema, "\\Rcases") == 0 || strcmp(lexema, "\\RLcases") == 0 ||
		  strcmp(lexema, "\\Rbracecases") == 0 || strcmp(lexema, "\\RLbracecases") == 0) {
		
		if (graffe) {
		  
		  // STAMPO LE GRAFFE SE E' SCELTA L'OPZIONE
		  fprintf(parametri->outFile, "<td rowspan=%d><img src=\"micons/braces/aleft%d.gif\"></td>\n",
				  nrighe, nrighe);
		} else {
		  
		  // ALTRIMENTI STAMPO UNA BARRETTA NERA LUNGA QUANTO LE RIGHE INTERESSATE DAL CASE
		  fprintf(parametri->outFile, "<td rowspan=%d bgcolor=\"black\">.</td>\n", nrighe);
		}
	  }
	}
	
	// STAMPO IL TESTO IN UNA COLONNA DELLA TABELLA
	fprintf(parametri->outFile, "<td>");
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "</td>\n");
	
	if (primo) {
	  // SE CI SONO PARENTESI CHIUSE
	  if (strcmp(lexema, "\\Lcases") == 0 || strcmp(lexema, "\\RLcases") == 0 ||
		  strcmp(lexema, "\\Lbracecases") == 0 || strcmp(lexema, "\\RLbracecases") == 0) {
		// STAMPO LE GRAFFE SE E' SCELTA L'OPZIONE
		if (graffe) {
		  fprintf(parametri->outFile,"<td rowspan=%d><img src=\"micons/braces/aright%d.gif\"></td>\n",
				  nrighe, nrighe);
		  // ALTRIMENTI STAMPO UNA BARRETTA NERA LUNGA QUANTO LE RIGHE INTERESSATE DAL CASE
		} else {
		  fprintf(parametri->outFile, "<td rowspan=%d bgcolor=\"black\">.</td>\n", nrighe);
		}
	  }
	}
	
	fprintf(parametri->outFile, "</tr>\n");
	// STAMPO UN ALTRO BLOCCO DI TESTO
	printCasi(testo->testo2, parametri, sigle, false, lexema);
  }
}

// FUNZIONE DI STAMPA DEI CASES
void printCases(short key, struct testo * testo, PARAMETRI * parametri, struct testo * sigle){

	if (testo) {
		fprintf(parametri->outFile, "<table align=\"center\" cellpadding=\"0\" cellspacing=\"4\">\n");
		// STAMPO IL BLOCCO DI TESTO INTERESSATO DAI CASES
		printCasi(testo->testo2, parametri, sigle, true, testo->lexema);
		fprintf(parametri->outFile, "</table>\n");
	}
}

// STAMPA LA SIGLA DI UN TESTIMONE
void printSigla(const char * sigla, FILE * outFile) {

	char str[100], c;

	if (strlen(sigla) > 0) {
		fprintf(outFile, " ");
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
		case '9': // APICI
			sprintf(str, "%s<sup>%c</sup>", tmp, c);
			break;
		case '+': // CARATTERI NASCOSTI
			strcpy(str, tmp);
			break;
		default: // STANDARD
			strcpy(str,sigla);
		}
		delete tmp;

		// STAMPA DELLA SIGLA FORMATTATA
		printNota(str, outFile);
	}
}

// STAMPA IL RIFERIMENTO DA UNA LB
void printLabel(const char   * label,
		PARAMETRIm2h * parametri,
		struct testo * sigle,
		bool           duplex,
		bool           multiplex) {

	// RITROVA L'ETICHETTA
	rif * l = findLabel(label);

	if (!duplex && ! multiplex) {
		// STAMPA IL "TILDE"
		printNota(" &#732; ", parametri->outFile);
	}

	if (l) {

		// SE L'ETICHETTA ESISTE
		if (parametri->nUnit != l->nUnit) {

			// SE L'UNIT DELL'ETICHETTA E` DIVERSA DALLA UNIT CORRENTE
			printUnitRef(l->nUnit, parametri);
			fprintf(parametri->outFile, " ");
		}

		// SALVA I PARAMETRI
		bool banale  = parametri->banale;
		bool greco   = parametri->greco;
		bool justify = parametri->justify;

		// IMPOSTA I PARAMETRI PER L'ETICHETTA
		parametri->banale  = true;
		parametri->greco   = false;
		parametri->justify = true;

		// STAMPA L'ETICHETTA
		visitTesto(l->testo, parametri, sigle);

		// RIPRISTINA I PARAMETRI
		parametri->banale  = banale;
		parametri->greco   = greco;
		parametri->justify = justify;
	}
}

// STAMPA UN TESTO DI NOTA DELL'EDITORE
void printTestoNota(struct testo * testo,
		PARAMETRIm2h * parametri,
		struct testo * sigle) {
	printTagNota(parametri->outFile, true);
	visitTesto(testo, parametri, sigle);
	printTagNota(parametri->outFile, false);
}

// STAMPA IL CORPO DI UNA VARIANTE
bool printNotaVariante(struct testo * lezione,
		struct testo * nota,
		struct testo * sigle,
		PARAMETRIm2h * parametri,
		bool           duplex,
		bool           multiplex) {

	bool noNota = false; // INDICATORE DI NOTA VUOTA

	// SALVATAGGIO DEI PARAMETRI
	bool banale  = parametri->banale;
	bool greco   = parametri->greco;
	bool justify = parametri->justify;

	// IMPOSTAZIONE DEI PARAMETRI PER LA VARIANTE
	parametri->banale  = false;
	parametri->greco   = false;
	parametri->justify = true;

	// SE LA NOTA (SECONDO SOTTOCAMPO) ESISTE
	if (nota) {

		// VERIFICA DEL TIPO DI NOTA
		switch (nota->key) {
		case CRKEY:
			visitTesto(lezione, parametri, sigle);
			printLabel(nota->lexema, parametri, sigle, duplex, multiplex);
			noNota = true;
			break;
		case ANTEDELKEY:
			printNota("ante ", parametri->outFile);
			visitTesto(lezione, parametri, sigle);
			printNota(" del. ", parametri->outFile);
			visitTesto(nota->testo1, parametri, sigle);
			break;
		case POSTDELKEY:
			printNota("post ", parametri->outFile);
			visitTesto(lezione, parametri, sigle);
			printNota(" del. ", parametri->outFile);
			visitTesto(nota->testo1, parametri, sigle);
			break;
		case LECKEY:
			fprintf(parametri->outFile, " ");
			visitTesto(nota->testo1, parametri, sigle);
			break;
		case TRKEY:
		case EDKEY:
			visitTesto(lezione, parametri, sigle);
			fprintf(parametri->outFile, " ");
			printTestoNota(nota->testo1, parametri, sigle);
			break;
		case POSTSCRIPTKEY:
			visitTesto(lezione, parametri, sigle);
			break;
		case BISKEY:
			visitTesto(lezione, parametri, sigle);
			printNota(" bis", parametri->outFile);
			break;
		case INTERLKEY:
			visitTesto(lezione, parametri, sigle);
			printNota(" supra lineam", parametri->outFile);
			break;
		case SUPRAKEY:
			visitTesto(lezione, parametri, sigle);
			printNota(" supra lineam", parametri->outFile);
			break;
		case MARGKEY:
			visitTesto(lezione, parametri, sigle);
			printNota(" in marg.", parametri->outFile);
			break;
		case MARGSIGNKEY:
			visitTesto(lezione, parametri, sigle);
			printNota(" signo posito in marg.", parametri->outFile);
			break;
		case PCKEY:
			visitTesto(lezione, parametri, sigle);
			printNota(" post corr.", parametri->outFile);
			break;
		case EXKEY:
			visitTesto(lezione, parametri, sigle);
			printNota(" ex ", parametri->outFile);
			visitTesto(nota->testo1, parametri, sigle);
			break;
		case REPKEY:
			visitTesto(lezione, parametri, sigle);
			switch (*nota->lexema) {
			case '1':
				printNota(" priore loco", parametri->outFile);
				break;
			default:
				printNota(" altero loco", parametri->outFile);
				break;
			}
			break;
			case DESCOMPLKEY:
			case EDCOMPLKEY:
				printTestoNota(nota->testo1->testo1, parametri, sigle);
				fprintf(parametri->outFile, " ");
				visitTesto(lezione, parametri, sigle);
				fprintf(parametri->outFile, " ");
				printTestoNota(nota->testo1->testo2->testo1, parametri, sigle);
				fprintf(parametri->outFile, " ");
				visitTesto(nota->testo1->testo2->testo2, parametri, sigle);
				break;
			default:
				visitTesto(nota->testo1, parametri, sigle);
				break;
		}
	} else {
		noNota = true;
	}

	// RIPRISTINA I PARAMETRI
	parametri->banale  = banale;
	parametri->greco   = greco;
	parametri->justify = justify;

	// RITORNA L'INDICATORE DI NOTA VUOTA
	return noNota;
}

// STAMPA I POSTSCRIPT DI UNA VARIANTE
void printPostscript(struct testo * sigle, struct testo * nota, PARAMETRI * param) {

	PARAMETRIm2h * parametri = (PARAMETRIm2h *) param;

	//if (nota->key == POSTSCRIPTKEY) {
	/*
    fprintf(parametri->outFile, " ");
    printTestoNota(nota->testo1, parametri, sigle);
	 */
	printNotaVariante(NULL, nota, sigle, parametri, false, false);
	//}
}

bool printNoteVariante(struct testo * lezione,
		struct testo * note,
		struct testo * sigle,
		PARAMETRIm2h * parametri,
		bool           duplex,
		bool           multiplex) {

	bool noNota = true;

	if (note) {
		noNota = printNotaVariante(lezione, note->testo1, sigle, parametri, duplex, multiplex);
		if (note->testo1->key != POSTSCRIPTKEY) {
			noNota = printNoteVariante(NULL, note->testo2, sigle, parametri, duplex, multiplex) && noNota;
		}
	}

	return noNota;
}

// STAMPA IL CORPO DI UNA VARIANTE E RITORNA IL NUMERO DI TESTIMONI DI RIFERIMENTO
short printCorpoVariante(struct testo * lezione,
		struct testo * note,
		struct testo * sigle,
		PARAMETRI    * param,
		bool           duplex,
		bool           multiplex,
		bool           nested,
		int            nVar) {
	short i;
	bool noNota = true;
	short nTest;
	
	short test_1_len  = sizeof(test_1_L)/sizeof(test_1_L[0]); // corrisponde a MAXTEST
   short test_2_len  = sizeof(test_2_L)/sizeof(test_2_L[0]); // corrisponde a MAXTEST
   short t1l_si      = true;  // concordanze gruppo 1
   short t2l_si      = true;  // concordanze gruppo 2
   //short t1l_id[test_1_len];  // l'id di ciascuna lezione concorde nella variante
   //short t2l_id[test_2_len];  // l'id di ciascuna lezione concorde nella variante
   short t1l_no      = 0;     // conto concordanze gruppo 1
   short t2l_no      = 0;     // conto concordanze gruppo 2
   short t1ltot      = 0;     // testimoni totali gruppo 1
   short t2ltot      = 0;     // testimoni totali gruppo 2
   bool  _tllt_      = false; // flag per il controllo concordanze

	PARAMETRIm2h * parametri = (PARAMETRIm2h *) param;

	if (note) {
		noNota = printNoteVariante(lezione, note, sigle, parametri, duplex, multiplex);
	} else {
		if (parametri->math) {
			fprintf(parametri->outFile, "<em>");
		}
		visitTesto(lezione, parametri, sigle);
		if (parametri->math) {
			fprintf(parametri->outFile, "<em>");
		}
	}

	// STAMPA DELLE SIGLE
	nTest = visitSigle(sigle, noNota, parametri->outFile);
	
	// FILE DELLE CONCORDANZE
	if (testD1 != NULL && testD2 != NULL) {
	
	  // ITERO SUI GRUPPI DI TESTIMONI
	  // concordanze del gruppo 1
	  for (int tl=0; tl<test_1_len; tl++){ // testimoni gruppo 1 (anche null)
       if (test_1_L[tl] != NULL){ // almeno ne trovo uno
         t1ltot++; // aggiorno il numero di testimoni relativi a questa lezione
         // VERIFICA PRESENZA CONCORDANZE IN ERRORE
	      if (findTestRifNOMANUSALTERAE(sigle, test_1_L[tl])) { 
	        t1l_no++;
	      } else {t1l_si=false;}
	      //t1l_id[tl]=findTestIdNOMANUSALTERAE(sigle, test_1_L[tl]);
       }
     }  
     // concordanze del gruppo 2
     for (int lt=0; lt<test_2_len; lt++){ // testimoni gruppo 2 (anche null)
       if (test_2_L[lt] != NULL){ // almeno ne trovo uno
         t2ltot++;
         // VERIFICA PRESENZA CONCORDANZE IN ERRORE
         if (findTestRifNOMANUSALTERAE(sigle, test_2_L[lt])) {
	        t2l_no++;
	      } else {t2l_si=false;}
	      //t2l_id[lt]=findTestIdNOMANUSALTERAE(sigle, test_2_L[lt]);
       }
     }
     
     /*// controllo concordanza inter-gruppi
     if (test_1_len <= test_2_len) {
       for (int ii=0; ii<test_1_len; ii++) {
         if (t1l_id[ii] && t2l_id[ii]) {
           if (t1l_id[ii]==t2l_id[ii]) {
             _tllt_ = false;
             break;
           }
         }
       }
     } else { 
       for (int ii=0; ii<test_2_len; ii++) {
         if (t1l_id[ii] && t2l_id[ii]) {
           if (t1l_id[ii]==t2l_id[ii]) {
             _tllt_ = false;
             break;
           }
         }
       }
     }*/
     
     if ( t1l_si && t2l_no==0 ) {
       parametri->nCO1 = t1l_si;
       if ( parametri->nCO2 != 0 ) {
         _tllt_ = true;
       }
     }
     
     if ( t2l_si && t1l_no==0 ) {
       parametri->nCO2 = t2l_si;
       if ( parametri->nCO1 != 0 ) {
         _tllt_ = true;
       }
     }
	  
     // APERTURA DEL FILE DELLE CONCORDANZE
	  FILE * coFile = fopen(coFileName, "a");
	  if (coFile) {
       
       //CREO FILE CONCORDANZE
       if ( _tllt_ ) { // se i gruppi concordano all'interno e discordano tra loro
       
		   // INCREMENTO DEL NUMERO DELLE CONCORDANZE
	      parametri->nCO++;
	            
	        // INSERIMENTO DEL LINK ALLA NOTA NEL FILE DELLE CONCORDANZE
		     fprintf(coFile, 
		     "<a href=\"%s#VV%d\" target=\"variante\" nCo=\"%d\" t1x=\"%d\" t2x=\"%d\" nTest=\"%d\">%d</a>\n",
		     vvFileName,
		     parametri->nVV,
		     parametri->nCO,
		     t1l_no,
		     t2l_no,
		     nTest+1,
		     parametri->nVV);
		     
		     // AGGIORNAMENTO PARAMETRI AUSILIARI PER GRUPPI CONCORDANZE
		     parametri->nCO1 = 0;
		     parametri->nCO2 = 0;
       }
            
	  // CHIUSURA DEL FILE DELLE CONCORDANZE
	  fclose(coFile);
	  } else {
		 fprintf(stderr, "impossibile aprire il file %s\n", coFileName);
	  } 
	
	}
	
	// SE NON E` LA PRIMA VARIANTE (GLI ERRORI SONO NELLE ALTRE VARIANTI)
	// NB: variante != lezione
	if (nVar != 1) {


	  
	  // RICERCA DEGLI ERRORI DEI TESTIMONI RICHIESTI
	  for (i = 0; i < nTestEr; i++) {
	  
	    // VERIFICA PRESENZA ERRORI
	    if (findTestRif(sigle, testEr[i])) {
	  
	      // APERTURA DEL FILE DEGLI ERRORI
	      FILE * erFile = fopen(erFileName[i], "a");
	      if (erFile) {
	    
	        // INSERIMENTO DEL LINK ALLA NOTA NEL FILE DEGLI ERRORI
	        fprintf(erFile, "<a href=\"%s#VV%d\" target=\"variante\">%d</a>\n",
	        vvFileName, parametri->nVV, parametri->nVV);
	      
	        // CHIUSURA DEL FILE DEGLI ERRORI
	        fclose(erFile);
	      } else {
	      fprintf(stderr, "impossibile aprire il file %s\n", erFileName[i]);
	      }
	    } 
	  }
	}

	// RICERCA DELLE LEZIONI SINGOLARI
	for (i = 0; i < nTestLs; i++) {

		// VERIFICA PRESENZA DEL TESTIMONE
		if (findTestRif(sigle, testLs[i])) {

			// VERIFICA SINGOLARITA DEL TESTIMONE
			if (unicTestRif(sigle, testLs[i])) {

				// APERTURA DEL FILE LEZIONI SINGOLARI
				FILE * lsFile = fopen(lsFileName[i], "a");

				if (lsFile) {

					// INSERIMENTO DEL LINK ALLA NOTA NEL FILE LEZIONI SINGOLARI
					fprintf(lsFile, "<a href=\"%s#VV%d\" target=\"variante\">%d</a>\n",
							vvFileName, parametri->nVV, parametri->nVV);

					// CHIUSURA DEL FILE DELLE LEZIONI SINGOLARI
					fclose(lsFile);

				} else {
					fprintf(stderr, "impossibile aprire il file %s\n", lsFileName[i]);
				}
			}
		}
			
	}

	// STAMPA EVENTUALI POSTSCRIPT
	visitPostscript(sigle, note, parametri, false);

	// RITORNA IL NUMERO DI TESTIMONI
	return nTest;
}


void printOptions(struct testo * testo, FILE * outFile) {
	if (testo) {
		fprintf(outFile, "%s ", testo->lexema);
		printOptions(testo->testo2, outFile);
	}
}

// FUNZIONE DI STAMPA DEL PROLOGO HTML
void printPrologo(FILE * htmlFile) {
	fprintf(htmlFile, "<html>\n");
	fprintf(htmlFile, "<head>\n");
	fprintf(htmlFile, "</head>\n");
	fprintf(htmlFile, "<body bgcolor=\"white\">\n\n");
	fprintf(htmlFile, "<p align=\"justify\">\n");
}

// FUNZIONE DI STAMPA DELL'EPILOGO HTML
void printFine(FILE * htmlFile) {
	fprintf(htmlFile, "\n</p>");
	fprintf(htmlFile, "\n\n</body>");
	fprintf(htmlFile, "\n</html>\n");
}

// FUNZIONE DI STAMPA DEI FOLIA
void printFolium(const char * testimone, const char * pagina, PARAMETRI* parametri, struct testo *sigle) {
	fprintf(parametri->outFile, "<font size=\"-1\" color=green>[");
	if (testimone != NULL) {
	  fprintf(parametri->outFile, "%s:", testimone);
	}
	fprintf(parametri->outFile, "%s", pagina);
	fprintf(parametri->outFile, "]</font>");
}

// INIZIALIZZAZIONE DI UNA PAGINA
void nuovaPagina(char         * fileName,
				 PARAMETRIm2h * parametri,
				 struct testo * titolo) {

  FILE *vvFile;
  FILE *adFile;
  FILE *commFile;
  FILE *citFile;
  FILE *outFile;
  short i;
	
  short test_1_len = sizeof(test_1_L)/sizeof(test_1_L[0]); // corrisponde a MAXTEST
  short test_2_len = sizeof(test_2_L)/sizeof(test_2_L[0]); // corrisponde a MAXTEST
  short t1l_si     = 0; // il numero effettivo di testimoni
  short t2l_si     = 0; // calcolo dopo

  // INCREMENTO NUMERO DI PAGINA
  parametri->nPag++;
  if (titolo == NULL) {
	parametri->idPag++;
  }

  // ALLOCAZIONE DELLO SPAZIO PER I NOMI
  if (parametri->nPag == 1) {
	htmlFileName  = new char[strlen(fileName)+10];
	vvFileName    = new char[strlen(fileName)+strlen(VVEST)+10];
	citFileName   = new char[strlen(fileName)+strlen(CITEST)+10];
	adFileName    = new char[strlen(fileName)+strlen(ADNOTEST)+10];
	commFileName  = new char[strlen(fileName)+strlen(COMMEST)+10];
	frameFileName = new char[strlen(fileName)+strlen(FRAMEEST)+10];
  }

  // GENERAZIONE DEL NOME DEI FILE DI USCITA
  if (opzioneTestEx() == NULL) {
	sprintf(htmlFileName, "%s-%d%s", fileName, parametri->nPag, HTMLEST);
	sprintf(vvFileName, "%s-%d%s", fileName, parametri->nPag, VVEST);
  } else {
	sprintf(htmlFileName, "%s-%s-%d%s",
			fileName, opzioneTestEx(), parametri->nPag, HTMLEST);
	sprintf(vvFileName, "%s-%s-%d%s",
			fileName, opzioneTestEx(), parametri->nPag, VVEST);
  }
  sprintf(adFileName, "%s-%d%s", fileName, parametri->nPag, ADNOTEST);
  sprintf(commFileName, "%s-%d%s", fileName, parametri->nPag, COMMEST);
  sprintf(citFileName, "%s-%d%s", fileName, parametri->nPag, CITEST);
  sprintf(frameFileName, "%s-%d%s", fileName, parametri->nPag, FRAMEEST);

  // GENERAZIONE DEL LINK DELLA PAGINA INDICE
  if (titolo == NULL) {
	fprintf(indFile, "<a href=\"%s\" target=\"_parent\">%d</a><br>\n",
			frameFileName, parametri->idPag);
  } else {
	fprintf(indFile, "<a href=\"%s\" target=\"_parent\">", frameFileName);
	outFile = parametri->outFile;
	parametri->outFile = indFile;
	visitTesto(titolo, parametri, NULL);
	parametri->outFile = outFile;
	fprintf(indFile, "</a><br>\n");
  }

  // IMPOSTAZIONE FILE DI OUTPUT
  parametri->setFileName(htmlFileName);
  printPrologo(parametri->outFile);

  if (!livello) {

	// FILE DELLE CONCORDANZE
	if (testD1 != NULL && testD2 != NULL) {

	  coFileName = new char[strlen(fileName)+strlen(COEST)+2*MAXTEST+4]; // uri output
	  coT1DLName = new char[2*MAXTEST]; // gruppo 1 di testimoni per la stampa a video
	  coT2DLName = new char[2*MAXTEST]; // gruppo 2 di testimoni per la stampa a video
	  strcpy(coFileName, fileName); // uri file --(in)--> uri output
	  strcat(coFileName, "-"); // separatore uri-gruppoTestimoni1
	  strcat(coFileName, test_1_L[0]); // primo testimone gruppo 1 (deve esserci)
	  strcat(coT1DLName, test_1_L[0]);
     for (int i=1; i<test_1_len; i++){ // altri testimoni gruppo 1
      if (test_1_L[i] != NULL){ // se ci sono
       strcat(coFileName, test_1_L[i]); // altro testimone del gruppo 1 in uri output
       strcat(coT1DLName, ", ");
       strcat(coT1DLName, test_1_L[i]);
       t1l_si++;} // enumero
     }
	  strcat(coFileName, "-"); // separatore gruppoTestimoni1-gruppoTestimoni2
	  strcat(coFileName, test_2_L[0]); // primo testimone gruppo 2 (deve esserci)
	  strcat(coT2DLName, test_2_L[0]);
     for (int i=1; i<test_2_len; i++){ // altri testimoni gruppo 2
      if (test_2_L[i] != NULL){ // se ci sono
       strcat(coFileName, test_2_L[i]); // altro testimone del gruppo 2 in uri output
       strcat(coT2DLName, ", ");
       strcat(coT2DLName, test_2_L[i]);
       t2l_si++;} // enumero
     }
     strcat(coFileName, "-"); // separatore gruppoTestimoni2-COEST
	  strcat(coFileName, COEST); // COEST in uri output
	  //puts(coFileName); // scrive in stout

	  // GENERAZIONE DEL FILE
	  if (parametri->nPag == 1) {
		FILE * coFile = fopen(coFileName, "w");
		if (coFile) {
		  //printPrologo(coFile);
		  fprintf(coFile, "<h2>Varianti di {%s, %s} in cui {%s} discorda da {%s}</h2>\n", coT1DLName, coT2DLName, coT1DLName, coT2DLName);
	  
		  fclose(coFile);
		} else {
		  fprintf(stderr, "impossibile aprire il file %s\n", coFileName);
		}
	  }
	}

	// FILE LEZIONI SINGOLE
	for (i = 0; i < nTestLs; i++) {

	  // GENERAZIONE DEL FILE
	  if (parametri->nPag == 1) {
		lsFileName[i] = new char[strlen(fileName)+strlen(LSEST)+strlen(testLs[i])+10];
	  }
	  sprintf(lsFileName[i], "%s-%d-%s-%s", fileName, parametri->nPag, testLs[i], LSEST);
	  FILE * lsFile = fopen(lsFileName[i], "w");

	  if (lsFile) {
		printPrologo(lsFile);
		fprintf(lsFile, "<h2>Lezioni singolari di %s</h2>\n", testLs[i]);
		fclose(lsFile);
	  } else {
		fprintf(stderr, "impossibile aprire il file %s\n", lsFileName[i]);
	  }
	}

	// FILE DEGLI ERRORI
	for (i = 0; i < nTestEr; i++) {

	  // GENERAZIONE DEL FILE
	  if (parametri->nPag == 1) {
		erFileName[i] = new char[strlen(fileName)+strlen(EREST)+strlen(testEr[i])+10];
	  }
	  sprintf(erFileName[i], "%s-%d-%s-%s", fileName, parametri->nPag, testEr[i], EREST);
	  FILE * erFile = fopen(erFileName[i], "w");

	  if (erFile) {
		printPrologo(erFile);
		fprintf(erFile, "<h2> %s vs. testo</h2>\n", testEr[i]);
		fclose(erFile);
	  } else {
		fprintf(stderr, "impossibile aprire il file %s\n", erFileName[i]);
	  }
	}

	// FILE DELLE OMISSIONI
	for (i = 0; i < nTestOm; i++) {

	  // GENERAZIONE DEL FILE
	  if (parametri->nPag == 1) {
		omFileName[i] = new char[strlen(fileName)+strlen(OMEST)+strlen(testOm[i])+10];
	  }
	  sprintf(omFileName[i], "%s-%d-%s-%s", fileName, parametri->nPag, testOm[i], OMEST);
	  FILE * omFile = fopen(omFileName[i], "w");

	  if (omFile) {
		printPrologo(omFile);
		fprintf(omFile, "<h2>Omissioni di %s</h2>\n", testOm[i]);
		fclose(omFile);
	  } else {
		fprintf(stderr, "impossibile aprire il file %s\n", omFileName[i]);
	  }
	}

	// GENERAZIONE DEL FILE DELLE NOTE
	vvFile = fopen(vvFileName, "w");
	if (vvFile) {
	  printPrologo(vvFile);
	  fclose(vvFile);
	} else {
	  fprintf(stderr, "impossibile aprire il file %s\n", vvFileName);
	}

	// GENERAZIONE DEL FILE DELLE ANNOTAZIONI
	adFile = fopen(adFileName, "w");
	if (adFile) {
	  printPrologo(adFile);
	  fclose(adFile);
	} else {
	  fprintf(stderr, "impossibile aprire il file %s\n", adFileName);
	}
	
	// GENERAZIONE DEL FILE DEI COMMENTI
	commFile = fopen(commFileName, "w");
	if (commFile) {
	  printPrologo(commFile);
	  fclose(commFile);
	} else {
	  fprintf(stderr, "impossibile aprire il file %s\n", commFileName);
	}

	// GENERAZIONE FILE CITAZIONI
	citFile = fopen(citFileName, "w");
	if (citFile) {
	  printPrologo(citFile);
	  fclose(citFile);
	} else {
	  fprintf(stderr, "impossibile aprire il file %s\n", citFileName);
	}

	// GENERAZIONE DEL FRAMESET
	FILE * frameFile = fopen(frameFileName, "w");
	if (frameFile) {

	  // FRAMESET PRINCIPALE
	  fprintf(frameFile, "<FRAMESET COLS=\"10%%, 55%%, 35%%\">\n");

	  // FRAME DELL INDICE
	  fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"ind\">\n", indFileName);

	  // CONTROLLO PRESENZA OPZIONI DI ANALISI
	  if (nTestLs > 0 || nTestEr > 0 || nTestOm > 0 || (testD1 != NULL && testD2 != NULL)) {

		// CI SONO OPZIONI DI ANALISI: FRAMESET CON 3 FRAME
		fprintf(frameFile, "<FRAMESET ROWS=\"60%%, 10%%, 30%%\">\n");

		// FRAME DEL TESTO CRITICO
		fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"tc\">\n", htmlFileName);

		// FRAME DELLE ANNOTAZIONI
		fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"adnotatio\">\n", adFileName);

		// CONTATORE DELLE OPZIONI DI ANALISI
		short n = nTestLs + nTestEr + nTestOm;

		if (testD1 != NULL && testD2 != NULL) {
		  n++;
		}

		// PREPARAZIONE DELLE RELATIVE COLONNE
		if (n > 1) {
		  fprintf(frameFile, "<FRAMESET COLS=\"");
		  for (i = 0; i < n; i++) {
			fprintf(frameFile, "%d%%", 100/n);
			if (i < n-1) {
			  fprintf(frameFile, ", ");
			}
		  }
		  fprintf(frameFile, "\">\n");
		}

		// FRAME DELLE LEZIONI SINGOLE
		for (i = 0; i < nTestLs; i++) {
		  fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"singolari%s\">\n",
				  lsFileName[i], testLs[i]);
		}

		// FRAME DEGLI ERRORI
		for (i = 0; i < nTestEr; i++) {
		  fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"errori%s\">\n",
				  erFileName[i], testEr[i]);
		}

		// FRAME DELLE OMISSIONI
		for (i = 0; i < nTestOm; i++) {
		  fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"omissioni%s\">\n",
				  omFileName[i], testOm[i]);
		}

		// FRAME DELLE CONCORDANZE IN ERRORE
		if (testD1 != NULL && testD2 != NULL) {
		  fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"concordanze\">\n", coFileName);
		}

		// FINE FRAMESET OPZIONI DI ANALISI
		if (n > 1) {
		  fprintf(frameFile, "</FRAMESET>\n");
		}

		// FINE FRAMESET SECONDARIO
		fprintf(frameFile, "</FRAMESET>\n");
	  } else {

		// NESSUNA OPZIONE DI ANALISI: FRAME DI TESTO CRITICO E ADNOTATIO
		fprintf(frameFile, "<FRAMESET ROWS=\"80%%, 20%%\">\n");
		fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"tc\">\n", htmlFileName);
		fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"adnotatio\">\n", adFileName);
		fprintf(frameFile, "</FRAMESET>");
	  }
	  
	  if (!commenti) {

	     // FRAMESET DI VARIANTI E CITAZIONI
	     fprintf(frameFile, "<FRAMESET ROWS=\"50%%, 50%%\">\n");
	     fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"variante\">\n", vvFileName);
	     fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"cit\">\n", citFileName);
	     fprintf(frameFile, "</FRAMESET>\n");
	  } else {
	  
	     // FRAMESET DI VARIANTI, CITAZIONI E COMMENTI
	     fprintf(frameFile, "<FRAMESET ROWS=\"50%%, 30%%, 20%%\">\n");
	     fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"variante\">\n", vvFileName);
	     fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"cit\">\n", citFileName);
	     fprintf(frameFile, "<FRAME SRC=\"%s\" NAME=\"commento\">\n", commFileName);
	     fprintf(frameFile, "</FRAMESET>\n");
	  }

	  // FINE FRAMESET PRINCIPALE
	  fprintf(frameFile, "</FRAMESET>\n");

	} else {
	  fprintf(stderr, "impossibile aprire il file %s\n", frameFileName);
	}
  }
}

void finePagina(PARAMETRIm2h * parametri) {
	FILE *vvFile;
	FILE *adFile;
	FILE *commFile;
	FILE *citFile;
	short i;
	if (!livello) {
		// STAMPA DELL'EPILOGO HTML
		printFine(parametri->outFile);

		// GENERAZIONE DELLA FINE DEL FILE
		for (i = 0; i < nTestLs; i++) {

			// APERTURA DEL FILE DELLE LEZIONI SINGOLE
			FILE * lsFile = fopen(lsFileName[i], "a");

			if (lsFile) {

				// STAMPA LA FINE DEL FILE
				printFine(lsFile);
				// CHIUSURA DEL FILE DELLE LEZIONI SINGOLE
				fclose(lsFile);
			} else {
				fprintf(stderr, "impossibile aprire il file %s\n", lsFileName[i]);
			}
		}

		// GENERAZIONE DELLA FINE DEL FILE
		for (i = 0; i < nTestEr; i++) {

			// APERTURA DEL FILE DEGLI ERRORI
			FILE * erFile = fopen(erFileName[i], "a");

			if (erFile) {

				// STAMPA LA FINE DEL FILE
				printFine(erFile);
				// CHIUSURA DEL FILE DEGLI ERRORI
				fclose(erFile);
			} else {
				fprintf(stderr, "impossibile aprire il file %s\n", erFileName[i]);
			}
		}

		// GENERAZIONE DELLA FINE DEL FILE
		for (i = 0; i < nTestOm; i++) {

			// APERTURA DEL FILE DELLE OMISSIONI
			FILE * omFile = fopen(omFileName[i], "a");

			if (omFile) {

				// STAMPA LA FINE DEL FILE
				printFine(omFile);
				// CHIUSURA DEL FILE DELLE OMISSIONI
				fclose(omFile);
			} else {
				fprintf(stderr, "impossibile aprire il file %s\n", omFileName[i]);
			}
		}

		// GENERAZIONE DELLA FINE DEL FILE
		vvFile = fopen(vvFileName, "a");

		if (vvFile) {
			// STAMPA LA FINE DEL FILE
			printFine(vvFile);
			// CHIUSURA DEL FILE DELLE NOTE
			fclose(vvFile);
		} else {
			fprintf(stderr, "impossibile aprire il file %s\n", vvFileName);
		}

		// GENERAZIONE DELLA FINE DEL FILE
		adFile = fopen(adFileName, "a");

		// STAMPA DELL'ANNOTAZIONE
		if (adFile) {

			// STAMPA LA FINE DEL FILE
			printFine(adFile);

			// CHIUSURA DEL FILE DELLE ANNOTAZIONI
			fclose(adFile);
		} else {
			fprintf(stderr, "impossibile aprire il file %s\n", adFileName);
		}
		
		// GENERAZIONE DELLA FINE DEL FILE
		commFile = fopen(commFileName, "a");

		// STAMPA DEL COMMENTO
		if (commFile) {

			// STAMPA LA FINE DEL FILE
			printFine(commFile);

			// CHIUSURA DEL FILE DEI COMMENTI
			fclose(commFile);
		} else {
			fprintf(stderr, "impossibile aprire il file %s\n", commFileName);
		}

		// GENERAZIONE DELLA FINE DEL FILE
		citFile = fopen(citFileName, "a");

		if (citFile) {
			// STAMPA LA FINE DEL FILE
			printFine(citFile);
			// CHIUSURA DEL FILE DELLE CITAZIONI
			fclose(citFile);
		} else {
			fprintf(stderr, "impossibile aprire il file %s\n", citFileName);
		}
	}
}

// TRASFORMA UNA VARIANTE BANALE IN UNA SERIA SE RICHIESTO
bool opzioneBanale() {
	return vbTovv;
}

// FUNZIONI GENERICHE DEFINITE IN MVISIT

// FUNZIONE DI STAMPA DELLE DATE
void printDate(struct testo *testo, PARAMETRI* param, struct testo *sigle) {
	visitTesto(testo->testo1, param, sigle);
}

void handleListEnv(PARAMETRI * parametri, const char * env, const char * item, bool inizio) {
  if (inizio) {
    fprintf(parametri->outFile, "<%s>\n", env);
  } else {
    fprintf(parametri->outFile, "</%s>\n", item);
    fprintf(parametri->outFile, "</%s>\n", env);
  }
}

// FUNZIONE DI STAMPA DELLE CITAZIONI
void printCit(struct testo *testo, PARAMETRI* param, struct testo *sigle){

	// RECUPERA I PARAMETRI
	PARAMETRIm2h * parametri = (PARAMETRIm2h *) param;

	short iConf   = -1;
	const char  * libro = NULL;
	const char  * prop  = NULL;

	if (!parametri->nota) {

		// INCREMENTO DEL NUMERO DI CITAZIONE
		parametri->nCIT++;

		if (!livello) {

			// INSERIMENTO DELL'ANCORA NEL FILE DEL TESTO CRITICO
			fprintf(parametri->outFile, "<a name=\"CIT%d\"></a>", parametri->nCIT);

			FILE * citFile = fopen(citFileName, "a");

			if (citFile) {

				// SALVATAGGIO PARAMETRI
				FILE * outFile = parametri->outFile;

				// FILE CITAZIONI COME FILE DI OUTPUT
				parametri->outFile = citFile;

				// INSERIMENTO DEL LINK AL TESTO CRITICO
				fprintf(citFile, "<a href=\"%s#CIT%d\" target=\"tc\">%d</a>&nbsp; ",
						htmlFileName, parametri->nCIT, parametri->nUnit);

				// INSERIMENTO DELL'ANCORA
				fprintf(citFile, "<a name=\"CIT%d\"></a>", parametri->nCIT);

				// STAMPA DELL'OPZIONE
				if (testo->lexema != NULL) {
					printNota(testo->lexema, parametri->outFile);
				}

				if (testo->testo2) {
					trovaRiferimento(testo->testo2->testo1, &iConf, &libro, &prop);

					if (iConf >= 0) {

						// STAMPA RIFERIMENTO DELLA CITAZIONE
						printTagNota(parametri->outFile, true);
						printRiferimento(parametri, iConf, libro, prop);
						printTagNota(parametri->outFile, false);

						// CONDIZIONE PER L'ESCLUSIONE DELLA CITAZIONE
						if(!exclude){

							// STAMPA LA CITAZIONE ALL'OPERA
							stampaCitazioneOpera(parametri, libro, prop);
						}

					} else {
						printTagNota(parametri->outFile, true);
						visitTesto(testo->testo2->testo1, parametri, sigle);
						printTagNota(parametri->outFile, false);
					}

					// COMMENTO DELLA CITAZIONE
					if (testo->testo2->testo2) {
						fprintf(parametri->outFile, " ");
						printTestoNota(testo->testo2->testo2, parametri, sigle);
					}
				}

				fprintf(citFile, "<br>\n");

				// RIPRISTINO DEI PARAMETRI
				parametri->outFile = outFile;

				// CHIUSURA DEL FILE DELLE CITAZIONI
				fclose(citFile);
			} else {
				fprintf(stderr, "impossibile aprire il file %s\n", citFileName);
			}
		}
	}

	if (iConf >= 0 && (!livello)) {

		// INSERIMENTO DEL LINK ALL'OPERA NEL FILE DEL TESTO CRITICO
		if (exclude) {
			fprintf(parametri->outFile, "<a href=\"%s#", getConfigurazione(iConf)->htmlOpeFileName);
			if (libro) {
				fprintf(parametri->outFile, "%s", libro);
			}
			if (prop) {
				fprintf(parametri->outFile, ".%s", prop);
			}
			fprintf(parametri->outFile, "\" target=\"opera\">");
			visitTesto(testo->testo1, parametri, sigle);
			fprintf(parametri->outFile, "</a>");
		} else {
			fprintf(parametri->outFile, "<a href=\"%s#CIT%d", citFileName, parametri->nCIT);
			fprintf(parametri->outFile, "\" target=\"cit\">");
			visitTesto(testo->testo1, parametri, sigle);
			fprintf(parametri->outFile, "</a>");
		}
	} else {
		visitTesto(testo->testo1, parametri, sigle);
	}
}

// FUNZIONE DI STAMPA ITEM
void printItem(PARAMETRI * parametri) {
  switch (parametri->listEnv) {
  case ITEMIZE:
  case ENUMERATE:
	fprintf(parametri->outFile, "li");
	break;
  case DESCRIPTION:
	fprintf(parametri->outFile, "dd");
	break;
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
  PARAMETRIm2h * parametri = (PARAMETRIm2h *) param;
  
  char link[100];
  
  if (parametri->label == NULL || parametri->nota) {
	
	// IDENTIFICAZIONE DELLA MACRO
	switch (key) {
	default:
	  break;
	case COMMKEY:
	  if (inizio) { // se la key si riferisce alla graffa aperta
	    printf("Riga %ld, Comm %d, %s\n", ln,  parametri->nComm, testo->lexema);
	  }
	  visitTesto(testo, parametri, sigle);
	  break;
	case HTMLCUTKEY:
	  if (inizio) {
		printf("%s.tex: %ld - pagina: %d (%d)\n",
			   fileName, ln, parametri->nPag+1, parametri->idPag+1);
		if (parametri->outFile != NULL) {
		  finePagina(parametri);
		}
		nuovaPagina(fileName, parametri, testo);
	  }
	  break;
	case CLINEKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<hr width=\"90%%\" size=\"2\" noshade>");
	  }
	  break;
	case LEFTGRATABKEY:
	  if (inizio) {
		if (graffe) {
		  sprintf(link, "micons/braces/lbra%d.jpg", parametri->mRow);
		  printImage(link, "graffa aperta", parametri->outFile);
		} else {
		  fprintf(parametri->outFile, ".");
		}
	  }
	  break;
	case RIGHTGRATABKEY:
	  if (inizio) {
		if (graffe) {
		  sprintf(link, "micons/braces/rbra%d.jpg", parametri->mRow);
		  printImage(link, "graffa chiusa", parametri->outFile);
		} else {
		  fprintf(parametri->outFile, ".");
		}
	  }
	  break;
	case LEFTANGTABKEY:
	  if (inizio) {
		if (graffe) {
		  sprintf(link, "micons/braces/lang%d.jpg", parametri->mRow);
		  printImage(link, "angolare aperta", parametri->outFile);
		} else {
		  fprintf(parametri->outFile, ".");
		}
	  }
	  break;
	case RIGHTANGTABKEY:
	  if (inizio) {
		if (graffe) {
		  sprintf(link, "micons/braces/rang%d.jpg", parametri->mRow);
		  printImage(link, "angolare chiusa", parametri->outFile);
		} else {
		  fprintf(parametri->outFile, ".");
		}
	  }
	  break;
	case CONGKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<strong>[[</strong>");
	  } else {
		fprintf(parametri->outFile, "<strong>]]</strong>");
	  }
	  break;
	case PLKEY:
	  printTagNota(parametri->outFile, inizio);
	  break;
	case CLASSKEY:
	  /*
		if (inizio) {
		fprintf(parametri->outFile, "<!-- opzioni: ");
		printOptions(testo, parametri->outFile);
		} else {
		fprintf(parametri->outFile, " -->\n\n");
		}
	  */
	  break;
	case DOCENVKEY:
	  if (inizio) {
		//fprintf(parametri->outFile, "<!-- prologo:\n");
		//fprintf(stderr, "*** %ld\n", testo->testo1->key);
		visitTesto(testo->testo1, parametri, sigle);
		//fprintf(parametri->outFile, " -->\n\n");
		visitTesto(testo->testo2, parametri, sigle);
	  }
	  break;
	case USEPACKAGE:
	  /*if (inizio) {
		fprintf(parametri->outFile, "usepackage: ");
		if (testo->testo1) {
		fprintf(parametri->outFile, "[");
		printOptions(testo->testo1, parametri->outFile);
		fprintf(parametri->outFile, "]");
		}
		printOptions(testo->testo2, parametri->outFile);
		} else {
		fprintf(parametri->outFile, "\n");
		}*/
	  break;
	case GEOMETRY:
	  /*if (inizio) {
		fprintf(parametri->outFile, "geometry: ");
		printOptions(testo, parametri->outFile);
		} else {
		fprintf(parametri->outFile, "\n");
		}*/
	  break;
	case FOLIUMMARGOPT:
	  /*if (inizio) {
		fprintf(parametri->outFile, "foliumInMargine\n");
		}*/
	  break;
	case FOLIUMTESTOOPT:
	  /*if (inizio) {
		fprintf(parametri->outFile, "foliumInTesto\n");
		}*/
	  break;
	case UNITEMPTYKEY:
	  if (inizio) {
		printUnitRef(-1, parametri);
	  }
	  break;
	case UNITKEY:
	  if (inizio) {
		printUnitRef(parametri->nUnit, parametri);
	  }
	  break;
	case CAPITOLOKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<h2>");
	  } else {
		fprintf(parametri->outFile, "</h2>\n");
	  }
	  break;
	case SOTTOCAPITOLOKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<h3>");
	  } else {
		fprintf(parametri->outFile, "</h3>\n");
	  }
	  break;
	case TEOREMAKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<h2>");
	  } else {
		fprintf(parametri->outFile, "</h2>\n");
	  }
	  break;
	case ALITERKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<h3>");
	  } else {
		fprintf(parametri->outFile, "</h3>\n");
	  }
	  break;
	case LEMMAKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<h3>");
	  } else {
		fprintf(parametri->outFile, "</h3>\n");
	  }
	  break;
	case COROLLARIOKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<h3>");
	  } else {
		fprintf(parametri->outFile, "</h3>\n");
	  }
	  break;
	case SCOLIOKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<h3>");
	  } else {
		fprintf(parametri->outFile, "</h3>\n");
	  }
	  break;
	case ADDITIOKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<h3>");
	  } else {
		fprintf(parametri->outFile, "</h3>\n");
	  }
	  break;
	case SECTIONKEY:
	  if (inizio) {
	    fprintf(parametri->outFile, "<h2>");
	    if (secNumber) {
	      fprintf(parametri->outFile, "%d - ", parametri->nSec);
	    }
	  } else {
	    fprintf(parametri->outFile, "</h2>\n\n");
	  }
	  break;
	case SUBSECTIONKEY:
	  if (inizio) {
	    fprintf(parametri->outFile, "<h3>");
	    if (secNumber) {
	      fprintf(parametri->outFile, "%d.%d - ", parametri->nSec, parametri->nSubsec);
	    }
	  } else {
	    fprintf(parametri->outFile, "</h3>\n\n");
	  }
	  break;
	case SSSECTIONKEY:
	  if (inizio) {
	    fprintf(parametri->outFile, "<h4>");
	    if (secNumber) {
	      fprintf(parametri->outFile, "%d.%d.%d - ",
		      parametri->nSec, parametri->nSubsec, parametri->nSSubsec);
	    }
	  } else {
	    fprintf(parametri->outFile, "</h4>\n\n");
	  }
	  break;
	case PARAGRAPHKEY:
	  if (inizio) {
	    fprintf(parametri->outFile, "<h5>");
	  } else {
	    fprintf(parametri->outFile, "</h5>\n\n");
	  }
	  break;
	case LABELKEY:
	  if (inizio) {
	    fprintf(parametri->outFile, "<a name=\"%s\"></a>", testo->lexema);
	  }
	  break;
	case CITEKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<a href=\"./instrumenta/bibliographica.htm#%s\">%s</a>",
				testo->lexema, testo->lexema);
	  }
	  break;
	case REFKEY:
	  if (inizio) {
		const char * elk = findExternalLabel(testo->lexema);
		
		if (elk == NULL) {
		  labelKey * lk = findLabelKey(testo->lexema);
		  if (lk == NULL) {
			fprintf(stderr, "%s.tex: %ld - riferimento <%s> non trovato\n",
					fileName, ln, testo->lexema);
			fprintf(parametri->outFile,"??");
		  } else {
			fprintf(parametri->outFile, "<a href=\"#%s\">%d", testo->lexema, lk->sect);
			if (lk->ssect != 0) {
			  fprintf(parametri->outFile, ".%d", lk->ssect);
			}
			if (lk->sssect != 0) {
			  fprintf(parametri->outFile, ".%d", lk->sssect);
			}
			fprintf(parametri->outFile, "</a>");
		  }
		} else {
		  fprintf(parametri->outFile, "[%s]", testo->lexema);
		}
	  }
	  break;
	case ':':
	  if (inizio) {
		fprintf(parametri->outFile, "%c", ':');
	  }
	  break;
	case OMKEY:
	  if (inizio) {
		printOM("om.", parametri, sigle);
	  }
	  break;
	case DELKEY:
	  if (inizio) {
		printOM("del.", parametri, sigle);
	  }
	  break;
	case EXPKEY:
	  if (inizio) {
		printOM("exp.", parametri, sigle);
	  }
	  break;
	case OMLACKEY:
	  if (inizio) {
		printOM("spatio relicto om.", parametri, sigle);
	  }
	  break;
	case NLKEY:
	  if (inizio) {
		printNota("non legitur", parametri->outFile);
	  }
	  break;
	case LACMKEY:
	case LACSKEY:
	case LACCKEY:
	  if (inizio) {
		if (!parametri->nota) {
		  fprintf(parametri->outFile, "***");
		}
	  }
	  break;
	case BEGINEQNKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "</p>\n\n<center><em>");
	  } else {
		fprintf(parametri->outFile, "</em></center>\n\n<p align=\"justify\">");
	  }
	  break;
	case BEGINMATHKEY:
	case SLANTED:
	case EMPHASIS:
	case ITALIC:
	case CORRKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<em>");
	  } else {
		fprintf(parametri->outFile, "</em>");
	  }
	  break;
	case TITKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<span class=\"tit\">");
	  } else {
		fprintf(parametri->outFile, "</span>");
	  }
	  break;
	case PERSKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "[");
	  } else {
				fprintf(parametri->outFile, "]");
	  }
	  break;
	case STRIKEKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<strike>");
	  } else {
		fprintf(parametri->outFile, "</strike>");
	  }
	  break;
	case SUPKEY:
	case POWERKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<sup>");
	  } else {
		fprintf(parametri->outFile, "</sup>");
	  }
	  break;
	case SUBKEY:
	case INDEXKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<sub>");
	  } else {
		fprintf(parametri->outFile, "</sub>");
	  }
	  break;
	case TILDEKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"&nbsp;");
	  }
	  break;
	case DOTSKEY:
	  if (inizio) {
	    fprintf(parametri->outFile,"...");
	  }
	  break;
	case TIMESKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"&times;");
	  }
	  break;
	case ITEMKEY:
	  if (inizio) {
	    if (parametri->listEnv == 0) {
	      fprintf(stderr, "item fuori ambiente alla riga %ld\n", ln);
	    } else {
	      if (parametri->itemOpen) {
		fprintf(parametri->outFile,"</");
		printItem(parametri);
		fprintf(parametri->outFile,">\n");
	      }
	      if (parametri->listEnv == DESCRIPTION && testo != NULL) {
		fprintf(parametri->outFile,"<dt>");
		visitTesto(testo, parametri, sigle);
		fprintf(parametri->outFile,"</dt>");
	      }
	      fprintf(parametri->outFile,"<");
	      printItem(parametri);
	      fprintf(parametri->outFile,">");
	    }
	  }
	  break;
	case DSKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"//");
	  }
	  break;
	case SSKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"/");
	  }
	  break;
	case NBSPACEKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"&nbsp;");
	  }
	  break;
	case FLATKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"&#9837;");
	  }
	  break;
	case NATURALKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"&#9838;");
	  }
	  break;
	case CDOTKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"&middot;");
	  }
	  break;
	case VERTKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"|");
	  }
	  break;
	case LBRACKETKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"[");
	  }
	  break;
	case RBRACKETKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"]");
	  }
	  break;
	case SKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"&#167;");
	  }
	  break;
	case SLASH:
	  if (inizio) {
		fprintf(parametri->outFile, "/");
	  }
	  break;
	case GENERICENV:
	  if (inizio) {
		fprintf(parametri->outFile, "inizio ");
	  } else {
		fprintf(parametri->outFile, "fine ");
	  }
	  fprintf(parametri->outFile, "%s<hr noshade>\n", testo->lexema);
	  break;
	case TITMARG:
	  if (inizio) {
		fprintf(parametri->outFile, "</p><p align=\"right\"><table bgcolor=\"mintcream\"><tr><td>");
	  } else {
		fprintf(parametri->outFile, "</td></tr></table></p><p%s>\n\n",
				(parametri->justify) ? " align=\"justify\"" : "");
	  }
	  break;
	case ENUNCIATIO:
	  if (inizio) {
		fprintf(parametri->outFile, "</p><p align=\"center\"><font size=+2>");
	  } else {
		fprintf(parametri->outFile, "</font></p><p%s>\n\n",
				(parametri->justify) ? " align=\"justify\"" : "");
	  }
	  break;
	case QUOTE:
	  if (inizio) {
	    fprintf(parametri->outFile, "<blockquote>");
	  } else {
	    fprintf(parametri->outFile, "</blockquote>");
	  }
	  break;
	case ITEMIZE:
	  handleListEnv(parametri, "ul", "li", inizio);
	  break;
	case ENUMERATE:
	  handleListEnv(parametri, "ol", "li", inizio);
	  break;
	case DESCRIPTION:
	  handleListEnv(parametri, "dl", "dd", inizio);
	  break;
	case PROTASIS:
	  if (inizio) {
	    fprintf(parametri->outFile, "<font size=\"+1\">");
	  } else {
	    fprintf(parametri->outFile, "</font>");
	  }
	  break;
	case CENTERKEY:
	  if (inizio) {
	    fprintf(parametri->outFile, "<center>");
	  } else {
	    fprintf(parametri->outFile, "</center>");
	  }
	  break;
	case PARAGRAPH:
	  if (inizio) {
		fprintf(parametri->outFile, "</p>\n\n<p%s>\n",
				(parametri->justify) ? " align=\"justify\"" : "");
	  }
	  break;
	case PARAGRAPHEND:
	  if (inizio) {
		fprintf(parametri->outFile, "</p>\n\n<p%s>\n",
				(parametri->justify) ? " align=\"justify\"" : "");
	  }
	  break;
	case PARAGRAPHSTART:
	  if (inizio) {
		fprintf(parametri->outFile, "</p>\n\n<p%s>\n",
				(parametri->justify) ? " align=\"justify\"" : "");
	  }
	  break;
	case TITLEKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<center><h1>\n");
	  } else {
		fprintf(parametri->outFile, "</h1></center>\n\n");
	  }
	  break;
	case NUMINTESTKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<center><h1>\n");
	  } else {
		fprintf(parametri->outFile, ".</h1></center>\n\n");
	  }
	  break;
	case AUTHORKEY:
	case TITLEDATEKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<center><h2>\n");
	  } else {
		fprintf(parametri->outFile, "</h2></center>\n\n");
	  }
	  break;
	case ROMAN:
	  if (inizio) {
		fprintf(parametri->outFile, "</em>");
	  } else {
		fprintf(parametri->outFile, "<em>");
	  }
	  break;
	case BOLDFACE:
	  if (inizio) {
		fprintf(parametri->outFile, "<strong>");
	  } else {
		fprintf(parametri->outFile, "</strong>");
	  }
	  break;
	case SMALLCAPSKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<span style='font-variant: small-caps'>");
	  } else {
		fprintf(parametri->outFile, "</span>");
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
		fprintf(parametri->outFile, " ");
	  }
	  break;
	case MAUROTEXKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<strong><em>MauroTeX</em></strong>");
	  }
	  break;
	case MTEXKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<strong><em>MTeX</em></strong>");
	  }
	  break;
	case PERKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<strike>P</strike>");
	  }
	  break;
	case CONKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<strike>9</strike>");
	  }
	  break;
	case TEXKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<strong><em>TeX</em></strong>");
	  }
	  break;
	case LATEXKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"<strong><em>LaTeX</em></strong>");
	  }
	  break;
	case LONGKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"&#8210;");
	  }
	  break;
	case ADKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"&#8212;");
	  }
	  break;
	case SLQUOTEKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"`");
	  }
	  break;
	case SRQUOTEKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"'");
	  }
	  break;
	case DLQUOTEKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"&ldquo;");
	  }
	  break;
	case DRQUOTEKEY:
	  if (inizio) {
		fprintf(parametri->outFile,"&rdquo;");
	  }
	  break;
	case VECTORKEY:
	case CONTRKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<strike>");
	  } else {
		fprintf(parametri->outFile, "</strike>");
	  }
	  break;
	case CRUXKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "&#8224;");
	  } else {
		fprintf(parametri->outFile, "&#8224;");
	  }
	  break;
	case EXPUKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "[");
	  } else {
		fprintf(parametri->outFile, "]");
	  }
	  break;
	case INTEKEY:
	case LEFTANG:
	  if (inizio) {
		fprintf(parametri->outFile, "&lt;");
	  } else {
		fprintf(parametri->outFile, "&gt;");
	  }
	  break;
	case CITMARGKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<span style='background-color:rgb(255,200,200)'>");
	  } else {
		fprintf(parametri->outFile, "</span>");
	  }
	  break;
	case CITMARGSIGNKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<span style='background-color:rgb(255,255,200)'>");
	  } else {
		fprintf(parametri->outFile, "</span>");
	  }
	  break;
	case UNDERLINE:
	  if (inizio) {
		fprintf(parametri->outFile, "<u>");
	  } else {
		fprintf(parametri->outFile, "</u>");
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
		if (!parametri->greco) {
		  fprintf(parametri->outFile, ")");
		}
	  }
	  break;
	case INTESTAKEY:
	  if (inizio) {
		fprintf(param->outFile, "\n<p align=\"center\">\n");
		if (param->mitIntest) {
		  visitTesto(param->mitIntest, param, sigle);
		}
		if (param->desIntest) {
		  fprintf(param->outFile, " a ");
		  visitTesto(param->desIntest, param, sigle);
		}
		fprintf(param->outFile, "\n</p>\n");
		fprintf(param->outFile, "\n<p align=\"center\">\n");
		visitTesto(param->lmiIntest, param, sigle);
		if (param->lmiIntest && param->datIntest) {
		  fprintf(param->outFile, ", ");
		}
		visitTesto(param->datIntest, param, sigle);
		fprintf(param->outFile, "\n</p>\n");
		fprintf(param->outFile, "\n<p align=\"justify\">\n");
		visitTesto(param->comIntest, param, sigle);
		fprintf(param->outFile, "\n</p>\n");
		param->mitIntest = NULL;
		param->desIntest = NULL;
		param->lmiIntest = NULL;
		param->datIntest = NULL;
		param->comIntest = NULL;
	  }
	  break;
	case DAGKEY:
	  if (inizio) {
		  fprintf(parametri->outFile, "{");
		  visitTesto(testo->testo2, parametri, sigle);
		  fprintf(parametri->outFile, "}");
	  }
	  break;
	case HREFKEY:
	  if (inizio) {
		if (testo->testo1) {
		  fprintf(parametri->outFile, "<a href=\"");
		  visitTesto(testo->testo1, parametri, sigle);
		  fprintf(parametri->outFile, "\">");
		  visitTesto(testo->testo2, parametri, sigle);
		  fprintf(parametri->outFile, "</a>");
		} else {
		  visitTesto(testo->testo2, parametri, sigle);
		}
	  }
	  break;
	case NOTAMACRO:
	case OUTRANGE:
	  if (inizio) {
		fprintf(parametri->outFile, "<font color=red>&iquest;</font>");
	  }
	  break;
	}
  }
}

// FUNZIONE DI STAMPA DI UNA FIGURA
void printFig(short key, struct testo * testo, PARAMETRI * parametri) {
	fprintf(parametri->outFile, "</p><center>\n");
	if (key == FIGURAKEY) {
		fprintf(parametri->outFile, "<a href=\"");
		visitTesto(testo->testo2, parametri, NULL);
		fprintf(parametri->outFile,".jpg\"><img vspace=10 border=0 src=\"");
		visitTesto(testo->testo2, parametri, NULL);
		fprintf(parametri->outFile, ".jpg\" alt=\"");
		visitTesto(testo->testo2, parametri, NULL);
		fprintf(parametri->outFile, "\"></a>\n");
	} else {
		fprintf(parametri->outFile,
				"<a href=\"%sf%d.jpg\"><img vspace=10 border=0 src=\"%sf%d.jpg\" alt=\"figura %d\"></a>\n",
				fileName, parametri->nFig, fileName, parametri->nFig, parametri->nFig);
	}

	// DIDASCALIA
	if (testo->testo1) {
		fprintf(parametri->outFile, "<br>\n");
		visitTesto(testo->testo1, parametri, NULL);
	}
	fprintf(parametri->outFile, "</center><p align=\"justify\">\n\n");
}

// FUNZIONE DI STAMPA DI UN RIFERIMENTO AD UNA UNIT
void printUnitRef(const char * opera, const char * etichetta, PARAMETRI * parametri) {

  short nUnit = findUnit(etichetta);
  printUnitRef(nUnit, parametri);
}

void printUnitRef(short nUnit, PARAMETRI * parametri) {

	char line[100];
	bool nonempty = (nUnit >= 0);

	if (nonempty) {
		fprintf(parametri->outFile, "<font size=\"-1\" color=\"maroon\"><strong>");
		fprintf(parametri->outFile, "<a name=\"UN%d\">\n", nUnit);
		fprintf(parametri->outFile, "%d", nUnit);
		fprintf(parametri->outFile, "</a>\n\n");

	}

	if (unitFileName != NULL) {

		// E` STATA RICHIESTA UNA NUMERAZIONE PARALLELA
		if (fgets(line, sizeof(line), unitFile) != NULL && strcmp(line,"unit\n") == 0) {

			// IL FILE DELLE UNIT PARALLELE NON E` FINITO CONTIENE UNA UNIT
			nUnitRef++;
			if (nonempty) {
				fprintf(parametri->outFile, " (%d)", nUnitRef);
			}

		} else {

			// IL FILE DELLE UNIT E` FINITO O CONTIENE UNA UNIT VUOTA
			fprintf(parametri->outFile, " (&minus;)");

		}
	}

	if (nonempty) {
		fprintf(parametri->outFile, "</strong></font>");
	}

}

// FUNZIONE DI STAMPA DI UNA PAROLA
void printLexema(short key, const char * lexema, PARAMETRI * param) {

	PARAMETRIm2h * parametri = (PARAMETRIm2h *) param;

	if (lexema != NULL) {

	  // fprintf(stdout, "<%s>\n", lexema);

		// CONTROLLO INIBIZIONE DELLA STAMPA
		if (parametri->label == NULL || parametri->nota) {

			switch (key) {
			
			case TEXTCIRCLEDKEY:
		      printCircled(lexema, parametri->outFile);
		      break;

			// DOCUMENTCLASS
			case CLASSKEY:
				// fprintf(parametri->outFile, "<!-- classe: %s -->\n", lexema);
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
			case TILDE:
				printAccento(lexema, "tilde", parametri->outFile);
				break;
			case CIRC:
				printAccento(lexema, "circ", parametri->outFile);
				break;

			case PARSKIP:
			  //fprintf(parametri->outFile, "parskip=%s\n", lexema);
				break;
			case PARINDENT:
			  //fprintf(parametri->outFile, "parindent=%s\n", lexema);
				break;

				// SIMBOLI
			case SYMBOL:
				printSymbol(lexema, parametri->outFile);
				break;

				// MACRO
			case MACRO:
				printMacro(lexema, parametri->outFile);
				break;

			case EXTERNALDOCUMENT:
			  if (!addExternalLabel(lexema)) {
			  	fprintf(stderr, " label <%s> gia` utilizzata\n", lexema);
			  }
			  break;

				// PAROLA GENERICA
			default:
				if (parametri->greco) {

					unsigned int i;

					// GESTIONE DEL GRECO
					for (i = 0; i < strlen(lexema); i++) {
						char c = tolower(lexema[i]);
						switch (c) {
						default:
							if ((c >= 'a') && (c <= 'z')) {
								fprintf(parametri->outFile,
										"<img src=\"micons/greek/%cgreek.gif\" align=\"middle\" alt=\"%s\">",
										c, greekChar[c-'a']);
							}
							break;

						case ' ':
							fprintf(parametri->outFile, " ");
							break;
						}
					}
				} else {

					// PAROLA LATINA
					fprintf(parametri->outFile, "%s", lexema);
				}
				break;
			}
		}
	}
}

// FUNZIONE DI STAMPA DEL FRONTESPIZIO
void printFrontespizio(struct testo * testo,
					   PARAMETRI * parametri,
					   struct testo * sigle) {
  fprintf(parametri->outFile, "\n\n<div class='frontespizio'>\n");
  fprintf(parametri->outFile, "<h1 class='volume-id'>");
  visitTesto(testo->testo1, parametri, sigle);
  fprintf(parametri->outFile, "</h1>\n");
  fprintf(parametri->outFile, "<h1 class='volume-title'>");
  visitTesto(testo->testo2->testo1, parametri, sigle);
  fprintf(parametri->outFile, "</h1>\n");
  printAuthors(testo->testo2->testo2, parametri, sigle);
  fprintf(parametri->outFile, "</div>\n\n");
}
void internalPrintAuthors(struct testo * testo,
						  PARAMETRI * parametri,
						  struct testo * sigle) {
  fprintf(parametri->outFile, "<em>");
  visitTesto(testo->testo1, parametri, sigle);
  fprintf(parametri->outFile, "</em>");
  if (testo->testo2 != NULL) {
	fprintf(parametri->outFile, ", ");
  	internalPrintAuthors(testo->testo2, parametri, sigle);
  }
}

void printAuthors(struct testo * testo,
				  PARAMETRI * parametri,
				  struct testo * sigle) {
  fprintf(parametri->outFile, "<p class='authors'>a cura di ");
  internalPrintAuthors(testo, parametri, sigle);
  fprintf(parametri->outFile, "</p>\n\n");
}

// FUNZIONE DI STAMPA DI UNA NOTA A PIEDE DI PAGINA
void printFootnote(struct testo * testo,
				   PARAMETRI * parametri,
				   struct testo * sigle) {
}

// FUNZIONE DI STAMPA DI UNA ANNOTAZIONE
void printAdnotatio(struct testo * testo,
		PARAMETRI * parametri,
		struct testo * sigle) {

	FILE *adFile, *outFile;

	if (!livello) {

		// ANCORA DELL'ANNOTAZIONE NEL TESTO CRITICO
		fprintf(parametri->outFile, "<a name=\"adnotatio%d\"></a>",
				parametri->nAdnot);

		// LINK ALL'ANNOTAZIONE
		fprintf
		(parametri->outFile,
				"<sup><a HREF=\"%s#adnotatio%d\" target=\"adnotatio\">[%d]</a></sup>",
				adFileName, parametri->nAdnot, parametri->nAdnot);

		// APERTURA DEL FILE DELLE ANNOTAZIONI
		adFile = fopen(adFileName, "a");

		if (adFile) {

			// ANCORA DELL'ANNOTAZIONE
			fprintf(adFile, "<a name=adnotatio%d>   </a>", parametri->nAdnot);

			// LINK AL TESTO CRITICO
			fprintf(adFile, "<a href=\"%s#adnotatio%d\" target=\"tc\"><sup>%d</sup></a>",
					htmlFileName, parametri->nAdnot, parametri->nAdnot);

			// SALVATAGGIO DEL RIFERIMENTO AL FILE DI OUTPUT
			outFile = parametri->outFile;

			// STAMPA NEL FILE DELLE ANNOTAZIONI
			parametri->outFile = adFile;
			visitTesto(testo, parametri, sigle);
			fprintf(adFile, "<br>\n\n");

			// CHIUSURA DEL FILE DELLE ANNOTAZIONI
			fclose(adFile);

			// RIPRISTINO DEL RIFERIMENTO AL FILE DI OUTPUT
			parametri->outFile = outFile;

		} else {
			fprintf(stderr, "impossibile aprire il file %s\n", adFileName);
		}
	}
}

// FUNZIONE DI STAMPA DI UN COMMENTO
void printCommento(struct testo * testo,
		PARAMETRI * parametri,
		struct testo * sigle) {

	FILE *commFile, *outFile;

	if (!livello && commenti) {

		// ANCORA DELL'ANNOTAZIONE NEL TESTO CRITICO
		fprintf(parametri->outFile, "<a name=\"commento%d\"></a>",
				parametri->nComm);

		// LINK ALL'ANNOTAZIONE
		fprintf
		(parametri->outFile,
				"<sup><a HREF=\"%s#commento%d\" target=\"commento\">&bull;%d</a></sup>",
				commFileName, parametri->nComm, parametri->nComm);

		// APERTURA DEL FILE DELLE ANNOTAZIONI
		commFile = fopen(commFileName, "a");

		if (commFile) {

			// ANCORA DELL'ANNOTAZIONE
			fprintf(commFile, "<a name=commento%d>   </a>", parametri->nComm);

			// LINK AL TESTO CRITICO
			fprintf(commFile, "<a href=\"%s#commento%d\" target=\"tc\"><sup>%d</sup></a>",
					htmlFileName, parametri->nComm, parametri->nComm);

			// SALVATAGGIO DEL RIFERIMENTO AL FILE DI OUTPUT
			outFile = parametri->outFile;

			// STAMPA NEL FILE DELLE ANNOTAZIONI
			parametri->outFile = commFile;
			visitTesto(testo, parametri, sigle);
			fprintf(commFile, "<br>\n\n");

			// CHIUSURA DEL FILE DELLE ANNOTAZIONI
			fclose(commFile);

			// RIPRISTINO DEL RIFERIMENTO AL FILE DI OUTPUT
			parametri->outFile = outFile;

		} else {
			fprintf(stderr, "impossibile aprire il file %s\n", commFileName);
		}
	}
}

// FUNZIONE DI MARCATURA DEL TESTO CRITICO
void printInizioTestoCritico(short key, PARAMETRI * parametri) {
}

void printFineTestoCritico(short key, PARAMETRI * parametri) {
}

// FUNZIONE DI MARCATURA DELLE VARIANTI
void printInizioVarianti(PARAMETRI * parametri) {
	if (!livello) {
		fprintf(parametri->outFile,"<a name=\"VV%d\"></a>", parametri->nVV);
		fprintf(parametri->outFile,
				"<sup><font size=\"2\"><a HREF=\"%s#VV%d\" target=\"variante\">%d</a></font></sup>",
				vvFileName, parametri->nVV, parametri->nVV);
	}
}

void printSepVarianti(PARAMETRI * parametri) {
	fprintf(parametri->outFile, "&nbsp;&nbsp;&nbsp;");
}

void printFineVarianti(PARAMETRI * parametri) {}

// FUNZIONE PER ABILITARE LA GESTIONE DELLA VV DI MVISIT
bool checkVV(short          key,
		struct testo * testo,
		PARAMETRI    * param,
		struct testo * sigle) {
	return true;
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

	// RECUPERO DEI PARAMETRI
	PARAMETRIm2h * parametri = (PARAMETRIm2h *) param;

	if (!livello) {

		// SALVATAGGIO PARAMETRI
		bool nota = parametri->nota;
		FILE * outFile = parametri->outFile;

		if (parametri->nota) {

			// STAMPA DI UNA SOTTONOTA
			fprintf(parametri->outFile, " (");
			visitVarianti(testo, parametri, sigle, true, false, false, 1);
			fprintf(parametri->outFile, ")");
		} else {

			// SE NON SI STA GIA` GESTENDO UNA NOTA

			// APERTURA DEL FILE DELLE NOTE
			FILE *vvFile;
			vvFile = fopen(vvFileName, "a");

			if (vvFile) {

				// STAMPA DEL LINK AL FILE PRINCIPALE
				fprintf(vvFile, " <a name=\"VV%d\"></a>", parametri->nVV);
				fprintf(vvFile, "<a href=\"%s#VV%d\" target=\"tc\"><sup>%d</sup></a>",
						htmlFileName, parametri->nVV, parametri->nVV);
				if (key == VBKEY) {
					fprintf(vvFile, " (<em>banale</em>) ");
				}

				// STAMPA DELLA NOTA
				parametri->outFile = vvFile;
				parametri->nota = true;
				if (key == NOTEKEY) {
					visitTesto(testo, parametri, sigle);
				} else {
					short nTest = visitVarianti(testo, parametri, sigle,
							false, false, false, 1);
					if (numtest) {
						fprintf(vvFile, "(n. testimoni: %d)", nTest);
					}
				}
				fprintf(vvFile, "<br>\n\n");

				// CHIUSURA DEL FILE DELLE NOTE
				fclose(vvFile);
			} else {
				fprintf(stderr, "impossibile aprire il file %s\n", vvFileName);
			}
		}

		// RIPRISTINO DEI PARAMETRI
		parametri->outFile = outFile;
		parametri->nota = nota;
	}
}

// FUNZIONE PER CREARE IL FILE HTML DELLE OPERE
void creaHTMLOpere () {
  
  short i;
  
  if (!livello) {
	
	// NUMERO DELLE CONFIGURAZIONI
	for (i = 0; i < getNConf(); i++) {
	  
	  // PRENDO LA CONFIGURAZIONE CHE INTERESSA
	  Configurazione * conf = getConfigurazione(i);
	  
	  // APERTURA DEL FILE TXT DELLE CITAZIONI DELL'OPERA
	  FILE * txtOpeFile = fopen(conf->getTxtOpeFileName(), "r");
	  
	  // SE IL FILE TXT DELLE CITAZIONI ESISTE
	  if (txtOpeFile) {
		
		// CONTATORE DELLA RIGA
		short nRiga = 0;
		
		// APERTURA DEL FILE HTML DELLE CITAZIONI DELL'OPERA
		FILE * htmlOpeFile = fopen(conf->getHtmlOpeFileName(), "w");
		
		// STAMPA DEI TAG HTML INIZIALI
		printPrologo(htmlOpeFile);
		
		// FINCHE' DURA IL FILE DELLE CITAZIONI
		while (!feof(txtOpeFile)) {
		  
		  char riga[1000];
		  
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
			  // STAMPO L'ANCORA ED IL RIFERIMENTO SUL FILE DELL'OPERA
			  fprintf(htmlOpeFile, "<a name=\"%s\"><font color=green><strong>%s</font></strong>", tok, tok);
			  
			  // MEMORIZZO LA PARTE SUCCESSIVA DELLA RIGA FINO A \t \n \r
			  tok = strtok(NULL, "\t\n\r");
			  
			  // SE ESISTE
			  if (tok) {
				
				// STAMPO IL TESTO E LA FINE DELL'ANCORA SUL FILE DELL'OPERA
				fprintf(htmlOpeFile, " - %s</a><br>\n", tok);
			  }
			}
		  }
		}
		
		// STAMPA DEI TAG HTML FINALI
		printFine(htmlOpeFile);
		
		// CHIUSURA DEL FILE HTML DELL'OPERA
		fclose(htmlOpeFile);
		
		// CHIUSURA DEL FILE DELLE CONFIGURAZIONI
		fclose(txtOpeFile);
		
		// SE IL FILE DELLE CITAZIONI DELL'OPERA
	  } else {
		if (verbose) {
		  fprintf(stderr, "impossibile leggere il file %s\n", conf->getTxtOpeFileName());
		}
	  }
	}
  }
}

// FUNZIONE DI STAMPA DELL'ALBERO SINTATTICO
void printDocumento(struct testo * testo) {

	PARAMETRIm2h * parametri;

	// SE NON E` STATA RICHIESTA SOLO L'ANALISI
	if (!analisi) {

		if (testo) {
			if (!livello) {

				// LETTURA DEL FILE DI CONFIGURAZIONE
				leggiConfigurazione(confFileName);

				// CREA IL FILE HTML DELLE OPERE CITATE
				creaHTMLOpere();
			}

			// APERTURA FILE UNIT DI RIFERIMENTO
			if (unitFileName != NULL) {
				unitFile = fopen(unitFileName, "r");
			}

			// GENERAZIONE DEI PARAMETRI
			parametri = new PARAMETRIm2h();

			// ALLOCAZIONE DELLO SPAZIO E CREAZIONE DEL NOME PER L'INDICE
			indFileName = new char[strlen(fileName)+strlen(INDEST)+9];
			sprintf(indFileName, "%s-%s", fileName, INDEST);

			// GENERAZIONE DEL FILE INDICE
			indFile = fopen(indFileName, "w");

			// GENERAZIONE DEL PROLOGO DEL FILE INDICE
			printPrologo(indFile);
			fprintf(indFile, "<center><font size=\"-1\">\n");

			// INIZIO GENERAZIONE CODICE
			printf("generazione del codice\n");

			// STAMPA DEL DOCUMENTO
			visitTesto(testo, parametri, NULL);

			// CHIUSURA ULTIMA PAGINA
			finePagina(parametri);
			// GENERAZIONE DELLA FINE FILE
			if (testD1 != NULL && testD2 != NULL) {
			  // APERTURA DEL FILE DELLE CONCORDANZE
			  FILE * coFile = fopen(coFileName, "a");
			  if (coFile) {
				// STAMPA LA FINE DEL FILE
				printFine(coFile);
				// CHIUSURA DEL FILE DELLE CONCORDANZE
				fclose(coFile);
			  } else {
				fprintf(stderr, "impossibile aprire il file %s\n", coFileName);
			  }
			}

			fprintf(indFile, "</font></center>\n");

			// GENERAZIONE DELL'EPILOGO E CHIUSURA DEL FILE DI INDICE
			printFine(indFile);

			// CHIUSURA FILE
			fclose(indFile);
			if (unitFileName != NULL) {
				fclose(unitFile);
			}

			// FINE GENERAZIONE CODICE
			printf("fine generazione del codice\n");

			// PULIZIA DELLA MEMORIA
			delete(parametri);
		}
	}
}

// PROGRAMMA PRINCIPALE
int main (int argn, char ** argv) {

	// INIZIALIZZAZIONE DELLE VARIABILI
	nTestLs         = 0;
	nTestOm         = 0;
	nTestEr         = 0;
	nUnitRef        = 0;
	testEx          = NULL;
	
	testD1          = NULL;
	testD2          = NULL;
	
	short test_1_len      = 0;
	short test_2_len      = 0;
	
	char *test_1_x          = NULL;
	char *test_2_x          = NULL;
	
	confFileName    = NULL;
	unitFileName    = NULL;
	char * fn       = NULL;
	bool ok         = true;
	bool help       = false;

	analisi         = false;
	commenti        = false;
	livello         = false;
	vbTovv          = false;
	graffe          = false;
	exclude         = false;
	verbose         = false;
	secNumber	= false;

	short i;

	for (i = 1; i < argn; i++) {

		// CONTROLLO DEGLI ARGOMENTI INSERITI
		switch (argv[i][0]) {

		case '-':

			// CONTROLLO DELLE OPZIONI
			switch (argv[i][1]) {

			case 'f':
				// FILE DI CONFIGURAZIONE
				i++;
				confFileName = argv[i];
				break;

			case 's':
			case 'S':
				// LEZIONI SINGOLARI
				i++;
				if (nTestLs < MAXTEST) {
					testLs[nTestLs] = argv[i];
					nTestLs++;
				} else {
					ok = false;
					fprintf
					(stderr,
							"Errore: i testimoni per le lezioni singolari sono piu` di %d\n",
							MAXTEST);
				}
				break;

			case 'o':
			case 'O':
				// OMISSIONI DI UN TESTIMONE
				i++;
				if (nTestOm < MAXTEST) {
					testOm[nTestOm] = argv[i];
					nTestOm++;
				} else {
					ok = false;
					fprintf
					(stderr,
							"Errore: i testimoni di riferimento per le omissioni sono piu` di %d\n",
							MAXTEST);
				}
				break;

			case 'e':
			case 'E':
				// ERRORI DI UN TESTIMONE
				i++;
				if (nTestEr < MAXTEST) {
					testEr[nTestEr] = argv[i];
					nTestEr++;
				} else {
					ok = false;
					fprintf
					(stderr,
							"Errore: i testimoni di riferimento per gli errori sono piu` di %d\n",
							MAXTEST);
				}
				break;

			case 'c':
			case 'C':
				// DISCORDANZE IN ERRORE
				i++; // il punto in cui sono nel leggere gli argv
				testD1 = argv[i]; // siamo al primo di due (gruppi di testimoni)
				if (strstr(testD1, coSEP) != NULL) { // SE trovo il separatore testimoni
			   	test_1_x = strtok(testD1,coSEP); // fino a che lo trovo prendo testimoni
				   while (test_1_x != NULL) {
				      test_1_L[test_1_len++]=test_1_x;
				      test_1_x = strtok(NULL, coSEP);
				   }
				} else {
				  test_1_L[test_1_len]=testD1; // ALTRIMENTI prendo l'unico che trovo
				}
				i++; // faccio lo stesso con il secondo gruppo
				testD2 = argv[i];
				if (strstr(testD2, coSEP) != NULL) {
			   	test_2_x = strtok(testD2,coSEP);
				   while (test_2_x != NULL) {
				      test_2_L[test_2_len++]=test_2_x;
				      test_2_x = strtok(NULL, coSEP);
				   }
				} else {
				  test_2_L[test_2_len]=testD2;
				}/*
				for (int i = 0; i < test_1_len; i++) {
				   fprintf(stderr, "%s\n", test_1_L[i]);
				}
				for (int i = 0; i < test_2_len; i++) {
				   fprintf(stderr, "%s\n", test_2_L[i]);
				}*/
				break;

			case 'b':
				// VARIANTI BANALI TRATTATE COME VARIANTI SERIE
				vbTovv = true;
				break;

			case 'p':
				// PARENTESI GRAFFE INVECE DI BARRETTE
				graffe = true;
				break;

			case 'h':
				// OPZIONI POSSIBILI
				help = true;
				break;

			case 'n':
				// NUMERO TESTIMONI
				numtest = true;
				break;

			case 't':
				// ESTRATTO DI UN TESTIMONE
				i++;
				testEx = argv[i];
				vbTovv = true;
				break;

			case 'u':
				// NUMERAZIONE UNIT CON RIFERIMENTO ESTERNO
				i++;
				unitFileName = argv[i];
				break;

			case 'v':
				// MESSAGGI DI WARNING
				verbose = true;
				break;

			case 'x':
				// ESCLUSIONE DELLA CITAZIONE
				exclude = true;
				break;

			case 'y':
				// NUMERAZIONE SEZIONI
				secNumber = true;
				break;

			case 'a':
				// ESCLUSIONE DEI FILE
				analisi = true;
				break;
				
			case 'z':
				// INCLUSIONE COMMMENTI
				commenti = true;
				break;

			case 'l':
				// LIVELLO DI OUTPUT
				switch (argv[i][2]) {
				case '0':
					analisi = true;
					break;

				case '1':
					livello = true;
					break;

				case '2':
					break;

				default:
					fprintf(stderr, "Errore: livello %s non gestito\n", argv[i]);
					break;

				}
			}

			// CONTROLLO NUMERO DI PARAMETRI
			if (i >= argn) {
				ok = false;
			}
			break;

			// NOME DEL FILE DA ANALIZZARE
			default:
				if (fn == NULL) {
					fn = argv[i];
				} else {
					ok = false;
				}
				break;
		}
	}

	// INCOMPATIBILITA' FRA OPZIONI RICHIESTE
	if (testEx != NULL && ( nTestEr > 0 || nTestOm > 0)) {
		ok = false;
		printf("L'estrazione di un testimone non e` compatibile con le opzioni di analisi\n");
	}

	// CONTROLLO ESISTENZA DI UN FILE
	if (fn == NULL) {
		ok = false;
	}

	// ANALISI DEL TESTO SE NON E' STATO RICHIESTO L'HELP
	if (ok && !help) {

		// INVOCAZIONE DELL'ANALIZZATORE SINTATTICO
		parser(fn);

	} else {
		printf("traduttore MauroTeX -> HTML (%s)\n", VERSIONSTRING);
		printf("uso: %s [-a] [-b] [-h] [-l0/1/2] [-n] [-p] [-v] [-x] [-y] [-z]\n", argv[0]);
		printf("\t[-f <file>]\n");
		printf("\t[-s <testimone>] [-S <testimone>]\n");
		printf("\t[-o <testimone>] [-O <testimone>]\n");
		printf("\t[-e <testimone>] [-E <testimone>]\n");
		printf("\t[-c <test_1_1>,<test_1_2>,... <test_2_1>,<test_2_2>,...] [-C <test_1_1>,<test_1_2>,... <test_2_1>,<test_2_2>,...]\n");
		printf("\t[-t <testimone>]\n");
		printf("\t<nome_file>\n");

		// OPZIONI POSSIBILI
		if (help) {
			printf("opzioni:\n");
			printf("  -a                   equivalente a -l0\n");
			printf("  -b                   considera le varianti banali come effettive\n");
			printf("  -c <test_1_1>,<test_1_2>,... <test_2_1>,<test_2_2>,... genera il file delle concordanze tra il primo gruppo di testimoni - dato dai <test_1_x>, ed il secondo - dato dai <test_2_x>. Le sigle di ciascun gruppo devono essere date senza spazi oppure raggruppate da \"\n");
			printf("                       contro il testo base\n");
			printf("  -e <testimone>       genera il file delle discordanze di <testimone>\n");
			printf("                       rispetto al testo base\n");
			printf("  -f <file>            usa <file> come file di configurazione\n");
			printf("  -h                   stampa questo messaggio\n");
			printf("  -l0                  livello solo analisi (non genera i file)\n");
			printf("  -l1                  livello testo critico (non genera gli altri file)\n");
			printf("  -l2                  livello completo (genera tutti i file - default)\n");
			printf("  -n                   stampa il numero di testimoni di ogni variante\n");
			printf("  -s <testimone>       genera il file delle lezioni singolari di <testimone>\n");
			printf("  -o <testimone>       genera il file delle omissioni di <testimone>\n");
			printf("  -p                   traduce le parentesi con immagini\n");
			printf("  -t <testimone>       genera il file del testo di <testimone>\n");
			printf("  -u <file>            usa <file> come file delle Unit di riferimento\n");
			printf("  -v                   stampa messaggi di avvertimento\n");
			printf("  -x                   esclude dal file delle citazioni la stampa delle\n");
			printf("                       proposizioni citate\n");
			printf("  -y                   numera le sezioni, sottosezioni, ...\n");
			printf("  -z                   stampa riga e numero dei commenti\n");
		}
	}

	// FINE DEL PROGRAMMA
	return 0;
}
