/*----------------------------------------------------------------* 
 * $Archive: /xGate/AculabSwitch.cpp $
 * $Date: 3/06/03 5:10p $
 * $Revision: 16 $
 *
 * $History: AculabSwitch.cpp $
 * 
 * *****************  Version 16  *****************
 * User: Bennylp      Date: 3/06/03    Time: 5:10p
 * Updated in $/xGate
 * Fixed uninitialized memory read
 * 
 * *****************  Version 15  *****************
 * User: Junanto      Date: 30/04/02   Time: 17:39
 * Updated in $/xGate
 * Aculab switching changed:
 * - added option "looped back when idle" for Prosody
 * - at destruction, put back original switching mode
 * 
 * *****************  Version 14  *****************
 * User: Junanto      Date: 31/01/02   Time: 18:44
 * Updated in $/xGate
 * Original switching mode is memorized and put back
 * during idle time
 * 
 * *****************  Version 13  *****************
 * User: Bennylp      Date: 13/07/01   Time: 18:26
 * Updated in $/xGate
 * Fixed bug in GetCompatibleFabric
 * 
 * *****************  Version 12  *****************
 * User: Bennylp      Date: 7/02/01    Time: 7:01p
 * Updated in $/xGate
 * 
 * *****************  Version 11  *****************
 * User: Bennylp      Date: 7/02/01    Time: 6:56p
 * Updated in $/xGate
 * 
 * *****************  Version 10  *****************
 * User: Bennylp      Date: 7/02/01    Time: 6:36p
 * Updated in $/xGate
 * 
 * *****************  Version 9  *****************
 * User: Bennylp      Date: 6/29/01    Time: 12:30p
 * Updated in $/xGate
 * Added owner info in SCbusManager::AllocateAddress.
 * 
 * *****************  Version 8  *****************
 * User: Bennylp      Date: 6/13/01    Time: 8:31p
 * Updated in $/xGate
 * Aculab switch saves the transmit address.
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 8/06/01    Time: 10:37
 * Updated in $/xGate
 * Removed some CLog::Detail
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 6/06/01    Time: 10:47
 * Updated in $/xGate
 * Changed implementation of DeviceStopReceive from DISABLE
 * to PATTERN_MODE
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 26/05/01   Time: 16:12
 * Updated in $/xGate
 * Added CTbus owner's information and a new 
 * command 'show ctbus'
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 30/04/01   Time: 13:03
 * Updated in $/xGate
 * corrected some typographical error
 * 
 * *****************  Version 3  *****************
 * User: Junanto      Date: 30/04/01   Time: 12:25
 * Updated in $/xGate
 * Added SCbus address and dynamic allocaltion
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 24/04/01   Time: 18:25
 * Updated in $/xGate
 * first link between Prosody/Aculab Media and switching framework
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
 *  This is specific implementation of Aculab TDM switching 
 *----------------------------------------------------------------*/

#include <pthread.h>
#include "stdafx.h"
#include "xgconfig.h"
#ifdef XGATE_HAS_ACULABSWITCH

#include "aculabswitch.h"
#include "log.h"

#include "acu_type.h"
#include "sw_lib.h"

#include <iostream>

#define SWMODE_CTBUS_MVIP       0
#define SWMODE_CTBUS_SCBUS      1
#define SWMODE_CTBUS_H100       2
#define SWMODE_CTBUS_PEB        3
#define SWMODE_CTBUS_MC3        4


//--------------------------------------------
static const char __modname__[] = "AcuSwitch";
#define THISMODULE		__modname__
#define DEBUG_SWITCHING		0


/*----------------------------------------------------------------* 
 * Some helper static functions
 *----------------------------------------------------------------*/

/** Return the string representation of switching mode
 */
static const char *XlatModeToString(int mode)
{
    switch (mode) {
    case CONNECT_MODE:
	return "CONNECT";

    case DISABLE_MODE:
	return "DISABLE";

    case PATTERN_MODE:
	return "PATTERN";

    case FRAMED_CONNECT_MODE:
	return "FRAMED_CONNECT";

    default:
	return "*UnknownMode*";
    }

}


