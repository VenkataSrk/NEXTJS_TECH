/*----------------------------------------------------------------* 
 * $Archive: /xGate/MediaManager.cpp $
 * $Date: 7/13/04 2:01p $
 * $Revision: 97 $
 *
 * $History: MediaManager.cpp $
 * 
 * *****************  Version 97  *****************
 * User: Bennylp      Date: 7/13/04    Time: 2:01p
 * Updated in $/xGate
 * Only update RAP every one second
 * 
 * *****************  Version 96  *****************
 * User: Bennylp      Date: 4/23/03    Time: 5:34p
 * Updated in $/xGate
 * Administrivia: tidy-up the usage of API provider ID. Now the
 * enumeration in api.h should be used instead of bare constants.
 * 
 * *****************  Version 95  *****************
 * User: Bennylp      Date: 4/09/03    Time: 11:03a
 * Updated in $/xGate
 * Added interface type for interface (for SNMP purpose)
 * 
 * *****************  Version 94  *****************
 * User: Sonny        Date: 2/18/03    Time: 3:04p
 * Updated in $/xGate
 * fixed device usage misscalc
 * 
 * *****************  Version 93  *****************
 * User: Bennylp      Date: 1/31/03    Time: 2:25p
 * Updated in $/xGate
 * Removed 'interface enable/disable' command
 * 
 * *****************  Version 92  *****************
 * User: Junanto      Date: 1/07/02    Time: 13:14
 * Updated in $/xGate
 * 
 * *****************  Version 91  *****************
 * User: Junanto      Date: 1/07/02    Time: 12:40
 * Updated in $/xGate
 * shortened device name to 10
 * 
 * *****************  Version 90  *****************
 * User: Junanto      Date: 17/06/02   Time: 16:08
 * Updated in $/xGate
 * Put CircuitRef and Description in the same line if 
 * there's enough space (interface list)
 * 
 * *****************  Version 89  *****************
 * User: Bennylp      Date: 6/12/02    Time: 3:29p
 * Updated in $/xGate
 * Added GetCapacity()
 * 
 * *****************  Version 88  *****************
 * User: Junanto      Date: 11/06/02   Time: 12:02
 * Updated in $/xGate
 * Modified display for "interface list" and some fields are enlarged
 * 
 * *****************  Version 87  *****************
 * User: Junanto      Date: 15/05/02   Time: 10:22
 * Updated in $/xGate
 * Small modification in display during layer 1 transition
 * 
 * *****************  Version 86  *****************
 * User: Adham        Date: 3/04/02    Time: 19:58
 * Updated in $/xGate
 * Bug fix on CallStatistic (int to float)
 * 
 * *****************  Version 85  *****************
 * User: Sonny        Date: 13/02/02   Time: 15:04
 * Updated in $/xGate
 * Quality monitoring updates
 * 
 * *****************  Version 84  *****************
 * User: Sonny        Date: 11/02/02   Time: 16:30
 * Updated in $/xGate
 * quality monitoring updates
 * 
 * *****************  Version 83  *****************
 * User: Sonny        Date: 7/02/02    Time: 12:25
 * Updated in $/xGate
 * Interface::GetStatiistics
 * 
 * *****************  Version 82  *****************
 * User: Sonny        Date: 6/02/02    Time: 19:01
 * Updated in $/xGate
 * optimizations for quality monitoring
 * 
 * *****************  Version 81  *****************
 * User: Sonny        Date: 4/02/02    Time: 12:33
 * Updated in $/xGate
 * calls get quality from media
 * 
 * *****************  Version 80  *****************
 * User: Sonny        Date: 1/02/02    Time: 12:20
 * Updated in $/xGate
 * statistics monitor updates
 * 
 * *****************  Version 79  *****************
 * User: Sonny        Date: 31/01/02   Time: 13:12
 * Updated in $/xGate
 * add call quality monitoring
 * 
 * *****************  Version 78  *****************
 * User: Junanto      Date: 24/10/01   Time: 15:13
 * Updated in $/xGate
 * ARQ/DRQ/RII are replaced by periodical RII during 
 * MediaManager's HandleEvents()
 * 
 * *****************  Version 77  *****************
 * User: Junanto      Date: 16/10/01   Time: 14:08
 * Updated in $/xGate
 * 
 * *****************  Version 76  *****************
 * User: Junanto      Date: 16/10/01   Time: 14:08
 * Updated in $/xGate
 * 
 * *****************  Version 75  *****************
 * User: Bennylp      Date: 10/01/01   Time: 1:53p
 * Updated in $/xGate
 * Fixed freed memory write in Media::DetachCall
 * 
 * *****************  Version 74  *****************
 * User: Junanto      Date: 23/08/01   Time: 20:32
 * Updated in $/xGate
 * Changed implementation of Dec/IncMediaCount
 * during blocked condition
 * 
 * *****************  Version 73  *****************
 * User: Junanto      Date: 17/08/01   Time: 14:09
 * Updated in $/xGate
 * small changes
 * 
 * *****************  Version 72  *****************
 * User: Junanto      Date: 16/08/01   Time: 21:12
 * Updated in $/xGate
 * Integrated with DeviceUsage class
 * 
 * *****************  Version 71  *****************
 * User: Junanto      Date: 9/08/01    Time: 19:59
 * Updated in $/xGate
 * Call selection strategy
 * 
 * *****************  Version 70  *****************
 * User: Junanto      Date: 9/08/01    Time: 19:03
 * Updated in $/xGate
 * 
 * *****************  Version 69  *****************
 * User: Junanto      Date: 30/07/01   Time: 17:08
 * Updated in $/xGate
 * Removed warning on SCR
 * 
 * *****************  Version 68  *****************
 * User: Bennylp      Date: 7/30/01    Time: 11:45a
 * Updated in $/xGate
 * Changed SetMediaCount semantic, added SetCapacity.
 * 
 * *****************  Version 67  *****************
 * User: Junanto      Date: 27/07/01   Time: 11:02
 * Updated in $/xGate
 * Added guard in NotifyCallState when called with invalid arguments
 * 
 * *****************  Version 66  *****************
 * User: Bennylp      Date: 7/23/01    Time: 2:04p
 * Updated in $/xGate
 * Fixed bug in failed/total percentage calculation.
 * 
 * *****************  Version 65  *****************
 * User: Bennylp      Date: 7/23/01    Time: 1:25p
 * Updated in $/xGate
 * Added generic clear interface counter.
 * 
 * *****************  Version 64  *****************
 * User: Bennylp      Date: 7/20/01    Time: 4:30p
 * Updated in $/xGate
 * Added average holding/connect time in show interface.
 * 
 * *****************  Version 63  *****************
 * User: Iwanj        Date: 7/19/01    Time: 9:06p
 * Updated in $/xGate
 * add interface's direction and distribution info 
 * 
 * *****************  Version 62  *****************
 * User: Bennylp      Date: 7/19/01    Time: 4:59p
 * Updated in $/xGate
 * Fixed bug in dynamic deletion.
 * 
 * *****************  Version 61  *****************
 * User: Bennylp      Date: 7/19/01    Time: 2:17p
 * Updated in $/xGate
 * Changes to MediaManager functions to support rtp/aud shared stats.
 * 
 * *****************  Version 60  *****************
 * User: Junanto      Date: 18/07/01   Time: 18:59
 * Updated in $/xGate
 * Added support for ISUP circuit/group blocking/unblocking
 * 
 * *****************  Version 59  *****************
 * User: Junanto      Date: 18/07/01   Time: 12:52
 * Updated in $/xGate
 * Added flag to modify only mediaCount without affecting 
 * initial count
 * 
 * *****************  Version 58  *****************
 * User: Bennylp      Date: 7/13/01    Time: 3:11p
 * Updated in $/xGate
 * Reorganization of call statistic counters.
 * 
 * *****************  Version 57  *****************
 * User: Adham        Date: 7/13/01    Time: 10:56a
 * Updated in $/xGate
 * SNMP Framework integration
 * 
 * *****************  Version 56  *****************
 * User: Adham        Date: 7/09/01    Time: 1:14p
 * Updated in $/xGate
 * new method templates for counter 
 * 
 * *****************  Version 55  *****************
 * User: Junanto      Date: 4/07/01    Time: 20:47
 * Updated in $/xGate
 * 
 * *****************  Version 54  *****************
 * User: Junanto      Date: 4/07/01    Time: 17:33
 * Updated in $/xGate
 * Added SCR tracing and display
 * 
 * *****************  Version 53  *****************
 * User: Bennylp      Date: 6/30/01    Time: 8:12p
 * Updated in $/xGate
 * Changed RAPEnabled() to const.
 * 
 * *****************  Version 52  *****************
 * User: Junanto      Date: 26/06/01   Time: 10:12
 * Updated in $/xGate
 * Reduced SystemLock holding time
 * 
 * *****************  Version 51  *****************
 * User: Junanto      Date: 26/06/01   Time: 10:04
 * Updated in $/xGate
 * 
 * *****************  Version 50  *****************
 * User: Junanto      Date: 25/06/01   Time: 16:40
 * Updated in $/xGate
 * Added support for CallSetup with wild card 
 * specification
 * 
 * *****************  Version 49  *****************
 * User: Junanto      Date: 24/06/01   Time: 20:08
 * Updated in $/xGate
 * Changed AttachInterface to return bool
 * 
 * *****************  Version 48  *****************
 * User: Bennylp      Date: 6/22/01    Time: 11:02p
 * Updated in $/xGate
 * Changed some members to const, and others to non virtual, etc.
 * 
 * *****************  Version 47  *****************
 * User: Junanto      Date: 19/06/01   Time: 17:16
 * Updated in $/xGate
 * 
 * *****************  Version 46  *****************
 * User: Junanto      Date: 9/06/01    Time: 19:55
 * Updated in $/xGate
 * Protect "call list" and "trace call" against GPF when
 * call doesn't have media
 * 
 * *****************  Version 45  *****************
 * User: Junanto      Date: 8/06/01    Time: 21:37
 * Updated in $/xGate
 * 
 * *****************  Version 44  *****************
 * User: Junanto      Date: 8/06/01    Time: 10:37
 * Updated in $/xGate
 * Removed some CLog::Detail
 * 
 * *****************  Version 43  *****************
 * User: Bennylp      Date: 6/06/01    Time: 8:00p
 * Updated in $/xGate
 * Added regex selection to show service/interface command.
 * 
 * *****************  Version 42  *****************
 * User: Junanto      Date: 6/06/01    Time: 15:54
 * Updated in $/xGate
 * OnShutdown has a new parameter which indicates
 * that a requested shutdown has been cancelled
 * 
 * *****************  Version 41  *****************
 * User: Junanto      Date: 6/06/01    Time: 10:39
 * Updated in $/xGate
 * Added circuit-reference command
 * Included nb of active calls into interface list
 * 
 * *****************  Version 40  *****************
 * User: Junanto      Date: 4/06/01    Time: 13:54
 * Updated in $/xGate
 * Output of interface list contains nb of connected/busy
 * for incoming/outgoing calls
 * 
 * *****************  Version 39  *****************
 * User: Junanto      Date: 2/06/01    Time: 22:47
 * Updated in $/xGate
 * Changed interface list formatting
 * 
 * *****************  Version 38  *****************
 * User: Junanto      Date: 2/06/01    Time: 19:48
 * Updated in $/xGate
 * Added interface show/list
 * 
 * *****************  Version 37  *****************
 * User: Junanto      Date: 2/06/01    Time: 16:55
 * Updated in $/xGate
 * Added new command interface show/list
 * 
 * *****************  Version 36  *****************
 * User: Bennylp      Date: 5/30/01    Time: 6:27p
 * Updated in $/xGate
 * More restrictive use of system lock (i.e. recursive is not allowed)
 * 
 * *****************  Version 35  *****************
 * User: Bennylp      Date: 5/26/01    Time: 8:04p
 * Updated in $/xGate
 * Record each call to AcquireSystemLock in DEBUG mode.
 * 
 * *****************  Version 34  *****************
 * User: Junanto      Date: 21/05/01   Time: 15:54
 * Updated in $/xGate
 * Added call holding and connected duration (in secs)
 * 
 * *****************  Version 33  *****************
 * User: Junanto      Date: 17/05/01   Time: 18:20
 * Updated in $/xGate
 * - Interface can choose to be pollable
 * - Disable() can specify the reason
 * 
 * *****************  Version 32  *****************
 * User: Junanto      Date: 17/05/01   Time: 12:56
 * Updated in $/xGate
 * Changed some xlat table to static
 * 
 * *****************  Version 31  *****************
 * User: Iwanj        Date: 5/16/01    Time: 4:10p
 * Updated in $/xGate
 * change SetMediaCount
 * 
 * *****************  Version 30  *****************
 * User: Junanto      Date: 10/05/01   Time: 12:40
 * Updated in $/xGate
 * Replaced "\r\n" sequence to "\n". Telnet translates
 * an orphaned "\n" into "\r\n" during output
 * 
 * *****************  Version 29  *****************
 * User: Junanto      Date: 10/05/01   Time: 11:22
 * Updated in $/xGate
 * Replaced all "\r\n" with a constant
 * 
 * *****************  Version 28  *****************
 * User: Iwanj        Date: 5/09/01    Time: 10:27a
 * Updated in $/xGate
 * Changed RAP enable/disable
 * 
 * *****************  Version 27  *****************
 * User: Junanto      Date: 8/05/01    Time: 13:10
 * Updated in $/xGate
 * Fixed a bug in SetSignallingDriver
 * 
 * *****************  Version 26  *****************
 * User: Junanto      Date: 4/05/01    Time: 18:46
 * Updated in $/xGate
 * Corrected bug when a Cfg_Tree is deleted
 * 
 * *****************  Version 25  *****************
 * User: Junanto      Date: 4/05/01    Time: 18:20
 * Updated in $/xGate
 * Added support for dynamic interface creation/deletion
 * 
 * *****************  Version 24  *****************
 * User: Junanto      Date: 4/05/01    Time: 16:54
 * Updated in $/xGate
 * Interface can choose to use RAP or not
 * 
 * *****************  Version 23  *****************
 * User: Iwanj        Date: 5/03/01    Time: 10:42p
 * Updated in $/xGate
 * change RII, add resource status field
 * 
 * *****************  Version 22  *****************
 * User: Bennylp      Date: 5/02/01    Time: 1:33p
 * Updated in $/xGate
 * Tidy up ShowInterface output.
 * 
 * *****************  Version 21  *****************
 * User: Junanto      Date: 2/05/01    Time: 12:23
 * Updated in $/xGate
 * Fixed bug in OnShowInterface
 * 
 * *****************  Version 20  *****************
 * User: Bennylp      Date: 5/02/01    Time: 12:02p
 * Updated in $/xGate
 * Added show interface framework.
 * 
 * *****************  Version 19  *****************
 * User: Junanto      Date: 1/05/01    Time: 14:18
 * Updated in $/xGate
 * Simplified and reduced log output
 * 
 * *****************  Version 18  *****************
 * User: Junanto      Date: 29/04/01   Time: 19:02
 * Updated in $/xGate
 * change code in ItGetFirst/NextInterface
 * 
 * *****************  Version 17  *****************
 * User: Iwanj        Date: 4/27/01    Time: 5:17p
 * Updated in $/xGate
 * added interface iterator
 * 
 * *****************  Version 16  *****************
 * User: Junanto      Date: 27/04/01   Time: 16:51
 * Updated in $/xGate
 * added DisconnectListeners() to force 
 * switch listeners to unlisten from a
 * dying device
 * 
 * *****************  Version 15  *****************
 * User: Iwanj        Date: 4/24/01    Time: 8:53p
 * Updated in $/xGate
 * 
 * *****************  Version 14  *****************
 * User: Junanto      Date: 24/04/01   Time: 20:34
 * Updated in $/xGate
 * Added reference counting in SwitchDevice
 * and switching framework successfully tested
 * 
 * *****************  Version 13  *****************
 * User: Junanto      Date: 24/04/01   Time: 18:26
 * Updated in $/xGate
 * first link between Prosody voice / Aculab media with
 * switching framework
 * 
 * *****************  Version 12  *****************
 * User: Iwanj        Date: 4/24/01    Time: 5:52p
 * Updated in $/xGate
 * Add RII in Stop,Start,Enable,Disable
 * 
 * *****************  Version 11  *****************
 * User: Iwanj        Date: 4/24/01    Time: 4:22p
 * Updated in $/xGate
 * 
 * *****************  Version 10  *****************
 * User: Iwanj        Date: 4/24/01    Time: 4:15p
 * Updated in $/xGate
 * 
 * *****************  Version 9  *****************
 * User: Iwanj        Date: 4/24/01    Time: 11:40a
 * Updated in $/xGate
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 20/04/01   Time: 18:24
 * Updated in $/xGate
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

/*----------------------------------------------------------------* 
 * Description:
 *  
 *----------------------------------------------------------------*/

