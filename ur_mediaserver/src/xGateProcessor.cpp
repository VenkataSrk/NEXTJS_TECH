
//ace includes

//local includes
#include "xGateProcessor.h"
#include "xglog.h"
#define THISMODULE "Processor"

xGateProcessor::xGateProcessor(const char *serverType) :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false)
{
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);
  memset(m_encodeBuffer,0,ENCODE_BUF_SIZE);
  XGLOG_WARN( "xGateProcessor::xGateProcessor(%s)", m_serverType);
}

xGateProcessor::~xGateProcessor(void)
{
  XGLOG_WARN( "xGateProcessor::~xGateProcessor(%s)", m_serverType);
}

xGateRetVal xGateProcessor::stop(void)
{
  XGLOG_WARN( "xGateProcessor::stop(%s)", m_serverType);

  m_run = false;
  msg_queue()->deactivate();
  wait();

  XGLOG_WARN( "xGateProcessor::stop Stopped xGateProcessor(%s)", m_serverType);
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateProcessor::init(int numOfThreads)
{
  XGLOG_WARN( "xGateProcessor::init(%s)", m_serverType);
  m_run = true;
  xGateRetVal retVal = EN_XGATE_STATUS_SUCCESS;

  // Start the Dispatcher Thread.
  if (activate(THR_NEW_LWP |THR_BOUND|THR_JOINABLE | THR_INHERIT_SCHED ,
			  numOfThreads) == -1 )
  {
    XGLOG_ERROR( "xGateProcessor::startThread(%s)"\
        " ACE returned error while spawning", m_serverType);
    retVal = EN_XGATE_STATUS_ERROR;
  }
  else
  {
    XGLOG_WARN( "xGateProcessor::init(%s) Success", m_serverType);
  }
  return retVal;
}

int xGateProcessor::svc(void)
{
  XGLOG_WARN( "xGateProcessor::svc(%s)", m_serverType);
  ACE_Message_Block *pAmb = 0;

  while(m_run)
  {
    int result = getq(pAmb, NULL);
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
      XGLOG_ERROR( "xGateProcessor::svc(%s) Error in Message Queue", m_serverType);
      m_run = false;
    }
    // delete the message received
    if(pAmb)
    {
      //delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_WARN( "xGateProcessor::svc(%s) Stopped Dispatch Task Thread", m_serverType);
  return 0;
}

xGateRetVal xGateProcessor::handle_msg(ACE_Message_Block *pAmb)
{
  XGLOG_INFO( "Enter xGateProcessor::handle_msg(%s)", m_serverType);

  xGateBaseMsg *pMsg = 0;
  pMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
  //pMsg = (xGateBaseMsg*)pAmb;

  if(pMsg) {
          XGLOG_WARN( "xGateProcessor::handle_msg(%s) calling process_msg", m_serverType);
	  process_msg(pMsg);
	  return EN_XGATE_STATUS_SUCCESS;
  }
#if 0
  // On the server processor, we handle  
  switch(pMsg->getMsgType())
  {
    case EN_XGATE_MSG_MGC_MG_IN:
    case EN_XGATE_MSG_MGC_MG_OUT:
      {
        XGLOG_WARN( "Exit: xGateProcessor::going to call process_msg");
        process_msg(pMsg);
        break;
      }
    default:
      {
        XGLOG_ERROR( "xGateProcessor::handle_msg"\
            " Invalid Message Type:%d",pMsg->getMsgType());
        break;
      }
  }
#endif
  XGLOG_WARN( "Exit: xGateProcessor::handle_msg");
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateProcessor::process_msg(xGateBaseMsg *pMsg)
{
  XGLOG_WARN( "Enter xGateProcessor::process_msg(%s)", m_serverType); 
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateProcessor::dispatch_msg(xGateBaseMsg *pMsg)
{
  XGLOG_WARN("Enter xGateProcessor::dispatch_msg(%s)", m_serverType);
  return EN_XGATE_STATUS_SUCCESS;
}

