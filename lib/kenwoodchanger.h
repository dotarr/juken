#ifndef JUKEN_KENWOODCHANGER_H
#define JUKEN_KENWOODCHANGER_H

#include <deque>

#include "kenwooddevice.h"
#include "kenwoodlistener.h"
#include "types.h"
#include "payload.h"

// A class for communications to a Kenwood changer via a serial port.

class KenwoodChanger
{
    public:
        KenwoodChanger(KenwoodDevice& dev);
        virtual ~KenwoodChanger();

        void pushListener(KenwoodListener* listener);
        void popListener();

        void DoEvent();
        void ProcessEvent();
        void DoInfoEvent(const payload& event);
        void DoStateEvent(const payload& event);
        void DoDiscEvent(const payload& event);
        void DoDoorEvent(const payload& event);

        void DoListDiscs(byte x=0);
        void DoListTracks(const short slot, byte x=1);
        void DoListTrackTimes(const short slot);
        void DoListBest();
        void DoChangeDisc(const short slot, enum state cur_state);
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

    protected:

    private:
        KenwoodDevice& m_device;
        deque<KenwoodListener*> m_listeners;

        KenwoodChanger();
        KenwoodChanger(const KenwoodChanger& changer);
};

#endif /* JUKEN_KENWOODCHANGER_H */
