#ifndef XGATE_SIP_CONTROLLER
#define XGATE_SIP_CONTROLLER

#include <string>
#include <list>
//#include "callmanager.h"
//#include "call.h"
//#include "rtpmedia.h"
#include "xGateSipUtil.h"
//#include "../xgate_reactor/xGateBaseMsg.h"
#include "xGateMgMsg.h"
#include <ace/Task.h>
#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>

//#include "xGateMultiConference.h"

#include "xGateSipRecording.h"

typedef struct xGateSRSInfo{

std::string m_server_id;
std::string m_server_dns;
std::string m_server_ip;
std::string m_server_mac;
std::string m_present_call_load;
std::string m_max_call_load;
std::string m_callrec_srv_local_weburl;
std::string m_callrec_srv_sip_login_number;
std::string m_callsrv_sip_login_password;
std::string m_callsrv_registrar_sip_url;
std::string m_callsrv_registration_enabled;
std::string m_callsrv_server_enabled;
std::string m_callsrv_server_preference_order;

};

typedef struct xGateSwbInfo{
  std::string PlayFile;
  std::string DigitReceived;
  int DigitTimeOut; // in Secondes i
  int MaxDigit;
  //std::string ToDial;
  xGateSwbEvent SwbCallEvent; 
  operator=(const xGateSwbInfo &rhs){
    PlayFile.assign(rhs.PlayFile.c_str());
    DigitReceived.assign(rhs.DigitReceived.c_str());
    DigitTimeOut = rhs.DigitTimeOut;
    MaxDigit = rhs.MaxDigit;  // here is some problem in assignment ; needs to be solved
    SwbCallEvent = rhs.SwbCallEvent;
  }
};
typedef struct xGateConfInfo{
  xGateConfInfo() : gm_conf_dtmf_digits(""), gm_valid_conf_id(false),gm_conf_valid_pin(false),gm_conf_participant_user(false),\
                    gm_conf_status_flag(0),gm_conf_host_user(0),gm_conf_leg_isactive(0), gm_conf_NoOfValidDialAttempts_by_user(3), \
                    noOfConfParticipantsCurrentlyJoined(0), ConfCallEvent(EN_CONF_NULL), ConfDtmfState(EN_CONF_WELCOMECONNECT),ConfDtmfModule(EN_CONF_GETCONFID),\
                    gm_conf_entryExitTonesEnabled(true), gm_is_host_user_already_in_confCall(false), gm_mgResourceId(""), gm_confuid(""),gm_conference_status("inactive")  {};
  std::string PlayFile;
  std::string DigitReceived;
  int DigitTimeOut; // in Secondes
  int MaxDigit;
  // std::string ToDial;
  unsigned int ConfCallEvent;
  unsigned int conf_callFlowNo;
  unsigned int ConfDtmfState;
  unsigned int ConfDtmfModule;
  pthread_t  conf_timer_thread_1;
  pthread_t  conf_timer_thread_2;
  unsigned int noOfConfParticipantsCurrentlyJoined;
  unsigned int MaxConfParticipants;
  bool isConfLocked;
  bool isConfMute;
  bool isConfonHold;
  std::string gm_conf_id;
  std::string gm_PIN_from_user;
  std::string gm_PIN_from_database; // hostpin
  std::string gm_PARTICIPANT_PIN_from_database;

  // conference members
  std::string gm_conf_dtmf_digits;
  std::string gm_host_user;
  std::string gm_conference_status;

  bool gm_valid_conf_id;
  bool gm_conf_valid_pin;
  bool gm_conf_participant_user;
  int gm_conf_status_flag;
  bool gm_conf_host_user;
  int gm_conf_leg_isactive;
  bool gm_conf_valid_conf_id;
  int gm_conf_NoOfValidDialAttempts_by_user; // hould be int and NOT unsigned-int.
  bool gm_is_host_user_already_in_confCall;

  bool gm_conf_entryExitTonesEnabled;

  int gm_conf_user_type;
  std::string gm_conf_host_extension;
  std::string gm_mgResourceId;
  std::string gm_confuid;


  operator=(const xGateConfInfo &rhs){
    PlayFile.assign(rhs.PlayFile.c_str());
    DigitReceived.assign(rhs.DigitReceived.c_str());
    DigitTimeOut = rhs.DigitTimeOut;
    MaxDigit = rhs.MaxDigit;
    ConfCallEvent = rhs.ConfCallEvent;
    conf_callFlowNo = rhs.conf_callFlowNo;
    ConfDtmfState = rhs.ConfDtmfState;
    ConfDtmfModule = rhs.ConfDtmfModule;
    conf_timer_thread_1 = rhs.conf_timer_thread_1;
    conf_timer_thread_2 = rhs.conf_timer_thread_2;
    noOfConfParticipantsCurrentlyJoined = rhs.noOfConfParticipantsCurrentlyJoined;
    MaxConfParticipants = rhs.MaxConfParticipants;
    isConfLocked = rhs.isConfLocked;
    isConfMute = rhs.isConfMute;
    isConfonHold = rhs.isConfonHold;
    gm_conf_id.assign(rhs.gm_conf_id.c_str());
    gm_PIN_from_user.assign(rhs.gm_PIN_from_user.c_str());
    gm_PIN_from_database.assign(rhs.gm_PIN_from_database.c_str());
    gm_PARTICIPANT_PIN_from_database.assign(rhs.gm_PARTICIPANT_PIN_from_database.c_str());
    gm_conf_dtmf_digits.assign(rhs.gm_conf_dtmf_digits.c_str());
    gm_host_user.assign(rhs.gm_host_user.c_str());
    gm_conference_status = rhs.gm_conference_status;
    //gm_conference_status.assign(rhs.gm_conference_status.c_str());
    gm_valid_conf_id = rhs.gm_valid_conf_id;
    gm_conf_valid_pin = rhs.gm_conf_valid_pin;
    gm_conf_participant_user = rhs.gm_conf_participant_user;
    gm_conf_status_flag = rhs.gm_conf_status_flag;
    gm_conf_host_user = rhs.gm_conf_host_user;
    gm_conf_leg_isactive = rhs.gm_conf_leg_isactive;
    gm_conf_valid_conf_id = rhs.gm_conf_valid_conf_id;
    gm_conf_NoOfValidDialAttempts_by_user = rhs.gm_conf_NoOfValidDialAttempts_by_user;
    gm_is_host_user_already_in_confCall = rhs.gm_is_host_user_already_in_confCall;
    gm_mgResourceId.assign(rhs.gm_mgResourceId.c_str());
    gm_confuid.assign(rhs.gm_confuid.c_str());
    gm_conf_entryExitTonesEnabled = rhs.gm_conf_entryExitTonesEnabled;
    gm_conf_user_type = rhs.gm_conf_user_type;
    gm_conf_host_extension.assign(rhs.gm_conf_host_extension.c_str());
  }
  xGateConfInfo(const xGateConfInfo &rhs){
    PlayFile.assign(rhs.PlayFile.c_str());
    DigitReceived.assign(rhs.DigitReceived.c_str());
    DigitTimeOut = rhs.DigitTimeOut;
    MaxDigit = rhs.MaxDigit;
    ConfCallEvent = rhs.ConfCallEvent;
    conf_callFlowNo = rhs.conf_callFlowNo;
    ConfDtmfState = rhs.ConfDtmfState;
    ConfDtmfModule = rhs.ConfDtmfModule;
    conf_timer_thread_1 = rhs.conf_timer_thread_1;
    conf_timer_thread_2 = rhs.conf_timer_thread_2;
    noOfConfParticipantsCurrentlyJoined = rhs.noOfConfParticipantsCurrentlyJoined;
    MaxConfParticipants = rhs.MaxConfParticipants;
    isConfLocked = rhs.isConfLocked;
    isConfMute = rhs.isConfMute;
    isConfonHold = rhs.isConfonHold;
    gm_conf_id.assign(rhs.gm_conf_id.c_str());
    gm_PIN_from_user.assign(rhs.gm_PIN_from_user.c_str());
    gm_PIN_from_database.assign(rhs.gm_PIN_from_database.c_str());
    gm_PARTICIPANT_PIN_from_database.assign(rhs.gm_PARTICIPANT_PIN_from_database.c_str());
    gm_conf_dtmf_digits.assign(rhs.gm_conf_dtmf_digits.c_str());
    gm_host_user.assign(rhs.gm_host_user.c_str());
    gm_conference_status = rhs.gm_conference_status;
    //gm_conference_status.assign(rhs.gm_conference_status.c_str());
    gm_valid_conf_id = rhs.gm_valid_conf_id;
    gm_conf_valid_pin = rhs.gm_conf_valid_pin;
    gm_conf_participant_user = rhs.gm_conf_participant_user;
    gm_conf_status_flag = rhs.gm_conf_status_flag;
    gm_conf_host_user = rhs.gm_conf_host_user;
    gm_conf_leg_isactive = rhs.gm_conf_leg_isactive;
    gm_conf_valid_conf_id = rhs.gm_conf_valid_conf_id;
    gm_conf_NoOfValidDialAttempts_by_user = rhs.gm_conf_NoOfValidDialAttempts_by_user;
    gm_is_host_user_already_in_confCall = rhs.gm_is_host_user_already_in_confCall;
    gm_mgResourceId.assign(rhs.gm_mgResourceId.c_str());
    gm_confuid.assign(rhs.gm_confuid.c_str());
    gm_conf_entryExitTonesEnabled = rhs.gm_conf_entryExitTonesEnabled;
    gm_conf_user_type = rhs.gm_conf_user_type;
    gm_conf_host_extension.assign(rhs.gm_conf_host_extension.c_str());
  }
};


