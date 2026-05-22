//self includes
#include "xGateUtil.h"
#include "xglog.h"
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>
#define THISMODULE "Util"

vector<xGateTCPConTuple>xGateUtil::m_mgConnectionMap;
ACE_Thread_Mutex xGateUtil::m_mgConnectionMapLock;
xGateConnector *   xGateUtil::m_mgConnector = NULL;
xGateMgProcessor* xGateUtil::m_mgProcessor = NULL;
xGateTCPReceiver* xGateUtil::m_mgReceiver = NULL;
xGateSrvrProcessor* xGateUtil::m_srvrProcessor = NULL;
xGateMGDispatcher* xGateUtil::m_mgDispatcher = NULL;

#ifdef XGATE_HAS_DBM_SUPPORT
xGateDbmProcessor* xGateUtil::m_dbmProcessor = NULL;
xGateDbmDispatcher* xGateUtil::m_dbmDispatcher = NULL;
xGateDbmReceiver* xGateUtil::m_dbmReceiver = NULL;
#endif
#ifdef XGATE_HAS_BUSINESS_SUPPORT 
xGateBusinessProcessor* xGateUtil::m_businessProcessor = NULL;
xGateDBManager* xGateUtil::m_dbManager =  NULL;
xGateBusinessScheduler *xGateUtil::m_businessScheduler = NULL;
#endif

char xGateUtil::m_localIp4Addr[INET_ADDRSTRLEN] = {0};
char xGateUtil::m_localIp6Addr[INET6_ADDRSTRLEN] = {0};
char *xGateUtil::LocalIPadress = NULL;

xGateHmpProcessor* xGateUtil::m_hmpProcessor = NULL;
xGateHmpGstManager* xGateUtil::m_hmpGstManager = NULL;
unsigned int xGateUtil::m_usedGateWayId = 0;
void  xGateUtil::SetLocalIPAddress(void)
{
  char hostbuffer[256] = "";
  struct hostent *host_entry ;
  int hostname;
  hostname = gethostname(hostbuffer, sizeof(hostbuffer));
  host_entry = gethostbyname(hostbuffer);
  if(xGateUtil::LocalIPadress == NULL && host_entry)
  {
    xGateUtil::LocalIPadress  = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
   XGLOG_INFO("xGateUtil::SetLocalIPAddress as %s", LocalIPadress);
  }
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
   for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
    //if (!ifa->ifa_addr || strcmp(ifa->ifa_name, "ens160") != 0 || ifa->ifa_addr->sa_family != AddrType) {
    if (!ifa->ifa_addr || strcmp(ifa->ifa_name, "lo") == 0 || ifa->ifa_addr->sa_family != AddrType) {
      continue;
    }

    if (AF_INET == AddrType) { // check it is IP4
      // is a valid IP4 Address
      tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      inet_ntop(AF_INET, tmpAddrPtr, ipAddr, INET_ADDRSTRLEN);
      printf("xGateUtil::findLocalIpAddr, %s IP4 Address %s\n", ifa->ifa_name, ipAddr);
    } else if (AF_INET6 == AddrType) { // check it is IP6
      // is a valid IP6 Address
      tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
      inet_ntop(AF_INET6, tmpAddrPtr, ipAddr, INET6_ADDRSTRLEN);
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

/// Interface to add to MgConnection Map
void xGateUtil::addToMgConnectionMap(xGateTCPConTuple tuple)
{
 XGLOG_INFO("xGateUtil::addToMgConnectionMap Add the entry"\
      " Address :%s:%d Fd:%d",tuple.ipAddress.c_str(),tuple.port,tuple.fd);
  /// Acquire the Lock
  ACE_GUARD(ACE_Thread_Mutex,guard, m_mgConnectionMapLock);
  m_mgConnectionMap.push_back(tuple);
  XGLOG_WARN( "xGateUtil::addToMgConnectionMap"); 
}

xGateRetVal xGateUtil::getFromMgConnectionMap(int index, int& fd)
{
 XGLOG_INFO("xGateUtil::getFromMgConnectionMap Fetch the entry for index: %d", index);
  xGateRetVal retVal = EN_XGATE_STATUS_NOT_PRESENT;

  if(m_mgConnectionMap.size() <= 0) {
    XGLOG_WARN( "xGateUtil::getFromMgConnectionMap no connection avalible in MgConnectionMap");
    return retVal;
  }
  /// Acquire the NW Map Lock
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgConnectionMapLock, EN_XGATE_STATUS_ERROR);
  fd = m_mgConnectionMap[index].fd;
  if(fd > 0) {
    retVal = EN_XGATE_STATUS_SUCCESS;
  }
  return retVal;
}
std::string xGateUtil::getMgIPOnIndex(int mgid){
if(mgid < 0)
 return m_mgConnectionMap[0].ipAddress;

if(m_mgConnectionMap.size() == 0)
 return "";

if(mgid > m_mgConnectionMap.size())
 return m_mgConnectionMap[0].ipAddress;
return m_mgConnectionMap[mgid].ipAddress;
}
int xGateUtil::getFromMgConnectionMap(string ipaddr){
XGLOG_INFO("xGateUtil::getFromMgConnectionMap %s ipaddr", ipaddr.c_str());
unsigned int index = 0;
ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgConnectionMapLock, EN_XGATE_STATUS_ERROR);
 for(index = 0; index < m_mgConnectionMap.size();index++)
  {
    if(m_mgConnectionMap[index].ipAddress == ipaddr)
    {
	return index;	
    }
  }
return 0;
}
/// Interface to fetch the Fd from MgConnectionMap
xGateRetVal xGateUtil::getFromMgConnectionMap(string address, unsigned short port, int& fd)
{ 

  XGLOG_WARN( "xGateUtil::getFromMgConnectionMap Fetch the entry" 
      " for address :%s port:%d",address.c_str(),port);

  xGateRetVal retVal = EN_XGATE_STATUS_NOT_PRESENT;
  unsigned int index = 0;

  /// Acquire the NW Map Lock
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgConnectionMapLock, EN_XGATE_STATUS_ERROR);

  /// Iterate throw till the entry is found
  for(index = 0; index < m_mgConnectionMap.size();index++)
  {
    if(m_mgConnectionMap[index].ipAddress == address)
    {   
      /// Need to check if we have to match port also
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
      " for Address:%s Port:%d",address.c_str(),port);
    retVal = EN_XGATE_STATUS_NOT_PRESENT; 
  } 
  XGLOG_WARN( "Exit xGateUtil::getFromMgConnectionMap");
  return retVal; 
}

