#ifndef JUKEN_DVDCHANGER_H
#define JUKEN_DVDCHANGER_H

#include "kenwoodchanger.h"

class DVDChanger : public KenwoodChanger
{
    public:
        DVDChanger(char* id, KenwoodDevice& dev);
        virtual ~DVDChanger();

        void ProcessEvent();
        void DoInfoEvent(const payload& event);
        void DoStateEvent(const payload& event);
        void DoDiscEvent(const payload& event);
        void DoDoorEvent(const payload& event);

        void DoListDiscs();
        void DoListContents(const short slot);
        uint GetDiscId(const short slot);
        void DoListBest();
        void DoChangeDisc(const short slot, enum state cur_state);
        void DoPlayPause();
        void DoPrev();
        void DoNext();
        void DoStop();

    protected:
        void DoDiscQuery(const byte* query);
        void DoChangeState(const short state);

    private:
        DVDChanger();
        DVDChanger(const DVDChanger& changer);
};

#endif /* JUKEN_DVDCHANGER_H */
