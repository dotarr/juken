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

        protocol.IssueQueryDevice(fd, 0x00, 1, 0, 0x00, 0x07, 0x00);

        struct payload reply;
        while ( protocol.GetReply(fd, reply) )
        {
            struct userfile_data* info = (struct userfile_data*) reply.data;
            byte mask = info->mask;

            byte data_len = sizeof(struct userfile_data)-MAX_USER_TITLE_LENGTH-1;
            byte title_len = reply.len-data_len;
            info->title[title_len] = '\0';

            DebugMsg("mask:%02X unknown:%02X %02X %02X %02X %02X %02X title:%s\n",
                     mask, info->unknown1, info->unknown2, info->unknown3, 
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


