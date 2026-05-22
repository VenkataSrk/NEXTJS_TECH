
#include "SofiaSiphandler.h"
#include "xglog.h"
#include <iomanip>
#define THISMODULE "SOFH"
extern "C"{
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
}
//#include "sofia.h"
#include "xGateUtil.h"

#define XGSIP_HOST() xGateUtil::getLocalIpAddr()

#define XGSIPPORT "5060"
#define XGSIPSCHEME "sip:"
#define SIP_URL "sip:*:5060" 
#define SIPS_URL "sips:*:5061;transport=tls"
#define CERT_PATH "\root"
//#define XGATE_USER_AGENT "Vectone MGC 1.1"
#define XGATE_USER_AGENT "Switchlab xGate"
#define XGATE_SOFIA_HEADER_USER_INFO "X-XG-UI"
#define XGATE_SOFIA_HEADER_PRIVATE_INFO "X-XG-PI"
#define SETPARAM    SIPTAG_ALLOW_STR("INVITE, ACK, BYE, CANCEL, OPTIONS, MESSAGE,INFO"),\
  SIPTAG_USER_AGENT_STR(XGATE_USER_AGENT), \
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
NUTAG_ALLOW_EVENTS("talk"),\
NUTAG_ALLOW_EVENTS("hold"),\
NUTAG_ALLOW_EVENTS("conference"),\
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
#define USER_NO_INPUT_ENTER_PIN "/root/wav_files/sounds/vectone/CR_input_not_received.wav"

//static RegistrationApi *g_multithreaded_ippbx_registrationApiObj;

bool StringExists(const char *p)
{
  if(p!=NULL)
    if(strlen(p))
      return true;
  return false;
}

using namespace sipreturn;
SofiaSiphandler::SofiaSiphandler():xGateSipController(){

}
SofiaSiphandler::~SofiaSiphandler(){

}
//public Implemetation SofiaSipHandler
/* Method Name: SipLibInit
 * Method used to initilize sip handler & sip stack
 */
xGateRetVal SofiaSiphandler::SipLibInit(){
  try{
    //Log Initiating sofia Libarary 
    SofiaInit(); // Initltilizing sofia Libary
    SofiaInitMemory(m_homeMemory); // Creating Memory for sofia Libary
    if((m_root =  SofiaInitRoot(this)) == NULL){ 
      // Log Error Capture
      return EN_XGATE_STATUS_ERROR;
    } // Creating Root Instance 
  } catch(...){
    XGLOG_ERROR( "SofiaSiphandler::SipLibInit(%s)", "Exception Occured");
    return EN_XGATE_STATUS_ERROR;
  }
  XGLOG_INFO( "SofiaSiphandler::SipLibInit(%s)", "Success-Init");
  return EN_XGATE_STATUS_SUCCESS;
}
xGateRetVal SofiaSiphandler::SipLibLoad(){
  try{
    //g_multithreaded_ippbx_registrationApiObj = new RegistrationApi((RegistrationApi*)this, API_PROVIDER_ID_SOFIA,"multithreaded_ippbx"); // working
    ////_callTransferApi = new CallTransferApi((CallTransferApi*)this, API_PROVIDER_ID_CALLTRANSFER,"SofiaRegistrar");

    SofiaSetLog(9); // set log level for sofia library
    if((m_nuaHandle = SofiaCreate(m_root,&SofiaSiphandler::SofiaCallBack,this)) == NULL){
      XGLOG_ERROR( "SofiaSiphandler::SipLibLoad(%s)", "Sofia Handle Null Return");

      return EN_XGATE_STATUS_SUCCESS;
    }
  }catch(...){
    XGLOG_ERROR( "SofiaSiphandler::SipLibLoad(%s)", "Exception Occured");    
    return EN_XGATE_STATUS_ERROR;
  }
  XGLOG_INFO( "SofiaSiphandler::SipLibLoad(%s)", "Success-Loaded");
  return EN_XGATE_STATUS_SUCCESS;

}
xGateRetVal SofiaSiphandler::SipLibUnLoad(){
  try{
    if(m_nuaHandle){
      SofiaDestoryhandle(m_nuaHandle);
      SofiaDeInitRoot(m_root);
      SofiaDeInitMemory(m_homeMemory);
      SofiaDeInit();
    }
  }catch(...){
    XGLOG_ERROR( "SofiaSiphandler::SipUnLibLoad(%s)", "Exception Occured");
    return EN_XGATE_STATUS_ERROR;
  }
  XGLOG_INFO( "SofiaSiphandler::SipUnLibLoad(%s)", "Success-UnLoad");
  return EN_XGATE_STATUS_SUCCESS;

}
xGateRetVal SofiaSiphandler::RunEventLoop(){
  try{
#if 0 //SRC
    GetProcessRegisterEvent();
#endif
    GetProcessEvent();
    SofiaRootStep(m_nuaHandle,m_root);

  }catch(...){
    XGLOG_ERROR( "SofiaSiphandler::RunEventLoop(%s)", "Exception Occured");
    return EN_XGATE_STATUS_ERROR;
  }
  return EN_XGATE_STATUS_SUCCESS;
}

// Private Implemntation SofiaSipHandler

/*TODO: Need to modify the paramater to set Call Paramater from
 * sip structure
 */
