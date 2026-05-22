/* $Header: /xGate/application.cpp 57    5/03/05 4:16p Bennylp $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * SL Application management.
 *
 * $Log: /xGate/application.cpp $
 * 
 * 57    5/03/05 4:16p Bennylp
 * Added warning if app is compiled with older compiler
 * 
 * 56    11/29/04 7:14p Bennylp
 * Added LM protection in RouteIncomingCall, just in case it is bypassed
 * 
 * 55    7/08/04 6:48p Bennylp
 * Call Gapping
 * 
 * 54    3/08/04 7:01p Bennylp
 * Added:
 * - Registrar API (support spawning task for incoming registration)
 * - Authentication API
 * 
 * 53    3/07/03 4:10p Bennylp
 * Changed tracing format to be parsed-able by trace front end.
 * 
 * 52    3/01/03 7:03p Bennylp
 * Remove App_String
 * 
 * 51    21/02/03 18:10 Junanto
 * added safe application string
 * 
 * 50    1/31/03 2:24p Bennylp
 * Changed command mode to user
 * 
 * 49    22/05/02 16:40 Junanto
 * Prevent error and informational message from
 * being displayed during configuration file loading
 * 
 * 48    1/21/02 11:44a Bennylp
 * Ver 38.2 (configurable use of app mem pool)
 * 
 * 47    12/19/01 4:26p Bennylp
 * Fixed bug: unable to load app when data size is zero
 * 
 * 46    25/10/01 11:20 Junanto
 * Fixed bug in free memory pool management
 * 
 * 45    24/10/01 15:12 Junanto
 * Tasks' memory and string are no longer deleted on task
 * destruction but returned to a free pool for later use
 * 
 * 44    16/10/01 18:38 Junanto
 * Fixed/optimized Task_Exec_Context initialization
 * 
 * 43    7/25/01 8:06p Bennylp
 * Added application tracing.
 * 
 * 42    7/25/01 11:40a Bennylp
 * Added CIR command, which is an alias for 'Call-In-Route' command
 * 
 * 41    7/17/01 11:29a Bennylp
 * Added 'show application brief | detail'
 * 
 * 40    7/14/01 7:53p Bennylp
 * Reduce friends of class Task_Exec_Context.
 * 
 * 39    7/13/01 9:23p Bennylp
 * Fix bug in Reload() doesn't clean images.
 * 
 * 38    13/07/01 17:14 Sonny
 * new show app
 * 
 * 37    7/13/01 11:38a Bennylp
 * Added command 'show cir' to display all Call In Routes.
 * 
 * 36    7/13/01 10:55a Adham
 * SNMP Framework integration
 * 
 * 35    12/07/01 18:01 Sonny
 * 
 * 34    10/07/01 19:06 Junanto
 * Changed interface of Call class constructor
 * 
 * 33    2/07/01 19:14 Junanto
 * Added group information during no match route
 * 
 * 32    28/06/01 16:49 Junanto
 * Added application global parameters
 * 
 * 31    28/06/01 12:49 Sonny
 * 
 * 30    28/06/01 11:40 Sonny
 * Fixed bug in Task_Exec_Context destructor in low mem situation.
 * 
 * 29    26/06/01 10:58 Junanto
 * Added new parameter in application "call-proceeding"
 * which indicates whether a PROCEEDING needs to
 * be sent automatically during incoming call or not
 * 
 * 28    8/06/01 10:37 Junanto
 * CallInRoute priority defaults to 32
 * 
 * 27    5/30/01 7:29a Sonny
 * Fixed recursive acquire system lock.
 * 
 * 26    5/26/01 11:56p Bennylp
 * Allocate TDM address from CTbusManager for ATM channel.
 * 
 * 25    5/26/01 8:04p Bennylp
 * Record each call to AcquireSystemLock in DEBUG mode.
 * 
 * 24    5/26/01 5:48p Bennylp
 * Fixed bug when app/call route entry is deleted.
 * 
 * 23    23/05/01 11:02 Junanto
 * 
 * 22    23/05/01 10:40 Junanto
 * 
 * 21    23/05/01 10:37 Junanto
 * 
 * 20    23/05/01 10:35 Junanto
 * Added warning if application parameter is
 * not defined during ParamGet/Set
 * 
 * 19    21/05/01 17:09 Junanto
 * Added group name in the call-in criteria
 * 
 * 18    10/05/01 12:40 Junanto
 * Replaced "\r\n" sequence to "\n". Telnet translates
 * an orphaned "\n" into "\r\n" during output
 * 
 * 17    10/05/01 11:13 Junanto
 * Replaced all "\r\n" with a constant
 * 
 * 16    10/05/01 11:12 Junanto
 * Replaced all "\r\n" with a constant
 * 
 * 15    8/05/01 19:43 Junanto
 * LoadApp implementation changed from
 * CreateFile to a more standard fopen
 * 
 * 14    8/05/01 13:10 Junanto
 * Added parameter max-string
 * 
 * 13    5/01/01 9:04a Sonny
 * String/stack statistics
 * 
 * 12    5/01/01 2:39a Sonny
 * applicaton statistics
 * 
 * 11    29/04/01 15:05 Junanto
 * added parameter need-voice in 
 * application to configure voice
 * resource requirement per 
 * application
 * 
 * 10    4/23/01 10:42p Bennylp
 * Fixed std namespace conflicts.
 * 
 * 9     4/19/01 2:15p Bennylp
 * Fix memory leaks.
 * 
 * 8     4/19/01 12:25p Bennylp
 * Add routing incoming calls.
 * 
 * 7     4/18/01 8:27p Bennylp
 * Added call-in-route configuration.
 * 
 * 6     4/18/01 5:55p Bennylp
 * Implement task cmdline argument.
 * 
 * 5     4/18/01 3:39p Bennylp
 * Fix bug in application shutdown not releasing images.
 * 
 * 4     4/17/01 10:06p Bennylp
 * Fixed bug in log message.
 * 
 * 3     4/17/01 12:31p Bennylp
 * Integrate icommon.h to interpret.h.
 * 
 * 2     4/12/01 8:00p Bennylp
 * Integration.
 * 
 * 1     4/11/01 10:30a Bennylp
 * Created.
 *
 */

//#include "stdafx.h"
#include "application.h"
#include "interpret.h"
#include "kernel.h"
#include "log.h"
#include "task.h"		// Task_Exec_Context
#include "term.h"
//#include "pcre-3.4/pcreposix.h"	// Regex
#include <regex.h>
#include "call.h"		// CallParameter
#include "klog.h"		// Tracing
#include <sys/stat.h>
#include <sys/types.h>

#ifdef XGATE_HAS_SIP
#include "registrar.h"		// Registration info.
#endif

#ifdef XGATE_HAS_LM
#include "licence.h"		// licence manager.
#endif

//#include "slapplicationmonitor.h"
#include "SLApplicationMonitor.h"

#define HOSTNAME_LENGTH 64
#define _snprintf   snprintf    //linux porting
#define __linux__

#pragma pack(1)
typedef struct {
    short   signature; 
    short   version;   
    int	    data_size;
    int	    code_size;
    int	    stack_size;
    int	    stringconst_size;
    int	    stringtab_size;
    int	    string_index;
} TSLHEADER;


typedef struct {
    time_t datetime;
    char hostname[HOSTNAME_LENGTH];
} TSLTRAILER;
#pragma pack()

CT_App_Manager *CT_App_Manager::appManagerInstance;

Log_Trace   *appTrace;

#define SYS			"SYS"
#define NEWLINE			"\n"
#define DEF_STACK_SIZE		(8 * 1024)
#define DEF_MAX_STRINGS		(1024)
#define CIR_FILTER_ITEM_LEN	128

//_CRTIMP int __cdecl stat(const char *, struct stat *);
extern "C" int stat(const char *, struct stat *);	//linux porting

///////////////////////////////////////////////////////////////////////////////
/** This class describes an instance of SL application image.
    An application normally only has one instance of SL application image at
    any given time, but it MAY have multiple instances if the application
    has been reloaded; in this case, the CT_Application() will wait until all tasks 
    that use the old image to quit, then unload/ the image.
 */
class CT_App_Image {
public:

    typedef List<char*> MemoryPool;
    Safe_Memory_Allocator allocator_;

    CT_Application* Application;

    CT_App_Image()
    {
	m_code  = NULL;
	m_data  = NULL;
	m_codesize  = 0;
	m_datasize  = 0;
	m_stacksize = 0;
	m_RefCount = 0;
	m_stringtab_size = 0;
	m_minstridx = 0;
	m_stringidx = 0;
	m_debug_level = 0;
	m_const_string = 0;
    }

    ~CT_App_Image()
    {
	if (m_code) delete[] m_code;
	if (m_data) delete[] m_data;

	m_code = NULL;
	m_data = NULL;

	if (m_const_string) {
	    for (int i=0; i<m_minstridx; i++)
		if (m_const_string[i])
		    delete m_const_string[i];
	    delete [] m_const_string;
	}
    }

    void SetDebugLevel(size_t dbg_lvl) {
	m_debug_level = dbg_lvl;
    }
    size_t GetDebugLevel() {
	return m_debug_level;
    }

    void  SetCode (TMEM *code, size_t size)
    {
	m_code = code;
	m_codesize = size;
    }

    void  SetData(TMEM *data,int data_size,int stack_size)
    {
	m_data     = data;
	m_datasize = data_size;
	m_stacksize= stack_size;
    }

    void  SetStringTab(char* strconst, int size, int index)
    {
	int i, offset, len; 
	int* pint;
	char* pstr;
	
	m_stringidx = index;
	m_minstridx = index;
	m_stringtab_size = size;

	m_const_string = new App_String*[index];
	memset(m_const_string, 0, index*sizeof(App_String*));

	offset = 0;
	pint = (int*)(strconst + offset);
	for (i = 0; i < size; i++) {
	    if (i == *pint) {
		offset += sizeof(int);
		pstr = &strconst[offset];
		len = strlen(pstr);
		offset += (len + 1);

		m_const_string[i] = new App_String(pstr);

		pint = (int*)(strconst + offset);
		if (*pint == -1) break;
	    }
	}
    }

    void InitStatistics() 
    {
	m_StatAllString = 0;
	Application->SetStatMaxString(0);
	Application->SetStatMaxAllString(0);
	Application->SetStatMaxSStack(0);

    }

    const TMEM* GetCode() const { return m_code; }
    const TMEM* GetData() const { return m_data; }

    size_t GetStringTabSize() const { return m_stringtab_size; }
    int  GetStringIndex() const { return m_stringidx; }

    App_String** GetConstantString() const { return m_const_string; }
    
    int  GetStatAllString() const { return m_StatAllString; }

    void AddRef()		{ ++m_RefCount;  }
    void SubRef()		{ --m_RefCount;  }
    size_t GetRefCount() const	{ return m_RefCount; }
    size_t GetDataSize() const  { return m_datasize; }
    size_t GetStackSize() const { return m_stacksize; }

private:
    TMEM   *m_code,*m_data;
    size_t  m_codesize,m_datasize,m_stacksize;

    //add info for max string size
    size_t  m_stringtab_size;
    //App_String* m_stringtab;
    size_t  m_stringidx;
    size_t  m_minstridx;
    size_t m_debug_level;

    size_t  m_StatAllString;	// all string used

    String  m_name;
    
    // reference count: how many task use this image
    size_t  m_RefCount;

    // application constant string storage
    App_String** m_const_string;
};

/** AppGlobalParameter: This class is responsible for maintaining application
    global parameters
 */
class AppGlobalParameter : public Term_Cmd_Target {
public:

    /** Default constructor
     */
    AppGlobalParameter() {
	_cfgTree = new Cfg_Tree_Group("global-parameter","Global application parameters",Kernel::Instance()->RootConfig());

	// cmd: 'param <name> <value>'
	Term_Cmd *cmd = new Term_Cmd(
	    this,
	    Term_Cmd_App_Param,
	    "param", 
	    "Set application specific parameters",
	    PrivilegeLevelAdmin,
	    TermModeConfig,
	    true);

	cmd->Add(new Term_Cmd_Arg("name",CVariant(""),CVariantValidator("","Parameter name")));
	cmd->Add(new Term_Cmd_Arg("value",CVariant(""),CVariantValidator("","Parameter value")));
	RegisterTermCmd(cmd,_cfgTree);

	// Register show handler.
	_cfgTree->RegisterShowHandler(this);
    }

    /** destructor
     */
    virtual ~AppGlobalParameter() {
	// do nothing
    }

    /** Get config tree.
     */
    Cfg_Tree_Group* GetConfigTree() {
	return _cfgTree;
    }

    /** Get application parameter.
	Return false if the parameter can't be found.
     */
    bool GetAppParam(const IString& name, String& value) const {
	Map <IString, String>::const_iterator it;
	it = _appParam.find (name);
	if (it == _appParam.end()) {
	    value.erase();
	    return false;
	}
	value = it->second;
	return true;
    }

    /** Set application param.
	Return false if the parameter can't be found.
     */
    bool SetAppParam(const IString& name, const String& value) {
	Map <IString, String>::iterator it;
	it = _appParam.find (name);
	if (it == _appParam.end()) {
	    return false;
	}
	it->second = value;
	return true;
    }

    /** Show configuration for the specified entry. This function will be called by the 
	framework whenever it needs to display the specified entry.
     */
    virtual bool OnShowConfig(const Cfg_Tree_Entry*,String& output,const String& indent,int indent_size) const {
	// parameters
	Map <IString, String>::const_iterator it;
	for (it = _appParam.begin(); it != _appParam.end(); ++it)
	    output += indent + "param " + it->first + " \"" + it->second + "\"" + NEWLINE;

	return OK;
    }

private:
    enum {
	Term_Cmd_App_Param,
    };

    /** Add a new application param.
	Return false if the parameter already exists.
     */
    bool AddAppParam(const IString& name, const String& value) {
	Map <IString, String>::iterator it;
	it = _appParam.find (name);
	if (it == _appParam.end()) {
	    it = _appParam.insert (it, std::pair<IString, String>(name, value));
	    return true;
	}
	
	// param already exist
	return false;
    }

    /** Delete an application param.
	Return false if the parameter can't be found.
     */
    bool DelAppParam(const IString& name) {
	Map <IString, String>::iterator it;
	it = _appParam.find (name);
	if (it != _appParam.end()) {
	    _appParam.erase(it);
	    return true;
	}
	
	// param not found
	return false;
    }

    /** Set application param, or add a new one if it does not exist.
     */
    bool SetAddAppParam ( const IString & name, const String & value ) {
	if (!AddAppParam (name, value))
	    return SetAppParam(name, value);
	return true;
    }

