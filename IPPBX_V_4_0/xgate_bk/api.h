/* $Header: /xGate/api.h 17    24/05/04 11:17 Sonny $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * API MANAGEMENT for APPLICATION.
 *
 * $Log: /xGate/api.h $
 * 
 * 17    24/05/04 11:17 Sonny
 * added API_PROVIDER_ID_TIMER
 * 
 * 16    3/08/04 7:03p Bennylp
 * Added:
 * - Registrar API (support spawning task for incoming registration)
 * - Authentication API
 * 
 * 15    1/27/04 10:53a Bennylp
 * Add MBOX provider ID.
 * 
 * 14    1/09/04 12:09p Sonny
 * 
 * 13    10/20/03 3:52p Sonny
 * added POS api
 * 
 * 12    8/08/03 3:57p Sonny
 * 
 * 11    4/23/03 5:34p Bennylp
 * Administrivia: tidy-up the usage of API provider ID. Now the
 * enumeration in api.h should be used instead of bare constants.
 * 
 * 10    4/04/02 3:35p Bennylp
 * Changed High_Res_Timer to highest_Res_Timer, which uses
 * CPU_Timestamp_Clock_Timer.
 * 
 * 9     10/09/01 6:55p Bennylp
 * Reset times counters on clear counter command
 * 
 * 8     7/05/01 19:38 Junanto
 * added safeguard when calling undeclared API
 * 
 * 7     5/03/01 11:52a Bennylp
 * Improve profiling calculations and outputs.
 * 
 * 6     4/30/01 10:21a Bennylp
 * Clean unused API profile enabled flag in kernel.
 * 
 * 5     4/30/01 9:59a Bennylp
 * Added API profiling.
 * 
 * 4     4/28/01 6:33a Bennylp
 * Added service profiling and database driver (testing)
 * 
 * 3     4/13/01 8:36p Bennylp
 * Added export API.
 * 
 * 2     4/12/01 8:00p Bennylp
 * Integration.
 *
 */
#ifndef __CAPI_H__
#define __CAPI_H__
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "variant.h"
#include "serviceprovider.h"
#include "hrtimer.h"
#include "log.h"

//-----------------------------------------------------------------------------

enum API_Provider_ID
{
    API_PROVIDER_ID_SYS_API = 1,
    API_PROVIDER_ID_CALL = 2,
    API_PROVIDER_ID_SLEEP = 3,
    API_PROVIDER_ID_DB = 4,
    API_PROVIDER_ID_VOICE = 5,
    API_PROVIDER_ID_FILE = 6,
    API_PROVIDER_ID_MODEM = 7,
    API_PROVIDER_ID_SWITCHING = 9,
    API_PROVIDER_ID_RAP = 10,
    API_PROVIDER_ID_CONSOLE = 11,
    API_PROVIDER_ID_INTERFACE = 12,
    API_PROVIDER_ID_EC = 13,
    API_PROVIDER_ID_ANNOUNCE = 14,
    API_PROVIDER_ID_POS = 15,
    API_PROVIDER_ID_ASR = 16,
    API_PROVIDER_ID_VTVI = 17,
    API_PROVIDER_ID_VTVA = 18,
    API_PROVIDER_ID_HTTP = 20,
    API_PROVIDER_ID_MBOX = 21,
    API_PROVIDER_ID_REGISTRAR = 22,
    API_PROVIDER_ID_SIP_API = 23,
    API_PROVIDER_ID_TIMER = 24,
    API_PROVIDER_ID_DYNPIPE = 25,
    API_PROVIDER_ID_HTTP_SVC = 26,
    API_PROVIDER_ID_MAP = 27,
    API_PROVIDER_ID_DNS = 28,
    API_PROVIDER_ID_SMPP = 61,
    API_PROVIDER_ID_DIAMETER = 62,
    API_PROVIDER_ID_SOFIA = 63,
    API_PROVIDER_ID_CALLTRANSFER = 64,
    API_PROVIDER_ID_HUNTGROUP = 65,
 
};


//-----------------------------------------------------------------------------

typedef unsigned long API_RUNTIME_ID;
typedef unsigned long API_UID;

