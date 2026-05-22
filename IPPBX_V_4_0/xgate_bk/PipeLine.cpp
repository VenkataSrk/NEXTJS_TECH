/*----------------------------------------------------------------* 
 * $Archive: /xGate/PipeLine.cpp $
 * $Date: 4/09/03 11:03a $
 * $Revision: 41 $
 *
 * $History: PipeLine.cpp $
 * 
 * *****************  Version 41  *****************
 * User: Bennylp      Date: 4/09/03    Time: 11:03a
 * Updated in $/xGate
 * Added interface type for interface (for SNMP purpose)
 * 
 * *****************  Version 40  *****************
 * User: Junanto      Date: 1/07/02    Time: 12:39
 * Updated in $/xGate
 * removed unnecssary warning in pipes during shutdown
 * 
 * *****************  Version 39  *****************
 * User: Bennylp      Date: 7/30/01    Time: 11:45a
 * Updated in $/xGate
 * Changed SetMediaCount() to SetCapacity().
 * 
 * *****************  Version 38  *****************
 * User: Bennylp      Date: 7/19/01    Time: 2:17p
 * Updated in $/xGate
 * Changes to MediaManager functions to support rtp/aud shared stats.
 * 
 * *****************  Version 37  *****************
 * User: Bennylp      Date: 7/14/01    Time: 5:23p
 * Updated in $/xGate
 * Framework for signalling driver configuration in SigDrvManager.
 * 
 * *****************  Version 36  *****************
 * User: Junanto      Date: 10/07/01   Time: 19:06
 * Updated in $/xGate
 * Changed interface of Call class constructor
 * 
 * *****************  Version 35  *****************
 * User: Bennylp      Date: 6/30/01    Time: 8:13p
 * Updated in $/xGate
 * Changed StartListen() to return bool, to capture error.
 * 
 * *****************  Version 34  *****************
 * User: Junanto      Date: 27/06/01   Time: 17:41
 * Updated in $/xGate
 * Changed media naming convention
 * 
 * *****************  Version 33  *****************
 * User: Junanto      Date: 25/06/01   Time: 16:41
 * Updated in $/xGate
 * Group command is only effective without
 * resource locator
 * 
 * *****************  Version 32  *****************
 * User: Junanto      Date: 24/06/01   Time: 20:08
 * Updated in $/xGate
 * Changed AttachInterface to return bool
 * 
 * *****************  Version 31  *****************
 * User: Junanto      Date: 19/06/01   Time: 21:28
 * Updated in $/xGate
 * SETUP_ACK in Pipe is only sent during overlap sending
 * 
 * *****************  Version 30  *****************
 * User: Junanto      Date: 19/06/01   Time: 19:35
 * Updated in $/xGate
 * Fixed some bug in switching
 * 
 * *****************  Version 29  *****************
 * User: Junanto      Date: 7/06/01    Time: 22:33
 * Updated in $/xGate
 * 
 * *****************  Version 28  *****************
 * User: Junanto      Date: 6/06/01    Time: 10:40
 * Updated in $/xGate
 * Removed private implementation of
 * "description" field
 * 
 * *****************  Version 27  *****************
 * User: Junanto      Date: 3/06/01    Time: 21:03
 * Updated in $/xGate
 * Fixed bug when an outgoing pipe is disconnected 
 * before initiating a SETUP on the incoming peer
 * 
 * *****************  Version 26  *****************
 * User: Bennylp      Date: 5/26/01    Time: 8:04p
 * Updated in $/xGate
 * Record each call to AcquireSystemLock in DEBUG mode.
 * 
 * *****************  Version 25  *****************
 * User: Junanto      Date: 25/05/01   Time: 13:04
 * Updated in $/xGate
 * Added group and description parameters
 * 
 * *****************  Version 24  *****************
 * User: Junanto      Date: 23/05/01   Time: 20:52
 * Updated in $/xGate
 * Fixed bug in pipeline switching
 * 
 * *****************  Version 23  *****************
 * User: Junanto      Date: 21/05/01   Time: 15:54
 * Updated in $/xGate
 * Added call holding and connected duration (in secs)
 * 
 * *****************  Version 22  *****************
 * User: Junanto      Date: 11/05/01   Time: 11:28
 * Updated in $/xGate
 * Fixed a GPF buf at GetTransmitter()
 * 
 * *****************  Version 21  *****************
 * User: Junanto      Date: 10/05/01   Time: 12:40
 * Updated in $/xGate
 * Replaced "\r\n" sequence to "\n". Telnet translates
 * an orphaned "\n" into "\r\n" during output
 * 
 * *****************  Version 20  *****************
 * User: Iwanj        Date: 5/09/01    Time: 10:27a
 * Updated in $/xGate
 * Changed RAP enable/disable
 * 
 * *****************  Version 19  *****************
 * User: Junanto      Date: 9/05/01    Time: 10:07
 * Updated in $/xGate
 * Fixed GPF caused by early delete in Switching
 * 
 * *****************  Version 18  *****************
 * User: Junanto      Date: 8/05/01    Time: 16:48
 * Updated in $/xGate
 * Fixed memory leak
 * 
 * *****************  Version 17  *****************
 * User: Junanto      Date: 7/05/01    Time: 19:03
 * Updated in $/xGate
 * 
 * *****************  Version 16  *****************
 * User: Junanto      Date: 7/05/01    Time: 18:24
 * Updated in $/xGate
 * fixed bug when interface is newly created and 
 * immediately used
 * 
 * *****************  Version 15  *****************
 * User: Junanto      Date: 7/05/01    Time: 15:59
 * Updated in $/xGate
 * Fixed bug in [no] enable
 * 
 * *****************  Version 14  *****************
 * User: Junanto      Date: 4/05/01    Time: 18:46
 * Updated in $/xGate
 * Supports dynamic deletion
 * 
 * *****************  Version 13  *****************
 * User: Junanto      Date: 4/05/01    Time: 16:55
 * Updated in $/xGate
 * Changed RAP methods and a new command is 
 * added: [no] enable
 * 
 * *****************  Version 12  *****************
 * User: Junanto      Date: 2/05/01    Time: 20:12
 * Updated in $/xGate
 * Some bug fixes
 * 
 * *****************  Version 11  *****************
 * User: Junanto      Date: 2/05/01    Time: 15:24
 * Updated in $/xGate
 * 
 * *****************  Version 10  *****************
 * User: Junanto      Date: 2/05/01    Time: 15:23
 * Updated in $/xGate
 * 
 * *****************  Version 9  *****************
 * User: Junanto      Date: 2/05/01    Time: 15:16
 * Updated in $/xGate
 * Changed user interface commands
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 2/05/01    Time: 14:50
 * Updated in $/xGate
 * Asynchronous SetupInd invocation
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 2/05/01    Time: 12:24
 * Updated in $/xGate
 * Temporary check in to update OnShowInterface
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 1/05/01    Time: 13:49
 * Updated in $/xGate
 * add NEWLINE in output
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 30/04/01   Time: 21:21
 * Updated in $/xGate
 * Fixed bug in pipeline
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 30/04/01   Time: 19:12
 * Updated in $/xGate
 * Corrected bug in media allocation
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
 * A pipeline is a channel which allows an application
 * to "make a call" to another application inside the same 
 * machine without requiring any physical device.
 * From application point of view, a pipeline is considered as
 * a normal "line".
 *----------------------------------------------------------------*/

