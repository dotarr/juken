#include "unixdomainsock.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

// A class for communicating over a unix domain socket

UnixDomainSock::UnixDomainSock()
{
    // initialize the members
    fd = -1;
    p = NULL;
}

UnixDomainSock::~UnixDomainSock()
{
    // close the device
    CloseSock();
}

int
UnixDomainSock::OpenSock(const char* path)
{
    // open a socket
    fd = ::socket(PF_LOCAL, SOCK_STREAM, 0);
    ThrowIfMinus1(fd, "unable to open socket: ");

    // prepare an address
    struct sockaddr_un addr;
    ::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    ::strcpy(addr.sun_path, path);

    // connect to the path
    ThrowIfMinus1(::connect(fd, (struct sockaddr*) &addr, sizeof(addr)),
                  "unable to connect to %s: ", path);

    return fd;
} 

int
UnixDomainSock::OpenServerSock(const char* path)
{
    // open a scoket
    fd = ::socket(PF_LOCAL, SOCK_STREAM, 0);
    ThrowIfMinus1(fd, "unable to open socket: ");

    // prepare an address
    struct sockaddr_un addr;
    ::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    ::strcpy(addr.sun_path, path);

    // bind to the path
    ThrowIfMinus1(::bind(fd, (struct sockaddr*) &addr, sizeof(addr)),
                  "unable to bind to %s: ", path);
    p = path;

    // listen for connections
    ThrowIfMinus1(::listen(fd, 4), "unable to listen on %d: ", fd);

    return fd;
}

void
UnixDomainSock::CloseSock()
{
    // close the fd
    if ( fd != -1 )
        ::close(fd);
    fd = -1;
    // unlink the path
    if ( p != NULL )
        ::unlink(p);
    p = NULL;
}

