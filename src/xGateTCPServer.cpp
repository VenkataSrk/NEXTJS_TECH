
//local includes
#include "xglog.h"
#include "xGateUtil.h"
//self includes
#include "../include/xGateTCPServer.h"

#define THISMODULE "TCPSrvr"

xGateTCPServer::xGateTCPServer (const char* serverType):
  ACE_Event_Handler(),
  m_running(false),
  m_reactor(NULL)
{
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);
  m_reactor = new ACE_Reactor();
  XGLOG_WARN( "xGateTCPServer::xGateTCPServer(%s)", (char*)ur_log_string (m_serverType));
}

xGateRetVal xGateTCPServer::init (vector<xGateListenAddress>listenAddressList)
{
  XGLOG_WARN( "xGateTCPServer::init(%s)", (char*)ur_log_string (m_serverType));
  for( unsigned int i =0 ; i < listenAddressList.size();i++)
  {
    unsigned short port  = listenAddressList[i].port;
    string ipAddress     = listenAddressList[i].ipAddress;

    //Server Address
    ACE_INET_Addr addr(port,ipAddress.c_str());

    XGLOG_WARN( "xGateTCPServer::init(%s) Listening on Address: %s:%d", 
      (char*)ur_log_string (m_serverType), (char*)ur_log_string (ipAddress),port);

    errno=0;
    ACE_SOCK_Acceptor * acceptor = new ACE_SOCK_Acceptor(addr,1);
    if(errno != 0 )
    {
      XGLOG_WARN( "xGateTCPServer::init(%s) Error opening the socket Errno: %d"\
      " Error : %s", (char*)ur_log_string (m_serverType), errno, (char*)ur_log_string (strerror(errno)));
      return EN_XGATE_STATUS_ACE_ERROR;
    }
    m_acceptorList.push_back(acceptor);
    errno = 0;

    if(m_reactor->register_handler(acceptor->get_handle(), this, ACE_Event_Handler::READ_MASK)==-1)
    {
      XGLOG_WARN( "xGateTCPServer::init(%s) Error While registering socket"\
      " handler Errno:%d Error :%s",(char*)ur_log_string (m_serverType),errno,(char*)ur_log_string (strerror(errno)));
      return EN_XGATE_STATUS_ACE_ERROR;
    }
  }  

  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1)
  {
    XGLOG_WARN( "xGateTCPServer::init(%s)"\
        " ace error while spawning thread", (char*)ur_log_string (m_serverType));
    return EN_XGATE_STATUS_ACE_ERROR;
  }
}

void xGateTCPServer::run (void* threadClass)
{
  xGateTCPServer *pReceiver = static_cast<xGateTCPServer*>(threadClass);
  XGLOG_WARN( "xGateTCPServer::run Starting acceptor event processing loop (%s)", (char*)ur_log_string (pReceiver->m_serverType));

  pReceiver->m_running = true;
  pReceiver->m_reactor->owner(ACE_Thread::self());
  pReceiver->m_reactor->run_reactor_event_loop();

  XGLOG_WARN( "xGateTCPServer::run event loop is stopped");
  pReceiver->m_running = false;
}

xGateTCPServer::~xGateTCPServer (void)
{
  XGLOG_WARN( "xGateTCPServer::~xGateTCPServer(%s)", (char*)ur_log_string (m_serverType));

  for(unsigned int i =0;i<m_acceptorList.size();i++)
  {
     if (m_acceptorList[i])
         delete m_acceptorList[i];
  }
  if(m_reactor)
    delete m_reactor;

  m_running = false;
}

int xGateTCPServer::handle_input(ACE_HANDLE aceHandle)
{
  XGLOG_WARN( "xGateTCPServer::handle_input(%s)" \
      " Received Connection Request", (char*)ur_log_string (m_serverType));

  handle_connection(aceHandle);

  return 0;
}

int xGateTCPServer::handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
  XGLOG_WARN( "xGateTCPServer::handle_close(%s)", (char*)ur_log_string (m_serverType));
  return 0;
}

int xGateTCPServer::handle_connection(ACE_HANDLE aceHandle)
{
  XGLOG_WARN( "xGateTCPServer::handle_connection(%s)", (char*)ur_log_string (m_serverType));
  return 0;
}

xGateRetVal xGateTCPServer::stop_thread( void )
{
  XGLOG_WARN( "xGateTCPServer::stop_thread(%s) called", (char*)ur_log_string (m_serverType));
  m_reactor->end_reactor_event_loop();
  while (m_running)
  {
    sleep(1);
  }
  XGLOG_WARN( "xGateTCPServer::stop_thread(%s) Stopped xGateTCPServer", (char*)ur_log_string (m_serverType));
  return EN_XGATE_STATUS_SUCCESS;
}

int xGateTCPServer::handle_timeout(const ACE_Time_Value&, const void *arg)
{
  XGLOG_WARN( "xGateTCPServer::handle_timeout(%s) called",(char*)ur_log_string (m_serverType));
  return 0;
}

