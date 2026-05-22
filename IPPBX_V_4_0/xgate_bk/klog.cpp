/* $Header: /xGate/klog.cpp 42    9/06/04 5:28p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * LOGGING manager.
 *
 * $Log: /xGate/klog.cpp $
 * 
 * 42    9/06/04 5:28p Bennylp
 * Log prints warning if some message can not be printed.
 * 
 * 41    4/02/03 11:15a Bennylp
 * Fixed threading bug in queue's buffer.
 * 
 * 40    3/06/03 5:13p Bennylp
 * Fix memory leak and change tracing to support per user tracing
 * 
 * 39    1/31/03 2:25p Bennylp
 * Changed 'trace' command privilege to Normal
 * 
 * 38    11/20/02 4:56p Bennylp
 * Added mutex protection
 * 
 * 37    10/23/02 5:01p Bennylp
 * Fixed to use new macros in thread_process.h
 * 
 * 36    10/23/02 12:37p Bennylp
 * Modify to use memory pool for the log messages.
 * 
 * 35    22/05/02 17:59 Junanto
 * Application logging to terminal can now be filtered out
 * (to concentrate on system messages only) by a 
 * new parameter "application" in "term logging".
 * By default, application logging is displayed to terminal.
 * To disable it: "no term logging application".
 * 
 * 34    18/01/02 18:34 Junanto
 * Fixed bug when xGate can't create log file
 * 
 * 33    15/11/01 12:03 Bennylp
 * Changed default log verbosity to detail
 * 
 * 32    7/30/01 3:50p Bennylp
 * Added sender info when log format is invalid.
 * 
 * 31    7/25/01 8:06p Bennylp
 * Added more trace functions.
 * 
 * 30    7/17/01 11:47a Bennylp
 * Changed default input verbosity to INFO.
 * 
 * 29    7/17/01 11:46a Bennylp
 * Changed default file settings: num=4, size=5 MB
 * 
 * 28    16/07/01 13:33 Junanto
 * Removed short_sender from trace
 * 
 * 27    6/09/01 5:15p Bennylp
 * Fix bug in exception handler.
 * 
 * 26    6/07/01 5:48p Bennylp
 * Show verbosity in 'show log', and export some functions to klog.h.
 * 
 * 25    2/06/01 14:35 Junanto
 * Changed output format of call trace
 * 
 * 24    6/01/01 11:31a Bennylp
 * Changed log formatting
 * 
 * 23    5/30/01 3:31p Bennylp
* Show jobsize in 'show log' command.
* 
* 22    5/26/01 8:04p Bennylp
* Record each call to AcquireSystemLock in DEBUG mode.
* 
* 21    25/05/01 13:28 Junanto
* 
* 20    5/21/01 5:11p Bennylp
* 
* 19    11/05/01 11:28 Junanto
* Single threaded during debug
* 
* 18    10/05/01 12:40 Junanto
* Replaced "\r\n" sequence to "\n". Telnet translates
* an orphaned "\n" into "\r\n" during output
* 
* 17    10/05/01 11:16 Junanto
* 
* 16    10/05/01 11:12 Junanto
* Replaced all "\r\n" with a constant "\r\n"
* 
* 15    7/05/01 18:12 Junanto
* Increased defaul log size to 8 MB
* 
* 14    5/01/01 9:09p Bennylp
* Fixed bug in log file size information.
* 
* 13    5/01/01 3:28a Bennylp
* Added command to view log files attributes (show log).
* 
* 12    5/01/01 1:06a Bennylp
* Standard trace output, and added command 'viewlog'.
* 
* 11    4/27/01 6:03a Bennylp
* Add CVariantDynamicChoiceValidator.
* 
* 10    26/04/01 12:37 Junanto
* 
* 9     26/04/01 12:04 Junanto
* added choice for logging output display
* 
* 8     4/25/01 7:18a Bennylp
* Add lastlog command.
* 
* 7     25/04/01 10:50 Junanto
* added TraceList, a variant of Trace with va_list argument
* 
* 6     4/18/01 3:39p Bennylp
* Change tracing interface to include log identifier.
* 
* 5     4/17/01 10:04p Bennylp
* Added tracing.
* 
* 4     4/17/01 2:44p Bennylp
* Remove include "configurator.h"
* 
* 3     4/12/01 8:08p Bennylp
* Integration.
*
*/
//#include "stdafx.h"
#include <sys/types.h>
#include <sys/stat.h>	//linux porting

#include "unistd.h"

#include <fcntl.h>
#include "klog.h"
#include "kernel.h"
#include "task.h"
#include "term.h"
#include "thread_process.h"
#include <semaphore.h>
#include <libgen.h>
#include <setjmp.h>
#include <signal.h>
#include <iostream>
using namespace std;

jmp_buf ex_buf__;
struct sigaction act;

sem_t full,empty;
sem_t wd_pool_full,wd_pool_empty;

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define strcmpi strcasecmp


//#ifndef _DEBUG
#define LOG_MTHREAD
//#endif
//#define PRINT_NO_LOG
//

// constants
#define LOG_NAME_FORMAT	    "_LOG-%.3d.TXT"
#define LOG_MAX_NAME_FORMAT 64
#define LOG_MIN_SIZE	    512
#define LOG_NUM		    	4
//#define LOG_SIZE	    	(10000000)
#define LOG_SIZE	    	(20000000)
//#define LOG_DIR		    "c:/switchlab/log/"
#define LOG_DIR		    	"/var/log/xgate/log/"
#define LOG_MAX_QUEUE	    1500	    // max number of worker job queue, after
// which caller thread will block when
// it tries to issue Log()
//#define LOG_WRITE_DATA_SIZE (LOG_MAX_QUEUE+2)
#define LOG_WRITE_DATA_SIZE (LOG_MAX_QUEUE)
#define TRACE_SOURCE	    ((CTask*)-2)
#define NEWLINE		    	"\n"

//#define _mktemp mktemp	//linux porting
#define _mktemp mkstemp		//linux porting
#define _vsnprintf vsnprintf
#define _snprintf snprintf

KLog * KLog::klogInstance;

/* Command identification.
*/
enum Log_Term_Cmd_Id {
	Log_Term_Cmd_Logging,
	Log_Term_Cmd_Verbosity,
	Log_Term_Cmd_File,
	Log_Term_Cmd_Trace,
	Log_Term_Cmd_LastLog,
	Log_Term_Cmd_ViewLog,
	Log_Term_Cmd_LogFile,
};


/** Log level description.
Note: the first character in the shortName must be unique, because it will
be examined by PrintBlockContents(). And if it is changed, then the
PrintBlockContents() must be changed too.
*/
static struct {

	LogVerbosity level;		    // verbosity level
	const char  *shortName;	    // identifier that will appear in the log message
	const char  *levelName;	    // log name that will appear in commands
	const char  *levelDescription;  // description of the level

} Log_StrLevel[] = {
	{ LOG_DETAIL, "x",   "detail",	"Show all level of debugging" },
	{ LOG_DEBUG,  "d",   "debug",	"Show debug and above level messages only" },
	{ LOG_INFO,   "i",   "info",	"Show informational and above level messages only" },
	{ LOG_WARNING,"w",   "warning",	"Show warning and above level messages only"},
	{ LOG_ERRORS, "e",   "error",	"Show error and fatal messages only" },
	{ LOG_FATAL,  "f",	 "fatal",	"Show only fatal errors"}
};


/** Convert a string into log verbosity level.
*/
LogVerbosity Log_StrToLevel (const char *str)
{
	for (unsigned i=0; i<sizeof(Log_StrLevel)/sizeof(Log_StrLevel[0]); ++i) {
		if (!stricmp(str, Log_StrLevel[i].levelName))
			return Log_StrLevel[i].level;
	}
	assert(false);
	return LOG_INFO;
}

/** Get the short name of a log verbosity.
*/
const char * Log_LevelToStr (LogVerbosity level, const char **desc)
{
	for (unsigned i=0; i<sizeof(Log_StrLevel)/sizeof(Log_StrLevel[0]); ++i) {
		if (level == Log_StrLevel[i].level) {
			if (desc) *desc = Log_StrLevel[i].levelDescription;
			return Log_StrLevel[i].shortName;
		}
	}
	if (level == LOG_TRACE) return "t";

	assert(false);
	return "info";
}


