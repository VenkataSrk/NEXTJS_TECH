/* $Header: /xGate/klog.h 18    9/06/04 5:28p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * LOGGING manager.
 *
 * $Log: /xGate/klog.h $
 * 
 * 18    9/06/04 5:28p Bennylp
 * Log prints warning if some message can not be printed.
 * 
 * 17    11/20/02 4:56p Bennylp
 * Added mutex protection
 * 
 * 16    10/23/02 5:01p Bennylp
 * Fixed to use new macros in thread_process.h
 * 
 * 15    10/23/02 12:37p Bennylp
 * Modify to use memory pool for the log messages.
 * 
 * 14    22/05/02 17:59 Junanto
 * Application logging to terminal can now be filtered out
 * (to concentrate on system messages only) by a 
 * new parameter "application" in "term logging".
 * By default, application logging is displayed to terminal.
 * To disable it: "no term logging application".
 * 
 * 13    7/25/01 8:06p Bennylp
 * Added more trace functions.
 * 
 * 12    6/07/01 5:48p Bennylp
 * Show verbosity in 'show log', and export some functions to klog.h.
 * 
 * 11    5/01/01 3:28a Bennylp
 * Added command to view log files attributes (show log).
 * 
 * 10    5/01/01 1:06a Bennylp
 * Standard trace output, and added command 'viewlog'.
 * 
 * 9     4/27/01 6:03a Bennylp
 * Add CVariantDynamicChoiceValidator.
 * 
 * 8     26/04/01 12:04 Junanto
 * added choice for logging output display
 * 
 * 7     4/25/01 7:18a Bennylp
 * Add lastlog command.
 * 
 * 6     25/04/01 10:50 Junanto
 * added TraceList, a variant of Trace with va_list argument
 * 
 * 5     4/18/01 3:39p Bennylp
 * Change tracing interface to include log identifier.
 * 
 * 4     4/17/01 10:04p Bennylp
 * Added tracing.
 * 
 * 3     4/12/01 8:08p Bennylp
 * Integration.
 *
 */
#ifndef __KLOG_H__
#define __KLOG_H__

#include <sys/wait.h> //waitpid() - linux porting
#include <linux/limits.h>	//define PATH_MAX
#include <sys/stat.h>

#include "log.h"
#include "serviceprovider.h"
#include "static_queue.h"
#include <ace/Token.h>
#include <ace/Thread.h>
#include <errno.h>    //for error handling

//linux porting
#define _O_RDONLY       0x0000  /* open for reading only */
#define _O_BINARY       0x8000  /* file mode is binary (untranslated) */
#define _MAX_PATH 	PATH_MAX	/* # chars in a path name including nul */
#define OPEN_EXISTING               0 
#define FILE_ATTRIBUTE_NORMAL 	128
#define INVALID_HANDLE_VALUE ((HANDLE)(-1))


/* Forward decl.
*/
class Cfg_Tree_Group;
class Trace_Name_Validator;
class Log_Trace;

//extern int errno;//error handling

// constants: maximum of log message
const size_t LOG_MAX_MSG = 1500;

//typedef ACE_Token ACE_Thread_Mutex;	//linux porting
#define _snprintf   snprintf    //linux porting
#define _vsnprintf vsnprintf
#define MINISOCK_LAST_ERROR errno

//-----------------------------------------------------------------------------
// CFileRotate: class to rotate log file
//
class CLogRotate : public CLogDevice {

	public:

		// Constructor
		CLogRotate();

		// Destructor
		~CLogRotate();

		// Initialize the log rotator
		bool Init( const char *dir, const char *pattern, size_t max_file, size_t max_size);
		//STATUS Init( const char *dir, const char *pattern, size_t max_file, size_t max_size);

		// Get the max file number
		size_t GetMaxFileNum() const {
			return m_MaxIndex;
		}

		// Set the max file number.
		void SetMaxFileNum (size_t num) {
			m_MaxIndex = num;
		}

		// Get max file size.
		size_t GetMaxFileSize() const {
			return m_MaxSize;
		}

