#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

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
        protocol.IssueQueryDevice(fd, 0x00, 0x04, disc, 0x00, 0x00, 0x00);

        // process the reply
        struct payload reply;
        while ( protocol.GetReply(fd, reply) )
        {
            short slot = *((short*) reply.data);
            byte unknown1 = reply.data[1];
            byte unknown2 = reply.data[2];
            byte unknown3 = reply.data[3];
            byte unknown4 = reply.data[4];
            byte num_tracks = reply.data[5];
            DebugMsg("slot:%3d unknown:0x%02X,0x%02X,0x%02X,0x%02X num_tracks:%3d\n",
                    slot, unknown1, unknown2, unknown3, unknown4, num_tracks);
            DebugMsg("track\tstart\tunknown\n");
            DebugMsg("-----------------------------\n");
            byte* p = &reply.data[6];
            for (int i=0; i<num_tracks+1; i++)
            {
                byte start_min = *(p++);
                byte start_sec = *(p++);
                byte unknown = *(p++);
                DebugMsg("%5d\t%02x:%02x\t0x%02X\n", i+1, start_min, start_sec, unknown);
            }
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


