#include "xglog.h"
#include "xGateMrcpCliAppService.h"
//#include "xGateHttpService.h"

xGateMrcpCliAppService::xGateMrcpCliAppService() :
   ACE_Task<ACE_MT_SYNCH>(),
   m_run(false), m_dbConfigMgrConfig(NULL), m_mrcpCliAppHandler(NULL)
{
   XGLOG_FUNCTION_EN_EX("");
   m_strModuleName = "xGateMrcpCliAppService";
}

xGateMrcpCliAppService::~xGateMrcpCliAppService(void)
{
       XGLOG_FUNCTION_EN_EX("");
	if (m_mrcpCliAppHandler)
		delete m_mrcpCliAppHandler;
}

int xGateMrcpCliAppService::initModule(IURModuleConfig * pregConfig)
{
   XGLOG_FUNCTION_EN_EX("");
   m_dbConfigMgrConfig = pregConfig;

   if(!init()) {
      XGLOG_ERROR("xGateMrcpCliAppService::init() function failed"); 
      return IURDefines::UR_RETURN_ERROR;
   }
   XGLOG_INFO("xGateMrcpCliAppService initModule initialized !");  
   
   return IURDefines::UR_RETURN_SUCCESS;
}

int xGateMrcpCliAppService::unitModule()
{
   XGLOG_FUNCTION_EN_EX("");
   stop();
   return IURDefines::UR_RETURN_SUCCESS;
}

void xGateMrcpCliAppService::getModuleName(std::string& moduleName){
   moduleName=m_strModuleName;
}

IURDefines::MODULE_ID xGateMrcpCliAppService::getModuleID(){
   return IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE;
}

IURModuleConfig * xGateMrcpCliAppService::getModuleConfig(){
   return m_dbConfigMgrConfig;
}

bool xGateMrcpCliAppService::stop(void)
{
   XGLOG_FUNCTION_EN_EX("");
   m_run = false;
   msg_queue()->deactivate();
   wait();
   XGLOG_INFO("xGateMrcpCliAppService stopped !");
   return true;
}

bool xGateMrcpCliAppService::init()
{
   XGLOG_FUNCTION_EN_EX("");
   XGLOG_INFO("xGateMrcpCliAppService::init() called");
   m_run = true;

   // Start the log thread.
   if(activate(THR_NEW_LWP, 1) == -1 ) {
      XGLOG_ERROR("xGateMrcpCliAppService::init() failed to start DbConfigService thread !");
      return false;
   } else {
      XGLOG_INFO("xGateMrcpCliAppService::init() success !");
   }

   m_mrcpCliAppHandler = new xGateMrcpCliAppHandler();
   if (!m_mrcpCliAppHandler){
       XGLOG_ERROR("xGateMrcpCliAppService::init() failed to create xGateMrcpCliAppHandler!");
       return false;
   }

   return m_mrcpCliAppHandler->init();
}

int xGateMrcpCliAppService::pushModuleMsg(IURModuleMsg * pdbconfigMsg)
{
   XGLOG_FUNCTION_EN_EX("");
   XGLOG_INFO("xGateMrcpCliAppService::inside pushModuleMsg");
   if (!pdbconfigMsg) {
      XGLOG_ERROR("pushModuleMsg::Message not received from controller!");
      return IURDefines::UR_RETURN_ERROR;
   }
   else if(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE == pdbconfigMsg->getDstModuleId()) {
      XGLOG_DEBUG("xGateMrcpCliAppService::Message received from controller to regservice");
      MRCP_CLI_APP_SERVICE->putq(dynamic_cast<ACE_Message_Block*>(pdbconfigMsg));
   }
   return IURDefines::UR_RETURN_SUCCESS;
}

