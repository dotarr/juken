#include "dvdchanger.h"
#include "dvdpayload.h"
#include "discid.h"
#include "dvdconstants.h"

DVDChanger::DVDChanger(char* id, KenwoodDevice& dev, KenwoodListener* listener) 
: KenwoodChanger(id, 403, dev, listener)
{
    InfoMsg("DVDChanger::DVDChanger(%s, device, listener)\n", id);
    m_chain_id = 1;

    // process InfoChanged
    // process StateChanged
    DoAnyEvents();

    if ( m_cur_door_pos == DoorClosed )
        ScanDiscs();
    LoadUserfiles();
}

DVDChanger::~DVDChanger()
{
    InfoMsg("DVDChanger::~DVDChanger()\n");
}

void
DVDChanger::DoInfoEvent(const payload& event)
{
    InfoMsg("DVDChanger::DoInfoEvent()\n");
    dvd_InfoEvent info(event);

    if ( info_changed(info.slot(), info.title(), info.chapter()) )
    {
        InfoChanged(info.slot(), info.title(), info.chapter());
    }
    if ( program_changed(info.program()) )
    {
        byte param = 0;
        if ( m_cur_mode > UserfileMode ) param = info.userfile();
        else if ( m_cur_mode > MusicTypeMode ) param = info.genre();
        else if ( m_cur_mode == ProgramMode ) param = info.program();
        ModeChanged(m_cur_mode, m_cur_repeat, param);
    }

    if ( info.toc_complete() )
        LogMsg("toc read complete\n");
}

void
DVDChanger::DoStateEvent(const payload& event)
{
    InfoMsg("DVDChanger::DoStateEvent()\n");
    dvd_StateEvent info(event);

    if ( mode_changed(info.mode()) || 
         repeat_changed(info.repeat()) || 
         param_changed(info.param(), info.mode()) )
    {
        byte param = (info.mode()==ProgramMode ? m_cur_param : info.param());
        ModeChanged(info.mode(), info.repeat(), param);
    }
    if ( info.door_pos() != m_cur_door_pos )
    {
        DoorChanged(info.door_pos());
    }
    if ( state_changed(info.state()) )
    {
        StateChanged(info.state());
    }

    if ( info.at_end() )
        LogMsg("at track/disc end\n");
    if ( info.library() )
        LogMsg("library on\n");
}
 
void
DVDChanger::DoQuery(byte a, byte b, byte c, short slot, byte title, short chapter)
{
    InfoMsg("DVDChanger::DoQuery()\n");
    // build the payload
    dvd_DataAccess query((enum access)a, (enum data_type)b, c, 
                     m_chain_id, slot, title, chapter);

    print_payload(stderr, "query", query.cmd, query.len, query.data);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        print_payload(stderr, "reply", reply.cmd, reply.len, reply.data);
    }
}

NameList
DVDChanger::DoListUserfiles()
{
    InfoMsg("DVDChanger::DoListUserfiles()\n");
    NameList names;

    dvd_DataAccess query(RetrieveDataAccess, TextDataType, UserfileNames, m_chain_id, 0, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        dvd_TextData info(reply);
        Name data(info.index(), USERFILE_NAME, info.text());
        names.push_back(data);
    }

    return names;
}

void
DVDChanger::DoListUserfiles(void* context, NameCallback* callback)
{
    InfoMsg("DVDChanger::DoListUserfiles(callback)\n");
    dvd_DataAccess query(RetrieveDataAccess, TextDataType, UserfileNames, m_chain_id, 0, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        dvd_TextData info(reply);
        Name data(info.index(), USERFILE_NAME, info.text());
        (*callback)(context, data);
    }
}

void
DVDChanger::DoListDiscs(void* context, DiscCallback* callback)
{
    InfoMsg("DVDChanger::DoListDiscs(callback)\n");
    // build the payload
    dvd_DataAccess query(RetrieveDataAccess, TextDataType, DiscNames, m_chain_id, 0, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        dvd_TextData info(reply);
        byte type = 255;
        switch ( (info.formatting()&0x7F) )
        {
            case 0x20: type = DISC_CD_A;   break;
            case 0x22: type = DISC_CD_MP3; break;
            case 0x21: type = DISC_CD_V;   break;
            case 0x10: type = DISC_DVD_A;  break;
            case 0x11: type = DISC_DVD_V;  break;
        }
        Disc data(info.index(), type, info.text(), NULL, info.userfiles(), info.genre());
        (*callback)(context, data);
    }
}

