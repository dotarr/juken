#include <common.h>

#include <discid.h>

#include "consolelistener.h"

void
ConsoleListener::Handshake(const char* id)
{
    ::fprintf(m_fd, "connection established to %s\n", id+4);
}

void
ConsoleListener::InfoChanged(short slot, byte track, enum mode mode, 
                             enum random random, bool repeat, 
                             byte userfile)
{
    //display the info event
    ::fprintf(m_fd, "disc#: %d track: %d", slot, track);
    ::fprintf(m_fd, " mode: %s", MODE_NAMES[mode]);
    switch ( mode )
    {
        case UserfileMode:
            ::fprintf(m_fd, "(0x%02X)", userfile);
            break;
        //case MusicTypeMode:
        //    ::fprintf(m_fd, "(%s)", GENRE_NAMES[genre]);
        //    break;
    }

    if ( random!=RandomOff || repeat )
    {
        ::fprintf(m_fd, " (");
        if ( random != RandomOff ) 
        {
            ::fprintf(m_fd, "%s", RANDOM_NAMES[random]);
            if ( repeat )
                ::fprintf(m_fd, ",");
        }
        if ( repeat )
            ::fprintf(m_fd, "repeat");
        ::fprintf(m_fd, ")");
    }
    ::fprintf(m_fd, "\n");
}

void
ConsoleListener::StateChanged(enum state state)
{
    // display the current state
    ::fprintf(m_fd, "state: %s\n", STATE_NAMES[state]);
}

void
ConsoleListener::DiscChanged(short slot)
{
    // display the current disc number
    ::fprintf(m_fd, "disc#: %d\n", slot);
}

void
ConsoleListener::DoorChanged(bool door_closed)
{
    // display the door state
    ::fprintf(m_fd, "door: %s\n", door_closed?"closed":"open");
}

void
ConsoleListener::DiscDataReply(DiscData* info)
{
    if ( info->title[0] == 0x01 )
        info->title[0] = '\0';

    // output the reply
    if ( info->track == 0 )
    {
        ::fprintf(m_fd, "%-25s ", info->title);
        if ( info->genre != UNKNOWN )
            ::fprintf(m_fd, "genre: %-22s ", GENRE_NAMES[info->genre]);
        if ( info->userfiles != 0x00 )
            ::fprintf(m_fd, " userfiles: 0x%02X ", info->userfiles);
        if ( info->formatting != 0x00 )
            ::fprintf(m_fd, "formatting: %s ", (info->formatting==0x13)?"cd-text":"unknown");
        ::fprintf(m_fd, "\n");
    }
    else
    {
        ::fprintf(m_fd, "[%3d] ", info->track);
        ::fprintf(m_fd, "%-25s ", info->title);
        if ( info->request_type != 0x01 )
            ::fprintf(m_fd, "   request_type: 0x%02X ", info->request_type);
        ::fprintf(m_fd, "\n");
    }
}

void
ConsoleListener::CDTextDataReply(CDTextData* info)
{
    if ( info->title[0] == 0x01 )
        info->title[0] = '\0';

    // output the reply
    if ( info->track == 0 )
    {
        ::fprintf(m_fd, "%-25s ", info->title);
    }
    else
    {
        ::fprintf(m_fd, "[%3d] ", info->track);
        ::fprintf(m_fd, "%-25s ", info->title);
    }

    ::fprintf(m_fd, "unknown : ");
    ::fprintf(m_fd, "0x%02X ", info->unknown_1);
    ::fprintf(m_fd, "0x%02X ", info->unknown_2);
    ::fprintf(m_fd, "0x%02X ", info->unknown_3);
    ::fprintf(m_fd, "\n");
}

void
ConsoleListener::TrackTimesReply(TrackTimes* info)
{
    TimeInfo* times = (TimeInfo*) &(info->times);

    ::fprintf(m_fd, "disc: %3d ", info->slot);
    if ( info->formatting != 0x00 )
        ::fprintf(m_fd, "formatting: %s ", (info->formatting==0x13)?"cd-text":"unknown");
    ::fprintf(m_fd, "\n");
    // output the reply
    for (int i=0; i<info->num_tracks+1; i++)
    {
        ::fprintf(m_fd, "[%3d] ", i);
        ::fprintf(m_fd, "%02X:%02X:%02X ", times[i].minute, times[i].second, times[i].subsecond);
        ::fprintf(m_fd, "\n");
    }
    ::fprintf(m_fd, "discid=[%08x]\n", discid(info->num_tracks, times));
}

void
ConsoleListener::DiscTrackListReply(DiscTrackList* info)
{
    //::fprintf(m_fd, "%s cmd=%d len=%d\n", "best data", reply_cmd, reply_len);
    //printdata(m_fd, reply_data, reply_len);
}


