#include "URCBController.h"
#include "xGateUtil.h"
#include "URModuleFactory.h"

URCBController::URCBController() : ACE_Task<ACE_MT_SYNCH>()
{
   arrayModules.resize(IURDefines::UR_MAX_LEN);
}

URCBController::~URCBController(void)
{}

bool URCBController::init()
{
   XGLOG_FUNCTION_EN_EX("");

   if (activate(THR_NEW_LWP , 1) == -1 )
   {
      XGLOG_ERROR("URCBController::startThread ACE returned error while spawning");
      return false;
   }
   else
   {
      XGLOG_INFO("URCBController::init Success");
   }
   URENGINE_CONFIG::instance()->init();

   initModule();
   //std::string message = "{\"Msg\":\"VideoMeet_Testing\"}";
   std::string message("");
   for(int i=1; i<=1; i++)
   {
      RedisInfo redisInfo;
      if(i == 1) {
      	redisInfo.m_enRedisEvent = EN_REDIS_EVENT_PUBLISH_TO_UCAAS_SIO;
   	message = "{\"Msg\":\"VideoMeet_Testing_Socket\"}";
      } else if(i == 2){
      	redisInfo.m_enRedisEvent = EN_REDIS_EVENT_PUBLISH_TO_ACD;
   	message = "{\"Msg\":\"VideoMeet_Testing_ACD\"}";
      } else {
      	redisInfo.m_enRedisEvent = EN_REDIS_EVENT_PUBLISH_TO_CCAAS_SIO;
   	message = "{\"Msg\":\"VideoMeet_Testing_CCAAS_CONF_BRIDGE\"}";
      }
      redisInfo.m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_SERVICE_CONTROLLER;
      redisInfo.m_strMsg = message.c_str();
      RedisServiceMsg *redisMsg = new RedisServiceMsg();
      redisMsg->set_redis_info(redisInfo);
      redisMsg->setMsgType(EN_XGATE_MSG_REDIS_PUBLISH);
      redisMsg->setDstModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
      redisMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
      this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(redisMsg));
   }

   return true;
}

int URCBController::svc(void)
{
   XGLOG_FUNCTION_EN_EX("");  
   int rc;
   rc = pthread_setname_np(ACE_Thread::self(), "URCBCntrlr");
   if(rc!=0)
     printf("Failed to set name for URCB controller thread");

   ACE_Message_Block *pAmb = 0;

   while(true)
   {
      ACE_Time_Value tv ((time(NULL) + 20));
      int result = getq(pAmb, &tv);
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
         XGLOG_ERROR("URCBController::svc() Error in Message Queue");
         break;
      }
      if(pAmb) 
      {
         delete pAmb;
         pAmb = NULL;
      }
   }
  
   return 0;
}

bool URCBController::stop(void)
{
   XGLOG_FUNCTION_EN_EX("");
   msg_queue()->deactivate();
   wait();
   return true;
}

bool URCBController::initModule()
{
   XGLOG_FUNCTION_EN_EX("");

   // Scheduler module
   IURModule *pSchedulerService = NULL;
   URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_SCHEDULER_SERVICE, pSchedulerService);
   if(!pSchedulerService) 
   {
      XGLOG_ERROR("Initializing scheduler service module in ACD controller failed !");
      return false;
   }
   XGConfig *pSchedulerConfig = new XGConfig();
   if(!pSchedulerConfig) 
   {
      XGLOG_ERROR("Initializing configuration for scheduler service module in ACD controller failed !");
      return false;
   }
   pSchedulerConfig->setCallBack(this);
   arrayModules[pSchedulerService->getModuleID()] = pSchedulerService;
   if (!pSchedulerService->initModule(pSchedulerConfig) == IURDefines::UR_RETURN_ERROR) 
   {
      XGLOG_ERROR("init() failed while initializing 'SchedulerService' !");
      return false;
   }
   else 
   {
      XGLOG_INFO("init() successfully initialized 'SchedulerService' !");
   }

   //SIP Service Module
   IURModule *pSipSigProcessor = NULL;
   URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE, pSipSigProcessor);
   if (!pSipSigProcessor) 
   {
      XGLOG_ERROR("Initializing Sip Signal Processor in ivr controller failed !");
      return false;
   }
   XGConfig *psipConfig = new XGConfig();
   if (!psipConfig) 
   {
      XGLOG_ERROR("Initializing configuration for Sip Signal Processor in ivr controller failed !");
      return false;
   }
   psipConfig->setCallBack(this);
   arrayModules[pSipSigProcessor->getModuleID()] = pSipSigProcessor;
   if (!pSipSigProcessor->initModule(psipConfig) == IURDefines::UR_RETURN_ERROR) 
   {
      XGLOG_ERROR("init() failed while initializing 'SipSignalProcessor' !");
      return false;
   } 
   else 
   {
      XGLOG_INFO("init() successfully initialized 'SipSignalProcessor' !");
   }

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

   //MBCLink Service Module
   IURModule *pMBCLinkService = NULL;
   URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_MBC_LINK_SERVICE, pMBCLinkService);
   if (!pMBCLinkService) 
   {
      XGLOG_ERROR("Initializing MBCLink service module in ivr controller failed !");
      return false;
   }
   XGConfig *pmbclinkConfig = new XGConfig();
   if (!pmbclinkConfig) 
   {
      XGLOG_ERROR("Initializing configuration for MBClink service module in ivr controller failed !");
      return false;
   }
   pmbclinkConfig->setCallBack(this);
   arrayModules[pMBCLinkService->getModuleID()] = pMBCLinkService;
   if (!pMBCLinkService->initModule(pmbclinkConfig) == IURDefines::UR_RETURN_ERROR) 
   {
      XGLOG_ERROR("init() failed while initializing 'MBCLinkService' !");
      return false;
   } 
   else 
   {
      XGLOG_INFO("init() successfully initialized 'MBCLinkService' !");
   }

   fetch_acd_channel_list();

   return true;
}

bool URCBController::fetch_acd_channel_list()
{
   XGLOG_FUNCTION_EN_EX("");
   auto itr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapPubRedisConnInfo.find(EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_ACD);
   if(itr != URENGINE_CONFIG::instance()->m_redisModProfile.m_mapPubRedisConnInfo.end())
   {
      RedisConnInfo * redisConnInfo = itr->second;
      if(redisConnInfo)
      {
	 m_vAcdChannelList = redisConnInfo->m_vChannel;
      }
   }
   return true;
}

bool URCBController::handle_msg(ACE_Message_Block *pAmb)
{
   XGLOG_FUNCTION_EN_EX("In URCB Controller thread queue");

   xGateBaseMsg *pMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
   switch(pMsg->getMsgType())
   {
      case EN_XGATE_MSG_REDIS_SUBSCRIBE_NOTIFY:
      {
         XGLOG_INFO("URCBController handle_msg received message from REDIS module");
         handleRedisModuleMsg(pMsg);
         break;
      }
      case EN_XGATE_MSG_REDIS_GET_RES:
      {
         XGLOG_INFO("URCBController handle_msg received GET method response from REDIS module");
         handleRedisGetResponse(pMsg);
         break;
      }
      case EN_XGATE_MSG_SIP_MODULE://handle messages from Sip Service Module
      {
         XGLOG_INFO("URCBController handling 'EN_XGATE_MSG_SIP_MODULE' event from sipservice");
         handleSipModuleMsg(pMsg);
         break;
      }
      case EN_XGATE_MSG_MGC_MG_OUT://handle messages from MBC Link Service Module
      {
         XGLOG_INFO("URCBController handling 'EN_XGATE_MSG_MGC_MG_OUT' event from MBC Link Service");
         handleMBCLinkModuleMsg(pMsg);
         break;
      }
      case EN_XGATE_MSG_TIMER://handle messages from Scheduler Service Module
      {
         XGLOG_INFO("URCBController handling 'EN_XGATE_MSG_TIMER' event from scheduler service");
         handle_TimerResponse(pMsg);
         break;
      }
      case EN_XGATE_MSG_SCHEDULER_UPDATE://handle messages from Scheduler Service Module
      {
         XGLOG_INFO("URCBController handling 'EN_XGATE_MSG_TIMER' event from scheduler service");
         handleSchedulerUpdateMsg(pMsg);
         break;
      }
      case EN_XGATE_MSG_UNKNOWN:
      default:
      {
         XGLOG_ERROR("handle_msg failed to handle invalid message");
         break;
      }
   }

   return true;
}

bool URCBController::handleSipModuleMsg(xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateSIPMsg  *ptSipmsg = dynamic_cast<xGateSIPMsg *>(pMsg);
   switch (ptSipmsg->getSipEventType())
   {
      case xGateSIPMsg::SIP_EVENT_NEW_CALL:
      {
         XGLOG_INFO("URCBController handling 'SIP_EVENT_NEW_CALL' event from sipservice");
         handleNewCallNotify(pMsg);
         break;
      }
      case xGateSIPMsg::SIP_EVENT_CALL_CONNECTED:
      {
         XGLOG_INFO("URCBController handling 'SIP_EVENT_CALL_CONNECTED' event from sipservice");
         handleCallConnectedNotify(pMsg);
         break;
      }
      case xGateSIPMsg::SIP_EVENT_CALL_DISCONNECTED:
      {
         XGLOG_INFO("URCBController handling 'SIP_EVENT_CALL_DISCONNECTED' event from sipservice");
         handleCallDisconnectedNotify(pMsg);
         break;
      }
      case xGateSIPMsg::SIP_EVENT_UNKNOWN:
      default:
      {
         XGLOG_ERROR("URCBController failed to handle SipModule message: %d !", ptSipmsg->getSipEventType());
         break;
      }
   }

   return true;
}

bool URCBController::handleNewCallNotify(xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateSIPMsg *ptSipmsg = dynamic_cast<xGateSIPMsg *>(pMsg);
   if (!ptSipmsg) 
   {
      XGLOG_TRACE("handleNewCallNotify failed invalid message type ID error!");
      return false;
   }
   SIPCallInfo &tSipInfo = ptSipmsg->getSipInfoDetails();
   URCBCallContext * ptCtx = new URCBCallContext();
   if (!ptCtx)
   {
      XGLOG_FATAL("OOM Issue. new voice mail call. failed create context memory!");
      return false;
   }
   string strCallId = ptSipmsg->getUid();
   URConfCallUserInfo * ptConfUserInfo = new URConfCallUserInfo();
   ptCtx->m_strCallId = strCallId.c_str();
   if(!getURConfInfoFromUnknownHeaderList(ptCtx,ptSipmsg))
   {
     XGLOG_FATAL("handleNewCallNotify failed. Conference Header validation failed for callid:%s!",strCallId.c_str());
     ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_VALIDATION_FAILED);
     sendErrorNotifyCommand(ptCtx);
     return false;
   }
   string strConfRoomId = ptCtx->m_strConfRoomId;
   string strConfConnId = ptCtx->m_strConfConnId;
   //check for 302 forwarding logic
   if(handle_call_redirection_verification(strConfRoomId,strCallId)) 
   {
     XGLOG_NOTICE("handle_call_redirection_verification forwarding the call");
     return true;
   } 
   URConfRoom * urConfRoom = NULL;
   //check for conf room already exists or not
   if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
   {
      XGLOG_ERROR("handleNewCallNotify getConfRoomCallContext conf room id (%s) not found",\
	    ptCtx->m_strConfRoomId.c_str());
      urConfRoom = new URConfRoom();
      urConfRoom->m_strConfRoomId = strConfRoomId.c_str();
      m_tURConfRoomCtxHndlr.insertConfRoomCallContext(strConfRoomId,urConfRoom);
   }

   m_tURCBCallCtxHndlr.insertURCBCallContext(strCallId, ptCtx);  
   update_callid_in_socketio_user_info(ptCtx);
   check_for_conf_room_create_info(ptCtx);

   ptCtx->m_strCalledNumber = tSipInfo.cnCalled.c_str();
   ptCtx->m_strConfUserExtn = tSipInfo.cnCalling.c_str();
   ptCtx->m_strDomainName   = tSipInfo.domainName.c_str(); 
   ptCtx->m_enumUserDeviceType = tSipInfo.deviceType;
   ptCtx->m_strCallingNumber = tSipInfo.cnCalling.c_str();
   ptCtx->m_strConfUserAgent = tSipInfo.confUserAgent.c_str();
   ptCtx->m_strSDPInfo = ptSipmsg->getSdpInfo().c_str();
   ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_NEW_CALL);

   copyRequiredConfUserInfo(ptCtx, ptConfUserInfo);
   m_tURConfRoomCtxHndlr.add_user_to_conf_call_list(strConfRoomId,ptCtx->m_strCallId,ptConfUserInfo);

   if(!process_msg(ptCtx,pMsg)) 
   {
      XGLOG_ERROR("handleNewCallNotify failed while processing URCBCALL_STATUS_NEW_CALL");
      return false;
   }

   return true;
}

bool URCBController::copyRequiredConfUserInfo(URCBCallContext * ptCtx, URConfCallUserInfo *& ptConfUserInfo)
{
   XGLOG_FUNCTION_EN_EX("");
   ptConfUserInfo->m_strConfRoomID = ptCtx->m_strConfRoomId.c_str();
   ptConfUserInfo->m_strConfCallID = ptCtx->m_strCallId.c_str();
   ptConfUserInfo->m_strConfUserExtn = ptCtx->m_strConfUserExtn.c_str();
   ptConfUserInfo->m_strConfUserAgent = ptCtx->m_strConfUserAgent.c_str();  
   ptConfUserInfo->m_strDomainName = ptCtx->m_strDomainName.c_str();
   ptConfUserInfo->m_enumConfUserType = ptCtx->m_enumConfUserType;
   ptConfUserInfo->m_enumConfUserLogInDeviceType = ptCtx->m_enumUserDeviceType;
   return true;
}

bool URCBController::handle_call_redirection_verification(std::string strConfRoomId, std::string strCallId)
{
   XGLOG_FUNCTION_EN_EX("for confroomid : %s",strConfRoomId.c_str());
   std::string strRedirectIp("");
   if(m_tURConfRoomCtxHndlr.findConfRoomIdInRemoteList(strConfRoomId,strRedirectIp))
   {
      return sendCallRedirectionRequest(strCallId,strRedirectIp);
   }
   return false;
}

bool URCBController::sendCallRedirectionRequest(std::string strCallId, std::string strRedirectIp)
{
   XGLOG_INFO("==> REDIRECT CALL : %s", strCallId.c_str());

   xGateSIPMsg* sipMsg = new xGateSIPMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER,
         IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,
         xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_REDIRECT_CALL,
         xGateSIPMsg::SIP_MSG_TYPE::SIP_MSG_COMMAND);
   if (!sipMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendCallRedirectionRequest failed to create xGateSIPMsg memory !");
      return false;
   }
   sipMsg->setFeatureType(XGATECALLFEATURE::EN_URCBCONF_CALL);
   sipMsg->setDstModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
   sipMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);
   sipMsg->setRedirectIp(strRedirectIp.c_str());
   sipMsg->setUid(strCallId.c_str());
   this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(sipMsg));
   return true;
}

bool URCBController::update_callid_in_socketio_user_info(URCBCallContext *& ptCtx)
{
   XGLOG_FUNCTION_EN_EX("");

   URConfRoom * urConfRoom = NULL;
   std::string strConfRoomId = ptCtx->m_strConfRoomId;
   std::string strCallId = ptCtx->m_strCallId;
   if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom)) 
   {
      XGLOG_ERROR("update_callid_in_socketio_user_info getConfRoomCallContext conf room id (%s) not found",\
            ptCtx->m_strConfRoomId.c_str());
      return false;
   }
   URCBSIOUserInfo * ptSioUserInfo = NULL;
   SIO_USER_INFO_MAP tSIOUserInfoMap = urConfRoom->m_URCBSIOUserInfoList;
   ITR_SIO_USER_INFO_MAP itr = tSIOUserInfoMap.find(ptCtx->m_strConfConnId);
   if(itr != tSIOUserInfoMap.end())
   {
      XGLOG_INFO("update_callid_in_socketio_user_info inserting call_id(%s) for conn_id(%s) in confRoom(%s)",\
            strCallId.c_str(), ptCtx->m_strConfConnId.c_str(), ptCtx->m_strConfRoomId.c_str());
      ptSioUserInfo = itr->second;
      ptSioUserInfo->m_strConfCallId = strCallId.c_str();
      ptCtx->m_strQueueId = ptSioUserInfo->m_tURConfCcaasInfo.m_strQueueId.c_str();
      ptCtx->m_strDomainId = ptSioUserInfo->m_tURConfCcaasInfo.m_strDomainId.c_str();
      ptCtx->m_strCustomerName = ptSioUserInfo->m_tURConfCcaasInfo.m_strCustomerName.c_str();
      ptCtx->m_strCustomerNum = ptSioUserInfo->m_tURConfCcaasInfo.m_strCustomerNum.c_str();
      ptCtx->m_isCcaas = ptSioUserInfo->m_isCcaas;
      CONF_USER_INFO_MAP t_URConfUserInfoList = ptSioUserInfo->m_tURConfUserInfoList;
      ITR_CONF_USER_INFO_MAP userItr = t_URConfUserInfoList.find(ptCtx->m_strConfConnId);  
      if(userItr != t_URConfUserInfoList.end())
      {
         ptCtx->m_iPlanType = userItr->second.m_iPlanType;
         ptCtx->m_strDeviceId = userItr->second.m_strDeviceId.c_str();
         ptCtx->m_strMeetingName = ptSioUserInfo->m_tURConfMeetInfo.m_strMeetingName.c_str();
      }
   }

   return true;
}

bool URCBController::check_for_conf_room_create_info(URCBCallContext *ptCtx)
{
   XGLOG_FUNCTION_EN_EX("");

   std::string strConfRoomId = ptCtx->m_strConfRoomId.c_str();
   std::string strConfConnId = ptCtx->m_strConfConnId.c_str();
   int confCallCount(0);
   m_tURConfRoomCtxHndlr.get_conf_call_list_size(strConfRoomId,confCallCount);
   if(confCallCount == 0) 
   {
     XGLOG_NOTICE("check_for_conf_room_create_info broadcasting conf_room info for confRoom(%s) with confCallCount : (%d)",\
                 ptCtx->m_strConfRoomId.c_str(), confCallCount);
     send_confroom_created_info_to_mbc(ptCtx);
   } 

   return true;
}

bool URCBController::handleCallConnectedNotify(xGateBaseMsg *pMsg) 
{
   XGLOG_FUNCTION_EN_EX("");

   xGateSIPMsg *ptSipmsg = dynamic_cast<xGateSIPMsg *>(pMsg);
   if (!ptSipmsg) 
   {
      XGLOG_ERROR("handleCallConnectedNotify failed invalid message type ID error!");
      return false;
   }
   URCBCallContext * ptCtx = NULL;
   SIPCallInfo &tSipInfo = ptSipmsg->getSipInfoDetails();
   std::string strID = ptSipmsg->getUid();
   if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx)) 
   {
      XGLOG_ERROR("handleCallConnectedNotify get URCBCallContext failed for callid (%s)",\
	    strID.c_str());
      return false;
   }
   std::string strConfRoomId = ptCtx->m_strConfRoomId.c_str();
   std::string strConfConnId = ptCtx->m_strConfConnId.c_str();
   cancel_timer(strConfConnId,EN_SCHEDULER_URCONF_PORT_ALLOC_RES);
   ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_CALL_CONNECTED);

	 URConfRoom * urConfRoom = NULL;
	 if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
	 {
		 XGLOG_ERROR("handleCallConnectedNotify getConfRoomCallContext conf room id (%s) not found",\
				 strConfRoomId.c_str());
		 return false;
	 }

   //Send User Joined Info to Socket IO server
   URCBSIOUserInfo * ptSioUserInfo = NULL;
   if(!m_tURConfRoomCtxHndlr.get_conf_socket_info_from_connid(strConfRoomId.c_str(), strConfConnId.c_str(),ptSioUserInfo)) 
   {
      XGLOG_ERROR("handleCallConnectedNotify get_conf_socket_info_from_connid connid (%s) not found in confRoom:%s",\
      strConfConnId.c_str(),strConfRoomId.c_str());
      return false;
   }
   if(ptCtx->m_enumConfUserType == CONF_USER_TYPE_CUSTOMER)
   {
		get_acd_channel_from_redis(ptCtx);
   }
   else if(ptCtx->m_enumConfUserType == CONF_USER_TYPE_AGENT)
   {
		 if(m_tURConfRoomCtxHndlr.get_acd_channel(strConfRoomId, ptCtx->m_strChannel))
		 {
			 XGLOG_INFO("handleCallConnectedNotify get_acd_channel:%s for conf_room:%s success !", \
					 ptCtx->m_strChannel.c_str(), strConfRoomId.c_str());
			 ptCtx->setCcaasEvent(EN_CCAAS_CB_ACD_CALL_AGENT_CONNECTED);
			 post_ccaas_conf_info_to_acd(ptCtx);
		 }
      else
      {
	 XGLOG_ERROR("handleCallConnectedNotify get_acd_channel for conf_room:%s failed !", strConfRoomId.c_str());
      }
   }

   ptCtx->m_strMeetingName = ptSioUserInfo->m_tURConfMeetInfo.m_strMeetingName.c_str();
   if (ptCtx->m_enumConfUserType == CONF_USER_TYPE_HOST)
   {
      ptSioUserInfo->m_tURConfEvent.m_strEventName = "host_joined";
   }
   else if (ptCtx->m_enumConfUserType == CONF_USER_TYPE_CUSTOMER)
   {
      ptSioUserInfo->m_tURConfEvent.m_strEventName = "customer_joined";
   }
   else if (ptCtx->m_enumConfUserType == CONF_USER_TYPE_AGENT)
   {
      ptSioUserInfo->m_tURConfEvent.m_strEventName = "agent_joined";
   }
   else if (ptCtx->m_enumConfUserType == CONF_USER_TYPE_SCREENSHARE)
   {
      ptSioUserInfo->m_tURConfEvent.m_strEventName = "screenshare_started";
      urConfRoom->isScreenShareActive = true;
   } 
   else 
   {
      ptSioUserInfo->m_tURConfEvent.m_strEventName = "participant_joined";
   }
   post_msg_to_socketio(ptSioUserInfo);
	 if(!urConfRoom->isMeetingTimerSet)
	 {
		 check_for_meeting_duration(strConfRoomId.c_str());
	 }

   return true;
}

bool URCBController::check_for_meeting_duration(std::string strConfRoomId)
{
	 XGLOG_FUNCTION_EN_EX("for meeting_id:%s", strConfRoomId.c_str());
	 std::string strKey("");
   frame_meeting_duration_key(strConfRoomId, strKey);
	 RedisInfo redisInfo;
	 redisInfo.m_strContextId = strConfRoomId.c_str();
	 redisInfo.m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_SERVICE_CONTROLLER;
	 redisInfo.m_strKey = strKey.c_str();
	 redisInfo.m_enRedisEvent = EN_REDIS_EVENT_GET_MEETING_DURATION;
	 RedisServiceMsg *redisMsg = new RedisServiceMsg();
	 redisMsg->set_redis_info(redisInfo);
	 redisMsg->setMsgType(EN_XGATE_MSG_REDIS_GET);
	 redisMsg->setDstModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
	 redisMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
	 this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(redisMsg)); 

   
	 return true;
}

bool URCBController::frame_meeting_duration_key(std::string strConfRoomId, std::string &strKey)
{
  auto itr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapGetRedisConnInfo.find(EN_REDIS_GET_TYPE::EN_REDIS_GET_TYPE_MEETING_DURATION);
	if(itr != URENGINE_CONFIG::instance()->m_redisModProfile.m_mapGetRedisConnInfo.end())
	{
		RedisConnInfo * redisConnInfo = itr->second;
		strKey.assign(redisConnInfo->m_strKey.c_str());
		strKey.append(strConfRoomId.c_str());
		return true;
	}
   return false;
}

bool URCBController::get_acd_channel_from_redis(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid:%s", ptCtx->m_strCallId.c_str());
	std::string strKey("");
   frame_acd_channel_key(ptCtx->m_strDomainId, ptCtx->m_strAcdKey);

	RedisInfo redisInfo;
	redisInfo.m_strContextId = ptCtx->m_strCallId.c_str();
	redisInfo.m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_SERVICE_CONTROLLER;
	redisInfo.m_strKey = ptCtx->m_strAcdKey.c_str();
	redisInfo.m_enRedisEvent = EN_REDIS_EVENT_GET_ACD_CHANNEL;
	RedisServiceMsg *redisMsg = new RedisServiceMsg();
	redisMsg->set_redis_info(redisInfo);
	redisMsg->setMsgType(EN_XGATE_MSG_REDIS_GET);
	redisMsg->setDstModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
	redisMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
	this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(redisMsg));
	return true;
}

bool URCBController::frame_acd_channel_key(std::string strDomainId, std::string &strKey)
{
   auto itr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapGetRedisConnInfo.find(EN_REDIS_GET_TYPE::EN_REDIS_GET_TYPE_ACD_CHANNEL);
	if(itr != URENGINE_CONFIG::instance()->m_redisModProfile.m_mapGetRedisConnInfo.end())
	{
		RedisConnInfo * redisConnInfo = itr->second;
		strKey.assign(redisConnInfo->m_strKey.c_str());
		strKey.append(strDomainId.c_str());
		return true;
	}
   return false;
}

bool URCBController::post_msg_to_socketio(URCBSIOUserInfo *ptSioUserInfo)
{
   XGLOG_FUNCTION_EN_EX("for callid:%s",ptSioUserInfo->m_strConfCallId.c_str());
   string message("");
   ptSioUserInfo->m_tURConfEvent.m_strEventTime = getCurrentUTCTime();
   message = form_socketIO_json_msg(ptSioUserInfo);

   XGLOG_INFO("==> CONFBRIDGE TO SOCKET - message:%s, callid:%s", message.c_str(), ptSioUserInfo->m_strConfCallId.c_str());

   RedisInfo redisInfo;
   redisInfo.m_strContextId = ptSioUserInfo->m_strConfCallId.c_str();
   if(ptSioUserInfo->m_isCcaas)
   {
     redisInfo.m_enRedisEvent = EN_REDIS_EVENT_PUBLISH_TO_CCAAS_SIO;
   }
   else
   {
     redisInfo.m_enRedisEvent = EN_REDIS_EVENT_PUBLISH_TO_UCAAS_SIO;
   }
   redisInfo.m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_SERVICE_CONTROLLER;
   redisInfo.m_strMsg = message.c_str();
   RedisServiceMsg *redisMsg = new RedisServiceMsg();
   if(!redisMsg) 
   {
      XGLOG_ERROR("post_msg_to_socketio message failed - %s", ptSioUserInfo->m_strConfCallId.c_str());
      return false;
   }
   redisMsg->set_redis_info(redisInfo);
   redisMsg->setDstModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
   redisMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
   redisMsg->setMsgType(EN_XGATE_MSG_REDIS_PUBLISH);
   this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(redisMsg));

   return true;
}