Disc
DVDChanger::DoListContents(const short slot)
{
    InfoMsg("DVDChanger::DoListContents(%d)\n", slot);
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);

    m_listener->ProgressStart(this, KenwoodListener::ReadingDisc, 0);
    // build the payload
    dvd_DataAccess query(RetrieveDataAccess, TextDataType, DiscArtistTrackNames, m_chain_id, slot, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    Disc data;
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        dvd_TextData info(reply);
        switch ( info.text_type() )
        {
            case DiscText:
            {
                data.index = info.index();
                switch ( (info.formatting()&0x7F) )
                {
                    case 0x20: data.type = DISC_CD_A;   break;
                    case 0x22: data.type = DISC_CD_MP3; break;
                    case 0x21: data.type = DISC_CD_V;   break;
                    case 0x10: data.type = DISC_DVD_A;  break;
                    case 0x11: data.type = DISC_DVD_V;  break;
                }
                data.title = ::strdup(info.text());
                data.userfiles = info.userfiles();
                data.genre = info.genre();
            }
            break;
            case TrackText:
            {
                Name track(info.index(), TRACK_NAME, info.text());
                data.tracks.push_back(track);
            }
            break;
            case ArtistText:
            {
                data.artist = ::strdup(info.text());
            }
            break;
            m_listener->Progress(this, KenwoodListener::ReadingDisc, 
                                 info.index(), info.text());
        }
    }

    m_listener->ProgressEnd(this, KenwoodListener::ReadingDisc);
    return data;
}

void
DVDChanger::DoListContents(const short slot, void* context, DiscCallback* callback)
{
    InfoMsg("DVDChanger::DoListContents(%d, callback)\n", slot);
    Disc data = DoListContents(slot);
    (*callback)(context, data);
}

Info
DVDChanger::GetDiscInfo(const short slot)
{
    InfoMsg("DVDChanger::GetDiscInfo(%d)\n", slot);
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);

    // build the payload
    dvd_DataAccess query(RetrieveDataAccess, InfoDataType, 0, m_chain_id, slot, 0, 0);
    
    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    char* id = NULL;

    // get the replies
    payload reply;
    GetOneReply(reply);

    // process the data
    dvd_DiscInfo info(reply);
    byte type = 255;
    switch ( (info.formatting()&0x7F) )
    {
        case 0x20: type = DISC_CD_A;   break;
        case 0x22: type = DISC_CD_MP3; break;
        case 0x21: type = DISC_CD_V;   break;
        case 0x10: type = DISC_DVD_A;  break;
        case 0x11: type = DISC_DVD_V;  break;
    }

    if ( (info.formatting()&0x10) != 0 )
        return Info(info.slot(), type, info.title_count());
    else
        return Info(info.slot(), type, info.length());
}

char*
DVDChanger::GetDiscId(const short slot)
{
    InfoMsg("DVDChanger::GetDiscId(%d)\n", slot);
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);

    // build the payload
    dvd_DataAccess query(RetrieveDataAccess, TOCDataType, TOCId, m_chain_id, slot, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    GetOneReply(reply);
    // process the data
    dvd_DiscTOC info(reply);
    char* id = NULL;
    switch ( (info.formatting()&0x7F) )
    {
        case 0x20: // DISC_CD_A
        {
            uint disc_id = info.disc_id();
            id = new char[8+1];
            sprintf(id, "%08x", disc_id);
        }
        break;
        case 0x22: // DISC_CD_MP3
        case 0x21: // DISC_CD_V
        case 0x10: // DISC_DVD_A
        case 0x11: // DISC_DVD_V
            id = ::strdup(info.vol_id());
        break;
    }

    return id;
}

byte
DVDChanger::GetDiscUserfiles(const short slot)
{
    InfoMsg("DVDChanger::GetDiscUserfiles(%d)\n", slot);
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);

    // build the payload
    dvd_DataAccess query(RetrieveDataAccess, TOCDataType, TOCId, m_chain_id, slot, 0, 0);
    
    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    GetOneReply(reply);
    dvd_DiscUserfiles info(reply);

    return info.userfiles();
}

enum genre
DVDChanger::GetDiscGenre(const short slot)
{
    InfoMsg("DVDChanger::GetDiscGenre(%d)\n", slot);
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);

    // build the payload
    dvd_DataAccess query(RetrieveDataAccess, TOCDataType, TOCId, m_chain_id, slot, 0, 0);
    
    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    GetOneReply(reply);
    dvd_DiscGenre info(reply);

    return info.genre();
}

void
DVDChanger::DoChangeDisc(const short slot)
{
    InfoMsg("DVDChanger::DoChangeDisc(%d)\n", slot);
    // build the payload
    dvd_ChangeDisc req(m_chain_id, slot, 0, 0, TrackMode, 0, 2);

    // issue the request
    IssueRequest(req, NO_REPLIES); 

    // StateEvent
    DoEvent();
    // InfoEvent
    DoEvent();
    // StateEvent
    DoEvent();
    //DoAnyEvents();
}

