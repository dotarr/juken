#ifndef __JUKEBOX_H__
#define __JUKEBOX_H__

#include <unistd.h>

#include "util.h"
#include "types.h"

class Jukebox
{
    public:
        Jukebox();
        virtual ~Jukebox();

        void ProcessIncomingMessage(int fd);
        void ProcessOutgoingMessage(int fd, int msg_fd);

        void IssueChangeState(int fd, const byte state);
        void IssueQueryDevice(int fd,
                              const int b0,
                              const byte t,
                              const ushort d,
                              const int b1,
                              const int b2,
                              const int b3);
        void IssueRequest(int fd, byte cmd, struct payload& msg, bool replies);
        bool GetReply(int fd, struct payload& msg);

    protected:
        void ProcessEvent(const byte cmd, const struct payload& msg);

        void HandleInfoEvent(const struct payload& msg);
        void HandleStateEvent(const struct payload& msg);
        void HandleDiscEvent(const struct payload& msg);
        void HandleDoorEvent(const struct payload& msg);

    private:
        Jukebox(const Jukebox&);

        byte checksum(byte cmd, const struct payload& msg);

        void write_payload(const byte cmd, const struct payload& msg, int fd);
        byte read_payload(byte& cmd, struct payload& msg, int fd);

        void writefully(int fd, const void* buf, size_t count);
        int readfully(int fd, void* buf, size_t count);

        void writecntl(int fd, byte c);
	byte readcntl(int fd);
        void writec(int fd, byte c);
        byte readc(int fd);
};

#endif /* __JUKEBOX_H__ */
