#
# FILE:
# Makefile
#

CFLAGS= -g -O2
.SUFFIXES: .cpp

OBJS=   jukend.o \
	consolelistener.o \
	util.o \
	serialdevice.o \
	kenwooddevice.o \
	kenwoodchanger.o 

.cpp.o:
	cc $(CFLAGS) -c $< 

all: jukend

jukend: $(OBJS)
	cc $(OBJS) -o jukend

clean:
	rm -f core *.core tmp junk *.o *.so *.swp *.bak .depend

realclean: clean
	rm -f jukend

