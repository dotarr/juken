#ifndef __SERIALDEVICE_H__
#define __SERIALDEVICE_H__

#include <stdio.h>
#include "util.h"

// A class for communications over a serial ;port. Only basic
// configuration of the prot is provided.

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
