#ifndef _URCB_CTRL_DEFINES_H
#define _URCB_CTRL_DEFINES_H

#define URCONF_USER_JOIN_WAIT_TIMER_VAL 60
#define URCONF_PORT_ALLOC_RES_TIMER_VAL 20
#define URCONF_USER_DISCONNECT_NOTIFY_TIMER_VAL 10
#define URCONF_ACTIVE_SPEAKER_INFO_TIMER_VAL 20
#define URCONF_ACD_RES_TIMER_VAL 5

#include <map>
#include <queue>
#include "xGateUtil.h"

using namespace std;

/*
 * SocketIO-ConfBridge Defines Starts
*/

typedef enum URConfSIOMsgType
{
  URCONF_SIO_MSG_UNKNOWN = 0,
  URCONF_SIO_MSG_HOST_JOINING = 1,
  URCONF_SIO_MSG_HOST_JOINED = 2,
  URCONF_SIO_MSG_PARTICIPANT_JOINING = 3,
  URCONF_SIO_MSG_PARTICIPANT_JOINED = 4,
  URCONF_SIO_MSG_UPDATED_PARTICIPANT_LIST = 5,
  URCONF_SIO_MSG_USER_ACTION_DISCONNECTED = 6,
  URCONF_SIO_MSG_USER_ACTION_TERMINATED = 7,
  URCONF_SIO_MSG_PARTICIPANT_REMOVE = 8,
  URCONF_SIO_MSG_END_MEETING_ALL = 9,
  URCONF_SIO_MSG_SCREENSHARE_START = 10,
  URCONF_SIO_MSG_SCREENSHARE_STOP = 11,
  URCONF_SIO_MSG_SDP_UPDATE_REQUEST = 12,
  URCONF_SIO_MSG_SDP_UPDATE_RESPONSE = 13,
  URCONF_SIO_MSG_TILE_CHANGE_REQUEST = 14,
  URCONF_SIO_MSG_TILE_CHANGE_RESPONSE = 15,
  URCONF_SIO_MSG_SCREENSHARE_VIDEO_REQUEST = 16,
  URCONF_SIO_MSG_ACTIVE_SPEAKER_VIDEO_REQUEST = 17,
  URCONF_SIO_MSG_RECORDING_START = 18,
  URCONF_SIO_MSG_RECORDING_PAUSE = 19,
  URCONF_SIO_MSG_RECORDING_RESUME = 20,
  URCONF_SIO_MSG_RECORDING_STOP = 21,
  URCONF_SIO_MSG_ACTIVE_SPEAKER_INFO = 22,
  URCONF_SIO_MSG_CUSTOMER_JOINING = 23,
  URCONF_SIO_MSG_CUSTOMER_JOINED = 24,
  URCONF_SIO_MSG_AGENT_JOINING = 25,
  URCONF_SIO_MSG_AGENT_JOINED = 26,
	URCONF_SIO_MSG_AGENT_TRANSFER_INIT = 27,
}URConfSIOMsgType;

typedef enum URConfCallType
{
  URCONF_CALL_TYPE_UNKNOWN = 0,
  URCONF_CALL_TYPE_TRANSFER = 1,
}URConfCallType;

struct URCBTimerData
{
   URCBTimerData()
   {
      m_strConfMeetingId="";
      m_strConfDevId="";
      m_strConfConnId="";
   }
   ~URCBTimerData()	 
   {
      m_strConfMeetingId="";
      m_strConfDevId="";
      m_strConfConnId="";
   }
   string m_strConfMeetingId;
   string m_strConfDevId;
   string m_strConfConnId;	
};

struct URActiveSpeakerInfo
{
   URActiveSpeakerInfo()
   {
      m_strCallId="";
      m_strConnId="";
   }
   ~URActiveSpeakerInfo()	 
   {
      m_strCallId="";
      m_strConnId="";
   }
   string m_strCallId;
   string m_strConnId;	
};

class URCONF_EVENT
{
   public:

      URCONF_EVENT()
      {
         m_strEventSeq="";
         m_strEventName="";
         m_strEventTime="";
      }
      ~URCONF_EVENT()
      {
         m_strEventSeq="";
         m_strEventName="";
         m_strEventTime="";
      }
      URCONF_EVENT(const URCONF_EVENT& tRobj)
      {
         m_strEventSeq = tRobj.m_strEventSeq.c_str();
         m_strEventName = tRobj.m_strEventName.c_str();
         m_strEventTime = tRobj.m_strEventTime.c_str();
      }
      URCONF_EVENT& operator=(const URCONF_EVENT& tRobj)
      {
         m_strEventSeq = tRobj.m_strEventSeq.c_str();
         m_strEventName = tRobj.m_strEventName.c_str();
         m_strEventTime = tRobj.m_strEventTime.c_str();
         return *this;
      }

