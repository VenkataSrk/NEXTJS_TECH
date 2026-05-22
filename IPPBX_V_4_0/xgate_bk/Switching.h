/*----------------------------------------------------------------* 
 * $Archive: /xGate/Switching.h $
 * $Date: 25/06/04 11:28 $
 * $Revision: 16 $
 *
 * $History: Switching.h $
 * 
 * *****************  Version 16  *****************
 * User: Sonny        Date: 25/06/04   Time: 11:28
 * Updated in $/xGate
 * 
 * *****************  Version 15  *****************
 * User: Junanto      Date: 22/04/04   Time: 18:43
 * Updated in $/xGate
 * 
 * *****************  Version 14  *****************
 * User: Sonny        Date: 9/04/02    Time: 12:53p
 * Updated in $/xGate
 * Added SetName
 * 
 * *****************  Version 13  *****************
 * User: Bennylp      Date: 8/29/02    Time: 8:17p
 * Updated in $/xGate
 * Added command "show switching"
 * 
 * *****************  Version 12  *****************
 * User: Junanto      Date: 3/07/01    Time: 13:04
 * Updated in $/xGate
 * Added error recovery in StopListen when a compatible
 * switching fabric can't be found
 * 
 * *****************  Version 11  *****************
 * User: Bennylp      Date: 6/30/01    Time: 8:13p
 * Updated in $/xGate
 * Changed StartListen() to return bool, to capture error.
 * 
 * *****************  Version 10  *****************
 * User: Bennylp      Date: 6/29/01    Time: 12:28p
 * Updated in $/xGate
 * Move Set/GetOwner to SwitchAddress from CTbusAddress.
 * 
 * *****************  Version 9  *****************
 * User: Junanto      Date: 6/06/01    Time: 20:37
 * Updated in $/xGate
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 30/04/01   Time: 10:57
 * Updated in $/xGate
 * Pipeline signalling and switching done
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 29/04/01   Time: 18:02
 * Updated in $/xGate
 * SwitchAPI class declaration moved to .cpp
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 29/04/01   Time: 17:37
 * Updated in $/xGate
 * interfaces changed to prepare for
 * accomodating pipeline switching
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 27/04/01   Time: 16:51
 * Updated in $/xGate
 * added DisconnectListeners() to force 
 * switch listeners to unlisten from a
 * dying device
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 24/04/01   Time: 20:34
 * Updated in $/xGate
 * Added reference counting in SwitchDevice
 * and switching framework successfully tested
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

#if !defined(CT2_SWITCHING_H__INCLUDED_)
#define CT2_SWITCHING_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "api.h"


/** Define SWITCH_MANAGE_DEVICE to enable switch device tracking in SwitchManager.
    When switch device tracking is enabled, user execute command "show switching"
 */
#define SWITCH_MANAGE_DEVICES


/** Forward declaration
 */
class SwitchAPI;
class Term_Session;

/** SwitchFabric: the supported types of switching fabric
 */
enum SwitchFabric
{
    SF_Invalid = -1,

    SF_Aculab = 0,
    SF_CTbus,
    SF_SCbus,
    SF_Kallastra,
    SF_Host,
    SF_Remote
};


/** SwitchAddress: It is the abstract base class for switching address
 */
class SwitchAddress
{
public:
    /** SwitchAddress's constructor
     */
    SwitchAddress(SwitchFabric fabric) {
	_fabric = fabric;
	_owner[0] = _owner[sizeof(_owner)-1] = 0;
    }

    /** SwitchAddress's destructor
     */
    virtual ~SwitchAddress() {
	// do nothing
    }

    /** Set the owner's name of this address
     */
    void SetOwner(const char* owner) {
	size_t len = strlen(owner);
	if (len < sizeof(_owner)) strcpy(_owner, owner);
	else memcpy(_owner, owner, sizeof(_owner)-1);
    }

    /** Get the owner's name of this address
     */
    const char* GetOwner() {
	return _owner;
    }

