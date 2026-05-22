#include "xGateSipController.h"
#include "xGateSipSignalDispatcher.h"
#include "xGateSipSignalProcessor.h"
#include "xGateMGDispatcher.h"
#include "xGateHmpProcessor.h"
//#include "xGateDBManager.h" // commented for ippbx
#include "xglog.h"
//#include "klog.h"
#define THISMODULE "SofSip"
#define CP(x,y) if(x == NULL) {CLog::Error(y); return false;}
#include <fstream>

#define SIPINFO_SIGNAL_STR      "Signal="
#define SIPINFO_DURATION_STR    "Duration="
#define SIPINFO_DURATION_PCMA8000_MIN    100
#define SIPINFO_DURATION_PCMA8000_MAX    5000
#define SIPINFO_DELIM_STR       "\r\n"

//static Rtp_Interface *global_itf;
//static	Media *global_med;
//static	unsigned int global_port;

xGateSipController::xGateSipController(){
 XGLOG_INFO("xateSipController", "CONSTRUCTOR-%s","CALLED " );
  ReadProxyConfigFile();
}
callInfoParameter::callInfoParameter(const callInfoParameter &rhs)
{
   routeAddr =rhs.routeAddr;
  csCallingContact = rhs.csCallingContact;
  callingDeviceType=rhs.callingDeviceType;
  calledDeviceType=rhs.calledDeviceType;
  csaCalled=rhs.csaCalled;
  cnCalled=rhs.cnCalled;
  siprec_callid = rhs.siprec_callid;
  csaCalling=rhs.csaCalling;
  cnCalling=rhs.cnCalling;
  proxyUserName=rhs.proxyUserName;
  routeIpAddress=rhs.routeIpAddress;
  saveIOSProxyUserName=rhs.saveIOSProxyUserName;
  call_id= rhs.call_id;
  uid = rhs.uid;
  csfilename=rhs.csfilename;
  voicemailRecordingFile=rhs.voicemailRecordingFile;
  callForwardingFile=rhs.callForwardingFile;
  callOnHoldFilenameCalledParty=rhs.callOnHoldFilenameCalledParty;
  callOnHoldFilenameCallingParty=rhs.callOnHoldFilenameCallingParty;
  rbtFile=rhs.rbtFile;
  userGreetingFile=rhs.userGreetingFile; // userConnectingFile file play back
  userConnectingFile=rhs.userConnectingFile; // userCo
  crUserName=rhs.crUserName;
  crPassword=rhs.crPassword;
  cui = rhs.cui;
  cpinfo= rhs.cpinfo;
  Legid= rhs.Legid;
  PbState= rhs.PbState;
  cntCalling= rhs.cntCalling;
  cnpCalling= rhs.cnpCalling;
  callRecording= rhs.callRecording;
  csCallAgenttype= rhs.csCallAgenttype;
  csCallingCodec= rhs.csCallingCodec;
  csCalledCodec= rhs.csCalledCodec;
  cnpCalled= rhs.cnpCalled;
  cntCalled= rhs.cntCalled;
  ccp= rhs.ccp;
  gm_site_code= rhs.gm_site_code;
  gm_switch_code= rhs.gm_switch_code;
  gm_db_selector= rhs.gm_db_selector;
  gm_trunk_in= rhs.gm_trunk_in;
  gm_port_in= rhs.gm_port_in;
  gm_trunk_out= rhs.gm_trunk_out;
  gm_port_out= rhs.gm_port_out;
  gm_mgc_ip= rhs.gm_mgc_ip;
  gm_my_ip_address= rhs.gm_my_ip_address;
  gm_A_domain_id= rhs.gm_A_domain_id;
  gm_user_A_DOMAIN_NAME= rhs.gm_user_A_DOMAIN_NAME;
  gm_B_domain_id= rhs.gm_B_domain_id;
  gm_user_B_DOMAIN_NAME= rhs.gm_user_B_DOMAIN_NAME;
  gm_swb_custom_ivr= rhs.gm_swb_custom_ivr;
  gm_swb_custom_ivr_after= rhs.gm_swb_custom_ivr_after;
  gm_call_forward_number= rhs.gm_call_forward_number;
  gm_user_greeting_file_name_user_B= rhs.gm_user_greeting_file_name_user_B;
  gm_user_greeting_file_name_user_B_After= rhs.gm_user_greeting_file_name_user_B_After;
  gm_user_connecting_file_name_user_B= rhs.gm_user_connecting_file_name_user_B;
  gm_user_rbt_file_name_user_B= rhs.gm_user_rbt_file_name_user_B;
  gm_swb_swb_id= rhs.gm_swb_swb_id;
  gm_swb_operator_extension= rhs.gm_swb_operator_extension;
  gm_swb_comp_hour_extension= rhs.gm_swb_comp_hour_extension;
  gm_swb_comp_hour_extension_after= rhs.gm_swb_comp_hour_extension_after;
  // conference members
  gm_swb_company_greeting_file= rhs.gm_swb_company_greeting_file;
  gm_user_company_hour = rhs.gm_user_company_hour;
  gm_incoming_calling_type = rhs.gm_incoming_calling_type;
  gm_user_rbt_user_B_type= rhs.gm_user_rbt_user_B_type;
  gm_swb_company_hour= rhs.gm_swb_company_hour;
  gm_swb_custom_ivr_present= rhs.gm_swb_custom_ivr_present;
  gm_swb_custom_ivr_present_after= rhs.gm_swb_custom_ivr_present_after;
  gm_swb_greeting_type= rhs.gm_swb_greeting_type;
  gm_swb_greeting_type_after= rhs.gm_swb_greeting_type_after;
  g_swb_call_state_current= rhs.g_swb_call_state_current;
  g_swb_call_state_previous= rhs.g_swb_call_state_previous;
  g_swb_SleepTimeOut_MainLoop= rhs.g_swb_SleepTimeOut_MainLoop;
  gm_swb_caller_no_action= rhs.gm_swb_caller_no_action;
  gm_swb_caller_no_action_after= rhs.gm_swb_caller_no_action_after;
  gm_swb_auto_routing= rhs.gm_swb_auto_routing;
  gm_swb_auto_routing_after= rhs.gm_swb_auto_routing_after;
  gm_dialTimeout = rhs.gm_dialTimeout;
  gm_totalNoOfRegisteredUsers = rhs.gm_totalNoOfRegisteredUsers ;
  isForkingenabled=rhs.isForkingenabled;
  isHuntGroupCall=rhs.isHuntGroupCall;
  isSwitchBoardCall=rhs.isSwitchBoardCall;
  isSwitchBoardCall_duringDialledExtension=rhs.isSwitchBoardCall_duringDialledExtension;
  isThisCallForwarded=rhs.isThisCallForwarded;
  gm_legA_deskphone_Status=rhs.gm_legA_deskphone_Status;
  gm_legB_deskphone_Status=rhs.gm_legB_deskphone_Status;
  gm_pinToConnect_dtmfDigits=rhs.gm_pinToConnect_dtmfDigits;
  gm_pin_to_connect=rhs.gm_pin_to_connect;
  gm_is_blocked_number=rhs.gm_is_blocked_number;
  gm_blocked_no_ivr=rhs.gm_blocked_no_ivr;
  gm_is_callscreening_enabled=rhs.gm_is_callscreening_enabled;

  gm_is_direct_call_status=rhs.gm_is_direct_call_status;
  gm_is_direct_call_pin_required=rhs.gm_is_direct_call_pin_required;
  gm_direct_call_pin=rhs.gm_direct_call_pin;

  isconferenceCall=rhs.isconferenceCall;
  transportType =  rhs.transportType;

  confinfo=rhs.confinfo;
  cdrJsonString = rhs.cdrJsonString;
  isPstnNumber=rhs.isPstnNumber;
  isTransferenabled =  rhs.isTransferenabled;
  inband_dtmf_method=rhs.inband_dtmf_method;
  sipinfo_method=rhs.sipinfo_method;
  gm_isThisCallForwarded_CompanyHours=rhs.gm_isThisCallForwarded_CompanyHours;
  gm_isThisCallForwarded_AfterHours=rhs.gm_isThisCallForwarded_AfterHours;
  gm_userDiallDTMFdigits=rhs.gm_userDiallDTMFdigits;
  gm_isDNDEnabled=rhs.gm_isDNDEnabled;


}
callInfoParameter &callInfoParameter::operator=(const callInfoParameter &rhs)
{
  routeAddr =rhs.routeAddr;
  csCallingContact = rhs.csCallingContact;
  callingDeviceType=rhs.callingDeviceType;
  calledDeviceType=rhs.calledDeviceType;
  csaCalled=rhs.csaCalled;
  cnCalled=rhs.cnCalled;
  siprec_callid = rhs.siprec_callid;
  csaCalling=rhs.csaCalling;
  cnCalling=rhs.cnCalling;
  proxyUserName=rhs.proxyUserName;
  routeIpAddress=rhs.routeIpAddress;
  saveIOSProxyUserName=rhs.saveIOSProxyUserName;
  call_id= rhs.call_id;
  uid = rhs.uid;
  csfilename=rhs.csfilename;
  cdrJsonString = rhs.cdrJsonString;
  voicemailRecordingFile=rhs.voicemailRecordingFile;
  callForwardingFile=rhs.callForwardingFile;
  callOnHoldFilenameCalledParty=rhs.callOnHoldFilenameCalledParty;
  callOnHoldFilenameCallingParty=rhs.callOnHoldFilenameCallingParty;
  rbtFile=rhs.rbtFile;
  userGreetingFile=rhs.userGreetingFile; // userConnectingFile file play back
  userConnectingFile=rhs.userConnectingFile; // userCo
  crUserName=rhs.crUserName;
  crPassword=rhs.crPassword;
  cui = rhs.cui;
  cpinfo= rhs.cpinfo;
  Legid= rhs.Legid;
  PbState= rhs.PbState;
  cntCalling= rhs.cntCalling;
  cnpCalling= rhs.cnpCalling;
  callRecording= rhs.callRecording;
  csCallAgenttype= rhs.csCallAgenttype;
  csCallingCodec= rhs.csCallingCodec;
  csCalledCodec= rhs.csCalledCodec;
  cnpCalled= rhs.cnpCalled;
  cntCalled= rhs.cntCalled;
  ccp= rhs.ccp;
  gm_site_code= rhs.gm_site_code;
  gm_switch_code= rhs.gm_switch_code;
  gm_db_selector= rhs.gm_db_selector;
  gm_trunk_in= rhs.gm_trunk_in;
  gm_port_in= rhs.gm_port_in;
  gm_trunk_out= rhs.gm_trunk_out;
  gm_port_out= rhs.gm_port_out;
  gm_mgc_ip= rhs.gm_mgc_ip;
  gm_my_ip_address= rhs.gm_my_ip_address;
  gm_A_domain_id= rhs.gm_A_domain_id;
  gm_user_A_DOMAIN_NAME= rhs.gm_user_A_DOMAIN_NAME;
  gm_B_domain_id= rhs.gm_B_domain_id;
  gm_user_B_DOMAIN_NAME= rhs.gm_user_B_DOMAIN_NAME;
  gm_swb_custom_ivr= rhs.gm_swb_custom_ivr;
  gm_swb_custom_ivr_after= rhs.gm_swb_custom_ivr_after;
  gm_call_forward_number= rhs.gm_call_forward_number;
  gm_user_greeting_file_name_user_B= rhs.gm_user_greeting_file_name_user_B;
  gm_user_greeting_file_name_user_B_After= rhs.gm_user_greeting_file_name_user_B_After;
  gm_user_connecting_file_name_user_B= rhs.gm_user_connecting_file_name_user_B;
  gm_user_rbt_file_name_user_B= rhs.gm_user_rbt_file_name_user_B;
  gm_swb_swb_id= rhs.gm_swb_swb_id;
  gm_swb_operator_extension= rhs.gm_swb_operator_extension;
  gm_swb_comp_hour_extension= rhs.gm_swb_comp_hour_extension;
  gm_swb_comp_hour_extension_after= rhs.gm_swb_comp_hour_extension_after;
  // conference members
  gm_swb_company_greeting_file= rhs.gm_swb_company_greeting_file;
  gm_user_company_hour = rhs.gm_user_company_hour;
  gm_incoming_calling_type = rhs.gm_incoming_calling_type;
  gm_user_rbt_user_B_type= rhs.gm_user_rbt_user_B_type;
  gm_swb_company_hour= rhs.gm_swb_company_hour;
  gm_swb_custom_ivr_present= rhs.gm_swb_custom_ivr_present;
  gm_swb_custom_ivr_present_after= rhs.gm_swb_custom_ivr_present_after;
  gm_swb_greeting_type= rhs.gm_swb_greeting_type;
  gm_swb_greeting_type_after= rhs.gm_swb_greeting_type_after;
  g_swb_call_state_current= rhs.g_swb_call_state_current;
  g_swb_call_state_previous= rhs.g_swb_call_state_previous;
  g_swb_SleepTimeOut_MainLoop= rhs.g_swb_SleepTimeOut_MainLoop;
  gm_swb_caller_no_action= rhs.gm_swb_caller_no_action;
  gm_swb_caller_no_action_after= rhs.gm_swb_caller_no_action_after;
  gm_swb_auto_routing= rhs.gm_swb_auto_routing;
  gm_swb_auto_routing_after= rhs.gm_swb_auto_routing_after;
  gm_dialTimeout = rhs.gm_dialTimeout;
  gm_totalNoOfRegisteredUsers = rhs.gm_totalNoOfRegisteredUsers ;
  isForkingenabled=rhs.isForkingenabled;
  isHuntGroupCall=rhs.isHuntGroupCall;
  isSwitchBoardCall=rhs.isSwitchBoardCall;
  isSwitchBoardCall_duringDialledExtension=rhs.isSwitchBoardCall_duringDialledExtension;
  isThisCallForwarded=rhs.isThisCallForwarded;
  gm_legA_deskphone_Status=rhs.gm_legA_deskphone_Status;
  gm_legB_deskphone_Status=rhs.gm_legB_deskphone_Status;
  gm_pinToConnect_dtmfDigits=rhs.gm_pinToConnect_dtmfDigits;
  gm_pin_to_connect=rhs.gm_pin_to_connect;
  gm_is_blocked_number=rhs.gm_is_blocked_number;
  gm_blocked_no_ivr=rhs.gm_blocked_no_ivr;
  gm_is_callscreening_enabled=rhs.gm_is_callscreening_enabled;

  gm_is_direct_call_status=rhs.gm_is_direct_call_status;
  gm_is_direct_call_pin_required=rhs.gm_is_direct_call_pin_required;
  gm_direct_call_pin=rhs.gm_direct_call_pin;

  isconferenceCall=rhs.isconferenceCall;
  transportType =  rhs.transportType;

  confinfo=rhs.confinfo;

  isPstnNumber=rhs.isPstnNumber;
  isTransferenabled =  rhs.isTransferenabled;
  inband_dtmf_method=rhs.inband_dtmf_method;
  sipinfo_method=rhs.sipinfo_method;
  gm_isThisCallForwarded_CompanyHours=rhs.gm_isThisCallForwarded_CompanyHours;
  gm_isThisCallForwarded_AfterHours=rhs.gm_isThisCallForwarded_AfterHours;
  gm_userDiallDTMFdigits=rhs.gm_userDiallDTMFdigits;
  gm_isDNDEnabled=rhs.gm_isDNDEnabled;
  //   m_mappedUserList = rhs.m_mappedUserList;

  return *this;
}
/*CDR Functionality */
xGateCDRStruct::xGateCDRStruct(){
}

