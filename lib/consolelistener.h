#ifndef __CONSOLELISTENER_H__
#define __CONSOLELISTENER_H__

#include "kenwoodlistener.h"

class ConsoleListener : public KenwoodListener
{
    public:
        ConsoleListener() { };

        virtual void InfoChanged(short slot, byte track, enum mode mode, 
                                 enum random random, bool repeat, 
                                 byte userfile);
        virtual void StateChanged(enum state state);
        virtual void DiscChanged(short slot);
        virtual void DoorChanged(bool door_closed);
        
    protected:

    private:
};

#endif /* __CONSOLELISTENER_H__ */