bool URCBController::handleCallDisconnectedNotify(xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateSIPMsg *ptSipmsg = dynamic_cast<xGateSIPMsg *>(pMsg);
   if (!ptSipmsg) 
   {
      XGLOG_TRACE("handleCallDisconnectedNotify failed invalid message type ID error!");
      return false;
   }
   SIPCallInfo &tSipInfo = ptSipmsg->getSipInfoDetails();
   string strID = ptSipmsg->getUid();
   URCBCallContext * ptCtx = NULL;
   if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx)) 
   {
      XGLOG_ERROR("handleCallDisconnectedNotify get URCBCallContext failed for callid (%s)",\
                     strID.c_str());
      return false;
   }
   if(ptCtx->m_bIsRecStarted)
   {
      XGLOG_ERROR("handleCallDisconnectedNotify ignored call clearing since recording started for session (%s)",\
                     strID.c_str());
        return false;
   }
   std::string strConfConnId = ptCtx->m_strConfConnId.c_str();   
   ptCtx->m_strSDPInfo = ptSipmsg->getSdpInfo().c_str();
   ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_CALL_DISCONNECTED);
   if(!process_msg(ptCtx)) 
   {
	   XGLOG_ERROR("handleCallDisconnectedNotify failed URCBCALL_STATUS_CALL_DISCONNECTED process_msg failed for callid:%s",strID.c_str());
	   return false;
   }
   handleCallClearing(ptCtx);

   return true;
}

bool URCBController::handleCallClearing(URCBCallContext * ptCtx)
{
   std::string strCallId = ptCtx->m_strCallId;
   std::string strConfRoomId = ptCtx->m_strConfRoomId;

   XGLOG_FUNCTION_EN_EX("for callid - %s !", strCallId.c_str());

   URConfRoom * urConfRoom = NULL;
   if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom)) 
   {
      XGLOG_ERROR("handleCallClearing getConfRoomCallContext conf room id (%s) not found",\
                  ptCtx->m_strConfRoomId.c_str());
      return false;
   } 
   std::string strConfConnId = ptCtx->m_strConfConnId.c_str();
   URCBSIOUserInfo * ptSioUserInfo = NULL;
   if(!m_tURConfRoomCtxHndlr.get_conf_socket_info_from_connid(strConfRoomId.c_str(), strConfConnId.c_str(),ptSioUserInfo))
   {
      XGLOG_ERROR("handleCallClearing get_conf_socket_info_from_connid connid (%s) not found in confRoom:%s",\
	    strConfConnId.c_str(),strConfRoomId.c_str());
      return false;
   }
   if(ptSioUserInfo)
   {
      std::string strEventName = ptSioUserInfo->m_tURConfEvent.m_strEventName.c_str();
      if(strncmp(strEventName.c_str(),"user_action_disconnected",strEventName.length()) == 0)
      {
         ptSioUserInfo->m_tURConfEvent.m_strEventName = "user_disconnected";
      	 post_msg_to_socketio(ptSioUserInfo);
      }
      else if(strncmp(strEventName.c_str(),"user_action_terminated",strEventName.length()) == 0)
      {
         ptSioUserInfo->m_tURConfEvent.m_strEventName = "user_terminated";
      	 post_msg_to_socketio(ptSioUserInfo);
      }
      else if(strncmp(strEventName.c_str(),"participant_remove",strEventName.length()) == 0)
      {
         ptSioUserInfo->m_tURConfEvent.m_strEventName = "participant_removed";
      	 post_msg_to_socketio(ptSioUserInfo);
      }
      else if(strncmp(strEventName.c_str(),"screenshare_stop",strEventName.length()) == 0)
      {
	 urConfRoom->isScreenShareActive = false;
         ptSioUserInfo->m_tURConfEvent.m_strEventName = "screenshare_stopped";
      	 post_msg_to_socketio(ptSioUserInfo);
      }
   }
   m_tURConfRoomCtxHndlr.remove_connid_from_socket_user_info_list(strConfRoomId,strConfConnId);
   m_tURConfRoomCtxHndlr.remove_user_from_conf_call_list(strConfRoomId,strCallId);

   int confCallCount(0);
   m_tURConfRoomCtxHndlr.get_conf_call_list_size(strConfRoomId,confCallCount);
   if(confCallCount == 0) 
   {
			//Cancel the scheduled meeting timer
			cancel_timer(strConfRoomId, EN_SCHEDULER_URCONF_MEETING_DURATION);
      if(!m_tURConfRoomCtxHndlr.deleteConfRoomCallContext(strConfRoomId)) 
      {
         XGLOG_ERROR("handleCallClearing deleteConfRoomCallContext failed - %s", strConfRoomId.c_str());
      } 
      if((ptCtx->m_enumConfUserType == CONF_USER_TYPE_CUSTOMER) || (ptCtx->m_enumConfUserType == CONF_USER_TYPE_AGENT))
      {
	      ptCtx->setCcaasEvent(EN_CCAAS_CB_ACD_CALL_DISCONNECTED);
	      post_ccaas_conf_info_to_acd(ptCtx);
      }
   }

   if(!m_tURCBCallCtxHndlr.DeleteURCBCallContext(strCallId)) 
   {
     XGLOG_ERROR("handleCallClearing DeleteURCBCallContext failed - %s", strCallId.c_str());
   }
 
   return true;
}

bool URCBController::sendCallDisconnectRequest(URCBCallContext * ptCtx)
{
   XGLOG_INFO("==> DISCONNECT CALL - (%s) !", ptCtx->m_strCallId.c_str());
   string strCallId("");
   xGateSIPMsg  *sipMsg = NULL;
   sipMsg = new xGateSIPMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER,
                           IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,
                           xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_CALL_DISCONNECTED,
                           xGateSIPMsg::SIP_MSG_TYPE::SIP_MSG_REQUEST);
   if(!sipMsg) 
   {
      XGLOG_ERROR("OOM Issue. Falied to forward Call Disconnection message from URCB Ctrl to SIP Module !");
      return false;
   }
   sipMsg->setDstModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
   sipMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);
   sipMsg->setFeatureType(XGATECALLFEATURE::EN_URCBCONF_CALL);
   if(ptCtx)
   {
      strCallId = ptCtx->m_strCallId.c_str();
   }
   sipMsg->setUid(strCallId);
   this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(sipMsg));

   return true;
}

bool URCBController::process_msg(URCBCallContext * ptCtx, xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !", ptCtx->m_strCallId.c_str());

   switch (ptCtx->getStatus())
   {
      case URCBCallContext::URCBCALL_STATUS_NEW_CALL:
      {
         XGLOG_INFO("process_msg URCBCALL_STATUS_NEW_CALL - %s", ptCtx->m_strCallId.c_str());
         if(!sendConfPortAllocationRequest(ptCtx,pMsg)) 
         {
            XGLOG_ERROR("process_msg URCBCALL_STATUS_NEW_CALL sendConfPortAllocationRequest failed - %s", ptCtx->m_strCallId.c_str());
         } 
         //Schedule Port Alloc Resp Timer
         URCBTimerData * urcbTimerData = new URCBTimerData();
         urcbTimerData->m_strConfMeetingId = ptCtx->m_strConfRoomId.c_str();
         std::string uid = urcbTimerData->m_strConfConnId = ptCtx->m_strConfConnId.c_str();
         set_timer(uid, urcbTimerData, EN_SCHEDULER_URCONF_PORT_ALLOC_RES, URCONF_PORT_ALLOC_RES_TIMER_VAL);
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_CALL_ANSWER_ALLOW:
      {
         XGLOG_INFO("process_msg URCBCALL_STATUS_CALL_ANSWER_ALLOW - %s", ptCtx->m_strCallId.c_str());
         if(!sendCallAnswerCommand(ptCtx)) 
         {
            XGLOG_ERROR("process_msg URCBCALL_STATUS_CALL_ANSWER_ALLOW sendCallApnswerCommand failed - %s", ptCtx->m_strCallId.c_str());
         } 
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_SDP_UPDATE_REQ:
      {
         XGLOG_INFO("process_msg URCBCallContext::URCBCALL_STATUS_SDP_UPDATE_REQ - %s", ptCtx->m_strCallId.c_str());
         if(!sendConfUserSDPUpdateRequest(ptCtx)) 
         {
            XGLOG_ERROR("process_msg URCBCallContext::URCBCALL_STATUS_SDP_UPDATE_REQ sendConfUserSDPUpdateRequest failed - %s", ptCtx->m_strCallId.c_str());
         } 
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_SDP_UPDATE_RES:
      {
         XGLOG_INFO("process_msg URCBCALL_STATUS_SDP_UPDATE_RES - %s", ptCtx->m_strCallId.c_str());
         if(!sendConfUserSDPUpdateResponse(ptCtx)) 
         {
            XGLOG_ERROR("process_msg URCBCALL_STATUS_SDP_UPDATE_RES sendConfUserSDPUpdateResponse failed - %s", ptCtx->m_strCallId.c_str());
         } 
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_TILE_CHANGE_REQ:
      {
         XGLOG_INFO("process_msg URCBCallContext::URCBCALL_STATUS_TILE_CHANGE_REQ - %s", ptCtx->m_strCallId.c_str());
         if(!sendConfUserTileChangeRequest(ptCtx)) 
         {
            XGLOG_ERROR("process_msg URCBCallContext::URCBCALL_STATUS_TILE_CHANGE_REQ sendConfUserTileChangeRequest failed - %s", ptCtx->m_strCallId.c_str());
         } 
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_TILE_CHANGE_RES:
      {
         XGLOG_INFO("process_msg URCBCALL_STATUS_TILE_CHANGE_RES - %s", ptCtx->m_strCallId.c_str());
         if(!sendConfUserTileChangeResponse(ptCtx)) 
         {
            XGLOG_ERROR("process_msg URCBCALL_STATUS_TILE_CHANGE_RES sendConfUserTileChangeResponse failed - %s", ptCtx->m_strCallId.c_str());
         } 
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_RECORDING_START:
      {
         XGLOG_INFO("process_msg URCBCallContext::URCBCALL_STATUS_RECORDING_START - %s", ptCtx->m_strCallId.c_str());
         if(!sendConfUserRecStartReq(ptCtx)) 
         {
            XGLOG_ERROR("process_msg URCBCallContext::URCBCALL_STATUS_RECORDING_START sendConfUserRecStartReq failed - %s", ptCtx->m_strCallId.c_str());
         } 
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_RECORDING_PAUSE:
      {
         XGLOG_INFO("process_msg URCBCallContext::URCBCALL_STATUS_RECORDING_PAUSE - %s", ptCtx->m_strCallId.c_str());
         if(!sendConfUserRecPauseReq(ptCtx)) 
         {
            XGLOG_ERROR("process_msg URCBCallContext::URCBCALL_STATUS_RECORDING_PAUSE sendConfUserRecPauseReq failed - %s", ptCtx->m_strCallId.c_str());
         } 
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_RECORDING_RESUME:
      {
         XGLOG_INFO("process_msg URCBCallContext::URCBCALL_STATUS_RECORDING_RESUME - %s", ptCtx->m_strCallId.c_str());
         if(!sendConfUserRecResumeReq(ptCtx)) 
         {
            XGLOG_ERROR("process_msg URCBCallContext::URCBCALL_STATUS_RECORDING_RESUME sendConfUserRecResumeReq failed - %s", ptCtx->m_strCallId.c_str());
         } 
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_RECORDING_STOP:
      {
         XGLOG_INFO("process_msg URCBCallContext::URCBCALL_STATUS_RECORDING_STOP - %s", ptCtx->m_strCallId.c_str());
         if(!sendConfUserRecStopReq(ptCtx)) 
         {
            XGLOG_ERROR("process_msg URCBCallContext::URCBCALL_STATUS_RECORDING_STOP sendConfUserRecStopReq failed - %s", ptCtx->m_strCallId.c_str());
         } 
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_SCREENSHARE_VIDEO_REQ:
      {
         XGLOG_INFO("process_msg URCBCallContext::URCBCALL_STATUS_SCREENSHARE_VIDEO_REQ - %s", ptCtx->m_strCallId.c_str());
         if(!sendScreenshareVideoRequest(ptCtx)) 
         {
            XGLOG_ERROR("process_msg URCBCallContext::URCBCALL_STATUS_SCREENSHARE_VIDEO_REQ sendScreenshareVideoRequest failed - %s", ptCtx->m_strCallId.c_str());
         } 
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_ACTIVE_SPEAKER_VIDEO_REQ:
      {
         XGLOG_INFO("process_msg URCBCallContext::URCBCALL_STATUS_ACTIVE_SPEAKER_VIDEO_REQ - %s", ptCtx->m_strCallId.c_str());
         if(!sendActiveSpeakerVideoRequest(ptCtx)) 
         {
            XGLOG_ERROR("process_msg URCBCallContext::URCBCALL_STATUS_ACTIVE_SPEAKER_VIDEO_REQ sendActiveSpeakerVideoRequest failed - %s", ptCtx->m_strCallId.c_str());
         } 
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_GRID_VIEW_VIDEO_REQ:
      {
	 XGLOG_INFO("process_msg URCBCallContext::URCBCALL_STATUS_GRID_VIEW_VIDEO_REQ - %s", ptCtx->m_strCallId.c_str());
	 if(!sendGridViewVideoRequest(ptCtx))
	 {
	    XGLOG_ERROR("process_msg URCBCallContext::URCBCALL_STATUS_GRID_VIEW_VIDEO_REQ sendGridViewVideoRequest failed - %s", ptCtx->m_strCallId.c_str());
	 }
	 break;
      }
      case URCBCallContext::URCBCALL_STATUS_CONF_DEL_PARTICIPANT:
      {
         XGLOG_INFO("process_msg URCBCALL_STATUS_CONF_DEL_PARTICIPANT - %s", ptCtx->m_strCallId.c_str());
         if(!sendConfDeleteParticipantRequest(ptCtx)) 
         {
            XGLOG_ERROR("process_msg URCBCALL_STATUS_CONF_DEL_PARTICIPANT sendConfDeleteParticipantRequest failed - %s", ptCtx->m_strCallId.c_str());
         } 
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_CALL_DISCONNECTED:
      {
         XGLOG_INFO("process_msg URCBCALL_STATUS_CALL_DISCONNECTED - %s", ptCtx->m_strCallId.c_str());
         if(!sendConfMediaDeAllocationRequest(ptCtx)) 
         {
            XGLOG_ERROR("process_msg URCBCALL_STATUS_CALL_DISCONNECTED sendConfMediaDeAllocationRequest failed - %s", ptCtx->m_strCallId.c_str());
         } 
         break;
      }
      default:
      {
	      break;
      }
   }

   return true;
}

bool URCBController::sendConfPortAllocationRequest(URCBCallContext * ptCtx,xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !", ptCtx->m_strCallId.c_str());

   xGateSIPMsg *ptSipmsg = dynamic_cast<xGateSIPMsg *>(pMsg);
   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = new xGateMBCLinkServiceMsg();
   if (!pMBCLinkServiceMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendConfPortAllocationRequest failed to create xGateMBCLinkServiceMsg memory !");
      return false;
   }
   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   mediaDetail.mgmsg_type = EN_XGATE_MG_MEDIA_CREATE;
   mediaDetail.is_ccaas = ptCtx->m_isCcaas;
   mediaDetail.mg_ip = chooseMbcIp(ptCtx->m_strConfRoomId);
   ptCtx->m_strMbcIp =  mediaDetail.mg_ip;

   fillMediaDetailForMediaRequest(ptCtx,mediaDetail);
   fillSDPInfoForMediaRequest(ptSipmsg,mediaDetail);

   pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_PORT_ALLOCATION_REQ);
   pMBCLinkServiceMsg->setUid(ptCtx->m_strCallId.c_str());
   post_msg_to_mbclink_mod(pMBCLinkServiceMsg);   

   return true;
}

bool URCBController::sendConfUserSDPUpdateRequest(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !", ptCtx->m_strCallId.c_str());

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = new xGateMBCLinkServiceMsg();
   if (!pMBCLinkServiceMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendConfUserSDPUpdateRequest failed to create xGateMBCLinkServiceMsg memory !");
      return false;
   }
   std::string strConfRoomId("");
   strConfRoomId = ptCtx->m_strConfRoomId.c_str();

   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   mediaDetail.mgmsg_type = EN_XGATE_MG_SDP_UPDATE_REQUEST ;
   mediaDetail.mg_ip = ptCtx->m_strMbcIp;

   fillMediaDetailForMediaRequest(ptCtx,mediaDetail);
   fillParticipantInfoForMediaRequest(mediaDetail,strConfRoomId,ptCtx);

   pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_USER_SDP_UPDATE_REQ);
   pMBCLinkServiceMsg->setUid(ptCtx->m_strCallId.c_str());
   post_msg_to_mbclink_mod(pMBCLinkServiceMsg);

   return true;
}

bool URCBController::sendConfUserTileChangeRequest(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !", ptCtx->m_strCallId.c_str());

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = new xGateMBCLinkServiceMsg();
   if (!pMBCLinkServiceMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendConfUserTileChangeRequest failed to create xGateMBCLinkServiceMsg memory !");
      return false;
   }
   std::string strConfRoomId("");
   strConfRoomId = ptCtx->m_strConfRoomId.c_str();
   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   mediaDetail.mgmsg_type = EN_XGATE_MG_TILE_CHANGE_REQUEST ;
   mediaDetail.mg_ip = ptCtx->m_strMbcIp;

   fillMediaDetailForMediaRequest(ptCtx,mediaDetail);
   fillParticipantInfoForMediaRequest(mediaDetail,strConfRoomId,ptCtx);

   pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_USER_TILE_CHANGE_REQ);
   pMBCLinkServiceMsg->setUid(ptCtx->m_strCallId.c_str());
   post_msg_to_mbclink_mod(pMBCLinkServiceMsg);

   return true;
}

bool URCBController::sendConfUserRecStartReq(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !", ptCtx->m_strCallId.c_str());

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = new xGateMBCLinkServiceMsg();
   if (!pMBCLinkServiceMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendConfUserRecStartReq failed to create xGateMBCLinkServiceMsg memory !");
      return false;
   }
   std::string strConfRoomId("");
   strConfRoomId = ptCtx->m_strConfRoomId.c_str();

   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   mediaDetail.mgmsg_type = EN_XGATE_MG_RECORDING_EVENT;
   mediaDetail.sdp_str = "";
   mediaDetail.browser_type = ptCtx->m_strBrowserType.c_str();
   mediaDetail.record_file = ptCtx->m_strRecFilePath.c_str();
   mediaDetail.media_event = EN_MEDIA_EVENT_RECORD_START;
   mediaDetail.mg_ip = ptCtx->m_strMbcIp;
 
   fillMediaDetailForMediaRequest(ptCtx,mediaDetail);
   fillParticipantInfoForMediaRequest(mediaDetail,strConfRoomId,ptCtx);

   pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_USER_RECORDING_REQ);
   pMBCLinkServiceMsg->setUid(ptCtx->m_strCallId.c_str());
   post_msg_to_mbclink_mod(pMBCLinkServiceMsg);

   return true;
}

bool URCBController::sendConfUserRecPauseReq(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !", ptCtx->m_strCallId.c_str());

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = new xGateMBCLinkServiceMsg();
   if (!pMBCLinkServiceMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendConfUserRecPauseReq failed to create xGateMBCLinkServiceMsg memory !");
      return false;
   }

   std::string strConfRoomId("");
   strConfRoomId = ptCtx->m_strConfRoomId.c_str();

   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   mediaDetail.mgmsg_type = EN_XGATE_MG_RECORDING_EVENT;
   mediaDetail.sdp_str = "";
   mediaDetail.media_event = EN_MEDIA_EVENT_RECORD_PAUSE;
   mediaDetail.mg_ip = ptCtx->m_strMbcIp;

   fillMediaDetailForMediaRequest(ptCtx,mediaDetail);
   fillParticipantInfoForMediaRequest(mediaDetail,strConfRoomId,ptCtx);

   pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_USER_RECORDING_REQ);
   pMBCLinkServiceMsg->setUid(ptCtx->m_strCallId.c_str());
   post_msg_to_mbclink_mod(pMBCLinkServiceMsg);

   return true;
}

bool URCBController::sendConfUserRecResumeReq(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !", ptCtx->m_strCallId.c_str());

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = new xGateMBCLinkServiceMsg();
   if (!pMBCLinkServiceMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendConfUserRecResumeReq failed to create xGateMBCLinkServiceMsg memory !");
      return false;
   }
   std::string strConfRoomId("");
   strConfRoomId = ptCtx->m_strConfRoomId.c_str();

   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   mediaDetail.mgmsg_type = EN_XGATE_MG_RECORDING_EVENT;
   mediaDetail.sdp_str = "";
   mediaDetail.media_event = EN_MEDIA_EVENT_RECORD_RESUME;
   mediaDetail.call_dir = 1;
   mediaDetail.mg_ip = ptCtx->m_strMbcIp;

   fillMediaDetailForMediaRequest(ptCtx,mediaDetail);
   fillParticipantInfoForMediaRequest(mediaDetail,strConfRoomId,ptCtx);

   pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_USER_RECORDING_REQ);
   pMBCLinkServiceMsg->setUid(ptCtx->m_strCallId.c_str());
   post_msg_to_mbclink_mod(pMBCLinkServiceMsg);

   return true;
}

bool URCBController::sendConfUserRecStopReq(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !", ptCtx->m_strCallId.c_str());

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = new xGateMBCLinkServiceMsg();
   if (!pMBCLinkServiceMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendConfUserRecStopReq failed to create xGateMBCLinkServiceMsg memory !");
      return false;
   }

   std::string strConfRoomId("");
   strConfRoomId = ptCtx->m_strConfRoomId.c_str();

   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   mediaDetail.mgmsg_type = EN_XGATE_MG_RECORDING_EVENT;
   mediaDetail.sdp_str = "";
   mediaDetail.media_event = EN_MEDIA_EVENT_RECORD_STOP;
   mediaDetail.record_file = ptCtx->m_strRecFilePath.c_str();
   mediaDetail.record_url = ptCtx->m_strRecUrl.c_str();
   mediaDetail.sip_id = ptCtx->m_strSipId.c_str();
   mediaDetail.device_id = ptCtx->m_strDeviceId.c_str();
   mediaDetail.meeting_name = ptCtx->m_strMeetingName.c_str();
   mediaDetail.mg_ip = ptCtx->m_strMbcIp;

   fillMediaDetailForMediaRequest(ptCtx,mediaDetail);
   fillParticipantInfoForMediaRequest(mediaDetail,strConfRoomId,ptCtx);
   fillURMeetUserInfoForMediaRequest(mediaDetail,strConfRoomId,ptCtx);

   pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_USER_RECORDING_REQ);
   pMBCLinkServiceMsg->setUid(ptCtx->m_strCallId.c_str());
   post_msg_to_mbclink_mod(pMBCLinkServiceMsg);

   return true;
}

bool URCBController::sendScreenshareVideoRequest(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !", ptCtx->m_strCallId.c_str());

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = new xGateMBCLinkServiceMsg();
   if (!pMBCLinkServiceMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendScreenshareVideoRequest failed to create xGateMBCLinkServiceMsg memory !");
      return false;
   }
   std::string strConfRoomId("");
   strConfRoomId = ptCtx->m_strConfRoomId.c_str();

   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   mediaDetail.mgmsg_type = EN_XGATE_MG_SCREENSHARE_VIDEO_REQUEST;
   mediaDetail.mg_ip = ptCtx->m_strMbcIp;
 
   fillMediaDetailForMediaRequest(ptCtx,mediaDetail);
   fillParticipantInfoForMediaRequest(mediaDetail,strConfRoomId,ptCtx);

   pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_SCREENSHARE_VIDEO_REQ);
   pMBCLinkServiceMsg->setUid(ptCtx->m_strCallId.c_str());
   post_msg_to_mbclink_mod(pMBCLinkServiceMsg);

   return true;
}

bool URCBController::sendActiveSpeakerVideoRequest(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !", ptCtx->m_strCallId.c_str());

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = new xGateMBCLinkServiceMsg();
   if (!pMBCLinkServiceMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendActiveSpeakerVideoRequest failed to create xGateMBCLinkServiceMsg memory !");
      return false;
   }

   std::string strConfRoomId("");
   strConfRoomId = ptCtx->m_strConfRoomId.c_str();

   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   mediaDetail.mgmsg_type = EN_XGATE_MG_ACTIVE_SPEAKER_VIDEO_REQUEST;
   mediaDetail.mg_ip = ptCtx->m_strMbcIp;
 
   fillMediaDetailForMediaRequest(ptCtx,mediaDetail);
   fillParticipantInfoForMediaRequest(mediaDetail,strConfRoomId,ptCtx);

   pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_ACTIVE_SPEAKER_VIDEO_REQ);
   pMBCLinkServiceMsg->setUid(ptCtx->m_strCallId.c_str());
   post_msg_to_mbclink_mod(pMBCLinkServiceMsg);

   return true;
}

bool URCBController::sendGridViewVideoRequest(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !", ptCtx->m_strCallId.c_str());

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = new xGateMBCLinkServiceMsg();
   if (!pMBCLinkServiceMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendGridViewVideoRequest failed to create xGateMBCLinkServiceMsg memory !");
      return false;
   }

   std::string strConfRoomId("");
   strConfRoomId = ptCtx->m_strConfRoomId.c_str();

   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   mediaDetail.mgmsg_type = EN_XGATE_MG_GRID_VIEW_VIDEO_REQUEST;
   mediaDetail.mg_ip = ptCtx->m_strMbcIp;
 
   fillMediaDetailForMediaRequest(ptCtx,mediaDetail);
   fillParticipantInfoForMediaRequest(mediaDetail,strConfRoomId,ptCtx);

   pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_GRID_VIEW_VIDEO_REQ);
   pMBCLinkServiceMsg->setUid(ptCtx->m_strCallId.c_str());
   post_msg_to_mbclink_mod(pMBCLinkServiceMsg);

   return true;
}

bool URCBController::sendConfRoomActiveSpeakerInfo(std::string strConfRoomId)
{
   XGLOG_FUNCTION_EN_EX("for confroom - %s !", strConfRoomId.c_str());

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = new xGateMBCLinkServiceMsg();
   if (!pMBCLinkServiceMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendConfRoomActiveSpeakerInfo failed to create xGateMBCLinkServiceMsg memory !");
      return false;
   }

   URConfRoom * urConfRoom = NULL;
   if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom)) {
      XGLOG_ERROR("sendConfRoomActiveSpeakerInfo getConfRoomCallContext conf room id (%s) not found", strConfRoomId.c_str());
      return false;
   }

	URActiveSpeakerInfo t_urActiveSpeakerInfo = urConfRoom->m_URActiveSpeakerInfo;
   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   mediaDetail.mgmsg_type = EN_XGATE_MG_ACTIVE_SPEAKER_INFO;
   mediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
   mediaDetail.mgresource_id = strConfRoomId.c_str();
   mediaDetail.call_id = t_urActiveSpeakerInfo.m_strCallId.c_str();
   mediaDetail.call_dir = 1;
   mediaDetail.mg_ip = urConfRoom->m_strMbcIp;

   pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_ACTIVE_SPEAKER_INFO);
   pMBCLinkServiceMsg->setUid(t_urActiveSpeakerInfo.m_strCallId.c_str());
   post_msg_to_mbclink_mod(pMBCLinkServiceMsg);

   return true;
}

