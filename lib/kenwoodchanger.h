#ifndef JUKEN_KENWOODCHANGER_H
#define JUKEN_KENWOODCHANGER_H

#include <deque>

#include "kenwooddevice.h"
#include "kenwoodlistener.h"
#include "types.h"
#include "cdpayload.h"

// A class for communications to a Kenwood changer via a serial port.

class KenwoodChanger
{
    public:
        KenwoodChanger(char* id, KenwoodDevice& dev);
        virtual ~KenwoodChanger();

        void pushListener(KenwoodListener* listener);
        void popListener();

        void DoEvent();
        virtual void ProcessEvent() = 0;

        virtual void DoListDiscs(byte x=0) = 0;
        virtual void DoListContents(const short slot, byte x=1) = 0;
        virtual void DoListBest() = 0;
        virtual void DoChangeDisc(const short slot, enum state cur_state) = 0;
        virtual void DoPlayPause() = 0;
        virtual void DoPrev() = 0;
        virtual void DoNext() = 0;
        virtual void DoStop() = 0;

    protected:
        KenwoodDevice& m_device;
        deque<KenwoodListener*> m_listeners;

        void IssueRequest(const payload& msg, const bool has_replies);
        void GetOneReply(payload& reply);
        bool GetReply(payload& reply);
        bool GetEvent(payload& event);

    private:
        KenwoodChanger();
        KenwoodChanger(const KenwoodChanger& changer);
};

#endif /* JUKEN_KENWOODCHANGER_H */
