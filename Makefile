#
# FILE:
# Makefile
#

CFLAGS= -g -O2

.cpp.o:
	cc $(CFLAGS) -c $< 

all: depend test 

depend:
	makedepend -f - -- $(CFLAGS) -- *.h *.cpp >.depend

# testapp
test: test.o consolelistener.o util.o serialdevice.o kenwooddevice.o kenwoodchanger.o

# housekeeping
clean:
	rm -f core tmp junk *.o *.swp *.bak .depend

realclean: clean
	rm -f test

ifeq (.depend,$(wildcard .depend))
include .depend
endif
