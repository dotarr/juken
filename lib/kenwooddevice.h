#ifndef JUKEN_KENWOODDEVICE_H
#define JUKEN_KENWOODDEVICE_H

#include "serialdevice.h"
#include "types.h"
#include "payload.h"

// A class for communications to a Kenwood changer via a serial port.

class KenwoodDevice : public SerialDevice
{
    public:
        KenwoodDevice(const char* dev);
        virtual ~KenwoodDevice();

        char* DoHandshake(const char* id);

        bool getEventPending() { return m_event_pending; }
        bool setEventPending(bool pending) { m_event_pending = pending; }

        void SendMessage(const payload& msg, const bool has_replies);
        void EndMessage();
        bool RecvMessage(payload& msg);

        void WritePayload(const payload& msg);
        byte ReadPayload(payload& msg);

        byte ComputeChecksum(const payload& msg);

        void WriteCntl(byte c);
        byte ReadCntl();

    private:
        bool m_event_pending;

        bool CheckForEvent(int usecs);

        KenwoodDevice();
        KenwoodDevice(const KenwoodDevice&);
};

#endif /* JUKEN_KENWOODDEVICE_H */
