#
# FILE:
# Makefile
#

CFLAGS= -g -O2
.SUFFIXES: .cpp

OBJS= \
	util.o \
	serialdevice.o \
	kenwooddevice.o \
	kenwoodchanger.o 

.cpp.o:
	cc $(CFLAGS) -c $< 

all: juken.a

juken.a: $(OBJS)
	ar -cr juken.a $(OBJS)

clean:
	rm -f core *.core tmp junk *.o *.so *.swp *.bak .depend

realclean: clean
	rm -f juken.a

