/*----------------------------------------------------------------* 
 * $Archive: /xGate/serviceprovider.cpp $
 * $Date: 10/19/01 2:18p $
 * $Revision: 32 $
 *
 * $History: serviceprovider.cpp $
 * 
 * *****************  Version 32  *****************
 * User: Bennylp      Date: 10/19/01   Time: 2:18p
 * Updated in $/xGate
 * Fixed bug in attach/detach timer
 * 
 * *****************  Version 31  *****************
 * User: Bennylp      Date: 10/19/01   Time: 12:13p
 * Updated in $/xGate
 * Clear attach/detach counter
 * 
 * *****************  Version 30  *****************
 * User: Bennylp      Date: 10/19/01   Time: 11:46a
 * Updated in $/xGate
 * More detail info in attach/detach times in show times.
 * 
 * *****************  Version 29  *****************
 * User: Bennylp      Date: 10/18/01   Time: 9:01p
 * Updated in $/xGate
 * Added attach times in show times
 * 
 * *****************  Version 28  *****************
 * User: Bennylp      Date: 10/09/01   Time: 6:55p
 * Updated in $/xGate
 * Reset times counters on clear counter command
 * 
 * *****************  Version 27  *****************
 * User: Junanto      Date: 5/10/01    Time: 11:08
 * Updated in $/xGate
 * Widened service name column in "show times"
 * 
 * *****************  Version 26  *****************
 * User: Bennylp      Date: 7/23/01    Time: 1:25p
 * Updated in $/xGate
 * Added confirmation if clear counter is applied to all counters.
 * 
 * *****************  Version 25  *****************
 * User: Bennylp      Date: 7/23/01    Time: 12:12p
 * Updated in $/xGate
 * Added framework for clearing counters.
 * 
 * *****************  Version 24  *****************
 * User: Bennylp      Date: 6/14/01    Time: 7:43p
 * Updated in $/xGate
 * Show info when Loading, Unloading, and CommitConfig.
 * 
 * *****************  Version 23  *****************
 * User: Bennylp      Date: 6/06/01    Time: 8:00p
 * Updated in $/xGate
 * Added regex selection to show service/interface command.
 * 
 * *****************  Version 22  *****************
 * User: Junanto      Date: 6/06/01    Time: 15:54
 * Updated in $/xGate
 * OnShutdown has a new parameter which indicates
 * that a requested shutdown has been cancelled
 * 
 * *****************  Version 21  *****************
 * User: Junanto      Date: 3/06/01    Time: 21:03
 * Updated in $/xGate
 * Add OnShutdown handler which is called when Kernel
 * receives shutdown request
 * 
 * *****************  Version 20  *****************
 * User: Bennylp      Date: 5/29/01    Time: 4:44p
* Updated in $/xGate
* Added log and system lock info in unloading phase.
* 
* *****************  Version 19  *****************
* User: Bennylp      Date: 5/28/01    Time: 6:11p
* Updated in $/xGate
* More info to system locking and handle unreleased recursive lock.
* 
* *****************  Version 18  *****************
* User: Bennylp      Date: 5/26/01    Time: 8:04p
* Updated in $/xGate
* Record each call to AcquireSystemLock in DEBUG mode.
* 
* *****************  Version 17  *****************
* User: Bennylp      Date: 5/21/01    Time: 12:12p
* Updated in $/xGate
* Rollback SIP changes to stable version.
* 
* *****************  Version 16  *****************
* User: Junanto      Date: 10/05/01   Time: 13:15
* Updated in $/xGate
* Replaced "\r\n" with "\n"
* 
* *****************  Version 15  *****************
* User: Bennylp      Date: 5/10/01    Time: 12:51p
* Updated in $/xGate
* Temporary checkin
* 
* *****************  Version 14  *****************
* User: Junanto      Date: 7/05/01    Time: 19:02
* Updated in $/xGate
* Added CommitConfiguration after configuration
* file is loaded
* 
* *****************  Version 13  *****************
* User: Bennylp      Date: 5/03/01    Time: 4:44p
* Updated in $/xGate
* Improve show times output.
* 
* *****************  Version 12  *****************
* User: Bennylp      Date: 5/03/01    Time: 1:09p
* Updated in $/xGate
* Changed profiling to 'show times', and more improvements.
* 
* *****************  Version 11  *****************
* User: Bennylp      Date: 5/03/01    Time: 11:52a
* Updated in $/xGate
* Improve profiling calculations and outputs.
* 
* *****************  Version 10  *****************
* User: Bennylp      Date: 4/30/01    Time: 7:44a
* Updated in $/xGate
* Added percentage total in svc profiling.
* 
* *****************  Version 9  *****************
* User: Bennylp      Date: 4/28/01    Time: 6:33a
	* Updated in $/xGate
* Added service profiling and database driver (testing)
	* 
	* *****************  Version 8  *****************
	* User: Bennylp      Date: 4/23/01    Time: 10:44p
	* Updated in $/xGate
	* Fixed std namespace conflicts.
	* 
	* *****************  Version 7  *****************
	* User: Bennylp      Date: 4/18/01    Time: 3:41p
	* Updated in $/ct15
	* Write log when Load() failed.
	* 
	* *****************  Version 6  *****************
	* User: Junanto      Date: 13/04/01   Time: 18:01
	* Updated in $/ct15
	* 
	* *****************  Version 5  *****************
	* User: Bennylp      Date: 4/12/01    Time: 9:00p
	* Updated in $/ct15
	* Integration and some bug fixes.
	* 
	* *****************  Version 4  *****************
	* User: Junanto      Date: 11/04/01   Time: 11:37
	* Updated in $/ct15
	* 
	* *****************  Version 3  *****************
	* User: Bennylp      Date: 4/10/01    Time: 11:38a
	* Updated in $/ct15
	* Intermediate checkin for integration with others.
	* 
	* *****************  Version 2  *****************
	* User: Bennylp      Date: 3/29/01    Time: 3:11p
	* Updated in $/ct15
	* Clean compile.
	* 
	* *****************  Version 1  *****************
	* User: Bennylp      Date: 3/22/01    Time: 4:31p
	* Created in $/ct15
	* 
	* *****************  Version 3  *****************
	* User: Bennylp      Date: 3/22/01    Time: 11:01a
	* Updated in $/ct2
	* Temporary checkin because files are needed by other project.
	* 
	* *****************  Version 2  *****************
	* User: Junanto      Date: 15/03/01   Time: 17:20
	* Updated in $/ct2
	* 
	* *****************  Version 1  *****************
	* User: Junanto      Date: 12/03/01   Time: 22:21
	* Created in $/ct2
	* Created
	* 
	*----------------------------------------------------------------*/

	/*----------------------------------------------------------------* 
	 * Description:
	 *  
	 *----------------------------------------------------------------*/

	//#include "stdafx.h"
