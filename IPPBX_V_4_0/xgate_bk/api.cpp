/* $Header: /xGate/api.cpp 22    3/08/04 7:03p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * API MANAGEMENT for APPLICATION.
 *
 * $Log: /xGate/api.cpp $
 * 
 * 22    3/08/04 7:03p Bennylp
 * Protection for GetStringTab/CopyString which halts the task if
 * something bad happens
 * 
 * 21    10/19/01 12:13p Bennylp
 * Sort the show times output
 * 
 * 20    7/14/01 7:53p Bennylp
 * Reduce friends of class Task_Exec_Context.
 * 
 * 19    5/28/01 6:11p Bennylp
 * More info to system locking and handle unreleased recursive lock.
 * 
 * 18    5/26/01 8:04p Bennylp
 * Record each call to AcquireSystemLock in DEBUG mode.
 * 
 * 17    10/05/01 12:40 Junanto
 * Replaced "\r\n" sequence to "\n". Telnet translates
 * an orphaned "\n" into "\r\n" during output
 * 
 * 16    10/05/01 11:12 Junanto
 * Replaced all "\r\n" with a constant "\r\n"
 * 
 * 15    7/05/01 20:23 Junanto
 * activate API debugging
 * 
 * 14    5/03/01 1:09p Bennylp
 * Changed profiling to 'show times', and more improvements.
 * 
 * 13    5/03/01 11:52a Bennylp
 * Improve profiling calculations and outputs.
 * 
 * 12    4/30/01 9:59a Bennylp
 * Added API profiling.
 * 
 * 11    4/28/01 6:33a Bennylp
 * Added service profiling and database driver (testing)
 * 
 * 10    4/26/01 3:37a Sonny
 * 
 * 9     4/25/01 6:46a Sonny
 * 
 * 8     4/17/01 12:31p Bennylp
 * Tidy up ExportAPI output.
 * 
 * 7     4/13/01 11:15p Bennylp
 * Tidy up API export output.
 * 
 * 6     4/13/01 8:36p Bennylp
 * Added export API.
 * 
 * 5     13/04/01 20:05 Junanto
 * 
 * 4     4/12/01 8:59p Bennylp
 * Integration and some bug fixes.
 * 
 * 3     4/12/01 8:00p Bennylp
 * Integration.
 *
 */
//#include "stdafx.h"
#include <asm/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdint.h>
#include "api.h"
#include "log.h"
#include "kernel.h"
#include "types.h"
#include "task.h"
#include "term.h"	// Term_Session


// The identifier of Global API Provider in the Log
#define GAP	    "api"
#define NEWLINE	    "\n"
#define _snprintf   snprintf	//linux porting

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define strcmpi strcasecmp

API_Provider_Manager *API_Provider_Manager::apiProviderManagerInstance;

//Narasimham added for debug purpose

 STATUS (API_CALL CAPIProvider::*funcptr)();

//-----------------------------------------------------------------------------
// API Provider constructor
//
CAPIProvider::CAPIProvider() {
}

CAPIProvider::CAPIProvider( int provider_id, const char *name, unsigned int reserved_space )
: m_ProviderId(provider_id), 
  m_Name(name), 
  m_ReservedSpace(reserved_space), 
  m_ProviderTime(0, 0)
{
    m_API = new ptr_api_def[m_ReservedSpace];
    memset( &m_API[0], 0, m_ReservedSpace * sizeof(ptr_api_def));
}


//-----------------------------------------------------------------------------
// API Provider destructor
//
CAPIProvider::~CAPIProvider()
{
    if (API_Provider_Manager::Instance()->IsProviderRegistered(this))
	API_Provider_Manager::Instance()->UnregisterProvider(this);

    for (int i=0; i<m_ReservedSpace; i++)
	delete m_API[i];

    delete [] m_API;
	m_API = NULL;
}


//-----------------------------------------------------------------------------
// Register an API. The API will be saved in a list.
//
STATUS CAPIProvider::RegisterAPI (  const char *name,
				    int ordinal, bool async,
				    CVariant::VType return_type,
				    CAPIProvider::APIPROC proc_addr,
				    int calling_type,
				    unsigned int num_of_args,
				    ...) 
{
    va_list  args;
    unsigned i;

    va_start( args, num_of_args );

    //*** CLog::Detail( GAP, "Registering API %s",name);

    // validate ordinal
    if (ordinal<0 || ordinal>=m_ReservedSpace) {
	CLog::Error( GAP, "The API ordinal for %s (%d) exceeds reserved space (%d)",
			  name, ordinal, m_ReservedSpace);
	goto on_error;
    }

    // validate number of arguments
    if (num_of_args > API_MAX_ARGS) {
	CLog::Error( GAP, "Number of arguments of '%s' is exceeding maximum (%d)",
			   name, API_MAX_ARGS);
	goto on_error;
    }

    // create the API
    m_API[ ordinal ] = new CAPIDef( name, MakeUID(ordinal), (CAPIDef::CallingType)calling_type,
				    async, return_type, num_of_args, proc_addr);

    // Fill the argument definitions

    for ( i=0; i<num_of_args; i++) {
	CArgTypeT* p = (CArgTypeT*)va_arg(args, CArgTypeT*);
	m_API[ ordinal ]->m_ArgType.push_back(p);

    }
    va_end( args );


    // Register the API
    if (API_Provider_Manager::Instance()->RegisterAPI(m_API[ordinal]) != -1) {
	return OK;
    } else {
	delete m_API[ ordinal ];
	m_API[ ordinal ] = 0;	
	return NOT_OK;
    }


    // should never reach here
    return OK;

on_error:

    for (unsigned j=0; j<num_of_args; j++) {
	CArgTypeT* p = (CArgTypeT*)va_arg(args, CArgTypeT*);
	delete p;
	p = NULL;

    }
    va_end( args );

    return NOT_OK;
}



