#ifndef _XGATE_UTIL_H
#define _XGATE_UTIL_H

//system includes
#include <string>
#include <map>
#include <vector>
#include <list>
#include <gst/gst.h>
//ace inludes
#include <ace/Guard_T.h>
#include <ace/Thread_Manager.h>
#include <ace/SOCK_Stream.h>

#include <ace/Null_Mutex.h>
#include <ace/Recursive_Thread_Mutex.h>


#include "xglog.h"
//local includes

#define XGATE_HAS_JSON_SUPPORT

#define CHANNEL_NAME_SIZE 300 

#ifdef XGATE_HAS_JSON_SUPPORT
//rapidjson includes
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/ostreamwrapper.h"
#include <iostream>
using namespace rapidjson;
#endif

#include "xgconfig.h"

using namespace std;

#define SERVER_TYPE_SIZE 20
#define SERVER_ADDRESS_LENGTH 50
#define MGC_MG_SIGNAL_PORT 32778
#define XGATE_TCP_READ_SIZE  2048
#define SIP_SERVER_PORT 5060
#define ENCODE_BUF_SIZE 4*1024
#define START_BANDWIDTH 8000
#ifdef XGATE_HAS_JSON_SUPPORT
static const int jsonParseFlags = kParseDefaultFlags | kParseInsituFlag;
//mgc-mg json template messages
static const char json_mg_register_tmpl[] = " { \"msg_type\" : 0, \
					     \"msg_len\" : 0 , \
					     \"data\" : { \
					     \"gw_name\": \"\", \ 
					     \"gw_profile\": \"\", \
						\"channels_count\" : 0, \
						\"rls_count\": 0, \
						\"rls_record\": \"\" } } ";



static const char json_mg_media_tmpl[] = " { \"msg_type\" : 0, \
					  \"msg_len\" : 999 , \
					  \"data\" : { \
					  \"mgresource_id\": \"\", \
					  \"call_type\": 0, \
					  \"call_id\": \"\", \
					  \"old_mgresource_id\":\"\",\
					  \"old_call_id\":\"\",\
                                          \"group_call_id\": \"\", \
                                          \"switch_call_id\": \"\", \
					  \"gateway_id\" : 0, \
					  \"context_id\" : 0, \
					  \"leg_id\" : 0, \
					  \"media_proto\" : \"\",\
					  \"out_proto\": \"\", \
					  \"record_file\": \"\", \
					  \"record_type\": 0, \
					  \"file_size\": 0, \
					  \"mg_ipv4_intf\": \"\", \
					  \"mg_ipv6_intf\": \"\", \
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
					  \"sampling_rate\": 0, \
					  \"dtmf_fmtp\": \"\", \
					  \"dtmf_sampling_rate\": 0, \
					  \"cn_pt\": 0, \
					  \"cn_sampling_rate\": 0, \
					  \"rtcp_fp_payload\": 0, \
					  \"rtcp_fp_mechanism\" : \"\",\
					  \"ice_ufrag\" : \"\",\
					  \"ice_pwd\" : \"\",\
					  \"fingerprint\" : \"\",\
					  \"ptime\" : 20,\
					  \"max_ptime\" : 0,\
					  \"play_file\" : \"\",\
					  \"framerate\": 15,\
					  \"imageattr_x\": 0,\
					  \"imageattr_y\": 0,\
					  \"ssrc_legA\": \"\", \
					  \"ssrc_legB\": \"\", \
					  \"cname\": \"\", \
					  \"mslable\": \"\", \
					  \"lable\": \"\",\
					  \"inband_dtmf\":0}], \
					  \"tts_disconnect\":{ \
					  \"disconn_code\": \"\", \
					  \"disconn_reason\": \"\"}, \
					  \"dtmf_digits\" : \"\",\
					  \"joiner_type\" : 0,\
					  \"reset_flag\" : 0,\
					  \"sig_type\" : 0, \
					  \"call_dir\" : 0, \
					  \"media_state\": \"\", \
					  \"rtp_endpoint\": \"\", \
					  \"mrcp_port_allocate\" : 0, \
					  \"mrcp_port_activate\" : 0, \
					  \"mrcp_ip_info\":\"\", \
					  \"mrcp_port\" : 0,\
					  \"mrcp_state\" : 0,\
					  \"ccaas_voice_recog_port\" : 0,\
					  \"ccaas_block_id\" :\"\",\
					  \"ccaas_block_type\":\"\",\
					  \"ccaas_channel_id\":\"\",\
					  \"transfer_to\":0,\
					  \"ccaas_chatbot_init\":0,\
					  \"ccaas_bot_first\":0,\
					  \"ccaas_bot_url\":0,\
					  \"ccaas_bot_id\":0,\
                  \"is_smart_assist\":0,\
					  \"ccaas_agent_id\":\"\" }} ";