#include "serviceprovider.h"
#include "dbdriver.h"
	//#include <Isup.h>
#include "log.h"
#include "kernel.h"
#include "term.h"
//#include "pcre-3.4/pcreposix.h"	// Regex
#include <regex.h>

#include <numeric>	// std::accumulate
#define __linux__

/* singleton instance of ServiceProviderManager
*/
ServiceProviderManager *ServiceProviderManager::theInstance;


/* When kernel lock debugging is enabled, this will contain the name of 
	   current service provider being polled.
	   */
#ifdef KERNEL_RECORD_ACQUIRE_LOCK
	static char currentSpPolled[64];
#endif


	/*----------------------------------------------------------------*/

ServiceProvider::ServiceProvider(const String & name, 
		bool attach_to_task, 
		bool always_poll, 
		Priority prio, 
		const String & desc,
		Information_Class inf_class) 
: serviceName(name), serviceDescription(desc), priority(prio), 
	attachToTask(attach_to_task), periodicPoll(always_poll),
	information_class(inf_class)
{
	memset (&pollTime, 0, sizeof(pollTime));
	memset (&attachTime, 0, sizeof(attachTime));
	memset (&detachTime, 0, sizeof(detachTime));

	ServiceProviderManager *mgr = ServiceProviderManager::Instance();
	assert(mgr);

	mgr->InsertServiceProvider(this, attach_to_task);
	if (always_poll) mgr->RegisterPoll (this);
}


