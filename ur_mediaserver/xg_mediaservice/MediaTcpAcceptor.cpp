//local includes
#include "xglog.h"
#include "MediaTcpReceiver.h"

//self includes
#include "xGateMediaServiceMsg.h"
#include "xGateUtil.h"
#include "MediaServiceDefine.h"
#include "xGateMediaService.h"
#include "MediaTcpAcceptor.h"

MediaTcpAcceptor::MediaTcpAcceptor():
  ACE_Event_Handler(),
  m_running(false),
  m_reactor(NULL)
{
  m_reactor = new ACE_Reactor();
}

MediaTcpAcceptor::~MediaTcpAcceptor(void)
{
  for(unsigned int i =0;i<m_acceptorList.size();i++) {
     if (m_acceptorList[i])
         delete m_acceptorList[i];
  }
  if(m_reactor)
    delete m_reactor;

  m_running = false;
}

xGateRetVal MediaTcpAcceptor::add_new_server(string Ip,int port,int dstModuleId)
{

    //Server Address
    
    ACE_Write_Guard<ACE_Recursive_Thread_Mutex> m_write_lock(m_tMtxServerList,true); 	
    ACE_INET_Addr addr(port, Ip.c_str());
    XGLOG_DEBUG("TcpAcceptor start listening on tcp address: %s:%d", Ip.c_str(), port);

    errno=0;
    ACE_SOCK_Acceptor * acceptor = new ACE_SOCK_Acceptor(addr,1);
    if(errno != 0) {
      XGLOG_ERROR("Initialization failed whlle opening the socket port: %d with error: %d %s", \
          port, errno, strerror(errno));
      return EN_XGATE_STATUS_ACE_ERROR;
    }
    m_acceptorList.push_back(acceptor);
    string conn_addr = Ip + ":" + std::to_string(port);
    m_acceptorModuleId.insert({conn_addr,dstModuleId});
    
    errno = 0;
    if(m_reactor->register_handler(acceptor->get_handle(),
          this,
          ACE_Event_Handler::READ_MASK)==-1) {
      XGLOG_ERROR("Initialization failed while registering TcpSocketHandler with error %d %s", \
          errno,strerror(errno));
      return EN_XGATE_STATUS_ACE_ERROR;
    }  
   return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal MediaTcpAcceptor::init()
{

  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1) {
      XGLOG_ERROR("initialization failed while spawning ace thread !");
      return EN_XGATE_STATUS_ACE_ERROR;
     }
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal MediaTcpAcceptor::send_new_client(unsigned int connectId,int dstModuleId,
                                        string Ip,int port)
{
 XGLOG_INFO("MediaTcpAcceptor::send_new_client posting EN_XGATE_MSG_NEW_CLIENT msg with conn_id:%d !",connectId);
 xGateMediaServiceMsg *pMsg = new xGateMediaServiceMsg();
 xGateNetConnection netConnInfo;
 pMsg->setMsgType((xGateMsgType)EN_XGATE_MSG_NEW_CLIENT);
 pMsg->setSrcModuleId(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE);
 pMsg->setDstModuleId((IURDefines::MODULE_ID)dstModuleId);
 pMsg->setConnectionId(connectId);
 netConnInfo.recvIp_ = Ip;
 netConnInfo.recvPort_ = port;
 pMsg->set_netConnInfo(netConnInfo);
 MEDIASERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
 return EN_XGATE_STATUS_SUCCESS;
}	

void MediaTcpAcceptor::run(void* threadClass)
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

int MediaTcpAcceptor::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
  XGLOG_DEBUG("handle_close triggered for ace handle: %d", handle);
  return 0;
}

