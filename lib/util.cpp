#include <stdarg.h>

#include "util.h"

#define DUMP_MSGS
//#define DUMP_CONN
#define DUMP_PAYLOAD
#define TRACE_FLOW

void
DebugMsg(const char* fmt, ...)
{
#ifdef DUMP_MSGS
    va_list ap;
    va_start(ap, fmt);
    ::vfprintf(stdout, fmt, ap);
    va_end(ap);
#endif
}

void
DebugConn(const char* fmt, ...)
{
#ifdef DUMP_CONN
    va_list ap;
    va_start(ap, fmt);
    ::vfprintf(stdout, fmt, ap);
    va_end(ap);
#endif
}

void
DebugPayload(const char* label, const payload& msg, const byte cksum)
{
#ifdef DUMP_PAYLOAD
    ::fprintf(stdout, "%s cmd=%d len=%d\n", label, msg.cmd, msg.len);
    printdata(msg.data, msg.len);
    ::fprintf(stdout, "cksum=0x%02X\n", cksum);
#endif
}

void
TraceFlow(const char* fmt, ...)
{
#ifdef TRACE_FLOW
    va_list ap;
    va_start(ap, fmt);
    ::vfprintf(stdout, fmt, ap);
    va_end(ap);
#endif
}


