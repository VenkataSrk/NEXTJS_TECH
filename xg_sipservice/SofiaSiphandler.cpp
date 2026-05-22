#include "xglog.h" //DebugVM
#include <sstream>
#include "SofiaSiphandler.h"
//DebugVM #include "log.h"
//DebugVM #include "klog.h"
//#include "sip.h"
//#include "siputil.h"

#include <iomanip>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <bits/stdc++.h>
#include<string>
#include <cstring>

#define THISMODULE "SOFH"
extern "C"
{
#include <sofia-sip/url.h>
#include <sofia-sip/nua.h>
#include <sofia-sip/sip.h>
#include <sofia-sip/sdp.h>
#include <sofia-sip/soa.h>
#include <sofia-sip/tport.h>
#include <sofia-sip/soa_tag.h>
#include <sofia-sip/sip_tag.h>
#include <sofia-sip/sip_header.h>
#include <sofia-sip/sip_status.h>
#include <sofia-sip/msg_addr.h>
#include <sofia-sip/su_log.h>
//#include <sofia-sip/sip_extra.h>
//#include <sofia-sip/sdp.h>
}
//DebugVM #include "sofia.h"
#include "xGateUtil.h"

#include <new>
#include "SipMediahandler.h"  //DebugVM
#include "../xg_iurmodule/IURDefines.h"  //DebugVM
//#include "../inc/IvrConfig.h"  //DebugVM
#include "../inc/UREngineConfig.h"  //DebugVM
using std::bad_alloc;

//#define XGSIP_HOST() "10.22.3.105"
//#define XGSIP_HOST() xGateUtil::GetLocalIPAddress()
#define XGSIP_HOST() xGateUtil::getLocalIpAddr()

#define XGSIPPORT "5060"
#define XGSIPSCHEME "sip:"
//#define SIP_URL "sip:*:5060" //TODO: Need to be configurable - Yadav
#define SIP_URL "sip:*:5060;transport=sctp" //TODO: Need to be configurable - Yadav
#define SIPS_URL "sips:*:5061;transport=tls"
#define CERT_PATH "\root"


//#define XGATE_USER_AGENT "Vectone MGC 1.1"
#define XGATE_USER_AGENT "Switchlab xGate"
#define XGATE_SOFIA_HEADER_USER_INFO "X-XG-UI"
#define XGATE_SOFIA_HEADER_PRIVATE_INFO "X-XG-PI"
#define DIVERSION "Diversion"
#define SETPARAM    SIPTAG_ALLOW_STR("INVITE, ACK, BYE, CANCEL, OPTIONS, MESSAGE, INFO"),\
  SIPTAG_USER_AGENT_STR(XGATE_USER_AGENT), \
NUTAG_MEDIA_ENABLE(0),\
NUTAG_AUTOANSWER(0),\
NUTAG_AUTOACK(1),\
NUTAG_AUTOALERT(0),\
NTATAG_EXTRA_100(0),\
NUTAG_ALLOW("REGISTER"),\
NUTAG_ALLOW("PRACK"),\
NUTAG_ALLOW("INVITE"),\
NUTAG_ALLOW("ACK"),\
NUTAG_ALLOW("CANCEL"),\
NUTAG_ALLOW("OPTIONS"),\
NUTAG_ALLOW("BYE"),\
NUTAG_ALLOW("REFER"),\
NUTAG_ALLOW("SUBSCRIBE"),\
NUTAG_ALLOW("PUBLISH"),\
NUTAG_ALLOW("NOTIFY"),\
NUTAG_ALLOW_EVENTS("talk"),\
NUTAG_ALLOW_EVENTS("hold"),\
NUTAG_ALLOW_EVENTS("conference"),\
NUTAG_ALLOW_EVENTS("presence"),\
NUTAG_ALLOW_EVENTS("presence.winfo"),\
NUTAG_APPL_METHOD("REGISTER"),\
TAG_END()
#define URLSCHEMA "sip"


#define	RTP_TELEPHONE_EVENT_STR	"telephone-event"
#define	RTP_PAYLOADTYPE_101	101

#define	TEMP_BUSY_FILE	"/root/wav_files/sounds/vectone/CQ_person_temp_busy.wav"
#define TEMP_NO_RESPONSE "/root/wav_files/sounds/vectone/person_not_avail_try_later.wav"
#define TEMP_DISCONNECT "/root/wav_files/sounds/vectone/No_answer.wav"
#define TEM_DEFAULT_HOLD "/root/wav_files/sounds/vectone/hold_stay_on_line.wav"
#define BEEP "/root/wav_files/sounds/vectone/Beautiful.wav"
#define DISCONNECT_DEFAULT "/root/wav_files/sounds/vectone/CR_Disconnecting.wav"

#define CONF_USER_NO_INPUT_FILE "/root/wav_files/sounds/vectone/CR_no_input.wav"
//#define USER_NO_INPUT_ENTER_PIN "/root/wav_files/sounds/vectone/CR_no_input_enter_pin.wav"
//#define USER_NO_INPUT_ENTER_PIN "/root/wav_files/sounds/vectone/CR_enter_conf_pin.wav"
#define USER_NO_INPUT_ENTER_PIN "/root/wav_files/sounds/vectone/CR_input_not_received.wav"

//DebugVM static RegistrationApi *g_multithreaded_ippbx_registrationApiObj;

//DebugVM start
XGATECODECLIST xGateCodecList[MAXCODEC]= {
{18,"G729",8000,"annexb=no",NULL,2},
{8,"PCMA",8000,NULL,NULL,2},
{111,"OPUS",48000,"maxplaybackrate=16000;maxaveragebitrate=28000;minptime=10;useinbandfec=1","/2",2},
{0,"PCMU",8000,NULL,NULL,2},
{9,"G722",8000,NULL,NULL,2},
{97,"H264",90000,NULL,NULL,3},
{98,"H265",90000,NULL,NULL,3},
{100,"VP8",90000,NULL,NULL,3},
{102,"VP9",90000,NULL,NULL,3}

};
//DebugVM end

bool StringExists(const char *p)
{
  if(p!=NULL)
    if(strlen(p))
      return true;
  return false;
}

SofiaSiphandler::SofiaSiphandler():xGateSipController(){
}
SofiaSiphandler::~SofiaSiphandler(){

}
//public Implemetation SofiaSipHandler
/* Method Name: SipLibInit
 * Method used to initilize sip handler & sip stack
 */

xGateReturn SofiaSiphandler::SipLibInit(){
  XGLOG_INFO("SofiaSiphandler::SipLibInit()");
  try{
    //Log Initiating sofia Libarary 
    SofiaInit(); // Initltilizing sofia Libary
    SofiaInitMemory(m_homeMemory); // Creating Memory for sofia Libary
    if((m_root =  SofiaInitRoot(this)) == NULL){ 
      // Log Error Capture
      return xGateReturn::FAIL;
    } // Creating Root Instance 
  } catch(...){
  XGLOG_WARN("SofiaSiphandler::SipLibInit(%s)", "Exception Occured");
    return xGateReturn::FAIL;
  }
  XGLOG_INFO("SofiaSiphandler::SipLibInit(%s)", "Success-Init");
  return xGateReturn::SUCCESS;
}

xGateReturn SofiaSiphandler::SipLibLoad(){
  XGLOG_INFO("SofiaSiphandler::SipLibLoad()"); //DebugVM
  try{

    SofiaSetLog(9); // set log level for sofia library
    if((m_nuaHandle = SofiaCreate(m_root,&SofiaSiphandler::SofiaCallBack,this)) == NULL){
      XGLOG_WARN("SofiaSiphandler::SipLibLoad(%s)", "Sofia Handle Null Return");

      return xGateReturn::FAIL;
    }
  }catch(...){
    XGLOG_WARN("SofiaSiphandler::SipLibLoad(%s)", "Exception Occured");    
    return xGateReturn::FAIL;
  }
  XGLOG_INFO("SofiaSiphandler::SipLibLoad(%s)", "Success-Loaded");
  //DebugVM
  //SendRegister();
  return xGateReturn::SUCCESS;

}

xGateReturn SofiaSiphandler::SipLibUnLoad()
{
#if 1 //DebugVM start
  try {
    if(m_nuaHandle) {
      SofiaDestoryhandle(m_nuaHandle);
      SofiaDeInitRoot(m_root);
      SofiaDeInitMemory(m_homeMemory);
      SofiaDeInit();
    }
  } catch(...) {
    //DebugVM CLog::Warning(THISMODULE, "SofiaSiphandler::SipUnLibLoad(%s)", "Exception Occured");
    return xGateReturn::FAIL;
  }
  //DebugVM CLog::Detail(THISMODULE, "SofiaSiphandler::SipUnLibLoad(%s)", "Success-UnLoad");
#endif //DebugVM end
  return xGateReturn::SUCCESS;
}

xGateReturn SofiaSiphandler::RunEventLoop()
{
  try {
    GetProcessRegisterEvent();
    GetProcessEvent();
    SofiaRootStep(m_nuaHandle,m_root);

  } catch(...) {
    XGLOG_INFO("SofiaSiphandler::RunEventLoop(%s)", "Exception Occured");
    return xGateReturn::FAIL;
  }
  return xGateReturn::SUCCESS;
}

#if 0 //DebugVM start
xGateReturn SofiaSiphandler::DialOut(xGateSofiaobj *sobj)
{
  CLog::Detail(THISMODULE, "SofiaSiphandler::DialOut(%s)CallDefault Started",sobj->uid.c_str());
  //   sobj->cp.proxyUserName.assign("302-i4fc8j9h0jh27");
  CLog::Detail(THISMODULE,"SofiaSiphandler::DialOut() sobj->cp.proxyUserName = (%s)", sobj->cp.proxyUserName.c_str());
  if(sobj->cp.isForkingenabled) {
    CallForking(sobj);
  } else {
    if(CallDefault(sobj) == FAIL) {
      CLog::Error(THISMODULE, "SofiaSiphandler::DialOut(%s)CallDefault Failed",sobj->uid.c_str());
      //RespondToPeer(EN_503_REL,sobj);
    }	
  }
  return SUCCESS;
}

#endif

xGateReturn SofiaSiphandler::PrepareHandleAndSdpMakeCall(xGateSofiaobj *sobj,callInfoParameter cp,xGateSofiaobj *dialsobj)
{
	XGLOG_INFO("SofiaSiphandler::PrepareHandleAndSdpMakeCall In(%s)",sobj->uid.c_str());
	nua_handle_t *handle = NULL;
	sdp_session_t* psdp = NULL;
	std::string sdpstr("");
	handle = CreateSofiaDialhandle(cp,(sip_t*)sobj->siptag) ;
	if(handle != NULL)
	{
		if(dialsobj){   
			dialsobj->sofianh = (void*)handle;
			dialsobj->sofiamh = (nua_hmagic_t*)sobj->sofiamh;
			SofiaBindhandle(handle,(nua_hmagic_t*)dialsobj);
			InsertCallList(dialsobj->uid,dialsobj);
		}
		else
			return FAIL;

		//    if( psdp = CreateLocalSdpParam(EN_SDP_DIALOUT,dialsobj))


		if(dialsobj && !dialsobj->cp.sdpStr.empty())
		{
			sdpstr = dialsobj->cp.sdpStr.c_str();
		}

		#if 0
		std::string ip = "10.22.3.229"; 
		int index ;
		std::string s1,s2;
		std::string t_sdpstr = sdpstr;
		while((index = t_sdpstr.find("0.0.0.0"))!=string::npos){
			s1 = t_sdpstr.substr(0, index+7); 
			s2 = t_sdpstr.substr(index+7);
			s1.replace(index,ip.length(),ip);
			t_sdpstr = s1.append(s2);
		} 
		sdpstr = t_sdpstr;   
		#endif

		//sdpstr = CreateLocalSdpParam(EN_SDP_DIALOUT,dialsobj);
		if(sdpstr.length() > 0)
		{
			//       nua_set_hparams(handle,SOATAG_USER_SDP_STR(sdpstr.c_str()),NUTAG_INVITE_TIMER(10), NUTAG_ALLOW("UPDATE"), TAG_END());
#if 0
                        nua_invite(handle,NUTAG_AUTOACK(1),SOATAG_AUDIO_AUX("cn telephone-event"),SOATAG_USER_SDP_STR(sdpstr.c_str()),NUTAG_INVITE_TIMER(10),TAG_END());
#else
                        nua_invite(handle,NUTAG_AUTOACK(1),SOATAG_AUDIO_AUX("cn telephone-event"),SIPTAG_CONTENT_TYPE_STR("application/sdp"),SIPTAG_PAYLOAD_STR(sdpstr.c_str()),NUTAG_INVITE_TIMER(10),TAG_END());
#endif

			XGLOG_INFO("SofiaSiphandler::PrepareHandleAndSdpMakeCall(%s)Call Invite Requested ", sobj->uid.c_str());
		}
		else{
			XGLOG_ERROR("SofiaSiphandler::PrepareHandleAndSdpMakeCall(%s) SDP Creation  Failed",sobj->uid.c_str());
			return FAIL;
		}
	}
	else{
		XGLOG_ERROR("SofiaSiphandler::PrepareHandleAndSdpMakeCall(%s) Handle Creation  Failed",sobj->uid.c_str() );
		if(dialsobj)
		{
		  XGLOG_INFO("SofiaSiphandler::PrepareHandleAndSdpMakeCall(%s) Deleting the callpair created for uid:(%s)",\
				sobj->uid.c_str() );
		  DeleteConnectedList(dialsobj->uid, dialsobj, EN_OT_ORIGINATION);
		}
		return FAIL;
	}

	XGLOG_INFO("SofiaSiphandler::PrepareHandleAndSdpMakeCall Out(%s)",sobj->uid.c_str());

	return SUCCESS;
}


bool SofiaSiphandler::ProcessDialoutForMakeCall(xGateSofiaobj *sobj,xGateCallObj *callObj)
{
	XGLOG_INFO("SofiaSiphandler::ProcessDialoutForMakeCall In");
	map<std::string,multiDevice*>::iterator itr = callObj->cp.m_mappedUserList.begin();
	//for (itr; itr != sobj->cp.m_mappedUserList.end(); ++itr) {
	if (itr != sobj->cp.m_mappedUserList.end()) {
		std::string key("");
		key = itr->first.c_str();
		callObj->uid = key.c_str();
		//Commented since required params in sobj->cp is overwritten by callObj->cp
		//sobj->cp = callObj->cp;
		multiDevice *mappedUser = itr->second;
		xGateSofiaobj *peerSofiaObj = new xGateSofiaobj();
		
		//peerSofiaObj->cp = sobj->cp;
		peerSofiaObj->cp = callObj->cp;
		peerSofiaObj->cp.call_id = itr->first;
		peerSofiaObj->uid = callObj->uid.c_str();
		peerSofiaObj->cp.sdpStr = callObj->cp.sdpStr.c_str();

		peerSofiaObj->cp.calledDeviceType =  mappedUser->m_userDeviceType;//m_calledDeviceType;
		//peerSofiaObj->cp.csCallAgenttype  = 0;
		#if 1
		if(strstr(peerSofiaObj->cp.calledDeviceType.c_str(),"URWEB")) {
			peerSofiaObj->cp.csCallAgenttype  = 3;
			//peerSofiaObj->cp.transportType = "transport=tcp";
			peerSofiaObj->cp.transportType = "transport=sctp";
		} else {
			peerSofiaObj->cp.csCallAgenttype  = 0;
		}
		#endif
		//peerSofiaObj->cp.transportType = "transport=tcp";
		peerSofiaObj->callstream = EN_OUTBOUND;
		//peerSofiaObj->cp.csCallingCodec = sobj->cp.csCallingCodec;

		peerSofiaObj->cp.csCalledCodec = mappedUser->m_calledcodec;
		peerSofiaObj->cp.proxyUserName = mappedUser->m_proxyUserName;
		peerSofiaObj->cp.routeIpAddress = mappedUser->m_routeIpAddress;
		peerSofiaObj->uid = callObj->uid;
		peerSofiaObj->cp.cnCalling = mappedUser->m_userNumber;
		peerSofiaObj->cp.cnCalled = mappedUser->m_proxyUserName;
		peerSofiaObj->cp.csaCalled = mappedUser->m_userSignalAddr;
		peerSofiaObj->cp.Legid = 2; // All outbound call will leg id 2 since it second call for attachement

                peerSofiaObj->cp.UrAvConferenceUserType = callObj->cp.UrAvConferenceUserType.c_str();
                peerSofiaObj->cp.UrAvConferenceRoomId = callObj->cp.UrAvConferenceRoomId.c_str();
                peerSofiaObj->cp.UrAvConferenceConnId = callObj->cp.UrAvConferenceConnId.c_str();
                peerSofiaObj->cp.UrAvConferenceDstConnId = callObj->cp.UrAvConferenceDstConnId.c_str();
                peerSofiaObj->cp.callStream = "OUTBOUND";

		#if 0
		peerSofiaObj->localSDPinfo[0] = sobj->tempSDPinfo[0];
		peerSofiaObj->localSDPinfo[1] = sobj->tempSDPinfo[1];
		peerSofiaObj->remoteSDPinfo[0].outproto =  sobj->remoteSDPinfo[0].mediamode;
		peerSofiaObj->localSDPinfo[0].avcodec = sobj->remoteSDPinfo[0].avcodec;
		peerSofiaObj->localSDPinfo[1].avcodec = sobj->remoteSDPinfo[1].avcodec;
		#endif
		InsertConnectedList(peerSofiaObj->uid,peerSofiaObj,XGATEOBJECT::EN_OT_ORIGINATION);
		peerSofiaObj->callstate = EN_CS_INIT;

		AddServerCodecToList(peerSofiaObj);

		if(PrepareHandleAndSdpMakeCall(sobj,peerSofiaObj->cp,peerSofiaObj) == FAIL){
			XGLOG_ERROR( "SofiaSiphandler::CallForking (%s) Failed",peerSofiaObj->cp.proxyUserName.c_str());
		}
	}

	XGLOG_INFO("SofiaSiphandler::ProcessDialoutForMakeCall Out (%s)",sobj->uid.c_str());
	return true;
}

#if 0
xGateReturn SofiaSiphandler::CallForking(xGateSofiaobj *sobj)
{
  CLog::Detail(THISMODULE, "SofiaSiphandler::CallForking");
  sobj->isPhase1InviteSend = true;
  map<std::string,multiDevice*>::iterator itr = sobj->cp.m_mappedUserList.begin();
  for (itr; itr != sobj->cp.m_mappedUserList.end(); ++itr) {
    multiDevice *mappedUser = itr->second;
    xGateSofiaobj *forkPeerSofiaObj = new xGateSofiaobj();
    forkPeerSofiaObj->cp = sobj->cp;
    forkPeerSofiaObj->cp.cnCalled =  sobj->cp.cnCalled;
    forkPeerSofiaObj->cp.calledDeviceType =  mappedUser->m_userDeviceType;//m_calledDeviceType;
    CLog::Detail(THISMODULE, "SofiaSiphandler::CallForking DeviceType=%s PresenceStatus=%d m_isCalledDeviceInviteSend=%d", forkPeerSofiaObj->cp.calledDeviceType.c_str(), 
                    mappedUser->m_CalledDevicePresenceStatus, mappedUser->m_isCalledDeviceInviteSend);
    if(forkPeerSofiaObj->cp.calledDeviceType == "URAPP:AND") {
      if((mappedUser->m_CalledDevicePresenceStatus == 1) && (mappedUser->m_isCalledDeviceInviteSend == false)) {
        CLog::Detail(THISMODULE, "SofiaSiphandler::CallForking for URAPP:AND");
        mappedUser->m_isCalledDeviceInviteSend = true;
        forkPeerSofiaObj->cp.csCallAgenttype  = 1;
      }
      else {
        //Dialout for URApp skipped here
        continue;
      }
    }
    else if(forkPeerSofiaObj->cp.calledDeviceType == "URAPP:IOS") {
      if((mappedUser->m_CalledDevicePresenceStatus == 1) && (mappedUser->m_isCalledDeviceInviteSend == false)) {
        CLog::Detail(THISMODULE, "SofiaSiphandler::CallForking for URAPP:IOS");
        mappedUser->m_isCalledDeviceInviteSend = true;
        forkPeerSofiaObj->cp.csCallAgenttype  = 1;
      }
      else {
        //Dialout for URApp skipped here
        continue;
      }
    } else if(strstr(forkPeerSofiaObj->cp.calledDeviceType.c_str(),"URDESK") || \
        forkPeerSofiaObj->cp.isPstnNumber) {
      forkPeerSofiaObj->cp.csCallAgenttype  = 0;
    } else if(strstr(forkPeerSofiaObj->cp.calledDeviceType.c_str(),"URWEB")) {
      forkPeerSofiaObj->cp.csCallAgenttype  = 3;
      forkPeerSofiaObj->cp.transportType = "transport=tcp";
    } else { 
      forkPeerSofiaObj->cp.csCallAgenttype  = 0;
    }
    forkPeerSofiaObj->callstream = EN_OUTBOUND;
    forkPeerSofiaObj->cp.csCallingCodec = sobj->cp.csCallingCodec;
    forkPeerSofiaObj->cp.csCalledCodec = mappedUser->m_calledcodec;
    forkPeerSofiaObj->cp.proxyUserName = mappedUser->m_proxyUserName;
    forkPeerSofiaObj->cp.routeIpAddress = mappedUser->m_routeIpAddress;
    forkPeerSofiaObj->cp.callOnHoldFilenameCalledParty = sobj->cp.callOnHoldFilenameCalledParty;
    forkPeerSofiaObj->cp.callOnHoldFilenameCallingParty = sobj->cp.callOnHoldFilenameCallingParty;
    forkPeerSofiaObj->uid = sobj->uid;
    forkPeerSofiaObj->mgid = sobj->mgid;
    forkPeerSofiaObj->cp.cnCalled = mappedUser->m_userNumber;
    forkPeerSofiaObj->cp.csaCalled = mappedUser->m_userSignalAddr;
    forkPeerSofiaObj->cp.isForkingenabled = 1; // forking is enabled in this leg
    forkPeerSofiaObj->cp.Legid = 2; // All outbound call will leg id 2 since it second call for attachement
    forkPeerSofiaObj->sofiaPeerId.erase(forkPeerSofiaObj->sofiaPeerId.begin(), forkPeerSofiaObj->sofiaPeerId.end());
    forkPeerSofiaObj->sofiaPeerId.push_back(sobj->callid);
    forkPeerSofiaObj->localSDPinfo[0] = sobj->tempSDPinfo[0];
    forkPeerSofiaObj->localSDPinfo[1] = sobj->tempSDPinfo[1];
    forkPeerSofiaObj->remoteSDPinfo[0].outproto =  sobj->remoteSDPinfo[0].mediamode; 
    forkPeerSofiaObj->localSDPinfo[0].avcodec = sobj->remoteSDPinfo[0].avcodec;
    forkPeerSofiaObj->localSDPinfo[1].avcodec = sobj->remoteSDPinfo[1].avcodec;
    forkPeerSofiaObj->callstate = EN_CS_INIT;

    char dialid[50] = "\0";
    sprintf(dialid,"%d_%s",rand(),sobj->callid.c_str());
    forkPeerSofiaObj->dialcallid =  dialid;
    AddServerCodecToList(forkPeerSofiaObj);
    //forkPeerSofiaObj->SofiaLegB = sobj;
    //sobj->SofiaForkLeg.push_back(forkPeerSofiaObj);
    if(PrepareHandleAndSdpMakeCall(sobj,forkPeerSofiaObj->cp,forkPeerSofiaObj) == FAIL){
      CLog::Error(THISMODULE, "SofiaSiphandler::CallForking (%s) Failed",forkPeerSofiaObj->cp.proxyUserName );
    }
  }
  if(sobj->cp.m_mappedUserList.size()<=0) {
    CLog::Error(THISMODULE, "CallForking Dialout Failed. No peer address available for call_id: %s !", \
        sobj->uid.c_str());
    PlayResponse(480, sobj);
    return FAIL;
  }
  return SUCCESS;
}

xGateReturn SofiaSiphandler::CallForkingApp(xGateSofiaobj *sobj, std::string proxyusername, std::string devtype)
{
  CLog::Detail(THISMODULE,"SofiaSiphandler::CallForkingApp");
  sobj->isPhase1InviteSend = true;
  map<std::string,multiDevice*>::iterator itr = sobj->cp.m_mappedUserList.begin();
  for (itr; itr != sobj->cp.m_mappedUserList.end(); ++itr) 
  {
    multiDevice *mappedUser = itr->second;
    xGateSofiaobj *forkPeerSofiaObj = new xGateSofiaobj();
    forkPeerSofiaObj->cp = sobj->cp;
    forkPeerSofiaObj->cp.cnCalled =  sobj->cp.cnCalled;
    forkPeerSofiaObj->cp.calledDeviceType =  mappedUser->m_userDeviceType;//m_calledDeviceType;
    //Check devicetype, apptype and cookie
    CLog::Detail(THISMODULE,"SofiaSiphandler::CallForkingApp DeviceType=%s PresenceStatus=%d m_isCalledDeviceInviteSend=%d", forkPeerSofiaObj->cp.calledDeviceType.c_str(), 
                    mappedUser->m_CalledDevicePresenceStatus, mappedUser->m_isCalledDeviceInviteSend);
    if((mappedUser->m_userDeviceType ==  devtype) && (mappedUser->m_CalledDevicePresenceStatus == 0) && (mappedUser->m_isCalledDeviceInviteSend == false)  &&
         (mappedUser->m_proxyUserName != proxyusername))
    {
        mappedUser->m_isCalledDeviceInviteSend = true;

        CLog::Detail(THISMODULE,"SofiaSiphandler::CallForkingApp URAPP matched found: Dialout to URApp");
        forkPeerSofiaObj->cp.csCallAgenttype  = 1;
        forkPeerSofiaObj->callstream = EN_OUTBOUND;
        forkPeerSofiaObj->cp.csCallingCodec = sobj->cp.csCallingCodec;
        forkPeerSofiaObj->cp.csCalledCodec = mappedUser->m_calledcodec;
        //forkPeerSofiaObj->cp.proxyUserName = mappedUser->m_proxyUserName;
        //Setting proxyUserName to NEW value received from MESSAGE from MGC
        forkPeerSofiaObj->cp.proxyUserName = proxyusername;
        forkPeerSofiaObj->cp.routeIpAddress = mappedUser->m_routeIpAddress;
        forkPeerSofiaObj->cp.callOnHoldFilenameCalledParty = sobj->cp.callOnHoldFilenameCalledParty;
        forkPeerSofiaObj->cp.callOnHoldFilenameCallingParty = sobj->cp.callOnHoldFilenameCallingParty;
        forkPeerSofiaObj->uid = sobj->uid;
        forkPeerSofiaObj->mgid = sobj->mgid;
        forkPeerSofiaObj->cp.cnCalled = mappedUser->m_userNumber;
        forkPeerSofiaObj->cp.csaCalled = mappedUser->m_userSignalAddr;
        forkPeerSofiaObj->cp.isForkingenabled = 1; // forking is enabled in this leg
        forkPeerSofiaObj->cp.Legid = 2; // All outbound call will leg id 2 since it second call for attachement
        forkPeerSofiaObj->sofiaPeerId.erase(forkPeerSofiaObj->sofiaPeerId.begin(), forkPeerSofiaObj->sofiaPeerId.end());
        forkPeerSofiaObj->sofiaPeerId.push_back(sobj->callid);
        forkPeerSofiaObj->localSDPinfo[0] = sobj->tempSDPinfo[0];
        forkPeerSofiaObj->localSDPinfo[1] = sobj->tempSDPinfo[1];
        forkPeerSofiaObj->remoteSDPinfo[0].outproto =  sobj->remoteSDPinfo[0].mediamode; 
        forkPeerSofiaObj->localSDPinfo[0].avcodec = sobj->remoteSDPinfo[0].avcodec;
        forkPeerSofiaObj->localSDPinfo[1].avcodec = sobj->remoteSDPinfo[1].avcodec;
        forkPeerSofiaObj->callstate = EN_CS_INIT;
        forkPeerSofiaObj->isPhase1InviteSend = true;

        char dialid[50] = "\0";
        sprintf(dialid,"%d_%s",rand(),sobj->callid.c_str());
        forkPeerSofiaObj->dialcallid =  dialid;
        AddServerCodecToList(forkPeerSofiaObj);
        //forkPeerSofiaObj->SofiaLegB = sobj;
        //sobj->SofiaForkLeg.push_back(forkPeerSofiaObj);
        if(PrepareHandleAndSdpMakeCall(sobj,forkPeerSofiaObj->cp,forkPeerSofiaObj) == FAIL){
          CLog::Error(THISMODULE, "SofiaSiphandler::CallForking (%s) Failed",forkPeerSofiaObj->cp.proxyUserName );
        }
        break;
    }
    else
    {
      return FAIL;
    }
  }
  if(sobj->cp.m_mappedUserList.size()<=0) {
    CLog::Error(THISMODULE, "CallForking Dialout Failed. No peer address available for call_id: %s !", \
        sobj->uid.c_str());
    PlayResponse(480, sobj);
    return FAIL;
  }
  return SUCCESS;
}

xGateReturn SofiaSiphandler::CallDefault(xGateSofiaobj *sobj)
{
  CLog::Detail(THISMODULE,"SofiaSiphandler::CallDefault %s", sobj->uid.c_str());
  xGateSofiaobj *sofiaPeerLeg = new xGateSofiaobj();
  sofiaPeerLeg->cp =  sobj->cp;
  sofiaPeerLeg->cp.Legid = 2;
  sofiaPeerLeg->callstream = EN_OUTBOUND;

  sofiaPeerLeg->cp.csCallingCodec = sobj->cp.csCallingCodec;

  //if(sofiaPeerLeg->cp.isPstnNumber)
  //sofiaPeerLeg->cp.csCalledCodec = 8;
  //else
  sofiaPeerLeg->cp.csCalledCodec =  sobj->cp.csCalledCodec;
  sofiaPeerLeg->cp.callOnHoldFilenameCalledParty = sobj->cp.callOnHoldFilenameCalledParty;
  sofiaPeerLeg->cp.callOnHoldFilenameCallingParty = sobj->cp.callOnHoldFilenameCallingParty;
  sofiaPeerLeg->uid = sobj->uid;
  sofiaPeerLeg->mgid = sobj->mgid; //TODO: Yoga, what is the use of this param value?
  sofiaPeerLeg->sofiaPeerId.erase(sofiaPeerLeg->sofiaPeerId.begin(), sofiaPeerLeg->sofiaPeerId.end());
  sofiaPeerLeg->sofiaPeerId.push_back(sobj->callid);
  sofiaPeerLeg->localSDPinfo[0] = sobj->tempSDPinfo[0];
  sofiaPeerLeg->localSDPinfo[1] = sobj->tempSDPinfo[1];
  sofiaPeerLeg->remoteSDPinfo[0].outproto =  sobj->remoteSDPinfo[0].mediamode;
  sofiaPeerLeg->localSDPinfo[0].avcodec = sobj->remoteSDPinfo[0].avcodec;
  sofiaPeerLeg->localSDPinfo[1].avcodec = sobj->remoteSDPinfo[1].avcodec;
  sofiaPeerLeg->callstate = EN_CS_INIT;
  sofiaPeerLeg->cp.confinfo = sobj->cp.confinfo;
  if(strstr(sobj->cp.calledDeviceType.c_str(),"URAPP")) {
    sofiaPeerLeg->cp.csCallAgenttype  = 1;
  } else if( strstr(sobj->cp.calledDeviceType.c_str(),"URDESK") || sofiaPeerLeg->cp.isPstnNumber) {
    sofiaPeerLeg->cp.csCallAgenttype  = 0;
  } else if(strstr(sofiaPeerLeg->cp.calledDeviceType.c_str(),"URWEB")) {
    sofiaPeerLeg->cp.csCallAgenttype  = 3;
    //sofiaPeerLeg->cp.transportType = "transport=tcp";
    sofiaPeerLeg->cp.transportType = "transport=sctp";
  } else {
    sofiaPeerLeg->cp.csCallAgenttype  = 0;
  }

  char dialid[50] = "\0";
  sprintf(dialid,"%d_%s",rand(),sobj->callid);
  sofiaPeerLeg->dialcallid =  dialid;
  AddServerCodecToList(sofiaPeerLeg);

  if(PrepareHandleAndSdpMakeCall(sobj,sobj->cp,sofiaPeerLeg ) == FAIL) {
    CLog::Error(THISMODULE, "SofiaSiphandler::CallDefault (%s) Failed",sobj->uid.c_str());
    return FAIL;
  }
  return SUCCESS;
}

// Private Implemntation SofiaSipHandler
/*TODO: Need to modify the paramater to set Call Paramater from
 * sip structure
 */
#endif //DebugVM end
bool SofiaSiphandler::CreateCallobjparam(xGateCallObj *callobj,xGateSofiaobj *sobj,\
    nua_handle_t *nh, sip_t const  *sip,tagi_t  tags[])
{
  //callobj =  new xGateCallObj;

  XGLOG_INFO("SofiaSiphandler::CreateCallobjparam(%s)",sobj->uid.c_str()); 
  callobj->xGateCallObjInit(); // callobj initilized

  callobj->callid = sobj->callid;
  callobj->uid = sobj->uid;

  // callobj->callid += '\0'; // added for multithreaded-ippbx

  // Create Call Pararm 
  SetcallInfoParameter(callobj->cp,sip,tags);
  callobj->cp.call_id =  callobj->callid; 
  callobj->cp.uid = sobj->uid;
  callobj->cp.sdpStr = sobj->cp.sdpStr; 
  callobj->cp.tUnKnownHeaderMap = sobj->cp.tUnKnownHeaderMap;

  if((strcmp(callobj->cp.userAgentType.c_str(),"URIOS:VIDEO") == 0)
	  || (strcmp(callobj->cp.userAgentType.c_str(),"URAND:VIDEO") == 0))
	{
	 sobj->m_AddrRecords.m_strContactaddress = sip->sip_contact->m_url->url_host;
	 sobj->m_AddrRecords.m_strContactaddress +=  ":";
	 sobj->m_AddrRecords.m_strContactaddress += sip->sip_contact->m_url->url_port;

	 sobj->m_AddrRecords.m_strIpaddress = sip->sip_contact->m_url->url_host;
	 sobj->m_AddrRecords.m_strProxyusername = sip->sip_contact->m_url->url_user;
	 sobj->m_AddrRecords.m_strDeviceType = callobj->cp.userAgentType.c_str();

	 std::string strAor("");
	 formUnknownHeaderForAor(sobj,strAor);

	 std::string key = "UR-AV-AOR";
	 callobj->cp.UrAvConfAor = strAor.c_str();
	 callobj->cp.tUnKnownHeaderMap.insert({key,callobj->cp.UrAvConfAor});
	 XGLOG_INFO("Unknown  header for userAgent:(%s) key:(%s) value:(%s) ", callobj->cp.userAgentType.c_str(), key.c_str(), callobj->cp.UrAvConfAor.c_str());
	}

  //DebugVM callobj->cp.gm_mgc_ip = xGateUtil::getMgIPOnIndex(sobj->mgid);
  //DebugVM callobj->cp.confinfo = sobj->cp.confinfo;
  // Set Transport
  if(strcasecmp(sip->sip_contact->m_url->url_scheme,"sip") == 0){
    if(ValidateSipMsgPointer(sip->sip_contact->m_url->url_params))
      nua_set_hparams(nh,
          NUTAG_M_PARAMS(sip->sip_contact->m_url->url_params),
          TAG_END());
  }

  //TODO: Yoga, added this to avoid unnecessary copying entire cp params
  //let sofia stack maintain this cp param details and supply
//DebugVMCT #ifdef XGATE_HAS_BUSINESS_SUPPORT
  sobj->cp = callobj->cp;
  //sobj->cdr.updateCDRInfo(sobj->cp);
//DebugVMCT  sobj->cdr.callDateTime = sobj->cdr.setCurrentDateTime();
  //callobj->cp.cdrJsonString =  sobj->cdr.formJsonString();
//#endif
  return true;
}

bool SofiaSiphandler::formUnknownHeaderForAor(xGateSofiaobj *sobj,std::string &aor)
{
  XGLOG_INFO("SofiaSiphandler::formUnknownHeaderForAor with Proxyusername(%s),Contactaddress(%s),Ipaddress(%s),DeviceType(%s)",\
                                        sobj->m_AddrRecords.m_strProxyusername.c_str(),sobj->m_AddrRecords.m_strContactaddress.c_str(),sobj->m_AddrRecords.m_strIpaddress.c_str(),sobj->m_AddrRecords.m_strDeviceType.c_str());
  aor += sobj->m_AddrRecords.m_strProxyusername;
  aor += "@";
  aor += sobj->m_AddrRecords.m_strContactaddress;
  aor += ";";
  aor += sobj->m_AddrRecords.m_strIpaddress;
  aor += ";";
  aor += sobj->m_AddrRecords.m_strDeviceType;
  aor += ";";
 return true;
}

