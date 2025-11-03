#include "version.h"
/*
  file "m2ledmac.c"
  PROGRAMMA PRINCIPALE

  m2ledmac: traduttore da M-TeX a Reledmac basato su mvisit

  eLabor sc
  via Enrico Fermi 19, Pisa - Italia
  <https://elabor.biz>
  <info@elabor.biz>

  responsabile sviluppo:
  Paolo Mascellani (paolo@elabor.biz)

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
#include "mparsey.hpp"
#include "mvisit.h"

#define LATEXEST ".m.tex"

// GESTIONE PLUGIN

#include "standard.h"
#include "epistolario.h"

#define NUM_PLUGINS 2

static Plugin *plugins[NUM_PLUGINS];
static Plugin *lPlugins[NUM_PLUGINS];

static int numPlugin = 0;

void loadPlugin(const char *name)
{
	bool loaded = false;
	for (unsigned int index = 0; !loaded && index < NUM_PLUGINS; index++)
	{
		Plugin *plugin = plugins[index];
		const char *pName = plugin->name;
		if (strncmp(name, pName, strlen(name)) == 0)
		{
			printf("caricamento plugin %s\n", pName);
			lPlugins[numPlugin] = plugin;
			numPlugin++;
			loaded = true;
		}
	}
	if (!loaded)
	{
		fprintf(stderr, "impossibile caricare il plugin %s\n", name);
	}
}

// STRUTTURA DATI PARAMETRI
class PARAMETRIm2l : public PARAMETRI
{

public:
	short nOM;
	bool justify;
	bool noPar;
	bool parStarted;
	bool endnumbering;
	int numero;

	PARAMETRIm2l() : PARAMETRI()
	{
		this->nOM = 0;
		this->justify = true;
		this->noPar = false;
		this->parStarted = false;
		this->endnumbering = false;
		this->numero = 0;
	}
};

// STRUTTURA CODA DI VV
class VV
{

public:
	short key;
	struct testo *testo;
	VV *next;

	VV(short key,
	   struct testo *testo)
	{

		this->key = key;
		this->testo = testo;
		this->next = NULL;
	}
};

// VARIABILI GLOBALI
static char *latexFileName;
static bool vbTovv;
static bool variantePrecedente; // LEZIONE VARIANTE PRECEDENTE SE LEZIONE VUOTA
static char *testEx;			// TESTIMONE DI RIFERIMENTO PER L'ASTRATTO DI UN TESTIMONE
static char *confFileName;
static bool exclude;
static bool foliummarg;
static bool foliumtesto;
static short nSpaces;
static VV *firstVV;
static VV *lastVV;
static short nCodaVV;
static bool verbose; // STAMPA I MESSAGGI DI WARNING

// FUNZIONI PER OPZIONI GESTITE ESTERNAMENTE
bool verboseWarning()
{
	return verbose;
}

// FUNZIONI SPECIFICHE DEL TRADUTTORE LATEX

void printVF(struct testo *testo, PARAMETRI *parametri, struct testo *sigle) {
	fprintf(parametri->outFile, "\\par\n\\ApparatoFigure{");
	visitVarianti(testo, parametri, sigle, true, false, false, 1, NULL);
	fprintf(parametri->outFile, "}\n");
}

void startParagraph(PARAMETRIm2l *parametri)
{
	if (!parametri->parStarted && !parametri->noPar)
	{
		fprintf(parametri->outFile, "\\pstart\n");
		parametri->parStarted = true;
	}
}

void endParagraph(PARAMETRIm2l *parametri)
{
	if (parametri->parStarted)
	{
		fprintf(parametri->outFile, "\\pend");
		parametri->parStarted = false;
	}
}

bool checkLexema(const char *lex1, const char *lex2)
{
	bool value;
	if (lex1 == lex2)
	{
		value = true;
	}
	else
	{
		if (lex1 == NULL || lex2 == NULL)
		{
			value = false;
		}
		else
		{
			value = (strcmp(lex1, lex2) == 0);
		}
	}
	return value;
}

bool checkTesto(struct testo *testo1, struct testo *testo2)
{
	bool value;
	if (testo1 == testo2)
	{
		value = true;
	}
	else
	{
		if (testo1 == NULL || testo2 == NULL)
		{
			value = false;
		}
		else
		{
			if (checkLexema(testo1->lexema, testo2->lexema))
			{
				value = checkTesto(testo1->testo1, testo2->testo1) && checkTesto(testo1->testo2, testo2->testo2);
			}
			else
			{
				value = false;
			}
		}
	}
	return value;
}

class Descrizione
{

public:
	struct testo *descrizione;
	Descrizione *next;
};

bool checkDescrizione(struct testo *descrizione, Descrizione *descrizioni)
{
	bool value;
	if (descrizioni == NULL)
	{
		value = false;
	}
	else
	{
		if (checkTesto(descrizione, descrizioni->descrizione))
		{
			value = true;
		}
		else
		{
			value = checkDescrizione(descrizione, descrizioni->next);
		}
	}
	return value;
}

static Descrizione *descrizioni;

void addDescrizione(struct testo *descrizione)
{
	Descrizione *nuova = new Descrizione();
	nuova->descrizione = descrizione;
	nuova->next = descrizioni;
	descrizioni = nuova;
}

void disposeDescrizioni(Descrizione *descrizioni)
{
	if (descrizioni != NULL)
	{
		disposeDescrizioni(descrizioni->next);
		delete (descrizioni);
	}
}

void resetDescrizioni()
{
	disposeDescrizioni(descrizioni);
	descrizioni = NULL;
}

bool checkDescrizione(struct testo *descrizione)
{
	return checkDescrizione(descrizione, descrizioni);
}

void printCodaVV(PARAMETRIm2l *parametri, struct testo *sigle);

// TRASFORMA UNA VARIANTE BANALE IN UNA SERIA SE RICHIESTO
bool opzioneBanale()
{
	return vbTovv;
}

bool opzioneVariantePrecedente()
{
	return variantePrecedente;
}

// RITORNA IL TESTIMONE PER VEDERENE L'ESTRATTO SE RICHIESTO
char *opzioneTestEx()
{
	return testEx;
}

// FUNZIONE CHE STAMPA IL PROLOGO DI UNA TABELLA
void printTableStart(int key, struct testo *testo, PARAMETRI *param, struct testo *sigle)
{
	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;
	if (!parametri->parStarted)
	{
		fprintf(parametri->outFile, "\\pstart");
		parametri->parStarted = true;
	}
	const char *type = (key == TABLEKEY ? "tabular" : "longtable");
	fprintf(parametri->outFile, "\\begin{%s}{rcl}\n", type);
	visitTesto(testo, parametri, sigle);
	fprintf(parametri->outFile, " & ");
}

// FUNZIONE CHE STAMPA L'EPILOGO DI UNA TABELLA
void printTableFinish(int key, struct testo *tabella, PARAMETRI *parametri, struct testo *sigle)
{
	struct testo * testo = tabella->testo1->testo1;
	const char *type = (key == TABLEKEY ? "tabular" : "longtable");
	fprintf(parametri->outFile, " & ");
	visitTesto(testo, parametri, sigle);
	fprintf(parametri->outFile, "\\end{%s} ", type);
}

// FUNZIONE CHE STAMPA LO HEADER DI RIGA DI UNA TABELLA
void printRowHeader(short key, PARAMETRI *parametri)
{
	if (key == HLINEKEY)
	{
		visitKey(key, 0, NULL, parametri, NULL);
	}
}

// FUNZIONE CHE STAMPA IL FOOTER DI RIGA DI UNA TABELLA
void printRowFooter(short key, PARAMETRI *parametri)
{
	fprintf(parametri->outFile, "\\\\");
	fprintf(parametri->outFile, "\n");
}

// FUNZIONE CHE STAMPA LO HEADER DI UNA TABELLA
void printTableHeader(int key, const char *colonne, PARAMETRI *parametri)
{
	const char *type = (key == TABLEKEY ? "tabular" : "longtable");
	fprintf(parametri->outFile, "\\begin{%s}{%s}\n", type, colonne);
}

// FUNZIONE CHE STAMPA IL FOOTER DI UNA TABELLA
void printTableFooter(int key, PARAMETRI *parametri)
{
	const char *type = (key == TABLEKEY ? "tabular" : "longtable");
	fprintf(parametri->outFile, "\\end{%s}", type);
}

// FUNZIONE CHE STAMPA UN CAMPO DI UNA TABELLA
void printCampo(short nRow,
				short nCol,
				const char *colonne,
				struct testo *campo,
				PARAMETRI *parametri,
				struct testo *sigle)
{
	if (nCol > 1)
	{
		fprintf(parametri->outFile, " & ");
	}
	if (campo != NULL)
	{
		// GESTIONE INIZIALE MULTIROW E MULTICOL
		switch (campo->key)
		{
		case MULTICOLKEY:
			if (strchr(colonne, '|') == NULL)
			{
				fprintf(parametri->outFile, "\\multicolumn{%d}{c}{", atoi(campo->lexema));
			}
			else
			{
				fprintf(parametri->outFile, "\\multicolumn{%d}{|c|}{", atoi(campo->lexema));
			}
			break;
		}

		switch (campo->key)
		{
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
		switch (campo->key)
		{
		case MULTICOLKEY:
			fprintf(parametri->outFile, "}");
			break;
		}
	}
}

// ANALISI TESTO DEI CASES
void printCasi(struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{
	if (testo != NULL)
	{
		visitTesto(testo->testo1, parametri, sigle);
		fprintf(parametri->outFile, "\\\\\n");
		printCasi(testo->testo2, parametri, sigle);
	}
}

// FUNZIONE DI STAMPA DEI CASES
void printCases(short key, struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{

	fprintf(parametri->outFile, "\\(");
	fprintf(parametri->outFile, "\\left");
	if (strcmp(testo->lexema, "\\Rcases") == 0 ||
		strcmp(testo->lexema, "\\RLcases") == 0)
	{
		fprintf(parametri->outFile, "\\langle ");
	}
	if (strcmp(testo->lexema, "\\Lcases") == 0 ||
		strcmp(testo->lexema, "\\Lbracecases") == 0 ||
		strcmp(testo->lexema, "\\Voidcases") == 0)
	{
		fprintf(parametri->outFile, ".");
	}
	if (strcmp(testo->lexema, "\\Rbracecases") == 0 ||
		strcmp(testo->lexema, "\\RLbracecases") == 0)
	{
		fprintf(parametri->outFile, "\\{");
	}
	fprintf(parametri->outFile, "\n\\begin{tabular}{l}\n");
	printCasi(testo->testo2, parametri, sigle);
	fprintf(parametri->outFile, "\\end{tabular}");
	fprintf(parametri->outFile, "\\right ");
	if (strcmp(testo->lexema, "\\Rcases") == 0 ||
		strcmp(testo->lexema, "\\Rbracecases") == 0 ||
		strcmp(testo->lexema, "\\Voidcases") == 0)
	{
		fprintf(parametri->outFile, ".");
	}
	if (strcmp(testo->lexema, "\\Lcases") == 0 ||
		strcmp(testo->lexema, "\\RLcases") == 0)
	{
		fprintf(parametri->outFile, "\\rangle ");
	}
	if (strcmp(testo->lexema, "\\Lbracecases") == 0 ||
		strcmp(testo->lexema, "\\RLbracecases") == 0)
	{
		fprintf(parametri->outFile, "\\}");
	}
	fprintf(parametri->outFile, "\\)");
}

void printTagDES(FILE *outFile, bool inizio)
{
	if (inizio)
	{
		fprintf(outFile, "\\tagdes{");
	}
	else
	{
		fprintf(outFile, "}");
	}
}

void printTagED(FILE *outFile, bool inizio)
{
	if (inizio)
	{
		fprintf(outFile, "\\taged{");
	}
	else
	{
		fprintf(outFile, "}");
	}
}

void printTagNota(FILE *outFile, bool inizio)
{
	if (inizio)
	{
		fprintf(outFile, "\\tagid{ ");
	}
	else
	{
		fprintf(outFile, "}");
	}
}

void printTagCit(FILE *outFile, bool inizio)
{
	if (inizio)
	{
		fprintf(outFile, "\\tagcit{");
	}
	else
	{
		fprintf(outFile, "}");
	}
}

void printPhantom(struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{
}

// STAMPA I TITOLI DELLE OPERE
void printTitleWork(struct testo *testo, PARAMETRI *param, struct testo *sigle)
{
	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;
	if (parametri->parStarted)
	{
		fprintf(parametri->outFile, "\\pend");
	}
	parametri->parStarted = true;
	fprintf(parametri->outFile, "\n\\Title{");
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "}");
	fprintf(parametri->outFile, "{");
	if (testo->testo2 != NULL)
	{
		visitTesto(testo->testo2, parametri, sigle);
	}
	fprintf(parametri->outFile, "}");
	parametri->parStarted = false;
}

// FUNZIONE DI STAMPA DELLE FRAZIONI
void printFrac(struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{
	fprintf(parametri->outFile, "\\frac{");
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "}{");
	visitTesto(testo->testo2, parametri, sigle);
	fprintf(parametri->outFile, "}");
}

// FUNZIONE DI STAMPA DELLE FRAZIONI
void printMFrac(short key, struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{
	fprintf(parametri->outFile, "\\mfrac{");
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "}{");
	visitTesto(testo->testo2, parametri, sigle);
	fprintf(parametri->outFile, "}");
}

// FUNZIONE DI STAMPA DELL'ESPONENTE E DEL DEPONENTE
void printSupSub(struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{
	fprintf(parametri->outFile, "\\hbox{$^{\\rm ");
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "}_{\\rm ");
	visitTesto(testo->testo2, parametri, sigle);
	fprintf(parametri->outFile, "}$}");
}

// STAMPA UNA STRINGA DI NOTA DELL'EDITORE
void printNota(const char *nota, FILE *outFile)
{
	// if (testEx == NULL) {
	printTagNota(outFile, true);
	fprintf(outFile, "%s", nota);
	printTagNota(outFile, false);
	//}
}

// STAMPA GLI ACCENTI
void printAccento(const char *lexema, const char *accento, FILE *outFile)
{

	int i; // POSIZIONE DEL CARATTERE ACCENTATO

	// IGNORA L'EVENTUALE PARENTESI O BACKSLASH
	if (lexema[2] == '{' || lexema[2] == '\\')
		i = 3;
	else
		i = 2;

	// STAMPA IL CARATTERE COL SUO ACCENTO
	fprintf(outFile, "{%s%c}", accento, lexema[i]);
}

// STAMPA LE ANCORE DEGLI OMISSIS
void printOM(const char *nota, PARAMETRIm2l *parametri, struct testo *sigle)
{
	printNota(nota, parametri->outFile);
}

// STAMPA I LINK ALLE IMMAGINI
void printImage(char *path, char *alt, FILE *outFile)
{
	fprintf(outFile, "<img src=\"%s\" alt=\"%s\">", path, alt);
}

// STAMPA LA SIGLA DI UN TESTIMONE
void printSigla(const char *sigla, FILE *outFile)
{

	char str[100], c;

	if (strlen(sigla) > 0)
	{
		fprintf(outFile, " ");
		size_t len = strlen(sigla);
		char *tmp = new char[len];
		strncpy(tmp, sigla, len - 1);
		tmp[len - 1] = 0;
		c = sigla[strlen(sigla) - 1];
		switch (c)
		{
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
			sprintf(str, "%s\\textsuperscript{%c}", tmp, c);
			break;
		case '+': // CARATTERI NASCOSTI
			strcpy(str, tmp);
			break;
		default: // STANDARD
			strcpy(str, sigla);
		}
		delete[] tmp;

		// STAMPA DELLA SIGLA FORMATTATA
		printNota(str, outFile); /*
		 // STAMPA DELLA SIGLA NON FORMATTATA
		 printNota(sigla, outFile);*/
	}
}

