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

        protocol.IssueChangeState(fd, PLAY_PAUSE_PARAM);
    }
    catch (char* e)
    {
        ::fprintf(stderr, "%s\n", e);
        return 0;
    }
    return 1;
}