//#include "stdafx.h"
#include "PipeLine.h"
#include "kernel.h"
#include "term.h"
#include "task.h"
#include "log.h"

class PipeCall;

//-----------------------------------------------------------------
static char __modname__[] = "Pipe";
#define THISMODULE	    __modname__
#define NEWLINE		    "\n"


//-----------------------------------------------------------------
/** Singleton instances
 */
PipeSignalling* PipeSignalling::thePipeSignalling = NULL;
PipeManager* PipeManager::_thePipeManager = NULL;



/*----------------------------------------------------------------* 
 * Implementation of PipeSwitch
 *----------------------------------------------------------------*/

/** PipeSwitch: This object implements switching capability of pipeline
 */
class PipeSwitch : public SwitchDevice
{
    friend class PipeCall;
    friend class PipeMedia;
    friend class PipeSignalling;

public:

    /** PipeSwitch's constructor
     */
    PipeSwitch(const char *name) : SwitchDevice(name) {
	_peer = NULL;
    }

    /** PipeSwitch's destructor
     */
    virtual ~PipeSwitch() {
	if (_peer) {
	    _peer->Unlisten();
	    _peer->_peer = NULL;
	    _peer = NULL;
	}
#ifdef _DEBUG
	CLog::Detail(THISMODULE,"%s destroyed",GetName());
#endif
    }

    /** The framework will query the device to find out which 
	switching fabric is supported by this device to exchange TDM
	data with another device.
     */
    virtual SwitchFabric GetCompatibleFabric(SwitchDevice *dev) {
	assert(false);
	return SF_Invalid;
    }

    /** This function is called when the framework decides that it is
	the first time the specific device transmits to a particular
	fabric.
     */
    virtual SwitchAddress *AllocateAddress(SwitchFabric fabric) {
	assert(false);
	return NULL;
    }

    /** It returns the address used for switching in the specified fabric
     */
    virtual SwitchAddress *GetAddress(SwitchFabric fabric) {
	assert(false);
	return NULL;
    }

    /** Instruct this device to listen from another device
     */
    void Listen(SwitchDevice *source) {

	// if we already listen to the same source then ignore the request
	if (_transmitter && (source == _transmitter->GetTransmitter()))
	    return;

	// disconnect any previous connection
	Unlisten();

	// add this object to the list of listener
	// check device validity
	source->AddListener(this);
	_transmitter = source;

	SwitchDevice *listener;
	listener = _peer->IterGetFirstListener();
	while (listener) {
	    listener->StartListen(source->GetTransmitter());
	    listener = _peer->IterGetNextListener();
	}
    }

    /** Instruct this device to stop listening from another device if any
     */
    void Unlisten() {
	if (_transmitter) {
	    _transmitter->RemoveListener(this);
	    assert(_peer);
	    SwitchDevice *dev;
	    if (_peer && ((dev = _transmitter->GetTransmitter()) != NULL)) {
		SwitchDevice *listener = _peer->IterGetFirstListener();
		while (listener) {
		    listener->StopListen(dev);
		    listener = _peer->IterGetNextListener();
		}
	    }
	    _transmitter = NULL;
	}
    }

    /** Returns the physical transmitter switch device.
     */
    virtual SwitchDevice *GetTransmitter() {
	return _peer ? ((_peer->_transmitter) ? _peer->_transmitter->GetTransmitter() : NULL) : NULL;
    }

    /** It instructs the device to start transmitting to a certain location 
	using the specified switch fabric.
     */
    bool StartListen(SwitchDevice *device) {
	if (device) {
	    SwitchDevice *listener = _peer->IterGetFirstListener();
	    while (listener) {
		listener->StartListen(device->GetTransmitter());
		listener = _peer->IterGetNextListener();
	    }
	}
	return true;
    }

    /** It instructs the device to stop listening from a certain location 
	using the specified switch fabric.
     */
    void StopListen(SwitchDevice *device) {
	SwitchDevice *listener = _peer->IterGetFirstListener();
	while (listener) {
	    listener->StopListen(device->GetTransmitter());
	    listener = _peer->IterGetNextListener();
	}
    }

    void SetPeer(PipeSwitch *peer) {
	_peer = peer;
    }

private:
    PipeSwitch *_peer;
};



/*----------------------------------------------------------------* 
 * Implementation of PipeMedia
 *----------------------------------------------------------------*/

/** PipeMedia: A pipeline is composed of two medias and two calls ;
    one is outgoing and the other one is incoming
 */
