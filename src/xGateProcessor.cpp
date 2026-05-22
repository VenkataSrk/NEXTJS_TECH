
//ace includes

//local includes
#include "xglog.h"
#include "../include/xGateProcessor.h"
#include "xGateUtil.h"
#define THISMODULE "Processor"

xGateProcessor::xGateProcessor(const char *serverType) :
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false)
{
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);
  memset(m_encodeBuffer,0,ENCODE_BUF_SIZE);
  XGLOG_WARN( "xGateProcessor::xGateProcessor(%s)", (char*)ur_log_string (m_serverType));
}

xGateProcessor::~xGateProcessor(void)
{
  XGLOG_WARN( "xGateProcessor::~xGateProcessor(%s)", (char*)ur_log_string (m_serverType));
}

xGateRetVal xGateProcessor::stop(void)
{
  XGLOG_WARN( "xGateProcessor::stop(%s)", (char*)ur_log_string (m_serverType));

  m_run = false;
  msg_queue()->deactivate();
  wait();

  XGLOG_WARN( "xGateProcessor::stop Stopped xGateProcessor(%s)", (char*)ur_log_string (m_serverType));
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateProcessor::init(int numOfThreads)
{
  XGLOG_WARN( "xGateProcessor::init(%s)", (char*)ur_log_string (m_serverType));
  m_run = true;
  xGateRetVal retVal = EN_XGATE_STATUS_SUCCESS;

  // Start the Dispatcher Thread.
  if (activate(THR_NEW_LWP ,numOfThreads) == -1 )
  {
    XGLOG_ERROR( "xGateProcessor::startThread(%s)"\
        " ACE returned error while spawning", (char*)ur_log_string (m_serverType));
    retVal = EN_XGATE_STATUS_ERROR;
  }
  else
  {
    XGLOG_WARN( "xGateProcessor::init(%s) Success", (char*)ur_log_string (m_serverType));
  }
  return retVal;
}

int xGateProcessor::svc(void)
{
  XGLOG_WARN( "xGateProcessor::svc(%s)", (char*)ur_log_string (m_serverType));
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
      XGLOG_ERROR( "xGateProcessor::svc(%s) Error in Message Queue", (char*)ur_log_string (m_serverType));
      m_run = false;
    }
    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }

  XGLOG_WARN( "xGateProcessor::svc(%s) Stopped Dispatch Task Thread", (char*)ur_log_string (m_serverType));
  return 0;
}

xGateRetVal xGateProcessor::handle_msg(ACE_Message_Block *pAmb)
{
  XGLOG_WARN( "Enter xGateProcessor::handle_msg(%s)", (char*)ur_log_string (m_serverType));

  xGateBaseMsg *pMsg = 0;
  pMsg = dynamic_cast<xGateBaseMsg*>(pAmb);

  XGLOG_WARN( "Enter xGateProcessor &&");
  // On the server processor, we handle  
  switch(pMsg->getMsgType())
  {
    case EN_XGATE_MSG_MGC_MG_IN:
    case EN_XGATE_MSG_MGC_MG_OUT:
    case EN_XGATE_MSG_STT_TTS_IN:
#ifdef XGATE_HAS_SELF_AUDIT
    case EN_XGATE_MSG_AUDIT:
#endif
      {
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
  XGLOG_WARN( "Exit: xGateProcessor::handle_msg %d",pMsg->getMsgType());
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateProcessor::process_msg(xGateBaseMsg *pMsg)
{
  XGLOG_WARN( "Enter xGateProcessor::process_msg(%s)", (char*)ur_log_string (m_serverType)); 
  return EN_XGATE_STATUS_SUCCESS;
}
