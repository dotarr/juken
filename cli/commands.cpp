#include <common.h>

#include "commands.h"

extern bool done; // exit flag from juken-cli.cpp

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
    { "bests",  GetBests },
    { "change", DoChangeDisc },
    { "cd",     DoChangeDisc },
    { "play",   DoPlay },
    { "pause",  DoPlay },
    { "next",   DoNext },
    { "p",      DoPlay },
    { "stop",   DoStop },
    { "s",      DoStop },
    { "", NULL }
};

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

void
DoQuit(KenwoodChanger& changer, int argc, char* argv[])
{
    done = true;
}

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


