
//local includes
#include "log.h"
#include "klog.h"

//self includes
#include "xGateConnector.h"

#define THISMODULE "Contor"

/**
 * Default Constructor
 */
//TODO: time configuration must be dynamic
xGateConnector::xGateConnector(const char* serverType): 
  m_running(false),
  m_reactor(NULL),
  m_timerInterval(1),
  m_connectTimeOut(3)
{
  CLog::Warning(THISMODULE, "YOGA xGateConnector::xGateConnector(%s)", serverType);

  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);

  /// Instantiate the Reactor
  m_reactor = new ACE_Reactor();

  CLog::Warning(THISMODULE, "xGateConnector::xGateConnector(%s)"\
      " Timer Interval :%d Connection Time Out:%d", m_serverType, m_timerInterval, m_connectTimeOut);
}

/**
 * Initialize the xGateConnector
 */
xGateRetVal xGateConnector::init (void)
{

  CLog::Warning(THISMODULE, "xGateConnector::init(%s)", m_serverType);

  // Spawn the thread
  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1)
  {
    // Spawn Failure
    CLog::Warning(THISMODULE, "xGateConnector::init(%s)"\
        " ace error while spawning thread", m_serverType);
    // cleanup the timer manager singelton instance
    return EN_XGATE_STATUS_ACE_ERROR;
  } 
  return EN_XGATE_STATUS_SUCCESS;  
}//end init

/**
 *Start the event processing loop
 */
void xGateConnector::run (void* threadClass)
{
  CLog::Warning(THISMODULE, "xGateConnector::run Starting reactor event processing loop");

  int rc;
  rc = pthread_setname_np(ACE_Thread::self(), "XGConnctr");
  if(rc!=0)
    printf("Failed to set name for xgate connector thread");

  xGateConnector* pReceiver = static_cast<xGateConnector*>(threadClass);     

  pReceiver->m_running = true; 
  pReceiver->m_reactor->owner(ACE_Thread::self());
  pReceiver->m_reactor->run_reactor_event_loop(); 
  pReceiver->m_running = false;

  CLog::Warning(THISMODULE, "xGateConnector::run reactor loop is ended");
}//end run

/**
 *  Start a Timer
 **/
xGateRetVal xGateConnector::connectToPeer(xGateTCPConnectionStr &conStr) 
{
  CLog::Warning(THISMODULE, "Enter xGateConnector::connectToPeer"); 
  CLog::Warning(THISMODULE, "Exit  xGateConnector::connectToPeer");
  return EN_XGATE_STATUS_SUCCESS; 
}

/**
 * Destructor
 */
xGateConnector::~xGateConnector (void)
{
  CLog::Warning(THISMODULE, "xGateConnector::~xGateConnector(%s) destructor called", m_serverType);

  if(m_reactor)
    delete m_reactor; 
  m_running = false;
}//end destructor

/// Interface to stop the connector thread
xGateRetVal xGateConnector::stopThread( void )
{
  CLog::Warning(THISMODULE, "xGateConnector::stopThread(%s) called", m_serverType);
  // end the reactor event loop
  m_reactor->end_reactor_event_loop();
  // wait till the reactor event loop finishes
  while (m_running)
  {
    sleep(1);
  }
  CLog::Warning(THISMODULE, "xGateConnector::stopThread(%s) Stopped xGateConnector", m_serverType);
  return EN_XGATE_STATUS_SUCCESS;
} 

/// Method to populate the entry in the fdToSocketHandlerMap 
void xGateConnector::addFdToSocketHandlerMap(int fd, xGateTCPSocketHandler  *pHandler)
{
   CLog::Warning(THISMODULE, "Enter xGateConnector::addFdToSocketHandlerMap");
   m_fdToSocketHandlerMap[fd] = pHandler;
   CLog::Warning(THISMODULE, "Exit xGateConnector::addFdToSocketHandlerMap");
}

// Method to fetch SocketHandler From FD Map
xGateRetVal xGateConnector::getHandlerFromFdToSocketHandlerMap(int fd, xGateTCPSocketHandler *&pHandler)
{
   CLog::Warning(THISMODULE, "Enter xGateConnector::getHandlerFromFdToSocketHandlerMap");
   map<int, xGateTCPSocketHandler*>::iterator itr;
   xGateRetVal retVal = EN_XGATE_STATUS_SUCCESS;

   /// Find the entry for the key
   itr = m_fdToSocketHandlerMap.find(fd); 
   if(itr != m_fdToSocketHandlerMap.end())
   {
      pHandler = itr->second;
   } 
   else
   {
      CLog::Error(THISMODULE, "xGateConnector::getHandlerFromFdToSocketHandlerMap No entry found");
      retVal = EN_XGATE_STATUS_NOT_PRESENT; 
   } 
   CLog::Warning(THISMODULE, "Exit xGateConnector::getHandlerFromFdToSocketHandlerMap");
   return retVal;
}

/// Method to remove the entry from FD map
void xGateConnector::removeFromFdToSocketHandlerMap(int fd)
{
   CLog::Warning(THISMODULE, "Enter xGateConnector::removeFromFdToSocketHandlerMap");

   if(m_fdToSocketHandlerMap.find(fd) != m_fdToSocketHandlerMap.end())
   { 
     m_fdToSocketHandlerMap.erase(m_fdToSocketHandlerMap.find(fd));
     CLog::Warning(THISMODULE, "xGateConnector::removeFromFdToSocketHandlerMap erased the entry");
   } 
   CLog::Warning(THISMODULE, "Exit xGateConnector::removeFromFdToSocketHandlerMap"); 
}


