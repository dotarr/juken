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
        protocol.IssueQueryDevice(fd, 0x00, 0x01, disc, 0x00, 0x01, 0x00);

        // process the reply
        struct payload reply;
        while ( protocol.GetReply(fd, reply) )
        {
            // cast the reply into a track_data
            struct disc_data* info = (struct disc_data*) reply.data;

            // add null terminator to title
            byte data_len = sizeof(struct disc_data)-MAX_TITLE_LENGTH-1;
            byte title_len = 0;
            if ( info->title[0] != 0x01 )
                title_len = reply.len-data_len;
            info->title[title_len] = '\0';

            // output the reply
            DebugMsg("slot:%3d index:%3d unknown:0x%02x title:%s genre:%s userfiles:0x%02X formatting:%s\n",
                    info->slot, info->index, info->unknown1, info->title, GENRE_NAMES[info->genre], info->userfiles, (info->format==0x13)?"cd-text":"none");
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


