//local includes
#include "xGateMBCUtil.h"
#include "xGateHmpGstUtil.h"
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>
#define THISMODULE "Util"
vector<xGateTCPConTuple>xGateMBCUtil::m_mgConnectionMap;
map<int, pid_t>xGateMBCUtil:: m_mgSfuConnectionMap;
map<string, int>xGateMBCUtil:: m_mgSfuIdResIdMap;
map<int, string>xGateMBCUtil:: m_mgResIdSfuIdMap;
//map<int,ACE_SOCK_Stream>xGateMBCUtil:: m_mgSfuIdSocketIdMap;
map<int, MBCTcpConnInfo>xGateMBCUtil:: m_mbcConnectionMap;
map<int, int>xGateMBCUtil:: m_sfuIdPbxConnIdMap;
map<int, int>xGateMBCUtil:: m_sfuIdSfuConnIdMap;
map<int, int>xGateMBCUtil:: m_sfuConnIdPbxConnIdMap;
map<int,pair<int, char*>>xGateMBCUtil:: m_sfuIdAllocMsgMap;
vector<MBCSfuMsg *>xGateMBCUtil::m_mbcSfuMsgVect;
ACE_Thread_Mutex xGateMBCUtil::m_mgConnectionMapLock;
ACE_Thread_Mutex xGateMBCUtil::m_mgSfuConnectionMapLock;
ACE_Thread_Mutex xGateMBCUtil::m_mgSfuIdLock;
ACE_Thread_Mutex xGateMBCUtil::m_mgSfuIdResIdLock;
ACE_Thread_Mutex xGateMBCUtil::m_mgResIdSfuIdLock;
ACE_Thread_Mutex xGateMBCUtil::m_mbcConnectionMapLock;

ACE_Thread_Mutex xGateMBCUtil::m_sfuIdPbxConnIdMapLock;
ACE_Thread_Mutex xGateMBCUtil::m_sfuIdSfuConnIdMapLock;
ACE_Thread_Mutex xGateMBCUtil::m_sfuConnIdPbxConnIdMapLock;
ACE_Thread_Mutex xGateMBCUtil::m_allocMsgMapLock;
//xGateTCPReceiver* xGateMBCUtil::m_mgReceiver = NULL;
MBCControllerProcessor* xGateMBCUtil::m_mbcCtrlProcessor = NULL;
//vector<string> xGateMBCUtil::m_MgcIpAddressList;
char xGateMBCUtil::m_localIp4Addr[INET_ADDRSTRLEN] = {0};
char xGateMBCUtil::m_localIp6Addr[INET6_ADDRSTRLEN] = {0};

#ifdef XGATE_HAS_GSTREAMER

ClientService* xGateMBCUtil::m_mbcClientService = NULL;
//MBCSessionHandler* xGateMBCUtil::m_mbcSessionHandler = NULL;
//SFUController* xGateMBCUtil::m_sfuController = NULL;
//SFUBridgeProcessor* xGateMBCUtil:: m_sfuBridgeProcessor = NULL;
//xGateMgSFUDispatcher* xGateMBCUtil:: m_mgSFUDispatcher = NULL;
unsigned int xGateMBCUtil::m_sfuId = 0;
#endif

int  xGateMBCUtil::getSfuId(int &sfuId)
{
    printf("\nxGateMBCUtil::getSfuId : enter  ");
    sfuId = m_sfuId++;
    return true;
#if 0
     ACE_GUARD(ACE_Thread_Mutex, guard, m_mgSfuIdLock);
    map<string,int>::iterator it ;
    bool isEmptySlotFound = true; 
    bool found = false;
    
    for(int sfu_id=0; sfu_id < m_sfuId+1; sfu_id++)
    {
	found = false;
        isEmptySlotFound = true; 
        for (it = m_mgSfuIdResIdMap.begin(); it != m_mgSfuIdResIdMap.end(); ++it)
        {
            if (it->second == sfu_id)
            {
		found = true;
                isEmptySlotFound = false;
		break;
            }
        }
	if(found == false  && sfu_id < m_sfuId){
	  sfuId = sfu_id;
	  return isEmptySlotFound;
	}
    }
    sfuId = m_sfuId;
    printf("\nxGateMBCUtil::getSfuId : allocated sfuId - %d", m_sfuId);
    //XGLOG_WARN( "\nxGateMBCUtil::getSfuId : allocated sfuId - %d", m_sfuId);
    m_sfuId++;   // not to use function because of locking problem
    return isEmptySlotFound;
#endif
}
#if 0
static int  xGateMBCUtil::getSfuId(int &sfuId)
{
    printf("\nxGateMBCUtil::getSfuId : enter  ");
    //XGLOG_WARN( "\nxGateMBCUtil::getSfuId : enter");
     ACE_GUARD(ACE_Thread_Mutex, guard, m_mgSfuIdLock);
    map<string,int>::iterator it ;
    bool isEmptySlotFound = true; 
    
    for(int sfu_id=0; sfu_id < m_sfuId+1; sfu_id++)
    {
        isEmptySlotFound = true; 
        for (auto it = m_mgSfuIdResIdMap.begin(); it != m_mgSfuIdResIdMap.end(); ++it)
        {
            if (it->second == sfu_id)
            {
                isEmptySlotFound = false;
                sfuId = sfu_id;
                printf("\nxGateMBCUtil::getSfuId : found sfuId - %d", sfu_id);
                return isEmptySlotFound;
            }
        }
    }
    sfuId = m_sfuId;
    printf("\nxGateMBCUtil::getSfuId : allocated sfuId - %d", m_sfuId);
    //XGLOG_WARN( "\nxGateMBCUtil::getSfuId : allocated sfuId - %d", m_sfuId);
    m_sfuId++;   // not to use function because of locking problem
    return isEmptySlotFound;
}
#endif
void  xGateMBCUtil::incrSfuId(void)
{
    ACE_GUARD(ACE_Thread_Mutex, guard, m_mgSfuIdLock);
    m_sfuId++;
    printf("\nxGateMBCUtil::incrSfuId m_sfuId=%d\n",m_sfuId);
    return;
}

xGateRetVal xGateMBCUtil::findLocalIpAddr(char *ipAddr, unsigned short AddrType)
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
    if (AF_INET ==ifa->ifa_addr->sa_family) { // check it is IP4
      // is a valid IP4 Address
      tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      inet_ntop(AF_INET, tmpAddrPtr, ipAddr, INET_ADDRSTRLEN);
      printf("xGateMBCUtil::findLocalIpAddr, %s IP4 Address %s\n", ifa->ifa_name, ipAddr);
    } else if (AF_INET6 == ifa->ifa_addr->sa_family) { // check it is IP6
      // is a valid IP6 Address
      tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
      inet_ntop(AF_INET6, tmpAddrPtr, ipAddr, INET6_ADDRSTRLEN);
      printf("xGateMBCUtil::findLocalIpAddr, %s IP6 Address %s\n", ifa->ifa_name, ipAddr);
    }
    break;
  }
  if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
  return EN_XGATE_STATUS_SUCCESS;
}

