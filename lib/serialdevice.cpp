#include <fcntl.h>
#include <sys/stat.h>

#include <common.h>

#ifndef HAVE_CFMAKERAW
BEGIN_C_DECLS
void cfmakeraw(struct termios* termios_p);
END_C_DECLS
#endif

#include "serialdevice.h"

// A class for handling serial port communications

SerialDevice::SerialDevice() 
{
    // init the fd member
    fd = -1;
}

SerialDevice::~SerialDevice()
{
    // close the port
    CloseDevice();
}

int
SerialDevice::OpenDevice(const char* dev)
{
    if ( !dev ) 
        Throw("Unable to open device: No device name provided");

    // open the serial port, make sure that its not the controlling tty
    DebugMsg("opening %s ...", dev);
    fd = ::open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    ThrowIfMinus1(fd, "Unable to open %s: ", dev);
    DebugMsg("opened\n");

    // check to make sure that dev is a device and not say a directory
    DebugMsg("checking for tty ...");
    ThrowIf(!::isatty(fd), "%s does not appear to be a device. \n", dev );
    DebugMsg("done\n");

    // flush any garbage remaining on the port from previous operations.
    DebugMsg("flushing ...");
    ThrowIfMinus1(::tcflush(fd, TCIOFLUSH), "Failed to flush: ");
    DebugMsg("done\n");

    // save the ports current attributes
    SaveAttributes();

    // Setup the port
    SetupDefault();

    // Set blocking
    BlockingMode(true);

    return fd;
}

void
SerialDevice::CloseDevice()
{
    // return if not open
    if ( fd <= 0 ) return;

    // flush any garbage remaining on the port from previous operations.
    ThrowIfMinus1(::tcflush(fd, TCIOFLUSH), "Failed to flush: ");

    // restore the ports original attributes
    RestoreAttributes();

    // reset the fd member
    fd = -1;
}

void
SerialDevice::WriteFully(const void* buf, const size_t count)
{
    // repeat until all bytes are written (or the write fails)
    size_t c = count;
    const void* p = buf;
    do 
    {
        ssize_t sent = ::write(fd, p, c);
        ThrowIfMinus1(sent, "write failed: ");
        p = ((byte*)p) + sent;
        c -= sent;
    }
    while ( c > 0 );
}

void
SerialDevice::ReadFully(void* buf, const size_t count)
{
    // repeat until all bytes are read (or the read fails)
    size_t c = count;
    void* p = buf;
    do 
    {
        ssize_t rcvd = ::read(fd, p, c);
        ThrowIfMinus1(rcvd, "read failed: ");
        if ( rcvd == 0 ) Throw("eof");
        p = ((byte*)p) + rcvd;
        c -= rcvd;
    }
    while ( c > 0 );
}

void
SerialDevice::SaveAttributes()
{
    // get the current attributes
    DebugMsg("getting attributes ...");
    ThrowIfMinus1(::tcgetattr(fd, &m_saved_attr), 
                    "Failed to save attributes: ");
    DebugMsg("done\n");
}

void
SerialDevice::RestoreAttributes()
{
    DebugMsg("setting attributes...");
    ThrowIfMinus1(::tcsetattr(fd, TCSAFLUSH, &m_saved_attr), 
                    "Failed to restore attributes: ");
    DebugMsg("done\n");
}

void
SerialDevice::SetupDefault()
{
    struct termios tset;
    ThrowIfMinus1(::tcgetattr(fd, &tset), "Failed to get attributes: ");

    // default raw settings 
    DebugMsg("setting raw mode ...");
    ::cfmakeraw(&tset);
    DebugMsg("done\n");

    // 8 bits, no parity, one stop bit, 9600 baud
    tset.c_cflag = CREAD|CS8|B9600|HUPCL;

    tset.c_cc[VMIN] = 1;
    tset.c_cc[VTIME] = 50;
   
    // set the attributes
    DebugMsg("setting 8N1 9600 baud ...");
    ThrowIfMinus1(::tcsetattr(fd, TCSAFLUSH, &tset), "Failed to set attributes: ");
    DebugMsg("done\n");
}

void
SerialDevice::BlockingMode(bool block)
{
    DebugMsg("setting blocking mode ...");
    // get the current mode
    int flags = ::fcntl(fd, F_GETFL, 0);
    // set the mode
    if ( block )
        ThrowIfMinus1(::fcntl(fd, F_SETFL, flags & ~O_NDELAY),
                     "Failed to set blocking mode: ");
    else
        ThrowIfMinus1(::fcntl(fd, F_SETFL, flags | O_NDELAY),
                     "Failed to set blocking mode: ");
    DebugMsg("done\n");
}

