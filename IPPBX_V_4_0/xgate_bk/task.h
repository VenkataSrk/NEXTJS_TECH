/* $Header: /xGate/task.h 26    24/05/04 11:08 Sonny $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Application task instance.
 *
 * $Log: /xGate/task.h $
 * 
 * 26    24/05/04 11:08 Sonny
 * Added exception id: EX_TIMER
 * 
 * 25    3/08/04 7:08p Bennylp
 * Support for different startup type (such as Registrar) and protection
 * to GetStringTab() and CopyString()
 * 
 * 24    1/27/04 10:53a Bennylp
 * Change MBOX exception ID constant (from IPC)
 * 
 * 23    1/09/04 6:19p Bennylp
 * Fixed bug in CTaskInfo (accessed by telnet when task is deleted)
 * 
 * 22    1/09/04 12:10p Sonny
 * added EX_AUTOMATION_MSG
 * 
 * 21    3/01/03 7:03p Bennylp
 * Remove App_String
 * 
 * 20    21/02/03 18:10 Junanto
 * added safe application string
 * 
 * 19    4/04/02 3:35p Bennylp
 * Changed High_Res_Timer to highest_Res_Timer, which uses
 * CPU_Timestamp_Clock_Timer.
 * 
 * 18    1/30/02 12:05p Bennylp
 * Fixed miscelanous bugs related to interpreter.
 * 
 * 17    10/17/01 8:16p Bennylp
 * Hide some Task_Exec_Context member vars from interpreter
 * 
 * 16    7/08/01 13:14 Sonny
 * check string index for debugging
 * 
 * 15    7/25/01 3:51p Bennylp
 * Changed low res timer with high res timer.
 * 
 * 14    7/25/01 10:29a Bennylp
 * New HandleEvents() scheduling.
 * 
 * 13    7/14/01 7:53p Bennylp
 * Reduce friends of class Task_Exec_Context.
 * 
 * 12    28/06/01 12:49 Sonny
 * 
 * 11    28/06/01 11:40 Sonny
 * Fixed bug in Task_Exec_Context destructor in low mem situation.
 * 
 * 10    1/05/01 15:43 Junanto
 * Include source in RaiseException
 * for logging purpose
 * 
 * 9     5/01/01 2:39a Sonny
 * applicaton statistics
 * 
 * 8     4/30/01 12:21a Sonny
 * 
 * 7     4/28/01 6:33a Bennylp
 * Added service profiling and database driver (testing)
 * 
 * 6     4/25/01 6:46a Sonny
 * 
 * 5     4/18/01 5:54p Bennylp
 * Added ArgGet..() and more time APIs.
 * 
 * 4     4/13/01 11:21p Bennylp
 * Added interface for getting the task's command line arguments.
 * 
 * 3     4/12/01 8:08p Bennylp
 * Integration.
 *
 */
#ifndef __CTASK_H__
#define __CTASK_H__

//-----------------------------------------------------------------------------

#include "types.h"
#include "hrtimer.h"
#include "application.h"

//-----------------------------------------------------------------------------

/* Forward decl.
 */
class Kernel;
class ServiceProvider;
struct CAPIDef;
class CTask;
union TMEM;
class Code_Interpreter;
class API_Provider_Manager;

#define App_String		String
#define Safe_Memory_Allocator	int

//-----------------------------------------------------------------------------

// Maximum TLS entry (also affect maximum number of drivers)
#define MAX_TLS		64

// Number of arguments to be stored for late API logging
#define API_MAX_ARGS	16


//-----------------------------------------------------------------------------
// EXCEPTIONS
//
enum TaskExceptionId {
    
    EX_NO_EXCEPTION=0, 
    EX_LINE_DROP,
    EX_DIAL_STATUS,
    EX_HANGUP,
    EX_MBOX,
    EX_AUTOMATION_MSG,
    EX_RUNTIME_ERROR,
    EX_TIMER,
    EX_HANDOVER_FAILURE,
    EX_SUSPEND,
    EX_RESUME,
    EX_NOTIFY_ECT,
    EX_TRANSFER_B,
    EX_CONF,
    EX_CALLFLIP_B,
    E_DIGIT,
    EX_TRANSFER_A,
    EX_CALLFLIP_A,
    EX_CALLFLIP_APP_B,
    EX_CALLFLIP_APP_A,
    EX_CALLPARK_B,
    EX_CALLUNFLIP_APP_B,
    EX_CALLUNFLIP_APP_A,
    EX_CALLPARK_A,
    EX_CONNECT_BACK,
    EX_SUSPEND_APP,
    EX_RESUME_APP,
    EX_REC_START,
    EX_REC_STOP,
    EX_SWB_MAIN=29,
};


