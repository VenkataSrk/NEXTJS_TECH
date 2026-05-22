//local includes
#include "log.h"
#include "klog.h"
#include "xGateTCPReceiver.h"

#define THISMODULE "TcpRecv"

xGateTCPReceiver::xGateTCPReceiver (const char*serverType, ACE_Task<ACE_MT_SYNCH>* taskPtr):
  ACE_Event_Handler(),
  m_reactor(NULL),
  m_running(false),
  m_taskPtr(taskPtr)
{
  CLog::Warning(THISMODULE, "xGateTCPReceiver::xGateTCPReceiver(%s)", serverType);
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);

  m_reactor = new ACE_Reactor();
}//end default constructor

xGateRetVal xGateTCPReceiver::init (void)
{
  CLog::Warning(THISMODULE, "xGateTCPReceiver::init(%s)", m_serverType);
  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1)
  {
    CLog::Error(THISMODULE, "xGateTCPReceiver::init(%s)"\
        "ace error while spawning thread", m_serverType);
    return EN_XGATE_STATUS_ACE_ERROR;
  }
  return EN_XGATE_STATUS_SUCCESS;
}//end init

void xGateTCPReceiver::run (void* threadClass)
{
  CLog::Warning(THISMODULE, "xGateTCPReceiver::run Starting acceptor event processing loop");

  xGateTCPReceiver* pReceiver = static_cast<xGateTCPReceiver*>(threadClass);

  pReceiver->m_running = true;
  pReceiver->m_reactor->owner(ACE_Thread::self());
  pReceiver->m_reactor->run_reactor_event_loop();
  pReceiver->m_running = false;
}//end run

xGateTCPReceiver::~xGateTCPReceiver (void)
{
  CLog::Warning(THISMODULE, "xGateTCPReceiver::~xGateTCPReceiver(%s) destructor called", m_serverType);

  if(m_reactor)
    delete m_reactor;

  m_running = false;
}//end destructor

int xGateTCPReceiver::handle_timeout(const ACE_Time_Value&, const void *arg)
{
  CLog::Warning(THISMODULE, "Enter: xGateTCPReceiver::handle_timeout");

  xGateTimerData *pTimerData = (xGateTimerData*)arg;
  if (!pTimerData)
  {
    CLog::Error(THISMODULE, "xGateTCPReceiver::handle_timeout Fd not present");
    return 0;
  }

  int fd = pTimerData->fd;
  delete pTimerData;
  xGateTCPSocketHandler *pHandler = NULL;
  if (EN_XGATE_STATUS_SUCCESS == getHandlerFromFdToSocketHandlerMap(fd,pHandler))
  {
    if (pHandler)
    {
      CLog::Warning(THISMODULE, "xGateTCPReceiver::handle_timeout Fd obtained to close the handler");
      removeFromFdToSocketHandlerMap(fd);
      if (pHandler->getReactor())
      {
        pHandler->getReactor()->remove_handler(fd,ACE_Event_Handler::READ_MASK);
      }
    }
  }

  CLog::Warning(THISMODULE, "Exit: xGateTCPReceiver::handle_timeout");
  return 0;
}

xGateRetVal xGateTCPReceiver::stop_thread(void)
{
  CLog::Warning(THISMODULE, "xGateTCPReceiver::stopThread(%s) called", m_serverType);

  m_reactor->end_reactor_event_loop();
  // wait till the reactor event loop finishes
  while (m_running)
  {
    sleep(1);
  }
  CLog::Warning(THISMODULE, "xGateTCPReceiver::stopThread(%s) Stopped xGateTCPReceiver", m_serverType);
  return EN_XGATE_STATUS_SUCCESS;
}

void xGateTCPReceiver::addFdToSocketHandlerMap(int fd, xGateTCPSocketHandler *pHandler)
{
   CLog::Warning(THISMODULE, "Enter xGateTCPReceiver::addFdToSocketHandlerMap ");

   m_fdToSocketHandlerMap[fd] = pHandler;
   CLog::Warning(THISMODULE, "Exit  xGateTCPReceiver::addFdToSocketHandlerMap");

}

xGateRetVal xGateTCPReceiver::getHandlerFromFdToSocketHandlerMap(int fd, xGateTCPSocketHandler *&pHandler)
{
  CLog::Warning(THISMODULE, "Enter xGateTCPReceiver::getHandlerFromFdToSocketHandlerMap ");
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
    CLog::Error(THISMODULE, "xGateTCPReceiver::getHandlerFromFdToSocketHandlerMap No entry found");
    retVal = EN_XGATE_STATUS_NOT_PRESENT;
  }
  CLog::Warning(THISMODULE, "Exit xGateTCPReceiver::getHandlerFromFdToSocketHandlerMap");
  return retVal;
}

void xGateTCPReceiver::removeFromFdToSocketHandlerMap(int fd)
{
  CLog::Warning(THISMODULE, "Enter xGateTCPReceiver::removeFromFdToSocketHandlerMap");

  if(m_fdToSocketHandlerMap.find(fd) != m_fdToSocketHandlerMap.end())
  {
    m_fdToSocketHandlerMap.erase(m_fdToSocketHandlerMap.find(fd));
    CLog::Warning(THISMODULE, "xGateTCPReceiver::removeFromFdToSocketHandlerMap erased the entry");
  }
  CLog::Warning(THISMODULE, "Exit xGateTCPReceiver::removeFromFdToSocketHandlerMap");
}

