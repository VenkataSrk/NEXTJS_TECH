//rapidjson includes
#include "filereadstream.h"
#include "reader.h"
#include "error/en.h"

//local includes
#include "xGateUtil.h"
#include "xglog.h"
#include "UREngineConfig.h"

#define MAX_CONF_BUFFER_SIZE 4096*2

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
 * URWssControllerProfile class implementation - start
 */
URWssControllerProfile::URWssControllerProfile() : m_db_UserRegURL(""), m_db_ConfBridgeIp("")
{
}

URWssControllerProfile::~URWssControllerProfile()
{
}

URWssControllerProfile::URWssControllerProfile(const URWssControllerProfile& src)
{
}

URWssControllerProfile& URWssControllerProfile::operator=(const URWssControllerProfile& src)
{
         m_db_ConfBridgeIp = src.m_db_ConfBridgeIp;
         m_db_UserRegURL = src.m_db_UserRegURL;
         return *this;
}
/*
 * URWssControllerProfile class implementation - end
 */

/*LWSS Roun robin start*/
bool BridgeRouteIpList::addIpAddr(std::string routeIp)
{
    XGLOG_FUNCTION_EN_EX("");
    if (routeIp.empty()){       
        XGLOG_ERROR("Input route IP is empty");
        return false;
    }

    if (ip_index == MAX_ROUTE_IPS_PER_BRIDGE){
        XGLOG_ERROR("Reached MAX_ROUTE_IPS_PER_BRIDGE");
        return false;
    }

    m_addrList[ip_index] = routeIp;
    ip_index++;

    return true;
}

std::string BridgeRouteIpList::getBridgeIpToRoute()
{
    XGLOG_FUNCTION_EN_EX("");
    if (ip_index == 0){
        XGLOG_ERROR("Bridge IP List is Empty");
        return std::string();
    }

    if (to_use_ip_index == ip_index){
        to_use_ip_index = 0;
    }

    std::string routeIp = m_addrList[to_use_ip_index];
    to_use_ip_index++;

    return routeIp;
}

/*LWSS Roun robin end*/

/*LWSS Changes Start*/
LightWeightWSSProfile::LightWeightWSSProfile() : m_default_pbx_ip(""), m_contactWssIp(""), m_db_UserRegURL(""),m_db_UserRegVmeetURL(""),m_isSbcEnabled("false"),m_isReadWhiteListFromDb("false")
{
}

LightWeightWSSProfile::~LightWeightWSSProfile()
{
}

LightWeightWSSProfile::LightWeightWSSProfile(const LightWeightWSSProfile &src)
{
    m_default_pbx_ip = src.m_default_pbx_ip;
    m_contactWssIp = src.m_contactWssIp;
    m_dbQueryIntervalInSec = src.m_dbQueryIntervalInSec;
    m_server_resp_timeout = src.m_server_resp_timeout;
    m_whitelistDomainList = src.m_whitelistDomainList;
    m_msTeamsDomainList   = src.m_msTeamsDomainList;
    m_inboundRouteAddrMap = src.m_inboundRouteAddrMap;
    m_db_UserRegURL = src.m_db_UserRegURL;
    m_db_UserRegVmeetURL = src.m_db_UserRegURL;
    m_pbxUA = src.m_pbxUA;
    m_sipURL = src.m_sipURL;
    m_sipsURL = src.m_sipsURL;
    m_wssURL = src.m_wssURL;
    m_sipPort = src.m_sipPort;
    m_sipsPort = src.m_sipsPort;
    m_wssPort = src.m_wssPort;
    m_certPath = src.m_certPath;
    m_dcType = src.m_dcType;
    m_sipOptionsTimeout = src.m_sipOptionsTimeout;
    m_isSbcEnabled = src.m_isSbcEnabled;
    m_isReadWhiteListFromDb = src.m_isReadWhiteListFromDb;
}

