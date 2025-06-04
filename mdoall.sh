#!/bin/bash
#
# Jean-Pierre Sutto, 25 mars, 29 septembre 2003, 18 mars 2004,
# 14, 17 avril 2006
#
# automatise la gestion du site
# usage mdoall.sh [radical]
# This is GPL free software

# argument ?
if [[ w$1 == w ]]
then
	# si aucun argument, on prend le premier fichier .tex du répertoire
	dir=`ls *.tex | sort -r | tail -n1`
	radical=`basename $dir .tex`
	echo $radical.tex
else
	radical=$1
fi

# on fait un peu de nettoyage
mkdir -p old
mv -f $radical*.htm old/ 2>/dev/null

# suite de programmes
m2hv -p $radical > /dev/null
micons.sh $radical > /dev/null
mindex.sh $radical > /dev/null
mframe.sh $radical > /dev/null
menttc.py $radical > /dev/null
mentvar.py $radical > /dev/null
mentadn.py $radical > /dev/null
mlinks.py $radical > /dev/null
#mentcit.py $radical

# on fait encore un peu de nettoyage final
mv -f $radical???.tex old/ 2>/dev/null

# si un script shell mplushtml.sh existe dans le répertoire, on l'execute
# utile pour quelques modifs
if test -f 'mplushtml.sh'
then
	sh 'mplushtml.sh'
fi
