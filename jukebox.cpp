#include <sys/errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "jukebox.h"


// This class implements the primary protocol handling. The class is used
// by the daemon to process communications to/from the player and communications
// to/from the tools. It is also used by the tools to issue requests and to
// process replies.

Jukebox::Jukebox() { }

Jukebox::~Jukebox() { }

// This function processes messages received from the player. These messages
// consist of events sent by the player in response to player state changes 
// (e.g. someone hit the play button ...)
void
Jukebox::ProcessIncomingMessage(int fd)
{
    byte cmd;
    struct payload msg;
    byte cksum;

    bool done = false;
    // while the message hasn't completed ...
    while ( !done )
    {
        // read a control byte
        byte cntl = readcntl(fd);
        switch ( cntl )
        {
            case STX:
                // read the payload from the player
                cksum = read_payload(cmd, msg, fd);
                // validate the checksum
                if ( cksum == checksum(cmd, msg) )
                {
                    // acknowledge the event
                    writecntl(fd, ACK);
                }
                else
                {
                    // signal transmission error (should cause retransmit ...)
                    TraceFlow("Bad checksum in recieved data\n");
                    writecntl(fd, NAK);
                }
                break;
            case EOT:
                // transmission completed
                done = true;
                writecntl(fd, ACK);
                break;
            case ENQ:
                // acknowledge we are ready for transmission
                writecntl(fd, ACK);
                break;
            case ACK:
                // protocol corrupted?
                TraceFlow("->Unexpected ACK\n");
                break;
            case NAK:
                // protocol corrupted?
                TraceFlow("->Unexpected NAK\n");
                break;
        }
    }

    DebugConn("   processing event\n");
    ProcessEvent(cmd, msg);
}

// This function processes messages being sent by the tools to the player.
void
Jukebox::ProcessOutgoingMessage(int fd, int msg_fd)
{
    byte cmd;
    struct payload msg;
    byte cksum;

    // read the payload from the tool
    read_payload(cmd, msg, msg_fd);
    // determine if we need to signal EOT or not
    bool send_eot = (readc(msg_fd) == EOT);

    // signal player we wish to transmit
    writecntl(fd, ENQ);

    bool msg_sent = false;
    bool eot_sent = false;
    bool done = false;
    while ( !done )
    {
        // read a control byte
        byte cntl = readcntl(fd);
        switch ( cntl )
        {
            case STX:
                // read the payload from the player
                cksum = read_payload(cmd, msg, fd);
                // validate the checksum
                if ( cksum == checksum(cmd, msg) )
                {
                    DebugConn("   processing reply\n");
                    // send the reply onto the tool
                    writecntl(msg_fd, STX);
                    write_payload(cmd, msg, msg_fd);
                    writecntl(fd, ACK);
                }
                else
                {
                    // signal transmission error (should cause retransmit ...)
                    TraceFlow("Bad checksum in recieved data\n");
                    writecntl(fd, NAK);
                }
                break;
            case EOT:
                // transmission completed
                done = true;
                writecntl(msg_fd, EOT);
                writecntl(fd, ACK);
                break;
            case ENQ:
                // protocol corrupted?
                TraceFlow("->Unexpected ENQ\n");
                break;
            case ACK:
                if ( msg_sent )
                {
                    // signal player we are finished
                    if ( send_eot )
                        if ( eot_sent )
                            done = true;
                        else
                            writecntl(fd, EOT);
                    //send_eot = false;
                    eot_sent = true;
                }
                else
                {
                    DebugConn("   sending request\n");
                    // write the payload to the player
                    writecntl(fd, STX);
                    write_payload(cmd, msg, fd);
                    msg_sent = true;
                }
                break;
            case NAK:
                // resend the payload to the player
                writecntl(fd, STX);
                write_payload(cmd, msg, fd);
                break;
        }
    }
}

