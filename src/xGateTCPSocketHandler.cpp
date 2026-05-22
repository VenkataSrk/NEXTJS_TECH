
//local includes
#include "xglog.h"
#include "xGateUtil.h"

//self includes
#include "xGateMGCConnector.h"
#include "xGateTCPSocketHandler.h"
#include "xGateMgMsg.h"

#define THISMODULE "TCPSockHandler"

xGateTCPSocketHandler::xGateTCPSocketHandler(const char* serverType, ACE_Reactor *reactor,
    ACE_Task<ACE_MT_SYNCH>* taskPtr):
  m_reactor(reactor),
  m_taskPtr(taskPtr)
{
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);

  memset(m_socketData, 0, XGATE_TCP_READ_SIZE);

  m_recvdBuf = new TcpJsonPendingMsg(); 
  XGLOG_WARN( "xGateTCPSocketHandler::xGateTCPSocketHandler(%s)", (char*)ur_log_string (m_serverType));
} 

xGateTCPSocketHandler::~xGateTCPSocketHandler (void)
{
  XGLOG_WARN( "xGateTCPSocketHandler::~xGateTCPSocketHandler(%s)",(char*)ur_log_string(m_serverType)); 
  delete m_recvdBuf;
}

xGateRetVal xGateTCPSocketHandler::init (ACE_SOCK_Stream& peerStream)
{ 

  XGLOG_WARN( "xGateTCPSocketHandler::init(%s)",(char*)ur_log_string(m_serverType));
  m_peer = peerStream; 
  m_peer.get_local_addr(m_localAddr);
  m_peer.get_remote_addr(m_remoteAddr);

  char ipAddr[INET6_ADDRSTRLEN+1];
  memset(ipAddr,0,INET6_ADDRSTRLEN+1);
  m_remoteAddr.get_host_addr(ipAddr, INET6_ADDRSTRLEN+1);
  m_remoteIpAddr = ipAddr;

  /// Register with the Receiver Reactor
  errno=0;
  if(m_reactor->register_handler(m_peer.get_handle(),this, ACE_Event_Handler::READ_MASK)==-1)
  //if(m_reactor->register_handler(this, ACE_Event_Handler::READ_MASK)==-1)
  {
    XGLOG_ERROR( "xGateTCPSocketHandler::init(%s) Failed to register MGC ip: %s" \
        " with Reactor, due to Error:%s",(char*)ur_log_string(m_serverType),(char*)ur_log_string(ipAddr),(char*)ur_log_string(strerror(errno)));
    //this->getReactor()->remove_handler(this, ACE_Event_Handler::READ_MASK);
    return EN_XGATE_STATUS_ACE_ERROR;
  }

  //After successfully register with reactor, add into connection map
  xGateTCPConTuple conTuple;
  conTuple.ipAddress = m_remoteAddr.get_host_addr();
  conTuple.port      = m_remoteAddr.get_port_number();
  conTuple.fd        = m_peer.get_handle(); 
  conTuple.srcPort   = m_localAddr.get_port_number();

  xGateMGCConnector *pConnector = NULL;
  if(strcmp(m_serverType, "MG-MGC") == 0)
  {
    conTuple.eReceiverType = EN_XGATE_TCP_RECV_MG; 
    xGateUtil::addToMgConnectionMap(conTuple);
    pConnector = xGateUtil::getMgcConnector();
  }

  if (pConnector)
  {
    pConnector->addFdToSocketHandlerMap((int)m_peer.get_handle(),this); 
  }

  int keepAlive = 1;
  this->peer().set_option(SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive));
  struct timeval timeout;      
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;  //TODO: Make it as configurable in xGateUtil
  if (setsockopt (m_peer.get_handle(), SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
        sizeof(timeout)) < 0)
  {
    XGLOG_ERROR( "xGateTCPSocketHandler::init set_option for SO_SNDTIMEO failed");
  }
  return EN_XGATE_STATUS_SUCCESS; //return success
}

ACE_HANDLE xGateTCPSocketHandler::get_handle(void) const
{
  return this->m_peer.get_handle(); 
}

ACE_SOCK_Stream& xGateTCPSocketHandler::peer() const
{
  return (ACE_SOCK_Stream &) this->m_peer; 
} 

