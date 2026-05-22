#ifndef SIP_MEDIA_HANDLER
#define SIP_MEDIA_HANDLER
#include "xglog.h"
#include "xGateSipController.h"

class SipAuthDetails
{
public:
	SipAuthDetails()
	{
		m_sUsername = "";
		m_sRealm = "";
		m_sNonce = "";
		m_sURI = "";
	}
	~SipAuthDetails()
	{
		m_sUsername = "";
		m_sRealm = "";
		m_sNonce = "";
		m_sURI = "";
	}
	std::string m_sUsername;
	std::string m_sRealm;
	std::string m_sNonce;
	std::string m_sURI;
};

class SIPOPTIONSInfo
{
public:
	SIPOPTIONSInfo()
	{
		m_iTrunkId = 0;
		m_iRespCode = 0;
		m_sIpAddress = "";
		m_sUsername = "";
	};
	~SIPOPTIONSInfo()
	{
		m_iTrunkId = 0;
		m_iRespCode = 0;
		m_sIpAddress = "";
		m_sUsername = "";
	};
	SIPOPTIONSInfo(const SIPOPTIONSInfo &rhs)
	{
		m_iTrunkId = rhs.m_iTrunkId;
		m_iRespCode = rhs.m_iRespCode;
		m_sIpAddress = rhs.m_sIpAddress.c_str();
		m_sUsername = rhs.m_sUsername.c_str();
	};
	SIPOPTIONSInfo &operator=(const SIPOPTIONSInfo &rhs)
	{
		m_iTrunkId = rhs.m_iTrunkId;
		m_iRespCode = rhs.m_iRespCode;
		m_sIpAddress = rhs.m_sIpAddress.c_str();
		m_sUsername = rhs.m_sUsername.c_str();
		return *this;
	};
	int m_iTrunkId;
	int m_iRespCode;
	std::string m_sIpAddress;
	std::string m_sUsername;
};

class SIPCallInfo
{
public:
	SIPCallInfo()
	{
		callid = "";
		uid = "";
		m_sCallingNumber = "";
		m_sCalledNumber = "";
		m_sDomainname = "";
		m_sCallingDeviceType = "";
		m_sPassword = "";
		m_sRouteIP = "";
		m_sKey = "";
		m_sCallInfo = "";
		m_sCallingExt = "";
	}

	~SIPCallInfo()
	{
		callid = "";
		uid = "";
		m_sCallingNumber = "";
		m_sCalledNumber = "";
		m_sDomainname = "";
		m_sCallingDeviceType = "";
		m_sPassword = "";
		m_sRouteIP = "";
		m_sKey = "";
		m_sCallInfo = "";
		m_sCallingExt = "";
	}

	SIPCallInfo(const SIPCallInfo &tIn)
	{
		callid = tIn.callid.c_str();
		uid = tIn.uid.c_str();
		m_sCallingNumber = tIn.m_sCallingNumber.c_str();
		m_sCalledNumber = tIn.m_sCalledNumber.c_str();
		m_sDomainname = tIn.m_sDomainname.c_str();
		m_sCallingDeviceType = tIn.m_sCallingDeviceType.c_str();
		m_sPassword = tIn.m_sPassword.c_str();
		m_sRouteIP = tIn.m_sRouteIP.c_str();
		m_sKey = tIn.m_sKey.c_str();
		m_sCallInfo = tIn.m_sCallInfo.c_str();
		m_sCallingExt = tIn.m_sCallingExt.c_str();
	}

	SIPCallInfo &operator=(const SIPCallInfo &tIn)
	{
		callid = tIn.callid.c_str();
		uid = tIn.uid.c_str();
		m_sCallingNumber = tIn.m_sCallingNumber.c_str();
		m_sCalledNumber = tIn.m_sCalledNumber.c_str();
		m_sDomainname = tIn.m_sDomainname.c_str();
		m_sCallingDeviceType = tIn.m_sCallingDeviceType.c_str();
		m_sPassword = tIn.m_sPassword.c_str();
		m_sRouteIP = tIn.m_sRouteIP.c_str();
		m_sKey = tIn.m_sKey.c_str();
		m_sCallInfo = tIn.m_sCallInfo.c_str();
		m_sCallingExt = tIn.m_sCallingExt.c_str();
		return *this;
	}

	std::string callid;
	std::string uid;
	std::string m_sCallingNumber;
	std::string m_sCalledNumber;
	std::string m_sDomainname;
	std::string m_sCallingDeviceType;
	std::string m_sPassword;
	std::string m_sRouteIP;
	std::string m_sKey;
	std::string m_sCallInfo;
	std::string m_sCallingExt;
};

class RegCallInfo
{
public:
	RegCallInfo()
	{
		regCallId = "";
		regTransId = "";
		domainName = "";
		contactAddress = "";
		regUserName = "";
		regUserId = "";
		regDeviceType = "";
		regDeviceId = "";
		routeIpAddress = "";
		regUserPassword = "";
		regTransportType = "";
		addressType = 0;
		aaa = 0;
		expires = 0;
		cseq = 0;
		proxyUserName = "";
		aorDetails = "";
		regAppType = "";
		regPush_id = "";
	}

