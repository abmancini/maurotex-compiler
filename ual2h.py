#! /usr/bin/env python
# -*- coding: iso-8859-1 -*-

#---------------------------------------------------------------------#
#                              Routines
#---------------------------------------------------------------------#
def help():

	print """jpl2h.py convertit un fichier LaTeX2e en html.
jps, 29 novembre 2006.
Utilisation: jpl2h.py [-a] [-b] [-c] [-d] [-f fichiermod] [-h] [-l] [-p] [-r] [-t] fichier[.[tex]]
  -a : équivalent à -b -c -p -r -t
  -b : supprime le lien "haut de page" en Bas de page
  -c : supprime les Commandes non reconnues
  -d : affiche les routines pendant le traitement
  -f fichiermod : lit les modifications de comportement dans fichiermod
  -h : Help: ce message
  -l : Liste des commandes supportées
  -n : convertit uniquement le corps du texte
  -p : supprime la limitation de la largeur de Page
  -r : supprime les Références en pied de page
  -t : supprime l'inclusion de la Table des sections en haut de page"""

	return

#---------------------------------------------------------------------#
def listecommandes():

	print """jpl2h.py, 29 novembre 2006.
Liste des commandes supportées:
$
$$
%
\\#
\\$
\\%
\\&
\\(
\\)
\\,
\\-
\\/
\\<
\\_
\\{
\\}
\\~
^
_
~
'\\ '
\\\\ (paragraphe)
& (tabular)
\\\\ (tabular)
abstract (environnement)
accents iso type latin1 -> html
accents type \\'e -> html
accents type \\'{e} -> html
\\anchor
\\and
\\author
\\backslash
\\bibitem
\\bigskip
\\caption
center (environnement)
\\centerline
\\chapter
\\circ
\\cite
\\copyright
\\date
description (environnement)
\\det
\\dim
\\displaystyle
\\dots
\\emph
\\emptyset
enumerate (environnement)
figure (environnement)
flushright (environnement)
\\footnote
\\footnotemark
\\footnotesize
\\footnotetext
\\frac
\\ge
\\hline
\\href
\\hspace
\\Huge
\\huge
\\iff
\\include
\\includefigure
\\includegraphics
\\infty
\\index
\\input
itemize (environnement)
\\ker
\\label
\\Large
\\large
\\LaTeX
\\LaTeX2e
\\lbrack
\\ldots
\\le
\\leftarrow
\\lim
\\ln
longtable (environnement)
\\mail
\\maj
\\maketitle
\\marginpar
\\mathcal
\\mbox
\\medskip
\\min
\\motsclefs
\\multicolumn
\\multirow
\\ne
\\newline
\\newpage
\\n\\n
\\noindent
\\normalsize
\\notehref
\\notemail
\\onlyhtml
\\onlylatex
\\pagestyle
\\par
\\printindex
\\quad
quotation (environnement)
quote (environnement)
\\rbrack
\\ref
\\Rightarrow
\\rightarrow
\\S
\\section
\\sim
\\simpleanchor
\\simplehref
\\small
\\smallskip
\\sqrt
\\subsection
\\subsubsection
\\sup
symboles mathématiques grecs
table (environnement)
\\tableofcontents
\\TeX
\\textbf
\\textcolor
\\textit
\\textrm
\\textsc
\\textsuperscript
\\texttt
\\thanks
thebibliography  (environnement)
\\thispagestyle
\\times
\\tiny
\\title
\\titlehtml
titlepage (environnement)
\\to
\\toc
\\toctwo
\\underligne
\\url
\\verb
verbatim (environnement)
\\verbatiminput
\\voidhref
\\vspace
\\wedge
\\pastorange
\\pastgreen
\\pastred
"""
	return

#---------------------------------------------------------------------#
def findrightbrace(s,debut):

	# retourne la position de la } correspondante a la { qui se trouve dans s
	# avant debut

	reserve=debut
	import string
	k=1
	while k!=0:
		accoGauche=string.find(s,"{",debut+1)
		accoDroite=string.find(s,"}",debut+1)
		if accoGauche<accoDroite and accoGauche!=-1 and accoDroite!=-1:
			k=k+1
			debut=accoGauche
		elif accoDroite!=-1:
			k=k-1
			debut=accoDroite
		else:
			print "findrightbrace(): aie. Impossible de trouver l'accolade correspondante"
			reserve=string.rfind(s,'{',0,reserve+1)
			print s[reserve:reserve+50]
			sys.exit(1)
			
	return accoDroite
	
#---------------------------------------------------------------------#
def findparent(s,cible1,cible2,debut):

	# retourne la position de la cible2 correspondante a la
	# première cible1 trouvée dans s à partir de debut

	reserve=debut
	k=1
	while k!=0:
	
		pos1=string.find(s,cible1,debut+1)
		pos2=string.find(s,cible2,debut+1)

		#print "pos1,pos2=*",pos1,pos2,"*"

		if pos1 == -1 and pos2 == -1:
			print "findparent(): aie. "+cible1+" et "+cible2+" n'existent pas !"
			reserve=string.rfind(s,cible1,0,reserve+1)
			print s[reserve:reserve+20]
			sys.exit(1)
		elif pos1<pos2 and pos1 != -1 and pos2 != -1:
			k=k+1
			debut=pos1
		elif pos2 != -1 :
			k=k-1
			debut=pos2
		else:
			print 'findparent(): aie. Impossible de trouver le '+cible1+\
			' correspondant au '+cible2
			reserve=string.rfind(s,cible1,0,reserve+1)
			print s[reserve:reserve+20]
			sys.exit(1)

	return pos2
	
#---------------------------------------------------------------------#
def findrightpar(s,debut):

	# retourne la position de la ) correspondante a la ( qui se trouve dans s
	# avant debut

	reserve=debut
	import string
	k=1
	while k!=0:
		parGauche=string.find(s,"(",debut+1)
		parDroite=string.find(s,")",debut+1)
		if parGauche<parDroite and parGauche!=-1 and parDroite!=-1:
			k=k+1
			debut=parGauche
		elif parDroite!=-1:
			k=k-1
			debut=parDroite
		else:
			print "findrightpar(): aie. Impossible de trouver la parenthèse correspondante"
			reserve=string.rfind(s,'(',0,reserve+1)
			print s[reserve:reserve+50]
			sys.exit(1)
			
	return parDroite
	
#---------------------------------------------------------------------#
def enregistre(s,variable):

	# renvoie le contenu de  de l'argument de \variable{}

	cible='\\'+variable+'{'
	debut=string.find(s,cible)
	if debut != -1:
		if s[debut+len(cible)]=='}':
			valeur=''
		else:
			fin=findrightbrace(s,debut+len(cible)-1)
			if fin != -1:
				valeur=s[debut+len(cible):fin]
			else:
				valeur=None
	else:
		valeur=None		

#	print s
#	print "variable,valeur=*",variable,valeur,"*"
#	print "===================================="

	return valeur

#---------------------------------------------------------------------#
def effacecommandeetargument(s,cible):

	# \target{a} -> 
	
	cible='\\'+cible+'{'
	debut=string.find(s,cible)
	
	while debut != -1:
		accogauche=debut+len(cible)-1
		fin=findrightbrace(s,accogauche)

		if accogauche != -1 and fin != -1:
			s=s[:debut]+'{}'+s[fin+1:]
		else:
			print "effacecommandeetargument(): aie. Problème d'accolade pour "\
			+cible
			sys.exit(1)
		debut=string.find(s,cible)

	return s

#---------------------------------------------------------------------#
def effaceseulementcommande(s,cible):

	# \target{a} -> a
	
	cible='\\'+cible+'{'
	debut=string.find(s,cible)
	
	while debut != -1:
		accogauche=debut+len(cible)-1
		fin=findrightbrace(s,accogauche)
		if accogauche != -1 and fin != -1:
			s=s[:debut]+s[accogauche+1:fin]+s[fin+1:]
		else:
			print "effaceseulementcommande(): aie. Problème d'accolade pour "\
			+cible
			sys.exit(1)
		debut=string.find(s,cible)

	return s

#---------------------------------------------------------------------#
def commandeetunargument(s,cible,avant,apres):

	# \target{a} -> avant+{a}+apres
	
	cible='\\'+cible+'{'
	debut=string.find(s,cible)
	
	while debut != -1:
		accogauche=debut+len(cible)-1
		fin=findrightbrace(s,accogauche)
		if accogauche != -1 and fin != -1:
			champ=s[accogauche+1:fin]
			#print "cet1a: champ=*"+champ+"*"
			s=s[:debut]+'{'+avant+champ+apres+'}'+s[fin+1:]
		else:
			print "simple(): aie. Problème d'accolade pour "+cible
			sys.exit(1)
		debut=string.find(s,cible)

	return s

#---------------------------------------------------------------------#
def commandeetdeuxarguments(s,cible,avant,pendant,apres):

	# \target{a}{b} -> avant+a+pendant+b+apres
	
	cible='\\'+cible+'{'
	debut=string.find(s,cible)
	
	while debut != -1:
		accogauche=debut+len(cible)-1
		fin=findrightbrace(s,accogauche)
		accogauche2=string.find(s,'{',fin)
		fin2=findrightbrace(s,accogauche2)
		
		if accogauche != -1 and fin != -1 and accogauche2 != -1 and fin2 != -1 :
			s=s[:debut]+avant+s[accogauche+1:fin]+pendant\
			+s[accogauche2+1:fin2]+apres+s[fin2+1:]
		else:
			print "double(): aie. Problème d'accolade pour "+cible
			sys.exit(1)
		debut=string.find(s,cible)

	return s

#---------------------------------------------------------------------#
def estentre(s,pos,cible1,cible2):

	# renvoie 1 si pos se trouve entre les positions des chaines
	# cible1 et cible2; -1 sinon

	pos1=string.rfind(s,cible1,0,pos)
	if pos1 != -1 :
		pos2=findparent(s,cible1,cible2,pos1)
		if pos2 != -1:
		
			if pos1 != -1 and pos2 != -1 and pos1 < pos and pos < pos2:
				renvoi=1
			else:
				pos1=string.find(s,cible1,pos)
				pos2=string.find(s,cible2,pos)
				
				if pos1 != -1 and pos2 !=-1 and pos1 < pos2:
					renvoi=-1
				elif pos1 != -1 and pos2 != -1 and pos2 < pos1:
					renvoi=1
				elif pos1 == -1 and pos2 != -1:
					renvoi=1
				elif pos1 != -1 and pos2 == -1:
					renvoi=0
				elif pos1 == -1 and pos2 == -1:
					renvoi=-1
				else:
					print 'estentre(): aie. Cas non prévu'
					sys.exit(1)
		else:
			renvoi=-1
	else:
		renvoi=-1

	return renvoi

#---------------------------------------------------------------#
def time():

	# renvoie la date dans un format jj-mm-aa
	
	import time
	return time.strftime("%d-%m-%y",time.localtime(time.time()))

#---------------------------------------------------------------------#
def indexmin(s,liste,debut,fin):

	# renvoie une liste contenant la commande de liste et sa position
	# trouvée en premier dans la chaine s entre debut et fin
	# si on ne trouve rien, renvoie None.
	
	index=[]
	lim=len(s)
	ok=0
	for chaine in liste:
		pos=string.find(s,chaine,debut,fin)
		if pos==-1:
			pos=lim
		else:
			ok=1
		index.append(pos)
	m=min(index)
	if ok==1:
		for i in range(len(liste)):
			if index[i]==m:
				result=[liste[i],index[i]]
	else:
		result=None

	return result

#---------------------------------------------------------------------#
def myfind(s,pattern):

	# cherche une expression régulière pattern dans s et renvoie les positions
	# de départ et de fin de la première chaîne trouvée. Renvoie -1,-1 sinon.
	
	match=re.search(pattern,s)
	if match != None:
		debut=match.start()
		fin=match.end()
	else:
		debut=-1
		fin=-1
	
	return debut,fin

#---------------------------------------------------------------------#
def accentsiso(s):

	# accents: latin1 -> html
	s=string.replace(s,"à","&agrave;")
	s=string.replace(s,"â","&acirc;")
	s=string.replace(s,"À","&Agrave;")
	s=string.replace(s,"é","&eacute;")
	s=string.replace(s,"è","&egrave;")
	s=string.replace(s,"ê","&ecirc;")
	s=string.replace(s,"ë","&euml;")
	s=string.replace(s,"É","&Eacute;")
	s=string.replace(s,"È","&Egrave;")
	s=string.replace(s,"Ë","&Euml;")
	s=string.replace(s,"ï","&iuml;")
	s=string.replace(s,"î","&icirc;")
	s=string.replace(s,"ô","&ocirc;")
	s=string.replace(s,"ù","&ugrave;")
	s=string.replace(s,"û","&ucirc;")
	s=string.replace(s,"ç","&ccedil;")
	s=string.replace(s,"«","&laquo;")
	s=string.replace(s,"»","&raquo;")
#	s=string.replace(s,"<<","&laquo;")
#	s=string.replace(s,">>","&raquo;")
	
	return s
	
