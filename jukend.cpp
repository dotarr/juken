#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "kenwooddevice.h"
#include "kenwoodchanger.h"
#include "consolelistener.h"

#if 0
char* serial_device = "/dev/ttyS0";
#endif
char* serial_device = "/dev/tty00";

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
#if 0
    struct option long_opts[] = {
        { "version", no_argument, NULL, 'v' },
        { "help", no_argument, NULL, 'h' },
        { NULL, no_argument, NULL, 0 }
    };
#endif

    int c = EOF;
#if 0
    while( (c=::getopt_long(argc, argv, short_opts, long_opts, NULL)) != EOF )
#endif
    while( (c=::getopt(argc, argv, short_opts)) != EOF )
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
                ::fprintf(stdout, "\t<device>\tthe serial device to use (defaults to /dev/ttyS0)\n");
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
DoQuit(KenwoodChanger& changer, int argc, char* argv[])
{
    done = true;
}

void
DoHelp(KenwoodChanger& changer, int argc, char* argv[])
{
    ::fprintf(stdout, "\tquit\tquit the application\n");
    ::fprintf(stdout, "\thelp\tthis help output\n");
    ::fprintf(stdout, "\tlist\tlist the changer contents\n");
}

void
disc_print(ushort reply_len, byte* reply_data)
{
    // cast the reply
    data_0xFE_a* info = (data_0xFE_a*) reply_data;

    // add null terminator to title
    char title[MAX_TITLE_LENGTH+1];
    byte data_len = 7; //sizeof(data_0xFE_a)-MAX_TITLE_LENGTH-1;
    byte title_len = 0;
    if ( info->title[0] != 0x01 )
    {
        title_len = reply_len-data_len;
        ::strncpy(title, info->title, title_len);
        title[title_len] = '\0';
    }
    else
    {
        title[0] = '\0';
    }

    // output the reply
    printf("[%3d] ", info->slot);
    printf("%-25s ", title);
    if ( info->genre != UNKNOWN )
        printf("genre: %-22s ", GENRE_NAMES[info->genre]);
    if ( info->userfiles != 0x00 )
        printf(" userfiles: 0x%02X ", info->userfiles);
    if ( info->formatting != 0x00 )
        printf("formatting: %s ", (info->formatting==0x13)?"cd-text":"unknown");
    if ( info->request_type != 0x00 )
        printf("   request_type: 0x%02X ", info->request_type);
    printf("\n");
}

void
track_print(ushort reply_len, byte* reply_data)
{
    // cast the reply
    data_0xFE_a* info = (data_0xFE_a*) reply_data;

    // add null terminator to title
    char title[MAX_TITLE_LENGTH+1];
    byte data_len = 7; //sizeof(data_0xFE_a)-MAX_TITLE_LENGTH-1;
    byte title_len = 0;
    if ( info->title[0] != 0x01 )
    {
        title_len = reply_len-data_len;
        ::strncpy(title, info->title, title_len);
        title[title_len] = '\0';
    }
    else
    {
        title[0] = '\0';
    }

    // output the reply
    if ( info->track == 0 )
    {
        printf("%-25s ", title);
        if ( info->genre != UNKNOWN )
            printf("genre: %-22s ", GENRE_NAMES[info->genre]);
        if ( info->userfiles != 0x00 )
            printf(" userfiles: 0x%02X ", info->userfiles);
        if ( info->formatting != 0x00 )
            printf("formatting: %s ", (info->formatting==0x13)?"cd-text":"unknown");
        if ( info->request_type != 0x01 )
            printf("   request_type: 0x%02X ", info->request_type);
        printf("\n");
    }
    else
    {
        printf("[%3d] ", info->track);
        printf("%-25s ", title);
        if ( info->request_type != 0x01 )
            printf("   request_type: 0x%02X ", info->request_type);
        printf("\n");
    }
}

void
DoList(KenwoodChanger& changer, int argc, char* argv[])
{
    int slot = 0;
    if ( argc > 1 )
    {
        slot = atoi(argv[1]);
        changer.DoListTracks(slot, track_print);
    }
    else
    {
        changer.DoListDiscs(slot, disc_print);
    }
}

void
DoPlay(KenwoodChanger& changer, int argc, char* argv[])
{
    changer.DoPlayPause();
}

void
DoStop(KenwoodChanger& changer, int argc, char* argv[])
{
    changer.DoStop();
}


typedef void (*cmd_func)(KenwoodChanger& changer, int argc, char* argv[]);
typedef struct cmd { char* name; cmd_func func; };

struct cmd commands[] = 
{
    { "quit",   DoQuit },
    { "exit",   DoQuit },
    { "q",      DoQuit },
    { "help",   DoHelp },
    { "h",      DoHelp },
    { "list",   DoList },
    { "ls",     DoList },
    { "play",   DoPlay },
    { "pause",  DoPlay },
    { "stop",   DoStop },
    { "", NULL }
};

void
DoCommand(KenwoodChanger& changer)
{
    // readthe line from stdin
    char line[256];
    ::fgets(line, 255, stdin);

    // if it is empty then just return
    if ( line[0] == '\n' ) return;

    // terminate the string and strip any carriage return
    line[255] = 0;
    int l = ::strlen(line);
    if ( line[l-1] == '\n' ) line[l-1] = 0;

    // build up the argc, argv
    int argc = 0;
    char** argv = new char*[16];
    argv[argc] = strtok(line, " ");
    while ( argv[argc] != NULL )
        argv[++argc] = strtok(NULL, " ");

    // find the command function corresponding to argv[0]
    int i = 0;
    while ( commands[i].func != NULL )
    {
        if ( strcmp(argv[0], commands[i].name) == 0 )
        {
            // execute the command function
            commands[i].func(changer, argc, argv);
            break;
        }
        i++;
    }

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
        ConsoleListener listener;
        KenwoodChanger changer(device, listener);

        int juke_fd = device.GetFileDescriptor();
        fd_set fds;
        while ( !done )
        {
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
                    changer.DoEvent();
                if ( FD_ISSET(STDIN_FILENO, &fds) )
                    DoCommand(changer);
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

