#ifndef _XGATE_SCHEDULER_HANDLER_H
#define _XGATE_SCHEDULER_HANDLER_H

//ace includes
#include <ace/Mutex.h>
#include <ace/Guard_T.h>

//local inludes
#include "xGateUtil.h"
#include "SchedulerServiceDefine.h"
#include "xGateScheduler.h"
#include "SchedulerContext.h"

class xGateSchedulerHandler : public xGateScheduler
{
  public:
 	static xGateSchedulerHandler* getInstance();
    	static void deleteInstance();
	bool doSchedulerRequest(SchedulerInfo &schedulerInfo);
  
    	xGateSchedulerHandler();
    	virtual ~xGateSchedulerHandler();

        SchedulerContext m_tSchedulerContext;

    	xGateSchedulerHandler(const xGateSchedulerHandler& rhs);
    	xGateSchedulerHandler& operator=(const xGateSchedulerHandler& rhs);
 
        virtual int handle_timeout(const ACE_Time_Value&, const void *);
  private:
	//member variables
	static xGateSchedulerHandler *m_instance;

};
#endif

