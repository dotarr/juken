#ifndef __KENWOODCHANGER_H__
#define __KENWOODCHANGER_H__

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
        void DoListDiscs(const short slot, reply_handler func);
        void DoListTracks(const short slot, reply_handler func);
        void DoListTrackTimes(reply_handler func);
        void DoChangeDisc(const short slot);
        void DoPlayPause();
        void DoStop();

        void DoDiscQuery(const data_0x03& query, reply_handler func);
        void DoChangeState(const short state);

        void IssueRequest(const payload& msg, const bool has_replies);
        void GetOneReply(payload& reply);
        bool GetReply(payload& reply);
        bool GetEvent(payload& event);

        void SendMessage(const payload& msg, const bool has_replies);
        bool RecvMessage(payload& msg);

        short getCurrentSlot() const { return m_cur_slot; };
        byte getCurrentTrack() const { return m_cur_track; };
        enum state getCurrentState() const { return m_cur_state; };
        enum mode getCurrentMode() const { return m_cur_mode; };

    protected:
        char* m_id;
        short m_capacity;
        
        short m_cur_slot;
        byte m_cur_track;
        enum state m_cur_state;
        enum mode m_cur_mode;
        bool m_door_closed;

    private:
        KenwoodDevice& m_device;
        KenwoodListener& m_listener;

        KenwoodChanger();
        KenwoodChanger(const KenwoodChanger& changer);
};

#endif /* __KENWOODCHANGER_H__ */
