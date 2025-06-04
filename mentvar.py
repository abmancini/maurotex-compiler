#! /usr/bin/env python
# -*- coding: iso-8859-1 -*-
#
# Jean-Pierre Sutto, 25 mars 2003, 14 avril 2006
#
# ajoute les entêtes aux pages des variantes.
# usage mentvar2.py radical
# This is GPL free software

import sys
import string
import mroutines
import os

print '---------------------- mentvar --------------------'

radical=sys.argv[1]
fp=radical+'.fp'

# vérification de l'existence du fichier particulier
if not(os.path.isfile(fp)):
	print 'Il n\'y a pas de fichier',fp
	sys.exit(1)

# récupération de la variable titres_proposition
titres_propositions=mroutines.recup_titres_propositions(radical)

# date interne à récupérer
date=mroutines.date()
	
# remplissage des constantes
rc='\n'
tab='\t'	

titre='<title>variante</title>'
signature='<!-- Jean-Pierre Sutto '+date+' -->'
created='<!-- Created with m2hv, mtex, python, LaTeX and linux. -->'
haut='<a name="haut"></a>'
bodycolor='<body bgcolor=white>'

header='<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">'+rc\
		+'<html>'+rc+rc\
		+'<head>'+rc\
		+tab+'<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">'+rc\
		+tab+titre+rc\
		+tab+signature+rc\
		+tab+created+rc\
		+'</head>'+rc\
		+rc\
		+bodycolor+rc

nom="""<table width="100%" cellpadding=1>
	<tr>
	<td bgcolor=green align=center valign=middle>
	<font color="white">Apparatus</font>
	</td>
	</tr>
</table>
"""

avant=header+rc+nom+rc
apres='</body>\n</html>'

# on boucle sur la liste des propositions
variantes=0
for i in range(len(titres_propositions)):

	fichier=radical+'-'+repr(i+1)+'v.htm'
	
	# on ne traite que si le fichier de variantes existe
	if os.path.isfile(fichier):

		variantes=1
		s=mroutines.readfile(fichier)
			
		# élimination des anciennes entêtes
		s=string.replace(s,'<html>\n<head>\n</head>\n<body>','')
		s=string.replace(s,'<html>\n<head>\n</head>\n<body bgcolor="white">','')
		s=string.replace(s,'</body>\n</html>','')
	
		# quelques améliorations esthétiques provisoires
		#s=string.replace(s,'</p>\n\n<p>','<br>\n\n')
	
		s=avant+s+apres
	
		mroutines.savefile(s,fichier)
	
# on informe si rien n'a été fait
if variantes == 0:
	print 'Rien fait.'
else:
	print 'Ok.'


