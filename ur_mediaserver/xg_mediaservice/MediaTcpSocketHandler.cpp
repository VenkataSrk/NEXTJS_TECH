//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xglog.h"
#include "xGateUtil.h"
#include "MediaTcpReceiver.h"
#include "MediaMsgProcessor.h"
#include "xGateMediaService.h"
//self includes
#include "MediaTcpSocketHandler.h"

MediaTcpSocketHandler::MediaTcpSocketHandler(ACE_Reactor *reactor,int moduleId,unsigned int connId):
  m_reactor(reactor),m_moduleId(moduleId),m_connId(connId)
{
  m_maxMsgSize = TCP_MAX_MESSAGE_SIZE;
  memset(m_socketData, 0, XGATE_TCP_READ_SIZE);
  m_recvdBuf = new TcpJsonPendingMsg();
} 

MediaTcpSocketHandler::~MediaTcpSocketHandler (void)
{
  delete m_recvdBuf;
}

xGateRetVal MediaTcpSocketHandler::send_connection_closed(void)
{	
 xGateMediaServiceMsg *pmsg = new xGateMediaServiceMsg();
 pmsg->setMsgType((xGateMsgType)EN_XGATE_MSG_CONN_CLOSED);
 pmsg->setSrcModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
 pmsg->setDstModuleId((IURDefines::MODULE_ID)m_moduleId);
 pmsg->setConnectionId(m_connId);
 xGateNetConnection netConn;
 netConn.recvIp_ = m_remoteIpAddr;
 netConn.recvPort_ = m_remoteAddr.get_port_number();
 netConn.fd_ = m_peer.get_handle();
 netConn.protocol_ = EN_XGATE_TRANSPORT_TCP;
 pmsg->set_netConnInfo(netConn);
 MEDIASERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pmsg));
 return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal MediaTcpSocketHandler::init (ACE_SOCK_Stream& peerStream)
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
    if(!MEDIAUTIL->removeConnectionId(m_connId)){
      XGLOG_ERROR("Invalid request recvd for connId:%d",m_connId);
    }	    
    return EN_XGATE_STATUS_ACE_ERROR;
  }

  MEDIAUTIL->addToMgConnectionMap(conTuple);
  MEDIATCPRECEIVER->addFdToSocketHandlerMap((int)m_peer.get_handle(), this); 
  MEDIAUTIL->addToConnTupleInfo(m_connId,conTuple);
  struct timeval timeout;
  int keepAlive = 1;
  this->peer().set_option(SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive));
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;  //TODO: Make it as configurable in xGateUtil
  if(setsockopt (m_peer.get_handle(), SOL_SOCKET, 
        SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
    XGLOG_ERROR("Initializing TcpSocketHandler failed for peer: %s:%d while setting socket options !", \
      conTuple.m_remoteIp.c_str(), conTuple.m_remotePort);
    MEDIAUTIL->removeConnectionId(m_connId);
    MEDIAUTIL->removeConnTupleInfo(m_connId);
    return EN_XGATE_STATUS_ERROR;
  }
  return EN_XGATE_STATUS_SUCCESS; //return success
}

ACE_HANDLE MediaTcpSocketHandler::get_handle(void) const
{
  return this->m_peer.get_handle(); 
}

ACE_SOCK_Stream& MediaTcpSocketHandler::peer() const
{
  return (ACE_SOCK_Stream &) this->m_peer; 
} 

int MediaTcpSocketHandler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
  XGLOG_DEBUG("handle_close triggered for handle %d", handle); 

  long int timerId = 0;
  MEDIAUTIL->getTimerIdFromConnMap((int)handle, timerId);
  MEDIATCPRECEIVER->removeFromFdToSocketHandlerMap(int(handle));
  MEDIAUTIL->removeConnectionId(m_connId);
  MEDIAUTIL->removeConnTupleInfo(m_connId);
  if(timerId != 0) {
    int *pFd = NULL;
    if (m_reactor->cancel_timer(timerId,(const void**)&pFd)) {
      XGLOG_DEBUG("handle_close cancel_timer successful");
      if (pFd) {
        delete pFd;
      }
    }
  }
  this->peer().close(); 

  /// remove the fd from map
  TcpConTuple conTuple;
  conTuple.m_remoteIp = m_remoteAddr.get_host_addr();
  conTuple.m_remotePort = m_remoteAddr.get_port_number();
  conTuple.m_fd = handle; 
  conTuple.m_localPort = m_localAddr.get_port_number();

  XGLOG_DEBUG("handle_close removing the address %s:%d and fd: %d" \
      " from connection map", conTuple.m_remoteIp.c_str(), conTuple.m_remotePort, conTuple.m_fd);

  MEDIAUTIL->removeFromMgConnectionMap(conTuple);  //TODO: Need to check this functionality
  send_connection_closed();
  /// Clean up this Socket handler
  delete this;
  return 0;
}//end handle_close

