//local includes
#include "xGateAuditManager.h"
#include "xglog.h"
#include "xGateUtil.h"

#define THISMODULE "AudManager"

xGateAuditManager::xGateAuditManager() :
  m_uid(""), m_channelId(""), m_pHandler(NULL)
{
}

xGateAuditManager::~xGateAuditManager(void)
{
}

bool xGateAuditManager::init()
{
  XGLOG_INFO( "xGateAuditManager::init !");

  //initialize xGateAuditScheudler
  if(!AUDITSCHEDULER->init()) {
    XGLOG_ERROR( "xGateAuditManager::init failed while initializing Audit Scheduler !"); 
    return false;
  }

  return true;
}

bool xGateAuditManager::deinit()
{
  XGLOG_INFO( "xGateAuditManager::deinit !");
  return true;
}

bool xGateAuditManager::doAuditInit(xGateAuditMsg *pMsg)
{
  if(m_pHandler) {
    XGLOG_ERROR( "doAuditInit skipped. AuditHandler object already been initialized for uid:%s and channel: %s !", \
        (char*)ur_log_string (m_uid),(char*)ur_log_string(m_channelId));
    return false;
  }

  m_pHandler = new xGateAuditHandler(m_uid, m_channelId);
  if(!m_pHandler->init()) {
    XGLOG_ERROR( "doAuditInit failed. Init AuditHandler failed for uid: %s and channel: %s !", \
        (char*)ur_log_string(m_uid), (char*)ur_log_string(m_channelId)); 
    return false;
  }

  AuditState state = pMsg->getAuditState();
  if(state == EN_AUDIT_STATE_UNKNOWN) {
    XGLOG_ERROR( "doAuditInit failed. Unknown audit state defined for uid: %s and channel: %s !", \
        (char*)ur_log_string(m_uid), (char*)ur_log_string(m_channelId)); 
    return false;
  }
  m_pHandler->setAuditState(state);

  //add a new entry into audit handler session map
  if(!addAuditEntry()) {
    XGLOG_ERROR( "doAuditInit failed. AuditHandler entry already available for uid: %s and channel: %s !", \
        (char*)ur_log_string(m_uid), (char*)ur_log_string(m_channelId)); 
    return false;
  }
  return true;
}

bool xGateAuditManager::processAuditMsg(xGateAuditMsg *pMsg)
{
  XGLOG_INFO( "xGateAuditManager::processAuditMsg start !");
  if(!readMsg(pMsg)) {
    XGLOG_ERROR( "processAuditMsg failed while reading incoming audit message !");
    return false; 
  }

  switch(pMsg->getAuditMsgType()) {
    case EN_AUDIT_MSG_REGISTER_CALL:
      {
        XGLOG_INFO( "processAuditMsg (processing REGISTER_CALL audit message)");
        //initialize audit handler
        if(!doAuditInit(pMsg)) {
          XGLOG_ERROR( "processAuditMsg failed. doAuditInit failed !");
          return false;
        }
        break;
      }
    case EN_AUDIT_MSG_DEREGSITER_CALL:
      {
        XGLOG_INFO( "processAuditMsg (processing DEREGSITER_CALL audit message)");
        deleteAuditEntry();
        break;
      }
    case EN_AUDIT_MSG_CHECK_CALL_STATUS:
      {
        XGLOG_INFO( "processAuditMsg (processing CHECK_CALL_STATUS audit message)");
        break;
      }
    case EN_AUDIT_MSG_CHECK_CLIENT_STATUS:
      {
        XGLOG_INFO( "processAuditMsg (processing CHECK_CLIENT_STATUS audit message)");
        checkClientStatusResponse(pMsg);
        break;
      }
    case EN_AUDIT_MSG_CLEANUP_CALL:
      {
        XGLOG_INFO( "processAuditMsg (processing CLEANUP_CALL audit message)");
        break;
      }
    case EN_AUDIT_MSG_CLEANUP_CLIENT:
      {
        XGLOG_INFO( "processAuditMsg (processing CLEANUP_CLIENT audit message)");
        break;
      }
    case EN_AUDIT_MSG_UNKNOWN:
    default:
      {
        XGLOG_ERROR( "processAuditMsg failed. got UNKNOWN audit message !");
        break;
      }      
  }
  return true;
}

bool xGateAuditManager::processSchedulerMsg(xGateSchedulerMsg *pMsg)
{
  XGLOG_INFO( "xGateAuditManager::processSchedulerMsg start !");
  if(!readMsg(pMsg)) {
    XGLOG_ERROR( "processSchedulerMsg failed while reading incoming scheduler message !");
    return false; 
  }

  if(!m_pHandler) {
    XGLOG_ERROR("processSchedulerMsg failed. AuditHandler object entry not found for uid: %s !", \
       (char*)ur_log_string(m_uid));
    return false;
  }

  return m_pHandler->handleSchedulerEvent(pMsg);
}