		// Set the max file size.
		void SetMaxFileSize (size_t size) {
			m_MaxSize = size;
		}

		// Shutdown the log rotator
		bool Shutdown();

		// Return the name
		const char *GetName() { return "file"; };

		// Get current index.
		unsigned GetCurrentIndex() const {
			return m_CurrentIndex;
		}

	private:
		unsigned int m_CurrentIndex, m_MaxIndex;
		unsigned long m_CurrentSize, m_MaxSize;

		FILE *m_CurrentFile;

		String m_FilePattern, m_Dir;

		STATUS OpenFile( unsigned int index );
		STATUS CloseFile();
		STATUS GetFirstFile();
		STATUS GetNextFile();

		// Write to log rotator
		bool __Write( LogVerbosity verbosity, size_t length, const char *buf );
};



//-----------------------------------------------------------------------------
// KLog: the actual logger instance, created and owned by Kernel
//
class KLog : public ServiceProvider {
	public:
		enum LogFormat {
			LFmt_Standard,		// date/time verbosity sender taskid msg
			LFmt_DateTime,		// date/time msg
			LFmt_TimeOnly,		// time msg
			LFmt_Short = LFmt_TimeOnly,
			LFmt_TimeMsec,		// hh:mm:ss.xxx msg
			LFmt_Raw,		// msg
		};

		// determines if the sender is application or system
		enum SenderType {
			Application,
			System,
		};

		/** Get the singleton instance of this log.
		  This must be called AFTER the Kernel is initialized.
		  */
		static KLog *Instance() {
			return klogInstance;
		}

		/** This function will be called by the service framework after 
		  instantiation of this class, to allow the instance to initialize
		  itself.
		  */
		virtual bool Load();

		/** This function will be called by the service framework before it is
		  destroyed/deleted, to allow the instance to cleanup itself.
		  */
		virtual bool Unload();

		/**
		  Terminal command callback
		  */
		virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

		/**
		  Show configuration for the logging entry.
		  */
		virtual bool OnShowConfig(
				const Cfg_Tree_Entry *entry,
				String & output, 
				const String & indent, 
				int indent_size) const;


		/** Create a trace identifier.
		*/
		Log_Trace * CreateTrace (
				const String & trace_id,    // ID as appears in the message (MAX 6 chars).
				const String & trace_name,  // name as appears in the configuration
				const String & description  // description for the configuration
				);

		/** Destroy a trace identifier.
		*/
		void DestroyTrace (Log_Trace * & trace);

		/** Determine whether tracing is enabled for the specified entry.
		*/
		bool IsTraceEnabled (const Log_Trace *trace) const;

		/** Submit a trace message with the specified format.
		*/
		void TraceFormat ( LogFormat fmt, Log_Trace *trace, const char *format, ...);
		void TraceFormat ( LogFormat fmt, Log_Trace *trace, CTask *task, const char *format, ...);

		/** Submit a trace message with standard format.
		*/
		void TraceStandard ( Log_Trace *trace, const char *format, ...);
		void TraceStandard ( Log_Trace *trace, CTask *task, const char *format, ...);

		/** Submit a trace message (variant with va_list)
		*/
		void Trace ( LogFormat fmt, Log_Trace *trace, const char *format, va_list& marker);
		void Trace ( LogFormat fmt, Log_Trace *trace, CTask *task, const char *format, va_list& marker);

		/** Put a message to the log.
		*/
		void Log( LogVerbosity, const char *sender, CTask *task,
				const char *format, va_list& marker);
		void LogApp( LogVerbosity, const char *sender, CTask *task,
				const char *format, va_list& marker);

		/** Put a message to the log using the specified format
		*/
		void LogFmt( SenderType senderType, LogFormat fmt, LogVerbosity, const char *sender, CTask *task,
				const char *format, va_list& marker);

		/** Attach a logger device to the KLog.
		*/
		STATUS Attach( CLogDevice *dev );

		/** Detach a logger device from the KLog.
		*/
		STATUS Detach( CLogDevice *dev );

