//local includes
#include "xglog.h"

//self includes
#include "xGateScheduler.h"

#define THISMODULE "Scheduler"

xGateScheduler::xGateScheduler(const char* schedulerType): 
  m_running(false),
  m_reactor(NULL),
  m_schedulerType(schedulerType)
{
  XGLOG_INFO("xGateScheduler::xGateScheduler(%s)", m_schedulerType.c_str());

  m_schedulerType = schedulerType; 

  /// Instantiate the Reactor
  m_reactor = new ACE_Reactor();
}

xGateScheduler::~xGateScheduler (void)
{
 XGLOG_WARN( "xGateScheduler::~xGateScheduler(%s) destructor called", m_schedulerType.c_str());

  if(m_reactor)
    delete m_reactor; 
  m_running = false;
}//end destructor

/**
 * Initialize the xGateScheduler
 */
xGateRetVal xGateScheduler::init (void)
{

 XGLOG_WARN( "xGateScheduler::init(%s)", m_schedulerType.c_str());

  // Spawn the thread
  if (ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1)
  {
    // Spawn Failure
   XGLOG_WARN( "xGateScheduler::init(%s)"\
        " ace error while spawning thread", m_schedulerType.c_str());
    // cleanup the timer manager singelton instance
    return EN_XGATE_STATUS_ACE_ERROR;
  } 
  return EN_XGATE_STATUS_SUCCESS;  
}//end init

void xGateScheduler::run (void* threadClass)
{
 XGLOG_WARN( "xGateScheduler::run Starting reactor event processing loop");

  xGateScheduler* pReceiver = static_cast<xGateScheduler*>(threadClass);     

  pReceiver->m_running = true; 
  pReceiver->m_reactor->owner(ACE_Thread::self());
  pReceiver->m_reactor->run_reactor_event_loop(); 
  pReceiver->m_running = false;

 XGLOG_WARN( "xGateScheduler::run reactor loop is ended");
}//end run

/// Interface to stop the connector thread
xGateRetVal xGateScheduler::stopThread( void )
{
 XGLOG_WARN( "xGateScheduler::stopThread(%s) called", m_schedulerType.c_str());
  // end the reactor event loop
  m_reactor->end_reactor_event_loop();
  // wait till the reactor event loop finishes
  while (m_running)
  {
    sleep(1);
  }
 XGLOG_WARN( "xGateScheduler::stopThread(%s) Stopped xGateScheduler", m_schedulerType.c_str());
  return EN_XGATE_STATUS_SUCCESS;
} 

int xGateScheduler::handle_timeout(const ACE_Time_Value&, const void *arg)
{
 XGLOG_WARN( "xGateScheduler::handle_timeout");
  xGateTimerData *pTimerData = (xGateTimerData*)arg;
  if (!pTimerData)
  {
    XGLOG_ERROR( "xGateScheduler::handle_timeout "\
        "Timerdata not present");
    return 0;
  }

  delete pTimerData;
  return 0;
}
