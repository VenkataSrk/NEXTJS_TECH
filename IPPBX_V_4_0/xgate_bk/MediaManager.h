/*----------------------------------------------------------------* 
 * $Archive: /xGate/MediaManager.h $
 * $Date: 7/13/04 2:01p $
 * $Revision: 60 $
 *
 * $History: MediaManager.h $
 * 
 * *****************  Version 60  *****************
 * User: Bennylp      Date: 7/13/04    Time: 2:01p
 * Updated in $/xGate
 * Only update RAP every one second
 * 
 * *****************  Version 59  *****************
 * User: Sonny        Date: 25/06/04   Time: 11:28
 * Updated in $/xGate
 * 
 * *****************  Version 58  *****************
 * User: Bennylp      Date: 4/09/03    Time: 11:03a
 * Updated in $/xGate
 * Added interface type for interface (for SNMP purpose)
 * 
 * *****************  Version 57  *****************
 * User: Sonny        Date: 2/18/03    Time: 3:04p
 * Updated in $/xGate
 * fixed device usage misscalc
 * 
 * *****************  Version 56  *****************
 * User: Bennylp      Date: 6/12/02    Time: 3:29p
 * Updated in $/xGate
 * Added GetCapacity()
 * 
 * *****************  Version 55  *****************
 * User: Bennylp      Date: 5/24/02    Time: 6:07p
 * Updated in $/xGate
 * Fixed miscalculation of average connect time statistics
 * 
 * *****************  Version 54  *****************
 * User: Sonny        Date: 5/15/02    Time: 4:23p
 * Updated in $/xGate
 * adaptive synchronization (beta)
 * 
 * *****************  Version 53  *****************
 * User: Adham        Date: 9/04/02    Time: 16:27
 * Updated in $/xGate
 * Moved GetDisabledCause to public for SNMP
 * 
 * *****************  Version 52  *****************
 * User: Adham        Date: 3/04/02    Time: 19:58
 * Updated in $/xGate
 * Bug fix on CallStatistic (int to float)
 * 
 * *****************  Version 51  *****************
 * User: Adham        Date: 3/04/02    Time: 16:49
 * Updated in $/xGate
 * GetSCR moved to public for SNMP access
 * 
 * *****************  Version 50  *****************
 * User: Sonny        Date: 13/02/02   Time: 15:43
 * Updated in $/xGate
 * 
 * *****************  Version 49  *****************
 * User: Sonny        Date: 11/02/02   Time: 16:30
 * Updated in $/xGate
 * quality monitoring updates
 * 
 * *****************  Version 48  *****************
 * User: Sonny        Date: 7/02/02    Time: 12:25
 * Updated in $/xGate
 * Interface::GetStatiistics
 * 
 * *****************  Version 47  *****************
 * User: Sonny        Date: 6/02/02    Time: 19:03
 * Updated in $/xGate
 * 
 * *****************  Version 46  *****************
 * User: Sonny        Date: 6/02/02    Time: 19:01
 * Updated in $/xGate
 * optimizations for quality monitoring
 * 
 * *****************  Version 45  *****************
 * User: Sonny        Date: 4/02/02    Time: 12:33
 * Updated in $/xGate
 * calls get quality from media
 * 
 * *****************  Version 44  *****************
 * User: Sonny        Date: 1/02/02    Time: 12:20
 * Updated in $/xGate
 * statistics monitor updates
 * 
 * *****************  Version 43  *****************
 * User: Sonny        Date: 31/01/02   Time: 13:12
 * Updated in $/xGate
 * add call quality monitoring
 * 
 * *****************  Version 42  *****************
 * User: Junanto      Date: 31/10/01   Time: 16:42
 * Updated in $/xGate
 * Prevent calls from being made on disabled interfaces
 * 
 * *****************  Version 41  *****************
 * User: Junanto      Date: 24/10/01   Time: 15:13
 * Updated in $/xGate
 * ARQ/DRQ/RII are replaced by periodical RII during 
 * MediaManager's HandleEvents()
 * 
 * *****************  Version 40  *****************
 * User: Junanto      Date: 16/08/01   Time: 21:12
 * Updated in $/xGate
 * Integrated with DeviceUsage class
 * 
 * *****************  Version 39  *****************
 * User: Junanto      Date: 9/08/01    Time: 19:59
 * Updated in $/xGate
 * Call selection strategy
 * 
 * *****************  Version 38  *****************
 * User: Bennylp      Date: 7/30/01    Time: 11:45a
 * Updated in $/xGate
 * Changed SetMediaCount semantic, added SetCapacity.
 * 
 * *****************  Version 37  *****************
 * User: Bennylp      Date: 7/23/01    Time: 1:25p
 * Updated in $/xGate
 * Added generic clear interface counter.
 * 
 * *****************  Version 36  *****************
 * User: Iwanj        Date: 7/19/01    Time: 9:28p
 * Updated in $/xGate
 * 
 * *****************  Version 35  *****************
 * User: Iwanj        Date: 7/19/01    Time: 9:06p
 * Updated in $/xGate
 * add interface's direction and distribution info 
 * 
 * *****************  Version 34  *****************
 * User: Bennylp      Date: 7/19/01    Time: 2:17p
 * Updated in $/xGate
 * Changes to MediaManager functions to support rtp/aud shared stats.
 * 
 * *****************  Version 33  *****************
 * User: Junanto      Date: 18/07/01   Time: 18:59
 * Updated in $/xGate
 * Added support for ISUP circuit/group blocking/unblocking
 * 
 * *****************  Version 32  *****************
 * User: Junanto      Date: 18/07/01   Time: 12:52
 * Updated in $/xGate
 * Added flag to modify only mediaCount without affecting 
 * initial count
 * 
 * *****************  Version 31  *****************
 * User: Bennylp      Date: 7/13/01    Time: 3:11p
 * Updated in $/xGate
 * Reorganization of call statistic counters.
 * 
 * *****************  Version 30  *****************
 * User: Adham        Date: 7/13/01    Time: 10:56a
 * Updated in $/xGate
 * SNMP Framework integration
 * 
 * *****************  Version 29  *****************
 * User: Adham        Date: 7/09/01    Time: 1:28p
 * Updated in $/xGate
 * Adding new fields for Interface
 * 
 * *****************  Version 28  *****************
 * User: Adham        Date: 7/09/01    Time: 1:14p
 * Updated in $/xGate
 * new method templates for counter 
 * 
 * *****************  Version 27  *****************
 * User: Junanto      Date: 4/07/01    Time: 17:33
 * Updated in $/xGate
 * Added SCR tracing and display
 * 
 * *****************  Version 26  *****************
 * User: Bennylp      Date: 6/30/01    Time: 8:12p
 * Updated in $/xGate
 * Changed RAPEnabled() to const.
 * 
 * *****************  Version 25  *****************
 * User: Junanto      Date: 25/06/01   Time: 16:40
 * Updated in $/xGate
 * Added support for CallSetup with wild card 
 * specification
 * 
 * *****************  Version 24  *****************
 * User: Junanto      Date: 24/06/01   Time: 20:08
 * Updated in $/xGate
 * Changed AttachInterface to return bool
 * 
 * *****************  Version 23  *****************
 * User: Bennylp      Date: 6/22/01    Time: 11:02p
 * Updated in $/xGate
 * Changed some members to const, and others to non virtual, etc.
 * 
 * *****************  Version 22  *****************
 * User: Bennylp      Date: 6/06/01    Time: 8:00p
 * Updated in $/xGate
 * Added regex selection to show service/interface command.
 * 
 * *****************  Version 21  *****************
 * User: Junanto      Date: 6/06/01    Time: 15:54
 * Updated in $/xGate
 * OnShutdown has a new parameter which indicates
 * that a requested shutdown has been cancelled
 * 
 * *****************  Version 20  *****************
 * User: Junanto      Date: 6/06/01    Time: 10:39
 * Updated in $/xGate
 * Added circuit-reference command
 * Included nb of active calls into interface list
 * 
 * *****************  Version 19  *****************
 * User: Junanto      Date: 4/06/01    Time: 13:54
 * Updated in $/xGate
 * Output of interface list contains nb of connected/busy
 * for incoming/outgoing calls
 * 
 * *****************  Version 18  *****************
 * User: Junanto      Date: 2/06/01    Time: 16:55
 * Updated in $/xGate
 * Added new command interface show/list
 * 
 * *****************  Version 17  *****************
 * User: Junanto      Date: 21/05/01   Time: 15:54
 * Updated in $/xGate
 * Added call holding and connected duration (in secs)
 * 
 * *****************  Version 16  *****************
 * User: Junanto      Date: 17/05/01   Time: 18:20
 * Updated in $/xGate
 * - Interface can choose to be pollable
 * - Disable() can specify the reason
 * 
 * *****************  Version 15  *****************
 * User: Iwanj        Date: 5/09/01    Time: 5:45p
 * Updated in $/xGate
 * Promote RAPEnabled to public
 * 
 * *****************  Version 14  *****************
 * User: Junanto      Date: 4/05/01    Time: 18:20
 * Updated in $/xGate
 * Added support for dynamic interface creation/deletion
 * 
 * *****************  Version 13  *****************
 * User: Junanto      Date: 4/05/01    Time: 16:54
 * Updated in $/xGate
 * Interface can choose to use RAP or not
 * 
 * *****************  Version 12  *****************
 * User: Bennylp      Date: 5/02/01    Time: 12:02p
 * Updated in $/xGate
 * Added show interface framework.
 * 
 * *****************  Version 11  *****************
 * User: Iwanj        Date: 4/27/01    Time: 5:17p
 * Updated in $/xGate
 * added interface iterator
 * 
 * *****************  Version 10  *****************
 * User: Junanto      Date: 24/04/01   Time: 18:26
 * Updated in $/xGate
 * first link between Prosody voice / Aculab media with
 * switching framework
 * 
 * *****************  Version 9  *****************
 * User: Iwanj        Date: 4/24/01    Time: 4:01p
 * Updated in $/xGate
 * 
 * *****************  Version 8  *****************
 * User: Bennylp      Date: 4/23/01    Time: 10:44p
 * Updated in $/xGate
 * Fixed std namespace conflicts.
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 18/04/01   Time: 11:51
 * Updated in $/ct15
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 15/04/01   Time: 20:48
 * Updated in $/ct15
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 14/04/01   Time: 20:00
 * Updated in $/ct15
 * Add more APIs
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 13/04/01   Time: 21:03
 * Updated in $/ct15
 * 1st version of integration with call manager
 * 
 * *****************  Version 3  *****************
 * User: Junanto      Date: 11/04/01   Time: 22:02
 * Updated in $/ct15
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 11/04/01   Time: 11:37
 * Updated in $/ct15
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 10/04/01   Time: 18:13
 * Created in $/ct15
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 8/04/01    Time: 19:44
 * Updated in $/ct2
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 3/04/01    Time: 11:30
 * Created in $/ct2
 * 
 *----------------------------------------------------------------*/

