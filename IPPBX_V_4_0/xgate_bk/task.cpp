/* $Header: /xGate/task.cpp 32    5/03/05 3:46p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Application task instance.
 *
 * $Log: /xGate/task.cpp $
 * 
 * 32    5/03/05 3:46p Bennylp
 * Added fatal log before crashing in clear_stringtab
 * 
 * 31    3/08/04 7:08p Bennylp
 * Support for different startup type (such as Registrar) and protection
 * to GetStringTab() and CopyString()
 * 
 * 30    12/18/03 1:04p Bennylp
 * Remove debugging during clearing stringtab
 * 
 * 29    3/01/03 7:03p Bennylp
 * Remove App_String
 * 
 * 28    21/02/03 18:10 Junanto
 * added safe application string
 * 
 * 27    1/30/02 12:05p Bennylp
 * Fixed miscelanous bugs related to interpreter.
 * 
 * 26    1/21/02 11:44a Bennylp
 * Ver 38.2 (configurable use of app mem pool)
 * 
 * 25    25/10/01 11:20 Junanto
 * Fixed bug in free memory pool management
 * 
 * 24    24/10/01 15:31 Junanto
 * Fixed leaked memory
 * 
 * 23    24/10/01 15:12 Junanto
 * Tasks' memory and string are no longer deleted on task
 * destruction but returned to a free pool for later use
 * 
 * 22    7/26/01 11:56a Bennylp
 * Fixed bug in waiting time calculation.
 * 
 * 21    7/25/01 3:53p Bennylp
 * Changed low res timer with high res timer.
 * 
 * 20    7/25/01 10:29a Bennylp
 * New HandleEvents() scheduling.
 * 
 * 19    7/16/01 4:53p Bennylp
 * Debug Task_Exec_Context destructor bug.
 * 
 * 18    7/14/01 7:53p Bennylp
 * Reduce friends of class Task_Exec_Context.
 * 
 * 17    28/06/01 12:49 Sonny
 * 
 * 16    28/06/01 11:40 Sonny
 * Fixed bug in Task_Exec_Context destructor in low mem situation.
 * 
 * 15    6/01/01 4:59p Bennylp
 * Reorder methods for optimization.
 * 
 * 14    5/24/01 2:09p Bennylp
 * Added message when task is blocked when shutdown.
 * 
 * 13    17/05/01 12:56 Junanto
 * Changed some xlat table to static
 * 
 * 12    2/05/01 11:09 Junanto
 * 
 * 11    1/05/01 15:51 Junanto
 * Trace output aligned to logging
 * 
 * 10    1/05/01 15:43 Junanto
 * Include source in RaiseException
 * for logging purpose
 * 
 * 9     4/30/01 3:34a Sonny
 * 
 * 8     4/30/01 12:20a Sonny
 * 
 * 7     4/28/01 6:33a Bennylp
 * Added service profiling and database driver (testing)
 * 
 * 6     4/26/01 11:31p Bennylp
 * Added task-event tracing.
 * 
 * 5     4/26/01 1:27a Sonny
 * 
 * 4     4/25/01 6:46a Sonny
 * 
 * 3     4/19/01 2:15p Bennylp
 * Fix memory leaks.
 * 
 * 2     4/12/01 8:08p Bennylp
 * Integration.
 *
 */
#include "stdafx.h"
#include <poll.h>
#include "task.h"
#include "serviceprovider.h"
#include "klog.h"
#include "kernel.h"
#include "api.h"
#include "interpret.h"


// Task's PID incrementor
unsigned int CTask::m_PIDCounter = 0;


// Task identification in log
#undef TASK
#define TASK	"task"


// Task state tracing
// This tracing is created by kernel
Log_Trace *taskTracing;

static Safe_Memory_Allocator allocator;
App_String Task_Exec_Context::Non_Existent_String;

void Sleep( unsigned long ms )
{
#ifndef __linux__
    ::Sleep(ms);
#else
    poll(0,0,ms); // __linux__
#endif
};


/** Task_Exec_Context constructor.
 */
