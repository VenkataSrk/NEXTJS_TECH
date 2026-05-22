#ifndef _XGATE_AUDIT_UTIL_H
#define _XGATE_AUDIT_UTIL_H

//ace includes
#include <ace/Task.h>
#include <ace/Singleton.h>

//local includes
#include "xGateUtil.h"
#include "xGateAuditDefine.h"
#include "xGateBaseMsg.h"
#include "xglog.h"

#define AUDITUTIL AuditUtil::instance()
class xGateAuditUtil : public ACE_Task<ACE_MT_SYNCH> {
  public:
    xGateAuditUtil();
    ~xGateAuditUtil(void);
        
    long int scheduleTimer(xGateSchedulerData *data, unsigned int timeoutSecond);
    bool cancelTimer(long int timerId, string& uid);
    bool postToAudit(xGateBaseMsg *pMsg);
    bool postToHmpProcessor(xGateBaseMsg *pMsg);
    unsigned short getAuditStartTime();
    void setAuditStartTime(unsigned short value);
    unsigned short getAuditAttemptCount();
    void setAuditAttemptCount(unsigned short value);

  private:
    unsigned short m_auditStartTime;
    unsigned short m_auditAttemptCount;
};

typedef ACE_Unmanaged_Singleton<xGateAuditUtil, ACE_Null_Mutex> AuditUtil;
#endif