xGateSofiaobj *SofiaSiphandler::CreateSofiaobjparam(std::string callid,nua_handle_t *nh,\
    nua_magic_t  *magic,sip_t *siptag)
{ 
  xGateSofiaobj *sofiaobj =  new xGateSofiaobj(); // create new Dialog
  if(!sofiaobj){
     XGLOG_ERROR("SofiaSiphandler::CreateSofiaobjparam failed to create xGateSofiaobj for '%s' !",callid.c_str());
  }  
  sofiaobj->callstream = EN_INBOUND;
  sofiaobj->callid = callid;
  //  sofiaobj->callid += '\0'; // added for multithreaded-ippbx
  char uniqid[100] = "\0";
  snprintf( uniqid, 100, "%s%s%s",siptag->sip_from->a_url->url_user,siptag->sip_to->a_url->url_user,siptag->sip_call_id->i_id);
  sofiaobj->uid = callid;
  XGLOG_INFO("SofiaSiphandler::CreateSofiaobjparam(%s)",sofiaobj->uid.c_str());
  //DebugVM sofiaobj->mgid = xGateUtil::selectMgId(); //Multi MG support 
  sofiaobj->sofianh = (void*)nh; // Transaction handle
  sofiaobj->siptag = (void*)siptag; // Sip tag struct information 
  sofiaobj->sofiamh = (void*)magic; // Application context
  if(ValidateSipMsgPointer(siptag->sip_contact->m_url->url_params)) {
    sofiaobj->siptransport = siptag->sip_contact->m_url->url_params;
    sofiaobj->cp.cnCalled = siptag->sip_contact->m_url->url_user; // added for multithreaded-ippbx
  }

  sofiaobj->P2PMedia =   0;

  if(siptag && siptag->sip_payload && siptag->sip_payload->pl_data)
  {
	  //sofiaobj->cp.sdpStr = siptag->sip_payload->pl_data;
	  std::string strPayloadSdp("");
	  copyPayloadData(siptag->sip_payload->pl_data, siptag->sip_content_length->l_length, strPayloadSdp);
	  sofiaobj->cp.sdpStr = strPayloadSdp.c_str();
	  if(sofiaobj && !sofiaobj->cp.sdpStr.empty())
	  {
		  XGLOG_INFO(THISMODULE, "sdp info value for callid (%s) is (%s)",callid.c_str(), sofiaobj->cp.sdpStr.c_str());
	  }
  }

        if(siptag && siptag->sip_unknown)
        {
                sip_unknown_t* pUnknown = siptag->sip_unknown;
                while (pUnknown)
                {
                        if (!strcmp(pUnknown->un_name, "UR-AV-ConferenceRoom-ID") && pUnknown->un_value) {
                                std::string key = pUnknown->un_name;
                                sofiaobj->cp.UrAvConferenceRoomId = pUnknown->un_value;
                                sofiaobj->cp.tUnKnownHeaderMap.insert({key,sofiaobj->cp.UrAvConferenceRoomId});
                                XGLOG_INFO(THISMODULE, "Unknown  header (%s) value is (%s)",key.c_str(), sofiaobj->cp.UrAvConferenceRoomId.c_str());
                        }
                        if (!strcmp(pUnknown->un_name,"UR-AV-Conference-UserType") && pUnknown->un_value) {
                                std::string key = pUnknown->un_name;
                                sofiaobj->cp.UrAvConferenceUserType = pUnknown->un_value;
                                sofiaobj->cp.tUnKnownHeaderMap.insert({key,sofiaobj->cp.UrAvConferenceUserType});
                                XGLOG_INFO(THISMODULE, "Unknown  header (%s) value is (%s)",key.c_str(), sofiaobj->cp.UrAvConferenceUserType.c_str());
                        }
                        if (!strcmp(pUnknown->un_name,"UR-AV-ConferenceConn-ID") && pUnknown->un_value) {
                                std::string key = pUnknown->un_name = "UR-AV-ConferenceConn-ID";
                                sofiaobj->cp.UrAvConferenceConnId = pUnknown->un_value;
                                sofiaobj->cp.tUnKnownHeaderMap.insert({key,sofiaobj->cp.UrAvConferenceConnId});
                                XGLOG_INFO(THISMODULE, "Unknown  header (%s) value UR-AV-ConferenceConn-ID is (%s)",key.c_str(), sofiaobj->cp.UrAvConferenceConnId.c_str());
                        }
                        if (!strcmp(pUnknown->un_name,"UR-AV-AOR") && pUnknown->un_value) {
                                std::string key = pUnknown->un_name = "UR-AV-AOR";
                                sofiaobj->cp.UrAvConfAor = pUnknown->un_value;
                                sofiaobj->cp.tUnKnownHeaderMap.insert({key,sofiaobj->cp.UrAvConfAor});
                                XGLOG_INFO(THISMODULE, "Unknown  header (%s) value UR-AV-AOR is (%s)",key.c_str(), sofiaobj->cp.UrAvConfAor.c_str());
                        }

                        pUnknown = pUnknown->un_next;
                }
        }


  //  InsertCallInfo(sofiaobj);
  InsertConnectedList(sofiaobj->uid,sofiaobj,XGATEOBJECT::EN_OT_ORIGINATION);
  char frmchKey[50] = "\0";
  snprintf(frmchKey, 50, "%s%s",siptag->sip_from->a_url->url_user,siptag->sip_from->a_url->url_host);
  char tochKey[50] = "\0";
  snprintf(tochKey, 50, "%s%s",siptag->sip_to->a_url->url_user,siptag->sip_to->a_url->url_host);

  printf(" Incoming call channel id %s --- %s",frmchKey , tochKey);

  SofiaBindhandle(nh,(nua_hmagic_t*)sofiaobj); 
#if 0 //DebugVM start
  sofiaobj->switchboardparam.SwbCallEvent = EN_SWB_NULL;
  sofiaobj->cp.confinfo.ConfCallEvent = EN_CONF_NULL;
  sofiaobj->cp.confinfo.conf_timer_thread_1 = 0;
  sofiaobj->cp.confinfo.conf_timer_thread_2 = 0;
#endif //DebugVM end
  return sofiaobj;
  //	sofiaobj->SofiaLegB->switchboardparam.SwbCallEvent = EN_SWB_NULL;
}

unsigned char hexToChar(const std::string &str) {
  short c = 0;

  if(!str.empty()) {
    std::istringstream in(str);

    in >> std::hex >> c;

    if(in.fail()) {
      //throw std::runtime_error("stream decode failure");
    }
  }
  return static_cast<unsigned char>(c);
}

std::string SipHeaderDecode(const std::string &toDecode) {
  std::ostringstream out;

  for(std::string::size_type i=0; i < toDecode.length(); ++i) {
    if(toDecode.at(i) == '%') {
      std::string str(toDecode.substr(i+1, 2));
      out << hexToChar(str);
      i += 2;
    } else {
      out << toDecode.at(i);
    }
  }

  return out.str();
}

std::string SipHeaderEncode(const std::string &value) {
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
    std::string::value_type c = (*i);

    // Keep alphanumeric and other accepted characters intact
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      escaped << c;
      continue;
    }

    // Any other characters are percent-encoded
    escaped << '%' << std::setw(2) << int((unsigned char) c);
  }
  return escaped.str();
}

bool SofiaSiphandler::SetcallInfoParameter(callInfoParameter &cp, sip_t const *sip, tagi_t tags[])
{
  // TODO:  Restructure the below code for clarity & best practices
  /* Here need to fill
     1. csaCalled
     2. cnCalled
     3. cntCalled
     4. cnpcalled

     1. csaCalling
     2. cnCalling
     3. cntCalling
     4. cnpcalling
     */

  char _temp[256];
  //Updating called paramater

  //TODO: Yoga, done for New-MGC PBX integration

  if(ValidateSipMsgPointer(sip->sip_from->a_url->url_host)) {
    cp.routeAddr = sip->sip_from->a_url->url_host;
  }
  cp.csaCalled = "";
  cp.csaCalled += sip->sip_request->rq_url->url_host;
  //cp.csaCalled += ':';
  // cp.csaCalled += XGSIPPORT;

  cp.cnCalled = sip->sip_to->a_url->url_user;

  cp.csCallAgenttype = 0;
  if(sip->sip_user_agent) { 
    if(sip->sip_user_agent->g_string) {
      cp.callingDeviceType = sip->sip_user_agent->g_string; // Added to support Peer to Peer check in PBX Yadav
      if(strstr(sip->sip_user_agent->g_string,"URAPP")) {
        cp.csCallingContact = sip->sip_contact->m_url->url_host;
        cp.csCallAgenttype = 1; //App calli;
        cp.csCallingCodec = 111;
      } else if(strstr(sip->sip_user_agent->g_string,"URWEB")) {
        cp.csCallAgenttype = 3;
        cp.csCallingCodec = 111;
      }else {
        cp.csCallingCodec = 8;
      }
    }
  }

  if(sip && sip->sip_user_agent)
	{
	  std::string strUserAgentType = sip->sip_user_agent->g_string;
		if(strstr(sip->sip_user_agent->g_string,"IOS")) {
		  cp.userAgentType = "URIOS:VIDEO";
		} else if (strstr(sip->sip_user_agent->g_string,"AND")) {
		      cp.userAgentType = "URAND:VIDEO";
		}
	}


  // check paid service for calling party
  // TODO Need to check required or any  hard code value need
  sip_p_asserted_identity_t * Paidurl=sip_p_asserted_identity(sip);

  if(Paidurl && Paidurl->paid_url) {
    cp.csaCalling += Paidurl->paid_url->url_host;
    if(Paidurl->paid_url->url_port) {
      cp.csaCalling += ":";
      cp.csaCalling += Paidurl->paid_url->url_port;
    }   
    cp.cnCalling = Paidurl->paid_url->url_user;
  } else {
    cp.csaCalling += sip->sip_from->a_url->url_host; //TODO: We have enable this for live call
    if(sip->sip_from->a_url->url_port) {
      cp.csaCalling += ":";
      cp.csaCalling += sip->sip_from->a_url->url_port;
    }
    cp.cnCalling = sip->sip_from->a_url->url_user;
  }

#if 0 //TODO: Not REquired
  cp.cui="";
  cp.cpinfo="";
  sip_unknown_t* pUnknown=sip->sip_unknown;
  while(pUnknown) {
    if(!strcmp(pUnknown->un_name, XGATE_SOFIA_HEADER_USER_INFO)) {
      cp.cui=SipHeaderDecode(pUnknown->un_value);
    }
    if(!strcmp(pUnknown->un_name, XGATE_SOFIA_HEADER_PRIVATE_INFO)) {
      cp.cpinfo=SipHeaderDecode(pUnknown->un_value);
    }
    pUnknown=pUnknown->un_next;
  }
#endif

  if(cp.Legid == -1) { // This check done for outbound call on forking maded as inbound on ios.
    cp.Legid = 1;
  }
  return true;;
}
#if 0//DebugVM start


/**/
//Yadav, we are using this funciton for testing
void SofiaSiphandler::CreateSDPParam(xGateSofiaobj *sobj){
  bool is_hold=true;
#define sdp ((sdp_session_s*)sobj->rsdp)
  //#define nhandle ((nua_handle_t*)sobj->SofiaLegB->sofianh)
  // if(sdp)
  //   if(sdp)
  //     if(sdp->sdp_media)
  //       if(sdp->sdp_media->m_port)
  //         if(sdp->sdp_media->m_mode>2)
  //           is_hold=false;

  //   nua_set_hparams(nhandle, 
  //       SOATAG_AUDIO_AUX("telephone-event"),
  //       SOATAG_USER_SDP(sdp), 
  //       TAG_END());
  //   if(!is_hold)
  /*   nua_set_hparams(nhandle, 
       SOATAG_AUDIO_AUX("telephone-event"),
       SOATAG_HOLD(""),// force sofia to put "sendrecv"
       SOATAG_USER_SDP_STR("m=audio 8000 RTP/AVP 8 0\n"
       "c=IN IP4 10.22.3.92\n"),
       SOATAG_ACTIVE_AUDIO(SOA_ACTIVE_SENDRECV),// force sofia to put "sendrecv"
       SOATAG_ACTIVE_VIDEO(SOA_ACTIVE_SENDRECV),// force sofia to put "sendrecv"
       TAG_END());*/
}

std::string SofiaSiphandler::EncodeDialSDP(xGateSofiaobj *sobj)
{
  std::stringstream sdpsession;
  sdpsession << "v=0\r\n";
  //sdpsession << "o=SIP " <<" "<< (rand()&0xFFFFFFFF) << " "<< (rand()&0xFFFFFFFF) << " IN IPV4 " << XGSIP_HOST() << "\r\n";
  sdpsession << "s=SDP session\r\n";
  sdpsession << "t=0 0 \r\n";
  if(sobj->cp.isPstnNumber) {
    sobj->localSDPinfo[0].ptime = 20;
  }
  if(sobj->cp.csCalledCodec == 8 || sobj->cp.csCalledCodec == 18 || sobj->cp.csCalledCodec == 0) {
    sdpsession << MultiCodecAVP(sobj,EN_SDP_DIALOUT);
  } else {
    sdpsession << MultiCodecSAVPF(sobj,EN_SDP_DIALOUT);
  }

  //if (sobj->cp.csCalledCodec == 8)
  //sdpsession << CreateG711Codec(sobj->localSDPinfo,sobj,EN_SDP_DIALOUT);
  //else 
  //sdpsession << CreateOpusCodec(sobj->localSDPinfo,sobj,EN_SDP_DIALOUT);
  return sdpsession.str();
}

#endif //DebugVM end
std::string SofiaSiphandler::EncodeResponseSDP(xGateSofiaobj *sobj)
{
  std::stringstream sdpsession;
  //sdpsession << "s=SDP session\r\n";
  sdpsession << "v=0\r\n";
  //sdpsession << "o=SIP " <<" "<< "397431008138638888" << " "<< (rand()&0xFFFFFFFF) << " IN IPV4 " << XGSIP_HOST() << "\r\n";
  sdpsession << "s=SDP session\r\n";
  sdpsession << "t=0 0 \r\n";
  sdp_session_t *rsdp = (sdp_session_t*) sobj->rsdp;
  if(rsdp) {
    switch(rsdp->sdp_media->m_proto) {
      case sdp_proto_x:// DTLS
        {
          //DebugVM sdpsession << MultiCodecSAVPF(sobj,EN_SDP_RESPONSE);
          break;
        }
      case sdp_proto_srtp: //SRTP
        {
          //DebugVM sdpsession << MultiCodecSAVPF(sobj,EN_SDP_RESPONSE);
          break;
        }
      case sdp_proto_rtp: // AVP
        {
          sdpsession << MultiCodecAVP(sobj,EN_SDP_RESPONSE);
          break;
        }
      default:
        {
          break;
        }
    }
  }
  return sdpsession.str();
}
#if 0//DebugVM start

std::string SofiaSiphandler::MultiCodecSAVPF(xGateSofiaobj *sobj,XGATESDPMODE sdpmode)
{
  std::stringstream SDPStr;
  SDPStr << EncodeSessionAttribute(sobj->localSDPinfo[0], sobj->localSDPinfo[1],sobj->cp.csCallAgenttype);
  SDPStr << EncodeConnectionInformation(sobj->localSDPinfo[0],sdpmode,sobj->cp.csCallAgenttype);
  //SDPStr<<"c=IN IP4 "<<ipaddress<<"\r\n";
  SDPStr<<"a=msid-semantic:WMS " <<sobj->localSDPinfo[0].mslabel<<"\r\n";
  // MLine Attribute
  if(sobj->localSDPinfo[0].mediatype == sdp_media_audio) {
    SDPStr << EncodeMediaAttribute(sobj->localSDPinfo[0],sdpmode,sobj->cp.csCallAgenttype); //audio codec
  }
  if(sobj->localSDPinfo[1].mediatype == sdp_media_video) {
    SDPStr << EncodeMediaAttribute(sobj->localSDPinfo[1],sdpmode,sobj->cp.csCallAgenttype); //video codec
  }
  return SDPStr.str();
}

#endif //DebugVM end
std::string SofiaSiphandler::MultiCodecAVP(xGateSofiaobj *sobj,XGATESDPMODE sdpmode){
  std::stringstream SDPStr;
  std::vector<xGateAVCodec*> tempAvCodec;

  if((sdpmode == EN_SDP_RESPONSE) && (sobj->mediastate == EN_MED_SUSPENDED || sobj->mediastate == EN_MED_RESUMED ))
  {
    tempAvCodec = sobj->localSDPinfo[0].avcodec;
    sobj->localSDPinfo[0].avcodec = sobj->remoteSDPinfo[0].avcodec;
  }

  SDPStr << EncodeConnectionInformation(sobj->localSDPinfo[0],sdpmode,sobj->cp.csCallAgenttype);
  if(sobj->localSDPinfo[0].mediatype == sdp_media_audio )
    SDPStr << EncodeMediaAttribute(sobj->localSDPinfo[0],sdpmode,sobj->cp.csCallAgenttype); //audio codec
  
  if((sdpmode == EN_SDP_RESPONSE) && (sobj->mediastate == EN_MED_SUSPENDED || sobj->mediastate == EN_MED_RESUMED ))
  {
    sobj->localSDPinfo[0].avcodec = tempAvCodec;
  }
  if(sobj->localSDPinfo[1].mediatype == sdp_media_video )
    SDPStr << EncodeMediaAttribute(sobj->localSDPinfo[1],sdpmode,sobj->cp.csCallAgenttype); //video codec

  //printf("\n AVP- SDP %s", SDPStr.str().c_str());
  return SDPStr.str();
}

std::string SofiaSiphandler::EncodeConnectionInformation(xGateSDPInfo sdpinfo,XGATESDPMODE sdpmode,int DeviceType){
  std::stringstream SDPStr;
  SDPStr <<"c=IN IP4 ";
  (sdpmode == EN_SDP_DIALOUT) ? SDPStr << sdpinfo.mediadialip : SDPStr << sdpinfo.mediaresponseip;
  SDPStr << "\r\n";
  return SDPStr.str();
}
#if 0//DebugVM start
std::string SofiaSiphandler::EncodeSessionAttribute(xGateSDPInfo audio, xGateSDPInfo video,int DeviceType){
  std::stringstream SDPStr;

  //unified plan 
  SDPStr <<"a=group:BUNDLE" << " ";
  (audio.mediatype == sdp_media_audio)  ? SDPStr << "0" : SDPStr << "";
  (video.mediatype == sdp_media_video ) ? SDPStr << " 1" : SDPStr << "";
  SDPStr <<"\r\n";
  return SDPStr.str();
}
#endif //DebugVM end
std::string SofiaSiphandler::EncodeMediaAttribute(xGateSDPInfo sdpinfo, XGATESDPMODE sdpmode,int DeviceType){
  std::stringstream SDPStr;
  std::string ipaddress;
  int port;
  if(sdpmode == EN_SDP_DIALOUT){
    ipaddress = sdpinfo.mediadialip;
    port = sdpinfo.mediadialport;
  }
  else
  {
    ipaddress = sdpinfo.mediaresponseip;
    port = sdpinfo.mediaresponseport;
  }
  SDPStr <<"m=";
  if (sdpinfo.mediatype == sdp_media_audio) SDPStr << "audio ";
  else if(sdpinfo.mediatype == sdp_media_video)  SDPStr << "video " ;  
  else SDPStr << "av";
  SDPStr <<port << " ";
  //SDPStr << (sdpmode == EN_SDP_DIALOUT) ? sdpinfo.mediadialport : sdpinfo.mediaresponseport << " ";
  //SDPStr << (strcmp(sdpinfo.mediamode ,"DTLS") == 0) ? "UDP/TLS/RTP/SAVPF" : "RTP/AVP" <<" " ;
  //SDPStr<<"a=msid-semantic:WMS " <<sdpinfo.mslabel<<"\r\n";
  SDPStr <<((DeviceType ==  0) ? "RTP/AVP" : "UDP/TLS/RTP/SAVPF")  << " " ;

  std::vector<xGateAVCodec*>::iterator it ;

	for (it = sdpinfo.avcodec.begin(); it != sdpinfo.avcodec.end(); ++it){
		if((sdpmode == EN_SDP_RESPONSE) && (8 != (*it)->payloadtype) )
			continue;
		
		SDPStr << (*it)->payloadtype  << " ";
	}
	
	
	(sdpinfo.mediatype == sdp_media_audio) ? SDPStr << "101\r\n" : SDPStr << "\r\n";

	for (it = sdpinfo.avcodec.begin(); it != sdpinfo.avcodec.end(); ++it){
		
		//int index = GetCodecFromList((*it)->payloadtype);
		if((sdpmode == EN_SDP_RESPONSE) && (8 != (*it)->payloadtype) ){
			continue;
		}
		SDPStr <<"a=rtpmap:" << (*it)->payloadtype << " ";
		int index = GetCodecFromList((*it)->name.c_str(),sdpinfo.mediatype);
		SDPStr << xGateCodecList[index].codecname <<"/" << xGateCodecList[index].samplingrate ;
		
		if(xGateCodecList[index].mode)
			SDPStr << xGateCodecList[index].mode <<"\r\n";
		else
			SDPStr <<"\r\n";
	
		if(xGateCodecList[index].fmtp)
		SDPStr << "a=fmtp:"<< xGateCodecList[index].payload <<" " << xGateCodecList[index].fmtp <<"\r\n";
		
	}
  if(sdpinfo.mediatype == sdp_media_audio)
  {
    SDPStr <<"a=rtpmap:"<<"101 " << "telephone-event/8000\r\n";
    SDPStr <<"a=fmtp:"<<"101 0-15\r\n";
  }
  if(DeviceType == 0){
    if(sdpinfo.mediatype == sdp_media_audio)
      if(sdpinfo.ptime > 0)
        SDPStr <<"a=ptime:"<<sdpinfo.ptime<<"\r\n";
      else
        SDPStr <<"a=ptime:20\r\n";
  }
  else{
    SDPStr <<"a=setup:" <<((sdpmode==EN_SDP_DIALOUT ) ? "actpass" : "passive") <<"\r\n";
    SDPStr <<"a=maxptime:"<<"60\r\n";
    SDPStr <<"a=candidate:1 1 udp "<<(rand()&0xFFFFFFFF)<<" "<<ipaddress<<" " <<port<<" typ relay"<<"\r\n";
    SDPStr <<"a=ice-ufrag:"<<sdpinfo.iceufrag<<"\r\n";
    SDPStr <<"a=ice-pwd:"<<sdpinfo.icepwd<<"\r\n";
    SDPStr <<"a=ice-option:trickle"<<"\r\n";
    SDPStr <<"a=fingerprint:sha-256 " << sdpinfo.fingerprint <<"\r\n";
    SDPStr<<"a=rtcp-mux\r\n";
    SDPStr<<"a=msid:"<<sdpinfo.mslabel<<" "<<sdpinfo.label<<"\r\n";
    SDPStr <<"a=extmap:"<<"1 urn:ietf:params:rtp-hdrext:ssrc-audio-level vad=off\r\n";
    SDPStr <<"a=extmap:"<<"2 urn:ietf:params:rtp-hdrext:sdes:mid\r\n";
    //if(DeviceType == 1)
    //SDPStr<<"a=mid:" << ((sdpinfo.mediatype == sdp_media_audio) ? "audio" : "video") <<"\r\n";
    //else
    SDPStr<<"a=mid:" << ((sdpinfo.mediatype == sdp_media_audio) ? "0" : "1") <<"\r\n";

    SDPStr <<"a=ssrc:"<<sdpinfo.ssrc<<" cname:"<<sdpinfo.cname<<"\r\n";
    SDPStr <<"a=ssrc:"<<sdpinfo.ssrc<<" msid:"<<sdpinfo.mslabel<<" "<<sdpinfo.label<<"\r\n";
    SDPStr <<"a=ssrc:"<<sdpinfo.ssrc<<" mslable:"<<sdpinfo.mslabel<<"\r\n";
    SDPStr <<"a=ssrc:"<<sdpinfo.ssrc<<" lable:"<<sdpinfo.label<<"\r\n";
    SDPStr <<"a="<<sdpinfo.sendreceive<<"\r\n";

  }

  return SDPStr.str();
}

std::string SofiaSiphandler::CreateLocalSdpParam(XGATESDPMODE sdpMode,xGateSofiaobj *sobj) 
{
  //sdp_session_t *sdp = sdp_session_dup(&m_homeMemory,(sdp_session_t*)sobj->rsdp);
  std::string encodeSdp = "";
  // Check the Port 
  switch(sdpMode) {
#if 0 //DebugVM start
    case EN_SDP_DIALOUT:
      {
        if (sobj->localSDPinfo[0].mediadialport > 0 ) {
          encodeSdp =   EncodeDialSDP(sobj);
          printf("SDP Createed %s", encodeSdp.c_str());
          CLog::Detail(THISMODULE, "Created Dial_Out sdb: [%s]", encodeSdp.c_str());
        } else {
          CLog::Detail(THISMODULE, "Created Dial_Out sdd failed !");
        }
        break;
      }
#endif //DebugVM end
    case EN_SDP_RESPONSE:
      {
        if (sobj->localSDPinfo[0].mediaresponseport > 0)
          encodeSdp = EncodeResponseSDP(sobj);
        break;
      }
    case EN_SDP_PEERTOPEER:
      {
        //  return (sdp_session_t*)sobj->rsdp;
        break;
      }
    default:
      return "";
  }

  return encodeSdp;
  /*sdp_parser_t* sdpparser = sdp_parse(&m_homeMemory,encodeSdp.c_str(),sizeof(encodeSdp.c_str()),sdp_f_strict);
    if (sdpparser)
    return sdp_session(sdpparser);
    else 
    return NULL;*/
}
#if 0//DebugVM start


/* Used to initialize run-time environment including sockets for Sofia Library 
 * Input Param : Void
 */

#endif //DebugVM end
void SofiaSiphandler::SofiaInit(){
  XGLOG_INFO("SofiaSiphandler::SofiaInit()");
  su_init();
}

/* Used to De - initialize run-time environment including sockets for Sofia Library 
 * Input Param : Void
 */
void SofiaSiphandler::SofiaDeInit(){
  su_deinit();
}

/* Used to initialize applicaton memory for Sofia Library,
 * This helps to start mulitple sofia library using differnt memeory 
 * Input Param : su_home_t structure address
 */
void SofiaSiphandler::SofiaInitMemory(su_home_t &homeMemory ){
  su_home_init(&homeMemory);
}

/* Used to deinitialize applicaton memory for Sofia Library,
 *  Input Param : su_home_t structure address
 */
void SofiaSiphandler::SofiaDeInitMemory(su_home_t &homeMemory){
  su_home_deinit(&homeMemory);
}

/* Used to create root instance
*/
su_root_t *SofiaSiphandler::SofiaInitRoot(su_root_magic_t* appInitance){
  return su_root_create(appInitance);
}

/* Used to destroy root instance
*/
void SofiaSiphandler::SofiaDeInitRoot(su_root_t* nuaRoot){
  su_root_destroy(nuaRoot);
}
/**/
nua_t* SofiaSiphandler::SofiaCreate(su_root_t *root,nua_callback_f	callback, nua_magic_t *magic){

  /* @NEW_1_12_7. In order to use @b Alert-Info header, initialize the SIP
   * parser before calling nta_agent_create() or nua_create() with, e.g.,
   * sip_update_default_mclass(sip_extend_mclass(NULL)). Sofia -Sip Lib
   */
  sip_update_default_mclass(sip_extend_mclass(NULL));

  m_nuaHandle=  nua_create(root,callback,magic,\
      NUTAG_URL(SIP_URL),\
      // NUTAG_SIPS_URL(SIPS_URL)
      NUTAG_CERTIFICATE_DIR(CERT_PATH),\
      TAG_END());

  SofiaSetParam(m_nuaHandle);
  return m_nuaHandle;

}
/*
 * Create auth mod for authendicating client
 * Still need to chagen the paramater for auth registration
 * */

auth_mod_t* SofiaSiphandler::SofiaInitAuth(su_root_t* root){
  return auth_mod_create(root,AUTHTAG_METHOD("Digest"),AUTHTAG_ALGORITHM("MD5"),AUTHTAG_QOP("auth"),AUTHTAG_REALM("xgatesipreg.vectone.com"),TAG_END());
}
void SofiaSiphandler::SofiaDeInitAuth(auth_mod_t *authmod){
  if(authmod)
    auth_mod_destroy(authmod);
}

void SofiaSiphandler::SofiaSetParam(nua_t* nuaHandle){
  nua_set_params(nuaHandle,SETPARAM);
}

void SofiaSiphandler::SofiaShutdown(nua_t *nuaHandle){
  nua_shutdown(nuaHandle);
}

void SofiaSiphandler::SofiaSetLog(int level){
  //su_log_t* sulog =  new su_log_t;
  su_log_set_level(su_log_global,level); //windows call
}
/**/
void SofiaSiphandler::SofiaDestoryhandle(nua_t* nuaHandle){
  nua_destroy(nuaHandle);

}
/**/
void SofiaSiphandler::SofiaRespond(nua_handle_t *nh,int status, char const *phrase){
  nua_respond(nh,status,phrase,TAG_END());
} 
/**/
void SofiaSiphandler::SofiaRespond(nua_handle_t *nh,int status, char const *phrase,tagi_t tags[]){
  //nua_respond(nh,status,phrase,tags);
} 
/**/
void SofiaSiphandler::SofiaSendAck(nua_handle_t *nh){
  nua_ack(nh, TAG_END());

}
#if 0  //DebugVM start
/**/
void SofiaSiphandler::SofiaSendInvite(nua_handle_t *nh){
  nua_invite(nh,TAG_END());
}
/**/
#endif //DebugVM end
void SofiaSiphandler::SofiaBindhandle(nua_handle_t *nh,nua_hmagic_t* sofiaobjhandle){
  nua_handle_bind(nh,sofiaobjhandle);
}
#if 0  //DebugVM start
/**/
void SofiaSiphandler::SofiaReBindhandle(nua_handle_t *Old_nh,nua_handle_t *New_nh,nua_hmagic_t* sofiaobjhandle){
  Old_nh = NULL;
  //SofiaBindhandle(NULL,NULL);
  SofiaBindhandle(New_nh,sofiaobjhandle);
  Old_nh = New_nh;

}
#endif //DebugVM end

url_t* SofiaSiphandler::SofiaCreateMake(su_home_t &su_home,const char* dialstr){

  SofiaInitMemory(su_home);
  return url_make(&su_home,dialstr);

}

//Yoga
bool SofiaSiphandler::UpdateSofiaobjParam(xGateSDPInfo &sdpinfo, xGateCallObj *callObj,int avType){
  XGLOG_INFO("SofiaSiphandler::UpdateSofiaobjParam(%s)",callObj->uid.c_str());
  if(avType == 2) // audio
    sdpinfo  = callObj->sdpInfo[0];
  else
    sdpinfo  = callObj->sdpInfo[1];

  //memcpy((*sobj)->remoteSDPinfo, &callObj->sdpInfo);  
#if 0
  if((*sobj)->mediastate == EN_MED_REQ){
    (*sobj)->mediastate = EN_MED_SET;
    (*sobj)->ipaddr = event->ipaddr;
    (*sobj)->port = event->port;
    (*sobj)->codec = event->codec;
    (*sobj)->remoteSDPinfo.iceufrag = event->ice_ufrag;
    (*sobj)->remoteSDPinfo.icepwd = event->ice_pwd;
    (*sobj)->remoteSDPinfo.fingerprint = event->fingerprint;
    (*sobj)->remoteSDPinfo.ssrc = event->ssrc;
    (*sobj)->remoteSDPinfo.cname = event->cname;
    (*sobj)->remoteSDPinfo.mslabel = event->mslabel;
    (*sobj)->remoteSDPinfo.label = event->label;
    (*sobj)->remoteSDPinfo.mediamode = event->media_proto;
    (*sobj)->remoteSDPinfo.ptime =  event->ptime;
    if(event->DialPort > 0)
      (*sobj)->DialPort =  event->DialPort; 
    (*sobj)->dialipaddr =  event->dialipaddr;
    (*sobj)->respipaddr =  event->respipaddr;
    if(event->ResponsePort > 0)
      (*sobj)->ResponsePort =  event->ResponsePort;
    return true;
  }
  return false;
#endif
  return true;
}

