#
# FILE:
# Makefile
#
# FUNCTION:
# Makefile for serial port utility

CFLAGS= -g -O2

.cpp.o:
	cc $(CFLAGS) -c $< 

.o:
	cc $(CFLAGS) $< -o $* libjuken.a

all: depend jukend handshake query queryuserfiles querydisctitles querydisctracks playpause 

depend:
	makedepend -f - -- $(CFLAGS) -- *.h *.cpp >.depend

libjuken.a: jukebox.o unixdomainsock.o util.o
	ar vru libjuken.a jukebox.o unixdomainsock.o util.o

jukend: jukend.o serialdevice.o libjuken.a
	cc -g jukend.cpp -o jukend serialdevice.o libjuken.a

handshake: handshake.o libjuken.a
query: query.o libjuken.a
queryuserfiles: queryuserfiles.o libjuken.a
querydisctitles: querydisctitles.o libjuken.a
querydisctracks: querydisctracks.o libjuken.a
playpause: playpause.o libjuken.a

clean:
	rm -f core tmp junk *.o *.swp *.bak .depend


realclean: clean
	rm -f libjuken.a jukend handshake query queryuserfiles querydisctitles querydisctracks playpause

ifeq (.depend,$(wildcard .depend))
include .depend
endif
