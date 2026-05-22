#ifndef _XGATE_AUDIT_SERVICE_H
#define _XGATE_AUDIT_SERVICE_H

//ace includes
#include <ace/Task.h>
#include <ace/Singleton.h>

//local includes
#include "xGateAuditManager.h"

#define AUDITSERVICE  AuditService::instance()
class xGateAuditService :  public ACE_Task<ACE_MT_SYNCH>
{
  public:
    xGateAuditService();
    virtual ~xGateAuditService(void);

    bool init();
    bool deinit();
  private:
    //member functions
    virtual int svc(void);
    bool handle_msg(ACE_Message_Block *pAmb);

    //member variables
    bool m_run;
};

typedef ACE_Unmanaged_Singleton<xGateAuditService, ACE_Null_Mutex> AuditService;
#endif
