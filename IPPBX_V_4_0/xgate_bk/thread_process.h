/* $Header: /xGate/thread_process.h 2     2/02/05 9:31a Bennylp $
 *   
 * XGATE
 * (C) 2001 Switchlab, Ltd
 *
 * Thread and process monitoring.
 *
 * $Log: /xGate/thread_process.h $
 * 
 * 2     2/02/05 9:31a Bennylp
 * Removed assertion for value above 100 percent.
 * 
 * 1     10/23/02 10:51a Bennylp
 * Initial version.
 *
 */
//#ifndef __THREAD_PROCESS_H__
//#define __THREAD_PROCESS_H__
//#define __stdcall

//#include "ServiceProvider.h"
#include "serviceprovider.h"
//#include "Thread.h"
#include <ace/Thread.h>
#include "commonutil.h"

//#include "common.h"
///#include <ace/Thread.h>
//#include "/home/rajk/xgate/branches/DigitalDialogic_driver/core/lace/ace/Thread.h"

// The below code for porting into linux
//typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
typedef union _LARGE_INTEGER LARGE_INTEGER;
//unsigned long         Lhthread_;
#define Lhthread_	void*

///////////////////////////////////////////////////////////////////////////////
/** Abstract base class to maintain the load monitoring values. For the class to be
    able to calculate the load, this class must be called periodically.
    This class is used by the kernel.
 */

class ServiceProvider;

class Load_Monitor {
public:
    enum SnapshotType {
	OneSecond,
	FiveSeconds,
	OneMinute,
	FiveMinutes,
	OneHour,
    };

    /** Get the percentage time spent in user space for this process.
     */
    unsigned GetPercentUserTime (SnapshotType type) const;

    /** Get the percentage time spent in kernel space for this process.
     */
    unsigned GetPercentKernelTime (SnapshotType type) const;

    /** Call this function periodically, at least once per second if application
	wants to get one second time precision.
     */
    virtual void HandleEvents() = 0;

    /** Create time output.
     */
    String Print() const;

protected:
    Load_Monitor();
    ~Load_Monitor();

    //void UpdateTimes(const FILETIME & kernel, const FILETIME & user);
    void UpdateTimes(const time_t & kernel, const time_t & user);

private:
    LONGLONG  startTime;

    class Snapshot {
    public:
	size_t	    updateInterval;
	LONGLONG    timeTaken;
	LONGLONG    lastUserSnapshot, lastKernelSnapshot;
	size_t      percentUser, percentKernel;
	bool	    firstTime;

	Snapshot(size_t intv)
	    : updateInterval(intv), lastUserSnapshot(0), lastKernelSnapshot(0),
	      percentUser(0), percentKernel(0)
	{
	    firstTime = true;
	}
	void Update (const LONGLONG & now, LONGLONG user, LONGLONG kernel)
	{
	    if (firstTime) {
		timeTaken = now;
		lastUserSnapshot = user;
		lastKernelSnapshot = kernel;
		firstTime = false;
		return;
	    }

	    LONGLONG interval = now-timeTaken;
	    LONGLONG intervalSec = interval/10000000;

	    if (intervalSec >= updateInterval) {
		percentUser = (user - lastUserSnapshot) * 100 / interval;
		lastUserSnapshot = user;
		percentKernel = (kernel - lastKernelSnapshot) * 100 / interval;
		if (percentKernel > 100) percentKernel = 100;
		lastKernelSnapshot = kernel;
		timeTaken = now;
	    }
	}
	unsigned GetPercentUser() const 
	{
	    return percentUser;
	}
	unsigned GetPercentKernel() const
	{
	    return percentKernel;
	}
    };
    
    Snapshot  oneSecond, fiveSeconds, oneMinute, fiveMinutes, oneHour;
};


// defined in sleepdrv.cpp
class ProcessorTime;

///////////////////////////////////////////////////////////////////////////////
/** This class monitors the global CPU load. This is a singleton instance since
    we don't need multiple CPU_Load_Monitor in a single process.
 */
class CPU_Load_Monitor : public Load_Monitor, public ServiceProvider {
public:
    static CPU_Load_Monitor * Create();

    static CPU_Load_Monitor * Instance() {
	return cpuLoadMonitorInstance;
    }

//private:
    static CPU_Load_Monitor * cpuLoadMonitorInstance;
    int pollCounter;
    ProcessorTime *procTime;
    //FILETIME fzero;
    time_t fzero;

