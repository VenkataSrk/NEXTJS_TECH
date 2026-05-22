//local includes
//#include "xGateUtil.h"
#include "CAlog.h"
#include "CAExceplog.h"
#include "LogProfile.h"


//using namespace rapidjson;

/* 
 * LogProfile class implementation - start
 */
LogProfile::LogProfile() : mfilePath(""), m_fileCount(LOG_FILE_COUNT),  
  m_fileSize(LOG_FILE_SIZE), m_fileColor(true), m_fileLogLevel(EN_LOG_LEVEL_INFO),
  m_consoleLogLevel(EN_LOG_LEVEL_INFO)
{
}

LogProfile::~LogProfile()
{
}

LogProfile::LogProfile(const LogProfile& src)
{
}

LogProfile& LogProfile::operator=(const LogProfile& src)
{
  mfilePath = src.mfilePath;
  m_fileCount = src.m_fileCount;
  m_fileSize = src.m_fileSize;
  m_fileColor = src.m_fileColor;
  m_fileLogLevel = src.m_fileLogLevel;
  m_consoleLogLevel = src.m_consoleLogLevel;
  return *this;
}
#if 0
void LogProfile::setFileLogLevel(const char *logLevel)
{
  if(!strcasecmp(logLevel,"trace")) {
    m_fileLogLevel =  EN_LOG_LEVEL_TRACE;
  } else if(!strcasecmp(logLevel,"debug")) {
    m_fileLogLevel =  EN_LOG_LEVEL_DEBUG;
  } else if(!strcasecmp(logLevel,"info")){
    m_fileLogLevel =  EN_LOG_LEVEL_INFO;
  } else if(!strcasecmp(logLevel,"warning")) {	
    m_fileLogLevel =  EN_LOG_LEVEL_WARN;
  } else if(!strcasecmp(logLevel,"error")) {
    m_fileLogLevel =  EN_LOG_LEVEL_ERROR;
  } else if(!strcasecmp(logLevel,"critical")) {
    m_fileLogLevel =  EN_LOG_LEVEL_CRITICAL;
  } else if(!strcasecmp(logLevel,"fatal")) {
    m_fileLogLevel =  EN_LOG_LEVEL_FATAL;
  } else {
    m_fileLogLevel =  EN_LOG_LEVEL_OFF;
  }
}

void LogProfile::setConsoleLogLevel(const char* logLevel)
{
  if(!strcasecmp(logLevel,"trace")) {
    m_consoleLogLevel =  EN_LOG_LEVEL_TRACE;
  } else if(!strcasecmp(logLevel,"debug")) {
    m_consoleLogLevel =  EN_LOG_LEVEL_DEBUG;
  } else if(!strcasecmp(logLevel,"info")){
    m_consoleLogLevel=  EN_LOG_LEVEL_INFO;
  } else if(!strcasecmp(logLevel,"warning")) {
    m_consoleLogLevel =  EN_LOG_LEVEL_WARN;
  } else if(!strcasecmp(logLevel,"error")) {
    m_consoleLogLevel =  EN_LOG_LEVEL_ERROR;
  } else if(!strcasecmp(logLevel,"critical")) {
    m_consoleLogLevel =  EN_LOG_LEVEL_CRITICAL;
  } else if(!strcasecmp(logLevel,"fatal")) {
    m_fileLogLevel =  EN_LOG_LEVEL_FATAL;
  } else {
    m_consoleLogLevel =  EN_LOG_LEVEL_OFF;
  }
}
/* 
 * LogProfile class implementation - end
 */

bool XGConfig::readLogProfile()
{
  if(!m_configDoc.HasMember("log_profile")) {
    CAlog_ERROR("readLogProfile failed. 'log_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &logProfiles = m_configDoc["log_profile"];
  if(logProfiles.HasMember("file_path")) {
    m_logProfile.m_filePath = logProfiles["file_path"].GetString();
  } else {
    CAlog_ERROR("readLogProfile failed. 'file_path' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(logProfiles.HasMember("file_count")) {
    m_logProfile.m_fileCount = logProfiles["file_count"].GetInt();
  } else {
    CAlog_ERROR("readLogProfile failed. 'file_count' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(logProfiles.HasMember("file_size")) {
    m_logProfile.m_fileSize  = logProfiles["file_size"].GetInt();
  } else {
    CAlog_ERROR("readLogProfile failed. 'file_size' detail is missing in '%s' file !", m_configPath.c_str());
    return false;  
  }

  if(logProfiles.HasMember("verbosity_console")) {
    m_logProfile.setFileLogLevel(logProfiles["verbosity_console"].GetString());
  } else {
    CAlog_ERROR("readLogProfile failed. 'verbosity_console' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(logProfiles.HasMember("verbosity_file")) {
    m_logProfile.setConsoleLogLevel(logProfiles["verbosity_file"].GetString());
  } else {
    CAlog_ERROR("readLogProfile failed. 'verbosity_file' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }
  CAlog_INFO("Succesfully read log profile details from '%s' file", m_configPath.c_str());

  //re-configure log module
  CAlog *logInstance = CAlog_TASK::instance();
  if(logInstance) {
    if(logInstance->loadlogProfile(m_logProfile)) {
      CAlog_INFO("CAlog module re-initialized successfully based on configuration");
    } else {
      CAlog_ERROR("CAlog module re-initialization failed !");
      return false;
    }
  }

  return true;
}
#endif
