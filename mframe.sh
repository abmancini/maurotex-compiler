#!/bin/bash
#
# Jean-Pierre Sutto, 1 avril, 12 juin, 5 nov 2003, 16, 17, 18 mars 2003
# 24 septembre 2004, 7 avril 2005, 14 avril 2006
#
# usage mframe.sh [radical]
# script PROVISOIRE pour gérer la génération des frames
#
# This is GPL free software

rad=$1

echo '------------- mframe -------------'

# élimination PROVISOIRE de tous les fichiers de frames préexistants
rm -f $rad-*f.htm

# élimination PROVISOIRE de tous les fichiers de citations
rm -f $rad-*c.htm

# élimination PROVISOIRE de tous les fichiers d'index
rm -f $rad-*ind.htm $rad-*indf.htm

#index=`ls | grep -c index\.htm`
#echo 'index='$index

VV=`cat $rad.tex | sed -e 's:%.\+:X:g' | grep -c '\\\\VV\|\\\\Sex'`
#echo 'VV='$VV
#XXX attention, il y a un bug!
#Si le fichier ne contient que des macros de simplifications...
if [ $VV == 0 ]
then
	echo "j'efface les variantes"
	rm -f $rad-*v.htm
fi

adn=`cat $rad.tex | sed -e 's:%.\+:X:g' | grep -c '\\Adnotatio'`
#echo 'adn='$adn
if [ $adn == 0 ]
then
	echo "j'efface les adnotatio"
	rm -f $rad-*a.htm
fi

htmlcut=`cat $rad.tex | sed -e 's:%.\+:X:g' | grep -c '\\htmlcut'`
#echo 'htmlcut='$htmlcut

liste=`ls $rad-*[^acfv]\.htm`
echo 'liste='$liste

if [[ $VV == 0 ]] && [[ $adn == 0 ]] &&  [[ $htmlcut == 1 ]] 
then
	echo "ni variante ni note ni htmlcut: je ne crée pas de frame"
	exit 0
fi

echo 'je crée les frames'

for file in $liste
do
	long=`echo $file | sed -e 's:\.htm::g'`
	#echo 'long='$long

	frame=$long'f.htm'
	#echo 'frame='$frame

	echo '<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Frameset//EN">' > $frame
	echo '<html>' >> $frame
	echo '<head>' >> $frame
	echo '<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">' >> $frame
	echo '<title>'$long'</title>' >> $frame
	echo '</head>' >> $frame
	echo >> $frame

	if [ $htmlcut != 1 ] && [ $VV != 0 ]
	then
		echo '<frameset cols="90,*,25%">' >> $frame
		echo '	<frame src="index.htm" name="index">' >> $frame
	elif [ $htmlcut != 1 ] && [ $adn != 0 ]
	then
		echo '<frameset cols="90,*,25%">' >> $frame
		echo '	<frame src="index.htm" name="index">' >> $frame
	elif [ $htmlcut != 1 ] && [ $adn == 0 ] && [ $VV == 0 ]
	then
		echo '<frameset cols="90,*">' >> $frame
		echo '	<frame src="index.htm" name="index">' >> $frame
	elif [ $htmlcut == 1 ] && [ $VV != 0 ] && [ $adn == 0 ]
	then
		echo '<frameset cols="*,25%">' >> $frame
	elif [ $htmlcut == 1 ] && [ $adn != 0 ] && [ $VV == 0 ]
	then
		echo '<frameset cols="*,25%">' >> $frame
	else
		#echo 'je ne crée pas de frame d index'
		echo '<frameset cols="*,25%">' >> $frame
	fi

	echo '  <frame src="'$long'.htm" name="tc">' >> $frame

	if [ $VV != 0 ] && [ $adn != 0 ]
	then
		#echo 'je crée les frames de variante et de adn'
		echo '	<frameset rows="*,30%">' >> $frame
		echo '		<frame src="'$long'v.htm" name="variante">' >> $frame
		echo '		<frame src="'$long'a.htm" name="adnotatio">' >> $frame
		echo '	</frameset>' >> $frame
	fi

	if [ $VV != 0 ] && [ $adn == 0 ]
	then
		#echo 'je crée la frame de variante'
		echo '	<frame src="'$long'v.htm" name="variante">' >> $frame
	fi

	if [ $VV == 0 ] && [ $adn != 0 ]
	then
		#echo 'je crée la frame de adn'
		echo '	<frame src="'$long'a.htm" name="adnotatio">' >> $frame
	fi

	echo '</frameset>' >> $frame
	echo >> $frame
	echo '</html>' >> $frame

done

echo 'Ok.'