      std::string m_strEventSeq;
      std::string m_strEventName;
      std::string m_strEventTime;
};

class URCONF_MEETING_INFO
{
   public:

      URCONF_MEETING_INFO()
      {
         m_strMeetingId="";
         m_strMeetingName="";
         m_bHostJoined=false;
      }
      ~URCONF_MEETING_INFO()
      {
         m_strMeetingId="";
         m_strMeetingName="";
         m_bHostJoined=false;
      }
      URCONF_MEETING_INFO(const URCONF_MEETING_INFO& tRobj)
      {
         m_strMeetingId = tRobj.m_strMeetingId.c_str();
         m_strMeetingName = tRobj.m_strMeetingName.c_str();
         m_bHostJoined = tRobj.m_bHostJoined;
      }
      URCONF_MEETING_INFO& operator=(const URCONF_MEETING_INFO& tRobj)
      {
         m_strMeetingId = tRobj.m_strMeetingId.c_str();
         m_strMeetingName = tRobj.m_strMeetingName.c_str();
         m_bHostJoined = tRobj.m_bHostJoined;
         return *this;
      }

      std::string m_strMeetingId;
      std::string m_strMeetingName;
      bool m_bHostJoined;
};

class URCONF_USER_INFO
{
   public:

      URCONF_USER_INFO()
      {
         m_strUserId="";
         m_strSipId="";
         m_strConnId="";
         m_strDeviceId="";
         m_strUserType="";
         m_strBrowserType="";
         m_strRecFilePath="";
         m_strRecUrl="";
         m_strRecId="";
         m_iPlanType=0;
      }
      ~URCONF_USER_INFO()
      {
         m_strUserId="";
         m_strSipId="";
         m_strConnId="";
         m_strDeviceId="";
         m_strUserType="";
         m_strBrowserType="";
         m_strRecFilePath="";
         m_strRecUrl="";
         m_strRecId="";
         m_iPlanType=0;
      }
      URCONF_USER_INFO(const URCONF_USER_INFO& tRobj)
      {
         m_strUserId = tRobj.m_strUserId.c_str();
         m_strSipId = tRobj.m_strSipId.c_str();
         m_strConnId = tRobj.m_strConnId.c_str();
         m_strDeviceId = tRobj.m_strDeviceId.c_str();
         m_strUserType = tRobj.m_strUserType.c_str();
         m_strBrowserType = tRobj.m_strBrowserType.c_str();
         m_strRecFilePath = tRobj.m_strRecFilePath.c_str();
         m_strRecUrl = tRobj.m_strRecUrl.c_str();
         m_strRecId = tRobj.m_strRecId.c_str();
         m_iPlanType = tRobj.m_iPlanType;
      }
      URCONF_USER_INFO& operator=(const URCONF_USER_INFO& tRobj)
      {
         m_strUserId = tRobj.m_strUserId.c_str();
         m_strSipId = tRobj.m_strSipId.c_str();
         m_strConnId = tRobj.m_strConnId.c_str();
         m_strDeviceId = tRobj.m_strDeviceId.c_str();
         m_strUserType = tRobj.m_strUserType.c_str();
         m_strBrowserType = tRobj.m_strBrowserType.c_str();
         m_strRecFilePath = tRobj.m_strRecFilePath.c_str();
         m_strRecUrl = tRobj.m_strRecUrl.c_str();
         m_strRecId = tRobj.m_strRecId.c_str();
         m_iPlanType = tRobj.m_iPlanType;
         return *this;
      }

      std::string m_strUserId;
      std::string m_strSipId;
      std::string m_strConnId;
      std::string m_strDeviceId;
      std::string m_strUserType;
			std::string m_strBrowserType;
      std::string m_strRecFilePath;
      std::string m_strRecUrl;
      std::string m_strRecId;
      int m_iPlanType;
};

class URMEET_USER_INFO
{
	public:

		URMEET_USER_INFO()
      {
			m_strEmail="";
			m_strProfileImg="";
			m_strSipId="";
			m_strName="";
			m_strExtn="";
			m_strUserType="";
			m_iRoleType=0;
		}
		~URMEET_USER_INFO()
      {
			m_strEmail="";
			m_strProfileImg="";
			m_strSipId="";
			m_strName="";
			m_strExtn="";
			m_strUserType="";
			m_iRoleType=0;
		}
		URMEET_USER_INFO(const URMEET_USER_INFO& tRobj)
      {
			m_strEmail = tRobj.m_strEmail.c_str();
			m_strProfileImg = tRobj.m_strProfileImg.c_str();
			m_strSipId = tRobj.m_strSipId.c_str();
			m_strName = tRobj.m_strName.c_str();
			m_strExtn = tRobj.m_strExtn.c_str();
			m_strUserType = tRobj.m_strUserType.c_str();
			m_iRoleType = tRobj.m_iRoleType;
		}
		URMEET_USER_INFO& operator=(const URMEET_USER_INFO& tRobj)
      {
			m_strEmail = tRobj.m_strEmail.c_str();
			m_strProfileImg = tRobj.m_strProfileImg.c_str();
			m_strSipId = tRobj.m_strSipId.c_str();
			m_strName = tRobj.m_strName.c_str();
			m_strExtn = tRobj.m_strExtn.c_str();
			m_strUserType = tRobj.m_strUserType.c_str();
			m_iRoleType = tRobj.m_iRoleType;
			return *this;
		}

		std::string m_strEmail;
		std::string m_strProfileImg;
		std::string m_strSipId;
		std::string m_strName;
		std::string m_strExtn;
		std::string m_strUserType;
		int m_iRoleType;
};

class URCONF_MEDIA_INFO
{
   public:

      URCONF_MEDIA_INFO()
      {
         m_bVideoEnabled=false;
         m_bAudioEnabled=false;
         m_bRecordingEnabled=false;
         m_strSdp = "";
      }
      ~URCONF_MEDIA_INFO()
      {
         m_bVideoEnabled=false;
         m_bAudioEnabled=false;
         m_bRecordingEnabled=false;
         m_strSdp = "";
      }
      URCONF_MEDIA_INFO(const URCONF_MEDIA_INFO& tRobj)
      {
         m_bVideoEnabled = tRobj.m_bVideoEnabled;
         m_bAudioEnabled = tRobj.m_bAudioEnabled;
         m_bRecordingEnabled = tRobj.m_bRecordingEnabled;
         m_strSdp = tRobj.m_strSdp;
      }
      URCONF_MEDIA_INFO& operator=(const URCONF_MEDIA_INFO& tRobj)
      {
         m_bVideoEnabled = tRobj.m_bVideoEnabled;
         m_bAudioEnabled = tRobj.m_bAudioEnabled;
         m_bRecordingEnabled = tRobj.m_bRecordingEnabled;
         m_strSdp = tRobj.m_strSdp;
         return *this;
      }

      bool m_bVideoEnabled;
      bool m_bAudioEnabled;
      bool m_bRecordingEnabled;
      std::string m_strSdp;
};

class URCONF_PARTICIPANT_INFO
{
   public:

      URCONF_PARTICIPANT_INFO()
      {
         m_strCallId="";
         m_strConnId="";
         m_strUserId="";
         m_strUserType="";
      }
      ~URCONF_PARTICIPANT_INFO()
      {
         m_strCallId="";
         m_strConnId="";
         m_strUserId="";
         m_strUserType="";
      }
      URCONF_PARTICIPANT_INFO(const URCONF_PARTICIPANT_INFO& tRobj)
      {
         m_strCallId = tRobj.m_strCallId.c_str();
         m_strConnId = tRobj.m_strConnId.c_str();
         m_strUserId = tRobj.m_strUserId.c_str();
         m_strUserType = tRobj.m_strUserType.c_str();
      }
      URCONF_PARTICIPANT_INFO& operator=(const URCONF_PARTICIPANT_INFO& tRobj)
      {
         m_strCallId = tRobj.m_strCallId.c_str();
         m_strConnId = tRobj.m_strConnId.c_str();
         m_strUserId = tRobj.m_strUserId.c_str();
         m_strUserType = tRobj.m_strUserType.c_str();
         return *this;
      }

      std::string m_strCallId;
      std::string m_strConnId;
      std::string m_strUserId;
      std::string m_strUserType;
};

class URCONF_CCAAS_INFO
{
   public:

      URCONF_CCAAS_INFO()
      {
				 m_strCli = "";
				 m_strDdi = "";
				 m_strCallType = "";
				 m_strQueueId = "";
				 m_strCustomerName = "";
				 m_strCustomerNum = "";
				 m_strCustomerId = "";
				 m_strAgentId = "";
				 m_strAgentExt = "";
				 m_strAgentMail = "";
				 m_strDomainId = "";
				 m_strAnncText = "";
				 m_strPrimaryAgentId = "";
      }
      ~URCONF_CCAAS_INFO()
      {
				 m_strCli = "";
				 m_strDdi = "";
				 m_strCallType = "";
         m_strQueueId = "";
         m_strCustomerName = "";
         m_strCustomerNum = "";
				 m_strCustomerId = "";
				 m_strAgentId = "";
				 m_strAgentExt = "";
				 m_strAgentMail = "";
				 m_strDomainId = "";
				 m_strAnncText = "";
				 m_strPrimaryAgentId = "";
      }
      URCONF_CCAAS_INFO(const URCONF_CCAAS_INFO& tRobj)
      {
				 m_strCli = tRobj.m_strCli.c_str();
				 m_strDdi = tRobj.m_strDdi.c_str();
				 m_strCallType = tRobj.m_strCallType.c_str();
         m_strQueueId = tRobj.m_strQueueId.c_str();
         m_strCustomerName = tRobj.m_strCustomerName.c_str();
         m_strCustomerNum = tRobj.m_strCustomerNum.c_str();
				 m_strCustomerId = tRobj.m_strCustomerId.c_str();
				 m_strAgentId = tRobj.m_strAgentId.c_str();
				 m_strAgentExt = tRobj.m_strAgentExt.c_str();
				 m_strAgentMail = tRobj.m_strAgentMail.c_str();
				 m_strDomainId = tRobj.m_strDomainId.c_str();
				 m_strAnncText = tRobj.m_strAnncText.c_str();
				 m_strPrimaryAgentId = tRobj.m_strPrimaryAgentId.c_str();
      }
      URCONF_CCAAS_INFO& operator=(const URCONF_CCAAS_INFO& tRobj)
      {
				 m_strCli = tRobj.m_strCli.c_str();
				 m_strDdi = tRobj.m_strDdi.c_str();
				 m_strCallType = tRobj.m_strCallType.c_str();
         m_strQueueId = tRobj.m_strQueueId.c_str();
         m_strCustomerName = tRobj.m_strCustomerName.c_str();
         m_strCustomerNum = tRobj.m_strCustomerNum.c_str();
				 m_strCustomerId = tRobj.m_strCustomerId.c_str();
				 m_strAgentId = tRobj.m_strAgentId.c_str();
				 m_strAgentExt = tRobj.m_strAgentExt.c_str();
				 m_strAgentMail = tRobj.m_strAgentMail.c_str();
				 m_strDomainId = tRobj.m_strDomainId.c_str();
				 m_strAnncText = tRobj.m_strAnncText.c_str();
				 m_strPrimaryAgentId = tRobj.m_strPrimaryAgentId.c_str();
         return *this;
      }

      std::string m_strCli;
      std::string m_strDdi;
      std::string m_strCallType;
      std::string m_strQueueId;
      std::string m_strCustomerName;
      std::string m_strCustomerNum;
      std::string m_strCustomerId;
      std::string m_strAgentId;
      std::string m_strAgentExt;
      std::string m_strAgentMail;
      std::string m_strDomainId;
      std::string m_strAnncText;
      std::string m_strPrimaryAgentId;
};

typedef std::map<std::string,URCONF_USER_INFO> CONF_USER_INFO_MAP;
typedef CONF_USER_INFO_MAP::iterator ITR_CONF_USER_INFO_MAP;
typedef std::deque <URCONF_PARTICIPANT_INFO> CONF_PARTICIPANT_INFO_DEQUE;
typedef std::deque<URMEET_USER_INFO> MEET_USER_INFO_DEQUE;