#endif
class xGateTCPSocketHandler;
class xGateTCPReceiver;
class xGateMgProcessor;
class xGateMGDispatcher;
class xGateMGCConnector;
class xGateHmpProcessor;
class xGateHmpGstManager;

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

//Bridge module sending the MrcpCommand to the MG

typedef enum
{
   EN_MSG_UNKNOWN = 0,
   EN_MSG_PORT_ALLOCATE_REQ = 1,
   EN_MRCP_MSG_RECOGNITION_START_REQ = 3,
   EN_MSG_RECOGNITION_COMPLETE = 4,
   EN_STT_MSG_RECOGNITION_START_REQ = 5,
   EN_SMATIC_MSG_RECOGNITION_COMPLETE = 6,
   EN_MSG_POST_MSG_TO_DB = 7,
   EN_MSG_RELEASE = 8,
   EN_MRCP_MSG_SYNTHESIZE_REQ = 9,
   EN_MRCP_MSG_SYNTHESIZE_STOP =10,
   EN_MSG_TERMINATE = 11,
   EN_MSG_DTMF_DIGITS = 12,
   EN_TTS_MSG_START_REQ = 13,
   EN_MSG_TRANSFER_REQ = 14,
   EN_MRCP_MSG_UNKNOWN = 15,
   EN_MRCP_MSG_PORT_ALLOCATE_REQ = 31,
   EN_MRCP_MSG_RECOGNITION_SENT = 32,
   EN_MRCP_MSG_RECOGNITION_INPROGRESS = 33,
   EN_MRCP_MSG_RECOGNITION_COMPLETE = 34,
   EN_MRCP_MSG_POST_MSG_TO_DB = 35,
   EN_MRCP_MSG_RELEASE = 36,
   EN_MRCP_MSG_EOS = 37,
   EN_MRCP_MSG_RECOGNITION_PAUSE = 38,
   EN_MRCP_MSG_RECOGNITION_RESUME = 39,
   EN_MRCP_MSG_RECOGNITION_STOP = 40,
   EN_MRCP_MSG_REMOVE_PARTICIPANT_REQ = 41,
   EN_MRCP_MSG_REMOVE_PARTICIPANT_RES = 42,
   EN_MRCP_MSG_ADD_PARTICIPANT_REQ = 43,
   EN_MRCP_MSG_POST_MSG_TO_HTTP = 44, // Added to post converted data from module bridge to HTTP module
   EN_MRCP_MSG_TERMINATE = 45,
   EN_MRCP_MSG_DTMF_DIGITS = 46,
   EN_MRCP_TTS_MSG_START_REQ = 47,
   EN_MRCP_MSG_TRANSFER_REQ = 48,
   EN_SMATIC_MSG_PORT_ALLOCATE_REQ = 49,
   EN_SMATIC_MSG_PORT_ALLOCATE_RES = 50,
   EN_GRPC_MSG_START_CAPTURE_REQ = 51,
   EN_GRPC_MSG_RECOGNITION_START_REQ = 52,
   EN_GRPC_MSG_STOP_CAPTURE_REQ = 53,
   EN_TTS_SPEAK_COMPLETE = 54,
   EN_MRCP_MSG_ALLOCATE_STT_TTS = 55,
   EN_STT_TTS_MSG_ALLOCATE_RES = 56,
   EN_XGATE_GRPC_PIPELINE_ERROR=57,
   EN_STT_REMOVE_PARTICIPANT = 58,
   EN_TTS_CHANNEL_START_REQ = 59,
   EN_TTS_CHANNEL_STOP_REQ = 60,
   EN_POST_BASE64_MSG = 61,
   EN_GET_VARAIBLE_STT = 62,
   EN_STT_TTS_TRANSFER_ADD_PARTICIPANT = 63,
   EN_STT_TTS_RECONNECTION_MSG = 64 ,
   EN_STT_START_REQ = 65,
   EN_STT_PAUSE_REQ = 66,
   EN_STT_RESUME_REQ = 67,
   EN_STT_STOP_REQ = 68,
   EN_RESTART_STT_RECOGNIZATION = 69,
   EN_VOICE_BOT_DEINIT = 70,
   EN_MRCP_MSG_DELETE_SESSIONINFO = 71,
   EN_CREATE_STT_SOCKET_CONNECTION = 72,
   EN_STT_START = 73,
   EN_STT_RECOGNITION_PAUSE = 74,
   EN_STT_RECOGNITION_RESUME = 75,
}xGateMrcpCommand;

