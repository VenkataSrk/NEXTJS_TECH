#ifndef _XGATE_SCHEDULER_SERVICE_MSG_H
#define _XGATE_SCHEDULER_SERVICE_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "URMRCPUtil.h"
#include "xGateSchedulerHandler.h"

class xGateSchedulerServiceMsg : public URMRCPBaseMsg
{
  public:
    xGateSchedulerServiceMsg(void) :
    URMRCPBaseMsg(IURDefines::UR_MODULE_SCHEDULER_SERVICE)
    {};
    virtual ~xGateSchedulerServiceMsg()
    {};
    SchedulerInfo& get_scheduler_info() 
    {
      return m_schedulerInfo;
    }
    void set_scheduler_info(SchedulerInfo &schedulerInfo) {
      m_schedulerInfo = schedulerInfo;
    }

  protected:
    xGateSchedulerServiceMsg& operator=(const xGateSchedulerServiceMsg&);

  private:
    xGateSchedulerServiceMsg(const xGateSchedulerServiceMsg&);
    SchedulerInfo m_schedulerInfo;
};
#endif