/*----------------------------------------------------------------* 
 * Implementation of AculabAddress
 *----------------------------------------------------------------*/

/** AculabAddress's constructor
 */
AculabAddress::AculabAddress(const char* serial_num, int swdrv,int stream,int timeslot,tSMModuleId module_id) :
    SwitchAddress(SF_Aculab)
{
    _swdrv = swdrv;
    _stream = stream;
    _timeslot = timeslot;
    _module_id = module_id;

    char name[32];
    sprintf(name,"ACU[%s:%d:%d]", serial_num, stream, timeslot);
    _name = name;
}

/** AculabAddress's destructor
 */
AculabAddress::~AculabAddress() 
{
    // do nothing
}

/** Return the string representation of this address. It must be
    implemented by all descendants
 */
const char *AculabAddress::ToString()
{
    return _name.c_str();
}

/** Pool of Aculab H-Bus addresses
 **/
class HBusPool
{
    struct Timeslot
    {
	bool busy;
	AculabAddress* addr;
    };

    Timeslot* _pool;
    int _next_addr;
    int _pool_maxsz;
    int _pool_avail;
    int _start_stream;
    int _ts_per_stream;

public:
    // constructor
    HBusPool(const char* serial_num, int swdrv, tSMModuleId module_id, int start_stream, int ts_per_stream)
    {
	_start_stream = start_stream;
	_ts_per_stream = ts_per_stream;
	_next_addr = 0;
	_pool_maxsz = 2 * ts_per_stream;
	_pool = (Timeslot*) malloc(_pool_maxsz*sizeof(Timeslot));
	_pool_avail = 0;
	for (int st=start_stream; st<start_stream+2; st++)
	    for (int ts=0; ts<ts_per_stream; ts++) {
		_pool[_pool_avail].busy = false;
		_pool[_pool_avail].addr = new AculabAddress( serial_num, swdrv, st, ts, module_id );
		_pool_avail++;
	    }
    }

    // destructor
    virtual ~HBusPool()
    {
	for (int i=0; i<_pool_maxsz; i++)
	    delete _pool[i].addr;

	free(_pool);
    }

    // allocate from the free pool, an H-Bus address for this module
    AculabAddress* allocate_address()
    {
	for (int i=0; i<_pool_maxsz; i++, _next_addr=(_next_addr+1)%_pool_maxsz) {
	    if (!_pool[_next_addr].busy) {
		_pool[_next_addr].busy = true;
		AculabAddress* addr = _pool[_next_addr].addr;
		_next_addr=(_next_addr+1)%_pool_maxsz;
		_pool_avail--;
		return addr;
	    }
	}
	return 0;
    }

    // put the specified H-Bus address back in the pool
    void free_address(AculabAddress* addr)
    {
	int index = (addr->GetStream() - _start_stream)*_ts_per_stream + addr->GetTimeslot();
	if (_pool[index].busy) {
	    _pool[index].busy = false;
	    _pool_avail++;
	}
	return;
    }

    int count_free_address()
    {
	return _pool_avail;
    }
};

AculabBusManager::AculabBusManager()
{
    instance = this;
}

AculabBusManager::~AculabBusManager()
{
    map_of_pools::iterator it;
    for (it = modules.begin(); it != modules.end(); it++) {
	delete (*it).second;
    }
}

AculabAddress* AculabBusManager::AllocateAddress( const char* serial_num, const char* owner, int swdrv, int stream, int timeslot, tSMModuleId module_id )
{
    AculabAddress* addr = 0;
    
    if (module_id != 0) {
	map_of_pools::iterator it = modules.find(module_id);
	if (it == modules.end()) {
	    CLog::Error(THISMODULE, "Allocate H-Bus address requested on unregistered module %x", module_id);
	    return 0;
    	}

	HBusPool* module = (*it).second;
	addr = module->allocate_address();
	if (!addr) {
	    CLog::Error(THISMODULE, "No more H-Bus address available in module %x", module_id);
	    return 0;
	}
    } else {
	addr = new AculabAddress( serial_num, swdrv, stream, timeslot );
    }
    
    CLog::Detail( THISMODULE, "%s allocated to %s", addr->ToString(), owner );
    addr->SetOwner(owner);
    
    return addr;
}

