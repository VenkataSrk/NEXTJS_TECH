
//local include

//self include
#include "xGateConnector.h"
#include "xglog.h"
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
  XGLOG_INFO( "xGateConnector::xGateConnector(%s)", serverType);
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);

  /// Instantiate the Reactor
  m_reactor = new ACE_Reactor();

  XGLOG_INFO( "xGateConnector::xGateConnector(%s)"\
      " Timer Interval :%d Connection Time Out:%d", m_serverType, m_timerInterval, m_connectTimeOut);
}

/**
 * Initialize the xGateConnector
 */
/*xGateRetVal xGateConnector::init (void)
{

  XGLOG_INFO( "xGateConnector::init(%s)", m_serverType);

  // Spawn the thread
  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1)
  {
    // Spawn Failure
    XGLOG_INFO( "xGateConnector::init(%s)"\
        " ace error while spawning thread", m_serverType);
    // cleanup the timer manager singelton instance
    return EN_XGATE_STATUS_ACE_ERROR;
  } 
  return EN_XGATE_STATUS_SUCCESS;  
}//end init*/

xGateRetVal xGateConnector::init()
{
  XGLOG_INFO( "xGateConnector::init(%s)", m_serverType);
  
  // Spawn the thread
  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,(void*) this,THR_NEW_LWP|THR_DETACHED) == -1)
  {
    // Spawn Failure
    XGLOG_INFO( "xGateConnector::init(%s)ace error while spawning thread", m_serverType);
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
  XGLOG_INFO( "xGateConnector::run Starting reactor event processing loop");

  xGateConnector* pReceiver = static_cast<xGateConnector*>(threadClass);     

  pReceiver->m_running = true; 
  pReceiver->m_reactor->owner(ACE_Thread::self());
  pReceiver->m_reactor->run_reactor_event_loop(); 
  pReceiver->m_running = false;

  XGLOG_INFO( "xGateConnector::run reactor loop is ended");
}//end run

/**
 *  Start a Timer
 **/
xGateRetVal xGateConnector::connectToPeer(xGateTCPConnectionStr &conStr) 
{
  XGLOG_INFO( "Enter xGateConnector::connectToPeer"); 
  XGLOG_INFO( "Exit  xGateConnector::connectToPeer");
  return EN_XGATE_STATUS_SUCCESS; 
}

/**
 * Destructor
 */
xGateConnector::~xGateConnector (void)
{
  XGLOG_INFO( "xGateConnector::~xGateConnector(%s) destructor called", m_serverType);

  if(m_reactor)
    delete m_reactor; 
  m_running = false;
}//end destructor

/// Interface to stop the connector thread
xGateRetVal xGateConnector::stopThread( void )
{
  XGLOG_INFO( "xGateConnector::stopThread(%s) called", m_serverType);
  // end the reactor event loop
  m_reactor->end_reactor_event_loop();
  // wait till the reactor event loop finishes
  while (m_running)
  {
    sleep(1);
  }
  XGLOG_INFO( "xGateConnector::stopThread(%s) Stopped xGateConnector", m_serverType);
  return EN_XGATE_STATUS_SUCCESS;
} 

/// Method to populate the entry in the fdToSocketHandlerMap 
void xGateConnector::addFdToSocketHandlerMap(int fd, MediaTcpSocketHandler  *pHandler)
{
   XGLOG_INFO( "Enter xGateConnector::addFdToSocketHandlerMap");
   m_fdToSocketHandlerMap[fd] = pHandler;
   XGLOG_INFO( "Exit xGateConnector::addFdToSocketHandlerMap");
}

// Method to fetch SocketHandler From FD Map
xGateRetVal xGateConnector::getHandlerFromFdToSocketHandlerMap(int fd, MediaTcpSocketHandler *&pHandler)
{
   XGLOG_INFO( "Enter xGateConnector::getHandlerFromFdToSocketHandlerMap");
   map<int, MediaTcpSocketHandler*>::iterator itr;
   xGateRetVal retVal = EN_XGATE_STATUS_SUCCESS;

   /// Find the entry for the key
   itr = m_fdToSocketHandlerMap.find(fd); 
   if(itr != m_fdToSocketHandlerMap.end())
   {
      pHandler = itr->second;
   } 
   else
   {
      XGLOG_ERROR( "xGateConnector::getHandlerFromFdToSocketHandlerMap No entry found");
      retVal = EN_XGATE_STATUS_NOT_PRESENT; 
   } 
   XGLOG_INFO( "Exit xGateConnector::getHandlerFromFdToSocketHandlerMap");
   return retVal;
}

/// Method to remove the entry from FD map
void xGateConnector::removeFromFdToSocketHandlerMap(int fd)
{
   XGLOG_INFO( "Enter xGateConnector::removeFromFdToSocketHandlerMap");

   if(m_fdToSocketHandlerMap.find(fd) != m_fdToSocketHandlerMap.end())
   { 
     m_fdToSocketHandlerMap.erase(m_fdToSocketHandlerMap.find(fd));
     XGLOG_INFO( "xGateConnector::removeFromFdToSocketHandlerMap erased the entry");
   } 
   XGLOG_INFO( "Exit xGateConnector::removeFromFdToSocketHandlerMap"); 
}


