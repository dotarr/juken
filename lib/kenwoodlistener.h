#ifndef JUKEN_KENWOODLISTENER_H
#define JUKEN_KENWOODLISTENER_H

#include "types.h"
#include "payload.h"

class KenwoodListener 
{
    public:
        ~KenwoodListener() { };

        virtual bool InfoChanged(short slot, byte track, enum mode mode, 
                                 enum random random, bool repeat, 
                                 byte userfile) { return false; };
        virtual bool StateChanged(enum state state) { return false; };
        virtual bool DiscChanged(short slot) { return false; };
        virtual bool DoorChanged(bool door_closed) { return false; };
        
        virtual bool DiscDataReply(short slot, byte track, byte userfiles, 
                                   byte request_type, byte genre, 
                                   byte formatting, char* title) 
            { return false; };
        virtual bool CDTextDataReply(short slot, byte track, byte request_type,
                                     byte formatting, char* title) 
            { return false; };
        virtual bool DiscTrackListReply(int num_tracks, DiscTrack* info) 
            { return false; };

    protected:
        KenwoodListener() { };

    private:
};

#endif /* JUKEN_KENWOODLISTENER_H */