    /** Terminal command callback
    */
    void DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd) {
	CVariant v1, v2;
	cmd->GetArg((size_t)0)->GetValue(v1);
	cmd->GetArg(1)->GetValue(v2);
	IString pname = v1.ToString();
	
	switch (cmd->GetId()) {
	case Term_Cmd_App_Param:
	    if (!ACQUIRE_SYSTEM_LOCK())
		return;

	    if (no) {
		if (DelAppParam(pname) == false)
		    session->SendErrorf ( "Parameter '%s' not found" NEWLINE,  pname.c_str());
	    } else {
		if (SetAddAppParam(pname, v2.ToString()) == false)
		    session->SendErrorf ( "Can not set/add parameter '%s'" NEWLINE,  pname.c_str());
	    }
	    RELEASE_SYSTEM_LOCK();
	    break;
	}
    }
    
    Map <IString, String> _appParam;
    Cfg_Tree_Group* _cfgTree;
};

///////////////////////////////////////////////////////////////////////////////
/** This class describes call-in-route.
 */
class Call_In_Route : public Term_Cmd_Target {
public:
    /** Default constructor.
     */
    Call_In_Route(const String & name);

    /** Destructor.
     */
    ~Call_In_Route();

    /** Initialize.
     */
    bool Init ( Cfg_Tree_Group *parent );

    /** Check whether this route is shutdown.
     */
    bool IsShutdown() const {
	return isShutdown;
    }

    /** Match the incoming call to the specification.
	Return true if match.
     */
    bool Match ( Task_Startup_Type type,
		 const String & in_ddi,	    // also used by http-server
		 const String & in_cli,	    // also used by http-method
		 const String & port_in,    // also used by http-url
		 const String & group,
		 const String & reg_domain,
		 const String & reg_method,
		 int smpp_command_id = 0,
		 const String & smpp_link_name = "",
		 const String & diameter_link_name = "",
		 const String & diameter_peer_realm = "");

    /** Get the route name.
     */
    const String & GetName() const {
	return name;
    }

    /** Get the application name.
     */
    const String & GetAppName() const {
	return appName;
    }

    /** Get the spawn-task arguments.
     */ 
    const List<String> & GetAppArg() const {
	return appArg;
    }

    /** Get the priority.
     */
    int GetPriority() const {
	return priority;
    }

    /** Get the DDI spec. 
     */
    const String & GetDDI() const {
	return ddiSpec;
    }

    /** Get the CLI spec.
     */
    const String & GetCLI() const {
	return cliSpec;
    }

    /** Get the port/interface spec.
     */
    const String & GetInterface() const {
	return portInSpec;
    }

    /** Get the group spec.
     */
    const String & GetGroup() const {
	return groupSpec;
    }

    /** Get Route type.
     */
    Task_Startup_Type GetRouteType() const {
	return routeType;
    }

    /** Get registration domain.
     */
    const String & GetRegDomain() const {
	return regDomain;
    }

    /** Get registration method.
     */
    const String & GetRegMethod() const {
	return regMethod;
    }

    const unsigned GetCallGap() const {
	return call_gap_msec;
    }

    /** Get the spawn spec.
     */
    String GetSpawn() const {
	String spawn = appName;
	List <String>::const_iterator it, end;
	end = appArg.end();
	for (it = appArg.begin(); it!=end; ++it) {
	    spawn += " " + *it;
	}
	return spawn;
    }

    /**
     Check call gap restriction allows this route
     **/
    bool CallGapInForce() {

	// no call gap
	if (call_gap_msec == 0)
	    return false;

	// if the new call arrives after minimum interval, then gap the call
	tm_last_routed.stop();
	unsigned long interval = tm_last_routed.get_elapsed_msec();
	if (interval < call_gap_msec)
	    return true;

	// greater than minimum interval, no call gap
	tm_last_routed.start();
	return false;
    }
	

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
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const;

    /** Get the config tree.
     */
    Cfg_Tree_Group * GetConfigTree() {
	return configTree;
    }

    /** Show configuration entry without acquiring system lock.
     */
    bool OnShowConfig_NoLock(
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const;

    unsigned usage_counter;

private:
    String  name;
    String  ddiSpec;
    String  cliSpec;
    String  groupSpec;
    String  portInSpec;
    Task_Startup_Type routeType;
    String  regDomain;
    String  regMethod;
    
    String  smppLink;
    String  smppMsg;
    String  diameterLink;
    String  diameterPeerRealm;

    int     priority;
    bool    isShutdown;
    //Low_Res_Timer  tm_last_routed;
    High_Res_Timer  tm_last_routed;
    long call_gap_msec;
    Cfg_Tree_Group *configTree;
    String  appName;
    List <String> appArg;
    regex_t ddiRegex, cliRegex, portInRegex, groupRegex, regDomainRegex, regMethodRegex;
    regex_t smppLinkRegex, diameterLinkRegex, diameterPeerRealmRegex;

    enum {
	CIR_Cmd_Shutdown,
	CIR_Cmd_Ddi,
	CIR_Cmd_Cli,
	CIR_Cmd_Interface,
	CIR_Cmd_SpawnTask,
	CIR_Cmd_Priority,
	CIR_Cmd_Group,
	CIR_Cmd_Route_Type,
	CIR_Cmd_Reg_Domain,
	CIR_Cmd_Reg_Method,
	CIR_Cmd_Call_Gap,
	CIR_Cmd_Smpp_Link,
	CIR_Cmd_Smpp_Message,
	CIR_Cmd_Diameter_Link,
	CIR_Cmd_Diameter_PeerRealm,
    };

    /* not accessible */
    Call_In_Route ( const Call_In_Route & ) { assert(false); }

    /* not accessible */
    const Call_In_Route & operator = (const Call_In_Route & ) {
	assert(false);
	return *this;
    }
};


/** Default constructor.
 */
Call_In_Route::Call_In_Route(const String & nm)
: name(nm), ddiSpec(".*"), cliSpec(".*"), portInSpec(".*"), 
  routeType(TASK_STARTUP_CALL), regDomain(".*"), regMethod(".*"),
  groupSpec(".*"), isShutdown(true), configTree(NULL), usage_counter(0),
  priority(32), smppLink(".*"), diameterLink(".*")
{
    memset(&ddiRegex, 0, sizeof(ddiRegex));
    memset(&cliRegex, 0, sizeof(cliRegex));
    memset(&portInRegex, 0, sizeof(portInRegex));
    memset(&groupRegex, 0, sizeof(groupRegex));
    memset(&regDomainRegex, 0, sizeof(regDomainRegex));
    memset(&regMethodRegex, 0, sizeof(regMethodRegex));
    memset(&smppLinkRegex, 0, sizeof(smppLinkRegex));
    memset(&diameterLinkRegex, 0, sizeof(diameterLinkRegex));
    memset(&diameterPeerRealmRegex, 0, sizeof(diameterPeerRealmRegex));

    if (regcomp(&ddiRegex, ddiSpec.c_str(), REG_ICASE | REG_EXTENDED)) {
	goto on_error;
    }
    if (regcomp(&cliRegex, cliSpec.c_str(), REG_ICASE | REG_EXTENDED)) {
	goto on_error;
    }
    if (regcomp(&portInRegex, portInSpec.c_str(), REG_ICASE | REG_EXTENDED)) {
	goto on_error;
    }
    if (regcomp(&groupRegex, groupSpec.c_str(), REG_ICASE | REG_EXTENDED)) {
	goto on_error;
    }
    if (regcomp(&regDomainRegex, regDomain.c_str(), REG_ICASE | REG_EXTENDED)) {
	goto on_error;
    }
    if (regcomp(&regMethodRegex, regMethod.c_str(), REG_ICASE | REG_EXTENDED)) {
	goto on_error;
    }
    if (regcomp(&smppLinkRegex, smppLink.c_str(), REG_ICASE | REG_EXTENDED)) {
	goto on_error;
    }
    if (regcomp(&diameterLinkRegex, diameterLink.c_str(), REG_ICASE | REG_EXTENDED)) {
	goto on_error;
    }

    tm_last_routed.init();
    tm_last_routed.start();
    call_gap_msec = 0;

    return;

on_error:
    regfree(&ddiRegex);
    regfree(&cliRegex);
    regfree(&portInRegex);
    regfree(&groupRegex);
    regfree(&regDomainRegex);
    regfree(&regMethodRegex);
    regfree(&smppLinkRegex);
    regfree(&diameterLinkRegex);

    memset(&ddiRegex, 0, sizeof(ddiRegex));
    memset(&cliRegex, 0, sizeof(cliRegex));
    memset(&portInRegex, 0, sizeof(portInRegex));
    memset(&groupRegex, 0, sizeof(groupRegex));
    memset(&regDomainRegex, 0, sizeof(regDomainRegex));
    memset(&regMethodRegex, 0, sizeof(regMethodRegex));
    memset(&smppLinkRegex, 0, sizeof(smppLinkRegex));
    memset(&diameterLinkRegex, 0, sizeof(diameterLinkRegex));
}


/** Destructor.
 */
Call_In_Route::~Call_In_Route()
{
    delete configTree;
    regfree(&ddiRegex);
    regfree(&cliRegex);
    regfree(&portInRegex);
    regfree(&groupRegex);
    regfree(&regDomainRegex);
    regfree(&regMethodRegex);
    regfree(&smppLinkRegex);
    regfree(&diameterLinkRegex);
}


/** Initialize.
 */
bool Call_In_Route::Init ( Cfg_Tree_Group *parent )
{
    NEW_RETURN_ (configTree, Cfg_Tree_Group(name, "", parent));
    if (configTree == NULL)
	return false;

    Term_Cmd *cmd_shut = NULL,
	     *cmd_route_type = NULL,
	     *cmd_ddi = NULL,
	     *cmd_cli = NULL,
	     *cmd_portin = NULL,
	     *cmd_interface = NULL,
	     *cmd_group = NULL,
	     *cmd_spawn = NULL,
	     *cmd_reg_domain = NULL,
	     *cmd_reg_method = NULL,
	     *cmd_prio = NULL,
	     *cmd_cgap = NULL;

#ifdef XGATE_HAS_DIAMETER
    Term_Cmd *cmd_diameter_link = 0;
    Term_Cmd *cmd_diameter_peer_realm = 0;
#endif

    try {
	// 'shutdown' command
	cmd_shut = new Term_Cmd (this, 
				 CIR_Cmd_Shutdown, 
				 "shutdown", 
				 "Disable this route", 
				 PrivilegeLevelAdmin, 
				 TermModeConfig, 
				 true);

	// 'route-type' command
	cmd_route_type = new Term_Cmd (this,
				       CIR_Cmd_Route_Type,
				       "route-type",
				       "Set the route type",
				       PrivilegeLevelAdmin,
				       TermModeConfig,
				       true);
	CVariantChoiceValidator route_type_vdtor("call", "Route for incoming call (default)");
	route_type_vdtor.Add("registration", "Route for incoming registration request");
#ifdef XGATE_HAS_DIAMETER
	route_type_vdtor.Add("diameter", "Route for incoming Diameter message");
#endif
	cmd_route_type->Add( new Term_Cmd_Arg("type", "call", route_type_vdtor) );


	// 'ddi' command
	cmd_ddi = new Term_Cmd (this,
				CIR_Cmd_Ddi,
				"ddi",
				"Destination number specification",
				PrivilegeLevelAdmin,
				TermModeConfig,
				true);
	CVariantValidator numVdtor(CVariant(""), "Number specification (regular expression)");
	cmd_ddi->Add ( new Term_Cmd_Arg( "spec", CVariant(""), numVdtor) );

	// 'cli' command
	cmd_cli = new Term_Cmd (this,
				CIR_Cmd_Cli,
				"cli",
				"Caller number specification",
				PrivilegeLevelAdmin,
				TermModeConfig,
				true);
	cmd_cli->Add ( new Term_Cmd_Arg( "spec", CVariant(""), numVdtor) );

	// 'port-in' command (!!! obsolete !!!)
	cmd_portin = new Term_Cmd (this,
				   CIR_Cmd_Interface,
				   "port-in",
				   "Incoming port name specification",
				   PrivilegeLevelAdmin,
				   TermModeConfig,
				   true);
	cmd_portin->Add ( new Term_Cmd_Arg("spec", CVariant(""), 
					   CVariantValidator(CVariant(""), "Port name specification (regular expression)")) );

	// 'interface' command
	cmd_interface = new Term_Cmd (this,
				   CIR_Cmd_Interface,
				   "interface",
				   "Incoming interface name specification",
				   PrivilegeLevelAdmin,
				   TermModeConfig,
				   true);
	cmd_interface->Add ( new Term_Cmd_Arg("spec", CVariant(""), 
					   CVariantValidator(CVariant(""), "Interface name specification (regular expression)")) );

	// 'group' command
	cmd_group = new Term_Cmd (this,
				   CIR_Cmd_Group,
				   "group",
				   "Incoming group name specification",
				   PrivilegeLevelAdmin,
				   TermModeConfig,
				   true);
	cmd_group->Add ( new Term_Cmd_Arg("spec", CVariant(""), 
					   CVariantValidator(CVariant(""), "Group name specification (regular expression)")) );

	// 'priority' command
	cmd_prio = new Term_Cmd (  this,
				   CIR_Cmd_Priority,
				   "priority",
				   "Priority of this route (higher priority will be matched first)",
				   PrivilegeLevelAdmin,
				   TermModeConfig,
				   true);
	CVariantRangeValidator prioVdtor(1, 1000, "Priority (higher priority will be matched first)");
	cmd_prio->Add ( new Term_Cmd_Arg("prio", CVariant(1), prioVdtor) );

	// 'call-gap' command
	cmd_cgap = new Term_Cmd (  this,
				   CIR_Cmd_Call_Gap,
				   "call-gap",
				   "Minimum interval between 2 consecutive calls (in msec)",
				   PrivilegeLevelAdmin,
				   TermModeConfig,
				   true);
	CVariantRangeValidator cgapVdtor(0, 10000, "Interval in msec");
	cmd_cgap->Add ( new Term_Cmd_Arg("interval", CVariant(1), cgapVdtor) );

	// 'registration-domain' command.
	cmd_reg_domain = new Term_Cmd ( this,
					CIR_Cmd_Reg_Domain,
					"registration-domain",
					"Set the regular expression to match registration domain",
					PrivilegeLevelAdmin,
					TermModeConfig,
					true);
	cmd_reg_domain->Add( new Term_Cmd_Arg("reg-domain", CVariant(""), CVariantValidator("", "Regular expression")) );

	// 'registration-method' command.
	cmd_reg_method = new Term_Cmd ( this,
					CIR_Cmd_Reg_Method,
					"registration-method",
					"Regular expression to match method (method=REGISTER)",
					PrivilegeLevelAdmin,
					TermModeConfig,
					true);
	cmd_reg_domain->Add( new Term_Cmd_Arg("reg-method", CVariant(""), CVariantValidator("", "Regular expression")) );

#ifdef XGATE_HAS_DIAMETER
	// 'diameter-link' command.
	cmd_diameter_link = new Term_Cmd ( this,
					CIR_Cmd_Diameter_Link,
					"diameter-link",
					"Set the link for diameter",
					PrivilegeLevelAdmin,
					TermModeConfig,
					true);
	cmd_diameter_link->Add( new Term_Cmd_Arg("diameter-link", CVariant(""), CVariantValidator("", "Link name of Diameter")) );

	// 'diameter-peer-realm' command.
	cmd_diameter_peer_realm = new Term_Cmd ( this,
					CIR_Cmd_Diameter_PeerRealm,
					"diameter-peer-realm",
					"Set the peer realm for diameter",
					PrivilegeLevelAdmin,
					TermModeConfig,
					true);
	cmd_diameter_peer_realm->Add( new Term_Cmd_Arg("diameter-peer-realm", CVariant(""), CVariantValidator("", "Peer realm name of Diameter")) );
#endif

	// 'spawntask' command
	cmd_spawn = new Term_Cmd (this,
				   CIR_Cmd_SpawnTask,
				   "spawntask",
				   "Specification of application to be executed and its arguments",
				   PrivilegeLevelAdmin,
				   TermModeConfig,
				   true);
	CVariantValidator appVdtor(CVariant(""), "Application name");
	cmd_spawn->Add ( new Term_Cmd_Arg("app", CVariant(""), appVdtor) );
	cmd_spawn->Add ( new Term_Cmd_Arg("arg1", CVariant(""), CVariantValidator(CVariant(""), "Argument 1"), TermArgOptional) );
	cmd_spawn->Add ( new Term_Cmd_Arg("arg2", CVariant(""), CVariantValidator(CVariant(""), "Argument 2"), TermArgOptional) );
	cmd_spawn->Add ( new Term_Cmd_Arg("arg3", CVariant(""), CVariantValidator(CVariant(""), "Argument 3"), TermArgOptional) );
	cmd_spawn->Add ( new Term_Cmd_Arg("arg4", CVariant(""), CVariantValidator(CVariant(""), "Argument 4"), TermArgOptional) );

    } catch (...) {
	CLog::Error ( "SYS", "System memory allocation failed in Call_In_Route::Init()");
	goto on_error;
    }

    RegisterTermCmd(cmd_shut, configTree);
    RegisterTermCmd(cmd_route_type, configTree);
    RegisterTermCmd(cmd_ddi, configTree);
    RegisterTermCmd(cmd_cli, configTree);
    RegisterTermCmd(cmd_portin, configTree);
    RegisterTermCmd(cmd_interface, configTree);
    RegisterTermCmd(cmd_group, configTree);
    RegisterTermCmd(cmd_prio, configTree);
    RegisterTermCmd(cmd_cgap, configTree);
    RegisterTermCmd(cmd_spawn, configTree);
    RegisterTermCmd(cmd_reg_domain, configTree);
    RegisterTermCmd(cmd_reg_method, configTree);
#ifdef XGATE_HAS_DIAMETER
    RegisterTermCmd(cmd_diameter_link, configTree);
    RegisterTermCmd(cmd_diameter_peer_realm, configTree);
#endif
    configTree->RegisterShowHandler (this);
    return true;

on_error:
    delete cmd_shut;
    delete cmd_route_type;
    delete cmd_ddi;
    delete cmd_cli;
    delete cmd_portin;
    delete configTree;
    configTree = NULL;
    return false;
}


/** Match the incoming call to the specification.
    Return true if match.
 */
bool Call_In_Route::Match ( Task_Startup_Type type,
			    const String & in_ddi,
			    const String & in_cli,
			    const String & port_in,
			    const String & group,
			    const String & reg_domain,
			    const String & reg_method,
			    int smpp_command_id,
			    const String & smpp_link_name,
			    const String & diameter_link_name,
			    const String & diameter_peer_realm)
{
    assert (isShutdown == false);

    if (type == TASK_STARTUP_CALL) {
	// Match DDI
	if (regexec(&ddiRegex, in_ddi.c_str(), 0, NULL, 0) == REG_NOMATCH)
	    return false;

	// Match CLI
	if (regexec(&cliRegex, in_cli.c_str(), 0, NULL, 0) == REG_NOMATCH)
	    return false;

	// Match port name
	if (regexec(&portInRegex, port_in.c_str(), 0, NULL, 0) == REG_NOMATCH)
	    return false;

	// Match group name
	if (regexec(&groupRegex, group.c_str(), 0, NULL, 0) == REG_NOMATCH)
	    return false;

    } else if (type == TASK_STARTUP_REGISTRATION) {
	// Match registration domain.
	if (regexec(&regDomainRegex, reg_domain.c_str(), 0, NULL, 0) == REG_NOMATCH)
	    return false;

	// Match registration method.
	if (regexec(&regMethodRegex, reg_method.c_str(), 0, NULL, 0) == REG_NOMATCH)
	    return false;

#ifdef XGATE_HAS_DIAMETER
    } else if (type == TASK_STARTUP_DIAMETER) {
	// Match link name
	if (regexec(&diameterLinkRegex, diameter_link_name.c_str(), 0, NULL, 0) == REG_NOMATCH)
	    return false;
	// Match peer realm name
	if (regexec(&diameterPeerRealmRegex, diameter_peer_realm.c_str(), 0, NULL, 0) == REG_NOMATCH)
	    return false;
#endif
    } else {
	assert(!"Unsupported route type");
    }

    // match
    return true;
}


/**
 Terminal command callback
 */
void Call_In_Route::DoTermCmd (Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant v1;
    String temp;
    regex_t temp_regex;

    switch (cmd->GetId()) {
    case CIR_Cmd_Shutdown:
	if (appName.empty()) {
	    session->SendErrorf ("spawntask must be specified" NEWLINE);
	    return;
	}
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	isShutdown = !no;
	RELEASE_SYSTEM_LOCK();
	break;

    case CIR_Cmd_Route_Type:
	cmd->GetArg((size_t)0)->GetValue(v1);

	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	temp = v1.ToString();
	if (temp == "call")
	    routeType = TASK_STARTUP_CALL;
	else if (temp == "registration")
	    routeType = TASK_STARTUP_REGISTRATION;
	else if (temp == "smpp")
	    routeType = TASK_STARTUP_SMPP;
	else if (temp == "diameter")
	    routeType = TASK_STARTUP_DIAMETER;
	else 
	    assert(!"Unsupported route type");

	RELEASE_SYSTEM_LOCK();
	break;

    case CIR_Cmd_Ddi:
	cmd->GetArg((size_t)0)->GetValue(v1);
	temp = v1.ToString();
	if (regcomp(&temp_regex, temp.c_str(), REG_ICASE | REG_EXTENDED | REG_NEWLINE)) {
	    session->SendErrorf ("Invalid regular expression" NEWLINE);
	    return;
	}
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	regfree(&ddiRegex);
	ddiRegex = temp_regex;
	ddiSpec = temp;
	RELEASE_SYSTEM_LOCK();
	break;
    case CIR_Cmd_Cli:
	cmd->GetArg((size_t)0)->GetValue(v1);
	temp = v1.ToString();
    //printf("String=%s, size=%d\n",temp.c_str(),temp.size());
	if (regcomp(&temp_regex, temp.c_str(), REG_ICASE | REG_EXTENDED)) {
	    session->SendErrorf ("Invalid regular expression" NEWLINE);
	    return;
	}
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	regfree(&cliRegex);
	cliRegex = temp_regex;
    //printf(" cliSpec size =%d, temp size =%d,%d\n", cliSpec.size(),temp.size());
	cliSpec = temp;
	RELEASE_SYSTEM_LOCK();
	break;
    case CIR_Cmd_Interface:
	cmd->GetArg((size_t)0)->GetValue(v1);
	temp = v1.ToString();
	if (regcomp(&temp_regex, temp.c_str(), REG_ICASE | REG_EXTENDED)) {
	    session->SendErrorf ("Invalid regular expression" NEWLINE);
	    return;
	}
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	regfree(&portInRegex);
	portInRegex = temp_regex;
	portInSpec = temp;
	RELEASE_SYSTEM_LOCK();
	break;
    case CIR_Cmd_Group:
	cmd->GetArg((size_t)0)->GetValue(v1);
	temp = v1.ToString();
	if (regcomp(&temp_regex, temp.c_str(), REG_ICASE | REG_EXTENDED)) {
	    session->SendErrorf ("Invalid regular expression" NEWLINE);
	    return;
	}
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	regfree(&groupRegex);
	groupRegex = temp_regex;
	groupSpec = temp;
	RELEASE_SYSTEM_LOCK();
	break;

    case CIR_Cmd_Reg_Domain:
	cmd->GetArg((size_t)0)->GetValue(v1);
	temp = v1.ToString();
	if (regcomp(&temp_regex, temp.c_str(), REG_ICASE | REG_EXTENDED)) {
	    session->SendErrorf ("Invalid regular expression" NEWLINE);
	    return;
	}
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	regfree(&regDomainRegex);
	regDomainRegex = temp_regex;
	regDomain = temp;
	RELEASE_SYSTEM_LOCK();
	break;

    case CIR_Cmd_Reg_Method:
	cmd->GetArg((size_t)0)->GetValue(v1);
	temp = v1.ToString();
	if (regcomp(&temp_regex, temp.c_str(), REG_ICASE | REG_EXTENDED)) {
	    session->SendErrorf ("Invalid regular expression" NEWLINE);
	    return;
	}
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	regfree(&regMethodRegex);
	regMethodRegex = temp_regex;
	regMethod = temp;
	RELEASE_SYSTEM_LOCK();
	break;

#ifdef XGATE_HAS_DIAMETER
    case CIR_Cmd_Diameter_Link:
	cmd->GetArg((size_t)0)->GetValue(v1);
	temp = v1.ToString();
	if (regcomp(&temp_regex, temp.c_str(), REG_ICASE | REG_EXTENDED)) {
	    session->SendErrorf ("Invalid regular expression" NEWLINE);
	    return;
	}
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	regfree(&diameterLinkRegex);
	diameterLinkRegex = temp_regex;
	diameterLink = temp;
	RELEASE_SYSTEM_LOCK();
	break;

    case CIR_Cmd_Diameter_PeerRealm:
	cmd->GetArg((size_t)0)->GetValue(v1);
	temp = v1.ToString();
	if (regcomp(&temp_regex, temp.c_str(), REG_ICASE | REG_EXTENDED)) {
	    session->SendErrorf ("Invalid regular expression" NEWLINE);
	    return;
	}
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	regfree(&diameterPeerRealmRegex);
	diameterPeerRealmRegex = temp_regex;
	diameterPeerRealm = temp;
	RELEASE_SYSTEM_LOCK();
	break;
#endif

    case CIR_Cmd_SpawnTask:
	if (no) {
	    appName.erase();
	    appArg.clear();
	} else {
	    List <String> cmdline;
	    for (unsigned i=1; i<cmd->GetArgCount(); ++i) {
		Term_Cmd_Arg *arg = cmd->GetArg(i);
		if (arg->HasValue()) {
		    CVariant param1;
		    arg->GetValue(param1);
		    cmdline.push_back(param1.ToString());
		}
	    }

	    cmd->GetArg((size_t)0)->GetValue(v1);
	    temp = v1.ToString();

	    if (!ACQUIRE_SYSTEM_LOCK())
		return;
	    appName = temp;
	    appArg = cmdline;
	    RELEASE_SYSTEM_LOCK();
	}
	break;

    case CIR_Cmd_Priority:
	cmd->GetArg((size_t)0)->GetValue(v1);
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	priority = v1.AsInt();
	CT_App_Manager::Instance()->CIR_Resort (this);
	RELEASE_SYSTEM_LOCK();
	break;
    
    case CIR_Cmd_Call_Gap:
	cmd->GetArg((size_t)0)->GetValue(v1);
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	call_gap_msec = v1.AsInt();
	CT_App_Manager::Instance()->CIR_Resort (this);
	RELEASE_SYSTEM_LOCK();
	break;
    }
}


/**
 Show configuration for the specified entry.
 */
bool Call_In_Route::OnShowConfig_NoLock( const Cfg_Tree_Entry *entry,
					   String & output, 
					   const String & indent, 
					   int indent_size) const
{
    if (this->routeType == TASK_STARTUP_CALL) {
	// CLI
	output += indent + "cli \"" + cliSpec + "\"" + NEWLINE;

	//DDI
	output += indent + "ddi \"" + ddiSpec + "\"" + NEWLINE;

	// Port-in
	output += indent + "interface \"" + portInSpec + "\"" + NEWLINE;

	// Group
	output += indent + "group \"" + groupSpec + "\"" + NEWLINE;

    } else if (this->routeType == TASK_STARTUP_REGISTRATION) {
	// registration domain.
	output += indent + "registration-domain \"" + regDomain + "\"" + NEWLINE;

	// registration method.
	output += indent + "registration-method \"" + regMethod + "\"" + NEWLINE;

    } else if (this->routeType == TASK_STARTUP_SMPP) {
	// smpp link name.
	output += indent + "smpp-link \"" + smppLink + "\"" + NEWLINE;

	// smpp Message.
	output += indent + "smpp-msg " + smppMsg + NEWLINE;

    } else if (this->routeType == TASK_STARTUP_DIAMETER) {
	// diameter link
	output += indent + "diameter-link \"" + diameterLink + "\"" + NEWLINE;
	output += indent + "diameter-peer-realm \"" + diameterPeerRealm + "\"" + NEWLINE;

    } else {
	assert(!"Unsupported route type");
    }

    // Priority.
    char temp[20];
    sprintf(temp, "%u", priority);
    output += indent + "priority " + temp + NEWLINE;

    // call-gap
    sprintf(temp, "%u", call_gap_msec);
    output += indent + "call-gap " + temp + NEWLINE;

    // Route type.
    switch (this->routeType) {
    case TASK_STARTUP_CALL:
	output += indent + "route-type call" + NEWLINE;
	break;
    case TASK_STARTUP_REGISTRATION:
	output += indent + "route-type registration" + NEWLINE;
	break;
    case TASK_STARTUP_SMPP:
	output += indent + "route-type smpp" + NEWLINE;
	break;
    case TASK_STARTUP_DIAMETER:
	output += indent + "route-type diameter" + NEWLINE;
	break;
    }

    // Spawn task.
    if (appName.empty())
	output += indent + "no spawntask" + NEWLINE;
    else {
	output += indent + "spawntask " + appName;
	List<String>::const_iterator it, end;
	end = appArg.end();
	for (it = appArg.begin(); it!=end; ++it) {
	    output += " \"" + *it + "\"";
	}
	output += NEWLINE;
    }

    // Shutdown ?
    if (isShutdown)
	output += indent + "shutdown" + NEWLINE;
    else
	output += indent + "no shutdown" + NEWLINE;

    return OK;
}

/**
 Show configuration for the specified entry.
 */
bool Call_In_Route::OnShowConfig( const Cfg_Tree_Entry *entry,
				    String & output, 
				    const String & indent, 
				    int indent_size) const
{
    if (!ACQUIRE_SYSTEM_LOCK())
	return NOT_OK;

    OnShowConfig_NoLock( entry, output, indent, indent_size);
    RELEASE_SYSTEM_LOCK();
    return OK;
}


///////////////////////////////////////////////////////////////////////////////
enum {
    Term_Cmd_App,
    Term_Cmd_App_Exe,
    Term_Cmd_App_StackSize,
    Term_Cmd_App_MaxString,
    Term_Cmd_App_AutoSpawn,
    Term_Cmd_App_Shutdown,
    Term_Cmd_App_Param,
    Term_Cmd_App_Reload,
    Term_Cmd_CIR,
    Term_Cmd_App_GlobalParam,
    Term_Cmd_TestRoute,
    Term_Cmd_AppTraceFilter,
    
    Term_Cmd_App_NeedVoice,
    Term_Cmd_App_CallProceed,
    Term_Cmd_Debug_Level
};

/* Utility: verify the existance of the file.
 */
static bool IsFileExist(const char *filename)
{
    struct stat buf;
    return (::stat(filename, &buf) == 0);
}

/* Utility: 
 */

///////////////////////////////////////////////////////////////////////////////

CT_Application::CT_Application (const IString & name)
: appName(name)
{
    // Set default working directory.
    appWorkingDir = Kernel::Instance()->GetBaseDirectory() + "apps/" + appName + "/";

    // Set some parameters
    appAutoSpawn = 0;
    appStackSize = DEF_STACK_SIZE;
    appDebugLevel = 0;
    appMaxString = DEF_MAX_STRINGS;

    appIsLicensed = false;

    // By default, application is shutdown.
    // It must be re-enabled manually or via configuration script.
    appIsShutdown = true;

    // Build default application EXE path.
    appEXEFile = appWorkingDir + appName + ".slx";

    needVoice = false;
    autoCallProceeding = true;

    m_StatMaxStack = 0;
    m_StatMaxSStack = 0;
    m_StatMaxString = 0;
    m_StatMaxAllString = 0;

    usage_counter = 0;

    SLApplicationMonitor::CreateTableEntry(this);

}

/** Destructor. */
CT_Application::~CT_Application()
{
    while (appImageList.size()) {
	delete appImageList.back();
	appImageList.pop_back();
    }
}


/** Initialization.
 */
bool CT_Application::Initialize()
{
    //
    // Create application entry in configuration tree.
    //
    NEW_RETURN_ (appConfigTree, Cfg_Tree_Group ( appName, "Application " + appName, CT_App_Manager::Instance()->RootConfig()));
    if (appConfigTree == NULL)
	return false;

    //
    // Create comamnds.
    //

    Term_Cmd_Arg *p_arg;
    Term_Cmd *cmd_exe = NULL;
    Term_Cmd *cmd_stacksize = NULL;
    Term_Cmd *cmd_debug_level = NULL;
    Term_Cmd *cmd_maxstring = NULL;
    Term_Cmd *cmd_autospawn = NULL;
    Term_Cmd *cmd_shut = NULL;
    Term_Cmd *cmd_param = NULL;
    Term_Cmd *cmd_needvoice = NULL;
    Term_Cmd *cmd_callproceed = NULL;
    Term_Cmd *cmd_reload = NULL;

    // cmd: 'executable <path>'
    NEW_RETURN_ (cmd_exe, Term_Cmd ( this, 
				     Term_Cmd_App_Exe, 
				     "executable", 
				     "Path or filename of the application file (*.slx)",
				     PrivilegeLevelAdmin,
				     TermModeConfig,
				     false));
    if (cmd_exe == NULL)
	goto on_error;
    NEW_RETURN_ (p_arg, Term_Cmd_Arg("path", CVariant(""), CVariantValidator("", "Filename/path")));
    if (p_arg == NULL)
	goto on_error;
    cmd_exe->Add (p_arg);

    // cmd: 'stack-size <size>'
    NEW_RETURN_ (cmd_stacksize, Term_Cmd ( this,
					 Term_Cmd_App_StackSize,
					 "stack-size",
					 "Stack size for the application (in kilobytes)",
					 PrivilegeLevelAdmin,
					 TermModeConfig,
					 false));
    if (cmd_stacksize == NULL)
	goto on_error;
    NEW_RETURN_ (p_arg, Term_Cmd_Arg("size", 
				     CVariant(DEF_STACK_SIZE / 1024), 
				     CVariantRangeValidator(1, 128, "Size in kilobytes")));
    if (p_arg == NULL)
	goto on_error;
    cmd_stacksize->Add (p_arg);

    // cmd: 'max-string <number>'
    NEW_RETURN_ (cmd_maxstring, Term_Cmd ( this,
					 Term_Cmd_App_MaxString,
					 "max-string",
					 "Maximum number of strings for the application",
					 PrivilegeLevelAdmin,
					 TermModeConfig,
					 false));
    if (cmd_maxstring == NULL)
	goto on_error;
    NEW_RETURN_ (p_arg, Term_Cmd_Arg("numstring", 
				     CVariant((int)0),
				     CVariantValidator(CVariant(0), "Number of strings")));
    if (p_arg == NULL)
	goto on_error;
    cmd_maxstring->Add (p_arg);

    // cmd: 'need-voice'
    NEW_RETURN_ (cmd_needvoice, Term_Cmd ( this,
					 Term_Cmd_App_NeedVoice,
					 "need-voice",
					 "Voice resource requirement of this application",
					 PrivilegeLevelAdmin,
					 TermModeConfig,
					 true));
    if (cmd_needvoice == NULL)
	goto on_error;

    // cmd: 'call-proceeding'
    NEW_RETURN_ (cmd_callproceed, Term_Cmd ( this,
					 Term_Cmd_App_CallProceed,
					 "call-proceeding",
					 "Automatically send PROCEEDING during incoming call",
					 PrivilegeLevelAdmin,
					 TermModeConfig,
					 true));
    if (cmd_callproceed == NULL)
	goto on_error;

    // cmd: 'autospawn <value>'
    NEW_RETURN_ (cmd_autospawn, Term_Cmd ( this,
					  Term_Cmd_App_AutoSpawn,
					  "autospawn",
					  "Number of tasks to create when system started/restarted",
					  PrivilegeLevelAdmin,
					  TermModeConfig,
					  true));
    if (cmd_autospawn == NULL)
	goto on_error;
    NEW_RETURN_ (p_arg, Term_Cmd_Arg ("ninstance", 
				      CVariant((int)0), 
				      CVariantRangeValidator(0, 1024, "Number of instances")));
    if (p_arg == NULL)
	goto on_error;
    cmd_autospawn->Add (p_arg);


    // cmd: 'shutdown'
    NEW_RETURN_ (cmd_shut, Term_Cmd ( this, 
				     Term_Cmd_App_Shutdown, 
				     "shutdown", 
				     "Disable this application",
				     PrivilegeLevelAdmin,
				     TermModeConfig,
				     true));
    if (cmd_shut == NULL)
	goto on_error;

    // cmd: 'param <name> <value>'
    NEW_RETURN_ (cmd_param, Term_Cmd ( this, 
				     Term_Cmd_App_Param, 
				     "param", 
				     "Set application specific parameters",
				     PrivilegeLevelAdmin,
				     TermModeConfig,
				     true));
    if (cmd_param == NULL)
	goto on_error;
    NEW_RETURN_ (p_arg, Term_Cmd_Arg("name", 
				     CVariant(""), 
				     CVariantValidator("", "Parameter name")));
    if (p_arg == NULL)
	goto on_error;
    cmd_param->Add (p_arg);

    NEW_RETURN_ (p_arg, Term_Cmd_Arg("value", 
				     CVariant(""), 
				     CVariantValidator("", "Parameter value"),TermArgOptional));
    if (p_arg == NULL)
	goto on_error;
    cmd_param->Add (p_arg);

    // Register reload command
    NEW_RETURN_ (cmd_reload, Term_Cmd (this,
				       Term_Cmd_App_Reload,
				       "reload",
				       "Reload application image file",
				       PrivilegeLevelAdmin,
				       TermModeConfig,
				       false));
    if (cmd_reload == NULL)
	goto on_error;

    NEW_RETURN_ (cmd_debug_level, Term_Cmd ( this,
					 Term_Cmd_Debug_Level,
					 "debug-level",
					 "Debug level (1-String table i/o operation)",
					 PrivilegeLevelAdmin,
					 TermModeConfig,
					 false));
    if (cmd_debug_level == NULL)
	goto on_error;
    NEW_RETURN_ (p_arg, Term_Cmd_Arg("level", 
				     CVariant((int)0), 
				     CVariantRangeValidator(0, 10, "Debug level")));
    if (p_arg == NULL)
	goto on_error;
    cmd_debug_level->Add (p_arg);

    // Add the commands to application's config tree
    RegisterTermCmd (cmd_exe, appConfigTree);
    RegisterTermCmd (cmd_stacksize, appConfigTree);
    RegisterTermCmd (cmd_debug_level, appConfigTree);
    RegisterTermCmd (cmd_maxstring, appConfigTree);
    RegisterTermCmd (cmd_autospawn, appConfigTree);
    RegisterTermCmd (cmd_shut, appConfigTree);
    RegisterTermCmd (cmd_param, appConfigTree);
    RegisterTermCmd (cmd_needvoice, appConfigTree);
    RegisterTermCmd (cmd_callproceed, appConfigTree);
    RegisterTermCmd (cmd_reload, appConfigTree);

    appConfigTree->RegisterShowHandler (this);
    return true;

on_error:
    delete cmd_exe;
    delete cmd_stacksize;
    delete cmd_autospawn;
    delete cmd_needvoice;
    delete cmd_callproceed;
    delete cmd_shut;
    delete cmd_param;
    delete cmd_reload;
    delete cmd_debug_level;
    return false;
}


bool CT_Application::IsLicensed() const
{
    return appIsLicensed;
}

/* Utility: make a full path.
 */
void CT_Application::MakeFullPath ( String & filepath )
{
    // If path is a full path, then use it as it is.
    // Otherwise construct full path from working directory.
    if (strchr(filepath.c_str(), '/') || strchr(filepath.c_str(), '\\'))
	;
    else
	filepath = appWorkingDir + filepath;
}

/**
 Terminal command callback
 */
void CT_Application::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant v1, v2;
    if (cmd->GetArgCount() > 0)
	cmd->GetArg((size_t)0)->GetValue(v1);
    if (cmd->GetArgCount() > 1)
	cmd->GetArg(1)->GetValue(v2);

    switch (cmd->GetId()) {
    case Term_Cmd_App_Exe:
	{
	    String path = v1.ToString();

	    // Verify the file.
	    MakeFullPath (path);
	    if (appIsShutdown && !IsFileExist(path.c_str()) && session->GetStream().IsInteractive()) {

		if (session->PromptYesNo(false, 
					 "File '%s' does not exist." NEWLINE
					 "Use it anyway (y/n) ? ",
					 path.c_str()) == false)
		    return;
	    }

	    if (!ACQUIRE_SYSTEM_LOCK())
		return;

	    if (ReloadApp(path.c_str()) == false) {
		RELEASE_SYSTEM_LOCK();
		session->SendErrorf ("Unable to open/read file '%s'" NEWLINE, path.c_str());
	    } else
		RELEASE_SYSTEM_LOCK();
	}
	break;

    case Term_Cmd_App_StackSize:
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	appStackSize = v1.AsInt() * 1024;
	RELEASE_SYSTEM_LOCK();
	break;

    case Term_Cmd_Debug_Level:
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	appDebugLevel= v1.AsInt();
	RELEASE_SYSTEM_LOCK();
	break;

    case Term_Cmd_App_MaxString:
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	appMaxString = v1.AsInt();
	RELEASE_SYSTEM_LOCK();
	break;

    case Term_Cmd_App_AutoSpawn:
	{
	    int value;
	    if (no || !cmd->GetArg((size_t)0)->HasValue() ||
		(cmd->GetArg((size_t)0)->HasValue() && v1.AsInt()==0)) {
		value = 0;
	    } else
		value = v1.AsInt();

	    if (!ACQUIRE_SYSTEM_LOCK())
		return;

	    appAutoSpawn = value;
	    RELEASE_SYSTEM_LOCK();
	}
	break;

    case Term_Cmd_App_NeedVoice:
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	needVoice = !no;
	RELEASE_SYSTEM_LOCK();
	break;

    case Term_Cmd_App_CallProceed:
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	autoCallProceeding = !no;
	RELEASE_SYSTEM_LOCK();
	break;

    case Term_Cmd_App_Shutdown:
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	if (no) {
	    // temporarily set appShutdown to false, so that reload will actually
	    // reload the app.
	    bool prev_app_is_shutdown = appIsShutdown;
	    appIsShutdown = false;

	    if (ReloadApp(NULL) == true)
		appIsShutdown = false;
	    else {
		appIsShutdown = true;
		session->SendErrorf ("Error in loading the application" NEWLINE);
	    }
	} else {
	    Shutdown();
	}
	RELEASE_SYSTEM_LOCK();
	break;

    case Term_Cmd_App_Param:
	{
	    IString pname = v1.ToString();
	    IString pvalue = v2.ToString();

	    if (!ACQUIRE_SYSTEM_LOCK())
		return;

	    if (no) {
		if (DelAppParam(pname) == false)
		    session->SendErrorf ( "Parameter '%s' not found" NEWLINE,  pname.c_str());
	    } else {
		CLog::Debug("SYS", "Param %s (%s) = %s", pname.c_str(), v2.GetTypeStr(), pvalue.c_str());
		//if (SetAddAppParam(pname, v2.ToString()) == false)
		if (SetAddAppParam(pname, pvalue.c_str()) == false)
		    session->SendErrorf ( "Can not set/add parameter '%s'" NEWLINE,  pname.c_str());
	    }
	    RELEASE_SYSTEM_LOCK();
	}
	break;

    case Term_Cmd_App_Reload:
	if (!ACQUIRE_SYSTEM_LOCK())
	    return;

	bool result = ReloadApp();
	RELEASE_SYSTEM_LOCK();
	if (result == false)
	    session->SendErrorf ("Error in reloading file image" NEWLINE);
	break;

    }
}


