/* $Header: /xGate/application.h 26    5/03/05 4:16p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * SL Application management.
 *
 * $Log: /xGate/application.h $
 * 
 * 26    5/03/05 4:16p Bennylp
 * Added warning if app is compiled with older compiler
 * 
 * 25    7/08/04 6:49p Bennylp
 * Call Gapping
 * 
 * 24    3/08/04 7:01p Bennylp
 * Added:
 * - Registrar API (support spawning task for incoming registration)
 * - Authentication API
 * 
 * 23    1/21/02 11:44a Bennylp
 * Ver 38.2 (configurable use of app mem pool)
 * 
 * 22    25/10/01 11:20 Junanto
 * Fixed bug in free memory pool management
 * 
 * 21    24/10/01 15:12 Junanto
 * Tasks' memory and string are no longer deleted on task
 * destruction but returned to a free pool for later use
 * 
 * 20    7/25/01 8:06p Bennylp
 * Added application tracing.
 * 
 * 19    7/17/01 11:29a Bennylp
 * Added 'show application brief | detail'
 * 
 * 18    7/14/01 7:53p Bennylp
 * Reduce friends of class Task_Exec_Context.
 * 
 * 17    13/07/01 17:22 Sonny
 * 
 * 16    7/13/01 11:38a Bennylp
 * Added command 'show cir' to display all Call In Routes.
 * 
 * 15    7/13/01 10:55a Adham
 * SNMP Framework integration
 * 
 * 14    10/07/01 19:06 Junanto
 * Changed interface of Call class constructor
 * 
 * 13    28/06/01 16:49 Junanto
 * Added application global parameters
 * 
 * 12    26/06/01 10:58 Junanto
 * Added new parameter in application "call-proceeding"
 * which indicates whether a PROCEEDING needs to
 * be sent automatically during incoming call or not
 * 
 * 11    5/30/01 7:29a Sonny
 * Fixed recursive acquire system lock.
 * 
 * 10    21/05/01 17:09 Junanto
 * Added group name in the call-in criteria
 * 
 * 9     8/05/01 13:10 Junanto
 * Added parameter max-string
 * 
 * 8     5/01/01 2:39a Sonny
 * applicaton statistics
 * 
 * 7     29/04/01 15:05 Junanto
 * added parameter need-voice in 
 * application to configure voice
 * resource requirement per 
 * application
 * 
 * 6     4/19/01 2:15p Bennylp
 * Fix memory leaks.
 * 
 * 5     4/19/01 12:25p Bennylp
 * Add routing incoming calls.
 * 
 * 4     4/18/01 8:27p Bennylp
 * Added call-in-route configuration.
 * 
 * 3     4/18/01 3:39p Bennylp
 * Fix bug in application shutdown not releasing images.
 * 
 * 2     4/12/01 8:00p Bennylp
 * Integration.
 * 
 * 1     4/11/01 10:30a Bennylp
 * Created.
 *
 */

#ifndef __SL_APPLICATION_H__
#define __SL_APPLICATION_H__

#include "serviceprovider.h"
#include "cfg.h"
#include "call.h"	//linux porting

#ifdef XGATE_HAS_DIAMETER
#include "diameter/diameter_avp.h"
#endif

 /* Forward decl.
 */
union TMEM;
class CT_App_Image;
class CTask;
class Task_Exec_Context;
class Call_In_Route;
struct CallParameter;
enum CallCause;
class AppGlobalParameter;
struct App_Trace_Criteria;
struct Reg_Info;

typedef CT_App_Image * App_Reg_Key;

// Task creation reason.
enum Task_Startup_Type
{
    TASK_STARTUP_CALL,		// Task is created because of incoming call
    TASK_STARTUP_AUTO,		// Task is created automatically when xGate starts.
    TASK_STARTUP_TELNET,	// Task is created from telnet
    TASK_STARTUP_API,		// Task is created by API
    TASK_STARTUP_REGISTRATION,	// Task is created because of incoming registration.
    TASK_STARTUP_HTTP,		// Task is created by incoming HTTP request.
    TASK_STARTUP_SCCP,
    TASK_STARTUP_SMPP,
    TASK_STARTUP_DIAMETER,
};


///////////////////////////////////////////////////////////////////////////////
/** This class describes a single application loaded into the system.
 */
class CT_Application : public Term_Cmd_Target {
public:
	int GetTotalDurationTime();
	int GetTotalBlockTime();
    /** Constructor. */
    CT_Application (const IString & name);

    /** Destructor. */
    ~CT_Application();

    /** Initialization.
     */
    bool Initialize();

    /** Reload the application image.
     */
    bool ReloadApp (const char *path = NULL);

    /** Get number of file images and number of tasks running.
     */
    void GetAppInfo (unsigned & n_images, unsigned & n_tasks);

    /** Shutdown/disable the application.
     */
    bool Shutdown();

    /** Re-enable the application.
     */
    bool NoShutdown();

    /** Determine whether the application is shutdown/disabled.
     */
    bool IsShutdown() const {
	return appIsShutdown;
    }

