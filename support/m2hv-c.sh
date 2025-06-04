#!/bin/sh
#m2hv-c alpha test
#TODO: resource saving - zip/unzip after/before writing/opening generated files
# +-------------------------------+
# | String manipulating functions |
# +-------------------------------+
getAllTheCharactersAfterTheLast1in2()
{ #prints the sequence of chars found after the last occurrence of $1 in $2
echo $2 | sed "s|.*$1||"
}
cutAllTheCharactersAfterTheLast1in2()
{ #prints the sequence of chars found before the last occurrence of $1 in $2
echo $2 | sed "s|\(.*\)$1.*|\1|"
}
cutAllTheCharactersEquivalentTo1in2()
{ #prints the sequence of chars found removing any occurrence of $1 in $2
echo $2 | sed "s/$1//g"
}
primaDecinadifilesalmassimoOpzioneV()
{ #lists this folder's filenames ending with "-" followed by a 1-digit number and "v.htm"
ls | grep "[-][[:digit:]]\{1\}v.htm"
}
primaDecinadifilesalmassimoNOpzione()
{ #lists this folder's filenames ending with "-" followed by a 1-digit number and ".htm"
ls | grep "[-][[:digit:]]\{1\}.htm"
}
sIfYouUseThisFunctionWithAnArgument()
{ # echo this function's name if it was called with a non-zero argument.
if [ $1 -ne 0 ] ; then echo "s" ; fi #TODO: generalize "s" with "this.$1"
}
#TODO: exception handling
# +-------------------------------------------+
# | Mtex microservices using zero/one Helper. |
# +-------------------------------------------+
c()
{ #go in test folder to make clean quietly; then echo a message
cd ../test ; make clean > /dev/null 2>&1 ; echo test files have been cleaned
}
r()
{ #software run on c's arguments list, with zero echos.
cd ../m2x; ./m2hv -c "$1" "$2" "../test/$3" > /dev/null 2>&1 ; echo \'m2hv\ -c\'\ ran\ on\ $#\ argument$(sIfYouUseThisFunctionWithAnArgument $#)
}
l()
{ #list test folder d contents (first 2)
cd ../test ; ls -lth *"-d.htm" | head -2
}
v()
{ #list test folder v contents
cd ../test ; ls -a *"-v[1-9].htm" | sort
}
g()
{ #open test program's output (after replacing h2 with p) cd ../test ;
sed -i 's/h2/p/g' | gopen "$(cutAllTheCharactersAfterTheLast1in2 "\." $0).test.htm"
}
# +------------------------------------------+
# | Helper function on some conditional echo |
# +------------------------------------------+