ServiceProvider::~ServiceProvider()
{
	ServiceProviderManager *mgr = ServiceProviderManager::Instance();
	assert(mgr);

	mgr->RemoveServiceProvider(this);
}


/** Get priority string.
*/
const char *ServiceProvider::GetPriorityString() const
{
	switch(priority) {
		case High_Priority: return "high";
		case Normal_Priority: return "normal";
		case Lower_Priority:  return "lower";
		case Low_Priority:	  return "low";
		case Very_Low_Priority: return "verylow";
		case Lowest_Priority: return "lowest";
	}
	return "unknown";
}

/** This function will be called by the service framework once the 
  configuration file has been fully read.
  */
bool ServiceProvider::CommitConfiguration()
{
	// default, return true
	return true;
}


/** Show service provider's status.
*/
bool ServiceProvider::ShowStatus(String & output)
{
	output += "Service " + serviceName;
	if (periodicPoll) {
		output += ", priority ";
		output += GetPriorityString();
	}
	output += "\n";
	return OnShowStatus(output);
}


///////////////////////////////////////////////////////////////////////////////

/** Acquire mutex lock.
  This function will be called by the framework BEFORE calling
  GetChoiceList(). The lock will be kept until this class is finished
  processing all the choices in the choice list.
  */
void ServiceProvider_DynamicValidator::AcquireLock() const 
{
	ACQUIRE_SYSTEM_LOCK();
}

/** Release mutex lock.
  This function will be called by the framework AFTER calling
  GetChoiceList().
  */
void ServiceProvider_DynamicValidator::ReleaseLock() const 
{
	RELEASE_SYSTEM_LOCK();
}



///////////////////////////////////////////////////////////////////////////////

	ServiceProviderManager::ServiceProviderManager()
: isProfilingEnabled(true), lastPriority(ServiceProvider::Lowest_Priority), _shutdownCount(0)
{
	// Special choices are inserted to the choice list to help user.
	spValidatorChoiceList.push_front(ValidatorChoice("<Regular expression>", ""));
}


ServiceProviderManager::~ServiceProviderManager()
{
	theInstance = NULL;
}


/** Create a singleton instance of ServiceProviderManager.
*/
ServiceProviderManager * ServiceProviderManager::Create()
{
	assert(theInstance==NULL);
	if (theInstance==NULL) {
		theInstance = new ServiceProviderManager;
	}
	return theInstance;
}

/** Destroy the singleton instance of ServiceProviderManager.
*/
void ServiceProviderManager::Destroy() 
{
	if (theInstance != NULL) {
		delete theInstance;
	}
}

/** Call Load() for all registered ServiceProviders.
*/
bool ServiceProviderManager::Load()
{
	bool status = true;
	ListServiceProvider::iterator it, end;

	end = spAllList.end();

	for (it = spAllList.begin(); it!=end; ++it) {
		ServiceProvider *sp = *it;

#ifdef KERNEL_RECORD_ACQUIRE_LOCK
		atomic_set (&Kernel::Instance()->currentService, sp->Name().c_str());
#endif

	//	CLog::Detail ( "SYS", "Loading service %s...", sp->Name().c_str());
		//printf("Loading service %s...\n", sp->Name().c_str());
		//ACE_OS::sleep(ACE_Time_Value(0, 10000));  // Sleep() to make sure log is written.

		if (sp->Load() == false) {
			status = false;
			CLog::Error ( "SYS", "Service provider %s initialization error", sp->Name().c_str());

			//printf("Service provider %s initialization error...",sp->Name().c_str());
		}

	//	CLog::Detail ( "SYS", "Finished loading service %s...", sp->Name().c_str());
		//ACE_OS::sleep(ACE_Time_Value(0, 10000));  // Sleep() to make sure log is written.
		//printf(" Finished loading service %s...", sp->Name().c_str());
	}

#ifdef KERNEL_RECORD_ACQUIRE_LOCK
	atomic_set (&Kernel::Instance()->currentService, "");
#endif

	return status;
}

