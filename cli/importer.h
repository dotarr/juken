#ifndef JUKEN_IMPORTER_H
#define JUKEN_IMPORTER_H

#include <kenwooddevice.h>
#include <kenwoodchanger.h>
#include <kenwoodlistener.h>

#include "changerdata.h"

class Importer : public KenwoodListener
{
    public:
        Importer(const char* filename);
        ~Importer();

        void InitState();

        void Run();

        bool InfoChanged(short slot, byte title, short chapter);
        bool ModeChanged(enum mode mode, bool repeat, byte param);
        bool StateChanged(enum state state);
        bool DoorChanged(bool door_open);
        
        bool TextDataReply(short slot, byte track, byte userfiles, 
                           byte request_type, byte genre, 
                           byte formatting, char* title);

    private:
        ChangerData m_data;

        KenwoodDevice* m_device;
        KenwoodChanger* m_changer;

        bool m_done;

        void print_data();
};

#endif /* JUKEN_IMPORTER_H */