xGateCDRStruct& xGateCDRStruct::operator=(const xGateCDRStruct &rhs){
 uid = rhs.uid;
 grpid =  rhs.grpid;
 callDateTime = rhs.callDateTime;
 connectedTime = rhs.connectedTime;
 disconnectedTime = rhs.disconnectedTime;
 domainName = rhs.domainName;
 callFeature = rhs.callFeature;
 pbxip = rhs.pbxip;
 operatorIP= rhs.operatorIP;
 callingCodec = rhs.callingCodec;
 calledCoded = rhs.calledCoded;
 callType = rhs.callType;
 mgip = rhs.mgip;
 callForwardNo = rhs.callForwardNo;
 disconnectReason = rhs.disconnectReason;
 digitPressed = rhs.digitPressed;
 transferNo = rhs.transferNo;
 conferenceType = rhs.conferenceType;
 conferenceNumber = rhs.conferenceNumber;
 conferenceExt = rhs.conferenceExt;

 callingNo = rhs.callingNo;
 calledNo = rhs.calledNo; 
 callDuration= rhs.callDuration;
 domainID = rhs.domainID;
 pbxid = rhs.pbxid;
 mgid = rhs.mgid;
 bundleID = rhs.bundleID;
 holdDuration = rhs.holdDuration;
 callForwarded = rhs.callForwarded;
 callRecording = rhs.callRecording;
 recordingType = rhs.recordingType;
 callTransfer = rhs.callTransfer;
 confJoinerType = rhs.confJoinerType;

return *this;
}
std::string xGateCDRStruct::setCurrentDateTime(){
std::ostringstream oss;
time_t now = time(0);
tm *ltm = localtime(&now);
oss << 1900 + ltm->tm_year << "-" <<  1 + ltm->tm_mon << "-" << ltm->tm_mday << " " \
    << 1 + ltm->tm_hour << ":" << 1 + ltm->tm_min << ":" << 1 + ltm->tm_sec;
return oss.str();

}
std::string xGateCDRStruct::setCurrentTime(){
std::ostringstream oss;
time_t now = time(0);
tm *ltm = localtime(&now);
oss << 1 + ltm->tm_hour << ":" << 1 + ltm->tm_min << ":" << 1 + ltm->tm_sec;
return oss.str();
}
void xGateCDRStruct::updateCDRInfo(const callInfoParameter &cp){
uid = cp.call_id;
 grpid = cp.uid;
 callingNo = stoi(cp.cnCalling.c_str());
 calledNo = stoi (cp.cnCalled.c_str()); 
// callDateTime;
 //connectedTime;
 //disconnectedTime;
 //callDuration;
 domainName = cp.routeAddr;
// domainID = stoi(cp.gm_A_domain_id.c_str()); 
 callFeature = "INTERNAL" ;
 pbxip = xGateUtil::getLocalIpAddr(AF_INET) ;
 pbxid= 0;
 mgip = "";
 mgid = 0;
 bundleID = 0;
 operatorIP = "";
 callingCodec = "";
 calledCoded = "";
 callType = "INBOUND";
 holdDuration = 0;
 callForwarded = 0;
 callForwardNo = "";
 disconnectReason = "NORMAL";
 digitPressed = "";
 callRecording = 0;
 recordingType = 0;
 callTransfer = 0;
 transferNo = "";
 conferenceType = "";
 confJoinerType = 0;
 conferenceNumber = "";
 conferenceExt = "";
}
std::string xGateCDRStruct::formJsonString(){
std::ostringstream oss;
#if 0
oss << KVS << KVSTR("Uid",uid) << ","  << KVINT("Calling_No",callingNo) << "," << KVINT("Called_No",calledNo) << "," \
    << KVSTR("Call_DateTime",callDateTime) << KVC  << KVSTR("Connected_Time",connectedTime) << KVC \
    << KVSTR("Disconnected_Time",disconnectedTime) << KVC  \
    << KVINT("Call_Duration",callDuration) << KVC  << KVSTR("Domain_Name",domainName) << KVC << KVINT("Domain_ID",domainID) << KVC \
    << KVSTR("CallFeature",callFeature) << KVC  << KVSTR("PBXIP",pbxip) << KVC << KVINT("PBXID",pbxid) << KVC \
    << KVSTR("MGIP",mgip)<< KVC << KVINT("MGID",mgid) << KVC  << KVINT("bundleID",bundleID) << KVC \
    << KVSTR("operatorIP",operatorIP) << KVC  << KVSTR("calling_codec",callingCodec) << KVC \
    << KVSTR("called_coded",calledCoded) << KVC  << KVSTR("calltype",callType) << KVC \
    << KVINT("holdDuration",holdDuration) << KVC << KVINT("callforwarded",callForwarded) << KVC \
    << KVSTR("callforward_No",callForwardNo) << KVC << KVSTR("Disconnect_Reason",disconnectReason) << KVC \
    << KVSTR("DTMF_Pressed",digitPressed) << KVC << KVINT("call_recording",callRecording) << KVC \
    << KVINT("recordingtype",recordingType)<< KVC << KVINT("CallTransfer",callTransfer) << KVC \
    << KVSTR("Transfer_No",transferNo) << KVC << KVINT("Conference_Type",conferenceType) << KVC \
    << KVINT("Conf_joiner_type",confJoinerType) << KVC << KVSTR("Conference_Number",conferenceNumber) << KVC\
    << KVSTR("Conference_Ext",conferenceExt) << KVC << KVSTR("GrpID",grpid) \
    << "}";
#endif
oss <<jbegin << "Uid" << jcolon << uid << jcomma << "Calling_No" << jcolon << callingNo << jcomma << "Called_No" << jcolon << calledNo << jcomma \
  << "Call_DateTime" << jcolon << callDateTime << jcomma << "Connected_Time" << jcolon << connectedTime << jcomma << "Disconnected_Time" << jcolon << disconnectedTime << jcomma \
  << "Call_Duration"<< jcolon << callDuration << jcomma << "Domain_Name" << jcolon << domainName << jcomma << "Domain_ID" << jcolon << domainID << jcomma \
  << "CallFeature"<< jcolon << callFeature << jcomma << "PBXIP" << jcolon << pbxip << jcomma << "PBXID" << jcolon << pbxid << jcomma \
  << "MGIP"<< jcolon << mgip << jcomma << "MGID"<< jcolon << mgid << jcomma << "bundleID"<< jcolon << bundleID << jcomma \
  << "operatorIP"<< jcolon << operatorIP << jcomma << "calling_codec" << jcolon << callingCodec << jcomma \
  << "called_coded"<< jcolon << calledCoded << jcomma << "calltype"<< jcolon << callType << jcomma \
  << "holdDuration"<< jcolon << holdDuration << jcomma << "callforwarded"<< jcolon << callForwarded << jcomma \
  << "callforward_No"<< jcolon << callForwardNo << jcomma << "Disconnect_Reason"<< jcolon << disconnectReason << jcomma \
  << "DTMF_Pressed"<< jcolon << digitPressed << jcomma << "call_recording"<< jcolon << callRecording << jcomma \
  << "recordingtype"<< jcolon << recordingType << jcomma << "CallTransfer"<< jcolon << callTransfer << jcomma \
  << "Transfer_No"<< jcolon << transferNo << jcomma << "Conference_Type" << jcolon << conferenceType << jcomma \
  << "Conf_joiner_type"<< jcolon << confJoinerType << jcomma << "Conference_Number"<< jcolon << conferenceNumber << jcomma \
  << "Conference_Ext"<< jcolon << conferenceExt << jcomma << "GrpID" << jcolon << grpid << jend;

std::string temp ="{ \"uid\":\"590weact-qwert-qwert\",\"Calling_No\":590,\"Called_No\":380,\"Call_DateTime\":\"2020-04-15T19:20:41.052Z\",\"Connected_Time\":\"2020-04-15T19:20:41.052Z\",\"Disconnected_Time\":\"2020-04-15T19:20:41.052Z\",\"Call_Duration\":\"0:0:0\",\"Domain_Name\":\"6367.UR.mundio.com\",\"Domain_ID\":1674,\"CallFeature\":\"INTERNAL\",\"PBXIP\":\"10.22.7.141\",\"PBXID\":1,\"MGIP\":\"10.22.7.118\",\"MGID\":1,\"bundleID\":0,\"operatorIP\":\"\",\"calling_codec\":\"G711\",\"called_coded\":\"G711\",\"calltype\":\"INBOUND\",\"holdDuration\":\"0:0:0\",\"callforwarded\":false,\"callforward_No\":\"\",\"Disconnect_Reason\":\"NORMAL\",\"DTMF_Pressed\":\"\",\"call_recording\":false,\"recordingtype\":0,\"CallTransfer\":false,\"Transfer_No\":\"\",\"Conference_Type\":\"\",\"Conf_joiner_type\":0,\"Conference_Number\":\"\",\"Conference_Ext\":\"\" }";

return temp;
//return oss.str();
}
xGateSipController::~xGateSipController(){
  XGLOG_WARN("xGateSipController", "DESTRUCTOR-%s","CALLED " );
  xGateSbcinfo *sbcinfo;
  while(sbcip.size()){
    sbcinfo = sbcip.front();
    if(sbcinfo) delete sbcinfo;
  }

  sbcip.clear();
}
void xGateSipController::ReadProxyConfigFile(){ 
  std::ifstream proxyfile;
  proxyfile.open(GetSystemconfigPath().c_str());
  std::string sLine;
  if(proxyfile.good())
    while(!proxyfile.eof()){
      getline(proxyfile,sLine);
      if(strlen(sLine.c_str()) > 0)
        if(sLine.at(0) != '#'){
          cout << sLine <<endl;
          TokenizingSettings(sLine);
        }

    }

}
void xGateSipController::TokenizingSettings(std::string sLine){
  char *scbstr = strtok(sLine.c_str(),":,;\n ");
  xGateSbcinfo *info;
  int proxytype = 0;
  int count = 0 ;
  while(scbstr !=NULL){
    switch(count){
      case 0:
        if(!strcmp(scbstr,"SBC_PROXY")){
          proxytype =  1;
        }else  if(!strcmp (scbstr,"IPPBX_PROXY")){
          proxytype = 2;
        }else if(!strcmp (scbstr,"SIGGW_PROXY")){
          proxytype = 3;
        }else if(!strcmp (scbstr,"CONFMG_IP")){
          proxytype = 4;
#if 1
        }else if(!strcmp (scbstr,"server_id")){
         proxytype = 5;
        }else if(!strcmp (scbstr,"server_dns")){
         proxytype = 6;
        }else if(!strcmp (scbstr,"server_ip")){
         proxytype = 7;
        }else if (!strcmp (scbstr,"server_mac")){
         proxytype = 8;
        }else if (!strcmp (scbstr,"present_call_load")){
         proxytype = 9;
        }else if (!strcmp (scbstr ,"max_call_load")){
         proxytype = 10;
        }else if (!strcmp (scbstr ,"callrec_srv_local_weburl")){
         proxytype = 11;
        }else if (!strcmp (scbstr ,"callrec_srv_sip_login_number")){
         proxytype = 12 ;
        }else if (!strcmp (scbstr ,"callsrv_sip_login_password")){
         proxytype = 13;
        }else if (!strcmp (scbstr ,"callsrv_registrar_sip_url")){
         proxytype = 14;
        }else if (!strcmp (scbstr ,"callsrv_registration_enabled")){
         proxytype = 15 ;
        }else if (!strcmp (scbstr ,"callsrv_server_enabled")){
         proxytype = 16;
        }else if (!strcmp (scbstr ,"callsrv_server_preference_order")){
         proxytype = 17;
#endif 
        }
        break;
      case 1:
        if(scbstr[0] == ' ')
          break;
        info =  new xGateSbcinfo(); 
        info->ipadd = scbstr;
        info->state = 1;
        if(proxytype == 1)
          sbcip.push_back(info);
        else if (proxytype == 2)
          pbxip.push_back(info);
        else if (proxytype == 3)
          siggwip.push_back(info);
        else if(proxytype == 4)
          confmgip.push_back(info);
#if 1
        else if(proxytype == 5)
          m_SRSServerInfo.m_server_id = scbstr;
        else if(proxytype == 6)
          m_SRSServerInfo.m_server_dns = scbstr;
        else if(proxytype == 7)
          m_SRSServerInfo.m_server_ip = scbstr;
        else if(proxytype == 8)
          m_SRSServerInfo.m_server_mac = scbstr;
        else if(proxytype == 9)
          m_SRSServerInfo.m_present_call_load = scbstr; 
        else if (proxytype == 10)
          m_SRSServerInfo.m_max_call_load = scbstr;
        else if(proxytype == 11)
          m_SRSServerInfo.m_callrec_srv_local_weburl = scbstr;
        else if(proxytype == 12)
          m_SRSServerInfo.m_callrec_srv_sip_login_number = scbstr;
        else if(proxytype == 13)
          m_SRSServerInfo.m_callsrv_sip_login_password = scbstr;
        else if(proxytype == 14)
          m_SRSServerInfo.m_callsrv_registrar_sip_url = scbstr;  
        else if(proxytype == 15)
          m_SRSServerInfo.m_callsrv_registration_enabled = scbstr;
        else if(proxytype == 16)
          m_SRSServerInfo.m_callsrv_server_enabled = scbstr;
        else if(proxytype == 17)
          m_SRSServerInfo.m_callsrv_server_preference_order = scbstr;
       #endif
        break;
    }    
    count = 1;
    scbstr = strtok(NULL,":,;\n ");
  }

}
bool xGateSipController::CheckDomainForCallAuth(std::string sDomainName){
  if(strstr(sDomainName.c_str(),"delightapp.vectone.com"))
    return true;
  else if (strstr(sDomainName.c_str(),"vectoneapp.vectone.com"))
    return true;
  else if(strstr(sDomainName.c_str(),"mundio.com"))
    return true;

  return false;
}
int xGateSipController::CheckforP2PMedia(xGateSofiaobj *sobj){
  if(sobj->cp.calledDeviceType.compare(sobj->cp.callingDeviceType) == 0 )// Device Type are common {
    if(sobj->cp.csfilename.empty()) // No User Greetings
      if(sobj->cp.callRecording == 0) {//  No Recording feature enabled on the Both Leg
        sip_t* siptag = (sip_t*) sobj->siptag;
        if(siptag)
          if(strcmp(siptag->sip_from->a_url->url_host,siptag->sip_to->a_url->url_host) == 0)
            return 1; // Yes it can be peer to peer
      }

  return 0; // No P2P need Media in server for call
}
std::string xGateSipController::GetRtpPortType(xGateSofiaobj *sobj)
{
  string rtpproto;
  switch(sobj->callstream){
    case EN_INBOUND: // This is Leg A Peer
      if(sobj->cp.csCallAgenttype == 2 || strstr(sobj->cp.calledDeviceType.c_str(),"URWEB") || strstr(sobj->cp.calledDeviceType.c_str(),"URAPP")) {
        rtpproto = "DTLS";
      } else { 
        rtpproto = "RTP";
      }
      break;
    case EN_OUTBOUND: // This is Leg B Peer
      if(sobj->cp.csCallAgenttype == 1 || sobj->cp.csCallAgenttype == 3) {
        rtpproto = "DTLS";
      } else {
        rtpproto = "RTP";
      }
      break;
    default:
      rtpproto = "RTP";
      break;
  }
  return rtpproto;
}
void xGateSipController::Siplogger(std::string logstr){
  // after testing use prorper log function call
  //XGLOG_WARN( "--%s--", logstr);
}
xGateSofiaobj*  xGateSipController::GetProcessEvent(){
 XGLOG_INFO("xGateSipController::GetProcessEvent(%s)","No Implemetation");

}
std::string xGateSipController::CreateUniqueCallid(char const *user,char const *host,char const *tag, char const *callid){
  std::string UniqueCallid = "";
  if( user && host && tag && callid){
    UniqueCallid=user;
    /*    UniqueCallid=user;
          UniqueCallid+="@";
          UniqueCallid+=host;
          UniqueCallid+=";";
          UniqueCallid+=tag;
          UniqueCallid+="-";*/
    UniqueCallid+=callid;
   XGLOG_INFO("xGateSipController::CreateUniqueCallid(%s)",UniqueCallid.c_str());
  }
  return UniqueCallid;
}
xGateCallPair* xGateSipController::GetConnectedList(std::string key){

  CALL_CONNECTED_LIST::ITERATOR it(m_CallConnectedList);
  xGateCallPair *callpair = NULL;
  if(m_CallConnectedList.find(key.c_str(),callpair) != -1){
   XGLOG_INFO("xGateSipController::GetConnectLsit(Unique ID = %s Object Found )",key.c_str());
  }
  return callpair;
}
bool xGateSipController::InsertConnectedList(std::string key,xGateSofiaobj* sobj,XGATEOBJECT type){
  if(!sobj)
    return false;

  xGateCallPair *callpair = NULL;
  CALL_CONNECTED_LIST::ENTRY *callEntry = 0;
  switch(type)
  {
    case EN_OT_ORIGINATION:
      callpair =  new xGateCallPair();
      if(callpair) callpair->Orgcall =  sobj;
      break;
    case EN_OT_DESTINATION:
      callpair =  GetConnectedList(key);
      if(callpair) callpair->Dstcall = sobj;
      callEntry =  (CALL_CONNECTED_LIST::ENTRY*)sobj->callEntry;
      break;
  }
  if(!callpair){
   XGLOG_INFO("xGateSipController::InsertConnectedCall(%s- Object Type %d ) (callpair NULL) failed",key.c_str(),type);
    return false;
  }
  int status = m_CallConnectedList.bind(key.c_str(),callpair,callEntry);
  if(status == 0 && callEntry){
   XGLOG_INFO("xGateSipController::InsertConnectedCall(%s- Object Type %d ) Success",key.c_str(),type);
    sobj->callEntry = (void*)callEntry;
  }
  else if(status == 1 && callEntry){
   XGLOG_INFO("xGateSipController::InsertConnectedCall(%s- Object Type %d ) Modified",key.c_str(),type);
    sobj->callEntry = (void*)callEntry;

  }else
    return false;
  return true;
}

