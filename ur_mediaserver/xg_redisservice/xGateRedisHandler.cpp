#include "xglog.h"
#include "xGateRedisHandler.h"


xGateRedisHandler::xGateRedisHandler(RedisConnInfo * redisConnInfo): m_redisConnInfo(redisConnInfo), m_pubConn(create_redis_pub_connection()), m_pubRedis(m_pubConn)
{}

xGateRedisHandler::~xGateRedisHandler()
{}

int xGateRedisHandler::init(void)
{
  #if 0
  std::map<EN_REDIS_MSG_TYPE, RedisConnInfo>::iterator socketItr;
  socketItr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapPubRedisConnInfo.find(EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_SOCKET);
  if(socketItr != URENGINE_CONFIG::instance()->m_redisModProfile.m_mapPubRedisConnInfo.end())
  {
    m_pubSocketRedisConnInfo = socketItr->second;
  }

  std::map<EN_REDIS_MSG_TYPE, RedisConnInfo>::iterator confItr;
  confItr = URENGINE_CONFIG::instance()->m_redisModProfile.m_mapPubRedisConnInfo.find(EN_REDIS_MSG_TYPE::EN_REDIS_MSG_TYPE_CONFBRIDGE);
  if(confItr != URENGINE_CONFIG::instance()->m_redisModProfile.m_mapPubRedisConnInfo.end())
  {
    m_pubConfBridgeRedisConnInfo = confItr->second;
  }
  #endif
  std::thread m_thread(&xGateRedisHandler::consume_msg,this);
  m_thread.detach();
  return 0;
}

void xGateRedisHandler::consume_msg()
{
  try
  {
    //RedisConnInfo& subRedisConnInfo = URENGINE_CONFIG::instance()->m_redisModProfile.m_subRedisConnInfo;
    if(!m_redisConnInfo->m_strServerHost.empty())
    {
      ConnectionOptions t_connOpts;
      t_connOpts.host = m_redisConnInfo->m_strServerHost.c_str();
      t_connOpts.port = (m_redisConnInfo->m_iServerPort > 0) ? m_redisConnInfo->m_iServerPort : 6379;
      if(!m_redisConnInfo->m_strPassword.empty())
      {
	      t_connOpts.password = m_redisConnInfo->m_strPassword.c_str();
      }

      Redis subRedis(t_connOpts);
      //RedisCluster subRedis(t_connOpts);
      auto sub = subRedis.subscriber();

      sub.on_message([this](std::string channel, std::string msg)
      {
      static int msgCount = 0;
      msgCount++;
      XGLOG_INFO("Consuming msg count:%d from channel:%s", msgCount, channel.c_str());
      postMsgToController(msg.c_str(), channel.c_str());
      });

      for(auto ch : m_vsubChannel)
      {
	      XGLOG_INFO("$$$$$$$$$$$$$$$ consume_msg subscribing to channel:%s for redis_ip: (%s)", \
			      ch.c_str(), m_redisConnInfo->m_strServerHost.c_str());
	      sub.subscribe(ch.c_str());
      }
      bool b_isConnectionExists = true;
      while(b_isConnectionExists)
      {
        try
        {
          sub.consume();
        }
        catch (const TimeoutError &e)
        {
          continue;
        }
        catch (const Error &err)
        {
          XGLOG_ERROR("init redis subscriber consume message failed. Error - %s", err.what());
          b_isConnectionExists = false;
          for(auto ch : m_vsubChannel)
          {
            sub.unsubscribe(ch.c_str());
          }
          //sendConnFailedNotify();
          return;
        }
      }
    }
  }
  catch(const Error &err)
  {
    XGLOG_ERROR("subscribeChannel redis subscribe to channel failed. Exception caught - %s",err.what());
    //sendConnFailedNotify();
    return;
  }
  return;
}

bool xGateRedisHandler::process_msg(RedisServiceMsg *pMsg)
{
   XGLOG_DEBUG("xGateRedisHandler::process_msg called");
   IURDefines::MODULE_ID srcModuleId = pMsg->getSrcModuleId();
   switch(srcModuleId)
   {
      case IURDefines::UR_MODULE_SERVICE_CONTROLLER:
      {
        return process_msg_from_controller(pMsg);
      } 
      case IURDefines::UR_MODULE_UNKNOWN:
      default:
      {
        XGLOG_ERROR("process_msg failed. Unknown source module id !");
        return false;
      } 
   } 
   return true;
}