	private:
		static KLog *klogInstance;

		KLog();
		~KLog();

		enum LogMsgType {
			MSG_EMPTY,
			MSG_WRITE_LOG_MSG,
			MSG_SET_VERBOSITY,
			MSG_INIT_ROTATOR,
			MSG_ATTACH_DEVICE,
			MSG_QUIT,
		};

		// msg data
		struct LogData {
			signed char refcnt_;

			LogData() : refcnt_(0) {}
			virtual void Destroy() = 0;
			virtual bool EqualTo( const LogData &lhs ) = 0;
		};
		// msg container data.
		struct LogContainerData {
			LogData *ld_;

			LogContainerData(LogData *ld) : ld_(ld) {}
			~LogContainerData() { ld_->Destroy(); }
		};

		// msg
		struct LogMsg {
			LogMsgType   m_Type;
			LogData	    *m_Data;

			LogMsg( LogMsgType t, LogData *d ) : m_Type(t), m_Data(d) 
			{
				if (m_Data)
					++m_Data->refcnt_;
			}

			LogMsg( const LogMsg &lhs ) : m_Type(lhs.m_Type), m_Data(lhs.m_Data) 
			{
				if (m_Data)
					++m_Data->refcnt_;
			}

			~LogMsg()
			{
				if (m_Data) {
					--m_Data->refcnt_;
					if (m_Data->refcnt_ == 0)
						m_Data->Destroy();
				}
			}

			LogMsg & operator = (const LogMsg &lhs)
			{
				m_Type = lhs.m_Type;
				m_Data = lhs.m_Data;
				++m_Data->refcnt_;
				return *this;
			}

			LogData *get_data() 
			{
				LogData *tmp = NULL;
				if (m_Data == NULL)
					return NULL;

				try {
					tmp = (LogData*)m_Data->refcnt_;
				} catch (...) {
					return NULL;
				}
				return m_Data;
			}
		};

		// write data
		struct WriteData : public LogData {
		    size_t       m_Length;
			LogVerbosity m_Verbosity;
			char	     m_Buf[LOG_MAX_MSG];
			SenderType   m_SenderType;

			WriteData(SenderType senderType) {
				m_SenderType = senderType;
			}

			WriteData( SenderType senderType, LogVerbosity v, size_t length, const char *msg) 
				: m_Verbosity(v) 
			{
				m_SenderType = senderType;
				m_Length = length;
				safe_strcpy( m_Buf, msg, LOG_MAX_MSG );
			}

			WriteData( SenderType senderType, LogVerbosity v, const char *msg) 
				: m_Verbosity(v) 
			{
				m_SenderType = senderType;
				safe_strcpy( m_Buf, msg, LOG_MAX_MSG );
				m_Length = strlen(m_Buf);
			}

			virtual void Destroy()
			{
				KLog::Instance()->ReturnWriteData(this);
			}

			virtual bool EqualTo( const LogData &lhs ) 
			{
				const WriteData &w = static_cast<const WriteData&> (lhs);
				return m_Length==w.m_Length && m_Verbosity==w.m_Verbosity &&
					!strcmp( m_Buf, w.m_Buf);
			};
		};

		// init rotator
		struct InitRotatorData : public LogData {
			String	m_LogDir;
			String	m_LogNameFormat;
			int	m_MaxFile;
			long m_MaxSize;

			InitRotatorData( const String &log_dir, const char *format, size_t mf, size_t ms )
				: m_LogDir(log_dir), m_LogNameFormat(format), m_MaxFile(mf), m_MaxSize(ms) 
			{};

			InitRotatorData( const InitRotatorData &lhs )
				: m_LogDir(lhs.m_LogDir), m_LogNameFormat(lhs.m_LogNameFormat), m_MaxFile(lhs.m_MaxFile), m_MaxSize(lhs.m_MaxSize) 
			{};

			operator = (const InitRotatorData &lhs ) 
			{
				m_LogDir = lhs.m_LogDir;
				m_LogNameFormat = lhs.m_LogNameFormat;
				m_MaxFile = lhs.m_MaxFile;
				m_MaxSize = lhs.m_MaxSize;
			};

