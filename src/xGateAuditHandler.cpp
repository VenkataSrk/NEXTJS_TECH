//local includes
#include "xglog.h"
#include "xGateAuditHandler.h"
#include "xGateAuditMsg.h"
#include "xGateUtil.h"

#define THISMODULE "AudHandler"

xGateAuditHandler::xGateAuditHandler(string& uid, string& channelId) : 
  m_uid(uid), m_channelId(channelId), m_auditInterval(0), m_numOfAuditDone(0),
  m_currentAuditState(EN_AUDIT_STATE_UNKNOWN), m_maxAuditAttempt(0),
  m_auditTimerId(-1)
{
}

xGateAuditHandler::~xGateAuditHandler(void)
{
  cancelAuditTimer();
}

const char* xGateAuditHandler::getRegisteredDataTime()
{
}

bool xGateAuditHandler::init()
{
  XGLOG_INFO( "xGateAuditHandler::init !");

  m_maxAuditAttempt = AUDITUTIL->getAuditAttemptCount();
  m_auditInterval = AUDITUTIL->getAuditStartTime();

  //set initial auditing time
  return setAuditTimer(EN_AUDIT_TIMER_CHECK_CLIENT_STATUS);
}

bool xGateAuditHandler::setAuditTimer(AuditTimerType timerType)
{
  if(m_auditTimerId > 0) {
    XGLOG_ERROR( "xGateAuditHandler::setAuditTimer failed. already audit timer scheduled for uid: %s", \
        (char*)ur_log_string (m_uid));
    cancelAuditTimer(); //cancel old scheduled audit timer and scheduled new audit timer
  }

  xGateSchedulerData *schedulerData = new xGateSchedulerData(m_uid, timerType, 1);
  m_auditTimerId = AUDITUTIL->scheduleTimer(schedulerData, m_auditInterval);
  if(m_auditTimerId == -1) {
    XGLOG_ERROR( "xGateAuditHandler::setAuditTimer failed. Scheduling CHECK_CALL_STATUS audit timer failed for uid: %s !", \
        (char*)ur_log_string (m_uid));
    return false;
  }

  ++m_numOfAuditDone;
  return true;
}

bool xGateAuditHandler::cancelAuditTimer()
{
  if(m_auditTimerId > -1) {
    AUDITUTIL->cancelTimer(m_auditTimerId, m_uid);
    m_auditTimerId = -1;
    return true;
  }
  return false;
}

bool xGateAuditHandler::deinit()
{
  XGLOG_INFO( "xGateAuditHandler::deinit !");
  return true;
}

void xGateAuditHandler::setAuditState(AuditState state)
{
  m_currentAuditState = state;
}

AuditState xGateAuditHandler::getAuditState()
{
  return m_currentAuditState;
}

bool xGateAuditHandler::handleSchedulerEvent(xGateSchedulerMsg *pMsg)
{
  xGateSchedulerData *data = pMsg->getSchedulerData();
  if(!data) {
    XGLOG_ERROR( "handleSchedulerEvent failed. invalid SchedulerData for uid: %s !", \
        (char*)ur_log_string (m_uid));
    return false;
  }
  
  //cancel existing scheduled timer
  cancelAuditTimer();

  switch(data->m_schedulerType) {
    case EN_AUDIT_TIMER_CHECK_CALL_STATUS:
      {
        XGLOG_INFO( "handleSchedulerEvent handling CHECK_CALL_STATUS event for uid: %s", (char*)ur_log_string (m_uid));
        break;
      }
    case EN_AUDIT_TIMER_CHECK_CLIENT_STATUS:
      {
        XGLOG_INFO( "handleSchedulerEvent handling CHECK_CLIENT_STATUS event for uid: %s", (char*)ur_log_string (m_uid));
        xGateAuditMsg *auditMsg = new xGateAuditMsg(m_uid, m_channelId);
        auditMsg->setAuditMsgType(EN_AUDIT_MSG_CHECK_CLIENT_STATUS);
        auditMsg->setAuditState(m_currentAuditState);
        AUDITUTIL->postToHmpProcessor(auditMsg); 
        break;
      }
    default:
      {
        XGLOG_INFO( "Unknown/Unsupported timeout event triggered for uid: %s", (char*)ur_log_string (m_uid));
        break;
      }
  }
  return true;
}

bool xGateAuditHandler::checkReauditRequired(xGateAuditMsg *pMsg)
{
  XGLOG_INFO( "xGateAuditHandler::checkReauditRequired for uid: %s", \
      (char*)ur_log_string (m_uid));

  //cancel existing scheduled timer if any
  cancelAuditTimer();

  AuditState state = pMsg->getAuditState();

  switch(state) {
    case EN_AUDIT_STATE_CLIENT_ENTRY_AVAILABLE:
      {
        XGLOG_INFO("checkReauditRequired processing AUDIT_STATE_CLIENT_ENTRY_AVAILABLE for uid: %s", \
            (char*)ur_log_string (m_uid));
        if(m_numOfAuditDone >= m_maxAuditAttempt) {
          XGLOG_INFO( "checkReauditRequired sending AUDIT_MSG_CLEANUP_CLIENT to HmpProcessor. \
              Maximum auditing attempts: %d reached for uid: %s", m_numOfAuditDone, (char*)ur_log_string (m_uid));
          //send EN_AUDIT_MSG_CLEANUP_CALL to HmpProcessor, so it will release resource  
          xGateAuditMsg *auditMsg = new xGateAuditMsg(m_uid, m_channelId);
          auditMsg->setAuditMsgType(EN_AUDIT_MSG_CLEANUP_CLIENT);
          auditMsg->setAuditState(m_currentAuditState);
          AUDITUTIL->postToHmpProcessor(auditMsg); 
          return true;
        }
        return setAuditTimer(EN_AUDIT_TIMER_CHECK_CLIENT_STATUS);
      }
    case EN_AUDIT_STATE_CLIENT_STILL_ALIVE:
      {
        //TODO: Yoga, re-schedule auditing even if max attempt reached since client is alive  
        break;
      }
    default:
      {
        break;
      }
  }
  return true;
}
