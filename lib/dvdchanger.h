#ifndef JUKEN_DVDCHANGER_H
#define JUKEN_DVDCHANGER_H

#include "kenwoodchanger.h"

class InfoEvent;
class StateEvent;

class DVDChanger : public KenwoodChanger
{
    public:
        DVDChanger(char* id, KenwoodDevice& dev);
        virtual ~DVDChanger();

        void ProcessEvent();
        void DoInfoEvent(const payload& event);
        void DoStateEvent(const payload& event);

        void DoQuery(byte a, byte b, byte c,
                     short slot, byte title, short chapter);
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
        bool m_setup;

        short m_cur_slot;
        byte m_cur_title;
        short m_cur_chapter;
        enum mode m_cur_mode;
        enum repeat m_cur_repeat;
        byte m_cur_param;
        byte m_cur_program;
        enum state m_cur_state;
        enum door m_cur_door_open;

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