void SofiaSiphandler::OnProcessEvent(xGateSofiaobj *sobj, xGateCallObj *callObj)
{
  //CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent(%s)",sobj->uid.c_str());
  switch(callObj->sipeventtype) {
#if 0 //DebugVM start
    case EN_SETMEDIA_TREL: // Response on A Incoming Call Media Request
      {
        CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent EN_SETMEDIA_TREL ",sobj->uid.c_str());
        UpdateSofiaobjParam(sobj->tempSDPinfo[0],callObj,2); // Copy to A's object since carry only peer detail
        UpdateSofiaobjParam(sobj->tempSDPinfo[1],callObj,3);
        //sobj->cp.PbState = EN_XGATE_PB_PLAY;
        RequestForMedia(sobj, EN_MEDIA_18X);
        break;
      }
    case EN_SETMEDIA_OREL:
      {
        CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent EN_SETMEDIA_OREL ",sobj->uid.c_str());
        //	UpdateSofiaobjParam(sobj->localSDPinfo[0],callObj,2); // Copy to A's local sdp infor since it carry original sdp info
        //	UpdateSofiaobjParam(sobj->localSDPinfo[1],callObj,3); 

        //	sobj->mediastate =  EN_MED_SET;
        break;
      }
#endif //DebugVM end
    case EN_SETMEDIA_EREL:
      {
        XGLOG_INFO("SofiaSiphandler::OnProcessEvent EN_SETMEDIA_EREL ",sobj->uid.c_str());
        UpdateSofiaobjParam(sobj->localSDPinfo[0],callObj,2); // Copy to A's local sdp infor since it carry original sdp info
       // UpdateSofiaobjParam(sobj->localSDPinfo[1],callObj,3);
        // OnB2B Call connected
        sobj->localSDPinfo[0].avcodec = sobj->remoteSDPinfo[0].avcodec;
       // sobj->localSDPinfo[1].avcodec = sobj->remoteSDPinfo[1].avcodec;
       // RespondToPeer(EN_183_REL, sobj);
       // RespondToPeer(EN_180_REL, sobj); 
        sobj->cp.sdpStr = callObj->cp.sdpStr.c_str();
        sobj->cp.routeIpAddress = callObj->cp.routeIpAddress.c_str();
        sobj->cp.csCallAgenttype = 0;

	//Fix for 200 OK immediately sent by stack for REINVITE
        nua_set_hparams((nua_handle_t*)sobj->sofianh,SOATAG_AUDIO_AUX("telephone-event"),SOATAG_USER_SDP_STR(sobj->cp.sdpStr.c_str()),
                        NUTAG_EARLY_MEDIA (1),
                        NUTAG_EARLY_ANSWER(1),
                        NUTAG_AUTOANSWER(0),
                        TAG_NULL());
        sobj->callstate = EN_CS_ALERTING;
        SofiaRespond((nua_handle_t*)sobj->sofianh,EN_180_REL,"Ringing");

        RespondToPeer(EN_200_REL, sobj);
        sobj->mediastate = EN_MED_CONNECTED;
        break;
      } //DebugVM
    case EN_REDIRECT_REQUEST_EVENT_REL : //Added for redirecting conf calls to same conf bridge with 302 request
      {
        XGLOG_INFO("SofiaSiphandler::OnProcessEvent EN_REDIRECT_REQUEST_EVENT_REL to redirectIp: %s for uid: %s",\
                        callObj->cp.redirectIp.c_str(),sobj->uid.c_str());
        sobj->cp.redirectIp =  callObj->cp.redirectIp.c_str();
        RespondToPeer(EN_302_REL, sobj);
        break;
      }
    case EN_REINVITE_REL:
      {
        XGLOG_INFO("SofiaSiphandler::OnProcessEvent EN_REINVITE_REL ",sobj->uid.c_str());
        UpdateSofiaobjParam(sobj->localSDPinfo[0],callObj,2);
        sobj->localSDPinfo[0].avcodec = sobj->remoteSDPinfo[0].avcodec;
        sobj->cp.sdpStr = callObj->cp.sdpStr.c_str();
        sobj->cp.csCallAgenttype = 0;
        RespondToPeer(EN_200_REL, sobj);
        break;
      }
    case EN_SDP_UPDATE_REL:
      {
        XGLOG_INFO("SofiaSiphandler::OnProcessEvent EN_SDP_UPDATE_REL ",sobj->uid.c_str());
        UpdateSofiaobjParam(sobj->localSDPinfo[0],callObj,2);
        sobj->localSDPinfo[0].avcodec = sobj->remoteSDPinfo[0].avcodec;
        sobj->cp.sdpStr = callObj->cp.sdpStr.c_str();
        sobj->cp.csCallAgenttype = 0;
        SendReInviteForConfUser(sobj);
        break;
      }
    case EN_TRANSFER_REQ:
      {
        XGLOG_INFO("SofiaSiphandler::OnProcessEvent EN_TRANSFER_REQ",sobj->uid.c_str());
       
        //SendReInvite(sobj);
        SendRefer(sobj, callObj);

        break;
      } //DebugVM
#if 0 //DebugVM start
        if(sobj->cp.isSwitchBoardCall) {
          sobj->switchboardparam.SwbCallEvent = EN_SWB_PLAY_PASSTHROUGH;
          //	PlayOnConnect(sobj,EN_FILE_IVR);

          //sobj->switchboardparam.PlayFile.assign("");
          //sobj->switchboardparam.MaxDigit = 3;
          //sobj->switchboardparam.DigitTimeOut = 8;
          //sobj->switchboardparam.DigitReceived.assign("");

          // sobj->switchboardparam.SwbCallEvent = EN_SWB_GETDTMF;
          OnSwitchBoardCallProcess(sobj);
        } else if(sobj->cp.isconferenceCall) {
#if 0 //TODO: Yoga disabled this part 
          sobj->cp.confinfo.ConfDtmfModule = EN_CONF_GETCONFID;
          sobj->cp.confinfo.ConfDtmfState = EN_CONF_WELCOMECONNECT;
#endif
          sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY_PASSTHROUGH_GETDTMF;
#ifdef GET_PIN_ONLY
          //sobj->cp.confinfo.MaxDigit = 4; // PIN is 4-digit
          sobj->cp.confinfo.MaxDigit = 8; // PIN is 8-digit
          sobj->cp.confinfo.DigitTimeOut = 22; // 16 is equivalent to 12 seconds
#else
          sobj->cp.confinfo.MaxDigit = 5; // Conf-id is 5-digit
          sobj->cp.confinfo.DigitTimeOut = 16; // 16 is equivalent to 12 seconds
#endif
          sobj->cp.confinfo.gm_conf_NoOfValidDialAttempts_by_user = 7; // 7 is equivalent to 3 attempts.
          OnConferenceCallProcess(sobj);
        } else if (sobj->cp.isThisCallForwarded) {
          PlayOnConnect(sobj,EN_FILE_CALLFWD);
        } else {
          PlayOnConnect(sobj,EN_FILE_GREETING);
        }
        break;
      }
    case EN_FOKINGDIAL_REL: 
      {
        CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent EN_FOKINGDIAL_REL %s",sobj->uid.c_str());
#ifdef XGATE_HAS_BUSINESS_SUPPORT
        //only copy interested call params for this sipevent
        copyRequiredCallParam(callObj->sipeventtype, sobj, callObj);
#else
        //sobj->cp = callObj->cp; //Yoga, commented for 1-1 call teting 
        //sobj->cp.m_mappedUserList = callObj->cp.m_mappedUserList;	
#endif
        if(!sobj->cp.m_mappedUserList.empty()) {
          DialOut(sobj);
        } else {
          PlayOnConnect(sobj,EN_FILE_GREETING);
        }

        break;
      }
    case EN_TRANSFER_REL:
      {
        sobj->cp = callObj->cp;
        sobj->cp.m_mappedUserList = callObj->cp.m_mappedUserList;
        DialOut(sobj);
        break;
      }
    case EN_PB_COMPLETE_REL:
      {
        switch(sobj->fileState) {
          case EN_FILE_GREETING:
            {
              if(!sobj->cp.isPstnNumber) {
                if(!sobj->cp.m_mappedUserList.empty()) {
                  // Send for Dial re-query;
                  PlayOnConnect(sobj,EN_FILE_CONNECTINGMSG);
                  //below code has been disabled to fix connecting message overlapping by ring-tone ivr for 1-1 call
#if 0 //Yoga, disabled this as part of business layer redesign task
                  if(sobj->cp.isForkingenabled) {
                    xGateCallObj *temp_callObj;
                    temp_callObj =  new xGateCallObj;
                    temp_callObj->xGateCallObjInit();
                    temp_callObj->uid = sobj->uid;
                    temp_callObj->sipeventtype =EN_FORKING_REQ ;
                    temp_callObj->cp = sobj->cp;
                    temp_callObj->cp.confinfo = sobj->cp.confinfo;
                    PostMessage(temp_callObj,XGATEPOSTTYPE::EN_PROCESS);
                  }
#endif
                } else {
                  SofiaSendBye(sobj->sofianh);
                  if (!sobj->cp.isconferenceCall &&
                      !sobj->cp.isTransferenabled) {
                    RequestForMedia(sobj,EN_MEDIA_DELETE);
                  }
                }
              } else {
                PlayOnConnect(sobj,EN_FILE_CONNECTINGMSG);
              }
              break;
            }
          case EN_FILE_CALLFWD:
            {
              PlayOnConnect(sobj,EN_FILE_GREETING);
              break;
            }
          case EN_FILE_CONNECTINGMSG:
            {
              //below code has been disabled to fix connecting message ivr overlapping by ring-tone ivr for 1-1 call
#if 0 //Yoga, disabled this as part of business layer redesign task
              if(!sobj->cp.isForkingenabled) {
                DialOut(sobj);
              }
#else
              if(!sobj->cp.isForkingenabled) {
                DialOut(sobj);
              } else if(sobj->cp.isForkingenabled) {
                xGateCallObj *temp_callObj;
                temp_callObj =  new xGateCallObj;
                temp_callObj->xGateCallObjInit();
                temp_callObj->uid = sobj->uid;
                temp_callObj->sipeventtype =EN_FORKING_REQ ;
                temp_callObj->cp = sobj->cp;
                temp_callObj->cp.confinfo = sobj->cp.confinfo;
                PostMessage(temp_callObj,XGATEPOSTTYPE::EN_PROCESS);
              }
#endif
              if(sobj->cp.isPstnNumber == false) {
                PlayOnConnect(sobj,EN_FILE_RBT);
              }
              break;
            }
          case EN_FILE_DISCONNECT:
            {
              CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent EN_PB_COMPLETE_REL disconnection %s",sobj->uid.c_str());
              SofiaSendBye(sobj->sofianh);
              //Fix for JIRA issue UA-298
              #if 1
              if (!sobj->cp.isTransferenabled) {
                RequestForMedia(sobj,EN_MEDIA_DELETE);
              }
	      #else
              if (!sobj->cp.isconferenceCall &&
                  !sobj->cp.isTransferenabled) {
                RequestForMedia(sobj,EN_MEDIA_DELETE);
              }
	      #endif
              break;
            }
          case EN_FILE_IVR:
            {
              CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent EN_PB_COMPLETE_REL %s",sobj->uid.c_str());
              if(sobj->cp.isSwitchBoardCall) {
                if(sobj->switchboardparam.SwbCallEvent == EN_SWB_PLAY) {
                  sobj->switchboardparam.SwbCallEvent = EN_SWB_PLAY_COMPLET;
                }
                if(sobj->switchboardparam.SwbCallEvent != EN_SWB_PLAY_PASSTHROUGH) {
                  OnSwitchBoardCallProcess(sobj);
                }
              } else if(sobj->cp.isconferenceCall) {
#if 0 //Yoga, disbaled this part
                if(sobj->cp.confinfo.ConfCallEvent == EN_CONF_PLAY) {
                  printf("********** EN_PB_COMPLETE_REL EN_FILE_IVR complete for EN_CONF_PLAY ****** \n");
                  CLog::Detail(THISMODULE,"EN_PB_COMPLETE_REL EN_FILE_IVR complete for EN_CONF_PLAY %s",sobj->uid.c_str());
                  sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY_COMPLET;
                  OnConferenceCallProcess(sobj);	
                  break;
                }
                if(sobj->cp.confinfo.ConfCallEvent == EN_CONF_IVRPLAY){
                  printf("********** EN_PB_COMPLETE_REL EN_FILE_IVR complete for EN_CONF_IVRPLAY ****** \n");
                  CLog::Detail(THISMODULE,"EN_PB_COMPLETE_REL EN_FILE_IVR complete for EN_CONF_IVRPLAY %s",sobj->uid.c_str());
                  OnConferenceCallProcess(sobj);	
                  break;
                }
                if(sobj->cp.confinfo.ConfDtmfState == EN_CONF_NO_DTMF) {
                  sobj->cp.confinfo.ConfDtmfState = EN_CONF_DTMFSTART;
                  sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY_PASSTHROUGH_GETDTMF;
                  OnConferenceCallProcess(sobj); // is required -as ConfCallEvent == PLAY_PASSTHROUGH
                }
                if(sobj->cp.confinfo.ConfCallEvent != EN_CONF_PLAY_PASSTHROUGH_GETDTMF) {
                  OnConferenceCallProcess(sobj);
                } 
#else //this enough in business layer design
                CLog::Detail(THISMODULE,"EN_PB_COMPLETE_REL occured for conference call%s",sobj->uid.c_str());
                OnConferenceCallProcess(sobj);	
#endif
              }
              break;
            }
        }
      }
    case EN_HOLDRESUME_REL:
      {
        OnHold_Resume(sobj); // sobj is A Leg Peer
        break;
      }
    case EN_P2PMEDIA_REL:
      {
        break;
      }
    case EN_DIALOUT_REL:
      {
        CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent EN_DIALOUT_REL %s",sobj->uid.c_str());
        //Need to Copy the Call Info Paramater
#ifdef XGATE_HAS_BUSINESS_SUPPORT
        //only copy interested call params for this sipevent
        copyRequiredCallParam(callObj->sipeventtype, sobj, callObj);
        /*sobj->cdr.updateCDRInfo(sobj->cp); //TODO: Need to verify with Yadav, taking more time to execute return back
          updateCDR(sobj);*/
#else
        sobj->cp = callObj->cp; 
        sobj->cp.m_mappedUserList = callObj->cp.m_mappedUserList; 
#endif

        sobj->P2PMedia =  CheckforP2PMedia(sobj);
        sobj->remoteSDPinfo[0].outproto =  "DTLS";
        RequestForMedia(sobj);
        //if(!sobj->P2PMedia)
        //		RequestForMedia(sobj);
        //	else
        //				DialOut(sobj);
        break;
      }
    case EN_SWB_EVENT_REL:{
                            CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent EN_SWB_EVENT_REL  %s",sobj->cp.call_id.c_str());
#ifdef XGATE_HAS_BUSINESS_SUPPORT
                            //only copy interested call params for this sipevent
                            copyRequiredCallParam(callObj->sipeventtype, sobj, callObj);
#else
                            sobj->cp =  callObj->cp;
                            sobj->cp.m_mappedUserList = callObj->cp.m_mappedUserList; 
                            sobj->switchboardparam.PlayFile =  callObj->cp.swbinfo.PlayFile;
                            sobj->switchboardparam.SwbCallEvent = callObj->cp.swbinfo.SwbCallEvent;
                            sobj->switchboardparam.MaxDigit = callObj->cp.swbinfo.MaxDigit;
                            sobj->switchboardparam.DigitTimeOut = callObj->cp.swbinfo.DigitTimeOut;
#endif
                            OnSwitchBoardCallProcess(sobj);
                            break;
                          }
    case EN_CONF_EVENT_REL:
                          {
                            CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent EN_CONF_EVENT_REL  %s", sobj->cp.uid.c_str());
                            printf("EN_CONF_REL -- conf event %d uid %s callid %s \n", \
                                callObj->cp.confinfo.ConfCallEvent, callObj->uid.c_str(), callObj->callid.c_str());
                            //sobj->callid =  callObj->callid;
#ifdef XGATE_HAS_BUSINESS_SUPPORT
                            //only copy interested call params for this sipevent
                            if(callObj->cp.confinfo.ConfCallEvent != EN_CONF_NULL) {
                              copyRequiredCallParam(callObj->sipeventtype, sobj, callObj);
                              OnConferenceCallProcess(sobj);
                            }
                            break;
#else
                            //sobj->uid = callObj->uid;
                            if(callObj->cp.confinfo.ConfCallEvent != EN_CONF_NULL){
                              sobj->dialcallid =  "";	
                              sobj->cp =  callObj->cp;
                              sobj->cp.m_mappedUserList = callObj->cp.m_mappedUserList; 
                              sobj->cp.confinfo = callObj->cp.confinfo;
                            }
                            OnConferenceCallProcess(sobj); // whether this will process DTMF-request?
                            break;
#endif
                          }
    case EN_CALLSTATE_UPDATE_REL:{
                                   memcpy(&sobj->cp, &(callObj->cp),sizeof(callObj->cp));
                                   CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent EN_CALLSTATE_UPDATE_REL  %s",sobj->cp.call_id.c_str());
                                   break;
                                 }
    case EN_NULL:{
                   CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent ==> %s","EN_NULL - No Action Required");
                   break;
                 }
#endif //DebugVM end
    case EN_TERMINATED_REL:
		 { 
		    XGLOG_INFO("SofiaSiphandler::OnProcessEvent ==> %s EN_TERMINATED_REL disconnect peers", sobj->uid.c_str());
		    if(sobj)
		    {
		       if(sobj->callstate == EN_CS_CONNECTED)
		       {
			  //SofiaSendBye((nua_handle_t*)sobj->sofianh);

			  //Fix for Bye not sending issue
#if 1
			  std::string url("");
			  url = "sip:";
			  url.append(sobj->cp.routeIpAddress.c_str());
			  url.append(":");
			  url.append(XGSIPPORT);
			  url.append(";");
			  url.append("transport=");
			  url.append("udp");

			  nua_bye((nua_handle_t*)sobj->sofianh, NUTAG_PROXY((url_string_t*)url.c_str()),TAG_END());
#endif
		       }
		       else if(sobj->callstate < EN_CS_CONNECTED)
		       {
			  XGLOG_INFO("SofiaSiphandler::OnProcessEvent Sending 484 Address Incomplete Response");
			  nua_respond((nua_handle_t*)sobj->sofianh, 484,"Address Incomplete", TAG_END());
		       }
		    }
		    break;
		 }
#if 0 //DebugVM end
    case EN_MEDIA_UPDATE_REL:{
                               CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent  EN_MEDIA_UPDATE_REL %s",sobj->cp.call_id.c_str());
                               RequestForMedia(sobj,EN_MEDIA_UPDATE);
                               break;
                             }

    case EN_MEDIA_GETDIGIT_REL: // On Media get dtmf digits
                             if(!sobj->cp.digitReceived.empty()) { // Check if digit is filled by SIP INFO already 
                               if(sobj->cp.isconferenceCall){
                                 sobj->cp.confinfo.ConfCallEvent = EN_CONF_GETDTMF_COMPLET;
                                 OnConferenceCallProcess(sobj);
                               }else if (sobj->cp.isSwitchBoardCall){
                                 sobj->switchboardparam.SwbCallEvent = EN_SWB_GETDTMF_COMPLET;
                                 OnSwitchBoardCallProcess(sobj);

                               }

                             }else{// Request to Media to get digit received.
                               sobj->cp.PbState = callObj->cp.PbState;
                               sobj->cp.Legid = callObj->cp.Legid;
                               CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent ==> EN_MEDIA_GETDIGIT_REQ");
                               RequestForMedia(sobj,EN_MEDIA_GET_DTMF_DIGITS);
                             }
                             break;
    case EN_MEDIA_RECVDIGIT_REL: // From Media dtmf digits are received.
                             CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent ==> EN_MEDIA_RECVDIGIT_RES");
                             /*
                                if ((sobj->cp.sipinfo_method == false) && (strlen(event->media_dtmf_digits.c_str()) > 0))

                                {
                                sobj->cp.inband_dtmf_method = true;
                             //sobj->cp.sipinfo_method = false;
                             sobj->switchboardparam.DigitReceived = event->media_dtmf_digits; 
                             sobj->switchboardparam.SwbCallEvent = EN_SWB_GETDTMF_COMPLET;
                             OnSwitchBoardCallProcess(sobj);
                             }
                             */
                             if ((strlen(callObj->sdpInfo[0].media_dtmf_digits.c_str()) <= 0) && (sobj->cp.isSwitchBoardCall))
                               callObj->sdpInfo[0].media_dtmf_digits.assign("0");

                             if ((sobj->cp.sipinfo_method == false) && (strlen(callObj->sdpInfo[0].media_dtmf_digits.c_str()) > 0))
                             {
                               sobj->cp.inband_dtmf_method = true;
                               //sobj->cp.sipinfo_method = false;
                               if(sobj->cp.isSwitchBoardCall)
                               {
                                 sobj->cp.digitReceived =  callObj->sdpInfo[0].media_dtmf_digits;
                                 //                                 sobj->switchboardparam.DigitReceived.assign(callObj->sdpInfo[0].media_dtmf_digits.c_str());
                                 CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent ==> EN_MEDIA_RECVDIGIT_RES DigitReceived = %s",sobj->cp.digitReceived.c_str());
                                 sobj->switchboardparam.SwbCallEvent = EN_SWB_GETDTMF_COMPLET;
                                 OnSwitchBoardCallProcess(sobj);
                               }
                               else if(sobj->cp.isconferenceCall)
                               {
                                 if(!sobj->cp.confinfo.isUserJoined) {
                                   //                                   sobj->cp.confinfo.DigitReceived.assign(callObj->sdpInfo[0].media_dtmf_digits.c_str());
                                   sobj->cp.digitReceived =  callObj->sdpInfo[0].media_dtmf_digits;                                   
                                   CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent ==> EN_MEDIA_RECVDIGIT_RES DigitReceived = %s",sobj->cp.digitReceived.c_str());
                                   sobj->cp.confinfo.ConfCallEvent = EN_CONF_GETDTMF_COMPLET;
                                   OnConferenceCallProcess(sobj);
                                 }
                               }
                             }
                             if (sobj->cp.isSwitchBoardCall)
                               sobj->cp.gm_userDiallDTMFdigits = true;
                             break;

    case EN_APPNOTIFICATION_TIMEOUT_RES:
                             CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent ==> EN_APPNOTIFICATION_TIMEOUT_RES");
                             if (!sobj->cp.gm_appNotificationDialOutSent)
                             { // PushNotification Timer is expired, and Registration-event not received so far. need to send  DialOut for this call.
                               //sobj->cp.gm_appNotificationDialOutSent = true;
                               sobj->cp.m_mappedUserList =  callObj->cp.m_mappedUserList;
                               sobj->cp.confinfo =  callObj->cp.confinfo;
                               AppDialOnWake(sobj->uid);

                               //	callObj->sipeventtype = XGATESIPEVENT::EN_APPREGCOMPLET_REQ;
                               // 	PostMessage(callObj,XGATEPOSTTYPE::EN_PROCESS);
                             }
                             break;
                             // obsolete
#if 0 
    case EN_CALLHANDLING_TIMEOUT_RES:
                             CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent ==> EN_CALLHANDLING_TIMEOUT_RES");
                             //callHandling Timer is expired, need to send  DialOut for this call.
                             sobj->cp.ptrCallHandlingVector =  callObj->cp.ptrCallHandlingVector;
                             sobj->cp.confinfo = callObj->cp.confinfo;
                             // Find out which device received timer-timeout...
                             /*
                                for (auto it = (*sobj->cp.ptrCallHandlingVector).begin(), index = 0; (index == (it->currentRecordIndex)) && (it < (*sobj->cp.ptrCallHandlingVector).end()); it++, index++)
                                {
                             // find out uid for this call of this device...
                             //CallListCancle(sobj->uid);
                             CheckResponseRequired(200,sobj);
                             // if this is last device on timeout, then send Bye...
                             SofiaSendBye(sobj->sofianh);

                             }
                             <*/
                             break;
#endif
    case EN_DIAL_TIMEOUT_RES:
                             {
                               CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent ==> EN_DIAL_TIMEOUT_RES");
                               if(sobj->callstate == EN_CS_CONNECTED)
                               {
                                 CallListCancle(sobj->uid);
                               }
                               break;
                             }
    case EN_SWBMAINMENUIVR_TIMEOUT_RES:
                             CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent ==> EN_SWBMAINMENUIVR_TIMEOUT_RES");
                             // following line added for testing; to be removed later...
                             //sobj->cp.inband_dtmf_method = true;

                             if (strcmp(sobj->cp.cnCalled.c_str(), callObj->cp.cnCalled.c_str()))
                               break; // both are NOT same; so extension is dialled; so come out of this timer-indication...

                             //if (((strlen(sobj->switchboardparam.DigitReceived.c_str()) <= 0) || (!strcmp(sobj->switchboardparam.DigitReceived.c_str(),"$$$")))&& (!sobj->cp.inband_dtmf_method))
                             if ((strlen(sobj->cp.digitReceived.c_str()) <= 0) && (!sobj->cp.inband_dtmf_method))
                             {
                               CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent ==> EN_SWBMAINMENUIVR_TIMEOUT_RES => EN_XGATE_PB_SENDDIGIT");
                               sobj->cp.PbState = EN_XGATE_PB_SENDDIGIT; // EN_XGATE_PB_SENDDIGIT => Forced DTMF-digit-collection request to media...
                               sobj->cp.Legid = 1;
                               // post message to Dispatcher.
                               ((SofiaSiphandler*)((nua_magic_t*)sobj->sofiamh))-> RequestForMedia(sobj,EN_MEDIA_GET_DTMF_DIGITS);
                               break;
                             }
                             //if(strlen(sobj->switchboardparam.DigitReceived.c_str()) > 0)
                             if(strlen(sobj->cp.digitReceived.c_str()) > 0)
                               break;

                             // else this is sipinfo_method; and inform Processor-thread about this timer-thread timeout.
                             xGateCallObj *temp_callObj;
                             temp_callObj =  new xGateCallObj;
                             temp_callObj->xGateCallObjInit();
                             temp_callObj->callid = callObj->callid;
                             temp_callObj->sipeventtype = XGATESIPEVENT::EN_SWBMAINMENUIVR_TIMEOUT_RES;
                             temp_callObj->cp = callObj->cp;
                             //temp_callObj->switchboardinfo.DigitReceived.assign("0");
                             //temp_callObj->cp.swbinfo.DigitReceived.assign(SWB_MAINMENUIVR_TIMEOUT_WITH_NO_DTMF_DIGITS);
                             PostMessage(temp_callObj,XGATEPOSTTYPE::EN_PROCESS);
                             break;

    case EN_CONFERENCE_DTMFDIGIT_TIMEOUT_RES:
                             CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent ==> EN_CONFERENCE_DTMFDIGIT_TIMEOUT_RES");
                             // following line added for testing; to be removed later...
                             if(sobj->fileState == EN_FILE_DISCONNECT || sobj->fileState > EN_FILE_DISCONNECT)// call is in disconnect/Non initialize state.
                               return ;
                             sobj->cp.inband_dtmf_method = true;

                             sobj->cp.confinfo.conf_timer_thread_1 = 0;
                             sobj->cp.confinfo.conf_timer_thread_2 = 0;

                             if ((strlen(sobj->cp.digitReceived.c_str()) > 0) || (sobj->cp.gm_userDiallDTMFdigits))
                             {
                               CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent ==> EN_CONFERENCE_DTMFDIGIT_TIMEOUT_RES ; Timeout event neglected...");
                               sobj->cp.digitReceived.assign("");
                               sobj->cp.gm_userDiallDTMFdigits = false;
                               break; // DTMF-digits are pressed; so don't prcess this timeout request...
                             }

                             if (((strlen(sobj->cp.digitReceived.c_str()) <= 0) || (!strcmp(sobj->cp.digitReceived.c_str(),"$$$")))&& (!sobj->cp.inband_dtmf_method))
                             {
                               CLog::Detail(THISMODULE,"SofiaSiphandler::OnProcessEvent ==> EN_CONFERENCE_DTMFDIGIT_TIMEOUT_RES => EN_XGATE_PB_SENDDIGIT");
                               sobj->cp.PbState = EN_XGATE_PB_SENDDIGIT; // EN_XGATE_PB_SENDDIGIT => Forced DTMF-digit-collection request to media...
                               sobj->cp.Legid = 1;
                               // post message to Dispatcher.
                               ((SofiaSiphandler*)((nua_magic_t*)sobj->sofiamh))-> RequestForMedia(sobj,EN_MEDIA_GET_DTMF_DIGITS);
                               break;
                             }
                             //sobj->cp.confinfo.ConfCallEvent = EN_CONF_GETDTMF_COMPLET;
                             sobj->cp.confinfo.ConfDtmfState = EN_CONF_NO_DTMF;
                             sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY;
                             OnConferenceCallProcess(sobj);
                             break;

                             // else this is sipinfo_method; and inform Processor-thread about this timer-thread timeout.
                             //xGateCallObj *temp_callObj;
                             temp_callObj =  new xGateCallObj;
                             temp_callObj->xGateCallObjInit();
                             temp_callObj->callid = callObj->callid;
                             temp_callObj->sipeventtype = XGATESIPEVENT::EN_CONFERENCE_DTMFDIGIT_TIMEOUT_RES;
                             temp_callObj->cp = callObj->cp;
                             //temp_callObj->switchboardinfo.DigitReceived.assign("0");
                             //temp_callObj->cp.swbinfo.DigitReceived.assign(SWB_MAINMENUIVR_TIMEOUT_WITH_NO_DTMF_DIGITS);
                             PostMessage(temp_callObj,XGATEPOSTTYPE::EN_PROCESS);
                             break;

#endif //DebugVM end

    case EN_SIP_MAKECALL_EVENT_REQ:
            {
              XGLOG_INFO("SofiaSiphandler::OnProcessEvent EN_SIP_MAKECALL_EVENT_REQ for uid(%s)",callObj->uid.c_str());
              ProcessDialoutForMakeCall(sobj,callObj);
            }
            break;
    case EN_ERROR_NOTIFY_REL :
            {
              XGLOG_INFO("SofiaSiphandler::OnProcessEvent EN_ERROR_NOTIFY_REL for uid(%s)",callObj->uid.c_str());
              handleErrorNotifyResponse(sobj,callObj);
            }
    default:{
              XGLOG_INFO("SofiaSiphandler::OnProcessEvent Default");
              if(callObj->sipeventtype  > 399){
                // check incoming or out
                // Call the Responce 1xx to 6xx function
              }
              break;	
            }
  }
}

bool SofiaSiphandler::handleErrorNotifyResponse(xGateSofiaobj *sobj, xGateCallObj *callObj)
{
	sobj->cp = callObj->cp;
	XGLOG_INFO("SofiaSiphandler::handleErrorNotifyResponse for uid(%s)",sobj->uid.c_str());
	if(strncmp(sobj->cp.sipErrResponse.c_str(),"SIP_ERROR_EVENT_SERVER_TIMEOUT",sobj->cp.sipErrResponse.length()) == 0)
	{
		XGLOG_INFO("SofiaSiphandler::handleErrorNotifyResponse Sending response for uid(%s)",sobj->uid.c_str());
		RespondToPeer(EN_504_REL,sobj); 
	}
	else if(strncmp(sobj->cp.sipErrResponse.c_str(),"SIP_ERROR_EVENT_VALIDATION_FAILURE",sobj->cp.sipErrResponse.length())== 0)
	{
		XGLOG_INFO("SofiaSiphandler::handleErrorNotifyResponse Sending 403 response for uid(%s)",sobj->uid.c_str());
		RespondToPeer(EN_403_REL,sobj); 
	}
	else if(strncmp(sobj->cp.sipErrResponse.c_str(),"SIP_ERROR_EVENT_REQ_NOT_ACCEPTABLE",sobj->cp.sipErrResponse.length())== 0)
	{
		XGLOG_INFO("SofiaSiphandler::handleErrorNotifyResponse Sending 406 response for uid(%s)",sobj->uid.c_str());
		//RespondToPeer(EN_406_REL,sobj); 
	}
	return true;
}

#if 0//DebugVM start

bool SofiaSiphandler::copyRequiredCallParam(XGATESIPEVENT sipevent, xGateSofiaobj *sobj, xGateCallObj *callObj)
{
  callInfoParameter &dstCallParam = sobj->cp;
  callInfoParameter &srcCallParam = callObj->cp;
  switch(sipevent) {
    case EN_DIALOUT_REL:
      {
        CLog::Detail(THISMODULE,"SofiaSiphandler::copyRequiredCallParam for sipevent: %d uid: %s", \
            sipevent, srcCallParam.uid.c_str());
        dstCallParam.calledDeviceType = srcCallParam.calledDeviceType;
        dstCallParam.m_mappedUserList = srcCallParam.m_mappedUserList;
        dstCallParam.rbtFile = srcCallParam.rbtFile;
        dstCallParam.userGreetingFile = srcCallParam.userGreetingFile;
        dstCallParam.userConnectingFile = srcCallParam.userConnectingFile;
        dstCallParam.callOnHoldFilenameCalledParty = srcCallParam.callOnHoldFilenameCalledParty;
        dstCallParam.callOnHoldFilenameCallingParty = srcCallParam.callOnHoldFilenameCallingParty;
        dstCallParam.gm_user_greeting_file_name_user_B = srcCallParam.gm_user_greeting_file_name_user_B;
        dstCallParam.gm_user_greeting_file_name_user_B_After = srcCallParam.gm_user_greeting_file_name_user_B_After;
        dstCallParam.gm_user_company_hour = srcCallParam.gm_user_company_hour;
        dstCallParam.isForkingenabled = srcCallParam.isForkingenabled; 
        dstCallParam.isconferenceCall = srcCallParam.isconferenceCall;
        dstCallParam.isThisCallForwarded = srcCallParam.isThisCallForwarded;
        dstCallParam.callForwardingFile = srcCallParam.callForwardingFile;
        dstCallParam.isPstnNumber = srcCallParam.isPstnNumber;
        if(dstCallParam.isPstnNumber) {
          dstCallParam.csfilename = srcCallParam.userConnectingFile;
          dstCallParam.gm_user_company_hour = 0;
          dstCallParam.cnCalling = srcCallParam.cnCalling;
        }
        dstCallParam.isSwitchBoardCall = srcCallParam.isSwitchBoardCall;
        break;
      }
    case EN_SWB_EVENT_REL:
      {
        xGateSwbInfo &dstSwbInfo = sobj->switchboardparam;
        xGateSwbInfo &srcSwbInfo = callObj->cp.swbinfo;
        dstSwbInfo.SwbCallEvent = srcSwbInfo.SwbCallEvent;
        switch(dstSwbInfo.SwbCallEvent) {
          case EN_SWB_WELCOME:
            {
              dstSwbInfo.PlayFile =     srcSwbInfo.PlayFile;
              dstSwbInfo.MaxDigit =     srcSwbInfo.MaxDigit;
              dstSwbInfo.DigitTimeOut = srcSwbInfo.DigitTimeOut;
              break;
            }
          case EN_SWB_DIAL:
            {
              dstCallParam.calledDeviceType = srcCallParam.calledDeviceType;
              dstCallParam.m_mappedUserList = srcCallParam.m_mappedUserList;
              dstCallParam.rbtFile = srcCallParam.rbtFile;
              dstCallParam.userGreetingFile = srcCallParam.userGreetingFile;
              dstCallParam.userConnectingFile = srcCallParam.userConnectingFile;
              dstCallParam.callOnHoldFilenameCalledParty = srcCallParam.callOnHoldFilenameCalledParty;
              dstCallParam.callOnHoldFilenameCallingParty = srcCallParam.callOnHoldFilenameCallingParty;
              dstCallParam.gm_user_greeting_file_name_user_B = srcCallParam.gm_user_greeting_file_name_user_B;
              dstCallParam.gm_user_greeting_file_name_user_B_After = srcCallParam.gm_user_greeting_file_name_user_B_After;
              dstCallParam.gm_user_company_hour = srcCallParam.gm_user_company_hour;
              break;
            }
          default:
            {
              break;
            }
        }
        dstCallParam.isForkingenabled = srcCallParam.isForkingenabled; 
        dstCallParam.isconferenceCall = srcCallParam.isconferenceCall;
        dstCallParam.isThisCallForwarded = srcCallParam.isThisCallForwarded;
        dstCallParam.callForwardingFile = srcCallParam.callForwardingFile;
        dstCallParam.isPstnNumber = srcCallParam.isPstnNumber;
        dstCallParam.isSwitchBoardCall = srcCallParam.isSwitchBoardCall;
        break;
      }
    case EN_FOKINGDIAL_REL:
      {
        dstCallParam.m_mappedUserList = srcCallParam.m_mappedUserList;
        dstCallParam.userGreetingFile = srcCallParam.userGreetingFile;
        return true; //TODO: It's required here. we don't need to update other below common params
      }
    case EN_CONF_EVENT_REL:
      {
        xGateConfInfo &dstConfInfo = sobj->cp.confinfo;
        xGateConfInfo &srcConfInfo = callObj->cp.confinfo;
        dstConfInfo.ConfCallEvent = srcConfInfo.ConfCallEvent;
        switch(dstConfInfo.ConfCallEvent) {
          case EN_CONF_WELCOME:
            {
              dstConfInfo.PlayFile = srcConfInfo.PlayFile;
              dstConfInfo.MaxDigit = srcConfInfo.MaxDigit;
              dstCallParam.gm_mgc_ip = srcCallParam.gm_mgc_ip;
              break;
            }
          case EN_CONF_PLAY_PASSTHROUGH_GETDTMF:
            {
              dstConfInfo.PlayFile = srcConfInfo.PlayFile;
              //dstConfInfo.ConfDtmfState = srcConfInfo.ConfDtmfState;
              dstConfInfo.gm_conf_NoOfValidDialAttempts_by_user = srcConfInfo.gm_conf_NoOfValidDialAttempts_by_user;
              break;
            }
          case EN_CONF_CALLDISCONNECT:
            {
              CLog::Error(THISMODULE, "copyRequiredCallParam for EN_CONF_CALLDISCONNECT !");
              dstConfInfo.PlayFile = srcConfInfo.PlayFile;
              break;
            }
          case EN_CONF_ADDPARTICIPANT:
            {
              CLog::Error(THISMODULE, "copyRequiredCallParam for EN_CONF_ADDPARTICIPANT !");
              dstConfInfo.PlayFile = srcConfInfo.PlayFile;
              dstConfInfo.noOfConfParticipantsCurrentlyJoined = srcConfInfo.noOfConfParticipantsCurrentlyJoined;
              dstConfInfo.gm_mgResourceId = srcConfInfo.gm_mgResourceId;
              dstConfInfo.gm_conference_status = srcConfInfo.gm_conference_status;
              dstConfInfo.gm_conf_host_user = srcConfInfo.gm_conf_host_user;
              dstCallParam.gm_mgc_ip = srcCallParam.gm_mgc_ip;
              break;
            }
          case EN_CONF_PLAY:
            {
              break;
            }
          default:
            {
              break;
            }
        }
        dstCallParam.isForkingenabled = srcCallParam.isForkingenabled; 
        dstCallParam.isconferenceCall = srcCallParam.isconferenceCall;
        dstCallParam.isThisCallForwarded = srcCallParam.isThisCallForwarded;
        dstCallParam.callForwardingFile = srcCallParam.callForwardingFile;
        dstCallParam.isPstnNumber = srcCallParam.isPstnNumber;
        dstCallParam.isSwitchBoardCall = srcCallParam.isSwitchBoardCall;
      }
    default:
      {
        break;
      }
  }
  return true;
}

/**
 * */
void SofiaSiphandler::OnSwitchBoardCallProcess(xGateSofiaobj *sobj){
  CLog::Detail(THISMODULE,"SofiaSiphandler::OnSwitchBoardCallProcess  event id %d unique id  %s",sobj->switchboardparam.SwbCallEvent,sobj->uid.c_str());
  switch(sobj->switchboardparam.SwbCallEvent){
    case EN_SWB_NULL:
      return;
      break;
    case EN_SWB_WELCOME:
      sobj->switchboardparam.SwbCallEvent=  EN_SWB_IVRCONNECT;
      sobj->cp.csfilename =  sobj->switchboardparam.PlayFile; //TODO: Yoga added for swb ivr play testing
      sobj->remoteSDPinfo[0].outproto =  "DTLS"; // Fix for Dialout through  SWB without MSID & DTLS info -Yadav
      RequestForMedia(sobj);
      //	RequestForMedia(sobj, EN_MEDIA_18X);
      return;
    case EN_SWB_IVRCONNECT:
      break;
    case EN_SWB_PLAY:
    case EN_SWB_PLAY_PASSTHROUGH:
      PlayOnConnect(sobj,EN_FILE_IVR);
      // sobj->cp.PbState = EN_XGATE_PB_PLAY;
      // sobj->cp.csfilename =  sobj->switchboardparam.PlayFile;
      sobj->cp.Legid = 1;// TODO  Yadav Chang the Leg ID to 2 since this leb B (this code is for temp need to revert after testeing)
      sobj->cp.digitReceived = "";
      // RequestForMedia(sobj, EN_MEDIA_UPDATE);
      //sobj->switchboardparam.SwbCallEvent = EN_SWB_GETDTMF;
      sobj->switchboardparam.PlayFile.assign("");
      sobj->switchboardparam.MaxDigit = 3;
      sobj->switchboardparam.DigitTimeOut = 10;
      sobj->switchboardparam.SwbCallEvent = EN_SWB_GETDTMF;
      return;
    case EN_SWB_PLAY_COMPLET:
      sobj->switchboardparam.SwbCallEvent = EN_SWB_PLAY_COMPLET;
      break;
    case EN_SWB_GETDTMF:
      sobj->switchboardparam.SwbCallEvent = EN_SWB_GETDTMF;
      //      sobj->switchboardparam.DigitReceived = "$$$";
      CLog::Detail(THISMODULE,"SofiaSiphandler::OnSwitchBoardCallProcess  Wating for MAX Digit %d",sobj->switchboardparam.MaxDigit);
      return;
    case EN_SWB_GETDTMF_COMPLET:
      sobj->cp.csfilename = "";
      sobj->switchboardparam.PlayFile = "";
      sobj->switchboardparam.SwbCallEvent = EN_SWB_GETDTMF_COMPLET;
      CLog::Detail(THISMODULE,"SofiaSiphandler::OnSwitchBoardCallProcess  DTMF Digit %s",sobj->cp.digitReceived.c_str());
      sobj->cp.gm_userDiallDTMFdigits = true;
      //sobj->cp.cnCalled.assign(sobj->switchboardparam.DigitReceived.c_str());
      break;
    case EN_SWB_DIAL:
      //        sobj->ipaddr = sobj->dialipaddr;
      // sobj->SofiaLegB->xGateSofiaobjInit();
      //  sobj->SofiaLegB->xGateCopyPeerInfo(*sobj);
      //PlayOnConnect(sobj,EN_FILE_CALLFWD);
      if (strlen(sobj->cp.callForwardingFile.c_str()) > 0)
        PlayOnConnect(sobj,EN_FILE_CALLFWD);
      else
        PlayOnConnect(sobj,EN_FILE_GREETING);
      //DialOut(sobj);
      //sobj->switchboardparam.SwbCallEvent = EN_SWB_CALLCONNECTED; // added for tsting; to be removed later...
      return;
    case EN_SWB_CALLCONNECTED:
      break;
    case EN_SWB_DIALDEFAULT:
      break;
    case EN_SWB_CALLDISCONNECT:
      CLog::Detail(THISMODULE,"SofiaSiphandler::OnSwitchBoardCallProcess : case EN_SWB_CALLDISCONNECT:");
      //:         SofiaSendBye(sobj->SofiaLegB->sofianh); 
      sobj->switchboardparam.SwbCallEvent = EN_SWB_CALLDISCONNECT;
      break;
    case EN_SWB_CALLDISCONNECTED:
      break;
    default:
      return;

  }
  xGateCallObj *callobj = new xGateCallObj;
  callobj->xGateCallObjInit();
  callobj->callid = sobj->callid;
  callobj->uid = sobj->uid;
  callobj->cp = sobj->cp;
  callobj->cp.m_mappedUserList = sobj->cp.m_mappedUserList;
  callobj->cp.swbinfo = sobj->cp.swbinfo;
  callobj->cp.swbinfo.SwbCallEvent = sobj->cp.swbinfo.SwbCallEvent;
  //callobj->cp = sobj->cp;
  callobj->sipeventtype = EN_SWB_EVENT_REQ;
  callobj->cp.swbinfo.PlayFile = sobj->switchboardparam.PlayFile;
  //  callobj->cp.swbinfo.DigitReceived = sobj->switchboardparam.DigitReceived;
  callobj->cp.swbinfo.SwbCallEvent =  sobj->switchboardparam.SwbCallEvent;
  //	memcpy(&callobj->switchboardinfo, &sobj->switchboardparam,sizeof(sobj->switchboardparam));
  PostMessageToProcessor(callobj);
  sobj->switchboardparam.PlayFile = "";
  // sobj->switchboardparam.DigitReceived = "";

}

