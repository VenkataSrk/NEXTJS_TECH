//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xglog.h"
#include "xGateUtil.h"
#include "TcpReceiver.h"
#include "NetIOMsgProcessor.h"

//self includes
#include "TcpSocketHandler.h"

TcpSocketHandler::TcpSocketHandler(ACE_Reactor *reactor,int moduleId,unsigned int connId):
  m_reactor(reactor),m_moduleId(moduleId),m_connId(connId)
{
  m_maxMsgSize = TCP_MAX_MESSAGE_SIZE;
  memset(m_socketData, 0, XGATE_TCP_READ_SIZE);
  m_recvdBuf = new TcpJsonPendingMsg();
} 

TcpSocketHandler::~TcpSocketHandler (void)
{
  delete m_recvdBuf;
}

xGateRetVal TcpSocketHandler::init (ACE_SOCK_Stream& peerStream,unsigned int conn_id)
{ 
  m_peer = peerStream; 
  m_peer.get_local_addr(m_localAddr);
  m_peer.get_remote_addr(m_remoteAddr);

  char ipAddr[INET6_ADDRSTRLEN+1];
  memset(ipAddr, 0, INET6_ADDRSTRLEN+1);
  m_remoteAddr.get_host_addr(ipAddr, INET6_ADDRSTRLEN+1);
  m_remoteIpAddr = ipAddr;

  TcpConTuple conTuple;
  conTuple.m_remoteIp = m_remoteAddr.get_host_addr();
  conTuple.m_remotePort = m_remoteAddr.get_port_number();
  conTuple.m_fd = m_peer.get_handle(); 
  conTuple.m_localPort = m_localAddr.get_port_number();
  XGLOG_DEBUG("Initializing TcpSocketHandler for peer %s:%d and fd: %d", \
      conTuple.m_remoteIp.c_str(), conTuple.m_remotePort, conTuple.m_fd);

  /// Register with the Receiver Reactor
  errno=0;
  if(m_reactor->register_handler(m_peer.get_handle(),
        this, ACE_Event_Handler::READ_MASK)==-1) {
    XGLOG_ERROR("initializing TcpSocketHandler failed for peer: %s:%d while registering" \
        " with reactor due to error: %s !", conTuple.m_remoteIp.c_str(), conTuple.m_remotePort, strerror(errno));
    return EN_XGATE_STATUS_ACE_ERROR;
  } else {
    XGLOG_INFO("initializing TcpSocketHandler success for peer: %s:%d  and fd :%d" \
         "with reactor", conTuple.m_remoteIp.c_str(), conTuple.m_remotePort,conTuple.m_fd);
  }

  NETIOUTIL->addToMgConnectionMap(conTuple);
  TCPRECEIVER->addFdToSocketHandlerMap((int)m_peer.get_handle(), this); 
  NETIOUTIL->addToConnTupleInfo(m_connId,conTuple);

  int keepAlive = 1;
  this->peer().set_option(SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive));
  struct timeval timeout;      
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;  //TODO: Make it as configurable in xGateUtil
  if (setsockopt(m_peer.get_handle(), SOL_SOCKET,
                 SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
  {
    XGLOG_ERROR("Initializing TcpSocketHandler failed for peer: %s:%d while setting socket options !",
                conTuple.m_remoteIp.c_str(), conTuple.m_remotePort);
    NETIOUTIL->removeConnectionId(m_connId);
    NETIOUTIL->removeConnTupleInfo(m_connId);
  }
  return EN_XGATE_STATUS_SUCCESS; //return success
}

ACE_HANDLE TcpSocketHandler::get_handle(void) const
{
  return this->m_peer.get_handle(); 
}

ACE_SOCK_Stream& TcpSocketHandler::peer() const
{
  return (ACE_SOCK_Stream &) this->m_peer; 
} 

int TcpSocketHandler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
  XGLOG_DEBUG("handle_close triggered for handle %d", handle); 

  long int timerId = 0;
  NETIOUTIL->getTimerIdFromConnMap((int)handle, timerId);
  TCPRECEIVER->removeFromFdToSocketHandlerMap(int(handle));
  if(timerId != 0) {
    int *pFd = nullptr;
    if (m_reactor->cancel_timer(timerId,(const void**)&pFd)) {
      XGLOG_DEBUG("handle_close cancel_timer successful");
      if (pFd) {
        delete pFd;
      }
    }
  }
  this->peer().close();
  SendCloseConnection();

  /// remove the fd from map
  TcpConTuple conTuple;
  conTuple.m_remoteIp = m_remoteAddr.get_host_addr();
  conTuple.m_remotePort = m_remoteAddr.get_port_number();
  conTuple.m_fd = handle; 
  conTuple.m_localPort = m_localAddr.get_port_number();

  XGLOG_DEBUG("handle_close removing the address %s:%d and fd: %d" \
      " from connection map", conTuple.m_remoteIp.c_str(), conTuple.m_remotePort, conTuple.m_fd);

  NETIOUTIL->removeFromMgConnectionMap(conTuple);  //TODO: Need to check this functionality
  NETIOUTIL->removeConnTupleInfo(m_connId);
  /// Clean up this Socket handler
  delete this;
  return 0;
}//end handle_close

