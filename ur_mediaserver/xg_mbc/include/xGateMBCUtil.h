#ifndef _XGATE_MBCUTIL_H
#define _XGATE_MBCUTIL_H

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
#include "xGateUtil.h"
#include "xglog.h"
#include "xGateMBCServiceDefine.h"
#include "MBCControllerProcessor.h"
#include "ClientService.h"

using namespace std;

#define SERVER_TYPE_SIZE 20
#define SERVER_ADDRESS_LENGTH 50
#define MGC_MG_SIGNAL_PORT 32779
#define XGATE_TCP_READ_SIZE  2048
#define SIP_SERVER_PORT 5060
#define ENCODE_BUF_SIZE 4*1024
#define MAX_NUM_SFU 10000 

class xGateMBCUtil {
  public:

    static void addToMgConnectionMap(xGateTCPConTuple);
    static xGateRetVal getFromMgConnectionMap(string, unsigned short, int&);
    static void removeFromMgConnectionMap(xGateTCPConTuple);
    static bool isPresentInMgConnectionMap(string,unsigned short);
    static void getTimerIdFromConnMap (int fd, xGateTCPConnectionType eConnType, long int &timerId);

    /// Method to set and get local ip4/ip6 addrs
    static xGateRetVal findLocalIpAddr(char *ipAddr, unsigned short AddrType = AF_INET);
    static char * getLocalIpAddr(unsigned short AddrType = AF_INET);
    static void setLocalIpAddr(char *localIpAddr);
    static void makeDir(const char *dir);

    static void addToMgSfuConnectionMap(int sfu_id, pid_t pid);
    static int getPidFromMgSfuConnectionMap(int sfu_id, pid_t& pid);
    static int removePidFromMgSfuConnectionMap(int sfu_id);
    static int removeSfuIdFromResIdMap(int sfu_id);
    static int removeResIdFromSfuIdMap(int sfu_id);
    static void removeMgSfuSocketMap(int sfu_id);
    static xGateRetVal getFromMgSfuConnectionMap(string, unsigned short, int&);
    static bool isPresentInMgSfuConnectionMap(string,unsigned short);

    //Functions to handle incoming MBC message
	static xGateRetVal handle_media_req(Value& dataMemObj, MgMediaDetail& clientMediaDetail);
    static xGateRetVal handle_sdpinfo_process(const Value& sdpObj, MgMediaDetail& clientMediaDetail);
    static xGateRetVal handle_sdpinfo_audio(const Value& sdpObj, MgMediaDetail& clientMediaDetail);
    static xGateRetVal handle_sdpinfo_video(const Value& sdpObj, MgMediaDetail& clientMediaDetail);

    static int create_media_msg(MgMediaDetail& clientMediaDetail, string &jsonBuff, xGateMgMsgType mgMsgType);
    static Value setJsonParam(string strValue,Document &doc);
    static Value setJsonParam(unsigned int iValue,Document &doc);
    static Value setJsonParam(MgMediaDetail& media, Document &doc);
	
    /// Method to access the Srvr Processor
    static MBCControllerProcessor* getMBCctlProcessor(void)
    {
      return  m_mbcCtrlProcessor;
    }

    /// Method to set the Srvr Processor
    static void setMBCctlProcessor(MBCControllerProcessor* mbcProcessor)
    {
      m_mbcCtrlProcessor = mbcProcessor;
    }
    // Method to access the Client Service
    static ClientService* getMBCClientService(void)
    {
      return  m_mbcClientService;
    }

    /// Method to get the Client Service
    static void setMBCClientService(ClientService* mbcClientService)
    {
      m_mbcClientService = mbcClientService;
    }

    /// Method to access the SFUId which is unique

    static int getSfuId(int &sfuId);
    static void incrSfuId(void);

    static void addToMgSfuResMap(string resId, int sfu_id);
    static void addToMgResSfuMap(int sfu_id,string resId);
    static int getsfuIdFromResIdMap(string resId);
    static bool getResIdFromSfuIdMap(int sfuId, string &uid);

    static void addToMbcConnectionMap(int conn_id, MBCTcpConnInfo mbcTcpConnInfo);
    static void getConnInfoFromMbcConnectionMap(int conn_id, MBCTcpConnInfo &mbcTcpConnInfo);

    static void insert_sfuId_pbxConnId_map(int sfuId,int pbxConnId);
    static void get_pbxConnId_from_sfuId(int sfuId,int &pbxConnId);
    static void removeSfuIdFromPbxConnIdMap(int sfu_id);

    static void insert_sfuId_sfuConnId_map(int sfuId,int sfuConnId);
    static void get_sfuConnId_from_sfuId(int sfuId,int &sfuConnId);
    static void get_sfuId_from_sfuConnId(int sfuConnId,int &sfuId);
    static void removeSfuIdFromSfuConnIdMap(int sfu_id);

    static void insert_sfuConnId_pbxConnId_map(int sfuConnId,int pbxConnId);
    static void get_pbxConnId_from_sfuConnId(int sfuConnId,int &pbxConnId);
    static void removeSfuConnIdFromPbxConnIdMap(int sfu_conn_id);

    static void insert_sfuId_alloc_msg_map(int sfuId,int msgLen,char* msg);
    static void get_alloc_msg_from_sfuId(int sfuId,pair<int, char*> & allocMsgPair);
    static void remove_alloc_msg_from_map(int sfuId);

  private:
    xGateMBCUtil();
    xGateMBCUtil(const xGateMBCUtil& rhs);
    xGateMBCUtil& operator= (const xGateMBCUtil& rhs);
    static vector<xGateTCPConTuple> m_mgConnectionMap;
    static map<int, pid_t> m_mgSfuConnectionMap;
    static map<string, int>m_mgSfuIdResIdMap;
    static map<int, string>m_mgResIdSfuIdMap;
    //static map<int, ACE_SOCK_Stream>m_mgSfuIdSocketIdMap;
    static map<int, MBCTcpConnInfo> m_mbcConnectionMap;
    static ACE_Thread_Mutex m_mgConnectionMapLock;
    static ACE_Thread_Mutex m_mgSfuConnectionMapLock;
    static ACE_Thread_Mutex m_mgSfuIdLock;
    static ACE_Thread_Mutex m_mgSfuIdResIdLock;
    static ACE_Thread_Mutex m_mgResIdSfuIdLock;
    static ACE_Thread_Mutex m_mbcConnectionMapLock;


    static map<int, int> m_sfuIdPbxConnIdMap;
    static map<int, int> m_sfuIdSfuConnIdMap;
    static map<int, int> m_sfuConnIdPbxConnIdMap;
    static map<int,pair<int, char*>> m_sfuIdAllocMsgMap; 
    static ACE_Thread_Mutex m_sfuIdPbxConnIdMapLock;
    static ACE_Thread_Mutex m_sfuIdSfuConnIdMapLock;
    static ACE_Thread_Mutex m_sfuConnIdPbxConnIdMapLock;
    static ACE_Thread_Mutex m_allocMsgMapLock;

    /// Store the MBC Controller Processor
    static MBCControllerProcessor* m_mbcCtrlProcessor;

    /// Store the MBC Client Service handle
    static ClientService* m_mbcClientService;
  public:
    static char m_localIp4Addr[INET_ADDRSTRLEN];
    static char m_localIp6Addr[INET6_ADDRSTRLEN];
    static unsigned int m_sfuId;
    static vector<MBCSfuMsg *> m_mbcSfuMsgVect;
};

#endif
