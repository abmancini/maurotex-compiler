WC	= i586-mingw32msvc-gcc
WCPP	= i586-mingw32msvc-c++
WLFLAGS	=
CFLAGS	= -Wall -O2 -DYY_NO_UNPUT -Werror -Wno-deprecated-declarations -Wno-unused-but-set-variable -Wno-unneeded-internal-declaration -Wno-memsize-comparison
LFLAGS	= -lfl

INSTDIR	= /usr/local/bin

CTARGETS = m2web m2hv m2lv m2ledmac m2m m2xml m2tei m2unit mmerge
TARGETS	= $(CTARGETS) afronte
WTARGETS= m2hv.exe m2lv.exe m2ledmac.exe m2m.exe m2xml.exe m2unit.exe mmerge.exe

# REGOLE AD ALTO LIVELLO




default:
	@echo please select target: linux win osx

all: linux win

linux: $(TARGETS)

win: $(WTARGETS)

osx: LFLAGS = -ll 

osx: $(TARGETS)

afronte:

install: linux
	cp -f $(TARGETS) $(INSTDIR)

execute: mmerge.m.dvi

prova%: prova%-a.tex prova%-b.tex
	mmerge $^

clean:
	rm -fr mparsef.cpp mparsey.cpp mparsey.hpp *.o *.w $(CTARGETS) $(WTARGETS) *.output *.htm* *.m.* *.rtf *.log core *~ mauro.*pk mauro.tfm *.ppm *.jpg *.ps *.pdf mmerge.tex *.flc *.unit *.json 1.* 2.* 3.* 4.*

mmerge.tex: mmerge1.tex mmerge2.tex mmerge
	./mmerge mmerge1.tex mmerge2.tex

# VERSIONE LINUX

mmerge: mmerge.o mparsef.o mparsey.o
	c++ $^ $(LFLAGS) -o $@

m2web: m2web.o mparsef.o mparsey.o mvisit.o
	c++ $^ $(LFLAGS) -o $@

m2hv: m2hv.o mparsef.o mparsey.o mvisit.o
	c++ $^ $(LFLAGS) -o $@

m2cgr: m2cgr.o mparsef.o mparsey.o mvisit.o
	c++ $^ $(LFLAGS) -o $@

m2lv: m2lv.o mparsef.o mparsey.o mvisit.o plugin.o standard.o epistolario.o
	c++ $^ $(LFLAGS) -o $@

m2ledmac: m2ledmac.o mparsef.o mparsey.o mvisit.o plugin.o standard.o epistolario.o
	c++ $^ $(LFLAGS) -o $@

m2m: m2m.o mparsef.o mparsey.o mvisit.o plugin.o standard.o epistolario.o
	c++ $^ $(LFLAGS) -o $@

m2xml: m2xml.o mparsef.o mparsey.o mvisit.o
	c++ $^ $(LFLAGS) -o $@

m2tei: m2tei.o mparsef.o mparsey.o mvisit.o
	c++ $^ $(LFLAGS) -o $@

m2unit: m2unit.o mparsef.o mparsey.o mvisit.o
	c++ $^ $(LFLAGS) -o $@

m2rtf: m2rtf.o mparsef.o mparsey.o mvisit.o
	c++ $^ $(LFLAGS) -o $@

# VERSIONE WINDOWS

mmerge.exe: mmerge.w yywrap.w mparsef.w mparsey.w
	$(WCPP) $^ $(WLFLAGS) -o $@

m2web.exe: m2web.w yywrap.w mparsef.w mparsey.w mvisit.w
	$(WCPP) $^ $(WLFLAGS) -o $@

m2hv.exe: m2hv.w yywrap.w mparsef.w mparsey.w mvisit.w
	$(WCPP) $^ $(WLFLAGS) -o $@

m2lv.exe: m2lv.w yywrap.w mparsef.w mparsey.w mvisit.w standard.w epistolario.w
	$(WCPP) $^ $(WLFLAGS) -o $@

m2ledmac.exe: m2ledmac.w yywrap.w mparsef.w mparsey.w mvisit.w standard.w epistolario.w
	$(WCPP) $^ $(WLFLAGS) -o $@

m2m.exe: m2m.w yywrap.w mparsef.w mparsey.w mvisit.w standard.w epistolario.w
	$(WCPP) $^ $(WLFLAGS) -o $@

m2xml.exe: m2xml.w yywrap.w mparsef.w mparsey.w mvisit.w
	$(WCPP) $^ $(WLFLAGS) -o $@

m2unit.exe: m2unit.w yywrap.w mparsef.w mparsey.w mvisit.w
	$(WCPP) $^ $(WLFLAGS) -o $@

m2rtf.exe: m2rtf.w yywrap.w mparsef.w mparsey.w mvisit.w
	$(WCPP) $^ $(WLFLAGS) -o $@


# REGOLE INDIPENDENTI DALLA PIATTAFORMA TARGET

%y.cpp %y.hpp %y.h: %.y
	bison -v -d -o $*y.cpp $<

%f.cpp: %.f
	flex -o$@ $<

%.jpg: %.ppm
	ppmtojpeg $< > $@

%.ppm: %.ps
	pstopnm -stdout -portrait $< > $@

%.ps: %.mp
	mpost $<

%.m.tex: %.tex
	m2lv $<

%.m.dvi: %.m.tex
	latex $<

%.htm: %.tex
	m2hv $<

# REGOLE DIPENDENTI DALLA PIATTAFORMA TARGET

mvisit.o: mvisit.cpp mvisit.h mparse.h mparsein.h mparsey.hpp comp.h
	c++ -c $(CFLAGS) $< -o $@

mvisit.w: mvisit.cpp mvisit.h mparse.h mparsein.h mparsey.hpp comp.h
	$(WCPP) -c $(CFLAGS) $< -o $@

yywrap.w: yywrap.cpp
	$(WC) -c $(CFLAGS) $< -o $@

#%.o: %.c mparse.h mparsein.h mparsey.hpp mvisit.h comp.h plugin.h standard.h epistolario.h
#	c++ -c $(CFLAGS) $< -o $@

%.o: %.cpp mparse.h mparsein.h mparsey.hpp mvisit.h comp.h plugin.h standard.h epistolario.h
	c++ -c $(CFLAGS) $< -o $@

%.w: %.cpp mparse.h mparsein.h mparsey.hpp mvisit.h comp.h
	$(WCPP) -c $(CFLAGS) $< -o $@

.SUFFIXES:
