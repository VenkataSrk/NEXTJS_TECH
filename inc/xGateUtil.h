#ifndef _XGATE_UTIL_H
#define _XGATE_UTIL_H
//ace includes
#include <ace/Task.h>
#include <ace/Singleton.h>
#include <queue>

//rapidjson includes
#include "document.h"
#include "error/en.h"
#include "writer.h"
#include "reader.h"
#include "stringbuffer.h"
#include "prettywriter.h"
#include "ostreamwrapper.h"
using namespace rapidjson;

//local includes
#include "xGateDefine.h"
#include "xGateBaseMsg.h"
#include "xglog.h"

#define XGATE_TCP_READ_SIZE  2048
#define TCP_MAX_MESSAGE_SIZE 8192

typedef enum
{
  EN_BUSINESS_UNKNOWN_MSG=0,
  EN_BUSINESS_SIGLAYER_MSG=1, //message between business and signalling layer
  EN_BUSINESS_DBLAYER_MSG=2, //message between business and db layer
  EN_BUSINESS_SCHEDULER_MSG=3, //message between business and db layer
  EN_BUSINESS_HTTP_MSG=4, //message between http module and business layer
}xGateBusinessMsgType;

typedef enum
{
  EN_SCHEDULER_EVENT_UNKNOWN=0,
  EN_SCHEDULER_EVENT_TIMEOUT=1,
  EN_SCHEDULER_EVENT_CANCEL=2,
}xGateBusinessSchedulerEvent;

static string json_business_tmpl = "{\
                                    \"uid\": \"\", \
                                    \"opcode\": 0, \
                                    \"errcode\": 0, \
                                    \"input\": {},  \
                                    \"output\": {}, \
                                    \"msg\": \"\"  \
                                    }";

typedef enum
{
  EN_USER_DEVICE_UNKNOWN=0,
  EN_USER_DEVICE_DESKPHONE=1,
  EN_USER_DEVICE_IOS=2,
  EN_USER_DEVICE_ANDROID=3,
  EN_USER_DEVICE_WEB=4,
  EN_USER_DEVICE_WINDOWS=5,
  EN_USER_DEVICE_PSTN=6,
  EN_USER_DEVICE_CCAAS_WEB=7,
  EN_USER_DEVICE_TPI_WEB = 8,
} UserDevice;

enum CallCallingPresentation
{
    CCP_Allowed = 0,
    CCP_Restricted = 1,
    CCP_NotAvailable = 2,
};

typedef enum XGATEINTERFACETYPE
{
  EN_XGATE_INTERFACE_UNKNOWN,
  EN_XGATE_INTERFACE_PRIVATE,
  EN_XGATE_INTERFACE_PUBLIC,
}xGateInterfaceType;

class TrunkInfo
{
  public:
    TrunkInfo() = default;
    ~TrunkInfo() = default;
    TrunkInfo(const TrunkInfo &rhs)
    {
      m_iTrunk_id = rhs.m_iTrunk_id;
      isActive = rhs.isActive;
      m_usPort = rhs.m_usPort;
      m_sIpAddress = rhs.m_sIpAddress.c_str();
      m_sDcType = rhs.m_sDcType.c_str();
    };
    TrunkInfo &operator=(const TrunkInfo &rhs)
    {
      m_iTrunk_id = rhs.m_iTrunk_id;
      isActive = rhs.isActive;
      m_usPort = rhs.m_usPort;
      m_sIpAddress = rhs.m_sIpAddress.c_str();
      m_sDcType = rhs.m_sDcType.c_str();
      return *this;
    }

    int m_iTrunk_id;
    int isActive;
    unsigned short m_usPort = 5060;
    std::string m_sIpAddress;
    std::string m_sDcType;
};

#define XGUTIL XGUtil::instance()
class xGateUtil : public ACE_Task<ACE_MT_SYNCH> {
	public:  
		xGateUtil();
		~xGateUtil(void);
		bool post(xGateBaseMsg *pBaseMsg);

