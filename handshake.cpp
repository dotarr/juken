#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

#include "unixdomainsock.h"
#include "jukebox.h"

char* messaging_socket = "/tmp/juken";

void
parse_args(int argc, char* argv[])
{
}

int
main (int argc, char* argv[])
{
    parse_args(argc, argv);

    try
    {
        Jukebox protocol;
        UnixDomainSock sock;
        int fd = sock.OpenSock(messaging_socket);

        const char* NAME = "I'm PC";
        struct payload msg;
        msg.len = strlen(NAME);
        ::memcpy(msg.data, (byte*) NAME, msg.len);

        protocol.IssueRequest(fd, HANDSHAKE_REQ, msg, HAS_REPLIES); 

        struct payload reply;
        while ( protocol.GetReply(fd, reply) )
        {
            ushort name_len = reply.len-4;
            char* name = new char[name_len+1];
            ::strncpy(name, (char*) &reply.data[4], name_len);
            name[name_len] = '\0';

            DebugMsg("connected to %s\n", name);
            delete[] name;
        }
    }
    catch (char* e)
    {
        ::fprintf(stderr, "%s\n", e);
        return 0;
    }
    return 1;
}


