#! /usr/bin/env python
# -*- coding: iso-8859-1 -*-
#
# Jean-Pierre Sutto, 22, 24 juin 2005, 13, 16 avril 2006
#
# ajoute des liens aux refs des folios.
# usage moutils2.py radical
# This is GPL free software

import sys
import string
import mroutines
import os
import glob
import re

def format(entier,longueur):
	
	# entrées: deux entiers
	# renvoie une chaine representative du premier entier de longueur le 
	# deuxième entrée en mettant le bon nombre de 0 avant.
	
	chaine='0000000000'+repr(entier)
	chaine=chaine[-longueur:]

	return chaine


print '----------------------  mlinks --------------------'

# récupération de l'argument
radical=sys.argv[1]
print 'radical=',radical

# vérification de l'existence du fichier particulier
fp=radical+'.fp'
#print "fp=*",fp,"*"
if not(os.path.isfile(fp)):
	print 'Il n\'y a pas de fichier',fp
	sys.exit(1)

# récupération des infos du fichier particulier
t=mroutines.readfile(fp)

liste=['temoins']
chainetemoins=mroutines.readfp(t,liste)[0]
#print "chainetemoins=*",chainetemoins,"*"
listetemoins=chainetemoins.split(',')
print "listetemoins=*",listetemoins,"*"

listesiteimages=[]
listetypes=[]
for temoin in listetemoins:
	#print "temoin=*",temoin,"*"

	liste=['siteimage('+temoin+')']
	listesiteimages.append(mroutines.readfp(t,liste)[0])
	liste=['type('+temoin+')']
	listetypes.append(mroutines.readfp(t,liste)[0])

print "listesiteimages=*",listesiteimages,"*"
print "listetypes=*",listetypes,"*"

# recupération de tous les fichiers concernés
liste=glob.glob(radical+'*.htm')
#print "liste=",liste

# on boucle sur la liste des fichiers à traiter
for fichier in liste:

	#print "==============================================="
	#print "fichier=*",fichier,"*"

	change=0
	s=mroutines.readfile(fichier)

	k=0
	for temoin in listetemoins:
	
		target='\<font size="-1" color=green\>\['+temoin+'\:([0-9rv]+)\]\</font\>'
		match=re.search(target,s)
	#	print "match=*",match,"*"
	
		while match!=None:
	
			change=1
			
			complet=string.strip(match.group())
			#print "complet=*",complet,"*"
	
			folio=string.strip(match.group(1))
			#print "folio=*",folio,"*"
			
			type=listetypes[k]
			
			if type=='01v-02r':
				longueur=2
				nb=int(re.sub('[rv]','',folio))
				rv=re.sub('[0-9]+','',folio)
				#print "nb=*",nb,"*"
				#print "rv=*",rv,"*"
	
				if rv=='r':
					ref=format(nb-1,longueur)+'v-'+format(nb,longueur)+'r'
				else:
					ref=format(nb,longueur)+'v-'+format(nb+1,longueur)+'r'
					
			elif type=='1v-2r':
				longueur=1
				nb=int(re.sub('[rv]','',folio))
				rv=re.sub('[0-9]+','',folio)
				#print "nb=*",nb,"*"
				#print "rv=*",rv,"*"
	
				if rv=='r':
					ref=format(nb-1,longueur)+'v-'+format(nb,longueur)+'r'
				else:
					ref=format(nb,longueur)+'v-'+format(nb+1,longueur)+'r'
					
			elif type=='001v-002r':
				longueur=3
				nb=int(re.sub('[rv]','',folio))
				rv=re.sub('[0-9]+','',folio)
				#print "nb=*",nb,"*"
				#print "rv=*",rv,"*"
	
				if rv=='r':
					ref=format(nb-1,longueur)+'v-'+format(nb,longueur)+'r'
				else:
					ref=format(nb,longueur)+'v-'+format(nb+1,longueur)+'r'
					
			elif type=='002-003':
				longueur=3
				nb=int(folio)
				#print "nb=*",nb,"*"
				if nb % 2 == 0:
					ref=format(nb,longueur)+'-'+format(nb+1,longueur)
				else:
					ref=format(nb-1,longueur)+'-'+format(nb,longueur)
				
			elif type=='01-02':
				longueur=2
				nb=int(folio)
				#print "nb=*",nb,"*"
				if nb % 2 == 0:
					ref=format(nb,longueur)+'-'+format(nb+1,longueur)
				else:
					ref=format(nb-1,longueur)+'-'+format(nb,longueur)
				
			elif type=='1r':
				longueur=1
				nb=int(re.sub('[rv]','',folio))
				rv=re.sub('[0-9]+','',folio)
				#print "nb=*",nb,"*"
				#print "rv=*",rv,"*"
	
				if rv=='r':
					ref=format(nb,longueur)+'r'
				else:
					ref=format(nb,longueur)+'v'

			elif type=='01r':
				longueur=2
				nb=int(re.sub('[rv]','',folio))
				rv=re.sub('[0-9]+','',folio)
				#print "nb=*",nb,"*"
				#print "rv=*",rv,"*"
	
				if rv=='r':
					ref=format(nb,longueur)+'r'
				else:
					ref=format(nb,longueur)+'v'
					
			else:
				print 'Je ne connais ce type:',type
				sys.exit(1)
			
			#print "ref=*",ref,"*"
	
			siteimage=listesiteimages[k]
			lien=siteimage+ref+'.html'
			#print "lien=*",lien,"*"
	
			champ='['+temoin+':'+folio+']'
			#print "champ=*",champ,"*"
	
			replace='<font size="-1" color="green"><a href="'+lien+'" target="_top">'+champ+'</a></font>'
			# target="_blank"
			#print "replace=*",replace,"*"
	
			s=string.replace(s,complet,replace)
			match=re.search(target,s)
		
		k=k+1
			
	if change==1:
		mroutines.savefile(s,fichier)
	
print 'Ok.'
