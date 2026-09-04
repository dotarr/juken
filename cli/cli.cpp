#include <signal.h>

#include <common.h>
#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
#endif

#include "juken.h"

int g_log_level = 1;

void 
usage(char* prog_name)
{
    ::fprintf(stdout, "usage: %s [option] device\n", prog_name);
    ::fprintf(stdout, "options:\n");
#ifdef HAVE_GETOPT_LONG
    ::fprintf(stdout, "\t-v, --version\tdisplay version information\n");
    ::fprintf(stdout, "\t-h, --help\tdisplay this message\n");
    ::fprintf(stdout, "\t-l, --loglevel\tlogging level\n");
#else
    ::fprintf(stdout, "\t-v\t\tdisplay version information\n");
    ::fprintf(stdout, "\t-h\t\tdisplay this message\n");
    ::fprintf(stdout, "\t-l\t\tlogging level (defaults to 1)\n");
#endif
    ::fprintf(stdout, "device:\tthe device to use (defaults to %s)\n", SerialDevice::DefaultDevice);
}

void 
version()
{
    ::fprintf(stdout, "%d.%d.%d build: %d\n", 0, 1, 0, 1);
}

void
parse_args(int argc, char* argv[])
{
    char* short_opts = "vhl:";

#ifdef HAVE_GETOPT_LONG
    struct option long_opts[] = {
        { "version", no_argument, NULL, 'v' },
        { "help", no_argument, NULL, 'h' },
        { "loglevel", required_argument, NULL, 'l' },
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
            case 'l':
                g_log_level = ::atoi(optarg);
                break;
            case ':': ::fprintf(stderr, "missing parameter\n"); break;
            case '?': ::fprintf(stderr, "unknown option\n");    break;
        }
    }
}

int
main(int argc, char* argv[])
{
    int result = EXIT_FAILURE;

    // parse parameters
    parse_args(argc, argv);

    // open the logfile
    char log_filename[255];
    ::strcpy(log_filename, argv[0]);
    ::strcat(log_filename, ".log");
    OpenLog(log_filename, g_log_level);

    char* serial_device = (char*) SerialDevice::DefaultDevice;
    if ( optind < argc )
        serial_device = argv[optind++];

    if ( g_log_level > 0 )
        ::fprintf(stdout, "logging to %s at level %d\n", log_filename, g_log_level);

    try
    {
        Juken juken(serial_device);
        juken.InitState();
        juken.Run();
        result = EXIT_SUCCESS;
    }
    catch(char* e)
    {
        // output the error
        ::fprintf(stderr, "exception caught: %s\n", e);

        // exit with failure
        result = EXIT_FAILURE;
    }

    // close the logfile
    CloseLog();

    // return with success
    return result;
}

