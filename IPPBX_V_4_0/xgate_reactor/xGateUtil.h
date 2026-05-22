#ifndef _XGATE_UTIL_H
#define _XGATE_UTIL_H

//system includes
#include <string>
#include <map>
#include <vector>

//ace inludes
#include <ace/Guard_T.h>
#include <ace/Thread_Manager.h>
#include <ace/SOCK_Stream.h>
#define XGATE_HAS_JSON_SUPPORT
#define XGATE_HAS_DBM_SUPPORT
#define XGATE_HAS_BUSINESS_SUPPORT

#define GET_PIN_ONLY  // In conference-feature; it will ask only PIN and Not Conference-Id.

using namespace std;
#ifdef XGATE_HAS_JSON_SUPPORT
//rapidjson includes
#include "document.h"
#include "error/en.h"
#include "writer.h"
#include "reader.h"
#include "stringbuffer.h"
#include "prettywriter.h"
#include "ostreamwrapper.h"
#include "ostreamwrapper.h"
using namespace rapidjson;
#endif

//local includes
//#include "xgconfig.h"

#define SERVER_TYPE_SIZE 20
#define SERVER_ADDRESS_LENGTH 50
#define MGC_MG_SIGNAL_PORT 32779
#define XGATE_TCP_READ_SIZE  2048
#define SIP_SERVER_PORT 5060
#define ENCODE_BUF_SIZE 4*1024 

#define SERVER_TYPE_SIZE 20
#define SERVER_ADDRESS_LENGTH 50
#define MGC_MG_SIGNAL_PORT 32779
#define XGATE_TCP_READ_SIZE  2048
#define SIP_SERVER_PORT 5060
#define ENCODE_BUF_SIZE 4*1024

#define GET_EXTENSION_DATA_REQ          EN_INVITE_REQ
#define START_SWITCHBOARD_CALL_REQ      EN_DB_SWBCOMBINEDSP_REQ
#define START_CONFERENCE_CALL_REQ       EN_DB_CONFERENCECALL_REQ

#define TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_INITIAL 50
#define TOTAL_NOOFCOLUMNS_DB_CALLHANDLING 6
#define TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_SWB     16
#define TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_SELECTOR        8
#define TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_REGISTRAR       7
#define TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_CONF_INFO       24
#define TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_CONF_ID         7
#define TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_DIRECT_NUMBER       8
#define TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_CONF_REGISTERINFO       8
#define TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_GETOPERATORDETAILS       3
#define TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_GET_CALLRECORDINFO       9


#define MAX_DB_RECORDS  10

#define SWB_MAINMENUIVR_TIMEOUT_WITH_NO_DTMF_DIGITS     "swb_mainmenuivr_timeout_with_no_dtmf_digits"
#define CONF_WELCOMEIVR_TIMEOUT_WITH_NO_DTMF_DIGITS     "conf_welcomeivr_timeout_with_no_dtmf_digits"

#define SL_APPLICATION_NAME "ippbx_call_connect"

#define FEATURE_PIN_TO_CONNECT_IVR "/root/wav_files/sounds/vectone/enter_pin_to_connect.wav"

#define	COLUMN_1	1
#define	COLUMN_2	2
#define	COLUMN_3	3
#define	COLUMN_4	4
#define	COLUMN_5	5
#define	COLUMN_6	6
#define	COLUMN_7	7
#define	COLUMN_8	8
#define	COLUMN_9	9
#define	COLUMN_10	10
#define	COLUMN_11	11
#define	COLUMN_12	12
#define	COLUMN_13	13
#define	COLUMN_14	14
#define	COLUMN_15	15
#define	COLUMN_16	16
#define	COLUMN_17	17
#define	COLUMN_18	18
#define	COLUMN_19	19
#define	COLUMN_20	20
#define	COLUMN_21	21
#define	COLUMN_22	22
#define	COLUMN_23	23
#define	COLUMN_24	24
#define	COLUMN_25	25
#define	COLUMN_26	26
#define	COLUMN_27	27
#define	COLUMN_28	28
#define	COLUMN_29	29
#define	COLUMN_30	30
#define	COLUMN_31	31
#define	COLUMN_32	32
#define	COLUMN_33	33
#define	COLUMN_34	34
#define	COLUMN_35	35
#define	COLUMN_36	36
#define	COLUMN_37	37
#define	COLUMN_38	38
#define	COLUMN_39	39
#define	COLUMN_40	40
#define	COLUMN_41	41
#define	COLUMN_42	42
#define COLUMN_43       43
#define COLUMN_44       44
#define COLUMN_45       45
#define COLUMN_46       46
#define COLUMN_47       47
#define COLUMN_48       48
#define COLUMN_49       49
#define COLUMN_50       50

