#include <stdlib.h>
#include <signal.h>
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

bool done = false;

void
INThandler(int sig)
{
    // specify to ignore the signal ...
    //signal(sig, SIG_IGN);

    done = true;

    ::fprintf(stderr, "ctrl-c hit\n");
}

void
ProcessTraffic(Jukebox& protocol, int juke_fd, int sock_fd)
{

    int num_conns = 0;
    int conn_fds[64];

    fd_set rfds;
    fd_set efds;
    
    // repeat until ...
    while ( !done )
    {
        // setup fd set's
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

        // select for something to do
        int num_fds = ::select(max_fd+1, &rfds, NULL, NULL, NULL);
        if ( num_fds == -1 )
            done = true;

        // if we have something to do ...
        if ( num_fds > 0 )
        {
            if ( FD_ISSET(juke_fd, &rfds) )
            {
                // the player has a message for us
                DebugConn("incoming message detected\n");
                protocol.ProcessIncomingMessage(juke_fd);
            }
            for (int i=0; i<num_conns; i++)
            {
                // handle messages from the tools
                int fd = conn_fds[i];
                if ( FD_ISSET(fd, &rfds) )
                {
                    // detect tool connection closure
                    int count;
                    ThrowIfMinus1(::ioctl(fd, FIONREAD, &count), "ioctl failed: ");
                    if ( count > 0 )
                    {
                        // the tool has a message to send to the player
                        DebugConn("outgoing message detected\n");
                        protocol.ProcessOutgoingMessage(juke_fd, fd);
                    }
                    else
                    {
                        // the tool has closed the connection
                        DebugConn("tool connection closed\n");
                        // remove the connection fd from our array
                        num_conns--;
                        ::memmove(&conn_fds[i], &conn_fds[i+1], num_conns-i);
                        i--;
                        FD_CLR(fd, &rfds);
                    }
                }
            }
            if ( FD_ISSET(sock_fd, &rfds) )
            {
                // a tool has opened a connection
                DebugConn("tool connection established\n");
                // accept the new connection
                struct sockaddr_un addr;
                socklen_t addr_len = sizeof(addr);
                int fd = ::accept(sock_fd, (struct sockaddr*) &addr, &addr_len);
                ThrowIfMinus1(fd, "accept failed: ");
                // add the connection to our array
                conn_fds[num_conns++] = fd;
            }
        }
    }
}

void
parse_env()
{
    char* dev = getenv("JUKEN_DEV");
    if ( dev != NULL )  serial_device = dev;

    char* sock = getenv("JUKEN_SOCK");
    if ( sock != NULL ) messaging_socket = sock;
}

void
parse_args(int argc, char* argv[])
{
    // no arguments yet ...
}

int
main (int argc, char* argv[])
{
    // install a ctrl-c signal handler
    signal(SIGINT, INThandler);

    // parse parameters
    parse_env();
    parse_args(argc, argv);

    ::fprintf(stderr, "serial_device=%s\n", serial_device);
    ::fprintf(stderr, "messaging_socket=%s\n", messaging_socket);

    try
    {
        // create communication devices
        SerialDevice device;
        UnixDomainSock ssock;

        // open communication devices
        int juke_fd = device.OpenDevice(serial_device);
        int sock_fd = ssock.OpenServerSock(messaging_socket);

        // create the protocol object
        Jukebox protocol;

        // process traffic on communication devices
        ProcessTraffic(protocol, juke_fd, sock_fd);
    }
    catch (char* e)
    {
        // output the error
        ::fprintf(stderr, "exception caught!!!\n%s\n", e);
        
        // exit with failure
        return EXIT_FAILURE;
    }
    
    // return with success
    return EXIT_SUCCESS;
}


