#include <common.h>

#include "commands.h"

extern bool done; // exit flag from juken-cli.cpp

typedef void (*cmd_func)(KenwoodChanger& changer, int argc, char* argv[]);
typedef struct cmd { char* name; cmd_func func; char* help; };

struct cmd commands[] = 
{
    { "quit",   DoQuit, "quit the application" },
    { "exit",   DoQuit, "quit the application" },
    { "q",      DoQuit, "quit the application" },
    { "help",   DoHelp, "this help output" },
    { "h",      DoHelp, "this help output" },
    { "export", DoExport, "export the changer contents" },
    { "list",   DoList, "list the changer contents" },
    { "ls",     DoList, "list the changer contents" },
    { "lsx",    DoExperiment, "experimental" },
    { "times",  GetTimes, "list the current discs track start times" },
    { "bests",  GetBests, "list the best tracks" },
    { "change", DoChangeDisc, "change the current disc" },
    { "cd",     DoChangeDisc, "change the current disc" },
    { "play",   DoPlay, "play the current disc" },
    { "pause",  DoPlay, "pause the current disc" },
    { "prev",   DoPrev, "play the previous track on the current disc" },
    { "next",   DoNext, "play the next track on the current disc" },
    { "p",      DoPlay, "play/pause the current disc" },
    { "stop",   DoStop, "stop the current disc" },
    { "s",      DoStop, "stop the current disc" },
    { NULL, NULL }
};

void
DoHelp(KenwoodChanger& changer, int argc, char* argv[])
{
    int i = 0;
    while ( commands[i].name != NULL )
    {
        ::fprintf(stdout, "\t%s\t%s\n", commands[i].name, commands[i].help);
        i++;
    }
}

void
DoExport(KenwoodChanger& changer, int argc, char* argv[])
{
}

void
DoList(KenwoodChanger& changer, int argc, char* argv[])
{
    if ( argc > 1 )
    {
        int slot = atoi(argv[1]);
        changer.DoListTracks(slot);
    }
    else
    {
        changer.DoListDiscs();
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
    changer.DoListTracks(slot, x);
}

void
GetTimes(KenwoodChanger& changer, int argc, char* argv[])
{
    changer.DoListTrackTimes();
}

void
GetBests(KenwoodChanger& changer, int argc, char* argv[])
{
    changer.DoListBest();
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
DoPrev(KenwoodChanger& changer, int argc, char* argv[])
{
    changer.DoPrevTrack();
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
    while ( commands[i].name != NULL )
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