bool xGateRedisHandler::process_msg_from_controller(RedisServiceMsg *pMsg)
{
  XGLOG_INFO("In xGateRedisHandler::process_msg_from_controller");
  try
  {
    RedisInfo redisInfo = pMsg->get_redis_info();
    RedisServiceEvent redisEvent = redisInfo.m_enRedisEvent;
    switch(pMsg->getMsgType())
    {
      case EN_XGATE_MSG_REDIS_PUBLISH:
	{
	  XGLOG_INFO("process_msg_from_controller REDIS_PUBLISH for callid:%s", redisInfo.m_strContextId.c_str());
	  switch(redisInfo.m_enRedisMethod) 
	  {
	    case EN_REDIS_METHOD::EN_REDIS_METHOD_PUB_SUB:
	    {
	      m_pubRedis.publish(redisInfo.m_strChannel, redisInfo.m_strMsg.c_str());	
	      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	      XGLOG_INFO("EN_REDIS_MSG_TYPE_PUB_SUB success for callid:%s && message:%s && channel:%s && redis_ip:%s", \
		  redisInfo.m_strContextId.c_str(), redisInfo.m_strMsg.c_str(), redisInfo.m_strChannel.c_str(), m_redisConnInfo->m_strServerHost.c_str()); 
	      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	      break;
	    }
	    case EN_REDIS_METHOD::EN_REDIS_METHOD_PUSH_POP:
	    {
	      m_pubRedis.rpush(redisInfo.m_strChannel, redisInfo.m_strMsg.c_str());
	      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	      XGLOG_INFO("EN_REDIS_MSG_TYPE_PUSH_POP success for callid:%s && message:%s && channel:%s && redis_ip:%s", \
		  redisInfo.m_strContextId.c_str(), redisInfo.m_strMsg.c_str(), redisInfo.m_strChannel.c_str(), m_redisConnInfo->m_strServerHost.c_str());
	      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	      break;
	    }
	    case EN_REDIS_METHOD::EN_REDIS_METHOD_STREAM:
	    {
	      std::string strPushId("");
	      std::vector<std::pair<std::string, std::string>> t_vAttrs;
	      t_vAttrs.push_back(std::make_pair("message", redisInfo.m_strMsg.c_str()));
	      strPushId = m_pubRedis.xadd(redisInfo.m_strChannel, "*", t_vAttrs.begin(),t_vAttrs.end());

	      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	      XGLOG_INFO("EN_REDIS_MSG_TYPE_STREAM success for callid:%s && message:%s && push_id:%s && stream:%s && redis_ip:%s", \
		  redisInfo.m_strContextId.c_str(), redisInfo.m_strMsg.c_str(), strPushId.c_str(), \
		  redisInfo.m_strChannel.c_str(), m_redisConnInfo->m_strServerHost.c_str());
	      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	      break;
	    }
	    default:
	    {
	      break;
	    }
	  }//inner-switch
	  break;
	}
      case EN_XGATE_MSG_REDIS_GET:
	{
	  XGLOG_INFO("process_msg_from_controller REDIS_GET for callid:%s", redisInfo.m_strContextId.c_str());
	  try
	  {
	    auto value = m_pubRedis.get(redisInfo.m_strKey.c_str());
	    redisInfo.m_strValue = (*value);
	    XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	    XGLOG_INFO("EN_XGATE_MSG_REDIS_GET success for callid:%s && key:%s && value:%s && redis_ip:%s", \
		redisInfo.m_strContextId.c_str(), redisInfo.m_strKey.c_str(), redisInfo.m_strValue.c_str(), m_redisConnInfo->m_strServerHost.c_str());
	    XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	    postGetResponseToController(redisInfo);
	  }
	  catch(const Error &err)
	  {
	    XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	    XGLOG_ERROR("EN_XGATE_MSG_REDIS_GET failed. Exception caught - %s",err.what());
	    XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	  }
	  break;
	}
      case EN_XGATE_MSG_REDIS_SET:
	{
	  XGLOG_INFO("process_msg_from_controller REDIS_SET for callid:%s", redisInfo.m_strContextId.c_str());
	  try
	  {
	    if(redisInfo.m_strKey.empty() || redisInfo.m_strValue.empty())
	    {
	    	XGLOG_ERROR("process_msg_from_controller REDIS_SET failed. key/value is empty");
	    	return false;
	    }
	    m_pubRedis.set(redisInfo.m_strKey.c_str(), redisInfo.m_strValue.c_str());
	    XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	    XGLOG_INFO("EN_XGATE_MSG_REDIS_SET success for callid:%s && key:%s && value:%s && redis_ip:%s", \
		redisInfo.m_strContextId.c_str(), redisInfo.m_strKey.c_str(), redisInfo.m_strValue.c_str(), m_redisConnInfo->m_strServerHost.c_str());
	    XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	  }
	  catch(const Error &err)
	  {
	    XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	    XGLOG_ERROR("EN_XGATE_MSG_REDIS_SET failed. Exception caught - %s",err.what());
	    XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
	  }
	  break;
	}
      default:
	{
	  break;
	}
    }
  } 
  catch(const Error &err)
  {
    XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    XGLOG_ERROR("EN_XGATE_MSG_REDIS_PUBLISH failed. Exception caught - %s for redis_ip:%s", \
	err.what(), m_redisConnInfo->m_strServerHost.c_str());
    XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
  }
  return true;
} 

