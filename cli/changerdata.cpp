#include <common.h>

#include "changerdata.h"

ChangerData::ChangerData()
    : m_model(NULL), m_device(NULL)
{
    for (int i=0; i<8; i++) 
        m_userfiles[i] = NULL;
}

ChangerData::~ChangerData()
{
    delete m_model;
    delete m_device;
    for (int i=0; i<8; i++) 
        delete m_userfiles[i];

    while ( !m_discs.empty() )
    {
        DiscElement* disc = m_discs.back();
        m_discs.pop_back();
        delete disc;
    }
}

byte 
ChangerData::getUserfileByName(const char* name)
{
    for (int i=0; i<8; i++)
    {
        if ( ::strcmp(name, m_userfiles[i]) == 0 )
            return 1<<i;
    }
    printf("userfile %s not found\n", name);
    return 0;
}

ElementHandler*
ChangerData::StartHandler(const XML_Char* element, const XML_Char** attributes)
{
    if ( ::strcmp(element, "Changer") == 0 )
        return this;
    else if ( ::strcmp(element, "Model") == 0 )
        return new StringHandler(&m_model);
    else if ( ::strcmp(element, "Device") == 0 )
        return new StringHandler(&m_device);
    else if ( ::strcmp(element, "Userfiles") == 0 )
        return new UserfileNameElement(m_userfiles);
    else 
    {
        DiscElement* disc = NULL;
        if ( ::strcmp(element, "CD") == 0 )
            disc = new CDElement(this);
        else if ( ::strcmp(element, "MP3") == 0 )
            disc = new MP3Element(this);
        else if ( ::strcmp(element, "VCD") == 0 )
            disc = new VCDElement(this);
        else if ( ::strcmp(element, "DVDA") == 0 )
            disc = new DVDAElement(this);
        else if ( ::strcmp(element, "DVD") == 0 )
            disc = new DVDElement(this);
        if ( disc != NULL )
            m_discs.push_back(disc);
        return disc;
    }
}

ElementHandler* 
UserfileNameElement::StartHandler(const XML_Char* element, const XML_Char** attrbutes)
{
    if ( ::strncmp(element, "Userfile", ::strlen("Userfile")) == 0 )
    {
        int i = element[::strlen("Userfile")] - '0';
        if ( i>=1 && i<=8 )
            return new StringHandler(&m_names[i-1]);
    }

    return NULL;
}

DiscElement::DiscElement(ChangerData* changer)
    : m_changer(changer), m_id(NULL), m_title(NULL), 
      m_short_title(NULL), m_description(NULL), m_artist(NULL)
{
}

DiscElement::~DiscElement()
{
    delete m_id;
    delete m_title;
    delete m_short_title;
    delete m_description;
    delete m_artist;
}

ElementHandler* 
DiscElement::StartHandler(const XML_Char* element, const XML_Char** attrbutes)
{
    if ( ::strcmp(element, "ID") == 0 )
        return new StringHandler(&m_id);
    else if ( ::strcmp(element, "Slot") == 0 )
        return new ShortHandler(&m_slot);
    else if ( ::strcmp(element, "Title") == 0 )
        return new StringHandler(&m_title);
    else if ( ::strcmp(element, "ShortTitle") == 0 )
        return new StringHandler(&m_short_title);
    else if ( ::strcmp(element, "Description") == 0 )
        return new StringHandler(&m_description);
    else if ( ::strcmp(element, "Artist") == 0 )
        return new StringHandler(&m_artist);
    else if ( ::strcmp(element, "Userfiles") == 0 )
        return new UserfilesElement(m_changer, &m_userfiles);
    else
    {
        printf("unrecognized tag: %s\n", element);
        return NULL;
    }
}

void 
DiscElement::EndHandler(const XML_Char* element)
{ 
    //print(); 
}

ElementHandler* 
CDElement::StartHandler(const XML_Char* element, const XML_Char** attrbutes)
{
    if ( ::strcmp(element, "Tracks") == 0 )
        return new TracksElement(&m_tracks);
    else
        return DiscElement::StartHandler(element, attrbutes);
}

ElementHandler*
UserfilesElement::StartHandler(const XML_Char* element, const XML_Char** attributes)
{
    return new StringHandler(&m_names[m_count++]);
}

void 
UserfilesElement::EndHandler(const XML_Char* element)
{
    for (int i=0; i<m_count; i++)
    {
        (*m_userfiles) |= m_changer->getUserfileByName(m_names[i]);
        delete m_names[i];
    }
}

ElementHandler*
TracksElement::StartHandler(const XML_Char* element, const XML_Char** attributes)
{
    m_tracks->push_back(NULL);
    return new StringHandler(&m_tracks->back());
}