typedef enum
{
   EN_XGATE_ASR_TYPE_UNKNOWN=0,
   EN_XGATE_ASR_INHOUSE=1,
   EN_XGATE_ASR_SPEECH_MATICS=2,
   EN_XGATE_ASR_VOICE_GAIN=3,
   EN_XGATE_ASR_GOOGLE=4,
}xGateASRType;

typedef enum
{
   EN_XGATE_TTS_TYPE_UNKNOWN=0,
   EN_XGATE_TTS_READ_SPEAKER=1,
   EN_XGATE_TTS_INHOUSE=2,
}xGateTTSType;

//PBX sending the state to the MG

enum class STT_TTS_STATE
{
   EN_STT_TTS_STATE_EVENT_NULL = 0,
   EN_STT_ALLOCATE_PORT,
   EN_STT_RECOGNITION_START,
   EN_STT_RECOGNITION_PAUSE,
   EN_STT_RECOGNITION_RESUME,
   EN_STT_RECOGNITION_STOP,
   EN_TTS_START,
   EN_STT_TTS_ALLOCATE_PORT,
   EN_STT_START,
   EN_STT_STOP,
   EN_CCAAS_VOICE_RECOG_START,
   EN_CCAAS_VOICE_RECOG_STOP,
   EN_TTS_STOP_REQ,
   EN_STT_GET_VARIABLE,
   EN_BOT_DEINIT,
};

typedef enum
{
   EN_XGATE_MG_TYPE_UNKNOWN = 0,
   EN_XGATE_MG_REGISTER = 1,
   EN_MEDIA_STT_TTS_PORT_ALLOCATE = 23,
   EN_XGATE_STT_TTS_RELEASE = 25,
   EN_XGATE_STT_TTS_REMOVE_PARTICIPANT = 26,
   EN_XGATE_STT_TTS_TRANSFER = 27,
   EN_XGATE_STT_TTS_MEDIA_CREATE = 28,
   EN_XGATE_STT_TTS_RECONNECTION = 31,
   EN_XGATE_STT_TTS_PLAY_TEXT = 38
}xGateMgMsgType;

typedef enum
{
   EN_MEDIA_EVENT_NULL = 0, // Inital state no actin to take in MG
   EN_MEDIA_EVENT_EOS = 3,
   EN_MEDIA_EVENT_EOL = 11,    //added as part of PLAY_LIST event. PBX may expect this response from MG
   EN_MEDIA_EVENT_REQUEST_IVR=12,
   EN_MEDIA_EVENT_TRANSFER = 16,
   EN_MEDIA_EVENT_ATTEND_TRANSFER = 17,
   EN_MEDIA_EVENT_FLIP = 18,
   EN_MEDIA_EVENT_BROADCAST_HOST_JOIN = 31,
   EN_MEDIA_EVENT_CALL_PARK = 39,//stop the play file and enable the voice channel
   EN_MEDIA_EVENT_PLAY_TEXT_EOS = 40,
   EN_MEDIA_EVENT_STT_TTS_ALLOC_RES = 41,
} xGateMediaEvent;

typedef enum
{
   EN_XGATE_CALL_TYPE_UNKNOWN,
   EN_XGATE_CALL_TYPE_SIMPLE,
   EN_XGATE_CALL_TYPE_CONFERENCE,
   EN_XGATE_CALL_TYPE_SIMPLE_VIDEO,
   EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO,
   EN_XGATE_CALL_TYPE_VMS,
   EN_XGATE_CALL_TYPE_TRANSFER, //this will allocat media for single leg
   EN_XGATE_CALL_TYPE_CCAAS, //this will allocat media for single leg
} xGateCallType;


typedef enum
{
   EN_JOINER_TYPE_UNKNOWN,
   EN_JOINER_TYPE_HOST,
   EN_JOINER_TYPE_PARTICIPANT,
} xGateJoinerType;

typedef enum
{
   EN_XGATE_MSG_PRIORITY_LOW,
   EN_XGATE_MSG_PRIORITY_HIGH,
   EN_XGATE_MSG_PRIORITY_UNKNOWN
}xGateMsgPriority;