static const char json_mg_media_tmpl[] = " { \"msg_type\" : 0, \
                                          \"msg_len\" : 999 , \
                                          \"data\" : { \
                                          \"mgresource_id\": \"\", \
                                          \"call_type\": 1, \
                                          \"call_id\": \"\", \
                                          \"gateway_id\" : 0, \
                                          \"context_id\" : 0, \
                                          \"leg_id\" : 0, \
                                          \"sdpinfo\":[{ \
                                          \"media_type\": 0,\
                                          \"ip_addr\": \"\", \
                                          \"ip_port\": 0, \
                                          \"dial_ip\": \"\", \
                                          \"dial_port\": 0, \
                                          \"resp_ip\": \"\", \
                                          \"resp_port\": 0, \
                                          \"relayip_addr\" : \"\",\
                                          \"relay_port\" : 0,\
                                          \"reflexip_addr\" : \"\",\
                                          \"reflex_port\" : 0,\
                                          \"codec\": 0, \
                                          \"codec_name\": \"\",\
                                          \"fmtp\": \"\", \
                                          \"ice_ufrag\" : \"\",\
                                          \"ice_pwd\" : \"\",\
                                          \"fingerprint\" : \"\",\
                                          \"ptime\" : 20,\
                                          \"framerate\" : 15,\
                                          \"imageattr_x\" : 720,\
                                          \"imageattr_y\" :526,\
                                          \"play_file\" : \"\",\
                                          \"ssrc\": \"\", \
                                          \"cname\": \"\", \
                                          \"mslable\": \"\", \
                                          \"lable\": \"\"}],\
                                          \"call_record_info\": [{ \
                                          \"dummy_call_record_info\": \"sip_rec_values\"}],\
                                          \"joiner_type\": 0, \
                                          \"media_proto\" : \"\",\
                                          \"out_proto\": \"\", \
                                          \"dtmf_digits\" : \"\",\
                                          \"sig_type\" : 0, \
                                          \"call_dir\" : 0, \
                                          \"media_state\": \"\", \
                                          \"rtp_endpoint\": \"\",\
                                          \"mp3_duration\":0,\
                                          \"mp3_filesize\":0} } ";

#if 0 //TODO: Yoga, do we need this really?
static const char json_db_tmpl[] = " {    \"trans_id\" : 0, \
                                    \"msg_len\" : 0, \
                                    \"db_type\" : 0, \
                                    \"DB_Operation\" : 0, \
                                    \"DB_name\" : \"XGREGISTAR\", \
                                    \"table_name\" : \"\", \
                                    \"sp_name\" : \"\", \
                                    \"input_data\" : { \
                                    \"key1\": \"value1\",\
                                    \"key2\": \"value2\"} \
                                    \"Output_data\" : { \
                                    \"key1\": \"value1\",\
                                    \"key2\": \"value2\"} \
                                    \"affectedRows\": 0, \
                                    \"errmsg\": \"\", \
                                    } ";
#endif

#ifdef XGATE_HAS_DBM_SUPPORT
#define NODEJS_URL "http://10.22.7.29:5001/api"
//json template used for DBM server communication
static string json_dbm_server_tmpl = "{\
                                      \"trans_id\": \"\", \
                                      \"db_operation\": 0, \
                                      \"db_name\": \"\",  \
                                      \"table_name\": \"\", \
                                      \"sp_name\": \"\",  \
                                      \"input\": [],  \
                                      \"output\": [], \
                                      \"msg\": { \
                                      \"status_code\": 0,  \
                                      \"err_code\": 0, \
                                      \"affected_rows\": 0,  \
                                      \"message\": \"\"} \
                                      }";
static string json_dbm_client_tmpl = "{\
                                      \"trans_id\": \"\", \
                                      \"db_type\": 0,  \
                                      \"db_operation\": \"\", \
                                      \"db_name\": \"\",  \
                                      \"table_name\": \"\", \
                                      \"sp_name\": \"\"  \
                                      }";
#endif

#ifdef XGATE_HAS_BUSINESS_SUPPORT 
//TODO: json template for business operations
static string json_business_tmpl = "{\
                                    \"uid\": \"\", \
                                    \"opcode\": 0, \
                                    \"errcode\": 0, \
                                    \"input\": {},  \
                                    \"output\": {}, \
                                    \"msg\": \"\"  \
                                    }";
#endif

//forward class declaration
class xGateConnector;
class xGateTCPSocketHandler;
class xGateTCPReceiver;
class xGateMgProcessor;
class xGateSrvrProcessor;
class xGateMGDispatcher;
class xGateHmpProcessor;
class xGateHmpGstManager;
#ifdef XGATE_HAS_DBM_SUPPORT
class xGateDbmProcessor;
class xGateDbmDispatcher;
class xGateDbmReceiver;
#endif
#ifdef XGATE_HAS_BUSINESS_SUPPORT
class xGateBusinessProcessor;
class xGateDBManager;
class xGateBusinessScheduler;
#endif

