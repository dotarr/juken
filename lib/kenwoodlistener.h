#ifndef JUKEN_KENWOODLISTENER_H
#define JUKEN_KENWOODLISTENER_H

#include "types.h"
#include "payload.h"

class KenwoodListener 
{
    public:
        ~KenwoodListener() { };

        virtual bool InfoChanged(short slot, byte title, short chapter) 
            { return false; };
        virtual bool ModeChanged(enum mode mode, bool repeat, byte param) 
            { return false; };
        virtual bool StateChanged(enum state state) 
            { return false; };
        virtual bool DoorChanged(bool door_open) { return false; };
        
        virtual bool TextDataReply(short slot, byte track, byte userfiles, 
                                   byte request_type, byte genre, 
                                   byte formatting, char* title) 
            { return false; };

        virtual bool DiscTrackListReply(byte num_tracks, DiscTrack* tracks)
            { return false; };

    protected:
        KenwoodListener() { };

    private:
};

#endif /* JUKEN_KENWOODLISTENER_H */
