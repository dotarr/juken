#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <stdlib.h>

#include "unixdomainsock.h"
#include "jukebox.h"

char* messaging_socket = "/tmp/juken";

short disc = 0;
byte track= 0;
byte play = 0;

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
        disc = (short) atol(argv[1]);
    if ( argc > 2 )
        track = (byte) atol(argv[2]);
    if ( argc > 3 )
        play = (byte) atol(argv[3]);
::fprintf(stderr, "disc=%d track=%d play=%d\n", disc, track, play);
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

        // prepare the handshake request
        struct payload msg;
        msg.len = 4;
        *((short*) msg.data) = disc;
        msg.data[2] = track;
        msg.data[3] = play;

                // issue the request
        protocol.IssueRequest(fd, 11, msg, NO_REPLIES); 
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