// RBT vlaue for communication between MGC and MG
typedef enum
{
  EN_XGATE_PB_NULL = 0, // Inital state no actin to take in MG
  EN_XGATE_PB_PLAY,	// state at PLAY create RBT pipeline in Mg and play RBT
  EN_XGATE_PB_STOP,	// State at STOP if RBT pipeline is running stop the play
  EN_XGATE_PB_EOS,
  EN_XGATE_PB_REPEAT,
  EN_XGATE_PB_HOLD,
  EN_XGATE_PB_RESUME,
  EN_XGATE_PB_GETDIGIT,   // Request for start to collect the DTMF Digit in events
  EN_XGATE_PB_SENDDIGIT,  // Request for send the Collected DTMF Digits in events.
  EN_XGATE_PB_RECVDIGIT,    // Posting the Collected DTMF Digits in events.
  EN_XGATE_PB_MUTE,
  EN_XGATE_PB_UNMUTE,
  EN_XGATE_PB_DEAF,
  EN_XGATE_PB_LOCK,
  EN_XGATE_PB_UNLOCK,
  EN_XGATE_PB_EXTEND,
  EN_XGATE_PB_TRANSFER,
  EN_XGATE_PB_CONFALL,
  EN_XGATE_PB_CONF_END,
  EN_XGATE_PB_RECORD_START,
  EN_XGATE_PB_RECORD_STOP,
  EN_XGATE_PB_RECORD_HOLD,
  EN_XGATE_PB_RECORD_RESUME,
  EN_XGATE_PB_RECORD_SUCCESS,
  EN_XGATE_PB_RECORD_FAIL,
  EN_XGATE_PB_RECORD_INFO
}xGatePbState;
//ENUM definiton - Start
/// Enum for Return Code
typedef enum
{
  EN_XGATE_STATUS_NOT_SUPPORTED,
  EN_XGATE_STATUS_SUCCESS,
  EN_XGATE_STATUS_STOPPING,
  EN_XGATE_STATUS_ACE_ERROR,
  EN_XGATE_STATUS_ENQUEUE_FAIL,
  EN_XGATE_STATUS_DEQUEUE_FAIL,
  EN_XGATE_STATUS_NO_CONNECTION,
  EN_XGATE_STATUS_NOT_PRESENT,
  EN_XGATE_STATUS_ERROR
}xGateRetVal;

typedef enum 
{
  EN_XGATE_DB_QUERY_UNKNOWN,
  EN_XGATE_DB_QUERY_STORE_RECORD_DATA,
  EN_XGATE_DB_QUERY_STORE_CCAAS_RECORD_DATA,
  EN_XGATE_DB_QUERY_STORE_CRM_RECORD_DATA
}xGateDBEvent;
/// Enum for TCP Connections Type
typedef enum
{  
  EN_XGATE_UNKNOWN_CONNECTION, 
  EN_XGATE_NW_CONNECTION, 
  EN_XGATE_MG_CONNECTION,
}xGateTCPConnectionType;

typedef enum 
{
  EN_XGATE_TIMER_RECONNECT,
  EN_XGATE_TIMER_TIMEOUT,
  EN_XGATE_TIMER_RECONNECT_MG,
  EN_XGATE_TIMER_RECONNECT_RLS,
  EN_XGATE_TIMER_CLEANUP,
}xGateTimerType;

typedef enum
{
  EN_XGATE_TCP_RECV_UNKNOWN,
  EN_XGATE_TCP_RECV_NW,
  EN_XGATE_TCP_RECV_MG,
  EN_XGATE_TCP_RECV_MG_CONN
}xGateTCPReceiverType;

// Enum for Message Type
typedef enum
{
  EN_XGATE_MSG_UNKNOWN=0,
  EN_XGATE_MSG_MGC_MG_IN, //incomming msg from peer(it will be either MG/MGC)
  EN_XGATE_MSG_MGC_MG_OUT, //outgoing msg to peer(it will be either MG/MGC)
  EN_XGATE_MSG_SIP_IN, //incoming sip msg from peer
  EN_XGATE_MSG_SIP_OUT, //outgoing sip msg from peer
  EN_XGATE_MSG_SIPREG_IN, // incoming sip registration request
  EN_XGATE_MSG_SIPREG_OUT,// outgoing sip registration reply
  EN_XGATE_MSG_P2PMEDIA_IN,
  EN_XGATE_MSG_P2PMEDIA_OUT,
  EN_XGATE_MSG_HOLDRESUME_IN,
  EN_XGATE_MSG_HOLDRESUME_OUT,
  EN_XGATE_MSG_DBCONNECT_IN,
  EN_XGATE_MSG_DB_IN,
#ifdef XGATE_HAS_DBM_SUPPORT
  EN_XGATE_MSG_TO_DBM,
  EN_XGATE_MSG_FROM_DBM,
#endif
#ifdef XGATE_HAS_BUSINESS_SUPPORT
  EN_XGATE_MSG_TO_BUSINESS,
  EN_XGATE_MSG_FROM_BUSINESS,
#endif
  EN_XGATE_MSG_TO_HTTP,
  EN_XGATE_MSG_REDIS_CONN_FAILED_NOTIFY,
}xGateMsgType;

