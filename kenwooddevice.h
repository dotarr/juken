#ifndef __KENWOODDEVICE_H__
#define __KENWOODDEVICE_H__

#include "serialdevice.h"
#include "types.h"
#include "payload.h"

// A class for communications to a Kenwood changer via a serial port.

class KenwoodDevice : public SerialDevice
{
    public:
        KenwoodDevice(const char* dev);
        virtual ~KenwoodDevice();

        void WritePayload(const payload& msg);
        byte ReadPayload(payload& msg);

        byte ComputeChecksum(const payload& msg);

        void WriteCntl(byte c);
        byte ReadCntl();

    protected:

    private:
        KenwoodDevice();
        KenwoodDevice(const KenwoodDevice&);
};

#endif /* __KENWOODDEVICE_H__ */
