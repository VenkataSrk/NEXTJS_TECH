#ifndef SIP_MEDIA_HANDLER
#define SIP_MEDIA_HANDLER 
#include "xglog.h" 
#include "xGateBaseMsg.h"
#include "xGateSipController.h"

class MediaDetails {
	
public:

	MediaDetails()
	{
		msgType = "";
		callId = "";
		mport = 0;
		media = "";
		codec = ""; 
	}

	~MediaDetails()
	{
                msgType = "";
                callId = "";
                mport = 0;
                media = "";
                codec = "";
	}

	MediaDetails& operator=(MediaDetails &src)
	{
		msgType = src.msgType;
		callId   = src.callId;
		mport = src.mport;
		media = src.media;
		codec = src.media;
		return *this;
	}

	public:

	std::string msgType;
	std::string callId;
	std::string uid;
	unsigned long mport;
	std::string media;
	std::string codec;

};

class SipInfoReqDetails 
{
	public:
	SipInfoReqDetails()
	{
		dtmfdigits = "";
		duration = "";
	}
	~SipInfoReqDetails()
	{
		                dtmfdigits = "";
                duration = "";

	}

	void setDtmgDigits(std::string d_digits)
	{
		dtmfdigits = d_digits;
	}
	void setDtmgDuration(std::string d_duration)
	{
		duration = d_duration;
	}
	std::string getDtmfDigits()
	{
		return dtmfdigits;
	}
	std::string getDtmfDuration()
	{
		return duration;
	}

	public:
	std::string dtmfdigits;
	std::string duration;

};

class SIPCallInfo
{
	public:
		SIPCallInfo() {
			routeAddr = "";
			csCallingContact = "";
			callingDeviceType = "";
			csaCalled = "" ;
			cnCalled = "";
			csaCalling = "";
			cui = "";cpinfo = "";
			inband_dtmf_method = false;
			isPstnNumber = false;
			sipinfo_method = false;
			proxyUserName = "";
			calledDeviceType = "";
			call_id = "";
			crUserName = "";
			crPassword = "";
			uid = "" ;
			routeIpAddress = "";
			isTransferenabled = false;
			transportType = "";
			digitReceived = "";
			fromNumber = "";
			toNumber = "";
			transferTarget = "";
			domainName = "";
			deviceType = EN_USER_DEVICE_UNKNOWN;
			contactAddress = "";
			confRoomId = "";
			confConnId = "";
			dstConnId = "";
			confUserType = 0;
			confUserAgent = "";
                        redirectIp = "";
		}

		~SIPCallInfo(){
			routeAddr = "";
			csCallingContact = "";
			callingDeviceType = "";
			csaCalled = "" ;
			cnCalled = "";
			csaCalling = "";
			cui = "";cpinfo = "";
			inband_dtmf_method = false;
			isPstnNumber = false;
			sipinfo_method = false;
			proxyUserName = "";
			calledDeviceType = "";
			call_id = "";
			crUserName = "";
			crPassword = "";
			uid = "" ;
			routeIpAddress = "";
			isTransferenabled = false;
			transportType = "";
			digitReceived = "";
			fromNumber = "";
			toNumber = "";
			transferTarget = "";
			domainName = "";
			deviceType = EN_USER_DEVICE_UNKNOWN;
			contactAddress = "";
			confRoomId = "";
			confConnId = "";
			dstConnId = "";
			confUserType = 0;
			confUserAgent = "";
                        redirectIp = "";
		}