typedef enum
{
  EN_XGATE_MG_TYPE_UNKNOWN=0,
  EN_XGATE_MG_REGISTER,
  EN_XGATE_MG_ALLOCATE_ORGINATE,
  EN_XGATE_MG_ALLOCATE_TERMINATE,
  EN_XGATE_MG_RELEASE,
  EN_XGATE_MG_18X_RESPONSE,
  EN_XGATE_MG_PLAY_BACK,
  EN_XGATE_MG_MEDIA_CREATE,
  EN_XGATE_MG_HOLD_RESUME,
  EN_XGATE_MG_DTMF_DIGITS,
  EN_XGATE_MG_ADD_PARTICIPANT,
  EN_XGATE_MG_MEDIA_DELETE,
  EN_XGATE_MG_REMOVE_PARTICIPANT,
  EN_XGATE_MG_CONFERENCE_FOR_CLIENT = 15,
  EN_XGATE_MG_ADDCLIENT_TO_CONFERENCE = 16,
  EN_XGATE_MG_CALL_RECORD,
  EN_XGATE_MG_ALLOCATE_RECORD,
  EN_XGATE_MG_RELEASE_RECORD,
  EN_XGATE_MG_MP3_DETAIL=25,
}xGateMgMsgType;

/*typedef enum 
{
EN_XGATE_RECORDING_STATUS_UNKNOWN,
EN_XGATE_RECORDING_STATUS_SUCCESS,
EN_XGATE_RECORDING_STATUS_FAIL,
EN_XGATE_CONVERSION_FAIL,
}xGateRecordingStatus;
*/
#ifdef XGATE_HAS_DBM_SUPPORT
typedef enum
{
  EN_XGATE_DBM_MSG_UNKNOWN,
  EN_XGATE_DBM_MSG_DBREAD,
  EN_XGATE_DBM_MSG_DBWRITE,
  EN_XGATE_DBM_MSG_DBUPDATE,
  EN_XGATE_DBM_MSG_DBDELETE,
  EN_XGATE_DBM_MSG_DBEXECUTE,
}xGateDbmMsgType;

typedef enum
{
  EN_XGATE_DB_LAYER_UNKNOW=0,
  EN_XGATE_DB_LAYER_MONGODB=1,
  EN_XGATE_DB_LAYER_MSDB=2,
  EN_XGATE_DB_LAYER_ORACLEDB=3,
}xGateDbLayerType;
#endif

#ifdef XGATE_HAS_BUSINESS_SUPPORT
typedef enum
{
  EN_BUSINESS_UNKNOWN_MSG=0,
  EN_BUSINESS_SIGLAYER_MSG=1, //message between business and signalling layer
  EN_BUSINESS_DBLAYER_MSG=2, //message between business and db layer
  EN_BUSINESS_SCHEDULER_MSG=3, //message between business and db layer
}xGateBusinessMsgType;

typedef enum
{
  EN_BUSINESS_EVENT_UNKNOWN=0,
  EN_BUSINESS_EVENT_INIT=1,
  EN_BUSINESS_EVENT_FORKING=2,
  EN_BUSINESS_EVENT_TIMER=3,
  EN_BUSINESS_EVENT_CDRUPDATE=4,
}xGateBusinessSigEvent;

typedef enum
{
  EN_DB_EVENT_PREQUERY=1,
  EN_DB_EVENT_GET_AOR=2,
}xGateBusinessDbEvent;

typedef enum
{
  EN_SCHEDULER_EVENT_UNKNOWN=0,
  EN_SCHEDULER_EVENT_TIMEOUT=1,
  EN_SCHEDULER_EVENT_CANCEL=2,
}xGateBusinessSchedulerEvent;
#endif 

typedef enum
{
  EN_XGATE_CALL_TYPE_UNKNOWN,
  EN_XGATE_CALL_TYPE_AUDIO,
  EN_XGATE_CALL_TYPE_AUDIO_CONF,
  EN_XGATE_CALL_TYPE_VIDEO,
  EN_XGATE_CALL_TYPE_VIDEO_CONF
} xGateCallType;

typedef enum
{
  EN_XGATE_JOINER_TYPE_UNKNOWN,
  EN_XGATE_JOINER_TYPE_HOST,
  EN_XGATE_JOINER_TYPE_PARTICIPANT
} xGateJoinerType;

typedef enum
{
  EN_XGATE_MSG_PRIORITY_LOW,
  EN_XGATE_MSG_PRIORITY_HIGH,
  EN_XGATE_MSG_PRIORITY_UNKNOWN
}xGateMsgPriority;

typedef enum
{
  EN_XGATE_TRANSPORT_UNKNOWN,
  EN_XGATE_TRANSPORT_UDP,
  EN_XGATE_TRANSPORT_TCP,
  EN_XGATE_TRANSPORT_TLS,
}xGateTransportType;

