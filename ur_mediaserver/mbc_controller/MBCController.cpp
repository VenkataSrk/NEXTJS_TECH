
#include "xGateBaseMsg.h"
#include "MBCController.h"
#include "xGateUtil.h"
#include "URModuleFactory.h"
#include "xGateMBCServiceMsg.h"
#include "xGateMediaServiceMsg.h"
#include "MediaServiceDefine.h"

MBCController::MBCController() :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false),m_localIpAddress("")
{
  arrayModules.resize(IURDefines::UR_MAX_LEN);
}

MBCController::~MBCController(void)
{
}

bool MBCController::stop(void)
{
  m_run = false;
  msg_queue()->deactivate();
  wait();
  XGLOG_INFO("MBCController stopped !");
  return true;
}

bool MBCController::init()
{
  XGLOG_INFO("init() called");
  m_run = true;

  // Start the log thread.
  if(activate(THR_NEW_LWP, 1) == -1) {
    XGLOG_ERROR("init() failed to start VMS controller thread !");
    return false;
  } else {
    XGLOG_INFO("init() success !");
  }

  m_localIpAddress = CONFIGSERVICE->m_localIp4Addr;

  if(initModule()) {
    XGLOG_INFO("MBCController initialized required service Modules successfully");
  } else {
    XGLOG_ERROR("MBCController failed to initialize required Service Modules !");
    return false;
  }

  return true;
}

int MBCController::initModule()
{
  //Initialize Media Proxy Module
  IURModule *pMediaService = NULL;
  URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE, pMediaService);
  if(!pMediaService) {
    XGLOG_ERROR("Initializing Media service/proxy module failed !");
    return false;
  }

  CONFIGSERVICE->setCallBack(this);
  arrayModules[pMediaService->getModuleID()] = pMediaService;
  if(!pMediaService->initModule(CONFIGSERVICE) == IURDefines::UR_RETURN_ERROR) {
    XGLOG_ERROR("init() failed while initializing 'MediaService' !");
    return false;
  } else {
    XGLOG_INFO("init() successfully initialized 'MediaService' !");
  }

  //Initialize Media Broadcast Module
  IURModule *pMediaBroadcastService = NULL ;
  URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_XG_MBC_SERVICE, pMediaBroadcastService);
  if(!pMediaBroadcastService){
    XGLOG_ERROR("Initializing Media Broadcast service module in vms controller failed !");
    return 0;
  }

  CONFIGSERVICE->setCallBack(this);
  arrayModules[pMediaBroadcastService->getModuleID()] = pMediaBroadcastService;

  if(!pMediaBroadcastService->initModule(CONFIGSERVICE)==IURDefines::UR_RETURN_ERROR) {
    XGLOG_ERROR("init() failed while initializing 'xGateMBCService' !");
    return 0;
  } else {
    XGLOG_INFO("init() successfully initialized 'xGateMBCService' !");
  }

  return 1;
}

int MBCController::svc(void)
{
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
      XGLOG_ERROR("MBCController::svc() Error in Message Queue");
      m_run = false;
    }
    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_INFO("MBCController::svc() stopped");
  return 0;
}

bool MBCController::handle_msg(ACE_Message_Block *pAmb)
{
  XGLOG_INFO("MBCController handling message from queue");
  xGateBaseMsg *pMsg = dynamic_cast<xGateBaseMsg*>(pAmb);

  if(!pMsg)
  {
    XGLOG_ERROR("MBCController::handle_msg failed. pMsg is NULL");
    return false;
  }

  IURDefines::MODULE_ID srcModuleId = pMsg->getSrcModuleId();
  switch(srcModuleId)
  {
    case IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE:
    {
      XGLOG_INFO("MBCController handling 'EN_XGATE_MSG_MEDIA_SERVICE_MODULE' event from media broadcast service");
      handle_MediaServiceModuleMsg(pMsg);
      break;
    }
    case IURDefines::UR_MODULE_XG_MBC_SERVICE:
    {
      XGLOG_INFO("MBCController handling 'EN_XGATE_MSG_MBC_MODULE' event from media broadcast service");
      handle_MBCModuleMsg(pMsg);
      break;
    }
    default:
    {
      XGLOG_ERROR("MBCController recvd msg from UNKNOWN src module");
      return false;
    }
  }

  return true;
}