void SofiaSiphandler::OnConferenceHostJoin(xGateSofiaobj *sobj)
{
  if(sobj) {
    CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceHostJoin uid %s callid - %s",sobj->uid.c_str(),sobj->cp.call_id.c_str());
    sobj->cp.confinfo.ConfDtmfModule = EN_CONF_HOSTCONTROL;
    sobj->cp.confinfo.ConfDtmfState = EN_CONF_ADDPARTICIPANTSENDMESSAGE10;
    sobj->cp.confinfo.ConfCallEvent = EN_CONF_NULL;
    sobj->cp.confinfo.MaxConfParticipants = CONF_MAX_PARTICIPANTS;
    // sobj->cp.confinfo.ConfCallEvent = EN_CONF_NULL;
    if(sobj->cp.confinfo.noOfConfParticipantsCurrentlyJoined > 0) {
      sobj->cp.PbState = EN_XGATE_PB_CONFALL;
      sobj->cp.csfilename =  "/root/wav_files/sounds/vectone/conf/conf_entry.wav";
    } else {
      sobj->cp.PbState = EN_XGATE_PB_REPEAT;
      sobj->cp.csfilename = BEEP;
    }
    RequestForMedia(sobj,EN_MEDIA_UPDATE);
  }
}

void SofiaSiphandler::OnConferenceParticipantJoin(xGateSofiaobj *sobj)
{
  if(sobj) {
    CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceParticipantJoin uid %s callid - %s  conf State = %s", \
        sobj->uid.c_str(),sobj->cp.call_id.c_str(),sobj->cp.confinfo.gm_conference_status.c_str());
    // sobj->cp.confinfo.ConfCallEvent = EN_CONF_NULL;
    sobj->cp.confinfo.ConfDtmfModule = EN_CONF_HOSTCONTROL;
    sobj->cp.confinfo.ConfDtmfState = EN_CONF_ADDPARTICIPANTSENDMESSAGE10;
    sobj->cp.confinfo.ConfCallEvent = EN_CONF_NULL;
    sobj->cp.confinfo.MaxConfParticipants = CONF_MAX_PARTICIPANTS;
    if(strcmp( sobj->cp.confinfo.gm_conference_status.c_str(), "active") == 0) {
      sobj->cp.PbState = EN_XGATE_PB_CONFALL;
      sobj->cp.csfilename =  "/root/wav_files/sounds/vectone/conf/conf_entry.wav";
    } else {
      sobj->cp.PbState = EN_XGATE_PB_REPEAT;
      sobj->cp.csfilename = BEEP;
    }
    RequestForMedia(sobj,EN_MEDIA_UPDATE);
  }
}

#if 1
void SofiaSiphandler::OnConferenceCallProcess(xGateSofiaobj *sobj)
{
  CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceCallProcess  event id %d callid  %s",sobj->cp.confinfo.ConfCallEvent,sobj->cp.call_id.c_str());

  switch(sobj->cp.confinfo.ConfCallEvent) {
    case EN_CONF_NULL:
      {
        printf("SofiaSiphandler::OnConferenceCallProcess EVENT EN_CONF_NULL ocurred !\n");
        return;
      }
    case EN_CONF_WELCOME:
      {
        //sobj->cp.gm_mgc_ip = GetConfernecMgIp();	
        CLog::Detail(THISMODULE, "SofiaSiphandler::OnConferenceCallProcess  %s Conference MG IP" ,sobj->cp.gm_mgc_ip.c_str());
        sobj->cp.confinfo.ConfCallEvent=  EN_CONF_IVRCONNECT;
        RequestForMedia(sobj, EN_MEDIA_CONF);
        return;
      }
    case EN_CONF_PLAY_PASSTHROUGH_GETDTMF:
      {
        sobj->cp.confinfo.ConfCallEvent = EN_CONF_GETDTMF; // playing file at background and collecting DTMF-digits parallely.
        sobj->cp.csfilename =  sobj->cp.confinfo.PlayFile;
        PlayOnConnect(sobj,EN_FILE_IVR); // gets DTMF-digits during PLAY_PASSTHROUGH
        return;
      }
    case EN_CONF_GETDTMF_COMPLET: //Note: yoga this empty case is required
      {
        CLog::Detail(THISMODULE, "SofiaSiphandler::OnConferenceCallProcess GETDTMF_COMPLET event triggered !");
        if(sobj->cp.digitReceived.empty()) {
          CLog::Error(THISMODULE,"OnConferenceCallProcess callid  %s", \
              sobj->cp.call_id.c_str());
          return;
        }
        break;
      }
    case EN_CONF_CALLDISCONNECT:
      {
        CLog::Detail(THISMODULE,"OnConferenceCallProcess : case EN_CONF_CALLDISCONNECT:");
        PlayResponse(700,sobj);
        return;
      }
    case EN_CONF_ADDPARTICIPANT:
      {
        CLog::Detail(THISMODULE,"OnConferenceCallProcess : case EN_CONF_ADDPARTICIPANT adding %d participant", \
            sobj->cp.confinfo.noOfConfParticipantsCurrentlyJoined);
        InsertCalltoConferenceList(sobj->cp.confinfo.gm_mgResourceId, sobj);
        sobj->cp.csfilename =  sobj->cp.confinfo.PlayFile;
        RequestForMedia(sobj, EN_MEDIA_CONF_ADDPARTICIPANT);
        sobj->cp.confinfo.ConfCallEvent = EN_CONF_IVRPLAY;
        sobj->cp.confinfo.isUserJoined = true;
        return;
      }
    case EN_CONF_IVRPLAY:
      {
        CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceCallProcess : EN_CONF_IVRPLAY uid %s -- callid -%s", \
            sobj->uid.c_str(),sobj->callid.c_str());
        sobj->cp.csfilename =  sobj->cp.confinfo.PlayFile;
        if(sobj->cp.confinfo.gm_conference_status == "active") {
          sobj->cp.PbState = EN_XGATE_PB_CONFALL;
          sobj->cp.csfilename =  "/root/wav_files/sounds/vectone/conf/conf_entry.wav";
        } else {
          sobj->cp.PbState = EN_XGATE_PB_REPEAT;
          sobj->cp.csfilename = BEEP;
        }
        RequestForMedia(sobj,EN_MEDIA_UPDATE);
        return;
      }
    case EN_CONF_DELPARTICIPANT:
      {
        CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceCallProcess : case EN_CONF_DELPARTICIPANT:");
        sobj->cp.csfilename =  "/root/wav_files/sounds/vectone/conf/conf_exit.wav";
        sobj->cp.PbState = EN_XGATE_PB_CONFALL;
        RequestForMedia(sobj, EN_MEDIA_UPDATE);
        if(sobj->cp.confinfo.isUserJoined) {
          RequestForMedia(sobj, EN_MEDIA_CONF_DELPARTICIPANT);
        } else {
          RequestForMedia(sobj, EN_MEDIA_DELETE);
        }
        DeleteCallFromConferenceList(sobj->cp.confinfo.gm_mgResourceId, sobj);
        break;
      }
    default:
      {
        return;
      }
  }
#if 1
  try {
    xGateCallObj *callobj = NULL;
    try {
      callobj = new xGateCallObj;
    }
    catch(std::bad_alloc& ba) {
      CLog::Error(THISMODULE, "OnConferenceCallProcess failed due to '%s' exception !",  ba.what());
      //TODO: Need to safely disconnect the call and clear the related call objects
      return;
    }
    if(callobj != NULL) {
      callobj->xGateCallObjInit();
      callobj->callid = sobj->callid;
      callobj->uid = sobj->uid;
      callobj->cp = sobj->cp;
      if(callobj->cp.call_id.empty())
        callobj->cp.call_id  = callobj->callid;
      if(callobj->cp.uid.empty())
        callobj->cp.uid = callobj->uid;
      callobj->cp.m_mappedUserList = sobj->cp.m_mappedUserList;
      callobj->cp.confinfo = sobj->cp.confinfo;
      callobj->sipeventtype = EN_CONF_EVENT_REQ;
      PostMessageToProcessor(callobj);
      sobj->cp.confinfo.PlayFile = "";
      sobj->cp.confinfo.gm_conf_id = "";
      sobj->cp.confinfo.gm_PIN_from_user = "";
      sobj->cp.confinfo.gm_PIN_from_database = "";
      sobj->cp.confinfo.gm_PARTICIPANT_PIN_from_database = "";
      sobj->cp.digitReceived = "";
    }
  }
  catch(...) {
    CLog::Error(THISMODULE, "OnConferenceCallProcess failed due to unknown !");
    //TODO: Need to safely disconnect the call and clear the related call objects
    return;
  }
#endif
}
#else
void SofiaSiphandler::OnConferenceCallProcess(xGateSofiaobj *sobj)
{
  CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceCallProcess  event id %d callid  %s",sobj->cp.confinfo.ConfCallEvent,sobj->cp.call_id.c_str());

  switch(sobj->cp.confinfo.ConfCallEvent) {
    case EN_CONF_NULL:
      {
        return;
      }
    case EN_CONF_WELCOME:
      {
        sobj->cp.confinfo.ConfCallEvent=  EN_CONF_IVRCONNECT;
        //sobj->cp.gm_mgc_ip = GetConfernecMgIp();	
        CLog::Detail(THISMODULE, "SofiaSiphandler::OnConferenceCallProcess  %s Conference MG IP" ,sobj->cp.gm_mgc_ip.c_str());
        RequestForMedia(sobj, EN_MEDIA_CONF);
        return;
      }
    case EN_CONF_IVRCONNECT:
      {
        break;
      }
    case EN_CONF_PLAY_PASSTHROUGH:
      {
        break;
      }
    case EN_CONF_PLAY_PASSTHROUGH_GETDTMF:
      {
        sobj->cp.confinfo.ConfCallEvent = EN_CONF_GETDTMF; // playing file at background and collecting DTMF-digits parallely.
#if 1 //TODO: Yoga introduced this part 
        sobj->cp.confinfo.gm_conf_NoOfValidDialAttempts_by_user--; //TODO: Yoga do we need this any more?
        PlayOnConnect(sobj,EN_FILE_IVR); // gets DTMF-digits during PLAY_PASSTHROUGH
#else //TODO: Yoga disabled this part
        if (sobj->cp.confinfo.ConfDtmfState == EN_CONF_DTMFSTART)
        {
          sobj->cp.confinfo.PlayFile.assign(WAV_FILE_PATH);
#ifdef GET_PIN_ONLY
          sobj->cp.confinfo.PlayFile.append("CR_enter_conf_pin.wav");
#else
          sobj->cp.confinfo.PlayFile.append("CR_enter_conf_id.wav");
#endif;
          if (sobj->cp.confinfo.ConfDtmfModule == EN_CONF_GETPIN)
          {
            sobj->cp.confinfo.PlayFile.assign(WAV_FILE_PATH);
            sobj->cp.confinfo.PlayFile.append("CR_enter_conf_pin.wav");
          }
          sobj->cp.confinfo.DigitTimeOut = 12;
        }

        if (sobj->cp.confinfo.ConfDtmfState == EN_CONF_INVALID)
        {
          sobj->cp.confinfo.gm_conf_NoOfValidDialAttempts_by_user--;
          sobj->cp.confinfo.PlayFile.assign(WAV_FILE_PATH); //TODO: Yoga, commented this
#ifdef GET_PIN_ONLY
          //sobj->cp.confinfo.PlayFile.append("CR_conf_pin_incorrect.wav");
          sobj->cp.confinfo.PlayFile.append("invalid_access_code.wav"); //TODO: Yoga, commented this
#else
          sobj->cp.confinfo.PlayFile.append("CR_provide_valid_conf_id.wav");
#endif;
          if (sobj->cp.confinfo.ConfDtmfModule == EN_CONF_GETPIN)
          {
            sobj->cp.confinfo.PlayFile.assign(WAV_FILE_PATH);
            sobj->cp.confinfo.PlayFile.append("CR_conf_pin_incorrect.wav");
          }
          sobj->cp.confinfo.ConfDtmfState = EN_CONF_DTMFSTART;
        }
        sobj->cp.confinfo.gm_conf_NoOfValidDialAttempts_by_user--;
        if (sobj->cp.confinfo.gm_conf_NoOfValidDialAttempts_by_user == 0)
        {
          //sobj->cp.confinfo.gm_conf_NoOfValidDialAttempts_by_user--;
          sobj->cp.confinfo.ConfDtmfState = EN_CONF_DTMFATTEMPTS_EXHAUSTED;
          sobj->cp.confinfo.PlayFile.assign(WAV_FILE_PATH);
          sobj->cp.confinfo.PlayFile.append("SB_thank_you.wav");
          if (sobj->cp.confinfo.ConfDtmfModule == EN_CONF_CHECKCONFSTATUS)
          {
            sobj->cp.confinfo.PlayFile.assign(WAV_FILE_PATH);
            sobj->cp.confinfo.PlayFile.append("CR_no_conf_scheduled.wav");
          }
          sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY;
          //PlayOnConnect(sobj,EN_FILE_IVR);
          PlayResponse(700,sobj);
          return;
        }
        PlayOnConnect(sobj,EN_FILE_IVR); // gets DTMF-digits during PLAY_PASSTHROUGH
#endif //TODO: Yoga disabled this part
        return;
      }
    case EN_CONF_PLAY:
      if (sobj->cp.confinfo.ConfDtmfState == EN_CONF_NO_DTMF)
      {
        sobj->cp.confinfo.PlayFile.assign(CONF_USER_NO_INPUT_FILE);
        if (sobj->cp.confinfo.ConfDtmfModule == EN_CONF_GETPIN)
          sobj->cp.confinfo.PlayFile.assign(USER_NO_INPUT_ENTER_PIN);
        //sobj->cp.confinfo.ConfDtmfState = EN_CONF_DTMFSTART;
        sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY_PASSTHROUGH_GETDTMF;
      }
      if (sobj->cp.confinfo.ConfDtmfState == EN_CONF_DTMFATTEMPTS_EXHAUSTED)
      {
        if (sobj->cp.confinfo.gm_conf_NoOfValidDialAttempts_by_user == 0)
        {
          sobj->cp.confinfo.gm_conf_NoOfValidDialAttempts_by_user--;
          sobj->cp.confinfo.PlayFile.assign(WAV_FILE_PATH);
          sobj->cp.confinfo.PlayFile.append("CR_Disconnecting.wav");
          if (sobj->cp.confinfo.ConfDtmfModule == EN_CONF_CHECKCONFSTATUS)
          {
            sobj->cp.confinfo.PlayFile.assign(WAV_FILE_PATH);
            sobj->cp.confinfo.PlayFile.append("SB_thank_you.wav");
          }
          sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY;
          //				PlayOnConnect(sobj,EN_FILE_IVR);
          PlayResponse(700,sobj);
          return;
        }
        sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY;
        //PlayOnConnect(sobj,EN_FILE_DISCONNECT);
        PlayResponse(700,sobj);
        sobj->cp.gm_userDiallDTMFdigits = true;
        return;
      }
      PlayOnConnect(sobj,EN_FILE_IVR); // gets DTMF-digits during PLAY_PASSTHROUGH
      break; // inform DBManager to start Timer...
      //return;

    case EN_CONF_PLAY_COMPLET:
      //sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY_COMPLET;
      if (sobj->cp.confinfo.ConfDtmfState == EN_CONF_DTMFATTEMPTS_EXHAUSTED)
      {
        if (sobj->cp.confinfo.gm_conf_NoOfValidDialAttempts_by_user == 0)
        {
          sobj->cp.confinfo.gm_conf_NoOfValidDialAttempts_by_user--;
          sobj->cp.confinfo.PlayFile.assign(WAV_FILE_PATH);
          sobj->cp.confinfo.PlayFile.append("CR_Disconnecting.wav");
          sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY;
          //PlayOnConnect(sobj,EN_FILE_IVR);
          PlayResponse(700,sobj);
          return;
        }
        sobj->cp.confinfo.ConfCallEvent = EN_CONF_CALLDISCONNECT;
        //PlayOnConnect(sobj,EN_FILE_DISCONNECT);
        PlayResponse(700,sobj);
        //SofiaSendBye(sobj->sofianh);
        sobj->cp.gm_userDiallDTMFdigits = true;

        // cleanuo code for conference
        DeleteCallFromConferenceList(sobj->uid,sobj);
        sobj->cp.confinfo.noOfConfParticipantsCurrentlyJoined--;
        sobj->cp.confinfo.isConfLocked = false;
        return;
      }
      if (sobj->cp.confinfo.ConfDtmfState == EN_CONF_ADDPARTICIPANT)
      {
        sobj->cp.confinfo.PlayFile.assign(WAV_FILE_PATH);
        sobj->cp.confinfo.PlayFile.append("beep-01a.wav");
        sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY;
        PlayOnConnect(sobj,EN_FILE_IVR);
        return;
      }
      if (sobj->cp.confinfo.ConfDtmfModule == EN_CONF_HOSTCONTROL)
      {
        sobj->cp.confinfo.ConfCallEvent = EN_CONF_GETDTMF; // generally PLAY_PASSTHROUGH is associated with GETDTMF.
      }
      break;

    case EN_CONF_GETDTMF:
      CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceCallProcess  Wating for MAX Digit %d",sobj->cp.confinfo.MaxDigit);
      sobj->cp.confinfo.ConfCallEvent = EN_CONF_GETDTMF;
      return;

      //case EN_CONF_VALIDATE_CONF_ID_GETDTMF_COMPLET:
    case EN_CONF_GETDTMF_COMPLET:
      CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceCallProcess  DTMF Digit %s",sobj->cp.confinfo.DigitReceived.c_str());
      sobj->cp.csfilename = "";
      sobj->cp.confinfo.PlayFile = "";
      sobj->cp.confinfo.ConfCallEvent = EN_CONF_GETDTMF_COMPLET;
      sobj->cp.gm_userDiallDTMFdigits = true;

      //	xGateSofiaobj* saved_sobj_2;
      //	if (!sobj->cp.confinfo.gm_conf_id.empty())
      //	{
      //		saved_sobj_2 = GetSofiaObj_FromFirstConferenceCall(sobj->cp.confinfo.gm_conf_id.c_str());
      //		if (saved_sobj_2)
      //			sobj->cp.g_noOfConfParticipantsCurrentlyJoined = saved_sobj_2->cp.g_noOfConfParticipantsCurrentlyJoined;
      //	}

      /*
         if (sobj->cp.confinfo.ConfDtmfModule == EN_CONF_INITIATED)
         {// Collect DTMF-digits for HOST-control commands...
         processHostControlCommand(sobj);
         sobj->cp.confinfo.ConfCallEvent = EN_CONF_GETDTMF; // media should always be in DTMF-collect mode.
         return;
         }
         */
      if (sobj->cp.confinfo.ConfDtmfModule == EN_CONF_HOSTCONTROL)
      {// Collect DTMF-digits for HOST-control commands...
        //processHostControlCommand(sobj);
        sobj->cp.confinfo.ConfCallEvent = EN_CONF_GETDTMF; // media should always be in DTMF-collect mode.
        return;
      }
      break;

    case EN_CONF_DIAL:
      DialOut(sobj);
      sobj->cp.confinfo.ConfCallEvent = EN_CONF_CALLCONNECTED; // added for tsting; to be removed later...
      return;
    case EN_CONF_CALLCONNECTED:
      break;
    case EN_CONF_DIALDEFAULT:
      break;
    case EN_CONF_CALLDISCONNECT:
      {
        CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceCallProcess : case EN_CONF_CALLDISCONNECT:");
        PlayResponse(700,sobj);
        return;
      }
    case EN_CONF_CALLDISCONNECTED:
      break;
    case EN_CONF_HOLD:
      sobj->cp.PbState = EN_XGATE_PB_HOLD;
      sobj->cp.csfilename =  sobj->cp.confinfo.PlayFile;
      sobj->cp.confinfo.ConfCallEvent = EN_CONF_HOLD;
      RequestForMedia(sobj, EN_MEDIA_HOLD_RESUME);
      OnHold_Resume(sobj);
      //RespondToPeer(EN_200_REL,sobj);
      break;
    case EN_CONF_RESUME:
      sobj->cp.PbState = EN_XGATE_PB_RESUME;
      sobj->cp.csfilename =  sobj->cp.confinfo.PlayFile;
      sobj->cp.confinfo.ConfCallEvent = EN_CONF_RESUME;
      RequestForMedia(sobj, EN_MEDIA_HOLD_RESUME);
      RespondToPeer(EN_200_REL,sobj);
      break;
    case EN_CONF_MUTE:
      sobj->cp.PbState = EN_XGATE_PB_MUTE;
      sobj->cp.csfilename =  sobj->cp.confinfo.PlayFile;
      sobj->cp.confinfo.ConfCallEvent = EN_CONF_MUTE;
      RequestForMedia(sobj, EN_MEDIA_MUTE);
      RespondToPeer(EN_200_REL,sobj);
      break;
    case EN_CONF_UNMUTE:
      sobj->cp.PbState = EN_XGATE_PB_UNMUTE;
      sobj->cp.csfilename =  sobj->cp.confinfo.PlayFile;
      sobj->cp.confinfo.ConfCallEvent = EN_CONF_UNMUTE;
      RequestForMedia(sobj, EN_MEDIA_UNMUTE);
      RespondToPeer(EN_200_REL,sobj);
      break;
    case EN_CONF_DEAF:
      sobj->cp.PbState = EN_XGATE_PB_DEAF;
      sobj->cp.csfilename =  sobj->cp.confinfo.PlayFile;
      sobj->cp.confinfo.ConfCallEvent = EN_CONF_DEAF;
      RequestForMedia(sobj, EN_MEDIA_DEAF);
      break;
    case EN_CONF_LOCK:
      sobj->cp.PbState = EN_XGATE_PB_LOCK;
      sobj->cp.csfilename =  sobj->cp.confinfo.PlayFile;
      sobj->cp.confinfo.ConfCallEvent = EN_CONF_LOCK;
      break;
    case EN_CONF_UNLOCK:
      sobj->cp.PbState = EN_XGATE_PB_UNLOCK;
      sobj->cp.csfilename =  sobj->cp.confinfo.PlayFile;
      sobj->cp.confinfo.ConfCallEvent = EN_CONF_UNLOCK;
      break;
    case EN_CONF_EXTEND:
      sobj->cp.PbState = EN_XGATE_PB_EXTEND;
      sobj->cp.csfilename =  sobj->cp.confinfo.PlayFile;
      sobj->cp.confinfo.ConfCallEvent = EN_CONF_EXTEND;
      break;
    case EN_CONF_TRANSFER:
      sobj->cp.PbState = EN_XGATE_PB_TRANSFER;
      sobj->cp.csfilename =  sobj->cp.confinfo.PlayFile;
      sobj->cp.confinfo.ConfCallEvent = EN_CONF_TRANSFER;
      break;
    case EN_CONF_END:
      sobj->cp.PbState = EN_XGATE_PB_CONF_END;
      sobj->cp.csfilename =  sobj->cp.confinfo.PlayFile;
      sobj->cp.confinfo.ConfCallEvent = EN_CONF_END;
      RequestForMedia(sobj, EN_MEDIA_CONF_END);
      DeleteConferenceList(sobj->uid);
      break;

    case EN_CONF_ADDPARTICIPANT:{
                                  CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceCallProcess : case EN_CONF_ADDPARTICIPANT:noOfConfParticipantsCurrentlyJoined = %d",sobj->cp.confinfo.noOfConfParticipantsCurrentlyJoined);
                                  //sobj->uid = sobj->cp.confinfo.gm_mgResourceId;
                                  InsertCalltoConferenceList(sobj->cp.confinfo.gm_mgResourceId, sobj);
                                  //sobj->cp.csfilename = TEM_DEFAULT_HOLD;
                                  sobj->cp.csfilename = "";
                                  //sobj->cp.confinfo.ConfDtmfModule = EN_CONF_INITIATED;	
                                  sobj->cp.confinfo.ConfDtmfModule = EN_CONF_HOSTCONTROL;
                                  sobj->cp.confinfo.ConfDtmfState = EN_CONF_ADDPARTICIPANTSENDMESSAGE10;
                                  sobj->cp.confinfo.ConfCallEvent = EN_CONF_NULL;
                                  sobj->cp.confinfo.MaxConfParticipants = CONF_MAX_PARTICIPANTS;
                                  RequestForMedia(sobj,EN_MEDIA_CONF_ADDPARTICIPANT);
                                  char TotalJoine[100] = "\0";
                                  sprintf(TotalJoine,"/root/wav_files/sounds/vectone/conf/%d_participant.wav",  sobj->cp.confinfo.noOfConfParticipantsCurrentlyJoined);
                                  sobj->cp.confinfo.PlayFile = TotalJoine;
                                  //sobj->cp.csfilename= TotalJoine;
                                  sobj->cp.confinfo.ConfCallEvent = EN_CONF_IVRPLAY;
                                  PlayOnConnect(sobj,EN_FILE_IVR);
                                }
                                return;
    case EN_CONF_IVRPLAY:{
                           CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceCallProcess : EN_CONF_IVRPLAY uid %s -- callid -%s",sobj->uid.c_str(),sobj->callid.c_str());
                           if(sobj->cp.confinfo.gm_conf_host_user){
                             OnConferenceHostJoin(sobj);
                           }
                           else
                             OnConferenceParticipantJoin(sobj);
                           return;
                         }

                         // fallthru without adding break
#if 0
    case EN_CONF_CONFROOMHANDLERECVD:
                         // else conference-handle is already created.
                         CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceCallProcess : EN_CONF_CONFROOMHANDLERECVD");
                         InsertCalltoConferenceList(sobj->cp.confinfo.gm_conf_id.c_str(),sobj);

                         xGateSofiaobj* saved_sobj;
                         saved_sobj = GetSofiaObj_FromFirstConferenceCall(sobj->cp.confinfo.gm_conf_id.c_str());
                         if (saved_sobj)
                         {
                           (saved_sobj->cp.g_noOfConfParticipantsCurrentlyJoined)++;
                           sobj->cp.g_noOfConfParticipantsCurrentlyJoined = saved_sobj->cp.g_noOfConfParticipantsCurrentlyJoined;
                         }
                         sobj->cp.confinfo.ConfDtmfModule = EN_CONF_INITIATED;
                         sobj->cp.confinfo.ConfDtmfState = EN_CONF_ADDPARTICIPANTPLAYIVR;
                         sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY;

                         sobj->uid = GetUid_FromFirstConferenceCall(sobj->cp.confinfo.gm_conf_id.c_str());
                         sobj->uid.assign(sobj->cp.confinfo.gm_mgResourceId.c_str());

                         sobj->cp.confinfo.PlayFile.assign(WAV_FILE_PATH);
                         sobj->cp.confinfo.PlayFile.append("CR_entered.wav");
                         //sobj->cp.confinfo.PlayFile.assign("/root/wav_files/sounds/vectone/beep-01a.wav");
                         sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY;
                         //PlayOnConnect(sobj,EN_FILE_IVR);
                         return;
                         break;
#endif

                         /*
                            case EN_CONF_CONFROOMHANDLERECVD:
                            CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceCallProcess : case EN_CONF_CONFROOMHANDLERECVD:");
                         // Play the 2 files... CR_entered.wav and beep-01a.wav
                         sobj->cp.confinfo.PlayFile.assign("/root/wav_files/sounds/vectone/CR_entered.wav");
                         //sobj->cp.confinfo.PlayFile.assign("/root/wav_files/sounds/vectone/beep-01a.wav");
                         sobj->cp.confinfo.ConfCallEvent = EN_CONF_PLAY;
                         PlayOnConnect(sobj,EN_FILE_IVR);
                         RequestForMedia(sobj,EN_MEDIA_CONF_ADDPARTICIPANT);
                         return;
                         break;
                         */

    case EN_CONF_DELPARTICIPANT:
                         //CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceCallProcess : case EN_CONF_DELPARTICIPANT:");

                         sobj->cp.csfilename =  "/root/wav_files/sounds/vectone/conf/conf_exit.wav";
                         sobj->cp.PbState = EN_XGATE_PB_CONFALL;
                         RequestForMedia(sobj,EN_MEDIA_UPDATE);
                         CLog::Detail(THISMODULE,"SofiaSiphandler::OnConferenceCallProcess : Play ALL exit conference %s", sobj->cp.csfilename.c_str());
                         if(sobj->cp.confinfo.gm_conf_host_user){
                           // Delete all the peer
                           //g_noOfConfParticipantsCurrentlyJoined

                           // RequestForMedia(sobj,EN_MEDIA_DELETE);
                           RequestForMedia(sobj,EN_MEDIA_CONF_DELPARTICIPANT);
                           //  sobj->cp.g_noOfConfParticipantsCurrentlyJoined = 0;

                         } 
                         else{
                           //if(sobj->cp.g_noOfConfParticipantsCurrentlyJoined > 0){
                           //sobj->cp.confinfo.gm_conf_participant_user= 1;
                           RequestForMedia(sobj,EN_MEDIA_CONF_DELPARTICIPANT);
                           //RequestForMedia(sobj,EN_MEDIA_DELETE);
                           //sobj->cp.g_noOfConfParticipantsCurrentlyJoined--;
                         }

                         DeleteCallFromConferenceList(sobj->cp.confinfo.gm_mgResourceId,sobj);
                         break;

                           default:
                         return;

                         }
                         xGateCallObj *callobj = new xGateCallObj;
                         callobj->xGateCallObjInit();
                         callobj->callid = sobj->callid;
                         callobj->uid = sobj->uid;
                         callobj->cp = sobj->cp;
                         if(callobj->cp.call_id.empty())
                           callobj->cp.call_id  = callobj->callid;
                         if(callobj->cp.uid.empty())
                           callobj->cp.uid = callobj->uid;
                         callobj->cp.m_mappedUserList = sobj->cp.m_mappedUserList;
                         callobj->cp.confinfo = sobj->cp.confinfo;
                         /*
                            callobj->cp.confinfo.ConfCallEvent = sobj->cp.confinfo.ConfCallEvent;
                            callobj->cp.confinfo.ConfDtmfState = sobj->cp.confinfo.ConfDtmfState;
                            callobj->cp.confinfo.ConfDtmfModule = sobj->cp.confinfo.ConfDtmfModule;
                            callobj->cp.confinfo.MaxDigit = sobj->cp.confinfo.MaxDigit;
                            callobj->cp.confinfo.conf_timer_thread_1 = sobj->cp.confinfo.conf_timer_thread_1;
                            callobj->cp.confinfo.conf_timer_thread_2 = sobj->cp.confinfo.conf_timer_thread_2;
                            callobj->cp.confinfo.conf_callFlowNo =  sobj->cp.confinfo.conf_callFlowNo;
                            */
                         callobj->sipeventtype = EN_CONF_EVENT_REQ;
                         /*
                            callobj->cp.confinfo.PlayFile = sobj->cp.confinfo.PlayFile;
                            callobj->cp.confinfo.MaxDigit = sobj->cp.confinfo.MaxDigit;
                            callobj->cp.confinfo.gm_conf_id = sobj->cp.confinfo.gm_conf_id;
                            callobj->cp.confinfo.gm_PIN_from_user = sobj->cp.confinfo.gm_PIN_from_user;
                            callobj->cp.confinfo.gm_PIN_from_database = sobj->cp.confinfo.gm_PIN_from_database;
                            callobj->cp.confinfo.gm_PARTICIPANT_PIN_from_database = sobj->cp.confinfo.gm_PARTICIPANT_PIN_from_database;
                            callobj->cp.confinfo.DigitReceived = sobj->cp.confinfo.DigitReceived;
                            */

                         PostMessageToProcessor(callobj);

                         sobj->cp.confinfo.PlayFile = "";
                         sobj->cp.confinfo.gm_conf_id = "";
                         sobj->cp.confinfo.gm_PIN_from_user = "";
                         sobj->cp.confinfo.gm_PIN_from_database = "";
                         sobj->cp.confinfo.gm_PARTICIPANT_PIN_from_database = "";
                         sobj->cp.confinfo.DigitReceived = "";
  }
#endif

  /**/
