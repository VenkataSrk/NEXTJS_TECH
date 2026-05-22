/*----------------------------------------------------------------* 
 * $Archive: /xGate/AculabSwitch.h $
 * $Date: 30/04/02 17:39 $
 * $Revision: 7 $
 *
 * $History: AculabSwitch.h $
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 30/04/02   Time: 17:39
 * Updated in $/xGate
 * Aculab switching changed:
 * - added option "looped back when idle" for Prosody
 * - at destruction, put back original switching mode
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 31/01/02   Time: 18:44
 * Updated in $/xGate
 * Original switching mode is memorized and put back
 * during idle time
 * 
 * *****************  Version 5  *****************
 * User: Bennylp      Date: 7/02/01    Time: 7:01p
 * Updated in $/xGate
 * 
 * *****************  Version 4  *****************
 * User: Bennylp      Date: 6/13/01    Time: 8:31p
 * Updated in $/xGate
 * Aculab switch saves the transmit address.
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

#if !defined(CT2_ACULABSWITCH_H__INCLUDED_)
#define CT2_ACULABSWITCH_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Switching.h"
//#include "prosody.h"
#include "CTbusMgr.h"
#include "SCbusMgr.h"

#include "sw_lib.h"
#include "smhlib.h"
#include <stack>
#include <map>
#include <pthread.h>
#include <errno.h>
#include <algorithm>          //05092016  Event
#include <deque>              //05092016  Event

#include <sqltypes.h>	//linux porting

#define ACU_REMEMBER_ADDRESS
#define COND_WAIT

#define WAIT_TIMEOUT ETIMEDOUT  //05092016  Event

#ifdef COND_WAIT
    struct am_event_t_
    {
        pthread_cond_t CVariable;
        pthread_mutex_t Mutex;
        bool AutoReset;
        bool State;
    #ifdef WFMO
        std::deque<m_wfmo_info_t_> RegisteredWaits;
    #endif
//        SM_CHANNEL_SET_EVENT_PARMS_t _parms;
    };

    typedef void* HANDLE_T;
    typedef am_event_t_* am_event_t;
#endif

/** AculabAddress: Representation of the local Aculab switching address
 */
class AculabAddress : public SwitchAddress
{
public:
    /** AculabAddress's constructor/destructor
     */
    AculabAddress(const char* serial_num, int swdrv, int stream, int timeslot, tSMModuleId module_id = 0);
    virtual ~AculabAddress();

    /** Return the string representation of this address.
     */
    virtual const char *ToString();

    /** Return the switch driver of this address
     */
    int GetSwitch() {
	return _swdrv;
    }

    /** Return the stream part of this address
     */
    int GetStream() {
	return _stream;
    }

    /** Return the timeslot part of this address
     */
    int GetTimeslot() {
	return _timeslot;
    }

    tSMModuleId	GetModule()
    {
	return _module_id;
    }

private:
    int		_swdrv,_stream,_timeslot;
    String	_name;
    tSMModuleId	_module_id;
};

class HBusPool;
class AculabBusManager
{
    typedef std::map<tSMModuleId, HBusPool*> map_of_pools;
    map_of_pools modules;
    static AculabBusManager* instance;

public:
    AculabBusManager();
    virtual ~AculabBusManager();
    AculabAddress* AllocateAddress( const char* serial_num, const char* owner, int swdrv, int stream, int timeslot, tSMModuleId module_id );
    void FreeAddress( AculabAddress* address );
    int	GetNumAddressRemaining( tSMModuleId module_id );
    void RegisterModule( const char* serial_num, int swdrv, tSMModuleId module_id, int minStream, int numTimeslotsPerStream );

    static AculabBusManager* Instance();
};

/** AculabSwitch: Representation of the Aculab switching device
 */
class AculabSwitch : public SwitchDevice
{
public:
    /** AculabSwitch's constructor/destructor
     */
    AculabSwitch(const char *name, const char* serial_num, int swdrv, int stream, int timeslot, bool disable_when_idle, tSMModuleId module_id=0);
    virtual ~AculabSwitch();

    /** The framework will query the device to find out which 
	switching fabric is supported by this device to exchange TDM
	data with another device.
     */
    SwitchFabric GetCompatibleFabric(SwitchDevice *dev);

