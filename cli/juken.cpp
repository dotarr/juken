#include <common.h>

#include <discid.h>
#include <cdchanger.h>
#include <dvdchanger.h>

#include "juken.h"
#include "exportlistener.h"

#ifdef HAVE_LIBREADLINE
    #if defined(HAVE_READLINE_READLINE_H)
        #include <readline/readline.h>
    #elif defined(HAVE_READLINE_H)
        #include <readline.h>
    #else /* !defined(HAVE_READLINE_H) */
        #error cant find readline
    #endif /* !defined(HAVE_READLINE_H) */
#else /* !defined(HAVE_READLINE_READLINE_H) */
    #error we require readline
#endif /* HAVE_LIBREADLINE */

#ifdef HAVE_READLINE_HISTORY
    #if defined(HAVE_READLINE_HISTORY_H)
        #include <readline/history.h>
    #elif defined(HAVE_HISTORY_H)
        #include <history.h>
    #endif /* defined(HAVE_READLINE_HISTORY_H) */
#endif /* HAVE_READLINE_HISTORY */

BEGIN_C_DECLS
extern char** buildargv(char *);
extern void freeargv(char **);
END_C_DECLS

Juken* Juken::g_juken = NULL;

struct Juken::cmd Juken::m_short_commands[] = 
{
    { "q",      &Juken::DoQuit,       "quit the application" },
    { "h",      &Juken::DoHelp,       "this help output" },
    { "ls",     &Juken::DoList,       "list the changer contents" },
    { "cd",     &Juken::DoChangeDisc, "change the current disc" },
    { "p",      &Juken::DoPlay,       "play/pause the current disc" },
    { "s",      &Juken::DoStop,       "stop the current disc" },
    { NULL, NULL }
};

struct Juken::cmd Juken::m_commands[] = 
{
    { "quit",   &Juken::DoQuit,       "quit the application" },
    { "exit",   &Juken::DoQuit,       "quit the application" },
    { "help",   &Juken::DoHelp,       "this help output" },
    { "export", &Juken::DoExport,     "export the changer contents" },
    { "list",   &Juken::DoList,       "list the changer contents" },
    { "bests",  &Juken::GetBests,     "list the best tracks" },
    { "change", &Juken::DoChangeDisc, "change the current disc" },
    { "play",   &Juken::DoPlay,       "play the current disc" },
    { "pause",  &Juken::DoPlay,       "pause the current disc" },
    { "prev",   &Juken::DoPrev,       "play the previous track on the current disc" },
    { "next",   &Juken::DoNext,       "play the next track on the current disc" },
    { "stop",   &Juken::DoStop,       "stop the current disc" },
    { NULL, NULL }
};

Juken::Juken(const char* serial_device)
: m_device(NULL), m_changer(NULL)
{
    m_done = false;

    m_file = stdout;

    m_door_closed = true;
    m_cur_slot = 0;
    m_cur_track = 0;
    m_cur_state = Stopped;
    m_cur_mode = TrackMode;
    m_cur_random = RandomOff;
    m_cur_repeat = false;
    m_cur_userfile = 0x00;

    m_capacity = 0;
    m_titles = (char**) NULL;

    m_device = new KenwoodDevice(serial_device);

    g_juken = this;
}

Juken::~Juken()
{
    g_juken = NULL;

    rl_crlf();
    rl_callback_handler_remove();

    if ( m_titles != NULL )
        for (int i=0; i<m_capacity; i++)
            delete m_titles[i];
    delete[] m_titles;
    m_titles = NULL;

    //::fclose(m_file);
    m_file = NULL;

    delete m_changer;
    m_changer = NULL;

    delete m_device;
    m_device = NULL;
}

void
Juken::InitState()
{
    // handshake with device
    char* id = m_device->DoHandshake("I'm PC");
    ::fprintf(m_file, "connection established to %s\n", id+4);

    // create appropriate changer 
    if ( ::strcmp(id, "I'm CD-425M") == 0 )
    {
        m_changer = new CDChanger(id, *m_device);
        m_capacity = 200;
    }
    else
    {
        m_changer = new DVDChanger(id, *m_device);
        m_capacity = 403;
    }

    // add this as a listener that prints events and data to stdout
    m_changer->pushListener(this);

    // process InfoChanged
//    m_changer->DoEvent();
    // process DoorChanged
//    m_changer->DoEvent();
    // process StateChanged
//    m_changer->DoEvent();
    // process DiscChanged
//    m_changer->DoEvent();

    if ( m_door_closed )
    {
        // obtain disc listing
        if ( m_titles == NULL )
        {
            m_titles = new char*[m_capacity];
            for (int i=0; i<m_capacity; i++)
                m_titles[i] = NULL;
        }

        ::fprintf(m_file, "listing discs\n");
        usleep(10);

        m_loading_titles = true;
usleep(10); // an ugly hack, but I can't figure out how/why/when the player isn't "ready"
        m_changer->DoListDiscs();
        m_loading_titles = false;
        ::fprintf(m_file, "\n");
    }
}

