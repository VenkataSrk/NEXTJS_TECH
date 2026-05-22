#include "xglog.h"
#include "xGateMultiSessionTracker.h"

xGateMultiSessionTracker::xGateMultiSessionTracker()
{
}

xGateMultiSessionTracker::~xGateMultiSessionTracker(void)
{
        // TODO. clear the map and clearn session detialss
}


void xGateMultiSessionTracker::keepSession(SessionDetail *details){

        ACE_GUARD(ACE_Recursive_Thread_Mutex,guard, m_sessionDetailMapLock);

        m_sessionDetailMap[details->m_uid] = details;

}

SessionDetail* xGateMultiSessionTracker::accessSession(string& uid){

        //ACE_GUARD(ACE_Recursive_Thread_Mutex,guard, m_sessionDetailMapLock);

        SESSION_DETAIL_MAP::iterator itr = m_sessionDetailMap.find(uid);

        if(m_sessionDetailMap.end() != itr) {
               return itr->second;
        }
#if 0
        SESSION_DETAIL_MAP::iterator itr =m_sessionDetailMap.begin();
        for (itr; itr != m_sessionDetailMap.end(); ++itr)
        {
                if(itr->first == uid)
                {
                        return (itr->second);
                }
        }
#endif

        return NULL;
}

void xGateMultiSessionTracker::dropSession(string &uid){

        ACE_GUARD(ACE_Recursive_Thread_Mutex,guard, m_sessionDetailMapLock);

        SESSION_DETAIL_MAP::iterator itr;
        itr = m_sessionDetailMap.find(uid);

        if(m_sessionDetailMap.end() != itr) {
                m_sessionDetailMap.erase(itr);
        }
}

