#!/bin/bash
#test[m2m]{Linguaggio2linguaggio}
clear;
echo "trova"
query=${1:-LEC}
x=$query;
y=$query;
z=${query}KEY;
read -p "nel file da tradurre        (default: $x): " X;
X=${X:-$x};
read -p "nel file tradotto           (default: $y): " Y;
Y=${Y:-$y};
read -p "nell'analizzatore lessicale (default: $z): " Z;
Z=${Z:-$z};
S=---------------------------------------------;
O=maurpru;
a=m2m;
b=test;
cd ..;
ab=$PWD;
ba=mparse.*\ mparsey.h\ m2m.c;
A=$ab/$a;
B=$ab/$b;
N=${2:-0}
echo "$S$PWD  | pulizia";
cd $B;
rm $O.二.tex;
cd $A;
echo "$S$PWD  | inizializzazione";
echo "ricorda"
grep -m 1 makeTesto mmerge.c -A 5;
make linux;
cd $B;
echo "$S$PWD input";
ls $O.tex;
echo "$S$PWD | esecuzione";
../$a/$a $O.tex;
echo "$S$PWD output";
ls $O.二.tex;
cd $A;
echo "$S$PWD  lexis       --> $Z";
grep -nw --color=always "$Z" $ba    -A $N -B $N | grep -v "%";
cd $B;
echo "$S$PWD input       --> $X";
grep -nw --color=always "$X" $O.tex   -A $N -B $N | grep -v "%";
echo "$S$PWD output 二   --> $Y";
grep -nw --color=always "$Y" $O.二.tex -A $N -B $N | grep -v "%";
