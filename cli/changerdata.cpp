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
        Disc* disc = m_discs.back();
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
        return new UserfileNameHandler(m_userfiles);
    else 
    {
        Disc* disc = NULL;
        if ( ::strcmp(element, "CD") == 0 )
            disc = new CD(this);
        else if ( ::strcmp(element, "DVD") == 0 )
            disc = new DVD_V(this);
        else if ( ::strcmp(element, "MP3") == 0 )
            disc = new MP3(this);
        if ( disc != NULL )
            m_discs.push_back(disc);
        return disc;
    }
}

ElementHandler* 
UserfileNameHandler::StartHandler(const XML_Char* element, const XML_Char** attrbutes)
{
    if ( ::strncmp(element, "Userfile", ::strlen("Userfile")) == 0 )
    {
        int i = element[::strlen("Userfile")] - '0';
        if ( i>=1 && i<=8 )
            return new StringHandler(&m_names[i-1]);
    }

    return NULL;
}

Disc::Disc(ChangerData* changer)
    : m_changer(changer), m_id(NULL), m_title(NULL), m_short_title(NULL), m_description(NULL)
{
}

Disc::~Disc()
{
    delete m_id;
    delete m_title;
    delete m_short_title;
    delete m_description;
}

ElementHandler* 
Disc::StartHandler(const XML_Char* element, const XML_Char** attrbutes)
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
    else if ( ::strcmp(element, "Userfiles") == 0 )
        return new UserfilesHandler(m_changer, &m_userfiles);
    else
        return NULL;
}

void 
Disc::EndHandler(const XML_Char* element)
{ 
    //print(); 
}

ElementHandler*
UserfilesHandler::StartHandler(const XML_Char* element, const XML_Char** attributes)
{
    return new StringHandler(&m_names[m_count++]);
}

void 
UserfilesHandler::EndHandler(const XML_Char* element)
{
    for (int i=0; i<m_count; i++)
    {
        (*m_userfiles) |= m_changer->getUserfileByName(m_names[i]);
        delete m_names[i];
    }
}