#---------------------------------------------------------------------#
def accents(s):
		
	s=accentsiso(s)
	
	# accents: {\'e} -> html
	s=string.replace(s,"{\\`a}","@@@etcommercial@@@agrave;")
	s=string.replace(s,"{\\^a}","@@@etcommercial@@@acirc;")
	s=string.replace(s,"{@@@chapeau@@@a}","@@@etcommercial@@@acirc;")
	s=string.replace(s,"{\\'a}","@@@etcommercial@@@aacute;")
	s=string.replace(s,"{\\`A}","@@@etcommercial@@@Agrave;")
	s=string.replace(s,"{\\'e}","@@@etcommercial@@@eacute;")
	s=string.replace(s,"{\\`e}","@@@etcommercial@@@egrave;")
	s=string.replace(s,"{\\^e}","@@@etcommercial@@@ecirc;")
	s=string.replace(s,"{@@@chapeau@@@e}","@@@etcommercial@@@ecirc;")
	s=string.replace(s,"{\\'E}","@@@etcommercial@@@Eacute;")
	s=string.replace(s,"{\\`E}","@@@etcommercial@@@Egrave;")
	s=string.replace(s,"{\\^i}","@@@etcommercial@@@icirc;")
	s=string.replace(s,"{@@@chapeau@@@i}","@@@etcommercial@@@icirc;")
	s=string.replace(s,"{\\`i}","@@@etcommercial@@@igrave;")
	s=string.replace(s,"{\\`\\i}","@@@etcommercial@@@igrave;")
	s=string.replace(s,"{\\'i}","@@@etcommercial@@@iacute;")
	s=string.replace(s,"{\\`i}","@@@etcommercial@@@igrave;")
	s=string.replace(s,"{\\'\\i}","@@@etcommercial@@@iacute;")
	s=string.replace(s,"{\\`I}","@@@etcommercial@@@Igrave;")
	s=string.replace(s,"{\\^o}","@@@etcommercial@@@ocirc;")
	s=string.replace(s,"{@@@chapeau@@@o}","@@@etcommercial@@@ocirc;")
	s=string.replace(s,"{\\`o}","@@@etcommercial@@@ograve;")
	s=string.replace(s,"{\\'o}","@@@etcommercial@@@oacute;")
	s=string.replace(s,"{\\^u}","@@@etcommercial@@@ucirc;")
	s=string.replace(s,"{@@@chapeau@@@u}","@@@etcommercial@@@ucirc;")
	s=string.replace(s,"{\\`u}","@@@etcommercial@@@ugrave;")
	s=string.replace(s,"{\\'u}","@@@etcommercial@@@uacute;")
	s=string.replace(s,'{\\"U}',"@@@etcommercial@@@Uuml;")
	
	s=string.replace(s,"{\\~n}","@@@etcommercial@@@ntilde;")
	
	# accents: \'e -> html
	s=string.replace(s,"\\'e","@@@etcommercial@@@eacute;")
	s=string.replace(s,"\\'E","@@@etcommercial@@@Eacute;")
	s=string.replace(s,"\\`E","@@@etcommercial@@@Egrave;")
	s=string.replace(s,"\\`e","@@@etcommercial@@@egrave;")
	s=string.replace(s,"\\`a","@@@etcommercial@@@agrave;")
	s=string.replace(s,"\\^e","@@@etcommercial@@@ecirc;")
	s=string.replace(s,"\\^o","@@@etcommercial@@@ocirc;")
	s=string.replace(s,"\\^u","@@@etcommercial@@@ucirc;")
	s=string.replace(s,"\\^a","@@@etcommercial@@@acirc;")
	s=string.replace(s,"\\^i","@@@etcommercial@@@icirc;")
	s=string.replace(s,"\\^\\i","@@@etcommercial@@@icirc;")
	s=string.replace(s,"@@@chapeau@@@e","@@@etcommercial@@@ecirc;")
	s=string.replace(s,"@@@chapeau@@@o","@@@etcommercial@@@ocirc;")
	s=string.replace(s,"@@@chapeau@@@u","@@@etcommercial@@@ucirc;")
	s=string.replace(s,"@@@chapeau@@@a","@@@etcommercial@@@acirc;")
	s=string.replace(s,"@@@chapeau@@@i","@@@etcommercial@@@icirc;")
	s=string.replace(s,"@@@chapeau@@@\\i","@@@etcommercial@@@icirc;")
	s=string.replace(s,"\\`A","@@@etcommercial@@@Agrave;")
	s=string.replace(s,"\\`u","@@@etcommercial@@@ugrave;")
	s=string.replace(s,"\\`\\i","@@@etcommercial@@@igrave;")
	s=string.replace(s,"\\'\\i","@@@etcommercial@@@iacute;")
	s=string.replace(s,"\\'i","@@@etcommercial@@@iacute;")
	s=string.replace(s,"\\`i","@@@etcommercial@@@igrave;")
	s=string.replace(s,"\\`o","@@@etcommercial@@@ograve;")
	s=string.replace(s,"\\'u","@@@etcommercial@@@uacute;")
	s=string.replace(s,'\\"u',"@@@etcommercial@@@uuml;")
	s=string.replace(s,"\\'o","@@@etcommercial@@@oacute;")
	s=string.replace(s,"\\'a","@@@etcommercial@@@aacute;")
	s=string.replace(s,"\\`I","@@@etcommercial@@@Igrave;")
	
	s=string.replace(s,"\\~n","@@@etcommercial@@@ntilde;")
	
	# accents: \'{e} -> html
	s=string.replace(s,"\\'{e}","@@@etcommercial@@@eacute;")
	s=string.replace(s,"\\'{E}","@@@etcommercial@@@Eacute;")
	s=string.replace(s,"\\`{E}","@@@etcommercial@@@Egrave;")
	s=string.replace(s,"\\`{e}","@@@etcommercial@@@egrave;")
	s=string.replace(s,"\\`{a}","@@@etcommercial@@@agrave;")
	s=string.replace(s,"\\^{e}","@@@etcommercial@@@ecirc;")
	s=string.replace(s,"\\^{o}","@@@etcommercial@@@ocirc;")
	s=string.replace(s,"\\^{u}","@@@etcommercial@@@ucirc;")
	s=string.replace(s,"\\^{a}","@@@etcommercial@@@acirc;")
	s=string.replace(s,"\\^{i}","@@@etcommercial@@@icirc;")
	s=string.replace(s,"@@@chapeau@@@{e}","@@@etcommercial@@@ecirc;")
	s=string.replace(s,"@@@chapeau@@@{o}","@@@etcommercial@@@ocirc;")
	s=string.replace(s,"@@@chapeau@@@{u}","@@@etcommercial@@@ucirc;")
	s=string.replace(s,"@@@chapeau@@@{a}","@@@etcommercial@@@acirc;")
	s=string.replace(s,"@@@chapeau@@@{i}","@@@etcommercial@@@icirc;")
	s=string.replace(s,"\\`{A}","@@@etcommercial@@@Agrave;")
	s=string.replace(s,"\\`{u}","@@@etcommercial@@@ugrave;")
	
	s=string.replace(s,"\\'{i}","@@@etcommercial@@@iacute;")
	s=string.replace(s,"\\'{\\i}","@@@etcommercial@@@iacute;")
	s=string.replace(s,"\\`{i}","@@@etcommercial@@@igrave;")
	s=string.replace(s,"\\`{\\i}","@@@etcommercial@@@igrave;")
	s=string.replace(s,"\\`{o}","@@@etcommercial@@@ograve;")
	s=string.replace(s,"\\'{u}","@@@etcommercial@@@uacute;")
	s=string.replace(s,"\\'{o}","@@@etcommercial@@@oacute;")
	s=string.replace(s,"\\'{a}","@@@etcommercial@@@aacute;")
	s=string.replace(s,"\\`{I}","@@@etcommercial@@@Igrave;")
	
	s=string.replace(s,"\\~{n}","@@@etcommercial@@@ntilde;")
	
	# accents: autres -> html
	s=string.replace(s,"{\c c}","@@@etcommercial@@@ccedil;")
	s=re.sub('\{*\\\\"\{*a\}*','@@@etcommercial@@@auml;',s)
	s=re.sub('\{*\\\\"\{*\\\\*i\}*','@@@etcommercial@@@iuml;',s)
	s=re.sub('\\\\"e','@@@etcommercial@@@euml;',s)
	s=re.sub("\{*\\\\\{*c{c}\}*","@@@etcommercial@@@ccedil;",s)

#	s=re.sub('\{*\\\\oe\}*','oe',s)
#	s=re.sub('\{*\\\\OE\}*','OE',s)
	s=string.replace(s,"\\oe","oe")
	s=string.replace(s,"\\OE","OE")
#	s=re.sub('\{*\\\\ae\}*','ae',s)
#	s=re.sub('\{*\\\\AE\}*','AE',s)
	s=string.replace(s,"{\\ae}","ae")
	s=string.replace(s,"{\\AE}","AE")
	
	s=string.replace(s,"\\acute{o}","@@@etcommercial@@@oacute;")
	s=string.replace(s,"{\\ss}","@@@etcommercial@@@szlig;")
	
	return s

#---------------------------------------------------------------------#
def greek(s):

	s=string.replace(s,'$\\tilde$$\\omega$','{<i><font face="symbol">v</font></i>}')
	s=string.replace(s,'\\tilde\\omega','{<font face="symbol">v</font>}')
	
	s=re.sub('\\\\alpha([^a-zA-Z])','{<font face="symbol">a</font>}\\1',s)
	s=re.sub('\\\\Alpha([^a-zA-Z])','{<font face="symbol">A</font>}\\1',s)
	s=re.sub('\\\\beta([^a-zA-Z])','{<font face="symbol">b</font>}\\1',s)
	s=re.sub('\\\\Beta([^a-zA-Z])','{<font face="symbol">B</font>}\\1',s)
	s=re.sub('\\\\chi([^a-zA-Z])','{<font face="symbol">c</font>}\\1',s)
	s=re.sub('\\\\delta([^a-zA-Z])','{<font face="symbol">d</font>}\\1',s)
	s=re.sub('\\\\Delta([^a-zA-Z])','{<font face="symbol">D</font>}\\1',s)
	s=re.sub('\\\\varepsilon([^a-zA-Z])','{<font face="symbol">e</font>}\\1',s)
	s=re.sub('\\\\gamma([^a-zA-Z])','{<font face="symbol">g</font>}\\1',s)
	s=re.sub('\\\\Gamma([^a-zA-Z])','{<font face="symbol">G</font>}\\1',s)
	s=re.sub('\\\\lambda([^a-zA-Z])','{<font face="symbol">l</font>}\\1',s)
	s=re.sub('\\\\Lambda([^a-zA-Z])','{<font face="symbol">L</font>}\\1',s)
	s=re.sub('\\\\mu([^a-zA-Z])','{<font face="symbol">m</font>}\\1',s)
	s=re.sub('\\\\Mu([^a-zA-Z])','{<font face="symbol">M</font>}\\1',s)
	s=re.sub('\\\\nu([^a-zA-Z])','{<font face="symbol">n</font>}\\1',s)
	s=re.sub('\\\\Nu([^a-zA-Z])','{<font face="symbol">N</font>}\\1',s)
	s=re.sub('\\\\pi([^a-zA-Z])','<font face="symbol">p</font>}\\1',s)
	s=re.sub('\\\\Pi([^a-zA-Z])','<font face="symbol">P</font>}\\1',s)
	s=re.sub('\\\\phi([^a-zA-Z])','{<font face="symbol">f</font>}\\1',s)
	s=re.sub('\\\\varphi([^a-zA-Z])','{<font face="symbol">f</font>}\\1',s)
	s=re.sub('\\\\psi([^a-zA-Z])','{<font face="symbol">y</font>}\\1',s)
	s=re.sub('\\\\Psi([^a-zA-Z])','{<font face="symbol">Y</font>}\\1',s)
	s=re.sub('\\\\Phi([^a-zA-Z])','{<font face="symbol">F</font>}\\1',s)
	s=re.sub('\\\\rho([^a-zA-Z])','{<font face="symbol">r</font>}\\1',s)
	s=re.sub('\\\\sigma([^a-zA-Z])','{<font face="symbol">s</font>}\\1',s)
	s=re.sub('\\\\varsigma([^a-zA-Z])','{<font face="symbol">s</font>}\\1',s)
	s=re.sub('\\\\omega([^a-zA-Z])','{<font face="symbol">w</font>}\\1',s)
	s=re.sub('\\\\Omega([^a-zA-Z])','{<font face="symbol">W</font>}\\1',s)
	s=re.sub('\\\\theta([^a-zA-Z])','{<font face="symbol">q</font>}\\1',s)
	s=re.sub('\\\\Theta([^a-zA-Z])','{<font face="symbol">Q</font>}\\1',s)

	s=re.sub('\\\\epsilon([^a-zA-Z])','{<font face="symbol">e</font>}\\1',s)
	s=re.sub('\\\\eta([^a-zA-Z])','{<font face="symbol">h</font>}\\1',s)
	s=re.sub('\\\\iota([^a-zA-Z])','{<font face="symbol">i</font>}\\1',s)
	s=re.sub('\\\\kappa([^a-zA-Z])','{<font face="symbol">k</font>}\\1',s)
	s=re.sub('\\\\xi([^a-zA-Z])','{<font face="symbol">x</font>}\\1',s)
	s=re.sub('\\\\tau([^a-zA-Z])','{<font face="symbol">t</font>}\\1',s)
	s=re.sub('\\\\upsilon([^a-zA-Z])','{<font face="symbol">u</font>}\\1',s)
	s=re.sub('\\\\zeta([^a-zA-Z])','{<font face="symbol">z</font>}\\1',s)

	return s

#---------------------------------------------------------------------#
def symbol(s):

#	s=string.replace(s,'$\\tilde$$\\omega$','<i><font color="purple">@@@tilde@@@o</font></i>')
#	s=string.replace(s,'\\tilde\\omega','<font color="purple">@@@tilde@@@o</font>')
#	s=re.sub('\\\\alpha([^a-zA-Z])','<font color="purple">a</font>\\1',s)
#	s=re.sub('\\\\Alpha([^a-zA-Z])','<font color="purple">A</font>\\1',s)
#	s=re.sub('\\\\beta([^a-zA-Z])','<font color="purple">b</font>\\1',s)
#	s=re.sub('\\\\Beta([^a-zA-Z])','<font color="purple">B</font>\\1',s)
#	s=re.sub('\\\\delta([^a-zA-Z])','<font color="purple">d</font>\\1',s)
#	s=re.sub('\\\\Delta([^a-zA-Z])','<font color="purple">D</font>\\1',s)
#	s=re.sub('\\\\gamma([^a-zA-Z])','<font color="purple">g</font>\\1',s)
#	s=re.sub('\\\\Gamma([^a-zA-Z])','<font color="purple">G</font>\\1',s)
#	s=re.sub('\\\\lambda([^a-zA-Z])','<font color="purple">l</font>\\1',s)
#	s=re.sub('\\\\Lambda([^a-zA-Z])','<font color="purple">Lda</font>\\1',s)
#	s=re.sub('\\\\mu([^a-zA-Z])','<font color="purple">mu</font>\\1',s)
#	s=re.sub('\\\\Mu([^a-zA-Z])','<font color="purple">Mu</font>\\1',s)
#	s=re.sub('\\\\nu([^a-zA-Z])','<font color="purple">nu</font>\\1',s)
#	s=re.sub('\\\\Nu([^a-zA-Z])','<font color="purple">Nu</font>\\1',s)
#	s=re.sub('\\\\pi([^a-zA-Z])','<font color="purple">pi</font>\\1',s)
#	s=re.sub('\\\\Pi([^a-zA-Z])','<font color="purple">Pi</font>\\1',s)
#	s=re.sub('\\\\phi([^a-zA-Z])','<font color="purple">phi</font>\\1',s)
#	s=re.sub('\\\\Phi([^a-zA-Z])','<font color="purple">Phi</font>\\1',s)
#	s=re.sub('\\\\varsigma([^a-zA-Z])','<font color="purple">varsigma</font>\\1',s)
#	s=re.sub('\\\\omega([^a-zA-Z])','<font color="purple">o</font>\\1',s)
#	s=re.sub('\\\\Omega([^a-zA-Z])','<font color="purple">O</font>\\1',s)
#	s=re.sub('\\\\theta([^a-zA-Z])','<font color="purple">theta</font>\\1',s)
#	s=re.sub('\\\\Theta([^a-zA-Z])','<font color="purple">Theta</font>\\1',s)
#	s=re.sub('\\\\varepsilon([^a-zA-Z])','<font color="purple">e</font>\\1',s)
#	s=re.sub('\\\\varphi([^a-zA-Z])','<font color="purple">phi</font>\\1',s)
#	s=re.sub('\\\\rho([^a-zA-Z])','<font color="purple">rho</font>\\1',s)
#	s=re.sub('\\\\Psi([^a-zA-Z])','<font color="purple">Psi</font>\\1',s)

	s=re.sub('\\\\ne([^a-zA-Z])','{<font face="symbol">¹</font>}\\1',s)
