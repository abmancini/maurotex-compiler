# -*- coding: iso-8859-1 -*-
# Jean-Pierre Sutto, 25 mars 2003, 9 août 2005
#
# routines utilisées par les scripts de gestion du site Maurolico 
# This is GPL free software


#----------------------------------------------------------#
def recup_titres_propositions(radical):

	# récupère la liste des propositions
	# à partir de la variable titres_propositions du fichier particulier

	fp=radical+'.fp'
	#print 'fp=',fp
	s=readfile(fp)
	liste=['titres_propositions']
	titres_propositions=readfp(s,liste)[0]
	#print 'mroutines:titres_propositions=',titres_propositions

	listeprop=readprops(titres_propositions)
	#print 'mroutines:listeprop=',listeprop

	return listeprop

#----------------------------------------------------------#
def readprops(props):

	# découpe props selon les '-' et les ','
	# et renvoie une liste de couples comprenant le titre d'une
	# proposition
	
	import string
	
	liste=[]

#	for s in string.split(props,','):
	for s in props.split(','):
		if string.find(s,'-')!=-1:
			petit,grand=string.split(s,'-')
			for h in range(int(petit),int(grand)+1):
				titre='Propositio'+' '+repr(h)
				liste.append(titre)
		else:
			liste.append(s)

	return liste

#----------------------------------------------------------#
def position_sommaire(radical):

	# trouve la position du sommaire
	
	import string
	import os
	
	fp=radical+'.fp'

	# récupération de la variable position_sommaire du fichier particulier
	s=readfile(fp)
	liste=['position_sommaire']
	position_sommaire=readfp(s,liste)[0]

	# sinon, on cherche www-source dans le path
	if position_sommaire == '@':
		path=os.path.abspath('./')
		c=string.count(path,'/',string.find(path,'www-source'))
		position_sommaire='../'*c

	return position_sommaire

#-----------------------------------------------------------#
def readfp(s,liste):

	# renvoie la liste des valeurs trouvées dans le fichier particulier
	# correspondant aux variables de la liste liste
	
	import string
	
	var=[]
	fin=len(s)
	
	for target in liste:
		pos_target=string.find(s,'\n'+target)
		if pos_target==-1:
			retour='@'
		else:
			pos_target=pos_target+1
			l_target=len(target)
			pos_egal=string.find(s,'=',pos_target+l_target)
			pos_rc=string.find(s,'\n',pos_egal)
			if pos_rc==-1: pos_rc=fin
			pos_diese=string.find(s,'#',pos_egal)
			if pos_diese==-1: pos_diese=fin
			retour=string.strip(s[pos_egal+1:min(pos_rc,pos_diese)])
		var.append(retour)

	return var

#---------------------------------------------------------------#
def date():

	# renvoie la date dans un format 10-02-99
	
	import time
	return time.strftime("%d-%m-%y",time.localtime(time.time()))

#---------------------------------------------------------------#
def savefile(s,file):

	f=open(file,'w')
	f.write(s)
	f.close()

	return

#---------------------------------------------------------------#
def readfile(file):

	f=open(file,'r')
	s=f.read()
	f.close()

	return s

