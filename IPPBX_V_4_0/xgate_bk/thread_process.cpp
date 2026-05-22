/* $Header: /xGate/thread_process.cpp 4     10/29/02 1:06p Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * Thread and process monitoring.
 *
 * $Log: /xGate/thread_process.cpp $
 * 
 * 4     10/29/02 1:06p Bennylp
 * Minor changes: changed "main" to "main.cpp" for main thread name.
 * 
 * 3     10/24/02 8:35p Bennylp
 * Use _beginthreadex instead of CreateThread (libc safety).
 * 
 * 2     10/23/02 12:38p Bennylp
 * Finished initial version.
 * 
 * 1     10/23/02 10:51a Bennylp
 * Initial version.
 *
 */

//#pragma once

///#include "stdafx.h"
#include "thread_process.h"
#include "term.h"
#include "kernel.h"
///#include "process.h"
//#include <process.h>
#include "commonutil.h"
//#include "winnt.h"

CPU_Load_Monitor *CPU_Load_Monitor::cpuLoadMonitorInstance;
Process_Load_Monitor * Process_Load_Monitor::processLoadMonitorInstance;

/*---------------------------------------------------------------------*
 * A Win32 implementation for obtaining processor time
 *---------------------------------------------------------------------*/
//#include <winperf.h>

static unsigned long GetTickCount()
{
  struct timeval tv;
  if( gettimeofday(&tv, NULL) != 0 )
  return 0;

  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}


LONGLONG HiResFrequency()
{
    //LARGE_INTEGER freq;
   // QueryPerformanceFrequency(&freq);
    //return freq.QuadPart;
} 

LONGLONG HiResCurrentTime()
{
    /**
    static freq = HiResFrequency();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    double ret = (counter.QuadPart*1.0L)/freq;
    ret *= 10000000;
    return (LONGLONG)ret;
    **/
    //FILETIME now;
    time_t now; //linux porting
    now = time(NULL);

    //GetSystemTimeAsFileTime(&now);
    return *(LONGLONG*)&now;
}

class ProcessorTime
{
public:
    ProcessorTime() {
	allocSize = 1024;
	perfData = (PERF_DATA_BLOCK*) malloc( allocSize );

	// yields the current time-slice to another thread
	//SwitchToThread();
	pthread_yield(); // linux porting

	// this part is executed at the beginning of time-slice
	startIdleTime = get_idle_process_time();
	startSystemTime = HiResCurrentTime();
    }

    ~ProcessorTime() {
	if (perfData) free(perfData);
    }

   //------------------------------------------------
    LONGLONG get_elapsed_processor_time() {

	// yields the current time-slice to another thread
	//SwitchToThread();
	pthread_yield(); // linux porting

	// this part is executed at the beginning of time-slice
	LONGLONG idleTime = get_idle_process_time();
	LONGLONG now = HiResCurrentTime();
	// hopefully we're not preempted up to this point

	return (now-startSystemTime)-(idleTime-startIdleTime);
    }

private:
    unsigned allocSize;
    PERF_DATA_BLOCK* perfData;
    LONGLONG startSystemTime;
    LONGLONG startIdleTime;

    //------------------------------------------------
    PERF_INSTANCE_DEFINITION* first_instance( PERF_OBJECT_TYPE* perfObject ) {
	return( (PERF_INSTANCE_DEFINITION*)((PBYTE)perfObject +
	    perfObject->DefinitionLength) );
    }

    //------------------------------------------------
    PERF_INSTANCE_DEFINITION* next_instance( PERF_INSTANCE_DEFINITION* perfInstance ) {
	PERF_COUNTER_BLOCK* perfCounterBlk;

	perfCounterBlk = (PERF_COUNTER_BLOCK*)((PBYTE)perfInstance +
	    perfInstance->ByteLength);

	return( (PERF_INSTANCE_DEFINITION*)((PBYTE)perfCounterBlk +
	    perfCounterBlk->ByteLength) );
    }

    //------------------------------------------------
    PERF_COUNTER_DEFINITION* first_counter( PERF_OBJECT_TYPE* perfObject ) {
	return( (PERF_COUNTER_DEFINITION*) ((PBYTE)perfObject +
	    perfObject->HeaderLength) );
    }

    //------------------------------------------------
    PERF_COUNTER_DEFINITION* next_counter( PERF_COUNTER_DEFINITION* perfCounter ) {
	return( (PERF_COUNTER_DEFINITION*)((PBYTE)perfCounter +
	    perfCounter->ByteLength) );
    }