class PipeMedia : public Media
{
    friend class PipeCall;
    friend class PipeSignalling;
    friend class PipeInterface;

public:

    /** PipeMedia's constructor
     */
    PipeMedia(const char *name,MediaId id,PipeInterface *itf) : Media(name,id,itf,new PipeSwitch(name)) {
	_peer = NULL;
	_useRAP = false;
    }

    /** PipeMedia's destructor
     */
    virtual ~PipeMedia() {
    }

private:
    /** Set the media used at the other end
     */
    void SetPeer(PipeMedia *peer) {
	_peer = peer;
    }

    PipeMedia		*_peer;
    bool		_useRAP;
};


/*----------------------------------------------------------------* 
 * Declaration of DynamicPipeManager
 *----------------------------------------------------------------*/

/** DynamicPipeManager class
 */
class DynamicPipeManager : public ServiceProvider
{
public:
    DynamicPipeManager();
    ~DynamicPipeManager();

    static DynamicPipeManager* Instance() {
	return _instance;
    }

    void OnSetupInd(CTask* owner, PipeCall* call);
    void RemoveWaitingCall(PipeCall* call);

    int DynamicPipeCreate(CTask* task, const char* name, const char* group, int maxcall);

    int DynamicPipeDelete(CTask* task, const char* name);

    void DynamicPipeWaitCall(CTask* task, int* retval, int timeout);

    int DynamicPipeIsExist(CTask* task, const char* name);

private:
    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    virtual bool Load();

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    virtual bool Unload();

    /** This function will be called by the service framework once the 
	configuration file has been fully read.
     */
    virtual bool CommitConfiguration();

    /** This function will be called whenever a new task is created.
     */
    virtual bool AttachTask(CTask *);

    /** This function will be called whenever a task is about to be deleted.
     */
    virtual void DetachTask(CTask *);

    /** This function will be called if this instance has a pending job for
	the task.
     */
    virtual bool CancelJob(CTask *);

    /** HandleEvents: Descendent shall implement this abstract method.
	It is called by Service Provider Manager
     */
    virtual void HandleEvents();

private:
    class DynamicPipeAPI* _api;
    static DynamicPipeManager* _instance;
    int _tlsId;

    typedef List<PipeInterface*> ListPipe;
    ListPipe _deleted_list;

    typedef List<CTask*> ListTask;
    ListTask _blocked_tasks;

    time_t _last_polled;
};


DynamicPipeManager* DynamicPipeManager::_instance = 0;

/*----------------------------------------------------------------* 
 * Implementation of PipeCall
 *----------------------------------------------------------------*/

/** PipeCall: Specific implementation of Call for pipeline signalling driver
 */
class PipeCall : public Call
{
    friend class PipeSignalling;
    friend class DynamicPipeManager;

public:
    /** Default constructor and destructor
     */
    PipeCall(CallDirection dir,const char* itfname) : Call(dir,itfname) {
	_peer = 0;
	_scheduled = false;
	_task = 0;
    }

    virtual ~PipeCall() {
	DynamicPipeManager::Instance()->RemoveWaitingCall(this);
    }

    /** Requests: these methods need to be overridden by each Signalling 
	Driver to implement specific behaviour of signalling protocol
     */
    virtual bool DeviceOverlapReq(const char *additionalDigits,bool complete) {
	assert(_peer);
	_peer->OverlapInd(additionalDigits,complete);
	return true;
    }

    virtual bool DeviceProceedingReq(CallProgress progress, CallExtraInfo* exinfo) {
	assert(_peer);
	_peer->ProceedingInd(progress, exinfo);
	return true;
    }

    virtual bool DeviceProgressReq(CallProgress progress, CallExtraInfo* exinfo) {
	assert(_peer);
	_peer->ProgressInd(progress, exinfo);
	return true;
    }

    virtual bool DeviceAlertingReq(CallProgress progress, CallExtraInfo* exinfo) {
	assert(_peer);
	_peer->AlertingInd(progress, exinfo);
	return true;
    }

    virtual bool DeviceSendInfo(CallExtraInfo* exinfo) {
        return true;
        }

    virtual bool DeviceConnectReq(CallExtraInfo* exinfo) {
	assert(_peer);
	_peer->ConnectInd(exinfo);
	ConnectCnf();
	return true;
    }

    virtual bool DeviceSuspendReq() {
	assert(_peer);
	_peer->SuspendInd();
	return true;
    }

    virtual bool DeviceResumeReq() {
	assert(_peer);
	_peer->ResumeInd();
	return true;
    }

    virtual bool DeviceNotifyECTReq(tect_indicator indicator) {
	assert(_peer);
	_peer->NotifyECTInd(indicator);
	return true;
    }

    virtual bool DeviceSendInfo(CallCause cause,CallProgress progress) {
                return true;
        }

    virtual bool DeviceDisconnectReq(CallCause cause,CallProgress progress) {
	assert(_peer);

	// this call has been aborted before even attempted
	if (_scheduled) {
	    PipeSignalling::Instance()->CancelNewCall(_peer);
	    _scheduled = false;
	    _peer->DetachMedia(0,0);
	    delete _peer;
	    SetPeer(NULL);
	}

	switch (GetState()) {
	case CS_Disconnected:
	case CS_Releasing:
	    if (_peer && (_peer->GetState() != CS_Null)) _peer->RelComInd(cause);
	    RelComInd(cause);
	    break;

	default:
	    if (_peer) _peer->DisconnectInd(cause,progress);
	    else RelComInd(cause);
	}
	return true;
    }

    virtual void SetupInd(CallParameter &parameter) {
	Media* med = GetMedia();
	PipeInterface *itf = med ? (PipeInterface*) med->GetInterface() : 0;

	Call::SetupInd(parameter);

	if (itf && itf->IsDynamic()) {
	    CTask* owner = itf->GetOwner();
	if (owner) {
	    DynamicPipeManager::Instance()->OnSetupInd(owner, this);
	    } else {
		DisconnectReq(CC_DestinationOutOfOrder, CP_NoIndication);
	}
    }
    }

private:
    /** Set the call used at the other end
     */
    void SetPeer(PipeCall *peer) {
	_peer = peer;
    }

