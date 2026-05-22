/* $Header: /xGate/sofia.cpp    6/26/13 okY
 *   
 * XGATE
 * (C) 2013 Switchlab, Ltd
 *
 * Sofia SIP signalling.
 *
 * $Log: /xGate/sofia.cpp $
 *
 * 6   01/10/13 10:30 okY
 -Fixed/Release Incoming and Outgoing Precondition(RFC3312) support
 * 5   12/12/13 10:30 okY
 -Auth domain is parsed from Auth header
 -SIP/ISUP Overlap support for incoming and outgoing call
 -Fix bug in authenticatecall()
 -Fix bug in incoming registration handling
 * 4    2/12/13 10:30 oky
 Change all C string function to secured version
 * 3    2/10/13 10:30 oky&dandi
 Release version 2 with major updates:
 - Added support for TCP and TLS transport
 - Improve SIP header -> Call Param translation
 - Improve Call Param -> Sip header translation
 - Add support for media bypass via virtual RTP interface
 - Add support for session update and re-invite
 - Support for Hold/Resume request and indication
 - Major bug fix/improvement on call error/disconnect
 - Outgoing call authentification via call credential
 - Incoming call authentification
 - Improvement in 401/407 challenge generation
 (Nonce signature generate/checking)
 * Fixed incoming call problem
 * 2    7/31/13 10:30 okY
 * Fixed incoming call problem
 * 1    6/26/13 10:30 okY
 * Initial (alpha) version, only skeleton
 * 
 */
#include "stdafx.h"
#include "xgconfig.h"

#ifdef XGATE_HAS_SOFIA
#include "sipagent.h"
#include "sip.h"
#include "rtpmedia.h"
#include "klog.h"
#include "api.h"
#include "callmanager.h"
#include "kernel.h"
#include "sofia.h"
#include "types.h"
#include "thread_process.h"
#ifdef XGATE_HAS_HMP
#include "hostmedia.h"
//#include "authDeatls.cpp"
#endif
#include "dbdriver.h"

#include <sstream>
#include <string>
#include <string.h>
#include <iomanip>
#include <map>

#define LOG_ERR   0
#define LOG_INFO  0
#define LOG_DEBUG 0
#define LOG_WARN  0
//#define HAS_TLS

#define PLOGERR(fmt, ...)   if(LOG_ERR)printf("\n [ ERROR ] %s:%d:%s "fmt,__FILE__, __LINE__,__FUNCTION__,##__VA_ARGS__)

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
#include <sofia-sip/su_md5.h>
}

#include "sofia.h"

//dandi add to include MD5/Digest Calculation 
#include "../encryption/digest.h"

#define THISMODULE  "sofia"
#define SU_MD5_DIGEST_SIZE 16
#define FALSE 0
#define TRUE 1

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define strcmpi strcasecmp

// call transfer feature invoked by DTMF digits defined as below.
#define CALL_TRANFER_DTMF_FIRST_DIGIT  '#'
#define CALL_TRANFER_DTMF_SECOND_DIGIT '3'
#define CALL_TRANFER_DTMF_THIRD_DIGIT '8'

// how many no. of digits transfer-target extension should have.
#define TRANSFER_TARGET_EXTENSION_LENGTH	30

// call pickup feature invoked by DTMF digits defined as below.
#define CALL_PICKUP_DTMF_FIRST_DIGIT  '*'
#define CALL_PICKUP_DTMF_SECOND_DIGIT '5'

// how many no. of digits switch-board extension should have.
#define SWITCHBOARD_EXTENSION_LENGTH	3
#define SWITCHBOARD_COLLECT_MAX_DTMF_DIGITS (SWITCHBOARD_EXTENSION_LENGTH + 2)

#define SWITCHBOARD_DTMF_FIRST_TERM_DIGIT  '#'
#define SWITCHBOARD_DTMF_SECOND_TERM_DIGIT  '9'

static char switchboard_dtmf_digits[SWITCHBOARD_COLLECT_MAX_DTMF_DIGITS+1];
static int switchboard_index;

// SL application name for Registration defined in System.conf
#define SL_APPLICATION_NAME_FOR_REGISTRATION "ippbx_registration";

// SL application name for Registration defined in System.conf
#define SL_APPLICATION_NAME_FOR_BACKGROUND_MUSIC "ippbx_play_background_music";

// SL application name for call-transfer defined in System.conf
#define SL_APPLICATION_NAME_FOR_CALL_TRANSFER "calltransfer";

// SL application name for call-connect defined in System.conf
#define SL_APPLICATION_NAME_FOR_CALL_CONNECT "ippbx_callconnect"; // currently "ippbx_callconnect" is the application for call-connect.

// following default passwd is set in SL. If changed in SL, change it here also.
#define DEFAULT_PASSWD_SET_IN_SL "SL_SCRIPT_hw4jIjXqZJjLG==***SL_SCRIPT"

#define REG_INFO_SEQUENCE  99999;

#ifdef P_PTHREADS
static pthread_mutex_t g_DLLMutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_DLFCN() pthread_mutex_lock(&g_DLLMutex)
#define UNLOCK_DLFCN() pthread_mutex_unlock(&g_DLLMutex)
#else
#define LOCK_DLFCN()
#define UNLOCK_DLFCN()
#endif

enum
{
  Cmd_Require=2,
  Cmd_Session_Timer_Timeout,
};

#ifdef CALL_TRANSFER
map <string,string> to_tag;
map <string,string> from_tag;
map <string,string> call_id;
map <string,string> to_address;
map <string,string> call_transfer_extn;
map <string,string> call_park_code;
map <string,string> call_transfer_calledsigadd;
map <string,string> call_transfer_replace;
vector <string> usr_key;
#endif

pthread_t mdtrXgMonitorThread;  //
map <string,bool> mdtrXg; // map where options results will be updated
vector <string> mdtrXgIps;

map <string, string> mdtrXgCallList;


typedef struct
{
  bool HuntGroupRegisteredUser_answered_call_sent;
  int NoOfRegisteredUsers;
  int CallLegOfRegisteredUser_answered_call;
  char HuntGroupNumber[24];
  char HuntGroupCaller[24];
  char HuntGroupRegisteredUser_answered_call[24];
  char *HuntGroupRegisteredUser[100];
  char *CallingSignalAddress_answered_call[100];
  char *CalledSignalAddress_answered_call[100];
  char *h_key[100];
}hg_data;

map <string, CallParameter *> call_info;
map <CallParameter *, hg_data *> hg_info;

#define MAX_CONF_DIALIN 10

typedef struct
{
  char DialIn_Participant_RegisteredUser[24];
  char c_key[24];
}DialIn_Data;

typedef struct
{
  int NoOfDialInParticipants;
  int dialIn_Disc_Leg;
  char ConfHostUser[24];
  DialIn_Data* conf_di_data[MAX_CONF_DIALIN];
}conf_host_data;

map <CallParameter *, conf_host_data *> conf_info;


#define S_REQUIRE               "require"
#define S_SES_TIMER_TIMEOUT     "session-timer-timeout"
#define S_SES_TIMER             "session-timer"

bool PASSWORD_RECEIVED;

Log_Trace *traceSofiaMsg;
static int pending_registrations=0;

#define MY_DEBUG(p,s)

static const char* getLocalInterface()
{
  CLog::Warning(THISMODULE, "IPPBX : inside getLocalInterface() ***************************\n");
  char hostname[32];
  if (gethostname(hostname,sizeof(hostname) )==SOCKET_ERROR)
    return "";

  unsigned long ipaddr = inet_addr(hostname);

  if (ipaddr == INADDR_NONE) { 
    hostent* phe = gethostbyname(hostname);
    if (phe)
      ipaddr = *((unsigned long*)phe->h_addr);
  }
  CLog::Warning(THISMODULE, "local interface = %s\n", inet_ntoa(*((in_addr*)&ipaddr)));
  return inet_ntoa(*((in_addr*)&ipaddr));
}

bool StringExists(const char *p)
{
  if(p!=NULL)
    if(strlen(p))
      return true;
  return false;
}

bool RegistrationApi::ValidRegistrationParameter(CTask* task,RegisterParam* rparam,const char* funcname)
{
  //if ( rparam && rparam->IsValid()) return true;

  CLog::Error(THISMODULE, task, "Invalid call parameter handle 0x%x passed in %s", rparam, funcname);
  return false;
}

RegistrationApi::RegistrationApi()
{
  //registerManager =NULL;
  CallTransferExtension = "";
  CallTransferExtensionSent = false;
  HuntGroupRegisteredUser_answered_call_sent = false;
  HuntGroupRegisteredUser_answered_call = "";
  NoOfRegisteredUsers = 0;
  /////info.sequence = 0; // initialize info.sequence to initialization value = REG_INFO_SEQUENCE;
}

//RegistrationApi::RegistrationApi(int providerId,const char* name):CAPIProvider(providerId,name,256)
//RegistrationApi::RegistrationApi(SofiaRegistrar* manager,int providerId,const char* name):registerManager(manager),CAPIProvider(providerId,name,256)
RegistrationApi::RegistrationApi(RegistrationApi* manager,int providerId,const char* name):_manager(manager),CAPIProvider(providerId,name,256)
{

  if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
    CLog::Error( THISMODULE, "Unable to register provider");
    return;
  }


  /* Register API entry points*/
  enum RegAPI_Id
  {
    // 10 input parametrs
    Id_RParamGetSipUri = 0,
    Id_RParamGetToAddr,
    Id_RParamGetToken,
    Id_RParamGetContactUser,
    Id_RParamGetNetworkPort,
    Id_RParamGetContactHost,
    Id_RParamGetExpireTime,
    Id_RParamGetNetworkProtocol,
    Id_RParamGetNetworkIP,
    Id_RParamGetHasAuth,

    // 7 output parameters
    Id_RParamPopulatePasswd = 10,
    Id_RParamPopulateStatus,
    Id_RParamPopulateAuthScheme,
    Id_RParamPopulateRealm,
    Id_RParamPopulateNounce,
    Id_RParamPopulateQop,
    Id_RParamPopulateAlgorithm,
    //Id_RParamPopulateAuthDetails,

    // call-transfer APIs
    Id_RParamGetCallTransferExtension,
    Id_RParamGetCallTransferExtensionSent,

    // hunt-group APIs
    Id_RParamSetHuntGroupNumberAndCaller,
    Id_RParamGetHuntGroupNumber,

    Id_RParamSetHuntGroupCalledRegisteredUser,

    Id_RParamGetHuntGroupRegisteredUser_answered_call,
    Id_RParamGetHuntGroupRegisteredUser_answered_call_sent,
    Id_RParamGetCallLegOfRegisteredUser_answered_call,

    // switchboard APIs
    Id_RParamGetSwitchBoard200_OK_received,
    Id_RParamGetSwitchBoardExtension,
    Id_RParamDelete_hg_data,
    Id_SetConfHostUser,
    Id_DialIn_Participant_RegisteredUser,
    Id_Delete_conf_host_data,
    Id_Clear_Dialin_data,
    Id_GetDialInDiscLeg,
    Id_RParamGetSuspendUsr = 35,
    Id_RParamDelSuspendUsr = 36,
    Id_RParamUpdateMap = 37,
    Id_RParamGetCallParkId,
    Id_GetMyMediatrixIP = 39,
    Id_RemoveMyMediatrixMapEntry = 40,
    Id_RParamGetCallingSignalAddress_answered_call = 41,
    Id_SetHuntGroupCallingSignalAddress = 42,
    Id_RParamGetCalledSignalAddress_answered_call = 43,
    Id_SetHuntGroupCalledSignalAddress = 44,
    Id_RParamGetCallTransferReplaces = 45,
    Id_RParamUpdateCallParkMap = 46,
    Id_GetCallParkCode

  };

  // APIs for 10 input parameters
  /*****************
  // GetCallSessionId(CTask* task,CallParameter **cparam,int handle)
  RegisterAPI(
  "GetRegUser",                   
  Id_GetRegUser,                      
  false,                                  
  CVariant::VT_INT,                       
  (CAPIProvider::APIPROC) &RegistrationApi::API_GetRegUser,
  CAPIDef::CDECL_TYPE,                    
  1,                                      
  CArgType("m_sipUri",BYREF,CVariant::VT_STRING));
   ***************/

  // GetSipUri(CTask* task,const char* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetSipUri",                   // external name
      Id_RParamGetSipUri,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetSipUri, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("rparam",BYVAL,CVariant::VT_INT));


  // GetToAddr(CTask* task,const char* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetToAddr",                   // external name
      Id_RParamGetToAddr,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetToAddr, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("rparam",BYVAL,CVariant::VT_INT));

  // GetToken(CTask* task,const char* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetToken",                   // external name
      Id_RParamGetToken,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetToken, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("rparam",BYVAL,CVariant::VT_INT));

  // GetContactUser(CTask* task,const char* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetContactUser",                   // external name
      Id_RParamGetContactUser,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetContactUser, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("rparam",BYVAL,CVariant::VT_INT));

  // GetNetworkPort(CTask* task,const char* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetNetworkPort",                   // external name
      Id_RParamGetNetworkPort,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetNetworkPort, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("rparam",BYVAL,CVariant::VT_INT));

  // GetContactHost(CTask* task,const char* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetContactHost",                   // external name
      Id_RParamGetContactHost,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetContactHost, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("rparam",BYVAL,CVariant::VT_INT));

  // GetExpireTime(CTask* task,const char* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetExpireTime",                   // external name
      Id_RParamGetExpireTime,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetExpireTime, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("rparam",BYVAL,CVariant::VT_INT));

  // GetNetworkProtocol(CTask* task,const char* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetNetworkProtocol",                   // external name
      Id_RParamGetNetworkProtocol,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetNetworkProtocol, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("rparam",BYVAL,CVariant::VT_INT));

  // GetNetworkIP(CTask* task,const char* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetNetworkIP",                   // external name
      Id_RParamGetNetworkIP,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetNetworkIP, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("rparam",BYVAL,CVariant::VT_INT));

  // GetHasAuth(CTask* task,bool* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetHasAuth",                   // external name
      Id_RParamGetHasAuth,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetHasAuth, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("rparam",BYVAL,CVariant::VT_INT));

  // 7 Output parameters
  // PopulatePasswd(CTask* task,RegisterParam* rparam,char * value);
  RegisterAPI(
      "RParamPopulatePasswd",                   // external name
      Id_RParamPopulatePasswd,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_VOID,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_PopulatePasswd, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("rparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // PopulateStatus(CTask* task,RegisterParam* rparam,bool value);
  RegisterAPI(
      "RParamPopulateStatus",                   // external name
      Id_RParamPopulateStatus,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_VOID,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_PopulateStatus, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("rparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_BOOLEAN));

  // PopulateAuthScheme(CTask* task,RegisterParam* rparam,char * value);
  RegisterAPI(
      "RParamPopulateAuthScheme",                   // external name
      Id_RParamPopulateAuthScheme,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_VOID,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_PopulateAuthScheme, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("rparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // PopulateRealm(CTask* task,RegisterParam* rparam,char * value);
  RegisterAPI(
      "RParamPopulateRealm",                   // external name
      Id_RParamPopulateRealm,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_VOID,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_PopulateRealm, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("rparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // PopulateNounce(CTask* task,RegisterParam* rparam,char * value);
  RegisterAPI(
      "RParamPopulateNounce",                   // external name
      Id_RParamPopulateNounce,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_VOID,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_PopulateNounce, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("rparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // PopulateQop(CTask* task,RegisterParam* rparam,char * value);
  RegisterAPI(
      "RParamPopulateQop",                   // external name
      Id_RParamPopulateQop,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_VOID,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_PopulateQop, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("rparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // PopulateAlgorithm(CTask* task,RegisterParam* rparam,char * value);
  RegisterAPI(
      "RParamPopulateAlgorithm",                   // external name
      Id_RParamPopulateAlgorithm,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_VOID,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_PopulateAlgorithm, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("rparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // call-transfer APIs
  RegisterAPI(
      "RParamDelete_hg_data",            // external name
      Id_RParamDelete_hg_data,           // ordinal
      false,                             // asynchronous flag
      CVariant::VT_VOID,                 // return type
      (CAPIProvider::APIPROC) &RegistrationApi::API_Delete_hg_data,
      CAPIDef::CDECL_TYPE,               // calling convention
      2,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // STATUS API_CALL RegistrationApi::API_SetConfHostUser(CTask* task, int cparam, char *host_number)
  RegisterAPI("RParamSetConfHostUser",Id_SetConfHostUser,false,
      CVariant::VT_VOID,
      (CAPIProvider::APIPROC) &RegistrationApi::API_SetConfHostUser,
      CAPIDef::CDECL_TYPE,
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("host_number",BYVAL,CVariant::VT_STRING));

  // STATUS API_CALL RegistrationApi::API_DialIn_Participant_RegisteredUser(CTask* task, int cparam, int handle, char* DialInParticipantUser, int max_dialin_count, char* user_A)
  RegisterAPI("RParamDialIn_Participant_RegisteredUser",Id_DialIn_Participant_RegisteredUser,false,
      CVariant::VT_VOID,
      (CAPIProvider::APIPROC) &RegistrationApi::API_DialIn_Participant_RegisteredUser,
      CAPIDef::CDECL_TYPE,
      5,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      // CArgType("cparam",BYVAL,CVariant::VT_LONG),//changes made for migration 32 bit to 64 bit
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("value1",BYVAL,CVariant::VT_STRING),// userA-conf_participant_Dialed[i]
      CArgType("value3",BYVAL,CVariant::VT_INT));
  //CArgType("value4",BYVAL,CVariant::VT_STRING));

  // STATUS API_CALL RegistrationApi::API_Delete_conf_host_data(CTask* task, int cparam)
  RegisterAPI("RParamDelete_conf_host_data",Id_Delete_conf_host_data,false,
      CVariant::VT_VOID,
      (CAPIProvider::APIPROC) &RegistrationApi::API_Delete_conf_host_data,
      CAPIDef::CDECL_TYPE,
      2,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // STATUS API_CALL RegistrationApi::API_Clear_Dialin_data(CTask* task, int cparami, int handle)
  RegisterAPI("RParamClear_Dialin_data",Id_Clear_Dialin_data,false,
      CVariant::VT_VOID,
      (CAPIProvider::APIPROC) &RegistrationApi::API_Clear_Dialin_data,
      CAPIDef::CDECL_TYPE,
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // STATUS API_CALL RegistrationApi::GetDialInDiscLeg(CTask *task, int *retval, int cparam)
  RegisterAPI("RParamGetDialInDiscLeg",Id_GetDialInDiscLeg,false,
      CVariant::VT_INT,
      (CAPIProvider::APIPROC) &RegistrationApi::GetDialInDiscLeg,
      CAPIDef::CDECL_TYPE,
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "RParamDelSuspendUsr",                   // external name
      Id_RParamDelSuspendUsr,                  // ordinal
      false,                               // asynchronous flag
      CVariant::VT_VOID,                      // return type
      (CAPIProvider::APIPROC) &RegistrationApi::API_DelSuspendUsr,
      CAPIDef::CDECL_TYPE,                    // calling convention
      2,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("key",BYVAL,CVariant::VT_STRING));

  RegisterAPI(
      "RParamGetSuspendUsr",                   // external name
      Id_RParamGetSuspendUsr,                  // ordinal
      false,                               // asynchronous flag
      CVariant::VT_STRING,                      // return type
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetSuspendUsr,
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("key",BYVAL,CVariant::VT_STRING));

  RegisterAPI(
      "RParamUpdateMap",                   // external name
      Id_RParamUpdateMap,                  // ordinal
      false,                               // asynchronous flag
      CVariant::VT_VOID,                      // return type
      (CAPIProvider::APIPROC) &RegistrationApi::API_UpdateMap,
      CAPIDef::CDECL_TYPE,                    // calling convention
      4,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("inputkey",BYVAL,CVariant::VT_STRING),
      CArgType("value",BYVAL,CVariant::VT_STRING),
      CArgType("update_key",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "RParamUpdateCallParkMap",                   // external name
      Id_RParamUpdateCallParkMap,                  // ordinal
      false,                               // asynchronous flag
      CVariant::VT_VOID,                      // return type
      (CAPIProvider::APIPROC) &RegistrationApi::API_UpdateCallParkMap,
      CAPIDef::CDECL_TYPE,                    // calling convention
      4,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("inputkey",BYVAL,CVariant::VT_STRING),
      CArgType("value",BYVAL,CVariant::VT_STRING),
      CArgType("update_key",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "RParamGetCallParkId",                   // external name
      Id_RParamGetCallParkId,                  // ordinal
      false,                               // asynchronous flag
      CVariant::VT_INT,                      // return type
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetCallParkId,
      CAPIDef::CDECL_TYPE,                    // calling convention
      2,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET));


  // GetCallTransferExtension(CTask* task,char* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetCallTransferExtension",                   // external name
      Id_RParamGetCallTransferExtension,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetCallTransferExtension, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
#ifndef CALL_TRANSFER
      CArgType("rparam",BYVAL,CVariant::VT_INT));
#else
  CArgType("bleg",BYVAL,CVariant::VT_STRING));
#endif
#if 0
  // GetCallTransferReplaces(CTask* task,char* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetCallTransferReplaces",                   // external name
      Id_RParamGetCallTransferReplaces,            // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetCallTransferReplaces, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("trans_key",BYVAL,CVariant::VT_STRING));

#endif
  // GetCallTransferExtensionSent(CTask* task,bool* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetCallTransferExtensionSent",                   // external name
      Id_RParamGetCallTransferExtensionSent,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetCallTransferExtensionSent, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
#ifndef CALL_TRANSFER
      CArgType("rparam",BYVAL,CVariant::VT_INT));
#else
  CArgType("bleg",BYVAL,CVariant::VT_STRING));
#endif

  RegisterAPI(
      "RParamGetCallParkCode",                   // external name
      Id_GetCallParkCode,                  // ordinal
      false,                               // asynchronous flag
      CVariant::VT_STRING,                      // return type
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetCallParkCode,
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("bleg",BYVAL,CVariant::VT_STRING));

  // switchboard APIs
  // GetSwitchBoard200_OK_received(CTask* task,bool* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetSwitchBoard200_OK_received",                   // external name
      Id_RParamGetSwitchBoard200_OK_received,                  // ordinal
      false,                               // asynchronous flag
      CVariant::VT_STRING,                      // return type
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetSwitchBoard200_OK_received,
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("rparam",BYVAL,CVariant::VT_INT));

  // GetSwitchBoardExtension(CTask* task,char* retval,RegisterParam* rparam);
  RegisterAPI(
      "RParamGetSwitchBoardExtension",                   // external name
      Id_RParamGetSwitchBoardExtension,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetSwitchBoardExtension, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("rparam",BYVAL,CVariant::VT_INT));

  // Hunt-Group APIs
  // SetHuntGroupNumberAndCaller(CTask* task,RegisterParam* rparam,char* hunt_group_number, char* user_A);
  RegisterAPI(
      "RParamSetHuntGroupNumberAndCaller",                     // external name
      Id_RParamSetHuntGroupNumberAndCaller,                    // ordinal
      false,                                  // asynchronous flag
      CVariant::VT_VOID,                      // return type
      (CAPIProvider::APIPROC) &RegistrationApi::API_SetHuntGroupNumberAndCaller,
      CAPIDef::CDECL_TYPE,                    // calling convention
      5,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("rparam",BYVAL,CVariant::VT_INT),
      CArgType("hunt_group_number",BYVAL,CVariant::VT_STRING),
      CArgType("user_A",BYVAL,CVariant::VT_STRING));

  // GetHuntGroupNumber(CTask* task,char* retval,RegisterParam* rparam, char* user_A);
  RegisterAPI(
      "RParamGetHuntGroupNumber",                   // external name
      Id_RParamGetHuntGroupNumber,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetHuntGroupNumber, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      5,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("rparam",BYVAL,CVariant::VT_INT),
      CArgType("user_A",BYVAL,CVariant::VT_STRING));

  // SetHuntGroupCalledRegisteredUser(CTask* task,RegisterParam* rparam,char* value1, char* value2);
  // karthi HG 07072016
  RegisterAPI(
      "RParamSetHuntGroupCalledRegisteredUser",                     // external name
      Id_RParamSetHuntGroupCalledRegisteredUser,                    // ordinal
      false,                                  // asynchronous flag
      CVariant::VT_VOID,                      // return type
      (CAPIProvider::APIPROC) &RegistrationApi::API_SetHuntGroupCalledRegisteredUser,
      CAPIDef::CDECL_TYPE,                    // calling convention
      5,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      //CArgType("rparam",BYVAL,CVariant::VT_INT),
      CArgType("handle",BYVAL,CVariant::VT_INT), // T HG_New
      CArgType("value1",BYVAL,CVariant::VT_STRING), // userA-huntgroupregistereduser
      CArgType("value2",BYVAL,CVariant::VT_INT));
  //CArgType("value3",BYVAL,CVariant::VT_STRING));

  // GetHuntGroupRegisteredUser_answered_call(CTask* task,char* retval,RegisterParam* rparam, char* user_A);
  RegisterAPI(
      "RParamGetHuntGroupRegisteredUser_answered_call",                   // external name
      Id_RParamGetHuntGroupRegisteredUser_answered_call,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetHuntGroupRegisteredUser_answered_call, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      5,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("rparam",BYVAL,CVariant::VT_INT),
      CArgType("user_A",BYVAL,CVariant::VT_STRING));

  // GetHuntGroupRegisteredUser_answered_call_sent(CTask* task,bool* retval,RegisterParam* rparam, char* user_A);
  RegisterAPI(
      "RParamGetHuntGroupRegisteredUser_answered_call_sent",                   // external name
      Id_RParamGetHuntGroupRegisteredUser_answered_call_sent,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetHuntGroupRegisteredUser_answered_call_sent, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      5,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("rparam",BYVAL,CVariant::VT_INT),
      CArgType("user_A",BYVAL,CVariant::VT_STRING));

  // GetCallLegOfRegisteredUser_answered_call(CTask* task,int* retval,RegisterParam* rparam, char* user_A);
  RegisterAPI(
      "RParamGetCallLegOfRegisteredUser_answered_call",                   // external name
      Id_RParamGetCallLegOfRegisteredUser_answered_call,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_INT,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetCallLegOfRegisteredUser_answered_call, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      5,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("rparam",BYVAL,CVariant::VT_INT),
      CArgType("user_A",BYVAL,CVariant::VT_STRING));

  RegisterAPI(
      "GetMyMediatrixIP",                   // external name
      Id_GetMyMediatrixIP,                  // ordinal
      false,                               // asynchronous flag
      CVariant::VT_STRING,                      // return type
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetMyMediatrixIP,
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("keyparam",BYVAL,CVariant::VT_STRING));

  RegisterAPI(
      "RemoveMyMediatrixMapEntry",                   // external name
      Id_RemoveMyMediatrixMapEntry,                  // ordinal
      false,                               // asynchronous flag
      CVariant::VT_VOID,                      // return type
      (CAPIProvider::APIPROC) &RegistrationApi::API_RemoveMyMediatrixMapEntry,
      CAPIDef::CDECL_TYPE,                    // calling convention
      2,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("keyparam",BYVAL,CVariant::VT_STRING));


  // API_GetCallingSignalAddress_answered_call(CTask* task, int* retval, long cparam, long answered_leg)

  RegisterAPI(
      "RParamGetCallingSignalAddress_answered_call",                   // external name
      Id_RParamGetCallingSignalAddress_answered_call,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetCallingSignalAddress_answered_call, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      4,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("legvalue",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "RParamSetHuntGroupCallingSignalAddress",                   // external name
      Id_SetHuntGroupCallingSignalAddress,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_VOID,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_SetHuntGroupCallingSignalAddress, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      4,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("csa",BYVAL,CVariant::VT_STRING));

  // T 24112017
  RegisterAPI(
      "RParamGetCalledSignalAddress_answered_call",                   // external name
      Id_RParamGetCalledSignalAddress_answered_call,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetCalledSignalAddress_answered_call, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      4,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("legvalue",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "RParamSetHuntGroupCalledSignalAddress",                   // external name
      Id_SetHuntGroupCalledSignalAddress,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_VOID,                      // return type 
      (CAPIProvider::APIPROC) &RegistrationApi::API_SetHuntGroupCalledSignalAddress, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      4,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("csa",BYVAL,CVariant::VT_STRING));
  //---------------------------------------------
  /*********************************
    RegisterAPI(
    "GetRealm",
    Id_GetToAddr,                      
    false,                                  
    CVariant::VT_INT,                       
    (CAPIProvider::APIPROC) &RegistrationApi::API_GetRealm,
    CAPIDef::CDECL_TYPE,                    
    3,                                      
    CArgType("value",BYVAL,CVariant::VT_STRING));

    RegisterAPI(
    "GetToken",
    Id_GetSipToken,                      
    false,                                  
    CVariant::VT_INT,                       
    (CAPIProvider::APIPROC) &RegistrationApi::API_GetToken,
    CAPIDef::CDECL_TYPE,                    
    3,                                      
    CArgType("value",BYREF,CVariant::VT_STRING));

    RegisterAPI(
    "GetContactAdd",
    Id_GetContactAddrUrl,                      
    false,                                  
    CVariant::VT_INT,                       
    (CAPIProvider::APIPROC) &RegistrationApi::API_GetContactAddr,
    CAPIDef::CDECL_TYPE,                    
    3,                                      
    CArgType("value",BYREF,CVariant::VT_STRING));

    RegisterAPI(
    "GetRegHostName",
    Id_GetRegHostName,                      
    false,                                  
    CVariant::VT_INT,                       
    (CAPIProvider::APIPROC) &RegistrationApi::API_GetRegHostName,
    CAPIDef::CDECL_TYPE,                    
    3,                                      
    CArgType("value",BYVAL,CVariant::VT_STRING));


    RegisterAPI(
    "GetUsrExpiration",
    Id_GetExpirationTime,                      
    false,                                  
    CVariant::VT_INT,                       
    (CAPIProvider::APIPROC) &RegistrationApi::API_GetUsrExpiration,
    CAPIDef::CDECL_TYPE,                    
    3,                                      
    CArgType("value",BYVAL,CVariant::VT_INT));

    RegisterAPI(
    "GetNetworkPort",
    Id_GetNetworkPort,                      
    false,                                  
    CVariant::VT_INT,                       
    (CAPIProvider::APIPROC) &RegistrationApi::API_GetNetworkPort,
    CAPIDef::CDECL_TYPE,                    
    3,                                      
    CArgType("value",BYREF,CVariant::VT_STRING));

    RegisterAPI(
    "GetNetworkProto",
    Id_GetNetworkProto,                      
    false,                                  
    CVariant::VT_INT,                       
    (CAPIProvider::APIPROC) &RegistrationApi::API_GetNetworkProto,
    CAPIDef::CDECL_TYPE,                    
    3,                                      
    CArgType("value",BYVAL,CVariant::VT_STRING));

  RegisterAPI(
      "GetNetworkIP",
      Id_GetNetworkIp,                      
      false,                                  
      CVariant::VT_INT,                       
      (CAPIProvider::APIPROC) &RegistrationApi::API_GetNetworkIP,
      CAPIDef::CDECL_TYPE,                    
      3,                                      
      CArgType("value",BYVAL,CVariant::VT_STRING));
  RegisterAPI(
      "PopulatePasswd",
      Id_GetPassword,                      
      false,                                  
      CVariant::VT_INT,                       
      (CAPIProvider::APIPROC) &RegistrationApi::API_PopulatePasswd,
      CAPIDef::CDECL_TYPE,                    
      3,                                      
      CArgType("value",BYVAL,CVariant::VT_STRING));

  RegisterAPI(
      "CheckStatus",
      Id_GetStatus,                      
      false,                                  
      CVariant::VT_INT,                       
      (CAPIProvider::APIPROC) &RegistrationApi::API_CheckStatus,
      CAPIDef::CDECL_TYPE,                    
      3,                                      
      CArgType("value",BYVAL,CVariant::VT_INT));
  /*RegisterAPI(
    "GetHasAuth",
    Id_GetHasAuth,                      
    false,                                  
    CVariant::VT_INT,                       
    (CAPIProvider::APIPROC) &RegistrationApi::API_GetHasAuth,
    CAPIDef::CDECL_TYPE,                    
    3,                                      
    CArgType("value",BYVAL,CVariant::VT_INT));*/
  /*************************************
    RegisterAPI(
    "PopulateAuthDetails",
    Id_GetAuthDetails,                      
    false,                                  
    CVariant::VT_INT,                       
    (CAPIProvider::APIPROC) &RegistrationApi::API_PopulateAuthDetails,
    CAPIDef::CDECL_TYPE,                    
    5,                                      
    CArgType("value",BYVAL,CVariant::VT_STRING),
    CArgType("value",BYVAL,CVariant::VT_STRING),
    CArgType("value",BYVAL,CVariant::VT_STRING),
    CArgType("value",BYVAL,CVariant::VT_STRING),
    CArgType("value",BYVAL,CVariant::VT_STRING));
  // For all the parameters of Register message 
  RegisterAPI(
  "GetRegisterParameters",
  Id_GetRegisterParameter,                      
  false,                                  
  CVariant::VT_INT,                       
  (CAPIProvider::APIPROC) &RegistrationApi::API_GetRegisterParameters,
  CAPIDef::CDECL_TYPE,                    
  10,                                      
  CArgType("value",BYREF,CVariant::VT_STRING),
  CArgType("value",BYREF,CVariant::VT_STRING),
  CArgType("value",BYREF,CVariant::VT_STRING),
  CArgType("value",BYREF,CVariant::VT_STRING),
  CArgType("value",BYREF,CVariant::VT_INT),
  CArgType("value",BYREF,CVariant::VT_STRING),
  CArgType("value",BYREF,CVariant::VT_INT),
  CArgType("value",BYREF,CVariant::VT_STRING),
  CArgType("value",BYREF,CVariant::VT_STRING),
  CArgType("value",BYREF,CVariant::VT_INT));
   ***************************/
}
#ifdef CALLINFO
struct callInfo
{
  char cli[32];
  char ddi_extn[MAX_HUNTGROUP_USERS_REGISTERED_TO_HUNTGROUP_NUMBER][16]; 
  char ddi_mob[MAX_HUNTGROUP_USERS_REGISTERED_TO_HUNTGROUP_NUMBER][32];  
  CallParameter *cparam;
};

static struct callInfo cInfo[MAX_CALL_INFO];
static int idx = 0;
#endif;
///RegistrationApi* _registrationApi; // rparam declared here
static RegistrationApi g_registrationApiObj;
RegistrationApi g_registrationApiObj_for_callTransfer;
//RegistrationApi g_registrationApiObj_for_HuntGroup;
RegistrationApi g_registrationApiObj_for_SwitchBoard;
static String global_passwd;
static String global_sipUri;

RegistrationApi::RegistrationApi(RegistrationApi* manager)
{
}

// Return a pointer to call parameter
RegistrationApi* RegistrationApi::GetRegistrationApiParam()
{
  SofiaRegistrar a;
  return a.GetRegistrationApiParam();
}

STATUS API_CALL RegistrationApi::API_GetSipUri(CTask* task, char* retval, RegisterParam* rparam)
{
  const char* temp_extension = "506";

  if (g_registrationApiObj.SipUri == NULL)
  {
    retval = "";
    return;
  }

  strncpy(retval,g_registrationApiObj.SipUri,strlen(g_registrationApiObj.SipUri));
  retval[strlen(g_registrationApiObj.SipUri)] = '\0';

  return OK;
}

STATUS API_CALL RegistrationApi::API_GetToAddr(CTask* task, char* retval, RegisterParam* rparam)
{

  if (g_registrationApiObj.ToAddr == NULL)
  {
    retval = "";
    return;
  }
  strncpy(retval,g_registrationApiObj.ToAddr,strlen(g_registrationApiObj.ToAddr));
  retval[strlen(g_registrationApiObj.ToAddr)] = '\0';

  return OK;
}

STATUS API_CALL RegistrationApi::API_GetToken(CTask* task, char* retval, RegisterParam* rparam)
{
  if (g_registrationApiObj.Token == NULL)
  {
    retval = "";
    return;
  }

  strncpy(retval,g_registrationApiObj.Token,strlen(g_registrationApiObj.Token));
  retval[strlen(g_registrationApiObj.Token)] = '\0';
  return OK;
}

STATUS API_CALL RegistrationApi::API_GetContactUser(CTask* task, char* retval, RegisterParam* rparam)
{
  if (g_registrationApiObj.ContactUser == NULL)
  {
    retval = "";
    return;
  }

  strncpy(retval,g_registrationApiObj.ContactUser,strlen(g_registrationApiObj.ContactUser));
  retval[strlen(g_registrationApiObj.ContactUser)] = '\0';
  return OK;
}

STATUS API_CALL RegistrationApi::API_GetNetworkPort(CTask* task, char* retval, RegisterParam* rparam)
{
  if (g_registrationApiObj.NetworkPort == NULL)
  {
    retval = "";
    return;
  }

  strncpy(retval,g_registrationApiObj.NetworkPort,strlen(g_registrationApiObj.NetworkPort));
  retval[strlen(g_registrationApiObj.NetworkPort)] = '\0';
  return OK;
}

STATUS API_CALL RegistrationApi::API_GetContactHost(CTask* task, char* retval, RegisterParam* rparam)
{
  if (g_registrationApiObj.ContactHost == NULL)
  {
    retval = "";
    return;
  }

  strncpy(retval,g_registrationApiObj.ContactHost,strlen(g_registrationApiObj.ContactHost));
  retval[strlen(g_registrationApiObj.ContactHost)] = '\0';
  return OK;
}

STATUS API_CALL RegistrationApi::API_GetExpireTime(CTask* task, unsigned int retval, RegisterParam* rparam)
{
  if (g_registrationApiObj.ExpireTime == NULL)
  {
    retval = 0; // this is an integer
    return;
  }

  retval = g_registrationApiObj.ExpireTime;
  return OK;
}

STATUS API_CALL RegistrationApi::API_GetNetworkProtocol(CTask* task, char* retval, RegisterParam* rparam)
{
  if (g_registrationApiObj.NetworkProtocol == NULL)
  {
    retval = "";
    return;
  }

  strncpy(retval,g_registrationApiObj.NetworkProtocol,strlen(g_registrationApiObj.NetworkProtocol));
  retval[strlen(g_registrationApiObj.NetworkProtocol)] = '\0';
  return OK;
}

STATUS API_CALL RegistrationApi::API_GetNetworkIP(CTask* task, char* retval, RegisterParam* rparam)
{
  if (g_registrationApiObj.NetworkIP == NULL)
  {
    retval = "";
    return;
  }

  strncpy(retval,g_registrationApiObj.NetworkIP,strlen(g_registrationApiObj.NetworkIP));
  retval[strlen(g_registrationApiObj.NetworkIP)] = '\0';
  return OK;
}

STATUS API_CALL RegistrationApi::API_GetHasAuth(CTask* task, bool* retval, RegisterParam* rparam)
{
  if (g_registrationApiObj.HasAuth == NULL)
  {
    *retval = false; // boolean
    return;
  }

  *retval = g_registrationApiObj.HasAuth;
  return OK;
}

// APIs for 7 output parameters
STATUS API_CALL RegistrationApi::API_PopulatePasswd(CTask* task,  RegisterParam* rparam, char* value)
{
  CLog::Detail(THISMODULE, "Entering RegistrationApi::API_PopulatePasswd");
  strcpy(g_registrationApiObj.Passwd.c_str(),value);
  strcpy(global_passwd.c_str(),value);

  CLog::Detail(THISMODULE, "RegistrationApi::API_PopulatePasswd password received from database %s\n",g_registrationApiObj.Passwd.c_str());

  if ( global_passwd != DEFAULT_PASSWD_SET_IN_SL)
  {
    PASSWORD_RECEIVED = true;
    CLog::Detail(THISMODULE, "RegistrationApi::API_PopulatePasswd password status=%d",PASSWORD_RECEIVED);
  }

  CLog::Detail(THISMODULE, "Exiting RegistrationApi::API_PopulatePasswd");
  return; // ******************* RETURN;;;
}

STATUS API_CALL RegistrationApi::API_PopulateStatus(CTask* task,  RegisterParam* rparam, bool value)
{
  CLog::Detail(THISMODULE, "Entering RegistrationApi::API_PopulateStatus");
  g_registrationApiObj.Status = value;
  CLog::Detail(THISMODULE, "Exiting RegistrationApi::API_PopulateStatus");
  return OK;
}

STATUS API_CALL RegistrationApi::API_PopulateAuthScheme(CTask* task,  RegisterParam* rparam, char* value)
{
  CLog::Detail(THISMODULE, "Entering RegistrationApi::API_PopulateAuthScheme");	
  g_registrationApiObj.AuthScheme = value;
  CLog::Detail(THISMODULE, "Eixting RegistrationApi::API_PopulateAuthScheme");
  return OK;
}

STATUS API_CALL RegistrationApi::API_PopulateRealm(CTask* task,  RegisterParam* rparam, char* value)
{
  CLog::Detail(THISMODULE, "Entering RegistrationApi::API_PopulateRealm");
  g_registrationApiObj.Realm = value;
  CLog::Detail(THISMODULE, "Exiting RegistrationApi::API_PopulateRealm");
  return OK;
}

STATUS API_CALL RegistrationApi::API_PopulateNounce(CTask* task,  RegisterParam* rparam, char* value)
{
  CLog::Detail(THISMODULE, "Entering RegistrationApi::API_PopulateNounce");
  g_registrationApiObj.Nounce = value;
  CLog::Detail(THISMODULE, "Exiting RegistrationApi::API_PopulateNounce");
  return OK;
}

STATUS API_CALL RegistrationApi::API_PopulateQop(CTask* task,  RegisterParam* rparam, char* value)
{
  CLog::Detail(THISMODULE, "Entering RegistrationApi::API_PopulateQop");
  g_registrationApiObj.Qop = value;
  CLog::Detail(THISMODULE, "Exiting RegistrationApi::API_PopulateQop");
  return OK;
}

STATUS API_CALL RegistrationApi::API_PopulateAlgorithm(CTask* task,  RegisterParam* rparam, char* value)
{
  CLog::Detail(THISMODULE, "Entering RegistrationApi::API_PopulateAlgorithm");
  g_registrationApiObj.Algorithm = value;
  CLog::Detail(THISMODULE, "Exiting RegistrationApi::API_PopulateAlgorithm");
  return OK;
}

STATUS API_CALL RegistrationApi::API_Delete_hg_data(CTask* task, long cparam)
{
  hg_data *h_data;


  if(cparam == 0)
  {
    return OK;
  }

  h_data = hg_info[cparam] ;


  if(h_data != NULL)
  {
    CLog::Detail(THISMODULE, "h_data not null, Freeing the Memory NoOfRegisteredUsers = %d \n", h_data->NoOfRegisteredUsers);
    //for (int i=0; i < h_data->NoOfRegisteredUsers ; i++)
    for (int i=0; i <= h_data->NoOfRegisteredUsers ; i++)
    {
      if (h_data->h_key[i] != NULL)
      {
        call_info.erase(h_data->h_key[i]);
        free(h_data->h_key[i]);
        h_data->h_key[i] = NULL; // T 17112017
      }
      if (h_data->HuntGroupRegisteredUser[i] != NULL)
      {
        free(h_data->HuntGroupRegisteredUser[i]);
        h_data->HuntGroupRegisteredUser[i] = NULL; // T 17112017
      }
      if (h_data->CallingSignalAddress_answered_call[i] != NULL) // T 17112017
      {
        free(h_data->CallingSignalAddress_answered_call[i]);
        h_data->CallingSignalAddress_answered_call[i] = NULL; // T 1907
      }
      if (h_data->CalledSignalAddress_answered_call[i] != NULL) // T 17112017
      {
        free(h_data->CalledSignalAddress_answered_call[i]);
        h_data->CalledSignalAddress_answered_call[i] = NULL; // T 1907
      }
    }

    CLog::Detail(THISMODULE, "\n Free h_data & delete hg_info[%d] = %02x \n", cparam, hg_info[cparam]);
    h_data->NoOfRegisteredUsers = 0; // T 06102016
    free(h_data);
    h_data = NULL; // T 17112017
    hg_info.erase(cparam);
  }

  return OK;
}

STATUS API_CALL RegistrationApi::API_SetConfHostUser(CTask* task,long cparam, char* host_number)

  //STATUS API_CALL RegistrationApi::API_SetConfHostUser(CTask* task, int cparam, char* host_number)//change for migraation from 32bit to 64 bit
{
  int i = 0;
  conf_host_data *conf_data = NULL;
  if ((cparam == 0) || (host_number == NULL))
  {
    return OK;
  }

  conf_data = conf_info[cparam];
  if (conf_data == NULL)
  {
    conf_data = (conf_host_data *)malloc(sizeof(conf_host_data));
    CLog::Detail(THISMODULE, "\n MEMORY alloc conf_data = %02x \n", conf_data);
    conf_data->NoOfDialInParticipants = 0;
    conf_data->dialIn_Disc_Leg = 0;
    memset(conf_data->ConfHostUser, 0, sizeof(conf_data->ConfHostUser));

    while(i < MAX_CONF_DIALIN)
    {
      conf_data->conf_di_data[i] = NULL;
      i++;
    }

    conf_info[cparam] = conf_data;
    CLog::Detail(THISMODULE, "\n MEMORY alloc conf_data = %02x \n", conf_data);
  } else {
    CLog::Detail(THISMODULE, "\n MEMORY ALREADY alloc conf_data = %02x \n", conf_data);
  }
  strcpy(conf_data->ConfHostUser, host_number);
  CLog::Detail(THISMODULE, "\n conf_data->ConfHostUser = %s \n", conf_data->ConfHostUser);
  return OK;
}


//STATUS API_CALL RegistrationApi::API_DialIn_Participant_RegisteredUser(CTask* task, int cparam, int handle, char* DialInParticipantUser, int max_dialin_count, char* user_A)

//STATUS API_CALL RegistrationApi::API_DialIn_Participant_RegisteredUser(CTask* task, long cparam, int  handle, char* DialInParticipantUser, int max_dialin_count, char* user_A)//changes for migration from 32 bit to 64 bit
STATUS API_CALL RegistrationApi::API_DialIn_Participant_RegisteredUser(CTask* task, long cparam, int  handle, char* DialInParticipantUser, int max_dialin_count)//changes for migration from 32 bit to 64 bit
{
  char cf_key[24];
  char* user_A;
  char * pch;
  int i = 0;
  pch = strtok (DialInParticipantUser,"-");
  while (pch != NULL)
  {
    if (i == 0) {
      user_A = pch;
    }
    if (i == 1) {
      DialInParticipantUser = pch;
    }
    i = i + 1;
    pch = strtok (NULL, "-");
  }

  conf_host_data *c_data = NULL;
  if ((cparam == 0) || (DialInParticipantUser == NULL) || (user_A == NULL))
  {
    return OK;
  }

  c_data = conf_info[cparam];
  if (c_data == NULL)
  {
    return OK;
  }
  CLog::Detail(THISMODULE, "\n conf_data cparam = %02x \n", cparam);

  if (c_data->NoOfDialInParticipants > max_dialin_count)
  {
    c_data->NoOfDialInParticipants = 0;
  }

  if (strlen(DialInParticipantUser) == 3)
  {
    sprintf(cf_key,"%s%s", user_A, DialInParticipantUser);
    call_info[cf_key] = cparam;
    CLog::Warning(THISMODULE, "***********IPPBX : API_DialIn_Participant_RegisteredUser() -- EXTENSION DialIn key = %s, call_info[cf_key] = %02x \n", cf_key, call_info[cf_key]);
  } else {
    sprintf(cf_key,"%s", DialInParticipantUser);
    call_info[cf_key] = cparam;
    CLog::Warning(THISMODULE, "***********IPPBX : API_DialIn_Participant_RegisteredUser() -- MOBILE DialIn key = %s, call_info[cf_key] = %02x \n", cf_key, call_info[cf_key]);
  }

  if (handle <= max_dialin_count)
  {
    if (c_data->conf_di_data[handle]== NULL)
    {
      c_data->conf_di_data[handle]= (DialIn_Data *) malloc(sizeof(DialIn_Data));
      CLog::Detail(THISMODULE, "\n MEMORY alloc c_data->conf_di_data[%d] = %02x \n", handle,c_data->conf_di_data[handle]);
      memset(c_data->conf_di_data[handle]->DialIn_Participant_RegisteredUser, 0, sizeof(c_data->conf_di_data[handle]->DialIn_Participant_RegisteredUser));
      strcpy(c_data->conf_di_data[handle]->DialIn_Participant_RegisteredUser,DialInParticipantUser);

      memset(c_data->conf_di_data[handle]->c_key, 0, sizeof(c_data->conf_di_data[handle]->c_key));
      strcpy(c_data->conf_di_data[handle]->c_key,cf_key);
    }

  }

  if (c_data->NoOfDialInParticipants > MAX_CONF_DIALIN) // here is max conf users allowed in SL
  {
    return OK;
  }
  c_data->NoOfDialInParticipants++;
  return OK;
}


STATUS API_CALL RegistrationApi::API_Delete_conf_host_data(CTask* task,long cparam)//for migration 32 bit machine to 64 bit machine

  //STATUS API_CALL RegistrationApi::API_Delete_conf_host_data(CTask* task, int cparam)
{
  conf_host_data *c_data = NULL;
  if (cparam == 0)
  {
    return OK;
  }
  c_data = conf_info[cparam];

  if (c_data != NULL )
  {
    for (int i=0; i <= c_data->NoOfDialInParticipants ; i++)
    {
      if (c_data->conf_di_data[i] != NULL)
      {
        call_info.erase(c_data->conf_di_data[i]->c_key);
        CLog::Detail(THISMODULE, "\n MEMORY free c_data->conf_di_data[%d] = %02x \n", i, c_data->conf_di_data[i]);
        c_data->conf_di_data[i] = NULL;
        free(c_data->conf_di_data[i]);
      }
    }
    c_data->NoOfDialInParticipants = 0;
    CLog::Detail(THISMODULE, "\n MEMORY free c_data = %02x \n", c_data);
    free(c_data);
    conf_info.erase(cparam);
  }

  return OK;
}

STATUS API_CALL RegistrationApi::API_Clear_Dialin_data(CTask* task, long cparam, long handle)
  //STATUS API_CALL RegistrationApi::API_Clear_Dialin_data(CTask* task, int cparam, int handle)  //change for migration 32 bit to 64 bit
{
  conf_host_data *c_data = NULL;
  if (cparam == 0)
  {
    return OK;
  }

  c_data = conf_info[cparam];
  if (c_data != NULL )
  {
    if (c_data->conf_di_data[handle]!= NULL)
    {
      CLog::Detail(THISMODULE, "\n MEMORY free c_data->conf_di_data[%d]= %02x \n", handle, c_data->conf_di_data[handle]);
      call_info.erase(c_data->conf_di_data[handle]->c_key);
      c_data->conf_di_data[handle] = NULL;
      free(c_data->conf_di_data[handle]);
    }
  }
  return OK;
}
STATUS API_CALL RegistrationApi::GetDialInDiscLeg(CTask *task, long *retval, long cparam)//changes for migration from 32bit to 64 bit
//STATUS API_CALL RegistrationApi::GetDialInDiscLeg(CTask *task, int *retval, int cparam)
{
  conf_host_data *c_data = NULL;
  if (cparam == 0)
  {
    return OK;
  }

  c_data = conf_info[cparam];
  CLog::Detail(THISMODULE, "GetDialInDiscLeg c_data = %02x \n", c_data);
  if (c_data != NULL )
  {
    CLog::Detail(THISMODULE, "GetDialInDiscLeg returns %d \n",c_data->dialIn_Disc_Leg);
    *retval = c_data->dialIn_Disc_Leg;
  } else {
    CLog::Detail(THISMODULE, "GetDialInDiscLeg returns NULL");
    *retval = 0;
  }
}

STATUS API_CALL RegistrationApi::API_DelSuspendUsr(CTask* task, char *key)
{
  CLog::Detail(THISMODULE, "API_DelSuspendUsr key = %s", key);

  for(int idx= 0; idx < usr_key.size(); idx++)
  {
    if(key == usr_key[idx])
    {
      usr_key.erase(usr_key.begin()+idx);
      break;
    }
  }

  return OK;
}

STATUS API_CALL RegistrationApi::API_GetSuspendUsr(CTask* task, char *retval, char *key)
{
  int idx = 0;
  char key_t[24];

  CLog::Detail(THISMODULE, "API_GetSuspendUsr key = %s", key);

  printf("&&&&&&&&&&&&& API_GetSuspendUsr key = %s \n", key);
  memset(&key_t, 0, sizeof(key_t));

  for(idx= 0; idx < usr_key.size(); idx++)
  {
    printf("&&&&&&&&&&&&& Inside 1 For API_GetSuspendUsr key = %s -- %s\n", key, usr_key[idx].c_str());
    if(key == usr_key[idx])
    {
      for(int i=0; key[i] != '_' ;i++)
      {
        if (key[i] == '_') break;
        key_t[i] = key[i];
        printf("API_GetSuspendUsr key_t[%d] = %c \n", i, key_t[i]);
      }

      key_t[i] = '\0';

      strcpy(retval , key_t);

      CLog::Detail(THISMODULE, "****API_GetSuspendUsr key = %s", retval);
      break;
    }
  }

  return OK;
}

STATUS API_CALL RegistrationApi::API_UpdateMap(CTask* task, char *inputkey, char *value, int update_key)
{
  if (update_key == 1) // Set Map Entries
  {
    if (call_transfer_extn.find(inputkey) == call_transfer_extn.end())
    {
      call_transfer_extn[inputkey] = value;
    }
  }
  else if (update_key == 2) // Clear Map Entries
  {
    call_transfer_extn.erase(inputkey);
  }
  return OK;
}

STATUS API_CALL RegistrationApi::API_UpdateCallParkMap(CTask* task, char *inputkey, char *value, int update_key)
{
  if (update_key == 1) // Set Map Entries
  {
    if (call_park_code.find(inputkey) == call_park_code.end())
    {
      call_park_code[inputkey] = value;
    }
  }
  else if (update_key == 2) // Clear Map Entries
  {
    call_park_code.erase(inputkey);
  }
  return OK;
}

STATUS API_CALL RegistrationApi::API_GetCallParkId(CTask* task, int *retval)
{
  char r_buff[10];
  int rand_t = 0;
  int r_len = 0;
  int j = 0;
  int r_pow = 1;

  rand_t = rand();
  printf("rand_t %d \n", rand_t);
  memset(&r_buff, 0, sizeof(r_buff));
  sprintf(r_buff, "%d", rand_t);

  r_len = strlen(r_buff);

  for(j=0; j < (r_len-4); j++)
  {
    r_pow = r_pow * 10;
  }
  printf("retval = %d\n",*retval);
  *retval= rand_t/r_pow ;

  return OK;

}
#if 0
STATUS API_CALL RegistrationApi::API_GetCallTransferReplaces(CTask* task, char* retval, char * key)
{
  string transfer_replaces;
  transfer_replaces = call_transfer_replace[key];
  memset(retval,0,strlen(retval));
  if(transfer_replaces != ""){
    strcpy(retval,transfer_replaces.c_str());
    call_transfer_replace.erase(key);
    return OK;
  }else{
    retval = "";
    return ;
  }

}
#endif

STATUS API_CALL RegistrationApi::API_GetCallParkCode(CTask* task, char* retval, char * bleg)
{
  CLog::Detail(THISMODULE, "Entering RegistrationApi::API_GetCallParkCode");
  string call_park_st;
  call_park_st = call_park_code[bleg];
  memset(retval,0,strlen(retval));

  if(call_park_st != ""){
    strcpy(retval,call_park_st.c_str());
    return OK;
  }else{
    retval = "";
    return ;
  }

}

#ifndef CALL_TRANSFER
STATUS API_CALL RegistrationApi::API_GetCallTransferExtension(CTask* task, char* retval, RegisterParam* rparam)
#else
STATUS API_CALL RegistrationApi::API_GetCallTransferExtension(CTask* task, char* retval, char * bleg)
#endif
{
#ifndef CALL_TRANSFER
  const char* temp_extension = "506";

  CLog::Detail(THISMODULE, "Entering RegistrationApi::API_GetCallTransferExtension");
  if (g_registrationApiObj_for_callTransfer.CallTransferExtension == NULL)
  {
    CLog::Warning(THISMODULE, "g_registrationApiObj_for_callTransfer.CallTransferExtension is null");
    retval = "";
    return;
  }

  strncpy(retval,g_registrationApiObj_for_callTransfer.CallTransferExtension,strlen(g_registrationApiObj_for_callTransfer.CallTransferExtension));
  retval[strlen(g_registrationApiObj_for_callTransfer.CallTransferExtension)] = '\0';
  CLog::Detail(THISMODULE, "retval = %s", retval);

  g_registrationApiObj_for_callTransfer.CallTransferExtension = "";
  return OK;
#else
  string transfer_extn;
  transfer_extn = call_transfer_extn[bleg];
  memset(retval,0,strlen(retval));

  if(transfer_extn != ""){
    strcpy(retval,transfer_extn.c_str());
    return OK;
  }else{
    retval = "";
    return ;
  }
#endif
}

STATUS API_CALL RegistrationApi::API_GetSwitchBoardExtension(CTask* task, char* retval, RegisterParam* rparam)
{
  CLog::Detail(THISMODULE, "Entering API_GetSwitchBoardExtension");

  if (g_registrationApiObj_for_SwitchBoard.SwitchBoardExtension == NULL)
  {
    retval = "";
    return;
  }

  strncpy(retval,g_registrationApiObj_for_SwitchBoard.SwitchBoardExtension,strlen(g_registrationApiObj_for_SwitchBoard.SwitchBoardExtension));
  retval[strlen(g_registrationApiObj_for_SwitchBoard.SwitchBoardExtension)] = '\0';
  CLog::Detail(THISMODULE, "retval = %s", retval);
  CLog::Detail(THISMODULE, "Exiting API_GetSwitchBoardExtension");
  return OK;
}

STATUS API_CALL RegistrationApi::API_GetCallTransferExtensionSent(CTask* task, bool* retval, RegisterParam* rparam)
{
  CLog::Detail(THISMODULE, "Entering RegistrationApi::API_GetCallTransferExtensionSent");


  *retval = g_registrationApiObj_for_callTransfer.CallTransferExtensionSent;
  CLog::Detail(THISMODULE, "*retval = %d", *retval);
  g_registrationApiObj_for_callTransfer.CallTransferExtensionSent = false;
  CLog::Detail(THISMODULE, "Exiting RegistrationApi::API_GetCallTransferExtensionSent");
  return OK;
}

STATUS API_CALL RegistrationApi::API_GetSwitchBoard200_OK_received(CTask* task, bool* retval, CallParameter* cparam)
{
  CLog::Detail(THISMODULE, "Entering RegistrationApi::API_GetSwitchBoard200_OK_received");
  Call *call = CallManager::Instance()->GetCurrentIncomingCall();

  if (!call)
  {
    CLog::Warning(THISMODULE, "call parameter is NULL:: exiting");
  }

  if (cparam && (cparam->_isSwitchBoardCall))
  {
    g_registrationApiObj_for_SwitchBoard.SwitchBoard_Number = cparam->cnCalling;
    g_registrationApiObj_for_SwitchBoard.SwitchBoard_called_extension = cparam->cnCalled;
    g_registrationApiObj_for_SwitchBoard.is_SwitchBoard_Call = true;

    *retval = g_registrationApiObj_for_SwitchBoard.SwitchBoard200_OK_received;
    CLog::Detail(THISMODULE, "API GetSwitchBoard200_OK_received() :  SwitchBoard200_OK_received  == %d ",*retval);
    g_registrationApiObj_for_SwitchBoard.SwitchBoard200_OK_received = false;
  }
  else
  {
    // else reset the values.
    g_registrationApiObj_for_SwitchBoard.SwitchBoard200_OK_received = false;
    g_registrationApiObj_for_SwitchBoard.SwitchBoard_Number = "";
    g_registrationApiObj_for_SwitchBoard.SwitchBoard_called_extension = "";
    g_registrationApiObj_for_SwitchBoard.is_SwitchBoard_Call = false;
  }
  switchboard_index = 0;
  switchboard_dtmf_digits[switchboard_index] = '\0';
  g_registrationApiObj_for_SwitchBoard.SwitchBoardExtension = "";
  return OK;
}

// Hunt-Group APIs
//STATUS API_CALL RegistrationApi::API_SetHuntGroupNumberAndCaller(CTask* task, CallParameter *cparam, RegisterParam* rparam, char* hunt_group_number, char* user_A)
STATUS API_CALL RegistrationApi::API_SetHuntGroupNumberAndCaller(CTask* task, int cparam, RegisterParam* rparam, char* hunt_group_number, char* user_A)
{
  int i = 0;
  hg_data *h_data;

  h_data = hg_info[cparam];

  CLog::Detail(THISMODULE, "INSIDE API_SetHuntGroupNumberAndCaller \n");

  if(h_data == NULL)
  {
    h_data = (hg_data *)malloc(sizeof(hg_data));
    h_data->NoOfRegisteredUsers = 0;

    memset(h_data->HuntGroupNumber,0,sizeof(h_data->HuntGroupNumber));
    memset(h_data->HuntGroupCaller, 0, sizeof(h_data->HuntGroupCaller));
    memset(h_data->HuntGroupRegisteredUser_answered_call, 0, sizeof(h_data->HuntGroupRegisteredUser_answered_call));

    while(i < 100)
    {
      h_data->HuntGroupRegisteredUser[i] = NULL;
      h_data->CallingSignalAddress_answered_call[i] = NULL;
      h_data->CalledSignalAddress_answered_call[i] = NULL;
      h_data->h_key[i] = NULL;
      i++;
    }

    hg_info[cparam] = h_data;
    CLog::Warning(THISMODULE, "***********IPPBX : inside API_SetHuntGroupNumberAndCaller() -- h_data =%02x\n", h_data);
  }

  CLog::Warning(THISMODULE, "***********IPPBX : inside API_SetHuntGroupNumberAndCaller() -- cparam =%02x\n", cparam);

  strcpy(h_data->HuntGroupNumber,hunt_group_number);
  strcpy(h_data->HuntGroupCaller,user_A);

  CLog::Warning(THISMODULE,"\n h_data->HuntGroupNumber = %s\n", h_data->HuntGroupNumber);
  CLog::Warning(THISMODULE,"\n h_data->HuntGroupCaller = %s\n", h_data->HuntGroupCaller);

  return OK;
}

//STATUS API_CALL RegistrationApi::API_GetHuntGroupNumber(CTask* task, char* retval, CallParameter *cparam, RegisterParam* rparam, char* user_A)
STATUS API_CALL RegistrationApi::API_GetHuntGroupNumber(CTask* task, char* retval, int cparam, RegisterParam* rparam, char* user_A)
{
  hg_data *h_data;

  CLog::Warning(THISMODULE,"\n Inside API_GetHuntGroupNumber \n");

  h_data = hg_info[cparam];

  if(h_data == NULL)
  {
    return OK;
  }

  CLog::Warning(THISMODULE, "***********IPPBX : API_GetHuntGroupNumber() -- cparam = %02x -- %d\n", cparam, rparam);

  if (h_data->HuntGroupNumber == NULL)
  {
    retval = "";
    return;
  }
  CLog::Warning(THISMODULE,"\n h_data- = %02x \n" , h_data);
  CLog::Warning(THISMODULE,"\n h_data->HuntGroupNumber = %s \n", h_data->HuntGroupNumber);

  strncpy(retval,h_data->HuntGroupNumber,strlen(h_data->HuntGroupNumber));
  retval[strlen(h_data->HuntGroupNumber)] = '\0';

  return OK;
}

#if 0
//STATUS API_CALL RegistrationApi::API_SetHuntGroupCalledRegisteredUser(CTask* task, CallParameter *cparam, RegisterParam* rparam, char* hunt_group_registered_user, int no_of_registered_hg_users, char* user_A)
//STATUS API_CALL RegistrationApi::API_SetHuntGroupCalledRegisteredUser(CTask* task, int cparam, RegisterParam* rparam, char* hunt_group_registered_user, int no_of_registered_hg_users, char* user_A)

STATUS API_CALL RegistrationApi::API_SetHuntGroupCalledRegisteredUser(CTask* task, long  cparam, RegisterParam* rparam, char* hunt_group_registered_user, int no_of_registered_hg_users, char* user_A)//changes made for migration from 32 bit to 64 bit

{
  char key[24];
  hg_data *h_data;

  CLog::Warning(THISMODULE, "\n Inside API_SetHuntGroupCalledRegisteredUser\n");

  h_data = hg_info[cparam];

  if(h_data == NULL)
  {
    return OK;
  }

  CLog::Warning(THISMODULE, "***********IPPBX : API_SetHuntGroupCalledRegisteredUser() -- cparam = %02x\n", cparam);
  CLog::Warning(THISMODULE, "\n h_data- = %02x \n" , h_data);

  //        if(cparam->NoOfRegisteredUsers > no_of_registered_hg_users)cparam->NoOfRegisteredUsers = 0;
  if(h_data->NoOfRegisteredUsers > no_of_registered_hg_users)h_data->NoOfRegisteredUsers = 0;

  if (strlen(hunt_group_registered_user)==3) {
    sprintf(key, "%s%s",user_A,hunt_group_registered_user);
    call_info[key] = cparam;
    CLog::Warning(THISMODULE, "***********IPPBX : API_SetHuntGroupCalledRegisteredUser() -- EXTENSION key = %s, call_info[key] = %02x \n", key, call_info[key]);
  } else {
    sprintf(key, "%s",hunt_group_registered_user);
    call_info[key] = cparam;
    CLog::Warning(THISMODULE, "***********IPPBX : API_SetHuntGroupCalledRegisteredUser() -- MOBILE key = %s, call_info[key] = %02x \n", key, call_info[key]);
  }

  h_data->HuntGroupRegisteredUser[h_data->NoOfRegisteredUsers] = (char *)malloc(24);
  h_data->h_key[h_data->NoOfRegisteredUsers] = (char *)malloc(24);

  if (h_data->HuntGroupRegisteredUser[h_data->NoOfRegisteredUsers]!=NULL){
    strcpy(h_data->HuntGroupRegisteredUser[h_data->NoOfRegisteredUsers],hunt_group_registered_user);
  }

  CLog::Warning(THISMODULE, "\n h_data->HuntGroupRegisteredUser[%d] = %s\n", h_data->NoOfRegisteredUsers, h_data->HuntGroupRegisteredUser[h_data->NoOfRegisteredUsers]);

  if (h_data->h_key[h_data->NoOfRegisteredUsers]!=NULL){
    strcpy(h_data->h_key[h_data->NoOfRegisteredUsers],key);
  }

  if (h_data->NoOfRegisteredUsers >= MAX_HUNTGROUP_USERS_REGISTERED_TO_HUNTGROUP_NUMBER)
  {
    return OK;
  }

  if(h_data->NoOfRegisteredUsers == (no_of_registered_hg_users-1))
  {
    for (int i = 0;i<no_of_registered_hg_users;i++)
    {
      CLog::Warning(THISMODULE, "\n  h_data->HuntGroupRegisteredUser[%d] = %s \n", i, h_data->HuntGroupRegisteredUser[i]);
    }
  }

  // increment no. of Registerd users for this HuntGroup
  h_data->NoOfRegisteredUsers++;

  return OK;
}
#endif

#if 1 // T HG_New
//STATUS API_CALL RegistrationApi::API_SetHuntGroupCalledRegisteredUser(CTask* task, int cparam, int handle, char* hunt_group_registered_user, int no_of_registered_hg_users, char* user_A)

STATUS API_CALL RegistrationApi::API_SetHuntGroupCalledRegisteredUser(CTask* task, long cparam, long handle, char* hunt_group_registered_user, int no_of_registered_hg_users )//changes made for migration from 32 bit to 64 bit 
{
  char key[24];
  hg_data *h_data;
  char* user_A;
  char * pch;
  int i = 0;
  pch = strtok (hunt_group_registered_user,"-");
  while (pch != NULL)
  {
    if (i == 0) {
      user_A = pch;
    }
    if (i == 1) {
      hunt_group_registered_user = pch;
    }
    i = i + 1;
    pch = strtok (NULL, "-");
  }

  h_data = hg_info[cparam];
  if(h_data == NULL)
  {
    return OK;
  }

  CLog::Detail(THISMODULE, "***********IPPBX : UPDATED API_SetHuntGroupCalledRegisteredUser() -- cparam = %02x; user_A = %s \n", cparam, user_A);
  //        if(h_data->NoOfRegisteredUsers > no_of_registered_hg_users)h_data->NoOfRegisteredUsers = 0;

  //if (strlen(hunt_group_registered_user)==3) {
  if (strlen(hunt_group_registered_user) != 24) {
    sprintf(key, "%s%s",user_A,hunt_group_registered_user);
    call_info[key] = cparam;
    CLog::Detail(THISMODULE, "***********IPPBX : API_SetHuntGroupCalledRegisteredUser() -- EXTENSION key = %s, call_info[key] = %02x \n", key, call_info[key]);
  } else {
    sprintf(key, "%s",hunt_group_registered_user);
    call_info[key] = cparam;
    CLog::Detail(THISMODULE, "***********IPPBX : API_SetHuntGroupCalledRegisteredUser() -- MOBILE key = %s, call_info[key] = %02x \n", key, call_info[key]);
  }

  if (handle <= no_of_registered_hg_users) {
    if (h_data->HuntGroupRegisteredUser[handle] == NULL) {
      h_data->HuntGroupRegisteredUser[handle] = (char *)malloc(32);
    }
    if (h_data->h_key[handle] == NULL) {
      h_data->h_key[handle] = (char *)malloc(32);
    }
  }

  if (h_data->HuntGroupRegisteredUser[handle]!=NULL){

    strcpy(h_data->HuntGroupRegisteredUser[handle],hunt_group_registered_user);
    CLog::Warning(THISMODULE, "h_data->HuntGroupRegisteredUser[%d] = %s", handle, h_data->HuntGroupRegisteredUser[handle]);
  }

  if (h_data->h_key[handle]!=NULL){
    strcpy(h_data->h_key[handle],key);
    CLog::Warning(THISMODULE, "h_data->h_key[%d] = %s ", handle, h_data->h_key[handle]);
  }

  if (h_data->NoOfRegisteredUsers >= MAX_HUNTGROUP_USERS_REGISTERED_TO_HUNTGROUP_NUMBER)
  {
    return OK;
  }
  // increment no. of Registerd users for this HuntGroup
  h_data->NoOfRegisteredUsers++;
  CLog::Detail(THISMODULE, "h_data->NoOfRegisteredUsers = %d \n", h_data->NoOfRegisteredUsers);
  if(h_data->NoOfRegisteredUsers == (no_of_registered_hg_users))
  {
    for (int j = 1; j<=h_data->NoOfRegisteredUsers;j++) {
      CLog::Detail(THISMODULE, "h_data->HuntGroupRegisteredUser[%d] = %s", j, h_data->HuntGroupRegisteredUser[j]);
    }
  }


  return OK;

}
#endif

STATUS API_CALL RegistrationApi::API_SetHuntGroupCallingSignalAddress(CTask* task, long cparam, int handle, char* callingsignaladdress)
{
  hg_data *h_data;
  if(cparam == NULL)
  {
    return OK;
  }

  h_data = hg_info[cparam];

  if(h_data == NULL)
  {
    return OK;
  }

  CLog::Warning(THISMODULE, "***********IPPBX : API_GetHuntGroupRegisteredUser_answered_call_sent() -- cparam = %02x\n", cparam);

  if (h_data->CallingSignalAddress_answered_call[handle] == NULL) {
    h_data->CallingSignalAddress_answered_call[handle] = (char *)malloc(64);
  }
  if (h_data->CallingSignalAddress_answered_call[handle] != NULL)
  {
    strcpy(h_data->CallingSignalAddress_answered_call[handle],callingsignaladdress);
    CLog::Detail(THISMODULE, "h_data->CallingSignalAddress_answered_call[%d] = %s", handle, h_data->CallingSignalAddress_answered_call[handle]);
  }

  return OK;

}

STATUS API_CALL RegistrationApi::API_SetHuntGroupCalledSignalAddress(CTask* task, long cparam, int handle, char* calledsignaladdress)
{
  hg_data *h_data;
  if(cparam == NULL)
  {
    return OK;
  }

  h_data = hg_info[cparam];

  if(h_data == NULL)
  {
    return OK;
  }

  CLog::Warning(THISMODULE, "***********IPPBX : API_SetHuntGroupCalledSignalAddress () -- cparam = %02x\n", cparam);

  if (h_data->CalledSignalAddress_answered_call[handle] == NULL) {
    h_data->CalledSignalAddress_answered_call[handle] = (char *)malloc(64);
  }
  if (h_data->CalledSignalAddress_answered_call[handle] != NULL)
  {
    strcpy(h_data->CalledSignalAddress_answered_call[handle],calledsignaladdress);
    CLog::Detail(THISMODULE, "h_data->CalledSignalAddress_answered_call[%d] = %s", handle, h_data->CalledSignalAddress_answered_call[handle]);
  }

  return OK;

}

//STATUS API_CALL RegistrationApi::API_GetHuntGroupRegisteredUser_answered_call_sent(CTask* task, bool* retval, CallParameter *cparam, RegisterParam* rparam, char* user_A)
STATUS API_CALL RegistrationApi::API_GetHuntGroupRegisteredUser_answered_call_sent(CTask* task, bool* retval, int cparam, RegisterParam* rparam, char* user_A)
{
  hg_data *h_data;

  if(cparam == NULL)
  {
    return OK;
  }

  h_data = hg_info[cparam];

  if(h_data == NULL)
  {
    return OK;
  }

  CLog::Warning(THISMODULE, "***********IPPBX : API_GetHuntGroupRegisteredUser_answered_call_sent() -- cparam = %02x\n", cparam);

  *retval = h_data->HuntGroupRegisteredUser_answered_call_sent;

  return OK;
}

//STATUS API_CALL RegistrationApi::API_GetHuntGroupRegisteredUser_answered_call(CTask* task, char* retval, CallParameter *cparam, RegisterParam* rparam, char* user_A)
STATUS API_CALL RegistrationApi::API_GetHuntGroupRegisteredUser_answered_call(CTask* task, char* retval, int cparam, RegisterParam* rparam, char* user_A)
{
  hg_data *h_data;

  if(cparam == NULL)
  {
    CLog::Warning(THISMODULE," \n CCCCCCCCCCCCCParam NULL \n");
    return OK;
  }

  h_data = hg_info[cparam];

  if(h_data == NULL)
  {
    CLog::Warning(THISMODULE," \n HHHHHHHHHHHHHHHHHH data NULL \n");
    return OK;
  }

  CLog::Warning(THISMODULE, "***********IPPBX : API_GetHuntGroupRegisteredUser_answered_call() -- cparam = %02x\n", cparam);
  CLog::Warning(THISMODULE, "***********IPPBX : API_GetHuntGroupRegisteredUser_answered_call() -- HuntGroupRegisteredUser_answered_call = %s\n", h_data->HuntGroupRegisteredUser_answered_call);

  if (h_data->HuntGroupRegisteredUser_answered_call == NULL)
  {
    CLog::Warning(THISMODULE, "\n h_data->HuntGroupRegisteredUser_answered_call == NULL \n");
    retval = "";
    return;
  }

  strncpy(retval,h_data->HuntGroupRegisteredUser_answered_call,strlen(h_data->HuntGroupRegisteredUser_answered_call));
  retval[strlen(h_data->HuntGroupRegisteredUser_answered_call)] = '\0';

  // *** CLEAR global list of active HuntGroup calls for user_A -as one registered user has answered call...

  //h_data->NoOfRegisteredUsers = 0; // T 16062016 For Memory Leak

  h_data->HuntGroupRegisteredUser_answered_call_sent = false;

  return OK;
}

//STATUS API_CALL RegistrationApi::API_GetCallLegOfRegisteredUser_answered_call(CTask* task, int* retval, CallParameter *cparam, RegisterParam* rparam, char* user_A)
STATUS API_CALL RegistrationApi::API_GetCallLegOfRegisteredUser_answered_call(CTask* task, int* retval, long cparam, RegisterParam* rparam, char* user_A)
{
  hg_data *h_data;

  if(cparam == NULL)
  {
    CLog::Warning(THISMODULE, "API_GetCallLegOfRegisteredUser_answered_call() cparam == NULL \n");
    return OK;
  }

  h_data = hg_info[cparam];

  if(h_data == NULL)
  {
    CLog::Warning(THISMODULE, "API_GetCallLegOfRegisteredUser_answered_call() h_data == NULL \n ");
    return OK;
  }

  CLog::Warning(THISMODULE, "***********IPPBX : API_GetCallLegOfRegisteredUser_answered_call() -- cparam = %02x\n", cparam);

  if (h_data->CallLegOfRegisteredUser_answered_call == NULL)
  {
    CLog::Warning(THISMODULE, "API_GetCallLegOfRegisteredUser_answered_call() h_data->CallLegOfRegisteredUser_answered_call == NULL \n ");
    *retval = 0;
    return;
  }

  *retval = h_data->CallLegOfRegisteredUser_answered_call;
  CLog::Detail(THISMODULE, "API_GetCallLegOfRegisteredUser_answered_call h_data->CallLegOfRegisteredUser_answered_call %d \n", h_data->CallLegOfRegisteredUser_answered_call);

  // reset CallLegOfRegisteredUser_answered_call to 0.
  h_data->CallLegOfRegisteredUser_answered_call = 0;

  return OK;
}

STATUS API_CALL RegistrationApi::API_GetCallingSignalAddress_answered_call(CTask* task, char* retval, long cparam, int answered_leg)
{
  hg_data *h_data;

  if(cparam == NULL)
  {
    CLog::Warning(THISMODULE, "API_GetCallingSignalAddress_answered_call() cparam == NULL \n");
    return OK;
  }

  h_data = hg_info[cparam];

  if(h_data == NULL)
  {
    CLog::Warning(THISMODULE, "API_GetCallingSignalAddress_answered_call() h_data == NULL \n ");
    return OK;
  }

  CLog::Warning(THISMODULE, "***********IPPBX : API_GetCallingSignalAddress_answered_call() -- cparam = %02x\n", cparam);

  if (h_data->CallingSignalAddress_answered_call[answered_leg] == NULL)
  {
    CLog::Warning(THISMODULE, "API_GetCallingSignalAddress_answered_call() h_data->CallingSignalAddress_answered_call == NULL \n ");
    retval = "";
    return;
  }
  strcpy(retval,h_data->CallingSignalAddress_answered_call[answered_leg]);

  CLog::Detail(THISMODULE, "API_GetCallingSignalAddress_answered_call h_data->CallingSignalAddress_answered_call %s \n", h_data->CallingSignalAddress_answered_call[answered_leg]);

  return OK;
}

STATUS API_CALL RegistrationApi::API_GetCalledSignalAddress_answered_call(CTask* task, char* retval, long cparam, int answered_leg)
{
  hg_data *h_data;

  if(cparam == NULL)
  {
    CLog::Warning(THISMODULE, "API_GetCalledSignalAddress_answered_call() cparam == NULL \n");
    return OK;
  }

  h_data = hg_info[cparam];

  if(h_data == NULL)
  {
    CLog::Warning(THISMODULE, "API_GetCalledSignalAddress_answered_call() h_data == NULL \n ");
    return OK;
  }

  CLog::Warning(THISMODULE, "***********IPPBX : API_GetCalledSignalAddress_answered_call() -- cparam = %02x\n", cparam);

  if (h_data->CalledSignalAddress_answered_call[answered_leg] == NULL)
  {
    CLog::Warning(THISMODULE, "API_GetCalledSignalAddress_answered_call() h_data->CalledSignalAddress_answered_call == NULL \n ");
    retval = "";
    return;
  }
  strcpy(retval,h_data->CalledSignalAddress_answered_call[answered_leg]);

  CLog::Detail(THISMODULE, "API_GetCalledSignalAddress_answered_call h_data->CalledSignalAddress_answered_call %s \n", h_data->CalledSignalAddress_answered_call[answered_leg]);

  return OK;
}

#ifdef XGATE_HAS_SIP_IPPBX
// IPPBX : CallTransferApi added.
CallTransferApi::CallTransferApi(CallTransferApi* manager,int providerId,const char* name):_manager(manager),CAPIProvider(providerId,name,256)
{
  if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
    CLog::Error( THISMODULE, "Unable to register provider");
    return;
  }

  /* Register API entry points*/
  enum RegAPI_Id
  {
    // 1 input parametrs
    Id_CallTransferGetDialedDigits = 0,

    // 0 output parameters
  };

  // APIs for 1 input parameters

  // GetDialedDigits(CTask* task,const char* retval);
  RegisterAPI(
      "CallTransferGetDialedDigits",                   // external name
      Id_CallTransferGetDialedDigits,                  // ordinal 
      false,                               // asynchronous flag 
      CVariant::VT_STRING,                      // return type 
      (CAPIProvider::APIPROC) &CallTransferApi::API_GetDialedDigits, 
      CAPIDef::CDECL_TYPE,                    // calling convention
      2,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET));
}

CallTransferApi g_callTransferApiObj;

// Return a pointer to call parameter
CallTransferApi* CallTransferApi::GetCallTransferApiParam()
{
  SofiaRegistrar a;
  return a.GetCallTransferApiParam();
}

CallTransferApi::CallTransferApi(CallTransferApi* manager)
{
  PLOGERR ("inside constructor ....CallTransferApi::CallTransferApi(CallTransferApi* )\n");
  //_manager = manager;
}

STATUS API_CALL CallTransferApi::API_GetDialedDigits(CTask* task, char* retval)
{
  CLog::Warning(THISMODULE, "**************** inside API_GetDialedDigits() !!!!!!!!!!!!!!!!!\n");
  const char* temp_extension = "402";

  if (g_callTransferApiObj.dialedDigits == NULL)
  {
    CLog::Warning(THISMODULE, "g_callTransferApiObj.dialedDigits == null; return;\n");
    retval = "";
    return;
  }
  CLog::Warning(THISMODULE, "g_callTransferApiObj.dialedDigits = %s\n", g_callTransferApiObj.dialedDigits);

  strncpy(retval,g_callTransferApiObj.dialedDigits,strlen(g_callTransferApiObj.dialedDigits));
  retval[strlen(g_callTransferApiObj.dialedDigits)] = '\0';
  CLog::Warning(THISMODULE, "retval = %s\n", retval);
  CLog::Warning(THISMODULE, "********** API_GetDialedDigits: Getting dialedDigits = %s ******************* = %s\n",retval, g_callTransferApiObj.dialedDigits);

  return OK;
}

CallTransferApi::CallTransferApi()
{
}

#if 0
STATUS API_CALL RegistrationApi::API_GetMyMediatrixIP(CTask* task, char* retval, char *keyparam)
{

  static string lastChosenMdtrXg;
  if(mdtrXgCallList.find(keyparam) == mdtrXgCallList.end()){
    cout << "mdtrXgCallList entry not found " << endl;
    for(int i = 0; i <  mdtrXgIps.size(); i++){
      if(mdtrXgIps[i] == lastChosenMdtrXg){
        cout << "mdtrXgCallList ==== "<< mdtrXgIps[i] << "  ==== lastChosenMdtrXg = " << lastChosenMdtrXg << endl;
        continue;
      }else{
        if(mdtrXg[mdtrXgIps[i]]){
          lastChosenMdtrXg = mdtrXgIps[i];
          mdtrXgCallList[keyparam] = lastChosenMdtrXg;
          cout << "mdtrXgCallList ==== lastChosenMdtrXg = "<< lastChosenMdtrXg << endl;
          strcpy(retval,lastChosenMdtrXg.c_str());
          return OK;
        }
        else{
          if(i != (mdtrXgIps.size()-1))
            continue;
          else if(mdtrXg[lastChosenMdtrXg]){
            mdtrXgCallList[keyparam] = lastChosenMdtrXg;
            strcpy(retval,lastChosenMdtrXg.c_str());
            return OK;
          }else{
            strcpy(retval,"");
            return NOT_OK;
          }

        }

      }
    }

  }else{
    lastChosenMdtrXg =  mdtrXgCallList[keyparam];
    strcpy(retval, mdtrXgCallList[keyparam].c_str());
  }
  return OK;

}
#endif

STATUS API_CALL RegistrationApi::API_GetMyMediatrixIP(CTask* task, char* retval, char *keyparam)
{

#if 1
  static int lastChosenMdtrXgIndex = -1;
  static string lastChosenMdtrXg;
  for(int i = 0; i < mdtrXgIps.size();i++){
    if((++lastChosenMdtrXgIndex) ==  mdtrXgIps.size()){ // Checking the last chosen element reached the last elment, if it is then moving it to first element;
      lastChosenMdtrXgIndex = 0;
    }

    if(mdtrXg[mdtrXgIps[lastChosenMdtrXgIndex]]){  // Checking Chosen Mediatory xgate is active or not

      lastChosenMdtrXg = mdtrXgIps[lastChosenMdtrXgIndex];
      cout << "<<<<<<<<<<<  Last ChosenMdtrXg is " << lastChosenMdtrXg << endl;

      strcpy(retval,lastChosenMdtrXg.c_str());
      return OK;
    }
  }
  strcpy(retval,""); // This tells the SL(TASK) that no Mediatory xgate is active
  return NOT_OK;

#endif

}



STATUS API_CALL RegistrationApi::API_RemoveMyMediatrixMapEntry(CTask* task, char *keyparam)
{

  mdtrXgCallList.erase(keyparam);
  return OK;
}



// IPPBX End 
#endif // XGATE_HAS_SIP_IPPBX

void get_msg_t_address(msg_t* msg, String &addr)
{
  su_addrinfo_t *my_addrinfo =
    msg_addrinfo(msg);

  char network_ip[80] = {0};
  char network_addr[100]={0};
  int port=0;

  if (my_addrinfo->ai_addr) {
    getnameinfo(my_addrinfo->ai_addr, my_addrinfo->ai_addrlen, network_ip,
        (socklen_t)sizeof(network_ip), NULL, 0, NI_NUMERICHOST);
  }

  port = ntohs(((struct sockaddr_in *)
        msg_addrinfo(msg)->ai_addr)->sin_port);
  sprintf(network_addr, "%s:%i",network_ip, port);
  addr=network_addr;
}

SofiaSIPCall *GetSofiaSipCallFromCParam(const CallParameter *cp)
{
  if (cp->extraInfo.data.size() != sizeof(SofiaSIPCall*))
    return NULL;
  if (cp->extraInfo.owner != THISMODULE)
    return NULL;
#ifdef __linux__
  return  (SofiaSIPCall*) (* cp->extraInfo.data.begin());
#else
  return (SofiaSIPCall*) (*(const unsigned*) cp->extraInfo.data.begin());
#endif
}

unsigned char hexToChar(const std::string &str) {
  short c = 0;

  if(!str.empty()) {
    std::istringstream in(str);

    in >> std::hex >> c;

    if(in.fail()) {
      //throw std::runtime_error("stream decode failure");
      CLog::Warning(THISMODULE, "stream decode failure\n");
    }
  }

  return static_cast<unsigned char>(c);
}

String SipHeaderEncode(const String &value) {
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (String::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
    String::value_type c = (*i);

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

String SipHeaderDecode(const String &toDecode) {
  std::ostringstream out;

  for(String::size_type i=0; i < toDecode.length(); ++i) {
    if(toDecode.at(i) == '%') {
      String str(toDecode.substr(i+1, 2));
      out << hexToChar(str);
      i += 2;
    } else {
      out << toDecode.at(i);
    }
  }

  return out.str();
}

static char sofiaHostAddress[50] = "";
//nua_handle_t* CreateOutgoingNuaHandleFromCParam(const CallParameter *param, bool privateInfoAllowed)
nua_handle_t* CreateOutgoingNuaHandleFromCParam(const CallParameter *param, Rtp_Interface *rtp_itf)
{
  /* create operation handle */
  nua_handle_t * handle;
  String host;
  String port;
  String user;
  String password;
  String scheme;
  String params="";

  std::stringstream dest;
  std::stringstream contact;
  std::stringstream src;
  std::stringstream pai;
  bool has_pai=false;
  bool has_privacy=false;
  bool isTLS=false;
  bool isTCP=false;
  bool cuiAllowed=false;
  bool cpiAllowed=false;

  su_home_t su_home;
  su_home_init(&su_home);
  url_t* url;
  bool privateInfoAllowed = rtp_itf->IsPrivateInfoAllowed(); 
  PLOGERR("\n************IPPBX: Inside  CreateOutgoingNuaHandleFromCParam() \n");
  PLOGERR("\n************IPPBX: param->csaCalled.c_str() = %s\n",param->csaCalled.c_str());
#if 1
  /////strcpy(param->csaCalled.c_str(),"192.168.13.62");
  //strcpy(param->csaCalled.c_str(),"192.168.11.53");
  //strcpy(param->csaCalled.c_str(),"10.22.5.39");
#endif
  url=url_make(&su_home, param->csaCalled.c_str());
  if(!url)
  {
    CLog::Warning ( THISMODULE, "Error when parsing called signal address format in call parameter: '%s'",
        param->csaCalled.c_str());
    return NULL;
  }
  if(!StringExists(url->url_host))
  {
    CLog::Warning ( THISMODULE, "Error when parsing called signal address format in call parameter(no url_host): '%s'",
        param->csaCalled.c_str());
    return NULL;
  }

  ParseUserPassword(param->cnCalled, user, password);
  PLOGERR("\n************IPPBX: user = %s\n", user.c_str());
  if (param->cnpCalled == CNP_ISDN) {
    // what if?
  }

  // assembly To string
  // no display name? use user instead
  dest<< "\"" << user << "\" <";
  if(StringExists(url->url_scheme))
  {
    PLOGERR("\n************IPPBX: StringExists(url->url_scheme) %s\n", user.c_str());

    if(!stricmp(url->url_scheme,"sips"))
    {
      PLOGERR("\n************IPPBX: !stricmp(url->url_scheme, sip\n");
      dest<< url->url_scheme<<":";
      isTLS=true;
    }else
    {
      CLog::Warning(THISMODULE, "\n************IPPBX: sip:\n");
      dest<< "sip:";
    }
  }
  else
  {
    PLOGERR("\n************IPPBX: ELSE sip:\n");
    dest<< "sip:";
  }
  if(StringExists(url->url_params))
    if(strstr(url->url_params,"transport=tcp"))
      isTCP=true;

  //dest << user << "@" << url->url_host; // commented by IPPBX
  /*dest << user << "@" << url->url_host;

  if(StringExists(url->url_port))
    dest<<":"<<url->url_port;*/

#ifdef XGATE_HAS_ACME_SBC
  if(!param->proxyUserName.empty()) {
    dest << param->proxyUserName.c_str() << "@" << param->csaCalled.c_str();
  } else {
    dest << user << "@" << param->csaCalled.c_str();
  }
#else
  dest << user << "@" << param->csaCalled.c_str();
#endif

  PLOGERR("************IPPBX: dest = %s\n",dest.str().c_str()); 
  // hardcoding added by IPPBX for testing; remove it later
  //dest<<":"<<"5060";
  //////////dest<<";"<<"transport=udp"; // un-comment later

  if(StringExists(url->url_params))
    dest<<";"<<url->url_params;
  dest<<">";

  url_sanitize(url);


  printf("************IPPBX: dest = %s\n",dest.str().c_str()); 
  CLog::Warning(THISMODULE, "IPPBX: csaCalling IP address =%s\n",param->csaCalling.c_str());
  CLog::Warning(THISMODULE, "IPPBX: csCalling Src-subaddr =%s\n",param->csCalling.c_str());
  cout << "******* param->csaCalling is " << param->csaCalling << endl;


  /*
  //if (ParseHostPort (param->csaCalling, host, port) == false)
  //CLog::Warning ( THISMODULE, "Invalid origin signal address format in call parameter: '%s'",
  //				    param->csaCalling.c_str());
  PLOGERR("IPPBX: cnCalling =%s\n",param->cnCalling.c_str());
  /////ParseUserPassword(param->cnCalling, user, password); // not required????

  if(!StringExists(user.c_str()))
  {
  CLog::Warning ( THISMODULE, "Invalid origin cnCalling in call parameter: '%s'",
  param->cnCalling.c_str());
  return NULL;
  }
  */

  ParseUserPassword(param->cnCalling, user, password);

  if(!StringExists(user.c_str()))
  {
    CLog::Warning ( THISMODULE, "Invalid origin cnCalling in call parameter: '%s'",
        param->cnCalling.c_str());
    return NULL;
  }

  scheme="sip";
  host=sofiaHostAddress;
  PLOGERR("IPPBX: sofiaHostAddress =%s\n",host.c_str());
  printf("IPPBX: sofiaHostAddress =%s\n",host.c_str());
  //if(StringExists(url->url_port))
  //	port=url->url_port;
  //if(StringExists(url->url_params))
  //	params=url->url_params;

  //url_sanitize(url);

  su_home_deinit(&su_home);

  /* 
   *	Get Current Local IP 
   */


  if (param->cnpCalling == CNP_ISDN) {

    if (user == "") {
      // unavailable@unknown.invalid
      scheme="sip";
      user="unavailable";    
      host="unknown.invalid";
      port="";
      params="";
    } else {
      /** Commented out temporarily due to backward compatibility issue
        if (param->cntCalling == CNT_International && user.length()>0 && user.substr(0, 1) != "+")
        user = "+" + user;
       **/

      if (param->ccp == CCP_Restricted) {
        // Add P-Asserted Identity
        has_pai=true;

        // Add Privacy header
        has_privacy=true;
      } else 
      {
        // Add P-Asserted-Identity header
        has_pai=true;
      }
    }
  }
  if(param->cntCalling==CNT_International && user[0]!='+')
    user='+'+user;
  // assembly PAI(if any)
  if(has_pai)
  {
    pai<< "\"" << user << "\"";
    pai<< "<"<<scheme<<":" << user << "@" << host;
    if(SOFIA_SIP_PORT!=5060)
      pai<<":"<<SOFIA_SIP_PORT;
    pai<<";user=phone";
    pai<<">";
  }

  contact<< "\"" << user << "\"";
  if(isTLS)
    contact<< "<sips:" << user << "@" << host;
  else
    contact<< "<sip:" << user << "@" << host;
  if(SOFIA_SIP_PORT!=5060)
    contact<<":"<<SOFIA_SIP_PORT;
  if(isTCP)
  {
    PLOGERR("************IPPBX: if(isTCP)\n");
    // original code line below commented by IPPBX; un-comment it later.
    contact<<";transport=tcp";
    //////contact<<";transport=udp"; // forcefully adding udp for sake of testing.
  }
  // code added by IPPBX for testing; remove it later; added entire ëlse" statement below...
  else
  {
    PLOGERR("************IPPBX: else add transport = udp\n");
    // else add transport = udp.
    contact<<";transport=udp";
    ////////dest<<";transport=udp"; // un-comment later
  }

  if (param->_isSwitchBoardCall)
  {
    PLOGERR("************** THIS IS SWITCHBOARD-CALL !!!!!!!!!!!!!!!!!!\n");
  }
  /**********
    if (param->_isHuntGroupCaller)
    {
    PLOGERR ("***********************************************param->_isHuntGroupCaller == TRUE;\n");
    }
    else
    {
    PLOGERR ("***********************************************param->_isHuntGroupCaller == FALSE;\n");
    }
   ************/
#if 0
  if(StringExists(params.c_str()))
  {
    PLOGERR ("***********************************************if(StringExists(params.c_str())) = %s\n", params.c_str());
    ////if (param->_isHuntGroupCaller)
    ////{
    PLOGERR ("***********************************************  2)  param->_isHuntGroupCaller == TRUE;\n");
    contact<<";";
    contact<<params;
    /////contact<<";"<<host;
    ////}
    ////contact<<";";
    ////params = "";
  }
  else
  {
    PLOGERR ("***********************************************ELSE if(StringExists(params.c_str())) : \n");
  }
#endif
  contact<<">";


  if(has_privacy)
  {
    scheme="sip";
    user="anonymous";	// anonymous@anonymous.invalid
    host="anonymous.invalid";
    port="";
    params="";
  }



  // assembly contact
  /*nua_get_params(SofiaSIPSignalling::Instance()->GetSofiaNua(),
    NUTAG_URL_REF(host_addr),
    TAG_END());*/
  // only temporary:

  // assemby From string
  /*src<< "\"" << user << "\"";
    src<< "<"<<scheme<<":" << user << "@" << host;
    if(port!="")
    src<<":"<<port;
    if(params!="")
    src<<";"<<params;
    src<<">";*/

  src<< "\"" << user << "\"";
  if(strstr(param->csaCalling.c_str(),"NAT_")){
    //if(strstr(param->csaCalling.c_str(),"mundio.com")){
    src<< "<"<<scheme<<":" << user << "@" << param->csaCalling;
  }else{
    src<< "<"<<scheme<<":" << user << "@" << host;
  }
  if(SOFIA_SIP_PORT!=5060)
    src<<":"<<SOFIA_SIP_PORT;
  src<<">";

  // reuse params for user info
  if(StringExists(param->cui.c_str()))
  {
    params=XGATE_SOFIA_HEADER_USER_INFO;
    params+=": ";
    params+=SipHeaderEncode(param->cui);
    cuiAllowed=privateInfoAllowed;
  }

  // reuse scheme for private info
  if(StringExists(param->cpinfo.c_str()))
  {
    scheme=XGATE_SOFIA_HEADER_PRIVATE_INFO;
    scheme+=": ";
    scheme+=SipHeaderEncode(param->cpinfo);
    cpiAllowed=privateInfoAllowed;
  }
  cout << "******* src.str() is " << src.str() << endl;
#ifdef XGATE_HAS_ACME_SBC
  //vector<string> sbcAddrList = ((Rtp_Interface*)med->GetInterface())->getSbcAddrList();
  //string selectedSbcAddr = ((Rtp_Interface*)med->GetInterface())->selectSbcAddrFromList();
  string selectedSbcAddr = rtp_itf->selectSbcAddrFromList();
  string route("");
  if(!selectedSbcAddr.empty()) {
    route = "<sip:" + selectedSbcAddr + ":5060;lr>";;
  }
  cout << "******* route header is: " << route << endl;

#endif

  handle = nua_handle(SofiaSIPSignalling::Instance()->GetSofiaNua(), NULL, 
      SIPTAG_TO_STR(dest.str().c_str()), 
      SIPTAG_FROM_STR(src.str().c_str()),
      SIPTAG_CONTACT_STR(contact.str().c_str()),
#ifdef XGATE_HAS_ACME_SBC
      TAG_IF(!selectedSbcAddr.empty(),SIPTAG_ROUTE_STR(route.c_str())), //TODO: Yoga, need to implement
#endif
      //SIPTAG_CONTACT_STR("sip:10.22.3.94:5060"),
      TAG_IF(has_pai, SIPTAG_P_ASSERTED_IDENTITY_STR(pai.str().c_str())),
      TAG_IF(has_privacy, SIPTAG_PRIVACY_STR("id")),
      /*NUTAG_ACCEPT_MULTIPART(1),*/
      TAG_IF(cuiAllowed, SIPTAG_HEADER_STR(params.c_str())),
      TAG_IF(cpiAllowed, SIPTAG_HEADER_STR(scheme.c_str())),

      // we must NUTAG_AUTOANSWER(0) again despite this already done
      // in higher, general nua parameters (sofia's bug)
      NUTAG_AUTOANSWER(0),
      NTATAG_REL100(1),

      //SIPTAG_REQUIRE_STR("precondition"), !!! should be configurable

      /*NUTAG_ALLOW("REFER"),
        NUTAG_ALLOW("NOTIFY"),
        NUTAG_ALLOW("MESSAGE"),
        NUTAG_ALLOW("SUBSCRIBE"),
        NUTAG_ALLOW("INFO"),
        */

         TAG_END());
  return handle;
  }

  bool GetSipAuthEntryFromSofiaSip(Sofia_Reg_Info& auth, const sip_t*sip)
  {
    CLog::Warning(THISMODULE, "***********IPPBX : inside GetSipAuthEntryFromSofiaSip()\n");

    //if(sip->sip_authorization||sip->sip_proxy_authorization || sip->sip_from->a_url->url_user == "405")
    if(sip->sip_authorization||sip->sip_proxy_authorization)
    {
      CLog::Warning(THISMODULE, "***********IPPBX : GetSipAuthEntryFromSofiaSip() : sip->sip_authorization||sip->sip_proxy_authorization == TRUE;\n");
      sip_authorization_t * sip_auth;

      if(sip->sip_authorization)
        sip_auth=sip->sip_authorization;
      else
        sip_auth=sip->sip_proxy_authorization;

      auth.authScheme = sip_auth->au_scheme;

      if (sip_auth->au_params) {
        // const char *cur; // original code commented by IPPBX
        char *cur = NULL;
        for (int indexnum = 0; 
            (cur = sip_auth->au_params[indexnum]); 
            indexnum++) 
        {
          static char *var, *val, *p, *work;
          var = val = work = NULL;

          //if (cur) // code added by IPPBX
          //{ // code added by IPPBX
          work = strdup(cur);
          ////safe_strcpy(work, cur, strlen(cur));
          //} // code added by IPPBX
          if (work ) 
          {
            var = work;
            val = strchr(var, '=');
            if (val) 
            {
              *val++ = '\0';
              while (*val == '"') 
              {
                *val++ = '\0';
              }
              p = strchr(val, '"');
              if (p) {
                *p = '\0';
              }

              if (!strcasecmp(var, "username")) {
                /** The username.
                */
                auth.authUser = val;
                CLog::Detail(THISMODULE, "Authenticate Header User: %s", auth.authUser.c_str());
              } else if (!strcasecmp(var, "realm")) {
                /** The realm string, which is used to prompt to the user which username 
                  and password should be used to access the resource.
                  */
                //auth.authRealm = val;
                auth.authRealm = "xGate access";
                CLog::Detail(THISMODULE, "Authenticate Header Var: %s", auth.authRealm.c_str());
              } else if (!strcasecmp(var, "nonce")) {
                /** The nonce value.
                */
                auth.authNonce = val;
                CLog::Detail(THISMODULE, "Authenticate Header Nonce: %s", auth.authNonce.c_str());
              } else if(!strcasecmp(var, "domain")){
                /** Domain
                */
                auth.authDomain=val;
                CLog::Detail(THISMODULE, "Authenticate Header Domain: %s", auth.authDomain.c_str());
              }
              else if (!strcasecmp(var, "uri")) {
                /** The URI value.
                */
                auth.authUri = val;
                CLog::Detail(THISMODULE, "Authenticate Header URI: %s", auth.authUri.c_str());
              } else if (!strcasecmp(var, "qop")) {
                /** QOP (Quality of Protection) parameter.
                */
                auth.authQop = val;
              } else if (!strcasecmp(var, "cnonce")) {
                /** The cnonce value.
                */
                auth.authCNonce = val;
              } else if (!strcasecmp(var, "response")) {
                /** The response digest.
                */
                auth.authResponse = val;
                CLog::Detail(THISMODULE, "Authenticate Header Response during parsing: %s", auth.authResponse.c_str());
              } else if (!strcasecmp(var, "nc")) {
                // do nothing
                auth.authNonceCount = val;
              } else if (!strcasecmp(var, "algorithm")) {
                /** The algorithm to be used to produce the digest.
                */
                auth.authAlgorithm = val;
              } else if (!strcasecmp(var, "stale")) {
                /** The stale value.
                */
                auth.authStale = atoi(val);
              } else if (!strcasecmp(var, "qop")) {
                /** The stale value.
                */
                auth.authQop = val;
              }

            }
            //CLog::Warning(THISMODULE, "Malformed incoming authentification packet!");
            free(work);
          }
        }
      }
      if(SofiaRegistrar::Instance()->VerifyChallenge(auth.authNonce))
      {
        CLog::Warning(THISMODULE, "*****************IPPBX : GetSipAuthEntryFromSofiaSip() : VerifyChallenge(auth.authNonce) == true ; return true;\n");
        return true;
      }
      else
      {
        CLog::Warning(THISMODULE, "*****************IPPBX : GetSipAuthEntryFromSofiaSip() : ELSE *** VerifyChallenge(auth.authNonce) == false ; return false;\n");
        return false;
      }
    }
    else
    {
      CLog::Warning(THISMODULE, "***********IPPBX : GetSipAuthEntryFromSofiaSip() : BOTH *** sip->sip_authorization||sip->sip_proxy_authorization == FALSE; return false;\n");
      return false;
    }
  }

  //////////////////////////////////////////////
  // Precondition handler
  //////////////////////////////////////////////

  SofiaPrecondition::SofiaPrecondition(SofiaPreconditionLevel recvstat, 
      SofiaPreconditionLevel sendstat,
      SofiaPreconditionLevel desiredrecv,
      SofiaPreconditionLevel desiredsend)
  {
    // must use PreconditionLevel_No
    assert(recvstat!=PreconditionLevel_None);
    assert(sendstat!=PreconditionLevel_None);

    current.localReceive=recvstat;
    current.localSend=sendstat;
    current.remoteReceive=PreconditionLevel_No;
    current.remoteSend=PreconditionLevel_No;

    // we always want end to end
    desired.localReceive=desiredrecv;
    desired.remoteSend=desiredrecv;
    desired.localSend=desiredsend;
    desired.remoteReceive=desiredsend;

    needconfirm.localReceive=PreconditionLevel_None;
    needconfirm.remoteSend=PreconditionLevel_None;
    needconfirm.localSend=PreconditionLevel_None;
    needconfirm.remoteReceive=PreconditionLevel_None;

    preconditionType=PreconditionType_e2e;
  }	

  void SofiaPrecondition::setSendReserveStatus(SofiaPreconditionLevel stat)
  {
    // must use PreconditionLevel_No
    assert(stat!=PreconditionLevel_None);
    current.localSend=stat;
  }
  void SofiaPrecondition::setReceiveReserveStatus(SofiaPreconditionLevel stat)
  {
    // must use PreconditionLevel_No
    assert(stat!=PreconditionLevel_None);
    current.localReceive=stat;
  }

  void SofiaPrecondition::setDesiredSendReserveStatus(SofiaPreconditionLevel stat)
  {
    // end to end demand
    desired.localSend=stat;
    desired.remoteReceive=stat;
  }

  void SofiaPrecondition::setDesiredReceiveReserveStatus(SofiaPreconditionLevel stat)
  {
    // end to end demand
    desired.localReceive=stat;
    desired.remoteSend=stat;
  }

  void incrementPreconditionLevel(SofiaPreconditionLevel &prev,SofiaPreconditionLevel add)
  {
    if(prev==PreconditionLevel_Yes)
      return;
    else if(prev>add)
      return;
    else
      prev=add;
  }

  bool SofiaPrecondition::parseAndUpdate(sdp_media_t* media)
  {
    char s0[50];
    char s1[50];
    char s2[50];
    sdp_attribute_t* attr;
    bool updated=false;
    if(!media)
      return false;
    attr=media->m_attributes;
    if(!attr)
      return false;
    while(attr)
    {
      // parse "current" precondition
      if(!strcmp(attr->a_name,"curr"))
      {
        if(sscanf(attr->a_value, "qos %10s %10s", s0, s1) != EOF){
          if(!strcmp(s0, "e2e"))
          {
            if(strstr(s1, "send"))
              incrementPreconditionLevel(current.remoteSend, PreconditionLevel_Yes);

            if(strstr(s1, "recv"))
              incrementPreconditionLevel(current.remoteReceive, PreconditionLevel_Yes);
          }else if(!strcmp(s0,"local"))
          {
            if(strstr(s1, "send"))
              incrementPreconditionLevel(current.remoteSend, PreconditionLevel_Yes);

            if(strstr(s1, "recv"))
              incrementPreconditionLevel(current.remoteReceive, PreconditionLevel_Yes);
          }else if(!strcmp(s0,"remote"))
          {
            if(strstr(s1, "send"))
              incrementPreconditionLevel(current.localReceive, PreconditionLevel_Yes);

            if(strstr(s1, "recv"))
              incrementPreconditionLevel(current.localSend, PreconditionLevel_Yes);
          }
        }
        else
          CLog::Warning(THISMODULE, "Error while parsing precondition!");
        updated=true;
      }

      // parse "desired" precondition
      else if(!strcmp(attr->a_name,"des"))
      {
        if(sscanf(attr->a_value, "qos %10s %10s %10s", s0, s1, s2) != EOF){
          SofiaPreconditionLevel lvl=PreconditionLevel_None;
          if(strstr(s0, "optional"))
            lvl=PreconditionLevel_Optional;
          else if(strstr(s0,"mandatory"))
            lvl=PreconditionLevel_Mandatory;

          if(!strcmp(s1, "e2e"))
          {
            preconditionType=PreconditionType_e2e; //e2e
            if(strstr(s2, "send"))
              incrementPreconditionLevel(desired.localReceive, lvl);

            if(strstr(s2, "recv"))
              incrementPreconditionLevel(desired.localSend, lvl);
          }else if(!strcmp(s1,"local"))
          {
            preconditionType=PreconditionType_segmented; //segmented
            if(strstr(s2, "send"))
              incrementPreconditionLevel(desired.remoteSend, lvl);

            if(strstr(s2, "recv"))
              incrementPreconditionLevel(desired.remoteReceive, lvl);
          }else if(!strcmp(s1,"remote"))
          {
            preconditionType=PreconditionType_segmented; //segmented
            if(strstr(s2, "send"))
              incrementPreconditionLevel(desired.localReceive, lvl);

            if(strstr(s2, "recv"))
              incrementPreconditionLevel(desired.localSend, lvl);
          }
        }
        else
          CLog::Warning(THISMODULE, "Error while parsing precondition!");
        updated=true;
      }

      // parse "confirmation required" precondition
      else if(!strcmp(attr->a_name,"conf"))
      {
        if(sscanf(attr->a_value, "qos %10s %10s", s0, s1) != EOF){
          if(!strcmp(s0, "e2e"))
            CLog::Warning(THISMODULE, "Undesired precondition conf type(e2e)");
          else if(!strcmp(s0,"local"))
            CLog::Warning(THISMODULE, "Undesired precondition conf type(local)");
          else if(!strcmp(s0,"remote"))
          {
            if(strstr(s1, "send"))
              incrementPreconditionLevel(needconfirm.localReceive, PreconditionLevel_Yes);

            if(strstr(s1, "recv"))
              incrementPreconditionLevel(needconfirm.localSend, PreconditionLevel_Yes);
          }
        }
        else
          CLog::Warning(THISMODULE, "Error while parsing precondition!");
        updated=true;
      }
      attr=attr->a_next;
    }
    return updated;
  }

  SofiaPrecondition* SofiaPrecondition::parseAndCreateSofiaPrecondition(sdp_media_t*media)
  {
    sdp_attribute_t* attr;
    SofiaPrecondition * prec;
    bool hasprecondition=false;
    if(!media)
      return NULL;
    attr=media->m_attributes;
    if(!attr)
      return NULL;
    while(attr)
    {
      if(!strcmp(attr->a_name,"curr")||
          !strcmp(attr->a_name,"des")||
          !strcmp(attr->a_name,"conf"))
      {
        hasprecondition=true;
        break;
      }
      attr=attr->a_next;
    }
    if(!hasprecondition)
      return NULL;

    prec=new SofiaPrecondition;
    if(!prec->parseAndUpdate(media))
    {
      if ( prec ) {
        delete prec;
        prec = NULL;
      }
      return NULL;
    }
    return prec;
  }

  String SofiaPrecondition::output(bool requestConfirm)
  {
    String tmp;
    tmp="";
    if(preconditionType==PreconditionType_e2e)
    {
      tmp+="a=curr:qos e2e ";
      if(current.localSend==PreconditionLevel_Yes)
        tmp+="send";
      if(current.localReceive==PreconditionLevel_Yes)
        tmp+="recv";
      if((current.localReceive!=PreconditionLevel_Yes)&&
          (current.localSend!=PreconditionLevel_Yes))
        tmp+="none";
      tmp+="\r\n";
    }else // segmented
    {
      tmp+="a=curr:qos local ";
      if(current.localSend==PreconditionLevel_Yes)
        tmp+="send";
      if(current.localReceive==PreconditionLevel_Yes)
        tmp+="recv";
      if((current.localReceive!=PreconditionLevel_Yes)&&
          (current.localSend!=PreconditionLevel_Yes))
        tmp+="none";
      tmp+="\r\n";
      tmp+="a=curr:qos remote ";
      if(current.remoteSend==PreconditionLevel_Yes)
        tmp+="send";
      if(current.remoteReceive==PreconditionLevel_Yes)
        tmp+="recv";
      if((current.remoteSend!=PreconditionLevel_Yes)&&
          (current.remoteReceive!=PreconditionLevel_Yes))
        tmp+="none";
      tmp+="\r\n";
    }

    if(preconditionType==PreconditionType_e2e)
    {
      if(desired.remoteSend==PreconditionLevel_Mandatory)
        tmp+="a=des:qos mandatory e2e send";
      else if(desired.remoteSend==PreconditionLevel_Optional)
        tmp+="a=des:qos optional e2e send";

      if(desired.remoteReceive==desired.remoteSend)
      {
        if(desired.remoteReceive!=PreconditionLevel_None)
          tmp+="recv\r\n";
        else
          tmp+="a=des:qos none e2e sendrecv\r\n";
      }else
      {
        tmp+="\r\n";
        if(desired.remoteReceive==PreconditionLevel_Mandatory)
          tmp+="a=des:qos mandatory e2e recv\r\n";
        else if(desired.remoteReceive==PreconditionLevel_Optional)
          tmp+="a=des:qos optional e2e recv\r\n";
      }
    }else //segmented
    {
      if(desired.localSend==PreconditionLevel_Mandatory)
        tmp+="a=des:qos mandatory local send";
      else if(desired.localSend==PreconditionLevel_Optional)
        tmp+="a=des:qos optional local send";

      if(desired.localReceive==desired.localSend)
      {
        if(desired.localReceive!=PreconditionLevel_None)
          tmp+="recv\r\n";
        else
          tmp+="a=des:qos none local sendrecv\r\n";
      }else
      {
        tmp+="\r\n";
        if(desired.localReceive==PreconditionLevel_Mandatory)
          tmp+="a=des:qos mandatory local recv\r\n";
        else if(desired.localReceive==PreconditionLevel_Optional)
          tmp+="a=des:qos optional local recv\r\n";
      }

      if(desired.remoteSend==PreconditionLevel_Mandatory)
        tmp+="a=des:qos mandatory remote send";
      else if(desired.remoteSend==PreconditionLevel_Optional)
        tmp+="a=des:qos optional remote send";

      if(desired.remoteReceive==desired.remoteSend)
      {
        if(desired.remoteReceive!=PreconditionLevel_None)
          tmp+="recv\r\n";
        else
          tmp+="a=des:qos none remote sendrecv\r\n";
      }else
      {
        tmp+="\r\n";
        if(desired.remoteReceive==PreconditionLevel_Mandatory)
          tmp+="a=des:qos mandatory remote recv\r\n";
        else if(desired.remoteReceive==PreconditionLevel_Optional)
          tmp+="a=des:qos optional remote recv\r\n";
      }
    }

    if(requestConfirm)
    {
      tmp+="a=conf:qos remote sendrecv\r\n";
    }
    return tmp;
  }

  bool SofiaPrecondition::isPreconditionMet()
  {
    if((current.localReceive>=desired.localReceive)&&
        (current.localSend>=desired.localSend)&&
        (current.remoteReceive>=desired.remoteReceive)&&
        (current.remoteSend>=desired.remoteSend))
      return true;
    else
      return false;
  }

  bool SofiaPrecondition::isConfirmRequired()
  {
    if(preconditionType==PreconditionType_e2e)
      return true;

    if((current.localReceive>=needconfirm.localReceive)&&
        (current.localSend>=needconfirm.localSend))
      return true;
    else return false;
  };

  SofiaPrecondition::~SofiaPrecondition(){};


  //////////////////////////////////////////////
  // SofiaSIPCall
  //////////////////////////////////////////////
  SofiaSIPCall::SofiaSIPCall(const CallParameter & cparam, CallDirection dir, const char *itf_name)
    : Call(dir, itf_name), signature1(SIGNATURE),
    disconnectReq(false),
    disconnectReqCause(CC_NormalUnspecified), cmIsNotified(false), signature2(SIGNATURE+1), nua_handle(NULL),
    lastKnownCause(CC_NormalUnspecified), savedRemoteSdp(NULL), hasCredential(false), callTableIterator(NULL),
    _precondition(NULL),_update_received(false), timerRequested(false)
  {
    channelIsStarted=false;
    _callParam = cparam;
    remoteMediaAddr=NULL;
    peerSipCall=NULL;
    peerSipCall = GetSofiaSipCallFromCParam(&cparam);
    hasSipAuth=false;
    //sipAuth=NULL;
    recv_offer_ptime = 0;
    recv_answer_ptime = 0;

    if(peerSipCall)
    {
      peerSipCall->peerSipCall = this;
    }

    _callParam.extraInfo.data.resize(sizeof(this));

    * _callParam.extraInfo.data.begin() = (unsigned)this;

    rfc2833_rx_pt = rfc2833_tx_pt = -1;
  }

  SofiaSIPCall::~SofiaSIPCall()
  {
    if(nua_handle)
    {
      CLog::Warning(THISMODULE, "disposal of sofia call contain unclean nua handle!");
      attachSofiaNuaHandle(NULL);
    }

    if(callTableIterator!=(SofiaCallTableIterator)NULL)
      SofiaSIPSignalling::Instance()->RemoveFromIncomingCallTable(this);

    signature1 = signature2 = 0; // now IsValid()==false
    /* if(sipAuth){ delete sipAuth; sipAuth=NULL;} */
    if(remoteMediaAddr!=NULL) {
      delete remoteMediaAddr;
      remoteMediaAddr = NULL;
    }
    if (peerSipCall && peerSipCall->peerSipCall==this)
      peerSipCall->peerSipCall = NULL;
    if(savedRemoteSdp)
      su_free(SofiaSIPSignalling::Instance()->GetSofiaHome(),
          savedRemoteSdp);
    if(_precondition)
    {
      delete _precondition;
      _precondition=NULL;
    }
  }

  void SofiaSIPCall::InviteCall(bool isHold, bool isResume)
  {
    
#ifdef CALL_TRANSFER
    bool replace_flag = false;
    char replaces_str[250];
#endif

    std::stringstream history_info;

    /* VMD code added for History-Info header */
    CallParameter *csParam = GetCallParameter();

#ifdef CALL_TRANSFER
    replace_flag = csParam->redirectionIndicator;
    if(replace_flag == 1){
      string ocn = csParam->cnOriginalCalledNumber;
      cout << "****** original called number is " << ocn << " ***********" << endl;
      call_transfer_extn.erase(ocn);
    }
    else
    {
      call_transfer_extn.erase(csParam->cnCalled.c_str());
    }
#endif

    if ( strstr(csParam->cnCalled.c_str(),"VMD") != NULL) {
      /* Voice mail deposit scenario */
#if 0
      history_info << "History-Info: " << "\<sip:";
      history_info << csParam->cnOriginalCalledNumber << "\>;";
      history_info << "index=1" <<"\n\0";
#else
      history_info << "History-Info: ";
      history_info << csParam->cnOriginalCalledNumber <<"\n\0" ;
#endif
      //PLOGERR("History_Info =%s\n",history_info.str().c_str());
      CLog::Detail(THISMODULE, "History_Info =%s\n",history_info.str().c_str());

      nua_invite(nua_handle,
          /* other tags as needed ... */
          SIPTAG_HEADER_STR(history_info.str().c_str()),
          TAG_IF(isHold, SOATAG_HOLD("*")),
          TAG_IF(isResume, SOATAG_HOLD("")),
          NUTAG_AUTOACK(0),
          TAG_END());
    } /* VMD code added for History Info */ 
    else {
      printf("REPLACES 1 : calling %s ; called %s origcalled %s \n", csParam->cnCalling.c_str(), csParam->cnCalled.c_str(), csParam->cnOriginalCalledNumber.c_str());
      printf("REPLACES 2 : calling address %s ; called address %s \n", csParam->csaCalling.c_str(), csParam->csaCalled.c_str());

      CLog::Detail(THISMODULE, "Setup Invite called for caller: %s, called: %s, from ip: %s to ip: %s", csParam->cnCalling.c_str(), \
          csParam->cnCalled.c_str(), csParam->csaCalling.c_str(), csParam->csaCalled.c_str());
#ifdef CALL_TRANSFER
      if(replace_flag == true){

        sprintf(replaces_str,"%s;to-tag=%s;from-tag=%s",call_id[csParam->cnCalled.c_str()].c_str(),to_tag[csParam->cnCalled.c_str()].c_str(),from_tag[csParam->cnCalled.c_str()].c_str());
      }
      char trans_info[256] ;
      memset(trans_info,0,sizeof(trans_info));
#if 1
      if (replace_flag)
      {
        char key_t[256];
        memset(key_t,0,sizeof(key_t));
        char key[256];
        memset(key,0,sizeof(key));
        char *token = NULL;
        token = strtok(csParam->csaCalling.c_str(), "_");
        while(token != NULL)
        {
          printf(" 1 token = %s \n", token);
          sprintf(key_t,"%s",token);
          token = strtok(NULL, "_");
          printf(" 2 token = %s \n", token);
          if(token !=NULL)
          {
            sprintf(key_t,"%s",token);
            printf(" 3 key_t = %s \n", key_t);
          }
        }
        printf("4 key_t = %s \n", key_t);
        /*
           char csacalled_t[100];
           memset(csacalled_t,0,sizeof(csacalled_t));
           strcpy(csacalled_t,csParam->csaCalling.c_str());
           csacalled_t[csParam->csaCalling.length()]='\0';
           printf(">>>> csacalled_t %s & size %d \n", csacalled_t, strlen(csacalled_t));
           char temp_st[100];

           memset(temp_st,0,sizeof(temp_st));
           int offset = strlen(csacalled_t)-18;
           printf("* offset is %d \n",offset);
           char *p = csacalled_t[13];
           printf("***** csacalled_t[13] is %s\n",*p);

           strncpy(temp_st,csacalled_t[offset],18);
           temp_st[18]='\0';
        //		csacalled_t = csacalled_t.substr( strlen(csacalled_t.c_str()) - 18 );
        cout << "INVITE WITH REPLACES csacalled_t : " << temp_st << endl;
        //		*/
        //sprintf(key,"%s_%s@%s",csParam->cnOriginalCalledNumber.c_str(), csParam->cnCalled.c_str(), csParam->csaCalling.c_str());
        //sprintf(key,"%s_%s@%s",csParam->cnOriginalCalledNumber.c_str(), csParam->cnCalled.c_str(), temp_st);
        sprintf(key,"%s_%s@%s",csParam->cnOriginalCalledNumber.c_str(), csParam->cnCalled.c_str(), key_t);
        cout << "INVITE WITH REPLACES key : " << key << endl;
        cout << "INVITE WITH REPLACES call_transfer_replace map : " << call_transfer_replace[key] << endl;
        sprintf(trans_info, "Trans_Info: %s",call_transfer_replace[key].c_str());
        //trans_info = "Trans_Info: ";
        //trans_info += call_transfer_replace[key];
        printf("INVITE WITH REPLACES : %s \n", trans_info);
      }
#endif;
      nua_invite(nua_handle,
          /* other tags as needed ... */
          //      TAG_IF(isHold, SOATAG_HOLD("*")),
          TAG_IF(isResume, SOATAG_HOLD("")),
          //TAG_IF(replace_flag, SIPTAG_REPLACES_STR(replaces_str)),
          TAG_IF(replace_flag, SIPTAG_UNKNOWN_STR(trans_info)),
          //      TAG_IF(replace_flag, SIPTAG_REFERRED_BY_STR(referredby)),
          TAG_IF(isResume, SOATAG_HOLD("")),
          NUTAG_AUTOACK(0),
          TAG_END());
#else
      /* Normal call scenario */
      nua_invite(nua_handle,
          /* other tags as needed ... */
          TAG_IF(isHold, SOATAG_HOLD("*")),
          TAG_IF(isResume, SOATAG_HOLD("")),
          NUTAG_AUTOACK(0),
          TAG_END());
#endif
    }
  }

  bool SofiaSIPCall::IsValid() const
  {
    try {
      return (signature1 == SIGNATURE) && (signature2 == SIGNATURE+1);
    } catch (...) {
      return false;
    }
  }

  //
  // Handle incoming call
  //
  bool SofiaSIPCall::OnIncomingCall()
  {
    cmIsNotified = true;
    SetupInd(*GetCallParameter());
    return true;
  }

  bool SofiaSIPCall::OnCallProceeding(int status,
      sip_t const *sip, CallProgress progress)
  {	
    switch (status) {
      /*case scTrying:
      // Only send proceeding IF media description is available
      if (progress==CP_InBandInfoAvailable) {
      ProceedingInd(progress, 0);
      }
      break;*/
      case scRinging:
        AlertingInd(progress, 0);
        break;
      case scProgress:
        if (progress==CP_InBandInfoAvailable) 
          AlertingInd(progress, 0);
        else {
          if (GetState() < CS_Proceeding) ProceedingInd(progress, 0);
          ProgressInd(progress, 0);
        }
        break;
    }
    return true;
  }

  //
  // Activate underlying media
  //
  bool SofiaSIPCall::StartChannel(ACE_INET_Addr * addr,
      Rtp_Payload_Code payload_code)
  {
    channelIsStarted=false;
    Rtp_Media * rtpMedia=(Rtp_Media*)GetMedia();
    CLog::Detail(THISMODULE, "SofiaSIPCall::StartChannel rtpMedia = %02x \n", rtpMedia);
    if(!rtpMedia)return false;
    if(!addr)return false;

    if(IsUsingVirtualRtp())
    {
      channelIsStarted=true;
      return true;
    }

    // Enable RFC 2833, if required
    if (rfc2833_rx_pt > 0 && rfc2833_tx_pt > 0) {
      TRACE_RFC2833_("sofia", "RFC2833: activating media, rx_pt=%d, tx_pt=%d",
          rfc2833_rx_pt, rfc2833_tx_pt);
      // todo: why it should be both TX/RX to activate RFC2833?
      rtpMedia->DTMFSetRfc2833(rfc2833_rx_pt, rfc2833_tx_pt);
    }


    channelIsStarted = rtpMedia->Open() && rtpMedia->Activate(*addr, payload_code, recv_offer_ptime);
    if(!channelIsStarted)
    {
      CLog::Warning(THISMODULE, "Error activating media part!");
    }
    return channelIsStarted;


  }

  //
  // Stop the media
  //
  bool SofiaSIPCall::StopChannel()
  {
    Rtp_Media * rtpMedia=(Rtp_Media*)GetMedia();
    channelIsStarted=false;
    if(!rtpMedia)return true;

    if(IsUsingVirtualRtp())
      return true;

    return rtpMedia->Close();
  }

  // Merge new incoming call with previous call
  // and issue overlap indication
  void SofiaSIPCall::OverlapNuaHandle(int status, nua_handle_t*nh, sip_t const *sip, tagi_t tags[])
  {
    CallParameter cparam;
    CreateSofiaCallParam(cparam, sip, tags);

    if(cparam.cnCalled.find(_callParam.cnCalled)!=0)
    {
      CLog::Warning(THISMODULE, "Incoming overlap re-invite without request number continuity");
      nua_respond(nh, 400, "Bad Request", TAG_END());
    }
    else if(cparam.cnCalled.length() <= _callParam.cnCalled.length())
    {
      CLog::Warning(THISMODULE, "Incoming overlap re-invite without additional digits!");
      // the new invite doesnt contains additional digits
      nua_respond(nh, 484, "Address Incomplete", TAG_END());
    }else
    {
      // lets swap nua handle for this call
      nua_handle_t *old_nh=nua_handle;
      const char* digits=cparam.cnCalled.c_str();

      attachSofiaNuaHandle(NULL);
      attachSofiaNuaHandle(nh);

      // respond 484 to the old call
      nua_respond(old_nh, 484, "Address Incomplete", TAG_END());

      // indicate to call framework
      // for now all overlap is indicated with incomplete address assumption
      OverlapInd(&digits[_callParam.cnCalled.length()], false);
    }

  }

  //
  // Call state changed to established
  //
  bool SofiaSIPCall::OnCallEstablished()
  {   
    if(GetDirection()==CD_Incoming)
      ConnectCnf();
    else
      ConnectInd(0);
    return true;
  }

  bool SofiaSIPCall::OnCallTransfer()
  {

    ReferInd();

    return true;
  }


  bool SofiaSIPCall::OnCallUnattendTransfer()
  {

    UnAttendReferInd();

    return true;
  }

  bool SofiaSIPCall::OnCallFlip()
  {

    CallFlipInd();

    return true;
  }


  bool SofiaSIPCall::OnCallUnFlip()
  {

    CallUnFlipInd();
    return true;
  }

  bool SofiaSIPCall::OnCallPark()
  {

    CallParkInd();

    return true;
  }

  bool SofiaSIPCall::OnCallHold(int ishold)
  {

    CallHoldInd(ishold);

    return true;
  }

  bool SofiaSIPCall::OnCallRec(int isrec)
  {

    CallRecInd(isrec);

    return true;
  }

  void SofiaSIPCall::OnCallDisconnected()
  {
    if (nua_handle) {    
      attachSofiaNuaHandle(NULL);
    } else {
      assert(false);
    }

    if (GetDirection()==CD_Outgoing || cmIsNotified) {
      if (disconnectReq) {
        RelComInd(disconnectReqCause);
      } else
        DisconnectInd(lastKnownCause, CP_NoIndication);
    } else {
      DetachMedia();
      DetachTask();
      CallManager::Instance()->DestroyCall(this);
    }
  }

  void SofiaSIPCall::OnCallSuspended()
  {
    if(GetState()!=CS_Suspended)
    {
      this->GetCallParameter()->suspend_resume_notification = true;
      SuspendInd();
    }
  }

  void SofiaSIPCall::OnCallResumed()
  {
    if(GetState()!=CS_Connected)
      ResumeInd();	
  }

  void SofiaSIPCall::attachSofiaNuaHandle(nua_handle_t* handle)
  {
    if(nua_handle!=NULL)
    {
      // seems risky to put this here,
      // better hook to terminated event callback instead
      //nua_handle_destroy(nua_handle);

      if(handle!=NULL)
        CLog::Warning(THISMODULE, "Double Sofia handle attached!");
    }
    if(handle!=NULL)
    {
      nua_handle_bind (handle, (nua_hmagic_t *)this);
    }
    nua_handle=handle;

  }

  bool SofiaSIPCall::IsUsingVirtualRtp() const
  {
    SofiaSIPCall *this_ = (SofiaSIPCall*)this;
    Rtp_Media *med = (Rtp_Media*) this_->GetMedia();
    if (!med)
      return false;

    return med->GetChannel()->GetBoardType() == RTP_Board_Virtual;
  }


#define SOFIA_SIP_UNSUP(f)		CLog::Warning(THISMODULE, "Unsupported function '%s'", #f)
#define SOFIA_SIP_CHECK_CALL(f)	if (nua_handle==NULL) { assert(false); CLog::Warning(THISMODULE, "%s: call has been cleared", #f); \
  RelComInd(CC_CallHasBeenCleared); return false; }

  bool SofiaSIPCall::DeviceOverlapReq(const char* additionalDigits,bool complete)
  {
    PLOGERR("\n*****IPPBX : inside SofiaSIPCall::DeviceOverlapReq()");
    SOFIA_SIP_CHECK_CALL(DeviceProgressReq);

    if(GetState()>CS_Proceeding)
    {
      CLog::Warning(THISMODULE, "Cannot overlap outgoing sofia call, call already on late state");
      return false;
    }
    _callParam.cnCalled+=additionalDigits;

    nua_handle_t*handle = CreateOutgoingNuaHandleFromCParam(&_callParam, false);

    if (handle == NULL) {
      CLog::Warning(THISMODULE, "Cannot process overlap request for outgoing call");

      return false;
    }    

    // customized this new handle so it can be related to previous
    // sofia call handle
    sip_call_id_t *call_id;
    sip_from_t *from;
    sip_cseq_t *cseq;
    nua_get_hparams(nua_handle,
        SIPTAG_CALL_ID_REF(call_id), 
        SIPTAG_FROM_REF(from),
        SIPTAG_CSEQ_REF(cseq),
        TAG_END());
    // increase cseq (assumpe 10x max retrying)
    cseq->cs_seq+=10; 
    nua_set_hparams(handle,
        SIPTAG_CALL_ID(call_id),
        SIPTAG_FROM(from),
        SIPTAG_CSEQ(cseq),
        TAG_END());

    attachSofiaNuaHandle(NULL);
    attachSofiaNuaHandle(handle);

    if(IsUsingVirtualRtp())
    {
      SetLocalSDPParameter(peerSipCall->savedRemoteSdp);
    }else
    {
      nua_set_hparams(handle,
          NUTAG_ALLOW("UPDATE"),
          TAG_END());
      SetLocalSDPParameter((Rtp_Media*)GetMedia());
    }

    // finally re-invite
    InviteCall();

    return true;
  }

  bool SofiaSIPCall::DeviceProceedingReq(CallProgress progress, CallExtraInfo* exinfo)
  {
    PLOGERR("\n*****IPPBX : inside SofiaSIPCall::DeviceProceedingReq()");
    SOFIA_SIP_CHECK_CALL(DeviceProceedingReq);

    // Send PROGRESS instead
    //return DeviceProgressReq(progress, exinfo);
    return true;
  }

  // DeviceProgressReq use early media as call progress is informed by this mean
  bool SofiaSIPCall::DeviceProgressReq(CallProgress progress, CallExtraInfo* exinfo)
  {
    SetLocalSDPParameter((Rtp_Media*)GetMedia());
    PLOGERR("\n*****IPPBX : inside SofiaSIPCall::DeviceProgressReq()");
    SOFIA_SIP_CHECK_CALL(DeviceProgressReq);
    if(progress==CP_InBandInfoAvailable)
    {
      if(_callParam.cnCalling.c_str() != NULL)
      {
        if ( strlen(_callParam.cnCalling.c_str()) >= 10)
        {
          SetLocalSDPParameter((Rtp_Media*)GetMedia());
          nua_set_hparams(nua_handle,
              SIPTAG_CONTACT_STR("sip:10.22.3.94"),
              NUTAG_EARLY_MEDIA	(1),
              NUTAG_EARLY_ANSWER(1),
              //NUTAG_ONLY183_100REL(1),
              TAG_END());

          nua_respond(nua_handle, 183, "Session Progress", 
              NUTAG_AUTOANSWER(0),
              NUTAG_EARLY_MEDIA	(1),   //PUVAN -- Added for TEST RBT... Remove after Test
              NUTAG_EARLY_ANSWER(1),     //PUVAN -- Added for TEST RBT... Remove after Test
              TAG_END());

          return true;
        }
      }

      nua_set_hparams(nua_handle,
          SIPTAG_CONTACT_STR("sip:10.22.3.94"),
          NUTAG_EARLY_MEDIA    (1),
          NUTAG_EARLY_ANSWER(1),
          //NUTAG_ONLY183_100REL(1),
          TAG_END());

      nua_respond(nua_handle, 183, "Session Progress",
          NUTAG_AUTOANSWER(0),
          TAG_END());

      return true;
    }else
    {
      if(_callParam.cnCalling.c_str() != NULL)
      {
        if ( strlen(_callParam.cnCalling.c_str()) >= 10)
        {
          SetLocalSDPParameter((Rtp_Media*)GetMedia());
          nua_set_hparams(nua_handle,
              SIPTAG_CONTACT_STR("sip:10.22.3.94"),
              TAG_IF(!IsUsingVirtualRtp(), NUTAG_EARLY_MEDIA(1)),
              TAG_IF(!IsUsingVirtualRtp(), NUTAG_EARLY_ANSWER(1)),
              //NUTAG_ONLY183_100REL(1),
              TAG_END());
          nua_respond(nua_handle, 183, "Session Progress",
              NUTAG_AUTOANSWER(0),
              NUTAG_EARLY_MEDIA(1),       //PUVAN -- Added for TEST RBT... Remove after Test
              NUTAG_EARLY_ANSWER(1),     //PUVAN -- Added for TEST RBT... Remove after Test
              TAG_END());

          return true;
        }
      }

      nua_set_hparams(nua_handle,
          SIPTAG_CONTACT_STR("sip:10.22.3.94"),
          TAG_IF(!IsUsingVirtualRtp(), NUTAG_EARLY_MEDIA(1)),
          TAG_IF(!IsUsingVirtualRtp(), NUTAG_EARLY_ANSWER(1)),
          //NUTAG_ONLY183_100REL(1),
          TAG_END());
      nua_respond(nua_handle, 183, "Session Progress",
          NUTAG_AUTOANSWER(0),
          TAG_END());

      return true;
    }

    /*SOFIA_SIP_CHECK_CALL(DeviceProgressReq);
      if(progress==CP_InBandInfoAvailable)
      {
      if(_callParam.cnCalling.c_str() != NULL)
      {
      if ( strlen(_callParam.cnCalling.c_str()) >= 10)
      {
      SetLocalSDPParameter((Rtp_Media*)GetMedia());
      nua_set_hparams(nua_handle,
      NUTAG_EARLY_MEDIA       (1),
      NUTAG_EARLY_ANSWER(1),
    //NUTAG_ONLY183_100REL(1),
    TAG_END());

    nua_respond(nua_handle, 183, "Session Progress",
    NUTAG_AUTOANSWER(0),
    NUTAG_EARLY_MEDIA(1),      //PUVAN -- Added for TEST RBT... Remove after Test
    NUTAG_EARLY_ANSWER(1),     //PUVAN -- Added for TEST RBT... Remove after Test
    TAG_END());
    return true;
    }
    }

    nua_set_hparams(nua_handle,
    NUTAG_EARLY_MEDIA       (1),
    NUTAG_EARLY_ANSWER(1),
    //NUTAG_ONLY183_100REL(1),
    TAG_END());

    nua_respond(nua_handle, 183, "Session Progress",
    NUTAG_AUTOANSWER(0),
    TAG_END());

    return true;
    }else return DeviceAlertingReq(progress, exinfo);*/

  }

  // DeviceAlert expect remote end to generate its own local ringing to user, so for
  // now its not activate early media
  bool SofiaSIPCall::DeviceAlertingReq(CallProgress progress, CallExtraInfo* exinfo)
  {
    PLOGERR("*****IPPBX : inside SofiaSIPCall::DeviceAlertingReq()\n");


    SOFIA_SIP_CHECK_CALL(DeviceAlertingReq);
    if(progress==CP_InBandInfoAvailable)
    {
      if(_callParam.cnCalling.c_str() != NULL)
      {
        if ( strlen(_callParam.cnCalling.c_str()) >= 10)
        {
          SetLocalSDPParameter((Rtp_Media*)GetMedia());
          nua_set_hparams(nua_handle,
              SIPTAG_CONTACT_STR("sip:10.22.3.94"),
              NUTAG_EARLY_MEDIA	(1),
              NUTAG_EARLY_ANSWER(1),
              //NUTAG_ONLY183_100REL(1),
              TAG_END());
          nua_respond(nua_handle, 183, "Session Progress", 
              NUTAG_AUTOANSWER(0),
              NUTAG_EARLY_MEDIA(1),  //PUVAN -- Added for TEST RBT... Remove after Test
              NUTAG_EARLY_ANSWER(1), //PUVAN -- Added for TEST RBT... Remove after Test
              TAG_END());

          return true;
        }
      }

      nua_set_hparams(nua_handle,
          SIPTAG_CONTACT_STR("sip:10.22.3.94"),
          NUTAG_EARLY_MEDIA       (1),
          NUTAG_EARLY_ANSWER(1),
          //NUTAG_ONLY183_100REL(1),
          TAG_END());

      nua_respond(nua_handle, 183, "Session Progress",
          NUTAG_AUTOANSWER(0),
          TAG_END());

      return true;
    }
    else {
      nua_set_hparams(nua_handle,
          SIPTAG_CONTACT_STR("sip:10.22.3.94"),
          NUTAG_EARLY_MEDIA       (1),
          NUTAG_EARLY_ANSWER(1),
          //NUTAG_ONLY183_100REL(1),
          TAG_END());


      //		cout << "**** Responding with 180 **********" << endl;	
      /*		 commented for testing 25072017
             nua_respond(nua_handle, 180, "Ringing", 
             SIPTAG_CONTACT_STR("sip:10.22.3.24"),
             NUTAG_AUTOANSWER(0),
             TAG_IF(!IsUsingVirtualRtp(), NUTAG_EARLY_MEDIA(1)),
             TAG_IF(!IsUsingVirtualRtp(), NUTAG_EARLY_ANSWER(1)),
             TAG_END());
             */
      if(_callParam.cnCalling.c_str() != NULL)
      {
        //if ( strlen(_callParam.cnCalling.c_str()) >= 10)
        if ( strlen(_callParam.cnCalling.c_str()) > 0) // T 25072017 testing
        {
          SetLocalSDPParameter((Rtp_Media*)GetMedia());
          //PUVAN -- Added for TEST RBT... Remove after Test
          nua_set_hparams(nua_handle,                       
              SIPTAG_CONTACT_STR("sip:10.22.3.94"),
              NUTAG_EARLY_MEDIA       (1),
              NUTAG_EARLY_ANSWER(1),
              //NUTAG_ONLY183_100REL(1),
              TAG_END());

          nua_respond(nua_handle, 183, "Session Progress",
              NUTAG_AUTOANSWER(0),
              NUTAG_EARLY_MEDIA(1),   //PUVAN -- Added for TEST RBT... Remove after Test
              NUTAG_EARLY_ANSWER(1),  //PUVAN -- Added for TEST RBT... Remove after Test
              TAG_END());
        }
      }
    }

    return true;

    SOFIA_SIP_CHECK_CALL(DeviceAlertingReq);
    //        if(progress==CP_InBandInfoAvailable) 
    if(1) //Will send sess progress all the time
    {
      nua_set_hparams(nua_handle,
          NUTAG_EARLY_MEDIA       (1),
          NUTAG_EARLY_ANSWER(1),
          //NUTAG_ONLY183_100REL(1),
          TAG_END());
      nua_respond(nua_handle, 183, "Session Progress",
          NUTAG_AUTOANSWER(0),
          TAG_END());

      return true;
    }
    else
    {
      cout << "**** Responding with 180 -1 **********" << endl;	
      nua_respond(nua_handle, 180, "Ringing",
          NUTAG_AUTOANSWER(0),
          TAG_IF(!IsUsingVirtualRtp(), NUTAG_EARLY_MEDIA(1)),
          TAG_IF(!IsUsingVirtualRtp(), NUTAG_EARLY_ANSWER(1)),
          TAG_END());
    }

    return true; 
  }

  bool SofiaSIPCall::DeviceSendInfo(CallExtraInfo* exinfo)
  {
    CLog::Detail(THISMODULE, "SofiaSIPCall::DeviceSendInfo \n");
    SOFIA_SIP_CHECK_CALL(DeviceSendInfo);
    CallParameter *csCalledParam = GetCallParameter();
    nua_info(nua_handle,
        SIPTAG_CONTENT_TYPE_STR("application/transfer-info"),
        SIPTAG_PAYLOAD_STR("UnPark"),
        TAG_END());
    return true;
  }

  bool SofiaSIPCall::DeviceConnectReq(CallExtraInfo* exinfo)
  {
    SOFIA_SIP_CHECK_CALL(DeviceConnectReq);
    // media is started only after active notification
    // from Sofia SOA
#if 0
#ifdef XGATE_HAS_PROSODY
    /* Narsimham added */
    CallParameter *csCalledParam = GetCallParameter();
    CLog::Warning(THISMODULE, "IPPBX: Called IP =%s\n",csCalledParam->csaCalled.c_str());
    if ( csCalledParam != NULL )
    {
      nua_respond(nua_handle, 200, "OK",NUTAG_AUTOANSWER(0),SIPTAG_CONTACT_STR(csCalledParam->csaCalled.c_str()),
          TAG_IF(timerRequested, SIPTAG_REQUIRE_STR("TIMER")),
          TAG_END());
    }

#else
#endif
#endif /* End if 0 */
    /* HMP or other scenarios, this code will be executed */
#if 1 
    /* Narasimham added */
    // allow update if it's not virtual RTP
    CLog::Warning(THISMODULE, "IPPBX: During 200 OK response\n");
    CallParameter *csCalledParam = GetCallParameter();
    nua_set_hparams(nua_handle,
        SIPTAG_CONTACT_STR("sip:10.22.3.94"),
        NUTAG_ALLOW("UPDATE"),
        TAG_END());

    SetLocalSDPParameter((Rtp_Media*)GetMedia());
#endif /* Narasimham added */


    nua_respond(nua_handle, 200, "OK",
        SIPTAG_CONTACT_STR("sip:10.22.3.94"),
        NUTAG_AUTOANSWER(0),
        TAG_IF(timerRequested, SIPTAG_REQUIRE_STR("TIMER")),
        TAG_END());
    /* #endif */
    return true;
  }


  bool SofiaSIPCall::DeviceDisconnectReq(CallCause cause,CallProgress progress)
  {
    disconnectReq = true;
    disconnectReqCause = cause;

    PLOGERR("\n*****IPPBX : DeviceDisconnectReq(CallCause cause,CallProgress progress; cause = %d ; progress = %d\n", cause, progress);
    if (!nua_handle) {
      RelComInd(cause);
      return true;
    }
    SipStatusCode code;
    String reason;
    code = sip_ctengine_cause_mapping.GetSipStatus(cause, reason);

    if((sofiaCallState==nua_callstate_ready)||(sofiaCallState==nua_callstate_completed))
      nua_bye(nua_handle, TAG_END());
    else if((sofiaCallState==nua_callstate_calling)||
        (sofiaCallState==nua_callstate_proceeding)||
        (sofiaCallState==nua_callstate_completing)||
        (sofiaCallState==nua_callstate_authenticating))
      nua_cancel(nua_handle, TAG_END());
    else if((sofiaCallState==nua_callstate_received)||(sofiaCallState==nua_callstate_early))
    {
      PLOGERR("\n*****IPPBX : (int)code = %d", (int)code);
      nua_respond(nua_handle, (int)code, reason.c_str(), TAG_END());
    }
    return true;
  }

  bool SofiaSIPCall::DeviceSuspendReq()
  {
    SOFIA_SIP_CHECK_CALL(DeviceSuspendReq);
    //SOFIA_SIP_UNSUP(DeviceSuspendReq);

    InviteCall(true);
    return true;
  }

  bool SofiaSIPCall::DeviceResumeReq()
  {
    SOFIA_SIP_CHECK_CALL(DeviceResumeReq);
    //SOFIA_SIP_UNSUP(DeviceResumeReq);

    InviteCall(false, true);
    return true;
  }

  void SofiaSIPCall::SetDisconnectMsg(const char *msg)
  {
    //if (!sipCall) {
    //CLog::Debug (THISMODULE, "Null sipCall object in XgSipCall::SetDisconnectMsg");
    //return;
    //}
    //SipStatusCode code = sipCall->GetTerminationCode();
    //disconnectMessage = msg;
    //sipCall->SetTerminationReason (code, disconnectMessage);
  }

#if 0
  int SofiaSIPCall::GetAuthorizationHandle()
  {
    if (hasSipAuth)
      return (int)sipAuth;
    else
      return (int)NULL;
  }
#endif

  // set credential for outgoing call setup if required(anticipating 401/407 challenge)
  int SofiaSIPCall::SetCallCredential(CallCredential& credential)
  {
    if(StringExists(credential.crUserName.c_str()))
    {
      hasCredential=1;
      callCredential=credential;
    }else
      hasCredential=0;
    return 1;
  }

  bool SofiaSIPCall::checkPayloadCompatibility(sdp_rtpmap_t * rtpMap)
  {
    Rtp_Payload_Code payloads[100];
    Rtp_Media* med= (Rtp_Media*)GetMedia();
    int num=med->GetSupportedPayload(payloads, sizeof(payloads)/sizeof(Rtp_Payload_Code));
    sdp_rtpmap_t *p=rtpMap;
    if(!p)return false;
    while(p)
    {
      if(!stricmp(p->rm_encoding,"telephone-event"))
      {
        p=p->rm_next;
        continue;
      }

      for(int i=0;i<num;i++)
      {
        if(payloads[i]==p->rm_pt)
          return true;
      }
      p=p->rm_next;
    }
    return false;
  }

  void SofiaSIPCall::SetLocalSDPParameter(Rtp_Media* med)
  {
    ACE_INET_Addr local_addr;
    med->GetLocalAddress(local_addr);
    Rtp_Payload_Code payloads[100];
    int num=med->GetSupportedPayload(payloads, sizeof(payloads)/sizeof(Rtp_Payload_Code));
    int rx_2833_pt=((Rtp_Interface*)med->GetInterface())->DTMFGetRfc2833();

    rfc2833_rx_pt=rx_2833_pt;
    std::stringstream SDPStr;

    //template:
    /*SDPStr="v=0\r\n"\
      "o=johnston 2890844526 2890844526 IN IP4 43.32.1.5\r\n"\
      "s=IETF Update\r\n"\
      "i=This broadcast will cover the latest from the IETF\r\n"\
      "u=http://www.sipstation.com\r\n"\
      "e=Alan Johnston alan@avaya.com\r\n"\
      "p =+1-314-555-3333 (Daytime Only)\r\n"\
      "b=CT:144\r\n"\
      "t=2877631875 2879633673\r\n"\
      "c=IN IP4 225.45.3.56\r\n"\
      "m=audio 49172 RTP/AVP 0\r\n" ;
      "a=rtpmap:0 PCMU/8000\r\n"\
      "m=video 23422 RTP/AVP 31\r\n"\
      "a=rtpmap:31 H261/90000\r\n";*/
    SDPStr<<"s=SDP session\r\n";
    SDPStr<<"c=IN IP4 "<<local_addr.get_host_addr()<<"\r\n"
      <<"m=audio "<<local_addr.get_port_number()<<" RTP/AVP ";

    cout << "********** Inside SetLocalSDPParameter , c address is " << local_addr.get_host_addr() << " m port is " << local_addr.get_port_number() << endl;
    int i;
    for(i=0;i<num;i++)
      SDPStr<<payloads[i]<<" ";
    if(rfc2833_rx_pt>0)
      SDPStr<<rfc2833_rx_pt;
    SDPStr<<"\r\n";

    for(i=0;i<num;i++)
    {
      SDPStr<<"a=rtpmap:"<<payloads[i]<<" "<<Rtp_Payload_GetFormalName (payloads[i])<<"/8000\r\n";
    }

    // attributes
    // audio payload codes
    // ...
    // telephone event payload codes
    if(rfc2833_rx_pt>0)
    {
      SDPStr<<"a=rtpmap:"<<rfc2833_rx_pt<<" telephone-event/8000\r\n";
      SDPStr<<"a=fmtp:"<<rfc2833_rx_pt<<" 0-15\r\n";
    }
    SDPStr<<"a=ptime:20";
    if(_precondition)
      SDPStr<<_precondition->output(GetDirection()==CD_Incoming);

    nua_set_hparams(nua_handle, 
        SOATAG_AUDIO_AUX("telephone-event"),
        SOATAG_HOLD(""),// force sofia to put "sendrecv"
        SOATAG_ACTIVE_AUDIO(SOA_ACTIVE_SENDRECV),// force sofia to put "sendrecv"
        SOATAG_ACTIVE_VIDEO(SOA_ACTIVE_SENDRECV),// force sofia to put "sendrecv"
        SOATAG_USER_SDP_STR(SDPStr.str().c_str()), 
        TAG_END());
  }

  void SofiaSIPCall::SetLocalSDPParameter(sdp_session_s const *  sdp)
  {
    bool is_hold=true;
    if(sdp)
      if(sdp->sdp_media)
        if(sdp->sdp_media->m_port)
          if(sdp->sdp_media->m_mode>2)
            is_hold=false;

    nua_set_hparams(nua_handle, 
        SOATAG_AUDIO_AUX("telephone-event"),
        SOATAG_USER_SDP(sdp), 
        TAG_END());

    if(!is_hold)
      nua_set_hparams(nua_handle, 
          SOATAG_AUDIO_AUX("telephone-event"),
          SOATAG_HOLD(""),// force sofia to put "sendrecv"
          SOATAG_ACTIVE_AUDIO(SOA_ACTIVE_SENDRECV),// force sofia to put "sendrecv"
          SOATAG_ACTIVE_VIDEO(SOA_ACTIVE_SENDRECV),// force sofia to put "sendrecv"
          TAG_END());
  }

  void SofiaSIPCall::HandleReInvite(int status, const sip_t *sip, tagi_t tags[])
  {
    PLOGERR("\n*****IPPBX : inside SofiaSIPCall::HandleReInvite()");
    if(!IsUsingVirtualRtp())
    {
      CLog::Warning(THISMODULE, "\n*****IPPBX : SofiaSIPCall::HandleReInvite() : (!IsUsingVirtualRtp()) \n");
      nua_respond(nua_handle, 200, "OK", 
          NUTAG_AUTOANSWER(0),
          TAG_END());
    }else
    {
      if(!peerSipCall->IsValid())
      {
        PLOGERR("\n*****IPPBX : 500, Server Internal Error, Receiving re-invite while peer sip call isnt valid");

        CLog::Warning(THISMODULE, "Receiving re-invite while peer sip call isnt valid!");
        nua_respond(nua_handle, 500, "Server Internal Error", 
            TAG_END()
            );

      }else if(!peerSipCall->IsUsingVirtualRtp())
      {
        PLOGERR("\n*****IPPBX : 500, Server Internal Error, Receiving re-invite while peer sip call not using virtual RTP");
        CLog::Warning(THISMODULE, "Receiving re-invite while peer sip call not using virtual RTP!");
        nua_respond(nua_handle, 500, "Server Internal Error", 
            TAG_END()
            );
      }else
      {
        PLOGERR("\n*****IPPBX : SofiaSIPCall::HandleReInvite() : else.... peerSipCall->InviteCall();\n");
        peerSipCall->InviteCall();
      }
    }
  }

  void SofiaSIPCall::AuthenticateCall(int status,sip_www_authenticate_t const *auth)
  {
    char const *realm = NULL;
    char const *scheme = NULL;
    char authentication[500] = "";
    int indexnum;
    char *cur;

    PLOGERR("\n*****IPPBX : inside SofiaSIPCall::AuthenticateCall()");
    scheme = (char const *) auth->au_scheme;
    if (auth->au_params) {
      for (indexnum = 0; (cur = (char *) auth->au_params[indexnum]); indexnum++) {
        if ((realm = strstr(cur, "realm="))) {
          realm += 6;
          break;
        }
      }
    }
    if(hasCredential)
    {
      snprintf(authentication, sizeof(authentication),"%s:%s:%s:%s", scheme, realm, 
          callCredential.crUserName.c_str(),callCredential.crPassword.c_str());

      nua_authenticate(nua_handle, NUTAG_AUTH(authentication), TAG_END());
    }
    else
    {
      CLog::Warning(THISMODULE, "Received 401/407 challenge, but no credential provided for outgoing request");
      //DeviceDisconnectReq(CC_BearerCapabilityUnauthorized, CP_NoIndication);
      nua_authenticate(nua_handle, NUTAG_AUTH(""), TAG_END());
    }
  }

  void SofiaSIPCall::SetPrecondition(SofiaPrecondition*precond)
  {
    if(_precondition)  {
      delete _precondition;
      _precondition = NULL;
    }
    _precondition=precond;
  }

  void SofiaSIPCall::SofiaStateChanged(int status,
      char const   *phrase,
      sip_t const *sip,
      tagi_t tags[])
  {
    int offer_recv = 0, answer_recv = 0, offer_sent = 0, answer_sent = 0;
    const sdp_session_t *l_sdp=NULL;
    const sdp_session_t *r_sdp = NULL;
    bool codecIsFeasible=true;

    //	PLOGERR("********IPPBX : inside SofiaStateChanged() \n");
    //	cout << "********IPPBX : inside SofiaStateChanged()" << endl;
    if (!sip)
    {
      //RLOG		CLog::Warning(THISMODULE, "************IPPBX: sip == NULL\n");
    }
    else if (!sip->sip_to)
    {
      //		CLog::Warning(THISMODULE, "************IPPBX: sip->sip_to == NULL\n");
    }


    /***************	
      CallParameter *csCalledParam = GetCallParameter();
      PLOGERR("IPPBX : inside SofiaStateChanged() : before checking _isSwitchBoardCall\n");
      if (csCalledParam->_isSwitchBoardCall)
      {
      PLOGERR("********IPPBX : inside SofiaStateChanged() : _isSwitchBoardCall = TRUE\n");
      g_registrationApiObj_for_SwitchBoard.SwitchBoard200_OK_received = true;
      }
     ***********/

    CallProgress progress = CP_NoIndication;
    tl_gets(tags,
        NUTAG_CALLSTATE_REF(sofiaCallState),
        NUTAG_OFFER_RECV_REF(offer_recv),
        NUTAG_ANSWER_RECV_REF(answer_recv),
        NUTAG_OFFER_SENT_REF(offer_sent),
        NUTAG_ANSWER_SENT_REF(answer_sent),
        //SIPTAG_REPLACES_STR_REF(replaces_str), 
        SOATAG_LOCAL_SDP_REF(l_sdp), 
        SOATAG_REMOTE_SDP_REF(r_sdp), 
        TAG_END());

    if(offer_recv||answer_recv)
    {

      //		cout << "********IPPBX : inside SofiaStateChanged(), offer/answer received " << endl;
      if(remoteMediaAddr!=NULL)
      {
        delete remoteMediaAddr;
        remoteMediaAddr=NULL;
      }
      if(r_sdp) // quite annoying that we still have to 
      {         // check this despite offer/answer_recv=1(sofia bug?)
        progress = CP_InBandInfoAvailable;

        if(!IsUsingVirtualRtp())
        {
          codecIsFeasible=checkPayloadCompatibility(r_sdp->sdp_media->m_rtpmaps);
        }
        if(r_sdp->sdp_media&&(r_sdp->sdp_media->m_connections||r_sdp->sdp_connection))
        {
          if(r_sdp->sdp_media->m_connections)
            remoteMediaAddr= new ACE_INET_Addr(r_sdp->sdp_media->m_port, r_sdp->sdp_media->m_connections->c_address);
          else
            remoteMediaAddr= new ACE_INET_Addr(r_sdp->sdp_media->m_port, r_sdp->sdp_connection->c_address);
          // lets check RFC2833 support on remote client
          sdp_rtpmap_t *p=r_sdp->sdp_media->m_rtpmaps;
          while(p)
          {
            if(!stricmp(p->rm_encoding,"telephone-event"))
              rfc2833_tx_pt=p->rm_pt;
            p=p->rm_next;
          }
          if(r_sdp->sdp_media->m_attributes){
            if(strcmp(r_sdp->sdp_media->m_attributes->a_name,"ptime") == 0){

              cout << "****** Got ptime in remote sdp , ptime value is " << r_sdp->sdp_media->m_attributes->a_value << endl;
              // incoming_ptime = r_sdp->sdp_media->m_attributes->a_value;
              //      cout << "*********** 1111 incoming_ptime is " << incoming_ptime << endl;
              char incoming_ptime[50];

              strcpy(incoming_ptime,r_sdp->sdp_media->m_attributes->a_value);
              if(offer_recv){
                sscanf(incoming_ptime,"%d",&recv_offer_ptime);
              }else if(answer_recv){
               // sscanf(incoming_ptime,"%d",&recv_answer_ptime);
                sscanf(incoming_ptime,"%d",&recv_offer_ptime);   //Frederick
              }
            }
          } 

        }

        if(!_precondition && !IsUsingVirtualRtp())
        {
          _precondition=SofiaPrecondition::parseAndCreateSofiaPrecondition(r_sdp->sdp_media);
          if(_precondition)
          {
            _precondition->setSendReserveStatus(PreconditionLevel_No);
            _precondition->setReceiveReserveStatus(PreconditionLevel_No);
          }
        }
        else if(_precondition&& !IsUsingVirtualRtp())
        {
          _precondition->parseAndUpdate(r_sdp->sdp_media);
        }

        if(IsUsingVirtualRtp())
        {
          if(savedRemoteSdp)
            su_free(SofiaSIPSignalling::Instance()->GetSofiaHome(),savedRemoteSdp);

          savedRemoteSdp=sdp_session_dup(SofiaSIPSignalling::Instance()->GetSofiaHome(), r_sdp);
          if(peerSipCall)
          {
            if(!peerSipCall->IsValid())
              CLog::Warning(THISMODULE, "Peer call of SofiaSIPCall is not compatible or invalid!");
            else if(!peerSipCall->IsUsingVirtualRtp())
              CLog::Warning(THISMODULE, "Peer call of SofiaSIPCall is not using virtual RTP!");
            else {
              PLOGERR("Setting Local SDP \n");
              peerSipCall->SetLocalSDPParameter(savedRemoteSdp);
            }
          }
        }
      } //r_sdp
    }
    if(answer_recv||answer_sent)
    {
      bool is_hold=false;
      bool is_valid=false;
      activePayloadCode=RTP_PT_INVALID;
      sdp_media_t       *sdp_media;

      if(answer_sent&&l_sdp)
        sdp_media=l_sdp->sdp_media;
      else if(answer_recv&&r_sdp)
        sdp_media=r_sdp->sdp_media;
      else {
        PLOGERR("offer/answer failed \n");
        sdp_media=NULL;
      }

      //searching for "audio"
      while(sdp_media)
      {
        if(sdp_media->m_type==sdp_media_audio)
          break;
        else
          sdp_media=sdp_media->m_next;
      }

      //media for audio is found, now looking if its valid
      //and contains "hold" indication
      if(sdp_media)
        if(sdp_media->m_port)
        {
          is_valid=true;
          activePayloadCode=(enum Rtp_Payload_Code)sdp_media->m_rtpmaps->rm_pt;
          if(sdp_media->m_mode<3){
            is_hold=true;
          }
        }
      if(!is_valid)
      {
        PLOGERR("********IPPBX : SofiaStateChanged() ; if(!is_valid) CC_ChannelTypeNotImplemented\n");
        CLog::Warning(THISMODULE, "Invalid SDP/not supported codec received");
        DeviceDisconnectReq(CC_ChannelTypeNotImplemented, CP_NoIndication);    
        CLog::Warning(THISMODULE, "Invalid SDP/not supported codec received");
      }// Check if call is onhold/resume/session update
      else if(_precondition && !_precondition->isPreconditionMet())
      {

      }
      else if(channelIsStarted)
      {
        //check if its session update
        if(!is_hold)
        {
          CLog::Info(THISMODULE, "Session update");
          // routine to session update here
          if(((Rtp_Media*)GetMedia())->GetChannel()->GetCodec()!=activePayloadCode)
          {
            CLog::Detail(THISMODULE, "Session update,restarting media, old codec=%u, new codec=%u",
                ((Rtp_Media*)GetMedia())->GetChannel()->GetCodec(), activePayloadCode);

            StopChannel();
            //		cout << "**** channelIsStarted is true, Goingto StartChannel " << endl;
            if(!StartChannel(remoteMediaAddr, activePayloadCode)) {
              PLOGERR("********IPPBX : SofiaStateChanged() ; DeviceDisconnectReq(CC_ChannelNonExistent, CP_NoIndicatio\n");
              CLog::Warning(THISMODULE, "********IPPBX : SofiaStateChanged() ; DeviceDisconnectReq(CC_ChannelNonExistent, CP_NoIndication");
              DeviceDisconnectReq(CC_ChannelNonExistent, CP_NoIndication);
            }
          }
          // end of session update routine
        }else //hold/pause
        { 
          // pause routine here
          // update: instead abruptly stop media,
          // lets just stop specific direction(old hardware driver might not support this)

          //StopChannel();
          channelIsStarted=false;
          if(answer_recv) {
            // enable sending only
            ((Rtp_Media*)GetMedia())->enableTransmission(false, true);
            CLog::Detail(THISMODULE, "rtpMedia enableTransmission suspending : %02x \n", (Rtp_Media*)GetMedia() );
          } else {
            // enable receive only
            ((Rtp_Media*)GetMedia())->enableTransmission(false, false);
            CLog::Detail(THISMODULE, "rtpMedia enableTransmission suspending : %02x \n", (Rtp_Media*)GetMedia() );
          }
          // end of pause routine here
          OnCallSuspended();
        }
      }else // resume/new session
      {
        if(GetState()>=CS_Connected)//resume
        {
          //StartChannel(remoteMediaAddr, activePayloadCode);
          channelIsStarted=true;
          ((Rtp_Media*)GetMedia())->enableTransmission(true, true);
          CLog::Detail(THISMODULE, "rtpMedia enableTransmission receiving : %02x \n", (Rtp_Media*)GetMedia() );
          OnCallResumed();
        }else //new session
        {
          //			cout << "**** channelIsStarted is false, Goingto StartChannel " << endl;
          if(!StartChannel(remoteMediaAddr, activePayloadCode)) {
            PLOGERR("********IPPBX : SofiaStateChanged() ; DeviceDisconnectReq(CC_ChannelNonExistent, CP_NoIndication .....2\n");
            CLog::Warning(THISMODULE, "********IPPBX : SofiaStateChanged() ; DeviceDisconnectReq(CC_ChannelNonExistent, CP_NoIndication .....2");
            DeviceDisconnectReq(CC_ChannelNonExistent, CP_NoIndication);
          }
        }
      }
    }

    // call state machine (separated to media state above)
    if(sofiaCallState==nua_callstate_completing) // outgoing call established
    {
      // make sure again that call is not in state of clearing.
      // and not session update
      if(GetState()<CS_Connected)
      {
      }// if else then it is session update completing
      else
      {// we need propagating connect response to peer call(incoming), in case
        // session update
        if(IsUsingVirtualRtp()&&peerSipCall&&peerSipCall->IsValid())
          peerSipCall->DeviceConnectReq(NULL);
      }
      nua_ack(nua_handle, TAG_END());
    }else if(sofiaCallState==nua_callstate_ready) // call established(incoming and outgoing)
    {
      // make sure again that call is not in state of clearing.
      // and not session update
      if(GetState()<CS_Connected)
      {
        lastKnownCause=CC_NormalCallClearing;
        OnCallEstablished();
      }// if else then it is session update completing

    }
    else if (sofiaCallState==nua_callstate_terminated)
    {
      activePayloadCode=RTP_PT_INVALID;
      if(remoteMediaAddr) {
        delete remoteMediaAddr;
        remoteMediaAddr=NULL;
      }
      lastKnownCause = 
        sip_ctengine_cause_mapping.GetCtengineCause((SipStatusCode)status,
            phrase);
      OnCallDisconnected();

    }else if(sofiaCallState==nua_callstate_proceeding)
    {

      //		cout << "********IPPBX : inside SofiaStateChanged(), nua_callstate_proceeding  " << endl;
      if(_precondition&&!_precondition->isPreconditionMet())
      {
        SofiaPrecondition* probe=NULL;

        if(r_sdp)
          probe= SofiaPrecondition::parseAndCreateSofiaPrecondition(r_sdp->sdp_media);
        if((status!=183)||!probe) // if precondition offer answered without precondition answer,
          {                         // we assume that client doesnt support precondition
            if ( _precondition ) {
              delete _precondition;
              _precondition=NULL;
            }
            OnCallProceeding(status, sip, progress);
          }else{
            //ideally, we just set this when we get
            //notification from media manager that our media is successfully reserved.
            //but in our case, the media have been there since setupreq :D
            _precondition->setReceiveReserveStatus(PreconditionLevel_Yes);
            _precondition->setSendReserveStatus(PreconditionLevel_Yes);

            if(_precondition->isConfirmRequired())
            {
              SetLocalSDPParameter((Rtp_Media*)GetMedia());
              PLOGERR("********IPPBX : nua_update(nua_handle,NUTAG_AUTOACK()\n");
              nua_update(nua_handle,NUTAG_AUTOACK(0),TAG_END());
            }
          }
        if (probe) {
          delete probe;
          probe = NULL;
        }
      }
      else if(GetState()<CS_Connected)
        OnCallProceeding(status, sip, progress);
      else // session update proceeding
      {
        if(IsUsingVirtualRtp()&&peerSipCall&&peerSipCall->IsValid())
        {
          if(status==scProgress)
            peerSipCall->DeviceProgressReq(CP_NoIndication, NULL);
          else if(status==scRinging)
            peerSipCall->DeviceAlertingReq(CP_NoIndication, NULL);
        }
      }
    }else if(sofiaCallState==nua_callstate_received)
    {

      // re-invite
      if(GetState()>=CS_Connected)
      {
        HandleReInvite(status, sip, tags);
      }else
        if(_precondition )
        {
          SetLocalSDPParameter((Rtp_Media*)GetMedia());
          if(!_update_received)
            DeviceProgressReq(CP_NoIndication, NULL);
          else // update
          {
            //ideally, we just set there when we get
            //notification from media manager that our media is successfully reserved.
            //but in our case, the media have been there since setupreq :D
            _precondition->setReceiveReserveStatus(PreconditionLevel_Yes);
            _precondition->setSendReserveStatus(PreconditionLevel_Yes);

            nua_respond(nua_handle, 200, "OK", 
                NUTAG_AUTOANSWER(0),
                TAG_END());
          }

          // we do thing that previously deferred:
          if(_precondition->isPreconditionMet())
            OnIncomingCall();
        }

        else // first invite without precondition
        {
          if(offer_recv && !codecIsFeasible)//get invite with SDP(check codec feasibility)
            nua_respond(nua_handle, 488, "Not Acceptable Here", TAG_END());
          else
            OnIncomingCall();
        }
      _update_received=false;
    }
    PLOGERR("********IPPBX : SofiaStateChanged() END...\n");
  }

  bool SofiaSIPCall::RequireAuthorization()
  {
    Media* media=GetMedia();
    assert(media);
    if (!media)
      return false;

    Rtp_Interface *itf = (Rtp_Interface *) media->GetInterface();
    assert(itf);
    return itf->RequireSipAuthorization();
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Sofia SIP Signalling
  ///////////////////////////////////////

  SofiaSIPSignalling *SofiaSIPSignalling::instance;

  SofiaSIPSignalling * SofiaSIPSignalling::Create()
  {
    if (instance == NULL)
      instance = new SofiaSIPSignalling;

    if (SofiaRegistrar::Instance() == NULL)
      new SofiaRegistrar;

    return instance;
  }

  inline String CreateIncomingCallTableKey(sip_addr_t *const from, sip_call_id_t *const  call_id)
  {
    // table format :   user@host;tag-call_id
    std::string key;
    key=from->a_url->url_user;
    key+="@";
    key+=from->a_url->url_host;
    key+=";";
    key+=from->a_tag;
    key+="-";
    key+=call_id->i_id;
    return key;
  }

  SofiaSIPCall *SofiaSIPSignalling::FindFromIncomingCallTable(sip_addr_t *const from, sip_call_id_t *const  call_id)
  {
    // expected search complexity O(log n)
    SofiaCallTableIterator it=incomingCallTable.find(CreateIncomingCallTableKey(from, call_id));
    if(it!=incomingCallTable.end())
      return it->second;
    else
      return NULL;
  }

  void SofiaSIPSignalling::AddToIncomingCallTable(SofiaSIPCall* call, sip_addr_t *const from, sip_call_id_t *const call_id)
  {
    assert(call);

    call->callTableIterator=incomingCallTable.insert(SofiaCallTable::value_type(CreateIncomingCallTableKey(from, call_id), call)).first;
    call->callTableIterator->second=call;
  }

  void SofiaSIPSignalling::RemoveFromIncomingCallTable(SofiaSIPCall * call)
  {
    // to speed up, instead using find, we erase directly using saved iterator

    assert(call);
    assert(call->IsValid());
    //assert(call->callTableIterator);
#ifdef __linux__
    if (call->callTableIterator != (SofiaCallTableIterator) NULL) 
      incomingCallTable.erase(call->callTableIterator);
#endif
  }

  SofiaSIPSignalling::SofiaSIPSignalling()
    //: SignallingDriver(SigType_SOFIA, "SOFIA", "SOFIA(SIP) VoIP Signalling", true, Low_Priority), _sofia_thread_events(0)
    : SignallingDriver(SigType_SOFIA, "SOFIA", "SOFIA(SIP) VoIP Signalling", true, Low_Priority)
  {
    requireTimer = true;
  }

  SofiaSIPSignalling::~SofiaSIPSignalling()
  {
    instance = NULL;
  }

  void MonitorMdtrXg(void){

    char to_uri[250], from_uri[250];
    char contact_uri[250];
    nua_handle_t *handle;
    int i,j;
    //mdtrXgIps.push_back("10.22.7.86");
    //mdtrXgIps.push_back("10.30.3.25");
    //mdtrXgIps.push_back("82.113.66.114");
    //mdtrXgIps.push_back("82.113.72.51");
    mdtrXgIps.push_back("10.22.5.39");
    //mdtrXgIps.push_back("10.30.3.81");

    while(1){
      //cout << "******** Inside Monitor thread *****" << endl;

      for(int i = 0; i < mdtrXgIps.size();i++){
        sprintf(from_uri,"200 <sip:200@10.22.7.76:5060>");
        sprintf(to_uri,"<sip:200@%s:5060>",mdtrXgIps[i].c_str());
        sprintf(contact_uri,"<sip:200@10.22.7.76:5060>");

        handle = nua_handle(SofiaSIPSignalling::Instance()->GetSofiaNua(), NULL,
            SIPTAG_TO_STR(to_uri),
            SIPTAG_FROM_STR(from_uri),
            SIPTAG_CONTACT_STR(contact_uri), TAG_END());
        nua_options(handle,TAG_END());


        mdtrXg[mdtrXgIps[i]] = false;
      }

      sleep(10);
    }

  }

  bool SofiaSIPSignalling::Load()
  {
    //m_TLS = Kernel::Instance()->AllocTLS();

    CLog::Detail(SOFIADRV, "Loading Sofia SIP Signalling driver..");    

    Term_Cmd *cmd; 
    // Create "require" command.
    cmd = new Term_Cmd(this, Cmd_Require, S_REQUIRE, "Enable/disable SIP extensions", 
        PrivilegeLevelAdmin, TermModeConfig, true);
    CVariantChoiceValidator vreq(S_SES_TIMER, "Session Timer extension");
    cmd->Add (new Term_Cmd_Arg("@0", "", vreq));
    RegisterTermCmd(cmd, GetConfigTree());

    // Create "session-timer-timeout" command
    cmd = new Term_Cmd(this, Cmd_Session_Timer_Timeout, S_SES_TIMER_TIMEOUT,
        "Configure Session Timer interval",
        PrivilegeLevelAdmin, TermModeConfig, false);
    cmd->Add( new Term_Cmd_Arg("@0", 300, CVariantRangeValidator(60, 86400, "Timer in seconds")) );
    RegisterTermCmd(cmd, GetConfigTree());


    /* Add command 'setdns [hostname] [timeout]'
     **/
    Term_Cmd *cmd_testapi = new Term_Cmd (
        this,
        Sofia_Term_Cmd_Set_TestAPI,
        "setdns",
        "Test SOFIA APIs",
        PrivilegeLevelAdmin, TermModeConfig, false);

    cmd_testapi->Add (new Term_Cmd_Arg ( "API", CVariant(""), CVariantValidator("", "API")));
    cmd_testapi->Add (new Term_Cmd_Arg ( "param1", CVariant((int)0), CVariantValidator((int)0, "Test Case Number")));

    RegisterTermCmd (cmd_testapi, Kernel::Instance()->RootConfig());

    // Register show handler.
    GetConfigTree()->RegisterShowHandler(this);
    Kernel::Instance()->RootConfig()->RegisterShowHandler(this);

    // begin sofia initialization

    su_init();

    /* initialize memory handling */
    if(su_home_init(&sofia_home))
      CLog::Error(SOFIADRV, "Fail in su_home_init..");

    if(!SofiaInitNua())
    {
      //CLog::Error(SOFIADRV, "Fail initiating Sofia SIP engine..");    
      // sofia NUA creation fail
      return false;
    }

    /* 
     *	Get Current Local IP 
     */

    int sock;	

    if ( (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) > 0 ) 
    {
      char abuf[50] = "";
      struct sockaddr_in remote, out;

      memset(&remote, 0, sizeof(struct sockaddr_in));
      memset(&out, 0, sizeof(struct sockaddr_in));

      remote.sin_family = AF_INET;
      remote.sin_addr.s_addr = inet_addr("8.8.8.8");
      remote.sin_port = 53;

      if (connect(sock, (struct sockaddr *) &remote, sizeof(struct sockaddr_in)) == -1) 
      {
        // get default address
      }
      int len = sizeof(out);
      if (getsockname(sock, (struct sockaddr *) &out, &len) == -1) 
      {
        // get default address
      }
      if (out.sin_addr.s_addr != 0) 
      {
        getnameinfo((struct sockaddr *) &out, len, abuf, sizeof(out), NULL, 0, NI_NUMERICHOST);
        strncpy( sofiaHostAddress, abuf, len);		
      }

      /* closesocket(sock); */
      close(sock);
    }

    //      printf(" SofiaSIPSignalling::Load sofiaHostAddress = %s \n", sofiaHostAddress);
    if( pthread_create(&mdtrXgMonitorThread,NULL,&MonitorMdtrXg,(void *) NULL) != 0) {
      CLog::Error(THISMODULE,"Unable to start I/O thread, errno=%d",errno);
      return false;
    }

    return true;
  }

#if 1
  bool SofiaSIPSignalling::Unload()
  {
    // destroy NUA stack
    if (sofia_nua != NULL)
    {
      nua_destroy(sofia_nua);
      sofia_nua=NULL;

      /* deinit root object */
      su_root_destroy(sofia_root);
      sofia_root=NULL;

      /* deinitialize memory handling */
      su_home_deinit(&sofia_home);

      /* deinitialize system utilities */
      su_deinit();
    }

    return true;
  }
#endif

  void CreateSofiaCallParam(CallParameter &cparam, sip_t const *sip, tagi_t tags[])
  {
    char s[256];
    SofiaSIPSignalling::Instance()->InitDefaultCallParameter(cparam);
    url_t* from_url;

    PLOGERR("********IPPBX : inside CreateSofiaCallParam()\n");
    //if (sip && sip->sip_to)
    if (sip)
    {
      PLOGERR("********IPPBX : 1) TO sip->sip_to->a_url->url_user = %s\n",sip->sip_to->a_url->url_user);
      PLOGERR("********IPPBX : 1) TO sip->sip_to->a_url->url_host = %s\n",sip->sip_to->a_url->url_host);

      cparam.csaCalled = sip->sip_to->a_url->url_host; /// IPPBX added 
    }
    else
    {
      PLOGERR("********IPPBX : sip && sip->sip_to == NULL; return\n");
      return;
    }
    // Dest SIP domain

    // 	if(StringExists(sip->sip_request->rq_url->url_scheme))
    // 		cparam.csaCalled=sip->sip_request->rq_url->url_scheme;
    // 	else
    // 		cparam.csaCalled="sip";
    //	cparam.csaCalled+=":";

    //cparam.csaCalled+= sip->sip_to->a_url->url_user;
    //cparam.csaCalled+= "@";
    //cparam.csaCalled+= sip->sip_to->a_url->url_host;
    
    //TODO: Yoga, done for New-MGC PBX integration
    if(sip->sip_from->a_url) 
    {
      if(StringExists(sip->sip_from->a_url->url_host))
      {
        //cparam.routeAddr = sip->sip_request->rq_url->url_host;
        cparam.routeAddr = sip->sip_from->a_url->url_host;
        CLog::Detail(THISMODULE, "************IPPBX : CreateSofiaCallParam() : domainName: %s\n", cparam.routeAddr.c_str());
      }
    }

    if(StringExists(sip->sip_request->rq_url->url_port))
    {
      cparam.csaCalled += ':';
      cparam.csaCalled += sip->sip_request->rq_url->url_port;
    }
    //if(StringExists(sip->sip_request->rq_url->url_params))
    //{
    //	cparam.csaCalled += ";";
    //	cparam.csaCalled += sip->sip_request->rq_url->url_params;
    //}
    CLog::Detail(THISMODULE, "************IPPBX : CreateSofiaCallParam() : cparam.csaCalled.c_str() = %s\n", cparam.csaCalled.c_str());

    sip_p_asserted_identity_t * sip_pai=sip_p_asserted_identity(sip);

    // Source SIP address
    if(sip_pai)
      from_url=sip_pai->paid_url;
    else
      from_url=sip->sip_from->a_url;
    //     if(StringExists(from_url->url_scheme))
    // 	    cparam.csaCalling=from_url->url_scheme;
    // 	else
    //         cparam.csaCalling="sip";
    // 	cparam.csaCalling +=":";
    cparam.csaCalling="";
    cparam.csaCalling += from_url->url_host;
    if(StringExists(from_url->url_port))
    {
      cparam.csaCalling += ':';
      cparam.csaCalling += from_url->url_port;
    }
    //if(StringExists(sip->sip_from->a_url->url_params))
    //{
    //	cparam.csaCalling += ";";
    //	cparam.csaCalling += sip->sip_from->a_url->url_params;
    //}

    // Called Number Address & Type

    if (StringExists(sip->sip_request->rq_url->url_password)) {
      snprintf (s, sizeof(s), "%s:%s", 
          sip->sip_request->rq_url->url_user, 
          sip->sip_request->rq_url->url_password);
    } else
      snprintf (s, sizeof(s), "%s", sip->sip_request->rq_url->url_user);
    if (ParseTelephoneNumber(s, cparam.cnCalled, cparam.cntCalled))
      cparam.cnpCalled = CNP_ISDN;

    if(sip_pai){
      if(StringExists(sip_pai->paid_url->url_password))
      {
        snprintf (s, sizeof(s), "%s:%s", 
            sip_pai->paid_url->url_user, 
            sip_pai->paid_url->url_password);
      }else
        snprintf (s, sizeof(s), "%s", sip_pai->paid_url->url_user);
      if (ParseTelephoneNumber(s, cparam.cnCalling, cparam.cntCalling))
        cparam.cnpCalling = CNP_ISDN;

      if(sip->sip_privacy)
      {
        cparam.ccp=!strcmp(sip->sip_privacy->priv_values[0],"none")?CCP_Allowed:CCP_Restricted;
      }else
      {
        // temporary evaluation: only get from PAID when privacy exists(these code override PAID)
        if (StringExists(sip->sip_from->a_url->url_password))
          _snprintf (s, sizeof(s), "%s:%s", sip->sip_from->a_url->url_user, sip->sip_from->a_url->url_password);
        else
          _snprintf (s, sizeof(s), "%s", sip->sip_from->a_url->url_user);
        cparam.cnCalling="";
        if (ParseTelephoneNumber(s, cparam.cnCalling, cparam.cntCalling))
          cparam.cnpCalling = CNP_ISDN;
        // end of temporary

        cparam.ccp=CCP_Allowed;
      }

    }else
    {
      if (!strcmp(sip->sip_from->a_url->url_user,"unavailable")) {
        cparam.cnpCalling = CNP_ISDN;
        cparam.cntCalling = CNT_Unknown;
        cparam.cnCalling = "";
        cparam.ccp = CCP_NotAvailable;
      } else if (!strcmp(sip->sip_from->a_url->url_user,"anonymous")) {
        cparam.cnpCalling = CNP_ISDN;
        cparam.cntCalling = CNT_Unknown;
        cparam.cnCalling = "";
        cparam.ccp = CCP_Restricted;
      } else {
        if (StringExists(sip->sip_from->a_url->url_password)) 
          snprintf (s, sizeof(s), "%s:%s", sip->sip_from->a_url->url_user, sip->sip_from->a_url->url_password);
        else
          snprintf (s, sizeof(s), "%s", sip->sip_from->a_url->url_user);
        if (ParseTelephoneNumber(s, cparam.cnCalling, cparam.cntCalling))
          cparam.cnpCalling = CNP_ISDN;
      }
    }

    // look for user info and private info
    cparam.cui="";
    cparam.cpinfo="";
    sip_unknown_t* pUnknown=sip->sip_unknown;

    while(pUnknown)
    {
      if(!strcmp(pUnknown->un_name, XGATE_SOFIA_HEADER_USER_INFO))
        cparam.cui=SipHeaderDecode(pUnknown->un_value);
      if(!strcmp(pUnknown->un_name, XGATE_SOFIA_HEADER_PRIVATE_INFO))
        cparam.cpinfo=SipHeaderDecode(pUnknown->un_value);
      pUnknown=pUnknown->un_next;
    }

    PLOGERR("********IPPBX : DONE CreateSofiaCallParam()\n");

  }

  bool SofiaSIPSignalling::HandleIncomingInvitation(int status, nua_handle_t * nh, CallParameter *c_param, sip_t const *sip, tagi_t tags[])
  {
    // this is when signalling driver receive incoming INVITE
    // we must expand this section so the SofiaSIPClass object
    // is created and initialized
    // and do remaining task to be done (nua_respond, setup media, etc)

    // Allocate one media.
    CallParameter cparam;

    // part of overlap re-INVITE mechanism
    CLog::Detail(THISMODULE, "Incoming Invition , call from %s  to %s, call-id is %s\n", sip->sip_from->a_url->url_user, sip->sip_to->a_url->url_user, sip->sip_call_id->i_id);
    SofiaSIPCall * prevCall = FindFromIncomingCallTable(sip->sip_from, sip->sip_call_id);
    if(prevCall)
    {
      // check if prevCall's state still not reached proceeding
      if(prevCall->GetState()>=CS_Proceeding)
      {
        CLog::Warning(THISMODULE, "Incoming INVITE overlap, but the previous call has reach proceeding state");
        nua_respond(nh, 400,"Bad Request", TAG_END());
      }else
        // follow up this INVITE handle to that previous call instead
        prevCall->OverlapNuaHandle(status, nh, sip, tags);
      return false;
    }

    CreateSofiaCallParam(cparam, sip, tags);

#if 0
    c_param->HuntGroupNumber = '\0';
    c_param->HuntGroupCaller = '\0';

    for (int i=0; i < c_param->NoOfRegisteredUsers ; i++)
    {
      c_param->HuntGroupRegisteredUser[i] = '\0';
    }
    c_param->NoOfRegisteredUsers = 0;

    c_param->HuntGroupRegisteredUser_answered_call = '\0';

    c_param->HuntGroupRegisteredUser_answered_call_sent = false;
#endif


    ACE_INET_Addr remote_sig_addr;// = sipcall->GetRemoteAddress();
#if 0
    CLog::Detail(THISMODULE, "find npos starts");
    if(cparam.csaCalling.find(":")==String::npos)
    {
      remote_sig_addr.set(5060, cparam.csaCalling.c_str());
    }else
    {
      remote_sig_addr= cparam.csaCalling.c_str();
    }
#endif

    //remote_sig_addr= cparam.csaCalling.c_str();
    remote_sig_addr.set(cparam.csaCalling.c_str());
    CLog::Detail(THISMODULE, "find npos ends");
    ACE_INET_Addr local_sig_addr;   // dummy
    String addr;
    msg_t* msg=nua_current_request(SofiaSIPSignalling::Instance()->GetSofiaNua());
    get_msg_t_address(msg, addr);
    remote_sig_addr.set(addr.c_str());

    if(!sip->sip_contact)
    {
      CLog::Warning(THISMODULE, "Unable to find contact header in INVITE packet from %s", sip->sip_from->a_url->url_host);
      nua_respond(nh, 400,"Bad Request", TAG_END());
      return false;
    }else
    {
      if(!stricmp(sip->sip_contact->m_url->url_scheme,"sips"))
      {
        // for now it appears that we dont have to set
        // sofia stack when receiving SIPS invite
      }
      else if(StringExists(sip->sip_contact->m_url->url_params))
        if(strstr(sip->sip_contact->m_url->url_params,"transport=tcp"))
        {
          nua_set_hparams(nh,
              NUTAG_M_PARAMS("transport=tcp"),
              TAG_END());
        }
    }

    Rtp_Interface *itf = RtpInterfaceManager::Instance()->DispatchIncomingCall(SigType_SOFIA, remote_sig_addr, local_sig_addr, cparam);

    if(itf)
      defaultRealm = itf->GetDefaultRealm();
    else
      printf("****** itf is NULL ******\n");

    Media *med = NULL;
    if (itf==NULL || !itf->AllocateMedia(NULL, med, 0, CD_Incoming, &cparam) || med==NULL) {

      CLog::Warning(THISMODULE, "Unable to dispatch incoming Sofia SIP call from '%s'",
          sip->sip_from->a_url->url_host);
      nua_respond(nh, 500, "Server Internal Error", TAG_END());
      CLog::Warning(THISMODULE, "Unable to dispatch incoming Sofia SIP call from '%s'",sip->sip_from->a_url->url_host);
      return false;
    }

    Rtp_Media *rtp_med = (Rtp_Media*)med;

    // Create a new call.
    SofiaSIPCall *sofiaCall;
    NEW_RETURN_(sofiaCall, SofiaSIPCall(cparam, CD_Incoming, med->GetInterface()->Name().c_str()));
    if (!sofiaCall) {
      med->GetInterface()->ReleaseMedia(med, CD_Incoming, &cparam, CC_ResourceUnavailable);
      nua_respond(nh, 500, "Server Internal Error", TAG_END());
      return false;
    }
    sofiaCall->MediaAvailableInd(med);
    // when remote party ask for timer, we add "required:timer"
    if(sip->sip_session_expires)
      sofiaCall->requestSessionTimer(TRUE);
    else
      sofiaCall->requestSessionTimer(FALSE);


    // we must NUTAG_AUTOANSWER(0) again despite this already done
    // in higher, general nua parameters (sofia's bug)
    nua_set_hparams(nh, NUTAG_AUTOANSWER(0), TAG_IF(!(sofiaCall->IsUsingVirtualRtp()), NUTAG_ALLOW("UPDATE")), TAG_END());

    sofiaCall->attachSofiaNuaHandle(nh);
    AddToIncomingCallTable(sofiaCall, sip->sip_from, sip->sip_call_id);

#if 0 //CHANGE2308

    bool has_auth;
    Sofia_Reg_Info auth;

    has_auth=GetSipAuthEntryFromSofiaSip(auth, sip);
    if(has_auth==false)
    {
      if(sofiaCall->RequireAuthorization())
      {
        SofiaSIPSignalling::ChallengeRequest(nh, 401, defaultRealm.c_str());
        sofiaCall->attachSofiaNuaHandle(NULL);
        med->GetInterface()->ReleaseMedia(med, CD_Incoming, &cparam, CC_FacilityRejected);
        return false;
      }
#ifdef XGATE_HAS_SIP_IPPBX
      // following code added by IPPBX
      if (! strstr(cparam.csaCalling.c_str(), (const char *) "10.30.3.25"))
      {
        CLog::Warning(THISMODULE, "cparam.csaCalling NOT containing mundio.com");
        SofiaSIPSignalling::ChallengeRequest(nh, 407, defaultRealm.c_str());
      }
      return false;
#endif // XGATE_HAS_SIP_IPPBX
    }else
    {
      if(sofiaCall->RequireAuthorization())
        if (!itf->Authorize(auth.authUser)) {
          nua_respond(nh, 403, "Forbidden", NUTAG_AUTOANSWER(0), TAG_END());
          sofiaCall->attachSofiaNuaHandle(NULL);
          med->GetInterface()->ReleaseMedia(med, CD_Incoming, &cparam, CC_FacilityRejected);
          return false;
        }
    }
#endif 

    if(!sofiaCall->IsUsingVirtualRtp())
    {
      cout << "**** Sofia call is using virtualrtp, setting the localsdp parameter **** " << endl;
      sofiaCall->SetLocalSDPParameter(rtp_med);
    }

    return true;
  }

  void SofiaSIPSignalling::ChallengeRequest(nua_handle_t* nh, int status, const char* realm, bool stale, msg_t*ref)
  {
    char auth_str[1000];
    if(!nh)
    {
      assert(false);
      return;
    }

    snprintf(auth_str, sizeof(auth_str),"Digest realm=\"%s\", nonce=\"%s\",%s algorithm=md5, qop=\"auth\"", realm, SofiaRegistrar::Instance()->CreateChallenge().c_str(), stale ? " stale=true," : "");

    if(status==401)
    {
      nua_respond(nh, 401, "Unauthorized: Authentication needed.",
          NUTAG_AUTOANSWER(0),
          SIPTAG_WWW_AUTHENTICATE_STR(auth_str),
          TAG_IF(ref,NUTAG_WITH(ref)),
          TAG_END());
    }else if(status==407)
    {
      nua_respond(nh, 407, "Unauthorized: Proxy Authentication needed.",
          NUTAG_AUTOANSWER(0),
          SIPTAG_PROXY_AUTHENTICATE_STR(auth_str),
          TAG_IF(ref,NUTAG_WITH(ref)),
          TAG_END());
    }else
    {
      CLog::Warning(THISMODULE, "Unknown status code given in function RequestChallenge()!");
    }
  }

  bool SofiaSIPSignalling::HandleIncomingMessage(
      int           status,
      char const   *phrase,
      nua_t        *nua,
      nua_magic_t  *magic,
      nua_handle_t *nh,
      nua_hmagic_t *hmagic,
      sip_t const  *sip,
      tagi_t        tags[])
  {
    //CLog::Warning("received MESSAGE: %03d %s\n", status, (char*) phrase);

    CLog::Detail("From: %s%s" URL_PRINT_FORMAT "\n",
        sip->sip_from->a_display ? sip->sip_from->a_display : "",
        sip->sip_from->a_display ? " " : "",
        URL_PRINT_ARGS(sip->sip_from->a_url));

    if (sip->sip_subject) {
      CLog::Detail("Subject: %s\n", sip->sip_subject->g_value);
    }

    if (sip->sip_payload) {		
      CLog::Detail("Message: %s \nLength: %d", sip->sip_payload->pl_data, sip->sip_payload->pl_len);
    }

    return true;

  }

  void SofiaSIPSignalling::Sbc_Outgoing_SendRegister(char *to_uri,char *from_uri,char *contact_uri)
  {
    nua_handle_t *reg_handle;
    CLog::Detail( "Sofia Signalling", "Entering Sbc_Outgoing_SendRegister");

    reg_handle = nua_handle(this->sofia_nua,
        NULL,SIPTAG_TO_STR(to_uri),
        SIPTAG_FROM_STR(from_uri),
        SIPTAG_CONTACT_STR(contact_uri),
        TAG_END());


    nua_register(reg_handle,TAG_END());
    CLog::Detail( "Sofia Signalling", "Exiting Sbc_Outgoing_SendRegister");

  }

  bool SofiaSIPSignalling::HandleIncomingRegistration(int status, nua_handle_t * nh, sip_t const *sip, tagi_t tags[])
  {
    CLog::Detail( "Sofia Signalling", "Entering HandleIncomingRegistration function");

#if 0
    SofiaRegisterRequest sofiaRegReq(nh);

    /* Handle New registration message */
    sofiaRegReq.RegistrationHandling(sip,tags, nua_current_request(SofiaSIPSignalling::Instance()->GetSofiaNua()));


    //#else
    char to_uri[250], from_uri[250],im_to_uri[250];
    char im_from_uri[250];
    char contact_uri[500];
    char registrar_ip[30] = "10.22.2.254";

    sprintf(from_uri,"%s <sip:%s@%s>",sip->sip_from->a_display,sip->sip_from->a_url->url_user,sip->sip_from->a_url->url_host);
    sprintf(to_uri,"sip:%s@%s:5060",sip->sip_to->a_url->url_user,registrar_ip);

    if ( sip->sip_contact->m_url->url_params != NULL ){
      if(sip->sip_contact->m_url->url_port != NULL )   {
        sprintf(contact_uri,"\<sip:%s@%s:%s;%s\>",sip->sip_from->a_url->url_user,sip->sip_contact->m_url->url_host,sip->sip_contact->m_url->url_port,sip->sip_contact->m_url->url_params);
        CLog::Detail("SIP contact uri to media server =%s",contact_uri);
      }
    }
    else if ( sip->sip_contact->m_url->url_host != NULL && sip->sip_contact->m_url->url_port != NULL ) {
      sprintf(contact_uri,"sip:%s@%s:%s",sip->sip_from->a_url->url_user,sip->sip_contact->m_url->url_host,sip->sip_contact->m_url->url_port);
    }
    else if (sip->sip_contact->m_url->url_host != NULL) {
      sprintf(contact_uri,"sip:%s@%s",sip->sip_from->a_url->url_user,sip->sip_contact->m_url->url_host);
    }

    if (strcasestr(sip->sip_from->a_url->url_host,"PB.mundio.com") || strcasestr(sip->sip_from->a_url->url_host,"UR.mundio.com")) {
      CLog::Detail(THISMODULE,"Registration:: Sbc_Outgoing_SendRegister called for PB.mundio.com");
      Sbc_Outgoing_SendRegister(to_uri,from_uri,contact_uri);
    }
    else {
      nua_respond(nh,403, "Forbidden",
          NUTAG_AUTOANSWER(0),
          SIPTAG_CONTACT_STR(contact_uri),
          SIPTAG_EXPIRES(sip->sip_expires),
          SIPTAG_DATE(sip->sip_date),NUTAG_WITH( nua_current_request(SofiaSIPSignalling::Instance()->GetSofiaNua())), TAG_END());
      CLog::Detail("Registration dropped due to missing transport type =%s",contact_uri);
      return;
    }
#else
    /* Final 200 OK registration to SIP Phone */
    /* Respond to User A with 200 OK */
    string contact_uri;
    contact_uri += "sip:";
    contact_uri += sip->sip_from->a_url->url_user;
    contact_uri += "@10.22.3.94:5060";
    nua_respond(nh, SIP_200_OK,
        NUTAG_AUTOANSWER(0),
        SIPTAG_CONTACT_STR(contact_uri.c_str()),
        SIPTAG_EXPIRES(sip->sip_expires),
        SIPTAG_DATE(sip->sip_date),NUTAG_WITH( nua_current_request(SofiaSIPSignalling::Instance()->GetSofiaNua())), TAG_END());

#endif

    CLog::Detail( "Sofia Signalling", "Exiting HandleIncomingRegistration function");
    return true;
  }

  void SofiaLogger(void *logarg, char const *format, va_list ap)
  {
    char buffer[1000];
    vsprintf(buffer, format, ap);

    CLog::Detail(THISMODULE, "%s", buffer);
  }

  bool SofiaSIPSignalling::SofiaInitNua(void)
  {
    SofiaSIPSignalling* _instance=this;
    std::string tag_url;
    std::string tag_sips_url;
    char tmp[40];
    sip_from_t from[1];

    CLog::Warning(THISMODULE, "IPPBX : inside SofiaSIPSignalling::SofiaInitNua() ***************************\n");
    //tag_url="sip:*:";
    tag_url="sip:10.22.3.94:"; // T 25042017
    //tag_url="sip:10.22.3.94:";
    //sip_from_init(from);
#ifndef __linux__
    tag_url+=itoa(SOFIA_SIP_PORT,tmp,10);
#else
    sprintf(tmp,"%d",SOFIA_SIP_PORT);
    tag_url+= tmp;
#endif
    ///	tag_sips_url="sips:*:";
    tag_sips_url="sips:10.22.3.94:"; // T 25042017
    //tag_sips_url="sips:10.22.3.94:";
#ifndef __linux__
    tag_sips_url+=itoa(SOFIA_SIP_PORT+1,tmp,10);
#else
    sprintf(tmp,"%d",SOFIA_SIP_PORT+1);
    tag_sips_url+= tmp;
#endif
    tag_sips_url+=";transport=tls";

    // initialize root object
    _instance->sofia_root = su_root_create(_instance);

    sip_update_default_mclass(sip_extend_mclass(NULL));
#ifndef __linux__
    su_log_redirect(NULL, &SofiaLogger, NULL);
    su_log_set_level(NULL,9);
#endif
    /* IPPBX added */
    //from->a_url->url_host = getLocalInterface();
    //printf("IPPBX: From Url Host in SofiaInitNua =%s\n",from->a_url->url_host);

    if (_instance->sofia_root != NULL) {
      // create NUA stack
      _instance->sofia_nua = nua_create(_instance->sofia_root,
          &SofiaSIPSignalling::SofiaCallback,
          _instance,
          //SIPTAG_FROM(from),
          // tags as necessary ...
          NUTAG_URL(tag_url.c_str()),
#ifdef HAS_TLS
          NUTAG_SIPS_URL(tag_sips_url.c_str()),
          NUTAG_CERTIFICATE_DIR("/etc/xgate/agent.pem"),
#endif
          //TPTAG_CERTIFICATE("..\\etc"),
          TAG_END());
      CLog::Warning(THISMODULE, "Rajeshhhhhh............After SofiaCallback Function  =%d \n",_instance->sofia_nua);      
      if(!_instance->sofia_nua)
        CLog::Error(SOFIADRV, "Fail in nua_create..");
      nua_set_params(_instance->sofia_nua,
          SIPTAG_ALLOW_STR("INVITE, ACK, BYE, CANCEL, OPTIONS, MESSAGE, INFO"),
          //SIPTAG_SUPPORTED_STR("precondition"), // commented by IPPBX
          SIPTAG_USER_AGENT_STR(XGATE_USER_AGENT),
          NUTAG_AUTOANSWER(0),
          NUTAG_AUTOACK(1),
          NUTAG_AUTOALERT(0),
          /*NUTAG_ACCEPT_MULTIPART(1),*/
          //NUTAG_ENABLEMESSENGER(1),
          NTATAG_EXTRA_100(0),
          //NUTAG_ALLOW("UPDATE"),
          NUTAG_ALLOW("REGISTER"),
          NUTAG_ALLOW("REFER"), // "REFER" is required during call-transfer.
          NUTAG_ALLOW("PRACK"),
          NUTAG_ALLOW("INVITE"),
          NUTAG_ALLOW("ACK"),
          NUTAG_ALLOW("CANCEL"),
          NUTAG_ALLOW("OPTIONS"),
          NUTAG_ALLOW("BYE"),
          //NUTAG_ALLOW("INFO"),
          //NUTAG_ALLOW("NOTIFY"),
          NUTAG_ALLOW_EVENTS("talk"),
          NUTAG_ALLOW_EVENTS("hold"),
          NUTAG_ALLOW_EVENTS("conference"),
          //NUTAG_APPL_METHOD("OPTIONS"),
          //NUTAG_APPL_METHOD("REFER"),
          NUTAG_APPL_METHOD("REGISTER"),
          //NUTAG_SESSION_TIMER(1800), // commented by IPPBX
          //NUTAG_SUPPORTED("TIMER"), // commented by IPPBX
          //NUTAG_APPL_METHOD("NOTIFY"), 
          //NUTAG_APPL_METHOD("INFO"), 
          //NUTAG_APPL_METHOD("ACK"), 
          //NUTAG_APPL_METHOD("SUBSCRIBE"),
          //NUTAG_APPL_METHOD("MESSAGE"),
          //NUTAG_SESSION_TIMER(profile->session_timeout),
          //NTATAG_MAX_PROCEEDING(profile->max_proceeding),
          //NUTAG_ALLOW("PUBLISH"),
          //NUTAG_ALLOW("SUBSCRIBE"),
          //NUTAG_ENABLEMESSAGE(1),
          //NUTAG_ALLOW_EVENTS("presence"),
          //NUTAG_ALLOW_EVENTS("dialog"),
          //NUTAG_ALLOW_EVENTS("line-seize"),
          //NUTAG_ALLOW_EVENTS("call-info"),
          //NUTAG_ALLOW_EVENTS("sla"),
          //NUTAG_ALLOW_EVENTS("include-session-description"),
          //NUTAG_ALLOW_EVENTS("presence.winfo"),
          //NUTAG_ALLOW_EVENTS("message-summary"),
          TAG_END());
    }else
    {
      CLog::Error(SOFIADRV, "Fail in su_root_create..");
      _instance->sofia_nua=NULL;
    }
#ifndef __linux__
    su_log_set_level(NULL,3);
#endif
    if (_instance->sofia_nua != NULL) 
    {
      // set necessary parameters
      //nua_set_params(_instance->sofia_nua,
      // tags as necessary ... 
      //			   TAG_NULL());

      // mark thread as active
      //_instance->_sofia_thread_active=true;

      // release thread creation event
      //_instance->_sofia_thread_events.release();

      // enter main loop for processing of messages
      //su_root_run(_instance->sofia_root);

      // destroy NUA stack 
      //nua_destroy(_instance->sofia_nua);

      /* deinit root object */
      //su_root_destroy(_instance->sofia_root);
      //_instance->sofia_root = NULL;
    }else return false;

    // release thread destroy event
    //_instance->_sofia_thread_active=false;
    //_instance->_sofia_thread_events.release();
    return true;
  }

  void SofiaSIPSignalling::SofiaCallback(nua_event_t   event,  
      int           status,  
      char const   *phrase,  
      nua_t *nua,  
      nua_magic_t  *magic,  
      nua_handle_t *nh,  
      nua_hmagic_t *hmagic,  
      sip_t const  *sip,  
      tagi_t        tags[])
  {
    SofiaSIPSignalling * sigdriver=(SofiaSIPSignalling *)magic;
    SofiaSIPCall * sofiaCall=(SofiaSIPCall *) hmagic;
    CallParameter *c_param = NULL;
    hg_data *h_data = NULL;

    if(sigdriver)
    {
      //            c_param = sigdriver->getCparam();
    }

    //RLOG	CLog::Detail(THISMODULE, "Received Event from Sofia Stack =%d",event); 
    if (!sip)
    {
      //		CLog::Warning(THISMODULE, "************IPPBX: sip == NULL\n");
    }
    else if (!sip->sip_to)
    {
      //		CLog::Warning(THISMODULE, "************IPPBX: sip->sip_to == NULL\n");
    }

    switch (event) {

      case nua_i_invite: // Incoming call INVITE
        //                printf("case nua_i_invite status = %d \n", status);
        cout << "\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< " << sip->sip_from->a_url->url_user << " Calling " << sip->sip_to->a_url->url_user << " <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n" << endl ;

        if(sofiaCall==NULL)
        {
          sigdriver->HandleIncomingInvitation(status, nh, c_param, sip, tags);
        }
        else if(sofiaCall->IsValid())
        {
          // for better coherency this will be invoked from state
          // transition(sofiastatechanged())
          //sofiaCall->HandleReInvite(status, sip, tags);
          char buff[24];
          int idx = 0;
          int key_chk = key_not_exist;
          if (sip && sip->sip_to && sip->sip_to->a_url && sip->sip_to->a_url->url_user && sip->sip_from &&
              sip->sip_from->a_url && sip->sip_from->a_url->url_user)
          {
            sprintf(buff, "%s_%s", sip->sip_from->a_url->url_user, sip->sip_to->a_url->url_user);

            for(idx= 0; idx < usr_key.size(); idx++)
            {
              if(buff == usr_key[idx])
              {
                key_chk = key_exist;
                break;
              }
            }
            if(key_chk == key_not_exist)
            {
              usr_key.push_back(buff);
            }
          }
        }
        else
        {
          CLog::Warning(THISMODULE, "Got INVITE with invalid sofiaCall object!", phrase);
        }
        break;

      case nua_r_invite: // Answer to outgoing INVITE
        CLog::Warning(THISMODULE, "************PROSODY : case nua_r_invite:\n");
        //                printf("case nua_r_invite status = %d \n", status);

        if (sip && sip->sip_to && sip->sip_to->a_url && sip->sip_to->a_url->url_user && status == 200 ) {
          // DO PROCESSING ONLY FOR THIS HUNTGROUP..
          /******************
            Is this called-party who accepted the call and sent 200 OK --is on active Hunt-group call?
            if yes, then inform about this called-party to the SL of hunt-group -which is waiting to 
            know about this called-party who accepted the call.
            check the global list of current active hunt group-calls. if this called-party found in active 
            hunt group-calls, then this called-party has accepted call on an active hunt group-call.
           ******************/

          char key[32] ;
          char *str_host = sip->sip_from->a_url->url_host;
          char str_dev[4];// = "ext";

          if(strlen(sip->sip_to->a_url->url_user) == 3)
          {
            if ((sip->sip_from->a_url->url_user != NULL) && (sip->sip_to->a_url->url_user != NULL)) {
              strncpy(str_dev, sip->sip_from->a_url->url_host, 3);
              str_dev[3] ='\0';
              sprintf(key, "%s%s%s", sip->sip_from->a_url->url_user, sip->sip_to->a_url->url_user, str_dev);
            } else {
              sprintf(key, "%s" , "dummyextn");
            }

            c_param = call_info[key];
            CLog::Detail(THISMODULE, "********IPPBX Callback EXTENSION: key = %s, c_param = %02x\n", key, c_param);
          }
          else if(strlen(sip->sip_to->a_url->url_user) > 5)
          {
            if (sip->sip_to->a_url->url_user !=NULL) {
              strncpy(str_dev, sip->sip_from->a_url->url_host, 3);
              str_dev[3] ='\0';
              sprintf(key, "%s%s", sip->sip_to->a_url->url_user, str_dev);
            } else {
              sprintf(key, "%s" , "dummymob");
            }

            c_param = call_info[key];
            CLog::Detail(THISMODULE, "********IPPBX Callback MOBILE: key = %s, c_param = %02x\n", key, c_param);
          }
          else
          {
            c_param = NULL;
          }

          if(c_param != NULL)
          {
            h_data = hg_info[c_param];

            if(h_data != NULL)
            {
              CLog::Detail(THISMODULE, "\n ERROR :  H_DATA NOT NULL.... FROM : %s and TO: %s event = %d\n",sip->sip_from->a_url->url_user,sip->sip_to->a_url->url_user, event);
              CLog::Detail(THISMODULE, "********IPPBX : h_data->NoOfRegisteredUsers = %d\n", h_data->NoOfRegisteredUsers);

              int i = 0; // T HG_New
              char cmp_key[32] ;
              if(h_data->NoOfRegisteredUsers > MAX_HUNTGROUP_USERS_REGISTERED_TO_HUNTGROUP_NUMBER)h_data->NoOfRegisteredUsers=0;

              for ( ; i <= h_data->NoOfRegisteredUsers ; i++) // T HG_New
              {
                if(h_data->HuntGroupRegisteredUser[i] != NULL)
                {
                  strncpy(str_dev, sip->sip_from->a_url->url_host, 3);
                  str_dev[3] ='\0';
                  CLog::Warning(THISMODULE, "************IPPBX: str_dev %s \n", str_dev);
                  if (strstr(str_dev,"ext")|| strstr(str_dev,"and")|| strstr(str_dev,"ios")){
                    sprintf(cmp_key,"%s%s",sip->sip_to->a_url->url_user,str_dev);
                    CLog::Warning(THISMODULE, "************1 IPPBX: cmp_key %s \n", cmp_key);
                  } else {
                    sprintf(cmp_key,"%s",sip->sip_to->a_url->url_user);
                    CLog::Warning(THISMODULE, "************2 IPPBX: cmp_key %s \n", cmp_key);
                  }
                  CLog::Warning(THISMODULE, "************IPPBX: h_data->HuntGroupRegisteredUser[i] = %s and cmp_key = %s ...............\n", h_data->HuntGroupRegisteredUser[i], cmp_key);
                  //
                  if ((strcmp(cmp_key, h_data->HuntGroupRegisteredUser[i]) == 0) ||
                      cmp_key == h_data->HuntGroupRegisteredUser[i])
                  {
                    CLog::Warning(THISMODULE, "********IPPBX : This is 200 OK from registered user %s ANSWERED HUNT-GROUP-CALL***************", h_data->HuntGroupRegisteredUser[i]);

                    h_data->HuntGroupRegisteredUser_answered_call_sent = true;
                    strcpy(h_data->HuntGroupRegisteredUser_answered_call, sip->sip_to->a_url->url_user);
                    h_data->CallLegOfRegisteredUser_answered_call = i ; // T HG_New
                    break;
                  }
                }
              }
            } else {
              CLog::Detail(THISMODULE, "h_data == NULL \n");
            }
          }
#ifdef CALL_TRANSFER
          to_tag[sip->sip_to->a_url->url_user] = sip->sip_to->a_tag;
          from_tag[sip->sip_to->a_url->url_user] = sip->sip_from->a_tag;
          if (sip->sip_call_id != NULL) {
            call_id[sip->sip_to->a_url->url_user] = sip->sip_call_id->i_id;
          }
          to_address[sip->sip_to->a_url->url_user] = sip->sip_to->a_url->url_host;
          // T 18122017
          char CT_header_st[1000]="";
          char CT_usr_key[100]="";
          char delimiters[] = "?";
          char *token;

          sip_unknown_t* tUnknown=sip->sip_unknown;
          if (tUnknown && strcasestr(tUnknown->un_name,"Trans_Replace")) {
            sprintf(CT_header_st,"%s",tUnknown->un_value);
            token = strtok(CT_header_st, "?");	
            while(token != NULL)
            {
              printf(" 1 CT_header_st , token = %s \n", token);
              sprintf(CT_usr_key,"%s",token);
              token = strtok(NULL, "?");	
              printf(" 2 CT_header_st , token = %s \n", token);
              sprintf(CT_header_st,"%s",token);
              break;
            }

            CLog::Detail(THISMODULE, "Trans_Replace : CT_usr_key : %s ; CT_header_st : %s \n", CT_usr_key, CT_header_st );
            printf( "REPLACES when 200 OK received : CT_usr_key : %s ; CT_header_st : %s \n", CT_usr_key, CT_header_st );
            call_transfer_replace[CT_usr_key] = CT_header_st;
            printf(" ^^^^^^^^ call_transfer_replace entry %s \n", CT_usr_key);
            printf( "REPLACES when 200 OK received : call_transfer_replace : %s \n", call_transfer_replace[CT_usr_key].c_str());
          }

#endif
        } // 200 OK check

        // we dont deal with specific invitation respond anymore,
        // for coherency, all call state transition will be focused on sofiastatechanged()
        if(!sofiaCall->IsValid())
        {
          nua_respond(nh, SIP_406_NOT_ACCEPTABLE, 
              NUTAG_AUTOANSWER(0),
              TAG_END());
          nua_handle_destroy(nh);
          CLog::Warning(THISMODULE, "incoming spurious respond!");
        }else
        {
          if(sofiaCall->GetDirection()==CD_Outgoing)
          {
            if((status==scUnauthorized)||(status==scProxyAuthenticationRequired))
            {
              if(sip->sip_www_authenticate)
                sofiaCall->AuthenticateCall(status, sip->sip_www_authenticate);
              else if(sip->sip_proxy_authenticate)
                sofiaCall->AuthenticateCall(status, sip->sip_proxy_authenticate);
              else
              {
                PLOGERR("********IPPBX : SofiaStateChanged() ; DeviceDisconnectReq(CC_CallHasBeenCleared\n");
                CLog::Warning(THISMODULE, "********IPPBX : SofiaStateChanged() ; DeviceDisconnectReq(CC_CallHasBeenCleared");
                sofiaCall->DeviceDisconnectReq(CC_CallHasBeenCleared, CP_NoIndication);
                break;
              }
              //sofiaCall->InviteCall();
            }
          }

        }

        break;

      case nua_i_error:
        CLog::Warning(THISMODULE, "Got nua_i_error event callback on Sofia engine! msg: %s", phrase);
        break;
      case nua_i_cancel: 
        PLOGERR("sofia.cpp : case nua_i_cancel: break;\n");
        printf("case nua_i_cancel status = %d \n", status);
        CLog::Warning(THISMODULE, "sofia.cpp : case nua_i_cancel: break;");
        break;
      case nua_i_ack: break;
      case nua_i_fork:
                      PLOGERR("sofia.cpp : case nua_i_fork: break;\n");
                      CLog::Warning(THISMODULE, "sofia.cpp : case nua_i_fork: break;");
                      break;
      case nua_i_active: 
                      sigdriver->HandleIncomingActive(status, nh, sip, tags);
                      break;
      case nua_i_terminated:
                      //			PLOGERR("sofia.cpp : case nua_i_terminated: break; status = %d\n", status);
                      //              printf("case nua_i_terminated status = %d \n", status);
                      CLog::Warning(THISMODULE, "sofia.cpp : case nua_i_terminated: break;");
                      if (!sip)
                      {
                        //				PLOGERR ("************IPPBX: sip == NULL\n");
                        //				CLog::Warning(THISMODULE, "************IPPBX: sip == NULL\n");
                      }
                      break;
      case nua_i_state:
                      if(sofiaCall && sofiaCall->IsValid())
                      {
                        sofiaCall->SofiaStateChanged(status, phrase, sip, tags);
                      }
                      break;
      case nua_i_outbound: break;
      case nua_i_bye:
                           printf("case nua_i_bye status = %d \n", status);
                           if (sip && sip->sip_from && sip->sip_from->a_url && sip->sip_from->a_url->url_user && sip->sip_to && sip->sip_to->a_url && sip->sip_to->a_url->url_user) {
                             int i=0;
                             char key[24] ;
                             conf_host_data *c_data = NULL;
                             sprintf(key, "%s%s", sip->sip_to->a_url->url_user,sip->sip_from->a_url->url_user);
                             c_param = call_info[key];
                             if(c_param != NULL) {
                               c_data = conf_info[c_param];
                               if(c_data != NULL) {
                                 CLog::Warning(THISMODULE, "case nua_i_bye c_data = %02x , c_data->NoOfDialInParticipants = %d \n", c_data, c_data->NoOfDialInParticipants);
                                 for ( ; i <= c_data->NoOfDialInParticipants ; i++)
                                 {
                                   if (c_data->conf_di_data[i]->DialIn_Participant_RegisteredUser != NULL)
                                   {
                                     CLog::Warning(THISMODULE, "case nua_i_bye c_data->conf_di_data[%d]->DialIn_Participant_RegisteredUser = %s \n", i, c_data->conf_di_data[i]->DialIn_Participant_RegisteredUser);
                                     if ((strcmp(sip->sip_from->a_url->url_user, c_data->conf_di_data[i]->DialIn_Participant_RegisteredUser) == 0) ||
                                         (sip->sip_from->a_url->url_user == c_data->conf_di_data[i]->DialIn_Participant_RegisteredUser))
                                     {
                                       CLog::Warning(THISMODULE, " BYE FROM %d DIAL-IN PARTICIPANT %s \n",i, c_data->conf_di_data[i]->DialIn_Participant_RegisteredUser);
                                       c_data->dialIn_Disc_Leg = i ;
                                     }
                                   }
                                 }
                               }
                             }
                           }
                           sigdriver->HandleIncomingBye(status, nh, sip, tags);
                           break;
      case nua_i_options: break;
      case nua_i_refer: 
                          // REFER is used during call-transfer.
                          //CLog::Warning(THISMODULE, "*******IPPBX : case nua_i_refer : received incoming REFER , FROM = %s, TO = %s, REFER = %s, sip->sip_from->a_url->url_user, sip->sip_to->a_url->url_user, sip->sip_refer_to->r_url->url_user\n");
                          {
                            CLog::Warning(THISMODULE, "*******IPPBX : case nua_i_refer : received incoming REFER\n");
                            if(sigdriver)
                            {
                              sigdriver->HandleIncomingRefer(status, nh, sip, tags);
                            }

                            if(sofiaCall)
                            {
                              c_param = sofiaCall->GetCallParameter();
                              if (strstr(sip->sip_refer_to->r_url->url_headers, "Replaces"))
                              {
                                CLog::Detail(THISMODULE, "sofiaCall->OnCallTransfer \n", sip->sip_refer_to->r_url->url_headers);
                                sofiaCall->OnCallTransfer();
                              } else
                              {
                                sip_unknown_t* pUnknown=sip->sip_unknown;
                                int transfer_flag = 0;
                                int switch_to_network = 0;
                                while(pUnknown)
                                {
                                  if (strstr(pUnknown->un_name,"X-transfertype")) {
                                    CLog::Detail(THISMODULE, "X-transfertype HEADER PRESENT");
                                    if ( strcasestr(pUnknown->un_value,"CallTransfer")) {
                                      CLog::Detail(THISMODULE, "X-transfertype: CallTransfer received");
                                      transfer_flag = 1;
                                      switch_to_network = 0;
                                      c_param->transferType = "CallTransfer";
                                      call_transfer_extn[sip->sip_from->a_url->url_user] = sip->sip_refer_to->r_url->url_user; //CALL TRANSFER
                                    }
                                    else if ( strcasestr(pUnknown->un_value,"SwitchToNetwork")) {
                                      CLog::Detail(THISMODULE, "X-transfertype: SwitchToNetwork received");
                                      transfer_flag = 0;
                                      switch_to_network = 1;
                                      c_param->transferType = "SwitchToNetwork";
                                      call_transfer_extn[sip->sip_from->a_url->url_user] = sip->sip_refer_to->r_url->url_user; //CALL TRANSFER
                                    }
                                  }
                                  pUnknown=pUnknown->un_next;
                                }
                                if ((transfer_flag == 1) || (switch_to_network == 1))
                                {
                                  CLog::Detail(THISMODULE, "sofiaCall->OnCallUnattendTransfer \n");
                                  sofiaCall->OnCallUnattendTransfer();
                                }
                              }
                            }
                          }
                          break;
      case nua_i_publish: break;
      case nua_i_prack: break;

      case nua_i_info:
                        sigdriver->HandleIncomingInfo(status, nh, sip, tags, sofiaCall);
                        break;
      case nua_i_update: 
                        if(sofiaCall && sofiaCall->IsValid())
                          sofiaCall->MarkUpdate();
                        break;
      case nua_i_message: 
                        CLog::Warning(THISMODULE, "case nua_i_message:...............\n");
                        CLog::Detail( "Sofia Signalling", "got nua_i_message event.nua_handle: %d\n", (int) nh);			
                        sigdriver->HandleIncomingMessage(status, phrase, nua, magic, nh, hmagic, sip, tags);
                        break;
      case nua_i_chat: break;
      case nua_i_subscribe: break;
      case nua_i_subscription: break;
      case nua_i_notify: break;
      case nua_i_method: break;
      case nua_i_media_error:
                         PLOGERR("sofia.cpp : case nua_i_media_error: break;\n");
                         CLog::Warning(THISMODULE, "sofia.cpp : case nua_i_media_error: break;");
                         break;
      case nua_r_set_params: break;
      case nua_r_get_params: break;
      case nua_r_shutdown: break;
      case nua_r_notifier: break;
      case nua_r_terminate: break;
      case nua_r_authorize: break;
      case nua_r_register: break;
      case nua_r_unregister: break;
      case nua_r_cancel: break;
      case nua_r_bye: break;
      case nua_r_options: 

                      if ( status == 200 ) {


                        if(mdtrXg.find(sip->sip_to->a_url->url_host) == mdtrXg.end()){
                          cout << "*** Key not found for options 200 result ****" << sip->sip_to->a_url->url_host << endl;
                          //                                        mdtrXg[sip->sip_to->a_url->url_host] = true;
                        }else{
                          //cout << "*** Key found for options 200 result ****" << sip->sip_to->a_url->url_host << endl;
                          mdtrXg[sip->sip_to->a_url->url_host] = true;
                        }

                      }

                      break;

      case nua_r_refer: break;
      case nua_r_publish: break;
      case nua_r_unpublish: break;
      case nua_r_info: break;
      case nua_r_prack: break;
      case nua_r_update: break;
      case nua_r_message: break;
      case nua_r_chat: break;
      case nua_r_subscribe: break;
      case nua_r_unsubscribe: break;
      case nua_r_notify: break;
      case nua_r_method: break;
      case nua_r_authenticate: break; 
      case nua_i_network_changed: break;
      case nua_i_register: 
                                  CLog::Detail( "Sofia Signalling", "New Register message from network: %d", (int) nh);			
                                  sigdriver->HandleIncomingRegistration(status, nh, sip, tags);
                                  break;
      default:
                                  /* unknown event -> print out error message */
                                  CLog::Detail( "Sofia Signalling", "Unknowne event=%d",event);
                                  printf("case default status = %d \n", status);
                                  break;
    }
    // CLog::Warning(THISMODULE, "IPPBX: End of SofiaCallback function \n");
  }

  void SofiaSIPSignalling::HandleEvents()
  {
    /* Oky asked me(Narasimham) to remove below code as it is testing code.
     * It should not exist. If you remove the comment, xgate crashes....
     */

    /*
       static int counter=0;
       static HostGSTRtpBin*binTmp[170]={NULL};

       int cycle=counter%8000;
       counter++;

       if((cycle >= 0) && (cycle<170))
       {
       if(binTmp[cycle])delete binTmp[cycle];
       binTmp[cycle]=NULL;
       }
       else if((cycle >= 4000) && (cycle<4170))
       {
       binTmp[cycle-4000]=new HostGSTRtpBin("test");
       binTmp[cycle-4000]->Open(20000+rand()%10000);
    //binTmp[cycle-4000]->Activate("192.168.1.1:50002", 0);
    binTmp[cycle-4000]->Activate("82.113.72.32:50002", 0);
    }
    */

    if (sofia_nua != NULL)
      su_root_step(sofia_root, 0);
  }

  /** Show service status.
  */
  bool SofiaSIPSignalling::OnShowStatus(String & output)
  {
    /*String indent(SHOW_STATUS_INDENT);
      SipAgent *agent = SipAgent::Instance();
      if (!agent)
      return false;

      OStringStream strm;
      agent->Dump(strm);
      output += strm.Str();
      */
    return true;
  }

  void SofiaSIPSignalling::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
  {
    IString arg0;
    CVariant v;


    String APIname;
    int testCaseNumber;
    SipAgent *agent = SipAgent::Instance();

    switch (cmd->GetId()) {
      case Sofia_Term_Cmd_Set_TestAPI:
        // get test API name
        cmd->GetArg((unsigned)0)->GetValue(v);
        if( v.IsEmpty() )
          break;
        else APIname = v.AsString();

        // get test Test Case number
        cmd->GetArg((unsigned)1)->GetValue(v);
        if( v.IsEmpty() )
          break;
        else testCaseNumber = v.AsInt();


        break;
      case Cmd_Require:
        cmd->GetArg((int)0)->GetValue(v);
        arg0 = v.AsString();
        if (arg0 == S_SES_TIMER) {
          if (no) {
            requireTimer = false;
          } else {
            requireTimer = true;
          }
          agent->SetRequireExtension( SipAgent::TimerExtension, requireTimer );
        }
        break;
      case Cmd_Session_Timer_Timeout:
        cmd->GetArg((int)0)->GetValue(v);
        int timeout;
        timeout = v.AsInt();
        agent->SetSessionExpiresTimeout(timeout);
        agent->SetRequireExtension( SipAgent::TimerExtension, requireTimer );
        break;
      default:
        CLog::Error( "SOFIA API TEST", "No DNS Request command found.");
        break;
    }

    return;
  }

  /* dandi add send msg for testing purpose */
  void SofiaSIPSignalling::SendSIPMessage(String uri, String message)
  {
    PLOGERR("\n*****IPPBX : inside SofiaSIPSignalling::SendSIPMessage()");
    nua_handle_t *handle;  

    handle = nua_handle(this->sofia_nua, NULL, SIPTAG_TO_STR(uri.c_str()), TAG_END());  

    nua_message(handle,  
        SIPTAG_CONTENT_TYPE_STR("text/plain"),  
        SIPTAG_PAYLOAD_STR(message.c_str()),  
        TAG_END());  

    nua_handle_destroy (handle);
  }


  bool SofiaSIPSignalling::OnShowConfig(const Cfg_Tree_Entry *entry,
      String & output, 
      const String & indent, 
      int indent_size) const
  {
    String new_indent(indent);
    new_indent.append(indent_size, ' ');

    ACQUIRE_SYSTEM_LOCK();
    SipAgent *agent = SipAgent::Instance();
    bool ses_timer_required = agent->IsExtensionRequired(SipAgent::TimerExtension);
    int ses_timer_interval = agent->GetSessionExpiresTimeout();
    RELEASE_SYSTEM_LOCK();

    char temp[80];
    output += indent + "signalling sip\n";
    if (ses_timer_required)
      output += new_indent + "require " S_SES_TIMER "\n";
    else
      output += new_indent + "no require " S_SES_TIMER "\n";

    sprintf(temp, " %d", ses_timer_interval);
    output += new_indent + S_SES_TIMER_TIMEOUT + temp + "\n";

    output += new_indent + "top\n\n";
    return OK;
  }

  /** AttachInterface: scan interface codec capability
  */
  bool SofiaSIPSignalling::AttachInterface(Interface *itf) 
  {
    /*
       int codec[20], i1[10], i2[10];
       std::set_intersection(i1, i1+10, i2, i2+10, codec);
       */
    return true;
  }

  /** DetachInterface: re-scan interface codec capability
  */
  void SofiaSIPSignalling::DetachInterface(Interface *itf) 
  {
    // Do nothing ?
  }


  /** SetupReq: Initiate a new outgoing call in a specific Interface
    using the specified call parameter
    */
  Call *SofiaSIPSignalling::SetupReq(Interface *itf,SelectionStrategy selection,CallParameter *cparam)
  {
    int i=0;

    PLOGERR("\n ******************* SetupReq() , selection = %d ******************* \n",selection);
    CLog::Detail(THISMODULE, "SetupReq call-id cparam = %d \n", cparam);

    if ( cparam == NULL ) {
      PLOGERR("cparam is NULL in sofia.cpp, returning from SetupReq \n");
      return NULL;
    }

    PLOGERR("\n*****IPPBX : inside SofiaSIPSignalling::SetupReq()\n");
    SofiaSIPCall * sofiaCall;
    NEW_RETURN_ (sofiaCall, SofiaSIPCall(*cparam, CD_Outgoing, itf->Name().c_str()));
    if(!sofiaCall)
      return NULL;
    /**
    // For CallQ
    if (selection == 999){
    return sofiaCall;
    }
     **/


    // Allocate one RTP media
    Rtp_Interface *rtp_itf = (Rtp_Interface *)itf;
    Media *rtp_media = NULL;
    if (!rtp_itf->AllocateMedia(sofiaCall, rtp_media, 0, CD_Outgoing, cparam)) {
      assert(rtp_media == NULL);
      sofiaCall->SetCause(CC_ResourceUnavailable);
      return sofiaCall;
    }
    CLog::Detail(THISMODULE, "SetupReq AllocateMedia rtp_media = %02x \n", rtp_media);
    sofiaCall->MediaAvailableInd(rtp_media);

    // Pass silence compression setting to SDP for G.729 attribute
    /*RTP_Config config;
      rtp_itf->GetRtpChannelConfig(config);
      xcall->silenceCompression = config.SilenceCompression;*/

    // actually create request sofia to create call
    //operation *op;


    //nua_handle_t*handle = CreateOutgoingNuaHandleFromCParam(cparam, rtp_itf->IsPrivateInfoAllowed());
    nua_handle_t*handle = CreateOutgoingNuaHandleFromCParam(cparam, rtp_itf);


    if (handle == NULL) {
      CLog::Warning(THISMODULE, "Cannot create outgoing NUA handle!");
      sofiaCall->SetCause(CC_ResourceUnavailable);
      sofiaCall->DetachMedia();

      return sofiaCall;
    }    

    sofiaCall->attachSofiaNuaHandle(handle);

    // set call credential
    sofiaCall->SetCallCredential(cparam->credential);

    // allow update if it's not virtual RTP
    if(sofiaCall->IsUsingVirtualRtp())
    {

      if(!sofiaCall->peerSipCall || !sofiaCall->peerSipCall->IsValid())
      {
        CLog::Warning(THISMODULE, "Outgoing Sofia Call using virtual RTP must have incoming peer call!");
        sofiaCall->SetCause(CC_InvalidCallReference);
        sofiaCall->DetachMedia();

        return sofiaCall;
      }

      sofiaCall->SetLocalSDPParameter(sofiaCall->peerSipCall->savedRemoteSdp);
    }else
    {
      nua_set_hparams(handle,
          NUTAG_ALLOW("UPDATE"),
          TAG_END());

      //SofiaPrecondition * precond=new SofiaPrecondition(PreconditionLevel_No, PreconditionLevel_No,
      //	                                              PreconditionLevel_Mandatory,PreconditionLevel_Mandatory);
      //sofiaCall->SetPrecondition(precond);
      sofiaCall->SetLocalSDPParameter((Rtp_Media*)rtp_media);
    }

    if (selection != 999)
    {
      sofiaCall->InviteCall();
    }
    else
    {
      // this is call-que case
      PLOGERR("\n ******************* this is CALLQUE CASE with selection = 999 ; avoid sending INVITE******************* \n");
    }


    return sofiaCall;
  }

  /** CreateDefaultCallParameter: Returns a CallParameter structure containing
    default values for this signalling protocol
    */
  CallParameter *SofiaSIPSignalling::CreateDefaultCallParameter()
  {
    CallParameter *cparam;
    NEW_RETURN_(cparam, CallParameter);
    if (!cparam)
      return NULL;

    InitDefaultCallParameter(*cparam);
    return cparam;
  }

  /** Initialize default call parameter.
  */
  void SofiaSIPSignalling::InitDefaultCallParameter( CallParameter & cparam )
  {
    cparam.cct = CCT_Speech;		// connection type
    cparam.ccl = CCL_NoIndication;	// companding law

    cparam.cpcCalled = CPC_Ordinary;	// party category
    cparam.cnpCalled = CNP_SIP;		// call number plan
    cparam.cntCalled = CNT_Unknown;	// call number type
    cparam.cnCalled.erase();
    cparam.complete = true; // for now, assume the number is incomplete
    cparam.csCalled.erase();

#ifdef XGATE_HAS_ACME_SBC
    //cparam.routeAddr.erase(); 
    //cparam.proxyUserName.erase();
    //cparam.domainName.erase();
#endif
    cparam.cui.erase();

    cparam.cpcCalling = CPC_Ordinary;
    cparam.cnpCalling = CNP_SIP;
    cparam.cntCalling = CNT_Unknown;
    cparam.csCalling.erase();
    cparam.ccp = CCP_Allowed;		// call calling presentation.
    cparam.ccs = CCS_Network;		// screening.

    cparam.extraInfo.owner = THISMODULE;
    cparam.extraInfo.sendable = false;
  }


  ///////////////////////////////////////////////////////////////////////////////
  /*
   * Sofia Registrar service.
   */

  SofiaRegistrar *SofiaRegistrar::instance;
  SofiaRegistrar::SofiaRegistrar()
    : ServiceProvider("SofiaReg", false, true, Lowest_Priority, "Sofia Registrar Server")/*, 
                                                                                           Registrar_Agent("REGISTER")*/
  {
    instance = this;
    time_to_check_ = time(NULL) + CHECK_INTERVAL;
    total_requests_ = 0;
    total_answered_ = 0;
    total_success_ = 0;
    total_timeout_ = 0;
  }

  SofiaRegistrar::~SofiaRegistrar()
  {
    CLog::Detail(THISMODULE, "Entering destructor SofiaRegistrar::~SofiaRegistrar");
    instance = NULL;
  }

  bool SofiaRegistrar::Load()
  {
    CLog::Warning(THISMODULE, "**********************SofiaRegistrar::Load()**************************\n");
    //_registrationApi = new RegistrationApi((RegistrationApi*)this);

    _registrationApi = new RegistrationApi((RegistrationApi*)this, API_PROVIDER_ID_SOFIA,"SofiaRegistrar"); // working
    ////rparam = new RegistrationApi((RegistrationApi*)this, API_PROVIDER_ID_SOFIA,"SofiaRegistrar");

    ////CallAPI::GetCallParam(CTask* task,CallParameter **cparam,int handle);
    _callTransferApi = new CallTransferApi((CallTransferApi*)this, API_PROVIDER_ID_CALLTRANSFER,"SofiaRegistrar"); // working

    //VoiceManager* var_a = (VoiceManager*) new VoiceAPI(API_PROVIDER_ID_VOICE,(VoiceManager*)this);
    //var_a->Create();
    //var_a->Load();

    return true;
  }

  bool SofiaRegistrar::CommitConfiguration()
  {
    return true;
  }

  bool SofiaRegistrar::Unload()
  {
    return true;
  }

  unsigned long GenID()
  {
    return (rand() | (((unsigned long)rand()) << 16) |\
        (((unsigned long)rand()) << 32) |\
        (((unsigned long)rand()) << 48) );
  }


#define XGATEKEYPASWD "9d7qb3ef423fsg45y2t"

  // create our signature in challenge's nonce (including time)
  String SofiaRegistrar::CreateChallenge()
  {	
    HASHHEX HA1;

    char timestr[9];
    char nonce[100];
    String Nonce_str;
    char randstr[10];


    time_t now = time(NULL);
    snprintf(timestr,sizeof(timestr), "%08x",(unsigned) now);
    snprintf(randstr,sizeof(randstr),"%04x",(unsigned) GenID() & 0x0000FFFF);

    DigestCalc::_DigestCalcHA1("md5", randstr, (char *) SofiaSIPSignalling::Instance()->GetRealm().c_str(), timestr, XGATEKEYPASWD, "", HA1);

    // 12341234123412341234123412341234
    //               |
    //               V
    // 12341234-2341-3412-4123
    HA1[8]='-';
    HA1[13]='-';
    HA1[18]='-';
    HA1[23]=0; // terminate string

    sprintf(nonce,"%s-%s%s", HA1, randstr, timestr);
    CLog::Detail( "Sofia Signalling", "Nonce=%s,Nonce len=%d",nonce,strlen(nonce));
    strcpy(Nonce_str.c_str(), nonce);
    return (Nonce_str);
  }

  // verify if nonce has our signature(with valid time)
  int SofiaRegistrar::VerifyChallenge(String& Nonce)
  {
    HASHHEX HA1;

    char timestr[9];
    char randstr[5];

    // step 1, check length
    if(Nonce.length()<36)
    {
      CLog::Warning(THISMODULE, "**********IPPBX : VerifyChallenge() : Invalid Nonce received(nonce too short) ; return false;\n");
      CLog::Warning(THISMODULE,"Invalid Nonce received(nonce too short)!");
      return false;
    }

    // step 2, verify if time information is expired
    strcpy(timestr, Nonce.c_str()+28);
    time_t incoming_time;
    time_t now = time(NULL);
    incoming_time=strtol(timestr, NULL, 16);
    if((now-incoming_time)>AUTH_EXP_TIME)
    {
      CLog::Warning(THISMODULE, "**********IPPBX : VerifyChallenge() : now-incoming_time)>AUTH_EXP_TIME ; return false;\n");
      return false;
    }

    // step 3, verify if rest of codes has valid signature
    // 012345678901234567890123456789012345
    // abcdabcd-abcd-abcd-acbd-abcdabcdabcd
    strcpy(randstr, Nonce.substr(24, 4).c_str());
    DigestCalc::_DigestCalcHA1("md5", randstr, (char *) SofiaSIPSignalling::Instance()->GetRealm().c_str(), timestr, XGATEKEYPASWD, "", HA1);

    HA1[8]='-';
    HA1[13]='-';
    HA1[18]='-';
    HA1[23]=0; // terminate string

    if(stricmp(HA1, Nonce.substr(0,23).c_str()))
    {
      CLog::Warning(THISMODULE, "**********IPPBX : VerifyChallenge() : stricmp(HA1, Nonce.substr(0,23).c_str() ; return false;\n");
      return false;
    }

    return true;
  }

  /**************
  // Rajesh, 27th july 2015
  char* RegistrationApi::getsipUri()
  {
  return sipUri;
  }
  void RegistrationApi::setsipUri(const char* m_sipUri)
  {
  memcpy(sipUri,m_sipUri,strlen(m_sipUri));
  CLog::Warning(THISMODULE, "********setsipUri() : SIP URI From registering phone is  = %s \n",sipUri);
  //PLOGERR("********setsipUri() : SIP URI From registering phone is = %s \n",m_sipUri);
  return;
  }

  char* RegistrationApi::getToAddr()
  {
  return toAddr;
  }
  void RegistrationApi::setToAddr(const char* m_ToAddr)
  {
  memcpy(toAddr,m_ToAddr,strlen(m_ToAddr));
  //PLOGERR("SIP toAddr From X-Lite is =%s \n",toAddr);
  return;
  }

  char* RegistrationApi::getSipToken()
  {
  return sipToken;
  }
  void RegistrationApi::setSipToken(const char* m_SipToken)
  {
  memcpy(sipToken,m_SipToken,strlen(m_SipToken));
  //PLOGERR("SIP sipToken From X-Lite is =%s \n",sipToken);
  return;
  }

  char* RegistrationApi::getContactUsr()
  {
  return ContactUsr;
  }
  void RegistrationApi::setContactUsr(const char* m_ContactUsr)
  {
  memcpy(ContactUsr,m_ContactUsr,strlen(m_ContactUsr));
  //PLOGERR("SIP ContactUsr From X-Lite is =%s \n",ContactUsr);
  return;
  }

  char* RegistrationApi::getNetworkPort()
  {
  return networkPort;
  }
  void RegistrationApi::setNetworkPort(const char* m_NetworkPort)
  {
  memcpy(networkPort,m_NetworkPort,strlen(m_NetworkPort));
  //PLOGERR("SIP networkPort From X-Lite is =%d \n",atoi(networkPort));
  return;
  }

  char* RegistrationApi::getHostName()
  {
  return contactHost;
  }
  void RegistrationApi::setHostName(const char* m_HostName)
  {
  memcpy(contactHost,m_HostName,strlen(m_HostName));
  CLog::Warning(THISMODULE, "SIP contactHost From phone is = %s \n",contactHost);
  return;
  }

  unsigned int RegistrationApi::getUsrExpiration()
  {
  return expireTime;
}
void RegistrationApi::setUsrExpiration(unsigned int m_expireTime)
{
  expireTime=m_expireTime;
  CLog::Warning(THISMODULE, "SIP expireTime From X-Lite is =%d \n",expireTime);
  return;
}


char* RegistrationApi::getNetworkIp()
{
  return networkIp;
}
void RegistrationApi::setNetworkIp(const char* m_NetworkIp)
{
  memcpy(networkIp,m_NetworkIp,strlen(m_NetworkIp));
  //PLOGERR("SIP networkIp From X-Lite is =%s \n",networkIp);
  return;
}


char* RegistrationApi::getNetworkProto()
{
  return networkProto;
}
void RegistrationApi::setNetworkProto(const char* m_NetworkProto)
{
  memcpy(networkProto,m_NetworkProto,strlen(m_NetworkProto));
  //PLOGERR("SIP networkProto From X-Lite is =%s \n",networkProto);
  return;
}

bool RegistrationApi::getHasAuth()
{
  return hasAuth;
}
void RegistrationApi::setHasAuth(bool m_hasAuth)
{
  hasAuth=m_hasAuth; 
  //PLOGERR("SIP m_hasAuth From X-Lite is =%d \n",hasAuth);
  return;
}

bool RegistrationApi::getStatus()
{
  return status;
}
void RegistrationApi::setStatus(bool m_status)
{
  status=m_status;
  //PLOGERR("SIP m_status From X-Lite is =%d \n",status);
  return;
}

char* RegistrationApi::getPassword()
{
  CLog::Warning(THISMODULE, "getPassword().....SIP m_password From object is =%s \n",r_password);
  return r_password;
}
void RegistrationApi::setPassword(const char *m_password)
{
  CLog::Warning(THISMODULE, "setPassword()..........SIP m_password From DataBase is =%s \n",m_password);
  memcpy(r_password,m_password,strlen(m_password));
  return;
}

char* RegistrationApi::getAuthScheme()
{
  return authScheme;
}
void RegistrationApi::setAuthScheme(const char *m_authScheme)
{
  memcpy(authScheme,m_authScheme,strlen(m_authScheme));
  //PLOGERR("SIP m_AuthScheme From X-Lite is =%s \n",password);
  return;
}
char* RegistrationApi::getRealm()
{
  return realm;
}
void RegistrationApi::setRealm(const char *m_realm)
{
  memcpy(realm,m_realm,strlen(m_realm));
  //PLOGERR("SIP m_realm From X-Lite is =%s \n",realm);
  return;
}
char* RegistrationApi::getNounce()
{
  return nounce;
}
void RegistrationApi::setNounce(const char *m_nounce)
{
  memcpy(nounce,m_nounce,strlen(m_nounce));
  //PLOGERR("SIP m_nounce From X-Lite is =%s \n",nounce);
  return;
}

char* RegistrationApi::getQop()
{
  return qop;
}
void RegistrationApi::setQop(const char *m_qop)
{
  memcpy(qop,m_qop,strlen(m_qop));
  //PLOGERR("SIP m_qop From X-Lite is =%s \n",qop);
  return;
}

char* RegistrationApi::getAlgorithm()
{
  return algorithm;
}
void RegistrationApi::setAlgorithm(const char *m_algorithm)
{
  memcpy(algorithm,m_algorithm,strlen(m_algorithm));
  //PLOGERR("SIP m_m_algorithm From X-Lite is =%s \n",algorithm);
  return;
}
*********************/

// Rajesh End
//void SofiaRegistrar::IncomingRegistration(SofiaRegisterRequest *request)
/*authDetails objAuthDetails; */
void SofiaRegistrar::IncomingRegistration(SofiaRegisterRequest *request)
{
  static Sofia_Reg_Info reg_info; // commented because global instance created.

  char addr[200];
  enum { DEFAULT_EXPIRATION = 3600 };
  msg_t *msg=request->GetRegisterRequest();
  const sip_t* sip=request->GetSofiaSip();

  CLog::Detail( "Sofia Signalling", "Registration Handling");

  sip_contact_t *contact = sip->sip_contact;

  /* Fill in basic information. 
  */
  pending_registrations++;

  CLog::Detail(THISMODULE,"narsim:pending trans=%d",pending_registrations);

  reg_info.ses_id = sip->sip_call_id->i_id;
  reg_info.sequence = sip->sip_cseq->cs_seq;
  reg_info.domain = sip->sip_to->a_url->url_host;
  reg_info.has_auth = false;

  CLog::Detail( "Sofia Signalling", " SIP Contact:%s:%s", 
      sip->sip_contact->m_url->url_host, 
      sip->sip_contact->m_url->url_port);
  CLog::Detail("Sofia Signalling", "IncomingRegistration: New registration request received from phone = %s", sip->sip_from->a_url->url_user);

  if(sip->sip_user_agent)
    reg_info.user_agent = sip->sip_user_agent->g_string;

  /* Next step:
   *	- extract Address of Record from To: header.
   */
  if(sip->sip_to)
  {
    reg_info.reg_rec_id = sip->sip_to->a_url->url_user;
    reg_info.reg_rec_addr = sip->sip_to->a_url->url_host;
  }

  /* Fill in source address and id.
  */		
  if(sip->sip_from)
  {
    reg_info.src_id = sip->sip_from->a_url->url_user;
  }
  get_msg_t_address(msg, reg_info.src_addr);

  if(sip->sip_user_agent)
    CLog::Detail(THISMODULE, "The user_ID is %s :",sip->sip_user_agent->g_string);

  reg_info.has_auth=GetSipAuthEntryFromSofiaSip(reg_info, sip);
  CLog::Detail(THISMODULE, "GetSipAuthEntryFromSofiaSip() completed");

  if(!reg_info.has_auth){

    reg_info.authAlgorithm = "";
    reg_info.authScheme = "";
    reg_info.authQop="";

    reg_info.authAlgorithm = "MD5";
    reg_info.authScheme = "Digest";
    reg_info.authQop="auth";
    memcpy(reg_info.authRealm.c_str(),sip->sip_to->a_url->url_host,strlen(sip->sip_to->a_url->url_host));
    reg_info.authNonce = this->CreateChallenge();
  }

  /* Next step:
   *	- check Contact header.
   */
  if (sip->sip_contact == NULL) 
  {
    CLog::Detail(THISMODULE, "Received invalid REGISTER (no Contact header) from %s", reg_info.src_addr.c_str());			
    nua_respond(request->GetNuaHandle(), 400, "Bad Request",
        NUTAG_AUTOANSWER(0),
        SIPTAG_CONTACT(contact), 
        NUTAG_WITH(msg), 
        TAG_END());
    return;
  } 
  else 
  {
    Reg_Entry entry; 

    CLog::Detail( "Sofia Signalling", "SIP message contains Contact header");
    if(sip->sip_contact->m_url->url_user)
      entry.user = sip->sip_contact->m_url->url_user;

    if(sip->sip_contact-> m_url->url_host && sip->sip_contact->m_url->url_port)
      sprintf(addr, "%s:%s", sip->sip_contact-> m_url->url_host, sip->sip_contact->m_url->url_port);

    CLog::Detail( "Sofia Signalling", "Create new registration entry");
    if(sip->sip_contact->m_expires)
    {
      entry.expiration = atoi(sip->sip_contact->m_expires);
    }
    else entry.expiration = DEFAULT_EXPIRATION;
    entry.priority = 1;

    CLog::Detail( "Sofia Signalling", "New registration entry pushed into list.");
  }

  CLog::Detail(THISMODULE, "IncomingRegistration User : %s", reg_info.authUser.c_str());
  CLog::Detail(THISMODULE, "IncomingRegistration URI  : %s", reg_info.authUri.c_str());
  CLog::Detail(THISMODULE, "IncomingRegistration Realm: %s", reg_info.authRealm.c_str());
  CLog::Detail(THISMODULE, "IncomingRegistration Nonce: %s", reg_info.authNonce.c_str());

  /* Notify registrar manager. */
  CLog::Detail(THISMODULE, "SIP REGISTER request for %s@%s from %s:%d dispatched successfully",sip->sip_to->a_url->url_user, sip->sip_to->a_url->url_host, sip->sip_from->a_url->url_host, sip->sip_from->a_url->url_user);
  //	g_registrationApiObj.SipUri = "10.22.3.94"; // initialize to some valid value
  g_registrationApiObj.ToAddr = "";
  g_registrationApiObj.Token = "";
  g_registrationApiObj.ContactUser = "";
  g_registrationApiObj.NetworkPort = "";
  g_registrationApiObj.ContactHost = "";
  g_registrationApiObj.ExpireTime = 0;
  g_registrationApiObj.NetworkProtocol = "";
  g_registrationApiObj.NetworkIP = "";
  g_registrationApiObj.HasAuth = false;

  // 7 output parameters	
  // don't initialize passwd with blank or "" -because this can be a valid passwd. instead initialize it with a globally unique value.
  char* default_temp_passwd = "hw4jIjXqZJjLG+3565xN6MIr3yxdsyYKNASKt4LhiUeKyw==***";
  if (PASSWORD_RECEIVED == false)
    g_registrationApiObj.Passwd = default_temp_passwd; // **** don't erase password what first Register has sent it.
  g_registrationApiObj.Status = false;
  g_registrationApiObj.AuthScheme = "";
  g_registrationApiObj.Realm = "";
  g_registrationApiObj.Nounce = "";
  g_registrationApiObj.Qop = "";
  g_registrationApiObj.Algorithm = "";

  g_registrationApiObj.SipUri =  sip->sip_to->a_url->url_user;

  if (PASSWORD_RECEIVED == false)
    global_sipUri = sip->sip_to->a_url->url_user; // save user-extension during initial REGISTER

  g_registrationApiObj.ToAddr = sip->sip_contact-> m_url->url_host;
  /* Narasimham commented */
  /* g_registrationApiObj.Token = info.token; */
  g_registrationApiObj.ContactUser = addr;
  g_registrationApiObj.NetworkPort = sip->sip_contact->m_url->url_port;
  g_registrationApiObj.ContactHost = sip->sip_from->a_url->url_host;
  g_registrationApiObj.HasAuth = reg_info.has_auth;

  CLog::Detail(THISMODULE, "SofiaRegistrar::IncomingRegistration password status=%d",PASSWORD_RECEIVED);

  //PASSWORD_RECEIVED = true; /* Spawnask is causing problem now */
  if (PASSWORD_RECEIVED == false)
  {
    List <String> cmdline_arg;
    //String arg="SLRegistration";
    char *arg="SLRegistration";

    cmdline_arg.push_back (arg);
    CLog::Detail(THISMODULE, "SofiaRegistrar::IncomingRegistration SpawnTask is called");
    CTask* new_task = Kernel::Instance()->SpawnTask ("ippbx_registration", cmdline_arg,  TASK_STARTUP_CALL);

    if (!new_task)
    {
      CLog::Warning(THISMODULE, "new task can not be created; return\n");
      return;
    }

    new_task->SetState_2( CTask::READY );
    ///////int m_TLS = Kernel::Instance()->AllocTLS();
    if (!g_registrationApiObj.Status)
    {
      SofiaRegistrar::AnswerRegistration(request,reg_info,scUnauthorized);
    }
    else if((!reg_info.has_auth))
    {
      SofiaRegistrar::AnswerRegistration(request,reg_info,scUnauthorized);
    }
    else 
    {
      SofiaRegistrar::AnswerRegistration(request,reg_info,scUnauthorized);
    }

    return;
  } ///  PASSWORD_RECEIVED
  if (sip->sip_to->a_url->url_user == global_sipUri) // check whether second REGISTER came from same user
  {
    g_registrationApiObj.Passwd = global_passwd;
  }

  bool	res=SofiaRegistrar::authResponse(reg_info.authUser.c_str(),reg_info.authRealm.c_str(), global_passwd.c_str(), reg_info.authNonce.c_str(),reg_info.authNonceCount.c_str(),reg_info.authCNonce.c_str(),reg_info.authQop.c_str() ,(sip->sip_request->rq_method_name),reg_info.authUri.c_str(),reg_info.authResponse.c_str());

  if((res) && (PASSWORD_RECEIVED))
  {
    SofiaRegistrar::AnswerRegistration(request,reg_info,Reg_Info::STATUS_OK); // Successful enable of Register message
    PASSWORD_RECEIVED = false;
    global_sipUri = "";
    global_passwd = "";
  }
  else
  {
    SofiaRegistrar::AnswerRegistration(request,reg_info,scUnauthorized);
  }
}

void SofiaRegistrar::AddCall(SofiaRegisterRequest *call)
{
  CLog::Detail(THISMODULE, "Entering SofiaRegistrar::AddCall Constructor");

  calls_.insert(call);

  CLog::Detail(THISMODULE, "Exiting SofiaRegistrar::AddCall Constructor");
}

void SofiaRegistrar::RemoveCall(SofiaRegisterRequest *call)
{
  CLog::Detail(THISMODULE, "Entering SofiaRegistrar::RemoveCall Destructor");
  if (call) 
    calls_.erase(call);
  CLog::Detail(THISMODULE, "Exiting SofiaRegistrar::RemoveCall Destructor");
}

void SofiaRegistrar::CancelRegistration(SofiaRegisterRequest *req,Sofia_Reg_Info info)
{
  CLog::Detail(THISMODULE, "Entering SofiaRegistrar::CancelRegistration Function");
  AnswerRegistration(req,info,Reg_Info::STATUS_SERVER_ERROR);

  CLog::Detail(THISMODULE, "Exiting SofiaRegistrar::CancelRegistration Function");
}

bool SofiaRegistrar::AnswerRegistration(SofiaRegisterRequest *req, Sofia_Reg_Info reg_info, int status)
{

  sip_t *sip;

  CLog::Detail(THISMODULE , "Entering SofiaRegistrar::AnswerRegistration Function");

  CLog::Detail(THISMODULE, "SofiaRegistrar::AnswerRegistration URI  : %s", reg_info.authUri.c_str());
  CLog::Detail(THISMODULE, "SofiaRegistrar::AnswerRegistration Realm: %s", reg_info.authRealm.c_str());
  CLog::Detail(THISMODULE, "SofiaRegistrar::AnswerRegistration Nonce: %s", reg_info.authNonce.c_str());	

  if (!req->IsValid()) {
    CLog::Warning(THISMODULE, "SofiaRegistrar::AnswerRegistration is called for an invalid request.");
    return false;
  }

  nua_handle_t *nh = (nua_handle_t *) req->GetNuaHandle();
  if(!nh)
    CLog::Warning(THISMODULE, "SofiaRegistrar::AnswerRegistration Unable to get nua_handle.");
  else
    CLog::Detail(THISMODULE, "SofiaRegistrar::AnswerRegistration nua_handle: %d", (int) nh);

  sip = (sip_t *) req->GetSofiaSip();
  if(!sip)
    CLog::Warning(THISMODULE, "SofiaRegistrar::AnswerRegistration Unable to get sip_t handle.");

  nua_t * nua = SofiaSIPSignalling::Instance()->GetSofiaNua();
  if(!nua)
    CLog::Warning(THISMODULE, "SofiaRegistrar::AnswerRegistration Unable to get nua_t handle.");

  //msg_t *msg = nua_current_request(nua);

  ++total_answered_;

  if (status == Reg_Info::STATUS_OK)
    ++total_success_;
  //PLOGERR("Total Success is =%d",total_success_);
  /* If response is 2xx */
  if (status / 100 == 2) {
    nua_respond(nh, SIP_200_OK, 
        NUTAG_AUTOANSWER(0),
        SIPTAG_CONTACT(sip->sip_contact), 
        /* SIPTAG_EXPIRES(sip->sip_expires), */
        SIPTAG_EXPIRES_STR("3600"),
        SIPTAG_DATE(sip->sip_date), NUTAG_WITH(req->GetRegisterRequest()), TAG_END());

  } else if (status == scUnauthorized) {


    SofiaSIPSignalling::ChallengeRequest(nh, 401, 
        SofiaSIPSignalling::Instance()->GetRealm().c_str(), reg_info.authStale, req->GetRegisterRequest());
  } else if (status == scProxyAuthenticationRequired) {

    SofiaSIPSignalling::ChallengeRequest(nh, 407, SofiaSIPSignalling::Instance()->GetRealm().c_str(), reg_info.authStale, req->GetRegisterRequest());
  }

  pending_registrations--;
  CLog::Detail(THISMODULE,"narsim: Pending trans=%d",pending_registrations);
  CLog::Detail(THISMODULE , "Exiting SofiaRegistrar::AnswerRegistration Function");	
  return true;
}

void SofiaRegistrar::HandleEvents()
{

}

bool SofiaRegistrar::OnShowStatus(String & output)
{
  return true;
}
void SofiaRegistrar::AddRegistration( SofiaRegisterRequest*)
{
}
void SofiaRegistrar::RemoveRegistration( SofiaRegisterRequest*)
{
}

///////////////////////////////////////////////////////////////////////////////
/*
 * Sofia Registrar per Call/Registration
 */
  SofiaRegisterRequest::SofiaRegisterRequest(nua_handle_t * nh) 
:signature_(SIGNATURE), nua_handle_(nh)
{
  SofiaRegistrar::Instance()->AddCall(this);	
}


SofiaRegisterRequest::~SofiaRegisterRequest()
{
  SofiaRegistrar::Instance()->RemoveCall(this);
}


void SofiaRegisterRequest::RegistrationHandling( sip_t const *sip, tagi_t tags[], msg_t* msg)
{
  register_msg_ = msg;
  sip_ = sip;

  SofiaRegistrar::Instance()->IncomingRegistration(this);

  return;
}

bool SofiaRegisterRequest::IsValid() const
{
  try {
    return signature_ == SIGNATURE;
  } catch (...) {
    return false;
  }
}
/*DigestCalc::_DigestCalcResponse( HA1,
  (char*)"eebd8d22-7c4c-550b-3d4e-d7af55c97eca",
  (char*)"00000001",
  (char*)"e3c29acfde8913af63e2fced5c8011ca",
  (char*)"auth",
  (char*)"Digest",
  (char*)"sip:80.74.227.71",
  HA2,
  Response);*/

// Rajesh, 12/10/2015, added this new method for authentication validation
bool SofiaRegistrar::authResponse(char *username, char *realm,char *passwd,char *nonce,char *nc, char *cnonce, char *qop, const char* regstr,char *authUri,char *response)
{
  const char *a1_hash = NULL;
  su_md5_t ctx;
  //auth_res_t ret = AUTH_FORBIDDEN;
  char uridigest[2 * SU_MD5_DIGEST_SIZE + 1];
  char bigdigest[2 * SU_MD5_DIGEST_SIZE + 1];
  char hexdigest[2 * SU_MD5_DIGEST_SIZE + 1] = "";
  //char *username, *realm, *nonce, *uri, *qop, *cnonce, *nc, *response, *input = NULL, *input2 = NULL;
  char input[200]={'\0'};
  char input2[200]={'\0'};
  //username = realm = nonce = qop = cnonce = nc = response = NULL;
  if (!a1_hash) {
    //      input = switch_mprintf("%s:%s:%s", username, realm, passwd);
    sprintf(input,"%s:%s:%s", username, realm, passwd);
    su_md5_init(&ctx);
    su_md5_strupdate(&ctx, input);
    su_md5_hexdigest(&ctx, hexdigest);
    su_md5_deinit(&ctx);
    //switch_safe_free(input);
    a1_hash = hexdigest;
  }
  char temp[200]={'\0'};
  sprintf(temp,"%s:%s", regstr, authUri);
  //if ((input = switch_mprintf("%s:%q", regstr, uri)))
  if (temp)
  {
    su_md5_init(&ctx);
    su_md5_strupdate(&ctx, temp);
    su_md5_hexdigest(&ctx, uridigest);
    su_md5_deinit(&ctx);
  }

  if (nc && cnonce && qop) {
    //input2 = switch_mprintf("%s:%s:%s:%s:%s:%s", a1_hash, nonce, nc, cnonce, qop, uridigest);
    sprintf(input2,"%s:%s:%s:%s:%s:%s", a1_hash, nonce, nc, cnonce, qop, uridigest);
  } else {
    sprintf(input2,"%s:%s:%s", a1_hash, nonce, uridigest);
    //input2 = switch_mprintf("%s:%s:%s", a1_hash, nonce, uridigest);
  }

  if (input2) {
    memset(&ctx, 0, sizeof(ctx));
    su_md5_init(&ctx);
    su_md5_strupdate(&ctx, input2);
    su_md5_hexdigest(&ctx, bigdigest);
    su_md5_deinit(&ctx);
  }
  int val=strcasecmp(bigdigest, response);
  PLOGERR("****authResponse : bigdigest %s : response %s \n",bigdigest,response);
  if (input2 && !strcasecmp(bigdigest, response))
  {
    CLog::Warning(THISMODULE, "**************IPPBX : input2 && !strcasecmp(bigdigest, response == TRUE\n");
    return true;
  }
  else 
  {
    CLog::Warning(THISMODULE, "**************IPPBX : input2 && !strcasecmp(bigdigest, response == FALSE ; return false;\n");
    /* return false; */
    return true; /* Send 200 OK as code is not handling Registration according to RFC3261. Needs to be modified later */
  }
  //return ret;
}

#ifdef XGATE_HAS_SIP_IPPBX
// code below added by IPPBX.
char* SofiaSIPSignalling::switch_stristr(const char *instr, const char *str)
{
  const char *pptr, *sptr, *start;

  if (!str || !instr)
    return NULL;

  for (start = str; *start; start++) {
    // find start of pattern in string
    for (; ((*start) && (toupper(*start) != toupper(*instr))); start++);

    if (!*start)
      return NULL;

    pptr = instr;
    sptr = start;

    while (toupper(*sptr) == toupper(*pptr)) {
      sptr++;
      pptr++;

      /* if end of pattern then pattern was found */
      if (!*pptr)
        return (start);

      if (!*sptr)
        return NULL;
    }
  }
  return NULL;
}

static char local_dtmf_store_call_id[TRANSFER_TARGET_EXTENSION_LENGTH][35];
static int local_dtmf_store_call_seq[TRANSFER_TARGET_EXTENSION_LENGTH];
static char local_dtmf_store_dtmf_char[TRANSFER_TARGET_EXTENSION_LENGTH];
static char transfer_target_dtmf_digits[TRANSFER_TARGET_EXTENSION_LENGTH+1];
static char transfer_target_extension_array[TRANSFER_TARGET_EXTENSION_LENGTH+1];
static int transfer_target_index;
static bool flag_collecting_transfer_target_digits;

static char switchboard_last_digit_1;
static char switchboard_last_digit_2;
bool SofiaSIPSignalling::HandleIncomingInfo(int status, nua_handle_t * nh, sip_t const *sip, tagi_t tags[], SofiaSIPCall * sCall)
{
  PLOGERR("************IPPBX: Inside  HandleIncomingInfo() ; status = %d\n", status);
  // Send 200 OK response to this incoming INFO.
  nua_respond(nh, SIP_200_OK, TAG_END());

  char *signal_ptr;
  char DTMF_dialed_digit;
  g_callTransferApiObj.dialedDigits = "";
  /////g_registrationApiObj_for_callTransfer.CallTransferExtension = "";
  /////g_registrationApiObj_for_callTransfer.CallTransferExtensionSent = false;

  if(sCall)
  {
    char *signal_ptr;
    int flip_flag = 0;
    int unflip_flag = 0;
    int call_park = 0;
    int suspend = 0;
    int resume = 0;
    int rec_start = 0;
    int rec_stop = 0;
    if (signal_ptr = switch_stristr("UnFlip=", sip->sip_payload->pl_data))
    {
      flip_flag = 0;
      unflip_flag = 1;
      call_park = 0;
      suspend = 0;
      resume = 0;
      rec_start = 0;
      rec_stop = 0;
    } else if (signal_ptr = switch_stristr("Flip=", sip->sip_payload->pl_data))
    {
      flip_flag = 1;
      unflip_flag = 0;
      call_park = 0;
      suspend = 0;
      resume = 0;
      rec_start = 0;
      rec_stop = 0;
    } else if (signal_ptr = switch_stristr("Park=", sip->sip_payload->pl_data))
    {
      call_park = 1;
      flip_flag = 0;
      unflip_flag = 0;
      suspend = 0;
      resume = 0;
      rec_start = 0;
      rec_stop = 0;
    } else if (signal_ptr = switch_stristr("Suspend=", sip->sip_payload->pl_data))
    {
      call_park = 0;
      flip_flag = 0;
      unflip_flag = 0;
      suspend = 1;
      resume = 0;
      rec_start = 0;
      rec_stop = 0;
    } else if (signal_ptr = switch_stristr("Resume=", sip->sip_payload->pl_data))
    {
      call_park = 0;
      flip_flag = 0;
      unflip_flag = 0;
      suspend = 0;
      resume = 1;
      rec_start = 0;
      rec_stop = 0;
    } else if (signal_ptr = switch_stristr("RecStart=", sip->sip_payload->pl_data))
    {	flip_flag = 0;
      unflip_flag = 0;
      call_park = 0;
      suspend = 0;
      resume = 0;
      rec_start = 1;
      rec_stop = 0;
    } else if (signal_ptr = switch_stristr("RecStop=", sip->sip_payload->pl_data))
    {       flip_flag = 0;
      unflip_flag = 0;
      call_park = 0;
      suspend = 0;
      resume = 0;
      rec_start = 0;
      rec_stop = 1;
    } else
    {
      nua_respond(nh, 400,"Bad Request", TAG_END());
      return false;
    }
    // Send 200 OK response to this incoming INFO.
    nua_respond(nh, SIP_200_OK, TAG_END());
    if (flip_flag || unflip_flag || call_park)
    {
      CLog::Detail(THISMODULE, "HandleIncomingInfo : payload %s \n", sip->sip_payload->pl_data);
      char *token;
      char payload_st[128]="";
      char usr_key[64]="";
      sprintf(payload_st,"%s",sip->sip_payload->pl_data);
      token = strtok(payload_st,"=");
      while(token!=NULL)
      {
        CLog::Detail(THISMODULE, "HandleIncomingInfo token = %s \n", token);
        sprintf(usr_key,"%s",token);
        token = strtok(NULL, "=");
      }
      CLog::Detail(THISMODULE, "HandleIncomingInfo : token %s \n" , token);
      CLog::Detail(THISMODULE, "HandleIncomingInfo : usr_key %s ; from user %s \n" , usr_key, sip->sip_from->a_url->url_user);
      if (call_park)
      {
        call_park_code[sip->sip_from->a_url->url_user] = usr_key;
      }
      else
      {
        call_transfer_extn[sip->sip_from->a_url->url_user] = usr_key;
      }
    }
    if (flip_flag)
    {
      sCall->OnCallFlip();
    }
    if (unflip_flag)
    {
      sCall->OnCallUnFlip();
    }
    if (call_park)
    {
      sCall->OnCallPark();
    }
    if (suspend || resume)
    {
      char buff[24];
      int idx = 0;
      int key_chk = key_not_exist;
      if (sip && sip->sip_to && sip->sip_to->a_url && sip->sip_to->a_url->url_user && sip->sip_from &&
          sip->sip_from->a_url && sip->sip_from->a_url->url_user)
      {
        sprintf(buff, "%s_%s", sip->sip_from->a_url->url_user, sip->sip_to->a_url->url_user);

        for(idx= 0; idx < usr_key.size(); idx++)
        {
          if(buff == usr_key[idx])
          {
            key_chk = key_exist;
            break;
          }
        }
        if(key_chk == key_not_exist)
        {
          usr_key.push_back(buff);
        }
      }
    }
    if (suspend)
    {
      sCall->OnCallHold(1);
    }
    if (resume)
    {
      sCall->OnCallHold(0);
    }
    if (rec_start || rec_stop)
    {
      char buff[24];
      int idx = 0;
      int key_chk = key_not_exist;
      if (sip && sip->sip_to && sip->sip_to->a_url && sip->sip_to->a_url->url_user && sip->sip_from &&
          sip->sip_from->a_url && sip->sip_from->a_url->url_user)
      {
        sprintf(buff, "%s_%s", sip->sip_from->a_url->url_user, sip->sip_to->a_url->url_user);

        for(idx= 0; idx < usr_key.size(); idx++)
        {
          if(buff == usr_key[idx])
          {
            key_chk = key_exist;
            break;
          }
        }
        if(key_chk == key_not_exist)
        {
          usr_key.push_back(buff);
        }
      }
    }
    if (rec_start)
    {
      sCall->OnCallRec(1);
    }
    if( rec_stop)
    {
      sCall->OnCallRec(0);
    }	
  } else
  {
    nua_respond(nh, 400,"Bad Request", TAG_END());
    return false;
  }

#if 0
  // Try and find signal information in the payload
  if ((signal_ptr = switch_stristr("Signal=", sip->sip_payload->pl_data))) 
  {
    int tmp;
    bool call_pickup_feature = false;
    bool process_call_transfer = false;

    // move signal_ptr where we need it (right past Signal=)
    signal_ptr = signal_ptr + 7;

    // handle broken devices with spaces
    while (*signal_ptr && *signal_ptr == ' ')
    {
      signal_ptr++;
    }

    tmp = atoi(signal_ptr);
    //PLOGERR("************IPPBX: tmp = %d \n", tmp);
    DTMF_dialed_digit = switch_rfc2833_to_char(tmp);
    PLOGERR("************IPPBX: DTMF digit = %c \n", DTMF_dialed_digit);

    if (sip->sip_call_id && sip->sip_call_id->i_id) 
    {
      PLOGERR("************IPPBX: sip->sip_call_id->i_id = %s \n", sip->sip_call_id->i_id);
    }
    else
      return false; // no call-id? no point in processing dtmf from this message

    if (sip->sip_cseq && sip->sip_cseq->cs_seq) 
    {
      PLOGERR("************IPPBX: sip->sip_cseq->cs_seq = %u \n", sip->sip_cseq->cs_seq);
    }
    else
      return false; // no cs_seq? no point in processing dtmf from this message

    if (sip->sip_from && sip->sip_from->a_url->url_user) 
    {
      PLOGERR("************IPPBX: sip->sip_from->a_url->url_user = %s \n", sip->sip_from->a_url->url_user);
    }
    else
      return false; // no sip_from? no point in processing dtmf from this message

    // check whether dtmf is coming from sip_from for the first time. if yes, store sip_from in its array.

    switch (DTMF_dialed_digit)
    {
      case CALL_TRANFER_DTMF_SECOND_DIGIT :
        PLOGERR("************IPPBX: SECOND SWITCH CASE = %c \n", DTMF_dialed_digit);
        if (flag_collecting_transfer_target_digits == false)
        {
          local_dtmf_store_dtmf_char[1] = DTMF_dialed_digit;
        }
        else
        {
          PLOGERR("************IPPBX: NON-DEFAULT case : else store the dtmf digits in the buffer\n");
          // else store the call-transfer dtmf digits to buffer
          transfer_target_dtmf_digits[transfer_target_index] = DTMF_dialed_digit;
          transfer_target_index++;
        }
        break;

      case CALL_TRANFER_DTMF_FIRST_DIGIT :
      case CALL_PICKUP_DTMF_FIRST_DIGIT :
        PLOGERR("************IPPBX: FIRST SWITCH CASE = %c \n", DTMF_dialed_digit);
        if (flag_collecting_transfer_target_digits == false)
        {
          local_dtmf_store_dtmf_char[0] = DTMF_dialed_digit;
          break;
        }
        else
        {
          PLOGERR("************IPPBX: NON-DEFAULT case : else store the dtmf digits in the buffer\n");
          // else store the call-transfer dtmf digits to buffer
          transfer_target_dtmf_digits[transfer_target_index] = DTMF_dialed_digit;
          transfer_target_index++;
        }
        //break;

      default:
        PLOGERR("************IPPBX: switch DEFAULT case = %c \n", DTMF_dialed_digit);

        // call-transfer
        ///if (local_dtmf_store_dtmf_char[0] == CALL_TRANFER_DTMF_FIRST_DIGIT)
        if ((local_dtmf_store_dtmf_char[0] == CALL_TRANFER_DTMF_FIRST_DIGIT) &&
            (local_dtmf_store_dtmf_char[1] == CALL_TRANFER_DTMF_SECOND_DIGIT))
        {
          // process '*' or CALL_TRANFER_DTMF_FIRST_DIGIT and next dialed digit.
          PLOGERR("*************IPPBX : process '*' or CALL_TRANFER_DTMF_FIRST_DIGIT and next dialed digit.\n");
          ////if ((DTMF_dialed_digit == CALL_TRANFER_DTMF_SECOND_DIGIT) && 
          if ((DTMF_dialed_digit == CALL_TRANFER_DTMF_THIRD_DIGIT) && 
              (flag_collecting_transfer_target_digits == false))
          {
            // transferor pressed '*3' ; collect digits of transfer-target
            PLOGERR("************IPPBX: CALL_TRANFER : transferor pressed '*3' ; collect digits of transfer-target\n");
            // set flag_collecting_transfer_target_digits to true  while collecting dtmf digits

            // collect_transfer_target_digits() should set flag_collecting_transfer_target_digits
            // to true while collecting dtmf digits of transfer-target.
            //collect_transfer_target_digits();
            //if (transfer_target_digits_collected_successfully())
            //{
            // 1) send UPDATE to transferee
            // 2) send INVITE to transfer-target, 
            // and then 3) send BYE to transferor.
            flag_collecting_transfer_target_digits = true; // start collecting digits
            transfer_target_index = 0; // reset the index to 0.
            //}
          }
          else
          {
            PLOGERR("************IPPBX: else store the dtmf digits in the buffer\n");
            // else store the dtmf digits in the buffer
            transfer_target_dtmf_digits[transfer_target_index] = DTMF_dialed_digit;
            transfer_target_index++;
            if ((transfer_target_index >= TRANSFER_TARGET_EXTENSION_LENGTH) ||
                (DTMF_dialed_digit == '#'))
            {
              transfer_target_index = 0; // reset index
              int i = 0;
              //PLOGERR("************IPPBX: CALL_TRANFER : transfer_target_dtmf_digits[0] = %c\n",transfer_target_dtmf_digits[0]);
              //PLOGERR("************IPPBX: CALL_TRANFER : transfer_target_dtmf_digits[1] = %c\n",transfer_target_dtmf_digits[1]);
              //PLOGERR("************IPPBX: CALL_TRANFER : transfer_target_dtmf_digits[2] = %c\n",transfer_target_dtmf_digits[2]);
              for (i=0; (i < TRANSFER_TARGET_EXTENSION_LENGTH) && (transfer_target_dtmf_digits[i] != '#'); i++)
              {
                PLOGERR("************IPPBX: CALL_TRANFER : transfer_target_dtmf_digits[i] = %c\n",transfer_target_dtmf_digits[i]);
              }
              transfer_target_dtmf_digits[TRANSFER_TARGET_EXTENSION_LENGTH]='\0';
              transfer_target_dtmf_digits[i]='\0';
              CLog::Detail(THISMODULE, "**************** DIALED DIGITS ARE = %s!!!!!!\n", transfer_target_dtmf_digits);
              CLog::Warning(THISMODULE, "**************** DIALED DIGITS ARE = %s!!!!!!", transfer_target_dtmf_digits);

              g_callTransferApiObj.dialedDigits = transfer_target_dtmf_digits;
              g_registrationApiObj_for_callTransfer.CallTransferExtensionSent = true;
              g_registrationApiObj_for_callTransfer.CallTransferExtension = transfer_target_dtmf_digits;
              PLOGERR("************IPPBX: CALL_TRANFER : g_registrationApiObj_for_callTransfer.CallTransferExtensionSent = %d\n",g_registrationApiObj_for_callTransfer.CallTransferExtensionSent);
              PLOGERR("************IPPBX: CALL_TRANFER : g_registrationApiObj_for_callTransfer.CallTransferExtension = %s\n",g_registrationApiObj_for_callTransfer.CallTransferExtension);

              process_call_transfer = true; // process only after 200 OK (INFO) sent out.
              flag_collecting_transfer_target_digits = false;
            }
          }
        }
        // "else" call-pickup. "else" makes either call-transfer or call-pickup.
        else if (local_dtmf_store_dtmf_char[0] == CALL_PICKUP_DTMF_FIRST_DIGIT)
        {
          // process '#' or CALL_PICKUP_DTMF_FIRST_DIGIT and next dialed digit.
          if (DTMF_dialed_digit == CALL_PICKUP_DTMF_SECOND_DIGIT)
          {
            PLOGERR("************IPPBX: CALL_PICKUP : yes previous digit pressed was %c \n",CALL_PICKUP_DTMF_FIRST_DIGIT);
            // this is call-pickup feature. 1) send only BYE to party-B.
            call_pickup_feature = true; // process only after 200 OK (INFO) sent out.
            flag_collecting_transfer_target_digits = false;
          }
        }

        if (flag_collecting_transfer_target_digits == false)
        {
          // else clear '*' from local_dtmf_store_dtmf_char[0] as default action here.
          local_dtmf_store_dtmf_char[0] = ' ';
          // no need to clear other digits; -as first digit above is already cleared.
          //local_dtmf_store_dtmf_char[1] = ' ';
          //local_dtmf_store_dtmf_char[2] = ' ';
          //transfer_target_index = 0; // reset index
        }

        break; // case "default"
    }

    CallParameter cparam;
    CreateSofiaCallParam(cparam, sip, tags);

    PLOGERR (" ************** HANDLE_INCOMONG_INFO() : \n");
    if (cparam._isSwitchBoardCall)
    {
      PLOGERR("************** HANDLE_INCOMONG_INFO() : THIS IS SWITCHBOARD-CALL !!!!!!!!!!!!!!!!!!\n");
    }

    //while (switchboard_index < SWITCHBOARD_COLLECT_MAX_DTMF_DIGITS )
    //&& (switchboard_last_digit_1 != SWITCHBOARD_DTMF_FIRST_TERM_DIGIT) && 
    //(switchboard_last_digit_2 != SWITCHBOARD_DTMF_SECOND_TERM_DIGIT))
    //{
    PLOGERR ("SWITCHBOARD_COLLECT_MAX_DTMF_DIGITS = %d\n", SWITCHBOARD_COLLECT_MAX_DTMF_DIGITS);
    switchboard_dtmf_digits[switchboard_index] = DTMF_dialed_digit;
    switchboard_index++;
    switchboard_dtmf_digits[switchboard_index] = '\0';
    PLOGERR ("switchboard_dtmf_digits = %s\n", switchboard_dtmf_digits);
    //}
    if ((switchboard_index >= (SWITCHBOARD_COLLECT_MAX_DTMF_DIGITS )) || (strstr(switchboard_dtmf_digits, "#9")))
    {
      g_registrationApiObj_for_SwitchBoard.SwitchBoardExtension = switchboard_dtmf_digits;
      g_registrationApiObj_for_SwitchBoard.SwitchBoardExtension[SWITCHBOARD_COLLECT_MAX_DTMF_DIGITS] = '\0';
      PLOGERR ("g_registrationApiObj_for_SwitchBoard.SwitchBoardExtension = %s\n", g_registrationApiObj_for_SwitchBoard.SwitchBoardExtension);
    }


    // 200 OK is sent to incoming INFO as above. now process for call-transfer or call-pickup
    if (process_call_transfer) // BYE should be sent out only during call-transfer or call-pickup.
    {
      PLOGERR("************IPPBX: process_call_transfer == true\n");

      return true; /// we return here.... not processing code below....

      // you probably need to tear down existing call first, before creating new task/call.
      // so send BYE first... and then spawn a task.
      CLog::Detail(THISMODULE, "**************** send BYE first... and then spawn a task.!!!!!!");
      nua_bye(nh, TAG_END()); // BYE sent out to party-B.
      ///nua_bye(nh, TAG_END());

      // send re-INVITE to party-A. this may restore call-state of second call-leg h_legB in SL.
      /////sleep(1);

      // 1) send UPDATE to transferee; UPDATE maybe optional in call-flow.
      //nua_set_hparams(nh, NUTAG_ALLOW("UPDATE"), TAG_END());
      ////nua_update(nh,NUTAG_AUTOACK(0),TAG_END()); // UPDATE not sent out before of Invite ???

      // 2) send INVITE to the transfer-target
      strcpy(sip->sip_from->a_url->url_user , sip->sip_to->a_url->url_user);
      strcpy(sip->sip_from->a_url->url_host , sip->sip_to->a_url->url_host);

      // 2) send RE-INVITE to party-A.
      PLOGERR("********IPPBX : 3) NEW FROM sip->sip_from->a_url->url_user = %s\n",sip->sip_from->a_url->url_user);

      // populate sip_to with that of transfer-target
      strcpy(sip->sip_to->a_url->url_user , transfer_target_dtmf_digits); // transfer-target's extension
      PLOGERR("********IPPBX : 3) NEW TO sip->sip_to->a_url->url_user = %s\n",sip->sip_to->a_url->url_user);

      // transfer-target's IP.
      //strcpy(sip->sip_to->a_url->url_host , "192.168.11.82");
      //strcpy(sip->sip_to->a_url->url_host , "192.168.13.30");
      //strcpy(sip->sip_to->a_url->url_host , "192.168.12.56"); // my softphone
      //strcpy(sip->sip_to->a_url->url_host , "192.168.13.66"); // Rajesh's hardphone

      //////nua_update(nh,NUTAG_AUTOACK(0),TAG_END()); // on-hold ??

      SofiaSIPCall *sofiaCall = SofiaSIPSignalling::InitiateInviteCall_to_transferTarget(status, nh, sip, tags); // probably this not needed.
      if (sofiaCall == NULL)
      {
        return;
      }

      String application_name = SL_APPLICATION_NAME_FOR_CALL_TRANSFER;
      List <String> cmdline_arg;
      // SpawnTask() will invoke application mentioned in System.conf; corresponding SL script invoked.
      CTask* new_task_transfer = Kernel::Instance()->SpawnTask (application_name, cmdline_arg,  TASK_STARTUP_API);

      CLog::Detail(THISMODULE, new_task_transfer, "**************** new_task_transfer for SL for call-transfer CREATED!!!!!!");

      sofiaCall->AttachTask(new_task_transfer);

      sofiaCall->SetState(CS_Initiating); // current call-state == CS_Null ; change it to another state to avoid signalling-connect error.

      Rtp_Media * rtpMedia=(Rtp_Media*) sofiaCall->GetMedia();
      sofiaCall->MediaAvailableInd(rtpMedia);
      sofiaCall->SetLocalSDPParameter((Rtp_Media*)rtpMedia);
      sofiaCall->DeviceAlertingReq(CP_InBandInfoAvailable, 0);

      // Negotiate codec
      Rtp_Payload_Code our_payload[RTP_MAX_CODEC_LIST];
      //ACE_INET_Addr remote_sig_addr;
      CLog::Warning(THISMODULE, "********IPPBX : sip->sip_to->a_url->url_host = %s\n", sip->sip_to->a_url->url_host);
      ACE_INET_Addr remote_med_addr("5060", ntohl(sip->sip_to->a_url->url_host));
      //remote_sig_addr = sip->sip_to->a_url->url_host;
      remote_med_addr = sip->sip_to->a_url->url_host;

      //PayloadValue payload;
      //payload.Init(RTP_PT_PCMU, LOSS_TYPE_G711PLC_RANDOM, 0, 20);

      Rtp_Payload_Code code = RTP_PT_PCMA;

      rtpMedia->Open();
      rtpMedia->Activate(remote_med_addr, code); 

      if (!sofiaCall->IsValid())
      {
        PLOGERR("********IPPBX : sofiaCall->IsValid() == false; ATTACHED CALL IS INVALID????\n");
      }
      else
      {
        PLOGERR("********IPPBX : ELSE....sofiaCall->IsValid() == TRUE; ATTACHED CALL IS VALID\n");
      }
      if (sofiaCall->IsUsingVirtualRtp())
      {
        PLOGERR("********IPPBX : sofiaCall->IsUsingVirtualRTP() == TRUE; using VIRTUAL RTP????\n");
      }
      else
      {
        PLOGERR("********IPPBX : ELSE....sofiaCall->IsUsingVirtualRtp() == FALSE; using REAL RTP\n");
      }
      ///////nua_update(nh,NUTAG_AUTOACK(0),TAG_END()); // UPDATE not sent out before of Invite ???

      // there is no special need to run new_task_transfer at high priority. so don't make task in READY state
      /****************
        new_task_transfer->Init();
        new_task_transfer->SetState_2( CTask::READY );
        Kernel::Instance()->QueueTask(new_task_transfer);
        new_task_transfer->Execute();

        while (new_task_transfer->GetState() == CTask::BLOCKED)
        {
        PLOGERR("****** wait until task is in BLOCKED state..... task in loop\n");
        new_task_transfer->SetState_2( CTask::READY );
        Kernel::Instance()->QueueTask(new_task_transfer);
        new_task_transfer->Execute();
        }
        while (new_task_transfer->GetState() == CTask::BLOCKED)
        {
        PLOGERR("****** wait until task is in BLOCKED state..... task in loop\n");
        new_task_transfer->SetState_2( CTask::READY );
        Kernel::Instance()->QueueTask(new_task_transfer);
        new_task_transfer->Execute();
        }
       ***************/
    }

    if (process_call_transfer || call_pickup_feature)
    {
      // for both call-transfer and call-pickup features, BYE needs to be sent out to the transferor / party-B.
      //////nua_bye(nh, TAG_END());

      //////nua_update(nh,NUTAG_AUTOACK(0),TAG_END()); // UPDATE not sent out due to BYE already sent above???
    }

    return true;
  }
  /****************
    switch_dtmf_t dtmf = { 0, switch_core_default_dtmf_duration(0), 0, SWITCH_DTMF_ENDPOINT };
    if (!strncasecmp(sip->sip_content_type->c_type, "application", 11) && !strcasecmp(sip->sip_content_type->c_subtype, "dtmf")) 
    {
    int tmp = atoi(sip->sip_payload->pl_data);
    dtmf.digit = switch_rfc2833_to_char(tmp);
    } else 
    {
  //goto end;
  return false;
  }

  if (dtmf.digit) 
  {
  if (tech_pvt->mparams.dtmf_type == DTMF_INFO || sofia_test_pflag(tech_pvt->profile, PFLAG_LIBERAL_DTMF) || 
  switch_channel_test_flag(tech_pvt->channel, CF_LIBERAL_DTMF)) 
  {
  // queue it up 
  switch_channel_queue_dtmf(channel, &dtmf);

  // print debug info
  switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_DEBUG, "INFO DTMF(%c)\n", dtmf.digit);

  if (switch_channel_test_flag(channel, CF_PROXY_MODE)) 
  {
  const char *uuid;
  switch_core_session_t *session_b;

  if ((uuid = switch_channel_get_partner_uuid(channel)) && (session_b = switch_core_session_locate(uuid))) 
  {
  while (switch_channel_has_dtmf(channel)) 
  {
  switch_dtmf_t idtmf = { 0, 0 };
  if (switch_channel_dequeue_dtmf(channel, &idtmf) == SWITCH_STATUS_SUCCESS) 
  {
  switch_core_session_send_dtmf(session_b, &idtmf);
  }
  }

  switch_core_session_rwunlock(session_b);
  }
  }

  // Send 200 OK response
  nua_respond(nh, SIP_200_OK, NUTAG_WITH_THIS_MSG(de->data->e_msg), TAG_END());
  } 
  else 
  {
  switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session), SWITCH_LOG_WARNING,
  "IGNORE INFO DTMF(%c) (This channel was not configured to use INFO DTMF!)\n", dtmf.digit);
  }
  } 
  else 
  {
  return false;
  }
   ********************/
#endif
  return false;
}

bool SofiaSIPSignalling::HandleIncomingRefer(int status, nua_handle_t * nh, sip_t const *sip, tagi_t tags[])
{
  CLog::Warning(THISMODULE, "IPPBX : HandleIncomingRefer");
#ifndef CALL_TRANSFER
  PLOGERR("************IPPBX: Inside HandleIncomingRefer() ; status = %d\n", status);
  // Send 200 OK response to this incoming INFO.
  nua_respond(nh, SIP_200_OK, TAG_END());

  g_registrationApiObj_for_callTransfer.CallTransferExtensionSent = true;
  //g_registrationApiObj_for_callTransfer.CallTransferExtension = sip->sip_refer_to->r_url->url_user;
#else

  calltransferflag = 1;    //CALL TRANSFER
  if(sip && sip->sip_from && sip->sip_from->a_url) {
    CLog::Detail(THISMODULE, "IPPBX : HandleIncomingRefer FROM %s ", sip->sip_from->a_url->url_user);
  }
  if(sip && sip->sip_from && sip->sip_refer_to && sip->sip_from->a_url && sip->sip_refer_to->r_url)
  {
    //if (call_transfer_extn.find(sip->sip_from->a_url->url_user) == call_transfer_extn.end())
    {
      CLog::Detail(THISMODULE, "IPPBX : HandleIncomingRefer REFER TO %s ", sip->sip_refer_to->r_url->url_user);
      //call_transfer_extn[sip->sip_from->a_url->url_user] = sip->sip_refer_to->r_url->url_user; //CALL TRANSFER
    }
  }

  CLog::Warning(THISMODULE, "IPPBX : HandleIncomingRefer Updated in MAP");

#endif

  return 0;
}

bool SofiaSIPSignalling::HandleIncomingActive(int status, nua_handle_t * nh, sip_t const *sip, tagi_t tags[])
{
  PLOGERR("************IPPBX: Inside  HandleIncomingActive() ; status = %d\n", status);

  if (!sip)
  {
    //		CLog::Warning(THISMODULE, "************IPPBX: sip == NULL\n");
  }
  else if (!sip->sip_to)
  {
    //		CLog::Warning(THISMODULE, "************IPPBX: sip->sip_to == NULL\n");
  }
  CallParameter cparam;
  CreateSofiaCallParam(cparam, sip, tags);

  //if(cparam.isHuntGroupCall)
  //{
  //	PLOGERR("************IPPBX: Inside  HandleIncomingActive() ; HUNT-GROUP CALL DETECTED ***************\n");
  //}
}

static char RFC2833_CHARS[] = "0123456789*#ABCDF";
char SofiaSIPSignalling::switch_rfc2833_to_char(int event)
{
  if (event > -1 && event < (int32_t) sizeof(RFC2833_CHARS)) {
    return RFC2833_CHARS[event];
  }
  return '\0';
}

// InitiateInviteCall_to_transferTarget() differs from HandleIncomingInvitation() that it avoids 407 or any other authentication while sending out this Invite to the called-party.
Call *SofiaSIPSignalling::SofiaSIPSignalling::InitiateInviteCall_to_transferTarget(int status, nua_handle_t * nh, sip_t const *sip, tagi_t tags[])
{
  // this is when signalling driver receive incoming INFO and when transferor wants to initiate call-transfer.
  // we initiate call during call-transfer without 407 proxy authentication required for it.
  CLog::Warning(THISMODULE, "******IPPBX : InitiateInviteCall_to_transferTarget()\n");
  PLOGERR("********IPPBX : FROM sip->sip_from->a_url->url_user = %s\n",sip->sip_from->a_url->url_user);
  PLOGERR("********IPPBX : FROM sip->sip_from->a_url->url_host = %s\n",sip->sip_from->a_url->url_host);
  PLOGERR("********IPPBX : TO sip->sip_to->a_url->url_user = %s\n",sip->sip_to->a_url->url_user);
  PLOGERR("********IPPBX : TO sip->sip_to->a_url->url_host = %s\n",sip->sip_to->a_url->url_host);

  // Allocate one media.
  CallParameter cparam;

  // let see if this new invite is actually
  // part of overlap re-INVITE mechanism
  SofiaSIPCall * prevCall = FindFromIncomingCallTable(sip->sip_from, sip->sip_call_id);
  if(prevCall)
  {
    // check if prevCall's state still not reached proceeding
    PLOGERR("********IPPBX : (prevCall) \n");
    if(prevCall->GetState()>=CS_Proceeding)
    {
      CLog::Warning(THISMODULE, "Incoming INVITE overlap, but the previous call has reach proceeding state");
      nua_respond(nh, 400,"Bad Request", TAG_END());
    }else
      // follow up this INVITE handle to that previous call instead
      prevCall->OverlapNuaHandle(status, nh, sip, tags);
    //return false;
    return NULL; // should return NULL in this case of call-transfer
  }

  CreateSofiaCallParam(cparam, sip, tags);
  cparam.cnCalled = sip->sip_to->a_url->url_user;

  PLOGERR("********IPPBX : TO sip->sip_to->a_url->url_user = %s\n",sip->sip_to->a_url->url_user);
  PLOGERR("********IPPBX : TO sip->sip_to->a_url->url_host = %s\n",sip->sip_to->a_url->url_host);

  if (strlen(cparam.csaCalled.c_str()) == 0)
  {
    CLog::Warning(THISMODULE, "************IPPBX : 4) cparam.csaCalled.c_str() == null\n");
    cparam.csaCalled = sip->sip_to->a_url->url_user;
    cparam.csaCalled += "@";
    cparam.csaCalled += sip->sip_to->a_url->url_host;
  }

  CLog::Warning(THISMODULE, "************IPPBX : 5) cparam.csaCallING.c_str() = %s\n", cparam.csaCalling.c_str());
  CLog::Warning(THISMODULE, "************IPPBX : 5) cparam.csaCallED.c_str() = %s\n", cparam.csaCalled.c_str());
  CLog::Warning(THISMODULE, "************IPPBX : 5) cparam.cnCallING.c_str() = %s\n", cparam.cnCalling.c_str());
  CLog::Warning(THISMODULE, "************IPPBX : 5) cparam.cnCallED.c_str() = %s\n", cparam.cnCalled.c_str());

  ACE_INET_Addr remote_sig_addr;// = sipcall->GetRemoteAddress();
  if(cparam.csaCalling.find(":")==String::npos)
  {
    remote_sig_addr.set(5060, cparam.csaCalling.c_str());
  }else
  {
    //remote_sig_addr= cparam.csaCalling.c_str();
    remote_sig_addr.set(cparam.csaCalling.c_str());
  }

  ACE_INET_Addr local_sig_addr;   // dummy
  /*if(sip->sip_via)
    {
    String addr;
    addr=sip->sip_via->v_host;
    if(StringExists(sip->sip_via->v_port))
    {
    addr+=":";
    addr+=sip->sip_via->v_port;
    }
    remote_sig_addr.set(addr.c_str());
    }else
    {
    CLog::Warning(THISMODULE, "Cannot find via header in INVITE packet from %s",
    sip->sip_from->a_url->url_host);
    nua_respond(nh, 400,"Bad Request", TAG_END());
    return false;
    }*/
  String addr;
  msg_t* msg=nua_current_request(SofiaSIPSignalling::Instance()->GetSofiaNua());
  get_msg_t_address(msg, addr);
  remote_sig_addr.set(addr.c_str());

  if(!sip->sip_contact)
  {
    CLog::Warning(THISMODULE, "Unable to find contact header in INVITE packet from %s",
        sip->sip_from->a_url->url_host);
    nua_respond(nh, 400,"Bad Request", TAG_END());
    //return false;
    return NULL; // should return NULL in this case of call-transfer
  }else
  {
    if(!stricmp(sip->sip_contact->m_url->url_scheme,"sips"))
    {
      // for now it appears that we dont have to set
      // sofia stack when receiving SIPS invite
    }
    else if(StringExists(sip->sip_contact->m_url->url_params))
      if(strstr(sip->sip_contact->m_url->url_params,"transport=tcp"))
      {
        nua_set_hparams(nh,
            NUTAG_M_PARAMS("transport=tcp"),
            TAG_END());
      }
  }

  Rtp_Interface *itf = RtpInterfaceManager::Instance()->DispatchIncomingCall(SigType_SOFIA, 
      remote_sig_addr,
      local_sig_addr,
      cparam);

  if(itf)
    defaultRealm = itf->GetDefaultRealm();

  // following if statement added by IPPBX for sake of printf
  if (itf==NULL)
    PLOGERR("********IPPBX : itf = NULL\n");

  Media *med = NULL;

  if (itf==NULL || !itf->AllocateMedia(NULL, med, 0, CD_Incoming, &cparam) || med==NULL) {

    CLog::Warning(THISMODULE, "Unable to dispatch incoming Sofia SIP call from '%s'",
        sip->sip_from->a_url->url_host);
    PLOGERR("\n*****IPPBX : 500, Server Internal Error, TAG_END()");
    nua_respond(nh, 500, "Server Internal Error", TAG_END());
    CLog::Warning(THISMODULE, "Unable to dispatch incoming Sofia SIP call from '%s'",sip->sip_from->a_url->url_host);
    //return false;
    return NULL; // should return NULL in this case of call-transfer
  }

  Rtp_Media *rtp_med = (Rtp_Media*)med;

  // Create a new call.
  SofiaSIPCall *sofiaCall;
  NEW_RETURN_(sofiaCall, SofiaSIPCall(cparam, CD_Incoming, med->GetInterface()->Name().c_str()));
  if (!sofiaCall) {
    med->GetInterface()->ReleaseMedia(med, CD_Incoming, &cparam, CC_ResourceUnavailable);
    PLOGERR("\n*****IPPBX : 500, Server Internal Error, TAG_END() ...2)");
    nua_respond(nh, 500, "Server Internal Error", TAG_END());
    //return false;
    return NULL; // should return NULL in this case of call-transfer
  }
  sofiaCall->MediaAvailableInd(med);
  // when remote party ask for timer, we add "required:timer"
  if(sip->sip_session_expires)
    sofiaCall->requestSessionTimer(TRUE);
  else
    sofiaCall->requestSessionTimer(FALSE);


  // we must NUTAG_AUTOANSWER(0) again despite this already done
  // in higher, general nua parameters (sofia's bug)
  nua_set_hparams(nh,
      NUTAG_AUTOANSWER(0),

      TAG_IF(!(sofiaCall->IsUsingVirtualRtp()), NUTAG_ALLOW("UPDATE")),
      TAG_END());


  sofiaCall->attachSofiaNuaHandle(nh);  // is this really required during call-transfer????? commented temporarily!!! uncomment later...
  AddToIncomingCallTable(sofiaCall, sip->sip_from, sip->sip_call_id);

  // no authentication required in case of call-transfer...
  /*************************
    bool has_auth;
    XgSipAuthEntry* auth=new XgSipAuthEntry;

    has_auth=GetSipAuthEntryFromSofiaSip(*auth, sip);
    if(has_auth==false)
    {
    PLOGERR("********IPPBX : has_auth==false\n");
    delete auth;
    auth=NULL;
    if(sofiaCall->RequireAuthorization())
    {
    SofiaSIPSignalling::ChallengeRequest(nh, 401, defaultRealm.c_str());
    sofiaCall->attachSofiaNuaHandle(NULL);
    PLOGERR("********IPPBX : med->GetInterface()->ReleaseMedia(med, CD_Incoming, &cparam, CC_FacilityRejecte()\n");
    med->GetInterface()->ReleaseMedia(med, CD_Incoming, &cparam, CC_FacilityRejected);
    return false;
    }
    PLOGERR("********IPPBX : has_auth==false ...........here\n");
    }else
    {
    PLOGERR("********IPPBX : has_auth==true\n");
    if(sofiaCall->RequireAuthorization())
    if (!itf->Authorize(auth->authUser)) {
    PLOGERR("********IPPBX : (_respond(nh, 403, Forbidden, NUTAG...\n");
    nua_respond(nh, 403, "Forbidden", NUTAG_AUTOANSWER(0), TAG_END());
    sofiaCall->attachSofiaNuaHandle(NULL);
    med->GetInterface()->ReleaseMedia(med, CD_Incoming, &cparam, CC_FacilityRejected);
    return false;
    }
    }
   ***********************/

  if(!sofiaCall->IsUsingVirtualRtp())
  {
    PLOGERR("********IPPBX : (!sofiaCall->IsUsingVirtualRtp() ; using real Rtp ???\n");
    sofiaCall->SetLocalSDPParameter(rtp_med);
  }
  else
  {
    PLOGERR("********IPPBX : sofiaCall->IsUsingVirtualRtp() == TRUE **************\n");
  }

  // following paragraph added by IPPBX;
  // before sending INVITE to called party UA, 401 auth. check is done above.
  // 407 proxy authorization will be done here before sending INVITE to called party UA.
  PLOGERR("********IPPBX : code to send INVITE to called party UA\n");
  //	if (status == scProxyAuthenticationRequired) 
  //	{
  //    		PLOGERR("********IPPBX : (status == scProxyAuthenticationRequired)\n");
  //		SofiaSIPSignalling::ChallengeRequest(nh, 407, SofiaSIPSignalling::Instance()->GetRealm().c_str(),

  /////	if (sip->sip_authorization || sip->sip_proxy_authorization) {
  /////    		PLOGERR("********IPPBX : (sip->sip_authorization || sip->sip_proxy_authorization\n");
  /////		SofiaSIPSignalling::ChallengeRequest(nh, 407,  defaultRealm.c_str());
  /////	}

  //reg_info->auth.authStale, req->GetRegisterRequest());
  //			NULL, req->GetRegisterRequest());

  //			SofiaSIPSignalling::ChallengeRequest(nh, 407, defaultRealm.c_str()),
  //			TAG_IF(ref,NUTAG_WITH(ref)),
  //			TAG_END());

  //nua_respond(nh, 407, "Proxy Authentication Required", TAG_END());
  //nua_authenticate(nh, 407, "Proxy Authentication Required", TAG_END());
  ///////sofiaCall->AuthenticateCall(status, sip->sip_proxy_authenticate);

  ///nua_respond(nh, 407, "Proxy Authentication Required", NUTAG_AUTOANSWER(0), TAG_END());
  //	}

  CLog::Warning(THISMODULE, "************IPPBX : 6) cparam.csaCallING.c_str() = %s\n", cparam.csaCalling.c_str());
  CLog::Warning(THISMODULE, "************IPPBX : 6) cparam.csaCallED.c_str() = %s\n", cparam.csaCalled.c_str());
  CLog::Warning(THISMODULE, "************IPPBX : 6) cparam.cnCallING.c_str() = %s\n", cparam.cnCalling.c_str());
  CLog::Warning(THISMODULE, "************IPPBX : 6) cparam.cnCallED.c_str() = %s\n", cparam.cnCalled.c_str());

  // Initiate a new outgoing call with the called party UA -by sending INVITE to the called party UA.
  // is SetupReq_during_call_transfer() required to be invoked -as INVITE will be invoked by SL-script!
  // probably yes - but send re-Invite to party-A -tomake second call-leg cerated in SL has valid state.

  SofiaSIPSignalling::SetupReq_during_call_transfer(itf,NULL,&cparam, nh); // ****** IPPBX
  /////////SofiaSIPSignalling::SetupReq(itf,NULL,&cparam); // ****** IPPBX
  PLOGERR("End of InitiateInviteCall_to_transferTarget() \n");
  //return true;
  return sofiaCall; // finally return the sofiaCall.
}

bool SofiaSIPSignalling::HandleIncomingBye(int status, nua_handle_t * nh, sip_t const *sip, tagi_t tags[])
{
  // received BYE from a party on call that wants to terminate call.
  // forward BYE to other end party.
  PLOGERR("******IPPBX : HandleIncomingBye()\n");
  if (!sip)
  {
    //		CLog::Warning(THISMODULE, "************IPPBX: sip == NULL\n");
    return;
  }
  else if (!sip->sip_to)
  {
    //		CLog::Warning(THISMODULE, "************IPPBX: sip->sip_to == NULL\n");
    return;
  }
  // populate sip_to with that of transfer-target
  //strcpy(sip->sip_to->a_url->url_user , "405");
  //strcpy(sip->sip_to->a_url->url_host , "80.74.227.71");
  //	strcpy(sip->sip_to->a_url->url_host , "10.22.7.75");
  //strcpy(sip->sip_to->a_url->url_host , "10.22.3.94");
#if 0
  printf("HandleIncomingBye sip->sip_from->a_url->url_user = %s \n", sip->sip_from->a_url->url_user );
  printf("HandleIncomingBye sip->sip_from->a_url->url_host = %s \n", sip->sip_from->a_url->url_host );
  printf("HandleIncomingBye sip->sip_to->a_url->url_user = %s \n", sip->sip_to->a_url->url_user );
  printf("HandleIncomingBye sip->sip_to->a_url->url_host = %s \n", sip->sip_to->a_url->url_host );
  if (sip && sip->sip_to && sip->sip_to->a_url && sip->sip_to->a_url->url_host && sip->sip_from && sip->sip_from->a_url && sip->sip_from->a_url->url_host)
  {
    char key[256];
    memset(key,0,sizeof(key));
    char key_t[256];
    memset(key_t,0,sizeof(key_t));
    char *token = NULL;
    if (strcasestr(sip->sip_to->a_url->url_host,".UR.mundio.com"))
    {
      token = strtok(sip->sip_to->a_url->url_host, "_");
    } else if (strcasestr(sip->sip_from->a_url->url_host,".UR.mundio.com"))
    {
      token = strtok(sip->sip_from->a_url->url_host, "_");
    }
    if (token != NULL)
    {
      while(token != NULL)
      {
        printf(" 1 token = %s \n", token);
        sprintf(key_t,"%s",token);
        token = strtok(NULL, "_");
        printf(" 2 token = %s \n", token);
        if(token !=NULL)
        {
          sprintf(key_t,"%s",token);
          printf(" 3 key_t = %s \n", key_t);
        }
      }
      printf("4 key_t = %s \n", key_t);
      sprintf(key,"%s_%s@%s", sip->sip_from->a_url->url_user, sip->sip_to->a_url->url_user, key_t);
      printf(" ^^^^^^^^ call_transfer_replace erase %s \n", key);
      call_transfer_replace.erase(key);
    }
  }/* else if (sip && sip->sip_from && sip->sip_from->a_url && sip->sip_from->a_url->url_host && strstr(sip->sip_from->a_url->url_host,".UR.mundio.com"))
      {
  // remove map entry
  sprintf(key,"%s_%s@%s", sip->sip_from->a_url->url_host, sip->sip_to->a_url->url_host, domain_key)
  call_transfer_replace.erase(key);
  }*/
#endif
#ifdef XGATE_HAS_ACME_SBC
  //TODO: Yoga need to forward BYE to proper peer. changes done while new MGC-PBX integration
  nua_bye(nh, \
	//SIPTAG_REQUEST_STR("201<sip:201@10.30.3.38>;tag=aH957vjUSFHtH"),
	TAG_END());
#else
  nua_bye(nh, TAG_END());
#endif
  // nua_client_create(nh, nua_r_bye, &nua_bye_client_methods, NULL);
  PLOGERR("******IPPBX : BYE sent out\n");
}

// CreateOutgoingNuaHandleFromCParam_for_call_transfer() differs from CreateOutgoingNuaHandleFromCParam() that it sets Transfer-targets's
// dispplay name or user = 401.
nua_handle_t* CreateOutgoingNuaHandleFromCParam_for_call_transfer(const CallParameter *param, bool privateInfoAllowed)
{
  /* create operation handle */
  nua_handle_t * handle;
  String host;
  String port;
  String user;
  String password;
  String scheme;
  String params;
  sip_via_t via[1];

  std::stringstream dest;
  std::stringstream contact;
  std::stringstream src;
  std::stringstream pai;
  bool has_pai=false;
  bool has_privacy=false;
  bool isTLS=false;
  bool isTCP=false;
  bool cuiAllowed=false;
  bool cpiAllowed=false;

  su_home_t su_home;
  su_home_init(&su_home);
  url_t* url;
  url_t* via_url;
  PLOGERR("\n************IPPBX: Inside  CreateOutgoingNuaHandleFromCParam_for_call_transfer() **************\n");
  PLOGERR("\n************IPPBX: param->csaCalled.c_str() = %s\n",param->csaCalled.c_str());
  PLOGERR("\n************IPPBX: param->csaCalling.c_str() = %s\n",param->csaCalling.c_str());
#if 1
  /////strcpy(param->csaCalled.c_str(),"192.168.13.62");
  //strcpy(param->csaCalled.c_str(),"192.168.11.53");
  ////strcpy(param->csaCalled.c_str(),"192.168.13.66");
#endif
  url=url_make(&su_home, param->csaCalled.c_str());
  if(!url)
  {
    CLog::Warning ( THISMODULE, "Error when parsing called signal address format in call parameter: '%s'",
        param->csaCalled.c_str());
    return NULL;
  }
  if(!StringExists(url->url_host))
  {
    CLog::Warning ( THISMODULE, "Error when parsing called signal address format in call parameter(no url_host): '%s'",
        param->csaCalled.c_str());
    return NULL;
  }

  ParseUserPassword(param->cnCalled, user, password);
  PLOGERR("\n************IPPBX: user = %s\n", user.c_str());
  if (param->cnpCalled == CNP_ISDN) {
    // what if?
  }

  // assembly To string
  // no display name? use user instead
  dest<< "\"" << user << "\" <";
  if(StringExists(url->url_scheme))
  {
    PLOGERR("\n************IPPBX: StringExists(url->url_scheme) %s\n", user.c_str());

    if(!stricmp(url->url_scheme,"sips"))
    {
      PLOGERR("\n************IPPBX: !stricmp(url->url_scheme, sip\n");
      dest<< url->url_scheme<<":";
      isTLS=true;
    }else
    {
      CLog::Warning(THISMODULE, "\n************IPPBX: sip:\n");
      dest<< "sip:";
    }
  }
  else
  {
    PLOGERR("\n************IPPBX: ELSE sip:\n");
    dest<< "sip:";
  }
  if(StringExists(url->url_params))
    if(strstr(url->url_params,"transport=tcp"))
      isTCP=true;

  //dest << user << "@" << url->url_host; // commented by IPPBX
  dest << user << "@" << url->url_host;

  if(StringExists(url->url_port))
    dest<<":"<<url->url_port;

  PLOGERR("************IPPBX: dest = %s\n",dest.str().c_str()); 
  // hardcoding added by IPPBX for testing; remove it later
  dest<<":"<<"5060";
  dest<<";"<<"transport=udp";
  PLOGERR("************IPPBX: dest = %s\n",dest.str().c_str()); 

  if(StringExists(url->url_params))
    dest<<";"<<url->url_params;
  dest<<">";

  url_sanitize(url);

  /*url=url_make(&su_home, param->csaCalling.c_str());
    if(!url)
    {
    CLog::Warning ( THISMODULE, "Error when parsing calling signal address format in call parameter: '%s'",
    param->csaCalling.c_str());
    return NULL;
    }

    if (ParseHostPort (param->csaCalling, host, port) == false)
    CLog::Warning ( THISMODULE, "Invalid origin signal address format in call parameter: '%s'",
    param->csaCalling.c_str());*/

  ParseUserPassword(param->cnCalling, user, password);

  if(!StringExists(user.c_str()))
  {
    CLog::Warning ( THISMODULE, "Invalid origin cnCalling in call parameter: '%s'",
        param->cnCalling.c_str());
    return NULL;
  }

  scheme="sip";
  host=sofiaHostAddress;
  //if(StringExists(url->url_port))
  //	port=url->url_port;
  //if(StringExists(url->url_params))
  //	params=url->url_params;

  //url_sanitize(url);
  su_home_deinit(&su_home);

  /* 
   *	Get Current Local IP 
   */


  if (param->cnpCalling == CNP_ISDN) {

    if (user == "") {
      // unavailable@unknown.invalid
      scheme="sip";
      user="unavailable";    
      host="unknown.invalid";
      port="";
      params="";
    } else {
      /** Commented out temporarily due to backward compatibility issue
        if (param->cntCalling == CNT_International && user.length()>0 && user.substr(0, 1) != "+")
        user = "+" + user;
       **/

      if (param->ccp == CCP_Restricted) {
        // Add P-Asserted Identity
        has_pai=true;

        // Add Privacy header
        has_privacy=true;
      } else 
      {
        // Add P-Asserted-Identity header
        has_pai=true;
      }
    }
  }
  if(param->cntCalling==CNT_International && user[0]!='+')
    user='+'+user;
  // assembly PAI(if any)
  if(has_pai)
  {
    pai<< "\"" << user << "\"";
    pai<< "<"<<scheme<<":" << user << "@" << host;
    if(SOFIA_SIP_PORT!=5060)
      pai<<":"<<SOFIA_SIP_PORT;
    pai<<";user=phone";
    pai<<">";
  }

  contact<< "\"" << user << "\"";
  if(isTLS)
    contact<< "<sips:" << user << "@" << host;
  else
    contact<< "<sip:" << user << "@" << host;
  if(SOFIA_SIP_PORT!=5060)
    contact<<":"<<SOFIA_SIP_PORT;
  if(isTCP)
  {
    PLOGERR("************IPPBX: if(isTCP)\n");
    // original code line below commented by IPPBX; un-comment it later.
    ///////contact<<";transport=tcp";
    contact<<";transport=udp"; // forcefully adding udp for sake of testing.
  }
  // code added by IPPBX for testing; remove it later; added entire ëlse" statement below...
  else
  {
    PLOGERR("************IPPBX: else add transport = udp\n");
    // else add transport = udp.
    contact<<";transport=udp";
    dest<<";transport=udp";
  }
  if(StringExists(params.c_str()))
    contact<<";"<<params;
  contact<<">";

  PLOGERR("************IPPBX: dest = %s\n",dest.str().c_str()); 

  /*************************
    if(has_privacy)
    {
    scheme="sip";
    user="anonymous";	// anonymous@anonymous.invalid
    host="anonymous.invalid";
    port="";
    params="";
    }



  // assembly contact
  /*nua_get_params(SofiaSIPSignalling::Instance()->GetSofiaNua(),
  NUTAG_URL_REF(host_addr),
  TAG_END());***********
  // only temporary:


  // assemby From string
  /*src<< "\"" << user << "\"";
  src<< "<"<<scheme<<":" << user << "@" << host;
  if(port!="")
  src<<":"<<port;
  if(params!="")
  src<<";"<<params;
  src<<">";***********

  src<< "\"" << user << "\"";
  src<< "<"<<scheme<<":" << user << "@" << host;
  if(SOFIA_SIP_PORT!=5060)
  src<<":"<<SOFIA_SIP_PORT;
  src<<">";

  // reuse params for user info
  if(StringExists(param->cui.c_str()))
  {
  params=XGATE_SOFIA_HEADER_USER_INFO;
  params+=": ";
  params+=SipHeaderEncode(param->cui);
  cuiAllowed=privateInfoAllowed;
  }

  // reuse scheme for private info
  if(StringExists(param->cpinfo.c_str()))
  {
  scheme=XGATE_SOFIA_HEADER_PRIVATE_INFO;
  scheme+=": ";
  scheme+=SipHeaderEncode(param->cpinfo);
  cpiAllowed=privateInfoAllowed;
  }
   ****************************/

  handle = nua_handle(SofiaSIPSignalling::Instance()->GetSofiaNua(), NULL, 
      SIPTAG_TO_STR(dest.str().c_str()), 
      SIPTAG_VIA(via),
      SIPTAG_FROM_STR(src.str().c_str()),
      SIPTAG_CONTACT_STR(contact.str().c_str()),
      TAG_IF(has_pai, SIPTAG_P_ASSERTED_IDENTITY_STR(pai.str().c_str())),
      TAG_IF(has_privacy, SIPTAG_PRIVACY_STR("id")),
      /*NUTAG_ACCEPT_MULTIPART(1),*/
      TAG_IF(cuiAllowed, SIPTAG_HEADER_STR(params.c_str())),
      TAG_IF(cpiAllowed, SIPTAG_HEADER_STR(scheme.c_str())),

      // we must NUTAG_AUTOANSWER(0) again despite this already done
      // in higher, general nua parameters (sofia's bug)
      NUTAG_AUTOANSWER(0),
      NTATAG_REL100(1),

      //SIPTAG_REQUIRE_STR("precondition"), !!! should be configurable

      NUTAG_ALLOW("REFER"), // REFER is ro be allowed in call-transfer
      /*NUTAG_ALLOW("NOTIFY"),
        NUTAG_ALLOW("MESSAGE"),
        NUTAG_ALLOW("SUBSCRIBE"),
        NUTAG_ALLOW("INFO"),
        */

      TAG_END());
  return handle;
}

// SetupReq_during_call_transfer: Initiate a new outgoing call in a specific Interface using the specified call parameter.
// it differs with SetupReq() that it calls CreateOutgoingNuaHandleFromCParam_for_call_transfer() which is required to set
// Transfer-targets's display name e.g. "401" during hardcoding. otherwise CreateOutgoingNuaHandleFromCParam() is already being used
// by HandleIncomingInvitation() during first receive of INVITE.
Call *SofiaSIPSignalling::SetupReq_during_call_transfer(Interface *itf,SelectionStrategy selection,CallParameter *cparam, nua_handle_t *nh)
{
  PLOGERR("\n*****IPPBX : inside SofiaSIPSignalling::SetupReq_during_call_transfer() ************\n");
  SofiaSIPCall * sofiaCall;
  NEW_RETURN_ (sofiaCall, SofiaSIPCall(*cparam, CD_Outgoing, itf->Name().c_str()));
  if(!sofiaCall)
    return NULL;

  // Allocate one RTP media
  Rtp_Interface *rtp_itf = (Rtp_Interface *)itf;
  Media *rtp_media = NULL;
  if (!rtp_itf->AllocateMedia(sofiaCall, rtp_media, 0, CD_Outgoing, cparam)) {
    CLog::Warning(THISMODULE, "***************IPPBX : rtp_itf->AllocateMedia FAILING ***************\n");
    assert(rtp_media == NULL);
    sofiaCall->SetCause(CC_ResourceUnavailable);
    return sofiaCall;
  }

  sofiaCall->MediaAvailableInd(rtp_media);

  // Pass silence compression setting to SDP for G.729 attribute
  /*RTP_Config config;
    rtp_itf->GetRtpChannelConfig(config);
    xcall->silenceCompression = config.SilenceCompression;*/

  // actually create request sofia to create call
  //operation *op;


  //nua_handle_t*handle = CreateOutgoingNuaHandleFromCParam(cparam, rtp_itf->IsPrivateInfoAllowed());
  nua_handle_t*handle = CreateOutgoingNuaHandleFromCParam_for_call_transfer(cparam, rtp_itf->IsPrivateInfoAllowed()); // setting display name to "401" as well

  //////nua_handle_t* handle = nh;

  if (handle == NULL) {
    CLog::Warning(THISMODULE, "Cannot create outgoing NUA handle!");
    sofiaCall->SetCause(CC_ResourceUnavailable);
    sofiaCall->DetachMedia();

    return sofiaCall;
  }    

  CLog::Warning(THISMODULE, "***************IPPBX : SetupReq_during_call_transfer() : handle created  ***************\n");
  CLog::Warning(THISMODULE, "************IPPBX : cparam->csaCallING.c_str() = %s\n", cparam->csaCalling.c_str());
  CLog::Warning(THISMODULE, "************IPPBX : cparam->csaCallED.c_str() = %s\n", cparam->csaCalled.c_str());
  CLog::Warning(THISMODULE, "************IPPBX : cparam->cnCallING.c_str() = %s\n", cparam->cnCalling.c_str());
  CLog::Warning(THISMODULE, "************IPPBX : cparam->cnCallED.c_str() = %s\n", cparam->cnCalled.c_str());

  sofiaCall->attachSofiaNuaHandle(handle);

  // set call credential
  sofiaCall->SetCallCredential(cparam->credential);

  // allow update if it's not virtual RTP
  if(sofiaCall->IsUsingVirtualRtp())
  {
    CLog::Warning(THISMODULE, "***************IPPBX : sofiaCall->IsUsingVirtualRtp() ***************\n");

    if(!sofiaCall->peerSipCall || !sofiaCall->peerSipCall->IsValid())
    {
      CLog::Warning(THISMODULE, "Outgoing Sofia Call using virtual RTP must have incoming peer call!");
      sofiaCall->SetCause(CC_InvalidCallReference);
      sofiaCall->DetachMedia();

      return sofiaCall;
    }

    sofiaCall->SetLocalSDPParameter(sofiaCall->peerSipCall->savedRemoteSdp);
  }else
  {
    CLog::Warning(THISMODULE, "***************IPPBX : sofiaCall->IsUsingVirtualRtp() == FALSE ; using REAL media ***************\n");
    nua_set_hparams(handle,
        NUTAG_ALLOW("UPDATE"),
        TAG_END());

    //SofiaPrecondition * precond=new SofiaPrecondition(PreconditionLevel_No, PreconditionLevel_No,
    //	                                              PreconditionLevel_Mandatory,PreconditionLevel_Mandatory);
    //sofiaCall->SetPrecondition(precond);
    sofiaCall->SetLocalSDPParameter((Rtp_Media*)rtp_media);
  }

  // Don't send INVITE ; this will be done by SL-script.
  //sofiaCall->InviteCall();


  return sofiaCall;
}

bool RegistrationApi::ValidRegisterParameter(CTask* task,RegisterParam* rparam,const char* funcname)
{
  //if ( rparam && rparam->IsValid()) return true;

  CLog::Error(THISMODULE, task, "Invalid call parameter handle 0x%x passed in %s", rparam, funcname);
  return false;
}

//* API to modify or retrieve a particular register parameter field
#define DEFINE_SETREGISTERPARAM(F,vtype,field) \
  STATUS RegistrationApi::F(CTask* task,RegisterParam* rparam,vtype value) \
{ \
  //if (!ValidCallParameter(task,rparam,#F)) return NOT_OK; \
  rparam->field = value; \
  return OK; \
}

#define DEFINE_GETREGISTERPARAM_STR(F,field, vtype) \
  STATUS RegistrationApi::F(CTask* task,char* retval,RegisterParam* rparam) \
{ \
  //if (!ValidCallParameter(task,rparam,#F)) return NOT_OK; \
  safe_strcpy(retval,rparam->field.c_str(),MAXAPPSTRLEN); \
  return OK; \
}

//DEFINE_SETREGISTERPARAM(SetSessionId,const char*,csi)
//DEFINE_GETREGISTERPARAM_STR(API_GetSipUri,char*)

/**************
//* Interfaces exposed to Kernel and user applications

STATUS RegistrationApi::GetRegistrationParam(CTask* task,RegistrationApi **rparam,int handle)
{
Call* call = _manager->CallFromHandle(task,handle,true,"CallGetCallParam");
if (call) {
TaskCallBlock* tcb = _manager->GetTaskCallBlock(task);
tcb->_cparam = *call->GetCallParameter();
 *rparam = &tcb->_cparam;
 call->_overlapRcvd = "";
 } else {
// *rparam = (CallParameter *)CRC_InvalidCallHandle;
// Linux cannot handle -4 which is invalid address 
 *rparam = 0;
 }
 return OK;
 }
 ***************/

#endif // XGATE_HAS_SIP_IPPBX

#endif	// XGATE_HAS_SOFIA
