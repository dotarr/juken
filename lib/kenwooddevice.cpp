#include "kenwooddevice.h"
#include "util.h"

// ----------------------------------------------------------------------------

const char* CLIENT_ID = "I'm PC";

const char* CD425M_ID = "I'm CD-425M";
const char* CD4700M_ID = "I'm CD-4700M";
const char* CD4260M_ID = "I'm CD-4260M";
const char* DV5900M_ID = "I'm DV-5900M";
const char* DV5050M_ID = "I'm DV-5050M";


KenwoodDevice::KenwoodDevice(const char* dev) 
: SerialDevice()
{
    OpenDevice(dev);
}

KenwoodDevice::~KenwoodDevice()
{
}

char*
KenwoodDevice::DoHandshake(const char* id)
{
    // build the payload
    Handshake req(id);

    // issue the request
    SendMessage(req, HAS_REPLIES); 

    // get the reply
    payload reply;
    if ( RecvMessage(reply) )
    {
        payload eor;
        RecvMessage(eor);
    }

    Handshake info(reply);
    return ::strdup(info.identifier());
}

void
KenwoodDevice::SendMessage(const payload& msg, const bool has_replies)
{
    byte cntl;

    // signal player we wish to transmit
    bool acked = false;
    while ( !acked )
    {
        WriteCntl(ENQ);

        cntl = ReadCntl();
        switch ( cntl )
        {
            case STX: ::fprintf(stderr, "->Unexpected STX\n"); break;
            case EOT: ::fprintf(stderr, "->Unexpected EOT\n"); break;
            case ENQ: ::fprintf(stderr, "->Unexpected ENQ\n"); break;
            case ACK: acked = true; break;
            case NAK: ::fprintf(stderr, "->Unexpected NAK\n"); break;
        }
    }

    bool sent = false;
    while ( !sent )
    {
        WriteCntl(STX);
        WritePayload(msg);

        cntl = ReadCntl();
        switch ( cntl )
        {
            case STX: ::fprintf(stderr, "->Unexpected STX\n"); break;
            case EOT: ::fprintf(stderr, "->Unexpected EOT\n"); break;
            case ENQ: ::fprintf(stderr, "->Unexpected ENQ\n"); break;
            case ACK: sent = true; break; 
            case NAK: ::fprintf(stderr, "->Unexpected NAK\n"); break;
        }
    }


    if ( !has_replies )
    {
        acked = false;
        while ( !acked )
        {
            WriteCntl(EOT);

            cntl = ReadCntl();
            switch ( cntl )
            {
                case STX: ::fprintf(stderr, "->Unexpected STX\n"); break;
                case EOT: ::fprintf(stderr, "->Unexpected EOT\n"); break;
                case ENQ: ::fprintf(stderr, "->Unexpected ENQ\n"); break;
                case ACK: acked = true; break;
                case NAK: ::fprintf(stderr, "->Unexpected NAK\n"); break;
            }
        }
    }
}

bool
KenwoodDevice::RecvMessage(payload& msg)
{
    byte cntl = ReadCntl();

    if ( cntl == EOT )
    {
        WriteCntl(ACK);
        msg.cmd = 0xFF;
        return false;
    }
    else if ( cntl == ACK )
    {
        WriteCntl(ACK);
        cntl = ReadCntl();
    }

    if ( cntl != STX )
    {
        switch ( cntl )
        {
            case ENQ: ::fprintf(stderr, "->Unexpected ENQ\n"); break;
            case ACK: ::fprintf(stderr, "->Unexpected ACK\n"); break;
            case NAK: ::fprintf(stderr, "->Unexpected NAK\n"); break;
        }
    }

    byte cksum = ReadPayload(msg);
    if ( cksum == ComputeChecksum(msg) )
    {
        WriteCntl(ACK);
    }
    else
    {
        // signal transmission err (should cause retransmit ...)
        ::fprintf(stderr, "Bad checksum in recieved data\n");
        WriteCntl(NAK);
    }

    return true;
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

