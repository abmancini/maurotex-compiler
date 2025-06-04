#define VERSIONSTRING "versione 1.13b18 del 30 luglio 2020"

/*

  file "m2web.c"
  PROGRAMMA PRINCIPALE

  m2web: traduttore da M-TeX a HTML basato mvisit

  eLabor scrl
  (ex. Laboratorio Informatico del Consorzio Sociale "Polis")
  via G. Garibaldi 33, 56124 Pisa - Italia
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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include "comp.h"

#include "mparse.h"
#include "mparsey.h"
#include "mvisit.h"

#define CONTENT_FILE     "contents.html"
#define ADNOTATIONS_FILE "annotations.html"
#define FOOTNOTES_FILE   "footnotes.html"
#define VARIANTS_FILE    "variants.html"
#define CITATIONS_FILE   "sources.html"
#define EDITION_FILE     "edition"
#define TOC_FILE         "toc.htm"
#define INDEX_FILE       "index.json"
#define TITLE_FILE       "titlepage.html"

#define MAXCONF 100
#define MAXTESTIMONI 10


// VARIABILI GLOBALI
static char * htmlFileName;
static char * vvFileName;
static char * confFileName;
static char * figExtension;
static char * adFileName;
static char * citFileName;
static char * edFileName;
static const char * pathName;
static const char * refFileName;
static const char * unitsFileName;
static bool vbTovv;          // VARIANTI BANALI COME EFFETTIVE
static bool graffe;          // PARENTESI GRAFFE INVECE DI BARRETTE
static bool verbose;         // STAMPA I MESSAGGI DI WARNING
static bool primaLabel;      // PER INIZIALIZZARE IL FILE JSON DELLE LABEL
static bool primaUnit;       // PER INIZIALIZZARE IL FILE JSON DELLE UNIT
static bool intro;           // INTRODUZIONE
static bool primaIntro;      // PER INIZIALIZZARE IL FILE DELLA INTRODUZIONE
static bool primoIndex;

static const char * labelkey;         // IDENTIFICATORE DELL'OPERA
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

static const char * lastTitolo;   // IDENTIFICATORE DELLA SECTION O SUBSECTION O SUBSUBSECTION

typedef struct Pagina {
  int           numero;
  const char  * titolo;
  Pagina      * next;
} Pagina;

const char * trim(const char * str) {
  int i;
  for(i = 0; str[i] == ' '; i++) {}
  return &str[i];
}

// STAMPA I CARATTERI SPECIALI
const char * getMacro(const char * lexema) {
  const char * macro = NULL;
  if (strcmp(lexema, "\\ss")         == 0) macro = "&szlig;";
  if (strcmp(lexema, "\\ae")         == 0) macro = "&aelig;";
  if (strcmp(lexema, "\\oe")         == 0) macro = "&#339;";
  if (strcmp(lexema, "\\OE")         == 0) macro = "&#338;";
  if (strcmp(lexema, "\\AE")         == 0) macro = "&AElig;";
  if (strcmp(lexema, "\\c e")        == 0) macro = "<strike>e</strike>";
  if (strcmp(lexema, "\\c p")        == 0) macro = "<strike>p</strike>";
  if (strcmp(lexema, "\\c c")        == 0) macro = "&ccedil;";
  if (strcmp(lexema, "\\c C")        == 0) macro = "&Ccedil;";
  if (strcmp(lexema, "\\&")          == 0) macro = "&amp;";
  if (strcmp(lexema, "\\it\\&")      == 0) macro = "<em><strong>&amp;</strong></em>";
  if (strcmp(lexema, "\\.o")         == 0) macro = "<strike>o</strike>";
  if (strcmp(lexema, "\\v o")        == 0) macro = "<strike>o</strike>";
  if (strcmp(lexema, "\\Alpha")      == 0) macro = "&Alpha;";
  if (strcmp(lexema, "\\Beta")       == 0) macro = "&Beta;";
  if (strcmp(lexema, "\\Gamma")      == 0) macro = "&Gamma;";
  if (strcmp(lexema, "\\Delta")      == 0) macro = "&Delta;";
  if (strcmp(lexema, "\\Epsilon")    == 0) macro = "&Epsilon;";
  if (strcmp(lexema, "\\Zeta")       == 0) macro = "&Zeta;";
  if (strcmp(lexema, "\\Eta")        == 0) macro = "&Eta;";
  if (strcmp(lexema, "\\Theta")      == 0) macro = "&Theta;";
  if (strcmp(lexema, "\\Iota")       == 0) macro = "&Iota;";
  if (strcmp(lexema, "\\Kappa")      == 0) macro = "&Kappa;";
  if (strcmp(lexema, "\\Lambda")     == 0) macro = "&Lambda;";
  if (strcmp(lexema, "\\Mu")         == 0) macro = "&Mu;";
  if (strcmp(lexema, "\\Nu")         == 0) macro = "&Nu;";
  if (strcmp(lexema, "\\Xi")         == 0) macro = "&Xi;";
  if (strcmp(lexema, "\\Omicron")    == 0) macro = "&Omicron;";
  if (strcmp(lexema, "\\Pi")         == 0) macro = "&Pi;";
  if (strcmp(lexema, "\\Rho")        == 0) macro = "&Rho;";
  if (strcmp(lexema, "\\Sigma")      == 0) macro = "&Sigma;";
  if (strcmp(lexema, "\\Tau")        == 0) macro = "&Tau;";
  if (strcmp(lexema, "\\Upsilon")    == 0) macro = "&Upsilon;";
  if (strcmp(lexema, "\\Phi")        == 0) macro = "&Phi;";
  if (strcmp(lexema, "\\Chi")        == 0) macro = "&Chi;";
  if (strcmp(lexema, "\\Psi")        == 0) macro = "&Psi;";
  if (strcmp(lexema, "\\Omega")      == 0) macro = "&Omega;";
  if (strcmp(lexema, "\\alpha")      == 0) macro = "&alpha;";
  if (strcmp(lexema, "\\beta")       == 0) macro = "&beta;";
  if (strcmp(lexema, "\\gamma")      == 0) macro = "&gamma;";
  if (strcmp(lexema, "\\delta")      == 0) macro = "&delta;";
  if (strcmp(lexema, "\\varepsilon") == 0) macro = "&epsilon;";
  if (strcmp(lexema, "\\epsilon")    == 0) macro = "&epsilon;";
  if (strcmp(lexema, "\\zeta")       == 0) macro = "&zeta;";
  if (strcmp(lexema, "\\eta")        == 0) macro = "&eta;";
  if (strcmp(lexema, "\\theta")      == 0) macro = "&theta;";
  if (strcmp(lexema, "\\iota")       == 0) macro = "&iota;";
  if (strcmp(lexema, "\\kappa")      == 0) macro = "&kappa;";
  if (strcmp(lexema, "\\lambda")     == 0) macro = "&lambda;";
  if (strcmp(lexema, "\\mu")         == 0) macro = "&mu;";
  if (strcmp(lexema, "\\nu")         == 0) macro = "&nu;";
  if (strcmp(lexema, "\\xi")         == 0) macro = "&xi;";
  if (strcmp(lexema, "\\omicron")    == 0) macro = "&omicron;";
  if (strcmp(lexema, "\\pi")         == 0) macro = "&pi;";
  if (strcmp(lexema, "\\rho")        == 0) macro = "&rho;";
  if (strcmp(lexema, "\\varsigma")   == 0) macro = "&sigmaf;";
  if (strcmp(lexema, "\\sigma")      == 0) macro = "&sigma;";
  if (strcmp(lexema, "\\tau")        == 0) macro = "&tau;";
  if (strcmp(lexema, "\\upsilon")    == 0) macro = "&upsilon;";
  if (strcmp(lexema, "\\varphi")     == 0) macro = "&phi;";
  if (strcmp(lexema, "\\phi")        == 0) macro = "&phi;";
  if (strcmp(lexema, "\\chi")        == 0) macro = "&chi;";
  if (strcmp(lexema, "\\psi")        == 0) macro = "&psi;";
  if (strcmp(lexema, "\\omega")      == 0) macro = "&omega;";
  return macro;
}

// FUNZIONE DI STAMPA DELLE MANI
void printManus(struct testo *testo, PARAMETRI* param, struct testo *sigle) {}

void printMacro(const char * lexema, FILE * outFile) {
  const char * macro = getMacro(lexema);
  if (macro != NULL) {
	fprintf(outFile, "%s", macro);
  }
}

char * accento(const char * tipo, char vocale) {
  char * t = new char[10];
  sprintf(t, "&%c%s;", vocale, tipo);
  return t;
}

const char * getTesto(struct testo * testo) {
  char * tit = new char[1];
  *tit = 0;
  while(testo != NULL) {
	int len = strlen(tit);
	char * t;
	switch (testo->key) {
	default:
	  {
		if (testo->lexema != NULL) {
		  const char * lexema;
		  if (testo->lexema[0] == '\\') {
			lexema = getMacro(testo->lexema);
		  } else {
			lexema = testo->lexema;
		  }
		  t = new char[len+strlen(lexema)+1];
		  sprintf(t, "%s%s", tit, lexema);
		  tit = t;
		  len = strlen(tit);
		}
		const char * titolo = getTesto(testo->testo1);
		t = new char[len+strlen(titolo)+1];
		sprintf(t, "%s%s", tit, titolo);
		tit = t;
	  }
	  break;
	case SPACES:
		t = new char[len+2];
		sprintf(t, "%s ", tit);
		tit = t;
	  break;
	case LEFTPAR:
		t = new char[len+2];
		sprintf(t, "%s(", tit);
		tit = t;
	  break;
	case RIGHTPAR:
		t = new char[len+2];
		sprintf(t, "%s)", tit);
		tit = t;
	  break;
	case GRAVE:
	  tit = accento("grave", testo->lexema[2]);
	  break;
	case ACUTE:
	  tit = accento("acute", testo->lexema[2]);
	  break;
	case CIRC:
	  tit = accento("circ", testo->lexema[2]);
	  break;
	case UML:
	  tit = accento("uml", testo->lexema[2]);
	  break;
	}
	testo = testo->testo2;
  }
  return tit;
}

Pagina * findPagina(int numero, Pagina * pagine) {
  
  Pagina * pag = pagine;
  Pagina * last = NULL;
  while (pag != NULL && pag->numero < numero) {
	last = pag;
	pag = pag->next;
  }
  return last;
}

bool addPagina (int numero,  Pagina ** pagine) {
  
  Pagina * found = findPagina(numero, *pagine);
  Pagina * next = (found == NULL) ? *pagine : found->next;
  bool inserito = (next == NULL || next->numero != numero);
  
  if (inserito) {
    Pagina * newPag = new Pagina;
    newPag->numero = numero;
    newPag->next  = next;
	newPag->titolo = trim(lastTitolo);
	if (found == NULL) {
	  *pagine = newPag;
	} else {
	  found->next = newPag;
	}
  }
 return inserito;
}

typedef struct Argomento {
  const char * titolo;
  Pagina     * pagine;
  Argomento  * next;
} Argomento;

Argomento * findArgomento(const char * titolo, Argomento * argomenti) {
  Argomento * arg = argomenti;
  Argomento * last = NULL;
  while (arg != NULL && strncmp(arg->titolo, titolo, strlen(arg->titolo)) < 0) {
	last = arg;
	arg = arg->next;
  }
  return last;
}

void addArgomento (const char * titolo, int pagina, Argomento ** argomenti) {

  Argomento * prev = findArgomento(titolo, *argomenti);
  Argomento * next = (prev == NULL) ? *argomenti : prev->next;
  bool found = (next != NULL && (strncmp(next->titolo, titolo, strlen(next->titolo)) == 0));

  if (found) {
	addPagina(pagina, &next->pagine);
  } else {
    Argomento * newArg = new Argomento;
	newArg->titolo = titolo;
	newArg->pagine = NULL;
	newArg->next = next;
	if (prev == NULL) {
	  *argomenti = newArg;
	} else {
	  prev->next = newArg;
	}
	addPagina(pagina, &newArg->pagine);
  }
}

typedef struct Proposizione {
  const char   * label;
  Pagina       * pagine;
  Argomento    * argomenti;
  Proposizione * next;
} Proposizione;

static Proposizione * lastProp = NULL;
static Proposizione * proposizioni = NULL;

Proposizione * findProposizione(const char * label) {
  
  Proposizione * prop = proposizioni;
  Proposizione * last = NULL;

  while (prop != NULL && strncmp(prop->label, label, strlen(prop->label)) < 0) {
	last = prop;
	prop = prop->next;
  }
  return last;
}

void addProposizione (const char * label, int pagina) {
  
  Proposizione * prev = findProposizione(label);
  Proposizione * next = (prev == NULL) ? proposizioni : prev->next;
  bool found = (next != NULL && (strncmp(next->label, label, strlen(next->label)) == 0));

  if (found) {
	addPagina(pagina, &next->pagine);
	lastProp = next;
  } else {
    Proposizione * newProp = new Proposizione;
    newProp->label = label;
    newProp->next  = next;
	newProp->pagine = NULL;
	newProp->argomenti = NULL;
	if (prev == NULL) {
	  proposizioni = newProp;
	} else {
	  prev->next = newProp;
	}
	addPagina(pagina, &newProp->pagine);
	lastProp = newProp;
  }
}

void handleArgument(testo * testo, int pagina) {
  const char * label = testo->lexema;
  if (label) {
    Proposizione * prev = findProposizione(label);
    Proposizione * next = (prev == NULL) ? proposizioni : prev->next;
    bool found = (next != NULL && (strncmp(next->label, label, strlen(next->label)) == 0));
    
    Proposizione * prop;
    struct testo * t;
    
    if (found) {
      prop = next;
      t = testo->testo2;
    } else {
      prop = lastProp;
      t = testo;
    }
    if (t && t->lexema[0] == '!') {
      t = t->testo2;
    }
    
    const char * titolo = getTesto(t);
    addArgomento(titolo, pagina, &prop->argomenti);
  }
}

typedef struct RiferimentoIncrociato {
  const char * opera;
  const char * ref;
  const char * title;
  RiferimentoIncrociato * next;
} RiferimentoIncrociato;

static RiferimentoIncrociato * riferimentiIncrociati;

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

RiferimentoIncrociato * leggiRiferimentiIncrociati(const char * fileName) {
  RiferimentoIncrociato * riferimenti = NULL;
  if (fileName != NULL) {
	FILE * file = fopen(fileName, "r");
	printf("analisi del file dei riferimenti incrociati (%s)\n", fileName);
	if (file) {
	  int nRiga = 0;
	  while (!feof(file)) {
		char riga[1000];
		char *r = fgets(riga, sizeof(riga), file);
		nRiga++;
		if (r != NULL && *riga != '#') {
		  char * opera;
		  char * ref;
		  char * title;
		  char * tok = strtok(riga, "\t\n\r");
		  if (tok) {
			opera = clone(tok);
			tok = strtok(NULL, "\t\n\r");
		  }
		  if (tok) {
			ref = clone(tok);
			tok = strtok(NULL, "\t\n\r");
		  }
		  if (tok) {
			title = clone(tok);
			RiferimentoIncrociato * riferimento = new RiferimentoIncrociato;
			riferimento->opera = opera;
			riferimento->ref = ref;
			riferimento->title = title;
			riferimento->next = riferimenti;
			riferimenti = riferimento;
		  } else {
			fprintf
			  (stderr,
			   "la riga %d non contiene tutti i campi richiesti (verificare l'uso degli spazi)\n",
			   nRiga);
		  }
		}
	  }
	  fclose(file);
	} else {
	  printf("il file %s non esiste\n", fileName);
	}
  }
  return riferimenti;
}

const char * findRiferimentoIncrociato(
									   RiferimentoIncrociato * riferimenti,
									   const char * label,
									   const char * ref,
									   int skip) {
  RiferimentoIncrociato * riferimento = riferimenti;
  while (
		 riferimento != NULL &&
		 (strcmp(label,riferimento->opera) != 0 || strcmp(ref, riferimento->ref) != 0)) {
	riferimento = riferimento->next;
  }
  const char * title;
  if (riferimento == NULL) {
	char * ph = new char[strlen(label)+strlen(ref)+1];
	strcpy(ph, label);
	strcat(ph, ref);
	title = ph;
  } else {
	title = &riferimento->title[skip];
  }
  return title;
}

typedef struct externalLabel {
  const char  * label;
  externalLabel * next;
} externalLabel;

static externalLabel * externalLabels = NULL;

const char * findExternalLabel(const char * label) {
  
  externalLabel * lab = externalLabels;
  
  while (lab != NULL && strncmp(lab->label, label, strlen(lab->label)) != 0) {
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
	short nCIT;
	bool  justify;
	short mRow, mCol;
	short nPag;

	PARAMETRIm2h(): PARAMETRI() {
		this->nER     = 0;
		this->nCO     = 0;
		this->nCIT    = 0;
		this->justify = true;
		this->mRow    = 1;
		this->mCol    = 1;
		this->nPag    = 0;
	}
};

class LEXEMA {
 public:
  char * sigla;
  char * pagina;
};

static LEXEMA * lexemi[MAXTESTIMONI];

LEXEMA * getLexema(const char * sigla) {
  int index = 0;
  while (index < MAXTESTIMONI && lexemi[index] != NULL && strcmp(lexemi[index]->sigla,sigla) != 0) {
	index++;
  }
  if (index >= MAXTESTIMONI) {
	fprintf(stderr, "\nERRORE: superato il numero massimo di testimoni\n\n");
	exit(1);
  }
  if (lexemi[index] == NULL) {
	lexemi[index] = new LEXEMA();
	lexemi[index]->sigla = new char[strlen(sigla)+1];
	strcpy(lexemi[index]->sigla, sigla);
  }
  return lexemi[index];
}

// FUNZIONI PER OPZIONI GESTITE ESTERNAMENTE
bool verboseWarning() {
  return verbose;
}

// FUNZIONI SPECIFICHE DEL TRADUTTORE HTML

char * getIndFileName(const char * sigla) {
  char * indFileName = new char[strlen(sigla) + strlen(pathName) + 20];
  sprintf(indFileName, "%s/%s-toImage.json", pathName, sigla);
  return indFileName;
}

char * getImageFileName(const char * sigla) {
  char * imageFileName = new char[strlen(sigla) + strlen(pathName) + 20];
  sprintf(imageFileName, "%s/%s-toPage.json", pathName, sigla);
  return imageFileName;
}

char * getLabelFileName() {
  char * labelFileName = new char[strlen(pathName) + 20];
  sprintf(labelFileName, "%s/referencemap.json", pathName);
  return labelFileName;
}

char * getUnitFileName() {
  char * unitFileName = new char[strlen(pathName) +20];
  sprintf(unitFileName, "%s/unitmap.json", pathName);
  return unitFileName;
}

void putUnitPage(const char * label, int nPag, int anchor) {

  const char * option = (primaUnit ? "w" : "a");
  char * unitName = getUnitFileName();
  FILE * unitFile = fopen(unitName, option);

  if (primaUnit) {
	fprintf(unitFile, "{\n");
	primaUnit = false;
  } else {
	fprintf(unitFile, ",\n");
  }
  fprintf(unitFile, "\t\"%s\": {\"page\":\"%d\",\"anchor\":\"UN%d\"}", label, nPag, anchor);
  fclose(unitFile);
}

void putLabelPage(const char * label, const char * anchor) {
  const char * option = (primaLabel ? "w" : "a");
  char * labelName = getLabelFileName();
  FILE * labelFile = fopen(labelName, option);

  if (primaLabel) {
	fprintf(labelFile, "{\n");
	primaLabel = false;
  } else {
	fprintf(labelFile, ",\n");
  }
  fprintf(labelFile, "\t\"%s\": {\"anchor\":\"SEC%s\"}", label, anchor);
  fclose(labelFile);
}

void putIndPage(const char * sigla, int nPag, const char * pagina) {
  const char * option = (nPag == 1 ? "w" : "a");
  char * indFileName = getIndFileName(sigla);
  FILE * indFile = fopen(indFileName, option);

  if (nPag == 1) {
	fprintf(indFile, "{\n");
  } else {
	fprintf(indFile, ",\n");
  }
  fprintf(indFile, "\t\"%d\":\"%s.jpg\"", nPag, pagina);
  fclose(indFile);
}


void putImagePage(const char * sigla, bool firstFolium, int nPag, const char * immagine) {
  char * imageFileName = getImageFileName(sigla);
  FILE * imageFile;
  if (firstFolium) {
	imageFile = fopen(imageFileName, "w");
	fprintf(imageFile, "{\n");
  } else {
	imageFile = fopen(imageFileName, "a");
	fprintf(imageFile, ",\n");
  }
  fprintf(imageFile, "\t\"%s.jpg\":\"%d\"", immagine, nPag);
  fclose(imageFile);
}

// RITORNA IL TESTIMONE PER VEDERE IL SUO TESTO SE IMPOSTATO
char * opzioneTestEx() {
	return NULL;
}

// INSERISCE IL TAG DI STILE DI UNA NOTA
void printTagNota(FILE * outFile, bool inizio) {
	if (inizio) {
	  //		fprintf(outFile, "<font color=green><em>");
	  fprintf(outFile, "<span class=\"editor\">");
	} else {
	  //fprintf(outFile, "</em></font>");
			fprintf(outFile, "</span>");
	}
}

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

	// DEFINIZIONE DELL'ATTRIBUTO DI ALLINEAMENTO ORIZZONTALE
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

	fprintf(parametri->outFile, "<td ");

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
		fprintf(parametri->outFile, "valign=\"top\" align=\"%s\"%s%s>\n", attr, cols, bgColor);

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
  fprintf(parametri->outFile, "<p class=\"title\">");
  visitTesto(testo->testo1, parametri, sigle);
  fprintf(parametri->outFile, "</p>");
	fprintf(parametri->outFile, "<p class=\"subtitle\">");
  if (testo->testo2 != NULL) {
	visitTesto(testo->testo2, parametri, sigle);
	}
	fprintf(parametri->outFile, "</p>");
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
	printNota(nota, parametri->outFile);
}

// STAMPA I LINK ALLE IMMAGINI
void printImage(const char * path, const char * alt, FILE * outFile) {
	fprintf(outFile, "<img src=\"%s\" alt=\"%s\" class=\"micons\">",
			path, alt);
}

// STAMPA I SIMBOLI MATEMATICI ED ASTRONOMICI
void printSymbol(const char * lexema, FILE * outFile) {
	if (strcmp(lexema, "\\RDX")  == 0)
		printImage("./micons/RDX.gif", "Radix", outFile);
	if (strcmp(lexema, "\\mtilde")  == 0)
		printImage("./micons/mtilde.gif", "mtilde", outFile);
	if (strcmp(lexema, "\\ptilde")  == 0)
		printImage("./micons/ptilde.gif", "ptilde", outFile);
	if (strcmp(lexema, "\\rdx")  == 0)
		printImage("./micons/rdxs.gif", "radix", outFile);
	if (strcmp(lexema, "\\SOL")  == 0)
		printImage("./micons/astro/SOL.gif", "Sole", outFile);
	if (strcmp(lexema, "\\LUN")  == 0)
		printImage("./micons/astro/LUN.gif", "Luna", outFile);
	if (strcmp(lexema, "\\TER")  == 0)
		printImage("./micons/astro/TER.gif", "Terra", outFile);
	if (strcmp(lexema, "\\MER")  == 0)
		printImage("./micons/astro/MER.gif", "Mercurio", outFile);
	if (strcmp(lexema, "\\VEN")  == 0)
		printImage("./micons/astro/VEN.gif", "Venere", outFile);
	if (strcmp(lexema, "\\MAR")  == 0)
		printImage("./micons/astro/MAR.gif", "Marte", outFile);
	if (strcmp(lexema, "\\GIO")  == 0)
		printImage("./micons/astro/GIO.gif", "Giove", outFile);
	if (strcmp(lexema, "\\SAT")  == 0)
		printImage("./micons/astro/SAT.gif", "Saturno", outFile);
	if (strcmp(lexema, "\\ARS")  == 0)
		printImage("./micons/astro/ARS.gif", "Ariete", outFile);
	if (strcmp(lexema, "\\TRS")  == 0)
		printImage("./micons/astro/TRS.gif", "Toro", outFile);
	if (strcmp(lexema, "\\GMN")  == 0)
		printImage("./micons/astro/GMN.gif", "Gemelli", outFile);
	if (strcmp(lexema, "\\CNC")  == 0)
		printImage("./micons/astro/CNC.gif", "Cancro", outFile);
	if (strcmp(lexema, "\\LEO")  == 0)
		printImage("./micons/astro/LEO.gif", "Leone", outFile);
	if (strcmp(lexema, "\\VRG")  == 0)
		printImage("./micons/astro/VRG.gif", "Vergine", outFile);
	if (strcmp(lexema, "\\LBR")  == 0)
		printImage("./micons/astro/LBR.gif", "Bilancia", outFile);
	if (strcmp(lexema, "\\SCR")  == 0)
		printImage("./micons/astro/SCR.gif", "Scorpione", outFile);
	if (strcmp(lexema, "\\SGT")  == 0)
		printImage("./micons/astro/SGT.gif", "Sagittario", outFile);
	if (strcmp(lexema, "\\CPR")  == 0)
		printImage("./micons/astro/CPR.gif", "Capricorno", outFile);
	if (strcmp(lexema, "\\AQR")  == 0)
		printImage("./micons/astro/AQR.gif", "Acquario", outFile);
	if (strcmp(lexema, "\\PSC")  == 0)
		printImage("./micons/astro/PSC.gif", "Pesci", outFile);
	if (strcmp(lexema, "\\PRL")  == 0 || strcmp(lexema, "\\PG") == 0)
		printImage("./micons/PRL.gif", "parallelogramma", outFile);
	if (strcmp(lexema, "\\TRN")  == 0 || strcmp(lexema, "\\T")  == 0)
		printImage("./micons/TRN.gif", "triangolo", outFile);
	if (strcmp(lexema, "\\QDR")  == 0 || strcmp(lexema, "\\Q")  == 0)
		printImage("./micons/QDR.gif", "quadrato", outFile);
	if (strcmp(lexema, "\\RTT")  == 0 || strcmp(lexema, "\\R")  == 0)
		printImage("./micons/RTT.gif", "rettangolo", outFile);
	if (strcmp(lexema, "\\DRTT")  == 0 || strcmp(lexema, "\\R")  == 0)
		printImage("./micons/DRTT.gif", "doppiorettangolo", outFile);
	if (strcmp(lexema, "\\CUB")  == 0)
		printImage("./micons/CUB.gif", "cubo", outFile);
	if (strcmp(lexema, "\\PYR")  == 0)
		printImage("./micons/PYR.gif", "piramide", outFile);
	if (strcmp(lexema, "\\PPD")  == 0)
		printImage("./micons/PPD.gif", "parallelepipedo", outFile);
	if (strcmp(lexema, "\\PEN")  == 0)
		printImage("./micons/PEN.gif", "pentagono", outFile);
	if (strcmp(lexema, "\\HEX")  == 0)
		printImage("./micons/HEX.gif", "esagono", outFile);
	if (strcmp(lexema, "\\HEXC")  == 0)
		printImage("./micons/HEXC.gif", "esagono centrale", outFile);
	if (strcmp(lexema, "\\TRP")  == 0 || strcmp(lexema, "\\TP") == 0)
		printImage("./micons/TRP.gif", "trapezio", outFile);
	if (strcmp(lexema, "\\CNJ")  == 0)
		printImage("./micons/astro/CNJ.gif", "congiunzione", outFile);
	if (strcmp(lexema, "\\OPP")  == 0)
		printImage("./micons/astro/OPP.gif", "opposizione", outFile);
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
		  fprintf(parametri->outFile, "<td rowspan=%d><img src=\"./micons/braces/aleft%d.gif\"></td>\n",
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
		  fprintf(parametri->outFile,"<td rowspan=%d><img src=\"./micons/braces/aright%d.gif\"></td>\n",
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
	printNota(" &#126; ", parametri->outFile);
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

void printTextGreek(struct testo * testo,
					PARAMETRI * parametri,
					struct testo * sigle) {
  visitTesto(testo, parametri, sigle);
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
	bool noNota = true;
	short nTest;

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

// FUNZIONE DI STAMPA DEI FOLIA
void printFolium(const char * testimone, const char * pagina, PARAMETRI* param, struct testo *sigle) {
  PARAMETRIm2h * parametri = (PARAMETRIm2h *) param;
  if (testimone == NULL) {
	fprintf(parametri->outFile, "<span class=\"folium\">[");
	fprintf(parametri->outFile, "%s", pagina);
	fprintf(parametri->outFile, "]</span>");
  } else {
	LEXEMA * lex = getLexema(testimone);
	bool firstFolium;
	if (lex->pagina == NULL) {
	  for (int nPag = 1; nPag <= parametri->nPag; nPag++) {
		putIndPage(testimone, nPag, pagina);
	  }
	  firstFolium = true;
	} else {
	  firstFolium = false;
	}
	putImagePage(testimone, firstFolium, parametri->nPag, pagina);
	lex->pagina = new char[strlen(pagina)+1];
	strcpy(lex->pagina, pagina);
	fprintf(parametri->outFile, "<span class=\"folium\"><a name=\"");
	fprintf(parametri->outFile, "%s:", testimone);
	fprintf(parametri->outFile, "%s", pagina);
	//visitTesto(testo->testo2, parametri, sigle);
	fprintf(parametri->outFile, "\" href=\"?folium=");
	fprintf(parametri->outFile, "%s:", testimone);
	fprintf(parametri->outFile, "%s", pagina);
	//visitTesto(testo->testo2, parametri, sigle);
	fprintf(parametri->outFile, "#");
	fprintf(parametri->outFile, "%s:", testimone);
	fprintf(parametri->outFile, "%s", pagina);
	//visitTesto(testo->testo2, parametri, sigle);
	fprintf(parametri->outFile, "\">[");
	fprintf(parametri->outFile, "%s:", testimone);
	fprintf(parametri->outFile, "%s", pagina);
	//visitTesto(testo->testo2, parametri, sigle);
	fprintf(parametri->outFile, "]</a></span>");
  }
}

// INIZIALIZZAZIONE DI UNA PAGINA
void nuovaPagina(char         * fileName,
				 PARAMETRIm2h * parametri,
				 struct testo * titolo) {

  FILE *vvFile;
  FILE *adFile;
  FILE *citFile;
  FILE *edFile;

  const char * tit = getTesto(titolo);

  // INCREMENTO NUMERO DI PAGINA
  parametri->nPag++;

  // ALLOCAZIONE DELLO SPAZIO PER I NOMI
  char * folderName = new char[strlen(tit) + strlen(pathName) + 10];
  edFileName    = new char[strlen(EDITION_FILE) + strlen(pathName) + strlen(tit) + 10];
  htmlFileName  = new char[strlen(CONTENT_FILE) + strlen(pathName) + strlen(tit) + 10];
  vvFileName    = new char[strlen(VARIANTS_FILE) + strlen(pathName) + strlen(tit) + 10];
  citFileName   = new char[strlen(CITATIONS_FILE) + strlen(pathName) + strlen(tit) + 10];
  adFileName    = new char[strlen(ADNOTATIONS_FILE) + strlen(pathName) + strlen(tit) + 10];

  // GENERAZIONE DEL NOME DEI FILE DI USCITA
  sprintf(folderName, "%s/%d.%s", pathName, parametri->nPag, tit);
  sprintf(htmlFileName, "%s/%s", folderName, CONTENT_FILE);
  sprintf(vvFileName,   "%s/%s", folderName, VARIANTS_FILE);
  sprintf(adFileName,   "%s/%s", folderName, ADNOTATIONS_FILE);
  sprintf(citFileName,  "%s/%s", folderName, CITATIONS_FILE);
  sprintf(edFileName,   "%s/%s", folderName, EDITION_FILE);

  // CREAZIONE CARTELLA
  mkdir(folderName, S_IRWXU | S_IRWXG | S_IRWXO);

  // IMPOSTAZIONE FILE DI OUTPUT
  parametri->setFileName(htmlFileName);

  // GENERAZIONE DEL FILE DELLE NOTE
  vvFile = fopen(vvFileName, "w");
  if (vvFile) {
	fclose(vvFile);
  } else {
	fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", vvFileName);
  }

  // GENERAZIONE DEL FILE DELLE ANNOTAZIONI
  adFile = fopen(adFileName, "w");
  if (adFile) {
	fclose(adFile);
  } else {
	fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", adFileName);
  }

  // GENERAZIONE FILE CITAZIONI
  citFile = fopen(citFileName, "w");
  if (citFile) {
	fclose(citFile);
  } else {
	fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", citFileName);
  }

  if (!intro) {
	// GENERAZIONE FILE EDIZIONE
	edFile = fopen(edFileName, "w");
	if (edFile) {
	  fclose(edFile);
	} else {
	  fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", edFileName);
	}
  }

  // GENERAZIONE FILE INDICI
  for (int index = 0; index < MAXTESTIMONI; index++) {
	LEXEMA * lex = lexemi[index];
	if (lex != NULL) {
	  putIndPage(lex->sigla, parametri->nPag, lex->pagina);
	}
  }
}

char * getTocFilename() {
  char * tocFilename =  new char[strlen(pathName) + strlen(TOC_FILE) + 2];
  sprintf(tocFilename, "%s/%s", pathName, TOC_FILE);
  return tocFilename;
}

char * getIndexFilename() {
  char * indexFilename =  new char[strlen(pathName) + strlen(INDEX_FILE) + 2];
  sprintf(indexFilename, "%s/%s", pathName, INDEX_FILE);
  return indexFilename;
}

char * getTitleFilename() {
  char * titleFilename =  new char[strlen(pathName) + strlen(TITLE_FILE) + 2];
  sprintf(titleFilename, "%s/%s", pathName, TITLE_FILE);
  return titleFilename;
}

void printToc(int level, int nPag, const char * titolo) {
  if (strlen(titolo) > 0) {
	char * tocFilename = getTocFilename();
	FILE * toc = fopen(tocFilename, "a");
	fprintf(toc, "<p class='level%d'>%s<span class='pagenum'>p. %d</span></p>\n", level, titolo, nPag);
	fclose(toc);
  }
}

void printNewIndexLine(FILE * index) {
  if (primoIndex) {
	primoIndex = false;
  } else {
	fprintf(index, ",");
  }
  fprintf(index, "\n");
}

void printIndexSec(PARAMETRI * parametri, const char * titolo) {
  if (strlen(titolo) > 0) {
	char * indexFilename = getIndexFilename();
	FILE * index = fopen(indexFilename, "a");
	printNewIndexLine(index);
	fprintf(
			index,
			"\t{\"id\":\"%d\", \"titolo\":\"%s\"}",
			parametri->nSec,
			titolo);
	fclose(index);
  }
}

void printIndexSubsec(PARAMETRI * parametri, const char * titolo) {
  if (strlen(titolo) > 0) {
	char * indexFilename = getIndexFilename();
	FILE * index = fopen(indexFilename, "a");
	printNewIndexLine(index);
	fprintf(
			index,
			"\t{\"id\":\"%d.%d\", \"titolo\":\"%s\"}",
			parametri->nSec, parametri->nSubsec,
			titolo);
	fclose(index);
  }
}

void printIndexSubsubsec(PARAMETRI * parametri, const char * titolo) {
  if (strlen(titolo) > 0) {
	char * indexFilename = getIndexFilename();
	FILE * index = fopen(indexFilename, "a");
	printNewIndexLine(index);
	fprintf(
			index,
			"\t{\"id\":\"%d.%d.%d\", \"titolo\":\"%s\"}",
			parametri->nSec, parametri->nSubsec, parametri->nSSubsec,
			titolo);
	fclose(index);
  }
}

// INIZIALIZZAZIONE PAGINE FILE DI INTRODUZIONE SENZA LA CREAZIONE DELLE DIRECTORY
void introduzionePagina(char         * fileName,
						PARAMETRIm2h * parametri,
						struct testo * titolo) {
  
  FILE *vvFile;
  FILE *adFile;
  FILE *citFile;
  FILE *edFile;
  
  // ALLOCAZIONE DELLO SPAZIO PER I NOMI
  edFileName    = new char[strlen(EDITION_FILE) + strlen(pathName) + 10];
  htmlFileName  = new char[strlen(CONTENT_FILE) + strlen(pathName) + 10];
  vvFileName    = new char[strlen(VARIANTS_FILE) + strlen(pathName) + 10];
  citFileName   = new char[strlen(CITATIONS_FILE) + strlen(pathName) + 10];
  adFileName    = new char[strlen(ADNOTATIONS_FILE) + strlen(pathName) + 10];
    
  // GENERAZIONE DEL NOME DEI FILE DI USCITA
  sprintf(htmlFileName, "%s/%s", pathName, CONTENT_FILE);
  sprintf(vvFileName,   "%s/%s", pathName, VARIANTS_FILE);
  sprintf(adFileName,   "%s/%s", pathName, ADNOTATIONS_FILE);
  sprintf(citFileName,  "%s/%s", pathName, CITATIONS_FILE);
  sprintf(edFileName,   "%s/%s", pathName, EDITION_FILE);
  
  // IMPOSTAZIONE FILE DI OUTPUT
  if (primaIntro) {
	mkdir(pathName, S_IRWXU | S_IRWXG | S_IRWXO);
	parametri->setFileName(htmlFileName);
	
	// GENERAZIONE DEL FILE DELLE NOTE
	vvFile = fopen(vvFileName, "w");
	if (vvFile) {
	  fclose(vvFile);
	} else {
	  fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", vvFileName);
	}
	
	// GENERAZIONE DEL FILE DELLE ANNOTAZIONI
	adFile = fopen(adFileName, "w");
	if (adFile) {
	  fclose(adFile);
	} else {
	  fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", adFileName);
	}
	
	// GENERAZIONE FILE CITAZIONI
	citFile = fopen(citFileName, "w");
	if (citFile) {
	  fclose(citFile);
	} else {
	  fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", citFileName);
	}
	
	if (!intro) {
	  // GENERAZIONE FILE EDIZIONE
	  edFile = fopen(edFileName, "w");
	  if (edFile) {
		fclose(edFile);
	  } else {
		fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", edFileName);
	  }
	}
	
	// GENERAZIONE FILE INDICI
	for (int index = 0; index < MAXTESTIMONI; index++) {
	  LEXEMA * lex = lexemi[index];
	  if (lex != NULL) {
		putIndPage(lex->sigla, primaIntro ? 1 : parametri->nPag, lex->pagina);
	  }
	}
  }
  primaIntro = false;
}

void finePagina(PARAMETRIm2h * parametri) {
  
  FILE *vvFile;
  FILE *adFile;
  FILE *citFile;
  
  // GENERAZIONE DELLA FINE DEL FILE
  vvFile = fopen(vvFileName, "a");
  if (vvFile) {
	// CHIUSURA DEL FILE DELLE NOTE
	fclose(vvFile);
  } else {
	fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", vvFileName);
  }
  
  // GENERAZIONE DELLA FINE DEL FILE
  adFile = fopen(adFileName, "a");
  if (adFile) {
	// CHIUSURA DEL FILE DELLE ANNOTAZIONI
	fclose(adFile);
  } else {
	fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", adFileName);
  }
  
  // GENERAZIONE DELLA FINE DEL FILE
  citFile = fopen(citFileName, "a");
  if (citFile) {
	// CHIUSURA DEL FILE DELLE CITAZIONI
	fclose(citFile);
  } else {
	fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", citFileName);
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

	// INSERIMENTO DELL'ANCORA NEL FILE DEL TESTO CRITICO
	fprintf(parametri->outFile, "<a name=\"CIT%d\"></a>", parametri->nCIT);

	FILE * citFile = fopen(citFileName, "a");

	if (citFile) {

	  // SALVATAGGIO PARAMETRI
	  FILE * outFile = parametri->outFile;

	  // FILE CITAZIONI COME FILE DI OUTPUT
	  parametri->outFile = citFile;

	  // INSERIMENTO DEL LINK AL TESTO CRITICO
	  // IL NUMERO VIENE INSERITO CON LA CLASS DELLA UNIT
	  fprintf(citFile, "<li><a href=\"#CIT%d\" class=\"unit\">%d</a>&nbsp; ",
			  parametri->nCIT, parametri->nUnit);

	  // INSERIMENTO DELL'ANCORA
	  fprintf(citFile, "<a name=\"CITP%d\"></a>", parametri->nCIT);

	  // STAMPA DELL'OPZIONE
	  if (testo->lexema != NULL) {
		printNota(testo->lexema, parametri->outFile);
		fprintf(parametri->outFile, " ");
	  }
	  // STAMPO IL TESTO NEL FILE DELLE CITAZIONI
	  visitTesto(testo->testo1, parametri, sigle);

	  if (testo->testo2) {
		//	  fprintf(parametri->outFile, " ");
		trovaRiferimento(testo->testo2->testo1, &iConf, &libro, &prop);

		if (iConf >= 0) {
		  fprintf(parametri->outFile, " ");
		  printTagNota(parametri->outFile, true);
		  printRiferimento(parametri, iConf, libro, prop);
		  printTagNota(parametri->outFile, false);
		  stampaCitazioneOpera(parametri, libro, prop);
		} else {
		  fprintf(parametri->outFile, " ");
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
	  
	  fprintf(citFile, "</li>\n");
	  
	  // RIPRISTINO DEI PARAMETRI
	  parametri->outFile = outFile;
	  
	  // CHIUSURA DEL FILE DELLE CITAZIONI
	  fclose(citFile);
	} else {
	  fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", citFileName);
	}
  }
  
  //if (iConf >= 0) {

	// INSERIMENTO DEL LINK ALL'OPERA NEL FILE DEL TESTO CRITICO
	fprintf(parametri->outFile, "<a href=\"#CITP%d", parametri->nCIT);
	fprintf(parametri->outFile, "\" class=\"source\">");
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "</a>");
	//} else {
	//visitTesto(testo->testo1, parametri, sigle);
	//}
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
void printKey(bool inizio,
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
	case HTMLCUTKEY:
	  if (intro) {
		introduzionePagina(fileName, parametri, testo);
	  } else {
		if (inizio) {
		  const char * tit = getTesto(testo);
		  printf("%s.tex: %ld - pagina: %d.%s\n",
				 fileName, ln, parametri->nPag+1, tit);
		  if (parametri->outFile != NULL) {
			finePagina(parametri);
		  }
		  nuovaPagina(fileName, parametri, testo);
		}
	  }
	  break;
	case PROPKEY:
	  if (inizio) {
		if (testo) {
		  struct testo * t = testo;
		  while(t) {
			addProposizione(t->lexema, parametri->nPag);
			t = t->testo2;
		  }
		}
	  }
	  break;
	case ARGKEY:
	  if (inizio) {
		handleArgument(testo, parametri->nPag);
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
		  sprintf(link, "./micons/braces/lbra%d.jpg", parametri->mRow);
		  printImage(link, "graffa aperta", parametri->outFile);
		} else {
		  fprintf(parametri->outFile, ".");
		}
	  }
	  break;
	case RIGHTGRATABKEY:
	  if (inizio) {
		if (graffe) {
		  sprintf(link, "./micons/braces/rbra%d.jpg", parametri->mRow);
		  printImage(link, "graffa chiusa", parametri->outFile);
		} else {
		  fprintf(parametri->outFile, ".");
		}
	  }
	  break;
	case LEFTANGTABKEY:
	  if (inizio) {
		if (graffe) {
		  sprintf(link, "./micons/braces/lang%d.jpg", parametri->mRow);
		  printImage(link, "angolare aperta", parametri->outFile);
		} else {
		  fprintf(parametri->outFile, ".");
		}
	  }
	  break;
	case RIGHTANGTABKEY:
	  if (inizio) {
		if (graffe) {
		  sprintf(link, "./micons/braces/rang%d.jpg", parametri->mRow);
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
		/*
		  fprintf(parametri->outFile, "<!-- prologo:\n");
		  visitTesto(testo->testo1, parametri, sigle);
		  fprintf(parametri->outFile, " -->\n\n");
		*/
		visitTesto(testo->testo1, parametri, sigle);
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
		}
		break;*/
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
		printf(parametri->outFile, "foliumInTesto\n");
		}*/
	  break;
	case UNITEMPTYKEY:
	  if (inizio) {
		printUnitRef(-1, parametri);
	  }
	  break;
	case UNITKEY:
	  if (inizio) {
		if (testo != NULL) {
		  if (testo->lexema != NULL) {
		  }
		}
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
	case MANUSKEY:
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
	    fprintf(parametri->outFile, "<a name=\"SEC%d\">", parametri->nSec);
	    lastTitolo = getTesto(testo);
	    printToc(1, parametri->nPag, lastTitolo);
		if (intro) {
		  printIndexSec(parametri, lastTitolo);
		}
	  } else {
	    fprintf(parametri->outFile, "</a>");
	    fprintf(parametri->outFile, "</h2>\n\n");
	  }
	  break;
	case SUBSECTIONKEY:
	  if (inizio) {
	    fprintf(parametri->outFile, "<h3>");
	    fprintf(parametri->outFile, "<a name=\"SEC%d.%d\">",
				parametri->nSec, parametri->nSubsec);
	    lastTitolo = getTesto(testo);
	    printToc(2, parametri->nPag, lastTitolo);
		if (intro) {
		  printIndexSubsec(parametri, lastTitolo);
		}
	  } else {
	    fprintf(parametri->outFile, "</a>");
	    fprintf(parametri->outFile, "</h3>\n\n");
	  }
	  break;
	case SSSECTIONKEY:
	  if (inizio) {
	    fprintf(parametri->outFile, "<h4>");
	    fprintf(parametri->outFile, "<a name=\"SEC%d.%d.%d\">",
				parametri->nSec, parametri->nSubsec, parametri->nSSubsec);
	    lastTitolo = getTesto(testo);
	    printToc(3, parametri->nPag, lastTitolo);
		if (intro) {
		  printIndexSubsubsec(parametri, lastTitolo);
		}
	  } else {
	    fprintf(parametri->outFile, "</a>");
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
	case CITEKEY:
	  if (inizio) {
	    fprintf(parametri->outFile,
		    "<a href=\"./instrumenta/bibliographica.htm#%s\">%s</a>",
		    testo->lexema, testo->lexema);
	  }
	  break;
	case MCITEKEY:
	  if (inizio) {
	    fprintf(parametri->outFile, "<a href=\"./instrumenta.html?cite=%s\">", testo->lexema);
	  } else {
	    fprintf(parametri->outFile, "</a>");
	  }
	  break;
	case LABELKEY:
	  if (inizio) {
	    char * sezione= new char[21];
	    sprintf(sezione, "%d", parametri->nSec);
	    if (parametri->nSubsec != 0) {
	      sprintf(sezione, "%d.%d", parametri->nSec, parametri->nSubsec);
	    }
	    if (parametri->nSSubsec) {
	      sprintf(sezione, "%d.%d.%d",
		      parametri->nSec, parametri->nSubsec, parametri->nSSubsec);
	    }
	    if (intro) {
	      putLabelPage(testo->lexema, sezione);
	    }
	  }
	  break;
	case REFKEY:
	  if (inizio) {
	    const char * label = findExternalLabel(testo->lexema);
	    const char * ref;
	    if (label == NULL) {
	      label = labelkey;
	      ref = testo->lexema;
	    } else {
	      int start = strlen(label);
	      ref = &testo->lexema[start];
	    }
	    /* char * cleanLabel = new char[strlen(label)+1]; */
	    /* strcpy(cleanLabel, label); */
	    /* if (cleanLabel[strlen(label)-1] == '_') { */
	    /*   cleanLabel[strlen(label)-1] = 0; */
	    /* } */
		const char * title = findRiferimentoIncrociato(riferimentiIncrociati, label, ref, 3);
	    fprintf(parametri->outFile,
		    "<a href=\"./paragrafo.html?intro=%s&section=%s\">&sect; %s</a>",
				label, ref, title);
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
		fprintf(parametri->outFile, "<p>\n\n<center><em>");
	  } else {
		fprintf(parametri->outFile, "</em></center>\n\n</p>");
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
	    // fprintf(parametri->outFile,"//");
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
	  /* 			if (inizio) { */
	  /* 				fprintf(parametri->outFile, "inizio "); */
	  /* 			} else { */
	  /* 				fprintf(parametri->outFile, "fine "); */
	  /* 			} */
	  /* 			fprintf(parametri->outFile, "%s<hr noshade>\n", testo->lexema); */
	  break;
	case TITMARG:
	  if (inizio) {
		fprintf(parametri->outFile, "<p align=\"right\"><table bgcolor=\"mintcream\"><tr><td>");
	  } else {
		fprintf(parametri->outFile, "</td></tr></table></p>");
	  }
	  break;
	case ENUNCIATIO:
	  if (inizio) {
	    fprintf(parametri->outFile, "<p class=\"enunciatio\">");
	  } else {
	    fprintf(parametri->outFile, "</p>");
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
	    fprintf(parametri->outFile, "<span class=\"protasis\">");
	  } else {
	    fprintf(parametri->outFile, "</span>");
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
		// CONTROLLARE FORSE VA ELIMINATO
		fprintf(parametri->outFile, "\n<p></p>");
	  }
	  break;
	case PARAGRAPHEND:
	  if (inizio) {
		// CONTROLLARE FORSE VA ELIMINATO
		fprintf(parametri->outFile, "\n<p></p>");
	  }
	  break;
	case PARAGRAPHSTART:
	  if (inizio) {
		// CONTROLLARE FORSE VA ELIMINATO
		fprintf(parametri->outFile, "\n<p></p>");
	  }
	  break;
	case TITLEKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<p class='title'>");
	  } else {
		fprintf(parametri->outFile, "</p>\n\n");
	  }
	  break;
	case NUMINTESTKEY:
	  if (inizio) {
		fprintf(parametri->outFile, "<div class='intestazione'>\n<br/>\n<p>");
	  } else {
		fprintf(parametri->outFile, "</p>\n</div>\n");
	  }
	  break;
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
		fprintf(param->outFile, "\n<div class='intestazione'>\n<p>");
		if (param->mitIntest) {
		  visitTesto(param->mitIntest, param, sigle);
		}
		if (param->desIntest) {
		  fprintf(param->outFile, " a ");
		  visitTesto(param->desIntest, param, sigle);
		}
		fprintf(param->outFile, "</p>\n");
		fprintf(param->outFile, "<p>");
		visitTesto(param->lmiIntest, param, sigle);
		if (param->lmiIntest && param->datIntest) {
		  fprintf(param->outFile, ", ");
		}
		visitTesto(param->datIntest, param, sigle);
		fprintf(param->outFile, "</p>\n");
		fprintf(param->outFile, "</div>\n");
		fprintf(param->outFile, "\n<div class='commento'>\n<p>");
		fprintf(param->outFile, "<p>");
		visitTesto(param->comIntest, param, sigle);
		fprintf(param->outFile, "</p>\n");
		fprintf(param->outFile, "</div>\n");
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
	  /* 			if (inizio) { */
	  /* 				fprintf(parametri->outFile, "<font color=red>&iquest;</font>"); */
	  /* 			} */
	  break;
	}
  }
}

