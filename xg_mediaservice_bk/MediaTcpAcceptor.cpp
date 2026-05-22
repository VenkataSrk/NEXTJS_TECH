//local includes
#include "xglog.h"
#include "MediaTcpReceiver.h"

//self includes
#include "MediaTcpAcceptor.h"

MediaTcpAcceptor::MediaTcpAcceptor ():
  ACE_Event_Handler(),
  m_running(false),
  m_reactor(NULL)
{
  m_reactor = new ACE_Reactor();
}

MediaTcpAcceptor::~MediaTcpAcceptor (void)
{
  for(unsigned int i =0;i<m_acceptorList.size();i++) {
     if (m_acceptorList[i])
         delete m_acceptorList[i];
  }
  if(m_reactor)
    delete m_reactor;

  m_running = false;
}

xGateRetVal MediaTcpAcceptor::init (vector<TcpLisentAddr> listenAddressList)
{
  for(unsigned int i =0 ; i < listenAddressList.size();i++) {
    unsigned short port = listenAddressList[i].m_port;
    string ip = listenAddressList[i].m_ip;

    //Server Address
    //ACE_INET_Addr addr(port, ip.c_str(),AF_INET6);
    ACE_INET_Addr addr(port);
    XGLOG_DEBUG("TcpAcceptor start listening on tcp address: %s:%d", ip.c_str(), port);

    errno=0;
    ACE_SOCK_Acceptor * acceptor = new ACE_SOCK_Acceptor(addr,1);
    if(errno != 0) {
      XGLOG_ERROR("Initialization failed whlle opening the socket port: %d with error: %d %s", \
          port, errno, strerror(errno));
      return EN_XGATE_STATUS_ACE_ERROR;
    }
    m_acceptorList.push_back(acceptor);
    
    errno = 0;
    if(m_reactor->register_handler(acceptor->get_handle(),
          this,
          ACE_Event_Handler::READ_MASK)==-1) {
      XGLOG_ERROR("Initialization failed while registering TcpSocketHandler with error %d %s", \
          errno,strerror(errno));
      return EN_XGATE_STATUS_ACE_ERROR;
    }
  }  

  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1) {
    XGLOG_ERROR("initialization failed while spawning ace thread !");
    return EN_XGATE_STATUS_ACE_ERROR;
  }
	return EN_XGATE_STATUS_SUCCESS;
}

void MediaTcpAcceptor::run (void* threadClass)
{
  XGLOG_DEBUG("starting event processing loop");
  MediaTcpAcceptor *pReceiver = static_cast<MediaTcpAcceptor*>(threadClass);
  pReceiver->m_running = true;
  pReceiver->m_reactor->owner(ACE_Thread::self());
  pReceiver->m_reactor->run_reactor_event_loop();
  XGLOG_DEBUG("run event loop is stopped");
  pReceiver->m_running = false;
}

int MediaTcpAcceptor::handle_input(ACE_HANDLE aceHandle)
{
  handle_connection(aceHandle);
  return 0;
}

int MediaTcpAcceptor::handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
  XGLOG_DEBUG("handle_close triggered for ace handle: %d", handle);
  return 0;
}

int MediaTcpAcceptor::handle_connection(ACE_HANDLE aceHandle)
{
  //XGLOG_DEBUG("Handling new media-gateway connection request on ace handle: %d", aceHandle);
  //return 0;//no need to execute below part because this is only for monitoring
  ACE_SOCK_Stream newPeerStream;
  unsigned int index= 0;
 // for(index=0;index<m_acceptorList.size();index++) {
 //   if(aceHandle == m_acceptorList[index]->get_handle()) {
 //     break;
 //   }
 // }

 // if(index >= m_acceptorList.size()) {
 //   XGLOG_DEBUG("New media-gateway connection request ignored due to invalid handle %d", aceHandle);
 //   return 0;
 // }

  m_acceptorList[index]->accept(newPeerStream);
  //newPeerStream.close();
#if 0
  ACE_INET_Addr remoteAddr;
  newPeerStream.get_remote_addr(remoteAddr);
  char peerIp[INET6_ADDRSTRLEN+1];
  memset(peerIp, 0, INET6_ADDRSTRLEN+1);
  remoteAddr.get_host_addr(peerIp, INET6_ADDRSTRLEN+1);
  unsigned short peerPort = remoteAddr.get_port_number();
  unsigned short fd = newPeerStream.get_handle();
#endif
#if 1
  if(MEDIATCPRECEIVER->get_reactor() == NULL) {
    //XGLOG_ERROR("Handle connection from peer %s:%d failed due to invalid reactor in MediaTcpReceiver !", \
        peerIp, peerPort);
    newPeerStream.close();
    return 0;
  }
#endif

  /// Create the Connection handler
  /// Register the connection with appropriate Receiver
#if 1
 MediaTcpSocketHandler* socketHandler =
   new MediaTcpSocketHandler(MEDIATCPRECEIVER->get_reactor());

 //Initialize the new eventHandler object with the handle to the socket stream
 if(socketHandler->init(newPeerStream) != EN_XGATE_STATUS_SUCCESS) {
   /*XGLOG_ERROR("handle connection from peer %s:%d failed due to MediaTcpSocketHandler initialization failure !",\
     peerIp, peerPort);*/
   newPeerStream.close();
   delete socketHandler;
   return 0;
 }
 /*XGLOG_DEBUG("Sucessfully handled new connection request from peer %s:%d on fd: %d", \
  peerIp, peerPort, fd);*/
#endif
  return 0;
}

xGateRetVal MediaTcpAcceptor::stop_thread(void)
{
  XGLOG_DEBUG("Stopping MediaTcpAcceptor thread");
  m_reactor->end_reactor_event_loop();
  while (m_running) {
    sleep(1);
  }
  XGLOG_DEBUG("Stopped MediaTcpAcceptor thread !");
  return EN_XGATE_STATUS_SUCCESS;
}

int MediaTcpAcceptor::handle_timeout(const ACE_Time_Value&, const void *arg)
{
  XGLOG_DEBUG("handle_timeout called");
  return 0;
}

