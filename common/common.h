#ifndef COMMON_COMMON_H_
#define COMMON_COMMON_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>

#if STDC_HEADERS
#include <stdlib.h>
#include <string.h>
#elif HAVE_STRINGS_H
#include <strings.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_ERRNO_H
#include <errno.h>
#endif
#ifndef errno
extern int errno;
#endif

typedef unsigned char  byte;

void ThrowIf(bool cond, const char* fmt, ...);
void ThrowIfMinus1(int val, const char* fmt, ...);
void ThrowIfNeg(int val, const char* fmt, ...);
void ThrowIfNull(const void* p, const char* fmt, ...);
void Throw(const char* fmt, ...);

void printdata(const byte data[], int count);

#endif /* COMMON_COMMON_H */
