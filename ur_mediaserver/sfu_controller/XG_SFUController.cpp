
#include "xGateBaseMsg.h"
#include "XG_SFUController.h"
#include "xGateUtil.h"
#include "URModuleFactory.h"
//#include "VMSCallContextHandler.h"
#include "xGateHttpServiceMsg.h"
#include "xGateSFUServiceMsg.h"
#include "xGateDBServiceMsg.h"
#include "xGateMediaServiceMsg.h"
#include "xGateSFUService.h"
#include "MediaServiceDefine.h"
#include "IURDefines.h"
#include "RedisServiceDefine.h"
#include "RedisServiceMsg.h"

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

//bool XG_SFUController::init(int sfu_id)
bool XG_SFUController::init()
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
#if 0
  if(initHttpService()) {
    XGLOG_INFO("XG_SFUController initialized Http Service Module successfully");
  } else {
    XGLOG_ERROR("XG_SFUController failed to initialize Http Service Module !");
    return false;
  }
#endif
  if(initMediaModules()) {
    XGLOG_INFO("XG_SFUController initialized Media Service Module successfully");
  } else {
    XGLOG_ERROR("XG_SFUController failed to initialize Media Service Module !");
    return false;
  }

  //if(initSFUModule(sfu_id)){
  if(initSFUModule()){
   XGLOG_INFO("XG_SFUController initialized SFU Modules successfully");
  }
  else {
   XGLOG_ERROR("XG_SFUController failed to initializing SFU Modules");
   return false;   
 }

  if(initDBModule()){
   XGLOG_INFO("XG_SFUController initialized SFU Modules successfully");
  }
  else {
   XGLOG_ERROR("XG_SFUController failed to initializing SFU Modules");
   return false;   
 }
 if(initRedisModule()){
  XGLOG_INFO("XG_SFUController initialized Redis Module successfully");
 }else{
  XGLOG_ERROR("XG_SFUController failed to initializing Redis Modules");
	return false;
 }
  return true;
}

int XG_SFUController::initHttpService()
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

#if 0
bool XG_SFUController::handle_msg(ACE_Message_Block *pAmb)
{
  XGLOG_INFO("XG_SFUController handling message from queue");
  xGateBaseMsg *pMsg = dynamic_cast<xGateBaseMsg*>(pAmb);

  switch(pMsg->getMsgType())
  {
    case EN_XGATE_MSG_SERVICE_CONTROLLER:
      {
        XGLOG_INFO("XG_SFUController handling 'EN_XGATE_MSG_SERVICE_CONTROLLER' event from ivrservice");
        //handle_InternalCtrlMsg(pMsg);
        break;
      }
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        if (pMsg->getSrcModuleId() == IURDefines::UR_MODULE_HTTP_SERVICE)
        {
          XGLOG_INFO("XG_SFUController handling 'IURDefines::UR_MODULE_HTTP_SERVICE' event from ivrservice");
          handle_HttpResponse(pMsg);
          break;
        }

        XGLOG_ERROR("XG_SFUController failed to handle invalid message: %s !",pMsg->getMsgType());
        break;
      }
  }

  return true;
}
#endif

