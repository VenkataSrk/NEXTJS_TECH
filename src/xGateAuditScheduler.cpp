//local includes
#include "xGateSchedulerMsg.h"
#include "xglog.h"

//self includes
#include "xGateAuditDefine.h"
#include "xGateAuditScheduler.h"
#include "xGateAuditUtil.h"
#include "xGateUtil.h"

#define THISMODULE "AudSched"

xGateAuditScheduler::xGateAuditScheduler() :
  xGateScheduler()
{
}

xGateAuditScheduler::~xGateAuditScheduler(void)
{
}

int xGateAuditScheduler::handle_timeout(const ACE_Time_Value&, const void *arg)
{
  XGLOG_INFO( "xGateAuditScheduler::handle_timeout triggered");
  xGateSchedulerData *data = (xGateSchedulerData*)arg;
  if(!data) {
    XGLOG_ERROR( "handle_timeout failed. invalid xGateSchedulerData !");
    return 0;
  }
  
  string uid = data->m_uid.c_str();
  if(uid.empty()) {
    XGLOG_ERROR( "handle_timeout failed. uid is empty !");
    return 0; 
  }
  switch(data->m_schedulerType) {
    case EN_AUDIT_TIMER_CHECK_CALL_STATUS:
      {
        XGLOG_INFO( "CHECK_CALL_STATUS audit timer triggered for uid: %s", (char*)ur_log_string(uid));
        break;
      }
    case EN_AUDIT_TIMER_CHECK_CLIENT_STATUS:
      {
        XGLOG_INFO( "CHECK_CLIENT_STATUS audit timer triggered for uid: %s",(char*)ur_log_string(uid));
        break;
      }
    default:
      {
        XGLOG_INFO( "Unknown/Unsupported timeout event triggered for uid: %s",(char*)ur_log_string(uid));
        return 0;
      }
  }

  xGateSchedulerMsg *pMsg = new xGateSchedulerMsg();
  pMsg->setUid(uid);
  pMsg->setSchedulerData(data);
  AUDITUTIL->postToAudit(pMsg);
  //delete data; //Yoga don't delete here, we are deleting this pointer in xGateAuditHandler
  return 0;
}
