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
#define MAX_ROUTE_IPS_PER_BRIDGE 100

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

typedef struct
{
  std::string m_sIP;
  unsigned short m_iPort;
}NetIOModuleProfile;

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

class URWssControllerProfile {
  public:
    URWssControllerProfile();
    ~URWssControllerProfile();
    URWssControllerProfile(const URWssControllerProfile& src);
    URWssControllerProfile& operator=(const URWssControllerProfile& src);

    //member variables
	std::string m_db_UserRegURL;
	std::string m_db_ConfBridgeIp;
};


/*LWSS Round robin start*/
class BridgeRouteIpList
{
  public:
    BridgeRouteIpList(){
        ip_index = 0;
        to_use_ip_index = 0;
    };
    bool addIpAddr(string routeIp);
    string getBridgeIpToRoute();

    string m_addrList[MAX_ROUTE_IPS_PER_BRIDGE];
    unsigned int ip_index;
    unsigned int to_use_ip_index;
};

/*LWSS Round robin end*/

/*LWSS Changes Start*/

class DBProfile {
  public:
    DBProfile() = default;
    ~DBProfile() = default;

      std::string m_server_ip;
      std::string m_username;
      std::string m_password;
      std::string m_database;
      unsigned short m_server_port;
};

class LightWeightWSSProfile {
  public:
    LightWeightWSSProfile();
    ~LightWeightWSSProfile();
    LightWeightWSSProfile(const LightWeightWSSProfile& src);
    LightWeightWSSProfile& operator=(const LightWeightWSSProfile& src);

	bool addInboundRoute (std::string host_url, std::string bridge_ip);
  std::string getInboundRouteIp(std::string host_url);
  bool addIPListToWhiteList(std::vector<std::string> ip_list);
  bool addIPToWhiteList(std::string ip_addr);
  bool removeIPFromWhiteList(std::vector<std::string> ip_list);
  bool removeIPFromWhiteList(std::string ip_addr);
  bool insertTrunkInfo(std::string ip_addr,int trunk_id);
  bool removeTrunkInfo(std::string ip_addr,int trunk_id);
  bool addSbcFqdnToTeamsDomainList(std:: string sbc_fqdn);
  bool removeSbcFqdnFromTeamsDomainList(std:: string sbc_fqdn);

  //member variables
	std::string m_db_UserRegURL;
	std::string m_db_UserRegVmeetURL;
	unsigned int registrationExpiresInSec;
  int m_server_resp_timeout;
  std::string m_default_pbx_ip;
  std::string m_contactWssIp;
  std::string m_contactWssIpv6;
	unsigned int m_dbQueryIntervalInSec;
  std::string m_pbxUA;
  std::string m_sipURL;
  std::string m_sipsURL;
  std::string m_wssURL;
  std::string m_sipPort;
  std::string m_sipsPort;
  std::string m_wssPort;
  std::string m_certPath;
  std::string m_dcType;
  unsigned int m_sipOptionsTimeout;
  bool m_isSbcEnabled;
  bool m_isReadWhiteListFromDb;
  std::vector<std::string> m_whitelistDomainList;
  std::vector<std::string> m_msTeamsDomainList;
 	std::map<std::string, BridgeRouteIpList *> m_inboundRouteAddrMap;
  std::multimap<std::string,int> m_TrunkMap;

  private:
    std::mutex _mutex;
    std::mutex _trunk_mutex;

};
/*LWSS Changes End*/

class XGConfig:public IURModuleConfig 
	{
  public:
    bool init(...);
    bool deInit();

    URConfControllerProfile& getURConfControllerProfile();
    void setCallBack(IURModuleCallBack* ptCallBack);
    virtual IURModuleCallBack* 	getCallBack();
    SipModuleProfile& getSipModConfig();

	/*LWSS Changes Start*/
	LightWeightWSSProfile& getLightWeightWSSProfile();
	bool updateLwssProfileFromDBConfigMgr(LightWeightWSSProfile *ptrLwssProfile);
	/*LWSS Changes End*/
	
    string m_localIp4Addr;

    //member functions
    Document m_configDoc;
    string m_configPath;
	
    string m_localIp6Addr;
    LogProfile	m_logProfile;
    DBProfile m_dbModuleProfile;
    URWssControllerProfile m_urWssBridgeCtrlProfile;
    URConfControllerProfile m_urConfControllerProfile;
    IvrControllerProfile m_ivrControllerProfile;
    MediaModuleProfile m_mediaModuleProfile;
    SipModuleProfile m_sipModuleProfile;
    std::vector<TcpLisentAddr> 	m_listenPortList;
    std::vector<NetIOModuleProfile>   m_interrogatorProfileList;
   // void setCallBack(IURModuleCallBack* ptCallBack);
    LightWeightWSSProfile m_lightWeightWSSProfile;

  private: 
    bool checkConfigPath();
    bool readConfig();
    bool parseJsonConfigFile();
    bool readLogProfile();
    bool readDBModuleProfile();
    bool readURWssControllerProfile();
    bool readSipModuleProfile();
	  bool readLightWeightWSSProfile();
    bool readNetIOModuleProfile();
    bool readMediaModuleProfile();

		
	ACE_Thread_Mutex m_dbConfigDatalock;
};

typedef ACE_Unmanaged_Singleton<XGConfig, ACE_Null_Mutex> URENGINE_CONFIG;
#endif
