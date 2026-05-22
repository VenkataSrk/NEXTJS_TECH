#ifndef _XGATE_UTIL_H
#define _XGATE_UTIL_H

//system includes
#include <string>
#include <map>
#include <vector>
#include <list>
//ace inludes
#include <ace/Guard_T.h>
#include <ace/Thread_Manager.h>
#include <ace/SOCK_Stream.h>

//local includes

#define XGATE_HAS_JSON_SUPPORT

#ifdef XGATE_HAS_JSON_SUPPORT
//rapidjson includes
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/ostreamwrapper.h"
#include "jsonlookaheadparser.h"
#include <iostream>
using namespace rapidjson;
#endif

#include <openssl/evp.h>
#include <openssl/x509.h>
using namespace std;

#define SERVER_TYPE_SIZE 20
#define SERVER_ADDRESS_LENGTH 50
#define MGC_MG_SIGNAL_PORT 32779
#define XGATE_TCP_READ_SIZE  2048
#define SIP_SERVER_PORT 5060
#define ENCODE_BUF_SIZE 4*1024
#define MAX_NUM_SFU 10000 
#ifdef XGATE_HAS_JSON_SUPPORT
#define DEFAULT_STUN_USERNAME "admin"
#define DEFAULT_STUN_PASSWORD "system123"
#define DEFAULT_STUN_DOMAIN "vturn01.unifiedring.co.uk"
#define DEFAULT_STUN_PORT 3478

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
                                          \"gateway_id\" : 0, \
                                          \"context_id\" : 0, \
                                          \"is_ccaas\" : 0, \
                                          \"leg_id\" : 0, \
                                          \"retrieved_audiossrc\" : 0, \
                                          \"retrieved_videossrc\" : 0, \
                                          \"media_proto\" : \"\",\
                                          \"out_proto\": \"\", \
                                          \"record_file\": \"\", \
                                          \"browser_type\": \"\", \
                                          \"record_url\": \"\", \
                                          \"record_id\": \"\", \
                                          \"meeting_name\": \"\", \
                                          \"sip_id\": \"\", \
                                          \"customer_num\": \"\", \
                                          \"domain_id\": 0, \
                                          \"agent_id\": 0, \
                                          \"file_size\": 0, \
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
                                          \"play_file\" : \"\",\
                                          \"framerate\": 15,\
                                          \"imageattr_x\": 0,\
                                          \"imageattr_y\": 0,\
                                          \"ssrc\": \"\", \
                                          \"cname\": \"\", \
                                          \"mslable\": \"\", \
                                          \"lable\": \"\"}], \
                                          \"dtmf_digits\" : \"\",\
					  \"screenshare_id\" : \"\", \
                                          \"sdp_str\": \"\", \
                                          \"joiner_type\" : 0,\
                                          \"sig_type\" : 0, \
                                          \"call_dir\" : 0, \
                                          \"media_state\": \"\", \
                                          \"d_r_e\": \"\", \
					  \"o_r_e\": \"\", \
					  \"call_Id_msg10\": \"\", \
					  \"pClient\" : 0, \
																					\"urmeet_user_list\":[{ \
																					\"email\": \"\",\
																					\"profile_img\": \"\", \
																					\"sip_id\": \"\", \
																					\"name\": \"\", \
																					\"ext\": \"\", \
																					\"role_type\": 0, \
																					\"user_type\": \"\"}], \
                                            \"participant_list\":[{ \
                                          \"call_id\": \"\",\
                                          \"joiner_type\": 0}]}}";

#endif



class xGateTCPSocketHandler;
class xGateTCPReceiver;
//class MBCControllerProcessor;
class xGateMGDispatcher;
class xGateMGCConnector;
class MBCSForwarderController;
class MBCSessionHandler;
//class SFUController;
class SFUBridgeProcessor;
class xGateMgSFUBridgeProcessor;
class SFUBridgeProcessor;
class xGateMgSFUDispatcher;

