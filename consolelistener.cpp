#include <stdio.h>
#include "consolelistener.h"

void
ConsoleListener::InfoChanged(short slot, byte track, byte num_tracks, 
                    byte mode, byte userfiles, byte userfile_mode)
{
    //display the info event
    ::fprintf(stderr, "disc#: %d track: %d (of%d)\n",
             slot, track, num_tracks);
    ::fprintf(stderr, "\tmode: %s\n", MODE_NAMES[mode]);
    ::fprintf(stderr, "\tuserfiles: %02X\n", userfiles);
    ::fprintf(stderr, "\tuserfile_mode: %02X\n", userfile_mode);
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