    PipeCall*	_peer;
    bool	_scheduled;
    CTask*	_task;
};


/*----------------------------------------------------------------* 
 * Implementation of PipeInterface
 *----------------------------------------------------------------*/

/** PipeInterface's constructor
 */
PipeInterface::PipeInterface(const char *fullName, int maxcall, bool dynamic, CTask* owner) :
    Interface(ITF_PIPE, fullName, false, Normal_Priority, dynamic)
{
    _nextId = 0;
    _signalling = NULL;
    _maxChannels = maxcall*2;
    _usedChannels = 0;
    _isDynamic = dynamic;
    _owner = owner;
    SetSignallingDriver(PipeSignalling::Instance());
    Start();
    SetMediaCount(_maxChannels);
    Enable();
}

/** PipeInterface's destructor
 */
PipeInterface::~PipeInterface()
{
    PipeSignalling::Instance()->DetachInterface(this);
}

/** Allocate unique identifier
 */
unsigned PipeInterface::AllocateMediaId()
{
    if (_listId.empty()) {
	_listId.push_back(_nextId++);
    }
    unsigned id = _listId.front();
    _listId.pop_front();
    return id;
}

/** Free a media identifier
 */
void PipeInterface::FreeMediaId(unsigned id)
{
    _listId.push_back(id);
}

/** AllocateMedia: Called by CallManager to allocate a media in this itf.
    If a free media can be allocated, it is returned synchronously after calling
    MediaAllocatedInd method of the Call object (unless if NULL is specified 
    in call argument). It returns false when there is no free media available
 */
bool PipeInterface::AllocateMedia(Call *call,Media *&media,SelectionStrategy sel,CallDirection dir,CallParameter *cparam)
{
    if (_enabled && (_usedChannels < _maxChannels)) {
	unsigned id = AllocateMediaId();
	char name[32];
	sprintf(name,"%s:%u",Name().c_str(),id);
	PipeMedia *pm = new PipeMedia(name,id,this);
	if (call) call->MediaAvailableInd(pm);
	DecMediaCount(pm, dir,cparam);
	_usedChannels++;
	media = pm;
	return true;
    } else {
	media = NULL;
	return false;
    }
}

/** AllocateMedia: A variant of AllocateMedia that specify a particular media
    to be allocated. It returns false when there is such media is not available
 */
bool PipeInterface::AllocateMedia(Call *call,Media *&media,MediaId id,CallDirection dir,CallParameter *cparam)
{
    return AllocateMedia(call,media,SS_LIFO,dir,cparam);
}

/** ReleaseMedia: Called by Call object when the call becomes idle. The underlying
    channel is then ready to use for another call.
 */
void PipeInterface::ReleaseMedia(Media *media,CallDirection dir,CallParameter *cparam,CallCause cc)
{
    _usedChannels--;
    IncMediaCount(media,dir,cparam,cc);
    delete media;
}

/** Load:
 */
bool PipeInterface::Load()
{
    // Register configuration commands
    // timeslot
    Term_Cmd *cmd = new Term_Cmd(this, Cmd_MaxChannels, "max-channels", "Specify the maximum number of channels (0=unlimited)", PrivilegeLevelAdmin, TermModeConfig, false);
    cmd->Add(new Term_Cmd_Arg("arg1", CVariant(0), CVariantValidator(CVariant(0), "number of channels")));
    RegisterTermCmd(cmd,_cfgTree);

    cmd = new Term_Cmd(this, Cmd_UseRLS, "resource-locator", "Use Resource Locator", PrivilegeLevelAdmin, TermModeConfig, true);
    RegisterTermCmd(cmd,_cfgTree);

    /* Cmd_ResourceType
     */
        Interface::AddResourceTypeCmd(this, Cmd_ResourceType, _cfgTree);

    cmd = new Term_Cmd(this, Cmd_Description, "description", "Gives destription of this interface", PrivilegeLevelAdmin, TermModeConfig, true);
    cmd->Add(new Term_Cmd_Arg("arg1", "", CVariantValidator("", "description"),TermArgOptional));
    RegisterTermCmd(cmd,_cfgTree);

    cmd = new Term_Cmd(this, Cmd_Group, "group", "Assign this interface to a particular group", PrivilegeLevelAdmin, TermModeConfig, true);
    cmd->Add(new Term_Cmd_Arg("arg1", "", CVariantValidator("", "group name"),TermArgOptional));
    RegisterTermCmd(cmd,_cfgTree);

    cmd = new Term_Cmd(this, Cmd_Enable, "enable", "Enable the interface", PrivilegeLevelAdmin, TermModeConfig, true);
    RegisterTermCmd(cmd,_cfgTree);

    _cfgTree->RegisterShowHandler(this);
    return true;
}

void PipeInterface::OnShowInterface ( Term_Session * session )
{
    session->Send(SHOW_INTERFACE_INDENT);
    if (RAPEnabled()) session->Send("RAP enabled" NEWLINE);
    else session->Send("RAP disabled" NEWLINE);

    session->Sendf(SHOW_INTERFACE_INDENT "Channels usage: %u/%u" NEWLINE,_usedChannels,_maxChannels);
}

/** Unload:
 */
bool PipeInterface::Unload()
{
    return true;
}

/** Start:
 */
bool PipeInterface::Start()
{
    return Interface::Start();
}

/** Stop:
 */
bool PipeInterface::Stop()
{
    return Interface::Stop();
}

/** Show configuration for the specified entry.
    This function will be called by the framework whenever it needs to display the
    specified entry. Previously this instance should tell the entry that it wishes to
    handle displaying the entry by calling entry->RegisterShowHandler().
 */
