#include <common.h>

#include <discid.h>
#include <cdchanger.h>
#include <dvdchanger.h>

#include "juken.h"

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
    { "id",     &Juken::DoId,         "get current discid" },
    { NULL, NULL }
};

struct Juken::cmd Juken::m_commands[] = 
{
    { "quit",   &Juken::DoQuit,       "quit the application" },
    { "exit",   &Juken::DoQuit,       "quit the application" },
    { "help",   &Juken::DoHelp,       "this help output" },
    { "list",   &Juken::DoList,       "list the changer contents" },
    { "change", &Juken::DoChangeDisc, "change the current disc" },
    { "play",   &Juken::DoPlay,       "play the current disc" },
    { "pause",  &Juken::DoPlay,       "pause the current disc" },
    { "prev",   &Juken::DoPrev,       "play the previous track on the current disc" },
    { "next",   &Juken::DoNext,       "play the next track on the current disc" },
    { "stop",   &Juken::DoStop,       "stop the current disc" },
    { "query",  &Juken::DoQuery,      "freeform query" },
    { NULL, NULL }
};

Juken::Juken(const char* serial_device)
: m_device(NULL), m_listener(NULL), m_changer(NULL), m_done(false)
{
    m_device = new KenwoodDevice(serial_device);
    g_juken = this;
}

Juken::~Juken()
{
    g_juken = NULL;

    rl_crlf();
    rl_callback_handler_remove();

    //::fclose(stdout);
    stdout = NULL;

    delete m_changer;
    m_changer = NULL;

    delete m_listener;
    m_listener = NULL;

    delete m_device;
    m_device = NULL;
}

void
Juken::InitState()
{
    // handshake with device
    char* id = m_device->DoHandshake("I'm PC");
    ::fprintf(stdout, "connection established to %s\n", id+4);
    LogMsg("connection established to %s\n", id+4);

    // make a listener
    m_listener = new ConsoleListener(stdout);
    
    // create appropriate changer 
    if ( ::strcmp(id, "I'm CD-425M") == 0 )
        m_changer = new CDChanger(id, *m_device, m_listener);
    else
        m_changer = new DVDChanger(id, *m_device, m_listener);
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

// -----------------------------------------------------------------------------------

void 
Juken::ListingDiscsCallback(void* context, Disc& data)
{
    Juken* _this = (Juken*) context;

    char* type = "";
    switch ( data.type )
    {
        case DISC_CD_A:   type = "cd";    break;
        case DISC_CD_MP3: type = "mp3";   break;
        case DISC_CD_V:   type = "vcd";   break;
        case DISC_DVD_A:  type = "dvd-a"; break;
        case DISC_DVD_V:  type = "dvd";   break;
    }
    ::fprintf(stdout, "[%3d] %-5s ", data.index, type);
    const char* title  = ((data.title==NULL) ? "" : data.title);
    if ( data.artist == NULL )
        ::fprintf(stdout, "%s\n", title);
    else
        ::fprintf(stdout, "%s:%s\n", title, data.artist);
}

void 
Juken::ListingDiscContentsCallback(void* context, Disc& data)
{
    Juken* _this = (Juken*) context;

    ListingDiscsCallback(context, data);
    if ( data.userfiles != 0 )
    {
        char* label = "userfiles:";
        byte uf = data.userfiles;
        for (int i=0; i<8; i++)
        {
            if ( uf&0x01 == 1 )
            {
                const char* name = _this->m_changer->getUserfileName(i);
                ::fprintf(stdout, "    %10s %s\n", label, name);
                label = "";
            }
            uf = uf>>1;
        }
    }
    if ( data.genre > UNASSIGNED )
        ::fprintf(stdout, "        genre: %s\n", GENRE_NAMES[data.genre]);
    for (NameList::iterator iter=data.tracks.begin(); iter!=data.tracks.end(); iter++)
    {
        Name& track = (*iter);
        const char* title = ((track.text==NULL) ? "" : track.text);
        ::fprintf(stdout,"  [%2d]%-21s\n", track.index, title);
    }
}

// ------------------------------- Command Handling ----------------------------------
void
Juken::DoCommand(char* line)
{
    LogMsg("DoCommand %s\n", line);

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
Juken::DoList(Juken* _this, int argc, char* argv[])
{
    KenwoodChanger* changer = _this->m_changer;
    if ( argc > 1 )
    {
        int slot = atoi(argv[1]);
        changer->DoListContents(slot, _this, &ListingDiscContentsCallback);
    }
    else
    {
        changer->DoListDiscs(_this, &ListingDiscsCallback);
    }
}

void
Juken::DoChangeDisc(Juken* _this, int argc, char* argv[])
{
    KenwoodChanger* changer = _this->m_changer;
    int slot = 0;
    if ( argc > 1 )
    {
        slot = atoi(argv[1]);
        changer->DoChangeDisc(slot);
    }
}

void
Juken::DoPlay(Juken* _this, int argc, char* argv[])
{
    KenwoodChanger* changer = _this->m_changer;
    changer->DoPlayPause();
}

void
Juken::DoPrev(Juken* _this, int argc, char* argv[])
{
    KenwoodChanger* changer = _this->m_changer;
    changer->DoPrev();
}

void
Juken::DoNext(Juken* _this, int argc, char* argv[])
{
    KenwoodChanger* changer = _this->m_changer;
    changer->DoNext();
}

void
Juken::DoStop(Juken* _this, int argc, char* argv[])
{
    KenwoodChanger* changer = _this->m_changer;
    changer->DoStop();
}

void
Juken::DoId(Juken* _this, int argc, char* argv[])
{
    KenwoodChanger* changer = _this->m_changer;
    if ( argc > 1 )
    {
        int slot = atoi(argv[1]);
        char* disc_id = changer->GetDiscId(slot);
        ::fprintf(stdout, "[%3d] id=%s\n", slot, disc_id);
    }
}

void
Juken::DoQuery(Juken* _this, int argc, char* argv[])
{
    DVDChanger* changer = (DVDChanger*) _this->m_changer;
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    int c = atoi(argv[3]);
    int slot = atoi(argv[4]);
    int title = atoi(argv[5]);
    int chapter = atoi(argv[6]);
    changer->DoQuery(a, b, c, slot, title, chapter);
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