bool URCBController::sendConfDeleteParticipantRequest(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !", ptCtx->m_strCallId.c_str());

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = new xGateMBCLinkServiceMsg();
   if (!pMBCLinkServiceMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendConfDeleteParticipantRequest failed to create xGateMBCLinkServiceMsg memory !");
      return false;
   }
 
   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   mediaDetail.mgmsg_type = EN_XGATE_MG_MEDIA_DELETE;
   mediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
   mediaDetail.mgresource_id = ptCtx->m_strConfRoomId.c_str();
   mediaDetail.call_id = ptCtx->m_strCallId.c_str();
	if(ptCtx->m_enumConfUserType == CONF_USER_TYPE_CUSTOMER)
	{
   	mediaDetail.joiner_type = 2;
	}
	else if(ptCtx->m_enumConfUserType == CONF_USER_TYPE_AGENT)
	{
   	mediaDetail.joiner_type = 1;
	}
	else
	{
   	mediaDetail.joiner_type = ptCtx->m_enumConfUserType;
	}
   mediaDetail.call_dir = 1;
   mediaDetail.mg_ip = ptCtx->m_strMbcIp;

   pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_DEL_PARTICIPANT_REQ);
   pMBCLinkServiceMsg->setUid(ptCtx->m_strCallId.c_str());
   post_msg_to_mbclink_mod(pMBCLinkServiceMsg);   

   return true;
}

bool URCBController::sendConfMediaDeAllocationRequest(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !",ptCtx->m_strCallId.c_str());

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = new xGateMBCLinkServiceMsg();
   if (!pMBCLinkServiceMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendConfMediaDeAllocationRequest failed to create xGateMBCLinkServiceMsg memory !");
      return false;
   }

   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   std::string strConfRoomId = ptCtx->m_strConfRoomId.c_str();
   mediaDetail.mg_ip = ptCtx->m_strMbcIp;
   int confCallCount(0);
   m_tURConfRoomCtxHndlr.get_conf_call_list_size(strConfRoomId,confCallCount);
   if(confCallCount > 1) 
   {
     mediaDetail.mgmsg_type = EN_XGATE_MG_MEDIA_DELETE;
     pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_DEL_PARTICIPANT_REQ);
   } 
   else 
   {
     mediaDetail.mgmsg_type = EN_XGATE_MG_RELEASE;
     pMBCLinkServiceMsg->setOpCode(EN_MBC_LINK_EVENT_CONF_DELETE_REQ);
     send_confroom_deleted_info_to_mbc(ptCtx);
   }
   mediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
   mediaDetail.mgresource_id = ptCtx->m_strConfRoomId.c_str();
   mediaDetail.call_id = ptCtx->m_strCallId.c_str();
   if(ptCtx->m_enumConfUserType == CONF_USER_TYPE_CUSTOMER)
   {
      mediaDetail.joiner_type = 2;
   }
   else if(ptCtx->m_enumConfUserType == CONF_USER_TYPE_AGENT)
   {
      mediaDetail.joiner_type = 1;
   }
   else
   {
      mediaDetail.joiner_type = ptCtx->m_enumConfUserType;
   }
   mediaDetail.call_dir = 1;

   pMBCLinkServiceMsg->setUid(ptCtx->m_strCallId.c_str());
   post_msg_to_mbclink_mod(pMBCLinkServiceMsg);   

   return true;
}

bool URCBController::post_msg_to_mbclink_mod(xGateMBCLinkServiceMsg *& pMBCLinkServiceMsg)
{
   if(!pMBCLinkServiceMsg)
   {
      XGLOG_ERROR("post_msg_to_mbclink_mod failed. MBCLink service message is NULL");
      return false;
   }
   pMBCLinkServiceMsg->setDstModuleId(IURDefines::UR_MODULE_MBC_LINK_SERVICE);
   pMBCLinkServiceMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
   pMBCLinkServiceMsg->setMsgType(EN_XGATE_MSG_SERVICE_CONTROLLER);
   this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMBCLinkServiceMsg));   
   return true;   
}

bool URCBController::fillMediaDetailForMediaRequest(URCBCallContext * ptCtx, MgMediaDetail &mediaDetail)
{
   mediaDetail.call_type = EN_XGATE_CALL_TYPE_CONFERENCE_VIDEO;
   mediaDetail.mgresource_id = ptCtx->m_strConfRoomId.c_str();
   mediaDetail.call_id = ptCtx->m_strCallId.c_str();
   mediaDetail.sdp_str = ptCtx->m_strSDPInfo.c_str();
   mediaDetail.conn_id = ptCtx->m_strConfConnId.c_str();
   mediaDetail.device_id = ptCtx->m_strDeviceId.c_str();
   if(ptCtx->m_enumConfUserType == CONF_USER_TYPE_CUSTOMER)
   {
      mediaDetail.joiner_type = 2;
   }
   else if(ptCtx->m_enumConfUserType == CONF_USER_TYPE_AGENT)
   {
      mediaDetail.joiner_type = 1;
   }
   else
   {
      mediaDetail.joiner_type = ptCtx->m_enumConfUserType;
   }
   mediaDetail.plan_type = ptCtx->m_iPlanType;
   mediaDetail.screenshare_id = ptCtx->m_strSSUserCallId;
   mediaDetail.dst_rtp_ep = ptCtx->m_strOrgCallId.c_str();
   mediaDetail.org_rtp_ep = ptCtx->m_strCallId.c_str();
   mediaDetail.call_dir = 1;
   return true;
}

bool URCBController::fillParticipantInfoForMediaRequest(MgMediaDetail &mediaDetail,std::string &strConfRoomId,URCBCallContext * ptCtx)
{
   URConfRoom * urConfRoom = NULL;
   if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
   {
      XGLOG_ERROR("fillParticipantInfoForMediaRequest getConfRoomCallContext conf room id (%s) not found",\
      strConfRoomId.c_str());
      return false;
   }
   for (int i=0; i < ptCtx->m_dequeURConfParticipantList.size(); i++)
   {
      URCONF_PARTICIPANT_INFO participantInfo = ptCtx->m_dequeURConfParticipantList[i];
      ParticipantDetail participantDetail;
      participantDetail.m_callId = participantInfo.m_strCallId.c_str();
      if(strcmp(participantInfo.m_strUserType.c_str(),"host") == 0) 
      {
         participantDetail.m_joinerType = 1;
      } 
      else if(strcmp(participantInfo.m_strUserType.c_str(),"screenshare") == 0)
      {
         participantDetail.m_joinerType = 3;
      } 
      else 
      {
         participantDetail.m_joinerType = 2;
      }
      mediaDetail.participantList.push_back(participantDetail);
   }
   return true;
}

bool URCBController::fillURMeetUserInfoForMediaRequest(MgMediaDetail &mediaDetail,std::string &strConfRoomId,URCBCallContext * ptCtx)
{
   URConfRoom * urConfRoom = NULL;
   if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom)) 
   {
      XGLOG_ERROR("fillURMeetUserInfoForMediaRequest getConfRoomCallContext conf room id (%s) not found",\
      strConfRoomId.c_str());
      return false;
   }
   for (int i=0; i < ptCtx->m_dequeURMeetUserList.size(); i++)
   {
      URMEET_USER_INFO userInfo = ptCtx->m_dequeURMeetUserList[i];
      URMeetUserDetail userDetail;
      userDetail.m_strEmail = userInfo.m_strEmail.c_str();
      userDetail.m_strProfileImg = userInfo.m_strProfileImg.c_str();
      userDetail.m_strSipId = userInfo.m_strSipId.c_str();
      userDetail.m_strName = userInfo.m_strName.c_str();
      userDetail.m_strExtn = userInfo.m_strExtn.c_str();
      userDetail.m_strUserType = userInfo.m_strUserType;
      userDetail.m_iRoleType = userInfo.m_iRoleType;
      mediaDetail.meetUserList.push_back(userDetail);
   }
   return true;
}

bool URCBController::fillSDPInfoForMediaRequest(xGateSIPMsg *ptSipmsg,MgMediaDetail &mediaDetail)
{ 
  SIPCallInfo &tSipInfo = ptSipmsg->getSipInfoDetails(); 
  for (int i = 0 ; i < 2 ; i++)
  {
    if(tSipInfo.m_sdpInfoDetails[i].mediatype == 2)
    {
      mediaDetail.audioDetail.m_mediaType =  tSipInfo.m_sdpInfoDetails[i].mediatype;
      mediaDetail.audioDetail.m_clientIp = tSipInfo.m_sdpInfoDetails[i].ipaddress;
      mediaDetail.audioDetail.m_clientPort = tSipInfo.m_sdpInfoDetails[i].port;
      mediaDetail.audioDetail.m_relayIp = tSipInfo.m_sdpInfoDetails[i].relayip;
      mediaDetail.audioDetail.m_rflxIp = tSipInfo.m_sdpInfoDetails[i].reflexip;
      mediaDetail.audioDetail.m_relayPort = tSipInfo.m_sdpInfoDetails[i].relayport;
      mediaDetail.audioDetail.m_rflxPort = tSipInfo.m_sdpInfoDetails[i].reflexport;

      mediaDetail.audioDetail.m_secureDetail.m_rIceUfrag = tSipInfo.m_sdpInfoDetails[i].iceufrag;
      mediaDetail.audioDetail.m_secureDetail.m_rIcePwd = tSipInfo.m_sdpInfoDetails[i].icepwd;
      mediaDetail.audioDetail.m_secureDetail.m_rFingerPrint = tSipInfo.m_sdpInfoDetails[i].fingerprint;
      mediaDetail.audioDetail.m_secureDetail.m_rSsrc = tSipInfo.m_sdpInfoDetails[i].ssrc;
      mediaDetail.audioDetail.m_secureDetail.m_rCname = tSipInfo.m_sdpInfoDetails[i].cname;
      mediaDetail.audioDetail.m_secureDetail.m_rMsLabel = tSipInfo.m_sdpInfoDetails[i].mslabel;
      mediaDetail.audioDetail.m_secureDetail.m_rLabel = tSipInfo.m_sdpInfoDetails[i].label;

      mediaDetail.audioDetail.m_ptime = tSipInfo.m_sdpInfoDetails[i].ptime;

      mediaDetail.audioDetail.m_codec = tSipInfo.m_sdpInfoDetails[i].avcodec.at(0)->payloadtype;
      mediaDetail.audioDetail.m_codecName = tSipInfo.m_sdpInfoDetails[i].avcodec.at(0)->name;
      mediaDetail.audioDetail.m_fmtp = tSipInfo.m_sdpInfoDetails[i].avcodec.at(0)->fmtp;

    } 
    else if(tSipInfo.m_sdpInfoDetails[i].mediatype == 3)
    {
      mediaDetail.videoDetail.m_mediaType =  tSipInfo.m_sdpInfoDetails[i].mediatype;
      mediaDetail.videoDetail.m_clientIp = tSipInfo.m_sdpInfoDetails[i].ipaddress;
      mediaDetail.videoDetail.m_clientPort = tSipInfo.m_sdpInfoDetails[i].port;
      mediaDetail.videoDetail.m_relayIp = tSipInfo.m_sdpInfoDetails[i].relayip;
      mediaDetail.videoDetail.m_rflxIp = tSipInfo.m_sdpInfoDetails[i].reflexip;
      mediaDetail.videoDetail.m_relayPort = tSipInfo.m_sdpInfoDetails[i].relayport;
      mediaDetail.videoDetail.m_rflxPort = tSipInfo.m_sdpInfoDetails[i].reflexport;
      mediaDetail.videoDetail.m_ptime = tSipInfo.m_sdpInfoDetails[i].ptime;
      
      mediaDetail.videoDetail.m_secureDetail.m_rIceUfrag = tSipInfo.m_sdpInfoDetails[i].iceufrag;
      mediaDetail.videoDetail.m_secureDetail.m_rIcePwd = tSipInfo.m_sdpInfoDetails[i].icepwd;
      mediaDetail.videoDetail.m_secureDetail.m_rFingerPrint = tSipInfo.m_sdpInfoDetails[i].fingerprint;
      mediaDetail.videoDetail.m_secureDetail.m_rSsrc = tSipInfo.m_sdpInfoDetails[i].ssrc;
      mediaDetail.videoDetail.m_secureDetail.m_rCname = tSipInfo.m_sdpInfoDetails[i].cname;
      mediaDetail.videoDetail.m_secureDetail.m_rMsLabel = tSipInfo.m_sdpInfoDetails[i].mslabel;
      mediaDetail.videoDetail.m_secureDetail.m_rLabel = tSipInfo.m_sdpInfoDetails[i].label;
      
      mediaDetail.media_proto = tSipInfo.m_sdpInfoDetails[i].mediamode;
      mediaDetail.out_proto = tSipInfo.m_sdpInfoDetails[i].outproto;
      mediaDetail.videoDetail.m_codec = tSipInfo.m_sdpInfoDetails[i].avcodec.at(0)->payloadtype;
      mediaDetail.videoDetail.m_codecName = tSipInfo.m_sdpInfoDetails[i].avcodec.at(0)->name;
      mediaDetail.videoDetail.m_fmtp = tSipInfo.m_sdpInfoDetails[i].avcodec.at(0)->fmtp;
    }
  } 
  return true;
}

bool URCBController::send_confroom_created_info_to_mbc(URCBCallContext * ptCtx)
{
   XGLOG_INFO("==> CONF_ROOM_CREATED_INFO - callid:(%s) and roomid:(%s)", ptCtx->m_strCallId.c_str(), ptCtx->m_strConfRoomId.c_str());

   URCBBroadCastMsg * ptURCBBroadCastMsg = new URCBBroadCastMsg();
   ptURCBBroadCastMsg->m_strMsgUuid = ptCtx->m_strConfRoomId.c_str();
   ptURCBBroadCastMsg->m_eBroadCastMsgType = URCONF_BROADCAST_MSG_ROOM_CREATED;
   ptURCBBroadCastMsg->m_tURConfRoomInfo.m_strConfBridgeIp = URENGINE_CONFIG::instance()->m_localIp4Addr;
   ptURCBBroadCastMsg->m_tURConfRoomInfo.m_strConfRoomId = ptCtx->m_strConfRoomId.c_str();
   
   std::string strJsonData = form_conf_room_info_json_msg(ptURCBBroadCastMsg);
   post_msg_to_confbridge(strJsonData.c_str());
   delete ptURCBBroadCastMsg;
   ptURCBBroadCastMsg = NULL;

   return true;
}

bool URCBController::send_confroom_deleted_info_to_mbc(URCBCallContext * ptCtx)
{
   XGLOG_INFO("==> CONF_ROOM_DELETED_INFO - callid:(%s) and roomid:(%s)", ptCtx->m_strCallId.c_str(), ptCtx->m_strConfRoomId.c_str());

   URCBBroadCastMsg * ptURCBBroadCastMsg = new URCBBroadCastMsg();
   ptURCBBroadCastMsg->m_strMsgUuid = ptCtx->m_strConfRoomId.c_str();
   ptURCBBroadCastMsg->m_eBroadCastMsgType = URCONF_BROADCAST_MSG_ROOM_DELETED;
   ptURCBBroadCastMsg->m_tURConfRoomInfo.m_strConfBridgeIp = URENGINE_CONFIG::instance()->m_localIp4Addr;
   ptURCBBroadCastMsg->m_tURConfRoomInfo.m_strConfRoomId = ptCtx->m_strConfRoomId.c_str();

   std::string strJsonData = form_conf_room_info_json_msg(ptURCBBroadCastMsg);
   post_msg_to_confbridge(strJsonData.c_str());
   delete ptURCBBroadCastMsg;
   ptURCBBroadCastMsg = NULL;

   return true;
}

bool URCBController::sendCallAnswerCommand(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid:%s !", ptCtx->m_strCallId.c_str());

   xGateSIPMsg* sipMsg = new xGateSIPMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER,
	 IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,
	 xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_ANSWER_CALL,
 	 xGateSIPMsg::SIP_MSG_TYPE::SIP_MSG_COMMAND);

   if (!sipMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendCallAnswerCommand failed to create xGateSIPMsg memory !");
      return false;
   }
 
   sipMsg->setFeatureType(XGATECALLFEATURE::EN_URCBCONF_CALL);
   sipMsg->setSdpInfo(ptCtx->m_strSDPInfo.c_str());
   sipMsg->setRouteAddr(ptCtx->m_tUserAor.m_ipAddr.c_str());   
   sipMsg->setDstModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
   sipMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);
   sipMsg->setUid(ptCtx->m_strCallId.c_str());
   this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(sipMsg));
   return true;
}

bool URCBController::sendConfUserSDPUpdateResponse(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid:%s !", ptCtx->m_strCallId.c_str());

   std::string strConfRoomId = ptCtx->m_strConfRoomId.c_str();
   std::string strConfConnId = ptCtx->m_strConfConnId.c_str();
   //Send Updated SDP Response to Socket IO server
   URCBSIOUserInfo * ptSioUserInfo = NULL;
   if(!m_tURConfRoomCtxHndlr.get_conf_socket_info_from_connid(strConfRoomId.c_str(), strConfConnId.c_str(),ptSioUserInfo)) 
   {
      XGLOG_ERROR("handleCallConnectedNotify get_conf_socket_info_from_connid connid (%s) not found in confRoom:%s",\
                     strConfConnId.c_str(),strConfRoomId.c_str());
      return false;
   }
   ptSioUserInfo->m_tURConfEvent.m_strEventName = "sdp_update_response";
   ptSioUserInfo->m_tURConfUserMediaInfo.m_strSdp = ptCtx->m_strSDPInfo.c_str();
   post_msg_to_socketio(ptSioUserInfo);
   return true;
}

bool URCBController::sendConfUserTileChangeResponse(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid:%s !", ptCtx->m_strCallId.c_str());

   std::string strConfRoomId = ptCtx->m_strConfRoomId.c_str();
   std::string strConfConnId = ptCtx->m_strConfConnId.c_str();
   //Send Updated SDP Response to Socket IO server
   URCBSIOUserInfo * ptSioUserInfo = NULL;
   if(!m_tURConfRoomCtxHndlr.get_conf_socket_info_from_connid(strConfRoomId.c_str(), strConfConnId.c_str(),ptSioUserInfo)) 
   {
      XGLOG_ERROR("sendConfUserTileChangeResponse get_conf_socket_info_from_connid connid (%s) not found in confRoom:%s",\
                     strConfConnId.c_str(),strConfRoomId.c_str());
      return false;
   }
   ptSioUserInfo->m_tURConfEvent.m_strEventName = "tile_change_response";
   ptSioUserInfo->m_tURConfUserMediaInfo.m_strSdp = ptCtx->m_strSDPInfo.c_str();
   post_msg_to_socketio(ptSioUserInfo);
   return true;
}

bool URCBController::sendErrorNotifyCommand(URCBCallContext *ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid:%s !", ptCtx->m_strCallId.c_str());

   xGateSIPMsg* sipMsg = new xGateSIPMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER,
                              IURDefines::UR_MODULE_SIPSIGNAL_SERVICE,
                              xGateSIPMsg::SIP_EVENT_TYPE::SIP_EVENT_ERROR_RESPONSE,
                              xGateSIPMsg::SIP_MSG_TYPE::SIP_MSG_COMMAND);
   if (!sipMsg) 
   {
      XGLOG_ERROR("OOM Issue. sendErrorNotifyCommand failed to create xGateSIPMsg memory !");
      return false;
   }
   sipMsg->setFeatureType(XGATECALLFEATURE::EN_URCBCONF_CALL);
   sipMsg->setDstModuleId(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE);
   switch(ptCtx->getStatus())
   {
      case URCBCallContext::URCBCALL_STATUS_REQUEST_TIMEOUT:
      {
         sipMsg->setSipErrorEvent(xGateSIPMsg::SIP_ERROR_EVENT_TYPE::SIP_ERROR_EVENT_SERVER_TIMEOUT);
         break;
      }
      case URCBCallContext::URCBCALL_STATUS_VALIDATION_FAILED:
      {
         sipMsg->setSipErrorEvent(xGateSIPMsg::SIP_ERROR_EVENT_TYPE::SIP_ERROR_EVENT_VALIDATION_FAILURE);
         break;
      }
      default:
      {
         break;
      }
   }
   sipMsg->setMsgType(EN_XGATE_MSG_SIP_MODULE);
   sipMsg->setUid(ptCtx->m_strCallId.c_str());
   this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(sipMsg));

   return true;
}

bool URCBController::handleMBCLinkModuleMsg(xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = dynamic_cast<xGateMBCLinkServiceMsg *>(pMsg); 
   switch (pMBCLinkServiceMsg->getOpCode())
   {
      case EN_MBC_LINK_EVENT_CONF_PORT_ALLOCATION_RES:
      {
         XGLOG_INFO("URCBController handling 'EN_MBC_LINK_EVENT_CONF_PORT_ALLOCATION_RES' event from MBC Link Service");
         handleConfPortAllocationResponse(pMsg);
         break;
      }
      case EN_MBC_LINK_EVENT_CONF_USER_SDP_UPDATE_RES:
      {
         XGLOG_INFO("URCBController handling 'EN_MBC_LINK_EVENT_CONF_USER_SDP_UPDATE_RES' event from MBC Link Service");
         handleConfUserSDPUpdateResponse(pMsg);
         break;
      }
      case EN_MBC_LINK_EVENT_CONF_USER_TILE_CHANGE_RES:
      {
         XGLOG_INFO("URCBController handling 'EN_MBC_LINK_EVENT_CONF_USER_TILE_CHANGE_RES' event from MBC Link Service");
         handleConfUserTileChangeResponse(pMsg);
         break;
      }
      case EN_MBC_LINK_EVENT_ACTIVE_SPEAKER_INFO:
      {
         XGLOG_INFO("URCBController handling 'EN_MBC_LINK_EVENT_ACTIVE_SPEAKER_INFO' event from MBC Link Service");
         handleActiveSpeakerInfoNotify(pMsg);
         break;
      }
      case EN_MBC_LINK_EVENT_SFU_CONN_CLOSED:
      {
         XGLOG_INFO("URCBController handling 'EN_MBC_LINK_EVENT_SFU_CONN_CLOSED' event from MBC Link Service");
         handleSfuConnClosedNotify(pMsg);
         break;
      }
      case EN_MBC_LINK_EVENT_DTLS_CONN_FAILED:
      {
         XGLOG_INFO("URCBController handling 'EN_MBC_LINK_EVENT_DTLS_CONN_FAILED' event from MBC Link Service");
         handleDtlsConnFailedNotify(pMsg);
         break;
      }
      case EN_MBC_LINK_EVENT_CLIENT_CONNECTED:
      {
         XGLOG_INFO("URCBController handling 'EN_MBC_LINK_EVENT_CLIENT_CONNECTED' event from MBC Link Service");
         handleMbcConnnectedMsg(pMsg);
         break;
      }
      case EN_MBC_LINK_EVENT_CLIENT_CONNECTION_CLOSED:
      {
         XGLOG_INFO("URCBController handling 'EN_MBC_LINK_EVENT_CLIENT_CONNECTION_CLOSED' event from MBC Link Service");
         handleMbcConClosedMsg(pMsg);
         break;
      }
      case EN_MBC_LINK_EVENT_UNKNOWN:
      default:
      {
         XGLOG_ERROR("URCBController failed to handle MBC Link module message: %d !", pMBCLinkServiceMsg->getOpCode());
         break;
      }
   }

   return true;
}

bool URCBController::handleConfPortAllocationResponse(xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = dynamic_cast<xGateMBCLinkServiceMsg *>(pMsg);
   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   AudioDetail &audioDetail = mediaDetail.audioDetail;
   VideoDetail &videoDetail = mediaDetail.videoDetail;
  
   std::string strCallId = mediaDetail.call_id;
   URCBCallContext * ptCtx = NULL;
   if(!m_tURCBCallCtxHndlr.getURCBCallContext(strCallId,ptCtx)) 
   {
      XGLOG_ERROR("handleConfPortAllocationResponse failed.Call context not found for callid:%s",\
		            strCallId.c_str());
      return false;
   }
   ptCtx->m_strSDPInfo = mediaDetail.sdp_str.c_str();
   ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_CALL_ANSWER_ALLOW);  
   if(!process_msg(ptCtx,pMsg)) 
   {
      XGLOG_ERROR("handleConfPortAllocationResponse failed while processing URCBCALL_STATUS_CALL_ANSWER_ALLOW");
      return false;
   }
   
   return true;
}

