#include "kenwoodchanger.h"
#include "util.h"

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
    m_is_ready = false;
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
    m_random_state = RandomOff;
    m_repeat = false;
    m_cur_userfile = 0;
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
            case INFO_EVENT:  DoInfoEvent(event);  break;
            case STATE_EVENT: DoStateEvent(event); break;
            case DISC_EVENT:  DoDiscEvent(event);  break;
            case DOOR_EVENT:  DoDoorEvent(event);  break;

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
    InfoEvent* info = (InfoEvent*) event.data;

    // save current slot/track
    m_cur_slot = info->slot;
    m_cur_track = info->track;

    // determine the current mode
    switch ( info->mode )
    {
        case 0:
        case 1:
        case 2:
            m_cur_mode = TrackMode;
            m_random_state = (enum random) (info->mode-0);
            break;
        case 3:
            m_cur_mode = ProgramMode;
            m_random_state = RandomOff; // not applicable
            break;
        case 4:
            m_cur_mode = BestMode;
            m_random_state = RandomOff; // not applicable
            break;
        case 5:
        case 6:
            m_cur_mode = MusicTypeMode;
            m_random_state = (info->mode-5 !=0) ? RandomAll : RandomOff;
            break;
        case 7:
        case 8:
        case 9:
            m_cur_mode = UserfileMode;
            m_random_state = (enum random) (info->mode-7);
            break;
    }

    m_repeat = (info->repeat!=0);
    m_cur_userfile = info->userfile;

    // notify listener
    m_listener.InfoChanged(m_cur_slot, m_cur_track, m_cur_mode, 
                           m_random_state, m_repeat, 
                           m_cur_userfile);
if ( info->unknown==0 && info->mode!=0 )
{
    fprintf(stderr, "wierd unknown detected\n");
    printdata(event.data, event.len);
}if ( info->unknown!=0 && info->mode==0 )
{
    fprintf(stderr, "wierd unknown detected\n");
    printdata(event.data, event.len);
}
}

void
KenwoodChanger::DoStateEvent(const payload& event)
{
    // cast the payload
    StateEvent* info = (StateEvent*) event.data;

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

    m_is_ready = true;
}
 
void
KenwoodChanger::DoDiscEvent(const payload& event)
{
    // cast the payload
    DiscEvent* info = (DiscEvent*) event.data;

    // save current slot
    m_cur_slot = info->slot;

    // notify listener
    m_listener.DiscChanged(m_cur_slot);
}
 
void
KenwoodChanger::DoDoorEvent(const payload& event)
{
    // cast the payload
    DoorEvent* info = (DoorEvent*) event.data;

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
    req.cmd = HANDSHAKE;
    req.len = ::strlen(id);
    ::memcpy(req.data, id, req.len);

    // issue the request
    SendMessage(req, HAS_REPLIES); 

    // get the reply
    payload reply;
    GetOneReply(reply);

    m_id = ::strdup((char*) reply.data);

    // notify listener
    m_listener.Handshake(m_id);
}

void
KenwoodChanger::DoListDiscs(byte x)
{
    DataAccess query = { 0, 1, 0, 0, x, 0 };
    DoDiscQuery(query);
}

void
KenwoodChanger::DoListTracks(const short slot, byte x)
{
    if ( slot == 0 )
    {
        DataAccess query = { 0, 1, m_cur_slot, 0, x, 0 };
        DoDiscQuery(query);
    }
    else
    {
        DataAccess query = { 0, 1, slot, 0, x, 0 };
        DoDiscQuery(query);
    }
}

void
KenwoodChanger::DoListTrackTimes()
{
    DataAccess query = { 0, 4, m_cur_slot, 0, 1, 0 };
    DoDiscQuery(query);
}

void
KenwoodChanger::DoListBest()
{
    DataAccess query = { 0, 32, 0, 0, 0, 0 };
    DoDiscQuery(query);
}

void
KenwoodChanger::DoChangeDisc(const short slot)
{
    // build the payload
    payload req;
    req.cmd = SELECT_DISC_TRACK;
    req.len = sizeof(SelectDiscTrack);
    SelectDiscTrack* select_disc = (SelectDiscTrack*) &req.data;
    select_disc->slot = slot;
    select_disc->track = 1;
    select_disc->begin = (m_cur_state==Playing)?1:0;

    // issue the request
    SendMessage(req, NO_REPLIES); 

    m_is_ready = false;
}

void
KenwoodChanger::DoPlayPause()
{
    DoChangeState(PLAY_PAUSE_CMD | STATE_PARAM);
}

void
KenwoodChanger::DoPrevTrack()
{
    DoChangeState(PREV_CMD | STATE_PARAM);
    DoChangeState(NULL_PARAM);
}

void
KenwoodChanger::DoNextTrack()
{
    DoChangeState(NEXT_CMD | STATE_PARAM);
    DoChangeState(NULL_PARAM);
}

void
KenwoodChanger::DoStop()
{
    DoChangeState(STOP_CMD | STATE_PARAM);
}

void
KenwoodChanger::DoDiscQuery(const DataAccess& query)
{
    // build the payload
    payload req;
    req.cmd = DATA_ACCESS;
    req.len = sizeof(DataAccess);
    ::memcpy(req.data, &query, req.len);

    // issue the request
    SendMessage(req, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        switch ( reply.cmd )
        {
            case DISC_DATA:
                m_listener.DiscDataReply((DiscData*)reply.data);
                break;

            case CD_TEXT_DATA:
                m_listener.CDTextDataReply((CDTextData*)reply.data);
                break;

            case TRACK_TIMES:
                m_listener.TrackTimesReply((TrackTimes*)reply.data);
                break;

            case DISC_TRACK_LIST:
                m_listener.DiscTrackListReply((DiscTrackList*)reply.data);
                break;
        }
    }
}

void
KenwoodChanger::DoChangeState(const short state)
{
    // build the payload
    payload req;
    req.cmd = DO_ACTION;
    req.len = sizeof(DoAction);
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
        // signal transmission err (should cause retransmit ...)
        ::fprintf(stderr, "Bad checksum in recieved data\n");
        m_device.WriteCntl(NAK);
    }

    return true;
}