    //------------------------------------------------
    LONGLONG get_idle_process_time() {
	LONGLONG cumulProcTime = { 0 };
	PERF_OBJECT_TYPE* perfObject;
	PERF_INSTANCE_DEFINITION* perfInstance;
	PERF_COUNTER_DEFINITION* perfCounter;
	PERF_COUNTER_BLOCK* counterBlock;

	// obtain the performance data for Processor object (238)
	/* unsigned long bufferSize = allocSize;
	while( RegQueryValueEx(
		HKEY_PERFORMANCE_DATA, "238",
		NULL, NULL, (LPBYTE) perfData,
		&bufferSize ) == ERROR_MORE_DATA ) {
	    allocSize += 1024;
	    perfData = (PERF_DATA_BLOCK*) realloc( perfData, allocSize );
	    bufferSize = allocSize;
	} */

	// examine Processor object (if any)
	if (perfData->NumObjectTypes) {

	    perfObject = (PERF_OBJECT_TYPE*)((PBYTE)perfData + perfData->HeaderLength);
	    perfCounter = first_counter( perfObject );
	    if( perfObject->NumInstances > 0 ) {

		perfInstance = first_instance( perfObject );

		// accumulate all "Processor Time" counter (6) for all instances
		for(int k=0; k<perfObject->NumInstances; k++ ) {

		    PERF_COUNTER_DEFINITION* currentCtrDef = perfCounter;
		    counterBlock = (PERF_COUNTER_BLOCK*) ((PBYTE)perfInstance + perfInstance->ByteLength);
		    for(unsigned j=0; j < perfObject->NumCounters; j++ ) {

			if (currentCtrDef->CounterNameTitleIndex == 6) {
			    LONGLONG procTime = *(LONGLONG*) ((PBYTE)counterBlock + currentCtrDef->CounterOffset);
			    cumulProcTime += procTime;
			    break;
			}

			currentCtrDef = next_counter( currentCtrDef );
		    }

		    // next instance.
		    perfInstance = next_instance( perfInstance );
		}

		// get the average of Processor Time
		cumulProcTime = cumulProcTime/perfObject->NumInstances;
	    }
	}

	return cumulProcTime;
    }
};
/*---------------------------------------------------------------------*/


///////////////////////////////////////////////////////////////////////////////

Load_Monitor::Load_Monitor()
: oneSecond(1), fiveSeconds(5), oneMinute(60), fiveMinutes(300), oneHour(3600)
{
}


Load_Monitor::~Load_Monitor()
{
}

/** Get the percentage time spent in user space for this process.
 */
unsigned Load_Monitor::GetPercentUserTime (SnapshotType type) const
{
    switch (type) {
    case OneSecond:
	return oneSecond.GetPercentUser();
    case FiveSeconds:
	return fiveSeconds.GetPercentUser();
    case OneMinute:
	return oneMinute.GetPercentUser();
    case FiveMinutes:
	return fiveMinutes.GetPercentUser();
    case OneHour:
	return oneHour.GetPercentUser();
    }
    assert (false);
    return 0;
}

/** Get the percentage time spent in kernel space for this process.
 */
unsigned Load_Monitor::GetPercentKernelTime (SnapshotType type) const
{
    switch (type) {
    case OneSecond:
	return oneSecond.GetPercentKernel();
    case FiveSeconds:
	return fiveSeconds.GetPercentKernel();
    case OneMinute:
	return oneMinute.GetPercentKernel();
    case FiveMinutes:
	return fiveMinutes.GetPercentKernel();
    case OneHour:
	return oneHour.GetPercentKernel();
    }
    assert (false);
    return 0;
}


//void Load_Monitor::UpdateTimes(const FILETIME & tKernel, const FILETIME & tUser)
void Load_Monitor::UpdateTimes(const time_t & tKernel, const time_t & tUser)
{
    LONGLONG kernel, user;
    /* kernel = tKernel.dwHighDateTime;
    kernel <<= 32;
    kernel += tKernel.dwLowDateTime;
    user = tUser.dwHighDateTime;
    user <<= 32;
    user += tUser.dwLowDateTime;

    LONGLONG now = HiResCurrentTime();

    oneSecond.Update (now, user, kernel);
    fiveSeconds.Update (now, user, kernel);
    oneMinute.Update (now, user, kernel);
    fiveMinutes.Update (now, user, kernel);
    oneHour.Update( now, user, kernel); */
}


