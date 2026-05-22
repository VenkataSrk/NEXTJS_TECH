#include <sys/time.h>
#include <dlfcn.h>
#include <stdint.h>

#define DNS_QUERY_STANDARD 0
#define DnsFreeRecordList 0
#define USYNC_THREAD  1
#define THR_NEW_LWP 0
#define THR_JOINABLE 0
#define THREAD_PRIORITY_ERROR_RETURN  255
typedef unsigned long UINT32;


//#define stdcall __attribute__((stdcall))
#define __stdcall __attribute__((stdcall))

//typedef int64_t LONGLONG;

/* unsigned long GetTickCount()
{
  struct timeval tv;
  if( gettimeofday(&tv, NULL) != 0 )
  return 0;

  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
} */
