#include <stdarg.h>
#include <stdio.h>
#include "common.h"
#include "log.h"

FILE* g_log_file = NULL;
int g_level = 1;

void
OpenLog(const char* filename, int level) 
{
    if ( level > 0 )
        g_log_file = ::fopen(filename, "w");
    g_level = level;
}

void
CloseLog()
{
    if ( g_log_file != NULL )
        ::fclose(g_log_file);
    g_log_file = NULL;
}

void
LogMsg(const char* fmt, ...)
{
    if ( g_log_file==NULL || g_level<1 )
        return;

    va_list ap;
    va_start(ap, fmt);
    ::vfprintf(g_log_file, fmt, ap);
    va_end(ap);
    ::fflush(g_log_file);
}

void
InfoMsg(const char* fmt, ...)
{
    if ( g_log_file==NULL || g_level<2 )
        return;

    va_list ap;
    va_start(ap, fmt);
    ::vfprintf(g_log_file, fmt, ap);
    va_end(ap);
    ::fflush(g_log_file);
}

void
DebugMsg(const char* fmt, ...)
{
    if ( g_log_file==NULL || g_level<3 )
        return;

    va_list ap;
    va_start(ap, fmt);
    ::vfprintf(g_log_file, fmt, ap);
    va_end(ap);
    ::fflush(g_log_file);
}

void
DebugPayload(const char* label, byte cmd, ushort len, const byte* data)
{
    if ( g_log_file==NULL || g_level<3 )
        return;

    print_payload(g_log_file, label, cmd, len, data);
}

void
TraceMsg(const char* fmt, ...)
{
    if ( g_log_file==NULL || g_level<4 )
        return;

    va_list ap;
    va_start(ap, fmt);
    ::vfprintf(g_log_file, fmt, ap);
    va_end(ap);
    ::fflush(g_log_file);
}

void 
print_payload(FILE* file, const char* label, byte cmd, ushort len, const byte* data)
{
    ::fprintf(file, "%s cmd=%d len=%d\n", label, cmd, len);
    printdata(file, data, len);
    ::fflush(g_log_file);
}