bool xGateSipController::DeleteConnectedList(std::string key,xGateSofiaobj* sobj,XGATEOBJECT type){
  xGateCallPair * callpair =  GetConnectedList(key);
#if 0
  switch(type){
    case EN_OT_ORIGINATION:
      if(callpair)
        callpair->Orgcall = NULL;
      break;
    case EN_OT_DESTINATION:
      if(callpair)
        callpair->Dstcall = NULL;
      break;
    case EN_OT_BOTH:
      if(callpair){
        callpair->Orgcall = NULL;
        callpair->Dstcall = NULL;
      }
      break;
    default:
      break;
  }
#endif
  if(callpair){
    callpair->Orgcall = NULL;
    callpair->Dstcall = NULL;
    m_CallConnectedList.unbind(key.c_str());
   XGLOG_INFO("xGateSipController::DeleteConnectList(%s)",key.c_str());
    if (callpair) delete callpair;
    callpair = NULL;
    return true;
  }

  return false;
}

/*
 * SRS connected list to maintain Recording call connected list.
 * SRS Connected List getcall pair for UID, key
 */
xGateSipRecPair* xGateSipController::GetSRSconnectedList(std::string key)
{
    SRS_CONNECTED_LIST::ITERATOR it(m_SRScallConnectedList);
    xGateSipRecPair *callpair = NULL;
    if(m_SRScallConnectedList.find(key.c_str(),callpair) != -1){
       XGLOG_INFO("xGateSipController::GetSRSconnectList(Unique ID = %s Object Found )",key.c_str());
    }
    return callpair;
}
/*
 * SRS InsertIntoConnectedList to
 * insert a uid fromSofia object.
 */
bool xGateSipController::InsertSRSconnectedList(std:: string key, xGateSofiaobj* sobj, XGATEOBJECT type)
{
     if(!sobj)
       return false;
     // std::string     key  = sobj->uid;
     // XGATECALLSTREAM type = sobj->callstream;//

     xGateSipRecPair *callpair = NULL;
     SRS_CONNECTED_LIST::ENTRY *callEntry = 0;
     switch(type)
     {
         case EN_OT_ORIGINATION:
             callpair =  new xGateSipRecPair();
             if(callpair) callpair->srsLeg =  sobj;
         break;
         case EN_OT_DESTINATION:
             callpair =  GetSRSconnectedList(key);
             if(callpair) callpair->sipRecParties.insert(std::pair<std::string, xGateSofiaobj*>(sobj->callid, sobj));
            XGLOG_INFO("xGateSipController::InsertSRSconnectedList(Call ID = %s- Object Type %d )",sobj->callid.c_str(),type);
             callEntry =  (SRS_CONNECTED_LIST::ENTRY*)sobj->callEntry;
         break;
     }
     if(!callpair){
        XGLOG_INFO("xGateSipController::InsertSRSconnectedCall(Unique ID = %s- Object Type %d ) (callpair NULL) failed",key.c_str(),type);
         return false;
     }
     int status = m_SRScallConnectedList.bind(key.c_str(),callpair,callEntry);
     if(status == 0 && callEntry){
        XGLOG_INFO("xGateSipController::InsertSRSconnectedCall(Unique ID = %s- Object Type %d ) Success",key.c_str(),type);
         sobj->callEntry = (void*)callEntry;
     }
     else if(status == 1 && callEntry){
        XGLOG_INFO("xGateSipController::InsertSRSconnectedCall(Unique ID = %s- Object Type %d ) Modified",key.c_str(),type);
         sobj->callEntry = (void*)callEntry;
     }else {
         return false;
     }
     return true;
}

/*
 * SRS Connected List, delete UID based on input key
 */
bool xGateSipController::DeleteSRSconnectedList(std::string key, XGATEOBJECT type)
{
    xGateSipRecPair* callpair =  GetSRSconnectedList(key);

     switch(type)
     {
         case EN_OT_ORIGINATION:
             if(callpair){
                 m_SRScallConnectedList.unbind(key.c_str());
                XGLOG_INFO("xGateSipController::DeleteSRSconnectList(%s)",key.c_str());
                 callpair->sipRecParties.clear();
                 return true;
             }
         break;
         case EN_OT_DESTINATION:
             if (callpair) {
                     //delete element from vector
             }
         break;
     }

    return false;
}

std::map<std::string,xGateSofiaobj*> *xGateSipController::GetCallList(std::string grpkey){
  std::map<std::string,xGateSofiaobj*> *calllist = NULL;
  CALL_LIST::ITERATOR it(m_CallDialList);
  if(m_CallDialList.find(grpkey.c_str(),calllist) != -1)
   XGLOG_INFO("xGateSipController::GetCallList(Group ID = %s Group Found )",grpkey.c_str());

  return calllist;
}
std::map<std::string,xGateSofiaobj*> *xGateSipController::GetExtChannelList(std::string grpkey){
  std::map<std::string,xGateSofiaobj*> *calllist = NULL;
  CALL_LIST::ITERATOR it(m_ExtChannelList);
  if(m_ExtChannelList.find(grpkey.c_str(),calllist) != -1)
   XGLOG_INFO("xGateSipController::GetCallList(Group ID = %s Group Found )",grpkey.c_str());

  return calllist;
}