		SIPCallInfo(const SIPCallInfo& tIn){
			routeAddr = tIn.routeAddr.c_str();
			csCallingContact = tIn.csCallingContact.c_str();
			calledDeviceType = tIn.calledDeviceType.c_str();
			csaCalled = tIn.csaCalled.c_str();
			cnCalled = tIn.cnCalled.c_str();
			csaCalling = tIn.csaCalling.c_str();
			cui = tIn.cui.c_str();
			cpinfo = tIn.cpinfo.c_str();
			inband_dtmf_method = tIn.inband_dtmf_method;
			isPstnNumber = tIn.isPstnNumber;
			sipinfo_method = tIn.sipinfo_method;
			proxyUserName = tIn.proxyUserName.c_str();
			calledDeviceType = tIn.calledDeviceType.c_str();
			call_id = tIn.call_id.c_str();
			crUserName = tIn.crUserName.c_str();
			crPassword = tIn.crPassword.c_str();
			uid = tIn.uid.c_str();
			routeIpAddress = tIn.routeIpAddress.c_str();
			isTransferenabled = tIn.isTransferenabled;
			transportType = tIn.transportType.c_str();
			digitReceived = tIn.digitReceived.c_str();
			fromNumber = tIn.fromNumber.c_str();
			toNumber = tIn.toNumber.c_str();
			transferTarget = tIn.transferTarget.c_str();
			domainName = tIn.domainName.c_str();
			deviceType = tIn.deviceType;	
			contactAddress = tIn.contactAddress.c_str();
                        confRoomId = tIn.confRoomId.c_str();
                        confConnId = tIn.confConnId.c_str();;
                        dstConnId = tIn.dstConnId.c_str();;
                        confUserType = tIn.confUserType;
		        confUserAgent = tIn.confUserAgent.c_str();
                        redirectIp = tIn.redirectIp.c_str();
		}

		SIPCallInfo &  operator=(const SIPCallInfo& tIn){
			routeAddr = tIn.routeAddr.c_str();
			csCallingContact = tIn.csCallingContact.c_str();
			calledDeviceType = tIn.calledDeviceType.c_str();
			csaCalled = tIn.csaCalled.c_str();
			cnCalled = tIn.cnCalled.c_str();
			csaCalling = tIn.csaCalling.c_str();
			cui = tIn.cui.c_str();
			cpinfo = tIn.cpinfo.c_str();
			inband_dtmf_method = tIn.inband_dtmf_method;
			isPstnNumber = tIn.isPstnNumber;
			sipinfo_method = tIn.sipinfo_method;
			proxyUserName = tIn.proxyUserName.c_str();
			calledDeviceType = tIn.calledDeviceType.c_str();
			call_id = tIn.call_id.c_str();
			crUserName = tIn.crUserName.c_str();
			crPassword = tIn.crPassword.c_str();
			uid = tIn.uid.c_str();
			routeIpAddress = tIn.routeIpAddress.c_str();
			isTransferenabled = tIn.isTransferenabled;
			transportType = tIn.transportType.c_str();
			digitReceived = tIn.digitReceived.c_str();
			fromNumber = tIn.fromNumber.c_str();
			toNumber = tIn.toNumber.c_str();
			transferTarget = tIn.transferTarget.c_str();
			domainName = tIn.domainName.c_str();
			deviceType = tIn.deviceType;
			contactAddress = tIn.contactAddress.c_str();
                        confRoomId = tIn.confRoomId.c_str();
                        confConnId = tIn.confConnId.c_str();;
                        dstConnId = tIn.dstConnId.c_str();;
                        confUserType = tIn.confUserType;
		        confUserAgent = tIn.confUserAgent.c_str();
                        redirectIp = tIn.redirectIp.c_str();
			return *this;
		}

	public:
		std::string routeAddr;
		std::string csCallingContact;
		std::string callingDeviceType;
		std::string calledDeviceType;
		std::string csaCalled;
		std::string cnCalled;
		std::string csaCalling;
		std::string cnCalling;
		std::string proxyUserName;
		std::string routeIpAddress;
		std::string call_id;
		std::string uid;
		std::string callType;
		std::string disconnectReason;
		std::string domainId;
		std::string crUserName;
		std::string crPassword; 
		std::string cui;
		std::string cpinfo;
		std::string digitReceived;
		std::string transportType;
		std::string m_sTargetNumber;//voicemail box number
		std::string m_sTargetReason;//voicemail box number