void AculabBusManager::FreeAddress( AculabAddress* address )
{
    tSMModuleId module_id = address->GetModule();

    if (module_id != 0) {
	map_of_pools::iterator it = modules.find(module_id);
	if (it == modules.end()) {
	    CLog::Fatal(THISMODULE, "H-Bus address %s freed on unregistered module %x", address->ToString(), module_id);
	    delete address;
	    return;
    	}

	HBusPool* module = (*it).second;
	module->free_address(address);
	CLog::Detail( THISMODULE, "%s released", address->ToString() );
	return;
    }

    CLog::Detail( THISMODULE, "%s released", address->ToString() );
    delete address;
}

int AculabBusManager::GetNumAddressRemaining(tSMModuleId module_id )
{
    map_of_pools::iterator it = modules.find(module_id);
    if (it == modules.end()) {
	CLog::Fatal(THISMODULE, "H-Bus counter requested on unregistered module %x", module_id);
	return 0;
    }

    HBusPool* module = (*it).second;
    return module->count_free_address();
}

void AculabBusManager::RegisterModule(const char* serial_num, int swdrv, tSMModuleId module_id, int minStream, int numTimeslotsPerStream )
{
    map_of_pools::iterator it = modules.find(module_id);
    if (it != modules.end()) {
	CLog::Error(THISMODULE, "Module %s:%x registered more than once", serial_num, module_id);
	return;
    }

    HBusPool* module = new HBusPool(serial_num, swdrv, module_id, minStream, numTimeslotsPerStream);
    modules[module_id] = module;
}

AculabBusManager* AculabBusManager::Instance()
{
    return instance;
}


AculabBusManager* AculabBusManager::instance = 0;
static AculabBusManager bus_mgr_singleton;


/** The following functions reduce calling unnecessarily Aculab driver by caching the result
 **/
int cache_ct_bus_mode(int swdrv)
{
    static Map<int, int> swdrv_to_mode;
    Map<int, int>::iterator it;

    it = swdrv_to_mode.find(swdrv);
    if (it == swdrv_to_mode.end()) {
	// get the value, cache and return the result
	SWMODE_PARMS swmode;
	INIT_ACU_STRUCT(&swmode);

	sw_mode_switch(swdrv, &swmode);
	swdrv_to_mode[swdrv] = swmode.ct_buses;
	return swmode.ct_buses;
    }

    return it->second;
}


/*----------------------------------------------------------------* 
 * Implementation of AculabSwitch
 *----------------------------------------------------------------*/

/** AculabSwitch's constructor
 */
AculabSwitch::AculabSwitch(
    const char *name, 
    const char* serial_num, 
    int swdrv, int stream, int timeslot,
    bool disable_when_idle,
    tSMModuleId module_id
    ) 
: SwitchDevice(name)
#ifdef ACU_REMEMBER_ADDRESS
  ,_txCTbusAddr(-1, -1), _txSCbusAddr(-1)
#endif
{ 
    _swdrv = swdrv;
    strcpy(_serial_num, serial_num);

    _addrLocal = AculabBusManager::Instance()->AllocateAddress( serial_num, name, swdrv, stream, timeslot, module_id );

    _addrCTbus = NULL;
    _addrSCbus = NULL;

#ifdef ACU_REMEMBER_ADDRESS
    _txCTbus = false;
    _txSCbus = false;
#endif

    INIT_ACU_STRUCT(&_lastOutput);

    // check if we can support CTbus and/or SCbus
    _canDoCTbus = (cache_ct_bus_mode(swdrv) & (1 << SWMODE_CTBUS_H100)) ? true : false;
    _canDoSCbus = (cache_ct_bus_mode(swdrv) & (1 << SWMODE_CTBUS_SCBUS)) ? true : false;

    INIT_ACU_STRUCT(&_initialOutput);
    if (!disable_when_idle) {
	// get the original output settings
	_initialOutput.ost = _addrLocal->GetStream();
	_initialOutput.ots = _addrLocal->GetTimeslot();
	sw_query_output(swdrv, &_initialOutput);
    } else {
	_initialOutput.mode = DISABLE_MODE;
	_initialOutput.ost = _addrLocal->GetStream();
	_initialOutput.ots = _addrLocal->GetTimeslot();
    }
}

