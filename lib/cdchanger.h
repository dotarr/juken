#ifndef JUKEN_CDCHANGER_H
#define JUKEN_CDCHANGER_H

#include "kenwoodchanger.h"

class CDChanger : public KenwoodChanger
{
    public:
        CDChanger(char* id, KenwoodDevice& dev, KenwoodListener* listener);
        virtual ~CDChanger();

        void DoInfoEvent(const payload& event);
        void DoStateEvent(const payload& event);
        void DoDiscEvent(const payload& event);
        void DoDoorEvent(const payload& event);

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
        bool info_changed(short slot, byte track);
        bool mode_changed(enum mode mode);
        bool state_changed(enum state state);
        bool program_changed(byte program, enum mode mode);
        bool repeat_changed(enum repeat repeat);
        bool param_changed(byte param, enum mode mode);

        CDChanger();
        CDChanger(const CDChanger& changer);
};

#endif /* JUKEN_CDCHANGER_H */
