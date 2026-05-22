#ifndef _XGATE_NETIO_SERVICE_H
#define _XGATE_NETIO_SERVICE_H
#include <ace/Task.h>
#include <ace/Singleton.h>

//local includes
#include "NetIOServiceDefine.h"
#include "xGateNetIOServiceMsg.h"
#include "xGateBaseMsg.h"
#include "IURModule.h"
#include "xGateUtil.h"

#define NETIOSERVICE NetIOService::instance()
class xGateNetIOService : public ACE_Task<ACE_MT_SYNCH>,public IURModule {
    
    IURModuleConfig * m_netioConfig;
  public:  
    xGateNetIOService();
    ~xGateNetIOService(void);
 
    IURModuleConfig *getConfig(){return m_netioConfig; }
    virtual IURDefines::MODULE_ID getModuleID() ;
    virtual IURModuleConfig * getModuleConfig();

  private:
    bool init();
    virtual int svc(void);
    bool stop();
     
    int initModule(IURModuleConfig *);
    int unitModule();
    void getModuleName(std::string& moduleName);

		bool CreateTcpClient(xGateNetIOServiceMsg *pNetIOMsg);
		bool CreateTcpListener(xGateNetIOServiceMsg *pNetIOMsg);
		bool SendMessageToSocket(xGateNetIOServiceMsg *pNetIOMsg);

    int pushModuleMsg(IURModuleMsg * pnetioMsg);
    bool handle_msg(ACE_Message_Block *pAmb);
    //member variables
    bool m_run;
    std::string m_strModuleName;
};

typedef ACE_Unmanaged_Singleton<xGateNetIOService, ACE_Null_Mutex> NetIOService;

#endif