#include "stdafx.h"
#include "types.h"
#include "kernel.h"
#include "MediaManager.h"
#include "SignallingDriver.h"
#include "DigitalNetworkInterface.h"
#include "term.h" 
#include "log.h"
#include "klog.h"
//#include "RapParam.h"
//#include "RapManager.h"
//#include "pcre-3.4/pcreposix.h"	// Regex
#include <regex.h>

#include "SLInterfaceMonitor.h"

static char __modname__[] = "Media";
#define THISMODULE	    __modname__

#define NEWLINE		    "\n"
#define stricmp strcasecmp
#define strnicmp strncasecmp
#define strcmpi strcasecmp



/*----------------------------------------------------------------* 
 * Implementation of Media
 *----------------------------------------------------------------*/

/** Constructor/destructor
 */
Media::Media(const char *name,MediaId id,Interface *itf,SwitchDevice *swdev) :
    _itf(itf),
    _id(id),
    _name(name),
    _call(NULL),
    _tmHolding(0),
    _tmConnected(0),
    SupportSwitching(swdev)
{
}

Media::~Media() 
{
}

void Media::AttachCall(Call *call) 
{
    //***CLog::Detail(THISMODULE,"Media %s attached to call %s",GetName(),call->GetName());
    _call = call;
}

void Media::DetachCall(CallDirection dir,CallParameter *cparam,CallCause cc) 
{
    //***CLog::Detail(THISMODULE,"Media %s detached from call %s",GetName(),_call ? _call->GetName() : "??");
    GetSwitchDevice()->DisconnectListeners();
    GetSwitchDevice()->Unlisten();
    _call = NULL;
    _itf->ReleaseMedia(this,dir,cparam,cc);
}

Call *Media::GetCall() 
{
    return _call;
}

Interface *Media::GetInterface() 
{
    return _itf;
}

MediaId Media::GetId() 
{
    return _id;
}

const char *Media::GetName()
{
    return _name.c_str();
}

void Media::SetDuration(unsigned tmHolding,unsigned tmConnected)
{
    _tmHolding = tmHolding;
    _tmConnected = tmConnected;
}


/** This will be called by the Call instance when the call state changes.
    The only guaranteed notification is when the call is connected and
    disconnected.
 */
void Media::NotifyCallState (Call *call, CallState call_state)
{
    if (call) {
	Media* media = call->GetMedia();
	if (media && _itf && _itf->IsValid()) {
	    if (call_state==CS_Connected) {
		_itf->IncCurrentConnectedCount(media, call->GetDirection());
	    } else if (call_state >= CS_Disconnected) {
		_itf->DecCurrentConnectedCount(media, call->GetDirection());
	    }
	} else {
	    CLog::Warning(THISMODULE,"NotifyCallState called using call with invalid media (null)");
	}
    } else {
	CLog::Warning(THISMODULE,"NotifyCallState called with invalid call (null)");
    }
}

/*----------------------------------------------------------------* 
 * Implementation of Interface
 *----------------------------------------------------------------*/

enum Cmd_Itf{
    Cmd_Itf_QInterval = 1080,
};

Interface::Interface(Interface_Type type, const char *name, bool needPoll, Priority prio, bool dynamic) :
    _signature(DEADCODE),
    _signalling(NULL),
    _state(OS_Closed),
    _name(name),
    ServiceProvider(name,false,needPoll,prio),
    DeviceUsage(name),
    _mediaCount(0),
    _initialMediaCount(0),
    _totCarrierTrans(0),
    _enabled(false),
    _mediaCountChanged(true),
    _timeLastClearingCounter(0),
    _timeLastUsage(0),
    _last100Success(0),
    _last100Failed(0),
    _last100cnt(0),
    _last100nxt(0),
    _hardwareType(type),
    _lastWarning(time(NULL)),
    _direction(' '),
    _distribution(' '),
    _sel(SS_Uniform),
    _statisticsInterval(6),
    _dynamic(dynamic),
	_resourceType(0)
{
    // Clear statistics counters
    char *p_stat_begin, *p_stat_end;
    if (&stat_marker_begin < &stat_marker_end) {
	p_stat_begin = &stat_marker_begin;
	p_stat_end = &stat_marker_end;
    } else {
	p_stat_begin = &stat_marker_end;
	p_stat_end = &stat_marker_begin;
    }
    size_t size = p_stat_end-p_stat_begin;
    memset(p_stat_begin, 0, size);

    memset(_last100list, 0, sizeof(_last100list));

    MediaManager::GetInstance()->InsertInterface(this);

    _cfgTree = new Cfg_Tree_Group(_name,"",MediaManager::GetInstance()->GetConfigTree());
    _cfgTree->RegisterShowHandler(this);

    /* Cmd_QInterval
     */
    Term_Cmd* cmd = new Term_Cmd(this, Cmd_Itf_QInterval, "stat-interval", "Configure statistics interval", PrivilegeLevelAdmin, TermModeConfig, false);
    CVariantRangeValidator idrange(0, 24, "interval (0 to 24)");
    cmd->Add (new Term_Cmd_Arg ("arg1", CVariant(0), idrange));

    RegisterTermCmd(cmd,_cfgTree);

    _disableCause = DC_Administrative;
    DisableRAP();

    SLInterfaceMonitor::CreateTableEntry(this);
}

Interface::~Interface()
{
    _signature = ~DEADCODE;
    _cfgTree->UnregisterShowHandler(this);
    MediaManager::GetInstance()->RemoveInterface(this);
}