		std::string fromNumber;
		std::string toNumber;
		std::string transferTarget;
		std::string domainName;
		std::string contactAddress;
		UserDevice deviceType;
		xGateSDPInfo m_sdpInfoDetails[2];
		MappedUserList m_mappedUserList;
		bool isPstnNumber;
		bool isTransferenabled;
		bool inband_dtmf_method;
		bool sipinfo_method;

                std::string confRoomId;
                std::string confConnId;
                std::string dstConnId;
                int confUserType;
		std::string confUserAgent;
		std::string redirectIp;
};


class RegCallInfo 
{
	public: 
		RegCallInfo()
		{
			regCallId = "";
			domainName = "";
			contactAddress = "";
			regUserName = "";
			regUserId = "";
			regDeviceType = "";
			regDeviceId = "";
			routeIpAddress = "";
			regUserPassword = "";
			addressType = 0;
			aaa = 0;expires = 0; 
			cseq = 0;
			proxyUserName = "";
			aorDetails = "";
			regAppType = "";
		}

		~ RegCallInfo()
		{
			regCallId = "";
			domainName = "";
			contactAddress = "";
			regUserName = "";
			regUserId = "";
			regDeviceType = "";
			regDeviceId = "";
			routeIpAddress = "";
			regUserPassword = "";
			addressType = 0;
			aaa = 0;expires = 0;
			cseq = 0;
			proxyUserName = "";
			aorDetails = "";
			regAppType = "";
		}

		RegCallInfo(const RegCallInfo& tIn){
			regCallId =tIn.regCallId.c_str();
			domainName = tIn.domainName.c_str();
			contactAddress = tIn.contactAddress.c_str();
			regUserName = tIn.regUserName.c_str();
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
		}

		RegCallInfo &  operator=(const RegCallInfo& tIn){
			regCallId =tIn.regCallId.c_str();
			domainName = tIn.domainName.c_str();
			contactAddress = tIn.contactAddress.c_str();
			regUserName = tIn.regUserName.c_str();
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
			return *this;
		}

	public :

		std::string regCallId;
		std::string domainName;
		std::string contactAddress;
		std::string regUserName;
		std::string regUserId ;
		std::string regDeviceType;
		std::string regAppType;
		std::string regDeviceId;
		std::string routeIpAddress;
		std::string regUserPassword;
		std::string proxyUserName;
		std::string aorDetails;
		int addressType;
		int aaa;
		int expires;
		int cseq;

};



class xGateSIPMsg: public xGateBaseMsg
{
	public:	
		typedef enum __sip_event_type {
			SIP_EVENT_UNKNOWN = 0,
			SIP_EVENT_NEW_CALL = 1,
			SIP_EVENT_ANSWER_CALL=2,
			SIP_EVENT_MEDIA_ALLOC_REQUEST=3,
			SIP_EVENT_MEDIA_ALLOC_RESPONSE=4,
			SIP_EVENT_CALL_CONNECTED=5,
			SIP_EVENT_TERMINATE_CALL=6,
			SIP_EVENT_CALL_DISCONNECTED=7,
			SIP_EVENT_REJECT_CALL=8,
			SIP_EVENT_SIP_INFO_REQUEST=9,
			SIP_EVENT_CALL_TRANSFER=10,
			SIP_EVENT_MAKE_CALL_REQUEST=11,
			SIP_EVENT_MAKE_CALL_FAILED_RESPONSE=12,
                        SIP_EVENT_AUTH_USER_REG = 13,
                        SIP_EVENT_SEND_401_UNAUTH_REQ = 14,
                        SIP_EVENT_GET_USER_PWD_REQ = 15,
                        SIP_EVENT_GET_USER_PWD_RES = 16,
                        SIP_EVENT_DO_USER_REG_REQ = 17,
			SIP_EVENT_DO_USER_REG_RES = 18,
			SIP_EVENT_AUTH_CALL_REQ = 19,
			SIP_EVENT_AUTH_CALL_RES = 20,
			SIP_EVENT_DE_REG_REQ = 21,
                        SIP_EVENT_ERROR_RESPONSE = 22,
                        SIP_EVENT_SDP_UPDATE_REQ = 23,
                        SIP_EVENT_SDP_UPDATE_RES = 24,
                        SIP_EVENT_MEDIA_UPDATE_SDP_REQ = 25,
                        SIP_EVENT_MEDIA_UPDATE_SDP_RES = 26,
			SIP_EVENT_REDIRECT_CALL = 27,
			SIP_EVENT_DTLS_RECONNECT_REQ = 28,
			SIP_EVENT_DTLS_RECONNECT_RES = 29,
		}SIP_EVENT_TYPE;
		typedef enum __sip_msg_type {
			SIP_MSG_UNKNOWN = 0,
			SIP_MSG_NOTIFY = 1,
			SIP_MSG_COMMAND=2,
			SIP_MSG_REQUEST=3,
			SIP_MSG_RESPONSE=4
		}SIP_MSG_TYPE;

