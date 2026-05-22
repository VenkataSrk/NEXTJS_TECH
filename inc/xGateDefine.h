#ifndef _XGATE_DEFINE_H
#define _XGATE_DEFINE_H
#include <string>
#include "IURDefines.h"
using namespace std;

//ENUM definiton - Start
/// Enum for Return Code

//DebugVM start
#define SERVER_TYPE_SIZE 20
#define ENCODE_BUF_SIZE 4*1024
#define SERVER_ADDRESS_LENGTH 50

// Enum for TCP Connections Type
typedef enum
{
  EN_XGATE_UNKNOWN_CONNECTION,
  EN_XGATE_NW_CONNECTION,
  EN_XGATE_MG_CONNECTION,
}xGateTCPConnectionType;

//DebugVM end 

typedef enum
{
  EN_XGATE_STATUS_SUCCESS,
  EN_XGATE_STATUS_NOT_SUPPORTED,
  EN_XGATE_STATUS_STOPPING,
  EN_XGATE_STATUS_ACE_ERROR,
  EN_XGATE_STATUS_ENQUEUE_FAIL,
  EN_XGATE_STATUS_DEQUEUE_FAIL,
  EN_XGATE_STATUS_NO_CONNECTION,
  EN_XGATE_STATUS_NOT_PRESENT,
  EN_XGATE_STATUS_SDP_STARTED,
  EN_XGATE_STATUS_ERROR
} xGateRetVal;

typedef enum
{
  EN_XGATE_TRANSPORT_UNKNOWN,
  EN_XGATE_TRANSPORT_UDP,
  EN_XGATE_TRANSPORT_TCP,
  EN_XGATE_TRANSPORT_TLS,
} xGateTransportType;

typedef enum 
{
  EN_CONTROLLER_EVENT_UNKNOWN = 0,// not using
  EN_CONTROLLER_EVENT_TEST1 = 1,
  EN_CONTROLLER_EVENT_TEST2 = 2,
  EN_CONTROLLER_EVENT_TEST3 = 3,
} ControllerEvent;

typedef enum 
{  EN_SIP_EVENT_UNKNOWN = 0,// not using
  EN_SIP_EVENT_IVR_CALL_INIT = 1,
  EN_SIP_EVENT_IVR_CALL_DEINIT = 2,
} SipServiceEvent;

class TcpLisentAddr
{
  public:
    string m_ip;
    unsigned short m_port;
    TcpLisentAddr(): m_ip(""), m_port(0) {};
    ~TcpLisentAddr() {};
};

class xGateNetConnection
{
  public:
    string m_recvIp;
    unsigned short  m_recvPort;
    xGateTransportType m_protocol;
    int m_fd;
    unsigned short m_srcPort;

    xGateNetConnection() :
      m_recvIp(""), m_recvPort(5080), 
      m_protocol(EN_XGATE_TRANSPORT_UDP),
      m_fd(0), m_srcPort(0) {};

    ~xGateNetConnection() {};

    xGateNetConnection& operator=(xGateNetConnection &src)
    {
      m_recvIp = src.m_recvIp;
      m_recvPort = src.m_recvPort;
      m_protocol = src.m_protocol;
      m_fd = src.m_fd;
      m_srcPort = src.m_srcPort;
    }
};

struct xGateTimerData
{
  xGateTimerData(std::string &uid, unsigned int type, int retryCount, IURDefines::MODULE_ID requestorModule, void * ctxData = NULL) :
    m_uid(uid), m_timerType(type), m_retryCount(retryCount), m_requestorModule(requestorModule), m_pCtxdata(ctxData){};
  ~xGateTimerData(){};
  std::string m_uid;
  unsigned int m_timerType;
  int m_retryCount;
  IURDefines::MODULE_ID m_requestorModule; 
  void* m_pCtxdata;
};

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
  EN_XGATE_MSG_SIP_MODULE,
  EN_XGATE_MSG_IVR_MODULE,
  EN_XGATE_MSG_SERVICE_CONTROLLER,
 
  EN_XGATE_MSG_MBC_CONTROLLER_MESSAGE, // controller messages
  EN_XGATE_MSG_MBC_SUBSCRIBE_NOTIFY,// message as consumer
  EN_XGATE_MSG_MBC_PUBLISH_MESSAGE, // messages to the nextwork
  EN_XGATE_MSG_FROM_MBC,
  EN_XGATE_MSG_TIMER,
  EN_XGATE_MSG_REG_MODULE,
  EN_XGATE_MSG_REDIS_SUBSCRIBE_NOTIFY,// redis message for subscriber
  EN_XGATE_MSG_REDIS_CONN_FAILED_NOTIFY,
  EN_XGATE_MSG_REDIS_PUBLISH,
  EN_XGATE_MSG_REDIS_GET,
  EN_XGATE_MSG_REDIS_GET_RES,
  EN_XGATE_MSG_REDIS_SET,

}xGateMsgType;

typedef enum
{
  EN_XGATE_MSG_PRIORITY_LOW,
  EN_XGATE_MSG_PRIORITY_HIGH,
  EN_XGATE_MSG_PRIORITY_UNKNOWN
}xGateMsgPriority;
#endif
