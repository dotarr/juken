#include "dvdchanger.h"
#include "dvdpayload.h"
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
    byte changer = event.data[0];
    InfoEvent* info = (InfoEvent*) &event.data[1];

    printf("InfoEvent(%d): ", changer);
    printf("D%03d T%02d C%02d\n", info->slot, info->title, info->chapter);
printf("hex\t%02X ", info->unknown_1);
printf("%02X ", info->unknown_2);
printf("%02X ", info->unknown_3);
printf("%02X ", info->unknown_4);
printf("%02X\n", info->unknown_5);
printf("decimal\t%02d ", info->unknown_1);
printf("%02d ", info->unknown_2);
printf("%02d ", info->unknown_3);
printf("%02d ", info->unknown_4);
printf("%02d\n", info->unknown_5);
}

void
DVDChanger::DoStateEvent(const payload& event)
{
    Foo bar(event.data);

    byte changer = event.data[0];
    StateEvent* info = (StateEvent*) &event.data[1];

    printf("StateEvent(%d): ", changer);
    enum state state;
    switch ( info->state )
    {
        case STOPPED_STATE:  state = Stopped;      break;
        case STANDBY_STATE:  state = Standby;      break;
        case STOPPING_STATE: state = Stopping;     break;
        case CHANGING_STATE: state = Changing;     break;
        case PLAYING_STATE:  state = Playing;      break;
        case PAUSED_STATE:   state = Paused;       break;
        case SKIPFORW_STATE: state = SkipForward;  break;
        case SKIPBACK_STATE: state = SkipBackward; break;
        default:             state = Unknown;      break;
    }
if ( state == Unknown )
    printf("%s(%02X)\n", "unknown", info->state);
else
    printf("%s\n", STATE_NAMES[state]);
printf("hex\t%02X ", info->unknown_1);
printf("%02X ", info->unknown_2);
printf("%02X ", info->unknown_3);
printf("%02X ", info->unknown_4);
printf("%02X ", info->unknown_5);
printf("%02X\n", info->unknown_6);
printf("decimal\t%02d ", info->unknown_1);
printf("%02d ", info->unknown_2);
printf("%02d ", info->unknown_3);
printf("%02d ", info->unknown_4);
printf("%02d ", info->unknown_5);
printf("%02d\n", info->unknown_6);
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
DVDChanger::DoListDiscs()
{
}

void
DVDChanger::DoListContents(const short slot)
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
DVDChanger::DoDiscQuery(const byte* query)
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


