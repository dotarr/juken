#include "cdchanger.h"
#include "cdpayload.h"
#include "discid.h"
#include "cdconstants.h"

CDChanger::CDChanger(char* id, KenwoodDevice& dev, KenwoodListener* listener) 
: KenwoodChanger(id, 200, dev, listener)
{
    InfoMsg("CDChanger::CDChanger(%s, device, listener)\n", id);
    //
    // process InfoChanged
    // process DoorChanged
    // process StateChanged
    // process DiscChanged
    DoAnyEvents();

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
    InfoEvent info(event);

    if ( info_changed(info) )
    {
        InfoChanged(info.slot(), info.track(), 0);
    }
    if ( mode_changed(info) || repeat_changed(info) || 
         program_changed(info) || param_changed(info) )
    {
        byte param = (info.mode()==ProgramMode ? info.program() : info.param());
        ModeChanged(info.mode(), info.repeat(), param);
    }
}

void
CDChanger::DoStateEvent(const payload& event)
{
    InfoMsg("CDChanger::DoStateEvent()\n");
    StateEvent info(event);

    if ( state_changed(info) )
    {
        StateChanged(info.state());
    }
}
 
void
CDChanger::DoDiscEvent(const payload& event)
{
    InfoMsg("CDChanger::DoDiscEvent()\n");
    DiscEvent info(event);

    if ( info.slot() != m_cur_slot )
    {
        InfoChanged(info.slot(), 1, 0);
    }
}
 
void
CDChanger::DoDoorEvent(const payload& event)
{
    InfoMsg("CDChanger::DoDoorEvent()\n");
    DoorEvent info(event);

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

    DataAccess query(RetrieveDataAccess, TextDataType, 0, UserfileNames, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        TextData info(reply);
        Name data(info.index(), USERFILE_NAME, info.text());
        names.push_back(data);
    }

    return names;
}

void
CDChanger::DoListUserfiles(void* context, NameCallback* callback)
{
    InfoMsg("CDChanger::DoListUserfiles(callback)\n");
    DataAccess query(RetrieveDataAccess, TextDataType, 0, UserfileNames, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        TextData info(reply);
        Name data(info.index(), USERFILE_NAME, info.text());
        (*callback)(context, data);
    }
}

void
CDChanger::DoListDiscs(void* context, DiscCallback* callback)
{
    InfoMsg("CDChanger::DoListDiscs(callback)\n");
    // build the payload
    DataAccess query(RetrieveDataAccess, TextDataType, AllSlots, DiscNames, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        TextData info(reply);
        Disc data(info.slot(), DISC_CD_A, info.text(), NULL, info.userfiles(), info.genre());
        (*callback)(context, data);
    }
}

Disc
CDChanger::DoListContents(const short slot)
{
    InfoMsg("CDChanger::DoListContents(%d)\n", slot);
    // build the payload
    DataAccess query(RetrieveDataAccess, TextDataType, slot, TrackNames, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    Disc data;
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        TextData info(reply);
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
    }

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
    DataAccess query(RetrieveDataAccess, InfoDataType, slot, 0, UNKNOWN);
    
    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    char* id = NULL;

    // get the replies
    payload reply;
    GetOneReply(reply);

    // process the data
    DiscInfo info(reply);

    return Info(info.slot(), DISC_CD_A, info.length());
}

char*
CDChanger::GetDiscId(const short slot)
{
    InfoMsg("CDChanger::GetDiscId(%d)\n", slot);
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);
    
    // build the payload
    DataAccess query(RetrieveDataAccess, TOCDataType, slot, TrackNames, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    char* id = NULL;

    // get the replies
    payload reply;
    GetOneReply(reply);
    
    // process the data
    DiscTOC info(reply);
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
    DataAccess query(RetrieveDataAccess, TOCDataType, slot, 0, UNKNOWN);
    
    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    GetOneReply(reply);

    // process the data
    DiscUserfiles info(reply);

    return info.userfiles();
}

