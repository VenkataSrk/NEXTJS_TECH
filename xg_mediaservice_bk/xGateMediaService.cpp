#include "xglog.h"
#include "xGateBaseMsg.h"
#include "xGateMediaService.h"
#include "MediaTcpAcceptor.h"
#include "MediaMsgProcessor.h"
#include "MediaTcpReceiver.h"
#include "MediaMsgDispatcher.h"
//#include "IvrConfig.h"
#include "UREngineConfig.h"

xGateMediaService::xGateMediaService() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false)
{
  m_strModuleName ="MediaServiceModule";
}

xGateMediaService::~xGateMediaService(void)
{
}

int xGateMediaService::initModule(IURModuleConfig * pmediaConfig)
{
   m_mediaConfig = pmediaConfig;
   init();
   return IURDefines::UR_RETURN_SUCCESS;
}
int xGateMediaService::unitModule()
{
   stop();
   return IURDefines::UR_RETURN_SUCCESS;
}
void xGateMediaService::getModuleName(std::string& moduleName){
    moduleName=m_strModuleName;
}

IURDefines::MODULE_ID xGateMediaService::getModuleID(){
  return IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE;
}

IURModuleConfig * xGateMediaService::getModuleConfig(){
  return m_mediaConfig;
}

bool xGateMediaService::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("xGateMediaService stopped !");
  return true;
}

bool xGateMediaService::init()
{
  XGLOG_INFO("xGateMediaService::init() called");
  m_run = true;

  if(activate(THR_NEW_LWP, 1) == -1) {
    XGLOG_ERROR("MediaService failed to start MediaService thread !");
    return false;
  } else {
    //initialize MediaTcpAcceptor
    vector<TcpLisentAddr> listenAddressList;
    TcpLisentAddr listenAddress;
    listenAddress.m_ip = CONFIGSERVICE->m_localIp4Addr;
    listenAddress.m_port = CONFIGSERVICE->m_mediaModuleProfile.mediaTcpListenPort; 
    listenAddressList.push_back(listenAddress);
    if(MEDIATCPACCEPTOR->init(listenAddressList) != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_ERROR("MediaService failed to intialize media tcp acceptor !");
      return false;
    }

    //initialize MediaTcpReceiver
    if(MEDIATCPRECEIVER->init() != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_ERROR("MediaService failed to intialize media tcp receiver !");
      return false;
    }

    //initialize MediaMsgProcessor
    if(!MEDIAMSGPROCESSOR->init()) {
      XGLOG_ERROR("MediaService failed to initialize media message processor !");
      return false;
    }

    //initialize MediaMsgDispatcher
    if(!MEDIAMSGDISPATCHER->init()) {
      XGLOG_ERROR("MediaService failed to initialize media message disapatcher !");
      return false;
    }

#if 1
    sleep(1); //added for connection testing 
#endif
    XGLOG_INFO("xGateMediaService successfully initialized all media-module threads !");
  }
  return true;
}

int xGateMediaService::svc(void)
{
  ACE_Message_Block *pAmb = 0;

  while(m_run) {
    ACE_Time_Value tv((time(NULL) + 20));
    int result = getq(pAmb, &tv);
    if((result == -1) && (errno == EWOULDBLOCK)) {
      continue;
    }

    if((result != -1)) {
      handle_msg(pAmb);
    } else {
      XGLOG_ERROR("xGateMediaService::svc Error in Message Queue");
      m_run = false;
    }

    // delete the message received
    if(pAmb) {
      delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_INFO("xGateMediaService::svc() stopped");
  return 0;
}

bool xGateMediaService::handle_msg(ACE_Message_Block *pAmb)
{
  xGateBaseMsg *pBaseMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
  if(!pBaseMsg) {
    XGLOG_ERROR("handle_msg failed. xGateBaseMsg pointer is NULL !");
    return false;
  }

  xGateMediaServiceMsg *pMsg = dynamic_cast<xGateMediaServiceMsg*>(pBaseMsg);
  if(!pMsg) {
    XGLOG_ERROR("handle_msg failed. xGateMediaServiceMsg pointer is NULL !");
    return false;
  }
  return MEDIAMSGDISPATCHER->process_msg(pMsg); 
}
int xGateMediaService::pushModuleMsg(IURModuleMsg * pmediaMsg)
{
     XGLOG_INFO("xGateMediaService::inside pushModuleMsg");
     if (!pmediaMsg) {
          XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
          return IURDefines::UR_RETURN_ERROR;
       }
      else{
         XGLOG_DEBUG("xGateMediaService::Message received from controller to mediaservice");
          xGateMediaService *mediaService = MEDIASERVICE;
           mediaService->putq(dynamic_cast<ACE_Message_Block*>(pmediaMsg));
      }
  return IURDefines::UR_RETURN_SUCCESS;

}

