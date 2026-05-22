/* $Header: /xGate/kernel.h 33    2/02/05 5:25p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * CTENGINE Kernel.
 *
 * $Log: /xGate/kernel.h $
 * 
 * 33    2/02/05 5:25p Bennylp
 * Added sbh-threshold command
 * 
 * 32    7/08/04 6:49p Bennylp
 * Call Gapping
 * 
 * 31    3/08/04 7:01p Bennylp
 * Added:
 * - Registrar API (support spawning task for incoming registration)
 * - Authentication API
 * 
 * 30    1/12/04 10:52a Bennylp
 * 
 * 29    1/09/04 6:19p Bennylp
 * Fixed bug in CTaskInfo (accessed by telnet when task is deleted)
 * 
 * 28    1/09/04 12:09p Sonny
 * 
 * 27    9/19/02 11:17a Sonny
 * Fix compilation error on atomic_get() on MSVC 6.
 * 
 * 26    9/17/02 6:08p Bennylp
 * Fixed compilation error in atomic operations under MSVC.NET/W2K
 * 
 * 25    4/04/02 3:35p Bennylp
 * Changed High_Res_Timer to highest_Res_Timer, which uses
 * CPU_Timestamp_Clock_Timer.
 * 
 * 24    3/07/02 9:21p Bennylp
 * Warning if 'save file' is called when kernel is not running
 * 
 * 23    10/08/01 6:47p Bennylp
 * Added config command to modify main thread priority.
 * 
 * 22    7/25/01 1:07p Bennylp
 * Added more statistics, and fixed bug in system lock.
 * 
 * 21    7/25/01 10:29a Bennylp
 * New HandleEvents() scheduling.
 * 
 * 20    7/16/01 11:10a Bennylp
 * New command 'sys raise' and new restart procedure.
 * 
 * 19    6/07/01 5:49p Bennylp
 * Added CheckSystemLock() to verify that thread is owning the lock.
 * 
 * 18    5/30/01 6:27p Bennylp
 * More restrictive use of system lock (i.e. recursive is not allowed)
 * 
 * 17    5/30/01 11:56a Bennylp
 * Added more states: INIT and UNLOADING for debugging.
 * 
 * 16    5/28/01 6:11p Bennylp
 * More info to system locking and handle unreleased recursive lock.
 * 
 * 15    5/27/01 11:25p Bennylp
 * Change system lock from ACE_Token to Win32 mutex.
 * 
 * 14    5/27/01 10:49p Bennylp
 * Print log when ACQUIRE_SYSTEM_LOCK failed.
 * 
 * 13    5/26/01 8:04p Bennylp
 * Record each call to AcquireSystemLock in DEBUG mode.
 * 
 * 12    21/05/01 17:09 Junanto
 * Added group name in the call-in criteria
 * 
 * 11    4/05/01 20:36 Junanto
 * The default config file is System.conf. No "configure"
 * is appended to System.conf
 * Another file "User.conf" is also loaded at startup.
 * This file is not overwritten by xGate
 * 
 * 10    5/03/01 1:09p Bennylp
 * Changed profiling to 'show times', and more improvements.
 * 
 * 9     5/03/01 11:52a Bennylp
 * Improve profiling calculations and outputs.
 * 
 * 8     4/30/01 10:21a Bennylp
 * Clean unused API profile enabled flag in kernel.
 * 
 * 7     4/28/01 6:33a Bennylp
 * Added service profiling and database driver (testing)
 * 
 * 6     4/23/01 10:44p Bennylp
 * Fixed std namespace conflicts.
 * 
 * 5     4/19/01 12:25p Bennylp
 * Add routing incoming calls.
 * 
 * 4     4/17/01 10:04p Bennylp
 * Added tracing and SIP/ATM related.
 * 
 * 3     4/12/01 8:08p Bennylp
 * Integration.
 *
 */
#ifndef __CTENGINE_KERNEL__
#define __CTENGINE_KERNEL__



#include "types.h"
#include "cfg.h"
#include "hrtimer.h"
/* #include <ace/Token.h> */
#include "call.h"

//-----------------------------------------------------------------------------
#ifdef XGATE_HAS_DIAMETER
#include "diameter/diameter_avp.h"
#endif
//-----------------------------------------------------------------------------

class CTask;
struct CTaskInfo;
class CAppConfig;
class Cfg_Tree_Group;
class ServiceProvider;
class ServiceProviderManager;
class CKernelInfo;
struct CallParameter;
class Call;
enum CallCause;
struct Reg_Info;

#ifdef Yield
#undef Yield
#endif