char * xGateMBCUtil::getLocalIpAddr(unsigned short AddrType) {
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
        printf("xGateMBCUtil::getLocalIpAddr, given address type not supported\n");
        break;
      }
  }
	return NULL;
}

void xGateMBCUtil::setLocalIpAddr(char *localIpAddr)
{
  if(strlen(localIpAddr) > 16) {
    memset(m_localIp6Addr, '\0', INET6_ADDRSTRLEN);
    memcpy(m_localIp6Addr, localIpAddr, INET6_ADDRSTRLEN);
  } else {
    memset(m_localIp4Addr, '\0', INET_ADDRSTRLEN);
    memcpy(m_localIp4Addr, localIpAddr, INET_ADDRSTRLEN);
  }
}

void xGateMBCUtil::makeDir(const char *dir)
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

void xGateMBCUtil::addToMbcConnectionMap(int conn_id, MBCTcpConnInfo mbcTcpConnInfo)
{
  XGLOG_WARN( "xGateMBCUtil::addToMbcConnectionMap Add the entry"\
      " Address :%d",conn_id);
  ACE_GUARD(ACE_Thread_Mutex,guard, m_mbcConnectionMapLock);
  map<int,MBCTcpConnInfo>::iterator it = m_mbcConnectionMap.begin();
  m_mbcConnectionMap.insert(it, pair<int,MBCTcpConnInfo>(conn_id, mbcTcpConnInfo));
  XGLOG_WARN( "xGateMBCUtil::addToMbcConnectionMap success");
}

void xGateMBCUtil::getConnInfoFromMbcConnectionMap(int conn_id, MBCTcpConnInfo &mbcTcpConnInfo)
{
  XGLOG_INFO( "xGateMBCUtil::addToMbcConnectionMap Add the entry"\
      " Address :%d",conn_id);
  ACE_GUARD(ACE_Thread_Mutex,guard, m_mbcConnectionMapLock);
  map<int,MBCTcpConnInfo>::iterator it = m_mbcConnectionMap.find(conn_id);
  if(it != m_mbcConnectionMap.end()) {
    mbcTcpConnInfo = it->second;
    XGLOG_INFO("xGateMBCUtil::getConnInfoFromMbcConnectionMap Found entry for conn_id:%d",conn_id);
  }
  XGLOG_WARN( "xGateMBCUtil::addToMbcConnectionMap success");
}

//To store pbx_conn_id for sfu_id
void xGateMBCUtil::insert_sfuId_alloc_msg_map(int sfu_id, int msgLen, char* msg)
{
  MBCSfuMsg   *pMbcSfuMsg = NULL;
  ACE_GUARD(ACE_Thread_Mutex,guard, m_allocMsgMapLock);
  XGLOG_INFO( "xGateMBCUtil::insert_sfuId_alloc_msg_map");
  pMbcSfuMsg = new (MBCSfuMsg);
  if (pMbcSfuMsg){
     pMbcSfuMsg->sfuId = sfu_id;
     pMbcSfuMsg->len = msgLen;
     pMbcSfuMsg->msg = msg;
     m_mbcSfuMsgVect.push_back(pMbcSfuMsg);
     XGLOG_INFO( "xGateMBCUtil::insert_sfuId_alloc_msg_map success vector size: %d", m_mbcSfuMsgVect.size());
  }
}

void xGateMBCUtil::get_alloc_msg_from_sfuId(int sfu_id,std::pair<int, char*> & allocMsgPair)
{
  XGLOG_WARN( "xGateMBCUtil::get_alloc_msg_from_sfuId");
  ACE_GUARD(ACE_Thread_Mutex,guard, m_allocMsgMapLock);
  std::map<int,std::pair<int, char*>>::iterator it;
  it = m_sfuIdAllocMsgMap.find(sfu_id);
  if(it != m_sfuIdAllocMsgMap.end()) {
    allocMsgPair = it->second;
    XGLOG_INFO("xGateMBCUtil::get_alloc_msg_from_sfuId Found entry for sfu_id:%d",sfu_id);
  }
}

void xGateMBCUtil::remove_alloc_msg_from_map(int sfu_id)
{
  XGLOG_WARN( "xGateMBCUtil::remove_alloc_msg_from_map sfuId=%d",sfu_id);
  ACE_GUARD(ACE_Thread_Mutex,guard, m_allocMsgMapLock);
  m_sfuIdAllocMsgMap.erase(sfu_id);
}

//To store pbx_conn_id for sfu_id
void xGateMBCUtil::insert_sfuId_pbxConnId_map(int sfu_id, int pbx_conn_id)
{
  XGLOG_WARN( "xGateMBCUtil::insert_sfuId_pbxConnId_map");
  ACE_GUARD(ACE_Thread_Mutex,guard, m_sfuIdPbxConnIdMapLock);
  map<int,int>::iterator it = m_sfuIdPbxConnIdMap.begin();
  m_sfuIdPbxConnIdMap.insert(it, pair<int,int>(sfu_id,pbx_conn_id));
  XGLOG_WARN( "xGateMBCUtil::addToMbcConnectionMap success");
}

void xGateMBCUtil::get_pbxConnId_from_sfuId(int sfuId,int &pbxConnId)
{
  XGLOG_WARN( "xGateMBCUtil::get_pbxConnId_from_sfuId");
  ACE_GUARD(ACE_Thread_Mutex,guard, m_sfuIdPbxConnIdMapLock);
  map<int,int>::iterator it = m_sfuIdPbxConnIdMap.find(sfuId);
  if(it != m_sfuIdPbxConnIdMap.end()) {
    pbxConnId = it->second;
    XGLOG_INFO("xGateMBCUtil::get_pbxConnId_from_sfuId Found entry for sfu_id:%d",sfuId);
  }
}

void xGateMBCUtil::removeSfuIdFromPbxConnIdMap(int sfu_id)
{
  XGLOG_WARN( "xGateMBCUtil::removeSfuIdFromPbxConnIdMap sfuId=%d",sfu_id);
  ACE_GUARD(ACE_Thread_Mutex,guard, m_sfuIdPbxConnIdMapLock);
  m_sfuIdPbxConnIdMap.erase(sfu_id);
}