//-----------------------------------------------------------------------------
class CTask;
class API_Provider_Manager;
class CCfgEntry;
struct CAPIDef;


//-----------------------------------------------------------------------------
// Types of an argument
// NOTE:
//  - value can be retrieved by calling: void *Value() or by type-
//    casting it.
//    ex.:
//	i = (int)arg.Value()
//	i = (int)arg;
//  - for String, value can be set by strcpy-ing Value()
//    ex.:
//	strcpy( arg.Value(), s)
//  - for integer and float, value can be set by direct asignment
//    ex.:
//	arg = i;
//	arg = f;
//  - for BYREF passing type, Value() holds the pointer to the 
//    physical location address in the Intepreter stack.
//    ex.:
//	i = *(int*)arg.Value();
//
//	*(int*)arg.Value() = i;
//
//  - for more details, see "variant.h"
//
//-----------------------------------------------------------------------------
enum PassingType {
    BYVAL,
    BYREF
};
    
//-----------------------------------------------------------------------------
/** This class describes an argument to the API for tasks.
 */
struct CArgTypeT : protected CVariant {

public:

    /** Type of passing, by value or by reference.
     */
    PassingType	    m_PassingType;

    /** The name of the argument (used when exporting the API to an external
        file).
     */
    String	    m_Name;


    /** Construct an argument.
     */
    CArgTypeT( const char *name, PassingType p, CVariant::VType t)
    : CVariant(), m_Name(name), m_PassingType(p)
    {
	ChangeType(t);
    }

    CArgTypeT( const char *name, PassingType p, CVariant::VType t, char *utname)
    : CVariant(), m_Name(name), m_PassingType(p)
    {
		SetValue(utname);
		ChangeType(t);
    }

    /** Copy constructor.
     */
    CArgTypeT(const CArgTypeT& a)
    : CVariant(a), m_PassingType(a.m_PassingType), m_Name(a.m_Name) {}


    /** Destructor.
     */
    ~CArgTypeT() {}


    /** Assignment operator.
     */
    const CArgTypeT & operator = (const CArgTypeT & rhs){
		CVariant::SetValue(rhs);
		
		m_Name = rhs.m_Name;
		m_PassingType = rhs.m_PassingType;
		return *this;
    }

    // Re-publish public method
    using CVariant::Type;
    using CVariant::GetTypeStr;
    using CVariant::AsString;

private:
    CArgTypeT()
    : CVariant(), m_PassingType(BYVAL) {}


    friend class API_Provider_Manager;
};



//-----------------------------------------------------------------------------
// Flags for exporting API
//
#define API_EXP_SHOW_UID	1
#define API_EXP_SHOW_TASK_ARG	2
#define API_EXP_SHOW_RET_ARG	4

#define API_EXP_DEFAULT		(API_EXP_SHOW_UID)


//-----------------------------------------------------------------------------
// API Provider
// NOTE: (for API Provider developers)
//  You can pass these values by specifying the type of argument:
//
//	VALUE		TYPE
//  ----------------------------
//    current task	VT_TASK
//    return value	VT_RET
//
// NOTE:
//  - Provider registration and un-registration must be called explicitly by
//    caller/owner of the provider.
//

//The below code for linux porting
#ifndef STDCALL
#define STDCALL __attribute__((stdcall))
#endif


#define API_CALL

class CAPIProvider {

public:

    typedef STATUS (API_CALL CAPIProvider::*APIPROC)();
    typedef STATUS (API_CALL CAPIProvider::*APIPROC_PTR)(...);

    // Constructor
     CAPIProvider();
    CAPIProvider(int provider_id, const char *name, unsigned int reserved_space=64);


    // QUICK HACK!!
    // This is a function to create CArgTypeT
    CArgTypeT *CArgType( const char *name, PassingType p, CVariant::VType t) 
    { 
	return new CArgTypeT(name,p,t);
    }

    CArgTypeT *CArgType( const char *name, PassingType p, CVariant::VType t, char *utname) 
    { 
	return new CArgTypeT(name,p,t,utname);
    }

    // Destructor
    virtual ~CAPIProvider();


    // Initialize
    virtual STATUS Init() { return OK; }


