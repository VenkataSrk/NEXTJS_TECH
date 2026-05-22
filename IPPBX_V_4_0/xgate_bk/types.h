/* $Header: /xGate/types.h 11    2/20/04 11:52a Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * BASIC XGATE TYPES, MACROS, AND TOOL FUNCTIONS.
 *
 * $Log: /xGate/types.h $
 * 
 * 11    2/20/04 11:52a Bennylp
 * 
 * 10    7/13/01 3:10p Bennylp
 * Include weeks/days in ConvertInterval2String.
 * 
 * 9     4/28/01 6:33a Bennylp
 * Added service profiling and database driver (testing)
 */
#ifndef __XGATE_TYPES_H__
#define __XGATE_TYPES_H__

#include <stdio.h>
#include <string.h>
#include <sqltypes.h>
//#include "winnt.h"	//linux porting

#define WIN32_IGNORE 0
#define INFINITE 0xffffffff

#define THREAD_PRIORITY_IDLE	1
#define THREAD_PRIORITY_LOWEST	2
#define THREAD_PRIORITY_BELOW_NORMAL	3
#define THREAD_PRIORITY_NORMAL	4
#define THREAD_PRIORITY_ABOVE_NORMAL	5
#define THREAD_PRIORITY_HIGHEST	6
#define THREAD_PRIORITY_TIME_CRITICAL	15

/** STATUS: for compatibility with old CTENGINE.
    New XGATE methods should use bool to indicate status. For compatibility
    and to avoid bugs, the value of the enums have been changed however.
 */

enum STATUS{
    NOT_OK = false,
    OK = true
};


// strncpy replacement
inline void safe_strcpy( char *dst, const char *src, size_t len)
{
    if (len == 0) return;

    unsigned srclen = strlen(src)+1; //what if the src not null terminated, can we have the strlen ?
    if (srclen<len) len = srclen;
    strncpy( dst, src, len-1);
    dst[len-1] = '\0';
};

//strncpy replacement alternatives :
inline size_t strlcpy(char *dst, const char *src, size_t siz)
{
		char *d = dst;
		const char *s = src;
		size_t n = siz;

		// Copy as many bytes as will fit
		if (n != 0) {
			while (--n != 0) {
				if ((*d++ = *s++) == '\0')
					break;
			}
		}

		// Not enough room in dst, add NUL and traverse rest of src 
		if (n == 0) {
			if (siz != 0)
				*d = '\0';		// NUL-terminate dst
			while (*s++)
				;
		}

		return(s - src - 1);	// count does not include NUL
}

// min function.
#undef min
inline int min(int a, int b)
{
    if (a<b) return a;
    return b;
}


// application's max. string length
#define MAXAPPSTRLEN		1023


// application's boolean value
#define APPTRUE	    0
#define APPFALSE    -1


// pair of id-name
struct IdNamePair {
    int Id;
    const char *Name;
};
#define IDNAMEPAIR(id)	{ id, #id }


/** Macro to allocate an instance with new, but returns NULL and write to
    log when the allocation failed.
 */
