#include <sys/errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "jukebox.h"


Jukebox::Jukebox() 
{
}

Jukebox::~Jukebox()
{
}

/*

        struct track_data* info = (struct track_data*) msg.data;
        index = info->index;

        byte title_len = msg.len-sizeof(struct track_data)+1;
        if ( info->title[0] == 0x01 )
            title_len = 0;
        if ( title_len > 0 )
            ::strncpy(title, info->title, title_len);
        title[title_len] = '\0';

        DebugMsg("unknown: %02x %02x index:%d 
                  unknown: %02x %02x %02x %02x title:%s\n",
                 info->unknown1, info->unknown2, index, info->unknown3, 
                 info->unknown4, info->unknown5, info->unknown6, title);
*/

void
Jukebox::ProcessIncomingMessage(int fd)
{
    bool done = false;
    byte cmd;
    byte cksum;
    struct payload msg;

    while ( !done )
    {
        byte cntl = readcntl(fd);
        switch ( cntl )
        {
            case STX:
                cksum = read_payload(cmd, msg, fd);
                if ( cksum == checksum(cmd, msg) )
                {
                    ProcessEvent(cmd, msg);
                    writecntl(fd, ACK);
                }
                else
                {
                    TraceFlow("Bad checksum in recieved data\n");
                    writecntl(fd, NAK);
                }
                break;
            case EOT:
                done = true;
                writecntl(fd, ACK);
                break;
            case ENQ:
                writecntl(fd, ACK);
                break;
            case ACK:
                TraceFlow("->Unexpected ACK\n");
                break;
            case NAK:
                TraceFlow("->Unexpected NAK\n");
                break;
        }
    }
}

void
Jukebox::ProcessOutgoingMessage(int fd, int msg_fd)
{
    bool done = false;
    bool msg_sent = false;
    byte cmd;
    byte cksum;
    struct payload msg;

    read_payload(cmd, msg, msg_fd);
    bool send_eot = (readc(msg_fd) == EOT);

    writecntl(fd, ENQ);

    while ( !done )
    {
        byte cntl = readcntl(fd);
        switch ( cntl )
        {
            case STX:
                cksum = read_payload(cmd, msg, fd);
                if ( cksum == checksum(cmd, msg) )
                {
                    writecntl(msg_fd, STX);
                    write_payload(cmd, msg, msg_fd);
                    writecntl(fd, ACK);
                }
                else
                {
                    TraceFlow("Bad checksum in recieved data\n");
                    writecntl(fd, NAK);
                }
                break;
            case EOT:
                done = true;
                writecntl(msg_fd, EOT);
                writecntl(fd, ACK);
                break;
            case ENQ:
                TraceFlow("->Unexpected ENQ\n");
                break;
            case ACK:
                if ( msg_sent )
                {
                    if ( send_eot ) writecntl(fd, EOT);
                    send_eot = false;
                }
                else
                {
                    writecntl(fd, STX);
                    write_payload(cmd, msg, fd);
                    msg_sent = true;
                }
                break;
            case NAK:
                writecntl(fd, STX);
                write_payload(cmd, msg, fd);
                break;
        }
    }
}

void
Jukebox::ProcessEvent(const byte cmd, const struct payload& msg)
{
    switch ( cmd )
    {
        case INFO_EVT:  HandleInfoEvent(msg);  break;
        case STATE_EVT: HandleStateEvent(msg); break;
        case DISC_EVT:  HandleDiscEvent(msg);  break;
        case READY_EVT: HandleReadyEvent(msg); break;

        default:
            {
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
        byte   user_files;
        byte   unknown;
        byte   random_mode;
        byte   repeat_mode;
    };

    struct cur_info* info = (struct cur_info*) msg.data;

    int mode = TrackMode;
    if ( info->best_mode )
        mode = BestMode;
    else if ( info->repeat_mode )
        mode = RepeatMode;
    else if ( info->random_mode )
        mode = (info->random_mode==1) ? OneRandomMode : AllRandomMode;
    else
        mode = TrackMode;

    DebugMsg("disc#: %d track: %d (of%d)\n",
             info->disc_num, info->track_num, info->track_count);
    DebugMsg("\tmode: %s\n", MODE_NAMES[mode]);
    DebugMsg("\tuserfiles: %02X\n", info->user_files);
    DebugMsg("\tunknown: %02X\n", info->unknown);
}

void
Jukebox::HandleStateEvent(const struct payload& msg)
{
    byte s = msg.data[0];

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

    DebugMsg("state: %s\n", STATE_NAMES[state]);
}
 
void
Jukebox::HandleDiscEvent(const struct payload& msg)
{
    ushort d = *((ushort*) msg.data);
    int disc = d;
    DebugMsg("disc#: %d\n", disc);
}
 
void
Jukebox::HandleReadyEvent(const struct payload& msg)
{
    byte val = msg.data[0];
    DebugMsg("ready: 0x%02X\n", val);
}

void
Jukebox::IssueChangeState(int fd, const byte state)
{
    struct change_state
    {
        byte b;
        byte state;
    };

    struct payload msg;
    struct change_state* s = (struct change_state*) msg.data;
    s->b = (state==NULL_PARAM ? NULL_PARAM : STATE_PARAM);
    s->state = state;
    msg.len = sizeof(struct change_state);

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
 
    struct payload msg;
    struct query_device* q = (struct query_device*) msg.data;
    q->b0 = b0;
    q->t = t;
    q->d = d;
    q->b1 = b1;
    q->b2 = b2;
    q->b3 = b3;
    msg.len = sizeof(struct query_device) - 1;

    IssueRequest(fd, QUERY_REQ, msg, HAS_REPLIES); 
}

void
Jukebox::IssueRequest(int fd, byte cmd, struct payload& msg, bool replies)
{
    write_payload(cmd, msg, fd);
    if ( replies )
        writec(fd, ETB);
    else
        writec(fd, EOT);
}

bool
Jukebox::GetReply(int fd, struct payload& msg)
{
    byte cntl = readcntl(fd);
    if ( cntl == EOT ) return false;

    byte cmd;
    byte cksum = read_payload(cmd, msg, fd);

    return true;
}

byte
Jukebox::checksum(byte cmd, const struct payload& msg)
{
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
    writec(fd, cmd);
    writefully(fd, &msg, msg.len+sizeof(msg.len));
    writec(fd, cksum);
}

byte
Jukebox::read_payload(byte& cmd, struct payload& msg, int fd)
{
    cmd = readc(fd);
    readfully(fd, &msg.len, sizeof(msg.len));
    readfully(fd, &msg.data, msg.len);
    byte cksum = readc(fd); //checksum
    DebugPayload("read_payload", cmd, msg, cksum);
    return cksum;
}

void
Jukebox::writefully(int fd, const void* buf, size_t count)
{
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