bool SofiaSiphandler::CreateCallobjparam(xGateCallObj *callobj,xGateSofiaobj *sobj,\
    nua_handle_t *nh, sip_t const  *sip,tagi_t  tags[]){

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
  callobj->cp.gm_mgc_ip = xGateUtil::getMgIPOnIndex(sobj->mgid);
  callobj->cp.confinfo = sobj->cp.confinfo;
  // Set Transport
  if(strcasecmp(sip->sip_contact->m_url->url_scheme,"sip") == 0){
    if(ValidateSipMsgPointer(sip->sip_contact->m_url->url_params))
      nua_set_hparams(nh,
          NUTAG_M_PARAMS(sip->sip_contact->m_url->url_params),
          TAG_END());
  }

  return true;
}
xGateSofiaobj *SofiaSiphandler::CreateSofiaobjparam(std::string callid,nua_handle_t *nh,\
    nua_magic_t  *magic,sip_t *siptag){ 

  xGateSofiaobj *sofiaobj =  new xGateSofiaobj(); // create new Dialog
  sofiaobj->callstream = EN_INBOUND;
  sofiaobj->callid = callid;

  sofiaobj->mgid = xGateUtil::selectMgId(); //Multi MG support 
  sofiaobj->sofianh = (void*)nh; // Transaction handle
  sofiaobj->siptag = (void*)siptag; // Sip tag struct information 
  sofiaobj->sofiamh = (void*)magic; // Application context

  const char *siprec_callid = NULL;
  const char *siprec_uid    = NULL;
  for(sip_unknown_t *h = siptag->sip_unknown; h!=NULL; h = h->un_next) {
      if ( strcmp(h->un_name, "siprec_callid") == 0) {
          siprec_callid = h->un_value;
      }
      if ( strcmp(h->un_name, "iuid") == 0) {
          siprec_uid = h->un_value;
      }
      XGLOG_INFO( "Unknown  header (%s): %s", h->un_name, h->un_value);
  }
  if ( siprec_callid != NULL ) {
      sofiaobj->uid = siprec_uid;
      sofiaobj->cp.siprec_callid = siprec_callid;
      XGLOG_INFO( "siprec_callid: %s", sofiaobj->cp.siprec_callid.c_str());
  }
  XGLOG_INFO("SofiaSiphandler::CreateSofiaobjparam(%s)",sofiaobj->uid.c_str());

  if(ValidateSipMsgPointer(siptag->sip_contact->m_url->url_params))
  {
    sofiaobj->siptransport = siptag->sip_contact->m_url->url_params;
    sofiaobj->cp.cnCalled = siptag->sip_contact->m_url->url_user; // added for multithreaded-ippbx
  }

  sofiaobj->P2PMedia =   0;
  //  InsertCallInfo(sofiaobj);
  InsertConnectedList(sofiaobj->uid,sofiaobj,XGATEOBJECT::EN_OT_ORIGINATION);
  char frmchKey[50] = "\0";
  sprintf(frmchKey,"%s%s\0",siptag->sip_from->a_url->url_user,siptag->sip_from->a_url->url_host);
  char tochKey[50] = "\0";
  sprintf(tochKey,"%s%s\0",siptag->sip_to->a_url->url_user,siptag->sip_to->a_url->url_host);

  printf(" Incoming call channel id %s --- %s",frmchKey , tochKey);

  SofiaBindhandle(nh,(nua_hmagic_t*)sofiaobj); 

  sofiaobj->switchboardparam.SwbCallEvent = EN_SWB_NULL;
  sofiaobj->cp.confinfo.ConfCallEvent = EN_CONF_NULL;
  sofiaobj->cp.xgateCallFeature = XGATECALLFEATURE::EN_NULL_CALL;
  sofiaobj->cp.confinfo.conf_timer_thread_1 = 0;
  sofiaobj->cp.confinfo.conf_timer_thread_2 = 0;
  return sofiaobj;
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

  if(ValidateSipMsgPointer(sip->sip_from->a_url->url_host)){
    cp.routeAddr = sip->sip_from->a_url->url_host;

  }
  cp.csaCalled = "";
  cp.csaCalled += sip->sip_request->rq_url->url_host;
  //cp.csaCalled += ':';
  // cp.csaCalled += XGSIPPORT;

  cp.cnCalled = sip->sip_to->a_url->url_user;


  cp.csCallAgenttype = 0;
  if(sip->sip_user_agent){ 
    if(sip->sip_user_agent->g_string){
      cp.callingDeviceType = sip->sip_user_agent->g_string; // Added to support Peer to Peer check in PBX Yadav
      if(strstr(sip->sip_user_agent->g_string,"URAPP")){
        cp.csCallingContact = sip->sip_contact->m_url->url_host;
        cp.csCallAgenttype = 1; //App calli;
        cp.csCallingCodec = 111;


      }
      else if(strstr(sip->sip_user_agent->g_string,"URWEB")){
        cp.csCallAgenttype = 3;
        cp.csCallingCodec = 111;
      }else
        cp.csCallingCodec = 8;
    }
  }



  // check paid service for calling party
  // TODO Need to check required or any  hard code value need
  sip_p_asserted_identity_t * Paidurl=sip_p_asserted_identity(sip);

  if(Paidurl && Paidurl->paid_url){
    cp.csaCalling += Paidurl->paid_url->url_host;
    if(Paidurl->paid_url->url_port){
      cp.csaCalling += ":";
      cp.csaCalling += Paidurl->paid_url->url_port;
    }   
    cp.cnCalling = Paidurl->paid_url->url_user;
  }else{
    cp.csaCalling += sip->sip_from->a_url->url_host; //TODO: We have enable this for live call
    if(sip->sip_from->a_url->url_port) {
      cp.csaCalling += ":";
      cp.csaCalling += sip->sip_from->a_url->url_port;
    }
    cp.cnCalling = sip->sip_from->a_url->url_user;
  }

  if(cp.Legid == -1) // This check done for outbound call on forking maded as inbound on ios.
    cp.Legid = 1;

  return true;;
}


std::string SofiaSiphandler::EncodeDialSDP(xGateSofiaobj *sobj)
{
  std::stringstream sdpsession;
  sdpsession << "v=0\r\n";
  //sdpsession << "o=SIP " <<" "<< (rand()&0xFFFFFFFF) << " "<< (rand()&0xFFFFFFFF) << " IN IPV4 " << XGSIP_HOST() << "\r\n";
  sdpsession << "s=SDP session\r\n";
  sdpsession << "t=0 0 \r\n";
  if(sobj->cp.isPstnNumber)
    sobj->localSDPinfo[0].ptime = 20;
  if(sobj->cp.csCalledCodec == 8 || sobj->cp.csCalledCodec == 18 || sobj->cp.csCalledCodec == 0){
    sdpsession << MultiCodecAVP(sobj,EN_SDP_DIALOUT);
  }
  else
  {
    sdpsession << MultiCodecSAVPF(sobj,EN_SDP_DIALOUT);
  }
  return sdpsession.str();
}

