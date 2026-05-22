#include "xglog.h"
#include "xGateRedisService.h"

xGateRedisService::xGateRedisService() :
   ACE_Task<ACE_MT_SYNCH>(),
   m_run(false), m_redisConfig(NULL)
{
   m_strModuleName = "RedisServiceModule";
}

xGateRedisService::~xGateRedisService(void)
{
}

int xGateRedisService::initModule(IURModuleConfig * predisConfig)
{
   XGLOG_INFO("xGateRedisService initModule initialized !");  
   m_redisConfig = predisConfig;

   if(!init()) 
   {
      return IURDefines::UR_RETURN_ERROR;
   }
   return IURDefines::UR_RETURN_SUCCESS;
}

int xGateRedisService::unitModule()
{
   stop();
   return IURDefines::UR_RETURN_SUCCESS;
}

void xGateRedisService::getModuleName(std::string& moduleName){
   moduleName=m_strModuleName;
}

IURDefines::MODULE_ID xGateRedisService::getModuleID(){
   return IURDefines::UR_MODULE_REDIS_SERVICE;
}

IURModuleConfig * xGateRedisService::getModuleConfig(){
   return m_redisConfig;
}

bool xGateRedisService::stop(void)
{
   m_run = false;
   msg_queue()->deactivate();
   wait();
   XGLOG_INFO("xGateRedisService stopped !");
   return true;
}

bool xGateRedisService::init()
{
   XGLOG_INFO("xGateRedisService::init() called");
   m_run = true;
   // Start the log thread.
   if(activate(THR_NEW_LWP, 1) == -1 ) 
   {
      XGLOG_ERROR("xGateRedisService::init() failed to start RegService thread !");
      return false;
   }
   else 
   {
      XGLOG_INFO("xGateRedisService::init() success !");
   }
   try
   {
      initRedisHandler();
   }
   catch (std::exception& e)
   {
      XGLOG_ERROR("xGateRedisService::init() Exception caught - %s", e.what());
   }
   return true;
}

int xGateRedisService::pushModuleMsg(IURModuleMsg * predisMsg)
{
   XGLOG_INFO("xGateRedisService::inside pushModuleMsg");
   if (!predisMsg) 
   {
      XGLOG_ERROR("pushModuleMsg::Message not received from controller!");
      return IURDefines::UR_RETURN_ERROR;
   }
   else if(IURDefines::UR_MODULE_REDIS_SERVICE == predisMsg->getDstModuleId()) 
   {
      XGLOG_DEBUG("xGateRedisService::Message received from controller to regservice");
      REDISSERVICE->putq(dynamic_cast<ACE_Message_Block*>(predisMsg));
   }
   return IURDefines::UR_RETURN_SUCCESS;
}

int xGateRedisService::svc(void)
{
   int rc;
   rc = pthread_setname_np(ACE_Thread::self(), "RedisSrvc");
   if(rc!=0)
   {
     printf("Failed to set name for redis service thread");
   }
   ACE_Message_Block *pAmb = 0;
   while(m_run)
   {
      ACE_Time_Value tv ((time(NULL) + 20));
      int result = getq(pAmb, &tv);
      if ((result == -1) && (errno == EWOULDBLOCK))
      {
	      continue;
      }
      if ((result != -1))
      {
         XGLOG_INFO("xGateRedisService::process handle_msg success !");
         handle_msg(pAmb);
      }
      else
      {
         XGLOG_ERROR("xGateRedisService::svc Error in Message Reg");
         m_run = false;
      }
      // delete the message received
      if(pAmb)
      {
         delete pAmb;
         pAmb = NULL;
      }
   }

   XGLOG_INFO("xGateRedisService::svc() stopped");
   return 0;
}