		typedef enum _sip_error_event_type
		{
			SIP_ERROR_EVENT_NULL = 0,
			SIP_ERROR_EVENT_SERVER_TIMEOUT = 1,
			SIP_ERROR_EVENT_VALIDATION_FAILURE = 2,
			SIP_ERROR_EVENT_REQ_NOT_ACCEPTABLE = 3,
		}SIP_ERROR_EVENT_TYPE;

	private:

		xGateSIPMsg& operator=(const xGateSIPMsg&);
		MediaDetails 		mDetails;
		SIP_EVENT_TYPE 		m_tenumSipEventType;
		SIP_MSG_TYPE 		m_tenumSipMsgType;
		SIPCallInfo 		m_tSipInfo;
		RegCallInfo 		m_tRegInfo;
		SipInfoReqDetails              m_tSipInfoReqDetails;
	        SIP_ERROR_EVENT_TYPE 	m_tenumErrEventType;
		std::string m_tStrSdpInfo;
		std::map <std::string,std::string> m_tUnknownHeaderMap;
		std::map <std::string,multiDevice*> m_tMappedUserInfoMap;

	public:

		xGateSIPMsg(){}
		xGateSIPMsg(IURDefines::MODULE_ID src,IURDefines::MODULE_ID dst,SIP_EVENT_TYPE tsipEventType,SIP_MSG_TYPE tsipmsgType);
		virtual ~xGateSIPMsg();
		xGateSIPMsg(const xGateSIPMsg& msg);
		bool fillMediaDetails(xGateSIPMsg *sMsg, xGateCallObj* callObj);
		bool fillSipInfoReqDetails(xGateSIPMsg *sMsg, xGateCallObj* callObj);
		bool findUserDeviceType(string &sDeviceName,UserDevice &deviceType);
		bool fillRegistrationInfoDetails(xGateSIPMsg *sMsg,xGateSofiaSipRegobj *sipregobj);

		void setMediaDetails(MediaDetails &media)
		{
			mDetails = media;
		}
		SIPCallInfo & getSipInfoDetails()
		{
			return m_tSipInfo;
		}

		void setRegInfoDetails(RegCallInfo &regInfo)
		{
			m_tRegInfo = regInfo;
		}

		RegCallInfo& getRegInfoDetails()
		{
			return m_tRegInfo;
		}
	
		void setAorDetails(std::string aorDetails)
		{
			m_tRegInfo.aorDetails = aorDetails.c_str();
		}

		std::string& getAorDetails()
		{
			return m_tRegInfo.aorDetails;
		}

		void setSdpInfo (std::string sdpInfo)
		{
			m_tStrSdpInfo = sdpInfo.c_str();
		}