typedef enum XGATEDBEVENT
{
  EN_DB_PREQUERY_REQ=200,
  EN_DB_PREQUERY_RES,
  EN_DB_NOTIFYE_REQ,
  EN_DB_NOTIFYE_RES,
  EN_DB_GETREG_REQ,
  EN_DB_REGDEATILS_REQ,
  EN_DB_REGDEATILS_RES,
  EN_DB_QUERYSWB_REQ,
  EN_DB_SWBDOMAINNAME_REQ,
  EN_DB_SWBDOMAINNAME_RES,
  EN_DB_DOMAINID_REQ,
  EN_DB_DOMAINID_RES,
  EN_DB_COMPANYHOURUSER_REQ,
  EN_DB_COMPANYHOURUSER_RES,
  EN_DB_INCOMINGCALLTYPE_REQ,
  EN_DB_INCOMINGCALLTYPE_RES,
  EN_DB_FORWARDSTATUSENABLED_REQ,
  EN_DB_FORWARDSTATUSENABLED_RES,
  EN_DB_CALLFORWARDNUMBER_REQ,
  EN_DB_CALLFORWARDNUMBER_RES,
  EN_DB_TOTALNOOFREGISTEREDUSERS_REQ,
  EN_DB_TOTALNOOFREGISTEREDUSERS_RES,
  EN_DB_COMBINEDSPSELECTOR_REQ,
  EN_DB_COMBINEDSPSELECTOR_RES,
  EN_DB_COMBINEDSPINITIAL_REQ,
  EN_DB_COMBINEDSPINITIAL_RES,
  EN_DB_COMBINEDSPREGISTAR_REQ_1,
  EN_DB_COMBINEDSPREGISTAR_RES_1,
  EN_DB_COMBINEDSPREGISTAR_REQ_2,
  EN_DB_COMBINEDSPREGISTAR_RES_2,
  // SwitchBoard related only...
  EN_DB_SWBCOMBINEDSP_REQ,
  EN_DB_SWBCOMBINEDSP_RES,
  EN_DB_SWBMAINMENUIVRTIMEOUT_RES,
  EN_DB_SWB_EVENT_REQ,
  // Conference related only...
  EN_DB_CONFERENCECALL_REQ,
  EN_DB_CONFERENCECALL_RES,
  EN_DB_CONFPIN_REQ,
  EN_DB_CONFERENCEIVR_TIMEOUT_RES,
  EN_DB_CONFPIN_RES,
  EN_DB_CONF_EVENT_REQ,
  EN_DB_SWB_EVENT_RES,
  EN_DB_RECORDING_EVENT_REQ
};

//ENUM definiton - End

//STRUCT Definition - Start
struct MgRegistrationDetail
{
  char gw_name[25];
  char gw_profile[25];
  int channels_count;
  int rsc_count;
  char rls_record[500];

  MgRegistrationDetail() : gw_name({0}), gw_profile({0}), channels_count(0), rsc_count(0), rls_record({0})
  {
  };
};
struct MgPlayBackDetail
{
  string call_id;
  unsigned int rbt_status;

  MgPlayBackDetail()
  {
    call_id = "";
    rbt_status = 0;
  }

  ~MgPlayBackDetail()
  {
  }

  MgPlayBackDetail& operator=(MgPlayBackDetail &src)
  {
    call_id   = src.call_id;
    rbt_status = src.rbt_status;
    return *this;
  }
};
typedef struct avDetails{
  string ip_addr;
  unsigned int port;
  string relayip_addr;
  unsigned int relay_port;
  string reflexip_addr;
  unsigned int reflex_port;
  int codec;
  string codecname;
  string fmtp;
  string ice_ufrag;
  string ice_pwd;
  string fingerprint;
  string ssrc;
  string cname;
  string mslabel;
  string label;
  unsigned int ptime;
  string play_file;
  string dial_ip;
  unsigned int dial_port;
  string resp_ip;
  unsigned int resp_port;
  unsigned int mediaType;
  unsigned int framerate;
  unsigned int imageattr_x ;
  unsigned int imageattr_y;
  avDetails(){
    ip_addr = "";
    relayip_addr = "";
    reflexip_addr = "";
    codecname = "";
    fmtp = "";
    ice_ufrag = "";
    ice_pwd = "";
    fingerprint = "";
    ssrc = "";
    cname = "";
    mslabel = "";
    label= ""; 
    play_file = "";
    dial_ip = "";
    resp_ip = "";
    port = 0;
    relay_port = 0;
    reflex_port = 0;
    codec = -1;
    ptime  = 30;
    dial_port = 0;
    resp_port = 0 ;
    mediaType = 0;
    framerate = 15;
    imageattr_x = 720;
    imageattr_y = 576;
  }
  avDetails& operator=(avDetails &src){
    ip_addr = src.ip_addr;
    relayip_addr = src.relayip_addr;
    reflexip_addr = src.reflexip_addr;
    ice_ufrag = src.ice_ufrag;
    ice_pwd = src.ice_pwd;
    codecname =  src.codecname;
    fmtp = src.fmtp;
    fingerprint = src.fingerprint;
    ssrc = src.ssrc;
    cname = src.cname;
    mslabel = src.mslabel;
    label= src.label; 
    play_file = src.play_file;
    dial_ip = src.dial_ip;
    resp_ip = src.resp_ip;
    port = src.port;
    relay_port = src.relay_port;
    reflex_port = src.reflex_port;
    codec = src.codec;
    ptime  = src.ptime;
    dial_port = src.dial_port;
    resp_port = src.resp_port ;
    mediaType =  src.mediaType;
    framerate = src.framerate;
    imageattr_x = src.imageattr_x;
    imageattr_y = src.imageattr_y;
    return *this;
  }

};

