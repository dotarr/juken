#ifndef JUKEN_KENWOODCHANGER_H
#define JUKEN_KENWOODCHANGER_H

#include <deque>

#include "kenwooddevice.h"
#include "kenwoodlistener.h"
#include "types.h"
#include "payload.h"

// A class for communications to a Kenwood changer via a serial port.

class KenwoodChanger
{
    public:
        KenwoodChanger(char* id, short capacity, KenwoodDevice& dev, KenwoodListener* listener);
        virtual ~KenwoodChanger();

        void pushListener(KenwoodListener* listener);
        void popListener();

        void DoEvent();
        virtual void DoInfoEvent(const payload& event) { };
        virtual void DoStateEvent(const payload& event) { };
        virtual void DoDiscEvent(const payload& event) { };
        virtual void DoDoorEvent(const payload& event) { };

        virtual char* getIdentifier() { return m_identifier; }
        virtual short getCapacity() { return m_capacity; }
        virtual const char* getUserfileName(byte i) { return m_userfiles[i]; }

        virtual bool isSlotOccupied(short i) { return (m_slots[i-1]!=NULL); }
        virtual bool isStopped() { return (m_cur_state==Stopped); }
        virtual bool isDoorClosed() { return (m_cur_door_pos==DoorClosed); }
        virtual bool isSlotCurrent(short i) { return (m_cur_slot==i); }

        virtual NameList DoListUserfiles() = 0;
        virtual void DoListUserfiles(void* context, NameCallback* callback) = 0;

        virtual void DoListDiscs(void* context, DiscCallback* callback) = 0;
        
        virtual Disc DoListContents(const short slot) = 0;
        virtual void DoListContents(const short slot, void* context, DiscCallback* callback) = 0;

        virtual char* GetDiscId(const short slot) = 0;

        virtual void DoChangeDisc(const short slot) = 0;

        virtual void DoPlayPause() = 0;
        virtual void DoPrev() = 0;
        virtual void DoNext() = 0;
        virtual void DoStop() = 0;

        virtual void WriteUserfileNames(const char* names[]) = 0;
        virtual void WriteDisc(short slot, Disc& disc) = 0;

    protected:
        char* m_identifier;
        short m_capacity;

        Disc** m_slots;
        char* m_userfiles[8];

        short m_cur_slot;
        byte m_cur_title;
        short m_cur_chapter;
        enum mode m_cur_mode;
        enum repeat m_cur_repeat;
        byte m_cur_param;
        enum state m_cur_state;
        enum door m_cur_door_pos;

        KenwoodDevice& m_device;
        KenwoodListener* m_listener;

        void IssueRequest(const payload& msg, const bool has_replies);
        void GetOneReply(payload& reply);
        bool GetReply(payload& reply);

        void InfoChanged(short slot, byte title, short chapter);
        void ModeChanged(enum mode mode, enum repeat repeat, byte param);
        void StateChanged(enum state state);
        void DoorChanged(enum door door_open);

        void ScanDiscs();
        void LoadUserfiles();

    private:
        static void ScanDiscsCallback(void* context, Disc& data);
        static void LoadUserfilesCallback(void* context, Name& data);

        KenwoodChanger();
        KenwoodChanger(const KenwoodChanger& changer);
};

#endif /* JUKEN_KENWOODCHANGER_H */
