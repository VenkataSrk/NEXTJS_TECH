//local includes
#include "xglog.h"
#include "../include/xGateTCPReceiver.h"
#include "xGateUtil.h"


#define THISMODULE "TcpRecv"

xGateTCPReceiver::xGateTCPReceiver (const char*serverType, ACE_Task<ACE_MT_SYNCH>* taskPtr):
  ACE_Event_Handler(),
  m_reactor(NULL),
  m_running(false),
  m_taskPtr(taskPtr)
{
  XGLOG_WARN( "xGateTCPReceiver::xGateTCPReceiver(%s)", (char*)ur_log_string (serverType));
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);

  m_reactor = new ACE_Reactor();
}//end default constructor

xGateRetVal xGateTCPReceiver::init (void)
{
  XGLOG_WARN( "xGateTCPReceiver::init(%s)", (char*)ur_log_string (m_serverType));
  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1)
  {
    XGLOG_ERROR( "xGateTCPReceiver::init(%s)"\
        "ace error while spawning thread", (char*)ur_log_string (m_serverType));
    return EN_XGATE_STATUS_ACE_ERROR;
  }
  return EN_XGATE_STATUS_SUCCESS;
}//end init

void xGateTCPReceiver::run (void* threadClass)
{
  XGLOG_WARN( "xGateTCPReceiver::run Starting acceptor event processing loop");

  xGateTCPReceiver* pReceiver = static_cast<xGateTCPReceiver*>(threadClass);

  pReceiver->m_running = true;
  pReceiver->m_reactor->owner(ACE_Thread::self());
  pReceiver->m_reactor->run_reactor_event_loop();
  pReceiver->m_running = false;
}//end run

xGateTCPReceiver::~xGateTCPReceiver (void)
{
  XGLOG_WARN( "xGateTCPReceiver::~xGateTCPReceiver(%s) destructor called", (char*)ur_log_string (m_serverType));

  if(m_reactor)
    delete m_reactor;

  m_running = false;
}//end destructor

int xGateTCPReceiver::handle_timeout(const ACE_Time_Value&, const void *arg)
{
  XGLOG_WARN( "Enter: xGateTCPReceiver::handle_timeout");

  xGateTimerData *pTimerData = (xGateTimerData*)arg;
  if (!pTimerData)
  {
    XGLOG_ERROR( "xGateTCPReceiver::handle_timeout Fd not present");
    return 0;
  }

  int fd = pTimerData->fd;
  delete pTimerData;
  xGateTCPSocketHandler *pHandler = NULL;
  if (EN_XGATE_STATUS_SUCCESS == getHandlerFromFdToSocketHandlerMap(fd,pHandler))
  {
    if (pHandler)
    {
      XGLOG_WARN( "xGateTCPReceiver::handle_timeout Fd obtained to close the handler");
      removeFromFdToSocketHandlerMap(fd);
      if (pHandler->getReactor())
      {
        pHandler->getReactor()->remove_handler(fd,ACE_Event_Handler::READ_MASK);
      }
    }
  }

  XGLOG_WARN( "Exit: xGateTCPReceiver::handle_timeout");
  return 0;
}

xGateRetVal xGateTCPReceiver::stop_thread(void)
{
  XGLOG_WARN( "xGateTCPReceiver::stopThread(%s) called", (char*)ur_log_string (m_serverType));

  m_reactor->end_reactor_event_loop();
  // wait till the reactor event loop finishes
  while (m_running)
  {
    sleep(1);
  }
  XGLOG_WARN( "xGateTCPReceiver::stopThread(%s) Stopped xGateTCPReceiver", (char*)ur_log_string (m_serverType));
  return EN_XGATE_STATUS_SUCCESS;
}

void xGateTCPReceiver::addFdToSocketHandlerMap(int fd, xGateTCPSocketHandler *pHandler)
{
   XGLOG_WARN( "Enter xGateTCPReceiver::addFdToSocketHandlerMap ");

   m_fdToSocketHandlerMap[fd] = pHandler;
   XGLOG_WARN( "Exit  xGateTCPReceiver::addFdToSocketHandlerMap");

}

xGateRetVal xGateTCPReceiver::getHandlerFromFdToSocketHandlerMap(int fd, xGateTCPSocketHandler *&pHandler)
{
  XGLOG_WARN( "Enter xGateTCPReceiver::getHandlerFromFdToSocketHandlerMap ");
  map<int,xGateTCPSocketHandler*>::iterator itr;
  xGateRetVal retVal = EN_XGATE_STATUS_SUCCESS;

  /// Find the entry for the key
  itr=m_fdToSocketHandlerMap.find(fd);
  if(itr != m_fdToSocketHandlerMap.end())
  {
    pHandler = itr->second;
  }
  else
  {
    XGLOG_ERROR( "xGateTCPReceiver::getHandlerFromFdToSocketHandlerMap No entry found");
    retVal = EN_XGATE_STATUS_NOT_PRESENT;
  }
  XGLOG_WARN( "Exit xGateTCPReceiver::getHandlerFromFdToSocketHandlerMap");
  return retVal;
}

void xGateTCPReceiver::removeFromFdToSocketHandlerMap(int fd)
{
  XGLOG_WARN( "Enter xGateTCPReceiver::removeFromFdToSocketHandlerMap");

  if(m_fdToSocketHandlerMap.find(fd) != m_fdToSocketHandlerMap.end())
  {
    m_fdToSocketHandlerMap.erase(m_fdToSocketHandlerMap.find(fd));
    XGLOG_WARN( "xGateTCPReceiver::removeFromFdToSocketHandlerMap erased the entry");
  }
  XGLOG_WARN( "Exit xGateTCPReceiver::removeFromFdToSocketHandlerMap");
}