#if !defined(CT2_MEDIAMANAGER_H__INCLUDED_)
#define CT2_MEDIAMANAGER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "call.h"
#include "serviceprovider.h"
#include "cfg.h"
#include "Switching.h"
#include "devusage.h"
#include "timed_stat.h"

#define UINT_MAX ((unsigned )-1)

/** Forward declaration
 */
class SignallingDriver;
class Interface;
class Call;
class InterfaceAPI;


/** SelectionStrategy: It defines which device/media is allocated next. After each 
    media usage, the media is put back at the end of a list of free pool. The next 
    time a media is requested, depending on the allocation strategy a free media is 
    returned:
    - Ascending: the media with smallest timeslot
    - Descending: the media with the highest timeslot
    - FIFO: the head of the list
    - LIFO: the tail of the list
    - Uniform: equivalent to FIFO
 */
enum SelectionStrategy
{
    SS_Ascending,
    SS_Descending,
    SS_FIFO,
    SS_LIFO,
    SS_Uniform,
};


typedef int MediaId;

/** Media: Base class for media. A media is the representation of 
    a channel and it supports TDM switching. A media is allocated
    to a call and belongs to a itf.
    During its lifetime, a media may signal an occurence of physical
    problem in transmission medium to the associated Call object by 
    calling MediaUnavailableInd. It depends on the Call obejct to 
    disconnect or not the active call on receipt of such indication.
 */
