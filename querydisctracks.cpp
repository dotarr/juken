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

        protocol.IssueQueryDevice(fd, 0x00, 1, 33, 0x00, 0x01, 0x00);

        struct payload reply;
        while ( protocol.GetReply(fd, reply) )
        {
            struct track_data* info = (struct track_data*) reply.data;
            int index = info->index;

            byte data_len = sizeof(struct track_data)-MAX_TRACK_TITLE_LENGTH-1;
            byte title_len = 0;
            if ( info->title[0] != 0x01 )
                title_len = reply.len-data_len;
            info->title[title_len] = '\0';

            DebugMsg("unknown: %02x %02x index:%d unknown: %02x %02x %02x %02x title:%s\n",
                     info->unknown1, info->unknown2, index, info->unknown3, 
                     info->unknown4, info->unknown5, info->unknown6, info->title);
        }
    }
    catch (char* e)
    {
        ::fprintf(stderr, "%s\n", e);
        return 0;
    }
    return 1;
}


