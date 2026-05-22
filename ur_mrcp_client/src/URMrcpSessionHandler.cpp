#include "xglog.h"
#include "URMrcpSessionHandler.h"

bool URMrcpSessionHandler::InsertMrcpCallUid(std::string callId, std::string uid)
{
  IT_MRCP_CALL_MAP it = m_tMrcpCallInfoMap.find(callId);
  if(it != m_tMrcpCallInfoMap.end()){
    XGLOG_ERROR("URMrcpSessionHandler::InsertMrcpCallUid failed .CallId(%s) already exists in the map!",callId.c_str());
    return false;
  }
  m_tMrcpCallInfoMap.insert(MRCP_CALL_MAP::value_type(callId,uid));
  XGLOG_DEBUG("Inserting the CallId (%s) and mapping uid(%s)in the map!",callId.c_str(),uid.c_str());
  return true;
}

bool URMrcpSessionHandler::DeleteMrcpCallUid(std::string callId)
{
  IT_MRCP_CALL_MAP it = m_tMrcpCallInfoMap.find(callId);
  if(it == m_tMrcpCallInfoMap.end()) {
    XGLOG_ERROR(" Deleting the call Info context is failed. Call Id does NOT existis in the map!");
    return false;
  }
  std::string uid = it->second;
  m_tMrcpCallInfoMap.erase(callId);

  XGLOG_DEBUG(" Deleteing the Mrcp Call Id(%s) from the map!",callId.c_str());
  return true;
}

bool URMrcpSessionHandler::GetMrcpCallUid(std::string callId,std::string &uid)
{
  IT_MRCP_CALL_MAP it = m_tMrcpCallInfoMap.find(callId);
  if(it == m_tMrcpCallInfoMap.end()) {
    XGLOG_ERROR("Retrieving the uid is failed. Call Id does NOT existis in the map!");
    return false;
  }
  uid = it->second;

  if(uid.empty()) {
    XGLOG_ERROR("Call Id.uid doesn't exists and context is NULL");
    return false;
  }
  XGLOG_DEBUG("Accessing  the session for uid(%s) context Id from the map!");
  return true;
}

bool URMrcpSessionHandler::InsertMrcpSesssionHandlerInfo(std::string callId, std::string sessionId)
{
  IT_MRCP_SESSION_MAP it = m_tMrcpSessionInfoMap.find(callId);
  if(it != m_tMrcpSessionInfoMap.end()){
    XGLOG_ERROR("URMrcpSessionHandler::InsertMrcpSesssionHandlerInfo failed .CallId(%s) already exists in the map!",callId.c_str());
    return false;
  }

  m_tMrcpSessionInfoMap.insert(MRCP_SESSION_MAP::value_type(callId,sessionId));
  XGLOG_DEBUG("Inserting the CallId (%s) and mapping sessionId(%s)in the map!",callId.c_str(),sessionId.c_str());
  return true;
}

bool URMrcpSessionHandler::DeleteMrcpSesssionHandlerInfo(std::string callId)
{
  IT_MRCP_SESSION_MAP it = m_tMrcpSessionInfoMap.find(callId);
  if(it == m_tMrcpSessionInfoMap.end()) {
    XGLOG_ERROR(" Deleting the Session Info context is failed. Call Id does NOT existis in the map!");
    return false;
  }
  std::string sessionId = it->second;
  m_tMrcpSessionInfoMap.erase(sessionId);

  XGLOG_DEBUG(" Deleteing the Mrcp Call Id(%s) from the map!",callId.c_str());
  return true;
}

std::string URMrcpSessionHandler::GetMrcpSesssionHandlerInfo(std::string callId)
{
  IT_MRCP_CALL_MAP it = m_tMrcpSessionInfoMap.find(callId);
  if(it == m_tMrcpSessionInfoMap.end()) {
    XGLOG_ERROR("Retrieving the sessionId is failed. Call Id does NOT existis in the map!");
    return NULL;
  }
  std::string sId = it->second;
  if(sId.empty()) {
    XGLOG_ERROR("Call Id.sessionId doesn't exists and context is NULL");
    return NULL;
  }
  XGLOG_DEBUG("Accessing the sessionId(%s) from the map!");
  return sId;
}

bool URMrcpSessionHandler::InsertMrcpSessionDetails(std::string callId,UrMrcpSessionDetails *sessionDetails)
{
  IT_MRCP_SESSION_DETAIL_MAP it = m_tMrcpSessionDetailMap.find(callId);
  if(it != m_tMrcpSessionDetailMap.end()){
    XGLOG_ERROR("URMrcpSessionHandler::InsertMrcpSesssionDetails failed .CallId(%s) already exists in the map!",callId.c_str());
    return false;
  }

  m_tMrcpSessionDetailMap.insert(MRCP_SESSION_DETAIL_MAP::value_type(callId,sessionDetails));
  return true;
}

bool URMrcpSessionHandler::DeleteMrcpSesssionDetails(std::string callId)
{
  IT_MRCP_SESSION_DETAIL_MAP it = m_tMrcpSessionDetailMap.find(callId);
  if(it == m_tMrcpSessionDetailMap.end()) {
    XGLOG_ERROR(" Deleting the Session Info context is failed. Call Id does NOT existis in the map!");
    return false;
  }
  m_tMrcpSessionInfoMap.erase(callId);

  XGLOG_DEBUG(" Deleteing the Mrcp Call Id(%s) from the map!",callId.c_str());
  return true;
}

UrMrcpSessionDetails* URMrcpSessionHandler::GetMrcpSesssionDetails(std::string callId)
{
  IT_MRCP_SESSION_DETAIL_MAP it = m_tMrcpSessionDetailMap.find(callId);
  if(it == m_tMrcpSessionDetailMap.end()) {
    XGLOG_ERROR("Retrieving the sessionId is failed. Call Id does NOT existis in the map!");
    return NULL;
  }
  UrMrcpSessionDetails *sessionDetails = it->second;
  if(!sessionDetails) {
    XGLOG_ERROR("Call Id.sessionId doesn't exists and context is NULL");
    return NULL;
  }
  XGLOG_DEBUG("Accessing the sessionId(%s) from the map!");
  return sessionDetails;
}


