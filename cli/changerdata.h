#ifndef JUKEN_CHANGER_DATA_H
#define JUKEN_CHANGER_DATA_H

#include "xmlparser.h"
#include <list>

class DiscElement;


class ChangerData : public XMLParser
{
    public:
        ChangerData();
        ~ChangerData();

        const char* getModel() { return m_model; }
        const char* getDevice() { return m_device; }
        const char* getUserfileName(int i) { return m_userfiles[i]; }
        char** getUserfileNames() { m_userfiles; }
        list<DiscElement*> getDiscs() { return m_discs; }

        byte getUserfileByName(const char* name);

    protected:
        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        void CharHandler(const XML_Char* text, int len) { };
        void EndHandler(const XML_Char* element) { };

    private:
        char* m_model;
        char* m_device;
        char* m_userfiles[8];
        list<DiscElement*> m_discs;
};



class UserfileNameElement: public ElementHandler
{
    public:
        UserfileNameElement(char** names) : m_names(names) { };

        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        void CharHandler(const XML_Char* text, int len) { };
        void EndHandler(const XML_Char* element) { };

    private:
        char** m_names;
};

class DiscElement : public ElementHandler
{
    public:
        DiscElement(ChangerData* changer);
        virtual ~DiscElement();

        virtual ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        virtual void CharHandler(const XML_Char* text, int len) { };
        virtual void EndHandler(const XML_Char* element);

        enum disc_type { cd=0, mp3, vcd, dvd, dvda };
        virtual enum disc_type getType() = 0;

        const char* getId() { return m_id; };
        short getSlot() { return m_slot; };
        const char* getTitle() { return m_title; };
        const char* getShortTitle() { return m_short_title; };
        const char* getDescription() { return m_description; };
        const char* getArtist() { return m_artist; };
        list<char*>& getTracks() { return m_tracks; };
        const byte getUserfiles() { return m_userfiles; }
        const byte getGenre() { return m_genre; }

        void print(FILE* file)
        {
            ::fprintf(file, "[%3d]  ", m_slot);
            ::fprintf(file, "%-25s\n", m_short_title);
            for (list<char*>::iterator iter=m_tracks.begin(); iter!=m_tracks.end(); iter++)
                ::fprintf(file, "  %-25s\n", *iter);
        }

    protected:
        ChangerData* m_changer;

        char* m_id;
        short m_slot;
        char* m_title;
        char* m_short_title;
        char* m_description;
        char* m_artist;
        byte m_userfiles;
        byte m_genre;
        list<char*> m_tracks;
};

class CDElement : public DiscElement
{
    public:
        CDElement(ChangerData* changer) : DiscElement(changer) { }
        virtual ~CDElement() { }
        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        enum disc_type getType() { return cd; };
};

class MP3Element : public DiscElement
{
    public:
        MP3Element(ChangerData* changer) : DiscElement(changer) { }
        virtual ~MP3Element() { }
        enum disc_type getType() { return mp3; };
};

class VCDElement : public DiscElement
{
    public:
        VCDElement(ChangerData* changer) : DiscElement(changer) { }
        virtual ~VCDElement() { }
        enum disc_type getType() { return vcd; };
};

class DVDElement : public DiscElement
{
    public:
        DVDElement(ChangerData* changer) : DiscElement(changer) { }
        virtual ~DVDElement() { }
        enum disc_type getType() { return dvd; };
};

class DVDAElement : public DiscElement
{
    public:
        DVDAElement(ChangerData* changer) : DiscElement(changer) { }
        virtual ~DVDAElement() { }
        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        enum disc_type getType() { return dvda; };
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
        TracksElement(list<char*>* tracks) 
            : m_tracks(tracks) { }

        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes);
        void CharHandler(const XML_Char* text, int len) { };
        void EndHandler(const XML_Char* element) { };

    private:
        list<char*>* m_tracks;
};

#endif /* JUKEN_CHANGER_DATA_H */