    // Get the provider ID
    int GetProviderId() const {return m_ProviderId;}


    // Get the name
    const char * GetName() { return m_Name.c_str(); }


    // Helper function, convert provider id and func ordinal to unique API id.
    static API_UID  MakeUID( unsigned int provider_id, unsigned int ordinal) {
	return ((provider_id & 0xFFFF) << 16) + (ordinal & 0xFFFF);
    }


    // Helper function, extract function ordinal from an unique API id
    static int OrdinalFromUID(API_UID u) {
	return u & 0x0000FFFF;
    }


    // Helper function, extract provider ID from an unique API id
    static int ProviderFromUID(API_UID u) {
	return u >> 16;
    }


    // Export a single definition of API to a String
    static STATUS ExportAPI( CAPIDef *api, String &output, int options = API_EXP_DEFAULT);


    // Get the API
    CAPIDef *GetAPI(unsigned int ordinal);


    // Statistics: add value to provider time
    void AddProviderTime(const Time_Value &interval) { m_ProviderTime += interval; }


    // Statistics: returns total provider time
    const Time_Value & GetProviderTime() const { return m_ProviderTime; }


    // Statistics: reset the current provider time value
    void ResetStat() { m_ProviderTime.set(0, 0); }


protected:
    // Given function ordinal number (and provider id of this CAPIProvider), make a
    // unique API identifier
    API_UID MakeUID( unsigned int ordinal) const {
	return MakeUID( m_ProviderId, ordinal);
    }

    // Register an API
    STATUS RegisterAPI( const char *name, int ordinal, bool async,
			CVariant::VType return_type, CAPIProvider::APIPROC proc_addr,
			int calling_type, unsigned int num_of_args,
			...);

    
private:

    // Attribute
    unsigned int m_ProviderId;
    String m_Name;

    // array of APIs
    int	m_ReservedSpace;

    typedef CAPIDef *ptr_api_def;
    ptr_api_def *m_API;

    // timing/statistics
    Time_Value m_ProviderTime;
};



//-----------------------------------------------------------------------------
/** This class describes definition of an API function
 */
struct CAPIDef {

public:

// definition:

    /** Name of the function.
     */
    String		    m_Name;

    /** Unique identifier of the API.
     */
    API_UID		    m_Uid;

    /** API calling type, describes how the API will be called internally.
	Currently the only supported value is CDECL_TYPE.
     */
    enum CallingType {
	CDECL_TYPE = 1,
	VARIANT_TYPE
    }			    m_CallingType;

    /** Does this API causes the task to block?
     */
    bool		    m_Async;

    /** Type of the return value.
     */
    CVariant::VType	    m_RetType;

    /** Number of arguments.
     */
    unsigned int	    m_NumOfArgument;

    /** Types of arguments.
     */
    Vector <CArgTypeT *>    m_ArgType;

	unsigned int m_NumOfSIndex;	
    /** Pointer to C function in CTENGINE that implements this API.
     */
    CAPIProvider::APIPROC    m_Proc;


    /** Default constructor.
     */
    CAPIDef() : m_Proc(0) {};

    /** Constructor.
     */
    CAPIDef( const char *name, unsigned int uid, CallingType ct, bool async,
	     CVariant::VType rettype, unsigned int nargs, CAPIProvider::APIPROC proc)
    : m_Name(name), m_Uid(uid), m_CallingType(ct), m_Async(async),
      m_RetType(rettype), m_NumOfArgument(nargs), m_Proc(proc)
    {
	if (nargs>0) m_ArgType.reserve(nargs);
    }

    /** Constructor.
     */
	
    CAPIDef( const char *name, unsigned int uid, CallingType ct, bool async,
	     CVariant::VType rettype, unsigned int nargs, unsigned int nsindex, CAPIProvider::APIPROC proc)
    : m_Name(name), m_Uid(uid), m_CallingType(ct), m_Async(async),
      m_RetType(rettype), m_NumOfArgument(nargs), m_NumOfSIndex(nsindex), m_Proc(proc)
    {
	if (nargs>0) m_ArgType.reserve(nargs);
    }


