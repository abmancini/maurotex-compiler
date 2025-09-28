/*

  file "mparse.y"
  ANALIZZATORE SINTATTICO
  mparse: controllore sintattico e generatore albero sintattico per M-TeX
  versione 1.16b1 del 25 febbraio 2025

  eLabor sc
  via Enrico Fermi 8, Pisa - Italia
  <http://elabor.biz>
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

%{

#define YYMAXDEPTH 200000
#define YYSTYPE_IS_TRIVIAL 1
#define YYLTYPE_IS_TRIVIAL 1

#include "mparse.h"
#include "mparsein.h"

#include <stdlib.h>
#include <string.h>

#define ERRNEW "impossibile allocare nuova memoria"

  static bool errore = false;

  /* STRUTTURA PER IL CONTROLLO DI ELENCO TESTIMONI NEL PROLOGO */

  typedef struct wit {
    const char   * witness;
    wit          * next;
  } wit;

  /* FUNZIONI DEFINITE DALL'ANALIZZATORE LESSICALE */
  
  void yyerror (const char * s);
  int yylex (void);
  int sect = 0;
  int ssect= 0;
  int sssect = 0;
  
  /* PROTOTIPI FUNZIONI DI SUPPORTO */
  
  struct testo * makeTesto(long           ln,
			   long           key,
			   const char   * lexema,
			   struct testo * testo1,
			   struct testo * testo2);
  struct testo * makeSemplAgg(long           ln,
			      const char   * testimone,
			      struct testo * lezione,
			      long           key);
  struct testo * makeSemplCorr(long           ln,
			       const char   * testimone,
			       struct testo * lezcanc,
			       struct testo * lezione,
			       long           key);
  struct testo * makeSemplCanc(long           ln,
			       const char   * testimone,
			       struct testo * lezcanc,
			       struct testo * lezione,
			       long           key);
  bool addLabel(const char * label, struct testo * testo);
  bool addUnit(const char * label);
  bool addLabelKey(const char * label);
  void checkEnv(const char * inizio, short lni, const char * fine, short lnf);
  bool checkGenericEnv(const char * ambiente);
  wit * findWitness(const char * witness);
  bool addWitness(const char * wit);
 
%}

%token TEXTCIRCLEDKEY
%token BANALEKEY
%token NOKEY
%token ONESTEP
%token WORD
%token CLASSKEY
%token BEGINKEY
%token ELENCOTESTIMONIKEY
%token USEPACKAGE EXTERNALDOCUMENT
%token CITEKEY MCITEKEY
%token ENDKEY
%token SUPKEY SUBKEY
%token DOCENVKEY
%token GRAVE
%token ACUTE
%token CIRC
%token SPACES NEWLINE
%token PARAGRAPH
%token PARAGRAPHSTART
%token PARAGRAPHEND
%token EMPHASIS
%token UNDERLINE
%token SLANTED
%token ITALIC ITALICKEY
%token CORRKEY
%token ROMAN
%token BOLDFACE BOLDFACEKEY
%token SMALLCAPSKEY
%token TYPEFACE
%token CENTERKEY
%token MATHKEY
%token BEGINMATHKEY
%token ENDMATHKEY
%token BEGINEQNKEY
%token ENDEQNKEY
%token POWERKEY INDEXKEY
%token GEOMETRY
%token SETLENGTH
%token PARSKIP
%token PARINDENT
%token FOLIUMMARGOPT FOLIUMTESTOOPT
%token TITLEKEY MAKETITLEKEY AUTHORKEY TITLEDATEKEY
%token TITLEWORKKEY
%token TEXKEY LATEXKEY MAUROTEXKEY MTEXKEY
%token PERKEY CONKEY
%token SECTIONKEY SUBSECTIONKEY SSSECTIONKEY
%token PARAGRAPHKEY
%token LABELKEY REFKEY PAGEREFKEY LETTERLABELKEY
%token TILDEKEY
%token LEFTPAR RIGHTPAR
%token LEFTANG RIGHTANG
%token LEFTBRA RIGHTBRA
%token FRACKEY
%token ENUNCIATIO PROTASIS GENERICENV
%token TABULAR
%token SSKEY DSKEY
%token GREEKKEY
%token CONTRKEY
%token ABBRKEY
%token FMARGKEY
%token VECTORKEY
%token MACRO
%token FOLIUMKEY
%token TEOREMAKEY CAPITOLOKEY
%token ALITERKEY LEMMAKEY COROLLARIOKEY SCOLIOKEY ADDITIOKEY SOTTOCAPITOLOKEY
%token CITKEY DATEKEY
%token CITLONGAKEY
%token FORMULAKEY FIGSKIPKEY FIGURAKEY TAVOLAKEY COMMKEY CONGKEY ADNOTKEY NOTEKEY PROPKEY
%token OPERAKEY LIBROKEY NOMEKEY LUOGOKEY
%token CASES
%token UML TILDE
%token LEFTSQUARE RIGHTSQUARE
%token SYMBOL UNITKEY UNITEMPTYKEY
%token SCHOLKEY
%token VVKEY TVKEY VBKEY TBKEY VFKEY
%token OMKEY NIHILKEY DELKEY EXPKEY OMLACKEY NLKEY LACMKEY LACCKEY LACSKEY
%token PLKEY TRKEY EDKEY BISKEY INTERLKEY SUPRAKEY MARGKEY MARGSIGNKEY
%token ANTEDELKEY POSTDELKEY EXKEY PCKEY NMARGKEY REPKEY LECKEY
%token BEGINNMKEY ENDNMKEY
%token CRUXKEY EXPUKEY INTEKEY
%token CRKEY LBKEY
%token COMMENTKEY ANNOTAZKEY
%token UNITREFKEY
%token NOTAMACRO
%token ADKEY LONGKEY SLQUOTEKEY SRQUOTEKEY DLQUOTEKEY DRQUOTEKEY
%token OUTRANGE
%token NOMTESTKEY
%token SMARGKEY
%token SMARGSKEY
%token SINTERLKEY SSUPRAKEY
%token SPCKEY
%token SBISKEY
%token SMCORRKEY
%token SMSCORRKEY
%token SICORRKEY
%token SANTEDELKEY
%token SPOSTDELKEY
%token SEXKEY
%token DESCOMPLKEY
%token EDCOMPLKEY
%token SUPSUBKEY TITKEY RUBRKEY STRIKEKEY ARGKEY MARKKEY
%token SLASH
%token LONGTABLEKEY TABLEKEY SEPROWKEY SEPCOLKEY HLINEKEY CLINEKEY MULTICOLKEY MULTIROWKEY
%token BEGINTABKEY ENDTABKEY LEFTGRATABKEY RIGHTGRATABKEY LEFTANGTABKEY RIGHTANGTABKEY
%token NUMINTESTKEY MITINTESTKEY DESINTESTKEY LMIINTESTKEY DATINTESTKEY COMINTESTKEY
%token INTESTAKEY
%token HREFKEY VOIDHREFKEY
%token HTMLCUTKEY
%token TESTIMONIKEY
%token POSTSCRIPTKEY
%token PERSKEY TITMARG TEXTGREEK
%token NBSPACEKEY
%token FLATKEY
%token NATURALKEY
%token CDOTKEY
%token VERTKEY
%token LBRACKETKEY
%token RBRACKETKEY
%token SKEY
%token CITMARGKEY CITMARGSIGNKEY
%token DAGKEY
%token DOTSKEY
%token ITEMIZE DESCRIPTION ENUMERATE ITEMKEY
%token QUOTE
%token TIMESKEY
%token NOINDENTKEY NEWPAGEKEY VSPACEKEY HYPHENKEY
%token FOOTNOTEKEY
%token FRONTESPIZIOKEY LISTSEPKEY
%token BEGINEPISTOLAKEY ENDEPISTOLAKEY
%token BIBRIFKEY
%token PRELOADKEY
%token MANUSKEY ALIAMANUSKEY
%token COMMA
%token INTER
%token MFRACKEY
%token OMFRACKEY
%token MFRACOKEY
%token PHANTOMKEY

%%

documento: documentocompl {
  if (!errore) {
    printDocumento($1.testo);
  }
};

documentocompl: optspaces CLASSKEY opzioni wordbra corpo {
  if ($5.testo == NULL) {
    $$.testo = NULL;
  } else {
    $$.testo = makeTesto($1.ln, CLASSKEY, $4.lexema, $3.testo, $5.testo);
  }
}
| testo {
  $$.testo = makeTesto($1.ln, CLASSKEY, NULL, NULL, $1.testo);
};

opzioni: { /* NESSUNA OPZIONE */
  $$.testo = NULL;
}
| LEFTSQUARE listaopt RIGHTSQUARE {
  $$.testo = $2.testo;
}
| LEFTSQUARE error {
  yyemessage($1.ln, "nelle opzioni");
  $$.testo = NULL;
};

corpo: optspaces prologo BEGINKEY LEFTBRA DOCENVKEY RIGHTBRA
optspaces htmlcut testonosquare ENDKEY LEFTBRA DOCENVKEY RIGHTBRA optspaces {
  $$.testo = makeTesto($1.ln, DOCENVKEY, NULL,
		       makeTesto($1.ln,
				 NOKEY,
				 NULL,
				 $2.testo,
				 makeTesto($1.ln, HTMLCUTKEY, NULL, $8.testo, $9.testo)),
		       NULL);
}
| optspaces prologo BEGINKEY LEFTBRA DOCENVKEY RIGHTBRA
optspaces htmlcut testonosquare ENDKEY wordbra {
  $$.testo = makeTesto($1.ln, DOCENVKEY, NULL,
		       makeTesto($1.ln,
				 NOKEY,
				 NULL,
				 $2.testo,
				 makeTesto($1.ln, HTMLCUTKEY, NULL, $8.testo, $9.testo)),
		       NULL);
  checkEnv("document", $5.ln, $11.lexema, $11.ln);
}
| optspaces prologo BEGINKEY LEFTBRA DOCENVKEY RIGHTBRA optspaces error {
  $$.testo = NULL;
  yyemessage($7.ln, "un documento deve iniziare con \\htmlcut");
};

