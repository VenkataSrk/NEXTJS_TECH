/*----------------------------------------------------------------* 
 * $Archive: /xGate/PipeLine.h $
 * $Date: 24/06/01 20:08 $
 * $Revision: 11 $
 *
 * $History: PipeLine.h $
 * 
 * *****************  Version 11  *****************
 * User: Junanto      Date: 24/06/01   Time: 20:08
 * Updated in $/xGate
 * Changed AttachInterface to return bool
 * 
 * *****************  Version 10  *****************
 * User: Junanto      Date: 7/06/01    Time: 22:33
 * Updated in $/xGate
 * 
 * *****************  Version 9  *****************
 * User: Junanto      Date: 6/06/01    Time: 10:40
 * Updated in $/xGate
 * Removed private implementation of
 * "description" field
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 25/05/01   Time: 13:04
 * Updated in $/xGate
 * Added group and description parameters
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 4/05/01    Time: 18:46
 * Updated in $/xGate
 * Supports dynamic deletion
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 4/05/01    Time: 16:55
 * Updated in $/xGate
 * Changed RAP methods and a new command is 
 * added: [no] enable
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 2/05/01    Time: 14:50
 * Updated in $/xGate
 * Asynchronous SetupInd invocation
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 2/05/01    Time: 12:24
 * Updated in $/xGate
 * Temporary check in to update OnShowInterface
 * 
 * *****************  Version 3  *****************
 * User: Junanto      Date: 30/04/01   Time: 18:49
 * Updated in $/xGate
 * linked with media manager
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 30/04/01   Time: 10:57
 * Updated in $/xGate
 * Pipeline signalling and switching done
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 30/04/01   Time: 10:16
 * Created in $/xGate
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 29/04/01   Time: 17:33
 * Created in $/xGate
 * Added pipeline signalling and switching
 * Pipeline signalling tested
 * 
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 * A pipeline is a virtual channel which allows an application
 * to "make a call" to another application inside the same 
 * machine without requiring any physical device.
 * From application point of view, a pipeline is considered as
 * a normal "line".
 *----------------------------------------------------------------*/

#if !defined(CT2_PIPELINE_H__INCLUDED_)
#define CT2_PIPELINE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "callmanager.h"
#include "SignallingDriver.h"


/** PipeInterface:
 */
class PipeInterface : public Interface
{
    friend class PipeSignalling;

public:
    /** Constructor/destructor
     */
    PipeInterface(const char *fullName, int maxcall, bool dynamic, CTask* owner);
    virtual ~PipeInterface();

    CTask* GetOwner() {
	return _owner;
    }

    void SetOwner(CTask* task) {
	_owner = task;
    }

    /** This interface support dynamic deletion only when there is no call
     */
    virtual bool SupportDynamicDeletion();

private:
    //* Overloaded implementation for pipe
    /** AllocateMedia: Called by CallManager to allocate a media in this itf.
	If a free media can be allocated, it is returned synchronously after calling
	MediaAllocatedInd method of the Call object (unless if NULL is specified 
	in call argument). It returns false when there is no free media available
     */
    virtual bool AllocateMedia(Call *call,Media *&media,SelectionStrategy sel,CallDirection dir,CallParameter *cparam);

    /** AllocateMedia: A variant of AllocateMedia that specify a particular media
	to be allocated. It returns false when there is such media is not available
     */
    virtual bool AllocateMedia(Call *call,Media *&media,MediaId id,CallDirection dir,CallParameter *cparam);

    /** ReleaseMedia: Called by Call object when the call becomes idle. The underlying
	channel is then ready to use for another call.
     */
    virtual void ReleaseMedia(Media *media,CallDirection dir,CallParameter *cparam,CallCause cc);

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
	const Cfg_Tree_Entry *entry,
	String & output, 
	const String & indent, 
	int indent_size) const;

    /** Terminal command callback
     */
    void DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd);

    virtual void OnShowInterface ( Term_Session * session );

    /** Called if the interface is about to be dynamically destroyed
     */
    virtual void DestroyInterface();

private:
    /** Allocate unique identifier
     */
    unsigned AllocateMediaId();

    /** Free a media identifier
     */
    void FreeMediaId(unsigned id);

    typedef List<unsigned>	ListUnsigned;
    ListUnsigned	_listId;
    unsigned		_nextId;
    unsigned		_maxChannels;
    unsigned		_usedChannels;

    bool		_isDynamic;
    CTask*		_owner;

    enum CmdId {
	Cmd_MaxChannels,
	Cmd_UseRLS,
	Cmd_Enable,
	Cmd_Description,
	Cmd_Group,
        Cmd_ResourceType //HMP
    };
};


/** PipeSignalling: A singleton class which handles call control 
    signalling specific to pipelines.
 */
class PipeSignalling : public SignallingDriver
{
public:
    /** Create a singleton instance of PipeSignalling
     */
    static PipeSignalling *Create();

    /** Instance: Returns a pointer to this singleton instance, if necessary
	create one instance first before returning a pointer to it
     */
    static PipeSignalling *Instance();

    /** SetupReq: Initiate a new outgoing call in a specific Interface
	using the specified call parameter
     */
    virtual Call *SetupReq(Interface *itf,SelectionStrategy selection,CallParameter *cparam);

    /** CreateDefaultCallParameter: Returns a CallParameter structure containing
	default values for this signalling protocol
     */
    virtual CallParameter *CreateDefaultCallParameter();

    /** AttachInterface: A new Interface object is attached to this 
	SignallingDriver object
     */
    virtual bool AttachInterface(Interface *itf);

    /** DetachInterface: Remove association between a Interface object and this 
	SignallingDriver object
     */
    virtual void DetachInterface(Interface *itf);

    /** DisableInterface: an interface has been disabled, the signalling
	driver should take the necessary action if it can
     */
    virtual bool DisableInterface(Interface *itf);

    /** EnableInterface: an interface has been enabled, the signalling
	driver should take the necessary action if it can
     */
    virtual bool EnableInterface(Interface *itf);

    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** HandleEvents: It is called by Service Provider Manager
     */
    virtual void HandleEvents();

    /** Schedule a call to be initiated at the next HandleEvents
     */
    void ScheduleNewCall(Call *call);

    /** Cancel a call from a scheduling list
     */
    void CancelNewCall(Call *call);

private:
    static PipeSignalling *thePipeSignalling;

    /** Private constructor
     */
    PipeSignalling();

    typedef List<PipeInterface *> ListInterface;
    ListInterface _interface;

    typedef List<Call*> ListCall;
    ListCall _newCall;
};


/** InterfaceManager: Dynamic creation.deletion of pipes are handled by 
    this object
 */
class PipeManager : public InterfaceManager
{
public:
    PipeManager();
    virtual ~PipeManager();

    /** Create a singleton instance of PipeManager 
     */
    static PipeManager *Create();

    /** Destroy the singleton instance of PipeManager 
     */
    static void Destroy();

    /** Instance: Returns a pointer to this singleton instance, if necessary
	create one instance first before returning a pointer to it
     */
    static PipeManager *Instance();

    virtual bool CanCreateInterface(const char *name);

    virtual Interface *CreateInterface(const char *name);

    virtual Interface *CreateInterface(const char *name, int maxcall, bool dynamic, CTask* owner);

private:
    static PipeManager* _thePipeManager;
    class DynamicPipeManager* _dynamicPipeMgr;
};

#endif // !defined(CT2_PIPELINE_H__INCLUDED_)
