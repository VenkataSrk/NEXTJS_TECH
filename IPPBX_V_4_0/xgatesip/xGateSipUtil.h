#ifndef _XGATE_SIP_UTIL_H
#define _XGATE_SIP_UTIL_H
#include <ace/Reactor.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch.h>
#include <ace/OS.h>
#include <ace/Task.h>
#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>
#include <ace/Token.h>
#define XGATETERMINATE TRUE // Enable this if you need to compile & deploy for xgate Termination side
#define MAXCODEC 9 
#define MAXAVCODEC 9
#define MAXMEDIA 2

#define CONF_MAX_PARTICIPANTS	50

#define WAV_FILE_PATH  "/root/wav_files/sounds/vectone/"

#define CALLHANDLING_SIMULTANEOUS_RINGING	true
#define CALLHANDLING_SEQUENTIAL_RINGING		false

#define	DESKPHONE_EXTENSION_MAXLENGTH	6
#define MOBILE_EXTENSION_MINLENGTH	10
#define KVSTR(k,v) "\"" k "\"" << ":" << "\"" << v << "\""
#define KVINT(k,v) "\"" k "\"" << ":" <<  v
#define KVBOOL(k,v)"\"" k "\"" << ":" <<  ((v) ? "true" : "false")
//#define jcolon ":"
#define KVC  ","
#define KVS  "{"
#define KVE  "}"
//#define SAVE_RECORD_URL "http://ursip-api.unifiedring.co.uk/v1/call_recorder/save_recorder"
namespace sipreturn{
typedef enum xGateReturn{
FAIL=0,
SUCCESS,
};
}
// Used  to check the sip header validation
typedef enum SIPVALIDATE{
    FROM=1,
    TO,
    REQUEST,
    CONTACT,
    TAG,
    CALLID,
    VIA,
    SUBJECT,
    CONTACTLEN,
    PAI //P-Asserted Identity


};
/* _REL Reply(Reply request) : from processer to sofia 
*  _REQ Request (incoming Request) :  from sofia to receiver 
*  _RES Responce (outgoing Responce): from sofia to receiver 
*  Enumarate to post and receive message between Receiver to Processor & Dispatcher and vice versa Threads
*/

typedef enum XGATESIPEVENT{
    EN_NULL=0,
    EN_INVITE_REQ,
    EN_INVITE_RES,
    EN_REINVITE_REQ,
    EN_REINVITE_REL,
    EN_HOLDRESUME_REQ,
    EN_HOLDRESUME_REL,
    EN_TRANSFER_REQ,
    EN_TRANSFER_REL,	
    EN_P2PMEDIA_REQ,
    EN_P2PMEDIA_REL,
    EN_REGISTER_REQ,
    EN_REGISTER_RES,
    EN_MESSAGE_REQ,
    EN_MESSAGE_RES,
    EN_BYE_REQ,
    EN_BYE_RES,
    EN_TERMINATED_REQ,
    EN_TERMINATED_REL,
    EN_TIMEOUT_REL,
    EN_DIALOUT_REL,
    EN_SETMEDIA_OREL, // Originatin Reply
    EN_SETMEDIA_TREL, // Termination Reply
    EN_SETMEDIA_EREL, // Early Media Reply
    EN_MP3_DETAIL,
    EN_PB_COMPLETE_REL,// Play Back Complet from MG
    EN_FORKING_REQ, // to Send forking push notification
    EN_APPREGCOMPLET_REQ, // Message on  Regisrtation complet on push notification 
    EN_FOKINGDIAL_REL, // Forkign Dail Out after push notification  
    EN_MEDIA_UPDATE_REL,
    EN_SWB_EVENT_REQ,
    EN_SWB_EVENT_REL,
    EN_CDR_UPDATE_REQ,// CDR updation from Signalling Layer to Business service
    EN_100_REL=100,
    EN_180_REL=180,
    EN_181_REL,
    EN_182_REL,
    EN_183_REL,
    EN_199_REL=199,
    EN_200_REL=200,
    EN_202_REL=202,
    EN_204_REL=204,
    EN_300_REL=300,
    EN_400_REL=400,
    EN_401_REL,
    EN_402_REL,
    EN_403_REL=403,
    EN_404_REL,
    EN_407_REL=407,
    EN_408_REL=408,
    EN_410_REL=410,
    EN_480_REL=480,
    EN_484_REL=484,
    EN_486_REL=486,
    EN_487_REL,
    EN_488_REL=488,
    EN_500_REL=500,
    EN_501_REL,
    EN_502_REL,
    EN_503_REL,
    EN_504_REL,
    EN_510_REL=510,
    EN_600_REL=600,
    EN_603_REL=603,
    EN_604_REL,
    EN_606_REL=606,
    EN_607_REL=607,
    EN_UNKNOWN_SIP,
    EN_CALLSTATE_UPDATE_REL,
    EN_SIPINFO_REQ,
    EN_SWITCHBOARD_REQ,
    EN_MEDIA_GETDIGIT_REL,
    EN_MEDIA_RECVDIGIT_REL,
    EN_DIAL_TIMEOUT_RES,
    EN_SWBMAINMENUIVR_TIMEOUT_RES,
    EN_CONF_EVENT_REQ,
    EN_CONF_EVENT_REL,
    EN_MULTICONF_MEDIA_REL,
    EN_CONFERENCEIVR_TIMEOUT_RES,
    EN_APPNOTIFICATION_TIMEOUT_RES,
    EN_CONFERENCE_DTMFDIGIT_TIMEOUT_RES,
    EN_PIN_TO_CONNECT,
    EN_CALLHANDLING_TIMEOUT_RES
};
// Enumuration to get Message handler
typedef enum XGATEPOSTTYPE{
    EN_UNKNOWN=0,
    EN_DISPATCH,
    EN_PROCESS,
    EN_DBMANAGER
};

