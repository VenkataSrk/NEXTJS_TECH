/*----------------------------------------------------------------* 
 * $Archive: /xGate/Isup.h $
 * $Date: 17/06/02 12:33 $
 * $Revision: 14 $
 *
 * $History: Isup.h $
 * 
 * *****************  Version 14  *****************
 * User: Junanto      Date: 17/06/02   Time: 12:33
 * Updated in $/xGate
 * added new commands in ISUP:
 *   block-faulty-TS
 *   auto-proceeding
 * 
 * *****************  Version 13  *****************
 * User: Junanto      Date: 26/12/01   Time: 18:47
 * Updated in $/xGate
 * Added new parameters (nature-of-call-indicators,
 * forward-call-indicators, backward-call-indicators)
 * 
 * *****************  Version 12  *****************
 * User: Junanto      Date: 5/10/01    Time: 11:07
 * Updated in $/xGate
 * Changed HandleEvents from individual signalling to
 * the IsupManager
 * 
 * *****************  Version 11  *****************
 * User: Junanto      Date: 16/08/01   Time: 16:37
 * Updated in $/xGate
 * On AttachInterface internal circuit state is reset to known state
 * Remote/local unblock is not interchangeable
 * Timeslots are blocked when connection to the controller is lost
 * During shutdown, a maintenance block is sent to the network
 * 
 * *****************  Version 10  *****************
 * User: Junanto      Date: 19/07/01   Time: 12:27
 * Updated in $/xGate
 * Alligned continuity test procedure as described in Datakinetics release
 * notes
 * Reset does not change blocked state
 * If RLC timeout 90 secs after a REL/RLC sent, the circuit is reset
 * 
 * *****************  Version 9  *****************
 * User: Junanto      Date: 17/07/01   Time: 13:47
 * Updated in $/xGate
 * Added watchdog to force release a call if RLC is not
 * received within 90 seconds
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 16/07/01   Time: 13:48
 * Updated in $/xGate
 * Fixed some bugs, normalized log output, ignore receipt of 
 * RLC after sending RLC
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 27/06/01   Time: 20:53
 * Updated in $/xGate
 * Added support for timeslot blocking
 * ISUP maintenance command can be issued from xGate
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 26/06/01   Time: 17:16
 * Updated in $/xGate
 * First version
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 26/06/01   Time: 10:21
 * Updated in $/xGate
 * temporary check-in
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 26/06/01   Time: 10:04
 * Updated in $/xGate
 * temporary check-in
 * 
 * *****************  Version 3  *****************
 * User: Bennylp      Date: 4/23/01    Time: 10:42p
 * Updated in $/xGate
 * Fixed std namespace conflicts.
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 18/04/01   Time: 11:51
 * Updated in $/ct15
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 15/04/01   Time: 20:48
 * Created in $/ct15
 * Created
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 * It implements ISUP signalling driver
 *----------------------------------------------------------------*/

#if !defined(CT2_ISUP_H__INCLUDED_)
#define CT2_ISUP_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "SignallingDriver.h"
//#include "cfg.h"
#include <SignallingDriver.h>
#include <cfg.h>


/** Forward declaration
 */
class IsupLinkAgent;
class DigitalNetworkInterface;
//class AculabInterface;
class IsupCall;


/** IsupSignalling: A singleton class which handles call control 
    signalling specific to Datakinetic ISUP driver.
 */
class IsupSignalling : public SignallingDriver
{
    friend class IsupCall;
    friend class IsupLinkAgent;
    friend class IsupManager;

public:
    /** Constructor/destructor
     */
    IsupSignalling(const char* name);
    virtual ~IsupSignalling();

    /** GetConfigTree: Returns a pointer to the configuration tree of
	interface sub commands
     */
    Cfg_Tree_Group* GetConfigTree();

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

    /** This function will be called by ServiceProviderManager to display 
	informations about service provider's status.
	This function is called with the system lock acquired, so descendants
	doesn't need to call Kernel::AcquireSystemLock() again.
     */
    virtual bool OnShowStatus(String & output);

    /** Send a formatted request to the ISUP controller
     */
    bool SendToController(Call* call,int type,int gid,int tslot,int msg,unsigned param_length,const void* param_data);

    /** Mark the call as wating for the receipt of RLC message. If no RLC message is
	received within a predefined time, the signalling will reset the circuit and
	alarm the operator
     */
    void InsertReleasedCall(IsupCall* call, int lastMsg);
    void RemoveReleasedCall(IsupCall* call);

    /** Returns various call parameters
     */
    unsigned GetNCI();	// nature of connection indicators
    unsigned GetFCI();	// forward call indicators
    unsigned GetBCI();	// backward call indicators

    /** Determine how timeslot is selected within an E1
     *  0 x x x = normal
     *  1 x x 0 = use odd first then even timeslots
     *  1 x x 1 = use even first the odd timeslots
     **/
    unsigned GetTslotSelection() {
	return _tslot_selection;
    }

private:
    enum { 
	MaxGroup = 128,
	MaxTimeslot = 32,
    };

    enum State {
	CState_Undefined,
	CState_Idle,
	CState_Incoming,
	CState_Outgoing,
    };

    // the state of each circuit
    struct CircuitState {
	State state;
	bool lblock;
	bool rblock;
	bool reachable;
    };