space: SPACES {
  $$.testo = NULL;
  $$.ln = $1.ln;
}
| NEWLINE {
  $$.testo = NULL;
  $$.ln = $1.ln;
};

spaces: space {
  $$.testo = NULL;
}
| spaces space {
  $$.testo = NULL;
};

optspaces: {
  /* NESSUNO SPAZIO */
  $$.testo = NULL;
}
| spaces {
  $$.testo = NULL;
};

prologo: {
  /* FINE PROLOGO */
  $$.testo = NULL;
}
| elencotestimoni optspaces prologo {
  $$.testo = makeTesto($1.ln, ELENCOTESTIMONIKEY, NULL, $1.testo, $3.testo);
}
| TESTIMONIKEY LEFTBRA testimoni RIGHTBRA optspaces prologo {
  $$.testo = makeTesto($1.ln, TESTIMONIKEY, NULL, $3.testo, $6.testo);
}
| USEPACKAGE opzioni LEFTBRA listaopt RIGHTBRA optspaces prologo {
  $$.testo = makeTesto($1.ln, USEPACKAGE, NULL,
		       makeTesto($1.ln, NOKEY, NULL, $2.testo, $4.testo),
		       $7.testo);
}
| EXTERNALDOCUMENT option testobra optspaces prologo {
  $$.testo = makeTesto($1.ln, EXTERNALDOCUMENT, $2.lexema, $3.testo, $5.testo);
}
| GEOMETRY LEFTBRA listaopt RIGHTBRA optspaces prologo {
  $$.testo = makeTesto($1.ln, GEOMETRY, NULL, $3.testo, $6.testo);
}
| SETLENGTH optspaces prologo {
  $$.testo = makeTesto($1.ln, SETLENGTH, NULL, NULL, $3.testo);
}
| PARSKIP wordbra optspaces prologo {
  $$.testo = makeTesto($1.ln, PARSKIP, $2.lexema, NULL, $4.testo);
}
| PARINDENT wordbra optspaces prologo {
  $$.testo = makeTesto($1.ln, PARINDENT, $2.lexema, NULL, $4.testo);
}
| PRELOADKEY wordbra optspaces prologo {
  $$.testo = makeTesto($1.ln, PRELOADKEY, $2.lexema, NULL, $4.testo);
}
| manus optspaces prologo {
  $$.testo = makeTesto($1.ln, MANUSKEY, NULL, $1.testo, $3.testo);
}
| FOLIUMMARGOPT optspaces prologo {
  $$.testo = makeTesto($1.ln, FOLIUMMARGOPT, NULL, NULL, $3.testo);
}
| FOLIUMTESTOOPT optspaces prologo {
  $$.testo = makeTesto($1.ln, FOLIUMTESTOOPT, NULL, NULL, $3.testo);
};

manus: MANUSKEY wordbra testobra testobra wordbra {
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, 
					   makeTesto($3.ln, NOKEY, NULL, $3.testo, $4.testo),
					   makeTesto($5.ln, NOKEY, $5.lexema, NULL, NULL));
}
| MANUSKEY wordbra testobra testobra {
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, 
					   makeTesto($3.ln, NOKEY, NULL, $3.testo, $4.testo),
					   NULL);
}

listaopt: optspaces { /* NESSUNA OPZIONE */
  $$.testo = NULL;
}
| optspaces WORD {
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, NULL, NULL);
 }
| optspaces WORD COMMA listaopt {
  // if ($2.lexema[0] != ',') {
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, NULL, $4.testo);
  //} else {
  //$$.testo = $3.testo;
  //}
 };

testo: testonosquare {
  $$.testo = $1.testo;
}
| testosquare {
  $$.testo = makeTesto($1.ln, LEFTSQUARE, NULL, $1.testo, NULL);
};

opttestosquare: {
  $$.testo = NULL;
}
| testosquare {
  $$.testo = $1.testo;
}

testosquare: LEFTSQUARE testo RIGHTSQUARE {
  if ($3.ln - $1.ln > 20) {
    yywmessage($1.ln,"quadra aperta");
    yywmessage($3.ln,"chiusa in questo punto");
  }
  $$.testo = $2.testo;
}
| LEFTSQUARE error {
  $$.testo = NULL;
  yyemessage($1.ln, "dopo questo punto");
};

testonosquare: testonospace {
  $$.testo = $1.testo;
}
| spaces testonospace {
  $$.testo = makeTesto($1.ln, SPACES, NULL, NULL, $2.testo);
};

