#ifndef JUKEN_DVDPAYLOAD_H
#define JUKEN_DVDPAYLOAD_H

#include "payload.h"
#include "discid.h"

//
//command = 0x03
class DataAccess : public payload
{
    public:
        DataAccess(enum access access, enum data_type type, 
                   byte info_type, byte changer, short slot, 
                   byte title, short chapter)
        {
            cmd = DATA_ACCESS;
            len = 10;
            data[0] = (byte) access;
            data[1] = (byte) type;
            data[2] = info_type;
            data[3] = changer;
            *((short*) (&data[4])) = slot;
            data[6] = title;
            *((short*) (&data[7])) = chapter;
            data[9] = 0;
        }
};

//command = 0x04
class DiscInfo : public payload
{
    public:
        DiscInfo(const payload& info) : payload(info) { }
        byte page() { return data[0]; }
        byte changer() { return data[1]; }
        short slot() { return *((short*) (&data[2])); }
        byte formatting() { return data[4]; }

        byte first_track() { return data[5]; }
        byte last_track() { return data[6]; }
        byte length() { return last_track()-first_track()+1; }

        byte title_count() { return data[5]; }
        short* chapter_counts() { return (short*) (&data[6]); }
};

//command = 0x06
class DiscTOC : public payload
{
    public:
        DiscTOC(const payload& info) : payload(info) { }
        byte page() { return data[0]; }
        byte changer() { return data[1]; }
        short slot() { return *((short*) (&data[2])); }
        byte formatting() { return data[4]; }
        
        byte first_track() { return data[5]; }
        byte last_track() { return data[6]; }
        byte length() { return last_track()-first_track()+1; }
        TimeInfo max_time() { return *((TimeInfo*) (&data[7])); }
        TimeInfo* times() { return (TimeInfo*) (&data[10]); }
        uint disc_id() { return ::discid(length(), times()); }

        char* vol_id() { return (char*) (&data[5]); }
} ;

//command = 0x07
class DiscUserfiles : public payload
{
    public:
        DiscUserfiles(const payload& info) : payload(info) { }
        byte changer() { return data[0]; }
        short slot() { return *((short*) (&data[1])); }
        byte userfiles() { return data[3]; }
};

//command = 0x08
class DiscGenre : public payload
{
    public:
        DiscGenre(const payload& info) : payload(info) { }
        byte changer() { return data[0]; }
        short slot() { return *((short*) (&data[1])); }
        enum genre genre() { return (enum genre) data[3]; }
};

//command = 0x09
class ReadyForData : public payload
{
    public:
        ReadyForData(const payload& info) : payload(info) { }
        byte changer() { return data[0]; }
        byte type() { return data[1]; }
};

//command = 0x0A
class DoAction : public payload
{
    public:
        DoAction(const byte changer, const short action)
        {
            cmd = DO_ACTION;
            len = 3;
            data[0] = changer;
            *((short*) (&data[1])) = action;
        }
};

//command = 0x0B
class ChangeDisc : public payload
{
    public:
        ChangeDisc(const byte changer, const short slot, 
                   const byte title, const short chapter,
                   const byte mode, const byte param, const byte state)
        {
            cmd = CHANGE_DISC;
            len = 10;
            data[0] = changer;
            *((short*) (&data[1])) = slot;
            data[3] = title;
            *((short*) (&data[4])) = chapter;
            data[6] = mode;
            data[7] = param;
            data[8] = state;
            data[9] = 0;
        }
};

//command = 0x0C
class ChapterFrames : public payload
{
    public:
        ChapterFrames(const payload& info) : payload(info) { }
        byte page() { return data[0]; }
        byte changer() { return data[1]; }
        short slot() { return *((short*) (&data[2])); }
        short length() { return *((short*) (&data[4])); }
        ChapterFrame* frames() { return (ChapterFrame*) (&data[6]); }
};

