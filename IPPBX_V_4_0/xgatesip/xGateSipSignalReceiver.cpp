#include "xglog.h"
#include "xGateSipSignalReceiver.h"
//#include "SofiaSiphandler.h"
#define MODULE "SIPRECEIVER"
xGateSipSignalReceiver::xGateSipSignalReceiver(const char*serverType, ACE_Task<ACE_MT_SYNCH>* taskPtr):
ACE_Event_Handler(),m_running(false)
  {
     // CLOG::Warning(MODULE,"xGateSipSignalReceiver::xGateSipSignalReceiver(%s)",serverType);
      memset(m_serverType,'/0', SERVER_TYPE_SIZE);
      strcpy(m_serverType,serverType);
      pSipController =  new SofiaSiphandler();
      
  }

  int xGateSipSignalReceiver:: init(void){
      XGLOG_INFO("multithreaded-ippbx : xGateSipSignalReceiver:: init(%s)",m_serverType);
      if(ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
        (void*) this,
        THR_NEW_LWP|THR_DETACHED) == -1){
        /////THR_NEW_LWP|THR_DETACHED|THR_DAEMON|THR_BOUND) == -1){ // Spawning here a new daemon-thread -which is an LWP and OS-schedulable.
       // XGLOG_ERROR( "xGateSipSignalReceiver::init(%s)"\
        "ace error while spawning thread", m_serverType);
        return -1;
        }
        return 0;


  }
void  xGateSipSignalReceiver::run(void * thread){

//HA_CommandHandler hp_handler("HighPriority");
////hp_handler.activate(THR_NEW_LWP| THR_DETACHED|THR_DAEMON|THR_BOUND, 1, 1, HI_PRIORITY); // Setting this thread to HI_PRIORITY.
//hp_handler.activate(THR_NEW_LWP| THR_DETACHED, 1, 1, HI_PRIORITY); // Setting this thread to HI_PRIORITY.

        xGateSipSignalReceiver *pSipReceiver = static_cast<xGateSipSignalReceiver*>(thread);
        pSipReceiver->m_running = true;
        if(pSipReceiver->pSipController->SipLibInit())
          if(pSipReceiver->pSipController->SipLibLoad()){
            while(pSipReceiver->m_running){
              pSipReceiver->pSipController->RunEventLoop();
	    // XGLOG_INFO("Inside While Loop"); // just for testing the code
              //usleep(500000); // 500 millisecond loop to get event for each call
            }
            pSipReceiver->pSipController->SipLibUnLoad();
          }
	return 0;
      

  }

  xGateSipSignalReceiver::~xGateSipSignalReceiver(){
   //   XGLOG_WARN("xGateSipSignalReceiver::~xGateSipSignalReceiver(%s) destructor called", m_serverType);
      
        m_running = false;
  }
int  xGateSipSignalReceiver::stop_thread(void){
  //  m_reactor->end_reactor_event_loop();
    m_running = false;
	return 0;
   // XGLOG_WARN("xGateSipSignalReceiver::stopThread(%s) Stopped xGateSipSignalReceiver", m_serverType);
}
