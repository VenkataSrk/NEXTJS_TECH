
//ace includes

//local includes
//DebugVM #include "log.h"
//DebugVM #include "klog.h"
#include "xGateProcessor.h"

#define THISMODULE "Processor"
#include "xGateDefine.h" //DebugVM

xGateProcessor::xGateProcessor(const char *serverType) :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false)
{
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);
  memset(m_encodeBuffer,0,ENCODE_BUF_SIZE);
  //CLog::Warning(THISMODULE, "xGateProcessor::xGateProcessor(%s)", m_serverType);
}

xGateProcessor::~xGateProcessor(void)
{
  //CLog::Warning(THISMODULE, "xGateProcessor::~xGateProcessor(%s)", m_serverType);
}

xGateRetVal xGateProcessor::stop(void)
{
  //CLog::Warning(THISMODULE, "xGateProcessor::stop(%s)", m_serverType);

  m_run = false;
  msg_queue()->deactivate();
  wait();

  //CLog::Warning(THISMODULE, "xGateProcessor::stop Stopped xGateProcessor(%s)", m_serverType);
  return EN_XGATE_STATUS_SUCCESS;
}
xGateRetVal xGateProcessor::init(int numOfThreads)
{
  //CLog::Warning(THISMODULE, "xGateProcessor::init(%s)", m_serverType);
  m_run = true;
  xGateRetVal retVal = EN_XGATE_STATUS_SUCCESS;

  // Start the Dispatcher Thread.
  if (activate(THR_NEW_LWP ,numOfThreads) == -1 )
  {
    //CLog::Error(THISMODULE, "xGateProcessor::startThread(%s)"\
        " ACE returned error while spawning", m_serverType);
    retVal = EN_XGATE_STATUS_ERROR;
  }
  else
  {
    //CLog::Warning(THISMODULE, "xGateProcessor::init(%s) Success", m_serverType);
  }
  return retVal;
}

int xGateProcessor::svc(void)
{
  //CLog::Warning(THISMODULE, "xGateProcessor::svc(%s)", m_serverType);
   int rc;
   rc = pthread_setname_np(ACE_Thread::self(), "SipProcessor");
   if(rc!=0)
     printf("Failed to set name for sip xGateProcessor thread");
  
  ACE_Message_Block *pAmb = 0;

  while(m_run)
  {
    ACE_Time_Value tv ((time(NULL) + 20));
    int result = getq(pAmb, &tv);
    if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
    {
      continue;
    }
    if ((result != -1))
    {
      handle_msg(pAmb);
    }
    else
    {
      //CLog::Error(THISMODULE, "xGateProcessor::svc(%s) Error in Message Queue", m_serverType);
      m_run = false;
    }
    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }

  //CLog::Warning(THISMODULE, "xGateProcessor::svc(%s) Stopped Dispatch Task Thread", m_serverType);
  return 0;
}

xGateRetVal xGateProcessor::handle_msg(ACE_Message_Block *pAmb)
{
  //CLog::Warning(THISMODULE, "Enter xGateProcessor::handle_msg(%s)", m_serverType);

  xGateBaseMsg *pMsg = 0;
  pMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
#if 0 //DebugVM start
  // On the server processor, we handle  
  switch(pMsg->getMsgType())
  {
    case EN_XGATE_MSG_MGC_MG_IN:
    case EN_XGATE_MSG_MGC_MG_OUT:
#ifdef XGATE_HAS_DBM_SUPPORT
    case EN_XGATE_MSG_TO_DBM:
    case EN_XGATE_MSG_FROM_DBM:
#endif
#ifdef XGATE_HAS_BUSINESS_SUPPORT 
    case EN_XGATE_MSG_TO_BUSINESS:
    case EN_XGATE_MSG_FROM_BUSINESS:
#endif
      {
        process_msg(pMsg);
        break;
      }
    default:
      {
        //CLog::Error(THISMODULE, "xGateProcessor::handle_msg"\
            " Invalid Message Type:%d",pMsg->getMsgType());
        break;
      }
  }
  //CLog::Warning(THISMODULE, "Exit: xGateProcessor::handle_msg");
#endif //DebugVM end
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateProcessor::process_msg(xGateBaseMsg *pMsg)
{
  //CLog::Warning(THISMODULE, "Enter xGateProcessor::process_msg(%s)", m_serverType); 
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateProcessor::dispatch_msg(xGateBaseMsg *pMsg)
{
  //CLog::Warning(THISMODULE, "Enter xGateProcessor::dispatch_msg(%s)", m_serverType); 
  return EN_XGATE_STATUS_SUCCESS;
}
