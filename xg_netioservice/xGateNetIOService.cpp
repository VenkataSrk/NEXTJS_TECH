#include "xglog.h"
#include "xGateBaseMsg.h"
#include "xGateNetIOService.h"
#include "TcpAcceptor.h"
#include "TcpConnector.h"
#include "NetIOMsgProcessor.h"
#include "TcpReceiver.h"
#include "NetIOMsgDispatcher.h"
//#include "IvrConfig.h"
#include "UREngineConfig.h"

xGateNetIOService::xGateNetIOService() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false)
{
  m_strModuleName ="NetIOServiceModule";
}

xGateNetIOService::~xGateNetIOService(void)
{
}

int xGateNetIOService::initModule(IURModuleConfig * pnetioConfig)
{
   m_netioConfig = pnetioConfig;
   init();
   return IURDefines::UR_RETURN_SUCCESS;
}
int xGateNetIOService::unitModule()
{
   stop();
   return IURDefines::UR_RETURN_SUCCESS;
}
void xGateNetIOService::getModuleName(std::string& moduleName){
    moduleName=m_strModuleName;
}

IURDefines::MODULE_ID xGateNetIOService::getModuleID(){
  return IURDefines::UR_MODULE_NETIO_SERVICE;
}

IURModuleConfig * xGateNetIOService::getModuleConfig(){
  return m_netioConfig;
}

bool xGateNetIOService::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("xGateNetIOService stopped !");
  return true;
}

bool xGateNetIOService::init()
{
  XGLOG_INFO("xGateNetIOService::init() called");
  m_run = true;
  if(activate(THR_NEW_LWP, 1) == -1) {
    XGLOG_ERROR("MediaService failed to start MediaService thread !");
    return false;
  } else {
#if 0
    //initialize MediaTcpAcceptor
    vector<TcpLisentAddr> listenAddressList;
    TcpLisentAddr listenAddress;
    listenAddress.m_ip = CONFIGSERVICE->m_localIp4Addr;
    listenAddress.m_port = CONFIGSERVICE->m_mediaModuleProfile.mediaTcpListenPort; 
    listenAddressList.push_back(listenAddress);
    if(TCPACCEPTOR->init(listenAddressList) != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_ERROR("MediaService failed to intialize media tcp acceptor !");
      return false;
    }
#endif

    //initialize TcpConnector
    if(TCPCONNECTOR->init() != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_ERROR("NetIOService failed to intialize tcp receiver !");
      return false;
    }

    if(TCPCONNECTOR->connect_to_peer() != EN_XGATE_STATUS_SUCCESS){
      XGLOG_ERROR("NetIOService failed to connect to peer");
      return false;
    }

    //initialize TcpReceiver
    if(TCPRECEIVER->init() != EN_XGATE_STATUS_SUCCESS) {
      XGLOG_ERROR("NetIOService failed to intialize tcp receiver !");
      return false;
    }

    //initialize NetMgrMsgProcessor
    if(!NETIOMSGPROCESSOR->init()) {
      XGLOG_ERROR("NetIOService failed to initialize message processor !");
      return false;
    }

    //initialize NetMgrMsgDispatcher
    if(!NETIOMSGDISPATCHER->init()) {
      XGLOG_ERROR("MediaService failed to initialize message disapatcher !");
      return false;
    }
    XGLOG_INFO("xGateNetIOService successfully initialized all media-module threads !");
  }
  return true;
}

int xGateNetIOService::svc(void)
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
      XGLOG_ERROR("xGateNetIOService::svc Error in Message Queue");
      m_run = false;
    }

    // delete the message received
    if(pAmb) {
      delete pAmb;
      pAmb = nullptr;
    }
  }

  XGLOG_INFO("xGateNetIOService::svc() stopped");
  return 0;
}

bool xGateNetIOService::handle_msg(ACE_Message_Block *pAmb)
{
	xGateBaseMsg *pBaseMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
	if(!pBaseMsg) {
		XGLOG_ERROR("handle_msg failed. xGateBaseMsg pointer is nullptr !");
		return false;
	}

	xGateNetIOServiceMsg *pMsg = dynamic_cast<xGateNetIOServiceMsg*>(pBaseMsg);
	if(!pMsg) {
		XGLOG_ERROR("handle_msg failed. xGateNetIOServiceMsg pointer is nullptr !");
		return false;
	}

	switch(pMsg->getNetIOMsgType())
	{
		case EN_XGATE_MSG_CONNECT_CLIENT:
			{
				CreateTcpClient(pMsg);
				break;
			}
		case EN_XGATE_MSG_LISTEN:
			{
				CreateTcpListener(pMsg); 
				break;
			}
		case EN_XGATE_MSG_SEND:
			{
				SendMessageToSocket(pMsg);
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
}

bool xGateNetIOService::CreateTcpClient(xGateNetIOServiceMsg *pNetIOMsg)
{
	xGateNetConnection netConInfo = pNetIOMsg->get_netConnInfo();
	TCPCONNECTOR->add_new_client(netConInfo.m_IpAdd,netConInfo.m_Port,pNetIOMsg->getSrcModuleId());
	XGLOG_INFO("Recvd client connection request");
	return true;
}

bool xGateNetIOService::CreateTcpListener(xGateNetIOServiceMsg *pNetIOMsg)
{
	return true;
}

bool xGateNetIOService::SendMessageToSocket(xGateNetIOServiceMsg *pNetIOMsg)
{
	ACE_Time_Value tval ((time(nullptr) ));
	tval += ACE_Time_Value(0,1);
	ACE_Message_Block *pAmb = dynamic_cast<ACE_Message_Block*>(pNetIOMsg);
	NETIOMSGDISPATCHER->process_msg(pNetIOMsg);
	return true;
}

int xGateNetIOService::pushModuleMsg(IURModuleMsg *pnetiopMsg)
{
     XGLOG_INFO("xGateNetIOService::inside pushModuleMsg");
     if (!pnetiopMsg) {
          XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
          return IURDefines::UR_RETURN_ERROR;
       }
      else{
         XGLOG_DEBUG("xGateNetIOService::Message received from controller to netioservice");
          xGateNetIOService *netioService = NETIOSERVICE;
           netioService->putq(dynamic_cast<ACE_Message_Block*>(pnetiopMsg));
      }
  return IURDefines::UR_RETURN_SUCCESS;

}

