#include <signal.h>

#include <common.h>

#include <kenwooddevice.h>
#include <kenwoodchanger.h>

#include "commands.h"
#include "consolelistener.h"

char* serial_device = "/dev/juken";

bool done = false;

void
parse_env()
{
    char* dev = getenv("JUKEN_DEV");
    if ( dev != NULL )  serial_device = dev;
}

void
parse_args(int argc, char* argv[])
{
    char* short_opts = "vhd:";

#ifdef HAVE_GETOPT_LONG
    struct option long_opts[] = {
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
            case 'v':
                ::fprintf(stdout, "%d.%d.%d build: %d\n", 0, 1, 0, 1);
                exit(EXIT_SUCCESS);
            case 'h':
                ::fprintf(stdout, "usage: jukebox {-v|--version|-h|--help} <device>\n");
                ::fprintf(stdout, "\t-v, --version\tdisplay version information\n");
                ::fprintf(stdout, "\t-h, --help\tdisplay this message\n");
                ::fprintf(stdout, "\t<device>\tthe serial device to use (defaults to /dev/juken)\n");
                exit(EXIT_SUCCESS);
            case ':': ::fprintf(stderr, "missing parameter\n"); break;
            case '?': ::fprintf(stderr, "unknown option\n");    break;
        }
    }

    if ( optind < argc )
        serial_device = argv[optind];
}

void
INThandler(int sig)
{
    done = true;
    ::fprintf(stderr, "ctrl-c hit\n");
}

void
printPrompt(const KenwoodChanger& changer)
{
    //printf("disc %d # ", changer.getCurrentSlot()); 
    printf("# ", changer.getCurrentSlot()); 
    fflush(stdout);
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
        KenwoodDevice device(serial_device);
        ConsoleListener listener(stdout);
        KenwoodChanger changer(device, &listener);

        int juke_fd = device.GetFileDescriptor();
        fd_set fds;
        while ( !done )
        {
            if ( changer.isReady() )
                printPrompt(changer);

            // setup fd set
            FD_ZERO(&fds);
            FD_SET(juke_fd, &fds);
            FD_SET(STDIN_FILENO, &fds);
            
            // select for something to do
            int num_fds = ::select(juke_fd+1, &fds, NULL, NULL, NULL);
            if ( num_fds == -1 )
                done = true;

            // if we have something to do ...
            if ( num_fds > 0 )
            {
                if ( FD_ISSET(juke_fd, &fds) )
                {
                    if ( changer.isReady() )
                        printf("\n");
                    changer.DoEvent();
                }
                if ( FD_ISSET(STDIN_FILENO, &fds) )
                {
                    DoCommand(changer);
                }
            }
        }
    }
    catch(char* e)
    {
        // output the error
        ::fprintf(stderr, "exception caught: %s\n", e);

        // exit with failure
        return EXIT_FAILURE;
    }

    // return with success
    return EXIT_SUCCESS;
}

