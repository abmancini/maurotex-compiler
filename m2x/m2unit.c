#define VERSIONSTRING "versione 1.15b1 del 24 agosto 2022"

/*
file "m2unit.c"
PROGRAMMA PRINCIPALE

m2unit: traduttore da M-TeX a UNIT basato mvisit

eLabor scrl
(ex. Laboratorio Informatico del Consorzio Sociale "Polis")
via Ponte a Pigòieri 8, 56121 Pisa - Italia
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
#include "mparsey.h"
#include "mvisit.h"

#define UNITEST ".unit"

// VARIABILI GLOBALI
static char *unitFileName;
static short nSpaces;

// FUNZIONI PER OPZIONI GESTITE ESTERNAMENTE
bool verboseWarning()
{
	return false;
}

// TRASFORMA UNA VARIANTE BANALE IN UNA SERIA SE RICHIESTO
bool opzioneBanale()
{
	return false;
}

bool opzioneVariantePrecedente()
{
	return false;
}

// RITORNA IL TESTIMONE PER VEDERENE L'ESTRATTO SE RICHIESTO
char *opzioneTestEx()
{
	return NULL;
}

void printVF(struct testo *testo, PARAMETRI *parametri, struct testo *sigle) {}

// FUNZIONE DI STAMPA DELLE MANI
void printManus(struct testo *testo, PARAMETRI *param, struct testo *sigle) {}

// FUNZIONE CHE STAMPA IL PROLOGO DI UNA TABELLA
void printTableStart(int key, struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{
	visitTesto(testo, parametri, sigle);
}

// FUNZIONE CHE STAMPA L'EPILOGO DI UNA TABELLA
void printTableFinish(int key, struct testo *tabella, PARAMETRI *parametri, struct testo *sigle)
{
  	struct testo * testo = tabella->testo1->testo1;
	visitTesto(testo, parametri, sigle);
}

// FUNZIONE CHE STAMPA LO HEADER DI RIGA DI UNA TABELLA
void printRowHeader(short key, PARAMETRI *parametri)
{
	if (key == HLINEKEY)
	{
		visitKey(key, 0, NULL, parametri, NULL);
	}
	else
	{
		// fprintf(parametri->outFile, "<tr>\n");
	}
}

// FUNZIONE CHE STAMPA IL FOOTER DI RIGA DI UNA TABELLA
void printRowFooter(short key, PARAMETRI *parametri)
{
	if (key != HLINEKEY)
	{
		// fprintf(parametri->outFile, "</tr>\n");
	}
}

// FUNZIONE CHE STAMPA LO HEADER DI UNA TABELLA
void printTableHeader(int key, const char *colonne, PARAMETRI *parametri)
{
	// fprintf(parametri->outFile, "\n<tabula>\n");
}

// FUNZIONE CHE STAMPA IL FOOTER DI UNA TABELLA
void printTableFooter(int key, PARAMETRI *parametri)
{
	// fprintf(parametri->outFile, "</tabula>\n");
}

// FUNZIONE CHE STAMPA UN CAMPO DI UNA TABELLA
void printCampo(short nRow,
				short nCol,
				const char *colonne,
				struct testo *campo,
				PARAMETRI *parametri,
				struct testo *sigle)
{

	if (campo != NULL)
	{
		// GESTIONE INIZIALE MULTIROW E MULTICOL
		switch (campo->key)
		{
		case MULTICOLKEY:
			// fprintf(parametri->outFile, " colspan=\"%d\"", atoi(campo->lexema));
			break;
		case MULTIROWKEY:
			// fprintf(parametri->outFile, " rowspan=\"%d\"", atoi(campo->lexema));
			break;
		}
		// fprintf(parametri->outFile, ">");

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
		// fprintf(parametri->outFile, "</td>\n");
	}
}

// ANALISI TESTO DEI CASES
void printCasi(struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{
	if (testo != NULL)
	{
		// fprintf(parametri->outFile, "\n<case>");
		visitTesto(testo->testo1, parametri, sigle);
		// fprintf(parametri->outFile, "</case>");

		printCasi(testo->testo2, parametri, sigle);
	}
}

// FUNZIONE DI STAMPA DEI CASES
void printCases(short key, struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{

	// fprintf(parametri->outFile, "<cases ");

	if (strcmp(testo->lexema, "\\Rcases") == 0)
	{
		// fprintf(parametri->outFile, " type = \"R\" frame = \"bracket\"");
	}

	if (strcmp(testo->lexema, "\\RLcases") == 0)
	{
		// fprintf(parametri->outFile, " type = \"RL\" frame = \"bracket\"");
	}

	if (strcmp(testo->lexema, "\\Lcases") == 0)
	{
		// fprintf(parametri->outFile, " type = \"L\" frame = \"bracket\"");
	}

	if (strcmp(testo->lexema, "\\Lbracecases") == 0)
	{
		// fprintf(parametri->outFile, " type = \"L\" frame = \"brace\"");
	}

	if (strcmp(testo->lexema, "\\Voidcases") == 0)
	{
		// fprintf(parametri->outFile, " type = \"VOID\" frame = \"void\"");
	}

	if (strcmp(testo->lexema, "\\Rbracecases") == 0)
	{
		// fprintf(parametri->outFile, " type = \"R\" frame = \"brace\"");
	}

	if (strcmp(testo->lexema, "\\RLbracecases") == 0)
	{
		// fprintf(parametri->outFile, " type = \"RL\" frame = \"brace\"");
	}

	// fprintf(parametri->outFile, ">");

	printCasi(testo->testo2, parametri, sigle);

	// fprintf(parametri->outFile, "\n</cases>\n");
}

void printPhantom(struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{
}

// STAMPA I TITOLI DELLE OPERE
void printTitleWork(struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{
	/*
	fprintf(parametri->outFile, "<title>");
	visitTesto(testo->testo1, parametri, sigle);
	fprintf(parametri->outFile, "</title>");
	fprintf(parametri->outFile, "<subtitle>");
	if (testo->testo2 != NULL) {
	  visitTesto(testo->testo2, parametri, sigle);
	}
	fprintf(parametri->outFile, "</subtitle>");
	*/
}