/*----------------------------------------------------------------------------------
 *                       xGateSipController Delecation
 ------------------------------------------------------------------------------------*/

typedef struct  xGateDirectNumberRecord {
  xGateDirectNumberRecord() : mappedExtension(""), domainName("") {};
  std::string mappedExtension;
  std::string domainName;
};

typedef struct  xGatePstnOperatorDetails {
  xGatePstnOperatorDetails() : operatorName(""), codec(""), vtCode("") {};
  std::string operatorName;
  std::string codec;
  std::string vtCode;
};

typedef struct  multiDevice {
  multiDevice():m_callId(""), m_userNumber(""), m_domainName(""), m_domainId(0), m_calledcodec(1),\
                m_callDirecion(EN_IDLE), m_userDeviceType(""), m_calledDeviceType(""),m_proxyUserName(""),\
                m_userSignalAddr(""), m_outInterface(""), m_routeIpAddress("") {};
  std::string m_callId; //i.e call_id created by sofia object
  std::string m_userNumber;
  std::string m_domainName;
  unsigned int m_domainId;
  int m_calledcodec;
  XGATECALLSTREAM m_callDirecion;
  std::string m_userDeviceType;
  std::string m_calledDeviceType;
  std::string m_proxyUserName; //used by ACME SBC
  std::string m_userSignalAddr;  //immediate peer signalling addredd (i.e) MGC/SBC
  std::string m_outInterface;
  std::string m_routeIpAddress;
};