Task_Exec_Context::Task_Exec_Context ( CT_Application *app )
{
    //memset(this, NULL, sizeof(*this));
    m_code = 0;
    m_data = 0;
    m_datasize = 0;
    m_stacksize = 0;

    m_SS.min = m_SS.max = 0;;
    m_IP = m_SP = m_BP = m_EP = 0;

    m_stringidx = 0;
    m_minstridx = 0;
    m_sstack = 0;
    /* IPPBX crash fix. In case of side affects, check this code for sure */
    /* memset(m_apistringtab, 0, sizeof(m_apistringtab)); */
    for (int i = 0; i < API_MAX_ARGS; i++) {
	m_apistringref[i] = -1;
    }

    m_Application = app;
    m_AppImage = 0;
    allocator_ = 0;

    m_stringtab_size = 0;
    m_stringtab = 0;
    stringtab_debug_level = 0;
    m_const_string = 0;

    m_StatAllString = 0;

#ifdef STRINGTAB_CHECK
    m_stringtab_checksum = 0;
#endif
}


void clear_stringtab(App_String stringtab[], unsigned idx)
{
    stringtab[idx].erase();
}

struct Guard_Zone
{
    unsigned signature;
    unsigned size;
};


Task_Exec_Context::~Task_Exec_Context()
{
    if (m_data) {
	// check overflow
	unsigned datasize_in_tmem = (m_datasize/sizeof(TMEM) + 1);
	if (GetApplication() && m_data[datasize_in_tmem].i != 0xdeadc0de) {
	    CLog::Fatal ( "sys", "Stack overflow %s", GetApplication()->Name().c_str());
	}

	free(m_data);
	m_data = 0;
    }

    if (m_stringtab) {

	try {
	    delete [] m_stringtab;
	    m_stringtab_size = 0; // add by ipoeng: 20070413
#ifdef STRINGTAB_CHECK
	    Check_StringTab("Destructor-Deleted(1)", __FILE__, __LINE__, true);
#endif
	}
	catch (...) {
	    CLog::Fatal("SYS", "--FATAL SYSTEM ERROR--: unrecovered access violation: "
			"application=%s, deleting stringtab array. SYSTEM WILL CRASH NOW.",
			this->GetApplication()->Name().c_str());
	    //Sleep(2000);
	    //Sleep(2000);
	    throw;
	}
	m_stringtab = NULL;
    }

    allocator_ = NULL;
}


const char * Task_Exec_Context::GetStringTab_Checked(unsigned index) const
{
    return ((Task_Exec_Context&)(*this)).GetStringTab(index).c_str();
}

App_String & Task_Exec_Context::GetStringTab(unsigned index)
{
    if (index >=m_stringtab_size) {
	assert(false);
/*
	CLog::Fatal ("SYS", "--FATAL RUN-TIME ERROR--: "
			    "Invalid string index (%d) in GetStringTab."
			    , index);
*/
	return Non_Existent_String;
    }

    if (index < m_minstridx && m_const_string[index]) {
	return *m_const_string[index];
    } else {
	return m_stringtab[index];
    }
}

/** Copy the contents of string in string table[src_str_tab] to temporary
    API string storage at index[dst_api_tab].
 */
char* Task_Exec_Context::CopyString(unsigned dst, unsigned src) 
{
    assert(dst < API_MAX_ARGS);
    assert(src < m_stringtab_size);

    if (dst >= API_MAX_ARGS) {
	assert(false);
	CLog::Fatal ("SYS", "--FATAL RUN-TIME ERROR--: "
			    "Invalid destination string index (%d) in CopyString", dst);
	m_apistringtab[API_MAX_ARGS-1][0] = '\0';
	return NULL;
    }

    if (src >= m_stringtab_size) {
	CLog::Fatal ("SYS", "--FATAL RUN-TIME ERROR--: "
			    "Invalid source string index (%d) in CopyString."
			    , src);
	return NULL;
    }

    m_apistringref[dst] = src;
    memset(m_apistringtab[dst], 0, sizeof(m_apistringtab[dst]));
    strncpy(m_apistringtab[dst], m_stringtab[src].c_str(), MAXAPPSTRLEN);
#ifdef STRINGTAB_CHECK
    Check_StringTab("CopyString-Copied(1)", __FILE__, __LINE__, true);
#endif
    m_apistringtab[dst][MAXAPPSTRLEN-1] = '\0';

    return m_apistringtab[dst];
}


/** Synchronize stringtab with temporary string tables.
 */
