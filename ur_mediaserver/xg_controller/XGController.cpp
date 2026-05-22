
#include "xGateBaseMsg.h"
#include "XGController.h"
#include "xGateUtil.h"
#include "URModuleFactory.h"


XGController::XGController() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false),m_localIpAddress("")
{
  m_eCtrlStatus = XGCONTROLLER_STATUS_DOWN;
  arrayModules.resize(IURDefines::UR_MAX_LEN);
}

XGController::~XGController(void)
{
}

bool XGController::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("XGController stopped !");
  return true;
}

bool XGController::init()
{
  XGLOG_INFO("init() called");
  m_run = true;

  // Start the log thread.
  if(activate(THR_NEW_LWP, 1) == -1) {
    XGLOG_ERROR("init() failed to start VMS controller thread !");
    return false;
  } else {
    XGLOG_INFO("init() success !");
  }

  m_localIpAddress = CONFIGSERVICE->m_localIp4Addr;
  if(initHttpService()) {
    XGLOG_INFO("XGController initialized Http Service Module successfully");
  } else {
    XGLOG_ERROR("XGController failed to initialize Http Service Module !");
    return false;
  }

  if(initMediaModules()) {
    XGLOG_INFO("XGController initialized Media Service Module successfully");
    return true;
  } else {
    XGLOG_ERROR("XGController failed to initialize Media Service Module !");
    return false;
  }


#if 0
  xGateBaseMsg* ptMsg =  new xGateBaseMsg();
  if(!ptMsg)
  {
    XGLOG_FATAL("OOM Issue. failed create xGateBaseMsg memory!");
    return false;
  }
  m_eCtrlStatus = XGCONTROLLER_STATUS_DOWN;
  ptMsg->setMsgType(EN_XGATE_MSG_SERVICE_CONTROLLER);
  ptMsg->setOpCode(XGController_CTRL_INT_SRV_INFO);
  this->putq(ptMsg);
#endif
  return true;
}

int XGController::initHttpService()
{
  IURModule *pHttpService = NULL ;
  URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_HTTP_SERVICE, pHttpService);
  if(!pHttpService){
    XGLOG_ERROR("Initializing Http service module in vms controller failed !");
    return 0;
  }

  CONFIGSERVICE->setCallBack(this);
  arrayModules[pHttpService->getModuleID()] = pHttpService;

  if(!pHttpService->initModule(CONFIGSERVICE)==IURDefines::UR_RETURN_ERROR) {
    XGLOG_ERROR("init() failed while initializing 'HttpService' !");
    return 0;
  } else {
    XGLOG_INFO("init() successfully initialized 'HttpService' !");
  }

  return 1;
}

int XGController::svc(void)
{
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
      XGLOG_ERROR("XGController.svc() Error in Message Queue");
      m_run = false;
    }
    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_INFO("XGController.svc() stopped");
  return 0;
}

bool XGController::handle_msg(ACE_Message_Block *pAmb)
{
  XGLOG_INFO("XGController handling message from queue");
  xGateBaseMsg *pMsg = dynamic_cast<xGateBaseMsg*>(pAmb);

  switch(pMsg->getMsgType())
  {
    case EN_XGATE_MSG_SERVICE_CONTROLLER:
      {
        XGLOG_INFO("XGController handling 'EN_XGATE_MSG_SERVICE_CONTROLLER' event from ivrservice");
        //handle_InternalCtrlMsg(pMsg);
        break;
      }
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        XGLOG_ERROR("XGController failed to handle invalid message: %s !",pMsg->getMsgType());
        break;
      }
  }

  return true;
}

bool XGController::handleModuleCallbackMsg(IURModuleMsg * pctrlMsg)
{
  // xGateBaseMsg *pMsg = dynamic_cast<xGateBaseMsg*>(pctrlMsg);
  XGLOG_INFO("XGController::Inside handleModuleCallbackMsg !");
  if (pctrlMsg) {
    XGLOG_DEBUG("XGController::Message Received in handleModuleCallbackMsg!");
    if(IURDefines::UR_MODULE_SERVICE_CONTROLLER == pctrlMsg->getDstModuleId()) {
      XGCONTROLLER->putq( dynamic_cast<ACE_Message_Block*>(pctrlMsg));
    } else if(arrayModules[pctrlMsg->getDstModuleId()]) {
      arrayModules[pctrlMsg->getDstModuleId()]->pushModuleMsg(pctrlMsg);
    } else {
      XGLOG_ERROR("handleModuleCallbackMsg failed,destination Unknown!");
    }	
  }
  return true;
}

bool XGController::initMediaModules()
{

  //Initialize Media Proxy Module  
  IURModule *pMediaService = NULL;
  URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE, pMediaService);
  if(!pMediaService) {
    XGLOG_ERROR("Initializing Media service/proxy module failed !");
    return false;
  }

  CONFIGSERVICE->setCallBack(this);
  arrayModules[pMediaService->getModuleID()] = pMediaService;
  if(!pMediaService->initModule(CONFIGSERVICE) == IURDefines::UR_RETURN_ERROR) {
    XGLOG_ERROR("init() failed while initializing 'MediaService' !");
    return false;
  } else {
    XGLOG_INFO("init() successfully initialized 'MediaService' !");
  }

  return true;
}

std::string XGController::getCurrentDateTime() 
{
  time_t temp = time(NULL);
  struct tm * tmmytm = localtime(&temp);
  char current_time[100];
  strftime(current_time, sizeof(current_time), "%Y_%m/%d/%H_%M_%S", tmmytm);
  return current_time;
}
