#include "CASocket.h"
#include "LogHandler.h"

using namespace CA;

/* Method Name: CASocket
 ** Description: parameterized constructor
 ** Parameters: EN_CA_SOCKET_TYPE socketType, unsigned short port, EN_CA_SOCKET_PROTOCOL protocol, std::vector<std::string> ipList
 ** return: nil
 **/
CASocket::CASocket(EN_CA_SOCKET_TYPE socketType, unsigned short port, EN_CA_SOCKET_PROTOCOL protocol, \
		CAThread* ptCASocketController, unsigned short connTimeout, unsigned short resTimeout, \
		std::vector<std::string> ipList, int srcMsgType, int closeMsgType):
   m_reactor(NULL)
{
   loginfo("","CASocket::CASocket called");
   m_reactor = new ACE_Reactor();
   m_ptCASocketContext = new CASocketContext();
   if(!ipList.empty())
   {
      m_vConnectList.assign(ipList.begin(), ipList.end());
      m_vConnectedList.assign(ipList.begin(), ipList.end());
   }
   m_iConnectPort = port;
   m_enConnectProtocol = protocol;
   m_enSocketType = socketType;
   m_ptCASocketController = ptCASocketController;
   m_iConnectionTimeout = connTimeout;
   m_iResponseTimeout = resTimeout;
   m_iSrcMsgType = srcMsgType;
   m_iCloseMsgType = closeMsgType;

   if(!init())
   {
      loginfo("","CASocket failed to initiate socket connection");
   }
}

/* Method Name: ~CASocket
 ** Description: detructor
 ** Parameters: nil
 ** return: nil
 **/
CASocket::~CASocket()
{
   loginfo("","CASocket::~CASocket called");
   if(m_reactor)
   {
      delete m_reactor;
   }
   if(m_ptCASocketContext)
   {
      delete m_ptCASocketContext;
      m_ptCASocketContext = nullptr;
   }
   if(m_ptCASocketController)
   {
      delete m_ptCASocketController;
      m_ptCASocketController = nullptr;
   }
   m_running = false; 
}

/* Method Name: run
 ** Description: To start event loop for thread
 ** Parameters: void* threadClass
 ** return: void
 **/
void CASocket::run(void* threadClass)
{
   CASocket* pCASocket = static_cast<CASocket*>(threadClass);
   pCASocket->m_running = true;
   pCASocket->m_reactor->owner(ACE_Thread::self());
   pCASocket->m_reactor->run_reactor_event_loop();
   pCASocket->m_running = false;
}

/* Method Name: stopThread
 ** Description: To stop thread
 ** Parameters: void
 ** return: boolean(true|false)
 **/
bool CASocket::stopThread(void)
{
   loginfo("","CASocket::stopThread called");
   m_reactor->end_reactor_event_loop();
   while (m_running)
   {
      sleep(1);
   }
   return true;
}

/* Method Name: init
 ** Description: To initiate socket as client or server based on argument
 ** Parameters: nil
 ** return: boolean(true|false)
 **/
bool CASocket::init()
{
   loginfo("","CASocket::init called");
   if(ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,(void*) this,THR_NEW_LWP|THR_DETACHED) == -1)
   {
      loginfo("","CASocket::init failed");
      return false;
   }
   if((m_iConnectPort==0))
   {
      loginfo("","CASocket failed to init socket - port(%d) is empty", m_iConnectPort);
      return false;
   }
   if(m_enSocketType == EN_CA_SOCKET_TYPE::EN_CA_SOCKET_TYPE_CLIENT)
   {
      loginfo("","CASocket CLIENT initiated");
      if(m_vConnectList.empty())
      {
         loginfo("","CASocket falied to initiate connector - connection list is empty");
	 return false;
      }
   }
   else if(m_enSocketType == EN_CA_SOCKET_TYPE::EN_CA_SOCKET_TYPE_SERVER)
   {
      loginfo("","CASocket SERVER initiated");
      if(!acceptor())
      {
         loginfo("","CASocket init acceptor failed.");
      }
   }
   else
   {
      loginfo("","CASocket init socket type is empty or invalid");
   }
   return true;
}

/* Method Name: acceptor
 ** Description: To initiate server socket connection
 ** Parameters: nil
 ** return: boolean(true|false)
 **/