//To store sfuConnId for sfuId
void xGateMBCUtil::insert_sfuId_sfuConnId_map(int sfu_id, int sfu_conn_id)
{
  XGLOG_WARN( "xGateMBCUtil::insert_sfuId_sfuConnId_map");
  ACE_GUARD(ACE_Thread_Mutex,guard, m_sfuIdSfuConnIdMapLock);
  map<int,int>::iterator it = m_sfuIdSfuConnIdMap.begin();
  m_sfuIdSfuConnIdMap.insert(it, pair<int,int>(sfu_id,sfu_conn_id));
  XGLOG_WARN( "xGateMBCUtil::insert_sfuId_sfuConnId_map success");
}

void xGateMBCUtil::get_sfuConnId_from_sfuId(int sfuId,int &sfuConnId)
{
  XGLOG_WARN( "xGateMBCUtil::get_sfuConnId_from_sfuId: %d", sfuId);
  ACE_GUARD(ACE_Thread_Mutex,guard, m_sfuIdSfuConnIdMapLock);
  map<int,int>::iterator it = m_sfuIdSfuConnIdMap.find(sfuId);
  if(it != m_sfuIdSfuConnIdMap.end()) {
    sfuConnId = it->second;
    XGLOG_INFO("xGateMBCUtil::get_sfuConnId_from_sfuId: %d Found entry for sfu_id:%d",sfuConnId, sfuId);
  }else{
    XGLOG_ERROR("xGateMBCUtil::Failed to get entry for sfu_id:%d ",sfuId);
    sfuConnId = -1;
  }
}

void xGateMBCUtil::get_sfuId_from_sfuConnId(int sfuConnId,int &sfuId)
{
  XGLOG_WARN( "xGateMBCUtil::get_sfuConnId_from_sfuId");
  ACE_GUARD(ACE_Thread_Mutex,guard, m_sfuIdSfuConnIdMapLock);
  map<int,int>::iterator it = m_sfuIdSfuConnIdMap.begin();
  for(it; it != m_sfuIdSfuConnIdMap.end(); it++) {
    if(it->second == sfuConnId) {
      sfuId = it->first;
      XGLOG_INFO("xGateMBCUtil::get_sfuId_from_sfuConnId Found entry for sfuConnId:%d",sfuConnId);
      return;
    }
  }
	if(it == m_sfuIdSfuConnIdMap.end()){
		sfuId = -1;
	}
}

void xGateMBCUtil::removeSfuIdFromSfuConnIdMap(int sfu_id)
{
  XGLOG_WARN( "xGateMBCUtil::removeSfuIdFromSfuConnIdMap sfuId=%d",sfu_id);
  ACE_GUARD(ACE_Thread_Mutex,guard, m_sfuIdSfuConnIdMapLock);
  m_sfuIdSfuConnIdMap.erase(sfu_id);
}

//To store pbxConnId for sfuConnId
void xGateMBCUtil::insert_sfuConnId_pbxConnId_map(int sfu_conn_id, int pbx_conn_id)
{
  XGLOG_WARN( "xGateMBCUtil::insert_sfuConnId_pbxConnId_map");
  ACE_GUARD(ACE_Thread_Mutex,guard, m_sfuConnIdPbxConnIdMapLock);
  map<int,int>::iterator it = m_sfuConnIdPbxConnIdMap.begin();
  m_sfuConnIdPbxConnIdMap.insert(it, pair<int,int>(sfu_conn_id,pbx_conn_id));
  XGLOG_WARN( "xGateMBCUtil::insert_sfuConnId_pbxConnId_map success");
}

void xGateMBCUtil::get_pbxConnId_from_sfuConnId(int sfu_conn_id, int &pbx_conn_id)
{
  XGLOG_WARN( "xGateMBCUtil::get_pbxConnId_from_sfuConnId");
  ACE_GUARD(ACE_Thread_Mutex,guard, m_sfuConnIdPbxConnIdMapLock);
  map<int,int>::iterator it = m_sfuConnIdPbxConnIdMap.find(sfu_conn_id);
  if(it != m_sfuConnIdPbxConnIdMap.end()) {
    pbx_conn_id = it->second;
    XGLOG_INFO("xGateMBCUtil::get_sfuConnId_from_sfuId Found entry for sfu_id:%d",sfu_conn_id);
  }
}

void xGateMBCUtil::removeSfuConnIdFromPbxConnIdMap(int sfu_conn_id)
{
  XGLOG_WARN( "xGateMBCUtil::removeSfuIdFromPbxConnIdMap sfu_conn_id=%d",sfu_conn_id);
  ACE_GUARD(ACE_Thread_Mutex,guard, m_sfuConnIdPbxConnIdMapLock);
  m_sfuConnIdPbxConnIdMap.erase(sfu_conn_id);
}

void xGateMBCUtil::addToMgSfuConnectionMap(int sfu_id, pid_t pid)
{
  XGLOG_WARN( "xGateMBCUtil::addToMgSfuConnectionMap Add the entry"\
      " Address :%d",sfu_id);
  ACE_GUARD(ACE_Thread_Mutex,guard, m_mgSfuConnectionMapLock);
  map<int,pid_t>::iterator it = m_mgSfuConnectionMap.begin();
  m_mgSfuConnectionMap.insert(it, pair<int,pid_t>(sfu_id, pid));
  XGLOG_WARN( "xGateMBCUtil::addToMgConnectionMap");
}

int xGateMBCUtil::getPidFromMgSfuConnectionMap(int sfu_id, pid_t& pid)
{

  XGLOG_WARN( "xGateMBCUtil::getPidFromMgSfuConnectionMap Fetch the entry"
      " for mg resouce id= [%d]",sfu_id);

  xGateRetVal retVal = EN_XGATE_STATUS_NOT_PRESENT;
  
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgSfuConnectionMapLock, EN_XGATE_STATUS_ERROR);
  map<int,pid_t>::iterator it = m_mgSfuConnectionMap.begin();
  it = m_mgSfuConnectionMap.find(sfu_id);
  if (it != m_mgSfuConnectionMap.end())
  {
     pid = it->second;
     retVal = EN_XGATE_STATUS_SUCCESS;
     printf("xGateMBCUtil::getPidFromMgSfuConnectionMap is FOUND");
  }
  else
  {
     printf("xGateMBCUtil::getPidFromMgSfuConnectionMap is NULL");
     return (retVal);
  }

  XGLOG_WARN( "Exit xGateMBCUtil::getPidFromMgSfuConnectionMap");
  return retVal;
}

