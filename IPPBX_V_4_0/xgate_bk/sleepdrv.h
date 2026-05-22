/* $Header: /xGate/sleepdrv.h 14    10/22/02 5:14p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * This provider provides sleep() API.
 *
 * $Log: /xGate/sleepdrv.h $
 * 
 * 14    10/22/02 5:14p Bennylp
 * Move load monitor to thread_process.*
 * 
 * 13    6/25/02 8:50p Bennylp
 * Updated to support both CPU load and process load (version 43.0)
 * 
 * 12    19/06/02 18:45 Junanto
 * 
 * 11    18/06/02 22:17 Junanto
 * CPU load is now retrieved from performance data
 * to reflect the global load
 * 
 * 10    6/22/01 10:26p Bennylp
 * Protect when CancelJob can not find task state (strange!).
 * 
 * 9     6/13/01 8:32p Bennylp
 * Changes to internal data struct (uses vector now).
 * 
 * 8     6/13/01 4:53p Bennylp
 * Added API SleepEx for miliseconds precision.
 * 
 * 7     5/10/01 12:51p Bennylp
 * Temporary checkin
 * 
 * 6     5/02/01 6:16p Bennylp
 * Added Load_Monitor
 * 
 * 5     5/02/01 11:17a Bennylp
 * Fixed bug when task state is not destroyed when task killed.
 * 
 * 4     4/26/01 11:31p Bennylp
 * Added sleep-events tracing.
 * 
 * 3     4/13/01 11:16p Bennylp
 * Added Instance() member (needed by SystemAPI)
 * 
 * 2     4/13/01 8:36p Bennylp
 * Integration.
 * 
 */
#if !defined(_SLEEP_DRIVER_H__INCLUDED_)
#define _SLEEP_DRIVER_H__INCLUDED_

#include "serviceprovider.h"
#include "types.h"

class Sleep_Service;
class Sleep_API;
class Sleep_State;




//-----------------------------------------------------------------------------
// Sleep driver
//
class Sleep_Service : public ServiceProvider {
public:
    /** Create the singleton instance of this class.
     */
    static Sleep_Service * Create();

    /** Get the instance of this class.
	This is needed by System_API, because RaiseException() needs to block
	the task.
     */
    static Sleep_Service *Instance() {
	return sleepSvcInstance;
    }

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** This function will be called if this instance has a pending job for
	the task.
     */
    virtual bool CancelJob(CTask *);

    /** HandleEvents: Descendent shall implement this abstract method.
	It is called by Service Provider Manager
     */
    virtual void HandleEvents();

    /** This function will be called whenever a new task is created.
     */
    virtual bool AttachTask(CTask *);

    /** This function will be called whenever a task is about to be deleted.
     */
    virtual void DetachTask(CTask *);

    /** API Sleep() for SL Applications.
     */
    STATUS API_Sleep( CTask *,int seconds);

    /** API SleepEx() for SL Applications.
     */
    STATUS API_SleepEx( CTask *,int miliseconds);

private:
    /* Internal representation of a sleeping task inside sleep driver
     */
    struct Sleep_State {
	CTask	    *task;
	Time_Value  wakeupTime;
	bool	    active;

	Sleep_State() : active(false) {}
	Sleep_State(CTask *t) : task(t), active(false) {}
	Sleep_State(CTask *t, const Time_Value & w) : task(t), wakeupTime(w), active(false) { }
    };


    /* Sleep_State comparison class based on the time.
     */
    struct Sleep_State_Later {
	bool operator () (const Sleep_State *ss1, const Sleep_State *ss2) {
	    return ss1->wakeupTime > ss2->wakeupTime;
	}
    };

    static Sleep_Service *sleepSvcInstance;
    unsigned m_TLS;
    typedef Vector <Sleep_State*> SleepStateList_T;
    SleepStateList_T m_SleepTask;

    // API
    Sleep_API	    *m_SleepAPI;

    Sleep_Service();
    ~Sleep_Service();

    SleepStateList_T::iterator FindState ( Sleep_State *state, CTask *task, bool must_find ) {
	SleepStateList_T::iterator end;
	end = m_SleepTask.end();
	SleepStateList_T::iterator it = 
	    std::lower_bound(m_SleepTask.begin(), m_SleepTask.end(), state, Sleep_State_Later());
	while (it!=end && (*it)->task != task && (*it)->wakeupTime==state->wakeupTime)
	    ++it;
	if (it==end) return end;
	return (*it)->task==task ? it : end;

	//return std::find(m_SleepTask.begin(), end, state);
    }
};


#endif // !defined(_SLEEP_DRIVER_H__INCLUDED_)