			virtual void Destroy()
			{
				delete this;
			}

			virtual bool EqualTo( const LogData &lhs )
			{
				const InitRotatorData &r = static_cast<const InitRotatorData&> (lhs);
				return m_LogDir==r.m_LogDir && m_LogNameFormat==r.m_LogNameFormat &&
					m_MaxFile==r.m_MaxFile && m_MaxSize==r.m_MaxSize;
			}
		};

		// set verbosity message data
		struct SetVerbosityData : public LogData {
			Map <String, LogVerbosity>  m_VOutput;

			SetVerbosityData() {};

			SetVerbosityData( const SetVerbosityData &lhs )
				: m_VOutput(lhs.m_VOutput) 
			{};

			operator = ( const SetVerbosityData &lhs )
			{
				m_VOutput = lhs.m_VOutput;
			};

			virtual void Destroy()
			{
				delete this;
			}

			virtual bool EqualTo( const LogData &lhs )
			{
				const SetVerbosityData &v = static_cast<const SetVerbosityData&> (lhs);
				return m_VOutput==v.m_VOutput;
			};
		};

		// attach device message data
		struct AttachDeviceData : public LogData {
			CLogDevice  *m_Device;

			AttachDeviceData(CLogDevice *d) : m_Device(d) {};
			AttachDeviceData( const AttachDeviceData &lhs )
				: m_Device(lhs.m_Device) {};
			operator = ( const AttachDeviceData &lhs )
			{
				m_Device = lhs.m_Device;
			};


			virtual void Destroy()
			{
				delete this;
			}

			virtual bool EqualTo( const LogData &lhs )
			{
				const AttachDeviceData &d = static_cast<const AttachDeviceData &> (lhs);
				return m_Device==d.m_Device;
			};
		};

		// quit message data
		struct QuitData : public LogData {
			virtual void Destroy()
			{
				delete this;
			}

			virtual bool EqualTo( const LogData &)
			{
				return true;
			};
		};


		// Convert Verbosity into String
		static const char *StrLevel(LogVerbosity);

		// Flag to indicate whether we're already loaded.
		bool isLoaded;

		// input verbosity
		LogVerbosity m_VInput;

		// output verbosities, per device name
		Map <String, LogVerbosity> __m_VOutput;

		// Config tree.
		Cfg_Tree_Group *m_LoggingTree;

		// last error message
		char __m_LastErrMsg[LOG_MAX_MSG];

		// flag to indicate log thread has exited because of errors
		bool	    m_ErrorOnThread;

		// internal message queue
		List<LogMsg>    m_LogMsg;
		ACE_Token	    m_LogMsgMutex;

		// internal WriteData pool.
		//ACE_Thread_Mutex m_WriteDataPoolMutex;
		pthread_mutex_t m_WriteDataPoolMutex;
		static_queue<WriteData*> m_WriteDataPool;


	public:
		/** Allocate one WriteData instance from the static buffer.
		*/
		WriteData *AllocWriteData(SenderType senderType);

		/** Return WriteData instance.
		*/
		void ReturnWriteData(WriteData *data);

	private:
		// get message from message queue. If message queue is empty, message of type
		// MSG_EMPTY will be returned.
		LogMsg __GetMessage();

		// put message in the message queue
		void PutMessage( const LogMsg &msg );

		// put message 
		void PutMessage( LogMsgType type, LogData *data );

		// remove messages which match type and LogData
		void RemoveMessage( LogMsgType type, LogData &data_equal_to);

		// Worker thread main proc
		//ACE_thread_t    thread_id_;
		//ACE_hthread_t   thread_hnd_;
		pthread_t    thread_id_;
		pthread_t   thread_hnd_;
		bool	    thread_running_;
		static void *thread_proc(void *);
		void *ThreadProc();
		bool IsThreadRunning() const;

		// output devices
		ACE_Token		m_DeviceMutex;
		List <CLogDevice*>	__m_Devices;