testonospace: {
  /* FINE TESTO */
  $$.testo = NULL;
}
| DOCENVKEY testo {
  $$.testo = makeTesto($1.ln, NOKEY, "document", NULL, $2.testo);
}
| CENTERKEY testo {
  $$.testo = makeTesto($1.ln, NOKEY, "center", NULL, $2.testo);
}
| COMMA testo {
  $$.testo = makeTesto($1.ln, NOKEY, ",", NULL, $2.testo);
}
| TABLEKEY testo {
  $$.testo = makeTesto($1.ln, NOKEY, "tabula", NULL, $2.testo);
}
| INTER testo {
  $$.testo = makeTesto($1.ln, NOKEY, $1.lexema, NULL, $2.testo);
}
| ':' testo {
  $$.testo = makeTesto($1.ln, NOKEY, ":", NULL, $2.testo);
}
| ADKEY testo {
  $$.testo = makeTesto($1.ln, ADKEY, NULL, NULL, $2.testo);
}
| LONGKEY testo {
  $$.testo = makeTesto($1.ln, LONGKEY, NULL, NULL, $2.testo);
}
| SLQUOTEKEY testo {
  $$.testo = makeTesto($1.ln, SLQUOTEKEY, NULL, NULL, $2.testo);
}
| SRQUOTEKEY testo {
  $$.testo = makeTesto($1.ln, SRQUOTEKEY, NULL, NULL, $2.testo);
}
| DLQUOTEKEY testo {
  $$.testo = makeTesto($1.ln, DLQUOTEKEY, NULL, NULL, $2.testo);
}
| DRQUOTEKEY testo {
  $$.testo = makeTesto($1.ln, DRQUOTEKEY, NULL, NULL, $2.testo);
}
| MTEXKEY testo {
  $$.testo = makeTesto($1.ln, MTEXKEY, NULL, NULL, $2.testo);
}
| PERKEY testo {
  $$.testo = makeTesto($1.ln, PERKEY, NULL, NULL, $2.testo);
}
| CONKEY testo {
  $$.testo = makeTesto($1.ln, CONKEY, NULL, NULL, $2.testo);
}
| MAUROTEXKEY testo {
  $$.testo = makeTesto($1.ln, MAUROTEXKEY, NULL, NULL, $2.testo);
}
| LATEXKEY testo {
  $$.testo = makeTesto($1.ln, LATEXKEY, NULL, NULL, $2.testo);
}
| TEXKEY testo {
  $$.testo = makeTesto($1.ln, TEXKEY, NULL, NULL, $2.testo);
}
| WORD testo {
  $$.testo = makeTesto($1.ln, NOKEY, $1.lexema, NULL, $2.testo);
}
| GRAVE testo {
  $$.testo = makeTesto($1.ln, GRAVE, $1.lexema, NULL, $2.testo);
}
| ACUTE testo {
  $$.testo = makeTesto($1.ln, ACUTE, $1.lexema, NULL, $2.testo);
}
| UML testo {
  $$.testo = makeTesto($1.ln, UML, $1.lexema, NULL, $2.testo);
}
| TILDE testo {
  $$.testo = makeTesto($1.ln, TILDE, $1.lexema, NULL, $2.testo);
}
| CIRC testo {
  $$.testo = makeTesto($1.ln, CIRC, $1.lexema, NULL, $2.testo);
}
| PARAGRAPH testo {
  $$.testo = makeTesto($1.ln, PARAGRAPH, NULL, NULL, $2.testo);
}
| PARAGRAPHSTART testo {
  $$.testo = makeTesto($1.ln, PARAGRAPHSTART, NULL, NULL, $2.testo);
}
| PARAGRAPHEND testo {
  $$.testo = makeTesto($1.ln, PARAGRAPHEND, NULL, NULL, $2.testo);
}
| EMPHASIS space testo {
  $$.testo = makeTesto($1.ln, EMPHASIS, NULL, $3.testo, NULL);
}
| SLANTED space testo {
  $$.testo = makeTesto($1.ln, SLANTED, NULL, $3.testo, NULL);
}
| ITALIC space testo {
  $$.testo = makeTesto($1.ln, ITALIC, NULL, $3.testo, NULL);
}
| italic testo {
  $$.testo = makeTesto($1.ln, ITALIC, NULL, $1.testo, $2.testo);
}
| CORR testo {
  $$.testo = makeTesto($1.ln, CORRKEY, NULL, $1.testo, $2.testo);
}
| BOLDFACE space testo {
  $$.testo = makeTesto($1.ln, BOLDFACE, NULL, $3.testo, NULL);
}
| boldface testo {
  $$.testo = makeTesto($1.ln, BOLDFACE, NULL, $1.testo, $2.testo);
}
| smallcaps testo {
  $$.testo = makeTesto($1.ln, SMALLCAPSKEY, NULL, $1.testo, $2.testo);
}
| TYPEFACE space testo {
  $$.testo = makeTesto($1.ln, TYPEFACE, NULL, $3.testo, NULL);
}
| ROMAN space testo {
  $$.testo = makeTesto($1.ln, ROMAN, NULL, $3.testo, NULL);
}
| testobra testo {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $1.testo, $2.testo);
}
| center testo {
  $$.testo = makeTesto($1.ln, CENTERKEY, NULL, $1.testo, $2.testo);
}
| titolo testo {
  $$.testo = makeTesto($1.ln, TITKEY, NULL, $1.testo, $2.testo);
}
| personaggio testo {
  $$.testo = makeTesto($1.ln, PERSKEY, NULL, $1.testo, $2.testo);
}
| rubrica testo {
  $$.testo = makeTesto($1.ln, RUBRKEY, NULL, $1.testo, $2.testo);
}
| strike testo {
  $$.testo = makeTesto($1.ln, STRIKEKEY, NULL, $1.testo, $2.testo);
}
| underline testo {
  $$.testo = makeTesto($1.ln, UNDERLINE, NULL, $1.testo, $2.testo);
}
| greco testo {
  $$.testo = makeTesto($1.ln, GREEKKEY, NULL, $1.testo, $2.testo);
}
| crux testo {
  $$.testo = makeTesto($1.ln, CRUXKEY, NULL, $1.testo, $2.testo);
}
| expu testo {
  $$.testo = makeTesto($1.ln, EXPUKEY, NULL, $1.testo, $2.testo);
}
| inte testo {
  $$.testo = makeTesto($1.ln, INTEKEY, NULL, $1.testo, $2.testo);
}
| bibrif testo {
  $$.testo = makeTesto($1.ln, BIBRIFKEY, NULL, $1.testo, $2.testo);
}
| citmarg testo {
  $$.testo = makeTesto($1.ln, CITMARGKEY, NULL, $1.testo, $2.testo);
}
| citmargsign testo {
  $$.testo = makeTesto($1.ln, CITMARGSIGNKEY, NULL, $1.testo, $2.testo);
}
| matematica testo {
  $$.testo = makeTesto($1.ln, BEGINMATHKEY, NULL, $1.testo, $2.testo);
}
| testocerchiato testo {
  $$.testo = makeTesto($1.ln, TEXTCIRCLEDKEY, NULL, $1.testo, $2.testo);
}
| phantom testo {
  $$.testo = makeTesto($1.ln, PHANTOMKEY, NULL, $1.testo, $2.testo);
}
| equazione testo {
  $$.testo = makeTesto($1.ln, BEGINEQNKEY, NULL, $1.testo, $2.testo);
}
| supersubscript testo {
  $$.testo = makeTesto($1.ln, SUPSUBKEY, NULL, $1.testo, $2.testo);
}
| power testo {
  $$.testo = makeTesto($1.ln, POWERKEY, NULL, $1.testo, $2.testo);
}
| sup testo {
  $$.testo = makeTesto($1.ln, SUPKEY, NULL, $1.testo, $2.testo);
}
| index testo {
  $$.testo = makeTesto($1.ln, INDEXKEY, NULL, $1.testo, $2.testo);
}
| sub testo {
  $$.testo = makeTesto($1.ln, SUBKEY, NULL, $1.testo, $2.testo);
}
| title testo {
  $$.testo = makeTesto($1.ln, TITLEKEY, NULL, $1.testo, $2.testo);
}
| MAKETITLEKEY testo {
  $$.testo = makeTesto($1.ln, MAKETITLEKEY, NULL, NULL, $2.testo);
}
| author testo {
  $$.testo = makeTesto($1.ln, AUTHORKEY, NULL, $1.testo, $2.testo);
}
| titledate testo {
  $$.testo = makeTesto($1.ln, TITLEDATEKEY, NULL, $1.testo, $2.testo);
}
| LEFTPAR testo {
  $$.testo = makeTesto($1.ln, LEFTPAR, NULL, NULL, $2.testo);
}
| RIGHTPAR testo {
  $$.testo = makeTesto($1.ln, RIGHTPAR, NULL, NULL, $2.testo);
}
| testoang testo {
  $$.testo = makeTesto($1.ln, LEFTANG, NULL, $1.testo, $2.testo);
}
| section testo {
  $$.testo = makeTesto($1.ln, SECTIONKEY, NULL, $1.testo, $2.testo);
}
| subsection testo {
  $$.testo = makeTesto($1.ln, SUBSECTIONKEY, NULL, $1.testo, $2.testo);
}
| subsubsection testo {
  $$.testo = makeTesto($1.ln, SSSECTIONKEY, NULL, $1.testo, $2.testo);
}
| cite testo {
  $$.testo = makeTesto($1.ln, CITEKEY, NULL, $1.testo, $2.testo);
}
| maurocite testo {
  $$.testo = makeTesto($1.ln, MCITEKEY, NULL, $1.testo, $2.testo);
}
| paragraph testo {
  $$.testo = makeTesto($1.ln, PARAGRAPHKEY, NULL, $1.testo, $2.testo);
}
| label testo {
  $$.testo = makeTesto($1.ln, LABELKEY, NULL, $1.testo, $2.testo);
}
| letterlabel testo {
  $$.testo = makeTesto($1.ln, LETTERLABELKEY, NULL, $1.testo, $2.testo);
}
| ref testo {
  $$.testo = makeTesto($1.ln, REFKEY, NULL, $1.testo, $2.testo);
}
| pageref testo {
  $$.testo = makeTesto($1.ln, PAGEREFKEY, NULL, $1.testo, $2.testo);
}
| frazione testo {
  $$.testo = makeTesto($1.ln, FRACKEY, NULL, $1.testo, $2.testo);
}
| frazionemultipla testo {
  $$.testo = makeTesto($1.ln, MFRACKEY, NULL, $1.testo, $2.testo);
}
| ofrazionemultipla testo {
  $$.testo = makeTesto($1.ln, OMFRACKEY, NULL, $1.testo, $2.testo);
}
| frazionemultiplao testo {
  $$.testo = makeTesto($1.ln, MFRACOKEY, NULL, $1.testo, $2.testo);
}
| Title testo {
  $$.testo = makeTesto($1.ln, TITLEWORKKEY, NULL, $1.testo, $2.testo);
}
| TILDEKEY testo {
  $$.testo = makeTesto($1.ln, TILDEKEY, NULL, NULL, $2.testo);
}
| ITEMKEY opttestosquare testonosquare {
  $$.testo = makeTesto($1.ln, ITEMKEY, NULL, $2.testo, $3.testo);
}
| ambiente testo {
  bool env = false;
  if (strcmp($1.testo->lexema,"quote") == 0) {
    $$.testo = makeTesto($1.ln, QUOTE, NULL, $1.testo, $2.testo);
    env = true;
  }
  if (strcmp($1.testo->lexema,"itemize") == 0) {
    $$.testo = makeTesto($1.ln, ITEMIZE, NULL, $1.testo, $2.testo);
    env = true;
  }
  if (strcmp($1.testo->lexema,"enumerate") == 0) {
    $$.testo = makeTesto($1.ln, ENUMERATE, NULL, $1.testo, $2.testo);
    env = true;
  }
  if (strcmp($1.testo->lexema,"description") == 0) {
    $$.testo = makeTesto($1.ln, DESCRIPTION, NULL, $1.testo, $2.testo);
    env = true;
  }
  if (strcmp($1.testo->lexema,"Enunciatio") == 0) {
    $$.testo = makeTesto($1.ln, ENUNCIATIO, NULL, $1.testo, $2.testo);
    env = true;
  }
  if (strcmp($1.testo->lexema,"Protasis") == 0) {
    $$.testo = makeTesto($1.ln, PROTASIS, NULL, $1.testo, $2.testo);
    env = true;
  }
  if (strcmp($1.testo->lexema,"tabular") == 0) {
    $$.testo = makeTesto($1.ln, TABULAR, NULL, $1.testo, $2.testo);
    env = true;
  }
  if (strcmp($1.testo->lexema,"titmarg") == 0) {
    $$.testo = makeTesto($1.ln, TITMARG, NULL, $1.testo, $2.testo);
    env = true;
  }
  if (strcmp($1.testo->lexema,"textgreek") == 0) {
    $$.testo = makeTesto($1.ln, TEXTGREEK, NULL, $1.testo, $2.testo);
    env = true;
  }
  if (!env) {
    $$.testo = makeTesto($1.ln, GENERICENV, NULL, $1.testo, $2.testo);
    if (!checkGenericEnv($1.testo->lexema)) {
      char * envmsg = new char[strlen($1.testo->lexema)+100];
      sprintf(envmsg, "ambiente <%s> sconosciuto", $1.testo->lexema);
      yywmessage($1.ln, envmsg);
      delete[](envmsg);
    }
  }
}
| DSKEY testo {
  $$.testo = makeTesto($1.ln, DSKEY, NULL, NULL, $2.testo);
}
| SSKEY testo {
  $$.testo = makeTesto($1.ln, SSKEY, NULL, NULL, $2.testo);
}
| DOTSKEY testo {
  $$.testo = makeTesto($1.ln, DOTSKEY, NULL, NULL, $2.testo);
}
| TIMESKEY testo {
  $$.testo = makeTesto($1.ln, TIMESKEY, NULL, NULL, $2.testo);
}
| NBSPACEKEY testo {
  $$.testo = makeTesto($1.ln, NBSPACEKEY, NULL, NULL, $2.testo);
}
| FLATKEY testo {
  $$.testo = makeTesto($1.ln, FLATKEY, NULL, NULL, $2.testo);
}
| NATURALKEY testo {
  $$.testo = makeTesto($1.ln, NATURALKEY, NULL, NULL, $2.testo);
}
| CDOTKEY testo {
  $$.testo = makeTesto($1.ln, CDOTKEY, NULL, NULL, $2.testo);
}
| VERTKEY testo {
  $$.testo = makeTesto($1.ln, VERTKEY, NULL, NULL, $2.testo);
}
| LBRACKETKEY testo {
  $$.testo = makeTesto($1.ln, LBRACKETKEY, NULL, NULL, $2.testo);
}
| RBRACKETKEY testo {
  $$.testo = makeTesto($1.ln, RBRACKETKEY, NULL, NULL, $2.testo);
}
| SKEY testo {
  $$.testo = makeTesto($1.ln, SKEY, NULL, NULL, $2.testo);
}
| SLASH testo {
  $$.testo = makeTesto($1.ln, SLASH, NULL, NULL, $2.testo);
}
| VECTORKEY testo {
  $$.testo = makeTesto($1.ln, VECTORKEY, NULL, $2.testo, NULL);
}
| BEGINNMKEY testo {
  $$.testo = makeTesto($1.ln, BEGINNMKEY, NULL, $2.testo, NULL);
}
| ENDNMKEY testo {
  $$.testo = makeTesto($1.ln, ENDNMKEY, NULL, $2.testo, NULL);
}
| contr testo {
  $$.testo = makeTesto($1.ln, CONTRKEY, NULL, $1.testo, $2.testo);
}
| abbr testo {
  $$.testo = makeTesto($1.ln, ABBRKEY, NULL, $1.testo, $2.testo);
}
| fmarg testo {
  $$.testo = makeTesto($1.ln, FMARGKEY, NULL, $1.testo, $2.testo);
}
| MACRO testo {
  $$.testo = makeTesto($1.ln, MACRO, $1.lexema, NULL, $2.testo);
}
| SYMBOL testo {
  $$.testo = makeTesto($1.ln, SYMBOL, $1.lexema, NULL, $2.testo);
}
| unitempty testonosquare {
  $$.testo = makeTesto($1.ln, UNITEMPTYKEY, $1.lexema, NULL, $2.testo);
}
| unit testonosquare {
  $$.testo = makeTesto($1.ln, UNITKEY, NULL, $1.testo, $2.testo);
}
| folium testo {
  $$.testo = makeTesto($1.ln, FOLIUMKEY, NULL, $1.testo, $2.testo);
}
| teorema testo {
  $$.testo = makeTesto($1.ln, TEOREMAKEY, NULL, $1.testo, $2.testo);
}
| capitolo testo {
  $$.testo = makeTesto($1.ln, CAPITOLOKEY, NULL, $1.testo, $2.testo);
}
| aliter testo {
  $$.testo = makeTesto($1.ln, ALITERKEY, NULL, $1.testo, $2.testo);
}
| lemma testo {
  $$.testo = makeTesto($1.ln, LEMMAKEY, NULL, $1.testo, $2.testo);
}
| corollario testo {
  $$.testo = makeTesto($1.ln, COROLLARIOKEY, NULL, $1.testo, $2.testo);
}
| scolio testo {
  $$.testo = makeTesto($1.ln, SCOLIOKEY, NULL, $1.testo, $2.testo);
}
| additio testo {
  $$.testo = makeTesto($1.ln, ADDITIOKEY, NULL, $1.testo, $2.testo);
}
| sottocapitolo testo {
  $$.testo = makeTesto($1.ln, SOTTOCAPITOLOKEY, NULL, $1.testo, $2.testo);
}
| citatio testo {
  $$.testo = makeTesto($1.ln, CITKEY, NULL, $1.testo, $2.testo);
}
| data testo {
  $$.testo = makeTesto($1.ln, DATEKEY, NULL, $1.testo, $2.testo);
}
| formula testo {
  $$.testo = makeTesto($1.ln, FORMULAKEY, NULL, $1.testo, $2.testo);
}
| tavola testo {
  $$.testo = makeTesto($1.ln, TAVOLAKEY, NULL, $1.testo, $2.testo);
}
| figskip testo {
  $$.testo = makeTesto($1.ln, FIGSKIPKEY, NULL, $1.testo, $2.testo);
}
| figura testo {
  $$.testo = makeTesto($1.ln, FIGURAKEY, NULL, $1.testo, $2.testo);
}
| commento testo {
  $$.testo = makeTesto($1.ln, COMMKEY, NULL, $1.testo, $2.testo);
}
| congettura testo {
  $$.testo = makeTesto($1.ln, CONGKEY, NULL, $1.testo, $2.testo);
}
| opera testo {
  $$.testo = makeTesto($1.ln, OPERAKEY, NULL, $1.testo, $2.testo);
}
| nome testo {
  $$.testo = makeTesto($1.ln, NOMEKEY, NULL, $1.testo, $2.testo);
}
| luogo testo {
  $$.testo = makeTesto($1.ln, LUOGOKEY, NULL, $1.testo, $2.testo);
}
| libro testo {
  $$.testo = makeTesto($1.ln, LIBROKEY, NULL, $1.testo, $2.testo);
}
| adnotatio testo {
  $$.testo = makeTesto($1.ln, ADNOTKEY, NULL, $1.testo, $2.testo);
}
| footnote testo {
  $$.testo = makeTesto($1.ln, FOOTNOTEKEY, NULL, $1.testo, $2.testo);
}
| note testo {
  $$.testo = makeTesto($1.ln, NOTEKEY, NULL, $1.testo, $2.testo);
}
| prop testo {
  $$.testo = makeTesto($1.ln, PROPKEY, NULL, $1.testo, $2.testo);
}
| argomento testo {
  $$.testo = makeTesto($1.ln, ARGKEY, NULL, $1.testo, $2.testo);
}
| markup testo {
  $$.testo = makeTesto($1.ln, MARKKEY, NULL, $1.testo, $2.testo);
}
| cases testo {
  $$.testo = makeTesto($1.ln, CASES, NULL, $1.testo, $2.testo);
}
| scholium testo {
  $$.testo = makeTesto($1.ln, SCHOLKEY, NULL, $1.testo, $2.testo);
}
| varseria testo {
  $$.testo = makeTesto($1.ln, VVKEY, NULL, $1.testo, $2.testo);
}
| citlonga testo {
  $$.testo = makeTesto($1.ln, CITLONGAKEY, NULL, $1.testo, $2.testo);
}
| varfigura testo {
  $$.testo = makeTesto($1.ln, VFKEY, NULL, $1.testo, $2.testo);
}
| varbanale testo {
  $$.testo = makeTesto($1.ln, VBKEY, NULL, $1.testo, $2.testo);
}
| trasposizioneseria testo {
  $$.testo = makeTesto($1.ln, TVKEY, NULL, $1.testo, $2.testo);
}
| trasposizionebanale testo {
  $$.testo = makeTesto($1.ln, TBKEY, NULL, $1.testo, $2.testo);
}
| notamarg testo {
  $$.testo = makeTesto($1.ln, NMARGKEY, NULL, $1.testo, $2.testo);
}
| OMKEY testo {
  $$.testo = makeTesto($1.ln, OMKEY, NULL, NULL, $2.testo);
}
| NIHILKEY testo {
  $$.testo = makeTesto($1.ln, NIHILKEY, NULL, NULL, $2.testo);
}
| DELKEY testo {
  $$.testo = makeTesto($1.ln, DELKEY, NULL, NULL, $2.testo);
}
| EXPKEY testo {
  $$.testo = makeTesto($1.ln, EXPKEY, NULL, NULL, $2.testo);
}
| OMLACKEY testo {
  $$.testo = makeTesto($1.ln, OMLACKEY, NULL, NULL, $2.testo);
}
| LACMKEY testo {
  $$.testo = makeTesto($1.ln, LACMKEY, NULL, NULL, $2.testo);
}
| LACCKEY testo {
  $$.testo = makeTesto($1.ln, LACCKEY, NULL, NULL, $2.testo);
}
| LACSKEY testo {
  $$.testo = makeTesto($1.ln, LACSKEY, NULL, NULL, $2.testo);
}
| NLKEY testo {
  $$.testo = makeTesto($1.ln, NLKEY, NULL, NULL, $2.testo);
}
| COMMENTKEY testo {
  $$.testo = makeTesto($1.ln, COMMENTKEY, NULL, NULL, $2.testo);
}
| ANNOTAZKEY testo {
  $$.testo = makeTesto($1.ln, ANNOTAZKEY, NULL, NULL, $2.testo);
}
| lb testo {
  $$.testo = makeTesto($1.ln, LBKEY, NULL, $1.testo, $2.testo);
}
| unitref testo {
  $$.testo = makeTesto($1.ln, UNITREFKEY, NULL, $1.testo, $2.testo);
}
| aliamanus testo {
  $$.testo = makeTesto($1.ln, ALIAMANUSKEY, NULL, $1.testo, $2.testo);
}
| NOTAMACRO testo {
  $$.testo = makeTesto($1.ln, NOTAMACRO, $1.lexema, NULL, $2.testo);
}
| OUTRANGE testo {
  $$.testo = makeTesto($1.ln, OUTRANGE, NULL, NULL, $2.testo);
}
| nometestimone testo {
  $$.testo = makeTesto($1.ln, NOMTESTKEY, $1.lexema, NULL, $2.testo);
}
| semplice testo {
  $$.testo = makeTesto($1.ln, VVKEY, NULL, $1.testo, $2.testo);
}
| tabella testo {
  $$.testo = makeTesto($1.ln, TABLEKEY, NULL, $1.testo, $2.testo);
}
| tablonga testo {
  $$.testo = makeTesto($1.ln, LONGTABLEKEY, NULL, $1.testo, $2.testo);
}
| INTESTAKEY testo {
  $$.testo = makeTesto($1.ln, INTESTAKEY, NULL, NULL, $2.testo);
}
| numintest testo {
  $$.testo = makeTesto($1.ln, NUMINTESTKEY, NULL, $1.testo, $2.testo);
}
| mitintest testo {
  $$.testo = makeTesto($1.ln, MITINTESTKEY, NULL, $1.testo, $2.testo);
}
| desintest testo {
  $$.testo = makeTesto($1.ln, DESINTESTKEY, NULL, $1.testo, $2.testo);
}
| lmiintest testo {
  $$.testo = makeTesto($1.ln, LMIINTESTKEY, NULL, $1.testo, $2.testo);
}
| datintest testo {
  $$.testo = makeTesto($1.ln, DATINTESTKEY, NULL, $1.testo, $2.testo);
}
| comintest testo {
  $$.testo = makeTesto($1.ln, COMINTESTKEY, NULL, $1.testo, $2.testo);
}
| href testo {
  $$.testo = makeTesto($1.ln, HREFKEY, NULL, $1.testo, $2.testo);
}
| dag testo {
  $$.testo = makeTesto($1.ln, DAGKEY, NULL, $1.testo, $2.testo);
}
| NOINDENTKEY testo {
  $$.testo = makeTesto($1.ln, NOINDENTKEY, NULL, NULL, $2.testo);
}
| NEWPAGEKEY testo {
  $$.testo = makeTesto($1.ln, NEWPAGEKEY, NULL, NULL, $2.testo);
 }
