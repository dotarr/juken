#ifndef JUKEN_DVDCHANGER_H
#define JUKEN_DVDCHANGER_H

#include "kenwoodchanger.h"

class InfoEvent;
class StateEvent;

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

        void WriteUserfileNames(const char* names[]);
        void WriteDisc(short slot, Disc& disc);

    private:
        byte m_chain_id;

        bool info_changed(const InfoEvent& info);
        bool mode_changed(const StateEvent& info);
        bool state_changed(const StateEvent& info);
        bool program_changed(const InfoEvent& info);
        bool repeat_changed(const StateEvent& info);
        bool param_changed(const StateEvent& info);

        DVDChanger();
        DVDChanger(const DVDChanger& changer);
};

#endif /* JUKEN_DVDCHANGER_H */
