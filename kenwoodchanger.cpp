//#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kenwoodchanger.h"
#include "util.h"
//#include "constants.h"

// ----------------------------------------------------------------------------

const char* CLIENT_ID = "I'm PC";

const char* CD425M_ID = "I'm CD-425M";
const char* CD4700M_ID = "I'm CD-4700M";
const char* CD4260M_ID = "I'm CD-4260M";
const char* DV5900M_ID = "I'm DV-5900M";
const char* DV5050M_ID = "I'm DV-5050M";

const short CD425M_CAPACITY = 200;
const short CD4700M_CAPACITY = 200;
const short CD4260M_CAPACITY = 200;
const short DV5900M_CAPACITY = 400;
const short DV5050M_CAPACITY = 400;

// ----------------------------------------------------------------------------

KenwoodChanger::KenwoodChanger(KenwoodDevice& dev, KenwoodListener& listener) 
: m_device(dev), m_listener(listener)
{
    DoHandshake(CLIENT_ID);

    if      ( ::strcmp(m_id, CD425M_ID)  == 0 ) m_capacity = CD425M_CAPACITY;
    else if ( ::strcmp(m_id, CD4260M_ID) == 0 ) m_capacity = CD4260M_CAPACITY;
    else if ( ::strcmp(m_id, CD4700M_ID) == 0 ) m_capacity = CD4700M_CAPACITY;
    else if ( ::strcmp(m_id, DV5900M_ID) == 0 ) m_capacity = DV5900M_CAPACITY;
    else if ( ::strcmp(m_id, DV5050M_ID) == 0 ) m_capacity = DV5050M_CAPACITY;

    m_cur_slot = 0;
    m_cur_track = 0;
    m_cur_state = Unknown;
    m_cur_mode = TrackMode;
    m_door_closed = true;
}

KenwoodChanger::~KenwoodChanger()
{
}

void
KenwoodChanger::DoEvent()
{
    byte cntl;

    if ( (cntl=m_device.ReadCntl()) != ENQ )
    {
        switch ( cntl )
        {
            case STX: ::fprintf(stderr, "->Unexpected STX\n"); break;
            case EOT: ::fprintf(stderr, "->Unexpected EOT\n"); break;
            case ACK: ::fprintf(stderr, "->Unexpected ACK\n"); break;
            case NAK: ::fprintf(stderr, "->Unexpected NAK\n"); break;
        }
    }
    
    m_device.WriteCntl(ACK);
    
    payload event;
    while ( GetEvent(event) )
    {
        switch ( event.cmd )
        {
            case INFO_EVT:  DoInfoEvent(event);  break;
            case STATE_EVT: DoStateEvent(event); break;
            case DISC_EVT:  DoDiscEvent(event);  break;
            case DOOR_EVT:  DoDoorEvent(event);  break;

            default:
                DebugPayload("unhandled event", event, m_device.ComputeChecksum(event));
                break;
        }
    }
}

void
KenwoodChanger::DoInfoEvent(const payload& event)
{
    // cast the payload
    data_0x12* info = (data_0x12*) event.data;

    // save current slot/track
    m_cur_slot = info->slot;
    m_cur_track = info->track;

    // determine the current mode
    if ( info->best_mode )
        m_cur_mode = BestMode;
    else if ( info->userfile_mode != 0 )
        m_cur_mode = UserfileMode;
    else if ( info->repeat_mode )
        m_cur_mode = RepeatMode;
    else if ( info->random_mode )
        m_cur_mode = (info->random_mode==1) ? OneRandomMode : AllRandomMode;
    else
        m_cur_mode = TrackMode;

    // notify listener
    m_listener.InfoChanged(m_cur_slot, m_cur_track, info->num_tracks,
                           m_cur_mode, info->userfiles, info->userfile_mode);
}

void
KenwoodChanger::DoStateEvent(const payload& event)
{
    // cast the payload
    data_0x13* info = (data_0x13*) event.data;

    // determine the current state
    switch ( info->state )
    {
        case STOPPED_STATE:  m_cur_state = Stopped;      break;
        case STOPPING_STATE: m_cur_state = Stopping;     break;
        case CHANGING_STATE: m_cur_state = Changing;     break;
        case PLAYING_STATE:  m_cur_state = Playing;      break;
        case PAUSED_STATE:   m_cur_state = Paused;       break;
        case SKIPFORW_STATE: m_cur_state = SkipForward;  break;
        case SKIPBACK_STATE: m_cur_state = SkipBackward; break;
        default:
            ::fprintf(stderr, "state=%d\n", info->state);
            m_cur_state = Unknown;
            break;
    }

    // notify listener
    m_listener.StateChanged(m_cur_state);
}
 
void
KenwoodChanger::DoDiscEvent(const payload& event)
{
    // cast the payload
    data_0x14* info = (data_0x14*) event.data;

    // save current slot
    m_cur_slot = info->slot;

    // notify listener
    m_listener.DiscChanged(m_cur_slot);
}
 
void
KenwoodChanger::DoDoorEvent(const payload& event)
{
    // cast the payload
    data_0x15* info = (data_0x15*) event.data;

    // determine the door state
    m_door_closed = (info->door_pos==0);

    // notify listener
    m_listener.DoorChanged(m_door_closed);
}

void
KenwoodChanger::DoHandshake(const char* id)
{
    // build the payload
    payload req;
    req.cmd = HANDSHAKE_REQ;
    req.len = ::strlen(id);
    ::memcpy(req.data, id, req.len);

    // issue the request
    SendMessage(req, HAS_REPLIES); 

    // get the reply
    payload reply;
    GetOneReply(reply);

    // process the data
    reply.data[reply.len] = '\0';
    m_id = ::strdup((char*) reply.data);
::fprintf(stderr, "%s\n", m_id);
}

