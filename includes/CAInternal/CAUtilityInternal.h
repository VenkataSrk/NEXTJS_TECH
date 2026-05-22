#ifndef CA_UTILITYINTERNAL
#define CA_UTILITYINTERNAL
#pragma once
#include "CAThread.h"
#include "CAThreadPoolManager.h"
#include "CASocket.h" // for ace socket implemetation
#include "CAString.h"
#include "RapidJSON.h"
#include "RapidXML.h"
#include "CAFile.h"
#include "CAHash.h"
#include "CAServerSocket.h" // properatiory socket server implemetation
#include "CADBDriver.h"

#define SET(type, vartype, varname) void set##varname(type varname) {m_##vartype##varname = varname;}
#define GET(vartype, varname) auto get##varname() {return m_##vartype##varname;}
#define GETSTR(vartype, varname) STR get##varname() {return m_##vartype##varname.c_str();}

#define SOC_SERVER CA::EN_CA_SOCKET_TYPE::EN_CA_SOCKET_TYPE_SERVER
#define SOC_CLIENT CA::EN_CA_SOCKET_TYPE::EN_CA_SOCKET_TYPE_CLIENT
#define SOC_TCP CA::EN_CA_SOCKET_PROTOCOL::EN_CA_SOCKET_PROTOCOL_TCP

#define LOCAL_IPV4_ADDR GetLocalIPAddress(AF_INET)
#define LOCAL_IPV6_ADDR GetLocalIPAddress(AF_INET6)
// this define for business server applications
#define BOSTR(x) private:\
std::string m_##x ;\
public:\
void set##x(std::string val){ m_##x=val;}\
std::string get##x(){return m_##x;}

#define BOINT(x) private:\
int m_##x ;\
public:\
void set##x(int val){ m_##x=val;}\
int get##x(){return m_##x;}

#define BOFLOAT(x) private:\
float m_##x ;\
public:\
void set##x(float val){ m_##x=val;}\
float get##x(){return m_##x;}

#define BODOUBLE(x) private:\
double m_##x ;\
public:\
void set##x(double val){ m_##x=val;}\
double get##x(){return m_##x;}

#define BOLONG(x) private:\
long m_##x ;\
public:\
void set##x(long val){ m_##x=val;}\
long get##x(){return m_##x;}

enum class EN_CA_MEDIA_TYPE
{
   EN_CA_MEDIA_TYPE_UNKNOWN,
   EN_CA_MEDIA_TYPE_AUDIO = 2
};

enum class EN_CA_MEDIA_MODE
{
   EN_CA_MEDIA_MODE_INACTIVE = 0,
   EN_CA_MEDIA_MODE_SENDONLY,
   EN_CA_MEDIA_MODE_RECVONLY,
   EN_CA_MEDIA_MODE_SENDRECV
};

enum class EN_CA_CALL_TYPE
{
   EN_CA_CALL_TYPE_NULL = 0,
   EN_CA_CALL_TYPE_SIMPLE
};

enum class EN_BS_REPORT_TYPE
{
   EN_BS_REPORT_TYPE_NULL=0,
   EN_BS_REPORT_TYPE_CDR,
   EN_BS_REPORT_TYPE_CDS,
   EN_BS_REPORT_TYPE_CRR,
   EN_BS_REPORT_TYPE_DAT,
};

enum class EN_CA_CALL_DIRECTION
{
   EN_CA_CALL_DIRECTION_UNKNOWN = 0,
   EN_CA_CALL_DIRECTION_INBOUND,
   EN_CA_CALL_DIRECTION_OUTBOUND
};

