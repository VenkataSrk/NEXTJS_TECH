#ifndef _XGATE_LOG_H
#define _XGATE_LOG_H
#pragma once
#include <ace/Task.h>
#include <ace/Singleton.h>
#include <ace/Log_Msg.h>
#include <ace/OS.h>
#include <ace/OS_NS_unistd.h>
#include <list>
#include <sys/stat.h>
/* value Modified based on modified log msg len declared in
 *  /usr/local/src/ACE_wrappers/ace/Default_Constants.h file
 * define ACE_MAXLOGMSGLEN 100 * 1024
 */
#define MAX_SIZE_PER_MSG   (102400 - 100)
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
  EN_LOG_LEVEL_OFF = 7,
}LogLevel;

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define NEWLINE "\n"

#define XGLOG_INIT XGLOG_TASK::instance()

#define XGLOG_TRACE(FMT, ...) \
  XGLOG_TASK::instance()->write(\
      EN_LOG_LEVEL_TRACE, \
      TRACE_PREFIX, \
      __FILENAME__, \
      __LINE__, \
      FMT, \
      ##__VA_ARGS__)

#define XGLOG_DEBUG(FMT, ...) \
  XGLOG_TASK::instance()->write(\
      EN_LOG_LEVEL_DEBUG, \
      DEBUG_PREFIX, \
      __FILENAME__, \
      __LINE__, \
      FMT, \
      ##__VA_ARGS__)  

#define XGLOG_INFO(FMT, ...) \
  XGLOG_TASK::instance()->write(\
      EN_LOG_LEVEL_INFO, \
      INFO_PREFIX, \
      __FILENAME__, \
      __LINE__, \
      FMT, \
      ##__VA_ARGS__)  

#define XGLOG_WARN(FMT, ...) \
  XGLOG_TASK::instance()->write(\
      EN_LOG_LEVEL_WARN, \
      WARN_PREFIX, \
      __FILENAME__, \
      __LINE__, \
      FMT, \
      ##__VA_ARGS__)  

#define XGLOG_ERROR(FMT, ...) \
  XGLOG_TASK::instance()->write(\
      EN_LOG_LEVEL_ERROR, \
      ERROR_PREFIX, \
      __FILENAME__, \
      __LINE__, \
      FMT, \
      ##__VA_ARGS__)  

#define XGLOG_CRIT(FMT, ...) \
  XGLOG_TASK::instance()->write(\
      EN_LOG_LEVEL_CRITICAL, \
      CRITICAL_PREFIX, \
      __FILENAME__, \
      __LINE__, \
      FMT, \
      ##__VA_ARGS__)

#define XGLOG_FATAL(FMT, ...) \
  XGLOG_TASK::instance()->write(\
      EN_LOG_LEVEL_FATAL, \
      FATAL_PREFIX, \
      __FILENAME__, \
      __LINE__, \
      FMT, \
      ##__VA_ARGS__)

class LogProfile {
  public:
    LogProfile();
    ~LogProfile();
    LogProfile(const LogProfile& src);
    LogProfile& operator=(const LogProfile& src);

    //member variables
    std::string m_filePath;
    unsigned int m_fileCount;
    unsigned int m_fileSize; //in mb
    bool m_fileColor; //on or off
    LogLevel m_fileLogLevel;
    LogLevel m_consoleLogLevel;

    void setFileLogLevel(const char* logLevel);
    void setConsoleLogLevel(const char* logLevel);
};

class XGLog : public ACE_Task<ACE_MT_SYNCH>
{
  public:  
    XGLog();
    ~XGLog(void);

    bool init(std::string proc_name);
    bool loadlogProfile(LogProfile &profile);
    bool create_directory();
    bool create_file();
    virtual int svc(void);
    bool stop();
    bool rotateLog();
    bool write(LogLevel msgType, const char* verbos, const char* fileName, \
        unsigned int lineNum, const char* fmt...);
    int LogDataNTime (char *log);
    bool SendLogMsgToAce (LogLevel  level, char *logMsg);
  private:
    bool m_run;
    std::string m_proc_name;
    int m_fileCount;
    char m_dirName[85];
    std::string m_strDirName;
    std::string m_activeLogDate;
    std::ofstream *m_logStream;
    std::string m_filePath;
    LogProfile m_logProfile;

    bool post(LogLevel msgType, const char* fmt);
    bool handle_msg(ACE_Message_Block *pAmb);

};

typedef ACE_Singleton<XGLog, ACE_Recursive_Thread_Mutex > XGLOG_TASK;
#endif