/* Sip Call Recording Info structure for JSON i
 * usedfor pMSG media info
 */
struct RecDetails {
  // all string member for
  // session
  // participants
  // session
  // partisessassociation
  // partistreamassociation

};


struct MgMediaDetail
{
  string call_id; //this value of calltablekey generated in sofia.cpp
  string mediaResource_id;
  int gateway_id; //media gateway id of media server
  unsigned int leg_id;
  int context_id;
  unsigned int pb_state;
  string media_proto;
  string out_proto;
  string mg_ip;
  string record_file;
  long int file_size;
  bool isRecordingStatus;
  unsigned int mp3_duration;
  unsigned int mp3_filesize;
  string dtmf_digits;
  unsigned int call_dir; //this value from CallDirection enum from call.h
  unsigned int sig_type; //this value from SignallingType enum from SignallingDriver.h
  /* SwitchListen is merged into CalledParty Rtp Activate message, piggybacking
   * so no switchlisten message from MGC to MG explicitly */
  unsigned int call_type;
  string rtp_ep;
  unsigned int  rbtState;
  unsigned int joiner_type;
  avDetails sdpinfo[2];
  RecDetails callRecInfo;
  MgMediaDetail()
  {
    call_id = "";
    call_type = 1;
    mediaResource_id = "";
    gateway_id = 0;
    leg_id = 0;
    context_id = 0;
    pb_state = 0;
    media_proto= "";
    out_proto = "";
    mg_ip = "";
    record_file = "";
    file_size = 0;
    isRecordingStatus=false;
    mp3_duration=0;
    mp3_filesize=0;
    dtmf_digits= "";
    call_dir = 0;
    sig_type = 3;
    rtp_ep = "";
    joiner_type = 0;
  };

  ~MgMediaDetail()
  {
  }
  //Assignment operator
  MgMediaDetail& operator=(MgMediaDetail &src)
  {
    call_id   = src.call_id;
    call_type   = src.call_type;
    mediaResource_id = src.mediaResource_id;
    leg_id = src.leg_id;
    context_id = src.context_id;
    pb_state = src.pb_state;
    media_proto= src.media_proto;
    out_proto = src.out_proto;
    mg_ip =  src.mg_ip;
    record_file = src.record_file;
    file_size = src.file_size;
    isRecordingStatus= src.isRecordingStatus;
    mp3_duration=src.mp3_duration;
    mp3_filesize=src.mp3_filesize;
    dtmf_digits = src.dtmf_digits;
    call_dir  = src.call_dir;
    call_type  = src.call_type;
    sig_type  = src.sig_type;
    rtp_ep = src.rtp_ep;
    joiner_type   = src.joiner_type;
    sdpinfo[0] =  src.sdpinfo[0];
    sdpinfo[1] = src.sdpinfo[1];
    callRecInfo= src.callRecInfo;
    return *this;
  }
};


struct xGateListenAddress
{
  std::string     ipAddress;
  unsigned short  port;
  //CTOR
  xGateListenAddress():ipAddress(""), port(0)
  {
  }
  //DTOR
  ~xGateListenAddress()
  {
  }
};

struct xGateTCPConnectionStr
{
  xGateTCPConnectionType conType;
  string peerIPAddress;  /// In the form of IPADDRESS
  string fqdn;           /// Destination FQDN

  xGateTCPConnectionStr()
  {
    conType = EN_XGATE_UNKNOWN_CONNECTION;
  }

  ~xGateTCPConnectionStr()
  {
  }

  //Assignment operator
  xGateTCPConnectionStr& operator=(xGateTCPConnectionStr &src)
  {
    conType       = src.conType;
    peerIPAddress = src.peerIPAddress;
    fqdn          = src.fqdn;
    return *this;
  }
  //Equal to operator

  bool operator==(xGateTCPConnectionStr &src)
  {
    if(conType   == src.conType && peerIPAddress == src.peerIPAddress &&  fqdn == src.fqdn)
      return true;
    else
      return false;
  } 
};

/// Connection Tuple
struct xGateTCPConTuple 
{
  std::string     ipAddress;
  unsigned short  port;
  int             fd;
  unsigned short  srcPort;
  unsigned short timeoutCount;
  xGateTCPReceiverType eReceiverType;
  long int timerId;

  //CTOR
  xGateTCPConTuple()
  {
    port     = 0;
    fd       = -1;
    srcPort  = 0;
    timeoutCount = 0;
    eReceiverType = EN_XGATE_TCP_RECV_UNKNOWN;
    timerId = 0;
  }
  //DTOR
  ~xGateTCPConTuple()
  {
  }

