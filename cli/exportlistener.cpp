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
        m_num_tracks = info->track;
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
        m_num_tracks = info->track;
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
