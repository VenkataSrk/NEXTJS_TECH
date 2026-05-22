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

//local includes

//#define XGATE_HAS_JSON_SUPPORT

#ifdef XGATE_HAS_JSON_SUPPORT
//rapidjson includes
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/ostreamwrapper.h"
#include "jsonlookaheadparser.h"
//#include "xGateMGConnector.h"
#include <iostream>


using namespace rapidjson;
#endif

//local includes
//#include "h248sig.h"
#include "xgconfig.h"

using namespace std;

#define SERVER_TYPE_SIZE 20
#define SERVER_ADDRESS_LENGTH 50
#define MGC_MG_SIGNAL_PORT 32779
#define XGATE_TCP_READ_SIZE  1500
#define SIP_SERVER_PORT 5060
#define ENCODE_BUF_SIZE 4*1024

#ifdef XGATE_HAS_JSON_SUPPORT
static const int jsonParseFlags = kParseDefaultFlags | kParseInsituFlag;
//mgc-mg json template messages
static const char json_mg_register_tmpl[] = " { \"msg_type\" : 0, \
                                             \"msg_len\" : 999, \
                                             \"data\" : { \
                                             \"gw_name\": \"\", \ 
                                             \"gw_profile\": \"\", \
                                               \"channels_count\" : 0, \
                                               \"rls_count\": 0, \
                                               \"rls_record\": \"\" } } ";

static const char json_mg_media_tmpl[] = " { \"msg_type\" : 0, \
                                          \"msg_len\" : 999, \
                                          \"data\" : { \
                                          \"call_id\": \"\", \
                                          \"gateway_id\" : 0, \
                                          \"context_id\" : 0, \
                                          \"ip_addr\": \"\", \
                                          \"ip_port\": 0, \
           				  \"relayip_addr\" : \"\",\
					  \"relay_port\" : 0,\
					  \"reflexip_addr\" : \"\",\
					  \"reflex_port\" : 0,\
                                          \"codec\": 0, \
					  \"ice_ufrag\" : \"\",\
					  \"ice_pwd\" : \"\",\
				          \"fingerprint\" : \"\",\
					  \"ptime\" : 20,\
					  \"media_proto\" : \"\",\
                                          \"out_proto\" : \"\",\
                                          \"sig_type\" : 0, \
                                          \"call_dir\" : 0, \
                                          \"rtp_endpoint\": \"\"} } ";

// Json message for RBT and play back files messge between MGC and Mg
static const char json_media_playback_tmpl[]= "{ \"msg_type\" :0,\
                                                \"msg_len\" : 999 , \
                                                \"data\" : { \
                                                \"call_id\": \"\", \
                                                \"rbtstatus\": 0,\}}";


#endif
//class xGateConnector;
class xGateTCPSocketHandler;
class xGateTCPReceiver;
class xGateMgProcessor;
class xGateMGDispatcher;
class xGateMGCConnector;
class xGateHmpProcessor;
class xGateHmpGstManager;
//class HmpGstPool;

// RBT vlaue for communication between MGC and MG
typedef enum
{
 EN_XGATE_NULL_RBT = 0, // Inital state no actin to take in MG
 EN_XGATE_PLAY_RBT,     // state at PLAY create RBT pipeline in Mg and play RBT
 EN_XGATE_STOP_RBT,     // State at STOP if RBT pipeline is running stop the play
 EN_XGATE_CLEAR_RBT     // State at CLEAR if RBT pipline is at PLAY or STOP state destory the pipeline
}xGateRbtState;


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
  EN_XGATE_MSG_PRIORITY_LOW,
  EN_XGATE_MSG_PRIORITY_HIGH,
  EN_XGATE_MSG_PRIORITY_UNKNOWN
}xGateMsgPriority;

typedef  enum H248MsgTypes {
  LOCAL_MEDIA_ALLOCATE_REQUEST = 2,
  LOCAL_MEDIA_ALLOCATE_RESPONSE,
  MEDIAGATEWAY_MEDIA_CREATE,
  MEDIAGATEWAY_MEDIA_CREATE_RESPONSE,
  REMOTE_MEDIA_ALLOCATE_REQUEST,
  REMOTE_MEDIA_ALLOCATE_RESPONSE,
  MEDIA_LISTEN_REQUEST,
  MEDIA_LISTEN_RESPONSE,
  MEDIA_RELEASE_REQUEST,
  MEDIA_RELEASE_RESPONSE,
  GATEWAY_REGISTRATION_REQUEST,
  GATEWAY_REGISTRATION_RESPONSE,
  RESOURCE_RECORD_REQUEST,
  RESOURCE_RECORD_RESPONSE,
  KEEP_ALIVE_CHECK, //SRI Added for Heart Beat
  NEW_START_UP  //Testing
} H248_MSG_TYPES;

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
}xGateMsgType;