int xGateMBCUtil::removePidFromMgSfuConnectionMap(int sfu_id)
{
  XGLOG_WARN( "xGateMBCUtil::removePidFromMgSfuConnectionMap"
      " for mg resouce id= [%d]",sfu_id);

  xGateRetVal retVal = EN_XGATE_STATUS_SUCCESS;
  
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgSfuConnectionMapLock, EN_XGATE_STATUS_ERROR);
  m_mgSfuConnectionMap.erase(sfu_id);

  return retVal;
}
#if 1
xGateRetVal xGateMBCUtil::getFromMgConnectionMap(string address, unsigned short port, int& fd)
{

  XGLOG_WARN( "xGateMBCUtil::getFromMgConnectionMap Fetch the entry"
      " for address :%s port:%d",address.c_str(),port);

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
    XGLOG_ERROR( "xGateMBCUtil::getFromMgConnectionMap No entry found"
        " for Address:%s Port:%d",address.c_str(),port);
    retVal = EN_XGATE_STATUS_NOT_PRESENT;
  }
  XGLOG_WARN( "Exit xGateMBCUtil::getFromMgConnectionMap");
  return retVal;
}
#endif
void xGateMBCUtil::addToMgSfuResMap(string resId, int sfu_id)
{
  XGLOG_WARN( "xGateMBCUtil::addToMgSfuResMap mg resource Id=[%s] sfuId=%d",resId.c_str(),sfu_id);
  //ACE_GUARD(ACE_Thread_Mutex,guard, m_mgSfuResMapLock);
  map<string,int>::iterator it = m_mgSfuIdResIdMap.begin();
  m_mgSfuIdResIdMap.insert(it, pair<string,int>(resId,sfu_id));
}

void xGateMBCUtil::addToMgResSfuMap(int sfu_id,string resId)
{
  XGLOG_WARN( "xGateMBCUtil::addToMgResSfuMap mg resource Id=[%s] sfuId=%d",resId.c_str(),sfu_id);
  //ACE_GUARD(ACE_Thread_Mutex,guard, m_mgSfuResMapLock);
  map<int,string>::iterator it = m_mgResIdSfuIdMap.begin();
  m_mgResIdSfuIdMap.insert(it, pair<int,string>(sfu_id,resId));
}

bool xGateMBCUtil::getResIdFromSfuIdMap(int sfuId,string& uid)
{
  XGLOG_WARN( "xGateMBCUtil::getResIdFromSfuIdResIdmap Fetch the entry for mg sfu id= [%d]",sfuId);
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgResIdSfuIdLock, NULL);
  map<int,string>::iterator it;
  it = m_mgResIdSfuIdMap.find(sfuId);
  if (it != m_mgResIdSfuIdMap.end())
  {
		 uid = it->second.c_str();
     return true; 
  }
  else
  {
     return false;
  }
}

int xGateMBCUtil::getsfuIdFromResIdMap(string resId)
{
  XGLOG_WARN( "xGateMBCUtil::getsfuIdFromResIdMap Fetch the entry"
      " for mg resouce id= [%s]",resId.c_str());

  xGateRetVal retVal = EN_XGATE_STATUS_NOT_PRESENT;
  
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgSfuIdResIdLock, EN_XGATE_STATUS_ERROR);
  map<string,int>::iterator it;
  it = m_mgSfuIdResIdMap.find(resId);
  if (it != m_mgSfuIdResIdMap.end())
  {
     return (it->second);
  }
  else
  {
     return (retVal);
  }

  XGLOG_WARN( "Exit xGateMBCUtil::getsfuIdFromResIdMap");
  return retVal;
}

int xGateMBCUtil::removeSfuIdFromResIdMap(int sfu_id)
{
  XGLOG_WARN( "xGateMBCUtil::removeSfuIdFromResIdMap"
      " for sfu_id= [%d]",sfu_id);

  xGateRetVal retVal = EN_XGATE_STATUS_SUCCESS;
  
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgSfuIdResIdLock, EN_XGATE_STATUS_ERROR);
  map<string,int>::iterator it;
  for (auto it = m_mgSfuIdResIdMap.begin(); it != m_mgSfuIdResIdMap.end(); ++it)
  {
      if (it->second == sfu_id)
      {
          m_mgSfuIdResIdMap.erase(it);
	  return retVal; 
      }
  }
  retVal = EN_XGATE_STATUS_ERROR;
  return retVal;
}

int xGateMBCUtil::removeResIdFromSfuIdMap(int sfu_id)
{
  XGLOG_WARN( "xGateMBCUtil::removeResIdFromSfuIdMap"
      " for sfu_id= [%d]",sfu_id);

  xGateRetVal retVal = EN_XGATE_STATUS_SUCCESS;
  
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgResIdSfuIdLock, EN_XGATE_STATUS_ERROR);
  map<int,string>::iterator it;
  for (auto it = m_mgResIdSfuIdMap.begin(); it != m_mgResIdSfuIdMap.end(); ++it)
  {
      if (it->first == sfu_id)
      {
          m_mgResIdSfuIdMap.erase(it);
	  return retVal; 
      }
  }
  retVal = EN_XGATE_STATUS_ERROR;
  return retVal;
}

#if 1
bool xGateMBCUtil::isPresentInMgConnectionMap(string address, unsigned short port)
{
  XGLOG_WARN( "xGateMBCUtil::isPresentInMgConnectionMap Address :%s"
      ,address.c_str());


  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgConnectionMapLock, false);

  for(unsigned int i = 0; i< m_mgConnectionMap.size();i++)
  {
    if(m_mgConnectionMap[i].ipAddress == address
        && m_mgConnectionMap[i].port == port)
    {

      XGLOG_WARN( "xGateMBCUtil::isPresentInMgConnectionMap" \
          " entry is Present For Address :%s:%d",address.c_str(),port);
      return true;
    }
  }
  return false;
}

void xGateMBCUtil::addToMgConnectionMap(xGateTCPConTuple tuple)
{
  XGLOG_WARN( "xGateMBCUtil::addToMgConnectionMap Add the entry"\
      " Address :%s:%d Fd:%d",tuple.ipAddress.c_str(),tuple.port,tuple.fd);
  ACE_GUARD(ACE_Thread_Mutex,guard, m_mgConnectionMapLock);
  m_mgConnectionMap.push_back(tuple);
  XGLOG_WARN( "xGateMBCUtil::addToMgConnectionMap");
}
#endif

