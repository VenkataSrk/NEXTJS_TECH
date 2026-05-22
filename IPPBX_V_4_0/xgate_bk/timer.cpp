/*----------------------------------------------------------------* 
 * $Archive: /xGate/timer.cpp $
 * $Date: 3/08/04 9:57 $
 * $Revision: 5 $
 *
 * $History: timer.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Sonny        Date: 3/08/04    Time: 9:57
 * Updated in $/xGate
 * 
 * *****************  Version 4  *****************
 * User: Sonny        Date: 24/05/04   Time: 10:54
 * Updated in $/xGate
 * added Timer APIs: TimerAlloc, TimerFree, TimerSchedule, TimerCancel,
 * TimerGet
 * 
 * *****************  Version 3  *****************
 * User: Sonny        Date: 23/01/02   Time: 16:45
 * Updated in $/xGate
 * Clear all timer jobs on ~TimerManager
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 9/01/02    Time: 12:09
 * Updated in $/xGate
 * Put into operational state
 * 
 * *****************  Version 1  *****************
 * User: Bennylp      Date: 3/22/01    Time: 4:31p
 * Created in $/ct15
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 12/03/01   Time: 22:22
 * Created in $/ct2
 * Created
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 *  
 *----------------------------------------------------------------*/

//#include "StdAfx.h"
#include <algorithm>
#include "timer.h"
#include "api.h"
#include "task.h"
//#include "common.h"


#define THISMODULE "Timer"

TimerManager* TimerManager::_theTimerManager = NULL;


#define TIMER_API_TIMER_MAX_COUNT	1024

unsigned long GetTickCount()
{
  struct timeval tv;
  if( gettimeofday(&tv, NULL) != 0 )
  return 0;

  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}


class Timer_API;
class Timer_API_Timer : public Timer
{
    friend class Timer_API;
private:
    Timer_API *const api_;

    Timer_API_Timer(Timer_API *api);

    virtual void OnTimer(int timerId,void *param);
};

class Timer_API : public CAPIProvider
{
    friend class Timer_API_Timer;

public:
    enum API_Id {
	Id_Alloc,
	Id_Free, 
	Id_Schedule,
	Id_Cancel,
	Id_Get,
    };

    Timer_API();
    ~Timer_API();

    void DetachTask(CTask *);
private:

    struct TimerInfo {
	int id_;
	bool active_;
	CTask *task_;
	bool expired_;
    };

    Timer_API_Timer *const api_timer_;
    TimerInfo timers_[TIMER_API_TIMER_MAX_COUNT];

    virtual void OnTimer(int timerId);


    STATUS API_CALL TimerAlloc(CTask* task, int *retval);
    STATUS API_CALL TimerFree(CTask* task, int timerId);
    STATUS API_CALL TimerSchedule(CTask* task, int timerId, int duration);
    STATUS API_CALL TimerCancel(CTask* task, int timerId);
    STATUS API_CALL TimerGet(CTask* task, int *retval);
};


/** Timer_API_Timer implementation
 */
Timer_API_Timer::Timer_API_Timer(Timer_API *api)
: api_(api)
{
}

void Timer_API_Timer::OnTimer(int timerId,void *param)
{
    api_->OnTimer(timerId);
}

/** Timer_API implementation
 */
