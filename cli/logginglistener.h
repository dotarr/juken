#ifndef JUKEN_LOGGING_LISTENER_H
#define JUKEN_LOGGING_LISTENER_H

#include "consolelistener.h"

class LoggingListener : public ConsoleListener
{
    public:
        LoggingListener() : ConsoleListener(stderr) { }
        ~LoggingListener() { }

        void InfoChanged(KenwoodChanger* changer, short slot, byte title, short chapter);
        void ModeChanged(KenwoodChanger* changer, enum mode mode, bool repeat, byte param);
        void StateChanged(KenwoodChanger* changer, enum state state);
        void DoorChanged(KenwoodChanger* changer, bool door_open);

        void ProgressStart(KenwoodChanger* changer, enum operation op, int length);
        void Progress(KenwoodChanger* changer, enum operation op, int progress);
        void ProgressEnd(KenwoodChanger* changer, enum operation op);
};

#endif /* JUKEN_LOGGING_LISTENER_H */
