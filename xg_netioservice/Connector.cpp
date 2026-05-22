
//local include

//self include
#include "Connector.h"
#include "xglog.h"
#define THISMODULE "Contor"

/**
 * Default Constructor
 */
//TODO: time configuration must be dynamic
Connector::Connector(const char* serverType): 
  m_running(false),
  m_reactor(nullptr),
  m_timerInterval(1),
  m_connectTimeOut(3)
{
  XGLOG_INFO( "Connector::Connector(%s)", serverType);
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);

  /// Instantiate the Reactor
  m_reactor = new ACE_Reactor();

  XGLOG_INFO( "Connector::Connector(%s)"\
      " Timer Interval :%d Connection Time Out:%d", m_serverType, m_timerInterval, m_connectTimeOut);
}

/**
 * Initialize the Connector
 */
/*xGateRetVal Connector::init (void)
{

  XGLOG_INFO( "Connector::init(%s)", m_serverType);

  // Spawn the thread
  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1)
  {
    // Spawn Failure
    XGLOG_INFO( "Connector::init(%s)"\
        " ace error while spawning thread", m_serverType);
    // cleanup the timer manager singelton instance
    return EN_XGATE_STATUS_ACE_ERROR;
  } 
  return EN_XGATE_STATUS_SUCCESS;  
}//end init*/

xGateRetVal Connector::init()
{
  XGLOG_INFO( "Connector::init(%s)", m_serverType);
  
  // Spawn the thread
  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,(void*) this,THR_NEW_LWP|THR_DETACHED) == -1)
  {
    // Spawn Failure
    XGLOG_INFO( "Connector::init(%s)ace error while spawning thread", m_serverType);
    // cleanup the timer manager singelton instance
    return EN_XGATE_STATUS_ACE_ERROR;
  }
  return EN_XGATE_STATUS_SUCCESS;
}//end init



/**
 *Start the event processing loop
 */
void Connector::run (void* threadClass)
{
  XGLOG_INFO( "Connector::run Starting reactor event processing loop");

  Connector* pReceiver = static_cast<Connector*>(threadClass);     

  pReceiver->m_running = true; 
  pReceiver->m_reactor->owner(ACE_Thread::self());
  pReceiver->m_reactor->run_reactor_event_loop(); 
  pReceiver->m_running = false;

  XGLOG_INFO( "Connector::run reactor loop is ended");
}//end run

/**
 *  Start a Timer
 **/
 #if 0
xGateRetVal Connector::connectToPeer(xGateTCPConnectionStr &conStr) 
{
  XGLOG_INFO( "Enter Connector::connectToPeer"); 
  XGLOG_INFO( "Exit  Connector::connectToPeer");
  return EN_XGATE_STATUS_SUCCESS; 
}
#endif

/**
 * Destructor
 */
Connector::~Connector (void)
{
  XGLOG_INFO( "Connector::~xGateConnector(%s) destructor called", m_serverType);

  if(m_reactor)
    delete m_reactor; 
  m_running = false;
}//end destructor

/// Interface to stop the connector thread
xGateRetVal Connector::stopThread( void )
{
  XGLOG_INFO( "Connector::stopThread(%s) called", m_serverType);
  // end the reactor event loop
  m_reactor->end_reactor_event_loop();
  // wait till the reactor event loop finishes
  while (m_running)
  {
    sleep(1);
  }
  XGLOG_INFO( "Connector::stopThread(%s) Stopped Connector", m_serverType);
  return EN_XGATE_STATUS_SUCCESS;
} 

/// Method to populate the entry in the fdToSocketHandlerMap 
void Connector::addFdToSocketHandlerMap(int fd, TcpSocketHandler  *pHandler)
{
   XGLOG_INFO( "Enter Connector::addFdToSocketHandlerMap");
   m_fdToSocketHandlerMap[fd] = pHandler;
   XGLOG_INFO( "Exit Connector::addFdToSocketHandlerMap");
}

// Method to fetch SocketHandler From FD Map
xGateRetVal Connector::getHandlerFromFdToSocketHandlerMap(int fd, TcpSocketHandler *&pHandler)
{
   XGLOG_INFO( "Enter Connector::getHandlerFromFdToSocketHandlerMap");
   map<int, TcpSocketHandler*>::iterator itr;
   xGateRetVal retVal = EN_XGATE_STATUS_SUCCESS;

   /// Find the entry for the key
   itr = m_fdToSocketHandlerMap.find(fd); 
   if(itr != m_fdToSocketHandlerMap.end())
   {
      pHandler = itr->second;
   } 
   else
   {
      XGLOG_ERROR( "Connector::getHandlerFromFdToSocketHandlerMap No entry found");
      retVal = EN_XGATE_STATUS_NOT_PRESENT; 
   } 
   XGLOG_INFO( "Exit Connector::getHandlerFromFdToSocketHandlerMap");
   return retVal;
}

/// Method to remove the entry from FD map
void Connector::removeFromFdToSocketHandlerMap(int fd)
{
   XGLOG_INFO( "Enter Connector::removeFromFdToSocketHandlerMap");

   if(m_fdToSocketHandlerMap.find(fd) != m_fdToSocketHandlerMap.end())
   { 
     m_fdToSocketHandlerMap.erase(m_fdToSocketHandlerMap.find(fd));
     XGLOG_INFO( "Connector::removeFromFdToSocketHandlerMap erased the entry");
   } 
   XGLOG_INFO( "Exit Connector::removeFromFdToSocketHandlerMap"); 
}