		static char m_localIp4Addr[INET_ADDRSTRLEN];
		static char m_localIp6Addr[INET6_ADDRSTRLEN];

		static char *LocalIPadress;
		static xGateRetVal findLocalIpAddr(char *ipAddr, unsigned short AddrType = AF_INET, xGateInterfaceType eInterfaceType = EN_XGATE_INTERFACE_PUBLIC);
		static char * getLocalIpAddr(unsigned short AddrType = AF_INET, xGateInterfaceType eInterfaceType = EN_XGATE_INTERFACE_PUBLIC);
		static void setLocalIpAddr(char *localIpAddr);
		static bool isPrivateIPv4(std::string ip);

		static  const char* GetLocalIPAddress()
		{
			//cout<<"LOCAL IPADRESS IS"<<LocalIPadress<<endl;
			return LocalIPadress;
		}
    std::string generateTransid();
    std::string UUIDToSessionId(const char *uuid, const size_t len);
    bool isValidSessionID(const char* uuid, const size_t len);
		//rapidjson utilily functions - start 
		bool parseJson(const char *jsonData, Document &doc);
		Value setJsonParam(const string &strValue, Document &doc);
		Value setJsonParam(unsigned int iValue);
		//Value setJsonParam(unsigned int iValue, Document &doc);
		//rapidjson utilily functions - end

};

struct  multiDevice {
  multiDevice():m_callId(""), m_userNumber(""), m_domainName(""), m_domainId(0), m_calledcodec(1),\
                m_userDeviceType(""), m_calledDeviceType(""),m_proxyUserName(""),m_userSignalAddr(""),\
                m_outInterface(""), m_routeIpAddress(""), m_CalledDevicePresenceStatus(0),\
		m_isCalledDeviceRegistered(false), m_isCalledDeviceInviteSend(false)  {};
  std::string m_callId; //i.e call_id created by sofia object
  std::string m_userNumber;
  std::string m_domainName;
  unsigned int m_domainId;
  int m_calledcodec;
  std::string m_userDeviceType;
  std::string m_calledDeviceType;
  std::string m_proxyUserName; //used by ACME SBC
  std::string m_userSignalAddr;  //immediate peer signalling addredd (i.e) MGC/SBC
  std::string m_outInterface;
  std::string m_routeIpAddress;
  int m_CalledDevicePresenceStatus;
  bool m_isCalledDeviceRegistered;
  bool m_isCalledDeviceInviteSend;
};

struct sessionDetail
{
  sessionDetail(string &uid, IURDefines::MODULE_ID srcModuleId) :
  m_uid(uid),m_srcModuleId(srcModuleId){};
  ~sessionDetail(){};
  IURDefines::MODULE_ID m_srcModuleId;
  std::string m_uid;
};

struct completedMsgBuf
{
  string msg; //one extracted complete msg
  unsigned int msgLen; //length of one extracted complete msg

  completedMsgBuf() : msg(""), msgLen(0){};
  ~completedMsgBuf() {
    msg.clear();
    msgLen = 0;
  };
  completedMsgBuf(const completedMsgBuf& rhs) {
    msg = rhs.msg;
    msgLen = rhs.msgLen;
  };
  completedMsgBuf& operator= (const completedMsgBuf& rhs) {
    msg = rhs.msg;
    msgLen = rhs.msgLen;
    return *this;
  };
};

typedef enum
{
  EN_XGATE_CALL_TYPE_UNKNOWN,
  EN_XGATE_CALL_TYPE_SIMPLE,
  EN_XGATE_CALL_TYPE_CONFERENCE,
  EN_XGATE_CALL_TYPE_SIMPLE_VIDEO,
  EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO,
  EN_XGATE_CALL_TYPE_VMS,
} xGateCallType;

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
  EN_XGATE_MG_MAX_TYPE = 30,
} xGateMgMsgType;

typedef enum
{
  EN_MEDIA_EVENT_NULL = 0, // Inital state no actin take on media state
  EN_MEDIA_EVENT_PLAY = 1,  // state at PLAY create RBT pipeline in Mg and play RBT
  EN_MEDIA_EVENT_STOP = 2,  // State at STOP if RBT pipeline is running stop the play
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
  EN_MEDIA_EVENT_RECORD_STOP = 22,
  EN_MEDIA_EVENT_RECORD_SUCCESS = 23,
  EN_MEDIA_EVENT_RECORD_FAILED = 24,
  EN_MEDIA_EVENT_RECORD_INFO = 25,
  EN_MEDIA_EVENT_RTP_TIMEOUT = 26,
  EN_MEDIA_EVENT_RECORD_PLAY = 27,
  EN_MEDIA_EVENT_RECORD_SAVE = 28,
  EN_MEDIA_EVENT_RECORD_DELETE = 29,
} xGateMediaEvent;

typedef enum
{
  EN_MEDIA_SDPINFO_UNKNOWN=0,
  EN_MEDIA_SDPINFO_AUDIO=2,
  EN_MEDIA_SDPINFO_VIDEO=3,
} xGateMediaSdpType;

typedef enum
{
  EN_CANDIDATE_TYPE_UNKNOWN=0,
  EN_CANDIDATE_TYPE_HOST=1,
  EN_CANDIDATE_TYPE_PRFLX=2, //peer reflexive candidate
  EN_CANDIDATE_TYPE_SRFLX=3, //server reflexive candidate
  EN_CANDIDATE_TYPE_RELAY=4, //relay candidate
} xGateCandidateType;

typedef enum XGATECALLFEATURE
{
EN_NULL_CALL=0,
EN_URCBCONF_CALL = 1,
EN_URPBX_CALL = 2,
}XGATECALLFEATURE;

class TcpConTuple
{
  public:
    int m_fd;
    string m_remoteIp;
    unsigned short m_remotePort;
    unsigned short m_localPort;
    unsigned short m_timeoutCount;
    long int m_timerId;

    TcpConTuple() :
      m_fd(-1), m_remoteIp(""), m_remotePort(0), m_localPort(0), 
      m_timerId(0), m_timeoutCount(0) {};

    ~TcpConTuple() {};

#if 0
    TcpConTuple& operator=(TcpConTuple &src)
    {
      m_fd = src.m_fd;
      m_remoteIp = src.m_remoteIp;
      m_remotePort = src.m_remotePort;
      m_localPort = src.m_localPort;
      m_timeoutCount = src.m_timeoutCount;
      m_timerId = src.m_timerId;
    }
#endif

    bool operator==(TcpConTuple &src) 
    {
      if(m_remoteIp == src.m_remoteIp && m_remotePort == src.m_remotePort \
          && m_fd == src.m_fd && m_localPort == src.m_localPort) {
        return true;
      } else {
        return false;
      }
    }
};

//class MBCLinkTcpSocketHandler;
class MediaTimerData
{
  public: 
    int m_fd;
    string m_ip;
    //MBCLinkTcpSocketHandler *m_pHandler;
    //ACE_SOCK_Stream m_newPeerStream;

    MediaTimerData() :
      m_fd(0), m_ip("") {}; 
    ~MediaTimerData() {};
    MediaTimerData(const MediaTimerData &src) 
    {
      m_fd = src.m_fd;
      m_ip = src.m_ip;
      //m_pHandler = src.m_pHandler;
      //m_newPeerStream = src.m_newPeerStream;
    };
    MediaTimerData& operator=(MediaTimerData &src)
    {
      m_fd = src.m_fd;
      m_ip = src.m_ip;
      //m_pHandler = src.m_pHandler;
      //m_newPeerStream = src.m_newPeerStream;
      return *this;
    };
};

class CandidateDetail
{
  public:
    CandidateDetail() : 
      m_priority(1), m_transportType(EN_XGATE_TRANSPORT_UDP), m_raddr(""),
      m_rport(0), m_candidateType(EN_CANDIDATE_TYPE_UNKNOWN) {};
    ~CandidateDetail() {};
    CandidateDetail& operator=(CandidateDetail &src)
    {
      m_priority = src.m_priority;
      m_transportType = src.m_transportType;
      m_raddr = src.m_raddr;
      m_rport = src.m_rport;
      m_candidateType = src.m_candidateType;
      return *this;
    };

