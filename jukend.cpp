#include <sys/time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <asm/ioctls.h>
#include <errno.h>

#include "serialdevice.h"
#include "unixdomainsock.h"
#include "jukebox.h"

char* serial_device = "/dev/ttyS0";
char* messaging_socket = "/tmp/juken";

void
ProcessTraffic(Jukebox& protocol, int juke_fd, int sock_fd)
{
    bool done = false;

    int num_conns = 0;
    int conn_fds[64];

    fd_set rfds;
    fd_set efds;
    
    while ( !done )
    {
        FD_ZERO(&rfds);

        FD_SET(juke_fd, &rfds);
        FD_SET(sock_fd, &rfds);

        int max_fd = (juke_fd>sock_fd) ? juke_fd : sock_fd;

        for (int i=0; i<num_conns; i++)
        {
            int fd = conn_fds[i];
            FD_SET(fd, &rfds);
            if ( fd > max_fd ) max_fd = fd;
        }

        int num_fds = ::select(max_fd+1, &rfds, NULL, NULL, NULL);
        ThrowIfMinus1(num_fds, "select failed: ");

        if ( num_fds > 0 )
        {
            if ( FD_ISSET(juke_fd, &rfds) )
            {
                //printf("incoming message detected\n");
                protocol.ProcessIncomingMessage(juke_fd);
            }
            for (int i=0; i<num_conns; i++)
            {
                int fd = conn_fds[i];
                if ( FD_ISSET(fd, &rfds) )
                {
                    int count;
                    ThrowIfMinus1(::ioctl(fd, FIONREAD, &count), "ioctl failed: ");
                    if ( count > 0 )
                    {
                        //printf("outgoing message detected\n");
                        protocol.ProcessOutgoingMessage(juke_fd, fd);
                    }
                    else
                    {
                        //printf("client connection closed\n");
                        num_conns--;
                        ::memmove(&conn_fds[i], &conn_fds[i+1], num_conns-i);
                        i--;
                        FD_CLR(fd, &rfds);
                    }
                }
            }
            if ( FD_ISSET(sock_fd, &rfds) )
            {
                //printf("client connection established\n");
                struct sockaddr_un addr;
                socklen_t addr_len = sizeof(addr);
                int fd = ::accept(sock_fd, (struct sockaddr*) &addr, &addr_len);
                ThrowIfMinus1(fd, "accept failed: ");
                conn_fds[num_conns++] = fd;
            }
        }
    }
}

void
parse_args(int argc, char* argv[])
{
    if ( argc > 1 ) serial_device = argv[1];
    if ( argc > 2 ) messaging_socket = argv[2];
}

int
main (int argc, char* argv[])
{
    parse_args(argc, argv);

    pid_t pid = -1;
    try
    {
        SerialDevice device;
        UnixDomainSock ssock;

        int juke_fd = device.OpenDevice(serial_device);
        int sock_fd = ssock.OpenServerSock(messaging_socket);

        Jukebox protocol;

        ProcessTraffic(protocol, juke_fd, sock_fd);
    }
    catch (char* e)
    {
        ::fprintf(stderr, "exception caught for pid=%d!!!\n%s\n", pid, e);
        return 0;
    }
    return 1;
}


