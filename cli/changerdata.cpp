#include <common.h>
#include <types.h>
#include <constants.h>

#include "changerdata.h"

ChangerData::ChangerData()
    : m_model(NULL), m_device(NULL), m_userfiles()
{
}

ChangerData::~ChangerData()
{
    delete m_model;
    delete m_device;
}

byte 
ChangerData::getUserfileByName(const char* name)
{
    for (NameList::iterator iter=m_userfiles.begin(); iter!=m_userfiles.end(); iter++)
    {
        Name& userfile_name = (*iter);
        if ( ::strcmp(name, userfile_name.text) == 0 )
            return 1<<(userfile_name.index);
    }
    LogMsg("userfile %s not found\n", name);
    return 0;
}

ElementHandler*
ChangerData::StartHandler(const XML_Char* element, const XML_Char** attributes)
{
    DebugMsg("ChangerData::StartHandler(%s)\n", element);
    if ( ::strcmp(element, "Changer") == 0 )
        return this;
    else if ( ::strcmp(element, "Model") == 0 )
        return new StringHandler(&m_model);
    else if ( ::strcmp(element, "Device") == 0 )
        return new StringHandler(&m_device);
    else if ( ::strcmp(element, "Userfiles") == 0 )
        return new UserfileNameElement(&m_userfiles);
    else 
    {
        byte type = 255;
        if ( ::strcmp(element, "CD") == 0 )
            type = DISC_CD_A;
        else if ( ::strcmp(element, "MP3") == 0 )
            type = DISC_CD_MP3;
        else if ( ::strcmp(element, "VCD") == 0 )
            type = DISC_CD_V;
        else if ( ::strcmp(element, "DVDA") == 0 )
            type = DISC_DVD_A;
        else if ( ::strcmp(element, "DVD") == 0 )
            type = DISC_DVD_V;
        m_discs.push_back(Disc(type));
        return new DiscElement(this, &(m_discs.back()));
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

void
UserfileNameElement::EndHandler(const XML_Char* element)
{
    for (int i=0; i<8; i++)
    {
        Name name(i, USERFILE_NAME, m_names[i]);
        m_name_list->push_back(name);
    }
}
 
DiscElement::DiscElement(ChangerData* changer, Disc* disc)
    : m_changer(changer), m_disc(disc), m_id(NULL)
{
}

ElementHandler* 
DiscElement::StartHandler(const XML_Char* element, const XML_Char** attrbutes)
{
    DebugMsg("DiscElement::StartHandler(%s)\n", element);
    if ( ::strcmp(element, "ID") == 0 )
        return NULL; //new StringHandler(&(m_disc->id));
    else if ( ::strcmp(element, "Slot") == 0 )
        return new ShortHandler(&(m_disc->index));
    else if ( ::strcmp(element, "Title") == 0 )
        return NULL; //new StringHandler(&(m_disc->title));
    else if ( ::strcmp(element, "ShortTitle") == 0 )
        return new StringHandler(&(m_disc->title));
    else if ( ::strcmp(element, "Description") == 0 )
        return new StringHandler(&(m_disc->artist));
    else if ( ::strcmp(element, "Artist") == 0 )
        return new StringHandler(&(m_disc->artist));
    else if ( ::strcmp(element, "Userfiles") == 0 )
        return new UserfilesElement(m_changer, &(m_disc->userfiles));
    else if ( ::strcmp(element, "Genre") == 0 )
        return new GenreElement(&(m_disc->genre));
    else if ( ::strcmp(element, "Tracks") == 0 )
        return new TracksElement(&(m_disc->tracks));
    else
    {
        LogMsg("unrecognized tag: %s\n", element);
        return NULL;
    }
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

void 
GenreElement::EndHandler(const XML_Char* element)
{
    if ( m_name == NULL ) return;

    for (byte genre=ADULT_CONTEMPORARY; genre<=WORLD_MUSIC ;genre++)
    {
        if ( ::strcmp(m_name, GENRE_NAMES[genre]) == 0 )
        {
            *m_genre = genre;
            return;
        }
    }
}

ElementHandler*
TracksElement::StartHandler(const XML_Char* element, const XML_Char** attributes)
{
    if ( ::strcmp(element, "Track") == 0 )
    {
        m_tracks->push_back(Name(m_index, TRACK_NAME, NULL));
        m_index++;
        return new StringHandler(&(m_tracks->back().text));
    }
    else
        return NULL;
}