/** Set interface capacity (initial media count).
    Current media available will be calculated as new capacity substracted
    by current media channels being used.

    The new capacity must not be less than current media being used.
    This function will send RII message to the Resource Location server

    @return true if capacity has been updated successfully.
 */
bool Interface::SetCapacity(unsigned capacity)
{
    unsigned currently_used = _initialMediaCount - _mediaCount;
    if (capacity < currently_used) {
	CLog::Error (THISMODULE, "Can not set capacity in %s to %d (currently used is %d)",
				 Name().c_str(), capacity, currently_used);
	return false;
    }

    _initialMediaCount = capacity;
    _mediaCount = capacity - currently_used;
  //  cout << "Setting the mediaCount " << __FILE__ << ":" << __LINE__ << " mediaCount is "  << _mediaCount << endl;

    /***
    if (_useRap) {
	RapParam::RII param(Name(),_mediaCount,_enabled?RapParam::Rs_Enabled:RapParam::Rs_Disabled);
	RapManager::Instance()->RII(param);
    }
    ***/
    _mediaCountChanged = true;
    return true;
}


/** SetMediaCount: Set the number of channels available initially. 
    It sends an RII message to the Resource Location server
 */
void Interface::SetMediaCount(unsigned media_count)
{
    //***CLog::Detail(THISMODULE,"Interface %s has %u media(s)",Name().c_str(),media_count);
    unsigned currently_used = _initialMediaCount - _mediaCount;
    SetCapacity (currently_used + media_count);
}


/** DecMediaCount: Decrease the number of channels available
    for usage, a channel is allocated for incoming or outgoing call
    with a specific call parameters.
    It sends an ARQ message to the Resource Location server
 */
void Interface::DecMediaCount(Media *med, CallDirection dir,CallParameter *cparam,bool dontUseRLS)
{
    //***CLog::Detail(THISMODULE,"Interface %s decremented from %u",Name().c_str(),_mediaCount);

    //assert(_mediaCount);
    /* Instead of crashing system, return flase when mediaCount is zero */
#ifdef __linux__
    if ( _mediaCount == 0 ) {
        _mediaCountChanged = false;
	return;
    }
#endif

    _mediaCount--;
    _timeLastUsage = time(NULL);

    // Note:
    // Rtp_Interface relies on these and only these functions are called within
    // DecMediaCount
    IncCurrentBusyCount(med, dir);
    IncTotalCallAttempts(med, dir);

    /***
    if (_useRap && !dontUseRLS) {
	RapParam::ARQ param(
	    Name(),
	    cparam->csi,
	    cparam->cnCalling,
	    cparam->cnCalled,
	    dir==CD_Incoming?0:(dir==CD_Outgoing?1:-1)
	    );

	RapManager::Instance()->ARQ(param);
    }
    ***/
    _mediaCountChanged = true;
}

/** IncMediaCount: Increase the number of channels available
    for usage, a channel is deallocated for incoming or outgoing 
    call with a specific call parameters
    It sends a DRQ message to the Resource Location server
 */
void Interface::IncMediaCount(Media *med, CallDirection dir,CallParameter *cparam,CallCause cc,bool dontUseRLS)
{
    //***CLog::Detail(THISMODULE,"Interface %s incremented from %u",Name().c_str(),_mediaCount);

    _mediaCount++;

    // Note:
    // Rtp_Interface relies on this and only this function is called within
    // IncMediaCount
    DecCurrentBusyCount(med,dir);

    unsigned holdingTime=0,connectTime=0;
    if (med) {
	holdingTime = med->GetHoldingTime();
	connectTime = med->GetConnectedTime();
	StopUsage(holdingTime);
    }

    /***
    if (_useRap && !dontUseRLS) {
	RapParam::DRQ param(
	    Name(),
	    cparam->csi,
	    cparam->cnCalling,
	    cparam->cnCalled,
	    dir==CD_Incoming?0:(dir==CD_Outgoing?1:-1),
	    holdingTime,
	    connectTime,
	    cc
	    );
	RapManager::Instance()->DRQ(param);
    }
    ***/
    _mediaCountChanged = true;
}

/** Disable: Exclude the itf from further selection
    It sends an RII message with 0 number of channels to the 
    Resource Location server
 */
void Interface::Disable(DisableCause cause)
{
    if (_enabled) {
	if (cause != DC_Layer1) 
	    CLog::Warning(THISMODULE,"Interface %s disabled",Name().c_str());

	_enabled = false;
	_disableCause = cause;

	/***
	if (_useRap) {
	    //TBD: Send RII with mediaCount but status=disabled here
	    RapParam::RII param(Name(),_mediaCount,RapParam::Rs_Disabled);
	    RapManager::Instance()->RII(param);
	}
	***/
	_mediaCountChanged = true;
    }
}

/** Disable: Exclude the itf from further selection
    It sends an RII message with 0 number of channels to the 
    Resource Location server
 */
void Interface::Disable()
{
    Disable(DC_Administrative);
}

/** Enable: Re-include the itf from further selection
    It sends an RII message with the current number of channels to the 
    Resource Location server
 */
void Interface::Enable()
{
    if (!_enabled) {
	if (_disableCause != DC_Layer1) 
	    CLog::Debug(THISMODULE,"Interface %s enabled",Name().c_str());
	_enabled = true;
	if (GetState() == OS_InService && _useRap) {

	    //TBD: Send RII with _mediaCount here
	    /***
	    RapParam::RII param(Name(),_mediaCount,RapParam::Rs_Enabled);
	    RapManager::Instance()->RII(param);
	    ***/
	    _mediaCountChanged = true;
	}
    }
}

ObjectState Interface::GetState() const
{
    ObjectState state = _state;
    return state;
}

char Interface::GetResourceType() const
{
        return _resourceType;
}

String Interface::GetResourceTypeStr() const
{
        return this->_resourceType?((this->_resourceType==1)?"tdm":"voip") : "unspecified";
}

void Interface::SetState(ObjectState state)
{
    static const char *xlat[] = {
	"Closed",
	"OutOfService",
	"InService",
    };
    //***CLog::Detail(THISMODULE,"Interface %s state changed to %s",Name().c_str(),xlat[state]);

    _state = state;
}

bool Interface::SetSignallingDriver(SignallingDriver *signalling)
{
//    cout << "**** Inside MediaManager.cpp , SetSignallingDriver *****" << endl;
    if (signalling != _signalling) {
	if (_signalling) _signalling->DetachInterface(this);
	_signalling = NULL;
	if (signalling) {
	    if (signalling->AttachInterface(this)) {
		_signalling = signalling;
	    } else {
		CLog::Error(THISMODULE,"Unable to attach %s to signalling %s",Name().c_str(),signalling->Name().c_str());
		return false;
	    }
	}
	//***CLog::Detail(THISMODULE,"Interface %s attached to signalling %s",Name().c_str(),signalling ? signalling->Name().c_str() : "(null)");
    }
    return true;
}

void Interface::SetResourceType(char resourceType)
{
        _resourceType=resourceType;
        _mediaCountChanged=true;
}

void Interface::SetResourceTypeStr(const char* res)
{
    if(!stricmp(res, "voip"))
                SetResourceType(2);
        else if(!stricmp(res, "tdm"))
                SetResourceType(1);
        else
                SetResourceType(0);
}

void Interface::AddResourceTypeCmd(Term_Cmd_Target *target, int id, Cfg_Tree_Group *_cfgTree_)
{
    Term_Cmd *cmd = new Term_Cmd(target, id, "resource-type",
                       "Specify this interface resource type to be registered to RLS",
                       PrivilegeLevelAdmin, TermModeConfig, false);

    CVariantChoiceValidator sigVdtor("","");
    sigVdtor.Add("unspecified", "resource type generic");
    sigVdtor.Add("tdm", "TDM/mobile/landline resource type");
        sigVdtor.Add("voip","VoIP resource type");

    cmd->Add(new Term_Cmd_Arg("@0", "", sigVdtor));
        target->RegisterTermCmd(cmd, _cfgTree_);
}

SignallingDriver *Interface::GetSignallingDriver() 
{
    SignallingDriver *signalling = _signalling;
    return signalling;
}

unsigned Interface::GetMediaCount() const
{
    unsigned mediaCount = _mediaCount;
    return _mediaCount;
}

/** Get the interface capacity (initial media count).
 */
unsigned Interface::GetCapacity() const
{
    return _initialMediaCount;
}


const char * Interface::GetGroupName() const
{
    return _groupName.c_str();
}

void Interface::SetGroupName(const char* groupName)
{
    //***CLog::Detail(THISMODULE,"Interface %s assigned to group %s",Name().c_str(),groupName);
    _groupName = groupName;
}

bool Interface::IsActive() const
{
    return ((GetState() == OS_InService) && _enabled);
}

/** Set the inteface description.
 */
void Interface::SetDescription ( const String & desc )
{
    _description = desc;
}

/** Get the interface description.
 */
const String & Interface::GetDescription() const
{
    return _description;
}

/** Set the inteface reference.
 */
void Interface::SetCircuitReference ( const String & circuitRef )
{
    _circuitRef = circuitRef;
}

/** Get the interface reference
 */
const String & Interface::GetCircuitReference() const
{
    return _circuitRef;
}

/** Enable RAP usage
 */
void Interface::EnableRAP()
{
    //***CLog::Detail(THISMODULE,"RAP enabled for %s",Name().c_str());
    _useRap = true;
//     cout << "Setting the mediaCount " << __FILE__ << ":" << __LINE__ << " mediaCount is "  << _mediaCount << endl;
    SetMediaCount(_mediaCount);
}

/** Disable RAP usage
 */
void Interface::DisableRAP()
{
    //***CLog::Detail(THISMODULE,"RAP disabled for %s",Name().c_str());
    _useRap = false;
}

/** Return RAP usage flag
 */
bool Interface::RAPEnabled() const
{
    return _useRap;
}

/** It returns true when the interface support dynamic deletion
 */
bool Interface::SupportDynamicDeletion() 
{
    // by default, interface does not support this feature
    return false;
}

/** Called if the interface which support dynamic deletion is about
    to be destroyed
 */
void Interface::DestroyInterface()
{
    assert(false);
}

/** Total call counters related methods
 */
void Interface::IncTotalCallSuccess(Media *med, CallDirection dir) 
{
    assert(dir==CD_Incoming || dir==CD_Outgoing);
    _last100Success++;
    if (_last100cnt < 100) {
	_last100list[_last100nxt++] = true;
	_last100cnt++;
    } else {
	if (_last100nxt == 100) _last100nxt = 0;
	if (_last100list[_last100nxt]) _last100Success--;
	else _last100Failed--;
	_last100list[_last100nxt++] = true;
    }
    ++_totCallSuccess[dir];
    ++_lastClr_CallSuccess[dir];
    CheckSCR();
    _hourlyCallSuccess.add(1);
}

/** Total call counters related methods
 */