enum class EN_CA_CALL_STATE
{
   EN_CA_CALL_STATE_NULL = 0,
   EN_CA_CALL_STATE_INIT,
   EN_CA_CALL_STATE_PROCESSING,
   EN_CA_CALL_STATE_PROCEEDING,
   EN_CA_CALL_STATE_ALERTING,
   EN_CA_CALL_STATE_CONNECTED,
   EN_CA_CALL_STATE_SUSPENDED,
   EN_CA_CALL_STATE_RESUMED,
   EN_CA_CALL_STATE_DISCONNECTED,
   EN_CA_CALL_STATE_TERMINATE,  // state from CAR/CAD to CAP for abnormal disconnection
   EN_CA_CALL_STATE_BUSINESS_PROCESS_SUCCESS,
   EN_CA_CALL_STATE_BUSINESS_PROCESS_FAILED,
   EN_CA_CALL_STATE_INBOUND_MEDIA_PROCESS_SUCCESS,
   EN_CA_CALL_STATE_INBOUND_MEDIA_PROCESS_FAILED,
   EN_CA_CALL_STATE_OUTBOUND_MEDIA_PROCESS_SUCCESS,
   EN_CA_CALL_STATE_OUTBOUND_MEDIA_PROCESS_FAILED,
   EN_CA_CALL_STATE_DIALOUT,
   EN_CA_CALL_STATE_RELEASE, // state from CAP to CAR/CAD for abnormal disconnection
   
};

enum class EN_CA_BUSINESS_STATE
{
   EN_CA_BUSINESS_STATE_NULL = 0,
   EN_CA_BUSINESS_STATE_INITIATE, //CA->BRS
   EN_CA_BUSINESS_STATE_PROCESS_SUCCESS, //BRS->CA
   EN_CA_BUSINESS_STATE_PROCESS_FAILED, //BRS->CA
   EN_CA_BUSINESS_STATE_PROCEEDING, //CA->BRS
   EN_CA_BUSINESS_STATE_ALERTING, //CA->BRS
   EN_CA_BUSINESS_STATE_CONNECTED, //CA->BRS
   EN_CA_BUSINESS_STATE_DISCONNECTED, //CA->BRS
   EN_CA_BUSINESS_STATE_CLEAR, //CA->BRS
   EN_CA_BUSINESS_STATE_RELEASE, //BRS->CA
   EN_CA_BUSINESS_STATE_FAILED, //CA->BRS
   EN_CA_BUSINESS_STATE_PLAY,
   EN_CA_BUSINESS_STATE_PLAY_N_CONNECT,
   EN_CA_BUSINESS_STATE_PLAY_N_DISCONNECT,
   EN_CA_BUSINESS_STATE_SERVICE_UNAVAILABLE, //BRS->CA
   EN_CA_BUSINESS_STATE_SERVICE_DOWN, //BRS->CA
   EN_CA_BUSINESS_STATE_ROUTE_ERROR, //BRS->CA				      
};

enum class EN_CA_CALL_CAUSE_CODE
{
   EN_NORMAL_DISCONNECT =  200,
  EN_BAD_REQUEST = 400,
  EN_UNAUTHORIZED = 401,
  EN_PAYMENT_REQUIRED =  402,
  EN_FORBIDDEN=403,
  EN_NOT_FOUND = 404,
  EN_REQUEST_TIMEOUT =  408,
  EN_GONE =  410, // Number changed
  EN_TEMPORARILY_UNAVAIABLE =  480,
  EN_TOO_MANY_HOPS = 483,
  EN__ADDRESS_INCOMPLET = 484,
  EN_BUSY_HERE =  486,
  EN_REQUESTED_TIMEOUT= 487,
  EN_INTERNAL_SERVER_ERROR = 500,
  EN_NOT_IMPLEMETED =  501,
  EN_BAD_GATEWAY =  502,
  EN_SERVICE_UNAVAILABLE =  503,
  EN_SERVER_TIMEOUT = 504,
  EN_DECLINE = 603,
  EN_CANCEL = 700,
  
};

enum class EN_CA_MEDIA_STATE
{
   EN_CA_MEDIA_STATE_NULL = 0,
   EN_CA_MEDIA_STATE_INBOUND_CREATE,
   EN_CA_MEDIA_STATE_OUTBOUND_CREATE,
   EN_CA_MEDIA_STATE_CONNECT,
   EN_CA_MEDIA_STATE_DISCONNECT,
   EN_CA_MEDIA_STATE_SUSPEND = 5,
   EN_CA_MEDIA_STATE_RESUME = 6,
   EN_CA_MEDIA_STATE_INBOUND_PLAY_IVR,
   EN_CA_MEDIA_STATE_SERVICE_UNAVAILABLE,
};

