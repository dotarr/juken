#include <common.h>

#include <discid.h>
#include <cdchanger.h>
#include <dvdchanger.h>

#include "exporter.h"

Exporter::Exporter(const char* device, short start, short end)
: m_device(NULL), m_listener(NULL), m_changer(NULL),
  m_device_name(device), m_model_name(NULL), m_start(start), m_end(end)
{
    m_device = new KenwoodDevice(device);
}

Exporter::~Exporter()
{
    delete m_model_name;
    m_model_name = NULL;

    delete m_changer;
    m_changer = NULL;

    delete m_listener;
    m_listener = NULL;

    delete m_device;
    m_device = NULL;
}

void
Exporter::InitState()
{
    // handshake with device
    char* id = m_device->DoHandshake("I'm PC");
    m_model_name = ::strdup(&id[4]);
    ::fprintf(stderr, "connection established to %s\n", id+4);

    // make a listener
    m_listener = new LoggingListener();
    
    // create appropriate changer 
    if ( ::strcmp(id, "I'm CD-425M") == 0 )
        m_changer = new CDChanger(id, *m_device, m_listener);
    else
        m_changer = new DVDChanger(id, *m_device, m_listener);
}

void
Exporter::Run()
{
    LogMsg("writing xml for %s on device %s\n", m_model_name, m_device_name);
    ::fprintf(stdout, "<Changer>\n");
    ::fprintf(stdout, "  <Model>%s</Model>\n", m_model_name);
    ::fprintf(stdout, "  <Device>%s</Device>\n", m_device_name);
    ::fprintf(stdout, "\n");
    ::fflush(stdout);

    LogMsg("writing userfiles\n");
    ::fprintf(stdout, "  <Userfiles>\n");
    for (int i=0; i<8; i++)
    {
        const char* name = m_changer->getUserfileName(i);
        ::fprintf(stdout, "    <Userfile%d>%s</Userfile%d>\n", i+1, name, i+1);
    }
    ::fprintf(stdout, "  </Userfiles>\n");
    ::fprintf(stdout, "\n");
    ::fflush(stdout);

    short capacity = m_changer->getCapacity();
    if ( m_start < 1 ) m_start = 1;
    if ( m_start > capacity ) m_start = capacity;
    if ( m_end < 1 ) m_end = 1;
    if ( m_end > capacity ) m_end = capacity;
    if ( m_end < m_start ) m_end = m_start;

    for (int slot=m_start; slot<=m_end; slot++)
    {
        if ( m_changer->isSlotOccupied(slot) )
        {
            LogMsg("processing slot %d\n", slot);
            char* disc_id = m_changer->GetDiscId(slot);
            Disc disc = m_changer->DoListContents(slot);

            char* element = NULL;
            switch ( disc.type )
            {
                case DISC_CD_A:   element = "CD";   break;
                case DISC_CD_MP3: element = "MP3";  break;
                case DISC_CD_V:   element = "VCD";  break;
                case DISC_DVD_A:  element = "DVDA"; break;
                case DISC_DVD_V:  element = "DVD";  break;
            }

            LogMsg("writing %s: %s id: %s\n", element, (const char*)disc, disc_id);
            ::fprintf(stdout, "  <%s>\n", element);

            ::fprintf(stdout, "    <ID>%s</ID>\n", disc_id);
            ::fprintf(stdout, "    <Slot>%03d</Slot>\n", disc.index);
            if ( disc.title != NULL )
            {
                ::fprintf(stdout, "    <ShortTitle>%s</ShortTitle>\n", disc.title);
            }
            if ( disc.artist != NULL )
                ::fprintf(stdout, "    <Description>%s</Description>\n", disc.artist);

            if ( disc.genre > UNASSIGNED )
                ::fprintf(stdout, "    <Genre>%s</Genre>\n", GENRE_NAMES[disc.genre]);

            if ( disc.userfiles != 0 )
            {
                ::fprintf(stdout, "    <Userfiles>\n");
                byte uf = disc.userfiles;
                for (int i=0; i<8; i++)
                {
                    if ( uf&0x01 == 1 )
                    {
                        const char* name = m_changer->getUserfileName(i);
                        ::fprintf(stdout, "      <Userfile>%s</Userfile>\n", name);
                    }
                    uf = uf>>1;
                }
                ::fprintf(stdout, "    </Userfiles>\n");
            }

            LogMsg("writing tracks\n");
            if ( disc.tracks.size() > 0 )
            {
                ::fprintf(stdout, "    <Tracks>\n");
                NameList& tracks = disc.tracks;
                for (NameList::iterator iter=tracks.begin(); iter!=tracks.end(); iter++)
                {
                    Name& track = (*iter);
                    ::fprintf(stdout,"      <Track>%s</Track>\n", track.text);
                }
                ::fprintf(stdout, "    </Tracks>\n");
            }

            ::fprintf(stdout, "  </%s>\n", element);
            ::fprintf(stdout, "\n");
            ::fflush(stdout);
        }
    }

    ::fprintf(stdout, "</Changer>\n");
    ::fflush(stdout);
    LogMsg("export complete!\n");
}

