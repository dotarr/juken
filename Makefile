#
# FILE:
# Makefile
#

CFLAGS= -g -O2

.cpp.o:
	cc $(CFLAGS) -c $< 

.o:
	cc $(CFLAGS) $< -o $* libjuken.a

all: depend jukend main query test links

depend:
	makedepend -f - -- $(CFLAGS) -- *.h *.cpp >.depend

# library
libjuken.a: jukebox.o unixdomainsock.o util.o
	ar vru libjuken.a jukebox.o unixdomainsock.o util.o

# daemon
jukend: jukend.o serialdevice.o libjuken.a
	cc -g jukend.cpp -o jukend serialdevice.o libjuken.a

# toolbox
main: main.o libjuken.a

# testapp
query: query.o libjuken.a
test: test.o libjuken.a

# links
links: main
	ln -sf main handshake
	ln -sf main play
	ln -sf main pause
	ln -sf main stop
	ln -sf main nexttrack
	ln -sf main prevtrack
	ln -sf main besttracks
	ln -sf main disctitles
	ln -sf main disctracks
	ln -sf main tracktimes
	ln -sf main userfiles

# housekeeping
clean:
	rm -f core tmp junk *.o *.swp *.bak .depend

realclean: clean
	rm -f libjuken.a jukend main query test
	rm -f handshake play pause stop nexttrack prevtrack besttracks disctitles disctracks tracktimes userfiles

ifeq (.depend,$(wildcard .depend))
include .depend
endif