  //TODO: getting compile error here
  //Assignment operator
  /*xGateTCPConTuple& operator=(xGateTCPConTuple &src)
    {
    ipAddress = src.ipAddress;
    port      = src.port; 
    fd        = src.fd;
    srcPort   = src.srcPort;
    timeoutCount = src.timeoutCount;
    eReceiverType = src.eReceiverType;
    timerId = src.timerId;
    return *this;
    }*/

  bool operator==(xGateTCPConTuple &src)
  {
    if(ipAddress == src.ipAddress && port == src.port \
        && fd == src.fd && srcPort == src.srcPort)
    {
      return true;
    }
    else 
    {
      return false;
    }
  } 

};

struct xGateTimerData
{
  int fd;
  xGateTimerType eTimerType;
  char ipAddress[16];
  xGateTCPSocketHandler *pHandler;
  ACE_SOCK_Stream newPeerStream;
};

struct xGateNetConnection
{
  std::string     recvIp_;
  unsigned short  recvPort_;
  xGateTransportType  protocol_;
  int fd_;
  xGateTCPConnectionType conType_;
  unsigned short  srcPort_;

  //CTOR
  xGateNetConnection()
  {
    recvPort_= 5080;
    protocol_= EN_XGATE_TRANSPORT_UDP;
    fd_      = 0;
    conType_ = EN_XGATE_UNKNOWN_CONNECTION;
    srcPort_ = 0;
  }
  //DTOR
  ~xGateNetConnection()
  {
  }
  //Assignment operator
  xGateNetConnection& operator=(xGateNetConnection &src)
  {
    recvIp_   = src.recvIp_;
    recvPort_ = src.recvPort_;
    protocol_ = src.protocol_;
    fd_       = src.fd_;
    conType_  = src.conType_;
    srcPort_  = src.srcPort_;
    return *this;
  }
};
union Data
{
  int integer_data;
  //char* string_data;
  std::string* string_data;
};
//
struct DBQueryResultData {
  bool data_type; // false == int or no string; true == std::string
  Data data;
};
Value getDBColumnValue(DBQueryResultData dbQueryData){
  Value retVal;
  Document doc;
  if (dbQueryData.data_type)
  {	// data_type == true => String-data
    retVal.SetString((*(dbQueryData.data.string_data)).c_str(), (*(dbQueryData.data.string_data)).length() , doc.GetAllocator());
  }
  else
  {
    retVal = dbQueryData.data.integer_data;
  }
  return retVal;
}

#ifdef XGATE_HAS_DBM_SUPPORT
class DbmReqDetail {
  public:
    DbmReqDetail();
    ~DbmReqDetail();

    //member variables
    string m_transId;
    unsigned int m_msgLen;
    int m_dbType;
    int m_opCode;
    string m_dbName;
    string m_tableName;
    string m_spName; //stored procedure or db function name
    string m_input;
    string m_output;
    unsigned int m_statusCode;
    int m_errCode;
    unsigned int m_affectedRows;
    string m_message;
    string m_url;
};
#endif
//STRUCT Definition - End

const char *jbegin = "{\"";
const char *jend = "\"}";
const char *jcomma = "\",\"";
const char *jcolon = "\":\""; 

class xGateUtil {
  public:
    static xGateRetVal findLocalIpAddr(char *ipAddr, unsigned short AddrType = AF_INET);
    static char * getLocalIpAddr(unsigned short AddrType = AF_INET);
    static void setLocalIpAddr(char *localIpAddr);

    static void addToMgConnectionMap(xGateTCPConTuple);
    static xGateRetVal getFromMgConnectionMap(int, int&);
    static xGateRetVal getFromMgConnectionMap(string, unsigned short, int&);
    static int  getFromMgConnectionMap(string); 
    static void removeFromMgConnectionMap(xGateTCPConTuple);
    static bool isPresentInMgConnectionMap(string,unsigned short);
    static void getTimerIdFromConnMap (int fd, xGateTCPConnectionType eConnType, long int &timerId);
    static std::string getMgIPOnIndex(int mgid);	
    static int selectMgId();
    static std::string getCurrentUTCTime();
    static time_t getCurrentTimeStamp();

    // Method to set Connector
    static char *LocalIPadress;
    static void SetLocalIPAddress(void);
    static unsigned int m_usedGateWayId;
    static  const char* GetLocalIPAddress()
    {
      //cout<<"LOCAL IPADRESS IS"<<LocalIPadress<<endl;
      return LocalIPadress;
    }

    static  xGateConnector* getMgConnector(void)
    {
      return m_mgConnector;
    }

    /// Method to set Connector
    static void setMgConnector(xGateConnector* pConnector)
    {
      m_mgConnector = pConnector;
    }

    /// Method to access the Mg Receiver
    static xGateTCPReceiver* getMgReceiver(void)
    {
      return  m_mgReceiver;
    }

    /// Method to set the Mg Receiver
    static void setMgReceiver(xGateTCPReceiver* receiver)
    {
      m_mgReceiver = receiver;
    }