enum class EN_CA_MEDIA_PB_STATE
{
   EN_MEDIA_EVENT_PB_NULL = 0,
   EN_MEDIA_EVENT_PB_PLAY = 1,        // state at PLAY create RBT pipeline in Mg and play RBT
   EN_MEDIA_EVENT_PB_STOP = 2,        // State at STOP if RBT pipeline is running stop the play
   EN_MEDIA_EVENT_PB_EOS = 3,
   EN_MEDIA_EVENT_PB_REPEAT = 4,
   EN_MEDIA_EVENT_PB_HOLD = 5,
   EN_MEDIA_EVENT_PB_RESUME = 6,
   EN_MEDIA_EVENT_PB_GETDIGIT = 7,   // Request for start to collect the DTMF Digit in events
   EN_MEDIA_EVENT_PB_SENDDIGIT = 8,  // Request for send the Collected DTMF Digits in events.
   EN_MEDIA_EVENT_PB_RECVDIGIT = 9,    // Posting the Collected DTMF Digits in events.
   EN_MEDIA_EVENT_PB_PLAY_LIST = 10,
   EN_MEDIA_EVENT_PB_EOL = 11, //MG will send end of list once all the given play files in list played
   EN_MEDIA_EVENT_PB_TRANSFER = 16,
   EN_MEDIA_EVENT_PB_ATTEND_TRANSFER = 17,
   EN_MEDIA_EVENT_PB_FLIP = 18,
   EN_MEDIA_EVENT_RECORD_PORT = 19,
   EN_MEDIA_EVENT_RECORD_BIND =20,
   EN_MEDIA_EVENT_RECORD_START=21,
   EN_MEDIA_EVENT_RECORD_STOP=22,
   EN_MEDIA_EVENT_PB_RECORD_SUCCESS = 23,
   EN_MEDIA_EVENT_PB_RECORD_FAIL = 24,
   EN_MEDIA_EVENT_PB_RECORD_INFO = 25,
   EN_MEDIA_EVENT_RECORD_PAUSE = 29,
   EN_MEDIA_EVENT_RECORD_RESUME = 30,
   EN_MEDIA_EVENT_BROADCAST_HOST_JOIN = 31,
   EN_MEDIA_EVENT_RECORD_HOLD = 32,
   EN_MEDIA_EVENT_RECORD_UNHOLD = 33,
   EN_MEDIA_EVENT_PLAY_REC_IVR = 34,
   EN_MEDIA_EVENT_PB_IVR_LIST_REPEAT = 35,
   EN_MEDIA_EVENT_PLAY_IVR = 36,
   EN_MEDIA_EVENT_PLAY_IVR_REPEAT = 37,
   EN_MEDIA_EVENT_PLAY_IVR_STOP = 38,
   EN_MEDIA_EVENT_PB_CALL_PARK = 39,
};

enum class EN_CA_MEDIA_MSG_TYPE
{
  EN_CA_MEDIA_MSG_TYPE_UNKNOWN=0,
  EN_CA_MEDIA_MSG_TYPE_REGISTER = 1,
  EN_CA_MEDIA_MSG_TYPE_ALLOCATE_ORGINATE =2,
  EN_CA_MEDIA_MSG_TYPE_ALLOCATE_TERMINATE=3,
  EN_CA_MEDIA_MSG_TYPE_RELEASE =4,
  EN_CA_MEDIA_MSG_TYPE_18X_RESPONSE=5,
  EN_CA_MEDIA_MSG_TYPE_PLAY_BACK=6,
  EN_CA_MEDIA_MSG_TYPE_MEDIA_CREATE=7,
  EN_CA_MEDIA_MSG_TYPE_HOLD_RESUME=8
};

