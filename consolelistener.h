#ifndef __CONSOLELISTENER_H__
#define __CONSOLELISTENER_H__

#include "kenwoodlistener.h"

class ConsoleListener : public KenwoodListener
{
    public:
        ConsoleListener() { };

        virtual void InfoChanged(short slot, byte track, byte num_tracks, 
                    byte mode, byte userfiles, byte userfile_mode);
        virtual void StateChanged(enum state state);
        virtual void DiscChanged(short slot);
        virtual void DoorChanged(bool door_closed);
        
    protected:

    private:
};

#endif /* __CONSOLELISTENER_H__ */
