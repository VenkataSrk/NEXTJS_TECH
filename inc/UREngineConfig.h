#ifndef _UR_ENGINE_CONFIG_H
#define _UR_ENGINE_CONFIG_H

//system includes
#include <iostream>
#include <map>
#include <vector>

//ace includes
#include <ace/Singleton.h>

//rapidjosn includes
#include "document.h"

//local includes
#include "xglog.h"
#include "xGateUtil.h"
#include "IURModuleConfig.h"

using namespace rapidjson;
using namespace std;

#define ENV_BASE "URENGINE_BASE" 
#define CONFIG_FILE_NAME "/urengine.conf"

#define CONFIGSERVICE URENGINE_CONFIG::instance()

typedef struct
{
  unsigned int mediaTcpListenPort;
  unsigned int mbcLinkTcpListenPort;
}MediaModuleProfile;

typedef struct
{
  std::string registerReqUri;
  std::string registerToUri;
  std::string registerUsername;
  std::string registerPassword;
}SipModuleProfile;

enum class EN_REDIS_MSG_TYPE
{
  EN_REDIS_MSG_TYPE_UNKNOWN = 0,
  EN_REDIS_MSG_TYPE_UCAAS_SIO = 1,
  EN_REDIS_MSG_TYPE_CONFBRIDGE = 2,
  EN_REDIS_MSG_TYPE_CCAAS_SIO = 3,
  EN_REDIS_MSG_TYPE_ACD = 4,
};

enum class EN_REDIS_GET_TYPE
{
  EN_REDIS_GET_TYPE_UNKNOWN = 0,
  EN_REDIS_GET_TYPE_ACD_CHANNEL = 1,
};

enum class EN_REDIS_METHOD
{
  EN_REDIS_METHOD_UNKNOWN = 0,
  EN_REDIS_METHOD_PUB_SUB = 1,
  EN_REDIS_METHOD_PUSH_POP = 2,
  EN_REDIS_METHOD_STREAM = 3,
};

class RedisConnInfo
{
  public:
    RedisConnInfo();
    ~RedisConnInfo();
    RedisConnInfo(const RedisConnInfo& src);
    RedisConnInfo& operator=(const RedisConnInfo& src);
  
    //member variables
    std::string m_strServerHost;
    unsigned int m_iServerPort;
    std::string m_strPassword;
    unsigned int m_iSocketTimeout;
    std::string m_strChannel;
    std::string m_strKey;
    EN_REDIS_METHOD m_enRedisMethod;
    EN_REDIS_MSG_TYPE m_enMsgType;
    EN_REDIS_GET_TYPE m_enGetType;
    std::vector<std::string> m_vChannel;
};

class RedisModuleProfile
{
  public:
    RedisModuleProfile();
    ~RedisModuleProfile();
    RedisModuleProfile(const RedisModuleProfile& src);
    RedisModuleProfile& operator=(const RedisModuleProfile& src);

    //member variables
    RedisConnInfo m_subRedisConnInfo;
    std::map<EN_REDIS_MSG_TYPE, RedisConnInfo*> m_mapSubRedisConnInfo;
    std::map<EN_REDIS_MSG_TYPE, RedisConnInfo*> m_mapPubRedisConnInfo;
    std::map<EN_REDIS_GET_TYPE, RedisConnInfo*> m_mapGetRedisConnInfo;
};

class IvrControllerProfile {
  public:
    IvrControllerProfile();
    ~IvrControllerProfile();
    IvrControllerProfile(const IvrControllerProfile& src);
    IvrControllerProfile& operator=(const IvrControllerProfile& src);

    //member variables
    std::string m_directAccessNumber;
    std::string m_extnAccessNumber;
    unsigned int m_responseRepeatCount;
    unsigned int m_maxCallCapacity;
    unsigned int m_maxRecordDuration;
    std::string m_ivrMultiLevelXml;
    std::string m_db_serverInfoURL;
    std::string m_db_multiLevelIvrURL;
};

class URConfControllerProfile {
  public:
    URConfControllerProfile();
    ~URConfControllerProfile();
    URConfControllerProfile(const URConfControllerProfile& src);
    URConfControllerProfile& operator=(const URConfControllerProfile& src);

    //member variables
	std::string m_db_ValidateExtnURL;
	std::string m_db_ValidatePinURL;
	std::string m_db_UserAorDetailURL;
	std::string m_db_UserAuthURL;
	std::string m_db_UserRegURL;
};

class XGConfig:public IURModuleConfig 
	{
  public:
    bool init(...);
    bool deInit();

    URConfControllerProfile& getURConfControllerProfile();
    void setCallBack(IURModuleCallBack* ptCallBack);
    virtual IURModuleCallBack* 	getCallBack();
    SipModuleProfile& getSipModConfig();
    string m_localIp4Addr;

    //member functions
    Document m_configDoc;
    string m_configPath;
	
    string m_localIp6Addr;
    LogProfile	m_logProfile;
    URConfControllerProfile m_urConfControllerProfile;
    IvrControllerProfile m_ivrControllerProfile;
    MediaModuleProfile m_mediaModuleProfile;
    RedisModuleProfile m_redisModProfile;
    SipModuleProfile m_sipModuleProfile;
    std::vector<TcpLisentAddr> 	m_listenPortList;
   // void setCallBack(IURModuleCallBack* ptCallBack);
  private: 
    bool checkConfigPath();
    bool readConfig();
    bool parseJsonConfigFile();
    bool readCommonDetails();
    bool readLogProfile();
    bool readURConfControllerProfile();
    bool readIvrControllerProfile();
    bool readMediaModuleProfile();
    bool readSipModuleProfile();
    bool readRedisModuleProfile();
    bool fillRedisConnInfo(const Value &val, RedisConnInfo *& redisConnInfo);
};

typedef ACE_Unmanaged_Singleton<XGConfig, ACE_Null_Mutex> URENGINE_CONFIG;
#endif
