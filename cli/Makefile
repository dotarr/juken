#
# FILE:
# Makefile
#

CFLAGS= -g -O2
.SUFFIXES: .cpp

OBJS=   juken.o
LIBS=   ../lib/juken.a

.cpp.o:
	cc $(CFLAGS) -c $< 

all: juken-cli

../lib/juken.a: 
	make -C ../lib

juken-cli: $(OBJS) $(LIBS)
	cc $(OBJS) $(LIBS) -o ../juken-cli

clean:
	rm -f core *.core tmp junk *.o *.so *.swp *.bak .depend

realclean: clean




#	for i in $(SUBDIRS) ;\
#	do \
#	echo "building" all "in $$i..."; \
#	( \
#		cd $$i ; \
#		make \
#	); \
#	done
#	@echo Finished!


