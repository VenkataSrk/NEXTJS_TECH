#include <fstream>
#include <stdexcept>
#include "xGateUtil.h"
#include "xglog.h"
#include "xGateBaseMsg.h"
#include "xGateMBCService.h"
#include "xGateMBCUtil.h"
#include "xGateMBCServiceMsg.h"

#define REC_ENABLED

xGateMBCService::xGateMBCService() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false),m_mediaBroadcastConfig(0)
{
   m_strModuleName ="xGateMBCServiceModule";
}

xGateMBCService::~xGateMBCService(void)
{
}

int xGateMBCService::initModule(IURModuleConfig * ptmediaBroadcastConfig)
{
   m_mediaBroadcastConfig = ptmediaBroadcastConfig;
   init();
   return IURDefines::UR_RETURN_SUCCESS;
}
int xGateMBCService::unitModule()
{
   stop();
   return IURDefines::UR_RETURN_SUCCESS;
}
void xGateMBCService::getModuleName(std::string& moduleName){
    moduleName=m_strModuleName;
}

IURDefines::MODULE_ID xGateMBCService::getModuleID(){
  return IURDefines::UR_MODULE_XG_MBC_SERVICE;
}

IURModuleConfig * xGateMBCService::getModuleConfig(){
  return m_mediaBroadcastConfig;
}

bool xGateMBCService::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("xGateMBCService stopped !");
  return true;
}

bool xGateMBCService::init()
{
  XGLOG_INFO("xGateMBCService::init() called");
  m_run = true;

  // Start the mbc thread.
  if (activate(THR_NEW_LWP, 1) == -1 )
  {
    XGLOG_ERROR("xGateMBCService::init() failed to start MBC service thread !");
    return false;
  }
  else
  {
    XGLOG_INFO("xGateMBCService::init() success !");
  }

  init_mbc();
  initClientConnectionRequest();
  initTcpListenRequest();
#ifdef REC_ENABLED
  initRecordingConnectionRequest();
#endif

  return true;
}

int xGateMBCService::svc(void)
{
  //CLog::Warning(THISMODULE, "xGateMBCService::svc(%s)", m_serverType);
  ACE_Message_Block *pAmb = 0;

  while(m_run)
  {
    int result = getq(pAmb, NULL);
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
      XGLOG_ERROR("xGateMBCService::svc Error in Message Queue");
      m_run = false;
    }
  }

  XGLOG_INFO("xGateMBCService::svc() stopped");
  return 0;
}

