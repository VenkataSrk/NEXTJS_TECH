/*----------------------------------------------------------------* 
 * $Archive: /xGate/Switching.cpp $
 * $Date: 25/06/04 11:28 $
 * $Revision: 30 $
 *
 * $History: Switching.cpp $
 * 
 * *****************  Version 30  *****************
 * User: Sonny        Date: 25/06/04   Time: 11:28
 * Updated in $/xGate
 * 
 * *****************  Version 29  *****************
 * User: Junanto      Date: 21/04/04   Time: 20:18
 * Updated in $/xGate
 * Fixed bug in SwitchDevice::StopListen if device failed to
 * listen previously
 * 
 * *****************  Version 28  *****************
 * User: Bennylp      Date: 2/11/04    Time: 12:20p
 * Updated in $/xGate
 * Invalidate SwitchDevice signature on destructor
 * 
 * *****************  Version 27  *****************
 * User: Bennylp      Date: 1/09/04    Time: 6:22p
 * Updated in $/xGate
 * Added NULL checking before validating switch object.
 * 
 * *****************  Version 26  *****************
 * User: Bennylp      Date: 4/23/03    Time: 5:34p
 * Updated in $/xGate
 * Administrivia: tidy-up the usage of API provider ID. Now the
 * enumeration in api.h should be used instead of bare constants.
 * 
 * *****************  Version 25  *****************
 * User: Bennylp      Date: 11/04/02   Time: 7:10p
 * Updated in $/xGate
 * Fixed bug in show switching command (buffer too short)
 * 
 * *****************  Version 24  *****************
 * User: Bennylp      Date: 10/29/02   Time: 1:07p
 * Updated in $/xGate
 * Fixes: don't show switching if device is no transmitting.
 * 
 * *****************  Version 23  *****************
 * User: Sonny        Date: 9/04/02    Time: 12:53p
 * Updated in $/xGate
 * Added SetName
 * 
 * *****************  Version 22  *****************
 * User: Bennylp      Date: 8/29/02    Time: 8:17p
 * Updated in $/xGate
 * Added command "show switching"
 * 
 * *****************  Version 21  *****************
 * User: Junanto      Date: 1/03/02    Time: 16:04
 * Updated in $/xGate
 * 
 * *****************  Version 20  *****************
 * User: Junanto      Date: 3/07/01    Time: 13:33
 * Updated in $/xGate
 * 
 * *****************  Version 19  *****************
 * User: Junanto      Date: 3/07/01    Time: 13:04
 * Updated in $/xGate
 * Added error recovery in StopListen when a compatible
 * switching fabric can't be found
 * 
 * *****************  Version 18  *****************
 * User: Junanto      Date: 1/07/01    Time: 18:30
 * Updated in $/xGate
 * Added protection in StopListen if StartListen has failed
 * previously
 * 
 * *****************  Version 17  *****************
 * User: Bennylp      Date: 6/30/01    Time: 8:13p
 * Updated in $/xGate
 * Changed StartListen() to return bool, to capture error.
 * 
 * *****************  Version 16  *****************
 * User: Junanto      Date: 7/06/01    Time: 17:34
 * Updated in $/xGate
 * Just add something to eliminate warning
 * 
 * *****************  Version 15  *****************
 * User: Junanto      Date: 6/06/01    Time: 20:37
 * Updated in $/xGate
 * 
 * *****************  Version 14  *****************
 * User: Junanto      Date: 23/05/01   Time: 20:52
 * Updated in $/xGate
 * Fixed bug in pipeline switching
 * 
 * *****************  Version 13  *****************
 * User: Junanto      Date: 17/05/01   Time: 12:58
 * Updated in $/xGate
 * Temporarily disabled excessive logging
 * 
 * *****************  Version 12  *****************
 * User: Junanto      Date: 11/05/01   Time: 11:28
 * Updated in $/xGate
 * Changed logging during debug
 * 
 * *****************  Version 11  *****************
 * User: Junanto      Date: 9/05/01    Time: 10:09
 * Updated in $/xGate
 * Added some asserts
 * 
 * *****************  Version 10  *****************
 * User: Junanto      Date: 30/04/01   Time: 14:55
 * Updated in $/xGate
 * Fixed bug in CTbus
 * 
 * *****************  Version 9  *****************
 * User: Junanto      Date: 30/04/01   Time: 10:57
 * Updated in $/xGate
 * Pipeline signalling and switching done
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 29/04/01   Time: 18:02
 * Updated in $/xGate
 * SwitchAPI class declaration moved to .cpp
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 29/04/01   Time: 17:35
 * Updated in $/xGate
 * added support for pipeline signalling
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 27/04/01   Time: 16:51
 * Updated in $/xGate
 * added DisconnectListeners() to force 
 * switch listeners to unlisten from a
 * dying device
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 24/04/01   Time: 20:34
 * Updated in $/xGate
 * Added reference counting in SwitchDevice
 * and switching framework successfully tested
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 24/04/01   Time: 19:14
 * Updated in $/xGate
 * 
 * *****************  Version 3  *****************
 * User: Junanto      Date: 24/04/01   Time: 19:10
 * Updated in $/xGate
 * Added API for SwitchListen/Unlisten
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 24/04/01   Time: 18:26
 * Updated in $/xGate
 * first link between Prosody voice / Aculab media with
 * switching framework
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
 *  Each physical device which is capable of exchanging TDM stream 
 *  across a switching fabric is represented by SwitchDevice class.
 *  A switching fabric supported by a SwitchingDevice object is 
 *  represented by SwitchFabric.
 *  A SwitchDevice may be capable of supporting more than one type of
 *  switching fabric.
 *  In order for two devices to exchange TDM data, they must negotiate
 *  the switching fabric which is to be used.
 *----------------------------------------------------------------*/