//-----------------------------------------------------------------------------
// Export a single definition of API to a String
//
STATUS CAPIProvider::ExportAPI( CAPIDef *api, String &s, int options )
{
    char numbers[20];
    
    s.erase();

    // show API ordinals?
    if (options & API_EXP_SHOW_UID) {
	sprintf( numbers, "  %3d %3d   ", ProviderFromUID(api->m_Uid),
					  OrdinalFromUID(api->m_Uid));
    
	s = numbers;
    }

    // proc or func?
    if (api->m_RetType==CVariant::VT_VOID)
	s += "sub  ";
    else
	s += "func ";
    
    // the arguments
    s += api->m_Name + "( ";
    bool not_first_arg = false;
    unsigned indent_begin = s.size();
    unsigned last_pos = 0;
    for (unsigned i=0; i<api->m_NumOfArgument; i++) {
		CArgTypeT *arg = api->m_ArgType[i];

		// show VT_RET argument?
		if (arg->Type() == CVariant::VT_RET && ! (options & API_EXP_SHOW_RET_ARG))
			continue;

		// show VT_TASK argument?
		if (arg->Type() == CVariant::VT_TASK && ! (options & API_EXP_SHOW_TASK_ARG))
			continue;
		
		if (not_first_arg)
			s += "; ";
		not_first_arg = true;

		// go to next line
		/*
		if ( (s.size()+arg->m_Name.size()+6+strlen(arg->GetTypeStr()) + (arg->m_PassingType==BYREF ? 4 : 0)) - last_pos > 79) {
			s += NEWLINE;
			s.append(indent_begin, ' ');
			last_pos += 80;
		}
		

		// byval or byref?
		if (arg->m_PassingType==BYREF)
			s += "var ";

		// name and type
		s += arg->m_Name + ": ";
		s += arg->GetTypeStr();
		*/
		if (arg->Type() == CVariant::VT_USRTYPE) {
			if ( (s.size()+strlen(arg->AsString())+6 + (arg->m_PassingType==BYREF ? 4 : 0)) - last_pos > 79) {
				s += NEWLINE;
				s.append(indent_begin, ' ');
				last_pos += 80;
			}
		} else {
			// go to next line
			if ( (s.size()+arg->m_Name.size()+6+strlen(arg->GetTypeStr()) + (arg->m_PassingType==BYREF ? 4 : 0)) - last_pos > 79) {
				s += NEWLINE;
				s.append(indent_begin, ' ');
				last_pos += 80;
			}
		}
		// byval or byref?
		if (arg->m_PassingType==BYREF)
			s += "var ";

		// name and type
		s += arg->m_Name + ": ";
		if (arg->Type() == CVariant::VT_USRTYPE) {
			s += arg->AsString();
		} else {
			s += arg->GetTypeStr();
		}

	}
	
    s += ")";
	

    // epilogue
    if (api->m_RetType==CVariant::VT_VOID)
	s += ";";
    else {
	// go to next line
	if (s.size()+3+strlen(CVariant::GetTypeStr( api->m_RetType )) - last_pos > 78) {
	    s += NEWLINE;
	    s.append(indent_begin, ' ');
	    last_pos += 80;
	}

	s += String(" : ") + CVariant::GetTypeStr( api->m_RetType ) + ";";
    }
	

    return OK;
}


//-----------------------------------------------------------------------------
// Global API Provider Constructor
//
API_Provider_Manager * API_Provider_Manager::Create()
{
    assert (apiProviderManagerInstance==NULL);
    if (apiProviderManagerInstance == NULL)
	apiProviderManagerInstance = new API_Provider_Manager;
    return apiProviderManagerInstance;
}

void API_Provider_Manager::Destroy()
{
    if (apiProviderManagerInstance != NULL) {
	delete apiProviderManagerInstance;
	apiProviderManagerInstance = NULL;
    }
}

//-----------------------------------------------------------------------------
// Global API Provider Constructor
//
API_Provider_Manager::API_Provider_Manager()
: ServiceProvider("api", false, false),
  m_MaxFunc(0), m_EnableSanityCheck(1), m_EnableAPIInfo(1), m_EnableAPITrace(1),
  m_MaxProvider(0)
{
    m_Provider=0;
    m_Func = 0;
}


//-----------------------------------------------------------------------------
// Global API Provider Destructor
API_Provider_Manager::~API_Provider_Manager()
{
}


//-----------------------------------------------------------------------------
// Load: loading contaners and API providers
//
bool API_Provider_Manager::Load()
{
    // maximum allocated space to hold provider
    m_MaxProvider = 100;  //H248

    // maximum allocated space to hold functions
    m_MaxFunc = 1024;

    // enable various checking in API registration
    m_EnableSanityCheck = 1;

    // enable information when API is registered
    m_EnableAPIInfo = 0;

    // enable API call tracing (note: this is expensive)
    m_EnableAPITrace = 1;

    // enable time profiling for each API provider
    m_EnableAPIProfile = true;

    // Initializing provider table
    typedef CAPIProvider *ptr_provider;

    m_Provider = new ptr_provider[m_MaxProvider];
    memset( m_Provider, 0, m_MaxProvider * sizeof(ptr_provider));

    // Initializing functions table
    typedef CAPIDef* ptr_api_def;

    m_Func = (ptr_api_def*) new ptr_api_def[m_MaxFunc];
    memset( m_Func, 0, m_MaxFunc * sizeof(ptr_api_def));

    m_NFunc = 0;

    if (m_Timer.init() == false) {
		CLog::Error ( GAP, "Unable to initialize high resolution timer");
		delete [] m_Func;
		m_Func = NULL;
		delete [] m_Provider;
		m_Provider = NULL;
		return false;
    }

    // loading API provider
    /* CLog::Debug( GAP, "Initialized with container to hold %d functions",
		       m_MaxFunc);
     */
    //printf("Initialized with container to hold %d functions", m_MaxFunc);

    return true;
}


