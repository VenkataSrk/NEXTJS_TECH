#include "xglog.h"
#include "NetIOServiceUtil.h"

NetIOServiceUtil::NetIOServiceUtil()
{
}

NetIOServiceUtil::~NetIOServiceUtil(void)
{
}

ACE_Thread_Mutex NetIOServiceUtil::m_sessionDetailMapLock;
void NetIOServiceUtil::addToMgConnectionMap(TcpConTuple& tuple)
{
  XGLOG_DEBUG("added peer connection %s:%d received on fd: %d in media-gateway connection map", \
      tuple.m_remoteIp.c_str(), tuple.m_remotePort, tuple.m_fd);
  /// Acquire the Lock
  ACE_GUARD(ACE_Thread_Mutex,guard, m_mgConnectionMapLock);
  m_mgConnectionMap.push_back(tuple);
}

bool NetIOServiceUtil::getFromMgConnectionMap(const unsigned short& index, unsigned short& fd)
{
  XGLOG_DEBUG("fetching the fd detail for given index: %d from media-gateway connection map", index);
  unsigned int connSize = m_mgConnectionMap.size();
  if(connSize <= 0 || index > connSize) {
    XGLOG_WARN("no media-gateway connection avalible in map for index: %d !", index);
    return false;
  }

  //Acquire the NW Map Lock
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgConnectionMapLock, EN_XGATE_STATUS_ERROR);
  fd = m_mgConnectionMap[index].m_fd;
  if(fd > 0) {
    return true;
  }
  return false;
}

int NetIOServiceUtil::getFromMgConnectionMap(string& ipaddr)
{
  XGLOG_DEBUG("fetching the index detail for given ip: %s from media-gateway connection map", \
      ipaddr.c_str());
  unsigned int index = 0;
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgConnectionMapLock, EN_XGATE_STATUS_ERROR);
  for(index = 0; index < m_mgConnectionMap.size();index++) {
    if(m_mgConnectionMap[index].m_remoteIp == ipaddr) {
      return index;
    }
  }
  return 0;
}

bool NetIOServiceUtil::getFromMgConnectionMap(string& ip, unsigned short port, int& fd)
{
  XGLOG_DEBUG("fetching fd detail for given address %s:%d from media-gateway connection map", \
      ip.c_str(), port);

  bool retVal = false;
  unsigned int index = 0;

  /// Acquire the NW Map Lock
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgConnectionMapLock, EN_XGATE_STATUS_ERROR);

  /// Iterate throw till the entry is found
  for(index = 0; index < m_mgConnectionMap.size();index++) {
    if(m_mgConnectionMap[index].m_remoteIp == ip) {
      /// Need to check if we have to match port also
      if( port == 0 || m_mgConnectionMap[index].m_remotePort == port) {
        retVal = true;
        break;
      }
    }
  }

  if(retVal == EN_XGATE_STATUS_SUCCESS) {
    fd = m_mgConnectionMap[index].m_fd;
  } else {
    XGLOG_WARN("no connection entry found for given address %s:%d !", \
        ip.c_str(), port);
    retVal = false;
  }
  return retVal;
}

void NetIOServiceUtil::removeFromMgConnectionMap(TcpConTuple& tuple)
{
  XGLOG_DEBUG("removing entry from media-gateway connection map for the given address %s:%d and fd: %d", \
      tuple.m_remoteIp.c_str(), tuple.m_remotePort, tuple.m_fd);

  /// Acquire the lock
  ACE_GUARD(ACE_Thread_Mutex, guard, m_mgConnectionMapLock);
  for(unsigned int i = 0; i<m_mgConnectionMap.size();i++) {
    if(m_mgConnectionMap[i] == tuple) {
      m_mgConnectionMap.erase(m_mgConnectionMap.begin()+i);
      XGLOG_INFO("entry for the given address %s:%d removed from media-gateway connection map", \
          tuple.m_remoteIp.c_str(), tuple.m_remotePort);
      return;
    }
  }
  XGLOG_WARN("remove entry failed. no connection entry found in map for the given address %s:%d and fd: %d", \
      tuple.m_remoteIp.c_str(), tuple.m_remotePort, tuple.m_fd);
  return;
}

