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
        
        virtual bool DiscDataReply(DiscData* info) { return false; };
        virtual bool CDTextDataReply(CDTextData* info) { return false; };
        virtual bool TrackTimesReply(TrackTimes* info) { return false; };
        virtual bool DiscTrackListReply(DiscTrackList* info) { return false; };

    protected:
        KenwoodListener() { };

    private:
};

#endif /* JUKEN_KENWOODLISTENER_H */