//-----------------------------------------------------------------------------
// 
//  Task information structure
//
#pragma pack(1)

const int MAX_TASK_INFO_NAME=32;
const int MAX_TASK_INFO_API_NAME=32;

struct CTaskInfo {
    unsigned int    m_Size;	    // size of this structure
    unsigned int    m_PID;	    // task PID
    char	    m_Name[MAX_TASK_INFO_NAME];	    // name
    char	    m_Status;	    // 1==READY, 2==BLOCKED
    char	    m_BlockedDrv[MAX_TASK_INFO_NAME];   // driver name, if blocked
    unsigned int    m_Uptime,	    // lifetime, in seconds
		    m_ExecCnt,	    // execution count
		    m_MSExecTime,   // Total exec time, in miliseconds
		    m_MSBlockedTime,// Total blocked time, in miliseconds
		    m_MSLongExec,   // Longest execution before blocked, in ms
		    m_MSLongBlocked,// Longest blocked time
		    m_MSLongWaiting;// Longest waiting time
    CAPIDef	   *m_LastAPI;	    // last API called

    // This member is only valid if task info is duplicated
    // via Kernel::GetTaskInfo(). Otherwise it's always empty.
    char	    m_LastAPIName[MAX_TASK_INFO_API_NAME];

    // ctor
    CTaskInfo() {
	m_Name[0] = m_BlockedDrv[0] = '\0';
	m_Uptime = m_ExecCnt = 
	m_MSExecTime = m_MSBlockedTime = 
	m_MSLongExec = m_MSLongWaiting = 
	m_MSLongBlocked = 0;
	m_LastAPI = NULL;
	m_LastAPIName[0] = '\0';
    }
};

#pragma pack()


class Task_Exec_Context;

//=============================================================================
/** Private data of Task execution context that will not be accessible from
    other classes.
 */
class Task_Private_Context {
private:
    int	    m_stringtab_size;
    App_String* m_stringtab;
    int stringtab_debug_level;

    // application constant string storage
    App_String** m_const_string;

    friend class Task_Exec_Context;
};


//=============================================================================
/** Task execution context; this class describes the task's running status.
 */
class Task_Exec_Context : private Task_Private_Context {

    typedef struct {
	int min,max;
    } TRANGE;

    const TMEM  *m_code;
    TMEM   *m_data;
    long    m_datasize,m_stacksize;

    TRANGE  m_SS;
    int m_IP,m_SP,m_BP, m_EP;

    int	    m_stringidx;
    int	    m_minstridx;
    int	    m_sstack;
    char    m_apistringtab[API_MAX_ARGS][MAXAPPSTRLEN];
    int	    m_apistringref[API_MAX_ARGS];

    CT_Application *m_Application;
    CT_App_Image* m_AppImage;
    Safe_Memory_Allocator *allocator_;

    Vector <String>  m_CmdLine;

    size_t m_StatAllString; // size of all string used by this context

    /* This should be the only fried this class has!!
       Common sense: more friends means more trouble!
     */
#ifdef STRINGTAB_CHECK
    unsigned long m_stringtab_checksum;
    unsigned long Checksum(char* file, unsigned line) {
	unsigned long chksum = -1;

	if (m_stringtab) {
	    try {
		chksum = 0;
		//chksum = (long) &m_stringtab;
		for(int i=0; i<m_stringtab_size; i++) {
		    //chksum += (long)m_stringtab[i].begin() + m_stringtab[i].size();
		    chksum += m_stringtab[i].size();
		}
	    } catch(...) {
		CLog::Error("TASK", "%s-Failed to calculate checksum. Please check %s line %d", this->GetApplication()->Name().c_str(), file, line);
	    }
	} else {
	    CLog::Error("TASK", "%s - Failed to calculate checksum. m_stringtab is NULL. Please check %s line %d", this->GetApplication()->Name().c_str(), file, line);
	}
	return chksum;
    }
#endif

    friend class Code_Interpreter;

public:
#ifdef STRINGTAB_CHECK
    void Check_StringTab(char *sender, char *file, unsigned line, int write_access) {
	unsigned long cur_checksum = Checksum(file, line);
	if (write_access) {
	    m_stringtab_checksum = cur_checksum;
	    if (stringtab_debug_level>0) {
		CLog::Info("TASK", "%s writing to %s's m_stringtab (%u).", sender, this->GetApplication()->Name().c_str(), m_stringtab_checksum);
	    }
	} else {
	    if (stringtab_debug_level>0) {
		CLog::Info("TASK", "%s reading from %s's m_stringtab (%u).", sender, this->GetApplication()->Name().c_str(), m_stringtab_checksum);
	    }
	}
	if (m_stringtab_checksum!=cur_checksum) {
	    CLog::Error("TASK", "%s's m_stringtab is invalid (%s). Checksum was %u. Current checksum is %u. Please check %s line %d", this->GetApplication()->Name().c_str(), sender, m_stringtab_checksum, cur_checksum, file, line);
	}
    }
#endif
    /** Constructor/destructor.
     */
    Task_Exec_Context ( CT_Application *app );
    ~Task_Exec_Context();