bool xGateMBCService::handle_msg(ACE_Message_Block *pAmb)
{
  if(pAmb != NULL) {
    xGateBaseMsg* pMsg = NULL;
    pMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
    if(pMsg  == NULL) {
      XGLOG_ERROR("xGateMBCService::handle_msg recvd invalid xGateBaseMsg Message");
      return false;
    }
    xGateMBCServiceMsg *pmediaBCastMsg = dynamic_cast<xGateMBCServiceMsg*>(pAmb);
    switch(pmediaBCastMsg->getMsgType())
    {
     case EN_XGATE_MSG_CLIENT_CONNECTED:
     {
       XGLOG_INFO("xGateMBCService::handle_msg recvd msg EN_XGATE_MSG_CLIENT_CONNECTED");
       int connId = pmediaBCastMsg->getConnectionId(); 
       MBCTcpConnInfo mbcTcpConnInfo = pmediaBCastMsg->get_mbc_tcp_conn_info();
       mbcTcpConnInfo.m_mbcClientType = EN_XG_MBC_CLIENT_PBX;
       xGateMBCUtil::addToMbcConnectionMap(connId,mbcTcpConnInfo);
       break;
     }
     case EN_XGATE_MSG_NEW_CLIENT:
     {
       XGLOG_INFO("xGateMBCService::handle_msg recvd msg EN_XGATE_MSG_NEW_CLIENT");
       int connId = pmediaBCastMsg->getConnectionId();
       MBCTcpConnInfo mbcTcpConnInfo = pmediaBCastMsg->get_mbc_tcp_conn_info();
       mbcTcpConnInfo.m_mbcClientType = EN_XG_MBC_CLIENT_SFU;
       xGateMBCUtil::addToMbcConnectionMap(connId,mbcTcpConnInfo);
       break;
     }    
     case EN_XGATE_MSG_REC_CLIENT_CONNECTED:
     {
       XGLOG_INFO("xGateMBCService::handle_msg recvd msg EN_XGATE_MSG_REC_CLIENT_CONNECTED");
       int connId = pmediaBCastMsg->getConnectionId();
       MBCTcpConnInfo mbcTcpConnInfo = pmediaBCastMsg->get_mbc_tcp_conn_info();
       mbcTcpConnInfo.m_mbcClientType = EN_XG_MBC_REC_CLIENT;
       xGateMBCUtil::addToMbcConnectionMap(connId,mbcTcpConnInfo);
       break;
     }    
     case EN_XGATE_MSG_RECV:
     {
       XGLOG_INFO("xGateMBCService::handle_msg recvd msg EN_XGATE_MSG_RECV");
       handle_IncomingMBCMsg(pmediaBCastMsg);
       return true;
     }
     case EN_XGATE_MSG_CONN_CLOSED:
     {
       XGLOG_INFO("xGateMBCService::handle_msg recvd msg EN_XGATE_MSG_CONN_CLOSED for pid:%d",getpid());
       int connId = pmediaBCastMsg->getConnectionId();
       MBCTcpConnInfo mbcTcpConnInfo;
       xGateMBCUtil::getConnInfoFromMbcConnectionMap(connId,mbcTcpConnInfo);
       xGateMBCClientType mbcClientType = mbcTcpConnInfo.m_mbcClientType;
       switch(mbcClientType) {
         case EN_XG_MBC_CLIENT_PBX:
           {
             XGLOG_INFO("xGateMBCService::handle_msg recvd msg EN_XGATE_MSG_CONN_CLOSED from PBX");
             MBCTcpConnInfo mbcTcpConnInfo = pmediaBCastMsg->get_mbc_tcp_conn_info();
             reconnectClient(mbcTcpConnInfo);
             break;
           }
         case EN_XG_MBC_CLIENT_SFU:
           {
             XGLOG_INFO("xGateMBCService::handle_msg recvd msg EN_XGATE_MSG_CONN_CLOSED from SFU");
             int sfuId(0);
             xGateMBCUtil::get_sfuId_from_sfuConnId(connId,sfuId);
						 if(sfuId < 0){
						 	 XGLOG_ERROR("SfuId Not present in sfuConnId map, might be Release Message received and removed from Map");
	     			 	 return true;
						 }
             xGateMBCUtil::removePidFromMgSfuConnectionMap(sfuId);
             xGateMBCUtil::removeSfuIdFromResIdMap(sfuId);
             xGateMBCUtil::removeSfuIdFromPbxConnIdMap(sfuId);
             xGateMBCUtil::removeSfuIdFromSfuConnIdMap(sfuId);
             xGateMBCUtil::removeSfuConnIdFromPbxConnIdMap(connId);
	     			 pmediaBCastMsg->setSfuId(sfuId);
       	     handle_IncomingMBCMsg(pmediaBCastMsg);
	     			 return true;
           }
         case EN_XG_MBC_REC_CLIENT:
           {
             XGLOG_INFO("xGateMBCService::handle_msg recvd msg EN_XGATE_MSG_CONN_CLOSED from REC Server");
             MBCTcpConnInfo mbcTcpConnInfo = pmediaBCastMsg->get_mbc_tcp_conn_info();
             xGateMBCUtil::removeFromMbcConnectionMap(connId);
             reconnectClient(mbcTcpConnInfo);
             break;
           }
         default:
           {
             XGLOG_ERROR("xGateMBCService::handle_msg recvd EN_XGATE_MSG_CONN_CLOSED from Unknown client");
             break;
           }
       }
      break;
     }
     default:
     {
       XGLOG_ERROR("xGateMBCService::handle_msg recvd no matching case");
       break;
     }
    }
    //Delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }
  return true;
}

bool xGateMBCService::init_mbc()
{
  MBCControllerProcessor *pmbcCtrlProcessor = new MBCControllerProcessor("MBCControllerProcessor");
  if(pmbcCtrlProcessor->init(1) != EN_XGATE_STATUS_SUCCESS)
  {
    cout << "MBCControllerProcessor initialized" << endl;
    return false;
  }
  xGateMBCUtil::setMBCctlProcessor(pmbcCtrlProcessor);
  //return true;

  ClientService *pmbcClientService = new ClientService("ClientService");
  if(pmbcClientService->init(1) != EN_XGATE_STATUS_SUCCESS)
  {
    cout << "ClientService initialized" << endl;
    return false;
  }
  xGateMBCUtil::setMBCClientService(pmbcClientService);
  return true;

}

bool xGateMBCService::reconnectClient(MBCTcpConnInfo & mbcTcpConnInfo)
{
  XGLOG_INFO("Initiating reconnection request with PBX");

  xGateMBCServiceMsg *ptmediaBCastMsg = new xGateMBCServiceMsg();
  ptmediaBCastMsg->setSrcModuleId(IURDefines::UR_MODULE_XG_MBC_SERVICE);
  ptmediaBCastMsg->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
  ptmediaBCastMsg->setMsgType(EN_XGATE_MSG_XG_MBC_MODULE);
  ptmediaBCastMsg->set_mbc_event_type(EN_XG_MBC_EVENT_INIT_CONNECTION_REQ);

  ptmediaBCastMsg->set_mbc_tcp_conn_info(mbcTcpConnInfo);
  XGMBCSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptmediaBCastMsg));
  return true;
}

