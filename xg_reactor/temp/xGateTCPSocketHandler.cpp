
//local includes
#include "log.h"
#include "klog.h"
#include "xGateUtil.h"

//self includes
#include "xGateConnector.h"
#include "xGateTCPReceiver.h"
#include "xGateTCPSocketHandler.h"
#include "xGateMgMsg.h"

#define THISMODULE "TCPSockHandler"
#define TCP_MAX_MESSAGE_SIZE 8192

xGateTCPSocketHandler::xGateTCPSocketHandler(const char* serverType, ACE_Reactor *reactor,
    ACE_Task<ACE_MT_SYNCH>* taskPtr):
  m_reactor(reactor),
  m_taskPtr(taskPtr)
{
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);

  m_maxMsgSize = TCP_MAX_MESSAGE_SIZE;
  memset(m_socketData, 0, XGATE_TCP_READ_SIZE);

  m_recvdBuf = new TcpJsonPendingMsg();
  CLog::Warning(THISMODULE, "xGateTCPSocketHandler::xGateTCPSocketHandler(%s)", m_serverType);
} 

xGateTCPSocketHandler::~xGateTCPSocketHandler (void)
{
  CLog::Warning(THISMODULE, "xGateTCPSocketHandler::~xGateTCPSocketHandler(%s)", m_serverType); 
  delete m_recvdBuf;
}

