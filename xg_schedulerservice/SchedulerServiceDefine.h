#ifndef _SCHEDULER_SERVICE_DEFINE_H
#define _SCHEDULER_SERVICE_DEFINE_H

#include "IURDefines.h"

typedef enum
{
  EN_SCHEDULER_NULL=0,
  EN_SCHEDULER_DTMF_INTERDIGIT_TIMER = 1,
  EN_SCHEDULER_BOT_RESP_WAIT_TIMER = 2,
  EN_SCHEDULER_CUSTOMER_RESP_WAIT_TIMER = 3,
  EN_SCHEDULER_MAX,
} SchedulerType;

typedef enum{
  SCHEDULER_FUNCTION_NULL=0,
  SCHEDULER_FUNCTION_SET_TIMER,
  SCHEDULER_FUNCTION_CANCEL_TIMER
}TimerFunction;

class SchedulerInfo
{
  public:
	SchedulerInfo()
	{
	  m_pCtxdata = NULL;
	}
 	~SchedulerInfo()
	{
	  m_pCtxdata = NULL;
	}

	SchedulerInfo(const SchedulerInfo& rhs)
	{
	  m_contextId = rhs.m_contextId;
	  m_schedulerType = rhs.m_schedulerType;
	  m_retryCount = rhs.m_retryCount;
    m_timeoutSecond = rhs.m_timeoutSecond;
	  m_requestorModule = rhs.m_requestorModule;
    m_enumTimerFunc = rhs.m_enumTimerFunc;
	  m_pCtxdata = rhs.m_pCtxdata;
 	}
	
	SchedulerInfo& operator = (const SchedulerInfo& rhs)
	{
	  m_contextId = rhs.m_contextId;
    m_schedulerType = rhs.m_schedulerType;
    m_retryCount = rhs.m_retryCount;
	  m_timeoutSecond = rhs.m_timeoutSecond;
	  m_requestorModule = rhs.m_requestorModule;
    m_enumTimerFunc = rhs.m_enumTimerFunc;
	  m_pCtxdata = rhs.m_pCtxdata;
	}

	std::string m_contextId;
 	SchedulerType m_schedulerType;
  int m_retryCount;
  unsigned int m_timeoutSecond;
  IURDefines::MODULE_ID m_requestorModule;        
  TimerFunction m_enumTimerFunc;
	void* m_pCtxdata;

};
#endif

