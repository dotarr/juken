#include <common.h>

#include "commands.h"
#include "consolelistener.h"
#include "exportlistener.h"

#include <readline/readline.h>

BEGIN_C_DECLS
extern char** buildargv(char *);
extern void freeargv(char **);
END_C_DECLS


struct cmd short_commands[] = 
{
    { "q",      DoQuit, "quit the application" },
    { "h",      DoHelp, "this help output" },
    { "ls",     DoList, "list the changer contents" },
    { "lsx",    DoExperiment, "experimental" },
    { "cd",     DoChangeDisc, "change the current disc" },
    { "p",      DoPlay, "play/pause the current disc" },
    { "s",      DoStop, "stop the current disc" },
    { NULL, NULL }
};

struct cmd commands[] = 
{
    { "quit",   DoQuit, "quit the application" },
    { "exit",   DoQuit, "quit the application" },
    { "help",   DoHelp, "this help output" },
    { "export", DoExport, "export the changer contents" },
    { "list",   DoList, "list the changer contents" },
    { "times",  GetTimes, "list the current discs track start times" },
    { "bests",  GetBests, "list the best tracks" },
    { "change", DoChangeDisc, "change the current disc" },
    { "play",   DoPlay, "play the current disc" },
    { "pause",  DoPlay, "pause the current disc" },
    { "prev",   DoPrev, "play the previous track on the current disc" },
    { "next",   DoNext, "play the next track on the current disc" },
    { "stop",   DoStop, "stop the current disc" },
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
    char* dir = "/var/juken/";
    if ( argc > 1 )
    {
        dir = argv[1];
    }

    ExportListener export_listener(dir);
    changer.pushListener(&export_listener);

    extern ConsoleListener* g_listener;
    char** titles = g_listener->getTitles();
    short capacity = g_listener->getCapacity();

    for (short slot=1; slot<=capacity; slot++)
    {
        if ( titles[slot] == NULL )
            continue;

        if ( g_listener->getCurSlot() != slot )
        {
            changer.DoChangeDisc(slot, Stopped);

            changer.DoEvent();
            changer.DoEvent();
            changer.DoEvent();
            changer.DoEvent();
            changer.DoEvent();
        }

usleep(10); // an ugly hack, but I can't figure out how/why/when the player isn't "ready"
        changer.DoListTrackTimes(slot);
usleep(10); // an ugly hack, but I can't figure out how/why/when the player isn't "ready"
        changer.DoListTracks(slot);
usleep(10); // an ugly hack, but I can't figure out how/why/when the player isn't "ready"
    }
    changer.popListener();
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
    int slot = 0;
    if ( argc > 1 )
        slot = atoi(argv[1]);
     changer.DoListTrackTimes(slot);
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
        changer.DoChangeDisc(slot, Stopped); //Playing);
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
    extern bool g_done; // exit flag from juken-cli.cpp
    g_done = true;
}

void
DoCommand(KenwoodChanger& changer, char* line)
{
    // build up the argc, argv
    int argc = 0;
    char** argv = ::buildargv(line);
    while ( argv[argc] != NULL ) argc++;

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

    i = 0;
    while ( short_commands[i].name != NULL )
    {
        if ( strcmp(argv[0], short_commands[i].name) == 0 )
        {
            // execute the command function
            short_commands[i].func(changer, argc, argv);
            break;
        }
        i++;
    }

    ::freeargv(argv);
}


