
#include "epistolario.h"

Epistolario::Epistolario() {
  this->name = "epistolario";
  this->description = "creazione LaTeX per epistolario";
}

void Epistolario::printPrologue(PARAMETRI * parametri) {
  fprintf(parametri->outFile, "\\makepagenote\n");
  fprintf(parametri->outFile, "\\let\\footnote\\pagenote\n");
  fprintf(parametri->outFile, "\\let\\footnotevv\\pagenote\n");
  fprintf(parametri->outFile, "\\let\\footnotecit\\pagenote\n");
  fprintf(parametri->outFile, "\\let\\footnotenam\\pagenote\n");
  fprintf(parametri->outFile, "\\let\\Adnotatio\\pagenote\n");
  fprintf(parametri->outFile, "\\let\\Annotazioni\\relax\n");
  fprintf(parametri->outFile, "\\renewcommand\\notedivision{\\par\\vskip12pt}\n");
  fprintf(parametri->outFile, "\\renewcommand\\pagenotesubhead[2]{}\n");
  fprintf(parametri->outFile, "\\renewcommand\\prenoteinnotes{\\bgroup\\footnotesize}\n");
  fprintf(parametri->outFile, "\\renewcommand\\postnoteinnotes{\\egroup}\n");
  fprintf(parametri->outFile,
	  "\\renewcommand{\\notenuminnotes}[1]{{\\normalfont\\ifnum#1=1\\relax\\else|| \\fi#1.}~}\n");
  fprintf(parametri->outFile,
	  "\\newcommand\\inizioepistola{\\setcounter{pagenote}{0}\\setcounter{NumUnit}{0}}\n");
  fprintf(parametri->outFile, "\\newcommand\\fineepistola{\\printnotes*}\n");
  fprintf(parametri->outFile, "\\makeatletter \n");
  fprintf(parametri->outFile,
	  "\\def\\letterlabel#1#2{\\immediate\\write\\@auxout{\\string\\newlabel{#1}{{\\theNumUnit}{#2}}}}\n");
}