void
KenwoodChanger::DoListDiscs(const short slot, reply_handler func)
{
    data_0x03 query = { 0, 1, slot, 0, 0, 0 };
    DoDiscQuery(query, func);
}

void
KenwoodChanger::DoListTracks(const short slot, reply_handler func)
{
    if ( slot == 0 )
    {
        data_0x03 query = { 0, 1, m_cur_slot, 0, 1, 0 };
        DoDiscQuery(query, func);
    }
    else
    {
        data_0x03 query = { 0, 1, slot, 0, 1, 0 };
        DoDiscQuery(query, func);
    }
}

void
KenwoodChanger::DoListTrackTimes(reply_handler func)
{
    data_0x03 query = { 0, 4, m_cur_slot, 0, 1, 0 };
    DoDiscQuery(query, func);
}

void
KenwoodChanger::DoChangeDisc(const short slot)
{
    // build the payload
    payload req;
    req.cmd = SELECT_DISC_REQ;
    req.len = sizeof(data_0x0B);
    data_0x0B* select_disc = (data_0x0B*) &req.data;
    select_disc->slot = slot;
    select_disc->track = 1;
    select_disc->begin = (m_cur_state==Playing)?1:0;

    // issue the request
    SendMessage(req, NO_REPLIES); 
}

void
KenwoodChanger::DoPlayPause()
{
    DoChangeState(PLAY_PAUSE_CMD | STATE_PARAM);
}

void
KenwoodChanger::DoStop()
{
    DoChangeState(STOP_CMD | STATE_PARAM);
}

void
KenwoodChanger::DoDiscQuery(const data_0x03& query, reply_handler func)
{
    // build the payload
    payload req;
    req.cmd = QUERY_REQ;
    req.len = sizeof(data_0x03);
    ::memcpy(req.data, &query, req.len);

    // issue the request
    SendMessage(req, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
        // process the data
        func(reply.cmd, reply.len, reply.data);
}

void
KenwoodChanger::DoChangeState(const short state)
{
    // build the payload
    payload req;
    req.cmd = STATE_REQ;
    req.len = sizeof(data_0x0A);
    ::memcpy(req.data, &state, req.len);

    // issue the request
    SendMessage(req, NO_REPLIES); 
}

void
KenwoodChanger::IssueRequest(const payload& msg, const bool has_replies)
{
    SendMessage(msg, has_replies);
}

void
KenwoodChanger::GetOneReply(payload& reply)
{
    if( RecvMessage(reply) )
    {
        payload eor;
        RecvMessage(eor);
    }
}

bool
KenwoodChanger::GetReply(payload& reply)
{
    return RecvMessage(reply);
}

bool
KenwoodChanger::GetEvent(payload& event)
{
    return RecvMessage(event);
}

void
KenwoodChanger::SendMessage(const payload& msg, const bool has_replies)
{
    byte cntl;

    // signal player we wish to transmit
    m_device.WriteCntl(ENQ);

    if ( (cntl=m_device.ReadCntl()) != ACK )
    {
        switch ( cntl )
        {
            case STX: ::fprintf(stderr, "->Unexpected STX\n"); break;
            case EOT: ::fprintf(stderr, "->Unexpected EOT\n"); break;
            case ENQ:
                // we may need to handle an event from the changer
                // before proceeding ...
                ::fprintf(stderr, "->Unexpected ENQ\n");
                break;
            case NAK: ::fprintf(stderr, "->Unexpected NAK\n"); break;
        }
    }
    bool sent = false;
    while ( !sent )
    {
        m_device.WriteCntl(STX);
        m_device.WritePayload(msg);

        cntl = m_device.ReadCntl();
        switch ( cntl )
        {
            case STX: ::fprintf(stderr, "->Unexpected STX\n"); break;
            case EOT: ::fprintf(stderr, "->Unexpected EOT\n"); break;
            case ENQ: ::fprintf(stderr, "->Unexpected ENQ\n"); break;
            case ACK: sent = true; break;
            case NAK: break;
        }
    }


    if ( !has_replies )
    {
        m_device.WriteCntl(EOT);
        if ( (cntl=m_device.ReadCntl()) != ACK )
        {
            switch ( cntl )
            {
                case STX: ::fprintf(stderr, "->Unexpected STX\n"); break;
                case EOT: ::fprintf(stderr, "->Unexpected EOT\n"); break;
                case ENQ: ::fprintf(stderr, "->Unexpected ENQ\n"); break;
                case NAK: ::fprintf(stderr, "->Unexpected NAK\n"); break;
            }
        }
    }
}

bool
KenwoodChanger::RecvMessage(payload& msg)
{
    byte cntl = m_device.ReadCntl();

    if ( cntl == EOT )
    {
        m_device.WriteCntl(ACK);
        msg.cmd = 0xFF;
        return false;
    }
    else if ( cntl != STX )
    {
        switch ( cntl )
        {
            case ENQ: ::fprintf(stderr, "->Unexpected ENQ\n"); break;
            case ACK: ::fprintf(stderr, "->Unexpected ACK\n"); break;
            case NAK: ::fprintf(stderr, "->Unexpected NAK\n"); break;
        }
    }

    byte cksum = m_device.ReadPayload(msg);
    if ( cksum == m_device.ComputeChecksum(msg) )
    {
        m_device.WriteCntl(ACK);
    }
    else
    {
        // signal transmission error (should cause retransmit ...)
        ::fprintf(stderr, "Bad checksum in recieved data\n");
        m_device.WriteCntl(NAK);
    }

    return true;
}

