/*----------------------------------------------------------------* 
 * $Archive: /xGate/timer.h $
 * $Date: 24/05/04 10:54 $
 * $Revision: 5 $
 *
 * $History: timer.h $
 * 
 * *****************  Version 5  *****************
 * User: Sonny        Date: 24/05/04   Time: 10:54
 * Updated in $/xGate
 * added Timer APIs: TimerAlloc, TimerFree, TimerSchedule, TimerCancel,
 * TimerGet
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 9/01/02    Time: 12:09
 * Updated in $/xGate
 * Put into operational state
 * 
 * *****************  Version 3  *****************
 * User: Bennylp      Date: 4/23/01    Time: 10:47p
 * Updated in $/xGate
 * Fixed std namespace conflicts.
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 11/04/01   Time: 11:37
 * Updated in $/ct15
 * 
 * *****************  Version 1  *****************
 * User: Bennylp      Date: 3/22/01    Time: 4:32p
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

#if !defined(CT2_TIMER_H__INCLUDED_)
#define CT2_TIMER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "ServiceProvider.h"
#include "serviceprovider.h"
//#include "common.h"


/** Classes which wish to implement timer functionnality shall derive
    from this class and override OnTimer method
 */
class Timer
{
protected:
    /** Constructor
     */
    Timer();

    /** Destructor: It will automatically stop active timers made 
	by this instance
     */
    virtual ~Timer();

public:

    /** Abstract method that must be implemented by descendant classes.
	This method is called by TimerManager once the timer is expired
     */
    virtual void OnTimer(int timerId,void *param) = 0;

    /** Start an active timer for this instance.int and param arguments 
	will be passed to OnTimer method after duration msec
     */
    bool ScheduleTimer(int timerId,unsigned duration,void *param);

    /** Cancel an active timer referenced by int
     */
    bool CancelTimer(int timerId);
};


/** Forward declaration of ActiveTimer
 */
class TimerJob;


class Timer_API;
/** TimerManager class
    This singleton class manages all active timers triggered by objects
    descendamt of class Timer
 */
class TimerManager : 
    public ServiceProvider
{
    /** List of active timers
     */
    typedef List<TimerJob*> ListTimer;
    ListTimer _timers;

    /* other member variables
     */
    unsigned _lastCheckMsec;

    // pointer to the singleton
    static TimerManager* _theTimerManager;

    /** Constructor: TimerManager is a singleton that is automatically 
	created by the first call to Instance.
     */
    TimerManager();

    friend class Timer;

public:
    Timer_API *api_;

    /** Destructor
     */
    virtual ~TimerManager();

    /** Returns the only instance of TimerManager. If no TimerManager has been
	instantiated, it creates one before returning a pointer to it
    */
    static TimerManager *Instance();

    /** Create the singleton of TimerManager if not created yet
    */
    static TimerManager *Create();

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    virtual bool AttachTask(CTask *) { return true; }
    virtual void DetachTask(CTask *);

    /** HandleEvents: Descendent shall implement this abstract method.
	It is called by Service Provider Manager
     */
    virtual void HandleEvents();
};


#endif // !defined(CT2_TIMER_H__INCLUDED_)