bool XG_SFUController::handle_HttpResponse(xGateBaseMsg *pMsg)
{
  XGLOG_DEBUG("XG_SFUController::Inside handle_HttpResponse");
  xGateHttpServiceMsg* ptHttpMsg = dynamic_cast<xGateHttpServiceMsg*>(pMsg);

  if(!ptHttpMsg) {
    XGLOG_ERROR("INVALID object. Http Response. Processing the VMS server INITIALIZATION info failed");
    return false;
  }

#if 0 //TODO: Yoga disabled this part for mbc
  if(XG_SFUController::XG_SFUController_STATUS_REQ_SRV_INFO == m_eCtrlStatus) {
    if(m_tXG_SFUControllerHandler.Handle_response_srvr_info_msg(ptHttpMsg, m_tSelfServerInfo)) {
      m_eCtrlStatus = XG_SFUController_STATUS_MODULES_INIT;
      initModule();
    } else {
      XGLOG_FATAL("VoiceMail Server Initialization Failed using  IP (%s).", m_localIpAddress.c_str());
    }
  } else {
    string strID = ptHttpMsg->getUid().c_str();

    VMSCallContext * ptCtx = NULL;
    if (!m_tCallContextHandler.getVmsCallContext(strID, ptCtx)) {
      XGLOG_ERROR("VoiceMail box validation failed, call id (%s) not found", strID.c_str());
      return false;
    }
    // never chagne the if checking order..
    if (ptCtx->getStatus() &  VMSCallContext::VMSCALL_STATUS_CALL_DISCONNECTED) {
      XGLOG_DEBUG("handle_http_repsonse,vmscallcontext is VMSCALL_STATUS_CALL_DISCONNECTED");	
      handleCallClearing(ptCtx,xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_CALL_DISCONNECTED);
      m_tCallContextHandler.DeleteVmsCallContext(strID);

    }
    else if (ptCtx->getStatus() &  VMSCallContext::VMSCALL_STATUS_CALL_SAVE_VMAIL){
      XGLOG_DEBUG("handle_http_repsonse,vmscallcontext is VMSCALL_STATUS_CALL_SAVE_VMAIL");
      // save voice maail.
      if (ptCtx->getStatus() &  VMSCallContext::VMSCALL_STATUS_CALL_DISCONNECTED) {

        handleCallClearing(ptCtx,xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_CALL_DISCONNECTED);
        m_tCallContextHandler.DeleteVmsCallContext(strID);
      }
    }
    else if (ptCtx->getStatus() &  VMSCallContext::VMSCALL_STATUS_REQ_USR_INFO) {
      XGLOG_DEBUG("handle_http_repsonse,vmscallcontext is VMSCALL_STATUS_REQ_USR_INFO");	
      if (!m_tXG_SFUControllerHandler.Handle_response_user_info_msg(ptHttpMsg, ptCtx->m_tUserInfo, strID)) {

        handleCallClearing(ptCtx,xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_REJECT_CALL);
        m_tCallContextHandler.DeleteVmsCallContext(strID);

        XGLOG_ERROR("VoiceMail box validation failed, call id (%s) not found", strID.c_str());
        return false;
      }
      ptCtx->setStatus(VMSCallContext::VMSCALL_STATUS_CALL_ANSWER_ALLOW);
      sendCallAnswerCommand(ptCtx);
    }
    else if (ptCtx->getStatus() &  VMSCallContext::VMSCALL_STATUS_CALL_VALIDATE_EXTN) {
      XGLOG_DEBUG("handle_http_repsonse,vmscallcontext is VMSCALL_STATUS_CALL_VALIDATE_EXTN");
      if (!m_tXG_SFUControllerHandler.Handle_response_Validate_Extn(ptHttpMsg, ptCtx->m_tUserPinInfo, strID)) {

        XGLOG_ERROR("VoiceMail Extn validation failed, call id (%s) not found", strID.c_str());
        return false;
      }
      sendExtnValidationResult(ptCtx);
    }
    else if (ptCtx->getStatus() &  VMSCallContext::VMSCALL_STATUS_CALL_VALIDATE_PIN) {
      XGLOG_DEBUG("handle_http_repsonse,vmscallcontext is VMSCALL_STATUS_CALL_VALIDATE_PIN");
      if (!m_tXG_SFUControllerHandler.Handle_response_Validate_Pin(ptHttpMsg, ptCtx->m_tUserPinInfo, strID)) {

        XGLOG_ERROR("VoiceMail Pin validation failed, call id (%s) not found", strID.c_str());
        return false;
      }
      sendPinValidationResult(ptCtx);
    }
  }
#endif
  return true;
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
    XGLOG_TRACE("Got EN_XGATE_MSG_SEND message data ptr: %p pMsg_ms: %p", pmbc_msg, pMsg_ms);
    break;	  
  }
  case EN_XGATE_MSG_REC_SERVER:
  { 
     XGLOG_INFO("Got connect client message from SFU Module");
     netConnInfo.recvIp_ = pMsg->get_conn_ip();
     netConnInfo.recvPort_ = pMsg->get_port_number();
     pMsg_ms->set_netConnInfo(netConnInfo);
     pMsg_ms->setConnectionId(pMsg->get_connection_id());
     pMsg_ms->setMsgType((xGateMsgType)EN_XGATE_MSG_LISTEN);
     pMsg_ms->setSrcModuleId(IURDefines::UR_MODULE_SFU_SERVICE);
     pMsg_ms->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
     break;
  }
  default:
  {
   XGLOG_INFO("Invalid msg recvd from SFU");
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
 case EN_XGATE_MSG_NEW_CLIENT:
 {
   psfuMsg->setMsgType((xGateMsgType)EN_XGATE_MSG_SFU_CLIENT_CONNECTED);
   psfuMsg->setSrcModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
   psfuMsg->setDstModuleId(IURDefines::UR_MODULE_SFU_SERVICE);
   psfuMsg->set_connection_id(pMsg->get_connectionId());
   break;
 }
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
   XGLOG_INFO("handle_ms_sfu with conn_id:%d",pMsg->get_connectionId());
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


bool XG_SFUController::handle_db_response(IURModuleMsg *pctrlMsg)
{
  XGLOG_INFO("XG_SFUController::handle_db_response");
  xGateDBServiceMsg *ptdbMsg = dynamic_cast<xGateDBServiceMsg*>(pctrlMsg);

  DBRequestInfo &dbReqInfo = ptdbMsg->get_db_request_info();
  std::string strCallId = dbReqInfo.m_contextId.c_str();
  std::string inputData = dbReqInfo.m_data;
  std::string output = dbReqInfo.m_output.c_str();
	std::string strUrl = "";
  std::string strMeetingId = "";
  Document outputdoc;
  ParseResult ok = outputdoc.Parse(output.c_str());
  if(!ok)
  {
     XGLOG_ERROR(" parseJsonMsg failed. Encountered with json parse error !");
     return false;
  }
  else if(!outputdoc.IsObject())
  {
     XGLOG_ERROR("handle_db_response parseJsonMsg failed. Not able create documnet object !");
     return false;
  }

	int iVid(0);
	if(outputdoc.HasMember("v_id"))
	{
    iVid = outputdoc["v_id"].GetInt();
	}

  int meet_pos = inputData.find("'", 1);
  if(meet_pos != -1)
    strMeetingId = inputData.substr(1,meet_pos-1);
  int pos = inputData.find("https");
	if(pos != -1){
    int pos2 = inputData.find(".mp4", pos) + 4; // Adjust to include '.mp4'
	  if(pos2 != -1){
      strUrl = inputData.substr(pos, pos2 - pos); // Finding recorded video URL 
	  }
	}
	XGLOG_INFO("handle_db_response URL:%s", strUrl.c_str());

	Document doc;
  StringBuffer strBuf;
  Writer<StringBuffer> writer(strBuf);
  Value outputVal(kObjectType);
  Document::AllocatorType& allocator = doc.GetAllocator();
	outputVal.AddMember("v_id", setJsonParam(iVid, doc), allocator);
	outputVal.AddMember("meeting_id", setJsonParam(strMeetingId, doc), allocator);
	outputVal.AddMember("record_url", setJsonParam(strUrl, doc), allocator);

  outputVal.Accept(writer);
  std::string strMsg = strBuf.GetString();

	post_msg_to_redis_mod(strMsg, strCallId);
  return true;
}

bool XG_SFUController::post_msg_to_redis_mod(std::string strMsg,std::string strCallId)
{
   RedisInfo redisInfo;
   redisInfo.m_strContextId = strCallId.c_str();
	 redisInfo.m_enRedisMethod = EN_REDIS_METHOD::EN_REDIS_METHOD_STREAM;
   redisInfo.m_enRedisEvent = EN_REDIS_EVENT_PUBLISH_TO_UCAAS_SIO;
   redisInfo.m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_SERVICE_CONTROLLER;
   redisInfo.m_strMsg = strMsg.c_str();
   RedisServiceMsg *redisMsg = new RedisServiceMsg();
   if(!redisMsg)
   {
      XGLOG_ERROR("post_msg_to_redis_mod message failed - %s", strCallId.c_str());
      return false;
   }
   redisMsg->set_redis_info(redisInfo);
   redisMsg->setDstModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
   redisMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
   redisMsg->setMsgType(EN_XGATE_MSG_REDIS_PUBLISH);
   this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(redisMsg));

   return true;
}

