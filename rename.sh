#!/bin/bash
for dir in `ls`
do
	if [ -d $dir ]; then
		echo $dir
		cd $dir
		rm -f *~
		rm -f *.m.*
		rm -f #*#
		if [ `ls -1 | grep -c -F .tex` == 1 ]; then
			mv *.tex $dir.tex
		else
			echo "  -- " "non ci sono tex"
		fi
		if [ `ls -1 | grep -c -F .fp` == 1 ]; then
			mv *.fp $dir.fp
		else
			echo "  -- " "non ci sono fp"
		fi
		if [ `ls -1 2> /dev/null | grep -c -F f1.jpg` == 1 ]; then
			image=`ls *f1.jpg`
			imageprefix=`echo $image | sed s/f1.jpg//`
			for filename in `ls $imageprefix*.jpg`
			do
				suffix=`echo $filename | sed s/$imageprefix//`
				mv $filename $dir$suffix
			done
		else
			echo "  -- " "non ci sono jpg"
		fi
		cd ..
	fi
done

