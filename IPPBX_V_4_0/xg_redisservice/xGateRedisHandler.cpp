#include "xglog.h"
#include "xGateRedisHandler.h"


xGateRedisHandler::xGateRedisHandler() : m_pubConn(create_redis_pub_connection()), m_pubRedis(m_pubConn)
{
}

xGateRedisHandler::~xGateRedisHandler()
{
}

int xGateRedisHandler::init(void)
{
  m_pubRedisConnInfo.m_strServerHost =  XGCONFIG().m_redisProfile.m_strServerHost;
  m_pubRedisConnInfo.m_iServerPort =  XGCONFIG().m_redisProfile.m_iServerPort;
  m_pubRedisConnInfo.m_strPassword = XGCONFIG().m_redisProfile.m_strPassword;
  m_pubRedisConnInfo.m_strChannel = XGCONFIG().m_redisProfile.m_strChannel;
  m_pubRedisConnInfo.m_iSocketTimeout = XGCONFIG().m_redisProfile.m_iSocketTimeout;
  //Currently we are not consuming anything from redis
  // std::thread m_thread(&xGateRedisHandler::consume_msg, this);
  // m_thread.detach();
  return 0;
}

void xGateRedisHandler::consume_msg()
{
#if 0//currently not consuming any message 
  try
  {
    RedisConnInfo &subRedisConnInfo = SRSServerConfig::instance()->m_redisConInfo;
    if (!subRedisConnInfo.m_strServerHost.empty())
    {
      ConnectionOptions t_connOpts;
      t_connOpts.host = subRedisConnInfo.m_strServerHost.c_str();
      t_connOpts.port = (subRedisConnInfo.m_iServerPort > 0) ? subRedisConnInfo.m_iServerPort : 6379;
      if (!subRedisConnInfo.m_strPassword.empty())
      {
        t_connOpts.password = subRedisConnInfo.m_strPassword.c_str();
      }

      Redis subRedis(t_connOpts);
      auto sub = subRedis.subscriber();

      sub.on_message([this](std::string channel, std::string msg)
      {
	      static int msgCount = 0;
	      msgCount++;
	      XGLOG_INFO("Consuming msg count:%d from channel:%s", msgCount, channel.c_str());
	      sendSubscribeNotifyMsgToCtrl(msg.c_str()); });

      for (auto ch : subRedisConnInfo.m_vChannel)
      {
        sub.subscribe(ch.c_str());
      }

      bool b_isConnectionExists = true;
      while (b_isConnectionExists)
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
          sendConnFailedNotify();
          return;
        }
      }
    }
  }
  catch (const Error &err)
  {
    XGLOG_ERROR("subscribeChannel redis subscribe to channel failed. Exception caught - %s", err.what());
    sendConnFailedNotify();
    return;
  }
#endif
  return;
}