/** Call CommitConfiguration() for all registered ServiceProviders.
*/
bool ServiceProviderManager::CommitConfiguration()
{
	bool status = true;
	ListServiceProvider::iterator it, end;
	end = spAllList.end();
	for (it = spAllList.begin(); it!=end; ++it) {
		ServiceProvider *sp = *it;
#ifdef KERNEL_RECORD_ACQUIRE_LOCK
		atomic_set (&Kernel::Instance()->currentService, sp->Name().c_str());
#endif
		CLog::Detail ( "SYS", "Committing config on %s...", sp->Name().c_str());
		//printf("Committing config on %s...", sp->Name().c_str());
		/* ACE_OS::sleep(ACE_Time_Value(0, 10000));  // Sleep() to make sure log is written.
		*/

		if (sp->CommitConfiguration() == false) {
			status = false;
			ServiceProvider *sp = *it;
			CLog::Error ( "SYS", "Service provider %s initialization error", sp->Name().c_str());
			//printf("Service provider %s initialization error %s...", sp->Name().c_str());
		}

		CLog::Detail ( "SYS", "Finished committing config on %s...", sp->Name().c_str());

		//printf("Finished committing config on %s...", sp->Name().c_str());
	}

#ifndef __linux__
#ifdef KERNEL_RECORD_ACQUIRE_LOCK
	atomic_set (&Kernel::Instance()->currentService, "");
#endif
#endif

	return status;
}


/** Call Unload() for all registered ServiceProviders.
*/
bool ServiceProviderManager::Unload()
{
	bool status = true;
	ListServiceProvider::reverse_iterator it, end;

	end = spAllList.rend();
	for (it = spAllList.rbegin(); it!=end; ++it) {
		ServiceProvider *sp = *it;

		//CLog::Info ( "SYS", "Unloading service %s...", sp->Name().c_str());
		//ACE_OS::sleep(ACE_Time_Value(0, 20000));  // Sleep() to make sure log is written.

#ifndef __linux__
#ifdef KERNEL_RECORD_ACQUIRE_LOCK
		atomic_set (&Kernel::Instance()->currentService, sp->Name().c_str());
#endif
#endif
		if (sp->Unload() == false)
			status = false;

		//CLog::Info ( "SYS", "Finished unloading service %s.", sp->Name().c_str());
		//ACE_OS::sleep(ACE_Time_Value(0, 10000));  // Sleep() to make sure log is written.
	}

#ifndef __linux__
#ifdef KERNEL_RECORD_ACQUIRE_LOCK
	atomic_set (&Kernel::Instance()->currentService, "");
#endif
#endif
	return status;
}

/** Unregister and destroy all registered ServiceProviders.
*/
void ServiceProviderManager::Unregister()
{
	while (spAllList.size()) {
		delete spAllList.back();
	}

	// All lists should be empty by now, because SP's destructor should automatically
	// unregister itself.
	assert(spAllList.empty());
	assert(spPollList.empty());
	assert(spAttachList.empty());

	// To be safe, clear all list.
	spAllList.clear();
	spPollList.clear();
	spAttachList.clear();
}

/** HandleEvents: It calls HandleEvents() method of all active 
  ServiceProvider. This method is continuosly called by Kernel
  */
void ServiceProviderManager::HandleEvents()
{
	Time_Value elapsed;
	ListServiceProvider::iterator it, end;
	end = spPollList.end();

	for (it=spPollList.begin(); it!=end; ++it) {
		ServiceProvider *sp = *it;
		if (sp->priority > lastPriority)
			break;

#ifdef KERNEL_RECORD_ACQUIRE_LOCK
		// We can't use sp->Name().c_str() directly in atomic_set, since SP
		// can be deleted dynamicly. So we have to use the more expensive
		// strcpy.
		strcpy(currentSpPolled, sp->Name().c_str());
		atomic_set (&Kernel::Instance()->currentService, currentSpPolled);
#endif

		profTimer.start();
		sp->HandleEvents();
		profTimer.stop();

		profTimer.get_elapsed (&elapsed);
		sp->pollTime += elapsed;
		serviceTime += elapsed;
	}

	// determine next last priority to poll.
	// In this implementation, we use simple increment
	lastPriority = (ServiceProvider::Priority)(lastPriority + 1);
	if (lastPriority > ServiceProvider::Lowest_Priority)
		lastPriority = (ServiceProvider::Priority)0;
}


