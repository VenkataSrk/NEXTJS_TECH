#ifndef _XGATE_REDIS_SERVICE_H
#define _XGATE_REDIS_SERVICE_H
#include <ace/Task.h>
#include <ace/Singleton.h>

//local includes
#include "xGateRedisHandler.h"
#include "xGateBaseMsg.h"
#include "IURModule.h"
#define REDISSERVICE RedisService::instance()

typedef std::map<std::string, xGateRedisHandler*> REDIS_HANDLER_MAP;
typedef REDIS_HANDLER_MAP::iterator IT_REDIS_HANDLER_MAP;

class xGateRedisService : public ACE_Task<ACE_MT_SYNCH>, public IURModule {

   IURModuleConfig* m_redisConfig;
   public:  
   xGateRedisService();
   ~xGateRedisService(void);

   IURModuleConfig *getConfig(){return m_redisConfig; }
   virtual IURDefines::MODULE_ID getModuleID() ;
   virtual IURModuleConfig * getModuleConfig();

   int initModule(IURModuleConfig *);
   int unitModule();
   void getModuleName(std::string& moduleName);

   int pushModuleMsg(IURModuleMsg * predisMsg);
   vector< IURModule*> arrayModules;

   private:

   bool init();
   virtual int svc(void);
   bool stop();
   bool initRedisHandler();
   bool handle_msg(ACE_Message_Block *pAmb);
   bool handle_redis_conn_failed_notify_req(std::string strRedisIp);

   //member variables
   bool m_run;
   std::string m_strModuleName;

   xGateRedisHandler *m_redisHandler;
   REDIS_HANDLER_MAP m_redisHandlerMap;

};

typedef ACE_Singleton<xGateRedisService, ACE_Recursive_Thread_Mutex> RedisService;

#endif

