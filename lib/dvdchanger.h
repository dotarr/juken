#ifndef JUKEN_DVDCHANGER_H
#define JUKEN_DVDCHANGER_H

#include "kenwoodchanger.h"

class DVDChanger : public KenwoodChanger
{
    public:
        DVDChanger(char* id, KenwoodDevice& dev, KenwoodListener* listener);
        virtual ~DVDChanger();

        void DoInfoEvent(const payload& event);
        void DoStateEvent(const payload& event);

        void DoQuery(byte a, byte b, byte c, short slot, byte title, short chapter);

        NameList DoListUserfiles();
        void DoListUserfiles(void* context, NameCallback* callback);

        void DoListDiscs(void* context, DiscCallback* callback);

        Disc DoListContents(const short slot);
        void DoListContents(const short slot, void* context, DiscCallback* callback);

        Info GetDiscInfo(const short slot);
        char* GetDiscId(const short slot);
        byte GetDiscUserfiles(const short slot);
        enum genre GetDiscGenre(const short slot);
        
        void DoChangeDisc(const short slot);

        void DoPlayPause();
        void DoPrev();
        void DoNext();
        void DoStop();

        void WriteUserfileNames(NameList& names);
        void WriteDisc(short slot, Disc& disc);

    private:
        byte m_chain_id;

        bool info_changed(short slot, byte title, short chapter);
        bool mode_changed(enum mode mode);
        bool state_changed(enum state state);
        bool program_changed(byte program);
        bool repeat_changed(enum repeat repeat);
        bool param_changed(byte param, enum mode mode);

        DVDChanger();
        DVDChanger(const DVDChanger& changer);
};

#endif /* JUKEN_DVDCHANGER_H */