int xGateTCPSocketHandler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
  XGLOG_INFO( "xGateTCPSocketHandler::handle_close(%s)" \
      " Connection closed on handle %d ", (char*)ur_log_string(m_serverType), handle); 
  xGateTCPConnectionType eConnType;
  long int timerId = 0;

  xGateMGCConnector *pConnector = NULL;

  if(strcmp(m_serverType, "MG-MGC") == 0)
  {
    eConnType = EN_XGATE_MG_CONNECTION;
    pConnector = xGateUtil::getMgcConnector();
  }

  xGateUtil::getTimerIdFromConnMap((int)handle, eConnType, timerId);
  if (pConnector)
  {
    pConnector->removeFromFdToSocketHandlerMap(int(handle));
    if (timerId != 0)
    {
      int *pFd = NULL;
      if (m_reactor->cancel_timer(timerId,(const void**)&pFd))
      {
        XGLOG_INFO( "SipProxyTCPSocketHandler::handle_close cancel_timer successful");
        if (pFd)
        {
          delete pFd;
        }
      }
    }
  }

  this->peer().close(); 

  /// remove the fd from map
  xGateTCPConTuple conTuple;
  char ipAddr[INET6_ADDRSTRLEN+1];
  memset(ipAddr,0,INET6_ADDRSTRLEN+1);
  m_remoteAddr.get_host_addr(ipAddr, INET6_ADDRSTRLEN+1);
  conTuple.ipAddress    = ipAddr;
  conTuple.port         = m_remoteAddr.get_port_number();
  conTuple.fd           = handle; 
  conTuple.srcPort      = m_localAddr.get_port_number();

  XGLOG_INFO( "xGateTCPSocketHandler::handle_close(%s) Removing the Address:%s:%d  Fd:%d" \
      " from Connection Map",(char*)ur_log_string(m_serverType),(char*)ur_log_string(conTuple.ipAddress), conTuple.port, conTuple.fd);

  if(strcmp(m_serverType,"MG-MGC") == 0)
  {
    xGateUtil::removeFromMgConnectionMap(conTuple);  //TODO: Need to check this functionality
    xGateTimerData *pData = new xGateTimerData();
    pData->eTimerType = EN_XGATE_TIMER_RECONNECT; 
    long int timerId = m_reactor->schedule_timer(pConnector, (void*)pData, ACE_Time_Value(0,0));
  }

  /// Clean up this Socket handler
  m_reactor->remove_handler(handle, ACE_Event_Handler::READ_MASK); 
  delete this;
  return 0;
}//end handle_close

int xGateTCPSocketHandler::handle_input(ACE_HANDLE aceHandle)
{
  int retVal = 0;
  memset (m_socketData,'\0',XGATE_TCP_READ_SIZE);

  //Receive the Data
  int bufLen = -1;
  errno = 0;
  bufLen = (int)peer().recv(m_socketData,XGATE_TCP_READ_SIZE -1);
  if(0 < bufLen)
  {
    //in all scenario's concatinate pending and currently received buffers
    m_recvdBuf->pendingMsg.append(reinterpret_cast<const char*>(m_socketData), (size_t)bufLen);
    //m_recvdBuf->pendingMsg.append(m_socketData, (size_t)bufLen);
    m_recvdBuf->pendingMsgLen += bufLen;
    m_recvdBuf->isPending = true;
    while(m_recvdBuf->isPending) {
      if(m_recvdBuf->get_one_complete_msg() == EN_XGATE_STATUS_SUCCESS) {
        //send msg to processor after tcp decoding is done.
        XGLOG_ERROR( "xGateTCPSocketHandler::handle_input(%s): fd:<%d>  it's Length:<%d> recv data:<%s>",\
        (char*)ur_log_string(m_serverType),aceHandle,bufLen,(char*)ur_log_string(m_recvdBuf->completedMsg.msg));
        this->post(m_recvdBuf->completedMsg.msg.c_str(), m_recvdBuf->completedMsg.msgLen);
      } else {
        break;
      }
    }
  }
  else
  {
    if(errno != 0)
    {
      XGLOG_ERROR( "xGateTCPSocketHandler::handle_input(%s) :"\
          " fd:<%d> Read Failed error %s ",(char*)ur_log_string(m_serverType),aceHandle,(char*)ur_log_string(strerror(errno)));
    }
    else
    {
      XGLOG_ERROR( "xGateTCPSocketHandler::handle_input(%s) :"\
          " fd:<%d> Read Failed",(char*)ur_log_string(m_serverType),aceHandle);   
    } 
    retVal = -1;
  } 

  if( (retVal == -1))
  {
    m_reactor->remove_handler(aceHandle,ACE_Event_Handler::READ_MASK); 
  }
  XGLOG_INFO( "xGateTCPSocketHandler::handle_input Successfully handled the Message");
  return retVal;
} 

