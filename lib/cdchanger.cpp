#include "cdchanger.h"
#include "cdpayload.h"
#include "discid.h"
#include "util.h"

CDChanger::CDChanger(char* id, KenwoodDevice& dev) 
: KenwoodChanger(id, dev)
{
}

CDChanger::~CDChanger()
{
}

void
CDChanger::ProcessEvent()
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
CDChanger::DoInfoEvent(const payload& event)
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
CDChanger::DoStateEvent(const payload& event)
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
CDChanger::DoDiscEvent(const payload& event)
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
CDChanger::DoDoorEvent(const payload& event)
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
CDChanger::DoListDiscs()
{
    DataAccess query = { 0, 1, 0, 0, 0, 0 };
    DoDiscQuery((byte*) &query);
}

void
CDChanger::DoListContents(const short slot)
{
    DataAccess query = { 0, 1, slot, 0, 1, 0 };
    DoDiscQuery((byte*) &query);
}

uint
CDChanger::GetDiscId(const short slot)
{
    // build the payload
    DataAccess query = { 0, 4, slot, 0, 1, 0 };
    payload req;
    req.cmd = DATA_ACCESS;
    req.len = sizeof(DataAccess);
    ::memcpy(req.data, &query, req.len);

    // issue the request
    m_device.SendMessage(req, HAS_REPLIES); 

    uint disc_id = 0;

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        TrackTimes* info = (TrackTimes*)reply.data;
        TimeInfo* times = (TimeInfo*) &(info->times);
        disc_id = discid(info->num_tracks, times);
    }

    return disc_id;
}

void
CDChanger::DoListBest()
{
    DataAccess query = { 0, 32, 0, 0, 0, 0 };
    DoDiscQuery((byte*) &query);
}

void
CDChanger::DoChangeDisc(const short slot, enum state cur_state)
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
CDChanger::DoPlayPause()
{
    DoChangeState(PLAY_PAUSE_CMD | STATE_PARAM);
}

void
CDChanger::DoPrev()
{
    DoChangeState(PREV_CMD | STATE_PARAM);
    DoChangeState(NULL_PARAM);
}

void
CDChanger::DoNext()
{
    DoChangeState(NEXT_CMD | STATE_PARAM);
    DoChangeState(NULL_PARAM);
}

void
CDChanger::DoStop()
{
    DoChangeState(STOP_CMD | STATE_PARAM);
}

void
CDChanger::DoDiscQuery(const byte* query)
{
    // build the payload
    payload req;
    req.cmd = DATA_ACCESS;
    req.len = sizeof(DataAccess);
    ::memcpy(req.data, query, req.len);

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
                    DiscData* info = (DiscData*) reply.data;
                    if ( m_listeners[i]->DiscDataReply(info->slot,
                                                       info->track,
                                                       info->userfiles,
                                                       info->request_type,
                                                       info->genre,
                                                       info->formatting,
                                                       info->title) )
                        break;
                }
                break;

            case CD_TEXT_DATA:
                for (int i=0; i<m_listeners.size(); i++)
                {
                    CDTextData* info = (CDTextData*) reply.data;
                    if ( m_listeners[i]->CDTextDataReply(info->slot,
                                                         info->track,
                                                         info->request_type,
                                                         info->formatting,
                                                         info->title) )
                        break;
                }
                break;

            case DISC_TRACK_LIST:
                for (int i=0; i<m_listeners.size(); i++)
                {
                    DiscTrackList* info = (DiscTrackList*) reply.data;
                    if ( m_listeners[i]->DiscTrackListReply(info->num_tracks, 
                                                            info->tracks) )
                        break;
                }
                break;
        }
    }
}

void
CDChanger::DoChangeState(const short state)
{
    // build the payload
    payload req;
    req.cmd = DO_ACTION;
    req.len = sizeof(DoAction);
    ::memcpy(req.data, &state, req.len);

    // issue the request
    m_device.SendMessage(req, NO_REPLIES); 
}


