#ifndef __SERIALDEVICE_H__
#define __SERIALDEVICE_H__

#include <stdio.h>
#include "util.h"

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

    protected:
        int fd;

        void SetupDefault();
        void BlockingMode(bool block);

    private:
        SerialDevice(const SerialDevice&);
};

#endif /* __SERIALDEVICE_H__ */
