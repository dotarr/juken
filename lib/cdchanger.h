#ifndef JUKEN_CDCHANGER_H
#define JUKEN_CDCHANGER_H

#include "kenwoodchanger.h"

class CDChanger : public KenwoodChanger
{
    public:
        CDChanger(char* id, KenwoodDevice& dev);
        virtual ~CDChanger();

        void ProcessEvent();
        void DoInfoEvent(const payload& event);
        void DoStateEvent(const payload& event);
        void DoDiscEvent(const payload& event);
        void DoDoorEvent(const payload& event);

        void DoListDiscs(byte x=0);
        void DoListContents(const short slot, byte x=1);
        uint GetDiscId(const short slot);
        void DoListBest();
        void DoChangeDisc(const short slot, enum state cur_state);
        void DoPlayPause();
        void DoPrev();
        void DoNext();
        void DoStop();

    protected:
        void DoDiscQuery(const DataAccess& query);
        void DoChangeState(const short state);

    private:
        CDChanger();
        CDChanger(const CDChanger& changer);
};

#endif /* JUKEN_CDCHANGER_H */
