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

        bool InfoChanged(short slot, byte track, enum mode mode, 
                         enum random random, bool repeat, 
                         byte userfile);
        bool StateChanged(enum state state);
        bool DiscChanged(short slot);
        bool DoorChanged(bool door_closed);
        
        bool DiscDataReply(DiscData* info);
        bool CDTextDataReply(CDTextData* info);
        bool DiscTrackListReply(DiscTrackList* info);

    protected:
        typedef void (*cmd_func)(Juken* _this, int argc, char* argv[]);
        typedef struct cmd { char* name; cmd_func func; char* help; };

        static struct cmd m_short_commands[];
        static struct cmd m_commands[];

        void DoCommand(char* line);

        static void DoHelp(Juken* _this, int argc, char* argv[]);
        static void DoExport(Juken* _this, int argc, char* argv[]);
        static void DoList(Juken* _this, int argc, char* argv[]);
        static void DoExperiment(Juken* _this, int argc, char* argv[]);
        static void GetBests(Juken* _this, int argc, char* argv[]);
        static void DoChangeDisc(Juken* _this, int argc, char* argv[]);
        static void DoPlay(Juken* _this, int argc, char* argv[]);
        static void DoPrev(Juken* _this, int argc, char* argv[]);
        static void DoNext(Juken* _this, int argc, char* argv[]);
        static void DoStop(Juken* _this, int argc, char* argv[]);
        static void DoQuit(Juken* _this, int argc, char* argv[]);


    private:
        static Juken* g_juken; // a ref to 'this' for callbacks

        KenwoodDevice* m_device;
        KenwoodChanger* m_changer;

        bool m_done;

        FILE* m_file;

        bool        m_door_closed;
        short       m_cur_slot;
        byte        m_cur_track;
        enum state  m_cur_state;
        enum mode   m_cur_mode;
        enum random m_cur_random;
        bool        m_cur_repeat;
        byte        m_cur_userfile;
        short       m_capacity;
        char**      m_titles;

        bool m_loading_titles;

        static void readline_callback(char* line);
        static char** completion_callback(const char* text, int start, int end);

};

#endif /* JUKEN_JUKEN_H */
