#ifndef _SCHEDULER_CONTEXT_H
#define _SCHEDULER_CONTEXT_H

#include "SchedulerServiceDefine.h"
#include <map>
#include <string>

class SchedulerContext
{
   typedef std::pair<std::string, SchedulerType> TIMER_ID_KEY;
   //typedef std::map<TIMER_ID_KEY, long int> SCHEDULER_CONTEXT_MAP;
   typedef std::map<TIMER_ID_KEY, SchedulerCtxInfo> SCHEDULER_CONTEXT_MAP;
   typedef SCHEDULER_CONTEXT_MAP::iterator IT_SCHEDULER_CONTEXT_MAP;

   SCHEDULER_CONTEXT_MAP m_mapSchedulerContext;

   public:
   SchedulerContext()
   {}
   ~SchedulerContext()
   {}

   bool insertSchedulerContext(std::string &uid, SchedulerType schedulerType, long int timerId, long int duration);
   bool deleteSchedulerContext(std::string &uid, SchedulerType schedulerType);
   bool deleteSchedulerContext(std::string &uid, SchedulerType schedulerType, long int &timeLeft);
   long int getSchedulerContext(std::string &uid, SchedulerType schedulerType);
};
#endif