typedef struct  xGateCallHandlingRecord {
  xGateCallHandlingRecord() : no_of_ring(0), sec_count(0), ring_type(0), device_count(0), device_type(""), ring_type_info(""),
  currentRecordIndex(0), thread_id(0), sequentialRingingTimerStarted(false) {};
  unsigned int no_of_ring;
  unsigned int sec_count;
  unsigned int ring_type;
  unsigned int device_count;

  std::string device_type;
  std::string ring_type_info;

  unsigned int currentRecordIndex;
  pthread_t	thread_id;

  bool sequentialRingingTimerStarted;
};

typedef struct  xGateRegistrationRecord {
  xGateRegistrationRecord() : Contact_address(""), proxy_username(""), appStatus(0), device_type(""), rec_count(0), prev_proxy_username(""),
  IpAddress(""), isSentAppNotification(false), noChangeInCookie(false) {};
  std::string Contact_address;
  std::string proxy_username;
  unsigned int appStatus;
  std::string device_type;
  unsigned int rec_count;
  std::string prev_proxy_username;
  std::string IpAddress;

  bool isSentAppNotification;
  bool noChangeInCookie;
};

typedef std::map<std::string,  multiDevice*> MappedUserList;

typedef struct callInfoParameter
{
  callInfoParameter() : routeAddr(""),csCallAgenttype(-1),csCallingContact(""),callingDeviceType(""),\
                        csaCalled(""),cnCalled(""),siprec_callid(""),csaCalling(""),cntCalling(0),cnpCalling(0),cui(""),cpinfo(""),\
                        isForkingenabled(false),isHuntGroupCall(false),isSwitchBoardCall(false),isSwitchBoardCall_duringDialledExtension(false),inband_dtmf_method(false),isPstnNumber(false), isconferenceCall(false),\
                        sipinfo_method(false),csCallingCodec(-1),cnpCalled(0),cntCalled(0),proxyUserName(""),saveIOSProxyUserName(""),calledDeviceType(""),\
                        call_id(""),csfilename(""),voicemailRecordingFile(""),callForwardingFile(""),callOnHoldFilenameCalledParty(""),\
                        callOnHoldFilenameCallingParty(""),rbtFile(""),Legid(-1),PbState(0),ccp(0),crUserName(""), crPassword(""),callRecording(0),\
                        gm_site_code(""),gm_switch_code(""),gm_db_selector(""),gm_trunk_in(""),gm_port_in(""),\
                        gm_trunk_out(""),gm_port_out(""),gm_A_domain_id(""),gm_user_A_DOMAIN_NAME(""),gm_mgc_ip(""),gm_my_ip_address(""),\
                        gm_B_domain_id(""),gm_user_B_DOMAIN_NAME(""),gm_call_forward_number(""),gm_user_greeting_file_name_user_B(""),\
                        gm_user_greeting_file_name_user_B_After(""),   gm_user_connecting_file_name_user_B(""),gm_user_rbt_file_name_user_B(""),\
                        gm_swb_custom_ivr(""),gm_swb_custom_ivr_after(""),gm_swb_company_greeting_file(""),gm_swb_swb_id(""),gm_swb_operator_extension(""),\
                        gm_swb_comp_hour_extension(""),gm_swb_comp_hour_extension_after(""),uid(""), isThisCallForwarded(false),csCalledCodec(8),\
                        gm_appNotificationDialOutSent(false),gm_legA_deskphone_Status(true), gm_legB_deskphone_Status(true), gm_blocked_no_ivr(""), \
                        gm_is_blocked_number(false), gm_pinToConnect_dtmfDigits(""), gm_pin_to_connect(false), gm_is_direct_call_status(false), \
                        gm_is_direct_call_pin_required(false), gm_direct_call_pin(""), gm_isDNDEnabled(false) , g_noOfConfParticipantsCurrentlyJoined(0),\
                        ptrCallHandlingVector(NULL), ptr_xGateRegistrationVector(NULL),routeIpAddress(""), gm_totalNoOfRegisteredUsers(0),isTransferenabled(false),\
                        gm_totalNoOfHuntGroupMembers(1),transportType(""),cdrJsonString(""),xgateCallFeature(XGATECALLFEATURE::EN_NULL_CALL) {
                        };
  std::string routeAddr;
  std::string csCallingContact;
  std::string callingDeviceType;
  std::string calledDeviceType;
  std::string csaCalled;
  std::string cnCalled;
  std::string siprec_callid;
  std::string csaCalling;
  std::string cnCalling;
  std::string proxyUserName;
  std::string routeIpAddress;
  std::string saveIOSProxyUserName;
  std::string call_id;
  std::string uid;
  std::string csfilename;
  std::string voicemailRecordingFile;
  std::string callForwardingFile;
  std::string callOnHoldFilenameCalledParty; // Music of B party
  std::string callOnHoldFilenameCallingParty;// Music of  A Party;
  std::string rbtFile;
  std::string userGreetingFile; // userConnectingFile file play back
  std::string userConnectingFile; // userConnectingFile file play back
  std::string crUserName;
  std::string crPassword; 
  std::string cui;
  std::string cpinfo;
  std::string gm_site_code;
  std::string gm_switch_code;
  std::string gm_db_selector;
  std::string gm_trunk_in;
  std::string gm_port_in;
  std::string gm_trunk_out;
  std::string gm_port_out;
  std::string gm_mgc_ip;
  std::string gm_my_ip_address;
  std::string gm_A_domain_id;
  std::string gm_user_A_DOMAIN_NAME;
  std::string gm_B_domain_id;
  std::string gm_user_B_DOMAIN_NAME;
  std::string gm_swb_custom_ivr;
  std::string gm_swb_custom_ivr_after;
  std::string gm_call_forward_number;
  std::string gm_user_greeting_file_name_user_B;
  std::string gm_user_greeting_file_name_user_B_After;
  std::string gm_user_connecting_file_name_user_B;
  std::string gm_user_rbt_file_name_user_B;
  std::string gm_swb_swb_id;
  std::string gm_swb_operator_extension;
  std::string gm_swb_comp_hour_extension;
  std::string gm_swb_comp_hour_extension_after;
  std::string gm_swb_company_greeting_file;
  std::string transportType;
  std::string cdrJsonString; // Will hold cdr information in json format

  // Following members are related to "PIN-to-Connect" feature
  std::string gm_pinToConnect_dtmfDigits;
  bool gm_pin_to_connect;

  int Legid;
  int PbState;
  int cntCalling;
  int  cnpCalling;
  int callRecording;
  int csCallAgenttype;
  int csCallingCodec;
  int csCalledCodec;
  int cnpCalled;
  int cntCalled;
  int ccp;
  int gm_user_company_hour;
  int gm_incoming_calling_type;
  int gm_user_rbt_user_B_type;
  int gm_swb_company_hour;
  int gm_swb_custom_ivr_present;
  int gm_swb_custom_ivr_present_after;
  int gm_swb_greeting_type;
  int gm_swb_greeting_type_after;
  int g_swb_call_state_current;
  int g_swb_call_state_previous;
  int g_swb_SleepTimeOut_MainLoop;
  int gm_swb_caller_no_action;
  int gm_swb_caller_no_action_after;
  int gm_swb_auto_routing;
  int gm_swb_auto_routing_after;

  unsigned int gm_dialTimeout;
  unsigned int gm_totalNoOfRegisteredUsers;
  unsigned int gm_totalNoOfHuntGroupMembers;

  bool isForkingenabled;
  bool isHuntGroupCall;
  bool isSwitchBoardCall;
  bool isSwitchBoardCall_duringDialledExtension;
  bool isconferenceCall;
  bool isThisCallForwarded;
  bool isPstnNumber;
  bool isTransferenabled;
  bool inband_dtmf_method;
  bool sipinfo_method;
  bool gm_isThisCallForwarded_CompanyHours;
  bool gm_isThisCallForwarded_AfterHours;
  bool gm_userDiallDTMFdigits;

  bool gm_legA_deskphone_Status;
  bool gm_legB_deskphone_Status;
  bool gm_is_callscreening_enabled;
  bool gm_isDNDEnabled;

  pthread_t  appNotification_timer_thread_1;
  pthread_t  callHandling_timer_thread_1;
  bool gm_appNotificationDialOutSent;
  std::string gm_blocked_no_ivr;
  bool gm_is_blocked_number;
  unsigned int gm_block_type;

  bool gm_is_direct_call_status;
  bool gm_is_direct_call_pin_required;
  std::string gm_direct_call_pin;
 XGATECALLFEATURE xgateCallFeature;  // call feature list 


  xGateSwbInfo swbinfo;
  xGateConfInfo confinfo;
  unsigned int g_noOfConfParticipantsCurrentlyJoined;
  //xGateCallHandlingRecord callHandling;
  std::vector<xGateCallHandlingRecord>* ptrCallHandlingVector;
  xGateDirectNumberRecord directNumberRecord;
  xGatePstnOperatorDetails pstnOperatorDetails;
  //std::map<std::string,  multiDevice*> m_mappedUserList;
  MappedUserList m_mappedUserList;
  std::vector<DBQueryResultData>* ptr_DBQueryResultData;
  std::vector<xGateRegistrationRecord>* ptr_xGateRegistrationVector;
  callInfoParameter(const callInfoParameter &rhs);
  callInfoParameter &operator=(const callInfoParameter &rhs); 
};
// This structure will hold the CDR informations
// the structure will be contained inside the sofia object till call ends
typedef struct xGateCDRStruct{
std::string uid;
std::string grpid;
std::string callDateTime;
std::string connectedTime;
std::string disconnectedTime;
std::string domainName;
std::string callFeature;
std::string pbxip;
std::string operatorIP;
std::string callingCodec;
std::string calledCoded;
std::string callType;
std::string mgip;
std::string callForwardNo;
std::string disconnectReason;
std::string digitPressed;
std::string transferNo;
std::string  conferenceType;
std::string conferenceNumber;
std::string conferenceExt;

int callingNo;
int calledNo; 
long callDuration;
int domainID;
int pbxid;
int mgid;
int bundleID;
long holdDuration;
bool callForwarded;
bool callRecording;
int recordingType;
bool callTransfer;
int confJoinerType;
xGateCDRStruct();
xGateCDRStruct& operator=(const xGateCDRStruct &rhs);
xGateCDRStruct(const xGateCDRStruct &rhs);
std::string setCurrentDateTime();
std::string setCurrentTime();
void  updateCDRInfo(const callInfoParameter &cp);
std::string formJsonString();
};

