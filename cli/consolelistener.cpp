#include "consolelistener.h"
#include "kenwoodchanger.h"

void
ConsoleListener::InfoChanged(KenwoodChanger* changer, short slot, byte title, short chapter) 
{
    //display the info event
    if ( chapter == 0 )
    {
        ::fprintf(m_file, "disc: %d track: %d\n", slot, title);
        LogMsg("disc: %d track: %d\n", slot, title);
    }
    else
    {
        ::fprintf(m_file, "disc: %d title: %d chapter: %d\n", slot, title, chapter);
        LogMsg("disc: %d title: %d chapter: %d\n", slot, title, chapter);
    }
}

void
ConsoleListener::ModeChanged(KenwoodChanger* changer, enum mode mode, bool repeat, byte param)
{
    ::fprintf(m_file, "mode: %s", MODE_NAMES[mode]);
    LogMsg("mode: %s", MODE_NAMES[mode]);

    switch ( mode )
    {
        case ProgramMode:
            ::fprintf(m_file, " (%d)", param);
            LogMsg(" (%d)", param);
        break;
        case UserfileMode:
        case UserfileModeRandomOne:
        case UserfileModeRandomAll:
        {
            byte i = 0;
            byte uf = param;
            while ( uf != 1 ) { uf = uf>>1; i++; }
            ::fprintf(m_file, " (0x%02X)", changer->getUserfileName(i));
            LogMsg(" (0x%02X)", changer->getUserfileName(i));
        }
        break;
        case MusicTypeMode:
        case MusicTypeModeRandomAll:
            ::fprintf(m_file, " (%s)", GENRE_NAMES[param]);
            LogMsg(" (%s)", GENRE_NAMES[param]);
        break;
    }

    if ( repeat )
    {
        ::fprintf(m_file, " (repeat)");
        LogMsg(" (repeat)");
    }

    ::fprintf(m_file, "\n");
    LogMsg("\n");
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
ConsoleListener::DoorChanged(KenwoodChanger* changer, bool door_open)
{
    // display the door state
    ::fprintf(m_file, "door: %s\n", door_open?"open":"closed");
    LogMsg("door: %s\n", door_open?"open":"closed");
}

const char* ConsoleListener::scanning_discs_str = 
                            "scanning discs ... ";
const char* ConsoleListener::loading_userfiles_str = 
                            "loading userfile names ... ";
const char* ConsoleListener::changing_disc_str = 
                            "changing disc ... ";
const char* ConsoleListener::reading_disc_str = 
                            "reading disc info ... ";
const char* ConsoleListener::writing_userfiles_str = 
                            "writing userfile names ... ";
const char* ConsoleListener::writing_disc_str = 
                            "writing disc info ... ";

void
ConsoleListener::ProgressStart(KenwoodChanger* changer, enum operation op, int length)
{
    const char* str = "";
    switch ( op )
    {
        case ScanDiscs:        str = scanning_discs_str;    break;
        case LoadUserfiles:    str = loading_userfiles_str; break;
        case ChangingDisc:     str = changing_disc_str;     break;
        case ReadingDisc:      str = reading_disc_str;      break;
        case WritingUserfiles: str = writing_userfiles_str; break;
        case WritingDisc:      str = writing_disc_str;      break;
        default: break;
    }

    ::fprintf(m_file, "%s", str);
    m_last_prog_len = 0;

    ::fflush(m_file);

    LogMsg("%s\n", str);
}

void
ConsoleListener::Progress(KenwoodChanger* changer, enum operation op, 
                          int progress, const char* str)
{
    const char* fmt = "";
    int fmt_len = 0;
    switch ( op )
    {
        case ScanDiscs:        fmt = "[%3d] %s"; fmt_len = 6; break;
        case LoadUserfiles:    fmt = "[%1d] %s"; fmt_len = 4; break;
        case ChangingDisc:     fmt = "[%1d] %s"; fmt_len = 4; break;
        case ReadingDisc:      fmt = "[%3d] %s"; fmt_len = 6; break;
        case WritingUserfiles: fmt = "[%1d] %s"; fmt_len = 4; break;
        case WritingDisc:      fmt = "[%3d] %s"; fmt_len = 6; break;
        default:
        break;
    }

    back_space(m_last_prog_len);
    ::fprintf(m_file, fmt, progress, str);
    m_last_prog_len = fmt_len + ::strlen(str);

    ::fflush(m_file);

    LogMsg(fmt, progress, str);
    LogMsg("\n");
}

void
ConsoleListener::ProgressEnd(KenwoodChanger* changer, enum operation op)
{
    back_space(m_last_prog_len);
    ::fprintf(m_file, "complete\n");
    m_last_prog_len = 0;

    ::fflush(m_file);

    LogMsg("complete\n");
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

