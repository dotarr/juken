#include <common.h>

#include <discid.h>
#include <cdchanger.h>
#include <dvdchanger.h>

#include "importer.h"

Importer::Importer(const char* filename)
: m_done(false), m_device(NULL), m_changer(NULL)
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

    delete m_device;
    m_device = NULL;
}

void
Importer::InitState()
{
    // handshake with device
    char* id = m_device->DoHandshake("I'm PC");
    ::fprintf(stdout, "connection established to %s\n", id+4);

    // create appropriate changer 
    if ( ::strcmp(id, "I'm CD-425M") == 0 )
        m_changer = new CDChanger(id, *m_device);
    else
        m_changer = new DVDChanger(id, *m_device);

    // add this as a listener that prints events and data to stdout
    m_changer->pushListener(this);

    // process InfoChanged
    m_changer->DoEvent();
    // process StateChanged
    m_changer->DoEvent();
}

void
Importer::Run()
{
    DVDChanger* dvdchanger = (DVDChanger*) m_changer;
    const char* names[8];
    for (int i=0; i<8; i++) names[i] = m_data.getUserfileName(i);

    dvdchanger->WriteUserfileNames(names);
    dvdchanger->DoQuery(0, 1, 5, 0, 0, 0);

    list<Disc*> discs = m_data.getDiscs();
    list<Disc*>::iterator iter = discs.begin();
    while ( iter != discs.end() )
    {
        Disc* disc = *iter;
printf("writing disc: %d title: %s artist: %s\n", disc->getSlot(), 
                                                  disc->getShortTitle(), 
                                                  disc->getDescription());
        dvdchanger->WriteTitleArtist(disc->getSlot(), 
                                     disc->getShortTitle(), 
                                     disc->getDescription());

printf("querying\n");
        dvdchanger->DoQuery(0, 1, 6, disc->getSlot(), 0, 0);
        iter++;
        usleep(100);
    }

printf("-------------------------------------------\n");


    /*
    int juke_fd = m_device->GetFileDescriptor();
    fd_set fds;
    while ( !m_done )
    {
        // setup fd set
        FD_ZERO(&fds);
        FD_SET(juke_fd, &fds);
        
        // select for something to do
        int num_fds = ::select(juke_fd+1, &fds, NULL, NULL, NULL);
        if ( num_fds == -1 )
            m_done = true;

        // if we have something to do ...
        if ( num_fds > 0 )
        {
            if ( FD_ISSET(juke_fd, &fds) )
            {
                // do an event
                m_changer->DoEvent();
            }
        }
    }
    */
}

// ------------------------------ Listener Interface ---------------------------------
bool
Importer::InfoChanged(short slot, byte title, short chapter) 
{
    //display the info event
    if ( chapter == 0 )
        ::fprintf(stdout, "disc: %d track: %d\n", slot, title);
    else
        ::fprintf(stdout, "disc: %d title: %d chapter: %d\n", slot, title, chapter);

    return false;
}

bool
Importer::ModeChanged(enum mode mode, bool repeat, byte param)
{
    ::fprintf(stdout, "mode: %s", MODE_NAMES[mode]);

    switch ( mode )
    {
        case ProgramMode:
            ::fprintf(stdout, " (%d)", param);
            break;
        case UserfileMode:
        case UserfileModeRandomOne:
        case UserfileModeRandomAll:
            ::fprintf(stdout, " (0x%02X)", param);
            break;
        case MusicTypeMode:
        case MusicTypeModeRandomAll:
            ::fprintf(stdout, " (%s)", GENRE_NAMES[param]);
            break;
    }

    if ( repeat )
        ::fprintf(stdout, " (repeat)");

    ::fprintf(stdout, "\n");

    return false;
}

bool
Importer::StateChanged(enum state state)
{
    // display the current state
    switch ( state )
    {
        case Stopped:      ::fprintf(stdout, "state: stopped\n"); break;
        case Standby:      ::fprintf(stdout, "state: standby\n"); break;
        case Stopping:     ::fprintf(stdout, "state: stopping\n"); break;
        case Changing:     ::fprintf(stdout, "state: changing\n"); break;
        case Playing:      ::fprintf(stdout, "state: playing\n"); break;
        case Paused:       ::fprintf(stdout, "state: paused\n"); break;
        case SkipForward:  ::fprintf(stdout, "state: skip forward\n"); break;
        case SkipBackward: ::fprintf(stdout, "state: skip backward\n"); break;
        default:           ::fprintf(stdout, "state: UNKNOWN\n"); break;
    }

    return false;
}

bool
Importer::DoorChanged(bool door_open)
{
    // display the door state
    ::fprintf(stdout, "door: %s\n", door_open?"open":"closed");

    return false;
}

bool
Importer::TextDataReply(short slot, byte track, byte userfiles, 
                     byte request_type, byte genre, 
                     byte formatting, char* title)
{
    if ( title[0] == 0x01 )
        title[0] = '\0';

    // output the reply
    if ( track == 0 )
    {
        ::fprintf(stdout, "[%3d] %-25s ", slot, title);
        if ( genre != UNKNOWN )
            ::fprintf(stdout, "genre: %-22s(%2d) ", GENRE_NAMES[genre], genre);
        if ( userfiles != 0x00 )
            ::fprintf(stdout, " userfiles: 0x%02X ", userfiles);
        if ( formatting != 0x00 )
            ::fprintf(stdout, "formatting: %s ", (formatting==0x13)?"cd-text":"unknown");
        ::fprintf(stdout, "\n");
    }
    else
    {
        ::fprintf(stdout, "[%2d] ", track);
        ::fprintf(stdout, "%-25s ", title);
        if ( request_type != 0x01 )
            ::fprintf(stdout, "   request_type: 0x%02X ", request_type);
        ::fprintf(stdout, "\n");
    }

    return false;
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

    list<Disc*> discs = m_data.getDiscs();
    list<Disc*>::iterator iter = discs.begin();
    while ( iter != discs.end() )
    {
        (*iter)->print();
        iter++;
    }

}