typedef struct xGateCallState {
  std::string callid;
  std::string ipaddr;
  std::string dialipaddr;
  std::string respipaddr;
  unsigned int port;
  unsigned int DialPort;
  unsigned int ResponsePort;
  unsigned int codec;
  unsigned int ptime;
  std::string ice_ufrag;
  std::string ice_pwd;
  std::string fingerprint;
  std::string ssrc;
  std::string cname;
  std::string mslabel;
  std::string label;
  std::string media_proto;
  XGATECALLSTREAM  calldir;
  int callstate;
  callInfoParameter cp;
  XGATESIPEVENT sipeventtype; // represent in SIP state side
  std::string media_dtmf_digits;
  //CTOR
  xGateCallState::xGateCallState():callid(""),ipaddr(""),port(0),codec(0),calldir(0),callstate(0),sipeventtype(0),\
                                   dialipaddr(""),respipaddr(""),DialPort(0),ResponsePort(0),ptime(0),ice_ufrag(""),ice_pwd(""),fingerprint(""),ssrc(""),\
                                   cname(""),mslabel(""),label(""),media_proto(""),media_dtmf_digits("") 
  {
    //memset(&cp, 0, sizeof cp);
  };

};
typedef struct xGateAVCodec{
  //std::string Value; // ABNF form
  std::string name;
  std::string fmtp;
  std::string miscparam; //other useful informatin on codec
  unsigned long samplingrate;
  unsigned int payloadtype;
  xGateAVCodec(){
    //Value = "";
    name = "";
    fmtp = "";
    miscparam = "";
    samplingrate = 0;
    payloadtype = 0;
  }
  void SetPayload(int pt){
    payloadtype =  pt;
  }
  void SetCodecName(const char *cn){
    name = cn;
  }
  void SetSamplingrate(int srate){
    samplingrate =  srate;
  }
  void ExtractCodecInfo(std::string Value){

    char *token = strtok(Value.c_str()," /");
    int count  = 1;
    while(token){
      switch(count){
        case 1: //payload
          //       payloadtype = atoi(token);
          SetPayload(atoi(token));
          break;
        case 2: // codec
          //     name = token;
          SetCodecName(token);     
          break;
        case 3: // sampling
          //  samplingrate = atoi(token);
          SetSamplingrate(atoi(token));
          break;
      }
      count++;
      token = strtok(NULL," /");
    }
  }


};
//Recording Inof
#if 1 
typedef struct xGateRecordingInfo{
  std::string callid;
  std::string from_number;
  std::string to_number;
  std::string domain_name;
  std::string domain_id;
  std::string extension;
  std::string file_path;
  std::vector<xGateRecordingInfo*> Rec_Info;  
  xGateRecordingInfo(){
    callid = "";
    from_number = "";
    to_number = "";
    domain_name = "";
    domain_id = "";
    extension = "";
    file_path = "";
  }
  operator = (const xGateRecordingInfo rhs){
    callid  = rhs.callid;
    from_number = rhs.from_number;
    to_number = rhs.to_number;   
    domain_name = rhs.domain_name;
    domain_id = rhs.domain_id;
    extension = rhs.extension;
    file_path = rhs.file_path;
   
  }
};
#endif


