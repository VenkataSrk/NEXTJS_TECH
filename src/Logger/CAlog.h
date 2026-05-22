#ifndef _XGATE_LOG_H
#define _XGATE_LOG_H
#pragma once
#include "../../utilities/CAUtilites.h"


#define MAX_SIZE_PER_MSG   2048
#define TRACE_PREFIX       ACE_TEXT ("[TRACE] ")
#define DEBUG_PREFIX       ACE_TEXT ("[DEBUG] ")
#define INFO_PREFIX        ACE_TEXT ("[INFO]  ")
#define NOTICE_PREFIX      ACE_TEXT ("[NOTICE]")
#define WARN_PREFIX        ACE_TEXT ("[WARN]  ")
#define ERROR_PREFIX       ACE_TEXT ("[ERROR] ")
#define CRITICAL_PREFIX    ACE_TEXT ("[CRIT]  ")
#define FATAL_PREFIX       ACE_TEXT ("[FATAL]  ")
#define ALERT_PREFIX       ACE_TEXT ("[ALERT] ")
#define EMERGENCY_PREFIX   ACE_TEXT ("[EMERGENCY]")
#define EXCEP_PREFIX       ACE_TEXT ("[EXCEP]")

#define LOG_FILE_PATH   "/home/yoga/vmslog.txt" 
#define LOG_FILE_SIZE   10 //in MB
#define LOG_FILE_COUNT  10

typedef enum
{
	EN_LOG_LEVEL_TRACE = 0,
	EN_LOG_LEVEL_DEBUG = 1,
	EN_LOG_LEVEL_INFO = 2,
	EN_LOG_LEVEL_WARN = 3,
	EN_LOG_LEVEL_ERROR = 4,
	EN_LOG_LEVEL_CRITICAL = 5,
	EN_LOG_LEVEL_FATAL = 6,
   EN_LOG_LEVEL_EXCEP = 7,
	EN_LOG_LEVEL_OFF = 8,
}LogLevel;

enum LOGFILETYPE {
   LOG_FILE_NONE = 0,
   LOG_FILE_ALL,        // All logs - File path configured under "logging" section in conf.xml
   LOG_FILE_EXCEP,      // Exception logs - File path configured under "excep_logging" section in conf.xml
};

/*
 * This will protoct the logoutput from empty string or null char* --jais
 *
 * Example:
 *
 *  CAlog_INFO( "HmpBin::~HmpBin start for bin: %s", (char*)ur_log_string (m_binName));// m_binName is a string
 *
 *   CAlog_INFO( "HmpBin::~HmpBin start for bin: %s", (char*)ur_log_string (lpszVal));// lpszVal is a char *
 *
 *
 *   */

class ur_log_string : public std::string
{
	public:
		ur_log_string (const std::string& tcstring)
		{
			if(tcstring.length() < 1)
			{
				std::string::operator=("empty_data");
			}
			else
				std::string::operator=(tcstring);
		}
		ur_log_string(const char*  lpcszStr)
		{
			if(lpcszStr == NULL)
			{
				std::string::operator=("null_data");
			}
			else
			{
				std::string::operator=(lpcszStr);
				if(this->length() < 1)
				{
					std::string::operator=("empty_data");
				}
			}
		}
		operator char*()
		{
			return (char*)this->c_str();
		}
};

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define NEWLINE "\n"
#define TAB "\t"

#define CAlog_INIT(level, path, size, lifeTime) CAlog_TASK::instance()->init(level, path, size, lifeTime)

#define CAlog_TRACE(FMT, ...) \
	CAlog_TASK::instance()->write(\
			EN_LOG_LEVEL_TRACE, \
			TRACE_PREFIX, \
			__FILENAME__, \
			__LINE__, \
			FMT, \
##__VA_ARGS__)

#define CAlog_DEBUG(FMT, ...) \
	CAlog_TASK::instance()->write(\
			EN_LOG_LEVEL_DEBUG, \
			DEBUG_PREFIX, \
			__FILENAME__, \
			__LINE__, \
			FMT, \
##__VA_ARGS__)  

#define CAlog_INFO(FMT, ...) \
	CAlog_TASK::instance()->write(\
			EN_LOG_LEVEL_INFO, \
			INFO_PREFIX, \
			__FILENAME__, \
			__LINE__, \
			FMT, \
##__VA_ARGS__)  

#define CAlog_WARN(FMT, ...) \
	CAlog_TASK::instance()->write(\
			EN_LOG_LEVEL_WARN, \
			WARN_PREFIX, \
			__FILENAME__, \
			__LINE__, \
			FMT, \
##__VA_ARGS__)  

#define CAlog_ERROR(FMT, ...) \
	CAlog_TASK::instance()->write(\
			EN_LOG_LEVEL_ERROR, \
			ERROR_PREFIX, \
			__FILENAME__, \
			__LINE__, \
			FMT, \
##__VA_ARGS__)  

#define CAlog_CRIT(FMT, ...) \
	CAlog_TASK::instance()->write(\
			EN_LOG_LEVEL_CRITICAL, \
			CRITICAL_PREFIX, \
			__FILENAME__, \
			__LINE__, \
			FMT, \
##__VA_ARGS__)

#define CAlog_FATAL(FMT, ...) \
	CAlog_TASK::instance()->write(\
			EN_LOG_LEVEL_FATAL, \
			FATAL_PREFIX, \
			__FILENAME__, \
			__LINE__, \
			FMT, \
##__VA_ARGS__)

#if 0
#define CAlog_EXCEP(FMT, ...) \
        CAlog_TASK::instance()->write(\
                        EN_LOG_LEVEL_EXCEP, \
                        EXCEP_PREFIX, \
			__FILENAME__, \
			__LINE__, \
                        FMT, \
##__VA_ARGS__)
#endif

#define CAlog_STOP() \
		CAlog_TASK::instance()->stop()

class LogProfile {
	public:
		LogProfile();
		~LogProfile();
		LogProfile(const LogProfile& src);
		LogProfile& operator=(const LogProfile& src);

		//member variables
		std::string mfilePath;
		unsigned int m_fileCount;
		unsigned int m_fileSize; //in mb
		bool m_fileColor; //on or off
		LogLevel m_fileLogLevel;
		LogLevel m_consoleLogLevel;

		void setFileLogLevel(const char* logLevel);
		void setConsoleLogLevel(const char* logLevel);
};

class CAlog : public ACE_Task<ACE_MT_SYNCH>
{
	private:
		bool m_run;
		std::ofstream *m_logStream;	
		std::string m_filePath;
		int m_fileCount;
		char m_dirName[85];
		std::string m_path;
		unsigned short m_maxFileSize;
		std::string m_enabledLogLevel;
		unsigned short m_lifeTime;
		 int m_queueSize;
			
		LogProfile m_logProfile;

		bool post(LogLevel msgType, const char* fmt);
		bool handle_msg(ACE_Message_Block *pAmb);
	public:  
		CAlog();
		~CAlog(void);

		bool init(std::string level, std::string path, unsigned short size, unsigned short lifeTime);
		bool loadlogProfile(LogProfile &profile);
		bool create_file();
		
		virtual int svc(void);
		bool stop();
		bool rotateLog();
		bool write(LogLevel msgType, const char* verbos, const char* fileName, \
		unsigned int lineNum, const char* fmt...);
		bool executeFileCleanup();

};

typedef ACE_Singleton<CAlog, ACE_Recursive_Thread_Mutex > CAlog_TASK;

#endif