class Media : public SupportSwitching
{
    friend class Interface;
protected:
    Interface* _itf;
    Call* _call;
    MediaId _id;
    String _name;
    unsigned _tmHolding;
    unsigned _tmConnected;
    unsigned int _direction; // T 18092018 // added for ippbx

public:
    /** Constructor/destructor
     */
    Media(const char *name,MediaId id,Interface *itf,SwitchDevice *swdev);
    virtual ~Media();

    void AttachCall(Call *call);
    void DetachCall(CallDirection dir,CallParameter *cparam,CallCause cc);
    void SetDuration(unsigned tmHolding,unsigned tmConnected);
    Call *GetCall();
    MediaId GetId();
    const char *GetName();
    Interface *GetInterface();
	MediaId GetMediaId() {return _id;}

//// added for ippbx
    void SetInterface(Interface *itf){_itf = itf;}
        // T 18092018
    void SetDirection(int dir){ _direction = dir;}
    int GetDirection(){return _direction; }

    unsigned GetHoldingTime() {
	return _tmHolding;
    }

    unsigned GetConnectedTime() {
	return _tmConnected;
    }

    /** This will be called by the Call instance when the call state changes.
	The only guaranteed notification is when the call is connected and
	disconnected.
	In this implementation, this will update the number of connected media
	in the interface.
     */
    void NotifyCallState (Call *call, CallState call_state);