// FUNZIONE DI STAMPA DI UNA FIGURA
void printFig(short key, struct testo * testo, PARAMETRI * parametri) {
  if (figExtension == NULL) {
	figExtension = new char[4];
	sprintf(figExtension, "%s", "jpg");
  }
  fprintf(parametri->outFile, "<p><center>\n");
  if (key == FIGURAKEY) {
	//fprintf(parametri->outFile, "<a href=\"./sources/codex/%s", fileName);
	fprintf(parametri->outFile, "<a href=\"./figure.html?fig=");
	visitTesto(testo->testo2, parametri, NULL);
	fprintf(parametri->outFile, ".%s\">", figExtension);
	fprintf(parametri->outFile,"<img vspace=10 border=0 src=\"./figure.html?fig=");
	visitTesto(testo->testo2, parametri, NULL);
	fprintf(parametri->outFile, ".%s\"", figExtension);
	fprintf(parametri->outFile, " alt=\"");
	visitTesto(testo->testo2, parametri, NULL);
	fprintf(parametri->outFile, "\"></a>\n");
  } else {
	/* fprintf(parametri->outFile, */
	/* 		"<a href=\"./sources/codex/%s/%sf%d.%s\"><img vspace=10 border=0 src=\"./sources/codex/%s/%sf%d.%s\" alt=\"figura %d\"></a>\n", */
	/* 		fileName, fileName, parametri->nFig, figExtension, */
	/* 		fileName, fileName, parametri->nFig, figExtension, */
	/* 		parametri->nFig); */
	fprintf(parametri->outFile,
			"<a href=\"./figure.html?fig=%sf%d.%s\"><img vspace=10 border=0 src=\"./figure.html?fig=%sf%d.%s\" alt=\"figura %d\"></a>\n",
			fileName, parametri->nFig, figExtension,
			fileName, parametri->nFig, figExtension,
			parametri->nFig);
  }
  
  // DIDASCALIA
  if (testo->testo1) {
	fprintf(parametri->outFile, "<br />\n");
	visitTesto(testo->testo1, parametri, NULL);
  }
  fprintf(parametri->outFile, "</center></p>");
}

