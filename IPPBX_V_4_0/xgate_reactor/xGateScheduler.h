#ifndef _XGATE_SCHEDULER_H
#define _XGATE_SCHEDULER_H

//ace includes
#include <ace/Event_Handler.h>
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>

//local includes
#include "xGateUtil.h"

class xGateScheduler : public ACE_Event_Handler
{
  public:
    xGateScheduler(const char* schedulerType);
    virtual ~xGateScheduler();

    xGateRetVal init (void);
    static void run (void* arguments);
    xGateRetVal stopThread( void );

    /// Returns the Reactor pointer
    ACE_Reactor * getReactor(void)
    {
      return m_reactor;
    }

    /// Called back on handle_time out
    virtual int handle_timeout(const ACE_Time_Value&, const void *); 

  private:
    bool m_running;
    string m_schedulerType;

  protected: 
    ACE_Reactor *m_reactor;
};

#endif