Timer_API::Timer_API() 
: CAPIProvider(API_PROVIDER_ID_TIMER, THISMODULE), api_timer_(new Timer_API_Timer(this))
{

    if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
	CLog::Error( THISMODULE, "Unable to register to api provider");
	return;
    }

    RegisterAPI(
	"TimerAlloc",				// external name
	Id_Alloc,				// ordinal
	false,					// asynchronous flag
	CVariant::VT_INT,			// return type
	(CAPIProvider::APIPROC)&Timer_API::TimerAlloc,
	CAPIDef::CDECL_TYPE,			// calling convention
	2,					// number of arguments
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("retval",BYREF,CVariant::VT_RET)
    );

    RegisterAPI(
	"TimerFree",				// external name
	Id_Free,				// ordinal
	false,					// asynchronous flag
	CVariant::VT_VOID,			// return type
	(CAPIProvider::APIPROC)&Timer_API::TimerFree,
	CAPIDef::CDECL_TYPE,			// calling convention
	2,					// number of arguments
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("timerID",BYVAL,CVariant::VT_INT)
    );


    RegisterAPI(
	"TimerSchedule",			// external name
	Id_Schedule,				// ordinal
	false,					// asynchronous flag
	CVariant::VT_VOID,			// return type
	(CAPIProvider::APIPROC)&Timer_API::TimerSchedule,
	CAPIDef::CDECL_TYPE,			// calling convention
	3,					// number of arguments
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("timerID",BYVAL,CVariant::VT_INT),
	CArgType("duration",BYVAL,CVariant::VT_INT)
    );

    RegisterAPI(
	"TimerCancel",			// external name
	Id_Cancel,				// ordinal
	false,					// asynchronous flag
	CVariant::VT_VOID,			// return type
	(CAPIProvider::APIPROC)&Timer_API::TimerCancel,
	CAPIDef::CDECL_TYPE,			// calling convention
	2,					// number of arguments
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("timerID",BYVAL,CVariant::VT_INT)
    );


    RegisterAPI(
	"TimerGet",				// external name
	Id_Get,					// ordinal
	false,					// asynchronous flag
	CVariant::VT_INT,			// return type
	(CAPIProvider::APIPROC)&Timer_API::TimerGet,
	CAPIDef::CDECL_TYPE,			// calling convention
	2,					// number of arguments
	CArgType("task",BYVAL,CVariant::VT_TASK),
	CArgType("retval",BYREF,CVariant::VT_RET)
    );

    for (int i = 0; i < TIMER_API_TIMER_MAX_COUNT; i++) {
	timers_[i].id_ = i;
	timers_[i].active_ = false;
    }

}

Timer_API::~Timer_API()
{
    delete api_timer_;
}

void Timer_API::OnTimer(int timerId)
{
    TimerInfo *timer_info = &timers_[timerId];
    if (timer_info->active_) {
    	timer_info->expired_ = true;
	timer_info->task_->RaiseException(EX_TIMER, "Timer");
    }

}

STATUS Timer_API::TimerAlloc(CTask* task, int *retval)
{
    for (int id = 0; id < TIMER_API_TIMER_MAX_COUNT; id++) {
	if (timers_[id].active_ == false) {
	    timers_[id].active_ = true;
	    timers_[id].expired_ = false;
	    timers_[id].task_ = task;
	    *retval = id;
	    return OK;
	}
    }
    *retval = -1;
    return OK;
}

STATUS Timer_API::TimerFree(CTask* task, int timerId)
{
    if (timerId < 0 || timerId >= TIMER_API_TIMER_MAX_COUNT) {
	CLog::Error(THISMODULE, "Invalid timer id");
	return OK;
    }
    TimerInfo *timer_info = &timers_[timerId];
    if (timer_info->active_ && timer_info->task_ == task) {
	api_timer_->CancelTimer(timerId);
	timers_[timerId].active_ = false;
    }
    return OK;
}

STATUS Timer_API::TimerSchedule(CTask* task, int timerId, int duration)
{
    if (timerId < 0 || timerId >= TIMER_API_TIMER_MAX_COUNT) {
	CLog::Error(THISMODULE, "Invalid timer id");
	return OK;
    }
    TimerInfo *timer_info = &timers_[timerId];
    if (timer_info->active_ && timer_info->task_ == task)
        api_timer_->ScheduleTimer(timerId, duration, NULL);
    return OK;
}

STATUS Timer_API::TimerCancel(CTask* task, int timerId)
{
    if (timerId < 0 || timerId >= TIMER_API_TIMER_MAX_COUNT) {
	CLog::Error(THISMODULE, "Invalid timer id");
	return OK;
    }
    TimerInfo *timer_info = &timers_[timerId];
    if (timer_info->active_ && timer_info->task_ == task)
	api_timer_->CancelTimer(timerId);
    return OK;
}

STATUS Timer_API::TimerGet(CTask* task, int *retval)
{
    for (int id = 0; id < TIMER_API_TIMER_MAX_COUNT; id++) {
	TimerInfo *timer_info = &timers_[id];
	if (timer_info->active_ && timer_info->task_ == task && timer_info->expired_) {
	    timer_info->expired_ = false;
	    *retval = id;
	    return OK;
	}
    }
    *retval = -1;
    return OK;
}

void Timer_API::DetachTask(CTask *task)
{
    for (int id = 0; id < TIMER_API_TIMER_MAX_COUNT; id++) {
	TimerInfo *timer_info = &timers_[id];
	if (timer_info->active_ && timer_info->task_ == task) {
	    api_timer_->CancelTimer(id);
	    timer_info->active_ = false;
	}
    }
}


