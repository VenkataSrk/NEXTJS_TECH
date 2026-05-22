#ifndef _XGATE_MEDIA_UTIL_H
#define _XGATE_MEDIA_UTIL_H
//system includes
#include <vector>

//ace includes
#include <ace/Task.h>
#include <ace/Singleton.h>
#include <map>
//local includes
#include "NetIOServiceDefine.h"
#include "xGateUtil.h"

#define NETIOUTIL NetIOUtil::instance()
class NetIOServiceUtil : public ACE_Task<ACE_MT_SYNCH> {
  public:
    NetIOServiceUtil();
    ~NetIOServiceUtil(void);

    //member functions
    void addToMgConnectionMap(TcpConTuple&);
    bool getFromMgConnectionMap(const unsigned short& index, unsigned short& fd);
    bool getFromMgConnectionMap(string& address, unsigned short port, int& fd);
    int getFromMgConnectionMap(string& ipaddr);
    void removeFromMgConnectionMap(TcpConTuple& tuple);
    bool isPresentInMgConnectionMap(string& ip, unsigned short port);
    void getTimerIdFromConnMap(int fd, long int &timerId);
    bool getConnectionId(unsigned int &);
    bool removeConnectionId(unsigned int &);
    bool addToConnTupleInfo(unsigned int, TcpConTuple);
    bool removeConnTupleInfo(unsigned int);
    bool getConnTupleInfo(unsigned int connId, TcpConTuple &tcpConn);

    map<std::string, sessionDetail> m_sessionDetail;
  private:
    vector<TcpConTuple> m_mgConnectionMap;
    ACE_Thread_Mutex m_mgConnectionMapLock;
    ACE_Thread_Mutex m_ConnectionIdLock;
    ACE_Thread_Mutex m_ConnTupleLock;
    unsigned int m_connectionId;
    map<unsigned int,int> ConnMap;
    map<unsigned int,TcpConTuple> m_ConnMap;
    static ACE_Thread_Mutex m_sessionDetailMapLock;
	
};


typedef ACE_Unmanaged_Singleton<NetIOServiceUtil, ACE_Null_Mutex> NetIOUtil;
#endif