void Task_Exec_Context::GetStringReturnValue() 
{
    for (int i = 0; i < API_MAX_ARGS; i++) {
	if (m_apistringref[i] == -1) {
	    break;
	}
	else {
	    m_stringtab[m_apistringref[i]] = m_apistringtab[i];
#ifdef STRINGTAB_CHECK
	    Check_StringTab("GetStringReturnValue-Assigned(1)", __FILE__, __LINE__, true);
#endif
	    m_apistringref[i] = -1;
	}
    }
}



/*
    For those who cares...
	Task_Exec_Context::Init() is defined in application.cpp
 */



//-----------------------------------------------------------------------------
// Constructor
//
CTask::CTask (CT_Application *app, Task_Exec_Context *context, App_Reg_Key reg_key,
	      Task_Startup_Type startup_type)
: m_Application(app), m_ExecContext(context), m_AppRegKey(reg_key),
  m_StartupType(startup_type), _signature(DEADCODE)
{
    SetState( BORN );
    m_PrevState = DEAD;

    m_Quitting = false;

    m_PID = m_PIDCounter++;

    memset( m_TLS, 0, MAX_TLS);

    m_LastError = m_LastErrorDetail = 0;
    m_LastException = EX_NO_EXCEPTION;

    m_CurrentFunc = NULL;
    m_TaskInfo = NULL;
    m_TaskTimer.init();

    m_Application->RegisterTask(this, m_AppRegKey);

    CT_TRACE (taskTracing, "[task %u] created", m_PID);
}


//-----------------------------------------------------------------------------
// Task destructor
//
CTask::~CTask()
{
    if (m_TaskInfo){
		delete m_TaskInfo;
		m_TaskInfo = NULL; // make sure set to null to avoid dangling pointer
	}
    m_Application->UnregisterTask (this, m_AppRegKey);
    CT_TRACE (taskTracing, "[task %u] destroyed", m_PID);
    _signature = 0;
}

bool CTask::IsValid() 
{
    try {
	return (_signature == DEADCODE);
    } catch (...) {
	return false;
    }
}

//-----------------------------------------------------------------------------
// Return string description of a state
//
const char* CTask::StrState( TState st)
{
    switch (st) {
    case READY:
	return "READY";
    case BLOCKED:
	return "BLOCKED";
    case DEAD:
	return "DEAD";
    case BORN:
	return "BORN";
    default:
	return "UNKNOWN";
    }
}


//-----------------------------------------------------------------------------
// Initialize the task
// (called by Kernel)
//
STATUS CTask::Init()
{
    assert (m_Application);
    assert (m_ExecContext);

    // create task info
    NEW_RETURN_ (m_TaskInfo, CTaskInfo);
    if (m_TaskInfo == NULL)
	return NOT_OK;

    m_TaskInfo->m_Size = sizeof(CTaskInfo);

    safe_strcpy( m_TaskInfo->m_Name, m_Application->Name().c_str(), MAX_TASK_INFO_NAME);
    m_TaskInfo->m_PID = GetPID();

    
    // fire our timers
    m_StartTime = time(0);
    StatPreWaiting();

    // Init service providers
    if (ServiceProviderManager::Instance()->AttachTask(this) == false)
	return NOT_OK;

    // Init exception state
    m_ExceptionDisabled = 0;

    // Init blocking state
    m_BlockedDriver=0;

    // Task is ready
//Narasimham
    SetState( READY );
    //SetState( BORN );
    return OK;
}


//-----------------------------------------------------------------------------
// Shutdown this task
// ( called by Kernel )
//
STATUS CTask::Shutdown()
{
    if (GetState() == DEAD) {
	// Do nothing
	return OK;
    }

    // Deinitialize drivers
    ServiceProviderManager::Instance()->DetachTask (this);

    // Rechecking and update statistics
    if (GetState() == READY || GetState() == BORN) {

	StatPostWaiting();

    } else if (GetState() == BLOCKED) {

	CLog::Fatal( TASK, this, "Task is blocked by %s when Shutdown is called",
			   m_BlockedDriver->Name().c_str());
	Wakeup();
	StatPostBlocked();

    } else {

	CLog::Fatal( TASK, this, "Task %u has state %u when Shutdown called", GetState());
    }

    // Destroy execution context
    delete m_ExecContext;
    m_ExecContext = NULL;

    // I'm a dead man
    SetState( DEAD );

    return OK;
}


