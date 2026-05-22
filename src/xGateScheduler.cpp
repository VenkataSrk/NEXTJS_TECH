//local includes
//#include "log.h"
//#include "klog.h"
#include "xglog.h"
#include "xGateUtil.h"
//self includes
#include "xGateScheduler.h"

#define THISMODULE "Scheduler"

xGateScheduler::xGateScheduler(): 
  m_running(false),
  m_reactor(NULL)
{
  /// Instantiate the Reactor
  m_reactor = new ACE_Reactor();
}

xGateScheduler::~xGateScheduler (void)
{
  if(m_reactor)
    delete m_reactor; 
  m_running = false;
}//end destructor

/**
 * Initialize the xGateScheduler
 */
bool xGateScheduler::init(void)
{
  XGLOG_INFO( "xGateScheduler initialization start !");
  // Spawn the thread
  if(ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1)
  {
    XGLOG_ERROR( "xGateScheduler init failed while spawning ace thread !");
    return false;
  } 
  return true;  
}//end init

void xGateScheduler::run (void* threadClass)
{
  XGLOG_INFO( "xGateScheduler starting reactor event loop !");
  xGateScheduler* pReceiver = static_cast<xGateScheduler*>(threadClass);     

  pReceiver->m_running = true; 
  pReceiver->m_reactor->owner(ACE_Thread::self());
  pReceiver->m_reactor->run_reactor_event_loop(); 
  pReceiver->m_running = false;

  XGLOG_INFO( "xGateScheduler reactor event loop stopped !");
}//end run

bool xGateScheduler::stopThread(void)
{
  XGLOG_INFO( "xGateScheduler stopping scheduler thread!");
  m_reactor->end_reactor_event_loop();
  while (m_running) {
    sleep(1);
  }
  XGLOG_INFO( "xGateScheduler stopped scheduler thread !");
  return true;
} 

int xGateScheduler::handle_timeout(const ACE_Time_Value&, const void *arg)
{
  XGLOG_INFO( "xGateScheduler timeout triggered in scheduler");
  return 0;
}
