//rapidjson includes
#include "filereadstream.h"
#include "reader.h"
#include "error/en.h"

//local includes
#include "xGateUtil.h"
#include "xglog.h"
#include "XGConfig.h"

#define MAX_CONF_BUFFER_SIZE 4096

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
    m_fileLogLevel =  EN_LOG_LEVEL_FATAL;
  } else {
    m_consoleLogLevel =  EN_LOG_LEVEL_OFF;
  }
  printf ("logLevel: %s m_consoleLogLevel: %d ",logLevel, m_consoleLogLevel);
}
/* 
 * LogProfile class implementation - end
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

DBConnInfo::DBConnInfo():m_db_server_ip(""),m_db_user_name(""),m_db_password(""),m_db_name(""),m_db_serv_port("")
{
}

DBConnInfo::~DBConnInfo()
{
}

DBConnInfo::DBConnInfo(const DBConnInfo &src)
{
m_db_server_ip.assign(src.m_db_server_ip.c_str());
m_db_user_name.assign(src.m_db_user_name.c_str());
m_db_password.assign(src.m_db_password.c_str());
m_db_name.assign(src.m_db_name.c_str());
m_db_serv_port.assign(src.m_db_serv_port.c_str());
}

DBConnInfo& DBConnInfo::operator=(const DBConnInfo& src)
{
m_db_server_ip.assign(src.m_db_server_ip.c_str());
m_db_user_name.assign(src.m_db_user_name.c_str());
m_db_password.assign(src.m_db_password.c_str());
m_db_name.assign(src.m_db_name.c_str());
m_db_serv_port.assign(src.m_db_serv_port.c_str());
return *this;
}

/*
 * DBModuleProfile class implementation - start
 */
DBModuleProfile::DBModuleProfile()
{
}

DBModuleProfile::~DBModuleProfile()
{
}

DBModuleProfile::DBModuleProfile(const DBModuleProfile& src)
{
  m_mapDbConnInfo = src.m_mapDbConnInfo;
}

DBModuleProfile& DBModuleProfile::operator=(const DBModuleProfile& src)
{
  m_mapDbConnInfo = src.m_mapDbConnInfo;
  return *this;
}

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
  } else if(!readDBModuleProfile()){
		XGLOG_ERROR("readConfig failed while reading DB module profile details !");
		return false;
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
    XGLOG_ERROR("parseJsonConfigFile failed. due to json parse error: %s and data:%s!", GetParseError_En(ok.Code()), confBuffer);
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
  } else if(!m_configDoc.HasMember("read_db_profile")) {
    XGLOG_ERROR("parseJsonConfigFile 'read_db_profile' details is missing, failed to read from %s file !", m_configPath.c_str());
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

bool XGConfig::readDBModuleProfile()
{
  if (!m_configDoc.HasMember("read_db_profile"))
  {
    XGLOG_ERROR("readDBModuleProfile failed. 'read_db_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &dbprofile = m_configDoc["read_db_profile"];
	for(Value::ConstValueIterator itr = dbprofile.Begin(); itr != dbprofile.End(); ++itr)
	{
      DBConnInfo* dbConnInfo = new DBConnInfo();
      for(Value::ConstMemberIterator iter = itr->MemberBegin(); iter != itr->MemberEnd(); ++iter)
      {
        if(strncmp(iter->name.GetString(),"msg_type", sizeof(iter->name.GetString()))==0)
        {
          dbConnInfo->m_enMsgType = (EN_DB_MSG_TYPE)iter->value.GetInt();
        }
        else if(strncmp(iter->name.GetString(),"db_sp_name", sizeof(iter->name.GetString()))==0)
        {
          dbConnInfo->m_sp_name = iter->value.GetString();
        }
				else if(strncmp(iter->name.GetString(),"db_server_ip", sizeof(iter->name.GetString()))==0)
				{
					dbConnInfo->m_db_server_ip = iter->value.GetString();
				}
				else if(strncmp(iter->name.GetString(),"db_serv_port", sizeof(iter->name.GetString()))==0)
				{
					dbConnInfo->m_db_serv_port = iter->value.GetString();
				}
				else if(strncmp(iter->name.GetString(),"db_user_name", sizeof(iter->name.GetString()))==0)
				{
					dbConnInfo->m_db_user_name = iter->value.GetString();
				}
				else if(strncmp(iter->name.GetString(),"db_password", sizeof(iter->name.GetString()))==0)
				{
					dbConnInfo->m_db_password = iter->value.GetString();
				}
				else if(strncmp(iter->name.GetString(),"db_name", sizeof(iter->name.GetString()))==0)
				{
					dbConnInfo->m_db_name = iter->value.GetString();
				}
        else
        {
          XGLOG_ERROR("readDBModuleProfile failed. detail is missing in '%s' file !", m_configPath.c_str());
        }
      }
      m_dbModProfile.m_mapDbConnInfo.insert(std::pair<EN_DB_MSG_TYPE, DBConnInfo*>(dbConnInfo->m_enMsgType, dbConnInfo));
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
      auto itr2 = m_redisModProfile.m_mapPubRedisConnInfo.insert(std::pair<EN_REDIS_MSG_TYPE, RedisConnInfo*>(redisConnInfo->m_enMsgType, redisConnInfo));
			if(itr2.second)
			  std::cout<<"\n\nCame inside inserted\n\n";
    }
  }
  if(redisModProfile.HasMember("get"))
  {
  std::cout<<"\n\nCame inside get\n\n";
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

void XGConfig::setCallBack(IURModuleCallBack* pCallBack)
{
  moduleCallBack = pCallBack;
}

IURModuleCallBack* XGConfig::getCallBack()
{
  return moduleCallBack;
}
