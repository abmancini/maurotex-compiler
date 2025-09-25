/*
  file "mparse.f"
  ANALIZZATORE LESSICALE

  mparse: controllore sintattico e generatore albero sintattico per M-TeX
  versione 1.16b1 del 25 febbraio 2025

  eLabor sc
  via Enrico Fermi 19, Pisa - Italia
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
%option noyywrap
      
%{

#include "mparse.h"   // IMPORTANTE:
#include "mparsein.h" // RISPETTARE
#include "mparsey.hpp"  // L'ORDINE
#include <string.h>

/*                                                                              */
/*                                   SIMBOLI                                    */
/*                                                                              */

/*                  TABELLA DEI SIMBOLI (LISTA DI STRINGHE CONTENENTI I LEXEMI) */
/*                                                                              */
typedef struct symtable                                                         {
  char *            lexema;
  int               num;
  struct symtable * next;
                                                                      } symtable;

struct symtable * install (int tipo);
void initlval (const char * lexema);

/* PUNTATORE ALLA TABELLA DEI SIMBOLI                                           */
/*                                                                              */
struct symtable * entry = NULL;

/*                                                                              */
/*                                   NUMERAZIONI                                */
/*                                                                              */

int line_number = 1;
int unit_number = 0;
int mathEnv     = 0;
int eqnEnv      = 0;

/*                                                            RESET NUMERAZIONI */
/*                                                                              */
void resetnumbers ()                                                            {
  line_number = 1;
  unit_number = 0;
  mathEnv     = 0;
  eqnEnv      = 0;
                                                                                }

/*                                                     GET NUMERAZIONI CORRENTI */
/*                                                                              */
int getlinenumber ()                                                            {
  return line_number;                                                           }
int getunitnumber ()                                                            {
  return unit_number;                                                           }


/*                                                                              */
/*                                   MACRO                                      */
/*                                                                              */

/*                                                             MACRO CONOSCIUTE */
/*                                                                              */
const char* allowedMacro[] = {
  "\\marginpar",
  "\\footnotesize",
  "\\raggedright",
  "\\endhead",
  "\\twocolumn",
  "\\onecolumn",
  "\\maurogreek",
  "\\ulcorner",
  "\\lrcorner",
  "\\left",
  "\\right",
  "\\int_",
  "\\mathcal"
};

/*                                                   CONTROLLO MACRO CONOSCIUTE */
/*                                                                              */
bool checkMacro(const char * currMacro)                                         {
  bool value = false;
  int length = sizeof(allowedMacro)/sizeof(char *);
  for (int index = 0; index < length; index++) {
    if (strcmp(currMacro, allowedMacro[index]) == 0) { value = true; }
  } return value;                                                               }

%}

/*                                   CATEGORIE SINTATTICHE                      */

