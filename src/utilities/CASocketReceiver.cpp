#include "CASocketReceiver.h"
#include "LogHandler.h"

using namespace CA;

/* Method Name: CASocketReceiver
 ** Description: parameterized constructor
 ** Parameters: ACE_Reactor *reactor, CASocket* caSocket
 ** return: nil
 **/
CASocketReceiver::CASocketReceiver(ACE_Reactor *reactor, CASocket* caSocket):
   m_reactor(reactor), m_ptCASocket(caSocket)
{
   memset(m_socketData, 0, SOCKET_READ_SIZE);
   m_recvdBuf = new SocketPendingMsg();
}

/* Method Name: ~CASocketReceiver
 ** Description: destructor
 ** Parameters: void
 ** return: nil
 **/
CASocketReceiver::~CASocketReceiver(void)
{
   delete m_recvdBuf;
}

/* Method Name: registerHandle
 ** Description: To register new handle with reactor for receiving messages
 ** Parameters: ACE_SOCK_Stream& peerStream
 ** return: boolean(true|false)
 **/
bool CASocketReceiver::registerHandle(ACE_SOCK_Stream& peerStream)
{
   loginfo("","CASocketReceiver::registerHandle called");
   m_peer = peerStream;
   m_peer.get_local_addr(m_localAddr);
   m_peer.get_remote_addr(m_remoteAddr);

   char ipAddr[INET6_ADDRSTRLEN+1];
   memset(ipAddr,0,INET6_ADDRSTRLEN+1);
   m_remoteAddr.get_host_addr(ipAddr, INET6_ADDRSTRLEN+1);

   errno=0;
   if(m_reactor->register_handler(m_peer.get_handle(),this, ACE_Event_Handler::READ_MASK)==-1)
   {
      loginfo("","CASocketReceiver::registerHandle failed to register ACD ip: %s with reactor, due to Error:%s",\
	    ipAddr, strerror(errno));
      return false;
   }

   CAConnInfo connInfo;
   connInfo.ipAddress = m_remoteAddr.get_host_addr();
   connInfo.port      = m_remoteAddr.get_port_number();
   connInfo.fd        = m_peer.get_handle();
   connInfo.srcPort   = m_localAddr.get_port_number();
   loginfo("","CASocketReceiver::registerHandle Connection Success for IP: (%s),Port: (%hu),fd: (%d) and srcPort: (%hu)",connInfo.ipAddress.c_str(),connInfo.port,connInfo.fd,connInfo.srcPort);
   //if(m_ptCASocket->getSocketType() == EN_CA_SOCKET_TYPE::EN_CA_SOCKET_TYPE_SERVER)
   //{
      m_ptCASocket->insertConnectedList(m_remoteAddr.get_host_addr());
   //}
   if(!m_ptCASocket->getSocketHandler()->insertConnectionInfo(connInfo))
   {
      loginfo("","CASocketReceiver::registerHandle failed to insert connection info in map");
   }
   else
   {
      loginfo("","CASocketReceiver::registerHandle Successfully inserted connection info in map");
   }

   int keepAlive = 1;
   this->peer().set_option(SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive));
   struct timeval timeout;
   timeout.tv_sec = 5;
   timeout.tv_usec = 0;
   if(setsockopt(m_peer.get_handle(), SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
   {
      loginfo("","CASocketReceiver::registerHandle set_option for SO_SNDTIMEO failed");
   }
   return true;
}

/* Method Name: registerSctpHandle
 ** Description: To register new handle with reactor for receiving messages
 ** Parameters: ACE_SOCK_SEQPACK_Association& peerStream
 ** return: boolean(true|false)
 **/
bool CASocketReceiver::registerSctpHandle(ACE_SOCK_SEQPACK_Association& peerStream)
{
   loginfo("","CASocketReceiver::registerSctpHandle called");
   m_peerSctp = peerStream;
   m_peerSctp.get_local_addr(m_localAddr);
   m_peerSctp.get_remote_addr(m_remoteAddr);

   char ipAddr[INET6_ADDRSTRLEN+1];
   memset(ipAddr,0,INET6_ADDRSTRLEN+1);
   m_remoteAddr.get_host_addr(ipAddr, INET6_ADDRSTRLEN+1);

   errno=0;
   if(m_reactor->register_handler(m_peerSctp.get_handle(),this, ACE_Event_Handler::READ_MASK)==-1)
   {
      loginfo("","registerSctpHandle failed to register ACD ip: %s with reactor, due to Error:%s",\
	    ipAddr, strerror(errno));
      return false;
   }

   CAConnInfo connInfo;
   connInfo.ipAddress = m_remoteAddr.get_host_addr();
   connInfo.port      = m_remoteAddr.get_port_number();
   connInfo.fd        = m_peer.get_handle();
   connInfo.srcPort   = m_localAddr.get_port_number();

   if(!m_ptCASocket->getSocketHandler()->insertConnectionInfo(connInfo))
   {
      loginfo("","registerSctpHandle failed to insert connection info in map");
   }

   int keepAlive = 1;
   this->peer().set_option(SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive));
   struct timeval timeout;
   timeout.tv_sec = 5;
   timeout.tv_usec = 0;
   if(setsockopt(m_peerSctp.get_handle(), SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
   {
      loginfo("","registerSctpHandle set_option for SO_SNDTIMEO failed");
   }
   return true;
}

/* Method Name: get_handle
 ** Description: To fetch the peer handle
 ** Parameters: void
 ** return: ACE_HANDLE
 **/
ACE_HANDLE CASocketReceiver::get_handle(void) const
{
   return this->m_peer.get_handle();
}

/* Method Name: peer
 ** Description: To fetch the peer stream
 ** Parameters: nil
 ** return: ACE_SOCK_Stream
 **/
ACE_SOCK_Stream& CASocketReceiver::peer() const
{
   return (ACE_SOCK_Stream &) this->m_peer;
}

/* Method Name: get_handle
 ** Description: To fetch the peer handle
 ** Parameters: void
 ** return: ACE_HANDLE
 **/
ACE_HANDLE CASocketReceiver::get_sctpHandle(void) const
{
   return this->m_peerSctp.get_handle();
}

/* Method Name: peer
 ** Description: To fetch the peer stream
 ** Parameters: nil
 ** return: ACE_SOCK_SEQPACK_Association
 **/
ACE_SOCK_SEQPACK_Association& CASocketReceiver::peerSctp() const
{
   return (ACE_SOCK_SEQPACK_Association &) this->m_peerSctp;
}

/* Method Name: handle_close
 ** Description: To process handle close callback
 ** Parameters: ACE_HANDLE handle, ACE_Reactor_Mask close_mask
 ** return: int
 **/
int CASocketReceiver::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
   loginfo("","CASocketReceiver::handle_close called, connection closed on handle %d", handle);
   m_reactor->cancel_timer(m_ptCASocket->m_iTimerId);

   this->peer().close();

   CAConnInfo connInfo;
   char ipAddr[INET6_ADDRSTRLEN+1];
   memset(ipAddr,0,INET6_ADDRSTRLEN+1);
   m_remoteAddr.get_host_addr(ipAddr, INET6_ADDRSTRLEN+1);
   connInfo.ipAddress    = ipAddr;
   connInfo.port         = m_remoteAddr.get_port_number();
   connInfo.fd           = handle;
   connInfo.srcPort      = m_localAddr.get_port_number();

   loginfo("","CASocketReceiver::handle_close Removing the Address:%s Port:%d  Handle:%d from Connection Map",\
	 connInfo.ipAddress.c_str(), connInfo.port, connInfo.fd);
   m_ptCASocket->removeConnectedList(connInfo.ipAddress);
   
   if(!m_ptCASocket->getSocketHandler()->eraseConnectionInfo(connInfo))
   {
      loginfo("","CASocketReceiver::handle_close failed to erase connection info from map");
   }
   if(m_ptCASocket->getSocketController())
   {
      std::unique_ptr<CASocketMsg> pSocketMsg(new CASocketMsg());
      pSocketMsg->setClientIP(m_remoteAddr.get_host_addr());
      pSocketMsg->setClientPort(m_remoteAddr.get_port_number());
      pSocketMsg->setMsgType(m_ptCASocket->getCloseMsgType());
      m_ptCASocket->getSocketController()->PostMessage(std::move(pSocketMsg));
   }
   m_reactor->remove_handler(handle, ACE_Event_Handler::READ_MASK);
   delete this;
   return 0;
}

/* Method Name: handle_input
 ** Description: To process handle input callback
 ** Parameters: ACE_HANDLE aceHandle
 ** return: int
 **/
int CASocketReceiver::handle_input(ACE_HANDLE aceHandle)
{
   loginfo("","CASocketReceiver::handle_input called");
   int retVal = 0;
   memset (m_socketData,'\0', SOCKET_READ_SIZE);

   int bufLen = -1;
   errno = 0;
   bufLen = (int)peer().recv(m_socketData, SOCKET_READ_SIZE -1);
   if(0 < bufLen)
   {
      m_recvdBuf->pendingMsg.append(m_socketData, (size_t)bufLen);
      m_recvdBuf->pendingMsgLen += bufLen;
      m_recvdBuf->isPending = true;
      while(m_recvdBuf->isPending)
      {
	 if(m_recvdBuf->getCompleteMsg())
	 {
	    loginfo("","handle_input for Handle:<%d>  Length:<%d> recv Data:<%s>",\
		  aceHandle,bufLen, m_recvdBuf->completeMsg.msg.c_str());
       if(!m_ptCASocket->getSocketController())
       {
          loginfo("","CASocketReceiver::handle_input socket controller instance is null");
       }
       else
       {
	 loginfo("","CASocketReceiver::handle_input socket controller instance is found");
         std::unique_ptr<CASocketMsg> pSocketMsg(new CASocketMsg());
         pSocketMsg->setData(m_recvdBuf->completeMsg.msg.c_str());
         pSocketMsg->setClientIP(m_remoteAddr.get_host_addr());
         pSocketMsg->setClientPort(m_remoteAddr.get_port_number());
	      pSocketMsg->setMsgType(m_ptCASocket->getSrcMsgType());
         m_ptCASocket->getSocketController()->PostMessage(std::move(pSocketMsg));
	 loginfo("","CASocketReceiver::handle_input Posted Message");
       }
	 }
	 else
	 {
	    break;
	 }
      }
   }
   else
   {
      if (bufLen == 0)
      {
         loginfo("", "CASocketReceiver::handle_input Handle:<%d> peer closed connection", aceHandle);
      } 
      else if (errno != 0) 
      {
         loginfo("", "CASocketReceiver::handle_input Handle:<%d> recv failed: %s", aceHandle, strerror(errno));
      }
      else 
      {
         loginfo("", "CASocketReceiver::handle_input Handle:<%d> unknown recv failure", aceHandle);
      } 
      retVal = -1;
   }

   if( (retVal == -1))
   {
      m_reactor->remove_handler(aceHandle,ACE_Event_Handler::READ_MASK);
   }
   return retVal;
}

/* Method Name: SocketPendingMsg
 ** Description: constructor
 ** Parameters: nil
 ** return: nil
 **/
SocketPendingMsg::SocketPendingMsg(): 
isPending(false), delimiter("\r\n\r\n"), pendingMsg(""), pendingMsgLen(0), completeMsg{}
{}

/* Method Name: ~SocketPendingMsg
 ** Description: destructor
 ** Parameters: nil
 ** return: nil
 **/
SocketPendingMsg::~SocketPendingMsg()
{}

/* Method Name: getCompleteMsg
 ** Description: To fetch complete message from socket
 ** Parameters: nil 
 ** return: boolean(true|false)
 **/
bool SocketPendingMsg::getCompleteMsg()
{
   loginfo("","SocketPendingMsg::getCompleteMsg called");
   size_t delimiterPos = pendingMsg.find(delimiter);

   if(delimiterPos == std::string::npos)
   {
      loginfo("","getCompleteMsg, one complete msg not available in given pending buffer: %s\n",\
	    pendingMsg.c_str());
      if((pendingMsg.length() > 0))
      {
	 isPending = true;
	 pendingMsgLen = pendingMsg.length();
      }
      return false;
   }
   else
   {
      completeMsg.msg = pendingMsg.substr(0, delimiterPos);
      size_t pos = completeMsg.msg.find("msg_len");
      std::string resultStr = completeMsg.msg.substr(pos+8, 5);
      if(sscanf(resultStr.c_str(), "%*[:]%d", &completeMsg.msgLen) <= 0)
      {
	 if(sscanf(resultStr.c_str(), "%d", &completeMsg.msgLen) <= 0)
	 {
	    loginfo("","getCompleteMsg, 'msg_len' hearder field not found in extracted Msg: %s\n",\
		  completeMsg.msg.c_str());
	    return false;
	 }
      }

      size_t nextMsgStartPos = delimiterPos + delimiter.length();
      pendingMsg.erase(0, nextMsgStartPos);

      if((completeMsg.msgLen == (delimiterPos)) && (pendingMsg.length() <= 0))
      {
	 isPending = false;
	 pendingMsgLen = 0;
	 return true;
      }

      if((completeMsg.msgLen == (delimiterPos)) && (pendingMsg.length() > 0))
      {
	 isPending = true;
	 pendingMsgLen = pendingMsg.length();
	 return true;
      }

      if((pendingMsg.length() > 0))
      {
	 isPending = true;
	 pendingMsgLen = pendingMsg.length();
	 return false;
      }
   }
   return false;
}