bool CASocket::acceptor()
{
   loginfo("","CASocket::acceptor called");
   std::string ipAddr = getLocalIpAddr(AF_INET);
   ACE_INET_Addr addr(m_iConnectPort, ipAddr.c_str());
   errno=0;

   if(m_enConnectProtocol == EN_CA_SOCKET_PROTOCOL::EN_CA_SOCKET_PROTOCOL_TCP)
   {
      ACE_SOCK_Acceptor * acceptor = new ACE_SOCK_Acceptor(addr,1);
      if(errno != 0 )
      {
	 loginfo("","acceptor falied to initiate new ace socket acceptor for address -> %s port -> %d", ipAddr.c_str(), m_iConnectPort);
	 return false;
      }
      m_acceptorList.push_back(acceptor);
      errno = 0;

   if(m_reactor->register_handler(acceptor->get_handle(), this, ACE_Event_Handler::READ_MASK)==-1)
   {
      loginfo("","acceptor failed to register handler with ace event handler for address -> %s port -> %d", ipAddr.c_str(), m_iConnectPort);
      return false;
   }
      loginfo("","acceptor Success to initiate new ace socket acceptor for address -> %s port -> %d", ipAddr.c_str(), m_iConnectPort);
   }
   return true;
}

/* Method Name: handle_close
 ** Description: To process handle close callback
 ** Parameters: ACE_HANDLE handle, ACE_Reactor_Mask close_mask
 ** return: int
 **/
int CASocket::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
   loginfo("","CASocket::handle_close called");
   return 0;
}

/* Method Name: handle_input
 ** Description: To process handle input callback for acception new connection from client
 ** Parameters: ACE_HANDLE aceHandle
 ** return: int
 **/
int CASocket::handle_input(ACE_HANDLE aceHandle)
{
   loginfo("","CASocket::handle_input called");
   unsigned int index= 0;
   if(m_enConnectProtocol == EN_CA_SOCKET_PROTOCOL::EN_CA_SOCKET_PROTOCOL_TCP){
      ACE_SOCK_Stream socketStream;
      for(index=0;index<m_acceptorList.size();index++){
         if(aceHandle == m_acceptorList[index]->get_handle()){
	         break;
         }
      }
      if(index >= m_acceptorList.size()){
         loginfo("","CASocket::handle_input Received connection handle is invalid :%d", aceHandle);
         return 0;
      }
      m_acceptorList[index]->accept(socketStream);
      ACE_INET_Addr remoteAddr;
      socketStream.get_remote_addr(remoteAddr);
      CASocketReceiver *ptCASocketReceiver = new CASocketReceiver(m_reactor, this);
      if(ptCASocketReceiver == NULL){
         loginfo("","CASocket::handle_input Failed to get the Receiver");
         socketStream.close();
         return 0;
      }
      if(!ptCASocketReceiver->registerHandle(socketStream)){
         loginfo("","CASocket::handle_input  registerHandle Failed");
         socketStream.close();
         delete ptCASocketReceiver;
         ptCASocketReceiver = NULL;
         return 0;
      }
   }
   return 0;
}

/* Method Name: sendData
 ** Description: To send given data to socket based on ip address and port
 ** Parameters: const char *data, int len, std::string ipAddr, unsigned short port
 ** return: boolean(true|false)
 **/
bool CASocket::sendData(const char *data, int len, std::string ipAddr, unsigned short port)
{
   loginfo("","CASocket::sendData called");
   int handle = 0;

   if(m_ptCASocketContext->getConnectionInfo(m_enSocketType, ipAddr, port, handle))
   {
      if(send(handle, data, len, MSG_DONTWAIT) == -1)
      {
	 loginfo("","sendData failed for address : %s, port : %d",ipAddr.c_str(), port);
	 return false;
      }
   }
   else
   {
      loginfo("","sendData no connection present for address : %s, port : %d",ipAddr.c_str(), port);
      return false;
   }
   return true;
}

/* Method Name: validateSocketConnection
 ** Description: To send given data to socket based on ip address and port
 ** Parameters: const char *data, int len, std::string ipAddr, unsigned short port
 ** return: boolean(true|false)
 **/