int MediaTcpAcceptor::handle_connection(ACE_HANDLE aceHandle)
{
  XGLOG_DEBUG("Handling new media-gateway connection request on ace handle: %d", aceHandle);
  ACE_Read_Guard<ACE_Recursive_Thread_Mutex> m_read_lock(m_tMtxServerList,true); 	
  ACE_SOCK_Stream newPeerStream;
  unsigned int index= 0;

  for(index=0;index<m_acceptorList.size();index++) {
    if(aceHandle == m_acceptorList[index]->get_handle()) {
      break;
    }
  }

  if(index >= m_acceptorList.size()) {
    XGLOG_DEBUG("New media-gateway connection request ignored due to invalid handle %d", aceHandle);
    return 0;
  }

  m_acceptorList[index]->accept(newPeerStream);

  ACE_INET_Addr localAddr;
  ACE_INET_Addr remoteAddr;
  newPeerStream.get_local_addr(localAddr);
  newPeerStream.get_remote_addr(remoteAddr);
  char peerIp[INET6_ADDRSTRLEN+1];
  memset(peerIp, 0, INET6_ADDRSTRLEN+1);
  //remoteAddr.get_host_addr(peerIp, INET6_ADDRSTRLEN+1);
  localAddr.get_host_addr(peerIp, INET6_ADDRSTRLEN+1);
  unsigned short peerPort = remoteAddr.get_port_number();
  unsigned short fd = newPeerStream.get_handle();

  if(MEDIATCPRECEIVER->get_reactor() == NULL) {
    XGLOG_ERROR("Handle connection from peer %s:%d failed due to invalid reactor in MediaTcpReceiver !", \
        peerIp, peerPort);
    newPeerStream.close();
    return 0;
  }
      map<string,int>::iterator it;
      int moduleId = 0;
#if 0
      string m_peerIp("10.22.3.229");
      string m_connDetails = m_peerIp + ":" + std::to_string(50000);
#else
      string m_peerIp(peerIp);
      unsigned short m_peerPort = localAddr.get_port_number();
      string m_connDetails = m_peerIp + ":" + std::to_string(m_peerPort);
#endif      
      XGLOG_WARN("MediaTcpAcceptor::handle_connection ============> m_connDetails : %s <================",m_connDetails.c_str());
      if((it = m_acceptorModuleId.find(m_connDetails)) != m_acceptorModuleId.end()){
	 moduleId = it->second;      
         //TODO:Erase entry from map m_acceptorModuleId
         //remove_entry_from_acceptor_moduleId_map(m_connDetails);

       } 
       else
       {
          XGLOG_ERROR("Could not find srcModuleId in map");
          return EN_XGATE_STATUS_ERROR;   	  
       }
       unsigned int connId = 0;
       if(!MEDIAUTIL->getConnectionId(connId)){
	 XGLOG_ERROR("Could not get Connection Id");
         return EN_XGATE_STATUS_ERROR;	 
       }	       
  /// Create the Connection handler
  /// Register the connection with appropriate Receiver
  MediaTcpSocketHandler* socketHandler =
    new MediaTcpSocketHandler(MEDIATCPRECEIVER->get_reactor(),moduleId,connId);

  //Initialize the new eventHandler object with the handle to the socket stream
  if(socketHandler->init(newPeerStream) != EN_XGATE_STATUS_SUCCESS) {
    XGLOG_ERROR("handle connection from peer %s:%d failed due to MediaTcpSocketHandler initialization failure !",\
        peerIp, peerPort);
    newPeerStream.close();
    delete socketHandler;
    return 0;
  }
  send_new_client(connId,moduleId,m_peerIp,peerPort);
  XGLOG_DEBUG("Sucessfully handled new connection request from peer %s:%d on fd: %d", \
  peerIp, peerPort, fd);
  return 0;
}

bool MediaTcpAcceptor::remove_entry_from_acceptor_moduleId_map(string & m_connDetails)
{
  if(m_connDetails.empty())
  {
    XGLOG_ERROR( "MediaTcpAcceptor::remove_entry_from_acceptor_moduleId_map failed. m_connDetails is empty !");
    return false;
  }

  ACE_Write_Guard<ACE_Recursive_Thread_Mutex> m_write_lock(m_tMtxServerList,true);
  map<string,int>::iterator it = m_acceptorModuleId.find(m_connDetails);
  if(it != m_acceptorModuleId.end())
  {
     m_acceptorModuleId.erase(it);
     XGLOG_INFO( "MediaTcpAcceptor::remove_entry_from_acceptor_moduleId_map success for m_connDetails(%s) !",m_connDetails.c_str());
     return true;
  }

  return false;
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