//#include "stdafx.h"
#include "api.h"
#include "Switching.h"
#include "kernel.h"
#include "term.h"
#include "log.h"


//--------------------------------------------
static const char __modname__[] = "Switch";
#define THISMODULE		__modname__


//--------------------------------------------
const int SwitchAPIProviderId = API_PROVIDER_ID_SWITCHING;


//--------------------------------------------
SwitchManager *SwitchManager::theSwitchManager = NULL;


/*----------------------------------------------------------------* 
 * Implementation of SwitchManager
 *----------------------------------------------------------------*/

/** SwitchAPI : Implements methods exposed to Kernel for media switching made on behalf of user 
    applications
 */
class SwitchAPI : public CAPIProvider
{
public:
    /** SwitchAPI's constructor
     */
    SwitchAPI() : CAPIProvider(SwitchAPIProviderId,THISMODULE) {
	if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
	    CLog::Error( THISMODULE, "Unable to register provider");
	    return;
	}

	/** Register API entry points
	 */
	// ordinal for entry points
	enum API_Id {
	    Id_Listen,
	    Id_Unlisten,
	};

	// Listen(CTask *task,int *retcode,SupportSwitching *target,SupportSwitching *source)
	RegisterAPI(
	    "SwitchListen",				// external name
	    Id_Listen,				// ordinal
	    true,					// asynchronous flag
	    CVariant::VT_INT,			// return type
	    (CAPIProvider::APIPROC) &SwitchAPI::Listen,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    4,					// number of arguments
	    CArgType("task",BYVAL,CVariant::VT_TASK),
	    CArgType("retval",BYREF,CVariant::VT_RET),
	    CArgType("target",BYVAL,CVariant::VT_STRING),
	    CArgType("source",BYVAL,CVariant::VT_INT));

