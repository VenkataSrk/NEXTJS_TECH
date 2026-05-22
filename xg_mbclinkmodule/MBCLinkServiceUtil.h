#ifndef _XGATE_MBCLINK_UTIL_H
#define _XGATE_MBCLINK_UTIL_H
//system includes
#include <vector>

//ace includes
#include <ace/Task.h>
#include <ace/Singleton.h>
#include <map>
//local includes
#include "MBCLinkServiceDefine.h"
#include "xGateUtil.h"

#define MBCLINKUTIL MBCLinkUtil::instance()
class MBCLinkServiceUtil : public ACE_Task<ACE_MT_SYNCH> {
  public:
    MBCLinkServiceUtil();
    ~MBCLinkServiceUtil(void);

    //member functions
    void addToMgConnectionMap(TcpConTuple&);
    bool getFromMgConnectionMap(const unsigned short& index, unsigned short& fd);
    bool getFromMgConnectionMap(string& address, unsigned short port, int& fd);
    int getFromMgConnectionMap(string& ipaddr);
    void removeFromMgConnectionMap(TcpConTuple& tuple);
    bool isPresentInMgConnectionMap(string& ip, unsigned short port);
    void getTimerIdFromConnMap(int fd, long int &timerId);
    map<std::string, sessionDetail> m_sessionDetail;
  private:
    vector<TcpConTuple> m_mgConnectionMap;
    ACE_Thread_Mutex m_mgConnectionMapLock;
    static ACE_Thread_Mutex m_sessionDetailMapLock;
	
};


typedef ACE_Unmanaged_Singleton<MBCLinkServiceUtil, ACE_Null_Mutex> MBCLinkUtil;
#endif