/** Get the name of the log verbosity.
*/
const char * Log_LevelToName (LogVerbosity level, const char **desc)
{
	for (unsigned i=0; i<sizeof(Log_StrLevel)/sizeof(Log_StrLevel[0]); ++i) {
		if (level == Log_StrLevel[i].level) {
			if (desc) *desc = Log_StrLevel[i].levelDescription;
			return Log_StrLevel[i].levelName;
		}
	}
	if (level == LOG_TRACE) return "t";

	assert(false);
	return "info";
}


//-----------------------------------------------------------------------------
// Log rotator
//-----------------------------------------------------------------------------
	CLogRotate::CLogRotate()
: m_CurrentIndex(0), m_MaxIndex(-1), m_CurrentSize(0), m_MaxSize(-1),
	m_CurrentFile(0)
{
}


//-----------------------------------------------------------------------------
// Destructor
//
CLogRotate::~CLogRotate()
{
	CloseFile();
}


//-----------------------------------------------------------------------------
// Initialize log rotator
//
//STATUS CLogRotate::Init( const char *dir, const char *pattern, size_t max_file, 
bool CLogRotate::Init( const char *dir, const char *pattern, size_t max_file, 
		size_t max_size)
{
	if (max_file < 1)
		return NOT_OK;

	if (max_size < LOG_MIN_SIZE)
		return NOT_OK;

	m_Dir = dir;
	m_FilePattern = pattern;
	m_MaxIndex = max_file;
	m_MaxSize = max_size;

	CloseFile();

	return GetFirstFile();
}


//-----------------------------------------------------------------------------
// Shutdown log rotator
//
bool CLogRotate::Shutdown()
{
	return CloseFile();
}


//-----------------------------------------------------------------------------
// Write to the log file
//
bool CLogRotate::__Write( LogVerbosity, size_t length, const char *buf )
{
	if (!m_CurrentFile) return NOT_OK;

	if ((m_CurrentSize += length) > m_MaxSize)
		if (GetNextFile() == NOT_OK)
			return NOT_OK;

	int ret = fputs( buf, m_CurrentFile);
	fflush( m_CurrentFile );

	return ret != EOF ? OK : NOT_OK;
}


//-----------------------------------------------------------------------------
// Open the first log file
//
STATUS CLogRotate::GetFirstFile()
{
	String filepath;
	char filename[LOG_MAX_NAME_FORMAT];
	time_t *file_time = new time_t[m_MaxIndex+1];

	// fill the file modification time array
	for (unsigned int i=1; i<=m_MaxIndex; ++i) {
		struct stat st;

		filepath = m_Dir;
		_snprintf( filename, LOG_MAX_NAME_FORMAT, m_FilePattern.c_str(), i);
		filepath += filename;

		if ( stat( filepath.c_str(), &st) )
			file_time[i] = 0;
		else
			file_time[i] = st.st_mtime;
	};

	// determine which is the oldest
	unsigned int choosen_index = 1;
	for (unsigned int j=2; j<=m_MaxIndex; j++) {
		if (file_time[j] < file_time[choosen_index])
			choosen_index = j;
	}

	delete [] file_time;

	return OpenFile(choosen_index);
}


//-----------------------------------------------------------------------------
// Open the log file
//
STATUS CLogRotate::OpenFile( unsigned int index )
{
//	printf("************* Inside %s ************\n",__FILE__);
	String filepath;
	char filename[LOG_MAX_NAME_FORMAT];


	// construct file full path
	filepath = m_Dir;
	_snprintf( filename, LOG_MAX_NAME_FORMAT, m_FilePattern.c_str(), index);
	time_t timenow;
	timenow = time(0);
	struct tm *tmbuff;
	tmbuff = localtime(&timenow);
	char current_time[100];
	strftime(current_time, sizeof(current_time),"%d_%m_%y-%H%M",tmbuff);
	String filename1;
	filename1 = current_time;
	filename1 = filename1 + filename;
	filepath += filename1;


	// open the file
	m_CurrentFile = fopen( filepath.c_str(), "wt");
	if (!m_CurrentFile)
		return NOT_OK;

	// adjust index
	m_CurrentIndex = index;
	m_CurrentSize = 0;

	// write signature
	time_t t = time(0);
	String str_start = ctime(&t);
	str_start.erase ( str_start.size()-1, 1);
	fprintf( m_CurrentFile, "Log created at %s" NEWLINE, str_start.c_str());

	return OK;
}


//-----------------------------------------------------------------------------
// CLose the log file
//
STATUS CLogRotate::CloseFile()
{
	if (m_CurrentFile) {
		fclose(m_CurrentFile);
		m_CurrentFile = 0;
	}
	return OK;
}


//-----------------------------------------------------------------------------
// Open next log file
//
STATUS CLogRotate::GetNextFile()
{
	unsigned int next_index = (m_CurrentIndex==m_MaxIndex) ? 1 : m_CurrentIndex+1;

	CloseFile();
	return OpenFile(next_index);
}


//-----------------------------------------------------------------------------
// KLog Constructor
//
KLog::KLog()
	: ServiceProvider("logging", false, false), m_VInput( LOG_DETAIL ), m_ErrorOnThread(false),
	m_WriteDataPool(LOG_WRITE_DATA_SIZE)
{
	__m_LastErrMsg[0] = '\0';
	klogInstance = this;
	isLoaded = false;

	thread_running_ = false;
	thread_id_ = 0;
	thread_hnd_ = NULL;
	m_MisedLogs = 0;
	m_LastMissed = time(NULL);
}


//-----------------------------------------------------------------------------
// KLog Destructor
//
KLog::~KLog()
{
	klogInstance = NULL;
}

//-----------------------------------------------------------------------------
bool KLog::IsThreadRunning() const
{
	return thread_running_ ? true : thread_hnd_ != NULL;
}

//-----------------------------------------------------------------------------
class Trace_Name_Validator : public CVariantDynamicChoiceValidator {
	public:
		/** Default constructor. */
		Trace_Name_Validator()
		{}

		/** Destructor. */
		virtual ~Trace_Name_Validator() {}

		/** Copy constructor */
		Trace_Name_Validator ( const Trace_Name_Validator & rhs)
			: CVariantDynamicChoiceValidator(rhs)
		{}

		/** Assignment operator */
		virtual const Trace_Name_Validator & operator =(const Trace_Name_Validator & rhs)
		{
			CVariantValidator::operator = (rhs);
			return *this;
		}

		/** Create a duplicate */
		virtual CVariantValidator *Clone() const
		{
			return new Trace_Name_Validator(*this);
		}

		/** Acquire mutex lock.
		  This function will be called by the framework BEFORE calling
		  GetChoiceList(). The lock will be kept until this class is finished
		  processing all the choices in the choice list.
		  */
		virtual void AcquireLock() const {
			KLog::Instance()->Trace_Lock();
		}

		/** Release mutex lock.
		  This function will be called by the framework AFTER calling
		  GetChoiceList().
		  */
		virtual void ReleaseLock() const {
			KLog::Instance()->Trace_Unlock();
		}

		/** Get list of choices available.
		*/
		virtual const List<ValidatorChoice> & GetChoiceList() const {
			return KLog::Instance()->Trace_GetList();
		}
};