	// Unlisten(CTask *task,int *retcode,SupportSwitching *target)
	RegisterAPI(
	    "SwitchUnlisten",			// external name
	    Id_Unlisten,				// ordinal
	    true,					// asynchronous flag
	    CVariant::VT_INT,			// return type
	    (CAPIProvider::APIPROC) &SwitchAPI::Unlisten,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    3,					// number of arguments
	    CArgType("task",BYVAL,CVariant::VT_TASK),
	    CArgType("retval",BYREF,CVariant::VT_RET),
	    CArgType("target",BYVAL,CVariant::VT_INT));
    }

    /** SwitchAPI's destructor
     */
    virtual ~SwitchAPI() {
	// do nothing
    }

    /** Entry point for SwitchListen API
     */
    STATUS API_CALL Listen(CTask *task,int *retcode,const char *targ,SupportSwitching *source) {
	*retcode = -1;
        SupportSwitching *target=NULL;
	char *err;
        target = strtol( targ, &err, 10);
      //  printf("source =%d target = %d\n", source, target);
	SwitchDevice *devTarget = NULL;
	SwitchDevice *devSource = NULL;
	// check device validity
	bool valid = false;
	if (target && source) {
	    try {
		devTarget = target->GetSwitchDevice();
		devSource = source->GetSwitchDevice();
               //  printf("devSource =%d devTarget = %d\n", devSource, devTarget);
                if(devTarget && devSource)
                {
		    valid = (devTarget->IsValid() && devSource->IsValid());
                }
	    } catch (...) {
		valid = false;
	    }
	} else {
	    valid = false;
	}

	if (valid) {
	    devTarget->Listen(devSource);
	    *retcode = 0;
	} else {
	    CLog::Error(THISMODULE,"%s unable to listen to invalid media 0x%x",GetName(),source);
	}

	return OK;
    }

    /** Entry point for SwitchUnlisten API
     */
    STATUS API_CALL Unlisten(CTask *task,int *retcode,SupportSwitching *target) {
	*retcode = -1;
	if (!target)
	{
		return OK;
	}
	try {
	    SwitchDevice *devTarget = target->GetSwitchDevice();

	    //***CLog::Info(THISMODULE, task, "%s stops listening from %s",devTarget->GetName());

	    devTarget->Unlisten();
	    *retcode = 0;
	} catch (...) {
	    CLog::Error(THISMODULE,task,"Unable to unlisten");
	}

	return OK;
    }
};


/*----------------------------------------------------------------* 
 * Implementation of SwitchDevice
 *----------------------------------------------------------------*/

/** SwitchDevice's constructor
 */
SwitchDevice::SwitchDevice(const char *name) 
{
    _transmitter = NULL;
    _name = name;
    _listenFabric = SF_Invalid;
    _listenAddr = NULL;

    _signature = DEADCODE;

    _refcntAculab = 0;
    _refcntCTbus = 0;
    _refcntSCbus = 0;
    _refcntKallastra = 0;
    _refcntHostmedia = 0;

    _refCount = 0;

    _iter = _listener.end();

#ifdef SWITCH_MANAGE_DEVICES
    SwitchManager::Instance()->AddDevice(this);
#endif
}

/** SwitchDevice's destructor
 */
SwitchDevice::~SwitchDevice()
{
    // Invalidate signature
    _signature = (MagicWord) ~DEADCODE;

    DisconnectListeners();
    Unlisten();

#ifdef SWITCH_MANAGE_DEVICES
    SwitchManager::Instance()->RemoveDevice(this);
#endif

}

bool SwitchDevice::IsValid() const 
{
    try {
	return (_signature == DEADCODE);
    } catch (...) {
	return false;
    }
}

/** GetName: return the name of this device
 */
const char *SwitchDevice::GetName()
{
    return _name.c_str();
}

/** SetName: set the name of this device
 */
void SwitchDevice::SetName(const char *name)
{
    _name = name;
}

/** It must be overridden by the descendant if it support
    Aculab switching fabric.
    Default implementation returns false.
 */
bool SwitchDevice::SupportAculab(SwitchDevice *requester)
{
    return false;
}

/** It must be overridden by the descendant if it support
    H.100/H.110 switching fabric
    Default implementation returns false.
 */
bool SwitchDevice::SupportCTbus(SwitchDevice *requester)
{
    return false;
}

/** It must be overridden by the descendant if it support
    SCSA switching fabric
    Default implementation returns false.
 */
bool SwitchDevice::SupportSCbus(SwitchDevice *requester)
{
    return false;
}

