#include "logginglistener.h"
#include "kenwoodchanger.h"

void
LoggingListener::InfoChanged(KenwoodChanger* changer, short slot, byte title, short chapter) 
{
    //display the info event
    if ( chapter == 0 )
        LogMsg("disc: %d track: %d\n", slot, title);
    else
        LogMsg("disc: %d title: %d chapter: %d\n", slot, title, chapter);
}

void
LoggingListener::ModeChanged(KenwoodChanger* changer, enum mode mode, bool repeat, byte param)
{
    LogMsg("mode: %s", MODE_NAMES[mode]);

    switch ( mode )
    {
        case ProgramMode:
            LogMsg(" (%d)", param);
        break;
        case UserfileMode:
        case UserfileModeRandomOne:
        case UserfileModeRandomAll:
        {
            byte i = 0;
            byte uf = param;
            while ( uf != 1 ) { uf = uf>>1; i++; }
            LogMsg(" (0x%02X)", changer->getUserfileName(i));
        }
        break;
        case MusicTypeMode:
        case MusicTypeModeRandomAll:
            LogMsg(" (%s)", GENRE_NAMES[param]);
        break;
    }

    if ( repeat )
        LogMsg(" (repeat)");

    LogMsg("\n");
}

void
LoggingListener::StateChanged(KenwoodChanger* changer, enum state state)
{
    // display the current state
    switch ( state )
    {
        case Stopped:      LogMsg("state: stopped\n"); break;
        case Standby:      LogMsg("state: standby\n"); break;
        case Stopping:     LogMsg("state: stopping\n"); break;
        case Changing:     LogMsg("state: changing\n"); break;
        case Playing:      LogMsg("state: playing\n"); break;
        case Paused:       LogMsg("state: paused\n"); break;
        case SkipForward:  LogMsg("state: skip forward\n"); break;
        case SkipBackward: LogMsg("state: skip backward\n"); break;
        default:           LogMsg("state: UNKNOWN\n"); break;
    }
}

void
LoggingListener::DoorChanged(KenwoodChanger* changer, bool door_open)
{
    // display the door state
    LogMsg("door: %s\n", door_open?"open":"closed");
}

const char* LoggingListener::scanning_discs_str = 
                            "scanning discs ...      ";
const char* LoggingListener::loading_userfiles_str = 
                            "loading userfile names ...    ";

void
LoggingListener::ProgressStart(KenwoodChanger* changer, enum operation op, int length)
{
    switch ( op )
    {
        case ScanDiscs:
            LogMsg("%s\n", scanning_discs_str);
        break;
        case LoadUserfiles:
            LogMsg("%s\n", loading_userfiles_str);
        break;
        default:
        break;
    }
}

void
LoggingListener::Progress(KenwoodChanger* changer, enum operation op, int progress)
{
    switch ( op )
    {
        case ScanDiscs:
            LogMsg("[%3d]\n", progress);
        break;
        case LoadUserfiles:
            LogMsg("[%1d]\n", progress);
        break;
        default:
        break;
    }
}

void
LoggingListener::ProgressEnd(KenwoodChanger* changer, enum operation op)
{
    switch ( op )
    {
        case ScanDiscs:
            LogMsg("complete\n");
        break;
        case LoadUserfiles:
            LogMsg("complete\n");
        break;
        default:
        break;
    }
}