//#define KERNEL_CFG	"etc/System.conf"
//#define USER_CFG	"etc/User.conf"

#define BASE_DIR "/etc/xgate/"
#define KERNEL_CFG	"System.conf"
#define USER_CFG	"User.conf"

#define KERNEL_RECORD_ACQUIRE_LOCK

typedef struct
{
   char ip1[24];
   char ip2[24];
   int pid;
}ip_info;


//-----------------------------------------------------------------------------
// Shutdown parameter
//
struct KShutdown {
    enum How {
	// Reject incoming call, wait all tasks to be finished
	GRACEFULLY,

	// Broadcast shutdown exception to all tasks
	HANGUP,

	// Same as HANGUP, but enter KS_MAINTENANCE mode afterwards.
	HANGUP_MAINTENANCE,

	// Abort task execution, don't inform application.
	// (only can be called by Kernel or main.cpp)
	DEINIT,

	// Abort all execution (just quit from kernel's main loop)
	ABORT,

    };

};

//-----------------------------------------------------------------------------
// Kernel class
//

class Kernel : public Term_Cmd_Target {


public:
    /** Create the instance of the Kernel.
     */
    static Kernel * Create();

    /** Get the singleton instance of the Kernel.
     */
    static Kernel * Instance() {
	return kernelInstance;
    }

    /** Destroy the singleton instance of the kernel.
     */
    static void Destroy();

    /** Initialize kernel.
     */
    STATUS Init();

    /** Start kernel main loop.
     */
    STATUS Start() { 
	return MainLoopProc(); 
    };

    /** Acquire global system-wide mutex lock.
	In DEBUG mode, a log message will be printed to show who requests the
	lock, and a timer will be started to record how long the lock will be
	held.
     */
//Linux Porting
/*
#ifdef KERNEL_RECORD_ACQUIRE_LOCK
    bool AcquireSystemLock(const char *file, unsigned linenum); 
#else
    bool AcquireSystemLock() 
    {
	return ::WaitForSingleObject (m_SystemLock, INFINITE) == WAIT_OBJECT_0;
    }
#endif
*/

#ifdef KERNEL_RECORD_ACQUIRE_LOCK
    bool AcquireSystemLock(const char *file, unsigned linenum);
#else
    bool AcquireSystemLock()
    {
        return pthread_cond_timedwait(&m_Pcondevent, &m_SystemLock, INFINITE);
        //return ::WaitForSingleObject (m_SystemLock, INFINITE) == WAIT_OBJECT_0;
    }
#endif

    /** Release global system-wide mutex lock.
	In DEBUG mode, a log message will be printed, and kernel will record how
	long the system lock was held.
     */
#ifdef KERNEL_RECORD_ACQUIRE_LOCK
    void ReleaseSystemLock(const char *file, unsigned linenum);
#else
    void ReleaseSystemLock()
    {
	//while (::ReleaseMutex (m_SystemLock))
        while (pthread_mutex_unlock(&m_SystemLock))
	    ;
    }
#endif

#ifdef KERNEL_RECORD_ACQUIRE_LOCK
    /** Check whether current thread holds the system lock.
     */
    /* Linux Porting */
    void CheckSystemLock(const char *file, unsigned line)
    {
	//if (ACE_Thread::self() != m_SystemLockOwner) {
	  if (pthread_self() != m_SystemLockOwner) {
	    //assert (!"ACE_Thread::self() == m_SystemLockOwner");
	    //CLog::Fatal ("KERNEL", "CHECK_SYSTEM_LOCK failed in %s:%u", file, line);
	}
    }

    /** When Kernel system lock debugging is activated in compile time, this
	string will contain the name of job currently being executed in the
	kernel, or an empty string when kernel currently doesn't hold the lock.
	Note: operations to this must use atomic_get/set.
     */
    const char *currentService;
#endif

    /** Get main thread ID.
	Other thread can use the returned ID to get the handle of main thread.
	(see documentation on ::GetCurrentThread())
     */
   /*
    ACE_thread_t GetMainThreadId() {
	return m_MainThreadId;
    }
    */
    pthread_t GetMainThreadId() {
        return m_MainThreadId;
    }


    /** Shutdown kernel.
     */
    STATUS Shutdown( KShutdown::How how = KShutdown::GRACEFULLY );


    /** Cancel shutdown in progress.
     */
    STATUS CancelShutdown();