/** AculabSwitch's destructor
 */
AculabSwitch::~AculabSwitch()
{
    if (_addrLocal) AculabBusManager::Instance()->FreeAddress(_addrLocal); 
    if (_addrCTbus) CTbusManager::Instance()->FreeAddress(_addrCTbus);
    if (_addrSCbus) SCbusManager::Instance()->FreeAddress(_addrSCbus);

    DeviceSetOutput(_swdrv, &_initialOutput);
}

/** The framework will query the device to find out which 
    switching fabric is supported by this device to exchange TDM
    data with another device.
 */
SwitchFabric AculabSwitch::GetCompatibleFabric(SwitchDevice *dev)
{
    if( dev->SupportAculab(this) ) {
	return SF_Aculab;
    } else {
	if( _canDoCTbus && dev->SupportCTbus(this) ) {
	    return SF_CTbus;
	} else if( _canDoSCbus && dev->SupportSCbus(this) ) {
	    return SF_SCbus;
	}
    }
    
    return SF_Invalid;
    
}

/** This function is called when the framework decides that it is
    the first time the specific device transmits to a particular
    fabric.
 */
SwitchAddress *AculabSwitch::AllocateAddress(SwitchFabric fabric) 
{
    switch (fabric) {
    case SF_Aculab:
	return _addrLocal;

    case SF_CTbus:
	_addrCTbus = CTbusManager::Instance()->AllocateAddress(GetName());
	return _addrCTbus;

    case SF_SCbus:
	_addrSCbus = SCbusManager::Instance()->AllocateAddress(GetName());
	return _addrSCbus;

    default:
	assert(false);
	return false;
    }
}

/** This function is called when the framework detects that no device
    is currently listening to this device.
 */
void AculabSwitch::FreeAddress(SwitchFabric fabric)
{
    switch (fabric) {
    case SF_Aculab:
	break;

    case SF_CTbus:
	CTbusManager::Instance()->FreeAddress(_addrCTbus);
	_addrCTbus = NULL;
	break;

    case SF_SCbus:
	SCbusManager::Instance()->FreeAddress(_addrSCbus);
	_addrSCbus = NULL;
	break;

    default:
	assert(false);
    }
}

/** It returns the address used for switching in the specified fabric
 */
SwitchAddress *AculabSwitch::GetAddress(SwitchFabric fabric)
{
    switch (fabric) {
    case SF_Aculab:
	return _addrLocal;

    case SF_CTbus:
	return _addrCTbus;

    case SF_SCbus:
	return _addrSCbus;

    default:
	assert(false);
	return NULL;
    }
}

/** This device support Aculab switching fabric if another device
    (which must be Aculab) is located on the same board
 */
bool AculabSwitch::SupportAculab(SwitchDevice *requester)
{
    AculabSwitch* swreq = (AculabSwitch*) requester;

    // different card?
    if (strcmp(swreq->GetCardSerialNum(), GetCardSerialNum())) 
	return false;

    // On Prosody/X DSP farm #1 can't talk directly to DSP farm #2
    // A DSP farm is a group of 4 DSP
    int local_stream = _addrLocal->GetStream();
    int req_stream = swreq->_addrLocal->GetStream();

    // Am I from DSP farm #1 and you are DSP farm #2
    if (belongs_to_prosodyx_dsp_farm_1(local_stream) && belongs_to_prosodyx_dsp_farm_2(req_stream))
	return false;

    // Or the other way around
    if (belongs_to_prosodyx_dsp_farm_2(local_stream) && belongs_to_prosodyx_dsp_farm_1(req_stream))
	return false;

    return true;
}

/** This device supports CTbus (H.100/H.110)
 */
bool AculabSwitch::SupportCTbus(SwitchDevice *requester)
{
    return _canDoCTbus;
}

/** This device supports SCbus
 */
bool AculabSwitch::SupportSCbus(SwitchDevice *requester)
{
    return _canDoSCbus;
}

