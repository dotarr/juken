#include "dvdchanger.h"
#include "dvdpayload.h"
#include "discid.h"
#include "util.h"
#include "dvdconstants.h"

DVDChanger::DVDChanger(char* id, KenwoodDevice& dev, KenwoodListener* listener) 
: KenwoodChanger(id, 403, dev, listener)
{
    m_chain_id = 1;

    // process InfoChanged
    DoEvent();
    // process StateChanged
    DoEvent();

    if ( m_cur_door_pos == DoorClosed )
        ScanDiscs();
    LoadUserfiles();
}

DVDChanger::~DVDChanger()
{
}

void
DVDChanger::DoInfoEvent(const payload& event)
{
    InfoEvent info(event);

    if ( info_changed(info) )
    {
        InfoChanged(info.slot(), info.title(), info.chapter());
    }
    if ( program_changed(info) )
    {
        byte param = 0;
        if ( m_cur_mode > UserfileMode ) param = info.userfile();
        else if ( m_cur_mode > MusicTypeMode ) param = info.genre();
        else if ( m_cur_mode == ProgramMode ) param = info.program();
        ModeChanged(m_cur_mode, m_cur_repeat, param);
    }

    if ( info.toc_complete() )
        fprintf(stderr, "toc read complete\n");
}

void
DVDChanger::DoStateEvent(const payload& event)
{
    StateEvent info(event);

    if ( mode_changed(info) || repeat_changed(info) || param_changed(info) )
    {
        byte param = (info.mode()==ProgramMode ? m_cur_param : info.param());
        ModeChanged(info.mode(), info.repeat(), param);
    }
    if ( info.door_pos() != m_cur_door_pos )
    {
        DoorChanged(info.door_pos());
    }
    if ( state_changed(info) )
    {
        StateChanged(info.state());
    }

    if ( info.at_end() )
        fprintf(stderr, "at track/disc end\n");
    if ( info.library() )
        fprintf(stderr, "library on\n");
}
 
