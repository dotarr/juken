#ifndef JUKEN_EXPORTER_H
#define JUKEN_EXPORTER_H

#include <kenwooddevice.h>
#include <kenwoodchanger.h>
#include <kenwoodlistener.h>

#include "changerdata.h"

class Exporter : public KenwoodListener
{
    public:
        Exporter(const char* device);
        ~Exporter();

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
        void DoExport(char* dir, short start, short end);

    private:
        ChangerData* m_data;

        KenwoodDevice* m_device;
        KenwoodChanger* m_changer;

        bool m_done;
};

#endif /* JUKEN_EXPORTER_H */
