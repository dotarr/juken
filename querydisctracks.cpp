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

        // issue the query request
        protocol.IssueQueryDevice(fd, 0x00, 1, 33, 0x00, 0x01, 0x00);

        // process the reply
        struct payload reply;
        while ( protocol.GetReply(fd, reply) )
        {
            // cast the reply into a track_data
            struct track_data* info = (struct track_data*) reply.data;
            int index = info->index;

            // add null terminator to title
            byte data_len = sizeof(struct track_data)-MAX_TRACK_TITLE_LENGTH-1;
            byte title_len = 0;
            if ( info->title[0] != 0x01 )
                title_len = reply.len-data_len;
            info->title[title_len] = '\0';

            // output the reply
            DebugMsg("unknown: %02x %02x index:%d unknown: %02x %02x %02x %02x title:%s\n",
                     info->unknown1, info->unknown2, index, info->unknown3, 
                     info->unknown4, info->unknown5, info->unknown6, info->title);
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