/** OnShutdown: This method is called when the Kernel receives
  an instruction to shutdown. ServiceProvider can implement a specific
  behaviour that reflect this changes such as stop receiving further 
  calls, inform the network to stop sending further calls etc
  */
void ServiceProviderManager::OnShutdown(bool cancel)
{
	if (++_shutdownCount > 1)
		CLog::Warning("SYS", "Shutdown has been requested %d times", _shutdownCount);

	ListServiceProvider::iterator end = spAllList.end();
	for (ListServiceProvider::iterator it=spAllList.begin(); it!=end; ++it) {
		CLog::Detail("SYS", "Requesting service %s to shutdown", (*it)->Name().c_str());
		(*it)->OnShutdown(cancel);
	}
}

/** InsertServiceProvider: It adds a new ServiceProvider
  to _listServiceProvider. Called by constructor of
  ServiceProvider class
  */
void ServiceProviderManager::InsertServiceProvider(ServiceProvider *sp, bool attach_to_task)
{
	static String _NullString;	//linux porting
	spAllList.push_back(sp);
	if (attach_to_task)
		spAttachList.push_back(sp);

	// Insert SP into SP names list
	spValidatorChoiceList.push_back(ValidatorChoice(sp->Name().c_str(), _NullString));

}


/** RemoveServiceProvider: It removes a ServiceProvider
  from _listServiceProvider. Called by destructor of
  ServiceProvider class
  */
void ServiceProviderManager::RemoveServiceProvider(ServiceProvider *sp)
{
	spAllList.remove (sp);
	spPollList.remove (sp);
	spAttachList.remove (sp);

	// Remove SP from SP names list
	List<ValidatorChoice>::iterator it, end;
	end = spValidatorChoiceList.end();
	CVariant vname(sp->Name().c_str());
	for (it=spValidatorChoiceList.begin(); it!=end; ++it) {
		ValidatorChoice & choice = *it;
		if (vname == choice.first) {
			spValidatorChoiceList.erase(it);
			break;
		}
	}

	assert(it!=end);
}

/** Called by Kernel whenever a new task is born.
*/
bool ServiceProviderManager::AttachTask ( CTask * task)
{
	ListServiceProvider tempAttachedList;
	ListServiceProvider::iterator it, end;

	Time_Value elapsed;

	end = spAttachList.end();
	for (it = spAttachList.begin(); it!=end; ++it) {
		ServiceProvider *sp = *it;

		char where[128];
		sprintf(where, "ServiceProviderManager::AttachTask::%s", sp->Name().c_str());
		atomic_set (&Kernel::Instance()->currentService, where);

		attachTimer.start();

		if (sp->AttachTask(task) == false) {
			attachTimer.stop();
			attachTimer.get_elapsed(&elapsed);
			sp->attachTime += elapsed;
			break;
		} else
			tempAttachedList.push_back (sp);

		attachTimer.stop();
		attachTimer.get_elapsed(&elapsed);
		sp->attachTime += elapsed;
	}

	if (it != end) {
		end = tempAttachedList.end();
		for (it = tempAttachedList.begin(); it!=end; ++it) {
			ServiceProvider *sp = *it;

			attachTimer.start();

			sp->DetachTask (task);

			attachTimer.stop();
			attachTimer.get_elapsed(&elapsed);

			sp->detachTime += elapsed;
		}
		return false;
	}
	return true;
}

/** Called by Kernel whenever a task is about to be destroyed.
*/
void ServiceProviderManager::DetachTask ( CTask * task)
{
	Time_Value elapsed;
	ListServiceProvider::iterator it, end;
	end = spAttachList.end();

	for (it = spAttachList.begin(); it!=end; ++it) {
		ServiceProvider *sp = *it;

		attachTimer.start();

		sp->DetachTask (task);

		attachTimer.stop();
		attachTimer.get_elapsed(&elapsed);

		sp->detachTime += elapsed;
	}
}


