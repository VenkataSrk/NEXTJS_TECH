/*----------------------------------------------------------------* 
 * $Archive: /xGate/Aculab.h $
 * $Date: 23/04/03 12:24 $
 * $Revision: 39 $
 *
 * $History: Aculab.h $
 * 
 * *****************  Version 39  *****************
 * User: Junanto      Date: 23/04/03   Time: 12:24
 * Updated in $/xGate
 * added FCI and NCI settings in Aculab based SS7
 * 
 * *****************  Version 38  *****************
 * User: Junanto      Date: 15/05/02   Time: 10:27
 * Updated in $/xGate
 * Implemented OnClearCounter
 * 
 * *****************  Version 37  *****************
 * User: Junanto      Date: 16/03/02   Time: 10:50
 * Updated in $/xGate
 * version 38.15
 * 
 * *****************  Version 36  *****************
 * User: Junanto      Date: 31/10/01   Time: 16:42
 * Updated in $/xGate
 * Added layer 2 monitoring - to prevent calls from being made 
 * when signalling is down
 * 
 * *****************  Version 35  *****************
 * User: Junanto      Date: 22/10/01   Time: 15:08
 * Updated in $/xGate
 * Event poller is moved to a separate thread to
 * increase main thread performance
 * 
 * *****************  Version 34  *****************
 * User: Junanto      Date: 19/10/01   Time: 16:45
 * Updated in $/xGate
 * One service provider (AculabItf) represents
 * all layer 1 checker
 * 
 * *****************  Version 33  *****************
 * User: Junanto      Date: 9/08/01    Time: 19:57
 * Updated in $/xGate
 * - Call distribution method is used to select timeslot
 * - Corrected behaviour when signalling is changed
 * 
 * *****************  Version 32  *****************
 * User: Junanto      Date: 25/07/01   Time: 17:49
 * Updated in $/xGate
 * Delayed signalling driver settings until all configuration
 * file is read (during CommitConfiguration)
 * 
 * *****************  Version 31  *****************
 * User: Junanto      Date: 18/07/01   Time: 18:59
 * Updated in $/xGate
 * Added support for ISUP circuit/group blocking/unblocking
 * 
 * *****************  Version 30  *****************
 * User: Junanto      Date: 27/06/01   Time: 20:53
 * Updated in $/xGate
 * Added support for timeslot blocking
 * ISUP maintenance command can be issued from xGate
 * 
 * *****************  Version 29  *****************
 * User: Junanto      Date: 26/06/01   Time: 17:15
 * Updated in $/xGate
 * Fixed a small bug
 * 
 * *****************  Version 28  *****************
 * User: Junanto      Date: 25/06/01   Time: 16:46
 * Updated in $/xGate
 * AttachInterface changed to bool
 * First attempt to integrate with ISUP signalling
 * group command can only be used without resource locator
 * 
 * *****************  Version 27  *****************
 * User: Bennylp      Date: 6/22/01    Time: 11:02p
 * Updated in $/xGate
 * Changed some members to const, and others to non virtual, etc.
 * 
 * *****************  Version 26  *****************
 * User: Junanto      Date: 19/06/01   Time: 16:07
 * Updated in $/xGate
 * Tested "reload" firmware functions
 * 
 * *****************  Version 25  *****************
 * User: Junanto      Date: 7/06/01    Time: 22:33
 * Updated in $/xGate
 * 
 * *****************  Version 24  *****************
 * User: Junanto      Date: 6/06/01    Time: 15:54
 * Updated in $/xGate
 * OnShutdown has a new parameter which indicates
 * that a requested shutdown has been cancelled
 * 
 * *****************  Version 23  *****************
 * User: Junanto      Date: 6/06/01    Time: 10:38
 * Updated in $/xGate
 * - Added circuit reference command
 * - SETUP_ACK is sent on receipt of the first EV_DETAILS
 * - CompandingLaw default to A-law
 * 
 * *****************  Version 22  *****************
 * User: Junanto      Date: 3/06/01    Time: 21:05
 * Updated in $/xGate
 * Added OnShutdown handler -> transmit RRA
 * 
 * *****************  Version 21  *****************
 * User: Junanto      Date: 2/06/01    Time: 16:56
 * Updated in $/xGate
 * Fixed layer 1 problem reporting
 * 
 * *****************  Version 20  *****************
 * User: Junanto      Date: 2/06/01    Time: 14:36
 * Updated in $/xGate
 * Aculab reset statistics counter on start up
 * 
 * *****************  Version 19  *****************
 * User: Junanto      Date: 19/05/01   Time: 21:34
 * Updated in $/xGate
 * Added support for no fwfile/fwargs/dspfile
 * Fixed bug in reload
 * 
 * *****************  Version 18  *****************
 * User: Junanto      Date: 17/05/01   Time: 18:20
 * Updated in $/xGate
 * Added regular layer 1 checking
 * 
 * *****************  Version 17  *****************
 * User: Junanto      Date: 8/05/01    Time: 13:11
 * Updated in $/xGate
 * Delayed transition to running state
 * 
 * *****************  Version 16  *****************
 * User: Bennylp      Date: 5/02/01    Time: 12:02p
 * Updated in $/xGate
 * Added show interface framework.
 * 
 * *****************  Version 15  *****************
 * User: Junanto      Date: 30/04/01   Time: 19:16
 * Updated in $/xGate
 * Disable event handling on absence of 
 * attached interface
 * 
 * *****************  Version 14  *****************
 * User: Junanto      Date: 29/04/01   Time: 17:34
 * Updated in $/xGate
 * AculabCall declaration moved to .cpp
 * 
 * *****************  Version 13  *****************
 * User: Junanto      Date: 24/04/01   Time: 18:26
 * Updated in $/xGate
 * first link between Prosody voice / Aculab media with
 * switching framework
 * 
 * *****************  Version 12  *****************
 * User: Junanto      Date: 24/04/01   Time: 14:09
 * Updated in $/xGate
 * 
 * *****************  Version 11  *****************
 * User: Bennylp      Date: 4/23/01    Time: 10:42p
 * Updated in $/xGate
 * Fixed std namespace conflicts.
 * 
 * *****************  Version 10  *****************
 * User: Junanto      Date: 20/04/01   Time: 18:24
 * Updated in $/xGate
 * 
 * *****************  Version 9  *****************
 * User: Junanto      Date: 19/04/01   Time: 17:55
 * Updated in $/ct15
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 18/04/01   Time: 11:49
 * Updated in $/ct15
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 15/04/01   Time: 20:48
 * Updated in $/ct15
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 14/04/01   Time: 19:59
 * Updated in $/ct15
 * Add more APIs
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 13/04/01   Time: 22:37
 * Updated in $/ct15
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 13/04/01   Time: 21:03
 * Updated in $/ct15
 * 1st version of integration with call manager
 * 
 * *****************  Version 3  *****************
 * User: Junanto      Date: 11/04/01   Time: 22:01
 * Updated in $/ct15
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 11/04/01   Time: 11:37
 * Updated in $/ct15
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 10/04/01   Time: 18:21
 * Created in $/ct15
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 8/04/01    Time: 19:43
 * Updated in $/ct2
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 4/04/01    Time: 16:02
 * Created in $/ct2
 * Created
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 *  
 *----------------------------------------------------------------*/