    // This string is used to return if interpreter requests
    // string that is out of index range
    static App_String Non_Existent_String;

    /* Check string index - for debugging
    */
    bool CheckStringIndex(CTask *task, int index) const {
	if (index >= m_stringtab_size || index < 0) {
	    CLog::Error("interpreter", task, "String index out of range (%d)", index);
	    return false;
	}
	return true;
    }

    bool CheckDataIndex(CTask *task, int index) const {
	if (index < 0 || index >= m_SS.max ) {
	    CLog::Error("interpreter", task, "Invalid stack/data index %d", index);
	    return false;
	}
	return true;
    }

    /** Initialize exec context with the specified application image.
     */
    bool Init(CT_App_Image *img, const List<String> & cmdline_param);

    /** Attach this app context to a task.
     */
    void Attach (CTask *task) {
	 //m_Task = task;
    }

    /** Get the number of arguments specified when creating task.
     */
    int GetArgCount() const {
	return m_CmdLine.size();
    }

    /** Get the task argument.
     */
    const char * GetArg (int index) {
	if (index >= (int) m_CmdLine.size())
	    return "";
	return m_CmdLine[index].c_str();
    }

    /** Get the application.
     */
    CT_Application * GetApplication() 
    { 
	return m_Application; 
    }

    /** Get the image.
     */
    CT_App_Image * GetAppImage() 
    { 
	return m_AppImage; 
    }

    /** Synchronize stringtab with temporary string tables.
     */
    void GetStringReturnValue();

    /** Copy the contents of string in string table[src_str_tab] to temporary
	API string storage at index[dst_api_tab].
     */
    char* CopyString(unsigned dst_api_tab, unsigned src_str_tab); 

    /** Get the size of the string table.
     */
    size_t GetStringTabSize() const {
	return m_stringtab_size;
    }

    /** Get the contents of string table at position index.
     */
    const char * GetStringTab_Checked(unsigned index) const;
    App_String & GetStringTab(unsigned index);
};


//=============================================================================
//
//  Task: represents executing instance of SL program
//

class CTask {

public:

    // States
    enum TState {
	BORN=0,
	READY,
	BLOCKED,
	DEAD
    };

    CTask (CT_Application *app, Task_Exec_Context *context, App_Reg_Key reg_key,
	   Task_Startup_Type startup_type);
    ~CTask();
    bool IsValid();

    // Get current state 
    TState GetState() const { return m_State; };

    // Get previous state
    TState GetPrevState() const { return m_PrevState; }

    // Get startup type.
    Task_Startup_Type GetStartupType() const { return m_StartupType; }

    // Get state in string 
    const char* GetStrState() const { return StrState(GetState()); };

    // Get PID 
    unsigned int GetPID() const {return m_PID;};

    // Get information structure 
    const CTaskInfo *GetTaskInfo();

    // Set the quit flag 
    STATUS Quit();

    // Check whether quit flag is set 
    bool IsQuitting() const { return m_Quitting; };

    // Execute: put the task in running queue 
    void Execute();

    // Initialize the task instance 
    STATUS Init();

    // Deinitialize the task instance 
    STATUS Shutdown();

    // Enable exception. Any pending exception will be moved to task
    // main event queue (called by Intepreter).
    void EnableException() {--m_ExceptionDisabled;};

    // Disable exception. Any RaiseException() will be queued as
    // pending exception (called by Intepreter).
    void DisableException() { ++m_ExceptionDisabled; };

    // Indicator whether exception is enabled
    bool IsExceptionEnabled() const { return m_ExceptionDisabled == 0; };

    // Raise an exception (can be called by Intepreter, Driver, Task, or 
    // Kernel).
    void RaiseException( TaskExceptionId ex_id, const char *source, const char *detail=NULL );

    // Pop an exception from queue (called by intepreter)
    TaskExceptionId PopException( );

    // Check task current state whether can handle exception or not
    bool CanProcessException() const { return IsExceptionEnabled(); };

    // Set this task's last error 
    void SetLastError(int, int det=0, int sev=0);

