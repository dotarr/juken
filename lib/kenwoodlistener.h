#ifndef JUKEN_KENWOODLISTENER_H
#define JUKEN_KENWOODLISTENER_H

#include "types.h"

class KenwoodListener 
{
    public:
        virtual void InfoChanged(short slot, byte track, enum mode mode, 
                                 enum random random, bool repeat, 
                                 byte userfile) = 0;
        virtual void StateChanged(enum state state) = 0;
        virtual void DiscChanged(short slot) = 0;
        virtual void DoorChanged(bool door_closed) = 0;
        
    protected:

    private:
};

#endif /* JUKEN_KENWOODLISTENER_H */
