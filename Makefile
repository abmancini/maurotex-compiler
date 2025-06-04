INSTDIR	= /usr/local/bin

clean:
	make -C m2x clean
	make -C test  clean
	make -C support clean
	rm -f *~ core

install:
	cp -f *.sh *.py $(INSTDIR)

