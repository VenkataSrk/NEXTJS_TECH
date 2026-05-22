#ifndef _XGATE_SCHEDULER_MSG_H
#define _XGATE_SCHEDULER_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
class xGateSchedulerMsg : public xGateBaseMsg
{
  public:
    xGateSchedulerMsg(void) : xGateBaseMsg(IURDefines::UR_MODULE_SCHEDULER_SERVICE),
    m_timerData(NULL) {};
    ~xGateSchedulerMsg(){};

    xGateTimerData* getTimerData() 
    { 
      return m_timerData; 
    };

    void setTimerData(xGateTimerData *timerData) 
    {
      m_timerData = timerData;
    };

  protected:
    xGateSchedulerMsg& operator=(const xGateSchedulerMsg&);

  private:
    // Not Implemented Methods
    xGateSchedulerMsg(const xGateSchedulerMsg&);
    xGateTimerData *m_timerData;
};
#endif