LightWeightWSSProfile &LightWeightWSSProfile::operator=(const LightWeightWSSProfile &src)
{
    m_default_pbx_ip = src.m_default_pbx_ip;
    m_contactWssIp = src.m_contactWssIp;
    m_dbQueryIntervalInSec = src.m_dbQueryIntervalInSec;
    m_server_resp_timeout = src.m_server_resp_timeout;
    m_whitelistDomainList = src.m_whitelistDomainList;
    m_msTeamsDomainList   = src.m_msTeamsDomainList;
    m_inboundRouteAddrMap = src.m_inboundRouteAddrMap;
    m_db_UserRegURL = src.m_db_UserRegURL;
    m_db_UserRegVmeetURL = src.m_db_UserRegURL;
    m_pbxUA = src.m_pbxUA;
    m_sipURL = src.m_sipURL;
    m_sipsURL = src.m_sipsURL;
    m_wssURL = src.m_wssURL;
    m_sipPort = src.m_sipPort;
    m_sipsPort = src.m_sipsPort;
    m_wssPort = src.m_wssPort;
    m_certPath = src.m_certPath;
    m_dcType = src.m_dcType;
    m_sipOptionsTimeout = src.m_sipOptionsTimeout;
    m_isSbcEnabled = src.m_isSbcEnabled;
    m_isReadWhiteListFromDb = src.m_isReadWhiteListFromDb;

    return *this;
}

/*LWSS Changes End*/
/*LWSS Roun robin start*/
bool LightWeightWSSProfile::addInboundRoute (string host_url, string bridge_ip)
{
    XGLOG_FUNCTION_EN_EX("");
    if (host_url.empty() || bridge_ip.empty()){
        XGLOG_ERROR("Input host-url and bridge ip are empty");
        return false;
    }

    BridgeRouteIpList *ptrBridgeIpList = nullptr;

    std::map<std::string, BridgeRouteIpList*>::iterator itr;
    itr = this->m_inboundRouteAddrMap.find(host_url);

    if(itr == this->m_inboundRouteAddrMap.end()) {
        ptrBridgeIpList = new BridgeRouteIpList();
    } else {
        ptrBridgeIpList = itr->second;
    }

    bool ret = ptrBridgeIpList->addIpAddr(bridge_ip);

    this->m_inboundRouteAddrMap[host_url] = ptrBridgeIpList;

    return ret;
}

std::string LightWeightWSSProfile::getInboundRouteIp(std::string host_url)
{
    XGLOG_FUNCTION_EN_EX("");
    if (host_url.empty()){
        XGLOG_ERROR("Input host-url is empty");
        return std::string(); 
    }

    BridgeRouteIpList *ptrBridgeIpList = nullptr;
    std::map<std::string, BridgeRouteIpList*>::iterator itr;
    itr = this->m_inboundRouteAddrMap.find(host_url);

    if(itr == this->m_inboundRouteAddrMap.end()) {
        XGLOG_ERROR("inboundRouteAddrMap is empty");
        return std::string();
    }

    ptrBridgeIpList = itr->second;
    if (ptrBridgeIpList == nullptr) {
        XGLOG_ERROR("ptrBridgeIpListptrBridgeIpListis empty");
        return std::string();
    }

    return ptrBridgeIpList->getBridgeIpToRoute();
}
/*
* Add List of IP/Domain to white List 
*/
bool LightWeightWSSProfile::addIPListToWhiteList(std::vector<std::string> ip_list)
{
		std::lock_guard<std::mutex> lock(_mutex);
		for (auto &it : ip_list)
		{
				auto itt = std::find(m_whitelistDomainList.begin(), m_whitelistDomainList.end(), (it));
				if (itt == m_whitelistDomainList.end())
				{
					m_whitelistDomainList.push_back((it));
					XGLOG_INFO("LightWeightWSSProfile::addIPListToWhiteList IP:(%s) Added to White List",(it).c_str());
				}
		}
		XGLOG_INFO("LightWeightWSSProfile::addIPListToWhiteList List Updated ");
		return true;
}

