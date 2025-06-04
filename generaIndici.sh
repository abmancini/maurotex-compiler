#!/bin/bash

############
#
# PARAMETRI
#
############

cartellaBase=`pwd`

# da dove si prendono i sorgenti
cartellaSorgenti=$cartellaBase/sitoSorgente

# da dove prendo la lista dei documenti che comporranno il sito
metaIndice=Indice.ual

# dove creo il sito
cartellaDestinazione=$cartellaBase/sitoDestinazione

# dove va la mappa con percorsi assoluti dei documenti
indiceCreato=IndiceAssoluto.ual

# Pulizia della cartella destinazione
rm -rf $cartellaDestinazione/*
mkdir -p $cartellaDestinazione
#touch $indiceCreato

# dove va la pianta del sito
indexCreato=pianta.htm
rm -fr $cartellaDestinazione/$indexCreato
touch $cartellaDestinazione/$indexCreato

# il nome del sito
nomeSito="/maurolico"

####################
#
# SETTAGGI INIZIALI
#
####################

traduttore=ual2h.py

# Azzeramenti del caso
livelloAttuale=1
titoloSalvato=""
percorso=""
stato=livello
titolo=""
depth=1

ultimoIndice=""
stampa () {
    echo -e $* >> $ultimoIndice
}

# Creazione della base dell'indice
indicizza () {
    echo -e $* >> $cartellaDestinazione/$indexCreato
}

indicizza "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
indicizza "<html>\n<head>"
indicizza "\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">"
indicizza "\t<title>Indice</title>"
indicizza "</head><body><table>"

# Creazione della base del toc
tocca () {
    echo -e $* >> $cartellaDestinazione/toc.htm
}

tocca "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
tocca "<html>\n<head>"
tocca "\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">"
tocca "\t<title>Table of Contents</title>"
tocca "\t<script type=\"text/javascript\">"
tocca "\tfunction aprichiudi(item) {"
tocca "\t\telem=document.getElementById(item);"
tocca "\t\tvisibile=(elem.style.display!=\"none\");"
tocca "\t\tprefisso=document.getElementById(\"menu\" + item);"
tocca "\t\tif (visibile) {"
tocca "\t\t\telem.style.display=\"none\";"
tocca "\t\t\tprefisso.innerHTML=\"<img src='micons/menu/cartella.gif' width='16' height='16' hspace='0' vspace='0' border='0'>\";"
tocca "\t\t} else {"
tocca "\t\t\telem.style.display=\"block\";"
tocca "\t\t\tprefisso.innerHTML=\"<img src='micons/menu/cartellaaperta.gif' width='16' height='16' hspace='0' vspace='0' border='0'>\";"
tocca "\t\t}"
tocca "\t}"
tocca "\tfunction apripagina(pagina) {"
tocca "\t\twindow.open(pagina,'principale');"
tocca "\t}</script>"
tocca "</head><body BGCOLOR=whitesmoke><table BORDER=0 WIDTH=\"100%\"><TR VALIGN=top><TD>"
tocca "<TABLE BORDER=1 WIDTH=\"100%\"><TR><TD>"
tocca "<TABLE BORDER=0 WIDTH=\"100%\"><TR><TD COLSPAN=2 BGCOLOR=gainsboro ALIGN=center><FONT SIZE=-1>Volumi</FONT></TD></TR></TABLE>"

#######
#
# VIA!
#
#######

# Una lista di dati che seguono il seguente schema di esempio:
#
# PROFONDITA	TITOLO			NOME		TIPO
#
# #       	"la_prima_pagina"       prima.htm       -H-
# ##      	"un_secondo_livello"    sotto.htm       -H-
# #       	"una_parallela"         seconda.htm     -H-
lista=`cat $cartellaSorgenti/$metaIndice | grep "#"`

for i in $lista; do

	###################################################################################
	# gestisco la PROFONDITA: la estraggo e eventualmente risalgo al livello richiesto	
	###################################################################################
	
	if [ $stato == livello ]; then
		profondita=`echo $i | grep "#" | wc -m`
		if [ $profondita != 0 ]; then
			let "profondita -= 1"
#			echo "VALUTO L'ELEMENTO: "$i" - profondita: "$profondita
			stato=titolo
			continue
		fi
	fi

	#####################################################
	# gestisco il TITOLO: lo estraggo e creo la cartella
	#####################################################
	
	if [ $stato == titolo ]; then
		titolo=$titolo" "$i
		fine=`echo $i | grep ".*;;"`
		if [ w$fine != w ]; then
			titoloSalvato=`echo $titolo | sed s/\"//g | sed s/";;"//`
			if [ "$titoloSalvato" != "" ]; then
				echo "        "$titoloSalvato
			fi
			inizio=`echo $titoloSalvato | grep -o ^[0-9][0-9]*[^\ ]*\.`
			inizioVolume=`echo $titoloSalvato | grep -o Volume`
			if [ "$inizioVolume" != "" ]; then
				percorso=`echo "$titoloSalvato" | grep -o "[0-9][0-9]*"`
			elif [ "$inizio" != "" ]; then
				percorso=`echo $inizio | sed s/\.$// | sed 's/\./\//g'`
			fi
			mkdir -p $cartellaDestinazione/$percorso
			cd $cartellaDestinazione/$percorso
			titolo=""
			stato=nome
			continue
		fi	

	fi

	################################
	# gestisco il NOME: lo estraggo
	################################

	if [ $stato == nome ]; then
		nomeHTML=`echo $i | grep htm$`
		nomeTEX=`echo $i | grep tex$`
		nomeNULL=`echo $i | grep null$`
		#nome=$nomeHTML$nomeTEX$nomeNULL
		nome=$i
		if [ w$nome != w ]; then
			#echo "-> riconosco il nome:" $nome
			nomeSalvato=$nome
			if [ w$nomeHTML != w ]; then
				nomeBase=`echo $nomeHTML | sed s/.htm//`
			elif [ w$nomeTEX != w ]; then
				nomeBase=`echo $nomeTEX | sed s/.tex//`
			elif [ w$nomeNULL == w ]; then
				nomeBase=$nome
			else
				nomeBase=""
			fi
			#echo "nome base ->" $nomeBase
			stato=tipo
			continue
		fi
	fi

	#############################################################
	# gestisco il TIPO: lo estraggo e scelgo che operazioni fare
	#############################################################
	if [ $stato == tipo ]; then
		tipo=`echo $i | grep -e "-[HTM]-" | sed s/-//g`
		if [ "$tipo" != "" ]; then
			#echo "-> riconosco il tipo:" "-"$tipo"-"

			#############
			# TIPO: Html
			#############

			if [ $tipo == H ]; then
				#echo "-> copio il documento Html: " $nomeSalvato
				cp $cartellaSorgenti/$nomeSalvato $cartellaDestinazione/$percorso/
				if [ $nomeSalvato == $indexCreato ]; then
					mv $cartellaDestinazione/$percorso/$nomeSalvato $cartellaDestinazione/$percorso/$nomeSalvato.temp
				fi

			##############
			# TIPO: LaTeX
			##############

			elif [ "$tipo" == "T" ]; then
				#echo "-> creo il documento LaTeX: " $nomeSalvato
				if [ "$nomeSalvato" == "null" ]; then
					touch intro.htm
					echo "<font size=\"+1\"><b>Introduzione in corso di realizzazione</b></font><br>" > intro.htm
				elif [ -f "$cartellaSorgenti/$nomeSalvato" ]; then
					cp $cartellaSorgenti/$nomeSalvato $cartellaDestinazione/$percorso/
					#cp $cartellaBase/$traduttore $cartellaDestinazione/$percorso/
					#echo $traduttore $nomeBase.tex
					cd $cartellaDestinazione/$percorso/
		                       	$traduttore $nomeBase.tex
		                       	#rm $traduttore
					mv $nomeBase.html intro.htm
					rm $nomeBase.tex
				else
					touch intro.htm
					echo "<font size=\"+1\"><b>Introduzione in corso di realizzazione</b></font><br>" > intro.htm
					echo  IL FILE $nomeSalvato NON ESISTE
				fi
				echo "Depth:$profondita Path:$nomeSito/$percorso/intro.htm Title:$titoloSalvato" >> $cartellaDestinazione/$indiceCreato
				ultimoIndice=`pwd`/intro.htm
				indicizza "<tr><td><div style=\"margin-left: "$profondita"em;\"><table><tr>"
				indicizza "<td><a href=\"$nomeSito/$percorso/intro.htm\">$titoloSalvato</a>"
				indicizza "</td></tr></table></td></tr>"

				oldAppunto=$appunto
				appunto=`echo $percorso | sed 's/\///g'`
				rientro=`echo $appunto | grep -o ^$oldAppunto`
				if [ $profondita -lt $depth ]; then
					while [ $profondita -lt $depth ]; do
						let "depth = depth - 1"
						tocca "</div>"
					done
				fi
				depth=$profondita
				if [ $rientro ]; then
					tocca "<div id=\"$oldAppunto\" style=\"display: none; margin-left: "$profondita"em;\">"
				fi
				tocca "<table cellpadding='1' cellspacing='1'><tr>"
				tocca "<td><a id=\"menu$appunto\" href=\"javascript:aprichiudi('$appunto');\"><img src='micons/menu/cartella.gif' width='16' height='16' hspace='0' vspace='0' border='0'></td>"
				tocca "<td><a href=\"javascript:apripagina('$nomeSito/$percorso/intro.htm')\">$titoloSalvato</a>"
				tocca "</td></tr></table>"

			#################
			# TIPO: MauroTeX
			#################

			elif [ $tipo == M ]; then
				#echo inizio trattamento $nomeBase
				cartellaLavoro=$cartellaSorgenti/$nomeBase
				if [ -f "$cartellaLavoro/$nomeBase.tex" ]; then
				    if [ -f "$cartellaLavoro/$nomeBase.fp" ]; then
						#echo "-> creo il documento MauroTeX: " $percorso/$nomeBase
						mkdir -p $cartellaDestinazione/$percorso/$nomeBase
						cd $cartellaDestinazione/$percorso/$nomeBase
						cp -rf $cartellaLavoro/$nomeBase* .
	                	mdoall.sh $nomeBase
						presenzaSottosezione=`cat $cartellaLavoro/$nomeBase.fp | grep -c ^titre_livre`
						if [ $presenzaSottosezione != 0 ]; then
							nomeSottosezione=`cat $cartellaLavoro/$nomeBase.fp | grep ^titre_livre | sed s/titre_livre=//`
							#echo $nomeSottosezione
							presenzaProposizioni=`cat $cartellaLavoro/$nomeBase.fp | grep -c ^titres_proposition`
							listaProposizioni=`cat $cartellaLavoro/$nomeBase.fp | grep ^titres_proposition | sed s/titres_propositions=// | sed s/\ /#/g | sed s/,/\ /g`
							stampa "<SS><font size=\"-1\">"$nomeSottosezione"</font><br>"
							contatore=0
							for p in $listaProposizioni; do
								pp=`echo $p | sed s/#/\ /g`
								let "contatore = contatore + 1"
								stampa "<SS><a href=\""$nomeBase/$nomeBase"-"$contatore"f.htm\"><font size=\"-1\">$pp</font></a>"
							done
						else
							presenzaProposizioni=0
						fi
						if [ $presenzaProposizioni == 0 ]; then
							stampa "<SS><a href=\""$nomeBase/$nomeBase"-1.htm\"><font size=\"-1\">Edizione</a>"
						fi
						stampa "<SS><br>"
	                	rm -rf old
				    else
						echo IL FILE $nomeBase.fp NON ESISTE
				    fi
				else
					echo IL FILE $nomeBase.tex NON ESISTE
				fi
				#echo fine trattamento $nomeBase
			fi

			#################################
			# comunque vada faccio questo...
			#################################
			nomeSalvato=""
			stato=livello
		fi
	fi
done 

tocca "</TD></TR></TABLE></TD><TD>&nbsp;&nbsp;&nbsp;</TD></TABLE></BODY></HTML>"


cp -rf $cartellaSorgenti/ $cartellaDestinazione/sorgenti
mv $cartellaDestinazione/sorgenti/micons $cartellaDestinazione/micons
mv $cartellaDestinazione/sorgenti/mtex $cartellaDestinazione/mtex
cat $cartellaDestinazione/$indexCreato >> $cartellaDestinazione/$indexCreato.temp
mv $cartellaDestinazione/$indexCreato.temp $cartellaDestinazione/$indexCreato
