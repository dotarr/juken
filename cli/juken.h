#ifndef JUKEN_JUKEN_H
#define JUKEN_JUKEN_H

#include <kenwooddevice.h>
#include <kenwoodchanger.h>
#include "consolelistener.h"

class Juken
{
    public:
        Juken(const char* serial_device);
        ~Juken();

        void InitState();

        void Run();

    protected:
        typedef void (*cmd_func)(Juken* _this, int argc, char* argv[]);
        typedef struct cmd { char* name; cmd_func func; char* help; };

        static struct cmd m_short_commands[];
        static struct cmd m_commands[];

        static void ListingDiscsCallback(void* context, Disc& data);
        static void ListingDiscContentsCallback(void* context, Disc& data);

        void DoCommand(char* line);

        static void DoHelp(Juken* _this, int argc, char* argv[]);
        static void DoList(Juken* _this, int argc, char* argv[]);
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
        ConsoleListener* m_listener;
        KenwoodChanger* m_changer;

        bool m_done;
        FILE* m_file;

        static void readline_callback(char* line);
        static char** completion_callback(const char* text, int start, int end);
};

#endif /* JUKEN_JUKEN_H */
