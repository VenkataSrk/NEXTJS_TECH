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

class SttConnInfo
{
  public:
    SttConnInfo();
    ~SttConnInfo();
    SttConnInfo(const SttConnInfo &src);
    SttConnInfo& operator=(const SttConnInfo &src);

    //member variables
    std::string m_strServerHost;
    std::string m_strServerPort;
    int m_strConnectType;
};

class SttProfile
{
  public:
    SttProfile();
    ~SttProfile();
    SttProfile(const SttProfile &src);
    SttProfile& operator=(const SttProfile &src);

    //member variables
    SttConnInfo m_smaticsConnInfo;
    SttConnInfo m_inhouseConnInfo;
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
    string m_localPublicAddr;
    string m_localIp6Addr;
    LogProfile m_logProfile;
    StunConfig m_stunConfig;
    MediaModuleProfile m_mediaModuleProfile;
    MediaModuleProfile m_recordingProfile;
    SttProfile m_sttProfile;
    bool readStunConfig(IP_TYPE ip_type);
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
    bool readSttProfile();
    bool readRecordingProfile();
};

typedef ACE_Unmanaged_Singleton<XGConfig, ACE_Null_Mutex> XG_CONFIG;
#endif
