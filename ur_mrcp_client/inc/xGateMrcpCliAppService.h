#ifndef _UR_MRCP_CLI_APP_SERVICE_H
#define _UR_MRCP_CLI_APP_SERVICE_H
#include <ace/Task.h>
#include <ace/Singleton.h>

//local includes
#include "IURModule.h"
#include "xGateMrcpCliAppHandler.h"
#include "UniMrcpCliAppFwSpeechRecog.h"


#define MRCP_CLI_APP_SERVICE MrcpCliAp_Srvice::instance()
class xGateMrcpCliAppService : public ACE_Task<ACE_MT_SYNCH>, public IURModule {

   IURModuleConfig* m_dbConfigMgrConfig;
   public:  
   xGateMrcpCliAppService();
   ~xGateMrcpCliAppService(void);

   IURModuleConfig *getConfig(){return m_dbConfigMgrConfig; }
   virtual IURDefines::MODULE_ID getModuleID() ;
   virtual IURModuleConfig * getModuleConfig();

   int initModule(IURModuleConfig *);
   int unitModule();
	
   void trigger_event(); //Test function

   void getModuleName(std::string& moduleName);
   int pushModuleMsg(IURModuleMsg * pdbconfigMsg);

   vector< IURModule*> arrayModules;

   UniMrcpCliAppFwSpeechRecog* m_pFrameworkRecog;

   private:

   bool init();
   virtual int svc(void);
   bool stop();
   bool handle_msg(ACE_Message_Block *pAmb);

   //member variables
   bool m_run;
   std::string m_strModuleName;

   xGateMrcpCliAppHandler *m_mrcpCliAppHandler;
};

typedef ACE_Singleton<xGateMrcpCliAppService, ACE_Recursive_Thread_Mutex> MrcpCliAp_Srvice;

#endif