    /** Determine whether the application can be deleted.
     */
    bool IsDeleteOk() const;


    /** Determine if application was compiled with new/licensed compiler.
     */
    bool IsLicensed() const;

    /**@name ---- Applications Attributes ----
     */
    //@{
    /** Get the application name.
     */
    const String & Name () const {
	return appName;
    }

    /** Get the working directory for this application.
     */
    const String & GetWorkingDir () const {
	return appWorkingDir;
    }

    /** Set the application's working directory.
     */
    void SetWorkingDir (const String & dir) {
	appWorkingDir = dir;
    }

    /** Get the autospawn number.
     */
    size_t GetAutospawnValue () const {
	return appAutoSpawn;
    }

    /** Set the autospawn number.
     */
    void SetAutospawnValue (size_t value) {
	appAutoSpawn = value;
    }

    /** Get the name of application's executable file.
     */
    const String & GetEXEFile () const {
	return appEXEFile;
    }

    /** Get application parameter.
	Return false if the parameter can't be found.
     */
    bool GetAppParam (const IString & name, String & value, bool warn_if_not_exist=true) const;

    /** Set application param.
	Return false if the parameter can't be found.
     */
    bool SetAppParam (const IString & name, const String & value);

    /** Add a new application param.
	Return false if the parameter already exists.
     */
    bool AddAppParam ( const IString & name, const String & value);

    /** Delete an application param.
	Return false if the parameter can't be found.
     */
    bool DelAppParam ( const IString & name );

    /** Set application param, or add a new one if it does not exist.
     */
    bool SetAddAppParam ( const IString & name, const String & value );

    /** Register the task to the list of tasks that use this application.
	This function will return the key to be used in unregistration
	later.
     */
    void RegisterTask ( CTask * task, App_Reg_Key key );

    /** Unregister task from the list of tasks that use this application.
     */
    void UnregisterTask ( CTask * task, App_Reg_Key key );

    /** Update statistics on maximum stack usage for this application.
     */
    void UpdateMaxStackUsageStatistic ( size_t size );

    /** Update statistics on maximum string usage for this application.
     */
    void UpdateMaxStringUsageStatistic ( size_t size );

    /** Create a new task for this application.
     */
    CTask * SpawnTask(const List<String> & cmdline_param, Task_Startup_Type startup_type);
    //@}

    /**
     Terminal command callback
     */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

