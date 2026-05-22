//rapidjson includes
#include "filereadstream.h"
#include "reader.h"
#include "error/en.h"

//local includes
#include "xGateUtil.h"
#include "xglog.h"
#include "XGConfig.h"

#define MAX_CONF_BUFFER_SIZE 2048

using namespace rapidjson;

/* 
 * LogProfile class implementation - start
 */
LogProfile::LogProfile() : m_filePath(""), m_fileCount(LOG_FILE_COUNT),  
  m_fileSize(LOG_FILE_SIZE), m_fileColor(true), m_fileLogLevel(EN_LOG_LEVEL_ERROR),
  m_consoleLogLevel(EN_LOG_LEVEL_ERROR)
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
  m_filePath = src.m_filePath;
  m_fileCount = src.m_fileCount;
  m_fileSize = src.m_fileSize;
  m_fileColor = src.m_fileColor;
  m_fileLogLevel = src.m_fileLogLevel;
  m_consoleLogLevel = src.m_consoleLogLevel;
  return *this;
}

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
    m_consoleLogLevel =  EN_LOG_LEVEL_FATAL;
  } else {
    m_consoleLogLevel =  EN_LOG_LEVEL_OFF;
  }
  printf ("logLevel: %s m_consoleLogLevel: %d ",logLevel, m_consoleLogLevel);
}
/* 
 * LogProfile class implementation - end
 */

/*
 * SttConnInfo class implementation - start
 */
SttConnInfo::SttConnInfo() : m_strServerHost(""),m_strServerPort(""),m_strConnectType(0)
{
}

SttConnInfo::~SttConnInfo()
{
}

SttConnInfo::SttConnInfo(const SttConnInfo &src)
{
  m_strServerHost = src.m_strServerHost.c_str();
  m_strServerPort = src.m_strServerPort.c_str();
  m_strConnectType = src.m_strConnectType;
}

SttConnInfo& SttConnInfo::operator=(const SttConnInfo& src)
{
  m_strServerHost = src.m_strServerHost.c_str();
  m_strServerPort = src.m_strServerPort.c_str();
  m_strConnectType = src.m_strConnectType;
  return *this;
}
/*
 * SttConnInfo class implementation - end
 */

/*
 * SttProfile class implementation - start
 */
SttProfile::SttProfile()
{
}

SttProfile::~SttProfile()
{
}

SttProfile::SttProfile(const SttProfile &src)
{
  m_smaticsConnInfo = src.m_smaticsConnInfo;
  m_inhouseConnInfo = src.m_inhouseConnInfo;
}

SttProfile& SttProfile::operator=(const SttProfile& src)
{
  m_smaticsConnInfo = src.m_smaticsConnInfo;
  m_inhouseConnInfo = src.m_inhouseConnInfo;
  return *this;
}
/*
 * SttProfile class implementation - end
 */

/* 
 * XGConfig class implementation - start
 */
XGConfig::XGConfig()
{
  XGLOG_INFO("XGConfig construction intializing xGate Configurations");
  //init();
}

XGConfig::~XGConfig()
{
  deInit();
}

bool XGConfig::init(...)
{
  if(!checkConfigPath()) {
    return false;
  }
  if(!readConfig()) {
    XGLOG_ERROR("read configuration file '%s' failed !", m_configPath.c_str());
    return false;
  }
  return true;
}

bool XGConfig::deInit()
{
  return true;
}

bool XGConfig::checkConfigPath() 
{
  char* configPath;
  configPath = getenv(ENV_BASE);

  if(configPath) {
    XGLOG_INFO("XGConfig will read '%s' file from path '%s'", CONFIG_FILE_NAME, configPath);
    m_configPath = configPath;
    m_configPath += CONFIG_FILE_NAME;
  } else {
    XGLOG_ERROR("XGConfig failed. '%s' environment variable is missing !", ENV_BASE);
    return false;
  }

  return true;
}

