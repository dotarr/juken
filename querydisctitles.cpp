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

        protocol.IssueQueryDevice(fd, 0x00, 1, 0, 0x00, 0x00, 0x00);

        struct payload reply;
        while ( protocol.GetReply(fd, reply) )
        {
            struct disc_data* info = (struct disc_data*) reply.data;
            byte slot = info->slot;
            byte userfiles = info->userfiles;
            enum genre genre = (enum genre) info->genre;

            byte data_len = sizeof(struct disc_data)-MAX_DISC_TITLE_LENGTH-1;
            byte title_len = reply.len-data_len;
            info->title[title_len+1] = '\0';

            DebugMsg("slot: %3d unknown:%02x userfiles: %02X unknown: %02x genre:%s unknown:%02x title:%s\n",
                    slot, info->unknown1, userfiles, info->unknown3, 
                    GENRE_NAMES[genre], info->unknown4, info->title);
        }
    }
    catch (char* e)
    {
        ::fprintf(stderr, "%s\n", e);
        return 0;
    }
    return 1;
}