void xGateTCPSocketHandler::post(char *pBuf,\
    int sizeOfMessage)
{
  XGLOG_INFO( "Enter xGateTCPSocketHandler::post");
  xGateNetConnection netConn;
  netConn.recvPort_ = m_remoteAddr.get_port_number();
  netConn.recvIp_ = m_remoteIpAddr;
  netConn.protocol_ = EN_XGATE_TRANSPORT_TCP;

  char* data = new char[sizeOfMessage+1];

  if(data) {
    memset(data, 0, (sizeOfMessage+1));
    memcpy (data, pBuf, sizeOfMessage);
    data[sizeOfMessage] = '\0';
    ACE_Message_Block* pAmb = NULL;

    if(strcmp(m_serverType, "MG-MGC") == 0) {
      xGateMgMsg* pMgMsg = new xGateMgMsg();
      if(pMgMsg) {
        netConn.conType_ = EN_XGATE_MG_CONNECTION;
        pMgMsg->setMsgType(EN_XGATE_MSG_MGC_MG_IN);
        pMgMsg->set_mg_msg_type(EN_XGATE_MG_REGISTER);
        pMgMsg->set_data(data);
        pMgMsg->set_data_len(sizeOfMessage);
        pMgMsg->set_net_con_info(netConn);
        pMgMsg->set_src_port(m_localAddr.get_port_number()); 
        pAmb = static_cast<xGateMgMsg*> (pMgMsg);
      }
    }
    ACE_Time_Value tval ((time(NULL) ));
    tval += ACE_Time_Value(0,1);
    if (this->m_taskPtr->putq(pAmb, &tval))
    {
      XGLOG_INFO( "xGateTCPSocketHandler::post "\
          "Enqueue Message Success");
    }
    else
    {
      XGLOG_ERROR( "xGateTCPSocketHandler::post "\
          "Enqueue Message Failed");
    }
    delete [] data;
  }
  XGLOG_INFO( "Exit xGateTCPSocketHandler::post");
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
  XGLOG_INFO( "Enter TcpJsonPendingMsg::get_one_complete_msg");
  int retVal = EN_XGATE_STATUS_ERROR;
  
  //1. find out position of end of msg(i.e) "\r\n\r\n" from given pending buffer
  size_t delimiterPos = pendingMsg.find(delimiter);

  //if(delimiterPos < 0) {
  if(delimiterPos == string::npos) {
    XGLOG_ERROR( "TcpJsonPendingMsg::get_one_complete_msg, one complete msg not available \
        in given pending buffer: %s\n",(char*)ur_log_string(pendingMsg));
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
    //3. get msg length
    size_t pos = completedMsg.msg.find("msg_len");
    string resultStr = completedMsg.msg.substr(pos+8, 5);
    if(sscanf(resultStr.c_str(), "%*[:]%d", &completedMsg.msgLen) <= 0) {
      if(sscanf(resultStr.c_str(), "%d", &completedMsg.msgLen) <= 0) {
        XGLOG_ERROR( "TcpJsonPendingMsg::get_one_complete_msg, 'msg_len' hearder field \
            not found in extracted Msg: %s\n",(char*)ur_log_string(completedMsg.msg));
        return EN_XGATE_STATUS_ERROR;
      }
    }

    size_t nextMsgStartPos = delimiterPos + delimiter.length();
    pendingMsg.erase(0, nextMsgStartPos);

    /*printf("\n*************************************\n");
    printf("extracted %d bytes of json msg: \n%s\n", completedMsg.msgLen, completedMsg.msg.c_str());
    printf("*************************************\n");*/

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
  }

  return EN_XGATE_STATUS_ERROR;
}

