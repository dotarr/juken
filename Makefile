#
# FILE:
# Makefile
#

CFLAGS= -g -O2
.SUFFIXES: .cpp

OBJS=   test.o \
	consolelistener.o \
	util.o \
	serialdevice.o \
	kenwooddevice.o \
	kenwoodchanger.o 

.cpp.o:
	cc $(CFLAGS) -c $< 

all: juken

juken: $(OBJS)
	cc $(OBJS) -o juken 

clean:
	rm -f core tmp junk *.o *.so *.swp *.bak .depend

realclean: clean
	rm -f juken 

