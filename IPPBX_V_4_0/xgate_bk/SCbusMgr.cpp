/*----------------------------------------------------------------* 
 * $Archive: /xGate/SCbusMgr.cpp $
 * $Date: 4/08/04 10:12a $
 * $Revision: 7 $
 *
 * $History: SCbusMgr.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Bennylp      Date: 4/08/04    Time: 10:12a
 * Updated in $/xGate
 * Fixed bug when NULL is given in FreeAddress
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 3/07/01    Time: 11:35
 * Updated in $/xGate
 * Added log display during AllocateAddress and FreeAddress
 * 
 * *****************  Version 5  *****************
 * User: Sonny        Date: 2/07/01    Time: 15:34
 * Updated in $/xGate
 * add ReserveAddress
 * 
 * *****************  Version 4  *****************
 * User: Bennylp      Date: 6/29/01    Time: 12:28p
 * Updated in $/xGate
 * Add 'show scbus' command.
 * 
 * *****************  Version 3  *****************
 * User: Junanto      Date: 8/05/01    Time: 12:54
 * Updated in $/xGate
 * Fixed memory leak on shutdown
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 30/04/01   Time: 14:55
 * Updated in $/xGate
 * Fixed bug in CTbus
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 30/04/01   Time: 12:25
 * Created in $/xGate
 * Added SCbus address and dynamic allocaltion
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 * It contains implementation of SCbus specific switching
 * address and dynamic allocation manager
 *----------------------------------------------------------------*/

#include "stdafx.h"
#include "SCbusMgr.h"
#include "log.h"
#include "term.h"


//--------------------------------------------
static const char __modname__[] = "SCbus";
#define THISMODULE		__modname__
#define NEWLINE			"\n"


//--------------------------------------------
const int MinTimeslot = 0;
const int MaxTimeslot = 1023;
const int MaxTslotPerStream = 128;


//--------------------------------------------
SCbusManager *SCbusManager::theSCbusManager = NULL;


/*----------------------------------------------------------------* 
 * Implementation of SCbusAddress
 *----------------------------------------------------------------*/

/** SCbusAddress's constructor
 */
SCbusAddress::SCbusAddress(int tslot) :
    SwitchAddress(SF_SCbus)
{
    _tslot = tslot;

    char name[32];
    sprintf(name,"SCbus[%d]",tslot);
    _name = name;
}

/** SCbusAddress's destructor
 */
SCbusAddress::~SCbusAddress() 
{
    // do nothing
}

/** Return the string representation of this address.
 */
const char *SCbusAddress::ToString()
{
    return _name.c_str();
}


/*----------------------------------------------------------------* 
 * Implementation of SCbusManager
 *----------------------------------------------------------------*/

/** SCbusManager's constructor
 */
SCbusManager::SCbusManager() :
    ServiceProvider(THISMODULE,false,false)
{
    // do nothing
}

/** SCbusManager's destructor
 */
SCbusManager::~SCbusManager()
{
    // do nothing
}

/** Create a singleton instance of SCbusManager
 */
SCbusManager *SCbusManager::Create()
{
    assert(theSCbusManager == NULL);
    theSCbusManager = new SCbusManager;
    return theSCbusManager;
}

/** Instance: Return a pointer to this singleton instance
 */
SCbusManager *SCbusManager::Instance()
{
    assert(theSCbusManager);
    return theSCbusManager;
}

/** Reserve one specified SCbus address
 */
SCbusAddress* SCbusManager::ReserveAddress(const char *requestor, const int ts) {
    ListSCbusAddress::iterator it;
    SCbusAddress* addr=NULL;

    for (it = _freeAddress.begin(); it != _freeAddress.end(); it++) {
	if ((*it)->GetTimeslot() == ts) {
	    addr = *it;
	    break;
	}
    }

    if (it != _freeAddress.end()) {
	assert(addr);
	_freeAddress.erase(it);
	_usedAddress.insert(addr);
	addr->SetOwner(requestor);
	return addr;
    }
    else {
	CLog::Error( THISMODULE, "Can't reserve an SCbus tslot for %s", requestor);
	return NULL;
    }

    assert(false);

}

/** Allocate one free SCbus address
 */
SCbusAddress *SCbusManager::AllocateAddress(const char *requestor)
{
    if (_freeAddress.empty()) {
	CLog::Error( THISMODULE, "Can't allocate a free SCbus tslot for %s", requestor);
	return NULL;
    }

    SCbusAddress *addr = _freeAddress.front();
    addr->SetOwner(requestor);

    _freeAddress.pop_front();
    _usedAddress.insert(addr);
    CLog::Detail(THISMODULE,"%s allocated to %s",addr->ToString(),requestor);
    return addr;
}

/** Free one SCbus address and put it in the free pool
 */
void SCbusManager::FreeAddress(SCbusAddress *addr)
{
    if (addr == NULL)
	return;

    _freeAddress.push_front(addr);
    _usedAddress.erase(addr);
   CLog::Detail(THISMODULE,"%s freed by %s",addr->ToString(),addr->GetOwner());
}

/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
 */
bool SCbusManager::Load()
{
    // populate the SCbus stream/tslot for use by the manager
    for (int ts=MinTimeslot;ts<=MaxTimeslot;ts++)
	_freeAddress.push_front(new SCbusAddress(ts));

    return true;
}

/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
 */
bool SCbusManager::Unload()
{
    while (!_freeAddress.empty()) {
	SCbusAddress* addr = _freeAddress.front();
	delete addr;
	_freeAddress.pop_front();
    }
    return true;
}


#define SCBUS_INDENT	"   "

/** Show SCbus usage
 */
void SCbusManager::ShowUsage(Term_Session *session)
{
    session->Sendf(SCBUS_INDENT "SCbus usage: %u/%u" NEWLINE,_usedAddress.size(),_freeAddress.size());

    std::set<SCbusAddress*,SCbus_Cmp>::iterator it = _usedAddress.begin(), end = _usedAddress.end();

    unsigned line = 1;
    while (it != end) {
	SCbusAddress* addr = *it;
	session->Sendf(SCBUS_INDENT "%d: %s allocated by %s" NEWLINE,line++,addr->ToString(),addr->GetOwner());
	++it;
    }
}