//-----------------------------------------------------------------------------
// Shutdown
//
bool API_Provider_Manager::Unload()
{
    // unloading API provider, if any (and consequently unloading their APIs)
    for (int i=0; i<m_MaxProvider; i++) {
	if (m_Provider[i]) {
	    UnregisterProvider(m_Provider[i]);
	}
    }

    // deleting function table
    if (m_Func) {
	delete [] m_Func;
	m_Func = 0;
    }
    m_MaxFunc = 0;

    // deleting provider table
    delete [] m_Provider;
    m_Provider=0;

    CLog::Debug( GAP, "Shutdown");

    return true;
}


//-----------------------------------------------------------------------------
// Register an API function, returns the run-time id of the function.
// Return -1 if error.
API_RUNTIME_ID API_Provider_Manager::RegisterAPI(	CAPIDef *func )
{

    // validate parameter
    if (!func) {
	CLog::Fatal( GAP, "API pointer is NULL in RegisterAPI");
	return -1;
    }

    unsigned i;
    CAPIProvider *prov;

    if (m_NFunc == m_MaxFunc) {
	CLog::Error( GAP, "Function table is full");
	return -1;
    }

    //
    // SANITY CHECKS
    //
    int ret_arg_found;

    if (!m_EnableSanityCheck)
	goto skip_check;

    // 0. Is provider already registered?
    prov = FindProvider(CAPIProvider::ProviderFromUID(func->m_Uid) );
    if (!prov) {
	CLog::Error( GAP, "Provider with id %d is not registered when "
			   "registering API '%s'", 
			   CAPIProvider::ProviderFromUID(func->m_Uid), 
			   func->m_Name.c_str());
	return -1;
    }

    // 0.5 Execution type
    if (func->m_CallingType == CAPIDef::VARIANT_TYPE) {
	CLog::Error( GAP, "Calling type VARIANT is not supported for function %s", 
			   func->m_Name.c_str());
	return -1;
    }

    // 1. Check number of arguments v.s. size of argument array
    if ((int)func->m_ArgType.size() != func->m_NumOfArgument) {
	CLog::Error( GAP, "Number of array arguments doesn't match the one "
			  "specified in NumOfArgument");
	return -1;
    }

    // 2. check arguments and return value consistency
    ret_arg_found = -1;
    for (i=0; i<func->m_NumOfArgument; i++) {
	if (func->m_ArgType[i]->Type() == CVariant::VT_RET) {
	    if (ret_arg_found != -1) {
		CLog::Error( GAP, "API '%s': Return value has already been specified "
				   "as argument %d", 
				   func->m_Name.c_str(), ret_arg_found);
		return -1;
	    } else if (func->m_RetType == CVariant::VT_VOID) {
		CLog::Error( GAP, "API '%s': Return value can't be VT_VOID while "
				   "you specify argument %d as VT_RET", 
				   func->m_Name.c_str(), i);
		return -1;
	    } else if (func->m_ArgType[i]->m_PassingType != BYREF) {
		CLog::Warning( GAP, "API '%s': passing type must be BY_REF "
				   "for return value (argument %d)", 
				   func->m_Name.c_str(), i);
		func->m_ArgType[i]->m_PassingType = BYREF;
		return -1;
	    } else if (func->m_ArgType[i]->Type() == CVariant::VT_VOID) {
		CLog::Error( GAP, "API '%s': argument %d can't be VT_VOID", i);
		return -1;
	    }

	    ret_arg_found = i;
	}
    }

    // 3. if no VT_RET argument found, but the return type is not VT_VOID..
    if (ret_arg_found==-1 && func->m_RetType != CVariant::VT_VOID) {
	CLog::Error( GAP, "API '%s': return value is not VT_VOID, but "
			  "there's no VT_RET argument specified",
			  func->m_Name.c_str());
	return -1;
    } 
    
    // 4. return type is valid?
    if (func->m_RetType != CVariant::VT_INT && 
	func->m_RetType != CVariant::VT_FLOAT &&
	func->m_RetType != CVariant::VT_STRING &&
	func->m_RetType != CVariant::VT_BOOLEAN &&
	func->m_RetType != CVariant::VT_VOID) {
	CLog::Error( GAP, "API '%s': return value is unsupported",
			  func->m_Name.c_str());
	return -1;
    };

    // 5. check existing ID, name, and proc address
    for (i=0; i<m_MaxFunc; i++) {
	if (!m_Func[i])
	    continue;

	if (m_Func[i]->m_Uid == func->m_Uid) {
	    CLog::Warning( GAP, "API '%s': another API with same UID has "
			      "already been registered to '%s'",
			      func->m_Name.c_str(), m_Func[i]->m_Name.c_str());

	}
	
	if ( !stricmp(m_Func[i]->m_Name.c_str(), func->m_Name.c_str())) {
	    CLog::Error( GAP, "API '%s' has the same name with previously "
				"registered API", func->m_Name.c_str());
	    return -1;

	}
	
	if (m_Func[i]->m_Proc == func->m_Proc) {
	    CLog::Info( GAP, "API '%s' has the same function address "
			      "with API '%s'", 
			      func->m_Name.c_str(), m_Func[i]->m_Name.c_str());
	    break;
	}
    }

    // - end of sanity checks
skip_check:

    // find unused table entry, first try m_Func[m_NFunc]
    if (m_Func[m_NFunc] == 0) {
	m_Func[m_NFunc] = func;
	func->m_RunTimeId = m_NFunc;
    } else {
	for (i=0; i<m_MaxFunc && m_Func[i]!=0; ++i)
	    ;
	
	if (i==m_MaxFunc) {
	    CLog::Error( GAP, "No empty table entry left for the new API");
	    return -1;
	}
	m_Func[i] = func;
	func->m_RunTimeId = i;
    }

    // calculate real number of argument placed in the stack. Note that some
    // argument type (like VT_TASK and VT_RET) are not real argument passed
    // by application. But rather, these argument types will be filled by us, 
    // later in ExecuteAPI.
    func->m_NStackArg=0;
    for( i=0, func->m_NStackArg=0; i<func->m_NumOfArgument; ++i) {
		if (func->m_ArgType[i]->m_Type != CVariant::VT_TASK &&
			func->m_ArgType[i]->m_Type != CVariant::VT_RET) {
			func->m_NStackArg++;
		}
    }

    m_NFunc++;

    if (m_EnableAPIInfo) {
	String info;
	
	CAPIProvider::ExportAPI( func, info );
	CLog::Detail( GAP, "API registered: %s", info.c_str());
    }

    return i;
}




