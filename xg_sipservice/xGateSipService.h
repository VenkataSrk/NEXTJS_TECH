#ifndef _XGATE_SIP_SERVICE_H
#define _XGATE_SIP_SERVICE_H
#include <ace/Task.h>
#include <ace/Singleton.h>

//local includes
//#include "SipServiceDefine.h"
#include "xGateSipUtil.h"
#include "IURModule.h"
static void *SipDispatcher;
static void *SipProcessor;
static void *DBManager;

void SetSipPostHandler(void *Handler,XGATEPOSTTYPE type);
void *GetSipPostHandler(XGATEPOSTTYPE type);


#define SIPSERVICE SipService::instance()
class xGateSipService : public ACE_Task<ACE_MT_SYNCH>,public IURModule {
  public:  
    xGateSipService();
    ~xGateSipService(void);
    
    IURModuleConfig * m_sipConfig;
    //DebugVM

    private:
    bool init();
    bool uninit(); //DebugVM

    virtual int svc(void);
    bool stop();

    int initModule(IURModuleConfig *);
    int unitModule();
    void getModuleName(std::string& moduleName);
 
   IURDefines::MODULE_ID getModuleID(){
      return IURDefines::UR_MODULE_SIPSIGNAL_SERVICE;
    }
    std::string m_strModuleName;
    bool m_run;
    int pushModuleMsg(IURModuleMsg * phttpMsg);
    bool handle_msg(ACE_Message_Block *pAmb);
};

typedef ACE_Unmanaged_Singleton<xGateSipService, ACE_Null_Mutex> SipService;

#endif
