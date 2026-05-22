//local includes
#include "xGateHmpGstBin.h"
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <ifaddrs.h>
#define THISMODULE "Util"

vector<xGateTCPConTuple>xGateUtil::m_mgConnectionMap;
ACE_Thread_Mutex xGateUtil::m_mgConnectionMapLock;
xGateTCPReceiver* xGateUtil::m_mgReceiver = NULL;
xGateMgProcessor* xGateUtil::m_mgProcessor = NULL;
xGateMGDispatcher* xGateUtil::m_mgDispatcher=NULL;
xGateTCPSocketHandler* xGateUtil::m_tcpHandler=NULL;
xGateMGCConnector* xGateUtil::m_mgcConnector=NULL;
vector<string> xGateUtil::m_MgcIpAddressList;
char xGateUtil::m_localIp4Addr[INET_ADDRSTRLEN] = {0};
char xGateUtil::m_localIp6Addr[INET6_ADDRSTRLEN] = {0};

#ifdef XGATE_HAS_GSTREAMER
xGateHmpProcessor* xGateUtil::m_hmpProcessor = NULL;
xGateHmpGstManager* xGateUtil::m_hmpGstManager = NULL;
#endif

void xGateUtil::SetLocalIpAddr(char *localIpAddr)
{
  if(strlen(localIpAddr) > 16) {
    memset(m_localIp6Addr, '\0', INET6_ADDRSTRLEN);
    memcpy(m_localIp6Addr, localIpAddr, INET6_ADDRSTRLEN);
  } else {
    memset(m_localIp4Addr, '\0', INET_ADDRSTRLEN);
    memcpy(m_localIp4Addr, localIpAddr, INET_ADDRSTRLEN);
  }
}

gboolean xGateUtil::Thread_creation(void *data,void (*Pfuntion)(void *),string classname)
{
  ACE_thread_t tid;
  if(ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC)Pfuntion,data,THR_NEW_LWP|THR_DETACHED,&tid)< 0)
  return FALSE;
 
  if(classname == "client")
  { 
   Client *client=static_cast<Client *>(data);
   client->m_threadID=tid;
   XGLOG_INFO("xGateUtil::Thread_creation, m_threadID:%d --->CallID:%s",tid,client->m_detail.m_callId.c_str());
   }

  return TRUE;

}
gboolean xGateUtil::Thread_destruction(ACE_thread_t tid)
{
   ACE_Thread_Manager::instance()->cancel(tid,1);
   XGLOG_INFO("xGateUtil::Thread_destruction, m_threadID:%d",tid);
   return TRUE;
}


xGateRetVal xGateUtil::findLocalIpAddr(char *ipAddr, unsigned short AddrType)
{
  struct ifaddrs * ifAddrStruct=NULL;
  struct ifaddrs * ifa=NULL;
  void * tmpAddrPtr=NULL;

  if(ipAddr == NULL) {
    return EN_XGATE_STATUS_ERROR;
  }

  getifaddrs(&ifAddrStruct);

  for(ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
    if(!ifa->ifa_addr || ifa->ifa_addr->sa_family != AddrType || strcmp(ifa->ifa_name, "lo") == 0){
      continue;
    }
    std::string startStr = "fe80";
    if (AF_INET ==ifa->ifa_addr->sa_family) { // check it is IP4
      // is a valid IP4 Address
      tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      inet_ntop(AF_INET, tmpAddrPtr, ipAddr, INET_ADDRSTRLEN);
      printf("xGateUtil::findLocalIpAddr, %s IP4 Address %s\n", ifa->ifa_name, ipAddr);
    } else if (AF_INET6 == ifa->ifa_addr->sa_family) { // check it is IP6
      // is a valid IP6 Address
      tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
      inet_ntop(AF_INET6, tmpAddrPtr, ipAddr, INET6_ADDRSTRLEN);
     if (std::string(ipAddr).substr(0, startStr.length()) == startStr) continue;
      printf("xGateUtil::findLocalIpAddr, %s IP6 Address %s\n", ifa->ifa_name, ipAddr);
    }
    break;
  }
  if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
  return EN_XGATE_STATUS_SUCCESS;
}