void
DVDChanger::DoPlayPause()
{
    InfoMsg("DVDChanger::DoPlayPause()\n");
    // build the payload
    dvd_DoAction req(m_chain_id, PLAY_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req, NO_REPLIES); 
}

void
DVDChanger::DoPrev()
{
    InfoMsg("DVDChanger::DoPrev()\n");
    // build the payload
    dvd_DoAction req1(m_chain_id, PREV_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req1, NO_REPLIES); 
    
    // build the payload
    dvd_DoAction req2(m_chain_id, PLAY_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req2, NO_REPLIES); 
}

void
DVDChanger::DoNext()
{
    InfoMsg("DVDChanger::DoNext()\n");
    // build the payload
    dvd_DoAction req1(m_chain_id, NEXT_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req1, NO_REPLIES); 
    
    // build the payload
    dvd_DoAction req2(m_chain_id, PLAY_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req2, NO_REPLIES); 
}

void
DVDChanger::DoStop()
{
    InfoMsg("DVDChanger::DoStop()\n");
    // build the payload
    dvd_DoAction req(m_chain_id, STOP_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req, NO_REPLIES); 
}

void
DVDChanger::WriteUserfileNames(NameList& names)
{
    InfoMsg("DVDChanger::WriteUserfileNames()\n");
    m_listener->ProgressStart(this, KenwoodListener::WritingUserfiles, 8);
    dvd_DataAccess query(WriteUserfilesAccess, ReadyDataType, AllUserfileNames, m_chain_id, 0, 0, 0);

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
        dvd_TextData data(m_chain_id, 6, 1<<(name.index), 0, 0, 0, name.text);
        IssueRequest(data, NO_REPLIES); 
    }
    m_listener->ProgressEnd(this, KenwoodListener::WritingUserfiles);
}

void
DVDChanger::WriteDisc(short slot, Disc& disc)
{
    InfoMsg("DVDChanger::WriteDisc(%s)\n", (const char*)disc);
    m_listener->ProgressStart(this, KenwoodListener::WritingDisc, disc.tracks.size()+2);
    dvd_DataAccess query(WriteTextAccess, ReadyDataType, DiscArtistNames, m_chain_id, slot, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the reply
    payload reply;
    GetOneReply(reply);

    m_listener->Progress(this, KenwoodListener::WritingDisc, 
                         0, (const char*)disc);
    dvd_TextData disc_name(m_chain_id, DiscText, slot, 0, 
                       disc.userfiles, disc.genre, disc.title);
    IssueRequest(disc_name, NO_REPLIES); 

    short count = 0;
    NameList& tracks = disc.tracks;
    for (NameList::iterator iter=tracks.begin(); iter!=tracks.end(); iter++)
    {
        if ( ++count > 20 ) continue;

        Name& track = (*iter);
        m_listener->Progress(this, KenwoodListener::WritingDisc, 
                             track.index, (const char*)track);
        dvd_TextData track_name(m_chain_id, TrackText, track.index, 0, 
                            disc.userfiles, disc.genre, track.text);
        IssueRequest(track_name, NO_REPLIES); 
    }

    const char* art = (disc.artist==NULL) ? "" : disc.artist;
    m_listener->Progress(this, KenwoodListener::WritingDisc, 
                         0, art);
    dvd_TextData artist_name(m_chain_id, ArtistText, slot, 0, 
                         disc.userfiles, disc.genre, disc.artist);
    IssueRequest(artist_name, NO_REPLIES); 
    m_listener->ProgressEnd(this, KenwoodListener::WritingDisc);
}

bool 
DVDChanger::info_changed(short slot, byte title, short chapter)
{
    return ( (slot!=m_cur_slot) || 
             (title!=m_cur_title) || 
             (chapter!=m_cur_chapter) );
}

bool 
DVDChanger::mode_changed(enum mode mode)
{
    return (mode != m_cur_mode);
}

bool 
DVDChanger::state_changed(enum state state)
{
    return (state != m_cur_state);
}

bool 
DVDChanger::program_changed(byte program)
{
    return ( (program!=m_cur_param) && (m_cur_mode==ProgramMode) );
}

bool 
DVDChanger::repeat_changed(enum repeat repeat)
{
    return (repeat != m_cur_repeat);
}

bool 
DVDChanger::param_changed(byte param, enum mode mode)
{
    return ( (param!=m_cur_param) && (mode>=MusicTypeMode) );
}