// FUNZIONE DI STAMPA DI UN RIFERIMENTO AD UNA UNIT
void printUnitRef(const char * opera, const char * etichetta, PARAMETRI * parametri) {
  fprintf(parametri->outFile, "<a class=\"unitref\" href=\"./paragrafo.html?");
  if (opera != NULL) {
	const char * title = findRiferimentoIncrociato(riferimentiIncrociati, opera, etichetta, 2);
	fprintf(parametri->outFile, "opera=%s", opera);
	fprintf(parametri->outFile, "&unit=%s\">%s</a>", etichetta, title);
  } else {
	short nUnit = findUnit(etichetta);
	fprintf(parametri->outFile, "opera=%s", labelkey);
	fprintf(parametri->outFile, "&unit=%s\">%d</a>", etichetta, nUnit);
  }
}

void printUnitRef(short nUnit, PARAMETRI * parametri) {
	bool nonempty = (nUnit >= 0);
	if (nonempty) {
		fprintf(parametri->outFile, "<span class=\"unit\">");
		fprintf(parametri->outFile, "<a name=\"UN%d\">", nUnit);
		fprintf(parametri->outFile, "%d", nUnit);
		fprintf(parametri->outFile, "</a>\n\n");
		fprintf(parametri->outFile, "</span>");
	}
}