// FUNZIONE DI STAMPA DI ALIAMANUS
void printAliaManus(struct testo *testo,
					PARAMETRI *parametri,
					struct testo *sigle)
{
}

// FUNZIONE DI STAMPA DELLE FRAZIONI
void printFrac(struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{

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
	// fprintf(parametri->outFile, "/");
	visitTesto(testo->testo2, parametri, sigle);
}

// FUNZIONE DI STAMPA DELLE FRAZIONI MULTIPLE
void printMFrac(short key, struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{
	visitTesto(testo->testo1, parametri, sigle);
	// fprintf(parametri->outFile, "/");
	visitTesto(testo->testo2, parametri, sigle);
}

// FUNZIONE DI STAMPA DELL'ESPONENTE E DEL DEPONENTE
void printSupSub(struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{

	// fprintf(parametri->outFile, "<expdep>");

	// fprintf(parametri->outFile, "<exp>");
	visitTesto(testo->testo1, parametri, sigle);
	// fprintf(parametri->outFile, "</exp>");

	// fprintf(parametri->outFile, "<dep>");
	visitTesto(testo->testo2, parametri, sigle);
	// fprintf(parametri->outFile, "</dep>");

	// fprintf(parametri->outFile, "</expdep>");
}

// STAMPA GLI ACCENTI
void printAccento(char *lexema, char *accento, FILE *outFile)
{

	// int i; // POSIZIONE DEL CARATTERE ACCENTATO

	// IGNORA L'EVENTUALE PARENTESI O BACKSLASH
	// if (lexema[2] == '{' || lexema[2] == '\\') i = 3; else i = 2;

	// STAMPA IL CARATTERE COL SUO ACCENTO
	// fprintf(outFile, "&%c%s;", lexema[i], accento);
}

// STAMPA LA SIGLA DI UN TESTIMONE
void printSigla(const char *sigla, FILE *outFile)
{
}

// STAMPA UNA STRINGA DI NOTA DELL'EDITORE
void printNota(const char *nota, FILE *outFile)
{
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
	short nTest = 0;
	return nTest;
}

// STAMPA I POSTSCRIPT DI UNA VARIANTE
void printPostscript(struct testo *sigle, struct testo *nota, PARAMETRI *param)
{
}

// FUNZIONI GENERICHE DEFINITE IN MVISIT

void printTextGreek(struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{
	visitTesto(testo, parametri, sigle);
}

void printFolium(const char *testimone, const char *pagina, PARAMETRI *parametri, struct testo *sigle)
{
	// fprintf(parametri->outFile, "<folium page=\"");
	fprintf(parametri->outFile, "%s", pagina);
	// fprintf(parametri->outFile, "\"><ids><id id=\"%s\"/></ids></folium>", testo->lexema);
}

// FUNZIONE DI STAMPA DELLE DATE
void printDate(struct testo *testo, PARAMETRI *param, struct testo *sigle)
{

	// RECUPERA I PARAMETRI
	// PARAMETRIm2unit * parametri = (PARAMETRIm2unit *) param;

	// fprintf(parametri->outFile, "\n<date>");
	// fprintf(parametri->outFile, "<reading>");
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
void printCit(struct testo *testo, PARAMETRI *param, struct testo *sigle)
{

	/*
	// RECUPERA I PARAMETRI
	PARAMETRIm2unit * parametri = (PARAMETRIm2unit *) param;

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

// FUNZIONE DI STAMPA DI UNA MACRO GENERICA
void printKey(bool inizio,
			  short key,
			  long ln,
			  struct testo *testo,
			  PARAMETRI *parametri,
			  struct testo *sigle)
{

	if (parametri->label == NULL || parametri->nota)
	{

		// IDENTIFICAZIONE DELLA MACRO
		switch (key)
		{
		default:
			break;
		case HLINEKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<linea />");
			}
			break;
		case CLINEKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<regula />");
			}
			break;
		case LEFTGRATABKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<leftgratab />");
			}
			break;
		case RIGHTGRATABKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<rightgratab />");
			}
			break;
		case LEFTANGTABKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<leftangtab />");
			}
			break;
		case RIGHTANGTABKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "rightangtab /");
			}
			break;
		case CONGKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<cong>");
			}
			else
			{
				// fprintf(parametri->outFile, "</cong>");
			}
			break;
		case LBKEY:
			/*
			  if (inizio) {
			  //fprintf(parametri->outFile, "<label");
			  //fprintf(parametri->outFile, " label=\"%s\">", testo->lexema);
			  } else {
			  //fprintf(parametri->outFile, "</label>");
			  }
			*/
			break;
		case COMMENTKEY:
			if (inizio)
			{
				////fprintf(parametri->outFile, "<commenti />");
			}
			break;
		case ANNOTAZKEY:
			if (inizio)
			{
				////fprintf(parametri->outFile, "<annotazioni />");
			}
			break;
		case PROPKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<prop>");
				visitTesto(testo, parametri, sigle);
			}
			else
			{
				// fprintf(parametri->outFile, "</prop>");
			}
			break;
		case OPERAKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<opera>");
				visitTesto(testo, parametri, sigle);
			}
			else
			{
				// fprintf(parametri->outFile, "</opera>");
			}
			break;
		case ARGKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<arg>");
				visitTesto(testo, parametri, sigle);
			}
			else
			{
				// fprintf(parametri->outFile, "</arg>");
			}
			break;
		case LIBROKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<book>");
				visitTesto(testo, parametri, sigle);
			}
			else
			{
				// fprintf(parametri->outFile, "</book>");
			}
			break;

		case BEGINNMKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<margnote>");
			}
			break;

		case ENDNMKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "</margnote>");
			}
			break;

		case MARKKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<markup>");
				// fprintf(parametri->outFile, "<des>");
				visitTesto(testo->testo1, parametri, sigle);
				// fprintf(parametri->outFile, "</des>");
				// fprintf(parametri->outFile, "<reading>");
				visitTesto(testo->testo2, parametri, sigle);
				// fprintf(parametri->outFile, "</reading>");
			}
			else
			{
				// fprintf(parametri->outFile, "</markup>");
			}
			break;
		case COMMKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<comment value=\"");
				visitTesto(testo, parametri, sigle);
				// fprintf(parametri->outFile, "\"/>");
			}
			break;
		case GREEKKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<greek>");
			}
			else
			{
				// fprintf(parametri->outFile, "</greek>");
			}
			break;
		case ABBRKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<abbr>");
			}
			else
			{
				// fprintf(parametri->outFile, "</abbr>");
			}
			break;
		case SETLENGTH:
			if (inizio)
			{
				////fprintf(parametri->outFile, "\\setlength");
				////fprintf(parametri->outFile, "<setlength />\n");
			}
			break;
		case MAKETITLEKEY:
			if (inizio)
			{
				////fprintf(parametri->outFile, "\\maketitle\n\n");
				////fprintf(parametri->outFile, "<maketitle />\n");
			}
			break;
		case PLKEY:
			if (inizio)
			{
				////fprintf(parametri->outFile, "<pl>");
			}
			else
			{
				////fprintf(parametri->outFile, "</pl>");
			}
			break;
		case CLASSKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
			}
			break;
		case DOCENVKEY:
			if (inizio)
			{
				visitTesto(testo->testo1, parametri, sigle);
				// fprintf(parametri->outFile, "<edition>");
				// fprintf(parametri->outFile, "<content>");
				visitTesto(testo->testo2, parametri, sigle);
				// fprintf(parametri->outFile, "</content>\n");
				// fprintf(parametri->outFile, "</edition>\n");
			}
			break;
		case UNITEMPTYKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "unitempty\n");
			}
			break;
		case UNITKEY:
			if (inizio)
			{
				fprintf(parametri->outFile, "unit\n");
			}
			break;
		case SECTIONKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "\n\n<section>");
			}
			else
			{
				// fprintf(parametri->outFile, "</section>\n\n");
			}
			break;
		case SUBSECTIONKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "\n\n<subsection>");
			}
			else
			{
				// fprintf(parametri->outFile, "</subsection>\n\n");
			}
			break;
		case SSSECTIONKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "\n\n<subsubsection>");
			}
			else
			{
				// fprintf(parametri->outFile, "</subsubsection>\n\n");
			}
			break;
		case ':':
			if (inizio)
			{
				// fprintf(parametri->outFile, "%c", ':');
			}
			break;
		case OMKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<om />");
			}
			break;
		case DELKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<del />");
			}
			break;
		case EXPKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<exp />");
			}
			break;
		case NIHILKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<nihil />");
			}
			break;
		case OMLACKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<omlac />");
			}

			break;
		case NLKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<nl />");
			}
			break;
		case LACMKEY:
			if (inizio)
			{
				if (!parametri->nota)
				{
					// fprintf(parametri->outFile, "<lacm />");
				}
			}
			break;
		case LACSKEY:
			if (inizio)
			{
				if (!parametri->nota)
				{
					// fprintf(parametri->outFile, "<lacs />");
				}
			}
			break;
		case LACCKEY:
			if (inizio)
			{
				if (!parametri->nota)
				{
					// fprintf(parametri->outFile, "<lacc />");
				}
			}
			break;
		case BEGINEQNKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<eqn>");
			}
			else
			{
				// fprintf(parametri->outFile, "</eqn>");
			}
			break;
		case BEGINMATHKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<math>");
			}
			else
			{
				// fprintf(parametri->outFile, "</math>");
			}
			break;
		case SLANTED:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<slanted>");
			}
			else
			{
				// fprintf(parametri->outFile, "</slanted>");
			}
			break;
		case EMPHASIS:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<emph>");
			}
			else
			{
				// fprintf(parametri->outFile, "</emph>");
			}
			break;
		case TITKEY:
			if (inizio)
			{
				////fprintf(parametri->outFile, "<tit>");
			}
			else
			{
				////fprintf(parametri->outFile, "</tit>");
			}
			break;
		case ITALIC:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<italics>");
			}
			else
			{
				// fprintf(parametri->outFile, "</italics>");
			}
			break;
		case STRIKEKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<strike>");
			}
			else
			{
				// fprintf(parametri->outFile, "</strike>");
			}
			break;
		case POWERKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<power>");
			}
			else
			{
				// fprintf(parametri->outFile, "</power>");
			}
			break;
		case INDEXKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<underline>");
			}
			else
			{
				// fprintf(parametri->outFile, "</underline>");
			}
			break;
		case SUPKEY:
			if (inizio)
			{
				////fprintf(parametri->outFile, "<sup>");
			}
			else
			{
				////fprintf(parametri->outFile, "</sup>");
			}
			break;
		case SUBKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<sub>");
			}
			else
			{
				// fprintf(parametri->outFile, "</sub>");
			}
			break;
		case TILDEKEY:
			if (inizio)
			{
				////fprintf(parametri->outFile,"&nbsp;");
			}
			break;
		case DSKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile,"<db />");
			}
			break;
		case SSKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile,"<sb />");
			}
			break;
		case SLASH:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<slash />");
			}
			break;
		case GENERICENV:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<%s>\n", testo->lexema);
			}
			else
			{
				// fprintf(parametri->outFile, "</%s>\n", testo->lexema);
			}
			break;
		case ENUNCIATIO:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<enunciatio>");
			}
			else
			{
				// fprintf(parametri->outFile, "</enunciatio>");
			}
			break;
		case PROTASIS:
			if (inizio)
			{
				////fprintf(parametri->outFile, "<protasis>");
			}
			else
			{
				////fprintf(parametri->outFile, "</protasis>\n");
			}
			break;
		case CENTERKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<center>");
			}
			else
			{
				// fprintf(parametri->outFile, "</center>\n");
			}
			break;
		case PARAGRAPHEND:
			if (inizio)
			{
				// fprintf(parametri->outFile, "\n");
			}
			break;
		case PARAGRAPHSTART:
			if (inizio)
			{
				// fprintf(parametri->outFile, "\n");
			}
			break;
		case PARAGRAPH:
			if (inizio)
			{
				// fprintf(parametri->outFile, "\n");
			}
			break;
		case TITLEKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<title>");
			}
			else
			{
				// fprintf(parametri->outFile, "</title>");
			}
			break;
		case AUTHORKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<author>");
			}
			else
			{
				// fprintf(parametri->outFile, "</author>");
			}
			break;
		case ROMAN:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<roman>");
			}
			else
			{
				// fprintf(parametri->outFile, "</roman>");
			}
			break;
		case BOLDFACE:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<b>");
			}
			else
			{
				// fprintf(parametri->outFile, "</b>");
			}
			break;
		case TYPEFACE:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<tt>");
			}
			else
			{
				// fprintf(parametri->outFile, "</tt>");
			}
			break;
		case SPACES:
		case NEWLINE:
			if (inizio)
			{
				// fprintf(parametri->outFile, " ");
				/*
				  if (nSpaces < 10) {
				  nSpaces++;
				  //fprintf(parametri->outFile, " ");
				  } else {
				  nSpaces = 0;
				  //fprintf(parametri->outFile, "\n");
				  }
				*/
			}
			break;
		case MAUROTEXKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile,"<mauroteX />");
			}
			break;
		case MTEXKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile,"<mtex />");
			}
			break;
		case PERKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile,"<per />");
			}
			break;
		case CONKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile,"<con />");
			}
			break;
		case TEXKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile,"<tex />");
			}
			break;
		case LATEXKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile,"<latex />");
			}
			break;
		case ADKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "---");
			}
			break;
		case VECTORKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<vector>");
			}
			else
			{
				// fprintf(parametri->outFile, "</vector>");
			}
			break;
		case CONTRKEY:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<contr>");
			}
			else
			{
				// fprintf(parametri->outFile, "</contr>");
			}
			break;
		case CRUXKEY:
			if (inizio)
			{
				////fprintf(parametri->outFile, "<crux>");
			}
			else
			{
				////fprintf(parametri->outFile, "</crux>");
			}
			break;
		case EXPUKEY:
			if (inizio)
			{
				////fprintf(parametri->outFile, "<expu>");
			}
			else
			{
				////fprintf(parametri->outFile, "</expu>");
			}
			break;
		case INTEKEY:
			if (inizio)
			{
				////fprintf(parametri->outFile, "<inte>\n");
			}
			else
			{
				////fprintf(parametri->outFile, "</inte>\n");
			}
			break;
		case LEFTANG:
			if (inizio)
			{
				// fprintf(parametri->outFile, "<leftang>\n");
			}
			else
			{
				// fprintf(parametri->outFile, "</leftang>\n");
			}
			break;
		case LEFTSQUARE:
			if (inizio)
			{
				// fprintf(parametri->outFile, "[");
			}
			else
			{
				// fprintf(parametri->outFile, "]");
			}
			break;
		case LEFTPAR:
			if (inizio)
			{
				// fprintf(parametri->outFile, "(");
			}
			break;
		case RIGHTPAR:
			if (inizio)
			{
				// fprintf(parametri->outFile, ")");
			}
			break;
		case HREFKEY:
			if (inizio)
			{
				visitTesto(testo->testo2, parametri, sigle);
			}
			break;
			// case NOTAMACRO:
		case OUTRANGE:
			if (inizio)
			{
				// fprintf(parametri->outFile, "??");
			}
			break;
			/*
			  case NOKEY:
			  if (testo) {
			  if (inizio) {
			  ////fprintf(parametri->outFile, "{");
			  //fprintf(parametri->outFile, "{");
			  } else {
			  ////fprintf(parametri->outFile, "}");
			  //fprintf(parametri->outFile, "}");
			  }
			  }
			  break;
			*/
		}
		////fprintf(parametri->outFile, "</macro>");
	}
}
/*
  {

  // RECUPERA I PARAMETRI

  if (parametri->label == NULL || parametri->nota) {

  // IDENTIFICAZIONE DELLA MACRO
  switch (key) {
  default:
  fprintf(parametri->outFile, "<key />");
  break;
  case UNITKEY:
  if (inizio) {
  fprintf(parametri->outFile, "<unit />");
  }
  break;
  }
  }
  }
*/