bool XG_SFUController::handle_msg(ACE_Message_Block *pAmb)
{
 IURModuleMsg *pctrlMsg = NULL;
 pctrlMsg = dynamic_cast<IURModuleMsg *>(pAmb);
 XGLOG_ERROR("$$$$$$ XG_SFUController::handle_msg src module:%d", pctrlMsg->getSrcModuleId());
 switch (pctrlMsg->getSrcModuleId())
 {
 case IURDefines::UR_MODULE_SFU_SERVICE:
 {
  if (pctrlMsg->getDstModuleId() == IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE)
  {
      handle_sfu_ms(pctrlMsg);
  }
  else
  {
      XGLOG_ERROR("Currently this dstModuleId not support in SFU Controller");
      return false;
  }
  break;
 }
 case IURDefines::UR_MODULE_DB_SERVICE:
 {
  handle_db_response(pctrlMsg);
  break;
 }
 case IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE:
 {
  if (pctrlMsg->getDstModuleId() == IURDefines::UR_MODULE_SFU_SERVICE)
  {
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
  XGLOG_ERROR("SFU Controller unsupported src module:%d", pctrlMsg->getSrcModuleId());
  return false;
  break;
 }
 }
 return true;
}

bool XG_SFUController::handleModuleCallbackMsg(IURModuleMsg * pctrlMsg)
{
  // xGateBaseMsg *pMsg = dynamic_cast<xGateBaseMsg*>(pctrlMsg);
  XGLOG_TRACE("XG_SFUController::Inside handleModuleCallbackMsg !");
  if (pctrlMsg) {
		if((IURDefines::UR_MODULE_DB_SERVICE == pctrlMsg->getDstModuleId()) 
				|| (IURDefines::UR_MODULE_REDIS_SERVICE == pctrlMsg->getDstModuleId()))
		{
			arrayModules[pctrlMsg->getDstModuleId()]->pushModuleMsg(pctrlMsg);
		}
		else
		{
		  XGLOG_INFO("$$$$$$ XG_SFUController::handleModuleCallbackMsg src_module:%d",pctrlMsg->getSrcModuleId());
			XGSFUCONTROLLER->putq(dynamic_cast<ACE_Message_Block*>(pctrlMsg));
		}
    XGLOG_TRACE("XG_SFUController::Message Received in handleModuleCallbackMsg!");
  }
  return true;
}

//bool XG_SFUController::initSFUModule(int sfu_id)
bool XG_SFUController::initSFUModule()
{
 IURModule *pSFUService = NULL;
 URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_SFU_SERVICE,pSFUService);
 if(!pSFUService){
    XGLOG_ERROR("Initializing SFU service/proxy module failed !");
    return false;
 }
  //CONFIGSERVICE->m_sfu_id = sfu_id;
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

  //TODO: Initialize Media Processing Module
#if 0
  IURModule *pMediaProcessor = NULL;
  URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_MEDIA_PROCESSING_SERVICE, pMediaProcessor);
  if(!pMediaService) {
    XGLOG_ERROR("Initializing Media processing module failed !");
    return false;
  }