#	s=string.replace(s,'$\\pm$','<sup>+</sup>/<sub>-</sub>')
	s=re.sub('\\\\pm([^a-zA-Z])','{±}\\1',s)
	s=re.sub('\\\\infty([^a-zA-Z])','{<big>@@@etcommercial@@@infin;</big>}\\1',s)
	s=re.sub('\\\\Rightarrow([^a-zA-Z])','{<font color="purple">=@@@etcommercial@@@gt;</font>}\\1',s)
	s=re.sub('\\\\rightarrow([^a-zA-Z])','{<font face="symbol">®</font>}\\1',s)
	s=re.sub('\\\\leftarrow([^a-zA-Z])','{<font face="symbol">¬</font>}\\1',s)
	s=re.sub('\\\\ge([^a-zA-Z])','{@@@etcommercial@@@ge;}\\1',s)
	s=re.sub('\\\\le([^a-zA-Z])','{@@@etcommercial@@@le;}\\1',s)
	s=re.sub('\\\\to([^a-zA-Z])','{<font face="symbol">®</font>}\\1',s)
	s=re.sub('\\\\circ([^a-zA-Z])','{<font color="purple">o</font>}\\1',s)
	s=re.sub('\\\\iff([^a-zA-Z])','{<font color="purple">@@@etcommercial@@@lt;=@@@etcommercial@@@gt;</font>}\\1',s)
	s=re.sub('\\\\sim([^a-zA-Z])','{@@@sim@@@}\\1',s)
	s=string.replace(s,'$\\times$','@@@etcommercial@@@times;')
	s=re.sub('\\\\times([^a-zA-Z])','{@@@etcommercial@@@times;}\\1',s)
	s=re.sub('\\\\wedge([^a-zA-Z])','{@@@chapeau@@@}\\1',s)
	s=re.sub('\\\\emptyset([^a-zA-Z])','{Ø}\\1',s)
	s=re.sub('\\\\lim([^a-zA-Z])','{ lim }\\1',s)
	s=re.sub('\\\\ln([^a-zA-Z])','{ ln }\\1',s)
	s=re.sub('\\\\sup([^a-zA-Z])','{ sup }\\1',s)
	s=re.sub('\\\\min([^a-zA-Z])','{ min }\\1',s)
	s=re.sub('\\\\dim([^a-zA-Z])','{ dim }\\1',s)
	s=re.sub('\\\\det([^a-zA-Z])','{ det }\\1',s)
	s=re.sub('\\\\ker([^a-zA-Z])','{ ker }\\1',s)
	s=re.sub('\\\\cos([^a-zA-Z])','{ cos }\\1',s)
	s=re.sub('\\\\sin([^a-zA-Z])','{ sin }\\1',s)
	s=re.sub('\\\\tan([^a-zA-Z])','{ tan }\\1',s)
	s=re.sub('\\\\arctan([^a-zA-Z])','{ arctan }\\1',s)

	s=re.sub('\\\\exists([^a-zA-Z])','{<font face="symbol">@@@dollar@@@</font>}\\1',s)
	s=re.sub('\\\\in([^a-zA-Z])','{<font face="symbol">Î</font>}\\1',s)
	s=re.sub('\\\\notin([^a-zA-Z])','{<font face="symbol">Ï</font>}\\1',s)
	s=re.sub('\\\\subset([^a-zA-Z])','{<font face="symbol">Ì</font>}\\1',s)
	s=re.sub('\\\\forall([^a-zA-Z])','{<font face="symbol">@@@etcommercial@@@quot;</font>}\\1',s)
	s=re.sub('\\\\parallel([^a-zA-Z])','{<font color="purple"> || </font>}\\1',s)
	s=re.sub('\\\\perp([^a-zA-Z])','{<font face="symbol">@@@chapeau@@@</font>}\\1',s)

	s=re.sub('\\\\langle([^a-zA-Z])','{@@@etcommercial@@@lt;}\\1',s)
	s=re.sub('\\\\rangle([^a-zA-Z])','{@@@etcommercial@@@gt;}\\1',s)

	s=re.sub('\\\\int([^a-zA-Z])','{<big><big>@@@etcommercial@@@int;</big></big>}\\1',s)
	s=re.sub('\\\\sum([^a-zA-Z])','{<big>@@@etcommercial@@@sum;</big>}\\1',s)
	s=re.sub('\\\\cap([^a-zA-Z])','{{@@@etcommercial@@@cap;}\\1',s)
	s=re.sub('\\\\cup([^a-zA-Z])','{{<font face="symbol">È</font>}\\1',s)
	# NdU: forse questo non e' il posto migliore per gestire i \bullet

	s=string.replace(s,'\\greenbullet','<img src="micons/pastilles/pastgreen.gif" alt="green">')
        s=string.replace(s,'\\redbullet','<img src="micons/pastilles/pastred.gif" alt="red">')
        s=string.replace(s,'\\orangebullet','<img src="micons/pastilles/pastorange.gif" alt="orange">')

	s=re.sub('\\\prod([^a-zA-Z])','{<big><font face="symbol">P</font></big>}\\1',s)

	return s

#---------------------------------------------------------------------#
def indice(s):

	cible='_'
	debut=string.find(s,cible)
	while debut != -1:
		if s[debut+1]=='{':
			pos2=findrightbrace(s,debut+1)
			#print 'complexe'
			champ='{'+s[debut+2:pos2]+'}'
			s=s[:debut]+'<sub>'+champ+'</sub>'+s[pos2+1:]
		else:
			#print 'simple'
			champ=s[debut+1]
			s=s[:debut]+'<sub>'+champ+'</sub>'+s[debut+len(cible)+1:]
		#print 'champ=*'+champ+'*'
	
		debut=string.find(s,cible)

	return s

#---------------------------------------------------------------------#
def exposant(s):

	cible='^'
	debut=string.find(s,cible)
	while debut != -1:
		if s[debut+1]=='{':
			pos2=findrightbrace(s,debut+1)
			#print 'complexe'
			champ='{'+s[debut+2:pos2]+'}'
			s=s[:debut]+'<sup>'+champ+'</sup>'+s[pos2+1:]
		else:
			#print 'simple'
			champ=s[debut+1]
			s=s[:debut]+'<sup>'+champ+'</sup>'+s[debut+len(cible)+1:]
		#print 'champ=*'+champ+'*'
	
		debut=string.find(s,cible)

	return s

#---------------------------------------------------------------------#
def textsc(s):
	cible='\\textsc{'
	debut=string.find(s,cible)
	while debut != -1:
		fin=findrightbrace(s,debut+len(cible))
		champ=s[debut+len(cible):fin]
		
		# hack au cas où il y a une commande dans le champ
		# qu'on ne veut pas passer en majuscule !
		if string.find(champ,'\\') == -1:
			newchamp=string.upper(champ)
			newchamp=string.replace(newchamp,'ACUTE;','acute;')
			newchamp=string.replace(newchamp,'GRAVE;','grave;')
			newchamp=string.replace(newchamp,'CIRC;','circ;')
		else:
			newchamp=champ
		
		s=s[:debut]+'<font color="teal">'+newchamp+'</font>'+s[fin+1:]
		debut=string.find(s,cible)

	return s

#---------------------------------------------------------------------#
def enrichissements(s):
		
	s=re.sub('\{\\\\bf( |\n|@@@commentaire@@@)','\\\\textbf{',s)
	s=commandeetunargument(s,'textbf','<b>','</b>')
	s=re.sub('\{\\\\it( |\n|@@@commentaire@@@)','\\\\textit{',s)
	s=commandeetunargument(s,'textit','<i>','</i>')
	s=re.sub('\{\\\\em( |\n|@@@commentaire@@@)','\\\\emph{',s)
	s=commandeetunargument(s,'emph','<i>','</i>')
	s=commandeetunargument(s,'texttt','<tt>','</tt>')
#	s=commandeetunargument(s,'textsc','<font color="teal">','</font>')
	s=textsc(s)
	s=commandeetunargument(s,'textsl','<i><font color="purple">','</font></i>')
	s=commandeetunargument(s,'textsuperscript','<sup>','</sup>')
	#s=re.sub('\^\{(.+?)}','<sup><i>\\1</i></sup>',s)
	#s=re.sub('\^(.)','<sup><i>\\1</i></sup>',s)
	#s=re.sub('\_\{(.+?)}','<sub><i>\\1</i></sub>',s)
	#s=re.sub('\\_(.)','<sub>\\1</sub>',s)
	s=commandeetunargument(s,'underline','<u>','</u>')
#	s=commandeetunargument(s,'textrm','</i>','<i>')
	s=effaceseulementcommande(s,'textrm')
	
	return s

#---------------------------------------------------------------------#
def tabular(tabularlatex,compteur):

	tabularlatex=string.replace(tabularlatex,'&',' & ')
	tabularlatex=string.replace(tabularlatex,'@@@doublebarre@@@',' @@@doublebarre@@@ ')
	tabularlatex=string.replace(tabularlatex,'@@@commentaire@@@',' ')
	#print "tabularlatex=*",tabularlatex,"*"

	cible='\\begin{tabular}'
	cible2='\\end{tabular}'
	debut=string.find(tabularlatex,cible)
	debutalignement=string.find(tabularlatex,'{',len(cible))
	finalignement=findrightbrace(tabularlatex,debutalignement)
	alignement=tabularlatex[debutalignement+1:finalignement]
	
	nbbarres=string.count(alignement,'|')
	alignement=string.replace(alignement,'|','')
	
	alignement=re.sub(' +','',alignement)
	alignement=re.sub('\\hspace\{.+?\}','',alignement)
	alignement=re.sub('p\{.+?\}','j',alignement)
	
	if string.find(alignement,'@') != -1 :
		cellpadding=10
		alignement=re.sub('@\{\{.*?\}\}','',alignement)
		#pour tenir de l'effacement de \hspace --> {}
		alignement=re.sub('@\{.*?\}','',alignement)
	else:
		cellpadding=0
	
	debutetoile=string.find(alignement,'*')
	while debutetoile!=-1:
		debutetoile1=string.find(alignement,'{',debutetoile)
		finetoile1=findrightbrace(alignement,debutetoile1)
		nbetoile=int(string.strip(alignement[debutetoile1+1:finetoile1]))
		debutetoile2=string.find(alignement,'{',finetoile1)
		finetoile2=findrightbrace(alignement,debutetoile2)
		alignetoile=string.strip(alignement[debutetoile2+1:finetoile2])
		alignement=alignement[:debutetoile]+alignetoile*nbetoile\
		+alignement[finetoile2+1:]
		
		debutetoile=string.find(alignement,'*')
	
	nbtotalcolonnes=len(alignement)
	#print "alignement=*",alignement,"*"
	#print "nbtotalcolonnes=*",nbtotalcolonnes,"*"

	alignementcolonne=[]
	for i in range(len(alignement)):
		alignementcolonne.append(alignement[i])
		
	if nbbarres!=0:
		tabularlatex=re.sub('@@@doublebarre@@@ *\n*\\\\hline( *\n*\\\\end\{tabular\})','\\1',tabularlatex)
		tabularlatex=string.replace(tabularlatex,'\\hline','')

	corpstabular=tabularlatex[finalignement+1:string.find(tabularlatex,cible2)]
			
	listecommandes=['&','@@@doublebarre@@@']
	listecellule=[]
	oldindex=0
	oldcommande='&'
	i=0
	j=0
	result=indexmin(corpstabular,listecommandes,oldindex,len(corpstabular))
	while result != None:
		commande=result[0]

		if commande=='&':
			numerocolonne=i
			numeroligne=j
			i=i+1
		elif commande=='@@@doublebarre@@@':
			numerocolonne=i
			numeroligne=j
			i=0
			j=j+1
		else:
			print 'tabular(): aie. Commande de tableau inconnue: '+commande
			sys.exit(1)
		index=result[1]
		corpslistecellule=string.strip(corpstabular[oldindex+len(oldcommande)-1:index])
		
		#print
		#print "commande=*",commande,"*"
		#print "numerocolonne=*",numerocolonne,"*"
		#print "numeroligne=*",numeroligne,"*"
		#print "corpslistecellule=*",corpslistecellule,"*"
		
		listecellule.append([numeroligne,numerocolonne,\
		alignementcolonne[numerocolonne],corpslistecellule])
		
		oldindex=index+1
		oldcommande=commande
		result=indexmin(corpstabular,listecommandes,\
		oldindex+len(commande),len(corpstabular))

	corpslistecellule=string.strip(corpstabular[oldindex+len(oldcommande)-1:])

	numerocolonne=i
	numeroligne=j
	#print
	#print "commande=*",commande,"*"
	#print "numerocolonne=*",numerocolonne,"*"
	#print "numeroligne=*",numeroligne,"*"
	#print "corpslistecellule=*",corpslistecellule,"*"

	listecellule.append([numeroligne,numerocolonne,\
	alignementcolonne[numerocolonne],corpslistecellule])

	nlistecellule=[]
	nbhline=0
	cible='\\hline'
	for cellule in listecellule:
		i=cellule[0]
		j=cellule[1]
		alignementcellule=cellule[2]
		corpscellule=cellule[3]
		hline=string.find(corpscellule,cible)
		if hline != -1:
			nlistecellule.append([i+nbhline,0,'',nbtotalcolonnes,cible])
			nbhline=nbhline+1
			corpscellule=string.strip(string.replace(corpscellule,cible,''))
			if cellule!=listecellule[len(listecellule)-1]:
				nlistecellule.append([i+nbhline,j,alignementcellule,1,corpscellule])
		else:
			nlistecellule.append([i+nbhline,j,alignementcellule,1,corpscellule])

	nnlistecellule=[]
	cible='\\multicolumn{'
	for cellule in nlistecellule:
		i=cellule[0]
		j=cellule[1]
		alignementcellule=cellule[2]
		nbcolonnes=cellule[3]
		corpscellule=cellule[4]
		debut=string.find(corpscellule,cible)
		if debut != -1:
			finnbcolonnes=findrightbrace(corpscellule,debut+len(cible))
			nbcolonnes=int(corpscellule[debut+len(cible):finnbcolonnes])

			debutalign=string.find(corpscellule,'{',finnbcolonnes)
			finalign=findrightbrace(corpscellule,debutalign)
			alignementcellule=string.strip(corpscellule[debutalign+1:finalign])

			debutcorpsmulticolumn=string.find(corpscellule,'{',finalign)
			fincorpsmulticolumn=findrightbrace(corpscellule,debutcorpsmulticolumn)
			corpscellule=string.strip(corpscellule[debutcorpsmulticolumn+1:fincorpsmulticolumn])

		nnlistecellule.append([i,j,alignementcellule,nbcolonnes,corpscellule])

	#print nnlistecellule
	
	nnnlistecellule=[]
	aeffacer=[]
	cible='\\multirow{'
	for cellule in nnlistecellule:
		i=cellule[0]
		j=cellule[1]
		alignementcellule=cellule[2]
		nbcolonnes=cellule[3]
		corpscellule=cellule[4]
		nblignes=1

		debut=string.find(corpscellule,cible)
		if debut != -1:
			finnblignes=findrightbrace(corpscellule,debut+len(cible))
			nblignes=int(corpscellule[debut+len(cible):finnblignes])

			debutalign=string.find(corpscellule,'{',finnblignes)
			finalign=findrightbrace(corpscellule,debutalign)
			etoile=string.strip(corpscellule[debutalign+1:finalign])

			debutcorpsmultirow=string.find(corpscellule,'{',finalign)
			fincorpsmultirow=findrightbrace(corpscellule,debutcorpsmultirow)
			corpscellule=string.strip(corpscellule\
			[debutcorpsmultirow+1:fincorpsmultirow])

			for k in range(nblignes-1):
				aeffacer.append([i+k+1,j])
		
		nnnlistecellule.append([i,j,alignementcellule,nbcolonnes,nblignes,corpscellule])
		
	# Le texte à côté d'un multirow est placé en haut de sa cellule, à la
	# différence de latex qui le centre. Je préfère comme cela.
	t='<table summary="tableau '+repr(compteur)+'"'
	if nbbarres != 0:
		t=t+' border="1"'
	if cellpadding == 10 and nbhline==0:
		t=t+' cellspacing="0" cellpadding="10"'
	elif cellpadding == 10 and nbhline!=0:
		t=t+' cellspacing="0" cellpadding="3"'
	elif nbbarres == 0 and nbhline != 0:
		t=t+' cellspacing="0" cellpadding="2"'
	else:
		t=t+' cellspacing="0" cellpadding="3"'
	t=t+'>\n<tr>\n'
	oldj=0
	oldi=0
	for cellule in nnnlistecellule:
		i=cellule[0]
		j=cellule[1]
		alignementcellule=cellule[2]
		nbcolonnescellule=cellule[3]
		nblignescellule=cellule[4]
		corpscellule=cellule[5]

		#print "corpscellule=*",corpscellule,"*"
		#print "i,oldi=*",i,oldi,"*"
		#print "j,oldj=*",j,oldj,"*"

		efface=0
		for blank in aeffacer:
			iblank=blank[0]
			jblank=blank[1]
			if iblank==i and jblank==j:
				efface=1
				#print 'blank=',blank

		if efface == 0:
			if string.strip(dehtml(corpscellule))=='' \
			and string.count(corpscellule,'<img')==0 \
			and string.count(corpscellule,'<a name')==0 :
				corpscellule="@@@etcommercial@@@nbsp;"
	
			if i>oldi:
				t=t+'</tr>\n<tr>\n'
			oldi=i
			
			t=t+'<td'
			if nbcolonnescellule != 1:
				t=t+' valign="top" colspan="'+repr(nbcolonnescellule)+'"'
			elif nblignescellule != 1:
				t=t+' valign="middle" rowspan="'+repr(nblignescellule)+'"'
			else:
				t=t+' valign="top"'
			if corpscellule=='\\hline':
				#t=t+' bgcolor=black colspan='+repr(nbtotalcolonnes)+'>\n'
				#t=t+' bgcolor="black">\n'
				#t=t+'<table summary="ligne" border="0" width="100%" cellspacing="0" cellpadding="0">'
				#t=t+'<table summary="ligne" border="0" width="100%" cellspacing="0" cellpadding="0">'
				#t=t+'<tr><td></td></tr>'
				#t=t+'</table>\n<p align="justify">\n'
				t=t+'>\n<hr noshade size=1>\n'
			else:
				if alignementcellule == 'l':
					alignementcellule='left'
				elif alignementcellule == 'j':
					alignementcellule='justify'
				elif alignementcellule=='c':
					alignementcellule='center'
				elif alignementcellule=='r':
					alignementcellule='right'
				else:
					print "tabular(): aie. Cas non prévu dans l'alignement des cellules."
					print "alignementcellule=*",alignementcellule+'*'
					sys.exit(1)
				t=t+' align="'+alignementcellule+'">\n'
				t=t+corpscellule+'\n'
			t=t+'</td>\n'
	t=t+'</tr>\n</table>\n<p align="justify">\n'



	return t

