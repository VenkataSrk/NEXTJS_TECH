#include "xglog.h"
#include "xGateBaseMsg.h"
#include "xGateMBCLinkService.h"
#include "MBCLinkTcpAcceptor.h"
#include "MBCLinkMsgProcessor.h"
#include "MBCLinkTcpReceiver.h"
#include "MBCLinkMsgDispatcher.h"
#include "UREngineConfig.h"

xGateMBCLinkService::xGateMBCLinkService() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false)
{
  m_strModuleName ="MBCLinkModule";
}

xGateMBCLinkService::~xGateMBCLinkService(void)
{
}

int xGateMBCLinkService::initModule(IURModuleConfig * pmediaConfig)
{
   m_mediaConfig = pmediaConfig;
   init();
   return IURDefines::UR_RETURN_SUCCESS;
}
int xGateMBCLinkService::unitModule()
{
   stop();
   return IURDefines::UR_RETURN_SUCCESS;
}
void xGateMBCLinkService::getModuleName(std::string& moduleName){
    moduleName=m_strModuleName;
}

IURDefines::MODULE_ID xGateMBCLinkService::getModuleID(){
  return IURDefines::UR_MODULE_MBC_LINK_SERVICE;
}

IURModuleConfig * xGateMBCLinkService::getModuleConfig(){
  return m_mediaConfig;
}

bool xGateMBCLinkService::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("xGateMBCLinkService stopped !");
  return true;
}

bool xGateMBCLinkService::init()
{
  XGLOG_INFO("xGateMBCLinkService::init() called");
  m_run = true;

  if(activate(THR_NEW_LWP, 1) == -1) {
    XGLOG_ERROR("MBCLinkService failed to start MBCLinkService thread !");
    return false;
  } else {
    //initialize MBCLinkTcpAcceptor
    vector<TcpLisentAddr> listenAddressList;
    TcpLisentAddr listenAddress;
    listenAddress.m_ip = CONFIGSERVICE->m_localIp4Addr;
    listenAddress.m_port = CONFIGSERVICE->m_mediaModuleProfile.mbcLinkTcpListenPort; 
    listenAddressList.push_back(listenAddress);
    if(MBCLINKTCPACCEPTOR->init(listenAddressList) != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_ERROR("MBCLinkService failed to intialize mbclink tcp acceptor !");
      return false;
    }

    //initialize MBCLinkTcpReceiver
    if(MBCLINKTCPRECEIVER->init() != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_ERROR("MBCLinkService failed to intialize mbclink tcp receiver !");
      return false;
    }

    //initialize MBCLinkMsgProcessor
    if(!MBCLINKMSGPROCESSOR->init()) {
      XGLOG_ERROR("MBCLinkService failed to initialize mbclink message processor !");
      return false;
    }

    //initialize MBCLinkDispatcher
    if(!MBCLINKMSGDISPATCHER->init()) {
      XGLOG_ERROR("MBCLinkService failed to initialize mbclink message disapatcher !");
      return false;
    }

#if 1
    sleep(1); //added for connection testing 
#endif
    XGLOG_INFO("xGateMBCLinkService successfully initialized all media-module threads !");
  }
  return true;
}

int xGateMBCLinkService::svc(void)
{
  ACE_Message_Block *pAmb = 0;

   int rc;
   rc = pthread_setname_np(ACE_Thread::self(), "MBCLinkSrvc");
   if(rc!=0)
     printf("Failed to set name for xGateMBCLinkService thread");

  while(m_run) {
    ACE_Time_Value tv((time(NULL) + 20));
    int result = getq(pAmb, &tv);
    if((result == -1) && (errno == EWOULDBLOCK)) {
      continue;
    }

    if((result != -1)) {
      handle_msg(pAmb);
    } else {
      XGLOG_ERROR("xGateMBCLinkService::svc Error in Message Queue");
      m_run = false;
    }

    // delete the message received
    if(pAmb) {
      delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_INFO("xGateMBCLinkService::svc() stopped");
  return 0;
}

bool xGateMBCLinkService::handle_msg(ACE_Message_Block *pAmb)
{
  xGateBaseMsg *pBaseMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
  if(!pBaseMsg) {
    XGLOG_ERROR("handle_msg failed. xGateBaseMsg pointer is NULL !");
    return false;
  }

  xGateMBCLinkServiceMsg *pMsg = dynamic_cast<xGateMBCLinkServiceMsg*>(pBaseMsg);
  if(!pMsg) {
    XGLOG_ERROR("handle_msg failed. xGateMBCLinkServiceMsg pointer is NULL !");
    return false;
  }
  return MBCLINKMSGDISPATCHER->process_msg(pMsg); 
}
int xGateMBCLinkService::pushModuleMsg(IURModuleMsg * pmediaMsg)
{
     XGLOG_INFO("xGateMBCLinkService::inside pushModuleMsg");
     if (!pmediaMsg) {
          XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
          return IURDefines::UR_RETURN_ERROR;
       }
      else{
         XGLOG_DEBUG("xGateMBCLinkService::Message received from controller to mediaservice");
          xGateMBCLinkService *mediaService = MBCLINKSERVICE;
           mediaService->putq(dynamic_cast<ACE_Message_Block*>(pmediaMsg));
      }
  return IURDefines::UR_RETURN_SUCCESS;

}