String Load_Monitor::Print() const
{
    unsigned onesec_cpu, fivesec_cpu, fivemin_cpu, onehour_cpu;
    onesec_cpu =  GetPercentUserTime(OneSecond) + GetPercentKernelTime(OneSecond);
    fivesec_cpu = GetPercentUserTime(FiveSeconds) + GetPercentKernelTime(FiveSeconds);
    fivemin_cpu = GetPercentUserTime(FiveMinutes) + GetPercentKernelTime(FiveMinutes);
    onehour_cpu = GetPercentUserTime(OneHour) + GetPercentKernelTime(OneHour);

    char buf[128];
    sprintf( buf, "%u%% (1sec) %u%% (5secs) %u%% (5mins) %u%% (1hour)",
		   onesec_cpu, fivesec_cpu, fivemin_cpu, onehour_cpu);

    return (const char*)buf;
}


///////////////////////////////////////////////////////////////////////////////

#define POLL_FREQUENCY	    16

CPU_Load_Monitor * CPU_Load_Monitor::Create()
{
    if (cpuLoadMonitorInstance == NULL)
	cpuLoadMonitorInstance = new CPU_Load_Monitor;
    return cpuLoadMonitorInstance;
}

CPU_Load_Monitor::CPU_Load_Monitor()
: ServiceProvider("CPU %", false, true, Lowest_Priority)
{
    pollCounter = 0;
    procTime = new ProcessorTime;
    memset(&fzero, 0, sizeof(fzero));
    UpdateTimes(fzero, fzero);
}

CPU_Load_Monitor::~CPU_Load_Monitor()
{
    cpuLoadMonitorInstance = NULL;
    delete procTime;
}

void CPU_Load_Monitor::HandleEvents()
{
    if (++pollCounter == POLL_FREQUENCY) {
	//FILETIME load;
	time_t load;
	LONGLONG temp = procTime->get_elapsed_processor_time();

	//load = *(FILETIME*)&temp;
	load = *(time_t*)&temp;
	UpdateTimes(load, fzero);

	pollCounter = 0;
    }
}


///////////////////////////////////////////////////////////////////////////////


Process_Load_Monitor * Process_Load_Monitor::Create()
{
    if (processLoadMonitorInstance == NULL)
	processLoadMonitorInstance = new Process_Load_Monitor;
    return processLoadMonitorInstance;
}

Process_Load_Monitor::Process_Load_Monitor()
: ServiceProvider("Process %", false, true, Lowest_Priority)
{
}

Process_Load_Monitor::~Process_Load_Monitor()
{
    processLoadMonitorInstance = NULL;
}

void Process_Load_Monitor::HandleEvents()
{
    //FILETIME creation, exit_time, kernel, user;
    //time_t creation, exit_time, kernel, user;
    //GetProcessTimes( GetCurrentProcess(), &creation, &exit_time, &kernel, &user);
    //GetProcessTimes( getpid(), &creation, &exit_time, &kernel, &user);
    //UpdateTimes( kernel, user);
}


///////////////////////////////////////////////////////////////////////////////
/** This class manages all threads running in the program and keep track of
    their time usage (and other info).
 */
Thread_Manager *Thread_Manager::Instance()
{
    static Thread_Manager *instance;
    if (instance == NULL)
	instance = new Thread_Manager;
    return instance;
}

Thread_Manager::Thread_Manager()
: ServiceProvider("Thread-Mgr", false, true, Lowest_Priority, "Thread Manager"), last_poll_(0)
{
}

bool Thread_Manager::Load()
{
    //Thread_Instance main_thread("main.cpp", 1, GetCurrentThread(), GetCurrentThreadId(),
    Thread_Instance main_thread("main.cpp", 1, getpid(), 0, 0);
    thread_list_.push_back(main_thread);
    return true;
}

bool Thread_Manager::Unload()
{
    return true;
}

void Thread_Manager::HandleEvents()
{
    //int policy = SCHED_OTHER;

    enum { PERIODIC = 1000 };

    if (GetTickCount() - last_poll_ >= PERIODIC) {
	last_poll_ = GetTickCount();

	List<Thread_Instance>::iterator it, end=thread_list_.end();
	for (it=thread_list_.begin(); it!=end; ) {
	    Thread_Instance & instance = *it;
	    if (!instance.GetThreadTime())
		it = thread_list_.erase(it);
	    else {
		if (instance.prio_ == -1)
		    instance.prio_ = sched_get_priority_max(SCHED_OTHER); //instance.prio_ = GetThreadPriority(instance.hthread_);
		++it;
	    }
	}
    }
}