enum class EN_CA_MODULE_ID
{
   EN_CA_MODULE_UNKNOWN,
   EN_CA_MODULE_MEDIA,
   EN_CA_MODULE_BUSINESS,
   EN_CA_MODULE_DB,
   EN_CA_MODULE_MONITORING,
};

enum class EN_CA_DB_EVENT
{
   EN_CA_DB_EVENT_UNKNOWN,
   EN_CA_DB_EVENT_INSERT,
   EN_CA_DB_EVENT_GET,
   EN_CA_DB_EVENT_UPDATE,
   EN_CA_DB_EVENT_DELETE,
   EN_CA_DB_EVENT_DROP_TABLE
};

enum class EN_BS_DB_EVENT
{
   EN_BS_DB_EVENT_UNKNOWN,
   EN_BS_DB_EVENT_INSERT,
   EN_BS_DB_EVENT_GET,
   EN_BS_DB_EVENT_UPDATE,
   EN_BS_DB_EVENT_DELETE,
   EN_BS_DB_EVENT_DROP_TABLE
};


enum class EN_CA_MONITOR_STATE
{
   EN_CA_MONITOR_STATE_NULL = 0,
   EN_CA_MONITOR_STATE_PRIMARY,     // primary monitor service
   EN_CA_MONITOR_STATE_BACKUP,      // secondary/backup monitor service
};

enum class EN_CA_MONITOR_SERVER_STATE
{
   EN_CA_MONITOR_SERVER_STATE_NULL = 0,
   EN_CA_MONITOR_SERVER_STATE_ACTIVE,                 // Active Monitoring server
   EN_CA_MONITOR_SERVER_STATE_STANDBY,                // Standby Monitoring server
   EN_CA_MONITOR_SERVER_STATE_SERVICE_UNAVAILABLE,    // No Active monitor server
};

enum class EN_CA_MONITOR_EVENT
{
   EN_CA_MONITOR_EVENT_UNKNOWN,
   EN_CA_MONITOR_EVENT_CALLAGENT_DATA,    // CallAgent server sends data to Monitoring server 
   EN_CA_MONITOR_EVENT_BUSINESS_DATA,     // Business server sends data to Monitoring server
   EN_CA_MONITOR_EVENT_MEDIA_DATA,        // Media GW sends data to Monitoring server
   EN_CA_MONITOR_EVENT_CMD,               // Monitoring server sends remote command to perform CRUD opearations on config
   EN_CA_MONITOR_EVENT_ALERT,             // CallAgent, Business, Media GW send alarms as text to Monitoring server
   EN_CA_MONITOR_EVENT_TRACE,
   EN_CA_MONITOR_EVENT_UNAVAILABLE, 
};

enum class EN_CA_SERVER_TYPE
{
   EN_CA_SERVER_TYPE_UNKNOWN,
   EN_CA_SERVER_TYPE_CALLAGENT,           // Call Agent Server
   EN_CA_SERVER_TYPE_BUSINESS,            // Business Server
   EN_CA_SERVER_TYPE_MEDIA,               // Media Server
};

typedef std::map<STR, EN_CA_SERVER_TYPE> SERVER_TYPE_MAP;
typedef std::map<STR, EN_CA_MONITOR_SERVER_STATE> MONITOR_SERVER_STATE_MAP;