typedef enum XGATECALLSTREAM { //Need to depeceiate soon
    EN_IDLE = 0,
    EN_INBOUND,
    EN_OUTBOUND
};

typedef enum XGATESIPKEY{
    EN_USER=0,
    EN_PWD,
    EN_HOST,
    EN_PORT,
    EN_PARAM,
    EN_SCHEMA
};
typedef enum XGATESDPMODE{
EN_SDP_NULL=0,
EN_SDP_DIALOUT,
EN_SDP_RESPONSE,
EN_SDP_PEERTOPEER
};
typedef enum XGATEMGUPDATE{
   EN_MEDIA_NULL = 0,
   EN_MEDIA_CREATE,
   EN_MEDIA_UPDATE,
   EN_MEDIA_18X,
   EN_MEDIA_DELETE,
   EN_MEDIA_HOLD_RESUME = 9,
   EN_MEDIA_SL_PLAY_FILE,
   EN_MEDIA_GET_DTMF_DIGITS,
   EN_MEDIA_SL_DTMF_RESUME,
   // Conference-related 
   EN_MEDIA_MUTE,
   EN_MEDIA_UNMUTE,
   EN_MEDIA_DEAF,
   EN_MEDIA_RECORD,
   EN_MEDIA_CONF,
   EN_MEDIA_CONF_ADDPARTICIPANT,
   EN_MEDIA_CONF_DELPARTICIPANT,
   EN_MEDIA_MULTICONF_REQ_PORT ,// AD-HOC conference port request
   EN_MEDIA_MULTICONF_CREATE, // AD-HOC conference media channel creation 
   EN_MEDIA_CONF_END,
   EN_MEDIA_RECORD_REQ_PORT,
   EN_MEDIA_RECORD_CREAT,
   EN_MEDIA_RECORD_START,
   EN_MEDIA_RECORD_STOP,
   EN_MEDIA_RECORD_HOLD,
   EN_MEDIA_RECORD_RESUME,
   EN_MEDIA_RECORD_DELETE
};
typedef enum XGATERBTSTATE{
   EN_NULLRBT = 0,
   EN_PLAYRBT,
   EN_STOPRBT,
   EN_CLEARRBT
};
typedef enum XGATEPLAYFILE{ // On B2B file need too be copied to MG base 
 EN_FILE_NULL=0,
 EN_FILE_DELAY,
 EN_FILE_RBT,
 EN_FILE_HOLD,
 EN_FILE_GREETING,
 EN_FILE_CONNECTINGMSG,
 EN_FILE_CALLFWD,
 EN_FILE_IVR,
 EN_FILE_DISCONNECT
};
typedef enum XGATEOBJECT{ // Provided the xgateSofiaObject Type
EN_OT_NULL=0,
EN_OT_ORIGINATION = XGATECALLSTREAM::EN_INBOUND,
EN_OT_DESTINATION = XGATECALLSTREAM::EN_OUTBOUND,
EN_OT_BOTH
};
typedef struct XGATECODECLIST{
int payload;
const char* codecname;
unsigned long samplingrate;
const char *fmtp;
const char *mode; //sterio or mono
int type; //audio= 2 or video = 3
};

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