		// Misses logs
		unsigned		m_MisedLogs;
		time_t		m_LastMissed;

		// tracing identifiers
		List <Log_Trace*>	__m_Traces;
		List <ValidatorChoice> m_TraceChoiceList;

		// log rotator
		CLogRotate __m_LogRotator;

		// fill WriteData structure
		void FormatLog( WriteData *write_data, LogVerbosity level, const char *sender, CTask *task, 
				const char *format, va_list& marker);

		int m_OverflowCounter;

		// message handlers
		int __ProcessMessage( LogMsg &log_msg );
		//bool __ProcessMessage( LogMsg &log_msg );
		STATUS __OnInitRotator( LogMsg &log_msg );
		STATUS __OnSetVerbosity( LogMsg &log_msg );
		STATUS __OnWriteLog( LogMsg &log_msg );
		STATUS __OnAttachDevice( LogMsg &log_msg );


		/** Get the last error message.
		*/
		const char * GetLastErrMsg() const {
			return __m_LastErrMsg;
		}

		/** Command to enter logging tree.
		*/
		void Cmd_Logging(Term_Session *);

		/** Command to set verbosity.
		*/
		void Cmd_SetVerbosity ( Term_Session *, const String & device, const String & level );

		/** Command to set file parameters.
		*/
		void Cmd_SetFile ( Term_Session *, const String & setting, int value );

		/** Command to view the last messages printed.
		*/
		void Cmd_LastLog ( Term_Session *, unsigned lines, LogVerbosity level);

		/** Command to view the contents of the log file.
		*/
		void Cmd_ViewLog ( Term_Session *, const char *filename, int start, int end);

		/** Command to view the log files attributes.
		*/
		void Cmd_LogFile ( Term_Session * );

		/** Find a trace device.
		*/
		Log_Trace * Trace_Find (const char *name);

		/** Lock the trace members.
		*/
		void Trace_Lock();

		/** Unlock the trace members.
		*/
		void Trace_Unlock();

		/** Suggest a trace name.
		*/
		const List<ValidatorChoice> & Trace_GetList ();

		/** Create a printable trace names.
		*/
		void Trace_Vdtor_List (List<v_string_pair>& values);

		/** Enable/disable tracing.
		*/
		void Trace_Set_Enabled (const char * name, bool enabled);

		/** Enable/disable tracing.
		*/
		void Trace_Show (Term_Session *session);

		// Kernel needs to instantiate this class
		friend class Kernel;

		// Trace_Name_Validator needs to validate the trace names.
		friend class Trace_Name_Validator;
};


/** Convert log nam (string, like "detail" or "info") into log verbosity level.
*/
LogVerbosity Log_StrToLevel (const char *str);

/** Get the short name of a log verbosity.
*/
const char * Log_LevelToStr (LogVerbosity level, const char **desc = NULL);

/** Get the name of the log verbosity.
*/
const char * Log_LevelToName (LogVerbosity level, const char **desc = NULL);

//This CopyFile wrapper function for linux porting
int CopyFile(const char *src_file, const char *des_file) {
	int pid = fork();
	if (pid) {
		printf("************ This is a parent process **************\n");
		fflush(stdout);
		int s; waitpid(pid,&s,0);

	}else {
		printf("************ This is a child process **************\n");
		fflush(stdout);
		execl("/bin/cp","/bin/cp",src_file,des_file,0);
	}
}

long filelength(char *f)
{
	struct stat st;
	stat(f, &st);
	return st.st_size;
}

unsigned int timeGetTime()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return now.tv_usec/1000;
}

typedef struct _BY_HANDLE_FILE_INFORMATION {
	unsigned int dwFileAttributes;
	tm ftCreationTime;
	tm ftLastAccessTime;
	tm ftLastWriteTime;
	unsigned int    dwVolumeSerialNumber;
	unsigned int    nFileSizeHigh;
	unsigned int    nFileSizeLow;
	unsigned int    nNumberOfLinks;
	unsigned int    nFileIndexHigh;
	unsigned int    nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION;

#endif
