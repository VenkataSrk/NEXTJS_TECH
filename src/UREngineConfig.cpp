//rapidjson includes
#include "filereadstream.h"
#include "reader.h"
#include "error/en.h"

//local includes
#include "xGateUtil.h"
#include "xglog.h"
#include "UREngineConfig.h"

#define MAX_CONF_BUFFER_SIZE 4096
using namespace rapidjson;

/* 
 * LogProfile class implementation - start
 */
LogProfile::LogProfile() : m_filePath(""), m_fileCount(LOG_FILE_COUNT),  
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
    m_fileLogLevel =  EN_LOG_LEVEL_FATAL;
  } else {
    m_consoleLogLevel =  EN_LOG_LEVEL_OFF;
  }
}
/* 
 * LogProfile class implementation - end
 */
/*
 * RedisConnInfo class implementation - start
 */
RedisConnInfo::RedisConnInfo() : m_strServerHost(""), m_iServerPort(0), m_strPassword(""),
				 m_iSocketTimeout(0), m_strChannel(""), m_strKey("")
{
  m_vChannel.clear();
  m_enMsgType = EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_UNKNOWN;
  m_enRedisMethod = EN_REDIS_METHOD::EN_REDIS_METHOD_PUB_SUB;  
}

RedisConnInfo::~RedisConnInfo()
{
  m_vChannel.clear();
  m_enMsgType = EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_UNKNOWN;
  m_enRedisMethod = EN_REDIS_METHOD::EN_REDIS_METHOD_PUB_SUB;
}

RedisConnInfo::RedisConnInfo(const RedisConnInfo& src)
{
    m_strServerHost.assign(src.m_strServerHost.c_str());
    m_iServerPort = src.m_iServerPort;
    m_strPassword.assign(src.m_strPassword.c_str());
    m_iSocketTimeout = src.m_iSocketTimeout;
    m_vChannel.assign(src.m_vChannel.begin(), src.m_vChannel.end());
    m_strChannel.assign(src.m_strChannel.c_str());
    m_strKey.assign(src.m_strKey.c_str());
    m_enRedisMethod = src.m_enRedisMethod;
    m_enMsgType = src.m_enMsgType;
}

RedisConnInfo& RedisConnInfo::operator=(const RedisConnInfo& src)
{
    m_strServerHost.assign(src.m_strServerHost.c_str());
    m_iServerPort = src.m_iServerPort;
    m_strPassword.assign(src.m_strPassword.c_str());
    m_iSocketTimeout = src.m_iSocketTimeout;
    m_vChannel.assign(src.m_vChannel.begin(), src.m_vChannel.end());
    m_strChannel.assign(src.m_strChannel.c_str());
    m_strKey.assign(src.m_strKey.c_str());
    m_enRedisMethod = src.m_enRedisMethod;
    m_enMsgType = src.m_enMsgType;
    return *this;
}
/*
 * RedisConnInfo class implementation - end
 */

/*
 * RedisModuleProfile class implementation - start
 */
RedisModuleProfile::RedisModuleProfile()
{
}

RedisModuleProfile::~RedisModuleProfile()
{
}

RedisModuleProfile::RedisModuleProfile(const RedisModuleProfile& src)
{
  m_subRedisConnInfo = src.m_subRedisConnInfo;
}

RedisModuleProfile& RedisModuleProfile::operator=(const RedisModuleProfile& src)
{
  m_subRedisConnInfo = src.m_subRedisConnInfo;
  return *this;
}
/*
 * RedisModuleProfile class implementation - end
 */
/*
 * IvrControllerProfile class implementation - start
 */
IvrControllerProfile::IvrControllerProfile() : m_directAccessNumber(""), m_extnAccessNumber(""),
        m_responseRepeatCount(0), m_maxCallCapacity(0), m_maxRecordDuration(0),m_ivrMultiLevelXml("")
{
}

IvrControllerProfile::~IvrControllerProfile()
{
}

IvrControllerProfile::IvrControllerProfile(const IvrControllerProfile& src)
{
}

