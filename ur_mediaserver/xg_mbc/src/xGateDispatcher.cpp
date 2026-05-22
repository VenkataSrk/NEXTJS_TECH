
//local includes
#include "xGateDispatcher.h"
#include "xglog.h"
#define THISMODULE "Dispatch"

xGateDispatcher::xGateDispatcher(const char *serverType):
  ACE_Task<ACE_MT_SYNCH>(),
  m_run(false)
{
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType, serverType);
  memset(m_peerAddress,0,SERVER_ADDRESS_LENGTH);
  XGLOG_ERROR(THISMODULE "xGateDispatcher::xGateDispatcher(%s)", m_serverType);
}

xGateDispatcher::~xGateDispatcher(void)
{
  XGLOG_ERROR(THISMODULE "xGateDispatcher::~xGateDispatcher(%s)", m_serverType);
}

xGateRetVal xGateDispatcher::stop(void)
{
  XGLOG_ERROR(THISMODULE "xGateDispatcher::stop(%s)", m_serverType);

  // set the flag to false. In the thread method the while loop will be
  // stopped
  m_run = false;
  // Deactivate the message queue. Any one waiting on the message queue will
  // come out
  msg_queue()->deactivate();
  // wait till the thread is over
  wait();

  XGLOG_ERROR(THISMODULE "xGateDispatcher::stop Stopped xGateDispatcher(%s)", m_serverType);

  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateDispatcher::init()
{
  XGLOG_ERROR(THISMODULE "xGateDispatcher::init(%s)", m_serverType);
  m_run = true;
  xGateRetVal retVal = EN_XGATE_STATUS_SUCCESS;

  // Start the Dispatcher Thread. It will call the svc() method
  if (activate(THR_NEW_LWP , 1) == -1 )
  {
    // Error while starting
    XGLOG_ERROR(THISMODULE "xGateDispatcher::startThread(%s)"\
        " ACE returned error while spawning", m_serverType);
    retVal = EN_XGATE_STATUS_ERROR;
  }
  else
  {
    // Successfull start
    XGLOG_ERROR(THISMODULE "xGateDispatcher::init(%s) Success", m_serverType);
  }
  return retVal;
}

int xGateDispatcher::svc(void)
{
  XGLOG_ERROR(THISMODULE "xGateDispatcher::svc(%s)", m_serverType);
  cout << "Started dispatching thread;" << endl;

  // Message block in which the message is read
  ACE_Message_Block *pAmb = 0;

  // Till the flag run_ is true this loop will continue and the thread will
  // exist
  while(m_run)
  {
    // Read the message from the queue. If no message is present then this
    // method will block. This methods returns when the message is read or
    // the queue is deactivated
    // Get message from the queue
    int result = getq(pAmb, NULL);
    if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
    {
      continue;
    }

    // check the result
    if ((result != -1))
    {
      ///Process and Dispatch Message
      cout << "Process dispatch message" << endl;
      process_msg(pAmb);
    }
    else
    {
      // This is the case where the message queue is de-activated.
      // Need to stop the thread
      XGLOG_ERROR(THISMODULE "xGateDispatcher::svc(%s) Error in Message Queue", m_serverType);
      m_run = false;
    }

    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }
  return 0;
}

//implementation done at sub class
xGateRetVal xGateDispatcher::process_msg(ACE_Message_Block *pAmb)
{
  XGLOG_ERROR(THISMODULE "xGateDispatcher::dispatchMessage(%s)", m_serverType);
  return EN_XGATE_STATUS_SUCCESS;
}