    // Get this task's last error 
    int GetLastError();

    // Get this task's last error and the detail error number 
    int GetLastError( int *detail );

    // Get this task's last exception (used by API GetExceptionReason())
    int	GetLastException() const { return m_LastException; };

    // Check whether task has exceptions
    bool HasException() const { return !Q_Exception.empty(); };

    // Put this task in BLOCKED state. Usually only be called by CDriver.
    STATUS Sleep(ServiceProvider *sender);

    // Put this task in READY state. Usually only called by CDriver.
    STATUS Wakeup();

    // Get the value of specified TLS entry 
    #ifdef __x86_64__
    long GetTLSValue(int tls) const { 
	#else
    int GetTLSValue(int tls) const { 
	#endif
	assert(tls>=0 && tls<MAX_TLS);
	return tls>=0 && tls<MAX_TLS ? m_TLS[tls] : -1; 
    };

    // Set the value of specified TLS entry 
    #ifdef __x86_64__
    STATUS SetTLSValue(int tls, long val) { 
	#else
    STATUS SetTLSValue(int tls, int val) { 
	#endif
	assert(tls>=0 && tls<MAX_TLS);
	return tls>=0 && tls<MAX_TLS ? m_TLS[tls]=val, OK : NOT_OK;
    };

    // Get the working directory for this task 
    const char *GetAppDirectory() const;

    // Save function and arguments for later reporting
    // (called by CGlobalAPIProvider)
#if defined(__x86_64__)
    void SaveFunc( CAPIDef *f, long *args, int nargs);
#else
    void SaveFunc( CAPIDef *f, int *args, int nargs);
#endif

    // Get task's execution context
    Task_Exec_Context *GetExecContext() const { return m_ExecContext; }

    // Get task's application configuration
    CT_Application *GetApplication() const { return m_Application; }

    void SetIPCMessage(const char *_msg) {ipc_message = _msg;}
    const char *GetIPCMessage() {return ipc_message.c_str();}
    void SetTaskName(const char *_name) {task_name = _name;}
    const char *GetTaskName() {return task_name.c_str();}

    // Set task state
    void SetState_2( TState state) {m_State = state;};

//-----------------------------------------------------------------------------
// State, exceptions
//

private:
    enum { DEADCODE = 0xDEADC0DE };
    unsigned _signature;

    // Task information structure
    CTaskInfo *m_TaskInfo;
    String ipc_message;
    String task_name;

    // Current state
    TState m_State;

    // Previous state
    TState m_PrevState;

    // Flag to indicate that the task is quitting
    bool m_Quitting;

    // Task PID
    unsigned int	m_PID;

    // Static PID counter
    static unsigned int	m_PIDCounter;

    // Task's TLS array
    #ifdef __x86_64__
    long m_TLS[MAX_TLS];
	#elif __i386__
    int m_TLS[MAX_TLS];
	#endif

    // Task's last error information
    int m_LastError, m_LastErrorDetail;

    // current async api function & arguments
    CAPIDef *m_CurrentFunc;
#if defined(__x86_64__)
    long m_CurrentArgs[API_MAX_ARGS];
#else
    int m_CurrentArgs[API_MAX_ARGS];
#endif

private:
    // Typedef of exception queue
    typedef List <TaskExceptionId> TExceptionQueue;

    // Main exception queue
    TExceptionQueue Q_Exception;
    
    // Put an exception in exception queue
    void PushException( TaskExceptionId ex_id) { 
	Q_Exception.push_back( ex_id ); 
    };

private:

    // Flag to enable/disable exception processing. 
    int		m_ExceptionDisabled;


    // Currently blocked driver
    ServiceProvider	*m_BlockedDriver;

    // Last exception (used by API GetExceptionReason)
    int			m_LastException;

    // Set task state
    void SetState( TState state) {m_State = state;};

    // Get state string description
    static const char* StrState( TState st);

//-----------------------------------------------------------------------------
// Code instruction
//

private:

    // Execution context
    Task_Exec_Context    *m_ExecContext;

    // Application config
    CT_Application	 *m_Application;

    // Application registration key.
    App_Reg_Key		  m_AppRegKey;

    // Startup type.
    Task_Startup_Type	  m_StartupType;

//-----------------------------------------------------------------------------
// Statistics

private:

    // timers
    time_t   m_StartTime;
    Highest_Res_Timer m_TaskTimer;

    // statistic update
    void StatPreBlocked();
    void StatPostBlocked();
    void StatPreWaiting();
    void StatPostWaiting();
    void StatPreExecute();
    void StatPostExecute();

    friend class Kernel;
};


#endif