    /** Get Interface Quality
    */
    virtual float GetQuality(CallQualityParameter param) { return -1.0f; }
    
};


/** ObjectState
 */
enum ObjectState
{
    OS_Closed,
    OS_OutOfService,
    OS_InService,
};


/** Left indentation for show interface output.
 */
#define SHOW_INTERFACE_INDENT	"  "

enum DisableCause
{
    DC_Administrative,
    DC_Layer1,
    DC_Shutdown,
};

enum Interface_Type
{
    ITF_UNKNOWN,
    ITF_E1_T1,
    ITF_ATM,
    ITF_RTP,
    ITF_PIPE,
    ITF_ANALOG,
    ITF_AUDIOCODES,
    ITF_TRUNK,
    ITF_PROSODYX,
};

enum CallStatistics {
    CALLSTAT_ATTEMPT = 100, 
    CALLSTAT_SUCCESS, 
    CALLSTAT_FAILED,
    CALLSTAT_CONNECTED,
    CALLSTAT_SCR,
    CALLSTAT_ASR,
    CALLSTAT_AVG_CNKTIME,
    CALLSTAT_AVG_PDD,
};

/** Interface: It represents a group of medias located on a single physical port
 */
class Interface : public ServiceProvider, public DeviceUsage
{
    friend class MediaManager;
    friend class Media;		// Inc/DecIncomingConnectedCount
    friend class CallAPI;

protected:
    SignallingDriver* _signalling;
    String _name;
    String _description;
    String _groupName;
    String _circuitRef;
    char _resourceType;
    SelectionStrategy _sel;
    ObjectState _state;
    Cfg_Tree_Group* _cfgTree;
    bool _enabled;
    bool _mediaCountChanged;
    time_t _timeLastUsage;
    bool _useRap;
    DisableCause _disableCause;
    int _statisticsInterval;

    unsigned _hardwareType;
    char _direction,_distribution;

    /** Active call counters
     */
    enum {
	/** This is the array size for statistic counters.
	    For example, for _callBusy counter, we'll have:
		_callBusy[CD_Incoming], and
		_callBusy[CD_Outgoing].
	    Since CD_Outgoing is equal to 2, then this constant must be 3.
	 */
	Statistic_Size = 3
    };
    unsigned _mediaCount, _initialMediaCount;

    ///////////////////////////////////////////////////////////////////////////
    /** This variable serves as the marker for the beginning of statistic
	variables declaration. We will then memset() any area from this 
	variable to stat_marker_end.
     */
    char stat_marker_begin;

    /** Counters for current state.
     */
    unsigned _callBusy[Statistic_Size],		    // current call busy
	     _callConnected[Statistic_Size];	    // current call connected

    /* typedef unsigned int64_t LARGE_UINT; */

    /** Counters since the interface was created.
     */
    unsigned _totCarrierTrans;
    unsigned _totCallAttempt[Statistic_Size],	    // total call attempts so far
	     _totCallConnected[Statistic_Size],	    // total call connected so far
	     _totCallSuccess[Statistic_Size],	    // total call successfully delivered
	     _totCallFailed[Statistic_Size],	    // total call failed
	     _totCallConnected_Fin[Statistic_Size], // total call connected, not including current call connected
	     _totCallAttempt_Fin[Statistic_Size];   // total call attempts, not including current calls
    int64_t  _totCallHoldingTime[Statistic_Size],
	       _totCallConnectedTime[Statistic_Size];

    char stat_clrmarker_begin;

    /** Counters since the last time the counter is cleared.
     */
    unsigned _lastClr_CallAttempt[Statistic_Size],	    // total call attempts so far
	     _lastClr_CallConnected[Statistic_Size],	    // total call connected so far
	     _lastClr_CallSuccess[Statistic_Size],	    // total call successfully delivered
	     _lastClr_CallFailed[Statistic_Size],	    // total call failed
	     _lastClr_CallConnected_Fin[Statistic_Size],    // total call connected, not including current call connected
	     _lastClr_CallAttempt_Fin[Statistic_Size];	    // total call attempts, not including current calls
    int64_t _lastClr_CallHoldingTime[Statistic_Size],
	       _lastClr_CallConnectedTime[Statistic_Size];

    char stat_clrmarker_end;

    /** When the counter is cleared.
     */
    time_t   _timeLastClearingCounter;

    /** This variable serves as the marker for the end of statistics variables
	declaration.
     */
    char stat_marker_end;

    /** TRUE if the interface is dynamically created by application
     */
    bool _dynamic;