void Interface::IncTotalCallFailed(Media *med, CallDirection dir) 
{
    assert(dir==CD_Incoming || dir==CD_Outgoing);
    _last100Failed++;
    if (_last100cnt < 100) {
	_last100list[_last100nxt++] = false;
	_last100cnt++;
    } else {
	if (_last100nxt == 100) _last100nxt = 0;
	if (_last100list[_last100nxt]) _last100Success--;
	else _last100Failed--;
	_last100list[_last100nxt++] = false;
    }
    ++_totCallFailed[dir];
    ++_lastClr_CallFailed[dir];
    CheckSCR();
}


/** Warn if SCR is getting low
 */
void Interface::CheckSCR() 
{
    time_t now = time(NULL);
    if ((_last100cnt < 50) || (now-_lastWarning) < 30) return;

    unsigned scr = GetSCR();
    if (scr >= 0 && scr < 50) {
	char buffer[128];
	sprintf(buffer,"SCR on %s drops to %d%%",Name().c_str(),scr);
	CLog::Detail(THISMODULE,buffer);
	MediaManager::GetInstance()->TraceSCR(buffer);
    }
    _lastWarning = now;
}

/** Returns SCR
 */
unsigned Interface::GetSCR()
{
    unsigned totcalls = _last100Success+_last100Failed;
    return totcalls ? (_last100Success*100/totcalls) : -1;
}

bool Interface::Load()
{
    CLog::Detail(THISMODULE,"Interface %s opened",Name().c_str());
    SetState(OS_OutOfService);
    return true;
}

bool Interface::Unload()
{
    if (_state == OS_OutOfService) {
	CLog::Detail(THISMODULE,"Interface %s closed",Name().c_str());
	SetState(OS_Closed);
	return true;
    } else {
	// Invalid state
	return false;
    }
}

bool Interface::Start()
{
    CLog::Detail(THISMODULE,"Interface %s started",Name().c_str());
    SetState(OS_InService);
    if (_enabled) {
	//TBD: Send RII with _mediaCount here
	/***
	RapParam::RII param(Name(),_mediaCount,RapParam::Rs_Enabled);
	RapManager::Instance()->RII(param);
	***/
	_mediaCountChanged = true;
    }
    return true;
}

bool Interface::Stop()
{
    CLog::Detail(THISMODULE,"Interface %s stopped",Name().c_str());
    SetState(OS_OutOfService);

    //TBD: Send RII with mediaCount but status=disabled here
    /***
    RapParam::RII param(Name(),_mediaCount,RapParam::Rs_Disabled);
    RapManager::Instance()->RII(param);
    ***/
    _mediaCountChanged = true;

    return true;
}


/** Clear counter.
 */
void Interface::OnClearCounter()
{
    _timeLastClearingCounter = time(NULL);
    memset(&stat_clrmarker_begin, 0, &stat_clrmarker_end-&stat_clrmarker_begin);
    _last100Success = 0;
    _last100Failed = 0;
    memset(_last100list, 0, sizeof(_last100list));
    _last100cnt = 0;
    _last100nxt = 0;
}


/** Display interface information to the terminal users. This function will
    display common/generic information first, then show interface specific
    information by calling OnShowInterface().
 */
void Interface::ShowInterface ( Term_Session * session )
{
    // Copy vars to temporary while holding system lock.
    ACQUIRE_SYSTEM_LOCK();
    ObjectState tmp_state = _state;
    unsigned tmp_init_media_count = _initialMediaCount;
    unsigned tmp_media_count = _mediaCount;
    bool tmp_enabled = _enabled;
    String tmp_name = _name;
    String tmp_group_name = _groupName;
    String tmp_description = _description;
    String tmp_reference = _circuitRef;
    time_t tmp_time_last_usage = _timeLastUsage;

    unsigned totCallAttempt[Statistic_Size],
	     totCallConnected[Statistic_Size], 
	     totCallSuccess[Statistic_Size], 
	     totCallFailed[Statistic_Size];

    memcpy(totCallAttempt, _totCallAttempt, sizeof(totCallAttempt));
    memcpy(totCallConnected, _totCallConnected, sizeof(totCallConnected));
    memcpy(totCallSuccess, _totCallSuccess, sizeof(totCallSuccess));
    memcpy(totCallFailed, _totCallFailed, sizeof(totCallFailed));
    
    unsigned holding_lo = GetTotalHoldingTime_Lo(CD_Both),
	     holding_hi = GetTotalHoldingTime_Hi(CD_Both),
	     connected_lo = GetTotalConnectedTime_Lo(CD_Both),
	     connected_hi = GetTotalConnectedTime_Hi(CD_Both);
    unsigned long tot_holding = _totCallHoldingTime[CD_Incoming]+_totCallHoldingTime[CD_Outgoing],
	       tot_connected = _totCallConnectedTime[CD_Incoming]+_totCallConnectedTime[CD_Outgoing];

    unsigned tot_num_call_attemp_fin = _totCallAttempt_Fin[CD_Incoming] + _totCallAttempt_Fin[CD_Outgoing],
	     tot_num_call_cnct_fin = _totCallConnected_Fin[CD_Incoming] + _totCallConnected_Fin[CD_Outgoing];

    time_t tmp_time_last_clear = _timeLastClearingCounter;

    unsigned lastClr_CallAttempt[Statistic_Size],
	     lastClr_CallConnected[Statistic_Size], 
	     lastClr_CallSuccess[Statistic_Size], 
	     lastClr_CallFailed[Statistic_Size];

    memcpy(lastClr_CallAttempt, _lastClr_CallAttempt, sizeof(lastClr_CallAttempt));
    memcpy(lastClr_CallConnected, _lastClr_CallConnected, sizeof(lastClr_CallConnected));
    memcpy(lastClr_CallSuccess, _lastClr_CallSuccess, sizeof(lastClr_CallSuccess));
    memcpy(lastClr_CallFailed, _lastClr_CallFailed, sizeof(lastClr_CallFailed));

    unsigned long lastClr_tot_holding = _lastClr_CallHoldingTime[CD_Incoming]+_lastClr_CallHoldingTime[CD_Outgoing],
	       lastClr_tot_connected = _lastClr_CallConnectedTime[CD_Incoming]+_lastClr_CallConnectedTime[CD_Outgoing];

    unsigned lastClr_num_call_attemp_fin = _lastClr_CallAttempt_Fin[CD_Incoming] + _lastClr_CallAttempt_Fin[CD_Outgoing],
	     lastClr_num_call_cnct_fin = _lastClr_CallConnected_Fin[CD_Incoming] + _lastClr_CallConnected_Fin[CD_Outgoing];

    RELEASE_SYSTEM_LOCK();

    const char *state_str;
    switch (tmp_state) {
    case OS_Closed:
	state_str = "null";
	break;
    case OS_OutOfService:
	state_str = "out of service";
	break;
    case OS_InService:
	state_str = "running";
	break;
    default:
	state_str = "unknown";
	break;
    }

    // Name and state
    session->Sendf ("Interface %s ",tmp_name.c_str());

    if (tmp_enabled) {
	session->Send("is enabled");
    } else {
	switch (_disableCause) {
	case DC_Administrative:
	    session->Send("is disabled by administrator");
	    break;

	case DC_Shutdown:
	    session->Send("is disabled due to shutdown in progress");
	    break;

	case DC_Layer1:
	    session->Send("has layer 1 error");
	    break;

	default:
	    session->Send("is disabled (cause unknown)");
	    break;

	}

    }
    session->Sendf(", state is %s" NEWLINE,state_str);

    const char *hwtype = NULL;
    switch (_hardwareType) {
    case ITF_E1_T1:		hwtype = "E1/T1";	break;
    case ITF_ATM:		hwtype = "ATM";		break;
    case ITF_RTP:		hwtype = "RTP";		break;
    case ITF_PIPE:		hwtype = "PIPE";	break;
    case ITF_ANALOG:		hwtype = "ANALOG";	break;
    case ITF_AUDIOCODES:	hwtype = "AUDIOCODES";	break;
    case ITF_PROSODYX:		hwtype = "PROSODYX";	break;
    default:			hwtype = "UNKNOWN";	break;
    }
    session->Sendf(SHOW_INTERFACE_INDENT "Interface type is %s" NEWLINE, hwtype);

    // Description, if any
    if (_description.size())
	session->Sendf ( SHOW_INTERFACE_INDENT "Desription: %s" NEWLINE, tmp_description.c_str());

    // Description, if any
    if (_circuitRef.size())
	session->Sendf ( SHOW_INTERFACE_INDENT "Circuit reference: %s" NEWLINE, tmp_reference.c_str());

    // Group name
    if (tmp_group_name.size())
	session->Sendf ( SHOW_INTERFACE_INDENT "Assigned to group %s" NEWLINE, tmp_group_name.c_str());
    else
	session->Sendf ( SHOW_INTERFACE_INDENT "Interface is not assigned to any group" NEWLINE);

    // Media count
    session->Sendf (SHOW_INTERFACE_INDENT 
		    "Total number of channels: %u, channels in use: %u, channel free: %u" NEWLINE, 
		    tmp_init_media_count, tmp_init_media_count-tmp_media_count, tmp_media_count);

    // Last usage
    if (tmp_time_last_usage == 0)
	session->Sendf (SHOW_INTERFACE_INDENT "Last usage never" NEWLINE);
    else {
	String str_last_usage = ctime(&tmp_time_last_usage);
	str_last_usage.erase (str_last_usage.size()-1);
	time_t now = time(NULL);
	String ago = ConvertInterval2String(now - tmp_time_last_usage);
	session->Sendf (SHOW_INTERFACE_INDENT "Last usage: %s [%s ago]" NEWLINE, str_last_usage.c_str(), ago.c_str());
    }

    // Permanent call counters
    session->Sendf (SHOW_INTERFACE_INDENT "Permanent call counters (In/Out/Total):" NEWLINE
		    SHOW_INTERFACE_INDENT "  Total attempts: %u/%u/%u, connected: %u/%u/%u" NEWLINE
		    SHOW_INTERFACE_INDENT "  Total success: %u/%u/%u, failed: %u/%u/%u (%d/%d/NA %%)" NEWLINE,
		    totCallAttempt[CD_Incoming], 
		    totCallAttempt[CD_Outgoing], 
		    totCallAttempt[CD_Incoming]+totCallAttempt[CD_Outgoing],

		    totCallConnected[CD_Incoming], 
		    totCallConnected[CD_Outgoing], 
		    totCallConnected[CD_Incoming]+totCallConnected[CD_Outgoing],

		    totCallSuccess[CD_Incoming], 
		    totCallSuccess[CD_Outgoing], 
		    totCallSuccess[CD_Incoming]+totCallSuccess[CD_Outgoing],

		    totCallFailed[CD_Incoming], 
		    totCallFailed[CD_Outgoing], 
		    totCallFailed[CD_Incoming]+totCallFailed[CD_Outgoing],
		    
		    (totCallAttempt[CD_Incoming]==0 ? 0 : totCallFailed[CD_Incoming]*100/totCallAttempt[CD_Incoming]),
		    (totCallAttempt[CD_Outgoing]==0 ? 0 : totCallFailed[CD_Outgoing]*100/totCallAttempt[CD_Outgoing]));

    session->Sendf (SHOW_INTERFACE_INDENT "  Total holding time: %s (roll-over=%u)" NEWLINE
		    SHOW_INTERFACE_INDENT "  Total connected time: %s (roll-over=%u) (%d%%)" NEWLINE,
		    ConvertInterval2String(holding_lo).c_str(),
		    holding_hi,
		    ConvertInterval2String(connected_lo).c_str(),
		    connected_hi,
		    (tot_holding==0 ? 0 : tot_connected*100/tot_holding) // Avoid division by zero.
		    ); 

    String savg_hold = ConvertInterval2String(tot_num_call_attemp_fin==0 ? 0 : tot_holding/tot_num_call_attemp_fin),
	   savg_cnct = ConvertInterval2String(tot_num_call_cnct_fin==0 ? 0 : tot_connected/tot_num_call_cnct_fin);
    session->Sendf (SHOW_INTERFACE_INDENT "  Average holding time: %s, average connected time: %s" NEWLINE,
		    savg_hold.c_str(), savg_cnct.c_str());

    // Call counters since the last clear counter
    if (_timeLastClearingCounter==0)
	session->Sendf( SHOW_INTERFACE_INDENT "Last clear counter never" NEWLINE);
    else {
	String str_last_clear = ctime(&tmp_time_last_clear);
	str_last_clear.erase (str_last_clear.size()-1);
	time_t now = time(NULL);
	String ago = ConvertInterval2String(now - tmp_time_last_clear);
	session->Sendf (SHOW_INTERFACE_INDENT "Last clear counter: %s [%s ago]" NEWLINE, 
			str_last_clear.c_str(), ago.c_str());

	session->Sendf (SHOW_INTERFACE_INDENT "Call counters since last clear counter (In/Out/Total):" NEWLINE
			SHOW_INTERFACE_INDENT "  Total attempts: %u/%u/%u, connected: %u/%u/%u" NEWLINE
			SHOW_INTERFACE_INDENT "  Total success: %u/%u/%u, failed: %u/%u/%u (%d/%d/NA %%)" NEWLINE,
			lastClr_CallAttempt[CD_Incoming], 
			lastClr_CallAttempt[CD_Outgoing], 
			lastClr_CallAttempt[CD_Incoming] + lastClr_CallAttempt[CD_Outgoing],

			lastClr_CallConnected[CD_Incoming], 
			lastClr_CallConnected[CD_Outgoing], 
			lastClr_CallConnected[CD_Incoming] + lastClr_CallConnected[CD_Outgoing],

			lastClr_CallSuccess[CD_Incoming], 
			lastClr_CallSuccess[CD_Outgoing], 
			lastClr_CallSuccess[CD_Incoming] + lastClr_CallSuccess[CD_Outgoing],

			lastClr_CallFailed[CD_Incoming], 
			lastClr_CallFailed[CD_Outgoing], 
			lastClr_CallFailed[CD_Incoming] + lastClr_CallFailed[CD_Outgoing],
			
			(lastClr_CallAttempt[CD_Incoming]==0 ? 0 : lastClr_CallFailed[CD_Incoming]*100/lastClr_CallAttempt[CD_Incoming]),
			(lastClr_CallAttempt[CD_Outgoing]==0 ? 0 : lastClr_CallFailed[CD_Outgoing]*100/lastClr_CallAttempt[CD_Outgoing]));

	session->Sendf (SHOW_INTERFACE_INDENT "  Total holding time: %s" NEWLINE
			SHOW_INTERFACE_INDENT "  Total connected time: %s (%d%%)" NEWLINE,
			ConvertInterval2String(lastClr_tot_holding).c_str(),
			ConvertInterval2String(lastClr_tot_connected).c_str(),
			(lastClr_tot_holding==0 ? 0 : lastClr_tot_connected*100/lastClr_tot_holding) // Avoid division by zero.
			); 

	String savg_hold = ConvertInterval2String(lastClr_num_call_attemp_fin==0 ? 0 : lastClr_tot_holding/lastClr_num_call_attemp_fin),
	       savg_cnct = ConvertInterval2String(lastClr_num_call_cnct_fin==0 ? 0 : lastClr_tot_connected/lastClr_num_call_cnct_fin);
	session->Sendf (SHOW_INTERFACE_INDENT "  Average holding time: %s, average connected time: %s" NEWLINE,
			savg_hold.c_str(), savg_cnct.c_str());
    }

    // Assume that interface can not be deleted while engine is running,
    // so we don't expect 'this' instance is deleted when we call the function.
    ShowCallStatistics(session, _statisticsInterval);
    OnShowInterface(session);

}