bool xGateMBCService::initClientConnectionRequest()
{
  XGLOG_INFO("Initiating connection request with PBX");

  vector<TcpLisentAddr> &peerConnectAddrs = CONFIGSERVICE->m_mediaModuleProfile.m_peerConnectAddrs;
  if (peerConnectAddrs.size () <= 0)
  {
    XGLOG_INFO ("TCP List connection Address vector size is 0");
    return false;
  }
  vector<TcpLisentAddr>::iterator itrConnAddr = peerConnectAddrs.begin();
  for(itrConnAddr; itrConnAddr != peerConnectAddrs.end(); itrConnAddr++)
  {
    xGateMBCServiceMsg *ptmediaBCastMsg = new xGateMBCServiceMsg();
    ptmediaBCastMsg->setSrcModuleId(IURDefines::UR_MODULE_XG_MBC_SERVICE);
    ptmediaBCastMsg->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
    ptmediaBCastMsg->setMsgType(EN_XGATE_MSG_XG_MBC_MODULE);
    ptmediaBCastMsg->set_mbc_event_type(EN_XG_MBC_EVENT_INIT_CONNECTION_REQ);

    MBCTcpConnInfo mbcTcpConnInfo;
    mbcTcpConnInfo.m_ip = (*itrConnAddr).m_ip;
    mbcTcpConnInfo.m_port = (*itrConnAddr).m_port;
    ptmediaBCastMsg->set_mbc_tcp_conn_info(mbcTcpConnInfo);
    XGMBCSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptmediaBCastMsg));
  }

  return true;
}

bool xGateMBCService::initRecordingConnectionRequest()
{
  XGLOG_INFO("Initiating connection request with Recording Server");

  xGateMBCServiceMsg *ptmediaBCastMsg = new xGateMBCServiceMsg();
  ptmediaBCastMsg->setSrcModuleId(IURDefines::UR_MODULE_XG_MBC_SERVICE);
  ptmediaBCastMsg->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
  ptmediaBCastMsg->setMsgType(EN_XGATE_MSG_XG_MBC_MODULE);
  ptmediaBCastMsg->set_mbc_event_type(EN_XG_MBC_EVENT_INIT_CONNECTION_REQ);

  vector<TcpLisentAddr> &peerConnectAddrs = CONFIGSERVICE->m_recordingProfile.m_peerConnectAddrs;
  vector<TcpLisentAddr>::iterator itrConnAddr = peerConnectAddrs.begin();
  if(itrConnAddr != peerConnectAddrs.end()){
    MBCTcpConnInfo mbcTcpConnInfo;
    mbcTcpConnInfo.m_ip = (*itrConnAddr).m_ip;
    mbcTcpConnInfo.m_port = (*itrConnAddr).m_port;

    ptmediaBCastMsg->set_mbc_tcp_conn_info(mbcTcpConnInfo);
    XGMBCSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptmediaBCastMsg));
  }else{
    XGLOG_INFO("Initiating connection request with Recording Server failed PeerConnAddr is empty");
  }

  return true;
}

bool xGateMBCService::initTcpListenRequest()
{
  XGLOG_INFO("Initiating listen request for MBC_SFU communication");

  xGateMBCServiceMsg *ptmediaBCastMsg = new xGateMBCServiceMsg();
  ptmediaBCastMsg->setSrcModuleId(IURDefines::UR_MODULE_XG_MBC_SERVICE);
  ptmediaBCastMsg->setDstModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
  ptmediaBCastMsg->setMsgType(EN_XGATE_MSG_XG_MBC_MODULE);
  ptmediaBCastMsg->set_mbc_event_type(EN_XG_MBC_EVENT_INIT_LISTEN_REQ);
  MBCTcpConnInfo mbcTcpConnInfo;
  mbcTcpConnInfo.m_ip = CONFIGSERVICE->m_localIp4Addr;
  mbcTcpConnInfo.m_port = SFU_LISTEN_PORT;
  ptmediaBCastMsg->set_mbc_tcp_conn_info(mbcTcpConnInfo);
  XGMBCSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptmediaBCastMsg));
  return true;
}