bool XGConfig::readConfig(void)
{
  if(!parseJsonConfigFile()) {
    XGLOG_ERROR("readConfig failed while parsing json config file '%s' !", m_configPath.c_str());
    return false;
  } else if(!readCommonDetails()) {
    XGLOG_ERROR("readConfig failed while reading common details !");
    return false;
  } else if(!readLogProfile()) {
    XGLOG_ERROR("readConfig failed while reading log profile details !");
    return false;
  } else if(!readStunConfig(IP_TYPE_4) && !readStunConfig(IP_TYPE_6)) {
    XGLOG_ERROR("readConfig failed while reading stun details for ip4 and ip6!");
    return false;
  } else if(!readMediaModuleProfile()) {
    XGLOG_ERROR("readConfig failed while reading sip module profile details !");
    return false;
  } else if(!readRecordingProfile()) {
    XGLOG_ERROR("readConfig failed while reading recording profile details !");
    return false;
  }else if(!readSttProfile()) {
    XGLOG_ERROR("readConfig failed while reading stt profile details !");
    return false;
  }

  return true;
}

bool XGConfig::parseJsonConfigFile()
{
  char confBuffer[MAX_CONF_BUFFER_SIZE];
  FILE* fp = fopen(m_configPath.c_str(), "rb");
  if(fp == NULL) {
    XGLOG_ERROR("not able to open file %s", m_configPath.c_str());
    return false;
  }
  FileReadStream is(fp, confBuffer, MAX_CONF_BUFFER_SIZE);
  fclose(fp);

  //ParseResult ok = m_configDoc.Parse(confBuffer);
  ParseResult ok = m_configDoc.Parse<kParseCommentsFlag>(confBuffer);
  if(!ok) {
    XGLOG_ERROR("parseJsonConfigFile failed. due to json parse error: %s !", GetParseError_En(ok.Code()));
    return false;
  } else if(!m_configDoc.IsObject()) {
    XGLOG_ERROR("parseJsonConfigFile failed. not able to create document object !");
    return false;
  } else if(!m_configDoc.HasMember("log_profile")) {
    XGLOG_ERROR("parseJsonConfigFile failed. 'log_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  } else if(!m_configDoc.HasMember("media_module_profile")) {
    XGLOG_ERROR("parseJsonConfigFile failed. 'media_module_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  } else if(!m_configDoc.HasMember("stun_config_ip4") && !m_configDoc.HasMember("stun_config_ip6")) {
    XGLOG_ERROR("parseJsonConfigFile 'stun_config' details is missing, failed to read from %s file !", m_configPath.c_str());
  }

  XGLOG_INFO("Successfully parsed json data from %s file", m_configPath.c_str());
  return true; 
}

bool XGConfig::readCommonDetails()
{
  if(m_configDoc.HasMember("local_ip4_addr")) {
    m_localIp4Addr = m_configDoc["local_ip4_addr"].GetString();
  } else {
    XGLOG_ERROR("readCommonDetails failed. 'local_ip4_addr' detail is missing in '%s' file !", m_configPath.c_str());
    return false; 
  }
  if(m_configDoc.HasMember("local_public_addr")) {
    m_localPublicAddr = m_configDoc["local_public_addr"].GetString();
  } else {
    XGLOG_ERROR("readCommonDetails failed. 'local_public_addr' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }
  if(m_configDoc.HasMember("local_ip6_addr")) {
    m_localIp6Addr = m_configDoc["local_ip6_addr"].GetString();
  } else {
    XGLOG_ERROR("readCommonDetails failed. 'local_ip6_addr' detail is missing in '%s' file !", m_configPath.c_str());
    return false; 
  }
  XGLOG_INFO("Successfully read common configuration details from '%s' file", m_configPath.c_str());
  return true;
}

bool XGConfig::readLogProfile()
{
  if(!m_configDoc.HasMember("log_profile")) {
    XGLOG_ERROR("readLogProfile failed. 'log_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &logProfiles = m_configDoc["log_profile"];
  if(logProfiles.HasMember("file_path")) {
    m_logProfile.m_filePath = logProfiles["file_path"].GetString();
  } else {
    XGLOG_ERROR("readLogProfile failed. 'file_path' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(logProfiles.HasMember("file_count")) {
    m_logProfile.m_fileCount = logProfiles["file_count"].GetInt();
  } else {
    XGLOG_ERROR("readLogProfile failed. 'file_count' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(logProfiles.HasMember("file_size")) {
    m_logProfile.m_fileSize  = logProfiles["file_size"].GetInt();
  } else {
    XGLOG_ERROR("readLogProfile failed. 'file_size' detail is missing in '%s' file !", m_configPath.c_str());
    return false;  
  }

  if(logProfiles.HasMember("verbosity_console")) {
    printf ("console verbosity level is %s \n", logProfiles["verbosity_console"].GetString());
    m_logProfile.setConsoleLogLevel(logProfiles["verbosity_console"].GetString());
  } else {
    XGLOG_ERROR("readLogProfile failed. 'verbosity_console' detail is missing in '%s' file !", m_configPath.c_str());
  }

  if(logProfiles.HasMember("verbosity_file")) {
    printf ("file verbosity level is %s \n", logProfiles["verbosity_file"].GetString());
    m_logProfile.setFileLogLevel(logProfiles["verbosity_file"].GetString());
  } else {
    XGLOG_ERROR("readLogProfile failed. 'verbosity_file' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }
  XGLOG_INFO("Succesfully read log profile details from '%s' file", m_configPath.c_str());
#if 0
  //re-configure log module
  XGLog *logInstance = XGLOG_TASK::instance();
  if(logInstance) {
    if(logInstance->reinit(m_logProfile)) {
      XGLOG_INFO("XGLog module re-initialized successfully based on configuration");
    } else {
      XGLOG_ERROR("XGLog module re-initialization failed !");
      return false;
    }
  }
#endif
  return true;
}

bool XGConfig::readStunConfig(IP_TYPE eIPType)
{
  char *stun_config = NULL;
  
  if (IP_TYPE_4 == eIPType)
    stun_config = (char *)"stun_config_ip4";
  else if (IP_TYPE_6 == eIPType)
    stun_config =(char *) "stun_config_ip6";

  if(!m_configDoc.HasMember("stun_config_ip4") && (IP_TYPE_4 == eIPType)){
    XGLOG_ERROR("readStunConfig failed. 'stun_config for ip4 details is missing in %s file ", m_configPath.c_str());
    return false;
  }
  else if(!m_configDoc.HasMember("stun_config_ip6") && (IP_TYPE_6 == eIPType)){
    XGLOG_ERROR("readStunConfig failed. 'stun_config for ip6 details is missing in %s file ", m_configPath.c_str());
    return false;
  }  
  const Value &StunConfig = m_configDoc[stun_config];

  if(StunConfig.HasMember("username")) {
     m_stunConfig.m_userName = StunConfig["username"].GetString();
     if(m_stunConfig.m_userName.empty()){
        m_stunConfig.m_userName = DEFAULT_STUN_USERNAME;
            XGLOG_ERROR("Stun Username is empty in given config path '%s' and copying the default value !", m_configPath.c_str());
        }
  }
  if(StunConfig.HasMember("password")) {
    m_stunConfig.m_password = StunConfig["password"].GetString();
      if(m_stunConfig.m_password.empty()){
         m_stunConfig.m_password = DEFAULT_STUN_PASSWORD;
            XGLOG_ERROR("Stun Password is empty in given config path '%s' and copying the default value !", m_configPath.c_str());
      }
  }
  if(StunConfig.HasMember("stun_domain")) {
    m_stunConfig.m_stunDomain = StunConfig["stun_domain"].GetString();
      if(m_stunConfig.m_stunDomain.empty()){
        m_stunConfig.m_stunDomain = DEFAULT_STUN_DOMAIN;
        XGLOG_ERROR("StunDomain is empty in given config path '%s' and copying the default value !", m_configPath.c_str());
      }
  }
  if(StunConfig.HasMember("stun_port")) {
    m_stunConfig.m_port = StunConfig["stun_port"].GetUint();
    if(m_stunConfig.m_port == 0){
      m_stunConfig.m_port = DEFAULT_STUN_PORT;
      XGLOG_ERROR("Stun Port is empty in given config path '%s' and copying the default value !", m_configPath.c_str());
    }
  }
   return true;
}

bool XGConfig::readMediaModuleProfile()
{
  if(!m_configDoc.HasMember("media_module_profile")) {
    XGLOG_ERROR("readMediaModuleProfile failed. 'media_module_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &mediaModuleProfile = m_configDoc["media_module_profile"];
  int peerPort(0);
  if(mediaModuleProfile.HasMember("peer_connect_port")) {
    peerPort = mediaModuleProfile["peer_connect_port"].GetInt();
  } else {
    XGLOG_ERROR("readMediaModuleProfile failed. 'peer_connect_port' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  vector<TcpLisentAddr> &peerConnectAddrs = m_mediaModuleProfile.m_peerConnectAddrs;
  if(mediaModuleProfile.HasMember("peer_connect_ip")) {
    const Value &peerConnectIpList = mediaModuleProfile["peer_connect_ip"]; 
    if(peerConnectIpList.IsArray()) {
      Value::ConstValueIterator itr = peerConnectIpList.Begin();  
      for(itr; itr != peerConnectIpList.End(); itr++) {
        string peerIp = (*itr).GetString();
        TcpLisentAddr peerConnectAddr(peerIp, peerPort);  
        peerConnectAddrs.push_back(peerConnectAddr);
      }
    }
  } else {
    XGLOG_ERROR("readMediaModuleProfile failed. 'peer_connect_ip' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  return true;
}

bool XGConfig::readRecordingProfile()
{
  if(!m_configDoc.HasMember("recording_profile")) {
    XGLOG_ERROR("readRecordingProfile failed. 'recording_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &recordingProfile = m_configDoc["recording_profile"];
  int recPort(0);
  if(recordingProfile.HasMember("recording_connect_port")) {
    recPort = recordingProfile["recording_connect_port"].GetInt();
  } else {
    XGLOG_ERROR("readRecordingProfile failed. 'recording_connect_port' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  vector<TcpLisentAddr> &recConnectAddrs = m_recordingProfile.m_peerConnectAddrs;
  if(recordingProfile.HasMember("recording_connect_ip")) {
    const Value &recConnectIpList = recordingProfile["recording_connect_ip"];
    if(recConnectIpList.IsArray()) {
      Value::ConstValueIterator itr = recConnectIpList.Begin();
      for(itr; itr != recConnectIpList.End(); itr++) {
        string recIp = (*itr).GetString();
        TcpLisentAddr recConnectAddr(recIp, recPort);
        recConnectAddrs.push_back(recConnectAddr);
      }
    }
  } else {
    XGLOG_ERROR("readRecordingProfile failed. 'peer_connect_ip' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  return true;
}

bool XGConfig::readSttProfile()
{
  if(!m_configDoc.HasMember("stt_profile")) {
    XGLOG_ERROR("readSttProfile failed. 'stt_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &sttProfile = m_configDoc["stt_profile"];
  if(sttProfile.HasMember("inhouse"))
  {
    const Value &inhouseProfile = sttProfile["inhouse"];
    for(Value::ConstMemberIterator iter = inhouseProfile.MemberBegin(); iter != inhouseProfile.MemberEnd(); ++iter)
    {
      std::string name = iter->name.GetString();
      if(name == "host")
      {
        m_sttProfile.m_inhouseConnInfo.m_strServerHost = iter->value.GetString();
      }
      else if(name == "port")
      {
        m_sttProfile.m_inhouseConnInfo.m_strServerPort = iter->value.GetString();
      }
      else if(name == "conn_type")
      {
        m_sttProfile.m_inhouseConnInfo.m_strConnectType = iter->value.GetInt();
      }
    }
  }
  return true;
}

void XGConfig::setCallBack(IURModuleCallBack* pCallBack)
{
  moduleCallBack = pCallBack;
}

IURModuleCallBack* XGConfig::getCallBack()
{
  return moduleCallBack;
}
