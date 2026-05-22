#ifndef LOGGER_GOBAL
#define LOGGER_GOBAL
#include "CAUtilityInternal.h"
#define  GLID  "GLID1A2B3C4D5E6F7G8H9I"
#define MAXDATA 1024
#define FOMDATA 1200
//ACE related headers 
#include <ace/Task.h>
#include <ace/Singleton.h>
#include <ace/Log_Msg.h>
#include <ace/OS.h>
#include <ace/OS_NS_unistd.h>
#include <list>
#include <sys/stat.h>
#include <filesystem>

class logger{
private:
std::fstream   m_fd;
std::string m_path;
std::string m_date;
std::string m_time;
std::string m_cli;
std::string m_ddi;
std::string m_uuid;
std::string m_appName;
std::string m_trunk;
std::string m_currentDate;
public:
logger()=default;
virtual ~logger()=default;
void getFormattedDateTime();

logger(std::string fileName);

void createGlobalfile(std::string path);
void createSessionFile(std::string path,std::string cli,std::string uuid);
void updateSessionInfo(std::string uuid,std::string app,std::string cli,std::string ddi,std::string trunk);
void formatLog(std::string loglvl,std::string logdata);
static ACE_Log_Priority getLogLevelEnum(const std::string& levelStr);
void writeData(std::string data);
long checkFileSize();
void closeFile();

};






#endif