typedef enum
{
  EN_XGATE_STATUS_NOT_PRESENT = -1,
  EN_XGATE_STATUS_NOT_SUPPORTED,
  EN_XGATE_STATUS_SUCCESS,
  EN_XGATE_STATUS_STOPPING,
  EN_XGATE_STATUS_ACE_ERROR,
  EN_XGATE_STATUS_ENQUEUE_FAIL,
  EN_XGATE_STATUS_DEQUEUE_FAIL,
  EN_XGATE_STATUS_NO_CONNECTION,
  EN_XGATE_STATUS_ERROR
}xGateRetVal;

typedef enum
{
  EN_XGATE_MG_TYPE_UNKNOWN=0,
  EN_XGATE_MG_REGISTER = 1,
  EN_XGATE_MG_ALLOCATE_ORGINATE = 2,
  EN_XGATE_MG_ALLOCATE_TERMINATE = 3,
  EN_XGATE_MG_RELEASE = 4,
  EN_XGATE_MG_18X_RESPONSE = 5,
  EN_XGATE_MG_PLAY_BACK = 6,
  EN_XGATE_MG_MEDIA_CREATE = 7,
  EN_XGATE_MG_HOLD_RESUME = 8,
  EN_XGATE_MG_DTMF_DIGITS = 9,
  EN_XGATE_MG_ADD_PARTICIPANT =10,
  EN_XGATE_MG_MEDIA_DELETE = 11, 
  EN_XGATE_MG_REMOVE_PARTICIPANT = 12,
  EN_XGATE_MG_CALL_RECORD = 13, //call recording event
  EN_XGATE_MG_SWITCH_CALLTO_CONFERENCE = 14,
  EN_XGATE_MG_SFU_CONN_CLOSED = 15,
  EN_XGATE_MG_MEDIA_ALLOCATE_121_CLIENT1 = 16,
  EN_XGATE_MG_MEDIA_ALLOCATE_121_CLIENT2 = 17,
  EN_XGATE_MG_MEDIA_121_CLIENT1_TERMINATE = 18,
  EN_XGATE_MG_MEDIA_121_CLIENT2_TERMINATE = 19,
  EN_XGATE_MG_SFU_ID = 20,
  EN_XGATE_MG_MEDIA_REQUEST_KEY_FRAME = 21,
  EN_XGATE_MG_MEDIA_ADD_CLIENT_CONF_LIST = 22,
  EN_XGATE_MG_MEDIA_REQUEST_CREATE_SEND_CHANNEL = 23,
  EN_XGATE_MG_SDP_UPDATE_REQUEST = 24,
  EN_XGATE_MG_SDP_UPDATE_RESPONSE = 25,
  EN_XGATE_MG_MEDIA_DTLS_CONNECTION_FAILED = 26,
  EN_XGATE_MG_RETRY_MEDIA_CONNECTION = 27,
  EN_XGATE_MG_VIDEO_REC_MSG = 28,
  EN_XGATE_MG_VIDEO_REC_MSG_RESPONSE = 29,
  EN_XGATE_MG_VIDEO_KEY_FRAME_REQUEST = 36,

}xGateMgMsgType;

