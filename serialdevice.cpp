#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "serialdevice.h"
#include "util.h"

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

void
SerialDevice::SetDTR() 
{
    int set;
    // get the current set
    ::ioctl(fd, TIOCMGET, &set);
    // ensure that DTR is set
    set |= TIOCM_DTR;
    ::ioctl(fd, TIOCMSET, &set);
}

void
SerialDevice::ClearDTR() 
{
    int set;
    // get the current set
    ::ioctl(fd, TIOCMGET, &set);
    // ensure that DTR is clear
    set &= ~TIOCM_DTR;
    ::ioctl(fd, TIOCMSET, &set);
}

int
SerialDevice::OpenDevice(const char* dev)
{
    if ( !dev ) 
        Throw("Unable to open device: No device name provided");

    // open the serial port, make sure that its not the controlling tty
    fd = ::open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    ThrowIfMinus1(fd, "Unable to open %s: ", dev);

    // flush any garbage remaining on the port from previous operations.
    ThrowIfMinus1(::tcflush(fd, TCIOFLUSH), "Failed to flush: ");

    // Setup the port
    SetupDefault();

    // Set blocking
    BlockingMode(true);

    // indicate that we are ready
    SetDTR();

    return fd;
}

void
SerialDevice::CloseDevice()
{
    // return if not open
    if ( fd <= 0 ) return;

    // get the terminal attributes
    struct termios trm;
    ThrowIfMinus1(::tcgetattr(fd, &trm), "Failed to get attributes: ");

    // set terminal not to hangup on close
    trm.c_cflag &= ~HUPCL;
    ::tcsetattr(fd, TCSAFLUSH, &trm);
    //::tcsetattr(fd, TCSADRAIN, &trm);
    //::tcsetattr(fd, TCSANOW, &trm);

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
SerialDevice::SetupDefault()
{
    // flush any unwritten, unread data
    ::tcflush(fd, TCIOFLUSH);

    struct termios tset;

    // 8 bits, no parity, one stop bit, 9600 baud
    tset.c_cflag = CREAD|CS8|B9600|CRTSCTS|HUPCL;

    // ignore break, do not ignore parity
    tset.c_iflag = IGNBRK;
    tset.c_lflag &= ~ICANON;

    // no delay on carriage return, backspace, tab, etc.
    tset.c_oflag &= ~(NLDLY|CRDLY|TABDLY|BSDLY|VTDLY|FFDLY);
    tset.c_oflag |= NL0|CR0|TAB0|BS0|VT0|FF0;

    tset.c_cc[VEOF]   = _POSIX_VDISABLE;
    tset.c_cc[VEOL]   = _POSIX_VDISABLE;
    tset.c_cc[VERASE] = _POSIX_VDISABLE;
    tset.c_cc[VINTR]  = _POSIX_VDISABLE;
    tset.c_cc[VKILL]  = _POSIX_VDISABLE;
    tset.c_cc[VQUIT]  = _POSIX_VDISABLE;
    tset.c_cc[VSUSP]  = _POSIX_VDISABLE;
    tset.c_cc[VSTART] = _POSIX_VDISABLE;
    tset.c_cc[VSTOP]  = _POSIX_VDISABLE;
   
    // set the attributes
    ThrowIfMinus1(::tcsetattr(fd, TCSANOW, &tset), 
                  "Failed to set attributes: ");
}

void
SerialDevice::BlockingMode(bool block)
{
    // get the current mode
    int flags = ::fcntl(fd, F_GETFL, 0);
    // set the mode
    if ( block )
        ThrowIfMinus1(::fcntl(fd, F_SETFL, flags & ~O_NDELAY),
                     "Failed to set blocking mode: ");
    else
        ThrowIfMinus1(::fcntl(fd, F_SETFL, flags | O_NDELAY),
                     "Failed to set blocking mode: ");
}

