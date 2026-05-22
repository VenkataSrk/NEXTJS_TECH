#ifndef _MRCP_CONFIG_H
#define _MRCP_CONFIG_H

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

#define MRCP_ENV_BASE "MRCP_BASE" 
#define MRCP_CONFIG_FILE_NAME "/mrcp.conf"

#define MRCPCONFIGSERVICE MRCP_CONFIG::instance()

enum class EN_ENGINE_CONNECT_TYPE
{
  EN_ENGINE_CONNECT_TYPE_NULL = 0,
  EN_ENGINE_CONNECT_TYPE_IP,
  EN_ENGINE_CONNECT_TYPE_URL,
};

class SttTtsConnInfo
{
  public:
    SttTtsConnInfo()=default;
    ~SttTtsConnInfo()=default;
    SttTtsConnInfo(const SttTtsConnInfo &src)=default;
    SttTtsConnInfo& operator=(const SttTtsConnInfo &src)=default;

    //member variables
    std::string m_strServerHostIp;
    std::string m_strServerHostUrl;
    std::string m_strServerPort;
    EN_ENGINE_CONNECT_TYPE m_eConnectType;
};

class SttProfile
{
  public:
    SttProfile()=default;
    ~SttProfile()=default;
    SttProfile(const SttProfile &src)=default;
    SttProfile& operator=(const SttProfile &src)=default;

    //member variables
    string m_mrcp_xml_path;
    SttTtsConnInfo m_smaticsConnInfo;
    SttTtsConnInfo m_inhouseConnInfo;
    unsigned short m_iSendCcToWorkerNode;
};

class TtsProfile
{
  public:
    TtsProfile()=default;
    ~TtsProfile()=default;
    TtsProfile(const TtsProfile &src)=default;
    TtsProfile& operator=(const TtsProfile &src)=default;

    //member variables
    string m_mrcp_xml_path;
    SttTtsConnInfo m_smaticsConnInfo;
    SttTtsConnInfo m_inhouseConnInfo;
};

class TimerProfile
{
  public:
  TimerProfile() = default;
  ~TimerProfile() = default;
  TimerProfile(const TimerProfile &src) = default;
  TimerProfile& operator = (const TimerProfile &src) = default;

  //member variables
  unsigned int m_iDtmfTimeOut;
  unsigned int m_iBotResponseTimeOut;
  unsigned int m_iCustomerResponseTimeOut;
};

enum class EN_REDIS_TYPE
{
  EN_REDIS_TYPE_UNKNOWN = 0,
  EN_REDIS_TYPE_BOT = 1,
  EN_REDIS_TYPE_CCAAS_ACD = 2,
  EN_REDIS_TYPE_CCAAS_BOT = 3,
  EN_REDIS_TYPE_CCAAS_VMS_STT = 4,
  EN_REDIS_TYPE_CCAAS_NODE = 5,
  EN_REDIS_TYPE_CCAAS_CONV = 6,
  EN_REDIS_TYPE_CCAAS_VMS_TRANSCRIPT = 7,
  EN_REDIS_TYPE_UCAAS = 8,
  EN_REDIS_TYPE_PBX = 9,
};

enum class EN_REDIS_MSG_TYPE
{
  EN_REDIS_MSG_TYPE_UNKNOWN = 0,
  EN_REDIS_MSG_TYPE_PUBSUB = 1,
  EN_REDIS_MSG_TYPE_PUSHPOP = 2,
  EN_REDIS_MSG_TYPE_STREAM = 3,
};

