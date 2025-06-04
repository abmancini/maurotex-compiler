
#include "standard.h"

Standard::Standard() {
  this->name = "standard";
  this->description = "creazione LaTeX standard";
}

void Standard::printPrologue(PARAMETRI * parametri) {
  fprintf(parametri->outFile, "\\usepackage{manyfoot}\n");
  fprintf(parametri->outFile, "\\newfootnote{vv}\n");
  fprintf(parametri->outFile, "\\newcounter{footnotevv}\n");
  fprintf(parametri->outFile, "\\newcommand{\\footnotemarkvvmath}{%%\n");
  fprintf(parametri->outFile, "\\stepcounter{footnotevv}%%\n");
  fprintf(parametri->outFile, "\\Footnotemark{[\\thefootnotevv]}}\n");
  fprintf(parametri->outFile, "\\newcommand{\\footnotemarkvv}{%%\n");
  fprintf(parametri->outFile, "\\stepcounter{footnotevv}%%\n");
  fprintf(parametri->outFile, "\\Footnotemark{\\thefootnotevv}}\n");
  fprintf(parametri->outFile, "\\newcommand{\\footnotetextvv}{%%\n");
  fprintf(parametri->outFile, "\\Footnotetextvv{\\thefootnotevv}}\n");
  fprintf(parametri->outFile, "\\newcommand{\\footnotevv}{%%\n");
  fprintf(parametri->outFile, "\\footnotemarkvv\\footnotetextvv}\n");

  fprintf(parametri->outFile, "\\newfootnote{cit}\n");
  fprintf(parametri->outFile, "\\newcounter{footnotecit}\n");
  fprintf(parametri->outFile, "\\renewcommand{\\thefootnotecit}{\\roman{footnotecit}}\n");
  fprintf(parametri->outFile, "\\newcommand{\\footnotecit}{%%\n");
  fprintf(parametri->outFile, "\\stepcounter{footnotecit}%%\n");
  fprintf(parametri->outFile, "\\Footnotemark{%%\n");
  fprintf(parametri->outFile, "\\textbf{[}%%\n");
  fprintf(parametri->outFile, "\\thefootnotecit%%\n");
  fprintf(parametri->outFile, "\\textbf{]}}%%\n");
  fprintf(parametri->outFile, "\\Footnotetextcit{\\thefootnotecit}}\n");

  fprintf(parametri->outFile, "\\newfootnote{nam}\n");
  fprintf(parametri->outFile, "\\newcounter{footnotenam}\n");
  fprintf(parametri->outFile, "\\renewcommand{\\thefootnotenam}{\\roman{footnotenam}}\n");
  fprintf(parametri->outFile, "\\newcommand{\\footnotenam}{%%\n");
  fprintf(parametri->outFile, "\\stepcounter{footnotenam}%%\n");
  fprintf(parametri->outFile, "\\Footnotemark{%%\n");
  fprintf(parametri->outFile, "\\textbf{(}%%\n");
  fprintf(parametri->outFile, "\\thefootnotenam%%\n");
  fprintf(parametri->outFile, "\\textbf{)}}%%\n");
  fprintf(parametri->outFile, "\\Footnotetextnam{\\thefootnotenam}}\n");

  fprintf(parametri->outFile, "\\newcommand\\inizioepistola{}\n");
  fprintf(parametri->outFile, "\\newcommand\\fineepistola{}\n");
}
