#ifndef _CA_EXCEP_LOG_H
#define _CA_EXCEP_LOG_H
#pragma once
#include "../../utilities/CAUtilites.h"
#include "CAlog.h"


#define MAX_SIZE_PER_MSG   2048

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define NEWLINE "\n"
#define TAB "\t"

#define CAExceplog_INIT(level, path, size, lifeTime) CAExceplog_TASK::instance()->init(level, path, size, lifeTime)

#define CAExceplog_EXCEP(UID, FMT, ...) \
        CAExceplog_TASK::instance()->write(\
            EN_LOG_LEVEL_EXCEP, \
            EXCEP_PREFIX, \
			__FILENAME__, \
			__LINE__, \
			UID, \
            FMT, \
##__VA_ARGS__)

#define CAExceplog_STOP() \
		CAExceplog_TASK::instance()->stop()

class CAExceplog : public ACE_Task<ACE_MT_SYNCH>
{
	private:
		bool m_run;
		std::ofstream *m_logStream;	
		std::ofstream *m_uidLogStream;	
		std::string m_filePath;
		int m_fileCount;
		char m_dirName[85];
		std::string m_path;
		unsigned short m_maxFileSize;
		std::string m_enabledLogLevel;
		unsigned short m_lifeTime;
		 int m_queueSize;
			
		LogProfile m_logProfile;

		bool post(LogLevel msgType, const char* fmt, const char* uid);
		bool handle_msg(ACE_Message_Block *pAmb);
	public:  
		CAExceplog();
		~CAExceplog(void);

		bool init(std::string level, std::string path, unsigned short size, unsigned short lifeTime);
		bool loadlogProfile(LogProfile &profile);
		bool create_file(std::string uid);
		virtual int svc(void);
		bool stop();
		bool write(LogLevel msgType, const char* verbos, const char* fileName, \
		unsigned int lineNum, const char* uid, const char* fmt...);
		bool executeFileCleanup();

};

typedef ACE_Singleton<CAExceplog, ACE_Recursive_Thread_Mutex > CAExceplog_TASK;
#endif