class RedisConnInfo
{
  public:
    RedisConnInfo()
    {
      m_enPubType = EN_REDIS_TYPE::EN_REDIS_TYPE_UNKNOWN;
      m_strServerHost = "";
      m_iServerPort = 0;
      m_strPassword = "";
      m_iSocketTimeout = 0;
      m_vChannel.clear();
      m_strChannel = "";
      m_iCluster = 0;
      m_enMsgType = EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_UNKNOWN;
    }
    ~RedisConnInfo()
    {
      m_enPubType = EN_REDIS_TYPE::EN_REDIS_TYPE_UNKNOWN;
      m_strServerHost = "";
      m_iServerPort = 0;
      m_strPassword = "";
      m_iSocketTimeout = 0;
      m_vChannel.clear();
      m_strChannel = "";
      m_iCluster = 0;
      m_enMsgType = EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_UNKNOWN;
    }
    RedisConnInfo(const RedisConnInfo& src)
    {
      m_enPubType = src.m_enPubType;
      m_strServerHost.assign(src.m_strServerHost.c_str());
      m_iServerPort = src.m_iServerPort;
      m_strPassword.assign(src.m_strPassword.c_str());
      m_iSocketTimeout = src.m_iSocketTimeout;
      m_vChannel.assign(src.m_vChannel.begin(), src.m_vChannel.end());
      m_strChannel.assign(src.m_strChannel.c_str());
      m_iCluster = src.m_iCluster;
      m_enMsgType = src.m_enMsgType;
    }
    RedisConnInfo& operator=(const RedisConnInfo& src)
    {
      m_enPubType = src.m_enPubType;
      m_strServerHost.assign(src.m_strServerHost.c_str());
      m_iServerPort = src.m_iServerPort;
      m_strPassword.assign(src.m_strPassword.c_str());
      m_iSocketTimeout = src.m_iSocketTimeout;
      m_vChannel.assign(src.m_vChannel.begin(), src.m_vChannel.end());
      m_strChannel.assign(src.m_strChannel.c_str());
      m_iCluster = src.m_iCluster;
      m_enMsgType = src.m_enMsgType;
      return *this;
    }

    EN_REDIS_TYPE m_enPubType;
    std::string m_strServerHost;
    unsigned short m_iServerPort;
    std::string m_strPassword;
    int m_iSocketTimeout;
    std::vector<std::string> m_vChannel;
    std::string m_strChannel;
    int m_iCluster;
    EN_REDIS_MSG_TYPE m_enMsgType;
};

class RedisProfile
{
  public:
    RedisProfile()
    {}
    ~RedisProfile()
    {
      for(auto &m : m_mapSubRedisConnInfo){ delete m.second; m.second=nullptr;}
      m_mapSubRedisConnInfo.clear();
      for(auto &m : m_mapPubRedisConnInfo){ delete m.second; m.second=nullptr;}
      m_mapPubRedisConnInfo.clear();
    }
    RedisProfile(const RedisProfile& src)
    {
      m_mapSubRedisConnInfo.insert(src.m_mapSubRedisConnInfo.begin(), src.m_mapSubRedisConnInfo.end());
      m_mapPubRedisConnInfo.insert(src.m_mapPubRedisConnInfo.begin(), src.m_mapPubRedisConnInfo.end());
    }
    RedisProfile& operator=(const RedisProfile& src)
    {
      m_mapSubRedisConnInfo.insert(src.m_mapSubRedisConnInfo.begin(), src.m_mapSubRedisConnInfo.end());
      m_mapPubRedisConnInfo.insert(src.m_mapPubRedisConnInfo.begin(), src.m_mapPubRedisConnInfo.end());
      return *this;
    }

    std::map<EN_REDIS_TYPE, RedisConnInfo*> m_mapPubRedisConnInfo;
    std::map<EN_REDIS_TYPE, RedisConnInfo*> m_mapSubRedisConnInfo;
};


class MrcpConfig:public IURModuleConfig 
{
    public:
        bool init(...);
        bool deInit();
        void 						setCallBack(IURModuleCallBack* ptCallBack);
        virtual IURModuleCallBack* 	getCallBack();
        string m_no_resp_from_avamo_ivr;
        string m_no_query_from_user_ivr;
        string m_strIpV4Addr;
        string m_strPublicAddr;
        RedisProfile m_redisProfile;
        SttProfile m_sttProfile;
        TtsProfile m_ttsProfile;
        TimerProfile m_timerProfile;

        Document m_configDoc;
        string m_configPath;
    private: 
        bool checkConfigPath();
        bool readConfig();
        bool parseJsonConfigFile();
        bool readCommonDetails();
        bool readRedisProfile();
        bool readSttProfile();
        bool readTtsProfile();
        bool readTimerProfile();
};

typedef ACE_Unmanaged_Singleton<MrcpConfig, ACE_Null_Mutex> MRCP_CONFIG;
#endif