// TODO below stru need to modified with sub structure for multi code support
typedef struct xGateSDPInfo{
  std::string ipaddress;
  std::string mediadialip;
  std::string mediaresponseip;
  std::string relayip;
  std::string reflexip;
  std::string codename;
  std::string mediamode;
  std::string iceufrag;
  std::string icepwd;
  std::string fingerprint;
  std::string ssrc;
  std::string cname;
  std::string mslabel;
  std::string label;
  std::string rtcpip;
  std::string outproto;
  std::string media_dtmf_digits;
  std::string sendreceive;
  int port;
  int mediadialport;
  int mediaresponseport;
  int relayport;
  int reflexport;
  int samplingrate;
  int payloadtype;
  int mediatype; //0 audio 1 video
  int ptime;
  int rtcpport;
  int codec; // MG input 
  int maxcodec;
  //long sessionid;
  XGATERBTSTATE xGateRbtstate;
  //xGateAVCodec audiocodec[MAXAVCODEC];
  std::vector<xGateAVCodec*> avcodec;
  //xGateAVCodec videocodec[MAXAVCODEC];
  xGateSDPInfo(){
    ipaddress = "";
    mediadialip = "0.0.0.0";
    mediaresponseip = "0.0.0.0";
    relayip = "";
    reflexip = "";
    codename = "";
    mediamode = "";
    iceufrag = "";
    icepwd = "";
    fingerprint = "";
    ssrc = "";
    cname = "";
    mslabel = "";
    label = "";
    rtcpip = "";
    outproto = "";
    sendreceive = "sendrecv";
    xGateRbtstate = EN_NULLRBT;
    ptime = 0;
    avcodec = std::vector<xGateAVCodec*>();
    mediatype = 0 ;//sdp_media_x;
    //for(int i =0 ; i < MAXAVCODEC; i++){
    //memset(audiocodec[i],0, sizeof (xGateAVCodec));
    //}
  }
  operator = (const xGateSDPInfo rhs){
    ipaddress = rhs.ipaddress;
    mediadialip= rhs.mediadialip;
    mediaresponseip.clear(); // Since crash ocured during copy first clearing and then copying it yadav (temp solution)
    mediaresponseip= rhs.mediaresponseip;
    relayip= rhs.relayip;
    reflexip= rhs.reflexip;
    codename= rhs.codename;
    mediamode= rhs.mediamode;
    iceufrag= rhs.iceufrag;
    icepwd= rhs.icepwd;
    fingerprint= rhs.fingerprint;
    ssrc= rhs.ssrc;
    cname= rhs.cname;
    mslabel= rhs.mslabel;
    label= rhs.label;
    rtcpip= rhs.rtcpip;
    sendreceive = rhs.sendreceive;
    // outproto= rhs.outproto;
    outproto = "DTLS";
    media_dtmf_digits= rhs.media_dtmf_digits;
    port= rhs.port;
    mediadialport= rhs.mediadialport;
    mediaresponseport= rhs.mediaresponseport;
    relayport= rhs.relayport;
    reflexport= rhs.reflexport;
    samplingrate= rhs.samplingrate;
    payloadtype= rhs.payloadtype;
    mediatype= rhs.mediatype; //0 audio 1 video
    ptime= rhs.ptime;
    rtcpport= rhs.rtcpport;
    codec= rhs.codec; // MG input
  }

};