typedef enum
{
   EN_XGATE_UNKNOWN_CONNECTION,
   EN_XGATE_NW_CONNECTION,
   EN_XGATE_MG_CONNECTION,
}xGateTCPConnectionType;

typedef enum
{
   EN_XGATE_TRANSPORT_UNKNOWN,
   EN_XGATE_TRANSPORT_UDP,
   EN_XGATE_TRANSPORT_TCP,
   EN_XGATE_TRANSPORT_TLS,
}xGateTransportType;

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
   EN_XGATE_TCP_RECV_MGC,
}xGateTCPReceiverType;

typedef enum
{
   EN_XGATE_MSG_UNKNOWN=0,
   EN_XGATE_MSG_MGC_MG_IN, 
   EN_XGATE_MSG_MGC_MG_OUT, 
   EN_XGATE_MSG_SCHEDULER,

#ifdef XGATE_HAS_SELF_AUDIT
   EN_XGATE_MSG_AUDIT, //added for self audit concept
#endif

   EN_XGATE_MSG_STT_TTS_IN,
   EN_XGATE_MSG_TIMER,
}xGateMsgType;

typedef enum
{
   EN_XGATE_HMP_UNKNOWN = 0,
   EN_XGATE_HMP_SERVICE = 1,
   EN_XGATE_AUDIT_SERVICE = 2,
}xGateServiceId;

#ifdef XGATE_HAS_MGC_MG_COMMUNICATION
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

class AudioDetail {
   public:
      AudioDetail();
      ~AudioDetail();
      AudioDetail& operator=(AudioDetail &src);


      unsigned int m_dtmfpt;
      unsigned int m_inBandDtmf;
      unsigned int m_HoldPartyDtmfpt;
      unsigned int m_HoldPartyInBandDtmf;

      unsigned int m_mediaType;
      string m_fmtp;
      string m_dtmfFmtp;
      string m_rtcpFpMechanism;
      unsigned int m_codec;
      string m_codecName;
      unsigned int m_samplingRate;
      unsigned int m_dtmfSamplingRate;
      unsigned int m_cnPt;
      unsigned int m_cnSamplingRate;
      unsigned int m_rtcpFpPayload;
      unsigned int m_ptime;
      unsigned int m_maxPtime;
      string m_clientIp;
      unsigned int m_clientPort;
      string m_serverIp;
      unsigned int m_serverPort;
      string m_relayIp;
      unsigned int m_relayPort;
      string m_rflxIp;
      unsigned int m_rflxPort;
      string m_dialIp;
      unsigned int m_dialPort;
      string m_respIp;
      unsigned int m_respPort;
      std::list<string> m_ivrList;
      bool m_is_silence_added;
      unsigned int m_dtmf_pt;
      unsigned int m_serverTtsPort;
      unsigned int m_server64Port;
};


struct MRCPMediaDetail
{
   string mg_resource_id;
   string call_id; 
   string codec_name;
   unsigned int codec;
   unsigned int ptime;
   string mrcp_codec_name;
   unsigned int mrcp_codec;
   unsigned int mrcp_ptime;
   string mrcp_mg_ip;
   string mrcp_mg_port;
   string mrcp_mg_ttsport;
   string mrcp_server_ip;
   string mrcp_server_port;
   string mrcpUid;
   string mrcpCallid;
   string mrcpDomainid;
   string mrcp_Devicetype;
   string mrcp_Ext;
   unsigned int mrcp_isSttEnabled;
   unsigned int transcript_mode;
   unsigned int tts_enabled;

   MRCPMediaDetail()
   {
      mg_resource_id = "";
      call_id = "";
      codec_name="";
      codec=0;
      ptime = 0;
      mrcp_codec_name = "";
      mrcp_codec = 0;
      mrcp_ptime=0;
      mrcp_mg_port="";
      mrcp_mg_ip="";
      mrcp_mg_ttsport= "";
      mrcp_server_port="";
      mrcp_server_ip="";
      mrcpUid = "";
      mrcpCallid = "";
      mrcpDomainid = "";
      mrcp_Devicetype = "";
      mrcp_Ext = "";
      mrcp_isSttEnabled = 0;
      transcript_mode = 0;
      tts_enabled = 0;
   };

   ~MRCPMediaDetail()
   {
   }