// FUNZIONE DI STAMPA DI UNA FIGURA
void printFig(short key, struct testo *testo, PARAMETRI *parametri) {}

// FUNZIONE DI STAMPA DI UN RIFERIMENTO AD UNA UNIT
void printUnitRef(const char *opera, const char *etichetta, PARAMETRI *parametri)
{
	short nUnit = findUnit(etichetta);
	printUnitRef(nUnit, parametri);
}

void printUnitRef(short nUnit, PARAMETRI *parametri)
{
	// fprintf(parametri->outFile, "<unitref>%d</unitref>\n", nUnit);
}

// FUNZIONE DI STAMPA DI UNA PAROLA
void printLexema(short key, const char *lexema, PARAMETRI *parametri)
{
	fflush(parametri->outFile);
}

// FUNZIONE DI STAMPA DEL FRONTESPIZIO
void printFrontespizio(struct testo *testo,
					   PARAMETRI *parametri,
					   struct testo *sigle)
{
}

// FUNZIONE DI STAMPA DI UNA LISTA DI AUTORI
void printAuthors(struct testo *testo,
				  PARAMETRI *parametri,
				  struct testo *sigle)
{
}

// FUNZIONE DI STAMPA DI UNA NOTA A PIEDE DI PAGINA
void printFootnote(struct testo *testo,
				   PARAMETRI *parametri,
				   struct testo *sigle)
{
}

