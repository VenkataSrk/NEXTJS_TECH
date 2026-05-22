#ifndef _XGATE_AUDIT_SCHEDULER_H
#define _XGATE_AUDIT_SCHEDULER_H

//local includes
#include "xGateUtil.h"
#include "xGateScheduler.h"

#define AUDITSCHEDULER AuditScheduler::instance()
class xGateAuditScheduler : public xGateScheduler
{
	public:
		xGateAuditScheduler();
		virtual ~xGateAuditScheduler();

		/// Called back on handle_time out
		virtual int handle_timeout(const ACE_Time_Value&, const void *); 
};

typedef ACE_Unmanaged_Singleton<xGateAuditScheduler, ACE_Null_Mutex> AuditScheduler;
#endif