int TcpSocketHandler::handle_input(ACE_HANDLE aceHandle)
{
  int retVal = 0;
  XGLOG_DEBUG("handling input received on fd: %d", aceHandle);

  //Reset the TCP read Buffer
  memset (m_socketData, '\0', XGATE_TCP_READ_SIZE);

  //Receive the Data
  int bufLen = -1;
  errno = 0;
  bufLen = (int)peer().recv(m_socketData,XGATE_TCP_READ_SIZE -1);
  if(0 < bufLen) {
    XGLOG_DEBUG("received %d length of data: <%s> on fd: %d",\
      bufLen, m_socketData, aceHandle);
    m_recvdBuf->pendingMsg.append((const char*)m_socketData, (size_t)bufLen);
    m_recvdBuf->pendingMsgLen += bufLen;
    m_recvdBuf->isPending = true;
    while(m_recvdBuf->isPending) {
      if(m_recvdBuf->get_one_complete_msg() == EN_XGATE_STATUS_SUCCESS) {
        //send msg to processor after tcp decoding is done.
        this->post(m_recvdBuf->completedMsg.msg.c_str(), m_recvdBuf->completedMsg.msgLen);
        // m_recvdBuf->pendingMsg = "";
        // m_recvdBuf->pendingMsgLen = 0;
      } else {
        break;
      }
    }
  } else {
    if(errno != 0) {
      XGLOG_ERROR("handle_input failed for fd: %d with error %s ", \
          aceHandle, strerror(errno));
    } else {
      XGLOG_WARN("handle_input failed for fd: %d while reading data from socket", \
          aceHandle);   
    } 
    this->post("test message !", 20);
    retVal = -1;
  } 

  if((retVal == -1)) {
    m_reactor->remove_handler(aceHandle,ACE_Event_Handler::READ_MASK); 
  }
  return retVal;
} 

void TcpSocketHandler::post(const char *pBuf, int sizeOfMessage)
{

  XGLOG_DEBUG("posting message to NetIO service");
  xGateNetIOServiceMsg *pMsg = new xGateNetIOServiceMsg();
  xGateNetConnection netConn;
  netConn.m_IpAdd = m_remoteIpAddr;
  netConn.m_Port = m_remoteAddr.get_port_number();
  netConn.m_fd = m_peer.get_handle();
  netConn.m_protocol = EN_XGATE_TRANSPORT_TCP;

	pMsg->set_netConnInfo(netConn);
  pMsg->setNetIOmsg(pBuf);
  pMsg->setNetIOMsgType(EN_XGATE_MSG_RECV);
	pMsg->setMsgType(EN_XGATE_MSG_NETIO_MODULE);
	pMsg->setSrcModuleId(IURDefines::UR_MODULE_NETIO_SERVICE);
  pMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);

  XGLOG_INFO("TcpSocketHandler::post Send Socket Message to SGW Controller Msg(%s)",pBuf);
  NETIOSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
}

TcpJsonPendingMsg::TcpJsonPendingMsg() : isPending(false), delimiter("\r\n\r\n"),
  pendingMsg(""), pendingMsgLen(0), completedMsg{}
{
}

TcpJsonPendingMsg::~TcpJsonPendingMsg()
{
}

