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

#endif /* JUKEN_KENWOODDEVICE_H */
