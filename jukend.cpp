#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "lib/kenwooddevice.h"
#include "lib/kenwoodchanger.h"
#include "lib/consolelistener.h"

#ifdef BSD
char* serial_device = "/dev/tty00";
#else
char* serial_device = "/dev/ttyS0";
#endif

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

#ifdef HAS_GETOPT_LONG
    struct option long_opts[] = {
        { "version", no_argument, NULL, 'v' },
        { "help", no_argument, NULL, 'h' },
        { NULL, no_argument, NULL, 0 }
    };
#endif

    int c = EOF;
#ifdef HAS_GETOPT_LONG
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
#ifdef BSD
                ::fprintf(stdout, "\t<device>\tthe serial device to use (defaults to /dev/tty00)\n");
#else
                ::fprintf(stdout, "\t<device>\tthe serial device to use (defaults to /dev/ttyS0)\n");
#endif
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
    SelectPlayMode sdt;
    sdt.userfile = (byte)6;
    sdt.genre = (byte)0x10;
    done = true;
}

void
DoHelp(KenwoodChanger& changer, int argc, char* argv[])
{
    ::fprintf(stdout, "\tq|quit\tquit the application\n");
    ::fprintf(stdout, "\th|help\tthis help output\n");
    ::fprintf(stdout, "\tls|list\tlist the changer contents\n");
    ::fprintf(stdout, "\ttimes\tlist the current discs track start times\n");
    ::fprintf(stdout, "\tcd|change\tchange the current disc\n");
    ::fprintf(stdout, "\tp|play\tplay the current disc\n");
    ::fprintf(stdout, "\tP|pause\tpause the current disc\n");
    ::fprintf(stdout, "\ts|stop\tstop the current disc\n");
}

void
disc_print(byte reply_cmd, ushort reply_len, byte* reply_data)
{
    // cast the reply
    DiscData* info = (DiscData*) reply_data;

    if ( info->title[0] == 0x01 )
        info->title[0] = '\0';

    // output the reply
    printf("[%3d] ", info->slot);
    printf("%-25s ", info->title);
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
track_print(byte reply_cmd, ushort reply_len, byte* reply_data)
{
    if ( reply_cmd == 0xFE )
    {
        // cast the reply
        DiscData* info = (DiscData*) reply_data;

        if ( info->title[0] == 0x01 )
            info->title[0] = '\0';

        // output the reply
        if ( info->track == 0 )
        {
            printf("%-25s ", info->title);
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
            printf("%-25s ", info->title);
            if ( info->request_type != 0x01 )
                printf("   request_type: 0x%02X ", info->request_type);
            printf("\n");
        }
    }
    else if ( reply_cmd == 0xFD )
    {
        // cast the reply
        CDTextData* info = (CDTextData*) reply_data;

        if ( info->title[0] == 0x01 )
            info->title[0] = '\0';

        // output the reply
        if ( info->track == 0 )
        {
            printf("%-25s ", info->title);
        }
        else
        {
            printf("[%3d] ", info->track);
            printf("%-25s ", info->title);
        }

        printf("unknown : ");
        printf("0x%02X ", info->unknown_1);
        printf("0x%02X ", info->unknown_2);
        printf("0x%02X ", info->unknown_3);
        printf("\n");
    }
}

void
time_print(byte reply_cmd, ushort reply_len, byte* reply_data)
{
    // cast the reply
    TrackTimes* info = (TrackTimes*) reply_data;
    TimeInfo* times = (TimeInfo*) &(info->times);

    printf("disc: %3d ", info->slot);
    if ( info->formatting != 0x00 )
        printf("formatting: %s ", (info->formatting==0x13)?"cd-text":"unknown");
    printf("\n");
    // output the reply
    for (int i=0; i<info->num_tracks+1; i++)
    {
        printf("[%3d] ", i);
        printf("%02X:%02X:%02X ", times[i].minute, times[i].second, times[i].subsecond);
        printf("\n");
    }
}

extern void printdata(const byte data[], int count);
void
best_print(byte reply_cmd, ushort reply_len, byte* reply_data)
{
    ::fprintf(stdout, "%s cmd=%d len=%d\n", "best data", reply_cmd, reply_len);
    printdata(reply_data, reply_len);
}

void
DoList(KenwoodChanger& changer, int argc, char* argv[])
{
    if ( argc > 1 )
    {
        int slot = atoi(argv[1]);
        changer.DoListTracks(slot, track_print);
    }
    else
    {
        changer.DoListDiscs(disc_print);
    }
}

void
DoExperiment(KenwoodChanger& changer, int argc, char* argv[])
{
    int slot = 0;
    byte x = 1;
    if ( argc > 1 )
        slot = atoi(argv[1]);
    if ( argc > 2 )
        x = atoi(argv[2]);
    changer.DoListTracks(slot, track_print, x);
}

void
GetTimes(KenwoodChanger& changer, int argc, char* argv[])
{
    changer.DoListTrackTimes(time_print);
}

void
GetBests(KenwoodChanger& changer, int argc, char* argv[])
{
    changer.DoListBest(best_print);
}

void
DoChangeDisc(KenwoodChanger& changer, int argc, char* argv[])
{
    int slot = 0;
    if ( argc > 1 )
    {
        slot = atoi(argv[1]);
        changer.DoChangeDisc(slot);
    }
}

void
DoPlay(KenwoodChanger& changer, int argc, char* argv[])
{
    changer.DoPlayPause();
}

void
DoNext(KenwoodChanger& changer, int argc, char* argv[])
{
    changer.DoNextTrack();
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
    { "lsx",    DoExperiment },
    { "times",  GetTimes },
    { "bests",  GetBests},
    { "change", DoChangeDisc },
    { "cd",     DoChangeDisc },
    { "play",   DoPlay },
    { "pause",  DoPlay },
    { "next",   DoNext},
    { "p",      DoPlay },
    { "stop",   DoStop },
    { "s",      DoStop },
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
        ConsoleListener listener;
        KenwoodChanger changer(device, listener);

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

