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

#endif /* JUKEN_PAYLOAD_H */
