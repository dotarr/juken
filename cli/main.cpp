#include <signal.h>

#include <common.h>

#include "juken.h"

char* serial_device = "/dev/juken";

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
                ::fprintf(stdout, "usage: jukebox {-d <device>|--device <device>|-v|--version|-h|--help}\n");
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
    ::fprintf(stderr, "ctrl-c hit\n");
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
        Juken juken(serial_device);
        juken.InitState();
        juken.Run();
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