    ///////////////////////////////////////////////////////////////////////////

    unsigned _last100Success, _last100Failed;
    time_t _lastWarning;
    bool _last100list[100];
    unsigned _last100cnt;
    unsigned _last100nxt;


    timed_statistic<int, 1, 24>   _hourlyCallAttempt,
				  _hourlyCallConnected, 
				  _hourlyCallSuccess;
    timed_statistic<float, 1, 24> _hourlyCallAvgConnectedDuration,
				  _hourlyCallAvgPostDialDelay;

    //* Resource Location
    /** SetMediaCount: Set the number of channels available at present.  The 
	interface capacity will calculated as sum of total channels available
	and total channels currently being used.

	It sends an RII message to the Resource Location server
     */
    void SetMediaCount(unsigned mediaCount);

    /** Set interface capacity (initial media count).
	Current media available will be calculated as new capacity substracted
	by current media channels being used.

	The new capacity must not be less than current media being used.
	This function will send RII message to the Resource Location server

	@return true if capacity has been updated successfully.
     */
    bool SetCapacity(unsigned capacity);

    /** Disable: Exclude the itf from further selection
	It sends an RII message with 0 number of channels to the 
	Resource Location server
     */
    virtual void Disable();

    /** Disable: Disable with cause
     */
    virtual void Disable(DisableCause cause);


    /** Enable: Re-include the itf from further selection
	It sends an RII message with the current number of channels to the 
	Resource Location server
     */
    virtual void Enable();

    /** Display interface specific information to the terminal user.
	Decendants MUST implement this function.
     */
    virtual void OnShowInterface(Term_Session * session ) = 0;

    /** Clear counter.
     */
    virtual void OnClearCounter();

    /** Set the inteface reference.
     */
    void SetCircuitReference ( const String & circuitRef );

    /** Enable RAP usage
     */
    void EnableRAP();

    /** Disable RAP usage
     */
    void DisableRAP();

    /** Set the interface state.
     */
    void SetState(ObjectState state);

    /** Set the signalling driver for this interface.
     */
    bool SetSignallingDriver(SignallingDriver *signalling);
   
     /** Set the RLS resource type(0=unspecified)
      */
     void SetResourceType(char resourceType);
   
    /** Set the RLS resource type using string representation
     */
    void SetResourceTypeStr(const char* res);
    
    // helper to add resource type command
    static void AddResourceTypeCmd(Term_Cmd_Target *target, int id,         Cfg_Tree_Group *_cfgTree_);
 

    /** Warn if SCR is getting low
     */
    void CheckSCR();    

    enum { DEADCODE = 0xDEADC0DE };
    unsigned _signature;

public:
    bool IsValid() 
    {
	try {
	    return (_signature == DEADCODE);
	} catch (...) {
	    return false;
	}
    }
    /** Disable: Disable with cause
     */
    DisableCause GetDisableCause() {
	return _disableCause;
    }

    /** Set the inteface description.
     */
    bool IsDynamic() {
	return _dynamic;
    }

    /** Set the inteface description.
     */
    void SetDescription(const String & desc );

    /** Returns SCR
     */
    unsigned GetSCR();

    /** Constructor/destructor
     */
    Interface(Interface_Type type, const char *name, 
	      bool needPoll=false,
	      Priority prio=Normal_Priority,
	      bool dynamic=false);
    virtual ~Interface();

    /** Get the interface state.
     */
    ObjectState GetState() const;

    /** Get the signalling driver associated with this interface.
	This function might return NULL for interface can can be configured
	to use different signalling.
     */
    SignallingDriver *GetSignallingDriver();
    /** Get the resource type (unspecified/tdm/voip) to be used
        as resource differentiation in resource allocator server
    */
    char GetResourceType() const;
    
    /** Get the resource type (unspecified/tdm/voip) in string repre        sentation
     */
    String GetResourceTypeStr() const;
   
    /** Get the interface capacity (initial media count).
     */
    unsigned GetCapacity() const;

    /** Get the number of media/channel available to be used (in other words,
	the number of free media/channel).
     */
    unsigned GetMediaCount() const;

    /** Get the administrative group name assigned to this interface.
     */
    const char *GetGroupName() const;

    /** Set the administrative group name to be associated with this interface.
     */
    void SetGroupName(const char*);

    /** Set interface direction
	Value: I(nbound),O(utbound),B(othway)
    */
    void SetDirection(char direction) {
	_direction = direction;
    }

    /** Get interface direction
    */
    char GetDirection() const {
	return _direction;
    }