| BEGINEPISTOLAKEY testo {
  $$.testo = makeTesto($1.ln, BEGINEPISTOLAKEY, NULL, NULL, $2.testo);
}
| ENDEPISTOLAKEY testo {
  $$.testo = makeTesto($1.ln, ENDEPISTOLAKEY, NULL, NULL, $2.testo);
}
| VSPACEKEY testobra testo {
  $$.testo = makeTesto($1.ln, VSPACEKEY, NULL, $2.testo, $3.testo);
}
| HYPHENKEY testo {
  $$.testo = makeTesto($1.ln, HYPHENKEY, NULL, NULL, $2.testo);
}
| frontespizio testo {
  $$.testo = makeTesto($1.ln, FRONTESPIZIOKEY, NULL, $1.testo, $2.testo);
}
| htmlcut testonosquare {
  $$.testo = makeTesto($1.ln, HTMLCUTKEY, NULL, $1.testo, $2.testo);
 };

CORR: CORRKEY testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, $2.testo);
}

aliamanus: ALIAMANUSKEY testobra testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
}

frontespizio: FRONTESPIZIOKEY testobra testobra nameslistbra {
  $$.testo =
    makeTesto($1.ln, NOKEY, NULL, $2.testo,
	      makeTesto($1.ln, NOKEY, NULL, $3.testo, $4.testo));
 }