    /** Return the string representation of this address. It must be
	implemented by all descendants
     */
    virtual const char* ToString() = 0;

    /** SwitchAddress's constructor
     */
    SwitchFabric GetSwitchFabric() {
	return _fabric;
    }

private:
    SwitchFabric _fabric;
    char _owner[128];
};


/** SwitchDevice: It is the base class for all switching devices
 */
class SwitchDevice
{
    friend class SupportSwitching;

public:
    /** Constructor/destructor
     */
    SwitchDevice(const char* name);

    /** It returns the name of this device
     */
    const char* GetName();

    /** SetName: set the name of this device
     */
    void SetName(const char *name);

    /** The framework will query the device to find out which 
	switching fabric is supported by this device to exchange TDM
	data with another device.
     */
    virtual SwitchFabric GetCompatibleFabric(SwitchDevice* dev) = 0;

    /** It must be overridden by the descendant if it support
	Aculab switching fabric
     */
    virtual bool SupportAculab(SwitchDevice* requester);

    /** It must be overridden by the descendant if it support
	H.100/H.110 switching fabric
     */
    virtual bool SupportCTbus(SwitchDevice* requester);

    /** It must be overridden by the descendant if it support
	SCSA switching fabric
     */
    virtual bool SupportSCbus(SwitchDevice* requester);

    /** It must be overridden by the descendant if it support
	Aculab switching fabric
     */
    virtual bool SupportKallastra(SwitchDevice* requester) { return false; }

    /** For devices which support dynamic allocation to the given
	fabric, this is the place where it should allocate an address 
	in the given fabric and return a pointer to this address. 
	Otherwise, it simply returns a pointer to it without having to 
	create one.
	This function is called when the framework decides that it is
	the first time the specific device transmits to a particular
	fabric.
     */
    virtual SwitchAddress* AllocateAddress(SwitchFabric fabric) = 0;

    /** For devices which support dynamic allocation to the given
	fabric, this is the place where it should free the previously
	allocated address in the given fabric. Otherwise, it does not 
	do anything.
	This function is called when the framework detects that no device
	is currently listening to this device.
     */
    virtual void FreeAddress(SwitchFabric fabric);

    /** It returns the address used for switching in the specified fabric
     */
    virtual SwitchAddress* GetAddress(SwitchFabric fabric) = 0;

    /** This function is called to instruct all devices currently listening 
	from this device to stop listening. It is automatically called
	by the destructor, however, it may also be called explicitely
	if the caller does not want to delete this instance.
     */
    void DisconnectListeners();

    /** It instructs the device to start transmitting to a certain location 
	using the specified switch fabric.
	This method needs to be implemented by devices which support dynamic
	allocation to the given fabric. Devices which use static allocation 
	do not need to override this method
     */
    virtual void DeviceStartTransmit(SwitchFabric fabric,SwitchAddress* addr);

    /** It instructs the device to start listening from a certain location 
	using the specified switch fabric.
	This function needs to be overridden by all descendant classes.
     */
    virtual void DeviceStartReceive(SwitchFabric fabric,SwitchAddress* addr);

    /** It instructs the device to stop transmitting to a certain location 
	using the specified switch fabric. It is called when the reference
	count of the specified switch fabric reach 0.
	This method needs to be implemented by devices which support dynamic
	allocation to the given fabric. Devices which use static allocation 
	do not need to override this method
     */
    virtual void DeviceStopTransmit(SwitchFabric fabric,SwitchAddress* addr);

    /** It instructs the device to stop listening from a certain location 
	using the specified switch fabric.
	This function needs to be overridden by all descendant classes.
     */
    virtual void DeviceStopReceive(SwitchFabric fabric,SwitchAddress* addr);

    /** Instruct this device to listen from another device
     */
    virtual void Listen(SwitchDevice* source);

    /** Instruct this device to stop listening from another device if any
     */
    virtual void Unlisten();

    /** Add a listener device to the listener list
     */
    virtual void AddListener(SwitchDevice* device);