bool CASocket::validateSocketConnection(STR data)
{
   loginfo("","CASocket::validateSocketConnection called");
   int len = data.length();
   std::vector<STR>::iterator itr = m_vConnectList.begin();
   STR ip("");
   while(itr!=m_vConnectList.end())
   {
      ip = *itr;
      if(sendData(data.c_str(), len, *itr, m_iConnectPort))
      {
         loginfo("","validateSocketConnection socket validate message send success - IP(%s) PORT(%d)", ip.c_str(), m_iConnectPort);
      }
      else
      {
         loginfo("","validateSocketConnection socket validate message send failure - IP(%s) PORT(%d)", ip.c_str(), m_iConnectPort);
      }
      itr++;
   }
   SocketTimerData *timerData = new SocketTimerData();
	timerData->m_timerType = EN_CA_TIMER_TYPE::EN_CA_TIMER_SOCKET_VALIDATE;
   timerData->m_strData = data;
	m_reactor->schedule_timer(this, (void*)timerData, ACE_Time_Value(m_iConnectionTimeout, 0));
   return true;
}

char CASocket::m_localIp4Addr[INET_ADDRSTRLEN] = {0};
char CASocket::m_localIp6Addr[INET6_ADDRSTRLEN] = {0};

/* Method Name: getLocalIpAddr
 ** Description: To fetch local IP address
 ** Parameters: unsigned short AddrType 
 ** return: char*
 **/
char* CASocket::getLocalIpAddr(unsigned short AddrType) 
{
   loginfo("","CASocket::getLocalIpAddr called");
   switch(AddrType) 
   {
      case AF_INET:
	 {
	    if(m_localIp4Addr[0] != 0) 
	    {
	       return m_localIp4Addr;
	    } 
	    else 
	    {
	       findLocalIpAddr(m_localIp4Addr, AF_INET);
	       return m_localIp4Addr;
	    }
	    break;
	 }
      case AF_INET6:
	 {
	    if(m_localIp6Addr[0] != 0) 
	    {
	       return m_localIp6Addr;
	    } 
	    else 
	    {
	       findLocalIpAddr(m_localIp6Addr, AF_INET6);
	       return m_localIp6Addr;
	    }
	    break;
	 }
      default:
	 {
	    printf("getLocalIpAddr, given address type not supported\n");
	    loginfo("","CASocket::getLocalIpAddr, given address type not supported");
	    break;
	 }
   }
   return NULL;
}

/* Method Name: findLocalIpAddr
 ** Description: To find local IP address
 ** Parameters: char *ipAddr, unsigned short AddrType
 ** return: boolean(true|false)
 **/
bool CASocket::findLocalIpAddr(char *ipAddr, unsigned short AddrType)
{
   loginfo("","CASocket::getLocalIpAddr called");
   struct ifaddrs * ifAddrStruct=NULL;
   struct ifaddrs * ifa=NULL;
   void * tmpAddrPtr=NULL;

   if(ipAddr == NULL) 
   {
      return false;
   }

   getifaddrs(&ifAddrStruct);
   for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) 
   {
      if (!ifa->ifa_addr || \
	    strcmp(ifa->ifa_name, "lo") == 0 || \
	    ifa->ifa_addr->sa_family != AddrType) 
      {
	 continue;
      }
      std::string startStr = "fe80";
      if (AF_INET == AddrType) 
      {
	 tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
	 inet_ntop(AF_INET, tmpAddrPtr, ipAddr, INET_ADDRSTRLEN);
	 printf("findLocalIpAddr, %s IP4 Address %s\n", ifa->ifa_name, ipAddr);
	 loginfo("","findLocalIpAddr, %s IP4 Address %s", ifa->ifa_name, ipAddr);
	 break;
      } 
      else if (AF_INET6 == AddrType) 
      {
	 tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
	 inet_ntop(AF_INET6, tmpAddrPtr, ipAddr, INET6_ADDRSTRLEN);
	 if (std::string(ipAddr).substr(0, startStr.length()) == startStr) continue;
	 printf("findLocalIpAddr, %s IP6 Address %s\n", ifa->ifa_name, ipAddr);
	 loginfo("","findLocalIpAddr, %s IP6 Address %s", ifa->ifa_name, ipAddr);
	 break;
      }
   }
   if(ifAddrStruct!=NULL) 
   {
      freeifaddrs(ifAddrStruct);
   }
   return true;
}
