#ifndef JUKEN_IMPORTER_H
#define JUKEN_IMPORTER_H

#include <kenwooddevice.h>
#include <kenwoodchanger.h>
#include <consolelistener.h>

#include "changerdata.h"

class Importer
{
    public:
        Importer(const char* filename);
        ~Importer();

        void InitState();

        void Run();
        
    private:
        KenwoodDevice* m_device;
        ConsoleListener* m_listener;
        KenwoodChanger* m_changer;

        ChangerData m_data;

        void print_data();
};

#endif /* JUKEN_IMPORTER_H */