    unsigned int m_priority;
    xGateTransportType m_transportType;
    string m_raddr; //remote ip address
    unsigned int m_rport; //remote port
    xGateCandidateType m_candidateType;
};

/*  MediaSecureDetail class - Start
*/
class MediaSecureDetail
{
  public:
    MediaSecureDetail() :
      m_pemFile(""), m_lIceUfrag(""), m_lIcePwd(""), m_rIceUfrag(""),
      m_rIcePwd(""), m_lFingerPrint(""), m_rFingerPrint(""),
      m_rSsrc(""), m_lSsrc(""), m_lCname(""), m_rCname(""),
      m_rMsLabel(""), m_lMsLabel(""), m_rLabel(""), m_lLabel("") {};
    ~MediaSecureDetail() {};
    //Assignment operator
    MediaSecureDetail& operator=(MediaSecureDetail &src)
    {
      m_candidateDetail = src.m_candidateDetail;
      m_pemFile = src.m_pemFile;
      m_lIceUfrag = src.m_lIceUfrag;
      m_lIcePwd = src.m_lIcePwd;
      m_rIceUfrag = src.m_rIceUfrag;
      m_rIcePwd = src.m_rIcePwd;
      m_lFingerPrint = src.m_lFingerPrint;
      m_rFingerPrint = src.m_rFingerPrint;
      m_lSsrc = src.m_lSsrc;
      m_rSsrc = src.m_rSsrc;
      m_lCname = src.m_lCname;
      m_rCname = src.m_rCname;
      m_lMsLabel = src.m_lMsLabel;
      m_rMsLabel = src.m_rMsLabel;
      m_lLabel = src.m_lLabel;
      m_rLabel = src.m_rLabel;
      return *this;
    };

    //public member properties
    CandidateDetail m_candidateDetail;
    string m_pemFile;
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

class AudioDetail {
  public:
    AudioDetail():
      m_mediaType(0), m_fmtp(""), m_codec(0), m_codecName(""),
      m_ptime(0), m_playFile(""), m_clientIp(""), m_clientPort(0),
      m_serverIp(""), m_serverPort(0), m_relayIp(""), m_relayPort(0),
      m_rflxIp(""), m_rflxPort(0), m_dialIp(""), m_dialPort(0),
      m_respIp(""), m_respPort(0){};
    ~AudioDetail(){};
    AudioDetail& operator=(AudioDetail &src)
    {
      m_mediaType = src.m_mediaType;
      m_fmtp = src.m_fmtp;
      m_codec = src.m_codec;
      m_codecName = src.m_codecName;
      m_ptime = src.m_ptime;
      m_playFile = src.m_playFile;
      m_clientIp = src.m_clientIp;
      m_clientPort = src.m_clientPort;
      m_serverIp = src.m_serverIp;
      m_serverPort = src.m_serverPort;
      m_relayIp = src.m_relayIp;
      m_relayPort = src.m_relayPort;
      m_rflxIp = src.m_rflxIp;
      m_rflxPort = src.m_rflxPort;
      m_secureDetail = src.m_secureDetail;
      m_dialIp = src.m_dialIp;
      m_dialPort = src.m_dialPort;
      m_respIp = src.m_respIp;
      m_respPort = src.m_respPort;
      return *this;
   };