char * xGateUtil::getLocalIpAddr(unsigned short AddrType) {
  switch(AddrType) {
    case AF_INET:
      {
        if(m_localIp4Addr[0] != 0) {
          return m_localIp4Addr;
        } else {
          findLocalIpAddr(m_localIp4Addr, AF_INET);
          return m_localIp4Addr;
        }
        break;
      }
    case AF_INET6:
      {
        if(m_localIp6Addr[0] != 0) {
          return m_localIp6Addr;
        } else {
          findLocalIpAddr(m_localIp6Addr, AF_INET6);
          return m_localIp6Addr;
        }
        break;
      }
    default:
      {
        printf("xGateUtil::getLocalIpAddr, given address type not supported\n");
        break;
      }
  }
}

void xGateUtil::setLocalIpAddr(char *localIpAddr)
{
  if(strlen(localIpAddr) > 16) {
    memset(m_localIp6Addr, '\0', INET6_ADDRSTRLEN);
    memcpy(m_localIp6Addr, localIpAddr, INET6_ADDRSTRLEN);
  } else {
    memset(m_localIp4Addr, '\0', INET_ADDRSTRLEN);
    memcpy(m_localIp4Addr, localIpAddr, INET_ADDRSTRLEN);
  }
}

void xGateUtil::makeDir(const char *dir)
{
  char tmp[256];
  char *p = NULL;
  size_t len;

  snprintf(tmp, sizeof(tmp),"%s",dir);
  len = strlen(tmp);
  if(tmp[len - 1] == '/')
    tmp[len - 1] = 0;
  for(p = tmp + 1; *p; p++)
    if(*p == '/') {
      *p = 0;
      mkdir(tmp, S_IRWXU);
      *p = '/';
    }
  mkdir(tmp, S_IRWXU);
}


string xGateUtil::convertToString(char* a, int size)
{
  int i;
  string s = "";
  for (i = 0; i < size; i++) {
    s = s + a[i];
  }
  return s;
}


void xGateUtil::addToMgConnectionMap(xGateTCPConTuple tuple)
{
  XGLOG_WARN( "xGateUtil::addToMgConnectionMap Add the entry"\
      " Address :%s:%d Fd:%d",(char*)ur_log_string (tuple.ipAddress),tuple.port,tuple.fd);
  ACE_GUARD(ACE_Thread_Mutex,guard, m_mgConnectionMapLock);
  m_mgConnectionMap.push_back(tuple);
  XGLOG_WARN( "xGateUtil::addToMgConnectionMap");
}

xGateRetVal xGateUtil::getFromMgConnectionMap(string address, unsigned short port, int& fd)
{

  XGLOG_WARN( "xGateUtil::getFromMgConnectionMap Fetch the entry"
      " for address :%s port:%d",(char*)ur_log_string (address),port);

  xGateRetVal retVal = EN_XGATE_STATUS_NOT_PRESENT;
  unsigned int index = 0;


  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgConnectionMapLock, EN_XGATE_STATUS_ERROR);


  for(index = 0; index < m_mgConnectionMap.size();index++)
  {
    if(m_mgConnectionMap[index].ipAddress == address)
    {

      if( port == 0 || m_mgConnectionMap[index].port == port)
      {
        retVal = EN_XGATE_STATUS_SUCCESS;
        break;
      }
    }
  }

  if(retVal == EN_XGATE_STATUS_SUCCESS)
  {
    fd = m_mgConnectionMap[index].fd;
  }
  else
  {
    XGLOG_ERROR( "xGateUtil::getFromMgConnectionMap No entry found"
        " for Address:%s Port:%d",(char*)ur_log_string (address),port);
    retVal = EN_XGATE_STATUS_NOT_PRESENT;
  }
  XGLOG_WARN( "Exit xGateUtil::getFromMgConnectionMap");
  return retVal;
}

bool xGateUtil::isPresentInMgConnectionMap(string address, unsigned short port)
{
  XGLOG_WARN( "xGateUtil::isPresentInMgConnectionMap Address :%s"
      ,(char*)ur_log_string (address));


  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgConnectionMapLock, false);

  for(unsigned int i = 0; i< m_mgConnectionMap.size();i++)
  {
    if(m_mgConnectionMap[i].ipAddress == address
        && m_mgConnectionMap[i].port == port)
    {

      XGLOG_WARN( "xGateUtil::isPresentInMgConnectionMap" \
          " entry is Present For Address :%s:%d",(char*)ur_log_string (address),port);
      return true;
    }
  }
  return false;
}