	~RegCallInfo()
	{
		regCallId = "";
		regTransId = "";
		domainName = "";
		contactAddress = "";
		regUserName = "";
		regUserId = "";
		regDeviceType = "";
		regDeviceId = "";
		routeIpAddress = "";
		regUserPassword = "";
		regTransportType = "";
		addressType = 0;
		aaa = 0;
		expires = 0;
		cseq = 0;
		proxyUserName = "";
		aorDetails = "";
		regAppType = "";
		regPush_id = "";
	}

	RegCallInfo(const RegCallInfo &tIn)
	{
		regCallId = tIn.regCallId.c_str();
		regTransId = tIn.regTransId.c_str();
		domainName = tIn.domainName.c_str();
		contactAddress = tIn.contactAddress.c_str();
		regUserName = tIn.regUserName.c_str();
		regUserId = tIn.regUserId.c_str();
		regDeviceType = tIn.regDeviceType.c_str();
		regDeviceId = tIn.regDeviceId.c_str();
		routeIpAddress = tIn.routeIpAddress.c_str();
		regUserPassword = tIn.regUserPassword.c_str();
		regAppType = tIn.regAppType.c_str();
		regTransportType = tIn.regTransportType.c_str();
		addressType = tIn.addressType;
		aaa = tIn.aaa;
		expires = tIn.expires;
		cseq = tIn.cseq;
		proxyUserName = tIn.proxyUserName.c_str();
		aorDetails = tIn.aorDetails.c_str();
		regPush_id = tIn.regPush_id.c_str();
		regAuthDetails = tIn.regAuthDetails;
	}

	RegCallInfo &operator=(const RegCallInfo &tIn)
	{
		regCallId = tIn.regCallId.c_str();
		regTransId = tIn.regTransId.c_str();
		domainName = tIn.domainName.c_str();
		contactAddress = tIn.contactAddress.c_str();
		regUserName = tIn.regUserName.c_str();
		regTransportType = tIn.regTransportType.c_str();
		regUserId = tIn.regUserId.c_str();
		regDeviceType = tIn.regDeviceType.c_str();
		regDeviceId = tIn.regDeviceId.c_str();
		routeIpAddress = tIn.routeIpAddress.c_str();
		regUserPassword = tIn.regUserPassword.c_str();
		regAppType = tIn.regAppType.c_str();
		addressType = tIn.addressType;
		aaa = tIn.aaa;
		expires = tIn.expires;
		cseq = tIn.cseq;
		proxyUserName = tIn.proxyUserName.c_str();
		aorDetails = tIn.aorDetails.c_str();
		regPush_id = tIn.regPush_id.c_str();
		regAuthDetails = tIn.regAuthDetails;
		return *this;
	}

public:
	std::string regCallId;
	std::string regTransId;
	std::string domainName;
	std::string contactAddress;
	std::string regUserName;
	std::string regTransportType;
	std::string regUserId;
	std::string regDeviceType;
	std::string regAppType;
	std::string regDeviceId;
	std::string routeIpAddress;
	std::string regUserPassword;
	std::string proxyUserName;
	std::string aorDetails;
	std::string regPush_id;
	SipAuthDetails regAuthDetails;
	int addressType;
	int aaa;
	int expires;
	int cseq;
};

class xGateSipMsg : public xGateBaseMsg
{
public:
	typedef enum __sip_event_type
	{
		SIP_EVENT_UNKNOWN = 0,
		SIP_EVENT_DO_USER_REG_REQ = 1,  //sending Registration Request from signaling to controllor
		SIP_EVENT_DO_USER_REG_RES = 2,  //sending Registration Response from controllor to signaling
		SIP_EVENT_USER_DEREG_REQ = 3,
		SIP_EVENT_USER_REG_RESPONSE_ERROR = 4, //sending Registration Response error from controllor to signaling
		SIP_EVENT_USER_REG_UPDATE_AUTH_SUCCESS = 5, //sending Registration Auth success from signaling to controllor 
		SIP_EVENT_USER_REG_UPDATE_AUTH_FAILED = 6,//sending Registration Auth failled  from signaling to controllor 
		SIP_EVENT_INVITE_REQUEST = 7, //sending Invite Request from signaling to controllor
		SIP_EVENT_INVITE_RESPONSE = 8,//sending Invite Response from controllor to signaling
		SIP_EVENT_INVITE_RESPONSE_ERROR = 9,//sending Invite Response error from controllor to signaling
		SIP_EVENT_INVITE_INBOUND_PSTN_REQUEST = 10, //sending Invite Request for Inbound calls from signaling to controllor
		SIP_EVENT_INVITE_INBOUND_PSTN_RESPONSE = 11,//sending Invite Response for Inbound from controllor to signaling
		SIP_EVENT_INVITE_INBOUND_PSTN_ERROR = 12, //sending Invite Response Error for Inbound from controllor to signaling
		SIP_EVENT_INVITE_MS_TEAMS_REQUEST = 13,//sending Invite Request for MS Teams calls from signaling to controllor
		SIP_EVENT_INVITE_MS_TEAMS_RESPONSE = 14,//sending Invite Response for MS Teams calls from controllor to signaling
		SIP_EVENT_INVITE_MS_TEAMS_ERROR = 15,//sending Invite Response Error for MS Teams calls from controllor to signaling
		SIP_EVENT_OPTIONS_REQUEST = 16, //sending SIP Options Request from Monitoring service to sip service
		SIP_EVENT_OPTION_SUCCESS_RESPONSE = 17, //sending SIP Options success response from sip service to Monitoring service
		SIP_EVENT_OPTIONS_FAILED_RESPONSE = 18, //sending SIP Options failed response from sip service to Monitoring service
		SIP_EVENT_OPTIONS_TIMEOUT_RESPONSE =19 //sending SIP Options timeout response from sip service to Monitoring service
	} SIP_EVENT_TYPE;