bool URCBController::handleConfUserSDPUpdateResponse(xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = dynamic_cast<xGateMBCLinkServiceMsg *>(pMsg);
   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   AudioDetail &audioDetail = mediaDetail.audioDetail;
   VideoDetail &videoDetail = mediaDetail.videoDetail;

   std::string strCallId = mediaDetail.call_id;
   URCBCallContext * ptCtx = NULL;
   if(!m_tURCBCallCtxHndlr.getURCBCallContext(strCallId,ptCtx)) 
   {
      XGLOG_ERROR("handleConfUserSDPUpdateResponse failed.Call context not found for callid:%s",\
                strCallId.c_str());
      return false;
   }
   ptCtx->m_strSDPInfo = mediaDetail.sdp_str.c_str();
   ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_SDP_UPDATE_RES);
   if(!process_msg(ptCtx)) 
   {
      XGLOG_ERROR("handleConfUserSDPUpdateResponse failed while processing URCBCALL_STATUS_SDP_UPDATE_RES");
      return false;
   }

   return true;
}

bool URCBController::handleConfUserTileChangeResponse(xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = dynamic_cast<xGateMBCLinkServiceMsg *>(pMsg);
   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   AudioDetail &audioDetail = mediaDetail.audioDetail;
   VideoDetail &videoDetail = mediaDetail.videoDetail;

   std::string strCallId = mediaDetail.call_id;
   URCBCallContext * ptCtx = NULL;
   if(!m_tURCBCallCtxHndlr.getURCBCallContext(strCallId,ptCtx)) 
   {
      XGLOG_ERROR("handleConfUserTileChangeResponse failed.Call context not found for callid:%s",\
                strCallId.c_str());
      return false;
   }
   ptCtx->m_strSDPInfo = mediaDetail.sdp_str.c_str();
   ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_TILE_CHANGE_RES);
   if(!process_msg(ptCtx)) 
   {
      XGLOG_ERROR("handleConfUserTileChangeResponse failed while processing URCBCALL_STATUS_TILE_CHANGE_RES");
      return false;
   }

   return true;
}

bool URCBController::handleActiveSpeakerInfoNotify(xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = dynamic_cast<xGateMBCLinkServiceMsg *>(pMsg);
   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   AudioDetail &audioDetail = mediaDetail.audioDetail;
   VideoDetail &videoDetail = mediaDetail.videoDetail;

   std::string strCallId = mediaDetail.call_id;
   std::string strPrevActSpkrCallId = mediaDetail.prev_act_spkr;
   URCBCallContext * ptCtx = NULL;
   if(!m_tURCBCallCtxHndlr.getURCBCallContext(strCallId,ptCtx)) 
   {
      XGLOG_ERROR("handleActiveSpeakerInfoNotify failed.Call context not found for callid:%s",\
                strCallId.c_str());
      return false;
   }

   std::string strPrevActSpkrConnId("");
   URCBCallContext * ptPrevSpkrCtx = NULL;
   if(!m_tURCBCallCtxHndlr.getURCBCallContext(strPrevActSpkrCallId,ptPrevSpkrCtx)) {
      XGLOG_ERROR("handleActiveSpeakerInfoNotify failed.Call context not found for prev_act_spkr callid:%s",\
                strPrevActSpkrCallId.c_str());
      //return false;
   } else {
	 		strPrevActSpkrConnId = ptPrevSpkrCtx->m_strConfConnId.c_str();
	 }

	 std::string strConfRoomId = ptCtx->m_strConfRoomId.c_str();
	 std::string strConnId = ptCtx->m_strConfConnId.c_str();

   //update active speaker info in confroom context
   URConfRoom * urConfRoom = NULL;
	 if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
	 {
		 XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_INITIAL_SPEAKER_INFO getConfRoomCallContext conf room id (%s) not found",\
				 strConfRoomId.c_str());
		 return false;
	 }
   urConfRoom->m_URActiveSpeakerInfo.m_strConnId = strConnId.c_str();
   urConfRoom->m_URActiveSpeakerInfo.m_strCallId = strCallId.c_str();
   urConfRoom->m_URActiveSpeakerInfo.m_strPrevSpkrConnId = strPrevActSpkrConnId.c_str();
   urConfRoom->m_URActiveSpeakerInfo.m_strPrevSpkrCallId = strPrevActSpkrCallId.c_str();

	 URCBSIOUserInfo * ptSioUserInfo = NULL;
	 if(!m_tURConfRoomCtxHndlr.get_conf_socket_info_from_connid(strConfRoomId.c_str(), strConnId.c_str(),ptSioUserInfo)) 
	 {
		 XGLOG_ERROR("handleActiveSpeakerInfoNotify get_conf_socket_info_from_connid not found connid (%s) in confRoom:(%s)",\
				 strConnId.c_str(),strConfRoomId.c_str());
		 return false;
	 }
   
	 //Updating previous active speaker info
	 CONF_USER_INFO_MAP & t_URConfUserInfoList = ptSioUserInfo->m_tURConfUserInfoList;
	 ITR_CONF_USER_INFO_MAP userItr = t_URConfUserInfoList.find(strConnId.c_str());  
	 if(userItr != t_URConfUserInfoList.end())
	 {
		 userItr->second.m_strPrevSpkrConnId = strPrevActSpkrConnId.c_str();
	 }

	 ptSioUserInfo->m_tURConfEvent.m_strEventName = "current_active_speaker_info";
	 post_msg_to_socketio(ptSioUserInfo);

   return true;
}

bool URCBController::handleSfuConnClosedNotify(xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = dynamic_cast<xGateMBCLinkServiceMsg *>(pMsg);
   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   AudioDetail &audioDetail = mediaDetail.audioDetail;
   VideoDetail &videoDetail = mediaDetail.videoDetail;
   std::string strConfRoomId = mediaDetail.mgresource_id;

   URConfRoom * urConfRoom = NULL;
   if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom)) 
   {
      XGLOG_ERROR("handleSfuConnClosedNotify getConfRoomCallContext conf room id (%s) not found",\
                     strConfRoomId.c_str());
      return false;
   }

   std::map<std::string,std::string> tMapConnIdList;
   m_tURConfRoomCtxHndlr.get_connid_list_from_confroom(strConfRoomId,tMapConnIdList);
   std::map<std::string,std::string>:: iterator connIdItr = tMapConnIdList.begin();
   for(connIdItr; connIdItr != tMapConnIdList.end(); connIdItr++)
   {
      std::string strConfConnId = connIdItr->first;
      std::string strConfCallId = connIdItr->second;
      URCBSIOUserInfo * ptSioUserInfo = NULL;
      if(!m_tURConfRoomCtxHndlr.get_conf_socket_info_from_connid(strConfRoomId.c_str(), strConfConnId.c_str(),ptSioUserInfo)) 
      {
         XGLOG_ERROR("handleSfuConnClosedNotify connid (%s) not found in confRoom:%s",\
                     strConfConnId.c_str(),strConfRoomId.c_str());
      }
      ptSioUserInfo->m_tURConfEvent.m_strEventName = "user_joining_status_error";
      post_msg_to_socketio(ptSioUserInfo);
   }

   return true;
}

bool URCBController::handleDtlsConnFailedNotify(xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = dynamic_cast<xGateMBCLinkServiceMsg *>(pMsg);
   MgMediaDetail &mediaDetail = pMBCLinkServiceMsg->get_media_detail();
   AudioDetail &audioDetail = mediaDetail.audioDetail;
   VideoDetail &videoDetail = mediaDetail.videoDetail;
   std::string strConfRoomId = mediaDetail.mgresource_id;
   std::string strCallId = mediaDetail.call_id.c_str();

   URCBCallContext * ptCtx = NULL;
   if (!m_tURCBCallCtxHndlr.getURCBCallContext(strCallId, ptCtx)) 
   {
      XGLOG_ERROR("handleDtlsConnFailedNotify get call context failed for callid (%s) with confroomid:(%s)",\
            		   strCallId.c_str(), strConfRoomId.c_str());
      return false;
   }

   std::string strConfConnId = ptCtx->m_strConfConnId.c_str(); 
   URCBSIOUserInfo * ptSioUserInfo = NULL;
   if(!m_tURConfRoomCtxHndlr.get_conf_socket_info_from_connid(strConfRoomId.c_str(), strConfConnId.c_str(),ptSioUserInfo)) 
   {
      XGLOG_ERROR("handleDtlsConnFailedNotify get_conf_socket_info_from_connid not found connid (%s) in confRoom:(%s)",\
            strConfConnId.c_str(),strConfRoomId.c_str());
   }
   if(!ptSioUserInfo)
   {
      XGLOG_NOTICE("handleDtlsConnFailedNotify creating new socket-io message for connid:(%s) and callid:(%s)",strConfConnId.c_str(),strCallId.c_str());
      ptSioUserInfo = new URCBSIOUserInfo();
      ptSioUserInfo->m_strConfCallId = strCallId.c_str();
      ptSioUserInfo->m_tURConfMeetInfo.m_strMeetingId = strConfRoomId.c_str();
      URCONF_USER_INFO ConfUserInfo;
      ConfUserInfo.m_strConnId = strConfConnId.c_str();
      if(ptCtx->m_enumConfUserType == CONF_USER_TYPE_HOST) 
      {
         ConfUserInfo.m_strUserType = "host";
      } 
      else 
      {
         ConfUserInfo.m_strUserType = "participant";
      }
      ptSioUserInfo->m_tURConfUserInfoList.insert({ConfUserInfo.m_strConnId,ConfUserInfo});
   }
   ptSioUserInfo->m_tURConfEvent.m_strEventName = "dtls_connection_failure_error";
   post_msg_to_socketio(ptSioUserInfo);

   return true;
}

// Functions to find the active speaker by checking 4 events in the duration of 2 seconds
bool URCBController::isSpeaking(const std::chrono::steady_clock::time_point& currentTime, UserData& userData) {
  while (!userData.volumeUpdates.empty() && (currentTime - userData.volumeUpdates.front().timestamp) > std::chrono::milliseconds(2100)) {
    userData.volumeUpdates.pop_front();
  }
  return userData.volumeUpdates.size() >= 4;
}

void URCBController::updateUserVolume(const std::string& userId, double volumeLevel, URConfRoom* urConfRoom) {
  auto currentTime = std::chrono::steady_clock::now();
  auto& userData = urConfRoom->userDataMap[userId];

  userData.volumeLevel = volumeLevel;
  userData.lastActiveTime = currentTime;
  userData.volumeUpdates.push_back({volumeLevel, currentTime});
}

//Function to calculate the average volume based on the no. of active speaker events received
double URCBController::calculateAverageVolume(UserData& userData) {
 if (userData.volumeUpdates.empty()) return -100;

 double sum = 0;
 for (const auto& update : userData.volumeUpdates) {
	 sum += update.volumeLevel;
 }
 return sum / userData.volumeUpdates.size();
}

//Returns the current active speaker if the client speaking continuously for 2 seconds and having more average volume than the other participants.
std::string URCBController::findActiveSpeaker(URConfRoom* urConfRoom) {
  std::string activeSpeaker = "";
  double maxAverageVolume = -100;
  auto currentTime = std::chrono::steady_clock::now();

#if 0
	for (const auto& entry : urConfRoom->userDataMap) {
	  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - entry.second.lastActiveTime).count();
	  std::cout << "Key: " << entry.first << ", Value: {" << entry.second.volumeLevel << ", "<< duration << "ms, Updates: " << entry.second.volumeUpdates.size() << "}" << std::endl;
  }
#endif

  for (auto& entry : urConfRoom->userDataMap) {
    double averageVolume = calculateAverageVolume(entry.second);
    if (averageVolume > -50 && isSpeaking(currentTime, entry.second)) {
      if (averageVolume > maxAverageVolume) {
        XGLOG_INFO("-----Average volume is crossing threshold for connID: %s , Average Volume : %lf",entry.first.c_str(), averageVolume);
        activeSpeaker = entry.first;
        maxAverageVolume = averageVolume;
      }
    }
  }

  XGLOG_INFO("ActiveSpeaker in function----------------------------:%s  size : %d", activeSpeaker.c_str(), urConfRoom->userDataMap.size());
  return activeSpeaker;
}

void URCBController::activeSpeakerOnDisconnect(const std::string& strConfRoomId, URConfRoom* urConfRoom, URCBSIOUserInfo* ptSioUserInfo) {
  XGLOG_INFO("URCBController::activeSpeakerOnDisconnect Calling...");
  auto sendActiveSpeakerInfo = [&](const std::string& activeSpeakerConnId) {
    auto itr = urConfRoom->m_URCBSIOUserInfoList.find(activeSpeakerConnId);
    if (itr != urConfRoom->m_URCBSIOUserInfoList.end()) {
		  urConfRoom->m_URActiveSpeakerInfo.m_strConnId = activeSpeakerConnId.c_str();
			urConfRoom->m_URActiveSpeakerInfo.m_strCallId = itr->second->m_strConfCallId.c_str();
      ptSioUserInfo->m_tURConfUserInfoList = itr->second->m_tURConfUserInfoList;
      ptSioUserInfo->m_tURConfEvent.m_strEventName = "current_active_speaker_info";
      sendConfRoomActiveSpeakerInfo(strConfRoomId);
      post_msg_to_socketio(ptSioUserInfo);
    }
  };

  auto userInfoListItr = urConfRoom->m_URCBSIOUserInfoList.rbegin();
  if (userInfoListItr != urConfRoom->m_URCBSIOUserInfoList.rend()) {
	  std::string lastUserConnId = userInfoListItr->first;
    sendActiveSpeakerInfo(lastUserConnId);
    ++userInfoListItr;
    if (userInfoListItr != urConfRoom->m_URCBSIOUserInfoList.rend()) {
      std::string secondLastUserConnId = userInfoListItr->first;
      sendActiveSpeakerInfo(secondLastUserConnId);
    }
  }
}

bool URCBController::handleSchedulerUpdateMsg(xGateBaseMsg *pBaseMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateSchedulerServiceMsg *pMsg = dynamic_cast<xGateSchedulerServiceMsg*>(pBaseMsg);
   if(!pMsg) 
   {
      XGLOG_ERROR("handleSchedulerUpdateMsg failed. Invalid SchedulerServiceMsg pointer !");
      return false;
   }
	 SchedulerInfo &schedulerInfo = pMsg->get_scheduler_info();
	 switch(schedulerInfo.m_schedulerType)
	 {
		 case EN_SCHEDULER_URCONF_MEETING_DURATION:
			 {
			   std::string strConfRoomId = pMsg->getUid();
			 	 long int durationLeft = schedulerInfo.m_timeoutSecond;
				 set_meeting_duration(strConfRoomId, durationLeft);
				 XGLOG_INFO("handleSchedulerUpdateMsg EN_SCHEDULER_URCONF_MEETING_DURATION meeting:%s && duration:%d", \
				 						strConfRoomId.c_str(),durationLeft);
				 break;
			 }
		 default:
			 {
					XGLOG_ERROR("handleSchedulerUpdateMsg UNKNOWN scheduler msg received");
			 }
	 }
	 return true;
}

bool URCBController::handle_TimerResponse(xGateBaseMsg *pBaseMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   xGateSchedulerMsg *pMsg = dynamic_cast<xGateSchedulerMsg*>(pBaseMsg);
   if(!pMsg) 
   {
      XGLOG_ERROR("handle_TimerResponse failed. Invalid SchedulerMsg pointer !");
      return false;
   }
   xGateTimerData *data = pMsg->getTimerData();
   if(!data)
   {
      XGLOG_ERROR("handle_TimerResponse failed. invalid xGateTimerData !");
      return false;
   }
   string uid = data->m_uid.c_str();
   unsigned int timerType = data->m_timerType;
   if(uid.empty()) 
   {
      XGLOG_ERROR("handle_TimerResponse failed. uid is empty!");
      return false;
   }
   URCBTimerData * urcbTimerData = (URCBTimerData*)data->m_pCtxdata;
   if(!urcbTimerData) 
   {
      XGLOG_ERROR("handle_TimerResponse failed. urcbTimerData is NULL!");
      return false;
   }
   URConfRoom * urConfRoom = NULL;
   std::string strConfRoomId = urcbTimerData->m_strConfMeetingId.c_str();   
   std::string strConfConnId = urcbTimerData->m_strConfConnId.c_str();   
   if(urcbTimerData && data)
   {
      delete urcbTimerData;
      urcbTimerData = NULL;
      delete data;
      data = NULL;
   }
   switch(timerType)
   {
      case EN_SCHEDULER_URCONF_PORT_ALLOC_RES:
      {
         XGLOG_DEBUG("==> EN_SCHEDULER_URCONF_PORT_ALLOC_RES timeout occurred for uid: %s", uid.c_str());
         //Send User Joined Info to Socket IO server
         URCBSIOUserInfo * ptSioUserInfo = NULL;
         if(!m_tURConfRoomCtxHndlr.get_conf_socket_info_from_connid(strConfRoomId.c_str(), strConfConnId.c_str(),ptSioUserInfo)) 
         {
            XGLOG_ERROR("handle_TimerResponse get_conf_socket_info_from_connid not found connid (%s) in confRoom:(%s)",\
                        strConfConnId.c_str(),strConfRoomId.c_str());
            return false;
         }
         URCBCallContext * ptCtx = NULL;
         std::string strCallId = ptSioUserInfo->m_strConfCallId.c_str();
         if (!m_tURCBCallCtxHndlr.getURCBCallContext(strCallId, ptCtx)) 
         {
            XGLOG_ERROR("EN_SCHEDULER_URCONF_PORT_ALLOC_RES get call context failed for callid (%s)",\
                        strCallId.c_str());
            return false;
         }
         if(ptCtx->m_enumConfUserType == CONF_USER_TYPE_SCREENSHARE) 
         {
            ptSioUserInfo->m_tURConfEvent.m_strEventName = "screenshare_session_joining_error";
         } 
         else 
         {
            ptSioUserInfo->m_tURConfEvent.m_strEventName = "user_joining_status_error";
         }
         post_msg_to_socketio(ptSioUserInfo);
         //Respond with Sip Error Cause code
         ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_REQUEST_TIMEOUT);
         sendErrorNotifyCommand(ptCtx);
         m_tURConfRoomCtxHndlr.remove_connid_from_socket_user_info_list(strConfRoomId,strConfConnId);
         break;
      }
      case EN_SCHEDULER_URCONF_ACTIVE_SPEAKER_INFO:
      {
         XGLOG_DEBUG("==> EN_SCHEDULER_URCONF_ACTIVE_SPEAKER_INFO timeout occurred for uid: %s", uid.c_str());
         //Send Active speaker info
         sendConfRoomActiveSpeakerInfo(strConfRoomId);	   
         //Schedule Active Speaker Timer
         URCBTimerData * urcbTimerData = new URCBTimerData();
         urcbTimerData->m_strConfMeetingId = strConfRoomId.c_str();
         std::string uid = urcbTimerData->m_strConfConnId = strConfRoomId.c_str();
         set_timer(uid, urcbTimerData, EN_SCHEDULER_URCONF_ACTIVE_SPEAKER_INFO, URCONF_ACTIVE_SPEAKER_INFO_TIMER_VAL);
         break;
      }
      case EN_SCHEDULER_URCONF_MEETING_DURATION:
      {
				 XGLOG_DEBUG("==> EN_SCHEDULER_URCONF_MEETING_DURATION timeout occurred for uid: %s", uid.c_str());
				 URCBSIOUserInfo * ptSioUserInfo = new URCBSIOUserInfo();
				 std::string strTransId = generateTransid();	
				 if(!strTransId.empty())
				 {
						ptSioUserInfo->m_tURConfEvent.m_strEventSeq = strTransId.c_str();
				 }
				 ptSioUserInfo->m_tURConfEvent.m_strEventName = "meeting_expired";
				 ptSioUserInfo->m_tURConfMeetInfo.m_strMeetingId = uid.c_str();
				 post_msg_to_socketio(ptSioUserInfo);
				 break;
      }
      case EN_SCHEDULER_URCONF_ACD_RES:
      {
         XGLOG_DEBUG("==> EN_SCHEDULER_URCONF_ACD_RES timeout occurred for uid: %s", uid.c_str());
         //Retry Other ACD server
         URCBSIOUserInfo * ptSioUserInfo = NULL;
         if(!m_tURConfRoomCtxHndlr.get_conf_socket_info_from_connid(strConfRoomId.c_str(), strConfConnId.c_str(),ptSioUserInfo)) 
         {
            XGLOG_ERROR("handle_TimerResponse get_conf_socket_info_from_connid not found connid (%s) in confRoom:(%s)",\
                        strConfConnId.c_str(),strConfRoomId.c_str());
            return false;
         }
         URCBCallContext * ptCtx = NULL;
         std::string strCallId = ptSioUserInfo->m_strConfCallId.c_str();
         if (!m_tURCBCallCtxHndlr.getURCBCallContext(strCallId, ptCtx)) 
         {
            XGLOG_ERROR("EN_SCHEDULER_URCONF_ACD_RES get call context failed for callid (%s)",\
                        strCallId.c_str());
            return false;
         }

				 auto itr = std::find(ptCtx->m_vAcdChannelList.begin(), ptCtx->m_vAcdChannelList.end(), ptCtx->m_strChannel);
				 if(itr != ptCtx->m_vAcdChannelList.end())
				 {
						ptCtx->m_vAcdChannelList.erase(itr);
				 }

				 if(ptCtx->m_vAcdChannelList.size() > 0)
				 {
						ptCtx->m_strChannel = "";
						get_acd_channel_from_list(ptCtx);
						ptCtx->setCcaasEvent(EN_CCAAS_CB_ACD_CALL_INIT);
						post_ccaas_conf_info_to_acd(ptCtx);

						std::shuffle(std::begin(m_vAcdChannelList), std::end(m_vAcdChannelList), std::default_random_engine());

						//Schedule Timer for ACD Response
						URCBTimerData * urcbTimerData = new URCBTimerData();
						urcbTimerData->m_strConfMeetingId = ptCtx->m_strConfRoomId.c_str();
						std::string uid = urcbTimerData->m_strConfConnId = ptCtx->m_strConfConnId.c_str();
						set_timer(uid, urcbTimerData, EN_SCHEDULER_URCONF_ACD_RES, URCONF_ACD_RES_TIMER_VAL);
				 }
				 else
				 {
						XGLOG_ERROR("EN_SCHEDULER_URCONF_ACD_RES failed. Tried all the available acd server");
				 }
				 break;
      }
      default:
      {
         XGLOG_DEBUG("handle_TimerResponse Unknown timeout event triggered for uid: %s", uid.c_str());
         return false;
      }
   }

   return true;
}

bool URCBController::handleRedisGetResponse(xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   RedisServiceMsg *predisMsg = dynamic_cast<RedisServiceMsg*>(pMsg);
   if( predisMsg == NULL )
   {
      XGLOG_ERROR("handleRedisGetResponse recvd invalid Message class in Redis Handler");
      return false;
   }
	 RedisServiceEvent redisEvent = predisMsg->get_redis_info().m_enRedisEvent;
	 switch(redisEvent)
	 {
		 case EN_REDIS_EVENT_GET_ACD_CHANNEL:
			 {
				 std::string strCallId = predisMsg->get_redis_info().m_strContextId.c_str();
				 std::string strKey = predisMsg->get_redis_info().m_strKey.c_str();
				 std::string strValue = predisMsg->get_redis_info().m_strValue.c_str();

				 URCBCallContext * ptCtx = NULL;
				 if (!m_tURCBCallCtxHndlr.getURCBCallContext(strCallId, ptCtx))
				 {
					 XGLOG_ERROR("handleRedisGetResponse GET_ACD_CHANNEL get call context failed for callid (%s)", strCallId.c_str());
					 return false;
				 }
				 auto it = std::find(m_vAcdChannelList.begin(), m_vAcdChannelList.end(), strValue);
				 if(it != m_vAcdChannelList.end())
				 {
					 ptCtx->m_strChannel = strValue.c_str();
				 }
				 else
				 {
					 ptCtx->m_vAcdChannelList = m_vAcdChannelList;
					 get_acd_channel_from_list(ptCtx);
					 // shuffle the vector for round robin
					 std::shuffle(std::begin(m_vAcdChannelList), std::end(m_vAcdChannelList), std::default_random_engine());
					 XGLOG_INFO("handleRedisGetResponse fetching acd channel:%s from config for callid:%s", 
							 ptCtx->m_strChannel.c_str(), strCallId.c_str());
				 }
				 if(ptCtx->m_enumConfUserType == CONF_USER_TYPE_CUSTOMER) 
				 {
					 ptCtx->setCcaasEvent(EN_CCAAS_CB_ACD_CALL_INIT);
					 post_ccaas_conf_info_to_acd(ptCtx);
					 //Schedule Timer for ACD Response
					 URCBTimerData * urcbTimerData = new URCBTimerData();
					 urcbTimerData->m_strConfMeetingId = ptCtx->m_strConfRoomId.c_str();
					 std::string uid = urcbTimerData->m_strConfConnId = ptCtx->m_strConfConnId.c_str();
					 set_timer(uid, urcbTimerData, EN_SCHEDULER_URCONF_ACD_RES, URCONF_ACD_RES_TIMER_VAL);
				 }
			 }
			 break;
		 case EN_REDIS_EVENT_GET_MEETING_DURATION:
			 {
					std::string strConfRoomId = predisMsg->get_redis_info().m_strContextId.c_str();
					std::string strKey = predisMsg->get_redis_info().m_strKey.c_str();
					std::string strValue = predisMsg->get_redis_info().m_strValue.c_str();
					if(!strValue.empty())
					{
						 long int duration = stoi(strValue);
						 URConfRoom * urConfRoom = NULL;
						 if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
						 {
								XGLOG_ERROR("handleRedisGetResponse GET_MEETING_DURATION getConfRoomCallContext conf room id (%s) not found",\
											strConfRoomId.c_str());
								return false;
						 }
						 if(duration && !urConfRoom->isMeetingTimerSet)
						 {
								urConfRoom->isMeetingTimerSet = true;			

								URCBTimerData * urcbTimerData = new URCBTimerData();
								urcbTimerData->m_strConfMeetingId = strConfRoomId.c_str();
								std::string uid = urcbTimerData->m_strConfConnId = strConfRoomId.c_str();
								set_timer(uid, urcbTimerData, EN_SCHEDULER_URCONF_MEETING_DURATION, duration);

								XGLOG_INFO("GET_MEETING_DURATION set duration:%d for meeting_id:%s", duration, strConfRoomId.c_str());
						 } 
						 else
						 {
								XGLOG_ERROR("handleRedisGetResponse duration expired for meeting_id:%s", strConfRoomId.c_str());
						 }
					}
					else
					{
						 XGLOG_ERROR("handleRedisGetResponse GET_MEETING_DURATION key is not inserted/duration is NULL for meeting:%s",strConfRoomId.c_str());
					}
			 }
			 break;
		 default:
			 {
				 break;
			 }
	 }
   return true;
}