    /**
     Show configuration for the specified entry.
     This function will be called by the framework whenever it needs to display the
     specified entry. Previously this instance should tell the entry that it wishes to
     handle displaying the entry by calling entry->RegisterShowHandler().

     When this instance is deleted, it should call entry->UnregisterShowHandler()
     so that the handler will not be called (which will cause GPF).
     */
    //virtual STATUS OnShowConfig(
    virtual bool  OnShowConfig(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const;

    /* Show configuration entry without acquiring system lock.
     */
    bool OnShowConfig_NoLock(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const;

    /** Get applications's config tree.
     */
    Cfg_Tree_Group * GetConfigTree() {
	return appConfigTree;
    }

    /** Return true if application needs a voice resource
	to be automatically allocated on start-up
     */
    bool AppNeedVoice() {
	return needVoice;
    }

    /** Return true if application needs to send PROCEEDING
	during incoming call
     */
    bool AppSendCallProceeding() {
	return autoCallProceeding;
    }

    // get statistics interface for Interpreter
    size_t GetStatMaxStack() const { return m_StatMaxStack; }
    size_t GetStatMaxSStack() const { return m_StatMaxSStack; }
    size_t GetStatMaxString() const { return m_StatMaxString; }
    size_t GetStatMaxAllString() const { return m_StatMaxAllString; }
    
    // update statistics interface for Interpreter
    void SetStatMaxStack(size_t new_max)  { m_StatMaxStack = new_max; }
    void SetStatMaxSStack(size_t new_max)  { m_StatMaxSStack = new_max; }
    void SetStatMaxString(size_t new_max)  { m_StatMaxString=new_max; }
    void SetStatMaxAllString(size_t new_max)  { m_StatMaxAllString=new_max; }

    // Get the compile info of the active image.
    const String & GetImageCompileInfo() const;

    // Get configured stack size
    unsigned GetStackSize() const {
	return appStackSize;
    }

    unsigned GetUsageCount() {
	return usage_counter;
    }

    void IncUsageCount() {
	usage_counter++;
    }

    void ResetUsageCount() {
	usage_counter = 0;
    }

private:
    IString appName;	    // Application name.
    String  appWorkingDir;  // Working directory.
    size_t  appAutoSpawn;   // Autospawn value.
    String  appEXEFile;	    // Executable image file.
    size_t  appStackSize;   // App's stack size
    size_t  appMaxString;   // App's maximum nb of strings
    bool    appIsShutdown;
    bool    appIsLicensed;  // App is compiled with new/licensed compiler ver >1.5
    Map <IString, String> appParam;
    String  imageCompileInfo;
    size_t  appDebugLevel;   // App's debug level
    unsigned usage_counter;

    size_t  m_StatMaxStack,	// maximum stack usage
	    m_StatMaxSStack,	// maximum string stack usage
	    m_StatMaxString,	// maximum string usage
	    m_StatMaxAllString;	// maximum all string used

    // List of application images.
    List <CT_App_Image *>  appImageList;

    //List of instances of tasks
    List<CTask*> pTaskList;

    // Configuration tree.
    Cfg_Tree_Group * appConfigTree;

    // CT resources which are automatically
    // allocated to tasks of this application
    bool needVoice;

    // automatically send PROCEEDING message
    // during incoming call
    bool autoCallProceeding;

    // Load an app. image.
    CT_App_Image * LoadApp (const char *path);

    /* Create a new execution context.
     */
    Task_Exec_Context * CreateContext (const List<String> & cmdline_param);

    /* Utility: make a full path.
     */
    void MakeFullPath ( String & filepath );

    /* Cleanup unused images.
     */
    void Cleanup();
};


///////////////////////////////////////////////////////////////////////////////
/** This class describes the application manager.
 */
class CT_App_Manager : public ServiceProvider {
public:
    /** Create the singleton instance of this class.
     */
    static CT_App_Manager * Create();

    /** Get the singleton instance of this class.
     */
    static CT_App_Manager * Instance() {
	return appManagerInstance;
    }

    /** Initialize the application manager.
     */
    virtual bool Load();

    /** Deinitialize the application manager.
     */
    virtual bool Unload();

    /** Create a new application, or get the existing one if the application
	is already loaded.
	Return the instance of the application, or NULL on error.
     */
    CT_Application * CreateApp(const IString & appname);

    /** Get the existing application.
     */
    CT_Application * GetApp(const IString & appname);

    /** Display application trace.
     */
    void AppTrace( CTask *task, const char *trace_id, const char *trace_msg);

    /** Delete an application.
     */
    bool DeleteApp (const IString & appname);

    /** Create a new task.
     */
    CTask * SpawnTask(const IString & appname, const List<String> & cmdline_param,
		      Task_Startup_Type startup_type);

    /** Dispatch an incoming call to application.
	Return NULL if no application matched the properties of the incoming call.
     */
    CTask * RouteIncomingCall (const CallParameter & cparam, const String & port_in, const String &group, CallCause &cause);

    /** Dispatch incoming registration request.
     */
    CTask * RouteIncomingRegistration(const Reg_Info *reg_info);

#ifdef XGATE_HAS_DIAMETER
    /** Dispatch incoming diameter.
     */
    CTask * RouteIncomingDiameter(Diameter_msg msg, const string &linkName, const char* peerRealm);
#endif
    
    /** HandleEvents: this will periodically do clean up the deleted applications.
     */
    virtual void HandleEvents();

    /**
     Terminal command callback
     */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

    /**
     Show configuration for the specified entry.
     This function will be called by the framework whenever it needs to display the
     specified entry. Previously this instance should tell the entry that it wishes to
     handle displaying the entry by calling entry->RegisterShowHandler().

     When this instance is deleted, it should call entry->UnregisterShowHandler()
     so that the handler will not be called (which will cause GPF).
     */
    //virtual STATUS OnShowConfig(
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const;

    /** Get the application root configuration tree.
     */
    Cfg_Tree_Group * RootConfig() {
	return appRootCfg;
    }

    /** Show application statistics.
     */
    void ShowApp ( Term_Session * ) const;

    /** Show application statistics.
     */
    void ShowApp_Brief ( Term_Session * ) const;

    /** Show call in routes.
     */
    //void CT_App_Manager::ShowCallInRoute(Term_Session *session) const;
    void ShowCallInRoute(Term_Session *session) const;

    /** Re-sort call-in-routes.
     */
    void CIR_Resort ( Call_In_Route * route );

    AppGlobalParameter* GetGlobalParam() {
	return _appGlobalParam;
    }

private:
    CT_App_Manager();
    ~CT_App_Manager();

    static CT_App_Manager *appManagerInstance;

    typedef std::pair<CT_Application*, bool> App_List_Val_Type;
    typedef Map <IString,App_List_Val_Type> Application_List_Type;
    Application_List_Type applicationList;

    // Root configuration trees.
    Cfg_Tree_Group *appRootCfg, *cirRootCfg;

    // Application global parameter handler
    AppGlobalParameter* _appGlobalParam;

    // List of call-in-routes
    List <Call_In_Route*> cirList;

    // Trace criteria structure.
    List <App_Trace_Criteria*> appTraceCriteria;
    unsigned unusedTraceCriteriaId;

    // Find a call-in-route based on its name.
    Call_In_Route *FindRoute(const String & name);

    /* Test matching incoming call specifications against the call-in-routes.
     */
    void Cmd_TestRoute (Term_Session * session, 
			const String & cli, 
			const String & ddi, 
			const String & port_in,
			const String & group,
			const String & dev_in);
};


#endif	/* __SL_APPLICATION_H__ */
