#ifndef __KENWOODLISTENER_H__
#define __KENWOODLISTENER_H__

#include "types.h"

class KenwoodListener 
{
    public:
        virtual void InfoChanged(short slot, byte track, byte num_tracks, 
                    byte mode, byte userfiles, byte userfile_mode) = 0;
        virtual void StateChanged(enum state state) = 0;
        virtual void DiscChanged(short slot) = 0;
        virtual void DoorChanged(bool door_closed) = 0;
        
    protected:

    private:
};

#endif /* __KENWOODLISTENER_H__ */
