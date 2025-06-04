#!/bin/bash

############
#
# PARAMETRI
#
############

# dove va la mappa dei documenti
indiceCreato=index.htm

# da dove si prendono i sorgenti
cartellaDuso=./sitoDestinazione

# da dove prendo la lista di cui sotto
metaIndice=IndiceAssoluto.ual

# il nome del sito
nomeSito="/maurolico"

####################
#
# SETTAGGI INIZIALI
#
####################

cartellaBase=`pwd`

# il LIVELLO di partenza
livelloAttuale=1

#######
# VIA!
#######

lista=`cat $cartellaDuso/$metaIndice`

stampa () {
	echo -e $* >> $cartellaDuso/$percorsoFile
}

#pagine=`cat $cartellaDuso/$metaIndice | grep -o "http.*[! ]" | sed s/\ .*//`
#pagine=`cat $cartellaDuso/$metaIndice | grep -o "Path.* Title" | sed s/Path\:// | sed s/\ Title//`
#echo $pagine

IFS=$'\n'

#########################################################
# Cosa faccio per ciascuna pagina in IndiceAssoluto.ual
#########################################################

for target in $(cat $cartellaDuso/$metaIndice); do

	#########################
	# Parametri del documento
	#########################

	#echo "target: "$target

	# Il titolo della pagina
	titoloPagina=`echo $target | grep -o Title.* | sed s/Title\://`
	echo $titoloPagina

	# Il percorso della pagina
	#percorsoPagina=`echo $target | grep -o "http.*[! ]" | sed s/\ .*//`
	percorsoPagina=`echo $target | grep -o "Path.* Title" | sed s/Path\:// | sed s/\ Title//`

	# il percorso del file
	percorsoFile=`echo $percorsoPagina | grep -o [0-9].*$`
	#echo "percorsoFile: "$percorsoFile

	# La profondità della pagina
	profonditaPagina=`echo $target | grep -o "Depth:[0-9]*" | sed s/Depth\://`
	#echo "ProfonditaPag: "$profonditaPagina

	# Copia temporanea del documento per inserirne in seguito il contenuto nella pagina
	if [ "$percorsoFile" != "" ]; then
		cp $cartellaDuso/$percorsoFile $cartellaDuso/$percorsoFile.temp
		echo > $cartellaDuso/$percorsoFile
	fi

	# RESET di vari parametri [per ???]
	profondita=1
	inserisci=1
	percorso=""
	profonditaFormattata=0
	profonditaTemp=1
	limite=0
	trovato=""

	#echo "=== PROCESSO: "$titoloPagina" - LIVELLO: "$profonditaPagina" ==="

	##################################################
	# Costruzione della pagina: intro e parte sinistra
	##################################################

	# Intestazione della pagina
	stampa "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
	stampa "<html>\n<head>"
	stampa "\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">"
	stampa "\t<title>"$titoloPagina"</title>"
	stampa "<script type=\"text/javascript\">"
	stampa "\tfunction apritoc() {"
	stampa "\t\twindow.name=\"principale\";"
	stampa "\t\twindow.open('/maurolico/toc.htm','toc','width=500,height=500');\n\t}\n</script>"
	stampa "</head>"

	# Tabella di sinistra
	stampa "<body bgcolor=\"whitesmoke\">\n\t<table border=\"0\" width=\"100%\">\n\t<tr valign=\"top\"><td width=\"300\">"
	stampa "\t\t<table border=\"1\" width=\"100%\">\n\t\t<tr><td>"
	stampa "\t\t\t<table border=\"0\" width=\"100%\">"

	stampa "\t\t\t<table border=\"0\" width=\"100%\" cellpadding=\"2\">"
	stampa "\t\t\t<tr bgcolor=\"gainsboro\"><td align=\"center\"><font size=\"-1\">Sommario generale</font></td>"
	stampa "\t\t\t<td align=\"right\"><font size=\"-1\"><a href=\"javascript:apritoc()\">TOC</a></font></td></tr>"

	stampa "<TR><TD><FONT SIZE=\"-1\"><A HREF=\"$nomeSito/intro.htm\">Il progetto Maurolico</A></FONT></TD></TR>"
	stampa "<TR><TD><FONT SIZE=\"-1\"><A HREF=\"$nomeSito/proget.htm\">Descrizione del progetto</A></FONT></TD></TR>"
	stampa "<TR><TD><FONT SIZE=\"-1\"><A HREF=\"$nomeSito/comite.htm\">Comitato scientifico e collaboratori</A></FONT></TD></TR>"
	stampa "<TR><TD><FONT SIZE=\"-1\"><A HREF=\"$nomeSito/mtex/mtex.htm\">Il Mauro-TeX</A></FONT></TD></TR>"
	stampa "<TR><TD><FONT SIZE=\"-1\"><A HREF=\"$nomeSito/pianta.htm\">Pianta del sito</A></FONT></TD></TR>"
	stampa "<TR><TD><FONT SIZE=\"-1\"><A HREF=\"$nomeSito/help.htm\">Help</A></FONT></TD></TR>"
	stampa "<TR><TD><FONT SIZE=\"-1\"><A HREF=\"$nomeSito/search.htm\">Ricerca nel sito</A></FONT></TD></TR>"
	stampa "<TR BGCOLOR=gainsboro><TD ALIGN=center colspan=2><FONT SIZE=-1>Volumi</FONT></TD></TR>"

	stampa "<tr><td colspan=\"2\"><div style=\"margin-left: "$rientro"em;\"><table><tr>"
	#stampa "<td valign=\"top\" align=\"right\"><font size=\"-1\">$separatore</font>"
	#stampa "<td><font size=\"-1\"><a href=\"$percorsoLink\">$titolo</a></font>"
	#stampa "</td></tr>"
	stampa "</table></td></tr>"

	###################################################################
	# Costruzione dell'indice laterale: per ogni pagina dell'elenco...
	###################################################################

