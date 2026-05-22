/* $Header: /xGate/sl.cpp 5     10/22/02 5:14p Bennylp $ */
//
// SL.H
// Put in namespace SL:
//  - general purpose codes
//  - system dependent codes
//

#include "stdafx.h"
#include "sl.h"
#include "thread_process.h"
#include <iostream>
#include <fstream>
#include <poll.h>
//#include <pthread.h>
#include <signal.h>
#include <errno.h>
#define __linux__

#ifdef __linux__
#include <cstdlib>
#define INVALID_HANDLE_VALUE -1
typedef void * LPVOID;
#endif

using namespace std;

namespace sl {

//-------------------------------------------------------------------------
// SYSTEM DEPENDENT
//-------------------------------------------------------------------------

void Sleep( unsigned long ms )
{
#ifndef __linux__
	::Sleep(ms);
#else
	poll(0,0,ms); // __linux__
#endif
};

//-------------------------------------------------------------------------
// FILE OPERATION
//-------------------------------------------------------------------------

//-----------------------------------------------------------------------------
STATUS FileCopy( const char *src, const char *dest, bool overwrite)
{
#ifndef __linux__
    return CopyFile( src, dest, !overwrite)? OK : NOT_OK; 
#else
  //  __linux__
 /* -- shell implementation --
     if((overwrite) || strcmp(src,dest)){
     char buff[256];
     sprintf(buff,"cp %s %s",src,dest);
     system(buff);
     return OK;
     }else{
	return NOT_OK;
     }	*/
  // c++ Fstream implementation
     ifstream src_stream(src);
	if(overwrite == true){
		ofstream dest_stream(dest,ios::trunc);
		if(dest_stream.is_open()){
			dest_stream << src_stream.rdbuf();
		}
	}
	
	 

#endif	
}

//-----------------------------------------------------------------------------
bool   IsFullPath( const char *path )
{
#ifndef __linux__
    if (!path || !*path) {
	return false;
    } else if (path[0] == '/' || path[0]=='\\' || path[1] == ':') {
	return true;
    };
    
    return false;
#else
    
	if (!path || !*path) {
		return false;
    } else if (path[0] == '/') {
		return true;
    };
    

#endif
};

//-----------------------------------------------------------------------------
STATUS MakeFullPath( std::string *tgt, const char *dir, const char *source_name )
{
    if (IsFullPath( source_name )) {
	
	*tgt = source_name;
    } else {
	
	*tgt = std::string(dir) + source_name;
    }
    
    return OK;
};


//-----------------------------------------------------------------------------
// IPC
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// CPipe constructor
//
CPipe::CPipe()
{
    m_hRead = m_hWrite = INVALID_HANDLE_VALUE;
}


//-----------------------------------------------------------------------------
// CPipe destructor: destroy the pipe if it is still alive
//
CPipe::~CPipe()
{
    Close();
}


//-----------------------------------------------------------------------------
// Close the pipe
//
STATUS CPipe::Close()
{
#if 0
    if (m_hRead != INVALID_HANDLE_VALUE) {
	::CloseHandle( m_hRead );
	m_hRead = INVALID_HANDLE_VALUE;
    }

    if (m_hWrite != INVALID_HANDLE_VALUE) {
	::CloseHandle( m_hWrite );
	m_hWrite = INVALID_HANDLE_VALUE;
    }

    return OK;
#else //__linux__

    if (m_hRead != INVALID_HANDLE_VALUE) {
	close(m_hRead);
	m_hRead = INVALID_HANDLE_VALUE;
    }

    if (m_hWrite != INVALID_HANDLE_VALUE) {
	close( m_hWrite );
	m_hWrite = INVALID_HANDLE_VALUE;
    }
    return OK;

#endif
}


//-----------------------------------------------------------------------------
// Create the pipe. Must be called before pipe can be read or written
//
STATUS CPipe::Init( size_t buf_size)
{
#if 0
    if (!::CreatePipe( &m_hRead, &m_hWrite, 0, buf_size)) {
	return NOT_OK;
    }
    return OK;
#else //__linux__
    int cpipe[2];
    if(pipe(cpipe) == -1){
	return NOT_OK;
    }else{
	m_hRead = cpipe[0];
	m_hWrite = cpipe[1];
	return OK;
	}


#endif
}


//-----------------------------------------------------------------------------
// Write data to the pipe
//
STATUS CPipe::Write( const void *buf, size_t len, size_t *written )
{
#if 0
    size_t nb_written;
    if (!written) written = &nb_written;

    return ::WriteFile( m_hWrite, buf, len, (DWORD*)written, 0) ? OK : NOT_OK;
#else //__linux__
	*written = write(m_hWrite,buf,len);
	if(*written == -1){
		return NOT_OK;
	}else{
		return OK;
	}


#endif
}


//-----------------------------------------------------------------------------
// Read data from the pipe
//
STATUS CPipe::Read( void *buf, size_t len, size_t *rdlen )
{

#if 0
    size_t nb_read;
    if (!rdlen) rdlen = &nb_read;

    return ::ReadFile( m_hRead, buf, len, (DWORD*)rdlen, 0) ? OK : NOT_OK;
#else //__linux__
	*rdlen = read(m_hRead,buf,len);
	if(*rdlen == -1){
		return NOT_OK;
	}else{
		return OK;
	}
	
#endif
}


//-------------------------------------------------------------------------
// THREADS, PROCESSES
//-------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Set current process' priority class
//
STATUS SetPriorityClass( EPriorityClass cls)
{
#if 0
    return ::SetPriorityClass( GetCurrentProcess(), (DWORD)cls) ? OK : NOT_OK;
#else //__linux__

#endif 
}


//-----------------------------------------------------------------------------
// Set current thread's priority
//
STATUS SetThreadPriority( EThreadPriority prio)
{
#if 0
    return ::SetThreadPriority( GetCurrentThread(), (int)prio) ? OK : NOT_OK;
#else //__linux__

#endif
}


//-----------------------------------------------------------------------------
// Set current process's priority class
//
EPriorityClass  GetPriorityClass()
{
#if 0
    return (EPriorityClass) ::GetPriorityClass( GetCurrentProcess());
#else //__linux__
#endif
}


//-----------------------------------------------------------------------------
// Set current thread's priority
//
EThreadPriority GetThreadPriority()
{
#if 0
    return (EThreadPriority) ::GetThreadPriority(GetCurrentThread());
#else //__linux__

#endif
}

//-----------------------------------------------------------------------------
// CThread constructor
//
CThread::CThread()
{
#ifdef __linux__
    m_hThread = INVALID_HANDLE_VALUE;
 //   m_Id = -1;
#else

	cout << "CThread constructor" << endl;
	// m_hThread is not initialized in linux as it is an opaque object
#endif
};


//-----------------------------------------------------------------------------
// CThread destructor
//
CThread::~CThread()
{
#if 0
    CloseHandle( m_hThread );
    m_hThread = INVALID_HANDLE_VALUE;
#else // __linux__
#endif
};


//-----------------------------------------------------------------------------
// Start the execution of the thread
//
STATUS CThread::Start( void *param)
{
#ifndef __linux__
    m_Param = param;
    m_hThread = CREATE_THREAD( 0, 0, &_ThreadProc, (LPVOID)this, CREATE_SUSPENDED, &m_Id);
    if (!m_hThread) {
	return NOT_OK;
    };

    ::ResumeThread( m_hThread );

    return OK;
#else // __linux__
	m_Param = param;
	int ret;
	ret = pthread_create(&m_hThread,(pthread_attr_t *)param,&_ThreadProc,(void *) this);
	if(ret != 0){
		cout << "Unable to create thread" << endl;
//		exit(1);
		return NOT_OK;
	}
    // This is for testing added  - rkv
    if(ret)                             /* could not create thread */
    {
        printf("\n ERROR: return code from pthread_create is %d \n", ret);
        exit(1);
    }
    sleep(1);
    printf("\n Created new thread (%d) ... \n", ret);
    pthread_exit(NULL);	
    // This is for testing added  - rkv

	return OK;

#endif
};



//-----------------------------------------------------------------------------
// Determine whether the thread is running
//
bool CThread::IsRunning()
{
#ifndef __linux__
    if (m_hThread==INVALID_HANDLE_VALUE)
	return false;

    DWORD exitcode;
    if (::GetExitCodeThread(m_hThread, &exitcode) == FALSE || exitcode!=STILL_ACTIVE) {
	::CloseHandle (m_hThread);
	m_hThread = INVALID_HANDLE_VALUE;
	return false;
    }

    return true;
#else //__linux__
	
	if(pthread_kill(m_hThread,0) == ESRCH){ //This pthread_kill wont send any signal,but will return the error condition
		return false;
	}
	else 
		return true;


#endif
}

//-----------------------------------------------------------------------------
// Immediately stop the execution of the thread
//
STATUS CThread::Kill( int exit_code )
{

#if 0
    if (::TerminateThread( m_hThread, (DWORD) exit_code)) {
	CloseHandle (m_hThread );
	m_hThread = INVALID_HANDLE_VALUE;
	return OK;
    }

    return NOT_OK;
#else //__linux__
/*
	if(pthread_kill(m_hThread, exit_code) == 0){
		return OK;
	}else{
		return NOT_OK;
	}
*/
	if(pthread_cancel(m_hThread) == 0){
		return OK;
	}else{
		return NOT_OK;
	}

#endif
};


//-----------------------------------------------------------------------------
// Wait thread to stop. If specified timeout elapsed and the thread hasn't 
// been stopped yet, NOT_OK will be returned.
//
STATUS CThread::Wait( int ms_timeout )
{

#if 0
    if (m_hThread==INVALID_HANDLE_VALUE)
	return OK;

    DWORD rc = ::WaitForSingleObject(m_hThread, ms_timeout);
    if ( rc==WAIT_OBJECT_0 || rc==WAIT_FAILED) {
	return OK;
    }

    return NOT_OK;
#else //__linux__
	
	if(pthread_join(m_hThread,0) == 0){
		return OK;	
	}
	else {
		return NOT_OK;
		printf("The thread hasn'tbeen stopped yet, NOT_OK will be returned\n");	//rkv
	}

#endif
}


//-----------------------------------------------------------------------------
// Thread procedure
//
//DWORD WINAPI CThread::_ThreadProc( LPVOID param )
//static DWORD CThread::_ThreadProc( LPVOID param )
void * CThread::_ThreadProc( LPVOID param )
{
#if 0
    CThread *cthread_object = static_cast<CThread*>(param);
    DWORD ret = (DWORD) cthread_object->ThreadProc( cthread_object->m_Param );
    return ret;
#else //__linux__

#endif
}


//-----------------------------------------------------------------------------
// Get thread priority
//

#ifndef __linux__

EThreadPriority CThread::GetThreadPriority()
{

    return (EThreadPriority) ::GetThreadPriority( m_hThread );

}
#else 

int CThread::GetThreadPriority()
{

    struct sched_param param;
    int ret, policy = 0;
    ret = pthread_getschedparam(m_hThread, &policy, &param);
    if (ret != 0) {
    	cout << "Couldn't able to retrieve real-time scheduling paramers" << endl;
        return -1;
    }
//    std::cout << "Thread priority is " << params.sched_priority << std::endl;
    return param.sched_priority;


#endif
}


//-----------------------------------------------------------------------------
// Set thread priority
//
STATUS CThread::SetThreadPriority( EThreadPriority prio)
{
#if 0
    return ::SetThreadPriority( m_hThread, (int)prio ) ? OK : NOT_OK;
#else //__linux__
   struct sched_param params;
   int ret;
   params.sched_priority = prio;

   ret = pthread_setschedparam(m_hThread, 0, &params);
   if (ret != 0) {
 	   cout << "Unsuccessful in setting thread realtime prio" << endl;
       return NOT_OK;
   }
   return OK;

#endif
}


//-------------------------------------------------------------------------
// Mutex
//-------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Create mutex
//
CMutex::CMutex(bool initial_locked)
{
#if 0
    m_hMutex = ::CreateMutex( NULL, initial_locked, NULL);
    if (!m_hMutex)
	throw "Mutex initialization failed";
#else //__linux__
    m_hMutex = PTHREAD_MUTEX_INITIALIZER;

#endif
};


//-----------------------------------------------------------------------------
// Destroy mutex
//
CMutex::~CMutex()
{
#if 0
    ::CloseHandle( m_hMutex );
#else //__linux__

#endif
};


//-----------------------------------------------------------------------------
// Acquire lock
//
STATUS CMutex::Lock(unsigned long time_out)
{

#ifndef __linux__
    DWORD ret = ::WaitForSingleObject( m_hMutex, time_out );

    switch (ret) {
    case WAIT_ABANDONED:
    case WAIT_OBJECT_0:
	return OK;

    case WAIT_TIMEOUT:
    case WAIT_FAILED:
    default:
	return NOT_OK;
    };
#else

  if(!pthread_mutex_lock(&m_hMutex))
	return OK;
  else 
	return NOT_OK;

#endif
};


//-----------------------------------------------------------------------------
// Release lock
//
STATUS CMutex::Unlock()
{
#ifndef __linux__
    return ::ReleaseMutex(m_hMutex) ? OK : NOT_OK;
#else
  if(!pthread_mutex_unlock(&m_hMutex))
	return OK;
  else 
	return NOT_OK;

#endif
};


//-------------------------------------------------------------------------
// SOCKET
//-------------------------------------------------------------------------
#ifndef __linux__
in_addr GetHostByName( const char *hostname )
{
    in_addr addr;
    hostent *he;

    addr.s_addr = INADDR_NONE;


    if ( isdigit(hostname[0]) ) {
	addr.s_addr = inet_addr(hostname);
	return addr;
    }

    he = gethostbyname(hostname);
    if (!he)
	return addr;

    return *(in_addr*) he->h_addr;
};
#endif

//-------------------------------------------------------------------------
// MISCELANY


#ifndef __linux__
int stricmp( const char *lhs, const char *rhs )
{ 
    return ::stricmp(lhs, rhs); 
}
#else
// stricmp will be replaced by strcasecmp

#endif

int Prompt( const char *title, const char *message)
{
#if 0
    return ::MessageBox( NULL, message, title, MB_ICONHAND|MB_OK|MB_TOPMOST);
#else // __linux__

#endif
};

char * strltrim ( char * input )
{
    if (!input) return NULL;
    
    unsigned length = strlen(input) + 1;
    char * s = input;
    
    while (*s==' ' || *s=='\t') ++s, length--;
    memmove ( input, s, length);
    return input;
}

char * strrtrim ( char * input )
{
    if (!input) return NULL;

    char *last_ws_pos = NULL;
    char *s = input;
    
    while (*s) {
	if (*s == ' ' || *s == '\t') {
	    if (last_ws_pos==NULL) last_ws_pos = s;
	} else
	    last_ws_pos = NULL;

	s++;
    }

    if (last_ws_pos)
	*last_ws_pos = '\0';
    return input;
}



} // namespace
