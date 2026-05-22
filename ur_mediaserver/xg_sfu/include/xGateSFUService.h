#ifndef _XGATE_SFU_SERVICE_H
#define _XGATE_SFU_SERVICE_H
#include <ace/Task.h>
#include <ace/Singleton.h>

#include "xGateUtil.h"
#include "xGateHttpHandler.h"
#include "xGateBaseMsg.h"
#include "IURModule.h"

#define SFUSERVICE SFUService::instance()
#define SFU_LISTEN_PORT 50000
typedef enum
{
 EN_XGATE_MSG_SFU_CLIENT_CONNECTED=0,
 EN_XGATE_MSG_SFU_ID,
 EN_XGATE_MSG_SFU_CLIENT,
 EN_XGATE_MSG_MBC_SERVER,
 EN_XGATE_MSG_SFU_CONN_CLOSED,
 EN_XGATE_MSG_SFU_CONNECT_CLIENT,
 EN_XGATE_MSG_SFU_CLIENT_REC,
}xGateSFUMsgType;

class xGateSFUService : public ACE_Task<ACE_MT_SYNCH>,public IURModule
{
  public:  
    xGateSFUService();
    ~xGateSFUService(void);

    IURModuleConfig * m_sfuConfig;
    IURModuleConfig *getConfig(){return m_sfuConfig; }
    virtual IURDefines::MODULE_ID getModuleID() ;
    virtual IURModuleConfig * getModuleConfig();

  private:
    bool init(int);
    virtual int svc(void);
    bool stop();
    void init_sfu(int);
    int initModule(IURModuleConfig *);
    int unitModule();
    void getModuleName(std::string& moduleName);

    int pushModuleMsg(IURModuleMsg * phttpMsg);
    //member functions
    bool handle_msg(ACE_Message_Block *pAmb);
    bool post(HttpRequestInfo *httpReqInfo);
    bool make_connection(void);
    void init_dbservice();

    //member variables
    bool m_run;
    std::string m_strModuleName;
};

typedef ACE_Unmanaged_Singleton<xGateSFUService, ACE_Null_Mutex> SFUService;
#endif