xGateRetVal xGateMBCUtil::getFromMgSfuConnectionMap(string address, unsigned short port, int& fd)
{

  XGLOG_WARN( "xGateMBCUtil::getFromMgConnectionMap Fetch the entry"
      " for address :%s port:%d",address.c_str(),port);

  xGateRetVal retVal = EN_XGATE_STATUS_NOT_PRESENT;
  unsigned int index = 0;


  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgSfuConnectionMapLock, EN_XGATE_STATUS_ERROR);


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
    XGLOG_ERROR( "xGateMBCUtil::getFromMgConnectionMap No entry found"
        " for Address:%s Port:%d",address.c_str(),port);
    retVal = EN_XGATE_STATUS_NOT_PRESENT;
  }
  XGLOG_WARN( "Exit xGateMBCUtil::getFromMgConnectionMap");
  return retVal;
}

bool xGateMBCUtil::isPresentInMgSfuConnectionMap(string address, unsigned short port)
{
  XGLOG_WARN( "xGateMBCUtil::isPresentInMgConnectionMap Address :%s"
      ,address.c_str());


  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgSfuConnectionMapLock, false);

  for(unsigned int i = 0; i< m_mgConnectionMap.size();i++)
  {
    if(m_mgConnectionMap[i].ipAddress == address
        && m_mgConnectionMap[i].port == port)
    {

      XGLOG_WARN( "xGateMBCUtil::isPresentInMgConnectionMap" \
          " entry is Present For Address :%s:%d",address.c_str(),port);
      return true;
    }
  }
  return false;
}

#if 1
void xGateMBCUtil::getTimerIdFromConnMap (int fd, xGateTCPConnectionType eConnType,
    long int &timerId)
{
  XGLOG_INFO( "Enter: xGateMBCUtil::getTimerIdFromConnMap");
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
    XGLOG_ERROR( "xGateMBCUtil::getTimerIdFromConnMap Invalid eConnType %d", eConnType);
  }
  XGLOG_INFO( "Exit: xGateMBCUtil::getTimerIdFromConnMap");
}



void xGateMBCUtil::removeFromMgConnectionMap(xGateTCPConTuple tuple)
{
  XGLOG_WARN( "Enter xGateMBCUtil::removeFromMgConnectionMap"\
      " Address:%s:%d and Fd:%d", tuple.ipAddress.c_str(), tuple.port, tuple.fd);

  ACE_GUARD(ACE_Thread_Mutex, guard, m_mgConnectionMapLock);

  for(unsigned int i = 0; i<m_mgConnectionMap.size();i++)
  {
    if(m_mgConnectionMap[i] == tuple)
    {
      m_mgConnectionMap.erase(m_mgConnectionMap.begin()+i);
      XGLOG_WARN( "xGateMBCUtil::removeFromMgConnectionMap erased the entry");
      break;
    }
  }
  XGLOG_WARN( "Exit  xGateMBCUtil::removeFromMgConnectionMap");
}
#endif


