#ifndef XGATE_MBC_SERVICE_H
#define XGATE_MBC_SERVICE_H
#include <ace/Task.h>
#include <ace/Singleton.h>

#include "xGateBaseMsg.h"
#include "xGateUtil.h"
#include "XGConfig.h"
#include "IURModule.h"
#include "xGateMBCConfig.h"
#include "xGateMBCServiceDefine.h"
//#include "xGateMBCUtil.h"
#include "xGateMBCServiceMsg.h"
#include "MediaServiceDefine.h"

#define XGMBCSERVICE XGMBC_Service::instance()
#define SFU_LISTEN_PORT 50000

class xGateMBCService : public ACE_Task<ACE_MT_SYNCH>,public IURModule
{
  public:  
    xGateMBCService();
    ~xGateMBCService(void);

    IURModuleConfig * m_mediaBroadcastConfig;
    IURModuleConfig *getConfig(){return m_mediaBroadcastConfig;}
    virtual IURDefines::MODULE_ID getModuleID() ;
    virtual IURModuleConfig * getModuleConfig();

  private:
    bool init();
    virtual int svc(void);
    bool stop();
    
    int initModule(IURModuleConfig *);
    int unitModule();
    void getModuleName(std::string& moduleName);

    int pushModuleMsg(IURModuleMsg * pmediaBroadcastMsg);

    //member functions
    bool init_mbc();
    bool handle_msg(ACE_Message_Block *pAmb);
    bool handle_IncomingMBCMsg(xGateMBCServiceMsg *pmediaBCastMsg);
    bool initClientConnectionRequest();
    bool initRecordingConnectionRequest();
    bool initTcpListenRequest();
    bool reconnectClient(MBCTcpConnInfo & mbcTcpConnInfo);

    //member variables
    bool m_run;
    std::string m_strModuleName;
};

typedef ACE_Unmanaged_Singleton<xGateMBCService, ACE_Null_Mutex> XGMBC_Service;
#endif