/*
* Add IP/Domain to white List 
*/
bool LightWeightWSSProfile::addIPToWhiteList(std::string ip_addr)
{
		std::lock_guard<std::mutex> lock(_mutex);
		auto it = std::find(m_whitelistDomainList.begin(), m_whitelistDomainList.end(), ip_addr);
		if (it == m_whitelistDomainList.end())
		{
				m_whitelistDomainList.push_back(ip_addr.c_str());
        XGLOG_INFO("LightWeightWSSProfile::addIPToWhiteList (%s) Updated ",ip_addr.c_str());
        return true;
		}
    XGLOG_INFO("LightWeightWSSProfile::addIPToWhiteList (%s) Already Whitelisted ",ip_addr.c_str());
		return false;
}

/*
* Remove IP/Domain from white List 
*/
bool LightWeightWSSProfile::removeIPFromWhiteList(std::string ip_addr)
{
  std::lock_guard<std::mutex> lock(_mutex);
	m_whitelistDomainList.erase(std::remove(m_whitelistDomainList.begin(), m_whitelistDomainList.end(), ip_addr), m_whitelistDomainList.end());
	XGLOG_INFO("LightWeightWSSProfile::removeIPFromWhiteList (%s) Removed from List",ip_addr.c_str());
  return true;
}

/*
* Remove IP/Domain list from white List 
*/
bool LightWeightWSSProfile::removeIPFromWhiteList(std::vector<std::string> ip_list)
{
  std::lock_guard<std::mutex> lock(_mutex);
  for (auto value : ip_list)
  {
    m_whitelistDomainList.erase(std::remove(m_whitelistDomainList.begin(), m_whitelistDomainList.end(), value), m_whitelistDomainList.end());
    XGLOG_INFO("LightWeightWSSProfile::removeIPFromWhiteList (%s) Removed from List", value.c_str());
  }
  return true;
}

/*
 * Insert Trunk info for whitelisting(Trunk Info)
 */
bool LightWeightWSSProfile::insertTrunkInfo(std::string ip_addr,int trunk_id)
{
  std::lock_guard<std::mutex> lock(_trunk_mutex);
  m_TrunkMap.insert(std::make_pair(ip_addr,trunk_id));
  XGLOG_INFO("insertTrunkInfo IpAddress(%s),Trunk Id(%d) Removed from List",ip_addr.c_str(),trunk_id);
  return true;
}

/*
 * remove Trunk Info from the whitelist map(Trunk Info)
 */
bool LightWeightWSSProfile::removeTrunkInfo(std::string ip_addr, int trunk_id)
{
  std::lock_guard<std::mutex> lock(_trunk_mutex);
  for (auto itr = m_TrunkMap.find(ip_addr); itr != m_TrunkMap.end(); itr++)
  {
    if (itr->second == trunk_id)
    {
      m_TrunkMap.erase(itr);
      XGLOG_INFO("removeTrunkInfo IpAddress(%s),Trunk Id(%d) Removed from List", ip_addr.c_str(), trunk_id);
      return true;
    }
  }
  XGLOG_ERROR("removeTrunkInfo IpAddress(%s),Trunk Id(%d) not found in list", ip_addr.c_str(), trunk_id);
  return false;
}

/*
* Add frafos SBC FQDN to teams domain list
*/
bool LightWeightWSSProfile::addSbcFqdnToTeamsDomainList(std::string sbc_fqdn)
{
  std::lock_guard<std::mutex> lock(_mutex);
  if (sbc_fqdn.empty())
  {
    XGLOG_ERROR("addSbcFqdnToTeamsDomainList failed received SBC FQDN is empty");
    return false;
  }
  auto it = std::find(m_msTeamsDomainList.begin(), m_msTeamsDomainList.end(), sbc_fqdn);
  if (it == m_msTeamsDomainList.end())
  {
    m_msTeamsDomainList.push_back(sbc_fqdn);
    XGLOG_INFO("addSbcFqdnToTeamsDomainList SBC FQDN (%s) updated", sbc_fqdn.c_str());
    return true;
  }
  XGLOG_INFO("addSbcFqdnToTeamsDomainList SBC FQDN (%s) is alreay present", sbc_fqdn.c_str());
  return true;
}