static const char *extract_filename(const char *path)
{
    int len = strlen(path);
    if (len < 2)
	return path;

    char *p = (char*) (path+len-2);
    while (*p != '/' && *p!= '\\' && p != path) {
	--p;
    }
    
    if (*p=='/' || *p=='\\')
	++p;

    len = strlen(p);
    if (len > 18)
	p += (len-18);
    return p;
}


/* HANDLE Thread_Manager::CreateThread(const char *filename, int line,
				    LPSECURITY_ATTRIBUTES lpThreadAttributes,
				    DWORD dwStackSize,
				    LPTHREAD_START_ROUTINE lpStartAddress,
				    LPVOID lpParameter,
				    DWORD dwCreationFlags,
				    LPDWORD lpThreadId)
{
    DWORD id;
    unsigned flag = 0;

    if (dwCreationFlags & CREATE_SUSPENDED)
	flag |= CREATE_SUSPENDED;

    //HANDLE hthread = ::CreateThread( lpThreadAttributes, dwStackSize, lpStartAddress,
    //				     lpParameter, dwCreationFlags, &id);
    HANDLE hthread = (HANDLE)_beginthreadex( lpThreadAttributes, dwStackSize,
					     (unsigned (__stdcall *)(void*))lpStartAddress, lpParameter,
					     flag,(unsigned*)&id);

    if (hthread != NULL) {
	Thread_Instance instance(extract_filename(filename), line, hthread, id, lpStartAddress, lpParameter);
	thread_list_.push_back(instance);
	if (lpThreadId)
	    *lpThreadId = id;
    }

    return hthread;
} */

#if 0 //rkv commented this for testing
//HANDLE Thread_Manager::CreateThread()
void* Thread_Manager::CreateThread()
{

	pthread_t my_thread;
	//void *ret = NULL;
	int ret;
        int err = 0;

	#if 0
        ret = pthread_create(&my_thread, NULL, /*thread_handler*/NULL, NULL);
	if(ret != 0) {
           printf("Error: pthread_create() failed\n");
           exit(EXIT_FAILURE);
        }

        pthread_exit(NULL);
	#endif //rkv

}
#endif



// converter between ACE_THR_FUNC to (unsigned (__stdcall *)(void*))
struct Proxy_Argument
{
    ACE_THR_FUNC func;
    void* arg;
};

//unsigned proxy_function(void* v)
unsigned __stdcall proxy_function(void* v)
{
    Proxy_Argument* p = (Proxy_Argument*)v;

    unsigned rc = (long)p->func(p->arg);
    delete p;
    return rc;
}

//HANDLE Thread_Manager::spawn(  const char *filename, int line,
/* int Thread_Manager::spawn(  const char *filename, int line,
			    unsigned stack_size,
			    ACE_THR_FUNC func, 
			    void *arg, 
			    int flag_param, 
			    ACE_thread_t *t_id,
			    ACE_hthread_t *t_handle) */

int Thread_Manager::spawn( ACE_THR_FUNC func, void *arg, 
			    int flag_param, 
			    pthread_t *t_id,
			    pthread_t *t_handle)
{
    pthread_t my_thread;

    DWORD  id;
    //int result = ACE_Thread::spawn( func, arg, flags, &id, &hthread);
    int flag = 0;
    //if (flag_param & THR_SUSPENDED)
	//flag |= CREATE_SUSPENDED;

    Proxy_Argument* p = new Proxy_Argument;
    p->func = func;
    p->arg = arg;

    /* HANDLE hthread = (HANDLE)_beginthreadex( 0, stack_size,
					     proxy_function, p,
					     flag,(unsigned*)&id);
    if (hthread != NULL) {
	Thread_Instance instance(extract_filename(filename), line, hthread, id, func, arg);
	thread_list_.push_back(instance);
	if (t_handle)
	    *t_handle = hthread;
	if (t_id)
	    *t_id = id;
    } */

       int ret;
       ret = pthread_create(&my_thread, NULL, /*thread_handler*/proxy_function, (void *) arg);
       printf("return value from pthread_create =%d\n",ret);
       if(ret != 0) {
           printf("Error: pthread_create() failed\n");
           exit(EXIT_FAILURE);
        }

        //pthread_exit(NULL);

    //return hthread == NULL ? -1 : 0;

    return 0;
}

