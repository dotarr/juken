#include "cdchanger.h"
#include "cdpayload.h"
#include "discid.h"
#include "cdconstants.h"

CDChanger::CDChanger(char* id, KenwoodDevice& dev, KenwoodListener* listener) 
: KenwoodChanger(id, 200, dev, listener)
{
    InfoMsg("CDChanger::CDChanger(%s, device, listener)\n", id);
    
    // process InfoChanged
    DoEvent();
    // process DoorChanged
    DoEvent();
    // process StateChanged
    DoEvent();
    // process DiscChanged
    DoEvent();
    //DoAnyEvents();

    if ( m_cur_door_pos == DoorClosed )
        ScanDiscs();
    LoadUserfiles();
}

CDChanger::~CDChanger()
{
    InfoMsg("CDChanger::~CDChanger()\n");
}

void
CDChanger::DoInfoEvent(const payload& event)
{
    InfoMsg("CDChanger::DoInfoEvent()\n");
    cd_InfoEvent info(event);

    if ( info_changed(info.slot(), info.track()) )
    {
        InfoChanged(info.slot(), info.track(), 0);
    }
    if ( mode_changed(info.mode()) || 
         repeat_changed(info.repeat()) || 
         program_changed(info.program(), info.mode()) || 
         param_changed(info.param(), info.mode()) )
    {
        byte param = (info.mode()==ProgramMode ? info.program() : info.param());
        ModeChanged(info.mode(), info.repeat(), param);
    }
}

void
CDChanger::DoStateEvent(const payload& event)
{
    InfoMsg("CDChanger::DoStateEvent()\n");
    cd_StateEvent info(event);

    if ( state_changed(info.state()) )
    {
        StateChanged(info.state());
    }
}
 
void
CDChanger::DoDiscEvent(const payload& event)
{
    InfoMsg("CDChanger::DoDiscEvent()\n");
    cd_DiscEvent info(event);

    if ( info.slot() != m_cur_slot )
    {
        InfoChanged(info.slot(), 1, 0);
    }
}
 
void
CDChanger::DoDoorEvent(const payload& event)
{
    InfoMsg("CDChanger::DoDoorEvent()\n");
    cd_DoorEvent info(event);

    if ( info.door_pos() != m_cur_door_pos )
    {
        DoorChanged(info.door_pos());
    }
}

NameList
CDChanger::DoListUserfiles()
{
    InfoMsg("CDChanger::DoListUserfiles()\n");
    NameList names;

    cd_DataAccess query(RetrieveDataAccess, TextDataType, 0, UserfileNames, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        cd_TextData info(reply);
        Name data(info.index(), USERFILE_NAME, info.text());
        names.push_back(data);
    }

    return names;
}

void
CDChanger::DoListUserfiles(void* context, NameCallback* callback)
{
    InfoMsg("CDChanger::DoListUserfiles(callback)\n");
    cd_DataAccess query(RetrieveDataAccess, TextDataType, 0, UserfileNames, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        cd_TextData info(reply);
        Name data(info.index(), USERFILE_NAME, info.text());
        (*callback)(context, data);
    }
}

void
CDChanger::DoListDiscs(void* context, DiscCallback* callback)
{
    InfoMsg("CDChanger::DoListDiscs(callback)\n");
    // build the payload
    cd_DataAccess query(RetrieveDataAccess, TextDataType, AllSlots, DiscNames, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        cd_TextData info(reply);
        Disc data(info.slot(), DISC_CD_A, info.text(), NULL, info.userfiles(), info.genre());
        (*callback)(context, data);
    }
}

Disc
CDChanger::DoListContents(const short slot)
{
    InfoMsg("CDChanger::DoListContents(%d)\n", slot);
    m_listener->ProgressStart(this, KenwoodListener::ReadingDisc, 0);
    
    // build the payload
    cd_DataAccess query(RetrieveDataAccess, TextDataType, slot, TrackNames, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    Disc data;
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        cd_TextData info(reply);
        if ( info.index() == 0 )
        {
            data.index = info.slot();
            data.type = DISC_CD_A;
            data.title = ::strdup(info.text());
            data.userfiles = info.userfiles();
            data.genre = info.genre();
        }
        else
        {
            Name track(info.index(), TRACK_NAME, info.text());
            data.tracks.push_back(track);
        }
        m_listener->Progress(this, KenwoodListener::ReadingDisc, 
                             info.index(), info.text());
    }

    m_listener->ProgressEnd(this, KenwoodListener::ReadingDisc);
    return data;
}

void
CDChanger::DoListContents(const short slot, void* context, DiscCallback* callback)
{
    InfoMsg("CDChanger::DoListContents(callback)\n");
    Disc data = DoListContents(slot);
    (*callback)(context, data);
}

Info
CDChanger::GetDiscInfo(const short slot)
{
    InfoMsg("CDChanger::GetDiscInfo(%d)\n", slot);
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);

    // build the payload
    cd_DataAccess query(RetrieveDataAccess, InfoDataType, slot, 0, UNKNOWN);
    
    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    char* id = NULL;

    // get the replies
    payload reply;
    GetOneReply(reply);

    // process the data
    cd_DiscInfo info(reply);

    return Info(info.slot(), DISC_CD_A, info.length());
}

char*
CDChanger::GetDiscId(const short slot)
{
    InfoMsg("CDChanger::GetDiscId(%d)\n", slot);
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);
    
    // build the payload
    cd_DataAccess query(RetrieveDataAccess, TOCDataType, slot, TrackNames, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    char* id = NULL;

    // get the replies
    payload reply;
    GetOneReply(reply);
    
    // process the data
    cd_DiscTOC info(reply);
    uint disc_id = info.disc_id();
    id = new char[8+1];
    sprintf(id, "%08x", disc_id);

    return id;
}