#---------------------------------------------------------------------#
def dehtml(s):

	# enlève les balises html de la chaine donnée en argument
	
	debut=string.find(s,'<')
	while debut!=-1:
		fin=string.find(s,'>',debut)
		s=s[:debut]+s[fin+1:]
		debut=string.find(s,'<')
		
	return s

#---------------------------------------------------------------------#
def findelignes(s):

	s=string.replace(s,"\r\n","\n")
	s=string.replace(s,"\r","\n")
	s=string.replace(s,"\f\n","\n")
	s=string.replace(s,"\f","\n")
	s=string.replace(s,"\t","   ")
	# ce qui suit est un méchant hack du 18 février 2003
	s=re.sub('\n\n+','\n\n',s)

	return s

#---------------------------------------------------------------------#
def decoupe(s,argn):

	# découpe le document en 3 morceaux:
	# 1: -> \begin{document}
	# 2: \begin{document} -> \maketitle
	# 3: \maketitle -> \end{document}
	
	cible1='\\begin{document}'
	cible2='\\maketitle'
	cible3='\\end{document}'
	
	s=effaceseulementcommande(s,'onlyhtml')

	debut1=string.find(s,cible1)
	while debut1 != -1 and estentre(s,debut1,'\\begin{verbatim}','\\end{verbatim}') == 1:
		debut1=string.find(s,cible1,debut1+1)
		
	if debut1 != -1:
		head=s[:debut1]
	else:
		head=''
	
	debut2=string.find(s,cible2)
	if debut2 != -1:
		ismaketitle=1
	else:
		ismaketitle=0
	if debut1 != -1 and debut2 != -1 :
		entete=string.strip(s[debut1+len(cible1):debut2])
	else:
		entete=''
		
	debut3=string.find(s,cible3)
	while debut3 != -1 and estentre(s,debut3,'\\begin{verbatim}','\\end{verbatim}') == 1:
		debut3=string.find(s,cible3,debut3+1)
	
	if debut2 != -1 and debut3 != -1 :
		s=s[debut2+len(cible2):debut3]
	elif debut1 != -1 and debut2 == -1 and debut3 != -1 :
		s=s[debut1+len(cible1):debut3]
#	elif argn!=1:
#		print "decoupe(): warning. Ni \\begin{document} ni \\end{document}."

	return s,head,entete,ismaketitle

#---------------------------------------------------------------------#
def include(s):

	s=re.sub('\\\\include *\{','\\\\input{',s)
	
	return s

#---------------------------------------------------------------------#
def normalinput(s):

	pattern='\\\\input *\{'
	debut,fin=myfind(s,pattern)
	while debut != -1:
		fintout=findrightbrace(s,fin)
		file=s[fin:fintout]
		f=open(file+'.tex','r')
		t=f.read()
		f.close()
		s=s[:debut]+t+s[fintout+1:]
		debut,fin=myfind(s,pattern)
	
	s=findelignes(s)
	
	return s

#---------------------------------------------------------------------#
def verbatiminput(s):

	cible='\\verbatiminput{'
	debut=string.find(s,cible)
	while debut != -1:

		par=string.rfind(s,'\n',debut)
		chaine=s[par:debut]
		pourcent=string.count(chaine,'%')

		if pourcent!=0:
			s=s[:debut]+s[debut+len(cible):]
		else:
			fin=findrightbrace(s,debut+len(cible))
			file=s[debut+len(cible):fin]
			f=open(file,'r')
			t=f.read()
			f.close()
			s=s[:debut]+'\\begin{verbatim}'+t+'\\end{verbatim}'+s[fin+1:]
	
		debut=string.find(s,cible)
	
	return s

#---------------------------------------------------------------------#
def verbatim(s):

	cible1='\\begin{verbatim}'
	cible2='\\end{verbatim}'
	k=0
	verbatimlist=[]
	debut=string.find(s,cible1)

	while debut != -1:

		par=string.rfind(s,'\n',debut)
		chaine=s[par:debut]
		pourcent=string.count(chaine,'%')
		#print 'pourcent=',pourcent
		if pourcent!=0:
			s=s[:debut]+s[debut+len(cible1):]
		else:
			fin=string.find(s,cible2,debut)
			champ=s[debut+len(cible1):fin]
			#print "champ=*",champ,"*"

			verbatimlist.append(champ)
			s=s[:debut]+'@@@verbatim'+repr(k)+'@@@\\par'+s[fin+len(cible2):]
			k=k+1
			
		debut=string.find(s,cible1)

	nbverbatim=len(verbatimlist)
	
	return s,verbatimlist,nbverbatim

#---------------------------------------------------------------------#
def verb(s):

	cible='\\verb'
	k=0
	verblist=[]
	debut=string.find(s,cible)
	while debut != -1:
		carac=s[debut+len(cible):debut+len(cible)+1]
		fin=string.find(s,carac,debut+len(cible)+1)
		verblist.append(s[debut+len(cible)+1:fin])
		s=s[:debut]+'@@@verb'+repr(k)+'@@@'+s[fin+1:]
		k=k+1
		debut=string.find(s,cible)
	nbverb=k

	return s,verblist,nbverb

#---------------------------------------------------------------------#
def verbatimbis(s,nbverbatim,verbatimlist,nbverb,verblist):

	for k in range(nbverbatim):
		champ=verbatimlist[k]
		remplace='<table summary="verbatim" bgcolor="linen" cellpadding="5"><tr><td><pre>'\
		+champ+'</pre></td></tr></table>'
		s=string.replace(s,'@@@verbatim'+repr(k)+'@@@',remplace)
	
	for k in range(nbverb):
		champ=verblist[k]
		remplace='<tt>'+champ+'</tt>'
		s=string.replace(s,'@@@verb'+repr(k)+'@@@',remplace)
	
	return s
	
#---------------------------------------------------------------------#
def echappements(s):

	s=string.replace(s,'>','@@@gt@@@')
	s=string.replace(s,'<','@@@lt@@@')
	s=string.replace(s,"{\\~n}","@@@etcommercial@@@ntilde;")
	s=string.replace(s,"\\~n","@@@etcommercial@@@ntilde;")
	s=string.replace(s,"\\~{n}","@@@etcommercial@@@ntilde;")
	s=string.replace(s,'\\~','@@@tilde@@@')
	s=string.replace(s,'\\#','@@@diese@@@')
	s=string.replace(s,'\\sharp','@@@diese@@@')
	s=string.replace(s,'\\{','@@@accoladegauche@@@')
	s=string.replace(s,'\\}','@@@accoladedroite@@@')
	s=string.replace(s,'\\^','@@@chapeau@@@')
	s=string.replace(s,'\\$','@@@dollar@@@')
	s=string.replace(s,'\\_','@@@underscore@@@')
	s=string.replace(s,'$\\backslash$','@@@backslash@@@')
	s=string.replace(s,'\\backslash','@@@backslash@@@')
	s=string.replace(s,'\\\\','@@@doublebarre@@@')
	s=string.replace(s,'\\&','@@@etcommercial@@@amp;')

	return s

#---------------------------------------------------------------------#
def commentaires(s):

#	cible='%'
#	debut=string.find(s,cible)
#
#	while debut !=-1:
#		
#		if estentre(s,debut,'\\begin{verbatim}','\\end{verbatim}') == 1 :
#			replace='@@@pourcent@@@'
#			s=s[:debut]+replace+s[debut+len(cible):]
#		elif estentre(s,debut,'\\verb"','"') == 1 :
#			replace='@@@pourcent@@@'
#			s=s[:debut]+replace+s[debut+len(cible):]
#		elif s[debut-1]=='\\':
#			replace='@@@pourcent@@@'
#			s=s[:debut-1]+replace+s[debut+len(cible):]
#		else:
#			replace='@@@commentaire@@@'
#			par=string.find(s,'\n',debut)
#			s=s[:debut]+replace+s[par+1:]
#
#		debut=string.find(s,cible)

	s=string.replace(s,'\\%','@@@pourcent@@@')
	s=re.sub('%.*\n','@@@commentaire@@@',s)
	
	return s

#---------------------------------------------------------------------#
def taillecar(s):
	
	s=string.replace(s,'\\begin{Huge}','<big><big><big><big>')
	s=string.replace(s,'\\end{Huge}','</big></big></big></big>')
	s=string.replace(s,'\\begin{huge}','<big><big><big>')
	s=string.replace(s,'\\end{huge}','</big></big></big>')
	s=string.replace(s,'\\begin{Large}','<big><big>')
	s=string.replace(s,'\\end{Large}','</big></big>')
	s=string.replace(s,'\\begin{large}','<big>')
	s=string.replace(s,'\\end{large}','</big>')
	s=string.replace(s,'\\begin{small}','<small>')
	s=string.replace(s,'\\end{small}','</small>')
	s=string.replace(s,'\\begin{footnotesize}','<small><small>')
	s=string.replace(s,'\\end{footnotesize}','</small></small>')
	
	s=re.sub('\{\\\\Huge( |\n|@@@commentaire@@@)+','\\Huge{',s)
	s=commandeetunargument(s,'Huge','<big><big><big><big>','</big></big></big></big>')
	s=re.sub('\{\\\\huge( |\n|@@@commentaire@@@)+','\\huge{',s)
	s=commandeetunargument(s,'huge','<big><big><big>','</big></big></big>')
	s=re.sub('\{\\\\Large( |\n|@@@commentaire@@@)+','\\Large{',s)
	s=commandeetunargument(s,'Large','<big><big>','</big></big>')
	s=re.sub('\{\\\\large( |\n|@@@commentaire@@@)+','\\large{',s)
	s=commandeetunargument(s,'large','<big>','</big>')
	s=re.sub('\{\\\\normalsize( |\n|@@@commentaire@@@)+','\\\\normalsize{',s)
	s=commandeetunargument(s,'normalsize','<font size="3">','</font>')
	s=re.sub('\{\\\\small( |\n|@@@commentaire@@@)+','\\small{',s)
	s=commandeetunargument(s,'small','<small>','</small>')
	s=re.sub('\{\\\\footnotesize( |\n|@@@commentaire@@@)+','\\\\footnotesize{',s)
	s=commandeetunargument(s,'footnotesize','<small><small>','</small></small>')
	#s=string.replace(s,'\\footnotesize','<font size="2">')
	s=re.sub('\{\\\\tiny( |\n|@@@commentaire@@@)+','\\\\tiny{',s)
	s=commandeetunargument(s,'tiny','<small><small><small>','</small></small></small>')

	s=string.replace(s,'\\tiny','<font size="1">')
	s=string.replace(s,'\\small','<font size="2">')
	s=string.replace(s,'\\normalsize','</font>')
	s=string.replace(s,'\\large','<font size="4">')
	s=string.replace(s,'\\Large','<font size="5">')
	s=string.replace(s,'\\huge','<font size="6">')
	s=string.replace(s,'\\Huge','<font size="7">')

	return s