nameslistbra: LEFTBRA nameslist RIGHTBRA {
  $$.testo = $2.testo;
 }

nameslist: testo {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $1.testo, NULL);
 }
| testo LISTSEPKEY nameslist {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $1.testo, $3.testo);
 }

htmlcut: HTMLCUTKEY {
  $$.testo = NULL;
 }
| HTMLCUTKEY testosquare {
  $$.testo = $2.testo;
 };

boldface: BOLDFACEKEY testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, $2.testo);
 }

smallcaps: SMALLCAPSKEY testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, $2.testo);
 }

italic: ITALICKEY testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, $2.testo);
}

href: HREFKEY testobra testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
}
| VOIDHREFKEY testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, $2.testo);
};

dag: DAGKEY testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, $2.testo);
}

numintest: NUMINTESTKEY testobra {
  $$.testo = $2.testo;
};

mitintest: MITINTESTKEY testobra {
  $$.testo = $2.testo;
};

desintest: DESINTESTKEY testobra {
  $$.testo = $2.testo;
};

lmiintest: LMIINTESTKEY testobra {
  $$.testo = $2.testo;
};

datintest: DATINTESTKEY testobra {
  $$.testo = $2.testo;
};

comintest: COMINTESTKEY testobra {
  $$.testo = $2.testo;
};

tablonga: BEGINKEY LEFTBRA LONGTABLEKEY RIGHTBRA wordbra optspaces righe ENDKEY LEFTBRA LONGTABLEKEY RIGHTBRA {
  $7.testo->lexema = $5.lexema;
  $$.testo  = $7.testo;
 };

tabella: BEGINKEY LEFTBRA TABLEKEY RIGHTBRA wordbra corpotab ENDKEY LEFTBRA TABLEKEY RIGHTBRA {
  $6.testo->lexema = $5.lexema;
  $$.testo  = $6.testo;
 }
| BEGINKEY LEFTBRA TABLEKEY RIGHTBRA wordbra corpotab ENDKEY wordbra {
  $6.testo->lexema = $5.lexema;
  $$.testo  = $6.testo;
  checkEnv("tabula", $3.ln, $8.lexema, $8.ln);
 }
| BEGINKEY LEFTBRA TABLEKEY RIGHTBRA error {
  $$.testo = NULL;
  yyemessage($4.ln, "dopo questo punto");
 };

corpotab: iniziocorpotab righe finecorpotab {
  $$.testo = makeTesto($1.ln, NOKEY, NULL,
		       makeTesto($1.ln, BEGINTABKEY, NULL, $1.testo, $3.testo),
		       $2.testo);
 };

iniziocorpotab: optspaces BEGINTABKEY testobra optspaces {
  $$.testo = $3.testo;
 }
| optspaces {
  $$.testo = NULL;
  };

finecorpotab: ENDTABKEY testobra optspaces {
  $$.testo = $2.testo;
 }
| {
  $$.testo = NULL;
 };

righe: riga {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, $1.testo);
 }
| righe SEPROWKEY optspaces riga {
  $$.testo = makeTesto($1.ln, SEPROWKEY, NULL, $1.testo, $4.testo);
 };

riga: HLINEKEY optspaces {
  $$.testo = makeTesto($1.ln, HLINEKEY, NULL, NULL, NULL);
 }
| campitab {
  $$.testo = $1.testo;
  };

campitab: colonna {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, $1.testo);
 }
| campitab SEPCOLKEY optspaces colonna {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $1.testo, $4.testo);
 };

colonna: campotab {
  $$.testo = $1.testo;
 }
| MULTICOLKEY numbra optspaces campotab {
  $$.testo = makeTesto($1.ln, MULTICOLKEY, $2.lexema, $4.testo, NULL);
 }
| MULTIROWKEY numbra optspaces campotab {
  $$.testo = makeTesto($1.ln, MULTIROWKEY, $2.lexema, $4.testo, NULL);
 };

campotab: CLINEKEY optspaces {
  $$.testo = makeTesto($1.ln, CLINEKEY, NULL, NULL, NULL);
 }
| LEFTGRATABKEY optspaces {
  $$.testo = makeTesto($1.ln, LEFTGRATABKEY, NULL, NULL, NULL);
 }
| LEFTANGTABKEY optspaces {
  $$.testo = makeTesto($1.ln, LEFTANGTABKEY, NULL, NULL, NULL);
 }
| RIGHTGRATABKEY optspaces {
  $$.testo = makeTesto($1.ln, RIGHTGRATABKEY, NULL, NULL, NULL);
 }
| RIGHTANGTABKEY optspaces {
  $$.testo = makeTesto($1.ln, RIGHTANGTABKEY, NULL, NULL, NULL);
 }
| testonospace {
  $$.testo = $1.testo;
};

nometestimone: NOMTESTKEY wordbra {
  $$.lexema = $2.lexema;
};

mano: /* LA MANO E` OMESSA */ {
  $$.lexema = NULL;
}
| wordsquare {
  $$.lexema = $1.lexema;
};

semplice: SMARGKEY mano testobra {
  $$.testo = makeSemplAgg($1.ln, $2.lexema, $3.testo, MARGKEY);
}
| SMARGSKEY mano testobra {
  $$.testo = makeSemplAgg($1.ln, $2.lexema, $3.testo, MARGSIGNKEY);
}
| SINTERLKEY mano testobra {
  $$.testo = makeSemplAgg($1.ln, $2.lexema, $3.testo, INTERLKEY);
}
| SSUPRAKEY mano testobra {
  $$.testo = makeSemplAgg($1.ln, $2.lexema, $3.testo, SUPRAKEY);
}
| SPCKEY mano testobra {
  $$.testo = makeSemplAgg($1.ln, $2.lexema, $3.testo, PCKEY);
}
| SBISKEY mano testobra {
  $$.testo = makeSemplAgg($1.ln, $2.lexema, $3.testo, BISKEY);
}
| SMCORRKEY mano testobra testobra {
  $$.testo = makeSemplCorr($1.ln, $2.lexema, $3.testo, $4.testo, MARGKEY);
}
| SMSCORRKEY mano testobra testobra {
  $$.testo = makeSemplCorr($1.ln, $2.lexema, $3.testo, $4.testo, MARGSIGNKEY);
}
| SICORRKEY mano testobra testobra {
  $$.testo = makeSemplCorr($1.ln, $2.lexema, $3.testo, $4.testo, INTERLKEY);
}
| SANTEDELKEY mano testobra testobra {
  $$.testo = makeSemplCanc($1.ln, $2.lexema, $3.testo, $4.testo, ANTEDELKEY);
}
| SPOSTDELKEY mano testobra testobra {
  $$.testo = makeSemplCanc($1.ln, $2.lexema, $3.testo, $4.testo, POSTDELKEY);
}
| SEXKEY mano testobra testobra {
  $$.testo = makeSemplCanc($1.ln, $2.lexema, $3.testo, $4.testo, EXKEY);
};

unitempty: UNITEMPTYKEY {
};

unit: UNITKEY option {
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, NULL, NULL);
  if ($2.lexema) {
    if (!addUnit($2.lexema)) {
      yyemessage($1.ln, "etichetta gia` utilizzata");
    }
  }
};

testobra: LEFTBRA testo RIGHTBRA {
  if ($3.ln - $1.ln > 20) {
    yywmessage($1.ln,"graffa aperta");
    yywmessage($3.ln,"chiusa in questo punto");
  }
  $$.testo = $2.testo;
}
| LEFTBRA error {
  $$.testo = NULL;
  yyemessage($1.ln, "dopo questo punto");
};

numbra: wordbra {
  $$.lexema = $1.lexema;
  if (strspn($$.lexema, "0123456789") != strlen($$.lexema)) {
    char msg[100];
    strcpy(msg, "letto <");
    strncat(msg, $$.lexema, 60);
    strcat(msg, "> mentre era atteso un numero");
    yyemessage($1.ln, msg);
  }
};

wordbra: LEFTBRA WORD RIGHTBRA {
  $$.lexema = $2.lexema;
}
| LEFTBRA RIGHTBRA {
  $$.lexema = "";
}
| LEFTBRA error {
  $$.lexema = NULL;
  yyemessage($1.ln, "dopo questo punto");
};

wordsquare: LEFTSQUARE WORD RIGHTSQUARE {
  $$.lexema = $2.lexema;
}
| LEFTSQUARE error {
  $$.lexema = NULL;
  yyemessage($1.ln, "dopo questo punto");
};

testoang: LEFTANG testo RIGHTANG {
  if ($3.ln - $1.ln > 20) {
    yywmessage($1.ln,"angolare aperta");
    yywmessage($3.ln,"chiusa in questo punto");
  }
  $$.testo = $2.testo;
}
| LEFTANG error {
  $$.testo = NULL;
  yyemessage($1.ln, "dopo questo punto");
};

title: TITLEKEY testobra {
  $$.testo = $2.testo;
}
| TITLEKEY error {
  yyemessage($1.ln, "nel titolo");
};

author: AUTHORKEY nameslistbra {
  $$.testo = $2.testo;
};

titledate: TITLEDATEKEY testobra {
  $$.testo = $2.testo;
};

section: SECTIONKEY testobra {
  sect++;
  ssect = 0;
  sssect = 0;
  $$.testo = $2.testo;
};

subsection: SUBSECTIONKEY testobra {
  ssect++;
  sssect = 0;
  $$.testo = $2.testo;
};

subsubsection: SSSECTIONKEY testobra {
  sssect++;
  $$.testo = $2.testo;
};

cite: CITEKEY testobra {
  $$.testo = $2.testo;
};

maurocite: MCITEKEY wordbra testobra {
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, NULL, $3.testo);
};

paragraph: PARAGRAPHKEY testobra {
  $$.testo = $2.testo;
};

label: LABELKEY wordbra {
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, NULL, NULL);
  if ($2.lexema) {
    if (!addLabelKey($2.lexema)) {
      yyemessage($1.ln, "etichetta gia` utilizzata");
    }
  }
};

letterlabel: LETTERLABELKEY wordbra wordbra {
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, makeTesto($1.ln, NOKEY, $3.lexema, NULL, NULL), NULL);
  if ($2.lexema) {
    if (!addLabelKey($2.lexema)) {
      yyemessage($1.ln, "etichetta gia` utilizzata");
    }
  }
};

ref: REFKEY wordbra {
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, NULL, NULL);
};

pageref: PAGEREFKEY wordbra {
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, NULL, NULL);
};

ambiente: BEGINKEY wordbra wordsquare testonosquare ENDKEY wordbra {
  checkEnv($2.lexema, $2.ln, $6.lexema, $6.ln);
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, makeTesto($3.ln, NOKEY, $3.lexema, NULL, NULL), $4.testo);
  $$.ln = $1.ln;
}
| BEGINKEY wordbra testonosquare ENDKEY wordbra {
  checkEnv($2.lexema, $2.ln, $5.lexema, $5.ln);
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, NULL, $3.testo);
  $$.ln = $1.ln;
};

index: INDEXKEY testobra {
  $$.testo = $2.testo;
};

sub: SUBKEY testobra {
  $$.testo = $2.testo;
};

supersubscript: SUPSUBKEY testobra testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
};

power: POWERKEY testobra {
  $$.testo = $2.testo;
};

sup: SUPKEY testobra {
  $$.testo = $2.testo;
};

titolo: TITKEY testobra {
  $$.testo = $2.testo;
};

personaggio: PERSKEY testobra {
  $$.testo = $2.testo;
};

rubrica: RUBRKEY testobra {
  $$.testo = $2.testo;
};

strike: STRIKEKEY testobra {
  $$.testo = $2.testo;
};

center: BEGINKEY LEFTBRA CENTERKEY RIGHTBRA testo ENDKEY LEFTBRA CENTERKEY RIGHTBRA {
  $$.testo = $5.testo;
} |
BEGINKEY LEFTBRA CENTERKEY RIGHTBRA testo ENDKEY wordbra {
  $$.testo = $5.testo;
  checkEnv("center", $3.ln, $7.lexema, $7.ln);
};

underline: UNDERLINE testobra {
  $$.testo = $2.testo;
};

greco: GREEKKEY testobra {
  $$.testo = $2.testo;
};

crux: CRUXKEY testobra {
  $$.testo = $2.testo;
};

expu: EXPUKEY testobra {
  $$.testo = $2.testo;
};

inte: INTEKEY testobra {
  $$.testo = $2.testo;
};

bibrif: BIBRIFKEY testobra {
  $$.testo = $2.testo;
};

citmarg: CITMARGKEY testobra {
  $$.testo = $2.testo;
};

citmargsign: CITMARGSIGNKEY testobra {
  $$.testo = $2.testo;
};

matematica: BEGINMATHKEY testo ENDMATHKEY {
  $$.testo = $2.testo;
}
| MATHKEY testobra {
  $$.testo = $2.testo;
};

equazione: BEGINEQNKEY testo ENDEQNKEY {
  $$.testo = $2.testo;
};

testocerchiato: TEXTCIRCLEDKEY testobra {
  $$.testo = $2.testo;
};

phantom: PHANTOMKEY testobra {
  $$.testo = $2.testo;
};

frazione: FRACKEY testobra testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
};

frazionemultipla: MFRACKEY nameslistbra nameslistbra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
};

ofrazionemultipla: OMFRACKEY nameslistbra nameslistbra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
};

frazionemultiplao: MFRACOKEY nameslistbra nameslistbra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
};

Title: TITLEWORKKEY testobra testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
};

lb: LBKEY wordbra testobra {
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, NULL, $3.testo);
  if (!addLabel($2.lexema, $3.testo)) {
    yyemessage($1.ln, "etichetta gia` utilizzata");
  }
};

contr: CONTRKEY testobra {
  $$.testo = $2.testo;
};

abbr: ABBRKEY testobra {
  $$.testo = $2.testo;
};

folium: FOLIUMKEY LEFTBRA testimone ':' WORD RIGHTBRA {
  $$.testo = makeTesto($1.ln, NOKEY, $3.lexema, NULL,
		       makeTesto($5.ln, NOKEY, $5.lexema, NULL, NULL));
}
| FOLIUMKEY LEFTBRA WORD RIGHTBRA {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL,
		       makeTesto($3.ln, NOKEY, $3.lexema, NULL, NULL));
}
| FOLIUMKEY LEFTBRA error {
  $$.testo = NULL;
  yyemessage($2.ln, "dopo questo punto");
};

teorema: TEOREMAKEY testobra {
  $$.testo = $2.testo;
};

elencotestimoni: ELENCOTESTIMONIKEY LEFTBRA allowedtestimoni RIGHTBRA {
  $$.testo = $3.testo;
}

allowedtestimoni: allowedtestimone {
  $$.testo = makeTesto($1.ln, NOKEY, $1.lexema, NULL, NULL);
}
| allowedtestimone SLASH allowedtestimoni {
  $$.testo = makeTesto($1.ln, NOKEY, $1.lexema, NULL, $3.testo);
};

allowedtestimone: WORD {
  $$.lexema = $1.lexema;
  if ($$.lexema && strcmp($$.lexema,"*") != 0) { // check stato siglum
    if (!addWitness($$.lexema)) {                // check inseribilità siglum in elencotestimoni
      char msg[100];
      strcpy(msg, $$.lexema);
      strcat(msg, " duplicato in elenco testimoni");
      yywmessage($1.ln, msg);
    }
  }
};

capitolo: CAPITOLOKEY testobra {
  $$.testo = $2.testo;
};

aliter: ALITERKEY testobra {
  $$.testo = $2.testo;
};

lemma: LEMMAKEY testobra {
  $$.testo = $2.testo;
};

corollario: COROLLARIOKEY testobra {
  $$.testo = $2.testo;
};

scolio: SCOLIOKEY testobra {
  $$.testo = $2.testo;
};

additio: ADDITIOKEY testobra {
  $$.testo = $2.testo;
};

sottocapitolo: SOTTOCAPITOLOKEY testobra {
  $$.testo = $2.testo;
};

citatio: CITKEY option listatestibra {
  if ($2.lexema) {
    if ((strcmp($2.lexema, "exp") != 0) &&
	(strcmp($2.lexema, "imp") != 0) &&
	(strcmp($2.lexema, "eqv") != 0)) {
      yyemessage($1.ln, "opzione sconosciuta per \\Cit");
    }
  }
  $3.testo->lexema = $2.lexema;
  $$.testo = $3.testo;
};

data: DATEKEY listatestibra {
  $$.testo = $2.testo;
};

listatestibra: LEFTBRA listatesti RIGHTBRA {
  if ($2.testo) {
    $$.testo = $2.testo;
  } else {
    $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, NULL);
    yyerror("nessun testo nella lista");
  }
}
| LEFTBRA error {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, NULL);
  yyemessage($1.ln, "dopo questo punto");
};

listatesti: optspaces {
  $$.testo = NULL;
}
| optspaces testobra listatesti {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo,$3.testo);
};

scholium: SCHOLKEY variante {
  $$.testo = $2.testo;
};

varfigura: VFKEY listacampibra {
  $2.testo->lexema = $1.lexema;
  $$.testo = $2.testo;
};

varseria: VVKEY variante {
  $$.testo = $2.testo;
};

citlonga: CITLONGAKEY  clvariante {
  $$.testo = $2.testo;
};


varbanale: VBKEY variante {
  $$.testo = $2.testo;
};

variante: option listacampibra {
  if ($1.lexema) {
    if ((strcmp($1.lexema, "longa") != 0)) {
      yyemessage($1.ln, "opzione sconosciuta per una variante");
    }
  }
  $2.testo->lexema = $1.lexema;
  $$.testo = $2.testo;
};

clvariante: cllistacampibra {
  $$.testo = $1.testo;
};


trasposizioneseria: TVKEY trasposizione {
  $$.testo = $2.testo;
};

trasposizionebanale: TBKEY trasposizione {
  $$.testo = $2.testo;
};

trasposizione: option listacampibra {
  if ($1.lexema) {
    if ((strcmp($1.lexema, "longa")     != 0) &&
		(strcmp($1.lexema, "multiplex") != 0) &&
		(strcmp($1.lexema, "duplex")    != 0) &&
		(strcmp($1.lexema, "unit")      != 0)) {
      yyemessage($1.ln, "opzione sconosciuta per una trasposizione");
    }
  }
  $2.testo->lexema = $1.lexema;
  $$.testo = $2.testo;
};

notamarg: NMARGKEY listacampibra {
  $$.testo = $2.testo;
};

listacampibra: LEFTBRA listacampi RIGHTBRA {
  if ($2.testo) {
    $$.testo = $2.testo;
  } else {
    $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, NULL);
    yyerror("nessun campo nella lista");
  }
}
| LEFTBRA error {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, NULL);
  yyemessage($1.ln, "dopo questo punto");
};

cllistacampibra: LEFTBRA cllistacampi RIGHTBRA {
  if ($2.testo) {
    $$.testo = $2.testo;
  } else {
    $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, NULL);
    yyerror("nessun campo nella lista");
  }
}
| LEFTBRA error {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, NULL);
  yyemessage($1.ln, "dopo questo punto");
};

listacampi: optspaces {
  $$.testo = NULL;
}
| optspaces campobra listacampi {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo,$3.testo);
};

cllistacampi: optspaces {
  $$.testo = NULL;
}
| optspaces wordbra cllistacampi2 {
  $$.testo = makeTesto($1.ln, NOKEY, $2.lexema, $2.testo,$3.testo);
};

cllistacampi2: optspaces {
  $$.testo = NULL;
}
| optspaces testobra cllistacampi3 {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo,$3.testo);
};

cllistacampi3: optspaces {
  $$.testo = NULL;
}
| optspaces testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo,NULL);
};


campobra: LEFTBRA campo RIGHTBRA {
  $$.testo = $2.testo;
}
| LEFTBRA error {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, NULL, NULL);
  yyemessage($1.ln, "Y dopo questo punto");
};

campo:
testimoni ':' testo {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $1.testo,
    makeTesto($1.ln, NOKEY, NULL, $3.testo, NULL));
}
| testimoni ':' sottocampi ':' testo {
  $$.testo =
    makeTesto($1.ln, NOKEY, NULL, $1.testo,
      makeTesto($1.ln, NOKEY, NULL, $5.testo, $3.testo));
}
| testimoni BANALEKEY ':' testo {
  $$.testo = makeTesto($1.ln, BANALEKEY, NULL, $1.testo,
    makeTesto($1.ln, NOKEY, NULL, $4.testo, NULL));
}
| testimoni BANALEKEY ':' sottocampi ':' testo {
  $$.testo =
    makeTesto($1.ln, BANALEKEY, NULL, $1.testo,
      makeTesto($1.ln, NOKEY, NULL, $6.testo, $4.testo));
};

testimoni: testimone {
  $$.testo = makeTesto($1.ln, NOKEY, $1.lexema, NULL, NULL);
}
| testimone SLASH testimoni {
  $$.testo = makeTesto($1.ln, NOKEY, $1.lexema, NULL, $3.testo);
};

testimone: WORD {
  $$.lexema = $1.lexema;
  if ($$.lexema && strcmp($$.lexema,"*") != 0) { // check stato siglum
    if (!findWitness($$.lexema)) {               // check presenza siglum in elencotestimoni
      char msg[100];
      strcpy(msg, $$.lexema);
      strcat(msg, " assente da elenco testimoni");
      yywmessage($1.ln, msg);
    }
  }
};

unitref: UNITREFKEY option wordbra {
  struct testo * testo1  = makeTesto($1.ln, NOKEY, $2.lexema, NULL, NULL);
  struct testo * testo2  = makeTesto($1.ln, NOKEY, $3.lexema, NULL, NULL);
  $$.testo = makeTesto($1.ln, NOKEY, NULL, testo1, testo2);
};

sottocampi: sottocampo {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $1.testo, NULL);
}
| sottocampo sottocampi {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $1.testo, $2.testo);
};

pstestobra: testobra {
  $$.testo = $1.testo;
}
| LEFTBRA PLKEY testobra RIGHTBRA {
  $$.testo = makeTesto($1.ln, PLKEY, NULL, $3.testo, NULL);
};

sottocampo: TRKEY testobra {
  $$.testo = makeTesto($1.ln, TRKEY, NULL, $2.testo, NULL);
}
| EDKEY testobra {
  $$.testo = makeTesto($1.ln, EDKEY, NULL, $2.testo, NULL);
}
| POSTSCRIPTKEY {
  $$.testo = makeTesto($1.ln, POSTSCRIPTKEY, NULL, NULL, NULL);
}
| ANTEDELKEY pstestobra {
  $$.testo = makeTesto($1.ln, ANTEDELKEY, NULL, $2.testo, NULL);
}
| POSTDELKEY pstestobra {
  $$.testo = makeTesto($1.ln, POSTDELKEY, NULL, $2.testo, NULL);
}
| EXKEY pstestobra {
  $$.testo = makeTesto($1.ln, EXKEY, NULL, $2.testo, NULL);
}
| CRKEY wordbra {
  $$.testo = makeTesto($1.ln, CRKEY, $2.lexema, NULL, NULL);
}
| REPKEY wordsquare {
  $$.testo = makeTesto($1.ln, REPKEY, $2.lexema, NULL, NULL);
}
| BISKEY {
  $$.testo = makeTesto($1.ln, BISKEY, NULL, NULL, NULL);
}
| INTERLKEY {
  $$.testo = makeTesto($1.ln, INTERLKEY, NULL, NULL, NULL);
}
| SUPRAKEY {
  $$.testo = makeTesto($1.ln, SUPRAKEY, NULL, NULL, NULL);
}
| MARGKEY {
  $$.testo = makeTesto($1.ln, MARGKEY, NULL, NULL, NULL);
}
| MARGSIGNKEY {
  $$.testo = makeTesto($1.ln, MARGSIGNKEY, NULL, NULL, NULL);
}
| PCKEY {
  $$.testo = makeTesto($1.ln, PCKEY, NULL, NULL, NULL);
}
| DESCOMPLKEY testobra testobra pstestobra {
  $$.testo = makeTesto($1.ln, DESCOMPLKEY,
		       NULL,
		       makeTesto($1.ln, NOKEY, NULL, $2.testo, 
				 makeTesto($1.ln, NOKEY, NULL, $3.testo, $4.testo)),
		       NULL);
}
| EDCOMPLKEY testobra testobra testobra {
  $$.testo = makeTesto($1.ln, EDCOMPLKEY,
		       NULL,
		       makeTesto($1.ln, NOKEY, NULL, $2.testo, 
				 makeTesto($1.ln, NOKEY, NULL, $3.testo, $4.testo)), 
		       NULL);
}
| LECKEY pstestobra {
  $$.testo = makeTesto($1.ln, LECKEY, NULL, $2.testo, NULL);
};

option:
/* NESSUNA OPZIONE */ {
  $$.lexema = NULL;
} |
wordsquare {
  $$.lexema = $1.lexema;
};

formula: FORMULAKEY opttestosquare testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
};

tavola: TAVOLAKEY opttestosquare testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
};