bool xGateSipController::InsertCallList(std::string grpkey,xGateSofiaobj *sobj){
  std::map<std::string,xGateSofiaobj*> *calllist = GetCallList(grpkey);
  if(!calllist)
    calllist = new std::map<std::string,xGateSofiaobj*>;

  calllist->insert(std::pair<std::string,xGateSofiaobj*>(sobj->dialcallid,sobj));
 XGLOG_INFO("xGateSipController::InsertCallList %s insert count %d",grpkey.c_str(),calllist->size());
  CALL_LIST::ENTRY *callEntry = 0;
  int status = m_CallDialList.bind(grpkey.c_str(),calllist,callEntry);
  if(status == 0 && callEntry)
   XGLOG_INFO("xGateSipController::InsertCallList(%s) inserted success",grpkey.c_str());
  else if(status == 1 && callEntry)
   XGLOG_INFO("xGateSipController::InsertCallList(%s) inserted Modified",grpkey.c_str());
  else{
   XGLOG_ERROR("xGateSipController::InsertCallList(%s) inserted failed",grpkey.c_str());
    return false;
  }
  return true;
}
bool xGateSipController::InsertExtChannelList(std::string grpkey,xGateSofiaobj *sobj){
  std::map<std::string,xGateSofiaobj*> *calllist = GetExtChannelList(grpkey);
  if(!calllist)
    calllist = new std::map<std::string,xGateSofiaobj*>;

  calllist->insert(std::pair<std::string,xGateSofiaobj*>(sobj->uid,sobj));
 XGLOG_INFO("xGateSipController::InsertCallList %s insert count %d",grpkey.c_str(),calllist->size());
  CALL_LIST::ENTRY *callEntry = 0;
  int status = m_ExtChannelList.bind(grpkey.c_str(),calllist,callEntry);
  if(status == 0 && callEntry)
   XGLOG_INFO("xGateSipController::InsertCallList(%s) inserted success",grpkey.c_str());
  else if(status == 1 && callEntry)
   XGLOG_INFO("xGateSipController::InsertCallList(%s) inserted Modified",grpkey.c_str());
  else{
   XGLOG_ERROR("xGateSipController::InsertCallList(%s) inserted failed",grpkey.c_str());
    return false;
  }
  return true;
}

int xGateSipController::RemoveCallListValue(std::string grpkey, std::string callid ){
 XGLOG_INFO("xGateSipController::RemoveCallListValue(%s)",grpkey.c_str());
  std::map<std::string, xGateSofiaobj*>* calllist = GetCallList(grpkey);
  int listcount  = -1;
  if(calllist == NULL)
    return listcount;
  std::map<std::string, xGateSofiaobj*>::iterator it;
  for(it = calllist->begin(); it!=calllist->end(); ++it){
    if (callid.compare(it->first) == 0){
      calllist->erase(it);
      break;
    }
  }
  listcount = calllist->size();
 XGLOG_INFO("xGateSipController::RemoveCallListValue(%s) call list count  %d",grpkey.c_str(),listcount );
  return listcount ;
}
int xGateSipController::RemoveExtChannelValue(std::string grpkey, std::string callid ){
 XGLOG_INFO("xGateSipController::RemoveCallListValue(%s)",grpkey.c_str());
  std::map<std::string, xGateSofiaobj*>* calllist = GetExtChannelList(grpkey);
  int listcount  = -1;
  if(calllist == NULL)
    return listcount;
  std::map<std::string, xGateSofiaobj*>::iterator it;
  for(it = calllist->begin(); it!=calllist->end(); ++it){
    if (callid.compare(it->first) == 0){
      calllist->erase(it);
      break;
    }
  }
  listcount = calllist->size();
 XGLOG_INFO("xGateSipController::RemoveCallListValue(%s) call list count  %d",grpkey.c_str(),listcount );
  return listcount ;
}

bool xGateSipController::RemoveCallListValues(std::string grpkey, std::map<std::string, xGateSofiaobj*>* calllist){
 XGLOG_INFO("xGateSipController::RemoveCallListValues(%s)",grpkey.c_str());
  if(calllist == NULL)
    return false;
  std::map<std::string, xGateSofiaobj*>::iterator itr;
  for(itr=calllist->begin();itr!=calllist->end();++itr){
   XGLOG_INFO("xGateSipController::RemoveCallListValues(%s) call-id %s", grpkey.c_str(), itr->first.c_str());
    //calllist->erase(itr);
  }
  calllist->clear();
  return true;
}

bool xGateSipController::DeleteCallList(std::string grpkey){
  CALL_LIST::ENTRY* callEntry = NULL;
  std::map<std::string, xGateSofiaobj*>* calllist = GetCallList(grpkey);
  if(calllist){
    RemoveCallListValues(grpkey, calllist);
    delete calllist;
  }
  if(! m_CallDialList.find(grpkey.c_str(), callEntry)) {
    if(callEntry) {
      m_CallDialList.unbind(callEntry);
     XGLOG_INFO("xGateSipController::DeleteCallList(%s)",grpkey.c_str());
    }
  }
  return true;
}
bool xGateSipController::DeleteExtChannelList(std::string grpkey){
  CALL_LIST::ENTRY* callEntry = NULL;
  std::map<std::string, xGateSofiaobj*>* calllist = GetExtChannelList(grpkey);
  if(calllist){
    RemoveCallListValues(grpkey, calllist);
    delete calllist;
  }
  if(!m_ExtChannelList.find(grpkey.c_str(), callEntry)) {
    if(callEntry) {
      m_ExtChannelList.unbind(callEntry);
     XGLOG_INFO("xGateSipController::DeleteCallList(%s)",grpkey.c_str());
    }
  }
  return true;
}

/*
   Get the call information from Hash table 
   return true if object found in table
   return false if object not found
   */
/*xGateSofiaobj *xGateSipController::GetCallInfo(std::string callid){

  HASH_CALL_MAP::ITERATOR iterator(m_incomingCallHashTable);
  xGateSofiaobj *sofiaobj = NULL;
  if(m_incomingCallHashTable.find(callid.c_str(),sofiaobj) != -1){
 XGLOG_INFO("xGateSipController::GetCallInfo(Call ID = %s Call Info Found )",callid.c_str());
  return sofiaobj;
  }
  return   sofiaobj = NULL;
//  return false;
}*/
std::map<std::string,xGateSofiaobj*> *xGateSipController::GetConferenceCallList(std::string grpkey){
  std::map<std::string,xGateSofiaobj*> *conferenceList = NULL;
  CALL_LIST::ITERATOR it(m_CallConferenceList);
  if(m_CallConferenceList.find(grpkey.c_str(),conferenceList) != -1)
    return conferenceList;
  else
    return NULL;
}
std::string xGateSipController::GetUid_FromFirstConferenceCall(std::string grpkey){
  xGateSofiaobj* sobj = GetSofiaObj_FromFirstConferenceCall(grpkey);
  if (sobj)
    return(sobj->uid);
  else
    return("");
}
xGateSofiaobj* xGateSipController::GetSofiaObj_FromFirstConferenceCall(std::string grpkey){
  std::map<std::string,xGateSofiaobj*> *conferenceList = GetConferenceCallList(grpkey);
  if(conferenceList)
  {
    for (std::map<std::string,xGateSofiaobj*>::iterator  iterator_conf = conferenceList->begin(); iterator_conf != conferenceList->end(); ++iterator_conf)
    {
      if (iterator_conf->second)
      {
        if (!(((xGateSofiaobj*) (iterator_conf->second))->uid).empty())
        {
         XGLOG_INFO("SofiaSiphandler::OnProcessEvent EN_SETMEDIA_EREL obj->uid = %s",((xGateSofiaobj*) (iterator_conf->second))->uid.c_str());
          return ((xGateSofiaobj*) (iterator_conf->second));
        }
      }
    }
  }
  return NULL;
}
bool xGateSipController::InsertCalltoConferenceList(std::string grpkey,xGateSofiaobj *sobj){
  std::map<std::string,xGateSofiaobj*> *conferencelist = GetConferenceCallList(grpkey);
  if(!conferencelist)
    conferencelist = new std::map<std::string,xGateSofiaobj*>;
  conferencelist->insert(std::pair<std::string,xGateSofiaobj*>(sobj->uid,sobj));
  CALL_LIST::ENTRY *callEntry = 0;
  int status = m_CallConferenceList.bind(grpkey.c_str(),conferencelist,callEntry);
  if(status == 0 && callEntry)
   XGLOG_INFO("xGateSipController::InsertCalltoConferenceList(%s) inserted success",grpkey.c_str());
  else if(status == 1 && callEntry)
   XGLOG_INFO("xGateSipController::InsertCalltoConferenceList(%s) inserted Modified",grpkey.c_str());
  else{
   XGLOG_ERROR("xGateSipController::InsertCalltoConferenceList(%s) inserted failed",grpkey.c_str());
    return false;
  }
  return true;
}


bool xGateSipController::DeleteCallFromConferenceList(std::string grpkey,xGateSofiaobj *sobj){
  std::map<std::string,xGateSofiaobj*> *conferencelist = GetConferenceCallList(grpkey);
  if(conferencelist == NULL || sobj == NULL)
    return false;
  std::map<std::string, xGateSofiaobj*>::iterator itr = conferencelist->find(sobj->callid.c_str());
  if(itr->second == sobj)
   XGLOG_INFO("xGateSipController::DeleteCallFromConferenceList Found call object %s", sobj->callid.c_str());
  if(itr != conferencelist->end())
    conferencelist->erase(itr);	
  return true;
}
bool xGateSipController::RemoveConferenceListValue(std::string grpkey, std::map<std::string, xGateSofiaobj*>* conferencelist){

  if(conferencelist == NULL)
    return false;
  std::map<std::string, xGateSofiaobj*>::iterator itr;
  for(itr=conferencelist->begin();itr!=conferencelist->end();++itr){
    conferencelist->erase(itr);
  }
  return true;
}
bool xGateSipController::DeleteConferenceList(std::string grpkey){
  CALL_LIST::ENTRY* callEntry = NULL;
  std::map<std::string, xGateSofiaobj*>* conferencelist =GetConferenceCallList(grpkey);
  if(conferencelist){
    //RemoveConferenceListValue(grpkey, conferencelist);
    conferencelist->clear();
    delete conferencelist;
  }
  if(! m_CallConferenceList.find(grpkey.c_str(), callEntry)) {
    if(callEntry) {
      m_CallConferenceList.unbind(callEntry);
     XGLOG_INFO("xGateSipController::DeleteConferenceList(%s)",grpkey.c_str());
    }
  }
  return true;
}

std::map<std::string,xGateDBObj*> *xGateSipController::GetSPList(std::string grpkey){
  std::map<std::string,xGateDBObj*> *spList = NULL;
  DBSP_LIST::ITERATOR it(m_SPList);
  if(m_SPList.find(grpkey.c_str(),spList) != -1)
    return spList;
  else
    return NULL;
}
bool xGateSipController::InsertSPList(std::string grpkey,xGateDBObj *dbObj){
  std::map<std::string,xGateDBObj*> *spList = GetSPList(grpkey);
  if(!spList)
    spList = new std::map<std::string,xGateDBObj*>;
  spList->insert(std::pair<std::string,xGateDBObj*>(dbObj->cp.uid,dbObj));
  DBSP_LIST::ENTRY *callEntry = 0;
  int status = m_SPList.bind(grpkey.c_str(),spList,callEntry);
  if(status == 0 && callEntry)
   XGLOG_INFO("xGateSipController::InsertCalltoConferenceList(%s) inserted success",grpkey.c_str());
  else if(status == 1 && callEntry)
   XGLOG_INFO("xGateSipController::InsertCalltoConferenceList(%s) inserted Modified",grpkey.c_str());
  else{
   XGLOG_ERROR("xGateSipController::InsertCalltoConferenceList(%s) inserted failed",grpkey.c_str());
    return false;
  }
  return true;
}

bool xGateSipController::DeleteSPFromSPList(std::string grpkey, xGateDBObj *dbObj){

  std::map<std::string,xGateDBObj*> *spList;
  spList = GetSPList(grpkey);
  if(spList == NULL)
    return false;
  return true;
}
bool xGateSipController::RemoveSPListValue(std::string grpkey, std::map<std::string, xGateDBObj*>* spList){

  if(spList == NULL)
    return false;
  std::map<std::string, xGateDBObj*>::iterator itr;
  for(itr=spList->begin();itr!=spList->end();++itr){
    spList->erase(itr);
  }
  return true;
}
bool xGateSipController::DeleteSPList(std::string grpkey){
  DBSP_LIST::ENTRY* callEntry = NULL;
  std::map<std::string, xGateDBObj*>* spList =GetSPList(grpkey);
  if(spList){
    RemoveSPListValue(grpkey, spList);
    delete spList;
  }
  if(! m_SPList.find(grpkey.c_str(), callEntry)) {
    if(callEntry) {
      m_SPList.unbind(callEntry);
     XGLOG_INFO("xGateSipController::DeleteConferenceList(%s)",grpkey.c_str());
    }
  }
  return true;
}
/*
/*
Insert the call information to  Hash table on new request
return true if object inserted successfuly
return false if object failed to insert with error 
error is writen in log file
*/
/*bool xGateSipController::InsertCallInfo(xGateSofiaobj *sofiaobj){

  CP(sofiaobj,"xGateSipController::InsertCallInfo(callobj is null)");

  HASH_CALL_MAP::ENTRY *callEntry = 0;
  int status = m_incomingCallHashTable.bind(sofiaobj->callid.c_str(),sofiaobj,callEntry);
  if(status == 0 && callEntry) {
 XGLOG_INFO("xGateSipController::InsertCallInfo(%s)","New Call Inserted into map");
  sofiaobj->callEntry = (void*)callEntry;
//    sofiaobj->SofiaLegB->callEntry = (void*)callEntry;
}
else{ 
if(status == -1 && !callEntry)
CLog::Error("xGateSipController::InsertCallInfo(%s) Failed Insertion",sofiaobj->callid.c_str());
else if(status == 1 && callEntry)
CLog::Detail("xGateSipController::InsertCallInfo(%s) Call Already in map",sofiaobj->callid.c_str());
return false;
}
return true;
}*/
/*
   Delete the call information from  Hash table on bye request
   return true if object deleted successfuly
   return false if object failed to delete with error 
   error is writen in log file
   */