// The following five functions hadle state events from the player
void
Jukebox::ProcessEvent(const byte cmd, const struct payload& msg)
{
    switch ( cmd )
    {
        case INFO_EVT:  HandleInfoEvent(msg);  break;
        case STATE_EVT: HandleStateEvent(msg); break;
        case DISC_EVT:  HandleDiscEvent(msg);  break;
        case DOOR_EVT:  HandleDoorEvent(msg); break;

        default:
            {
                // display the unhandled event
                DebugMsg("cmd 0x%02X\t\n\t\t", cmd);
                for (int i=0; i<msg.len; i++) 
                    DebugMsg(" 0x%02X", msg.data[i]);
                DebugMsg("\n\t\t");
                for (int i=0; i<msg.len; i++)
                    DebugMsg("%c", isprint(msg.data[i])?msg.data[i]:'.');
                DebugMsg("\n");
            }
            break;
    }
}

void
Jukebox::HandleInfoEvent(const struct payload& msg)
{
    struct cur_info
    {
        ushort disc_num;
        byte   track_num;
        byte   best_mode;
        byte   track_count;
        byte   userfiles;
        byte   userfile_mode;
        byte   random_mode;
        byte   repeat_mode;
    };

    // cast the payload into a cur_info
    struct cur_info* info = (struct cur_info*) msg.data;

    // determine the current mode
    int mode = TrackMode;
    if ( info->best_mode )
        mode = BestMode;
    else if ( info->userfile_mode != 0 )
        mode = UserfileMode;
    else if ( info->repeat_mode )
        mode = RepeatMode;
    else if ( info->random_mode )
        mode = (info->random_mode==1) ? OneRandomMode : AllRandomMode;
    else
        mode = TrackMode;

    //display the info event
    DebugMsg("disc#: %d track: %d (of%d)\n",
             info->disc_num, info->track_num, info->track_count);
    DebugMsg("\tmode: %s\n", MODE_NAMES[mode]);
    DebugMsg("\tuserfiles: %02X\n", info->userfiles);
    DebugMsg("\tuserfile_mode: %02X\n", info->userfile_mode);
}

void
Jukebox::HandleStateEvent(const struct payload& msg)
{
    byte s = msg.data[0];

    // determine the current state
    int state = Unknown;
    switch ( s )
    {
        case STOPPED_STATE:  state = Stopped;      break;
        case STOPPING_STATE: state = Stopping;     break;
        case CHANGING_STATE: state = Changing;     break;
        case PLAYING_STATE:  state = Playing;      break;
        case PAUSED_STATE:   state = Paused;       break;
        case SKIPFORW_STATE: state = SkipForward;  break;
        case SKIPBACK_STATE: state = SkipBackward; break;
        default:   state = Unknown;      break;
    }

    // display the current state
    DebugMsg("state: %s\n", STATE_NAMES[state]);
}
 
void
Jukebox::HandleDiscEvent(const struct payload& msg)
{
    // determine the current disc number
    ushort d = *((ushort*) msg.data);
    int disc = d;
    // display the current disc number
    DebugMsg("disc#: %d\n", disc);
}
 
void
Jukebox::HandleDoorEvent(const struct payload& msg)
{
    // determine the ready state
    bool door_closed = (msg.data[0]==0);
    // display the ready state
    DebugMsg("door: %s\n", door_closed?"closed":"open");
}

void
Jukebox::IssueChangeState(int fd, const byte state)
{
    struct change_state
    {
        byte b;
        byte state;
    };

    // prepare a change state request
    struct payload msg;
    struct change_state* s = (struct change_state*) msg.data;
    s->b = (state==NULL_PARAM ? NULL_PARAM : STATE_PARAM);
    s->state = state;
    msg.len = sizeof(struct change_state);

    // issue the request
    IssueRequest(fd, STATE_REQ, msg, NO_REPLIES); 
}