//STATUS PipeInterface::OnShowConfig(const Cfg_Tree_Entry *entry,String & output,const String & indent,int indent_size) const
bool PipeInterface::OnShowConfig(const Cfg_Tree_Entry *entry,String & output,const String & indent,int indent_size) const
{
    char str[64];
    PipeInterface *This = (PipeInterface *)this;

    if (GetDescription() != "") output += indent + "description \"" + GetDescription() + "\"" + NEWLINE;

    if (!This->RAPEnabled() && strlen(This->GetGroupName())) 
	output += indent + "group \"" + This->GetGroupName() + "\"" + NEWLINE;

    sprintf(str,"max-channels %d",_maxChannels);
    output += indent + str + NEWLINE;

    sprintf(str,"%sresource-locator",This->RAPEnabled() ? "" : "no ");
    output += indent + str + NEWLINE;

    sprintf(str,"resource-type %s",GetResourceTypeStr().c_str());
    output += indent + str + NEWLINE;

    sprintf(str,"%senable",This->IsActive() ? "" : "no ");
    output += indent + str + NEWLINE;

    return OK;
}

/** Terminal command callback
 */
void PipeInterface::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    ACQUIRE_SYSTEM_LOCK();

    CVariant v1;
    unsigned newval;

    switch (cmd->GetId()) {

    // max-channels <n>
    case Cmd_MaxChannels:
	cmd->GetArg((unsigned)0)->GetValue(v1);
	newval = v1.AsInt();
	if (newval == 0) {
	    session->SendError("Invalid value 0" NEWLINE);
	    break;
	}
	if (newval < _usedChannels) {
	    session->Sendf("Maximum number of channels is set to %d" NEWLINE,_usedChannels);
	    newval = _usedChannels;
	}
	_maxChannels = newval;
	SetCapacity(_maxChannels);
	break;

    // [no] resource-locator
    case Cmd_UseRLS:
	if (no == RAPEnabled()) {
	    // check if no calls are active
	    if (_usedChannels) {
		session->SendError("Values can't be changed while there are active call(s)" NEWLINE);
		break;
	    }
	}
	if (no) DisableRAP();
	else EnableRAP();
	break;

    // resource-type
        case Cmd_ResourceType:
             cmd->GetArg((size_t)0)->GetValue(v1);
             SetResourceTypeStr(v1.AsString());
        break;

    // [no] enable
    case Cmd_Enable:
	if (no) Disable();
	else Enable();
	break;

    // [no] description <name>
    case Cmd_Description:
	if (no) SetDescription("");
	else {
	    cmd->GetArg((unsigned)0)->GetValue(v1);
	    SetDescription(v1.AsString());
	}
	break;

    // [no] group <name>
    case Cmd_Group:
	if (no) SetGroupName("");
	else {
	    if (!RAPEnabled()) {
		cmd->GetArg((unsigned)0)->GetValue(v1);
		SetGroupName(v1.AsString());
	    } else {
		session->SendError("This command may only be used without resource locator" NEWLINE);
	    }
	}
	break;
    }

    RELEASE_SYSTEM_LOCK();
}


/** This interface support dynamic deletion only when there is no call
 */
bool PipeInterface::SupportDynamicDeletion() 
{
    return _usedChannels == 0;
}

/** Called if the interface which support dynamic deletion is about
    to be destroyed
 */
void PipeInterface::DestroyInterface()
{
    delete this;
}


/*----------------------------------------------------------------* 
 * Implementation of PipeSignalling
 *----------------------------------------------------------------*/

/** Private constructor
 */
PipeSignalling::PipeSignalling() : SignallingDriver(SigType_Pipeline,"Pipe","Pipe",true)
{
}

/** Create a singleton instance of PipeSignalling
 */
PipeSignalling *PipeSignalling::Create()
{
    assert(thePipeSignalling == 0);
    return Instance();
}

/** Instance: Returns a pointer to this singleton instance, if necessary
    create one instance first before returning a pointer to it
 */
PipeSignalling *PipeSignalling::Instance() {
    if (thePipeSignalling == 0) thePipeSignalling = new PipeSignalling;
    return thePipeSignalling;
}

/** SetupReq: Initiate a new outgoing call in a specific Interface
    using the specified call parameter
 */
Call *PipeSignalling::SetupReq(Interface *itf,SelectionStrategy selection,CallParameter *cparam)
{
    // allocate an outgoing call objects
    PipeCall *callOut = new PipeCall(CD_Outgoing,itf->Name().c_str());

    // allocate two media objects : one incoming and one outgoing
    // first, for outgoing call
    Media *mediaOut;
    if (!itf->AllocateMedia(NULL,mediaOut,SS_Uniform,CD_Outgoing,cparam)) {
	callOut->SetCause(CC_NoCircuitAvailable);
	return callOut;
    }
    callOut->MediaAvailableInd(mediaOut);

    // and then for incoming call
    Media *mediaIn;
    if (!itf->AllocateMedia(NULL,mediaIn,SS_Uniform,CD_Incoming,cparam)) {
	callOut->SetCause(CC_NoCircuitAvailable);
	return callOut;
    }

    // allocate an incoming call objects
    PipeCall *callIn = new PipeCall(CD_Incoming,itf->Name().c_str());
    callIn->MediaAvailableInd(mediaIn);

    callOut->_callParam = callIn->_callParam = *cparam;

    // associate signalling
    callOut->SetPeer(callIn);
    callIn->SetPeer(callOut);

    // associate switching
    ((PipeSwitch*)(callIn->GetMedia()->GetSwitchDevice()))->SetPeer((PipeSwitch*)callOut->GetMedia()->GetSwitchDevice());
    ((PipeSwitch*)(callOut->GetMedia()->GetSwitchDevice()))->SetPeer((PipeSwitch*)callIn->GetMedia()->GetSwitchDevice());

    // schedule this call to be initiated by HandleEvents
    callOut->_scheduled = true;
    ScheduleNewCall(callIn);

    //*** not here *** callOut->SetupCnf();
    return callOut;
}