#endif //DebugVM end
//DebugVM   RETCALLBACK SofiaSiphandler::SofiaCallBack(nua_event_t   event,  
  void SofiaSiphandler::SofiaCallBack(nua_event_t   event,  
      int           status,  
      char const   *phrase,  
      nua_t       *nua,  
      nua_magic_t  *magic,  
      nua_handle_t *nh,  
      nua_hmagic_t *hmagic,  
      sip_t const  *sip,  
      tagi_t        tags[]){
    // future task move this to seperate function for modulaurity
    XGLOG_INFO("SofiaSiphandler::SofiaCallBack(Event: %s status: %d Des: %s)",nua_event_name(event),status,phrase);
    printf("SofiaSiphandler::SofiaCallBack(Event: %s status: %d Des: %s\n)",nua_event_name(event),status,phrase);
    //  printf("Sofia Event %s Des %s \n",nua_event_name(event),phrase);
    switch(event){
      // Error & Status Handling Event Function

      case nua_i_active:
        break;
#if 0//DebugVM start
      case nua_i_error:
        CLog::Error(THISMODULE,"SofiaSiphandler::SofiaCallBack(Event: %s status: %d Des: %s)",nua_event_name(event),status,phrase);
        break;
      case nua_i_fork :
        break;
      case nua_i_media_error :
        break;
      case nua_i_subscription :
        break;
#endif //DebugVM end
      case nua_i_state :
        ((SofiaSiphandler*)magic)->OnStateChange(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;
#if 0//DebugVM start
      case nua_i_terminated:
        break;
        //................
        // SIP Request Event Handler Function
      case nua_i_ack :
        break;
      case nua_i_bye :
        ((SofiaSiphandler*)magic)->OnByeRequest(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;
      case nua_i_cancel :
        ((SofiaSiphandler*)magic)->OnInviteCancel(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;
      case nua_i_chat :
        break;
#endif //DebugVM end
      case nua_i_info :
        ((SofiaSiphandler*)magic)->OnInfoRequest(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;
      case nua_i_invite :
        XGLOG_INFO("ippbx-multithreaded : case nua_i_invite : ");
        ((SofiaSiphandler*)magic)->OnInviteRequest(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;
//DebugVM start
      case nua_i_bye :
        ((SofiaSiphandler*)magic)->OnByeRequest(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;
      case nua_i_subscribe :
        break;

//DebugVM end
#if 0//DebugVM start
      case nua_i_message :
        ((SofiaSiphandler*)magic)->OnMessageRequest(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;
      case nua_i_method :
        break;
#endif //DebugVM end
      case nua_i_notify :
        ((SofiaSiphandler*)magic)->OnNotifyRequest(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;
#if 0//DebugVM start
      case nua_i_options :
        break;
      case nua_i_prack :
        break;
      case nua_i_publish :
        break;
      case nua_i_refer :
        ((SofiaSiphandler*)magic)->OnReferRequest(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;
#endif
      case nua_i_register :
        ((SofiaSiphandler*)magic)->OnRegistrationRequest(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;
#if 0
      case nua_i_subscribe :
        break;
      case nua_i_update:
        break;
        //--------------------
        //  Response Event Handler Function 
      case nua_r_get_params:
        break;
      case nua_r_notifier :
        break;
      case nua_r_shutdown :
        break;
      case nua_r_terminate:
        break;
        //-----------------------
        // SIP Response Event Handler Function
      case nua_r_bye:
        break; 
      case nua_r_cancel:
        break;  
      case nua_r_info:
        break;  
#endif
      case nua_r_invite:

        ((SofiaSiphandler*)magic)->OnRequestResponse(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;  
#if 0
      case nua_r_message:
        break;  
      case nua_r_notify:
        break;  
      case nua_r_options:
        break;  
      case nua_r_prack:
        break;  
      case nua_r_publish:
        break;  
      case nua_r_refer:
        break;  
#endif //DebugVM end
      case nua_r_register:
        ((SofiaSiphandler*)magic)->OnRegisterResponse(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;  
#if 0 //DebugVM start
      case nua_r_subscribe:
        break;  
      case nua_r_unpublish:
        break;  
      case nua_r_unregister:
        break;  
      case nua_r_unsubscribe:
        break;  
      case nua_r_update:
        break; 
#endif //DebugVM end
      default: //An unknown event can always be handled in the event callback
        XGLOG_INFO(THISMODULE,"SofiaSiphandler::SofiaCallBack UNKOWN EVENT");
#if 0 //DebugVM start
    char contact_uri[500];
    //sprintf(contact_uri,"<sip:%s@%s:%s;%s>",sip->sip_from->a_url->url_user,sip->sip_contact->m_url->url_host,\
           sip->sip_contact->m_url->url_port,sip->sip_contact->m_url->url_params);
    sprintf(contact_uri,"<sip:%s@%s:%s>", "201", "10.22.3.186",\
           "5060");
    //nua_respond(nh,SIP_100_TRYING,SIPTAG_CONTACT_STR(contact_uri),\
           SIPTAG_DATE(sip->sip_date),NUTAG_WITH(nua_saved_event_request((nua_saved_event_t const*)regobj->sofiahmsg)),NUTAG_M_PARAMS(sip->sip_contact->m_url->url_params),TAG_END());
    //SofiaRespond(nh,200,"OK"); //DebugVM
#if 0 //DebugVM start TODO working 200OK
    XGLOG_INFO("SofiaSiphandler:::SofiaCallBack  Sending 200 OK");
    xGateSofiaobj *sofiaobjsiphandle;
    sofiaobjsiphandle = (xGateSofiaobj*)hmagic;
    std::string sdpstr = "v=0\no=user1 53655765 2353687637 IN IP4 10.22.7.78\ns=-\nc=IN IP4 10.22.7.78\nt=0 0\nm=audio 6000 RTP/AVP 8\na=rtpmap: PCMA/8000\na=ptime:20\na=sendrecv\n";
    //nua_respond(nh, 200, "OK",SOATAG_AUDIO_AUX("telephone-event"),
    nua_respond(nh, 200, "OK",SIPTAG_CONTACT_STR(contact_uri), SOATAG_AUDIO_AUX("telephone-event"),
        SOATAG_USER_SDP_STR(sdpstr.c_str()),NUTAG_OFFER_SENT(1),
              NUTAG_AUTOANSWER(0),
        TAG_NULL());
#endif //DebugVM end TODO working 200OK
#endif //DebugVM end
#if 0 //DebugVM start
    xGateSofiaobj *sofiaobjsiphandle;
    sofiaobjsiphandle = (xGateSofiaobj*)hmagic;
        XGLOG_INFO("SofiaSiphandler::OnStateChange : Sending 200 OK");
        sofiaobjsiphandle->localSDPinfo[0].avcodec = sofiaobjsiphandle->remoteSDPinfo[0].avcodec;
        sofiaobjsiphandle->localSDPinfo[1].avcodec = sofiaobjsiphandle->remoteSDPinfo[1].avcodec;
        //RespondToPeer(EN_183_REL, sofiaobjsiphandle);
        //RespondToPeer(EN_180_REL, sofiaobjsiphandle);
        RespondToPeer(EN_200_REL, sofiaobjsiphandle);
#endif //DebugVM end
        if(status < 200 && nua_event_is_incoming_request(event)){
          nua_respond(nh,SIP_501_NOT_IMPLEMENTED, NUTAG_WITH_THIS(nua), TAG_END());
          if(hmagic == NULL)
            nua_handle_destroy(nh);
        }
        break;
    }
  }
  /**/
  void SofiaSiphandler::SofiaRootStep(nua_t* sua_handle,su_root_t* nuaRoot){
    if(sua_handle)
      su_root_step(nuaRoot,0);
  }
  /**/
  void SofiaSiphandler::SofiaSendBye(nua_handle_t *nh){
    nua_bye(nh,TAG_END());
  }
/* OnInfoRequest handels the sip info request from sofia stack event
 *
 * */
void SofiaSiphandler::OnInfoRequest(int status,char const *phrase,nua_t *nua,\
      nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
      tagi_t        tags[]){

  XGLOG_INFO("SofiaSiphandler::OnInfoRequest : Enter function");

  if(sip && sip->sip_payload && sip->sip_payload->pl_data){
	xGateSofiaobj *sofiaobj = NULL;
	if(hmagic){
		sofiaobj = (xGateSofiaobj*)hmagic;
		dtmfHandler(sip->sip_payload->pl_data,sofiaobj);
                //DebugVM start
                // Create a New Call object
                xGateCallObj* callobj = new xGateCallObj;
                callobj->xGateCallObjInit();
                callobj->uid = sofiaobj->uid;
                callobj->cp = sofiaobj->cp;
                callobj->sipeventtype = EN_SIPINFO_REQ;
                callobj->cp.digitReceived = sofiaobj->cp.digitReceived;

                PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);
                //DebugVM end
	}
  }else
	XGLOG_INFO("SofiaSiphandler::OnInfoRequest : SIP-Info message does NOT contain message-body");
}
#if 0//DebugVM start
/* ForwardSipInfo handles the forwading the sip info method to peer without processing
 * */
  xGateReturn SofiaSiphandler::ForwardSipInfo(xGateSofiaobj *sofiaobj,  sip_payload_t *payload){

    CLog::Detail(THISMODULE, "SofiaSiphandler::ForwardSipInfo(%s) Request", sofiaobj->uid.c_str());
    nua_info((nua_handle_t*)sofiaobj->sofianh,SIPTAG_CONTENT_TYPE_STR("application/dtmf-info"),SIPTAG_PAYLOAD(payload), TAG_END());
    //SofiaSendInfo(sofiaobj->SofiaLegB->sofianh);

    return SUCCESS;
  }
#endif //DebugVM end

  void SofiaSiphandler::OnInviteRequest(int status,char const *phrase,nua_t *nua, 
      nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
      tagi_t        tags[])
    {
    // Check the sip information is available if not return as bad request

        if(ValidateSipMessage(sip,SIPVALIDATE::FROM) == false || \
            //  ValidateSipMessage(sip,SIPVALIDATE::REQUEST)== false || 
            ValidateSipMessage(sip,SIPVALIDATE::CONTACT)== false || \
            ValidateSipMessage(sip,SIPVALIDATE::CALLID) == false){
          nua_respond(nh, 400,"Bad Request", TAG_END());
          XGLOG_INFO(THISMODULE,"inside SofiaSiphandler::OnInviteRequest() Header validation failed; call_id = %s", sip->sip_call_id->i_id);
          return;
        }
        XGLOG_INFO(THISMODULE,"SofiaSiphandler::OnInviteRequest() %s:%s",sip->sip_from->a_url->url_user,sip->sip_from->a_url->url_host);
        // Creating call id
        std::string callidstring = CreateUniqueCallid(sip->sip_from->a_url->url_user,\
            sip->sip_from->a_url->url_host,\
            sip->sip_from->a_tag,\
            sip->sip_call_id->i_id);
        callidstring += '\0';

        XGLOG_INFO(THISMODULE,"SofiaSiphandler::OnInviteRequest() callid  %s", callidstring.c_str());

      
        xGateSofiaobj *sofiaobj = NULL ;
        XGATESIPEVENT sipevent; 
        if(hmagic){
          sofiaobj = (xGateSofiaobj*)hmagic;

          XGLOG_INFO("SofiaSiphandler::OnInviteRequest() Re-Invite Request  %s", sofiaobj->uid.c_str());
	
	  sofiaobj->cp.isReInvite = true;//To identify incoming invite as reinvite

          switch(sofiaobj->callstate){
            case EN_CS_PROCEEDING:
            case EN_CS_ALERTING:
              nua_respond(nh, 400,"Bad Request", TAG_END());
              XGLOG_ERROR("inside SofiaSiphandler::OnInviteRequest()call in progress %s", sofiaobj->uid.c_str());
              return;
            case EN_CS_CONNECTED:
	      {
              // Set the call state to CS_Suspendi
              //DebugVM sofiaobj->callstate =  EN_CS_SUSPENDED;
              //DebugVM sofiaobj->mediastate =  EN_MED_SUSPENDED;
              //sipevent = EN_HOLDRESUME_REQ;
              //XGLOG_INFO("SofiaSiphandler::OnInviteRequest() Re-Invite Suspended  %s", sofiaobj->uid.c_str());

              XGLOG_INFO("SofiaSiphandler::OnInviteRequest() Re-Invite Arrived  %s", sofiaobj->uid.c_str());
              if(sip && sip->sip_payload && sip->sip_payload->pl_data)
              {
		      //sofiaobj->cp.sdpStr = sip->sip_payload->pl_data;
		      std::string strPayloadSdp("");
		      copyPayloadData(sip->sip_payload->pl_data, sip->sip_content_length->l_length, strPayloadSdp);
		      sofiaobj->cp.sdpStr = strPayloadSdp.c_str();
		      XGLOG_INFO(THISMODULE, "sdp info value in Re-invite for callid (%s) is (%s)", sofiaobj->uid.c_str(), sofiaobj->cp.sdpStr.c_str());
              }

	      if(sip && sip->sip_unknown)
	      {
		      sip_unknown_t* pUnknown = sip->sip_unknown;
		      while (pUnknown)
		      {
			      if (!strcmp(pUnknown->un_name, "UR-AV-Conference-DstConn-ID") && pUnknown->un_value) {
				      std::string key = pUnknown->un_name;
				      sofiaobj->cp.UrAvConferenceRoomId = pUnknown->un_value;
				      sofiaobj->cp.tUnKnownHeaderMap.insert({key,sofiaobj->cp.UrAvConferenceRoomId});
				      XGLOG_INFO(THISMODULE, "Unknown  header (%s) value is (%s)",key.c_str(), sofiaobj->cp.UrAvConferenceRoomId.c_str());
			      }
			      pUnknown = pUnknown->un_next;
		      }
	      }

              xGateCallObj* callobj = new xGateCallObj;
              if(!callobj){
                      XGLOG_ERROR("SofiaSiphandler::OnInviteRequest failed to create xGateCallObj for '%s'!", sofiaobj->uid.c_str());
              }
              callobj->xGateCallObjInit();
              callobj->uid = sofiaobj->uid;
              callobj->cp = sofiaobj->cp;
              callobj->sipeventtype = EN_REINVITE_REQ;
              PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);

              XGLOG_INFO("SofiaSiphandler::OnInviteRequest triggering EN_REINVITE_REQ for '%s'!", sofiaobj->uid.c_str());

              break;
              }
#if 0 //DebugVM start
            case EN_CS_SUSPENDED:
              // Set The call state to CS_Conecting
              sofiaobj->callstate = EN_CS_CONNECTED;
              sofiaobj->mediastate = EN_MED_RESUMED;
              sipevent = EN_HOLDRESUME_REQ;
              CLog::Detail(THISMODULE,"SofiaSiphandler::OnInviteRequest() Re-Invite Resumed %s" , sofiaobj->uid.c_str());
              break;

            case EN_CS_INIT: // re-invite
#endif //DebugVM end
            default: 
              sipevent  =  EN_REINVITE_REQ;		
              XGLOG_INFO("SofiaSiphandler::OnInviteRequest() Re-Invite on timeout %s" , sofiaobj->uid.c_str());
              break;
          }
          return;
        }

	// Verify PSTN or APP Invite
	// check the domain name from auth list for 407 (APP to PSTN, DESK TO APP, DESK TO APP , APP to DESK ALL case )

	//need to do only the contact address contains sbc ip
	if(sip->sip_from && sip->sip_from->a_url->url_host)
	{
		std::string domainName = sip->sip_from->a_url->url_host;
		if(domainName.find("urvideo.unifiedring.co.uk")!= string::npos)
		{
			if(CheckDomainForCallAuth(sip->sip_from->a_url->url_host) == true){
				XGLOG_WARN(THISMODULE,"SofiaSiphandler::OnInviteRequest(%s)","CheckDomainForCallAuth -- True");
				if(AuthenticateSipSession(status,phrase,nua,magic,nh,hmagic,sip,tags)== true )
				{
					sofiaobj  = CreateSofiaobjparam(callidstring,nh,magic,(sip_t *)sip);
				}
				return ;
			}
		}
		else 
		{		
			sofiaobj  = CreateSofiaobjparam(callidstring,nh,magic,(sip_t *)sip);
          		XGLOG_INFO("SofiaSiphandler::OnInviteRequest no need to authentication for invite for (%s) for uid (%s)",domainName.c_str(),sofiaobj->uid.c_str());
		}
	}

	if(!sofiaobj){
          XGLOG_INFO("SofiaSiphandler::OnInviteRequest ProcessInvite called sofiaobj is empty");
	  return;
	}

        //Fill IpAddress family
        if (sip && sip->sip_contact && sip->sip_contact->m_url && sip->sip_contact->m_url->url_host)
        {
                std::string strIpAddr = "";
                strIpAddr = sip->sip_contact->m_url->url_host;

                if (ip_validator(strIpAddr)==true)
                {
                        sofiaobj->m_enumCallLegIpAddrFamily = IP_ADDR_V6;
                        XGLOG_INFO( "SofiaSiphandler::OnInviteRequest On CallId (%s) NEW CALL From  (%s) ON IPADD [V6] ",sofiaobj->uid.c_str(),(sip->sip_from->a_url && sip->sip_from->a_url->url_user) ? sip->sip_from->a_url->url_user : "");
                }
                else
                {
                        sofiaobj->m_enumCallLegIpAddrFamily = IP_ADDR_V4;
                        XGLOG_INFO( "SofiaSiphandler::OnInviteRequest On CallId (%s) NEW CALL From  (%s) ON IPADD [V4] ",sofiaobj->uid.c_str(),(sip->sip_from->a_url && sip->sip_from->a_url->url_user) ? sip->sip_from->a_url->url_user : "");

                }
        }

        XGLOG_INFO("SofiaSiphandler::OnInviteRequest ProcessInvite called %s",sofiaobj->uid.c_str());
        sofiaobj->sipeventtype = EN_INVITE_REQ;
        ProcessInviteRequest(nh,sip,tags,sofiaobj);

        //Fetching Target value start
        XGLOG_INFO("SofiaSihandler::OnInvite Call Transfer call uid %s", sip->sip_request->rq_url->url_params);
        char const *params= sip->sip_request->rq_url->url_params;
        char *o_target;
        char *o_cause;
        int len1 = url_param(params, "target", NULL, 0);
        int len2 = url_param(params, "cause", NULL, 0);

        if(len1 > 0)
        {
          o_target = new char[len1+1];
          if(o_target != NULL)
          {
	     url_param(params, "target", o_target, len1+1);
             XGLOG_INFO("SofiaSihandler::OnInvite Call Transfer call uid %s", o_target);
             sofiaobj->target = o_target;
          }
        }

        if(len2 > 0)
        {
          o_cause = new char[len2+1];
          if(o_cause != NULL)
          {
       	     url_param(params, "cause", o_cause, len2+1);
             XGLOG_INFO("SofiaSihandler::OnInvite Call Transfer call uid %s", o_cause);
             sofiaobj->cause = o_cause;
          }
        }
  }
  

//Processing Invite Challenge

bool SofiaSiphandler::AuthenticateSipSession(int status,char const *phrase,nua_t *nua,\
    nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
    tagi_t        tags[]){

  xGateSofiaSipRegobj *sipregobj = new xGateSofiaSipRegobj();
  CreateSofiaRegobj(*sipregobj,sip);
  XGLOG_INFO("SofiaSiphandler","AuthenticateSipSession(%s)",sipregobj->callid.c_str());
  sipregobj->Reghandle = (void*) nh;
  sipregobj->sofiamh = (void*)magic;
  sipregobj->siptags = (void*)tags;
  bool ret = false;

  if(sip->sip_proxy_authorization){ // Request has auth paramaters

    //Post message to DB thread
    if (sip \
        && sip->sip_request \
        && sip->sip_request->rq_url\
        && sip->sip_request->rq_url->url_host){
      if(CheckDomainForCallAuth(sip->sip_request->rq_url->url_host) == false)
       if( sip \
        && sip->sip_from \
        && sip->sip_from->a_url\
        && sip->sip_from->a_url->url_host){
       sipregobj->domainname = sip->sip_from->a_url->url_host;
     }
    }
    sipregobj->Regevent = EN_AUTHCALL;
    xGateCallObj* callobj = new xGateCallObj;
    callobj->m_regObj = sipregobj;
    callobj->sipeventtype = EN_REGISTRATION_REQ;
    PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);

    XGLOG_INFO("SofiaSiphandler","AuthenticateSipSession(%s) - Verification Request to DB Manager",sipregobj->callid.c_str());
    ret = true;
  }else{
    // Send 407
    XGLOG_INFO("SofiaSiphandler","AuthenticateSipSession(%s) - Sending 407 proxy authentication required details back",sipregobj->callid.c_str());
    sipregobj->Regevent = EN_SEND407;
    RegistarResponse(sipregobj);

  }
  return ret;
}


   bool SofiaSiphandler::ProcessAuthCallInviteRequest(nua_handle_t *nh,sip_t const  *sip,tagi_t tags[],xGateSofiaobj *sobj)
  {
   XGLOG_WARN("SofiaSiphandler::ProcessInviteRequest(%s)",sobj->uid.c_str());

    // Create a New Call object Since it is new invite found
    xGateCallObj* callobj = new xGateCallObj;
    if(!callobj){
      XGLOG_ERROR("SofiaSiphandler::ProcessInviteRequest failed to create xGateCallObj for '%s'!",sobj->uid.c_str());
    }

    CreateCallobjparam(callobj,sobj,nh,sip,tags);
    // Post Message
    callobj->sipeventtype = EN_INVITE_REQ;

    PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);

    return true;
  }


    void SofiaSiphandler::ProcessInviteRequest(nua_handle_t *nh,sip_t const  *sip,tagi_t tags[],xGateSofiaobj *sobj){


    XGLOG_WARN("SofiaSiphandler::ProcessInviteRequest(%s)",sobj->uid.c_str());

    // Create a New Call object Since it is new invite found
    xGateCallObj* callobj = new xGateCallObj;
    if(!callobj){
      XGLOG_ERROR("SofiaSiphandler::ProcessInviteRequest failed to create xGateCallObj for '%s'!",sobj->uid.c_str());
    }

    CreateCallobjparam(callobj,sobj,nh,sip,tags);
    // Post Message
    callobj->sipeventtype = EN_INVITE_REQ;

    delete callobj;
    callobj = NULL;

#if 0
    //DebugVM start TODO Send msg to IVR Controller
    MediaRequest(sobj, sip, XGATEMGUPDATE::EN_MEDIA_CREATE);

    XGLOG_INFO("SofiaSiphandler::ProcessInviteRequest Call-id=%s", sip->sip_call_id->i_id);
    //duplicate the sdp structure to sofia object void pointer
    const sdp_session_t* lsdp = NULL;
    const sdp_session_t* rsdp = NULL;
    tl_gets(tags,NUTAG_CALLSTATE_REF(sobj->Sipcallstate),SOATAG_LOCAL_SDP_REF(lsdp),SOATAG_REMOTE_SDP_REF(rsdp),TAG_END());
    
    xGateSDPInfo sdpinfo = sobj->remoteSDPinfo[0];
    //sdp_session_t* SofiaRemotesdp =  sdp_session_dup(&m_homeMemory,rsdp);
    //sdp_media_t* media =  rsdp->sdp_media;
    //XGLOG_INFO("SofiaSiphandler::ProcessInviteRequest mport=%d", media->m_port);
    XGLOG_INFO("SofiaSiphandler::ProcessInviteRequest mport=%d", sdpinfo.port);
#endif    
    //DebugVM end

    //debugVM TEMP PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);
  }

char* SofiaSiphandler::deblank(char* input)
{
    int i,j;
    char *output=input;
    for (i = 0, j = 0; i<strlen(input); i++,j++)
    {
        if (input[i]!=' ')
            output[j]=input[i];
        else
            j--;
    }
    output[j]=0;
    return output;
}

void SofiaSiphandler::auth_MD5(nua_handle_t *nh,sip_t const *sip)
{
    SipModuleProfile sipProf = URENGINE_CONFIG::instance()->getSipModConfig();
    //SipModuleProfile sipProf = IVR_CONFIG::instance()->getSipModConfig();

    sip_www_authenticate_t const *wa = sip->sip_www_authenticate;
    sip_proxy_authenticate_t const *pa = sip->sip_proxy_authenticate;
    const char *method = NULL;
    const char *realm = NULL;

    char auth[100]="";

    if (wa) {
        realm = msg_params_find(wa->au_params, "realm=");
        method = wa->au_scheme;
          XGLOG_INFO("Sofia_Debug :>>>>>>>>>>>>>> sip >>>>> ***2 realm auth_MD5 wa auth is");
    }

     if (pa) {
        realm = msg_params_find(pa->au_params, "realm=");
        method = pa->au_scheme;
          XGLOG_INFO("Sofia_Debug :>>>>>>>>>>>>>> sip >>>>> ***2 realm auth_MD5 proxy auth");
    }


    if (realm == NULL)
    {
        return;
    }

//char *username = deblank(wAccount.user);
//char *passwd = deblank(wAccount.password);
//char *username = "200"; //deblank(wAccount.user);
//char *passwd = "NXXOSG73ZY30CAM"; //deblank(wAccount.password);
char *username = deblank((char*)sipProf.registerUsername.c_str());
char *passwd = deblank((char*)sipProf.registerPassword.c_str());

    sprintf(auth,"%s:%s:%s:%s",
            method, realm, (const char*)username ,(const char*)passwd);
   //XGLOG_INFO("Sofia_Debug :>>>>>>>>>>>>>> sip >>>>> ***2 realm auth_MD5 auth is %s and size of password is %d\n", auth,sizeof(wAccount.password));
    nua_authenticate(nh, NUTAG_AUTH(auth), TAG_END());
}

  void SofiaSiphandler::OnRegisterResponse(int status,char const *phrase,nua_t *nua, 
      nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
      tagi_t        tags[]){

    XGLOG_INFO("SofiaSiphandler::OnRegisterResponse(%d)",status);
   
    switch(status){
      case 401:
       {
         auth_MD5(nh, sip);
         break;
       }
      case 200:
       {
         XGLOG_INFO("SofiaSiphandler::OnRegisterResponse 200 OK received");
         break;
       }
      default:
       {
         break;
       }
    }
  }

#if 0//DebugVM start
  /**/
  void SofiaSiphandler::OnInviteCancel(int status,char const *phrase,nua_t *nua,\
      nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
      tagi_t        tags[]){


    xGateCallObj *callobj =  new xGateCallObj;
    callobj->xGateCallObjInit();
    callobj->callid = ((xGateSofiaobj*)hmagic)->callid;
    callobj->cp = ((xGateSofiaobj*)hmagic)->cp;
    callobj->sipeventtype = XGATESIPEVENT::EN_TERMINATED_REQ;

    PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);
    // ((SofiaSiphandler*)magic)->DeleteCallInfo((xGateSofiaobj*)hmagic);
    //((SofiaSiphandler*)magic)->DeleteSofiaObject((xGateSofiaobj*)hmagic);
  }
xGateReturn SofiaSiphandler::CallListCancle(std::string key){
        CLog::Detail(THISMODULE,"SofiaSiphandler::CallListCancle Group id %s",key.c_str());
        std::map<std::string,xGateSofiaobj*> *calllist = GetCallList(key);
        if(calllist == NULL)
                return FAIL;
        std::map<std::string,xGateSofiaobj*>::iterator itr = calllist->begin();
        while(itr!=calllist->end()){
                CLog::Detail(THISMODULE,"SofiaSiphandler::CallListCancle Group id %s call id %s callstate %d",key.c_str(), itr->first.c_str(),itr->second->callstate);
                if(itr->second->callstate < EN_CS_CONNECTED  && itr->second->callstate > EN_CS_NULL ){
                        auto deleteItr = itr++;
                        //calllist->erase(deleteItr);
                        //calllist->erase(itr);
                if(deleteItr->second->callstate > EN_CS_NULL)
                nua_cancel((nua_handle_t*)deleteItr->second->sofianh,TAG_END());
                calllist->erase(deleteItr);
                CLog::Detail(THISMODULE,"*************Cancel Send**********SIVA ");
                //calllist->erase(itr);
                }else{
                        itr++;
                }

                //if(itr->second->callstate > EN_CS_NULL)
                // nua_cancel((nua_handle_t*)itr->second->sofianh,TAG_END());
        }
        CLog::Detail(THISMODULE,"SofiaSiphandler::CallListCancle Group id %s Call List count %d", key.c_str(), calllist->size());
        return SUCCESS;
}
  /* This functionality will handle the reponse event from Sip Leg's 
   * and post the information to Main or Sip processor thread for further manipulation, 
   * */
#endif
  void SofiaSiphandler::OnRequestResponse(int status,char const *phrase,nua_t *nua, 
      nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
      tagi_t        tags[]){

    if(sip && sip->sip_payload && sip->sip_payload->pl_data)
    {
	    //((xGateSofiaobj*)hmagic)->cp.sdpStr = sip->sip_payload->pl_data;
	    std::string strPayloadSdp("");
	    copyPayloadData(sip->sip_payload->pl_data, sip->sip_content_length->l_length, strPayloadSdp);
	    ((xGateSofiaobj*)hmagic)->cp.sdpStr = strPayloadSdp.c_str();
    }

    //CLog::Detail(THISMODULE,"SofiaSiphandler::OnRequestResponse(%d)",status);
    ((xGateSofiaobj*)hmagic)->siptag = (void*)sip;
    //  ((xGateSofiaobj*)hmagic)->SofiaLegB->sipeventtype= status;
    // Since reply from peer B reply send back to be peer A should be done
    // so SofiaLegB of peer B is peer A
    #if 0
    if(CheckResponseRequired(status,(xGateSofiaobj*)hmagic)== false){
      CLog::Detail(THISMODULE,"SofiaSiphandler::OnRequestResponse(CheckResponseRequired) return false");
      return; 
    }
    #endif
    //   RespondToPeer(status,((xGateSofiaobj*)hmagic)->SofiaLegB);

    xGateCallObj *callobj =  new xGateCallObj;
    // TODO write sperate function to Authentication reply
    if(sip && sip->sip_to && sip->sip_to->a_url && sip->sip_from && sip->sip_call_id)
    {
	    ((xGateSofiaobj*)hmagic)->callid = CreateUniqueCallid(sip->sip_to->a_url->url_user,\
		    sip->sip_from->a_url->url_host,\
		    sip->sip_from->a_tag,\
		    sip->sip_call_id->i_id);
    }
    callobj->callid = ((xGateSofiaobj*)hmagic)->callid;


    callobj->xGateCallObjInit();
    // callobj->callid = ((xGateSofiaobj*)hmagic)->callid;
    callobj->uid = ((xGateSofiaobj*)hmagic)->uid;
    callobj->cp = ((xGateSofiaobj*)hmagic)->cp;
    //callobj->cp.confinfo = ((xGateSofiaobj*)hmagic)->cp.confinfo;
    if(status > 399){
      callobj->sipeventtype = XGATESIPEVENT::EN_TERMINATED_REQ;
    }
    else{
      callobj->sipeventtype = XGATESIPEVENT::EN_INVITE_RES;
    }

    switch(status){
      case 183:
        if(((xGateSofiaobj*)hmagic)->callstate < EN_CS_PROCEEDING){
          ((xGateSofiaobj*)hmagic)->callstate = EN_CS_PROCEEDING;
        }
        break;
      case 180:
        if(((xGateSofiaobj*)hmagic)->callstate < EN_CS_ALERTING){
          ((xGateSofiaobj*)hmagic)->callstate = EN_CS_ALERTING;
        }
        break;;
      case 200:{ 
                 if(((xGateSofiaobj*)hmagic)->callstate <= EN_CS_CONNECTED) { // TODO  need verification the logic yadav
                   ((xGateSofiaobj*)hmagic)->callstate = EN_CS_CONNECTED;

                 }
                 if(InsertConnectedList(((xGateSofiaobj*)hmagic)->uid,((xGateSofiaobj*)hmagic),(XGATEOBJECT)((xGateSofiaobj*)hmagic)->callstream)== false){
                   SofiaSendAck((nua_handle_t*)((xGateSofiaobj*)hmagic)->sofianh);
                   SofiaSendBye((nua_handle_t*)((xGateSofiaobj*)hmagic)->sofianh);
                   return;
                 }

                 //Fix for disconnection issue
                 ((xGateSofiaobj*)hmagic)->sofiamh = (void*)magic;

		 if(sip && sip->sip_payload && sip->sip_payload->pl_data)
		 {
			 //((xGateSofiaobj*)hmagic)->cp.sdpStr = sip->sip_payload->pl_data;
			 std::string strPayloadSdp("");
			 copyPayloadData(sip->sip_payload->pl_data, sip->sip_content_length->l_length, strPayloadSdp);
			 ((xGateSofiaobj*)hmagic)->cp.sdpStr = strPayloadSdp.c_str();
		 }
	
                 // Update Dst Leg info to Org Leg only.
                 if(((xGateSofiaobj*)hmagic)->callstream == EN_OUTBOUND)
                   LinkDstInfoWithOrgInfo(((xGateSofiaobj*)hmagic)->uid); 

                 // LinkPeerWithPeer(((xGateSofiaobj*)hmagic));   

                 // InsertCallInfo(((xGateSofiaobj*)hmagic));
               }

               break;
      case 404:
               //callobj->callcauseB = CC_NoRouteToDestination;
               break;
      case 486:
               // callobj->callcauseB = CC_UserBusy;
               break;
      case 408:
               // callobj->callcauseB = CC_NoUserResponding;
               return;
      case 480:
               // callobj->callcauseB = CC_NoAnswerFromUser;
               break;
      case EN_403_REL:
               // callobj->callcauseB = CC_CallRejected;
               break;	
      case EN_503_REL:
               //callobj->callcauseB = CC_ResourceUnavailable;
               break;
      default:
               // if(status > 399)
               //    callobj->callcauseB = CC_NormalUnspecified;
               break;

    }
    PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);
   #if 0
    if(((xGateSofiaobj*)hmagic)->cp.isPstnNumber == false || status > 399)
      PlayResponse(status,((xGateSofiaobj*)hmagic));
   #endif

  }

  //Added to copy SDP data based on content length
  void SofiaSiphandler::copyPayloadData(std::string strPayloadData, int payloadLen, std::string &strSDP)
  {
    strSDP = strPayloadData.c_str();
    strSDP = strSDP.substr(0,payloadLen);
    return;
  }

  void SofiaSiphandler::OnNotifyRequest(int status,char const *phrase,nua_t *nua,\
      nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
      tagi_t        tags[]){

    if(!hmagic)
      return;
    SofiaRespond(nh,200,"OK");
 }

  void SofiaSiphandler::OnByeRequest(int status,char const *phrase,nua_t *nua,\
      nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
      tagi_t        tags[]){

    if(!hmagic)
      return;
    //SofiaRespond(nh,100,"Trying");
    //SofiaRespond(nh,180,"Ringing");
    SofiaRespond(nh,200,"OK");
#if 0//DebugVM start
    if(((xGateSofiaobj*)hmagic)->cp.isconferenceCall)
    {
      ((xGateSofiaobj*)hmagic)->cp.confinfo.ConfCallEvent = EN_CONF_DELPARTICIPANT; 
      OnConferenceCallProcess(((xGateSofiaobj*)hmagic));
      DeleteConnectedList(((xGateSofiaobj*)hmagic)->uid,((xGateSofiaobj*)hmagic), EN_OT_ORIGINATION);
      return ;
    }
    if(((xGateSofiaobj*)hmagic)->callstream == EN_INBOUND)
      CallListCancle(((xGateSofiaobj*)hmagic)->uid); 

    xGateCallObj *callobj =  new xGateCallObj;
    callobj->xGateCallObjInit();
    callobj->callid = ((xGateSofiaobj*)hmagic)->callid;
    callobj->uid = ((xGateSofiaobj*)hmagic)->uid;
    callobj->sipeventtype = XGATESIPEVENT::EN_BYE_REQ;
    callobj->cp = ((xGateSofiaobj*)hmagic)->cp;
    PostMessage(callobj,EN_PROCESS);

    xGateCallPair *callpair = GetConnectedList(((xGateSofiaobj*)hmagic)->uid);
    xGateSofiaobj *peerLeg = NULL;
    XGATEOBJECT type = EN_OT_NULL;
    if(callpair)
      if(((xGateSofiaobj*)hmagic)->callstream == EN_INBOUND){
        if(callpair->Dstcall)
          peerLeg = callpair->Dstcall;
        type = EN_OT_ORIGINATION;	
      }else if (((xGateSofiaobj*)hmagic)->callstream == EN_OUTBOUND){
        if(callpair->Orgcall)
          peerLeg = callpair->Orgcall;
        type = EN_OT_DESTINATION;
      }
    if(peerLeg)
      SofiaSendBye(peerLeg->sofianh);

    DeleteConnectedList(((xGateSofiaobj*)hmagic)->uid,((xGateSofiaobj*)hmagic),type);
    RequestForMedia(((xGateSofiaobj*)hmagic),EN_MEDIA_DELETE);
#endif //DebugVM end
  }
#if 0//DebugVM start

  void SofiaSiphandler::RequestTermination(xGateSofiaobj *sobj){
    xGateCallObj *callobj =  new xGateCallObj;
    callobj->xGateCallObjInit();
    callobj->callid = sobj->callid;
    callobj->uid = sobj->uid;
    callobj->cp = sobj->cp;
    callobj->sipeventtype = XGATESIPEVENT::EN_TERMINATED_REQ;
    //callobj->callstate = CS_Disconnected;
    PostMessage(callobj,EN_PROCESS);

  }
  /**/
#endif //DebugVM end
  void SofiaSiphandler::OnStateChange(int status,char const *phrase,nua_t *nua, 
      nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,  
      tagi_t        tags[]){
    if(!hmagic)
      return;
    xGateSofiaobj *sofiaobjsiphandle;
    //DebugVM sdp_session_t* lsdp = NULL;
    //DebugVM sdp_session_t* rsdp = NULL;
    const sdp_session_t* lsdp = NULL;
    const sdp_session_t* rsdp = NULL;

    sofiaobjsiphandle = (xGateSofiaobj*)hmagic;
    tl_gets(tags,NUTAG_CALLSTATE_REF(sofiaobjsiphandle->Sipcallstate),SOATAG_LOCAL_SDP_REF(lsdp),SOATAG_REMOTE_SDP_REF(rsdp),TAG_END());

    //XGLOG_INFO("SofiaSiphandler::OnStateChange %s of %s",nua_callstate_name(sofiaobjsiphandle->Sipcallstate),\
        sofiaobjsiphandle->uid.c_str());
    //printf("State change %s callid %s\n",nua_callstate_name(sofiaobjsiphandle->Sipcallstate),sofiaobjsiphandle->callid.c_str());
    switch(sofiaobjsiphandle->Sipcallstate){
      case nua_callstate_init:

        break;
#if 0//DebugVM start
      case nua_callstate_authenticating:
        break;
      case nua_callstate_calling: // Invite Sent
        //Get lsdp
        if(lsdp){
          //duplicate the sdp structure to sofia object void pointer
          sdp_session_t* SofiaLocalsdp =	sdp_session_dup(&m_homeMemory,lsdp);
          sofiaobjsiphandle->lsdp = (void*)SofiaLocalsdp;
        }
        break;
      case nua_callstate_proceeding: // 18x Received (for outgoing invite response)
        {
          if(rsdp)
          {
            sdp_session_t* SofiaRemotesdp =  sdp_session_dup(&m_homeMemory,rsdp);
            sofiaobjsiphandle->rsdp = (void*)SofiaRemotesdp;
            //SDPPharser(SofiaRemotesdp,sofiaobjsiphandle->remoteSDPinfo[0]);
            SDPPharser(SofiaRemotesdp,sofiaobjsiphandle);
            RequestForMedia(sofiaobjsiphandle);
            //	  sofiaobjsiphandle->mediastate = EN_MED_SET;
          }
          break;
        } 
#endif //DebugVM end
      case nua_callstate_ready: // ACK send for 2xx received  or ACK received for 2xx sent
        {
		xGateCallObj* callobj = new xGateCallObj;
		if(!callobj){
			XGLOG_ERROR("SofiaSiphandler::OnStateChange failed to create xGateCallObj for '%s'!",sofiaobjsiphandle->uid.c_str());
		}
		callobj->xGateCallObjInit();
		callobj->uid = sofiaobjsiphandle->uid;
		callobj->cp = sofiaobjsiphandle->cp;
		callobj->sipeventtype = EN_200_REL;

		if(callobj->cp.xgateCallFeature == XGATECALLFEATURE::EN_URCBCONF_CALL)	
		{
			if(rsdp){
				sdp_session_t* SofiaRemotesdp =  sdp_session_dup(&m_homeMemory,rsdp);
				sofiaobjsiphandle->rsdp = (void*)SofiaRemotesdp;
				//SDPPharser(SofiaRemotesdp,sofiaobjsiphandle->remoteSDPinfo[0]);
				SDPPharser(SofiaRemotesdp,sofiaobjsiphandle);
				//DebugVM RequestForMedia(sofiaobjsiphandle);
			}
			sofiaobjsiphandle->callstate = EN_CS_CONNECTED;
			callobj->sdpInfo[0] = sofiaobjsiphandle->remoteSDPinfo[0];
			callobj->sdpInfo[1] = sofiaobjsiphandle->remoteSDPinfo[1];
		}
            //Filling Media Details
            //callobj->sdpInfo[0].ipaddress = SofiaRemotesdp->sdp_connection->c_address;
            //callobj->sdpInfo[0].port = SofiaRemotesdp->sdp_media->m_port;
            //callobj->sdpInfo[0].mediatype = SofiaRemotesdp->sdp_media->m_type;
            //callobj->sdpInfo[0].codec = 
            //callobj->sdpInfo[0].cname = 
            //callobj->sdpInfo[0].ptime = SofiaRemotesdp->sdp_media->m_attributes->a_value;

            PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);
            //DebugVM end
          //DebugVN }
          sofiaobjsiphandle->mediastate =  EN_MED_CONNECTED;
          //Send INVITE on Hold
          //SendReInvite(sofiaobjsiphandle);
          //SendRefer(sofiaobjsiphandle);
          break;
        }
#if 0//DebugVM start
      case nua_callstate_completing: // 2xx Received (for outgoing invite response)
        if(rsdp){
          //duplicate the sdp structure to sofia object void pointer
          sdp_session_t* SofiaRemotesdp =  sdp_session_dup(&m_homeMemory,rsdp);
          sofiaobjsiphandle->rsdp = (void*)SofiaRemotesdp;
          // SDPPharser(SofiaRemotesdp,sofiaobjsiphandle->remoteSDPinfo[0]);
          SDPPharser(SofiaRemotesdp,sofiaobjsiphandle);
          RequestForMedia(sofiaobjsiphandle);
        }
        sofiaobjsiphandle->mediastate =  EN_MED_CONNECTED;
        break;
#endif //DebugVM end
      case nua_callstate_received: // Invite Received (incoming call)
        XGLOG_INFO("SofiaSiphandler::OnStateChange : Invite Received (incoming call)");
#if 0
        if(rsdp){
          //duplicate the sdp structure to sofia object void pointer
          sdp_session_t* SofiaRemotesdp =  sdp_session_dup(&m_homeMemory,rsdp);
          sofiaobjsiphandle->rsdp = (void*)SofiaRemotesdp;
          //SDPPharser(SofiaRemotesdp,sofiaobjsiphandle->remoteSDPinfo[0]);
          SDPPharser(SofiaRemotesdp,sofiaobjsiphandle);

          XGLOG_INFO("SofiaSiphandler::OnStateChange : After SDPPharser");

	  xGateCallPair *callpair = GetConnectedList(sofiaobjsiphandle->uid);
	  if(callpair)
	  {
		if(callpair->Orgcall){
			sofiaobjsiphandle = callpair->Orgcall;
		}
	  }
	 
	  if(!sofiaobjsiphandle->cp.isReInvite)
	  { 
	     // Create a New Call object Since it is new invite found
	     xGateCallObj* callobj = new xGateCallObj;
	     if(!callobj){
		XGLOG_ERROR("SofiaSiphandler::OnStateChange failed to create xGateCallObj for '%s'!",sofiaobjsiphandle->uid.c_str());
	     }

	     callobj->xGateCallObjInit();
	     callobj->uid = sofiaobjsiphandle->uid;
	     callobj->cp = sofiaobjsiphandle->cp;
	     callobj->sipeventtype = EN_INVITE_REQ;
	     callobj->sdpInfo[0] = sofiaobjsiphandle->remoteSDPinfo[0];
	     callobj->sdpInfo[1] = sofiaobjsiphandle->remoteSDPinfo[1];

	     //Filling Media Details
	     //callobj->sdpInfo[0].ipaddress = SofiaRemotesdp->sdp_connection->c_address;
	     if(SofiaRemotesdp->sdp_connection != NULL)
	     { 
		callobj->sdpInfo[0].ipaddress = SofiaRemotesdp->sdp_connection->c_address;
	     }
	     else
	     {
		callobj->sdpInfo[0].ipaddress = SofiaRemotesdp->sdp_media->m_connections->c_address;
	     }
	     callobj->sdpInfo[0].port = SofiaRemotesdp->sdp_media->m_port;
	     callobj->sdpInfo[0].mediatype = SofiaRemotesdp->sdp_media->m_type;
	     //callobj->sdpInfo[0].codec = 
	     //callobj->sdpInfo[0].cname = 
	     //callobj->sdpInfo[0].ptime = SofiaRemotesdp->sdp_media->m_attributes->a_value;

	     //Fetching Target and Reason value start
	     callobj->target = sofiaobjsiphandle->target;
	     callobj->cause= sofiaobjsiphandle->cause;
	     //Fetching Targer and Reason value end

	     PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);
	  }
	}	
#else
  //Disabling SOA Layer
    if(!sofiaobjsiphandle->cp.isReInvite)
    {
       // Create a New Call object Since it is new invite found
       xGateCallObj* callobj = new xGateCallObj;
       if(!callobj){
    XGLOG_ERROR("SofiaSiphandler::OnStateChange failed to create xGateCallObj for '%s'!",sofiaobjsiphandle->uid.c_str());
       }

       callobj->xGateCallObjInit();
       callobj->uid = sofiaobjsiphandle->uid;
       callobj->cp = sofiaobjsiphandle->cp;
       callobj->sipeventtype = EN_INVITE_REQ;
       PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);
    }
#endif
        //SofiaRespond(nh,200,"OK"); //DebugVM
        //DebugVM if(sofiaobjsiphandle->mediastate == EN_MED_SUSPENDED || sofiaobjsiphandle->mediastate == EN_MED_RESUMED )
          //DebugVM OnHold_Resume(sofiaobjsiphandle);

        break;
#if 0//DebugVM start
      case nua_callstate_early: // 18x sent with/without  sdp (for incoming call)
        break;
#endif //DebugVM end
      case nua_callstate_completed:  //2xx sent (for incoming call)
        break;
      case nua_callstate_terminating: // BYE send 
      case nua_callstate_terminated: // ACK send for Bye Request
       {
        //DebugVM start
        // Create a New Call object 
        xGateCallObj* callobj = new xGateCallObj;
	if(!callobj){
		XGLOG_ERROR("SofiaSiphandler::OnStateChange failed to create xGateCallObj for '%s'!",sofiaobjsiphandle->uid.c_str());
	}
        callobj->xGateCallObjInit();
        callobj->uid = sofiaobjsiphandle->uid;
        callobj->cp = sofiaobjsiphandle->cp;

	#if 0
	if(status > 399) {
	  XGLOG_INFO("SofiaSiphandler::OnStateChange nua_callstate_terminated received status:(%d) triggering EN_MAKE_CALL_FAILED_RES for ('%s')!",status,sofiaobjsiphandle->uid.c_str());
	  callobj->sipeventtype = EN_MAKE_CALL_FAILED_RES;
	} else {
	  XGLOG_INFO("SofiaSiphandler::OnStateChange nua_callstate_terminated received status:(%d) triggering EN_BYE_REQ for ('%s')!",status,sofiaobjsiphandle->uid.c_str());
          callobj->sipeventtype = EN_BYE_REQ;
        }
	#else
          XGLOG_INFO("SofiaSiphandler::OnStateChange nua_callstate_terminated received status:(%d) triggering EN_BYE_REQ for ('%s')!",status,sofiaobjsiphandle->uid.c_str());
          callobj->sipeventtype = EN_BYE_REQ;
	#endif

        PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);
        //DebugVM end
        
        //if(sofiaobjsiphandle->callstream == EN_INBOUND)
        DeleteConnectedList(sofiaobjsiphandle->uid,sofiaobjsiphandle, EN_OT_ORIGINATION);

        DeleteSofiaObject(sofiaobjsiphandle);
       }
        break;
      default:
        break;
    }

  }



  xGateSofiaobj*  SofiaSiphandler::GetProcessEvent()
  {
    xGateSofiaobj *sobj = NULL;

    xGateCallObj *callObj;
    xGateCallPair *callpair = NULL;
   
   try{
      if((callObj = ReadFromReceiver()) != NULL){

        //if(m_incomingCallHashTable.find(callObj->callid.c_str(),sobj) != -1)
        //if(callObj->uid.empty() && (!callObj->cp.confinfo.gm_conf_valid_pin))
        // callObj->uid =  callObj->callid;
#if 1 //DebugVM start
        if(  callpair = GetConnectedList(callObj->uid))
          sobj = callpair->Orgcall;
#endif //DebugVM end
        // sobj=  GetCallInfo(callObj->callid);
        if(sobj && (SofiaSiphandler*)sobj->sofiamh){
          // sobj->callstate = callstateobj->callstate;
          // sobj->cp = callstateobj->cp;
          //sobj->sipeventtype = callstateobj->sipeventtype;
          ((SofiaSiphandler*)sobj->sofiamh)->OnProcessEvent(sobj,callObj);
          XGLOG_WARN("SofiaSiphandler", "GetProcessEvent  %s" ,callObj->uid.c_str());    
        }
	else
	{
	  XGLOG_INFO( "SofiaSiphandler::GetProcessEvent sobj or sofiamh is NULL :%s", callObj->uid.c_str());
	}

        if(callObj && (callObj->sipeventtype == EN_TERMINATED_REL))
          delete callObj;
      }
    }
    catch(...){
      XGLOG_WARN("SofiaSiphandler", "SetCallState- Exception ");
      return NULL;
    }
    return sobj;
  }