/**
 Show configuration for the specified entry.
 This function will be called by the framework whenever it needs to display the
 specified entry. Previously this instance should tell the entry that it wishes to
 handle displaying the entry by calling entry->RegisterShowHandler().

 When this instance is deleted, it should call entry->UnregisterShowHandler()
 so that the handler will not be called (which will cause GPF).
 */
bool CT_Application::OnShowConfig_NoLock( const Cfg_Tree_Entry *entry,
					    String & output, 
					    const String & indent, 
					    int indent_size) const
{
    char tmp[128];

    // 'autospawn'
    if (appAutoSpawn) {
	sprintf (tmp, "%u", appAutoSpawn);
	output += indent + "autospawn " + tmp + NEWLINE;
    }

    // 'executable <path>'
    output += indent + "executable \"" + appEXEFile + "\"" + NEWLINE;

    // parameters
    Map <IString, String>::const_iterator it;
    for (it = appParam.begin(); it != appParam.end(); ++it)
	output += indent + "param " + it->first + " \"" + it->second + "\"" + NEWLINE;

    // 'stacksize'
    sprintf(tmp, "%u", appStackSize/1024);
    output += indent + "stack-size " + tmp + NEWLINE;

    // debug level
    sprintf(tmp, "%d\0", appDebugLevel);
    output += indent + "debug-level " + tmp + NEWLINE;

    // maximum string
    sprintf(tmp, "%u\0", appMaxString);
    output += indent + "max-string " + tmp + NEWLINE;

    // need-voice
    output += indent + (needVoice ? "" : "no ") + "need-voice" + NEWLINE;

    // call-proceeding
    output += indent + (autoCallProceeding ? "" : "no ") + "call-proceeding" + NEWLINE;

    // shutdown ?
    output += indent + (appIsShutdown ? "" : "no ") + "shutdown" + NEWLINE;

    return OK;
}

bool CT_Application::OnShowConfig( const Cfg_Tree_Entry *entry,
				     String & output, 
				     const String & indent, 
				     int indent_size) const
{
    if (!ACQUIRE_SYSTEM_LOCK())
	return NOT_OK;

    OnShowConfig_NoLock( entry, output, indent, indent_size);
    RELEASE_SYSTEM_LOCK();
    return OK;
}


/** Shutdown/disable the application.
 */
bool CT_Application::Shutdown()
{
    // mark the shutdown flag to true.
    appIsShutdown = true;

    // Cleanup unused images.
    Cleanup();
    return true;
}

/** Re-enable the application.
 */
bool CT_Application::NoShutdown()
{
    if (appIsShutdown) {
	// Re-load application image, because app. image will be automatically
	// unloaded upon Shutdown (see UnregisterTask).
	if (ReloadApp()==false)
	    return false;

	appIsShutdown = false;
    }
    return true;
}

/** Determine whether the application can be deleted.
 */
bool CT_Application::IsDeleteOk() const
{
    // Application can be deleted only if it is shutdown and
    // the number of code images is zero.
    return IsShutdown() && appImageList.empty();
}


/** Get number of file images and number of tasks running.
 */
void CT_Application::GetAppInfo (unsigned & n_images, unsigned & n_tasks)
{
    n_tasks = 0;
    n_images = appImageList.size();

    List <CT_App_Image *>::iterator it, end;
    end = appImageList.end();
    for (it = appImageList.begin(); it!=end; ++it) {
	CT_App_Image *img = *it;
	n_tasks += img->GetRefCount();	
    }
}


/** Reload the application image.
 */
bool CT_Application::ReloadApp (const char *path)
{
    String new_path;

    if (path != NULL) {
	new_path = path;
	MakeFullPath (new_path);
    } else
	new_path = appEXEFile;

    // Attempt to load the application, if app. is not shutdown.
    if (!appIsShutdown) {
	CT_App_Image *image = LoadApp (new_path.c_str());
	if (image == NULL)
	    return false;

	// Scan imanges for unused ones.
	List <CT_App_Image*>::iterator it;
	for (it=appImageList.begin(); it!=appImageList.end();) {
	    CT_App_Image *img = *it;
	    if (img->GetRefCount() == 0) {
		delete img;
		it = appImageList.erase(it);
	    } else
		++it;
	}

	// By convention, new loaded image will go in the front of the
	// application list.
	appImageList.push_front(image);
    }

    // Update application path.
    appEXEFile = new_path;
    return true;
}

/** Get application parameter.
 */
bool CT_Application::GetAppParam (const IString & name, String & value, bool warn_if_not_exist) const
{
    Map <IString, String>::const_iterator it;
    it = appParam.find (name);
    if (it == appParam.end()) {
	if (!CT_App_Manager::Instance()->GetGlobalParam()->GetAppParam(name, value)) {
	    value.erase();
	    // param not found
	    if (warn_if_not_exist)
		CLog::Warning(SYS, "Parameter '%s' undefined in '%s'",name.c_str(),Name().c_str());
	    return false;
	}
    } else {
	value = it->second;
    }
    return true;
}

/** Set application param.
 */
bool CT_Application::SetAppParam (const IString & name, const String & value)
{
    Map <IString, String>::iterator it;
    it = appParam.find (name);
    if (it == appParam.end()) {
	if (!CT_App_Manager::Instance()->GetGlobalParam()->SetAppParam(name, value)) {
	    // param not found
	    CLog::Warning(SYS, "Parameter '%s' undefined in '%s'",name.c_str(),Name().c_str());
	    return false;
	}
    } else {
	it->second = value;
    }
    return true;
}

/** Add a new application param.
 */
bool CT_Application::AddAppParam ( const IString & name, const String & value)
{
    Map <IString, String>::iterator it;
    it = appParam.find (name);
    if (it == appParam.end()) {
	it = appParam.insert (it, std::pair<IString, String>(name, value));
	return true;
    }

    // param already exist
    return false;
}

/** Delete an application param.
 */
bool CT_Application::DelAppParam ( const IString & name )
{
    Map <IString, String>::iterator it;
    it = appParam.find (name);
    if (it != appParam.end()) {
	appParam.erase(it);
	return true;
    }

    // param not found
    return false;
}

/** Set application param, or add a new one if it does not exist.
 */
bool CT_Application::SetAddAppParam ( const IString & name, const String & value )
{
    if (!AddAppParam (name, value))
	return SetAppParam(name, value);
    return true;
}


/** Register the task to the list of tasks that use this application.
 */
void CT_Application::RegisterTask ( CTask * ptask, App_Reg_Key key)
{
    assert(key);
    key->AddRef();
	pTaskList.push_front(ptask);
}

/** Unregister task from the list of tasks that use this application.
 */
void CT_Application::UnregisterTask ( CTask *ptask, App_Reg_Key key)
{
    assert(key);
	pTaskList.remove(ptask);

    if (key) {
	if (!IsShutdown()) {
	    assert (appImageList.size());
	}
	if (appImageList.empty()) return;

	key->SubRef();

	// Unload application image if the reference count is zero, AND
	// either the image is not the active image or the application is shutdown
	if (key->GetRefCount() == 0 && (key != appImageList.front() || IsShutdown())) {
	    appImageList.remove(key);
	    delete key;
		key = NULL;
	}
    }
}


/* Cleanup unused images.
 */
void CT_Application::Cleanup()
{
    List <CT_App_Image *>::iterator it, end;
    end = appImageList.end();

    for (it = appImageList.begin(); it!=end; ) {
	CT_App_Image *img = *it;

	// Unload application image if the reference count is zero, AND
	// either the image is not the active image or the application is shutdown
	if (img->GetRefCount() == 0 && (it != appImageList.begin() || IsShutdown())) {
	    appImageList.erase(it++);
	    delete img;
	} else
	    ++it;
    }
}


// Load an app. image.
CT_App_Image * CT_Application::LoadApp (const char *filename)
{
    CT_App_Image *app;
    TSLHEADER	slh;
    DWORD	bytesRead;
    TMEM       *code=NULL,*data=NULL;
    char       *strconst=NULL;
    TSLTRAILER	slt;
    
    FILE *f = fopen(filename,"rb");
    if (f == NULL) {
	CLog::SysLastError(SYS,"Failed to open binary file '%s'", filename);
	return NULL;
    }
    
    bytesRead = fread(&slh,sizeof(TSLHEADER),1,f);
    if (bytesRead != 1) {
	CLog::SysLastError(SYS,"Read failed for file '%s'", filename);
	fclose(f);
	return NULL;
    }

    if (slh.signature!=0x4c53) {
	CLog::Error( SYS, "Invalid SLX application signature in file '%s'", filename);
	fclose(f);
	return NULL;
    }
    
    NEW_RETURN_ (data, TMEM[slh.data_size/sizeof(TMEM)]);
    if (data == NULL) {
	CLog::Error ( SYS, "Not enough memory to create SLX data segment");
	fclose(f);
	return NULL;
    }

    NEW_RETURN_ (code, TMEM[slh.code_size/sizeof(TMEM)]);
    if (code == NULL) {
	delete [] data;
	fclose(f);
	CLog::Error ( SYS, "Not enough memory to create SLX code segment");
	return NULL;
    }

    NEW_RETURN_ (strconst, char[slh.stringconst_size+sizeof(int)]);
    if (strconst == NULL) {
	delete [] data;
	delete [] code;
	fclose(f);
	CLog::Error ( SYS, "Not enough memory to create SLX string table");
	return NULL;
    }

    // read data part, if any
    if (slh.data_size) {
	bytesRead = fread(data,slh.data_size,1,f);
	if (bytesRead != 1) {
	    CLog::SysLastError( SYS,"Read failed for file '%s'", filename);
	    delete [] data;
	    delete [] code;
	    delete [] strconst;
	    fclose(f);
	    return NULL;
	}
    }

    // read code part
    bytesRead = fread(code,slh.code_size,1,f);
    if (bytesRead != 1) {
	CLog::SysLastError( SYS,"Read failed for file '%s'", filename);
	delete [] data;
	delete [] code;
	delete [] strconst;
	fclose(f);
	return NULL;
    }

    // read strings
    bytesRead = fread(strconst,slh.stringconst_size+sizeof(int),1,f);
    if (bytesRead != 1) {
	CLog::SysLastError( SYS,"Read failed for file '%s'", filename);
	delete [] data;
	delete [] code;
	delete [] strconst;
	fclose(f);
	return NULL;
    }

    // read trailer (hostname and datetime), no error checking to maintain compatibility
    bytesRead = fread(&slt,sizeof(TSLTRAILER),1,f);
    if (bytesRead != 1) {
	memset(&slt, 0, sizeof(TSLTRAILER));
	imageCompileInfo = "(none)\n";
    } else {
	slt.hostname[sizeof(slt.hostname)-1] = '\0';
	char temp[128];
	sprintf(temp, "compiled on %s at %s", slt.hostname, ctime(&slt.datetime));
	imageCompileInfo = temp;
    }

    int vermajor=0, verminor=0;
    char who[256];
    int ntoken = sscanf(slt.hostname, "%[a-zA-Z0-9_@ ](%x.%x)", who, &vermajor, &verminor);
    //appIsLicensed = (strstr(slt.hostname, "(1.5)") != NULL);
    appIsLicensed = (ntoken == 3) && (((vermajor == 1) && (verminor >= 0x05)) || (vermajor > 1));
    if (!IsLicensed()) {
	CLog::Warning( SYS, "Application '%s' is using obsolete file format which will "
			    "be deprecated in next version, please contact developer.",
			    Name().c_str());
    }

    fclose(f);

    NEW_RETURN_ (app, CT_App_Image);
    if (app == NULL) {
	delete [] data;
	delete [] code;
	delete [] strconst;
	CLog::Error ( SYS, "Not enough memory to create SLX application image");
	return NULL;
    }

    app->Application = this;
    app->SetCode( code, slh.code_size);
    app->SetData( data, slh.data_size, appStackSize);
    app->SetDebugLevel(appDebugLevel);
    printf("slh.string_index= %d appMaxString =%d",appMaxString+(slh.string_index),appMaxString);
    app->SetStringTab(strconst, 1024+(slh.string_index), slh.string_index);
    app->InitStatistics();

    delete [] strconst;
    return app;
#if 0
    HANDLE	hRead;
    hRead = ::CreateFile ( filename,	    // path
			   GENERIC_READ,    // access
			   FILE_SHARE_READ, // sharing
			   NULL,	    // security attrs.
			   OPEN_EXISTING,   // creation disposition
			   FILE_ATTRIBUTE_NORMAL,   // attrs.
			   NULL);	    // template
    if (hRead == INVALID_HANDLE_VALUE) {
	CLog::SysLastError(SYS,"Failed to open SLX file '%s'", filename);
	return NULL;
    }
    
    if (!ReadFile(hRead, &slh, sizeof(TSLHEADER), &bytesRead, NULL) || bytesRead!=sizeof(TSLHEADER)) {
	CLog::SysLastError(SYS,"Read failed for file '%s'", filename);
	CloseHandle(hRead);
	return NULL;
    }

    if (slh.signature!=0x4c53) {
	CLog::Error( SYS, "Invalid SLX application signature in file '%s'", filename);
	CloseHandle(hRead);
	return NULL;
    }
    
    NEW_RETURN_ (data, TMEM[slh.data_size/sizeof(TMEM)]);
    if (data == NULL) {
	CLog::Error ( SYS, "Not enough memory to create SLX data segment");
	CloseHandle (hRead);
	return NULL;
    }

    NEW_RETURN_ (code, TMEM[slh.code_size/sizeof(TMEM)]);
    if (code == NULL) {
	delete [] data;
	CloseHandle (hRead);
	CLog::Error ( SYS, "Not enough memory to create SLX code segment");
	return NULL;
    }

    NEW_RETURN_ (strconst, char[slh.stringconst_size+sizeof(int)]);
    if (strconst == NULL) {
	delete [] data;
	delete [] code;
	CloseHandle (hRead);
	CLog::Error ( SYS, "Not enough memory to create SLX string table");
	return NULL;
    }
    
    if (!ReadFile(hRead, data, slh.data_size, &bytesRead, NULL) || bytesRead!=slh.data_size) {
	CLog::SysLastError( SYS,"Read failed for file '%s'", filename);
	delete [] data;
	delete [] code;
	delete [] strconst;
	CloseHandle(hRead);
	return NULL;
    }

    if (!ReadFile(hRead, code, slh.code_size, &bytesRead, NULL) || bytesRead!=slh.code_size) {
	CLog::SysLastError( SYS,"Read failed for file '%s'", filename);
	delete [] data;
	delete [] code;
	delete [] strconst;
	CloseHandle(hRead);
	return NULL;
    }

    if (!ReadFile(hRead, strconst, slh.stringconst_size+sizeof(int), &bytesRead, NULL) || 
	bytesRead != slh.stringconst_size+sizeof(int)) {
	CLog::SysLastError( SYS,"Read failed for file '%s'", filename);
	delete [] data;
	delete [] code;
	delete [] strconst;
	CloseHandle(hRead);
	return NULL;
    }

    CloseHandle(hRead);

    NEW_RETURN_ (app, CT_App_Image);
    if (app == NULL) {
	delete [] data;
	delete [] code;
	delete [] strconst;
	CLog::Error ( SYS, "Not enough memory to create SLX application image");
	return NULL;
    }

    app->Application = this;
    app->SetCode( code, slh.code_size);
    app->SetData( data, slh.data_size, appStackSize);
    app->SetDebugLevel(appDebugLevel);
    app->SetStringTab(strconst, appMaxString+slh.string_index, slh.string_index);
    app->InitStatistics();

    delete [] strconst;
    return app;
#endif
}