typedef enum
{
  EN_MEDIA_EVENT_NULL = 0, // Inital state no actin to take in MG
  EN_MEDIA_EVENT_PLAY = 1,	// state at PLAY create RBT pipeline in Mg and play RBT
  EN_MEDIA_EVENT_STOP = 2,	// State at STOP if RBT pipeline is running stop the play
  EN_MEDIA_EVENT_EOS = 3,
  EN_MEDIA_EVENT_REPEAT = 4,
  EN_MEDIA_EVENT_HOLD = 5,
  EN_MEDIA_EVENT_RESUME = 6,
  EN_MEDIA_EVENT_GETDIGIT = 7,   // Request for start to collect the DTMF Digit
  EN_MEDIA_EVENT_SENDDIGIT = 8,  // Request for send the Collected DTMF Digits.
  EN_MEDIA_EVENT_RECVDIGIT = 9,    // Posting the Collected DTMF Digits.
  EN_MEDIA_EVENT_MUTE = 10,
  EN_MEDIA_EVENT_UNMUTE = 11,
  EN_MEDIA_EVENT_DEAF = 12,
  EN_MEDIA_EVENT_LOCK = 13,
  EN_MEDIA_EVENT_UNLOCK = 14,
  EN_MEDIA_EVENT_EXTEND = 15,
  EN_MEDIA_EVENT_TRANSFER = 16,
  EN_MEDIA_EVENT_CONFALL = 17,
  EN_MEDIA_EVENT_CONF_END = 18,
  EN_MEDIA_EVENT_RECORD_PORT = 19,
  EN_MEDIA_EVENT_RECORD_BIND = 20,
  EN_MEDIA_EVENT_RECORD_START = 21, //record both audio, video and entire call
  EN_MEDIA_EVENT_RECORD_PAUSE = 22, //record both audio, video and entire call
  EN_MEDIA_EVENT_RECORD_RESUME = 23, //record both audio, video and entire call
  EN_MEDIA_EVENT_RECORD_STOP = 24,
  EN_MEDIA_EVENT_RECORD_SUCCESS = 25,
  EN_MEDIA_EVENT_RECORD_FAILED = 26,
  EN_MEDIA_EVENT_RECORD_INFO = 27,
  EN_MEDIA_EVENT_RTP_TIMEOUT = 28,
  EN_MEDIA_EVENT_RECORD_SSRC_REMOVE = 30,
}xGateMediaEvent;

typedef enum
{
  EN_XGATE_CALL_TYPE_UNKNOWN,
  EN_XGATE_CALL_TYPE_SIMPLE,
  EN_XGATE_CALL_TYPE_CONFERENCE,
  EN_XGATE_CALL_TYPE_SIMPLE_VIDEO,
  EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO,
  EN_XGATE_CALL_TYPE_VMS
} xGateCallType;

typedef enum
{
  EN_MEDIA_SDPINFO_UNKNOWN=0,
  EN_MEDIA_SDPINFO_AUDIO=2,
  EN_MEDIA_SDPINFO_VIDEO=3,
} xGateMediaSdpType;

typedef enum
{
  EN_JOINER_TYPE_UNKNOWN,
  EN_JOINER_TYPE_HOST,
  EN_JOINER_TYPE_PARTICIPANT,
  EN_JOINER_TYPE_SCREENSHARE,
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
  EN_XGATE_MSG_SERVICE_CONTROLLER,
  EN_XGATE_MSG_XG_MBC_MODULE,
  EN_XGATE_MSG_REDIS_SUBSCRIBE_NOTIFY,// redis message for subscriber
  EN_XGATE_MSG_REDIS_CONN_FAILED_NOTIFY,
  EN_XGATE_MSG_REDIS_PUBLISH,
  EN_XGATE_MSG_REDIS_GET,
  EN_XGATE_MSG_REDIS_GET_RES,
  EN_XGATE_MSG_REDIS_SET,

}xGateMsgType;

typedef enum
{
  EN_CANDIDATE_TYPE_UNKNOWN=0,
  EN_CANDIDATE_TYPE_HOST=1, 
  EN_CANDIDATE_TYPE_PRFLX=2, //peer reflexive candidate
  EN_CANDIDATE_TYPE_SRFLX=3, //server reflexive candidate
  EN_CANDIDATE_TYPE_RELAY=4, //relay candidate
}xGateCandidateType;

typedef enum
{
  EN_XGATE_RCHOLD=0,
  EN_XGATE_RCINCREASE=1,
  EN_XGATE_RCDECREASE=2
}xGateRateControlState;

typedef enum
{
  EN_XGATE_MINLOSS=0,
  EN_XGATE_MEDIUMLOSS=1,
  EN_XGATE_MAXLOSS=2
}xGateLossLevel;

typedef enum
{
  EN_XGATE_BW_NORMAL = 0,
  EN_XGATE_BW_UNDERUSING = 1,
  EN_XGATE_BW_OVERUSING = 2,
  EN_XGATE_LAST
}xGateBandwidthUsage;




struct MgRegistrationDetail
{
  char gw_name[25];
  char gw_profile[25];
  int channels_count;
  int rsc_count;
  char rls_record[500];