xGateRetVal TcpJsonPendingMsg::get_one_complete_msg()
{
  XGLOG_DEBUG("Enter TcpJsonPendingMsg::get_one_complete_msg");
  int retVal = EN_XGATE_STATUS_ERROR;

  //1. find out position of end of msg(i.e) "\r\n\r\n" from given pending buffer
  size_t delimiterPos = pendingMsg.find(delimiter);

  if(delimiterPos == string::npos) {
    XGLOG_ERROR("TcpJsonPendingMsg::get_one_complete_msg, one complete msg not available \
        in given pending buffer: %s\n", pendingMsg.c_str());
    //3rd scenario: not able to extract one complete msg, becoz partial msg in buffer
    //in 3rd scenario we will receive remaining msg in next socket read
    if((pendingMsg.length() > 0)) {
      isPending = true;
      pendingMsgLen = pendingMsg.length();
    }
    return EN_XGATE_STATUS_ERROR;
  } else {
    //2. get first complete msg
#if 1 
    completedMsg.msg = pendingMsg.substr(0, delimiterPos);
    completedMsg.msgLen = completedMsg.msg.length();
    size_t nextMsgStartPos = delimiterPos + delimiter.length();
    pendingMsg.erase(0, nextMsgStartPos);

    if ((completedMsg.msgLen == (delimiterPos)) && (pendingMsg.length() <= 0)) {
      isPending = false;
      pendingMsgLen = 0;
      return EN_XGATE_STATUS_SUCCESS;
    } 

    if ((completedMsg.msgLen == (delimiterPos)) && (pendingMsg.length() > 0)) {
      isPending = true;
      pendingMsgLen = pendingMsg.length();
      return EN_XGATE_STATUS_SUCCESS;
    } 

#else // Siva commneted below line because currently we are not geting any msg_length from interagattor  
		char messageStr[30] = "\0";
		size_t tlen = completedMsg.msg.copy(messageStr,29,0);
    messageStr[tlen] = '\0';
		char *token;
		bool bRead = false;
		completedMsg.msgLen = 0;
		char *temp =  messageStr;
		 while(token = strtok_r(temp,",:",&temp )){
		 if(token && strstr(token , "msg_len")){
		  bRead =  true;
			continue;
			}
			if(bRead){
			completedMsg.msgLen =  atoi(token);  
			break;
			}
//			token =  (strtok(temp,nullptr));
		 }

     if(completedMsg.msgLen == 0 ){
       XGLOG_ERROR("TcpJsonPendingMsg::get_one_complete_msg, 'msg_len' hearder field \
            not found in extracted Msg: %s\n", completedMsg.msg.c_str());
						return EN_XGATE_STATUS_ERROR;
     }
    //3. get msg length
#if 0
    size_t pos = completedMsg.msg.find("msg_len");
    string resultStr = completedMsg.msg.substr(pos+8, 5);
    if(sscanf(resultStr.c_str(), "%*[:]%d", &completedMsg.msgLen) <= 0) {
      if(sscanf(resultStr.c_str(), "%d", &completedMsg.msgLen) <= 0) {
        XGLOG_ERROR("MediaTcpJsonPendingMsg::get_one_complete_msg, 'msg_len' hearder field \
            not found in extracted Msg: %s\n", completedMsg.msg.c_str());
        return EN_XGATE_STATUS_ERROR;
      }
    }
#endif
    size_t nextMsgStartPos = delimiterPos + delimiter.length();
    pendingMsg.erase(0, nextMsgStartPos);
#if 0
    printf("\n*************************************\n");
    printf("extracted %d bytes of json msg: \n%s\n", completedMsg.msgLen, completedMsg.msg.c_str());
    printf("*************************************\n");
#endif

    //1st scenario: extracted one complete msg and no more pending msg in buffer
    if((completedMsg.msgLen == (delimiterPos)) && (pendingMsg.length() <= 0)) {
      isPending = false;
      pendingMsgLen = 0;
      return EN_XGATE_STATUS_SUCCESS;
    }

    //2nd scenario: extracted one complete msg but still having pending msg in buffer
    if((completedMsg.msgLen == (delimiterPos)) && (pendingMsg.length() > 0)) {
      isPending = true;
      pendingMsgLen = pendingMsg.length();
      return EN_XGATE_STATUS_SUCCESS;
    }

    //3rd scenario: not able to extract one complete msg, becoz partial msg in buffer
    //in 3rd scenario we will receive remaining msg in next socket read
    if((pendingMsg.length() > 0)) {
      isPending = true;
      pendingMsgLen = pendingMsg.length();
      return EN_XGATE_STATUS_ERROR;
    }
#endif
  }

  return EN_XGATE_STATUS_ERROR;
}

/*
* Sending close connection request to controller 
*/
void TcpSocketHandler::SendCloseConnection()
{

  xGateNetIOServiceMsg *pMsg = new xGateNetIOServiceMsg();
  xGateNetConnection netConnInfo;

  netConnInfo.m_IpAdd = m_remoteAddr.get_host_addr();
  netConnInfo.m_Port = m_remoteAddr.get_port_number();
  pMsg->setConnectionId(m_connId);

  pMsg->set_netConnInfo(netConnInfo);
  pMsg->setNetIOMsgType(EN_XGATE_MSG_CONN_CLOSED);
  pMsg->setMsgType(EN_XGATE_MSG_NETIO_MODULE);
  pMsg->setSrcModuleId(IURDefines::UR_MODULE_NETIO_SERVICE);
  pMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);

  NETIOSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));

}