#if 0
bool xGateSipController::DeleteCallInfo(xGateSofiaobj *sofiaobj){

  CP(sofiaobj,"xGateSipController::DeleteCallInfo(callobj is null)");
  // if(sofiaobj->callstream == EN_OUTBOUND){	
  // sofiaobj->callEntry =  sofiaobj->SofiaLegB->callEntry;
  //}
  HASH_CALL_MAP::ENTRY* callEntry = NULL;
  if(!m_incomingCallHashTable.find(sofiaobj->callid.c_str(), callEntry)) {
    if(callEntry) {
      m_incomingCallHashTable.unbind(callEntry);
     XGLOG_INFO("xGateSipController::DeleteCallInfo(%s)","Deleted call from Map");
    }
#if 1 //  for ippbx
    //    PostMessageToMGCDispatcher(sofiaobj->callid,"",0,0,EN_IDLE,SigType_SIP,sofiaobj->mgid);
    if(sofiaobj->cp.Legid == 1)    
      PostMessageToMGCDispatcher(sofiaobj,EN_MEDIA_DELETE);
#endif
  }
  /*if(((HASH_CALL_MAP::ENTRY*)sofiaobj->callEntry) != NULL){
    m_incomingCallHashTable.unbind((HASH_CALL_MAP::ENTRY*)sofiaobj->callEntry);
   XGLOG_INFO("xGateSipController::DeleteCallInfo(%s)","Deleted call from Map");
  // Message to MGC-MG component to release the Media Resource on case
  PostMessageToMGCDispatcher(sofiaobj->callid,"",0,0,EN_IDLE,SigType_SIP);
  }*/
  else{
   XGLOG_ERROR("xGateSipController::DeleteCallInfo(%s) Failed Deletion",sofiaobj->callid.c_str());
    return false;
  }
  //}else
  //CLog::Error("xGateSipController::DeleteCallInfo(%s) Outbound Object",sofiaobj->callid.c_str());
return true;
}
#endif
/*
*/
bool xGateSipController::PostMessage(xGateSipMsgBlock *SipMsg, XGATEPOSTTYPE destType){

  //xGateSipMsgBlock *pSipRegMsg = new xGateSipMsgBlock(EN_XGATE_MSG_SIPREG_IN);
  //pSipRegMsg->m_sipregobj = sipregobj;
  //
  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateSipMsgBlock*> (SipMsg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  std::string handleName;
  switch(destType){
    case XGATEPOSTTYPE::EN_DISPATCH:
      handleName = "SipSignalDispatcher";
      taskPtr = (xGateSipSignalDispatcher*)GetSipPostHandler(XGATEPOSTTYPE::EN_DISPATCH);
      break;
    case XGATEPOSTTYPE::EN_PROCESS:
      handleName = "SipSignalProcessor";
      taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
      break;
    default:
      break;
      //diabled for IP-PBX
      // case XGATEPOSTTYPE::EN_DBMANAGER:
      //  handleName = "DBManager";
      // taskPtr = (xGateDBManager*)GetSipPostHandler(XGATEPOSTTYPE::EN_DBMANAGER);
      // break;

  }
  if(taskPtr){
    if(taskPtr->putq(pAmb,&tval))
     XGLOG_INFO("xGateSipController::PostMessage(%s) Successfully",handleName.c_str());
    else
     XGLOG_INFO("xGateSipController::PostMessage(%s) Failed",handleName.c_str());
  }
  else{
   XGLOG_INFO("xGateSipController::PostMessage(%s) Task Pointer Empty",handleName.c_str());
  }



  return true;
}
bool  xGateSipController::PostMessage(xGateCallObj *callobj,XGATEPOSTTYPE destType){
  try{
    xGateSipMsgBlock *pSipEventMsg = new xGateSipMsgBlock(); 
    pSipEventMsg->m_callObj = callobj;
    // Specify for Hold and Resume
    if(callobj->sipeventtype == EN_HOLDRESUME_REQ)
      pSipEventMsg->setMsgType(EN_XGATE_MSG_HOLDRESUME_IN);
    else if(callobj->sipeventtype == EN_P2PMEDIA_REQ)   // Peer to Peer to call
      pSipEventMsg->setMsgType(EN_XGATE_MSG_P2PMEDIA_IN);

    ACE_Message_Block* pAmb = NULL;
    pAmb = static_cast<xGateSipMsgBlock*> (pSipEventMsg);
    ACE_Time_Value tval ((time(NULL) ));
    tval += ACE_Time_Value(0,1);
    ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
    std::string handleName;
    switch(destType){
      case XGATEPOSTTYPE::EN_DISPATCH:
        handleName = "SipSignalDispatcher";
        taskPtr = (xGateSipSignalDispatcher*)GetSipPostHandler(XGATEPOSTTYPE::EN_DISPATCH);
        break;
      case XGATEPOSTTYPE::EN_PROCESS:
        handleName = "SipSignalProcessor";
        taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
        break;

    }   
    if(taskPtr){
      if(taskPtr->putq(pAmb,&tval))
       XGLOG_INFO("xGateSipController::PostMessage(%s) Successfully",handleName.c_str());
      else
       XGLOG_INFO("xGateSipController::PostMessage(%s) Failed",handleName.c_str());
    }
    else{
     XGLOG_INFO("xGateSipController::PostMessage(%s) Task Pointer Empty",handleName.c_str());
    }
  }catch(...){
   XGLOG_INFO("xGateSipController::PostMessage(%s)","Exception Raised");
  }


}

bool xGateSipController::PostMessageToMGCDispatcher(xGateSofiaobj *sofiaobj, XGATEMGUPDATE mgMsgType=EN_MEDIA_CREATE){

  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  //taskPtr = xGateUtil::getMgDispatcher();
  taskPtr =  xGateUtil::getHmpProcessor();
  if(!taskPtr){
   XGLOG_ERROR( "xGateSipController::PostMessageToMGCDispatcher  task not found");
    return false;
  }
  xGateMgMsg* pMgMsg = NULL;
  pMgMsg = new xGateMgMsg();
  if(!pMgMsg) {
   XGLOG_ERROR( "xGateSipController::PostMessageToMGCDispatcher xgateMgMsg creation failed");
    return false;
  }	

  MgMediaDetail &media_alloc_req = pMgMsg->get_media_detail();
  pMgMsg->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
  switch(mgMsgType){
    case EN_MEDIA_18X:
      pMgMsg->set_mg_msg_type(EN_XGATE_MG_18X_RESPONSE);
     XGLOG_INFO("SofiaSiphandler::PostMessageToMGCDispatcher Incoming EN_XGATE_MG_18X_RESPONSE");
      break;
    case EN_MEDIA_UPDATE:
      pMgMsg->set_mg_msg_type(EN_XGATE_MG_PLAY_BACK);
     XGLOG_INFO("SofiaSiphandler::PostMessageToMGCDispatcher Incoming EN_XGATE_MG_PLAY_BACK");
      //	media_alloc_req.pb_state  = sofiaobj->cp.PbState;
      //        media_alloc_req.leg_id = sofiaobj->cp.Legid;
      //        media_alloc_req.play_file =  sofiaobj->cp.csfilename;

      break;

    case EN_MEDIA_SL_PLAY_FILE:
      //case EN_MEDIA_SL_PLAY_FILE_BKGND:

      //case EN_MEDIA_SL_STOP_PLAY_FILE:
      //case EN_MEDIA_SL_STOP_PLAY_FILE_BKGND:

    case EN_MEDIA_HOLD_RESUME:
      pMgMsg->set_mg_msg_type(EN_XGATE_MG_HOLD_RESUME);
     XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher Incoming EN_XGATE_MG_HOLD_RESUME");
      break;

    case EN_MEDIA_GET_DTMF_DIGITS:
      pMgMsg->set_mg_msg_type(EN_XGATE_MG_DTMF_DIGITS);
     XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher Incoming EN_XGATE_MG_DTMF_DIGITS ");
      break;

    case EN_MEDIA_CONF:
    //case EN_MEDIA_CONF_GETCONFROOMHANDLE:
      pMgMsg->set_mg_msg_type(EN_XGATE_MG_MEDIA_CREATE);
     XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher Incoming EN_XGATE_MG_MEDIA_CREATE ");
      break;

    case EN_MEDIA_CONF_ADDPARTICIPANT:
      pMgMsg->set_mg_msg_type(EN_XGATE_MG_ADD_PARTICIPANT );
     XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher Incoming EN_XGATE_MG_ADD_PARTICIPANT  ");
      break;

    case EN_MEDIA_CONF_DELPARTICIPANT:
      //     pMgMsg->set_mg_msg_type(EN_XGATE_MG_REMOVE_PARTICIPANT);
      pMgMsg->set_mg_msg_type(EN_XGATE_MG_MEDIA_DELETE);
      //CLog::Detail("xGateSipController::PostMessageToMGCDispatcher Incoming EN_XGATE_MG_REMOVE_PARTICIPANT ");
     XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher Incoming EN_XGATE_MG_MEDIA_DELETE");
      break;

    case EN_MEDIA_SL_DTMF_RESUME:
      pMgMsg->setMsgType(EN_XGATE_MSG_SIP_IN);
      //CallManager::Instance()->ResumeOwnerTask(this); // will resume SL main thread - that was suspended for DTMF-digit collection.
     XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher Incoming EN_MEDIA_SL_DTMF_RESUME ");
      break;

    case EN_MEDIA_DELETE:
      pMgMsg->set_mg_msg_type(EN_XGATE_MG_RELEASE);
      break;
    case EN_MEDIA_MULTICONF_REQ_PORT:
     pMgMsg->set_mg_msg_type(EN_XGATE_MG_CONFERENCE_FOR_CLIENT);
    XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher Multi conf port request %s",sofiaobj->uid.c_str());
    break;
    case EN_MEDIA_MULTICONF_CREATE:
     pMgMsg->set_mg_msg_type(EN_XGATE_MG_ADDCLIENT_TO_CONFERENCE);
    XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher Multi conf channel creation %s",sofiaobj->uid.c_str());
    break;

    case EN_MEDIA_RECORD:
        pMgMsg->set_mg_msg_type( EN_XGATE_MG_ALLOCATE_RECORD );
       XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher Requesting call Recording port %s",sofiaobj->uid.c_str());
    break;

    case EN_MEDIA_RECORD_START:
        pMgMsg->set_mg_msg_type( EN_XGATE_MG_CALL_RECORD );
        //media_alloc_req.record_file = "/root/narrain/Work/" + sofiaobj->cp.siprec_callid.substr(0,3) + "-" + sofiaobj->uid;
        media_alloc_req.record_file = sofiaobj->cp.csfilename;
        media_alloc_req.record_file.append(".wav");
        

        sofiaobj->cp.PbState = EN_XGATE_PB_RECORD_START;
       XGLOG_INFO( "EN_MEDIA_RECORD_START Recording file (%s)", media_alloc_req.record_file.c_str());
        if (sofiaobj->cp.isconferenceCall) {
           XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher start conference call Recording %s",sofiaobj->cp.siprec_callid.c_str());
        } else {
           XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher start call Recording %s",sofiaobj->cp.siprec_callid.c_str());
        }
    break;

    case EN_MEDIA_RECORD_HOLD:
        pMgMsg->set_mg_msg_type( EN_XGATE_MG_CALL_RECORD);
        sofiaobj->cp.PbState = EN_XGATE_PB_RECORD_HOLD;
        if (sofiaobj->cp.isconferenceCall) {
           XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher hold conference call Recording %s",sofiaobj->cp.siprec_callid.c_str());
        } else {
           XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher hold call Recording %s",sofiaobj->cp.siprec_callid.c_str());
        }
    break;

    case EN_MEDIA_RECORD_RESUME:
        pMgMsg->set_mg_msg_type( EN_XGATE_MG_CALL_RECORD);
        sofiaobj->cp.PbState = EN_XGATE_PB_RECORD_RESUME;
        if (sofiaobj->cp.isconferenceCall) {
           XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher resume conference call Recording %s",sofiaobj->cp.siprec_callid.c_str());
        } else {
           XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher resume call Recording %s",sofiaobj->cp.siprec_callid.c_str());
        }
    break;

    case EN_MEDIA_RECORD_STOP:
        pMgMsg->set_mg_msg_type( EN_XGATE_MG_RELEASE_RECORD );
        sofiaobj->cp.PbState = EN_XGATE_PB_RECORD_STOP;
        if (sofiaobj->cp.isconferenceCall) {
           XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher stop conference call Recording %s",sofiaobj->uid.c_str());
        } else {
           XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher stop call Recording %s",sofiaobj->uid.c_str());
        }
    break;

    case EN_MEDIA_RECORD_DELETE:
        pMgMsg->set_mg_msg_type( EN_XGATE_MG_MEDIA_DELETE );
        //sofiaobj->cp.PbState = EN_XGATE_PB_RECORD_STOP;
       XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher leave party from conference call Recording %s",sofiaobj->cp.siprec_callid.c_str());
    break;

    case EN_MEDIA_CREATE:
    default:
      if(sofiaobj->callstream == EN_INBOUND){
        pMgMsg->set_mg_msg_type( EN_XGATE_MG_ALLOCATE_RECORD );
        //pMgMsg->set_mg_msg_type(EN_XGATE_MG_ALLOCATE_ORGINATE);
       XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher Incoming EN_XGATE_MG_ALLOCATE_RECORD %s",sofiaobj->uid.c_str());
        //CLog::Detail("SofiaSiphandler::PostMessageToMGCDispatcher Outgoing EN_XGATE_MG_ALLOCATE_ORGINATE ");
      }
      else if (sofiaobj->callstream == EN_OUTBOUND){
        pMgMsg->set_mg_msg_type(EN_XGATE_MG_ALLOCATE_ORGINATE);
        // pMgMsg->set_mg_msg_type(EN_XGATE_MG_ALLOCATE_TERMINATE);
       XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher Outgoing EN_XGATE_MG_ALLOCATE_ORGINATE ");
        //XGLOG_INFO("SofiaSiphandler::PostMessageToMGCDispatcher Incoming EN_XGATE_MG_ALLOCATE_TERMINATE");
      }else
        pMgMsg->set_mg_msg_type(EN_XGATE_MG_RELEASE);
      break;
  }
  media_alloc_req.call_id.assign(sofiaobj->cp.siprec_callid.c_str());
 XGLOG_INFO("xGateSipController::PostMessageToMGCDispatcher"
         " changed to siprec_callid (%s)", media_alloc_req.call_id.c_str());
//  media_alloc_req.call_id.assign(sofiaobj->callid.c_str());
  //media_alloc_req.call_id = sofiaobj->callid;
  if(sofiaobj->cp.isconferenceCall ){
    if( !sofiaobj->cp.confinfo.gm_mgResourceId.empty())
      media_alloc_req.mediaResource_id =  sofiaobj->cp.confinfo.gm_mgResourceId;
    else
      media_alloc_req.mediaResource_id =  sofiaobj->uid;

    media_alloc_req.mg_ip = sofiaobj->cp.gm_mgc_ip;
  }else{
    media_alloc_req.mediaResource_id =  sofiaobj->uid;
    media_alloc_req.mg_ip =  "";
  }
//sobj->cp.xgateCallFeature =  XGATECALLFEATURE::EN_MULTICONF_CALL;
#if 0 //SRC
if(sofiaobj->cp.xgateCallFeature == XGATECALLFEATURE::EN_MULTICONF_CALL ||
        sofiaobj->cp.xgateCallFeature == XGATECALLFEATURE::EN_RECORD_CALL )
   media_alloc_req.mediaResource_id = sofiaobj->ad_hocConf.getMultiConferenceId();
#endif
  media_alloc_req.leg_id = sofiaobj->cp.Legid;
  media_alloc_req.call_dir	= sofiaobj->callstream;
  //media_alloc_req.sig_type	= SigType_SIP;
  media_alloc_req.gateway_id    = sofiaobj->mgid; // Mulit MG support for PBX
  media_alloc_req.pb_state  = sofiaobj->cp.PbState;
  media_alloc_req.media_proto = sofiaobj->remoteSDPinfo[0].mediamode;
  media_alloc_req.out_proto =  sofiaobj->remoteSDPinfo[0].outproto;

  //printf("\n Unique id on Controller:%s\n", sofiaobj->uid.c_str());
  bool isAVCall = false;
  for (int i = 0 ; i < 2 ; i++){
    if(sofiaobj->remoteSDPinfo[i].mediatype > 0){
      media_alloc_req.sdpinfo[i].mediaType =  sofiaobj->remoteSDPinfo[i].mediatype;
      media_alloc_req.sdpinfo[i].ip_addr = sofiaobj->remoteSDPinfo[i].ipaddress;
      media_alloc_req.sdpinfo[i].port = sofiaobj->remoteSDPinfo[i].port;
      media_alloc_req.sdpinfo[i].relayip_addr = sofiaobj->remoteSDPinfo[i].relayip;
      media_alloc_req.sdpinfo[i].reflexip_addr = sofiaobj->remoteSDPinfo[i].reflexip;
      media_alloc_req.sdpinfo[i].relay_port = sofiaobj->remoteSDPinfo[i].relayport;
      media_alloc_req.sdpinfo[i].reflex_port = sofiaobj->remoteSDPinfo[i].reflexport;

      media_alloc_req.sdpinfo[i].ice_ufrag = sofiaobj->remoteSDPinfo[i].iceufrag;
      media_alloc_req.sdpinfo[i].ice_pwd = sofiaobj->remoteSDPinfo[i].icepwd;
      media_alloc_req.sdpinfo[i].fingerprint = sofiaobj->remoteSDPinfo[i].fingerprint;

      media_alloc_req.sdpinfo[i].ssrc = sofiaobj->remoteSDPinfo[i].ssrc;
      media_alloc_req.sdpinfo[i].cname = sofiaobj->remoteSDPinfo[i].cname;
      media_alloc_req.sdpinfo[i].mslabel = sofiaobj->remoteSDPinfo[i].mslabel;
      media_alloc_req.sdpinfo[i].label = sofiaobj->remoteSDPinfo[i].label;

      media_alloc_req.sdpinfo[i].ptime = sofiaobj->remoteSDPinfo[i].ptime;

      media_alloc_req.sdpinfo[i].codec = sofiaobj->remoteSDPinfo[i].avcodec.at(0)->payloadtype; 
      media_alloc_req.sdpinfo[i].codecname = sofiaobj->remoteSDPinfo[i].avcodec.at(0)->name;
      media_alloc_req.sdpinfo[i].fmtp = sofiaobj->remoteSDPinfo[i].avcodec.at(0)->fmtp;
      media_alloc_req.sdpinfo[i].play_file =  sofiaobj->cp.csfilename;
      if(sofiaobj->remoteSDPinfo[i].mediatype ==  3)
        isAVCall = true;
    }
  }
  if (sofiaobj->cp.isconferenceCall)
    media_alloc_req.call_type = EN_XGATE_CALL_TYPE_AUDIO_CONF;
  else if (isAVCall)
    media_alloc_req.call_type = EN_XGATE_CALL_TYPE_VIDEO; 

  if (sofiaobj->cp.confinfo.gm_conf_host_user)
    media_alloc_req.joiner_type = EN_XGATE_JOINER_TYPE_HOST;
  else
    media_alloc_req.joiner_type = EN_XGATE_JOINER_TYPE_PARTICIPANT;

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateMgMsg*> (pMgMsg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if (taskPtr->putq(pAmb, &tval)) {
   XGLOG_INFO( " xGateSipController::PostMessageToMGCDispatcher %d Media Server ID  enqueue message success call- %s",sofiaobj->mgid,sofiaobj->uid.c_str());
  } else {
   XGLOG_ERROR( " xGateSipController::PostMessageToMGCDispatcher  enqueue message failed");
    delete pMgMsg;
    return false;
  }	
  return true;

}

/*
*/

bool xGateSipController::PostMessageToMGCDispatcher(std::string call_id,std::string ip_addr,unsigned int ip_port,\
    unsigned int codec, unsigned int calldirection,unsigned int sigtype,int mgid){

  if(mgid == -1){

   XGLOG_ERROR( "xGateSipController::PostMessageToMGCDispatcher-  No Media server Connected ");
    //  return false;    // This is only for MG case Yadav
  }

  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr =  xGateUtil::getHmpProcessor();
  //taskPtr = xGateUtil::getMgDispatcher();
  xGateMgMsg* pMgMsg = NULL;
  pMgMsg = new xGateMgMsg();
  if(!pMgMsg) {
   XGLOG_ERROR( "xGateSipController::PostMessageToMGCDispatcher xgateMgMsg creation failed");
    return false;
  }	
  pMgMsg->setMsgType(EN_XGATE_MSG_MGC_MG_IN);

  if(calldirection == EN_INBOUND){
    pMgMsg->set_mg_msg_type( EN_XGATE_MG_ALLOCATE_RECORD );
   XGLOG_INFO("SofiaSiphandler::PostMessageToMGCDispatcher Incoming -ip address %s Port %d payload %d ",ip_addr.c_str(),ip_port,codec);
  }
  else if (calldirection == EN_OUTBOUND){
    pMgMsg->set_mg_msg_type(EN_XGATE_MG_ALLOCATE_ORGINATE);
   XGLOG_INFO("SofiaSiphandler::PostMessageToMGCDispatcher Outgoing -ip address %s Port %d payload %d ",ip_addr.c_str(),ip_port,codec);
  }else
    pMgMsg->set_mg_msg_type(EN_XGATE_MG_RELEASE);

  MgMediaDetail &media_alloc_req = pMgMsg->get_media_detail();
  media_alloc_req.call_id = call_id;
  /*  media_alloc_req.ip_addr = ip_addr;
      media_alloc_req.port = ip_port;
      media_alloc_req.codec = codec; 
      media_alloc_req.call_dir	= calldirection;
      media_alloc_req.sig_type	= sigtype;*/
  //  media_alloc_req.gateway_id    = mgid;

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateMgMsg*> (pMgMsg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if (taskPtr->putq(pAmb, &tval)) {
    XGLOG_WARN( " xGateSipController::PostMessageToMGCDispatcher %d Media Server ID  enqueue message success call- %s",mgid,call_id.c_str());
  } else {
   XGLOG_ERROR( " xGateSipController::PostMessageToMGCDispatcher  enqueue message failed");
    delete pMgMsg;
    return false;
  }	
  return true;
}

static char RFC2833_CHARS[] = "0123456789*#ABCDF";
char switch_rfc2833_to_char(int event)
{
  //if (event > -1 && event < (int32_t) sizeof(RFC2833_CHARS)) {
  if (event > -1 && event < sizeof(RFC2833_CHARS)) {
    return RFC2833_CHARS[event];
  }
  return '\0';
}

/*
*/
bool  xGateSipController::PostMessageToProcessor(xGateCallObj *callObj)  {
  try{
   XGLOG_INFO( "multithreaded-ippbx : xGateSipController::PostMessageToMGCDispatcher  enqueue message failed");
    xGateSipMsgBlock *pSipEventMsg = new xGateSipMsgBlock(); 
    pSipEventMsg->m_callObj = callObj;

    ACE_Message_Block* pAmb = NULL;
    pAmb = static_cast<xGateSipMsgBlock*> (pSipEventMsg);
    ACE_Time_Value tval ((time(NULL) ));
    tval += ACE_Time_Value(0,1);
    ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
    std::string handleName = "SipSignalProcessor";
    taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
    if(taskPtr){

      if(taskPtr->putq(pAmb,&tval))
       XGLOG_INFO("xGateSipController::PostMessageToProcessor(%s) Successfully",handleName.c_str());
      else
       XGLOG_INFO("xGateSipController::PostMessageToProcessor(%s) Failed",handleName.c_str());
    }
    else{
     XGLOG_INFO("xGateSipController::PostMessageToProcessor(%s) Task Pointer Empty",handleName.c_str());
    }
  }catch(...){
   XGLOG_INFO("xGateSipController::PostMessageToProcessor(%s)","Exception Raised");
  }
}
/*
*/
bool xGateSipController::ValidateSipMsgPointer(const char *p){
  if(  p !=NULL ){
    if(strlen(p))
      return true;
  }
  return false;
}
/*
*/
xGateRetVal xGateSipController::RespondWith1xx(xGateCallObj* pSipMgc ){
 XGLOG_INFO("xGateSipController::RespondWith1xx(%s)","Base Class");
  switch(pSipMgc->sipeventtype){
    case XGATESIPEVENT::EN_100_REL:

      break;
    case XGATESIPEVENT::EN_180_REL:

      break;
    case XGATESIPEVENT::EN_183_REL:

      break;
    default:
      break;

  }
  return EN_XGATE_STATUS_SUCCESS;
}
/*
*/
xGateRetVal xGateSipController::RespondWith2xx(xGateCallObj* pSipMgc){
 XGLOG_INFO("xGateSipController::RespondWith2xx(%s)","Base Class");
  switch(pSipMgc->sipeventtype){
    case XGATESIPEVENT::EN_200_REL:
      break;
    default:
      break;

  }
  return EN_XGATE_STATUS_SUCCESS;
}
/*
*/
xGateRetVal xGateSipController::RespondWith3xx(xGateCallObj* pSipMgc){
 XGLOG_INFO("xGateSipController::RespondWith3xx(%s)","Base Class");
  switch(pSipMgc->sipeventtype){
    case XGATESIPEVENT::EN_300_REL:
      break;
    default:
      break;

  }
  return EN_XGATE_STATUS_SUCCESS;
}
/*
*/
xGateRetVal xGateSipController::RespondWith4xx(xGateCallObj* pSipMgc){
 XGLOG_INFO("xGateSipController::RespondWith4xx(%s)","Base Class");
  switch(pSipMgc->sipeventtype){
    case XGATESIPEVENT::EN_400_REL:
      break;
    default:
      break;

  }
  return EN_XGATE_STATUS_SUCCESS;
}
/*
*/
xGateRetVal xGateSipController::RespondWith5xx(xGateCallObj* pSipMgc){
 XGLOG_INFO("xGateSipController::RespondWith5xx(%s)","Base Class");
  switch(pSipMgc->sipeventtype){
    case XGATESIPEVENT::EN_500_REL:
      break;
    default:
      break;

  }
  return EN_XGATE_STATUS_SUCCESS;
}
/*
*/
xGateRetVal xGateSipController::RespondWith6xx(xGateCallObj* pSipMgc){
 XGLOG_INFO("xGateSipController::RespondWith6xx(%s)","Base Class");
  switch(pSipMgc->sipeventtype){
    case XGATESIPEVENT::EN_600_REL:
      break;
    default:
      break;

  }
  return EN_XGATE_STATUS_SUCCESS;
}
std::string xGateSipController::GetCurrentSbcInfo(){
  std::string ret= "";
  static int CurrentSbc = 0;
  if(CurrentSbc >= sbcip.size())
    CurrentSbc = 0;
  ret = sbcip[CurrentSbc]->ipadd;
  CurrentSbc++;
 XGLOG_INFO("xGateSipController::GetCurrentSbcInfo(%s)",ret.c_str());
  return ret;
}
std::string xGateSipController::GetCurrentPbxInfo(){
  std::string ret = "";
  static int CurrentPbx = 0;
  if(CurrentPbx >= pbxip.size())
    CurrentPbx = 0;
  if (pbxip[CurrentPbx] != NULL)
  {
    ret = pbxip[CurrentPbx]->ipadd;
    CurrentPbx++;
  }
 XGLOG_INFO("xGateSipController::GetCurrentPbxInfo(%s)",ret.c_str());
  return ret;

}
std::string xGateSipController::GetSignalGateWayInfo(){
  std::string ret = "";
  static int CurrentSiggwip = 0;
  if(CurrentSiggwip >= pbxip.size())
    CurrentSiggwip = 0;
  ret = siggwip[CurrentSiggwip]->ipadd;
  CurrentSiggwip++;
 XGLOG_INFO("xGateSipController::GetCurrentSignalGateWayinfo(%s)",ret.c_str());
  return ret;
}
std::string xGateSipController::GetConfernecMgIp(){
  return confmgip[0]->ipadd;
}
std::string xGateSipController::CreateMedialine(int mediaType, int port, std::string mediaPath){
  std::stringstream  mline;
  mline << ((mediaType == 0) ? "audio " : "video ") << port << " " << mediaPath;

  return mline.str();
}
std::string xGateSipController::AppendMedialine(std::string preValue, std::string currentValue){
  std::stringstream mline;
  mline << preValue <<  currentValue;
  return mline.str();
}
std::string xGateSipController::CreateConnectionline(int networkType, std::string ipaddr){
  std::string cline;
  return cline;
}
std::string xGateSipController::CreateAttributeline(std::string header,std::string value){
  std::string aline;
  return aline;
}
std::string xGateSipController::AppendAttributeline(std::string preValue, std::string currentValue){
  std::string aline;
  return aline;
}
std::string xGateSipController::SdpCodec(int pt,std::string codecname){
  std::string aline;
  return aline;
}
/*------------------------------------------------------------------------------------------------------
 *                           xGateCallInfo Impementation
 ------------------------------------------------------------------------------------------------------*/
#if 0
xGateCallInfo::xGateCallInfo(CallDirection dir,const char* defInterfaceName,CallParameter cp) : Call(dir, defInterfaceName) {
 XGLOG_INFO( "xGateCallInfo::xGateCallInfo() **** Inside CONSTRUCTOR ****\n");
  //_callParam = cp;
  pPeerCallinfo = NULL;
  if(dir == CD_Incoming){
    if(AllocateInterface(dir,cp)) {
      //AllocateMedia(NULL,dir,_callParam);
      AllocateMedia(NULL,dir,cp); // commenteded temporary : for multithreaded_ippbx : to be uncommented later
      SetInterface(itf->Name().c_str());

    } else {
     XGLOG_ERROR("xGateCallInfo::xGateCallInfo FAILED");
    }
  }


}
xGateCallInfo::~xGateCallInfo(){

}
bool xGateCallInfo::DeviceOverlapReq(const char* additionalDigits,bool complete){

  return true;
}
bool xGateCallInfo::DeviceProceedingReq(CallProgress progress, CallExtraInfo* exinfo){

  return true;
}
bool xGateCallInfo::DeviceProgressReq(CallProgress progress, CallExtraInfo* exinfo){

  return true;
}
bool xGateCallInfo::DeviceAlertingReq(CallProgress progress, CallExtraInfo* exinfo){

  return true;
}
bool xGateCallInfo::DeviceConnectReq(CallExtraInfo* exinfo){

  return true;
}
bool xGateCallInfo::DeviceSuspendReq(){

  return true;
}    
bool xGateCallInfo::DeviceResumeReq(){

  return true;
}
bool xGateCallInfo::DeviceDisconnectReq(CallCause cause,CallProgress progress){

  return true;
}
bool xGateCallInfo::DeviceNotifyECTReq(tect_indicator indicator){

  return true;
}

/////// added for ippbx
bool xGateCallInfo::DeviceSendInfo(CallExtraInfo* exinfo)
{
  /***************
   XGLOG_INFO( "SofiaSIPCall::DeviceSendInfo \n");
    SOFIA_SIP_CHECK_CALL(DeviceSendInfo);
    CallParameter *csCalledParam = GetCallParameter();
    nua_info(nua_handle,
    SIPTAG_CONTENT_TYPE_STR("application/transfer-info"),
    SIPTAG_PAYLOAD_STR("UnPark"),
    TAG_END());
   ******************/
  return true;
}

/*--------------------------------------------------------------------------------
  Default CALL class implementation
  --------------------------------------------------------------------------------*/
void xGateCallInfo::OverlapReq(const char* additionalDigits,bool complete){

 XGLOG_INFO("xGateCallInfo::OverlapReq" );

  //Base class function call
  //Call::OverlapReq(additionalDigits,complete);
  //Post Message form here to receiver;
}
void xGateCallInfo::ProceedingReq(CallProgress progress, CallExtraInfo* exinfo){
 XGLOG_INFO("xGateCallInfo::ProceedingReq" );
  //Call::ProceedingReq(progress,exinfo);
}
void xGateCallInfo::ProgressReq(CallProgress progress, CallExtraInfo* exinfo){
  //Call::ProgressReq(progress,exinfo);
 XGLOG_INFO("xGateCallInfo::ProgressReq" );
}
void xGateCallInfo::AlertingReq(CallProgress progress, CallExtraInfo* exinfo){
  //Call::AlertingReq(progress,exinfo);  
 XGLOG_INFO("xGateCallInfo::AlertingReq" );
}
void xGateCallInfo::ConnectReq(CallExtraInfo* exinfo){
  //Call::ConnectReq(exinfo);
 XGLOG_INFO("xGateCallInfo::ConnectReq" );
}
void xGateCallInfo::SuspendReq(){
 XGLOG_INFO("xGateCallInfo::SuspendReq" );
  //Call::SuspendReq();

}
void xGateCallInfo::ResumeReq(){
  //Call::ResumeReq(); 
 XGLOG_INFO("xGateCallInfo::ResumeReq" );
}
void xGateCallInfo::DisconnectReq(CallCause cause,CallProgress progress){
 XGLOG_INFO("xGateCallInfo::DisconnectReq " );

  //  DeallocateMedia(GetDirection(), cause, _callParam);
  CallParameter cparam ;
  cparam.call_id =   GetCallParameter()->call_id;
  //cparam.callstate = GetCallParameter()->callstate; 
  PostMessageToReceiver(CS_Disconnected,cparam.call_id,cparam,ErrorCauseReturn(cause));

  //PostMessageToReceiver(Call::_state,cparam->call_id,cparam,XGATESIPEVENT::EN_500_REL);
  RelComInd(cause);

  //this function is called inside RelComInd but still required to call explictely
  CallManager::Instance()->DestroyCall(this); 
}
void xGateCallInfo::NotifyECTReq(tect_indicator indicator){
 XGLOG_INFO("xGateCallInfo::NotifyECTReq" );
  //Call::NotifyECTReq(indicator);
}
Call *xGateCallInfo::SetupReq(CTask *task,Interface *itf,SelectionStrategy selection,CallParameter *cparam){
 XGLOG_INFO("xGateCallInfo::SetupReq" );


  xGateCallInfo *LegBInfo = NULL;
  try{
   XGLOG_INFO("xGateCallInfo::SetupReq(%s) itf NAme = %s",itf->Name().c_str(), itf->Name().c_str());
    LegBInfo = new xGateCallInfo(CD_Outgoing,itf->Name().c_str(),*cparam);
    LegBInfo->itf = (Rtp_Interface*)itf;
    Rtp_Payload_Code payloadlist[1];
    size_t maxcod =  LegBInfo->itf->GetSupportedPayload(payloadlist,1);
    if(strstr(cparam->calledDeviceType.c_str(),"URWEB") || strstr(cparam->calledDeviceType.c_str(),"URAPP") )
      cparam->csCalledCodec =  111;     
    else if (strstr(cparam->calledDeviceType.c_str(),"URDESK"))
      cparam->csCalledCodec =  8;
    else
      cparam->csCalledCodec = 8;

    cparam->csProxyRoute = LegBInfo->itf->GetProxyRoute();
    //cout << "csProxyRoute \t" << cparam->csProxyRoute.c_str();
    //LegBInfo->LegACallinfo =(xGateCallInfo*)this;
    LegBInfo->pPeerCallinfo =(xGateCallInfo*)this; // Saving leg A peer to Leg B
    pPeerCallinfo = LegBInfo; // Saving leg B peer to Leg A
    if(cparam->mediaAddr == 0){
      /////#if 0 // temporarily added for testing : to be removed later
      if(LegBInfo->AllocateMedia((Call*)this/*LegAInfo*/,CD_Outgoing,*cparam) == false)
        LegBInfo->SetCause(CC_ResourceUnavailable);
    }else{ 
      //////#endif
      LegBInfo->med = cparam->mediaAddr;
      SetMediaState(MS_MediaActive);
    }

    if(!LegBInfo || !LegBInfo->med){
     XGLOG_INFO("xGateCallInfo::SetupReq(NULL return)");
      //PostMessageToReceiver(Call::_state,cparam->call_id,*cparam,XGATESIPEVENT::EN_500_REL); // Media creation error
      PostMessageToReceiver(Call::_state,cparam->call_id,*cparam,XGATESIPEVENT::EN_500_REL); // Media creation error
    }
    else {
      //PostMessageToReceiver(Call::_state,cparam->call_id,*cparam,XGATESIPEVENT::EN_DIALOUT_REL);
     XGLOG_INFO("xGateCallInfo::SetupReq(EN_DIALOUT_REL Posted)");
      PostMessageToReceiver(Call::_state,cparam->call_id,*cparam,XGATESIPEVENT::EN_DIALOUT_REL);

    }
  }catch(...){
    //PostMessageToReceiver(Call::_state,cparam->call_id,*cparam,XGATESIPEVENT::EN_500_REL); // Exception handling
    PostMessageToReceiver(Call::_state,cparam->call_id,*cparam,XGATESIPEVENT::EN_500_REL); // Exception handling
  }
  // Leg B is created so copy the call param to Leg B from Leg A
  if(LegBInfo)
    LegBInfo->_callParam = *cparam;

  return LegBInfo;
}
XGATESIPEVENT xGateCallInfo::ErrorCauseReturn(CallCause cause){
  XGATESIPEVENT ret;
 XGLOG_INFO("xGateCallInfo::ErrorCauseReturn %d", cause);

  switch(cause){
    case CC_UnallocatedNumber:
    case CC_NoRouteToTransitNetwork:
    case CC_NoRouteToDestination:
      ret=EN_404_REL;
      break;
    case CC_NormalCallClearing:
      ret =EN_TERMINATED_REL;	
      break;
    case CC_UserBusy:
      ret=EN_486_REL;
      break;
    case  CC_NoUserResponding:
      ret=EN_408_REL;
      break;
    case CC_NoAnswerFromUser:
    case CC_NormalUnspecified:
      ret=EN_480_REL;
      break;
    case CC_CallRejected:
    case CC_BearerCapabilityUnauthorized:
      ret=EN_403_REL;
      break;
    case CC_NumberChanged:
      ret=EN_410_REL;
      break;
    case CC_BearerCapabilityUnimplemented:
      ret=EN_488_REL;
      break;
    case CC_ProtocolError:
    case  CC_Interworking:
      ret=EN_500_REL;
      break;
    case CC_ServiceOrOptionNotImplementedUnspecified:
      ret=EN_501_REL;
      break;
    case CC_DestinationOutOfOrder:
      ret=EN_502_REL;
      break;
    case CC_NoCircuitAvailable:
    case CC_NetworkOutOfOrder:
    case CC_TemporaryFailure:
    case CC_SwitchingEquipmentCongestion:
    case CC_ResourceUnavailable:
    case CC_BearerCapabilityUnavailable:
    case CC_IncompatibleDestination:
      ret=EN_503_REL;
      break;
    case CC_RecoveryOnTimeout:
      ret=EN_504_REL;
      break;
    case CC_InvalidNumberFormat:
      ret=EN_484_REL;
      break;
    case CC_FacilityRejected:
      ret=EN_510_REL;
      break;
    default:
      ret = EN_500_REL;
      break;

  }

  return ret;
}
//void xGateCallInfo::PostMessageToReceiver(int cs, std::string callid,\
  CallParameter cp,XGATESIPEVENT sipevent){
    void xGateCallInfo::PostMessageToReceiver(int cs, std::string callid,\
        CallParameter cp,XGATESIPEVENT sipevent){
      // Post the message to Recevire q

      xGateCallState *obj =  new xGateCallState;
      obj->callstate = cs;
      obj->callid = callid;
      cp.call_id = callid; // Yadav for testing TODO remove ater finding fault in sl
      //obj->cp = cp;
      obj->cp = cp;
      obj->cp.call_id = obj->callid;
      if(GetDirection() == CD_Incoming)
        obj->calldir = EN_INBOUND;
      else 
        obj->calldir = EN_OUTBOUND;
      obj->sipeventtype = sipevent;
      WriteToReceiver(obj);
     XGLOG_INFO("multithreaded-ippbx : xGateCallInfo::PostMessageToReceiver (%s) ; obj->port = %u", obj->callid.c_str(), obj->port);
    }

    void xGateCallInfo::SetupCallInd(CallParameter &cparam,std::string callid){
     XGLOG_INFO("xGateCallInfo::SetupCallInd (%s)", callid.c_str());
      // it seems that AllocateInterface and AllocateMedia needs to be invoked before Call::SetupInd() -to make sure that task is set correctly.

      /********************
        if (itf)
        {
        AllocateInterface(2,cparam); // Allocate interface -if not allocated. // added for multithreaded_ippbx
        }
        global_med = NULL;
        if(itf->AllocateMedia((Call*)this, global_med, (SelectionStrategy)0, (CallDirection)2, &cparam) == false || med == NULL){
       XGLOG_INFO("xGateController::itf->AllocateMedia Failed");
        }
        else
       XGLOG_INFO("xGateController::itf->AllocateMedia SUCCESS");
       ***********************/

      Call::SetupInd(cparam); 
      PostMessageToReceiver(Call::_state,callid,cparam,XGATESIPEVENT::EN_CALLSTATE_UPDATE_REL);
    }
    bool xGateCallInfo::AllocateInterface(CallDirection dir,CallParameter cp){

      src_addr.set(cp.csaCalling.c_str());
      //src_addr.set(xGateUtil::GetLocalIPAddress());
      //src_addr.set(xGateUtil::GetLocalIpAddress());
      //src_addr.set("10.22.7.55"); //TODO: Hardcoded for live call testing, need to be fixed here
      //dest_addr.set(cp.csaCalling.c_str());

      //itf = RtpInterfaceManager::Instance()->DispatchIncomingCall(SigType_SOFIA,src_addr,dest_addr,cp);
      itf = RtpInterfaceManager::Instance()->DispatchIncomingCall(SigType_SOFIA,src_addr,src_addr,cp);
      if(!itf) {
       XGLOG_INFO("xGateCallInfo::AllocateInterface FAILED");
        return false;
      }
      // global_itf = itf;
     XGLOG_INFO("xGateCallInfo::AllocateInterface SUCCESS ; itf->Name().c_str() = %s",itf->Name().c_str());
      return true;
    }
    bool xGateCallInfo::AllocateMedia(Call *call,CallDirection dir,CallParameter cp){

      med =NULL;
      if(itf->allocate_interface(call, med, dir, &cp, itf->Name().c_str()) == false || med == NULL){
       XGLOG_INFO("xGateCallInfo::AllocateMedia Failed");
        return false;
      }

      if(dir == CD_Incoming)
        med->SetDirection(dir);
      rtp_med = (Rtp_Media*)med;
      MediaAvailableInd(rtp_med);
      SetMediaState(MS_MediaActive);
     XGLOG_INFO("xGateCallInfo::AllocateMedia SUCCESS");
      return true;
    }
    bool xGateCallInfo::DeallocateMedia(CallDirection dir, CallCause cc,CallParameter &cp){
      if(GetMediaState() < MS_Release_Complete && GetMediaState() > MS_Null ) {
       XGLOG_INFO("xGateCallInfo::DeallocateMedia( ReleaseMedia) callid %s ",_callParam.call_id.c_str() );
        itf->ReleaseMedia(med,dir,&cp,cc);
        SetMediaState(MS_Release_Complete);
      }
      return true;
    }
#endif
#if 0
    void xGateCallObj::CallinfoEventExecute(XGATESIPEVENT event){

      switch(event){

        case EN_INVITE_REQ:
         XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute (EN_INVITE_REQ) - %s ",callid.c_str());
          cp.call_id = callid;
          pCallInfo =  new xGateCallInfo(CD_Incoming," ",cp);
          if(pCallInfo){

            InsertCallProcessobj(pCallInfo,callid);
            pCallInfo->SetupCallInd(cp,callid);
          }

          break;
        case EN_INVITE_RES: // Dail out Response
         XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute (EN_INVITE_RES) - %s ",callid.c_str());
          pCallInfo = GetCallProcessobj(callid);
          if(pCallInfo)
            switch(callstate){
              case CS_Alerting:
                pCallInfo->AlertingInd(CP_InBandInfoAvailable,NULL);
                pCallInfo->pPeerCallinfo->AlertingInd(CP_InBandInfoAvailable,NULL);
                break;
              case CS_Proceeding:
                pCallInfo->ProceedingInd(CP_InBandInfoAvailable,NULL);
                pCallInfo->pPeerCallinfo->ProceedingInd(CP_InBandInfoAvailable,NULL);
                break;
              case CS_Connecting:
              case CS_Connected:
                if(pCallInfo->pPeerCallinfo){
                 XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute (CS_Connecting) - %s ",callid.c_str());
                  pCallInfo->pPeerCallinfo->ConnectInd(NULL);
                  pCallInfo->ConnectCnf();

                }

                break;
            }
          break;
        case EN_REINVITE_REQ:
         XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute (EN_REINVITE_REQ) - %s ",callid.c_str());
          pCallInfo = GetCallProcessobj(callid);
          if(pCallInfo){

            //pCallInfo->OverlapReq()
          }
          break;
        case EN_REGISTER_REQ:
         XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute (EN_REGISTER_REQ) - %s ",callid.c_str());
          break;
        case EN_REGISTER_RES:
         XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute (EN_REGISTER_RES) - %s ",callid.c_str());
          break;
        case EN_MESSAGE_REQ:
         XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute (EN_MESSAGE_REQ) - %s ",callid.c_str());
          break;
        case EN_MESSAGE_RES:
         XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute (EN_MESSAGE_RES) - %s ",callid.c_str());
          break;

        case EN_SIPINFO_REQ:
         XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute (EN_SIPINFO_REQ) - %s ",callid.c_str());

          char signal_ptr[strlen(cp.dtmf_digits.c_str()) + 1];

          cp.dtmf_digits.copy(signal_ptr, strlen(cp.dtmf_digits.c_str()), 0);
         XGLOG_INFO("xGateSipController::CallinfoEventExecute signal_ptr = %s ; strlen(signal_ptr) = %d",signal_ptr, strlen(signal_ptr));

          char DTMF_dialed_digit;
          int tmp ;
          char *token, *rest, *temptok;

          while (token = strtok_r(signal_ptr, SIPINFO_DELIM_STR, (char**)&rest))
          {
            if (temptok = strstr(token,SIPINFO_SIGNAL_STR))
            {
              temptok += strlen(SIPINFO_SIGNAL_STR);
              tmp = atoi(temptok);
              if ((tmp == 0) && (*temptok != '0'))
                DTMF_dialed_digit = *temptok;
              else
                DTMF_dialed_digit = switch_rfc2833_to_char(tmp);
             XGLOG_INFO("************IPPBX: DTMF digit = %c \n", DTMF_dialed_digit);
              break;
            }
          }
          if (!token)
           XGLOG_INFO("SIP-Info message does NOT contain Signal-header-info");

          while (token = strtok_r(temptok + strlen(SIPINFO_DELIM_STR) +1, SIPINFO_DELIM_STR, (char**)&rest))
          {
            if (temptok = strstr(token,SIPINFO_DURATION_STR))
            {
              tmp = atoi(temptok + strlen(SIPINFO_DURATION_STR));
             XGLOG_INFO("************IPPBX: Duration = %u ; cp.ptime = %u\n", tmp, cp.ptime);
              if ((tmp < SIPINFO_DURATION_PCMA8000_MIN) || (tmp > SIPINFO_DURATION_PCMA8000_MAX))
              {
                // inform gstreamer to collect dtmf-digits again
              }
              else // else duration of digit occurred in permissible period; send dtmf-digits.
                cp.dtmf_digits.assign(&DTMF_dialed_digit,1);
              break;
            }
          }
          if (!token)
           XGLOG_INFO("SIP-Info message does NOT contain Duration-header-info");
          break;
        case EN_SWB_EVENT_REQ:
         XGLOG_INFO("xGateCallObj::CallinfoEventExecute(EN_SWB_EVENT_REQ)");

          pCallInfo = GetCallProcessobj(callid);
          if(pCallInfo){

           XGLOG_INFO("xGateCallObj::CallinfoEventExecute(EN_SWB_EVENT_REQ) Switch Board Event %d",switchboardinfo.SwbCallEvent);
            //memcpy(&(pCallInfo->GetCallParameter()->swbinfo),&switchboardinfo,sizeof(switchboardinfo));
            //CLog::Detail("xGateCallObj::CallinfoEventExecute(EN_SWB_EVENT_REQ) Switch Board Event %d",pCallInfo->GetCallParameter()->swbinfo.SwbCallEvent);
            if(switchboardinfo.SwbCallEvent <= EN_SWB_IVRCONNECT)
              pCallInfo->GetTask()->RaiseException(EX_SWB_MAIN,"Sofia","App_SwitchBoard");
            else
              pCallInfo->GetTask()->Wakeup();
          }
          break;
        case EN_BYE_REQ:
         XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute  (EN_BYE_REQ) - %s ",callid.c_str());
          pCallInfo = GetCallProcessobj(callid);
          if(pCallInfo){
            //pCallInfo->DeallocateMedia(CD_Incoming,CC_NormalCallClearing,cp);
            pCallInfo->DisconnectInd(CC_NormalCallClearing,CP_NoIndication);

            //CallManager::Instance()->DestroyCall(pCallInfo);
            if(pCallInfo->pPeerCallinfo) {
              //pCallInfo->pPeerCallinfo->DeallocateMedia(CD_Outgoing,CC_NormalCallClearing,cp);
              pCallInfo->pPeerCallinfo->DisconnectInd(CC_NormalCallClearing,CP_NoIndication);
              //CallManager::Instance()->DestroyCall(pCallInfo->pPeerCallinfo);
            }
          }
          break;
        case EN_BYE_RES:
         XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute (EN_BYE_RES) - %s ",callid.c_str());
          break;
        case EN_TERMINATED_REQ:
         XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute (EN_TERMINATED_REQ) - %s ",callid.c_str());
          pCallInfo = GetCallProcessobj(callid);
          if(pCallInfo){
            //pCallInfo->DeallocateMedia(CD_Incoming,callcause,cp);
            pCallInfo->DisconnectInd(callcause,CP_NoIndication);
            //CallManager::Instance()->DestroyCall(pCallInfo);
            if(pCallInfo->pPeerCallinfo) {
              //pCallInfo->pPeerCallinfo->DeallocateMedia(CD_Outgoing,callcauseB,cp);
              pCallInfo->pPeerCallinfo->DisconnectInd(callcauseB,CP_NoIndication);
              //CallManager::Instance()->DestroyCall(pCallInfo->pPeerCallinfo);
            }
          }

          break;
        case EN_TIMEOUT_REQ:
         XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute (EN_TIMEOUT_REQ) - %s ",callid.c_str());
          break;
        default:
         XGLOG_INFO("CALLOBJ","xGateCallObj::CallinfoEventExecute (default) - %s ",callid.c_str());
          break;
      }
    }

#endif
    /*------------------------------------------------------------------------------------------------------
     *                           xGateSipMsgBlock Impementation
     ------------------------------------------------------------------------------------------------------*/


