#include "kenwoodchanger.h"

KenwoodChanger::KenwoodChanger(char* id, short capacity, 
                               KenwoodDevice& dev, KenwoodListener* listener) 
: m_identifier(id), m_capacity(capacity), m_slots(NULL), m_device(dev), m_listener(listener)
{
    m_cur_slot = -1;
    m_cur_title = (byte) -1;
    m_cur_chapter = -1;
    m_cur_mode = UnknownMode;
    m_cur_repeat = UnknownRepeat;
    m_cur_param = (byte) -1;
    m_cur_state = UnknownState;
    m_cur_door_pos = DoorUnknown;

    for (int i=0; i<8; i++)
        m_userfiles[i] = NULL;
}

KenwoodChanger::~KenwoodChanger()
{
    if ( m_slots != NULL )
    {
        for (int i=0; i<m_capacity; i++)
            delete m_slots[i];
        delete[] m_slots;
    }
    m_slots = NULL;

    for (int i=0; i<8; i++)
        delete m_userfiles[i];
}

void
KenwoodChanger::DoEvent()
{
    m_device.setEventPending(false);

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
    GetOneReply(event);
    switch ( event.cmd )
    {
        case INFO_EVENT:  DoInfoEvent(event);  break;
        case STATE_EVENT: DoStateEvent(event); break;
        case DISC_EVENT:  DoDiscEvent(event);  break;
        case DOOR_EVENT:  DoDoorEvent(event);  break;

        default:
            DebugPayload("unhandled event", event.cmd, event.len, event.data);
            break;
    }
}

void
KenwoodChanger::DoAnyEvents()
{
    while ( m_device.getEventPending() )
        DoEvent();
}

void
KenwoodChanger::InfoChanged(short slot, byte title, short chapter)
{
    InfoMsg("KenwoodChanger::InfoChanged(%d, %d, %d)\n", slot, title, chapter);
    m_cur_slot = slot;
    m_cur_title = title;
    m_cur_chapter = chapter;
    // notify listener
    m_listener->InfoChanged(this, slot, title, chapter);
}

void
KenwoodChanger::ModeChanged(enum mode mode, enum repeat repeat, byte param)
{
    InfoMsg("KenwoodChanger::ModeChanged(%d, %d, %d)\n", (byte)mode, (byte)repeat, param);
    m_cur_mode = mode;
    m_cur_repeat = repeat;
    m_cur_param = param;
    // notify listener
    m_listener->ModeChanged(this, mode, repeat==RepeatOn, param);
}

void
KenwoodChanger::StateChanged(enum state state)
{
    InfoMsg("KenwoodChanger::StateChanged(%d)\n", (byte)state);
    m_cur_state = state;
    // notify listener
    m_listener->StateChanged(this, state);
}

void
KenwoodChanger::DoorChanged(enum door door_pos)
{
    InfoMsg("KenwoodChanger::DoorChanged(%d)\n", (byte)door_pos);
    bool rescan = (m_cur_door_pos==DoorOpen && door_pos==DoorClosed);

    m_cur_door_pos = door_pos;
    // notify listener
    m_listener->DoorChanged(this, door_pos==DoorOpen);

    if ( rescan )
        ScanDiscs();
}

void
KenwoodChanger::ScanDiscs()
{
    InfoMsg("KenwoodChanger::ScanDiscs()\n");
    m_listener->ProgressStart(this, KenwoodListener::ScanDiscs, m_capacity);
    if ( m_slots == NULL )
    {
        m_slots = new Disc*[m_capacity];
        for (int i=0; i<m_capacity; i++)
        {
            m_slots[i] = NULL;
        }
    }
    else
    {
        for (int i=0; i<m_capacity; i++)
        {
            delete m_slots[i];
            m_slots[i] = NULL;
        }
    }
    DoListDiscs(this, &ScanDiscsCallback);
    m_listener->ProgressEnd(this, KenwoodListener::ScanDiscs);
}

void
KenwoodChanger::LoadUserfiles()
{
    InfoMsg("KenwoodChanger::LoadUserfiles()\n");
    m_listener->ProgressStart(this, KenwoodListener::LoadUserfiles, m_capacity);
    for (int i=0; i<8; i++)
    {
        delete m_userfiles[i];
        m_userfiles[i] = NULL;
    }
    DoListUserfiles(this, &LoadUserfilesCallback);
    m_listener->ProgressEnd(this, KenwoodListener::LoadUserfiles);
}

void
KenwoodChanger::IssueRequest(const payload& msg, const bool has_replies)
{
    DoAnyEvents();

    m_device.SendMessage(msg, has_replies);

    if ( !has_replies )
        m_device.EndMessage();
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

void 
KenwoodChanger::ScanDiscsCallback(void* context, Disc& data)
{
    KenwoodChanger* _this = (KenwoodChanger*) context;

    _this->m_slots[data.index-1] = new Disc(data);
    _this->m_listener->Progress(_this, KenwoodListener::ScanDiscs, data.index);
}

void 
KenwoodChanger::LoadUserfilesCallback(void* context, Name& data)
{
    KenwoodChanger* _this = (KenwoodChanger*) context;

    byte uf = data.index;
    int i = 0;
    while ( uf != 1 ) { uf = uf>>1; i++; }
    _this->m_userfiles[i] = ::strdup(data.text);
    _this->m_listener->Progress(_this, KenwoodListener::LoadUserfiles, i+1);
}