/** Register a ServiceProvider to the list of SP's to be polled.
*/
void ServiceProviderManager::RegisterPoll (ServiceProvider *sp)
{
	// make sure that the SP is not already in poll list, or otherwise
	// we'll get trouble because of duplication.
	assert(std::find(spPollList.begin(), spPollList.end(), sp) == spPollList.end());
	spPollList.remove (sp);

	// Insert the SP to the poll list sorted, higher priority first.
	List <ServiceProvider*>::iterator it, end;
	end = spPollList.end();
	for (it = spPollList.begin(); it!=end; ++it) {
		ServiceProvider *prov = *it;
		if (prov->priority > sp->priority)
			break;
	}
	spPollList.insert (it, sp);
}

/** Unregister a ServiceProvider to the list of SP's to be polled.
*/
void ServiceProviderManager::UnregisterPoll (ServiceProvider *sp)
{
	assert(std::find(spPollList.begin(), spPollList.end(), sp) != spPollList.end());
	spPollList.remove (sp);
}

/** Enable/disable service provider profiling.
*/
void ServiceProviderManager::SetProfilingEnabled ( bool enabled )
{
	isProfilingEnabled = enabled;
}

// This type describes the profiling result type.
typedef std::pair <String, const char*>  ProfileDesc;
typedef std::pair <Time_Value, ProfileDesc> ProfileResult;

inline operator < (const ProfileResult & x, const ProfileResult & y)
{
	return x.first < y.first;
}

inline operator > (const ProfileResult & x, const ProfileResult & y)
{
	return x.first > y.first;
}

struct Greater_Time {
	bool operator() (const ServiceProvider *sp1, const ServiceProvider *sp2) {
		return (sp1->attachTime + sp1->detachTime) > 
			(sp2->attachTime + sp2->detachTime);
	}
};

struct Get_Total_Attach {
	Time_Value operator() (const Time_Value & val, const ServiceProvider *sp) {
		return val + sp->attachTime;
	}
};

struct Get_Total_Detach {
	Time_Value operator() (const Time_Value & val, const ServiceProvider *sp) {
		return val + sp->detachTime;
	}
};