#	trovato=""
#	stop=0
#	limite=1
#	profonditaOld=1
	#echo $(cat $cartellaDuso/$metaIndice)
	for i in $(cat $cartellaDuso/$metaIndice); do
		#echo $i
		profondita=`echo $i | grep -o "Depth:[0-9]" | sed s/Depth://`
		#percorsoLink=`echo $i | grep -o "http.*[! ]" | sed s/\ .*//`
		percorsoLink=`echo $i | grep -o "Path.* Title" | sed s/Path\:// | sed s/\ Title//`
		percorsoFileLink=`echo $percorsoLink | grep -o [0-9].*$`
		livello=`echo $percorsoFileLink | sed s/intro.htm//`

		#echo "profondita: "$profondita" - limite: "$limite" - trovato: "$trovato

		padre=${percorsoLink%/*}
		nonno=${padre%/*}
		if [ "${percorsoPagina:0:${#nonno}}" == "$nonno" ]; then
			titolo=`echo $i | grep -o Title.* | sed 's/Title.*\.//' | sed 's/Volume\ //'`
			separatore=`echo $livello | sed 's/\//./g' | grep -o [A-Z0-9][A-Z0-9]*.$`
			let "rientro = profondita - 1"
			if [ "$percorsoPagina" == "$percorsoLink" ]; then
				stampa "<tr><td colspan=\"2\"><div style=\"margin-left: "$rientro"em;\"><table><tr>"
				stampa "<td valign=\"top\" align=\"right\" bgcolor=\"darkgray\"><font size=\"-1\" color=\"white\">$separatore</font>"
				stampa "<td bgcolor=\"darkgray\"><font size=\"-1\" color=\"white\">$titolo</font>"
				stampa "</td></tr></table></td></tr>"
				sottosezione=`cat $cartellaDuso/$percorsoFile.temp | grep -c "<SS>"`
				if [ $sottosezione != 0 ]; then
					corpoSottosezione=`cat $cartellaDuso/$percorsoFile.temp | grep "<SS>"`
					stampa "<tr><td colspan=\"2\"><div style=\"margin-left: "$rientro"em;\"><table><tr>"
					stampa "<td valign=\"top\" align=\"right\"><font size=\"-1\">&nbsp;</font>"
					stampa "<td><font size=\"-1\">"
					for c in $corpoSottosezione; do
						riga=`echo $c | sed 's/<SS>//'`
						stampa $riga
					done
					stampa "</font></td><td ALIGN=center BGCOLOR=yellow><A HREF=\"$nomeSito/niv.htm\"><FONT SIZE=\"-1\">Livello 0</A></td>"
					stampa "</td></tr></table></td></tr>"
					cat $cartellaDuso/$percorsoFile.temp | sed 's/<SS>.*//' > $cartellaDuso/$percorsoFile.temp
				fi
			else
				stampa "<tr><td colspan=\"2\"><div style=\"margin-left: "$rientro"em;\"><table><tr>"
				stampa "<td valign=\"top\" align=\"right\"><font size=\"-1\">$separatore</font>"
				stampa "<td><font size=\"-1\"><a href=\"$percorsoLink\">$titolo</a></font>"
				stampa "</td></tr></table></td></tr>"
			fi
		fi

		#########################
		# gestisco la PROFONDITA
		#########################
#		if [ $profondita -le $limite ]; then
#			inserisci=1
#			if [ $profondita -lt $profonditaOld ]; then
#				if [ $stop == 1 ]; then
#					if [ $profonditaOld -le $limite ]; then
#						let "limite = limite - 1"
#						#echo "scalo limite per aver trovato il MAX"
#					fi
#					if [ $profondita == 1 ]; then
#						let "limite = 1"
#					fi
#				fi
#			fi
#			if [ w`echo $percorsoFile | grep "^$livello"` != w ]; then
#				let "limite = limite + 1"
#				#echo "aumento limite per giusto percorso"
#				inserisci=1
#			fi
#		fi
		
		#echo "percorsoFileLink: "$percorsoFileLink" - inserisci: "$inserisci
		#echo "percorsoFile: "$percorsoFile" - livello: "$livello

#		if [ "$trovato" != "" ];then
#			if [ $profondita -eq $profonditaOld ]; then
#				let "limite = limite - 1"
#				#echo "diminuisco limite per mancanza di giusto percorso"
#			fi
#		fi
#
#		trovato=`echo $percorsoFile | grep -s "^$percorsoFileLink"`
#		if [ "$trovato" != "" ];then
#			stop=1
#			inserisci=2
#		fi

		#################################
		# gestisco il NOME del documento
		#################################

#		livelloOld=$livello
#		profonditaOld=$profondita

		#####################
		# gestisco il TITOLO
		#####################
		#echo $titolo
		#if [ $inserisci == 1 ]; then
		# per la pagina stessa
		#elif [ $inserisci == 2 ]; then
		#fi
	done
	stampa "\t\t\t</table>"
	stampa "\t\t</td></tr>\n\t\t</table>"
	stampa "\t</td>\n\t<td>&nbsp;&nbsp;&nbsp;</td>\n\t<td>" 

	##############################################
	# Costruzione della parte destra della pagina
	##############################################
        stampa "<CENTER><TABLE WIDTH=\"100%\" BORDER=0><TR><TD ALIGN=center BGCOLOR=darkgray><FONT SIZE=\"-1\" COLOR=white>"     

	stampa "F&nbsp;&nbsp;r&nbsp;&nbsp;a&nbsp;&nbsp;n&nbsp;&nbsp;c&nbsp;&nbsp;i&nbsp;&nbsp;s&nbsp;&nbsp;c&nbsp;&nbsp;i"
	stampa "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
	stampa "M&nbsp;&nbsp;a&nbsp;&nbsp;u&nbsp;&nbsp;r&nbsp;&nbsp;o&nbsp;&nbsp;l&nbsp;&nbsp;y&nbsp;&nbsp;c&nbsp;&nbsp;i"
	
	stampa "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
	stampa "O&nbsp;&nbsp;p&nbsp;&nbsp;e&nbsp;&nbsp;r&nbsp;&nbsp;a"
	stampa "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
	stampa "M&nbsp;&nbsp;a&nbsp;&nbsp;t&nbsp;&nbsp;h&nbsp;&nbsp;e&nbsp;&nbsp;m&nbsp;&nbsp;a&nbsp;&nbsp;t&nbsp;&nbsp;i&nbsp;&nbsp;c&nbsp;&nbsp;a"

	stampa "<a href=\"$nomeSito/sorgenti/index.htm\"><img src=\"$nomeSito/micons/trans.gif\" BORDER=\"0\" alt=\"trans\"></a>"	
	stampa "</FONT></TD></TR></TABLE></CENTER>"

	stampa "\t\t<table width=\"100%\" border=\"0\">\n\t\t<tr><td>\n\t\t\t<h1>"$titoloPagina"</h1>\n\t\t</td>"
	data=`date`
	stampa "\t\t<td align=\"right\">\n\t\t\t<font color=\"dimgray\" size=\"-1\">"$data"</font>\n\t\t</td></tr>\n\t\t</table>\n\n\t\t<br><hr>"

	corpo=`cat $cartellaDuso/$percorsoFile.temp`
	stampa $corpo

	# stampa della barra in basso con il collegamento per tornare all'inizio della pagina
	stampa "\t\t<p>\n\t\t<table width=\"100%\" border=\"0\">"
	stampa "\t\t\t<tr><td align=\"center\" bgcolor=\"silver\"><font color=\"white\"><a href=\"#haut\">Inizio della pagina</a></font></td></tr>"
	stampa "\t\t</table>"

	# chiusura della parte di destra e della pagina HMTL
	stampa "\t</td></tr>\n\t</table></body>\n</html>"

done
