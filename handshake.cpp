#include <stdlib.h>
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
    // no arguments yet ...
}

int
main (int argc, char* argv[])
{
    // parse parameters
    parse_args(argc, argv);

    try
    {
        // create the protocol object and messaging socket
        Jukebox protocol;
        UnixDomainSock sock;
        int fd = sock.OpenSock(messaging_socket);

        // prepare the handshake request
        const char* NAME = "I'm PC";
        struct payload msg;
        msg.len = strlen(NAME);
        ::memcpy(msg.data, (byte*) NAME, msg.len);

        // issue the request
        protocol.IssueRequest(fd, HANDSHAKE_REQ, msg, HAS_REPLIES); 

        // process the reply
        struct payload reply;
        while ( protocol.GetReply(fd, reply) )
        {
            // copy the reply into an allocated string
            ushort name_len = reply.len-4;
            char* name = new char[name_len+1];
            ::strncpy(name, (char*) &reply.data[4], name_len);
            name[name_len] = '\0';

            // output the reply
            DebugMsg("connected to %s\n", name);

            // free the string
            delete[] name;
        }
    }
    catch (char* e)
    {
        // output the error
        ::fprintf(stderr, "%s\n", e);

        // exit with failure
        return EXIT_FAILURE;
    }

    // return with success
    return EXIT_SUCCESS;
}