bool NetIOServiceUtil::isPresentInMgConnectionMap(string& ip, unsigned short port)
{
  XGLOG_DEBUG("checking the entry present in media-gateway connection map for the address %s:%d", \
      ip.c_str(), port);
  ///Acquire the lock
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_mgConnectionMapLock, false);
  for(unsigned int i = 0; i< m_mgConnectionMap.size();i++) {
    if(m_mgConnectionMap[i].m_remoteIp == ip 
        && m_mgConnectionMap[i].m_remotePort == port) {
      /// Entry is Present
      XGLOG_INFO("entry present in connection map for the given address %s:%d", \
          ip.c_str(), port);
      return true;
    }
  }
  XGLOG_WARN("no entry present in connecton map for the address %s:%d", ip.c_str(), port);
  return false;
}

void NetIOServiceUtil::getTimerIdFromConnMap(int fd, long int &timerId)
{
  XGLOG_DEBUG("getting timerId detail for given fd: %d from connection map", fd);
  unsigned int index = 0;
  ACE_GUARD(ACE_Thread_Mutex,guard, m_mgConnectionMapLock);
  for(index = 0; index < m_mgConnectionMap.size();index++) {
    if(m_mgConnectionMap[index].m_fd == fd) {
      timerId = m_mgConnectionMap[index].m_timerId;
      XGLOG_INFO("got timerId: %d for given fd: %d from connection map", \
          timerId, fd);
      return;
    }
  }//end for
  XGLOG_ERROR("timerId detail not able to fetch. entry not found in connection map for fd: %d", fd);
}
bool NetIOServiceUtil::getConnectionId(unsigned int &nextConnId)
{
  XGLOG_INFO("fetching connection id");
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_ConnectionIdLock, EN_XGATE_STATUS_ERROR);
  unsigned int ConnId = m_connectionId;
  std::map<unsigned int, int>::iterator it;
  unsigned int count = UINT_MAX;
  while (count > 0)
  {
    if ((it = ConnMap.find(ConnId)) == ConnMap.end())
    {
      m_connectionId = ConnId + 1;
      ConnMap.insert({ConnId, 0});
      nextConnId = ConnId;
      return true;
    }
    count--;
    ConnId++;
  }
  return false;
}

bool NetIOServiceUtil::removeConnectionId(unsigned int &connId)
{
  map<unsigned int,int>::iterator it;
  ACE_GUARD_RETURN(ACE_Thread_Mutex,guard,m_ConnectionIdLock,EN_XGATE_STATUS_ERROR);
  if((it = ConnMap.find(connId)) != ConnMap.end()){
    ConnMap.erase(connId);
    return true;
  }
  return false;
}

bool NetIOServiceUtil::addToConnTupleInfo(unsigned int connId, TcpConTuple tcpConn)
{
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_ConnTupleLock, EN_XGATE_STATUS_ERROR);
  map<unsigned int, TcpConTuple>::iterator it;
  if ((it = m_ConnMap.find(connId)) != m_ConnMap.end())
  {
    XGLOG_ERROR("Found connection Id already in the map");
    return false;
  }
  m_ConnMap.insert({connId, tcpConn});
  return true;
}

bool NetIOServiceUtil::removeConnTupleInfo(unsigned int connId)
{

  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_ConnTupleLock, EN_XGATE_STATUS_ERROR);
  map<unsigned int, TcpConTuple>::iterator it;
  if ((it = m_ConnMap.find(connId)) != m_ConnMap.end())
  {
    m_ConnMap.erase(connId);
    return true;
  }
  return false;
}

bool NetIOServiceUtil::getConnTupleInfo(unsigned int connId, TcpConTuple &connTuple)
{
  ACE_GUARD_RETURN(ACE_Thread_Mutex, guard, m_ConnTupleLock, EN_XGATE_STATUS_ERROR);
  map<unsigned int, TcpConTuple>::iterator it;
  if ((it = m_ConnMap.find(connId)) != m_ConnMap.end())
  {
    connTuple = m_ConnMap[connId];
    return true;
  }
  XGLOG_ERROR("Could not find ConnTupleInfo for connId:%d", connId);
  return false;
}
