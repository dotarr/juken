#ifndef __SERIALDEVICE_H__
#define __SERIALDEVICE_H__

#include "types.h"

// A class for communications over a serial port. Only basic
// configuration of the port is provided at this time.

class SerialDevice
{
    public:
        SerialDevice();
        virtual ~SerialDevice();

        int OpenDevice(const char* dev);
        void CloseDevice();

        int GetFileDescriptor() const { return fd; }

        void SetDTR();
        void ClearDTR();

        void WriteFully(const void* buf, const size_t count);
        void ReadFully(void* buf, const size_t count);

    protected:
        int fd;

        void SetupDefault();
        void BlockingMode(bool block);

    private:
        SerialDevice(const SerialDevice&);
};

#endif /* __SERIALDEVICE_H__ */
