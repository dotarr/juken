#ifndef __UTIL_H__
#define __UTIL_H__

#include "types.h"

void DebugMsg(const char* fmt, ...);
void DebugConn(const char* fmt, ...);
void DebugPayload(const char* label, const byte cmd, const struct payload& msg, const byte cksum);
void TraceFlow(const char* fmt, ...);

void ThrowIf(bool cond, const char* fmt, ...);
void ThrowIfMinus1(int val, const char* fmt, ...);
void ThrowIfNeg(int val, const char* fmt, ...);
void ThrowIfNull(const void* p, const char* fmt, ...);
void Throw(const char* fmt, ...);


#endif /* __UTIL_H__ */