  MgRegistrationDetail() : gw_name("0"), gw_profile("0"), channels_count(0), rsc_count(0), rls_record("0")
  {
  };
};

struct CandidateDetail
{
  CandidateDetail() : m_piority(1), m_transportType(EN_XGATE_TRANSPORT_UDP), 
  m_raddr(""), m_rport(0), m_candidateType(EN_CANDIDATE_TYPE_UNKNOWN)
  {};
  ~CandidateDetail(){};
  unsigned int m_piority;
  xGateTransportType m_transportType;
  string m_raddr; //remote ip address
  unsigned int m_rport; //remote port
  xGateCandidateType m_candidateType; 
};

typedef vector<CandidateDetail> CANDIDATE_LIST;

class TcpLisentAddr
{
  public:
    string m_ip;
    unsigned short m_port;
    TcpLisentAddr() : m_ip(""), m_port(0) {};
    TcpLisentAddr(string &ip, int &port) : m_ip(ip), m_port(port) {};
    ~TcpLisentAddr() {};
};

class MediaSecureDetail
{
  public:
    MediaSecureDetail();
    ~MediaSecureDetail();
    //Assignment operator
    MediaSecureDetail& operator=(MediaSecureDetail &src);

    //public member properties
    //CANDIDATE_LIST m_candidateList; 
    CandidateDetail m_candidateDetail;
    string m_pemFile;
    char m_pemDir[100];
    string m_lIceUfrag; //locally created ice ufrag
    string m_lIcePwd; //locally created ice password
    string m_rIceUfrag; //remote ice ufrag
    string m_rIcePwd; //remote ice password
    string m_lFingerPrint; //locally created finger print
    string m_rFingerPrint; //remote finger print

    string m_lSsrc, m_rSsrc; //local and remote ssrc
    string m_lCname, m_rCname; //local and remote cname
    string m_lMsLabel, m_rMsLabel; //local and remote mslable
    string m_lLabel, m_rLabel;

    //public member functions
    xGateRetVal add_candidate();
    xGateRetVal remove_candidate();
    xGateRetVal clear_candidate_list();
};
/*  MediaSecureDetail class - End
*/
class SslCertificate
{
  public:
    SslCertificate();
    ~SslCertificate();
    EVP_PKEY * pkey;
    X509 * x509;
    BIGNUM* exponent;
    RSA* rsa;
};

struct opCODEC {
  string codec_Namme;
  string codec_Info;
  int assigned_Number;
};
typedef std::vector<opCODEC> OP_CODEC_LIST;

class AudioDetail {
  public:
    AudioDetail();
    ~AudioDetail();
    AudioDetail& operator=(AudioDetail &src);

    unsigned int m_mediaType;
    string m_fmtp;
    unsigned int m_codec;
    unsigned int m_dynamicCodec;
    string m_codecName;
    unsigned int m_ptime;
    string m_playFile;
    string m_clientIp;
    unsigned int m_clientPort;
    string m_serverIp;
    unsigned int m_serverPort;
    string m_relayIp;
    unsigned int m_relayPort;
    string m_rflxIp;
    unsigned int m_rflxPort;
    MediaSecureDetail m_secureDetail;
    string m_dialIp;
    unsigned int m_dialPort;
    string m_respIp;
    unsigned int m_respPort;
    SslCertificate m_sslCertificate;
    int m_rtp_stream_ext_id;
    int m_repair_rtp_stream_ext_id;
    string m_mid_audio;
};

class VideoDetail {
  public:
    VideoDetail();
    ~VideoDetail();
    VideoDetail& operator=(VideoDetail &src);

    unsigned int m_mediaType;
    string m_fmtp;
    unsigned int m_codec;
    unsigned int m_dynamicCodec;
    string m_codecName;
    unsigned int m_frameRate;
    unsigned int m_imageattr_x;
    unsigned int m_imageattr_y;
    string m_playFile;
    string m_clientIp;
    unsigned int m_clientPort;
    string m_serverIp;
    unsigned int m_serverPort;
    string m_relayIp;
    unsigned int m_relayPort;
    string m_rflxIp;
    unsigned int m_rflxPort;
    MediaSecureDetail m_secureDetail;
    string m_dialIp;
    unsigned int m_dialPort;
    string m_respIp;
    unsigned int m_respPort;
    OP_CODEC_LIST m_opCodec;
    SslCertificate m_sslCertificate;
};