IvrControllerProfile& IvrControllerProfile::operator=(const IvrControllerProfile& src)
{
        m_directAccessNumber = src.m_directAccessNumber;
        m_extnAccessNumber = src.m_extnAccessNumber;
        m_responseRepeatCount = src.m_responseRepeatCount;
        m_maxCallCapacity = src.m_maxCallCapacity;
        m_maxRecordDuration = src.m_maxRecordDuration;
        m_ivrMultiLevelXml=src.m_ivrMultiLevelXml;
  	return *this;
}
/*
 * IvrControllerProfile class implementation - end
 */

/*
 * URConfControllerProfile class implementation - start
 */
URConfControllerProfile::URConfControllerProfile() : m_db_ValidateExtnURL(""), m_db_ValidatePinURL(""),
				m_db_UserAorDetailURL(""), m_db_UserAuthURL(""), m_db_UserRegURL("")
{
}

URConfControllerProfile::~URConfControllerProfile()
{
}

URConfControllerProfile::URConfControllerProfile(const URConfControllerProfile& src)
{
}

URConfControllerProfile& URConfControllerProfile::operator=(const URConfControllerProfile& src)
{
         m_db_ValidateExtnURL = src.m_db_ValidateExtnURL;
         m_db_ValidatePinURL = src.m_db_ValidatePinURL;
         m_db_UserAorDetailURL = src.m_db_UserAorDetailURL;
         m_db_UserAuthURL = src.m_db_UserAuthURL;
         m_db_UserRegURL = src.m_db_UserRegURL;
  	 return *this;

}
/*
 * URConfControllerProfile class implementation - end
 */

/* 
 * XGConfig class implementation - start
 */
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
  //configPath = "/home/preet/unifiedringengineplatform";

  if(configPath) {
    XGLOG_INFO("checkConfigPath:: Conf Bridge Service will read 'urengine.conf' file from path '%s'", configPath);
    m_configPath = configPath;
    m_configPath += CONFIG_FILE_NAME;
  } else {
    XGLOG_ERROR("checkConfigPath:: 'URENGINE_BASE' environment variable is missing !!");
    return false;
  }

  return true;
}