//-----------------------------------------------------------------------------
// Unregister API
//
STATUS API_Provider_Manager::UnregisterAPI( API_RUNTIME_ID id )
{

    // validate parameter
    if (id < 0 || id >= m_MaxFunc) {
	CLog::Fatal( GAP, "Invalid API runtime id in UnregisterAPI");
	return NOT_OK;
    }

    // unregister the API
    m_Func[id]->m_RunTimeId = -1;
    m_Func[id] = 0;
    m_NFunc--;

    return OK;
}


//-----------------------------------------------------------------------------
// Register a provider, so that it can be unregistered automatically when
// API_Provider_Manager shutdown
//
STATUS API_Provider_Manager::RegisterProvider( CAPIProvider *provider)
{
    
    // validate parameter
    if (!provider) {
	CLog::Fatal( GAP, "Provider parameter is NULL in RegisterProvider");
	return NOT_OK;
    }

    // validate provider ID
    if (provider->GetProviderId() < 0 || provider->GetProviderId() >= m_MaxProvider ) {
	CLog::Error( GAP, "Invalid provider id for %s. Expecting >=0 and < %d",
			   provider->GetName(), m_MaxProvider);
	return NOT_OK;
    }

    // find out if provider is already registered
    if (m_Provider[provider->GetProviderId()]) {
	CLog::Error( GAP, "Provider with id %d is already registered", 
			  provider->GetProviderId());
    }

    // register the provider
    m_Provider[provider->GetProviderId()] = provider;


   /*
    CLog::Debug( GAP, "Provider with ID %d (%s) registered", 
		     provider->GetProviderId(), provider->GetName());
    */
    /* printf("Provider with ID %d (%s) registered", provider->GetProviderId(), provider->GetName());
    */

    return OK;
}


//-----------------------------------------------------------------------------
// Unregister a provider and all APIs exported by it.
// Note: this function does NOT delete the provider. Object deletion should be
// performed by caller of this function
//
STATUS API_Provider_Manager::UnregisterProvider( CAPIProvider *provider)
{

    // validate our status
    if (!m_Func) {
	/*CLog::Fatal( GAP, "API_Provider_Manager is already shutdown (or haven't "
			    "been started) when unregistering provider %s", 
			    provider->GetName());
        */
	return NOT_OK;
    }

    // validate parameter
    if (!provider) {
       /*
	CLog::Fatal( GAP, "Parameter provider is NULL in UnregisterProvider");
        */
	return NOT_OK;
    }

    // unregistering its APIs
    int provider_id = provider->GetProviderId();
     for (unsigned i=0; i<m_MaxFunc; ++i) {
	if ( m_Func[i] && 
	    CAPIProvider::ProviderFromUID(m_Func[i]->m_Uid) == provider_id) {
	    UnregisterAPI(i);
	}
    }

    // removing from our provider list
    m_Provider[provider_id] = 0;

    //CLog::Debug( GAP, "Provider %s unregistered", provider->GetName());

    return OK;
}


//-----------------------------------------------------------------------------
// Find API by its name
//
CAPIDef* API_Provider_Manager::FindAPI( const char *name )
{
    for (unsigned i=0; i<m_MaxFunc; ++i)
	if ( m_Func[i] && !stricmp(m_Func[i]->m_Name.c_str(), name))
	    return m_Func[i];

    return 0;
}



//-----------------------------------------------------------------------------
// Export APIs to an .INC file
//
STATUS API_Provider_Manager::ExportAPI( String & output )
{
    output = "library" NEWLINE;
    for (unsigned i=0; i<m_MaxFunc; ++i) {
	if (!m_Func[i])
	    continue;

	String s;
	
	CAPIProvider::ExportAPI(m_Func[i], s);
	s += NEWLINE;
	output += s;
    }
    output += "endlibrary" NEWLINE;
    return OK;
}


