#include <common.h>

#include <discid.h>

#include "consolelistener.h"

ConsoleListener::ConsoleListener(FILE* f, short capacity)
: m_file(f), m_capacity(capacity)
{
    m_titles = NULL;
}

ConsoleListener::~ConsoleListener()
{
    if ( m_titles != NULL )
        for (int i=0; i<m_capacity; i++)
            delete m_titles[i];
    delete[] m_titles;
    m_titles = NULL;
}

bool
ConsoleListener::InfoChanged(short slot, byte track, enum mode mode, 
                             enum random random, bool repeat, 
                             byte userfile)
{
    //display the info event
    ::fprintf(m_file, "disc#: %d track: %d", slot, track);
    ::fprintf(m_file, " mode: %s", MODE_NAMES[mode]);
    switch ( mode )
    {
        case UserfileMode:
            ::fprintf(m_file, "(0x%02X)", userfile);
            break;
        //case MusicTypeMode:
        //    ::fprintf(m_file, "(%s)", GENRE_NAMES[genre]);
        //    break;
    }

    if ( random!=RandomOff || repeat )
    {
        ::fprintf(m_file, " (");
        if ( random != RandomOff ) 
        {
            ::fprintf(m_file, "%s", RANDOM_NAMES[random]);
            if ( repeat )
                ::fprintf(m_file, ",");
        }
        if ( repeat )
            ::fprintf(m_file, "repeat");
        ::fprintf(m_file, ")");
    }
    ::fprintf(m_file, "\n");

    return false;
}

bool
ConsoleListener::StateChanged(enum state state)
{
    // display the current state
    ::fprintf(m_file, "state: %s\n", STATE_NAMES[state]);

    return false;
}

bool
ConsoleListener::DiscChanged(short slot)
{
    // display the current disc number
    ::fprintf(m_file, "disc#: %d\n", slot);

    m_cur_slot = slot;

    return false;
}

bool
ConsoleListener::DoorChanged(bool door_closed)
{
    // display the door state
    ::fprintf(m_file, "door: %s\n", door_closed?"closed":"open");

    return false;
}

bool
ConsoleListener::DiscDataReply(DiscData* info)
{
    if ( info->title[0] == 0x01 )
        info->title[0] = '\0';

    if ( m_titles == NULL )
    {
        m_titles = new char*[m_capacity];
        for (int i; i<m_capacity; i++)
            m_titles[i] = NULL;
    }

    // output the reply
    if ( info->track == 0 )
    {
        m_titles[info->slot] = strdup(info->title);

        ::fprintf(m_file, "[%3d] %-25s ", info->slot, info->title);
        if ( info->genre != UNKNOWN )
            ::fprintf(m_file, "genre: %-22s(%2d) ", GENRE_NAMES[info->genre], info->genre);
        if ( info->userfiles != 0x00 )
            ::fprintf(m_file, " userfiles: 0x%02X ", info->userfiles);
        if ( info->formatting != 0x00 )
            ::fprintf(m_file, "formatting: %s ", (info->formatting==0x13)?"cd-text":"unknown");
        ::fprintf(m_file, "\n");
    }
    else
    {
        ::fprintf(m_file, "[%2d] ", info->track);
        ::fprintf(m_file, "%-25s ", info->title);
        if ( info->request_type != 0x01 )
            ::fprintf(m_file, "   request_type: 0x%02X ", info->request_type);
        ::fprintf(m_file, "\n");
    }

    return false;
}

bool
ConsoleListener::CDTextDataReply(CDTextData* info)
{
    if ( info->title[0] == 0x01 )
        info->title[0] = '\0';

    // output the reply
    if ( info->track == 0 )
    {
        ::fprintf(m_file, "%-25s ", info->title);
    }
    else
    {
        ::fprintf(m_file, "[%3d] ", info->track);
        ::fprintf(m_file, "%-25s ", info->title);
    }

    ::fprintf(m_file, "unknown : ");
    ::fprintf(m_file, "0x%02X ", info->unknown_1);
    ::fprintf(m_file, "0x%02X ", info->unknown_2);
    ::fprintf(m_file, "0x%02X ", info->unknown_3);
    ::fprintf(m_file, "\n");

    return false;
}

bool
ConsoleListener::TrackTimesReply(TrackTimes* info)
{
    TimeInfo* times = (TimeInfo*) &(info->times);

    ::fprintf(m_file, "disc: %3d ", info->slot);
    if ( info->formatting != 0x00 )
        ::fprintf(m_file, "formatting: %s ", (info->formatting==0x13)?"cd-text":"unknown");
    ::fprintf(m_file, "\n");
    // output the reply
    for (int i=0; i<info->num_tracks+1; i++)
    {
        ::fprintf(m_file, "[%3d] ", i);
        ::fprintf(m_file, "%02X:%02X:%02X ", times[i].minute, times[i].second, times[i].subsecond);
        ::fprintf(m_file, "\n");
    }
    ::fprintf(m_file, "discid=[%08x]\n", discid(info->num_tracks, times));

    return false;
}

bool
ConsoleListener::DiscTrackListReply(DiscTrackList* info)
{
    //::fprintf(m_file, "%s cmd=%d len=%d\n", "best data", reply_cmd, reply_len);
    //printdata(m_file, reply_data, reply_len);

    return false;
}