void
Juken::Run()
{
    // setup readline callbacks
    rl_callback_handler_install("juken> ", &Juken::readline_callback);
    rl_attempted_completion_function = &Juken::completion_callback;

    int juke_fd = m_device->GetFileDescriptor();
    fd_set fds;
    while ( !m_done )
    {
        // setup fd set
        FD_ZERO(&fds);
        FD_SET(juke_fd, &fds);
        FD_SET(STDIN_FILENO, &fds);
        
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
                rl_crlf();
                m_changer->DoEvent();
                rl_on_new_line();
                rl_redisplay();
            }
            if ( FD_ISSET(STDIN_FILENO, &fds) )
            {
                // process input
                rl_callback_read_char();
            }
        }
    }
}

// ------------------------------ Listener Interface ---------------------------------
bool
Juken::InfoChanged(short slot, byte track, enum mode mode, 
                             enum random random, bool repeat, 
                             byte userfile)
{
    m_cur_slot = slot;
    m_cur_track = track;
    m_cur_mode = mode;
    m_cur_random = random;
    m_cur_repeat = repeat;
    m_cur_userfile = userfile;

    //display the info event
    ::fprintf(m_file, "disc#: %d track: %d", slot, track);
    ::fprintf(m_file, " mode: %s", MODE_NAMES[mode]);
    switch ( mode )
    {
        case UserfileMode:
            ::fprintf(m_file, "(0x%02X)", userfile);
            break;
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
Juken::StateChanged(enum state state)
{
    m_cur_state = state;

    // display the current state
    ::fprintf(m_file, "state: %s\n", STATE_NAMES[state]);

    return false;
}

bool
Juken::DiscChanged(short slot)
{
    m_cur_slot = slot;

    // display the current disc number
    ::fprintf(m_file, "disc#: %d\n", slot);

    m_cur_slot = slot;

    return false;
}

bool
Juken::DoorChanged(bool door_closed)
{
    bool uncache_titles = (!door_closed && m_door_closed);
    bool reload_titles = (door_closed && !m_door_closed);

    if ( uncache_titles )
    {
        // invalidate cached titles if door opens
        if ( m_titles != NULL )
            for (int i=0; i<m_capacity; i++)
                delete m_titles[i];
        delete[] m_titles;
        m_titles = NULL;
    }

    m_door_closed = door_closed;

    // display the door state
    ::fprintf(m_file, "door: %s\n", door_closed?"closed":"open");

    if ( reload_titles )
    {
        // obtain disc listing
        if ( m_titles == NULL )
        {
            m_titles = new char*[m_capacity];
            for (int i; i<m_capacity; i++)
                m_titles[i] = NULL;
        }

        ::fprintf(m_file, "listing discs\n");
        usleep(10);

        m_loading_titles = true;
usleep(10); // an ugly hack, but I can't figure out how/why/when the player isn't "ready"
        m_changer->DoListDiscs();
        m_loading_titles = false;
        ::fprintf(m_file, "\n");
    }

    return false;
}

bool
Juken::DiscDataReply(short slot, byte track, byte userfiles, 
                     byte request_type, byte genre, 
                     byte formatting, char* title)
{
    if ( title[0] == 0x01 )
        title[0] = '\0';

    // output the reply
    if ( track == 0 )
    {
        if ( m_loading_titles )
        {
            m_titles[slot] = strdup(title);
            ::fprintf(m_file, "#");
            ::fflush(m_file);
        }
        else
        {
            ::fprintf(m_file, "[%3d] %-25s ", slot, title);
            if ( genre != UNKNOWN )
                ::fprintf(m_file, "genre: %-22s(%2d) ", GENRE_NAMES[genre], genre);
            if ( userfiles != 0x00 )
                ::fprintf(m_file, " userfiles: 0x%02X ", userfiles);
            if ( formatting != 0x00 )
                ::fprintf(m_file, "formatting: %s ", (formatting==0x13)?"cd-text":"unknown");
            ::fprintf(m_file, "\n");
        }
    }
    else
    {
        ::fprintf(m_file, "[%2d] ", track);
        ::fprintf(m_file, "%-25s ", title);
        if ( request_type != 0x01 )
            ::fprintf(m_file, "   request_type: 0x%02X ", request_type);
        ::fprintf(m_file, "\n");
    }

    return false;
}

bool
Juken::CDTextDataReply(short slot, byte track, byte request_type,
                       byte formatting, char* title)
{
    if ( title[0] == 0x01 )
        title[0] = '\0';

    // output the reply
    if ( track == 0 )
    {
        if ( m_loading_titles )
        {
            m_titles[slot] = strdup(title);
            ::fprintf(m_file, "#");
            ::fflush(m_file);
        }
        else
        {
            ::fprintf(m_file, "%-25s ", title);
        }
    }
    else
    {
        ::fprintf(m_file, "[%3d] ", track);
        ::fprintf(m_file, "%-25s ", title);
    }

/*
    ::fprintf(m_file, "unknown : ");
    ::fprintf(m_file, "0x%02X ", unknown_1);
    ::fprintf(m_file, "0x%02X ", unknown_2);
    ::fprintf(m_file, "0x%02X ", unknown_3);
    ::fprintf(m_file, "\n");
*/

    return false;
}

bool
Juken::DiscTrackListReply(int num_tracks, DiscTrack* info)
{
    return false;
}

// ------------------------------- Command Handling ----------------------------------
void
Juken::DoCommand(char* line)
{
    // build up the argc, argv
    int argc = 0;
    char** argv = ::buildargv(line);
    while ( argv[argc] != NULL ) argc++;

    // find the command function corresponding to argv[0]
    int i = 0;
    while ( m_commands[i].name != NULL )
    {
        if ( strcmp(argv[0], m_commands[i].name) == 0 )
        {
            // execute the command function
            m_commands[i].func(this, argc, argv);
            break;
        }
        i++;
    }

    i = 0;
    while ( m_short_commands[i].name != NULL )
    {
        if ( strcmp(argv[0], m_short_commands[i].name) == 0 )
        {
            // execute the command function
            m_short_commands[i].func(this, argc, argv);
            break;
        }
        i++;
    }

    ::freeargv(argv);
}

void
Juken::DoHelp(Juken* _this, int argc, char* argv[])
{
    int i = 0;
    while ( m_commands[i].name != NULL )
    {
        ::fprintf(stdout, "\t%s\t%s\n", m_commands[i].name, m_commands[i].help);
        i++;
    }
}

void
Juken::DoExport(Juken* _this, int argc, char* argv[])
{
    CDChanger* changer = ((CDChanger*) _this->m_changer);
    char* dir = "/var/juken/db/";
    short start = 1;
    short end = _this->m_capacity;

    if ( argc > 1 )
        dir = argv[1];
    if ( argc > 2 )
        start = atoi(argv[2]);
    if ( argc > 3 )
        end = atoi(argv[3]);

    char** titles = _this->m_titles;

    for (short slot=start; slot<=end; slot++)
    {
        if ( titles[slot] == NULL )
            continue;

        if ( _this->m_cur_slot != slot )
        {
usleep(10); // an ugly hack, but I can't figure out how/why/when the player isn't "ready"
            changer->DoChangeDisc(slot, Stopped);

            changer->DoEvent();
            changer->DoEvent();
            changer->DoEvent();
            changer->DoEvent();
            changer->DoEvent();
        }

usleep(10); // an ugly hack, but I can't figure out how/why/when the player isn't "ready"
        uint discid = changer->GetDiscId(slot);

        ExportListener export_listener(discid, dir);
        changer->pushListener(&export_listener);
usleep(10); // an ugly hack, but I can't figure out how/why/when the player isn't "ready"

        changer->DoListContents(slot);
        changer->popListener();
    }
}

void
Juken::DoList(Juken* _this, int argc, char* argv[])
{
    CDChanger* changer = ((CDChanger*) _this->m_changer);
    if ( argc > 1 )
    {
        int slot = atoi(argv[1]);
        changer->DoListContents(slot);
    }
    else
    {
        changer->DoListDiscs();
    }
}

void
Juken::GetBests(Juken* _this, int argc, char* argv[])
{
    CDChanger* changer = ((CDChanger*) _this->m_changer);
    changer->DoListBest();
}

void
Juken::DoChangeDisc(Juken* _this, int argc, char* argv[])
{
    CDChanger* changer = ((CDChanger*) _this->m_changer);
    int slot = 0;
    if ( argc > 1 )
    {
        slot = atoi(argv[1]);
        changer->DoChangeDisc(slot, Stopped); //Playing);
    }
}

void
Juken::DoPlay(Juken* _this, int argc, char* argv[])
{
    CDChanger* changer = ((CDChanger*) _this->m_changer);
    changer->DoPlayPause();
}

void
Juken::DoPrev(Juken* _this, int argc, char* argv[])
{
    CDChanger* changer = ((CDChanger*) _this->m_changer);
    changer->DoPrev();
}

void
Juken::DoNext(Juken* _this, int argc, char* argv[])
{
    CDChanger* changer = ((CDChanger*) _this->m_changer);
    changer->DoNext();
}

void
Juken::DoStop(Juken* _this, int argc, char* argv[])
{
    CDChanger* changer = ((CDChanger*) _this->m_changer);
    changer->DoStop();
}

void
Juken::DoQuit(Juken* _this, int argc, char* argv[])
{
    _this->m_done = true;
}

// ------------------------------ Readline Callbacks ---------------------------------
void
Juken::readline_callback(char* line)
{
    if ( line && line[0] )
    {
#ifdef HAVE_READLINE_HISTORY
        // add line to history list
        add_history(line);
#endif
        // process a command
        g_juken->DoCommand(line);
    }
}

char**
Juken::completion_callback(const char* text, int start, int end)
{
    char** matches = NULL;

    // generate command or parameter completions
    /*
    if (start == 0)
        matches = rl_completion_matches(text, &Juken::command_generator);
    else
        matches = rl_completion_matches(text, &Juken::param_generator);
    */

    /*
    if ( matches != NULL )
    {
        int i = 0;
        while ( matches[i] != NULL )
            printf("%s\n", matches[i++]);
    }
    */

    return matches;
}


