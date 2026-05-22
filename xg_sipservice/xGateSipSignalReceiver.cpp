#include "xglog.h" //DebugVM
#include "xGateSipSignalReceiver.h"
//#include "SofiaSiphandler.h"
#define MODULE "SIPRECEIVER"
xGateSipSignalReceiver::xGateSipSignalReceiver(const char*serverType, ACE_Task<ACE_MT_SYNCH>* taskPtr):
ACE_Event_Handler(),m_running(false)
  {
     // CLOG::Warning(MODULE,"xGateSipSignalReceiver::xGateSipSignalReceiver(%s)",serverType);
      memset(m_serverType,0, SERVER_TYPE_SIZE);
      strcpy(m_serverType,serverType);
     
      
  }

  int xGateSipSignalReceiver:: init(const char*serverType){
	  
	memset(m_serverType,0, SERVER_TYPE_SIZE);
	strcpy(m_serverType,serverType);
	pSipController =  new SofiaSiphandler();
	//CLog::Detail(MODULE,"multithreaded-ippbx : xGateSipSignalReceiver:: init(%s)",m_serverType);
	XGLOG_INFO(MODULE,"xGateSipSignalReceiver:: init(%s)",m_serverType);
	if(ACE_Thread_Manager::instance()->spawn((ACE_THR_FUNC) run,
	(void*) this,
	THR_NEW_LWP|THR_DETACHED) == -1){
		/////THR_NEW_LWP|THR_DETACHED|THR_DAEMON|THR_BOUND) == -1){ // Spawning here a new daemon-thread -which is an LWP and OS-schedulable.
		// CLog::Error(MODULE, "xGateSipSignalReceiver::init(%s)"\
		"ace error while spawning thread", m_serverType);
		return -1;
	}
	XGLOG_INFO(MODULE,"xGateSipSignalReceiver:: Initialization completed for (%s)",m_serverType);
	return 0;
  }
void  xGateSipSignalReceiver::run(void * thread){

//HA_CommandHandler hp_handler("HighPriority");
////hp_handler.activate(THR_NEW_LWP| THR_DETACHED|THR_DAEMON|THR_BOUND, 1, 1, HI_PRIORITY); // Setting this thread to HI_PRIORITY.
//hp_handler.activate(THR_NEW_LWP| THR_DETACHED, 1, 1, HI_PRIORITY); // Setting this thread to HI_PRIORITY.
      //xGateSipSignalReceiver *pSipReceiver = static_cast<xGateSipSignalReceiver*>(thread);

      XGLOG_INFO(MODULE,"xGateSipSignalReceiver::run");
        SIP_SIGNAL_RECEIVER::instance()->m_running = true;
        if(SIP_SIGNAL_RECEIVER::instance()->pSipController->SipLibInit())
          if(SIP_SIGNAL_RECEIVER::instance()->pSipController->SipLibLoad()){
            while(SIP_SIGNAL_RECEIVER::instance()->m_running){
              SIP_SIGNAL_RECEIVER::instance()->pSipController->RunEventLoop();
            }
            SIP_SIGNAL_RECEIVER::instance()->pSipController->SipLibUnLoad();
          }
	//DebugVM return 0;
	return ;
      

  }

  xGateSipSignalReceiver::~xGateSipSignalReceiver(){
   //   CLog::Warning(MODULE, "xGateSipSignalReceiver::~xGateSipSignalReceiver(%s) destructor called", m_serverType);
      
        m_running = false;
  }
int  xGateSipSignalReceiver::stop_thread(void){
  //  m_reactor->end_reactor_event_loop();
    m_running = false;
	return 0;
   // CLog::Warning(MODULE, "xGateSipSignalReceiver::stopThread(%s) Stopped xGateSipSignalReceiver", m_serverType);
}
