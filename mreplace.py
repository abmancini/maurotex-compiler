#! /usr/bin/env python
# -*- coding: iso-8859-1 -*-
#
# jps le 20 decembre 2006
#
# remplace.py [-r] find replace file
#

import sys
import string
import re


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

#---------------------------------------------------------------#

arg1=sys.argv[1]

if arg1=="-r":
	find=sys.argv[2]
	replace=sys.argv[3]
	myfile=sys.argv[4]
else:
	find=arg1
	replace=sys.argv[2]
	myfile=sys.argv[3]

print "find=", find
print "replace=", replace
print "myfile=", myfile

s=readfile(myfile)

if arg1=="-r":
	s=re.sub(find,replace,s)
else:
	s=s.replace(find,replace)

savefile(s,myfile)