class StunConfig
{
public:
  StunConfig();
  ~StunConfig();
  StunConfig& operator=(StunConfig &src);

  string m_userName;
  string m_password;
  string m_stunDomain;
  unsigned int m_port;
};

class ParticipantInfo{
public:
  int joiner_type;
  string call_id;
};
typedef std::vector<ParticipantInfo > PARTICIPANT_LIST;

class URMeetUserInfo{
  public:
		URMeetUserInfo():email(""), profile_img(""), sip_id(""), name(""), ext(""), user_type(""), role_type(0){};
		~URMeetUserInfo(){};

		URMeetUserInfo& operator=(const URMeetUserInfo& src)
		{
			email = src.email;
			profile_img = src.profile_img;
			sip_id = src.sip_id;
			name = src.name;
			ext = src.ext;
			user_type = src.user_type;
			role_type = src.role_type;
			return *this;
		};
  string email;
  string profile_img;
  string sip_id;
  string name;
  string ext;
  string user_type;
  unsigned int role_type;
};
typedef std::vector<URMeetUserInfo> USERINFO;

class xGateUtil
{
public:
  xGateUtil(){}
  ~xGateUtil(){}
  static char m_localIp4Addr[INET_ADDRSTRLEN];
  static char m_localIp6Addr[INET6_ADDRSTRLEN]; 
  static xGateRetVal findLocalIpAddr(char*,unsigned short);
  static char* getLocalIpAddr(unsigned short AddrType = AF_INET);
  static void setLocalIpAddr(const char*); 
};

struct MgMediaDetail
{
  string mgresource_id;
  unsigned int mgmsg_type;
  string mg_ip; 
  unsigned int call_type;
  string call_id; //this value of calltablekey generated in sofia.cpp
  string screenshare_id;
  unsigned int gateway_id;
  unsigned int leg_id;
  unsigned int retrieved_audiossrc;
  unsigned int retrieved_videossrc;
  int context_id;
  int is_ccaas;
  int domain_id;
  int agent_id;
  string media_proto;
  string out_proto;
  string record_file;
  string browser_type;
	string record_url;
	string record_id;
	string meeting_name;
	string sip_id;
  string customer_num;
  long int file_size;
  string dtmf_digits;
  unsigned int media_event;
  unsigned int joiner_type;
  unsigned int call_dir; //this value from CallDirection enum from call.h
  unsigned int sig_type; //this value from SignallingType enum from SignallingDriver.h
  string d_rtp_ep;
  string o_rtp_ep;
  string sdp_str;
  string call_Id_msg10;
  uint64_t pClient;
  AudioDetail audioDetail;
  VideoDetail videoDetail;
  PARTICIPANT_LIST Participant_list;
	USERINFO meetuser_info;
  MgMediaDetail()
  {
    mgresource_id = "";
    mgmsg_type = 0;
    mg_ip = "";
    call_type=0;
    call_id = "";
    gateway_id = 0;
    leg_id = 0;
    retrieved_videossrc = 0;
    retrieved_audiossrc = 0;
    context_id = 0;
    is_ccaas = 0;
    domain_id = 0;
    agent_id = 0;
    media_proto = "";
    out_proto = "";
    record_file = "";
    browser_type = "";
		record_url = "";
		record_id = "";
		meeting_name = "";
		sip_id = "";
		customer_num = "";
    file_size = 0;
    dtmf_digits = "";
    media_event = 0;
    screenshare_id = "";
    joiner_type = 0;
    call_dir = 0;
    sig_type = 3;
    d_rtp_ep = "";
    o_rtp_ep = "";
    sdp_str="";
    call_Id_msg10 = "";
    pClient = 0;
  };

  ~MgMediaDetail()
  {
  }