/*
 * Remove IP/Domain from MS teams domain list List
 */
bool LightWeightWSSProfile::removeSbcFqdnFromTeamsDomainList(std::string sbc_fqdn)
{
  std::lock_guard<std::mutex> lock(_mutex);
  m_msTeamsDomainList.erase(std::remove(m_msTeamsDomainList.begin(), m_msTeamsDomainList.end(), sbc_fqdn), m_msTeamsDomainList.end());
  XGLOG_INFO("LightWeightWSSProfile::removeSbcFqdnToTeamsDomainList (%s) Removed from MS Teams List", sbc_fqdn.c_str());
  return true;
}

/*LWSS Roun robin end*/

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
  }else if(!readLogProfile()) {
    XGLOG_ERROR("readConfig failed while reading log profile details !");
    return false;
  } else if(!readDBModuleProfile()) {
    XGLOG_ERROR("readConfig failed while reading DB profile details !");
    return false;
  } else if(!readURWssControllerProfile()) {
    XGLOG_ERROR("readConfig failed while reading urwssbridge controller profile details !");
    return false;
  }else if(!readLightWeightWSSProfile()) {
    XGLOG_ERROR("readConfig failed while reading sip module profile details !");
    return false;
  }else if (!readNetIOModuleProfile()) {
    XGLOG_ERROR("readConfig failed while reading netio profile details !");
    return false;
  }else if(!readMediaModuleProfile()) {
    XGLOG_ERROR("readConfig failed while reading media/monitor profile details !");
  }

  return true;
}

