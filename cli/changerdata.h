#ifndef JUKEN_CHANGER_DATA_H
#define JUKEN_CHANGER_DATA_H

#include "xmlparser.h"
#include <list>

class Disc;


class ChangerData : public XMLParser
{
    public:
        ChangerData();
        ~ChangerData();

        const char* getModel() { return m_model; }
        const char* getDevice() { return m_device; }
        const char* getUserfileName(int i) { return m_userfiles[i]; }
        char** getUserfileNames() { m_userfiles; }
        list<Disc*> getDiscs() { return m_discs; }

        byte getUserfileByName(const char* name);

    protected:
        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        void CharHandler(const XML_Char* text, int len) { };
        void EndHandler(const XML_Char* element) { };

    private:
        char* m_model;
        char* m_device;
        char* m_userfiles[8];
        list<Disc*> m_discs;
};



class UserfileNameHandler: public ElementHandler
{
    public:
        UserfileNameHandler(char** names) : m_names(names) { };

        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        void CharHandler(const XML_Char* text, int len) { };
        void EndHandler(const XML_Char* element) { };

    private:
        char** m_names;
};

class Disc : public ElementHandler
{
    public:
        Disc(ChangerData* changer);
        virtual ~Disc();

        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        void CharHandler(const XML_Char* text, int len) { };
        void EndHandler(const XML_Char* element);

        short getSlot() { return m_slot; };
        const char* getTitle() { return m_title; };
        const char* getShortTitle() { return m_short_title; };
        const char* getDescription() { return m_description; };

        void print()
        {
            printf("[%3d]  ", m_slot);
            printf("%-25s", m_short_title);
            if ( m_description != NULL )
                printf(": %-25s", m_description);
            else
                printf(": %-25s", " ");
            printf("     (%2X)  ", m_userfiles);
            printf("\n", m_description);
        }

    private:
        ChangerData* m_changer;

        char* m_id;
        short m_slot;
        char* m_title;
        char* m_short_title;
        char* m_description;
        byte m_userfiles;
};

class CD : public Disc
{
    public:
        CD(ChangerData* changer) : Disc(changer) { }
        virtual ~CD() { }
};

class DVD_V : public Disc
{
    public:
        DVD_V(ChangerData* changer) : Disc(changer) { }
        virtual ~DVD_V() { }
};

class MP3 : public Disc
{
    public:
        MP3(ChangerData* changer) : Disc(changer) { }
        virtual ~MP3() { }
};

class UserfilesHandler: public ElementHandler
{
    public:
        UserfilesHandler(ChangerData* changer, byte* userfiles) 
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

#endif /* JUKEN_CHANGER_DATA_H */
