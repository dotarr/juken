#ifndef JUKEN_JUKEN_H
#define JUKEN_JUKEN_H

#include <kenwooddevice.h>
#include <kenwoodchanger.h>
#include <kenwoodlistener.h>

class Juken : public KenwoodListener
{
    public:
        Juken(const char* serial_device);
        ~Juken();

        void InitState();

        void Run();

        bool InfoChanged(short slot, byte title, short chapter);
        bool ModeChanged(enum mode mode, bool repeat, byte param);
        bool StateChanged(enum state state);
        bool DoorChanged(bool door_open);
        
        bool TextDataReply(short slot, byte track, byte userfiles, 
                           byte request_type, byte genre, 
                           byte formatting, char* title);

    protected:
        typedef void (*cmd_func)(Juken* _this, int argc, char* argv[]);
        typedef struct cmd { char* name; cmd_func func; char* help; };

        static struct cmd m_short_commands[];
        static struct cmd m_commands[];

        void DoCommand(char* line);

        static void DoHelp(Juken* _this, int argc, char* argv[]);
        static void DoList(Juken* _this, int argc, char* argv[]);
        static void GetBests(Juken* _this, int argc, char* argv[]);
        static void DoChangeDisc(Juken* _this, int argc, char* argv[]);
        static void DoPlay(Juken* _this, int argc, char* argv[]);
        static void DoPrev(Juken* _this, int argc, char* argv[]);
        static void DoNext(Juken* _this, int argc, char* argv[]);
        static void DoStop(Juken* _this, int argc, char* argv[]);
        static void DoId(Juken* _this, int argc, char* argv[]);
        static void DoQuery(Juken* _this, int argc, char* argv[]);
        static void DoQuit(Juken* _this, int argc, char* argv[]);


    private:
        static Juken* g_juken; // a ref to 'this' for callbacks

        KenwoodDevice* m_device;
        KenwoodChanger* m_changer;

        bool m_done;

        enum door m_door_state;

        FILE* m_file;

        short       m_cur_slot;
        short       m_capacity;
        char**      m_titles;

        bool m_loading_titles;

        static void readline_callback(char* line);
        static char** completion_callback(const char* text, int start, int end);

};

#endif /* JUKEN_JUKEN_H */