//-----------------------------------------------------------------------------
// Trace API call
//
#if defined (__x86_64__)
STATUS API_Provider_Manager::TraceAPICall( CTask *task, CAPIDef *func, long v[], bool is_before)
#else
STATUS API_Provider_Manager::TraceAPICall( CTask *task, CAPIDef *func, int v[], bool is_before)
#endif
{
    unsigned i;

    if (!m_EnableAPITrace)
	return OK;
	
    bool show_by_ref = !func->m_Async || !is_before;

    int len = 0;
    int remaining_len = MAXAPPSTRLEN-len;
    char *s = m_TempStr1 + len;

    *m_TempStr1 = '\0';

    for (i=0; i<func->m_NumOfArgument && remaining_len>1; ++i) {
	switch (func->m_ArgType[i]->Type()) {
	case CVariant::VT_TASK:
	    //len += _snprintf( s, remaining_len, "task=%d;", task->GetPID());
	    len = 0;
	    break;
	    
	case CVariant::VT_RET:
	    if (!show_by_ref)
		continue;

	    switch ( func->m_RetType ) {
	    case CVariant::VT_STRING:
		len += _snprintf( s, remaining_len, "%s='%s';", func->m_ArgType[i]->m_Name.c_str(), v[i]);
		if (len == -1) {
		    len = MAXAPPSTRLEN;
		}
		break;
	    case CVariant::VT_INT:
		len += _snprintf( s, remaining_len, "%s=%d;", func->m_ArgType[i]->m_Name.c_str(), *(int*)v[i]);
		break;
	    case CVariant::VT_FLOAT:
		len += _snprintf( s, remaining_len, "%s=%f;", func->m_ArgType[i]->m_Name.c_str(), *(float*)v[i]);
		break;
	    default:
		*m_TempStr1 = '\0';
	    }

	    break;
	    
	case CVariant::VT_INT:
	    if (func->m_ArgType[i]->m_PassingType == BYVAL)
		len += _snprintf( s, remaining_len, "%s=%d;", func->m_ArgType[i]->m_Name.c_str(), v[i]);
	    else if (show_by_ref)
		len += _snprintf( s, remaining_len, "%s=%d;", func->m_ArgType[i]->m_Name.c_str(), v[i]);
	//	len += _snprintf( s, remaining_len, "%s=%d;", func->m_ArgType[i]->m_Name.c_str(), *(long *)v[i]);
	    break;

	case CVariant::VT_FLOAT:
	    if (func->m_ArgType[i]->m_PassingType == BYVAL)
		len += _snprintf( s, remaining_len, "%s=%f;", func->m_ArgType[i]->m_Name.c_str(), v[i]);
	    else if (show_by_ref)
		len += _snprintf( s, remaining_len, "%s=%f;", func->m_ArgType[i]->m_Name.c_str(), *(float*)v[i]);
	    break;

	case CVariant::VT_STRING:
	    if (func->m_ArgType[i]->m_PassingType == BYVAL || show_by_ref) {
		int slen;
                //if(i>2) 
                //  break;
		slen = _snprintf( s, remaining_len, "%s='%s';", func->m_ArgType[i]->m_Name.c_str(), v[i]);
		if (slen == -1)
		    slen = remaining_len;
		len += slen;
	    }
	    break;

	default:
	    len += _snprintf( s, remaining_len, "%s=0x%X;", func->m_ArgType[i]->m_Name.c_str(), v[i]);
	    break;
	}

	remaining_len = MAXAPPSTRLEN-len;
	s = m_TempStr1 + len;

    }

    m_TempStr1[MAXAPPSTRLEN] = '\0';

    if (func->m_Async) {
	if (is_before) {
	    CLog::Detail( GAP, task, "[API-b] %s %s", func->m_Name.c_str(), m_TempStr1);
	} else {
	    CLog::Debug( GAP, task, "[API-e] %s %s", func->m_Name.c_str(), m_TempStr1);
	}
    } else {
	CLog::Detail( GAP, task, "[API-s] %s %s", func->m_Name.c_str(), m_TempStr1);
    }

    return OK;
}


struct ProfileResult {
    String	providerName;
    Time_Value  providerTime;

    ProfileResult(const String &name, const Time_Value & t)
	: providerName(name), providerTime(t) {}
};

inline bool operator < (const ProfileResult & p1, const ProfileResult & p2)
{
    return p1.providerTime < p2.providerTime;
}

inline bool operator > (const ProfileResult & p1, const ProfileResult & p2)
{
    return p1.providerTime > p2.providerTime;
}

//-----------------------------------------------------------------------------
// Create formatted statistics output
//
void API_Provider_Manager::ShowProfile( Term_Session *session)
{
    //
    // Build results while holding system's lock.
    //

    // types
    Time_Value total(0, 0);
    Vector <ProfileResult> results;
    
    ACQUIRE_SYSTEM_LOCK();
    for (int i=0; i<m_MaxProvider; i++) {
	if (!m_Provider[i]) continue;
	results.push_back( ProfileResult(m_Provider[i]->GetName(), m_Provider[i]->GetProviderTime()) );
	total += results.back().providerTime;
    }
    double kernel_time = Kernel::Instance()->GetKernelBusyTime().to_usec();
    if (kernel_time == 0.0)
	kernel_time = 1;
    RELEASE_SYSTEM_LOCK();

    // Sort the results.
    std::sort(results.begin(), results.end(), std::greater<ProfileResult>());

    double total_time = total.to_usec();
    if (total_time == 0.0)
	total_time = 1;

    // Show the result.
    //session->Sendf ("API provider profiling is %s" NEWLINE, (m_EnableAPIProfile ? "enabled" : "disabled"));
    session->Sendf ("Total API execution time: %u sec:%06u usec (%5.2f %% system)" NEWLINE, 
						  total.sec(),
						  total.usec(),
						  total_time * 100 / kernel_time);
    session->Sendf (NEWLINE);
    session->Sendf (" API group name        Time elapsed          %% API    %% System" NEWLINE);
    session->Sendf (" =============================================================" NEWLINE);

    char temp[128];
    Vector<ProfileResult>::const_iterator it_result, end_result;
    end_result = results.end();
    for (it_result = results.begin(); it_result != end_result; ++it_result) {
	const ProfileResult & result = *it_result;
	int len = sprintf (temp, " %13s    %7u sec:%06u usec   %5.2f %%   %5.2f %%" NEWLINE, result.providerName.c_str(), 
									    result.providerTime.sec(),
									    result.providerTime.usec(),
									    result.providerTime.to_usec() * 100 / total_time,
									    result.providerTime.to_usec() * 100 / kernel_time);
	if (len > 0)
	    session->Send ( temp, (size_t)len);
    }
}


