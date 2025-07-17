INSTDIR	= /usr/bin

all:
	make -C m2x
	make -C test 
	make -C support

clean:
	make -C m2x clean
	make -C test  clean
	make -C support clean
	rm -f *~ core

##l' install e' gestito dal pacchetto debian
#install:
#	cp -f *.sh *.py $(INSTDIR)