figskip: FIGSKIPKEY opttestosquare figuraevariante {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
}
|
FIGSKIPKEY opttestosquare wordbra {
      $$.testo = makeTesto($1.ln, NOKEY, $3.lexema, $2.testo, NULL);
};

figura: FIGURAKEY opttestosquare figuraevariante {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
}
|
FIGURAKEY opttestosquare wordbra {
  $$.testo = makeTesto($1.ln, NOKEY, $3.lexema, $2.testo, NULL);
};

figuraevariante: LEFTBRA testobra opttestosquare RIGHTBRA {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
}

fmarg: FMARGKEY opttestosquare testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
};

commento: COMMKEY testobra {
  $$.testo = $2.testo;
};

congettura: CONGKEY testobra {
  $$.testo = $2.testo;
};

opera: OPERAKEY testobra {
  $$.testo = $2.testo;
};

nome: NOMEKEY testobra {
  $$.testo = $2.testo;
};

luogo: LUOGOKEY testobra {
  $$.testo = $2.testo;
};

libro: LIBROKEY testobra {
  $$.testo = $2.testo;
};

adnotatio: ADNOTKEY opttestosquare testobra {
  if ($2.testo != NULL) {
    $$.lexema = $2.lexema;
    fprintf(stderr, "parse: %s\n", $2.lexema);
  }
  $$.testo = $3.testo;
};

footnote: FOOTNOTEKEY testobra {
  $$.testo = $2.testo;
}

note: NOTEKEY testobra {
  $$.testo = $2.testo;
};

prop: PROPKEY LEFTBRA listaopt RIGHTBRA {
  $$.testo = $3.testo;
};

argomento: ARGKEY testobra {
  $$.testo = $2.testo;
};

markup: MARKKEY testobra testobra {
  $$.testo = makeTesto($1.ln, NOKEY, NULL, $2.testo, $3.testo);
};

cases: CASES listatestibra {
  $$.testo = makeTesto($1.ln, NOKEY, $1.lexema, NULL, $2.testo);
};

%%

struct testo * makeTesto(long           ln,
			 long           key,
			 const char   * lexema,
			 struct testo * testo1,
			 struct testo * testo2) {

  struct testo * newTesto = new struct testo;

  if (newTesto) {
    newTesto->ln     = ln;
    newTesto->key    = key;
    newTesto->lexema = lexema;
    newTesto->testo1 = testo1;
    newTesto->testo2 = testo2;
  } else {
    yyerror(ERRNEW);
  }

  return newTesto;
}

struct testo * makeSemplAgg(long           ln,
			    const char   * testimone,
			    struct testo * lezione,
			    long           key) {
  return makeTesto(ln, NOKEY,
		   NULL,
		   makeTesto(ln, NOKEY,
			     NULL,
			     makeTesto(ln, NOKEY,
				       testimone,
				       NULL,
				       NULL),
			     makeTesto(ln, NOKEY,
				       NULL,
				       lezione,
				       makeTesto(ln, NOKEY,
						 NULL,
						 makeTesto(ln, key, NULL, NULL, NULL),
						 NULL))),
		   NULL);
}

struct testo * makeSemplCorr(long           ln,
			     const char   * testimone,
			     struct testo * lezcanc,
			     struct testo * lezione,
			     long           key) {
  return 
    makeTesto(ln, NOKEY,
	      NULL,
	      makeTesto(ln, NOKEY,
			NULL,
			makeTesto(ln, NOKEY, testimone, NULL, NULL),
			makeTesto(ln, NOKEY,
				  NULL,
				  lezione,
				  makeTesto(ln, NOKEY,
					    NULL,
					    makeTesto(ln, key, NULL, NULL, NULL),
					    NULL))),
	      makeTesto(ln, NOKEY,
			NULL,
			makeTesto(ln, NOKEY,
				  NULL,
				  makeTesto(ln, NOKEY, NULL, NULL, NULL),
				  makeTesto(ln, NOKEY,
					    NULL,
					    lezcanc,
					    ((testimone == NULL) ?
					     makeTesto(ln, NOKEY,
						       NULL,
						       makeTesto(ln, TRKEY,
								 NULL,
								 makeTesto(ln, NOKEY,
									   "ante corr.",
									   NULL,
									   NULL),
								 NULL),
						       NULL)
					     : NULL)
					    )
				  ),
			NULL)
	      );
}

struct testo * makeSemplCanc(long           ln,
			     const char   * testimone,
			     struct testo * lezcanc,
			     struct testo * lezione,
			     long           key) {
  return makeTesto(ln, NOKEY,
		   NULL,
		   makeTesto(ln, NOKEY,
			     NULL,
			     makeTesto(ln, NOKEY, testimone, NULL, NULL),
			     makeTesto(ln, NOKEY, NULL, lezione,
				       makeTesto(ln, NOKEY,
						 NULL,
						 makeTesto(ln, key, NULL, lezcanc, NULL),
						 NULL))),
		     NULL);
}

static rif * labels = NULL;
  
rif * findLabel(const char * label) {
  rif * l = labels;
  while (l != NULL && strcmp(label,l->label) != 0) {
    l = l->next;
  }
  return l;
}

bool addLabel(const char * label, struct testo * testo) {

  bool noninserito = (findLabel(label) == NULL);

  if (noninserito) {
    rif * l = new rif;
    l->label = label;
    l->nUnit = getunitnumber();
    l->testo = testo;
    l->next  = labels;
    labels = l;
  }

  return noninserito;
}

typedef struct unit {
  const char   * label;
  int            nUnit;
  unit         * next;
} unit;

static unit * units = NULL;

int findUnit(const char * label) {
  unit * u = units;
  while (u != NULL && strcmp(label,u->label) != 0) {
    u = u->next;
  }
  return (u == NULL) ? 0 : u->nUnit;
}

const char * findUnitLabel(short nUnit) {
  unit * u = units;
  while (u != NULL && u->nUnit != nUnit) {
    u = u->next;
  }
  return (u == NULL) ? NULL : u->label;
}

bool addUnit(const char * label) {
  bool noninserito = (findUnit(label) == 0);
  if (noninserito) {
    unit * u = new unit;
    u->label = label;
    u->nUnit = getunitnumber();
    u->next  = units;
    units = u;
  }
  return noninserito;
}

static labelKey * labelKeys = NULL;

labelKey * findLabelKey(const char * label) {
  
  labelKey * lab = labelKeys;

  while (lab != NULL && strcmp(label,lab->label) != 0) {
    lab = lab->next;
  }

  return lab;
}

bool addLabelKey (const char * label) {

  bool noninserito = (findLabelKey(label) == NULL);

  if (noninserito) {
    labelKey * l = new labelKey;
    l->label = label;
    l->sect = sect;
    l->ssect = ssect;
    l->sssect = sssect;
    l->next  = labelKeys;
    labelKeys = l;
  }

  return noninserito;
}

static wit * elencotestimoni = NULL;

wit * findWitness(const char * witness) {  
  wit * w = elencotestimoni;
  while (w != NULL && strcmp(witness,w->witness) != 0) {
    w = w->next;
  }
  return w;
}

bool addWitness(const char * witness) {
  bool noninserito = (findWitness(witness) == NULL);
  if (noninserito) {
    wit * w = new wit;
    w->witness = witness;
    w->next  = elencotestimoni;
    elencotestimoni = w;
  }
  return noninserito;
}

const char* ambienti[] = {"flushright", "minipage"};

bool checkGenericEnv(const char * ambiente) {
  bool value = false;
  int length = sizeof(ambienti)/sizeof(char *);
  for (int index = 0; index < length; index++) {
    if (strcmp(ambiente, ambienti[index]) == 0) {
      value = true;
    }
  }
  return value;
}

void checkEnv(const char * inizio, short lni, const char * fine, short lnf) {
  
  if (strcmp(inizio, fine) != 0) {
    char *message = new char[strlen(inizio) + strlen(fine) + 50];
    sprintf(message, "ambiente %s", inizio);
    yyemessage(lni, message);
    sprintf(message, "chiuso con %s", fine);
    yywmessage(lnf, message);
    delete[](message);
  }
}

void yyerror (const char * s) {
  yyemessage(getlinenumber(), s);
}

void yygmessage (FILE * out, int ln, const char * s) {
  fprintf(out, "%s.tex:%d: %s\n", fileName, ln, s);
}

void yyemessage (int ln, const char * s) {
  errore = true;
  fprintf(stderr, "ERRORE:\n");
  yygmessage(stderr, ln, s);
}

void yywmessage (int ln, const char * s) {
  yygmessage(stdout, ln, s);
}

char *fileName;
char *inFileName;

void parser (char * fn) {

  // RIMOZIONE DELL'EVENTUALE ESTENSIONE TEX
  char * lastdot = strstr(fn, ".tex");
  if (lastdot == NULL || strcmp(lastdot, ".tex") != 0) {
    fileName = fn;
  } else {
    int fnLen = strlen(fn)-4;
    fileName = new char[fnLen+1];
    strncpy(fileName, fn, fnLen);
    fileName[fnLen] = 0;
  }

  // APERTURA DEL FILE DI INPUT
  inFileName = new char[strlen(fileName)+5];
  if (inFileName) {
    sprintf(inFileName, "%s.tex", fileName);
    yyin = fopen(inFileName, "r");
    if (yyin == NULL) {
      fprintf(stderr, "impossibile aprire %s in lettura\n", inFileName);
    } else {

      // ANALISI DEL DOCUMENTO
      printf("analisi del file %s\n", inFileName);
      resetnumbers();
      yyparse();
      printf("analisi terminata\n");
      fclose(yyin);
    }
  } else {
    fprintf(stderr, ERRNEW);
  }
}
