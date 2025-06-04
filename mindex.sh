#!/bin/bash
#
# Jean-Pierre Sutto, 18 mars 2003, 30 avril, 24 sept 2004, 14 avril 2006
#
# usage mindex.sh [radical]
#
# This is GPL free software

rad=$1

echo '------------- mindex -------------'

index='index.htm'

date=`date  +'%d-%m-%y'`

titres=`cat $rad.fp | grep titres_propositions | sed -e 's:titres_propositions *= *::g' | sed -e 's: *, *:,:g' | sed -e 's: :@:g' | sed -e 's:,: :g'`

tableau=( $titres )
nb=${#tableau[*]}

htmlcut=`cat $rad.tex | sed -e 's:%.\+:X:g' | grep -c '\\htmlcut'`

if test $htmlcut == 1
then
	echo "un seul htmlcut, pas d'index."
	echo "ok"
	exit 0
fi

if test $nb != $( expr $htmlcut - 1)
then
	echo "aie: ll y a $nb titres et $htmlcut htmlcut"
fi

echo '<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<title>index</title>
<!-- Jean-Pierre Sutto' $date '-->
<!-- Created with m2hv, mtex, python, LaTeX and linux. -->
</head>
<body bgcolor=white>
<table width="100%" cellpadding=1>
<tr>
<td bgcolor=peru align=center valign=middle>
<font color="white">Index</font>
</td>
</tr>
</table>
<br>
<center>
<font size=-1>' > $index

k=0
for titre in ${tableau[*]}
do
	k=$( expr $k + 1)
	titre=`echo $titre | sed -e 's:@: :g'`
	#echo $k,$titre
	echo '<a href="'$rad'-'$k'f.htm" target="_parent">'$titre'</a><br>'>> $index
done

echo '</font>
</center>
</body>
</html>' >> $index

echo 'Ok.'


