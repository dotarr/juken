#ifndef JUKEN_CONSOLELISTENER_H
#define JUKEN_CONSOLELISTENER_H

#include <kenwoodlistener.h>

class ConsoleListener : public KenwoodListener
{
    public:
        ConsoleListener(FILE* f) :m_fd(f) { };

        virtual void Handshake(const char* id);
        virtual void InfoChanged(short slot, byte track, enum mode mode, 
                                 enum random random, bool repeat, 
                                 byte userfile);
        virtual void StateChanged(enum state state);
        virtual void DiscChanged(short slot);
        virtual void DoorChanged(bool door_closed);
        
        virtual void DiscDataReply(DiscData* info);
        virtual void CDTextDataReply(CDTextData* info);
        virtual void TrackTimesReply(TrackTimes* info);
        virtual void DiscTrackListReply(DiscTrackList* info);

    protected:

    private:
        FILE* m_fd; // the file too fprintf to
};

#endif /* JUKEN_CONSOLELISTENER_H */
