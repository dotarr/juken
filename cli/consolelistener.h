#ifndef JUKEN_CONSOLE_LISTENER_H
#define JUKEN_CONSOLE_LISTENER_H

#include <kenwoodlistener.h>

class ConsoleListener : public KenwoodListener
{
    public:
        ConsoleListener(FILE* file) : m_file(file), m_last_prog_len(0)  { }
        ~ConsoleListener() { }

        void InfoChanged(KenwoodChanger* changer, short slot, byte title, short chapter);
        void ModeChanged(KenwoodChanger* changer, enum mode mode, bool repeat, byte param);
        void StateChanged(KenwoodChanger* changer, enum state state);
        void DoorChanged(KenwoodChanger* changer, bool door_open);

        void ProgressStart(KenwoodChanger* changer, enum operation op, int length);
        void Progress(KenwoodChanger* changer, 
                      enum operation op, 
                      int progress,
                      const char* str);
        void ProgressEnd(KenwoodChanger* changer, enum operation op);

    protected:
        static const char* scanning_discs_str;
        static const char* loading_userfiles_str;
        static const char* changing_disc_str;
        static const char* reading_disc_str;
        static const char* writing_userfiles_str;
        static const char* writing_disc_str;

        FILE* m_file;

        int m_last_prog_len;

        void back_space(int i);
};

#endif /* JUKEN_CONSOLE_LISTENER_H */
