#include "kenwoodchanger.h"
#include "util.h"

KenwoodChanger::KenwoodChanger(char* id, KenwoodDevice& dev) 
: m_device(dev)
{
}

KenwoodChanger::~KenwoodChanger()
{
}

void
KenwoodChanger::pushListener(KenwoodListener* listener)
{
    m_listeners.push_front(listener);
}

void
KenwoodChanger::popListener()
{
    m_listeners.pop_front();
}

void
KenwoodChanger::DoEvent()
{
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
    
    ProcessEvent();
}

void
KenwoodChanger::IssueRequest(const payload& msg, const bool has_replies)
{
    while ( !m_device.ClearToSend() )
    {
        //m_device.WriteCntl(ACK);
        //ProcessEvent();
    }
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

bool
KenwoodChanger::GetEvent(payload& event)
{
    return m_device.RecvMessage(event);
}

