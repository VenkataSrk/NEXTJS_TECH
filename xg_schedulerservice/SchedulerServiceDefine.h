#ifndef _SCHEDULER_SERVICE_DEFINE_H
#define _SCHEDULER_SERVICE_DEFINE_H

#include "IURDefines.h"

typedef enum
{
  EN_SCHEDULER_NULL = 0,
  EN_SCHEDULER_QUEUE_HOLD = 1,
  EN_SCHEDULER_AGENT_WAIT = 2,
  EN_SCHEDULER_AGENT_WARMUP = 3,
  EN_SCHEDULER_EXTN_WAIT = 4,
  EN_SCHEDULER_REFRESH_HUNTGROUP_INFO = 5,
  EN_SCHEDULER_PARK_TIMER = 6,
  EN_SCHEDULER_UNPARK_TIMER = 7,
  EN_SCHEDULER_URCONF_USER_JOIN_WAIT = 8,
  EN_SCHEDULER_URCONF_PORT_ALLOC_RES = 9,
  EN_SCHEDULER_URCONF_USER_DISCONNECT_NOTIFY = 10,
  EN_SCHEDULER_LWSS_DB_CONFIG_QUERY_TIMER = 11,
  EN_SCHEDULER_MAX = 12,
  EN_SCHEDULER_SIGN = 13,
  EN_SCHEDULER_INVITE_TIMER = 14,
  EN_SCHEDULER_OPTIONS_TIMER = 15,
  EN_SCHEDULER_WHITE_LIST_REFRESH
} SchedulerType;

typedef enum
{
  SCHEDULER_FUNCTION_NULL = 0,
  SCHEDULER_FUNCTION_SET_TIMER,
  SCHEDULER_FUNCTION_CANCEL_TIMER
} TimerFunction;

class SchedulerInfo
{
public:
  SchedulerInfo()
  {
    m_pCtxdata = nullptr;
  }
  ~SchedulerInfo()
  {
    m_pCtxdata = nullptr;
  }

  SchedulerInfo(const SchedulerInfo &rhs)
  {
    m_contextId = rhs.m_contextId;
    m_schedulerType = rhs.m_schedulerType;
    m_retryCount = rhs.m_retryCount;
    m_timeoutSecond = rhs.m_timeoutSecond;
    m_requestorModule = rhs.m_requestorModule;
    m_enumTimerFunc = rhs.m_enumTimerFunc;
    m_pCtxdata = rhs.m_pCtxdata;
  }

  SchedulerInfo &operator=(const SchedulerInfo &rhs)
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
  void *m_pCtxdata;
};
#endif