//command = 0x0D
class DiscListing : public payload
{
    public:
        DiscListing(const payload& info) : payload(info) { }
        byte changer() { return data[0]; }
        byte length() { return data[1]; }
        DiscTitleChapter* tracks() { (DiscTitleChapter*) (&data[2]); }
} ;

//command = 0x0E
class DiscVolumeId : public payload
{
    public:
        DiscVolumeId(const payload& info) : payload(info) { }
        byte changer() { return data[0]; }
        short slot() { return *((short*) (&data[1])); }
        byte formatting() { return data[3]; }
        VolumeId* volume_id() { (VolumeId*) (&data[4]); }
} ;

//command = 0x0F
class DiscTimeStamp : public payload
{
    public:
        DiscTimeStamp(const payload& info) : payload(info) { }
        byte changer() { return data[0]; }
        short slot() { return *((short*) (&data[1])); }
        byte formatting() { return data[3]; }
        TimeStamp* volume_id() { (TimeStamp*) (&data[4]); }
} ;

//command = 0x12
class InfoEvent : public payload
{
    public:
        InfoEvent(const payload& info) : payload(info) { }
        byte changer() const { return data[0]; }
        short slot() const { return *((short*) (&data[1])); }
        byte title() const { return data[3]; }
        short chapter() const { return *((short*) (&data[4])); }
        byte program() const { return data[6]; }
        byte formatting() const { return data[7]; }
        bool toc_complete() const { return (data[8]!=0); }
        byte genre() const { return data[9]; }
        byte userfile() const { return data[10]; }
};

//command = 0x13
class StateEvent : public payload
{
    public:
        StateEvent(const payload& info) : payload(info) { }
        byte changer() const { return data[0]; }
        enum state state() const { return (enum state) data[1]; }
        bool at_end() const { return (data[2]!=0); }
        enum mode mode() const { return (enum mode) data[3]; }
        byte param() const { return data[4]; }
        enum repeat repeat() const { return (enum repeat) data[5]; }
        enum door door_pos() const { return (enum door) data[6]; }
        bool library() const { return (data[7]!=0); }
};

//command = 0xFD or command = 0xFE
class TextData : public payload
{
    public:
        TextData(const payload& info) : payload(info)
            { 
                if ( cmd==TEXT_DATA && data[7]==0x01 ) data[7] = 0;
                if ( cmd!=TEXT_DATA && data[8]==0x01 ) data[8] = 0;
            }
        TextData(const byte changer, const byte text_type, 
                 const short index, const byte formatting,
                 const byte userfiles, const byte genre, const char* text)
        {
            int text_len = ::strlen(text);
            if ( text_len > 20 )
                text_len = 20;
            cmd = TEXT_DATA;
            len = 7 + text_len;
            data[0] = changer;
            data[1] = text_type;
            *((short*) (&data[2])) = index;
            data[4] = formatting;
            data[5] = userfiles;
            data[6] = genre;
            ::memcpy(&data[7], text, text_len);
        }
        byte page() { return (cmd==TEXT_DATA) ? 0 : data[0]; }
        byte changer() { return (cmd==TEXT_DATA) ? data[0] : data[1]; }
        byte text_type() { return ((cmd==TEXT_DATA) ? data[1] : data[2]); }
        short index() { return *((short*) ((cmd==TEXT_DATA) ? &data[2] : &data[3])); }
        byte formatting() { return (cmd==TEXT_DATA) ? data[4] : data[5]; }
        byte userfiles() { return (cmd==TEXT_DATA) ? data[5] : data[6]; }
        byte genre() { return (cmd==TEXT_DATA) ? data[6] : data[7]; }
        char* text() { return (char*) ((cmd==TEXT_DATA) ? &data[7] : &data[8]); }
};

//command = 0xFE
class UserfileData : public payload
{
    public:
        UserfileData(const payload& info) : payload(info) { }
        byte userfile() { return data[2]; }
        char* title() { return ::strdup((char*) &data[7]); }
};

#endif /* JUKEN_CDPAYLOAD_H */
