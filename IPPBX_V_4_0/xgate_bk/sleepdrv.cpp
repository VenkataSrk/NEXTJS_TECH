/* $Header: /xGate/sleepdrv.cpp 24    4/23/03 5:34p Bennylp $
 *
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * This provider provides sleep() API.
 *
 * $Log: /xGate/sleepdrv.cpp $
 * 
 * 24    4/23/03 5:34p Bennylp
 * Administrivia: tidy-up the usage of API provider ID. Now the
 * enumeration in api.h should be used instead of bare constants.
 * 
 * 23    10/22/02 5:14p Bennylp
 * Move load monitor to thread_process.*
 * 
 * 22    6/25/02 8:50p Bennylp
 * Updated to support both CPU load and process load (version 43.0)
 * 
 * 21    19/06/02 18:45 Junanto
 * 
 * 20    18/06/02 22:17 Junanto
 * CPU load is now retrieved from performance data
 * to reflect the global load
 *
 * 19    10/17/01 7:56p Bennylp
 * Fixed bug in SleepEx interval calculation
 *
 * 18    10/05/01 7:21p Bennylp
 * Fixed GPF when DetachTask() can not get task's TLS
 *
 * 17    7/25/01 10:29a Bennylp
 * New HandleEvents() scheduling.
 *
 * 16    6/13/01 8:32p Bennylp
 * Changes to internal data struct (uses vector now).
 *
 * 15    6/13/01 4:53p Bennylp
 * Added API SleepEx for miliseconds precision.
 *
 * 14    5/30/01 4:37p Bennylp
 * Changed the driver name to distinguish with API name
 *
 * 13    5/10/01 12:51p Bennylp
 * Temporary checkin
 *
 * 12    8/05/01 18:30 Junanto
 * Fixed memory leak
 *
 * 11    5/02/01 6:16p Bennylp
 * Added Load_Monitor
 *
 * 10    5/02/01 3:45p Bennylp
 * Fixed bug, not registering to AttachTask
 *
 * 9     5/02/01 11:17a Bennylp
 * Fixed bug when task state is not destroyed when task killed.
 *
 * 8     4/26/01 11:31p Bennylp
 * Added sleep-events tracing.
 *
 * 7     4/23/01 10:47p Bennylp
 * Fixed std namespace conflicts.
 *
 * 6     23/04/01 13:45 Junanto
 *
 * 5     4/20/01 3:47a Bennylp
 * Fixed bug in API_Sleep.
 *
 * 4     14/04/01 20:00 Junanto
 *
 * 3     4/13/01 11:16p Bennylp
 * Added Instance() member (needed by SystemAPI)
 *
 * 2     4/13/01 8:36p Bennylp
 * Integration.
 *
 */
#include "stdafx.h"
#include <limits.h>
#include "task.h"
#include "sleepdrv.h"
#include "kernel.h"
#include "klog.h"
#include "api.h"

#define SLEEPDRV	    "SleepSVC"
#define SLEEP_PROVIDER_ID   API_PROVIDER_ID_SLEEP
#define SLEEP_MAX_POOL	    1024
#define __linux__


Sleep_Service * Sleep_Service::sleepSvcInstance;

static Log_Trace *sleepTrace;


//-----------------------------------------------------------------------------
// Sleep API
//
class Sleep_API : public CAPIProvider {
public:
    Sleep_API(int id);

    STATUS Init(Sleep_Service *d);
    STATUS Shutdown();

private:
    Sleep_Service *m_Driver;

    STATUS API_CALL API_Sleep(CTask *task, int seconds) {
	return m_Driver->API_Sleep(task, seconds);
    }

    STATUS API_CALL API_SleepEx( CTask *task,int miliseconds) {
	return m_Driver->API_SleepEx(task, miliseconds);
    }
};



//-----------------------------------------------------------------------------
Sleep_Service * Sleep_Service::Create()
{
    if (sleepSvcInstance == NULL) {
	NEW_RETURN_ (sleepSvcInstance, Sleep_Service);
    }
    return sleepSvcInstance;
}


//-----------------------------------------------------------------------------
Sleep_Service::Sleep_Service()
 : ServiceProvider(SLEEPDRV, true, true, Normal_Priority)
{
#ifndef __linux__
    sleepTrace = KLog::Instance()->CreateTrace ("SLEEP", "sleep-events", "Trace Sleep() API events");
#else 
    //printf(" SLEEP, sleep-events, Trace Sleep API events \n");
#endif
   
}

Sleep_Service::~Sleep_Service()
{
    KLog::Instance()->DestroyTrace(sleepTrace);
}


//-----------------------------------------------------------------------------
bool Sleep_Service::Load()
{
    m_TLS = Kernel::Instance()->AllocTLS();
    NEW_RETURN_ (m_SleepAPI, Sleep_API(SLEEP_PROVIDER_ID));
    if (m_SleepAPI == NULL) {
	Kernel::Instance()->FreeTLS(m_TLS);
	return false;
    }
    return m_SleepAPI->Init(this) == OK ? true : false;
}


