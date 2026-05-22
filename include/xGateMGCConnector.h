#ifndef _XGATE_MG_CONNECTOR_H
#define _XGATE_MG_CONNECTOR_H

//Ace Includes
#include <ace/Reactor.h>
#include <ace/SOCK_Stream.h>
#include <ace/Task_T.h>
#include <ace/SOCK_Connector.h>

// Local Includes
#include "xGateUtil.h"
#include "xGateConnector.h"


class xGateMGCConnector : public xGateConnector
{
  public:

    /**
     *  Default Constructor
     */
    xGateMGCConnector(ACE_Task<ACE_MT_SYNCH>*);

    /**
     * Destructor
     */
    virtual ~xGateMGCConnector (void);

    /// Interface to schedule the timer
    xGateRetVal connect_to_peer(); 

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
    xGateMGCConnector(const xGateMGCConnector& rhs);

    /**
     * Private assignment operator - disallow assignment and automatic methods.
     **/
    xGateMGCConnector& operator= (const xGateMGCConnector& rhs);

    /// Interface to make the connections
    xGateRetVal make_connection();



    /// Lock For Connector Timer
    ACE_Task<ACE_MT_SYNCH>* m_taskPtr;

    xGateRetVal post_mg_register_msg(); 

};
#endif
