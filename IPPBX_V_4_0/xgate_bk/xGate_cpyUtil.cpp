//local includes
#include "log.h"
#include "klog.h"
#include "xGateUtil.h"
#include <netinet/in.h>
#include <netdb.h>
#define THISMODULE "Util"

vector<xGateTCPConTuple>xGateUtil::m_mgConnectionMap;
ACE_Thread_Mutex xGateUtil::m_mgConnectionMapLock;
xGateTCPReceiver* xGateUtil::m_mgReceiver = NULL;
xGateMgProcessor* xGateUtil::m_mgProcessor = NULL;
xGateMGDispatcher* xGateUtil::m_mgDispatcher=NULL;
xGateTCPSocketHandler* xGateUtil::m_tcpHandler=NULL;
xGateMGCConnector* xGateUtil::m_mgcConnector=NULL;
vector<string> xGateUtil::m_MgcIpAddressList;
char *xGateUtil::LocalIPadress = NULL;

#ifdef XGATE_HAS_GSTREAMER
xGateHmpProcessor* xGateUtil::m_hmpProcessor = NULL;
xGateHmpGstManager* xGateUtil::m_hmpGstManager = NULL;
#endif

// Interface to add to MgConnection Map
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
    CLog::Detail(THISMODULE, "xGateUtil::SetLocalIPAddress as %s", LocalIPadress);
  }
}

void xGateUtil::addToMgConnectionMap(xGateTCPConTuple tuple)
{
  CLog::Warning(THISMODULE, "xGateUtil::addToMgConnectionMap Add the entry"\
      " Address :%s:%d Fd:%d",tuple.ipAddress.c_str(),tuple.port,tuple.fd);
  ACE_GUARD(ACE_Thread_Mutex,guard, m_mgConnectionMapLock);
  m_mgConnectionMap.push_back(tuple);
  CLog::Warning(THISMODULE, "xGateUtil::addToMgConnectionMap");
}

xGateRetVal xGateUtil::getFromMgConnectionMap(string address, unsigned short port, int& fd)
{

  CLog::Warning(THISMODULE, "xGateUtil::getFromMgConnectionMap Fetch the entry"
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
    CLog::Error(THISMODULE, "xGateUtil::getFromMgConnectionMap No entry found"
      " for Address:%s Port:%d",address.c_str(),port);
 retVal = EN_XGATE_STATUS_NOT_PRESENT;
  }
  CLog::Warning(THISMODULE, "Exit xGateUtil::getFromMgConnectionMap");
  return retVal;
}

bool xGateUtil::isPresentInMgConnectionMap(string address, unsigned short port)
{
  CLog::Warning(THISMODULE, "xGateUtil::isPresentInMgConnectionMap Address :%s"
      ,address.c_str());

  
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgConnectionMapLock, false);

  for(unsigned int i = 0; i< m_mgConnectionMap.size();i++)
  {
    if(m_mgConnectionMap[i].ipAddress == address
        && m_mgConnectionMap[i].port == port)
    {
      
      CLog::Warning(THISMODULE, "xGateUtil::isPresentInMgConnectionMap" \
          " entry is Present For Address :%s:%d",address.c_str(),port);
      return true;
    }
  }
  return false;
}


void xGateUtil::getTimerIdFromConnMap (int fd, xGateTCPConnectionType eConnType,
                                            long int &timerId)
{
    CLog::Detail(THISMODULE, "Enter: xGateUtil::getTimerIdFromConnMap");
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
        CLog::Error(THISMODULE, "xGateUtil::getTimerIdFromConnMap Invalid eConnType %d", eConnType);
    }
    CLog::Detail(THISMODULE, "Exit: xGateUtil::getTimerIdFromConnMap");
}



void xGateUtil::removeFromMgConnectionMap(xGateTCPConTuple tuple)
{
  CLog::Warning(THISMODULE, "Enter xGateUtil::removeFromMgConnectionMap"\
      " Address:%s:%d and Fd:%d", tuple.ipAddress.c_str(), tuple.port, tuple.fd);

  ACE_GUARD(ACE_Thread_Mutex, guard, m_mgConnectionMapLock);

  for(unsigned int i = 0; i<m_mgConnectionMap.size();i++)
  {
    if(m_mgConnectionMap[i] == tuple)
    {
      m_mgConnectionMap.erase(m_mgConnectionMap.begin()+i);
      CLog::Warning(THISMODULE, "xGateUtil::removeFromMgConnectionMap erased the entry");
      break;
    }
  }
  CLog::Warning(THISMODULE, "Exit  xGateUtil::removeFromMgConnectionMap");
}