#---------------------------------------------------------------------#
def commandesgenerales(s):

	# commande à effacement simple
	s=string.replace(s,'\\noindent','@@@commentaire@@@')
	s=string.replace(s,'\\bigskip','@@@commentaire@@@')
	s=string.replace(s,'\\medskip','@@@commentaire@@@')
	s=string.replace(s,'\\smallskip','@@@commentaire@@@')
	s=string.replace(s,'\\newpage','@@@commentaire@@@')
	s=string.replace(s,'\\/','@@@commentaire@@@')
	s=string.replace(s,'\\-','@@@commentaire@@@')
	
	# commandes à effacer avec leur argument
	s=effacecommandeetargument(s,'newcommand')
	s=effacecommandeetargument(s,'renewcommand')
	s=effacecommandeetargument(s,'onlylatex')
	#s=effacecommandeetargument(s,'Comm')
	s=effacecommandeetargument(s,'thispagestyle')
	s=effacecommandeetargument(s,'hspace')
	s=effacecommandeetargument(s,'hspace*')
	s=string.replace(s,'\\vspace{1cm}','<br>&nbsp;<br>') # ajout/hack le 9-4-3 
#	s=commandeetunargument(s,'vspace','<br>&nbsp;<br>','') # ajout/hack le 13-1-4 
#	s=commandeetunargument(s,'vspace*','<br>&nbsp;<br>','') # ajout/hack le 13-1-4 
#	s=effacecommandeetargument(s,'vspace*')
	s=effacecommandeetargument(s,'vspace')
	s=effacecommandeetargument(s,'pagestyle')
	
	# commandes à remplacement simple
	s=string.replace(s,'\\,','@@@etcommercial@@@nbsp;')
	s=string.replace(s,'\\thinspace','@@@etcommercial@@@nbsp;')
	#s=string.replace(s,'~','@@@etcommercial@@@nbsp;')
	s=string.replace(s,'\\and','@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;')
	s=string.replace(s,'\\quad','@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;')
	s=string.replace(s,'\\ldots','...')
	s=string.replace(s,'\\dots','...')
	#s=string.replace(s,'\\S','@@@etcommercial@@@sect;')
	s=re.sub('\\\\S([^a-zA-Z])','{@@@etcommercial@@@sect;}\\1',s)
	s=re.sub('\\\\S([^a-zA-Z])','{@@@etcommercial@@@sect;}\\1',s)

	s=string.replace(s,'\\copyright','@@@etcommercial@@@copy;')
	s=string.replace(s,'\\TeX','<tt>T<sub>E</sub>X</tt>')
	s=string.replace(s,'\\LaTeX2e','<tt>L<sup>A</sup>T<sub>E</sub>X2<sub><big><font face="symbol">e</font></big></sub></tt>')
	#s=string.replace(s,'\\LaTeX2e','<tt>L<sup>A</sup>T<sub>E</sub>X2<sub><font size="+1">e</font></sub></tt>')
	s=string.replace(s,'\\LaTeX','<tt>L<sup>A</sup>T<sub>E</sub>X</tt>')
	s=string.replace(s,'\\lbrack','[')
	s=string.replace(s,'\\rbrack',']')
	s=re.sub('([^\\\\])\\\\ ','\\1 ',s)
	
	
	# commandes à effacer sans leur argument
	s=effaceseulementcommande(s,'mbox')
	s=effaceseulementcommande(s,'onlyhtml')
	s=effaceseulementcommande(s,'voidhref')
	s=effaceseulementcommande(s,'displaystyle')
	s=string.replace(s,'\\displaystyle','')
	
	# commandes à simple argument
	s=commandeetunargument(s,'maj','<p align="right"><small><font color="gray">m-à-j: <!-- maj -->',\
	'<!-- /maj --></font></small></p>')
	s=re.sub('\\\\includegraphics( |\*)*(\[.+?\])* *\{','\\\\includefigure{',s)
	#s=commandeetunargument(s,'includefigure','<img src="','.jpg" border="0">')
	s=commandeetunargument(s,'includegraphics','{<img src="','.jpg" alt="image">}')
	s=commandeetunargument(s,'marginpar','{<font color="green">[',']</font>}')
	s=commandeetunargument(s,'centerline','<center>','</center>')
	s=commandeetunargument(s,'mathcal','{<font color="purple">','</font>}')
	s=commandeetunargument(s,'paragraph','<p align="justify"><b>','</b>}')
	
	# commandes à double argument
	s=commandeetdeuxarguments(s,'notehref','<a href="','">','</a>')
	s=commandeetdeuxarguments(s,'href','<a href="','">','</a>')
	s=commandeetdeuxarguments(s,'mail','<a href="mailto:','">','</a>')
	s=commandeetdeuxarguments(s,'notemail','<a href="mailto:','">','</a>')
	s=commandeetdeuxarguments(s,'textcolor','<font color="','">','</font>')
	
	# synonymes
	s=string.replace(s,'\\tableofcontents','\\toc')

	return s

#---------------------------------------------------------------------#
def environnementsgeneraux(s):

	s=string.replace(s,'\\begin{quote}','<blockquote>')
	s=string.replace(s,'\\end{quote}','</blockquote>')
	s=string.replace(s,'\\begin{quotation}','<blockquote>')
	s=string.replace(s,'\\end{quotation}','</blockquote>')
	s=string.replace(s,'\\begin{abstract}','<big><b>R@@@etcommercial@@@eacute;sum@@@etcommercial@@@eacute;\
	</b></big><br>\n<blockquote>')
	s=string.replace(s,'\\end{abstract}','</blockquote>')
	s=string.replace(s,'\\begin{enumerate}','<ol>')
	s=string.replace(s,'\\end{enumerate}','</ol>')
	
	s=string.replace(s,'\\begin{center}','<center>')
	s=string.replace(s,'\\end{center}','</center>')
	
	s=string.replace(s,'\\begin{flushright}','<p align="right">')
	s=string.replace(s,'\\end{flushright}','</p>')
	s=string.replace(s,'\\begin{description}','<ul>')
	s=string.replace(s,'\\end{description}','</ul>')
	s=string.replace(s,'\\begin{itemize}','<ul>')
	s=string.replace(s,'\\end{itemize}','</ul>')
	s=string.replace(s,'\\begin{displaymath}','<blockquote>')
	s=string.replace(s,'\\end{displaymath}','</blockquote>')

	return s

#---------------------------------------------------------------------#
def item(s):

	pattern='\\\\item *(\[.+?\])* *'
	debut,fin=myfind(s,pattern)
	
	while debut != -1:
		crochetgauche=string.find(s,'[',debut,fin)
		if crochetgauche != -1:
			crochetdroit=string.find(s,']',crochetgauche)
			itemoption=string.strip(s[crochetgauche+1:crochetdroit])
			s=s[:debut]+'<li>'+itemoption+s[crochetdroit+1:]
		else:
			s=s[:debut]+'<li>'+s[fin:]
	
		debut,fin=myfind(s,pattern)
	
	#s=string.replace(s,'<li>','<p><li><p align="justify">')
	
	return s
	
#---------------------------------------------------------------------#
def caption(s):

	pattern='\\\\caption *(\[.+?\])* *\{'
	debut,fin=myfind(s,pattern)
	nbcaption=0
	while debut != -1:
		fintotale=findrightbrace(s,fin)
		caption=s[fin:fintotale]
		nbcaption=nbcaption+1
		s=s[:debut]+'<p align="center"><center>Tab. '+repr(nbcaption)+': '+caption+'</center>'+s[fintotale+1:]
		debut,fin=myfind(s,pattern)

	return s

#---------------------------------------------------------------------#
def tilde(s):

	s=re.sub('(\<a href=".+)~(.+\>)','\\1@@@tilde@@@\\2',s)
	s=re.sub('(\<a href=".+)~(.+\>)','\\1@@@tilde@@@\\2',s)
	s=re.sub('(\<a href=".+)~(.+\>)','\\1@@@tilde@@@\\2',s)
	s=re.sub('(\<a href=".+)~(.+\>)','\\1@@@tilde@@@\\2',s)
	s=re.sub('(\<a href=".+)~(.+\>)','\\1@@@tilde@@@\\2',s)
	s=re.sub('(\<a href=".+)~(.+\>)','\\1@@@tilde@@@\\2',s)
	# un hack pas très joli au cas où plusieurs tildes sur une ligne.
	s=string.replace(s,'~','@@@etcommercial@@@nbsp;')

#	while debut != -1:
#
#		pos1=string.rfind(s,'<a href="',0,debut)
#		pos2=string.find(s,'">',debut)
#		posespace=string.find(s,' ',pos1+len('<a href="'),pos2)
#		posrc=string.find(s,'\n',pos1+len('<a href="'),pos2)
#		# ce n'est pas très joli comme méthode...@@@
#		#print s[debut:debut+10],
#		#print pos1,pos2,posespace,posrc
#		if posespace != -1 and posrc != -1 or pos1 == -1 or pos2 == -1:
#			s=s[:debut]+'@@@etcommercial@@@nbsp;'+s[debut+1:]
#		else:
#			s=s[:debut]+'@@@tilde@@@'+s[debut+1:]
#		
#		debut=string.find(s,cible)
	
	return s	

#---------------------------------------------------------------------#
def label(s):

	cible='\\label{'
	debut=string.find(s,cible)
	
	while debut != -1:
	
		fin=findrightbrace(s,debut+len(cible))
		label=s[debut+len(cible):fin]
		
		remplace=''
		# XXX: il faudrait aussi gérer les label dans les figures.
		if estentre(s,debut,'\\begin{table}','\\end{table}') == 1:
			table=string.count(s,'\\begin{table}',0,debut)
			remplace=repr(table)
		else:

			# Il faudrait refaire cette partie
			# boucler sur tous les \ref et leur argument
			# chercher le label correspondant
			# chercher les chap/section/soussection/sousoussection correspondantes
			# remplacer le \ref
			# à la fin, éliminer tous les \label (inutilisés)
			
			nbchapteretoile=string.count(s,'\\chapter*',0,debut)
			nbchapter=string.count(s,'\\chapter',0,debut)-nbchapteretoile
			if nbchapter != 0:
				remplace=repr(nbchapter)+'.'
				poschapteretoile=string.rfind(s,'\\chapter*',0,debut)
				poschapter=string.rfind(s,'\\chapter',0,debut)
				poschapter=max(poschapteretoile,poschapter)
			else:
				poschapter=0
			
			nbsectionetoile=string.count(s,'\\section*',poschapter,debut)
			nbsection=string.count(s,'\\section',poschapter,debut)-nbsectionetoile
			if nbsection != 0:
				remplace=remplace+repr(nbsection)+'.'
				possectionetoile=string.rfind(s,'\\section*',poschapter,debut)
				possection=string.rfind(s,'\\section',poschapter,debut)
				possection=max(possectionetoile,possection)
			else:
				possection=poschapter
	
			nbsubsectionetoile=string.count(s,'\\subsection*',possection,debut)
			nbsubsection=string.count(s,'\\subsection',possection,debut)-nbsubsectionetoile
			if nbsubsection != 0:
				remplace=remplace+repr(nbsubsection)+'.'
				possubsectionetoile=string.rfind(s,'\\subsection*',possection,debut)
				possubsection=string.rfind(s,'\\subsection',possection,debut)
				possubsection=max(possubsectionetoile,possubsection)
			else:
				possubsection=poschapter

			nbsubsubsectionetoile=string.count(s,'\\subsubsection*',possubsection,debut)
			nbsubsubsection=string.count(s,'\\subsubsection',possubsection,debut)-nbsubsubsectionetoile
			if nbsubsubsection !=0:
				remplace=remplace+repr(nbsubsubsection)+'.'
			
			if remplace=='':
				#print 'remplace=vide'
				remplace='1'
			else:
				remplace=remplace[:-1]

		#print 'chapter...=',nbchapter,nbsection,nbsubsection,nbsubsubsection
		#print 'poschapter...=',poschapter,possection,possubsection
		#print 'label,remplace=',label,'*',remplace

		remplace='<a href="#'+remplace+'">'+remplace+'</a>'

		s=s[:debut]+s[fin+1:]
		s=string.replace(s,'\\ref{'+label+'}',remplace)
		debut=string.find(s,cible)

	return s	

#---------------------------------------------------------------------#
def index(s):

	cible='\\index{'
	debut=string.find(s,cible)
	k=0
	listeindex=[]
	
	while debut != -1:

		fin=findrightbrace(s,debut+len(cible))
		index=s[debut+len(cible):fin]
		k=k+1
		listeindex.append((index,k))
		remplace='<a name="index-'+repr(k)+'"></a>'
		s=s[:debut]+remplace+s[fin+1:]
		debut=string.find(s,cible)

	listeindex.sort()
#	print listeindex
	printindex=''
	oldindex=''

	for item in listeindex:
	
		index=item[0]
		k=item[1]
		
		if index!=oldindex:
			if oldindex!='':
				printindex=printindex+'<!-- /motindexe -->'
			printindex=printindex+'\n\n<!-- motindexe -->'+index+' <a href="#index-'+repr(k)+'">'+repr(k)+'</a>'
			oldindex=index
		else:
			printindex=printindex+', <a href="#index-'+repr(k)+'">'+repr(k)+'</a>'
			oldindex=index
		
	if printindex!='':
			printindex=printindex+'<!-- /motindexe -->'
	posindex=string.find(s,'\\printindex')
	if posindex!=-1:
		isindex=1
		#s=string.replace(s,'\\printindex',printindex)
	else:
		isindex=0

	return s,printindex,isindex

#---------------------------------------------------------------------#
def thanks(s):

	#print "avant-thanks-s=*",s,"*"

	cible='\\thanks{'
	debut=string.find(s,cible)
	k=0
	notes=''
		
	while debut != -1:
		k=k+1
		#print 'k=',k
		fin=findrightbrace(s,debut+len(cible))
		champ=s[debut+len(cible):fin]
		#print "champ=*",champ,"*"

		notes=notes+'<p align="justify"><a name="notedebut"></a>'\
		+'<a href="#appelthanks"><sup>'+'*'*k+'</sup></a>'\
		+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'\
		+string.strip(champ)
		s=s[:debut]+'<a name="appelthanks"></a>'\
		+'<sup><a href="#notethanks">'+'*'*k+'</a></sup>'+s[fin+1:]
		
		debut=string.find(s,cible)
		

	#print "apres-thanks-s=*",s,"*"
	#print "notes=*",notes,"*"

	return s,notes

#---------------------------------------------------------------------#
def figure(s):

	s=string.replace(s,'\\begin{figure}','\\begin{table}[]')
	s=string.replace(s,'\\end{figure}','\\end{table}')
		
	return s
	
#---------------------------------------------------------------------#
def table(s):

	cible='\\begin{table}['
	debut=string.find(s,cible)
	while debut != -1:
		fin=string.find(s,']',debut)
		s=s[:debut]+s[fin+1:]
		debut=string.find(s,cible)
	s=string.replace(s,'\\end{table}','')

	return s

#---------------------------------------------------------------------#
def longtable(s):

	s=string.replace(s,'\\begin{longtable}','\\begin{tabular}')
	s=string.replace(s,'\\end{longtable}','\\end{tabular}')
		
	s=string.replace(s,'\\begin{array}','\\begin{tabular}')
	s=string.replace(s,'\\end{array}','\\end{tabular}')
		
	return s