bool SofiaSiphandler::OnRegistrationRequest(int status,char const *phrase,nua_t *nua,\
		nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
		tagi_t        tags[]){

	XGLOG_INFO("SofiaSiphandler", " OnRegistrationRequest");

	//Validating From,To,Contact, CallId Header
	if(ValidateSipMessage(sip,SIPVALIDATE::FROM) == false || \
			ValidateSipMessage(sip,SIPVALIDATE::TO)== false || \
			ValidateSipMessage(sip,SIPVALIDATE::CONTACT)== false || \
			ValidateSipMessage(sip,SIPVALIDATE::CALLID) == false){

		nua_respond(nh, 400,"Bad Request", TAG_END());

		return false;
	}
	// Create the Registration object to post to Registration Manager
	xGateSofiaSipRegobj *sipregobj = (xGateSofiaSipRegobj*)hmagic;

	if(!sipregobj){
		sipregobj = new xGateSofiaSipRegobj();
		CreateSofiaRegobj(*sipregobj,sip);

	}
	sipregobj->Reghandle = (void*) nh;
	sipregobj->sofiamh = (void*)magic;

	// Save  the Registration Event help in reply Since REGISTRATION is not a full dialogi
	// sip stack does not retain the handle on out of scope
	nua_saved_event_t *RegEvent = new nua_saved_event_t;
	nua_save_event(m_nuaHandle,RegEvent);
	sipregobj->sofiahmsg  = (void*)RegEvent;
	//msg_dup(nua_current_request(m_nuaHandle));
	SofiaBindhandle(nh,(nua_hmagic_t*)sipregobj);
	//printf(" Event Occured %s\n",sipregobj->callid.c_str());
	// Post the Message to DB Manager



	if(RegistarResponse(sipregobj) == SUCCESS){
		xGateCallObj* callobj = new xGateCallObj;
		callobj->m_regObj = sipregobj;
		callobj->sipeventtype = EN_REGISTRATION_REQ;
		PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);    
	}

	return true;
}

bool SofiaSiphandler::CreateSofiaRegobj(xGateSofiaSipRegobj &regparam,sip_t const  *sip){

        if(sip && sip->sip_call_id && sip->sip_call_id->i_id ) regparam.callid = sip->sip_call_id->i_id;

        if(sip && sip->sip_contact && sip->sip_contact->m_url){
                if(sip->sip_contact->m_url->url_user) regparam.proxyusername = sip->sip_contact->m_url->url_user;
                if(sip->sip_contact->m_url->url_host) {
                        regparam.ipaddress = sip->sip_contact->m_url->url_host;
                        regparam.contactaddress = sip->sip_contact->m_url->url_host;
                }
                if(sip->sip_contact->m_url->url_port){
                        regparam.contactaddress +=  ":";
                        regparam.contactaddress += sip->sip_contact->m_url->url_port;
                        printf("New Registration contact address %s\n",regparam.contactaddress.c_str());
                }
        }

        regparam.domainname = sip->sip_from->a_url->url_host;
        if(sip && sip->sip_request && sip->sip_request->rq_url){
                if(sip->sip_request->rq_url->url_host)  regparam.domainname = sip->sip_request->rq_url->url_host;
        }

        if(sip && sip->sip_to && sip->sip_to->a_url && sip->sip_to->a_url->url_user) {
                regparam.username = sip->sip_to->a_url->url_user;
                regparam.mobileno = sip->sip_to->a_url->url_user;
        }
        regparam.devicetype = "";
        regparam.apptype = "";
        if( sip->sip_user_agent && sip->sip_user_agent->g_string ){
                if (strstr(sip->sip_user_agent->g_string,"IOS"))
                        regparam.devicetype =  "URIOS";
                else if(strstr(sip->sip_user_agent->g_string,"AND"))
                        regparam.devicetype =  "URAND";

                regparam.apptype = "VIDEO";
        }
#if 0
				if( sip->sip_user_agent && sip->sip_user_agent->g_string ){
                if (strstr(sip->sip_user_agent->g_string,"URAPP"))
                        regparam.devicetype =  "URAPP";
                else
                        regparam.devicetype =  "URDESK";

                if(strstr(sip->sip_user_agent->g_string,"IOS"))
                        regparam.apptype = "IOS";
                else if(strstr(sip->sip_user_agent->g_string,"AND"))
                        regparam.apptype = "AND";
        }
#endif				
        //  regparam.devicetype =  "URAPP";


        if(sip && sip->sip_unknown)
        {
                sip_unknown_t* pUnknown = sip->sip_unknown;
                while (pUnknown)
                {
                        if (!strcmp(pUnknown->un_name, "UR-AV-ConferenceRoom-ID") && pUnknown->un_value) {
                                std::string key = pUnknown->un_name;
                                regparam.confroomid = pUnknown->un_value;
                                regparam.tUnKnownHeaderMap.insert({key,regparam.confroomid});
                                XGLOG_INFO(THISMODULE, "CreateSofiaRegobj Unknown  header (%s) value is (%s)",key.c_str(), regparam.confroomid.c_str());
                        }
                        if (!strcmp(pUnknown->un_name,"UR-AV-ConferenceConn-ID") && pUnknown->un_value) {
                                std::string key = pUnknown->un_name = "UR-AV-ConferenceConn-ID";
                                regparam.connid = pUnknown->un_value;
                                regparam.tUnKnownHeaderMap.insert({key,regparam.connid});
                                XGLOG_INFO(THISMODULE, "CreateSofiaRegobj Unknown  header (%s) value UR-AV-ConferenceConn-ID is (%s)",key.c_str(), regparam.connid.c_str());
                        }

                        pUnknown = pUnknown->un_next;
                }
        }

        regparam.aaa =  EN_AAA_AUTHEN;
        regparam.Regevent = EN_VALIDATE;
        if(sip->sip_authorization && sip->sip_authorization->au_params ){
                regparam.username = msg_params_find(sip->sip_authorization->au_params,"username=");
                regparam.password ="";
                regparam.Regevent = EN_GETUSRPWD;
        }else if(sip->sip_proxy_authorization && sip->sip_proxy_authorization->au_params ){
                regparam.username = msg_params_find(sip->sip_proxy_authorization->au_params,"username=");
                regparam.mobileno = sip->sip_from->a_url->url_user;
                regparam.password ="";
                regparam.Regevent = EN_AUTHCALL;

        }
        regparam.cseq = sip->sip_cseq->cs_seq;
        if(sip->sip_expires){ // TODO need logic to loop all the conatct to be deleted if expire = 0 as per rfc 3261
                regparam.expries =(int) sip->sip_expires->ex_delta;
                if(regparam.expries == 0)
                        regparam.Regevent = EN_GETUSRPWD;
        }
        else if(sip->sip_contact && sip->sip_contact->m_expires){
                regparam.expries =atoi(sip->sip_contact->m_expires);
                if(regparam.expries == 0)
                        regparam.Regevent = EN_GETUSRPWD;
        }else
                regparam.expries = 3600;
        regparam.regsip =  (void*)sip;
        //if(strstr(regparam.apptype.c_str(),"IOS")){
        sip_unknown_t* pUnknown=sip->sip_unknown;
#define UID "iuid"
        while(pUnknown){
                if(!strcmp(pUnknown->un_name,UID)){
                        regparam.message =  pUnknown->un_value;
                        break;
                }
                pUnknown = pUnknown->un_next;
        }
        //}
        return true;
}

xGateReturn SofiaSiphandler::RegistarResponse(xGateSofiaSipRegobj *regobj){
	XGLOG_INFO("SofiaSiphandler", "RegistarReponse(%s)",regobj->callid.c_str());
	xGateReturn ret = SUCCESS;
	sip_t *sip = (sip_t*)regobj->regsip;
	nua_handle_t *nh = (nua_handle_t *)regobj->Reghandle;
	switch(regobj->Regevent)
		{
		case EN_VALIDATE:
				 {
					 char contact_uri[500];
					 sprintf(contact_uri,"<sip:%s@%s:%s;%s>",sip->sip_from->a_url->url_user,sip->sip_contact->m_url->url_host,\
							 sip->sip_contact->m_url->url_port,sip->sip_contact->m_url->url_params);
					 nua_respond(nh,SIP_100_TRYING,SIPTAG_CONTACT_STR(contact_uri),\
							 SIPTAG_DATE(sip->sip_date),NUTAG_WITH(nua_saved_event_request((nua_saved_event_t const*)regobj->sofiahmsg)),NUTAG_M_PARAMS(sip->sip_contact->m_url->url_params),TAG_END());
				 }
				 break;
		case EN_GETUSRPWD:
				{
				 XGLOG_INFO("SofiaSiphandler", "RegistarReponse(%s)-- Get User Name Password for verification",regobj->callid.c_str());
				 break;
				}
		case EN_REGISTER:
		case EN_DEREGISTER:
				{
				 //char reg[15] = "REGISTER";	
				 std::string reg = "REGISTER";	
				 if(VerifyAuthorisationResponce(sip,regobj,reg) == false)
				 {
					 ret = FAIL;
					 XGLOG_INFO("SofiaSiphandler", "RegistarReponse(%s)-- Auth Failed",regobj->callid.c_str());
				 }
				 else
				 {
					 XGLOG_INFO("SofiaSiphandler", "RegistarReponse(%s)-- VerifyAuthorisationResponce Success",regobj->callid.c_str()); 
					 // On Success verfication send 200 ok
					 xGateCallObj* callobj = new xGateCallObj;
					 callobj->sipeventtype = EN_REGISTRATION_REQ;
					 if((regobj->expries) > 0){
						 regobj->Regevent = EN_REGISTER;
					 }else {	
						 regobj->Regevent = EN_DEREGISTER;
					 }
					 callobj->m_regObj = regobj;
					 PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);
					 break; // Break is required only success case
				 }
				 break;
				}
		case EN_REG_FAILED:
		case EN_SEND401:
				{
					char authwww[624] = "";
					char buffer[512] = "";
					mod  = SofiaInitAuth(m_root);
					if(mod)
					{

						auth_generate_digest_nonce (mod,buffer,512,false,msg_now());

						snprintf(authwww, 624, " Digest realm=\"%s\",nonce=\"%s\"",\
								sip->sip_to->a_url->url_host,buffer);

				 		XGLOG_INFO("SofiaSiphandler", "RegistarReponse(%s)-- send 401 unauthorized response",regobj->callid.c_str());
						nua_respond(nh,401,"Unauthorized",SIPTAG_WWW_AUTHENTICATE_STR(authwww),\
								SIPTAG_DATE(sip->sip_date),NUTAG_WITH_THIS(m_nuaHandle),NUTAG_WITH_SAVED((nua_saved_event_t const*)regobj->sofiahmsg),NUTAG_M_PARAMS(sip->sip_contact->m_url->url_params),TAG_END());
						nua_destroy_event((nua_saved_event_t*)regobj->sofiahmsg);
					}
				}
				if(regobj) delete regobj;
				break;


		case EN_REG_SUCESS:{
					   char contact_uri[500];
					   snprintf(contact_uri, sizeof(contact_uri), "<sip:%s@%s:%s;%s>;received=\"sip:%s:%s\"",sip->sip_contact->m_url->url_user,sip->sip_contact->m_url->url_host,\
							   sip->sip_contact->m_url->url_port,sip->sip_contact->m_url->url_params,sip->sip_contact->m_url->url_host,sip->sip_contact->m_url->url_port);
					   std::string contact_addr = "";;
					   sip_contact_t *head = NULL,*loop = NULL;

					   sip_contact_t *ctemp = NULL;


					   std::string aorRecords = regobj->Addressofrecords.c_str();
					   std::string delimiter = "#";

					   int first = 0;

					   size_t pos = 0;
					   std::string token;
					   while ((pos = aorRecords.find(delimiter)) != std::string::npos) {
						   token = aorRecords.substr(0, pos);
						   std::cout << token << std::endl;
                                                   if(first == 0){
                                                           head  =  sip_contact_make(&m_homeMemory, token.c_str());
                                                           head->m_next = NULL;
                                                           loop = head;

                                                   }
                                                   else
                                                   {
                                                           ctemp  =  sip_contact_make(&m_homeMemory, token.c_str());
                                                           ctemp->m_next = NULL;
                                                           loop->m_next = ctemp;
                                                           loop =  ctemp;
                                                   }

                                                   first++;
						   aorRecords.erase(0, pos + delimiter.length());
					   }
					   nua_respond(nh,200,"OK", SIPTAG_CONTACT(head),SIPTAG_DATE(sip->sip_date),NUTAG_WITH_THIS(m_nuaHandle),NUTAG_WITH_SAVED((nua_saved_event_t const*)regobj->sofiahmsg),NUTAG_M_PARAMS(sip->sip_contact->m_url->url_params),TAG_END());
					   nua_destroy_event((nua_saved_event_t*)regobj->sofiahmsg);
				   }

				   if(regobj) delete regobj;
				   break;
		case EN_AUTHCALL:{ // Event will handle invite auth
					 if(VerifyAuthorisationResponce(sip,regobj,"INVITE") == false){
						 XGLOG_INFO("SofiaSiphandler", "RegistarReponse(%s)- Call/Session Auth Verification failed",regobj->callid.c_str());
						 ret = FAIL;
					 }else
					{

						 std::string callidstring = CreateUniqueCallid(sip->sip_from->a_url->url_user,\
								 sip->sip_from->a_url->url_host,\
								 sip->sip_from->a_tag,\
								 sip->sip_call_id->i_id);
						 xGateSofiaobj *sofiaobj = new xGateSofiaobj() ;
						 sofiaobj->callid = callidstring.c_str();
						 sofiaobj->uid = callidstring.c_str();
						 XGLOG_INFO("SofiaSiphandler", "RegistarReponse(%s)- Call/Session Verification Successed",callidstring.c_str());
						 ProcessAuthCallInviteRequest((nua_handle_t*)regobj->Reghandle,sip,(tagi_t*)regobj->siptags,sofiaobj);
						 if(regobj) delete regobj;
						 break;
					 }

				 }
		case EN_SEND407:{
					char proxyauth[624] = "";
					char buffer[512] = "";
					mod  = SofiaInitAuth(m_root);
					if(mod){

						auth_generate_digest_nonce (mod,buffer,512,false,msg_now());

						/*                        sprintf(proxyauth," Digest realm=\"%s\",qop=\"auth\",nonce=\"%s\",opaque=\"%s\",stale=FALSE,algorithm=\"MD5\"",\
									  sip->sip_contact->m_url->url_host,buffer,"");*/
						snprintf(proxyauth, 624, " Digest realm=\"%s\",nonce=\"%s\"",\
								sip->sip_contact->m_url->url_host,buffer);

						nua_respond(nh,407,"Proxy Authentication Required",SIPTAG_PROXY_AUTHENTICATE_STR(proxyauth),\
								SIPTAG_DATE(sip->sip_date),TAG_END());
						XGLOG_INFO("SofiaSiphandler", "RegistarReponse(%s)- 407 Proxy Authentication Required sent",regobj->callid.c_str());
					}
					if(regobj) delete regobj;
				}
				break;
		default:
				break;
	}
	return ret;
}


bool SofiaSiphandler::VerifyAuthorisationResponce(sip_t const *sip, xGateSofiaSipRegobj *regobj,std::string requestmethod)
{
  XGLOG_INFO("SofiaSiphandler", "VerifyAuthorisationResponce(%s)",regobj->callid.c_str());

  auth_response_t ar[1] = {{ 0 }};
  char const *md5 = NULL, *md5sess = NULL, *sha1 = NULL,
       *qop_auth = NULL, *qop_auth_int = NULL;
  struct msg_auth_s *rr  = NULL;
  if(strstr(requestmethod.c_str(),"INVITE")){
    rr = sip->sip_proxy_authorization;
  }
  else
  {
    rr = sip->sip_authorization;
  }
  if(rr)
  {
    auth_get_params(&m_homeMemory, rr->au_params,
        "username=", &ar->ar_username,
        "realm=", &ar->ar_realm,
        "nonce=", &ar->ar_nonce,
        "uri=", &ar->ar_uri,
        "response=", &ar->ar_response,
        "algorithm=", &ar->ar_algorithm,
        "opaque=", &ar->ar_opaque,
        "cnonce=", &ar->ar_cnonce,
        "qop=", &ar->ar_qop,
        "nc=", &ar->ar_nc,
        "algorithm=md5", &md5,
        "algorithm=md5-sess", &md5sess,
        "algorithm=sha1", &sha1,
        "qop=auth", &qop_auth,
        "qop=auth-int", &qop_auth_int,
        NULL);
    auth_hexmd5_t ha1;
    auth_digest_a1(ar,ha1,regobj->password.c_str());

    // Creating  HA2 and responce
    auth_hexmd5_t Regresponce;
    auth_digest_response(ar,Regresponce,ha1,requestmethod.c_str(),NULL,0);
    XGLOG_INFO(THISMODULE, "\n**SofiaSiphandler VerifyAuthorisationResponceUsername = (%s)&& password = (%s)", ar->ar_username,regobj->password.c_str());
    XGLOG_INFO("SofiaSiphandler VerifyAuthorisationResponce(%s)Create Hash %s -- Received Hash %s ",regobj->callid.c_str(),Regresponce,ar->ar_response);
    if(strstr(ar->ar_response, Regresponce) != NULL)
    {
      XGLOG_INFO("SofiaSiphandler VerifyAuthorisationResponce(%s)Create Hash %s -- Received Hash %s both are same",regobj->callid.c_str(),Regresponce,ar->ar_response);
      return true;
    }
  }
  //return false;
  return true;
}


#if 0 //DebugVM start
  /**/
  void SofiaSiphandler::OnMessageRequest(int status,char const *phrase,nua_t *nua,\
      nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
      tagi_t        tags[]){

    CLog::Detail(THISMODULE,"SofiaSiphandler::OnMessageRequest");

    if (sip->sip_payload) {
      CLog::Detail("Message: %s \nLength: %d", sip->sip_payload->pl_data, sip->sip_payload->pl_len);
      //AppDialOnWake(sip->sip_payload->pl_data);
      ProcessMessageRequest(sip);
    }


  }
  void SofiaSiphandler::OnReferRequest(int status,char const *phrase,nua_t *nua,\
      nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
      tagi_t        tags[]){
    xGateSofiaobj *sofiaobj =  (xGateSofiaobj*)hmagic;
    printf("\n Call Transfer call uid on refer %s \n",sofiaobj->uid.c_str());
    CLog::Detail(THISMODULE,"SofiaSihandler::OnReferRequest Call Transfer call uid %s",sofiaobj->uid.c_str());
    xGateCallPair *callpair =  GetConnectedList(sofiaobj->uid);

    if(!callpair)
      return;
    //nua_notify(nh,NUTAG_SUBSTATE(nua_substate_terminated),TAG_END());
    sofiaobj->cp.isTransferenabled = true;
    SofiaSendBye(nh);

    if(callpair->Orgcall) //callpair->Dstcall)
      CallTransfer(callpair->Orgcall,sip,1);
    // This for testing need have better logic yadav TODO
    sofiaobj->remoteSDPinfo[0].ipaddress=sofiaobj->cp.routeIpAddress;
    RequestForMedia(sofiaobj,EN_MEDIA_CONF_DELPARTICIPANT);


    // CLog::Detail("SofiaSiphandle","OnReferRequest");
    //OnHold_Resume(sofiaobj);

  }
#endif
  void SofiaSiphandler::GetProcessRegisterEvent(){
    xGateSofiaSipRegobj *RegQEvt = NULL;
    if((RegQEvt = PopFromReceiver()) != NULL){
      ((SofiaSiphandler*)RegQEvt->sofiamh)->RegistarResponse(RegQEvt);
    }

  }

  /**/
  void SofiaSiphandler::parsestringtokens(std::string input, char delimit, std::string &token1, std::string &token2 ){
std:string::size_type pos = input.find(delimit);

    if( pos == std::string::npos){
      token1 = input;
      token2 = "";
    }
    else{
      token1 =input.substr(0,pos);
      token2 = input.substr(pos+1);
    }
  }
  std::string SofiaSiphandler::ValidateDialParam(const char *pVal, SIPVALIDATE header,XGATESIPKEY key,callInfoParameter cp){
    std::string ret = "";
    std::string temp = "";  // TODO Need to removed after audit/review with current logic
    if(ValidateSipMsgPointer(pVal))
    {
      ret = pVal;

    } else
      //on url is void get form callparameter
      switch (header){
        case FROM:
          {
            cp.cnCalling.erase(std::remove(cp.cnCalling.begin(),cp.cnCalling.end(),'\"'),cp.cnCalling.end());
            {
              switch(key){
                case EN_USER:
                  parsestringtokens(cp.cnCalling,':',ret,temp); 
                  break;
                case EN_PWD:
                  parsestringtokens(cp.cnCalling,':',temp,ret);
                  break;
                case EN_HOST:
                  parsestringtokens(cp.csaCalling,':',ret,temp); 
                  break;
                case EN_PORT:
                  parsestringtokens(cp.csaCalling,':',temp,ret);
                  if(ret.empty())
                    ret = "5060";
                  break;
                case EN_SCHEMA:
                  ret = "sip";
                  break;
                case EN_PARAM:
                  ret = cp.transportType;
                  //ret="";
                  break;
                default:
                  ret = "";
                  break;
              }
            }

          }
          break;
        case TO:
          {
            switch(key){
              case EN_USER:

                //              ParseUserPassword(cp.cnCalled,ret,temp); 
                parsestringtokens(cp.cnCalled,':',ret,temp);  
                break;
              case EN_PWD:
                //ParseUserPassword(cp.cnCalled,temp,ret);
                parsestringtokens(cp.cnCalled,':',temp,ret);
                break;
              case EN_HOST:
                parsestringtokens(cp.csaCalled,':',ret,temp); 
                break;
              case EN_PORT:
                parsestringtokens(cp.csaCalled,':',temp,ret);
                if(ret.empty())
                  ret = "5060";
                break;
              case EN_SCHEMA:
                ret = "sip";
                break;
              case EN_PARAM:
                ret="";
                break;
              default:
                ret = "";
                break;
            }
          }
          break;
        case CONTACT:
          {
            cp.cnCalling.erase(std::remove(cp.cnCalling.begin(),cp.cnCalling.end(),'\"'),cp.cnCalling.end());
            switch(key){
              case EN_USER:
                parsestringtokens(cp.cnCalling,':',ret,temp); 
                break;
              case EN_PWD:
                parsestringtokens(cp.cnCalling,':',temp,ret);
                break;
              case EN_HOST:
                parsestringtokens(cp.csaCalling,':',ret,temp); 
                break;
              case EN_PORT:
                parsestringtokens(cp.csaCalling,':',temp,ret);
                if(ret.empty())
                  ret = "5060";

                break;
              case EN_SCHEMA:
                ret = "sip";
                break;
              case EN_PARAM:
                ret="";
                break;
              default:
                ret = "";
                break;
            }
          }
          break;
        case PAI:
          {
            switch(key){
              case EN_USER:
                parsestringtokens(cp.cnCalling,':',ret,temp); 
                break;
              case EN_PWD:
                parsestringtokens(cp.cnCalling,':',temp,ret);
                break;
              case EN_HOST:
                // parsestringtokens(cp.csaCalling,':',ret,temp); 
                //parsestringtokens(xGateUtil::GetLocalIPAddress(),':',ret,temp); 
                //parsestringtokens(xGateUtil::getLocalIpAddr(),':',ret,temp); 
                parsestringtokens(XGSIP_HOST(),':',ret,temp); 
                break;
              case EN_PORT:
                parsestringtokens(cp.csaCalling,':',temp,ret);
                if(ret.empty())
                  ret = "5060";
                break;
              case EN_SCHEMA:
                ret = "sip";
                break;
              case EN_PARAM:
                ret="";
                break;
              default:
                ret = "";
                break;
            }
          }
          break;
        case CALLID:
          break;
          break;
        case VIA:
        default:
          break;
      }
    return ret;

  }
  bool SofiaSiphandler::ValidateSipMessage(sip_t const *sip,SIPVALIDATE header){
    if(sip){
      switch(header){
        case SIPVALIDATE::FROM:

          if(!sip->sip_from || \
              !sip->sip_from->a_url->url_user || !sip->sip_from->a_url->url_host ||\
              !sip->sip_from->a_tag){
            XGLOG_WARN(THISMODULE,"SofiaSiphandler::ValidateSipMessage(%s)","Failed-Sip From Header Validation");
            return false;    
          }
          break;
        case SIPVALIDATE::REQUEST:
          if( !sip->sip_request || \
              !sip->sip_request->rq_url->url_user || !sip->sip_request->rq_url->url_host || \
              !sip->sip_request->rq_url->url_port){
            XGLOG_WARN(THISMODULE,"SofiaSiphandler::ValidateSipMessage(%s)","Failed-Sip Request Header Validation");
            return false;
          }
          break;
        case SIPVALIDATE::CONTACT:
          if(!sip->sip_contact){
            XGLOG_WARN(THISMODULE,"SofiaSiphandler::ValidateSipMessage(%s)","Failed-Sip Contact Header Validation");
            return false;
          }
          break;
        case SIPVALIDATE::TO:
          if(!sip->sip_to || \
              !sip->sip_to->a_url->url_user || !sip->sip_to->a_url->url_host){
            XGLOG_WARN(THISMODULE,"SofiaSiphandler::ValidateSipMessage(%s)","Failed-Sip To Header Validation");
            return false; 
          }
          break;
        case SIPVALIDATE::TAG:
          break;
        case SIPVALIDATE::CALLID:
          if(!sip->sip_call_id ||\
              !sip->sip_call_id->i_id){
            XGLOG_WARN(THISMODULE,"SofiaSiphandler::ValidateSipMessage(%s)","Failed-Sip Callid Header Validation");
            return false; 
          }
          break;
        default:
          XGLOG_WARN(THISMODULE,"SofiaSiphandler::ValidateSipMessage(%s)","No Header option to check");
          return false;
          break;
      } //swicth case Header 
    } //Sip check point
    else{
      XGLOG_WARN(THISMODULE,"SofiaSiphandler::ValidateSipMessage(%s)","Failed-Sip Structure Validation");
      return false;
    }
    return true;

  } // ValidateSipMessage


  /**/
  std::string SofiaSiphandler::FillHeader(url_t *url,callInfoParameter cp,SIPVALIDATE header){
    std::stringstream xgateSipheader ;
    if(url ==NULL){
      xgateSipheader << ValidateDialParam(NULL,header,XGATESIPKEY::EN_USER,cp)  ; //User
      xgateSipheader << "<" ;
      xgateSipheader << ValidateDialParam(NULL,header,XGATESIPKEY::EN_SCHEMA,cp) << ":" ;
      xgateSipheader << ValidateDialParam(NULL,header,XGATESIPKEY::EN_USER,cp) << "@" ;
      xgateSipheader << ValidateDialParam(NULL,header,XGATESIPKEY::EN_HOST,cp) << ":" ;
      xgateSipheader << ValidateDialParam(NULL,header,XGATESIPKEY::EN_PORT,cp);

    }else{

      xgateSipheader << ValidateDialParam(url->url_user,header,XGATESIPKEY::EN_USER,cp)  ; //User
      xgateSipheader << "<" ;
      xgateSipheader << ValidateDialParam(url->url_scheme,header,XGATESIPKEY::EN_SCHEMA,cp) << ":" ;
      xgateSipheader << ValidateDialParam(url->url_user,header,XGATESIPKEY::EN_USER,cp) << "@" ;
      xgateSipheader << ValidateDialParam(url->url_host,header,XGATESIPKEY::EN_HOST,cp)<< ":" ;
      xgateSipheader << ValidateDialParam(url->url_port,header,XGATESIPKEY::EN_PORT,cp);
    }
    switch (header){
      case FROM:
        xgateSipheader << ">";
        break;
      case TO:
        xgateSipheader << ">";


        break;
      case CONTACT:
        /*if(strstr(ValidateDialParam(url->url_params,header,XGATESIPKEY::EN_PARAM,cp).c_str(),"transport=tcp"))
          xgateSipheader << ";transport=tcp";
          xgateSipheader << ";" << ValidateDialParam(url->url_params,header,XGATESIPKEY::EN_PARAM,cp);*/
        if(strstr(ValidateDialParam(NULL,header,XGATESIPKEY::EN_PARAM,cp).c_str(),"transport=sctp"))
          xgateSipheader << ";transport=sctp";
        else
          xgateSipheader << ";transport=udp";
          //xgateSipheader << ";transport=tcp";
        /*xgateSipheader << ";" << ValidateDialParam(NULL,header,XGATESIPKEY::EN_PARAM,cp);*/
        xgateSipheader << ">";
        break;
      case PAI:
        xgateSipheader << ";user=phone";
        xgateSipheader << ">";
        break;
      case REQUEST:
        break;
      case TAG:
        break;
      case CALLID:
        break;
        break;
      case SUBJECT:
        break;
      case VIA:
      case CONTACTLEN :
        break;
      default:
        break;
    }



    return xgateSipheader.str();
  }

  //Added for IPv6 Support
  bool SofiaSiphandler::getSipContactURLbyIPAddrFamily(xGateSofiaobj *pSipMgc, string &strPbxIP)
  {

          if (pSipMgc->m_enumCallLegIpAddrFamily == IP_ADDR_V6)
                  strPbxIP = "[" + std::string(xGateUtil::getLocalIpAddr(AF_INET6)) + "]";
          else {
                  strPbxIP = xGateUtil::getLocalIpAddr();
          }
          char contact_uri[500];
          memset(contact_uri, 0, sizeof(contact_uri));
          snprintf(contact_uri,500, "<sip:%s@%s:%s;%s>",
                  pSipMgc->cp.cnCalling.c_str(),
                  strPbxIP.c_str(),
                  "5060",
                  pSipMgc->siptransport.c_str());
          strPbxIP = contact_uri;
          return true;
  }

  nua_handle_t* SofiaSiphandler::CreateSofiaDialhandle(callInfoParameter cp, sip_t const *sip){
#define FURL sip->sip_from->a_url
#define PURL Paidurl->paid_url
    nua_handle_t *nhob = NULL;
    try{
      su_home_t su_home;

      printf("Registerd Number %s \n",cp.csaCalled.c_str());
      // Below parser is for PSTN to APP support , This code need to moduled properly TODO yadav
      std::string to_host, to_port;
      //   if (sobj->cp.proxyUserName.empty())

      //Added for IPv6 Support
      //parsestringtokens(cp.csaCalled,':',to_host,to_port);
      if(ip_validator(cp.csaCalled)==true){
        std::string tmp_host(""), tmp("");
        parsestringtokens(cp.csaCalled, ']', to_host, tmp_host);
        parsestringtokens(tmp_host, ':', tmp, to_port);
        to_host += "]";
      }
      else {
        parsestringtokens(cp.csaCalled, ':', to_host, to_port);
      }

      cout << " TO USER PART " << to_host << " Proxy Name " << cp.proxyUserName << endl;
      XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle cp.proxyUserName = (%s)", cp.proxyUserName.c_str());

      url_t *url = SofiaCreateMake(su_home,to_host.c_str()); // creating url structure

      //Added for IPv6 Support
      if(ip_validator(cp.csaCalled)==true)
      {
        url->url_scheme = strdup("sip");
        url->url_user = strdup(cp.proxyUserName.c_str());
        url->url_password = ""; //strdup(cp.proxyUserName.c_str());
        url->url_host = strdup(to_host.c_str());
        url->url_port = strdup(to_port.c_str());
      }

      if(!cp.proxyUserName.empty())
        url->url_user =  cp.proxyUserName.c_str();
      std::string dest = FillHeader(url, cp, SIPVALIDATE::TO);
      url_sanitize(url);  // Only required for destination
      SofiaDeInitMemory(su_home);
      cout << dest.c_str() << endl;

      //Added for IPv6 Support
      if(ip_validator(cp.csaCalled)==true)
      {
        XGLOG_INFO( "SofiaSiphandler::CreateSofiaDialhandle Copying IPV6 PBXIP to cp.csaCalling for PSTN Transfer ");
        cp.csaCalling = "[" + std::string(xGateUtil::getLocalIpAddr(AF_INET6)) + "]"; //XGSIP_HOST();
      }
      else
      {
        XGLOG_INFO( "SofiaSiphandler::CreateSofiaDialhandle Copying IPV4 PBXIP to cp.csaCalling for PSTN Transfer");
        cp.csaCalling = XGSIP_HOST();
      }

      std::string src = FillHeader(NULL, cp, SIPVALIDATE::FROM);
    
      std::string callId = cp.call_id.c_str();
	
      cout << src.c_str() << endl;

      std::string contact = FillHeader(NULL, cp, SIPVALIDATE::CONTACT);
      cout << contact.c_str() << endl;

      // follwoing code-change is added for testing; to be removed later...
      sip_p_asserted_identity_t * Paidurl=NULL; // = sip_p_asserted_identity(sip);

      std::string pai = "";
      if(cp.isPstnNumber)
      {
        pai = FillHeader(NULL,cp,SIPVALIDATE::PAI); 
      }
      std::string privacy = "";
      if(Paidurl){
        if(cp.ccp == CCP_Restricted) {
          privacy = "id";
        }
      }
   
      std::string params("");
      std::string scheme("");
      if(ValidateSipMsgPointer(cp.cui.c_str()))
      {
        params=XGATE_SOFIA_HEADER_USER_INFO;
        params+=": ";
        params+=SipHeaderEncode(cp.cui);
      }

      if(ValidateSipMsgPointer(cp.cpinfo.c_str()))
      {
        scheme=XGATE_SOFIA_HEADER_PRIVATE_INFO;
        scheme+=": ";
        scheme+=SipHeaderEncode(cp.cpinfo);
      }

      std::string route = "";
      if(strstr(cp.calledDeviceType.c_str(),"PSTN") || cp.isPstnNumber){
        if(sbcip.size() > 0){
          route = "<sip:";
          route += GetCurrentPbxInfo();
          route += ":5060;lr>";
        }
        else {
          XGLOG_ERROR("SofiaSiphandler::CreateSofiaDialhandle Exception No Routing configured for PSTN call");
          return NULL;
        }
      }else if(!(cp.routeIpAddress.empty())){
        route = "<sip:";
        route += cp.routeIpAddress.c_str();
        route += ":5060;lr>";
      } 

      //Added for Video Conference

      char UrAvConfRoomId[250] = "\0";
      memset(UrAvConfRoomId, '\0', sizeof(UrAvConfRoomId));
      if (!cp.UrAvConferenceRoomId.empty()) {
              snprintf(UrAvConfRoomId,250, "UR-AV-ConferenceRoom-ID:%s", cp.UrAvConferenceRoomId.c_str());
              XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle UR-AV-ConferenceRoom-ID = (%s)", cp.UrAvConferenceRoomId.c_str());
      }
      else 
      {
              XGLOG_ERROR("SofiaSiphandler::CreateSofiaDialhandle UR-AV-ConferenceRoom-ID value is empty");
      }

      char UrAvConfUserType[250] = "\0";
      memset(UrAvConfUserType, '\0', sizeof(UrAvConfUserType));
      if (!cp.UrAvConferenceUserType.empty()) {
              snprintf(UrAvConfUserType,250, "UR-AV-Conference-UserType:%s", cp.UrAvConferenceUserType.c_str());
              XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle UR-AV-Conference-UserType = (%s)", cp.UrAvConferenceUserType.c_str());
      }
      else 
      {
              XGLOG_ERROR("SofiaSiphandler::CreateSofiaDialhandle UR-AV-Conference-UserType value is empty");
      }

      char UrAvConfConnId[250] = "\0";
      memset(UrAvConfConnId, '\0', sizeof(UrAvConfConnId));
      if (!cp.UrAvConferenceConnId.empty()) {
              snprintf(UrAvConfConnId,250, "UR-AV-ConferenceConn-ID:%s", cp.UrAvConferenceConnId.c_str());
              XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle UR-AV-ConferenceConn-ID = (%s)", cp.UrAvConferenceConnId.c_str());
      }
      else 
      {
              XGLOG_ERROR("SofiaSiphandler::CreateSofiaDialhandle UR-AV-ConferenceConn-ID value is empty");
      }

      char UrAvConfDstConnId[250] = "\0";
      memset(UrAvConfDstConnId, '\0', sizeof(UrAvConfDstConnId));
      if (!cp.UrAvConferenceDstConnId.empty()) {
              snprintf(UrAvConfDstConnId,250, "UR-AV-Conference-DstConn-ID:%s", cp.UrAvConferenceDstConnId.c_str());
              XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle UR-AV-ConferenceDstConn-ID = (%s)", cp.UrAvConferenceDstConnId.c_str());
      }
      else
      {
              XGLOG_ERROR("SofiaSiphandler::CreateSofiaDialhandle UR-AV-ConferenceDstConn-ID value is empty");
      }

      char UrAvSource[250] = "\0";
      memset(UrAvSource, '\0', sizeof(UrAvSource));
      if (!cp.cnCalling.empty()) {
              snprintf(UrAvSource,250, "UR-AV-Source:%s:%s", cp.cnCalling.c_str(),cp.callStream.c_str());
              XGLOG_INFO("SofiaSiphandler::CreateSofiaDialhandle UR-AV-Source = %s:%s", cp.UrAvConferenceConnId.c_str(),cp.callStream.c_str());
      }
      else
      {
              XGLOG_ERROR("SofiaSiphandler::CreateSofiaDialhandle UR-AV-Source value is empty");
      }

      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      XGLOG_INFO("Creating handle with dest:(%s), src:(%s), contact:(%s), route:(%s), org-conn-id:(%s), dst-conn-id(%s) for callid:(%s)",\
         dest.c_str(),src.c_str(),contact.c_str(),route.c_str(),cp.UrAvConferenceConnId.c_str(),cp.UrAvConferenceDstConnId.c_str(),callId.c_str());
      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");


      nhob = nua_handle(GetAppContext(),NULL,\
          SIPTAG_TO_STR(dest.c_str()),\
          SIPTAG_FROM_STR(src.c_str()),\
          SIPTAG_CONTACT_STR(contact.c_str()),\
	  SIPTAG_CALL_ID_STR(callId.c_str()),\
          TAG_IF(!route.empty(),SIPTAG_ROUTE_STR(route.c_str())), 
          TAG_IF(!pai.empty(),SIPTAG_P_ASSERTED_IDENTITY_STR(pai.c_str())),\
          TAG_IF(!privacy.empty(), SIPTAG_PRIVACY_STR("id")), \
          TAG_IF(!params.empty(), SIPTAG_HEADER_STR(params.c_str())),\
          TAG_IF(!scheme.empty(), SIPTAG_HEADER_STR(scheme.c_str())),\
          TAG_IF(!cp.UrAvConferenceRoomId.empty(), SIPTAG_UNKNOWN_STR(UrAvConfRoomId)), \
          TAG_IF(!cp.UrAvConferenceUserType.empty(), SIPTAG_UNKNOWN_STR(UrAvConfUserType)), \
          TAG_IF(!cp.UrAvConferenceConnId.empty(), SIPTAG_UNKNOWN_STR(UrAvConfConnId)), \
          TAG_IF(!cp.UrAvConferenceDstConnId.empty(), SIPTAG_UNKNOWN_STR(UrAvConfDstConnId)), \
          TAG_IF(!cp.cnCalling.empty(), SIPTAG_UNKNOWN_STR(UrAvSource)), \
          NUTAG_AUTOANSWER(0),\
          NUTAG_ALLOW("REFER"),\
          TAG_END());
    }
    catch(...) {
      XGLOG_WARN(THISMODULE,"SofiaSiphandler::CreateSofiaDialhandle Exception ");
      return NULL;
    }
     XGLOG_WARN(THISMODULE,"SofiaSiphandler::CreateSofiaDialhandle Completed ");
    return nhob;
  }
  /*------------------------------ inherted Dispatcher Functions--------------------------------------*/
  /*
  */
  xGateReturn SofiaSiphandler::RespondWith1xx(xGateSofiaobj* pSipMgc ){
    XGLOG_INFO(THISMODULE,"SofiaSiphandler::RespondWith1xx start ");
    sdp_session_t *psdp = NULL ;
    std::string sdpstr;
    switch(pSipMgc->sipeventtype){
      case XGATESIPEVENT::EN_100_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_100_REL,"Trying");
        break;
      case XGATESIPEVENT::EN_180_REL:
        //      if(  psdp = CreateLocalSdpParam(EN_SDP_RESPONSE,pSipMgc) )
        //DebugVM sdpstr = CreateLocalSdpParam(EN_SDP_RESPONSE,pSipMgc)      ;
        if(sdpstr.length() > 0)      
        {
          printf("\n RepondWitn1xx -- %s", sdpstr.c_str());
          nua_set_hparams((nua_handle_t*)pSipMgc->sofianh,SOATAG_AUDIO_AUX("telephone-event"),SOATAG_USER_SDP_STR(sdpstr.c_str()),
              NUTAG_EARLY_MEDIA (1),
              NUTAG_EARLY_ANSWER(1),
              NUTAG_AUTOANSWER(0),
              TAG_NULL());
          pSipMgc->callstate = EN_CS_ALERTING;
          SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_180_REL,"Ringing");
        }
        /* nua_respond((nua_handle_t*)pSipMgc->sofianh, 180, "Ringing",
           NUTAG_EARLY_MEDIA (1),
           NUTAG_EARLY_ANSWER(1),
           NUTAG_AUTOANSWER(0),
           TAG_NULL());*/

        break;