/**
 Terminal command callback
 */
void Interface::DoTermCmd(Term_Session* session, bool no, Term_Cmd* cmd) {

    CVariant v0;
    switch (cmd->GetId()) {
    case Cmd_Itf_QInterval:
	cmd->GetArg((size_t)0)->GetValue(v0);
	_statisticsInterval = v0.AsInt();
	break;
    default:
	break;
    }


}

void Interface::PrintData(Term_Session * session, time_t start, time_t end, int code) 
{
    ACQUIRE_SYSTEM_LOCK();
    int calls = _hourlyCallAttempt.calculate_sum(start, end);
    int connected = _hourlyCallConnected.calculate_sum(start, end);
    int success = _hourlyCallSuccess.calculate_sum(start, end);
    int avg_connected_time = _hourlyCallAvgConnectedDuration.calculate_avg(start, end);
    int avg_pdd = _hourlyCallAvgPostDialDelay.calculate_avg(start, end);
    RELEASE_SYSTEM_LOCK();
    
    char s_title[10];
    char s_avg_connected_time[10], s_avg_pdd[10];

    if (calls < 0) calls = 0;
    if (connected < 0) connected = 0;
    if (success < 0) success = 0;
    if (avg_connected_time < 0) avg_connected_time = 0;
    if (avg_pdd < 0) avg_pdd = 0;

    struct tm* the_time;
    int start_hour, end_hour;
    if (code == 1) {
	the_time = localtime(&start); start_hour = the_time->tm_hour;
        sprintf(s_title, "  %02d-now:", start_hour);
    }
    else if (code == 2) {
	the_time = localtime(&start); start_hour = the_time->tm_hour;
	the_time = localtime(&end); end_hour = the_time->tm_hour;
	sprintf(s_title, "  %02d-%02d :", start_hour, end_hour);
    }
    else if (code == 3) {
	sprintf(s_title, "%-9s", _name.c_str());
    }

    sprintf(s_avg_connected_time, "%u:%02u", avg_connected_time/60, avg_connected_time%60);
    sprintf(s_avg_pdd, "%u:%02u", avg_pdd/60, avg_pdd%60);

    session->Sendf("%7s %7d %7d %7d %7d %3d%% %3d%% %7s %7s\n", 
	s_title, calls, success, calls-success, connected, 
	calls ? success*100/calls : 0, calls ? connected*100/calls : 0, 
	s_avg_connected_time, s_avg_pdd);

}

void Interface::ShowCallStatistics(Term_Session* session, int interval) {
    if (interval <= 0 || interval > 24) return;

    session->Sendf("  Call statistics:\n"
		   "            Calls Success  Failed Connect  SCR  ASR Avg.Cnk Avg.PDD\n");

    unsigned currentHour;
    time_t ltime;
    struct tm* now;
    time(&ltime);
    now = localtime(&ltime);
    currentHour = now->tm_hour;
    now->tm_min = 0;
    now->tm_sec = 0;
    ltime = mktime(now);

    int i, n = 24 / interval + 1; if ((24 % interval) > 0) n++; 
    time_t start = ltime - (currentHour%interval)*3600;
    time_t end = ltime + 3600;

    for (i = 0; i < n; i++) {
	PrintData(session, start, end, (i == 0)?1:2);
	end = start;
	if (i == n-2) start = start - (interval-(currentHour%interval)-1)*3600;
	else start = start - interval*3600;
	if (end == start) break;
    }
}


float Interface::GetStatistics(CallStatistics callstat, time_t start, time_t end)
{
    float a, b;
    switch (callstat) {
    case CALLSTAT_ATTEMPT:
	a = _hourlyCallAttempt.calculate_sum(start, end);
	return (a<=0)?0:a;
    case CALLSTAT_SUCCESS:
	a = _hourlyCallSuccess.calculate_sum(start, end);
	return (a<=0)?0:a;
    case CALLSTAT_FAILED:
	a = _hourlyCallAttempt.calculate_sum(start, end);
	b = _hourlyCallSuccess.calculate_sum(start, end);
	if (a<=0) return 0;
	else if (b<=0) return a;
	else return a-b;
    case CALLSTAT_CONNECTED:
	a = _hourlyCallConnected.calculate_sum(start, end);
	return (a<=0)?0:a;
    case CALLSTAT_SCR:
	a = _hourlyCallAttempt.calculate_sum(start, end);
	b = _hourlyCallSuccess.calculate_sum(start, end);
	if (a<=0) return 0;
	else if (b<=0) return 0;
	else return b/a;
    case CALLSTAT_ASR:
	a = _hourlyCallAttempt.calculate_sum(start, end);
	b = _hourlyCallConnected.calculate_sum(start, end);
	if (a<=0) return 0;
	else if (b<=0) return 0;
	else return b/a;
    case CALLSTAT_AVG_CNKTIME:
	a = _hourlyCallAvgConnectedDuration.calculate_avg(start, end);
	return (a<=0)?0:a;
    case CALLSTAT_AVG_PDD:
	a = _hourlyCallAvgPostDialDelay.calculate_avg(start, end);
	return (a<=0)?0:a;
    default:
	return 0;
    }
}

/*----------------------------------------------------------------* 
 * Implementation of DummyCall
 *----------------------------------------------------------------*/

/** DummyCall: Specific implementation of Call for dummy call
 */
class DummyCall : public Call
{
public:
    /** Default constructor and destructor
     */
    DummyCall(const char* itfname) : Call(CD_Incoming, itfname) {
	SetState(CS_Initiating);
    }

    /** Requests: these methods need to be overridden by each Signalling 
     */
    virtual bool DeviceOverlapReq(const char *additionalDigits,bool complete) {
	// do nothing
	return true;
    }

    virtual bool DeviceProceedingReq(CallProgress progress, CallExtraInfo* exinfo) {
	// do nothing
	return true;
    }

    virtual bool DeviceProgressReq(CallProgress progress, CallExtraInfo* exinfo) {
	// do nothing
	return true;
    }

    virtual bool DeviceAlertingReq(CallProgress progress, CallExtraInfo* exinfo) {
	// do nothing
	return true;
    }

    virtual bool DeviceSendInfo (CallExtraInfo* exinfo) {
        // do nothing
        return true;
    }

    virtual bool DeviceConnectReq(CallExtraInfo* exinfo) {
	// call ConnectCnf to prevent task from being suspended indefinitely
	ConnectCnf();
	return true;
    }

    virtual bool DeviceSuspendReq() {
	// do nothing
	return true;
    }

    virtual bool DeviceResumeReq() {
	// do nothing
	return true;
    }

    virtual bool DeviceDisconnectReq(CallCause cause,CallProgress progress) {
	// call RelComInd to prevent task from being suspended indefinitely
	// and to release the allocated media
	RelComInd(cause);
	return true;
    }
};


