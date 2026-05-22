#ifndef _XGATE_MULTI_SESSION_TRACKER_H
#define _XGATE_MULTI_SESSION_TRACKER_H
//system includes
//ace includes
#include "IURDefines.h"
#include <ace/Task.h>
#include <ace/Singleton.h>
#include <map>
//local includes
using namespace std;

class SessionDetail
{
public:
  SessionDetail(string &uid, IURDefines::MODULE_ID srcModuleId, void *DataHolder) :
  m_uid(uid), m_ptDataHolder(nullptr), m_srcModuleId(srcModuleId){};
  ~SessionDetail(){};

  IURDefines::MODULE_ID m_srcModuleId;
  std::string m_uid;
  void* m_ptDataHolder;// use this to hang the object or additional details .
};

typedef map<std::string, SessionDetail*> SESSION_DETAIL_MAP;
#define SESSIONTRACKER MultiSessionTracker::instance()
class xGateMultiSessionTracker {
        public:
        xGateMultiSessionTracker();
        ~xGateMultiSessionTracker(void);

        void keepSession(SessionDetail *pdetail);
        SessionDetail* accessSession(string& uid);
        void dropSession(string& uid);


        private:
        SESSION_DETAIL_MAP m_sessionDetailMap;
        ACE_Recursive_Thread_Mutex m_sessionDetailMapLock;
};
typedef ACE_Unmanaged_Singleton<xGateMultiSessionTracker, ACE_Null_Mutex>MultiSessionTracker;
#endif