enum 
MESSAGEEVENT{ // message event  between module and componenets
MSG_ERROR = 0,
MSG_INIT, // message from main module to all service to initiate config and other initializing activity
MSG_CONFUPDATE, // message to all service for configuration update
MSG_DBWAITTIMEOUT, 
MSG_SHUTDOWN, // message from main or monitoring module to stop the service.
MSG_LOG, // message from all thread or module to Logger Thread
MSG_TIMER, // message from timer threads to Process Thread on elapaed time
MSG_UAS2CONF, // message from user agent service to configuration
MSG_UAS2CAR, //message from user Agent servevice to call agent receiver
MSG_UAS2CAD, // message from user agent service to call agent dispactcher
MSG_UAS2EXP, // message from uaser agent service to expection handler
MSG_CONF2UAS, // message from configuration to user agent service
MSG_CONF2CAR, // message from configuration to call agent receiver
MSG_CONF2CAD, // message from configuration to call agent dispatcher
MSG_CONF2MED, //message from configuration to media client
MSG_CONF2DB, // message from configuration to DB handler
MSG_CONF2BC, //message from config interface to business client
MSG_CONF2MON, //message from config interface to monitor client
MSG_CONF2EXP, // message from configuration to expection handler
MSG_CAP2CAR, // message from sip call manager to call agent receiver
MSG_CAP2CAD, // message  from sip call manager to call agent dispatcher
MSG_CAP2MED, // message from sip call manager to media client
MSG_CAP2MON, // message from sip call manager to monitoring client
MSG_CAP2DB, // message from sip call manager to DB handler,
MSG_CAP2BC, // message from sip call manager to business client
MSG_CAP2EXP, // message from sip call manager to Expection handler
MSG_CAR2CAP, // message from call agent receiver to sip call manager(sip processor)
MSG_CAR2CONF, //message from call agent recervier to configuration 
MSG_CAR2EXP, // message from call agent receiver to expection handler
MSG_CAD2CAP, // message from call agent dispatcher to sip call manager (sip processor)
MSG_CAD2CONF, //message from call agent dispatcher to configuration 
MSG_CAD2EXP, // message from call agent dispatcher to expection handler
MSG_MED2CAP, // message from media client to sip call manager
MSG_MED2CONF, // message from media client to configuration
MSG_MED2EXP, // message from media client to expection handler
MSG_DB2CONF, // message from DB handler to configuration
MSG_DB2CAP, // message from DB handler to sip call manager
MSG_DB2EXP, // message from DB handler to Expection handler
MSG_BC2CAP, // message from Business client to sip call manager
MSG_BC2CONF, //message from business client to configuration
MSG_BC2EXP, // message from business client to Expection handler
MSG_MON2CAP, // message from Monitoring client to sip call manager
MSG_MON2CONF, // message from monitoring client to configuration
MSG_MON2EXP, // message from monitoring client to Expection handler
MSG_SR2SC, // message from socket receiver to socket controller
MSG_EXP2UAS, // message from expection handler to user agent service
MSG_EXP2CAP, // message from expection handler to sip call manager
MSG_EXP2CAR, // message from expection handler to call agent receiver
MSG_EXP2CAD, // message from expection handler to call agetn dispatcher
MSG_CONF2BS, // message from config to business service
MSG_BS2CONF, // message from business service to configuraton
MSG_SOCDATA, // message from socket layer to application 
MSG_SOCDATA_SEND, // message to MVNO Business service socket thread to send data
MSG_SOC_CLOSE, // socket close event
MSG_BS2BSP, // message from business service to business service process
MSG_BSP2CONF, // message from business request processor to Configuer
MSG_CONF2BSP, // message from configure  to business request prcocessor.
MSG_CONF2BDB, // message from configuration service to business DB service
MSG_CONF2CHARG, // message from configuration service to Charging Service
MSG_BDB2CONF, // message from business DB service to configuration service
MSG_BDB2BSP, // message from DB service to Business Service  Process
MSG_BSP2CHARG, // message from Business Processor to charging service
MSG_BSP2ROUT, // message from Business Processor to Routing Service
MSG_BSP2GMSC, // message from Business processor to GMSC Service
MSG_BSP2MOMT, // message from Business Processor to MOMt GW Service
MSG_CHARG2GMSC,
MSG_CHARG2MOMT,
MSG_CHARG2ROUT,
MSG_CHARG2BSP,
MSG_ROUT2BSP,
MSG_ROUT2HTTP,
MSG_ROUT2MOMT,
MSG_ROUT2CHARG,
MSG_ROUT2GMSC,
MSG_ROUT2MON,
MSG_GMSC2ROUT,
MSG_GMSC2CHARG,
MSG_GMSC2MOMT,
MSG_GMSC2BSP,
MSG_GMSC2HTTP, 
MSG_MOMT2CONF,
MSG_CONF2MOMT,
MSG_MOMT2CHARG,
MSG_MOMT2GMSC,
MSG_MOMT2BSP,
MSG_CHARG2REP,
MSG_ROUT2REP,
MSG_SWITCH2REP,
MSG_GMSC2REP,
MSG_REP2CONF,
MSG_REP2DB,
MSG_BSP2MON,
MSG_HTTP2GMSC,
MSG_HTTP2BSP,
MSG_BSP2BSOP,
MSG_BSP2REP,
};

