#include "xglog.h"
#include "SchedulerContext.h"

bool SchedulerContext::insertSchedulerContext(std::string &uid, SchedulerType schedulerType, long int timerId, long int duration)
{
   TIMER_ID_KEY pairKey = make_pair(uid, schedulerType);	
   IT_SCHEDULER_CONTEXT_MAP it = m_mapSchedulerContext.find(pairKey);
   if(it != m_mapSchedulerContext.end()){
      XGLOG_ERROR(" Inserting the timer id failed. Context id already exist in map!");
      return false;
   }
	 #if 0
   m_mapSchedulerContext.insert(SCHEDULER_CONTEXT_MAP::value_type(pairKey, timerId));
	 #else
	 ACE_Time_Value start_time = ACE_OS::gettimeofday();
	 //Update Scheduler context info
	 SchedulerCtxInfo schCtxInfo;
	 schCtxInfo.m_timerId = timerId;
	 schCtxInfo.m_duration = duration;
	 schCtxInfo.m_startTime = start_time;
   m_mapSchedulerContext.insert(SCHEDULER_CONTEXT_MAP::value_type(pairKey, schCtxInfo));
	 #endif
   XGLOG_DEBUG(" Inserted timer id in the map!");
   return true;
}

bool SchedulerContext::deleteSchedulerContext(std::string &uid, SchedulerType schedulerType, long int &timeLeft)
{
   TIMER_ID_KEY pairKey = make_pair(uid, schedulerType);
   IT_SCHEDULER_CONTEXT_MAP it = m_mapSchedulerContext.find(pairKey);
   if(it == m_mapSchedulerContext.end()){
      XGLOG_ERROR(" Retrieving the timer id failed. Context id does not exist in map!");
      return false;
   }
	 else
	 {
		 ACE_Time_Value now = ACE_OS::gettimeofday();
		 ACE_Time_Value start_time = it->second.m_startTime;
		 ACE_Time_Value elapsed = now - start_time;
		 ACE_Time_Value remaining = ACE_Time_Value(it->second.m_duration) - elapsed;	
		 timeLeft = remaining.sec();
	 }
   m_mapSchedulerContext.erase(pairKey);

   XGLOG_DEBUG(" Deleted timer id from the map!");
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
   //timerId = it->second;
   timerId = it->second.m_timerId;
   if(timerId == -1) {
      XGLOG_ERROR("Timer id is NULL");
      return 0;
   }
   XGLOG_DEBUG("Got the timer id from the map!");
   return timerId;
}

