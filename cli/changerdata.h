#ifndef JUKEN_CHANGER_DATA_H
#define JUKEN_CHANGER_DATA_H

#include "xmlparser.h"
#include "types.h"
#include <list>

class DiscElement;


class ChangerData : public XMLParser
{
    public:
        ChangerData();
        ~ChangerData();

        const char* getModel() { return m_model; }
        const char* getDevice() { return m_device; }
        const char* getUserfileName(int i) 
            {
                NameList::iterator iter = m_userfiles.begin();
                while ( i > 0 ) { iter++; i--; }
                return (*iter).text;
            }
        NameList& getUserfileNames() { return m_userfiles; }
        DiscList& getDiscs() { return m_discs; }

        byte getUserfileByName(const char* name);

    protected:
        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        void CharHandler(const XML_Char* text, int len) { };
        void EndHandler(const XML_Char* element) { };

    private:
        char* m_model;
        char* m_device;
        NameList m_userfiles;
        DiscList m_discs;
};



class UserfileNameElement: public ElementHandler
{
    public:
        UserfileNameElement(NameList* names) : m_name_list(names)
        { for (int i=0; i<8; i++) m_names[i] = NULL; };

        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        void CharHandler(const XML_Char* text, int len) { };
        void EndHandler(const XML_Char* element);

    private:
        NameList* m_name_list;
        char* m_names[8];
};

class DiscElement : public ElementHandler
{
    public:
        DiscElement(ChangerData* changer, Disc* disc);
        virtual ~DiscElement() { };

        virtual ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        virtual void CharHandler(const XML_Char* text, int len) { };
        virtual void EndHandler(const XML_Char* element) { };

    protected:
        ChangerData* m_changer;
        Disc* m_disc;
        char* m_id;
};

class UserfilesElement: public ElementHandler
{
    public:
        UserfilesElement(ChangerData* changer, byte* userfiles) 
            : m_changer(changer), m_count(0), m_userfiles(userfiles)
            { for (int i=0; i<8; i++) m_names[i] = NULL; };

        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        void CharHandler(const XML_Char* text, int len) { };
        void EndHandler(const XML_Char* element);

    private:
        ChangerData* m_changer;
        int m_count;
        char* m_names[8];
        byte* m_userfiles;
};

class GenreElement: public StringHandler
{
    public:
        GenreElement(byte* genre)
            : StringHandler(&m_name), m_name(NULL), m_genre(genre) { }
        void EndHandler(const XML_Char* element);

    private:
        char* m_name;
        byte* m_genre;
};


class TracksElement: public ElementHandler
{
    public:
        TracksElement(NameList* tracks) 
            : m_tracks(tracks), m_index(1) { }

        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        void CharHandler(const XML_Char* text, int len) { };
        void EndHandler(const XML_Char* element) { };

    private:
        NameList* m_tracks;
        short m_index;
};

#endif /* JUKEN_CHANGER_DATA_H */
