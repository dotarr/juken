#include <signal.h>

#include <common.h>

#include "juken.h"

void 
usage(char* prog_name)
{
#ifdef HAVE_GETOPT_LONG
    ::fprintf(stdout, "usage: %s {-v|--version|-h|--help} device\n", prog_name);
    ::fprintf(stdout, "\t-v, --version\tdisplay version information\n");
    ::fprintf(stdout, "\t-h, --help\tdisplay this message\n");
#else
    ::fprintf(stdout, "usage: %s {-v|-h} file\n", prog_name);
    ::fprintf(stdout, "\t-v\t\tdisplay version information\n");
    ::fprintf(stdout, "\t-h\t\tdisplay this message\n");
#endif
    ::fprintf(stdout, "\t<device>\tthe device to use (defaults to /dev/juken)\n");
}

void 
version()
{
    ::fprintf(stdout, "%d.%d.%d build: %d\n", 0, 1, 0, 1);
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
            case 'v':
                version();
                exit(EXIT_SUCCESS);
            case 'h':
                usage(argv[0]);
                exit(EXIT_SUCCESS);
            case ':': ::fprintf(stderr, "missing parameter\n"); break;
            case '?': ::fprintf(stderr, "unknown option\n");    break;
        }
    }
}

int
main(int argc, char* argv[])
{
    // parse parameters
    parse_args(argc, argv);
    char* serial_device = "/dev/juken";
    if ( optind == argc-1 )
        serial_device = argv[optind];
    else
    {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

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

