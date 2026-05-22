#ifndef _XGATE_SFUTIL_H
#define _XGATE_SFUTIL_H

//system includes
#include <string>
#include <map>
#include <vector>

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
#include "SFUClient.h"
#include "SFUController.h"
using namespace std;

#define SERVER_TYPE_SIZE 20
#define SERVER_ADDRESS_LENGTH 50
#define ENCODE_BUF_SIZE 4*1024
#define RTP_STREAM_ID 12
#define REPAIR_RTP_STREAM_ID 13


class xGateSFUtil {
  public:

    /// Method to set and get local ip4/ip6 addrs
    static xGateRetVal findLocalIpAddr(char *ipAddr, unsigned short AddrType = AF_INET);
    static char * getLocalIpAddr(unsigned short AddrType = AF_INET);
    static void setLocalIpAddr(char *localIpAddr);
    static void makeDir(const char *dir);
    static string generate_random_string(int length);
    static string generate_callId ();
    /// Method to set the Mg Receiver
#if 0
    /// Method to access the Srvr Processor
    static MBCControllerProcessor* getMBCctlProcessor(void)
    {
      return  m_mbcCtrlProcessor;
    }
#endif
    static void setSFUId(int sfu_id){
      m_sfu_id = sfu_id; 	    
    }

    static int getSFUId(void){
       return m_sfu_id;	    
    }	    
#if 0
    /// Method to set the Srvr Processor
    static void setMBCctlProcessor(MBCControllerProcessor* mbcProcessor)
    {
      m_mbcCtrlProcessor = mbcProcessor;
    }
#endif
    /// Method to access the Hmp Processor
    static MBCSForwarderController* getMbcSfuCtl(void)
    {
      return  m_mbcSFctrl;
    }

    /// Method to set the Hmp Processor
    static void setMbcSfuCtl(MBCSForwarderController* mbcSFctrl)
    {
      m_mbcSFctrl = mbcSFctrl;
    }
#if 0
    /// Method to access the Hmp Processor
    static MBCSessionHandler* getMbcSessionHandler(void)
    {
      return  m_mbcSessionHandler;
    }

    /// Method to set the Hmp Processor
    static void setMbcSessionHandler(MBCSessionHandler* mbcSessionHandler)
    {
      m_mbcSessionHandler = mbcSessionHandler;
    }
#endif
    /// Method to access the SFUController
    static SFUController* getSFUController(void)
    {
      return m_sfuController;
    }

    /// Method to set the SFUController
    static void setSFUController(SFUController* sfuController)
    {
      m_sfuController = sfuController;
     }

    /// Method to access the SFUBridgeProcessor
    static SFUBridgeProcessor* getSFUBridgeProcessor(void)
    {
      return m_sfuBridgeProcessor;
    }

    /// Method to set the SFUBridgeProcessor
    static void setSFUBridgeProcessor(SFUBridgeProcessor* sfuBridgeProcessor)
    {
      m_sfuBridgeProcessor = sfuBridgeProcessor;
    }

    static SFUClient* getSFUClient(void)
    {
      return m_sfuClient;	     
    }
    
    static void setSFUClient(SFUClient *sfuClient)
    {
       m_sfuClient = sfuClient;
    }

  private:
    xGateSFUtil();
    xGateSFUtil(const xGateSFUtil& rhs);
    xGateSFUtil& operator= (const xGateSFUtil& rhs);

    /// Store the Connector

    /// Store the TCP Receiver

    /// Store the MBC Controller Processor
    //static MBCControllerProcessor* m_mbcCtrlProcessor;


    /// Store the MBC Forwarder Controller
    static MBCSForwarderController* m_mbcSFctrl;

    /// Store the MBC Session Handler
    //static MBCSessionHandler* m_mbcSessionHandler;

    /// Store the SFU Controller handle
    static SFUController* m_sfuController;
    
    /// Store the SFU Bridge Processor handle
    static SFUBridgeProcessor* m_sfuBridgeProcessor;

    static SFUClient* m_sfuClient;
    
    static int m_sfu_id;

    /// Store the Hmp Processor

    /// Store the Hmp Gstreamer Manager
    
  public:
    static vector<string> m_MgcIpAddressList;
    static char m_localIp4Addr[INET_ADDRSTRLEN];
    static char m_localIp6Addr[INET6_ADDRSTRLEN];
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