typedef struct xGateCallObj {
  std::string callid;     
  std::string uid ;// Unique id 
  callInfoParameter cp;
  XGATESIPEVENT sipeventtype; // represent in SIP state side
  //  xGateSwbInfo switchboardinfo; // Switch board event and inforation container
  unsigned long mp3_duration;
  unsigned long mp3_filesize;
  bool isRecordingStatus;
  xGateSDPInfo sdpInfo[2];
  void xGateCallObjInit(){
    sipeventtype = XGATESIPEVENT::EN_NULL;
  }
};

typedef struct xGateDBObj 
{
  callInfoParameter cp;
  XGATEDBEVENT dbevent;
  std::string transid;
  void *dbEntry;
  int rec_count;
  void xGateDBObjInit(){
  }
};

std::list<xGateCallObj*> callObjList;
ACE_Recursive_Thread_Mutex rmutex_;

void WriteToReceiver(xGateCallObj *obj){
  {
    //ACE_Guard<ACE_Thread_Mutex>gaurd(rmutex_);
    ACE_Guard<ACE_Recursive_Thread_Mutex>gaurd(rmutex_);
    callObjList.push_back(obj);
  }
}

xGateCallObj* ReadFromReceiver(){
  xGateCallObj *temp = NULL;
  {
    //   ACE_Guard<ACE_Thread_Mutex>gaurd(rmutex_);
    ACE_Guard<ACE_Recursive_Thread_Mutex>gaurd(rmutex_);
    if(!callObjList.empty()){
      temp = callObjList.front();
      callObjList.pop_front();
    }

  }
  return temp;
}
typedef struct xGateSbcinfo{
  std::string ipadd;
  int state;
}xGatePbxinfo;

typedef struct xGateSofiaobj{
  std::string callid;
  std::string uid; //unique id for all group
  std::string dialcallid;
  int mgid;
  void *sofiamh; // root memeory magic
  void *sofianh; // dialog handle inside root
  void *callEntry; // HASH MAP Entry
  void *siptag; // sip_t message
  void *lsdp; // sdp session of local
  void *rsdp; // sdp session of remote
  xGateSwbInfo switchboardparam; // All information for switch board need to done here 
  xGateRecordingInfo recrodinginfo;//Recording values
  //xGateConfInfo conferenceparam; // All information for switch board need to done here
  XGATECALLSTREAM callstream;
  XGATEPLAYFILE fileState; // current menu Play file state;
  std::string siptransport;
  //  CallState callstate; // hold the Call object state after process reply
  XGATECALLSTATE callstate;
  callInfoParameter cp;
  xGateCDRStruct cdr; 
  XGATESIPEVENT sipeventtype;
  XGATEMEDIASTATE mediastate;
  xGateSDPInfo localSDPinfo[MAXMEDIA]; // Dialout or Response SDP information  
  xGateSDPInfo remoteSDPinfo[MAXMEDIA]; // Invite received or 200 OK sent SDP information
  xGateSDPInfo tempSDPinfo[MAXMEDIA]; // tempory sdp information from MG hold  shou;d clena after swap
  xGateSipRecording sipRec;
  int P2PMedia; // 1 represents P2P enabled 0 as disabled
  int Sipcallstate;
  std::vector<std::string> sofiaPeerId; 
//  xGateMultiConference ad_hocConf; // Currently this attribute will hold the multiconference flow inside;  
  //  xGateSofiaobj *SofiaLegB;
  // std::vector<xGateSofiaobj*> SofiaForkLeg;
  void xGateCopyPeerInfo(xGateSofiaobj &sobj){// Copy only Required information
    callid = sobj.callid;
    dialcallid = sobj.dialcallid;
    uid =  sobj.uid;
    sofiamh = sobj.sofiamh;
    callEntry = sobj.callEntry;
    siptag = sobj.siptag;
    lsdp = sobj.lsdp;
    rsdp = sobj.rsdp;
    mgid = sobj.mgid;
    cp = sobj.cp;
    //SofiaLegB = &sobj;
  }
  xGateSofiaobj(){
    callid ="";
    uid ="";
    dialcallid="";
    siptransport = "transport=udp";
    callstate = EN_CS_NULL;
    siptag = NULL;
    callstream = EN_IDLE;
    mediastate = EN_MED_NULL;
    lsdp = NULL;
    rsdp = NULL;
    mgid =-1;
    //    SofiaLegB = NULL; // leg B has peer information
    P2PMedia = 0;
  }
  void DeleteAll(){
    if(lsdp) delete lsdp;
    if(rsdp)  delete rsdp;

  }

}; // 
// 
typedef union xGateMessageBlock{
  xGateCallObj* callobj;
  //xGateCallState* callstate;
};

typedef struct xGateCallPair{
  xGateSofiaobj* Orgcall;
  xGateSofiaobj* Dstcall;

  xGateCallPair(){
    Orgcall = NULL;
    Dstcall = NULL;
  }
  ~xGateCallPair(){
    Orgcall = NULL;
    Dstcall = NULL;

  }
};

 //typedef ACE_Hash_Map_Manager<ACE_TString, xGateSofiaobj*,ACE_SYNCH_RW_MUTEX> SipRecParties;
 struct xGateSipRecPair {
     xGateSofiaobj*                         srsLeg;
     std::map<std::string, xGateSofiaobj*>  sipRecParties;
     xGateSipRecPair() {
         srsLeg = NULL;
     }
     ~xGateSipRecPair() {
        if(srsLeg) delete srsLeg;
        srsLeg = NULL;
        sipRecParties.clear();
     }
 };


