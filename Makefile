#
# FILE:
# Makefile
#

CFLAGS= -g -O2

.cpp.o:
	cc $(CFLAGS) -c $< 

.o:
	cc $(CFLAGS) $< -o $* libjuken.a

all: depend jukend handshake query userfiles disctitles disctracks besttracks tracktimes playpause stop nexttrack

depend:
	makedepend -f - -- $(CFLAGS) -- *.h *.cpp >.depend

# library
libjuken.a: jukebox.o unixdomainsock.o util.o
	ar vru libjuken.a jukebox.o unixdomainsock.o util.o

# daemon
jukend: jukend.o serialdevice.o libjuken.a
	cc -g jukend.cpp -o jukend serialdevice.o libjuken.a

# tools
handshake: handshake.o libjuken.a
query: query.o libjuken.a
userfiles: userfiles.o libjuken.a
disctitles: disctitles.o libjuken.a
disctracks: disctracks.o libjuken.a
besttracks: besttracks.o libjuken.a
tracktimes: tracktimes.o libjuken.a
playpause: playpause.o libjuken.a
stop: stop.o libjuken.a
nexttrack: nexttrack.o libjuken.a

# housekeeping
clean:
	rm -f core tmp junk *.o *.swp *.bak .depend

realclean: clean
	rm -f libjuken.a jukend handshake query userfiles disctitles disctracks besttracks tracktimes playpause stop nexttrack

ifeq (.depend,$(wildcard .depend))
include .depend
endif