void
Jukebox::IssueQueryDevice(int fd,
                          const int b0,
                          const byte t, 
                          const ushort d,
                          const int b1,
                          const int b2,
                          const int b3)
{
    struct query_device
    {
        byte b0;
        byte t;
        ushort d;
        byte b1;
        byte b2;
        byte b3;
        byte pad;
    };
 
    // prepare a query device request
    struct payload msg;
    struct query_device* q = (struct query_device*) msg.data;
    q->b0 = b0;
    q->t = t;
    q->d = d;
    q->b1 = b1;
    q->b2 = b2;
    q->b3 = b3;
    msg.len = sizeof(struct query_device) - 1;

    // issue the request
    IssueRequest(fd, QUERY_REQ, msg, HAS_REPLIES); 
}

void
Jukebox::IssueRequest(int fd, byte cmd, struct payload& msg, bool replies)
{
    // write the payload
    write_payload(cmd, msg, fd);
    
    // signal if we need a reply
    if ( replies ) writec(fd, ETB);
    else           writec(fd, EOT);
}

bool
Jukebox::GetReply(int fd, struct payload& msg)
{
    // read the prelude control byte
    byte cntl = readcntl(fd);

    // if no more replies, signal completion
    if ( cntl == EOT )
        return false;

    // read the reply payload
    byte cmd;
    byte cksum = read_payload(cmd, msg, fd);

    // signal reply received
    return true;
}

byte
Jukebox::checksum(byte cmd, const struct payload& msg)
{
    // compute the payload checksum
    byte sum = cmd + (msg.len-1);
    for (int i=0; i<msg.len; i++)
        sum += msg.data[i];
    return ~sum;
}

void
Jukebox::write_payload(const byte cmd, const struct payload& msg, int fd)
{
    byte cksum = checksum(cmd, msg);
    DebugPayload("write_payload", cmd, msg, cksum);
    // write the command byte
    writec(fd, cmd);
    // write the payload
    writefully(fd, &msg, msg.len+sizeof(msg.len));
    // write the checksum
    writec(fd, cksum);
}

byte
Jukebox::read_payload(byte& cmd, struct payload& msg, int fd)
{
    // read the command
    cmd = readc(fd);
    // read the payload
    readfully(fd, &msg.len, sizeof(msg.len));
    readfully(fd, &msg.data, msg.len);
    // read the checksum
    byte cksum = readc(fd);
    DebugPayload("read_payload", cmd, msg, cksum);
    return cksum;
}

void
Jukebox::writefully(int fd, const void* buf, size_t count)
{
    // repeat until all bytes are written (or the write fails)
    const void* p = buf;
    do 
    {
        ssize_t sent = ::write(fd, p, count);
        ThrowIfMinus1(sent, "write failed: ");
        p = ((byte*)p) + sent;
        count -= sent;
    }
    while ( count > 0 );
}

int
Jukebox::readfully(int fd, void* buf, size_t count)
{
    // repeat until all bytes are read (or the read fails)
    void* p = buf;
    do 
    {
        ssize_t rcvd = ::read(fd, p, count);
        ThrowIfMinus1(rcvd, "read failed: ");
        if ( rcvd == 0 ) return -1;
        p = ((byte*)p) + rcvd;
        count -= rcvd;
    }
    while ( count > 0 );
    return count;
}

void
Jukebox::writecntl(int fd, byte c)
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
    writec(fd, c);
}

byte
Jukebox::readcntl(int fd)
{
    // reads a control byte (with possible tracing)
    byte c = readc(fd);
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

void
Jukebox::writec(int fd, byte c)
{
    ssize_t sent;
    do 
    {
        sent = ::write(fd, &c, 1);
        ThrowIfMinus1(sent, "write failed: ");
    }
    while ( sent == 0 );
}

byte
Jukebox::readc(int fd)
{
    byte c;
    ssize_t rcvd;
    do 
    {
        rcvd = ::read(fd, &c, 1);
        ThrowIfMinus1(rcvd, "read failed: ");
if ( rcvd == 0 )
        ThrowIf(rcvd==0, "EOF");
    }
    while ( rcvd == 0 );
    return c;
}
