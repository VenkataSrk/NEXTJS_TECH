#ifndef _XGATE_SCHEDULER_H
#define _XGATE_SCHEDULER_H

//ace includes
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>

//local includes
#include "xGateUtil.h"

class xGateScheduler : public ACE_Event_Handler
{
  public:
    xGateScheduler();
    virtual ~xGateScheduler();

    bool init(void);
    static void run(void* arguments);
    bool stopThread(void);

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