/** Initialize exec context with the specified application image.
 */
bool Task_Exec_Context::Init(CT_App_Image *appimage, const List<String> & cmdline_param)
{
    m_AppImage = appimage;

    CLog::Detail( SYS, "Initializing Task Exec Context for %s (data=%d, stack=%d, string=%d)",
	appimage->Application->Name().c_str(),
	appimage->GetDataSize(),
	appimage->GetStackSize(),
	appimage->GetStringTabSize());

    m_datasize   = appimage->GetDataSize();
    m_stacksize  = appimage->GetStackSize();
    m_stringtab_size = appimage->GetStringTabSize();
    stringtab_debug_level = appimage->GetDebugLevel();
    //stringtab_debug_level = 0;

    unsigned datasize_in_tmem = (m_datasize/sizeof(TMEM) + 1);
    unsigned stacksize_in_tmem = (m_stacksize/sizeof(TMEM) + 1);

    //unsigned totsize = m_datasize + m_stacksize + sizeof(unsigned);
    unsigned totsize_in_tmem = datasize_in_tmem + 1 + stacksize_in_tmem;

    m_code  = appimage->GetCode();
    m_data = (TMEM*)malloc(totsize_in_tmem*sizeof(TMEM));
    if (!m_data) {
	return false;
    }

    memset(m_data, 0, totsize_in_tmem*sizeof(TMEM));

    //copy only initialised data from app template
    memcpy(m_data,appimage->GetData(),m_datasize);

    // put a signature
    m_data[datasize_in_tmem].i = 0xdeadc0de;

    m_stringidx = appimage->GetStringIndex();
    m_minstridx = m_stringidx;
#ifdef STRINGTAB_CHECK
    m_stringtab_checksum = 0;
#endif

    NEW_RETURN_ (m_stringtab, App_String[m_stringtab_size+1]);
#ifdef STRINGTAB_CHECK
    Check_StringTab("Init-Created(2)", __FILE__, __LINE__, true);
#endif
    if (!m_stringtab) {
	/**
	delete [] m_data;
	m_data = NULL;
	**/
	free(m_data);
	m_data = NULL;
	return false;
    }

    // application constant string storage
    m_const_string = appimage->GetConstantString();

#ifdef STRINGTAB_CHECK
    Check_StringTab("Init-Copied(1)", __FILE__, __LINE__, true);
#endif

    m_StatAllString = appimage->GetStatAllString();
    m_IP     = 0;
    m_EP     = 0;
    //m_BP     = m_SP  = (totsize-sizeof(TMEM))/sizeof(TMEM);
    m_BP     = m_SP  = totsize_in_tmem-1;
    //m_SS.min = m_datasize/sizeof(TMEM);
    m_SS.min = datasize_in_tmem + 1;
    m_SS.max = m_BP;

    // Put command line parameters in task's cmdline.
    List<String>::const_iterator it, end;
    end = cmdline_param.end();
    for (it = cmdline_param.begin(); it!=end; ++it)
	m_CmdLine.push_back(*it);

    return true;
}

Task_Exec_Context * CT_Application::CreateContext (const List<String> & cmdline_param) 
{
    if (appIsShutdown)
	return NULL;

    if (appImageList.empty())
	return NULL;

    int *dummy=NULL;

    try {
        dummy = new int[100];
    } catch(...){
        dummy = NULL;
    }

    if(!dummy)
      return;
    else
      delete[] dummy;

    Task_Exec_Context *e=NULL;
    NEW_RETURN_ (e, Task_Exec_Context(this));

#if 0
    try { 
        e = new Task_Exec_Context(this);
    } catch (...) { 
        e = NULL; 
        //CLog::Fatal ( "sys", "System memory allocation failed in %s:%u: %s", __FILE__, __LINE__, "new " #exp); 
    }
#endif

    if (e == NULL)
	return NULL;

    CT_App_Image *appimage = appImageList.front();
    if (!e->Init(appimage, cmdline_param)) {
	CLog::Warning("SYS", "Can't create task for application %s, initialization failed", Name().c_str());
	delete e;
        e = NULL;
	return NULL;
    }

    return e;
}

/** Create a new task from this application.
 */
CTask * CT_Application::SpawnTask(const List<String> & cmdline_param,
				  Task_Startup_Type startup_type)
{
    CTask *p_task;
    Task_Exec_Context *p_context;
//    printf("Inside CT_Application::SpawnTask \n");
    
    // Make sure application is not shutdown
    if (IsShutdown()) {
		CLog::Info ( SYS, "Can't create task because application %s is shutdown",
			Name().c_str());
//	printf("Can't create task because application is shutdown\n");
	return NULL;
    }

    // Make sure that we have one image file loaded.
    assert (appImageList.size());
    if (appImageList.empty()) {
		printf("appImageList is empty\n");
		return NULL;
    }

    // Create and initialize a new task execution context.
#ifndef __linux__
    atomic_set (&Kernel::Instance()->currentService, "CT_Application::SpawnTask::CreateContext");
#endif
    p_context = CreateContext(cmdline_param);
    if (p_context == NULL) {
	printf("p_context == NULL \n");
	return NULL;
    }

    // Create the task.
#ifndef __linux__
    atomic_set (&Kernel::Instance()->currentService, "CT_Application::SpawnTask::CreateTask");
#endif
    NEW_RETURN_ (p_task, CTask(this, p_context, appImageList.front(), startup_type));
    if (p_task == NULL) {
		printf("------------------ Task Creation failed --------------------\n");
		delete p_context;
		return NULL;
    }

    // Connect task to the context.
    p_context->Attach(p_task);

    // Init task.
    atomic_set (&Kernel::Instance()->currentService, "CT_Application::SpawnTask::TaskInit");
    if (p_task->Init() == NOT_OK) {
	printf("p_task->Init() == NOT_OK \n");
	delete p_context;
	delete p_task;
        p_context = NULL;
        p_task = NULL;
	return NULL;
    }

    CLog::Detail(SYS, p_task, "Task created");
	printf("-------------------- Task Created : %d --------------------\n",p_task);
    assert (p_task->GetState() == CTask::READY);

    IncUsageCount();

//    printf("returned spawntask =%d\n",p_task);
    return p_task;
}


// Get the compile info of the active image.
const String & CT_Application::GetImageCompileInfo() const
{
    return imageCompileInfo;
}

/*
	Get total blocked time of all task spawned from this 
	application instance.
 */
int CT_Application::GetTotalBlockTime()
{
	List<CTask*>::iterator it;
	CTask* ptask;
	int retval = 0;
	
	for(it=pTaskList.begin();it!=pTaskList.end();it++)
	{
		ptask = *it;
		const CTaskInfo* pti = ptask->GetTaskInfo();
		retval += pti->m_MSBlockedTime;
	}

	return retval;
}

int CT_Application::GetTotalDurationTime()
{
	List<CTask*>::iterator it;
	CTask* ptask;
	int retval = 0;
	
	for(it=pTaskList.begin();it!=pTaskList.end();it++)
	{
		ptask = *it;
		const CTaskInfo* pti = ptask->GetTaskInfo();
		retval += pti->m_MSExecTime;
	}

	return retval;
}


///////////////////////////////////////////////////////////////////////////////

struct App_Trace_Criteria {
    unsigned id;
    regex_t  regex;
    String   definition;

    App_Trace_Criteria(unsigned _id, const char *_def)
	: id(_id), definition(_def)
    {
	memset(&regex, 0, sizeof(regex));
    }

    ~App_Trace_Criteria() {
	regfree(&regex);
    }
    bool Compile() {
	return regcomp(&regex, definition.c_str(), REG_ICASE | REG_EXTENDED)==0;
    }
    bool Match(const char *input) {
	return regexec(&regex, input, 0, NULL, 0) != REG_NOMATCH;
    }
};

///////////////////////////////////////////////////////////////////////////////

CT_App_Manager::CT_App_Manager()
: ServiceProvider ("appmgr", false, true, Lowest_Priority), cirRootCfg(NULL),
  unusedTraceCriteriaId(0)
{
    _appGlobalParam = NULL;

    appTrace = KLog::Instance()->CreateTrace("app", "application", "Application defined trace");
}

CT_App_Manager::~CT_App_Manager()
{
    while (applicationList.size()) {
	Application_List_Type::iterator it = applicationList.begin();
	delete it->second.first;
	applicationList.erase(it);
    }
    while (cirList.size()) {
	delete cirList.back();
	cirList.pop_back();
    }

    appManagerInstance = NULL;

    if (_appGlobalParam) delete _appGlobalParam;

    while (appTraceCriteria.size()) {
	delete appTraceCriteria.back();
	appTraceCriteria.pop_back();
    }

    KLog::Instance()->DestroyTrace(appTrace);
}

/** Create the singleton instance of this class.
 */
CT_App_Manager * CT_App_Manager::Create()
{
    if (appManagerInstance == NULL)
	appManagerInstance = new CT_App_Manager;
    return appManagerInstance;
}

/** Initialize the application manager.
 */
bool CT_App_Manager::Load()
{
    _appGlobalParam = new AppGlobalParameter;

    // Create root configuration tree for applications.
    appRootCfg = new Cfg_Tree_Group("application", 
				    "Applications settings",
				    Kernel::Instance()->RootConfig());

    // Create root configuration tree for call-in-route
    cirRootCfg = new Cfg_Tree_Group ("call-in-route",
				     "Application routing for incoming calls",
				     Kernel::Instance()->RootConfig());

    // Register command to go to/create application config.
    Term_Cmd *cmd_app = new Term_Cmd (this,
				      Term_Cmd_App,
				      "application",
				      "Create/configure applications",
				      PrivilegeLevelAdmin,
				      TermModeConfig,
				      true);
    cmd_app->Add ( new Term_Cmd_Arg("app", CVariant(""), CVariantValidator("", "Application name")) );
    RegisterTermCmd (cmd_app, Kernel::Instance()->RootConfig());

    // Register 'apptrace-filter' command
    Term_Cmd *cmd_apptrace = new Term_Cmd (this,
					   Term_Cmd_AppTraceFilter,
					   "apptrace-filter",
					   "Application trace filter",
					   PrivilegeLevelUser,
					   TermModeNormal,
					   false);
    CVariantChoiceValidator vchoice1("list", "Show application trace filters");
    vchoice1.Add ("add", "Add a new filter");
    vchoice1.Add ("delete", "Delete an existing filter");
    cmd_apptrace->Add(new Term_Cmd_Arg ("@0", CVariant(""), vchoice1));
    cmd_apptrace->AddDynamic(1, 0, "add",
			     new Term_Cmd_Arg("@1", "", CVariantValidator("", "String to match in the application trace criteria")));
    cmd_apptrace->AddDynamic(1, 0, "delete",
			     new Term_Cmd_Arg("@1", "", CVariantValidator("", "Filter number to delete (or 'all')")));
    RegisterTermCmd(cmd_apptrace,Kernel::Instance()->RootConfig());

    // Register command to go to/create application config.
    Term_Cmd *cmd_gparam = new Term_Cmd (this,
				      Term_Cmd_App_GlobalParam,
				      "global-parameter",
				      "Configure application global parameters",
				      PrivilegeLevelAdmin,
				      TermModeConfig,
				      false);
    RegisterTermCmd (cmd_gparam, Kernel::Instance()->RootConfig());

    // Call-In-Route command
    Term_Cmd *cmd_cir = new Term_Cmd (this,
				      Term_Cmd_CIR,
				      "call-in-route",
				      "Create/configure dispatching route for incoming calls",
				      PrivilegeLevelAdmin,
				      TermModeConfig,
				      true);
    cmd_cir->Add (new Term_Cmd_Arg ("cir", CVariant(""), CVariantValidator("", "Call-in-route name")));
    RegisterTermCmd (cmd_cir, Kernel::Instance()->RootConfig());

    // CIR command, which is the alias for 'Call-In-Route' command
    Term_Cmd *cmd_cir2 = new Term_Cmd (this,
				      Term_Cmd_CIR,
				      "cir",
				      "Alias for 'call-in-route' command",
				      PrivilegeLevelAdmin,
				      TermModeConfig,
				      true);
    cmd_cir2->Add (new Term_Cmd_Arg ("cir", CVariant(""), CVariantValidator("", "Call-in-route name")));
    RegisterTermCmd (cmd_cir2, Kernel::Instance()->RootConfig());

    // Register command to test call-in-route.
    Term_Cmd *cmd_test_route = new Term_Cmd (this,
					     Term_Cmd_TestRoute,
					     "test-route",
					     "Test the call-in-route specification",
					     PrivilegeLevelUser,
					     TermModeAll,
					     false);
    cmd_test_route->Add (new Term_Cmd_Arg ("ddi", CVariant(""), CVariantValidator("", "Called number (DDI)")));
    cmd_test_route->Add (new Term_Cmd_Arg ("cli", CVariant(""), CVariantValidator("", "Calling number (CLI)")));
    cmd_test_route->Add (new Term_Cmd_Arg ("interface", CVariant(""), CVariantValidator("", "Interface name")));
    cmd_test_route->Add (new Term_Cmd_Arg ("group", CVariant(""), CVariantValidator("", "Group name")));
    RegisterTermCmd (cmd_test_route, Kernel::Instance()->RootConfig());

    // Register show handler.
    Kernel::Instance()->RootConfig()->RegisterShowHandler (this);
    return true;
}

