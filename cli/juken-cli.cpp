#include <signal.h>

#include <common.h>

#include <kenwooddevice.h>
#include <kenwoodchanger.h>

#include "commands.h"
#include "consolelistener.h"

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

char* serial_device = "/dev/juken";

bool g_done = false;
KenwoodChanger* g_changer = NULL;
ConsoleListener* g_listener = NULL;

void
parse_env()
{
    char* dev = getenv("JUKEN_DEV");
    if ( dev != NULL )  serial_device = dev;
}

void
parse_args(int argc, char* argv[])
{
    char* short_opts = "d:vh";

#ifdef HAVE_GETOPT_LONG
    struct option long_opts[] = {
        { "device", required_argument, NULL, 'd' },
        { "version", no_argument, NULL, 'v' },
        { "help", no_argument, NULL, 'h' },
        { NULL, no_argument, NULL, 0 }
    };
#endif

    int c = EOF;
#ifdef HAVE_GETOPT_LONG
    while( (c=::getopt_long(argc, argv, short_opts, long_opts, NULL)) != EOF )
#else
    while( (c=::getopt(argc, argv, short_opts)) != EOF )
#endif
    {
        switch( c )
        {
            case 'd':
                serial_device = optarg;
                ::fprintf(stdout, "using serial device: %s\n", serial_device);
                break;
            case 'v':
                ::fprintf(stdout, "%d.%d.%d build: %d\n", 0, 1, 0, 1);
                exit(EXIT_SUCCESS);
            case 'h':
                ::fprintf(stdout, "usage: jukebox {-v|--version|-h|--help} <device>\n");
                ::fprintf(stdout, "\t-d, --device <device>\tthe serial device to use (defaults to /dev/juken)\n");
                ::fprintf(stdout, "\t-v, --version\t\tdisplay version information\n");
                ::fprintf(stdout, "\t-h, --help\t\tdisplay this message\n");
                //::fprintf(stdout, "\t<command>\t\ta command to execute\n");
                exit(EXIT_SUCCESS);
            case ':': ::fprintf(stderr, "missing parameter\n"); break;
            case '?': ::fprintf(stderr, "unknown option\n");    break;
        }
    }

    if ( optind < argc )
    {
        // treat remaining as a "command"
    }
}

void
INThandler(int sig)
{
    g_done = true;
    ::fprintf(stderr, "ctrl-c hit\n");
}

void
readline_callback(char* line)
{
    if ( line && line[0] )
    {
#ifdef HAVE_READLINE_HISTORY
        // add line to history list
        add_history(line);
#endif
        // process a command
        DoCommand(*g_changer, line);
    }
}

char*
command_generator(const char* text, int state)
{
    // generate a command completion
    static int i;
    static int len;

    if ( state == 0 )
    {
        i = 0;
        len = strlen(text);
    }

    while ( commands[i].name != NULL )
    {
        char* name = commands[i++].name;
        if ( strncmp(name, text, len) == 0 )
            return strdup(name);
    }

    if ( !state )
        rl_attempted_completion_over = true;
    return NULL;
}

char*
param_generator(const char* text, int state)
{
    // generate a parameter completion
    /*
    static int i;
    static int len;

    if ( state == 0 )
    {
        i = 0;
        len = strlen(text);
    }

    char** titles = g_listener->getTitles();
    if ( titles != NULL )
    {
        while ( i < g_listener->getCapacity() )
        {
            char* title = titles[i++];
            if ( title!=NULL && strncmp(title, text, len)==0 )
                return strdup(title);
        }
    }
    */

    if ( !state )
        rl_attempted_completion_over = true;
    return NULL;
}

char**
completion_callback(const char* text, int start, int end)
{
    char** matches = NULL;

    // generate command or parameter completions
    if (start == 0)
        matches = rl_completion_matches(text, command_generator);
    else
        matches = rl_completion_matches(text, param_generator);

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

int
main(int argc, char* argv[])
{
    // install a ctrl-c signal handler
    //signal(SIGINT, INThandler);

    // parse parameters
    parse_env();
    parse_args(argc, argv);

    try
    {
        // create and handshake with device
        KenwoodDevice device(serial_device);
        char* id = device.DoHandshake("I'm PC");
        ::fprintf(stdout, "connection established to %s\n", id+4);

        // create appropriate changer 
        KenwoodChanger changer(device);
        g_changer = &changer;

        // add a listener that prints events and data to stdout
        ConsoleListener listener(stdout, 200);
        g_listener = &listener;
        changer.pushListener(&listener);
        
        // setup readline callbacks
        rl_callback_handler_install("juken> ", readline_callback);
        rl_attempted_completion_function = completion_callback;

        int juke_fd = device.GetFileDescriptor();
        fd_set fds;
        while ( !g_done )
        {
            // setup fd set
            FD_ZERO(&fds);
            FD_SET(juke_fd, &fds);
            FD_SET(STDIN_FILENO, &fds);
            
            // select for something to do
            int num_fds = ::select(juke_fd+1, &fds, NULL, NULL, NULL);
            if ( num_fds == -1 )
                g_done = true;

            // if we have something to do ...
            if ( num_fds > 0 )
            {
                if ( FD_ISSET(juke_fd, &fds) )
                {
                    // do an event
                    rl_crlf();
                    changer.DoEvent();
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
    catch(char* e)
    {
        // cleanup globals and readline
        g_listener = NULL;
        g_changer = NULL;
        rl_crlf();
        rl_callback_handler_remove();

        // output the error
        ::fprintf(stderr, "exception caught: %s\n", e);

        // exit with failure
        return EXIT_FAILURE;
    }

    // cleanup globals and readline
    g_listener = NULL;
    g_changer = NULL;
    rl_crlf();
    rl_callback_handler_remove();

    // return with success
    return EXIT_SUCCESS;
}