//-----------------------------------------------------------------------------
// Read configurations and opens the log devices
//
bool KLog::Load()
{
	// Log is special because it needs to be loaded before any other SPs. So
	// Kernel will manually Load this instance. We need this flag because this
	// instance will be Load()-ed again by ServiceProviderManager
	//
//	printf( "***************** loading klog *******************\n");
	if (isLoaded)
		return true;

	unsigned int i;
	
/*	if(pthread_mutex_init(&m_WriteDataPoolMutex, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return false;
    }*/

	
	sem_init(&empty,0,LOG_MAX_QUEUE);
	sem_init(&full,0,0);

	sem_init(&wd_pool_full,0,0);
	sem_init(&wd_pool_empty,0,0);

	// Fill up WriteData pool.
//	for (i=0; i < LOG_WRITE_DATA_SIZE-1; ++i) {
	for (i=0; i < LOG_WRITE_DATA_SIZE; ++i) {

		WriteData *wd = new WriteData(Application);
		if (!wd)
			return false;
//		pthread_mutex_lock(&m_WriteDataPoolMutex);
		m_WriteDataPool.push_back(wd);
		sem_post(&wd_pool_full);
//		pthread_mutex_unlock(&m_WriteDataPoolMutex);

	}
	
//	pthread_mutex_lock(&m_WriteDataPoolMutex);
	unsigned int dummy = m_WriteDataPool.size();
//	pthread_mutex_unlock(&m_WriteDataPoolMutex);

//	printf("**** Inside kload , dummy is %d ********\n", dummy);

	// Setting log rotator
	// Must been done before thread is started
	__m_LogRotator.Init( LOG_DIR, LOG_NAME_FORMAT, LOG_NUM, LOG_SIZE);
	__m_Devices.push_back( &__m_LogRotator );

	// Initialize output verbosities
	__m_VOutput ["file"] = LOG_DETAIL;
	__m_VOutput ["console"] = LOG_DETAIL;


	// Create tree item for the logging.
	m_LoggingTree = new Cfg_Tree_Group ("logging", 
			"Logging configuration",
			Kernel::Instance()->RootConfig());

	// Register Show handler for the logging tree.
	m_LoggingTree->RegisterShowHandler(this);
	Kernel::Instance()->RootConfig()->RegisterShowHandler (this);


	//
	// Trace command.
	//
	Term_Cmd *cmd_trace = new Term_Cmd ( this,
			Log_Term_Cmd_Trace,
			"trace",
			"Enable/disable context sensitive tracing",
			PrivilegeLevelUser,
			TermModeAll,
			true);
	cmd_trace->Add ( new Term_Cmd_Arg ("tracename", CVariant(""), Trace_Name_Validator()));
	RegisterTermCmd (cmd_trace, Kernel::Instance()->RootConfig());

	//
	// Add command to enter to logging tree.
	//
	Term_Cmd *cmd_log = new Term_Cmd ( this,
			Log_Term_Cmd_Logging,
			"logging",
			"Logging parameters",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
	RegisterTermCmd (cmd_log, Kernel::Instance()->RootConfig());

	//
	// Add verbosity command
	//
	// Create the command
	Term_Cmd *cmd_verb = new Term_Cmd (	this, 
			Log_Term_Cmd_Verbosity, 
			"verbosity",
			"Set logging verbosity level",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);

	// Create choices for the first parameter
	CVariantChoiceValidator vDtor("input", "Input filter verbosity");
	vDtor.Add ("file", "File output verbosity");
	vDtor.Add ("console", "Console output verbosity");
	cmd_verb->Add ( new Term_Cmd_Arg( "device", CVariant(""), vDtor) );

	// Create choices for the second parameter
	CVariantChoiceValidator lvVdtor(Log_StrLevel[0].levelName, Log_StrLevel[0].levelDescription);
	for (i=1; i<sizeof(Log_StrLevel)/sizeof(Log_StrLevel[0]); ++i)
		lvVdtor.Add (Log_StrLevel[i].levelName, Log_StrLevel[i].levelDescription);
	cmd_verb->Add ( new Term_Cmd_Arg( "level", CVariant(""), lvVdtor));

	// register the command to the logging tree
	RegisterTermCmd ( cmd_verb, m_LoggingTree );


	//
	// File setting command.
	//
	// Create the command.
	Term_Cmd *cmd_file = new Term_Cmd ( this,
			Log_Term_Cmd_File,
			"file",
			"Set logging file parameters",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);

	// Create choice for the first parameter
	CVariantChoiceValidator fileVdtor ( "maxnum", "Number of files to use in round-robin fashion");
	fileVdtor.Add ( "maxsize", "Maximum size of each file (in bytes)");
	cmd_file->Add ( new Term_Cmd_Arg( "cmd", CVariant(""), fileVdtor) );

	// Create input for the second parameter.
	cmd_file->Add ( new Term_Cmd_Arg("num", CVariant(0), CVariantValidator(0, "Value")) );

	// Register the command.
	RegisterTermCmd (cmd_file, m_LoggingTree);

	//
	// Lastlog command.
	//
	Term_Cmd *cmd_lastlog = new Term_Cmd (this,
			Log_Term_Cmd_LastLog,
			"lastlog",
			"View the last log messages (latest message first)",
			PrivilegeLevelUser,
			TermModeNormal,
			false);
	cmd_lastlog->Add ( new Term_Cmd_Arg ("lines", CVariant(0), 
				CVariantRangeValidator(1, 5000, "Number of last messages to view")));
	cmd_lastlog->Add ( new Term_Cmd_Arg ("verbosity", CVariant("info"), lvVdtor, TermArgOptional) );
	RegisterTermCmd (cmd_lastlog, Kernel::Instance()->RootConfig());

	//
	// Viewlog command.
	//
	Term_Cmd *cmd_viewlog = new Term_Cmd (this,
			Log_Term_Cmd_ViewLog,
			"viewlog",
			"View the contents of log file",
			PrivilegeLevelUser,
			TermModeNormal,
			false);
	cmd_viewlog->Add (new Term_Cmd_Arg("start", 
				CVariant(0),
				CVariantValidator(0, "Start file pos (minus for relative from EOF)"),
				TermArgOptional));
	cmd_viewlog->Add (new Term_Cmd_Arg("end", 
				CVariant(-1),
				CVariantValidator(-1, "End bytes position in the file (-1 for EOF)"),
				TermArgOptional));
	cmd_viewlog->Add (new Term_Cmd_Arg("fname", 
				CVariant(""),
				CVariantValidator("", "Log file name (empty string for current file)"),
				TermArgOptional));
	RegisterTermCmd (cmd_viewlog, Kernel::Instance()->RootConfig());


	//
	// Logfile command.
	//
	/*
	   Term_Cmd *cmd_logfile = new Term_Cmd (this,
	   Log_Term_Cmd_LogFile,
	   "logfile",
	   "Show log files information",
	   PrivilegeLevelUser,
	   TermModeNormal,
	   false);
	   RegisterTermCmd (cmd_logfile, Kernel::Instance()->RootConfig());
	   */
#ifndef __linux__
#ifdef LOG_MTHREAD
	// start thread
	//if (SPAWN_THREAD(&thread_proc, this, THR_NEW_LWP|THR_JOINABLE, &thread_id_, &thread_hnd_)) {
	//void* func;
	if (SPAWN_THREAD(this, THR_NEW_LWP|THR_JOINABLE, &thread_id_, &thread_hnd_)) {
		//MessageBox( NULL, "xGate error", "Unable to start log thread", 0);
		CLog::Info(NULL, "xGate error", "Unable to start log thread", 0);
		thread_hnd_ = NULL;
		return false;
	}
#endif
#else
	if( pthread_create(&thread_hnd_,NULL,&thread_proc,(void *) this) != 0) {  
		/* thread_process argument passed here is NULL, CHANGE according 
		   to the actual scenario
		   */
		CLog::Info(NULL, "xGate error", "Unable to start log thread", 0);
		thread_hnd_ = NULL;
		return false;

	}
#endif //Linux


	isLoaded = true;
	return true;
}


//-----------------------------------------------------------------------------
// Attach a log device
//
//STATUS KLog::Attach( CLogDevice *dev )
STATUS KLog::Attach( CLogDevice *dev )
{
	if (IsThreadRunning())
		PutMessage(MSG_ATTACH_DEVICE, new AttachDeviceData(dev));
	else{
		//return __ProcessMessage(LogMsg(MSG_ATTACH_DEVICE, new AttachDeviceData(dev)))==1? OK : NOT_OK;
		LogMsg msg(MSG_ATTACH_DEVICE, new AttachDeviceData(dev));
		return __ProcessMessage(msg);
	}
	return OK;
}


//-----------------------------------------------------------------------------
// Detach a log device
// Note: this must be done synchronously because caller might delete log device
// right after calling this function. And this also must remove previous
// MSG_ATTACH_DEVICE message if there's any in the message queue.
//
STATUS KLog::Detach( CLogDevice *dev )
{
	if (IsThreadRunning()) {

		m_DeviceMutex.acquire();
		__m_Devices.remove(dev);
		m_DeviceMutex.release();

		AttachDeviceData data(dev);
		RemoveMessage( MSG_ATTACH_DEVICE, data);

	} else {

		__m_Devices.remove(dev);

		AttachDeviceData data(dev);
		RemoveMessage( MSG_ATTACH_DEVICE, data);

	}

	return OK;
}

/** Allocate one WriteData instance from the static buffer.
*/
KLog::WriteData *KLog::AllocWriteData(SenderType senderType)
{

    //ACE_Guard<ACE_Thread_Mutex> guard(m_WriteDataPoolMutex);
	unsigned int dummy = 0;
	int ret = 0;

//	m_WriteDataPoolMutex.acquire();
//	pthread_mutex_lock(&m_WriteDataPoolMutex);
	ret = sem_trywait(&wd_pool_full);
	if(ret == -1)
		return NULL;
	dummy = m_WriteDataPool.size();
//	printf("******* AllocWriteData , dummy is %d *****\n", dummy);
	if(dummy <= 500){
		cout << "datapool size is going to become less than 500: size is " <<  dummy << endl;
	} 

	if (m_WriteDataPool.empty()){
//		m_WriteDataPoolMutex.release();
		cout << "writeDataPool is empty ******** "<< endl;
//		pthread_mutex_unlock(&m_WriteDataPoolMutex);
		sem_post(&wd_pool_full);
		return NULL;
	}

	WriteData *wd = m_WriteDataPool.front();
	m_WriteDataPool.pop_front();
//	m_WriteDataPoolMutex.release();
//	pthread_mutex_unlock(&m_WriteDataPoolMutex);
	sem_post(&wd_pool_empty);	

	wd->m_SenderType = senderType;
	return wd;
}

/** Return WriteData instance.
*/
void KLog::ReturnWriteData(WriteData *data)
{
//	ACE_Guard<ACE_Thread_Mutex> guard(m_WriteDataPoolMutex);

//	pthread_mutex_lock(&m_WriteDataPoolMutex);
	if(sem_trywait(&wd_pool_empty) == 0){
		m_WriteDataPool.push_back(data);
		sem_post(&wd_pool_full);	
	}
//	pthread_mutex_unlock(&m_WriteDataPoolMutex); 

}

//-----------------------------------------------------------------------------
// Shutdown the log
//
bool KLog::Unload()
{
	m_LoggingTree->UnregisterShowHandler(this);
	Kernel::Instance()->RootConfig()->UnregisterShowHandler(this);

	// send quit message
	PutMessage( MSG_QUIT, new QuitData);

	// Wait logger thread to shutdown
	if (IsThreadRunning()) {
		ACE_Thread::join(thread_hnd_);
		thread_hnd_ = NULL;
	}

	// Delete write data pool.
	while (!m_WriteDataPool.empty()) {
		delete m_WriteDataPool.front();
		m_WriteDataPool.pop_front();
	}
	isLoaded = false;
	return true;
}



void KLog::Cmd_Logging(Term_Session *session)
{
	session->SetRoot (m_LoggingTree);
}

void KLog::Cmd_SetVerbosity ( Term_Session *session, const String & device, const String & str_level )
{
	LogVerbosity level = Log_StrToLevel (str_level.c_str());

	ACE_Guard <ACE_Token> guard1(m_LogMsgMutex);
	ACE_Guard <ACE_Token> guard2(m_DeviceMutex);

	if (!stricmp(device.c_str(), "input")) {
		m_VInput = level;
	}
	else {
		Map <String, LogVerbosity>::iterator it;
		it = __m_VOutput.find (device);
		if (it == __m_VOutput.end()) {
			assert(false);
			// TODO:
			//  should unlock mutexes before sending data through socket
			session->SendErrorf ("Invalid device name '%s'" NEWLINE, device.c_str());
			return;
		}
		it->second = level;
	}
}


void KLog::Trace_Lock()
{
	m_DeviceMutex.acquire();
}

void KLog::Trace_Unlock()
{
	m_DeviceMutex.release();
}

const List<ValidatorChoice> & KLog::Trace_GetList()
{
	return m_TraceChoiceList;
}

/** Find a trace device.
*/
Log_Trace * KLog::Trace_Find (const char *name)
{
	List <Log_Trace*>::iterator it, end;
	end = __m_Traces.end();
	for (it = __m_Traces.begin(); it!=end; ++it) {
		if (!stricmp((*it)->traceName.c_str(), name))
			return *it;
	}
	return NULL;
}

/** Enable/disable tracing.
*/
void KLog::Trace_Set_Enabled (const char * name, bool enabled)
{
	ACE_Guard <ACE_Token> guard2(m_DeviceMutex);
	Log_Trace *trace = Trace_Find(name);
	if (trace) {
		trace->traceEnabled = enabled ? 1 : 0;
	}
}


/** Show tracing.
*/
void KLog::Trace_Show (Term_Session *session)
{
	String output("Tracing:" NEWLINE);
	const String indent = "  ";

	m_DeviceMutex.acquire();
	List <Log_Trace*>::const_iterator it, end;
	end = __m_Traces.end();
	for (it = __m_Traces.begin(); it!=end; ++it) {
		Log_Trace *trace = *it;
		if (trace->traceEnabled)
			output += indent + trace->traceName + NEWLINE;
	}
	m_DeviceMutex.release();

	session->Send(output);
}

void KLog::Cmd_SetFile ( Term_Session *session, const String & setting, int value )
{
#define ONE_GIG	1000000000
	ACE_Guard <ACE_Token> guard1(m_LogMsgMutex);
	ACE_Guard <ACE_Token> guard2(m_DeviceMutex);

	//printf("TEST int value = %d\n", value); //rkv testing

	if (value < 1 || value > ONE_GIG) {
		return;
	}

	if (!stricmp(setting.c_str(), "maxnum"))
		__m_LogRotator.SetMaxFileNum (value);
	else if (!stricmp(setting.c_str(), "maxsize"))
		__m_LogRotator.SetMaxFileSize (value);
	else {
		assert(false);
		// TODO:
		//  should unlock mutexes before sending data through socket
		session->SendErrorf ("Invalid argument '%s'" NEWLINE, setting.c_str());
	}
}


/* Select log messages with the specified verbosity (or above) from a block
   of bytes.
   */
static bool PrintBlockContents ( Term_Session *session,
		LogVerbosity level,
		char *block,
		size_t block_size,
		unsigned max_lines,
		unsigned & lines_printed)
{
	static struct StrLevel2Verbosity {
		int level__[256];
		StrLevel2Verbosity() {
			memset (level__, 0, sizeof(level__));
			level__['x'] = 0;
			level__['d'] = 1;
			level__['i'] = 2;
			level__['w'] = 3;
			level__['e'] = 4;
			level__['E'] = 4;
			level__['f'] = 5;
			level__['F'] = 5;
		}
	} strLevel2Verbosity;

	assert (block_size > 0);
	char *p_end = block + (block_size-1);
	char *p_start=p_end;

	do {
		// get the beginning of the line.
		while (p_start > block && (*p_start=='\r' || *p_start=='\n'))
			--p_start;
		while (p_start > block && *p_start!='\r' && *p_start!='\n')
			--p_start;

		// find the position of the verbosity identifier, by
		// finding the first alpha char.
		char *ch_verbosity = p_start+1;
		while (!isalpha(*ch_verbosity) && *ch_verbosity!='\n')
			++ch_verbosity;

		// examine the verbosity.
		if (strLevel2Verbosity.level__[*ch_verbosity] < level) {
			p_end = p_start;
			continue;
		}

		++lines_printed;
		if (!session->Send ( p_start+1, (p_end-p_start)+1))
			return false;
		p_end = p_start;

	} while (p_start > block && lines_printed < max_lines);
	return true;
}

/* Select log messages with the specified verbosity (or above) from a file,
   and display them to the session. This will print the messages in newer first
   order.
   */
static bool PrintLogContents ( Term_Session *session, 
		const char *filename, 
		LogVerbosity level,
		unsigned max_lines,
		unsigned & lines_printed)
{
	int fhnd = open( filename, _O_RDONLY | _O_BINARY);
	if (fhnd == -1) 
		return false;

	enum {
		BLOCK_PROCESSING_SIZE	= 2048
	};

	char *block;
	NEW_RETURN_ (block, char[BLOCK_PROCESSING_SIZE+1]);
	if (block == NULL)
		goto on_error;

	long last_block_pos;
	size_t block_size;

	if ((last_block_pos = lseek(fhnd, 0, SEEK_END)) == -1)
		goto on_error;

	block_size = BLOCK_PROCESSING_SIZE;

	while (lines_printed < max_lines && last_block_pos > 0) {
		if (last_block_pos < BLOCK_PROCESSING_SIZE) {
			block_size = last_block_pos;
			last_block_pos = lseek(fhnd, 0, SEEK_SET);
		} else {
			last_block_pos = lseek (fhnd, last_block_pos - BLOCK_PROCESSING_SIZE, SEEK_SET);
		}
		if (last_block_pos == -1)
			break;

		if (read (fhnd, block, block_size) != block_size)
			goto on_error;
		block[block_size] = '\0';

		// align start block to the next line posision.
		char *start_block = block, *end_block = block + block_size;
		if (last_block_pos != 0) {
			while (start_block != end_block && *start_block != '\r' && *start_block != '\n')
				++start_block;
			while (start_block != end_block && (*start_block == '\r' || *start_block == '\n'))
				++start_block;
			block_size -= (start_block - block);
			last_block_pos += (start_block - block);
			if (block_size == 0)
				break;
		}

		if (PrintBlockContents (session, level, start_block, block_size, max_lines, lines_printed) == false)
			goto on_fatal_error;
	}


on_error:
	delete [] block;
	close(fhnd);
	return true;

on_fatal_error:
	delete [] block;
	close(fhnd);
	return false;
}

/* Utility: verify the existance of the file.
*/
static bool IsFileExist(const char *filename)
{
	FILE *fhnd = fopen (filename, "rb");
	if (fhnd == NULL)
		return false;
	fclose(fhnd);
	return true;
}

/* Utility: copy file 
*/

/** Command to view the last messages printed.
*/
void KLog::Cmd_LastLog ( Term_Session *session, unsigned max_lines, LogVerbosity level)
{
	unsigned lines = 0;

	ACQUIRE_SYSTEM_LOCK();
	unsigned curr_file_idx = __m_LogRotator.GetCurrentIndex();
	unsigned max_file_num = __m_LogRotator.GetMaxFileNum();
	RELEASE_SYSTEM_LOCK();

	String temp_path;
	if (getenv("TEMP"))
		temp_path = getenv("TEMP");
	else
		temp_path = "/tmp";
	temp_path.append (1, '/');


	unsigned n_file_processed;
	char filetemplate[128];
	strcpy (filetemplate, "logXXXX");

	for (n_file_processed = 0; n_file_processed < max_file_num; ++n_file_processed) {

		// Create current log filename.
		char logfilename[128];
		sprintf (logfilename, LOG_DIR LOG_NAME_FORMAT, curr_file_idx);
		if (!IsFileExist(logfilename))
			break;

		// Create a temporary, unique filename.
		char filepath[256];
		char *filename = _mktemp(filetemplate);
		_snprintf (filepath, 128, "%sxGate-%s", temp_path.c_str(), filetemplate);

		// Copy current log file to temporary file.
		if (CopyFile(logfilename, filepath) == 0) {	
			session->SendErrorf ( "Error copying file %s", logfilename );
			break;
		}

		// Process the file.
		bool status = PrintLogContents(session, filepath, level, max_lines, lines);

		// Delete the temporary file.
		//DeleteFile (filepath);
		remove(filepath);	//linux porting

		if (status==false || lines>=max_lines)
			break;

		if (curr_file_idx == 1)
			curr_file_idx = max_file_num;
		else
			--curr_file_idx;
	}
}


/** Command to view the contents of the log file.
*/
void KLog::Cmd_ViewLog ( Term_Session *session, const char *fname, int start, int end)
{
	char logfilename[_MAX_PATH];

	// format the filename
	if (*fname == '\0') {
		ACQUIRE_SYSTEM_LOCK();
		unsigned curr_file_idx = __m_LogRotator.GetCurrentIndex();
		RELEASE_SYSTEM_LOCK();

		sprintf (logfilename, LOG_DIR LOG_NAME_FORMAT, curr_file_idx);
	} else {
		sprintf( logfilename, LOG_DIR "%s", fname);
	}

	// check file exist
	if (!IsFileExist(logfilename)) {
		session->SendErrorf ("File '%s' doesn't exist" NEWLINE, logfilename);
		return;
	}

	// create a temporary, unique filename.
	String temp_path;
	if (getenv("TEMP"))
		temp_path = getenv("TEMP");
	else{
#ifndef __linux__
		temp_path = "C:/TEMP";
#else
		temp_path = "/tmp";
#endif
	} 

	temp_path.append (1, '/');

	char filetemplate[128];
	strcpy (filetemplate, "logXXXX");
	char filepath[_MAX_PATH];
	_mktemp(filetemplate);
	_snprintf (filepath, 128, "%sxGate-%s", temp_path.c_str(), filetemplate);

	// copy current log file to temporary file.
	//if (::CopyFile(logfilename, filepath, TRUE) == 0) {
	if (CopyFile(logfilename, filepath) == 0) {	
		session->SendErrorf ( "Error copying file '%s' to '%s'" NEWLINE, logfilename, filepath );
		return;
	}

	// open the file.
	int fhnd = open( filepath, _O_RDONLY | _O_BINARY);
	if (fhnd < 0) {
		session->SendErrorf ("Unable to read file '%s'" NEWLINE, filepath);
		return;
	}

	// calculate start/end.
	if (start < 0)
		start = filelength(fhnd) + start;
	if (end == -1)
		end = filelength(fhnd);
	else if (end < 0)
		end = filelength(fhnd) + end;

	// read buffer.
	enum {
		BLOCK_SIZE = 1024
	};
	char buffer[BLOCK_SIZE+1];

	// move file position to start pos.
	if (lseek(fhnd, start, SEEK_SET) < 0) {
		session->SendErrorf ("Invalid start position %d" NEWLINE, start);
		goto on_return;
	}

	// loop read and send the file contents.
	while (start < end) {
		size_t block_size;
		if (end - start >= BLOCK_SIZE)
			block_size = BLOCK_SIZE;
		else
			block_size = end - start;
		if (read(fhnd, buffer, block_size) != block_size)
			goto on_return;
		buffer[block_size] = '\0';
		if (session->Send (buffer, block_size) == false)
			goto on_return;
		start += block_size;
	}


on_return:
	close(fhnd);

	// Delete the temporary file.
	//DeleteFile (filepath);
	remove(filepath);
}

#define GENERIC_READ  (0x80000000L)

/** Command to view the log files attributes.
*/
void KLog::Cmd_LogFile ( Term_Session * session )
{
	ACQUIRE_SYSTEM_LOCK();
	unsigned curr_file_idx = __m_LogRotator.GetCurrentIndex();
	unsigned max_file_num = __m_LogRotator.GetMaxFileNum();
	unsigned max_file_size = __m_LogRotator.GetMaxFileSize();
	RELEASE_SYSTEM_LOCK();

	m_LogMsgMutex.acquire();
	unsigned job_size=m_LogMsg.size();
	m_LogMsgMutex.release();

	BY_HANDLE_FILE_INFORMATION info;

	session->Sendf (
			"Max. number of files:  %u" NEWLINE
			"Max. file size      :  %u" NEWLINE
			"Current file index  :  %u" NEWLINE
			"Current log job size:  %u" NEWLINE
			"Max. job size limit :  %u" NEWLINE
			"Input verbosity     :  %s" NEWLINE NEWLINE,
			max_file_num, max_file_size, curr_file_idx, job_size, LOG_MAX_QUEUE,
			Log_LevelToName(m_VInput));

	session->Sendf ( "  File size   Last write time    File name" NEWLINE
			"=============================================================" NEWLINE);

	char logfilename[_MAX_PATH];
	unsigned n_file_processed;
	for (n_file_processed = 0; n_file_processed < max_file_num; ++n_file_processed) {
		sprintf (logfilename, LOG_DIR LOG_NAME_FORMAT, curr_file_idx);

		// Open file.
		/* HANDLE fhnd = CreateFile ( logfilename,
		   GENERIC_READ,
		   FILE_SHARE_WRITE | FILE_SHARE_READ,
		   NULL,
		   OPEN_EXISTING,
		   FILE_ATTRIBUTE_NORMAL,
		   NULL); */
		/* This code eommneded for testing, need to re write if required
		   HANDLE fhnd = open(logfilename,
		   GENERIC_READ | OPEN_EXISTING,
		   FILE_ATTRIBUTE_NORMAL);

		   if (fhnd == INVALID_HANDLE_VALUE)
		   break; */

		// Get file info and close file.
		//BOOL result = GetFileInformationByHandle(fhnd, &info);
		BOOL result = 1;// linux porting
		//CloseHandle(fhnd);
		//pthread_mutex_destroy(fhnd);

		// print file statistics.
		if (result != 0) {
			//SYSTEMTIME filetime;
			tm filetime;
			//FileTimeToSystemTime (&info.ftLastWriteTime, &filetime);
			session->Sendf ( " %10u   %.2u/%.2u/%.4u %.2u:%.2u   %s" NEWLINE,
					info.nFileSizeLow,
					filetime.tm_mday, filetime.tm_mon, filetime.tm_year,
					filetime.tm_hour, filetime.tm_min,
					logfilename);
		}

		// next file.
		if (curr_file_idx == 1)
			curr_file_idx = max_file_num;
		else
			--curr_file_idx;
	}
}


/**
  Terminal command callback
  */
void KLog::DoTermCmd (Term_Session *session, bool no, Term_Cmd *command)
{
	CVariant p1, p2, p3;
	String temp;
	int value;

	switch (command->GetId()) {
		case Log_Term_Cmd_Logging:
			Cmd_Logging (session);
			break;

		case Log_Term_Cmd_Verbosity:
			assert (command->GetArgCount() == 2);
			command->GetArg ((size_t)0)->GetValue( p1 );
			command->GetArg (1)->GetValue (p2);
			Cmd_SetVerbosity (session, p1.ToString(), p2.ToString());
			break;

		case Log_Term_Cmd_File:
			assert (command->GetArgCount() == 2);
			command->GetArg ((size_t)0)->GetValue (p1);
			command->GetArg (1) ->GetValue (p2);
			Cmd_SetFile ( session, p1.ToString(), p2.AsInt() );
			//Cmd_SetFile ( session, p1.ToString(), value); //linux porting
			break;

		case Log_Term_Cmd_Trace:
			command->GetArg((size_t)0)->GetValue(p1);
			Trace_Set_Enabled(p1.AsString(), !no);
			break;

		case Log_Term_Cmd_LastLog:
			command->GetArg((size_t)0)->GetValue(p1);
			command->GetArg(1)->GetValue(p2);
			temp = p2.ToString();
			Cmd_LastLog (session, p1.AsInt(), Log_StrToLevel(temp.c_str()));
			break;

		case Log_Term_Cmd_ViewLog:
			command->GetArg ((size_t)0)->GetValue(p1);
			command->GetArg ((size_t)1)->GetValue(p2);
			command->GetArg ((size_t)2)->GetValue(p3);
			Cmd_ViewLog(session, p3.AsString(), p1.AsInt(), p2.AsInt());
			break;

		case Log_Term_Cmd_LogFile:
			Cmd_LogFile (session);
			break;
	}
}


bool KLog::OnShowConfig( const Cfg_Tree_Entry *entry,
		String & output, 
		const String & indent, 
		int indent_size) const
{
	// temporary variables so that we don't have to hold the mutexes
	// while building configs.
	LogVerbosity input_verbosity;
	Map <String, LogVerbosity> output_verbosity;
	size_t maxnum, maxsize;

	// copy values to temp variables while holding the mutexes
	do {
		ACE_Guard <ACE_Token> guard1(const_cast<ACE_Token&> (m_LogMsgMutex));
		ACE_Guard <ACE_Token> guard2(const_cast<ACE_Token&> (m_DeviceMutex));

		input_verbosity = m_VInput;
		output_verbosity = __m_VOutput;
		maxnum = __m_LogRotator.GetMaxFileNum();
		maxsize = __m_LogRotator.GetMaxFileSize();

	} while (0);

	char temp[128];

	//
	// if we're called from the root config tree, then add 'logging' command and
	// add indentation.
	//
	String newindent = indent;
	if (entry == Kernel::Instance()->RootConfig()) {
		sprintf ( temp, "%slogging" NEWLINE, newindent.c_str());
		output += temp;
		newindent.append ( (size_t)indent_size, ' ');
	}

	//
	// format configuration string.
	//
	sprintf ( temp, "%sverbosity input %s" NEWLINE, newindent.c_str(),  Log_LevelToName(input_verbosity));
	output += temp;

	Map <String, LogVerbosity>::iterator it, end;
	end = output_verbosity.end();
	for (it = output_verbosity.begin(); it!=end; ++it) {
		sprintf ( temp, "%sverbosity %s %s" NEWLINE, newindent.c_str(),  it->first.c_str(), Log_LevelToName(it->second));
		output += temp;
	}

	sprintf(temp, "%sfile maxnum %u" NEWLINE, newindent.c_str(), maxnum);
	output += temp;

	sprintf(temp, "%sfile maxsize %u" NEWLINE, newindent.c_str(), maxsize);
	output += temp;

	//
	// if we're called from the root config tree, then add 'top' command after
	// all commands
	//
	if (entry == Kernel::Instance()->RootConfig()) {
		sprintf ( temp, "%stop" NEWLINE, newindent.c_str());
		output += temp;
	}

	output += NEWLINE;
	return OK;
}


/** Create a trace identifier.
*/
Log_Trace * KLog::CreateTrace (const String & trace_id, const String & trace_name, const String & description)
{
	ACE_Guard <ACE_Token> guard2(m_DeviceMutex);
	Log_Trace *trace;
	NEW_RETURN_ (trace, Log_Trace(trace_id, trace_name, description));
	if (trace != NULL) {
		assert (std::find(__m_Traces.begin(), __m_Traces.end(), trace) == __m_Traces.end());
		__m_Traces.remove(trace);
		__m_Traces.push_back(trace);
		m_TraceChoiceList.push_back (std::make_pair(CVariant(trace_name.c_str()), description));
	}
	return trace;
}

/** Destroy a trace identifier.
*/
void KLog::DestroyTrace (Log_Trace *& trace)
{
	if (trace == NULL) return;

	ACE_Guard <ACE_Token> guard2(m_DeviceMutex);
	__m_Traces.remove(trace);

	// remove trace choice list.
	List <ValidatorChoice>::iterator it, end;
	end = m_TraceChoiceList.end();
	for (it = m_TraceChoiceList.begin(); it!=end; ++it) {
		ValidatorChoice & choice = *it;
		if (trace->traceName == choice.first.AsString()) {
			m_TraceChoiceList.erase(it);
			break;
		}
	}

	delete trace;
	trace = NULL;
}

/** Determine whether tracing is enabled for the specified entry.
*/
bool KLog::IsTraceEnabled (const Log_Trace *trace) const
{
	return trace && trace->traceEnabled;
}

/** Submit a trace message.
*/
void KLog::TraceStandard ( Log_Trace *trace, const char *format, ...)
{
	va_list marker;
	va_start(marker,format);
	Trace( LFmt_TimeMsec, trace, format, marker);
	va_end(marker);
}

/** Submit a trace message.
*/
void KLog::TraceStandard ( Log_Trace *trace, CTask *task, const char *format, ...)
{
	va_list marker;
	va_start(marker,format);
	Trace( LFmt_TimeMsec, trace, task, format, marker);
	va_end(marker);
}

/** Submit a trace message.
*/
void KLog::TraceFormat ( LogFormat fmt, Log_Trace *trace, const char *format, ...)
{
	va_list marker;
	va_start(marker,format);
	Trace( fmt, trace, format, marker);
	va_end(marker);
}

/** Submit a trace message.
*/
void KLog::TraceFormat ( LogFormat fmt, Log_Trace *trace, CTask *task, const char *format, ...)
{
	va_list marker;
	va_start(marker,format);
	Trace( fmt, trace, task, format, marker);
	va_end(marker);
}

/** Submit a trace message (variant with va_list)
*/
void KLog::Trace ( LogFormat fmt, Log_Trace *trace, const char *format, va_list& marker)
{
	if (!trace || !trace->traceEnabled)
		return;

	LogFmt(System, fmt, LOG_TRACE, trace->traceId.c_str(), TRACE_SOURCE, format, marker);
}

/** Submit a trace message (variant with va_list)
*/
void KLog::Trace ( LogFormat fmt, Log_Trace *trace, CTask *task, const char *format, va_list& marker)
{
	if (!trace || !trace->traceEnabled)
		return;

	LogFmt(System, fmt, LOG_TRACE, trace->traceId.c_str(), task, format, marker);
}

//-----------------------------------------------------------------------------
// Main log entry
//
void KLog::Log( LogVerbosity level, const char *sender, CTask *task, 
		const char *format, va_list& marker)
{
	//LogFmt(System, LFmt_TimeMsec/*LFmt_Standard*/,level,sender,task,format,marker);
//	printf("***** Inside KLog::Log **********\n");
	LogFmt(System, LFmt_Standard,level,sender,task,format,marker);
}


void KLog::LogApp( LogVerbosity level, const char *sender, CTask *task, 
		const char *format, va_list& marker)
{
	//LogFmt(Application, LFmt_TimeMsec/*LFmt_Standard*/,level,sender,task,format,marker);
	LogFmt(Application, LFmt_Standard,level,sender,task,format,marker);
}

static void sig_hdl (int sig, siginfo_t *siginfo, void *context)
{
	longjmp(ex_buf__, 1);
}


static int handleDev(char *m_Buf, char *buf, va_list& marker)
{
	int len = 0;
	memset (&act, '\0', sizeof(act));

	//printf("\n Inside Handle Device \n");

	/* Use the sa_sigaction field because the handles has two additional parameters */
	act.sa_sigaction = &sig_hdl;

	/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	//act.sa_flags = SA_SIGINFO;
	act.sa_flags = SA_NODEFER ;

	if (sigaction(SIGSEGV, &act, NULL) < 0)
	{
		perror ("sigaction");
		return 1;
	}

	if(!setjmp(ex_buf__))
	{
		len = _vsnprintf( m_Buf, LOG_MAX_MSG-2, buf, marker);
	}

	return len;
}


/** Put a mwssage to the log using the specified format
*/
void KLog::LogFmt(SenderType senderType, LogFormat fmt, LogVerbosity level, const char *sender, CTask *task,
		const char *format, va_list& marker)
{

	// Check input verbosity filter
//	if (level < m_VInput)
//		return;

//	printf("***** Inside KLog::LogFmt ******\n");
	char short_sender[6];
	char buf[LOG_MAX_MSG];
	char taskid[64];

	// Format the message's format
	struct timeb tb;
	DWORD msec = timeGetTime()%1000;
	struct tm *tm;

	ftime(&tb);
	tm = localtime(&tb.time);

	// build short sender
	safe_strcpy( short_sender, "          ", sizeof(short_sender));
	memcpy( short_sender, sender, min(sizeof(short_sender)-1,strlen(sender)));

	// build the string format
	switch (fmt) 
	{
		case LFmt_Standard:
			// Build the task argument.
			// Note that when this function is called via Trace(), the task
			// argument is TRACE_SOURCE.
			if (task == TRACE_SOURCE)
				taskid[0] = '\0';
			else if (task) {
				char appname[8];
				safe_strcpy(appname,task->GetApplication()->Name().c_str(),3);
				char pid[16];
				sprintf(pid,"%04u",task->GetPID());
				sprintf(taskid,"%s%s",appname,pid);
			} else {
				strcpy(taskid,"......");
			}

			_snprintf( buf, LOG_MAX_MSG, "%02d %02d%02d%02d.%03d %s %s %s %s" NEWLINE,
					tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, tb.millitm,
					Log_LevelToStr(level), 
					short_sender, taskid, format);
			break;

		case LFmt_DateTime:
			_snprintf( buf, LOG_MAX_MSG, "%02d %02d%02d%02d %s" NEWLINE,
					tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
					format);
			break;

		case LFmt_TimeOnly:
			_snprintf( buf, LOG_MAX_MSG, "%02d%02d%02d %s" NEWLINE,
					tm->tm_hour, tm->tm_min, tm->tm_sec,
					format);
			break;

		case LFmt_TimeMsec:
			/* For tracing, we put %T as the first character. */
#if 1
			_snprintf( buf, LOG_MAX_MSG, "%%%%T%02d%02d%02d.%03d %s" NEWLINE,
					tm->tm_hour, tm->tm_min, tm->tm_sec, tb.millitm,
					/*short_sender,*/format);
#else
			if (task == TRACE_SOURCE)
				taskid[0] = '\0';
			else if (task) {
				char appname[8];
				safe_strcpy(appname,task->GetApplication()->Name().c_str(),3);
				char pid[16];
				sprintf(pid,"%04u",task->GetPID());
				sprintf(taskid," %s%s",appname,pid);
			} else {
				taskid[0] = 0;
			}

			_snprintf( buf, LOG_MAX_MSG, "%%%%T%02d%02d%02d.%03d %s%s %s" NEWLINE,
					tm->tm_hour, tm->tm_min, tm->tm_sec, tb.millitm,
					short_sender, taskid, format);
#endif
			break;

		case LFmt_Raw:
			_snprintf( buf, LOG_MAX_MSG, "%s" NEWLINE, format);
			break;
	}
	buf[LOG_MAX_MSG-1] = '\0';

	// build the message, must have mutex.
	WriteData *write_data = AllocWriteData(senderType);
	if (!write_data) {
		++m_MisedLogs;
		printf("***** Inside KLog:: no write_data ******\n");
		return;
	}

	int len = 0;
	try {
#ifndef PRINT_NO_LOG
		//  len = handleDev(write_data->m_Buf, buf, marker);
		len = _vsnprintf( write_data->m_Buf, LOG_MAX_MSG-2, buf, marker);
#else
		len = snprintf( write_data->m_Buf, LOG_MAX_MSG-2, "%s", buf);
#endif
	} catch (...) {
		len = _snprintf( write_data->m_Buf, LOG_MAX_MSG-2, "Invalid log format sent by %s: '%s'\n", 
				sender, format);
		level = LOG_FATAL;
	}
	//write_data->m_Buf[ LOG_MAX_MSG-3 ] = '\r';
	write_data->m_Buf[ LOG_MAX_MSG-2 ] = '\n';
	write_data->m_Buf[ LOG_MAX_MSG-1 ] = '\0';

	// length and verbosity
	write_data->m_Length = (len == -1? 1 : len + 1);
	write_data->m_Verbosity = level;

	if (IsThreadRunning()) {
#ifndef PRINT_NO_LOG
	//	printf("********** klog thread running , calling PutMessage 1*********\n");
		PutMessage( MSG_WRITE_LOG_MSG, write_data);
#endif
	} else {

//		printf("********** klog thread not running , calling PutMessage 1*********\n");
		//__OnWriteLog(LogMsg(MSG_WRITE_LOG_MSG, write_data));
		LogMsg msg(MSG_WRITE_LOG_MSG, write_data);
		__OnWriteLog(msg);
	};

	//
	// Print missed log.
	//
#ifndef PRINT_NO_LOG
	if (m_MisedLogs && 
			time(NULL)-m_LastMissed > 5 && 
			(write_data = AllocWriteData(System)) != NULL ) 
	{
		len = sprintf(write_data->m_Buf, "***WARNING: Some %d log message(s) could not be printed, log too busy.\n", m_MisedLogs);
		write_data->m_Length = (len == -1? 1 : len + 1);
		write_data->m_Verbosity = LOG_WARNING;

		if (IsThreadRunning()) {
//			printf("********** klog thread running , calling PutMessage 2*********\n");
			PutMessage( MSG_WRITE_LOG_MSG, write_data);
		} else {
//			printf("********** klog thread not running , calling PutMessage 2*********\n");
			//__OnWriteLog(LogMsg(MSG_WRITE_LOG_MSG, write_data));
			LogMsg msg(MSG_WRITE_LOG_MSG, write_data);
			__OnWriteLog(msg);
		}

		m_MisedLogs = 0;
		m_LastMissed = time(NULL);
	}
#endif
}


//-----------------------------------------------------------------------------
// get message from message queue. If message queue is empty, message of type
// MSG_EMPTY will be returned.
//
KLog::LogMsg KLog::__GetMessage()
{
//	usleep(100);
//	cout << "__GetMessage sem_wait on full "  << endl;
	if(sem_trywait(&full) == 0){
//	cout << "__GetMessage after sem_wait on full "  << endl;
	m_LogMsgMutex.acquire();
	
	if (m_LogMsg.empty()) {
		m_LogMsgMutex.release();
		sem_post(&empty);
		return LogMsg( MSG_EMPTY, NULL);
	};

	LogMsg msg( m_LogMsg.front() );
	m_LogMsg.pop_front();

	m_LogMsgMutex.release();
	sem_post(&empty);
	return msg;
	}else{
		return LogMsg( MSG_EMPTY, NULL);
	}

}

//-----------------------------------------------------------------------------
// put message in the message queue
//
void KLog::PutMessage( const LogMsg &msg )
{

//	cout << "__GetMessage sem_wait on empty "  << endl;
	if(sem_trywait(&empty) == 0){
//	cout << "__GetMessage after sem_wait on empty "  << endl;
//	cout << "IInside PutMessage : " << m_LogMsg.size() << endl;
	m_LogMsgMutex.acquire();
/*	while (m_LogMsg.size() > LOG_MAX_QUEUE) {
		m_LogMsgMutex.release();
		sl::Sleep(5);
		m_LogMsgMutex.acquire();
	}*/

	m_LogMsg.push_back(msg);
	m_LogMsgMutex.release();
	sem_post(&full);
	} 
}

//-----------------------------------------------------------------------------
// put message 
//
void KLog::PutMessage( LogMsgType type, LogData *data )
{
//	cout << " Inside PutMessage ********** "  << endl;
	PutMessage( LogMsg(type, data));
}

//-----------------------------------------------------------------------------
// remove messages which match type and LogData
//
void KLog::RemoveMessage( LogMsgType type, LogData &data_equal_to)
{
	m_LogMsgMutex.acquire();
	List <LogMsg>::iterator it, end_it = m_LogMsg.end();
	for (it=m_LogMsg.begin(); it!=end_it; ) {
		if (it->m_Type == type && it->m_Data->EqualTo(data_equal_to))
			m_LogMsg.erase(it++);
		else
			it++;
	};
	m_LogMsgMutex.release();
}


//=============================================================================
//
// THE OTHER THREAD
//
void *KLog::thread_proc(void *p)
{
	KLog *klog = (KLog*)p;
	return klog->ThreadProc();
}

void *KLog::ThreadProc()
{
	int ret;

	thread_running_ = true;

	do {
		LogMsg msg(__GetMessage());
		ret = __ProcessMessage(msg);
	} while(ret==1);
//	printf(" ******************************************* Coming out of klog thread **************\n");
	
	thread_running_ = false;
	return (void*)ret;
}

//-----------------------------------------------------------------------------
//int KLog::__ProcessMessage( LogMsg &msg )
int KLog::__ProcessMessage( LogMsg &msg )
{
	switch(msg.m_Type) {
		case MSG_EMPTY:
//			cout << "******** Msg type is MSG_EMPTY *****" << endl;
			sl::Sleep(100);
			break;
		case MSG_WRITE_LOG_MSG:
//			cout << "******** Msg type is MSG_WRITE_LOG_MSG *****" << endl;
			__OnWriteLog(msg);
			break;
		case MSG_SET_VERBOSITY:
			__OnSetVerbosity(msg);
			break;
		case MSG_INIT_ROTATOR:
//			cout << "******** Msg type is MSG_INIT_ROTATOR *****" << endl;
			if (__OnInitRotator(msg) != OK)
				return -1;
			break;
		case MSG_QUIT:
			return 0;
		case MSG_ATTACH_DEVICE:
			__OnAttachDevice(msg);
			break;
		default:
			assert(false);
			break;
	};

	return 1;
}

//-----------------------------------------------------------------------------
STATUS KLog::__OnWriteLog( LogMsg &log_msg )
{
	WriteData *write_data = static_cast<WriteData*> (log_msg.get_data());
	if (!write_data)
		return NOT_OK;

	m_DeviceMutex.acquire();
	for ( List<CLogDevice*>::iterator it=__m_Devices.begin(); it!=__m_Devices.end(); it++) {
		CLogDevice* logger = *it;
		if (logger->__m_Verbosity <= write_data->m_Verbosity) {
			if ((write_data->m_SenderType == System) ||
					logger->AcceptAppLog()) {
				logger->__Write( write_data->m_Verbosity, write_data->m_Length, write_data->m_Buf );
			}
		}
	}
	m_DeviceMutex.release();

	// save the last error/fatal msg
	if (write_data->m_Verbosity >= LOG_ERRORS) {
		strcpy(__m_LastErrMsg, write_data->m_Buf);
	}

	
	return OK;
}

//-----------------------------------------------------------------------------
STATUS KLog::__OnInitRotator( LogMsg &log_msg )
{
	InitRotatorData *init_rot = static_cast<InitRotatorData*> (log_msg.get_data());
	if (!init_rot)
		return NOT_OK;

	//STATUS
	bool ret = __m_LogRotator.Init( init_rot->m_LogDir.c_str(), init_rot->m_LogNameFormat.c_str(), 
			init_rot->m_MaxFile, init_rot->m_MaxSize);

	// fatal: can't initialize log file
	if (ret!=OK) {
		m_DeviceMutex.acquire();
		__m_Devices.remove( &__m_LogRotator );
		m_DeviceMutex.release();

		WriteData *data = AllocWriteData(System);

		if (data) {
			data->m_Verbosity = LOG_FATAL;
			strcpy(data->m_Buf, "Error in initializing log file. "
					"Probably log directory is invalid.");
			data->m_Length = strlen(data->m_Buf);
			LogMsg msg(MSG_WRITE_LOG_MSG, data);
			__OnWriteLog(msg);
		}
		return NOT_OK;
	}

	return OK;
}

//-----------------------------------------------------------------------------
STATUS KLog::__OnSetVerbosity( LogMsg &log_msg )
{
	SetVerbosityData *verb_data = static_cast<SetVerbosityData*> (log_msg.get_data());
	if (!verb_data)
		return NOT_OK;

	__m_VOutput = verb_data->m_VOutput;

	m_DeviceMutex.acquire();
	for (List <CLogDevice*>::iterator it=__m_Devices.begin(); it!=__m_Devices.end(); it++) {
		LogVerbosity v = __m_VOutput[ (*it)->GetName() ];
		(*it)->__m_Verbosity = v;
	}
	m_DeviceMutex.release();

	return OK;
}

//-----------------------------------------------------------------------------
STATUS KLog::__OnAttachDevice( LogMsg &log_msg )
{
	AttachDeviceData *attach_data = static_cast<AttachDeviceData*> (log_msg.get_data());
	if (!attach_data)
		return NOT_OK;

	LogVerbosity v = __m_VOutput[ attach_data->m_Device->GetName() ];

	if (attach_data->m_Device->__m_Verbosity < v)
		attach_data->m_Device->__m_Verbosity = v;

	m_DeviceMutex.acquire();
	__m_Devices.push_back(attach_data->m_Device);
	m_DeviceMutex.release();

	return OK;
}