bool XGConfig::readConfig(void)
{
  if(!parseJsonConfigFile()) {
    XGLOG_ERROR("readConfig failed while parsing json config file %s !", m_configPath.c_str());
    return false;
  } else if(!readCommonDetails()) {
    XGLOG_ERROR("readConfig failed while reading common details !");
    return false;
  } else if(!readLogProfile()) {
    XGLOG_ERROR("readConfig failed while reading log profile details !");
    return false;
  } else if(!readIvrControllerProfile()) {
    XGLOG_ERROR("readConfig failed while reading ivr controller profile details !");
    return false;
  } else if(!readURConfControllerProfile()) {
    XGLOG_ERROR("readConfig failed while reading urconfbridge controller profile details !");
    return false;
  } else if(!readMediaModuleProfile()) {
    XGLOG_ERROR("readConfig failed while reading sip module profile details !");
    return false;
  //}else if(!readSipModuleProfile()) {
    //XGLOG_ERROR("readConfig failed while reading media module profile details !");
    //return false;
  } else if(!readRedisModuleProfile()) {
    XGLOG_ERROR("readConfig failed while reading redis module profile details !");
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
    XGLOG_ERROR("parseJsonConfigFile:%s failed. due to json parse error: %s !", confBuffer, GetParseError_En(ok.Code()));
    return false;
  } else if(!m_configDoc.IsObject()) {
    XGLOG_ERROR("parseJsonConfigFile failed. not able to create document object !");
    return false;
  } else if(!m_configDoc.HasMember("log_profile")) {
    XGLOG_ERROR("parseJsonConfigFile failed. 'log_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  } else if(!m_configDoc.HasMember("ivr_controller_profile")) {
    XGLOG_ERROR("parseJsonConfigFile failed. 'ivr_controller_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  } else if(!m_configDoc.HasMember("urconf_controller_profile")) {
    XGLOG_ERROR("parseJsonConfigFile failed. 'urconf_controller_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
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
    m_logProfile.setFileLogLevel(logProfiles["verbosity_console"].GetString());
  } else {
    XGLOG_ERROR("readLogProfile failed. 'verbosity_console' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(logProfiles.HasMember("verbosity_file")) {
    m_logProfile.setConsoleLogLevel(logProfiles["verbosity_file"].GetString());
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

bool XGConfig::readIvrControllerProfile()
{
  if(!m_configDoc.HasMember("ivr_controller_profile")) {
    XGLOG_ERROR("readIvrControllerProfile failed. 'ivr_controller_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &ivrControllerProfiles = m_configDoc["ivr_controller_profile"];
  if(ivrControllerProfiles.HasMember("direct_access_number")) {
    m_ivrControllerProfile.m_directAccessNumber = ivrControllerProfiles["direct_access_number"].GetString();
  } else {
    XGLOG_ERROR("readIvrControllerProfile failed. 'direct_access_number' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(ivrControllerProfiles.HasMember("extension_access_number")) {
    m_ivrControllerProfile.m_extnAccessNumber = ivrControllerProfiles["extension_access_number"].GetString();
  } else {
    XGLOG_ERROR("readIvrControllerProfile failed. 'extension_access_number' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(ivrControllerProfiles.HasMember("response_repeat_count")) {
    m_ivrControllerProfile.m_responseRepeatCount = ivrControllerProfiles["response_repeat_count"].GetInt();
  } else {
    XGLOG_ERROR("readIvrControllerProfile failed. 'response_repeat_count' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(ivrControllerProfiles.HasMember("max_call_capacity")) {
    m_ivrControllerProfile.m_maxCallCapacity = ivrControllerProfiles["max_call_capacity"].GetInt();
  } else {
    XGLOG_ERROR("readIvrControllerProfile failed. 'max_call_capacity' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(ivrControllerProfiles.HasMember("max_record_duration")) {
    m_ivrControllerProfile.m_maxRecordDuration = ivrControllerProfiles["max_record_duration"].GetInt();
  } else {
    XGLOG_ERROR("readIvrControllerProfile failed. 'max_record_duration' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if (ivrControllerProfiles.HasMember("server_config_url")) {
    m_ivrControllerProfile.m_db_serverInfoURL = ivrControllerProfiles["server_config_url"].GetString();
  }
  else {
    XGLOG_ERROR("readIvrControllerProfile failed. 'server_config_url' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if (ivrControllerProfiles.HasMember("multi_level_ivr_url")) {
    m_ivrControllerProfile.m_db_multiLevelIvrURL = ivrControllerProfiles["multi_level_ivr_url"].GetString();
  }
  else {
    XGLOG_ERROR("readIvrControllerProfile failed. 'multi_level_ivr_url' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  XGLOG_INFO("Succesfully read ivr controller profile details from '%s' file", m_configPath.c_str());

  return true;
}

bool XGConfig::readURConfControllerProfile()
{
  if(!m_configDoc.HasMember("urconf_controller_profile")) {
    XGLOG_ERROR("readURConfControllerProfile failed. 'urconf_controller_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &urconfControllerProfiles = m_configDoc["urconf_controller_profile"];
  if (urconfControllerProfiles.HasMember("validate_extn_url")) {
    m_urConfControllerProfile.m_db_ValidateExtnURL = urconfControllerProfiles["validate_extn_url"].GetString();
  }
  else {
    XGLOG_ERROR("readURConfControllerProfile failed. 'validate_extn_url' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if (urconfControllerProfiles.HasMember("validate_pin_url")) {
    m_urConfControllerProfile.m_db_ValidatePinURL = urconfControllerProfiles["validate_pin_url"].GetString();
  }
  else {
    XGLOG_ERROR("readURConfControllerProfile failed. 'validate_pin_url' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }
  if(urconfControllerProfiles.HasMember("user_aor_detail_url")) {
    m_urConfControllerProfile.m_db_UserAorDetailURL = urconfControllerProfiles["user_aor_detail_url"].GetString();
  } else {
    XGLOG_ERROR("readURConfControllerProfile failed. 'user_aor_detail_url' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(urconfControllerProfiles.HasMember("user_auth_url")) {
    m_urConfControllerProfile.m_db_UserAuthURL = urconfControllerProfiles["user_auth_url"].GetString();
  } else {
    XGLOG_ERROR("readURConfControllerProfile failed. 'user_auth_url' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(urconfControllerProfiles.HasMember("user_reg_url")) {
    m_urConfControllerProfile.m_db_UserRegURL = urconfControllerProfiles["user_reg_url"].GetString();
  } else {
    XGLOG_ERROR("readURConfControllerProfile failed. 'user_reg_url' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }
  XGLOG_INFO("Succesfully read acd controller profile details from '%s' file", m_configPath.c_str());

  return true;
}

bool XGConfig::readMediaModuleProfile()
{
  if(!m_configDoc.HasMember("media_module_profile")) {
    XGLOG_ERROR("readMediaModuleProfile failed. 'media_module_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &mediaModuleProf = m_configDoc["media_module_profile"];
  if(mediaModuleProf.HasMember("media_tcp_listen_port")) {
    m_mediaModuleProfile.mediaTcpListenPort = mediaModuleProf["media_tcp_listen_port"].GetInt();
  } else {
    XGLOG_ERROR("readMediaModuleProfile failed. 'media_tcp_listen_port' detail is missing in '%s' file !", m_configPath.c_str());
    //return false;
  }

  if(mediaModuleProf.HasMember("mbclink_tcp_listen_port")) {
    m_mediaModuleProfile.mbcLinkTcpListenPort = mediaModuleProf["mbclink_tcp_listen_port"].GetInt();
  } else {
    XGLOG_ERROR("readMediaModuleProfile failed. 'mbclink_tcp_listen_port' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

 return true;
}

bool XGConfig::readRedisModuleProfile()
{
  if(!m_configDoc.HasMember("redis_module_profile")) {
    XGLOG_ERROR("readRedisModuleProfile failed. 'redis_module_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &redisModProfile = m_configDoc["redis_module_profile"];
  if(redisModProfile.HasMember("sub"))
  {
    const Value &subRedisInfo = redisModProfile["sub"];
    for(Value::ConstValueIterator itr = subRedisInfo.Begin(); itr != subRedisInfo.End(); ++itr)
    {
      RedisConnInfo * redisConnInfo = new RedisConnInfo();
      for(Value::ConstMemberIterator iter = itr->MemberBegin(); iter != itr->MemberEnd(); ++iter)
      {
        if(strncmp(iter->name.GetString(),"msg_type", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_enMsgType = (EN_REDIS_MSG_TYPE)iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"server_host", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_strServerHost = iter->value.GetString();
        }
        else if(strncmp(iter->name.GetString(),"server_port", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_iServerPort = iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"password", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_strPassword = iter->value.GetString();
        }
        else if(strncmp(iter->name.GetString(),"socket_timeout", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_iSocketTimeout = iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"channel", sizeof(iter->name.GetString()))==0)
        {
          const Value &channelList = iter->value;
          for(Value::ConstValueIterator chItr = channelList.Begin(); chItr != channelList.End(); ++chItr)
          {
            redisConnInfo->m_vChannel.push_back(chItr->GetString());
          }
        }
        else
        {
          XGLOG_ERROR("readRedisModuleProfile failed. detail is missing in '%s' file !", m_configPath.c_str());
        }
      }
      m_redisModProfile.m_mapSubRedisConnInfo.insert(std::pair<EN_REDIS_MSG_TYPE, RedisConnInfo*>(redisConnInfo->m_enMsgType, redisConnInfo));
    }
  }
  if(redisModProfile.HasMember("pub"))
  {
    const Value &pubRedisInfo = redisModProfile["pub"];
    for(Value::ConstValueIterator itr = pubRedisInfo.Begin(); itr != pubRedisInfo.End(); ++itr)
    {
      RedisConnInfo * redisConnInfo = new RedisConnInfo();
      for(Value::ConstMemberIterator iter = itr->MemberBegin(); iter != itr->MemberEnd(); ++iter)
      {
        if(strncmp(iter->name.GetString(),"msg_type", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_enMsgType = (EN_REDIS_MSG_TYPE)iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"redis_method", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_enRedisMethod = (EN_REDIS_METHOD)iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"server_host", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_strServerHost = iter->value.GetString();
        }
        else if(strncmp(iter->name.GetString(),"server_port", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_iServerPort = iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"password", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_strPassword = iter->value.GetString();
        }
        else if(strncmp(iter->name.GetString(),"socket_timeout", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_iSocketTimeout = iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"channel", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_strChannel = iter->value.GetString();
        }
        else if(strncmp(iter->name.GetString(),"channel_list", sizeof(iter->name.GetString()))==0)
        {
          const Value &channelList = iter->value;
          for(Value::ConstValueIterator chItr = channelList.Begin(); chItr != channelList.End(); ++chItr)
          {
            redisConnInfo->m_vChannel.push_back(chItr->GetString());
          }
        }
        else
        {
          XGLOG_ERROR("readredisModProfile failed. detail is missing in '%s' file !", m_configPath.c_str());
        }
      }
      m_redisModProfile.m_mapPubRedisConnInfo.insert(std::pair<EN_REDIS_MSG_TYPE, RedisConnInfo*>(redisConnInfo->m_enMsgType, redisConnInfo));
    }
  }

  if(redisModProfile.HasMember("get"))
  {
    const Value &getRedisInfo = redisModProfile["get"];
    for(Value::ConstValueIterator itr = getRedisInfo.Begin(); itr != getRedisInfo.End(); ++itr)
    {
      RedisConnInfo * redisConnInfo = new RedisConnInfo();
      for(Value::ConstMemberIterator iter = itr->MemberBegin(); iter != itr->MemberEnd(); ++iter)
      {
        if(strncmp(iter->name.GetString(),"get_type", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_enGetType = (EN_REDIS_GET_TYPE)iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"server_host", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_strServerHost = iter->value.GetString();
        }
        else if(strncmp(iter->name.GetString(),"server_port", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_iServerPort = iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"password", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_strPassword = iter->value.GetString();
        }
        else if(strncmp(iter->name.GetString(),"socket_timeout", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_iSocketTimeout = iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"key", sizeof(iter->name.GetString()))==0)
        {
          redisConnInfo->m_strKey = iter->value.GetString();
        }
        else
        {
          XGLOG_ERROR("readredisModProfile failed. detail is missing in '%s' file !", m_configPath.c_str());
        }
      }
      m_redisModProfile.m_mapGetRedisConnInfo.insert(std::pair<EN_REDIS_GET_TYPE, RedisConnInfo*>(redisConnInfo->m_enGetType, redisConnInfo));
    }
  }
 return true;
}

bool XGConfig::readSipModuleProfile()
{
  if(!m_configDoc.HasMember("sip_module_profile")) {
    XGLOG_ERROR("readSipModuleProfile failed. 'sip_module_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &sipModuleProf = m_configDoc["sip_module_profile"];
  if(sipModuleProf.HasMember("register_request_uri")) {
    m_sipModuleProfile.registerReqUri = sipModuleProf["register_request_uri"].GetString();
  } else {
    XGLOG_ERROR("readSipModuleProfile failed. 'register_request_uri' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(sipModuleProf.HasMember("register_to_uri")) {
    m_sipModuleProfile.registerToUri = sipModuleProf["register_to_uri"].GetString();
  } else {
    XGLOG_ERROR("readSipModuleProfile failed. 'register_to_uri' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }
#if 0
  if(sipModuleProf.HasMember("register_contact_uri")) {
    m_sipModuleProfile.registerContactUri = sipModuleProf["register_contact_uri"].GetString();
  } else {
    XGLOG_ERROR("readSipModuleProfile failed. 'register_contact_uri' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }
#endif
  if(sipModuleProf.HasMember("register_username")) {
    m_sipModuleProfile.registerUsername = sipModuleProf["register_username"].GetString();
  } else {
    XGLOG_ERROR("readSipModuleProfile failed. 'register_username' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(sipModuleProf.HasMember("register_password")) {
    m_sipModuleProfile.registerPassword = sipModuleProf["register_password"].GetString();
  } else {
    XGLOG_ERROR("readSipModuleProfile failed. 'register_password' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  XGLOG_INFO("Succesfully read sip module  profile details from '%s' file", m_configPath.c_str());

  return true;
}

SipModuleProfile& XGConfig::getSipModConfig()
{
  return m_sipModuleProfile;
}

URConfControllerProfile& XGConfig::getURConfControllerProfile()
{
  return m_urConfControllerProfile;
}

void XGConfig::setCallBack(IURModuleCallBack* pCallBack)
{
moduleCallBack = pCallBack;
}

IURModuleCallBack* XGConfig::getCallBack()
{
   return moduleCallBack;
}