# +-------------------------------------------+
# | Parameterized Main function on n-ary swap |
# +-------------------------------------------+
m()
{ # echo line-by-line differences among arguments
echo "         $(date +"%Y-%m-%d %T")"
r $1 $2 $3
r $2 $1 $3
l
cd ../test ;
uno=$(cutAllTheCharactersEquivalentTo1in2 "," $1)
due=$(cutAllTheCharactersEquivalentTo1in2 "," $2) 
#TODO: control width according to max{text1.longest(line),text2.longest(line)}
text1=$3-$uno-$due-d.htm
text2=$3-$due-$uno-d.htm
sort $text1 > text1.sorted
sort $text2 > text2.sorted
diff --width=$4 --minimal --side-by-side --suppress-common-lines text1.sorted text2.sorted
rm text1.sorted
rm text2.sorted
rm "$text2"
echo "../test/$text1"
}
# +---------+
# | Main UI |
# +---------+
# initialization (optional args)
A=A;       [ $# -gt 0 ] && A=$1
B=B;       [ $# -gt 1 ] && B=$2
f=maurpro; [ $# -gt 2 ] && C=$3
w=210;     [ $# -gt 3 ] && f=$4
C="base";  [ $# -gt 4 ] && w=$5
# cleaning
c
# Debug view: messages in the terminal (echos from functions)
m $A $B $f $w
m $B $C $f $w
m $C $A $f $w
m $A $B,$C $f $w
m $B $C,$A $f $w
m $C $A,$B $f $w
# DevOp view: m2hv-c.test.htm browsing (14'' screen optimized)
echo "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Frameset//EN\" \"http://www.w3.org/TR/html4/frameset.dtd\">" > m2hv-c.test.htm
echo "<html>" >> m2hv-c.test.htm
echo "<head>" >> m2hv-c.test.htm
echo "<title>$A/$B/$C</title>" >> m2hv-c.test.htm
echo "<meta charset=\"utf-8\"/>" >> m2hv-c.test.htm
echo "<style>" >> m2hv-c.test.htm
echo "FRAMESET{background:Chartreuse;}" >> m2hv-c.test.htm
echo "FRAMESET:hover{background:white;}" >> m2hv-c.test.htm
echo "body > h2:nth-child(1){color:;}" >> m2hv-c.test.htm
echo "</style>" >> m2hv-c.test.htm
echo "</head>" >> m2hv-c.test.htm
echo "<FRAMESET ROWS=\"15%, 5%, 80%\" frameborder=\"0\">" >> m2hv-c.test.htm
echo "<FRAMESET COLS=\"10%, 70%, 10%, 10%\" NAME=\"mtext\">" >> m2hv-c.test.htm
for file in $(primaDecinadifilesalmassimoNOpzione); do #TODO: generalizzare i precedenti/seguenti proporzioni al caso in cui ne servano sino ad N (dove N è il numero dei testimoni)
filename=$(cutAllTheCharactersAfterTheLast1in2 "\." $file)
echo "<FRAME SRC=\"../test/$filename.htm\" NAME=\"$filename\">" >> m2hv-c.test.htm; done
echo "</FRAMESET>" >> m2hv-c.test.htm
echo "<FRAMESET COLS=\"10%, 70%, 10%, 10%\" NAME=\"folia\">>" >> m2hv-c.test.htm
for file in $(primaDecinadifilesalmassimoOpzioneV); do 
filename=$(cutAllTheCharactersAfterTheLast1in2 "\." $file)
echo "<FRAME SRC=\"../test/$filename.htm\" NAME=\"$filename\">" >> m2hv-c.test.htm; done
echo "</FRAMESET>" >> m2hv-c.test.htm

echo "<FRAMESET COLS=\"50%, 50%\" NAME=\"Concordanze\">" >> m2hv-c.test.htm
echo "<FRAMESET COLS=\"*, *, *\" NAME=\"discordanti\"  style=\"border:0px\">" >> m2hv-c.test.htm

echo "<FRAMESET ROWS=\"25%, *\" NAME=\"discordanti$A$B\">" >> m2hv-c.test.htm
echo "<FRAME SRC=\"$(m $A $B $f $w | tail -1)\" NAME=\"$A$B\">" >> m2hv-c.test.htm
echo "<FRAMESET ROWS=\"33%, 34%, 33%\" NAME=\"discordantiIncludendo$C\">" >> m2hv-c.test.htm
echo "<FRAME>" >> m2hv-c.test.htm
echo "<FRAME SRC=\"$(m $B $A,$C $f $w | tail -1)\" NAME=\"B_AC\">" >> m2hv-c.test.htm
echo "<FRAME SRC=\"$(m $A $B,$C $f $w | tail -1)\" NAME=\"A_BC\">" >> m2hv-c.test.htm
echo "</FRAMESET>" >> m2hv-c.test.htm
echo "</FRAMESET>" >> m2hv-c.test.htm

echo "<FRAMESET ROWS=\"25%, *\" NAME=\"discordanti$C$A\">" >> m2hv-c.test.htm
echo "<FRAME SRC=\"$(m $C $A $f $w | tail -1)\" NAME=\"$C$A\">" >> m2hv-c.test.htm
echo "<FRAMESET ROWS=\"33%, 34%, 33%\" NAME=\"discordantiIncludendo$B\">" >> m2hv-c.test.htm
echo "<FRAME SRC=\"$(m $C $A,$B $f $w | tail -1)\" NAME=\"$C_$A$B\">" >> m2hv-c.test.htm
echo "<FRAME>" >> m2hv-c.test.htm
echo "<FRAME SRC=\"$(m $A $B,$C $f $w | tail -1)\" NAME=\"$A_$B$C\">" >> m2hv-c.test.htm
echo "</FRAMESET>" >> m2hv-c.test.htm
echo "</FRAMESET>" >> m2hv-c.test.htm

echo "<FRAMESET ROWS=\"25%, *\" NAME=\"discordanti$B$C\">" >> m2hv-c.test.htm
echo "<FRAME SRC=\"$(m $B $C $f $w | tail -1)\" NAME=\">$B$C\">" >> m2hv-c.test.htm
echo "<FRAMESET ROWS=\"33%, 34%, 33%\" NAME=\"discordantiIncludendo$A\">" >> m2hv-c.test.htm
echo "<FRAME SRC=\"$(m $C $A,$B $f $w | tail -1)\" NAME=\"$C_$A$B\">" >> m2hv-c.test.htm
echo "<FRAME SRC=\"$(m $B $A,$C $f $w | tail -1)\" NAME=\"$B_$A$C\">" >> m2hv-c.test.htm
echo "<FRAME>" >> m2hv-c.test.htm
echo "</FRAMESET>" >> m2hv-c.test.htm
echo "</FRAMESET>" >> m2hv-c.test.htm

echo "</FRAMESET>" >> m2hv-c.test.htm
echo "<FRAMESET ROWS=\"50%, 20%, *\" NAME=\"DevOp\" style=\"background:grey\">" >> m2hv-c.test.htm
echo "<FRAME NAME=\"tc\">" >> m2hv-c.test.htm
echo "<FRAME NAME=\"variante\">" >> m2hv-c.test.htm
echo "<FRAME SRC=\"../test/\" NAME=\"filesystem\">" >> m2hv-c.test.htm
echo "</FRAMESET>" >> m2hv-c.test.htm
echo "</FRAMESET>" >> m2hv-c.test.htm
echo "</FRAMESET>" >> m2hv-c.test.htm
echo "</html>" >> m2hv-c.test.htm
g