   //Assignment operator
   MRCPMediaDetail& operator=(MRCPMediaDetail &src)
   {
      mg_resource_id = src.mg_resource_id;
      call_id   = src.call_id;
      codec_name = src.codec_name;
      codec = src.codec;
      ptime = src.ptime;
      mrcp_codec_name = src.mrcp_codec_name;
      mrcp_codec =src.mrcp_codec;
      mrcp_codec = src.mrcp_codec;
      mrcp_ptime = src.mrcp_ptime;
      mrcp_mg_port = src.mrcp_mg_port;
      mrcp_mg_ip = src.mrcp_mg_ip;
      mrcp_mg_ttsport = src.mrcp_mg_ttsport;
      mrcp_server_port = src.mrcp_server_port;
      mrcp_server_ip = src.mrcp_server_ip;
      mrcpUid = src.mrcpUid;
      mrcpCallid = src.mrcpCallid;
      mrcpDomainid = src.mrcpDomainid;
      mrcp_Devicetype = src.mrcp_Devicetype;
      mrcp_Ext = src.mrcp_Ext;
      mrcp_isSttEnabled = src.mrcp_isSttEnabled;
      transcript_mode = src.transcript_mode;
      tts_enabled = src.tts_enabled;
      return *this;
   }
};

struct TtsDisconnectInfo
{
   string disconnCode;
   string disconnReason;

   TtsDisconnectInfo()
   {
      disconnCode = "";
      disconnReason = "";
   }
   ~TtsDisconnectInfo()
   {
   }
   TtsDisconnectInfo& operator=(TtsDisconnectInfo &src)
   {
      disconnCode = src.disconnCode;
      disconnReason = src.disconnReason;
      return *this;
   }
};

struct MgMediaDetail
{
   string mgresource_id;
   unsigned int call_type;
   unsigned int asr_type;
   string call_id; //this value of calltablekey generated in sofia.cpp
   string old_mgresource_id;
   string old_call_id; //this value of calltablekey generated in sofia.cpp
   string group_call_id;
   string switch_call_id;
   unsigned int gateway_id;
   unsigned int leg_id;
   int context_id;
   string media_proto;
   string out_proto;
   string record_file;
   unsigned int record_type;
   unsigned int dtmf_pt;
   long int file_size;
   string mg_ipv4_intf;
   string mg_ipv6_intf;
   string dtmf_digits;
   unsigned int media_event;
   unsigned int joiner_type;
   unsigned int reset_flag;
   unsigned int call_dir; //this value from CallDirection enum from call.h
   unsigned int sig_type; //this value from SignallingType enum from SignallingDriver.h
   string rtp_ep;
   unsigned int mrcp_port_allocate;
   unsigned int mrcp_port_activate;
   string mrcp_port;
   string mrcp_ip_info;
   AudioDetail audioDetail;
   string mrcp_codec_name;
   unsigned int mrcp_codec;
   unsigned int mrcp_ptime;
   string mrcp_mg_ip;
   string mrcp_mg_port;
   string mrcp_server_ip;
   string mrcp_server_port;
   string mrcpUid;
   string mrcpCallid;
   string mrcpDomainid;
   string mrcp_Devicetype;
   string mrcp_Ext;
   string ccaas_block_id;
   string ccaas_block_type;
   string ccaas_channel_id;
   unsigned int mrcp_isSttEnabled;
   STT_TTS_STATE mrcp_state;
   unsigned int transcript_mode;
   unsigned int tts_enabled;
   unsigned int mg_port;
   string mg_Ip;
   TtsDisconnectInfo tts_disconnect_info;
   string transfer_to;
   unsigned int m_serverTtsPort;
   string tts_text;
   unsigned int is_ccaas;
   unsigned int ccaas_port_type;
   unsigned int ccaas_user_agent;
   unsigned int tts_fd;
   unsigned int ccaas_chatbot_init;
   unsigned int ccaas_voice_recog_port;
   string ccaas_agent_id;
   unsigned int tts_type;
   string ccaas_bot_url;
   unsigned int ccaas_bot_first;
   unsigned int ccaas_bot_id;
   string ccaas_agent_name;
   string ccaas_agent_email;
   string ccaas_agent_user_id;
   string ccaas_cust_firstname;
   string ccaas_cust_lastname;
   unsigned int ccaas_auto_attendant;
   unsigned int ccaas_virtual_agent;
   string ccaas_cli;
   string ccaas_ddi;
   unsigned int is_smart_assist;

