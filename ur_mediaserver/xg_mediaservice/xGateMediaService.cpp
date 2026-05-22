#include "xglog.h"
#include "xGateBaseMsg.h"
#include "xGateMediaService.h"
#include "MediaTcpConnector.h"
#include "MediaTcpAcceptor.h"
#include "MediaMsgProcessor.h"
#include "MediaTcpReceiver.h"
#include "MediaMsgDispatcher.h"
#include "xGateUtil.h"
#include "XGConfig.h"

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

#if 0 //For mbc or mg initializing TcpAcceptor is not required
    //initialize MediaTcpAcceptor
    vector<TcpLisentAddr> listenAddressList;
    TcpLisentAddr listenAddress;
    listenAddress.m_ip = CONFIGSERVICE->m_localIp4Addr;
    listenAddress.m_port = CONFIGSERVICE->m_mediaModuleProfile.tcpListenPort; 
    listenAddressList.push_back(listenAddress);
    if(MEDIATCPACCEPTOR->init(listenAddressList) != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_ERROR("MediaService failed to intialize media tcp acceptor !");
      return false;
    }
#else 
    //initialize MediaTcpConnector
    //vector<TcpLisentAddr> &peerConnectAddrs = CONFIGSERVICE->m_mediaModuleProfile.m_peerConnectAddrs;
    if(MEDIATCPCONNECTOR->init() != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_ERROR("MediaService failed to intialize media tcp connector !");
      return false;
    }
    if(MEDIATCPCONNECTOR->connect_to_peer() != EN_XGATE_STATUS_SUCCESS){
      XGLOG_ERROR("MediaService failed to connect to peer");
      return false;
    }
#endif

    //initialize MediaTcpReceiver
    if(MEDIATCPRECEIVER->init() != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_ERROR("MediaService failed to intialize media tcp receiver !");
      return false;
    }

    //initialize MediaMsgProcessor
    if(!MEDIATCPACCEPTOR->init()) {
      XGLOG_ERROR("MediaService failed to initialize media message processor !");
      return false;
    }

    //initialize MediaMsgDispatcher
    if(!MEDIAMSGDISPATCHER->init()) {
      XGLOG_ERROR("MediaService failed to initialize media message disapatcher !");
      return false;
    }

    XGLOG_INFO("xGateMediaService successfully initialized all media-module threads !");
  }
  return true;
}

int xGateMediaService::svc(void)
{
  ACE_Message_Block *pAmb = 0;

  while(m_run) {
    int result = getq(pAmb, NULL);
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
      //delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_INFO("xGateMediaService::svc() stopped");
  return 0;
}

bool xGateMediaService::handle_msg(ACE_Message_Block *pAmb)
{
  XGLOG_TRACE("handle_msg xGateBaseMsg xGateMediaService");
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
  switch((xGateMSMsgType)pMsg->getMsgType())
  {
    case EN_XGATE_MSG_CONNECT_CLIENT:
    { 
      xGateNetConnection netConInfo = pMsg->get_netConnInfo();	    
      XGLOG_INFO("Recvd client connection request from %s", netConInfo.recvIp_.c_str());
      MEDIATCPCONNECTOR->add_new_client(netConInfo.recvIp_,netConInfo.recvPort_,pMsg->getSrcModuleId());
      delete pMsg;
      pMsg = NULL;
      break;
    }	   
    case EN_XGATE_MSG_LISTEN:
    {
      XGLOG_INFO("Recv listen request");
      xGateNetConnection netConInfo = pMsg->get_netConnInfo();
      TcpLisentAddr listenAddress;
      listenAddress.m_port = netConInfo.recvPort_;
      listenAddress.m_ip = netConInfo.recvIp_;
      MEDIATCPACCEPTOR->add_new_server(listenAddress.m_ip,listenAddress.m_port,pMsg->getSrcModuleId());
      break;
    }
    case EN_XGATE_MSG_SEND:
    {
      ACE_Time_Value tval ((time(NULL) ));
      tval += ACE_Time_Value(0,1);
      ACE_Message_Block *pAmb = dynamic_cast<ACE_Message_Block*>(pMsg); 
      if(MEDIAMSGDISPATCHER->putq(pAmb,&tval)){
       }
       else
       {
	 XGLOG_ERROR("MediaService failed to send msg");
         return false;	 
       }	       
      break;      
    }	  
    default:
    {
     XGLOG_ERROR("Invalid request recvd from src Module");
     return false;
     break; 
    } 	    
  }
  return true;
  //return MEDIAMSGDISPATCHER->process_msg(pMsg); 
}

int xGateMediaService::pushModuleMsg(IURModuleMsg * pmediaMsg)
{
     if (!pmediaMsg) {
          XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
          return IURDefines::UR_RETURN_ERROR;
       }
      else{
         XGLOG_TRACE("xGateMediaService::Message received from controller to mediaservice");
          xGateMediaService *mediaService = MEDIASERVICE;
           mediaService->putq(dynamic_cast<ACE_Message_Block*>(pmediaMsg));
      }
  return IURDefines::UR_RETURN_SUCCESS;

}