/** Execute sw_set_output call and display activity and error to the log 
 */
bool AculabSwitch::DeviceSetOutput(int swdrv,OUTPUT_PARMS* parms)
{
#if 0
    // avoid doing the same thing twice
    if (memcmp(&_lastOutput, parms, sizeof(_lastOutput)) == 0)
	return true;

    int rc = sw_set_output(swdrv,parms);

    if (rc) {
	if (parms->mode == DISABLE_MODE) {
	    CLog::Error(THISMODULE,
		"%s sw_set_output(swdrv=%d,%s,ost=%d,ots=%d) return error %d",
		GetName(),
		swdrv,
		XlatModeToString(parms->mode),
		parms->ost,
		parms->ots,
		rc);
	} else {
	    CLog::Error(THISMODULE,
		"%s sw_set_output(swdrv=%d,%s,ist=%d,its=%d,ost=%d,ots=%d) return error %d",
		GetName(),
		swdrv,
		XlatModeToString(parms->mode),
		parms->ist,
		parms->its,
		parms->ost,
		parms->ots,
		rc);
	}
	return false;
    } else {
	memcpy(&_lastOutput, parms, sizeof(_lastOutput));
    }
#endif

    // avoid doing the same thing twice
    if (memcmp(&_lastOutput, parms, sizeof(_lastOutput)) == 0)
	return true;

    // do it asynchronously
    AculabSwitchManager::async_sw_set_output(swdrv, parms);

    // remember the last settings
    memcpy(&_lastOutput, parms, sizeof(_lastOutput));

    return true;
}


/** It instructs the device to start transmitting to a certain location 
    using the specified switch fabric.
 */
void AculabSwitch::DeviceStartTransmit(SwitchFabric fabric,SwitchAddress *addr)
{
    OUTPUT_PARMS conn;
    INIT_ACU_STRUCT( &conn );

    switch (fabric) {
    case SF_Aculab:
	// do nothing
	return;

    case SF_CTbus:
	conn.ist = _addrLocal->GetStream();
	conn.its = _addrLocal->GetTimeslot();
	conn.mode = CONNECT_MODE;
	conn.ost = ((CTbusAddress*)addr)->GetStream();
	conn.ots = ((CTbusAddress*)addr)->GetTimeslot();
#ifdef ACU_REMEMBER_ADDRESS
	_txCTbus = true;
	_txCTbusAddr = *(CTbusAddress*)addr;
#endif
	break;

    case SF_SCbus:
	conn.ist = _addrLocal->GetStream();
	conn.its = _addrLocal->GetTimeslot();
	conn.mode = CONNECT_MODE;
	conn.ost = 24;
	conn.ots = ((SCbusAddress*)addr)->GetTimeslot();
#ifdef ACU_REMEMBER_ADDRESS
	_txSCbus = true;
	_txSCbusAddr = *(SCbusAddress*)addr;
#endif
	break;

    default:
	{
	    const char* swname = addr ? addr->ToString() : "(null)";
	    CLog::Warning(THISMODULE,"%s unable to transmit to %s using invalid fabric %d",GetName(),swname,fabric);
	    return;
	}
    }

    DeviceSetOutput(_swdrv,&conn);
}

/** It instructs the device to start listening from a certain location 
    using the specified switch fabric.
 */
void AculabSwitch::DeviceStartReceive(SwitchFabric fabric,SwitchAddress *addr)
{
    OUTPUT_PARMS conn;
    INIT_ACU_STRUCT( &conn );

    switch (fabric) {
    case SF_Aculab:
	conn.ist = ((AculabAddress*)addr)->GetStream();
	conn.its = ((AculabAddress*)addr)->GetTimeslot();
	conn.mode = CONNECT_MODE;
	conn.ost = _addrLocal->GetStream();
	conn.ots = _addrLocal->GetTimeslot();
	break;

    case SF_CTbus:
	conn.ist = ((CTbusAddress*)addr)->GetStream();
	conn.its = ((CTbusAddress*)addr)->GetTimeslot();
	conn.mode = CONNECT_MODE;
	conn.ost = _addrLocal->GetStream();
	conn.ots = _addrLocal->GetTimeslot();
	break;

    case SF_SCbus:
	conn.ist = 24;
	conn.its = ((SCbusAddress*)addr)->GetTimeslot();
	conn.mode = CONNECT_MODE;
	conn.ost = _addrLocal->GetStream();
	conn.ots = _addrLocal->GetTimeslot();
	break;

    default:
	{
	    const char* swname = addr ? addr->ToString() : "(null)";
	    CLog::Warning(THISMODULE,"%s unable to receive from %s using invalid fabric %d",GetName(),swname,fabric);
	    return;
	}
    }

    DeviceSetOutput(_swdrv,&conn);
}

