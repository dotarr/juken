#include "dvdchanger.h"
#include "dvdpayload.h"
#include "discid.h"
#include "util.h"

DVDChanger::DVDChanger(char* id, KenwoodDevice& dev) 
: KenwoodChanger(id, dev)
{
    m_cur_slot = -1;
    m_cur_title = (byte) -1;
    m_cur_chapter = -1;
    m_cur_mode = UnknownMode;
    m_cur_repeat = UnknownRepeat;
    m_cur_param = (byte) -1;
    m_cur_program = (byte) -1;
    m_cur_state = UnknownState;
    m_cur_door_open = DoorUnknown;
}

DVDChanger::~DVDChanger()
{
}

void
DVDChanger::ProcessEvent()
{
    payload event;
    while ( GetEvent(event) )
    {
        switch ( event.cmd )
        {
            case INFO_EVENT:  DoInfoEvent(event);  break;
            case STATE_EVENT: DoStateEvent(event); break;

            default:
                DebugPayload("unhandled event", event, m_device.ComputeChecksum(event));
                break;
        }
    }
}

void
DVDChanger::DoInfoEvent(const payload& event)
{
    // cast the payload
    InfoEvent info(event);

    if ( info_changed(info) )
    {
        m_cur_slot = info.slot();
        m_cur_title = info.title();
        m_cur_chapter = info.chapter();
        // notify listener
        for (int i=0; i<m_listeners.size(); i++)
        {
            if ( m_listeners[i]->InfoChanged(info.slot(), info.title(), info.chapter()) )
                break;
        }
    }
    if ( program_changed(info) )
    {
        m_cur_program = info.program();
        // notify listener
        for (int i=0; i<m_listeners.size(); i++)
        {
            if ( m_listeners[i]->ModeChanged(m_cur_mode, 
                                             m_cur_repeat==RepeatOn, 
                                             info.program()) ) 
                break;
        }
    }
}

void
DVDChanger::DoStateEvent(const payload& event)
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
    if ( mode_changed(info) || repeat_changed(info) || param_changed(info) )
    {
        m_cur_mode = info.mode();
        m_cur_repeat = info.repeat();
        m_cur_param = info.param();
        // notify listener
        for (int i=0; i<m_listeners.size(); i++)
        {
            if ( m_listeners[i]->ModeChanged(info.mode(), 
                                             info.repeat()==RepeatOn, 
                                             (info.mode()==ProgramMode
                                                ? m_cur_program 
                                                : info.param())) )
                break;
        }
    }
}
 
void
DVDChanger::DoListDiscs()
{
    // build the payload
    DataAccess query(RetrieveData, Text, DVDDiscNames, 1, 0, 0, 0);

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
            if ( m_listeners[i]->TextDataReply(info.index(),
                                               0,
                                               info.userfile(),
                                               CDDiscNames,
                                               info.genre(),
                                               info.formatting(),
                                               info.text()) )
                break;
        }
    }
}

void
DVDChanger::DoListContents(const short slot)
{
}

char*
DVDChanger::GetDiscId(const short slot)
{
printf("GetDiscId:\n");
    // build the payload
    DataAccess query(RetrieveData, TOC, DVDCDTOC, 1, slot, 0, 0);

printf("query\n");
printdata(query.data, query.len);

    // issue the request
    m_device.SendMessage(query, HAS_REPLIES); 

    char* id = NULL;

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
printf("reply\n");
printdata(reply.data, reply.len);

        // process the data
        DiscTOC info(reply);
        if ( info.formatting() == 0x20 )
        {
            uint disc_id = info.disc_id();
            id = new char[8+1];
            sprintf(id, "%08x", disc_id);
        }
        else
        {
            id = ::strdup(info.vol_id());
        }
    }

    return id;
}

void
DVDChanger::DoListBest()
{
}

void
DVDChanger::DoChangeDisc(const short slot, enum state cur_state)
{
    // build the payload
    ChangeDisc req(1, slot, 1, 1, TrackMode, 0x00, cur_state==Playing);

    // issue the request
    m_device.SendMessage(req, NO_REPLIES); 
}

void
DVDChanger::DoPlayPause()
{
}

void
DVDChanger::DoPrev()
{
}

void
DVDChanger::DoNext()
{
}

void
DVDChanger::DoStop()
{
}

bool 
DVDChanger::info_changed(const InfoEvent& info)
{
    return ( (info.slot()!=m_cur_slot) || 
             (info.title()!=m_cur_title) || 
             (info.chapter()!=m_cur_chapter) );
}

bool 
DVDChanger::mode_changed(const StateEvent& info)
{
    return (info.mode() != m_cur_mode);
}

bool 
DVDChanger::state_changed(const StateEvent& info)
{
    return (info.state() != m_cur_state);
}

bool 
DVDChanger::program_changed(const InfoEvent& info)
{
    return ( (info.program()!=m_cur_program) &&
             (m_cur_mode==ProgramMode) );
}

bool 
DVDChanger::repeat_changed(const StateEvent& info)
{
    return (info.repeat() != m_cur_repeat);
}

bool 
DVDChanger::param_changed(const StateEvent& info)
{
    return ( (info.param()!=m_cur_param) &&
             (info.mode()>=MusicTypeMode) );
}


