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

typedef struct 
{
    byte minute;
    byte second;
    byte subsecond;
} TimeInfo;

typedef struct 
{
    short slot;
    byte track;
} DiscTrack;

typedef struct 
{
    short slot;
    byte track;
} DiscTrackChapter;

#endif /* JUKEN_PAYLOAD_H */