    /** Set interface distribution
	Value: A(scending),D(escending),F(ifo),L(ifo),U(niform)
    */
    void SetDistribution(char distribution) {
	_distribution = distribution;
	switch (tolower(distribution)) {
	case 'a':
	    _sel = SS_Ascending;
	    break;
	case 'd':
	    _sel = SS_Descending;
	    break;
	case 'f':
	    _sel = SS_FIFO;
	    break;
	case 'l':
	    _sel = SS_LIFO;
	    break;
	case 'u':
	default:
	    _sel = SS_Uniform;
	    break;
	}
    }

    /** Get interface distribution
    */
   char GetDistribution() const {
	return _distribution;
    }

    /** Get interface distribution
    */
   SelectionStrategy GetSelectionStrategy() const {
	return _sel;
    }

    /** Determine whether the interface is active.
	The interface is active if it is enabled and running.
     */
    virtual bool IsActive() const;

    /** Determine whether this interface is enabled.
     */
    bool IsEnabled() const {
	return _enabled;
    }

    /** Determine whether Resource Allocation Protocol is enabled for this 
	interface.
     */
    bool RAPEnabled() const;

    /** Get the interface description.
     */
    const String & GetDescription() const;

    /** Get the interface reference
     */
    const String & GetCircuitReference() const;

    /** Display interface information to the terminal users. This function will
	display common/generic information first, then show interface specific
	information by calling OnShowInterface().
     */
    void ShowInterface ( Term_Session * session );

    /**
     Terminal command callback
     */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

    
    /** Show call statistics
     */
    void PrintData(Term_Session * session, time_t start, time_t end, int code);
    void ShowCallStatistics(Term_Session* session, int interval);

    //* Abstract methods
    /** AllocateMedia: Called by CallManager to allocate a media in this itf.
	If a free media can be allocated, it may be returned synchronously or
	asynhronously (by calling MediaAllocatedInd method of Call object).
	NULL may be specified in call argument in the case of synchronous
	allocation.
	It returns false when there is no free media available
     */
    virtual bool AllocateMedia(Call *call,Media *&media,SelectionStrategy sel,CallDirection dir,CallParameter *cparam) = 0;

    /** AllocateMedia: A variant of AllocateMedia that specify a particular media
	to be allocated. It returns false when there is such media is not available
     */
    virtual bool AllocateMedia(Call *call,Media *&media,MediaId id,CallDirection dir,CallParameter *cparam) = 0;

    /** ReleaseMedia: Called by Call object when the call becomes idle. The underlying
	channel is then ready to use for another call.
     */
    virtual void ReleaseMedia(Media *media,CallDirection dir,CallParameter *cparam,CallCause cc) = 0;

    /** Load: Called once at initialization time
     */
    virtual bool Load() = 0;

    /** Unload: Called once at termination time
     */
    virtual bool Unload() = 0;

    /** Open: May be called several times to start the object
	The object may be then in In-Service (enabled/disabled) state
     */
    virtual bool Start() = 0;