// FUNZIONE DI STAMPA DI UNA PAROLA
void printLexema(short key, const char * lexema, PARAMETRI * param) {
  
  PARAMETRIm2h * parametri = (PARAMETRIm2h *) param;
  
  if (lexema != NULL) {
	
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
		
	  case UNITKEY:
		// SCRIVO IL FILE JSON
		if (!intro) {
		  putUnitPage(lexema, parametri->nPag, parametri->nUnit);
		}
		break;
		
	  case NOTAMACRO:
	  case OUTRANGE:
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
						"<img src=\"./micons/greek/%cgreek.gif\" align=\"middle\" alt=\"%s\">",
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

// FUNZIONE DI STAMPA DI ALIAMANUS
void printAliaManus(struct testo * testo,
					   PARAMETRI * parametri,
					   struct testo * sigle) {
  visitTesto(testo->testo2, parametri, sigle);
}

// FUNZIONE DI STAMPA DEL FRONTESPIZIO
void printFrontespizio(struct testo * testo,
					   PARAMETRI * parametri,
					   struct testo * sigle) {

  // APERTURA DEL FILE
  char *titleFilename = getTitleFilename();
  FILE *titleFile = fopen(titleFilename, "w");
  FILE *outFile = parametri->outFile;
  parametri->outFile = titleFile;
  // SCRIVO IL CONTENUTO DEL FILE
  fprintf(titleFile, "\n\n<div class='frontespizio'>\n");
  fprintf(titleFile, "<h1 class='volume-id'>");
  visitTesto(testo->testo1, parametri, sigle);
  fprintf(titleFile, "</h1>\n");
  fprintf(titleFile, "<h1 class='volume-title'>");
  visitTesto(testo->testo2->testo1, parametri, sigle);
  fprintf(titleFile, "</h1>\n");
  printAuthors(testo->testo2->testo2, parametri, sigle);
  fprintf(titleFile, "</div>\n\n");
  // CHIUSURA DEL FILE
  fclose(titleFile);
  parametri->outFile = outFile;

}

// FUNZIONI DI STAMPA DI UNA LISTA DI AUTORI
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

bool checkNotEmpty(struct testo * testo) {
  return testo->testo1 != NULL;
}

void printAuthors(struct testo * testo,
				  PARAMETRI * parametri,
				  struct testo * sigle) {
  if (checkNotEmpty(testo)) {
	fprintf(parametri->outFile, "<p class='authors'>a cura di ");
	fprintf(parametri->outFile, "<span>");
	internalPrintAuthors(testo, parametri, sigle);
	fprintf(parametri->outFile, "</span>");
	fprintf(parametri->outFile, "</p>\n\n");
  }
}

// FUNZIONE DI STAMPA DI UNA NOTA A PIEDE DI PAGINA
void printFootnote(struct testo * testo,
				   PARAMETRI * parametri,
				   struct testo * sigle) {
  
  FILE *adFile, *outFile;
  
  // ANCORA NEL TESTO CRITICO
  fprintf(parametri->outFile, "<a name=\"FOOT%d\"></a>",
		  parametri->nFootnote);
  
  // LINK
  fprintf
	(parametri->outFile,
	 "<sup class=\"footnote\"><a href=\"#FOOTP%d\">[%d]</a></sup>",
	 parametri->nFootnote, parametri->nFootnote);
  
  // APERTURA DEL FILE
  adFile = fopen(adFileName, "a");
  
  if (adFile) {
	
	// ANCORA NEL FILE
	fprintf(adFile, "<li><a name=FOOTP%d></a>", parametri->nFootnote);
	
	// LINK AL TESTO CRITICO
	fprintf(adFile, "<a href=\"#FOOT%d\"><sup class=\"annotation\">%d</sup></a>",
			parametri->nFootnote, parametri->nFootnote);
	
	// SALVATAGGIO DEL RIFERIMENTO AL FILE DI OUTPUT
	outFile = parametri->outFile;
	
	// STAMPA NEL FILE
	parametri->outFile = adFile;
	visitTesto(testo, parametri, sigle);
	fprintf(adFile, "</li>\n");
	
	// CHIUSURA DEL FILE
	fclose(adFile);
	
	// RIPRISTINO DEL RIFERIMENTO AL FILE DI OUTPUT
	parametri->outFile = outFile;
	
  } else {
	fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", adFileName);
  }
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
  
  FILE *adFile, *outFile;
  
  // ANCORA DELL'ANNOTAZIONE NEL TESTO CRITICO
  fprintf(parametri->outFile, "<a name=\"ADN%d\"></a>",
		  parametri->nAdnot);
  
  // LINK ALL'ANNOTAZIONE
  fprintf
	(parametri->outFile,
	 "<sup class=\"annotation\"><a href=\"#ADNP%d\">[%d]</a></sup>",
	 parametri->nAdnot, parametri->nAdnot);
  
  // APERTURA DEL FILE DELLE ANNOTAZIONI
  adFile = fopen(adFileName, "a");
  
  if (adFile) {
	
	// ANCORA DELL'ANNOTAZIONE
	fprintf(adFile, "<li><a name=ADNP%d></a>", parametri->nAdnot);
	
	// LINK AL TESTO CRITICO
	fprintf(adFile, "<a href=\"#ADN%d\"><sup class=\"annotation\">%d</sup></a>",
			parametri->nAdnot, parametri->nAdnot);
	
	// SALVATAGGIO DEL RIFERIMENTO AL FILE DI OUTPUT
	outFile = parametri->outFile;
	
	// STAMPA NEL FILE DELLE ANNOTAZIONI
	parametri->outFile = adFile;
	visitTesto(testo, parametri, sigle);
	fprintf(adFile, "</li>\n");
	
	// CHIUSURA DEL FILE DELLE ANNOTAZIONI
	fclose(adFile);
	
	// RIPRISTINO DEL RIFERIMENTO AL FILE DI OUTPUT
	parametri->outFile = outFile;
	
  } else {
	fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", adFileName);
  }
}

// FUNZIONE DI MARCATURA DEL TESTO CRITICO
void printInizioTestoCritico(short key, PARAMETRI * parametri) {
}

void printFineTestoCritico(short key, PARAMETRI * parametri) {
}

// FUNZIONE DI MARCATURA DELLE VARIANTI
void printInizioVarianti(PARAMETRI * parametri) {
  fprintf(parametri->outFile,"<a name=\"VV%d\"></a>", parametri->nVV);
  fprintf(parametri->outFile,
		  "<sup class=\"variant\"><a href=\"#VVP%d\">%d</a></sup>",
		  parametri->nVV, parametri->nVV);
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
	  fprintf(vvFile, "<li><a name=\"VVP%d\"></a>", parametri->nVV);
	  fprintf(vvFile, "<sup class=\"variant\"><a href=\"#VV%d\" >%d</a></sup>",
			  parametri->nVV, parametri->nVV);
	  if (key == VBKEY) {
		fprintf(vvFile, " (<em>banale</em>) ");
	  }

	  // STAMPA DELLA NOTA
	  parametri->outFile = vvFile;
	  parametri->nota = true;
	  if (key == NOTEKEY) {
		visitTesto(testo, parametri, sigle);
	  } else {
		visitVarianti(testo, parametri, sigle,
					  false, false, false, 1);
	  }
	  fprintf(vvFile, "</li>\n\n");

	  // CHIUSURA DEL FILE DELLE NOTE
	  fclose(vvFile);
	} else {
	  fprintf(stderr, "\nERRORE: impossibile aprire il file %s\n\n", vvFileName);
	}
  }

  // RIPRISTINO DEI PARAMETRI
  parametri->outFile = outFile;
  parametri->nota = nota;
}

