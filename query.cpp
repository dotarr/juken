#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <stdlib.h>

#include "unixdomainsock.h"
#include "jukebox.h"

char* messaging_socket = "/tmp/juken";

byte disc = 0;

void
parse_env()
{
    char* sock = getenv("JUKEN_SOCK");
    if ( sock != NULL ) messaging_socket = sock;
}

void
parse_args(int argc, char* argv[])
{
    if ( argc > 1 )
        disc = (byte) atol(argv[1]);
}

int
main (int argc, char* argv[])
{
    // parse parameters
    parse_env();
    parse_args(argc, argv);

    try
    {
        // create the protocol object and messaging socket
        Jukebox protocol;
        UnixDomainSock sock;
        int fd = sock.OpenSock(messaging_socket);

        // issue the query request
        protocol.IssueQueryDevice(fd, 0x20, 0x00, 0x00, 0x00, 0x00, 0x07);

        // process the reply(s)
        struct payload reply;
        while ( protocol.GetReply(fd, reply) )
        {
            printdata(reply.data, reply.len);
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

