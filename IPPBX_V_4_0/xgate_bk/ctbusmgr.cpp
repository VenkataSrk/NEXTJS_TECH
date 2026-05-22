/*----------------------------------------------------------------* 
 * $Archive: /xGate/CTbusMgr.cpp $
 * $Date: 4/08/04 10:29a $
 * $Revision: 7 $
 *
 * $History: CTbusMgr.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Bennylp      Date: 4/08/04    Time: 10:29a
 * Updated in $/xGate
 * Check if address is NULL in FreeAddress()
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 3/07/01    Time: 19:36
 * Updated in $/xGate
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 26/05/01   Time: 17:03
 * Updated in $/xGate
 * Changed CTbus allocation from LIFO to FIFO
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 26/05/01   Time: 16:12
 * Updated in $/xGate
 * Added CTbus owner's information and a new 
 * command 'show ctbus'
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
 * User: Junanto      Date: 24/04/01   Time: 14:57
 * Created in $/xGate
 * First integration of TDM switching framework.
 * Fabric supported: Aculab local bus and CT bus (H.100/H.110)
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 * It contains implementation of CTbus (H.100/H.110) specific switching
 * address and CTbus dynamic allocation manager
 *----------------------------------------------------------------*/

//#include "stdafx.h"
#include "CTbusMgr.h"
//#include "ctbusmgr.h"	//linux porting

//#include "CTbusMgr.h"
#include "log.h"
#include "term.h"


//--------------------------------------------
static const char __modname__[] = "CTbus";
#define THISMODULE		__modname__
#define CTBUS_INDENT		"  "
#define NEWLINE			"\n"


//--------------------------------------------


//--------------------------------------------
CTbusManager *CTbusManager::theCTbusManager = NULL;


/*----------------------------------------------------------------* 
 * Implementation of CTbusAddress
 *----------------------------------------------------------------*/

/** CTbusAddress's constructor
 */
CTbusAddress::CTbusAddress(int stream,int tslot) :
    SwitchAddress(SF_CTbus)
{
    _stream = stream;
    _tslot = tslot;

    char name[32];
    sprintf(name,"CTbus[%d,%d]",stream,tslot);
    _name = name;
}

/** CTbusAddress's destructor
 */
CTbusAddress::~CTbusAddress() 
{
    // do nothing
}

/** Return the string representation of this address.
 */
const char *CTbusAddress::ToString()
{
    return _name.c_str();
}


/*----------------------------------------------------------------* 
 * Implementation of CTbusManager
 *----------------------------------------------------------------*/

/** CTbusManager's constructor
 */
CTbusManager::CTbusManager() :
    ServiceProvider(THISMODULE,false,false)
{
    int idx = 0;
    for (int st=MinStream;st<=MaxStream;st++) {
	for (int ts=0;ts<TslotPerStream;ts++) {
	    pool[idx].busy = false;
	    pool[idx].addr = new CTbusAddress(st,ts);
	    idx++;
	}
    }
    pool_idx = 0;
    tslot_usage_cnt = 0;
}

/** CTbusManager's destructor
 */
CTbusManager::~CTbusManager()
{
    for (int i=0; i<MaxTslot; i++) {
	delete pool[i].addr;
    }
}

/** Create a singleton instance of CTbus manager
 */
CTbusManager *CTbusManager::Create()
{
    assert(theCTbusManager == NULL);
    theCTbusManager = new CTbusManager;
    return theCTbusManager;
}

/** Instance: Return a pointer to this singleton instance
 */
CTbusManager *CTbusManager::Instance()
{
    assert(theCTbusManager);
    return theCTbusManager;
}

/** Allocate one free CTbus address
 */
CTbusAddress *CTbusManager::AllocateAddress(const char *by)
{
    CTbusAddress *addr = 0;
    for (int i=0; i<MaxTslot; i++, pool_idx=(pool_idx+1)%MaxTslot) {
	if (!pool[pool_idx].busy) {
	    pool[pool_idx].busy = true;
	    addr = pool[pool_idx].addr;
	    pool_idx = (pool_idx+1)%MaxTslot;
	    break;
	}
    }

    if (!addr) {
	CLog::Error( THISMODULE, "Can't allocate a free CTbus timeslot for %s", by);
	return NULL;
    }

    tslot_usage_cnt++;
    addr->SetOwner(by);
    CLog::Detail(THISMODULE,"%s allocated to %s", addr->ToString(), by);
    return addr;
}

/** Free one CTbus address and put it in the free pool
 */
void CTbusManager::FreeAddress(CTbusAddress *addr)
{
    if (addr == NULL)
	return;

    int idx = addr->GetStream()*TslotPerStream + addr->GetTimeslot();
    if (pool[idx].busy) {
	tslot_usage_cnt--;
	pool[idx].busy = false;
	CLog::Detail(THISMODULE,"%s freed by %s", addr->ToString(), addr->GetOwner());
    }
}

/** Show CTbus usage
 */
void CTbusManager::ShowUsage(Term_Session *session)
{
    session->Sendf(CTBUS_INDENT "CTbus usage: %u/%u" NEWLINE, tslot_usage_cnt, MaxTslot-tslot_usage_cnt);

    unsigned line = 1;
    for (int i=0; i<MaxTslot; i++) {
	if (pool[i].busy) {
	    CTbusAddress* addr = pool[i].addr;
	    session->Sendf(CTBUS_INDENT "%d: %s allocated by %s" NEWLINE, line++, addr->ToString(), addr->GetOwner());
	}
    }
}

/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
 */
bool CTbusManager::Load()
{
    return true;
}

/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
 */
bool CTbusManager::Unload()
{
    return true;
}


/** Reserve one specified CTbus address */
CTbusAddress* CTbusManager::ReserveAddress(const char* requestor, int ts, int st)
{
    int idx = st*TslotPerStream + ts;
    if (pool[idx].busy) {
	CLog::Error( THISMODULE, "Can't reserve CTbus [%d,%d] for %s as it is already owned by %s", st, ts, requestor, pool[idx].addr->GetOwner());
	return 0;
    }

    pool[idx].busy = true;
    pool[idx].addr->SetOwner(requestor);
    return pool[idx].addr;
}