/** It instructs the device to stop listening from a certain location 
    using the specified switch fabric.
 */
void AculabSwitch::DeviceStopTransmit(SwitchFabric fabric,SwitchAddress *addr)
{
    OUTPUT_PARMS conn;
    INIT_ACU_STRUCT( &conn );

    switch (fabric) {
    case SF_Aculab:
	// do nothing
	return;

    case SF_CTbus:
#ifdef ACU_REMEMBER_ADDRESS
	if (!_txCTbus) return;
	_txCTbus = false;
	if (addr == NULL) {
	    if (_txCTbusAddr.GetStream() == -1) {
		assert (false);
		CLog::Warning (THISMODULE, "DeviceStopTransmit: device '%s' is not transmitting to CTBUS",
					   GetName());
		return;
	    }
	    addr = &_txCTbusAddr;
	}
#endif
	conn.mode = DISABLE_MODE;
	conn.ost = ((CTbusAddress*)addr)->GetStream();
	conn.ots = ((CTbusAddress*)addr)->GetTimeslot();

#ifdef ACU_REMEMBER_ADDRESS
	_txCTbusAddr = CTbusAddress(-1, -1);
#endif
	break;

    case SF_SCbus:
#ifdef ACU_REMEMBER_ADDRESS
	if (!_txSCbus) return;
	_txSCbus = false;
	if (addr == NULL) {
	    if (_txSCbusAddr.GetTimeslot() == -1) {
		assert (false);
		CLog::Warning (THISMODULE, "DeviceStopTransmit: device '%s' is not transmitting to SCBUS",
					   GetName());
		return;
	    }
	    addr = &_txSCbusAddr;
	}
#endif
	conn.mode = DISABLE_MODE;
	conn.ost = 24;
	conn.ots = ((SCbusAddress*)addr)->GetTimeslot();

#ifdef ACU_REMEMBER_ADDRESS
	_txSCbusAddr = SCbusAddress(-1);
#endif
	break;

    default:
	{
	    const char* swname = addr ? addr->ToString() : "(null)";
#ifdef ACU_REMEMBER_ADDRESS
	    CLog::Error(THISMODULE,"%s unable to stop transmit to %s using invalid fabric %d, attempting to recover",GetName(),swname,fabric);
	    if (_txCTbus) DeviceStopTransmit(SF_CTbus,NULL);
	    if (_txSCbus) DeviceStopTransmit(SF_SCbus,NULL);
#else
	    CLog::Error(THISMODULE,"%s unable to stop transmit to %s using invalid fabric %d",GetName(),swname,fabric);
#endif
	    return;
	}
    }

    DeviceSetOutput(_swdrv,&conn);
}

/** It instructs the device to stop transmitting to a certain location 
    using the specified switch fabric. It is called when the reference
    count of the specified switch fabric reach 0.
 */
void AculabSwitch::DeviceStopReceive(SwitchFabric fabric,SwitchAddress *addr)
{
    DeviceSetOutput(_swdrv,&_initialOutput);
}

/** It returns the switch driver index associated with this device
 */
int AculabSwitch::GetSwitchDriver() 
{
    return _swdrv;
}

#include "thread_process.h"

static AculabSwitchManager* self = 0;

