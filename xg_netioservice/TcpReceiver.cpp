//local includes
#include "xglog.h"
#include "TcpReceiver.h"

TcpReceiver::TcpReceiver ():
  ACE_Event_Handler(),
  m_reactor(nullptr),
  m_running(false)
{
  m_reactor = new ACE_Reactor();
}//end default constructor

xGateRetVal TcpReceiver::init (void)
{
  XGLOG_DEBUG("Initializing TcpReceiver");
  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1)
  {
    XGLOG_ERROR("Initializing TcpReceiver failed."\
        "ace error while spawning thread !");
    return EN_XGATE_STATUS_ACE_ERROR;
  }
  return EN_XGATE_STATUS_SUCCESS;
}//end init

void TcpReceiver::run (void* threadClass)
{
  XGLOG_DEBUG("Starting acceptor event processing loop");

  TcpReceiver* pReceiver = static_cast<TcpReceiver*>(threadClass);
  pReceiver->m_running = true;
  pReceiver->m_reactor->owner(ACE_Thread::self());
  pReceiver->m_reactor->run_reactor_event_loop();
  pReceiver->m_running = false;
}//end run

TcpReceiver::~TcpReceiver (void)
{
  if(m_reactor)
    delete m_reactor;
  m_running = false;
}//end destructor

int TcpReceiver::handle_timeout(const ACE_Time_Value&, const void *arg)
{
  XGLOG_DEBUG("handle_timeout triggered");

  MediaTimerData *pTimerData = (MediaTimerData*)arg;
  if(!pTimerData) {
    XGLOG_ERROR("handle_timeout failed. Invalid TimerData !");
    return 0;
  }
  int fd = pTimerData->m_fd;
  delete pTimerData;

  TcpSocketHandler *pHandler = nullptr;
  if(EN_XGATE_STATUS_SUCCESS == getHandlerFromFdToSocketHandlerMap(fd,pHandler)) {
    if(pHandler) {
      XGLOG_DEBUG("handle_timeout triggered to close the handler: %d", fd);
      removeFromFdToSocketHandlerMap(fd);
      if(pHandler->getReactor()) {
        pHandler->getReactor()->remove_handler(fd,ACE_Event_Handler::READ_MASK);
      }
    }
  }
  return 0;
}

xGateRetVal TcpReceiver::stop_thread(void)
{
  XGLOG_DEBUG("stopping MediaTcpRecevier thread");

  m_reactor->end_reactor_event_loop();
  // wait till the reactor event loop finishes
  while(m_running) {
    sleep(1);
  }
  XGLOG_DEBUG("stopped TcpReceiver thread !");
  return EN_XGATE_STATUS_SUCCESS;
}

void TcpReceiver::addFdToSocketHandlerMap(int fd, TcpSocketHandler *pHandler)
{
   XGLOG_DEBUG("Added TcpSocketHandler in TcpSocketHandler map for fd: %d", fd);
   m_fdToSocketHandlerMap[fd] = pHandler;
}

xGateRetVal TcpReceiver::getHandlerFromFdToSocketHandlerMap(int fd, TcpSocketHandler *&pHandler)
{
  XGLOG_DEBUG("Get TcpSocketHandler from TcpSocketHandler map for fd: %d", fd);
  map<int, TcpSocketHandler*>::iterator itr;
  xGateRetVal retVal = EN_XGATE_STATUS_SUCCESS;

  /// Find the entry for the key
  itr = m_fdToSocketHandlerMap.find(fd);
  if(itr != m_fdToSocketHandlerMap.end()) {
    pHandler = itr->second;
  } else {
    XGLOG_ERROR("Get TcpSocketHandler from map failed. No entry found for fd: %d", fd);
    retVal = EN_XGATE_STATUS_NOT_PRESENT;
  }
  return retVal;
}

void TcpReceiver::removeFromFdToSocketHandlerMap(int fd)
{
  XGLOG_DEBUG("Removing TcpSocketHandler from Map for fd: %d", fd);
  if(m_fdToSocketHandlerMap.find(fd) != m_fdToSocketHandlerMap.end()) {
    m_fdToSocketHandlerMap.erase(m_fdToSocketHandlerMap.find(fd));
    XGLOG_DEBUG("Removed TcpSocketHandler from Map for fd: %d", fd);
    return;
  } else {
    XGLOG_ERROR("Removing TcpSocketHandler from Map for fd: %d failed !", fd);
  }
}

ACE_Reactor * TcpReceiver::get_reactor(void)
{
  return m_reactor;
}