xGateRetVal xGateMBCUtil::handle_media_req(Value& dataMemObj, MgMediaDetail& clientMediaDetail) 
{
  XGLOG_INFO( "Enter xGateMBCUtil::handle_media_req"); 
  Value::ConstMemberIterator itr = dataMemObj.MemberBegin();
  Value::ConstMemberIterator itrEnd = dataMemObj.MemberEnd();

  for (itr; itr != itrEnd; ++itr) {
    if(!strcmp(itr->name.GetString(), "mgresource_id")) {
      clientMediaDetail.mgresource_id = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "call_type")) {
      clientMediaDetail.call_type = itr->value.GetUint();
    } else if(!strcmp(itr->name.GetString(), "call_id")) {
      clientMediaDetail.call_id = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "context_id")) {
      clientMediaDetail.context_id = itr->value.GetInt();  
    } else if(!strcmp(itr->name.GetString(), "leg_id")) {
      clientMediaDetail.leg_id = itr->value.GetUint();  
    } else if(!strcmp(itr->name.GetString(), "retrieved_audiossrc")) {
      clientMediaDetail.retrieved_audiossrc = itr->value.GetUint();
    }else if(!strcmp(itr->name.GetString(), "retrieved_videossrc")) {
      clientMediaDetail.retrieved_videossrc = itr->value.GetUint();
    } else if(!strcmp(itr->name.GetString(), "media_proto")) {
      clientMediaDetail.media_proto = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "out_proto")) {
      clientMediaDetail.out_proto = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "record_file")) {
      clientMediaDetail.record_file = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "browser_type")) {
      clientMediaDetail.browser_type = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "sig_type")) {
      clientMediaDetail.sig_type = itr->value.GetUint(); 
      continue;
    } else if(!strcmp(itr->name.GetString(), "call_dir")) {
      clientMediaDetail.call_dir = itr->value.GetUint(); 
      continue;
    } else if(!strcmp(itr->name.GetString(), "d_r_e")) {
      clientMediaDetail.d_rtp_ep = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "dtmf_digits")) {
      clientMediaDetail.dtmf_digits = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "joiner_type")) {
      clientMediaDetail.joiner_type = itr->value.GetUint();
      continue;
    }  else if(!strcmp(itr->name.GetString(), "sdp_str")) {
      clientMediaDetail.sdp_str = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "media_state")) {
      clientMediaDetail.media_event = itr->value.GetUint(); 
    } else if(!strcmp(itr->name.GetString(),"sdpinfo")) {
      // Call a function will decode the array details
      handle_sdpinfo_process(itr->value,clientMediaDetail);
      }
    }
  XGLOG_INFO( "xGateMBCUtil::handle_media_req successfully handled");
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateMBCUtil::handle_sdpinfo_process(const Value& sdpObj, MgMediaDetail& clientMediaDetail) 
{
  if(!sdpObj.IsArray()){
    XGLOG_INFO( " xGateMBCUtil::handle_sdpinfo_resp Non-Array Object");
    return EN_XGATE_STATUS_ERROR;
  }
  for (SizeType i = 0; i < sdpObj.Size(); i++){
    if(sdpObj[i].IsObject()){
      if(i == 0) {
        //clientMediaDetail.audioDetail.m_mediaType =  2;//sdp_media_audio
        handle_sdpinfo_audio(sdpObj, clientMediaDetail);
      } else if( i == 1) {
      //clientMediaDetail.audioDetail.m_mediaType =  3;//sdp_media_video
      handle_sdpinfo_video(sdpObj, clientMediaDetail);
      }
    }
  }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateMBCUtil::handle_sdpinfo_audio(const Value& sdpObj, MgMediaDetail& clientMediaDetail)
{
  unsigned int i = 0;

  for (Value::ConstMemberIterator itr = sdpObj[i].MemberBegin(); itr != sdpObj[i].MemberEnd(); itr++){
    if(!strcmp(itr->name.GetString(), "media_type")) {
      clientMediaDetail.audioDetail.m_mediaType = itr->value.GetUint();
    }
    if(clientMediaDetail.audioDetail.m_mediaType == 0) {
      break;
    }  

    if(!strcmp(itr->name.GetString(), "fmtp")) {
      clientMediaDetail.audioDetail.m_fmtp = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "codec")) {
      clientMediaDetail.audioDetail.m_codec = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "codec_name")) {
      clientMediaDetail.audioDetail.m_codecName = itr->value.GetString();
      string s1("opus");
      string s2("OPUS");
      if(s1.compare(clientMediaDetail.audioDetail.m_codecName)== 0 || s2.compare(clientMediaDetail.audioDetail.m_codecName)== 0)
      {
         clientMediaDetail.audioDetail.m_dynamicCodec = clientMediaDetail.audioDetail.m_codec;
         //let internal mapping be same as old one i.e. for opus
         //hard code to "111" and for external interfaces with client like
         //udpSrc use dynamic codec 96-127 as received from JSON message in "m_codec"
         clientMediaDetail.audioDetail.m_codec = EN_CODEC_OPUS;
      }
      continue;  
    } else if(!strcmp(itr->name.GetString(), "ptime")) {
      clientMediaDetail.audioDetail.m_ptime = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "play_file")) {
      clientMediaDetail.audioDetail.m_playFile = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ip_addr")) {
      clientMediaDetail.audioDetail.m_clientIp = itr->value.GetString();
      continue;
    } else if (!strcmp(itr->name.GetString(), "ip_port")) {
      clientMediaDetail.audioDetail.m_clientPort = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "relayip_addr")) {
      clientMediaDetail.audioDetail.m_relayIp = itr->value.GetString();
      continue;
    } else if (!strcmp(itr->name.GetString(), "relay_port")) {
      clientMediaDetail.audioDetail.m_relayPort = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "reflexip_addr")) {
      clientMediaDetail.audioDetail.m_rflxIp = itr->value.GetString();
      continue;
    } else if (!strcmp(itr->name.GetString(), "reflex_port")) {
      clientMediaDetail.audioDetail.m_rflxPort = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ice_ufrag")) {
      clientMediaDetail.audioDetail.m_secureDetail.m_rIceUfrag = itr->value.GetString();
      //for MBC make local=remote so that, SFU can copy from local to remote
      clientMediaDetail.audioDetail.m_secureDetail.m_lIceUfrag = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ice_pwd")) {
      clientMediaDetail.audioDetail.m_secureDetail.m_rIcePwd = itr->value.GetString();
      //for MBC make local=remote so that, SFU can copy from local to remote
      clientMediaDetail.audioDetail.m_secureDetail.m_lIcePwd = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "fingerprint")) {
      clientMediaDetail.audioDetail.m_secureDetail.m_rFingerPrint = itr->value.GetString();
      //for MBC make local=remote so that, SFU can copy from local to remote
      clientMediaDetail.audioDetail.m_secureDetail.m_lFingerPrint = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ssrc")) {
      clientMediaDetail.audioDetail.m_secureDetail.m_rSsrc = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "cname")) {
      clientMediaDetail.audioDetail.m_secureDetail.m_rCname = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mslable")) {
      clientMediaDetail.audioDetail.m_secureDetail.m_rMsLabel = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "lable")) {
      clientMediaDetail.audioDetail.m_secureDetail.m_rLabel = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "dial_ip")) {
      clientMediaDetail.audioDetail.m_dialIp = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "dial_port")) {
      clientMediaDetail.audioDetail.m_dialPort = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "resp_ip")) {
      clientMediaDetail.audioDetail.m_respIp = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "resp_port")) {
      clientMediaDetail.audioDetail.m_respPort = itr->value.GetUint();
      continue;
      } 
    }
  
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateMBCUtil::handle_sdpinfo_video(const Value& sdpObj, MgMediaDetail& clientMediaDetail)
{
  unsigned int i = 1;

  for (Value::ConstMemberIterator itr = sdpObj[i].MemberBegin(); itr != sdpObj[i].MemberEnd(); itr++){

    if(!strcmp(itr->name.GetString(), "media_type")) {
      clientMediaDetail.videoDetail.m_mediaType = itr->value.GetUint();
    }

    if(clientMediaDetail.videoDetail.m_mediaType == 0) {
      break;
    }

    if(!strcmp(itr->name.GetString(), "fmtp")) {
      clientMediaDetail.videoDetail.m_fmtp = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "codec")) {
      clientMediaDetail.videoDetail.m_codec = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "codec_name")) {
      clientMediaDetail.videoDetail.m_codecName = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "framerate")) {
      clientMediaDetail.videoDetail.m_frameRate = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "imageattr_x")) {
      clientMediaDetail.videoDetail.m_imageattr_x = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "imageattr_y")) {
      clientMediaDetail.videoDetail.m_imageattr_y = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "play_file")) {
      clientMediaDetail.videoDetail.m_playFile = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ip_addr")) {
      clientMediaDetail.videoDetail.m_clientIp = itr->value.GetString();
      continue;
    } else if (!strcmp(itr->name.GetString(), "ip_port")) {
      clientMediaDetail.videoDetail.m_clientPort = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "relayip_addr")) {
      clientMediaDetail.videoDetail.m_relayIp = itr->value.GetString();
      continue;
    } else if (!strcmp(itr->name.GetString(), "relay_port")) {
      clientMediaDetail.videoDetail.m_relayPort = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "reflexip_addr")) {
      clientMediaDetail.videoDetail.m_rflxIp = itr->value.GetString();
      continue;
    } else if (!strcmp(itr->name.GetString(), "reflex_port")) {
      clientMediaDetail.videoDetail.m_rflxPort = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ice_ufrag")) {
      clientMediaDetail.videoDetail.m_secureDetail.m_rIceUfrag = itr->value.GetString();
      //for MBC make local=remote so that, SFU can copy from local to remote
      clientMediaDetail.videoDetail.m_secureDetail.m_lIceUfrag = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ice_pwd")) {
      clientMediaDetail.videoDetail.m_secureDetail.m_rIcePwd = itr->value.GetString();
      //for MBC make local=remote so that, SFU can copy from local to remote
      clientMediaDetail.videoDetail.m_secureDetail.m_lIcePwd = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "fingerprint")) {
      clientMediaDetail.videoDetail.m_secureDetail.m_rFingerPrint = itr->value.GetString();
      //for MBC make local=remote so that, SFU can copy from local to remote
      clientMediaDetail.videoDetail.m_secureDetail.m_lFingerPrint = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "ssrc")) {
      clientMediaDetail.videoDetail.m_secureDetail.m_rSsrc = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "cname")) {
      clientMediaDetail.videoDetail.m_secureDetail.m_rCname = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "mslable")) {
      clientMediaDetail.videoDetail.m_secureDetail.m_rMsLabel = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "lable")) {
      clientMediaDetail.videoDetail.m_secureDetail.m_rLabel = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "dial_ip")) {
      clientMediaDetail.videoDetail.m_dialIp = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "dial_port")) {
      clientMediaDetail.videoDetail.m_dialPort = itr->value.GetUint();
      continue;
    } else if(!strcmp(itr->name.GetString(), "resp_ip")) {
      clientMediaDetail.videoDetail.m_respIp = itr->value.GetString();
      continue;
    } else if(!strcmp(itr->name.GetString(), "resp_port")) {
      clientMediaDetail.videoDetail.m_respPort = itr->value.GetUint();
      continue;
    }
  }
   // Incase codec number does not match codec name
   if( clientMediaDetail.videoDetail.m_codecName.compare("H264") == 0 || clientMediaDetail.videoDetail.m_codecName.compare("h264") == 0){
     clientMediaDetail.videoDetail.m_dynamicCodec = clientMediaDetail.videoDetail.m_codec;
     clientMediaDetail.videoDetail.m_codec = EN_CODEC_H264;
   } else if( clientMediaDetail.videoDetail.m_codecName.compare("H265") == 0 || clientMediaDetail.videoDetail.m_codecName.compare("h265") == 0 ){
      clientMediaDetail.videoDetail.m_dynamicCodec = clientMediaDetail.videoDetail.m_codec;
      clientMediaDetail.videoDetail.m_codec = EN_CODEC_H265;
   } else if( clientMediaDetail.videoDetail.m_codecName.compare("VP8") == 0 || clientMediaDetail.videoDetail.m_codecName.compare("vp8") == 0 ){
      clientMediaDetail.videoDetail.m_dynamicCodec = clientMediaDetail.videoDetail.m_codec;
      clientMediaDetail.videoDetail.m_codec = EN_CODEC_VP8;
   } else if( clientMediaDetail.videoDetail.m_codecName.compare("VP9") == 0 || clientMediaDetail.videoDetail.m_codecName.compare("vp9") == 0){
      clientMediaDetail.videoDetail.m_dynamicCodec = clientMediaDetail.videoDetail.m_codec;
      clientMediaDetail.videoDetail.m_codec = EN_CODEC_VP9;
   }
  return EN_XGATE_STATUS_SUCCESS;
}