bool xGateRedisService::initRedisHandler()
{
   std::map<EN_REDIS_MSG_TYPE, RedisConnInfo*>::iterator socketItr;
   socketItr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapSubRedisConnInfo.begin();
   for(socketItr; socketItr != URENGINE_CONFIG::instance()->m_redisModProfile.m_mapSubRedisConnInfo.end(); socketItr++)
   {
      xGateRedisHandler *redisHandler = nullptr;
      RedisConnInfo *& redisConnInfo = socketItr->second;
      if(redisConnInfo)
      {
	 std::string strRedisIp = redisConnInfo->m_strServerHost.c_str();
	 std::string strChannel = redisConnInfo->m_strChannel.c_str();
	 IT_REDIS_HANDLER_MAP itr = m_redisHandlerMap.find(strRedisIp);
	 if(itr == m_redisHandlerMap.end())
	 {
	    redisHandler = new xGateRedisHandler(redisConnInfo);
	    m_redisHandlerMap.insert(REDIS_HANDLER_MAP::value_type(strRedisIp, redisHandler));
	 }
	 else
	 {
	    redisHandler = itr->second;
	 }
	 if(redisHandler)
	 {
	    for(int i = 0; i < redisConnInfo->m_vChannel.size(); i++)
	    {
	       strChannel = redisConnInfo->m_vChannel[i];
	       redisHandler->m_vsubChannel.push_back(strChannel.c_str());
	    }
	 }
      }
   }
   socketItr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapPubRedisConnInfo.begin();
   for(socketItr; socketItr != URENGINE_CONFIG::instance()->m_redisModProfile.m_mapPubRedisConnInfo.end(); socketItr++)
   {
      xGateRedisHandler *redisHandler = nullptr;
      RedisConnInfo *& redisConnInfo = socketItr->second;
      if(redisConnInfo)
      {
	 std::string strRedisIp = redisConnInfo->m_strServerHost.c_str();
	 IT_REDIS_HANDLER_MAP itr = m_redisHandlerMap.find(strRedisIp);
	 if(itr == m_redisHandlerMap.end())
	 {
	    redisHandler = new xGateRedisHandler(redisConnInfo);
	    m_redisHandlerMap.insert(REDIS_HANDLER_MAP::value_type(strRedisIp, redisHandler));
	 }
      }
   } 
   auto getItr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapGetRedisConnInfo.begin();
   for(getItr; getItr != URENGINE_CONFIG::instance()->m_redisModProfile.m_mapGetRedisConnInfo.end(); getItr++)
   {
      xGateRedisHandler *redisHandler = nullptr;
      RedisConnInfo *& redisConnInfo = getItr->second;
      if(redisConnInfo)
      {
	 std::string strRedisIp = redisConnInfo->m_strServerHost.c_str();
	 IT_REDIS_HANDLER_MAP itr = m_redisHandlerMap.find(strRedisIp);
	 if(itr == m_redisHandlerMap.end())
	 {
	    redisHandler = new xGateRedisHandler(redisConnInfo);
	    m_redisHandlerMap.insert(REDIS_HANDLER_MAP::value_type(strRedisIp, redisHandler));
	 }
      }
   } 
   IT_REDIS_HANDLER_MAP it = m_redisHandlerMap.begin();
   for(it; it != m_redisHandlerMap.end(); it++) 
   {
      xGateRedisHandler *redisHandler = it->second;
      redisHandler->init();
   }
   return true;
}

