
#include "xGateBaseMsg.h"
#include "XG_SFUController.h"
#include "xGateUtil.h"
#include "URModuleFactory.h"
//#include "VMSCallContextHandler.h"
//#include "xGateHttpServiceMsg.h"
#include "xGateSFUServiceMsg.h"
#include "xGateMediaServiceMsg.h"
#include "xGateSFUService.h"
#include "MediaServiceDefine.h"
#include "IURDefines.h"

XG_SFUController::XG_SFUController() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false),m_localIpAddress("")
{
  m_eCtrlStatus = XGCONTROLLER_STATUS_DOWN;
  arrayModules.resize(IURDefines::UR_MAX_LEN);
}

XG_SFUController::~XG_SFUController(void)
{
}

bool XG_SFUController::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("XG_SFUController stopped !");
  return true;
}

bool XG_SFUController::init(int sfu_id)
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

  if(initMediaModules()) {
    XGLOG_INFO("XG_SFUController initialized Media Service Module successfully");
  } else {
    XGLOG_ERROR("XG_SFUController failed to initialize Media Service Module !");
    return false;
  }

  if(initSFUModule(sfu_id)){
   XGLOG_INFO("XG_SFUController initialized SFU Modules successfully");
  }
  else {
   XGLOG_ERROR("XG_SFUController failed to initializing SFU Modules");
   return false;   
 }
  return true;
}

int XG_SFUController::svc(void)
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
      XGLOG_ERROR("XG_SFUController.svc() Error in Message Queue");
      m_run = false;
    }
    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_INFO("XG_SFUController.svc() stopped");
  return 0;
}


bool XG_SFUController::handle_sfu_ms(IURModuleMsg *pctrlMsg)
{	
 xGateSFUServiceMsg *pMsg = dynamic_cast<xGateSFUServiceMsg*>(pctrlMsg);
 xGateMediaServiceMsg *pMsg_ms = new xGateMediaServiceMsg();
 char *pmbc_msg = NULL;
 int len  = 0;
 pmbc_msg = pMsg->getMsg();
 len = pMsg->getMsgLen();
 xGateNetConnection netConnInfo;
 switch((xGateSFUMsgType)pMsg->getMsgType())
 {	 
  case EN_XGATE_MSG_SFU_CONNECT_CLIENT:
  {	  
    XGLOG_INFO("Got connect client message from SFU Module");
    netConnInfo.recvIp_ = pMsg->get_conn_ip();
    netConnInfo.recvPort_ = pMsg->get_port_number();
    pMsg_ms->set_netConnInfo(netConnInfo);
    pMsg_ms->setMsgType((xGateMsgType)EN_XGATE_MSG_CONNECT_CLIENT);
    pMsg_ms->setSrcModuleId(IURDefines::UR_MODULE_SFU_SERVICE);
    pMsg_ms->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
    break;
  }
  case EN_XGATE_MSG_SFU_ID:
  {
    XGLOG_INFO("Got SFU ID message pmbc_msg: %p len: %d", pmbc_msg, len);
    pMsg_ms->setConnectionId(pMsg->get_connection_id());
    pMsg_ms->setMsgType((xGateMsgType)EN_XGATE_MSG_SEND);
    pMsg_ms->setSrcModuleId(IURDefines::UR_MODULE_SFU_SERVICE);
    pMsg_ms->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
    pMsg_ms->set_msg(pmbc_msg);
    pMsg_ms->setMsgLen(len);
    break;
  }
  case EN_XGATE_MSG_SFU_CLIENT:
  {
    pMsg_ms->setConnectionId(pMsg->get_connection_id());
    pMsg_ms->setMsgType((xGateMsgType)EN_XGATE_MSG_SEND);
    pMsg_ms->setSrcModuleId(IURDefines::UR_MODULE_SFU_SERVICE);
    pMsg_ms->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
    pMsg_ms->set_msg(pmbc_msg); 
    pMsg_ms->setMsgLen(len); 
    break;	  
  }
  case EN_XGATE_MSG_SFU_CLIENT_REC:
  {
    pMsg_ms->setConnectionId(pMsg->get_connection_id());
    pMsg_ms->setMsgType((xGateMsgType)EN_XGATE_MSG_SEND);
    pMsg_ms->setSrcModuleId(IURDefines::UR_MODULE_SFU_SERVICE);
    pMsg_ms->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
    pMsg_ms->set_msg(pmbc_msg); 
    pMsg_ms->setMsgLen(len); 
    break;	  
  }
  default:
  {
   XGLOG_INFO("Invalid msg recvd from SFU");
   delete pMsg_ms;
   pMsg_ms = NULL;
   return false;
   break;	  
  }
 }
 arrayModules[pMsg_ms->getDstModuleId()]->pushModuleMsg(pMsg_ms);
 return true;
}