//-----------------------------------------------------------------------------
// Reset statistic output
//
void API_Provider_Manager::ResetStat()
{
    for (int i=0; i<m_MaxProvider; i++) {
	if (!m_Provider[i]) continue;
	m_Provider[i]->ResetStat();
    }
}


//-----------------------------------------------------------------------------
// Execute an API (called by Intepreter).
// Parameter:
//  - prov	API provider
//  - func	The API
//  - task	The task
//  - data_seg	Start of data segment
//  - SP	Value of SP register (set to the location of BP, see below)
// Stack layout:
//  - BP
//  - ??
//  - arg-3 (last argument)
//  - arg-2
//  - arg-1 (first argument)

//-----------------------------------------------------------------------------
#if defined(__x86_64__)
inline STATUS _Exec( CAPIProvider *prov, CAPIProvider::APIPROC proc, 
		     int n_stack, long stack[])
#else
inline STATUS _Exec( CAPIProvider *prov, CAPIProvider::APIPROC proc, 
             int n_stack, int stack[])
#endif
{
    STATUS state;
    int saved_esi=0;
    int saved_esp=0;
    int return_value=0;
    int n=0,fetchaddr=0;
    int i,j;

    funcptr = proc;


#if defined(__x86_64__)
    long saved_rbx,saved_rsp, saved_rbp;
    long saved_r12,saved_r13,saved_r14,saved_r15;
    long return_rvalue=0;
    long n_64=0,fetchaddr_64=0;
    long amit =0;
// push the arguments to stack
/*
    printf("Number of arguments =%d\n",n_stack);
  for(i=0; i < n_stack; i++)
   printf(" arg[i]=%d,%ld\n",i,stack[i]);
*/
      __asm__ ("pushq %0" : : "r" (n_64));
   __asm__ ("movq %%rsp, %0" : "=r" (saved_rsp));
   __asm__ ("movq %%rbp, %0" : "=r" (saved_rbp));
   __asm__ ("movq %%rbx, %0" : "=r" (saved_rbx));
   __asm__ ("movq %%r12, %0" : "=r" (saved_r12));
   __asm__ ("movq %%r13, %0" : "=r" (saved_r13));
   __asm__ ("movq %%r14, %0" : "=r" (saved_r14));
   __asm__ ("movq %%r15, %0" : "=r" (saved_r15));

     /* C++: first argument is this pointer of about to be called function */
      __asm__ ("movq %0, %%rdi" : : "r" (prov)); /* this pointer of funcptr */
      switch(n_stack) {
        case 1: /*  first argument of new function goes in rsi */
           __asm__ ("movq %0, %%rsi" : : "r" ((long)stack[0]));
           break;
        case 2:
           __asm__ ("movq %0, %%rsi" : : "r" ((long)stack[0]));
           __asm__ ("movq %0, %%rdx" : : "r" ((long)stack[1]));
           break;
        case 3:
           __asm__ ("movq %0, %%rsi" : : "r" ((long)stack[0]));
           __asm__ ("movq %0, %%rdx" : : "r" ((long)stack[1]));
           __asm__ ("movq %0, %%rcx" : : "r" ((long)stack[2]));
           break;
        case 4:
           __asm__ ("movq %0, %%rsi" : : "r" ((long)stack[0]));
           __asm__ ("movq %0, %%rdx" : : "r" ((long)stack[1]));
           __asm__ ("movq %0, %%rcx" : : "r" ((long)stack[2]));
           __asm__ ("movq %0, %%r8" : : "r" ((long)stack[3]));
           break;
        case 5:
           __asm__ ("movq %0, %%rsi" : : "r" ((long)stack[0]));
           __asm__ ("movq %0, %%rdx" : : "r" ((long)stack[1]));
           __asm__ ("movq %0, %%rcx" : : "r" ((long)stack[2]));
           __asm__ ("movq %0, %%r8" : : "r" ((long)stack[3]));
           __asm__ ("movq %0, %%r9" : : "r" ((long)stack[4])); 
           break;
        case 6:
           __asm__ ("movq %0, %%rsi" : : "r" ((long)stack[0]));
           __asm__ ("movq %0, %%rdx" : : "r" ((long)stack[1]));
           __asm__ ("movq %0, %%rcx" : : "r" ((long)stack[2]));
           __asm__ ("movq %0, %%r8" : : "r" ((long)stack[3]));
           __asm__ ("movq %0, %%r9" : : "r" ((long)stack[4]));
              /* Remaining arguments stored in stack */
          __asm__ ("pushq %0" : : "r"  ((long)amit));
          __asm__ ("pushq %0" : : "r" ((long) stack[5])); 
        case 7:  /* mainly spawntask has 7 arguments: test as much as possible */
           __asm__ ("movq %0, %%rsi" : : "r" ((long)stack[0]));
           __asm__ ("movq %0, %%rdx" : : "r" ((long)stack[1]));
           __asm__ ("movq %0, %%rcx" : : "r" ((long)stack[2]));
           __asm__ ("movq %0, %%r8" : : "r" ((long)stack[3]));
           __asm__ ("movq %0, %%r9" : : "r" ((long)stack[4]));
              /* Remaining arguments stored in stack */
	   __asm__ ("pushq %0" : : "r"  ((long)amit));
           __asm__ ("pushq %0" : : "r" ((long) stack[5]));
           __asm__ ("pushq %0" : : "r" ((long) stack[6]));
           break;
        default: /* Hope 7 argument is worst case */
           break;
   	}
       __asm__ ("callq *funcptr");
       __asm__ ("movq %%rax, %0" : "=r" (return_rvalue));
   /* switch(n_stack) {
        case 1:
          __asm__ ("movq %0, %%rsp" : : "r" (saved_rsp));
          break;
        case 2:
         __asm__ ("movq %0, %%rsp" : : "r" (saved_rsp));
         //__asm__ ("movq %0, %%rbx" : : "r" (saved_rbx));
          break;
        case 3:
         __asm__ ("movq %0, %%rsp" : : "r" (saved_rsp));
        // __asm__ ("movq %0, %%rbx" : : "r" (saved_rbx));
         //__asm__ ("movq %0, %%rbp" : : "r" (saved_rbp));
          break;
        case 4:
         __asm__ ("movq %0, %%rsp" : : "r" (saved_rsp));
        // __asm__ ("movq %0, %%rbx" : : "r" (saved_rbx));
         //__asm__ ("movq %0, %%rbp" : : "r" (saved_rbp));
         //__asm__ ("movq %0, %%r12" : : "r" (saved_r12));
         break;
        case 5:
         __asm__ ("movq %0, %%rsp" : : "r" (saved_rsp));
        // __asm__ ("movq %0, %%rbx" : : "r" (saved_rbx));
         //__asm__ ("movq %0, %%rbp" : : "r" (saved_rbp));
         //__asm__ ("movq %0, %%r12" : : "r" (saved_r12));
         //__asm__ ("movq %0, %%r13" : : "r" (saved_r13));
         break;
        case 6:
         __asm__ ("movq %0, %%rsp" : : "r" (saved_rsp));
        // __asm__ ("movq %0, %%rbx" : : "r" (saved_rbx));
         //__asm__ ("movq %0, %%rbp" : : "r" (saved_rbp));
         //__asm__ ("movq %0, %%r12" : : "r" (saved_r12));
         //__asm__ ("movq %0, %%r13" : : "r" (saved_r13));
         //__asm__ ("movq %0, %%r14" : : "r" (saved_r14));
         break;
        case 7:
         __asm__ ("movq %0, %%rsp" : : "r" (saved_rsp));
         //__asm__ ("movq %0, %%rbx" : : "r" (saved_rbx));
         //__asm__ ("movq %0, %%rbp" : : "r" (saved_rbp));
         //__asm__ ("movq %0, %%r12" : : "r" (saved_r12));
         //__asm__ ("movq %0, %%r13" : : "r" (saved_r13));
         //__asm__ ("movq %0, %%r14" : : "r" (saved_r14));
         //__asm__ ("movq %0, %%r15" : : "r" (saved_r15));

         break;
        default:
           break;
   	}*/
/*
       __asm__ ("movq %0, %%rsp" : : "r" (saved_rsp));
       __asm__ ("movq %0, %%rbx" : : "r" (saved_rbx));
       __asm__ ("movq %0, %%rbp" : : "r" (saved_rbp));
       __asm__ ("movq %0, %%r12" : : "r" (saved_r12));
       __asm__ ("movq %0, %%r13" : : "r" (saved_r13));
       __asm__ ("movq %0, %%r14" : : "r" (saved_r14));
       __asm__ ("movq %0, %%r15" : : "r" (saved_r15));
*/
#else
/* Enable when debugging assembly code needed */
/*
  printf("Number of arguments =%d\n",n_stack);
  for (i=0; i < n_stack; i++)
    printf("Stack[%d]=%d\n",i,stack[i]);
*/
 
  //32 bit Linux or even less. Ok.
  __asm__ ("movl %%esp, %0" : "=r" (saved_esp));

// push the arguments to stack

   for (i=n_stack-1; i>=0; --i) {
        n = stack[i];
        __asm__ ("pushl %0" : : "r" (n));
   }
        __asm__ ("movl %0, %%esi" : : "r" (saved_esi)); 

	__asm__ ("movl %%esi, %0" : "=r" (saved_esi));
        __asm__ ("movl %0, %%edx" : : "r" (prov));
        __asm__ ("pushl %edx");
        __asm__("call *funcptr");
        __asm__ ("movl %%eax, %0" : "=r" (return_value));
        __asm__ ("movl %0, %%esi" : : "r" (saved_esi));
        __asm__ ("movl %0, %%esp": : "r" (saved_esp));

#endif // __x86_64__
    
    // call the function
    //state = (prov->*proc)();
    //printf("prov->*proc executed successfully in _Exec state=%d\n",state);


#if defined (__x86_64__)
   if (return_rvalue != 0) return OK;
   else
     return  NOT_OK;
#else //32 bit or less

   if (return_value != 0) return OK;
   else
     return  NOT_OK;
#endif

    /* return state; */
}