#if 0//DebugVM start
      case XGATESIPEVENT::EN_183_REL:
        //if (pSipMgc->remoteSDPinfo.mediamode.compare("RTP")== 0) { // TODO Frederick to stop DTLS playback
        //        if( psdp = CreateLocalSdpParam(EN_SDP_RESPONSE,pSipMgc) )
        sdpstr = CreateLocalSdpParam(EN_SDP_RESPONSE,pSipMgc);        
        {
          nua_set_hparams((nua_handle_t*)pSipMgc->sofianh,SOATAG_AUDIO_AUX("cn telephone-event"),SOATAG_USER_SDP_STR(sdpstr.c_str()),
              NUTAG_EARLY_MEDIA (1),
              NUTAG_EARLY_ANSWER(1),
              NUTAG_AUTOANSWER(0),
              TAG_NULL());

          pSipMgc->callstate = EN_CS_PROCEEDING;
          SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_183_REL,"Processing");
        }
        //}
        /*       nua_respond((nua_handle_t*)pSipMgc->sofianh, 183, "Processing",
                 NUTAG_EARLY_MEDIA (1),
                 NUTAG_EARLY_ANSWER(1),
                 NUTAG_AUTOANSWER(0),
                 TAG_NULL());*/

        break;
#endif //DebugVM end
      default:
        break;

    }
    XGLOG_INFO(THISMODULE,"SofiaSiphandler::RespondWith1xx Event(%d - %s) ",pSipMgc->sipeventtype,pSipMgc->uid.c_str());
    return xGateReturn::SUCCESS;
  }

  xGateReturn SofiaSiphandler::SendReInvite(xGateSofiaobj* sobj ){
    XGLOG_INFO("SofiaSiphandler::SendReInvite start DebugVMIVR========================================>");
#if 0
    xGateCallPair *callpair = NULL;
    xGateCallObj *callObj;
    callObj =  new xGateCallObj;
    callObj->xGateCallObjInit();
    callObj->uid = sobj->uid;
    callObj->sipeventtype = EN_REINVITE_REQ;
    if(  callpair = GetConnectedList(callObj->uid))
#endif
    {
      //xGateSofiaobj *psobj = callpair->Orgcall;

      //psobj->localSDPinfo[0].sendreceive = "sendonly";

      nua_handle_bind ((nua_handle_t*)sobj->sofianh, (nua_hmagic_t *)this);
      bool hold_resume = true;

      nua_set_hparams((nua_handle_t*)sobj->sofianh,/*SOATAG_USER_SDP_STR(sdpstr.c_str()),*/ TAG_IF(hold_resume,SOATAG_HOLD("audio")),
          TAG_IF(!hold_resume,SOATAG_HOLD("")),TAG_END());
      nua_invite((nua_handle_t*)sobj->sofianh,NUTAG_AUTOACK(1),TAG_END());

      SofiaRespond((nua_handle_t*)sobj->sofianh,EN_REINVITE_REQ,"ReInvite");
    }
    return xGateReturn::SUCCESS;
  }

  xGateReturn SofiaSiphandler::SendReInviteForConfUser(xGateSofiaobj* sobj )
  {
      XGLOG_INFO("SofiaSiphandler::SendReInviteForConfUser");
      //nua_handle_bind ((nua_handle_t*)sobj->sofianh, (nua_hmagic_t *)this);
      //nua_set_hparams((nua_handle_t*)sobj->sofianh,SOATAG_USER_SDP_STR(sobj->cp.sdpStr.c_str()),TAG_END());
      //nua_invite((nua_handle_t*)sobj->sofianh,NUTAG_AUTOACK(1),TAG_END());
      nua_invite((nua_handle_t*)sobj->sofianh,NUTAG_AUTOACK(1),SIPTAG_CONTENT_TYPE_STR("application/sdp"),SIPTAG_PAYLOAD_STR(sobj->cp.sdpStr.c_str()),TAG_END());

      return xGateReturn::SUCCESS;
  }

  xGateReturn SofiaSiphandler::SendRefer(xGateSofiaobj* sobj, xGateCallObj *callObj){
    XGLOG_INFO("SofiaSiphandler::SendRefer start ");
    XGLOG_INFO("SofiaSiphandler::SendRefer transfer_target=%s, transfer_to=%s, transfer_from=%s, domain=%s",
                               callObj->cp.m_transferDetail.transfer_target.c_str(),
                               callObj->cp.m_transferDetail.transfer_to.c_str(),
                               callObj->cp.m_transferDetail.transfer_from.c_str(),
                               callObj->cp.csaCalled.c_str());

    string referTo = "sip:" + callObj->cp.m_transferDetail.transfer_target + "@" + callObj->cp.csaCalled + ";5060";
    string referBy = "sip:" + callObj->cp.m_transferDetail.transfer_from + "@" + callObj->cp.csaCalling;

    nua_refer((nua_handle_t*)sobj->sofianh, NUTAG_AUTOANSWER(0),
                        SIPTAG_REFER_TO_STR(referTo.c_str()),
                        SIPTAG_REFERRED_BY_STR(referBy.c_str()),
                        /*** TAG_IF(!(sofiaCall->IsUsingVirtualRtp()), NUTAG_ALLOW("UPDATE")), *****/  TAG_END());
    return xGateReturn::SUCCESS;
  }

#if 0//DebugVM start

  xGateReturn SofiaSiphandler::RespondWithInvite(xGateSofiaobj* pSipMgc ){
    CLog::Detail(THISMODULE,"SofiaSiphandler::RespondWithInvite start ");

    nua_handle_bind ((nua_handle_t*)pSipMgc->sofianh, (nua_hmagic_t *)this);

    // we must NUTAG_AUTOANSWER(0) again despite this already done
    // in higher, general nua parameters (sofia's bug)
    ////nua_set_hparams((nua_handle_t*)pSipMgc->sofianh, NUTAG_AUTOANSWER(0), /*** TAG_IF(!(sofiaCall->IsUsingVirtualRtp()), NUTAG_ALLOW("UPDATE")), *****/  TAG_END());
    nua_invite((nua_handle_t*)pSipMgc->sofianh, NUTAG_AUTOANSWER(0), /*** TAG_IF(!(sofiaCall->IsUsingVirtualRtp()), NUTAG_ALLOW("UPDATE")), *****/  TAG_END());

    /****************
      nua_set_hparams((nua_handle_t*)pSipMgc->sofianh, NUTAG_EARLY_MEDIA (1),
      NUTAG_EARLY_ANSWER(1),
      NUTAG_AUTOANSWER(0),
      TAG_NULL());
     ****************/

    SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_INVITE_REQ,"Invite");
  }
  /*
  */
#endif //DebugVM end
  xGateReturn SofiaSiphandler::RespondWith2xx(xGateSofiaobj* pSipMgc){
    XGLOG_INFO("SofiaSiphandler::RespondWith2xx start ");
    sdp_session_t *psdp = NULL ;
    std::string sdpstr;
    switch(pSipMgc->sipeventtype){
      case XGATESIPEVENT::EN_200_REL:
        //        if(psdp = CreateLocalSdpParam(EN_SDP_RESPONSE,pSipMgc))
                     if(pSipMgc && !pSipMgc->cp.sdpStr.empty())
              {
    		      XGLOG_INFO("SofiaSiphandler::RespondWith2xx SdInfo is not empty for (%s)",pSipMgc->uid.c_str());
                      sdpstr = pSipMgc->cp.sdpStr.c_str();
              }
              else
              {
    		      XGLOG_INFO("SofiaSiphandler::RespondWith2xx SdInfo is empty,framing sdp param for (%s)",pSipMgc->uid.c_str());
                      sdpstr = CreateLocalSdpParam(EN_SDP_RESPONSE,pSipMgc);
              }
 
        if(sdpstr.length()> 0)
        {
          printf("\n RepondWitn2xx -- %s", sdpstr.c_str());

          std::string contact_uri = "";
          getSipContactURLbyIPAddrFamily(pSipMgc, contact_uri);

          nua_respond((nua_handle_t*)pSipMgc->sofianh, 200, "OK", SIPTAG_CONTACT_STR(contact_uri.c_str()),SOATAG_AUDIO_AUX("telephone-event"),
              //SOATAG_USER_SDP_STR(sdpstr.c_str()),NUTAG_OFFER_SENT(1),
              SIPTAG_ALLOW_STR("INVITE, ACK, BYE, CANCEL, OPTIONS, MESSAGE, INFO, SUBSCRIBE, NOTIFY, REFER, UPDATE"),SIPTAG_CONTENT_TYPE_STR("application/sdp"),SIPTAG_PAYLOAD_STR(sdpstr.c_str()), NUTAG_OFFER_SENT(1),
              TAG_NULL());
          if(pSipMgc->callstate < EN_CS_CONNECTED)
            pSipMgc->callstate = EN_CS_CONNECTED;
        }else{
          // Error 
          XGLOG_INFO("SofiaSiphandler::RespondWith2xx Media create failed %s",pSipMgc->uid.c_str());
          RespondToPeer(EN_503_REL,pSipMgc);
        }
        break;

      default:
        break;

    }
    XGLOG_INFO("SofiaSiphandler::RespondWith2xx Event(%d - %s) ",pSipMgc->sipeventtype,pSipMgc->uid.c_str());
    return xGateReturn::SUCCESS;
  }
  /*
  */
xGateReturn SofiaSiphandler::RespondWith3xx(xGateSofiaobj* pSipMgc)
{
  XGLOG_INFO( "SofiaSiphandler::RespondWith3xx");
  switch(pSipMgc->sipeventtype){
    case XGATESIPEVENT::EN_300_REL:
      break;
    case XGATESIPEVENT::EN_302_REL:
      {
	XGLOG_INFO( "SofiaSiphandler::RespondWith3xx with XGATESIPEVENT::EN_302_REL");
	char contact_uri[500];
	memset(contact_uri, 0, sizeof(contact_uri));
	snprintf(contact_uri,500, "<sip:%s@%s:%s;%s>", pSipMgc->cp.cnCalled.c_str(), pSipMgc->cp.redirectIp.c_str(), "5060", pSipMgc->siptransport.c_str());
	printf("\n RepondWith3xx -- uid : %s\n",pSipMgc->uid.c_str());
	XGLOG_INFO( "SofiaSiphandler::XGATESIPEVENT::EN_302_REL local MGC_IP(%s)",pSipMgc->cp.redirectIp.c_str());
	XGLOG_INFO( "Contact URI : (%s) on EN_302_REL for uid(%s)", contact_uri, pSipMgc->uid.c_str());
	nua_respond((nua_handle_t*)pSipMgc->sofianh, 302, "Temporarily Moved", SIPTAG_CONTACT_STR(contact_uri),TAG_NULL());
	break;
      }
    default:
      break;

  }
  XGLOG_INFO("SofiaSiphandler::RespondWith3xx Event(%d - %s) ",pSipMgc->sipeventtype,pSipMgc->callid.c_str());
  return xGateReturn::SUCCESS;
}
  /*
  */
  xGateReturn SofiaSiphandler::RespondWith4xx(xGateSofiaobj* pSipMgc){
    XGLOG_INFO("SofiaSiphandler::RespondWith4xx()....entered ");

    if(!pSipMgc && !pSipMgc->sofianh){
      XGLOG_INFO("SofiaSiphandler::RespondWith4xx - Invalid sofia handle");
      return  xGateReturn::FAIL ;
    }

    #if 0
    if(pSipMgc->cp.isSwitchBoardCall == 1){
      pSipMgc->switchboardparam.SwbCallEvent = EN_SWB_CALLDISCONNECT;
      if(pSipMgc && pSipMgc->sofianh){
        SofiaSendBye((nua_handle_t*)pSipMgc->sofianh);
        // OnSwitchBoardCallProcess(pSipMgc->SofiaLegB);
      }
      return xGateReturn::SUCCESS;
    }
   #endif
    //TODO: we are getting core dump here in live. so we comment.
    //CLog::Detail(THISMODULE,"SofiaSiphandler::RespondWith4xx Event(%d - %s) ",pSipMgc->sipeventtype,pSipMgc->callid.c_str());
    switch(pSipMgc->sipeventtype){
      case XGATESIPEVENT::EN_400_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_400_REL,"Bad Request"); 
        break;
      case XGATESIPEVENT::EN_401_REL:{
                                     //  std::string auth = Authenticating(pSipMgc,((sip_t*)(pSipMgc->siptag))->sip_www_authenticate);
                                     //  nua_authenticate((nua_handle_t*)pSipMgc->sofianh, NUTAG_AUTH(auth.c_str()), TAG_END());
                                     }
                                     //SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_401_REL,"Unauthorized");
                                     // Return done since for this error call must not be disconnected
                                     return  xGateReturn::SUCCESS;
      case XGATESIPEVENT::EN_407_REL:{
                                       // return done since for this error call must not disconnected
                                       // sip_proxy_authenticate

                                      // std::string auth  = Authenticating(pSipMgc,((sip_t*)(pSipMgc->siptag))->sip_proxy_authenticate);
                                      // nua_authenticate((nua_handle_t*)pSipMgc->sofianh, NUTAG_AUTH(auth.c_str()), TAG_END());
                                      }
                                     return  xGateReturn::SUCCESS;
      case XGATESIPEVENT::EN_403_REL:
                                     SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_403_REL,"Forbidden");
                                     break;
      case XGATESIPEVENT::EN_404_REL:
                                     SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_404_REL,"Not Found  or No Route");
                                     break;
      case XGATESIPEVENT::EN_408_REL:
                                     SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_408_REL,"Request Timeout");
                                     break;
      case XGATESIPEVENT::EN_410_REL:
                                     SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_410_REL,"Gone");
                                     break;
      case XGATESIPEVENT::EN_480_REL:
                                     SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_480_REL,"Temporarily Unavailable");
                                     break;
      case XGATESIPEVENT::EN_482_REL:
                                     SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_482_REL,"Loop Detected");
                                     break;
      case XGATESIPEVENT::EN_484_REL:
                                     SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_484_REL,"Address Incomplete");
                                     break;
      case XGATESIPEVENT::EN_486_REL:
                                     SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_486_REL,"Busy Here");
                                     break;
      case XGATESIPEVENT::EN_487_REL:
                                     SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_487_REL,"Request Terminated");
                                     break;
      case XGATESIPEVENT::EN_488_REL:
                                     SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_488_REL,"Not Acceptable Here");
                                     break;
      default:
                                     SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_408_REL,"default-Request Timeout");
                                     break;

    }
    // since it is error in Server we need to delete the call
    /*if(pSipMgc->SofiaLegB && pSipMgc->SofiaLegB->sofianh)
      SofiaSendBye((nua_handle_t*)pSipMgc->SofiaLegB->sofianh);*/


    //DeleteCallInfo((pSipMgc));

    return xGateReturn::SUCCESS;
  }
  /*
  */
  xGateReturn SofiaSiphandler::RespondWith5xx(xGateSofiaobj* pSipMgc){
    XGLOG_INFO("SofiaSiphandler::RespondWith5xx()....entered ");
    if(!pSipMgc && !pSipMgc->sofianh){
      XGLOG_INFO("SofiaSiphandler::RespondWith4xx - Invalid sofia handle");
      return  xGateReturn::FAIL;
    }

    XGLOG_INFO("SofiaSiphandler::RespondWith5xx Event(%d - %s) ",pSipMgc->sipeventtype,pSipMgc->uid.c_str());
    switch(pSipMgc->sipeventtype){
      case XGATESIPEVENT::EN_500_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_500_REL,"Server Internal Error");
        XGLOG_INFO("SofiaSiphandler::RespondWith5xx Event(%d - %s) ",pSipMgc->sipeventtype,pSipMgc->uid.c_str());
        break;
      case XGATESIPEVENT::EN_501_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_501_REL,"Not Implemented");
        break;
      case XGATESIPEVENT::EN_502_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_502_REL,"Bad Gateway");
        break;
      case XGATESIPEVENT::EN_503_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_503_REL,"Service Unavailable");
        break;
      case XGATESIPEVENT::EN_504_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_504_REL,"Server Time-out");
        break;
      case XGATESIPEVENT::EN_510_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_510_REL,"Facility Rejected");
        break;
      default:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_504_REL,"default-Server Time-out");
        break;

    }
    // since it is error in Server we need to delete the call
    /*if(pSipMgc->SofiaLegB && pSipMgc->SofiaLegB->sofianh)
      SofiaSendBye((nua_handle_t*)pSipMgc->SofiaLegB->sofianh);*/

    //DeleteCallInfo((pSipMgc));

    return xGateReturn::SUCCESS;
  }
  /*
  */
  xGateReturn SofiaSiphandler::RespondWith6xx(xGateSofiaobj* pSipMgc){

    XGLOG_INFO("SofiaSiphandler::RespondWith6xx()....entered ");
    XGLOG_INFO("SofiaSiphandler::RespondWith6xx Event(%d - %s) ",pSipMgc->sipeventtype,pSipMgc->uid.c_str());
    switch(pSipMgc->sipeventtype){
      case XGATESIPEVENT::EN_600_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_600_REL,"Busy Everywhere");
        break;
      case  XGATESIPEVENT::EN_603_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_603_REL,"Decline");
        break;
      case XGATESIPEVENT::EN_604_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_604_REL,"Does Not Exist Anywhere");
        break;
      case XGATESIPEVENT::EN_606_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_606_REL,"Not Acceptable");
        break;
      case XGATESIPEVENT::EN_607_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_607_REL,"Unwanted");
        break;
      default:
        XGLOG_INFO("SofiaSiphandler::RespondWith6xx Event(%d)- Event Not Impemented",pSipMgc->sipeventtype); 
        break;


    }
    // since it is error in Server we need to delete the calli
    /*if(pSipMgc->SofiaLegB && pSipMgc->SofiaLegB->sofianh)
      SofiaSendBye((nua_handle_t*)pSipMgc->SofiaLegB->sofianh);*/

    //DeleteCallInfo((pSipMgc));

    return xGateReturn::SUCCESS;
  }
#if 0//DebugVM start
  bool SofiaSiphandler::CheckResponseRequired(int status, xGateSofiaobj *sobj){
    if (!sobj)
      return false;
    if(sobj->cp.isForkingenabled){ // Forking Enabled
      return ForkingResponse(status,sobj);
    }
    //if(sobj->cp.isSwitchBoardCall && status > 300)
    //   return false;
    return true;
  }
#endif
  void SofiaSiphandler::LinkDstInfoWithOrgInfo(std::string uid){
    xGateCallPair *callpair =  GetConnectedList(uid);

    if(callpair == NULL)
      return ;
    if(callpair->Orgcall && callpair->Dstcall){
      callpair->Orgcall->cp.csCalledCodec = callpair->Dstcall->cp.csCalledCodec;
      callpair->Orgcall->cp.csCallingCodec = callpair->Dstcall->cp.csCallingCodec;
      //callpair->Dstcall->cp.csCallingCodec=callpair->Orgcall->cp.csCalledCodec;
      //callpair->Dstcall->cp.csCalledCodec = callpair->Orgcall->cp.csCallingCodec;
      //callpair->Orgcall->cp.csCallingCodec=callpair->Dstcall->cp.csCalledCodec;
      //callpair->Orgcall->cp.csCalledCodec = callpair->Dstcall->cp.csCallingCodec;


    }

  }
#if 0

  bool SofiaSiphandler::ForkingResponse(int status, xGateSofiaobj *sobj){
    CLog::Detail(THISMODULE,"SofiaSiphandler::ForkingResponse Proxy Name %s Callid %s ",sobj->cp.proxyUserName.c_str(), sobj->uid.c_str());
    xGateCallPair *callpair =  GetConnectedList(sobj->uid);

    switch(status/100){
      case 1:

        return true;
      case 2:{
               if(callpair == NULL)
               {
                 // No Call found on the connect list May be Originater  left the list.
                 // so Diconnect him 
                 CLog::Detail(THISMODULE,"SofiaSiphandler::ForkingResponse %s Originator not in connect list", sobj->uid.c_str());
                 // Since stack is enable with auto ack
                 //                     SofiaSendAck(sobj->sofianh);
                 SofiaSendBye(sobj->sofianh);
                 return false;
               }else{
                 if(callpair->Dstcall == NULL && callpair->Orgcall){
                   CLog::Detail(THISMODULE,"SofiaSiphandler::ForkingResponse %s first 200 OK add to connect list", sobj->uid.c_str());
                   SofiaSendAck(sobj->sofianh);
                   sobj->callstate = EN_CS_CONNECTED;
                   CallListCancle(sobj->uid);
                   // Send cancle to all others.
                   return true;
                 }else if(callpair->Dstcall && callpair->Orgcall){ // This on hold and resume
                   //			if(callpair->Dstcall->callstate ==  EN_CS_SUSPENDED || callpair->Orgcall->)
                   //TODO:Pradeepan commented for one way voice issue in call transfer UA-33 
                   //if(sobj->callstate == EN_CS_SUSPENDED)			
                     return true;
                   //RespondToPeer(EN_482_REL,sobj); // as per RFC 3261 8.2.2.2 Merged Requests
                   //return false; // Call has connected to another peer
                 }else {
                   CLog::Detail(THISMODULE,"SofiaSiphandler::ForkingResponse %s Dst found  or Org is not found", sobj->uid.c_str());
                   //                       SofiaSendAck(sobj->sofianh);
                   //                       SofiaSendBye(sobj->sofianh);
                   RespondToPeer(EN_482_REL,sobj); // as per RFC 3261 8.2.2.2 Merged Requests
                   return false;
                 }
               }
               break;
             }
      case 3:
             break;
      case 4:
             break;
      case 5:
             break;
      case 6:
             break;
      default:
             break;
    }
    if(callpair && callpair->Dstcall == NULL && callpair->Orgcall){
      if(RemoveCallListValue(sobj->uid,sobj->dialcallid) == 0)
        return true;
    }
    return false;
  }
#endif //DebugVM end
  void SofiaSiphandler::RespondToPeer(int status,xGateSofiaobj *sobj){
    if(!sobj){
      XGLOG_WARN(THISMODULE,"SofiaSiphandler::RespondToPeer Error - Invalid sobj");
      return;
    }
    XGLOG_INFO("SofiaSiphandler::RespondToPeer Event(%d - callid %s) ",(status/100), sobj->uid.c_str());
    sobj->sipeventtype = (XGATESIPEVENT)status;


    switch(status/100){
      case 1:
        RespondWith1xx(sobj);
        break;
      case 2:
        RespondWith2xx(sobj);
        break;
      case 3:
        RespondWith3xx(sobj);
        break;
      case 4:
        RespondWith4xx(sobj);
        break;
      case 5:
        RespondWith5xx(sobj);
        break;
      case 6:
        RespondWith6xx(sobj);
        break;
      default:
        break;
    }
  }
#if 0//DebugVM

  bool SofiaSiphandler::DeleteForkedSofiaObject(xGateSofiaobj *sobj){
    if(sobj){
      if(sobj->sofianh){
        SofiaBindhandle(sobj->sofianh, NULL);
        nua_handle_destroy(sobj->sofianh);
      } // Leg A handle clean up This stops the even for Leg A
      //    printf("****** delated forked LegId: %d\n",sobj->forkedLegId);
      sobj->sofianh = NULL;
      //    sobj->xGateSofiaobjInit();
      delete sobj;
      sobj = NULL;
    }
    return true;
  }
#endif //DebugVM end

  bool SofiaSiphandler::DeleteSofiaObject(xGateSofiaobj *sobj){
    if(sobj){
#ifdef XGATE_HAS_BUSINESS_SUPPORT
      //send intimation to business layer for object clean-up
      if(!sobj->cp.isTransferenabled && !sobj->cp.isForkingenabled){//Fix for trasnfer issue raised in 5.1.3 stage testing
      xGateCallObj *callobj = new xGateCallObj;
      if(!callobj){
	      XGLOG_ERROR("SofiaSiphandler::DeleteSofiaObject failed to create xGateCallObj for '%s'!",sobj->callid);
	      return false;
      }
      callobj->xGateCallObjInit();
      callobj->callid = sobj->callid;
      callobj->uid = sobj->uid;
      if(callobj->cp.call_id.empty())
        callobj->cp.call_id  = callobj->callid;
      if(callobj->cp.uid.empty())
        callobj->cp.uid = callobj->uid;
      callobj->sipeventtype = EN_BUSINESS_DEINIT;
        PostMessageToProcessor(callobj);
      }
#endif

      if(sobj->sofianh){
        nua_handle_t *handle = (nua_handle_t *)sobj->sofianh;
        //DebugVN SofiaBindhandle(sobj->sofianh, NULL);
        //DebugVM nua_handle_destroy(sobj->sofianh);
        SofiaBindhandle(handle, NULL);
        nua_handle_destroy(handle);
        //    su_free(&m_homeMemory, sobj); // Giving Assertion pls do not use this yadav

      } // Leg A handle clean up This stops the even for Leg A
    } 
    if(sobj) delete sobj;
    sobj = NULL;
    return true;
  }
#if 0//DebugVM
  bool SofiaSiphandler::CheckCodecCompatablity(xGateAVCodec *avcodec){
    // for(int i =0; i < MAXAVCODEC ; i++){
    //printf("code List %s\n", avcodec[i].name.c_str());
    if(GetCodecFromList(avcodec->payloadtype) >= 0)
      return true;
    //}
    //printf("Return false from CheckCodecCompatablity\n");
    return false;
  }
  bool SofiaSiphandler::CheckCodecCompatablity(std::vector<xGateAVCodec*> avcodec){
    //for (std::vector<xGateAVCodec*>::iterator it  =  codec.begin(); it !=codec.end(); ++it){
    if(avcodec.empty())
      return false;

    return true;
  }
  sdp_rtpmap_t *SofiaSiphandler::CheckCodecCompatablity(sdp_session_t *rsdp){
    if(rsdp){
      sdp_rtpmap_t *rtpmap = rsdp->sdp_media->m_rtpmaps;
      while(rtpmap != NULL){
        if(GetCodecFromList(rtpmap->rm_pt) >= 0)
          return rtpmap;
        rtpmap = rtpmap->rm_next;	 
      }
    }
    return NULL;
  } 
  int SofiaSiphandler::GetCodecFromList(int payload){
    int ret = -1;
    for (int index=0 ; index < MAXCODEC ; index++){
      if (xGateCodecList[index].payload == payload)
        return index;
    }
    return ret;
  }
#endif //DebugVM end
  int SofiaSiphandler::GetCodecFromList(const char* codecname , int mediaType){
    int ret = -1; 
    if(codecname == NULL)
      return ret;
    char buf[strlen(codecname)];
    for(int i = 0 ; i < strlen(codecname); i++){
      buf[i] =  toupper(codecname[i]);
    }
    for (int index = 0; index < MAXCODEC; index++){
      if(!strcmp(buf, xGateCodecList[index].codecname))
        return index;
    }
    return ret;
  }
