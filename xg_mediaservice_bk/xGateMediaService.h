#ifndef _XGATE_MEDIA_SERVICE_H
#define _XGATE_MEDIA_SERVICE_H
#include <ace/Task.h>
#include <ace/Singleton.h>

//local includes
#include "MediaServiceDefine.h"
#include "xGateBaseMsg.h"
#include "IURModule.h"
#include "xGateUtil.h"

#define MEDIASERVICE MediaService::instance()
class xGateMediaService : public ACE_Task<ACE_MT_SYNCH>,public IURModule {
    
    IURModuleConfig * m_mediaConfig;
  public:  
    xGateMediaService();
    ~xGateMediaService(void);
 
    IURModuleConfig *getConfig(){return m_mediaConfig; }
    virtual IURDefines::MODULE_ID getModuleID() ;
    virtual IURModuleConfig * getModuleConfig();

  private:
    bool init();
    virtual int svc(void);
    bool stop();
     
    int initModule(IURModuleConfig *);
    int unitModule();
    void getModuleName(std::string& moduleName);

    int pushModuleMsg(IURModuleMsg * pmediaMsg);
    bool handle_msg(ACE_Message_Block *pAmb);
    //member variables
    bool m_run;
    std::string m_strModuleName;
};

typedef ACE_Unmanaged_Singleton<xGateMediaService, ACE_Null_Mutex> MediaService;

#endif