bool XG_SFUController::handle_ms_sfu(IURModuleMsg *pctrlMsg)
{
  	
 xGateMediaServiceMsg *pMsg = dynamic_cast<xGateMediaServiceMsg*>(pctrlMsg);
 xGateSFUServiceMsg *psfuMsg = new xGateSFUServiceMsg();
 switch((xGateMSMsgType)pMsg->getMsgType())
{	 
 case EN_XGATE_MSG_CLIENT_CONNECTED:
 {
   psfuMsg->setMsgType((xGateMsgType)EN_XGATE_MSG_SFU_CLIENT_CONNECTED);
   psfuMsg->setSrcModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
   psfuMsg->setDstModuleId(IURDefines::UR_MODULE_SFU_SERVICE);
   psfuMsg->set_connection_id(pMsg->get_connectionId());
   break;	 
 }
 case EN_XGATE_MSG_RECV:
 {
   psfuMsg->setMsgType((xGateMsgType)EN_XGATE_MSG_MBC_SERVER);
   psfuMsg->setSrcModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
   psfuMsg->set_connection_id(pMsg->get_connectionId());
   psfuMsg->setDstModuleId(IURDefines::UR_MODULE_SFU_SERVICE);
   psfuMsg->setMsg((char *)pMsg->get_msg());
   psfuMsg->setMsgLen(pMsg->getMsgLen());
   break;
 }
 case EN_XGATE_MSG_CONN_CLOSED:
 {
  psfuMsg->setMsgType((xGateMsgType)EN_XGATE_MSG_SFU_CONN_CLOSED);
  psfuMsg->setSrcModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
  psfuMsg->set_connection_id(pMsg->get_connectionId());
  psfuMsg->setDstModuleId(IURDefines::UR_MODULE_SFU_SERVICE);
  break; 
 }	 
 default:
 {
  XGLOG_ERROR("Invalid msg recvd from media service");
  return false;
  break;
 }
} 
 arrayModules[psfuMsg->getDstModuleId()]->pushModuleMsg(psfuMsg);
 return true; 
}

bool XG_SFUController::handle_msg(ACE_Message_Block *pAmb)
{
 IURModuleMsg *pctrlMsg = NULL;
 pctrlMsg = dynamic_cast<IURModuleMsg*>(pAmb);
 switch(pctrlMsg->getSrcModuleId())
 {	 
	 case IURDefines::UR_MODULE_SFU_SERVICE:
         {
           if(pctrlMsg->getDstModuleId() == IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE){
	      handle_sfu_ms(pctrlMsg);
           }		   
           else
           {
	     XGLOG_ERROR("Currently this dstModuleId not support in SFU Controller");
             return false;	     
           }
	   break;
         }
	 case IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE:
	 {
           if(pctrlMsg->getDstModuleId() == IURDefines::UR_MODULE_SFU_SERVICE){
	     handle_ms_sfu(pctrlMsg);	   
           }
           else
	   {
	     XGLOG_ERROR("SFU Controller invalid dst Module for MediaService");
             return false;	     
	   }	   
           break;		 
         }
	 default:
	 {
           XGLOG_ERROR("SFU Controller unsupported src module");
           return false;	   
           break;
         }
     }
   return true; 
}

bool XG_SFUController::handleModuleCallbackMsg(IURModuleMsg * pctrlMsg)
{
  // xGateBaseMsg *pMsg = dynamic_cast<xGateBaseMsg*>(pctrlMsg);
  if (pctrlMsg) {
    XGSFUCONTROLLER->putq(dynamic_cast<ACE_Message_Block*>(pctrlMsg));
  }
  return true;
}

bool XG_SFUController::initSFUModule(int sfu_id)
{
 IURModule *pSFUService = NULL;
 URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_SFU_SERVICE,pSFUService);
 if(!pSFUService){
    XGLOG_ERROR("Initializing SFU service/proxy module failed !");
    return false;
 }
  CONFIGSERVICE->m_sfu_id = sfu_id;
  CONFIGSERVICE->setCallBack(this);
  arrayModules[pSFUService->getModuleID()] = pSFUService;
  if(!pSFUService->initModule(CONFIGSERVICE) == IURDefines::UR_RETURN_ERROR) {
    XGLOG_ERROR("init() failed while initializing 'MediaService' !");
    return false;
  } else {
    XGLOG_INFO("init() successfully initialized 'MediaService' !");
  }
  return true;
}

bool XG_SFUController::initMediaModules()
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

std::string XG_SFUController::getCurrentDateTime() 
{
  time_t temp = time(NULL);
  struct tm * tmmytm = localtime(&temp);
  char current_time[100];
  strftime(current_time, sizeof(current_time), "%Y_%m/%d/%H_%M_%S", tmmytm);
  return current_time;
}
