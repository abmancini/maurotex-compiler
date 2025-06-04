#! /usr/bin/env python
# -*- coding: iso-8859-1 -*-
#
# Jean-Pierre Sutto, 25 mars 2003, 14, 16 avril 2006
#
# ajoute les entêtes aux pages du texte critique.
# usage menttc2.py radical
# This is GPL free software

import mroutines
import string
import sys
import os

print '-------------- menttc ----------------'

sito='/maurolico/'

# récupération arguments
radical=sys.argv[1]
print "radical=*",radical,"*"
fp=radical+'.fp'
print "fp=*",fp,"*"

# vérification de l'existence du fichier particulier
if not(os.path.isfile(fp)):
	print 'Il n\'y a pas de fichier',fp
	sys.exit(1)

# récupération de variables du fichier particulier
s=mroutines.readfile(fp)
liste=['titre_oeuvre','titre_livre','livre_suivant','intro']
titre_oeuvre,titre_livre,livre_suivant,intro=mroutines.readfp(s,liste)
print "titre_oeuvre=*",titre_oeuvre,"*"

# récupération de la variable titres_propositions
titres_propositions=mroutines.recup_titres_propositions(radical)
print "titres_propositions=*",titres_propositions,"*"

# pdf
if os.path.isfile(radical+'.pdf'):
	pdf=1
else:
	pdf=0
print "pdf=*",pdf,"*"

# position intro
print "intro=",intro
if intro=="@":
	intro="intro.htm"

# position du sommaire
position_sommaire=mroutines.position_sommaire(radical)
if position_sommaire=="":
	position_sommaire=sito
print 'position_sommaire=*',position_sommaire,'*'

# on cherche si il y a des fichiers avec frames
frames=0
for i in range(len(titres_propositions)):
	if os.path.isfile(radical+'-'+repr(i+1)+'f.htm'):
		frames=1
print 'frames=*',frames,'*'

# nombre de propositions-1
dernier=len(titres_propositions)-1
if dernier == 0:
	print 'dernier=0. Cela indique une unique proposition.'

# constantes
rc="\n"
tab="\t"

# date interne à récupérer
date=mroutines.date()

