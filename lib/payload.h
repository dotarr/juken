#ifndef JUKEN_PAYLOAD_H
#define JUKEN_PAYLOAD_H

#include "types.h"

class payload
{
    public:
        payload() : cmd(0), len(0) { }
        payload(const payload& info)
        {
            cmd = info.cmd;
            len = info.len;
            ::memcpy(data, info.data, len+1);
        }
        payload(const byte cmd, const ushort len, const byte* data)
        {
            this->cmd = cmd;
            this->len = len;
            ::memcpy(this->data, data, len);
        }
        byte cmd;
        ushort len;
        byte data[MAX_PAYLOAD_LEN];
};

//command = 0x00
class Handshake : public payload
{
    public:
        Handshake(const payload& info) : payload(info) { }
        Handshake(const char* id) : payload(HANDSHAKE, (ushort) ::strlen(id), (byte*) id) { }
        char* identifier() { return (char*) data; }
};

#endif /* JUKEN_PAYLOAD_H */
