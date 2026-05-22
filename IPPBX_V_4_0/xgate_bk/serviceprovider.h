/*----------------------------------------------------------------* 
 * $Archive: /xGate/serviceprovider.h $
 * $Date: 4/04/02 3:35p $
 * $Revision: 21 $
 *
 * $History: serviceprovider.h $
 * 
 * *****************  Version 21  *****************
 * User: Bennylp      Date: 4/04/02    Time: 3:35p
 * Updated in $/xGate
 * Changed High_Res_Timer to highest_Res_Timer, which uses
 * CPU_Timestamp_Clock_Timer.
 * 
 * *****************  Version 20  *****************
 * User: Bennylp      Date: 10/19/01   Time: 2:18p
 * Updated in $/xGate
 * Fixed bug in attach/detach timer
 * 
 * *****************  Version 19  *****************
 * User: Bennylp      Date: 10/19/01   Time: 11:46a
 * Updated in $/xGate
 * More detail info in attach/detach times in show times.
 * 
 * *****************  Version 18  *****************
 * User: Bennylp      Date: 10/18/01   Time: 9:01p
 * Updated in $/xGate
 * Added attach times in show times
 * 
 * *****************  Version 17  *****************
 * User: Bennylp      Date: 10/09/01   Time: 6:55p
 * Updated in $/xGate
 * Reset times counters on clear counter command
 * 
 * *****************  Version 16  *****************
 * User: Bennylp      Date: 7/23/01    Time: 12:12p
 * Updated in $/xGate
 * Added framework for clearing counters.
 * 
 * *****************  Version 15  *****************
 * User: Bennylp      Date: 6/06/01    Time: 8:00p
 * Updated in $/xGate
 * Added regex selection to show service/interface command.
 * 
 * *****************  Version 14  *****************
 * User: Junanto      Date: 6/06/01    Time: 15:54
 * Updated in $/xGate
 * OnShutdown has a new parameter which indicates
 * that a requested shutdown has been cancelled
 * 
 * *****************  Version 13  *****************
 * User: Junanto      Date: 3/06/01    Time: 21:03
 * Updated in $/xGate
 * Add OnShutdown handler which is called when Kernel
 * receives shutdown request
 * 
 * *****************  Version 12  *****************
 * User: Bennylp      Date: 5/30/01    Time: 3:32p
 * Updated in $/xGate
 * Changed priority values.
 * 
 * *****************  Version 11  *****************
 * User: Bennylp      Date: 5/10/01    Time: 12:51p
 * Updated in $/xGate
 * Temporary checkin
 * 
 * *****************  Version 10  *****************
 * User: Junanto      Date: 7/05/01    Time: 19:02
 * Updated in $/xGate
 * Added CommitConfiguration after configuration
 * file is loaded
 * 
 * *****************  Version 9  *****************
 * User: Bennylp      Date: 5/03/01    Time: 4:44p
 * Updated in $/xGate
 * Improve show times output.
 * 
 * *****************  Version 8  *****************
 * User: Bennylp      Date: 5/03/01    Time: 11:52a
 * Updated in $/xGate
 * Improve profiling calculations and outputs.
 * 
 * *****************  Version 7  *****************
 * User: Bennylp      Date: 4/28/01    Time: 6:33a
 * Updated in $/xGate
 * Added service profiling and database driver (testing)
 * 
 * *****************  Version 6  *****************
 * User: Bennylp      Date: 4/23/01    Time: 10:44p
 * Updated in $/xGate
 * Fixed std namespace conflicts.
 * 
 * *****************  Version 5  *****************
 * User: Bennylp      Date: 4/13/01    Time: 6:21p
 * Updated in $/ct15
 * Change Priority enum. It does not make sense to have priorities 
 * higher than normal.
 * 
 * *****************  Version 4  *****************
 * User: Bennylp      Date: 4/12/01    Time: 8:08p
 * Updated in $/ct15
 * Integration.
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
 * User: Junanto      Date: 12/03/01   Time: 22:22
 * Created in $/ct2
 * Created
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 *  ServiceProvider is the root class for all Service Providers
 *----------------------------------------------------------------*/

#if !defined(CT2_SERVICEPROVIDER_H__INCLUDED_)
#define CT2_SERVICEPROVIDER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTask;


#include "termcmd.h"
#include "hrtimer.h"		// high resolution timer for SP profiling

#define SHOW_STATUS_INDENT  "  "

#ifndef XGATE_HAS_SIP
//static String NullString = "";
//static String Nullstring = ""; //linux porting
#endif
static String Nullstring = ""; //linux porting


///////////////////////////////////////////////////////////////////////////////
/** ServiceProvider class.
    All CTENGINE components should be derived from this class.

    At minimum, descendant needs to implement this functions:
      - Load()
      - Unload()

    If the SP want's to be notified when tasks created/destroyed, it must
    specify this in the constructor, and implements:
      - AttachTask()
      - DetachTask()

    If the SP want's to be polled by the framework, it must specify this
    in the constructor, and implements:
      - HandleEvents()

    For interface to manipulate configuration/commands, see class Term_Cmd_Target.
 */
