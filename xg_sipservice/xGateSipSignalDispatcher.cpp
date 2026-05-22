#include "xglog.h" //DebugVM
#include "xGateSipSignalDispatcher.h"

#define THISMODULE "SDQ"
/*
*/
xGateSipSignalDispatcher::xGateSipSignalDispatcher():xGateDispatcher("SipDispatcher"){
XGLOG_INFO(THISMODULE, "multithreaded-ippbx : xGateSipSignalDispatcher::xGateSipSignalDispatcher(%s)", "SDQ Thread Started");
m_run = true;
}
/*
*/
xGateSipSignalDispatcher::~xGateSipSignalDispatcher(){

}

/*
*/
xGateRetVal xGateSipSignalDispatcher::process_msg(ACE_Message_Block*pAmb){
xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;
#if 0 //DebugVM start
    if(pAmb){

        //CLog::Info(THISMODULE,"multithreaded-ippbx : xGateSipSignalDispatcher::process_msg(%s)", " pAmb  Received Msg good");
        xGateSipMsgBlock *pSipMsg = dynamic_cast<xGateSipMsgBlock*>(pAmb);
        if(pSipMsg){
           // Validate is to respond for request
           if(pSipMsg->m_callObj->sipeventtype > 99 ){
            //CLog::Info(THISMODULE,"xGateSipSignalDispatcher::process_msg(%s) callid", pSipMsg->m_callObj->callid.c_str() );
            SendSipMessage(pSipMsg->m_callObj);
          }
        }else
          //CLog::Info(THISMODULE,"xGateSipSignalDispatcher::process_msg(%s) ","Error On casting"  ); // Remove after  testing
        
    }
    else{
      //CLog::Info(THISMODULE,"xGateSipSignalDispatcher::process_msg(%s)", "pAmb Received Msg Bad");
    }
#endif //DebugVM end
    return retval;
}
int xGateSipSignalDispatcher::svc(void){
  
  XGLOG_INFO(THISMODULE, "multithreaded-ippbx : xGateSipSignalDispatcher::svc(%s)", m_serverType);

   int rc;
   rc = pthread_setname_np(ACE_Thread::self(), "SipSignalDispatcher");
   if(rc!=0)
     printf("Failed to set name for SipSignalDispatcher thread");

  ACE_Message_Block *pAmb = 0;
  while(m_run)
  {
    ACE_Time_Value tv ((time(NULL) + 20));

    // Get message from the queue
    int result = getq(pAmb, &tv);
    if((result == -1) && (errno == EWOULDBLOCK))
    {
      continue;
    }

    if ((result != -1))
    {
      process_msg(pAmb);
    }
    else
    {
      // This is the case where the message queue is de-activated.
      // Need to stop the thread
      XGLOG_ERROR(THISMODULE, "xGateSipSignalDispatcher::svc(%s) Error in Message Queue", m_serverType);
      m_run = false;
    }

    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }
  XGLOG_INFO(THISMODULE, "xGateSipSignalDispatcher::svc(%s) Stopped Dispatch Task Thread", m_serverType);
  return 0;
}
/*
*/
#if 0 //DebugVM start
void xGateSipSignalDispatcher::SendSipMessage(xGateCallObj *callobj){
  // TODO : need process timing print in log
  // CLog::Info(THISMODULE,"xGateSipSignalDispatcher::SendSipMessage(%s)","Entering Function"); 
  // SofiaSiphandler sipstack;
  // switch((callobj->sipeventtype)/100){
  //   case 1: 
  //   sipstack.RespondWith1xx(callobj);
  //   break;
  //   case 2:
  //   sipstack.RespondWith2xx(callobj);
  //   break;
  //   case 3:
  //   sipstack.RespondWith3xx(callobj);
  //   break;
  //   case 4:
  //   sipstack.RespondWith4xx(callobj);
  //   break;
  //   case 5:
  //   sipstack.RespondWith5xx(callobj);
  //   break;
  //   case 6:
  //   sipstack.RespondWith6xx(callobj);
  //   break;
  //   default:
  //   break;

 // }

  //CLog::Info(THISMODULE,"xGateSipSignalDispatcher::SendSipMessage(%s)","Leaving Function");
}
#endif //DebugVM end
/*
*/
xGateRetVal xGateSipSignalDispatcher::stop(){
    m_run = false;
    msg_queue()->deactivate();
    return EN_XGATE_STATUS_SUCCESS;
}