int xGateMBCUtil::create_media_msg(MgMediaDetail& mediaDetail, string &jsonBuff, xGateMgMsgType mgMsgType)
{
    XGLOG_INFO( "@@SFU Enter xGateMGDispatcher::create_media_msg");
    Document doc;
    StringBuffer s;
    Writer<StringBuffer> writer(s);
    XGLOG_INFO("xGateMBCUtil::create_media_msg %s",mediaDetail.sdp_str.c_str());

    int len = 0;
    len = sizeof(json_mg_media_tmpl);
    char buff[len];
    memcpy(buff, json_mg_media_tmpl, len);

    if (doc.ParseInsitu(buff).HasParseError()) {
        XGLOG_ERROR( "xGateMGDispatcher::create_media_msg json parse error");
        return EN_XGATE_STATUS_ERROR;
    }
    if(!doc.IsObject()) {
        XGLOG_ERROR( "xGateMGDispatcher::create_media_msg doc object creation failed");
        return EN_XGATE_STATUS_ERROR;
    }

    Value valMsgType;
    valMsgType = mgMsgType;
    doc["msg_type"] = valMsgType;
    doc["data"]["mgresource_id"] = setJsonParam(mediaDetail.mgresource_id,doc);
    doc["data"]["call_type"] = setJsonParam(mediaDetail.call_type,doc);
    doc["data"]["call_id"] = setJsonParam(mediaDetail.call_id,doc);
    doc["data"]["gateway_id"] = setJsonParam(mediaDetail.gateway_id,doc);
    doc["data"]["context_id"] = setJsonParam(mediaDetail.context_id,doc);
    doc["data"]["leg_id"] = setJsonParam(mediaDetail.leg_id,doc);
    doc["data"]["media_proto"] = setJsonParam(mediaDetail.media_proto,doc);
    doc["data"]["out_proto"] = setJsonParam(mediaDetail.out_proto,doc);
    doc["data"]["record_file"] = setJsonParam(mediaDetail.record_file,doc);
    doc["data"]["browser_type"] = setJsonParam(mediaDetail.browser_type,doc);
    doc["data"]["file_size"] = setJsonParam(mediaDetail.file_size,doc);
    doc["data"]["sig_type"] = setJsonParam(mediaDetail.sig_type,doc);
    doc["data"]["call_dir"]= setJsonParam(mediaDetail.call_dir,doc);
    doc["data"]["d_r_e"]= setJsonParam(mediaDetail.d_rtp_ep,doc);
    doc["data"]["dtmf_digits"]= setJsonParam(mediaDetail.dtmf_digits,doc);
    doc["data"]["joiner_type"]= setJsonParam(mediaDetail.joiner_type,doc);
    doc["data"]["media_state"] = setJsonParam(mediaDetail.media_event,doc);
    doc["data"]["sdp_str"] = setJsonParam(mediaDetail.sdp_str,doc);
    doc["data"]["sdpinfo"] = setJsonParam(mediaDetail,doc);
    doc["data"]["call_Id_msg10"]= setJsonParam(mediaDetail.call_Id_msg10,doc);
    doc["data"]["pClient"]= setJsonParam(mediaDetail.pClient,doc);
    doc.Accept(writer);
    //get the length of created message
    len = s.GetLength();

    if ( len > 999) {
        doc["msg_len"] = len+1;
    } else {
        doc["msg_len"] = len;
    }
    s.Clear();
    doc.Accept(writer);
    printf("\n***********************************\n");
    puts(s.GetString());
    jsonBuff = s.GetString();
    string delimiter("\r\n\r\n");
    jsonBuff.append(delimiter.c_str(), delimiter.length());
    printf("***********************************\n");
    XGLOG_ERROR( "xGateMBCUtil::create_media_msg created media msg  with length of (%d), media msg: %s", jsonBuff.length(), jsonBuff.c_str());
    return jsonBuff.length();
}

Value xGateMBCUtil::setJsonParam(string strValue,Document &doc ){
    Value retVal;
    retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
    return retVal;
}