/** CreateDefaultCallParameter: Returns a CallParameter structure containing
    default values for this signalling protocol
 */
CallParameter *PipeSignalling::CreateDefaultCallParameter()
{
    return new CallParameter;
}

/** AttachInterface: A new Interface object is attached to this 
    SignallingDriver object
 */
bool PipeSignalling::AttachInterface(Interface *itf)
{
    _interface.push_back((PipeInterface*)itf);
    return true;
}

/** DetachInterface: Remove association between a Interface object and this 
    SignallingDriver object
 */
void PipeSignalling::DetachInterface(Interface *itf)
{
    _interface.remove((PipeInterface*)itf);
}

/** HandleEvents: It is called by Service Provider Manager
 */
void PipeSignalling::HandleEvents()
{
    for (ListCall::iterator it=_newCall.begin();it!=_newCall.end();) {
	PipeCall *icall = (PipeCall*)*it;
	PipeCall *ocall = icall->_peer;
	it = _newCall.erase(it);
	// if (!ocall->GetCallParameter()->complete) ocall->SetupCnf();
	ocall->SetupCnf();
	ocall->_scheduled = false;
	icall->SetupInd(*icall->GetCallParameter());
    }
}

/** Schedule a call to be initiated at the next HandleEvents
 */
void PipeSignalling::ScheduleNewCall(Call *call)
{
    _newCall.push_back(call);
}

/** Cancel a call from a scheduling list
 */
void PipeSignalling::CancelNewCall(Call *call)
{
    _newCall.remove(call);
}

/** DisableInterface: an interface has been disabled, the signalling
    driver should take the necessary action if it can
 */
bool PipeSignalling::DisableInterface(Interface *itf)
{
    return true;
}

/** EnableInterface: an interface has been enabled, the signalling
    driver should take the necessary action if it can
 */
bool PipeSignalling::EnableInterface(Interface *itf)
{
    return true;
}

/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
 */
bool PipeSignalling::Load()
{
    PipeManager::Create();
    return true;
}

/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
 */
bool PipeSignalling::Unload()
{
    PipeManager::Destroy();
    for (ListInterface::iterator it=_interface.begin();it!=_interface.end();) {
	PipeInterface *pipe = *it++;
	delete pipe;
    }
    return true;
}



/*----------------------------------------------------------------* 
 * Implementation of DynamicPipeXXX API
 *----------------------------------------------------------------*/

/** DynamicPipeAPI class
 */
class DynamicPipeAPI : public CAPIProvider
{
public:
    /** Constructor: register APIs
     */
    DynamicPipeAPI(DynamicPipeManager* mgr) : CAPIProvider(API_PROVIDER_ID_DYNPIPE, THISMODULE) {
	_mgr = mgr;
	if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
	    CLog::Error( THISMODULE, "Unable to register provider");
	    return;
	}

	/** Register API entry points
	 */
	// ordinal for entry points
	enum API_Id {
	    Id_Create,
	    Id_Delete,
	    Id_WaitCall,
	    Id_IsExist,
	};

//	DynamicPipeCreate(string name, string group, int maxcall)
	RegisterAPI(
	    "DynamicPipeCreate",			// external name
	    Id_Create,					// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_INT,				// return type
	    (CAPIProvider::APIPROC) &DynamicPipeAPI::Create,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    5,						// number of arguments
	    CArgType("task", BYVAL, CVariant::VT_TASK),
	    CArgType("retval", BYREF, CVariant::VT_RET),
	    CArgType("name", BYVAL, CVariant::VT_STRING),
	    CArgType("group", BYVAL, CVariant::VT_STRING),
	    CArgType("maxcall", BYVAL, CVariant::VT_INT)
	    );

//	DynamicPipeDelete(string name)
	RegisterAPI(
	    "DynamicPipeDelete",			// external name
	    Id_Delete,					// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_INT,				// return type
	    (CAPIProvider::APIPROC) &DynamicPipeAPI::Delete,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    3,						// number of arguments
	    CArgType("task", BYVAL, CVariant::VT_TASK),
	    CArgType("retval", BYREF, CVariant::VT_RET),
	    CArgType("name", BYVAL, CVariant::VT_STRING)
	    );

//	DynamicPipeWaitCall(int timeout)
	RegisterAPI(
	    "DynamicPipeWaitCall",			// external name
	    Id_WaitCall,				// ordinal
	    true,					// asynchronous flag
	    CVariant::VT_INT,				// return type
	    (CAPIProvider::APIPROC) &DynamicPipeAPI::WaitCall,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    3,						// number of arguments
	    CArgType("task", BYVAL, CVariant::VT_TASK),
	    CArgType("retval", BYREF, CVariant::VT_RET),
	    CArgType("timeout", BYVAL, CVariant::VT_INT)
	    );

//	DynamicPipeIsExist(string name)
	RegisterAPI(
	    "DynamicPipeIsExist",			// external name
	    Id_IsExist,					// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_INT,				// return type
	    (CAPIProvider::APIPROC) &DynamicPipeAPI::IsExist,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    3,						// number of arguments
	    CArgType("task", BYVAL, CVariant::VT_TASK),
	    CArgType("retval", BYREF, CVariant::VT_RET),
	    CArgType("name", BYVAL, CVariant::VT_STRING)
	    );


    }

private:
    DynamicPipeManager* _mgr;

    /** DynamicPipeCreate(string name, string group, int maxcall)
     */
    STATUS API_CALL Create(CTask* task, int* retval, const char* name, const char* group, int maxcall)
    {
	*retval = _mgr->DynamicPipeCreate(task, name, group, maxcall);
	return OK;
    }

    /** DynamicPipeDelete(string name)
     */
    STATUS API_CALL Delete(CTask* task, int* retval, const char* name)
    {
	*retval = _mgr->DynamicPipeDelete(task, name);
	return OK;
    }

    /** DynamicPipeWaitCall(int timeout)
     */
    STATUS API_CALL WaitCall(CTask* task, int* retval, int timeout)
    {
	_mgr->DynamicPipeWaitCall(task, retval, timeout);
	return OK;
    }

    /** DynamicPipeIsExist(string name)
     */
    STATUS API_CALL IsExist(CTask* task, int* retval, const char* name)
    {
	*retval = _mgr->DynamicPipeIsExist(task, name);
	return OK;
    }

};