// STAMPA IL RIFERIMENTO DA UNA LB
void printLb(const char *label,
			 PARAMETRIm2l *parametri,
			 struct testo *sigle,
			 bool duplex,
			 bool multiplex)
{

	// RITROVA L'ETICHETTA
	rif *l = findLabel(label);

	if (!duplex && !multiplex)
	{

		// STAMPA IL "TILDE"
		printNota(" $\\sim$ ", parametri->outFile);
	}

	if (l)
	{

		// SE L'ETICHETTA ESISTE
		if (parametri->nUnit != l->nUnit)
		{

			// SE L'UNIT DELL'ETICHETTA E` DIVERSA DALLA UNIT CORRENTE
			/* printUnitRef(l->nUnit, parametri); */
			fprintf(parametri->outFile, "\\ref{");
			fprintf(parametri->outFile, "%s", label);
			fprintf(parametri->outFile, "} ");
		}

		// SALVA I PARAMETRI
		bool banale = parametri->banale;
		bool greco = parametri->greco;
		bool justify = parametri->justify;

		// IMPOSTA I PARAMETRI PER L'ETICHETTA
		parametri->banale = true;
		parametri->greco = false;
		parametri->justify = true;

		// STAMPA L'ETICHETTA
		visitTesto(l->testo, parametri, sigle);

		// RIPRISTINA I PARAMETRI
		parametri->banale = banale;
		parametri->greco = greco;
		parametri->justify = justify;
	}
}

