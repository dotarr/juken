#include "consolelistener.h"
#include "kenwoodchanger.h"

void
ConsoleListener::InfoChanged(KenwoodChanger* changer, short slot, byte title, short chapter) 
{
    //display the info event
    if ( chapter == 0 )
        ::fprintf(m_file, "disc: %d track: %d\n", slot, title);
    else
        ::fprintf(m_file, "disc: %d title: %d chapter: %d\n", slot, title, chapter);
}

void
ConsoleListener::ModeChanged(KenwoodChanger* changer, enum mode mode, bool repeat, byte param)
{
    ::fprintf(m_file, "mode: %s", MODE_NAMES[mode]);

    switch ( mode )
    {
        case ProgramMode:
            ::fprintf(m_file, " (%d)", param);
        break;
        case UserfileMode:
        case UserfileModeRandomOne:
        case UserfileModeRandomAll:
        {
            byte i = 0;
            byte uf = param;
            while ( uf != 1 ) { uf = uf>>1; i++; }
            ::fprintf(m_file, " (0x%02X)", changer->getUserfileName(i));
        }
        break;
        case MusicTypeMode:
        case MusicTypeModeRandomAll:
            ::fprintf(m_file, " (%s)", GENRE_NAMES[param]);
        break;
    }

    if ( repeat )
        ::fprintf(m_file, " (repeat)");

    ::fprintf(m_file, "\n");
}

void
ConsoleListener::StateChanged(KenwoodChanger* changer, enum state state)
{
    // display the current state
    switch ( state )
    {
        case Stopped:      ::fprintf(m_file, "state: stopped\n"); break;
        case Standby:      ::fprintf(m_file, "state: standby\n"); break;
        case Stopping:     ::fprintf(m_file, "state: stopping\n"); break;
        case Changing:     ::fprintf(m_file, "state: changing\n"); break;
        case Playing:      ::fprintf(m_file, "state: playing\n"); break;
        case Paused:       ::fprintf(m_file, "state: paused\n"); break;
        case SkipForward:  ::fprintf(m_file, "state: skip forward\n"); break;
        case SkipBackward: ::fprintf(m_file, "state: skip backward\n"); break;
        default:           ::fprintf(m_file, "state: UNKNOWN\n"); break;
    }
}

void
ConsoleListener::DoorChanged(KenwoodChanger* changer, bool door_open)
{
    // display the door state
    ::fprintf(m_file, "door: %s\n", door_open?"open":"closed");
}

const char* ConsoleListener::scanning_discs_str = 
                            "scanning discs ...      ";
const char* ConsoleListener::loading_userfiles_str = 
                            "loading userfile names ...    ";

void
ConsoleListener::ProgressStart(KenwoodChanger* changer, enum operation op, int length)
{
    switch ( op )
    {
        case ScanDiscs:
            ::fprintf(m_file, "%s", scanning_discs_str);
            ::fflush(m_file);
        break;
        case LoadUserfiles:
            ::fprintf(m_file, "%s", loading_userfiles_str);
            ::fflush(m_file);
        break;
        default:
        break;
    }
}

void
ConsoleListener::Progress(KenwoodChanger* changer, enum operation op, int progress)
{
    switch ( op )
    {
        case ScanDiscs:
            back_space(5);
            ::fprintf(m_file, "[%3d]", progress);
            ::fflush(m_file);
        break;
        case LoadUserfiles:
            back_space(3);
            ::fprintf(m_file, "[%1d]", progress);
            ::fflush(m_file);
        break;
        default:
        break;
    }
}

void
ConsoleListener::ProgressEnd(KenwoodChanger* changer, enum operation op)
{
    switch ( op )
    {
        case ScanDiscs:
            back_space(::strlen(scanning_discs_str));
            //back_space(5);
            //::fprintf(m_file, "complete\n");
            ::fflush(m_file);
        break;
        case LoadUserfiles:
            back_space(::strlen(loading_userfiles_str));
            //back_space(3);
            //::fprintf(m_file, "complete\n");
            ::fflush(m_file);
        break;
        default:
        break;
    }
}

void
ConsoleListener::back_space(int i)
{
    while ( i > 0 )
    {
        ::fprintf(m_file, "%c", char(8));
        ::fprintf(m_file, "%c", ' ');
        ::fprintf(m_file, "%c", char(8));
        i--;
    }
}