    /** Remove a listener device from the listener list
     */
    virtual void RemoveListener(SwitchDevice* device);

    /** Returns the physical transmitter switch device.
     */
    virtual SwitchDevice* GetTransmitter();

    /** It instructs the device to start transmitting to a certain location 
	using the specified switch fabric.
     */
    virtual bool StartListen(SwitchDevice* device);

    /** It instructs the device to stop listening from a certain location 
	using the specified switch fabric.
     */
    virtual void StopListen(SwitchDevice* device);

    /** Iterator for listener list
     */
    virtual SwitchDevice* IterGetFirstListener();
    virtual SwitchDevice* IterGetNextListener();

    SwitchAddress* GetListenAddress() {
	return _listenAddr;
    }

    SwitchFabric GetListenFabric() {
	return _listenFabric;
    }

protected:
    /** Protected destructor
     */
    virtual ~SwitchDevice();

    /** Reference count related, increment or decrement usage counter
	for this instance. When it reaches 0, this instance will be deleted
     */
    void IncReference();
    void DecReference();

    // this variables kept track of the current switch source
    SwitchDevice* _transmitter;

private:
    typedef List<SwitchDevice *> ListListener;

    // each SwitchDevice instance bears this signature
    enum MagicWord {
	DEADCODE = 0xDEADC0DE,
    };

    /** The framework will increment the usage counter for a specific
	switching fabric each time the fabric is selected for switching.
	This function returns the value of the counter after increment.
	Devices which use static allocation do not need to override this
	method
     */
    unsigned IncTxRefCount(SwitchFabric fabric);

    /** The framework will decrement the usage counter for a specific
	switching fabric each time a listener has stop listening to it.
	This function returns the value of the counter after decrement.
	Devices which use static allocation do not need to override this
	method
     */
    unsigned DecTxRefCount(SwitchFabric fabric);

    // reference counters for switching fabric usage
    unsigned _refcntAculab;
    unsigned _refcntCTbus;
    unsigned _refcntSCbus;
    unsigned _refcntKallastra;
    unsigned _refcntHostmedia;

    String _name;
    ListListener _listener;
    ListListener::iterator _iter;
    MagicWord _signature;
    SwitchFabric _listenFabric;
    SwitchAddress* _listenAddr;

    // Reference count, this instance will be deleted automatically
    // when reference count reaches 0
    int _refCount;

public:
    bool IsValid() const;
};


/** SupportSwitching: each device supporting switching TDM stream
    must derive from this base class
 */
class SupportSwitching
{
public:
    /** SupportSwitching's constructor
     */
    SupportSwitching(SwitchDevice* device);

    /** SupportSwitching's destructor
     */
    virtual ~SupportSwitching();

    /** Return switching device attached to this instance
     */
    virtual SwitchDevice* GetSwitchDevice();

private:
    SwitchDevice* _device;
};


/** SwitchManager : A singleton which the only purpose is to instantiate
    SwitchAPI
 */
class SwitchManager : public ServiceProvider
{
public:
    /** SwitchManager's destructor
     */
    virtual ~SwitchManager();

    /** Create a singleton instance of SwitchManager
     */
    static SwitchManager* Create();

    /** Get the singleton instance.
     */
    static SwitchManager *Instance();

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

#ifdef SWITCH_MANAGE_DEVICES
    /** Add a new device to the device list.
     */
    void AddDevice(SwitchDevice *dev);

    /** Remove device from the device list.
     */
    void RemoveDevice(SwitchDevice *dev);

    /** Show device connection.
     */
    void ShowSwitching(Term_Session *session);
#endif

private:
    /** Private constructor
     */
    SwitchManager();

    static SwitchManager* theSwitchManager;
    SwitchAPI* _api;

#ifdef SWITCH_MANAGE_DEVICES
    List<SwitchDevice*> _device_list;
#endif
};


#endif // !defined(CT2_SWITCHING_H__INCLUDED_)