    /** Destructor.
     */
    ~CAPIDef() 
    {
	for ( Vector <CArgTypeT*>::iterator it=m_ArgType.begin(); it!=m_ArgType.end(); it++)
	    delete (*it);
    }

    /** Copy constructor.
     */
    CAPIDef( const CAPIDef & a)
    {
	copy(a);
    }


    /** Assignment operator.
     */
    CAPIDef& operator = (const CAPIDef & a) {
	copy(a);
	return *this;
    }

    // equality operator
    // TODO: is this judgement valid?
    bool operator == (const CAPIDef & a) {
	return m_Uid==a.m_Uid;
    }

    // copy function
    void copy( const CAPIDef& a ) {

	// handle self assignment
	if (m_Uid == a.m_Uid || m_Proc == a.m_Proc)
	    return;

	m_Name = a.m_Name;
	m_Uid = a.m_Uid;
	m_CallingType = a.m_CallingType;
	m_Async = a.m_Async;
	m_RetType = a.m_RetType;
	m_NumOfArgument = a.m_NumOfArgument;
	m_ArgType = a.m_ArgType;
	m_Proc = a.m_Proc;
	m_RunTimeId = a.m_RunTimeId;
	m_NStackArg = a.m_NStackArg;
    }

    /** Return the validity.
     */
    bool IsValid() { return m_Proc!=0; };


//------------------------------------------------------------------
// run-time values, filled by Global API Provider after registration

    /** Run-time identification.
     */
    API_RUNTIME_ID	    m_RunTimeId;

    /** Represents actual number of item in the Intepreter's stack.
     */
    unsigned int	    m_NStackArg;

};


//-----------------------------------------------------------------------------
/** This class manages all API's in CTENGINE that are exported to the 
    application. It receives command to execute API from the interpreter,
    and find the correct API provider and API function.

    This is a singleton class. Use Create() and Destroy() member functions to
    manage the instance.
 */
class API_Provider_Manager : public ServiceProvider {
public:
    /** Create the singleton instance.
     */
    static API_Provider_Manager * Create();

    /** Get the singleton instance of this class.
     */
    static API_Provider_Manager * Instance() {
	return apiProviderManagerInstance;
    }

    /** Destroy the singleton instance.
     */
    static void Destroy();

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** Register an API function, returns the run-time id of the function.
        Return -1 on error.
     */
    API_RUNTIME_ID RegisterAPI( CAPIDef *func);
	

    /** Unregister API.
     */
    STATUS UnregisterAPI( API_RUNTIME_ID id );

    /** Register a provider, so that it can be deleted automatically when
        API_Provider_Manager shutdown.
     */
    STATUS RegisterProvider( CAPIProvider *);

    /** Unregister API provider and all API exported by it
        Note: this function does NOT delete the provider. Object deletion should
        be performed by caller of this function.
     */
    STATUS UnregisterProvider( CAPIProvider *);

    /** Check whether a provider is already registered
     */
    bool IsProviderRegistered( CAPIProvider *);

    /** Find an API based on its unique ID.
     */
    CAPIDef* FindAPI( API_UID uid );

    /** Find an API based on its provider ID and the function ordinal number
	within the provider.
     */
    CAPIDef* FindAPI( int provider_id, int ordinal );

    /** Find an API based on its name.
     */
    CAPIDef* FindAPI( const char *name);

    /** Find an API based on its run-time ID.
     */
    CAPIDef* GetAPI(API_RUNTIME_ID id) { 
	return m_Func[id]; 
    }

    /** Execute the API.
     */
	STATUS ExecuteAPI( CAPIDef *func, CTask *task, int *data_seg, int SP);

    /** Export all registered API to file.
     */
    STATUS ExportAPI( String & output );

    /** Reset statistics.
     */
    void ResetStat();

    /** API tracing.
     */
#if defined(__x86_64__)
    STATUS TraceAPICall( CTask *task, CAPIDef *func, long *v, bool is_before=false);
#else
    STATUS TraceAPICall( CTask *task, CAPIDef *func, int *v, bool is_before=false);
#endif

    /** Set API profiling enabled/disabled.
     */
    void SetEnableAPIProfile(bool enabled=true) {
	m_EnableAPIProfile = enabled;
    }