void printTextGreek(struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{
	visitTesto(testo, parametri, sigle);
}

// FUNZIONE DI STAMPA DELLE MANI
void printManus(struct testo *testo, PARAMETRI *param, struct testo *sigle)
{
	fprintf(param->outFile,
			"\\long\\expandafter\\def\\csname AliaManus%s\\endcsname#1{",
			testo->lexema);
	visitTesto(testo->testo1->testo1, param, sigle);
	struct testo *style = testo->testo2;
	if (style == NULL)
	{
		fprintf(param->outFile, "{#1}");
	}
	else
	{
		if (strlen(style->lexema) == 0)
		{
			fprintf(param->outFile, "{#1}");
		}
		else
		{
			fprintf(param->outFile, "{\\%s #1}", style->lexema);
		}
	}
	visitTesto(testo->testo1->testo2, param, sigle);
	fprintf(param->outFile, "}\n");
}

// FUNZIONE DI STAMPA DI ALIAMANUS
void printAliaManus(struct testo *testo,
					PARAMETRI *parametri,
					struct testo *sigle)
{
	fprintf(parametri->outFile, "\\AliaManus{");
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "}{");
	visitTesto(testo->testo2, parametri, sigle);
	fprintf(parametri->outFile, "}");
}

// STAMPA UN TESTO DI NOTA DELL'EDITORE
void printTestoNota(struct testo *testo,
					PARAMETRIm2l *parametri,
					struct testo *sigle)
{
	printTagNota(parametri->outFile, true);
	visitTesto(testo, parametri, sigle);
	printTagNota(parametri->outFile, false);
}

// STAMPA DES
void printDES(struct testo *testo,
			  PARAMETRIm2l *parametri,
			  struct testo *sigle)
{
	printTagDES(parametri->outFile, true);
	visitTesto(testo, parametri, sigle);
	printTagDES(parametri->outFile, false);
}

// STAMPA ED
void printED(struct testo *testo,
			 PARAMETRIm2l *parametri,
			 struct testo *sigle)
{
	printTagED(parametri->outFile, true);
	visitTesto(testo, parametri, sigle);
	printTagED(parametri->outFile, false);
}