AculabSwitchManager::AculabSwitchManager()
{
#ifndef __linux__
    _event = CreateEvent(0, 0, 0, 0);
#endif
    _quit = false;

    DWORD tid;
#ifndef __linux__
    _thread = CREATE_THREAD(0, 0, async_thread, 0, 0, &tid);
#else
#ifdef COND_WAIT
    _event = CreateEvent();
#endif

//    pthread_t aculab_thread;
    pthread_create(&aculab_thread,NULL,async_thread,NULL);
#endif
    //SetThreadPriority(_thread, THREAD_PRIORITY_BELOW_NORMAL);
}

AculabSwitchManager::~AculabSwitchManager()
{
    if (!_quit) stop();
    self = 0;
}

void AculabSwitchManager::stop()
{
    //printf("\n IN Stop :: Setting Event \n");
    if (!self) return;

    self->_quit = true;
#ifndef __linux__
    SetEvent(self->_event);

    if (WaitForSingleObject(self->_thread, 5000) == WAIT_TIMEOUT) {
	CLog::Warning(THISMODULE, "Unable to stop Aculab switching thread");
    }
#else
    //printf("\n IN Stop :: Setting Event \n");
    self->SetEvent(self->_event);

    if (self->WaitForEvent(self->_thread, -1) == WAIT_TIMEOUT) {
    }
   /* Write code for linux for above ifndef please */
#endif
}

void AculabSwitchManager::start()
{
    if (self) return;
    self = new AculabSwitchManager();
}

void AculabSwitchManager::async_sw_set_output(int swdrv, OUTPUT_PARMS* parms)
{
    Job* job = new Job;
    job->swdrv = swdrv;
    job->parms = *parms;

    {
	/* Narsim commented */
	Lock l(self->_cs);
	self->_jobs.push_back(job);
    }
#ifndef __linux__
    SetEvent(self->_event);
#else
    self->SetEvent(self->_event);
    /* Linux specific code */
#endif
}

DWORD  AculabSwitchManager::async_thread(void *arg)
{
    while (1) {

#ifndef __linux__
	int rc = WaitForSingleObject(self->_event, 5000);
#else
       int rc;
       rc = self->WaitForEvent(self->_event, 5000);
    
        /* Linux specific code should written */
#endif

	if (self->_quit)
	    break;
	
        // no event, continue waiting
#ifndef __linux__
	if (rc == WAIT_TIMEOUT)
	    continue;
#else
	if (rc == WAIT_TIMEOUT)
    {
	    continue;
    }
#endif
	
	Job* j = 0;
	{
	    Lock l(self->_cs);
	    if (self->_jobs.empty())
		continue;

	    j = self->_jobs.front();
	    self->_jobs.pop_front();
#ifndef __linux__
	    if (!self->_jobs.empty())
		SetEvent(self->_event);
#else
	    if (!self->_jobs.empty()){
           //printf("\n JOBS not Empty ... Set Event \n");
		   self->SetEvent(self->_event);
        }
#endif
	}

	rc = sw_set_output(j->swdrv, &j->parms);
	if (rc) {
	    if (j->parms.mode == DISABLE_MODE) {
		CLog::Error(THISMODULE,
		    "sw_set_output(swdrv=%d,%s,ost=%d,ots=%d) return error %d",
		    j->swdrv,
		    XlatModeToString(j->parms.mode),
		    j->parms.ost,
		    j->parms.ots,
		    rc);
	    } else {
		CLog::Error(THISMODULE,
		    "sw_set_output(swdrv=%d,%s,ist=%d,its=%d,ost=%d,ots=%d) return error %d",
		    j->swdrv,
		    XlatModeToString(j->parms.mode),
		    j->parms.ist,
		    j->parms.its,
		    j->parms.ost,
		    j->parms.ots,
		    rc);
	    }
	}

	delete j;    
    }

    Lock l(self->_cs);
    while (!self->_jobs.empty()) {
	Job* j = self->_jobs.front();
	delete j;
	self->_jobs.pop_front();
    }

    return 0;
}

#ifdef COND_WAIT
am_event_t AculabSwitchManager::CreateEvent(bool manualReset, bool initialState)
{
    am_event_t event = new am_event_t_;

    int result = pthread_cond_init(&event->CVariable, 0);
    assert(result == 0);

    result = pthread_mutex_init(&event->Mutex, 0);
    assert(result == 0);

    event->State = false;
    event->AutoReset = !manualReset;

    if (initialState)
    {
        result = SetEvent(event);
        std::cout << "EVENT = " << event << std::endl;
        assert(result == 0);
    }

    return event;
}