class URCBSIOUserInfo
{
   public:
      URCBSIOUserInfo()=default;
      ~URCBSIOUserInfo()=default;
      URCBSIOUserInfo(const URCBSIOUserInfo& tRobj)
      {
         m_strConfCallId = tRobj.m_strConfCallId.c_str();
	 m_isCcaas = tRobj.m_isCcaas;
         m_tURConfEvent = tRobj.m_tURConfEvent;
         m_tURConfMeetInfo = tRobj.m_tURConfMeetInfo;
         m_tURConfUserInfoList=tRobj.m_tURConfUserInfoList;
         m_tURConfUserMediaInfo=tRobj.m_tURConfUserMediaInfo;
      }
      URCBSIOUserInfo& operator=(const URCBSIOUserInfo& tRobj)
      {
         m_strConfCallId = tRobj.m_strConfCallId.c_str();
	 m_isCcaas = tRobj.m_isCcaas;
         m_tURConfEvent = tRobj.m_tURConfEvent;
         m_tURConfMeetInfo = tRobj.m_tURConfMeetInfo;
         m_tURConfUserInfoList=tRobj.m_tURConfUserInfoList;
         m_tURConfUserMediaInfo=tRobj.m_tURConfUserMediaInfo;
         return *this;
      }

      std::string m_strConfCallId;
      bool m_isCcaas;
      URCONF_EVENT m_tURConfEvent;
      URCONF_MEETING_INFO m_tURConfMeetInfo;
      CONF_USER_INFO_MAP m_tURConfUserInfoList;
      CONF_PARTICIPANT_INFO_DEQUE m_dequeURConfParticipantList;
      MEET_USER_INFO_DEQUE m_dequeURMeetUserList;
      URCONF_MEDIA_INFO m_tURConfUserMediaInfo;
      URCONF_CCAAS_INFO m_tURConfCcaasInfo;
};

/*
 * SocketIO-ConfBridge Defines Starts
*/

/*
 * ConfRoomInfo Defines For CallRedirection Starts
*/

typedef enum URCBBroadCastMsgType
{
  URCONF_BROADCAST_MSG_UNKNOWN = 0,
  URCONF_BROADCAST_MSG_ROOM_CREATED = 1,
  URCONF_BROADCAST_MSG_ROOM_DELETED = 2,
}URCBBroadCastMsgType;

class URCBRemoteConfRoomInfo
{
   public:

      URCBRemoteConfRoomInfo()
      {
         m_strConfBridgeIp="";
         m_strConfRoomId="";
      }
      ~URCBRemoteConfRoomInfo()
      {
         m_strConfBridgeIp="";
         m_strConfRoomId="";
      }
      URCBRemoteConfRoomInfo(const URCBRemoteConfRoomInfo& tRobj){
         m_strConfBridgeIp = tRobj.m_strConfBridgeIp.c_str();
         m_strConfRoomId = tRobj.m_strConfRoomId.c_str();
      }
      URCBRemoteConfRoomInfo& operator=(const URCBRemoteConfRoomInfo& tRobj){
         m_strConfBridgeIp = tRobj.m_strConfBridgeIp.c_str();
         m_strConfRoomId = tRobj.m_strConfRoomId.c_str();
         return *this;
      }

      std::string m_strConfBridgeIp;
      std::string m_strConfRoomId;
};

class URCBBroadCastMsg
{
  public:

        URCBBroadCastMsg()
        {
            m_strMsgUuid = "";
        }
        ~URCBBroadCastMsg()
        {
            m_strMsgUuid = "";
        }
        URCBBroadCastMsg(const URCBBroadCastMsg& tRobj)
        {
            m_strMsgUuid = tRobj.m_strMsgUuid.c_str();
            m_tURConfRoomInfo = tRobj.m_tURConfRoomInfo;
        }
        URCBBroadCastMsg& operator=(const URCBBroadCastMsg& tRobj)
        {
            m_strMsgUuid = tRobj.m_strMsgUuid.c_str();
            m_tURConfRoomInfo = tRobj.m_tURConfRoomInfo;
            return *this;
        }

        std::string m_strMsgUuid;
        URCBBroadCastMsgType m_eBroadCastMsgType;
        URCBRemoteConfRoomInfo m_tURConfRoomInfo;
};