//-----------------------------------------------------------------------------
// save function and arguments for later reporting
// (called by CGlobalAPIProvider)
//
#if defined(__x86_64__)
void CTask::SaveFunc( CAPIDef *f, long *args, int nargs)
#else
void CTask::SaveFunc( CAPIDef *f, int *args, int nargs)
#endif
{

    // validating
    if (nargs>API_MAX_ARGS) {
	CLog::Fatal( TASK, this, "Too many arguments to save for API %s", f->m_Name.c_str());
	m_CurrentFunc = m_TaskInfo->m_LastAPI = 0;
	return;
    }

    // save the function
    m_CurrentFunc = m_TaskInfo->m_LastAPI = f;
    memcpy( m_CurrentArgs, args, nargs*sizeof(long));
}


//-----------------------------------------------------------------------------
// Set task's last error
//
void CTask::SetLastError( int err_no, int detail, int)
{
    m_LastError = err_no;
    m_LastErrorDetail = detail;
}


//-----------------------------------------------------------------------------
// Set the quit flag. When the task is RUNNING next time, it will quit.
//
STATUS CTask::Quit()
{
    CT_TRACE (taskTracing, "[task %u] quitting", m_PID);
    
	if (GetState() == BLOCKED) {

	assert(m_BlockedDriver);

	StatPostBlocked();

	m_BlockedDriver->CancelJob(this);
	CLog::Debug( TASK, this, "Attempt to CancelJob because of quitting");

    }

    m_Quitting = true;
    return OK;
}


//-----------------------------------------------------------------------------
// Statistics: called when task enters READY state
//
inline void CTask::StatPreWaiting()
{
    m_TaskTimer.start();
}


//-----------------------------------------------------------------------------
// Statistics: called when task ends its READY state
//
inline void CTask::StatPostWaiting()
{
    m_TaskTimer.stop();

    unsigned long elapsed = m_TaskTimer.get_elapsed_msec();

    if (elapsed > m_TaskInfo->m_MSLongWaiting) {
	m_TaskInfo->m_MSLongWaiting = elapsed;
    }
}


//-----------------------------------------------------------------------------
// Statistics: called when task enters RUNNING state
//
inline void CTask::StatPreExecute()
{
    StatPostWaiting();

    m_TaskTimer.start();
    m_TaskInfo->m_ExecCnt++;
}


//-----------------------------------------------------------------------------
// Statistics: called when task ends its RUNNING state
//
inline void CTask::StatPostExecute()
{
    m_TaskTimer.stop();

    unsigned long elapsed = m_TaskTimer.get_elapsed_msec();

    m_TaskInfo->m_MSExecTime += elapsed;

    if (elapsed > m_TaskInfo->m_MSLongExec)
	m_TaskInfo->m_MSLongExec = elapsed;
}


//-----------------------------------------------------------------------------
// Statistics: called when task enters BLOCKED state
//
void CTask::StatPreBlocked()
{
    m_TaskTimer.start();
}


//-----------------------------------------------------------------------------
// Statistics: called when task ends its BLOCKED state
//
void CTask::StatPostBlocked()
{
    m_TaskTimer.stop();

    unsigned long elapsed = m_TaskTimer.get_elapsed_msec();

    m_TaskInfo->m_MSBlockedTime += elapsed;

    if (elapsed > m_TaskInfo->m_MSLongBlocked)
	m_TaskInfo->m_MSLongBlocked = elapsed;
}


//-----------------------------------------------------------------------------
// Execute: put this task in RUNNING state
//
void CTask::Execute()
{
    StatPreExecute(); 
    Code_Interpreter::Instance()->Execute(this); 
    m_PrevState = GetState();
    StatPostExecute();

    if (GetState() == READY)
	StatPreWaiting();
}