int MediaTcpSocketHandler::handle_input(ACE_HANDLE aceHandle)
{
  int retVal = 0;
  //XGLOG_DEBUG("handling input received on fd: %d", aceHandle);

  //Reset the TCP read Buffer
  memset (m_socketData, '\0', XGATE_TCP_READ_SIZE);

  //Receive the Data
  int bufLen = -1;
  errno = 0;
  bufLen = (int)peer().recv(m_socketData,XGATE_TCP_READ_SIZE -1);
  if(0 < bufLen) {
    XGLOG_TRACE("received %d length of data: <%s> on fd: %d",\
      bufLen, m_socketData, aceHandle);
    m_recvdBuf->pendingMsg.append((const char*)m_socketData, (size_t)bufLen);
    m_recvdBuf->pendingMsgLen += bufLen;
    m_recvdBuf->isPending = true;
    while(m_recvdBuf->isPending) {
      if(m_recvdBuf->get_one_complete_msg() == EN_XGATE_STATUS_SUCCESS) {
        //send msg to processor after tcp decoding is done.
        this->post(m_recvdBuf->completedMsg.msg.c_str(), m_recvdBuf->completedMsg.msgLen);
        m_recvdBuf->completedMsg.msg.clear();
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

void MediaTcpSocketHandler::post(const char *pBuf, int sizeOfMessage)
{
  xGateNetConnection netConn;
  netConn.recvIp_ = m_remoteIpAddr;
  netConn.recvPort_ = m_remoteAddr.get_port_number();
  netConn.fd_ = m_peer.get_handle();
  netConn.protocol_ = EN_XGATE_TRANSPORT_TCP;
  xGateMediaServiceMsg *pMsg = new xGateMediaServiceMsg();
  pMsg->setMsgType((xGateMsgType)EN_XGATE_MSG_RECV);
  pMsg->setSrcModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
  pMsg->setDstModuleId((IURDefines::MODULE_ID)m_moduleId);
  char *msg = new char[sizeOfMessage + 1];
  if(msg == NULL)
  {	  
    XGLOG_ERROR("Could not allocate memory for msg");
    return;  
  }	  
  XGLOG_TRACE("posting message to media msg processor: %p with %d size",msg, sizeOfMessage);
  msg[sizeOfMessage] = '\0';
  memcpy(msg,pBuf,sizeOfMessage);
  pMsg->set_msg(static_cast <void*>(msg));
  pMsg->setMsgLen(sizeOfMessage + 1);
  pMsg->setConnectionId(m_connId);
  pMsg->set_netConnInfo(netConn);
  MEDIASERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
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
  XGLOG_TRACE("Enter TcpJsonPendingMsg::get_one_complete_msg");
  int retVal = EN_XGATE_STATUS_ERROR;

  //1. find out position of end of msg(i.e) "\r\n\r\n" from given pending buffer
  size_t delimiterPos = pendingMsg.find(delimiter);

  //if(delimiterPos < 0) {
  if(delimiterPos == string::npos) {
    //XGLOG_ERROR("TcpJsonPendingMsg::get_one_complete_msg, one complete msg not available \
    //    in given pending buffer: %s\n", pendingMsg.c_str());
    //3rd scenario: not able to extract one complete msg, becoz partial msg in buffer
    //in 3rd scenario we will receive remaining msg in next socket read
    if((pendingMsg.length() > 0)) {
      isPending = true;
      pendingMsgLen = pendingMsg.length();
    }
    return EN_XGATE_STATUS_ERROR;
  } else {
    //2. get first complete msg
    completedMsg.msg = pendingMsg.substr(0, delimiterPos);
		char messageStr[40] = "\0";
		size_t tlen = completedMsg.msg.copy(messageStr,32,0);
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
//			token =  (strtok(temp,NULL));
		 }

     if(completedMsg.msgLen == 0 ){
       XGLOG_ERROR("TcpJsonPendingMsg::get_one_complete_msg, 'msg_len' hearder field \
            not found in extracted Msg: %s\n", completedMsg.msg.c_str());
       return EN_XGATE_STATUS_ERROR;
     }else{
       XGLOG_TRACE("completedMsg.msgLen extracted: %d delim_pos: %d ", completedMsg.msgLen, delimiterPos);
     }
    //3. get msg length
/*    size_t pos = completedMsg.msg.find("msg_len");
    string resultStr = completedMsg.msg.substr(pos+8, 5);
    if(sscanf(resultStr.c_str(), "%*[:]%d", &completedMsg.msgLen) <= 0) {
      if(sscanf(resultStr.c_str(), "%d", &completedMsg.msgLen) <= 0) {
        XGLOG_ERROR("TcpJsonPendingMsg::get_one_complete_msg, 'msg_len' hearder field \
            not found in extracted Msg: %s\n", completedMsg.msg.c_str());
        return EN_XGATE_STATUS_ERROR;
      }
    }*/

    size_t nextMsgStartPos = delimiterPos + delimiter.length();
    pendingMsg.erase(0, nextMsgStartPos);

    //1st scenario: extracted one complete msg and no more pending msg in buffer
    if((completedMsg.msgLen == (delimiterPos)) && (pendingMsg.length() <= 0)) {
      isPending = false;
      pendingMsgLen = 0;
      return EN_XGATE_STATUS_SUCCESS;
    }else{
      XGLOG_TRACE("completedMsg.msgLen %d == (delimiterPos): %d pendingMsg.length: %d \n",
                completedMsg.msgLen, delimiterPos, pendingMsg.length());
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
      XGLOG_INFO ("Returning error as there is still partial msg to be read pendingMsgLen: %d completedMsg.msgLen: %d delimiterPos: %d\n",pendingMsgLen, completedMsg.msgLen, delimiterPos);
      return EN_XGATE_STATUS_ERROR;
    }
  }

  return EN_XGATE_STATUS_ERROR;
}