/*----------------------------------------------------------------* 
 * Implementation of InterfaceAPI
 *----------------------------------------------------------------*/
const int InterfaceAPIProviderId = API_PROVIDER_ID_INTERFACE;

class InterfaceAPI : public CAPIProvider
{
public:
    InterfaceAPI() : CAPIProvider(InterfaceAPIProviderId,THISMODULE) {
	if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
	    CLog::Error( THISMODULE, "Unable to register provider");
	    return;
	}

	/** Register API entry points
	 */
	// ordinal for entry points
	enum API_Id {
	    Id_GetQuality,
	    Id_GetGroupName,
	    Id_GetSignallingType,
	    Id_GetSignallingName,
	    Id_GetSignallingGroup,
	    Id_CreateDummyCall,
	};

//	InterfaceGetQuality(CTask* task, float* value, int handle, int stream, 
//			    const char* name, bool active)
	RegisterAPI(
	    "InterfaceGetQuality",			// external name
	    Id_GetQuality,				// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_FLOAT,			// return type
	    (CAPIProvider::APIPROC) &InterfaceAPI::InterfaceGetQuality,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    9,					// number of arguments
	    CArgType("task",BYVAL,CVariant::VT_TASK),
	    CArgType("value",BYREF,CVariant::VT_RET),
	    CArgType("handle",BYVAL,CVariant::VT_INT),
	    CArgType("stream",BYVAL,CVariant::VT_INT),
	    CArgType("quality_name",BYVAL,CVariant::VT_STRING),
	    CArgType("interface_name",BYVAL,CVariant::VT_STRING),
	    CArgType("hour_start",BYVAL,CVariant::VT_INT),
	    CArgType("hour_end",BYVAL,CVariant::VT_INT),
	    CArgType("active",BYVAL,CVariant::VT_INT)
	    );

//	InterfaceGetGroupName(char* retval, const char* itfname)
	RegisterAPI(
	    "InterfaceGetGroupName",			// external name
	    Id_GetGroupName,				// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_STRING,			// return type
	    (CAPIProvider::APIPROC) &InterfaceAPI::InterfaceGetGroupName,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    2,						// number of arguments
	    CArgType("retval", BYREF, CVariant::VT_RET),
	    CArgType("itfname", BYVAL, CVariant::VT_STRING)
	    );

//	InterfaceGetSignallingType(char* retval, const char* itfname)
	RegisterAPI(
	    "InterfaceGetSignallingType",		// external name
	    Id_GetSignallingType,			// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_STRING,			// return type
	    (CAPIProvider::APIPROC) &InterfaceAPI::InterfaceGetSignallingType,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    2,						// number of arguments
	    CArgType("retval", BYREF, CVariant::VT_RET),
	    CArgType("itfname", BYVAL, CVariant::VT_STRING)
	    );

//	InterfaceGetSignallingName(char* retval, const char* itfname)
	RegisterAPI(
	    "InterfaceGetSignallingName",			// external name
	    Id_GetSignallingName,			// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_STRING,			// return type
	    (CAPIProvider::APIPROC) &InterfaceAPI::InterfaceGetSignallingName,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    2,						// number of arguments
	    CArgType("retval", BYREF, CVariant::VT_RET),
	    CArgType("itfname", BYVAL, CVariant::VT_STRING)
	    );

//	InterfaceGetSignallingGroup(int* retval, const char* itfname)
	RegisterAPI(
	    "InterfaceGetSignallingGroup",		// external name
	    Id_GetSignallingGroup,			// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_INT,				// return type
	    (CAPIProvider::APIPROC) &InterfaceAPI::InterfaceGetSignallingGroup,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    2,						// number of arguments
	    CArgType("retval", BYREF, CVariant::VT_RET),
	    CArgType("itfname", BYVAL, CVariant::VT_STRING)
	    );

//	InterfaceCreateDummyCall(CTask* task, int* retval, const char* itfname)
	RegisterAPI(
	    "InterfaceCreateDummyCall",			// external name
	    Id_CreateDummyCall,				// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_INT,				// return type
	    (CAPIProvider::APIPROC) &InterfaceAPI::InterfaceCreateDummyCall,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    3,						// number of arguments
	    CArgType("task", BYVAL, CVariant::VT_TASK),
	    CArgType("retval", BYREF, CVariant::VT_RET),
	    CArgType("itfname", BYVAL, CVariant::VT_STRING)
	    );

    }

    //---------------------
    STATUS API_CALL InterfaceGetQuality(CTask* task, 
	float* value, int handle, int stream, 
	const char* quality_name, const char* interface_name, 
	int hour_start, int hour_end, int active) {
	Interface* itf;
	CallQualityParameter param;

	time_t start_t, end_t;
	time(&start_t);
	struct tm* now = localtime(&start_t);
	now->tm_min = 0;
	now->tm_sec = 0;

	if (hour_start < 0 || hour_start >= 24) start_t = 0;
	else {
	    now->tm_hour = hour_start; 
	    start_t = mktime(now);
	}
	if (hour_end < 0 || hour_end >= 24) end_t = 0;
	else {
	    now->tm_hour = hour_end; 
	    end_t = mktime(now);
	}

	*value = -1;
	if ((itf = MediaManager::GetInstance()->FindInterface(interface_name)) != NULL) {
	    if (stream == 0) {
		if (strcmp(quality_name, "packets") == 0) param = CQ_RX_PACKETS;
		else if (strcmp(quality_name, "bytes") == 0) param = CQ_RX_BYTES;
		else if (strcmp(quality_name, "rtt") == 0) param = CQ_RX_RTT;
		else if (strcmp(quality_name, "jitter") == 0) param = CQ_RX_JITTER;
		else if (strcmp(quality_name, "loss") == 0) param = CQ_RX_LOSS;
		else if (strcmp(quality_name, "fracloss") == 0) param = CQ_RX_FRACLOSS;
		else if (strcmp(quality_name, "is") == 0) param = CQ_RX_IS;
		else if (strcmp(quality_name, "id") == 0) param = CQ_RX_ID;
		else if (strcmp(quality_name, "ie") == 0) param = CQ_RX_IE;
		else if (strcmp(quality_name, "itot") == 0) param = CQ_RX_ITOT;
		else return OK;
	    }
	    else {
		if (strcmp(quality_name, "packets") == 0) param = CQ_TX_PACKETS;
		else if (strcmp(quality_name, "bytes") == 0) param = CQ_TX_BYTES;
		else if (strcmp(quality_name, "rtt") == 0) param = CQ_TX_RTT;
		else if (strcmp(quality_name, "jitter") == 0) param = CQ_TX_JITTER;
		else if (strcmp(quality_name, "loss") == 0) param = CQ_TX_LOSS;
		else if (strcmp(quality_name, "fracloss") == 0) param = CQ_TX_FRACLOSS;
		else if (strcmp(quality_name, "is") == 0) param = CQ_TX_IS;
		else if (strcmp(quality_name, "id") == 0) param = CQ_TX_ID;
		else if (strcmp(quality_name, "ie") == 0) param = CQ_TX_IE;
		else if (strcmp(quality_name, "itot") == 0) param = CQ_TX_ITOT;
		else return OK;
	    }
	    *value = itf->GetQuality(param, start_t, end_t, active!=0);
	}

	return OK;
    }

    //---------------------
    STATUS API_CALL InterfaceGetGroupName(char* retval, const char* itfname)
    {
	Interface* itf;
	strcpy(retval, "*bad-interface*");

	if ((itf = MediaManager::GetInstance()->FindInterface(itfname)) != NULL) {
	    strcpy(retval, itf->GetGroupName());
	}

	return OK;
    }

    //---------------------
    STATUS API_CALL InterfaceGetSignallingType(char* retval, const char* itfname)
    {
	Interface* itf;
	strcpy(retval, "*bad-interface*");

	if ((itf = MediaManager::GetInstance()->FindInterface(itfname)) != NULL) {
	    SignallingDriver* sig = itf->GetSignallingDriver();

	    if (sig) {
		switch (sig->GetType()) {
		case SigType_Aculab:
		    strcpy(retval, "Aculab");
		    break;

		case SigType_ISUP:
		    strcpy(retval, "ISUP");
		    break;

		case SigType_H323:
		    strcpy(retval, "H323");
		    break;

		case SigType_SIP:
		    strcpy(retval, "SIP");
		    break;

		case SigType_SOFIA:
		    strcpy(retval, "SOFIA");
		    break;

		case SigType_DialogicAnalog:
		    strcpy(retval, "DialogicAnalog");
		    break;

		case SigType_DialogicDigital:
		    strcpy(retval, "DialogicDigital");
		    break;

		case SigType_Pipeline:
		    strcpy(retval, "Pipe");
		    break;

		case SigType_ATM:
		    strcpy(retval, "ATM");
		    break;

		case SigType_Q931oIP:
		    strcpy(retval, "Q931oIP");
		    break;

		case SigType_BSSAP:
		    strcpy(retval, "BSSAP");
		    break;

		default:
		    strcpy(retval, "*unknown*");
		    break;
		}
	    } else {
		strcpy(retval, "*invalid-signalling*");
	    }
	}

	return OK;
    }

    //---------------------
    STATUS API_CALL InterfaceGetSignallingName(char* retval, const char* itfname)
    {
	Interface* itf;
	strcpy(retval, "*bad-interface*");

	if ((itf = MediaManager::GetInstance()->FindInterface(itfname)) != NULL) {
	    SignallingDriver* sig = itf->GetSignallingDriver();
	    if (sig) strcpy(retval, sig->Name().c_str());
	    else strcpy(retval, "*invalid-signalling*");
	}

	return OK;
    }

    //---------------------
    STATUS API_CALL InterfaceGetSignallingGroup(int* retval, const char* itfname)
    {
	Interface* itf;
	*retval = -1;

	if ((itf = MediaManager::GetInstance()->FindInterface(itfname)) != NULL) {
	    SignallingDriver* sig = itf->GetSignallingDriver();
	    if (sig) {
		*retval = (sig->GetType() == SigType_ISUP || sig->GetType() == SigType_BSSAP) ?
		    ((DigitalNetworkInterface*)itf)->GetInterfaceGroup() : -2;
	    } else {
		*retval = -3;
	    }
	}

	return OK;
    }

    //---------------------
    STATUS API_CALL InterfaceCreateDummyCall(CTask* task, int* retval, const char* itfname)
    {
	Interface* itf;
	*retval = -1;

	if ((itf = MediaManager::GetInstance()->FindInterface(itfname)) != NULL) {
	    if (!itf->IsActive()) {
		*retval = CRC_NoCircuit;
		return OK;
	    }

	    DummyCall* call = new DummyCall(itfname);
	    CallParameter cparam;

	    // allocate a media from that interface
	    Media *media;
	    if (!itf->AllocateMedia(call, media, SS_Uniform, CD_Incoming, &cparam)) {
		delete call;
		*retval = CRC_NoCircuit;
		return OK;
	    }

	    // attach media to the call
	    call->MediaAvailableInd(media);

	    // copy call parameters
	    *(call->GetCallParameter()) = cparam;

	    // attach call to the task and allocate a handle
	    *retval = call->AttachTask(task);
	}

	return OK;
    }
};