xGateRetVal xGateTCPSocketHandler::init (ACE_SOCK_Stream& peerStream)
{ 
  CLog::Detail(THISMODULE, "xGateTCPSocketHandler::init(%s)", m_serverType);
  m_peer = peerStream; 
  m_peer.get_local_addr(m_localAddr);
  m_peer.get_remote_addr(m_remoteAddr);

  char ipAddr[INET6_ADDRSTRLEN+1];
  memset(ipAddr,0,INET6_ADDRSTRLEN+1);
  m_remoteAddr.get_host_addr(ipAddr, INET6_ADDRSTRLEN+1);
  m_remoteIpAddr = ipAddr;

  xGateTCPConTuple conTuple;
  conTuple.ipAddress = m_remoteAddr.get_host_addr();
  conTuple.port      = m_remoteAddr.get_port_number();
  conTuple.fd        = m_peer.get_handle(); 
  conTuple.srcPort   = m_localAddr.get_port_number();

  xGateTCPReceiver *pReceiver = NULL;
  if(strcmp(m_serverType, "MG-MGC") == 0)
  {
    conTuple.eReceiverType = EN_XGATE_TCP_RECV_MG; 
    xGateUtil::addToMgConnectionMap(conTuple);
    pReceiver = xGateUtil::getMgReceiver();
  }

  /// Register with the Receiver Reactor
  errno=0;
  if(m_reactor->register_handler(m_peer.get_handle(),
        this, ACE_Event_Handler::READ_MASK)==-1)
  {
    CLog::Error(THISMODULE, "xGateTCPSocketHandler::init(%s) Failed to register" \
        " with Reactor Error:%s", m_serverType, strerror(errno));
    return EN_XGATE_STATUS_ACE_ERROR;
  }

  if (pReceiver)
  {
    pReceiver->addFdToSocketHandlerMap((int)m_peer.get_handle(),this); 
  }

  int keepAlive = 1;
  this->peer().set_option(SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive));
  struct timeval timeout;      
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;  //TODO: Make it as configurable in xGateUtil
  if (setsockopt (m_peer.get_handle(), SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
        sizeof(timeout)) < 0)
  {
    CLog::Error(THISMODULE, "xGateTCPSocketHandler::init set_option for SO_SNDTIMEO failed");
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
  CLog::Warning(THISMODULE, "xGateTCPSocketHandler::handle_close(%s)" \
      " Connection closed on handle %d ", m_serverType, handle); 
  xGateTCPConnectionType eConnType;
  long int timerId = 0;

  xGateTCPReceiver *pReceiver = NULL;

  if(strcmp(m_serverType, "MG-MGC") == 0)
  {
    eConnType = EN_XGATE_MG_CONNECTION;
    pReceiver = xGateUtil::getMgReceiver();
  }

  xGateUtil::getTimerIdFromConnMap((int)handle, eConnType, timerId);
  if (pReceiver)
  {
    pReceiver->removeFromFdToSocketHandlerMap(int(handle));
    if (timerId != 0)
    {
      int *pFd = NULL;
      if (m_reactor->cancel_timer(timerId,(const void**)&pFd))
      {
        CLog::Warning(THISMODULE, "SipProxyTCPSocketHandler::handle_close cancel_timer successful");
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

  CLog::Warning(THISMODULE, "xGateTCPSocketHandler::handle_close(%s) Removing the Address:%s:%d  Fd:%d" \
      " from Connection Map",m_serverType, conTuple.ipAddress.c_str(), conTuple.port, conTuple.fd);

  if(strcmp(m_serverType,"MG-MGC") == 0)
  {
    xGateUtil::removeFromMgConnectionMap(conTuple);  //TODO: Need to check this functionality
  }

  /// Clean up this Socket handler
  delete this;
  return 0;
}//end handle_close

int xGateTCPSocketHandler::handle_input(ACE_HANDLE aceHandle)
{
  int retVal = 0;
  CLog::Warning(THISMODULE, "xGateTCPSocketHandler::handle_input(%s)", m_serverType);

  //Reset the TCP read Buffer
  memset (m_socketData,'\0',XGATE_TCP_READ_SIZE);

  //Receive the Data
  int bufLen = -1;
  errno = 0;
  bufLen = (int)peer().recv(m_socketData,XGATE_TCP_READ_SIZE -1);
  if(0 < bufLen)
  {
    CLog::Error(THISMODULE, "xGateTCPSocketHandler::handle_input(%s):"\
        " fd:<%d> recv data:<%s> and it's Length:<%d> ",\
        m_serverType, aceHandle, m_socketData, bufLen);
    printf("\n****************************\n");
    printf("xGateTCPSocketHandler::handle_input recved data len: %d\n", bufLen);
    printf("xGateTCPSocketHandler::handle_input recved data from socket: \n%s\n", m_socketData);
    printf("****************************\n");
    m_recvdBuf->pendingMsg.append(m_socketData, (size_t)bufLen);
    m_recvdBuf->pendingMsgLen += bufLen;
    m_recvdBuf->isPending = true;
    while(m_recvdBuf->isPending) {
      if(m_recvdBuf->get_one_complete_msg() == EN_XGATE_STATUS_SUCCESS) {
        //send msg to processor after tcp decoding is done.
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
      CLog::Error(THISMODULE, "xGateTCPSocketHandler::handle_input(%s) :"\
          " fd:<%d> Read Failed error %s ", m_serverType, aceHandle, strerror(errno));
    }
    else
    {
      CLog::Error(THISMODULE, "xGateTCPSocketHandler::handle_input(%s) :"\
          " fd:<%d> Read Failed", m_serverType, aceHandle);   
    } 
    retVal = -1;
  } 

  if( (retVal == -1))
  {
    m_reactor->remove_handler(aceHandle,ACE_Event_Handler::READ_MASK); 
  }
  CLog::Error(THISMODULE, "xGateTCPSocketHandler::handle_input(%s) Successfully handled the Message", m_serverType);
  return retVal;
} 

void xGateTCPSocketHandler::post( char *pBuf,\
    int sizeOfMessage)
{
  CLog::Warning(THISMODULE, "Enter xGateTCPSocketHandler::post");
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
      CLog::Warning(THISMODULE, "xGateTCPSocketHandler::post "\
          "Enqueue Message Success");
    }
    else
    {
      CLog::Error(THISMODULE, "xGateTCPSocketHandler::post "\
          "Enqueue Message Failed");
    }
  }
  CLog::Warning(THISMODULE, "Exit  xGateTCPSocketHandler::post");
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
  CLog::Detail(THISMODULE, "Enter TcpJsonPendingMsg::get_one_complete_msg");
  int retVal = EN_XGATE_STATUS_ERROR;

  //1. find out position of end of msg(i.e) "\r\n\r\n" from given pending buffer
  size_t delimiterPos = pendingMsg.find(delimiter);

  //if(delimiterPos < 0) {
  if(delimiterPos == string::npos) {
    CLog::Error(THISMODULE, "TcpJsonPendingMsg::get_one_complete_msg, one complete msg not available \
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
    completedMsg.msg = pendingMsg.substr(0, delimiterPos);
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
//			token =  (strtok(temp,NULL));
		 }

     if(completedMsg.msgLen == 0 ){
       CLog::Error(THISMODULE, "TcpJsonPendingMsg::get_one_complete_msg, 'msg_len' hearder field \
            not found in extracted Msg: %s\n", completedMsg.msg.c_str());
						return EN_XGATE_STATUS_ERROR;
     }
    //3. get msg length
/*    size_t pos = completedMsg.msg.find("msg_len");
    string resultStr = completedMsg.msg.substr(pos+8, 5);
    if(sscanf(resultStr.c_str(), "%*[:]%d", &completedMsg.msgLen) <= 0) {
      if(sscanf(resultStr.c_str(), "%d", &completedMsg.msgLen) <= 0) {
        CLog::Error(THISMODULE, "TcpJsonPendingMsg::get_one_complete_msg, 'msg_len' hearder field \
            not found in extracted Msg: %s\n", completedMsg.msg.c_str());
        return EN_XGATE_STATUS_ERROR;
      }
    }*/

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