typedef struct xGateSoifaSipRegobj_{
  std::string callid;
  std::string ipaddress;
  std::string contactaddress;
  std::string domainname;
  std::string username;
  std::string password;
  std::string deviceid;
  std::string mobileno;
  std::string devicetype;
  std::string proxyusername;
  std::string macaddr;
  REGAAA aaa; // Accounting-Authorising-Authenticating
  int expries;
  int cseq;
  int addresstype;
  int status;
  XGATEREGEVENT Regevent;
  void  *Reghandle;
  void *regsip;
  void *sofiamh;
  void *sofiahmsg;
  void *siptags;
  xGateSoifaSipRegobj(){
    callid = "";
    ipaddress = "";
    contactaddress = "";
    proxyusername = "";
    domainname = "";
    addresstype=4;
    username = "";
    password = "";
    deviceid = "";
    mobileno = "";
    devicetype = "DCAPP";
    aaa  = EN_AAA_NULL;
    expries = 3600;
    cseq = 0;
    Reghandle = NULL;
    Regevent = 0;
    regsip = NULL;
    sofiamh = NULL;
    siptags = NULL;
  }

}xGateSofiaSipRegobj;

std::list<xGateSofiaSipRegobj*> xGateRegQ;
ACE_Thread_Mutex regmutex_;

void PostToReceiver(xGateSofiaSipRegobj *obj){
  {
    ACE_Guard<ACE_Thread_Mutex>gaurd(regmutex_);
    xGateRegQ.push_back(obj);
  }
}

xGateSofiaSipRegobj *PopFromReceiver(){
  xGateSofiaSipRegobj *temp = NULL;
  {
    //   ACE_Guard<ACE_Thread_Mutex>gaurd(rmutex_);
    if(!xGateRegQ.empty()){
      temp = xGateRegQ.front();
      xGateRegQ.pop_front();
    }

  }
  return temp;
}
/*----------------------------------------------------------------------------------
 *                       xGateSipMsgBlock Delecation
 ------------------------------------------------------------------------------------*/
class xGateSipMsgBlock : public xGateBaseMsg{
  public:
    //xGateSipMsgBlock():xGateBaseMsg(EN_XGATE_MSG_UNKNOWN){
    xGateSipMsgBlock():xGateBaseMsg(EN_XGATE_MSG_SIP_IN){
    }

    xGateSipMsgBlock(xGateMsgType mgtype) :xGateBaseMsg(mgtype){
    }
    virtual ~xGateSipMsgBlock(){

    }

    xGateCallObj *m_callObj;
    xGateDBObj *m_dbObj;
    //xGateCallState *m_callstate;
    xGateSofiaSipRegobj *m_sipregobj;
};

class xGateSipController  {

  // Member vaiables


  //typedef ACE_Hash_Map_Manager<ACE_TString, xGateSofiaobj*, ACE_SYNCH_RW_MUTEX> HASH_CALL_MAP;
  //    HASH_CALL_MAP m_incomingCallHashTable ;
  private:    
    typedef ACE_Hash_Map_Manager<ACE_TString, xGateCallPair*,ACE_SYNCH_RW_MUTEX> CALL_CONNECTED_LIST;
    typedef ACE_Hash_Map_Manager<ACE_TString, xGateSipRecPair*,ACE_SYNCH_RW_MUTEX> SRS_CONNECTED_LIST;
    CALL_CONNECTED_LIST m_CallConnectedList;
    SRS_CONNECTED_LIST m_SRScallConnectedList;

    typedef ACE_Hash_Map_Manager<ACE_TString, std::map<std::string,xGateSofiaobj*>* , ACE_SYNCH_RW_MUTEX> CALL_LIST;
    CALL_LIST m_CallConferenceList;
    CALL_LIST m_CallConferenceJoinersList;
    CALL_LIST m_CallDialList;
    CALL_LIST m_ExtChannelList;
    // CALL_LIST m_dummy;
    CALL_LIST m_CallHuntGroupList;
    typedef ACE_Hash_Map_Manager<ACE_TString, std::map<std::string,xGateDBObj*>* , ACE_SYNCH_RW_MUTEX> DBSP_LIST;
    DBSP_LIST m_SPList;
    //HASH_CALL_MAP::ENTRY *CallTableEntry;
  public:
    std::vector <xGateSbcinfo*> sbcip;
    std::vector<xGatePbxinfo*>pbxip;
    std::vector<xGateSbcinfo*>siggwip;
    std::vector<xGateSbcinfo*>confmgip;	




    xGateSipController();
    virtual   ~xGateSipController();

    void Siplogger(std::string);

    // Pure derived functions
    virtual xGateRetVal SipLibInit()= 0; // Create the intializing of  SIP socket
    virtual xGateRetVal SipLibLoad()=0; // Loding SIP Library & creating call back
    virtual xGateRetVal SipLibUnLoad()=0; // Releasing Library
    virtual xGateRetVal RunEventLoop()=0; //  Event Polling 

    // SIP Response Functionality
    virtual xGateRetVal RespondWith1xx(xGateCallObj* pSipMgc);// Provisional Response
    virtual xGateRetVal RespondWith2xx(xGateCallObj* pSipMgc); // Succefull Response
    virtual xGateRetVal RespondWith3xx(xGateCallObj* pSipMgc); // Redirectional Response
    virtual xGateRetVal RespondWith4xx(xGateCallObj* pSipMgc); //  Client Failure Response
    virtual xGateRetVal RespondWith5xx(xGateCallObj* pSipMgc); // Server Failure Response
    virtual xGateRetVal RespondWith6xx(xGateCallObj* pSipMgc); // Global Failure REsponse
    virtual xGateSofiaobj* GetProcessEvent(); // Current call state is set
    // Local Functions