#define NEW_RETURN_(var,exp)	try { \
				    var = new exp; \
				} catch (...) { \
				    var = NULL; \
				    CLog::Fatal ( "sys", "System memory allocation failed in %s:%u: %s", \
						  __FILE__, __LINE__, "new " #exp); \
				}

 /** Include basic SIP types such as String, List, etc.
 */
#include "../slsip/include/siptypes.h" //must include this for sake of IString,List, String,Vector



/** Convert seconds interval to formatted string (which includes days, hours, etc).
 */
inline String ConvertInterval2String ( unsigned interval )
{
    enum {
	One_Minute = 60,
	One_Hour = 60 * One_Minute,
	One_Day = 24 * One_Hour,
	One_Week = 7 * One_Day
    };

    String output;
    char temp[40];

    // Include weeks.
    if (interval > One_Week) {
	sprintf(temp, "%uw:", interval / One_Week);
	output += temp;
	interval %= One_Week;
    }
    
    // Include days.
    if (interval > One_Day) {
	sprintf(temp, "%ud:", interval / One_Day);
	output += temp;
	interval %= One_Day;
    }

    sprintf (temp, "%.2uh:%.2um:%.2us",
		    interval/One_Hour, 
		    (interval % One_Hour)/One_Minute, 
		    interval % One_Minute);
    output += temp;
    return output;
}

//The below line added for porting linux
//typedef unsigned long LONG;
typedef void *           HANDLE;
//typedef unsigned long    DWORD;
typedef unsigned char    BYTE; 
typedef unsigned int     UINT; 
//typedef unsigned char    WCHAR;
typedef unsigned char *PBYTE;
typedef BYTE *LPBYTE;
typedef time_t SYSTEMTIME;
//typedef unsigned long LARGE_INTEGER;
//typedef unsigned long LONG;
//typedef unsigned char * LPWSTR;
typedef long WLONG;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef char CHAR;
//typedef bool BOOL;

//#ifdef __i386__

typedef long LONG;
typedef unsigned int DWORD;
//typedef unsigned long DWORD; //bcoz long is also 4 bytes  changed according to sqltypes.h
typedef unsigned short WCHAR;
typedef WCHAR *LPWSTR;
typedef double LONGLONG;

//#endif

typedef struct _PERF_DATA_BLOCK {
  WCHAR         Signature[4];
  DWORD         LittleEndian;
  DWORD         Version;
  DWORD         Revision;
  DWORD         TotalByteLength;
  DWORD         HeaderLength;
  DWORD         NumObjectTypes;
  DWORD         DefaultObject;
  SYSTEMTIME    SystemTime;
  unsigned long PerfTime;
  unsigned long PerfFreq;
  unsigned long PerfTime100nSec;
  DWORD         SystemNameLength;
  DWORD         SystemNameOffset;
} PERF_DATA_BLOCK;

typedef struct _PERF_INSTANCE_DEFINITION {
  DWORD ByteLength;
  DWORD ParentObjectTitleIndex;
  DWORD ParentObjectInstance;
  DWORD UniqueID;
  DWORD NameOffset;
  DWORD NameLength;
} PERF_INSTANCE_DEFINITION;

typedef struct _PERF_COUNTER_DEFINITION {
  DWORD  ByteLength;
  DWORD  CounterNameTitleIndex;
  LPWSTR CounterNameTitle;
  DWORD  CounterHelpTitleIndex;
  LPWSTR CounterHelpTitle;
  DWORD   DefaultScale;
  DWORD  DetailLevel;
  DWORD  CounterType;
  DWORD  CounterSize;
  DWORD  CounterOffset;
} PERF_COUNTER_DEFINITION;

typedef struct _PERF_OBJECT_TYPE {
  DWORD         TotalByteLength;
  DWORD         DefinitionLength;
  DWORD         HeaderLength;
  DWORD         ObjectNameTitleIndex;
  LPWSTR        ObjectNameTitle;
  DWORD         ObjectHelpTitleIndex;
  LPWSTR        ObjectHelpTitle;
  DWORD         DetailLevel;
  DWORD         NumCounters;
  DWORD         DefaultCounter;
  DWORD         NumInstances;
  DWORD         CodePage;
  unsigned long PerfTime;
  unsigned long PerfFreq;
} PERF_OBJECT_TYPE;

typedef struct _PERF_COUNTER_BLOCK {
  DWORD ByteLength;
} PERF_COUNTER_BLOCK;

#define IDLE_PRIORITY_CLASS 0
#define NORMAL_PRIORITY_CLASS 0
#define HIGH_PRIORITY_CLASS 0
#define REALTIME_PRIORITY_CLASS 0



#endif	/* __XGATE_TYPES_H__ */

