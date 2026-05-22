#include "xglog.h"
#include "xGateRedisService.h"

xGateRedisService::xGateRedisService() : ACE_Task<ACE_MT_SYNCH>(),
                                         m_run(false), m_redisConfig(NULL)
{
   m_strModuleName = "RegServiceModule";
   m_redisHandler = new xGateRedisHandler();
}

xGateRedisService::~xGateRedisService(void)
{
}

int xGateRedisService::initModule(IURModuleConfig *predisConfig)
{
   XGLOG_INFO("xGateRedisService initModule initialized !");
   m_redisConfig = predisConfig;

   if (!init())
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

void xGateRedisService::getModuleName(std::string &moduleName)
{
   moduleName = m_strModuleName;
}

IURDefines::MODULE_ID xGateRedisService::getModuleID()
{
   return IURDefines::UR_MODULE_REDIS_SERVICE;
}

IURModuleConfig *xGateRedisService::getModuleConfig()
{
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
   if (activate(THR_NEW_LWP, 1) == -1)
   {
      XGLOG_ERROR("xGateRedisService::init() failed to start RegService thread !");
      return false;
   }
   else
   {
      XGLOG_INFO("xGateRedisService::init() success !");
   }

   m_redisHandler->init();

   return true;
}

int xGateRedisService::pushModuleMsg(IURModuleMsg *predisMsg)
{
   XGLOG_INFO("xGateRedisService::inside pushModuleMsg");
   if (!predisMsg)
   {
      XGLOG_ERROR("pushModuleMsg::Message not received from controller!");
      return IURDefines::UR_RETURN_ERROR;
   }
   else if (IURDefines::UR_MODULE_REDIS_SERVICE == predisMsg->getDstModuleId())
   {
      XGLOG_DEBUG("xGateRedisService::Message received from controller to regservice");
      REDISSERVICE->putq(dynamic_cast<ACE_Message_Block *>(predisMsg));
   }
   return IURDefines::UR_RETURN_SUCCESS;
}

int xGateRedisService::svc(void)
{
   int rc;
   rc = pthread_setname_np(ACE_Thread::self(), "RedisSrvc");
   if (rc != 0)
      printf("Failed to set name for redis service thread");
   ACE_Message_Block *pAmb = 0;

   while (m_run)
   {
      ACE_Time_Value tv((time(NULL) + 20));
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
      if (pAmb)
      {
         delete pAmb;
         pAmb = NULL;
      }
   }

   XGLOG_INFO("xGateRedisService::svc() stopped");
   return 0;
}

bool xGateRedisService::handle_msg(ACE_Message_Block *pAmb)
{
   XGLOG_DEBUG("xGateRedisService::handle_msg called");
   if (!pAmb)
   {
      XGLOG_ERROR("handle_msg failed. ACE_Message_Block is NULL !");
      return false;
   }

   RedisServiceMsg *pMsg = dynamic_cast<RedisServiceMsg *>(pAmb);
   if (!pMsg)
   {
      XGLOG_ERROR("handle_msg failed. RedisServiceMsg pointer is NULL !");
      return false;
   }

   switch (pMsg->getMsgType())
   {
   case EN_XGATE_MSG_REDIS_CONN_FAILED_NOTIFY:
   {
      XGLOG_INFO("handle_msg REDIS_CONN_FAILED_NOTIFY from redis module");
      handle_redis_conn_failed_notify_req();
      return true;
   }
   default:
   {
      break;
   }
   }
   if (m_redisHandler)
   {
      return m_redisHandler->process_msg(pMsg);
   }

   return false;
}

bool xGateRedisService::handle_redis_conn_failed_notify_req()
{
   XGLOG_DEBUG("xGateRedisService::handle_redis_conn_failed_notify_req called");
   if (m_redisHandler)
   {
      // delete redis handler context
      XGLOG_DEBUG("handle_redis_conn_failed_notify_req deleting redis handler context");
      delete m_redisHandler;
      m_redisHandler = NULL;
   }
   if (!m_redisHandler)
   {
      XGLOG_DEBUG("handle_redis_conn_failed_notify_req creating redis handler context");
      // create redis handler context
      m_redisHandler->init();
   }

   return true;
}