#---------------------------------------------------------------------#
def gestiontabular(s):

	cible1='\\begin{tabular}'
	cible2='\\end{tabular}'
	debut=string.find(s,cible1)
	compteur=0
	while debut != -1:
	
		compteur=compteur+1
		fin=string.find(s,cible2,debut)

		debut=string.rfind(s,cible1,0,fin)
			
		tabularlatex=s[debut:fin+len(cible2)]
		tabularhtml=tabular(tabularlatex,compteur)
		s=s[:debut]+tabularhtml+s[fin+len(cible2):]
		debut=string.find(s,cible1)
	
	return s
	
#---------------------------------------------------------------------#
def frac(s):

	cible='\\frac{'
	debut1=string.find(s,cible)
	while debut1 != -1:
		fin1=findrightbrace(s,debut1+len(cible)-1)
		numerateur=s[debut1+len(cible):fin1]
		#print "s(60)=", s[debut1:debut1+60], "*"
		#print "numérateur=", numerateur, "*"
		
		if re.search('[^a-zA-Z0-9\']',string.strip(dehtml(numerateur))) != None:
			numerateur='('+numerateur+')'
		debut2=string.find(s,'{',fin1)	
		fin2=findrightbrace(s,debut2)
		denominateur=s[debut2+1:fin2]
	
		if re.search('[^a-zA-Z0-9\']',string.strip(dehtml(denominateur))) != None:
			denominateur='('+denominateur+')'
		s=s[:debut1]+numerateur+'/'+denominateur+s[fin2+1:]
		debut1=string.find(s,cible)
		
		#print "numérateur, dénominateur=", numerateur, "*", denominateur
	
	return s
#---------------------------------------------------------------------#
def sqrt(s):

	cible='\\sqrt{'
	debut=string.find(s,cible)
	while debut != -1:
		fin=findrightbrace(s,debut+len(cible))
		champs=s[debut+len(cible):fin]
		if re.search('[^a-zA-Z0-9\']',string.strip(dehtml(champs))) != None:
		#if len(string.strip(dehtml(champs))) != 1:
			champs='('+champs+')'
		s=s[:debut]+'\/<sup>@@@etcommercial@@@macr;</sup>'+champs+s[fin+1:]
		debut=string.find(s,cible)
		
	return s
#---------------------------------------------------------------------#
def simpleanchor(s):

	cible='\\simpleanchor{'
	debut=string.find(s,cible)
	while debut != -1:
		fin=findrightbrace(s,debut+len(cible))
		ancre=s[debut+len(cible):fin]
		s=s[:debut]+'<font color="green"><a name="'+ancre+'">'\
		+ancre+'</a></font>'+s[fin+1:]
		debut=string.find(s,cible)
			
	return s

#---------------------------------------------------------------------#
def anchor(s):
	cible='\\anchor{'
	debut=string.find(s,cible)
	while debut != -1:
		fin=findrightbrace(s,debut+len(cible))
		ancre=s[debut+len(cible):fin]
		s=s[:debut]+'<a name="'+ancre+'"></a>'+s[fin+1:]
		debut=string.find(s,cible)
			
	return s

#---------------------------------------------------------------------#
def simplehref(s):

	#s=string.replace(s,'\\url{','\\simplehref{')
	s=commandeetunargument(s,"url","<tt>\\simplehref{", "}</tt>")
	
	
	cible='\\simplehref{'
	debut=string.find(s,cible)
	while debut != -1:
		fin=findrightbrace(s,debut+len(cible))
		champ=s[debut+len(cible):fin]
		s=s[:debut]+'<a href="'+champ+'">'+champ+'</a>'+s[fin+1:]
		debut=string.find(s,cible)
		
	return s
#---------------------------------------------------------------------#
def includefigure(s):

	cible='\\includefigure{'
	debut=string.find(s,cible)
	while debut != -1:
		fin=findrightbrace(s,debut+len(cible))
		champ=s[debut+len(cible):fin]
		ext=champ[-4:]
		if ext=='.gif' or  ext=='.jpg' or  ext=='.png':
			newchamp=champ
		elif ext=='.noe':
			newchamp=champ[:-4]
		else:
			newchamp=champ+'.jpg'
		alt=re.sub('.+/(.+)','\\1',newchamp)
		
		s=s[:debut]+'<img src="'+newchamp+'" alt="'+alt+'" border="0">'+s[fin+1:]
		debut=string.find(s,cible)
		
	return s
#---------------------------------------------------------------------#
def cite(s):

	cible='\\cite{'
	debut=string.find(s,cible)
	while debut != -1:
		fin=findrightbrace(s,debut+len(cible))
		etiquette=s[debut+len(cible):fin]
		s=s[:debut]+'<font color="navy"><a href="#'+etiquette+'">['\
		+etiquette+']</a></font>'+s[fin+1:]
		debut=string.find(s,cible)
	
	cible='\\cite['
	debut=string.find(s,cible)
	while debut != -1:
		cadredroit=string.find(s,']',debut)
		option=s[debut+len(cible):cadredroit]
		accogauche=string.find(s,'{',cadredroit)
		fin=findrightbrace(s,accogauche+1)
		etiquette=s[accogauche+1:fin]
		s=s[:debut]+'<font color="navy">[<a href="#'+etiquette+'">'\
		+etiquette+'</a>, '+option+']</font>'+s[fin+1:]
		debut=string.find(s,cible)
	
	return s

#---------------------------------------------------------------------#
def doubledollar(s):

	# doit être lancé avant le greek pour ne pas laisser apparaître des $$
	
	cible='$$'
	debut=string.find(s,cible)
	while debut != -1:
		fin=string.find(s,'$$',debut+1)
		if fin==-1:
			print 'doubledollar(): aie. Impossible de trouver le $$ correspondant à '
			print s[debut:debut+50]
			sys.exit(1)
		
		champ=s[debut+len(cible):fin]
		#print "$$-champ=*",champ,"*"
	
		#champ2=re.sub('([0-9]+)','</i>\\1<i>',champ)
		champ2=re.sub('([0-9]+)','\\1',champ)
		#print "$$-champ2=*",champ2,"*"
		
		#s=s[:debut]+'<center><i>'+champ2+'</i></center>'+s[fin+len(cible):]
		s=s[:debut]+'<center>'+champ2+'</center>'+s[fin+len(cible):]
		debut=string.find(s,cible)
	
	s=string.replace(s,'<i></i>','')
	s=string.replace(s,'<i> </i>',' ')

	return s

#---------------------------------------------------------------------#
def dollar(s):

	s=string.replace(s,'\\(','$')
	s=string.replace(s,'\\)','$')
	cible='$'
	debut=string.find(s,cible)
	while debut != -1:
		fin=string.find(s,'$',debut+1)
		if fin==-1:
			print 'dollar(s): aie. Impossible de trouver le $ correspondante à '
			print 's=',s[debut:debut+50]
			sys.exit(1)
	
		champ=s[debut+len(cible):fin]
		#print "$-champ=*",champ,"*"
#		champ2=re.sub('([0-9]+)','</i>\\1<i>',champ)
		champ2=champ
		
#		s=s[:debut]+'<i>'+champ2+'</i>'+s[fin+len(cible):]
		s=s[:debut]+champ2+s[fin+len(cible):]
		#print 's=', s[debut:debut+50]
		debut=string.find(s,cible)
	
#	s=string.replace(s,'<i>{</i>','{')
#	s=string.replace(s,'</i>}<i>','}')
#	s=string.replace(s,'<i></i>','')
#	s=string.replace(s,'<i> </i>',' ')

	return s
	
#---------------------------------------------------------------------#
def biblio(s):

	cible1='\\begin{thebibliography}'
	debut1=string.find(s,cible1)
	if debut1 == -1:
		thebibliography=''
	else:
		cible2='\\end{thebibliography}'
		fin2=string.find(s,cible2,debut1)
		fincible1=findrightbrace(s,debut1+len(cible1)+1)
		thebibliography='<dl>'+s[fincible1+1:fin2]+'</dl>'
		s=s[:debut1]+s[fin2+len(cible2):]
		
		cible='\\bibitem['
		debut1=string.find(thebibliography,cible)
		while debut1 != -1:
			fin1=string.find(thebibliography,']',debut1+len(cible))
			etiquette=thebibliography[debut1+len(cible):fin1]
			debut2=string.find(thebibliography,'{',fin1)
			fin2=findrightbrace(thebibliography,debut2)
			nom=thebibliography[debut2+1:fin2]
			thebibliography=thebibliography[:debut1]\
			+'<dt><a name="'+etiquette+'"></a><font color="navy">['+nom+\
			']</font><dd><p align="justify">'+thebibliography[fin2+1:]
		
			debut1=string.find(thebibliography,cible)
	
	return s,thebibliography
	
#---------------------------------------------------------------------#
def gestionfootnote(s,notes):

	s=effacecommandeetargument(s,'stepcounter')

	cible='\\footnotemark'
	debutfootnotemark=string.find(s,cible)
	while debutfootnotemark!=-1:

		crochetgauchefootnotemark=string.find(s,'[',debutfootnotemark)
		crochetdroitfootnotemark=string.find(s,']',crochetgauchefootnotemark)
		footnotemark=string.strip(s[crochetgauchefootnotemark+1:crochetdroitfootnotemark])

		cible1='\\footnotetext['+footnotemark+']'

		debutfootnotetext=string.find(s,cible1)
		if debutfootnotetext==-1:
			print 'gestionfootnote(): aie. '+cible1+' non trouvé.'
			print s
			sys.exit(1)
		else:
			accogauchefootnotetext=string.find(s,'{',debutfootnotetext)
			accodroitefootnotetext=findrightbrace(s,accogauchefootnotetext+1)
			corps=s[accogauchefootnotetext+1:accodroitefootnotetext]
			
			s=s[:debutfootnotemark]+'\\footnote{'+corps+'}'\
			+s[crochetdroitfootnotemark+1:debutfootnotetext]\
			+s[accodroitefootnotetext+1:]
					
		debutfootnotemark=string.find(s,cible)

	cible='\\footnote{'
	k=0
	footnote=[]
	debut=string.find(s,cible)
	while debut != -1:
		fin=findrightbrace(s,debut+len(cible)-1)
		champ=string.strip(s[debut+len(cible):fin])
		#print "debut,fin=*",debut,fin,"*"
		#print "fn: champ=*"+champ+"*"
		footnote.append(champ)
		s=s[:debut]+'@@@footnote'+repr(k)+'@@@'+s[fin+1:]
		#print "s=*"+s+"*"
		k=k+1
		debut=string.find(s,cible)
	nbfootnote=k

	if nbfootnote != 0:

		for k in range(nbfootnote):
			remplace='<a name="appel'+repr(k+1)+'"></a>'\
			+'<sup><a href="#note'+repr(k+1)+'"><!-- appelfootnote -->'+repr(k+1)+'<!-- /appelfootnote --></a></sup>'
			s=string.replace(s,'@@@footnote'+repr(k)+'@@@',remplace)
		
			notes=notes+'<p align="justify"><a name="note'+repr(k+1)\
			+'"></a>\n<a href="#appel'+repr(k+1)+'"><sup>'+repr(k+1)+'</sup></a>'\
			+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;<!-- footnote -->'+footnote[k]+'<!-- /footnote -->\n\n'
	
	return s,notes

#---------------------------------------------------------------------#
def gestionsections(s,index,isindex):

	if string.find(s,'\\chapter')!=-1:
		book=1
	else:
		book=0
	#print "book=*",book,"*"

#	s=commandeetunargument(s,'chapter*','<h1>','</h1>')
#	s=commandeetunargument(s,'section*','<h2>','</h2>')
#	s=commandeetunargument(s,'subsection*','<h3>','</h3>')
#	s=commandeetunargument(s,'subsubsection*','<h4>','</h4>')

	titlessections=[]
	pattern='\\\\(chapter|section|subsection|subsubsection|chapter\*|section\*|subsection\*|subsubsection\*)(\[.+?\])*\{'
	match=re.search(pattern,s)
	c=0
	i=0
	j=0
	k=0

	while match != None:

		commande=match.group(1)
		option=match.group(2)
		#print commande,option
		debut=match.start()

		if option==None:
			fin=findrightbrace(s,debut+len(commande)+1)
			corps=s[debut+len(commande)+2:fin]
			option=corps
		else:
			fin=findrightbrace(s,debut+len(commande)+len(option)+1)
			corps=s[debut+len(commande)+len(option)+2:fin]
			option=option[1:-1]

		#print 'commande=',commande
		#print 'option=',option
		#print 'corps=',corps
		
		if commande=='chapter*':
			#c=c+1
			i=0
			j=0
			k=0
			ancre=repr(c)
			u='<br><br>'
			u=u+'<!-- chapter --><a name="'+ancre+'*"></a><h1>'
			u=u+corps+'</h1><!-- /chapter --><br><br><p align="justify">'
			t=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+option
		elif commande=='chapter':
			c=c+1
			i=0
			j=0
			k=0
			ancre=repr(c)
			u='<br><br><!-- chapter --><h1>Chapitre '+ancre+'</h1><br>'
			u=u+'<a name="'+ancre+'"></a><h1>'
			u=u+corps+'</h1><!-- /chapter --><br><br><p align="justify">'
			t=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+option
		elif commande=='section*':
			#i=i+1
			j=0
			k=0
			ancre=repr(i)
			if book==1:
				ancre=repr(c)+'.'+ancre
			tt=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+corps
			u='<!-- section --><a name="'+ancre+'*"></a><h2>'+tt+'</h2><!-- /section --><p align="justify">'
			t=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+option
		elif commande=='section':
			i=i+1
			j=0
			k=0
			ancre=repr(i)
			if book==1:
				ancre=repr(c)+'.'+ancre
			tt=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+corps
			u='<!-- section --><a name="'+ancre+'"></a><h2>'+tt+'</h2><!-- /section --><p align="justify">'
			t=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+option
		elif commande=='subsection*':
			#j=j+1
			k=0
			ancre=repr(i)+'.'+repr(j)
			if book==1:
				ancre=repr(c)+'.'+ancre
			tt=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+corps
			u='<!-- subsection --><a name="'+ancre+'*"></a><h3>'+tt+'</h3><!-- /subsection --><p align="justify">'
			t=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+option
		elif commande=='subsection':
			j=j+1
			k=0
			ancre=repr(i)+'.'+repr(j)
			if book==1:
				ancre=repr(c)+'.'+ancre
			tt=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+corps
			u='<!-- subsection --><a name="'+ancre+'"></a><h3>'+tt+'</h3><!-- /subsection --><p align="justify">'
			t=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+option
		elif commande=='subsubsection*':
			#k=k+1
			ancre=repr(i)+'.'+repr(j)+'.'+repr(k)
			if book==1:
				ancre=repr(c)+'.'+ancre
			tt=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+corps
			u='<!-- subsubsection --><a name="'+ancre+'*"></a><h4>'+tt+'</h4><!-- /subsubsection --><p align="justify">'
			t=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+option
		elif commande=='subsubsection':
			k=k+1
			ancre=repr(i)+'.'+repr(j)+'.'+repr(k)
			if book==1:
				ancre=repr(c)+'.'+ancre
			tt=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+corps
			u='<!-- subsubsection --><a name="'+ancre+'"></a><h4>'+tt+'</h4><!-- /subsubsection --><p align="justify">'
			t=ancre+'@@@etcommercial@@@nbsp;@@@etcommercial@@@nbsp;'+option
		else:
			print 'gestionsections(): aie. Cas non prévu dans les sections: '\
			+commande
			sys.exit(1)
		
		#print ancre
		titlessections.append((ancre,t))
		s=s[:debut]+u+s[fin+1:]
		match=re.search(pattern,s)
	
	if isindex==1:
		if book==1:
			c=c+1
			ancre=repr(c)
			t='Index'
			u='<br><br><!-- titreindex --><a name="'+ancre+'"></a><h1>'+t+\
				'</h1><!-- /titreindex --><!-- corpsindex -->'\
				+index+'<!-- /corpsindex --><p align="justify">'
		else:
			i=i+1
			ancre=repr(i)
			t='Index'
			u='<!-- titreindex --><a name="'+ancre+'"></a><h2>'+t+\
				'</h2><!-- /titreindex --><!-- corpsindex -->'\
				+index+'<!-- /corpsindex --><p align="justify">'
		
		titlessections.append((ancre,t))
		s=string.replace(s,'\\printindex',u)
	
	if len(titlessections) != 0:
                toctwo='<ul>\n'
                k=0
                for champs in titlessections:
                        k=k+1
                        ancre=champs[0]
                        section=champs[1]
                        toctwo=toctwo+'<li><a href="#'+ancre+'">'+section+'</a></li>\n'
                toctwo=toctwo+'</ul>\n\n<!-- /toc -->\n\n'