bool MBCController::handle_MBCModuleMsg(xGateBaseMsg *pMsg)
{
  XGLOG_INFO("MBCController handling the Media Servcie Module Msg");
  xGateMBCServiceMsg *ptmediaBCastMsg = dynamic_cast<xGateMBCServiceMsg *>(pMsg);
  xGateMBCServiceEvent mbcEventType = ptmediaBCastMsg->get_mbc_event_type();

  switch (mbcEventType)
  {
    case EN_XG_MBC_EVENT_INIT_CONNECTION_REQ:
    {
      XGLOG_INFO("MBCController handling 'EN_XG_MBC_EVENT_INIT_CONNECTION_REQ' event from media broadcast service");
      handleMBCClientConnectionReq(pMsg);
      break;
    }
    case EN_XG_MBC_EVENT_INIT_LISTEN_REQ:
    {
      XGLOG_INFO("MBCController handling 'EN_XG_MBC_EVENT_INIT_LISTEN_REQ' event from media broadcast service");
      handleMBCTcpListenRequest(pMsg);
      break;
    }  
    case EN_XG_MBC_EVENT_SEND:
    {
      XGLOG_INFO("MBCController handling 'EN_XG_MBC_EVENT_MBC_SEND' event from media broadcast service");
      postMBCMsgToMediaService(pMsg);
      break;
    }
    default:
    {
      XGLOG_ERROR("MBCController failed to handle media broadcast module message: %d !", mbcEventType);
      break;
    }
  }

  return true;
}

bool MBCController::handle_MediaServiceModuleMsg(xGateBaseMsg *pMsg)
{
  if(!pMsg) {
    XGLOG_ERROR("INVALID object. handle_MediaServiceModuleMsg failed");	  
    return false;
  }

  XGLOG_INFO("MBCController handling the Media Service Module Msg");
  xGateMediaServiceMsg *ptmediaServiceMsg = dynamic_cast<xGateMediaServiceMsg *>(pMsg);
  xGateMBCServiceMsg *ptmediaBCastMsg = new xGateMBCServiceMsg();
  ptmediaBCastMsg->setSrcModuleId(ptmediaServiceMsg->getSrcModuleId());
  ptmediaBCastMsg->setDstModuleId(ptmediaServiceMsg->getDstModuleId());
  ptmediaBCastMsg->setConnectionId(ptmediaServiceMsg->get_connectionId());
  
  switch (pMsg->getMsgType())
  {
    case EN_XGATE_MSG_CLIENT_CONNECTED:
    {
      XGLOG_INFO("MBCController handling 'EN_XGATE_MSG_CLIENT_CONNECTED' event from media service");
      xGateNetConnection netConnInfo = ptmediaServiceMsg->get_netConnInfo();
      MBCTcpConnInfo mbcTcpConnInfo;
      mbcTcpConnInfo.m_ip = netConnInfo.recvIp_;
      mbcTcpConnInfo.m_port = netConnInfo.recvPort_;
      mbcTcpConnInfo.m_connId = ptmediaServiceMsg->get_connectionId();
      ptmediaBCastMsg->set_mbc_tcp_conn_info(mbcTcpConnInfo);
      ptmediaBCastMsg->setMsgType((xGateMsgType)EN_XGATE_MSG_CLIENT_CONNECTED);
      break;
    }
    case EN_XGATE_MSG_NEW_CLIENT:
    {
      xGateNetConnection netConnInfo = ptmediaServiceMsg->get_netConnInfo();
      MBCTcpConnInfo mbcTcpConnInfo;
      mbcTcpConnInfo.m_ip = netConnInfo.recvIp_;
      mbcTcpConnInfo.m_port = netConnInfo.recvPort_;
      mbcTcpConnInfo.m_connId = ptmediaServiceMsg->get_connectionId();
      ptmediaBCastMsg->set_mbc_tcp_conn_info(mbcTcpConnInfo);
      ptmediaBCastMsg->setMsgType((xGateMsgType)EN_XGATE_MSG_NEW_CLIENT);
      break;
    }
    case EN_XGATE_MSG_CONN_CLOSED:
    {
      XGLOG_INFO("MBCController handling 'EN_XGATE_MSG_CONN_CLOSED' event from media broadcast service");
      xGateNetConnection netConnInfo = ptmediaServiceMsg->get_netConnInfo();
      MBCTcpConnInfo mbcTcpConnInfo;
      mbcTcpConnInfo.m_ip = netConnInfo.recvIp_;
      mbcTcpConnInfo.m_port = netConnInfo.recvPort_;
      mbcTcpConnInfo.m_connId = ptmediaServiceMsg->get_connectionId();
      ptmediaBCastMsg->set_mbc_tcp_conn_info(mbcTcpConnInfo);
      ptmediaBCastMsg->setMsgType((xGateMsgType)EN_XGATE_MSG_CONN_CLOSED);
      break;
    }
    case EN_XGATE_MSG_RECV:
    {
      XGLOG_INFO("MBCController handling 'EN_XGATE_MSG_RECV' event from media broadcast service");
      ptmediaBCastMsg->setMsg((char*)ptmediaServiceMsg->get_msg());
      ptmediaBCastMsg->setMsgLen(ptmediaServiceMsg->getMsgLen());
      ptmediaBCastMsg->setMsgType((xGateMsgType)EN_XGATE_MSG_RECV);
      XGLOG_INFO ("ptmediaBCastMsg->getMsg (): %p  len: %d", ptmediaBCastMsg->getMsg(), 
                                                             ptmediaBCastMsg->getMsgLen());
      break;
    }
    default:
    {
      XGLOG_ERROR("MBCController failed to handle media service message: %d !", pMsg->getMsgType());
      break;
    }
  }
  arrayModules[pMsg->getDstModuleId()]->pushModuleMsg(ptmediaBCastMsg);

  return true;
}

