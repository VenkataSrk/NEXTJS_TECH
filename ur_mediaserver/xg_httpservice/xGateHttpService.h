#ifndef _XGATE_HTTP_SERVICE_H
#define _XGATE_HTTP_SERVICE_H
#include <ace/Task.h>
#include <ace/Singleton.h>

#include "xGateUtil.h"
#include "xGateHttpHandler.h"
#include "xGateBaseMsg.h"
#include "IURModule.h"
#include "xgHttpConfig.h"

#define HTTPSERVICE HttpService::instance()
class xGateHttpService : public ACE_Task<ACE_MT_SYNCH>,public IURModule
{
  public:  
    xGateHttpService();
    ~xGateHttpService(void);

    IURModuleConfig * m_httpConfig;
    IURModuleConfig *getConfig(){return m_httpConfig; }
    virtual IURDefines::MODULE_ID getModuleID() ;
    virtual IURModuleConfig * getModuleConfig();

  private:
    bool init();
    virtual int svc(void);
    bool stop();
    
    int initModule(IURModuleConfig *);
    int unitModule();
    void getModuleName(std::string& moduleName);

    int pushModuleMsg(IURModuleMsg * phttpMsg);
    //member functions
    bool handle_msg(ACE_Message_Block *pAmb);
    bool post(HttpRequestInfo *httpReqInfo);

    //member variables
    bool m_run;
    std::string m_strModuleName;
};

typedef ACE_Unmanaged_Singleton<xGateHttpService, ACE_Null_Mutex> HttpService;
#endif
