#include <fstream>
#include <stdexcept>
#include "xGateUtil.h"
#include "xglog.h"
#include "xGateSchedulerService.h"
#include "xGateSchedulerHandler.h"
#include "xGateSchedulerServiceMsg.h"

xGateSchedulerService::xGateSchedulerService() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false),m_schedulerConfig(0)
{
   m_strModuleName ="SchedulerServiceModule";
}

xGateSchedulerService::~xGateSchedulerService(void)
{}

int xGateSchedulerService::initModule(IURModuleConfig * pschedulerConfig)
{
   m_schedulerConfig = pschedulerConfig;
   init();
   return IURDefines::UR_RETURN_SUCCESS;
}
int xGateSchedulerService::unitModule()
{
   stop();
   return IURDefines::UR_RETURN_SUCCESS;
}
void xGateSchedulerService::getModuleName(std::string& moduleName){
    moduleName=m_strModuleName;
}

IURDefines::MODULE_ID xGateSchedulerService::getModuleID(){
  return IURDefines::UR_MODULE_SCHEDULER_SERVICE;
}

IURModuleConfig * xGateSchedulerService::getModuleConfig(){
  return m_schedulerConfig;
}

bool xGateSchedulerService::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("xGateSchedulerService stopped !");
  return true;
}

bool xGateSchedulerService::init()
{
  XGLOG_INFO("xGateSchedulerService::init() called");
  m_run = true;

  // Start the scheduler thread.
  if (activate(THR_NEW_LWP, 1) == -1 )
  {
    XGLOG_ERROR("xGateSchedulerService::init() failed to start VMS controller thread !");
    return false;
  }
  else
  {
    XGLOG_INFO("xGateSchedulerService::init() success !");
  }

    xGateSchedulerHandler *schedulerHandler = xGateSchedulerHandler::getInstance();
    if(!schedulerHandler) {
      XGLOG_ERROR("xGateSchedulerService::handle_msg failed. invalid schedulerhandler object !");
      return false;
    }
    if(!schedulerHandler->init()) {
      XGLOG_ERROR("xGateSchedulerService::init() failed while initializing 'schedulerHandler->init' !");
      return false;
   } else {
      XGLOG_INFO("xGateSchedulerService::init() successfully initialized 'schedulerHandler->init' !");
   }

  return true;
}

int xGateSchedulerService::svc(void)
{
  //CLog::Warning(THISMODULE, "xGateSchedulerService::svc(%s)", m_serverType);
  int rc;
  rc = pthread_setname_np(ACE_Thread::self(), "ScdulrSrvc");
  if(rc!=0)
    printf("Failed to set name for scheduler service thread");
  ACE_Message_Block *pAmb = 0;

  while(m_run)
  {
    ACE_Time_Value tv ((time(nullptr) + 20));
    int result = getq(pAmb, &tv);
    if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
    {
      continue;
    }
    if ((result != -1))
    {
      handle_msg(pAmb);
    }
    else
    {
      XGLOG_ERROR("xGateSchedulerService::svc Error in Message Queue");
      m_run = false;
    }
    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = nullptr;
    }
  }

  XGLOG_INFO("xGateSchedulerService::svc() stopped");
  return 0;
}

bool xGateSchedulerService::handle_msg(ACE_Message_Block *pAmb)
{
  if(pAmb != nullptr) {
    xGateBaseMsg* pMsg = nullptr;
    pMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
    if(pMsg  == nullptr) {
      XGLOG_ERROR("xGateSchedulerService::handle_msg recvd invalid xGateBaseMsg Message");
      return false;
    }

    xGateSchedulerHandler *schedulerHandler = xGateSchedulerHandler::getInstance();
    if(!schedulerHandler) {
      XGLOG_ERROR("xGateSchedulerService::handle_msg failed. invalid schedulerhandler object !");
      return false;
    }

    //send scheduler request
    xGateSchedulerServiceMsg *pSchedulerMsg = dynamic_cast<xGateSchedulerServiceMsg*>(pAmb);
    SchedulerInfo &schedulerInfo = pSchedulerMsg->get_scheduler_info();
    if(!schedulerHandler->doSchedulerRequest(schedulerInfo)) {
      XGLOG_ERROR("xGateSchedulerService::handle_msg failed. send scheduler request failed !");
      return false;
    }
  }

  return true;
}

int xGateSchedulerService::pushModuleMsg(IURModuleMsg * pschedulerMsg)
{
     XGLOG_INFO("xGateSchedulerService::inside pushModuleMsg");
     if (!pschedulerMsg) {
          XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
          return IURDefines::UR_RETURN_ERROR;
       }
      else{
         XGLOG_DEBUG("xGateSchedulerService::Message received from controller to schedulerservice");
          xGateSchedulerService *schedulerService = SCHEDULERSERVICE;
           schedulerService->putq(dynamic_cast<ACE_Message_Block*>(pschedulerMsg));
      }
  return IURDefines::UR_RETURN_SUCCESS;
}

