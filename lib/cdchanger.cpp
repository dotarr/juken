#include "cdchanger.h"
#include "cdpayload.h"
#include "discid.h"
#include "util.h"

CDChanger::CDChanger(char* id, KenwoodDevice& dev) 
: KenwoodChanger(id, dev)
{
    m_cur_slot = -1;
    m_cur_track = (byte) -1;
    m_cur_mode = UnknownMode;
    m_cur_repeat = UnknownRepeat;
    m_cur_param = (byte) -1;
    m_cur_program = (byte) -1;
    m_cur_state = UnknownState;
    m_cur_door_open = DoorUnknown;
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
    InfoEvent info(event);

    if ( info_changed(info) )
    {
        m_cur_slot = info.slot();
        m_cur_track = info.track();
        // notify listener
        for (int i=0; i<m_listeners.size(); i++)
        {
            if ( m_listeners[i]->InfoChanged(info.slot(), info.track(), 0) )
                break;
        }
    }
    if ( mode_changed(info) || repeat_changed(info) || 
         program_changed(info) || param_changed(info) )
    {
        m_cur_mode = info.mode();
        m_cur_repeat = info.repeat();
        m_cur_program = info.program();
        m_cur_param = info.param();
        // notify listener
        for (int i=0; i<m_listeners.size(); i++)
        {
            if ( m_listeners[i]->ModeChanged(info.mode(), 
                                             info.repeat()==RepeatOn, 
                                             (info.mode()==ProgramMode
                                                ? info.program() 
                                                : info.param())) )
                break;
        }
    }
}

void
CDChanger::DoStateEvent(const payload& event)
{
    // cast the payload
    StateEvent info(event);

    if ( state_changed(info) )
    {
        m_cur_state = info.state();
        // notify listener
        for (int i=0; i<m_listeners.size(); i++)
        {
            if ( m_listeners[i]->StateChanged(info.state()) )
                break;
        }
    }
}
 
void
CDChanger::DoDiscEvent(const payload& event)
{
    // cast the payload
    DiscEvent info(event);

    if ( info.slot() != m_cur_slot )
    {
        m_cur_slot = info.slot();
        m_cur_track = 1;
        // notify listener
        for (int i=0; i<m_listeners.size(); i++)
        {
            if ( m_listeners[i]->InfoChanged(info.slot(), 1, 0) )
                break;
        }
    }
}
 
void
CDChanger::DoDoorEvent(const payload& event)
{
    // cast the payload
    DoorEvent info(event);

    if ( info.door_open() != m_cur_door_open )
    {
        m_cur_door_open = info.door_open();
        // notify listener
        for (int i=0; i<m_listeners.size(); i++)
        {
            if ( m_listeners[i]->DoorChanged(info.door_open()==DoorOpen) )
                break;
        }
    }
}

void
CDChanger::DoListDiscs()
{
    // build the payload
    DataAccess query(RetrieveData, Text, AllSlots, CDDiscNames, UNKNOWN);

    // issue the request
    m_device.SendMessage(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        TextData info(reply);
        for (int i=0; i<m_listeners.size(); i++)
        {
            if ( m_listeners[i]->TextDataReply(info.slot(),
                                               info.track(),
                                               info.userfiles(),
                                               info.info_type(),
                                               info.genre(),
                                               info.formatting(),
                                               info.text()) )
                break;
        }
    }
}

void
CDChanger::DoListContents(const short slot)
{
    // build the payload
    DataAccess query(RetrieveData, Text, slot, CDTrackNames, UNKNOWN);

    // issue the request
    m_device.SendMessage(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        TextData info(reply);
        for (int i=0; i<m_listeners.size(); i++)
        {
            if ( m_listeners[i]->TextDataReply(info.slot(),
                                               info.track(),
                                               info.userfiles(),
                                               info.info_type(),
                                               info.genre(),
                                               info.formatting(),
                                               info.text()) )
                break;
        }
    }
}

char*
CDChanger::GetDiscId(const short slot)
{
    // build the payload
    DataAccess query(RetrieveData, TOC, slot, CDTrackNames, UNKNOWN);

    // issue the request
    m_device.SendMessage(query, HAS_REPLIES); 

    char* id = NULL;

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        DiscTOC info(reply);
        uint disc_id = info.disc_id();
        id = new char[8+1];
        sprintf(id, "%08x", disc_id);
    }

    return id;
}

void
CDChanger::DoListBest()
{
    // build the payload
    DataAccess query(RetrieveData, Listing, AllSlots, CDDiscNames, UNKNOWN);

    // issue the request
    m_device.SendMessage(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        DiscListing info(reply);
        for (int i=0; i<m_listeners.size(); i++)
        {
            if ( m_listeners[i]->DiscTrackListReply(info.length(), info.tracks()) )
                break;
        }
    }
}

void
CDChanger::DoChangeDisc(const short slot, enum state cur_state)
{
    // build the payload
    ChangeDisc req(slot, 1, cur_state==Playing);

    // issue the request
    m_device.SendMessage(req, NO_REPLIES); 
}

void
CDChanger::DoPlayPause()
{
    // build the payload
    DoAction req(PLAY_PAUSE_CMD | STATE_PARAM);

    // issue the request
    m_device.SendMessage(req, NO_REPLIES); 
}

void
CDChanger::DoPrev()
{
    // build the payload
    DoAction req1(PREV_CMD | STATE_PARAM);

    // issue the request
    m_device.SendMessage(req1, NO_REPLIES); 
    
    // build the payload
    DoAction req2(NULL_PARAM);

    // issue the request
    m_device.SendMessage(req2, NO_REPLIES); 
}

void
CDChanger::DoNext()
{
    // build the payload
    DoAction req1(NEXT_CMD | STATE_PARAM);

    // issue the request
    m_device.SendMessage(req1, NO_REPLIES); 
    
    // build the payload
    DoAction req2(NULL_PARAM);

    // issue the request
    m_device.SendMessage(req2, NO_REPLIES); 
}

void
CDChanger::DoStop()
{
    // build the payload
    DoAction req(STOP_CMD | STATE_PARAM);

    // issue the request
    m_device.SendMessage(req, NO_REPLIES); 
}

bool 
CDChanger::info_changed(const InfoEvent& info)
{
    return ( (info.slot()!=m_cur_slot) || 
             (info.track()!=m_cur_track) ); 
}

bool 
CDChanger::mode_changed(const InfoEvent& info)
{
    return (info.mode() != m_cur_mode);
}

bool 
CDChanger::state_changed(const StateEvent& info)
{
    return (info.state() != m_cur_state);
}

bool 
CDChanger::program_changed(const InfoEvent& info)
{
    return ( (info.program()!=m_cur_program) &&
             (info.mode()==ProgramMode) );
}

bool 
CDChanger::repeat_changed(const InfoEvent& info)
{
    return (info.repeat() != m_cur_repeat);
}

bool 
CDChanger::param_changed(const InfoEvent& info)
{
    return ( (info.param()!=m_cur_param) &&
             (info.mode()>=MusicTypeMode) );
}


