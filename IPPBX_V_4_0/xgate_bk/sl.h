/* $Header: /ct15/sl.h 1     3/22/01 4:09p Bennylp $ */
//
// SL.H
// Put in namespace sl:
//  - general purpose codes
//  - system dependent codes
//

#ifndef __SL_H__
#define __SL_H__

#include "types.h"
#define __linux__
#ifndef PTHREAD
#define PTHREAD 
#include <pthread.h>
#endif
#include <sys/socket.h>
#include <netdb.h>
typedef int INTHANDLE;

#define HANDLE INTHANDLE



namespace sl {

    //-------------------------------------------------------------------------
    // SYSTEM DEPENDENT

    // Sleep for ms miliseconds
    void Sleep( unsigned long ms );


    //-------------------------------------------------------------------------
    // FILE OPERATIONS

    // Copy file
    STATUS FileCopy( const char *src, const char *dest, bool overwrite=false);

    // Is the path full path?
    bool   IsFullPath( const char *path );

    // Check if source_name is full path, prepend dir if it isn't
    STATUS MakeFullPath( std::string *tgt, const char *dir, const char *source_name );


    //-------------------------------------------------------------------------
    // IPC

    // Pipe
    class CPipe {
    public:
	CPipe();
	~CPipe();

	STATUS	Init( size_t buf_size=1024);
	STATUS	Close();
	STATUS	Write( const void *buf, size_t len, size_t *written=0 );
	STATUS	Read( void *buf, size_t len, size_t *rdlen=0);

    private:
	HANDLE	m_hRead, m_hWrite;
    };


    // Mutex
    class CMutex {
    public:
	enum {
	    infinite = INFINITE
	};

	CMutex(bool initial_locked=false);
	~CMutex();

	STATUS	Lock(unsigned long time_out = (unsigned long)infinite);
	STATUS	Unlock();

    private:
	pthread_mutex_t m_hMutex;
    };


    //-------------------------------------------------------------------------
    // THREADS, PROCESSES

    // Thread priority constants
    enum EThreadPriority {
	THREAD_PRIO_LOWEST = THREAD_PRIORITY_LOWEST,
	THREAD_PRIO_IDLE = THREAD_PRIORITY_IDLE,
	THREAD_PRIO_BELOW_NORMAL = THREAD_PRIORITY_BELOW_NORMAL,
	THREAD_PRIO_NORMAL = THREAD_PRIORITY_NORMAL,
	THREAD_PRIO_ABOVE_NORMAL = THREAD_PRIORITY_ABOVE_NORMAL,
	THREAD_PRIO_HIGHEST = THREAD_PRIORITY_HIGHEST,
	THREAD_PRIO_CRITICAL = THREAD_PRIORITY_TIME_CRITICAL,
    };

    // Process priority class
    enum EPriorityClass {
	PRIO_IDLE = IDLE_PRIORITY_CLASS,
	//PRIO_BELOW_NORMAL = BELOW_NORMAL_PRIORITY_CLASS,
	PRIO_NORMAL = NORMAL_PRIORITY_CLASS,
	//PRIO_ABOVE_NORMAL = ABOVE_NORMAL_PRIORITY_CLASS,
	PRIO_HIGH = HIGH_PRIORITY_CLASS,
	PRIO_REALTIME = REALTIME_PRIORITY_CLASS,
    };


    // Get and set thread priority
    STATUS SetPriorityClass( EPriorityClass cls);
    STATUS SetThreadPriority( EThreadPriority prio);

    EPriorityClass  GetPriorityClass();
    EThreadPriority GetThreadPriority();


    // Thread
    class CThread {
    public:
	CThread();
	~CThread();

	STATUS Start( void *param=0 );
	bool   IsRunning();
	unsigned long GetThreadId() { return m_Id; };
	STATUS Kill( int exit_code=-1 );
	STATUS Wait( int ms_timeout=INFINITE );
	STATUS SetThreadPriority( EThreadPriority prio );
//	EThreadPriority GetThreadPriority();
	int GetThreadPriority();  //__linux__

	virtual int ThreadProc(void *param) = 0;

    private:

#ifndef __linux__
	HANDLE m_hThread;
	unsigned  m_Id;

#else
	pthread_t m_hThread;

#endif
	//LPVOID m_Param;
	void * m_Param;
	unsigned long m_Id;


	//static DWORD WINAPI _ThreadProc(LPVOID);
	static void * _ThreadProc(void *); //__linux__ 
    };


    //-------------------------------------------------------------------------
    // SOCKET
#ifndef __linux__
    in_addr GetHostByName( const char *hostname );
#else
    struct hostent * GetHostByName( const char *hostname );

#endif

    //-------------------------------------------------------------------------
    // MISCELANY

    int stricmp( const char *lhs, const char *rhs );

    // prompt message to current user
    int Prompt( const char *title, const char *message );

    char * strltrim ( char * s );
    char * strrtrim ( char * s );
}

#endif