// FUNZIONE PER CREARE IL FILE HTML DELLE OPERE
void creaHTMLOpere () {

  short i;

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
			  fprintf(htmlOpeFile, " - %s</a><br />\n", tok);
			}
		  }
		}
	  }

	  // CHIUSURA DEL FILE HTML DELL'OPERA
	  fclose(htmlOpeFile);

	  // CHIUSURA DEL FILE DELLE CONFIGURAZIONI
	  fclose(txtOpeFile);

	  // SE IL FILE DELLE CITAZIONI DELL'OPERA
	} else {
	  if (verbose) {
		fprintf(stderr, "\nERRORE: impossibile leggere il file %s\n\n", conf->getTxtOpeFileName());
	  }
	}
  }
}

// FUNZIONE DI STAMPA DELL'ALBERO SINTATTICO
void printDocumento(struct testo * testo) {

  PARAMETRIm2h * parametri;
  if (testo) {
	// LETTURA DEL FILE DI CONFIGURAZIONE
	leggiConfigurazione(confFileName);
	// CREA IL FILE HTML DELLE OPERE CITATE
	// creaHTMLOpere();
	riferimentiIncrociati = leggiRiferimentiIncrociati(refFileName);
  }

  char * tocFilename = getTocFilename();
  FILE * tocFile = fopen(tocFilename, "w");
  if (tocFile == NULL) {
	fprintf(stderr, "\nERRORE: impossibile creare il file %s\n\n", tocFilename);
	exit(1);
  }
  fprintf(tocFile, "<html>\n");
  fprintf(tocFile, "<head>\n");
  fprintf(tocFile, "<link href='indice.css' rel='stylesheet' type='text/css'>\n");
  fprintf(tocFile, "</head>\n");
  fprintf(tocFile, "<body>\n");
  fclose(tocFile);

  if (intro) {
	char * indexFilename = getIndexFilename();
	FILE * indexFile = fopen(indexFilename, "w");
	fprintf(indexFile, "[");
	fclose(indexFile);
  }

  // GENERAZIONE DEI PARAMETRI
  parametri = new PARAMETRIm2h();

  // INIZIO GENERAZIONE CODICE
  printf("generazione del codice\n");
  primaLabel = true;
  primaUnit = true;
  primoIndex = true;

  // STAMPA DEL DOCUMENTO
  visitTesto(testo, parametri, NULL);

  // CHIUSURA ULTIMA PAGINA
  finePagina(parametri);

  // CHIUSURA FILE IND_JSON
  for (int index = 0; index < MAXTESTIMONI; index++) {
	LEXEMA * lex = lexemi[index];
	if (lex != NULL) {
	  char * indFileName = getIndFileName(lex->sigla);
	  FILE * indFile = fopen(indFileName, "a");
	  fprintf(indFile, "\n}");
	  fclose(indFile);
	  
	  // CHIUSURA FILE IMAGE_JSON
	  char * imageFileName = getImageFileName(lex->sigla);
	  FILE * imageFile = fopen(imageFileName, "a");
	  fprintf(imageFile, "\n}");
	  fclose(imageFile);
	}
  }
  
  if (!primaLabel) {
	char * labelFileName =  getLabelFileName();
	FILE * labelFile = fopen(labelFileName, "a");
	fprintf(labelFile, "\n}");
	fclose(labelFile);
  }
  
  if (!primaUnit) {
	char * unitFileName =  getUnitFileName();
	FILE * unitFile = fopen(unitFileName, "a");
	fprintf(unitFile, "\n}");
	fclose(unitFile);
  }

  tocFile = fopen(tocFilename, "a");
  fprintf(tocFile, "</body>\n");
  fprintf(tocFile, "</html>\n");
  fclose(tocFile);

  if (intro) {
	char * indexFilename = getIndexFilename();
	FILE * indexFile = fopen(indexFilename, "a");
	fprintf(indexFile, "\n]\n");
	fclose(indexFile);
  }

  // FILE DELLE PROPOSIZIONI
  char * propFileName =  new char[strlen(pathName) + 20];
  sprintf(propFileName, "%s/indice.htm", pathName);
  FILE * propFile = fopen(propFileName, "w");
  fprintf(propFile, "<html>\n");
  fprintf(propFile, "<head>\n");
  fprintf(propFile, "<link href='indice.css' rel='stylesheet' type='text/css'>\n");
  fprintf(propFile, "</head>\n");
  fprintf(propFile, "<body>\n");
  fprintf(propFile, "<dl>\n");
  
  Proposizione * proposizione = proposizioni;
  while (proposizione != NULL) {
	fprintf(propFile, "<dt class='main'>%s</dt><dd>\n", proposizione->label);
	Pagina * pagina = proposizione->pagine;
	fprintf(propFile, "<dl class='compact'>\n");
	while (pagina != NULL) {
	  fprintf(propFile, "<dt>p. %d</dt><dd>%s</dd>\n", pagina->numero,  pagina->titolo);
	  pagina = pagina->next;
	}
	fprintf(propFile, "</dl></dd>\n");
	Argomento * argomento = proposizione->argomenti;
	fprintf(propFile, "<dl>\n");
	while (argomento != NULL) {
	  fprintf(propFile, "<dt class='secondary'>%s</dt><dd><dl class='compact'>\n", argomento->titolo);
	  Pagina * pagina = argomento->pagine;
	  while (pagina != NULL) {
		fprintf(propFile, "<dt>p. %d</dt><dd>%s</dd>\n", pagina->numero,  pagina->titolo);
		pagina = pagina->next;
	  }
	  fprintf(propFile, "</dl></dd>\n");
	  argomento = argomento->next;
	}
	fprintf(propFile, "</dl>\n");
	proposizione = proposizione->next;
  }
  
  fprintf(propFile, "</dl>\n");
  fprintf(propFile, "</body>\n");
  fprintf(propFile, "</html>\n");
  fclose(propFile);
  
  // FINE GENERAZIONE CODICE
  printf("fine generazione del codice\n");

  // PULIZIA DELLA MEMORIA
  delete(parametri);

}