Value xGateMBCUtil::setJsonParam(unsigned int iValue, Document &doc){
    Value retVal;
    retVal = iValue;
    return retVal;
}

Value xGateMBCUtil::setJsonParam(MgMediaDetail& media, Document &doc){
    Value vArray(kArrayType);
    Document::AllocatorType& allocate =  doc.GetAllocator();
    for (int i =0 ; i< 2 ; i++){
        Value objectValue;
        objectValue.SetObject();
        if(i == 0 && media.audioDetail.m_mediaType == 2) {
            objectValue.AddMember("media_type",setJsonParam(media.audioDetail.m_mediaType,doc),allocate);
            objectValue.AddMember("fmtp",setJsonParam(media.audioDetail.m_fmtp,doc),allocate);
            objectValue.AddMember("codec",setJsonParam(media.audioDetail.m_codec,doc),allocate);
            objectValue.AddMember("codec_name",setJsonParam(media.audioDetail.m_codecName,doc),allocate);
            objectValue.AddMember("ptime",setJsonParam(media.audioDetail.m_ptime,doc),allocate);
            objectValue.AddMember("play_file",setJsonParam(media.audioDetail.m_playFile,doc),allocate);
            objectValue.AddMember("ip_addr",setJsonParam(media.audioDetail.m_clientIp,doc),allocate);
            objectValue.AddMember("ip_port",setJsonParam(media.audioDetail.m_clientPort,doc),allocate);
            objectValue.AddMember("relayip_addr",setJsonParam(media.audioDetail.m_relayIp,doc),allocate);
            objectValue.AddMember("relay_port",setJsonParam(media.audioDetail.m_relayPort,doc),allocate);
            objectValue.AddMember("reflexip_addr",setJsonParam(media.audioDetail.m_rflxIp,doc),allocate);
            objectValue.AddMember("reflex_port",setJsonParam(media.audioDetail.m_rflxPort,doc),allocate);
            objectValue.AddMember("ice_ufrag",setJsonParam(media.audioDetail.m_secureDetail.m_lIceUfrag,doc),allocate);
            objectValue.AddMember("ice_pwd",setJsonParam(media.audioDetail.m_secureDetail.m_lIcePwd,doc),allocate);
            objectValue.AddMember("fingerprint",setJsonParam(media.audioDetail.m_secureDetail.m_lFingerPrint,doc),allocate);
            objectValue.AddMember("ssrc",setJsonParam(media.audioDetail.m_secureDetail.m_lSsrc,doc),allocate);
            objectValue.AddMember("cname",setJsonParam(media.audioDetail.m_secureDetail.m_lCname,doc),allocate);
            objectValue.AddMember("mslable",setJsonParam(media.audioDetail.m_secureDetail.m_lMsLabel,doc),allocate);
            objectValue.AddMember("lable",setJsonParam(media.audioDetail.m_secureDetail.m_lLabel,doc),allocate);
            objectValue.AddMember("dial_ip",setJsonParam(media.audioDetail.m_dialIp,doc),allocate);
            objectValue.AddMember("dial_port",setJsonParam(media.audioDetail.m_dialPort,doc),allocate);
            objectValue.AddMember("resp_ip",setJsonParam(media.audioDetail.m_respIp,doc),allocate);
            objectValue.AddMember("resp_port",setJsonParam(media.audioDetail.m_respPort,doc),allocate);
            vArray.PushBack(objectValue,allocate);
        } else if(i == 1 && media.videoDetail.m_mediaType == 3) {
            objectValue.AddMember("media_type",setJsonParam(media.videoDetail.m_mediaType,doc),allocate);
            objectValue.AddMember("fmtp",setJsonParam(media.videoDetail.m_fmtp,doc),allocate);
            objectValue.AddMember("codec",setJsonParam(media.videoDetail.m_codec,doc),allocate);
            objectValue.AddMember("codec_name",setJsonParam(media.videoDetail.m_codecName,doc),allocate);
            objectValue.AddMember("framerate",setJsonParam(media.videoDetail.m_frameRate,doc),allocate);
            objectValue.AddMember("imageattr_x",setJsonParam(media.videoDetail.m_imageattr_x,doc),allocate);
            objectValue.AddMember("imageattr_y",setJsonParam(media.videoDetail.m_imageattr_y,doc),allocate);
            objectValue.AddMember("play_file",setJsonParam(media.videoDetail.m_playFile,doc),allocate);
            objectValue.AddMember("ip_addr",setJsonParam(media.videoDetail.m_clientIp,doc),allocate);
            objectValue.AddMember("ip_port",setJsonParam(media.videoDetail.m_clientPort,doc),allocate);
            objectValue.AddMember("relayip_addr",setJsonParam(media.videoDetail.m_relayIp,doc),allocate);
            objectValue.AddMember("relay_port",setJsonParam(media.videoDetail.m_relayPort,doc),allocate);
            objectValue.AddMember("reflexip_addr",setJsonParam(media.videoDetail.m_rflxIp,doc),allocate);
            objectValue.AddMember("reflex_port",setJsonParam(media.videoDetail.m_rflxPort,doc),allocate);
            objectValue.AddMember("ice_ufrag",setJsonParam(media.videoDetail.m_secureDetail.m_lIceUfrag,doc),allocate);
            objectValue.AddMember("ice_pwd",setJsonParam(media.videoDetail.m_secureDetail.m_lIcePwd,doc),allocate);
            objectValue.AddMember("fingerprint",setJsonParam(media.videoDetail.m_secureDetail.m_lFingerPrint,doc),allocate);
            objectValue.AddMember("ssrc",setJsonParam(media.videoDetail.m_secureDetail.m_lSsrc,doc),allocate);
            objectValue.AddMember("cname",setJsonParam(media.videoDetail.m_secureDetail.m_lCname,doc),allocate);
            objectValue.AddMember("mslable",setJsonParam(media.videoDetail.m_secureDetail.m_lMsLabel,doc),allocate);
            objectValue.AddMember("lable",setJsonParam(media.videoDetail.m_secureDetail.m_lLabel,doc),allocate);
            objectValue.AddMember("dial_ip",setJsonParam(media.videoDetail.m_dialIp,doc),allocate);
            objectValue.AddMember("dial_port",setJsonParam(media.videoDetail.m_dialPort,doc),allocate);
            objectValue.AddMember("resp_ip",setJsonParam(media.videoDetail.m_respIp,doc),allocate);
            objectValue.AddMember("resp_port",setJsonParam(media.videoDetail.m_respPort,doc),allocate);
            vArray.PushBack(objectValue,allocate);
        }
    }     
    return vArray;
}

