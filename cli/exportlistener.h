#ifndef JUKEN_EXPORTLISTENER_H
#define JUKEN_EXPORTLISTENER_H

#include <kenwoodlistener.h>

class ExportListener : public KenwoodListener
{
    public:
        ExportListener(FILE* f);
        ~ExportListener();

        void setEventListener(KenwoodListener* event_listener)
            { m_event_listener = event_listener; };

        virtual void Handshake(const char* id)
            {
                if ( m_event_listener != NULL )
                    m_event_listener->Handshake(id);
            };
        virtual void InfoChanged(short slot, byte track, enum mode mode, 
                                 enum random random, bool repeat, 
                                 byte userfile)
            {
                if ( m_event_listener != NULL )
                    m_event_listener->InfoChanged(slot, track, mode, random, repeat, userfile);
            };
        virtual void StateChanged(enum state state)
            {
                if ( m_event_listener != NULL )
                    m_event_listener->StateChanged(state);
            };
        virtual void DiscChanged(short slot)
            {
                if ( m_event_listener != NULL )
                    m_event_listener->DiscChanged(slot);
            };
        virtual void DoorChanged(bool door_closed)
            {
                if ( m_event_listener != NULL )
                    m_event_listener->DoorChanged(door_closed);
            };
        
        virtual void DiscDataReply(DiscData* info);
        virtual void CDTextDataReply(CDTextData* info);
        virtual void TrackTimesReply(TrackTimes* info);
        virtual void DiscTrackListReply(DiscTrackList* info);

    protected:

    private:
        KenwoodListener* m_event_listener; // the listener to handle non-export messages
        FILE* m_file; // the file too export to
        uint  m_discid;
        short m_track_count;
};

#endif /* JUKEN_EXPORTLISTENER_H */