    /** This function is called when the framework decides that it is
	the first time the specific device transmits to a particular
	fabric.
     */
    virtual SwitchAddress *AllocateAddress(SwitchFabric fabric);

    /** This function is called when the framework detects that no device
	is currently listening to this device.
     */
    virtual void FreeAddress(SwitchFabric fabric);

    /** It returns the address used for switching in the specified fabric
     */
    virtual SwitchAddress *GetAddress(SwitchFabric fabric);

    /** This device support Aculab switching fabric if another device
	(which must be Aculab) is located on the same board
     */
    virtual bool SupportAculab(SwitchDevice *requester);

    /** This device supports CTbus (H.100/H.110)
     */
    virtual bool SupportCTbus(SwitchDevice *requester);

    /** This device supports SCbus
     */
    virtual bool SupportSCbus(SwitchDevice *requester);

    /** It instructs the device to start transmitting to a certain location 
	using the specified switch fabric.
     */
    virtual void DeviceStartTransmit(SwitchFabric fabric,SwitchAddress *addr);

    /** It instructs the device to start listening from a certain location 
	using the specified switch fabric.
     */
    virtual void DeviceStartReceive(SwitchFabric fabric,SwitchAddress *addr);

    /** It instructs the device to stop transmitting to a certain location 
	using the specified switch fabric. It is called when the reference
	count of the specified switch fabric reach 0.
     */
    virtual void DeviceStopReceive(SwitchFabric fabric,SwitchAddress *addr);

    /** It instructs the device to stop listening from a certain location 
	using the specified switch fabric.
     */
    virtual void DeviceStopTransmit(SwitchFabric fabric,SwitchAddress *addr);

private:
    /** It returns the switch driver index associated with this device
     */
    int GetSwitchDriver();

    /** It returns the serial number of the card where this switch device resides
     */
    const char* GetCardSerialNum() {
	return _serial_num;
    }

    /** On Prosody/X DSP farm #1 can't talk directly to DSP farm #2. A DSP farm 
	is a group of 4 DSP. This functions check whether a stream belongs to
	DSP farm #1 or #2.
     */
    bool belongs_to_prosodyx_dsp_farm_1(int stream) { return (stream >= 64) && (stream <= 71); }
    bool belongs_to_prosodyx_dsp_farm_2(int stream) { return (stream >= 80) && (stream <= 87); }

    /** Execute sw_set_output call and display activity and error to the log 
     */
    bool DeviceSetOutput(int swdrv,OUTPUT_PARMS* parms);

    AculabAddress	*_addrLocal;
    CTbusAddress	*_addrCTbus;
    SCbusAddress	*_addrSCbus;
    OUTPUT_PARMS	_initialOutput;
    OUTPUT_PARMS	_lastOutput;
    char		_serial_num[64];

#ifdef ACU_REMEMBER_ADDRESS
    // used to store the address where the device is currently transmitting
    CTbusAddress	_txCTbusAddr;
    SCbusAddress	_txSCbusAddr;
    bool		_txCTbus, _txSCbus;
#endif

    int			_swdrv;
    bool		_canDoCTbus,_canDoSCbus;
};


#include "Critical_Section.h"
class AculabSwitchManager
{
public:
    static void start();
    static void stop();

    static void async_sw_set_output(int swdrv, OUTPUT_PARMS* parms);

#ifdef COND_WAIT
    /*** Event handling functions ***/
    am_event_t CreateEvent(bool manualReset = false, bool initialState = false);
    int DestroyEvent(am_event_t event);
    int DestroyEvents();
    int WaitForEvent(am_event_t event, uint64_t milliseconds = -1);
    int UnlockedWaitForEvent(am_event_t event, uint64_t milliseconds);
    int SetEvent(am_event_t event);
    int ResetEvent(am_event_t event);
#endif
private:
    struct Job
    {
	int swdrv;
	OUTPUT_PARMS parms;
    };
    typedef List<Job*> job_list;

    job_list _jobs;
    //HANDLE _event;
    //HANDLE _thread;
    HANDLE_T _event;
    HANDLE_T _thread;
    bool _quit;
    Critical_Section _cs;
    pthread_t aculab_thread;


    AculabSwitchManager();
    ~AculabSwitchManager();
    //static DWORD WINAPI async_thread(void *arg);
    static DWORD async_thread(void *arg);
};


#endif // !defined(CT2_ACULABSWITCH_H__INCLUDED_)
