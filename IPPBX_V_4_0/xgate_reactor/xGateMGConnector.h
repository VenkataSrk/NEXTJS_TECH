#ifndef _XGATE_MG_CONNECTOR_H
#define _XGATE_MG_CONNECTOR_H

//Ace Includes
#include <ace/Reactor.h>
#include <ace/SOCK_Stream.h>
#include <ace/Task_T.h>
#include <ace/SOCK_Connector.h>

// Local Includes
#include "log.h"
//#include "klog.h"
#include "xGateUtil.h"
#include "xGateConnector.h"


class xGateMGConnector : public xGateConnector
{
  public:

    /**
     *  Default Constructor
     */
    xGateMGConnector(ACE_Task<ACE_MT_SYNCH>*);

    /**
     * Destructor
     */
    virtual ~xGateMGConnector (void);

    /// Interface to schedule the timer
    xGateRetVal connect_to_peer(xGateTCPConnectionStr&); 

    /// Called back on handle_time out
    int handle_timeout(const ACE_Time_Value&, const void *); 

    ACE_Task<ACE_MT_SYNCH>* getTask(void)
    {
      return m_taskPtr;
    }

  private:

    /**
     * Private copy constructor - disallow copies and automatic methods.
     **/
    xGateMGConnector(const xGateMGConnector& rhs);

    /**
     * Private assignment operator - disallow assignment and automatic methods.
     **/
    xGateMGConnector& operator= (const xGateMGConnector& rhs);

    /// Interface to make the connections
    xGateRetVal make_connection();

    /// Timer Interval in Sec's
    //int  m_timerCount;


    /// Lock For Connector Timer
    //    ACE_Thread_Mutex  m_conTimerLock;
    ACE_Task<ACE_MT_SYNCH>* m_taskPtr;

};
#endif
