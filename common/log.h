#ifndef COMMON_LOG_H
#define COMMON_LOG_H

void OpenLog(const char* filename, int level=1);
void CloseLog();

// level 0 
void LogMsg(const char* fmt, ...);

// level 1 (default)
void InfoMsg(const char* fmt, ...);

// level 2 
void DebugMsg(const char* fmt, ...);
void DebugPayload(const char* label, byte cmd, ushort len, const byte* data);

// level 3 
void TraceMsg(const char* fmt, ...);

void print_payload(FILE* file, const char* label, byte cmd, ushort len, const byte* data);

#endif /* COMMON_LOG_H */