bool MBCController::handleMBCClientConnectionReq(xGateBaseMsg *pMsg)
{
  xGateMBCServiceMsg *ptmediaBCastMsg = dynamic_cast<xGateMBCServiceMsg *>(pMsg);
  xGateMediaServiceMsg *ptmediaServiceMsg = new xGateMediaServiceMsg();
  
  MBCTcpConnInfo mbcTcpConnInfo = ptmediaBCastMsg->get_mbc_tcp_conn_info();
  xGateNetConnection netConnInfo;
  netConnInfo.recvIp_ = mbcTcpConnInfo.m_ip;
  netConnInfo.recvPort_ = mbcTcpConnInfo.m_port;
  ptmediaServiceMsg->set_netConnInfo(netConnInfo);
  ptmediaServiceMsg->setMsgType((xGateMsgType)EN_XGATE_MSG_CONNECT_CLIENT);
  ptmediaServiceMsg->setSrcModuleId(ptmediaBCastMsg->getSrcModuleId());
  ptmediaServiceMsg->setDstModuleId(ptmediaBCastMsg->getDstModuleId());

  arrayModules[ptmediaBCastMsg->getDstModuleId()]->pushModuleMsg(ptmediaServiceMsg);

  return true;
}

bool MBCController::handleMBCTcpListenRequest(xGateBaseMsg *pMsg)
{
  xGateMBCServiceMsg *ptmediaBCastMsg = dynamic_cast<xGateMBCServiceMsg *>(pMsg);
  xGateMediaServiceMsg *ptmediaServiceMsg = new xGateMediaServiceMsg();

  MBCTcpConnInfo mbcTcpConnInfo = ptmediaBCastMsg->get_mbc_tcp_conn_info();
  xGateNetConnection netConnInfo;
  netConnInfo.recvIp_ = mbcTcpConnInfo.m_ip;
  netConnInfo.recvPort_ = mbcTcpConnInfo.m_port;
  ptmediaServiceMsg->set_netConnInfo(netConnInfo);
  ptmediaServiceMsg->setMsgType((xGateMsgType)EN_XGATE_MSG_LISTEN);
  ptmediaServiceMsg->setSrcModuleId(ptmediaBCastMsg->getSrcModuleId());
  ptmediaServiceMsg->setDstModuleId(ptmediaBCastMsg->getDstModuleId());

  arrayModules[ptmediaBCastMsg->getDstModuleId()]->pushModuleMsg(ptmediaServiceMsg);

  return true;
}

bool MBCController::postMBCMsgToMediaService(xGateBaseMsg *pMsg)
{
  xGateMBCServiceMsg *ptmediaBCastMsg = dynamic_cast<xGateMBCServiceMsg *>(pMsg);
  xGateMediaServiceMsg *ptmediaServiceMsg = new xGateMediaServiceMsg();

  MBCTcpConnInfo mbcTcpConnInfo = ptmediaBCastMsg->get_mbc_tcp_conn_info();
  xGateNetConnection netConnInfo;
  netConnInfo.recvIp_ = mbcTcpConnInfo.m_ip;
  netConnInfo.recvPort_ = mbcTcpConnInfo.m_port;
  ptmediaServiceMsg->set_netConnInfo(netConnInfo);
  ptmediaServiceMsg->setConnectionId(ptmediaBCastMsg->getConnectionId());
  ptmediaServiceMsg->setMsgType((xGateMsgType)EN_XGATE_MSG_SEND);
  ptmediaServiceMsg->set_msg(ptmediaBCastMsg->getMsg());
  ptmediaServiceMsg->setMsgLen(ptmediaBCastMsg->getMsgLen());
  XGLOG_INFO("post MBC msg send with %d",ptmediaBCastMsg->getMsgLen());
  ptmediaServiceMsg->setSrcModuleId(ptmediaBCastMsg->getSrcModuleId());
  ptmediaServiceMsg->setDstModuleId(ptmediaBCastMsg->getDstModuleId());
  arrayModules[ptmediaBCastMsg->getDstModuleId()]->pushModuleMsg(ptmediaServiceMsg);

  return true;
}

bool MBCController::handleModuleCallbackMsg(IURModuleMsg * pctrlMsg)
{
  XGLOG_INFO("MBCController::Inside handleModuleCallbackMsg !");
  if (pctrlMsg)
  {
    XGLOG_DEBUG("MBCController::Message Received in handleModuleCallbackMsg!");
    MBCCONTROLLER->putq(dynamic_cast<ACE_Message_Block*>(pctrlMsg));
  }
  return true;
}
