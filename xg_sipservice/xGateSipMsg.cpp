#include "xGateSipMsg.h"
//#include "../xg_reactor/xGateBaseMsg.h"

#define EN_USER_DEVICE_TYPE_ANDROID "UnifiedRing,v-1.1,URAPP,ANDROID"
#define EN_USER_DEVICE_TYPE_IOS  "UnifiedRing,v-1.1,URAPP,IOS"
#define EN_USER_DEVICE_TYPE_WEB "URWEB:WEB"
#define EN_USER_DEVICE_TYPE_CCAAS_WEB "CCAAS:BROWSER"
#define EN_USER_DEVICE_TYPE_TPI "URWEB:TPI"

xGateSipMsg::xGateSipMsg(IURDefines::MODULE_ID src,IURDefines::MODULE_ID dst, SIP_EVENT_TYPE tsipEventType,SIP_MSG_TYPE tsipmsgType):xGateBaseMsg(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE)
{
	this->setSrcModuleId(src);
	this->setDstModuleId(dst);
	this->m_tenumSipEventType = tsipEventType;
	this->m_tenumSipMsgType = tsipmsgType;
}

#if 0
xGateSipMsg::xGateSipMsg(const xGateSipMsg& msg)
{
	this->m_tenumSipEventType = msg.m_tenumSipEventType;
	this->m_tenumSipMsgType = msg.m_tenumSipMsgType;
}
#endif
xGateSipMsg::~xGateSipMsg()
{
}


bool xGateSipMsg::fillRegistrationInfoDetails(xGateSipMsg *sMsg,xGateSofiaSipRegobj *sipregobj)
{
	XGLOG_INFO("xGateSipMsg::fillRegistrationInfoDetails for callId(%s) and trans_id(%s)",\
			sipregobj->callid.c_str(), sipregobj->transid.c_str());

	sMsg->m_tRegInfo.regTransId = sipregobj->transid.c_str();
	sMsg->m_tRegInfo.regCallId = sipregobj->callid.c_str();
	sMsg->m_tRegInfo.domainName = sipregobj->domainname.c_str();
	sMsg->m_tRegInfo.contactAddress = sipregobj->contactaddress.c_str();
	//sMsg->m_tRegInfo.regUserName = sipregobj->username.c_str();
	sMsg->m_tRegInfo.regUserName = sipregobj->mobileno.c_str();
	sMsg->m_tRegInfo.regUserId = sipregobj->mobileno.c_str();
	sMsg->m_tRegInfo.regDeviceType = sipregobj->devicetype.c_str();
	sMsg->m_tRegInfo.regAppType = sipregobj->apptype.c_str();
	sMsg->m_tRegInfo.regDeviceId = sipregobj->deviceid.c_str();
	sMsg->m_tRegInfo.routeIpAddress = sipregobj->ipaddress.c_str();
	sMsg->m_tRegInfo.regUserPassword = sipregobj->password.c_str();
	sMsg->m_tRegInfo.addressType = sipregobj->addresstype;
	sMsg->m_tRegInfo.aaa = sipregobj->aaa;
	sMsg->m_tRegInfo.expires = sipregobj->expries;
	sMsg->m_tRegInfo.cseq = sipregobj->cseq;
	sMsg->m_tRegInfo.proxyUserName = sipregobj->proxyusername.c_str();
	sMsg->m_tRegInfo.regTransportType = sipregobj->transporttype.c_str();
	if(!sipregobj->push_id.empty())
	 sMsg->m_tRegInfo.regPush_id = sipregobj->push_id.c_str();
	if (sipregobj->devicetype == "URDESK") // Harshith: Required changes for Pro SBC
	{
		sMsg->m_tRegInfo.contactAddress = sipregobj->domainname.c_str();
		sMsg->m_tRegInfo.proxyUserName = sipregobj->mobileno.c_str();
	}
	return true;
}

bool xGateSipMsg::fillInviteRequestDetails(xGateSipMsg *sMsg, xGateCallObj *callobj)
{
	sMsg->m_tSipInfo.uid = callobj->uid.c_str();
	sMsg->m_tSipInfo.callid = callobj->callid.c_str();
	sMsg->m_tSipInfo.m_sCallingNumber = callobj->m_sCallingNumber.c_str();
	sMsg->m_tSipInfo.m_sCalledNumber = callobj->m_sCalledNumber.c_str();
	if (!callobj->m_sDomainName.empty())
	 sMsg->m_tSipInfo.m_sDomainname = callobj->m_sDomainName.c_str();
	if (!callobj->m_sKey.empty())
	 sMsg->m_tSipInfo.m_sKey = callobj->m_sKey.c_str();
	if (!callobj->m_sCallingExt.empty())
	 sMsg->m_tSipInfo.m_sCallingExt = callobj->m_sCallingExt.c_str();

	return true;
}

bool xGateSipMsg::findUserDeviceType(string &sDeviceName,UserDevice &deviceType)
{
	if(strncmp(sDeviceName.c_str(),EN_USER_DEVICE_TYPE_IOS,sDeviceName.length()) == 0 ){
		deviceType = EN_USER_DEVICE_IOS;
	}
	else if (strncmp(sDeviceName.c_str() ,EN_USER_DEVICE_TYPE_ANDROID, sDeviceName.length()) == 0){
		deviceType =  EN_USER_DEVICE_ANDROID;
	}
	else if (strncmp(sDeviceName.c_str(),EN_USER_DEVICE_TYPE_WEB,sDeviceName.length()) == 0) {
		deviceType =  EN_USER_DEVICE_WEB;
	}
	else if (strncmp(sDeviceName.c_str(),EN_USER_DEVICE_TYPE_CCAAS_WEB,sDeviceName.length()) == 0) {
	        deviceType =  EN_USER_DEVICE_CCAAS_WEB;
	}
	else if(strncmp(sDeviceName.c_str(),EN_USER_DEVICE_TYPE_TPI,sDeviceName.length()) == 0) {
		deviceType = EN_USER_DEVICE_TPI_WEB;
	}
	else {
		deviceType =  EN_USER_DEVICE_DESKPHONE;
	}
	return true;
}