class ServiceProvider : public Term_Cmd_Target {
public:
    /** Service Provider information class: to support individual show service
     */
    enum Information_Class {
	IC_Generic,
	IC_DB_Connection_Profile,
	IC_Signalling,
	IC_HTTP,
    };

    /** Get the name of this service provider.
     */
    const String & Name() const {
	return serviceName;
    }

    /** Return the information class of this service provider
     */
    Information_Class GetInformationClass() {
	return information_class;
    }

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load() = 0;

    /** This function will be called by the service framework once the 
	configuration file has been fully read.
     */
    virtual bool CommitConfiguration();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload() = 0;

    /** This function will be called whenever a new task is created.
     */
    virtual bool AttachTask(CTask *) {
	assert(false);
	return false;
    }

    /** This function will be called whenever a task is about to be deleted.
     */
    virtual void DetachTask(CTask *) {
	assert(false);
    }

    /** This function will be called if this instance has a pending job for
	the task.
     */
    virtual bool CancelJob(CTask *) {
	assert(false);
	return false;
    }

    /** HandleEvents: Descendent shall implement this abstract method.
	It is called by Service Provider Manager
     */
    virtual void HandleEvents() {
	assert(false);
    }

    /** OnShutdown: This method is called when the Kernel receives
	an instruction to shutdown. ServiceProvider can implement a specific
	behaviour that reflect this changes such as stop receiving further 
	calls, inform the network to stop sending further calls etc
     */
    virtual void OnShutdown(bool cancel) {
	// by default, do nothing
    }

    /** OnClearCounter is called by the framework to clear any statistic values
	maintained by this service provider.
     */
    virtual void OnClearCounter() {
	// by default, do nothing.
    }

    /** Show service provider's status.
     */
    bool ShowStatus(String & output);

    /** Get priority string.
     */
    const char *GetPriorityString() const;

//protected:	//linux porting
    /** Priority: determine the poll frequency.
     */
    enum Priority {
	High_Priority=0,	// HandleEvents() is called once per poll
	Normal_Priority=1,	// HandleEvents() is called once per ..
	Lower_Priority=2,	// HandleEvents() is called once per ..
	Low_Priority=4,		// HandleEvents() is called once per ..
	Very_Low_Priority=6,	// HandleEvents() is called once per ..
	Lowest_Priority=8,	// HandleEvents() is called once per ..
    };

    /** Constructor.
     */
    ServiceProvider(
	const String & name,		// SP name
	bool attach_to_task,		// call AttachTask() for all new tasks?
	bool always_poll,		// automaticly polled? or manually via SPManager->RegisterPoll()
	Priority prio = Normal_Priority,// polling priority/frequency
	//const String & desc = NullString,   // SP description
	const String & desc = Nullstring,   // SP description // linux porting
	Information_Class inf_class = IC_Generic
    );

    /** Destructor
     */
    virtual ~ServiceProvider();

    /** This function will be called by ServiceProviderManager to display 
	informations about service provider's status.
	This function is called with the system lock acquired, so descendants
	doesn't need to call Kernel::AcquireSystemLock() again.
     */
    virtual bool OnShowStatus(String & output) {
	return false;
    }


private:
    // Service name, should not be changed afterwards, because it is used
    // as the key in the ServiceProviderManager
    const String serviceName;

    String	serviceDescription;
    Priority	priority;
    bool	attachToTask;
    bool	periodicPoll;
    Information_Class information_class;


    /** Service profile time.
     */
    Time_Value  attachTime;	    // time to attach tasks
    Time_Value  detachTime;	    // time to detach tasks
    Time_Value  pollTime;	    // time in HandleEvents.
    
    friend class ServiceProviderManager;
    friend struct Greater_Time;
    friend struct Get_Total_Attach;
    friend struct Get_Total_Detach;
};


///////////////////////////////////////////////////////////////////////////////
/** ServiceProviderManager class
    This singleton class manages all active ServiceProvider instances
 */
class ServiceProviderManager
{
public:
    /** Create a singleton instance of ServiceProviderManager.
     */
    static ServiceProviderManager * Create();

    /** Destroy the singleton instance of ServiceProviderManager.
     */
    static void Destroy();

    /** Returns the only instance of ServiceProviderManager. 
	The service manager must have been instantiated with Create() prior
	to calling this function.
    */
    static ServiceProviderManager *Instance() {
	return theInstance;
    }

    /** Call Load() for all registered ServiceProviders.
     */
    bool Load();

    /** Call CommitConfiguration() for all registered ServiceProviders.
     */
    bool CommitConfiguration();

    /** Call Unload() for all registered ServiceProviders.
     */
    bool Unload();

    /** Unregister and destroy all registered ServiceProviders.
     */
    void Unregister();