void xGateUtil::getTimerIdFromConnMap (int fd, xGateTCPConnectionType eConnType,
    long int &timerId)
{
  XGLOG_INFO( "Enter: xGateUtil::getTimerIdFromConnMap");
  unsigned int index = 0;
  if (EN_XGATE_MG_CONNECTION == eConnType)
  {
    ACE_GUARD(ACE_Thread_Mutex,guard,m_mgConnectionMapLock);
    for(index = 0; index < m_mgConnectionMap.size();index++)
    {
      if(m_mgConnectionMap[index].fd == fd)
      {
        timerId = m_mgConnectionMap[index].timerId;
        break;
      }
    }
  }
  else
  {
    XGLOG_ERROR( "xGateUtil::getTimerIdFromConnMap Invalid eConnType %d", eConnType);
  }
  XGLOG_INFO( "Exit: xGateUtil::getTimerIdFromConnMap");
}



void xGateUtil::removeFromMgConnectionMap(xGateTCPConTuple tuple)
{
  XGLOG_WARN( "Enter xGateUtil::removeFromMgConnectionMap"\
      " Address:%s:%d and Fd:%d", (char*)ur_log_string (tuple.ipAddress), tuple.port, tuple.fd);

  ACE_GUARD(ACE_Thread_Mutex, guard, m_mgConnectionMapLock);

  for(unsigned int i = 0; i<m_mgConnectionMap.size();i++)
  {
    if(m_mgConnectionMap[i] == tuple)
    {
      m_mgConnectionMap.erase(m_mgConnectionMap.begin()+i);
      XGLOG_WARN( "xGateUtil::removeFromMgConnectionMap erased the entry");
      break;
    }
  }
  XGLOG_WARN( "Exit  xGateUtil::removeFromMgConnectionMap");
}

AudioDetail::AudioDetail():
  m_mediaType(0), m_fmtp(""), m_dtmfFmtp(""),m_rtcpFpMechanism(""),m_codec(0), m_codecName(""), m_ptime(0), m_maxPtime(0), 
  m_clientIp(""), m_clientPort(0), m_serverIp(""),/* m_serverIp6(""),*/ m_serverPort(0), m_relayIp(""), m_relayPort(0), m_inBandDtmf(0), 
  m_rflxIp(""), m_rflxPort(0), m_dialIp(""), m_dialPort(0), m_respIp(""), m_respPort(0),m_is_silence_added(false),
	m_samplingRate(0),m_dtmfSamplingRate(0),m_cnPt(0),m_cnSamplingRate(0),m_rtcpFpPayload(0),m_serverTtsPort(0),m_server64Port(0)
{
}

AudioDetail::~AudioDetail()
{
}

AudioDetail& AudioDetail::operator=(AudioDetail &src)
{
  m_mediaType = src.m_mediaType;
  m_fmtp = src.m_fmtp;
  m_dtmfFmtp = src.m_dtmfFmtp;
  m_rtcpFpMechanism = src.m_rtcpFpMechanism;
  m_codec = src.m_codec;
  m_codecName = src.m_codecName;
  m_ptime = src.m_ptime;
  m_maxPtime = src.m_maxPtime;
  m_samplingRate = src.m_samplingRate;
  m_dtmfSamplingRate = src.m_dtmfSamplingRate;
  m_clientIp = src.m_clientIp;
  m_clientPort = src.m_clientPort;
  m_serverIp = src.m_serverIp;
  m_serverPort = src.m_serverPort;
  m_relayIp = src.m_relayIp;
  m_relayPort = src.m_relayPort;
  m_rflxIp = src.m_rflxIp;
  m_rflxPort = src.m_rflxPort;
  m_dialIp = src.m_dialIp;
  m_dialPort = src.m_dialPort;
  m_respIp = src.m_respIp;
  m_respPort = src.m_respPort;
  m_ivrList = src.m_ivrList;
  m_is_silence_added = src.m_is_silence_added;
  m_cnPt = src.m_cnPt;
  m_cnSamplingRate = src.m_cnSamplingRate;
  m_rtcpFpPayload = src.m_rtcpFpPayload;
  m_inBandDtmf=src.m_inBandDtmf;
  m_dtmfpt=src.m_dtmfpt;
  m_serverTtsPort = src.m_serverTtsPort;
  m_server64Port = src.m_server64Port;
  return *this;
}