    /* Instruct the kernel to enter KS_MAINTENANCE mode. All current tasks will be 
       hung-up, and the driver who call this function will be shutdown, and
       then the kernel will enter KS_MAINTENANCE mode loop.

       During this mode, all incoming task will be rejected, and the kernel
       will attempt to Init() this driver. When this driver initialization
       is success, the kernel will RESTART.

       NOTE:
       the driver MUST support Shutdown() and re-Init().
     */
    void EnterMaintenanceMode( 
	ServiceProvider * sp,	// the failed driver
	int interval = 10,	// interval of re-initialization attempts (in secs)
	bool restart= false	// should kernel be restarted afterwards
    );


    /** Get root configuration entry.
     */
    Cfg_Tree_Group * RootConfig() { 
	return m_RootConfigTree; 
    }

    /** Get version information
     */
    const String & GetVersion() const { 
	return m_Version; 
    }

    /** Reset kernel information values
     */
    void ResetStat();

    /** Get processing time spent by the system. The value should be equal to 
	total service provider time plus total API calls time.
     */
    const Time_Value & GetKernelBusyTime() const {
	return kernelBusyTime;
    }

    /** Allocate task local storage ID (called by service providers)
     */
    int AllocTLS();


    /** Free task local storage ID (called by service providers)
     */
    STATUS FreeTLS(int);


    /** Notify kernel on incoming call.
	The kernel will create a task if there's a matched call-in-route.
     */
    STATUS RouteIncomingCall( ServiceProvider *sender, Call *call, CallCause &cause );
    CTask* RouteIncomingRegistration( const Reg_Info *info );

#ifdef XGATE_HAS_DIAMETER
    CTask* RouteIncomingDiameter(Diameter_msg msg, const string & linkName, const char* peerRealm);
#endif

    /** Find a task based on its PID
     */
    CTask* FindTask( unsigned int pid );
    CTask* FindTaskByName(const char *task_name);
    int CountTaskByAppName(const char *app_name);

    /** Spawn a new task
     */
    CTask* SpawnTask( const String & appname, const List<String> & cmdline_param,
		      int startup_type);

    /** Kill a task
     */
    STATUS KillTask(CTask *);

    /** Kill a task
     */
    STATUS KillTask(int pid);

    /** Get the currently executing task
     */
    CTask *GetCurrentTask() { 
	return m_CurrentTask; 
    }

    /** Put task in the executing queue.
     */
    STATUS QueueTask( CTask * );

    /** Get engine's base directory.
     */
    const String & GetBaseDirectory() { 
	return m_BaseDir; 
    }

    /** Determine whether kernel is in runing state.
     */
    bool IsInRunningState() const {
	return GetState() == KS_RUNNING;
    }

public:
    //Linux Porting
    //HANDLE	m_SystemLock;
    pthread_mutex_t m_SystemLock; //pthread mutex
    pthread_cond_t  m_Pcondevent;
    pthread_t       m_Thread;
    pthread_mutexattr_t  m_Pattr;
    struct timespec   m_Mutexts;
    static Kernel     *kernelInstance; //Linux Porting
#ifdef KERNEL_RECORD_ACQUIRE_LOCK
    // System lock statistics
    const char  *m_CurrentLockSrcFile;
    /* unsigned     m_CurrentLockSrcLine */
    //atomic_t m_CurrentLockSrcLine;
#endif

     Kernel();
    ~Kernel();
//-----------------------------------------------------------------------------
// Kernel state and event processing
//
private:

    // Kernel state
    enum KState {
	KS_INIT,
	KS_RUNNING,
	KS_BLOCKED,
	KS_UNLOADING,
	KS_DEAD,
	KS_MAINTENANCE,
    };

    // Init level.
    enum KInitLevel {
	LVL_NULL,
	LVL_LOG_STARTED,
	LVL_LOADED,
	LVL_STARTED,
	LVL_RUNNING,
    };

    KState	m_State;
    KInitLevel	m_InitLevel;
    int		m_MaintenanceInitInterval;
    bool	m_RestartAfterMaintenance;
    int		m_MaxWaitMaintenance;
    //static Kernel     *kernelInstance;
    //ACE_thread_t m_MainThreadId;
    pthread_t   m_MainThreadId;
    HANDLE	m_hMainThread;
    unsigned long	m_BoostSystemLockOwner;
    int		m_SbhDefaultThreshold,
		m_SbhThreshold;

#ifdef KERNEL_RECORD_ACQUIRE_LOCK
    // System lock statistics
    Highest_Res_Timer m_SystemLockTimer;
    Time_Value   m_LongestLockTime;
    const char  *m_LongestLockSrcFile;
    unsigned	 m_LongestLockSrcLine;
    //ACE_thread_t m_SystemLockOwner;
    pthread_t m_SystemLockOwner;
    unsigned	 m_SystemLockPriority;
#endif