#ifdef XGATE_HAS_MGC_MG_COMMUNICATION
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
}xGateMgMsgType;

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

struct MgMediaDetail
{
  string call_id; //this value of calltablekey generated in sofia.cpp
  int context_id;
  string ip_addr;
  unsigned int port;
  string relayip_addr;
  unsigned int relay_port;
  string reflexip_addr;
  unsigned int reflex_port;
  unsigned int codec;
  string ice_ufrag;
  string ice_pwd;
  string fingerprint;
  unsigned int ptime;
  string media_proto; 
  string out_proto;
  unsigned int call_dir; //this value from CallDirection enum from call.h
  unsigned int sig_type; //this value from SignallingType enum from SignallingDriver.h
  /* SwitchListen is merged into CalledParty Rtp Activate message, piggybacking
   * so no switchlisten message from MGC to MG explicitly */
  string rtp_ep;

  MgMediaDetail() 
  {
     call_id = "";
     context_id = 0;
     ip_addr ="";
     port = 0;
     relayip_addr = "";    
     relay_port = 0;
     reflexip_addr = "";
     reflex_port  = 0;
     codec = 0;
     ice_ufrag = "";
     ice_pwd = "";
     fingerprint = "";
     ptime = 0;
     media_proto= "";
     out_proto="";
     call_dir = 0;
     sig_type = 3;
     rtp_ep = "";  
  };

  ~MgMediaDetail()
  {
  }
  //Assignment operator
  MgMediaDetail& operator=(MgMediaDetail &src)
  {
    call_id   = src.call_id;
    context_id = src.context_id;
    ip_addr = src.ip_addr;
    port = src.port;
    relayip_addr = src.relayip_addr;
    relay_port = src.relay_port;
    reflexip_addr = src.reflexip_addr;
    reflex_port  = src.reflex_port;
    codec       = src.codec;
    ice_ufrag = src.ice_ufrag;
    ice_pwd = src.ice_pwd;
    fingerprint = src.fingerprint;
    ptime = src.ptime;
    media_proto= src.media_proto;
    out_proto = src.out_proto;
    call_dir  = src.call_dir;
    sig_type  = src.sig_type;
    rtp_ep = src.rtp_ep;
    return *this;
  }
};
#endif

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

class xGateUtil {
  public:
    static void test();
    //static int get_json_mg_msg_template(Document &, Writer<StringBuffer> &, H248_MSG_TYPES);
    static void addToMgConnectionMap(xGateTCPConTuple);
    static xGateRetVal getFromMgConnectionMap(string, unsigned short, int&);
    static void removeFromMgConnectionMap(xGateTCPConTuple);
    static bool isPresentInMgConnectionMap(string,unsigned short);
    static void getTimerIdFromConnMap (int fd, xGateTCPConnectionType eConnType, long int &timerId);
    
     static char *LocalIPadress;
    
     static void SetLocalIPAddress(void);
  
     static  const char* GetLocalIPAddress()
    {
        //cout<<"LOCAL IPADRESS IS"<<LocalIPadress<<endl;
                    return LocalIPadress;
    }
       
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
};


#if 0 
#include<iostream>
using namespace std;
struct MyHandler {
  bool Null() { cout << "Null()" << endl; return true; }
  bool Bool(bool b) { cout << "Bool(" << boolalpha << b << ")" << endl; return true; }
  bool Int(int i) { cout << "Int(" << i << ")" << endl; return true; }
  bool Uint(unsigned u) { cout << "Uint(" << u << ")" << endl; return true; }
  bool Int64(int64_t i) { cout << "Int64(" << i << ")" << endl; return true; }
  bool Uint64(uint64_t u) { cout << "Uint64(" << u << ")" << endl; return true; }
  bool Double(double d) { cout << "Double(" << d << ")" << endl; return true; }
  bool RawNumber(const char* str, SizeType length, bool copy) {
    cout << "Number(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
    return true;
  }
  bool String(const char* str, SizeType length, bool copy) {
    cout << "String(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
    return true;
  }
  bool StartObject() { cout << "StartObject()" << endl; return true; }
  bool Key(const char* str, SizeType length, bool copy) {
    cout << "Key(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
    return true;
  }
  bool EndObject(SizeType memberCount) { cout << "EndObject(" << memberCount << ")" << endl; return true; }
  bool StartArray() { cout << "StartArray()" << endl; return true; }
  bool EndArray(SizeType elementCount) { cout << "EndArray(" << elementCount << ")" << endl; return true; }
};
#endif
#endif
