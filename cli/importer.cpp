#include <common.h>

#include <discid.h>
#include <cdchanger.h>
#include <dvdchanger.h>

#include "importer.h"

Importer::Importer(const char* filename)
: m_device(NULL), m_listener(NULL), m_changer(NULL)
{
    ::fprintf(stderr, "parsing xml file %s\n", filename);
    LogMsg("parsing xml file %s\n", filename);
    m_data.ParseFile(filename);

    m_device = new KenwoodDevice(m_data.getDevice());
}

Importer::~Importer()
{
    delete m_changer;
    m_changer = NULL;

    delete m_listener;
    m_listener = NULL;

    delete m_device;
    m_device = NULL;
}

void
Importer::InitState()
{
    // handshake with device
    char* id = m_device->DoHandshake("I'm PC");
    ::fprintf(stderr, "connection established to %s\n", id+4);
    LogMsg("connection established to %s\n", id+4);

    // make a listener
    m_listener = new ConsoleListener(stderr);

    // create appropriate changer 
    if ( ::strcmp(id, "I'm CD-425M") == 0 )
        m_changer = new CDChanger(id, *m_device, m_listener);
    else
        m_changer = new DVDChanger(id, *m_device, m_listener);
}

void
Importer::Run()
{
    ::fprintf(stderr, "\n");
    LogMsg("\n");

    m_changer->WriteUserfileNames(m_data.getUserfileNames());

    DiscList& discs = m_data.getDiscs();
    for (DiscList::iterator iter=discs.begin(); iter!=discs.end(); iter++)
    {
        Disc& disc = (*iter);

        ::fprintf(stderr, "\n");
        LogMsg("\n");

        ::fprintf(stderr, "writing disc: %d\n", disc.index);
        LogMsg("writing disc: %d\n", disc.index);
        ::fprintf(stderr, "       title: %s\n", disc.title);
        LogMsg("       title: %s\n", disc.title);
        if ( disc.artist != NULL )
        {
            ::fprintf(stderr, "      artist: %s\n", disc.artist);
            LogMsg("      artist: %s\n", disc.artist);
        }
        if ( disc.userfiles != 0 )
        {
            ::fprintf(stderr, "   userfiles: ");
            LogMsg("   userfiles: ");
            int uf = disc.userfiles;
            for (int i=0; i<8; i++)
            {
                if ( (uf&1) != 0 )
                {
                    ::fprintf(stderr, "%s", m_data.getUserfileName(i));
                    LogMsg("%s", m_data.getUserfileName(i));
                    if ( uf != 1 )
                    {
                        ::fprintf(stderr, ", ");
                        LogMsg(", ");
                    }
                }
                uf = uf>>1;
            }
            ::fprintf(stderr, "\n");
            LogMsg("\n");
        }
        if ( disc.genre > 1 )
        {
            ::fprintf(stderr, "       genre: %s\n", GENRE_NAMES[disc.genre]);
            LogMsg("       genre: %s\n", GENRE_NAMES[disc.genre]);
        }

        m_changer->WriteDisc(disc.index, disc);
    }

    ::fprintf(stderr, "\n");
    LogMsg("\n");

    ::fprintf(stderr, "import complete!\n");
    LogMsg("import complete!\n");
}