/** Deinitialize the application manager.
 */
bool CT_App_Manager::Unload()
{
    Application_List_Type::iterator it;
    for (it = applicationList.begin(); it!=applicationList.end(); ++it)
	it->second.first->Shutdown();

    // Unregister show handler.
    Kernel::Instance()->RootConfig()->UnregisterShowHandler(this);
    return true;
}

/** Create a new application, or get the existing one if the application
    is already loaded.
    Return the instance of the application, or NULL on error.
 */
CT_Application * CT_App_Manager::CreateApp(const IString & appname)
{
    Application_List_Type::iterator it;
    it = applicationList.find(appname);
    if (it != applicationList.end())
	return it->second.first;

    CT_Application *app;
    NEW_RETURN_ (app, CT_Application(appname));
    if (app == NULL) {
	CLog::Error ( SYS, "Not enough memory to create a new application");
	return NULL;
    }

    if (!app->Initialize()) {
	delete app;
        app = NULL;
	return NULL;
    }

    App_List_Val_Type val(app, false);
    applicationList.insert(std::pair<IString, App_List_Val_Type>(appname, val));
    return app;
}

/** Get the existing application.
 */
CT_Application * CT_App_Manager::GetApp(const IString & appname)
{
    Application_List_Type::iterator it;
    it = applicationList.find(appname);
    if (it == applicationList.end())
	return NULL;
    return it->second.first;
}

/** Delete an application.
 */
bool CT_App_Manager::DeleteApp (const IString & appname)
{
    Application_List_Type::iterator it;
    it = applicationList.find(appname);
    if (it == applicationList.end())
	return false;

    CT_Application *app = it->second.first;
    app->Shutdown();
    it->second.second = true;

    if (app->IsDeleteOk()) {
	delete app;
        app = NULL;
	applicationList.erase(it);
	return true;
    }

    // Could not delete app at this time.
    return false;
}

/** Create a new task.
 */
CTask * CT_App_Manager::SpawnTask(const IString & appname, const List<String> & cmdline_param,
				  Task_Startup_Type startup_type)
{
    Application_List_Type::iterator it;
    it = applicationList.find(appname);
    if (it == applicationList.end()) {
	CLog::Warning ( SYS, "Spawntask: unable to find application '%s'", appname.c_str());
	return NULL;
    }

    return it->second.first->SpawnTask (cmdline_param, startup_type);
}

/** Display application trace.
 */
void CT_App_Manager::AppTrace( CTask *task, const char *trace_id, const char *trace_msg)
{

//    cout << "***** I am inside AppTrace ********"<< endl;
    // don't bother if trace is not enabled.
    if (!KLog::Instance()->IsTraceEnabled(appTrace))
	return;

    // Scan app trace criteria for a match
    List <App_Trace_Criteria *>::iterator it, end;
    end = appTraceCriteria.end();
    unsigned trace_num = 0;
    for (it = appTraceCriteria.begin(); it != end; ++it, ++trace_num) {
	App_Trace_Criteria *crit = *it;
	if (crit->Match(trace_id))
	    break;
    }

    if (it == end) {
	// no match
	return;
    }

    char taskid[16];
    sprintf(taskid, "%.2s%04u", task->GetApplication()->Name().c_str(), task->GetPID());
/*    KLog::Instance()->TraceFormat (KLog::LFmt_TimeMsec, appTrace, task, 
				   "A%02d %s %s", trace_num, taskid, trace_msg);*/
}

/** Dispatch an incoming call to application.
    Return NULL if no application matched the properties of the incoming call.
 */
CTask * CT_App_Manager::RouteIncomingCall (const CallParameter & cparam, const String & port_in, const String &group, CallCause &cause)
{
    const String & ddi = cparam.cnCalled;
    const String & cli = cparam.cnCalling;
    // find out whether the parameter matches the call-route.
    List <Call_In_Route *>::iterator it, end;
    end = cirList.end();
    for (it = cirList.begin(); it!=end; ++it) {
	Call_In_Route *route = *it;
	if (!route->IsShutdown() && route->GetRouteType() == TASK_STARTUP_CALL) {
	    if (route->Match(TASK_STARTUP_CALL, ddi, cli, port_in, group, "", "")) {
		route->usage_counter++;
		CLog::Detail ( "SYS", "Route match: CALL %s spawning %s", route->GetName().c_str(), route->GetAppName().c_str());
        /* printf("Route match: CALL %s spawning %s\n",route->GetName().c_str(), route->GetAppName().c_str()); */
		break;
	    }
	}
    }

    if (it == end) {
	CLog::Info ( "SYS", "No matching call in route for incoming call: "
			    "ddi='%s' cli='%s' port_in='%s' group='%s'",
			    ddi.c_str(), cli.c_str(), port_in.c_str(), group.c_str());

	cause = CC_NoRouteToDestination;
	return NULL;
    }

    // check call-gapping
    if ((*it)->CallGapInForce()) {
	cause = CC_SwitchingEquipmentCongestion;
	return NULL;
    }

    // Check license again (just in case people breaks the first protection).
    // But this time, we print a cryptic message.
#ifdef XGATE_HAS_LM
    if (LicenceManager::Instance()->GetCurrentLicenceMode()==Lm_Evaluation) {
	CLog::Error( "SYS", "Internal error code 0x0000FA7A1 in routing.");
	cause = CC_BearerCapabilityUnauthorized;
	return NULL;
    }
#endif

    // spawn the application.
    const String & appname = (*it)->GetAppName();
    const List <String> & arg = (*it)->GetAppArg();

    // if task can't be created, reject call with temporary failure
    cause = CC_TemporaryFailure;
    return SpawnTask (appname, arg, TASK_STARTUP_CALL);
}

/** Dispatch incoming registration.
 */
CTask *CT_App_Manager::RouteIncomingRegistration(const Reg_Info *info)
{
#ifdef XGATE_HAS_SIP
   printf("XGATE_HAS_SIP is defined \n");

   // find out whether the parameter matches the call-route.
    List <Call_In_Route *>::iterator it, end;
    end = cirList.end();
    for (it = cirList.begin(); it!=end; ++it) {
	Call_In_Route *route = *it;
	if (!route->IsShutdown() && route->GetRouteType() == TASK_STARTUP_REGISTRATION) {

		if (route->Match(TASK_STARTUP_REGISTRATION, "", "", "", "", info->domain, info->agent->GetMethod())) {
		    route->usage_counter++;
		    CLog::Detail ( "SYS", "Route match: REGISTRATION %s spawning %s", route->GetName().c_str(), route->GetAppName().c_str());
		    printf("Route match: REGISTRATION is success \n");
		    break;
	    }

	}
    }

    if (it == end) {
	CLog::Info ( "SYS", "No matching call in route for incoming registration: "
			    "domain='%s', method='%s'",
			    info->domain.c_str(), info->agent->GetMethod().c_str());
	printf("No matching call in route for incoming registration\n");
	return NULL;
    }

    // spawn the application.
    const String & appname = (*it)->GetAppName();
    const List <String> & arg = (*it)->GetAppArg();

    return SpawnTask (appname, arg, TASK_STARTUP_REGISTRATION);
#else
    //printf("XGATE_HAS_SIP not defined \n");
    return NULL;
#endif

}

#ifdef XGATE_HAS_DIAMETER
/** Dispatch incoming diameter.
 */
CTask *CT_App_Manager::RouteIncomingDiameter(Diameter_msg msg, const string &linkName, const char* peerRealm)
{
    // find out whether the parameter matches the call-route.
    std::list <Call_In_Route *>::iterator it, end;
    end = cirList.end();
    for (it = cirList.begin(); it!=end; ++it) {
	Call_In_Route *route = *it;
	if (!route->IsShutdown() && route->GetRouteType() == TASK_STARTUP_DIAMETER && 
	    route->Match(TASK_STARTUP_DIAMETER, "", "", "", "", "", "", -1, "", linkName, peerRealm)) {
	    break;
	    /*
	    if (route->Match(TASK_STARTUP_REGISTRATION, "", "", "", "", info->domain, info->agent->GetMethod())) {
		CLog::Detail ( "SYS", "Route match: REGISTRATION %s spawning %s", route->GetName().c_str(), route->GetAppName().c_str());
		break;
	    }
	    */
	    
	}
    }
    
    if (it == end) {
	CLog::Info ( "SYS", "No matching call in route for incoming diameter ");
	return NULL;
    }
    
    // spawn the application.
    const String & appname = (*it)->GetAppName();
    const std::list <String> & arg = (*it)->GetAppArg();
    
    return SpawnTask (appname, arg, TASK_STARTUP_REGISTRATION);
}
#endif

/** HandleEvents: this will periodically do clean up the deleted applications.
 */
void CT_App_Manager::HandleEvents()
{
    // For all applications, look for applications which has been marked to be deleted.
    Application_List_Type::iterator it;
    for (it = applicationList.begin(); it!=applicationList.end(); ) {
	if (it->second.second && it->second.first->IsDeleteOk()) {
	    delete it->second.first;
	    applicationList.erase(it);
	    it = applicationList.begin();
	} else
	    ++it;
    }
}

/** Re-sort call-in-routes.
 */
void CT_App_Manager::CIR_Resort ( Call_In_Route * route )
{
    int prio = route->GetPriority();

    // first remove from the list.
    cirList.remove(route);

    // traverse until got route with lower priority.
    List <Call_In_Route*>::iterator it, end;
    end = cirList.end();
    for (it = cirList.begin(); it!=end; ++it) {
	Call_In_Route *route = *it;
	if (prio > route->GetPriority())
	    break;
    }

    cirList.insert (it, route);
}


// Find a call-in-route based on its name.
Call_In_Route *CT_App_Manager::FindRoute(const String & name)
{
    List <Call_In_Route*>::iterator it, end;
    end = cirList.end();
    for (it = cirList.begin(); it!=end; ++it) {
	Call_In_Route *route = *it;
	if (!strcasecmp(route->GetName().c_str(), name.c_str()))
	    return route;
    }
    return NULL;
}


/**
 Terminal command callback
 */
void CT_App_Manager::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant v;
    IString name;

    switch (cmd->GetId()) {
    case Term_Cmd_App:
	cmd->GetArg ((size_t)0)->GetValue (v);
	name = v.ToString();
	if (no == false) {
	    if (!ACQUIRE_SYSTEM_LOCK())
		return;

	    CT_Application *app = GetApp(name);
	    RELEASE_SYSTEM_LOCK();
	    if (app != NULL) {
		session->SetRoot (app->GetConfigTree());
	    } else {
		if (session->GetStream().IsInteractive()) {
		    CVariant result;
		    bool status;

		    CVariantChoiceValidator yesnoVdtor("n", "No");
		    yesnoVdtor.Add ("y", "Yes, create new application");
		    status = session->PromptInput ( result, 
						    CVariant("n"), 
						    yesnoVdtor, 
						    "Create new application '%s' (y/n) ? ", 
						    name.c_str());
		    if (status != OK || tolower(result.ToString().c_str()[0]) != 'y' )
			return;
		}

		if (!ACQUIRE_SYSTEM_LOCK())
		    return;

		app = CreateApp(name);
		RELEASE_SYSTEM_LOCK();
		if (app != NULL) {
		    if (session->GetStream().IsInteractive())
			session->Sendf("Application '%s' created" NEWLINE, name.c_str());
		    session->SetRoot (app->GetConfigTree());
		} else
		    session->SendErrorf("Could not create application '%s'" NEWLINE, name.c_str());
	    }
	} else {
	    if (!ACQUIRE_SYSTEM_LOCK())
		return;

	    CT_Application *app = GetApp(name);
	    RELEASE_SYSTEM_LOCK();
	    if ( app == NULL ) {
		session->SendErrorf ( "Could not find application '%s'" NEWLINE, name.c_str());
		return;
	    }

	    if (session->GetStream().IsInteractive()) {
		CVariant result;
		bool status;

		CVariantChoiceValidator yesnoVdtor("n", "No");
		yesnoVdtor.Add ("y", "Yes");
		status = session->PromptInput ( result, 
						CVariant("n"), 
						yesnoVdtor, 
						"Delete application '%s' (y/n) ? ", 
						name.c_str());
		if (status != OK || tolower(result.ToString().c_str()[0]) != 'y' )
		    return;
	    }
	    Config_Controller::Instance()->NotifyEntryDeleted(session, app->GetConfigTree());

	    if (!ACQUIRE_SYSTEM_LOCK())
		return;

	    bool status = DeleteApp(name);
	    RELEASE_SYSTEM_LOCK();
	    if ( status == false)
		session->Sendf ("Application '%s' will be deleted when all tasks have exited." NEWLINE, name.c_str());
	    else
		session->Sendf ("Application '%s' deleted." NEWLINE, name.c_str());
	}
	break;
    case Term_Cmd_CIR:
	cmd->GetArg ((size_t)0)->GetValue (v);
	name = v.ToString();
	if (no == false) {
	    if (!ACQUIRE_SYSTEM_LOCK())
		return;

	    Call_In_Route *route = FindRoute(name);
	    RELEASE_SYSTEM_LOCK();
	    if (route != NULL) {
		session->SetRoot (route->GetConfigTree());
	    } else {
		if (session->GetStream().IsInteractive()) {
		    if (session->PromptYesNo(false, "Create a new route '%s' (y/n) ?", name.c_str()) != true)
			return;
		}

		NEW_RETURN_ (route, Call_In_Route(name));
		if (route == NULL)
		    return;
		if (route->Init(cirRootCfg) == false) {
		    session->SendErrorf("Could not create route '%s'" NEWLINE, name.c_str());
		    delete route;
                    route = NULL;
		    return;
		}
		if (session->GetStream().IsInteractive()) 
		    session->Sendf("Route '%s' created" NEWLINE, name.c_str());

		if (!ACQUIRE_SYSTEM_LOCK())
		    return;

		cirList.push_back (route);
		CIR_Resort (route);
		RELEASE_SYSTEM_LOCK();

		session->SetRoot(route->GetConfigTree());
	    }

	} else {
	    if (!ACQUIRE_SYSTEM_LOCK())
		return;

	    Call_In_Route *route = FindRoute(name);
	    RELEASE_SYSTEM_LOCK();
	    if ( route == NULL ) {
		session->SendErrorf ( "Could not find route '%s'" NEWLINE, name.c_str());
		return;
	    }

	    if (session->GetStream().IsInteractive()) {
		if (session->PromptYesNo(false, "Delete route '%s' (y/n)?", name.c_str()) == false)
		    return;
	    }

	    Config_Controller::Instance()->NotifyEntryDeleted(session, route->GetConfigTree());

	    if (!ACQUIRE_SYSTEM_LOCK())
		return;
	    cirList.remove (route);
	    RELEASE_SYSTEM_LOCK();
	    delete route;
            route = NULL;
	    session->Sendf ("Route '%s' deleted" NEWLINE, name.c_str());
	}
	break;
    case Term_Cmd_TestRoute:
	{
	    CVariant v0, v1, v2, v3;
	    cmd->GetArg((size_t)0)->GetValue(v0);
	    cmd->GetArg((size_t)1)->GetValue(v1);
	    cmd->GetArg((size_t)2)->GetValue(v2);
	    cmd->GetArg((size_t)3)->GetValue(v3);
	    Cmd_TestRoute (session, v1.ToString(), v0.ToString(), v2.ToString(), v3.ToString(), String());
	}
    case Term_Cmd_App_GlobalParam:
	{
	    session->SetRoot (_appGlobalParam->GetConfigTree());
	}
	break;
    case Term_Cmd_AppTraceFilter:
	cmd->GetArg((size_t)0)->GetValue(v);
	name = v.AsString();
	if (name == "list") {
	    if (appTraceCriteria.empty()) {
		session->Sendf("No trace filter defined\n");
		return;
	    }

	    String output;
	    char temp[64];

	    output += "Application trace criteria:\n"
		      "     id  String pattern\n"
		      "=======================================================\n";

	    List <App_Trace_Criteria*>::iterator it, end;

	    if (!ACQUIRE_SYSTEM_LOCK())
		return;

	    end = appTraceCriteria.end();
	    for (it=appTraceCriteria.begin(); it!=end; ++it) {
		App_Trace_Criteria *crit = *it;
		sprintf(temp, "% 7d  ", crit->id);
		output += temp;
		output += crit->definition;
		output += "\n";
	    }

	    RELEASE_SYSTEM_LOCK();
	    session->Send(output);

	} else if (name == "add") {
	    cmd->GetArg((size_t)1)->GetValue(v);
	    if (!ACQUIRE_SYSTEM_LOCK())
		return;

	    App_Trace_Criteria *new_criteria;
	    NEW_RETURN_(new_criteria, App_Trace_Criteria(unusedTraceCriteriaId, v.AsString()));
	    if (new_criteria == NULL) {
		RELEASE_SYSTEM_LOCK();
		return;
	    }

	    if (!new_criteria->Compile()) {
		RELEASE_SYSTEM_LOCK();
		session->SendErrorf("Invalid pattern specified\n");
		return;
	    }

	    appTraceCriteria.push_back(new_criteria);
	    ++unusedTraceCriteriaId;
	    RELEASE_SYSTEM_LOCK();

	} else if (name == "delete") {
	    cmd->GetArg(1)->GetValue(v);
	    int id;

	    if (!strcasecmp(v.AsString(), "all"))
		id = -1;
	    else {
		char *err;
		id = strtol(v.AsString(), &err, 10);
		if (*err) {
		    session->SendErrorf("Invalid trace id (specify a number or 'all')\n");
		    return;
		}
	    }

	    List <App_Trace_Criteria*>::iterator it, end;

	    if (!ACQUIRE_SYSTEM_LOCK())
		return;

	    bool found = false;
	    end = appTraceCriteria.end();
	    for (it=appTraceCriteria.begin(); it!=end;) {
		App_Trace_Criteria *crit = *it;
		if (id==-1 || crit->id == id) {
		    delete crit;
		    it = appTraceCriteria.erase(it);
		    found = true;
		    if (id!=-1)
			break;
		} else
		    ++it;
	    }
	    RELEASE_SYSTEM_LOCK();

	    if (!found) {
		session->SendErrorf("The specified criteria id was not found\n");
	    }

	} else
	    assert(false);
	break;
    }
}

