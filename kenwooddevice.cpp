#include <stdio.h>

#include "kenwooddevice.h"
#include "util.h"

KenwoodDevice::KenwoodDevice(const char* dev) 
: SerialDevice()
{
    OpenDevice(dev);
}

KenwoodDevice::~KenwoodDevice()
{
}

void
KenwoodDevice::WritePayload(const payload& msg)
{
    byte cksum = ComputeChecksum(msg);
    DebugPayload("write_payload", msg, cksum);
    WriteFully(&msg.cmd, sizeof(msg.cmd));
    WriteFully(&msg.len, sizeof(msg.len));
    WriteFully(msg.data, msg.len);
    WriteFully(&cksum, sizeof(cksum));
}

byte
KenwoodDevice::ReadPayload(payload& msg)
{
    byte cksum = 0;
    ReadFully(&msg.cmd, sizeof(msg.cmd));
    ReadFully(&msg.len, sizeof(msg.len));
    ReadFully(msg.data, msg.len);
    ReadFully(&cksum, sizeof(cksum));
    DebugPayload("read_payload", msg, cksum);
    // we null the checksum to make those payloads that
    // have a string at the end be null terminated
    msg.data[msg.len] = '\0'; // null out checksum
    return cksum;
}

byte
KenwoodDevice::ComputeChecksum(const payload& msg)
{
    // compute the payload checksum
    byte sum = msg.cmd + (msg.len-1);
    for (int i=0; i<msg.len; i++)
        sum += msg.data[i];
    return ~sum;
}

void
KenwoodDevice::WriteCntl(byte c)
{
    // writes a control byte (with possible tracing)
    switch ( c )
    {
        case NUL: TraceFlow("<- NUL\n"); break;
        case SOH: TraceFlow("<- SOH\n"); break;
        case STX: TraceFlow("<- STX\n"); break;
        case ETX: TraceFlow("<- ETX\n"); break;
        case EOT: TraceFlow("<- EOT\n"); break;
        case ENQ: TraceFlow("<- ENQ\n"); break;
        case ACK: TraceFlow("<- ACK\n"); break;
        case NAK: TraceFlow("<- NAK\n"); break;
    }
    WriteFully(&c, 1);
}

byte
KenwoodDevice::ReadCntl()
{
    // reads a control byte (with possible tracing)
    byte c = NUL;
    ReadFully(&c, 1);
    switch ( c )
    {
        case NUL: TraceFlow("-> NUL\n"); break;
        case SOH: TraceFlow("-> SOH\n"); break;
        case STX: TraceFlow("-> STX\n"); break;
        case ETX: TraceFlow("-> ETX\n"); break;
        case EOT: TraceFlow("-> EOT\n"); break;
        case ENQ: TraceFlow("-> ENQ\n"); break;
        case ACK: TraceFlow("-> ACK\n"); break;
        case NAK: TraceFlow("-> NAK\n"); break;
    }
    return c;
}