bool xGateAuditManager::readMsg(xGateAuditMsg* pMsg)
{
  m_uid.clear();
  m_channelId.clear();
  m_pHandler = NULL;

  if(!pMsg) {
    XGLOG_ERROR( "xGateAuditManager::readMsg failed. xGateAuditMsg pointer is NULL !");
    return false;
  }

  m_uid = pMsg->getUid();
  m_channelId = pMsg->getChannelId();

  if(m_uid.empty() || m_channelId.empty()) {
    XGLOG_ERROR( "xGateAuditManager::readMsg failed. uid or callid detail is empty !");
    return false;
  }

  findAuditEntry();
  return true;
}

bool xGateAuditManager::readMsg(xGateSchedulerMsg* pMsg)
{
  m_uid.clear();
  m_channelId.clear();
  m_pHandler = NULL;

  if(!pMsg) {
    XGLOG_ERROR( "xGateAuditManager::readMsg failed. xGateSchedulerMsg pointer is NULL !");
    return false;
  }

  m_uid = pMsg->getUid();

  if(m_uid.empty()) {
    XGLOG_ERROR( "xGateAuditManager::readMsg failed. uid or callid detail is empty !");
    return false;
  }

  findAuditEntry();
  return true;
}

bool xGateAuditManager::checkClientStatusResponse(xGateAuditMsg *pMsg)
{
  XGLOG_INFO( "checking client status reponse for uid: %s",(char*)ur_log_string(m_uid));
  if(!m_pHandler) {
    XGLOG_ERROR( "checkClientStatusResponse skipped. AuditHandler object already been deleted for uid:%s and channel: %s !", \
       (char*)ur_log_string(m_uid), (char*)ur_log_string(m_channelId));
    return false;
  }
  AuditState state = pMsg->getAuditState();  
  m_pHandler->setAuditState(state);

  switch(state) {
    case EN_AUDIT_STATE_CLIENT_ENTRY_AVAILABLE:
      {
        //TODO: Yoga, check whether re-schedule is required or need send clean-up event to HmpProcessor
        XGLOG_INFO( "checkClientStatusResponse processing AUDIT_STATE_CLIENT_ENTRY_AVAILABLE for uid: %s", \
          (char*)ur_log_string(m_uid));
        if(!m_pHandler->checkReauditRequired(pMsg)) {
          return deleteAuditEntry();
        }
        break;
      }
    case EN_AUDIT_STATE_CLIENT_ENTRY_DELETED:
      {
        XGLOG_INFO( "checkClientStatusResponse processing AUDIT_STATE_CLIENT_ENTRY_DELETED for uid: %s", \
          (char*)ur_log_string (m_uid));
        return deleteAuditEntry();
      }
    case EN_AUDIT_STATE_CLIENT_STILL_ALIVE:
      {
        //TODO: Yoga, re-schedule auditing even if max attempt reached since client is alive  
        break;
      }
    case EN_AUDIT_STATE_CALL_ENTRY_DELETED:
      {
        XGLOG_INFO( "checkClientStatusResponse processing AUDIT_STATE_CALL_ENTRY_DELETED for uid: %s", \
          (char*)ur_log_string (m_uid));
        return deleteAuditEntry();
      }
    default:
      {
        XGLOG_ERROR( "checkClientStatusResponse failed. Unknown audit state given for uid: %s", \
            (char*)ur_log_string (m_uid));
        return false;
      }
  }

  return true;
}


bool xGateAuditManager::addAuditEntry()
{
  int status = -1;

  status = m_auditorMap.bind(m_uid.c_str(), m_pHandler);
  if(status == 0) {
    XGLOG_INFO( "addAuditEntry success for uid: %s",(char*)ur_log_string(m_uid));
    return true;
  } else if(status == 1) {
    XGLOG_INFO( "addAuditEntry attempt is made to bind an existing entry for uid: %s",(char*)ur_log_string( m_uid));
  } else if(status == -1) {
    XGLOG_ERROR( "addAuditEntry failed for mgresource_id: %s", (char*)ur_log_string (m_uid));
  }
  return false;
}

bool xGateAuditManager::eraseAuditEntry()
{
  m_auditorMap.unbind(m_uid.c_str()); 
  XGLOG_INFO( "erase AuditEntry in map for uid: %s",(char*)ur_log_string(m_uid));
}

bool xGateAuditManager::findAuditEntry()
{
  m_pHandler = NULL;
  if(m_auditorMap.find(m_uid.c_str(), m_pHandler) != -1) {
    XGLOG_INFO( "findAuditEntry success for uid: %s",(char*)ur_log_string(m_uid));
    return true;
  } else {
    XGLOG_WARN( "findAuditEntry failed. not available for uid: %s", (char*)ur_log_string(m_uid));
  }
  return false;
}

bool xGateAuditManager::deleteAuditEntry()
{
  if(!m_pHandler || m_uid.empty()) {
    return false;   
  }
  XGLOG_INFO( "deleting client entry: %s from AuditorMap", \
     (char*)ur_log_string(m_uid));
  eraseAuditEntry();
  delete m_pHandler;
  m_pHandler = NULL;
  return true;
}
