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
        protocol.IssueQueryDevice(fd, 0x00, 1, 0, 0x00, 0x00, 0x00);

        // process the reply
        struct payload reply;
        while ( protocol.GetReply(fd, reply) )
        {
            // cast the reply into a disc_data
            struct disc_data* info = (struct disc_data*) reply.data;
            byte slot = info->slot;
            byte userfiles = info->userfiles;
            enum genre genre = (enum genre) info->genre;

            // add null terminator to title
            byte data_len = sizeof(struct disc_data)-MAX_DISC_TITLE_LENGTH-1;
            byte title_len = reply.len-data_len;
            info->title[title_len+1] = '\0';

            // output the reply
            DebugMsg("slot: %3d unknown:%02x userfiles: %02X unknown: %02x genre:%s unknown:%02x title:%s\n",
                    slot, info->unknown1, userfiles, info->unknown3, 
                    GENRE_NAMES[genre], info->unknown4, info->title);
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