#		toctwo='<small>\n'
#		k=0
#		for champs in titlessections:
#			k=k+1
#			ancre=champs[0]
#			section=champs[1]
#			toctwo=toctwo+'<a href="#'+ancre+'">'+section+'</a>'
#			if k!=len(titlessections):
#				toctwo=toctwo+'@@@etcommercial@@@nbsp;| \n'
#			else:
#				toctwo=toctwo+'\n'
#		toctwo=toctwo+'</small>'
	else:
		toctwo=''
	
	cible='\\toctwo'
	if string.find(s,cible) != -1 :
		s=string.replace(s,cible,toctwo)
	
	cible='Introduzione'
	if string.find(s,cible) != -1 :
		s=string.replace(s,cible,'')

	cible='\\toc'
	if string.find(s,cible) != -1 :
		t=' '
#		t='\n\n<!-- toc -->\n\n<ul>\n'
#		k=0
#		for champs in titlessections:
#			k=k+1
#			ancre=champs[0]
#			section=champs[1]
#			t=t+'@@@li@@@'+'<a href="#'+ancre+'">'+section+'</a></li>\n'
#		t=t+'</ul>\n\n<!-- /toc -->\n\n'
		s=string.replace(s,cible,t)
	
	return s,toctwo
	
#---------------------------------------------------------------------#
def maketitle(s,ismaketitle):
	
	import time
	
	#print "maketitle-s=*",s,"*"

	s=accents(s)
	
	title=enregistre(s,'title')
	#print "title=*",title,"*"

	author=enregistre(s,'author')
	#print "author=*",author,"*"

	date=enregistre(s,'date')

	#s=effacecommandeetargument(s,'title')
	#s=effacecommandeetargument(s,'author')
	#s=effacecommandeetargument(s,'date')
	
	t=''
	if title != None:
		t=t+'<h1><!-- titre -->'+title+'<!-- /titre --></h1>\n'
	else:
		title=''
	
	if date != None and date != '':
        	t=t+'<h6 align="right"><!-- date -->'+date+'<!-- /date --></h6>\n\n'
        elif date == None:
                t=t+'<h6 align="right"><!-- date -->'+time.strftime("%d-%m-%Y",time.localtime(time.time()))+'<!-- /date --></h6>\n\n'
	t=t+'<hr>\n'
	if ismaketitle==1:
		if author != None:
			t=t+'<font size="+1"><b>A cura di</b><br>'
			t=t+'<!-- auteur -->'+author+'<!-- /auteur --></font><hr>\n\n'
	else:
		t=''
	
	return t,title
	
#---------------------------------------------------------------------#
def nettoyage(s):

	s=string.replace(s,'{}','')
	s=string.replace(s,'{','')
	s=string.replace(s,'}','')

	s=string.replace(s,'<center></center>','')
	
	s=re.sub('\n\n+','\n\n',s)
	s=re.sub('\<a name="haut"\>\</a\>\n*(<p align="justify"\>)*\n*(\<p\>)*',\
'<a name="haut"></a>\n\n',s)

	s=re.sub('\<p align="justify"\>\n*\<\!-- /corps du texte --\>\n*\</td\>',\
	'\n\n<!-- /corps du texte -->\n\n</td>',s)

	s=re.sub(' +',' ',s)
	s=string.replace(s,'<i> </i>',' ')
	s=string.replace(s,'<i></i>','')
	s=re.sub('\n +','\n',s)

	s=re.sub('\n\n+','\n\n',s)
	s=re.sub('(\n*\<p align="justify"\>\n*)+','\n\n<p align="justify">\n\n',s)
	s=re.sub('\<p align="justify"\>\n*(@@@commentaire@@@)*\n*\<p align="right"\>','<p align="right">',s)
	s=re.sub('</p>\n*\<!-- tableau --\>','</p>\n\n<!-- tableau -->',s)
	
	s=re.sub('\<ul\>\n*\<p align="justify"\>\n*','<ul>',s)
	s=re.sub('\<p align="justify"\>\n*\<li\>\n*','<li>',s)
	s=re.sub('\<p align="justify"\>\n*\</center\>\n*','</center>\n',s)
	s=re.sub('\<p align="justify"\>\n*\<center\>\n*','<center>\n',s)
	s=re.sub('\<p align="center"\>\<center\>','<center>\n',s)
	#s=re.sub('\<center\>\n*\<center\>','<center>',s)
	#s=re.sub('\</center\>\n*\</center\>','</center>',s)
	
	# hack pour rattacher des liens suivants à la toc de haut de page
	s=re.sub('\</a\>\n*\</small\>\</center\>\n*\<!-- /toctwo --\>\n*\<!-- entete --\>\n*\<center\>\n*\<small\>\<a href=',\
	'</a>\n\n<!-- /entete -->\n\n<!-- /toctwo -->\n\n&nbsp;&nbsp;|| &nbsp; <a href=',s)

	s=re.sub('\<p align="justify"\>\n*\<!-- toc --\>\n*\<p align="justify"\>\n','<p align="justify">\n\n<!-- toc -->',s)

	s=re.sub('\<p\>\<center\>\n+','<center>',s)

	s=re.sub('\</center\>\n+\<center\>','\n',s)
	
#	s=re.sub('\<p align="justify"\>\n*\<p align="justify"\>','<p align="justify">',s)
	s=re.sub('\n\n+','\n\n',s)
	

	return s

#---------------------------------------------------------------------#
def nonreconnues(s,argc):

	pattern='\\\\[a-zA-Z]+'
	debut,fin=myfind(s,pattern)
	
	while debut != -1:
		commande=s[debut+1:fin]
		
		#print s
		#print "debut=*",debut,"*"
		#print "fin=*",fin,"*"
		#print "commande=*"+commande+"*"
		#print "s[fin]=*",s[fin],"*"
		#print "environnement=*"+s[fin-10:fin]+'*'+s[fin]+'*'+s[fin+1:fin+10]+'*'
		
		if s[fin]=='{':
			pos2=findrightbrace(s,fin)
			champ=s[fin:pos2]
			if argc==1:
				s=s[:debut]+'<!-- '+commande+' -->'+champ+s[pos2:]
			else:
				s=s[:debut]+'<font color="red"><b>@@@backslash@@@'+commande+'</b></font>'\
				+'@@@accoladegauche@@@'+champ+'@@@accoladedroite@@@'+s[pos2:]
		else:
			if argc==1:
				s=s[:debut]+'<!-- '+commande+' -->'+s[fin:]
			else:
				s=s[:debut]+'<font color="red"><b>@@@backslash@@@'+commande+'</b></font>'+s[fin:]
		
		#print "pos2=*",pos2,"*"
		#print "champ=*"+champ+"*"
		#sys.exit(1)
		
		debut,fin=myfind(s,pattern)

#	if argc==1:
#		s=re.sub('(\\\\[a-zA-Z0-9=\'"`]+ *(\[.+?\])* *(\{.+?\})*)','<!-- \\1 -->',s)
#	else:
#		s=re.sub('(\\\\[a-zA-Z0-9=\'"`]+)','<font color="red"><b>\\1</b></font>',s)

	return s

#---------------------------------------------------------------------#
def br(s):


	# doit être placé après la gestion des \n\n
	s=string.replace(s,'@@@doublebarre@@@','<br>')
	s=string.replace(s,'\\newline','<br>')

	return s
	
#---------------------------------------------------------------------#
def desechappement(s):

	s=string.replace(s,'@@@etcommercial@@@','&')
	s=string.replace(s,'@@@pourcent@@@','%')
	s=string.replace(s,'@@@diese@@@','#')
	s=string.replace(s,'@@@accoladegauche@@@','{')
	s=string.replace(s,'@@@accoladedroite@@@','}')
	s=string.replace(s,'@@@underscore@@@','_')
	s=string.replace(s,'@@@chapeau@@@','^')
	s=string.replace(s,'@@@dollar@@@','$')
	s=string.replace(s,'@@@gt@@@','&gt;')
	s=string.replace(s,'@@@lt@@@','&lt;')
	s=string.replace(s,'@@@backslash@@@','\\')
	s=string.replace(s,'@@@tilde@@@','~')
	s=string.replace(s,'@@@sim@@@','~')
	s=string.replace(s,'@@@commentaire@@@','')

	return s

#---------------------------------------------------------------------#
def titlehtml(s,title):

	titlehtml=enregistre(s,'titlehtml')
	#s=effacecommandeetargument(s,'titlehtml')

	if titlehtml==None:
		#titlehtml=dehtml(string.replace(br(title),'<br>',' - '))
		titlehtml=dehtml(br(accents(title)))
	else:
		titlehtml=accents(titlehtml)

	return titlehtml

#---------------------------------------------------------------------#
def motsclefs(s):

	motsclefs=enregistre(s,'motsclefs')
	if motsclefs==None:
		motsclefs=''

	return motsclefs

#---------------------------------------------------------------------#
def modifcomportement(s,fichiermodif,argt,argp):

	fichiermodif=re.sub('%.+\n','YYY',fichiermodif)
	
	cible='commandeetunargument('
	debut=string.find(fichiermodif,cible)
	while debut != -1:
		fin=findrightpar(fichiermodif,debut+len(cible)+1)
		champ=fichiermodif[debut+len(cible):fin]
		arg1,arg2,arg3=string.split(champ,',')
		#print "arg1,arg2,arg3=*",arg1,"*",arg2,"*",arg3,"*"

		s=commandeetunargument(s,arg1,arg2,arg3)
		debut=string.find(fichiermodif,cible,debut+1)
	
	cible='commandeetdeuxarguments('
	debut=string.find(fichiermodif,cible)
	while debut != -1:
		fin=findrightpar(fichiermodif,debut+len(cible)+1)
		champ=fichiermodif[debut+len(cible):fin]
		arg1,arg2,arg3,arg4=string.split(champ,',')
		s=commandeetdeuxarguments(s,arg1,arg2,arg3,arg4)
		debut=string.find(fichiermodif,cible,debut+1)
	
	cible='effacecommandeetargument('
	debut=string.find(fichiermodif,cible)
	while debut != -1:
		fin=findrightpar(fichiermodif,debut+len(cible)+1)
		arg=fichiermodif[debut+len(cible):fin]
		s=effacecommandeetargument(s,arg)
		debut=string.find(fichiermodif,cible,debut+1)
	
	cible='effaceseulementcommande('
	debut=string.find(fichiermodif,cible)
	while debut != -1:
		fin=findrightpar(fichiermodif,debut+len(cible)+1)
		arg=fichiermodif[debut+len(cible):fin]
		s=effaceseulementcommande(s,arg)
		debut=string.find(fichiermodif,cible,debut+1)
	
	cible='expreg('
	debut=string.find(fichiermodif,cible)
	while debut != -1:
		fin=findrightpar(fichiermodif,debut+len(cible)+1)
		champ=fichiermodif[debut+len(cible):fin]
		arg1,arg2=string.split(champ,',')
		s=re.sub(arg1,arg2,s)
		debut=string.find(fichiermodif,cible,debut+1)

	cible='remplace('
	debut=string.find(fichiermodif,cible)
	while debut != -1:
		fin=findrightpar(fichiermodif,debut+len(cible)+1)
		champ=fichiermodif[debut+len(cible):fin]
		arg1,arg2=string.split(champ,',')
		#print "arg1,arg2=*",arg1,"*",arg2,"*"

		arg1='\\'+arg1
		s=string.replace(s,arg1,arg2)
		debut=string.find(fichiermodif,cible,debut+1)
	
	cible='-t'
	debut=string.find(fichiermodif,cible)
	#print "argt(0)=", argt
	while debut != -1:
	 	#print "argt=1"
	 	argt=1
	 	debut=string.find(fichiermodif,cible,debut+1)

	cible='-p'
	debut=string.find(fichiermodif,cible)
	#print "argt(0)=", argt
	while debut != -1:
	 	#print "argt=1"
	 	argp=1
	 	debut=string.find(fichiermodif,cible,debut+1)

	return s,argt,argp

#---------------------------------------------------------------------#
def par(s):

	s=re.sub('\n\n+','\n\n',s)
	cible='\n\n'
	debut=string.find(s,cible)

	while debut !=-1:
		if estentre(s,debut,'<center>','</center>') == 1 :
			replace='</center><center>'
		#XXX il y a un problème ici si des center entourent un tableau
		#XXX et des \n\n...
		elif estentre(s,debut,'<ul>','</ul>') != 1 \
		and estentre(s,debut,'<ol>','</ol>') != 1 :
			replace='<p align="justify">'
		else:
			replace='@@@nn@@@'
			
		s=s[:debut]+replace+s[debut+len(cible):]
		debut=string.find(s,cible)

	s=string.replace(s,'@@@nn@@@','\n\n')
	s=string.replace(s,'<center></center>','')
	s=re.sub('\<p align="justify"\>n*\</center\>','</center>',s)

	cible='\\par'
	debut=string.find(s,cible)
	while debut !=-1:
		if estentre(s,debut,'<center>','</center>') == 1 :
			replace='</center><center>'
		else:
			replace='<p align="justify">'
			
		s=s[:debut]+replace+s[debut+len(cible):]
		debut=string.find(s,cible)

	#s=string.replace(s,'\\par','<p align="justify">')

	s=string.replace(s,'<blockquote>','<blockquote><p align="justify">')
	s=string.replace(s,'</blockquote>','</blockquote><p align="justify">')

	s=string.replace(s,'<li>','<p align="justify"><li><p align="justify">')
	#s=string.replace(s,'<li>','<li><p align="justify">')
	#s=string.replace(s,'<li>','<br>@@@etcommercial@@@nbsp;<li><p align="justify">')
	#s=re.sub('\<ol\>\n*\<br\>@@@etcommercial@@@nbsp;\<li\>','<ol><li>',s)
	#s=re.sub('\<ul\>\n*\<br\>@@@etcommercial@@@nbsp;\<li\>','<ul><li>',s)
	s=string.replace(s,'@@@li@@@','<li>')
	s=re.sub('(\<p align="justify">)*\n*\<ul\>\n*(\<p\>)*','<ul>',s)
	s=re.sub('(\<p align="justify">)*\n*\<ol\>\n*(\<p\>)*','<ol>',s)

	s=re.sub('\</center\>','</center><p align="justify">',s)
	s=re.sub('\<p align="justify"\>\n*\</td\>','</td>',s)
	
	#s=re.sub('\<p align="justify"\>((@@@commentaire@@@)*\<a name="([0-9\.])"\>\</a\>\<h3\>)',\
	#'\\1',s)

	return s

