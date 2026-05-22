#ifndef __URCONFROOM_H__
#define __URCONFROOM_H__

//#include "URCBCallContext.h"
#include "URCBCtrlDefines.h"
#include "xGateUtil.h"
#include <map>
#include <deque>

typedef enum
{
  CONF_USER_TYPE_NULL=0,
  CONF_USER_TYPE_HOST=1,
  CONF_USER_TYPE_PARTICIPANT=2,
  CONF_USER_TYPE_GUEST=3,
  CONF_USER_TYPE_SCREENSHARE=4,
  CONF_USER_TYPE_CUSTOMER=5,
  CONF_USER_TYPE_AGENT=6,
} ConfUserType;

typedef enum ConfUserDeviceType
{
  EN_DEVICE_TYPE_UNKNOWN=0,
  EN_DEVICE_TYPE_DESKPHONE=1,
  EN_DEVICE_TYPE_IOS=2,
  EN_DEVICE_TYPE_ANDROID=3,
  EN_DEVICE_TYPE_WEB=4,
  EN_DEVICE_TYPE_WINDOWS=5,
  EN_DEVICE_TYPE_PSTN=6,
  EN_DEVICE_TYPE_WEB_VIDEOMEET=7,
  EN_DEVICE_TYPE_IOS_VIDEOMEET=8,
  EN_DEVICE_TYPE_ANDROID_VIDEOMEET=9,
  EN_DEVICE_TYPE_MAX
}URCONF_USER_DEVICE_TYPE;

class AddressOfRecord
{
  public:
    AddressOfRecord(){
       m_contactAddr = "";
       m_ipAddr = "";
       m_proxyUserName = "";
       m_sDeviceType = "";
       m_sPresenceStatus = "";
       m_status = 0;
       m_iPresenceStatus = 0;
    };
    AddressOfRecord(std::string &contactAddr, std::string &ipAddr, std::string &proxyUserName, std::string &sDeviceType, int &m_status, std::string &m_sPresenceStatus, int &m_iPresenceStatus);
    ~AddressOfRecord(){};
    std::string m_contactAddr, m_ipAddr, m_proxyUserName, m_sDeviceType, m_sPresenceStatus;
    int m_status;
    int m_iPresenceStatus;
    UserDevice m_iDeviceType;

    UserDevice getDeviceType()
    {
			if(m_sDeviceType == "URDESK:") {
				m_iDeviceType = EN_USER_DEVICE_DESKPHONE;
			} else if (m_sDeviceType == "URAPP:IOS") {
				m_iDeviceType = EN_USER_DEVICE_IOS;
			} else if (m_sDeviceType == "URAPP:AND") {
				m_iDeviceType = EN_USER_DEVICE_ANDROID;
			} else if (m_sDeviceType == "URWEB:WEB") {
				m_iDeviceType = EN_USER_DEVICE_WEB;
			} else if (m_sDeviceType == "PSTN") {
				m_iDeviceType = EN_USER_DEVICE_PSTN;
      } else if (m_sDeviceType == "URWEB:VIDEO") {
			  m_iDeviceType = EN_USER_DEVICE_WEB_VIDEOMEET;						
			} else if (m_sDeviceType == "URIOS:VIDEO") {
				m_iDeviceType = EN_USER_DEVICE_IOS_VIDEOMEET;
			} else if (m_sDeviceType == "URAND:VIDEO") {
				m_iDeviceType = EN_USER_DEVICE_ANDROID_VIDEOMEET;
			} else {
				m_iDeviceType = EN_USER_DEVICE_DESKPHONE;
			}
      return m_iDeviceType;
    }
};

class URAVParticipantCall
{
  public:
    URAVParticipantCall(){
	m_strConfRoomID = "";
	m_strConfCallID = "";
	m_strUpdateOfferCallId = "";
	m_strDialoutCallId = "";
	m_strConfUserExtn = "";
	m_strSDPInfo = "";
    };    

    ~URAVParticipantCall() {};
    std::string m_strConfRoomID;
    std::string m_strConfCallID;
    std::string m_strUpdateOfferCallId;   
    std::string m_strDialoutCallId; 
    std::string m_strConfUserExtn;
    std::string m_strSDPInfo;	
};

class URConfCallUserInfo
{
  public:		
    URConfCallUserInfo() 
    {
      m_strUid = "";
      m_strConfRoomID = "";
      m_strConfCallID = "";
      m_strConfUserExtn = "";
      m_strConfUserName = "";
      m_strConfUserAgent = "";
      m_enumConfUserType = CONF_USER_TYPE_NULL;
      m_enumConfUserLogInDeviceType = EN_USER_DEVICE_UNKNOWN;
    };

    ~URConfCallUserInfo() {};
	
    std::string m_strUid; 
    std::string m_strConfRoomID;
    std::string m_strConfCallID;
    std::string m_strConfUserExtn; 
    std::string m_strConfUserName;
    std::string m_strDomainName;
    std::string m_strConfUserAgent;  
    ConfUserType m_enumConfUserType;
    UserDevice m_enumConfUserLogInDeviceType;			
}; 

typedef std::map <std::string, URCBSIOUserInfo*> SIO_USER_INFO_MAP;
typedef SIO_USER_INFO_MAP::iterator ITR_SIO_USER_INFO_MAP;
typedef std::map <std::string, URConfCallUserInfo*> CONF_CALL_LIST_MAP;
typedef CONF_CALL_LIST_MAP::iterator ITR_CONF_CALL_LIST_MAP;
typedef std::map <std::string, AddressOfRecord> CONF_USER_AOR_MAP;
typedef CONF_USER_AOR_MAP::iterator ITR_CONF_USER_AOR_MAP;

class URConfRoom
{
  public:
    URConfRoom(){
    };

    ~URConfRoom() {};

    std::string m_strConfRoomId;
    std::string m_strAcdChannel;
		std::string m_strPrimaryAgentId;
    URActiveSpeakerInfo m_URActiveSpeakerInfo;
    CONF_CALL_LIST_MAP m_URConfCallList;  //key is callId
    SIO_USER_INFO_MAP m_URCBSIOUserInfoList;   //Key is Conn Id
    CONF_PARTICIPANT_INFO_DEQUE m_dequeURConfParticipant;
};

#endif