    unsigned int m_mediaType;
    string m_fmtp;
    unsigned int m_codec;
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
};

class VideoDetail {
  public:
    VideoDetail():
      m_mediaType(0), m_fmtp(""), m_codec(0), m_codecName(""),
      m_ptime(0), m_playFile(""), m_clientIp(""), m_clientPort(0),
      m_serverIp(""), m_serverPort(0), m_relayIp(""), m_relayPort(0),
      m_rflxIp(""), m_rflxPort(0), m_dialIp(""), m_dialPort(0),
      m_respIp(""), m_respPort(0), m_frameRate(0), m_imageattr_x(0), 
			m_imageattr_y(0) {};
    ~VideoDetail() {};
    VideoDetail& operator=(VideoDetail &src)
    {
      m_mediaType = src.m_mediaType;
      m_fmtp = src.m_fmtp;
      m_codec = src.m_codec;
      m_codecName = src.m_codecName;
      m_ptime = src.m_ptime;
      m_playFile = src.m_playFile;
      m_clientIp = src.m_clientIp;
      m_clientPort = src.m_clientPort;
      m_serverIp = src.m_serverIp;
      m_serverPort = src.m_serverPort;
      m_relayIp = src.m_relayIp;
      m_relayPort = src.m_relayPort;
      m_rflxIp = src.m_rflxIp;
      m_rflxPort = src.m_rflxPort;
      m_secureDetail = src.m_secureDetail;
      m_dialIp = src.m_dialIp;
      m_dialPort = src.m_dialPort;
      m_respIp = src.m_respIp;
      m_respPort = src.m_respPort;
			m_frameRate = src.m_frameRate;
			m_imageattr_x = src.m_imageattr_x;
			m_imageattr_y = src.m_imageattr_y;
      return *this;
    };

    unsigned int m_mediaType;
    string m_fmtp;
    unsigned int m_codec;
    string m_codecName;
    unsigned int m_ptime;
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
};

class MgMediaDetail
{
  public:
    string mgresource_id;
    unsigned int mgmsg_type;
    string mg_ip;
    unsigned int call_type;
    string call_id; //this value of calltablekey generated in sofia.cpp
    unsigned int gateway_id;
    int context_id;
    unsigned int leg_id;
    string media_proto;
    string out_proto;
    string record_file;
    long int file_size;
    string dtmf_digits;
    unsigned int media_event;
    unsigned int joiner_type;
    string org_rtp_ep;
    string dst_rtp_ep;
    string sdp_str;
    unsigned int call_dir; //this value from CallDirection enum from call.h
    unsigned int sig_type; //this value from SignallingType enum from SignallingDriver.h
    AudioDetail audioDetail;
    VideoDetail videoDetail;

    MgMediaDetail() 
      : mgresource_id(""), mgmsg_type(0), mg_ip(""), call_type(0), 
      call_id(""), gateway_id(0), leg_id(0), context_id(0), 
      media_proto(""), out_proto(""), record_file(""), file_size(0), 
      dtmf_digits(""), media_event(0), joiner_type(0), call_dir(0), 
      sig_type(3), sdp_str(""), org_rtp_ep(""), dst_rtp_ep("") {};