   MgMediaDetail()
   {
      mgresource_id = "";
      call_type=0;
      asr_type=0;
      call_id = "";
      old_mgresource_id = "";
      old_call_id = "";
      group_call_id = "";
      switch_call_id = "";
      gateway_id = 0;
      leg_id = 0;
      context_id = 0;
      media_proto = "";
      out_proto = "";
      record_file = "";
      file_size = 0;
      dtmf_digits = "";
      media_event = 0;
      joiner_type = 0;
      call_dir = 0;
      sig_type = 3;
      rtp_ep = "";
      record_type = 0;
      mg_ipv4_intf = "";
      mg_ipv6_intf = "";
      reset_flag = 0;
      dtmf_pt = 0;
      mrcp_port_allocate = 0;
      mrcp_port_activate =0;
      mrcp_ip_info= "";
      mrcp_port = "";
      mrcp_codec_name = "";
      mrcp_codec = 0;
      mrcp_ptime=0;
      mrcp_mg_port="";
      mrcp_mg_ip="";
      mrcp_server_port="";
      mrcp_server_ip="";
      mrcpUid = "";
      mrcpCallid = "";
      mrcpDomainid = "";
      mrcp_Devicetype = "";
      mrcp_Ext = "";
      mrcp_isSttEnabled = 0;
      mrcp_state = STT_TTS_STATE::EN_STT_TTS_STATE_EVENT_NULL;
      transcript_mode = 0;
      tts_enabled = 0;
      transfer_to = "";
      mg_Ip = "";
      mg_port = 0;
      m_serverTtsPort =0;
      tts_text = "";
      is_ccaas = 0;
      ccaas_block_id="";
      ccaas_block_type="";
      ccaas_channel_id="";
      ccaas_port_type = 0;
      ccaas_user_agent = 0; 
      tts_fd = 0;
      ccaas_user_agent = 0;
      ccaas_chatbot_init = 0;
      ccaas_voice_recog_port = 0;
      ccaas_agent_id = "";
      tts_type = 0;
      ccaas_bot_url = "";
      ccaas_bot_first  = 0;
      ccaas_bot_id = 0;
      ccaas_agent_name = "";
      ccaas_agent_email = "";
      ccaas_agent_user_id = "";
      ccaas_cust_firstname = "";
      ccaas_cust_lastname = "";
      ccaas_auto_attendant = 0;
      ccaas_virtual_agent = 0;
      ccaas_cli = "";
      ccaas_ddi = "";
      is_smart_assist = 0;
   };

   ~MgMediaDetail()
   {
   }