#---------------------------------------------------------------------#
def stringarg(fichierentree):
	
	# renvoie la commande et les arguments utilisés pour lancer le programme
	# (les variables arg* sont globales a priori...)
	
	stringarg=""
	commande=re.sub('.+/','',sys.argv[0])

	if argn==1:
		stringarg=stringarg+' -n'
	elif arga==1:
		stringarg=stringarg+' -a'
	else:
		if argb==1: stringarg=stringarg+' -b'
		if argc==1: stringarg=stringarg+' -c'
		if argp==1: stringarg=stringarg+' -p'
		if argr==1: stringarg=stringarg+' -r'
		if argt==1: stringarg=stringarg+' -t'
	if argf==1 and argfforce==0: stringarg=stringarg+' -f '+nomfichiermodif

	stringarg=stringarg+ ' '+re.sub('.+/','',fichierentree)

	linkstringarg='<a href="http://jpsn.free.fr/jpl2h/readme.html">'\
	+commande+'</a>'+stringarg
	
	stringarg=commande+stringarg
	
	return stringarg,linkstringarg
	
#---------------------------------------------------------------------#
def constructpage(entete,maketitle,motsclefs,corps,toctwo,\
notes,thebibliography,argn,argp,argr,argb,argt):

	ref,linkref=stringarg(fichierentree)
	hr='<p align="left"><hr width="25%" align="left">\n\n'
	
	if argn==1:
		s=''
	else:
		s=''
#		s='<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">\n'
#		s=s+'<html>\n<head>\n'
#		s=s+'<meta name="generator" content="jpl2h.py: a simple python translator '
#		s=s+'from LaTeX to HTML, http://jpsn.free.fr/jpl2h/readme.html">\n'
#		s=s+'<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">\n'
#		if motsclefs!='':
#			s=s+'<meta name="keywords" content="'+motsclefs+'">\n'
#		s=s+'<title>'+titlehtml+'</title>\n'
#		s=s+'<!-- '+'jps : '+time()+' : '+ref+' -->\n' 
#		s=s+'</head>\n<body bgcolor="white">\n\n'
#		s=s+'<a name="haut"></a>\n\n'
#	if argt!=1 and toctwo != '' :
#		s=s+'<!-- toctwo -->\n\n'
#		s=s+'<center>'+toctwo+'</center>\n\n'
#		s=s+'<!-- /toctwo -->\n\n'
	if entete!='':
		s=s+'<!-- entete -->\n\n'
		s=s+entete
#	if argp!=1:
#		s=s+'<!-- tableau -->\n\n'
#		s=s+'<p align="center"><center><table summary="tout" border="0" width="500"><tr><td>\n\n'
	if maketitle!='' and argn!=1:
		s=s+'<!-- maketitle -->\n\n'
		s=s+'\n\n'+maketitle+'\n\n'
		s=s+'<!-- /maketitle -->\n\n'
        if argt!=1 and toctwo != '' :
                s=s+'<font size="+1"><b>Introduzione</b></font><br>'
                s=s+'<!-- toctwo -->\n\n'
                s=s+toctwo+'\n\n'
                s=s+'<!-- /toctwo -->\n\n'
	if argn!=1:
		s=s+'<!-- corps du texte -->\n\n' 
#		s=s+'<font size="+1"><b>Introduzione</b></font><br>'
	s=s+corps
	if notes!='':
		s=s+'\n\n<!-- notes -->\n\n'
		s=s+hr+notes
		s=s+'<!-- /notes -->\n\n'
	if thebibliography!='':
		s=s+'\n\n<!-- bibliography -->\n\n'
		s=s+hr+thebibliography
		s=s+'\n\n<!-- /bibliography -->\n\n'
	if argn!=1:
		s=s+'\n\n<!-- /corps du texte -->\n\n' 
#	if argp!=1:
#		s=s+'</td></tr></table></center>\n\n'
#		s=s+'<!-- /tableau -->\n\n'
#	if argb!=1:
#		s=s+'<br><p align="center"><center><small><a href="#haut">Haut de la page</a></small></center>\n\n'
#	if argr!=1:
#		s=s+'<p><small><small><font color="gray"><tt>'\
#		+linkref+' : '+time()+'</tt></font></small></small>\n\n'
#	if argn!=1:
#		s=s+'</body>\n</html>\n'

	return s
	
#---------------------------------------------------------------------#
def entree():

	import os
	
	arg=sys.argv
	
	arga=0
	argb=0
	argc=0
	argd=0
	argf=0
	argh=0
	argl=0
	argn=0
	argp=0
	argr=0
	argt=0
	
	if arg.count('-n') != 0:
		arga=0
		argb=1
		argn=1
		argp=1
		argr=1
		argt=1
		arg.remove('-n')

	if arg.count('-a') != 0:
		arga=1
		argb=1
		argc=1
		argp=1
		argr=1
		argt=1
		arg.remove('-a')

	if arg.count('-b') != 0:
		argb=1
		arg.remove('-b')

	if arg.count('-c') != 0:
		argc=1	
		arg.remove('-c')
	
	if arg.count('-d') != 0:
		argd=1	
		arg.remove('-d')
	
	contenufichiermodif=''
	nomfichiermodif=''
	if arg.count('-f') != 0:
		argf=1
		index=arg.index('-f')
		if arg[index+1]:
			nomfichiermodif=arg[index+1]
			if os.path.isfile(nomfichiermodif):
				f=open(nomfichiermodif,'r')
				contenufichiermodif=f.read()
				f.close()
			else:
				print 'Impossible de trouver le fichier de modification '\
				+nomfichiermodif
				sys.exit(1)
		else:
			print "Impossible de trouver l'argument de l'option -f"
			sys.exit(1)
		arg.remove('-f')
	
	if arg.count('-h') != 0:
		argh=1
		help()
		sys.exit(0)

	if arg.count('-l') != 0:
		argl=1
		listecommandes()
		sys.exit(0)

	if arg.count('-p') != 0:
		argp=1
		arg.remove('-p')

	if arg.count('-r') != 0:
		argr=1
		arg.remove('-r')

	if arg.count('-t') != 0:
		argt=1	
		arg.remove('-t')

	if len(arg)!=1:
		fichierentree=sys.argv[len(arg)-1]
	else:
		print 'entree(): aie. Pas de fichier en entrée'
		sys.exit(1)

	if os.path.isfile(fichierentree):
		pass
	elif os.path.isfile(fichierentree+'tex'):
			fichierentree=fichierentree+'tex'
	elif os.path.isfile(fichierentree+'.tex'):
			fichierentree=fichierentree+'.tex'
	else:
		print 'entree(): aie. Impossible de trouver "'+fichierentree+'"'
		sys.exit(1)
		
	pospoint=string.rfind(fichierentree,'.')
	ext=fichierentree[pospoint:]
	argfforce=0
	if ext == '.tex' and argf==0:
		radical=fichierentree[:pospoint]
		if os.path.isfile(radical+'.jpl2h'):
			argf=1
			argfforce=1
			nomfichiermodif=radical+'.jpl2h'
			f=open(nomfichiermodif,'r')
			contenufichiermodif=f.read()
			f.close()

	f=open(fichierentree,'r')
	s=f.read()
	f.close()

	return s,fichierentree,nomfichiermodif,contenufichiermodif,arga,argb,argc,argd,argf,argfforce,argn,argp,argr,argt

#---------------------------------------------------------------------#
def fichierdesortie(fichierentree):

	pospoint=string.rfind(fichierentree,'.')
	ext=fichierentree[pospoint:]
	if ext == '.tex':
		fichiersortie=fichierentree[:pospoint]+'.html'
	else:
		fichiersortie=fichierentree+'.html'

	return fichiersortie
	
#---------------------------------------------------------------------#
def ecriture(s,fichiersortie):

	f=open(fichiersortie,'w')
	f.write(s)
	f.close()

	return

#---------------------------------------------------------------------#
#                                Main
#---------------------------------------------------------------------#
import sys
import string
import re

s,fichierentree,nomfichiermodif,fichiermodif,\
arga,argb,argc,argd,argf,argfforce,argn,argp,argr,argt=entree()


#---------------------------------------------------------------------#

if argd==1:
	print 'fichierdesortie()' 
fichiersortie=fichierdesortie(fichierentree)

if argd==1:
	ecriture(s,fichiersortie)
	print 'findelignes()'
s=findelignes(s)

#if argd==1:
#	ecriture(s,fichiersortie)
#	print 'commentaires()' 
#s=commentaires(s)

if argd==1:
	ecriture(s,fichiersortie)
	print 'include()' 
s=include(s)

if argd==1:
	ecriture(s,fichiersortie)
	print 'normalinput()' 
s=normalinput(s)

if argd==1:
	ecriture(s,fichiersortie)
	print 'verbatiminput()' 
s=verbatiminput(s)

if argd==1:
	ecriture(s,fichiersortie)
	print 'verbatim()' 
corps,verbatimlist,nbverbatim=verbatim(s)

if argd==1:
	ecriture(s,fichiersortie)
	print 'commentaires()' 
corps=commentaires(corps) # pour enlever les comm des input

if argd==1:
	ecriture(s,fichiersortie)
	print 'verb()' 
corps,verblist,nbverb=verb(corps)

if argd==1:
	ecriture(s,fichiersortie)
	print 'decoupe()' 
corps,head,entete,ismaketitle=decoupe(corps,argn)

if argd==1:
	ecriture(s,fichiersortie)
	print 'echappements()' 
corps=echappements(corps)
head=echappements(head)
entete=echappements(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'modifcomportement()' 
if argfforce==1 and argd==1:
	print '* fichier de comportement '+nomfichiermodif+' trouvé et pris en compte'
if fichiermodif!='':
	corps,argt,argp=modifcomportement(corps,fichiermodif,argt,argp)
	head,argt,argp=modifcomportement(head,fichiermodif,argt,argp)
	entete,argt,argp=modifcomportement(entete,fichiermodif,argt,argp)

if argd==1:
	ecriture(s,fichiersortie)
	print 'commandesgenerales()' 
corps=commandesgenerales(corps)
head=commandesgenerales(head)
entete=commandesgenerales(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'includefigure()' 
corps=includefigure(corps)
head=includefigure(head)
entete=includefigure(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'environnementsgeneraux()' 
corps=environnementsgeneraux(corps)
entete=environnementsgeneraux(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'item()' 
corps=item(corps)
entete=item(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'longtable()' 
corps=longtable(corps)
entete=longtable(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'gestiontabular()' 
corps=gestiontabular(corps)
entete=gestiontabular(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'accents()' 
corps=accents(corps)
entete=accents(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'greek()' 
corps=greek(corps)
head=greek(head)
entete=greek(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'symbol()' 
corps=symbol(corps)
head=symbol(head)
entete=symbol(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'exposant()' 
corps=exposant(corps)
head=exposant(head)
entete=exposant(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'indice()' 
corps=indice(corps)
head=indice(head)
entete=indice(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'enrichissements()' 
corps=enrichissements(corps)
head=enrichissements(head)
entete=enrichissements(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'doubledollar()' 
corps=doubledollar(corps)
head=doubledollar(head)
entete=doubledollar(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'dollar()' 
corps=dollar(corps)
head=dollar(head)
entete=dollar(entete)

if argd==1:
	print 'taillecar()' 
	ecriture(s,fichiersortie)
corps=taillecar(corps)
head=taillecar(head)
entete=taillecar(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'caption()' 
corps=caption(corps)
entete=caption(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'figure()' 
corps=figure(corps)
entete=figure(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'table()' 
corps=table(corps)
entete=table(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'index()' 
corps,index,isindex=index(corps)

if argd==1:
	ecriture(s,fichiersortie)
	print 'label()' 
corps=label(corps)
entete=label(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'frac()' 
corps=frac(corps)
head=frac(head)
entete=frac(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'sqrt()' 
corps=sqrt(corps)
head=sqrt(head)
entete=sqrt(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'simplehref()' 
corps=simplehref(corps)
entete=simplehref(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'tilde()' 
corps=tilde(corps)
head=tilde(head)
entete=tilde(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'simpleanchor()' 
corps=simpleanchor(corps)
entete=simpleanchor(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'anchor()' 
corps=anchor(corps)
entete=anchor(entete)

if argd==1:
	ecriture(s,fichiersortie)
	print 'cite()' 
corps=cite(corps)
corps,thebibliography=biblio(corps)

if argd==1:
	ecriture(s,fichiersortie)
	print 'thanks()' 
head,notes=thanks(head)

if argd==1:
	ecriture(s,fichiersortie)
	print 'gestionfootnote()' 
corps,notes=gestionfootnote(corps,notes)

if argd==1:
	ecriture(s,fichiersortie)
	print 'gestionsections()' 
corps,toctwo=gestionsections(corps,index,isindex)

if argd==1:
	ecriture(s,fichiersortie)
	print 'maketitle()' 
maketitle,title=maketitle(head,ismaketitle)

if argd==1:
	ecriture(s,fichiersortie)
	print 'titlehtml()' 
titlehtml=titlehtml(head,title)

if argd==1:
	ecriture(s,fichiersortie)
	print 'motsclefs()' 
motsclefs=motsclefs(head)

if argd==1:
	ecriture(s,fichiersortie)
	print 'br()'
corps=br(par(corps))
head=br(head)
entete=br(par(entete))
notes=br(par(notes))
maketitle=br(maketitle)

#---------------------------------------------------------------------#

if argd==1:
	ecriture(s,fichiersortie)
	print 'constructpage()'
s=constructpage(entete,maketitle,motsclefs,corps,toctwo,\
notes,thebibliography,argn,argp,argr,argb,argt)

if argd==1:
	ecriture(s,fichiersortie)
	print 'nonreconnues()'
s=nonreconnues(s,argc)

if argd==1:
	ecriture(s,fichiersortie)
	print 'nettoyage()'
s=nettoyage(s)

if argd==1:
	ecriture(s,fichiersortie)
	print 'desechappement()'
s=desechappement(s)

if argd==1:
	ecriture(s,fichiersortie)
	print 'verbatimbis()'
s=verbatimbis(s,nbverbatim,verbatimlist,nbverb,verblist)

#---------------------------------------------------------------------#

if argd==1:
	print 'ecriture()'
ecriture(s,fichiersortie)