    /** HandleEvents: It calls HandleEvents() method of all active 
	ServiceProvider. This method is continuosly called by Kernel
     */
    void HandleEvents();

    /** OnShutdown: This method is called when the Kernel receives
	an instruction to shutdown. ServiceProvider can implement a specific
	behaviour that reflect this changes such as stop receiving further 
	calls, inform the network to stop sending further calls etc
     */
    void OnShutdown(bool cancel);

    /** Called by Kernel whenever a new task is born.
     */
    bool AttachTask ( CTask * );

    /** Called by Kernel whenever a task is about to be destroyed.
     */
    void DetachTask ( CTask * );

    /** InsertServiceProvider: It adds a new ServiceProvider
	to _listServiceProvider. Called by constructor of
	ServiceProvider class
     */
    void InsertServiceProvider(ServiceProvider *sp, bool attach_to_task);

    /** RemoveServiceProvider: It removes a ServiceProvider
	from _listServiceProvider. Called by destructor of
	ServiceProvider class
     */
    void RemoveServiceProvider(ServiceProvider *sp);

    /** Register a ServiceProvider to the list of SP's to be polled.
     */
    void RegisterPoll (ServiceProvider *sp);

    /** Unregister a ServiceProvider to the list of SP's to be polled.
     */
    void UnregisterPoll (ServiceProvider *sp);

    /** Enable/disable service provider profiling.
     */
    void SetProfilingEnabled ( bool enabled=true );

    /** Determine whether service provider profiling is enabled.
     */
    bool IsProfilingEnabled() const {
	return isProfilingEnabled;
    }

    /** Get total time spent in HandleEvents.
     */
    const Time_Value & GetServiceTime() const {
	return serviceTime;
    }

    /** Get total time spent in HandleEvents.
     */
    Time_Value & GetServiceTime() {
	return serviceTime;
    }

    /** Show profiling results.
     */
    void ShowProfile ( Term_Session * session );

    /** Show all service provider status.
     */
    void ShowStatus (Term_Session * session, const String & selector );

    /** Clear counter.
     */
    void ClearCounter (Term_Session *session, const String & selector);

    /** Get list of service provider names (for validator).
     */
    const List<ValidatorChoice> & GetValidatorChoiceList() const {
	return spValidatorChoiceList;
    }

private:
    /** The singleton instance of ServiceProviderManager.
     */
    static ServiceProviderManager *theInstance;

    typedef List<ServiceProvider*> ListServiceProvider;

    /** The number of times OnShutdown is called
     */
    int _shutdownCount;

    /** List of all service providers.
     */
    ListServiceProvider spAllList;

    /** ServiceProviders that needs to be attached to task.
     */
    ListServiceProvider spAttachList;

    /** ServiceProvides that needs to be polled.
     */
    ListServiceProvider spPollList;

    /** Flag to indicate whether profiling is enabled.
     */
    bool isProfilingEnabled;

    /** Get total time spent in HandleEvents.
     */
    Time_Value serviceTime;

    /** High resolution timer for SP profiling.
     */
    Highest_Res_Timer profTimer;
    Highest_Res_Timer attachTimer;

    /** Last priority polled.
     */
    ServiceProvider::Priority lastPriority;

    /** List of service provider names (for CVariantValidator)
     */
    List<ValidatorChoice> spValidatorChoiceList;

    /** Constructor: ServiceProviderManager is a singleton that is 
	automatically created by the first call to GetInstance.
     */
    ServiceProviderManager();

    /** Destructor
     */
    ~ServiceProviderManager();
};


///////////////////////////////////////////////////////////////////////////////
/** This class is used as the validator for service provider names.
    It will show the list of service provider names as a choice list, but it
    also accepts any string given by user.
 */
class ServiceProvider_DynamicValidator : public CVariantDynamicChoiceValidator {
public:
    ServiceProvider_DynamicValidator() {}

    /** This will always return true.
	ServiceProviderManager will take care of the names.
     */
    virtual bool Validate(const CVariant&) const {
	return true;
    }

    /** This will always return true.
	ServiceProviderManager will take care of the names.
     */
    virtual bool Suggest(CVariant&) const {
	return true;
    }

    /** Acquire mutex lock.
	This function will be called by the framework BEFORE calling
	GetChoiceList(). The lock will be kept until this class is finished
	processing all the choices in the choice list.
     */
    virtual void AcquireLock() const;

    /** Release mutex lock.
	This function will be called by the framework AFTER calling
	GetChoiceList().
     */
    virtual void ReleaseLock() const;

    /** Get list of choices available.
     */
    virtual const List<ValidatorChoice> & GetChoiceList() const {
	return ServiceProviderManager::Instance()->GetValidatorChoiceList();
    }

    /** Create a duplicate */
    virtual CVariantValidator *Clone() const {
	return new ServiceProvider_DynamicValidator;
    }
};


#endif // !defined(CT2_SERVICEPROVIDER_H__INCLUDED_)