/** For devices which support dynamic allocation to the given
    fabric, this is the place where it should free the previously
    allocated address in the given fabric. Otherwise, it does not 
    do anything.
    This function is called when the framework detects that no device
    is currently listening to this device.
    Default implementation does nothing.
 */
void SwitchDevice::FreeAddress(SwitchFabric fabric)
{
}

/** The framework will increment the usage counter for a specific
    switching fabric each time the fabric is selected for switching.
    This function returns the value of the counter after increment.
    Devices which use static allocation do not need to override this
    method
 */
unsigned SwitchDevice::IncTxRefCount(SwitchFabric fabric)
{
    switch (fabric) {
    case SF_Aculab:
	return ++_refcntAculab;
    case SF_CTbus:
	return ++_refcntCTbus;
    case SF_SCbus:
	return ++_refcntSCbus;
    case SF_Kallastra:
	return ++_refcntKallastra;
    case SF_Host:
	return ++_refcntHostmedia;
    }
    assert(false);
    return 1;
}

/** The framework will decrement the usage counter for a specific
    switching fabric each time a listener has stop listening to it.
    This function returns the value of the counter after decrement.
    Devices which use static allocation do not need to override this
    method
 */
unsigned SwitchDevice::DecTxRefCount(SwitchFabric fabric)
{
    switch (fabric) {
    case SF_Aculab:
	assert(_refcntAculab);
	return --_refcntAculab;
    case SF_CTbus:
	assert(_refcntCTbus);
	return --_refcntCTbus;
    case SF_SCbus:
	assert(_refcntSCbus);
	return --_refcntSCbus;
    case SF_Kallastra:
	return --_refcntKallastra;
    case SF_Host:
	assert(_refcntHostmedia);
	return --_refcntHostmedia;
    }
    assert(false);
    return 0;
}

/** This function is called to instruct all devices currently listening 
    from this device to stop listening. It is automatically called
    by the destructor, however, it may also be called explicitely
    if the caller does not want to delete this instance.
 */
void SwitchDevice::DisconnectListeners()
{
    SwitchDevice *device = IterGetFirstListener();
    while (device) {
	if (device->IsValid()) {
	    device->Unlisten();
	} else {
	    CLog::Fatal(THISMODULE, "Invalid SwitchDevice 0x%p in DisconnectListeners by device %s", device, GetName());
	}
	
	device = IterGetNextListener();
    }
}

/** It instructs the device to start transmitting to a certain location 
    using the specified switch fabric.
    This method needs to be implemented by devices which support dynamic
    allocation to the given fabric. Devices which use static allocation 
    do not need to override this method
    Default implementation does nothing.
 */
void SwitchDevice::DeviceStartTransmit(SwitchFabric fabric,SwitchAddress *addr)
{
}

/** It instructs the device to start listening from a certain location 
    using the specified switch fabric.
    This function needs to be overridden by all descendant classes.
    Default implementation does nothing.
 */
void SwitchDevice::DeviceStartReceive(SwitchFabric fabric,SwitchAddress *addr)
{
}

/** It instructs the device to stop transmitting to a certain location 
    using the specified switch fabric. It is called when the reference
    count of the specified switch fabric reach 0.
    This method needs to be implemented by devices which support dynamic
    allocation to the given fabric. Devices which use static allocation 
    do not need to override this method
    Default implementation does nothing.
 */
void SwitchDevice::DeviceStopTransmit(SwitchFabric fabric,SwitchAddress *addr)
{
}

/** It instructs the device to stop listening from a certain location 
    using the specified switch fabric.
    This function needs to be overridden by all descendant classes.
    Default implementation does nothing.
 */
void SwitchDevice::DeviceStopReceive(SwitchFabric fabric,SwitchAddress *addr) 
{
}

/** Instruct this device to listen from another device
 */
