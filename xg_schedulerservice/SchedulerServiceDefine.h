#ifndef _SCHEDULER_SERVICE_DEFINE_H
#define _SCHEDULER_SERVICE_DEFINE_H

#include "IURDefines.h"

typedef enum
{
  EN_SCHEDULER_NULL=0,
  EN_SCHEDULER_URCONF_USER_JOIN_WAIT=1,
  EN_SCHEDULER_URCONF_PORT_ALLOC_RES=2,
  EN_SCHEDULER_URCONF_USER_DISCONNECT_NOTIFY=3,
  EN_SCHEDULER_URCONF_ACTIVE_SPEAKER_INFO=4,
  EN_SCHEDULER_URCONF_ACD_RES=5,
	EN_SCHEDULER_URCONF_MEETING_DURATION=6,
  EN_SCHEDULER_MAX,
} SchedulerType;

typedef enum{
  SCHEDULER_FUNCTION_NULL=0,
  SCHEDULER_FUNCTION_SET_TIMER,
  SCHEDULER_FUNCTION_CANCEL_TIMER
}TimerFunction;

class SchedulerCtxInfo
{
  public:
  SchedulerCtxInfo()
  {
    m_timerId = -1;
    m_duration = 0;
  }
  ~SchedulerCtxInfo()
  {
    m_timerId = -1;
    m_duration = 0;
  }

  SchedulerCtxInfo(const SchedulerCtxInfo& rhs)
  {
    m_timerId = rhs.m_timerId;
    m_duration = rhs.m_duration;
    m_startTime = rhs.m_startTime;
  }
  SchedulerCtxInfo& operator = (const SchedulerCtxInfo& rhs)
  {
    m_timerId = rhs.m_timerId;
    m_duration = rhs.m_duration;
    m_startTime = rhs.m_startTime;
    return *this;
  }

  long int m_timerId;
  long int m_duration;
  ACE_Time_Value m_startTime;
};

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
      return *this;
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

