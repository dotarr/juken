#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <kenwoodchanger.h>

void DoQuit(KenwoodChanger& changer, int argc, char* argv[]);
void DoHelp(KenwoodChanger& changer, int argc, char* argv[]);
void DoList(KenwoodChanger& changer, int argc, char* argv[]);
void DoExperiment(KenwoodChanger& changer, int argc, char* argv[]);
void GetTimes(KenwoodChanger& changer, int argc, char* argv[]);
void GetBests(KenwoodChanger& changer, int argc, char* argv[]);
void DoChangeDisc(KenwoodChanger& changer, int argc, char* argv[]);
void DoPlay(KenwoodChanger& changer, int argc, char* argv[]);
void DoNext(KenwoodChanger& changer, int argc, char* argv[]);
void DoStop(KenwoodChanger& changer, int argc, char* argv[]);
 
void DoCommand(KenwoodChanger& changer);

#endif /* __COMMANDS_H__ */