  //Assignment operator
  MgMediaDetail& operator=(MgMediaDetail &src)
  {
    mgresource_id = src.mgresource_id;
    mgmsg_type = src.mgmsg_type;
    mg_ip = src.mg_ip;
    call_type = src.call_type;
    call_id   = src.call_id;
    gateway_id = src.gateway_id;
    leg_id = src.leg_id;
    retrieved_videossrc = src.retrieved_videossrc;
    retrieved_audiossrc = src.retrieved_audiossrc;
    context_id = src.context_id;
    is_ccaas = src.is_ccaas;
    domain_id = src.domain_id;
    agent_id = src.agent_id;
    media_proto = src.media_proto;
    out_proto = src.out_proto;
    record_file = src.record_file;
    browser_type = src.browser_type;
    record_url = src.record_url;
    record_id = src.record_id;
    meeting_name = src.meeting_name;
    sip_id = src.sip_id;
    customer_num = src.customer_num;
    file_size = src.file_size;
    dtmf_digits = src.dtmf_digits;
    media_event = src.media_event;
    screenshare_id = src.screenshare_id;
    joiner_type = src.joiner_type;
    call_dir  = src.call_dir;
    sig_type  = src.sig_type;
    d_rtp_ep = src.d_rtp_ep;
    o_rtp_ep = src.o_rtp_ep;
    sdp_str = src.sdp_str;
    call_Id_msg10 = src.call_Id_msg10;
    pClient = src.pClient;
    audioDetail =  src.audioDetail;
    videoDetail = src.videoDetail;
    Participant_list = src.Participant_list;
		meetuser_info = src.meetuser_info;
    return *this;
  }
  static string  getMediaMsgType(int value)
   {
	switch (value)
	{ 
	  case EN_XGATE_MG_TYPE_UNKNOWN:{
		return "EN_XGATE_MG_TYPE_UNKNOWN";
	  }
	  case EN_XGATE_MG_REGISTER :{
		return "EN_XGATE_MG_REGISTER";
	  }
	  case EN_XGATE_MG_ALLOCATE_ORGINATE :{
		return "EN_XGATE_MG_ALLOCATE_ORGINATE";
	  }
	  case EN_XGATE_MG_ALLOCATE_TERMINATE :{
		return "EN_XGATE_MG_ALLOCATE_TERMINATE";
	  }
	  case EN_XGATE_MG_RELEASE :{
		return "EN_XGATE_MG_RELEASE";
	  }
	  case EN_XGATE_MG_18X_RESPONSE :{
		return "EN_XGATE_MG_18X_RESPONSE";
	  }
	  case EN_XGATE_MG_PLAY_BACK :{
		return "EN_XGATE_MG_PLAY_BACK";
	  }
	  case EN_XGATE_MG_MEDIA_CREATE :{
		return "EN_XGATE_MG_MEDIA_CREATE";
	  }
	  case EN_XGATE_MG_HOLD_RESUME :{
		return "EN_XGATE_MG_HOLD_RESUME";
	  }
	  case EN_XGATE_MG_DTMF_DIGITS :{
		return "EN_XGATE_MG_DTMF_DIGITS";
	  }
	  case EN_XGATE_MG_ADD_PARTICIPANT :{
		return "EN_XGATE_MG_ADD_PARTICIPANT";
	  }
	  case EN_XGATE_MG_MEDIA_DELETE :{
		return "EN_XGATE_MG_MEDIA_DELETE";
	  }
	  case EN_XGATE_MG_REMOVE_PARTICIPANT :{
		return "EN_XGATE_MG_REMOVE_PARTICIPANT";
	  }
	  case EN_XGATE_MG_CALL_RECORD :{
		return "EN_XGATE_MG_CALL_RECORD";
	  }
	  case EN_XGATE_MG_SWITCH_CALLTO_CONFERENCE :{
		return "EN_XGATE_MG_SWITCH_CALLTO_CONFERENCE";
	  }
		default:{
			return "EN_XGATE_MG_TYPE_UNKNOWN";
		}
	}
     }


};

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

struct MBCSfuMsg {
  char *msg;
  int  len;
  int  sfuId;
};
#endif
