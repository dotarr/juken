#ifndef __UNIXDOMAINSOCK_H__
#define __UNIXDOMAINSOCK_H__

#include <stdio.h>
#include "util.h"

class UnixDomainSock
{
    public:
        UnixDomainSock();
        virtual ~UnixDomainSock();

        int OpenSock(const char* path);
        int OpenServerSock(const char* path);
        void CloseSock();

        int GetFileDescriptor() const { return fd; }

    protected:
        int fd;
        const char* p;

    private:
        UnixDomainSock(const UnixDomainSock&);
};

#endif /* __UNIXDOMAINSOCK_H__ */