    std::string CreateUniqueCallid(char const *user,char const *host, \
        char const *tag, char const *callid); // Creates call id for the call
    xGateSofiaobj *GetCallInfo(std::string callid); // Get info from Hash Map
    bool InsertCallInfo(xGateSofiaobj *sofiaobj); // Set info to Hash Map
    bool DeleteCallInfo(xGateSofiaobj *sofiaobj); // Delete info from Hash Map


    xGateCallPair*GetConnectedList(std::string key);
    bool InsertConnectedList(std::string key,xGateSofiaobj* sobj,XGATEOBJECT type );
    bool DeleteConnectedList(std::string key,xGateSofiaobj* sobj,XGATEOBJECT type);

    /* SRS  */
     xGateSipRecPair* xGateSipController::GetSRSconnectedList(std::string key);
     bool xGateSipController::InsertSRSconnectedList(std:: string key, xGateSofiaobj* sobj, XGATEOBJECT type);
     bool xGateSipController::DeleteSRSconnectedList(std::string key, XGATEOBJECT type);

    std::map<std::string,xGateSofiaobj*>*GetCallList(std::string grpkey);
    bool InsertCallList(std::string grpkey, xGateSofiaobj *sobj);
    bool RemoveCallListValues(std::string grpkey,std::map<std::string, xGateSofiaobj*>*);
    int RemoveCallListValue(std::string grpkey, std::string callid );
    bool DeleteCallList(std::string grpkey);


    std::map<std::string,xGateSofiaobj*>*GetExtChannelList(std::string grpkey);
    bool InsertExtChannelList(std::string grpkey, xGateSofiaobj *sobj);
    //    bool RemoveCallListValues(std::string grpkey,std::map<std::string, xGateSofiaobj*>*,CALL_LIST callList);
    int RemoveExtChannelValue(std::string grpkey, std::string callid );
    bool DeleteExtChannelList(std::string grpkey);

    // Conference-related
    std::map<std::string,xGateSofiaobj*>*GetConferenceCallList(std::string grpkey);
    std::string GetUid_FromFirstConferenceCall(std::string grpkey);
    xGateSofiaobj* GetSofiaObj_FromFirstConferenceCall(std::string grpkey);
    bool InsertCalltoConferenceList(std::string confUID, xGateSofiaobj *sobj);
    bool DeleteCallFromConferenceList(std::string confUID, xGateSofiaobj *sobj);
    bool DeleteConferenceList(std::string grpkey);
    bool RemoveConferenceListValue(std::string grpkey,std::map<std::string, xGateSofiaobj*>*);

    std::map<std::string,xGateDBObj*>*GetSPList(std::string grpkey);
    bool InsertSPList(std::string transID, xGateDBObj *dbObj);
    bool DeleteSPFromSPList(std::string transID, xGateDBObj *dbObj);
    bool DeleteSPList(std::string grpkey);
    bool RemoveSPListValue(std::string grpkey,std::map<std::string, xGateDBObj*>*);

    // HuntGroup-related
    std::map<std::string,xGateSofiaobj*>*GetHuntGroupCallList(std::string grpkey);
    std::string GetUid_FromFirstHuntGroupCall(std::string grpkey);
    xGateSofiaobj* GetSofiaObj_FromFirstHuntGroupCall(std::string grpkey);
    bool InsertCalltoHuntGroupList(std::string confUID, xGateSofiaobj *sobj);
    bool DeleteCallFromHuntGroupList(std::string confUID, xGateSofiaobj *sobj);
    bool DeleteHuntGroupList(std::string grpkey);
    bool RemoveHuntGroupListValue(std::string grpkey,std::map<std::string, xGateSofiaobj*>*);

    bool PostMessageToProcessor(xGateCallObj *callObj);
    bool PostMessage(xGateCallObj *callObj,XGATEPOSTTYPE destType);
    bool PostMessageToMGCDispatcher(std::string call_id,std::string ip_addr,unsigned int ip_port,\
        unsigned int codec, unsigned int calldirection,unsigned int sigtype,int mgid );
    bool PostMessageToMGCDispatcher(xGateSofiaobj *sofiaobj,XGATEMGUPDATE mgMsgType=EN_MEDIA_CREATE);
    bool PostMessage(xGateSipMsgBlock *SipMsg, XGATEPOSTTYPE destType); // General Post message for all the ACE thread
    bool ValidateSipMsgPointer(const char *p);
    std::string GetCurrentSbcInfo(); // Get the current SBC for routing the call, on empty will return null string
    std::string GetCurrentPbxInfo();
    std::string GetSignalGateWayInfo();
    std::string GetConfernecMgIp(); // this for temp function to support conference
    void ReadProxyConfigFile();
    void TokenizingSettings(std::string sLine);
    // Business Logic
    bool CheckDomainForCallAuth(std::string cDomainName);// This is Busniess Logic for Auth Enable
    int CheckforP2PMedia(xGateSofiaobj *sobj); // This is Busniess Loic for Peer-Peer Media enable
    std::string GetRtpPortType(xGateSofiaobj *sofiaobj); // This is Busniess Logic for meida rtp port request
    // Media 
    std::string CreateMedialine(int mediaType, int port, std::string mediaPath);
    std::string AppendMedialine(std::string preValue, std::string currentValue);
    std::string CreateConnectionline(int networkType, std::string ipaddr);
    std::string CreateAttributeline(std::string header,std::string value);
    std::string AppendAttributeline(std::string preValue, std::string currentValue);
    std::string SdpCodec(int pt,std::string codecname);


  private:
    int m_testing;
 
 protected:

    xGateSRSInfo  m_SRSServerInfo;
       
};
#endif
