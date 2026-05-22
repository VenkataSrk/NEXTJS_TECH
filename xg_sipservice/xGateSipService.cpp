#include "xglog.h"
#include "xGateBaseMsg.h"
#include "xGateSipService.h"
#include "xGateSipSignalReceiver.h"
#include "xGateSipSignalProcessor.h"

//DebugVM start
#define EXIT_NORMAL         0   // shutdown properly
#define EXIT_INIT_ERROR     1   // error in initialization
#define EXIT_NO_MEM         2   // new_bad_alloc exception
#define EXIT_ERROR          3   // unexpected run-time error (unhandled exceptions, etc)
#define EXIT_RESTART_REQ    4
//DebugVM end

xGateSipService::xGateSipService() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false)
{
  m_strModuleName="SipServiceModule";
}

xGateSipService::~xGateSipService(void)
{
}

int xGateSipService::initModule(IURModuleConfig * psipConfig)
{
   m_sipConfig = psipConfig;
   init();
   return IURDefines::UR_RETURN_SUCCESS;
}
int xGateSipService::unitModule()
{
   stop();
   return IURDefines::UR_RETURN_SUCCESS;
}
void xGateSipService::getModuleName(std::string& moduleName){
    moduleName=m_strModuleName;
}


void SetSipPostHandler(void *Handler,XGATEPOSTTYPE type){
    switch(type){
        case XGATEPOSTTYPE::EN_DISPATCH:
        SipDispatcher = Handler;
        break;
        case XGATEPOSTTYPE::EN_PROCESS:
        SipProcessor = Handler;
        break;
        case XGATEPOSTTYPE::EN_DBMANAGER:
        DBManager = Handler;
        default:
        break;
    }
}

void *GetSipPostHandler(XGATEPOSTTYPE type){
    void *handler = nullptr;
    switch(type){
        case XGATEPOSTTYPE::EN_DISPATCH:
        handler = SipDispatcher;
        break;
        case XGATEPOSTTYPE::EN_PROCESS:
        handler = SipProcessor;
        break;
        case XGATEPOSTTYPE::EN_DBMANAGER:
        handler = DBManager;
        break;
        default:
        handler = nullptr;
        break;
    }
    return handler;
}


bool xGateSipService::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("xGateSipService stopped !");
  return true;
}

bool xGateSipService::init()
{
  XGLOG_INFO("xGateSipService::init() called");
  m_run = true;
#if 0
  if(activate(THR_NEW_LWP, 1) == -1) {
    XGLOG_ERROR("init() failed to start sip service thread !");
    return false;
  } else {
    //TODO: initialize sip-stack
    xGateSipSignalReceiver *pSipSignalReceiver =  new xGateSipSignalReceiver("SipReceiver",nullptr);
    if(pSipSignalReceiver->init() != 0){
      return EXIT_INIT_ERROR;
    }

    xGateSipSignalDispatcher *pSipSignalDispatcher =  new xGateSipSignalDispatcher();
    if(pSipSignalDispatcher->init() !=  EN_XGATE_STATUS_SUCCESS){
      return EXIT_INIT_ERROR;
    }
    SetSipPostHandler((void*)pSipSignalDispatcher,XGATEPOSTTYPE::EN_DISPATCH);
    
    xGateSipSignalProcessor *pSipSignalProcessor =  new xGateSipSignalProcessor();
    if(pSipSignalProcessor->init(1) !=  EN_XGATE_STATUS_SUCCESS){
       return EXIT_INIT_ERROR;
    }
    SetSipPostHandler((void*)pSipSignalProcessor,XGATEPOSTTYPE::EN_PROCESS);

    XGLOG_INFO("xGateSipService::init() success !");
  }
#endif
  return true;
}

int xGateSipService::svc(void)
{
  ACE_Message_Block *pAmb = 0;

  while(m_run) {
    ACE_Time_Value tv((time(nullptr) + 20));
    int result = getq(pAmb, &tv);
    if((result == -1) && (errno == EWOULDBLOCK)) {
      continue;
    }

    if((result != -1)) {
      handle_msg(pAmb);
    } else {
      XGLOG_ERROR("xGateSipService::svc Error in Message Queue");
      m_run = false;
    }

    // delete the message received
    if(pAmb) {
      delete pAmb;
      pAmb = nullptr;
    }
  }

  XGLOG_INFO("xGateSipService::svc() stopped");
  return 0;
}

bool xGateSipService::handle_msg(ACE_Message_Block *pAmb)
{
  xGateBaseMsg *pBaseMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
  if(!pBaseMsg) {
    XGLOG_ERROR("handle_msg failed. xGateBaseMsg pointer is nullptr !");
    return false;
  }
#if 0 //TODO: need to implement
  xGateSipServiceMsg *pMsg = dynamic_cast<xGateSipServiceMsg*>(pBaseMsg);
  if(!pMsg) {
    XGLOG_ERROR("handle_msg failed. xGateSipServiceMsg pointer is nullptr !");
    return false;
  }
  return MEDIAMSGDISPATCHER->process_msg(pMsg); 
#endif
  return true;
}

int xGateSipService::pushModuleMsg(IURModuleMsg * psipMsg)
{
     XGLOG_INFO("xGateSipService::inside pushModuleMsg");
     if (!psipMsg) {
          XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
          return IURDefines::UR_RETURN_ERROR;
       }
      else{
         XGLOG_DEBUG("xGateSipService::Message received from controller to sipservice");
          xGateSipService *sipService = SIPSERVICE;
           sipService->putq(dynamic_cast<ACE_Message_Block*>(psipMsg));
      }
  return IURDefines::UR_RETURN_SUCCESS;
}