	typedef enum __sip_msg_type
	{
		SIP_MSG_UNKNOWN = 0,
		SIP_MSG_NOTIFY = 1,
		SIP_MSG_COMMAND = 2,
		SIP_MSG_REQUEST = 3,
		SIP_MSG_RESPONSE = 4
	} SIP_MSG_TYPE;

private:
	xGateSipMsg &operator=(const xGateSipMsg &);
	RegCallInfo m_tRegInfo;
	SIPCallInfo m_tSipInfo;
	SIP_MSG_TYPE m_tenumSipMsgType;
	SIP_EVENT_TYPE m_tenumSipEventType;
	SIPOPTIONSInfo m_tOptionsInfo;

public:
	xGateSipMsg() {}
	xGateSipMsg(IURDefines::MODULE_ID src, IURDefines::MODULE_ID dst, SIP_EVENT_TYPE tsipEventType, SIP_MSG_TYPE tsipmsgType);
	virtual ~xGateSipMsg();
	xGateSipMsg(const xGateSipMsg &msg);
	bool findUserDeviceType(string &sDeviceName, UserDevice &deviceType);
	bool fillRegistrationInfoDetails(xGateSipMsg *sMsg, xGateSofiaSipRegobj *sipregobj);
	bool fillInviteRequestDetails(xGateSipMsg *sMsg,xGateCallObj *callobj);

	SIPCallInfo &getSipInfoDetails()
	{
		return m_tSipInfo;
	}

	void setRegInfoDetails(RegCallInfo &regInfo)
	{
		m_tRegInfo = regInfo;
	}

	RegCallInfo &getRegInfoDetails()
	{
		return m_tRegInfo;
	}

	SIPOPTIONSInfo &getSipOptionsInfo()
	{
		return m_tOptionsInfo;
	}

	void setSipOptionsInfo(SIPOPTIONSInfo &OptionsInfo)
	{
		 m_tOptionsInfo = OptionsInfo;
	}


	void setAorDetails(std::string aorDetails)
	{
		m_tRegInfo.aorDetails = aorDetails.c_str();
	}

	std::string &getAorDetails()
	{
		return m_tRegInfo.aorDetails;
	}

	SIP_EVENT_TYPE getSipEventType()
	{
		return m_tenumSipEventType;
	}

	SIP_MSG_TYPE getSipMsgType()
	{
		return m_tenumSipMsgType;
	}

	std::string getUserPassword()
	{
		return m_tSipInfo.m_sPassword.c_str();
	}

	void setUserPassword(std::string password)
	{
		m_tSipInfo.m_sPassword = password.c_str();
	}

	void setRouteIP(std::string route_ip)
	{
		m_tSipInfo.m_sRouteIP = route_ip;
	}

	std::string getRouteIP()
	{
		return m_tSipInfo.m_sRouteIP;
	}

	void setCallInfo(std::string callinfo)
	{
		m_tSipInfo.m_sCallInfo = callinfo;
	}

	std::string getCallInfo()
	{
		return m_tSipInfo.m_sCallInfo;
	}

	void setTrunkId(int turnid)
	{
		m_tOptionsInfo.m_iTrunkId = turnid;
	}

	int getTrunkId()
	{
		return m_tOptionsInfo.m_iTrunkId;
	}

	void setResponseCode(int Rescode)
	{
		m_tOptionsInfo.m_iRespCode = Rescode;
	}

	int getResponseCode()
	{
		return m_tOptionsInfo.m_iRespCode;
	}

	void setTrunkIPAddress(std::string ipaddr)
	{
		m_tOptionsInfo.m_sIpAddress = ipaddr.c_str();
	}

	std::string getTrunkIPAddress()
	{
		return m_tOptionsInfo.m_sIpAddress;
	}

	void setTrunkUname(std::string uname)
	{
		m_tOptionsInfo.m_sUsername = uname.c_str();
	}
	
	std::string getTrunkUName()
	{
		return m_tOptionsInfo.m_sUsername;
	}
};
#endif