enum  SIPSIGNALEVENT{ 
SIP_NULL=0,
SIP_INVITE_REQ,
SIP_INVITE_RES,
SIP_STATE_CHANGE,
SIP_REINVITE_REQ,
SIP_CANCEL_REQ,
SIP_CANCEL_RES,
SIP_BYE_REQ,
SIP_BYE_RES,
SIP_MESSAGE_REQ,
SIP_MESSAGE_RES,
SIP_INFO_REQ,
SIP_INFO_RES,
SIP_PRACK_REQ,
SIP_PRACK_RES,
SIP_REGISTER_REQ,
SIP_REGISTER_RES,
SIP_TERMINATED_REQ,
SIP_TERMINATED_RES,
SIP_OPTION_REQ,
SIP_OPTION_RES,
SIP_PARAMUPDATE_RES,
SIP_ERROR_RECEIVED,
EN_CA_100_REL=100,
EN_CA_180_REL=180,
EN_CA_181_REL,
EN_CA_182_REL,
EN_CA_183_REL,
EN_CA_199_REL=199,
EN_CA_200_REL=200,
EN_CA_202_REL=202,
EN_CA_204_REL=204,
EN_CA_300_REL=300,
EN_CA_302_REL=302,
EN_CA_400_REL=400,
EN_CA_401_REL,
EN_CA_402_REL,
EN_CA_403_REL=403,
EN_CA_404_REL,
EN_CA_407_REL=407,
EN_CA_408_REL=408,
EN_CA_410_REL=410,
EN_CA_480_REL=480,
EN_CA_483_REL=483,
EN_CA_484_REL=484,
EN_CA_486_REL=486,
EN_CA_487_REL,
EN_CA_488_REL=488,
EN_CA_491_REL=491,
EN_CA_500_REL=500,
EN_CA_501_REL,
EN_CA_502_REL,
EN_CA_503_REL,
EN_CA_504_REL,
EN_CA_510_REL=510
};

static STR GetLocalIPAddress(unsigned short addrType)
{
   struct ifaddrs * ifAddrStruct=NULL;
   struct ifaddrs * ifa=NULL;
   void * tmpAddrPtr=NULL;
   char ipV4Addr[INET_ADDRSTRLEN] = {0};
   char ipV6Addr[INET6_ADDRSTRLEN] = {0};
   std::string ret("");

   getifaddrs(&ifAddrStruct);
   for(ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
   {
      if(!ifa->ifa_addr || ifa->ifa_addr->sa_family != addrType || strcmp(ifa->ifa_name, "lo") == 0 || strstr(ifa->ifa_name, "vir"))
      {
         continue;
      }
      std::string startStr = "fe80";
      if (AF_INET == addrType) 
      {
         tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
         inet_ntop(AF_INET, tmpAddrPtr, ipV4Addr, INET_ADDRSTRLEN);
         ret = ipV4Addr;
      }
      else if (AF_INET6 == addrType)
      {
         if (ifa->ifa_addr->sa_family==AF_INET6)
         {
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            inet_ntop(AF_INET6, tmpAddrPtr, ipV6Addr, INET6_ADDRSTRLEN);
            if (std::string(ipV6Addr).substr(0, startStr.length()) == startStr)
            {
               continue;
            }
            ret = ipV6Addr;
            break;
         }
      }
   }
   if (ifAddrStruct!=NULL)
   {
      freeifaddrs(ifAddrStruct);
   }
   return ret;
}

#endif
