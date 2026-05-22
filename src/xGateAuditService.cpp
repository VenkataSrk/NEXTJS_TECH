//local includes
#include "xGateAuditService.h"
#include "xglog.h"
#include "xGateUtil.h"

#define THISMODULE "AudService"

xGateAuditService::xGateAuditService() :
  ACE_Task<ACE_MT_SYNCH>(), m_run(false)
{
}

xGateAuditService::~xGateAuditService(void)
{
}

bool xGateAuditService::deinit()
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO( "xGateAuditService stopped !");
  return true;
}

bool xGateAuditService::init()
{
  // Start the log thread.
  if(activate(THR_NEW_LWP, 1) == -1 ) {
    XGLOG_ERROR( "xGateAuditService::init() failed to start AuditService thread !");
    return false;
  } else {
    XGLOG_INFO( "xGateAuditService::init() success !");
  }
  m_run = true;

  if(!AUDITMANAGER->init()) {
    XGLOG_ERROR( "xGateAuditService::init() failed while initializing 'AuditScheduler' !");
    return false;
  } else {
    XGLOG_INFO( "xGateAuditService::init() successfully initialized 'AuditScheduler' !");
  }

  return true;
}

int xGateAuditService::svc(void)
{
  ACE_Message_Block *pAmb = 0;

  while(m_run) {
    ACE_Time_Value tv ((time(NULL) + 20));
    int result = getq(pAmb, &tv);
    if ((result == -1) && (errno == EWOULDBLOCK)) {
      continue;
    }

    if ((result != -1)) {
      XGLOG_INFO( "xGateAuditService::svc handle_msg success");
      handle_msg(pAmb);
    } else {
      XGLOG_ERROR( "xGateAuditService::svc error in Message Queue !");
      m_run = false;
    }

    // delete the message received
    if(pAmb) {
      delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_INFO( "xGateAuditService::svc() stopped");
  return 0;
}

bool xGateAuditService::handle_msg(ACE_Message_Block *pAmb)
{
  xGateBaseMsg *pMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
  if(!pMsg) {
    XGLOG_ERROR( "xGateAuditService::handle_msg failed. xGateBaseMsg pointer is NULL !");
    return false;
  }

  switch(pMsg->getMsgType()) {
    case EN_XGATE_MSG_AUDIT:
      {
        XGLOG_INFO( "xGateAuditService::handle_msg (processing AUDIT message)");
        xGateAuditMsg *pAuditMsg = dynamic_cast<xGateAuditMsg*>(pMsg);
        return AUDITMANAGER->processAuditMsg(pAuditMsg);
      }
    case EN_XGATE_MSG_SCHEDULER:
      {
        XGLOG_INFO( "xGateAuditService::handle_msg (processing SCHEDULER message)");
        xGateSchedulerMsg *pSchedulerMsg = dynamic_cast<xGateSchedulerMsg*>(pMsg);
        return AUDITMANAGER->processSchedulerMsg(pSchedulerMsg);
      }
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        XGLOG_ERROR( "xGateAuditService::handle_msg failed. got UNKNOWN message !");
        break;
      }      
  }
  return false;
}


