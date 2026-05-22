
//local includes
#include "xGateAuditService.h"
#include "xGateAuditUtil.h"
#include "xGateAuditScheduler.h"
#include "xGateHmpProcessor.h"

#define THISMODULE "AudUtil"

xGateAuditUtil::xGateAuditUtil() : 
  m_auditStartTime(DEFAULT_AUDIT_START_TIME),
  m_auditAttemptCount(DEFAULT_AUDIT_ATTEMPT_COUNT)
{
}

xGateAuditUtil::~xGateAuditUtil(void)
{
}

long int xGateAuditUtil::scheduleTimer(xGateSchedulerData *data, unsigned int timeoutSecond)
{
  if(!data) {
    XGLOG_ERROR( "scheduleTimer failed. Invalid scheduler data object !");
    return -1; 
  }

  xGateAuditScheduler *pScheduler = AUDITSCHEDULER;
  ACE_Reactor *reactor = AUDITSCHEDULER->getReactor();
  long int timerId = reactor->schedule_timer(pScheduler, (void*)data, ACE_Time_Value(timeoutSecond, 0));
  if(timerId == -1) {
    XGLOG_ERROR( "scheduleTimer %d failed for uid: %s !", \
      data->m_schedulerType,(char*)ur_log_string(data->m_uid));
  } else {
    XGLOG_ERROR( "scheduleTimer %d for uid: %s success with timer_id: %ld", \
        data->m_schedulerType,(char*)ur_log_string(data->m_uid), timerId);
  }
  return timerId;
}

bool xGateAuditUtil::cancelTimer(long int timerId, string& uid)
{
  xGateAuditScheduler *pScheduler = AUDITSCHEDULER;
  ACE_Reactor *reactor = AUDITSCHEDULER->getReactor();
  if(timerId > -1) {
    XGLOG_ERROR( "AuditUtil::CancelTimer failed. Given timerId for uid: %s is invalid !", \
        (char*)ur_log_string(uid));
    return false;
  }
  if(reactor->cancel_timer(timerId)) {
    XGLOG_INFO( "AuditUtil::CancelTimer success for uid: %s and timer_id: %ld", \
        (char*)ur_log_string(uid), timerId);
    return true;
  } else {
    XGLOG_ERROR( "AuditUtil::CancelTimer failed for timer_id: %ld and uid: %s", \
       timerId, (char*)ur_log_string(uid));
  }
  return false;
}

bool xGateAuditUtil::postToAudit(xGateBaseMsg *pMsg)
{
  if(!pMsg){
    XGLOG_ERROR( "postToAudit failed. Invalid message pointer !");
    return false;
  }

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateBaseMsg*> (pMsg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  if(AUDITSERVICE->putq(pAmb, &tval)) {
    return true;
  }
  return false;
}

bool xGateAuditUtil::postToHmpProcessor(xGateBaseMsg *pMsg)
{
  if(!pMsg){
    XGLOG_ERROR( "postToHmpProcessor failed. Invalid message pointer !");
    return false;
  }
  pMsg->setMsgType(EN_XGATE_MSG_AUDIT);

  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = xGateUtil::getHmpProcessor();
  if(!taskPtr) {
    XGLOG_ERROR( "postToHmpProcessor failed. Invalid HmpProcessor task not found");
    return false;
  }

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateBaseMsg*> (pMsg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  if(taskPtr->putq(pAmb, &tval)) {
    return true;
  }
  return false;
}

unsigned short xGateAuditUtil::getAuditStartTime()
{
  if(m_auditStartTime <= 0) {
    m_auditStartTime = DEFAULT_AUDIT_START_TIME;
  }
  return m_auditStartTime; 
}

void xGateAuditUtil::setAuditStartTime(unsigned short value)
{
  if(value > 0) {
    m_auditStartTime = value;
  } else {
    m_auditStartTime = DEFAULT_AUDIT_START_TIME;
  }
  m_auditStartTime = m_auditStartTime * 60;
}

unsigned short xGateAuditUtil::getAuditAttemptCount()
{
  if(m_auditAttemptCount <= 0) {
    m_auditAttemptCount = DEFAULT_AUDIT_ATTEMPT_COUNT;
  }
  return m_auditAttemptCount; 
}

void xGateAuditUtil::setAuditAttemptCount(unsigned short value)
{
  if(value > 0) {
    m_auditAttemptCount = value;
  } else {
    m_auditAttemptCount = DEFAULT_AUDIT_ATTEMPT_COUNT;
  }
}
