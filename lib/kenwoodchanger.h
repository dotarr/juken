#ifndef JUKEN_KENWOODCHANGER_H
#define JUKEN_KENWOODCHANGER_H

#include "kenwooddevice.h"
#include "kenwoodlistener.h"
#include "types.h"
#include "payload.h"

// A class for communications to a Kenwood changer via a serial port.

class KenwoodChanger
{
    public:
        KenwoodChanger(KenwoodDevice& dev, KenwoodListener& listener);
        virtual ~KenwoodChanger();

        void DoEvent();
        void DoInfoEvent(const payload& event);
        void DoStateEvent(const payload& event);
        void DoDiscEvent(const payload& event);
        void DoDoorEvent(const payload& event);

        void DoHandshake(const char* id);

        void DoListDiscs(byte x=0);
        void DoListTracks(const short slot, byte x=1);
        void DoListTrackTimes();
        void DoListBest();
        void DoChangeDisc(const short slot);
        void DoPlayPause();
        void DoPrevTrack();
        void DoNextTrack();
        void DoStop();

        void DoDiscQuery(const DataAccess& query);
        void DoChangeState(const short state);

        void IssueRequest(const payload& msg, const bool has_replies);
        void GetOneReply(payload& reply);
        bool GetReply(payload& reply);
        bool GetEvent(payload& event);

        void SendMessage(const payload& msg, const bool has_replies);
        bool RecvMessage(payload& msg);

        bool isReady() const { return m_is_ready; };
        short getCurrentSlot() const { return m_cur_slot; };
        byte getCurrentTrack() const { return m_cur_track; };
        enum state getCurrentState() const { return m_cur_state; };
        enum mode getCurrentMode() const { return m_cur_mode; };

    protected:
        bool m_is_ready;

        char* m_id;
        short m_capacity;
        
        short m_cur_slot;
        byte m_cur_track;
        enum state m_cur_state;
        enum mode m_cur_mode;
        enum random m_random_state;
        bool m_repeat;
        byte m_cur_userfile;
        bool m_door_closed;

    private:
        KenwoodDevice& m_device;
        KenwoodListener& m_listener;

        KenwoodChanger();
        KenwoodChanger(const KenwoodChanger& changer);
};

#endif /* JUKEN_KENWOODCHANGER_H */
