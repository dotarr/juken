#include <common.h>

#include <discid.h>
#include <cdchanger.h>
#include <dvdchanger.h>

#include "importer.h"

Importer::Importer(const char* filename)
: m_device(NULL), m_listener(NULL), m_changer(NULL)
{
    ::fprintf(stderr, "xml filename %s\n", filename);
    m_data.ParseFile(filename);
    print_data();

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

    // make a listener
    m_listener = new ConsoleListener(stdout);

    // create appropriate changer 
    if ( ::strcmp(id, "I'm CD-425M") == 0 )
        m_changer = new CDChanger(id, *m_device, m_listener);
    else
        m_changer = new DVDChanger(id, *m_device, m_listener);
}

void
Importer::Run()
{
    const char* names[8];
    for (int i=0; i<8; i++) names[i] = m_data.getUserfileName(i);

    m_changer->WriteUserfileNames(names);

    list<DiscElement*> discs = m_data.getDiscs();
    list<DiscElement*>::iterator iter = discs.begin();
    while ( iter != discs.end() )
    {
        short slot = (*iter)->getSlot();
        byte type = (byte) (*iter)->getType();
        const char* title = (*iter)->getShortTitle();
        const char* artist = (*iter)->getDescription();
        Disc disc(slot, type, title, artist);
        list<char*> tracks = (*iter)->getTracks();
        int i = 1;
        for (list<char*>::iterator track=tracks.begin(); track!=tracks.end(); track++)
            disc.tracks.push_back(Name(i++, TRACK_NAME, *track));
        disc.userfiles = (*iter)->getUserfiles();

printf("writing disc: %d title: %s artist: %s\n", slot, title, artist);
        m_changer->WriteDisc(slot, disc);

        iter++;
    }

printf("-------------------------------------------\n");
}

void
Importer::print_data()
{
    printf("changer %s [%s]\n", m_data.getModel(), m_data.getDevice());
    typedef char* foo;
    const char* names[8];
    for (int i=0; i<8; i++) names[i] = m_data.getUserfileName(i);

    for (int i=0; i<8; i++)
    {
        printf("  userfile[%d] %s\n", i, names[i]);
    }

    list<DiscElement*> discs = m_data.getDiscs();
    list<DiscElement*>::iterator iter = discs.begin();
    while ( iter != discs.end() )
    {
        (*iter)->print();
        iter++;
    }

}