		std::string getSdpInfo()
		{
			return m_tStrSdpInfo.c_str();
		}

		void setRouteAddr (std::string routeAddr)
		{
			m_tSipInfo.routeIpAddress = routeAddr.c_str();
		}

		std::string getRouteAddr()
		{
			return m_tSipInfo.routeIpAddress.c_str();
		}
	
		void setUnKnownHeaderInfo(std::map <std::string,std::string> tUnknownInfoMap)
		{
			m_tUnknownHeaderMap = tUnknownInfoMap;
		}
		
		std::map<std::string,std::string> getUnKnownHeaderInfo()
		{
			return m_tUnknownHeaderMap;
		}

                void setMappedUserInfo(std::map <std::string,multiDevice*> tMappedUserInfoMap)
                {
                        m_tMappedUserInfoMap = tMappedUserInfoMap;
                }

                std::map<std::string,multiDevice*> getMappedUserInfo()
                {
                        return m_tMappedUserInfoMap;
                }

		void setSipInfoDetails(MediaDetails &sipInfo)
		{
			mDetails = sipInfo;
		}
		MediaDetails & getMediaDetails()
		{
			return mDetails;
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
			return m_tSipInfo.crPassword.c_str();
		}
		void setUserPassword(std::string password)
		{
			m_tSipInfo.crPassword = password.c_str();
		}
	
		void setSipInfoReqDetails(SipInfoReqDetails &sipInfoReq)
		{
			m_tSipInfoReqDetails = sipInfoReq;
		}
		SipInfoReqDetails & getSipInfoReqDetails()
		{
			return m_tSipInfoReqDetails;
		}
		void setTransferTarget(std::string extn)
		{
			m_tSipInfo.transferTarget = extn.c_str();
		}
		std::string getTransferTarget()
		{
			return m_tSipInfo.transferTarget.c_str();
		}
		void setFromExtension(std::string extn)
		{
			m_tSipInfo.fromNumber = extn.c_str();
		}
		std::string getFromExtension()
		{
			return m_tSipInfo.fromNumber.c_str();
		}
		void setToExtension(std::string extn)
		{
			m_tSipInfo.toNumber = extn.c_str();
		}
		std::string getToExtension()
		{
			return m_tSipInfo.toNumber.c_str();
		}
		void setDomainName(std::string domain)
		{
			m_tSipInfo.csaCalled = domain.c_str();
		}
		std::string getDomainName()
		{
			return m_tSipInfo.csaCalled.c_str();
		}
                void setConfRoomId(std::string confId)
                {
                        m_tSipInfo.confRoomId = confId.c_str();
                }
                std::string getConfRoomId()
                {
                        return m_tSipInfo.confRoomId.c_str();
                }
                void setConfConnId(std::string connId)
                {
                        m_tSipInfo.confConnId = connId.c_str();
                }
                std::string getConfConnId()
                {
                        return m_tSipInfo.confConnId.c_str();
                }
                void setDstConnId(std::string connId)
                {
                        m_tSipInfo.dstConnId = connId.c_str();
                }
                std::string getDstConnId()
                {
                        return m_tSipInfo.dstConnId.c_str();
                }
                void setConfUserType(int userType)
                {
                        m_tSipInfo.confUserType = userType;
                }
                int getConfUserType()
                {
                        return m_tSipInfo.confUserType;
                }
                void setSipErrorEvent(SIP_ERROR_EVENT_TYPE errEvent)
                {
                        m_tenumErrEventType = errEvent;
                }

                SIP_ERROR_EVENT_TYPE getSipErrorEvent()
                {
                        return m_tenumErrEventType;
                }

                void setRedirectIp(std::string strRedirectIp)
                {
                        m_tSipInfo.redirectIp = strRedirectIp.c_str();
                }
                std::string getRedirectIp()
                {
                        return m_tSipInfo.redirectIp.c_str();
                }

};
#endif
