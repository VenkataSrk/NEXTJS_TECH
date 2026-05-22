#include <fstream>
#include <stdexcept>
#include "xGateUtil.h"
#include "xglog.h"
#include "xGateDBHandler.h"
#include "xGateDBServiceMsg.h"
#include "xGateDBService.h"
#include "xGateBaseMsg.h"

xGateDBService::xGateDBService() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false),m_dbConfig(0)
{
   m_strModuleName ="DbServiceModule";
}

xGateDBService::~xGateDBService(void)
{
}

int xGateDBService::initModule(IURModuleConfig * phttpConfig)
{
   m_dbConfig = phttpConfig;
   init();
   return IURDefines::UR_RETURN_SUCCESS;
}

int xGateDBService::unitModule()
{
   stop();
   return IURDefines::UR_RETURN_SUCCESS;
}

void xGateDBService::getModuleName(std::string& moduleName){
    moduleName=m_strModuleName;
}

IURDefines::MODULE_ID xGateDBService::getModuleID(){
  return IURDefines::UR_MODULE_DB_SERVICE;
}

IURModuleConfig * xGateDBService::getModuleConfig(){
  return m_dbConfig;
}

bool xGateDBService::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("xGateDBService stopped !");
  return true;
}

bool xGateDBService::init()
{
  XGLOG_INFO("xGateDBService::init() called");
  m_run = true;

  // Start the http thread.
  if (activate(THR_NEW_LWP, 1) == -1 )
  {
    XGLOG_ERROR("xGateDBService::init() failed to start VMS controller thread !");
    return false;
  }
  else
  {
  DBHNDLR->init();
    XGLOG_INFO("*** xGateDBService::init() success DB module initialized ***");
  }
  return true;
}

int xGateDBService::svc(void)
{
  int rc;
  rc = pthread_setname_np(ACE_Thread::self(), "HttpSrvc");
  if(rc!=0)
    printf("Failed to set name for HTTP service thread");

  ACE_Message_Block *pAmb = 0;

  while(m_run)
  {
    ACE_Time_Value tv ((time(NULL) + 20));
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
      XGLOG_ERROR("xGateDBService::svc Error in Message Queue");
      m_run = false;
    }
    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_INFO("xGateDBService::svc() stopped");
  return 0;
}

bool xGateDBService::handle_msg(ACE_Message_Block *pAmb)
{
   xGateBaseMsg* pMsg = NULL;
   pMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
   if(pMsg  == NULL) {
      XGLOG_ERROR("xGateDBService::handle_msg recvd invalid xGateBaseMsg Message");
      return false;
   }
   DBHNDLR->process_msg(pMsg);

   return true;
}

int xGateDBService::pushModuleMsg(IURModuleMsg * pdbMsg)
{
     XGLOG_INFO("xGateDBService::inside pushModuleMsg");   
     if (!pdbMsg) {
          XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
          return IURDefines::UR_RETURN_ERROR;
       }
      else{
         XGLOG_DEBUG("xGateDBService::Message received from controller to httpservice");
         xGateDBService *dbService = DBSERVICE;
         dbService->putq(dynamic_cast<ACE_Message_Block*>(pdbMsg));
      }    
  return IURDefines::UR_RETURN_SUCCESS;
}