// STAMPA IL CORPO DI UNA VARIANTE
bool printNotaVariante(struct testo *lezione,
					   struct testo *nota,
					   struct testo *sigle,
					   PARAMETRIm2l *parametri,
					   bool duplex,
					   bool multiplex)
{

	bool noNota = false; // INDICATORE DI NOTA VUOTA

	// SALVATAGGIO DEI PARAMETRI
	bool banale = parametri->banale;
	bool greco = parametri->greco;
	bool justify = parametri->justify;

	// IMPOSTAZIONE DEI PARAMETRI PER LA VARIANTE
	parametri->banale = false;
	parametri->greco = false;
	parametri->justify = true;

	// SE LA NOTA (SECONDO SOTTOCAMPO) ESISTE
	if (nota)
	{

		// VERIFICA DEL TIPO DI NOTA
		switch (nota->key)
		{
		case CRKEY:
			visitTesto(lezione, parametri, sigle);
			printLb(nota->lexema, parametri, sigle, duplex, multiplex);
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
			visitTesto(lezione, parametri, sigle);
			fprintf(parametri->outFile, " ");
			printDES(nota->testo1, parametri, sigle);
			break;
		case EDKEY:
			visitTesto(lezione, parametri, sigle);
			fprintf(parametri->outFile, " ");
			printED(nota->testo1, parametri, sigle);
			break;
		case POSTSCRIPTKEY:
			visitTesto(lezione, parametri, sigle);
			break;
		case BISKEY:
			visitTesto(lezione, parametri, sigle);
			printNota("bis", parametri->outFile);
			break;
		case INTERLKEY:
			visitTesto(lezione, parametri, sigle);
			printNota("supra lineam", parametri->outFile);
			break;
		case SUPRAKEY:
			visitTesto(lezione, parametri, sigle);
			printNota("supra lineam", parametri->outFile);
			break;
		case MARGKEY:
			visitTesto(lezione, parametri, sigle);
			printNota("in marg.", parametri->outFile);
			break;
		case MARGSIGNKEY:
			visitTesto(lezione, parametri, sigle);
			printNota("signo posito in marg.", parametri->outFile);
			break;
		case PCKEY:
			visitTesto(lezione, parametri, sigle);
			printNota("post corr.", parametri->outFile);
			break;
		case EXKEY:
			visitTesto(lezione, parametri, sigle);
			printNota("ex ", parametri->outFile);
			visitTesto(nota->testo1, parametri, sigle);
			break;
		case REPKEY:
			visitTesto(lezione, parametri, sigle);
			switch (*nota->lexema)
			{
			case '1':
				printNota("priore loco", parametri->outFile);
				break;
			default:
				printNota("altero loco", parametri->outFile);
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
	}
	else
	{
		noNota = true;
	}

	// RIPRISTINA I PARAMETRI
	parametri->banale = banale;
	parametri->greco = greco;
	parametri->justify = justify;

	// RITORNA L'INDICATORE DI NOTA VUOTA
	return noNota;
}

bool printNoteVariante(struct testo *lezione,
					   struct testo *note,
					   struct testo *sigle,
					   PARAMETRIm2l *parametri,
					   bool duplex,
					   bool multiplex)
{

	bool noNota = true;

	if (note)
	{
		noNota = printNotaVariante(lezione, note->testo1, sigle, parametri, duplex, multiplex);
		if (note->testo1->key != POSTSCRIPTKEY)
		{
			noNota = printNoteVariante(NULL, note->testo2, sigle, parametri, duplex, multiplex) && noNota;
		}
	}

	return noNota;
}

// STAMPA IL CORPO DI UNA VARIANTE
short printCorpoVariante(
	struct testo *lezioneRif,
	struct testo *lezione, // A->testo2->testo1,
	struct testo *note,	   // A->testo2->testo2,
	struct testo *sigle,   // A->testo1,
	// per A che arriva da visitVariante(struct testo * A, etc.)
	PARAMETRI *param,
	bool duplex,
	bool multiplex,
	bool nested,
	int nVar)
{
	bool noNota = true; // INDICATORE DI NOTA VUOTA
	short nTest;
	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;
	parametri->numero = nVar;
	if (note)
	{
		noNota = printNoteVariante(lezione, note, sigle, parametri, duplex, multiplex);
	}
	else
	{
		if (parametri->math)
		{
			fprintf(parametri->outFile, "$");
		}
		visitTesto(lezione, parametri, sigle);
		if (parametri->math)
		{
			fprintf(parametri->outFile, "$");
		}
	}
	// STAMPA DELLE SIGLE
	nTest = visitSigle(sigle, noNota, parametri->outFile);
	// STAMPA EVENTUALI POSTSCRIPT
	visitPostscript(sigle, note, parametri, false);
	// RITORNA IL NUMERO DI TESTIMONI
	return nTest;
}

// STAMPA I POSTSCRIPT DI UNA VARIANTE
void printPostscript(struct testo *sigle, struct testo *nota, PARAMETRI *param)
{

	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;
	printNotaVariante(NULL, nota, sigle, parametri, false, false);

	/*
	  if (nota->key == POSTSCRIPTKEY) {
	  fprintf(parametri->outFile, " ");
	  printTestoNota(nota->testo1, parametri, sigle);
	  }
	*/
}

// STAMPA IL TESTO CRITICO
void printTestoCritico(struct testo *testo,
					   PARAMETRIm2l *parametri,
					   struct testo *sigle)
{
	bool banale = parametri->banale;
	parametri->banale = true;
	visitTesto(testo->testo1->testo2->testo1, parametri, sigle);
	parametri->banale = banale;
}

void printOptions(struct testo *testo, FILE *outFile, bool sep)
{
	if (testo)
	{
		if (sep && testo->lexema[0] != '=')
		{
			fprintf(outFile, ", ");
		}
		if (strcmp(testo->lexema, "mauro") == 0)
		{
			fprintf(outFile, "mauroreledmac");
		}
		else
		{
			fprintf(outFile, "%s", testo->lexema);
		}
		printOptions(testo->testo2, outFile, testo->lexema[0] != '=');
	}
}

// FUNZIONI GENERICHE DEFINITE IN MVISIT

// FUNZIONE DI STAMPA DEI FOLIA
void printFoliumCommon(const char *testimone, const char *pagina, PARAMETRI *parametri)
{
	if (testimone != NULL)
	{
		fprintf(parametri->outFile, "%s:", testimone);
	}
	fprintf(parametri->outFile, "%s", pagina);
}

void printFolium(const char *testimone, const char *pagina, PARAMETRI *param, struct testo *sigle)
{
	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;
	if (parametri->nota) {
		if (parametri->numero > 1) {
			fprintf(parametri->outFile, "[");
			printFoliumCommon(testimone, pagina, parametri);
			fprintf(parametri->outFile, "]");
		}
	} else {
		startParagraph(parametri);
		fprintf(parametri->outFile, "\\Folium{");
		printFoliumCommon(testimone, pagina, parametri);
		fprintf(parametri->outFile, "}");
	}
}

// FUNZIONE DI STAMPA DELLE DATE
void printDate(struct testo *testo, PARAMETRI *param, struct testo *sigle)
{
	startParagraph((PARAMETRIm2l *)param);
	visitTesto(testo->testo1, param, sigle);
}

// FUNZIONE DI STAMPA DELLE CITAZIONI
void printCit(struct testo *testo, PARAMETRI *param, struct testo *sigle)
{
	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;
	short iConf = -1;
	const char *libro = NULL;
	const char *prop = NULL;
	if (!parametri->nota)
	{
		fprintf(parametri->outFile, "\\edtext{");
		parametri->noPar = true;
	}
	visitTesto(testo->testo1, parametri, sigle);
	if (!parametri->nota)
	{
		fprintf(parametri->outFile, "}");
		fprintf(parametri->outFile, "{");
		fprintf(parametri->outFile, "\\footnotecit{");
		// STAMPA DELL'OPZIONE
		if (testo->lexema != NULL)
		{
			printNota(testo->lexema, parametri->outFile);
			if (strlen(testo->lexema) > 0)
			{
				fprintf(parametri->outFile, " ");
			}
		}
		if (testo->testo2)
		{
			trovaRiferimento(testo->testo2->testo1, &iConf, &libro, &prop);
			if (iConf >= 0)
			{
				// STAMPA RIFERIMENTO DELLA CITAZIONE
				printTagNota(parametri->outFile, true);
				printRiferimento(parametri, iConf, libro, prop);
				printTagNota(parametri->outFile, false);

				// CONDIZIONE PER L'ESCLUSIONE DELLA STAMPA CITAZIONE
				if (!exclude)
				{
					// STAMPA LA CITAZIONE ALL'OPERA
					stampaCitazioneOpera(parametri, libro, prop);
				}
			}
			else
			{
				printTagCit(parametri->outFile, true);
				visitTesto(testo->testo2->testo1, parametri, sigle);
				printTagCit(parametri->outFile, false);
			}

			// COMMENTO DELLA CITAZIONE
			if (testo->testo2->testo2)
			{
				fprintf(parametri->outFile, " ");
				printTestoNota(testo->testo2->testo2, parametri, sigle);
			}
		}
		fprintf(parametri->outFile, "}");
	}
	if (!parametri->nota)
	{
		fprintf(parametri->outFile, "}");
		parametri->noPar = false;
	}
}

void printEnv(PARAMETRI *param, const char *env, bool inizio)
{
	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;
	if (inizio)
	{
		if (parametri->parStarted)
		{
			fprintf(parametri->outFile, "\\pend");
			parametri->parStarted = false;
		}
		fprintf(parametri->outFile, "\n\\begin{%s}\n", env);
	}
	else
	{
		fprintf(parametri->outFile, "\n\\end{%s}\n", env);
	}
}

void printMarkup(const char *macro,
				 const char *style,
				 struct testo *testo,
				 PARAMETRIm2l *parametri,
				 struct testo *sigle)
{
	struct testo *lezione = testo->testo1;
	struct testo *descrizione = testo->testo2;
	if (style != NULL)
	{
		fprintf(parametri->outFile, "\\%s{", style);
	}
	visitTesto(lezione, parametri, sigle);
	if (style != NULL)
	{
		fprintf(parametri->outFile, "}");
	}
	if (!checkDescrizione(descrizione))
	{
		addDescrizione(descrizione);
		fprintf(parametri->outFile, "\\%s{", macro);
		visitTesto(descrizione, parametri, sigle);
		fprintf(parametri->outFile, "}");
	}
}

// FUNZIONE DI STAMPA DI UNA MACRO GENERICA
void printKey(bool inizio,
			  short key,
			  long ln,
			  struct testo *testo,
			  PARAMETRI *param,
			  struct testo *sigle)
{
	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;
	if (parametri->label == NULL || parametri->nota)
	{
		switch (key)
		{
		default:
			break;
		case LUOGOKEY:
			if (inizio)
			{
				visitTesto(testo->testo1, parametri, sigle);
			}
			break;
		case OPERAKEY:
			if (inizio)
			{
				printMarkup("footnotenam", NULL, testo, parametri, sigle);
			}
			break;
		case NOMEKEY:
			if (inizio)
			{
				printMarkup("footnotenam", NULL, testo, parametri, sigle);
			}
			break;
		case EXTERNALDOCUMENT:
			if (inizio)
			{
				fprintf(parametri->outFile, "{");
				visitTesto(testo, parametri, sigle);
				fprintf(parametri->outFile, "}");
			}
			break;
		case HLINEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\hline");
			}
			break;
		case CLINEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\rule{1cm}{1pt}");
			}
			break;
		case LEFTGRATABKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\{");
			}
			break;
		case RIGHTGRATABKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\}");
			}
			break;
		case LEFTANGTABKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "$<$");
			}
			break;
		case RIGHTANGTABKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "$>$");
			}
			break;
		case CONGKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\textbf{[[}");
			}
			else
			{
				fprintf(parametri->outFile, "\\textbf{]]}");
			}
			break;
		case COMMENTKEY:
			if (inizio)
			{
				if (parametri->parStarted)
				{
					fprintf(parametri->outFile, "\n\\pend\n");
					parametri->parStarted = false;
				}
				if (!parametri->endnumbering)
				{
					fprintf(parametri->outFile, "\n\\endnumbering\n");
					parametri->endnumbering = true;
				}
				fprintf(parametri->outFile, "\\Commenti");
			}
			break;
		case ANNOTAZKEY:
			if (inizio)
			{
				if (parametri->parStarted)
				{
					fprintf(parametri->outFile, "\n\\pend\n");
					parametri->parStarted = false;
				}
				if (!parametri->endnumbering)
				{
					fprintf(parametri->outFile, "\n\\endnumbering\n");
					parametri->endnumbering = true;
				}
				fprintf(parametri->outFile, "\\Annotazioni");
			}
			break;
		case COMMKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\pstart\\Comm{");
				visitTesto(testo, parametri, sigle);
				fprintf(parametri->outFile, "}");
			}
			break;
		case GREEKKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\GG{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case SETLENGTH:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\setlength");
			}
			break;
		case MAKETITLEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\maketitle");
			}
			break;
		case PLKEY:
			printTagNota(parametri->outFile, inizio);
			break;
		case CLASSKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\documentclass");
				if (testo && strcmp(testo->lexema, "") != 0)
				{
					fprintf(parametri->outFile, "[");
					printOptions(testo, parametri->outFile, false);
					fprintf(parametri->outFile, "]");
				}
				parametri->noPar = true;
			}
			break;
		case DOCENVKEY:
			if (inizio)
			{
				visitTesto(testo->testo1, parametri, sigle);
				fprintf(parametri->outFile, "\n\\begin{document}\n");
				fprintf(parametri->outFile, "\\beginnumbering");
				parametri->parStarted = false;
				parametri->noPar = false;
				visitTesto(testo->testo2, parametri, sigle);
				endParagraph(parametri);
				fprintf(parametri->outFile, "\\clearpage\n");
				if (!parametri->endnumbering)
				{
					fprintf(parametri->outFile, "\n\\endnumbering\n");
				}
				fprintf(parametri->outFile, "\\end{document}\n");
			}
			break;
		case USEPACKAGE:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\usepackage");
				if (testo->testo1)
				{
					fprintf(parametri->outFile, "[");
					printOptions(testo->testo1, parametri->outFile, false);
					fprintf(parametri->outFile, "]");
				}
				fprintf(parametri->outFile, "{");
				if (strcmp(testo->testo2->lexema, "mauro") == 0)
				{
					fprintf(parametri->outFile, "mauroreledmac");
				}
				else
				{
					printOptions(testo->testo2, parametri->outFile, false);
				}
				fprintf(parametri->outFile, "}\n");
			}
			break;
		case GEOMETRY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\geometry{");
				printOptions(testo, parametri->outFile, false);
				fprintf(parametri->outFile, "}\n");
			}
			break;
		case MANUSKEY:
			break;
		case FOLIUMMARGOPT:
			if (inizio)
			{
				foliummarg = true;
			}
			break;
		case FOLIUMTESTOOPT:
			if (inizio)
			{
				foliumtesto = true;
			}
			break;
		case UNITKEY:
			if (inizio)
			{
				printUnitRef(parametri->nUnit, parametri);
			}
			break;
		case LBKEY:
			if (inizio)
			{
			}
			else
			{
				fprintf(parametri->outFile, "\\label{%s}", testo->lexema);
			}
			break;
		case CAPITOLOKEY:
			if (inizio)
			{
				endParagraph(parametri);
				fprintf(parametri->outFile, "\n\\Capitolo{");
				parametri->noPar = true;
			}
			else
			{
				fprintf(parametri->outFile, "}");
				parametri->noPar = false;
			}
			break;
		case SOTTOCAPITOLOKEY:
			if (inizio)
			{
				endParagraph(parametri);
				fprintf(parametri->outFile, "\n\\Sottocapitolo{");
				parametri->noPar = true;
			}
			else
			{
				fprintf(parametri->outFile, "}");
				parametri->noPar = false;
			}
			break;
		case TEOREMAKEY:
			if (inizio)
			{
				endParagraph(parametri);
				fprintf(parametri->outFile, "\n\\Teorema{");
				parametri->noPar = true;
			}
			else
			{
				fprintf(parametri->outFile, "}");
				parametri->noPar = false;
			}
			break;
		case ALITERKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\Aliter{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case LEMMAKEY:
			if (inizio)
			{
				parametri->noPar = true;
				fprintf(parametri->outFile, "\\Lemma{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
				parametri->noPar = false;
			}
			break;
		case COROLLARIOKEY:
			if (inizio)
			{
				if (parametri->parStarted)
				{
					fprintf(parametri->outFile, "\\pend");
				}
				parametri->parStarted = true;
				fprintf(parametri->outFile, "\\Corollario{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
				parametri->parStarted = false;
			}
			break;
		case SCOLIOKEY:
			if (inizio)
			{
				if (parametri->parStarted)
				{
					fprintf(parametri->outFile, "\\pend");
				}
				parametri->parStarted = true;
				fprintf(parametri->outFile, "\\Scolio{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
				parametri->parStarted = false;
			}
			break;
		case ADDITIOKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\Additio{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case SECTIONKEY:
			if (inizio)
			{
				descrizioni = NULL;
				fprintf(parametri->outFile, "\\section*{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case SUBSECTIONKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\subsection*{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case SSSECTIONKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\subsubsection*{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case PARAGRAPHKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\paragraph{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case LABELKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\label{%s}", testo->lexema);
			}
			break;
		case LETTERLABELKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\letterlabel{%s}{%s}", testo->lexema, testo->testo1->lexema);
			}
			break;
		case CITEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\cite{%s}", testo->lexema);
			}
			break;
		case MCITEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\maurocite{%s}{", testo->lexema);
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case REFKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\ref{");
				visitTesto(testo, parametri, sigle);
				fprintf(parametri->outFile, "}");
			}
			break;
		case PAGEREFKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\pageref{");
				visitTesto(testo, parametri, sigle);
				fprintf(parametri->outFile, "}");
			}
			break;
		case ':':
			if (inizio)
			{
				fprintf(parametri->outFile, "%c", ':');
			}
			break;
		case OMKEY:
			if (inizio)
			{
				printOM("om.", parametri, sigle);
			}
			break;
		case DELKEY:
			if (inizio)
			{
				printOM("del.", parametri, sigle);
			}
			break;
		case EXPKEY:
			if (inizio)
			{
				printOM("exp.", parametri, sigle);
			}
			break;
		case OMLACKEY:
			if (inizio)
			{
				printOM("spatio relicto om.", parametri, sigle);
			}
			break;
		case NLKEY:
			if (inizio)
			{
				printNota("non legitur", parametri->outFile);
			}
			break;
		case LACMKEY:
		case LACSKEY:
		case LACCKEY:
			if (inizio)
			{
				if (!parametri->nota)
				{
					fprintf(parametri->outFile, "***");
				}
			}
			break;
		case BEGINEQNKEY:
			fprintf(parametri->outFile, "$$");
			if (!inizio)
			{
				printCodaVV(parametri, sigle);
			}
			break;
		case BEGINMATHKEY:
			startParagraph(parametri);
			fprintf(parametri->outFile, "$");
			if (!inizio)
			{
				printCodaVV(parametri, sigle);
			}
			break;
		case SLANTED:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\textsl{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case EMPHASIS:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\emph{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case TITKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\Tit{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case ITALIC:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\textit{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case CORRKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\CORR{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case PERSKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "[");
			}
			else
			{
				fprintf(parametri->outFile, "]");
			}
			break;
		case STRIKEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "$\\overline{\\hbox{");
			}
			else
			{
				fprintf(parametri->outFile, "}}$");
			}
			break;
		case POWERKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "^{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case INDEXKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "_{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case SUPKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\textsuperscript{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case SUBKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\textsubscript{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case TILDEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "~");
			}
			break;
		case DOTSKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "{\\dots}");
			}
			break;
		case TIMESKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "{\\times}");
			}
			break;
		case DSKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "{\\DB}");
			}
			break;
		case SSKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "{\\SB}");
			}
			break;
		case NBSPACEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\,");
			}
			break;
		case FLATKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\flat");
			}
			break;
		case NATURALKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\natural");
			}
			break;
		case CDOTKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\cdot");
			}
			break;
		case VERTKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\vert");
			}
			break;
		case LBRACKETKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\lbracket");
			}
			break;
		case RBRACKETKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\rbracket");
			}
			break;
		case SKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\S");
			}
			break;
		case SLASH:
			if (inizio)
			{
				fprintf(parametri->outFile, "/");
			}
			break;
		case GENERICENV:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\begin{%s}", testo->lexema);
				if (testo->testo1 != NULL)
				{
					fprintf(parametri->outFile, "[%s]", testo->testo1->lexema);
				}
			}
			else
			{
				fprintf(parametri->outFile, "\\end{%s}", testo->lexema);
			}
			break;
		case NOINDENTKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\noindent");
			}
			break;
		case NEWPAGEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\newpage");
			}
			break;
		case BEGINEPISTOLAKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\inizioepistola");
				resetDescrizioni();
			}
			break;
		case ENDEPISTOLAKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\fineepistola");
			}
			break;
		case VSPACEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\vspace{");
				visitTesto(testo, parametri, sigle);
				fprintf(parametri->outFile, "}");
			}
			break;
		case HYPHENKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\-");
			}
			break;
		case TITMARG:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\begin{flushright}\n");
			}
			else
			{
				fprintf(parametri->outFile, "\\end{flushright}");
			}
			break;
		case TEXTGREEK:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\textgreek{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case ENUNCIATIO:
			if (inizio)
			{
				fprintf(parametri->outFile, "\n\\begin{large}\n");
				fprintf(parametri->outFile, "\\begingroup\\centering\\pstart\n");
				parametri->parStarted = true;
			}
			else
			{
				fprintf(parametri->outFile, "\n\\pend\\endgroup\n");
				fprintf(parametri->outFile, "\\end{large}");
				parametri->parStarted = false;
			}
			break;
		case ITEMKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\n\\item");
				if (testo != NULL)
				{
					fprintf(parametri->outFile, "[");
					visitTesto(testo, parametri, sigle);
					fprintf(parametri->outFile, "]");
				}
			}
			break;
		case QUOTE:
			printEnv(parametri, "quote", inizio);
			break;
		case ITEMIZE:
			printEnv(parametri, "itemize", inizio);
			break;
		case ENUMERATE:
			printEnv(parametri, "enumerate", inizio);
			break;
		case DESCRIPTION:
			printEnv(parametri, "description", inizio);
			break;
		case PROTASIS:
			if (inizio)
			{
				if (parametri->parStarted)
				{
					fprintf(parametri->outFile, "\\pend");
				}
				parametri->parStarted = true;
				fprintf(parametri->outFile, "\n\\begin{protasis}");
			}
			else
			{
				fprintf(parametri->outFile, "\\end{protasis}\n");
				parametri->parStarted = false;
			}
			break;
		case CENTERKEY:
			if (inizio)
			{
				endParagraph(parametri);
				fprintf(parametri->outFile, "\n\\begingroup\\centering\\pstart\n");
				parametri->parStarted = true;
			}
			else
			{
				fprintf(parametri->outFile, "\n\\pend\\endgroup\n");
				parametri->parStarted = false;
			}
			break;
		case PARAGRAPH:
			if (inizio)
			{
				endParagraph(parametri);
			}
			break;
		case PARAGRAPHEND:
			if (inizio)
			{
				if (parametri->parStarted)
				{
					fprintf(parametri->outFile, "\\pend");
					parametri->parStarted = false;
				}
			}
			break;
		case PARAGRAPHSTART:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\pstart");
				parametri->parStarted = true;
			}
			break;
		case TITLEKEY:
			if (inizio)
			{
				if (parametri->parStarted)
				{
					fprintf(parametri->outFile, "\\pend");
					parametri->parStarted = false;
				}
				fprintf(parametri->outFile, "\\title{");
				parametri->parStarted = true;
			}
			else
			{
				fprintf(parametri->outFile, "}");
				parametri->parStarted = false;
			}
			break;
		case TITLEDATEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\date{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case ROMAN:
			if (inizio)
			{
				fprintf(parametri->outFile, "{\\rm ");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case BOLDFACE:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\textbf{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case SMALLCAPSKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\textsc{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case TYPEFACE:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\texttt{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case SPACES:
		case NEWLINE:
			if (inizio)
			{
				if (nSpaces < 10)
				{
					nSpaces++;
					fprintf(parametri->outFile, " ");
				}
				else
				{
					nSpaces = 0;
					fprintf(parametri->outFile, "\n");
				}
			}
			break;
		case MAUROTEXKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "{\\MauroTeX}");
			}
			break;
		case MTEXKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "{\\MTeX}");
			}
			break;
		case PERKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "{\\\"p}");
			}
			break;
		case CONKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "{\\\"9}");
			}
			break;
		case TEXKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "{\\TeX}");
			}
			break;
		case LATEXKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "{\\LaTeX}");
			}
			break;
		case SLQUOTEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "`");
			}
			break;
		case SRQUOTEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "'");
			}
			break;
		case DLQUOTEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "``");
			}
			break;
		case DRQUOTEKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "''");
			}
			break;
		case ADKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "---");
			}
			break;
		case LONGKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "--");
			}
			break;
		case VECTORKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\=");
			}
			break;
		case CONTRKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "$\\widetilde\\textrm{");
			}
			else
			{
				fprintf(parametri->outFile, "}$");
			}
			break;
		case CRUXKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\CRUX{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case EXPUKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\EXPU{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case INTEKEY:
		case LEFTANG:
			if (inizio)
			{
				startParagraph(parametri);
				fprintf(parametri->outFile, "\\INTE{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case BIBRIFKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "{[");
			}
			else
			{
				fprintf(parametri->outFile, "]}");
			}
			break;
		case CITMARGKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\CITMARG{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case CITMARGSIGNKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\CITMARGSIGN{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case UNDERLINE:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\UNDERLINE{");
			}
			else
			{
				fprintf(parametri->outFile, "}");
			}
			break;
		case LEFTSQUARE:
			if (inizio)
			{
				fprintf(parametri->outFile, "[");
			}
			else
			{
				fprintf(parametri->outFile, "]");
			}
			break;
		case LEFTPAR:
			if (inizio)
			{
				fprintf(parametri->outFile, "(");
			}
			break;
		case RIGHTPAR:
			if (inizio)
			{
				fprintf(parametri->outFile, ")");
			}
			break;
		case HREFKEY:
			if (inizio)
			{
				visitTesto(testo->testo2, parametri, sigle);
			}
			break;
		case DAGKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "\\{");
				visitTesto(testo->testo2, parametri, sigle);
				fprintf(parametri->outFile, "\\}");
			}
			break;
			// case NOTAMACRO:
		case OUTRANGE:
			if (inizio)
			{
				fprintf(parametri->outFile, "??");
			}
			break;
		case NOKEY:
			if (testo)
			{
				if (inizio)
				{
					fprintf(parametri->outFile, "{");
				}
				else
				{
					fprintf(parametri->outFile, "}");
				}
			}
			break;
		}
	}
}

// FUNZIONE DI STAMPA DI UNA FIGURA
void printFig(short key, struct testo *testo, PARAMETRI *param)
{
	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;
	// APERTURA
	bool parStarted = parametri->parStarted;
	if (key == FMARGKEY)
	{
		fprintf(parametri->outFile, "\\marginpar{");
	}
	else
	{
		if (parametri->parStarted)
		{
			fprintf(parametri->outFile, "\\pend");
		}
		parametri->parStarted = true;
		parStarted = false;
		fprintf(parametri->outFile, "\n\\pausenumbering\n");
		fprintf(parametri->outFile, "\\begin{figure}[h]\n");
		fprintf(parametri->outFile, "\\centering\n");
	}
	// TRATTAMENTO SPECIFICO
	switch (key)
	{
	case FIGURAKEY:
		fprintf(parametri->outFile, "\n");
		fprintf(parametri->outFile, "\\includegraphics");
		parametri->noPar = true;
		//sintassi alternativa per \Figura{{k}} e \\Figura{k}
		if (testo->lexema != NULL) { //OLD SYNTAX
			fprintf(parametri->outFile, "{");
			printLexema(key,testo->lexema,parametri);
			fprintf(parametri->outFile, "}");
		} else {
			visitTesto(testo->testo2, parametri, NULL);
		}
		parametri->noPar = false;
		break;
	case FIGSKIPKEY:
		fprintf(parametri->outFile, "\\fbox{\\rule{2cm}{0cm}\\rule{0cm}{");
		//sintassi alternativa per \Figskip{{k}} e \\Figskip{k}
		if (testo->lexema != NULL) { //OLD SYNTAX
			printLexema(key, testo->lexema, parametri);
			visitTesto(testo->testo1, parametri, NULL);
			fprintf(parametri->outFile, "}figura\\rule{2cm}{0cm}}\n");
		} else {
			visitTesto(testo->testo2->testo1, parametri, NULL);
			fprintf(parametri->outFile, "}figura\\rule{2cm}{0cm}}\n");
			visitTesto(testo->testo2->testo2, parametri, NULL);
		}
		break;
	case FMARGKEY:
		fprintf(parametri->outFile, "Fig.{");
		visitTesto(testo->testo2, parametri, NULL);
		fprintf(parametri->outFile, "}\n");
		break;
	case FORMULAKEY:
		fprintf(parametri->outFile, "\\begin{large}[Formula ");
		visitTesto(testo->testo2, parametri, NULL);
		fprintf(parametri->outFile, "]\\end{large}");
		break;
	case TAVOLAKEY:
		fprintf(parametri->outFile, "\\begin{large}[Tavola ");
		visitTesto(testo->testo2, parametri, NULL);
		fprintf(parametri->outFile, "]\\end{large}");
		break;
	}
	// DIDASCALIA
	visitTesto(testo->testo1, parametri, NULL);

	// CHIUSURA
	if (key == FMARGKEY)
	{
		fprintf(parametri->outFile, "}");
	}
	else
	{
		fprintf(parametri->outFile, "\n\\end{figure}\n");
		fprintf(parametri->outFile, "\\resumenumbering\n");
	}
	parametri->parStarted = parStarted;
}

// FUNZIONE DI STAMPA DI UN RIFERIMENTO AD UNA UNIT
void printUnitRef(const char *opera, const char *etichetta, PARAMETRI *parametri)
{
	fprintf(parametri->outFile, "\\ref{");
	if (opera != NULL && strlen(opera) > 0)
	{
		fprintf(parametri->outFile, "%s_", opera);
	}
	fprintf(parametri->outFile, "%s", etichetta);
	fprintf(parametri->outFile, "}");
}

/* void printUnitRef(short nUnit, PARAMETRI * parametri) { */
void printUnitRef(short nUnit, PARAMETRI *param)
{
	/* fprintf(parametri->outFile, "\n\\textlatin{\\small\\textbf{%d}}", nUnit); */

	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;

	if (!parametri->parStarted)
	{
		fprintf(parametri->outFile, "\\pstart");
		parametri->parStarted = true;
	}
	fprintf(parametri->outFile, "\\Unit");
}

// FUNZIONE DI STAMPA DI UNA PAROLA
void printLexema(short key, const char *lexema, PARAMETRI *param)
{
	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;
	if (lexema != NULL)
	{
		if (parametri->label == NULL || parametri->nota)
		{
			if (!parametri->parStarted && !parametri->noPar)
			{
				fprintf(parametri->outFile, "\\pstart\n");
				parametri->parStarted = true;
			}
			switch (key)
			{
			case TEXTCIRCLEDKEY:
				fprintf(parametri->outFile, "\\textcircled{%s}", lexema);
				break;
				// DOCUMENTCLASS
			case CLASSKEY:
				fprintf(parametri->outFile, "{%s}\n", lexema);
				break;
				// ACCENTI
			case GRAVE:
				printAccento(lexema, "\\`", parametri->outFile);
				break;
			case ACUTE:
				printAccento(lexema, "\\'", parametri->outFile);
				break;
			case UML:
				printAccento(lexema, "\\\"", parametri->outFile);
				break;
			case CIRC:
				printAccento(lexema, "\\^", parametri->outFile);
				break;
				// SIMBOLI E MACRO
			case PARSKIP:
				fprintf(parametri->outFile, "\\parskip{%s}\n", lexema);
				break;
			case EXTERNALDOCUMENT:
				fprintf(parametri->outFile, "\\externaldocument[%s]", lexema);
				break;
			case UNITKEY:
				fprintf(parametri->outFile, "\\label{%s}", lexema);
				break;
			case PARINDENT:
				fprintf(parametri->outFile, "\\parindent{%s}\n", lexema);
				break;
			case PRELOADKEY:
				fprintf(parametri->outFile, "\\PreloadUnicodePage{%s}\n", lexema);
				break;
			case SYMBOL:
			case MACRO:
				startParagraph(parametri);
				fprintf(parametri->outFile, "{%s}", lexema);
				break;
			default:
				fprintf(parametri->outFile, "%s", lexema);
				break;
			}
		}
	}
	fflush(parametri->outFile);
}

// FUNZIONI DI STAMPA DI UNA LISTA DI AUTORI
void internalPrintAuthors(struct testo *testo,
						  PARAMETRI *parametri,
						  struct testo *sigle)
{
	visitTesto(testo->testo1, parametri, sigle);
	if (testo->testo2 != NULL)
	{
		fprintf(parametri->outFile, ";");
		internalPrintAuthors(testo->testo2, parametri, sigle);
	}
}
void printAuthors(struct testo *testo,
				  PARAMETRI *parametri,
				  struct testo *sigle)
{
	fprintf(parametri->outFile, "\\author{");
	internalPrintAuthors(testo, parametri, sigle);
	fprintf(parametri->outFile, "}");
}

// FUNZIONE DI STAMPA DEL FRONTESPIZIO
void printFrontespizio(struct testo *testo,
					   PARAMETRI *parametri,
					   struct testo *sigle)
{
	fprintf(parametri->outFile, "\\frontespizio{");
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "}{");
	visitTesto(testo->testo2->testo1, parametri, sigle);
	fprintf(parametri->outFile, "}{");
	internalPrintAuthors(testo->testo2->testo2, parametri, sigle);
	fprintf(parametri->outFile, "}");
}

// FUNZIONE DI STAMPA DI UNA ANNOTAZIONE
void printFootnote(struct testo *testo,
				   PARAMETRI *parametri,
				   struct testo *sigle)
{
	fprintf(parametri->outFile, "\\footnote{");
	visitTesto(testo, parametri, sigle);
	fprintf(parametri->outFile, "}");
}

// FUNZIONE DI STAMPA DI UN COMMENTO
void printCommento(struct testo *testo, PARAMETRI *param, struct testo *sigle)
{
	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;
	if (parametri->parStarted)
	{
		fprintf(parametri->outFile, "\n\\pend\n");
	}
	parametri->parStarted = true;
	fprintf(parametri->outFile, "\n\\pstart\\Comm{");
	visitTesto(testo, parametri, sigle);
	fprintf(parametri->outFile, "}");
}

// FUNZIONE DI STAMPA DI UNA ANNOTAZIONE
void printAdnotatio(
	char const *label,
	struct testo *testo,
	PARAMETRI *parametri,
	struct testo *sigle)
{
	fprintf(parametri->outFile, "\\Adnotatio{");
	visitTesto(testo, parametri, sigle);
	fprintf(parametri->outFile, "}");
}

// FUNZIONE DI MARCATURA DEL TESTO CRITICO
void printInizioTestoCritico(short key, PARAMETRI *param)
{
	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;
	if (!parametri->banale && !parametri->nota && (key != VBKEY || opzioneBanale()))
	{
		startParagraph(parametri);
		fprintf(parametri->outFile, "\\edtext{");
	}
}

void printFineTestoCritico(short key, PARAMETRI *parametri)
{
	if (!parametri->banale && !parametri->nota && (key != VBKEY || opzioneBanale()))
	{
		fprintf(parametri->outFile, "}");
	}
}

// FUNZIONE DI MARCATURA DELLE VARIANTI
void printInizioVarianti(PARAMETRI *parametri)
{
	if (!parametri->nota)
	{
		if (parametri->math)
		{
			fprintf(parametri->outFile, "{");
			fprintf(parametri->outFile, "\\footnotemarkvvmath{");
		}
		else
		{
			fprintf(parametri->outFile, "{");
			fprintf(parametri->outFile, "\\footnotevv{");
		}
	}
}
void printSepVarianti(PARAMETRI *parametri)
{
	fprintf(parametri->outFile, "\\variantsep ");
}

void printFineVarianti(PARAMETRI *parametri)
{
	if (!parametri->nota)
	{
		fprintf(parametri->outFile, "}");
		fprintf(parametri->outFile, "}");
	}
}

// FUNZIONE CHE ACCODA LE NOTE CHE NON PUO` ESSERE STAMPATA
void accodaVV(short key,
			  struct testo *testo)
{

	if (lastVV)
	{
		lastVV->next = new VV(key, testo);
	}
	else
	{
		firstVV = new VV(key, testo);
		lastVV = firstVV;
		lastVV->next = NULL;
	}
	nCodaVV++;
}

// FUNZIONE PER ABILITARE LA GESTIONE DELLA VV DI MVISIT
bool checkVV(short key,
			 struct testo *testo,
			 PARAMETRI *param,
			 struct testo *sigle)
{
	return true;
}

// STAMPA UNA NOTA PRINCIPALE (NON SOTTONOTA)
void printVVPrincipale(short key,
					   struct testo *testo,
					   PARAMETRI *param,
					   struct testo *sigle)
{
	bool nota = param->nota;
	if (key == SCHOLKEY)
	{
	}
	else
	{
		param->nota = true;
	}
	if (key == NOTEKEY)
	{
		visitTesto(testo, param, sigle);
	}
	else
	{
		visitVarianti(testo, param, sigle, false, false, false, 1, NULL);
	}
	param->nota = nota;
}

// STAMPA LA CODA DELLE NOTE
void printCodaVV(PARAMETRIm2l *parametri, struct testo *sigle)
{
	bool math = parametri->math;
	parametri->math = true;
	if (firstVV)
	{
		fprintf(parametri->outFile, "\\addtocounter{footnotevv}{%d}", -nCodaVV);
		while (firstVV)
		{
			fprintf(parametri->outFile, "\\stepcounter{footnotevv}");
			fprintf(parametri->outFile, "\\footnotetextvv{");
			printVVPrincipale(firstVV->key, firstVV->testo, parametri, sigle);
			fprintf(parametri->outFile, "}");
			firstVV = firstVV->next;
		}
	}
	nCodaVV = 0;
	firstVV = NULL;
	lastVV = NULL;
	parametri->math = math;
}

// FUNZIONE DI STAMPA DI UNA NOTA
void printVV(short key,
			 struct testo *testo,
			 PARAMETRI *param,
			 struct testo *sigle)
{
	// RECUPERO DEI PARAMETRI
	PARAMETRIm2l *parametri = (PARAMETRIm2l *)param;
	// SALVATAGGIO PARAMETRI
	bool nota = parametri->nota;
	if (parametri->nota)
	{
		// STAMPA DI UNA SOTTONOTA
		fprintf(parametri->outFile, " \\subnote{");
		visitVarianti(testo, parametri, sigle, true, false, false, 1, NULL);
		fprintf(parametri->outFile, "}");
	}
	else
	{
		if (key == VBKEY)
		{
			fprintf(parametri->outFile, " (banale) ");
		}
		// STAMPA DI UNA NOTA
		if (parametri->math)
		{
			accodaVV(key, testo);
		}
		else
		{
			printVVPrincipale(key, testo, parametri, sigle);
		}
	}
	// RIPRISTINO DEI PARAMETRI
	parametri->nota = nota;
}

// FUNZIONE DI STAMPA DELL'ALBERO SINTATTICO
void printDocumento(struct testo *testo)
{

	PARAMETRIm2l *parametri;

	if (testo)
	{

		// LETTURA DEL FILE DI CONFIGURAZIONE
		leggiConfigurazione(confFileName);

		latexFileName = new char[strlen(fileName) + 10];
		sprintf(latexFileName, "%s%s", fileName, LATEXEST);
		parametri = new PARAMETRIm2l();
		parametri->setFileName(latexFileName);
		visitTesto(testo, parametri, NULL);
		delete (parametri);
	}
}

// PROGRAMMA PRINCIPALE
int main(int argn, char **argv)
{

	char *fileName = NULL;
	bool ok = true;
	bool help = false;
	vbTovv = false;
	variantePrecedente = false;
	exclude = false;
	foliummarg = false;
	foliumtesto = false;
	testEx = NULL;
	confFileName = NULL;
	nSpaces = 0;
	firstVV = NULL;
	lastVV = NULL;
	nCodaVV = 0;
	verbose = false;

	plugins[0] = new Standard();
	plugins[1] = new Epistolario();

	for (int i = 1; i < argn; i++)
	{

		// CONTROLLO DEGLI ARGOMENTI
		switch (argv[i][0])
		{

			// CONTROLLO LA PRESENZA DI OPZIONI
		case '-':
			switch (argv[i][1])
			{
			case 'p':
				// ABILITAZIONE PLUGIN
				i++;
				loadPlugin(argv[i]);
				break;
				// FILE DI CONFIGURAZIONE
			case 'f':
				i++;
				confFileName = argv[i];
				break;
				// TRASFORMA LE VARIANTI BANALI IN SERIE
			case 'b':
				vbTovv = true;
				break;
			case 'P': // LEZIONE VARIANTE PRECEDENTE SE LEZIONE VUOTA
				variantePrecedente = true;
				break;
			case 'h': // MOSTRA L'HELP
				help = true;
				break;
				// ESTRATTO DI UN TESTIMONE
			case 't':
				i++;
				testEx = argv[i];
				vbTovv = true;
				break;

			case 'v':
				// MESSAGGI DI WARNING
				verbose = true;
				break;

			case 'x':
				// LETTURA DELLA PROPOSIZIONE CITATA
				exclude = true;
				break;
			}
			break;

		default:
			// NOME  DEL FILE DA ANALIZZARE
			if (fileName == NULL)
			{
				fileName = argv[i];
			}
			else
			{
				ok = false;
			}
			break;
		}
	}

	// VERIFICA CHIAMATA CON NOME FILE
	if (fileName == NULL)
	{
		ok = false;
	}

	// AVVIO DELL'ANALISI DEL FILE SE NON E' STATO RICHIESTO L'HELP
	if (ok && !help)
	{

		// CARICAMENTO PLUGIN STANDARD
		if (numPlugin == 0)
		{
			loadPlugin("standard");
		}

		// INVOCAZIONE DELL'ANALIZZATORE SINTATTICO
		parser(fileName);
	}
	else
	{

		// STAMPA L'HELP
		printf("traduttore MauroTeX -> LaTeX (%s)\n", VERSIONSTRING);
		printf("uso: %s [-b] [-h] [-P] [-f <file>] [-t <testimone>] [-v] [-x] <nome_file>\n", argv[0]);
		if (help)
		{
			printf("opzioni:\n");
			printf("  -b             considera le varianti banali come effettive\n");
			printf("  -f <file>      usa <file> come file di configurazione\n");
			printf("  -h             stampa questo messaggio\n");
			printf("  -p <plugin>    abilita <plugin>\n");
			printf("  -P             usa la lezione della variante precedente se la lezione è vuota\n");
			printf("  -t <testimone> genera il file del testo di <testimone>\n");
			printf("  -v             stampa messaggi di avvertimento\n");
			printf("  -x             esclude dal file delle citazioni la stampa delle\n");
			printf("                 proposizioni citate\n");
			printf("\n");
			printf("plugin:\n");
			for (int index = 0; index < NUM_PLUGINS; index++)
			{
				Plugin *plugin = plugins[index];
				printf("\t%s\t%s\n", plugin->name, plugin->description);
			}
		}
	}
}