bool URCBController::get_acd_channel_from_list(URCBCallContext *& ptCtx)
{
   auto itr = ptCtx->m_vAcdChannelList.begin();
   if(itr != ptCtx->m_vAcdChannelList.end())
   {
      ptCtx->m_strChannel = (*itr);
   }
   return true;
}

bool URCBController::handleRedisModuleMsg(xGateBaseMsg *pMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   RedisServiceMsg *predisMsg = dynamic_cast<RedisServiceMsg*>(pMsg);
   if( predisMsg == NULL ) 
   {
      XGLOG_ERROR("handleRedisModuleMsg recvd invalid Message class in MBC Handler");
      return false;
   }
   std::string strMbcMsg = predisMsg->get_redis_info().m_strMsg.c_str();

   Document doc;
   ParseResult ok = doc.Parse(strMbcMsg.c_str());
   if(!ok) 
   {
      XGLOG_ERROR("handleRedisModuleMsg parseJsonMsg failed. Encountered with json parse error !");
      return false;
   } 
   else if(!doc.IsObject()) 
   {
      XGLOG_ERROR("handleRedisModuleMsg parseJsonMsg failed. Not able create documnet object !");
      return false;
   }

   std::string strChannel = predisMsg->get_redis_info().m_strChannel.c_str();
   if(strstr(strChannel.c_str(), "socketiorequest"))
   {
      URCBSIOUserInfo *ptSioUserInfo = new URCBSIOUserInfo(); 
      if(!decode_sio_json_msg_from_MBC(strMbcMsg.c_str(),ptSioUserInfo)) 
      {
	 XGLOG_ERROR("handleRedisModuleMsg decode_sio_json_msg_from_MBC failed !");
	 return false;
      } 
      //if(ptSioUserInfo && (strcmp(ptSioUserInfo->m_tURConfEvent.m_strEventName.c_str(), "active_speaker_info") != 0))
      {
	 XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	 XGLOG_INFO(" ==> UCAAS SOCKET TO CONFBRIDGE : %s", strMbcMsg.c_str());
	 XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      }
      handle_sio_mbc_message(ptSioUserInfo);
   }
   if(strstr(strChannel.c_str(), "CCAAS_SIO"))
   {
      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      XGLOG_INFO(" ==> CCAAS SOCKET TO CONFBRIDGE : %s", strMbcMsg.c_str());
      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");

      URCBSIOUserInfo *ptSioUserInfo = new URCBSIOUserInfo(); 
      if(!decode_sio_json_msg_from_MBC(strMbcMsg.c_str(),ptSioUserInfo)) 
      {
	 XGLOG_ERROR("handleRedisModuleMsg decode_sio_json_msg_from_MBC failed !");
	 return false;
      }
      ptSioUserInfo->m_isCcaas = true;
      handle_sio_mbc_message(ptSioUserInfo);
   }
   else if(strstr(strChannel.c_str(), "CONF_BRIDGE"))
   { 
      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      XGLOG_INFO("==> CONFBRIDGE MEET INFO : %s", strMbcMsg.c_str());
      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      //To handle conf room info from other bridge servers
      URCBBroadCastMsg * ptURCBBroadCastMsg = new URCBBroadCastMsg();
      if(!decode_conf_bridge_bcast_msg_from_MBC(strMbcMsg.c_str(),ptURCBBroadCastMsg))
      {
	 XGLOG_ERROR("handle_MBCResponse decode_conf_bridge_bcast_msg_from_MBC failed !");
	 return false;
      }
      handle_conf_bridge_broadcast_msg(ptURCBBroadCastMsg);
   }
   else if(strstr(strChannel.c_str(), "CB_ACD"))
   { 
      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      XGLOG_INFO("==> ACD TO CONFBRIDGE : %s", strMbcMsg.c_str());
      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      //To handle process ccaas call event from acd
      URCcaasBroadcastMsg * ptURCcaasBcastMsg = new URCcaasBroadcastMsg();
      if(!decode_ccaas_info_json_message(strMbcMsg.c_str(), ptURCcaasBcastMsg))
      {
	 XGLOG_ERROR("handle_MBCResponse decode_ccaas_info_json_message failed !");
	 return false;
      }
      handle_ccaas_acd_broadcast_msg(ptURCcaasBcastMsg);
   }
   else if(strstr(strChannel.c_str(), "CCAAS_PBX"))
   { 
      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      XGLOG_INFO("==> PBX TO CONFBRIDGE : %s", strMbcMsg.c_str());
      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
   }
   return true; 
}

bool URCBController::handle_ccaas_acd_broadcast_msg(URCcaasBroadcastMsg * ptURCcaasBcastMsg)
{
   XGLOG_FUNCTION_EN_EX("");
   if(!ptURCcaasBcastMsg)
   {
      XGLOG_ERROR("handle_ccaas_acd_broadcast_msg failed. Pointer is NULL !");
      return false;
   }
   std::string strConfRoomId = ptURCcaasBcastMsg->m_strUuid.c_str();
   CBACDMsg event = ptURCcaasBcastMsg->m_enCcaasEvent;
   switch(event)
   {
      case EN_CCAAS_CB_ACD_CALL_INIT_SUCCESS:
      {
			XGLOG_INFO("handle_ccaas_acd_broadcast_msg processing CALL_INIT_SUCCESS event for conf_room:%s", \
					ptURCcaasBcastMsg->m_strUuid.c_str());
			std::string strConnId(""), strCallId("");
			URCBCallContext * ptCtx = NULL;
			ConfUserType userType = CONF_USER_TYPE_CUSTOMER;
			if(m_tURConfRoomCtxHndlr.get_user_callid_from_conf_call_list(strConfRoomId, userType, strCallId))
			{
				if (!m_tURCBCallCtxHndlr.getURCBCallContext(strCallId, ptCtx)) 
				{
					XGLOG_ERROR("handle_ccaas_acd_broadcast_msg CALL_INIT_SUCCESS get URCBCallContext failed for callid (%s)",\
							strCallId.c_str());
					return false;
				}
				strConnId = ptCtx->m_strConfConnId.c_str();
			}
			cancel_timer(strConnId, EN_SCHEDULER_URCONF_ACD_RES);
			set_acd_channel(ptCtx);
			break;
      }
      case EN_CCAAS_CB_ACD_CALL_Q_ANNC:
      {
			XGLOG_INFO("handle_ccaas_acd_broadcast_msg processing CALL_Q_ANNC event for conf_room:%s", \
					ptURCcaasBcastMsg->m_strUuid.c_str());

			std::string strCallId("");
			std::string strConnId("");
			URCBCallContext * ptCtx = NULL;
			ConfUserType userType = CONF_USER_TYPE_CUSTOMER;
			if(m_tURConfRoomCtxHndlr.get_user_callid_from_conf_call_list(strConfRoomId, userType, strCallId))
			{
				if (!m_tURCBCallCtxHndlr.getURCBCallContext(strCallId, ptCtx)) 
				{
					XGLOG_ERROR("handle_ccaas_acd_broadcast_msg CALL_Q_ANNC get URCBCallContext failed for callid (%s)",\
							strCallId.c_str());
					return false;
				}
				strConnId = ptCtx->m_strConfConnId.c_str();

				URCBSIOUserInfo * ptSioUserInfo = NULL;
				if(!m_tURConfRoomCtxHndlr.get_conf_socket_info_from_connid(strConfRoomId.c_str(), strConnId.c_str(), ptSioUserInfo)) 
				{
					XGLOG_ERROR("get_conf_socket_info_from_connid connid (%s) not found in confRoom:%s",\
							strConnId.c_str(),strConfRoomId.c_str());
					return false;
				}
				std::string strTransId = generateTransid();	
				if(!strTransId.empty())
				{
					ptSioUserInfo->m_tURConfEvent.m_strEventSeq = strTransId.c_str();
				}
				ptSioUserInfo->m_tURConfEvent.m_strEventName = "queue_annc_text";
				ptSioUserInfo->m_tURConfUserMediaInfo.m_strSdp = "";
				ptSioUserInfo->m_tURConfCcaasInfo.m_strAnncText = ptURCcaasBcastMsg->m_strAnncText.c_str();
				post_msg_to_socketio(ptSioUserInfo);
			}
			break;
      }
      default:
      {
         break;
      }
   }
   return true;
}

bool URCBController::set_meeting_duration(std::string strConfRoomId, long int duration)
{
	 XGLOG_FUNCTION_EN_EX("");

	 std::string strKey("");
	 frame_meeting_duration_key(strConfRoomId, strKey);

	 RedisInfo redisInfo;
	 redisInfo.m_strContextId = strConfRoomId.c_str();
	 redisInfo.m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_SERVICE_CONTROLLER;
	 redisInfo.m_strKey = strKey.c_str();
	 redisInfo.m_strValue = to_string(duration);
	 redisInfo.m_enRedisEvent = EN_REDIS_EVENT_SET_MEETING_DURATION;
	 RedisServiceMsg *redisMsg = new RedisServiceMsg();
	 redisMsg->set_redis_info(redisInfo);
	 redisMsg->setMsgType(EN_XGATE_MSG_REDIS_SET);
	 redisMsg->setDstModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
	 redisMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
	 this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(redisMsg));

	 return true;
}

bool URCBController::set_acd_channel(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("");
	if(m_tURConfRoomCtxHndlr.set_acd_channel(ptCtx->m_strConfRoomId, ptCtx->m_strChannel))
	{
		XGLOG_INFO("CALL_INIT_SUCCESS set_acd_channel:%s for conf_room:%s success !", ptCtx->m_strChannel.c_str(),\
				ptCtx->m_strConfRoomId.c_str());
	}
	else
	{
		XGLOG_ERROR("CALL_INIT_SUCCESS set_acd_channel:%s for conf_room:%s failed !", ptCtx->m_strChannel.c_str(),\
				ptCtx->m_strConfRoomId.c_str());
	}

	RedisInfo redisInfo;
	redisInfo.m_strContextId = ptCtx->m_strCallId.c_str();
	redisInfo.m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_SERVICE_CONTROLLER;
	redisInfo.m_strKey = ptCtx->m_strAcdKey.c_str();
	redisInfo.m_strValue = ptCtx->m_strChannel.c_str();
   redisInfo.m_enRedisEvent = EN_REDIS_EVENT_SET_ACD_CHANNEL;
   RedisServiceMsg *redisMsg = new RedisServiceMsg();
	redisMsg->set_redis_info(redisInfo);
	redisMsg->setMsgType(EN_XGATE_MSG_REDIS_SET);
	redisMsg->setDstModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
	redisMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
	this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(redisMsg));

	return true;
}

bool URCBController::handle_conf_bridge_broadcast_msg(URCBBroadCastMsg * ptURCBBroadCastMsg)
{
   XGLOG_FUNCTION_EN_EX("");
   URCBBroadCastMsgType msgType = ptURCBBroadCastMsg->m_eBroadCastMsgType;
   std::string strConfRoomId = ptURCBBroadCastMsg->m_strMsgUuid;
   switch(msgType)
   {
      case URCONF_BROADCAST_MSG_ROOM_CREATED:
      {
         if(!m_tURConfRoomCtxHndlr.InsertRemoteConfRoomInfo(ptURCBBroadCastMsg))
         {
            XGLOG_ERROR("URCONF_BROADCAST_MSG_ROOM_CREATED InsertRemoteConfRoomInfo failed for confroomid:%s",\
                  strConfRoomId.c_str());
         }
         else
         {
            XGLOG_INFO("URCONF_BROADCAST_MSG_ROOM_CREATED InsertRemoteConfRoomInfo success for confroomid:%s",\
                  strConfRoomId.c_str());
         }
         break;
      }
      case URCONF_BROADCAST_MSG_ROOM_DELETED:
      {
         if(!m_tURConfRoomCtxHndlr.DeleteRemoteConfRoomInfo(ptURCBBroadCastMsg))
         {
            XGLOG_ERROR("URCONF_BROADCAST_MSG_ROOM_DELETED DeleteRemoteConfRoomInfo failed for confroomid:%s",\
                  strConfRoomId.c_str());
         }
         else
         {
            XGLOG_INFO("URCONF_BROADCAST_MSG_ROOM_DELETED DeleteRemoteConfRoomInfo success for confroomid:%s",\
                  strConfRoomId.c_str());
         }
         break;
      }
      default:
      {
         break;
      }
   }

   return true;
}

bool URCBController::isEventSequenceUnique(URCBSIOUserInfo * ptSioUserInfo)
{
   std::string strConfRoomId = ptSioUserInfo->m_tURConfMeetInfo.m_strMeetingId.c_str();
   std::string strConnId(""), strIncEvntSeq(""), strOldEvntSeq("");
   URConfRoom * urConfRoom = NULL;
   if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
   {
      XGLOG_ERROR("isEventSequenceUnique getConfRoomCallContext conf room id (%s) not found",\
                  strConfRoomId.c_str());
      return true;
   }
   CONF_USER_INFO_MAP tconfUserInfoMap = ptSioUserInfo->m_tURConfUserInfoList; 
   ITR_CONF_USER_INFO_MAP userItr = tconfUserInfoMap.begin();
   if (userItr != tconfUserInfoMap.end())
   {
      strConnId = userItr->first;  
      SIO_USER_INFO_MAP tSIOUserInfoMap = urConfRoom->m_URCBSIOUserInfoList;
      ITR_SIO_USER_INFO_MAP itr = tSIOUserInfoMap.find(strConnId);
      if(itr != tSIOUserInfoMap.end())
      {
         strIncEvntSeq = ptSioUserInfo->m_tURConfEvent.m_strEventSeq;
         strOldEvntSeq = itr->second->m_tURConfEvent.m_strEventSeq;
         if(!strIncEvntSeq.empty() && !strOldEvntSeq.empty() 
                     && (strcmp(strIncEvntSeq.c_str(), strOldEvntSeq.c_str()) == 0))
         {
            XGLOG_INFO("isEventSequenceUnique found inc_seq_num:%s and old_seq_num:%s as duplicate", \
                        strIncEvntSeq.c_str(), strOldEvntSeq.c_str());
            return false;
         }
      }
   }
	return true;
}

bool URCBController::handle_sio_mbc_message(URCBSIOUserInfo * ptSioUserInfo)
{
   XGLOG_FUNCTION_EN_EX("");
   std::string strConnId("");
   std::string strConfRoomId = ptSioUserInfo->m_tURConfMeetInfo.m_strMeetingId.c_str();
   std::string strEventName = ptSioUserInfo->m_tURConfEvent.m_strEventName.c_str();
   URConfSIOMsgType msgType;
   getConfSIOMsgType(strEventName,msgType);
   if(!isEventSequenceUnique(ptSioUserInfo)) 
   {
      XGLOG_ERROR("URCONF_SIO_MSG_HOST_JOINING ignoring duplicate event");
      return true;
   }
   switch(msgType) 
   {
      case URCONF_SIO_MSG_HOST_JOINING:
      {
         XGLOG_INFO("handle_sio_mbc_message processing URCONF_SIO_MSG_HOST_JOINING for conn_id:%s",strConnId.c_str());
         URConfRoom * urConfRoom = NULL;
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_HOST_JOINING creating conf room context for confroom_id (%s) ",\
                        strConfRoomId.c_str());
            urConfRoom = new URConfRoom();
            urConfRoom->m_strConfRoomId = strConfRoomId.c_str();
            m_tURConfRoomCtxHndlr.insertConfRoomCallContext(strConfRoomId,urConfRoom);
         }
         CONF_USER_INFO_MAP tconfUserInfoMap = ptSioUserInfo->m_tURConfUserInfoList; 
         ITR_CONF_USER_INFO_MAP userItr = tconfUserInfoMap.begin();
         for(userItr; userItr != tconfUserInfoMap.end(); userItr++)
         {
            strConnId = userItr->first;
            XGLOG_INFO("handle_sio_mbc_message URCONF_SIO_MSG_HOST_JOINING inserting conn_id(%s) in confRoom(%s)",\
                        strConnId.c_str(),strConfRoomId.c_str());
            urConfRoom->m_URCBSIOUserInfoList.insert(SIO_USER_INFO_MAP::value_type(strConnId,ptSioUserInfo));            
         } 
         break;
      }
      case URCONF_SIO_MSG_PARTICIPANT_JOINING:
      {
         XGLOG_INFO("handle_sio_mbc_message processing URCONF_SIO_MSG_PARTICIPANT_JOINING for conn_id:%s",strConnId.c_str());
         URConfRoom * urConfRoom = NULL;
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_PARTICIPANT_JOINING getConfRoomCallContext conf room id (%s) not found",\
            strConfRoomId.c_str());
            urConfRoom = new URConfRoom();
            urConfRoom->m_strConfRoomId = strConfRoomId.c_str();
            m_tURConfRoomCtxHndlr.insertConfRoomCallContext(strConfRoomId,urConfRoom);
         }
         CONF_USER_INFO_MAP tconfUserInfoMap = ptSioUserInfo->m_tURConfUserInfoList;
         ITR_CONF_USER_INFO_MAP userItr = tconfUserInfoMap.begin();
         for(userItr; userItr != tconfUserInfoMap.end(); userItr++)
         {
            strConnId = userItr->first;
            XGLOG_INFO("handle_sio_mbc_message URCONF_SIO_MSG_PARTICIPANT_JOINING inserting conn_id(%s) in confRoom(%s)",\
                        strConnId.c_str(),strConfRoomId.c_str());
            urConfRoom->m_URCBSIOUserInfoList.insert(SIO_USER_INFO_MAP::value_type(strConnId,ptSioUserInfo));
         }
         break;
      }
      case URCONF_SIO_MSG_CUSTOMER_JOINING:
      {
         XGLOG_INFO("handle_sio_mbc_message processing URCONF_SIO_MSG_CUSTOMER_JOINING for conn_id:%s",strConnId.c_str());
         URConfRoom * urConfRoom = NULL;
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_CUSTOMER_JOINING creating conf room context for confroom_id (%s) ",\
                        strConfRoomId.c_str());
            urConfRoom = new URConfRoom();
            urConfRoom->m_strConfRoomId = strConfRoomId.c_str();
            m_tURConfRoomCtxHndlr.insertConfRoomCallContext(strConfRoomId,urConfRoom);
         }
         CONF_USER_INFO_MAP tconfUserInfoMap = ptSioUserInfo->m_tURConfUserInfoList; 
         ITR_CONF_USER_INFO_MAP userItr = tconfUserInfoMap.begin();
         for(userItr; userItr != tconfUserInfoMap.end(); userItr++)
         {
            strConnId = userItr->first;
            XGLOG_INFO("handle_sio_mbc_message URCONF_SIO_MSG_CUSTOMER_JOINING inserting conn_id(%s) in confRoom(%s)",\
                        strConnId.c_str(),strConfRoomId.c_str());
            urConfRoom->m_URCBSIOUserInfoList.insert(SIO_USER_INFO_MAP::value_type(strConnId,ptSioUserInfo));            
         } 
         break;
      }
      case URCONF_SIO_MSG_AGENT_JOINING:
      {
         XGLOG_INFO("handle_sio_mbc_message processing URCONF_SIO_MSG_AGENT_JOINING for conn_id:%s",strConnId.c_str());
         URConfRoom * urConfRoom = NULL;
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_AGENT_JOINING creating conf room context for confroom_id (%s) ",\
                        strConfRoomId.c_str());
            urConfRoom = new URConfRoom();
            urConfRoom->m_strConfRoomId = strConfRoomId.c_str();
            m_tURConfRoomCtxHndlr.insertConfRoomCallContext(strConfRoomId,urConfRoom);
         }
         CONF_USER_INFO_MAP tconfUserInfoMap = ptSioUserInfo->m_tURConfUserInfoList; 
         ITR_CONF_USER_INFO_MAP userItr = tconfUserInfoMap.begin();
         for(userItr; userItr != tconfUserInfoMap.end(); userItr++)
         {
            strConnId = userItr->first;
            XGLOG_INFO("handle_sio_mbc_message URCONF_SIO_MSG_AGENT_JOINING inserting conn_id(%s) in confRoom(%s)",\
                        strConnId.c_str(),strConfRoomId.c_str());
            urConfRoom->m_URCBSIOUserInfoList.insert(SIO_USER_INFO_MAP::value_type(strConnId,ptSioUserInfo));            
         } 
         break;
      }
      case URCONF_SIO_MSG_SCREENSHARE_START:
      {
         XGLOG_INFO("handle_sio_mbc_message processing URCONF_SIO_MSG_SCREENSHARE_START for conn_id:%s",strConnId.c_str());
         URConfRoom * urConfRoom = NULL;
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom)) 
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_SCREENSHARE_START getConfRoomCallContext conf room id (%s) not found",\
                  strConfRoomId.c_str());
               return false;
         }
         CONF_USER_INFO_MAP tconfUserInfoMap = ptSioUserInfo->m_tURConfUserInfoList;
         ITR_CONF_USER_INFO_MAP userItr = tconfUserInfoMap.begin();
         for(userItr; userItr != tconfUserInfoMap.end(); userItr++)
         {
            strConnId = userItr->first;
            XGLOG_INFO("handle_sio_mbc_message URCONF_SIO_MSG_SCREENSHARE_START inserting conn_id(%s) in confRoom(%s)",\
                      strConnId.c_str(),strConfRoomId.c_str());
            urConfRoom->m_URCBSIOUserInfoList.insert(SIO_USER_INFO_MAP::value_type(strConnId,ptSioUserInfo));
         }
         break;
      }
      case URCONF_SIO_MSG_SCREENSHARE_STOP :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_SCREENSHARE_STOP");
         URCBCallContext * ptCtx = NULL;
         URConfRoom * urConfRoom = NULL;
         std::string strID("");
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom)) 
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_SCREENSHARE_STOP getConfRoomCallContext conf room id (%s) not found",\
                  strConfRoomId.c_str());
            return false;
         }
         ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         for(userItr; userItr != ptSioUserInfo->m_tURConfUserInfoList.end();userItr++)
         {
            strConnId = userItr->first;
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
            if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
            {
               strID = ptSioUserInfo->m_strConfCallId = itr->second->m_strConfCallId.c_str();
               itr->second = ptSioUserInfo;       
               if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx)) 
               {
                  XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_SCREENSHARE_STOP get URCBCallContext failed for callid (%s)",\
                              strID.c_str());
                  m_tURConfRoomCtxHndlr.remove_connid_from_socket_user_info_list(strConfRoomId,strConnId);
                  return false;
               }
               if(ptCtx)
               {
                  XGLOG_DEBUG("handle_sio_mbc_message URCONF_SIO_MSG_SCREENSHARE_STOP process Socket IO Stop Indication for callid (%s)",strID.c_str());
                  ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_CALL_DISCONNECTED);
                  if(!process_msg(ptCtx)) {
                     XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_SCREENSHARE_STOP failed for callid:%s",strID.c_str());
                     return false;
                  }
                  sendCallDisconnectRequest(ptCtx);//send bye to the socket io message user
                  handleCallClearing(ptCtx);
               }
            }
         }
         break;
      }
      case URCONF_SIO_MSG_RECORDING_START :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_START");
         URCBCallContext * ptCtx = NULL;
         URConfRoom * urConfRoom = NULL;
         std::string strID("");
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_START getConfRoomCallContext conf room id (%s) not found",\
                  strConfRoomId.c_str());
            return false;
         }
         ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         for(userItr; userItr != ptSioUserInfo->m_tURConfUserInfoList.end();userItr++)
         {
            URCONF_USER_INFO userInfo;
            userInfo = userItr->second;
            strConnId = userItr->first;       
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
            if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
            {
               strID = ptSioUserInfo->m_strConfCallId = itr->second->m_strConfCallId.c_str();
               itr->second = ptSioUserInfo;
               if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx)) 
               {
                  XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_START get URCBCallContext failed for callid (%s)",\
                        strID.c_str());
                  return false;
               }
               ptCtx->m_strRecFilePath = userInfo.m_strRecFilePath.c_str();
               ptCtx->m_strBrowserType = userInfo.m_strBrowserType.c_str();
            }
         }
         if(ptCtx)
         {
	    ptCtx->m_bIsRecStarted = true;
            ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_RECORDING_START);
            if(!process_msg(ptCtx)) 
            {
               XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_START process_msg failed for callid:%s",strID.c_str());
               return false;
            }
         }
         else
         {
         XGLOG_INFO("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_START call context not found for conn_id:%s" , strConnId.c_str());
         }
         break;
      }
      case URCONF_SIO_MSG_RECORDING_PAUSE :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_PAUSE");
         URCBCallContext * ptCtx = NULL;
         URConfRoom * urConfRoom = NULL;
         std::string strID("");
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_PAUSE getConfRoomCallContext conf room id (%s) not found",\
                  strConfRoomId.c_str());
            return false;
         }
         ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         for(userItr; userItr != ptSioUserInfo->m_tURConfUserInfoList.end();userItr++)
         {
            strConnId = userItr->first;            
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
            if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
            {
               strID = ptSioUserInfo->m_strConfCallId = itr->second->m_strConfCallId.c_str();
               itr->second = ptSioUserInfo;
               if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx)) 
               {
                  XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_PAUSE get URCBCallContext failed for callid (%s)",\
                        strID.c_str());
                  return false;
               }
            }
         }
         if(ptCtx)
         {
            ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_RECORDING_PAUSE);
            if(!process_msg(ptCtx)) 
            {
               XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_PAUSE process_msg failed for callid:%s",strID.c_str());
               return false;
            }
         }
         else
         {
            XGLOG_INFO("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_PAUSE call context not found for conn_id:%s" , strConnId.c_str());
         }
         break;
      }
      case URCONF_SIO_MSG_RECORDING_RESUME :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_RESUME");
         URCBCallContext * ptCtx = NULL;
         URConfRoom * urConfRoom = NULL;
         std::string strID("");
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_RESUME getConfRoomCallContext conf room id (%s) not found",\
                  strConfRoomId.c_str());
            return false;
         }
         ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         for(userItr; userItr != ptSioUserInfo->m_tURConfUserInfoList.end();userItr++)
         {
            strConnId = userItr->first;           
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
            if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
            {
               strID = ptSioUserInfo->m_strConfCallId = itr->second->m_strConfCallId.c_str();
               itr->second = ptSioUserInfo;
               if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx)) 
               {
                  XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_RESUME get URCBCallContext failed for callid (%s)",\
                        strID.c_str());
                  return false;
               }
            }
         }
         if(ptCtx)
         {
            ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_RECORDING_RESUME);
            if(!process_msg(ptCtx)) 
            {
               XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_RESUME process_msg failed for callid:%s",strID.c_str());
               return false;
            }
         }
         else
         {
            XGLOG_INFO("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_RESUME call context not found for conn_id:%s" , strConnId.c_str());
         }
         break;
      }
      case URCONF_SIO_MSG_RECORDING_STOP :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_STOP");
         URCBCallContext * ptCtx = NULL;
         URConfRoom * urConfRoom = NULL;
         std::string strID("");
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_STOP getConfRoomCallContext conf room id (%s) not found",\
                  strConfRoomId.c_str());
            return false;
         }
         ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         for(userItr; userItr != ptSioUserInfo->m_tURConfUserInfoList.end();userItr++)
         {
            URCONF_USER_INFO userInfo;
            userInfo = userItr->second;
            strConnId = userItr->first;          
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
            if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
            {
               strID = ptSioUserInfo->m_strConfCallId = itr->second->m_strConfCallId.c_str();
               itr->second = ptSioUserInfo;
               if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx)) 
               {
                  XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_RECORDING_STOP get URCBCallContext failed for callid (%s)",\
                              strID.c_str());
                  return false;
               }
               ptCtx->m_strRecFilePath = userInfo.m_strRecFilePath.c_str();
               ptCtx->m_strRecUrl = userInfo.m_strRecUrl.c_str();
               ptCtx->m_strSipId = userInfo.m_strSipId.c_str();
            }
         }

         if(ptCtx)
         {
            if(ptSioUserInfo->m_dequeURMeetUserList.size() > 0)
            {
               ptCtx->m_dequeURMeetUserList = ptSioUserInfo->m_dequeURMeetUserList;
            }
	    ptCtx->m_bIsRecStarted = false;
            ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_RECORDING_STOP);
            if(!process_msg(ptCtx)) 
            {
               XGLOG_ERROR("URCONF_SIO_MSG_RECORDING_STOP process_msg failed for callid:%s",strID.c_str());
               return false;
            }
         }
         else
         {
            XGLOG_INFO("URCONF_SIO_MSG_RECORDING_STOP call context not found for conn_id:%s" , strConnId.c_str());
         }
         break;
      }
      case URCONF_SIO_MSG_SDP_UPDATE_REQUEST :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_SDP_UPDATE_REQUEST");
         URCBCallContext * ptCtx = NULL;
         URConfRoom * urConfRoom = NULL;
         std::string strID("");
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_SDP_UPDATE_REQUEST getConfRoomCallContext conf room id (%s) not found",\
                  strConfRoomId.c_str());
            return false;
         }
         ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         for(userItr; userItr != ptSioUserInfo->m_tURConfUserInfoList.end();userItr++)
         {
            strConnId = userItr->first;           
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
            if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
            {
               strID = ptSioUserInfo->m_strConfCallId = itr->second->m_strConfCallId.c_str();
               itr->second = ptSioUserInfo;
               if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx)) 
               {
                  XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_SDP_UPDATE_REQUEST get URCBCallContext failed for callid (%s)",\
                        strID.c_str());
                  return false;
               }
               ptCtx->m_strSDPInfo = ptSioUserInfo->m_tURConfUserMediaInfo.m_strSdp.c_str();
            }
         }
         if(ptCtx)
         {
            fillURParticipantListInfo(ptCtx,urConfRoom,ptSioUserInfo);
            ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_SDP_UPDATE_REQ);
            if(!process_msg(ptCtx)) 
            {
               XGLOG_ERROR("handle_sio_mbc_message URCBCALL_STATUS_SDP_UPDATE_REQ process_msg failed for callid:%s",strID.c_str());
               return false;
            }
         }
         else
         {
         XGLOG_INFO("handle_sio_mbc_message URCBCALL_STATUS_SDP_UPDATE_REQ call context not found for conn_id:%s" , strConnId.c_str());
         }
         break;
      }
      case URCONF_SIO_MSG_TILE_CHANGE_REQUEST :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_TILE_CHANGE_REQUEST");
         URCBCallContext * ptCtx = NULL;
         URConfRoom * urConfRoom = NULL;
         std::string strID("");
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_TILE_CHANGE_REQUEST getConfRoomCallContext conf room id (%s) not found",\
                  strConfRoomId.c_str());
            return false;
         }
         ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         for(userItr; userItr != ptSioUserInfo->m_tURConfUserInfoList.end();userItr++)
         {
            strConnId = userItr->first;         
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
            if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
            {
               strID = ptSioUserInfo->m_strConfCallId = itr->second->m_strConfCallId.c_str();
               itr->second = ptSioUserInfo;
               if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx)) 
               {
                  XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_TILE_CHANGE_REQUEST get URCBCallContext failed for callid (%s)",\
                        strID.c_str());
                  return false;
               }
               ptCtx->m_strSDPInfo = ptSioUserInfo->m_tURConfUserMediaInfo.m_strSdp.c_str();
            }
         }
         if(ptCtx)
         {
            fillURParticipantListInfo(ptCtx,urConfRoom,ptSioUserInfo);
            ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_TILE_CHANGE_REQ);
            if(!process_msg(ptCtx)) 
            {
               XGLOG_ERROR("handle_sio_mbc_message URCBCALL_STATUS_TILE_CHANGE_REQ process_msg failed for callid:%s",strID.c_str());
               return false;
            }
         }
         else
         {
            XGLOG_INFO("handle_sio_mbc_message URCBCALL_STATUS_TILE_CHANGE_REQ call context not found for conn_id:%s" , strConnId.c_str());
         }
         break;
      }
      case URCONF_SIO_MSG_SCREENSHARE_VIDEO_REQUEST :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_SCREENSHARE_VIDEO_REQUEST");
         URCBCallContext * ptCtx = NULL;
         URConfRoom * urConfRoom = NULL;
         std::string strID("");
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_SCREENSHARE_VIDEO_REQUEST getConfRoomCallContext conf room id (%s) not found",\
                  strConfRoomId.c_str());
            return false;
         }
         ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         for(userItr; userItr != ptSioUserInfo->m_tURConfUserInfoList.end();userItr++)
         {
            strConnId = userItr->first;                    
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
            if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
            {
               strID = ptSioUserInfo->m_strConfCallId = itr->second->m_strConfCallId.c_str();
               itr->second = ptSioUserInfo;
               if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx)) 
               {
                  XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_SCREENSHARE_VIDEO_REQUEST get URCBCallContext failed for callid (%s)",\
                        strID.c_str());
                  return false;
               }
               ptCtx->m_strSDPInfo = ptSioUserInfo->m_tURConfUserMediaInfo.m_strSdp.c_str();
            }
         }
         if(ptCtx)
         {
            fillURParticipantListInfo(ptCtx,urConfRoom,ptSioUserInfo);
            ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_SCREENSHARE_VIDEO_REQ);
            if(!process_msg(ptCtx)) 
            {
               XGLOG_ERROR("handle_sio_mbc_message URCBCALL_STATUS_SCREENSHARE_VIDEO_REQ process_msg failed for callid:%s",strID.c_str());
               return false;
            }
         }
         else
         {
            XGLOG_INFO("handle_sio_mbc_message URCBCALL_STATUS_SCREENSHARE_VIDEO_REQ call context not found for conn_id:%s" , strConnId.c_str());
         }
         break;
      }
      case URCONF_SIO_MSG_ACTIVE_SPEAKER_VIDEO_REQUEST :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_ACTIVE_SPEAKER_VIDEO_REQUEST");
         URCBCallContext * ptCtx = NULL;
         URConfRoom * urConfRoom = NULL;
         std::string strID("");
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_ACTIVE_SPEAKER_VIDEO_REQUEST getConfRoomCallContext conf room id (%s) not found",\
                  strConfRoomId.c_str());
            return false;
         }
         ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         for(userItr; userItr != ptSioUserInfo->m_tURConfUserInfoList.end();userItr++)
         {
            strConnId = userItr->first;         
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
            if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
            {
               strID = ptSioUserInfo->m_strConfCallId = itr->second->m_strConfCallId.c_str();
               itr->second = ptSioUserInfo;
               if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx)) 
               {
                  XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_ACTIVE_SPEAKER_VIDEO_REQUEST get URCBCallContext failed for callid (%s)",\
                        strID.c_str());
                  return false;
               }
               ptCtx->m_strSDPInfo = ptSioUserInfo->m_tURConfUserMediaInfo.m_strSdp.c_str();
            }
         }
         if(ptCtx)
         {
            ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_ACTIVE_SPEAKER_VIDEO_REQ);
            if(!process_msg(ptCtx)) 
            {
               XGLOG_ERROR("handle_sio_mbc_message URCBCALL_STATUS_ACTIVE_SPEAKER_VIDEO_REQ process_msg failed for callid:%s",strID.c_str());
               return false;
            }
         }
         else
         {
            XGLOG_INFO("handle_sio_mbc_message URCBCALL_STATUS_ACTIVE_SPEAKER_VIDEO_REQ call context not found for conn_id:%s" , strConnId.c_str());
         }
         break;
      }
      case URCONF_SIO_MSG_GRID_VIEW_VIDEO_REQUEST :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_GRID_VIEW_VIDEO_REQUEST");
         URCBCallContext * ptCtx = NULL;
         URConfRoom * urConfRoom = NULL;
         std::string strID("");
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_GRID_VIEW_VIDEO_REQUEST getConfRoomCallContext conf room id (%s) not found",\
                  strConfRoomId.c_str());
            return false;
         }
         ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         for(userItr; userItr != ptSioUserInfo->m_tURConfUserInfoList.end();userItr++)
         {
            strConnId = userItr->first;         
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
            if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
            {
               strID = ptSioUserInfo->m_strConfCallId = itr->second->m_strConfCallId.c_str();
               itr->second = ptSioUserInfo;
               if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx)) 
               {
                  XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_GRID_VIEW_VIDEO_REQUEST get URCBCallContext failed for callid (%s)",\
                        strID.c_str());
                  return false;
               }
               ptCtx->m_strSDPInfo = ptSioUserInfo->m_tURConfUserMediaInfo.m_strSdp.c_str();
            }
         }
         if(ptCtx)
         {
            ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_GRID_VIEW_VIDEO_REQ);
            if(!process_msg(ptCtx)) 
            {
               XGLOG_ERROR("handle_sio_mbc_message URCBCALL_STATUS_GRID_VIEW_VIDEO_REQ process_msg failed for callid:%s",strID.c_str());
               return false;
            }
         }
         else
         {
            XGLOG_INFO("handle_sio_mbc_message URCBCALL_STATUS_GRID_VIEW_VIDEO_REQ call context not found for conn_id:%s" , strConnId.c_str());
         }
         break;
      }
      case URCONF_SIO_MSG_ACTIVE_SPEAKER_INFO :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_ACTIVE_SPEAKER_INFO");
