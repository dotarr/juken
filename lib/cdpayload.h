#ifndef JUKEN_CDPAYLOAD_H
#define JUKEN_CDPAYLOAD_H

#include "payload.h"
#include "discid.h"

//
//command = 0x03
class DataAccess : public payload
{
    public:
        DataAccess(enum access access, enum data_type type, short slot,
                   byte info_type, enum genre genre)
        {
            cmd = DATA_ACCESS;
            len = 7;
            data[0] = (byte) access;
            data[1] = type;
            *((short*) (&data[2])) = slot;
            data[4] = 0;
            data[5] = (byte) info_type;
            data[6] = (byte) genre;
        }
};

//command = 0x04
class DiscInfo : public payload
{
    public:
        DiscInfo(const payload& info) : payload(info) { }
        short slot() { return *((short*) (&data[0])); }
        byte first_track() { return data[2]; }
        byte last_track() { return data[3]; }
        byte length() { return last_track()-first_track()+1; }
        byte formatting() { return data[4]; }
};

//command = 0x06
class DiscTOC : public payload
{
    public:
        DiscTOC(const payload& info) : payload(info) { }
        short slot() { return *((short*) (&data[0])); }
        byte page_num() { return data[2]; }
        byte formatting() { return data[3]; }
        byte first_track() { return data[4]; }
        byte last_track() { return data[5]; }
        byte length() { return last_track()-first_track()+1; }
        TimeInfo* times() { return (TimeInfo*) (&data[6]); }
        uint disc_id() { return ::discid(length(), times()); }
} ;

//command = 0x07
class DiscUserfiles : public payload
{
    public:
        DiscUserfiles(const payload& info) : payload(info) { }
        short slot() { return *((short*) (&data[0])); }
        byte userfiles() { return data[2]; }
};

//command = 0x08
class DiscGenre : public payload
{
    public:
        DiscGenre(const payload& info) : payload(info) { }
        short slot() { return *((short*) (&data[0])); }
        enum genre genre() { return (enum genre) data[2]; }
};

//command = 0x09
class ReadyForData : public payload
{
    public:
        ReadyForData(const payload& info) : payload(info) { }
        byte type() { return data[0]; }
};

//command = 0x0A
class DoAction : public payload
{
    public:
        DoAction(const short action) : payload(DO_ACTION, 2, (byte*) &action) { }
};

//command = 0x0B
class ChangeDisc : public payload
{
    public:
        ChangeDisc(const short slot, const byte track, const byte state)
        {
            cmd = CHANGE_DISC;
            len = 4;
            *((short*) (&data[0])) = slot;
            data[2] = track;
            data[3] = state;
        }
};

//command = 0x0C
class ChangeMode : public payload
{
    public:
        ChangeMode(const byte mode, const byte param)
        {
            cmd = CHANGE_MODE;
            len = 2;
            data[0] = mode;
            data[1] = param;
        }
};

//command = 0x0D
class DiscListing : public payload
{
    public:
        DiscListing(const payload& info) : payload(info) { }
        byte length() { return data[0]; }
        DiscTrack* tracks() { (DiscTrack*) (&data[1]); }
} ;

//command = 0x12
class InfoEvent : public payload
{
    public:
        InfoEvent(const payload& info) : payload(info) { }
        short slot() const { return *((short*) (&data[0])); }
        byte track() const { return data[2]; }
        byte program() const { return data[3]; }
        byte num_tracks() const { return data[4]; }
        byte userfiles() const { return data[5]; }
        byte param() const { return data[6]; }
        enum mode mode() const { return (enum mode) data[7]; }
        enum repeat repeat() const { return (enum repeat) data[8]; }
};

//command = 0x13
class StateEvent : public payload
{
    public:
        StateEvent(const payload& info) : payload(info) { }
        enum state state() const { return (enum state) data[0]; }
};

//command = 0x14
class DiscEvent : public payload
{
    public:
        DiscEvent(const payload& info) : payload(info) { }
        short slot() { return *((short*) (&data[0])); }
};

//command = 0x15
class DoorEvent : public payload
{
    public:
        DoorEvent(const payload& info) : payload(info) { }
        enum door door_pos() { return (enum door) data[0]; }
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
        TextData(const short slot, const byte index, const byte userfiles,
                 const byte text_type, const byte genre, const byte formatting,
                 const char* text)
        {
            int text_len = ::strlen(text);
            if ( text_len <= 20 )
            {
                text_len = 20;
            }
            cmd = TEXT_DATA;
            len = 7 + text_len;
            *((short*) (&data[0])) = index;
            data[2] = index;
            data[3] = userfiles;
            data[4] = text_type;
            data[5] = genre;
            data[6] = formatting;
            ::memcpy(&data[7], text, text_len);
        }
        short slot() { return *((short*) (&data[0])); }
        byte index() { return data[2]; }
        byte userfiles() { return (cmd==TEXT_DATA) ? data[3] : 0; }
        byte text_type() { return data[4]; }
        byte genre() { return (cmd==TEXT_DATA) ? data[5] : 0; }
        byte formatting() { return data[6]; }
        char* text() { return (char*) ((cmd==TEXT_DATA) ? &data[7] : &data[8]); }
};

#endif /* JUKEN_CDPAYLOAD_H */
