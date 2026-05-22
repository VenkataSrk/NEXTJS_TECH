#pragma once

#include <ace/Task.h>
#include <ace/Singleton.h>

#include "xGateUtil.h"
#include "xGateDBHandler.h"
#include "IURModule.h"

#define DBSERVICE DBService::instance()
class xGateDBService : public ACE_Task<ACE_MT_SYNCH>,public IURModule
{
  public:  
    xGateDBService();
    ~xGateDBService(void);

    IURModuleConfig * m_dbConfig;
    IURModuleConfig *getConfig(){return m_dbConfig; }
    virtual IURDefines::MODULE_ID getModuleID() ;
    virtual IURModuleConfig * getModuleConfig();

  private:
    bool init();
    virtual int svc(void);
    bool stop();
    
    int initModule(IURModuleConfig *);
    int unitModule();
    void getModuleName(std::string& moduleName);

    int pushModuleMsg(IURModuleMsg * pdbMsg);
    //member functions
    bool handle_msg(ACE_Message_Block *pAmb);

    //member variables
    bool m_run;
    std::string m_strModuleName;
};

typedef ACE_Unmanaged_Singleton<xGateDBService, ACE_Null_Mutex> DBService;