std::string SofiaSiphandler::EncodeResponseSDP(xGateSofiaobj *sobj){
  std::stringstream sdpsession;
  //sdpsession << "s=SDP session\r\n";
  sdpsession << "v=0\r\n";
  //sdpsession << "o=SIP " <<" "<< "397431008138638888" << " "<< (rand()&0xFFFFFFFF) << " IN IPV4 " << XGSIP_HOST() << "\r\n";
  sdpsession << "s=SDP session\r\n";
  sdpsession << "t=0 0 \r\n";
  sdp_session_t *rsdp = (sdp_session_t*) sobj->rsdp;
  if(rsdp){
    switch(rsdp->sdp_media->m_proto){
      case sdp_proto_x:// DTLS
        sdpsession << MultiCodecSAVPF(sobj,EN_SDP_RESPONSE);
        break;
      case sdp_proto_srtp: //SRTP
        sdpsession << MultiCodecSAVPF(sobj,EN_SDP_RESPONSE);
        break;
      case sdp_proto_rtp: // AVP
        sdpsession << MultiCodecAVP(sobj,EN_SDP_RESPONSE);
        break;
      default:
        break;
    }
  }
  return sdpsession.str();
}
std::string SofiaSiphandler::MultiCodecSAVPF(xGateSofiaobj *sobj,XGATESDPMODE sdpmode){
  std::stringstream SDPStr;

  SDPStr << EncodeSessionAttribute(sobj->localSDPinfo[0], sobj->localSDPinfo[1],sobj->cp.csCallAgenttype);
  SDPStr << EncodeConnectionInformation(sobj->localSDPinfo[0],sdpmode,sobj->cp.csCallAgenttype);
  //SDPStr<<"c=IN IP4 "<<ipaddress<<"\r\n";
  SDPStr<<"a=msid-semantic:WMS " <<sobj->localSDPinfo[0].mslabel<<"\r\n";
  // MLine Attribute
  if(sobj->localSDPinfo[0].mediatype == sdp_media_audio )
    SDPStr << EncodeMediaAttribute(sobj->localSDPinfo[0],sdpmode,sobj->cp.csCallAgenttype); //audio codec
  if(sobj->localSDPinfo[1].mediatype == sdp_media_video )
    SDPStr << EncodeMediaAttribute(sobj->localSDPinfo[1],sdpmode,sobj->cp.csCallAgenttype); //video codec

  return SDPStr.str();

}
std::string SofiaSiphandler::MultiCodecAVP(xGateSofiaobj *sobj,XGATESDPMODE sdpmode){
  std::stringstream SDPStr;
  SDPStr << EncodeConnectionInformation(sobj->localSDPinfo[0],sdpmode,sobj->cp.csCallAgenttype);
  if(sobj->localSDPinfo[0].mediatype == sdp_media_audio )
    SDPStr << EncodeMediaAttribute(sobj->localSDPinfo[0],sdpmode,sobj->cp.csCallAgenttype); //audio codec
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
std::string SofiaSiphandler::EncodeSessionAttribute(xGateSDPInfo audio, xGateSDPInfo video,int DeviceType){
  std::stringstream SDPStr;

  //unified plan 
  SDPStr <<"a=group:BUNDLE" << " ";
  (audio.mediatype == sdp_media_audio)  ? SDPStr << "0" : SDPStr << "";
  (video.mediatype == sdp_media_video ) ? SDPStr << " 1" : SDPStr << "";
  SDPStr <<"\r\n";
  return SDPStr.str();
}
std::string SofiaSiphandler::EncodeMediaAttribute(xGateSDPInfo sdpinfo, XGATESDPMODE sdpmode,int DeviceType){
  std::stringstream SDPStr;
  std::string ipaddress;
  DeviceType = 0; //for testing
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
    SDPStr << (*it)->payloadtype  << " ";
  }
  (sdpinfo.mediatype == sdp_media_audio) ? SDPStr << "101\r\n" : SDPStr << "\r\n";

  for (it = sdpinfo.avcodec.begin(); it != sdpinfo.avcodec.end(); ++it){
    SDPStr <<"a=rtpmap:" << (*it)->payloadtype << " ";
    //int index = GetCodecFromList((*it)->payloadtype);
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
    case EN_SDP_DIALOUT:
      {
        if (sobj->localSDPinfo[0].mediadialport > 0 ) {
          encodeSdp =   EncodeDialSDP(sobj);
          printf("SDP Createed %s", encodeSdp.c_str());
          XGLOG_INFO( "Created Dial_Out sdb: [%s]", encodeSdp.c_str());
        } else {
          XGLOG_INFO( "Created Dial_Out sdd failed !");
        }
        break;
      }
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


/* Used to initialize run-time environment including sockets for Sofia Library 
 * Input Param : Void
 */

void SofiaSiphandler::SofiaInit(){
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
/**/
void SofiaSiphandler::SofiaSetParam(nua_t* nuaHandle){
  nua_set_params(nuaHandle,SETPARAM);
}
/**/
void SofiaSiphandler::SofiaShutdown(nua_t *nuaHandle){
  nua_shutdown(nuaHandle);
}
/**/
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
/**/
void SofiaSiphandler::SofiaSendInvite(nua_handle_t *nh){
  nua_invite(nh,TAG_END());
}
/**/
void SofiaSiphandler::SofiaBindhandle(nua_handle_t *nh,nua_hmagic_t* sofiaobjhandle){
  nua_handle_bind(nh,sofiaobjhandle);
}
/**/
void SofiaSiphandler::SofiaReBindhandle(nua_handle_t *Old_nh,nua_handle_t *New_nh,nua_hmagic_t* sofiaobjhandle){
  Old_nh = NULL;
  //SofiaBindhandle(NULL,NULL);
  SofiaBindhandle(New_nh,sofiaobjhandle);
  Old_nh = New_nh;

}
/**/
url_t* SofiaSiphandler::SofiaCreateMake(su_home_t &su_home,const char* dialstr){

  SofiaInitMemory(su_home);
  return url_make(&su_home,dialstr);

}

//Yoga
bool SofiaSiphandler::UpdateSofiaobjParam(xGateSDPInfo &sdpinfo, xGateCallObj *callObj,int avType){
  XGLOG_INFO("SofiaSiphandler::UpdateSofiaobjParam(%s)",callObj->uid.c_str());
  if(avType == 2)
    sdpinfo  = callObj->sdpInfo[0];
  else
    sdpinfo  = callObj->sdpInfo[1];
}

void SofiaSiphandler::OnProcessEvent(xGateSofiaobj *sobj, xGateCallObj *callObj)
{
  //XGLOG_INFO("SofiaSiphandler::OnProcessEvent(%s)",sobj->uid.c_str());
  switch(callObj->sipeventtype){
    case EN_SETMEDIA_EREL:
      XGLOG_INFO("SofiaSiphandler::OnProcessEvent EN_SETMEDIA_EREL ",sobj->uid.c_str());
      if (sobj->callstate == XGATECALLSTATE::EN_CS_TERMINATED)
      {
        XGLOG_DEBUG("SofiaSiphandler::OnProcessEvent EN_SETMEDIA_EREL not processing due to call ctate TERMINATED %s", sobj->uid.c_str());
        break;
      }
      UpdateSofiaobjParam(sobj->localSDPinfo[0],callObj,2); // Copy to A's local sdp infor since it carry original sdp info
      UpdateSofiaobjParam(sobj->localSDPinfo[1],callObj,3);
      // OnB2B Call connected
      sobj->localSDPinfo[0].avcodec = sobj->remoteSDPinfo[0].avcodec;
      sobj->localSDPinfo[1].avcodec = sobj->remoteSDPinfo[1].avcodec;
      RespondToPeer(EN_200_REL, sobj);
      sobj->mediastate = EN_MED_CONNECTED;
      //PlayOnConnect(sobj,EN_FILE_GREETING);
      break;
 
    case EN_MP3_DETAIL:
      {
      XGLOG_INFO("SofiaSiphandler::OnProcessEvent EN_ MP3_DETAIL(%s) ",sobj->uid.c_str());
      //string strSsrvURL = SAVE_RECORD_URL;
      string strSsrvURL = XGCONFIG().m_srsServerInfoProfile.m_save_record_info_url;
      std::vector<NfsMetaData>::iterator it = (sobj->sipRec.vNfsMetaData.begin());
      for(; it != sobj->sipRec.vNfsMetaData.end();it++ ){
	      if(strncmp((*it).CallId.c_str(),callObj->callid.c_str(),((*it).CallId.length()))==0){
		      if((*it).CallId.empty())
			      return ;
		      if(callObj->isRecordingStatus)
		      {
		      if(!m_callInfoEncoder)
			      m_callInfoEncoder = new xGateCallInfoEncoder();
		        string nfsMetaData("");
            string nfsJsonMetaData("");
      		  (*it).FileDuration= callObj->mp3_duration;
            (*it).FileSize= callObj-> mp3_filesize;
            (*it).CreatedTime = xGateUtil::getCurrentUTCTime();
            (*it).RecStopTime = xGateUtil::getCurrentTimeStamp();
		        (*it).CallUid= sobj->uid;
		        (*it).FilePath.append(".mp3"); 
#if 0
		      nfsMetaData = m_callInfoEncoder->encode_call_info_json_msg((*it) ); 
		      cout << "$$$$$$$ NFS_JSON_DATA" << nfsMetaData << std::endl;   		     
		      URMODULE_BRIDGE::instance()->PostMessagetoHTTP(sobj->callid, strSsrvURL, nfsMetaData); 
#else
            if (1 == (*it).IsCcaas)
            {
              nfsMetaData = m_callInfoEncoder->encode_ccaas_call_info_db_msg((*it));
              URMODULE_BRIDGE::instance()->PostMessageToDB(sobj->callid, nfsMetaData, EN_XGATE_DB_QUERY_STORE_CCAAS_RECORD_DATA);
            }
            else if (1 == (*it).IsCrm)
            {
              nfsMetaData = m_callInfoEncoder->encode_crm_call_info_db_msg((*it));
              URMODULE_BRIDGE::instance()->PostMessageToDB(sobj->callid, nfsMetaData, EN_XGATE_DB_QUERY_STORE_CRM_RECORD_DATA);
            }
            else
            {
              // Posting to DB Service
              nfsMetaData = m_callInfoEncoder->encode_call_info_db_msg((*it));
              URMODULE_BRIDGE::instance()->PostMessageToDB(sobj->callid, nfsMetaData, EN_XGATE_DB_QUERY_STORE_RECORD_DATA);

              // Posting to Redis Service
              nfsJsonMetaData = m_callInfoEncoder->encode_call_info_json_msg((*it));
              URMODULE_BRIDGE::instance()->PostMessageToRedis(sobj->callid.c_str(), nfsJsonMetaData.c_str());
            }
#endif
		      }
		      sobj->sipRec.vNfsMetaData.erase(it);
		      break;
	      }
      }
      if (sobj->sipRec.vNfsMetaData.empty())
      {
	      RequestForMedia(sobj,EN_MEDIA_DELETE);
      	      XGLOG_INFO("SofiaSiphandler::OnProcessEvent EN_ MP3_DETAIL Media deleted");
      	      xGateCallPair *callpair = GetConnectedList(sobj->uid.c_str());
	      if(!callpair)
		      return;
	      if(sobj->callstream == EN_INBOUND){
		      if(callpair->Orgcall)
			      DeleteConnectedList(sobj->uid.c_str(),sobj,EN_OT_ORIGINATION);
	      }else if (sobj->callstream == EN_OUTBOUND){
		      if(callpair->Dstcall)
			      DeleteConnectedList(sobj->uid.c_str(),sobj,EN_OT_DESTINATION);
	      }

      DeleteSofiaObject(sobj);      
      XGLOG_INFO("SofiaSiphandler::OnProcessEvent Sofia object deleted");
     }
	      break;
      }
    case EN_NULL:{
                   XGLOG_INFO("SofiaSiphandler::OnProcessEvent ==> %s","EN_NULL - No Action Required");
                   break;
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


  /**/
  RETCALLBACK SofiaSiphandler::SofiaCallBack(nua_event_t   event,  
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
      case nua_i_error:
        XGLOG_ERROR("SofiaSiphandler::SofiaCallBack(Event: %s status: %d Des: %s)",nua_event_name(event),status,phrase);
        break;
      case nua_i_fork :
        break;
      case nua_i_media_error :
        break;
      case nua_i_subscription :
        break;
      case nua_i_state :
        ((SofiaSiphandler*)magic)->OnStateChange(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;
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
      case nua_i_info :
        ((SofiaSiphandler*)magic)->OnInfoRequest(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;
      case nua_i_invite :
        XGLOG_INFO( "ippbx-multithreaded : case nua_i_invite : ");
        ((SofiaSiphandler*)magic)->OnInviteRequest(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;
      case nua_i_message :
        break;
      case nua_i_method :
        break;
      case nua_i_notify :
        break;
      case nua_i_options :
        break;
      case nua_i_prack :
        break;
      case nua_i_publish :
        break;
      case nua_i_refer :
        break;
      case nua_i_register :
        break;
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
      case nua_r_invite:

        ((SofiaSiphandler*)magic)->OnRequestResponse(status,phrase,nua,magic,nh,hmagic,sip,tags);
        break;  
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
      case nua_r_register:
        break;  
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
      default: //An unknown event can always be handled in the event callback
        XGLOG_ERROR("SofiaSiphandler::SofiaCallBack UNKOWN EVENT");
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

void SofiaSiphandler::OnInfoRequest(int status,char const *phrase,nua_t *nua,\
      nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
      tagi_t        tags[]){
  
 if(sip && sip->sip_content_type && sip->sip_payload && sip->sip_payload->pl_data){
  xGateSofiaobj *sofiaobj = NULL;
  if(hmagic){
//        sofiaobj = (xGateSofiaobj*)hmagic;
    if(sip->sip_content_type->c_subtype){
        if( strcmp(sip->sip_content_type->c_subtype, "rs-metadata+xml") == 0 ) {
//      if(strstr(sip->sip_content_type->c_subtype,"plain-text")){ // xml info
            XGLOG_INFO("SofiaSiphandler::OnInfoRequest : SIP-Info Received ");
            OnCallRecordProcess((xGateSofiaobj*)hmagic, sip->sip_payload->pl_data);
            return;
        }

     	}
    }else
    XGLOG_INFO("SofiaSiphandler::OnInfoRequest : SIP-Info message does NOT contain message-body");
  }
}

      /**/
void SofiaSiphandler::OnInviteRequest(int status,char const *phrase,nua_t *nua,\
     nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
     tagi_t        tags[])
{
    // Check the sip information is available if not return as bad request

    if(ValidateSipMessage(sip,SIPVALIDATE::FROM) == false || \
        //  ValidateSipMessage(sip,SIPVALIDATE::REQUEST)== false || 
        ValidateSipMessage(sip,SIPVALIDATE::CONTACT)== false || \
        ValidateSipMessage(sip,SIPVALIDATE::CALLID) == false){
      nua_respond(nh, 400,"Bad Request", TAG_END());
      XGLOG_ERROR("inside SofiaSiphandler::OnInviteRequest() Header validation failed; call_id = %s", sip->sip_call_id->i_id);
      return;
    }
    XGLOG_INFO("SofiaSiphandler::OnInviteRequest() %s:%s",sip->sip_from->a_url->url_user,sip->sip_from->a_url->url_host);
    // Creating call id
    std::string callidstring = CreateUniqueCallid(sip->sip_from->a_url->url_user,\
        sip->sip_from->a_url->url_host,\
        sip->sip_from->a_tag,\
        sip->sip_call_id->i_id);
    callidstring += '\0';

    XGLOG_INFO("SofiaSiphandler::OnInviteRequest() callid  %s", callidstring.c_str());

    xGateSofiaobj *sofiaobj = NULL ;
    XGATESIPEVENT sipevent; 

    if(hmagic){
      sofiaobj = (xGateSofiaobj*)hmagic;

      const char *siprec_callid = NULL;
      for(sip_unknown_t *h = sip->sip_unknown; h!=NULL; h = h->un_next) {
          if ( strcmp(h->un_name, "siprec_callid") == 0 ) {
              siprec_callid = h->un_value;
              XGLOG_INFO( "Unknown  header siprec_callid: (%s)", siprec_callid);
          }
          XGLOG_INFO( "Unknown  header (%s): %s", h->un_name, h->un_value);
      }
      if ( siprec_callid != NULL ) {
          ((xGateSofiaobj*)hmagic)->cp.siprec_callid = siprec_callid;
          XGLOG_INFO( "siprec_callid: %s", ((xGateSofiaobj*)hmagic)->cp.siprec_callid.c_str());
      }

      XGLOG_INFO("SofiaSiphandler::OnInviteRequest() Re-Invite Request  %s", sofiaobj->uid.c_str());

      switch(sofiaobj->callstate){
        case EN_CS_PROCEEDING:
        case EN_CS_ALERTING:
          nua_respond(nh, 400,"Bad Request", TAG_END());
          XGLOG_ERROR("inside SofiaSiphandler::OnInviteRequest()call in progress %s", sofiaobj->uid.c_str());
          return;
        case EN_CS_CONNECTED:
            sofiaobj->callstate =  EN_CS_SUSPENDED;
            sofiaobj->mediastate =  EN_MED_SUSPENDED;
            sipevent = EN_HOLDRESUME_REQ;
            XGLOG_INFO("SofiaSiphandler::OnInviteRequest() Re-Invite Suspended  %s", sofiaobj->uid.c_str());

          break;
        case EN_CS_SUSPENDED:
          // Set The call state to CS_Conecting
          sofiaobj->callstate = EN_CS_CONNECTED;
          sofiaobj->mediastate = EN_MED_RESUMED;
          sipevent = EN_HOLDRESUME_REQ;
          XGLOG_INFO("SofiaSiphandler::OnInviteRequest() Re-Invite Resumed %s" , sofiaobj->uid.c_str());
          break;

        case EN_CS_INIT: // re-invite
        default: 
          sipevent  =  EN_REINVITE_REQ;	
          XGLOG_INFO("SofiaSiphandler::OnInviteRequest() Re-Invite on timeout %s" , sofiaobj->uid.c_str());
          break;
      }

      return;
    }

   sofiaobj = CreateSofiaobjparam(callidstring,nh,magic,sip);
   sofiaobj->sipeventtype = EN_INVITE_REQ;
}

/**/
void SofiaSiphandler::OnInviteCancel(int status,char const *phrase,nua_t *nua,\
     nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
     tagi_t        tags[])
{
    XGLOG_INFO("SofiaSiphandler::OnInviteCancel(%d)",status);

    xGateCallObj *callobj =  new xGateCallObj;
    callobj->xGateCallObjInit();
    callobj->callid = ((xGateSofiaobj*)hmagic)->callid;
    callobj->cp = ((xGateSofiaobj*)hmagic)->cp;
    callobj->sipeventtype = XGATESIPEVENT::EN_TERMINATED_REQ;

    PostMessage(callobj,XGATEPOSTTYPE::EN_PROCESS);

   RequestForMedia(((xGateSofiaobj*)hmagic),EN_MEDIA_RECORD_STOP);
   ((xGateSofiaobj*)hmagic)->callstate = XGATECALLSTATE::EN_CS_TERMINATED;
   DeleteConnectedList(((xGateSofiaobj*)hmagic)->uid.c_str(),((xGateSofiaobj*)hmagic),EN_OT_ORIGINATION);
}

xGateRetVal SofiaSiphandler::CallListCancle(std::string key)
{
    XGLOG_INFO("SofiaSiphandler::CallListCancle Group id %s",key.c_str());
    std::map<std::string,xGateSofiaobj*> *calllist = GetCallList(key);
    if(calllist == NULL)
      return ;
    std::map<std::string,xGateSofiaobj*>::iterator it;
    for(it = calllist->begin(); it !=calllist->end(); ++it){
      XGLOG_INFO("SofiaSiphandler::CallListCancle Group id %s call id %s callstate %d",key.c_str(), it->first.c_str(),it->second->callstate);
      if(it->second->callstate < EN_CS_CONNECTED && it->second->callstate > EN_CS_NULL)
        calllist->erase(it);
      if(it->second->callstate > EN_CS_NULL)
        nua_cancel((nua_handle_t*)it->second->sofianh,TAG_END());
    }
    XGLOG_INFO("SofiaSiphandler::CallListCancle Group id %s Call List count %d", key.c_str(), calllist->size());
    return EN_XGATE_STATUS_SUCCESS;
}

 /* This functionality will handle the reponse event from Sip Leg's
  * and post the information to Main or Sip processor thread for further manipulation,
  * */
void SofiaSiphandler::OnRequestResponse(int status,char const *phrase,nua_t *nua,\
     nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
     tagi_t        tags[])
{
    XGLOG_INFO("SofiaSiphandler::OnRequestResponse(%d)",status);
}

void SofiaSiphandler::OnByeRequest(int status,char const *phrase,nua_t *nua,\
    nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
    tagi_t        tags[])
{

XGLOG_INFO("Inside SofiaSiphandler::OnByeRequest(%s) ",((xGateSofiaobj*)hmagic)->uid.c_str());
    if(!hmagic)
      return;
    SofiaRespond(nh,200,"OK");
    if(((xGateSofiaobj*)hmagic)->callstream == EN_INBOUND)
      CallListCancle(((xGateSofiaobj*)hmagic)->uid);

    xGateCallObj *callobj =  new xGateCallObj;
    callobj->xGateCallObjInit();
    callobj->callid = ((xGateSofiaobj*)hmagic)->callid;
    callobj->uid = ((xGateSofiaobj*)hmagic)->uid;
    callobj->sipeventtype = XGATESIPEVENT::EN_BYE_REQ;
    callobj->cp = ((xGateSofiaobj*)hmagic)->cp;
    PostMessage(callobj,EN_PROCESS);

#if 0 //Recording NFS push
      //string strSsrvURL("http://10.30.3.39:5002/v1/call_recorder/save_recorder");
      string strSsrvURL("http://ursip-api.unifiedring.co.uk/v1/call_recorder/save_recorder");
      std::vector<NfsMetaData>::iterator it = ((xGateSofiaobj*)hmagic)->sipRec.vNfsMetaData.begin();
      for(; it != ((xGateSofiaobj*)hmagic)->sipRec.vNfsMetaData.end();it++ ){
         if(!m_callInfoEncoder)
            m_callInfoEncoder = new xGateCallInfoEncoder();
	 string nfsMetaData("");        
         (*it).CallUid = ((xGateSofiaobj*)hmagic)->uid;
         (*it).FilePath.append(".mp3"); 
         nfsMetaData = m_callInfoEncoder->encode_call_info_json_msg((*it) ); 
         cout << "$$$$$$$ NFS_JSON_DATA" << nfsMetaData << std::endl;   
 
	 URMODULE_BRIDGE::instance()->PostMessagetoHTTP(((xGateSofiaobj*)hmagic)->callid, strSsrvURL, nfsMetaData); 
       }


#endif 
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
 
  //  DeleteConnectedList(((xGateSofiaobj*)hmagic)->uid,((xGateSofiaobj*)hmagic),type);
    if (((xGateSofiaobj*)hmagic)->cp.xgateCallFeature == XGATECALLFEATURE::EN_RECORD_CALL || ((xGateSofiaobj*)hmagic)->mediastate == EN_MED_CONNECTED)
    {
        RequestForMedia(((xGateSofiaobj*)hmagic),EN_MEDIA_RECORD_STOP);
    }
XGLOG_INFO("End of SofiaSiphandler::OnByeRequest(%s) ",((xGateSofiaobj*)hmagic)->uid.c_str());
//    RequestForMedia(((xGateSofiaobj*)hmagic),EN_MEDIA_DELETE);
}

void SofiaSiphandler::RequestTermination(xGateSofiaobj *sobj)
{
  xGateCallObj *callobj =  new xGateCallObj;
  callobj->xGateCallObjInit();
  callobj->callid = sobj->callid;
  callobj->uid = sobj->uid;
  callobj->cp = sobj->cp;
  callobj->sipeventtype = XGATESIPEVENT::EN_TERMINATED_REQ;

  PostMessage(callobj,EN_PROCESS);
}

/**/
void SofiaSiphandler::OnStateChange(int status,char const *phrase,nua_t *nua,\
    nua_magic_t  *magic,nua_handle_t *nh,nua_hmagic_t *hmagic,sip_t const  *sip,
    tagi_t        tags[])
{

    if(!hmagic)
      return;
    xGateSofiaobj *sofiaobjsiphandle;
    sdp_session_t* lsdp = NULL;
    sdp_session_t* rsdp = NULL;

    sofiaobjsiphandle = (xGateSofiaobj*)hmagic;
    tl_gets(tags,NUTAG_CALLSTATE_REF(sofiaobjsiphandle->Sipcallstate),SOATAG_LOCAL_SDP_REF(lsdp),SOATAG_REMOTE_SDP_REF(rsdp),TAG_END());

    cout <<"\n SofiaSiphandler::OnStateChange " << nua_callstate_name(sofiaobjsiphandle->Sipcallstate) << endl;

    XGLOG_INFO("SofiaSiphandler::OnStateChange %s of %s",nua_callstate_name(sofiaobjsiphandle->Sipcallstate),\
        sofiaobjsiphandle->uid.c_str());

    switch(sofiaobjsiphandle->Sipcallstate){
      case nua_callstate_init:
        break;
      case nua_callstate_authenticating:
        break;
      case nua_callstate_calling: // Invite Sent
        //Get lsdp
        break;
      case nua_callstate_proceeding: // 18x Received (for outgoing invite response)
          break;
      case nua_callstate_ready: // ACK send for 2xx received  or ACK received for 2xx sent
          break;
      case nua_callstate_completing: // 2xx Received (for outgoing invite response)
        break;
      case nua_callstate_received: // Invite Received (incoming call)
        XGLOG_INFO("SofiaSiphandler::OnStateChange : Invite Received (incoming call)");
        if(rsdp){
          //duplicate the sdp structure to sofia object void pointer
          sdp_session_t* SofiaRemotesdp =  sdp_session_dup(&m_homeMemory,rsdp);
          sofiaobjsiphandle->rsdp = (void*)SofiaRemotesdp;
          //SdpParser(SofiaRemotesdp,sofiaobjsiphandle->remoteSDPinfo[0]);
          SdpParser(SofiaRemotesdp,sofiaobjsiphandle);

          if(sofiaobjsiphandle->mediastate == EN_MED_SUSPENDED || sofiaobjsiphandle->mediastate == EN_MED_RESUMED )
          {
              xGateAVCodec *codec =  new xGateAVCodec();
              codec = sofiaobjsiphandle->remoteSDPinfo[0].avcodec.back();
              sofiaobjsiphandle->remoteSDPinfo[0].avcodec.clear();
              sofiaobjsiphandle->remoteSDPinfo[0].avcodec.push_back(codec);

              RequestForMedia(sofiaobjsiphandle, EN_MEDIA_RECORD);
              sofiaobjsiphandle->sipeventtype = EN_INVITE_REQ;
          } else {
            //Added to support get 2nd record port with Re-Invite
               RequestForMedia(sofiaobjsiphandle);
          }
        }
        // if(sofiaobjsiphandle->mediastate == EN_MED_SUSPENDED || sofiaobjsiphandle->mediastate == EN_MED_RESUMED )
        //   //OnHold_Resume(sofiaobjsiphandle);
        //     RequestForMedia(sofiaobjsiphandle);

        break;
      case nua_callstate_early: // 18x sent with/without  sdp (for incoming call)
        break;
      case nua_callstate_completed:  //2xx sent (for incoming call)
        break;
      case nua_callstate_terminating: // BYE send
      case nua_callstate_terminated: // ACK send for Bye Request
        if (sofiaobjsiphandle && sofiaobjsiphandle->callstate == XGATECALLSTATE::EN_CS_TERMINATED)
        {
          XGLOG_INFO("SofiaSiphandler::OnStateChange nua_callstate_terminated for uid %s", sofiaobjsiphandle->uid.c_str());
          DeleteSofiaObject(sofiaobjsiphandle);
        }
        break;
      default:
        break;
    }
}

/**/
xGateSofiaobj*  SofiaSiphandler::GetProcessEvent()
{

    xGateSofiaobj *sobj = NULL;

    xGateCallObj *callObj;
    xGateCallPair *callpair = NULL;
    try{
      if((callObj = ReadFromReceiver()) != NULL){

        if(  callpair = GetConnectedList(callObj->uid))
          sobj = callpair->Orgcall;
        // sobj=  GetCallInfo(callObj->callid);
        if(sobj){
          // sobj->callstate = callstateobj->callstate;
          // sobj->cp = callstateobj->cp;
          //sobj->sipeventtype = callstateobj->sipeventtype;
          ((SofiaSiphandler*)sobj->sofiamh)->OnProcessEvent(sobj,callObj);
          XGLOG_WARN("SofiaSiphandler", "GetProcessEvent  %s" ,callObj->uid.c_str());
        }
        if(callObj)
          delete callObj;
      }
    }catch(...){
      XGLOG_WARN("SofiaSiphandler", "SetCallState- Exception ");
      return NULL;
    }
    return sobj;
}

/**/
void SofiaSiphandler::parsestringtokens(std::string input, char delimit, std::string &token1, std::string &token2 )
{
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

/**/
bool SofiaSiphandler::ValidateSipMessage(sip_t const *sip,SIPVALIDATE header)
{
    if(sip){
      switch(header){
        case SIPVALIDATE::FROM:
  
          if(!sip->sip_from || \
              !sip->sip_from->a_url->url_user || !sip->sip_from->a_url->url_host ||\
              !sip->sip_from->a_tag){
            XGLOG_ERROR("SofiaSiphandler::ValidateSipMessage(%s)","Failed-Sip From Header Validation");
            return false;
          }
          break;
        case SIPVALIDATE::REQUEST:
          if( !sip->sip_request || \
              !sip->sip_request->rq_url->url_user || !sip->sip_request->rq_url->url_host || \
              !sip->sip_request->rq_url->url_port){
            XGLOG_ERROR("SofiaSiphandler::ValidateSipMessage(%s)","Failed-Sip Request Header Validation");
            return false;
          }
          break;
        case SIPVALIDATE::CONTACT:
          if(!sip->sip_contact){
            XGLOG_ERROR("SofiaSiphandler::ValidateSipMessage(%s)","Failed-Sip Contact Header Validation");
            return false;
          }
          break;
        case SIPVALIDATE::TO:
          if(!sip->sip_to || \
              !sip->sip_to->a_url->url_user || !sip->sip_to->a_url->url_host){
            XGLOG_ERROR("SofiaSiphandler::ValidateSipMessage(%s)","Failed-Sip To Header Validation");
            return false;
          }
          break;
        case SIPVALIDATE::TAG:
          break;
        case SIPVALIDATE::CALLID:
          if(!sip->sip_call_id ||\
              !sip->sip_call_id->i_id){
            XGLOG_ERROR("SofiaSiphandler::ValidateSipMessage(%s)","Failed-Sip Callid Header Validation");
            return false;
          }
          break;
        default:
          XGLOG_ERROR("SofiaSiphandler::ValidateSipMessage(%s)","No Header option to check");
          return false;
          break;
      } //swicth case Header
    } //Sip check point
    else{
      XGLOG_ERROR("SofiaSiphandler::ValidateSipMessage(%s)","Failed-Sip Structure Validation");
      return false;
    }
    return true;

} // ValidateSipMessage


/*------------------------------ inherted Dispatcher Functions--------------------------------------*/
/*
*/
xGateRetVal SofiaSiphandler::RespondWith1xx(xGateSofiaobj* pSipMgc )
{
    XGLOG_INFO("SofiaSiphandler::RespondWith1xx start ");
    sdp_session_t *psdp = NULL ;
    std::string sdpstr;
    switch(pSipMgc->sipeventtype){
      case XGATESIPEVENT::EN_100_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_100_REL,"Trying");
        break;
      case XGATESIPEVENT::EN_180_REL:
        break;
      case XGATESIPEVENT::EN_183_REL:
        break;
      default:
        break;

    }
    XGLOG_INFO("SofiaSiphandler::RespondWith1xx Event(%d - %s) ",pSipMgc->sipeventtype,pSipMgc->uid.c_str());
    return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal SofiaSiphandler::RespondWithInvite(xGateSofiaobj* pSipMgc )
{
    XGLOG_INFO("SofiaSiphandler::RespondWithInvite start ");

    //nua_handle_bind ((nua_handle_t*)pSipMgc->sofianh, (nua_hmagic_t *)this);
    //SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_INVITE_REQ,"Invite");
}

/*
*/
xGateRetVal SofiaSiphandler::RespondWith2xx(xGateSofiaobj* pSipMgc)
{
    XGLOG_INFO("SofiaSiphandler::RespondWith2xx start ");
    sdp_session_t *psdp = NULL ;
    std::string sdpstr;
    switch(pSipMgc->sipeventtype){
      case XGATESIPEVENT::EN_200_REL:

        sdpstr = CreateLocalSdpParam(EN_SDP_RESPONSE,pSipMgc);
        XGLOG_INFO( "Encoded SDP: %s", sdpstr.c_str());
        //sdpstr = "SRS dummy string";
        if(sdpstr.length()> 0)
        {
           xGateSipRecPair *callpair = GetSRSconnectedList(pSipMgc->uid);
           if(  !callpair ) {
               InsertSRSconnectedList(pSipMgc->uid, pSipMgc, XGATEOBJECT::EN_OT_ORIGINATION);
               callpair = GetSRSconnectedList(pSipMgc->uid);
           }
           if ( callpair && callpair->srsLeg ) {
               xGateSofiaobj *sofiaRecLeg = new xGateSofiaobj();
               if ( sofiaRecLeg == NULL ) {
                   XGLOG_INFO( "Could not create Sofiaobj Error");
                   return EN_XGATE_STATUS_ERROR;
               }
               sofiaRecLeg = pSipMgc;
               sofiaRecLeg->callid = sofiaRecLeg->cp.call_id  = pSipMgc->cp.siprec_callid;
               sofiaRecLeg->localSDPinfo[0]  = pSipMgc->localSDPinfo[0];
               sofiaRecLeg->localSDPinfo[1]  = pSipMgc->localSDPinfo[1];
               sofiaRecLeg->remoteSDPinfo[0] = pSipMgc->remoteSDPinfo[0];
               sofiaRecLeg->remoteSDPinfo[1] = pSipMgc->remoteSDPinfo[1];
               InsertSRSconnectedList(sofiaRecLeg->uid, pSipMgc, XGATEOBJECT::EN_OT_DESTINATION);
           }

           printf("\n RepondWitn2xx -- %s", sdpstr.c_str());

	   std::string contact_uri = "";
           getSipContactURLbyIPAddrFamily(pSipMgc, contact_uri);

           nua_respond((nua_handle_t*)pSipMgc->sofianh, 200, "OK", SIPTAG_CONTACT_STR(contact_uri.c_str()), SOATAG_AUDIO_AUX("telephone-event"),
               SOATAG_USER_SDP_STR(sdpstr.c_str()), NUTAG_AUTOANSWER(0),
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
    return EN_XGATE_STATUS_SUCCESS;
}

/*
*/
xGateRetVal SofiaSiphandler::RespondWith3xx(xGateSofiaobj* pSipMgc)
{
    switch(pSipMgc->sipeventtype){
      case XGATESIPEVENT::EN_300_REL:
        break;
      default:
        break;

    }
    XGLOG_INFO("SofiaSiphandler::RespondWith3xx Event(%d - %s) ",pSipMgc->sipeventtype,pSipMgc->callid.c_str());
    return EN_XGATE_STATUS_SUCCESS;
}

/*
*/
xGateRetVal SofiaSiphandler::RespondWith4xx(xGateSofiaobj* pSipMgc)
{
    XGLOG_INFO("SofiaSiphandler::RespondWith4xx()....entered ");

    if(!pSipMgc && !pSipMgc->sofianh){
      XGLOG_INFO("SofiaSiphandler::RespondWith4xx - Invalid sofia handle");
      return  EN_XGATE_STATUS_ERROR ;
    }
    if(pSipMgc->cp.isSwitchBoardCall == 1){
      pSipMgc->switchboardparam.SwbCallEvent = EN_SWB_CALLDISCONNECT;
      if(pSipMgc && pSipMgc->sofianh){
        SofiaSendBye((nua_handle_t*)pSipMgc->sofianh);
        // OnSwitchBoardCallProcess(pSipMgc->SofiaLegB);
      }
      return EN_XGATE_STATUS_SUCCESS;
    }
    //TODO: we are getting core dump here in live. so we comment.
    //XGLOG_INFO("SofiaSiphandler::RespondWith4xx Event(%d - %s) ",pSipMgc->sipeventtype,pSipMgc->callid.c_str());
    switch(pSipMgc->sipeventtype){
      case XGATESIPEVENT::EN_400_REL:
        SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_400_REL,"Bad Request");
        break;
      case XGATESIPEVENT::EN_401_REL:{
                                       std::string auth = Authenticating(pSipMgc,((sip_t*)(pSipMgc->siptag))->sip_www_authenticate);
                                       nua_authenticate((nua_handle_t*)pSipMgc->sofianh, NUTAG_AUTH(auth.c_str()), TAG_END());}
                                     //SofiaRespond((nua_handle_t*)pSipMgc->sofianh,EN_401_REL,"Unauthorized");
                                     // Return done since for this error call must not be disconnected
                                     return  EN_XGATE_STATUS_SUCCESS;
      case XGATESIPEVENT::EN_407_REL:{
                                       // return done since for this error call must not disconnected
                                       // sip_proxy_authenticate

                                       std::string auth  = Authenticating(pSipMgc,((sip_t*)(pSipMgc->siptag))->sip_proxy_authenticate);
                                       nua_authenticate((nua_handle_t*)pSipMgc->sofianh, NUTAG_AUTH(auth.c_str()), TAG_END());}
                                     return  EN_XGATE_STATUS_SUCCESS;
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

    return EN_XGATE_STATUS_SUCCESS;
}

/*
*/
xGateRetVal SofiaSiphandler::RespondWith5xx(xGateSofiaobj* pSipMgc)
{
    XGLOG_INFO("SofiaSiphandler::RespondWith5xx()....entered ");
    if(!pSipMgc && !pSipMgc->sofianh){
      XGLOG_INFO("SofiaSiphandler::RespondWith4xx - Invalid sofia handle");
      return  EN_XGATE_STATUS_ERROR;
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
    return EN_XGATE_STATUS_SUCCESS;
}

/*
*/
xGateRetVal SofiaSiphandler::RespondWith6xx(xGateSofiaobj* pSipMgc)
{
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
     return EN_XGATE_STATUS_SUCCESS;
}

void SofiaSiphandler::RespondToPeer(int status,xGateSofiaobj *sobj)
{
    if(!sobj){
      XGLOG_ERROR("SofiaSiphandler::RespondToPeer Error - Invalid sobj");
      return;
    }
    XGLOG_INFO("SofiaSiphandler::RespondToPeer Event(%d - callid %s) ",(status/100), sobj->uid.c_str());
    sobj->sipeventtype = status;

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

bool SofiaSiphandler::DeleteSofiaObject(xGateSofiaobj *sobj)
{
  XGLOG_INFO("SofiaSiphandler::%s() Entry uid:%s",__func__, sobj->uid.c_str());
    if(sobj){
      if(sobj->sofianh){
        SofiaBindhandle(sobj->sofianh, NULL);
        nua_handle_destroy(sobj->sofianh);
        //    su_free(&m_homeMemory, sobj); // Giving Assertion pls do not use this yadav

      } // Leg A handle clean up This stops the even for Leg A
    }
    if(sobj) delete sobj;
    sobj = NULL;
    return true;
}

bool SofiaSiphandler::CheckCodecCompatablity(xGateAVCodec *avcodec)
{
    // for(int i =0; i < MAXAVCODEC ; i++){

    if(GetCodecFromList(avcodec->payloadtype) >= 0)
      return true;


    return false;
}

bool SofiaSiphandler::CheckCodecCompatablity(std::vector<xGateAVCodec*> avcodec)
{
    if(avcodec.empty())
      return false;

    return true;
}

sdp_rtpmap_t *SofiaSiphandler::CheckCodecCompatablity(sdp_session_t *rsdp)
{
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

int SofiaSiphandler::GetCodecFromList(int payload)
{
    int ret = -1;
    for (int index=0 ; index < MAXCODEC ; index++){
      if (xGateCodecList[index].payload == payload)
        return index;
    }
    return ret;
}

int SofiaSiphandler::GetCodecFromList(const char* codecname , int mediaType)
{
    int ret = -1;
    if(codecname == NULL)
      return ret;
    char buf[strlen(codecname)];
    int length = sizeof(buf)/sizeof(char);
    for(int i = 0 ; i < strlen(codecname); i++){
      buf[i] =  toupper(codecname[i]);
    }
    for (int index = 0; index < MAXCODEC; index++){
      if(!strncmp(buf, xGateCodecList[index].codecname, length))
        return index;
    }
    return ret;
}

std::string SofiaSiphandler::Authenticating(xGateSofiaobj *sobj, sip_www_authenticate_t const *auth)
{
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
/**/
bool SofiaSiphandler::CreateSofiaRegobj(xGateSofiaSipRegobj &regparam,sip_t const  *sip)
{
    regparam.callid = sip->sip_call_id->i_id;

    regparam.proxyusername = sip->sip_contact->m_url->url_user;
    regparam.ipaddress = sip->sip_contact->m_url->url_host;
    regparam.contactaddress = sip->sip_contact->m_url->url_host;
    if(sip->sip_contact->m_url->url_port){
      regparam.contactaddress +=  ":";
      regparam.contactaddress += sip->sip_contact->m_url->url_port;
      printf("New Registration contact address %s\n",regparam.contactaddress.c_str());
    }

    //	regparam.domainname = sip->sip_from->a_url->url_host;
    regparam.domainname = sip->sip_request->rq_url->url_host;	
    regparam.username = sip->sip_to->a_url->url_user;
    regparam.mobileno = sip->sip_to->a_url->url_user; 
    if( sip->sip_user_agent && sip->sip_user_agent->g_string ){
      if(strstr(sip->sip_user_agent->g_string,"DCAPP"))
        regparam.devicetype =  "DCAPP" ;
      else if (strstr(sip->sip_user_agent->g_string,"VAPP"))
        regparam.devicetype =  "VAPP";
      else 
        regparam.devicetype =  "URAPP";
    }else 
      regparam.devicetype =  "URAPP";


    regparam.aaa =  EN_AAA_AUTHEN;
    regparam.Regevent = EN_VALIDATE;
    if(sip->sip_authorization && sip->sip_authorization->au_params ){
      regparam.username = msg_params_find(sip->sip_authorization->au_params,"username=");
      regparam.password =""; 
      regparam.Regevent = EN_GETUSRPWD; 
      XGLOG_INFO( "****************username == %s & password == %s", regparam.username.c_str() ,regparam.password.c_str());
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

    return true;
}

void SofiaSiphandler::SdpParser(sdp_session_t *rsdp,xGateSofiaobj *sobj)
{
    // Check the media poto type
    // UNKNOWN,TCP,Plain UDP,RTP/UDP, RTP/SAVP,UDPTL, TLS, wildcard
    XGLOG_INFO("SofiaSiphandler::SdpParser");
    switch(rsdp->sdp_media->m_proto){
      case sdp_proto_x:{ // Unknown may be any introduce in future will have this proto type.currently SAVPF
                         XGLOG_INFO("SofiaSiphandler::SdpParser sdp_proto_x");
                         IceCandidateParser(rsdp,sobj);
                       }
                       break;
      case sdp_proto_srtp:{ // RTP/SAVP is the proto supported
                            XGLOG_INFO("SofiaSiphandler::SdpParser sdp_proto_srtp");
                            IceCandidateParser(rsdp,sobj);
                          }
                          break;
      case sdp_proto_tcp:
                          XGLOG_INFO("SofiaSiphandler::SdpParser sdp_proto_tcp");
                          break;
      case sdp_proto_udp:
                          XGLOG_INFO("SofiaSiphandler::SdpParser sdp_proto_udp");
                          break;
      case sdp_proto_rtp:{ // RTP/AVP is the proto supported
                           XGLOG_INFO("SofiaSiphandler::SdpParser sdp_proto_rtp");
                           RtpAvpParser(rsdp,sobj);
                         }
                         break;
      case sdp_proto_udptl:
                         XGLOG_INFO("SofiaSiphandler::SdpParser sdp_proto_udptl");
                         break;
      case sdp_proto_tls:
                         XGLOG_INFO("SofiaSiphandler::SdpParser sdp_proto_tls");
                         break;
      case sdp_proto_any:
                         XGLOG_INFO("SofiaSiphandler::SdpParser sdp_proto_any");
                         break;
      default:
                         XGLOG_INFO("SofiaSiphandler::SdpParser default");
                         break;
    }

}

void SofiaSiphandler::IceCandidateParser(sdp_session_t *rsdp,xGateSofiaobj *sobj)
{
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
            sobj->remoteSDPinfo[i].mediamode = "DTLS";
          else sobj->remoteSDPinfo[i].mediamode = "SRTP";
        media = rsdp->sdp_media->m_next;
        i++;
        if (i > 1) // since we support only 2 MEDIA group now 
          break;
      }
    }
}

void SofiaSiphandler::RtpAvpParser(sdp_session_t *rsdp,xGateSofiaobj *sobj)
{
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

void SofiaSiphandler::MediaInfoParserSAVPF(sdp_media_t *media, xGateSDPInfo &sdpinfo)
{
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
        char *token =  strtok(temp.c_str()," ");
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

void SofiaSiphandler::MediaInfoParserAVP(sdp_media_t *media, xGateSDPInfo &sdpinfo)
{
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

void SofiaSiphandler::AttributeInfoParser(sdp_attribute_t *attribute, xGateSDPInfo &sdpinfo)
{

    bool f_ptime = false;
    bool f_label = false;
    while(attribute){
      if( !f_ptime && attribute->a_name && !strcmp(attribute->a_name, "ptime")){
        //  sdpinfo.ptime = (int*)attribute->a_value;
        sdpinfo.ptime = atoi(attribute->a_value);
        if(sdpinfo.ptime > 30)
          sdpinfo.ptime = 0;
        f_ptime = true;
        //break;
      }
      if( !f_label && attribute->a_name && !strcmp(attribute->a_name, "label") ) {
          sdpinfo.label = attribute->a_value;
          f_label = true;
      }
      attribute =  attribute->a_next;
    }
}

/*
 * */
void SofiaSiphandler::RequestForMedia(xGateSofiaobj *sofiaobjsiphandle,XGATEMGUPDATE mgMsgType=EN_MEDIA_CREATE)
{
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
      XGLOG_INFO("SofiaSiphandler::RequestForMedia(%s) Codec Compatablity failed ",sofiaobjsiphandle->uid.c_str());
      return;
    }

    if(PostMessageToMGCDispatcher(sofiaobjsiphandle,mgMsgType) == true){
      //printf("Post Message To dispatch Success %s\n",sofiaobjsiphandle->callid.c_str());
      XGLOG_INFO("SofiaSiphandler::RequestForMedia(%s) Media Direction  %d ",sofiaobjsiphandle->uid.c_str(),sofiaobjsiphandle->callstream );
    }else{
      RespondToPeer(EN_503_REL,sofiaobjsiphandle);
      RequestTermination(sofiaobjsiphandle); // Reqting SL application to stop the processing
      DeleteSofiaObject(sofiaobjsiphandle);
      XGLOG_INFO("SofiaSiphandler::RequestForMedia(%s) Post Message Failed  %d ",sofiaobjsiphandle->uid.c_str(),sofiaobjsiphandle->callstream );
    }
    //No additional task required for EN_MEDIA_RECORD_REQ_PORT
}

/*
 *  This function will handle the SIP INFO received with RS-Metadata the Recording info.
 */
void SofiaSiphandler::OnCallRecordProcess(xGateSofiaobj *sobj, const char* xml_payload)
{
    if(!sobj){
        XGLOG_ERROR("SofiaSiphandler::OnCallRecordProcess Could not intiate CallRecord NULL SofiaObject") ;
    return;
    }

    // Check the xmlinfo
    if(xml_payload == NULL){
        XGLOG_ERROR("SofiaSiphandler::OnCallRecordingProcess %s ",sobj->callid.c_str());
        return;
    }
    // parse XML string;
    if ( sobj->sipRec.infoRequest(xml_payload) ) {
        //get Participants ODR command START/STOP recording.
    }
    SipRecEvent event = sobj->sipRec.getSipInfoAction();

    xGateCallPair *callpair = GetConnectedList(sobj->uid);
    if( callpair ) {
        sobj->cp.xgateCallFeature = XGATECALLFEATURE::EN_RECORD_CALL;

        //Get the right callid for partial RS Metadata
        if (event.DataMode == "partial") sobj->cp.siprec_callid = event.CallId;
        if (event.CallType == "conference") sobj->cp.isconferenceCall = true;

        if (sobj->cp.isconferenceCall) {
            XGLOG_INFO( "SofiaSiphandler::OnCallRecordingProcess is Conference call");
        } else {
            XGLOG_INFO( "SofiaSiphandler::OnCallRecordingProcess is Normal call");
        }

	if (event.DataMode == "complete" || event.DataMode.empty() ) {
            std::vector<std::string>::iterator it = sobj->sipRec.StartParties.begin(); 
            for( ; it != sobj->sipRec.StartParties.end();it++ ) {
               std::string dummyCallid = *it;
               sobj->cp.siprec_callid = (dummyCallid);
#if 1 
               std::vector<NfsMetaData>::iterator itt = sobj->sipRec.vNfsMetaData.begin();
               for (; itt != sobj->sipRec.vNfsMetaData.end();itt++){
                   if(strcmp(sobj->cp.siprec_callid.c_str(),(*itt).CallId.c_str())==0 ){
                     sobj->cp.csfilename = (*itt).FilePath;
                     (*itt).RecStartTime = xGateUtil::getCurrentTimeStamp();
                     break;
                   }
               }
#endif
               if (!sobj->cp.csfilename.empty())
                 RequestForMedia(sobj, EN_MEDIA_RECORD_START);
               else
                 XGLOG_ERROR("SofiaSiphandler::%s() Unable to record due to file path is empty Uid: %s", __func__, sobj->uid.c_str());

               sobj->sipRec.StartedParties.push_back(dummyCallid);
            }
            sobj->sipRec.StartParties.clear();
        }
	else if (event.DataMode == "partial") {
	    if (event.CallState == SipRecEvent::HOLD)        RequestForMedia(sobj,EN_MEDIA_RECORD_HOLD);
	    else if (event.CallState == SipRecEvent::RESUME) RequestForMedia(sobj,EN_MEDIA_RECORD_RESUME);
	    else if (event.CallState == SipRecEvent::STOP)   RequestForMedia(sobj,EN_MEDIA_RECORD_DELETE);
            else
                XGLOG_INFO( "SofiaSiphandler::OnCallRecordingProcess Unknown Command Received!");
	}
    }
}

//Added for IPv6 Support
bool SofiaSiphandler::getSipContactURLbyIPAddrFamily(xGateSofiaobj *pSipMgc, string &strPbxIP)
{
        XGLOG_INFO("SofiaSiphandler::%s()",__func__);
        strPbxIP = XGCONFIG().m_srsServerInfoProfile.m_server_ip;
        char contact_uri[500];
        memset(contact_uri, 0, sizeof(contact_uri));
        snprintf(contact_uri,500, "<sip:%s>",
                        strPbxIP.c_str());
        strPbxIP = contact_uri;
        return true;
}