//-----------------------------------------------------------------------------
// Execute API by interpreter.
// Return NOT_OK here triggeres hHALT() for current task.
//
STATUS API_Provider_Manager::ExecuteAPICdecl( CAPIProvider *prov,
					    CAPIDef *func,
					    CTask *task,
					    int *data_seg,
					    int SP) 
{
    int i;
#if defined(__x86_64__)
    long ret_ptr, v[API_MAX_ARGS];
#else
    int  ret_ptr, v[API_MAX_ARGS];
#endif
    CAPIProvider::APIPROC proc = func->m_Proc;
    /* crashes are happening with stack corrupted*/
    memset(v,0,sizeof(v));

    int str_index = 0;

    if (!prov) {
		CLog::Error( GAP, task, "--RUN-TIME ERROR--: Provider not found for API '%s'", func->m_Name.c_str());
		return NOT_OK;
    }

    SP+=2;

    // deduce location of return pointer
    ret_ptr = SP + func->m_NStackArg;

    // get the argument value from task stack, and fill in the array
    for (i=func->m_NumOfArgument-1; i>=0; --i) {
		switch (func->m_ArgType[i]->Type()) {
			case CVariant::VT_TASK:
#if defined(__x86_64__)
				v[i] = (long)task;
#else
				v[i] = (int)task;
#endif
				break;
		
			case CVariant::VT_RET:
				if (func->m_RetType == CVariant::VT_STRING) {
				//v[i] = (int) ( data_seg + *(data_seg + ret_ptr));
				//strtab_ref[tempstr_idx] = *(data_seg + SP++);
				//strcpy(tempstr[tempstr_idx], task->GetExecContext()->m_stringtab[strtab_ref[tempstr_idx]].c_str());
				//v[i] = (int) &(tempstr[tempstr_idx]);	
#if defined(__x86_64__)
				v[i] = (long) task->GetExecContext()->CopyString(str_index, *(data_seg + SP++));
#else
                v[i] = (int) task->GetExecContext()->CopyString(str_index, *(data_seg + SP++));
		/* if (v[i] < 0 ) v[i] = 0; */
#endif
				if (v[i] == NULL) {
					// Fatal error has occured
					CLog::Fatal(GAP, task, "Fatal Error occured when calling API %s", func->m_Name.c_str());
					return NOT_OK;
				}
				str_index++;
			}
			else{
#if defined(__x86_64__)
				v[i] = (long) ( data_seg + ret_ptr);
#else
				v[i] = (int) ( data_seg + ret_ptr);
				/* if (v[i] < 0 ) v[i] = 0; */
#endif
			}
			break;

		default:

			if (func->m_ArgType[i]->m_PassingType == BYREF) {
				if (func->m_ArgType[i]->Type() == CVariant::VT_STRING) {
					//v[i] = (int) task->GetString(*(data_seg + SP++));
					//strtab_ref[tempstr_idx] = *(data_seg + SP++);
					//strcpy(tempstr[tempstr_idx], task->GetExecContext()->m_stringtab[strtab_ref[tempstr_idx]].c_str());
					//v[i] = (int) &(tempstr[tempstr_idx]);				
#if defined(__x86_64__)
					v[i] = (long) task->GetExecContext()->CopyString(str_index, *(data_seg + SP++));
#else
					v[i] = (int) task->GetExecContext()->CopyString(str_index, *(data_seg + SP++));
					/* if (v[i] < 0 ) v[i] = 0; */
#endif
					str_index++;
					if (v[i] == NULL) {
						// Fatal error has occured
						CLog::Fatal(GAP, task, "Fatal Error occured when calling API %s", func->m_Name.c_str());
						return NOT_OK;
					}

				}
				
				else {
#if defined(__x86_64__)
					v[i] = (long) (data_seg + *(data_seg + SP++));
#else
				    v[i] = (int) (data_seg + *(data_seg + SP++));
				    /* if (v[i] < 0 ) v[i] = 0; */
#endif
				}
			}
			else {
				if (func->m_ArgType[i]->Type() == CVariant::VT_STRING) {
					//v[i] = (int) task->GetExecContext()->m_stringtab[*(data_seg + SP++)].c_str();
#if defined(__x86_64__)
					v[i] = (long) task->GetExecContext()->GetStringTab_Checked(*(data_seg + SP++));
#else
					v[i] = (int) task->GetExecContext()->GetStringTab_Checked(*(data_seg + SP++));
					/* if (v[i] < 0 ) v[i] = 0; */
#endif
					if (v[i] == NULL) {
					// Fatal error has occured
					CLog::Fatal(GAP, task, "Fatal Error occured when calling API %s", func->m_Name.c_str());
					return NOT_OK;
					}
				}
				else {
					v[i] = *(data_seg + SP++);
					/* if (v[i] < 0 ) v[i] = 0; */
				}
			}
			break;
		}

    }

    // Trace before API
    if (func->m_Async)
	TraceAPICall( task, func, v, true);

    // Call timing
    bool is_api_profile_enabled = m_EnableAPIProfile;
    if (is_api_profile_enabled)
	m_Timer.start();

    // Execute
#ifdef KERNEL_RECORD_ACQUIRE_LOCK
    atomic_set (&Kernel::Instance()->currentService, func->m_Name.c_str());
#endif
	//printf("Function name=%s\n",func->m_Name.c_str());

	STATUS rc = _Exec( prov, proc, func->m_NumOfArgument, v);
    
#ifdef KERNEL_RECORD_ACQUIRE_LOCK
    atomic_set (&Kernel::Instance()->currentService, "Interpreter");
#endif

    // Get String return value
    if (task->GetState() != CTask::BLOCKED)
	task->GetExecContext()->GetStringReturnValue();

    // Call timing
    if (is_api_profile_enabled) {
	m_Timer.stop();
	prov->AddProviderTime(m_Timer.get_elapsed());
    }
    
    // handle if API returns NOT_OK
    if (rc == NOT_OK) {
	CLog::Error( GAP, task, "Function call '%s' has returned an error", 
			  func->m_Name.c_str());

	// Trace API call
	if (!func->m_Async) {
	    TraceAPICall( task, func, v, false);
	} else {
	    task->SaveFunc( func, v, func->m_NumOfArgument);
	}

	return OK;
    }

    // Trace API call
    if (!func->m_Async) {
	TraceAPICall( task, func, v, false);
    } else {
	task->SaveFunc( func, v, func->m_NumOfArgument);
    }

    return OK;
}