typedef List<PipeInterface*> ListDynamicPipe;
typedef List<PipeCall*> ListPipeCall;

struct Dynamic_Pipe_Task_Info
{
    int* _retval;
    time_t _timeout;
    bool _is_sleeping;
    ListDynamicPipe _owned_pipes;
    ListPipeCall _waiting_calls;

    Dynamic_Pipe_Task_Info()
    {
	_retval = 0;
	_timeout = 0;
	_is_sleeping = false;
    }
};


/*----------------------------------------------------------------* 
 * Implementation of DynamicPipeManager
 *----------------------------------------------------------------*/

/** Constructor
 */
DynamicPipeManager::DynamicPipeManager() : ServiceProvider("DynPipeMgr", true, true, Low_Priority)
{
    _instance = this;
    _last_polled = 0;
}

/** Destructor
 */
DynamicPipeManager::~DynamicPipeManager()
{
    _instance = 0;
}

/** 
 */
int DynamicPipeManager::DynamicPipeCreate(CTask* task, const char* name, const char* group, int maxcall)
{
    //if (strnicmp(name, "DynPipe", 7) != 0) {
    if (strncasecmp(name, "DynPipe", 7) != 0) {		//linux porting
	CLog::Warning(THISMODULE, "Can't create dynamic pipe: invalid name %s", name);
	return -1;
    }

    if (MediaManager::GetInstance()->FindInterface(name) != NULL) {
	CLog::Warning(THISMODULE, "Can't create dynamic pipe: name already exists %s", name);
	return -2;
    }

    Dynamic_Pipe_Task_Info* dpti = (Dynamic_Pipe_Task_Info*)task->GetTLSValue(_tlsId);

    Interface* pipe = PipeManager::Instance()->CreateInterface(name, maxcall, true, task);
    pipe->SetGroupName(group);

    char desc[1024];
    sprintf(desc, "Created by [%s:%d]", task->GetApplication()->Name().c_str(), task->GetPID());
    pipe->SetDescription(desc);

    dpti->_owned_pipes.push_back((PipeInterface*) pipe);

    return 0;
}

/** 
 */
int DynamicPipeManager::DynamicPipeDelete(CTask* task, const char* name)
{
    //if (strnicmp(name, "DynPipe", 7) != 0) {
    if (strncasecmp(name, "DynPipe", 7) != 0) {		//linux porting
	CLog::Warning(THISMODULE, "Can't delete dynamic pipe: invalid name %s", name);
	return -1;
    }

    PipeInterface* pipe = (PipeInterface*) MediaManager::GetInstance()->FindInterface(name);
    if (pipe == NULL) {
	CLog::Warning(THISMODULE, "Can't delete dynamic pipe: %s does not exist", name);
	return -2;
    }

    if (pipe->GetOwner() != task) {
	CLog::Warning(THISMODULE, "Can't delete dynamic pipe: %s does not belong to 0x%x", name, task);
	return -3;
    }

    if (!((Interface*)pipe)->SupportDynamicDeletion()) {
	CLog::Warning(THISMODULE, "Can't delete dynamic pipe: %s still busy with calls", name);
	return -4;
    }


    Dynamic_Pipe_Task_Info* dpti = (Dynamic_Pipe_Task_Info*)task->GetTLSValue(_tlsId);
    dpti->_owned_pipes.remove(pipe);

    pipe->SetOwner(0);
    _deleted_list.remove(pipe);
    _deleted_list.push_back(pipe);

    return 0;
}

/** 
 */
void DynamicPipeManager::DynamicPipeWaitCall(CTask* task, int* retval, int timeout)
{
    Dynamic_Pipe_Task_Info* dpti = (Dynamic_Pipe_Task_Info*)task->GetTLSValue(_tlsId);

    // check whether there's already one call waiting
    if (!dpti->_waiting_calls.empty()) {
	PipeCall* call = dpti->_waiting_calls.front();
	dpti->_waiting_calls.pop_front();
	call->_task = 0;

	// return the call to application
	*retval = call->AttachTask(task);
	return;
    }

    // default return value is timeout
    *retval = -1;

    // task doesn't want to block, return immediately
    if (timeout == 0) {
	return;
    }

    // need to sleep during the specified period, or
    // until a new call arrives

    // store retval to return the result later
    dpti->_retval = retval;
    dpti->_timeout = time(NULL) + timeout;
    dpti->_is_sleeping = true;
    _blocked_tasks.push_back(task);
    task->Sleep(this);
}

/** 
 */
int DynamicPipeManager::DynamicPipeIsExist(CTask* task, const char* name)
{
    //if (strnicmp(name, "DynPipe", 7) != 0) {
    if (strncasecmp(name, "DynPipe", 7) != 0) {		//linux porting
	// CLog::Warning(THISMODULE, "Can't delete dynamic pipe: invalid name %s", name);
	return 0;
    }

    Interface* itf = MediaManager::GetInstance()->FindInterface(name);
    if(itf && itf->GetSignallingDriver()->GetType() == SigType_Pipeline) {
	PipeInterface* pipe = (PipeInterface*) itf;
	if(pipe->IsDynamic() && pipe->IsActive())
	    return 1;
    }
    return 0;
}

/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
 */
bool DynamicPipeManager::Load()
{
    NEW_RETURN_(_api, DynamicPipeAPI(this));
    _tlsId = Kernel::Instance()->AllocTLS();
    return true;
}

/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
 */
bool DynamicPipeManager::Unload()
{
    delete _api;
    _api = 0;
    return true;
}