    CPU_Load_Monitor();
    ~CPU_Load_Monitor();

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load() {
	return true;
    }

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload() {
	return true;
    }

    /** HandleEvents: Descendent shall implement this abstract method.
	It is called by Service Provider Manager
     */
    virtual void HandleEvents();
};


///////////////////////////////////////////////////////////////////////////////
/** This class monitors the process CPU load. This is a singleton instance since
    we don't need multiple Process_Load_Monitor in a single process.
 */
class Process_Load_Monitor : public Load_Monitor, public ServiceProvider {
public:
    static Process_Load_Monitor * Create();
    static Process_Load_Monitor * Instance() {
	return processLoadMonitorInstance;
    }

private:
    static Process_Load_Monitor * processLoadMonitorInstance;

    Process_Load_Monitor();
    ~Process_Load_Monitor();

    virtual bool Load() {
	return true;
    }
    virtual bool Unload() {
	return true;
    }
    virtual void HandleEvents();
};



///////////////////////////////////////////////////////////////////////////////
/** This class monitors the thread time.
 */
class Thread_Time_Monitor : public Load_Monitor {
public:
    Thread_Time_Monitor() {}
    ~Thread_Time_Monitor() {}

    /** HandleEvents: Descendent shall implement this abstract method.
	It is called by Service Provider Manager
     */
    virtual void HandleEvents();
};


///////////////////////////////////////////////////////////////////////////////
/** This class manages all threads running in the program and keep track of
    their time usage (and other info).
 */
class Thread_Manager : public ServiceProvider
{
public:

    //Thread_Manager();

    static Thread_Manager *Instance();

    virtual bool Load();
    virtual bool Unload();
    virtual void HandleEvents();

    /* HANDLE CreateThread(const char *filename, int line,
			       LPSECURITY_ATTRIBUTES lpThreadAttributes,
			       DWORD dwStackSize,
			       LPTHREAD_START_ROUTINE lpStartAddress,
			       LPVOID lpParameter,
			       DWORD dwCreationFlags,
			       LPDWORD lpThreadId); */

    //HANDLE CreateThread();	//linux porting

    int spawn(  ACE_THR_FUNC func,void *arg = 0, 
		int flags = THR_NEW_LWP | THR_JOINABLE, 
		pthread_t *t_id = 0,
		pthread_t *t_handle = 0);

    /* HANDLE spawn(  const char *filename, int line,
		unsigned stack_size,
		ACE_THR_FUNC func, 
		void *arg = 0, 
		int flags = THR_NEW_LWP | THR_JOINABLE, 
		ACE_thread_t *t_id = 0,
		ACE_hthread_t *t_handle = 0); */

    void ShowThreadsStatus(Term_Session *session);

private:
    class Thread_Instance : public Load_Monitor
    {
    public:
	/* Thread_Instance(const char *file, int line, 
			HANDLE hthread, DWORD id,
			void *func, void *arg)
	    : filename_(file), line_(line), 
	      hthread_(hthread), id_(id),
	      start_func_(func), arg_(arg), prio_(-1)
	{
	} */

	Thread_Instance(const char *file, int line, 
			DWORD id,
			void *func, void *arg)
	    : filename_(file), line_(line), 
	      id_(id),
	      start_func_(func), arg_(arg), prio_(-1)
	{
	}

	bool GetThreadTime();
	virtual void HandleEvents();
	//unsigned long Thread_Manager:: GetTickCount(); //linux porting
	//unsigned long GetTickCount();

	const char *filename_;
	int	    line_;
	HANDLE	    hthread_;
	//unsigned lond 	hthread_;
	DWORD	    id_;
	void	   *start_func_;
	void       *arg_;
	int	    prio_;
    };

    List<Thread_Instance> thread_list_;
    DWORD last_poll_;

    Thread_Manager();
};

//linux porting
#define SPAWN_THREAD(arg, flags, id, handle)  \
	Thread_Manager::Instance()->spawn( arg, flags, id, handle)
#define SPAWN_THREAD2(stksz, func, arg, flags, id, handle)  \
	Thread_Manager::Instance()->spawn(__FILE__, __LINE__, stksz, func, arg, flags, id, handle) */

//#endif /* __THREAD_PROCESS_H__ */