/*----------------------------------------------------------------* 
 * Implementation of InterfaceManager
 *----------------------------------------------------------------*/

/** InterfaceManager's constructor
 */
InterfaceManager::InterfaceManager(const char *name)
{
    _name = name;
    MediaManager::GetInstance()->RegisterInterfaceManager(this);
}

/** InterfaceManager's destructor
 */
InterfaceManager::~InterfaceManager()
{
    MediaManager::GetInstance()->RemoveInterfaceManager(this);
}

/** Return the name of this object
 */
const char *InterfaceManager::GetName()
{
    return _name.c_str();
}


/*----------------------------------------------------------------* 
 * Implementation of MediaManager
 *----------------------------------------------------------------*/

/** MediaManager's constructor
 */
MediaManager::MediaManager() :
    ServiceProvider("InterfaceMgr",false,true,Lowest_Priority), _lastRIISent(0)
{
    Term_Cmd *cmd = new Term_Cmd(this, Cmd_InterfaceConfig, "interface", "Configure interfaces", PrivilegeLevelAdmin, TermModeConfig, true);
    cmd->Add(new Term_Cmd_Arg("arg1", "", CVariantValidator("", "Interface name"),TermArgOptional));
    RegisterTermCmd(cmd,Kernel::Instance()->RootConfig());


    cmd = new Term_Cmd(this, Cmd_InterfaceNormal, "interface", "Show interfaces status", PrivilegeLevelUser, TermModeNormal, false);
    CVariantChoiceValidator 
	liVdtor("show", "Show detailed status");
    liVdtor.Add("list", "Show short status");
    liVdtor.Add("statistics", "Show statistics");

    ///bennylp:
    /// Removed enable/disable because command 'interface' is accessible to normal
    /// user.
    ///liVdtor.Add("enable", "Enable an interface");
    ///liVdtor.Add("disable", "Disable an interface");

    cmd->Add(new Term_Cmd_Arg("@0", CVariant(""), liVdtor));
    cmd->Add(new Term_Cmd_Arg("arg2", "", CVariantValidator("", "Interface name or empty for all"), TermArgOptional));

    // interface statistics
    cmd->AddDynamic(2, 0, "statistics",new Term_Cmd_Arg("@2", CVariant(-1), CVariantRangeValidator(0, 24, "from (hour) or interval (hour)"), TermArgOptional));
    cmd->AddDynamic(3, 0, "statistics",new Term_Cmd_Arg("@3", CVariant(-1), CVariantRangeValidator(0, 23,   "to (hour)"), TermArgOptional));
    

    RegisterTermCmd(cmd,Kernel::Instance()->RootConfig());

    _interfaceCfgTree = new Cfg_Tree_Group("interface","Interfaces",Kernel::Instance()->RootConfig());
    Kernel::Instance()->RootConfig()->RegisterShowHandler(this);

     _traceSCR = KLog::Instance()->CreateTrace("SCR", "scr", "Successful call ratio warning");
    

    _iter = _listInterface.end();
    _iterRR = _listInterface.end();

    _interfaceAPI = NULL;
}


/** Show interface, in brief/listing format.
 */
void MediaManager::ShowInterfaceBrief ( Term_Session *session, const char *itfName )
{
    try {
	String crlf(NEWLINE);

	session->Send (crlf);
	session->Send(" Name       Group      DD  IC/IB/OC/OB/Idle Sta SCR CircuitRef/Desc." NEWLINE);
	session->Send(" ------------------------------------------------------------------------------" NEWLINE);
	int found = 0;
	for (ListInterface::iterator it = _listInterface.begin(); it!=_listInterface.end(); ++it) {
	    Interface *itf = *it;
	    // TBD: used regex to filter
	    if (strlen(itfName) == 0 || strcmpi(itfName,itf->Name().c_str()) == 0) {
		char counters[64];
		sprintf(counters,
		    "%d/%d/%d/%d/%d",
		    itf->GetCurrentConnectedCount(CD_Incoming),
		    itf->GetCurrentBusyCount(CD_Incoming)-itf->GetCurrentConnectedCount(CD_Incoming),
		    itf->GetCurrentConnectedCount(CD_Outgoing),
		    itf->GetCurrentBusyCount(CD_Outgoing)-itf->GetCurrentConnectedCount(CD_Outgoing),
		    itf->_mediaCount);
		char *state = "";
		if (itf->IsActive()) {
		    state = "Up ";
		} else {
		    switch (itf->GetDisableCause()) {
		    case DC_Administrative:
			state = "Dis";
			break;
		    case DC_Layer1:
			state = "L1 ";
			break;
		    case DC_Shutdown:
			state = "SiP";
			break;
		    }
		}
		char scr_text[30];
		unsigned scr = itf->GetSCR();
		if (scr >= 0 && scr <= 100) {
		    sprintf(scr_text,"%3u",scr);
		} else {
		    strcpy(scr_text,"   ");
		}
		String crefdesc = itf->GetCircuitReference();
		if (crefdesc.length() && itf->GetDescription().length()) {
		    if (crefdesc.length() + itf->GetDescription().length() < 28) {
			crefdesc += "  ";
		    } else {
			crefdesc += NEWLINE;
			crefdesc.append(52, ' ');
		    }
		}
		crefdesc += itf->GetDescription();
		session->Sendf(" %-10s %-10s %c%c %17s %s %s %s" NEWLINE,
		    itf->Name().c_str(),
		    itf->GetGroupName(),
		    itf->GetDirection(),
		    itf->GetDistribution(),
		    counters,
		    state,
		    scr_text,
		    crefdesc.c_str());
		found++;
	    }
	}
	if (!found && strlen(itfName)) session->SendError("There is no interface with such name" NEWLINE);
	else session->Sendf(NEWLINE);
    } catch (...) {
	CLog::Fatal ( THISMODULE, "Caught exception when calling interface list");
    }
}

/** Show interface statistics, in brief/listing format.
 */
void MediaManager::ShowInterfaceStatistics ( Term_Session *session, const char *itfName, int hour_start, int hour_end)
{
    char title[100];
    
    if (hour_start == -1)
	hour_start = 24;

    time_t ltime;
    time(&ltime);
    struct tm* now = localtime(&ltime);
    int hour_now = now->tm_hour;
    now->tm_min = 0;
    now->tm_sec = 0;
    ltime = mktime(now);
    time_t start_t, end_t;

    if (hour_end == -1) {
	if (hour_start == 0) return;
	start_t = ltime - (hour_start-1)*3600;
	end_t = ltime + 3600;
	sprintf(title, "for the last %d hour", hour_start);
    }
    else {
	if (hour_start == 24) hour_start = 0;
	assert (hour_end > 0 || hour_end < 24);

	now->tm_hour = hour_start;
	start_t = mktime(now);
	now->tm_hour = hour_end;
	end_t = mktime(now);

	// validate hour
	if (hour_end <= hour_now) {
	    if (hour_end < hour_start) {
		if (hour_now < hour_start) {
		    start_t -= 24*3600;
		}
		else {
		    start_t = ltime - 23*3600;
		}
	    }
	}
	else {
	    end_t -= 24*3600;
	    if (hour_now <= hour_start) {
		if (hour_end <= hour_start) end_t = ltime + 3600;
		start_t -= 24*3600;
	    }
	    else {
		end_t = ltime + 3600;
	    }
	}
	if (start_t == end_t) return;

	String start_s = ctime(&start_t);
	String end_s;
	ltime = time(NULL);
	if (end_t > ltime) {
	    end_s = ctime(&ltime);
	}
	else 
	    end_s = ctime(&end_t);
	if (start_s.size()) start_s.erase( start_s.size()-1, 1 );
	if (end_s.size()) end_s.erase( end_s.size()-1, 1 );
	sprintf(title, "from %s to %s", start_s.c_str(), end_s.c_str());
    }
    
    try {
	session->Sendf("Call statistics %s\n", title);
	session->Sendf("Name        Calls Success  Failed Connect  SCR  ASR Avg.Cnk Avg.PDD\n");
	session->Send("-------------------------------------------------------------------------------" NEWLINE);


	int found = 0;
	bool showAll = strcmpi(itfName, "all") == 0;
	for (ListInterface::iterator it = _listInterface.begin(); it!=_listInterface.end(); ++it) {
	    Interface *itf = *it;
	    if (showAll || strlen(itfName) == 0 || strcmpi(itfName,itf->Name().c_str()) == 0) {
		itf->PrintData(session, start_t, end_t, 3);
	    }
	}

    } catch (...) {
	CLog::Fatal ( THISMODULE, "Caught exception when calling interface statistics");
    }
}

/**
 Terminal command callback
 */