bool Thread_Manager::Thread_Instance::GetThreadTime()
{
    //FILETIME tCreation, tExit, tKernel, tUser;
    time_t tCreation, tExit, tKernel, tUser;
    //if (GetThreadTimes(hthread_, &tCreation, &tExit, &tKernel, &tUser) == FALSE)
	//return false;

    //UpdateTimes(tKernel, tUser);	//linux porting
    return true;
}

void Thread_Manager::Thread_Instance::HandleEvents()
{
}

static const char *priority_to_str(int prio)
{
    /* switch (prio) {
    case THREAD_PRIORITY_ABOVE_NORMAL:	return "  ABOVE ";
    case THREAD_PRIORITY_BELOW_NORMAL:	return "  BELOW ";
    case THREAD_PRIORITY_HIGHEST:	return " HIGHEST";
    case THREAD_PRIORITY_IDLE:		return "  IDLE  ";
    case THREAD_PRIORITY_LOWEST:	return " LOWEST ";
    case THREAD_PRIORITY_NORMAL:	return " NORMAL ";
    case THREAD_PRIORITY_TIME_CRITICAL: return "CRITICAL";
    default:				return "UNKNOWN?";
    } */

    //The below code added linux porting 
    switch (prio) {
    case 5:	return "  ABOVE ";
    case 3:	return "  BELOW ";
    case 6:	return " HIGHEST";
    case 1:	return "  IDLE  ";
    case 2:	return " LOWEST ";
    case 4:	return " NORMAL ";
    case 15: 	return "CRITICAL";
    default:	return "UNKNOWN?";
    }
}

void Thread_Manager::ShowThreadsStatus(Term_Session *session)
{
    String output;
    char temp[128];

    Load_Monitor *lm = CPU_Load_Monitor::Instance();
    if (lm) {
	sprintf(temp, "Average CPU Load    : %3d%% %3d%% %3d%% (5s 1m 1h)\n",
		lm->GetPercentUserTime(Load_Monitor::FiveSeconds) + lm->GetPercentKernelTime(Load_Monitor::FiveSeconds),
		lm->GetPercentUserTime(Load_Monitor::OneMinute) +   lm->GetPercentKernelTime(Load_Monitor::OneMinute),
		lm->GetPercentUserTime(Load_Monitor::OneHour) +	    lm->GetPercentKernelTime(Load_Monitor::OneHour));
	output += temp;
    }
    lm = Process_Load_Monitor::Instance();
    if (lm) {
	sprintf(temp, "Average process Load: %3d%% %3d%% %3d%%\n",
		lm->GetPercentUserTime(Load_Monitor::FiveSeconds) + lm->GetPercentKernelTime(Load_Monitor::FiveSeconds),
		lm->GetPercentUserTime(Load_Monitor::OneMinute) +   lm->GetPercentKernelTime(Load_Monitor::OneMinute),
		lm->GetPercentUserTime(Load_Monitor::OneHour) +	    lm->GetPercentKernelTime(Load_Monitor::OneHour));
	output += temp;
    }
    if (lm)
	output += "\n";


    output += "Dump thread status:\n";
    sprintf(temp, "  -  ID  -   Hnd  -  Prio  -  Start - Module Name      - Pos   5s%% 1m%% 1h%%\n");
    output += temp;
    output.append(79, '-');
    output += "\n";

    if (!ACQUIRE_SYSTEM_LOCK())
	return;

    List<Thread_Instance>::const_iterator it, end=thread_list_.end();
    int i=0;
    for (it=thread_list_.begin(); it!=end; ++it, ++i) {
	const Thread_Instance & instance = *it;
	sprintf(temp, "%2d %6x %8x %s %8x %18s %6u  %2d  %2d  %2d\n",
		      i,
	              instance.id_, instance.hthread_, priority_to_str(instance.prio_), 
		      instance.start_func_, instance.filename_, instance.line_,
		      instance.GetPercentUserTime(Load_Monitor::FiveSeconds) + instance.GetPercentKernelTime(Load_Monitor::FiveSeconds),
		      instance.GetPercentUserTime(Load_Monitor::OneMinute) + instance.GetPercentKernelTime(Load_Monitor::OneMinute),
		      instance.GetPercentUserTime(Load_Monitor::OneHour) + instance.GetPercentKernelTime(Load_Monitor::OneHour));
	output += temp;
    }

    RELEASE_SYSTEM_LOCK();
    session->Send(output.c_str(), output.size());
}

