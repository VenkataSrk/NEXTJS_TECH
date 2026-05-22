/*----------------------------------------------------------------* 
 * $Archive: /xGate/ConsoleMgr.cpp $
 * $Date: 4/23/03 5:34p $
 * $Revision: 6 $
 *
 * $History: ConsoleMgr.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Bennylp      Date: 4/23/03    Time: 5:34p
 * Updated in $/xGate
 * Administrivia: tidy-up the usage of API provider ID. Now the
 * enumeration in api.h should be used instead of bare constants.
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 8/10/01    Time: 10:25
 * Updated in $/xGate
 * Added option to exclude Console from build
 * 
 * *****************  Version 4  *****************
 * User: Bennylp      Date: 5/30/01    Time: 3:31p
 * Updated in $/xGate
 * Changed priority to Very_Low.
 * 
 * *****************  Version 3  *****************
 * User: Junanto      Date: 25/05/01   Time: 16:46
 * Updated in $/xGate
 * Console allocation is done dynamically at the first 
 * call of Display
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 1/05/01    Time: 15:14
 * Updated in $/xGate
 * add "Clear screen" when a task is lost
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 27/04/01   Time: 13:34
 * Created in $/xGate
 * added Display support to xGate
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------*
 * Description:
 * Console Server is the module which is responsible to maintain
 * application output display and to distribute each update to each of
 * connected clients (GUI display).
 *----------------------------------------------------------------*/

#include "stdafx.h"
#include "kernel.h"
#include "task.h"
#include "api.h"
#include "log.h"
#include "ConsoleMgr.h"
#include "ConServer.h"

#define USE_CONSOLE	0

//--------------------------------------------
static const char __modname__[] = "Console";
#define THISMODULE		__modname__
#define NOT_ALLOCATED		-1


//--------------------------------------------
const int ConsoleAPIProviderId = API_PROVIDER_ID_CONSOLE;


//--------------------------------------------
ConsoleManager *ConsoleManager::theConsoleManager = NULL;


/*----------------------------------------------------------------* 
 * Implementation of ConsoleAPI
 *----------------------------------------------------------------*/

class ConsoleAPI : public CAPIProvider
{
public:
    ConsoleAPI() : CAPIProvider(ConsoleAPIProviderId,THISMODULE) {
	if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
	    CLog::Error( THISMODULE, "Unable to register provider");
	    return;
	}

	/** Register API entry points
	 */
	// ordinal for entry points
	enum API_Id {
	    Id_Display,
	};

	// Display(CTask *task,int attr,const char *msg)
	RegisterAPI(
	    "Display",			// external name
	    Id_Display,			// ordinal
	    false,			// asynchronous flag
	    CVariant::VT_VOID,		// return type
	    (CAPIProvider::APIPROC) &ConsoleAPI::Display,
	    CAPIDef::CDECL_TYPE,	// calling convention
	    3,				// number of arguments
	    CArgType("task",BYVAL,CVariant::VT_TASK),
	    CArgType("attr",BYVAL,CVariant::VT_INT),
	    CArgType("msg",BYVAL,CVariant::VT_STRING));

    }

    STATUS API_CALL Display(CTask *task,int attr,const char *msg) {
	ConsoleManager::Instance()->Display(task,attr,msg);
	return OK;
    }

};


/*----------------------------------------------------------------* 
 * Implementation of ConsoleMgr
 *----------------------------------------------------------------*/

/** ConsoleManager's constructor
 */
ConsoleManager::ConsoleManager() :
    ServiceProvider(THISMODULE,true,true,Very_Low_Priority)
{
    _api = NULL;
    _nextRow = 0;
    _conServer = 0;
}

/** ConsoleManager's destructor
*/
ConsoleManager::~ConsoleManager()
{
}

/** Create a singleton instance of AculabSignalling
 */
ConsoleManager *ConsoleManager::Create()
{
    assert(theConsoleManager == NULL);

    theConsoleManager = new ConsoleManager;
    return theConsoleManager;
}

/** Instance: Returns a pointer to this singleton instance
 */
ConsoleManager *ConsoleManager::Instance()
{
    return theConsoleManager;
}

/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
 */
bool ConsoleManager::Load()
{
    _tlsId = Kernel::Instance()->AllocTLS();
    _api = new ConsoleAPI;
#if USE_CONSOLE
    _conServer = new ConsoleServer;
#endif
    return true;
}

/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
 */
bool ConsoleManager::Unload()
{
    Kernel::Instance()->FreeTLS(_tlsId);
    delete _api;
    if (_conServer) {
	delete _conServer;
	_conServer = NULL;
    }
    _api = NULL;
    return true;
}

/** This function will be called whenever a new task is created.
    Allocate an empty row for display purpose and attach it to the task
 */
bool ConsoleManager::AttachTask(CTask *task)
{
    //SetTaskRow(task,AllocRow());
    SetTaskRow(task,NOT_ALLOCATED);
    return true;
}

/** This function will be called whenever a task is about to be deleted.
    Put back the row used by this task in the pool
 */
void ConsoleManager::DetachTask(CTask *task)
{
    //int row = GetTaskRow(task);
    int row = task->GetTLSValue(_tlsId);
    if (row != NOT_ALLOCATED) {
	if (_conServer) _conServer->ClearDisplay(row,0);
	FreeRow(row);
    }
}

/** HandleEvents: It is called continuously by Service Provider Manager
 */
void ConsoleManager::HandleEvents()
{
    if (_conServer) _conServer->Timer();
}

/** AllocRow: Allocate an empty row for display purpose. If there is no
    available free row in the pool, create a new one
 */
int ConsoleManager::AllocRow()
{
    if (_freeRows.empty()) _freeRows.push(_nextRow++);
    int row = _freeRows.top();
    _freeRows.pop();
    return row;
}

/** FreeRow: Once a row is not used, it is put back in the pool
 */
void ConsoleManager::FreeRow(int row)
{
    _freeRows.push(row);
}

/** GetTaskRow: Returns the row number for a particular task
 */
int ConsoleManager::GetTaskRow(CTask *task)
{
    int row = task->GetTLSValue(_tlsId);
    if (row == NOT_ALLOCATED) SetTaskRow(task,row = AllocRow());
    return row;
}

/** SetTaskRow: Set the row number for a particular task
 */
void ConsoleManager::SetTaskRow(CTask *task,int row)
{
    task->SetTLSValue(_tlsId,row);
}

/** Display: API entry point
 */
void ConsoleManager::Display(CTask *task,int attr,const char *msg)
{
    if (_conServer) _conServer->Display(task->GetPID(),GetTaskRow(task),attr,msg);
}

