#include <stdio.h>
#include "consolelistener.h"

void
ConsoleListener::InfoChanged(short slot, byte track, enum mode mode, 
                             enum random random, bool repeat, 
                             byte userfile)
{
    //display the info event
    ::fprintf(stderr, "disc#: %d track: %d", slot, track);
    ::fprintf(stderr, " mode: %s", MODE_NAMES[mode]);
    switch ( mode )
    {
        case UserfileMode:
            ::fprintf(stderr, "(0x%02X)", userfile);
            break;
        //case MusicTypeMode:
        //    ::fprintf(stderr, "(%s)", GENRE_NAMES[genre]);
        //    break;
    }

    if ( random!=RandomOff || repeat )
    {
        ::fprintf(stderr, " (");
        if ( random != RandomOff ) 
        {
            ::fprintf(stderr, "%s", RANDOM_NAMES[random]);
            if ( repeat )
                ::fprintf(stderr, ",");
        }
        if ( repeat )
            ::fprintf(stderr, "repeat");
        ::fprintf(stderr, ")");
    }
    ::fprintf(stderr, "\n");
}

void
ConsoleListener::StateChanged(enum state state)
{
    // display the current state
    ::fprintf(stderr, "state: %s\n", STATE_NAMES[state]);
}

void
ConsoleListener::DiscChanged(short slot)
{
    // display the current disc number
    ::fprintf(stderr, "disc#: %d\n", slot);
}

void
ConsoleListener::DoorChanged(bool door_closed)
{
    // display the door state
    ::fprintf(stderr, "door: %s\n", door_closed?"closed":"open");
}