    /** Determine whether API profiling is enabled/disabled.
     */
    bool IsAPIProfileEnabled() const {
	return m_EnableAPIProfile;
    }

    /** Print API provider profiling.
     */
    void ShowProfile( Term_Session *session );

    /** OnClearCounter is called by the framework to clear any statistic values
	maintained by this service provider.
     */
    virtual void OnClearCounter() {
	ResetStat();
    }

private:
    // static singleton instance.
    static API_Provider_Manager *apiProviderManagerInstance;

    // number of functions registered
    unsigned m_NFunc;

    // maximum allocated space to hold functions
    unsigned m_MaxFunc;

    // array of pointer to API definition.
    CAPIDef **m_Func;

    // enable various checking in API registration
    int	m_EnableSanityCheck;

    // enable information when API is registered
    int	m_EnableAPIInfo;

    // enable API call tracing (note: this is expensive)
    int	m_EnableAPITrace;

    // enable time profiling for each API provider
    bool m_EnableAPIProfile;

    // Temporary tring buffer for API tracing
    char m_TempStr1[MAXAPPSTRLEN+1];

    // Timer
    Highest_Res_Timer m_Timer;

    // maximum allocated space to hold provider
    int	m_MaxProvider;

    // array of API providers.
    CAPIProvider** m_Provider;

    // Configuration tree 
    // Find a provider of an API, return NULL if not found
    CAPIProvider *FindProvider( unsigned int provider_id );

    // Execute API
	STATUS ExecuteAPICdecl( CAPIProvider *,
                CAPIDef *,
                CTask *task,
                int *data_seg,
                int SP);

    // Hidden constructor & destructor
    API_Provider_Manager();
    ~API_Provider_Manager();
};


//-----------------------------------------------------------------------------
// INLINES
//-----------------------------------------------------------------------------

// Get the API
inline CAPIDef *CAPIProvider::GetAPI(unsigned int ordinal) 
{
    if (ordinal<(unsigned)0 || ordinal>=(unsigned)m_ReservedSpace)
	return 0;
    else
	return (m_API[ordinal] && m_API[ordinal]->m_Proc) ? m_API[ordinal] : 0;
}

//-----------------------------------------------------------------------------
// Find provider
inline CAPIProvider *API_Provider_Manager::FindProvider( unsigned int provider_id )
{
    if (provider_id>=(unsigned)m_MaxProvider)
	return 0;

    CAPIProvider *prov = NULL;
    try {
	prov = m_Provider[provider_id];
    } catch (...) {
	prov = NULL;
    }
    return prov;
}

//-----------------------------------------------------------------------------
// Find an API based on its provider ID and ordinal, return -1 if not found
inline CAPIDef* API_Provider_Manager::FindAPI( int provider_id, int ordinal)
{
    if  (provider_id>=m_MaxProvider || provider_id == 64 )
	return 0;

    CAPIDef* def = NULL;
    try {
	def = m_Provider[ provider_id ]->GetAPI(ordinal);
    } catch (...) {
	def = NULL;
    }
    return def;
}


//-----------------------------------------------------------------------------
// Find a run-time identifier by its UID, return -1 if not found
inline CAPIDef* API_Provider_Manager::FindAPI( API_UID uid )
{
    return FindAPI( CAPIProvider::ProviderFromUID(uid), CAPIProvider::OrdinalFromUID(uid));
}


//-----------------------------------------------------------------------------
// Execute an API
inline STATUS API_Provider_Manager::ExecuteAPI(CAPIDef *func,
                         CTask *task,
                         int *data_seg,
                         int SP)
{
    CAPIProvider *prov = FindProvider( CAPIProvider::ProviderFromUID(func->m_Uid));
    if (prov) {
	return ExecuteAPICdecl( prov, func, task, data_seg, SP);
    } else {
	CLog::Error("API",task,"Invalid API");
	return NOT_OK;
    }
}

//-----------------------------------------------------------------------------
// Determine whether the specified API provider is registered
inline bool API_Provider_Manager::IsProviderRegistered( CAPIProvider *provider)
{
    return m_Provider[provider->GetProviderId()] != 0;
}


#endif