// Media state will make the communicatin state for sofia and mg
typedef enum XGATEMEDIASTATE{
	EN_MED_NULL=0,
	EN_MED_REQ,
	EN_MED_SET,
	EN_MED_CONNECTED,
	EN_MED_SUSPENDED,
	EN_MED_RESUMED,
	EN_MED_RELEASE
};
typedef enum REGAAA{
 EN_AAA_NULL = 0,    // 000
 EN_AAA_AUTHEN,      // 001
EN_AAA_AUTHZ,        // 010
EN_AAA_AUTHZ_AUTHEN, // 011
EN_AAA_ACCONT,       // 100
EN_AAA_ACC_AUTHEN,   // 101
EN_AAA_ACC_AUTHZ,    // 110
EN_AAA_ACC_AUTHZ_AUTHEN // 111
};
typedef enum SWBEVENT{
EN_SWB_NULL = 0,
EN_SWB_WELCOME,
EN_SWB_IVRCONNECT,// Connected with IVR or PBX Server 
EN_SWB_PLAY,
EN_SWB_PLAY_PASSTHROUGH,// DTMF between play message call pass through feature.
EN_SWB_PLAY_COMPLET,
EN_SWB_GETDTMF,
EN_SWB_GETDTMF_COMPLET,
EN_SWB_DIAL,
EN_SWB_CALLCONNECTED, // Connected to Peer Leg
EN_SWB_DIALDEFAULT,
EN_SWB_CALLDISCONNECT,
EN_SWB_CALLDISCONNECTED
} xGateSwbEvent;
typedef enum CONFEVENT{
EN_CONF_NULL = 301,
EN_CONF_WELCOME,
EN_CONF_IVRCONNECT,
EN_CONF_PLAY,
EN_CONF_PLAY_PASSTHROUGH,// DTMF between play message call pass through feature.
EN_CONF_PLAY_PASSTHROUGH_GETDTMF,// DTMF between play message call pass through feature.
EN_CONF_PLAY_COMPLET,
EN_CONF_GETDTMF,
EN_CONF_GETDTMF_COMPLET,
EN_CONF_DIAL,
EN_CONF_CALLCONNECTED, // Connected to Peer Leg
EN_CONF_DIALDEFAULT,
EN_CONF_CALLDISCONNECT,
EN_CONF_CALLDISCONNECTED,
EN_CONF_HOLD,
EN_CONF_RESUME,
EN_CONF_MUTE,
EN_CONF_UNMUTE,
EN_CONF_DEAF,
EN_CONF_LOCK,
EN_CONF_UNLOCK,
EN_CONF_EXTEND,
EN_CONF_TRANSFER,
EN_CONF_RECORD,
EN_CONF_END,
EN_CONF_IVRPLAY,
EN_CONF_ADDPARTICIPANT,
EN_CONF_DELPARTICIPANT
} xGateConfEvent;

typedef enum CONFDTMFSTATE{
EN_CONF_WELCOMECONNECT = 350,
EN_CONF_DTMFSTART,
EN_CONF_NO_DTMF,
EN_CONF_VALIDATE,
EN_CONF_INVALID,
EN_CONF_VALID_INPUT,
EN_CONF_DTMFATTEMPTS_EXHAUSTED,
EN_CONF_ADDPARTICIPANTSENDMESSAGE10,
EN_CONF_ADDPARTICIPANTPLAYIVR,
EN_CONF_GETCONFROOMHANDLE,
EN_CONF_CONFROOMHANDLERECVD,
EN_CONF_GETHOSTCONTROLDIGIT
} xGateConfDtmfState;

typedef enum CONFDTMFMODULE{
EN_CONF_GETCONFID,
EN_CONF_CHECKCONFSTATUS,
EN_CONF_GETPIN,
EN_CONF_INITIATED,
EN_CONF_HOSTCONTROL
} xGateConfDtmfModule;

typedef enum XGATECALLFEATURE{
EN_NULL_CALL=0,
EN_INTERNAL_CALL,
EN_EXTERNAL_CALL,
EN_SWB_CALL,
EN_CONFERENCE_CALL,
EN_MULTICONF_CALL,
EN_RECORD_CALL
};

typedef enum XGATEREGEVENT {
EN_VALIDATE =1,
EN_REGISTER,
EN_DEREGISTER,
EN_SEND401,
EN_SEND407,
EN_REG_SUCESS,
EN_REG_FAILED,
EN_REG_TIMEOUT,
EN_GETUSRPWD,
EN_AUTHCALL,
EN_REG_SET_STATUS
};
typedef enum XGATECALLSTATE{
EN_CS_NULL=0,
EN_CS_INIT,
EN_CS_PROCEEDING,
EN_CS_ALERTING,
EN_CS_CONNECTED,
EN_CS_SUSPENDED,
EN_CS_RESUMED,
EN_CS_DISCONNECTED,
EN_CS_TERMINATED

};

static void *SipDispatcher;
static void *SipProcessor;
static void *DBManager;

void SetSipPostHandler(void *Handler,XGATEPOSTTYPE type){
    switch(type){
        case XGATEPOSTTYPE::EN_DISPATCH:
        SipDispatcher = Handler;
        break;
        case XGATEPOSTTYPE::EN_PROCESS:
        SipProcessor = Handler;
        break;
        case XGATEPOSTTYPE::EN_DBMANAGER:
        DBManager = Handler;
        default:
        break;
    }
}
void *GetSipPostHandler(XGATEPOSTTYPE type){
    void *handler = NULL;
    switch(type){
        case XGATEPOSTTYPE::EN_DISPATCH:
        handler = SipDispatcher;
        break;
        case XGATEPOSTTYPE::EN_PROCESS:
        handler = SipProcessor;
        break;
        case XGATEPOSTTYPE::EN_DBMANAGER:
        handler = DBManager;
        break;
        default:
        handler = NULL;
        break;
    }
    return handler;
}
static std::string systemconfig = "";
void SetSystemconfigPath(std::string filepath){
  systemconfig = filepath;
}
std::string GetSystemconfigPath(){
return systemconfig;
}
#endif