// FUNZIONE DI STAMPA DI UN COMMENTO
void printCommento(struct testo *testo,
				   PARAMETRI *param,
				   struct testo *sigle)
{
}

// FUNZIONE DI STAMPA DI UNA ANNOTAZIONE
void printAdnotatio(
	char const *label,
	struct testo *testo,
	PARAMETRI *parametri,
	struct testo *sigle)
{
}

// FUNZIONE DI MARCATURA DEL TESTO CRITICO
void printInizioTestoCritico(short key, PARAMETRI *parametri)
{
}

void printFineTestoCritico(short key, PARAMETRI *parametri)
{
}

// FUNZIONE DI MARCATURA DELLE VARIANTI
void printInizioVarianti(PARAMETRI *parametri)
{
}

void printSepVarianti(PARAMETRI *parametri)
{
}

void printFineVarianti(PARAMETRI *parametri)
{
}

// STAMPO LE SIGLE
void stampaSigle(struct testo *testo, PARAMETRI *parametri) {}

// STAMPO LA LEZIONE
void stampaLezione(struct testo *testo, PARAMETRI *parametri, struct testo *sigle)
{
	// fprintf(parametri->outFile, "<reading>");
	visitTesto(testo, parametri, sigle);
	// fprintf(parametri->outFile, "</reading>\n");
}

