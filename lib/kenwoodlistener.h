#ifndef JUKEN_KENWOODLISTENER_H
#define JUKEN_KENWOODLISTENER_H

#include "types.h"
#include "payload.h"

class KenwoodListener 
{
    public:
        virtual void Handshake(const char* id) = 0;
        virtual void InfoChanged(short slot, byte track, enum mode mode, 
                                 enum random random, bool repeat, 
                                 byte userfile) = 0;
        virtual void StateChanged(enum state state) = 0;
        virtual void DiscChanged(short slot) = 0;
        virtual void DoorChanged(bool door_closed) = 0;
        
        virtual void DiscDataReply(DiscData* info) = 0;
        virtual void CDTextDataReply(CDTextData* info) = 0;
        virtual void TrackTimesReply(TrackTimes* info) = 0;
        virtual void DiscTrackListReply(DiscTrackList* info) = 0;

    protected:

    private:
};

#endif /* JUKEN_KENWOODLISTENER_H */