/** This function will be called by the service framework once the 
    configuration file has been fully read.
 */
bool DynamicPipeManager::CommitConfiguration()
{
    // do nothing here
    return true;
}

/** This function will be called whenever a new task is created.
 */
bool DynamicPipeManager::AttachTask(CTask *task)
{
    Dynamic_Pipe_Task_Info* dpti = new Dynamic_Pipe_Task_Info;
    task->SetTLSValue(_tlsId, (int)dpti);
    return true;
}

/** This function will be called whenever a task is about to be deleted.
 */
void DynamicPipeManager::DetachTask(CTask *task)
{
    Dynamic_Pipe_Task_Info* dpti = (Dynamic_Pipe_Task_Info*)task->GetTLSValue(_tlsId);

    if (dpti->_is_sleeping) {
	task->Wakeup();
    }

    _blocked_tasks.remove(task);

    // disconnect all pending calls
    while (!dpti->_waiting_calls.empty()) {
	PipeCall* call = dpti->_waiting_calls.front();
	dpti->_waiting_calls.pop_front();
	call->_task = 0;
	call->DisconnectReq(CC_DestinationOutOfOrder, CP_NoIndication);
    }

    // delete all owned pipes (well schedule them to be deleted)
    while (!dpti->_owned_pipes.empty()) {
	PipeInterface* pipe = dpti->_owned_pipes.front();
	dpti->_owned_pipes.pop_front();
	_deleted_list.remove(pipe);
	_deleted_list.push_back(pipe);
	pipe->SetOwner(0);
    }

    delete dpti;
    task->SetTLSValue(_tlsId, 0);
}


/** 
 */
void DynamicPipeManager::OnSetupInd(CTask* owner, PipeCall* call)
{
    Dynamic_Pipe_Task_Info* dpti = (Dynamic_Pipe_Task_Info*)owner->GetTLSValue(_tlsId);

    // check whether the owner task is expecting a call
    if (dpti->_is_sleeping) {
	*(dpti->_retval) = call->AttachTask(owner);
	owner->Wakeup();
	_blocked_tasks.remove(owner);
	dpti->_is_sleeping = false;
    } else {
	dpti->_waiting_calls.push_back(call);
	call->_task = owner;
    }
}

/** 
 */
void DynamicPipeManager::RemoveWaitingCall(PipeCall* call)
{
    if (call->_task) {
	Dynamic_Pipe_Task_Info* dpti = (Dynamic_Pipe_Task_Info*)call->_task->GetTLSValue(_tlsId);
	dpti->_waiting_calls.remove(call);
	call->_task = 0;
    }
}

/** This function will be called if this instance has a pending job for
    the task.
 */
bool DynamicPipeManager::CancelJob(CTask *task)
{
    Dynamic_Pipe_Task_Info* dpti = (Dynamic_Pipe_Task_Info*)task->GetTLSValue(_tlsId);

    if (dpti->_is_sleeping) {
	dpti->_is_sleeping = false;
	task->Wakeup();
    }

    _blocked_tasks.remove(task);

    return true;
}

/** HandleEvents: Descendent shall implement this abstract method.
    It is called by Service Provider Manager
 */
void DynamicPipeManager::HandleEvents() 
{
    time_t now = time(NULL);
    if (_last_polled == now)
	return;

    _last_polled = now;

    // delete pipes scheduled to be deleted
    if (!_deleted_list.empty()) {
	for (ListPipe::iterator itp = _deleted_list.begin(); itp != _deleted_list.end(); ) {
	    //PipeInterface* pipe = _deleted_list.front();
	    //_deleted_list.pop_front();
	    //delete pipe;

	    PipeInterface* pipe = *itp;

	    if (pipe->SupportDynamicDeletion()) {
		delete pipe;
		itp = _deleted_list.erase(itp);
	    } else {
		itp++;
	    }
	}
    }


    if (!_blocked_tasks.empty()) {
	for (ListTask::iterator itt = _blocked_tasks.begin(); itt != _blocked_tasks.end(); ) {
	    CTask* task = *itt;
	    Dynamic_Pipe_Task_Info* dpti = (Dynamic_Pipe_Task_Info*)task->GetTLSValue(_tlsId);

	    if (dpti->_timeout < now) {
		task->Wakeup();
		dpti->_is_sleeping = false;
		itt = _blocked_tasks.erase(itt);
	    } else {
		itt++;
	    }
	}
    }
}




/*----------------------------------------------------------------* 
 * Implementation of PipeManager
 *----------------------------------------------------------------*/

/** PipeManager's constructor
 */
PipeManager::PipeManager() : InterfaceManager("PipeMgr")
{
    NEW_RETURN_(_dynamicPipeMgr, DynamicPipeManager);
    assert(_thePipeManager == NULL);
    _thePipeManager = this;
}

/** PipeManager's destructor
 */
PipeManager::~PipeManager()
{
    delete _dynamicPipeMgr;
    _dynamicPipeMgr = NULL;
    _thePipeManager = NULL;
}

/** Create a singleton instance of PipeManager 
 */
PipeManager *PipeManager::Create()
{
    new PipeManager;
    return Instance();
}

/** Destroy the singleton instance of PipeManager 
 */
void PipeManager::Destroy()
{
    delete Instance();
}

/** Instance: Returns a pointer to this singleton instance, if necessary
    create one instance first before returning a pointer to it
 */
PipeManager *PipeManager::Instance()
{
    return _thePipeManager;
}

bool PipeManager::CanCreateInterface(const char *name)
{
    //return (strnicmp("PIPE", name, 4) == 0);
    return (strncasecmp("PIPE", name, 4) == 0);		//linux porting
}

Interface *PipeManager::CreateInterface(const char *name, int maxcall, bool dynamic, CTask* owner)
{
    return new PipeInterface(name, maxcall, dynamic, owner);
}

Interface *PipeManager::CreateInterface(const char *name)
{
    return CreateInterface(name, 512, false, NULL);
}