    /// Method to access the Srvr Processor 
    static xGateSrvrProcessor* getSrvrProcessor(void)
    {
      return  m_srvrProcessor;
    }

    /// Method to set the Srvr Processor
    static void setSrvrProcessor(xGateSrvrProcessor* srvrProcessor)
    {
      m_srvrProcessor = srvrProcessor;
    }
    /// Method to access the Srvr Processor
    static xGateMgProcessor* getMgProcessor(void)
    {
      return  m_mgProcessor;
    }
    static void setMgProcessor(xGateMgProcessor* mgProcessor)
    {
      m_mgProcessor = mgProcessor;
    }
    static xGateHmpProcessor* getHmpProcessor(void)
    {
      return  m_hmpProcessor;
    }

    /// Method to set the Hmp Processor
    static void setHmpProcessor(xGateHmpProcessor* hmpProcessor)
    {
      m_hmpProcessor = hmpProcessor;
    }

    /// Method to access the Hmp Processor
    static xGateHmpGstManager* getHmpGstManager(void)
    {
      return  m_hmpGstManager;
    }

    /// Method to set the Hmp Processor
    static void setHmpGstManager(xGateHmpGstManager* hmpGstManager)
    {
      m_hmpGstManager = hmpGstManager;
    }

    static xGateMGDispatcher* getMgDispatcher()
    {
      return m_mgDispatcher;
    }
    static void setMgDispatcher(xGateMGDispatcher* mgdispatcher) {
      m_mgDispatcher= mgdispatcher;;

    }

#ifdef XGATE_HAS_DBM_SUPPORT
    static xGateDbmProcessor* getDbmProcessor(void)
    {
      return  m_dbmProcessor;
    }
    static void setDbmProcessor(xGateDbmProcessor* dbmProcessor)
    {
      m_dbmProcessor = dbmProcessor;
    }

    static  xGateDbmDispatcher* getDbmDispatcher(void)
    {
      return m_dbmDispatcher;
    }
    static void setDbmDispatcher(xGateDbmDispatcher* pDispatcher)
    {
      m_dbmDispatcher = pDispatcher;
    }

    static  xGateDbmReceiver* getDbmReceiver(void)
    {
      return m_dbmReceiver;
    }
    static void setDbmReceiver(xGateDbmReceiver* pReceiver)
    {
      m_dbmReceiver = pReceiver;
    }
#endif
#ifdef XGATE_HAS_BUSINESS_SUPPORT 
    static xGateBusinessProcessor* getBusinessProcessor(void)
    {
      return  m_businessProcessor;
    }
    static void setBusinessProcessor(xGateBusinessProcessor* businessProcessor)
    {
      m_businessProcessor = businessProcessor;
    }

    static xGateBusinessScheduler* getBusinessScheduler(void)
    {
      return  m_businessScheduler;
    }
    static void setBusinessScheduler(xGateBusinessScheduler* businessScheduler)
    {
      m_businessScheduler = businessScheduler;
    }

    static xGateDBManager* getDBManager(void)
    {
      return m_dbManager;
    }
    static void setDBManager(xGateDBManager* dbManager)
    {
      m_dbManager = dbManager;
    }
#endif

  private:
    /**
     * Private constructor for static only methods
     **/
    xGateUtil();

    /**
     * Private copy constructor - disallow copies and automatic methods.
     **/
    xGateUtil(const xGateUtil& rhs);

    /**
     * Private assignment operator - disallow assignment and automatic methods.
     **/
    xGateUtil& operator= (const xGateUtil& rhs);

    static char m_localIp4Addr[INET_ADDRSTRLEN];
    static char m_localIp6Addr[INET6_ADDRSTRLEN];

    static vector<xGateTCPConTuple> m_mgConnectionMap;

    /// Locks to Serialize the above Maps
    static ACE_Thread_Mutex m_mgConnectionMapLock;

    /// Store the Connector
    static xGateConnector* m_mgConnector;

    /// Store the TCP Receiver
    static xGateTCPReceiver* m_mgReceiver;

    // Store the Server Processor
    static xGateSrvrProcessor* m_srvrProcessor;
    static xGateMgProcessor* m_mgProcessor;

    static xGateMGDispatcher* m_mgDispatcher;

    /// Store the Hmp Processor
    static xGateHmpProcessor* m_hmpProcessor;

    /// Store the Hmp Gstreamer Manager
    static xGateHmpGstManager* m_hmpGstManager;

#ifdef XGATE_HAS_DBM_SUPPORT
    static xGateDbmProcessor *m_dbmProcessor;
    static xGateDbmDispatcher *m_dbmDispatcher;
    static xGateDbmReceiver *m_dbmReceiver;
#endif
#ifdef XGATE_HAS_BUSINESS_SUPPORT 
    static xGateBusinessProcessor *m_businessProcessor;
    static xGateBusinessScheduler *m_businessScheduler;
    static xGateDBManager* m_dbManager;
#endif

};

#endif