void SwitchDevice::Listen(SwitchDevice *source)
{
    // blp: check switch devices
    if (!IsValid()) {
	CLog::Fatal(THISMODULE, "Device 0x%p unable to listen because it's not valid", this);
	return;
    }
    if (!source->IsValid()) {
	CLog::Fatal(THISMODULE, "%s: Unable to listen to device 0x%p because it's not valid", GetName(), source);
	return;
    }

    // if we already listen to the same source then ignore the request
    /*
    if (_transmitter && (source == _transmitter->GetTransmitter()))
	return;
    */
    if (_transmitter == source)
	return;

    //** TBD: Establish connection only if source->GetTransmitter() != NULL to avoid noise
    //	 due to unlisten

    // disconnect any previous connection
    Unlisten();

    // add this object to the list of listener
    source->AddListener(this);
    _transmitter = source;

    // and start listen
    if (!StartListen(source->GetTransmitter())) {
	source->RemoveListener(this);
	_transmitter = NULL;
    }
}

/** Instruct this device to stop listening from another device if any
 */
void SwitchDevice::Unlisten() 
{
    if (_transmitter) {
	_transmitter->RemoveListener(this);
	SwitchDevice *device = _transmitter->GetTransmitter();
	if (device) StopListen(device);

	_transmitter = NULL;
    }
}

/** It instructs the device to start transmitting to a certain location
    using the specified switch fabric.
 */
bool SwitchDevice::StartListen(SwitchDevice *transmitter)
{
    if (transmitter) {
	// find a compatible fabric to switch to
	SwitchFabric fabric = transmitter->GetCompatibleFabric(this);
	if (fabric != SF_Invalid) {

	    // find a free address in the switching fabric
	    SwitchAddress *addr;
	    if (transmitter->IncTxRefCount(fabric) == 1) {
		addr = transmitter->AllocateAddress(fabric);

		// switch this transmitter to the specified address
		if (addr) transmitter->DeviceStartTransmit(fabric,addr);
	    } else {
		addr = transmitter->GetAddress(fabric);
	    }

	    // switch this transmitter from the specified address
	    if (addr) {
		_listenFabric = fabric;
		_listenAddr = addr;
		DeviceStartReceive(fabric,addr);

		// increment reference count of transmitter
		transmitter->IncReference();

		CLog::Detail(THISMODULE,"%s listens from %s at %s",GetName(),transmitter->GetName(),addr->ToString());
	    } else {
		CLog::Warning(THISMODULE,"%s unable to listen from %s at address (null)",GetName(),transmitter->GetName());
	    }
	} else {
	    CLog::Warning(THISMODULE,"StartListen: %s unable to find compatible fabric with %s",GetName(),transmitter->GetName());
	    return false;
	}
    }
    return true;
}

/** It instructs the device to stop listening from a certain location 
    using the specified switch fabric.
 */
void SwitchDevice::StopListen(SwitchDevice *transmitter) 
{
    if (transmitter) {
	if (_listenFabric != SF_Invalid && _listenAddr != NULL) {
	    SwitchAddress *addr = transmitter->GetAddress(_listenFabric);
	    if (addr) {
		CLog::Detail(THISMODULE,"%s stops listening from %s at %s",GetName(),transmitter->GetName(),addr->ToString());
		DeviceStopReceive(_listenFabric, _listenAddr);
	    }

	    if (transmitter->DecTxRefCount(_listenFabric) == 0) {
		transmitter->DeviceStopTransmit(_listenFabric, _listenAddr);
		transmitter->FreeAddress(_listenFabric);
	    }

	    transmitter->DecReference();
	} else {
	    CLog::Error(THISMODULE,"StopListen: %s unable to find compatible fabric with %s",GetName(),transmitter->GetName());
	}
    }

    _listenFabric = SF_Invalid;
    _listenAddr = NULL;
}


/** Reference count related, increment or decrement usage counter
    for this instance. When it reaches 0, this instance will be deleted
 */
void SwitchDevice::IncReference()
{
    ++_refCount;
#ifdef _DEBUG
    //**CLog::Detail(THISMODULE,"%s refcount incremented to %u",GetName(),_refCount);
#endif
}

/** Reference count related, increment or decrement usage counter
    for this instance. When it reaches 0, this instance will be deleted
 */
