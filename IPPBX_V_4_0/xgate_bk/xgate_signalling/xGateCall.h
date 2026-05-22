#ifndef XGATE_CALL_OBJECT
#define XGATE_CALL_OBJECT
#include "xGateSipUtil.h"
typedef struct callInfoParameter
{
  callInfoParameter() : routeAddr(""),csCallAgenttype(-1),csCallingContact(""),callingDeviceType(""),\
                        csaCalled(""),cnCalled(""),csaCalling(""),cntCalling(0),cnpCalling(0),cui(""),cpinfo(""),\
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
                        gm_totalNoOfHuntGroupMembers(1),transportType(""),cdrJsonString("") {
                        };
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

class xGateCall{
public :
xGateCall();
~xGateCall();

};
#endif