int AculabSwitchManager::SetEvent(am_event_t event)
{
   int result = pthread_mutex_lock(&event->Mutex);
   //printf(" Set event mutex lock = %d", result);
   assert(result == 0);

   event->State = true;
   //printf(" event state = %d, Autoreset = %d",  event->State, event->AutoReset);
   ////printf(" event state = %d, Autoreset = %d",  event->State, event->AutoReset);

   //Depending on the event type, we either trigger everyone or only one
   if (event->AutoReset)
   {
       //printf(" Event Auto reset True");
       //printf("event->State = %d...", event->State);
       if (event->State)
       {
           //printf("event->State = %d..., mutex unlock ", event->State);
           result = pthread_mutex_unlock(&event->Mutex);
           assert(result == 0);

           //printf("event->State = %d..., Cond Signal... %02x", event->State, &event->CVariable);
           result = pthread_cond_signal(&event->CVariable);
           assert(result == 0);

           return 0;
       }
   }
   else
   {
       result = pthread_mutex_unlock(&event->Mutex);
       assert(result == 0);

       result = pthread_cond_broadcast(&event->CVariable);
       assert(result == 0);
   }

   return 0;
}

int AculabSwitchManager::WaitForEvent(am_event_t event, uint64_t milliseconds)
{
   int tempResult;

   if (milliseconds == 0)
   {
       tempResult = pthread_mutex_trylock(&event->Mutex);
       if (tempResult == EBUSY)
       {
           return WAIT_TIMEOUT;
       }
   }
   else
   {
       tempResult = pthread_mutex_lock(&event->Mutex);
   }

   assert(tempResult == 0);

   int result = UnlockedWaitForEvent(event, milliseconds);

   tempResult = pthread_mutex_unlock(&event->Mutex);
   assert(tempResult == 0);

   return result;
}

int AculabSwitchManager::UnlockedWaitForEvent(am_event_t event, uint64_t milliseconds)
{
   int result = 0;

   ////printf("result = %d, event->state = %d, event->AutoReset = %d\n", result, event->State, event->AutoReset);

   if (!event->State)
   {
       //Zero-timeout event state check optimization
       if (milliseconds == 0)
       {
           return WAIT_TIMEOUT;
       }

       timespec ts;
       if (milliseconds != (uint64_t) -1)
       {
           timeval tv;
           gettimeofday(&tv, NULL);

           uint64_t nanoseconds = ((uint64_t) tv.tv_sec) * 1000 * 1000 * 1000 + milliseconds * 1000 * 1000 + ((uint64_t) tv.tv_usec) * 1000;

			   ts.tv_sec = nanoseconds / 1000 / 1000 / 1000;
           ts.tv_nsec = (nanoseconds - ((uint64_t) ts.tv_sec) * 1000 * 1000 * 1000);
       }
       do
       {
           //Regardless of whether it's an auto-reset or manual-reset event:
           //wait to obtain the event, then lock anyone else out
           if (milliseconds != (uint64_t) -1)
           {
               result = pthread_cond_timedwait(&event->CVariable, &event->Mutex, &ts);
           }
           else
           {
               //printf("Calling pthread_cond_wait %02x\n", &event->CVariable);
               result = pthread_cond_wait(&event->CVariable, &event->Mutex);
           }
       } while (result == 0 && !event->State);

       if (result == 0 && event->AutoReset)
       {
           //We've only accquired the event if the wait succeeded
           event->State = false;
           //printf("Setting event state to False\n");
       }
   }
   else if (event->AutoReset)
   {
       //It's an auto-reset event that's currently available;
       //we need to stop anyone else from using it
       result = 0;
       event->State = false;
       //printf("Auto Resett True...Setting event state to False\n");
   }
   //Else we're trying to obtain a manual reset event with a signaled state;
   //don't do anything

//   //printf("Return Result = %d\n", result);
   return result;
}
#endif


#endif
// XGATE_HAS_ACULABSWITCH
