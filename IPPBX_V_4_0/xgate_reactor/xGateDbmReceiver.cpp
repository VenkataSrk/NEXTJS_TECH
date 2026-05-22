//local includes
#include "xglog.h"
#include "xGateDbmReceiver.h"
#include "xGateDbmProcessor.h"

#define THISMODULE "DbmRecv"

xGateDbmReceiver::xGateDbmReceiver(const char *serverType):xGateProcessor(serverType)
{
  XGLOG_WARN("xGateDbmReceiver::xGateDbmReceiver");
}

xGateDbmReceiver::~xGateDbmReceiver(void)
{
  XGLOG_WARN("xGateDbmReceiver::~xGateDbmReceiver");
}

xGateRetVal xGateDbmReceiver::process_msg(xGateBaseMsg* pMsg)
{
  XGLOG_INFO("Enter xGateDbmReceiver::process_msg(%s)", m_serverType); 

  switch(pMsg->getMsgType()) {
    case EN_XGATE_MSG_FROM_DBM: //read dbm response
      {
        XGLOG_INFO("Enter process_msg processing MSG_FROM_DBM"); 
        xGateDbmMsg *pDbmMsg = dynamic_cast<xGateDbmMsg*>(pMsg);
        if(!check_http_response(pDbmMsg)) {
         XGLOG_ERROR("xGateDbmReceiver::process_msg failed");
        }
        break;
      }
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
       XGLOG_ERROR("xGateDbmReceiver::process_msg is EN_XGATE_MSG_UNKNOWN");
        break;
      }      
  }
  return EN_XGATE_STATUS_SUCCESS;  
}

#if 0
bool xGateDbmReceiver::check_http_response(xGateDbmMsg *pMsg)
{
  xGateHttpHandler *httpHandler = xGateHttpHandler::getInstance();
  if(!httpHandler) {
   XGLOG_ERROR("check_http_response failed. invalid httphandler object !");
    return false;  
  }
  HttpRequestInfo *httpReqInfo = pMsg->get_http_request_info();
  if(!httpReqInfo) {
  }
  if(httpReqInfo) {
   XGLOG_ERROR("check_http_response failed. invalid httpReqInfo object !");
    return false;  
  }
  post(httpReqInfo);
  delete httpReqInfo;
  return true;
}
#else
bool xGateDbmReceiver::check_http_response(xGateDbmMsg *pMsg)
{
  xGateHttpHandler *httpHandler = xGateHttpHandler::getInstance();
  if(!httpHandler) {
   XGLOG_ERROR("check_http_response failed. invalid httphandler object !");
    return false;  
  }
  httpHandler->getHttpResponse(); 
}
#endif

bool xGateDbmReceiver::post(HttpRequestInfo *httpReqInfo)
{
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = xGateUtil::getDbmProcessor();
  if(!taskPtr) {
   XGLOG_ERROR("post failed. DbmProccessor taksptr is NULL !");
    return false;
  }
  
  if(httpReqInfo->m_output.empty()) {
   XGLOG_ERROR("post failed. http response is empty !");
    return false;
  }

  int len = httpReqInfo->m_output.length();
  char* data = new char[len+1];
  memset(data, 0, (len+1));
  if(!httpReqInfo->m_output.empty()) {
    memcpy (data, httpReqInfo->m_output.c_str(), len);
  }
  data[len] = '\0';

  xGateDbmMsg *pDbmMsg = new xGateDbmMsg();
  pDbmMsg->setMsgType(EN_XGATE_MSG_FROM_DBM);
  pDbmMsg->set_data(data);
  pDbmMsg->set_data_len(len);

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateDbmMsg*>(pDbmMsg);
  ACE_Time_Value tval ((time(NULL)));
  tval += ACE_Time_Value(0,1);

  if(taskPtr->putq(pAmb, &tval)) {
    XGLOG_INFO("successfully posted message to DbmProcessor");
  } else {
   XGLOG_ERROR("failed to post message to DbmProcessor !");
    return false;
  }
  return true;
}
