#include "kenwoodchanger.h"
#include "util.h"

KenwoodChanger::KenwoodChanger(char* id, KenwoodDevice& dev) 
: m_device(dev)
{
}

KenwoodChanger::~KenwoodChanger()
{
}

void
KenwoodChanger::pushListener(KenwoodListener* listener)
{
    m_listeners.push_front(listener);
}

void
KenwoodChanger::popListener()
{
    m_listeners.pop_front();
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
    
    ProcessEvent();
}

void
KenwoodChanger::ProcessEvent()
{
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

    short cur_slot = info->slot;
    byte cur_track = info->track;
    enum mode cur_mode;
    enum random random_state;
    bool repeat;
    byte cur_userfile;


    // determine the current mode
    switch ( info->mode )
    {
        case 0:
        case 1:
        case 2:
            cur_mode = TrackMode;
            random_state = (enum random) (info->mode-0);
            break;
        case 3:
            cur_mode = ProgramMode;
            random_state = RandomOff; // not applicable
            break;
        case 4:
            cur_mode = BestMode;
            random_state = RandomOff; // not applicable
            break;
        case 5:
        case 6:
            cur_mode = MusicTypeMode;
            random_state = (info->mode-5 !=0) ? RandomAll : RandomOff;
            break;
        case 7:
        case 8:
        case 9:
            cur_mode = UserfileMode;
            random_state = (enum random) (info->mode-7);
            break;
    }

    repeat = (info->repeat!=0);
    cur_userfile = info->userfile;

    // notify listener
    for (int i=0; i<m_listeners.size(); i++)
    {
        if ( m_listeners[i]->InfoChanged(cur_slot, cur_track, cur_mode, 
                                        random_state, repeat, 
                                        cur_userfile) )
            break;
    }
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

    enum state cur_state = Unknown;

    // determine the current state
    switch ( info->state )
    {
        case STOPPED_STATE:  cur_state = Stopped;      break;
        case STOPPING_STATE: cur_state = Stopping;     break;
        case CHANGING_STATE: cur_state = Changing;     break;
        case PLAYING_STATE:  cur_state = Playing;      break;
        case PAUSED_STATE:   cur_state = Paused;       break;
        case SKIPFORW_STATE: cur_state = SkipForward;  break;
        case SKIPBACK_STATE: cur_state = SkipBackward; break;
        default:
            ::fprintf(stderr, "state=%d\n", info->state);
            cur_state = Unknown;
            break;
    }

    // notify listener
    for (int i=0; i<m_listeners.size(); i++)
    {
        if ( m_listeners[i]->StateChanged(cur_state) )
            break;
    }
}
 
void
KenwoodChanger::DoDiscEvent(const payload& event)
{
    // cast the payload
    DiscEvent* info = (DiscEvent*) event.data;

    // notify listener
    for (int i=0; i<m_listeners.size(); i++)
    {
        if ( m_listeners[i]->DiscChanged(info->slot) )
            break;
    }
}
 
void
KenwoodChanger::DoDoorEvent(const payload& event)
{
    // cast the payload
    DoorEvent* info = (DoorEvent*) event.data;

    // notify listener
    for (int i=0; i<m_listeners.size(); i++)
    {
        if ( m_listeners[i]->DoorChanged((info->door_pos==0)) )
            break;
    }
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
    DataAccess query = { 0, 1, slot, 0, x, 0 };
    DoDiscQuery(query);
}

void
KenwoodChanger::DoListTrackTimes(const short slot)
{
    DataAccess query = { 0, 4, slot, 0, 1, 0 };
    DoDiscQuery(query);
}

void
KenwoodChanger::DoListBest()
{
    DataAccess query = { 0, 32, 0, 0, 0, 0 };
    DoDiscQuery(query);
}

void
KenwoodChanger::DoChangeDisc(const short slot, enum state cur_state)
{
    // build the payload
    payload req;
    req.cmd = SELECT_DISC_TRACK;
    req.len = sizeof(SelectDiscTrack);
    SelectDiscTrack* select_disc = (SelectDiscTrack*) &req.data;
    select_disc->slot = slot;
    select_disc->track = 1;
    select_disc->begin = (cur_state==Playing)?1:0;

    // issue the request
    m_device.SendMessage(req, NO_REPLIES); 
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
    m_device.SendMessage(req, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        switch ( reply.cmd )
        {
            case DISC_DATA:
                for (int i=0; i<m_listeners.size(); i++)
                {
                    if ( m_listeners[i]->DiscDataReply((DiscData*)reply.data) )
                        break;
                }
                break;

            case CD_TEXT_DATA:
                for (int i=0; i<m_listeners.size(); i++)
                {
                    if ( m_listeners[i]->CDTextDataReply((CDTextData*)reply.data) )
                        break;
                }
                break;

            case TRACK_TIMES:
                for (int i=0; i<m_listeners.size(); i++)
                {
                    if ( m_listeners[i]->TrackTimesReply((TrackTimes*)reply.data) )
                        break;
                }
                break;

            case DISC_TRACK_LIST:
                for (int i=0; i<m_listeners.size(); i++)
                {
                    if ( m_listeners[i]->DiscTrackListReply((DiscTrackList*)reply.data) )
                        break;
                }
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
    m_device.SendMessage(req, NO_REPLIES); 
}

void
KenwoodChanger::IssueRequest(const payload& msg, const bool has_replies)
{
    m_device.SendMessage(msg, has_replies);
}

void
KenwoodChanger::GetOneReply(payload& reply)
{
    if ( m_device.RecvMessage(reply) )
    {
        payload eor;
        m_device.RecvMessage(eor);
    }
}

bool
KenwoodChanger::GetReply(payload& reply)
{
    return m_device.RecvMessage(reply);
}

bool
KenwoodChanger::GetEvent(payload& event)
{
    return m_device.RecvMessage(event);
}