bool XGConfig::parseJsonConfigFile()
{
  char confBuffer[MAX_CONF_BUFFER_SIZE];
  FILE* fp = fopen(m_configPath.c_str(), "rb");
  if(fp == nullptr) {
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
  }else if(!m_configDoc.HasMember("lightweight_wss_profile")) {
    XGLOG_ERROR("parseJsonConfigFile failed. 'lightweight_wss_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  XGLOG_INFO("Successfully parsed json data from %s file", m_configPath.c_str());
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

bool XGConfig::readURWssControllerProfile()
{
  if(!m_configDoc.HasMember("urwss_controller_profile")) {
    XGLOG_ERROR("readURWssControllerProfile failed. 'urwss_controller_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &urwssControllerProfiles = m_configDoc["urwss_controller_profile"];
  if(urwssControllerProfiles.HasMember("user_reg_url")) {
    m_urWssBridgeCtrlProfile.m_db_UserRegURL = urwssControllerProfiles["user_reg_url"].GetString();
  } else {
    XGLOG_ERROR("readURWssControllerProfile failed. 'user_reg_url' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(urwssControllerProfiles.HasMember("conf_bridge_ip")) {
    m_urWssBridgeCtrlProfile.m_db_ConfBridgeIp = urwssControllerProfiles["conf_bridge_ip"].GetString();
  } else {
    XGLOG_ERROR("readURWssControllerProfile failed. 'conf_bridge_ip' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  XGLOG_INFO("Succesfully read wss controller profile details from '%s' file", m_configPath.c_str());

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


bool XGConfig::readLightWeightWSSProfile()
{
  XGLOG_FUNCTION_EN_EX("");
  if(!m_configDoc.HasMember("lightweight_wss_profile")) {
    XGLOG_ERROR("readUrcbProfile failed. 'lightweight_wss_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &wssprofile = m_configDoc["lightweight_wss_profile"];

  if(wssprofile.HasMember("sip_url")) {
    m_lightWeightWSSProfile.m_sipURL = wssprofile["sip_url"].GetString();
    XGLOG_INFO("Configuration sip_url(%s)", m_lightWeightWSSProfile.m_sipURL.c_str());
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'sip_url' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }
 
  if(wssprofile.HasMember("sips_url")) {
    m_lightWeightWSSProfile.m_sipsURL = wssprofile["sips_url"].GetString();
    XGLOG_INFO("Configuration sips_url(%s)", m_lightWeightWSSProfile.m_sipsURL.c_str());
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'sips_url' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(wssprofile.HasMember("wss_url")) {
    m_lightWeightWSSProfile.m_wssURL = wssprofile["wss_url"].GetString();
    XGLOG_INFO("Configuration wss_url(%s)", m_lightWeightWSSProfile.m_wssURL.c_str());
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'wss_url' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(wssprofile.HasMember("sip_port")) {
    m_lightWeightWSSProfile.m_sipPort = wssprofile["sip_port"].GetString();
    XGLOG_INFO("Configuration sip_port(%s)", m_lightWeightWSSProfile.m_sipPort.c_str());
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'sip_port' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(wssprofile.HasMember("sips_port")) {
    m_lightWeightWSSProfile.m_sipsPort = wssprofile["sips_port"].GetString();
    XGLOG_INFO("Configuration sips_port(%s)", m_lightWeightWSSProfile.m_sipsPort.c_str());
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'sips_port' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(wssprofile.HasMember("wss_port")) {
    m_lightWeightWSSProfile.m_wssPort = wssprofile["wss_port"].GetString();
    XGLOG_INFO("Configuration wss_port(%s)", m_lightWeightWSSProfile.m_wssPort.c_str());
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'wss_port' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(wssprofile.HasMember("sbc_support")) {
    m_lightWeightWSSProfile.m_isSbcEnabled = wssprofile["sbc_support"].GetBool();
		XGLOG_INFO("Configuration sbc_support(%d)", m_lightWeightWSSProfile.m_isSbcEnabled);;
  } else {
     XGLOG_ERROR("readLightWeightWSSProfile failed. 'sbc_support' detail is missing in '%s' file !", m_configPath.c_str());
     return false;
  }

  if(wssprofile.HasMember("certificate_path")) {
    m_lightWeightWSSProfile.m_certPath = wssprofile["certificate_path"].GetString();
    XGLOG_INFO("Configuration certificate_path(%s)", m_lightWeightWSSProfile.m_certPath.c_str());
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'certificate_path' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(wssprofile.HasMember("registration_expires_in_sec")) {
    m_lightWeightWSSProfile.registrationExpiresInSec = wssprofile["registration_expires_in_sec"].GetInt();
    XGLOG_INFO("Configuration registrationExpiresInSec(%d)", m_lightWeightWSSProfile.registrationExpiresInSec);
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'registration_expires_in_sec' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(wssprofile.HasMember("ippbx_ua")) {
    m_lightWeightWSSProfile.m_pbxUA = wssprofile["ippbx_ua"].GetString();
    XGLOG_INFO("Configuration ippbx_ua(%s)", m_lightWeightWSSProfile.m_pbxUA.c_str());
  } else {
      XGLOG_ERROR("readLightWeightWSSProfile failed. 'ippbx_ua' detail is missing in '%s' file !", m_configPath.c_str());
      return false;
  }

  if(wssprofile.HasMember("server_response_timeout")) {
    m_lightWeightWSSProfile.m_server_resp_timeout = wssprofile["server_response_timeout"].GetInt();
    XGLOG_INFO("Configuration server_response_timeout(%d)", m_lightWeightWSSProfile.m_server_resp_timeout);
  } else {
      XGLOG_ERROR("readLightWeightWSSProfile failed. 'server_response_timeout' detail is missing in '%s' file !", m_configPath.c_str());
      return false;
  }


  if(wssprofile.HasMember("default_ippbx_ip")) {
    m_lightWeightWSSProfile.m_default_pbx_ip = wssprofile["default_ippbx_ip"].GetString();
    XGLOG_INFO("Configuration m_default_pbx_ip(%s)", m_lightWeightWSSProfile.m_default_pbx_ip.c_str());
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'default_ippbx_ip' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }
     
  if(wssprofile.HasMember("contact_wss_ip")) {
    m_lightWeightWSSProfile.m_contactWssIp = wssprofile["contact_wss_ip"].GetString();
    XGLOG_INFO("Configuration contact_wss_ip(%s)", m_lightWeightWSSProfile.m_contactWssIp.c_str());
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'contact_wss_ip' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(wssprofile.HasMember("contact_wss_ipv6")) {
    m_lightWeightWSSProfile.m_contactWssIpv6 = wssprofile["contact_wss_ipv6"].GetString();
    XGLOG_INFO("Configuration contact_wss_ipv6(%s)", m_lightWeightWSSProfile.m_contactWssIpv6.c_str());
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'contact_wss_ipv6' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(wssprofile.HasMember("read_domain_whitelist_from_db")) {
    m_lightWeightWSSProfile.m_isReadWhiteListFromDb = wssprofile["read_domain_whitelist_from_db"].GetBool();
    XGLOG_INFO("Configuration read_domain_whitelist_from_db(%d)", m_lightWeightWSSProfile.m_isReadWhiteListFromDb);
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'read_domain_whitelist_from_db' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(wssprofile.HasMember("dc_type")) {
    m_lightWeightWSSProfile.m_dcType = wssprofile["dc_type"].GetString();
    XGLOG_INFO("Configuration dc_type(%s)", m_lightWeightWSSProfile.m_dcType.c_str());
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'dc_type' detail is missing in '%s' file !", m_configPath.c_str());
    return false;	
  }

  if(wssprofile.HasMember("sip_options_time_interval_in_sec")) {
    m_lightWeightWSSProfile.m_sipOptionsTimeout = wssprofile["sip_options_time_interval_in_sec"].GetInt();
    XGLOG_INFO("Configuration sip_options_time_interval_in_sec(%d)", m_lightWeightWSSProfile.m_sipOptionsTimeout);
  }else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'sip_options_time_interval_in_sec' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if (wssprofile.HasMember("domain_whitelist")) {
    const Value &wlist_domains = wssprofile["domain_whitelist"];
    for (Value::ConstValueIterator itr = wlist_domains.Begin(); itr != wlist_domains.End(); ++itr) {
      XGLOG_INFO("Domain under whitelist: %s", itr->GetString());
      m_lightWeightWSSProfile.m_whitelistDomainList.push_back(itr->GetString());
    }
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'domain_whitelist' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if (wssprofile.HasMember("inbound_route_addr_map")) {
    const Value &inbound_route_config = wssprofile["inbound_route_addr_map"];
    for (Value::ConstValueIterator itr = inbound_route_config.Begin(); itr != inbound_route_config.End(); ++itr) {
      for (Value::ConstMemberIterator iter = itr->MemberBegin(); iter != itr->MemberEnd(); ++iter) {
        XGLOG_INFO("Inbound_route_addr_map: host url (%s) and route IP(%s)", iter->name.GetString(), iter->value.GetString());
        // m_lightWeightWSSProfile.m_inboundRouteAddrMap[iter->name.GetString()] = iter->value.GetString();
        if (true != m_lightWeightWSSProfile.addInboundRoute(iter->name.GetString(), iter->value.GetString()))
          XGLOG_WARN("Failed to store the routing configuration");
      }
    }
  } else {
    XGLOG_ERROR("readLightWeightWSSProfile failed. 'inbound_route_addr_map' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  return true;
}

bool XGConfig::readNetIOModuleProfile()
{
  XGLOG_FUNCTION_EN_EX("");
  if (!m_configDoc.HasMember("netio_module_profile"))
  {
    XGLOG_ERROR("readNetIOModuleProfile failed. 'netio_module_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &NetIOProfile = m_configDoc["netio_module_profile"];
  if (NetIOProfile.HasMember("interagator_profile"))
  {
    const Value &interagator_profile = NetIOProfile["interagator_profile"];
    if (interagator_profile.IsArray())
    {
      if (interagator_profile.Size() > 0)
      {
        for (auto& val : interagator_profile.GetArray())
        {
          NetIOModuleProfile moduleProfile;
          if (val.HasMember("ip")) {
            moduleProfile.m_sIP = val["ip"].GetString();
            XGLOG_INFO("Configuration db_config_url(%s)", moduleProfile.m_sIP.c_str());
          } else {
            XGLOG_ERROR("readNetIOModuleProfile failed. 'interagator_profile' 'ip' detail is missing in '%s' file !", m_configPath.c_str());
            return false;
          }
          if(val.HasMember("port")) {
            moduleProfile.m_iPort = val["port"].GetInt();
          } else {
            XGLOG_ERROR("readNetIOModuleProfile failed. 'interagator_profile' 'port' detail is missing in '%s' file !", m_configPath.c_str());
            return false;
          }
          m_interrogatorProfileList.emplace_back(moduleProfile);
        }
      }
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

bool XGConfig::readDBModuleProfile()
{
  if(!m_configDoc.HasMember("db_module_profile")) {
    XGLOG_ERROR("readDBModuleProfile failed. 'db_module_profile' details is missing in %s file !", m_configPath.c_str());
    return false;
  }

  const Value &dbModuleProf = m_configDoc["db_module_profile"];
  if(dbModuleProf.HasMember("server_host")) {
    m_dbModuleProfile.m_server_ip = dbModuleProf["server_host"].GetString();
  } else {
    XGLOG_ERROR("readDBModuleProfile failed. 'server_host' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(dbModuleProf.HasMember("server_port")) {
    m_dbModuleProfile.m_server_port = dbModuleProf["server_port"].GetInt();
  } else {
    XGLOG_ERROR("readDBModuleProfile failed. 'server_port' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(dbModuleProf.HasMember("username")) {
    m_dbModuleProfile.m_username = dbModuleProf["username"].GetString();
  } else {
    XGLOG_ERROR("readDBModuleProfile failed. 'username' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(dbModuleProf.HasMember("password")) {
    m_dbModuleProfile.m_password = dbModuleProf["password"].GetString();
  } else {
    XGLOG_ERROR("readDBModuleProfile failed. 'password' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }

  if(dbModuleProf.HasMember("database")) {
    m_dbModuleProfile.m_database = dbModuleProf["database"].GetString();
  } else {
    XGLOG_ERROR("readDBModuleProfile failed. 'database' detail is missing in '%s' file !", m_configPath.c_str());
    return false;
  }
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

/*LWSS Changes Start*/
LightWeightWSSProfile& XGConfig::getLightWeightWSSProfile()
{
   return m_lightWeightWSSProfile;
}

bool XGConfig::updateLwssProfileFromDBConfigMgr(LightWeightWSSProfile *ptrLwssProfile)
{
    XGLOG_FUNCTION_EN_EX("");
    if (!ptrLwssProfile) {
        XGLOG_ERROR("XGConfig::updateLwssProfileFromDBConfigMgr Received Invalid input !");
        return false;
    }

    if (ptrLwssProfile->m_whitelistDomainList.empty()){
        XGLOG_ERROR("XGConfig::updateLwssProfileFromDBConfigMgr domain whitelist is empty");
        return false; 
    }
    
    if (ptrLwssProfile->m_inboundRouteAddrMap.empty()){
        XGLOG_ERROR("SofiaSiphandler::validateDomainWithWildcardRouteAddr inboundRouteAddrMap is empty");
        return false; 
    } 

    ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_dbConfigDatalock, EN_XGATE_STATUS_ERROR);
    m_lightWeightWSSProfile.m_whitelistDomainList = ptrLwssProfile->m_whitelistDomainList;
    m_lightWeightWSSProfile.m_inboundRouteAddrMap = ptrLwssProfile->m_inboundRouteAddrMap;

    return true;    
}


/*LWSS Changes End*/


void XGConfig::setCallBack(IURModuleCallBack* pCallBack)
{
moduleCallBack = pCallBack;
}

IURModuleCallBack* XGConfig::getCallBack()
{
   return moduleCallBack;
}