# on boucle sur la liste des propositions
for i in range(len(titres_propositions)):

	# on récupère les titres courant, précédent, suivant
	titre=titres_propositions[i]
	fichier=radical+'-'+repr(i+1)
	if i!=0:
		titre_prec=titres_propositions[i-1]
		fichier_prec=radical+'-'+repr(i)
	if i!=dernier:
		titre_suiv=titres_propositions[i+1]
		fichier_suiv=radical+'-'+repr(i+2)

	# entête
	t="""<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>

<head>
	<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
	<title>tc</title>
	<!-- Jean-Pierre Sutto """
	t=t+date
	t=t+""" -->
	<!-- created with m2hv and Mauro-TeX (P. Mascellani), -->
	<!-- python, latex and linux. -->
</head>

<body bgcolor=white>

<a name="haut"></a>

"""

	# premier tableau: Francisci Maurolici Opera Matematica
	t=t+"""<table width="100%" border=1>
	<tr align=center><td bgcolor=teal><font size="-1" color=white>

F&nbsp;&nbsp;r&nbsp;&nbsp;a&nbsp;&nbsp;n&nbsp;&nbsp;c&nbsp;&nbsp;i&nbsp;&nbsp;s&nbsp;&nbsp;c&nbsp;&nbsp;i
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
M&nbsp;&nbsp;a&nbsp;&nbsp;u&nbsp;&nbsp;r&nbsp;&nbsp;o&nbsp;&nbsp;l&nbsp;&nbsp;y&nbsp;&nbsp;c&nbsp;&nbsp;i
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
O&nbsp;&nbsp;p&nbsp;&nbsp;e&nbsp;&nbsp;r&nbsp;&nbsp;a
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
M&nbsp;&nbsp;a&nbsp;&nbsp;t&nbsp;&nbsp;h&nbsp;&nbsp;e&nbsp;&nbsp;m&nbsp;&nbsp;a&nbsp;&nbsp;t&nbsp;&nbsp;i&nbsp;&nbsp;c&nbsp;&nbsp;a

	</font></td></tr>
</table>

"""

	# deuxième tableau: liens généraux
	t=t+'<table width="100%" border=1 cellpadding=3>'+rc\
	+tab+'<tr align=center valign=middle>'+rc

	t=t+tab+'<td bgcolor=yellow><a href="'+intro+'" target="_top">\
	Introduzione</a></td>'+rc

	if pdf == 1:
		t=t+tab+'<td bgcolor=yellow><a href="'+fichier+'.pdf" \
		target="_top">Pdf</a></td>'+rc

	t=t+tab+'<td bgcolor=yellow><a href="' + sito + 'help.htm" target="_top">Help</a></td>'+rc
	
	t=t+tab+'<td bgcolor=yellow><a href="' + sito + 'pianta.htm" target="_top">Pianta</a></td>'+rc
	
	t=t+tab+'<td bgcolor=yellow><a href="' + position_sommaire+\
	'intro.htm" target="_top">Sommario</a></td>'+rc

	t=t+tab+'</tr>'+rc+'</table>'+rc+rc

	# troisième tableau: titres
	t=t+'<table width="100%" border=1 cellpadding=3>'+rc\
	+tab+'<tr align=center valign=middle>'+rc

	t=t+tab+'<td bgcolor=lightgreen><b>'+titre_oeuvre+'</b></td>'+rc

	if titre_livre != "@":
		t=t+tab+'<td bgcolor=lightgreen><b>'+titre_livre+'</b></td>'+rc

	if dernier != 0:
		t=t+tab+'<td bgcolor=lightgreen><b>'+titre+'</b></td>'+rc

	t=t+tab+'</tr>'+rc+'</table>'+rc+rc

	# quatrième tableau: liens entre propositions
	t=t+'<table width="100%" border=1 cellpadding=3>'+rc\
	+tab+'<tr align=center valign=middle>'+rc

	# frames
	if frames == 1 and  i != 0:
		t=t+tab+'<td bgcolor=darkturquoise><a href="'\
		+fichier_prec+'f.htm" target="_top">&lt;-</a></td>'+rc
	elif frames ==1 and dernier !=0:
		t=t+tab+'<td bgcolor=darkturquoise><b>|-</b></td>'+rc

	if frames == 1:
		t=t+tab+'<td bgcolor=darkturquoise><a href="'\
		+fichier+'f.htm" target="_top">App.</a></td>'+rc
	
	if frames == 1 and  i != dernier:
		t=t+tab+'<td bgcolor=darkturquoise><a href="'\
		+fichier_suiv+'f.htm" target="_top">-&gt;</a></td>'+rc
	elif frames ==1 and dernier !=0:
		t=t+tab+'<td bgcolor=darkturquoise><b>-|</b></td>'+rc

	# sans frame
	if i != 0 and dernier !=0:
		t=t+tab+'<td bgcolor=paleturquoise><a href="'\
		+fichier_prec+'.htm" target="_top">&lt;-</a></td>'+rc
	elif dernier !=0:
		t=t+tab+'<td bgcolor=paleturquoise><b>|-</b></td>'+rc

	if dernier !=0 or frames == 1:
		t=t+tab+'<td bgcolor=paleturquoise><a href="'\
		+fichier+'.htm" target="_top">=</a></td>'+rc
	
	if i != dernier and dernier !=0:
		t=t+tab+'<td bgcolor=paleturquoise><a href="'\
		+fichier_suiv+'.htm" target="_top">-&gt;</a></td>'+rc
	elif dernier !=0:
		t=t+tab+'<td bgcolor=paleturquoise><b>-|</b></td>'+rc

	t=t+tab+'</tr>'+rc+'</table>'+rc+rc

	# entêtes du corps
	t=t+'<table width="100%" border=0 cellpadding=5 bgcolor=white>'+rc
	t=t+tab+'<tr>'+rc+tab+'<td>'+rc+tab+'<!-- corps  -->'+rc+rc
	
	#recupération du corps
	corps=mroutines.readfile(fichier+'.htm')
	corps=string.replace(corps,'<html>','')
	corps=string.replace(corps,'<head>','')
	corps=string.replace(corps,'</head>','')
	corps=string.replace(corps,'<body bgcolor="white">','')
	corps=string.replace(corps,'</body>\n</html>','')
	corps=string.replace(corps,'<body>','')
	corps=string.replace(corps,'<font size=-1>','<font size="-1">')
	corps=string.replace(corps,'<font size=+1>','<font size="+1">')
	corps=string.replace(corps,'<font size=+2>','<font size="+2">')
	corps=string.replace(corps,'</p>','')
	
	# pieds du corps
	t=t+corps+rc+tab+'<!-- /corps  -->'+rc+rc+tab+'</td>'+rc
	t=t+tab+'</tr>'+rc+'</table>'+rc+rc
	
	# pied de page
	t=t+'<table width="100%" border=1 cellpadding=3>'+rc
	t=t+tab+'<tr align=center>'+rc
	t=t+tab+'<td bgcolor=yellow>'
	t=t+'<a href="#haut">Inizio della pagina</a><br>'
	t=t+'</td>'+rc
	
	# fichier suivant ou livre suivant dans le pied de page
	if i == dernier and livre_suivant != "@":
 		t=t+tab+'<td bgcolor=paleturquoise>'
		t=t+'<a href="'+livre_suivant+'" target="_top">=-&gt;</a>'
		t=t+'</td>'+rc
	elif i != dernier:
 		t=t+tab+'<td bgcolor=paleturquoise>'
		t=t+'<a href="'+fichier_suiv+'.htm" target="_top">-&gt;</a>'
		t=t+'</td>'+rc
		
	t=t+tab+'</tr>\n</table>'+rc+rc

	# fin
	t=t+'</body>'+rc+'</html>'
	
	# enregistrement du nouveau fichier
	mroutines.savefile(t,fichier+'.htm')



#print "ok."