   //Assignment operator
   MgMediaDetail& operator=(MgMediaDetail &src)
   {
      mgresource_id = src.mgresource_id;
      call_type = src.call_type;
      asr_type = src.asr_type;
      call_id   = src.call_id;
      old_mgresource_id = src.old_mgresource_id;
      old_call_id = src.old_call_id;
      group_call_id = src.group_call_id;
      switch_call_id = src.switch_call_id;
      gateway_id = src.gateway_id;
      leg_id = src.leg_id;
      context_id = src.context_id;
      media_proto = src.media_proto;
      out_proto = src.out_proto;
      record_file = src.record_file;
      record_type = src.record_type;
      dtmf_pt = src.dtmf_pt;
      file_size = src.file_size;
      dtmf_digits = src.dtmf_digits;
      media_event = src.media_event;
      joiner_type = src.joiner_type;
      reset_flag = src.reset_flag;
      call_dir  = src.call_dir;
      sig_type  = src.sig_type;
      rtp_ep = src.rtp_ep;
      mrcp_port_allocate = src.mrcp_port_allocate;
      mrcp_port_activate = src.mrcp_port_activate;
      mrcp_ip_info = src.mrcp_ip_info;
      mrcp_port = src.mrcp_port;
      mg_ipv4_intf = src.mg_ipv4_intf;
      mg_ipv6_intf = src.mg_ipv6_intf;
      audioDetail =  src.audioDetail;
      mrcp_codec_name = src.mrcp_codec_name;
      mrcp_codec =src.mrcp_codec;
      mrcp_ptime = src.mrcp_ptime;
      mrcp_mg_port = src.mrcp_mg_port;
      mrcp_mg_ip = src.mrcp_mg_ip;
      mrcp_server_port = src.mrcp_server_port;
      mrcp_server_ip = src.mrcp_server_ip;
      mrcpUid = src.mrcpUid;
      mrcpCallid = src.mrcpCallid;
      mrcpDomainid = src.mrcpDomainid;
      mrcp_Devicetype = src.mrcp_Devicetype;
      mrcp_Ext = src.mrcp_Ext;
      mrcp_isSttEnabled = src.mrcp_isSttEnabled;
      mrcp_state = src.mrcp_state;
      transcript_mode = src.transcript_mode;
      tts_disconnect_info = src.tts_disconnect_info;
      tts_enabled = src.tts_enabled;
      transfer_to = src.transfer_to;
      mg_Ip = src.mg_Ip;
      mg_port = src.mg_port;
      m_serverTtsPort = src.m_serverTtsPort;
      tts_text = src.tts_text;
      is_ccaas = src.is_ccaas;
      ccaas_port_type = src.ccaas_port_type;
      ccaas_user_agent = src.ccaas_user_agent;
      ccaas_block_id= src.ccaas_block_id;
      ccaas_block_type=src.ccaas_block_type;
      ccaas_channel_id=src.ccaas_channel_id; 
      tts_fd = src.tts_fd;
      ccaas_chatbot_init = src.ccaas_chatbot_init;
      ccaas_voice_recog_port = src.ccaas_voice_recog_port;
      ccaas_agent_id = src.ccaas_agent_id;
      tts_type = src.tts_type;
      ccaas_bot_url = src.ccaas_bot_url;
      ccaas_bot_first = src.ccaas_bot_first;
      ccaas_bot_id = src.ccaas_bot_id;
      ccaas_agent_name = src.ccaas_agent_name;
      ccaas_agent_email = src.ccaas_agent_email;
      ccaas_agent_user_id = src.ccaas_agent_user_id;
      ccaas_cust_firstname = src.ccaas_cust_firstname;
      ccaas_cust_lastname = src.ccaas_cust_lastname;
      ccaas_auto_attendant = src.ccaas_auto_attendant;
      ccaas_virtual_agent = src.ccaas_virtual_agent;
      ccaas_cli = src.ccaas_cli;
      ccaas_ddi = src.ccaas_ddi;
      is_smart_assist = src.is_smart_assist;
      return *this;
   }
};

#endif
typedef enum
{
   EN_XGATE_UNKNOWN = 0,
   EN_XGATE_STT = 1,
   EN_XGATE_TTS = 2,
   EN_XGATE_BOTH = 3,
}xGatePortType;

struct xGateListenAddress
{
   std::string     ipAddress;
   unsigned short  port;
   xGateListenAddress():ipAddress(""), port(0)
   {
   }
   ~xGateListenAddress()
   {
   }
};

struct xGateTCPConnectionStr
{
   xGateTCPConnectionType conType;
   string peerIPAddress;  
   string fqdn;           

   xGateTCPConnectionStr()
   {
      conType = EN_XGATE_UNKNOWN_CONNECTION;
   }

   ~xGateTCPConnectionStr()
   {
   }

   xGateTCPConnectionStr& operator=(xGateTCPConnectionStr &src)
   {
      conType       = src.conType;
      peerIPAddress = src.peerIPAddress;
      fqdn          = src.fqdn;
      return *this;
   }

   bool operator==(xGateTCPConnectionStr &src)
   {
      if(conType   == src.conType && peerIPAddress == src.peerIPAddress &&  fqdn == src.fqdn)
	 return true;
      else
	 return false;
   }
};

struct xGateTCPConTuple
{
   std::string     ipAddress;
   unsigned short  port;
   int             fd;
   unsigned short  srcPort;
   unsigned short timeoutCount;
   xGateTCPReceiverType eReceiverType;
   long int timerId;

   xGateTCPConTuple()
   {
      port     = 0;
      fd       = -1;
      srcPort  = 0;
      timeoutCount = 0;
      eReceiverType = EN_XGATE_TCP_RECV_UNKNOWN;
      timerId = 0;
   }

   ~xGateTCPConTuple()
   {
   }

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

struct xGateSchedulerData
{
   xGateSchedulerData(std::string &uid, unsigned int type, int retryCount) :
      m_uid(uid), m_schedulerType(type), m_retryCount(retryCount) {};
   ~xGateSchedulerData(){};
   std::string m_uid;
   unsigned int m_schedulerType;
   int m_retryCount;
};

struct xGateNetConnection
{
   std::string     recvIp_;
   unsigned short  recvPort_;
   xGateTransportType  protocol_;
   int fd_;
   xGateTCPConnectionType conType_;
   unsigned short  srcPort_;


