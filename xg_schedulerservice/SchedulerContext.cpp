#include "xglog.h"
#include "SchedulerContext.h"

bool SchedulerContext::insertSchedulerContext(std::string &uid, SchedulerType schedulerType, long int timerId)
{
   TIMER_ID_KEY pairKey = make_pair(uid, schedulerType);	
   IT_SCHEDULER_CONTEXT_MAP it = m_mapSchedulerContext.find(pairKey);
   if(it != m_mapSchedulerContext.end()){
      XGLOG_ERROR(" Inserting the timer id failed. Context id already exist in map!");
      return false;
   }
   m_mapSchedulerContext.insert(SCHEDULER_CONTEXT_MAP::value_type(pairKey, timerId));
   XGLOG_DEBUG(" Inserted timer id in the map!");
   return true;
}

bool SchedulerContext::deleteSchedulerContext(std::string &uid, SchedulerType schedulerType)
{
   TIMER_ID_KEY pairKey = make_pair(uid, schedulerType);
   IT_SCHEDULER_CONTEXT_MAP it = m_mapSchedulerContext.find(pairKey);
   if(it == m_mapSchedulerContext.end()){
      XGLOG_ERROR(" Retrieving the timer id failed. Context id does not exist in map!");
      return false;
   }
   m_mapSchedulerContext.erase(pairKey);

   XGLOG_DEBUG(" Deleted timer id from the map!");
   return true;
}

long int SchedulerContext::getSchedulerContext(std::string &uid, SchedulerType schedulerType)
{
   long int timerId = -1;
   TIMER_ID_KEY pairKey = make_pair(uid, schedulerType);
   IT_SCHEDULER_CONTEXT_MAP it = m_mapSchedulerContext.find(pairKey);
   if(it == m_mapSchedulerContext.end()){
      XGLOG_ERROR(" Retrieving the timer id failed. Context id does not exist in map!");
      return 0;
   }
   timerId = it->second;
   if(timerId == -1) {
      XGLOG_ERROR("Timer id is nullptr");
      return 0;
   }
   XGLOG_DEBUG("Got the timer id from the map!");
   return timerId;
}

