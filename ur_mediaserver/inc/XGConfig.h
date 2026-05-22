#ifndef _XG_CONFIG_H
#define _XG_CONFIG_H

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

#define ENV_BASE "MBC_BASE" 
#define CONFIG_FILE_NAME "/mbc.conf"

#define CONFIGSERVICE XG_CONFIG::instance()

typedef struct
{
  std::vector<TcpLisentAddr> 	m_peerConnectAddrs;
} MediaModuleProfile;

typedef enum _IP_TYPE {
  IP_TYPE_4,
  IP_TYPE_6
} IP_TYPE;

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

enum class EN_DB_MSG_TYPE
{
  EN_DB_MSG_TYPE_UNKNOWN = 0,
  EN_DB_MSG_TYPE_UCAAS_CALL_RECORD = 1,
  EN_DB_MSG_TYPE_CCAAS_CALL_RECORD = 2,
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

class DBConnInfo
{
	public:
		DBConnInfo();
		~DBConnInfo();
		DBConnInfo(const DBConnInfo &src);
		DBConnInfo& operator=(const DBConnInfo &src);

		//member variables
    EN_DB_MSG_TYPE m_enMsgType;
		string m_db_server_ip;
		string m_db_user_name;
		string m_db_password;
		string m_db_name;
    string m_sp_name;
		string m_db_serv_port;
};

class DBModuleProfile
{
  public:
    DBModuleProfile();
    ~DBModuleProfile();
    DBModuleProfile(const DBModuleProfile& src);
    DBModuleProfile& operator=(const DBModuleProfile& src);

    //member variables
    DBConnInfo m_dbConnInfo;
    std::map<EN_DB_MSG_TYPE, DBConnInfo*> m_mapDbConnInfo;
};

class XGConfig : public IURModuleConfig 
{
  public:
    XGConfig();
    ~XGConfig();

    bool init(...);
    void setCallBack(IURModuleCallBack* ptCallBack);
    virtual IURModuleCallBack* getCallBack();

    Document m_configDoc;
    string m_configPath;
    string m_localIp4Addr;
    string m_localIp6Addr;
    LogProfile m_logProfile;
    StunConfig m_stunConfig;
    MediaModuleProfile m_mediaModuleProfile;
    bool readStunConfig(IP_TYPE ip_type);
		DBModuleProfile m_dbModProfile;
    RedisModuleProfile m_redisModProfile;
    int m_sfu_id;
  //std::vector<TcpLisentAddr> 	m_listenPortList;

  private: 
    bool deInit();
    bool checkConfigPath();
    bool readConfig();
    bool parseJsonConfigFile();
    bool readCommonDetails();
    bool readLogProfile();
    bool readMediaModuleProfile();
		bool readDBModuleProfile();
    bool readRedisModuleProfile();
    bool fillRedisConnInfo(const Value &val, RedisConnInfo *& redisConnInfo);
};

typedef ACE_Unmanaged_Singleton<XGConfig, ACE_Null_Mutex> XG_CONFIG;
#endif
