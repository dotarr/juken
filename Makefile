#
# FILE:
# Makefile
#

CFLAGS= -g -O2
.SUFFIXES: .cpp

OBJS=   jukend.o
LIBS=   lib/juken.a

.cpp.o:
	cc $(CFLAGS) -c $< 

SUBDIRS = lib

all: jukend

lib/juken.a: 
	make -C lib

jukend: $(OBJS) $(LIBS)
	cc $(OBJS) $(LIBS) -o jukend

clean:
	make -C lib clean
	rm -f core *.core tmp junk *.o *.so *.swp *.bak .depend

realclean: clean
	make -C lib realclean
	rm -f jukend




#	for i in $(SUBDIRS) ;\
#	do \
#	echo "building" all "in $$i..."; \
#	( \
#		cd $$i ; \
#		make \
#	); \
#	done
#	@echo Finished!