//-----------------------------------------------------------------------------
bool Sleep_Service::Unload()
{
    Kernel::Instance()->FreeTLS( m_TLS );
    STATUS ret = m_SleepAPI->Shutdown();
    delete m_SleepAPI;
    return ret == OK ? true : false;
}


//-----------------------------------------------------------------------------
/** This function will be called whenever a new task is created.
 */
bool Sleep_Service::AttachTask(CTask *task)
{
    Sleep_State *ss = NULL;
    NEW_RETURN_ (ss, Sleep_State(task));
    if (!ss)
	return false;

    task->SetTLSValue(m_TLS, (int)ss);
    return true;
}


//-----------------------------------------------------------------------------
/** This function will be called whenever a task is about to be deleted.
 */
void Sleep_Service::DetachTask(CTask *task)
{
    Sleep_State *state = (Sleep_State*) task->GetTLSValue(m_TLS);
    assert (state && state->task==task);
    if (!state) return;

    if (state->active) {
	SleepStateList_T::iterator it, end=m_SleepTask.end();
	it = FindState (state, task, false);

	if (it != end) {
	    // To make sure that the FindState() works.
	    Sleep_State *found_state = *it;
	    assert (found_state->task == task);

	    CT_TRACE ( sleepTrace, "aborting sleep for task %u", task->GetPID());
	    m_SleepTask.erase(it);
	    task->Wakeup();
	}
    }

    delete state;
    task->SetTLSValue (m_TLS, 0);
}


//-----------------------------------------------------------------------------
// Scan the list for tasks to be woken up.
// The list of task's state is time-sorted.
//
void Sleep_Service::HandleEvents()
{
    static Time_Value now;

    if ( m_SleepTask.empty())
	return;

    Time_Value::get(&now);

    while (m_SleepTask.size()) {
	Sleep_State *state = m_SleepTask.back();
	if (state->wakeupTime <= now) {
	    CTask *task = state->task;
	    m_SleepTask.pop_back();
	    if(task && task->IsValid()) {
	    CT_TRACE ( sleepTrace, "waking up task %u", task->GetPID());
	    task->Wakeup();
	    }
	    state->active = false;
	} else
	    break;
    }
}


//-----------------------------------------------------------------------------
bool Sleep_Service::CancelJob(CTask *task)
{
    Sleep_State *state = (Sleep_State*) task->GetTLSValue(m_TLS);
    assert (state && state->task==task && state->active);

    SleepStateList_T::iterator it, end = m_SleepTask.end();
    it = FindState (state, task, true);
    if (it != end) {
	CT_TRACE ( sleepTrace, "aborting sleep for task %u", task->GetPID());
	m_SleepTask.erase(it);
	task->Wakeup();
    }
    return true;
}


//-----------------------------------------------------------------------------
STATUS Sleep_Service::API_SleepEx(CTask *task, int miliseconds)
{
    Sleep_State *state = (Sleep_State*) task->GetTLSValue(m_TLS);
    assert (state && state->task==task);

    Time_Value::get(&state->wakeupTime);
    if (miliseconds > 0)
	state->wakeupTime += Time_Value(miliseconds/1000, (miliseconds%1000) * 1000);
    state->active = true;

    // insert to the list sorted!
    SleepStateList_T::iterator pos;
    pos = std::lower_bound (m_SleepTask.begin(), m_SleepTask.end(), state, Sleep_State_Later());
    m_SleepTask.insert(pos, state);
    CT_TRACE ( sleepTrace, "task %u sleep for %u:%umsec",
			   task->GetPID(), miliseconds/1000, miliseconds%1000);
    task->Sleep(this);

    return OK;
}


//-----------------------------------------------------------------------------
STATUS Sleep_Service::API_Sleep(CTask *task, int seconds)
{
    if (seconds > INT_MAX/1000) {
	/* Interval truncated so that the miliseconds value it not negative.
	   The resulted value should be large enough to be useful anyway!
	 */
	seconds /= 1000;
    }

    return API_SleepEx(task, seconds * 1000);
}


//-----------------------------------------------------------------------------
Sleep_API::Sleep_API(int id)
: CAPIProvider(id, SLEEPDRV)
{
    m_Driver=0;
}

//-----------------------------------------------------------------------------
STATUS Sleep_API::Init(Sleep_Service *d)
{
    if (API_Provider_Manager::Instance()->RegisterProvider(this) != OK)
	return NOT_OK;

    m_Driver = d;

    if (RegisterAPI( "Sleep", 0, true, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &Sleep_API::API_Sleep, CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "seconds", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }
    if (RegisterAPI( "SleepEx", 1, true, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &Sleep_API::API_SleepEx, CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "milliseconds", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    return OK;
}


//-----------------------------------------------------------------------------
STATUS Sleep_API::Shutdown()
{
    return API_Provider_Manager::Instance()->UnregisterProvider(this);
}