#if !defined(CT2_ACULAB_H__INCLUDED_)
#define CT2_ACULAB_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "callmanager.h"
#include "MediaManager.h"
#include "SignallingDriver.h"
//#include "AculabSwitch.h"
#include "aculabswitch.h"
#include "DigitalNetworkInterface.h"

#include <sqltypes.h>


/** Forward declaration
 */
struct detail_xparms;
union uniquex;
class Term_Session;
class AculabInterface;

#define ACU_MAX_TIMESLOT_IN_INTERFACE		32

enum AculabMediaState {
    AMS_Closed,
    AMS_Idle,
    AMS_Busy,
};


/** AculabMedia:
 */
class AculabMedia : public DigitalNetworkMedia
{
public:
    AculabMedia(const char* serial_num, int port, int timeslot, AculabInterface* itf);
    virtual ~AculabMedia();

    // Set/get methods
    void SetState(AculabMediaState state);
    AculabMediaState GetState();
    int GetPort();
    int GetTimeSlot() { return _tslot; }

private:
    int _port,_tslot;
    AculabMediaState _state;
};


/** AculabInterface:
 */
class AculabInterface : public DigitalNetworkInterface//public Interface
{
    typedef Map<String,int> MapStringInt;
    friend class AculabSignalling;
    friend class AculabFactory;

    int			_buf_interfacegroup;
    int			_swdrv;
    int			_port;
    ACU_CARD_ID		_card_id;
    String		_sigName;
    String		_isupName;
    String		_bssapCallName;
    int			_currentAlarm;
    MapStringInt	_mapAlarm;
    char		_serial_num[64];

    // Runtime configuration parameters
    int			_cfg_lastalarm;
    bool		_cfg_enabled;
    String		_cfg_dspfwfile;
    String		_cfg_sigfwfile;
    String		_cfg_sigfwargs;
    SignallingDriver*	_cfg_signalling;
    bool		_cfg_appendEOS;
    int			_cfg_NCI;
    int			_cfg_FCI;

    bool		_running;
    bool		_monitorL2;
    bool		_firstPoll;
    bool		_interfaceLocked;