void MediaManager::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd) 
{
    ACQUIRE_SYSTEM_LOCK();

    switch (cmd->GetId()) {
    case Cmd_InterfaceConfig:
	{
	    CVariant v;
	    cmd->GetArg((unsigned)0)->GetValue(v);
	    const char *itfName = v.AsString();

	    // if interface name is specified, find that specific interface
	    if (strlen(itfName)) {
		Interface *itf = FindInterface(itfName);
		if (itf) {
		    if (!no) {
			session->SetRoot(itf->GetConfigTree());
		    } else {
			if (itf->SupportDynamicDeletion()) {
			    RELEASE_SYSTEM_LOCK();
			    if (!session->GetStream().IsInteractive() ||
				session->PromptYesNo(false, "Delete interface '%s' (y/n) ?", itfName)) {
				ACQUIRE_SYSTEM_LOCK();
				// Check again whether the interface can be deleted.
				if (itf->SupportDynamicDeletion()) {
				    Cfg_Tree_Group* cfg = itf->GetConfigTree();
				    Config_Controller::Instance()->NotifyEntryDeleted(session,cfg);
				    itf->Unload();
				    itf->DestroyInterface();
				}
				RELEASE_SYSTEM_LOCK();
			    }
			    ACQUIRE_SYSTEM_LOCK();
			} else {
			    session->SendErrorf("Can not delete interface %s" NEWLINE,itfName);
			}
		    }
		} else {
		    ListManager::iterator it=_listManager.end();

		    if (!no) {
			for (it=_listManager.begin();it!=_listManager.end();it++) {
			    InterfaceManager *mgr = *it;

//			     cout << "****** Inside MediaManager, CreateInterface() ****" << endl;

			    // can this interface dynamically created?
			    if (mgr->CanCreateInterface(itfName)) {
//			     	cout << "****** Inside MediaManager, Interface can be created  ****" << endl;
				RELEASE_SYSTEM_LOCK();
				if (!session->GetStream().IsInteractive() ||
				    session->PromptYesNo(false, "Create a new interface '%s' (y/n) ?", itfName)) {
					ACQUIRE_SYSTEM_LOCK();
					itf = mgr->CreateInterface(itfName);
//			     		cout << "****** Inside MediaManager, Loading the Interface ****" << endl;
					itf->Load();
					assert(itf);
					session->SetRoot(itf->GetConfigTree());
					RELEASE_SYSTEM_LOCK();
				}
				ACQUIRE_SYSTEM_LOCK();
				break;
			    }
			}
		    }
		    if (it == _listManager.end()) session->SendError("There is no interface with that name" NEWLINE);
		}
	    } else {
		session->Sendf("Available interfaces:" NEWLINE);
		for (ListInterface::iterator it = _listInterface.begin();it != _listInterface.end();it++) {
		    Interface *itf = *it;
		    session->Sendf("  %s" NEWLINE,itf->Name().c_str());
		}
	    }
	}
	break;

    case Cmd_InterfaceNormal:
	{
	    CVariant v1,v2;
	    cmd->GetArg((unsigned)0)->GetValue(v1);
	    cmd->GetArg((unsigned)1)->GetValue(v2);
	    const char *subcmd = v1.AsString();
	    const char *itfName = v2.AsString();

	    if (strcmpi(subcmd,"show") == 0) {
		RELEASE_SYSTEM_LOCK();

		// TBD: fix this!
		// We use 'try' block to prevent crashing when interfaces is deleted when 
		// we're calling their method. Alternatively, we can output the
		// ShowInterface() to a string, so that we can acquire system lock while
		// creating the output.
		try {
		    String crlf(NEWLINE);

		    int found = 0;
		    for (ListInterface::iterator it = _listInterface.begin(); it!=_listInterface.end(); ++it) {
			Interface *itf = *it;
			if (strlen(itfName) == 0 || strcmpi(itfName,itf->Name().c_str()) == 0) {
			    itf->ShowInterface(session);
			    session->Send (crlf);
			    found++;
			}
		    }
		    if (!found && strlen(itfName)) session->SendError("There is no interface with such name" NEWLINE);
		} catch (...) {
		    CLog::Fatal ( THISMODULE, "Caught exception when calling interface show");
		}
		ACQUIRE_SYSTEM_LOCK();
	    } else if (strcmpi(subcmd,"list") == 0) {
		RELEASE_SYSTEM_LOCK();
		ShowInterfaceBrief(session, itfName);
		ACQUIRE_SYSTEM_LOCK();
	    } else if (strcmpi(subcmd,"statistics") == 0) {
		CVariant v3,v4;
		cmd->GetArg((unsigned)2)->GetValue(v3);
		cmd->GetArg((unsigned)3)->GetValue(v4);
		int start_hour = v3.AsInt();
		int end_hour = v4.AsInt();
		RELEASE_SYSTEM_LOCK();
		ShowInterfaceStatistics(session, itfName, start_hour, end_hour);
		ACQUIRE_SYSTEM_LOCK();

/* Bennylp: removed command "interface enable/disable", because "interface" command
   is accessible for normal user.

	    } else if (strcmpi(subcmd,"enable") == 0) {
		if (strlen(itfName) == 0) {
		    session->SendError("You must specify an interface name" NEWLINE);
		} else {
		    try {
			for (ListInterface::iterator it = _listInterface.begin(); it!=_listInterface.end(); ++it) {
			    Interface *itf = *it;
			    if (strcmpi(itfName,itf->Name().c_str()) == 0) {
				itf->Enable();
				break;
			    }
			}
		    } catch (...) {
			CLog::Fatal ( THISMODULE, "Caught exception when calling interface enable");
		    }
		}
	    } else if (strcmpi(subcmd,"disable") == 0) {
		if (strlen(itfName) == 0) {
		    session->SendError("You must specify an interface name" NEWLINE);
		} else {
		    try {
			for (ListInterface::iterator it = _listInterface.begin(); it!=_listInterface.end(); ++it) {
			    Interface *itf = *it;
			    if (strcmpi(itfName,itf->Name().c_str()) == 0) {
				itf->Disable();
				break;
			    }
			}
		    } catch (...) {
			CLog::Fatal ( THISMODULE, "Caught exception when calling interface disable");
		    }
		}
*/
	    } else {
		session->SendError("Unimplemented or unsupported command" NEWLINE);
	    }
	}
	break;
    }

    RELEASE_SYSTEM_LOCK();
}

/**
 Show configuration for the specified entry.
 This function will be called by the framework whenever it needs to display the
 specified entry. Previously this instance should tell the entry that it wishes to
 handle displaying the entry by calling entry->RegisterShowHandler().

 When this instance is deleted, it should call entry->UnregisterShowHandler()
 so that the handler will not be called (which will cause GPF).
 */
/* STATUS MediaManager::OnShowConfig(const Cfg_Tree_Entry *entry,String & output, const String & indent, int indent_size) const */
bool MediaManager::OnShowConfig(const Cfg_Tree_Entry *entry,String & output, const String & indent, int indent_size) const
{
    const String crlf = NEWLINE;
    String new_indent = indent;
    new_indent.append((size_t)indent_size, ' ');

    for (ListInterface::const_iterator it = _listInterface.begin();it != _listInterface.end();it++) {
	const Interface *itf = *it;
	if (!itf->_dynamic) {
	    output += indent + "interface " + itf->Name() + crlf;
	    itf->OnShowConfig (entry, output, new_indent, indent_size);
	    output += new_indent + "top" + crlf;
	    output += crlf;
	}
    }
    return OK;
}

/** Show all interface information.
 */
void MediaManager::ShowInterface ( Term_Session *session, const String & selector )
{
    ListInterface::iterator it, end;

    // TBD: fix this!
    // We use 'try' block to prevent crashing when interfaces is deleted when 
    // we're calling their method. Alternatively, we can output the
    // ShowInterface() to a string, so that we can acquire system lock while
    // creating the output.
    regex_t reg;
    memset(&reg, 0, sizeof(reg));

    try {
	String crlf(NEWLINE);
	bool brief = false;
	int ret;
	
	if (selector.empty()) {
	    ret = regcomp(&reg, ".*", REG_ICASE|REG_EXTENDED);
	} else if (!stricmp(selector.c_str(), "brief")) {
	    ShowInterfaceBrief(session, "");
	    return;
	} else {
	    ret = regcomp(&reg, selector.c_str(), REG_ICASE|REG_EXTENDED);
	}

	if (ret) {
	    session->SendErrorf("Invalid regular expression '%s'\n", selector.c_str());
	    return;
	}

	for (it = _listInterface.begin(); it!=_listInterface.end(); ++it) {
	    Interface *itf = *it;
	    if (regexec(&reg, itf->Name().c_str(), 0, NULL, 0) == 0) {
		itf->ShowInterface (session);
		session->Send (crlf);
	    }
	}
    } catch (...) {
	CLog::Fatal ( THISMODULE, "Caught exception when calling ShowInterface");
    }

    regfree(&reg);
}

/** Destructor
 */
MediaManager::~MediaManager()
{
    for (ListInterface::iterator it = _listInterface.begin();it != _listInterface.end();) {
	Interface *itf = *it++;
	delete itf;
    }
    _listInterface.erase(_listInterface.begin(),_listInterface.end());
    //Kernel::Instance()->RootConfig()->UnregisterShowHandler(this);
    //KLog::Instance()->DestroyTrace (rtpSigTrace);

    delete _traceSCR;
}

/** HandleEvents: Descendent shall implement this abstract method.
    It is called by Service Provider Manager
 */
void MediaManager::HandleEvents()
{
    ListInterface::iterator end = _listInterface.end();
    //RapManager* rapmgr = RapManager::Instance();
    //if (!rapmgr) return;

    if (time(NULL) != _lastRIISent) {
	_lastRIISent = time(NULL);
	for (ListInterface::iterator it = _listInterface.begin();it != end;it++) {
	    Interface *itf = *it;
	    if (itf->_mediaCountChanged) {
#if 0
		if (itf->_useRap) {
		    RapParam::RII param(
			itf->Name(),
			itf->_mediaCount,
			(itf->_enabled && (itf->GetState()==OS_InService)) ? RapParam::Rs_Enabled : RapParam::Rs_Disabled);
		    rapmgr->RII(param, itf->GetResourceType());	
		}
#endif
		itf->_mediaCountChanged = false;
	    }
	}
    }
}

/** GetInstance: Returns a pointer to this singleton instance, if necessary
    create one instance first before returning a pointer to it
 */
MediaManager *MediaManager::GetInstance()
{
    static MediaManager* theMediaManager = NULL;

    if (!theMediaManager) theMediaManager = new MediaManager;
    return theMediaManager;
}

/** RootConfig: Returns a pointer to the root configuration tree of
    interface sub commands
 */
Cfg_Tree_Group *MediaManager::GetConfigTree()
{
    return _interfaceCfgTree;
}

/** Sends a SCR warning trace
 */
void MediaManager::TraceSCR(const char* msg)
{
    KLog::Instance()->TraceStandard(_traceSCR,msg);
}

/** A new Interface object is created
 */
void MediaManager::InsertInterface(Interface *itf)
{
    _listInterface.push_back(itf);
    _iterRR = _listInterface.begin();
}

/** A new Interface object is destroyed
 */
void MediaManager::RemoveInterface(Interface *itf)
{
    _listInterface.remove(itf);
    _iterRR = _listInterface.begin();
}

/** Called to register an interface manager
 */
void MediaManager::RegisterInterfaceManager(InterfaceManager *mgr)
{
    _listManager.push_back(mgr);
}

/** Called to deregister an interface manager
 */
void MediaManager::RemoveInterfaceManager(InterfaceManager *mgr)
{
    _listManager.remove(mgr);
}

bool MediaManager::Init()
{
    for (ListInterface::iterator it = _listInterface.begin();it != _listInterface.end();it++) {
	Interface *itf = *it;
	itf->Load();
    }
    for (it = _listInterface.begin();it != _listInterface.end();it++) {
	Interface *itf = *it;
	itf->Start();
    }

    return true;
}

bool MediaManager::Deinit()
{
    for (ListInterface::iterator it = _listInterface.begin();it != _listInterface.end();it++) {
	Interface *itf = *it;
	itf->Stop();
    }
    for (it = _listInterface.begin();it != _listInterface.end();it++) {
	Interface *itf = *it;
	itf->Unload();
    }
    KLog::Instance()->DestroyTrace(_traceSCR);
    return true;
}

/** Load: Called once at initialization time
 */
bool MediaManager::Load() {
    NEW_RETURN_(_interfaceAPI, InterfaceAPI);
    if (!_interfaceAPI) return false;
    return true;
}

/** Unload: Called once at termination time
 */
bool MediaManager::Unload() {
    delete _interfaceAPI;
    return true;
}


Interface *MediaManager::FindInterface(const char *name)
{
    for (ListInterface::iterator it = _listInterface.begin();it != _listInterface.end();it++) {
	if (strcmpi(name,(*it)->Name().c_str()) == 0) return *it;
    }
    return NULL;
}

Interface *MediaManager::ItGetFirstInterface()
{
    _iter = _listInterface.begin();
    if (_iter != _listInterface.end()) return *_iter++;
    else return NULL;
}

Interface *MediaManager::ItGetNextInterface()
{
    if (_iter != _listInterface.end()) return *_iter++;
    else return NULL;
}

Interface *MediaManager::ItGetInterfaceRoundRobin()
{
    ListInterface::iterator iter = _iterRR++;
    if (_iterRR == _listInterface.end()) _iterRR = _listInterface.begin();

    if (iter != _listInterface.end()) return *iter;
    else return NULL;
}

unsigned MediaManager::GetInterfaceCount()
{
    return _listInterface.size();
}