/// Interface to remove from Mg ConnectionaMap
void xGateUtil::removeFromMgConnectionMap(xGateTCPConTuple tuple)
{
  XGLOG_WARN( "Enter xGateUtil::removeFromMgConnectionMap"\
      " Address:%s:%d and Fd:%d", tuple.ipAddress.c_str(), tuple.port, tuple.fd); 

  /// Acquire the lock
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

/// Interface to check if the entry is Present in Mg Connection Map
bool xGateUtil::isPresentInMgConnectionMap(string address, unsigned short port)
{
  XGLOG_WARN( "xGateUtil::isPresentInMgConnectionMap Address :%s"
      ,address.c_str());

  ///Acquire the lock
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgConnectionMapLock, false);

  for(unsigned int i = 0; i< m_mgConnectionMap.size();i++)
  {
    if(m_mgConnectionMap[i].ipAddress == address 
        && m_mgConnectionMap[i].port == port)
    { 
      /// Entry is Present 
      XGLOG_WARN( "xGateUtil::isPresentInMgConnectionMap" \
          " entry is Present For Address :%s:%d",address.c_str(),port);
      return true;
    }
  }
  return false; 
}

void xGateUtil::getTimerIdFromConnMap (int fd, xGateTCPConnectionType eConnType,
                                            long int &timerId)
{
    XGLOG_WARN( "Enter: xGateUtil::getTimerIdFromConnMap");
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
        }//end for
    }//end else if
    else
    {
        XGLOG_ERROR( "xGateUtil::getTimerIdFromConnMap Invalid eConnType %d", eConnType);    
    }
    XGLOG_ERROR( "Exit: xGateUtil::getTimerIdFromConnMap");
}

#ifdef XGATE_HAS_DBM_SUPPORT
DbmReqDetail::DbmReqDetail() :
  m_transId(""), m_msgLen(0), m_dbType(0),
  m_opCode(0), m_dbName(""), m_tableName(""),
  m_spName(""), m_input(""), m_output(""), m_statusCode(0),
  m_errCode(0), m_affectedRows(0), m_message(""),m_url("")
{
}

DbmReqDetail::~DbmReqDetail()
{
}
#endif
int xGateUtil::selectMgId()
{
  unsigned int size = m_mgConnectionMap.size();
  //last_mg_id =((last_mg_id%xGateUtil::m_mgConnectionMap.size())+xGateUtil::m_mgConnectionMap.size())%xGateUtil::m_mgConnectionMap.size();
    if(size > 0) {
      m_usedGateWayId  = (m_usedGateWayId % size);
      if(m_usedGateWayId > size)
        m_usedGateWayId = 0;
        XGLOG_WARN( "xGateUtil::selectMgId %d", m_usedGateWayId);
        return m_usedGateWayId++;
     }
      XGLOG_WARN( "xGateUtil::selectMgId::<size %d", m_usedGateWayId);
     return m_usedGateWayId;
}

/*
* Get Current UTC time 
*/
std::string xGateUtil::getCurrentUTCTime()
{
    char buf[30] = "";
    time_t lt = time(NULL);
    auto local_time = *gmtime(&lt);
    auto local = mktime(&local_time);

    strftime(buf, sizeof(buf), "%F %T %Z", &local_time);
    // std::cout <<"Local Time is "<< buf;

    auto utc_time = *gmtime(&local);
    strftime(buf, sizeof(buf), "%FT%TZ", &utc_time);
    //   strftime(buf, sizeof(buf), "%F %T UTC", &utc_time);
    return std::string(buf);
}
/*
* Get Current UTC time epoch value
*/
time_t xGateUtil::getCurrentTimeStamp()
{
  return time(NULL);
}
