#ifndef JUKEN_PAYLOAD_H
#define JUKEN_PAYLOAD_H

#include "types.h"

typedef struct
{
    byte cmd;
    ushort len;
    byte data[MAX_PAYLOAD_LEN];
} payload;

//command = 0x00
typedef struct
{
    char identifier[1];
} Handshake;

//command = 0x03
typedef struct
{
    byte action;
    byte data_type;
    short slot;
    byte unknown;
    byte request_type;
    byte genre;
} DataAccess;

//command = 0x04
typedef struct
{
    short slot;
    byte unknown;
    byte num_tracks;
    byte formatting;
} DiscInfo;

//command = 0x06
typedef struct 
{
    byte minute;
    byte second;
    byte subsecond;
} TimeInfo;
typedef struct
{
    short slot;
    byte unknown_1;
    byte formatting;
    byte unknown_2;
    byte num_tracks;
    TimeInfo times[1];
} TrackTimes;

//command = 0x07
typedef struct
{
    short slot;
    byte userfiles;
} DiscUserfiles;

//command = 0x08
typedef struct
{
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
    short type;
} DoAction;

//command = 0x0B
typedef struct
{
    short slot;
    byte track;
    byte begin;
} SelectDiscTrack;

//command = 0x0C
typedef struct
{
    byte unknown;
    union
    {
        byte userfile;
        byte genre;
    };
} SelectPlayMode;

//command = 0x0D
typedef struct 
{
    short slot;
    byte track;
} DiscTrack;
typedef struct
{
    byte num_tracks;
    DiscTrack tracks[1];
} DiscTrackList;

//command = 0x12
typedef struct
{
    short slot;
    byte track;
    byte unknown;
    byte num_tracks;
    byte userfiles;
    byte userfile;
    byte mode;
    byte repeat;
} InfoEvent;

//command = 0x13
typedef struct
{
    byte state;
} StateEvent;

//command = 0x14
typedef struct
{
    short slot;
} DiscEvent;

//command = 0x15
typedef struct
{
    byte door_pos;
} DoorEvent;

//command = 0xFD
typedef struct
{
    short slot;
    byte track;
    byte unknown_1;
    byte request_type;
    byte unknown_2;
    byte formatting;
    byte unknown_3;
    char title[1];
} CDTextData;

//command = 0xFE
typedef struct
{
    short slot;
    byte track;
    byte userfiles;
    byte request_type;
    byte genre;
    byte formatting;
    char title[1];
} DiscData;

//command = 0xFE
typedef struct
{
    short unknown_1;
    byte userfile;
    byte unknown_2;
    byte unknown_3;
    byte unknown_4;
    byte unknown_5;
    char title[1];
} UserfileData;

#endif /* JUKEN_PAYLOAD_H */