    // the state of link to the ISUP controller
    enum LinkState {
	LState_Undefined,
	LState_Up,
	LState_Down,
    };

    typedef Map<int,DigitalNetworkInterface*> MapInterface;
    typedef List<IsupCall*> ListCall;

    /** FindInterface: find a particular interface by group or by interface
     */
    DigitalNetworkInterface* FindInterface(int group);
    DigitalNetworkInterface* FindInterface(DigitalNetworkInterface* itf);

    /** Called when a link for a particular group becomes available
	or unavailable
     */
    void OnLinkOpen(int group);
    void OnLinkClose(int group);

    /** FindCall: find a particular call by combination of group+timeslot
     */
    IsupCall* FindCall(int group,int tslot);

    /** Register/deregister a call
     */
    void InsertCall(IsupCall* call);
    void RemoveCall(IsupCall* call);

    /** Called when a data is received from the ISUP controller and needs
	to be processed
     */
    void OnData(int linkId,void* data,unsigned length);
    void ProcessMessage(int type,int gid,int tslot,int msg,unsigned param_length,void* param_data);

    /** CircuitState list management
     */
    CircuitState* FindCircuitState(int gid,int tslot);

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** Terminal command handler
     */
    void DoTermCmd(Term_Session* session, bool no, Term_Cmd* cmd);

    /** Issue various ISUP maintenance command
     */
    void IsupMaintenance(Term_Session* session, bool no, Term_Cmd* cmd);

    /** Send supervision circuit request, action can be:
	  0: reset
	  1: abort reset
	  2: block
	  3: unblock
	  4: abort block
	  5: hardware block
	  6: hardware unblock
	  7: abort hardware block
     */
    bool SupervisionRequest(int group,int timeslot,int action);

    /** This method is called when the Kernel receives an instruction 
	to shutdown. ISUP signalling will send maintenance block message
	to the SS7 network
     */
    virtual void OnShutdown(bool cancel);

    /** Show configuration handler
     */
    //STATUS OnShowConfig(const Cfg_Tree_Entry* entry,String &output,const String &indent,int indent_size) const;
    bool OnShowConfig(const Cfg_Tree_Entry* entry,String &output,const String &indent,int indent_size) const;

    Cfg_Tree_Group* _cfgTree;
    String _description;
    String _controllerAddress;
    short _controllerPort;
    MapInterface _interfaces;
    std::list<int> delayed_open;
    time_t last_delayed_open;
    ListCall _calls,_releasedCalls;
    IsupLinkAgent* _agent;
    bool _enabled;
    CircuitState _states[MaxGroup][MaxTimeslot];
    LinkState _links[MaxGroup];

    // Automatically send backward proceeding in enbloc sending
    // after n seconds (0: disabled)
    unsigned _autoProceedingEnbloc;
    ListCall _autoProceeding;

    // Automatically block TS after n times consecutive failure
    // with the specified cause-codes
    unsigned short _faultyTSMaxFailure;
    unsigned short _faultyTSCounter[MaxGroup][MaxTimeslot];
    int _faultyTSCause;

    unsigned _nci;	// nature of connection indicators
    unsigned _fci;	// forward call indicators
    unsigned _bci;	// backward call indicators
    unsigned _uknat_fci;    // UK national forward call indicators
    unsigned _uknat_fcil;   // UK national forward call indicators (link-by-link)
    bool _use_cause_location;	// use location in cause code
    unsigned _tslot_selection;
};


/** IsupManager: the singleton entity that manages all ISUP signalling 
    driver instances
 */
class IsupManager : public ServiceProvider
{
public:
    // public destructor
    virtual ~IsupManager();

    /** Create a singleton instance of IsupManager
     */
    static IsupManager* Create();

    /** Returns the only instance of IsupManager. 
	The service manager must have been instantiated with Create() prior
	to calling this function.
    */
    static IsupManager* Instance();

    /** GetConfigTree: Returns a pointer to the configuration tree of
	interface sub commands
     */
    Cfg_Tree_Group* GetConfigTree();

    /** Terminal command callback
     */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

    /** Show configuration for the specified entry.
	This function will be called by the framework whenever it needs to display the
	specified entry. Previously this instance should tell the entry that it wishes to
	handle displaying the entry by calling entry->RegisterShowHandler().
     */
    virtual bool OnShowConfig(
	const Cfg_Tree_Entry* entry,
	String & output, 
	const String & indent, 
	int indent_size) const;

    /** Add/remove an ISUP driver instance from the list, called by Interface constructore/destructor
     */
    void InsertIsupSignalling(IsupSignalling* sig);
    void RemoveIsupSignalling(IsupSignalling* sig);

    /** Find an instance of ISUP driver by name
     */
    IsupSignalling* FindSignallingByName(const char* name);

private:
    typedef List<IsupSignalling*> ListDriver;

    enum Cmd_Id {
	Cmd_Isup,
    };

    Cfg_Tree_Group		*_cfgTree;
    ListDriver			_listDriver;

    static IsupManager		*_theIsupManager;

    // private constructor
    IsupManager();

    /** Terminal command handler
     */
    void DoCmd_Isup(Term_Session* session, bool no, Term_Cmd* cmd);

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** HandleEvents: It is called by the Service Provider Manager to let
	this object to poll its underlying library/driver
     */
    virtual void HandleEvents();

};

#endif // !defined(CT2_ISUP_H__INCLUDED_)
