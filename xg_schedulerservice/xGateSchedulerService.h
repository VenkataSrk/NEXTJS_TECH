#ifndef _XGATE_SCHEDULER_SERVICE_H
#define _XGATE_SCHEDULER_SERVICE_H
#include <ace/Task.h>
#include <ace/Singleton.h>

#include "xGateUtil.h"
#include "xGateSchedulerHandler.h"
#include "xGateBaseMsg.h"
#include "IURModule.h"
#include "xgSchedulerConfig.h"

#define SCHEDULERSERVICE SchedulerService::instance()
class xGateSchedulerService : public ACE_Task<ACE_MT_SYNCH>,public IURModule
{
	// uuid , to service request info..
	//map<string, servicereqestinfo > 
  public:
    xGateSchedulerService();
    ~xGateSchedulerService(void);

    IURModuleConfig * m_schedulerConfig;
    IURModuleConfig *getConfig()
    {
      return m_schedulerConfig; 
    }
    virtual IURDefines::MODULE_ID getModuleID();
    virtual IURModuleConfig * getModuleConfig();

  private:
    bool init();
    virtual int svc(void);
    bool stop();

    int initModule(IURModuleConfig *);
    int unitModule();
    void getModuleName(std::string& moduleName);

    int pushModuleMsg(IURModuleMsg * pschedulerMsg);
    //member functions
    bool handle_msg(ACE_Message_Block *pAmb);
    bool post(SchedulerInfo *schedulerInfo);
    
    //member variables
    bool m_run;
    std::string m_strModuleName;
};

typedef ACE_Singleton<xGateSchedulerService, ACE_Recursive_Thread_Mutex> SchedulerService;
#endif


