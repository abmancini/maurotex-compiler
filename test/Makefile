
all: maurpro.ps maurpro.m.pdf

clean:
	make -C m2ledmac clean
	rm -fr edition *.output *.htm* *.m.* *.rtf *.log core *~ mauro.*pk mauro.tfm *.ppm *.jpg *.ps *.pdf *.flc *.unit *.json 1* 2* 3* 4*

%.pdf: %.ps
	ps2pdf $<

%.ps: %.mp
	mpost $<

%.m.tex: %.tex
	m2lv $<

%.m.dvi: %.m.tex
	latex $<

%.m.ps: %.m.dvi
	dvips $<

%.htm: %.tex
	m2hv $<

.SUFFIXES:
