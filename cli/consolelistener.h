#ifndef JUKEN_CONSOLELISTENER_H
#define JUKEN_CONSOLELISTENER_H

#include <kenwoodlistener.h>

class ConsoleListener : public KenwoodListener
{
    public:
        ConsoleListener(FILE* f, short capacity);
        ~ConsoleListener();

        bool InfoChanged(short slot, byte track, enum mode mode, 
                         enum random random, bool repeat, 
                         byte userfile);
        bool StateChanged(enum state state);
        bool DiscChanged(short slot);
        bool DoorChanged(bool door_closed);
        
        bool DiscDataReply(DiscData* info);
        bool CDTextDataReply(CDTextData* info);
        bool TrackTimesReply(TrackTimes* info);
        bool DiscTrackListReply(DiscTrackList* info);

        short getCurSlot() { return m_cur_slot; }
        
        short getCapacity() { return m_capacity; }
        char** getTitles() { return m_titles; }

    protected:

    private:
        FILE* m_file; // the file too fprintf to

        short m_cur_slot;
        short m_capacity;
        char** m_titles;
};

#endif /* JUKEN_CONSOLELISTENER_H */