//-----------------------------------------------------------------------------
// Go BLOCKED, should be called only by drivers
//
STATUS CTask::Sleep(ServiceProvider *sender)
{
    assert(sender);

    // Validating state
    if (GetState() == BLOCKED) {
	CLog::Fatal( TASK,  this, "Task is already BLOCKED when Sleep() is called");
	return NOT_OK;
    }

    CLog::Detail( TASK, this, "Suspended by %s", sender->Name().c_str());

    // Update statistics
    StatPreBlocked();

    // Sleep
    SetState( BLOCKED );
    m_BlockedDriver = sender;

    // Trace
    CT_TRACE (taskTracing, "[task %u] blocked by %s", m_PID, sender->Name().c_str());
    return OK;
}



//-----------------------------------------------------------------------------
// Go READY, called by driver
//
STATUS CTask::Wakeup()
{
    // Trace API call
    if (m_CurrentFunc && m_CurrentFunc->IsValid()) {
	API_Provider_Manager::Instance()->TraceAPICall( this, m_CurrentFunc, m_CurrentArgs, false);
	m_CurrentFunc=0;
	GetExecContext()->GetStringReturnValue();
    }

    // Check state
    if (GetState() != BLOCKED) {
	CLog::Fatal( TASK,  this, "Task is already woken up when Wakeup() is called");
	return NOT_OK;
    }

    // Check exception is done in the interpreter's Execute()


    // Statistics
    StatPostBlocked();
    StatPreWaiting();


    // Inform kernel
    SetState( READY );
    m_BlockedDriver = 0;
    Kernel::Instance()->QueueTask(this);

    // Trace
    CT_TRACE (taskTracing, "[task %u] unblocked, ready for execution", m_PID);
    return OK;
}



//-----------------------------------------------------------------------------
// Collect task's information
//
const CTaskInfo *CTask::GetTaskInfo()
{
    m_TaskInfo->m_Uptime = time(0) - m_StartTime;
    m_TaskInfo->m_Status = (char) m_State;

    if (m_State == BLOCKED) {

	assert(m_BlockedDriver);
	safe_strcpy( m_TaskInfo->m_BlockedDrv, m_BlockedDriver->Name().c_str(), MAX_TASK_INFO_NAME);

    } else {

	m_TaskInfo->m_BlockedDrv[0] = '\0';
    }

    return m_TaskInfo;
}


//-----------------------------------------------------------------------------
// Get task's working directory. Note that the working directory is not the
// operating system's PWD.
//
const char *CTask::GetAppDirectory() const
{ 
    return m_Application->GetWorkingDir().c_str();
}


//-----------------------------------------------------------------------------
// Raise an exception (can be called by Intepreter, Driver, Task, or 
// Kernel).
void CTask::RaiseException( TaskExceptionId ex_id, const char *source, const char *detail ) 
{
    static const char *xlat[] = {
	"NoException",
	"LineDrop",
	"DialStatus",
	"HangUp",
	"IPCMsg",
    };

    const char *exceptionStr = ((int)ex_id<(sizeof(xlat)/sizeof(xlat[0])) ? xlat[ex_id] : "Unknown" );

    CLog::Detail(TASK,this,"Received exception (%s) from %s %s",exceptionStr,source, detail ? detail : "");

    CT_TRACE (taskTracing, "[task %u] exception (%s) from %s %s", m_PID, exceptionStr,source, detail ? detail : "");
    if ( IsExceptionEnabled() && m_BlockedDriver) {
	m_BlockedDriver->CancelJob(this);
    }
    PushException( ex_id );
}


//-----------------------------------------------------------------------------
// Get an exception from exception queue
//
TaskExceptionId CTask::PopException( )
{
    if (!HasException()) {
	m_LastException = EX_NO_EXCEPTION;
	return EX_NO_EXCEPTION;
    }

    TaskExceptionId ex_id = Q_Exception.front();
    Q_Exception.pop_front();

    m_LastException = ex_id;
    CT_TRACE (taskTracing, "[task %u] processing exception %u", m_PID, ex_id);
    return ex_id;
}


//-----------------------------------------------------------------------------
// Get this task's last error 
int CTask::GetLastError() 
{ 
    int err=m_LastError; 
    
    m_LastError = m_LastErrorDetail = 0; 
    return err; 
}


//-----------------------------------------------------------------------------
// Get this task's last error and the detail error number 
int CTask::GetLastError( int *detail ) 
{
    assert(detail);

    int err=m_LastError; 

    *detail=m_LastErrorDetail;
    m_LastError = m_LastErrorDetail = 0;

    return err;
}