/** Show profiling results.
*/
void ServiceProviderManager::ShowProfile ( Term_Session * session )
{
	//
	// Build results while holding system's lock.
	//

	// types
	List <ProfileResult> result_list;

	ACQUIRE_SYSTEM_LOCK();
	ListServiceProvider::const_iterator it_poll, end_poll;
	end_poll = spPollList.end();
	for (it_poll = spPollList.begin(); it_poll!=end_poll; ++it_poll) {
		ServiceProvider *sp = *it_poll;
		result_list.push_back ( ProfileResult(sp->pollTime, ProfileDesc(sp->Name(), sp->GetPriorityString())) );
	}
	double ms_kernel_total = Kernel::Instance()->GetKernelBusyTime().to_msec();
	Time_Value total = GetServiceTime();

	RELEASE_SYSTEM_LOCK();

	double ms_total = total.to_msec();
	if (ms_total == 0)
		ms_total = 1;
	if (ms_kernel_total == 0)
		ms_kernel_total = 1;

	// Sort the list.
	result_list.sort(std::greater<ProfileResult>());

	// Show the result.
	//session->Sendf ("Service provider profiling is %s\n", (IsProfilingEnabled() ? "enabled" : "disabled"));
	session->Sendf ("Total service provider time: %u sec:%06u usec (%5.2f %% system)\n", 
			total.sec(),
			total.usec() % 1000000,
			ms_total * 100 / ms_kernel_total);
	session->Sendf ("\n");
	session->Sendf (" Service name  Priority      Time elapsed          %% Svc.   %% System\n");
	session->Sendf ("=====================================================================\n");

	char temp[80];
	List <ProfileResult>::const_iterator it_result, end_result;
	end_result = result_list.end();
	for (it_result = result_list.begin(); it_result != end_result; ++it_result) {
		const ProfileResult & result = *it_result;
		const ProfileDesc & sp = result.second;
		const Time_Value & tv = result.first;
		double ms_sp_time = tv.to_msec();
		int len = sprintf (temp, "%13s  %-8s %7u sec:%06u usec   %5.2f %%    %5.2f %%\n", sp.first.c_str(), 
				sp.second,
				tv.sec(),
				tv.usec(),
				ms_sp_time * 100 / ms_total,
				ms_sp_time * 100 / ms_kernel_total);
		if (len > 0)
			session->Send ( temp, (size_t)len);
	}



	String pollStat;
	pollStat.reserve(2000);

	ACQUIRE_SYSTEM_LOCK();

	// Copy to temporary vector before sorting
	Vector<ServiceProvider*> temp_sp;
	temp_sp.resize(spAttachList.size());
	std::copy(spAttachList.begin(), spAttachList.end(), temp_sp.begin());

	// Sort based on attach and detach time
	std::sort (temp_sp.begin(), temp_sp.end(), Greater_Time());

	// Get the total attach and detach time
	Time_Value tot_attach = std::accumulate(temp_sp.begin(), temp_sp.end(), Time_Value(0,0), Get_Total_Attach());
	Time_Value tot_detach = std::accumulate(temp_sp.begin(), temp_sp.end(), Time_Value(0,0), Get_Total_Detach());
	Time_Value tot_attach_detach = tot_attach + tot_detach;
	//double tot_attach_ms = tot_attach.sec() * 1000.0 + tot_attach.msec();
	//double tot_detach_ms = tot_detach.sec() * 1000.0 + tot_detach.msec();
	double tot_attach_ms = tot_attach.msec();
	double tot_detach_ms = tot_detach.msec();


	// Avoid division by zero
	if (tot_attach_ms < 0.1)
		tot_attach_ms = 0.1;
	if (tot_detach_ms < 0.1)
		tot_detach_ms = 0.1;

	double tot_attach_detach_ms = tot_attach_ms + tot_detach_ms;

	// Display
	Vector<ServiceProvider*>::iterator it, end;
	end = temp_sp.end();
	for (it = temp_sp.begin(); it!=end; ++it) {
		ServiceProvider *sp = *it;
		Time_Value & attach = sp->attachTime;
		//double ms_pct_attach = attach.sec()*100000.0 + attach.msec();
		double ms_pct_attach = attach.msec() * 100.0;
		int len = sprintf (temp, "%16s %s %7u sec:%06u usec     %5.2f %%  %5.2f %%\n", 
				sp->Name().c_str(), 
				"init",
				attach.sec(),
				attach.usec(),
				ms_pct_attach / tot_attach_detach_ms,
				ms_pct_attach / ms_kernel_total);
		if (len > 0)
			pollStat += temp;

		Time_Value & detach = sp->detachTime;
		//double ms_pct_detach = detach.sec()*100000.0 + detach.msec();
		double ms_pct_detach = detach.msec()*100.0;
		len = sprintf (temp, "%16s %s %7u sec:%06u usec     %5.2f %%  %5.2f %%\n", 
				"", 
				"shut",
				detach.sec(),
				detach.usec(),
				ms_pct_detach / tot_attach_detach_ms,
				ms_pct_detach / ms_kernel_total);
		if (len > 0)
			pollStat += temp;
	}

	RELEASE_SYSTEM_LOCK();

	// Show the result of attach/detach times.
	session->Sendf ("\n\nTotal task init times: %u sec:%06u usec (%5.2f %% system)\n",
			tot_attach.sec(), tot_attach.usec(), tot_attach_ms * 100 / ms_kernel_total);
	session->Sendf ("Total task shutdown times: %u sec:%06u usec (%5.2f %% system)\n",
			tot_detach.sec(), tot_detach.usec(), tot_detach_ms * 100 / ms_kernel_total);
	session->Sendf ("Total task init+shutdown times: %u sec:%06u usec (%5.2f %% system)\n\n",
			tot_attach_detach.sec(), tot_attach_detach.usec(), tot_attach_detach_ms * 100 / ms_kernel_total);
	session->Sendf ("  Service Name               Time elapsed          %% Svc.   %% System\n");
	session->Sendf ("=====================================================================\n");
	session->Send(pollStat);
}