    /** Media List manipulators
     */
    int _nextTS;
    AculabMedia* _listMedia[ACU_MAX_TIMESLOT_IN_INTERFACE];
    typedef List<AculabMedia*> QueueMedia;
    QueueMedia _queueMedia;
    void BuildMediaList();
    void EraseMediaList();

    enum ConfigId {
	// Configuration
	Cmd_Timeslot = 1,
	Cmd_Signalling,
	Cmd_Enable,
	Cmd_Group,
	Cmd_UseRLS,
	Cmd_MonitorLayer2,
	Cmd_Start,
	Cmd_Description,
	Cmd_CircuitRef,
	Cmd_SigFwFile,
	Cmd_SigFwArgs,
	Cmd_DspFwFile,
	Cmd_AppendEOS,
	Cmd_NCI,
	Cmd_FCI,

	// Specific commands
	Cmd_Alarm,
	Cmd_ShowStatus,
	Cmd_ResetStatus,
	Cmd_ReloadFirmware,
    };

    // Terminal command handlers
    void DoCmd_Timeslot(Term_Session* session,Term_Cmd* cmd);
    void DoCmd_Signalling(Term_Session* session,Term_Cmd* cmd);

    void SetAlarm(int alarmType,bool changeState=true);
    virtual void OnShowInterface ( Term_Session * session );
    void ResetStatus(Term_Session* session);
    void ReloadFirmware(Term_Session* session);

    /** HandleEvents: It is called by the Service Provider Manager to let
	this object to poll its underlying library/driver
     */
    virtual void HandleEvents();

    /** OnShutdown: This method is called when the Kernel receives
	an instruction to shutdown. Aculab interface transmits remote 
	alarm to the network on shutdown
     */
    virtual void OnShutdown(bool cancel);

    // Call counters
    int		    _cntIdle;
    int		    _cntInCall;
    int		    _cntOutCall;

    virtual bool IsActive() const;

    /** Set the signalling driver for this interface.
     */
    bool SetSignallingDriver(SignallingDriver* signalling);
    bool CommitSignallingDriver();

    virtual void OnClearCounter();

    bool	    _layer1_available;

protected:
    /* Enable:
     */
    void Enable();

    /* Disable:
     */
    void Disable();


public:
    /** Constructor/destructor
     */
    AculabInterface(const char* fullName, const char* serial_num, ACU_CARD_ID card_id, int port);
    virtual ~AculabInterface();


    inline int GetPort() {
	return _port;
    }


    inline int GetFCI() {
	return _cfg_FCI;
    }

    inline int GetNCI() {
	return _cfg_NCI;
    }

    inline int GetAppendEOS() {
	return _cfg_appendEOS;
    }


    /** Block/unblock a particular timeslot
     */
    virtual void BlockTimeslot(int timeslot,bool blocked);

    /** Return true if physical layer is properly set up
     */
    bool IsLayer1Available() const;

    //* Overloaded implementation for Aculab network
    /** AllocateMedia: A variant of AllocateMedia that specify a particular media
	to be allocated. It returns false when there is such media is not available
     */
    virtual bool AllocateMedia(Call* call,Media *&media,MediaId id,CallDirection dir,CallParameter* cparam);

    /** AllocateMedia: Called by CallManager to allocate a media in this itf.
	If a free media can be allocated, it is returned synchronously after calling
	MediaAllocatedInd method of the Call object (unless if NULL is specified 
	in call argument). It returns false when there is no free media available
     */
    virtual bool AllocateMedia(Call* call,Media *&media,SelectionStrategy sel,CallDirection dir,CallParameter* cparam);

    /** ReleaseMedia: Called by Call object when the call becomes idle. The underlying
	channel is then ready to use for another call.
     */
    virtual void ReleaseMedia(Media* media,CallDirection dir,CallParameter* cparam,CallCause cc);

    /** This function will be called by the service framework once the 
	configuration file has been fully read.
     */
    virtual bool CommitConfiguration();

    /** Load:
     */
    virtual bool Load();

    /** Unload:
     */
    virtual bool Unload();

    /** Start/Stop
     */
    virtual bool Start();
    virtual bool Stop();

    /**
     Show configuration for the specified entry.
     This function will be called by the framework whenever it needs to display the
     specified entry. Previously this instance should tell the entry that it wishes to
     handle displaying the entry by calling entry->RegisterShowHandler().
     */
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry* entry,
	String & output, 
	const String & indent, 
	int indent_size) const;

    /**
     Terminal command callback
     */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);
    /** Called by Layer1_Stats_Thread in AculabFactory
     */
    bool Device_Check_Layer1();
};


/** AculabSignalling: A singleton class which handles call control 
    signalling specific to Aculab proprietary API.
 */