#if 0//DebugVM start
  std::string SofiaSiphandler::Authenticating(xGateSofiaobj *sobj, sip_www_authenticate_t const *auth){
    std::string ret = "";
    if(sobj){
      if(strlen(sobj->cp.crUserName.c_str()) > 0 && strlen(sobj->cp.crPassword.c_str()) > 0){
        if(auth){
          ret = auth->au_scheme;
          sip_www_authenticate_t const *authloop = auth;
          while(authloop){
            const char * realm = strstr((char*)authloop->au_params, "realm=");
            if(realm){
              realm += 6;
              ret += ":" ;
              ret += realm;
              break;
            }
            authloop = authloop->au_next;
          }// for loop
          ret += ":" ;
          ret += sobj->cp.crUserName;
          ret += ":";
          ret += sobj->cp.crPassword;   
        }
      }
    }
    return ret;
  }
  #endif //DebugVM end
  void SofiaSiphandler::SDPPharser(sdp_session_t *rsdp,xGateSofiaobj *sobj){
    // Check the media poto type
    // UNKNOWN,TCP,Plain UDP,RTP/UDP, RTP/SAVP,UDPTL, TLS, wildcard
    XGLOG_INFO("SofiaSiphandler::SDPPharser");
    switch(rsdp->sdp_media->m_proto){
      case sdp_proto_x:{ // Unknown may be any introduce in future will have this proto type.currently SAVPF
                         XGLOG_INFO("SofiaSiphandler::SDPPharser sdp_proto_x");
                         IceCandidatePharser(rsdp,sobj);
                       }
                       break;
      case sdp_proto_srtp:{ // RTP/SAVP is the proto supported
                            XGLOG_INFO("SofiaSiphandler::SDPPharser sdp_proto_srtp");
                            IceCandidatePharser(rsdp,sobj);
                          }
                          break;
      case sdp_proto_tcp:
                          XGLOG_INFO("SofiaSiphandler::SDPPharser sdp_proto_tcp");
                          break;
      case sdp_proto_udp:
                          XGLOG_INFO("SofiaSiphandler::SDPPharser sdp_proto_udp");
                          break;
      case sdp_proto_rtp:{ // RTP/AVP is the proto supported
                           XGLOG_INFO("SofiaSiphandler::SDPPharser sdp_proto_rtp");
                           RtpAvpPharser(rsdp,sobj);
                         }
                         break;
      case sdp_proto_udptl:
                         XGLOG_INFO("SofiaSiphandler::SDPPharser sdp_proto_udptl");
                         break;
      case sdp_proto_tls:
                         XGLOG_INFO("SofiaSiphandler::SDPPharser sdp_proto_tls");
                         break;
      case sdp_proto_any:
                         XGLOG_INFO("SofiaSiphandler::SDPPharser sdp_proto_any");
                         break;
      default:
                         XGLOG_INFO("SofiaSiphandler::SDPPharser default");
                         break;
    }

  }
  void SofiaSiphandler::IceCandidatePharser(sdp_session_t *rsdp,xGateSofiaobj *sobj){
    // rtp map is in atrribute paramater
    sdp_attribute_t* iceattributes = NULL;
    if(rsdp){
      sdp_media_t *media = rsdp->sdp_media;
      int i =0;
      while(media){
        MediaInfoParserSAVPF(media,sobj->remoteSDPinfo[i]);
        if(rsdp->sdp_connection && rsdp->sdp_connection->c_address)
          sobj->remoteSDPinfo[i].ipaddress =rsdp->sdp_connection->c_address;
        if(media->m_proto_name)
          if(strstr(media->m_proto_name,"UDP/TLS/RTP/SAVPF"))
	{
            sobj->remoteSDPinfo[i].mediamode = "DTLS";
            sobj->remoteSDPinfo[i].outproto = "DTLS";
	}
          else sobj->remoteSDPinfo[i].mediamode = "SRTP";
        media = rsdp->sdp_media->m_next;
        i++;
        if (i > 1) // since we support only 2 MEDIA group now 
          break;
      }
    }
  }
  void SofiaSiphandler::RtpAvpPharser(sdp_session_t *rsdp,xGateSofiaobj *sobj){
    sdp_rtpmap_t *rtpmaps = NULL;
    sdp_attribute_t *rtpatt = NULL;
    if(rsdp){
      if(rsdp->sdp_attributes)
        rtpatt = rsdp->sdp_attributes;
      sdp_media_t* media =  rsdp->sdp_media;
      int i =0;
      while(media){
        MediaInfoParserAVP(media,sobj->remoteSDPinfo[i]);
        if(rsdp->sdp_connection && rsdp->sdp_connection->c_address)
          sobj->remoteSDPinfo[i].ipaddress =rsdp->sdp_connection->c_address;
        sobj->remoteSDPinfo[i].mediamode = "RTP";
        if(media->m_attributes)
          rtpatt =  media->m_attributes;
        AttributeInfoParser(rtpatt,sobj->remoteSDPinfo[i]);

        media = rsdp->sdp_media->m_next;
        i++;
        if(i > 1)
          break;
      }

    }
  }
  void SofiaSiphandler::MediaInfoParserSAVPF(sdp_media_t *media, xGateSDPInfo &sdpinfo){
    sdp_attribute_t* iceattributes = NULL;
    if(media){
      sdpinfo.mediatype = media->m_type;
      if(media->m_connections && media->m_connections->c_address)
        sdpinfo.ipaddress =  media->m_connections->c_address;

      sdpinfo.port = media->m_port;

      if(media->m_attributes){
        iceattributes =  media->m_attributes;
      }
    }

    while (iceattributes){
      if(strcmp(iceattributes->a_name,"ice-ufrag") == 0)
        sdpinfo.iceufrag = iceattributes->a_value;
      if(strcmp(iceattributes->a_name,"ice-pwd") == 0)
        sdpinfo.icepwd = iceattributes->a_value;
      if(strcmp(iceattributes->a_name,"fingerprint") == 0)
        sdpinfo.fingerprint =  iceattributes->a_value;
      if(strcmp(iceattributes->a_name,"rtpmap") == 0){
        std::string tempvalue  =  iceattributes->a_value;
        xGateAVCodec *avCodec =  new xGateAVCodec();
        avCodec->ExtractCodecInfo(tempvalue);
        //        if(GetCodecFromList(avCodec->payloadtype) >= 0)
        if(GetCodecFromList(avCodec->name.c_str(),sdpinfo.mediatype) >= 0)        
          sdpinfo.avcodec.push_back(avCodec);
        else
          delete avCodec;
      }	

      if(strcmp(iceattributes->a_name,"candidate") == 0){
        std::string temp =  iceattributes->a_value;
	char* buf = strdup(temp.c_str());
	char *token = strtok(buf, " ");
        //char *token =  strtok(temp.c_str()," ");
        std::string ipaddr="",port="";
        int count  = 1 ;
        while(token){
          if (count == 9) break;
          switch(count){
            case 5:
              ipaddr = token;
              break;
            case 6:
              port = token;
              break;
            case 8:
              if(strstr(token,"relay")){
                sdpinfo.relayip = ipaddr;
                sdpinfo.relayport = atoi(port.c_str()); 
              }else if (strstr(token,"srflx")){
                sdpinfo.reflexip = ipaddr;
                sdpinfo.reflexport = atoi(port.c_str());
              }else {
                sdpinfo.ipaddress = ipaddr;
                sdpinfo.port = atoi(port.c_str());
              }
              break;
            default:
              break;
          }
          count ++;
          token = strtok(NULL," ");
        }
      }
      iceattributes = iceattributes->a_next;
    }
  }
  void SofiaSiphandler::MediaInfoParserAVP(sdp_media_t *media, xGateSDPInfo &sdpinfo){
    sdp_rtpmap_t *rtpmaps = NULL;
    sdp_attribute_t *rtpatt = NULL;
    if(media){
      sdpinfo.mediatype = media->m_type;
      if(media->m_connections && media->m_connections->c_address)
        sdpinfo.ipaddress =  media->m_connections->c_address;

      sdpinfo.port = media->m_port;
      if(media->m_rtpmaps)
        rtpmaps = media->m_rtpmaps;
      sdpinfo.mediamode = "RTP";
      if(media->m_attributes){
        rtpatt =  media->m_attributes;
      }
    }

    int i =0;
    while(rtpmaps){
      //printf("codec rtpaavp %s\n",rtpmaps->rm_encoding);
      //     if( GetCodecFromList(rtpmaps->rm_pt)>= 0)
      if( GetCodecFromList(rtpmaps->rm_encoding,sdpinfo.mediatype)>= 0){		
        xGateAVCodec *avCodec =  new xGateAVCodec();
        avCodec->SetPayload(rtpmaps->rm_pt);
        avCodec->SetCodecName(rtpmaps->rm_encoding);
        avCodec->SetSamplingrate(rtpmaps->rm_rate);		
        sdpinfo.avcodec.push_back(avCodec);
      }
      rtpmaps = rtpmaps->rm_next;
    }

  }
  void SofiaSiphandler::AttributeInfoParser(sdp_attribute_t *attribute, xGateSDPInfo &sdpinfo){

    while(attribute){
      if(attribute->a_name && !strcmp(attribute->a_name, "ptime")){
        //  sdpinfo.ptime = (int*)attribute->a_value;
        sdpinfo.ptime = atoi(attribute->a_value);
        if(sdpinfo.ptime > 30)
          sdpinfo.ptime = 0;
        break;
      }
      attribute =  attribute->a_next;
    }
  }
 
  void SofiaSiphandler::MediaRequest(xGateSofiaobj *sobj, sip_t const  *sip, XGATEMGUPDATE mgMsgType=EN_MEDIA_CREATE)
  {
    //DebugVM start TODO
    //1. Create xGateSIPMsg
    xGateSIPMsg *sMsg = NULL;
    sMsg = new xGateSIPMsg(IURDefines::MODULE_ID::UR_MODULE_SIPSIGNAL_SERVICE,IURDefines::MODULE_ID::UR_MODULE_SERVICE_CONTROLLER, xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_NEW_CALL, xGateSIPMsg::SIP_MSG_TYPE::SIP_MSG_NOTIFY); //TODO set Src and Dst
    if(!sMsg)
    {
      XGLOG_ERROR("SofiaSiphandler::ProcessInviteRequest");
      return;
    }
    
    //2. Fill call-id, port, media, codeca
    MediaDetails mdata;
    XGLOG_INFO("SofiaSiphandler::ProcessInviteRequest Call-id=%s", sobj->callid.c_str());
    //sip_t const *sip = (sip_t*)sobj->siptag;
    //duplicate the sdp structure to sofia object void pointer
    const sdp_session_t* lsdp = NULL;
    const sdp_session_t* rsdp = NULL;
    //tl_gets(tags,NUTAG_CALLSTATE_REF(sobj->Sipcallstate),SOATAG_LOCAL_SDP_REF(lsdp),SOATAG_REMOTE_SDP_REF(rsdp),TAG_END());
    
    xGateSDPInfo sdpinfo = sobj->remoteSDPinfo[0];
    //sdp_session_t* SofiaRemotesdp =  sdp_session_dup(&m_homeMemory,rsdp);
    //sdp_media_t* media =  rsdp->sdp_media;
    //XGLOG_INFO("SofiaSiphandler::ProcessInviteRequest mport=%d", media->m_port);
    XGLOG_INFO("SofiaSiphandler::ProcessInviteRequest mport=%d", sdpinfo.port);
    
    //3. Push the Msg to IURModule
    //pushModuleMsg(pMsg);
    //DebugVM TODO  sMsg->getDstModuleId().pushModuleMsg(sMsg);
    //sMsg->getSrcModuleId().handleModuleCallbackMsg(sMsg);
    //pSipMediaModule->handleModuleCallbackMsg(sMsg);
    //DebugVM end
    
  }
#if 0//DebugVM start
  /*
   * */
  void SofiaSiphandler::RequestForMedia(xGateSofiaobj *sofiaobjsiphandle,XGATEMGUPDATE mgMsgType=EN_MEDIA_CREATE){
    // For Pst call Request
    if((mgMsgType == EN_MEDIA_CREATE) ){
      if(sofiaobjsiphandle->mediastate >= EN_MED_REQ)
        return;
      else
        sofiaobjsiphandle->mediastate = EN_MED_REQ;
    }



    if(sofiaobjsiphandle->callstream == EN_INBOUND &&  CheckCodecCompatablity(sofiaobjsiphandle->remoteSDPinfo[0].avcodec) == false) {
      RespondToPeer(EN_488_REL,sofiaobjsiphandle);
      RequestTermination(sofiaobjsiphandle); // Requesting SL application to stop the processing
      DeleteSofiaObject(sofiaobjsiphandle); // Deleting the object so it won't get further process or event
      CLog::Detail(THISMODULE,"SofiaSiphandler::RequestForMedia(%s) Codec Compatablity failed ",sofiaobjsiphandle->uid.c_str());
      return;
    }

    if(PostMessageToMGCDispatcher(sofiaobjsiphandle,mgMsgType) == true){
      //printf("Post Message To dispatch Success %s\n",sofiaobjsiphandle->callid.c_str());
      CLog::Detail(THISMODULE,"SofiaSiphandler::RequestForMedia(%s) Media Direction  %d ",sofiaobjsiphandle->uid.c_str(),sofiaobjsiphandle->callstream );
    }else{
      RespondToPeer(EN_503_REL,sofiaobjsiphandle);
      RequestTermination(sofiaobjsiphandle); // Reqting SL application to stop the processing
      DeleteSofiaObject(sofiaobjsiphandle);
      CLog::Detail(THISMODULE,"SofiaSiphandler::RequestForMedia(%s) Post Message Failed  %d ",sofiaobjsiphandle->uid.c_str(),sofiaobjsiphandle->callstream );
    }


  }

  void SofiaSiphandler::OnHold_Resume(xGateSofiaobj *sobj){

    CLog::Detail(THISMODULE,"SofiaSiphandler::OnHold_Resume %s",sobj->uid.c_str() );
    bool ONHOLD = true;
    bool ONRESUME = false;
    xGateCallPair *callpair =  GetConnectedList(sobj->uid);
    xGateSofiaobj *remotePeer = NULL;
    int tempcodec = 0;
    if(callpair && callpair->Orgcall && callpair->Dstcall)
    {
      if (callpair->Orgcall ==  sobj){
        tempcodec =  sobj->cp.csCallingCodec;
        remotePeer =  callpair->Dstcall;
      }
      else if(callpair->Dstcall == sobj){
        tempcodec =  sobj->cp.csCallingCodec;
        sobj->cp.csCallingCodec =  sobj->cp.csCalledCodec;
        remotePeer = callpair->Orgcall;
      }
    }
    else if(sobj->cp.isconferenceCall)
    {
      switch(sobj->mediastate)
      {
        case EN_MED_SUSPENDED:
          CLog::Detail(THISMODULE,"SofiaSiphandler::OnHold_Resume send 200 OK for Re-Invite Suspended");
          RespondToPeer(EN_200_REL,sobj);
          sobj->cp.PbState = EN_XGATE_PB_HOLD;
          sobj->cp.confinfo.ConfCallEvent = EN_CONF_HOLD;
          RequestForMedia(sobj, EN_MEDIA_HOLD_RESUME);
	  break;
        case EN_MED_RESUMED:
          CLog::Detail(THISMODULE,"SofiaSiphandler::OnHold_Resume send 200 OK for Re-Invite Resumed");
          RespondToPeer(EN_200_REL,sobj);
          sobj->cp.PbState = EN_XGATE_PB_RESUME;
          sobj->cp.confinfo.ConfCallEvent = EN_CONF_RESUME;
          RequestForMedia(sobj, EN_MEDIA_HOLD_RESUME);
	  break;
      }
    }
    else
      return ;
    if(!remotePeer)
      return;

    if(sobj->mediastate == EN_MED_SUSPENDED) {
      sobj->localSDPinfo[0].sendreceive = "recvonly";
      RespondToPeer(EN_200_REL,sobj);
      sobj->cp.csCallingCodec =  tempcodec; 
      if(remotePeer->mediastate == EN_MED_CONNECTED){
        // remotePeer->localSDPinfo.sendreceive = "sendonly";
        // ForwardReInvite(remotePeer,ONHOLD);
        if(remotePeer->cp.isPstnNumber){
          if(remotePeer->callstream == EN_OUTBOUND)//pradeepan
            music_on_hold(remotePeer,ONHOLD);
        }
        else if(!remotePeer->cp.isPstnNumber)
        music_on_hold(remotePeer,ONHOLD);
      }
    }
    else if(sobj->mediastate == EN_MED_RESUMED){

      sobj->localSDPinfo[0].sendreceive = "sendrecv";
      RespondToPeer(EN_200_REL,sobj);
      sobj->cp.csCallingCodec =  tempcodec;
      sobj->mediastate = EN_MED_CONNECTED;

      if(remotePeer->mediastate == EN_MED_CONNECTED){
        //remotePeer->localSDPinfo.sendreceive = "sendrecv";
        //      ForwardReInvite(remotePeer,ONRESUME);
        if(remotePeer->cp.isPstnNumber){
          if(remotePeer->callstream == EN_OUTBOUND)//pradeepan
            music_on_hold(remotePeer,ONRESUME);
        }
        else if(!remotePeer->cp.isPstnNumber)      
          music_on_hold(remotePeer,ONRESUME);
      }
    }     
  }

  xGateReturn SofiaSiphandler::ForwardReInvite(xGateSofiaobj *sofiaobj, bool hold_resume){

    CLog::Detail(THISMODULE, "SofiaSiphandler::ForwardReInvite(%s) Request", sofiaobj->uid.c_str());
    sdp_session_t* psdp = NULL;
    std::string sdpstr;
    if (sofiaobj->mediastate  == EN_MED_CONNECTED){
      sdpstr = CreateLocalSdpParam(EN_SDP_DIALOUT,sofiaobj);
      //DebugVM if(sdpstr.length() > 0)
      if(1)
      {
        CLog::Detail(THISMODULE, "SofiaSiphandler::ForwardReInvite CreateLocalSdpParam(sofiaobj) == true");
        nua_set_hparams(sofiaobj->sofianh,SOATAG_USER_SDP_STR(sdpstr.c_str()), TAG_IF(hold_resume,SOATAG_HOLD("audio")),
            TAG_IF(!hold_resume,SOATAG_HOLD("")),TAG_END());
        nua_invite(sofiaobj->sofianh,NUTAG_AUTOACK(1),TAG_END());
        CLog::Detail(THISMODULE, "SofiaSiphandler::ForwardReInvite Sent out....");
      }
      else
      {
        CLog::Detail(THISMODULE, "SofiaSiphandler::ForwardReInvite CreateLocalSdpParam(sofiaobj) == FALSE");
        RespondToPeer(EN_503_REL,sofiaobj);
      }
      music_on_hold(sofiaobj,hold_resume);
    }
    return SUCCESS;
  }

  xGateReturn SofiaSiphandler::music_on_hold(xGateSofiaobj *sofiaobj, bool ONHOLD)
  {
    // will play music-on-hold or stop playing it during call-resume...
    if (sofiaobj->callstream == EN_OUTBOUND)
    {
      CLog::Detail(THISMODULE, "SofiaSiphandler::music_on_hold cp.callOnHoldFilenameCalledParty = %s", sofiaobj->cp.callOnHoldFilenameCalledParty.c_str());
      CLog::Detail(THISMODULE, "SofiaSiphandler::music_on_hold cp.callOnHoldFilenameCallingParty = %s", sofiaobj->cp.callOnHoldFilenameCallingParty.c_str());
      //    sofiaobj->cp.Legid = 1;    
      sofiaobj->cp.csfilename  = sofiaobj->cp.callOnHoldFilenameCalledParty; 
    }
    else
    { // else resume/hold from caller
      CLog::Detail(THISMODULE, "SofiaSiphandler::music_on_hold cp.callOnHoldFilenameCallingParty = %s", sofiaobj->cp.callOnHoldFilenameCallingParty.c_str());
      CLog::Detail(THISMODULE, "SofiaSiphandler::music_on_hold cp.callOnHoldFilenameCalledParty = %s", sofiaobj->cp.callOnHoldFilenameCalledParty.c_str());
      //  sofiaobj->cp.Legid = 2;
      sofiaobj->cp.csfilename =  sofiaobj->cp.callOnHoldFilenameCallingParty;
    }
    if(sofiaobj->cp.csfilename.empty())
      sofiaobj->cp.csfilename = TEM_DEFAULT_HOLD;
    if (ONHOLD)
    {
      sofiaobj->cp.PbState = EN_XGATE_PB_HOLD;
      CLog::Detail(THISMODULE, "SofiaSiphandler::music_on_hold ONHOLD ");
    }
    else // else on-resume
    {
      sofiaobj->cp.PbState = EN_XGATE_PB_RESUME;
      CLog::Detail(THISMODULE, "SofiaSiphandler::music_on_hold ONRESUME ");
    }
    RequestForMedia(sofiaobj,EN_MEDIA_HOLD_RESUME);
    sofiaobj->cp.csfilename = "";
    return SUCCESS;
  }

  xGateReturn SofiaSiphandler::PlayOnConnect(xGateSofiaobj *sobj, XGATEPLAYFILE fileType)
  {
    if(!sobj) {
      return FAIL;
    }
    CLog::Detail(THISMODULE, "SofiaSiphandler::PlayOnConnect %s -- Play Type -- %d", sobj->uid.c_str(), fileType);
    sobj->fileState = fileType;
    switch(fileType) {
      case EN_FILE_NULL:
        {
          return FAIL;
        }
      case EN_FILE_DELAY:
        {
          break;
        }
      case EN_FILE_RBT:
        {
          CLog::Detail(THISMODULE, "SofiaSiphandler::PlayOnConnect FILE_RBT");
          sobj->cp.PbState =   EN_XGATE_PB_REPEAT;
          sobj->cp.csfilename = sobj->cp.rbtFile;
          break;
        }
      case EN_FILE_HOLD:
        {
          break;
        }
      case EN_FILE_GREETING:
        {
          CLog::Detail(THISMODULE, "SofiaSiphandler::PlayOnConnect FILE_GREETING");
          sobj->cp.PbState =   EN_XGATE_PB_PLAY;
          if (sobj->cp.gm_user_company_hour)
            //sobj->cp.csfilename = sobj->cp.gm_user_greeting_file_name_user_B;
            sobj->cp.csfilename = sobj->cp.userGreetingFile;
          else
            sobj->cp.csfilename.assign(sobj->cp.gm_user_greeting_file_name_user_B_After.c_str());
          break;
        }
      case EN_FILE_CONNECTINGMSG:
        {
          CLog::Detail(THISMODULE, "SofiaSiphandler::PlayOnConnect FILE_CONNECTINGMSG");
          sobj->cp.PbState =   EN_XGATE_PB_PLAY;
          sobj->cp.csfilename = sobj->cp.userConnectingFile;
          break;
        }
      case EN_FILE_CALLFWD:
        {
          CLog::Detail(THISMODULE, "SofiaSiphandler::PlayOnConnect FILE_CALLFWD");
          sobj->cp.PbState =   EN_XGATE_PB_PLAY;
          sobj->cp.csfilename = sobj->cp.callForwardingFile;
          break;
        }
      case EN_FILE_IVR:
        {
          CLog::Detail(THISMODULE, "SofiaSiphandler::PlayOnConnect FILE_IVR");
          sobj->cp.PbState = EN_XGATE_PB_PLAY;
          if (sobj->cp.isSwitchBoardCall) {
            //sobj->cp.csfilename.assign(sobj->cp.gm_swb_company_greeting_file.c_str()); //TODO: Yoga commented this part for swb call feature testing
            sobj->cp.csfilename.assign(sobj->switchboardparam.PlayFile); //TODO: Yoga, added this for swb testing
            //sobj->cp.csfilename.assign("/pbxshare/1909/UG/1_998_1909_usergreeting1582374284.wav"); //TODO: Yoga, hardcoded for swb testing
          }
          else if (sobj->cp.isconferenceCall) {
            sobj->cp.csfilename.assign(sobj->cp.confinfo.PlayFile.c_str());
          }
          break;
        }
    }
    if (fileType != EN_FILE_DISCONNECT) {
      RequestForMedia(sobj,EN_MEDIA_UPDATE);
    }
    return SUCCESS;
  }

  xGateReturn SofiaSiphandler::PlayResponse(int status, xGateSofiaobj *sobj)
  {
    CLog::Detail(THISMODULE, "SofiaSiphandler::PlayResponse callid %s Response code:%d",sobj->uid.c_str(),status );
    if(!sobj)
      return FAIL;

    //std::vector<std::string>::iterator itr;
    //for(itr = sobj->sofiaPeerId.begin(); itr != sobj->sofiaPeerId.end(); itr++){
    xGateCallPair *callpair = GetConnectedList(sobj->uid);
    if(callpair){
      xGateSofiaobj *sofiaPeerLeg = callpair->Orgcall; 

      if(sofiaPeerLeg){
        switch(status){
          case 183:
            break;
          case 180:
            sofiaPeerLeg->cp.PbState =   EN_XGATE_PB_REPEAT;
            sofiaPeerLeg->cp.csfilename =  sofiaPeerLeg->cp.rbtFile;
            break;
          case 200:
            sofiaPeerLeg->cp.PbState =  EN_XGATE_PB_STOP;
            sofiaPeerLeg->cp.csfilename =  "";
            break;
          case 486:
            sofiaPeerLeg->cp.PbState = EN_XGATE_PB_PLAY;
            sofiaPeerLeg->cp.csfilename = TEMP_BUSY_FILE;
            sofiaPeerLeg->fileState = EN_FILE_DISCONNECT;
            break; 
          case 500:
          case 600:
            sofiaPeerLeg->cp.PbState = EN_XGATE_PB_PLAY;
            sofiaPeerLeg->cp.csfilename = TEMP_DISCONNECT;
            sofiaPeerLeg->fileState = EN_FILE_DISCONNECT;
            break;
          default:
            sofiaPeerLeg->cp.PbState = EN_XGATE_PB_PLAY;
#if 1 //TODO: Yoga, added this as part of business layer redesign
            if(!sofiaPeerLeg->cp.isconferenceCall) {
              sofiaPeerLeg->cp.csfilename = DISCONNECT_DEFAULT;
            } else {
              sofiaPeerLeg->cp.csfilename = sofiaPeerLeg->cp.confinfo.PlayFile;
            }
#else
            sofiaPeerLeg->cp.csfilename = DISCONNECT_DEFAULT;
#endif
            sofiaPeerLeg->fileState = EN_FILE_DISCONNECT;
            break;
        }
        RequestForMedia(sofiaPeerLeg,EN_MEDIA_UPDATE);
      }
      else
        CLog::Detail(THISMODULE, "SofiaSiphandler::PlayResponse No Peer available callid %s",sobj->uid.c_str());
    }// for Loop
    return SUCCESS;
  }
  void SofiaSiphandler::AppDialOnWake(std::string uid){
    // Get the Connected list and start dial if 
    CLog::Detail(THISMODULE,"SofiaSiphandler::AppDialOnWake %s",uid.c_str());
    xGateCallPair *callpair = GetConnectedList(uid);
    if (callpair == NULL)
      return;

    if(callpair->Orgcall == NULL || callpair->Dstcall != NULL)
      return;

    if(callpair->Orgcall->cp.gm_appNotificationDialOutSent)
      return;
    CLog::Detail(THISMODULE,"SofiaSiphandler::AppDialOnWake DialOut Request success %s",uid.c_str());
    xGateCallObj *temp_callObj;
    temp_callObj =  new xGateCallObj;
    temp_callObj->xGateCallObjInit();
    temp_callObj->uid = uid;
    temp_callObj->sipeventtype =EN_APPREGCOMPLET_REQ ;
    callpair->Orgcall->cp.gm_appNotificationDialOutSent = true;
    temp_callObj->cp = callpair->Orgcall->cp;
    temp_callObj->cp.m_mappedUserList =  callpair->Orgcall->cp.m_mappedUserList;
    PostMessage(temp_callObj,XGATEPOSTTYPE::EN_PROCESS);


  }

#endif
  void SofiaSiphandler::AddServerCodecToList(xGateSofiaobj *sobj){
    std::vector<xGateAVCodec*> currentList = sobj->localSDPinfo[0].avcodec;
    std::vector<xGateAVCodec*>::iterator it;
    bool bFound = false;
    int priority = 0;
    for(int i =0 ; i < MAXCODEC ; i++){
      bFound = false;
      priority = 0;
      if(sobj->cp.isPstnNumber){
        if(xGateCodecList[i].payload == sobj->cp.csCalledCodec){
          sobj->localSDPinfo[0].avcodec.clear();
          xGateAVCodec *codec =  new xGateAVCodec();
          codec->SetPayload(xGateCodecList[i].payload);
          codec->SetCodecName(xGateCodecList[i].codecname);
          codec->SetSamplingrate(xGateCodecList[i].samplingrate);
          sobj->localSDPinfo[0].avcodec.push_back(codec);
          break;
        }

      }else{// Non PSTN customer;
        for(it = currentList.begin();it != currentList.end() ;++it){

          //if(xGateCodecList[i].payload == 111 && strstr(sobj->cp.calledDeviceType.c_str(),"URDESK")== NULL )
          //  priority = 1;

          if(xGateCodecList[i].payload == (*it)->payloadtype){
            bFound = true;
            break;
          }
        }// for currentList

        if(bFound == false && xGateCodecList[i].type == 2){
          xGateAVCodec *codec =  new xGateAVCodec();
          codec->SetPayload(xGateCodecList[i].payload);
          codec->SetCodecName(xGateCodecList[i].codecname);
          codec->SetSamplingrate(xGateCodecList[i].samplingrate);
          sobj->localSDPinfo[0].avcodec.push_back(codec);
        }
      }
    }// for MAXCODEC
  }

#if 0
  xGateReturn SofiaSiphandler::CallTransfer(xGateSofiaobj *sobj,sip_t const *sip, int transferType){


    //cp.csaCalled = "";
    // cp.csaCalled += sip->sip_request->rq_url->url_host;
    // cp.csaCalled += ':';
    //cp.csaCalled += XGSIPPORT;

    //cp.cnCalled = sip->sip_to->a_url->url_user;

    sobj->cp.cnCalled =  sip->sip_refer_to->r_url->url_user;
    sobj->cp.csaCalled = sip->sip_refer_to->r_url->url_host;
    sobj->cp.PbState = EN_XGATE_PB_TRANSFER;
    xGateCallObj *callobj =  new xGateCallObj;
    callobj->xGateCallObjInit();
    callobj->callid = sobj->callid;
    callobj->uid = sobj->uid;
    callobj->cp = sobj->cp;
    callobj->sipeventtype = EN_TRANSFER_REQ;
    PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);

    return SUCCESS;
  }

  void SofiaSiphandler::updateCDR(xGateSofiaobj *sobj)
  {
    xGateCallObj *callobj =  new xGateCallObj;
    callobj->xGateCallObjInit();
    callobj->callid = sobj->callid;
    callobj->uid = sobj->uid;
    callobj->cp = sobj->cp;
    //        sobj->cdr.updateCDRInfo(sobj->cp);
    callobj->cp.cdrJsonString  =  sobj->cdr.formJsonString();
    CLog::Detail(THISMODULE,"SofiaSiphandler::updateCDR %s ",callobj->cp.cdrJsonString.c_str());
    callobj->sipeventtype = EN_CDR_UPDATE_REQ;
    PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);
  }
#endif //DebugVM end
  xGateReturn SofiaSiphandler::dtmfHandler(const char* dtmf, xGateSofiaobj* sofiaobj ){
    char *dtmfReceived =  const_cast<char*>(dtmf);
    if(!sofiaobj)
      return FAIL;
    std::string dtmfDigit = "";
    bool endDigitReceived =  false;
    if(dtmfReceived){ // on dtmf received 
      char *token;
      int count = 0;
      while(token = strtok_r(dtmfReceived,"=\r\n",&dtmfReceived)){
        switch(count){
          case 0:{
                   break;
                 }		
          case 1:{
#if 0
                   if(strstr(token,"11")!= NULL || strstr(token,"#") != NULL){ // End of Digit Collection
                     dtmfDigit += "\0";
                     endDigitReceived = true;
                   }else
#endif
                   if(token[0] != ' ')
                       dtmfDigit += token;
                   dtmfDigit += "\0";
                   break;
                 }
        }//switch case
        count += 1;
      }// while
      sofiaobj->cp.digitReceived = dtmfDigit;
    }else{ //dtmf
      return FAIL;
    } //else dtmf
#if 0 //DebugVM start
    if (sofiaobj->cp.isSwitchBoardCall && sofiaobj->switchboardparam.SwbCallEvent == EN_SWB_GETDTMF){ // Switch board dtmf collection
      sofiaobj->cp.digitReceived +=  dtmfDigit;
      if(sofiaobj->cp.digitReceived.length() == sofiaobj->switchboardparam.MaxDigit || \
          endDigitReceived == true){
        sofiaobj->cp.PbState = EN_XGATE_PB_STOP;
        sofiaobj->cp.csfilename =  "";
        sofiaobj->cp.Legid = 1; //TODO: stop rbt for origination leg
        RequestForMedia(sofiaobj, EN_MEDIA_UPDATE);
        sofiaobj->switchboardparam.SwbCallEvent = EN_SWB_GETDTMF_COMPLET;
        OnSwitchBoardCallProcess(sofiaobj);
        sofiaobj->cp.digitReceived = "";
      }
    }else if (sofiaobj->cp.isconferenceCall && sofiaobj->cp.confinfo.ConfCallEvent == EN_CONF_GETDTMF){ // Conference call dtnf collectiuon
      sofiaobj->cp.digitReceived += dtmfDigit;
      if(sofiaobj->cp.digitReceived.length()== sofiaobj->cp.confinfo.MaxDigit || \
          endDigitReceived == true){
        sofiaobj->cp.PbState = EN_XGATE_PB_STOP;
        sofiaobj->cp.csfilename =  "";
        sofiaobj->cp.Legid = 1; //TODO: stop rbt for origination leg
        RequestForMedia(sofiaobj, EN_MEDIA_UPDATE);
        sofiaobj->cp.confinfo.ConfCallEvent = EN_CONF_GETDTMF_COMPLET;
        OnConferenceCallProcess(sofiaobj);
        sofiaobj->cp.digitReceived = "";

      }
    }
#endif //DebugVM end
    return SUCCESS;
  }
#if 0//DebugVM start

void SofiaSiphandler::ProcessMessageRequest(sip_t const  *sip)
{
  CLog::Detail(THISMODULE,"SofiaSiphandler::ProcessMessageRequest");
  std::string uid = sip->sip_payload->pl_data;
  std::string devicetype = sip->sip_call_info->ci_url[0].url_scheme;
  std::string apptype = sip->sip_call_info->ci_url[0].url_host;
  std::string proxyusername = sip->sip_from->a_url->url_user;
  std::string devAppType = devicetype + ":" + apptype;

  // Get the Connected list and start dial if 
  xGateCallPair *callpair = GetConnectedList(uid);
  if (callpair == NULL)
    return;

  if(callpair->Orgcall == NULL || callpair->Dstcall != NULL)
    return;

  if(callpair->Orgcall->cp.isForkingenabled)
  {
    if((callpair->Orgcall->isPhase1InviteSend))
    {
      {
        CallForkingApp(callpair->Orgcall, proxyusername, devAppType);
      }
    }
    else
    {
      //Cache the details:
      //Save the uid, devicetype, apptye and proxyusername
    }
  }
  
  return;
}
#endif //DebugVM end

//DebugVM start
bool SofiaSiphandler::ValidateSipMsgPointer(const char *p){
  if(  p !=NULL ){
    if(strlen(p))
      return true;
  }
  return false;
}
//DebugVM end

void SofiaSiphandler::SendRegister()
{
  SipModuleProfile sipProf = URENGINE_CONFIG::instance()->getSipModConfig();
  //SipModuleProfile sipProf = IVR_CONFIG::instance()->getSipModConfig();

  xGateSofiaSipRegobj *sipregobj = new xGateSofiaSipRegobj();
  nua_handle_t *reg_handle;
  XGLOG_INFO( "SofiaSiphandler::SendRegister Entering");
#if 0
  reg_handle = nua_handle(GetAppContext(),
      NULL,NUTAG_REGISTRAR("sip:6665.UR.mundio.com:5060"), SIPTAG_TO_STR("sip:200@6665.UR.mundio.com:5060"),
      SIPTAG_FROM_STR("sip:200@6665.UR.mundio.com:5060"),
      SIPTAG_CONTACT_STR("sip:200-lb7abo31n5rf1@10.22.5.39:5060;transport=udp"),
      TAG_END());
#endif
  reg_handle = nua_handle(GetAppContext(),
      NULL,NUTAG_REGISTRAR(sipProf.registerReqUri.c_str()), SIPTAG_TO_STR(sipProf.registerToUri.c_str()),
      SIPTAG_FROM_STR(sipProf.registerToUri.c_str()),
      //SIPTAG_CONTACT_STR(sipProf.registerContactUri.c_str()),
      TAG_END());

  sipregobj->Reghandle = reg_handle;

  nua_register(reg_handle,TAG_END());
  XGLOG_INFO( "SofiaSiphandler::SendRegister Exiting");
}

void SofiaSiphandler::SendDeRegister(char *to_uri,char *from_uri,char *contact_uri)
{
  XGLOG_INFO( "SofiaSiphandler::SendDeRegister Entering");
  nua_handle_t *reg_handle;
  reg_handle = nua_handle(GetAppContext(),
      NULL,NUTAG_REGISTRAR("sip:6665.UR.mundio.com:5060"), SIPTAG_TO_STR("sip:200@6665.UR.mundio.com:5060"),
      SIPTAG_FROM_STR("sip:200@6665.UR.mundio.com:5060"),
      SIPTAG_CONTACT_STR("*;expires=0"),
      TAG_END());

  XGLOG_INFO( "SofiaSiphandler::SendDeRegister Exiting");

}

void SofiaSiphandler::SendRefreshRegister(char *to_uri,char *from_uri,char *contact_uri)
{
  XGLOG_INFO( "SofiaSiphandler::SendRefreshRegister Entering");
  
  XGLOG_INFO( "SofiaSiphandler::SendRefreshRegister Exiting");
}

//Validate IP Address IPV4/IPV6
bool SofiaSiphandler::ip_validator(string ipAddress)
{
  char del = ':';
  int count = 0;
  for (int i = 0; (i = ipAddress.find(del, i)) != std::string::npos; i++) {
        count++;
  }
  if(count >= 3)
    return true;
  else
   return false;
}
