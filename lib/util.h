#ifndef JUKEN_UTIL_H
#define JUKEN_UTIL_H

#include <common.h>

#include "types.h"
#include "payload.h"

void DebugMsg(const char* fmt, ...);
void DebugConn(const char* fmt, ...);
void DebugPayload(const char* label, const payload& msg, const byte cksum);
void TraceFlow(const char* fmt, ...);

#endif /* JUKEN_UTIL_H */
