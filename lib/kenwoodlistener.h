#ifndef JUKEN_KENWOODLISTENER_H
#define JUKEN_KENWOODLISTENER_H

#include "types.h"
//#include "payload.h"

class KenwoodChanger;

class KenwoodListener 
{
    public:
        ~KenwoodListener() { };

        virtual void InfoChanged(KenwoodChanger* changer,
                                 short slot, byte title, short chapter) { }
        virtual void ModeChanged(KenwoodChanger* changer,
                                 enum mode mode, bool repeat, byte param) { }
        virtual void StateChanged(KenwoodChanger* changer,
                                 enum state state) { }
        virtual void DoorChanged(KenwoodChanger* changer,
                                 bool door_open) { }

        enum operation { ScanDiscs, LoadUserfiles, ChangeDisc };

        virtual void ProgressStart(KenwoodChanger* changer,
                                 enum operation op, int length) { };
        virtual void Progress(KenwoodChanger* changer,
                                 enum operation op, int progress) { };
        virtual void ProgressEnd(KenwoodChanger* changer,
                                 enum operation op) { };

    protected:
        KenwoodListener() { };

    private:
};

#endif /* JUKEN_KENWOODLISTENER_H */
