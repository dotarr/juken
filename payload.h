#ifndef __PAYLOAD_H__
#define __PAYLOAD_H__

#include "types.h"

typedef struct
{
    byte cmd;
    ushort len;
    byte data[MAX_PAYLOAD_LEN];
} payload;

typedef struct
{
    char identifier[1];
} data_0x00;

typedef struct
{
    byte action;
    byte data_type;
    short slot;
    byte unknown;
    byte request_type;
    byte genre;
} data_0x03;

typedef struct
{
    short slot;
    byte unknown;
    byte num_tracks;
    byte formatting;
} data_0x04;

typedef struct 
{
    byte min;
    byte sec;
    byte subsec;
} start_times;
typedef struct
{
    short slot;
    byte unknown_1;
    byte formatting;
    byte unknown_2;
    byte num_tracks;
    start_times start[1];
} data_0x06;

typedef struct
{
    short slot;
    byte userfiles;
} data_0x07;

typedef struct
{
    short slot;
    byte genre;
} data_0x08;

typedef struct
{
    byte data_type;
} data_0x09;

typedef struct
{
    short type;
} data_0x0A;

typedef struct
{
    short slot;
    byte track;
    byte begin;
} data_0x0B;

typedef struct
{
    byte unknown;
    byte userfile;
} data_0x0C;

typedef struct
{
    byte num_tracks;
    struct 
    {
        short slot;
        byte track;
    } best[1];
} data_0x0D;

typedef struct
{
    short slot;
    byte track;
    byte best_mode;
    byte num_tracks;
    byte userfiles;
    byte userfile_mode;
    byte random_mode;
    byte repeat_mode;
} data_0x12;

typedef struct
{
    byte state;
} data_0x13;

typedef struct
{
    short slot;
} data_0x14;

typedef struct
{
    byte door_pos;
} data_0x15;

typedef struct
{
    short slot;
    byte track;
    byte unknown_1;
    byte unknown_2;
    byte unknown_3;
    byte unknown_4;
    byte unknown_5;
    char title[1];
} data_0xFD;

typedef struct
{
    short slot;
    byte track;
    byte userfiles;
    byte request_type;
    byte genre;
    byte formatting;
    char title[1];
} data_0xFE_a;

typedef struct
{
    byte unknown_1;
    byte unknown_2;
    byte userfile;
    byte unknown_3;
    byte unknown_4;
    byte unknown_5;
    byte unknown_6;
    char title[1];
} data_0xFE_b;

#endif /* __PAYLOAD_H__ */