bool xGateMBCService::handle_IncomingMBCMsg(xGateMBCServiceMsg *pmediaBCastMsg)
{
  if(!pmediaBCastMsg) {
    XGLOG_ERROR("xGateMBCService::handle_IncomingMBCMsg recvd invalid xGateMBCServiceMsg");
    return false;
  }
  int connId = pmediaBCastMsg->getConnectionId();
  XGLOG_INFO("xGateMBCService::inside handle_IncomingMBCMsg with connId:%d",connId);	
  MBCTcpConnInfo mbcTcpConnInfo;
  xGateMBCUtil::getConnInfoFromMbcConnectionMap(connId,mbcTcpConnInfo);
  xGateMBCClientType mbcClientType = mbcTcpConnInfo.m_mbcClientType;

  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  ACE_Message_Block *pAmb = static_cast<ACE_Message_Block*>(pmediaBCastMsg);

  switch(mbcClientType)
  {
    case EN_XG_MBC_CLIENT_PBX:
      {
        XGLOG_INFO("xGateMBCService::inside handle_IncomingMBCMsg clientType:EN_XG_MBC_CLIENT_PBX");	
	MBCControllerProcessor *pmbcCtrlProcessor = xGateMBCUtil::getMBCctlProcessor();
	if(!pmbcCtrlProcessor) {
	  XGLOG_ERROR("xGateMBCService::handle_IncomingMBCMsg Failed to post msg to MBCControllerProcessor");
	  return false;
	}

	if(pmbcCtrlProcessor->putq(pAmb,&tval)) {
	  XGLOG_INFO("xGateMBCService::handle_IncomingMBCMsg posted msg to MBCControllerProcessor");
	} else {
	  XGLOG_ERROR("xGateMBCService::handle_IncomingMBCMsg failed to send msg");
	  return false;
	}
	break;       
      }
    case EN_XG_MBC_CLIENT_SFU:
      {
        XGLOG_INFO("xGateMBCService::inside handle_IncomingMBCMsg clientType:EN_XG_MBC_CLIENT_SFU");	
	ClientService *pClientSrvc = xGateMBCUtil::getMBCClientService();
	if(!pClientSrvc) {
	  XGLOG_ERROR("xGateMBCService::handle_IncomingMBCMsg Failed to post msg to ClientService!");
	  return false;
	}

	if(pClientSrvc->putq(pAmb,&tval)) {
	  XGLOG_INFO("xGateMBCService::handle_IncomingMBCMsg posted msg to ClientService");
	} else {
	  XGLOG_ERROR("xGateMBCService::handle_IncomingMBCMsg failed to send msg");
	  return false;
	}
	break;
      }
#ifdef REC_ENABLED
    case EN_XG_MBC_REC_CLIENT:
      {
        XGLOG_INFO("xGateMBCService::inside handle_IncomingMBCMsg clientType:EN_XG_MBC_REC_CLIENT");	
	MBCControllerProcessor *pmbcCtrlProcessor = xGateMBCUtil::getMBCctlProcessor();
	if(!pmbcCtrlProcessor) {
	  XGLOG_ERROR("xGateMBCService::handle_IncomingMBCMsg Failed to post msg to MBCControllerProcessor");
	  return false;
	}

	if(pmbcCtrlProcessor->putq(pAmb,&tval)) {
	  XGLOG_INFO("xGateMBCService::handle_IncomingMBCMsg posted msg to MBCControllerProcessor");
	} else {
	  XGLOG_ERROR("xGateMBCService::handle_IncomingMBCMsg failed to send msg");
	  return false;
	}
        break;
      }
#endif
    default:
      {
        XGLOG_ERROR("xGateMBCService::handle_IncomingMBCMsg failed to handle unknown client type");
        break;
      }
  }
  return true;
}

int xGateMBCService::pushModuleMsg(IURModuleMsg * ptmediaBCastMsg)
{
  XGLOG_INFO("xGateMBCService::inside pushModuleMsg");   
  if (!ptmediaBCastMsg) {
    XGLOG_ERROR("pushModuleMsg::Message not received from controller.!");
    return IURDefines::UR_RETURN_ERROR;
  }
  else{
    XGLOG_DEBUG("xGateMBCService::Message received from controller to XGMBCSERVICE");
    xGateMBCService *mediaBroadcastService = XGMBCSERVICE;
    mediaBroadcastService->putq(dynamic_cast<ACE_Message_Block*>(ptmediaBCastMsg));
  }    
  return IURDefines::UR_RETURN_SUCCESS;
}