#if 0
				 std::string strConfHostedIp("");
				 if(m_tURConfRoomCtxHndlr.findConfRoomIdInRemoteList(strConfRoomId,strConfHostedIp))
				 {
					 return false;
				 }
#endif
         URConfRoom * urConfRoom = NULL;
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_ACTIVE_SPEAKER_INFO getConfRoomCallContext conf room id (%s) not found",\
                  strConfRoomId.c_str());
            return false;
         }
         int confCallCount(0);
         m_tURConfRoomCtxHndlr.get_conf_call_list_size(strConfRoomId,confCallCount);
         if(!confCallCount)
         {
           return false;
         }

        /* ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         if(userItr != ptSioUserInfo->m_tURConfUserInfoList.end())
         {
            strConnId = userItr->first;
            URActiveSpeakerInfo t_urActiveSpeakerInfo = urConfRoom->m_URActiveSpeakerInfo;

            bool b_isNewConfActiveSpeaker = false;
            if(t_urActiveSpeakerInfo.m_strConnId.empty())
            {
               b_isNewConfActiveSpeaker = true;
            }
            //Update active speaker info in confroom
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
						if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
						{
							std::string lastActiveSpeaker = urConfRoom->m_URActiveSpeakerInfo.m_strConnId.c_str();
							if(strcmp(lastActiveSpeaker.c_str(),strConnId.c_str()) != 0)
							{
								b_isNewConfActiveSpeaker = true;
								urConfRoom->m_URActiveSpeakerInfo.m_strConnId = strConnId.c_str();
								urConfRoom->m_URActiveSpeakerInfo.m_strCallId = itr->second->m_strConfCallId.c_str();
								XGLOG_INFO("handle_sio_mbc_message URCONF_SIO_MSG_ACTIVE_SPEAKER_INFO setting callid:%s as active speaker", urConfRoom->m_URActiveSpeakerInfo.m_strCallId.c_str());
							}
						}
            else
            {
               XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_ACTIVE_SPEAKER_INFO unable to find the user with connid:%s", strConnId.c_str());
            }
						if(b_isNewConfActiveSpeaker && !urConfRoom->isScreenShareActive)
						{
							XGLOG_INFO("$$$$$$$$$$$$ SENDING ACTIVE SPEAKER INFO :%s for confroom :%s $$$$$$$$$$$", strConnId.c_str(), strConfRoomId.c_str());
							//send active speaker notify
							sendConfRoomActiveSpeakerInfo(strConfRoomId); 

							ptSioUserInfo->m_tURConfEvent.m_strEventName = "current_active_speaker_info";
							post_msg_to_socketio(ptSioUserInfo);

#if 0 
							//Schedule Active Speaker Timer
							URCBTimerData * urcbTimerData = new URCBTimerData();
							urcbTimerData->m_strConfMeetingId = strConfRoomId.c_str();
							std::string uid = urcbTimerData->m_strConfConnId = strConfRoomId.c_str();
							set_timer(uid, urcbTimerData, EN_SCHEDULER_URCONF_ACTIVE_SPEAKER_INFO, URCONF_ACTIVE_SPEAKER_INFO_TIMER_VAL);
#endif
						}
         }*/

				 ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         if(userItr != ptSioUserInfo->m_tURConfUserInfoList.end()){
           strConnId = userItr->first;
           updateUserVolume(strConnId, ptSioUserInfo->m_tURConfUserMediaInfo.m_volume, urConfRoom);
           std::string activeSpeaker = findActiveSpeaker(urConfRoom);
           if(activeSpeaker.empty()){
             XGLOG_INFO("None of the participant is fulfiling the criteria, active speaker is empty----------");
             strConnId = "";
             return false;
           }else{
             strConnId = activeSpeaker;
           }
				   URActiveSpeakerInfo t_urActiveSpeakerInfo = urConfRoom->m_URActiveSpeakerInfo;
				   bool b_isNewConfActiveSpeaker = false;
           if(t_urActiveSpeakerInfo.m_strConnId.empty()){
             b_isNewConfActiveSpeaker = true;
           }
           //Update active speaker info in confroom
           ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
           if(itr != urConfRoom->m_URCBSIOUserInfoList.end()){
             std::string lastActiveSpeaker = urConfRoom->m_URActiveSpeakerInfo.m_strConnId.c_str();
             if(strcmp(lastActiveSpeaker.c_str(),strConnId.c_str()) != 0){
               b_isNewConfActiveSpeaker = true;
               urConfRoom->m_URActiveSpeakerInfo.m_strConnId = strConnId.c_str();
               urConfRoom->m_URActiveSpeakerInfo.m_strCallId = itr->second->m_strConfCallId.c_str();
             }
           }else{
             XGLOG_ERROR("------ACTIVE_SPEAKER_INFO unable to find the user with connid------:%s", strConnId.c_str());
           }
				   if(b_isNewConfActiveSpeaker && !urConfRoom->isScreenShareActive){
             XGLOG_INFO("----- SENDING ACTIVE SPEAKER INFO---- :%s for confroom--- :%s", strConnId.c_str(), strConfRoomId.c_str());
             //send active speaker notify
						 auto itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
						 if(itr != urConfRoom->m_URCBSIOUserInfoList.end()){
						   //URCBSIOUserInfo*  ptSioUserInfo_ = nullptr;
               ptSioUserInfo->m_tURConfUserInfoList = itr->second->m_tURConfUserInfoList;
               ptSioUserInfo->m_tURConfEvent.m_strEventName = "current_active_speaker_info";
               sendConfRoomActiveSpeakerInfo(strConfRoomId);
               post_msg_to_socketio(ptSioUserInfo);
						 }
					 }
         }
         break;
      }
      case URCONF_SIO_MSG_INITIAL_SPEAKER_INFO :
         {
            XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_INITIAL_SPEAKER_INFO");
            URConfRoom * urConfRoom = NULL;

            std::string strConfHostedIp("");
            if(m_tURConfRoomCtxHndlr.findConfRoomIdInRemoteList(strConfRoomId,strConfHostedIp))
            {
              return false;
            }

            if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
            {
               XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_INITIAL_SPEAKER_INFO getConfRoomCallContext conf room id (%s) not found",\
                     strConfRoomId.c_str());
               return false;
            }

            ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
            if(userItr != ptSioUserInfo->m_tURConfUserInfoList.end())
            {
               strConnId = userItr->first;
               URActiveSpeakerInfo t_urActiveSpeakerInfo = urConfRoom->m_URActiveSpeakerInfo;

							 //Update active speaker info in confroom
							 ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
               if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
               {
                    urConfRoom->m_URActiveSpeakerInfo.m_strConnId = strConnId.c_str();
                    urConfRoom->m_URActiveSpeakerInfo.m_strCallId = itr->second->m_strConfCallId.c_str();
                    XGLOG_INFO("URCONF_SIO_MSG_INITIAL_SPEAKER_INFO setting callid:%s as active speaker", urConfRoom->m_URActiveSpeakerInfo.m_strCallId.c_str());
               }
               else
               {
                  XGLOG_ERROR("URCONF_SIO_MSG_INITIAL_SPEAKER_INFO unable to find the user with connid:%s", strConnId.c_str());
               }
               if(!urConfRoom->isScreenShareActive)
               {
                 XGLOG_INFO("$$$$$$$$$$$$ SENDING INITIAL SPEAKER INFO for confroom :%s $$$$$$$$$$$", strConfRoomId.c_str());
                 //send active speaker notify
                 sendConfRoomActiveSpeakerInfo(strConfRoomId);

                 ptSioUserInfo->m_tURConfEvent.m_strEventName = "current_active_speaker_info";
                 post_msg_to_socketio(ptSioUserInfo);

               }
            }

            XGLOG_INFO("Out handle_sio_mbc_message URCONF_SIO_MSG_INITIAL_SPEAKER_INFO");
            break;
         }
      case URCONF_SIO_MSG_USER_ACTION_DISCONNECTED :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_USER_ACTION_DISCONNECTED");
         URCBCallContext * ptCtx = NULL;
         URConfRoom * urConfRoom = NULL;
         std::string strID("");
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_USER_ACTION_DISCONNECTED getConfRoomCallContext conf room id (%s) not found",\
            strConfRoomId.c_str());
            return false;
         }
         ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         for(userItr; userItr != ptSioUserInfo->m_tURConfUserInfoList.end();userItr++)
         {
            strConnId = userItr->first;            
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
            if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
            {
               strID = ptSioUserInfo->m_strConfCallId = itr->second->m_strConfCallId.c_str();
               itr->second = ptSioUserInfo;
               if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx))
               {
                  XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_USER_ACTION_DISCONNECTED get URCBCallContext failed for callid (%s)",\
                     strID.c_str());
                  m_tURConfRoomCtxHndlr.remove_connid_from_socket_user_info_list(strConfRoomId,strConnId);                     
                  return false;
               }
               if(ptCtx)
               {
                  ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_CALL_DISCONNECTED);
                  if(!process_msg(ptCtx)) 
                  {
                     XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_USER_ACTION_DISCONNECTED failed for callid:%s",strID.c_str());
                     return false;
                  }
                  sendCallDisconnectRequest(ptCtx);//send bye to the socket io message user
                  handleCallClearing(ptCtx);
                  //activeSpeakerOnDisconnect(strConfRoomId, urConfRoom, ptSioUserInfo);
               }              
            }
         }
         XGLOG_INFO("Out handle_sio_mbc_message URCONF_SIO_MSG_USER_ACTION_DISCONNECTED");
         break;
      }
      case URCONF_SIO_MSG_USER_ACTION_TERMINATED :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_USER_ACTION_TERMINATED");
         URCBCallContext * ptCtx = NULL;
         URConfRoom * urConfRoom = NULL;
         std::string strID("");
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_USER_ACTION_TERMINATED getConfRoomCallContext conf room id (%s) not found",\
            strConfRoomId.c_str());
            return false;
         }
         ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         int userListSize = ptSioUserInfo->m_tURConfUserInfoList.size();
         XGLOG_INFO("handle_sio_mbc_message URCONF_SIO_MSG_USER_ACTION_TERMINATED user_list_size:%d",userListSize);
         for(userItr; userItr != ptSioUserInfo->m_tURConfUserInfoList.end();userItr++)
         {
            strConnId = userItr->first;
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
            if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
            {
               strID = ptSioUserInfo->m_strConfCallId = itr->second->m_strConfCallId.c_str();	
               itr->second = ptSioUserInfo;
               if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx))
               {
                  XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_USER_ACTION_TERMINATED get URCBCallContext failed for callid (%s)",\
                     strID.c_str());
                  m_tURConfRoomCtxHndlr.remove_connid_from_socket_user_info_list(strConfRoomId,strConnId);                      
                  return false;
               }
               if(ptCtx)
               {
                  XGLOG_DEBUG("handle_sio_mbc_message URCONF_SIO_MSG_USER_ACTION_TERMINATED process Socket IO Disconnected Indication for callid (%s)",strID.c_str());
                  ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_CALL_DISCONNECTED);
                  if(!process_msg(ptCtx)) 
                  {
                     XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_USER_ACTION_TERMINATED failed for callid:%s",strID.c_str());
                     return false;
                  }
                  sendCallDisconnectRequest(ptCtx);//send bye to the socket io message user
                  handleCallClearing(ptCtx);
                  //activeSpeakerOnDisconnect(strConfRoomId, urConfRoom, ptSioUserInfo);
               }
            }
         }
         XGLOG_INFO("Out handle_sio_mbc_message URCONF_SIO_MSG_USER_ACTION_TERMINATED");
         break;
      }
      case URCONF_SIO_MSG_PARTICIPANT_REMOVE :
      {
         XGLOG_INFO("In handle_sio_mbc_message URCONF_SIO_MSG_PARTICIPANT_REMOVE");
         URCBCallContext * ptCtx = NULL;
         URConfRoom * urConfRoom = NULL;
         std::string strID("");
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_PARTICIPANT_REMOVE getConfRoomCallContext conf room id (%s) not found",\
                        strConfRoomId.c_str());
            return false;
         }
         ITR_CONF_USER_INFO_MAP userItr =  ptSioUserInfo->m_tURConfUserInfoList.begin();
         for(userItr; userItr != ptSioUserInfo->m_tURConfUserInfoList.end();userItr++)
         {
            std::string connId = userItr->first;          
            ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(connId);
            if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
            {
               strID = ptSioUserInfo->m_strConfCallId = itr->second->m_strConfCallId.c_str();	
               itr->second = ptSioUserInfo;
               if (!m_tURCBCallCtxHndlr.getURCBCallContext(strID, ptCtx))
               {
                  XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_PARTICIPANT_REMOVE get URCBCallContext failed for callid (%s)", strID.c_str());
                  return false;
               }
               if(ptCtx)
               {
                  ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_CALL_DISCONNECTED);
                  if(!process_msg(ptCtx))
                  {
                     XGLOG_ERROR("handle_sio_mbc_message URCONF_SIO_MSG_PARTICIPANT_REMOVE failed. process_msg URCBCALL_STATUS_CALL_DISCONNECTED failed for callid:%s",strID.c_str());
                     return false;
                  }
                  sendCallDisconnectRequest(ptCtx);//send bye to the socket io message user
                  handleCallClearing(ptCtx);
                  //activeSpeakerOnDisconnect(strConfRoomId, urConfRoom, ptSioUserInfo);
               }
            }
         }
         XGLOG_INFO("Out handle_sio_mbc_message URCONF_SIO_MSG_PARTICIPANT_REMOVE");
         break;
      }
      case URCONF_SIO_MSG_END_MEETING_ALL:
      {
         XGLOG_INFO("handle_sio_mbc_message processing URCONF_SIO_MSG_END_MEETING_ALL for conf_id:%s",strConfRoomId.c_str());
         URConfRoom * urConfRoom = NULL;
         if(!m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
         {
            XGLOG_ERROR("URCONF_SIO_MSG_END_MEETING_ALL getConfRoomCallContext conf room id (%s) not found",\
            strConfRoomId.c_str());
            return false;
         }
         CONF_CALL_LIST_MAP tConfUserList = urConfRoom->m_URConfCallList;
         ITR_CONF_CALL_LIST_MAP userItr = tConfUserList.begin();
         for(userItr; userItr != tConfUserList.end(); userItr++)
         {
            std::string strCallId = userItr->first;
            URCBCallContext * ptCtx = NULL;
            if (!m_tURCBCallCtxHndlr.getURCBCallContext(strCallId, ptCtx))
            {
               XGLOG_ERROR("URCONF_SIO_MSG_END_MEETING_ALL get URCBCallContext failed for callid (%s)",\
                  strCallId.c_str());
               continue;
            }
            ptCtx->setStatus(URCBCallContext::URCBCALL_STATUS_CALL_DISCONNECTED);
            if(!process_msg(ptCtx)) 
            {
               XGLOG_ERROR("URCONF_SIO_MSG_END_MEETING_ALL failed URCBCALL_STATUS_CALL_DISCONNECTED process_msg failed for callid:%s",strCallId.c_str());
               continue;
            }
            sendCallDisconnectRequest(ptCtx);
            handleCallClearing(ptCtx); 
         }
				 //Cancel the scheduled meeting timer
   			 cancel_timer(strConfRoomId, EN_SCHEDULER_URCONF_MEETING_DURATION);
         if(!m_tURConfRoomCtxHndlr.deleteConfRoomCallContext(strConfRoomId))
         {
            XGLOG_ERROR("URCONF_SIO_MSG_END_MEETING_ALL deleteConfRoomCallContext failed - %s", strConfRoomId.c_str());
         }
         ptSioUserInfo->m_tURConfEvent.m_strEventName = "end_meeting_all_done";
         post_msg_to_socketio(ptSioUserInfo);
         XGLOG_INFO("Out handle_sio_mbc_message URCONF_SIO_MSG_END_MEETING_ALL");
         break;
      }
      case URCONF_SIO_MSG_UNKNOWN:
      default:
      {
         break;
      }
   }

   return true; 
}