enum genre
CDChanger::GetDiscGenre(const short slot)
{
    InfoMsg("CDChanger::GetDiscGenre(%d)\n", slot);
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);

    // build the payload
    DataAccess query(RetrieveDataAccess, TOCDataType, slot, 0, UNKNOWN);
    
    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    GetOneReply(reply);

    // process the data
    DiscGenre info(reply);

    return info.genre();
}

void
CDChanger::DoChangeDisc(const short slot)
{
    InfoMsg("CDChanger::DoChangeDisc(%d)\n", slot);
    // build the payload
    ChangeDisc req(slot, 1, 0);

    // issue the request
    IssueRequest(req, NO_REPLIES); 
    
    // StateEvent
    // InfoEvent
    // DiscEvent
    // StateEvent
    // StateEvent
    DoAnyEvents();
}

void
CDChanger::DoPlayPause()
{
    InfoMsg("CDChanger::DoPlayPause()\n");
    // build the payload
    DoAction req(PLAY_PAUSE_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req, NO_REPLIES); 
}

void
CDChanger::DoPrev()
{
    InfoMsg("CDChanger::DoPrev()\n");
    // build the payload
    DoAction req1(PREV_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req1, NO_REPLIES); 
    
    // build the payload
    DoAction req2(NULL_PARAM);

    // issue the request
    IssueRequest(req2, NO_REPLIES); 
}

void
CDChanger::DoNext()
{
    InfoMsg("CDChanger::DoNext()\n");
    // build the payload
    DoAction req1(NEXT_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req1, NO_REPLIES); 
    
    // build the payload
    DoAction req2(NULL_PARAM);

    // issue the request
    IssueRequest(req2, NO_REPLIES); 
}

void
CDChanger::DoStop()
{
    InfoMsg("CDChanger::DoStop()\n");
    // build the payload
    DoAction req(STOP_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req, NO_REPLIES); 
}

void
CDChanger::WriteUserfileNames(const char* names[])
{
    InfoMsg("CDChanger::WriteUserfileNames(%s, %s, %s, %s, %s, %s, %s, %s)\n",
            names[0], names[1], names[2], names[3],
            names[4], names[5], names[6], names[7]);
    DataAccess query(WriteUserfilesAccess, ReadyDataType, 0, 1, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the reply
    payload reply;
    GetOneReply(reply);

    for (int i=0; i<8; i++)
    {
        TextData data(0, 1<<i, 0, UserfileNames, 0, 0, names[i]);
        IssueRequest(data, NO_REPLIES); 
    }
}

void
CDChanger::WriteDisc(short slot, Disc& disc)
{
    InfoMsg("CDChanger::WriteDisc(%s)\n", (const char*) disc);
    // make sure slot is current
    //if ( slot != m_cur_slot ) DoChangeDisc(slot);

    const char none[] = { 0x01 };
    DataAccess query(WriteTextAccess, ReadyDataType, slot, 1, UNKNOWN);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the reply
    payload reply;
    GetOneReply(reply);

    TextData data(slot, 0, disc.userfiles, DiscNames, disc.genre, 0, disc.title);
    IssueRequest(data, NO_REPLIES); 

    short count = 0;
    NameList& tracks = disc.tracks;
    for (NameList::iterator iter=tracks.begin(); iter!=tracks.end(); iter++)
    {
        if ( ++count > 20 ) continue;

        const Name& track = (*iter);
        if ( track.text == NULL )
        {
            TextData data(slot, track.index, 0, TrackNames, disc.genre, 0, none);
            IssueRequest(data, NO_REPLIES); 
        }
        else
        {
            TextData data(slot, track.index, 0, TrackNames, disc.genre, 0, track.text);
            IssueRequest(data, NO_REPLIES); 
        }
    }
}

bool 
CDChanger::info_changed(const InfoEvent& info)
{
    return ( (info.slot()!=m_cur_slot) || 
             (info.track()!=m_cur_title) ); 
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
    return ( (info.program()!=m_cur_param) &&
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


