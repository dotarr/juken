#include "unixdomainsock.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>


UnixDomainSock::UnixDomainSock()
{
    fd = -1;
    p = NULL;
}

UnixDomainSock::~UnixDomainSock()
{
    CloseSock();
}

int
UnixDomainSock::OpenSock(const char* path)
{
    fd = ::socket(PF_LOCAL, SOCK_STREAM, 0);
    ThrowIfMinus1(fd, "unable to open socket: ");

    struct sockaddr_un addr;
    ::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    ::strcpy(addr.sun_path, path);

    ThrowIfMinus1(::connect(fd, (struct sockaddr*) &addr, sizeof(addr)),
                  "unable to connect to %s: ", path);

    return fd;
} 

int
UnixDomainSock::OpenServerSock(const char* path)
{
    fd = ::socket(PF_LOCAL, SOCK_STREAM, 0);
    ThrowIfMinus1(fd, "unable to open socket: ");

    struct sockaddr_un addr;
    ::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    ::strcpy(addr.sun_path, path);

    ThrowIfMinus1(::bind(fd, (struct sockaddr*) &addr, sizeof(addr)),
                  "unable to bind to %s: ", path);
    p = path;

    ThrowIfMinus1(::listen(fd, 4), "unable to listen on %d: ", fd);

    return fd;
}

void
UnixDomainSock::CloseSock()
{
    if ( fd != -1 )
    {
        ::close(fd);
    }
    fd = -1;
    if ( p != NULL )
    {
        ::unlink(p);
    }
    p = NULL;
}