bool URCBController::fillURParticipantListInfo(URCBCallContext *& ptCtx, URConfRoom * urConfRoom, URCBSIOUserInfo * ptSioUserInfo)
{
   XGLOG_FUNCTION_EN_EX("");
   std::string strConfRoomId = ptCtx->m_strConfRoomId.c_str(); 
   std::string strConnId("");
   ptCtx->m_dequeURConfParticipantList.clear();
   for(int i = 0; i < ptSioUserInfo->m_dequeURConfParticipantList.size(); i++)
   {
      URCBSIOUserInfo* ptUserInfo = NULL;
      URCONF_PARTICIPANT_INFO participantInfo = ptSioUserInfo->m_dequeURConfParticipantList[i];
      strConnId = participantInfo.m_strConnId;
      ITR_SIO_USER_INFO_MAP itr = urConfRoom->m_URCBSIOUserInfoList.find(strConnId);
      if(itr != urConfRoom->m_URCBSIOUserInfoList.end())
      {
         ptUserInfo = itr->second;
         XGLOG_INFO("fillURParticipantListInfo assigning call_id:%s for conn_id(%s) in confRoom(%s)",\
                     ptUserInfo->m_strConfCallId.c_str(),strConnId.c_str(),strConfRoomId.c_str());
         if(!ptUserInfo) 
         {
            XGLOG_ERROR("fillURParticipantListInfo socket IO user info is nullptr");
            continue;
         }
         participantInfo.m_strCallId = ptUserInfo->m_strConfCallId.c_str();
         ptCtx->m_dequeURConfParticipantList.push_back(participantInfo);
      }
      //Inserting audio MCU and active speakers mid value in m-line
      else if((strcmp(strConnId.c_str(),"audio_only")==0) || (strcmp(strConnId.c_str(),"screenshare")==0))
      {
      participantInfo.m_strCallId = strConnId;
      ptCtx->m_dequeURConfParticipantList.push_back(participantInfo);
      }
   }
   return true;
}

std::string URCBController::form_socketIO_json_msg(URCBSIOUserInfo *ptSioUserInfo)
{
   Document doc;
   StringBuffer strBuf;
   Writer<StringBuffer> writer(strBuf);
   Value urConfSIOVal(kObjectType);
   Document::AllocatorType& allocator = doc.GetAllocator();

   Value urConfMsgVal(kObjectType);
   //Encode conf event info
   Value urConfEventInfo(kObjectType);
   URCONF_EVENT &confEventInfo = ptSioUserInfo->m_tURConfEvent;
   urConfEventInfo.AddMember("event_sequence", setJsonParam(confEventInfo.m_strEventSeq, doc), allocator);
   urConfEventInfo.AddMember("event_name", setJsonParam(confEventInfo.m_strEventName, doc), allocator);
   urConfEventInfo.AddMember("event_time", setJsonParam(confEventInfo.m_strEventTime, doc), allocator);
   urConfMsgVal.AddMember("ur_conf_event", urConfEventInfo, allocator);

   //Encode conf meet info
   Value urConfMeetingInfo(kObjectType);
   URCONF_MEETING_INFO &confMeetInfo = ptSioUserInfo->m_tURConfMeetInfo;
   urConfMeetingInfo.AddMember("meeting_id", setJsonParam(confMeetInfo.m_strMeetingId, doc), allocator);
   urConfMeetingInfo.AddMember("meeting_name", setJsonParam(confMeetInfo.m_strMeetingName, doc), allocator);
   urConfMeetingInfo.AddMember("host_joined", setJsonParam(confMeetInfo.m_bHostJoined, doc), allocator);
   urConfMsgVal.AddMember("ur_conf_meeting_info", urConfMeetingInfo, allocator);

   //Encode conf user info list
   Value confUserInfoList(kArrayType);
   if(ptSioUserInfo->m_tURConfUserInfoList.size()>0) 
   {
      ITR_CONF_USER_INFO_MAP itr = ptSioUserInfo->m_tURConfUserInfoList.begin();
      for(itr; itr != ptSioUserInfo->m_tURConfUserInfoList.end(); itr++) 
      {
         URCONF_USER_INFO &userInfo = itr->second;
         Value newUserInfoVal(kObjectType);
         newUserInfoVal.AddMember("user_id", setJsonParam(userInfo.m_strUserId, doc), allocator);
         newUserInfoVal.AddMember("sip_id", setJsonParam(userInfo.m_strSipId, doc), allocator);
         newUserInfoVal.AddMember("connection_id", setJsonParam(userInfo.m_strConnId, doc), allocator);
         newUserInfoVal.AddMember("prev_act_spkr_info", setJsonParam(userInfo.m_strPrevSpkrConnId, doc), allocator);
         newUserInfoVal.AddMember("device_id", setJsonParam(userInfo.m_strDeviceId, doc), allocator);
         newUserInfoVal.AddMember("user_type", setJsonParam(userInfo.m_strUserType, doc), allocator);
         confUserInfoList.PushBack(newUserInfoVal, allocator);
      }
      urConfMsgVal.AddMember("ur_user_info", confUserInfoList, allocator);
   }

   //Encode conf user media info
   Value urConfMediaInfo(kObjectType);
   URCONF_MEDIA_INFO &confUserMediaInfo = ptSioUserInfo->m_tURConfUserMediaInfo;
   if(confEventInfo.m_strEventName == "current_active_speaker_info") {
       std::string strSdp("");
       urConfMediaInfo.AddMember("sdp_str", setJsonParam(strSdp, doc), allocator);
   } else {
       urConfMediaInfo.AddMember("sdp_str", setJsonParam(confUserMediaInfo.m_strSdp, doc), allocator);
   }
   urConfMsgVal.AddMember("ur_media_info", urConfMediaInfo, allocator);


   //Encode conf ccaas info
   Value urConfCcaasInfo(kObjectType);
   URCONF_CCAAS_INFO &confCcaasInfo = ptSioUserInfo->m_tURConfCcaasInfo;
   urConfCcaasInfo.AddMember("annc_text", setJsonParam(confCcaasInfo.m_strAnncText, doc), allocator);
   urConfMsgVal.AddMember("ur_ccaas_info", urConfCcaasInfo, allocator);

   urConfSIOVal.AddMember("ur_conf_message", urConfMsgVal, allocator);

   urConfSIOVal.Accept(writer);
   string urConfSIOJsonData("");
   urConfSIOJsonData = strBuf.GetString();

   return urConfSIOJsonData;
}

bool URCBController::decode_sio_json_msg_from_MBC(const char* jsonData,URCBSIOUserInfo *& ptSioUserInfo)
{
   if(!ptSioUserInfo)
   {
      XGLOG_INFO( "decode_sio_json_msg_from_MBC failed,UserInfo Object is NULL");
      return false;
   }
   Document doc;
   ParseResult ok = doc.Parse(jsonData);
   if(!ok) 
   {
      XGLOG_ERROR( "parseJsonMsg failed. Encountered with json parse error !");
      return false;
   } 
   else if(!doc.IsObject()) 
   {
      XGLOG_ERROR( "parseJsonMsg failed. Not able create documnet object !");
      return false;
   }
   if(doc.HasMember("ur_conf_message"))
   {
      const Value &confMsgDetails = doc["ur_conf_message"];
      if(confMsgDetails.HasMember("ur_conf_event"))
      {
         const Value &confEventInfoDetails = confMsgDetails["ur_conf_event"];
         if(confEventInfoDetails.HasMember("event_sequence"))
         {
            ptSioUserInfo->m_tURConfEvent.m_strEventSeq = confEventInfoDetails["event_sequence"].GetString();
         }
         else
         {
            XGLOG_ERROR("readConfEvent Details failed. 'event_sequence' is missing in json message");
            return false;
         }
         if(confEventInfoDetails.HasMember("event_name"))
         {
            ptSioUserInfo->m_tURConfEvent.m_strEventName = confEventInfoDetails["event_name"].GetString();
         }
         else
         {
            XGLOG_ERROR("readConfEvent Details failed. 'event_name' is missing in json message");
            return false;
         }
         if(confEventInfoDetails.HasMember("event_time"))
         {
            ptSioUserInfo->m_tURConfEvent.m_strEventTime = confEventInfoDetails["event_time"].GetString();
         }
         else
         {
            XGLOG_ERROR("readConfEvent Details failed. 'event_time' is missing in json message");
            return false;
         }
      }
      else
      {
         XGLOG_ERROR("readConfEvent Details failed. 'ur_conf_event' is missing in json message");
         return false;
      }
      if(confMsgDetails.HasMember("ur_conf_meeting_info"))
      {
         const Value &confMeetInfoDetails = confMsgDetails["ur_conf_meeting_info"];
         if(confMeetInfoDetails.HasMember("meeting_id"))
         {
            ptSioUserInfo->m_tURConfMeetInfo.m_strMeetingId = confMeetInfoDetails["meeting_id"].GetString();
         }
         else
         {
            XGLOG_ERROR("readMeetingInfo Details failed. 'meeting_id' is missing in json message");
            return false;
         }
         if(confMeetInfoDetails.HasMember("meeting_name"))
         {
            ptSioUserInfo->m_tURConfMeetInfo.m_strMeetingName = confMeetInfoDetails["meeting_name"].GetString();
         }
         else
         {
            XGLOG_ERROR("readMeetingInfo Details failed. 'meeting_name' is missing in json message");
            return false;
         }
         if(confMeetInfoDetails.HasMember("host_joined"))
         {
            ptSioUserInfo->m_tURConfMeetInfo.m_bHostJoined  = confMeetInfoDetails["host_joined"].GetBool();
         }
         else
         {
            XGLOG_ERROR("readMeetingInfo Details failed. 'host_joined' is missing in json message");
            return false;
         }
      }
      else
      {
         XGLOG_ERROR("readMeetingInfo Details failed. 'ur_conf_meeting_info' is missing in json message");
         return false;
      }

      if(confMsgDetails.HasMember("ur_user_info")) 
      {
         const Value &confUserInfo = confMsgDetails["ur_user_info"];
         if(confUserInfo.IsArray()) 
         {
            Value::ConstValueIterator userItr = confUserInfo.Begin();
            for(userItr; userItr != confUserInfo.End(); userItr++) 
            {
               const Value &userInfo = *userItr;
               if(!fillConfUserInfoDetails(userInfo,ptSioUserInfo)) 
               {
                  XGLOG_ERROR( "fillConfUserInfoDetails failed while filling user info in video conf details !");
                  return false;
               }
            }
         }
      }
      else
      {
         XGLOG_ERROR("readUserInfo Details failed. 'ur_user_info' is missing in json message");
         return false;
      }

      if(confMsgDetails.HasMember("ur_participant_list")) 
      {
         const Value &confParticipantInfo = confMsgDetails["ur_participant_list"];
         if(confParticipantInfo.IsArray()) 
         {
            Value::ConstValueIterator participantItr = confParticipantInfo.Begin();
            for(participantItr; participantItr != confParticipantInfo.End(); participantItr++) 
            {
               const Value &participantInfo = *participantItr;
               if(!fillConfParticipantInfoDetails(participantInfo,ptSioUserInfo)) 
               {
                  XGLOG_ERROR( "fillConfParticipantInfoDetails failed while filling participant info in video conf details !");
               }
            }
         }
      }
      else
      {
         XGLOG_ERROR("readParticipantInfo Details failed. 'ur_participant_list' is missing in json message");
      }

      if(confMsgDetails.HasMember("urmeet_user_list")) 
      {
         const Value &confUserList = confMsgDetails["urmeet_user_list"];
         if(confUserList.IsArray()) 
         {
            Value::ConstValueIterator userItr = confUserList.Begin();
            for(userItr; userItr != confUserList.End(); userItr++) 
            {
               const Value &userInfo = *userItr;
               if(!fillURMeetUserInfo(userInfo,ptSioUserInfo)) 
               {
                  XGLOG_ERROR( "fillURMeetUserInfo failed while filling user info in video conf details !");
               }
            }
         }
      }
      else
      {
         XGLOG_ERROR("readParticipantInfo Details failed. 'ur_participant_list' is missing in json message");
      }

      if(confMsgDetails.HasMember("ur_media_info"))
      {
	 const Value &confMediaInfoDetails = confMsgDetails["ur_media_info"];
         if(confMediaInfoDetails.HasMember("sdp_str")) 
         {
            ptSioUserInfo->m_tURConfUserMediaInfo.m_strSdp  = confMediaInfoDetails["sdp_str"].GetString();
         } 
         else 
         {
            XGLOG_ERROR("readMediaInfo Details failed. 'sdp_str' is missing in json message");
         }
         if(confMediaInfoDetails.HasMember("volume")){
           ptSioUserInfo->m_tURConfUserMediaInfo.m_volume  = confMediaInfoDetails["volume"].GetDouble();
         }
      }
      else
      {
         XGLOG_ERROR("readMediaInfo Details failed. 'ur_media_info' is missing in json message");
         return false;
      }

      if(confMsgDetails.HasMember("ur_ccaas_info"))
      {
	 const Value &confCcaasInfoDetails = confMsgDetails["ur_ccaas_info"];
         if(confCcaasInfoDetails.HasMember("queue_id")) 
         {
            ptSioUserInfo->m_tURConfCcaasInfo.m_strQueueId  = confCcaasInfoDetails["queue_id"].GetString();
         } 
         else 
         {
            XGLOG_ERROR("readMediaInfo Details failed. 'queue_id' is missing in json message");
         }

	if(confCcaasInfoDetails.HasMember("domain_id"))
         {
            ptSioUserInfo->m_tURConfCcaasInfo.m_strDomainId  = confCcaasInfoDetails["domain_id"].GetString();
         }
         else
         {
            XGLOG_ERROR("readMediaInfo Details failed. 'domain_id' is missing in json message");
         }

         if(confCcaasInfoDetails.HasMember("customer_name")) 
         {
            ptSioUserInfo->m_tURConfCcaasInfo.m_strCustomerName  = confCcaasInfoDetails["customer_name"].GetString();
         } 
         else 
         {
            XGLOG_ERROR("readMediaInfo Details failed. 'customer_name' is missing in json message");
         }

         if(confCcaasInfoDetails.HasMember("customer_number")) 
         {
            ptSioUserInfo->m_tURConfCcaasInfo.m_strCustomerNum  = confCcaasInfoDetails["customer_number"].GetString();
         } 
         else 
         {
            XGLOG_ERROR("readMediaInfo Details failed. 'customer_number' is missing in json message");
         }
      }
   }
   else
   {
      XGLOG_ERROR("readConfMessage Details failed. 'ur_conf_message' is missing in json message");
      return false;
   }

   return true;
}

std::string URCBController::form_conf_room_info_json_msg(URCBBroadCastMsg *ptURCBBroadCastMsg)
{
   XGLOG_FUNCTION_EN_EX("");

   Document doc;
   StringBuffer strBuf;
   Writer<StringBuffer> writer(strBuf);
   Value urConfBridgeVal(kObjectType);
   Document::AllocatorType& allocator = doc.GetAllocator();

   urConfBridgeVal.AddMember("msgUuid", setJsonParam(ptURCBBroadCastMsg->m_strMsgUuid, doc), allocator);
   urConfBridgeVal.AddMember("urconfroom_bcast_event", setJsonParam(ptURCBBroadCastMsg->m_eBroadCastMsgType, doc), allocator);

   Value urConfRoomMsgVal(kObjectType);
   URCBRemoteConfRoomInfo &confRoomInfo = ptURCBBroadCastMsg->m_tURConfRoomInfo;
   urConfRoomMsgVal.AddMember("conf_room_id", setJsonParam(confRoomInfo.m_strConfRoomId, doc), allocator);
   urConfRoomMsgVal.AddMember("conf_bridge_ip", setJsonParam(confRoomInfo.m_strConfBridgeIp, doc), allocator);
   urConfBridgeVal.AddMember("urconfroom_details", urConfRoomMsgVal, allocator);

   urConfBridgeVal.Accept(writer);
   string urConfBcastData("");
   urConfBcastData = strBuf.GetString();

   return urConfBcastData;
}

bool URCBController::decode_conf_bridge_bcast_msg_from_MBC(const char* jsonData,URCBBroadCastMsg *& ptURCBBroadCastMsg)
{
   XGLOG_INFO( "parsing conf bridge broadcast json data: %s", jsonData);
   Document doc;
   ParseResult ok = doc.Parse(jsonData);
   if(!ok) 
   {
      XGLOG_ERROR( "parseJsonMsg failed. Encountered with json parse error !");
      return false;
   } 
   else if(!doc.IsObject()) 
   {
      XGLOG_ERROR( "parseJsonMsg failed. Not able create documnet object !");
      return false;
   }
   if(doc.HasMember("msgUuid")) 
   {
     ptURCBBroadCastMsg->m_strMsgUuid = doc["msgUuid"].GetString();
   } 
   else 
   {
      XGLOG_ERROR("decode_conf_bridge_bcast_msg_from_MBC 'msgUuid' detail is missing in json message !");
      return false;
   }
   if(doc.HasMember("urconfroom_bcast_event")) 
   {
     ptURCBBroadCastMsg->m_eBroadCastMsgType = (URCBBroadCastMsgType) doc["urconfroom_bcast_event"].GetInt();
   } else 
   {
      XGLOG_ERROR("decode_conf_bridge_bcast_msg_from_MBC 'urconfroom_bcast_event' detail is missing in json message !");
      return false;
   }
   if(doc.HasMember("urconfroom_details"))
   {
     const Value &confMeetingInfo = doc["urconfroom_details"];
     if(confMeetingInfo.HasMember("conf_room_id"))
     {
       ptURCBBroadCastMsg->m_tURConfRoomInfo.m_strConfRoomId = confMeetingInfo["conf_room_id"].GetString();
     }
     else
     {
       XGLOG_ERROR("decode_conf_bridge_bcast_msg_from_MBC 'conf_room_id' is missing in json message");
       return false;
     }
     if(confMeetingInfo.HasMember("conf_bridge_ip"))
     {
       ptURCBBroadCastMsg->m_tURConfRoomInfo.m_strConfBridgeIp = confMeetingInfo["conf_bridge_ip"].GetString();
     }
     else
     {
       XGLOG_ERROR("decode_conf_bridge_bcast_msg_from_MBC 'conf_bridge_ip' is missing in json message");
       return false;
     }
   }
   else
   {
     XGLOG_ERROR("readConfEvent Details failed. 'urconfroom_details' is missing in json message");
     return false;
   }

   return true;
}

bool URCBController::post_ccaas_conf_info_to_acd(URCBCallContext * ptCtx)
{
   if (!ptCtx)
   {
      XGLOG_ERROR( "post_ccaas_conf_info_to_acd failed. Call context is NULL");
      return false;
   }
   string message("");
   URCcaasBroadcastMsg * ptURCcaasBcastMsg = fill_ccaas_bcast_msg_info(ptCtx);  
   message = encode_ccaas_conf_info_json_msg(ptURCcaasBcastMsg);
   if (!message.empty())
   {
      XGLOG_INFO( "post_ccaas_conf_info_to_acd posted msg to ACD (%s)", message.c_str());
      //std::string strTopicName = ptCtx->m_strAcdChannel.c_str();
      XGLOG_INFO("==> CONFBRIDGE TO ACD - message:%s, callid:%s", message.c_str(), ptCtx->m_strCallId.c_str());

      RedisInfo redisInfo;
      redisInfo.m_strContextId = ptCtx->m_strCallId.c_str();
      redisInfo.m_enRedisEvent = EN_REDIS_EVENT_PUBLISH_TO_ACD;
      redisInfo.m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_SERVICE_CONTROLLER;
      redisInfo.m_strMsg = message.c_str();
      redisInfo.m_strChannel = ptCtx->m_strChannel.c_str();  
      RedisServiceMsg *redisMsg = new RedisServiceMsg();
      if(!redisMsg)
      {
	 XGLOG_ERROR("post_ccaas_conf_info_to_acd failed - %s", ptCtx->m_strCallId.c_str());
	 return false;
      }
      redisMsg->set_redis_info(redisInfo);
      redisMsg->setDstModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
      redisMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
      redisMsg->setMsgType(EN_XGATE_MSG_REDIS_PUBLISH);
      this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(redisMsg));
   }
   else 
   {
      XGLOG_ERROR( "post_ccaas_conf_info_to_acd failed for msgid:%s", ptCtx->m_strCallId.c_str());
      return false;
   }
   return true;
}

URCcaasBroadcastMsg* URCBController::fill_ccaas_bcast_msg_info(URCBCallContext * ptCtx)
{
   URCcaasBroadcastMsg * ptURCcaasBcastMsg = new URCcaasBroadcastMsg();
   ptURCcaasBcastMsg->m_enModuleType = CCAAS_MESSAGE;
   ptURCcaasBcastMsg->m_enChannelType = CALL_CHANNEL_VIDEO;
   ptURCcaasBcastMsg->m_strUuid = ptCtx->m_strConfRoomId.c_str();
   ptURCcaasBcastMsg->m_enCcaasEvent = ptCtx->getCcaasEvent();
   ptURCcaasBcastMsg->m_strCli = ptCtx->m_strConfUserExtn.c_str();
   ptURCcaasBcastMsg->m_strDdi = ptCtx->m_strConfUserExtn.c_str();
   ptURCcaasBcastMsg->m_strDomainName = ptCtx->m_strDomainName.c_str();
   ptURCcaasBcastMsg->m_strDomainId = ptCtx->m_strDomainId.c_str();
   auto itr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapSubRedisConnInfo.find(EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_ACD);
   if(itr != URENGINE_CONFIG::instance()->m_redisModProfile.m_mapSubRedisConnInfo.end())
	{
		RedisConnInfo * redisConnInfo = itr->second;
		auto it = redisConnInfo->m_vChannel.begin();
		if(it != redisConnInfo->m_vChannel.end())
		{
			ptURCcaasBcastMsg->m_strTopicName = (*it).c_str();
		}
	}
   ptURCcaasBcastMsg->m_strPbxIp = URENGINE_CONFIG::instance()->m_localIp4Addr;
   ptURCcaasBcastMsg->m_strQueueId = ptCtx->m_strQueueId.c_str();
   ptURCcaasBcastMsg->m_strCustomerName = ptCtx->m_strCustomerName.c_str();
   ptURCcaasBcastMsg->m_strCustomerNum = ptCtx->m_strCustomerNum.c_str();
   return ptURCcaasBcastMsg;
}

std::string URCBController::encode_ccaas_conf_info_json_msg(URCcaasBroadcastMsg * ptURCcaasBcastMsg)
{
    Document doc;
    StringBuffer strBuf;
    Writer<StringBuffer> writer(strBuf);
    Value outputVal(kObjectType);
    Document::AllocatorType& allocator = doc.GetAllocator();

    outputVal.AddMember("module_type", setJsonParam(ptURCcaasBcastMsg->m_enModuleType, doc), allocator);
    outputVal.AddMember("channel_type", setJsonParam(ptURCcaasBcastMsg->m_enChannelType, doc), allocator);
    outputVal.AddMember("msg_uuid", setJsonParam(ptURCcaasBcastMsg->m_strUuid, doc), allocator);
    outputVal.AddMember("pbx_msg", setJsonParam(ptURCcaasBcastMsg->m_enCcaasEvent, doc), allocator);
    outputVal.AddMember("cb_msg", setJsonParam(ptURCcaasBcastMsg->m_enCcaasEvent, doc), allocator);
    outputVal.AddMember("cli", setJsonParam(ptURCcaasBcastMsg->m_strCli, doc), allocator);
    outputVal.AddMember("ddi", setJsonParam(ptURCcaasBcastMsg->m_strDdi, doc), allocator);
    outputVal.AddMember("domain_name", setJsonParam(ptURCcaasBcastMsg->m_strDomainName, doc), allocator);
    outputVal.AddMember("domain_id", setJsonParam(ptURCcaasBcastMsg->m_strDomainId, doc), allocator);
    outputVal.AddMember("topic_name", setJsonParam(ptURCcaasBcastMsg->m_strTopicName, doc), allocator);
    outputVal.AddMember("pbx_ip", setJsonParam(ptURCcaasBcastMsg->m_strPbxIp, doc), allocator);
    outputVal.AddMember("queue_id", setJsonParam(ptURCcaasBcastMsg->m_strQueueId, doc), allocator);
    outputVal.AddMember("customer_name", setJsonParam(ptURCcaasBcastMsg->m_strCustomerName, doc), allocator);
    outputVal.AddMember("customer_num", setJsonParam(ptURCcaasBcastMsg->m_strCustomerNum, doc), allocator);

    outputVal.Accept(writer);
    string broadcastJsonData("");
    broadcastJsonData = strBuf.GetString();

    return broadcastJsonData;
}