class URCcaasBroadcastMsg
{
   public:
      URCcaasBroadcastMsg()
      {
         m_enModuleType = CALL_MESSAGE_NULL;
	 			 m_enChannelType = CALL_CHANNEL_NULL;
         m_enCcaasEvent = CB_COMMAND_NULL;
         m_strUuid = "";
				 m_strCallId = "";
         m_strCli = "";
         m_strDdi = "";
         m_strDomainName = "";
         m_strDomainId = "";
         m_strTopicName = "";
         m_strPbxIp = "";
				 m_strQueueId = "";
         m_strCustomerName = "";
         m_strCustomerNum = "";
				 m_strCustomerId = "";
				 m_strAgentExt = "";
         m_strAgentMail = "";
         m_strAnncText = "";
      }
      ~URCcaasBroadcastMsg()
      {
         m_enModuleType = CALL_MESSAGE_NULL;
				 m_enChannelType = CALL_CHANNEL_NULL;
         m_enCcaasEvent = CB_COMMAND_NULL;
         m_strUuid = "";
				 m_strCallId = "";
         m_strCli = "";
         m_strDdi = "";
         m_strDomainName = "";
         m_strDomainId = "";
         m_strTopicName = "";
         m_strPbxIp = "";
				 m_strQueueId = "";
         m_strCustomerName = "";
         m_strCustomerNum = "";
				 m_strCustomerId = "";
				 m_strAgentExt = "";
         m_strAgentMail = "";
         m_strAnncText = "";
      }
      URCcaasBroadcastMsg(const URCcaasBroadcastMsg& rhs)
      {
         m_enModuleType = rhs.m_enModuleType;
				 m_enChannelType = rhs.m_enChannelType;
         m_enCcaasEvent = rhs.m_enCcaasEvent;
         m_strUuid.assign(rhs.m_strUuid.c_str());
				 m_strCallId.assign(rhs.m_strCallId.c_str());
         m_strCli.assign(rhs.m_strCli.c_str());
         m_strDdi.assign(rhs.m_strDdi.c_str());
         m_strDomainName.assign(rhs.m_strDomainName.c_str());
         m_strDomainId.assign(rhs.m_strDomainId.c_str());
         m_strTopicName.assign(rhs.m_strTopicName.c_str());
         m_strPbxIp.assign(rhs.m_strPbxIp.c_str());
         m_strQueueId.assign(rhs.m_strQueueId.c_str());
         m_strCustomerName.assign(rhs.m_strCustomerName.c_str());
         m_strCustomerNum.assign(rhs.m_strCustomerNum.c_str());
				 m_strCustomerId.assign(rhs.m_strCustomerId.c_str());
				 m_strAgentExt.assign(rhs.m_strAgentExt.c_str());
				 m_strAgentMail.assign(rhs.m_strAgentMail.c_str());
         m_strAnncText.assign(rhs.m_strAnncText.c_str());
      }
      URCcaasBroadcastMsg& operator=(const URCcaasBroadcastMsg& rhs)
      {
         m_enModuleType = rhs.m_enModuleType;
				 m_enChannelType = rhs.m_enChannelType;
         m_enCcaasEvent = rhs.m_enCcaasEvent;
         m_strUuid.assign(rhs.m_strUuid.c_str());
				 m_strCallId.assign(rhs.m_strCallId.c_str());
         m_strCli.assign(rhs.m_strCli.c_str());
         m_strDdi.assign(rhs.m_strDdi.c_str());
         m_strDomainName.assign(rhs.m_strDomainName.c_str());
         m_strDomainId.assign(rhs.m_strDomainId.c_str());
         m_strTopicName.assign(rhs.m_strTopicName.c_str());
         m_strPbxIp.assign(rhs.m_strPbxIp.c_str());
         m_strQueueId.assign(rhs.m_strQueueId.c_str());
         m_strCustomerName.assign(rhs.m_strCustomerName.c_str());
         m_strCustomerNum.assign(rhs.m_strCustomerNum.c_str());
				 m_strCustomerId.assign(rhs.m_strCustomerId.c_str());
				 m_strAgentExt.assign(rhs.m_strAgentExt.c_str());
				 m_strAgentMail.assign(rhs.m_strAgentMail.c_str());
         m_strAnncText.assign(rhs.m_strAnncText.c_str());
         return *this;
      }

      PBXModuleType m_enModuleType;
      CcaasChannelType m_enChannelType;
      CBACDMsg m_enCcaasEvent;
      std::string m_strUuid;
			std::string m_strCallId;
      std::string m_strCli;
      std::string m_strDdi;
      std::string m_strDomainName;
      std::string m_strDomainId;
      std::string m_strTopicName;
      std::string m_strPbxIp;
      std::string m_strQueueId;
      std::string m_strCustomerName;
      std::string m_strCustomerNum;
			std::string m_strCustomerId;
			std::string m_strAgentExt;
			std::string m_strAgentMail;
      std::string m_strAnncText;
};
/*
 * ConfRoomInfo Defines For CallRedirection Ends
*/

#endif

