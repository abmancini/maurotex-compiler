#!/bin/bash
#
# Jean-Pierre Sutto, 18 mars 2003
#
# UgoAL
#
# usage moutils.sh [radical]
#
# This is GPL free software

rad=$1

echo '------------- micons -------------'

position_sommaire=`cat $rad.fp | grep position_sommaire | sed -e 's:position_sommaire *= *::g'`

if test w$position_sommaire == w
then
	position_sommaire=`pwd | sed -e 's:.\+/www-source::g' | sed -e 's:[^/]\+:..:g' | sed -e 's:^/::g'`'/'
fi

echo position_sommaire=$position_sommaire

target='src="micons/'

boc='src="'$position_sommaire'micons/'

trova=`find . -regex ^./$rad.*[0-9].htm`

for i in $trova; do
	mreplace.py $target $boc  $i 
done

echo 'Ok.'