/** Data structure that represents each timer job (active timer)
 */
class TimerJob
{
public:
    TimerJob(unsigned expiry,int timerId,void *param,Timer *owner) :
	_expiry(expiry),_timerId(timerId),_param(param),_owner(owner) {
    }

    unsigned _expiry;
    int _timerId;
    void *_param;
    Timer *_owner;
};


/** Constructor: TimerManager is a singleton that is automatically 
    created by the first call to Instance.
 */
TimerManager::TimerManager() 
: ServiceProvider("Timer", true, true, Lowest_Priority, "Timer Manager")
{
    _lastCheckMsec = 0;
}

/** Destructor: do nothing
 */
TimerManager::~TimerManager()
{
    TimerManager::ListTimer::iterator it=_timers.begin();
    TimerManager::ListTimer::iterator end=_timers.end();
    while (it != end) {
	TimerJob *tjob = *it;
	delete tjob;
	it++;
    }
    _timers.clear();
    _theTimerManager = NULL;

}

/** HandleEvents: Called regularly by the Service Provider Manager
 */
void TimerManager::HandleEvents()
{
    // only check expired timers at least once every 100 msecs
    if (_lastCheckMsec+100 < GetTickCount()) {
	if (_timers.size()) {
	    TimerManager *mgr = TimerManager::Instance();
	    ListTimer::iterator it=mgr->_timers.begin();
	    ListTimer::iterator end=mgr->_timers.end();
	    while (it != end) {
		TimerJob *p = *it;
		if (p->_expiry > GetTickCount()) break;
		TimerJob tjob = *p;
		delete p;
		mgr->_timers.erase(it);
		tjob._owner->OnTimer(tjob._timerId,tjob._param);
		it = mgr->_timers.begin();
	    }
	}
	_lastCheckMsec = GetTickCount();
    }
}

/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
 */
bool TimerManager::Load()
{
    api_ = new Timer_API;
    return true;
}

/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
 */
bool TimerManager::Unload()
{
    delete api_;
    return true;
}

/** Returns the only instance of TimerManager. If no TimerManager has been
    instantiated, it creates one before returning a pointer to it
 */
TimerManager *TimerManager::Instance()
{
    if (_theTimerManager == NULL) 
	_theTimerManager = new TimerManager;
    return _theTimerManager;
}

/** Create the singleton of TimerManager if not created yet
 */
TimerManager *TimerManager::Create()
{
    return Instance();
}

void TimerManager::DetachTask(CTask *task)
{
    api_->DetachTask(task);
}

//-----------------------------------------------------------
/** Constructor: Timer constructor
 */
Timer::Timer()
{
}

/** Destructor
 */
Timer::~Timer()
{
    TimerManager *mgr = TimerManager::Instance();
    if (!mgr) return;
    TimerManager::ListTimer::iterator it=mgr->_timers.begin();
    TimerManager::ListTimer::iterator end=mgr->_timers.end();
    while (it != end) {
	TimerJob *tjob = *it;
	if (tjob->_owner == this) {
	    delete tjob;
	    it = mgr->_timers.erase(it);
	} else {
	    it++;
	}
    }
}

/** Schedule a timer. Within duration msec, the method OnTimer of the descendant
    class will be invoked with timerId and param as parameters
 */
bool Timer::ScheduleTimer(int timerId,unsigned duration,void *param)
{
    CancelTimer(timerId);

    TimerJob *tjob = new TimerJob(GetTickCount()+duration,timerId,param,this);
    TimerManager *mgr = TimerManager::Instance();
    TimerManager::ListTimer::iterator it=mgr->_timers.begin();
    TimerManager::ListTimer::iterator end=mgr->_timers.end();
    for (;it!=end;it++) {
	if (tjob->_expiry <= (*it)->_expiry) 
	    break;
    }
    mgr->_timers.insert(it,tjob);

    return true;
}

/** Cancel an active timer referenced by int
 */
bool Timer::CancelTimer(int timerId)
{
    TimerManager *mgr = TimerManager::Instance();
    TimerManager::ListTimer::iterator it=mgr->_timers.begin();
    TimerManager::ListTimer::iterator end=mgr->_timers.end();
    for (;it!=end;it++) {
	TimerJob *tjob = *it;
	if (tjob->_timerId == timerId && tjob->_owner == this) {
	    delete tjob;
	    mgr->_timers.erase(it);
	    break;
	}
    }
    return true;
}