void ServiceProviderManager::ShowStatus (Term_Session * session, const String & selector)
{
	String all_output;
	const String crlf("\n");
	regex_t reg;
	int ret;
	memset (&reg, 0, sizeof(reg));
	enum ACTION {
		SHOW_SERVICE,
		SHOW_DATABASE,
		SHOW_SIGNALLING,
		SHOW_HTTP,
	};
	ACTION type;

	ret = 0;
	if (selector.empty()) {
		ret=regcomp(&reg, ".*", REG_ICASE | REG_EXTENDED);
		type = SHOW_SERVICE;
	} else if (selector == "*database*") {
		type = SHOW_DATABASE;
	} else if (selector == "*signalling*") {
		type = SHOW_SIGNALLING;
	} else if (selector == "*http*") {
		type = SHOW_HTTP;
	} else {
		ret=regcomp(&reg, selector.c_str(), REG_ICASE | REG_EXTENDED);
		type = SHOW_SERVICE;
	}

	if (ret) {
		session->SendErrorf ("Invalid regular expression '%s'\n", selector.c_str());
		return;
	}

	ListServiceProvider::iterator it, end;
	bool found = false;

	ACQUIRE_SYSTEM_LOCK();
	end = spAllList.end();
	for (it = spAllList.begin(); it!=end; ++it) {
		String output;
		ServiceProvider *sp = *it;

		if (type == SHOW_SERVICE) {
			if (regexec(&reg, sp->Name().c_str(), 0, NULL, 0) == 0) {
				if (sp->ShowStatus(output) == true) {
					all_output += output;
					all_output += crlf;
				}
				found = true;
			}
		} else if (type == SHOW_DATABASE) {
			if (sp->GetInformationClass() == ServiceProvider::IC_DB_Connection_Profile) {
				if (sp->ShowStatus(output) == true) {
					all_output += output;
					all_output += crlf;
				}
				found = true;
			}
		} else if (type == SHOW_SIGNALLING) {
			if (sp->GetInformationClass() == ServiceProvider::IC_Signalling) {
				if (sp->ShowStatus(output) == true) {
					all_output += output;
					all_output += crlf;
				}
				found = true;
			}
		} else if (type == SHOW_HTTP) {
			if (sp->GetInformationClass() == ServiceProvider::IC_HTTP) {
				if (sp->ShowStatus(output) == true) {
					all_output += output;
					all_output += crlf;
				}
				found = true;
			}
		}
	}
	RELEASE_SYSTEM_LOCK();

	if (type == SHOW_SERVICE) regfree(&reg);

	if (!found)
		session->Sendf ("No service with the specified name criteria found\n");
	else
		session->Send (all_output);
}


void ServiceProviderManager::ClearCounter (Term_Session * session, const String & selector)
{
	String all_output;
	const String crlf("\n");
	regex_t reg;
	int ret;
	memset (&reg, 0, sizeof(reg));

	if (selector.empty() || selector==".*") {
		if (!session->PromptYesNo(false, "Clear all counters [y/n] ? "))
			return;
	}

	if (selector.empty())
		ret=regcomp(&reg, ".*", REG_ICASE | REG_EXTENDED);
	else
		ret=regcomp(&reg, selector.c_str(), REG_ICASE | REG_EXTENDED);
	if (ret) {
		session->SendErrorf ("Invalid regular expression '%s'\n", selector.c_str());
		return;
	}

	ListServiceProvider::iterator it, end;

	bool found = false;
	ACQUIRE_SYSTEM_LOCK();
	end = spAllList.end();
	for (it = spAllList.begin(); it!=end; ++it) {
		String output;
		ServiceProvider *sp = *it;

		if (regexec(&reg, sp->Name().c_str(), 0, NULL, 0) == 0) {
			found = true;
			sp->pollTime.set(0, 0);
			sp->attachTime.set(0, 0);
			sp->detachTime.set(0, 0);
			sp->OnClearCounter();
		}
	}
	RELEASE_SYSTEM_LOCK();

	regfree(&reg);

	if (!found) {
		session->Sendf ("No service with the specified name criteria found\n");
	} else
		session->Send (all_output);
}

