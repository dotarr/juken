#include "dvdchanger.h"
#include "discid.h"
#include "util.h"

DVDChanger::DVDChanger(char* id, KenwoodDevice& dev) 
: KenwoodChanger(id, dev)
{
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
            case DISC_EVENT:  DoDiscEvent(event);  break;
            case DOOR_EVENT:  DoDoorEvent(event);  break;

            default:
                DebugPayload("unhandled event", event, m_device.ComputeChecksum(event));
                break;
        }
    }
}

void
DVDChanger::DoInfoEvent(const payload& event)
{
    printf("InfoEvent:\n");
    printdata(event.data, event.len);
}

void
DVDChanger::DoStateEvent(const payload& event)
{
    printf("StateEvent:\n");
    printdata(event.data, event.len);
}
 
void
DVDChanger::DoDiscEvent(const payload& event)
{
    printf("DiscEvent:\n");
    printdata(event.data, event.len);
}
 
void
DVDChanger::DoDoorEvent(const payload& event)
{
    printf("DoorEvent:\n");
    printdata(event.data, event.len);
}

void
DVDChanger::DoListDiscs(byte x)
{
}

void
DVDChanger::DoListContents(const short slot, byte x)
{
}

uint
DVDChanger::GetDiscId(const short slot)
{
    return 0;
}

void
DVDChanger::DoListBest()
{
}

void
DVDChanger::DoChangeDisc(const short slot, enum state cur_state)
{
}

void
DVDChanger::DoPlayPause()
{
    DoChangeState(PLAY_PAUSE_CMD | STATE_PARAM);
}

void
DVDChanger::DoPrev()
{
    DoChangeState(PREV_CMD | STATE_PARAM);
    DoChangeState(NULL_PARAM);
}

void
DVDChanger::DoNext()
{
    DoChangeState(NEXT_CMD | STATE_PARAM);
    DoChangeState(NULL_PARAM);
}

void
DVDChanger::DoStop()
{
    DoChangeState(STOP_CMD | STATE_PARAM);
}

void
DVDChanger::DoDiscQuery(const DataAccess& query)
{
}

void
DVDChanger::DoChangeState(const short state)
{
    // build the payload
    payload req;
    req.cmd = DO_ACTION;
    req.len = sizeof(DoAction);
    ::memcpy(req.data, &state, req.len);

    // issue the request
    m_device.SendMessage(req, NO_REPLIES); 
}


