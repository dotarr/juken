#ifndef JUKEN_EXPORTER_H
#define JUKEN_EXPORTER_H

#include <kenwooddevice.h>
#include <kenwoodchanger.h>

#include "changerdata.h"
#include "consolelistener.h"

class Exporter
{
    public:
        Exporter(const char* device, short start=-1, short end=-1);
        ~Exporter();

        void InitState();

        void Run();

    private:
        KenwoodDevice* m_device;
        ConsoleListener* m_listener;
        KenwoodChanger* m_changer;

        const char* m_device_name;
        char* m_model_name;

        short m_start;
        short m_end;
};

#endif /* JUKEN_EXPORTER_H */