byte
CDChanger::GetDiscUserfiles(const short slot)
{
    InfoMsg("CDChanger::GetDiscUserfiles(%d)\n", slot);
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);

    // build the payload
    cd_DataAccess query(RetrieveDataAccess, TOCDataType, slot, 0, UNKNOWN);
    
    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    GetOneReply(reply);

    // process the data
    cd_DiscUserfiles info(reply);

    return info.userfiles();
}

enum genre
CDChanger::GetDiscGenre(const short slot)
{
    InfoMsg("CDChanger::GetDiscGenre(%d)\n", slot);
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);

    // build the payload
    cd_DataAccess query(RetrieveDataAccess, TOCDataType, slot, 0, UNKNOWN);
    
    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    GetOneReply(reply);

    // process the data
    cd_DiscGenre info(reply);

    return info.genre();
}

void
CDChanger::DoChangeDisc(const short slot)
{
    InfoMsg("CDChanger::DoChangeDisc(%d)\n", slot);
    // build the payload
    cd_ChangeDisc req(slot, 1, 0);

    // issue the request
    IssueRequest(req, NO_REPLIES); 
    
    // StateEvent
    DoEvent();
    // InfoEvent
    DoEvent();
    // DiscEvent
    DoEvent();
    // StateEvent
    DoEvent();
    // StateEvent
    DoEvent();
    //DoAnyEvents();
}

void
CDChanger::DoPlayPause()
{
    InfoMsg("CDChanger::DoPlayPause()\n");
    // build the payload
    cd_DoAction req(PLAY_PAUSE_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req, NO_REPLIES); 
}

void
CDChanger::DoPrev()
{
    InfoMsg("CDChanger::DoPrev()\n");
    // build the payload
    cd_DoAction req1(PREV_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req1, NO_REPLIES); 
    
    // build the payload
    cd_DoAction req2(NULL_PARAM);

    // issue the request
    IssueRequest(req2, NO_REPLIES); 
}

void
CDChanger::DoNext()
{
    InfoMsg("CDChanger::DoNext()\n");
    // build the payload
    cd_DoAction req1(NEXT_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req1, NO_REPLIES); 
    
    // build the payload
    cd_DoAction req2(NULL_PARAM);

    // issue the request
    IssueRequest(req2, NO_REPLIES); 
}

void
CDChanger::DoStop()
{
    InfoMsg("CDChanger::DoStop()\n");
    // build the payload
    cd_DoAction req(STOP_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req, NO_REPLIES); 
}

void
CDChanger::WriteUserfileNames(NameList& names)
{
    InfoMsg("CDChanger::WriteUserfileNames()\n");
    m_listener->ProgressStart(this, KenwoodListener::WritingUserfiles, 8);
    cd_DataAccess query(WriteUserfilesAccess, ReadyDataType, 0, 1, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the reply
    payload reply;
    GetOneReply(reply);

    for (NameList::iterator iter=names.begin(); iter!=names.end(); iter++)
    {
        Name& name = (*iter);
        m_listener->Progress(this, KenwoodListener::WritingUserfiles, 
                             name.index+1, (const char*)name);
        cd_TextData data(0, 1<<(name.index), 0, UserfileNames, 0, 0, name.text);
        IssueRequest(data, NO_REPLIES); 
    }
    m_listener->ProgressEnd(this, KenwoodListener::WritingUserfiles);
}

void
CDChanger::WriteDisc(short slot, Disc& disc)
{
    InfoMsg("CDChanger::WriteDisc(%s)\n", (const char*) disc);
    m_listener->ProgressStart(this, KenwoodListener::WritingDisc, disc.tracks.size()+1);
    cd_DataAccess query(WriteTextAccess, ReadyDataType, slot, 1, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the reply
    payload reply;
    GetOneReply(reply);

    m_listener->Progress(this, KenwoodListener::WritingDisc, 
                         0, (const char*)disc);
    cd_TextData data(slot, 0, disc.userfiles, DiscNames, 
                  disc.genre, 0, disc.title);
    IssueRequest(data, NO_REPLIES); 

    short count = 0;
    NameList& tracks = disc.tracks;
    for (NameList::iterator iter=tracks.begin(); iter!=tracks.end(); iter++)
    {
        if ( ++count > 20 ) continue;

        Name& track = (*iter);
        m_listener->Progress(this, KenwoodListener::WritingDisc, 
                             track.index, (const char*)track);
        cd_TextData data(slot, track.index, disc.userfiles, TrackNames, 
                      disc.genre, 0, track.text);
        IssueRequest(data, NO_REPLIES); 
    }
    m_listener->ProgressEnd(this, KenwoodListener::WritingDisc);
}

bool 
CDChanger::info_changed(short slot, byte track)
{
    return ( (slot!=m_cur_slot) || (track!=m_cur_title) ); 
}

bool 
CDChanger::mode_changed(enum mode mode)
{
    return (mode != m_cur_mode);
}

bool 
CDChanger::state_changed(enum state state)
{
    return (state != m_cur_state);
}

bool 
CDChanger::program_changed(byte program, enum mode mode)
{
    return ( (program!=m_cur_param) && (mode==ProgramMode) );
}

bool 
CDChanger::repeat_changed(enum repeat repeat)
{
    return (repeat != m_cur_repeat);
}

bool 
CDChanger::param_changed(byte param, enum mode mode)
{
    return ( (param!=m_cur_param) && (mode>=MusicTypeMode) );
}


