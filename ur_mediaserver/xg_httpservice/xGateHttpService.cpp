#include <fstream>
#include <stdexcept>
#include "xGateUtil.h"
#include "xglog.h"
#include "xGateBaseMsg.h"
#include "xGateHttpService.h"
#include "xGateHttpHandler.h"
#include "xGateHttpServiceMsg.h"

xGateHttpService::xGateHttpService() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false),m_httpConfig(0)
{
   m_strModuleName ="HttpServiceModule";
}

xGateHttpService::~xGateHttpService(void)
{
}

int xGateHttpService::initModule(IURModuleConfig * phttpConfig)
{
   m_httpConfig = phttpConfig;
   init();
   return IURDefines::UR_RETURN_SUCCESS;
}
int xGateHttpService::unitModule()
{
   stop();
   return IURDefines::UR_RETURN_SUCCESS;
}
void xGateHttpService::getModuleName(std::string& moduleName){
    moduleName=m_strModuleName;
}

IURDefines::MODULE_ID xGateHttpService::getModuleID(){
  return IURDefines::UR_MODULE_HTTP_SERVICE;
}

IURModuleConfig * xGateHttpService::getModuleConfig(){
  return m_httpConfig;
}

bool xGateHttpService::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("xGateHttpService stopped !");
  return true;
}

bool xGateHttpService::init()
{
  XGLOG_INFO("xGateHttpService::init() called");
  m_run = true;



  // Start the http thread.
  if (activate(THR_NEW_LWP, 1) == -1 )
  {
    XGLOG_ERROR("xGateHttpService::init() failed to start VMS controller thread !");
    return false;
  }
  else
  {
    XGLOG_INFO("xGateHttpService::init() success !");
  }

    xGateHttpHandler *httpHandler = xGateHttpHandler::getInstance();
    if(!httpHandler) {
      XGLOG_ERROR("xGateHttpService::handle_msg failed. invalid httphandler object !");
      return false;
    }

#if 0
  HttpRequestInfo httpReqInfo;
  httpReqInfo.m_url = "http://10.22.7.29:5001/api";
 string data =  "{\"trans_id\": \"123456\",\"db_name\": \"unifiedring_pbx\",\"db_operation\": \"EXECUTE\",\"table_name\": \"Sip_Register\",\"sp_name\": \"sip_get_Registered_user_info\",\"input\":{\"domain_name\": \"6367.UR.mundio.com\",\"Username\":\"380\"}}";
  httpReqInfo.m_data = data;
  httpReqInfo.m_httpReqMethod=EN_HTTP_METHOD_POST;
  if(!httpHandler->doHttpRequest(httpReqInfo)) {
    XGLOG_ERROR("xGateHttpService::init failed. send http request failed !");
    return false;
  }
 httpHandler->getHttpResponse(); 
// m_httpConfig->getCallBack()->handleModuleCallbackMsg(pMsg);
#endif
  return true;
}

int xGateHttpService::svc(void)
{
  //CLog::Warning(THISMODULE, "xGateHttpService::svc(%s)", m_serverType);
  ACE_Message_Block *pAmb = 0;

  while(m_run)
  {
    int result = getq(pAmb, NULL);
    if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
    {
      continue;
    }
    if ((result != -1))
    {
      handle_msg(pAmb);
    }
    else
    {
      XGLOG_ERROR("xGateHttpService::svc Error in Message Queue");
      m_run = false;
    }
    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_INFO("xGateHttpService::svc() stopped");
  return 0;
}

bool xGateHttpService::handle_msg(ACE_Message_Block *pAmb)
{
  if(pAmb != NULL) {
    xGateBaseMsg* pMsg = NULL;
    pMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
    if(pMsg  == NULL) {
      XGLOG_ERROR("xGateHttpService::handle_msg recvd invalid xGateBaseMsg Message");
      return false;
    }

    xGateHttpHandler *httpHandler = xGateHttpHandler::getInstance();
    if(!httpHandler) {
      XGLOG_ERROR("xGateHttpService::handle_msg failed. invalid httphandler object !");
      return false;
    }

    //send http request
    xGateHttpServiceMsg *pHttpMsg = dynamic_cast<xGateHttpServiceMsg*>(pAmb);
    HttpRequestInfo &httpReqInfo = pHttpMsg->get_http_request_info();
    if(!httpHandler->doHttpRequest(httpReqInfo)) {
      XGLOG_ERROR("xGateHttpService::handle_msg failed. send http request failed !");
      return false;
    }
  }

  return true;
}

int xGateHttpService::pushModuleMsg(IURModuleMsg * phttpMsg)
{
     XGLOG_INFO("xGateHttpService::inside pushModuleMsg");   
     if (!phttpMsg) {
          XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
          return IURDefines::UR_RETURN_ERROR;
       }
      else{
         XGLOG_DEBUG("xGateHttpService::Message received from controller to httpservice");
          xGateHttpService *httpService = HTTPSERVICE;
           httpService->putq(dynamic_cast<ACE_Message_Block*>(phttpMsg));
      }    
  return IURDefines::UR_RETURN_SUCCESS;
}

#if 0
bool xGateHttpService::post(HttpRequestInfo *httpReqInfo)
{
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  if(m_requestorModule == UR_MODULE_VMS_CONTROLLER) {
    taskPtr = VMSCONTROLLER;
  } else if(m_requestorModule == UR_MODULE_IVR_SERVICE) {
    taskPtr = IVRSERVICE;
  }

  if(!taskPtr) {
    XGLOG_ERROR("xGateHttpService::post failed. taksptr is NULL !");
    return false;
  }

  if(httpReqInfo->m_output.empty()) {
    XGLOG_ERROR("xGateHttpService::post failed. http response is empty !");
    return false;
  }

  int len = httpReqInfo->m_output.length();
  char* data = new char[len+1];
  memset(data, 0, (len+1));
  if(!httpReqInfo->m_output.empty()) {
    memcpy (data, httpReqInfo->m_output.c_str(), len);
  }
  data[len] = '\0';

  xGateHttpServiceMsg *pHttpMsg = new xGateHttpServiceMsg();
  /*pHttpMsg->set_http_msg_type(EN_XGATE_MSG_HTTP_RESPONSE);
    pHttpMsg->set_data(data);
    pHttpMsg->set_data_len(len);*/

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateHttpServiceMsg*>(pHttpMsg);
  ACE_Time_Value tval ((time(NULL)));
  tval += ACE_Time_Value(0,1);

  if(taskPtr->putq(pAmb, &tval)) {
    XGLOG_INFO("xGateHttpService successfully posted message to Http requestor module");
  } else {
    XGLOG_ERROR("xGateHttpService failed to post message to Http requestor module !");
    return false;
  }
  return true;
}
#endif