inter           [;.!]
gravi           [����������]
acuti           [��]
outrange        [\x80-\xFF]
ignore          [\x0D]
delim           [ \t]
newline         \n
ws              {delim}+
alfa            [A-Za-z|+_]
segno           [-*@=?."]
digit           [0-9]
any             [^\n]
comment         "%"{any}*
word            ({alfa}|{digit}|{outrange}|{segno})+
nonl            [^\n\"]
grave           "\\`"(\{[aeiouAEIOU]\}|[aeiouAEIOU]|"\\i")
acute           "\\'"(\{[aeiouAEIOU]\}|[aeiouAEIOU]|"\\i")
circ            "\\^"(\{[aeiouAEIOU]\}|[aeiouAEIOU]|"\\i")
uml             "\\\""(\{[aeiouAEIOU]\}|[aeiouAEIOU]|"\\i")
tilde           "\\~n"
macro           "\\"("ae"|"oe"|"AE"|"OE"|"c e"|"c p"|"c c"|".o"|"v o"|"&"|"it\\&"|"ss"|"Alpha"|"Beta"|"Gamma"|"Delta"|"Epsilon"|"Zeta"|"Eta"|"Theta"|"Iota"|"Kappa"|"Lambda"|"Mu"|"Nu"|"Xi"|"Omicron"|"Pi"|"Rho"|"Sigma"|"Tau"|"Upsilon"|"Phi"|"Chi"|"Psi"|"Omega"|"alpha"|"beta"|"gamma"|"delta"|"varepsilon"|"epsilon"|"zeta"|"eta"|"theta"|"iota"|"kappa"|"lambda"|"mu"|"nu"|"xi"|"omicron"|"pi"|"rho"|"varsigma"|"sigma"|"tau"|"upsilon"|"varphi"|"phi"|"chi"|"psi"|"omega")
symbol          "\\"("CNJ"|"OPP"|"PRL"|"TRN"|"RTT"|"DRTT"|"CUB"|"PYR"|"PPD"|"PEN"|"HEX"|"HEXC"|"TRP"|"QDR"|"RDX"|"rdx"|"mtilde"|"ptilde"|"SOL"|"LUN"|"TER"|"MER"|"VEN"|"MAR"|"GIO"|"SAT"|"ARS"|"TRS"|"GMN"|"CNC"|"LEO"|"VRG"|"LBR"|"SCR"|"SGT"|"CPR"|"AQR"|"PSC"|"Stella"|"Terra"|hfill)
cases          "\\"((("R"|"L"|"RL")("brace")?)|"Void")cases
skipamount     "\\"("medskipamount"|"smallskipamount"|"bigskipamount")
falsamacro     ("c e"|"c p"|"c c"|".o"|"v o"|"it\\&"|ss|CNJ|OPP|PRL|TRN|RTT|DRTT|CUB|PPD|HEX|HEXC|PEN|TRP|QDR|RDX|rdx|mtilde|ptilde|SOL|LUN|TER|MER|VEN|MAR|GIO|SAT|ARS|TRS|GMN|CNC|LEO|VRG|LBR|SCR|SGT|CPR|AQR|PSC|Stella|Terra|(((R|L|RL)(brace)?)|Void)cases|BeginNM|EndNM|MauroTeX|MTeX|CON|PER|Tex|LaTeX|documentclass|cite|externaldocument|usepackage|setlength|parskip|parindent|medskipamount|FoliumInMargine|FoliumInTesto|begin|end|Sup|Sub|SupSub|title|maketitle|author|Title|Teorema|Capitolo|Aliter|Lemma|Corollario|Scolio|Additio|Sottocapitolo|section|subsection|subsubsection|paragraph|label|ref|underline|CONTR|ABBR|Marg|Commenti|Annotazioni|Folium|Cit|Date|Formula|Tavola|Figskip|Figura|Comm|Prop|Unit|UnitEmpty|UN|Adnotatio|note|SCHOL|VV|TV|VB|OM|NIHIL|DEL|EXP|OMLAC|NL|TR|ED|LACm|LACc|LACs|BIS|INTERL|SUPRA|MARG|MARGSIGN|ANTEDEL|POSTDEL|EX|LEC|PC|NOTAMARG|CRUX|EXPU|INTE|CR|LB|REP|CITMARG|CITMARGSIGN|frac|DB|SB|GG|Nome|Luogo|Opera|Libro|Cong|PL|Math|Tit|RUBR|strike|Arg|Markup|NomeTestimone|Smarg|Smargsign|Sinterl|Ssupra|Spc|Sbis|Smargcorr|Smargsigncorr|Sintcorr|Santedel|Spostdel|Sex|DESCOMPL|EDCOMPL|linea|regula|mcol|mrow|inizio|fine|lgra|rgra|lang|rang|htmlcut|numero|Testimoni|Dag|maurocite|times|quote|noindent|newpage|vskip|vspace|footnote|frontespizio|pageref|inizioepistola|fineepistola|marginpar|footnotesize|maurogreek)
notamacro       "\\"(({alfa})+|{segno})

%%

{falsamacro}           { yylval.lexema = install(MACRO)->lexema; return (WORD); }
,                      { initlval(NULL); return COMMA;                          }
\\BeginNM              { initlval(NULL); return BEGINNMKEY;                     }
\\EndNM                { initlval(NULL); return ENDNMKEY;                       }
{comment}              { /* IGNORA I COMMENTI */                                }
\\htmlcut              { initlval(NULL); return HTMLCUTKEY;                     }
\\href                 { initlval(NULL); return HREFKEY;                        }
\\voidhref             { initlval(NULL); return VOIDHREFKEY;                    }
\\\\                   { initlval(NULL); return SEPROWKEY;                      }
&                      { initlval(NULL); return SEPCOLKEY;                      }
\\numero               { initlval(NULL); return NUMINTESTKEY;                   }
\\destinatario         { initlval(NULL); return DESINTESTKEY;                   }
\\mittente             { initlval(NULL); return MITINTESTKEY;                   }
\\luogomitt            { initlval(NULL); return LMIINTESTKEY;                   }
\\data                 { initlval(NULL); return DATINTESTKEY;                   }
\\commento             { initlval(NULL); return COMINTESTKEY;                   }
\\intestazione         { initlval(NULL); return INTESTAKEY;                     }
\\linea                { initlval(NULL); return HLINEKEY;                       }
\\regula               { initlval(NULL); return CLINEKEY;                       }
\\mcol                 { initlval(NULL); return MULTICOLKEY;                    }
\\mrow                 { initlval(NULL); return MULTIROWKEY;                    }
\\inizio               { initlval(NULL); return BEGINTABKEY;                    }
\\fine                 { initlval(NULL); return ENDTABKEY;                      }
\\lgra                 { initlval(NULL); return LEFTGRATABKEY;                  }
\\rgra                 { initlval(NULL); return RIGHTGRATABKEY;                 }
\\lang                 { initlval(NULL); return LEFTANGTABKEY;                  }
\\rang                 { initlval(NULL); return RIGHTANGTABKEY;                 }
"`"                    { initlval(NULL); return SLQUOTEKEY;                     }
"'"                    { initlval(NULL); return SRQUOTEKEY;                     }
"\`\`"                 { initlval(NULL); return DLQUOTEKEY;                     }
"\'\'"                 { initlval(NULL); return DRQUOTEKEY;                     }
--                     { initlval(NULL); return LONGKEY;                        }
---                    { initlval(NULL); return ADKEY;                          }
\\textcircled          { initlval(NULL); return TEXTCIRCLEDKEY;                 }
\\banale               { initlval(NULL); return BANALEKEY;                      }
\\MauroTeX             { initlval(NULL); return MAUROTEXKEY;                    }
\\MTeX                 { initlval(NULL); return MTEXKEY;                        }
\\PER                  { initlval(NULL); return PERKEY;                         }
\\CON                  { initlval(NULL); return CONKEY;                         }
\\ElencoTestimoni      { initlval(NULL); return ELENCOTESTIMONIKEY;             }
\\TeX                  { initlval(NULL); return TEXKEY;                         }
\\LaTeX                { initlval(NULL); return LATEXKEY;                       }
\\documentclass        { initlval(NULL); return CLASSKEY;                       }
\\Testimoni            { initlval(NULL); return TESTIMONIKEY;                   }
\\usepackage           { initlval(NULL); return USEPACKAGE;                     }
\\externaldocument     { initlval(NULL); return EXTERNALDOCUMENT;               }
\\cite                 { initlval(NULL); return CITEKEY;                        }
\\maurocite            { initlval(NULL); return MCITEKEY;                       }
\\geometry             { initlval(NULL); return GEOMETRY;                       }
\\setlength            { initlval(NULL); return SETLENGTH;                      }
\\parskip              { initlval(NULL); return PARSKIP;                        }
\\parindent            { initlval(NULL); return PARINDENT;                      }
\\FoliumInMargine      { initlval(NULL); return FOLIUMMARGOPT;                  }
\\FoliumInTesto        { initlval(NULL); return FOLIUMTESTOOPT;                 }
\\begin                { initlval(NULL); return BEGINKEY;                       }
\\end                  { initlval(NULL); return ENDKEY;                         }
\\par                  { initlval(NULL); return PARAGRAPH;                      }
\\pstart               { initlval(NULL); return PARAGRAPHSTART;                 }
\\pend                 { initlval(NULL); return PARAGRAPHEND;                   }
\\Sup                  { initlval(NULL); return SUPKEY;                         }
\\Sub                  { initlval(NULL); return SUBKEY;                         }
\\SupSub               { initlval(NULL); return SUPSUBKEY;                      }
\\title                { initlval(NULL); return TITLEKEY;                       }
\\maketitle            { initlval(NULL); return MAKETITLEKEY;                   }
\\author               { initlval(NULL); return AUTHORKEY;                      }
\\date                 { initlval(NULL); return TITLEDATEKEY;                   }
\\Title                { initlval(NULL); return TITLEWORKKEY;                   }
\\section              { initlval(NULL); return SECTIONKEY;                     }
\\subsection           { initlval(NULL); return SUBSECTIONKEY;                  }
\\subsubsection        { initlval(NULL); return SSSECTIONKEY;                   }
\\paragraph            { initlval(NULL); return PARAGRAPHKEY;                   }
\\label                { initlval(NULL); return LABELKEY;                       }
\\letterlabel          { initlval(NULL); return LETTERLABELKEY;                 }
\\pageref              { initlval(NULL); return PAGEREFKEY;                     }
\\ref                  { initlval(NULL); return REFKEY;                         }
\\underline            { initlval(NULL); return UNDERLINE;                      }
\\CONTR                { initlval(NULL); return CONTRKEY;                       }
\\Tit                  { initlval(NULL); return TITKEY;                         }
\\Personaggio          { initlval(NULL); return PERSKEY;                        }
\\RUBR                 { initlval(NULL); return RUBRKEY;                        }
\\strike               { initlval(NULL); return STRIKEKEY;                      }
\\ABBR                 { initlval(NULL); return ABBRKEY;                        }
\\Marg                 { initlval(NULL); return FMARGKEY;                       }
\\Commenti             { initlval(NULL); return COMMENTKEY;                     }
\\Annotazioni          { initlval(NULL); return ANNOTAZKEY;                     }
\\=                    { initlval(NULL); return VECTORKEY;                      }
\\Folium               { initlval(NULL); return FOLIUMKEY;                      }
\\Teorema              { initlval(NULL); return TEOREMAKEY;                     }
\\Capitolo             { initlval(NULL); return CAPITOLOKEY;                    }
\\Aliter               { initlval(NULL); return ALITERKEY;                      }
\\Lemma                { initlval(NULL); return LEMMAKEY;                       }
\\Corollario           { initlval(NULL); return COROLLARIOKEY;                  }
\\Scolio               { initlval(NULL); return SCOLIOKEY;                      }
\\Additio              { initlval(NULL); return ADDITIOKEY;                     }
\\Sottocapitolo        { initlval(NULL); return SOTTOCAPITOLOKEY;               }
\\Cit                  { initlval(NULL); return CITKEY;                         }
\\Citlonga             { initlval(NULL); return CITLONGAKEY;                    }
\\Date                 { initlval(NULL); return DATEKEY;                        }
\\Formula              { initlval(NULL); return FORMULAKEY;                     }
\\Tavola               { initlval(NULL); return TAVOLAKEY;                      }
\\Figskip              { initlval(NULL); return FIGSKIPKEY;                     }
\\Figura               { initlval(NULL); return FIGURAKEY;                      }
\\Comm                 { initlval(NULL); return COMMKEY;                        }
\\Prop                 { initlval(NULL); return PROPKEY;                        }
\\Opera                { initlval(NULL); return OPERAKEY;                       }
\\Nome                 { initlval(NULL); return NOMEKEY;                        }
\\Luogo                { initlval(NULL); return LUOGOKEY;                       }
\\Cong                 { initlval(NULL); return CONGKEY;                        }
\\Libro                { initlval(NULL); return LIBROKEY;                       }
\\Arg                  { initlval(NULL); return ARGKEY;                         }
\\Markup               { initlval(NULL); return MARKKEY;                        }
\\UnitEmpty            { initlval(NULL); return UNITEMPTYKEY;                   }
\\Bibrif               { initlval(NULL); return BIBRIFKEY;                      }
\\Unit                 { initlval(NULL); unit_number++; return UNITKEY;         }
\\UN                   { initlval(NULL); return UNITREFKEY;                     }
\\Adnotatio            { initlval(NULL); return ADNOTKEY;                       }
\\note                 { initlval(NULL); return NOTEKEY;                        }
\\SCHOL                { initlval(NULL); return SCHOLKEY;                       }
\\VB                   { initlval(NULL); return VBKEY;                          }
\\VF                   { initlval(NULL); return VFKEY;                          }
\\VV                   { initlval(NULL); return VVKEY;                          }
\\TV                   { initlval(NULL); return TVKEY;                          }
\\TB                   { initlval(NULL); return TBKEY;                          }
\\OM                   { initlval(NULL); return OMKEY;                          }
\\NIHIL                { initlval(NULL); return NIHILKEY;                       }
\\DEL                  { initlval(NULL); return DELKEY;                         }
\\EXP                  { initlval(NULL); return EXPKEY;                         }
\\OMLAC                { initlval(NULL); return OMLACKEY;                       }
\\NL                   { initlval(NULL); return NLKEY;                          }
\\PL                   { initlval(NULL); return PLKEY;                          }
\\DES                  { initlval(NULL); return TRKEY;                          }
\\ED                   { initlval(NULL); return EDKEY;                          }
\\POSTSCRIPT           { initlval(NULL); return POSTSCRIPTKEY;                  }
\\LACm                 { initlval(NULL); return LACMKEY;                        }
\\LACc                 { initlval(NULL); return LACCKEY;                        }
\\LACs                 { initlval(NULL); return LACSKEY;                        }
\\BIS                  { initlval(NULL); return BISKEY;                         }
\\INTERL               { initlval(NULL); return INTERLKEY;                      }
\\SUPRA                { initlval(NULL); return SUPRAKEY;                       }
\\MARG                 { initlval(NULL); return MARGKEY;                        }
\\MARGSIGN             { initlval(NULL); return MARGSIGNKEY;                    }
\\ANTEDEL              { initlval(NULL); return ANTEDELKEY;                     }
\\POSTDEL              { initlval(NULL); return POSTDELKEY;                     }
\\EX                   { initlval(NULL); return EXKEY;                          }
\\LEC                  { initlval(NULL); return LECKEY;                         }
\\PC                   { initlval(NULL); return PCKEY;                          }
\\NOTAMARG             { initlval(NULL); return NMARGKEY;                       }
\\CRUX                 { initlval(NULL); return CRUXKEY;                        }
\\EXPU                 { initlval(NULL); return EXPUKEY;                        }
\\INTE                 { initlval(NULL); return INTEKEY;                        }
\\CORR                 { initlval(NULL); return CORRKEY;                        }
\\CitMarg              { initlval(NULL); return CITMARGKEY;                     }
\\CitMargSign          { initlval(NULL); return CITMARGSIGNKEY;                 }
\\CR                   { initlval(NULL); return CRKEY;                          }
\\LB                   { initlval(NULL); return LBKEY;                          }
\\REP                  { initlval(NULL); return REPKEY;                         }
\\frac                 { initlval(NULL); return FRACKEY;                        }
\\DB                   { initlval(NULL); return DSKEY;                          }
\\SB                   { initlval(NULL); return SSKEY;                          }
\\GG                   { initlval(NULL); return GREEKKEY;                       }
\\Dag                  { initlval(NULL); return DAGKEY;                         }
\\Math                 { initlval(NULL); return MATHKEY;                        }
"\\em"                 { initlval(NULL); return EMPHASIS;                       }
"\\it"                 { initlval(NULL); return ITALIC;                         }
"\\sl"                 { initlval(NULL); return SLANTED;                        }
"\\bf"                 { initlval(NULL); return BOLDFACE;                       }
"\\tt"                 { initlval(NULL); return TYPEFACE;                       }
"\\rm"                 { initlval(NULL); return ROMAN;                          }
"\\["                  { initlval(NULL); return BEGINEQNKEY;                    }
"\\]"                  { initlval(NULL); return ENDEQNKEY;                      }
"\\("                  { initlval(NULL); return BEGINMATHKEY;                   }
"\\)"                  { initlval(NULL); return ENDMATHKEY;                     }
"\\,"                  { initlval(NULL); return NBSPACEKEY;                     }
"\\flat"               { initlval(NULL); return FLATKEY;                        }
"\\natural"            { initlval(NULL); return NATURALKEY;                     }
"\\cdot"               { initlval(NULL); return CDOTKEY;                        }
"\\vert"               { initlval(NULL); return VERTKEY;                        }
"\\lbracket"           { initlval(NULL); return LBRACKETKEY;                    }
"\\rbracket"           { initlval(NULL); return RBRACKETKEY;                    }
"\\Manus"              { initlval(NULL); return MANUSKEY;                       }
"\\AliaManus"          { initlval(NULL); return ALIAMANUSKEY;                   }
"\\S"                  { initlval(NULL); return SKEY;                           }
"\\dots"               { initlval(NULL); return DOTSKEY;                        }
"\\times"              { initlval(NULL); return TIMESKEY;                       }
"\\textsc"             { initlval(NULL); return SMALLCAPSKEY;                   }
"\\textbf"             { initlval(NULL); return BOLDFACEKEY;                    }
"\\emph"               { initlval(NULL); return ITALICKEY;                      }
"\\textit"             { initlval(NULL); return ITALICKEY;                      }
"\\item"               { initlval(NULL); return ITEMKEY;                        }
"\\noindent"           { initlval(NULL); return NOINDENTKEY;                    }
"\\newpage"            { initlval(NULL); return NEWPAGEKEY;                     }
"\\vspace"             { initlval(NULL); return VSPACEKEY;                      }
"\\-"                  { initlval(NULL); return HYPHENKEY;                      }
"\\footnote"           { initlval(NULL); return FOOTNOTEKEY;                    }
"\\frontespizio"       { initlval(NULL); return FRONTESPIZIOKEY;                }
"\\inizioepistola"     { initlval(NULL); return BEGINEPISTOLAKEY;               }
"\\fineepistola"       { initlval(NULL); return ENDEPISTOLAKEY;                 }
"\\PreloadUnicodePage" { initlval(NULL); return PRELOADKEY;                     }
"\\phantom"            { initlval(NULL); return PHANTOMKEY;                     }
"#"                    { initlval(NULL); return LISTSEPKEY;                     }
"$$"                   { initlval(NULL); 
                         if (eqnEnv == 0) { eqnEnv = 1; return BEGINEQNKEY;} 
                         else             { eqnEnv = 0; return ENDEQNKEY;  }    }
"$"                    { initlval(NULL);
                         if (mathEnv == 0) { mathEnv = 1; return BEGINMATHKEY;} 
                         else              { mathEnv = 0; return ENDMATHKEY;  } }
"^"                    { initlval(NULL); return POWERKEY;                       }
"_"                    { initlval(NULL); return INDEXKEY;                       }
document               { initlval(NULL); return DOCENVKEY;                      }
center                 { initlval(NULL); return CENTERKEY;                      }
ltabula                { initlval(NULL); return LONGTABLEKEY;                   }
tabula                 { initlval(NULL); return TABLEKEY;                       }
"{"                    { initlval(NULL); return LEFTBRA;                        }
"}"                    { initlval(NULL); return RIGHTBRA;                       }
"("                    { initlval(NULL); return LEFTPAR;                        }
")"                    { initlval(NULL); return RIGHTPAR;                       }
"<"                    { initlval(NULL); return LEFTANG;                        }
">"                    { initlval(NULL); return RIGHTANG;                       }
"["                    { initlval(NULL); return LEFTSQUARE;                     }
"]"                    { initlval(NULL); return RIGHTSQUARE;                    }
"\\"                   { initlval(NULL); return '\\';                           }
"~"                    { initlval(NULL); return TILDEKEY;                       }
":"                    { initlval(NULL); return ':';                            }
"/"                    { initlval(NULL); return SLASH;                          }
\\NomeTestimone        { initlval(NULL); return NOMTESTKEY;                     }
\\Smarg                { initlval(NULL); return SMARGKEY;                       }
\\Smargsign            { initlval(NULL); return SMARGSKEY;                      }
\\Sinterl              { initlval(NULL); return SINTERLKEY;                     }
\\Ssupra               { initlval(NULL); return SSUPRAKEY;                      }
\\Spc                  { initlval(NULL); return SPCKEY;                         }
\\Sbis                 { initlval(NULL); return SBISKEY;                        }
\\Smargcorr            { initlval(NULL); return SMCORRKEY;                      }
\\Smargsigncorr        { initlval(NULL); return SMSCORRKEY;                     }
\\Sintcorr             { initlval(NULL); return SICORRKEY;                      }
\\Santedel             { initlval(NULL); return SANTEDELKEY;                    }
\\Spostdel             { initlval(NULL); return SPOSTDELKEY;                    }
\\Sex                  { initlval(NULL); return SEXKEY;                         }
\\DESCOMPL             { initlval(NULL); return DESCOMPLKEY;                    }
\\EDCOMPL              { initlval(NULL); return EDCOMPLKEY;                     }
\\mfrac                { initlval(NULL); return MFRACKEY;                       }
\\omfrac               { initlval(NULL); return OMFRACKEY;                      }
\\mfraco               { initlval(NULL); return MFRACOKEY;                      }
{ws}                   { initlval(NULL); return SPACES;                         }
{newline}              { initlval(NULL); line_number++; return NEWLINE;         }
{word}                 { initlval(install(WORD)->lexema); return (WORD);        }
{inter}                { initlval(install(WORD)->lexema); return (INTER);       }
{grave}                { initlval(install(WORD)->lexema); return (GRAVE);       }
{acute}                { initlval(install(WORD)->lexema); return (ACUTE);       }
{uml}                  { initlval(install(WORD)->lexema); return (UML);         }
{tilde}                { initlval(install(WORD)->lexema); return (TILDE);       }
{circ}                 { initlval(install(WORD)->lexema); return (CIRC);        }
{macro}                { initlval(install(WORD)->lexema); return (MACRO);       }
{symbol}               { initlval(install(WORD)->lexema); return (SYMBOL);      }
{skipamount}           { initlval(install(WORD)->lexema); return (WORD);        }
{cases}                { initlval(install(WORD)->lexema); return (CASES);       }
{notamacro}            { initlval(install(WORD)->lexema);
                         char msg[100];
                         strncpy(msg, yytext, yyleng);
                         msg[yyleng] = 0;
                         if (!checkMacro(msg)) {
                          strcat(msg, ": macro inesistente");
                          yywmessage(getlinenumber(), msg);
                         } return NOTAMACRO;                                    }
{ignore}               { /* IGNORA IL TOKEN */                                  }



%%



/* CREA UNA STRINGA CONTENENTE IL TESTO PUNTATO DA st PER sl CARATTERI          */
/*                                                                              */
char * makestring (char * st, size_t sl)                                        {
  char *s = new char[sl+1];
  if (s) {
          memcpy (s, st, sl);                  /* COPIA I CARATTERI             */
          s [sl] = 0;                          /* SEGNALA LA FINE DELLA STRINGA */
         }
  return s;                                                                     }


/* RICERCA NELLA TABELLA DEI SIMBOLI IL LEXEMA PUNTATO DA s
   RITORNA NULL O IL PUNTATORE ALLA TABELLA                                     */
/*                                                                              */
struct symtable * lookup (struct symtable *entry, char *s)                      {
  if ((entry == NULL) ||
      (strcmp (entry->lexema, s) == 0)) return entry;
  else                                  return ( lookup (entry->next, s) );     } 


/* GESTIONE DELLA TABELLA DEI SIMBOLI: 
   I SIMBOLI POSSONO ESSERE IDENTIFICATORI O COSTANTI NUMERICHE,
   CHE COMUNQUE VENGONO VISTE COME STRINGHE; 
   LA FUNZIONE UTILIZZA LE VARIABILI GLOBALI YYTEXT E YYLENG, 
   FORNITE DA LEX, 
   E RITORNA IL PUNTATORE AD UN ELEMENTO DELLA TABELLA DEI SIMBOLI, 
   GENERANDOLO SE NON ESISTE.                                                   */
/*                                                                              */
struct symtable * install (int tipo)                                            {
  char *s;
  struct symtable *l;
  char msg[100];
  l = lookup (entry, yytext);                /* RICERCA IL LEXEMA NELLA TABELLA */
  if (l == NULL) {                           /* SE NON ESISTE LO GENERA         */
                  s = makestring (yytext, yyleng);
                  l = new (struct symtable);
                  if (l) {
                          l->lexema = s;
                          l->num    = 1;
                          l->next   = entry;
                          entry     = l;
                         }
                 } 
  else           { 
                  l->num++; 
                 }
  if (tipo == MACRO && 
      verboseWarning()) { 
                         sprintf(msg, "%s: nome di macro nel testo", l->lexema);
                         yywmessage(getlinenumber(), msg);
                        }
  return l;                                                                     }


/*                                                                              */
/*                                                                              */
void dumpsymtable (FILE * ok, FILE * ko)                                        {
  FILE *out;
  struct symtable *l = entry;
  while (l) {
    if (
        (strcspn(l->lexema,"\\.?!0123456789'`+*-:,;@/") == strlen(l->lexema)) 
        &&
        (strlen(l->lexema) > 1 || strspn(l->lexema, "AEIOU") == 1)
       ) 
       { out = ok; } else { out = ko; }
    fprintf(out, "%s\t%5d\n", l->lexema, l->num);
    l = l->next;
  }                                                                             }


/* INIZIALIZZAZIONE STRUTTURA LVAL                                              */
/*                                                                              */
void initlval (const char * lexema)                                             {
  yylval.lexema = lexema;
  yylval.ln = getlinenumber();                                                  }