    /** Close: Once started, an object may be requested to close
	The object goes to the Out-Of-Service state
     */
    virtual bool Stop() = 0;

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
	int indent_size) const = 0;

    virtual Cfg_Tree_Group *GetConfigTree() {
	return _cfgTree;
    }

    /** Interfaces which support dynamic creation/deletion must implement 
	the following method.
     */

    /** It returns true when the interface support dynamic deletion
     */
    virtual bool SupportDynamicDeletion();

    /** Called if the interface which support dynamic deletion is about
	to be destroyed
     */
    virtual void DestroyInterface();

    /** DecMediaCount: Decrease the number of channels available
	for usage, a channel is allocated for incoming or outgoing call
	with a specific call parameters.
	It sends an ARQ message to the Resource Location server
     */
    virtual void DecMediaCount(Media *med, CallDirection dir,CallParameter *cparam,bool dontUseRLS=false);

    /** IncMediaCount: Increase the number of channels available
	for usage, a channel is deallocated for incoming or outgoing 
	call with a specific call parameters
	It sends a DRQ message to the Resource Location server
     */
    virtual void IncMediaCount(Media *med, CallDirection dir,CallParameter *cparam,CallCause cc,bool dontUseRLS=false);

    /** Increment current busy counter.
     */
    virtual void IncCurrentBusyCount(Media *med, CallDirection dir) {
	assert(dir==CD_Incoming || dir==CD_Outgoing);
	++_callBusy[dir];
	StartUsage();
    }
    /** Decrement current busy counter.
     */
    virtual void DecCurrentBusyCount(Media *med, CallDirection dir) {
	assert(dir==CD_Incoming || dir==CD_Outgoing);
	--_callBusy[dir];
    }
    /** Increment current connected counter.
     */
    virtual void IncCurrentConnectedCount(Media *med, CallDirection dir) {
	assert(dir==CD_Incoming || dir==CD_Outgoing);
	++_callConnected[dir];
    }
    /** Decrement current connected counter.
     */
    virtual void DecCurrentConnectedCount(Media *med, CallDirection dir) {
	assert(dir==CD_Incoming || dir==CD_Outgoing);
	--_callConnected[dir];
    }
    

    /** Increment total call attempts counter.
     */
    virtual void IncTotalCallAttempts(Media *med, CallDirection dir) {
	assert(dir==CD_Incoming || dir==CD_Outgoing);
	++_totCallAttempt[dir];
	++_lastClr_CallAttempt[dir];

	_hourlyCallAttempt.add(1);
    }
    /** Increment total call connected counter.
     */
    virtual void IncTotalCallConnected(Media *med, CallDirection dir) {
	assert(dir==CD_Incoming || dir==CD_Outgoing);
	++_totCallConnected[dir];
	++_lastClr_CallConnected[dir];

	_hourlyCallConnected.add(1);
    }
    /** Increment total call succree counter.
     */
    virtual void IncTotalCallSuccess(Media *med, CallDirection dir);

    /** Increment total call failed counter.
     */
    virtual void IncTotalCallFailed(Media *med, CallDirection dir);

    /** Increment total call holding time.
     */
    virtual void IncTotalCallHoldingTime(Media *med, CallDirection dir, unsigned seconds) {
	assert(dir==CD_Incoming || dir==CD_Outgoing);
	_totCallHoldingTime[dir] += seconds;
	++_totCallAttempt_Fin[dir];

	_lastClr_CallHoldingTime[dir] += seconds;
	++_lastClr_CallAttempt_Fin[dir];

   
    }
    /** Increment total call connected time.
     */
    virtual void IncTotalCallConnectedTime(Media *med, CallDirection dir, unsigned seconds) {
	assert(dir==CD_Incoming || dir==CD_Outgoing);
	_totCallConnectedTime[dir] += seconds;
	++_totCallConnected_Fin[dir];
	_lastClr_CallConnectedTime[dir] += seconds;
	++_lastClr_CallConnected_Fin[dir];

	_hourlyCallAvgConnectedDuration.add_avg(seconds);
    }

    /** Get the number of calls in connected state (both parties are talking).
     */
    unsigned GetCurrentConnectedCount(CallDirection dir) const {
	return dir==CD_Both ? _callConnected[CD_Incoming]+_callConnected[CD_Outgoing] : _callConnected[dir];
    }
    /** Get the number of calls in busy state (the call has not been connected yet).
     */
    unsigned GetCurrentBusyCount(CallDirection dir) const {
	return dir==CD_Both ? _callBusy[CD_Incoming]+_callBusy[CD_Outgoing] : _callBusy[dir];
    }
    /** Get the total calls attempts to this interface so far.
     */
    unsigned GetTotalAttempts(CallDirection dir) const {
	return dir==CD_Both ? _totCallAttempt[CD_Incoming]+_totCallAttempt[CD_Outgoing] : _totCallAttempt[dir];
    }
    /** Get the total calls successfully delivered so far.
     */
    unsigned GetTotalSuccess(CallDirection dir) const {
	return dir==CD_Both ? _totCallSuccess[CD_Incoming]+_totCallSuccess[CD_Outgoing] : _totCallSuccess[dir];
    }
    /** Get the total calls successfully connected so far.
     */
    unsigned GetTotalConnected(CallDirection dir) const {
	return dir==CD_Both ? _totCallConnected[CD_Incoming]+_totCallConnected[CD_Outgoing] : _totCallConnected[dir];
    }
    /** Get the total calls successfully connected so far.
     */
    unsigned GetTotalFailed(CallDirection dir) const {
	return dir==CD_Both ? _totCallFailed[CD_Incoming]+_totCallFailed[CD_Outgoing] : _totCallFailed[dir];
    }
    /** Get the low 32 bits of total holding time of this interface.
     */
    unsigned GetTotalHoldingTime_Lo(CallDirection dir) const {
	int64_t holding =  dir==CD_Both ? _totCallHoldingTime[CD_Incoming]+_totCallHoldingTime[CD_Outgoing] : 
					     _totCallHoldingTime[dir];
	return (unsigned)(holding & UINT_MAX);
    }
    /** Get the hi 32 bits of total holding time of this interface.
     */
    unsigned GetTotalHoldingTime_Hi(CallDirection dir) const {
	int64_t holding =  dir==CD_Both ? _totCallHoldingTime[CD_Incoming]+_totCallHoldingTime[CD_Outgoing] : 
					     _totCallHoldingTime[dir];
	return (unsigned) (holding >> (sizeof(int64_t) - 32));
    }
    /** Get the low 32 bits of total connected time of this interface.
     */
    unsigned GetTotalConnectedTime_Lo(CallDirection dir) const {
	int64_t connect =  dir==CD_Both ? _totCallConnectedTime[CD_Incoming]+_totCallConnectedTime[CD_Outgoing] : 
					     _totCallConnectedTime[dir];
	return (unsigned) (connect & UINT_MAX);
    }
    /** Get the hi 32 bits of total connected time of this interface.
     */
    unsigned GetTotalConnectedTime_Hi(CallDirection dir) const {
	int64_t connect =  dir==CD_Both ? _totCallConnectedTime[CD_Incoming]+_totCallConnectedTime[CD_Outgoing] : 
					     _totCallConnectedTime[dir];
	return (unsigned) (connect >> (sizeof(int64_t) - 32));
    }
    /** Get hardware type.
     */
    int GetHardwareType() const {
	return _hardwareType;
    }
    /** Get total number of carrier transition.
     */
    unsigned GetTotalCarrierTrans() const {
	return _totCarrierTrans;
    }

    /** Get CallStatistics
    */
    float GetStatistics(CallStatistics callstat, time_t start, time_t end);

    /** Get Interface Quality
    */
    virtual float GetQuality(CallQualityParameter param, time_t start, time_t end, bool active) { return -1.0f; }

};