// STAMPO LA NOTE
void stampaNote(struct testo *testimoni,
				struct testo *testo,
				PARAMETRI *parametri,
				struct testo *sigle)
{
}

// FUNZIONE PER DISABILITARE LA GESTIONE DELLA VV DI MVISIT
bool checkVV(short key,
			 struct testo *testo,
			 PARAMETRI *parametri,
			 struct testo *sigle)
{

	// IGNORING VB VARIANTS
	if (key == VBKEY)
	{
		struct testo *restoVV = testo->testo1->testo2;

		// CONTROLLO ESISTENZA RESTO VV
		if (restoVV)
		{

			// STAMPO LA LEZIONE SE C'E'
			if (restoVV->testo1)
			{
				visitTesto(restoVV->testo1, parametri, sigle);
			}
		}
	}
	else
	{

		// fprintf(parametri->outFile, "<variant");
		/*
		  switch (key) {
		  case VVKEY:
		  fprintf(parametri->outFile, "\"vv");
		  if (testo->lexema) {
		  fprintf(parametri->outFile, "-%s", testo->lexema);
		  }
		  fprintf(parametri->outFile, "\"");
		  break;
		  case TVKEY:
		  fprintf(parametri->outFile, "\"tv");
		  if (testo->lexema) {
		  fprintf(parametri->outFile, "-%s", testo->lexema);
		  }
		  fprintf(parametri->outFile, "\"");
		  break;
		  case NMARGKEY:
		  fprintf(parametri->outFile, "\"nmarg\"");
		  break;
		  case NOTEKEY:
		  fprintf(parametri->outFile, "\"note\"");
		  break;
		  case VBKEY:
		  fprintf(parametri->outFile, "\"vb\"");
		  break;
		  default:
		  break;
		  }
		*/
		// fprintf(parametri->outFile, ">");

		// MEMORIZZO LA VV
		struct testo *t = testo;

		// CONTROLLO ESISTENZA VV
		while (t != NULL)
		{
			// fprintf(parametri->outFile, "<variantitem>\n");

			// CONTROLLO ESISTENZA VARIANTE
			if (t->testo1)
			{

				/*
				// MEMORIZZO LE SIGLE DELLA VV
				struct testo * s = t->testo1->testo1;

				// STAMPA DELLE SIGLE
				stampaSigle(s, parametri);
				*/

				// MEMORIZZO IL RESTO DELLA VV
				struct testo *restoVV = t->testo1->testo2;

				// CONTROLLO ESISTENZA RESTO VV
				if (restoVV)
				{

					// STAMPO LA LEZIONE SE C'E'
					if (restoVV->testo1)
					{
						stampaLezione(restoVV->testo1, parametri, sigle);
					}

					// STAMPO LE NOTE
					stampaNote(t->testo1->testo1, restoVV, parametri, sigle);
				}
			}

			// fprintf(parametri->outFile, "</variantitem>\n");
			t = t->testo2;
		}

		// fprintf(parametri->outFile, "</variant>\n");
	}
	return false;
}