URCcaasBroadcastMsg * URCBController::decode_ccaas_info_json_message(const char* data, URCcaasBroadcastMsg *& ptURCcaasBcastMsg)
{
   XGLOG_INFO("URCBController::decode_ccaas_info_json_message parsing json data: %s",data);

   Document doc;
   ParseResult ok = doc.Parse(data);
   if(!ok) 
   {
      XGLOG_ERROR( "URCBController::decode_ccaas_info_json_message 'parseJsonMsg' failed !");
      return nullptr;
   } 

   if(doc.HasMember("module_type"))
   {
      ptURCcaasBcastMsg->m_enModuleType = (PBXModuleType)doc["module_type"].GetInt();
      if(ptURCcaasBcastMsg->m_enModuleType != CCAAS_MESSAGE)
      {
	 return nullptr;
      }
   }
   else
   {
      XGLOG_ERROR("URCBController::decode_ccaas_info_json_message failed. 'moduleType' detail is missing in json message !");
   }

   if(doc.HasMember("msg_uuid"))
   {
      ptURCcaasBcastMsg->m_strUuid = doc["msg_uuid"].GetString();
   }
   else
   {
      XGLOG_ERROR("URCBController::decode_ccaas_info_json_message failed. 'msg_uuid' detail is missing in json message !");
   }

   if(doc.HasMember("acd_event"))
   {
      ptURCcaasBcastMsg->m_enCcaasEvent = (CBACDMsg)doc["acd_event"].GetInt();
   }
   else
   {
      XGLOG_ERROR("URCBController::decode_ccaas_info_json_message failed. 'acd_event' detail is missing in json message !");
   }

   if(doc.HasMember("cli"))
   {
      ptURCcaasBcastMsg->m_strCli = doc["cli"].GetString();
   }
   else
   {
      XGLOG_ERROR("URCBController::decode_ccaas_info_json_message failed. 'cli' detail is missing in json message !");
   }

   if(doc.HasMember("ddi"))
   {
      ptURCcaasBcastMsg->m_strDdi = doc["ddi"].GetString();
   }
   if(doc.HasMember("domain_name"))
   {
      ptURCcaasBcastMsg->m_strDomainName = doc["domain_name"].GetString();
   }
   else
   {
      XGLOG_ERROR("URCBController::decode_ccaas_info_json_message failed. 'domain_name' detail is missing in json message !");
   }

   if(doc.HasMember("domain_id"))
   {
      ptURCcaasBcastMsg->m_strDomainId = doc["domain_id"].GetString();
   }
   else
   {
      XGLOG_ERROR("URCBController::decode_ccaas_info_json_message failed. 'domain_id' detail is missing in json message !");
   }

   if(doc.HasMember("queue_id"))
   {
      ptURCcaasBcastMsg->m_strQueueId = doc["queue_id"].GetString();
   }
   else
   {
      XGLOG_ERROR("URCBController::decode_ccaas_info_json_message failed. 'queue_id' detail is missing in json message !");
   }

   if(doc.HasMember("topic_name"))
   {
      ptURCcaasBcastMsg->m_strTopicName = doc["topic_name"].GetString();
   }
   else
   {
      XGLOG_ERROR("URCBController::decode_ccaas_info_json_message failed. 'topic_name' detail is missing in json message !");
   }

   if(doc.HasMember("annc_text"))
   {
      ptURCcaasBcastMsg->m_strAnncText = doc["annc_text"].GetString();
   }
   else
   {
      XGLOG_ERROR("URCBController::decode_ccaas_info_json_message failed. 'play_text' detail is missing in json message !");
   }
   return ptURCcaasBcastMsg;
}

bool URCBController::fillConfUserInfoDetails(const Value &userInfoDetails, URCBSIOUserInfo *ptSioUserInfo)
{
   URCONF_USER_INFO ConfUserInfo;
   if(userInfoDetails.HasMember("user_id"))
   {
      ConfUserInfo.m_strUserId = userInfoDetails["user_id"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillConfUserInfoDetails Details failed. 'user_id' is missing in json message");
      return false;
   }
   if(userInfoDetails.HasMember("sip_id"))
   {
      ConfUserInfo.m_strSipId = userInfoDetails["sip_id"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillConfUserInfoDetails Details failed. 'sip_id' is missing in json message");
      return false;
   }
   if(userInfoDetails.HasMember("connection_id"))
   {
      ConfUserInfo.m_strConnId  = userInfoDetails["connection_id"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillConfUserInfoDetails Details failed. 'connection_id' is missing in json message");
      return false;
   }
   if(userInfoDetails.HasMember("device_id"))
   {
      ConfUserInfo.m_strDeviceId  = userInfoDetails["device_id"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillConfUserInfoDetails Details failed. 'device_id' is missing in json message");
      return false;
   }
   if(userInfoDetails.HasMember("browser_type"))
   {
      ConfUserInfo.m_strBrowserType  = userInfoDetails["browser_type"].GetString();
   }
   if(userInfoDetails.HasMember("user_type"))
   {
      ConfUserInfo.m_strUserType  = userInfoDetails["user_type"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillConfUserInfoDetails Details failed. 'user_type' is missing in json message");
      return false;
   }
   if(userInfoDetails.HasMember("plan_type"))
   {
      ConfUserInfo.m_iPlanType  = userInfoDetails["plan_type"].GetInt();
   }
   else
   {
      XGLOG_ERROR("fillConfUserInfoDetails Details failed. 'plan_type' is missing in json message");
   }
   if(userInfoDetails.HasMember("rec_file_path"))
   {
      ConfUserInfo.m_strRecFilePath  = userInfoDetails["rec_file_path"].GetString();
   }
   if(userInfoDetails.HasMember("rec_url"))
   {
      ConfUserInfo.m_strRecUrl  = userInfoDetails["rec_url"].GetString();
   }
   ptSioUserInfo->m_tURConfUserInfoList.insert({ConfUserInfo.m_strConnId,ConfUserInfo});

   return true;
}

bool URCBController::fillConfParticipantInfoDetails(const Value &participantInfoDetails, URCBSIOUserInfo *ptSioUserInfo)
{
   URCONF_PARTICIPANT_INFO ConfParticipantInfo;
   if(participantInfoDetails.HasMember("conId"))
   {
      ConfParticipantInfo.m_strConnId  = participantInfoDetails["conId"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillConfparticipantInfoDetails Details failed. 'connection_id' is missing in json message");
      return false;
   }
   if(participantInfoDetails.HasMember("user_id"))
   {
      ConfParticipantInfo.m_strUserId = participantInfoDetails["user_id"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillConfparticipantInfoDetails Details failed. 'user_id' is missing in json message");
   }
   if(participantInfoDetails.HasMember("ext"))
   {
      ConfParticipantInfo.m_strUserId = participantInfoDetails["ext"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillConfparticipantInfoDetails Details failed. 'ext' is missing in json message");
   }
   if(participantInfoDetails.HasMember("user_type"))
   {
      ConfParticipantInfo.m_strUserType  = participantInfoDetails["user_type"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillConfparticipantInfoDetails Details failed. 'user_type' is missing in json message");
      return false;
   }
   ptSioUserInfo->m_dequeURConfParticipantList.push_back(ConfParticipantInfo);

   return true;
}

bool URCBController::fillURMeetUserInfo(const Value &userInfoDetails, URCBSIOUserInfo *ptSioUserInfo)
{
   URMEET_USER_INFO meetUserInfo;
   if(userInfoDetails.HasMember("email"))
   {
      meetUserInfo.m_strEmail  = userInfoDetails["email"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillURMeetUserInfo Details failed. 'email' is missing in json message");
      return false;
   }
   if(userInfoDetails.HasMember("profileImg"))
   {
      meetUserInfo.m_strProfileImg  = userInfoDetails["profileImg"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillURMeetUserInfo Details failed. 'profileImg' is missing in json message");
      return false;
   }
   if(userInfoDetails.HasMember("sipId"))
   {
      meetUserInfo.m_strSipId  = userInfoDetails["sipId"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillURMeetUserInfo Details failed. 'sipId' is missing in json message");
      return false;
   }
   if(userInfoDetails.HasMember("name"))
   {
      meetUserInfo.m_strName  = userInfoDetails["name"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillURMeetUserInfo Details failed. 'name' is missing in json message");
      return false;
   }
   if(userInfoDetails.HasMember("ext"))
   {
      meetUserInfo.m_strExtn  = userInfoDetails["ext"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillURMeetUserInfo Details failed. 'ext' is missing in json message");
      return false;
   }
   if(userInfoDetails.HasMember("roletype"))
   {
      meetUserInfo.m_iRoleType  = userInfoDetails["roletype"].GetInt();
   }
   else
   {
      XGLOG_ERROR("fillURMeetUserInfo Details failed. 'roletype' is missing in json message");
      return false;
   }
   if(userInfoDetails.HasMember("user_type"))
   {
      meetUserInfo.m_strUserType  = userInfoDetails["user_type"].GetString();
   }
   else
   {
      XGLOG_ERROR("fillURMeetUserInfo Details failed. 'user_type' is missing in json message");
      return false;
   }
   ptSioUserInfo->m_dequeURMeetUserList.push_back(meetUserInfo);

   return true;
}

bool URCBController::handleModuleCallbackMsg(IURModuleMsg * pctrlMsg)
{
   XGLOG_FUNCTION_EN_EX("");
   if (pctrlMsg) 
   {
      XGLOG_INFO("handleModuleCallbackMsg Message Received!");
      if (IURDefines::UR_MODULE_SERVICE_CONTROLLER == pctrlMsg->getDstModuleId())
      {
	      URCBCONTROLLER->putq( dynamic_cast<ACE_Message_Block*>(pctrlMsg));
      }
      else if (arrayModules[pctrlMsg->getDstModuleId()])
      {
	      arrayModules[pctrlMsg->getDstModuleId()]->pushModuleMsg(pctrlMsg);
      }
      else
      {
	      XGLOG_ERROR("handleModuleCallbackMsg failed, destination Unknown!");
      }
   }

   return true;
}

bool URCBController::getURConfInfoFromUnknownHeaderList(URCBCallContext * ptCtx, xGateSIPMsg *ptSipmsg)
{
   XGLOG_FUNCTION_EN_EX("");
   std::map <std::string,std::string> tURConfInfoList = ptSipmsg->getUnKnownHeaderInfo();
   std::map <std::string,std::string>::iterator itr = tURConfInfoList.begin();
   for(itr; itr != tURConfInfoList.end(); itr++)
   {
      if(!strcmp(itr->first.c_str(),"UR-AV-ConferenceRoom-ID"))
      {
	      ptCtx->m_strConfRoomId = itr->second;
      }
      else if(!strcmp(itr->first.c_str(),"UR-AV-Conference-UserType"))
      {
         if(!strcmp(itr->second.c_str(),"1"))
         {       
            ptCtx->m_enumConfUserType = CONF_USER_TYPE_HOST;
         } 
         else if(!strcmp(itr->second.c_str(),"4"))
         {
            ptCtx->m_enumConfUserType = CONF_USER_TYPE_SCREENSHARE;
         }  
         else if(!strcmp(itr->second.c_str(),"5"))
         {
            ptCtx->m_enumConfUserType = CONF_USER_TYPE_CUSTOMER;
         }  
         else if(!strcmp(itr->second.c_str(),"6"))
         {
            ptCtx->m_enumConfUserType = CONF_USER_TYPE_AGENT;
         }  
         else 
         {
            ptCtx->m_enumConfUserType = CONF_USER_TYPE_PARTICIPANT;
         }
      }
      else if(!strcmp(itr->first.c_str(),"UR-AV-ConferenceConn-ID"))
      {
	      ptCtx->m_strConfConnId = itr->second;
      }
      else if(!strcmp(itr->first.c_str(),"UR-AV-AOR"))
      {
         ptCtx->m_strAor = itr->second;
         if(!fillAorDetails(ptCtx))
         {
            XGLOG_ERROR("getURConfInfoFromUnknownHeaderList failed. fillAorDetails failed for callid:%s!", ptCtx->m_strCallId.c_str());
            return false;
         }
      }
   }

   if(ptCtx->m_enumConfUserType == CONF_USER_TYPE_SCREENSHARE)
   {
      std::string strConfConnId = ptCtx->m_strConfConnId.c_str();
      std::string delimiter = "_screenshare";
      ptCtx->m_strSSUserConnId = strConfConnId.substr(0, strConfConnId.find(delimiter));
      m_tURConfRoomCtxHndlr.get_callid_from_connid(ptCtx->m_strConfRoomId, ptCtx->m_strSSUserConnId, ptCtx->m_strSSUserCallId);
   }
   
   if(ptCtx->m_strConfRoomId.empty() || ptCtx->m_strConfConnId.empty())
   {
      XGLOG_ERROR("getURConfInfoFromUnknownHeaderList failed. ConfRoom-Id/ConfConn-Id/ConfAor is missing");
      return false;
   }

   return true;
}

bool URCBController::getConfSIOMsgType(string &strEventName,URConfSIOMsgType &msgType)
{
   if(strncmp(strEventName.c_str(),"host_joining",strEventName.length()) == 0 )
   {
      msgType = URCONF_SIO_MSG_HOST_JOINING;
   }
   else if (strncmp(strEventName.c_str() ,"participant_joining", strEventName.length()) == 0)
   {
      msgType =  URCONF_SIO_MSG_PARTICIPANT_JOINING;
   }
   if(strncmp(strEventName.c_str(),"customer_joining",strEventName.length()) == 0 )
   {
      msgType = URCONF_SIO_MSG_CUSTOMER_JOINING;
   }
   if(strncmp(strEventName.c_str(),"agent_joining",strEventName.length()) == 0 )
   {
      msgType = URCONF_SIO_MSG_AGENT_JOINING;
   }
   else if (strncmp(strEventName.c_str() ,"tile_change_request", strEventName.length()) == 0)
   {
      msgType =  URCONF_SIO_MSG_TILE_CHANGE_REQUEST;
   }
   else if (strncmp(strEventName.c_str() ,"sdp_update_request", strEventName.length()) == 0)
   {
      msgType =  URCONF_SIO_MSG_SDP_UPDATE_REQUEST;
   }
   else if (strncmp(strEventName.c_str() ,"screenshare_start", strEventName.length()) == 0)
   {
      msgType =  URCONF_SIO_MSG_SCREENSHARE_START;
   }
   else if (strncmp(strEventName.c_str() ,"screenshare_stop", strEventName.length()) == 0)
   {
      msgType =  URCONF_SIO_MSG_SCREENSHARE_STOP;
   }
   else if (strncmp(strEventName.c_str() ,"screenshare_video_req", strEventName.length()) == 0)
   {
      msgType =  URCONF_SIO_MSG_SCREENSHARE_VIDEO_REQUEST;
   }
   else if (strncmp(strEventName.c_str() ,"active_speaker_video_req", strEventName.length()) == 0)
   {
      msgType =  URCONF_SIO_MSG_ACTIVE_SPEAKER_VIDEO_REQUEST;
   }
   else if (strncmp(strEventName.c_str() ,"active_speaker_info", strEventName.length()) == 0)
   {
      msgType =  URCONF_SIO_MSG_ACTIVE_SPEAKER_INFO;
   }
   else if (strncmp(strEventName.c_str() ,"initial_speaker_info", strEventName.length()) == 0){
      msgType =  URCONF_SIO_MSG_INITIAL_SPEAKER_INFO;
   }
   else if (strncmp(strEventName.c_str() ,"switch_to_grid_view", strEventName.length()) == 0){
      msgType =  URCONF_SIO_MSG_GRID_VIEW_VIDEO_REQUEST;
   }
   else if (strncmp(strEventName.c_str() ,"recording_start", strEventName.length()) == 0)
   {
      msgType =  URCONF_SIO_MSG_RECORDING_START;
   }
   else if (strncmp(strEventName.c_str() ,"recording_pause", strEventName.length()) == 0)
   {
      msgType =  URCONF_SIO_MSG_RECORDING_PAUSE;
   }
   else if (strncmp(strEventName.c_str() ,"recording_resume", strEventName.length()) == 0)
   {
      msgType =  URCONF_SIO_MSG_RECORDING_RESUME;
   }
   else if (strncmp(strEventName.c_str() ,"recording_stop", strEventName.length()) == 0)
   {
      msgType =  URCONF_SIO_MSG_RECORDING_STOP;
   }
   else if (strncmp(strEventName.c_str(),"user_action_disconnected",strEventName.length()) == 0) 
   {
      msgType =  URCONF_SIO_MSG_USER_ACTION_DISCONNECTED;
   }
   else if (strncmp(strEventName.c_str(),"participant_remove",strEventName.length()) == 0) 
   {
      msgType =  URCONF_SIO_MSG_PARTICIPANT_REMOVE;
   }
   else if (strncmp(strEventName.c_str(),"user_action_terminated",strEventName.length()) == 0) 
   {
      msgType =  URCONF_SIO_MSG_USER_ACTION_TERMINATED;
   }
   else if (strncmp(strEventName.c_str(),"end_meeting_all",strEventName.length()) == 0) 
   {
      msgType =  URCONF_SIO_MSG_END_MEETING_ALL;
   }

   return true;
}

bool URCBController::fillAorDetails(URCBCallContext * ptCtx)
{
   XGLOG_FUNCTION_EN_EX("for callid - %s !", ptCtx->m_strCallId.c_str());

   std::vector<std::string> strProxyUN;
   splitValues(strProxyUN, ptCtx->m_strAor, "@");
   if(strProxyUN.size() > 0)
   { 
      std::vector<std::string> strContactAddr;
      std::vector<std::string>::iterator it = strProxyUN.begin();
      while(it != strProxyUN.end())
      {
         if( std::string::npos != (*it).find(";"))
         {
            splitValues(strContactAddr,(*it).c_str(),";");
            break;
         }
         it++;
      }
      if(strContactAddr.size() < 3)
      {
         return false;
      } 
      else 
      {
         ptCtx->m_tUserAor.m_contactAddr = strContactAddr[0];
         ptCtx->m_tUserAor.m_ipAddr 	 = strContactAddr[1];
         ptCtx->m_tUserAor.m_sDeviceType = strContactAddr[2];
      }
      ptCtx->m_tUserAor.m_proxyUserName  = strProxyUN[0];
   }

   return true;
}

void URCBController::splitValues(std::vector<string> &Values, std::string strValue, std::string delim)
{
   size_t start = 0;
   size_t end = strValue.find(delim);
   while (end != std::string::npos)
   {
      Values.push_back(strValue.substr(start, end - start));
      start = end + delim.length();
      end = strValue.find(delim, start);
   }
   Values.push_back(strValue.substr(start, end));
   return;
}

std::string URCBController::getCurrentUTCTime()
{
   char buf[30]="";
   time_t lt = time(NULL);
   auto local_time = *gmtime(&lt);
   auto local = mktime(&local_time);

   strftime(buf, sizeof(buf), "%F %T %Z", &local_time);
   auto utc_time = *gmtime(&local);
   strftime(buf, sizeof(buf), "%FT%TZ", &utc_time);

   return string(buf);
}

bool URCBController::set_timer(string callId, URCBTimerData *& pturcbTimerData, SchedulerType schedulerType, int timeOut)
{
   XGLOG_FUNCTION_EN_EX("==> Set timer_type:%d for callid - %s !", schedulerType, callId.c_str());

   xGateSchedulerServiceMsg *pSchedulerMsg = new xGateSchedulerServiceMsg();
   if(!pSchedulerMsg)
   {
      XGLOG_ERROR("URCBController::set_timer failed to create xGateSchedulerServiceMsg!");
      return false;
   }
   SchedulerInfo schedulerInfo;
   schedulerInfo.m_contextId = callId;
   schedulerInfo.m_schedulerType = schedulerType;
   schedulerInfo.m_retryCount = 0;
   schedulerInfo.m_timeoutSecond = timeOut;
   schedulerInfo.m_pCtxdata = (void*) pturcbTimerData; 
   schedulerInfo.m_requestorModule = IURDefines::UR_MODULE_SERVICE_CONTROLLER;
   schedulerInfo.m_enumTimerFunc = SCHEDULER_FUNCTION_SET_TIMER;
   pSchedulerMsg->setUid(callId);
   pSchedulerMsg->setDstModuleId(IURDefines::UR_MODULE_SCHEDULER_SERVICE);
   pSchedulerMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
   pSchedulerMsg->set_scheduler_info(schedulerInfo);
   this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(pSchedulerMsg));

   return true;
}

bool URCBController::cancel_timer(string callId, SchedulerType schedulerType)
{
   XGLOG_FUNCTION_EN_EX("==> Cancel timer_type:%d for callid - %s !", schedulerType, callId.c_str());

   xGateSchedulerServiceMsg *pSchedulerMsg = new xGateSchedulerServiceMsg();
   if(!pSchedulerMsg)
   {
      XGLOG_ERROR("URCBController::cancel_timer failed to create xGateSchedulerServiceMsg!");
      return false;
   }
   SchedulerInfo schedulerInfo;
   schedulerInfo.m_contextId = callId;
   schedulerInfo.m_schedulerType = schedulerType;
   schedulerInfo.m_requestorModule = IURDefines::UR_MODULE_SERVICE_CONTROLLER;
   schedulerInfo.m_enumTimerFunc = SCHEDULER_FUNCTION_CANCEL_TIMER;
   pSchedulerMsg->setUid(callId);
   pSchedulerMsg->setDstModuleId(IURDefines::UR_MODULE_SCHEDULER_SERVICE);
   pSchedulerMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
   pSchedulerMsg->set_scheduler_info(schedulerInfo);
   this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(pSchedulerMsg));

   return true;
}

bool URCBController::post_msg_to_confbridge(string info)
{
   XGLOG_INFO("==> URCBBroadcastMsg message:%s", info.c_str());

   RedisInfo redisInfo;
   redisInfo.m_enRedisEvent = EN_REDIS_EVENT_PUBLISH_TO_CONFBRIDGE;
   redisInfo.m_enReqModule = IURDefines::MODULE_ID::UR_MODULE_SERVICE_CONTROLLER;
   redisInfo.m_strMsg = info.c_str();
   RedisServiceMsg *redisMsg = new RedisServiceMsg();
   if(!redisMsg)
   {
      XGLOG_ERROR("post_msg_to_confbridge message failed");
      return false;
   }
   redisMsg->set_redis_info(redisInfo);
   redisMsg->setMsgType(EN_XGATE_MSG_REDIS_PUBLISH);
   redisMsg->setDstModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
   redisMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
   this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(redisMsg));

   return true;
}

std::string URCBController::generateTransid()
{
   uuid_t id;
   size_t len;

   uuid_generate(id);

   char _string[100];
   memset(_string, 0, sizeof(_string));
   uuid_unparse(id, _string);
   len = strlen(_string);

   return UUIDToSessionId((char *)_string, len);
}

std::string URCBController::UUIDToSessionId(const char *uuid, const size_t len)
{
   char session_uuid[33] = {0}; /*"local-uuid", per rfc7989*/
   if ((!uuid) || len != 36)
      return nullptr;

   for (size_t i = 0, j = 0; i < len; i++)
   {
      if (uuid[i] != '-')
	 session_uuid[j++] = uuid[i];
   }
   session_uuid[len + 1] = '\0';

   if (!isValidSessionID(session_uuid, strlen(session_uuid)))
      return nullptr;
   std::string s(session_uuid);

   return s;
}

bool URCBController::isValidSessionID(const char *uuid, const size_t len)
{
   for (size_t i = 0; i < len; i++)
   {
      if ((uuid[i] < '0' || uuid[i] > '9') && (uuid[i] < 'a' || uuid[i] > 'f'))
	 return false;
   }
   return true;
}

Value URCBController::setJsonParam(string &strValue,Document &doc )
{
   Value retVal;
   retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
   return retVal;
}

Value URCBController::setJsonParam(unsigned int iValue, Document &doc)
{
   Value retVal;
   retVal = iValue;
   return retVal;
}

bool URCBController::handleMbcConnnectedMsg(xGateBaseMsg *pMsg)
{
  XGLOG_FUNCTION_EN_EX("");
  if (!pMsg)
  {
    XGLOG_ERROR("URCBController::%s() pMsg is empty !!",__func__);
    return false;
  }
  xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = dynamic_cast<xGateMBCLinkServiceMsg *>(pMsg);

  xGateNetConnection netConnInfo = pMBCLinkServiceMsg->get_netConnInfo();
  TcpConTuple conTuple;

  conTuple.m_fd = netConnInfo.m_fd;
  conTuple.m_remoteIp = netConnInfo.m_recvIp;
  conTuple.m_remotePort = netConnInfo.m_recvPort;
  XGLOG_INFO("URCBController::%s() received connection inserted into m_mbcConList success for IP:(%s) FD:(%d) Port:(%d)",__func__,\
	conTuple.m_remoteIp.c_str(), conTuple.m_fd, conTuple.m_remotePort);
	m_mbcConList.push_back(conTuple);
  return true;
}

bool URCBController::handleMbcConClosedMsg(xGateBaseMsg *pMsg)
{
  XGLOG_FUNCTION_EN_EX("");
  if (!pMsg)
  {
    XGLOG_ERROR("URCBController::%s() pMsg is empty !!",__func__);
    return false;
  }
  xGateMBCLinkServiceMsg *pMBCLinkServiceMsg = dynamic_cast<xGateMBCLinkServiceMsg *>(pMsg);
  xGateNetConnection netConnInfo = pMBCLinkServiceMsg->get_netConnInfo();
  unsigned short fd = netConnInfo.m_fd;
  std::string strIp = netConnInfo.m_recvIp;

  for (auto it = m_mbcConList.begin(); it != m_mbcConList.end(); it++)
  {
    if (it->m_remoteIp == strIp && it->m_fd == fd)
    {
			XGLOG_INFO("URCBController::%s() removed IP:(%s) FD:(%d) and Port:(%d) from connection map",__func__,\
					strIp.c_str(), fd, netConnInfo.m_recvPort);
      m_mbcConList.erase(it);
      return true;
    }
  }
	XGLOG_DEBUG("URCBController::%s() entry not found in m_mbcConList for IP:(%s), FD(%d) and Port:(%d)",__func__,\
			strIp.c_str(),fd,netConnInfo.m_recvPort);
  return false;
}

//To Choose Mbc Ip by Round robin method
std::string URCBController::chooseMbcIp(std::string strConfRoomId)
{
  XGLOG_FUNCTION_EN_EX("");
  URConfRoom * urConfRoom = NULL;
  if(m_tURConfRoomCtxHndlr.getConfRoomCallContext(strConfRoomId.c_str(), urConfRoom))
  {
    if(!urConfRoom->m_strMbcIp.empty())
    {
      XGLOG_INFO("URCBController::%s() Mbc IP:(%s) found for conf-room-id:(%s)",__func__,urConfRoom->m_strMbcIp.c_str(),strConfRoomId.c_str());
      return urConfRoom->m_strMbcIp;
    }
    else
    {
      auto it = m_mbcConList.begin();
      std::string ip = urConfRoom->m_strMbcIp = it->m_remoteIp;
      std::shuffle(std::begin(m_mbcConList), std::end(m_mbcConList), std::default_random_engine());
      XGLOG_INFO("URCBController::%s() Choosing Mbc Ip :(%s) by Round robin for conf-room-id:(%s)",__func__,ip.c_str(),strConfRoomId.c_str());
      return ip;
    }
  }
  XGLOG_DEBUG("URCBController::%s() entry not found in Conf Room Call Context for id (%s)",__func__,strConfRoomId.c_str());
  return "";
}