/** InterfaceManager: Dynamic interfaces are handled by object descendant
    from this base class
 */
class InterfaceManager
{
public:
    /** Constructor/destructor
     */
    InterfaceManager(const char *name);
    virtual ~InterfaceManager();

    /** Return the name of this object
     */
    const char *GetName();

    virtual bool CanCreateInterface(const char *name) = 0;
    virtual Interface *CreateInterface(const char *name) = 0;

private:
    String _name;
};


/** MediaManager: This singleton entity is the entity responsible for
    managing all medias/channels
 */
class MediaManager : public ServiceProvider
{
public:
    virtual ~MediaManager();

    /** GetInstance: Returns a pointer to the Media Manager instance, if necessary
	create one instance first before returning a pointer to it
     */
    static MediaManager *GetInstance();

    /** GetConfigTree: Returns a pointer to the configuration tree of interface sub commands
     */
    Cfg_Tree_Group *GetConfigTree();

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

    /** Show all interface information.
     */
    void ShowInterface ( Term_Session *session, const String & selector );

    /** Add/remove an interface from the list, called by Interface constructore/destructor
     */
    void InsertInterface(Interface *itf);
    void RemoveInterface(Interface *itf);

    /** Init: Open all attached media itfs and start them one by one
	Deinit: Stop all attached media itfs and close them one by one
     */
    bool Init();
    bool Deinit();

    Interface *FindInterface(const char *name);
    Interface *ItGetFirstInterface();
    Interface *ItGetNextInterface();

    // round-robin version
    Interface *ItGetInterfaceRoundRobin();
    unsigned GetInterfaceCount();

    /** Called to register or deregister an interface manager
     */
    void RegisterInterfaceManager(InterfaceManager *mgr);
    void RemoveInterfaceManager(InterfaceManager *mgr);

    void TraceSCR(const char* msg);

private:
    typedef List<Interface*> ListInterface;
    typedef List<InterfaceManager*> ListManager;

    enum Cmd_Id {
	Cmd_InterfaceConfig,
	Cmd_InterfaceNormal,
    };

    ListInterface _listInterface;
    ListManager _listManager;
    Cfg_Tree_Group* _interfaceCfgTree;
    ListInterface::iterator _iter;
    ListInterface::iterator _iterRR;
    Log_Trace* _traceSCR;
    InterfaceAPI* _interfaceAPI;
    time_t	_lastRIISent;

    MediaManager();

    /** Load: Called once at initialization time
     */
    virtual bool Load();

    /** Unload: Called once at termination time
     */
    virtual bool Unload();

    /** HandleEvents: Descendent shall implement this abstract method.
	It is called by Service Provider Manager
     */
    virtual void HandleEvents();

    /** Show interface, in brief/listing format.
     */
    void ShowInterfaceBrief ( Term_Session *session, const char *itfname );
    void ShowInterfaceStatistics( Term_Session *session, const char *itfname, int start_hour, int end_hour );

//    void OpenSync(in_addr

};

#endif // !defined(CT2_MEDIAMANAGER_H__INCLUDED_)
