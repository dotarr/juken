#include <common.h>

#include <discid.h>

#include "exportlistener.h"

ExportListener::ExportListener(FILE* f)
    : m_file(f), m_discid(0), m_track_count(0)
{
}

ExportListener::~ExportListener()
{
    ::fprintf(m_file, "EXTD=\n");
    for (int i=0; i<m_track_count; i++)
        ::fprintf(m_file, "EXTT%d=\n", i);
    ::fprintf(m_file, "PLAYORDER=\n");
}

bool
ExportListener::DiscDataReply(DiscData* info)
{
    if ( info->title[0] == 0x01 )
        info->title[0] = '\0';

    // output the reply
    if ( info->track == 0 )
    {
        ::fprintf(m_file, "DTITLE=%s\n", info->title);
        ::fprintf(m_file, "DYEAR=\n");
        const char* genre = "";
        if ( info->genre > 1 )
            genre = GENRE_NAMES[info->genre];
        ::fprintf(m_file, "DGENRE=%s\n", genre);
    }
    else
    {
        ::fprintf(m_file, "TITLE%d=%s\n", info->track-1, info->title);
        m_track_count++;
    }

    return true;
}

bool
ExportListener::CDTextDataReply(CDTextData* info)
{
    if ( info->title[0] == 0x01 )
        info->title[0] = '\0';

    // output the reply
    if ( info->track == 0 )
    {
        ::fprintf(m_file, "DTITLE=%s\n", info->title);
        ::fprintf(m_file, "DYEAR=\n");
        ::fprintf(m_file, "DGENRE=\n");
    }
    else
    {
        ::fprintf(m_file, "TITLE%d=%s\n", info->track-1, info->title);
        m_track_count++;
    }

    return true;
}

bool
ExportListener::TrackTimesReply(TrackTimes* info)
{
    TimeInfo* times = (TimeInfo*) &(info->times);
    m_discid = discid(info->num_tracks, times);
    ::fprintf(m_file, "DISCID=[%08x]\n", m_discid);

    return true;
}

bool
ExportListener::DiscTrackListReply(DiscTrackList* info)
{
    return true;
}


