#ifndef JUKEN_CDCHANGER_H
#define JUKEN_CDCHANGER_H

#include "kenwoodchanger.h"

class InfoEvent;
class StateEvent;

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

        void DoListDiscs();
        void DoListContents(const short slot);
        char* GetDiscId(const short slot);
        void DoListBest();
        void DoChangeDisc(const short slot, enum state cur_state);
        void DoPlayPause();
        void DoPrev();
        void DoNext();
        void DoStop();

    private:
        short m_cur_slot;
        byte m_cur_track;
        enum mode m_cur_mode;
        enum repeat m_cur_repeat;
        byte m_cur_param;
        byte m_cur_program;
        enum state m_cur_state;
        enum door m_cur_door_open;

        bool info_changed(const InfoEvent& info);
        bool mode_changed(const InfoEvent& info);
        bool state_changed(const StateEvent& info);
        bool program_changed(const InfoEvent& info);
        bool repeat_changed(const InfoEvent& info);
        bool param_changed(const InfoEvent& info);

        CDChanger();
        CDChanger(const CDChanger& changer);
};

#endif /* JUKEN_CDCHANGER_H */