bool xGateRedisService::handle_msg(ACE_Message_Block *pAmb)
{
   XGLOG_DEBUG("xGateRedisService::handle_msg called");
   if(!pAmb)
   {
      XGLOG_ERROR("handle_msg failed. ACE_Message_Block is NULL !");
      return false;
   }
   RedisServiceMsg *pMsg = dynamic_cast<RedisServiceMsg*>(pAmb);
   if(!pMsg) 
   {
      XGLOG_ERROR("handle_msg failed. RedisServiceMsg pointer is NULL !");
      return false;
   }
   switch(pMsg->getMsgType())
   {
      case EN_XGATE_MSG_REDIS_CONN_FAILED_NOTIFY:
      {
         XGLOG_INFO("handle_msg REDIS_CONN_FAILED_NOTIFY from redis module");

				 if(pMsg->getSrcModuleId() == IURDefines::UR_MODULE_REDIS_SERVICE)
				 {
					 XGLOG_INFO("REDIS_CONN_FAILED_NOTIFY srcmodule is redis handler");
				 }
				 if(pMsg->getDstModuleId() == IURDefines::UR_MODULE_REDIS_SERVICE)
				 {
					 XGLOG_INFO("REDIS_CONN_FAILED_NOTIFY dstmodule is redis handler");
				 }

				 std::string strRedisIp = pMsg->get_redis_info().m_strRedisIp.c_str();
				 handle_redis_conn_failed_notify_req(strRedisIp.c_str());
				 return true;
      }
      case EN_XGATE_MSG_REDIS_PUBLISH:
      {
				 XGLOG_INFO("handle_msg REDIS_PUBLISH from controller module");
				 xGateRedisHandler * redisHandler = nullptr;
				 RedisInfo &redisInfo = pMsg->get_redis_info();
				 RedisServiceEvent redisEvent = redisInfo.m_enRedisEvent;
				 std::map<EN_REDIS_MSG_TYPE, RedisConnInfo*>::iterator itr;
				 switch(redisEvent)
				 {
						case EN_REDIS_EVENT_PUBLISH_TO_UCAAS_SIO:
							 {
									XGLOG_INFO("handle_msg CONFBRIDGE_TO_UCAAS_SIO");
									itr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapPubRedisConnInfo.find(EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_UCAAS_SIO);
									break;
							 }
						case EN_REDIS_EVENT_PUBLISH_TO_CCAAS_SIO:
							 {
									XGLOG_INFO("handle_msg CONFBRIDGE_TO_CCAAS_SIO");
									itr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapPubRedisConnInfo.find(EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_CCAAS_SIO);
									break;
							 }
						case EN_REDIS_EVENT_PUBLISH_TO_CONFBRIDGE:
							 {
									XGLOG_INFO("handle_msg PUBLISH_TO_CONFBRIDGE");
									itr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapPubRedisConnInfo.find(EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_CONFBRIDGE);
									break;
							 }
						case EN_REDIS_EVENT_PUBLISH_TO_ACD:
							 {
									XGLOG_INFO("handle_msg CONFBRIDGE_TO_ACD");
									itr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapPubRedisConnInfo.find(EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_ACD);
									break;
							 }
						default:
							 {
									XGLOG_ERROR("handle_msg REDIS_PUBLISH event failed. Unknown redis event !");
									return true;
							 }
				 }//switch

				 if(itr != URENGINE_CONFIG::instance()->m_redisModProfile.m_mapPubRedisConnInfo.end())
				 {
						RedisConnInfo * redisConnInfo = itr->second;
						std::string strRedisIp = redisConnInfo->m_strServerHost.c_str();
						if(redisEvent != EN_REDIS_EVENT_PUBLISH_TO_ACD)
						{
							 redisInfo.m_strChannel = redisConnInfo->m_strChannel.c_str();
						}
						redisInfo.m_enRedisMethod = redisConnInfo->m_enRedisMethod;
						if(!strRedisIp.empty())
						{
							 IT_REDIS_HANDLER_MAP it = m_redisHandlerMap.find(strRedisIp);
							 if(it != m_redisHandlerMap.end())
							 {
									redisHandler = it->second;
							 }
						}
				 }
				 else
				 {
						XGLOG_ERROR("handle_msg REDIS_PUBLISH event failed. Not found redis_conn_info !");
						return false;
				 }
				 if(redisHandler)
				 {
						redisHandler->process_msg(pMsg);
				 }
				 break;
      }
      case EN_XGATE_MSG_REDIS_GET:
      {
				 XGLOG_INFO("handle_msg REDIS_GET from controller module");
				 xGateRedisHandler * redisHandler = nullptr;
				 RedisInfo &redisInfo = pMsg->get_redis_info();
				 RedisServiceEvent redisEvent = redisInfo.m_enRedisEvent;
				 std::map<EN_REDIS_GET_TYPE, RedisConnInfo*>::iterator itr;
				 switch(redisEvent)
				 {
						case EN_REDIS_EVENT_GET_ACD_CHANNEL:
							 {
									XGLOG_INFO("handle_msg REDIS_EVENT_GET_ACD_CHANNEL");
									itr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapGetRedisConnInfo.find(EN_REDIS_GET_TYPE::EN_REDIS_GET_TYPE_ACD_CHANNEL);
									break;
							 }
						case EN_REDIS_EVENT_GET_MEETING_DURATION:
							 {
									XGLOG_INFO("handle_msg REDIS_EVENT_GET_MEETING_DURATION");
									itr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapGetRedisConnInfo.find(EN_REDIS_GET_TYPE::EN_REDIS_GET_TYPE_MEETING_DURATION);
									break;
							 }
						default:
							 {

									XGLOG_ERROR("handle_msg REDIS_GET event failed. Unknown redis event !");
									return true;
							 }
				 }//switch
				 if(itr != URENGINE_CONFIG::instance()->m_redisModProfile.m_mapGetRedisConnInfo.end())
				 {
						RedisConnInfo * redisConnInfo = itr->second;
						std::string strRedisIp = redisConnInfo->m_strServerHost.c_str();
						if(!strRedisIp.empty())
						{
							 IT_REDIS_HANDLER_MAP it = m_redisHandlerMap.find(strRedisIp);
							 if(it != m_redisHandlerMap.end())
							 {
									redisHandler = it->second;
							 }
						}
				 }
				 else
				 {
						XGLOG_ERROR("handle_msg REDIS_GET event failed. Not found redis_conn_info !");
						return false;
				 }
				 if(redisHandler)
				 {
						redisHandler->process_msg(pMsg);
				 }
				 break;
      }
      case EN_XGATE_MSG_REDIS_SET:
      {
				 XGLOG_INFO("handle_msg REDIS_SET from controller module");
				 xGateRedisHandler * redisHandler = nullptr;
				 RedisInfo &redisInfo = pMsg->get_redis_info();
				 RedisServiceEvent redisEvent = redisInfo.m_enRedisEvent;
				 std::map<EN_REDIS_GET_TYPE, RedisConnInfo*>::iterator itr;
				 switch(redisEvent)
				 {
						case EN_REDIS_EVENT_SET_ACD_CHANNEL:
							 {
									XGLOG_INFO("handle_msg REDIS_EVENT_SET_ACD_CHANNEL");
									itr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapGetRedisConnInfo.find(EN_REDIS_GET_TYPE::EN_REDIS_GET_TYPE_ACD_CHANNEL);
									break;
							 }
						case EN_REDIS_EVENT_SET_MEETING_DURATION:
							 {
									XGLOG_INFO("handle_msg REDIS_EVENT_SET_MEETING_DURATION");
									itr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapGetRedisConnInfo.find(EN_REDIS_GET_TYPE::EN_REDIS_GET_TYPE_MEETING_DURATION);
									break;
							 }
						default:
							 {
									XGLOG_ERROR("handle_msg REDIS_SET event failed. Unknown redis event !");
									return true;
							 }
				 }//switch
				 if(itr != URENGINE_CONFIG::instance()->m_redisModProfile.m_mapGetRedisConnInfo.end())
				 {
						RedisConnInfo * redisConnInfo = itr->second;
						std::string strRedisIp = redisConnInfo->m_strServerHost.c_str();
						if(!strRedisIp.empty())
						{
							 IT_REDIS_HANDLER_MAP it = m_redisHandlerMap.find(strRedisIp);
							 if(it != m_redisHandlerMap.end())
							 {
									redisHandler = it->second;
							 }
						}
				 }
				 else
				 {
						XGLOG_ERROR("handle_msg REDIS_GET event failed. Not found redis_conn_info !");
						return false;
				 }
				 if(redisHandler)
				 {
						redisHandler->process_msg(pMsg);
				 }
				 break;
      }
			default:
			{
				 XGLOG_ERROR("handle_msg failed. Encountered unknown event !");
				 return false;
			}
   }

   return false;
}