#endif

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

bool XG_SFUController::initDBModule()
{
	//Initialize DB Module  
  IURModule *pDBService = NULL;
  URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_DB_SERVICE, pDBService);
  if(!pDBService) {
    XGLOG_ERROR("Initializing Media service/proxy module failed !");
    return false;
  }

  CONFIGSERVICE->setCallBack(this);
  arrayModules[pDBService->getModuleID()] = pDBService;
  if(!pDBService->initModule(CONFIGSERVICE) == IURDefines::UR_RETURN_ERROR) {
    XGLOG_ERROR("init() failed while initializing 'MediaService' !");
    return false;
  } else {
    XGLOG_INFO("init() successfully initialized 'MediaService' !");
  }

  return true;
}

bool XG_SFUController::initRedisModule()
{
	//REDIS SERVICE MODULE
	IURModule *pRedisService = NULL;
	URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_REDIS_SERVICE, pRedisService);
	if(!pRedisService)
	{
		XGLOG_ERROR("Initializing Redis service module in ivr controller failed !");
		return false;
	}
	XGConfig *pRedisConfig = new XGConfig();
	if(!pRedisConfig)
	{
		XGLOG_ERROR("Initializing configuration for Redis service module in ivr controller failed !");
		return false;
	}
	pRedisConfig->setCallBack(this);
	arrayModules[pRedisService->getModuleID()] = pRedisService;
	if(!pRedisService->initModule(pRedisConfig) == IURDefines::UR_RETURN_ERROR)
	{
		XGLOG_ERROR("init() failed while initializing 'RedisService' !");
		return false;
	}
	else
	{
		XGLOG_INFO("init() successfully initialized 'RedisService' !");
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

Value XG_SFUController::setJsonParam(string &strValue, Document &doc)
{
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
  return retVal;
}

Value XG_SFUController::setJsonParam(int iValue, Document &doc)
{
  Value retVal;
  retVal = iValue;
  return retVal;
}
