
#ifndef _XGATE_DB_SUBSCRIBE_H
#define _XGATE_DB_SUBSCRIBE_H
#include <ace/Task.h>
#include <ace/Singleton.h>
#include "sw/redis++/redis++.h"
#include <thread>

//local includes
#include "xGateBaseMsg.h"
#include "IURModule.h"
#include "UrMrcpDbServiceMsg.h"
#include "MrcpConfig.h"

using namespace sw::redis;
#define REDISDBSUBSERVICE RedisDbSubService::instance()

class MrcpRedisDbSubscriber : public ACE_Task<ACE_MT_SYNCH>, public IURModule 
{
    IURModuleConfig* m_redisDbConfig;
  public:  
    MrcpRedisDbSubscriber();
    ~MrcpRedisDbSubscriber(void);

    IURModuleConfig *getConfig(){return m_redisDbConfig; }
    virtual IURDefines::MODULE_ID getModuleID() ;
    virtual IURModuleConfig * getModuleConfig();
	
    int initModule(IURModuleConfig *);
    int unitModule();
    void getModuleName(std::string& moduleName);

    int pushModuleMsg(IURModuleMsg * pdbConnectorMsg);

  private:

    bool init();
    virtual int svc(void);
    bool stop();

    bool handle_msg(redisReply* reply);
    bool handle_channel_msg(redisReply* reply);
    bool connectToRedisDb(std::string host, int port, std::string pwd);
    bool redisDbDatabaseAuthorize(std::string pwd);
    bool subcribeAvaamoResponseChannel(std::string channelName);
    bool connectToCcaasRedisDb(std::string host, int port);
    bool ccaasRedisDbDatabaseAuthorize(std::string pwd);
    bool subcribeCcaasResponseChannel(std::string channelName);
    void redisCcaasResponse();
    bool free();
    bool connectToDb(std::string host, int port, std::string pwd, std::string channel);
    bool authorizeConnection(redisContext* context, std::string pwd);
    bool subcribeChannel(redisContext* context, std::string channelName);
    void onMessage(redisContext* context);
    UrMrcpDbServiceMsg* decode_transcript_request_msg(const char* msg);
    UrMrcpDbServiceMsg* decode_transcript_response_msg(const char* json_msg);
    UrMrcpDbServiceMsg* decode_msg(const char* msg);
    bool consume_from_subscribe_channel(RedisConnInfo* redisInfo);
    bool consume_from_pop_channel(RedisConnInfo* redisInfo);
    ConnectionOptions connectToRedis(RedisConnInfo* redisInfo);
    bool handle_redis_response(std::string msg);
    bool handle_redis_vms_request(std::string msg);
    bool handle_redis_vms_response(std::string msg);
    bool findAndTrim(std::string &text);
  private:
    bool m_run;
    std::string m_strModuleName;
    redisContext* m_subContext;
    redisContext* m_ccaasSubContext;
    RedisConnInfo* botRedisInfo;
    RedisConnInfo* ccaasRedisInfo;
    RedisConnInfo* sttRedisInfo;
    RedisConnInfo* vmsRedisTranscriptInfo;
};

typedef ACE_Unmanaged_Singleton<MrcpRedisDbSubscriber, ACE_Null_Mutex> RedisDbSubService;
#endif