// FUNZIONE DI STAMPA DI UNA NOTA
void printVV(short key,
			 struct testo *testo,
			 PARAMETRI *param,
			 struct testo *sigle)
{
}

// FUNZIONE DI STAMPA DELL'ALBERO SINTATTICO
void printDocumento(struct testo *testo)
{

	PARAMETRI *parametri;

	if (testo)
	{

		unitFileName = new char[strlen(fileName) + 10];
		sprintf(unitFileName, "%s%s", fileName, UNITEST);
		parametri = new PARAMETRI();
		parametri->setFileName(unitFileName);
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
	nSpaces = 0;

	for (int i = 1; i < argn; i++)
	{

		// CONTROLLO DEGLI ARGOMENTI
		switch (argv[i][0])
		{

			// CONTROLLO LA PRESENZA DI OPZIONI
		case '-':
			switch (argv[i][1])
			{
				// MOSTRA L'HELP
			case 'h':
				help = true;
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

		// INVOCAZIONE DELL'ANALIZZATORE SINTATTICO
		parser(fileName);
	}
	else
	{

		// STAMPA L'HELP
		printf("traduttore MauroTeX -> UNIT (%s)\n", VERSIONSTRING);
		printf("uso: %s [-h] <nome_file>\n", argv[0]);
		if (help)
		{
			printf("opzioni:\n");
			printf("  -h             stampa questo messaggio\n");
		}
	}
}
