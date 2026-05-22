#ifndef __URCBCALLCONTEXT_H__
#define __URCBCALLCONTEXT_H__

//#include "AcdScheduler.h"
#include "URConfRoom.h"
#include "xGateUtil.h"
#include <list>


typedef enum{
	CONF_CALL_FEATURE_NULL=0,
	CONF_CALL_FEATURE_AUDIO=1,
	CONF_CALL_FEATURE_VIDEO=2,
} ConfCallFeatureType;

typedef enum{
	CALL_STATE_NULL=0,
	CALL_STATE_INIT=1,
	CALL_STATE_CONNECTED=2,
} CallState;

class URCBCallContext
{
	public:
		typedef enum __urcb_call_status{
			URCBCALL_STATUS_NULL			  = 0x00,
			URCBCALL_STATUS_NEW_CALL		  = 0x01, 
			URCBCALL_STATUS_CALL_ANSWER_ALLOW	  = 0x02,
			URCBCALL_STATUS_REQ_CONF_USER_AOR_DETAIL  = 0x04,
			URCBCALL_STATUS_MAKE_CALL_REQ		  = 0x08,
			URCBCALL_STATUS_MAKE_CALL_RES		  = 0x10,
			URCBCALL_STATUS_CALL_CONNECTED		  = 0x20,
			URCBCALL_STATUS_CALL_DISCONNECTED 	  = 0x40, 
			URCBCALL_STATUS_REQUEST_TIMEOUT 	  = 0x80, 
			URCBCALL_STATUS_VALIDATION_FAILED 	  = 0x100,
			URCBCALL_STATUS_SDP_UPDATE_REQ            = 0x200,
			URCBCALL_STATUS_SDP_UPDATE_RES            = 0x400,
			URCBCALL_STATUS_MEDIA_UPDATE_SDP_REQ      = 0x800,
			URCBCALL_STATUS_MEDIA_UPDATE_SDP_RES      = 0x1000,
			URCBCALL_STATUS_DTLS_RECONNECT_REQ        = 0x2000, 
			URCBCALL_STATUS_DTLS_RECONNECT_RES        = 0x4000, 
			URCBCALL_STATUS_CONF_DEL_PARTICIPANT      = 0x8000,
			URCBCALL_STATUS_TILE_CHANGE_REQ           = 0x10000,
			URCBCALL_STATUS_TILE_CHANGE_RES           = 0x20000,
			URCBCALL_STATUS_SCREENSHARE_VIDEO_REQ     = 0x40000, 
			URCBCALL_STATUS_ACTIVE_SPEAKER_VIDEO_REQ  = 0x80000,  
			URCBCALL_STATUS_RECORDING_START	  	  = 0x100000,  
			URCBCALL_STATUS_RECORDING_PAUSE	  	  = 0x200000,  
			URCBCALL_STATUS_RECORDING_RESUME	  = 0x400000,  
			URCBCALL_STATUS_RECORDING_STOP	  	  = 0x800000,  
			URCBCALL_STATUS_ACTIVE_SPEAKER_INFO  	  = 0x1000000,  
		}URCBCALL_STATUS;                           

		URCBCallContext()
		{
			m_iURCBCallStatus = URCBCALL_STATUS_NULL;
			m_strCallId = "";
			m_strConfRoomId = "";
			m_strConfConnId = "";
			m_strDeviceId = "";
		 	m_strMeetingName = "";
			m_strOrgCallId = "";
			m_strSSUserCallId = "";
			m_strSSUserConnId = "";
			m_strRootParticipantCallId = "";
			m_strRootParticipantExtn = "";
			m_strDomainName = "";
			m_strDomainId = "";
			m_strCli = "";
			m_strDdi = "";
			m_strCustomerId = "";
			m_strAgentId = "";
			m_strAgentExt = "";
			m_strAgentMail = "";
			m_strCalledNumber = "";
			m_strCallingNumber = "";
			m_strConfUserExtn = "";
			m_strBrowserType = "";
			m_strSDPInfo = "";
			m_strQueueId = "";
			m_strChannel = "";
			m_strAcdKey = "";
			m_strRecId = "";
			m_iRetryDialCount = 0;
			m_iPlanType = 0;
			m_enumConfUserType = CONF_USER_TYPE_NULL;
			m_enumCallState = CALL_STATE_NULL;
			m_enumCallFeatureType = CONF_CALL_FEATURE_NULL;
			m_enumUserDeviceType = EN_USER_DEVICE_UNKNOWN;
			m_enCcaasEvent = CB_COMMAND_NULL;
		}

		~URCBCallContext(){}

		URCBCallContext(const URCBCallContext& tRobj)
		{
			m_strChannel = tRobj.m_strChannel.c_str();
		}

		URCBCallContext& operator=(const URCBCallContext& tRobj)
		{
			m_strChannel = tRobj.m_strChannel.c_str();
		}

		void setStatus(URCBCALL_STATUS tStatus)
		{
			m_iURCBCallStatus = (int)tStatus;
		}

		URCBCALL_STATUS getStatus()
		{
			return (URCBCALL_STATUS)m_iURCBCallStatus;
		}

		void setCcaasEvent(CBACDMsg ccaasEvent)
		{
			m_enCcaasEvent = ccaasEvent;
		}

		CBACDMsg getCcaasEvent()
		{
			return m_enCcaasEvent;
		}

		std::string m_strCallId;
		std::string m_strConfRoomId;
		std::string m_strConfConnId;
		std::string m_strDeviceId;
		std::string m_strMeetingName;
		std::string m_strOrgConnId;
		std::string m_strDstConnId;
		std::string m_strOrgCallId;
		std::string m_strSSUserCallId;
		std::string m_strSSUserConnId;
		std::string m_strRootParticipantCallId;
		std::string m_strRootParticipantExtn;
		std::string m_strDomainName;
		std::string m_strDomainId;
		std::string m_strCli;
		std::string m_strDdi;
		std::string m_strCalledNumber;
		std::string m_strCallingNumber;
		std::string m_strConfUserExtn;
		std::string m_strSDPInfo;
		std::string m_strAor;
		std::string m_strConfUserAgent;
		std::string m_strBrowserType;
		std::string m_strRecFilePath;
		std::string m_strRecUrl;
		std::string m_strRecId;
		std::string m_strSipId;
		std::string m_strQueueId;
		std::string m_strCustomerName;
		std::string m_strCustomerNum;
		std::string m_strCustomerId;
		std::string m_strAgentId;
		std::string m_strAgentExt;
		std::string m_strAgentMail;
		std::string m_strChannel;
		std::string m_strAcdKey;
		AddressOfRecord m_tUserAor;

		std::vector<std::string> m_vAcdChannelList;
		std::deque<std::string> m_dequeDialParticipants;
		std::deque<std::string> m_dequeUpdatedSDP;
		std::deque<URCONF_PARTICIPANT_INFO> m_dequeURConfParticipantList;
		std::deque<URMEET_USER_INFO> m_dequeURMeetUserList;

		int m_iRetryDialCount;
		int m_iPlanType;
		bool m_isCcaas;

		CallState m_enumCallState;
		UserDevice m_enumUserDeviceType;
		ConfUserType m_enumConfUserType;
		ConfCallFeatureType m_enumCallFeatureType;
		URConfCallType m_enCallType;


	private:
		int m_iURCBCallStatus;
		CBACDMsg m_enCcaasEvent;
};


#endif