// PROGRAMMA PRINCIPALE
int main (int argn, char ** argv) {

  // INIZIALIZZAZIONE DELLE VARIABILI
  confFileName    = NULL;
  char * fn       = NULL;
  bool ok         = true;
  bool help       = false;
  figExtension    = NULL;
  labelkey        = NULL;

  vbTovv          = false;
  graffe          = false;
  verbose         = false;
  intro           = false;
  short i;
  labelkey = "";
  pathName = ".";
  refFileName = NULL;
  unitsFileName = NULL;
  lastTitolo = "";
  
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

	  case 'b':
		// VARIANTI BANALI TRATTATE COME VARIANTI SERIE
		vbTovv = true;
		break;

	  case 'i':
		// INTRODUZIONE
		intro = true;
		primaIntro = true;
		break;

	  case 'p':
		// PARENTESI GRAFFE INVECE DI BARRETTE
		graffe = true;
		break;

	  case 'h':
		// OPZIONI POSSIBILI
		help = true;
		break;

	  case 'l':
		// IDENTIFICATORE DELL'OPERA
		i++;
		labelkey = argv[i];
		break;

	  case 'o':
		// DIRECTORY SALVATAGGIO COMPILATI
		i++;
		pathName = argv[i];
		break;

	  case 'v':
		// MESSAGGI DI WARNING
		verbose = true;
		break;

	  case 'r':
		// FILE RIFERIMENTI INCROCIATI
		i++;
		refFileName = argv[i];
		break;

	  case 'F':
		// ESTENSIONE DEL FILE DELLE FIGURE
		i++;
		figExtension = argv[i];
		break;

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
	printf("uso: %s [-b] [-h] [-p] [-i] [-v]\n", argv[0]);
	printf("\t[-f <conf-file>]\n");
	printf("\t[-F <extension>]\n");
	printf("\t[-o <path-name>]\n");
	printf("\t[-l <chiave-opera>]\n");
	printf("\t<nome_file>\n");

	// OPZIONI POSSIBILI
	if (help) {
	  printf("opzioni:\n");
	  printf("  -b                   considera le varianti banali come effettive\n");
	  printf("  -f <conf-file>       usa <conf-file> come file di configurazione\n");
	  printf("  -h                   stampa questo messaggio\n");
	  printf("  -i                   introduzione\n");
	  printf("  -l <chiave-opera>    identificatore dell'opera\n");
	  printf("  -p                   traduce le parentesi con immagini\n");
	  printf("  -v                   stampa messaggi di avvertimento\n");
	  printf("  -F <extension>       usa <extension> come estensione delle figure\n");
	  printf("  -o <path-name>       usa <path-name> come directory di output\n");
	  printf("  -r <path-name>       usa <path-name> come file dei riferimenti incrociati\n");
	}
  }

  // FINE DEL PROGRAMMA
  return 0;
}