    ~MgMediaDetail(){};

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
      context_id = src.context_id;
      media_proto = src.media_proto;
      out_proto = src.out_proto;
      record_file = src.record_file;
      file_size = src.file_size;
      dtmf_digits = src.dtmf_digits;
      media_event = src.media_event;
      joiner_type = src.joiner_type;
      call_dir  = src.call_dir;
      org_rtp_ep = src.org_rtp_ep;
      dst_rtp_ep = src.dst_rtp_ep;
      sdp_str = src.sdp_str;
      sig_type  = src.sig_type;
      audioDetail =  src.audioDetail;
      videoDetail = src.videoDetail;
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
  	}
  	return "";
  }
};
/*
class TcpJsonPendingMsg
{
  public:
    bool isPending; //pending msg is available or not
    string delimiter;
    string pendingMsg; //incomplete pending json msg in buffer
    unsigned int pendingMsgLen; //length of incomplete pending json msg in buffer
    completedMsgBuf completedMsg;
    TcpJsonPendingMsg();
    ~TcpJsonPendingMsg();
    xGateRetVal get_one_complete_msg();


};

TcpJsonPendingMsg::TcpJsonPendingMsg() : isPending(false), delimiter("\r\n\r\n"),
										  pendingMsg(""), pendingMsgLen(0), completedMsg{}
{
}

TcpJsonPendingMsg::~TcpJsonPendingMsg()
{
}

xGateRetVal TcpJsonPendingMsg::get_one_complete_msg()
{
  XGLOG_DEBUG("Enter TcpJsonPendingMsg::get_one_complete_msg");
  int retVal = EN_XGATE_STATUS_ERROR;

  //1. find out position of end of msg(i.e) "\r\n\r\n" from given pending buffer
  size_t delimiterPos = pendingMsg.find(delimiter);

  if(delimiterPos == string::npos) {
    XGLOG_ERROR("TcpJsonPendingMsg::get_one_complete_msg, one complete msg not available \
        in given pending buffer: %s\n", pendingMsg.c_str());
    //3rd scenario: not able to extract one complete msg, becoz partial msg in buffer
    //in 3rd scenario we will receive remaining msg in next socket read
    if((pendingMsg.length() > 0)) {
      isPending = true;
      pendingMsgLen = pendingMsg.length();
    }
    return EN_XGATE_STATUS_ERROR;
  } else {
    //2. get first complete msg
    completedMsg.msg = pendingMsg.substr(0, delimiterPos);
		char messageStr[30] = "\0";
		size_t tlen = completedMsg.msg.copy(messageStr,29,0);
    messageStr[tlen] = '\0';
		char *token;
		bool bRead = false;
		completedMsg.msgLen = 0;
		char *temp =  messageStr;
		 while(token = strtok_r(temp,",:",&temp )){
		 if(token && strstr(token , "msg_len")){
		  bRead =  true;
			continue;
			}
			if(bRead){
			completedMsg.msgLen =  atoi(token);  
			break;
			}
//			token =  (strtok(temp,NULL));
		 }

     if(completedMsg.msgLen == 0 ){
       XGLOG_ERROR("TcpJsonPendingMsg::get_one_complete_msg, 'msg_len' hearder field \
            not found in extracted Msg: %s\n", completedMsg.msg.c_str());
						return EN_XGATE_STATUS_ERROR;
     }
    //3. get msg length
#if 0				
    size_t pos = completedMsg.msg.find("msg_len");
    string resultStr = completedMsg.msg.substr(pos+8, 5);
    if(sscanf(resultStr.c_str(), "%*[:]%d", &completedMsg.msgLen) <= 0) {
      if(sscanf(resultStr.c_str(), "%d", &completedMsg.msgLen) <= 0) {
        XGLOG_ERROR("TcpJsonPendingMsg::get_one_complete_msg, 'msg_len' hearder field \
            not found in extracted Msg: %s\n", completedMsg.msg.c_str());
        return EN_XGATE_STATUS_ERROR;
      }
    }
#endif		
    size_t nextMsgStartPos = delimiterPos + delimiter.length();
    pendingMsg.erase(0, nextMsgStartPos);
#if 0		
    printf("\n*************************************\n");
    printf("extracted %d bytes of json msg: \n%s\n", completedMsg.msgLen, completedMsg.msg.c_str());
    printf("*************************************\n");
#endif		
    //1st scenario: extracted one complete msg and no more pending msg in buffer
    if((completedMsg.msgLen == (delimiterPos)) && (pendingMsg.length() <= 0)) {
      isPending = false;
      pendingMsgLen = 0;
      return EN_XGATE_STATUS_SUCCESS;
    }

    //2nd scenario: extracted one complete msg but still having pending msg in buffer
    if((completedMsg.msgLen == (delimiterPos)) && (pendingMsg.length() > 0)) {
      isPending = true;
      pendingMsgLen = pendingMsg.length();
      return EN_XGATE_STATUS_SUCCESS;
    }

    //3rd scenario: not able to extract one complete msg, becoz partial msg in buffer
    //in 3rd scenario we will receive remaining msg in next socket read
    if((pendingMsg.length() > 0)) {
      isPending = true;
      pendingMsgLen = pendingMsg.length();
      return EN_XGATE_STATUS_ERROR;
    }
  }

  return EN_XGATE_STATUS_ERROR;
}
*/
typedef ACE_Unmanaged_Singleton<xGateUtil, ACE_Null_Mutex> XGUtil;
#endif