int xGateMrcpCliAppService::svc(void)
{
   XGLOG_FUNCTION_EN_EX("");
   int rc;
   rc = pthread_setname_np(ACE_Thread::self(), "MrcpCliAppSrvc");
   if(rc!=0)
     printf("Failed to set name for dbconfig service thread");
   ACE_Message_Block *pAmb = 0;

   while(m_run)
   {
      ACE_Time_Value tv ((time(NULL) + 20));
      int result = getq(pAmb, &tv);
      if ((result == -1) && (errno == EWOULDBLOCK))
      {
	 continue;
      }
      if ((result != -1))
      {
	 XGLOG_INFO("xGateMrcpCliAppService::process handle_msg success !");
	 handle_msg(pAmb);
      }
      else
      {
	 XGLOG_ERROR("xGateMrcpCliAppService::svc Error in Message DbConfig");
	 m_run = false;
      }
      // delete the message received
      if(pAmb)
      {
	 delete pAmb;
	 pAmb = NULL;
      }
   }

   XGLOG_INFO("xGateMrcpCliAppService::svc() stopped");
   return 0;
}

bool xGateMrcpCliAppService::handle_msg(ACE_Message_Block *pAmb)
{
   xGateDbConfigMgrServiceMsg *mrcpServiceMsg = dynamic_cast<xGateDbConfigMgrServiceMsg*>(pAmb);
   //URMRCPBaseMsg *pMsg = dynamic_cast<URMRCPBaseMsg*>(pAmb);
   if(!mrcpServiceMsg) {
      XGLOG_ERROR("handle_msg failed. BaseMsg pointer is NULL !");
      return false;
   }

   return m_mrcpCliAppHandler->process_msg(mrcpServiceMsg);
   //return m_mrcpCliAppHandler->process_msg(pMsg);
}

#if 0
std::string CreateUniqueCallid()
{
  std::string UniqueCallid = "";

  std::srand(static_cast<unsigned int>(std::time(NULL)));
  ostringstream t_uuid;
  t_uuid << std::rand();
  UniqueCallid = t_uuid.str();

  XGLOG_INFO("xGateDbConfigMgr::CreateUniqueCallid(%s)",UniqueCallid.c_str());

  return UniqueCallid;
}
#endif
void xGateMrcpCliAppService::trigger_event()
{    
   XGLOG_FUNCTION_EN_EX("");

#if 0
   xGateSchedulerServiceMsg *pSchedulerMsg = new xGateSchedulerServiceMsg();
   if(!pSchedulerMsg){
      XGLOG_ERROR("URCBController::set_timer failed to create xGateSchedulerServiceMsg!");
      return;
   }

   std::string callId = CreateUniqueCallid();
   unsigned int t_timeoutSecond = 30;
   XGLOG_INFO("Timer is set for next (%d) seconds", t_timeoutSecond);

   SchedulerInfo *schedulerInfo = new SchedulerInfo();
   schedulerInfo->m_contextId = callId;
   schedulerInfo->m_schedulerType = EN_SCHEDULER_LWSS_DB_CONFIG_QUERY_TIMER;
   schedulerInfo->m_retryCount = 0;
//   schedulerInfo->m_timeoutSecond = this->m_lightWeightWSSProfile.m_dbConfigQueryTimer;
   schedulerInfo->m_timeoutSecond = t_timeoutSecond;
//   schedulerInfo->m_pCtxdata = (void*) pturcbTimerData;
   schedulerInfo->m_requestorModule = IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE;
   schedulerInfo->m_enumTimerFunc = SCHEDULER_FUNCTION_SET_TIMER;

   pSchedulerMsg->setUid(callId);
   pSchedulerMsg->setDstModuleId(IURDefines::UR_MODULE_SCHEDULER_SERVICE);
   pSchedulerMsg->setSrcModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
   pSchedulerMsg->set_scheduler_info(*schedulerInfo);
  // this->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(pSchedulerMsg));
   MRCP_CLI_APP_SERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(pSchedulerMsg));
   XGLOG_INFO("Timer is set for next DB Config Query for conext id(%s)", callId.c_str());
#endif
   return;
    
}