    // Set kernel state
    void SetState( KState state ) { 
	m_State = state; 
    }

    // Get kernel state
    KState GetState() const { 
	return m_State; 
    }

    // Main kernel loop.
    STATUS MainLoopProc();

    // Main kernel loop in KS_RUNNING state.
    STATUS RunningLoopProc();

    // Main kernel loop in KS_MAINTENANCE state.
    STATUS MaintenanceLoopProc();

    // Deinitialization
    STATUS Deinit();

    // Is kernel running?
    bool IsRunning() const { 
	return GetState()!=KS_DEAD; 
    }

    void HandleEvents();

//-----------------------------------------------------------------------------
// Task management
//
private:
    //Kernel();
    //~Kernel();

    List <CTask*> Q_Born;
    List <CTask*> Q_Ready;
    List <CTask*> Q_AllTask;

    int m_MaxTask;
    CTask *m_CurrentTask;

    // Pick up next task to be executed from ready task queue.
    CTask *GetReadyTask();

    // List all tasks (Important: see comment in the kernel.cpp file)
    CTaskInfo* GetTaskInfo(int *ntask);

    // Determine whether kernel current state can spawn a new task,
    bool CanSpawnTask();

    // Add a new task in the kernel's queues.
    void AddTask(CTask *task);

//-----------------------------------------------------------------------------
// Other vars & constants

    // Task local storage
    Vector <int> m_TLS;

    // Configuration
    String	m_BaseDir;
    Cfg_Tree_Group *m_RootConfigTree;
    Cfg_Tree_Group *m_SystemConfigTree;

    // Version information
    String m_Version;

    // Statistics
    CKernelInfo	*m_KernelInfo;

    // How long kernel will Sleep when all tasks are idle (configurable)
    int	m_SleepTime;

    // Times
    Time_Value	startTime;	// Time when the kernel was started.
    Time_Value	kernelBusyTime;	// Processing time.
    Time_Value	kernelTotal;	// Processing time + synchronization overhead.

    // Statistics update
    void UpdateStat( CTask *);

    // Config interfaces.
    void InitConfig();
    void DoTermCmd(Term_Session *,bool,Term_Cmd *);
    //STATUS OnShowConfig ( const Cfg_Tree_Entry *, String &, const String & , int) const;
    bool OnShowConfig ( const Cfg_Tree_Entry *, String &, const String & , int) const;

    // Various Show commands.
    void ShowApp (Term_Session *session);
    void ShowConfig (Term_Session *session);
    void ShowStatistic (Term_Session *session);
    void ShowTask (Term_Session *session);
    void ShowTimes (Term_Session *session);
    //void ShowAPIProfile (Term_Session *session);
    //void ShowSVCProfile (Term_Session *session);

    // Check whether configuration has changed and ask user to
    // save
    void CheckConfigChanged(Term_Session *session);

// end-of Kernel decl.
};



#ifdef KERNEL_RECORD_ACQUIRE_LOCK
#define ACQUIRE_SYSTEM_LOCK()	Kernel::Instance()->AcquireSystemLock(__FILE__, __LINE__)
#define RELEASE_SYSTEM_LOCK()	Kernel::Instance()->ReleaseSystemLock(__FILE__, __LINE__)
#define CHECK_SYSTEM_LOCK()	Kernel::Instance()->CheckSystemLock(__FILE__, __LINE__)

inline void 
atomic_set (long *var, long val)
{
    //commented this code linux porting & need to write equivalent linux code 
    //::InterlockedExchange (var, val);
}

inline void 
atomic_set (const char **var, const char *val)
{
    atomic_set ((long*)var, (long)val);
}

#if _MSC_VER == 1200 /* MSVC 6.0 */
inline long 
atomic_get  (long *var)
{
    return *var;
}

#elif _MSC_VER > 1200	/* MSVC.NET */
inline long 
atomic_get  (long *var)
{
    long exchange = -2;
    long comperand = -13;
    long result;

    result = ::InterlockedCompareExchange(var, exchange, comperand);
    return result;
}
#endif

inline const char *
atomic_get  (const char **var)
{
    // commented this code for linux porting & need to write equivalent linux code
    //return (const char*) atomic_get((long*)var);
}


#else
#define ACQUIRE_SYSTEM_LOCK()	Kernel::Instance()->AcquireSystemLock()
#define RELEASE_SYSTEM_LOCK()	Kernel::Instance()->ReleaseSystemLock()
#define CHECK_SYSTEM_LOCK()
#endif



#endif	// __CTENGINE_KERNEL__