   xGateNetConnection()
   {
      recvPort_= 5080;
      protocol_= EN_XGATE_TRANSPORT_UDP;
      fd_      = 0;
      conType_ = EN_XGATE_UNKNOWN_CONNECTION;
      srcPort_ = 0;
   }

   ~xGateNetConnection()
   {
   }

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

class xGateUtil {
   public:
      static void addToMgConnectionMap(xGateTCPConTuple);
      static xGateRetVal getFromMgConnectionMap(string, unsigned short, int&);
      static void removeFromMgConnectionMap(xGateTCPConTuple);
      static bool isPresentInMgConnectionMap(string,unsigned short);
      static void getTimerIdFromConnMap (int fd, xGateTCPConnectionType eConnType, long int &timerId);
      /// Method to set and get local ip4/ip6 addrs
      static xGateRetVal findLocalIpAddr(char *ipAddr, unsigned short AddrType = AF_INET);
      static void SetLocalIpAddr(char *localIpAddr);
      static char * getLocalIpAddr(unsigned short AddrType = AF_INET);
      static void setLocalIpAddr(char *localIpAddr);
      static void makeDir(const char *dir);
      static  string convertToString(char* a, int size);

      static gboolean Thread_creation(void *,void (*Pfunction)(void *),string classname);
      static gboolean Thread_destruction(ACE_thread_t tid);


      /// Method to set the Mg Receiver
      static void setMgReceiver(xGateTCPReceiver* receiver)
      {
	 m_mgReceiver = receiver;
      }

      /// Method to access the Mg Receiver
      static xGateTCPReceiver* getMgReceiver(void)
      {
	 return  m_mgReceiver;
      }

      /// Method to access the Srvr Processor
      static xGateMgProcessor* getMgProcessor(void)
      {
	 return  m_mgProcessor;
      }

      /// Method to set the Srvr Processor
      static void setMgProcessor(xGateMgProcessor* mgProcessor)
      {
	 m_mgProcessor = mgProcessor;
      }

#ifdef XGATE_HAS_GSTREAMER
      /// Method to access the Hmp Processor
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
#endif

#ifdef XGATE_HAS_MGC_MG_COMMUNICATION
      static  xGateMGCConnector* getMgcConnector(void)
      {
	 return m_mgcConnector;
      }

      static void setMgcConnector(xGateMGCConnector* pConnector)
      {
	 m_mgcConnector = pConnector;
      }	

      static xGateMGDispatcher* getMgDispatcher()
      {
	 return m_mgDispatcher;
      }

      static void setMgDispatcher(xGateMGDispatcher* mgdispatcher) {
	 m_mgDispatcher= mgdispatcher;;

      }
#endif

   private:
      xGateUtil();
      xGateUtil(const xGateUtil& rhs);
      xGateUtil& operator= (const xGateUtil& rhs);
      static vector<xGateTCPConTuple> m_mgConnectionMap;
      static ACE_Thread_Mutex m_mgConnectionMapLock;

      /// Store the Connector
      static xGateMGCConnector* m_mgcConnector;

      /// Store the TCP Receiver
      static xGateTCPReceiver* m_mgReceiver;

      /// Store the Server Processor
      static xGateMgProcessor* m_mgProcessor;

      static xGateMGDispatcher* m_mgDispatcher;

      /// Store the Hmp Processor
      static xGateHmpProcessor* m_hmpProcessor;

      /// Store the Hmp Gstreamer Manager
      static xGateHmpGstManager* m_hmpGstManager;

   public:
      static xGateTCPSocketHandler* m_tcpHandler;
      static vector<string> m_MgcIpAddressList;
      static char m_localIp4Addr[INET_ADDRSTRLEN];
      static char m_localIp6Addr[INET6_ADDRSTRLEN];
};

class ur_log_string : public std::string
{
   public:
      ur_log_string (const std::string& tcstring)
      {
	 if(tcstring.length() < 1)
	 {
	    std::string::operator=("empty_data");
	 }
	 else 
	    std::string::operator=(tcstring);
      }
      ur_log_string(const char*  lpcszStr)
      {
	 if(lpcszStr == NULL)
	 {
	    std::string::operator=("null_data");
	 }
	 else
	 {
	    std::string::operator=(lpcszStr);
	    if(this->length() < 1)
	    {
	       std::string::operator=("empty_data");
	    }
	 }
      }
      operator char*()
      {
	 return (char*)this->c_str();
      }
};

#endif
