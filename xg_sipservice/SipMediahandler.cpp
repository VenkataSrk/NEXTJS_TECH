#include "SipMediahandler.h"
#include "../xg_reactor/xGateBaseMsg.h"

#define EN_USER_DEVICE_TYPE_ANDROID "UnifiedRing,v-1.1,URAPP,ANDROID"
#define EN_USER_DEVICE_TYPE_IOS  "UnifiedRing,v-1.1,URAPP,IOS"
#define EN_USER_DEVICE_TYPE_WEB "URWEB:WEB"

xGateSIPMsg::xGateSIPMsg(IURDefines::MODULE_ID src,IURDefines::MODULE_ID dst, SIP_EVENT_TYPE tsipEventType,SIP_MSG_TYPE tsipmsgType):xGateBaseMsg(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE)
{
	this->m_tenumSipEventType = tsipEventType;
	this->m_tenumSipMsgType = tsipmsgType;

}

xGateSIPMsg::xGateSIPMsg(const xGateSIPMsg& msg)
{
	this->m_tenumSipEventType = msg.m_tenumSipEventType;
	this->m_tenumSipMsgType = msg.m_tenumSipMsgType;
}

xGateSIPMsg::~xGateSIPMsg()
{
}

bool xGateSIPMsg::fillMediaDetails(xGateSIPMsg *sMsg, xGateCallObj* callObj)
{
	XGLOG_INFO("xGateSIPMsg::fillMediaDetails for callId(%s) uid(%s)",callObj->callid.c_str(),callObj->uid.c_str());

	sMsg->mDetails.msgType = "EN_XGATE_MSG_SIP_MODULE";
	sMsg->mDetails.callId = callObj->callid;
	sMsg->mDetails.uid = callObj->uid;

	sMsg->m_tSipInfo.call_id = callObj->callid;
	sMsg->m_tSipInfo.uid = callObj->uid;


	sMsg->m_tSipInfo.cnCalling = callObj->cp.cnCalling;
	sMsg->m_tSipInfo.cnCalled = callObj->cp.cnCalled;
	sMsg->m_tSipInfo.m_sTargetNumber = callObj->target;
	sMsg->m_tSipInfo.m_sTargetReason = callObj->cause;
	sMsg->m_tSipInfo.domainName = callObj->cp.routeAddr;
	sMsg->m_tSipInfo.confUserAgent = callObj->cp.userAgentType;
	UserDevice deviceType;
	findUserDeviceType(callObj->cp.callingDeviceType,deviceType);
	sMsg->m_tSipInfo.deviceType = deviceType;
	sMsg->m_tSipInfo.m_sdpInfoDetails[0] = callObj->sdpInfo[0];
	sMsg->m_tSipInfo.m_sdpInfoDetails[1] = callObj->sdpInfo[1];
	return true;  
}

bool xGateSIPMsg::fillRegistrationInfoDetails(xGateSIPMsg *sMsg,xGateSofiaSipRegobj *sipregobj)
{
	XGLOG_INFO("xGateSIPMsg::fillRegistrationInfoDetails for callId(%s)",sipregobj->callid.c_str());

	sMsg->m_tRegInfo.regCallId = sipregobj->callid.c_str();
	sMsg->m_tRegInfo.domainName = sipregobj->domainname.c_str();
	sMsg->m_tRegInfo.contactAddress = sipregobj->contactaddress.c_str();
	sMsg->m_tRegInfo.regUserName = sipregobj->username.c_str();
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

	sMsg->setUnKnownHeaderInfo(sipregobj->tUnKnownHeaderMap);
				
	return true;
}

bool xGateSIPMsg::fillSipInfoReqDetails(xGateSIPMsg *sMsg, xGateCallObj* callObj)
{
        XGLOG_INFO("xGateSIPMsg::filling Sip Info Request Details");
	sMsg->getSipInfoReqDetails().setDtmgDigits(callObj->cp.digitReceived);
	return true;
}

bool xGateSIPMsg::findUserDeviceType(string &sDeviceName,UserDevice &deviceType)
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
	else {
		deviceType =  EN_USER_DEVICE_DESKPHONE;
	}
	return true;
}