ConnectionOptions xGateRedisHandler::create_redis_pub_connection()
{ 
  ConnectionOptions t_connOpts;
  t_connOpts.host = m_redisConnInfo->m_strServerHost.c_str(); 
  t_connOpts.port = (m_redisConnInfo->m_iServerPort > 0) ? m_redisConnInfo->m_iServerPort : 6379; 
  if(!m_redisConnInfo->m_strPassword.empty())
  {
    t_connOpts.password = m_redisConnInfo->m_strPassword.c_str();
  }
  t_connOpts.socket_timeout = (m_redisConnInfo->m_iSocketTimeout > 0) ? std::chrono::milliseconds(m_redisConnInfo->m_iSocketTimeout) : std::chrono::milliseconds(0); 
  return t_connOpts;
}

ConnectionOptions xGateRedisHandler::set_sub_connection_opts(RedisConnInfo &redisConnInfo)
{
  ConnectionOptions t_connOpts;
  t_connOpts.host = redisConnInfo.m_strServerHost.c_str();
  t_connOpts.port = (redisConnInfo.m_iServerPort > 0) ? redisConnInfo.m_iServerPort : 6379;
  t_connOpts.socket_timeout = (redisConnInfo.m_iSocketTimeout > 0) ? std::chrono::milliseconds(redisConnInfo.m_iSocketTimeout) : std::chrono::milliseconds(0);
  if(!redisConnInfo.m_strPassword.empty())
  {
    t_connOpts.password = redisConnInfo.m_strPassword.c_str();
  }
  return t_connOpts;
}

bool xGateRedisHandler::postMsgToController(std::string msg, std::string channel)
{
   XGLOG_DEBUG("xGateRedisHandler::postMsgToController called");
   RedisServiceMsg *pRedisMsg = new RedisServiceMsg();
   if(!pRedisMsg)
   {
      XGLOG_ERROR("postMsgToController failed to create redis message object");
      return false;
   }
   pRedisMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
   pRedisMsg->setSrcModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
   pRedisMsg->setMsgType(EN_XGATE_MSG_REDIS_SUBSCRIBE_NOTIFY);
   RedisInfo &redisInfo = pRedisMsg->get_redis_info();
   redisInfo.m_strMsg = msg.c_str();
   redisInfo.m_strChannel = channel.c_str();
   XGSFUCONTROLLER->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(pRedisMsg));
   return true;
}

bool xGateRedisHandler::postGetResponseToController(RedisInfo &t_redisInfo)
{
   XGLOG_DEBUG("xGateRedisHandler::postGetResponseToController called");
   RedisServiceMsg *pRedisMsg = new RedisServiceMsg();
   if(!pRedisMsg)
   {
      XGLOG_ERROR("postGetResponseToController failed to create redis message object");
      return false;
   }
   pRedisMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
   pRedisMsg->setSrcModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
   pRedisMsg->setMsgType(EN_XGATE_MSG_REDIS_GET_RES);
   RedisInfo &redisInfo = pRedisMsg->get_redis_info();
   redisInfo = t_redisInfo;
   XGSFUCONTROLLER->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(pRedisMsg));
   return true;
}

bool xGateRedisHandler::sendConnFailedNotify()
{
   XGLOG_INFO("xGateRedisHandler::sendConnFailedNotify called");
   RedisServiceMsg *pRedisMsg = new RedisServiceMsg();
   if(!pRedisMsg)
   {
      XGLOG_ERROR("sendConnFailedNotify failed to create redis message object");
      return false;
   }
   pRedisMsg->setDstModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
   pRedisMsg->setSrcModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
   pRedisMsg->setMsgType(EN_XGATE_MSG_REDIS_CONN_FAILED_NOTIFY);
   RedisInfo &redisInfo = pRedisMsg->get_redis_info();
   redisInfo.m_strRedisIp = m_redisConnInfo->m_strServerHost.c_str();
   XGSFUCONTROLLER->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(pRedisMsg));
   return true;
}
