#ifndef _XGATE_IVR_SERVICE_H
#define _XGATE_IVR_SERVICE_H
#include <ace/Task.h>
#include <ace/Singleton.h>
#include "sw/redis++/redis++.h"

//local includes
#include "xGateBaseMsg.h"
#include "IURModule.h"
#include "UrMrcpDbServiceMsg.h"
#include "MrcpConfig.h"

using namespace sw::redis;
#define REDISDBSERVICE RedisDbService::instance()
class MrcpRedisDbConnector : public ACE_Task<ACE_MT_SYNCH>, public IURModule {

    IURModuleConfig* m_redisDbConfig;
  public:  
    MrcpRedisDbConnector();
    ~MrcpRedisDbConnector(void);

    IURModuleConfig *getConfig(){return m_redisDbConfig; }
    virtual IURDefines::MODULE_ID getModuleID() ;
    virtual IURModuleConfig * getModuleConfig();
	
    int initModule(IURModuleConfig *);
    int unitModule();
    void getModuleName(std::string& moduleName);

    int pushModuleMsg(IURModuleMsg * pdbConnectorMsg);
    bool mrcpConnectToRedisDb(std::string host, int port);
    bool mrcpDatabaseAuthorize(std::string pwd);
    bool mrcpConnectToCcaasRedisDb(std::string host, int port);
    bool mrcpCcaasRedisDbAuthorize(std::string pwd);
    bool ccaasBotRedisDbConnect(std::string host, int port);
    bool ccaasBotRedisDbAuthorize(std::string pwd);
    // bool publishMrcpDetailsToDb(const char* channel,const char* msg, bool isccaas, bool isCcaasAgentConnected);
    bool MrcpRedisDbConnector::PostToRedis(std::string channel,const char* msg,redisContext* context,EN_REDIS_MSG_TYPE m_enMsgType);
    bool free();
    std::string formJsonMessageForRedisDb(UrMrcpDbServiceMsg *dbServiceMsg);
    std::string encodeJsonMessageForCcaasConv(UrMrcpDbServiceMsg *dbServiceMsg);
    std::string encodeJsonMessageForBotConv(UrMrcpDbServiceMsg *dbServiceMsg);
    std::string encodeJsonMessageForUcaasConv(UrMrcpDbServiceMsg *dbServiceMsg);
    bool publishChannel(RedisConnInfo* redisConnInfo, ccaasVmsInfo *ccaasInfo);
    bool encodeVmsSttMsg(ccaasVmsInfo *ccaasInfo);
    std::string frameDisplayInfo(string ext,string domain_id,string device);
    bool post_to_redis(RedisConnInfo* redisInfo,std::string channel,std::string msg);
    bool findAndTrim(std::string &text);
    ConnectionOptions connectToRedis(RedisConnInfo* redisInfo);
    Value setJsonParam(std::string strValue, Document &doc);
    Value setJsonParam(unsigned int ivalue, Document &doc);
    STR encodeJsonMessageForPbx(UrMrcpDbServiceMsg *dbServiceMsg);

    std::string dbIp;
    int port;
    std::string dbPwd;

  private:

    bool init();
    virtual int svc(void);
    bool stop();


    bool handle_msg(ACE_Message_Block *pAmb);
    //member variables
  private:
    bool m_run;
    std::string m_strModuleName;
    redisContext* m_dbContext;
    redisContext* m_ccaasDbContext;
    redisContext* m_ccaasBotContext;
    std::vector<std::string> m_vBotPubChannel;
    std::vector<std::string> m_vCcaasPubChannel;
    std::vector<std::string> m_vCcaasBotPubChannel;
    RedisConnInfo* m_tCcaasVmsSttInfo;
    RedisConnInfo* m_tbotPubRedisInfo;
    RedisConnInfo* m_tccaasPubRedisInfo;
    RedisConnInfo* m_tccaasBotRedisInfo;
    RedisConnInfo* m_tCcaasConv;
    RedisConnInfo* m_tUcaasConv;
    RedisConnInfo* m_tCcaasVmsTranscriptInfo;
    RedisConnInfo* m_tpbxPubRedisInfo;
    EN_REDIS_MSG_TYPE m_enBotMsgType;
    EN_REDIS_MSG_TYPE m_enAcdMsgType;
};

typedef ACE_Unmanaged_Singleton<MrcpRedisDbConnector, ACE_Null_Mutex> RedisDbService;

#endif
