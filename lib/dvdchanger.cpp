#include "dvdchanger.h"
#include "dvdpayload.h"
#include "discid.h"
#include "util.h"
#include "dvdconstants.h"

DVDChanger::DVDChanger(char* id, KenwoodDevice& dev) 
: KenwoodChanger(id, dev)
{
    m_setup = false;

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

    if ( m_setup || info_changed(info) )
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
    if ( m_setup || program_changed(info) )
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

    if ( m_setup || state_changed(info) )
    {
        m_cur_state = info.state();
        // notify listener
        for (int i=0; i<m_listeners.size(); i++)
        {
            if ( m_listeners[i]->StateChanged(info.state()) )
                break;
        }
    }
    if ( m_setup || mode_changed(info) || repeat_changed(info) || param_changed(info) )
    {
        m_setup = true;
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
DVDChanger::DoQuery(byte a, byte b, byte c,
                    short slot, byte title, short chapter)
{
    // build the payload
    DataAccess query((enum access)a, (enum data_type)b, (dvd_info_type)c, 
                     1, slot, title, chapter);

printf("query cmd=0x%02X len=%d\n", query.cmd, query.len);
printdata(query.data, query.len);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
printf("reply cmd=0x%02X len=%d\n", reply.cmd, reply.len);
printdata(reply.data, reply.len);
    }
}

void
DVDChanger::DoListDiscs()
{
    // build the payload
    DataAccess query(RetrieveData, Text, DVDDiscNames, 1, 0, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

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
                                               0,
                                               CDDiscNames,
                                               0,
                                               0,
                                               info.text()) )
                break;
        }
    }
}

void
DVDChanger::DoListContents(const short slot)
{
    // build the payload
    DataAccess query(RetrieveData, Text, DVDChapterNames, 1, slot, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

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
                                               0,
                                               CDDiscNames,
                                               0,
                                               0,
                                               info.text()) )
                break;
        }
    }
}

char*
DVDChanger::GetDiscId(const short slot)
{
    // build the payload
    DataAccess query(RetrieveData, TOC, DVDCDTOC, 1, slot, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    char* id = NULL;

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
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
    ChangeDisc req(1, slot, 0, 0, TrackMode, 0x00, 2);

    // issue the request
    IssueRequest(req, NO_REPLIES); 
}

void
DVDChanger::DoPlayPause()
{
    // build the payload
    DoAction req(1, PLAY_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req, NO_REPLIES); 
}

void
DVDChanger::DoPrev()
{
    // build the payload
    DoAction req1(1, PREV_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req1, NO_REPLIES); 
    
    // build the payload
    DoAction req2(1, PLAY_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req2, NO_REPLIES); 
}

void
DVDChanger::DoNext()
{
    // build the payload
    DoAction req1(1, NEXT_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req1, NO_REPLIES); 
    
    // build the payload
    DoAction req2(1, PLAY_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req2, NO_REPLIES); 
}

void
DVDChanger::DoStop()
{
    // build the payload
    DoAction req(1, STOP_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req, NO_REPLIES); 
}

void
DVDChanger::WriteUserfileNames(const char* names[])
{
    DataAccess query(SetUserfiles, Ready, DVDDiscNamesInGenre, 1, 0, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 
usleep(10);

    // get the reply
    payload reply;
    GetOneReply(reply);
usleep(10);

    for (int i=0; i<8; i++)
    {
        TextData data(1, 6, 1<<i, 0, 0, 0, names[i]);
        IssueRequest(data, NO_REPLIES); 
    }
usleep(10);
}

void
DVDChanger::WriteTitleArtist(short slot, const char* title, const char* artist)
{
    DataAccess query(WriteText, Ready, DVDChapterNames, 1, slot, 0, 0);
//printf("query cmd=0x%02X len=%d\n", query.cmd, query.len);
//printdata(query.data, query.len);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 
usleep(10);

    // get the reply
    payload reply;
    GetOneReply(reply);
usleep(10);

    TextData data(1, DVDDiscText, slot, 0, 0, 0, title);
//printf("data  cmd=0x%02X len=%d\n", data.cmd, data.len);
//printdata(data.data, data.len);
    IssueRequest(data, NO_REPLIES); 
usleep(10);

    if ( artist == NULL )
    {
        const char none[] = { 0x01 };
        TextData data(1, 3, slot, 0, 0, 0, none);
//printf("data  cmd=0x%02X len=%d\n", data.cmd, data.len);
//printdata(data.data, data.len);
        IssueRequest(data, NO_REPLIES); 
    }
    else
    {
        TextData data(1, 3, slot, 0, 0, 0, artist);
//printf("data  cmd=0x%02X len=%d\n", data.cmd, data.len);
//printdata(data.data, data.len);
        IssueRequest(data, NO_REPLIES); 
    }
usleep(10);
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