bool xGateRedisHandler::process_msg(RedisServiceMsg *pMsg)
{
  XGLOG_DEBUG("xGateRedisHandler::process_msg called");
  IURDefines::MODULE_ID srcModuleId = pMsg->getSrcModuleId();
  switch (srcModuleId)
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
  RedisInfo redisInfo = pMsg->get_redis_info();
  RedisServiceEvent redisEvent = pMsg->get_redis_event();
  RedisMsgType redisMsgType = pMsg->get_redis_msg_type();

  switch (redisEvent)
  {
  case EN_REDIS_EVENT_SUBSCRIBE_NOTIFY:
  {
    XGLOG_INFO("process_msg_from_controller EN_REDIS_EVENT_SUBSCRIBE");
    break;
  }
  case EN_REDIS_EVENT_PUBLISH:
  {
    XGLOG_INFO("process_msg_from_controller EN_REDIS_EVENT_PUBLISH for callid:%s", redisInfo.m_strContextId.c_str());
    try
    {
      m_pubRedis.publish(m_pubRedisConnInfo.m_strChannel, redisInfo.m_strMsg.c_str());
      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      XGLOG_INFO("process_msg_from_controller success for callid:%s && message:%s", redisInfo.m_strContextId.c_str(), redisInfo.m_strMsg.c_str());
      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    }
    catch (const Error &err)
    {
      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
      XGLOG_ERROR("process_msg_from_controller Redis DB publish message failed. Exception caught - %s", err.what());
      XGLOG_ERROR("process_msg_from_controller Redis DB publish message failed. callid- %s", redisInfo.m_strContextId.c_str());
      XGLOG_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    }
    break;
  }
  case EN_REDIS_EVENT_SET:
  {
    XGLOG_INFO("process_msg_from_controller EN_REDI_EVENT_SET for callid:%s", redisInfo.m_strContextId.c_str());
    try
    {
      m_pubRedis.set(redisInfo.m_strKey.c_str(), redisInfo.m_strValue.c_str());
      XGLOG_INFO("process_msg_from_controller success for callid:(%s) ,key(%s),value(%s)", redisInfo.m_strContextId.c_str(),redisInfo.m_strKey.c_str(),redisInfo.m_strValue.c_str());
      sendSetResponseMsgToCtrl(redisInfo.m_strContextId,redisMsgType);
      m_pubRedis.rpush(m_pubRedisConnInfo.m_strChannel,redisInfo.m_strMsg.c_str());
    }
    catch (const Error &err)
    {
      XGLOG_ERROR("process_msg_from_controller Redis DB set failed. Exception caught - %s", err.what());
      XGLOG_ERROR("process_msg_from_controller Redis DB set failed. callid:(%s),key(%s),value(%s)", redisInfo.m_strContextId.c_str(),redisInfo.m_strKey.c_str(),redisInfo.m_strValue.c_str());
    }
    break;
  }
  case EN_REDIS_EVENT_GET:
  {
    XGLOG_INFO("process_msg_from_controller EN_REDIS_EVENT_GET for callid:%s", redisInfo.m_strContextId.c_str());
    try
    {
      auto value = m_pubRedis.get(redisInfo.m_strKey.c_str());
      if(value)
      {
        std::cout << "key :" << redisInfo.m_strKey << "value:" << *value << std::endl;
        sendGetResponseMsgToCtrl(redisInfo.m_strContextId,redisInfo.m_strKey,*value,redisMsgType);
        XGLOG_INFO("process_msg_from_controller EN_REDIS_EVENT_GET success for callid:(%s),key(%s),value(%s)", redisInfo.m_strContextId.c_str(),redisInfo.m_strKey.c_str(),(*value).c_str());
      }
      else 
      {
        XGLOG_ERROR("process_ms");
        sendErrorResponseToCtrl(redisInfo.m_strContextId,redisMsgType);
        XGLOG_INFO("process_msg_from_controller failed for callid:%s and key:%s", redisInfo.m_strContextId.c_str(),redisInfo.m_strKey.c_str());
      }
      
    }
    catch (const Error &err)
    {
      XGLOG_ERROR("process_msg_from_controller Redis DB get failed. Exception caught - %s", err.what());
      XGLOG_ERROR("process_msg_from_controller Redis DB get failed. callid- %s", redisInfo.m_strContextId.c_str());
    }
    break;
  }
  case EN_REDIS_EVENT_RPUSH:
  {
    XGLOG_INFO("process_msg_from_controller EN_REDIS_EVENT_RPUSH for callid:%s", redisInfo.m_strContextId.c_str());
    try
    {
      m_pubRedis.rpush(m_pubRedisConnInfo.m_strChannel,redisInfo.m_strMsg.c_str());
      XGLOG_INFO("process_msg_from_controller EN_REDIS_EVENT_RPUSH success for callid:(%s) Msg:(%s)", redisInfo.m_strContextId.c_str(),redisInfo.m_strMsg.c_str());
    }
    catch(const Error &err)
    {
      XGLOG_ERROR("process_msg_from_controller Redis DB rpush failed. Exception caught - %s", err.what());
      XGLOG_ERROR("process_msg_from_controller Redis DB rpush failed. callid- %s", redisInfo.m_strContextId.c_str());
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

ConnectionOptions xGateRedisHandler::create_redis_pub_connection()
{
  ConnectionOptions t_connOpts;
  RedisProfile pubRedisConnInfo;
  pubRedisConnInfo.m_strServerHost = XGCONFIG().m_redisProfile.m_strServerHost;
  pubRedisConnInfo.m_iServerPort =  XGCONFIG().m_redisProfile.m_iServerPort;
  pubRedisConnInfo.m_strPassword = XGCONFIG().m_redisProfile.m_strPassword;
  pubRedisConnInfo.m_strChannel = XGCONFIG().m_redisProfile.m_strChannel;
  pubRedisConnInfo.m_iSocketTimeout = XGCONFIG().m_redisProfile.m_iSocketTimeout;
  t_connOpts.host = pubRedisConnInfo.m_strServerHost.c_str();
  t_connOpts.port = (pubRedisConnInfo.m_iServerPort > 0) ? pubRedisConnInfo.m_iServerPort : 6379;
  if (!pubRedisConnInfo.m_strPassword.empty())
  {
    t_connOpts.password = pubRedisConnInfo.m_strPassword.c_str();
  }
  t_connOpts.socket_timeout = (pubRedisConnInfo.m_iSocketTimeout > 0) ? std::chrono::milliseconds(pubRedisConnInfo.m_iSocketTimeout) : std::chrono::milliseconds(0);

  return t_connOpts;
}

ConnectionOptions xGateRedisHandler::set_sub_connection_opts(RedisProfile &redisProfile)
{
  ConnectionOptions t_connOpts;
  t_connOpts.host = redisProfile.m_strServerHost.c_str();
  t_connOpts.port = (redisProfile.m_iServerPort > 0) ? redisProfile.m_iServerPort : 6379;
  t_connOpts.socket_timeout = (redisProfile.m_iSocketTimeout > 0) ? std::chrono::milliseconds(redisProfile.m_iSocketTimeout) : std::chrono::milliseconds(0);
  if (!redisProfile.m_strPassword.empty())
  {
    t_connOpts.password = redisProfile.m_strPassword.c_str();
  }
  return t_connOpts;
}

bool xGateRedisHandler::sendSubscribeNotifyMsgToCtrl(std::string msg)
{
  XGLOG_DEBUG("xGateRedisHandler::sendSubscribeNotifyMsgToCtrl called");
  RedisServiceMsg *pRedisMsg = new RedisServiceMsg();
  if (!pRedisMsg)
  {
    XGLOG_ERROR("sendSubscribeNotifyMsgToCtrl failed to create redis message object");
    return false;
  }
  pRedisMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
  pRedisMsg->setSrcModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
  //pRedisMsg->setMsgType(EN_XGATE_REDIS_MODULE);
  pRedisMsg->set_redis_event(EN_REDIS_EVENT_SUBSCRIBE_NOTIFY);
  RedisInfo &redisInfo = pRedisMsg->get_redis_info();
  redisInfo.m_strMsg = msg.c_str();
  //SGWCONTROLLER->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pRedisMsg));
  return true;
}

bool xGateRedisHandler::sendGetResponseMsgToCtrl(std::string ctx_id,std::string key,std::string value,RedisMsgType redisMType)
{
  XGLOG_DEBUG("xGateRedisHandler::sendGetResponseMsgToCtrl called");
  RedisServiceMsg *pRedisMsg = new RedisServiceMsg();
  if (!pRedisMsg)
  {
    XGLOG_ERROR("sendGetResponseMsgToCtrl failed to create redis message object");
    return false;
  }

  pRedisMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
  pRedisMsg->setSrcModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
  //pRedisMsg->setMsgType(EN_XGATE_REDIS_MODULE);
  pRedisMsg->set_redis_event(EN_REDIS_EVENT_GET_RESPONSE);
  pRedisMsg->set_redis_msg_type(redisMType);
  RedisInfo &redisInfo = pRedisMsg->get_redis_info();
  redisInfo.m_strContextId = ctx_id.c_str();
  redisInfo.m_strKey = key.c_str();
  redisInfo.m_strValue = value.c_str();
  //SGWCONTROLLER->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pRedisMsg));

  return true;
}

bool xGateRedisHandler::sendSetResponseMsgToCtrl(std::string ctx_id, RedisMsgType redisMType)
{
  XGLOG_DEBUG("xGateRedisHandler::sendSetResponseMsgToCtrl called");
  RedisServiceMsg *pRedisMsg = new RedisServiceMsg();
  if (!pRedisMsg)
  {
    XGLOG_ERROR("sendSetResponseMsgToCtrl failed to create redis message object");
    return false;
  }

  pRedisMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
  pRedisMsg->setSrcModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
  //pRedisMsg->setMsgType(EN_XGATE_REDIS_MODULE);
  pRedisMsg->set_redis_event(EN_REDIS_EVENT_SET_RESPONSE);
  pRedisMsg->set_redis_msg_type(redisMType);
  RedisInfo &redisInfo = pRedisMsg->get_redis_info();
  redisInfo.m_strContextId = ctx_id.c_str();
  //SGWCONTROLLER->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pRedisMsg));

  return true;
}

bool xGateRedisHandler::sendErrorResponseToCtrl(std::string ctx_id, RedisMsgType redisMType)
{
#if 0
  XGLOG_DEBUG("xGateRedisHandler::sendSetResponseMsgToCtrl called");
  RedisServiceMsg *pRedisMsg = new RedisServiceMsg();
  if (!pRedisMsg)
  {
    XGLOG_ERROR("sendSetResponseMsgToCtrl failed to create redis message object");
    return false;
  }

  pRedisMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
  pRedisMsg->setSrcModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
  pRedisMsg->setMsgType(EN_XGATE_REDIS_MODULE);
  pRedisMsg->set_redis_event(EN_REDIS_EVENT_ERROR_RESPONSE);
  pRedisMsg->set_redis_msg_type(redisMType);
  RedisInfo &redisInfo = pRedisMsg->get_redis_info();
  redisInfo.m_strContextId = ctx_id.c_str();
  SGWCONTROLLER->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pRedisMsg));
#endif
  return true;
}

bool xGateRedisHandler::sendConnFailedNotify()
{
#if 0
  XGLOG_DEBUG("xGateRedisHandler::sendConnFailedNotify called");
  RedisServiceMsg *pRedisMsg = new RedisServiceMsg();
  if (!pRedisMsg)
  {
    XGLOG_ERROR("sendConnFailedNotify failed to create redis message object");
    return false;
  }
  pRedisMsg->setDstModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
  pRedisMsg->setSrcModuleId(IURDefines::UR_MODULE_REDIS_SERVICE);
  pRedisMsg->setMsgType(EN_XGATE_MSG_REDIS_CONN_FAILED_NOTIFY);
  // URCBCONTROLLER->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pRedisMsg));
#endif
  return true;
}