class AculabSignalling : public SignallingDriver
{
    typedef Map<int,Call*> MapIntCall;
    typedef List<int> ListInt;
    typedef List<Interface*> ListInterface;

    friend class AculabInterface;
    friend class AculabCall;

    class Event {
    public:
	Event(int handle,int state) {
	    _handle = handle;
	    _state = state;
	}
	int _handle;
	int _state;
    };
    typedef List<Event*> ListEvent;

    int			_paramMaxPollCount;
    MapIntCall		_mapCall;
    ListInt		_listIncomingHandles;
    ListInterface	_listInterface;
    unsigned		_itfAttached;

    // poller thread related
    HANDLE		_hPollThread;			// handle of event poller thread
    //CRITICAL_SECTION	_csec;				// to protect event list between main and poller threads
    pthread_mutex_t 	_csec;				// to protect event list between main and poller threads
    bool		_quit;				// true if we are quitting
    ListEvent		_events;			// list of gathered events
    ListEvent		_tmpevents;

    static AculabSignalling* theAculabSignalling;

    /** Private constructor/destructor
     */
    AculabSignalling();
    ~AculabSignalling();

    /** Close all call handles which wait for incoming call on a specific ports
     */
    void RemoveCallHandleOnPort(int port);

    /** Prepare to receive call on the specified port and
	timeslot. It returns call handle or 0 if error.
     */
    int EnableIncomingCall(int port,int ts);

    /** Find an interface amongst the attached interfaces having this port
     */
    Interface* FindInterfaceByPort(int port);

    /** Called when there is a new incoming call. It creates a call object
	and notifies the Kernel
     */
    void OnIncomingCall(int handle);

    CallProgress XlatFromDeviceProgress(int port,union uniquex* p);

    void RemoveHandleFromEvents(int handle);
    //static DWORD WINAPI PollThread(void *arg);
    static DWORD PollThread(void *arg);
    void Lock();
    void Unlock();

public:
    /** Create a singleton instance of AculabSignalling
     */
    static AculabSignalling* Create();

    /** Instance: Returns a pointer to this singleton instance, if necessary
	create one instance first before returning a pointer to it
     */
    static AculabSignalling* Instance() {
	return theAculabSignalling;
    }

    /** HandleEvents: It is called by the Service Provider Manager to let
	this object to poll its underlying library/driver
     */
    virtual void HandleEvents();

    /** SetupReq: Initiate a new outgoing call in a specific Interface
	using the specified call parameter
     */
    virtual Call* SetupReq(Interface* itf,SelectionStrategy selection,CallParameter* cparam);

    /** CreateDefaultCallParameter: Returns a CallParameter structure containing
	default values for this signalling protocol
     */
    virtual CallParameter* CreateDefaultCallParameter();

    /** AttachInterface: A new Interface object is attached to this 
	SignallingDriver object
     */
    virtual bool AttachInterface(Interface* itf);

    /** DetachInterface: Remove association between a Interface object and this 
	SignallingDriver object
     */
    virtual void DetachInterface(Interface* itf);

    /** DisableInterface: an interface has been disabled, the signalling
	driver should take the necessary action if it can
     */
    virtual bool DisableInterface(Interface* itf);

    /** EnableInterface: an interface has been enabled, the signalling
	driver should take the necessary action if it can
     */
    virtual bool EnableInterface(Interface* itf);

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

};


/** AculabFactory: A singleton class which :
    - Initialize Aculab library
    - Instantites all AculabInterface
    - Close Aculab library
 */
class AculabFactory : public ServiceProvider
{

    typedef List<AculabInterface*> ListInterface;

public:
    virtual ~AculabFactory();

    /** Create: create one instance first before returning a pointer to it
     */
    static AculabFactory* Create();

    /** Instance: Returns a pointer to this singleton instance, if necessary
	create one instance first before returning a pointer to it
     */
    static AculabFactory* Instance() {
	return theAculabFactory;
    }

    void Destroy();

private:
    AculabFactory();

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
	Initialize Aculab driver and for each Aculab port, create the associated
	Interface object
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
	Close Aculab driver
     */
    virtual bool Unload();

    /** HandleEvents: Descendent shall implement this abstract method.
	It is called by Service Provider Manager
     */
    virtual void HandleEvents();

    /** This thread will check the state of all layer 1 interface
     */
    //static DWORD WINAPI Layer1_Stats_Thread(void *arg);
    static DWORD Layer1_Stats_Thread(void *arg);
    HANDLE _quit_event;
    HANDLE _thread;

    time_t _l1CheckTime;
    bool _fOpened;
    static AculabFactory* theAculabFactory;
    ListInterface _interfaces;
};

#endif // !defined(CT2_ACULAB_H__INCLUDED_)
