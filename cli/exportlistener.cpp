#include <common.h>

#include <discid.h>

#include "exportlistener.h"

ExportListener::ExportListener(uint disc_id, const char* path)
    : m_file(NULL), m_num_tracks(0)
{
    OpenFile(disc_id, path);
    ::fprintf(m_file, "DISCID=[%08x]\n", disc_id);
}

ExportListener::~ExportListener()
{
    CloseFile();
}

bool
ExportListener::DiscDataReply(short slot, byte track, byte userfiles, 
                              byte request_type, byte genre, 
                              byte formatting, char* title)
{
    if ( title[0] == 0x01 )
        title[0] = '\0';

    // output the reply
    if ( track == 0 )
    {
        ::fprintf(m_file, "DTITLE=%s\n", title);
        ::fprintf(m_file, "DYEAR=\n");
        const char* genre_name = "";
        if ( genre > 1 )
            genre_name = GENRE_NAMES[genre];
        ::fprintf(m_file, "DGENRE=%s\n", genre_name);
    }
    else
    {
        ::fprintf(m_file, "TITLE%d=%s\n", track-1, title);
        m_num_tracks = track;
    }

    return true;
}

bool
ExportListener::CDTextDataReply(short slot, byte track, byte request_type,
                                byte formatting, char* title)
{
    if ( title[0] == 0x01 )
        title[0] = '\0';

    // output the reply
    if ( track == 0 )
    {
        ::fprintf(m_file, "DTITLE=%s\n", title);
        ::fprintf(m_file, "DYEAR=\n");
        ::fprintf(m_file, "DGENRE=\n");
    }
    else
    {
        ::fprintf(m_file, "TITLE%d=%s\n", track-1, title);
        m_num_tracks = track;
    }

    return true;
}

void
ExportListener::OpenFile(uint disc_id, const char* path)
{
    char* fname = (char*) malloc(strlen(path)+8+1);
    strcpy(fname, path);
    char id_str[8+1];
    sprintf(id_str, "%08x", disc_id);
    strcat(fname, id_str);
    
    printf("exporting to file: %s\n", fname);
    m_file = fopen(fname, "w+");
    ThrowIfNull(m_file, "unable to open %s: %s", fname, strerror(errno));
}

void
ExportListener::CloseFile()
{
    ::fprintf(m_file, "EXTD=\n");
    for (int i=0; i<m_num_tracks; i++)
        ::fprintf(m_file, "EXTT%d=\n", i);
    ::fprintf(m_file, "PLAYORDER=\n");

    ::fclose(m_file);
    m_file = NULL;
}
