#ifndef JUKEN_DVDPAYLOAD_H
#define JUKEN_DVDPAYLOAD_H

#include "payload.h"

struct Foo
{
    Foo(const byte* data)
    {
        changer = data[0];
    }
    byte changer;
};

//command = 0x03
typedef struct
{
    byte action;
    byte data_type;
    byte unknown_1;
    byte unknown_2;
    short slot;
    byte unknown_3;
    byte unknown_4;
    byte unknown_5;
    byte unknown_6;
} DataAccess;

//command = 0x04
typedef struct
{
    byte changer;
    byte unknown_1;
    short slot;
    byte unknown_2;
    char data[1];
} DiscInfo;

//command = 0x06
typedef struct
{
    byte changer;
    byte unknown_1;
    short slot;
    byte unknown_2;
    byte formatting;
    char data[1];
} TrackTimes;

//command = 0x07
typedef struct
{
    byte changer;
    short slot;
    byte userfiles;
} DiscUserfiles;

//command = 0x08
typedef struct
{
    byte changer;
    short slot;
    byte genre;
} DiscGenre;

//command = 0x09
typedef struct
{
    byte data_type;
} ReadyForData;

//command = 0x0A
typedef struct
{
    byte changer;
    short type;
} DoAction;

//command = 0x0B
typedef struct
{
    byte changer;
    short slot;
    byte unknown_1;
    byte unknown_2;
    byte unknown_3;
    byte unknown_4;
    byte unknown_5;
    byte unknown_6;
    byte unknown_7;
} SelectDiscTrack;

//command = 0x0D
typedef struct
{
    byte changer;
    byte num_tracks;
    DiscTrackChapter tracks[1];
} DiscTrackList;

//command = 0x12
typedef struct
{
    short slot;
    byte title;
    byte chapter;
    byte unknown_1;
    byte unknown_2;
    byte unknown_3;
    byte unknown_4;
    byte unknown_5;
} InfoEvent;

//command = 0x13
typedef struct
{
    byte state;
    byte unknown_1;
    byte unknown_2;
    byte unknown_3;
    byte unknown_4;
    byte unknown_5;
    byte unknown_6;
} StateEvent;

//command = 0xFD
typedef struct
{
    byte changer;
    byte unknown_1;
    byte unknown_2;
    short slot;
    byte unknown_3;
    byte unknown_4;
    byte unknown_5;
    char title[1];
} CDTextData;

//command = 0xFE
typedef struct
{
    byte changer;
    byte unknown_1;
    short slot;
    byte unknown_2;
    byte unknown_3;
    byte genre;
    char title[1];
} DiscData;

//command = 0xFE
typedef struct
{
    byte changer;
    short unknown_1;
    byte userfile;
    byte unknown_2;
    byte unknown_3;
    byte unknown_4;
    byte unknown_5;
    char title[1];
} UserfileData;

#endif /* JUKEN_DVDPAYLOAD_H */
