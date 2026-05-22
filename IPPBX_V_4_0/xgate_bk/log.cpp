#include <log.h>
//static void Log( LogVerbosity level, const char *sender, const char *format, ...);
//static void Log( LogVerbosity level, const char *sender, CTask *, const char *format, ...);
void CLog::Detail( const char *sender, const char *format, ...){}
//static void Detail( const char *sender, CTask *, const char *format, ...);

void CLog::Debug( const char *sender, const char *format, ...){}
//static void Debug( const char *sender, CTask *, const char *format, ...);

void CLog::Info( const char *sender, const char *format, ...){}
//static void Info( const char *sender, CTask *, const char *format, ...);
//static void InfoApp( const char *sender, CTask *, const char *format, ...);

void CLog::Warning( const char *sender, const char *format, ...){}
//static void Warning( const char *sender, CTask *, const char *format, ...);
//static void WarningApp( const char *sender, CTask *, const char *format, ...);

void CLog::Error( const char *sender, const char *format, ...){}
//static void Error( const char *sender, CTask *, const char *format, ...);
//static void ErrorApp( const char *sender, CTask *, const char *format, ...);

void CLog::Fatal( const char *sender, const char *format, ...){}
//static void Fatal( const char *sender, CTask *, const char *format, ...);

void CLog::SysLastError( const char *sender, const char *msgfmt, ...){}
//static void SysLastError( const char *sender, CTask *, const char *msgfmt, ...);