void SwitchDevice::DecReference()
{
    assert(_refCount);
#ifdef _DEBUG
    //**CLog::Detail(THISMODULE,"%s refcount decremented to %u",GetName(),_refCount-1);
#endif
    if (--_refCount == 0) delete this;
}

/** Add a listener device to the listener list
 */
void SwitchDevice::AddListener(SwitchDevice *device)
{
    _listener.push_back(device);
}

/** Remove a listener device from the listener list
 */
void SwitchDevice::RemoveListener(SwitchDevice *device)
{
    _listener.remove(device);
}

/** Iterator for listener list
 */
SwitchDevice *SwitchDevice::IterGetFirstListener()
{
    _iter = _listener.begin();
    if (_iter != _listener.end()) return *_iter++;
    else return NULL;
}

/** Iterator for listener list
 */
SwitchDevice *SwitchDevice::IterGetNextListener()
{
    if (_iter != _listener.end()) return *_iter++;
    else return NULL;
}


/** Returns the physical transmitter switch device.
 */
SwitchDevice *SwitchDevice::GetTransmitter()
{
    return this;
}


/*----------------------------------------------------------------* 
 * Implementation of SwitchManager
 *----------------------------------------------------------------*/

/** SwitchManager's constructor
 */
SwitchManager::SwitchManager() :
    ServiceProvider(THISMODULE,false,false)
{
    _api = NULL;
}

/** SwitchManager's destructor
 */
SwitchManager::~SwitchManager()
{
}

/** Create a singleton instance of AculabSignalling
 */
SwitchManager *SwitchManager::Create()
{
    assert(theSwitchManager == NULL);

    theSwitchManager = new SwitchManager;
    return theSwitchManager;
}

/** Get the singleton instance.
 */
SwitchManager *SwitchManager::Instance()
{
    return theSwitchManager;
}

/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
 */
bool SwitchManager::Load()
{
    _api = new SwitchAPI;
    return true;
}

/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
 */
bool SwitchManager::Unload()
{
    delete _api;
    _api = NULL;
    return true;
}


#ifdef SWITCH_MANAGE_DEVICES
/** Add a new device to the device list.
 */
void SwitchManager::AddDevice(SwitchDevice *dev)
{
    _device_list.push_back(dev);
}


/** Remove device from the device list.
 */
void SwitchManager::RemoveDevice(SwitchDevice *dev)
{
    _device_list.remove(dev);
}

/** Show device connection.
 */
void SwitchManager::ShowSwitching(Term_Session *session)
{
    if (!ACQUIRE_SYSTEM_LOCK())
	return;

    List<SwitchDevice*>::iterator it, end = _device_list.end();
    String output;
    int linenum = 0;
    char temp_line[120];
    for (it = _device_list.begin(); it != end; ++it) {
	SwitchDevice *dev1 = *it;
	SwitchAddress* dev2_addr = dev1->GetListenAddress();
	if (dev2_addr) {
	    linenum++;
	    sprintf(temp_line, " %4d: %-20s <-- %s@%s\n", linenum, dev1->GetName(), dev2_addr->GetOwner(), dev2_addr->ToString());
	    output += temp_line;
	}
    }

    RELEASE_SYSTEM_LOCK();
    session->Send(output.c_str(), output.size());
}

#endif

/*----------------------------------------------------------------* 
 * Implementation of SupportSwitching
 *----------------------------------------------------------------*/
/** SupportSwitching's constructor
 */
SupportSwitching::SupportSwitching(SwitchDevice *device) 
{
    _device = device;
    _device->IncReference();
}

/** SupportSwitching's destructor
 */
SupportSwitching::~SupportSwitching() 
{
    if (_device->IsValid()) {
	_device->DisconnectListeners();
	_device->Unlisten();
	_device->DecReference();
    } else {
	CLog::Fatal(THISMODULE, "SwitchDevice 0x%p already deleted before SupportSwitching 0x%p",
		    _device, this);
    }
}

/** Return switching device attached to this instance
 */
SwitchDevice *SupportSwitching::GetSwitchDevice() 
{
    return _device;
}