bool xGateRedisService::handle_redis_conn_failed_notify_req(std::string strRedisIp)
{
    XGLOG_DEBUG("xGateRedisService::handle_redis_conn_failed_notify_req called for redis_ip:%s", strRedisIp.c_str());
    try
    {
	    xGateRedisHandler *redisHandler = nullptr;
	    RedisConnInfo * redisConnInfo = nullptr;
	    std::vector<std::string> t_vsubChannel;
	    IT_REDIS_HANDLER_MAP itr = m_redisHandlerMap.find(strRedisIp);
	    if(itr != m_redisHandlerMap.end())
	    {
		    redisHandler = itr->second;
		    m_redisHandlerMap.erase(itr);
	    }
	    else
	    {
		    XGLOG_ERROR("handle_redis_conn_failed_notify_req failed. RedisHandler NULL !");
		    return false;
	    }
	    if(redisHandler)
	    {
		    redisConnInfo = redisHandler->m_redisConnInfo;
		    t_vsubChannel = redisHandler->m_vsubChannel;
		    //delete redis handler context
		    XGLOG_DEBUG("handle_redis_conn_failed_notify_req deleting redis handler context for redis_ip:%s", \
				    strRedisIp.c_str());
		    delete redisHandler;
		    redisHandler = NULL;
	    }
	    if(!redisHandler && redisConnInfo)
	    {
		    XGLOG_DEBUG("handle_redis_conn_failed_notify_req creating redis handler context for redis_ip:%s", \
				    strRedisIp.c_str());
		    //create redis handler context
		    redisHandler = new xGateRedisHandler(redisConnInfo);
		    m_redisHandlerMap.insert(REDIS_HANDLER_MAP::value_type(strRedisIp, redisHandler));
		    redisHandler->m_vsubChannel = t_vsubChannel;
		    redisHandler->init();
	    }
    }
    catch (std::exception& e)
    {
	    XGLOG_ERROR("handle_redis_conn_failed_notify_req Exception caught - %s", e.what());
    }

    return true;
}