void
DVDChanger::DoQuery(byte a, byte b, byte c, short slot, byte title, short chapter)
{
    // build the payload
    DataAccess query((enum access)a, (enum data_type)b, c, 
                     m_chain_id, slot, title, chapter);

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

NameList
DVDChanger::DoListUserfiles()
{
    NameList names;

    DataAccess query(RetrieveDataAccess, TextDataType, UserfileNames, m_chain_id, 0, 0, 0);

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
DVDChanger::DoListUserfiles(void* context, NameCallback* callback)
{
    DataAccess query(RetrieveDataAccess, TextDataType, UserfileNames, m_chain_id, 0, 0, 0);

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
DVDChanger::DoListDiscs(void* context, DiscCallback* callback)
{
    // build the payload
    DataAccess query(RetrieveDataAccess, TextDataType, DiscNames, m_chain_id, 0, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        TextData info(reply);
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
    // build the payload
    DataAccess query(RetrieveDataAccess, TextDataType, DiscArtistTrackNames, m_chain_id, slot, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    Disc data;
    payload reply;
    while ( GetReply(reply) )
    {
        // process the data
        TextData info(reply);
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
        }
    }

    return data;
}

void
DVDChanger::DoListContents(const short slot, void* context, DiscCallback* callback)
{
    Disc data = DoListContents(slot);
    (*callback)(context, data);
}

Info
DVDChanger::GetDiscInfo(const short slot)
{
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);

    // build the payload
    DataAccess query(RetrieveDataAccess, InfoDataType, 0, m_chain_id, slot, 0, 0);
    
    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    char* id = NULL;

    // get the replies
    payload reply;
    GetOneReply(reply);

    // process the data
    DiscInfo info(reply);
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
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);

    // build the payload
    DataAccess query(RetrieveDataAccess, TOCDataType, TOCId, m_chain_id, slot, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    GetOneReply(reply);
    // process the data
    DiscTOC info(reply);
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
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);

    // build the payload
    DataAccess query(RetrieveDataAccess, TOCDataType, TOCId, m_chain_id, slot, 0, 0);
    
    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    GetOneReply(reply);
    DiscUserfiles info(reply);

    return info.userfiles();
}

enum genre
DVDChanger::GetDiscGenre(const short slot)
{
    // make sure slot is current
    if ( slot != m_cur_slot ) DoChangeDisc(slot);

    // build the payload
    DataAccess query(RetrieveDataAccess, TOCDataType, TOCId, m_chain_id, slot, 0, 0);
    
    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the replies
    payload reply;
    GetOneReply(reply);
    DiscGenre info(reply);

    return info.genre();
}

void
DVDChanger::DoChangeDisc(const short slot)
{
    // build the payload
    ChangeDisc req(m_chain_id, slot, 0, 0, TrackMode, 0, 2);

    // issue the request
    IssueRequest(req, NO_REPLIES); 

    // StateEvent
    DoEvent();
    // InfoEvent
    DoEvent();
    // StateEvent
    DoEvent();
}

void
DVDChanger::DoPlayPause()
{
    // build the payload
    DoAction req(m_chain_id, PLAY_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req, NO_REPLIES); 
}

void
DVDChanger::DoPrev()
{
    // build the payload
    DoAction req1(m_chain_id, PREV_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req1, NO_REPLIES); 
    
    // build the payload
    DoAction req2(m_chain_id, PLAY_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req2, NO_REPLIES); 
}

void
DVDChanger::DoNext()
{
    // build the payload
    DoAction req1(m_chain_id, NEXT_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req1, NO_REPLIES); 
    
    // build the payload
    DoAction req2(m_chain_id, PLAY_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req2, NO_REPLIES); 
}

void
DVDChanger::DoStop()
{
    // build the payload
    DoAction req(m_chain_id, STOP_CMD | STATE_PARAM);

    // issue the request
    IssueRequest(req, NO_REPLIES); 
}

void
DVDChanger::WriteUserfileNames(const char* names[])
{
    DataAccess query(WriteUserfilesAccess, ReadyDataType, AllUserfileNames, m_chain_id, 0, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the reply
    payload reply;
    GetOneReply(reply);

    for (int i=0; i<8; i++)
    {
        TextData data(m_chain_id, 6, 1<<i, 0, 0, 0, names[i]);
        IssueRequest(data, NO_REPLIES); 
    }
}

void
DVDChanger::WriteDisc(short slot, Disc& disc)
{
    const char none[] = { 0x01 };
    DataAccess query(WriteTextAccess, ReadyDataType, DiscArtistNames, m_chain_id, slot, 0, 0);

    // issue the request
    IssueRequest(query, HAS_REPLIES); 

    // get the reply
    payload reply;
    GetOneReply(reply);

    TextData data(m_chain_id, DiscText, slot, 0, disc.userfiles, disc.genre, disc.title);
    IssueRequest(data, NO_REPLIES); 

    NameList& tracks = disc.tracks;
    for (NameList::iterator iter=tracks.begin(); iter!=tracks.end(); iter++)
    {
        const Name& track = (*iter);
        if ( track.text == NULL )
        {
            TextData data(m_chain_id, TrackText, track.index, 0, disc.userfiles, disc.genre, none);
            IssueRequest(data, NO_REPLIES); 
        }
        else
        {
            TextData data(m_chain_id, TrackText, track.index, 0, disc.userfiles, disc.genre, track.text);
            IssueRequest(data, NO_REPLIES); 
        }
    }

    if ( disc.artist == NULL )
    {
        TextData data(m_chain_id, ArtistText, slot, 0, disc.userfiles, disc.genre, none);
        IssueRequest(data, NO_REPLIES); 
    }
    else
    {
        TextData data(m_chain_id, ArtistText, slot, 0, disc.userfiles, disc.genre, disc.artist);
        IssueRequest(data, NO_REPLIES); 
    }
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
    return ( (info.program()!=m_cur_param) &&
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