/* Test matching incoming call specifications against the call-in-routes.
 */
void CT_App_Manager::Cmd_TestRoute (Term_Session * session, 
				    const String & cli, 
				    const String & ddi, 
				    const String & port_in,
				    const String & group,
				    const String & dev_in)
{
    size_t nmatch = 0;
    String output;

    // find out routes that matched the specification.
    if (!ACQUIRE_SYSTEM_LOCK())
	return;

    List <Call_In_Route *>::iterator it, end;
    end = cirList.end();
    for (it = cirList.begin(); it!=end; ++it) {
	Call_In_Route *route = *it;
	if (!route->IsShutdown()) {
	    if (route->Match(TASK_STARTUP_CALL, ddi, cli, port_in, group, "", "")) {
		++nmatch;
		output += "Route " + route->GetName() + " matched: spawn " + route->GetSpawn().c_str() + NEWLINE;
	    }
	}
    }
    RELEASE_SYSTEM_LOCK();

    if (nmatch == 0)
	session->Sendf ("No routes matched." NEWLINE);
    else {
	session->Sendf ("Found %d matched route(s)." NEWLINE, nmatch);
	session->Send(output);
	if (nmatch > 1)
	    session->Sendf("Warning: multiple matching routes. "
			   "Only the first route will be invoked." NEWLINE);
    }
}


class cir_sort {
public:
    bool operator()(const Call_In_Route* x, const Call_In_Route* y) const {
	return x->GetPriority() > y->GetPriority();
    }
};

/** Show call in routes.
 */
void CT_App_Manager::ShowCallInRoute(Term_Session *session) const
{
    String output;

    if (cirList.empty()) {
	session->Sendf("No incoming call routing defined.\n");
	return;
    }

    if (!ACQUIRE_SYSTEM_LOCK())
	return;

    // Print call in routes to a string.
    output = "Incoming call routing (highest priority will be matched first):\n"
	     " Pri Name             Usage Sta Specification\n"
	     "=================================================================\n";
    char temp[384];
    List <Call_In_Route *>::const_iterator it, end;
    end = cirList.end();
    for (it = cirList.begin(); it != end; ++it) {
	const Call_In_Route *cir = *it;
	int printed;

	if (cir->GetRouteType() == TASK_STARTUP_CALL) {
	    char temp_ddi[CIR_FILTER_ITEM_LEN];
	    if (strcmp(cir->GetDDI().c_str(), ".*")) {
		_snprintf(temp_ddi, sizeof(temp_ddi)-4, "DDI:%s", cir->GetDDI().c_str());
		temp_ddi[sizeof(temp_ddi)-4] = 0;
		strcat(temp_ddi, ", ");
	    } else temp_ddi[0] = 0;

	    char temp_cli[CIR_FILTER_ITEM_LEN];
	    if (strcmp(cir->GetCLI().c_str(), ".*")) {
		_snprintf(temp_cli, sizeof(temp_cli)-4, "CLI:%s", cir->GetCLI().c_str());
		temp_cli[sizeof(temp_cli)-4] = 0;
		strcat(temp_cli, ", ");
	    } else temp_cli[0] = 0;

	    char temp_itf[CIR_FILTER_ITEM_LEN];
	    if (strcmp(cir->GetInterface().c_str(), ".*")) {
		_snprintf(temp_itf, sizeof(temp_itf)-4, "ITF:%s", cir->GetInterface().c_str());
		temp_itf[sizeof(temp_itf)-4] = 0;
		strcat(temp_itf, ", ");
	    } else temp_itf[0] = 0;

	    char temp_grp[CIR_FILTER_ITEM_LEN];
	    if (strcmp(cir->GetGroup().c_str(), ".*")) {
		_snprintf(temp_grp, sizeof(temp_grp)-4, "GRP:%s", cir->GetGroup().c_str());
		temp_grp[sizeof(temp_grp)-4] = 0;
		strcat(temp_grp, ", ");
	    } else temp_grp[0] = 0;

	    char temp_cgap[CIR_FILTER_ITEM_LEN];
	    if (cir->GetCallGap()) {
		_snprintf(temp_cgap, sizeof(temp_cgap)-4, "CGap:%d", cir->GetCallGap());
		temp_cgap[sizeof(temp_cgap)-4] = 0;
		strcat(temp_cgap, ", ");
	    } else temp_cgap[0] = 0;

	    printed = _snprintf(temp, sizeof(temp)-3, " %3d %-16s %5d %s %s%s%s%s%sspawn %s",
				    cir->GetPriority(), cir->GetName().c_str(),
				    cir->usage_counter,
				    (cir->IsShutdown() ? "dis" : "ena"),
				    temp_ddi, temp_cli, temp_itf, temp_grp, temp_cgap,
				    cir->GetSpawn().c_str());

	} else if (cir->GetRouteType() == TASK_STARTUP_REGISTRATION) {
	    printed = _snprintf(temp, sizeof(temp)-3, " %3d %-16s %5d %s REGISTRATION method:%s domain:%s spawn %s",
					  cir->GetPriority(), cir->GetName().c_str(),
					  cir->usage_counter,
					  (cir->IsShutdown() ? "dis" : "ena"),
					  cir->GetRegMethod().c_str(), cir->GetRegDomain().c_str(),
					  cir->GetSpawn().c_str());
	} else {
	    printed = 0;
	    assert(!"Unsupported route type");
	}

	if (printed < 0)
	    printed = 80;

	temp[printed] = '\n';
	temp[printed+1] = '\0';
	output += temp;
    }
    RELEASE_SYSTEM_LOCK();

    session->Send (output);
}


/**
 Show configuration for the specified entry.
 This function will be called by the framework whenever it needs to display the
 specified entry. Previously this instance should tell the entry that it wishes to
 handle displaying the entry by calling entry->RegisterShowHandler().

 When this instance is deleted, it should call entry->UnregisterShowHandler()
 so that the handler will not be called (which will cause GPF).
 */
bool CT_App_Manager::OnShowConfig( const Cfg_Tree_Entry *entry,
				     String & output, 
				     const String & indent, 
				     int indent_size) const
{
    String new_indent = indent;
    pthread_mutex_t ct_lock;
    new_indent.append ((size_t)indent_size, ' ');
//#ifndef __linux__
    if (!ACQUIRE_SYSTEM_LOCK())
	return NOT_OK;
/*#else
    pthread_mutex_lock(&ct_lock);
#endif*/

    output += indent + "global-parameter" + NEWLINE;
    _appGlobalParam->OnShowConfig(_appGlobalParam->GetConfigTree(),output,new_indent,indent_size);
    output += new_indent + "top" + NEWLINE + NEWLINE;

    // Show applications
    Application_List_Type::const_iterator app_it, app_end;
    app_end = applicationList.end();
    for (app_it = applicationList.begin(); app_it!=app_end; ++app_it) {
	CT_Application *app = app_it->second.first;
	output += indent + "application " + app->Name() + NEWLINE;
	app->OnShowConfig_NoLock ( app->GetConfigTree(), output, new_indent, indent_size);
	output += new_indent + "top" + NEWLINE + NEWLINE;
    }

    // Show routes
    List <Call_In_Route*>::const_iterator route_it, route_end;
    route_end = cirList.end();
    for (route_it = cirList.begin(); route_it!=route_end; ++route_it) {
	Call_In_Route *route = *route_it;
	output += indent + "call-in-route " + route->GetName() + NEWLINE;
	route->OnShowConfig_NoLock ( route->GetConfigTree(), output, new_indent, indent_size);
	output += new_indent + "top" + NEWLINE + NEWLINE;
    }
//#ifndef __linux__
    RELEASE_SYSTEM_LOCK();
/*#else
    pthread_mutex_unlock(&ct_lock);
#endif*/
    return OK;
}

/** Show application statistics
 */
void CT_App_Manager::ShowApp ( Term_Session * session ) const
{
    const int size=128;
    char s[size+1];
    String output;
#define INDENT " "

    int counter=0;
    Application_List_Type::const_iterator it, end;
    if (!ACQUIRE_SYSTEM_LOCK())
	return;

    end = applicationList.end();
    for (it=applicationList.begin(); it != end; ++it) {
	CT_Application *app = it->second.first;

	unsigned n_images, n_tasks;
	app->GetAppInfo (n_images, n_tasks);

	s[size] = '\0';
	output += "Application " + app->Name();
	if (!app->IsLicensed()) {
	    output += " (using obsolete file format)";
	}
	output += NEWLINE;
	_snprintf (s, size, INDENT "Status is %s;  number of tasks: %u;  usage count: %u" NEWLINE,
			    app->IsShutdown() ? "shutdn" : "enabled",
			    n_tasks,
			    app->GetUsageCount());
	output += s;

	_snprintf (s, size, INDENT "Image count: %d, path: %s" NEWLINE, n_images, app->GetEXEFile().c_str());
	output += s;

	output += String(INDENT) + "Compile info: " + app->GetImageCompileInfo();

	_snprintf (s, size, INDENT "Stack size: %u bytes, max usage: %u bytes" NEWLINE, 
			    app->GetStackSize(), app->GetStatMaxStack());
	output += s;

	_snprintf (s, size, INDENT "Max. string stack count: %u, longest string len: %u bytes" NEWLINE
			    INDENT "Max total string len: %u bytes" NEWLINE,
			    app->GetStatMaxSStack(),
			    app->GetStatMaxString(),
			    app->GetStatMaxAllString());

	output += s;
	output += "\n";
    }
    RELEASE_SYSTEM_LOCK();
    session->Send(output);
}


/** Show application statistics
 */
void CT_App_Manager::ShowApp_Brief ( Term_Session * session ) const
{
    const int size=128;
    char s[size];
    String output;

    session->Sendf("Status\tImages\tUsage\tTasks\tStkSize\tMaxStr\tLgstStr\tTotStr\tName" NEWLINE);

    int counter=0;
    Application_List_Type::const_iterator it, end;
    if (!ACQUIRE_SYSTEM_LOCK())
	return;

    end = applicationList.end();
    for (it=applicationList.begin(); it != end; ++it) {
	CT_Application *app = it->second.first;

	unsigned n_images, n_tasks;
	app->GetAppInfo (n_images, n_tasks);
	_snprintf( s, size, "%s\t%u\t%u\t%u\t%u\t%u\t%u\t%u\t%s%s" NEWLINE, 
			    app->IsShutdown() ? "shutdn" : "enabled",
			    n_images, 
			    app->GetUsageCount(),
			    n_tasks,
			    (app->GetStatMaxStack()+1023)/1024,
			    app->GetStatMaxSStack(),
			    app->GetStatMaxString(),
			    (app->GetStatMaxAllString()+1023)/1024,
			    app->Name().c_str(),
			    (app->IsLicensed() ? "" : " (obsolete fmt)"));
	output += s;
    }
    RELEASE_SYSTEM_LOCK();
    session->Send(output);
}
