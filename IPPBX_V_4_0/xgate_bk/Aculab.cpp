/*----------------------------------------------------------------* 
 * $Archive: /xGate/Aculab.cpp $
 * $Date: 23/04/03 12:24 $
 * $Revision: 105 $
 *
 * $History: Aculab.cpp $
 * 
 * *****************  Version 105  *****************
 * User: Junanto      Date: 23/04/03   Time: 12:24
 * Updated in $/xGate
 * added FCI and NCI settings in Aculab based SS7
 * 
 * *****************  Version 104  *****************
 * User: Junanto      Date: 22/04/03   Time: 16:10
 * Updated in $/xGate
 * added COT support for Aculab SS7
 * added detail to callparam translation for SS7
 * 
 * *****************  Version 103  *****************
 * User: Bennylp      Date: 4/09/03    Time: 11:03a
 * Updated in $/xGate
 * Added interface type for interface (for SNMP purpose)
 * 
 * *****************  Version 102  *****************
 * User: Bennylp      Date: 10/22/02   Time: 5:14p
 * Updated in $/xGate
 * Keep track of thread creation ("show threads")
 * 
 * *****************  Version 101  *****************
 * User: Junanto      Date: 3/07/02    Time: 18:15
 * Updated in $/xGate
 * fixed when CLI was not properly retrieved in CAS
 * 
 * *****************  Version 100  *****************
 * User: Junanto      Date: 1/07/02    Time: 10:35
 * Updated in $/xGate
 * 
 * *****************  Version 99  *****************
 * User: Junanto      Date: 25/06/02   Time: 14:58
 * Updated in $/xGate
 * Fixed when IDLE was received without prior incoming SETUP 
 * (an incoming call has been missed) and caused subsequent
 * calls to this timeslot to fail
 * 
 * *****************  Version 98  *****************
 * User: Junanto      Date: 5/06/02    Time: 21:58
 * Updated in $/xGate
 * Modified behaviour for CAS protocol:
 * - fixed bug in EOS detection
 * - delayed SetupCnf() to prevent early switching
 * 
 * *****************  Version 97  *****************
 * User: Junanto      Date: 15/05/02   Time: 10:27
 * Updated in $/xGate
 * Implemented OnClearCounter
 * 
 * *****************  Version 96  *****************
 * User: Junanto      Date: 15/05/02   Time: 10:22
 * Updated in $/xGate
 * Small modification in display during layer 1 transition
 * 
 * *****************  Version 95  *****************
 * User: Junanto      Date: 5/02/02    Time: 8:00p
 * Updated in $/xGate
 * 
 * *****************  Version 94  *****************
 * User: Junanto      Date: 2/05/02    Time: 19:51
 * Updated in $/xGate
 * 
 * *****************  Version 93  *****************
 * User: Junanto      Date: 29/04/02   Time: 17:20
 * Updated in $/xGate
 * 
 * *****************  Version 92  *****************
 * User: Junanto      Date: 29/04/02   Time: 14:21
 * Updated in $/xGate
 * version 38.22: added protection in Aculab when IDLE events may be
 * received more than once
 * 
 * *****************  Version 91  *****************
 * User: Junanto      Date: 18/03/02   Time: 13:28
 * Updated in $/xGate
 * 
 * *****************  Version 90  *****************
 * User: Junanto      Date: 16/03/02   Time: 10:50
 * Updated in $/xGate
 * version 38.15
 * 
 * *****************  Version 89  *****************
 * User: Junanto      Date: 19/02/02   Time: 11:32
 * Updated in $/xGate
 * changed implementation for reload command
 * 
 * *****************  Version 88  *****************
 * User: Junanto      Date: 31/01/02   Time: 18:44
 * Updated in $/xGate
 * Original switching mode is memorized and put back
 * during idle time
 * 
 * *****************  Version 87  *****************
 * User: Junanto      Date: 6/11/01    Time: 19:48
 * Updated in $/xGate
 * removed automatic alarm on startup
 * 
 * *****************  Version 86  *****************
 * User: Junanto      Date: 31/10/01   Time: 16:48
 * Updated in $/xGate
 * 
 * *****************  Version 85  *****************
 * User: Junanto      Date: 31/10/01   Time: 16:42
 * Updated in $/xGate
 * Added layer 2 monitoring - to prevent calls from being made 
 * when signalling is down
 * 
 * *****************  Version 84  *****************
 * User: Junanto      Date: 24/10/01   Time: 20:25
 * Updated in $/xGate
 * Fixed a little bug in AculabFactory::HandleEvents()
 * 
 * *****************  Version 83  *****************
 * User: Iwanj        Date: 23/10/01   Time: 11:52
 * Updated in $/xGate
 * Perform call_event when there is at least one physical
 * interface attached
 * 
 * *****************  Version 82  *****************
 * User: Junanto      Date: 22/10/01   Time: 19:14
 * Updated in $/xGate
 * event list cleaned up on unload
 * 
 * *****************  Version 81  *****************
 * User: Junanto      Date: 22/10/01   Time: 15:08
 * Updated in $/xGate
 * Event poller is moved to a separate thread to
 * increase main thread performance
 * 
 * *****************  Version 80  *****************
 * User: Junanto      Date: 19/10/01   Time: 16:45
 * Updated in $/xGate
 * 
 * *****************  Version 79  *****************
 * User: Junanto      Date: 19/10/01   Time: 16:45
 * Updated in $/xGate
 * One service provider (AculabItf) represents
 * all layer 1 checker
 * 
 * *****************  Version 78  *****************
 * User: Junanto      Date: 15/10/01   Time: 18:42
 * Updated in $/xGate
 * Added idle_net_ts on outgoing call
 * 
 * *****************  Version 77  *****************
 * User: Bennylp      Date: 10/11/01   Time: 5:50p
 * Updated in $/xGate
 * Added idle_net_ts
 * 
 * *****************  Version 76  *****************
 * User: Junanto      Date: 23/08/01   Time: 20:32
 * Updated in $/xGate
 * Changed implementation of Dec/IncMediaCount
 * during blocked condition
 * 
 * *****************  Version 75  *****************
 * User: Junanto      Date: 20/08/01   Time: 19:00
 * Updated in $/xGate
 * Media can be allocated even if blocked in incoming call
 * 
 * *****************  Version 74  *****************
 * User: Junanto      Date: 16/08/01   Time: 16:33
 * Updated in $/xGate
 * Make sure that all timeslot are unblocked when
 * attached to Aculab signalling
 * 
 * *****************  Version 73  *****************
 * User: Junanto      Date: 9/08/01    Time: 19:57
 * Updated in $/xGate
 * - Call distribution method is used to select timeslot
 * - Corrected behaviour when signalling is changed
 * 
 * *****************  Version 72  *****************
 * User: Junanto      Date: 6/08/01    Time: 19:42
 * Updated in $/xGate
 * Added cause codes mapping in the case where raw cause code 
 * can't be obtained (protocol others than Q-931 based)
 * 
 * *****************  Version 71  *****************
 * User: Junanto      Date: 25/07/01   Time: 17:49
 * Updated in $/xGate
 * Delayed signalling driver settings until all configuration
 * file is read (during CommitConfiguration)
 * 
 * *****************  Version 70  *****************
 * User: Junanto      Date: 25/07/01   Time: 16:44
 * Updated in $/xGate
 * call_event is called at most 8 times per HandleEvent
 * 
 * *****************  Version 69  *****************
 * User: Junanto      Date: 23/07/01   Time: 22:13
 * Updated in $/xGate
 * Changed priority to lower
 * 
 * *****************  Version 68  *****************
 * User: Junanto      Date: 23/07/01   Time: 11:14
 * Updated in $/xGate
 * Fixed case when interface is disabled when assigned to ISUP
 * 
 * *****************  Version 67  *****************
 * User: Bennylp      Date: 7/19/01    Time: 2:17p
 * Updated in $/xGate
 * Changes to MediaManager functions to support rtp/aud shared stats.
 * 
 * *****************  Version 66  *****************
 * User: Junanto      Date: 18/07/01   Time: 18:59
 * Updated in $/xGate
 * Added support for ISUP circuit/group blocking/unblocking
 * 
 * *****************  Version 65  *****************
 * User: Junanto      Date: 16/07/01   Time: 17:40
 * Updated in $/xGate
 * Changed priority from Normal to Low
 * 
 * *****************  Version 64  *****************
 * User: Bennylp      Date: 7/14/01    Time: 5:23p
 * Updated in $/xGate
 * Framework for signalling driver configuration in SigDrvManager.
 * 
 * *****************  Version 63  *****************
 * User: Junanto      Date: 10/07/01   Time: 19:06
 * Updated in $/xGate
 * Changed interface of Call class constructor
 * 
 * *****************  Version 62  *****************
 * User: Junanto      Date: 29/06/01   Time: 22:44
 * Updated in $/xGate
 * Fixed bug during receipt of spurious EV_IDLE
 * CallSetup on ISUP always results in SetupCnf (overlap)
 * 
 * *****************  Version 61  *****************
 * User: Junanto      Date: 28/06/01   Time: 20:41
 * Updated in $/xGate
 * 
 * *****************  Version 60  *****************
 * User: Junanto      Date: 27/06/01   Time: 20:53
 * Updated in $/xGate
 * Added support for timeslot blocking
 * ISUP maintenance command can be issued from xGate
 * 
 * *****************  Version 59  *****************
 * User: Junanto      Date: 27/06/01   Time: 17:41
 * Updated in $/xGate
 * Changed media naming convention
 * 
 * *****************  Version 58  *****************
 * User: Junanto      Date: 26/06/01   Time: 17:27
 * Updated in $/xGate
 * 
 * *****************  Version 57  *****************
 * User: Junanto      Date: 26/06/01   Time: 17:15
 * Updated in $/xGate
 * Fixed a small bug
 * 
 * *****************  Version 56  *****************
 * User: Junanto      Date: 26/06/01   Time: 10:53
 * Updated in $/xGate
 * 
 * *****************  Version 55  *****************
 * User: Junanto      Date: 25/06/01   Time: 16:46
 * Updated in $/xGate
 * AttachInterface changed to bool
 * First attempt to integrate with ISUP signalling
 * group command can only be used without resource locator
 * 
 * *****************  Version 54  *****************
 * User: Bennylp      Date: 6/22/01    Time: 11:02p
 * Updated in $/xGate
 * Changed some members to const, and others to non virtual, etc.
 * 
 * *****************  Version 53  *****************
 * User: Junanto      Date: 22/06/01   Time: 11:38
 * Updated in $/xGate
 * Fixed bug during "disable" while there are
 * active calls on that interface
 * 
 * *****************  Version 52  *****************
 * User: Junanto      Date: 19/06/01   Time: 16:07
 * Updated in $/xGate
 * Tested "reload" firmware functions
 * 
 * *****************  Version 51  *****************
 * User: Junanto      Date: 8/06/01    Time: 10:37
 * Updated in $/xGate
 * Removed some CLog::Detail
 * 
 * *****************  Version 50  *****************
 * User: Junanto      Date: 7/06/01    Time: 22:33
 * Updated in $/xGate
 * 
 * *****************  Version 49  *****************
 * User: Junanto      Date: 6/06/01    Time: 15:54
 * Updated in $/xGate
 * OnShutdown has a new parameter which indicates
 * that a requested shutdown has been cancelled
 * 
 * *****************  Version 48  *****************
 * User: Junanto      Date: 6/06/01    Time: 10:38
 * Updated in $/xGate
 * - Added circuit reference command
 * - SETUP_ACK is sent on receipt of the first EV_DETAILS
 * - CompandingLaw default to A-law
 * 
 * *****************  Version 47  *****************
 * User: Junanto      Date: 3/06/01    Time: 21:05
 * Updated in $/xGate
 * Added OnShutdown handler -> transmit RRA
 * 
 * *****************  Version 46  *****************
 * User: Junanto      Date: 2/06/01    Time: 22:46
 * Updated in $/xGate
 * DAC changed to dac
 * 
 * *****************  Version 45  *****************
 * User: Junanto      Date: 2/06/01    Time: 16:56
 * Updated in $/xGate
 * Fixed layer 1 problem reporting
 * 
 * *****************  Version 44  *****************
 * User: Junanto      Date: 2/06/01    Time: 14:36
 * Updated in $/xGate
 * Aculab reset statistics counter on start up
 * 
 * *****************  Version 43  *****************
 * User: Junanto      Date: 30/05/01   Time: 18:32
 * Updated in $/xGate
 * 
 * *****************  Version 42  *****************
 * User: Junanto      Date: 29/05/01   Time: 18:30
 * Updated in $/xGate
 * Changed HandleEvents behaviour (loop only once)
 * 
 * *****************  Version 41  *****************
 * User: Junanto      Date: 29/05/01   Time: 0:29
 * Updated in $/xGate
 * 
 * *****************  Version 40  *****************
 * User: Bennylp      Date: 5/26/01    Time: 8:04p
 * Updated in $/xGate
 * Record each call to AcquireSystemLock in DEBUG mode.
 * 
 * *****************  Version 39  *****************
 * User: Junanto      Date: 26/05/01   Time: 17:05
 * Updated in $/xGate
 * Changed to AculabSignalling to lower priority
 * 
 * *****************  Version 38  *****************
 * User: Junanto      Date: 25/05/01   Time: 21:08
 * Updated in $/xGate
 * Fixed sub address parameter
 * 
 * *****************  Version 37  *****************
 * User: Junanto      Date: 23/05/01   Time: 17:15
 * Updated in $/xGate
 * 
 * *****************  Version 36  *****************
 * User: Junanto      Date: 21/05/01   Time: 15:54
 * Updated in $/xGate
 * Added call holding and connected duration (in secs)
 * 
 * *****************  Version 35  *****************
 * User: Junanto      Date: 21/05/01   Time: 10:26
 * Updated in $/xGate
 * Alarm put back to the previous settings
 * after reload
 * 
 * *****************  Version 34  *****************
 * User: Junanto      Date: 19/05/01   Time: 21:34
 * Updated in $/xGate
 * Added support for no fwfile/fwargs/dspfile
 * Fixed bug in reload
 * 
 * *****************  Version 33  *****************
 * User: Junanto      Date: 17/05/01   Time: 18:20
 * Updated in $/xGate
 * Added regular layer 1 checking
 * 
 * *****************  Version 32  *****************
 * User: Junanto      Date: 10/05/01   Time: 13:21
 * Updated in $/xGate
 * 
 * *****************  Version 31  *****************
 * User: Junanto      Date: 10/05/01   Time: 12:40
 * Updated in $/xGate
 * Replaced "\r\n" sequence to "\n". Telnet translates
 * an orphaned "\n" into "\r\n" during output
 * 
 * *****************  Version 30  *****************
 * User: Junanto      Date: 10/05/01   Time: 11:12
 * Updated in $/xGate
 * Replaced all "\r\n" with a constant "\r\n"
 * 
 * *****************  Version 29  *****************
 * User: Junanto      Date: 8/05/01    Time: 13:11
 * Updated in $/xGate
 * Delayed transition to running state
 * 
 * *****************  Version 28  *****************
 * User: Junanto      Date: 4/05/01    Time: 16:53
 * Updated in $/xGate
 * Changed RAP method and incoming calls
 * are now enabled/disabled during attach/detach
 * interface
 * 
 * *****************  Version 27  *****************
 * User: Junanto      Date: 4/05/01    Time: 13:23
 * Updated in $/xGate
 * Changed places for SystemLock() calls
 * 
 * *****************  Version 26  *****************
 * User: Junanto      Date: 2/05/01    Time: 12:50
 * Updated in $/xGate
 * Changed show interface output
 * 
 * *****************  Version 25  *****************
 * User: Bennylp      Date: 5/02/01    Time: 12:02p
 * Updated in $/xGate
 * Added show interface framework.
 * 
 * *****************  Version 24  *****************
 * User: Junanto      Date: 30/04/01   Time: 19:16
 * Updated in $/xGate
 * Disable event handling on absence of 
 * attached interface
 * 
 * *****************  Version 23  *****************
 * User: Junanto      Date: 30/04/01   Time: 18:50
 * Updated in $/xGate
 * minor changes
 * 
 * *****************  Version 22  *****************
 * User: Junanto      Date: 30/04/01   Time: 15:08
 * Updated in $/xGate
 * 
 * *****************  Version 21  *****************
 * User: Junanto      Date: 30/04/01   Time: 15:00
 * Updated in $/xGate
 * changed command name from 
 * tslot to timeslot
 * 
 * *****************  Version 20  *****************
 * User: Junanto      Date: 30/04/01   Time: 14:54
 * Updated in $/xGate
 * changed some log display
 * 
 * *****************  Version 19  *****************
 * User: Junanto      Date: 29/04/01   Time: 17:34
 * Updated in $/xGate
 * AculabCall declaration moved to .cpp
 * 
 * *****************  Version 18  *****************
 * User: Junanto      Date: 24/04/01   Time: 20:50
 * Updated in $/xGate
 * fixed bug in AttachSignalling
 * 
 * *****************  Version 17  *****************
 * User: Junanto      Date: 24/04/01   Time: 18:26
 * Updated in $/xGate
 * first link between Prosody voice / Aculab media with
 * switching framework
 * 
 * *****************  Version 16  *****************
 * User: Junanto      Date: 23/04/01   Time: 16:48
 * Updated in $/xGate
 * added overlap indication for incoming calls
 * 
 * *****************  Version 15  *****************
 * User: Junanto      Date: 21/04/01   Time: 23:05
 * Updated in $/xGate
 * 
 * *****************  Version 14  *****************
 * User: Junanto      Date: 20/04/01   Time: 18:24
 * Updated in $/xGate
 * 
 * *****************  Version 13  *****************
 * User: Junanto      Date: 19/04/01   Time: 17:55
 * Updated in $/ct15
 * 
 * *****************  Version 12  *****************
 * User: Junanto      Date: 18/04/01   Time: 19:54
 * Updated in $/ct15
 * 
 * *****************  Version 11  *****************
 * User: Bennylp      Date: 4/18/01    Time: 3:38p
 * Updated in $/ct15
 * Change Load() to always return true.
 * 
 * *****************  Version 10  *****************
 * User: Junanto      Date: 18/04/01   Time: 11:49
 * Updated in $/ct15
 * 
 * *****************  Version 9  *****************
 * User: Junanto      Date: 15/04/01   Time: 20:48
 * Updated in $/ct15
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 14/04/01   Time: 19:59
 * Updated in $/ct15
 * Add more APIs
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 13/04/01   Time: 22:37
 * Updated in $/ct15
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 13/04/01   Time: 21:03
 * Updated in $/ct15
 * 1st version of integration with call manager
 * 
 * *****************  Version 5  *****************
 * User: Bennylp      Date: 4/12/01    Time: 8:59p
 * Updated in $/ct15
 * Integration and some bug fixes.
 * 
 * *****************  Version 4  *****************
 * User: Junanto      Date: 12/04/01   Time: 19:54
 * Updated in $/ct15
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
#include "stdafx.h"
#include "xgconfig.h"
#ifdef XGATE_HAS_ACULAB

#include "Aculab.h"
#include "kernel.h"
#include "term.h"
#include "log.h"
#include "licence.h"
#include "thread_process.h"

#include "acu_type.h"
#include "sw_lib.h"
#include "cl_lib.h"
#include "res_lib.h"

#include "Isup.h"



//-----------------------------------------------------------------
static char __modname__[] = "Aculab";
#define THISMODULE	    __modname__
#define NEWLINE		    "\n"
#define INDENT		    SHOW_INTERFACE_INDENT

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define strcmpi strcasecmp

void Sleep( unsigned long ms )
{
#ifndef __linux__
    ::Sleep(ms);
#else
    poll(0,0,ms); // __linux__
#endif
};


//-----------------------------------------------------------------
#define LAYER1_CHECK_INTERVAL			15
#define WATCHDOG_TIMEOUT			60


//-----------------------------------------------------------------

/** Singleton instances
 */
AculabSignalling* AculabSignalling::theAculabSignalling;
AculabFactory* AculabFactory::theAculabFactory;


extern void SetLockingLocation(const char* where);

static unsigned long GetTickCount()
{
  struct timeval tv;
  if( gettimeofday(&tv, NULL) != 0 )
  return 0;

  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}


/*----------------------------------------------------------------* 
 * Internal classes
 *----------------------------------------------------------------*/

/** AculabCall: Specific implementation of Call for Aculab Signalling Driver
 */
class AculabCall : public Call
{
protected:
    friend class AculabSignalling;
    int		_callHandle;
    int		_port;
    bool	_cot_in_progress;	    // ISUP continuity test

    void SetCallHandle(int callHandle);
    void SetCallParameterFromDetail(struct detail_xparms*);

    /** Conversion/mapping functions
     */
    int XlatToDeviceIsupNOA(CallNumberType cnt);
    int XlatToDeviceNumberPlan(CallNumberPlan cnp);
    void FillSubaddress(unsigned char *,const char *);

    /** Actual interface to Aculab API
     */
    bool DeviceSetupReq(int port,int timeslot,AculabInterface* itf,int* handle,bool appendEOS);

public:
    /** Default constructor and destructor
     */
    AculabCall(CallDirection dir,int port,Interface* itf);
    virtual ~AculabCall();

    /** Requests: these methods need to be overridden by each Signalling 
	Driver to implement specific behaviour of signalling protocol
     */
    virtual bool DeviceOverlapReq(const char* additionalDigits,bool complete);
    virtual bool DeviceProceedingReq(CallProgress progress, CallExtraInfo* exinfo);
    virtual bool DeviceProgressReq(CallProgress progress, CallExtraInfo* exinfo);
    virtual bool DeviceAlertingReq(CallProgress progress, CallExtraInfo* exinfo);
    virtual bool DeviceConnectReq(CallExtraInfo* exinfo);
    virtual bool DeviceSendInfo(CallExtraInfo* exinfo);
    virtual bool DeviceSuspendReq();
    virtual bool DeviceResumeReq();
    virtual bool DeviceDisconnectReq(CallCause cause,CallProgress progress);

};


/*----------------------------------------------------------------* 
 * Useful helper functions
 *----------------------------------------------------------------*/

/** Map Aculab event (int) to its string representation
 */
static const char* MapEventToString(int event)
{
    static struct {
	int event;
	char* name;
    } xlat[] = {
#define MKSTR(x)	{ x, #x }
	MKSTR(EV_IDLE),
	MKSTR(EV_WAIT_FOR_INCOMING),
	MKSTR(EV_INCOMING_CALL_DET),
	MKSTR(EV_CALL_CONNECTED),
	MKSTR(EV_EMERGENCY_CONNECT),
	MKSTR(EV_TEST_CONNECT),
	MKSTR(EV_WAIT_FOR_OUTGOING),
	MKSTR(EV_OUTGOING_RINGING),
	MKSTR(EV_REMOTE_DISCONNECT),
	MKSTR(EV_WAIT_FOR_ACCEPT),
	MKSTR(EV_HOLD),
	MKSTR(EV_HOLD_REJECT),
	MKSTR(EV_TRANSFER_REJECT),
	MKSTR(EV_RECONNECT_REJECT),
	MKSTR(EV_PROGRESS),
	MKSTR(EV_OUTGOING_PROCEEDING),
	MKSTR(EV_DETAILS),
	MKSTR(EV_EXTENDED),
	MKSTR(EV_NOTIFY),
#undef MKSTR
    };

    for (int i=0;i<sizeof(xlat)/sizeof(xlat[0]);i++) {
	if (event == xlat[i].event) return xlat[i].name+3;
    }
    return "UnknownEvent";
}

/** Map Aculab error codes (int) to its string representation
 */
static const char* MapErrorToString(int rc)
{
    static struct {
	int event;
	char* name;
    } xlat[] = {
	{ ERR_HANDLE, "Illegal handle or out of handles" },
	{ ERR_COMMAND, "Illegal command" },
	{ ERR_NET, "Illegal network number" },
	{ ERR_PARM, "Illegal parameter" },
	{ ERR_RESPONSE, "Error response" },
	{ ERR_NOCALLIP, "No call in progress" },
	{ ERR_TSBAR, "Timeslot barred" },
	{ ERR_TSBUSY, "Timeslot busy" },
	{ ERR_CFAIL, "Command failed to execute" },
	{ ERR_SERVICE, "Invalid service code" },
	{ ERR_BUFF_FAIL, "Out of buffer resources" },
	{ ERR_DNLD_ZAP, "Debug monitor running" },
	{ ERR_DNLD_NOCMD, "Firmware not loaded" },
	{ ERR_DNLD_NODNLD, "Firmware already installed, download denied" },
	{ ERR_DNLD_GEN, "General failure during download" },
	{ ERR_DNLD_NOSIG, "Downloaded firmware failed to start" },
	{ ERR_DNLD_NOEXEC, "Downloaded firmware is not executing" },
	{ ERR_DNLD_NOCARD, "Downloaded firmware is not executing" },
	{ ERR_DNLD_SYSTAT, "Downloaded firmware detected an error" },
	{ ERR_DNLD_BADTLS, "Driver does not support the firmware" },
	{ ERR_DNLD_POST, "Board failed power on self test" },
	{ ERR_DNLD_SW, "Switch setup error" },
	{ ERR_DNLD_MEM, "Could not allocate memory for download" },
	{ ERR_DNLD_FILE, "Could not find the file to download" },
	{ ERR_DNLD_TYPE, "The file is not downloadable" },
	{ ERR_LIB_INCOMPAT, "Incompatible libary used" },
	{ ERR_DRV_INCOMPAT, "Incompatible driver used (pre v4.0)" },
	{ ERR_DRV_CALLINIT, "More than one process attempted to access the driver" },
	{ ERR_TS_BLOCKED, "Timeslot blocked" },
	{ ERR_NO_SYS_RES, "Used in NT_WOS code" },
    };

    for (int i=0;i<sizeof(xlat)/sizeof(xlat[0]);i++) {
	if (rc == xlat[i].event) return xlat[i].name;
    }
    return "UnknownError";
}

/** Map any possible protocol supported by Aculab to a bigger
    class of protocol
 */
static int MapCallType(int port)
{
    SetLockingLocation("Acu:010");
    switch (call_type(port)) {
    case S_1TR6:	
    case S_1TR6NET:
	return S_1TR6;

    case S_DASS:	
    case S_DASSNET:
	return S_DASS;

    case S_DPNSS:
	return S_DPNSS;

    case S_CAS:
    case S_CAS_TONE:
    case S_SS5_TONE:
    case S_T1CAS:	
    case S_T1CAS_TONE:
	return S_CAS;

    case S_ISUP:
	return S_ISUP;

    default:
	return S_ETS300;
    }
}

static const char* MakeMediaName(const char* portName,int tslot)
{
    static char mediaName[32];
    sprintf(mediaName,"%s:%02d",portName,tslot);
    return mediaName;
}


static CallCause MapCauseAculabToQ931(int ac_code)
{
    static struct {
	int ac_code;
	CallCause q931;
    } xlat[] = {
	{ LC_NORMAL, CC_NormalCallClearing },
	{ LC_NUMBER_BUSY, CC_UserBusy },
	{ LC_NO_ANSWER, CC_NoAnswerFromUser },
	{ LC_NUMBER_UNOBTAINABLE, CC_UnallocatedNumber },
	{ LC_NUMBER_CHANGED, CC_NumberChanged },
	{ LC_OUT_OF_ORDER, CC_DestinationOutOfOrder },
	{ LC_INCOMING_CALLS_BARRED, CC_ChannelUnacceptable },
	{ LC_CALL_REJECTED, CC_CallRejected },
	{ LC_CALL_FAILED, CC_NetworkOutOfOrder },
	{ LC_CHANNEL_BUSY, CC_RequestedCircuitUnavailable },
	{ LC_NO_CHANNELS, CC_RequestedCircuitUnavailable },
	{ LC_CONGESTION, CC_SwitchingEquipmentCongestion },
    };

    for (int i=0;i<sizeof(xlat)/sizeof(xlat[0]);i++) {
	if (ac_code == xlat[i].ac_code) return xlat[i].q931;
    }
    return CC_NormalUnspecified;
}

static int MapCauseQ931ToAculab(CallCause cc)
{
    static struct {
	CallCause cc;
	int ac_code;
    } xlat[] = {
	{ CC_UnallocatedNumber, LC_NUMBER_UNOBTAINABLE },
	{ CC_NormalCallClearing, LC_NORMAL },
	{ CC_UserBusy, LC_NUMBER_BUSY },
	{ CC_NoUserResponding, LC_CALL_FAILED },
	{ CC_NoAnswerFromUser, LC_NO_ANSWER },
	{ CC_CallRejected, LC_CALL_REJECTED },
	{ CC_NumberChanged, LC_NUMBER_CHANGED },
	{ CC_DestinationOutOfOrder, LC_OUT_OF_ORDER },
	{ CC_InvalidNumberFormat, LC_NUMBER_UNOBTAINABLE },
	{ CC_NoCircuitAvailable, LC_NO_CHANNELS },
	{ CC_NetworkOutOfOrder, LC_OUT_OF_ORDER },
	{ CC_TemporaryFailure, LC_OUT_OF_ORDER },
	{ CC_SwitchingEquipmentCongestion, LC_CONGESTION },
	{ CC_RequestedCircuitUnavailable, LC_NO_CHANNELS },
    };

    for (int i=0;i<sizeof(xlat)/sizeof(xlat[0]);i++) {
	if (cc == xlat[i].cc) return xlat[i].ac_code;
    }
    return LC_CALL_REJECTED;
}

/*----------------------------------------------------------------* 
 * Implementation of AculabFactory
 *----------------------------------------------------------------*/
AculabFactory::AculabFactory() : ServiceProvider("AculabItf",false,true,Lowest_Priority)
{
    _fOpened = false;
#ifndef __linux__
    _quit_event = CreateEvent(0, TRUE, 0, 0);
#endif

    _thread = 0;
}

AculabFactory::~AculabFactory()
{
#ifndef __linux__
    CloseHandle(_quit_event);
#endif
}

/** Create: create one instance first before returning a pointer to it
 */
AculabFactory* AculabFactory::Create()
{
    if (!theAculabFactory) {
	theAculabFactory = new AculabFactory;
	AculabSwitchManager::start();
    }
    return theAculabFactory;
}

void AculabFactory::Destroy()
{
    AculabSwitchManager::stop();
    delete this;
}

/** This thread will check the state of all layer 1 interface
 */
#ifndef __linux__
DWORD WINAPI AculabFactory::Layer1_Stats_Thread(void *arg)
#else
DWORD AculabFactory::Layer1_Stats_Thread(void *arg)
#endif
{
    AculabFactory* instance = (AculabFactory* )arg;

#ifndef __linux__
    while (WaitForSingleObject(instance->_quit_event, 5000) == WAIT_TIMEOUT) {

	// dont poll layer 1 if CPU is high
	int cpu_load = CPU_Load_Monitor::Instance()->GetPercentUserTime(Load_Monitor::FiveSeconds) + 
	    CPU_Load_Monitor::Instance()->GetPercentKernelTime(Load_Monitor::FiveSeconds);
	if (!instance->_interfaces.empty() && cpu_load > 75) {
	    CLog::Warning(THISMODULE, "CPU load is high (%d%%), skipping layer 1 check", cpu_load);
	    continue;
	}

	ListInterface::iterator it;
	for (it=instance->_interfaces.begin(); it!=instance->_interfaces.end(); it++) {
	    //CLog::Detail(THISMODULE, "Check layer 1 stats on port %s", (*it)->Name().c_str());
	    
	    DWORD now = GetTickCount();
	    (*it)->Device_Check_Layer1();

	    DWORD delay = GetTickCount() - now;
	    if (delay > 50) CLog::Error(THISMODULE, "Checking layer 1 on port %s took too long (%dms)", (*it)->Name().c_str(), delay);
	}
    }
#endif

    return 0;
}

/** Open: Initialize Aculab driver and for each Aculab port, create the associated
    Interface object
 */
bool AculabFactory::Load()
{
    assert (_fOpened == false);
    
    ACU_ERR		error;
    
    //Debug TING
    //TiNGtrace = 3;
    
    
    //We don't need to call system_init() since
    //everything is done in the Aculab configuration tool
    //( See migration guide )
    if (!_fOpened) 
    {
	/* Interrogate System */
	ACU_SNAPSHOT_PARMS rSnapshotParms;
	INIT_ACU_STRUCT(&rSnapshotParms);
	
	error = acu_get_system_snapshot(&rSnapshotParms);
	
	if(error)
	    CLog::Error(THISMODULE,"Unable to interrogate system (%s)",MapErrorToString(error));
	
	/* Open all available cards */
	ACU_OPEN_CARD_PARMS 	    rOpenParms;
	ACU_CARD_INFO_PARMS	    rCardInfo;
	ACU_OPEN_SWITCH_PARMS	    rOpenSwitch;
	ACU_OPEN_CALL_PARMS	    rOpenCall;
	OPEN_PORT_PARMS		    cOpenPort;
	
	CARD_INFO_PARMS		    cCardInfo;
	
	ACU_INT		currentPort = 0;
	for(ACU_UINT c = 0; c < rSnapshotParms.count; c++)
	{
	    INIT_ACU_STRUCT(&rOpenParms);
	    strcpy(rOpenParms.serial_no, rSnapshotParms.serial_no[c]);
	    
	    /* Open card */
	    error = acu_open_card(&rOpenParms);
	    
	    if(error) 	
		CLog::Error(THISMODULE,"Unable to open card %s; Error = (%s)", rSnapshotParms.serial_no[c], MapErrorToString(error));
	    
	    /* Reset switch drivers of this card */
	    sw_reset_switch(rOpenParms.card_id);
	    sw_switch_override_mode(rOpenParms.card_id,0);
	    
	    /* Get Card Info */
	    INIT_ACU_STRUCT(&rCardInfo);
	    rCardInfo.card_id = rOpenParms.card_id;
	    
	    error = acu_get_card_info(&rCardInfo);
	    if(error)
		CLog::Error(THISMODULE,"Unable to obtain card %s resource info; Error = (%s)", rSnapshotParms.serial_no[c], MapErrorToString(error));
	    
	    if(rCardInfo.resources_available & ACU_RESOURCE_SWITCH)
	    {
		/* Open Switch Driver Interface */
		INIT_ACU_STRUCT(&rOpenSwitch);
		rOpenSwitch.card_id = rOpenParms.card_id;
		
		error = acu_open_switch(&rOpenSwitch);
		if(error)
		    CLog::Error(THISMODULE,"Unable to open switch driver of card %s; Error = (%s)", rSnapshotParms.serial_no[c], MapErrorToString(error));
	    }
	    
	    if(rCardInfo.resources_available & ACU_RESOURCE_CALL)
	    {
		/* Open Call Driver Interface */
		INIT_ACU_STRUCT(&rOpenCall);
		rOpenCall.card_id = rOpenParms.card_id;
		
		error = acu_open_call(&rOpenCall);
		if(error) 
		    CLog::Error(THISMODULE,"Unable to open call driver of card %s; Error = (%s)", rSnapshotParms.serial_no[c], MapErrorToString(error));
	    }
	    else
		continue;
	    
	    /* Get Ports on Card */
	    INIT_ACU_STRUCT(&cCardInfo);
	    cCardInfo.card_id = rOpenParms.card_id;
	    
	    error= call_get_card_info(&cCardInfo);
	    if(error && rCardInfo.resources_available & ACU_RESOURCE_CALL)
		CLog::Error(THISMODULE,"Unable to obtain card %s call info; Error = (%s)", rSnapshotParms.serial_no[c], MapErrorToString(error));
	    
	    // get the number of ports
	    SetLockingLocation("Acu:020");
	    int nports = cCardInfo.ports;
	    if (nports < 0) 
	    {
		CLog::Error(THISMODULE,"Error while retrieving number of ports (%s)",MapErrorToString(nports));
		return true;
	    }
	    
	    CLog::Detail(THISMODULE,"%d ports Aculab device detected",nports);
	    if (nports == 0) 
	    {
		_fOpened = true;
		return true;
	    }
	    
	    for (int port=0;port<nports;port++,currentPort++) 
	    {
		char name[16];
		sprintf(name,"dac%02d",currentPort);
		
		INIT_ACU_STRUCT(&cOpenPort);
		cOpenPort.card_id = rOpenParms.card_id;
		cOpenPort.port_ix = port;
		
		error = call_open_port(&cOpenPort);
		
		if(error)
		    CLog::Error(THISMODULE,"Unable to open port %d on card with serial num %s; Error = (%s)", port, rSnapshotParms.serial_no[c], MapErrorToString(error));
		
		
		AculabInterface* p = new AculabInterface(name, rOpenParms.serial_no, rOpenParms.card_id, cOpenPort.port_id);
		_interfaces.push_back(p);
		//p->SetSignallingDriver(AculabSignalling::Instance());
	    }
	}
	
	DWORD tid;
	pthread_t acu_thread;
#ifndef __linux__
	_thread = CREATE_THREAD(0, 0, Layer1_Stats_Thread, this, 0, &tid);
	if (_thread == 0) {
	    CLog::Error(THISMODULE, "Unable to start layer1 stats checker thread, errno=%d", GetLastError());
	    return false;
	}
#else
	if( pthread_create(&acu_thread,NULL,&Layer1_Stats_Thread,(void *) this) != 0) {  // thread_process argument passed here is NULL, CHANGE according to the actual scenario
        CLog::Error(THISMODULE, "Unable to start layer1 stats checker thread, errno");
        return false;
    }
#endif
	
	_fOpened = true;
    }
    
    return _fOpened;
}


/** Close: Close Aculab driver
 */
bool AculabFactory::Unload()
{
    if (_fOpened) {
	// instruct poller thread to suicide
#ifndef __linux__
	SetEvent(_quit_event);
	if (WaitForSingleObject(_thread, 5000) == WAIT_TIMEOUT) {
	    CLog::Warning(THISMODULE,"Unable to stop layer1 stats checker thread");
	}
#endif

	_fOpened = false;
	_interfaces.erase(_interfaces.begin(),_interfaces.end());
	return true;
    } else {
	return false;
    }
}


/** HandleEvents: Poll layer 1 status of each AculabInterface
 */
void AculabFactory::HandleEvents()
{
    time_t now = time(NULL);
    if (_l1CheckTime < now) {
	ListInterface::iterator end = _interfaces.end();
	for (ListInterface::iterator it=_interfaces.begin();it != end;it++) {
	    (*it)->HandleEvents();
	}
	_l1CheckTime = (now + LAYER1_CHECK_INTERVAL);
    }
}


/*----------------------------------------------------------------* 
 * Implementation of AculabMedia
 *----------------------------------------------------------------*/
AculabMedia::AculabMedia(const char* serial_num, int port,int tslot,AculabInterface* itf) : 
    DigitalNetworkMedia(MakeMediaName(itf->GetName(), tslot), (MediaId)tslot, itf,
	new AculabSwitch(MakeMediaName(itf->GetName(), tslot), serial_num, 
		call_port_2_swdrvr(port), call_port_2_stream(port), tslot, false)),
    _port(port),
    _state(AMS_Closed),
	_tslot(tslot)

{
}

AculabMedia::~AculabMedia()
{
}

void AculabMedia::SetState(AculabMediaState state)
{
    _state = state;
}

AculabMediaState AculabMedia::GetState()
{
    return _state;
}

int AculabMedia::GetPort() 
{
    return _port;
}





/*----------------------------------------------------------------* 
 * Implementation of AculabInterface
 *----------------------------------------------------------------*/
AculabInterface::AculabInterface(const char* fullName, const char* serial_num, ACU_CARD_ID card_id, int port) :
    DigitalNetworkInterface(fullName)
{
    _port = port;
    _card_id = card_id;

    // default values for runtime configuration parameters
    _cfg_lastalarm = _currentAlarm = ALARM_NONE;
    _cfg_dspfwfile = "";
    _cfg_sigfwfile = "";
    _cfg_sigfwargs = "";
    _cfg_enabled = false;
    _cfg_appendEOS = true;
    _cfg_NCI = 0;
    _cfg_FCI = 0;
    _interfaceLocked = false;
    _nextTS = 0;
    _sigName = "aculab";
    _cfg_signalling = AculabSignalling::Instance();

    strcpy(_serial_num, serial_num);

    memset(_listMedia,0,sizeof(_listMedia));

    _mapAlarm["AIS"] = ALARM_AIS;
    _mapAlarm["RRA"] = ALARM_RRA;
    _mapAlarm["CML"] = ALARM_CML;

    // call counters
    _cntIdle = 0;
    _cntInCall = 0;
    _cntOutCall = 0;

    _running = false;
    _firstPoll = true;
    _monitorL2 = true;

    //SetAlarm(ALARM_RRA,false);

    /***
    WATCHDOG_XPARMS watchdog;
    INIT_ACU_STRUCT(&watchdog);

    watchdog.net = _port;
    watchdog.timeout = 0;
    SetLockingLocation("Acu:030");
    call_watchdog(&watchdog);
    ***/

    EnableRAP();

    _layer1_available = false;
}

AculabInterface::~AculabInterface()
{
    EraseMediaList();

    WATCHDOG_XPARMS watchdog;
    INIT_ACU_STRUCT(&watchdog);

    watchdog.net = _port;
    watchdog.timeout = 0;
    SetLockingLocation("Acu:040");
    call_watchdog(&watchdog);

    SetAlarm(ALARM_RRA,false);
}

/** Remove all media objects in the list
 */
void AculabInterface::EraseMediaList()
{
    for (int i=0;i<ACU_MAX_TIMESLOT_IN_INTERFACE;i++) {
	if (_listMedia[i]) {
	    BlockTimeslot(_listMedia[i]->GetTimeSlot(),true);
	    delete _listMedia[i];
	    _listMedia[i] = NULL;
	}
    }
    _queueMedia.clear();
}

/** Create media according to tslot mask and put them in the list
 */
void AculabInterface::BuildMediaList()
{
    EraseMediaList();

    _cntIdle = 0;
    _cntInCall = 0;
    _cntOutCall = 0;

    for (UINT32 tslot=0,j=1;tslot<ACU_MAX_TIMESLOT_IN_INTERFACE;tslot++,j<<=1) {
	if (_timeslotmask & j) {
	    AculabMedia* media = new AculabMedia(_serial_num, _port, tslot, this);
	    media->SetState(AMS_Idle);
	    _listMedia[tslot] = media;
	    _queueMedia.push_back(media);
	    _cntIdle++;
	    BlockTimeslot(tslot,false);
	}
    }

    SetMediaCount(_cntIdle);
}

void AculabInterface::BlockTimeslot(int timeslot,bool blocked)
{
    if (timeslot >=0 && timeslot < ACU_MAX_TIMESLOT_IN_INTERFACE) {
	AculabMedia* media = _listMedia[timeslot];
	if (media && media->IsBlocked() != blocked) {
	    media->SetBlocked(blocked);

	    CLog::Detail(THISMODULE,"%s %s",media->GetName(),blocked ? "blocked" : "unblocked");

	    // if we receive blocked state transition for an idle circuit
	    // change the media count and let the RLS know about it
	    if (media->GetState() != AMS_Busy) {
		SetMediaCount(_mediaCount + (blocked ? -1 : 1));
	    }
	}
    }
}

/** After each media usage, the media is put back at the end of a list of free pool. 
    The next time a media is requested, depending on the allocation strategy a free 
    media is returned:
    - Ascending: the media with smallest tslot
    - Descending: the media with the highest tslot
    - FIFO: the head of the list
    - LIFO: the tail of the list
    - Uniform: equivalent to FIFO
    NOTE: This implementation works only with permanent medias only

    The argument call may be NULL and it's only valid on synchronous allocation or
    permanent medias.
 */
bool AculabInterface::AllocateMedia(Call* call,Media *&media,SelectionStrategy sel,CallDirection dir,CallParameter* cparam)
{
    assert(dir != CD_Null);

    // check if we are in service
    if (!_enabled) return false;

    AculabMedia* aculabMedia = 0;

    bool tsel_normal = true;
    bool tsel_odd = false;

    /** Determine how timeslot is selected within an E1
     *  0 x x x = normal
     *  1 x x 0 = use odd first then even timeslots
     *  1 x x 1 = use even first the odd timeslots
     **/
    if (_cfg_signalling && (_cfg_signalling->GetType() == SigType_ISUP)) {
	IsupSignalling* isup = (IsupSignalling*)_cfg_signalling;
	tsel_normal = isup->GetTslotSelection() & 0x08 ? false : true;
	tsel_odd = isup->GetTslotSelection() & 0x01 ? false : true;
    }

    switch (GetSelectionStrategy()) {
    case SS_Ascending:
	{
	    if (tsel_normal) {
		for (int tslot = 0; tslot < ACU_MAX_TIMESLOT_IN_INTERFACE && aculabMedia == 0; tslot++) {
		    AculabMedia* media = _listMedia[tslot];
		    if (media && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}
	    } else {
		// Use the odd timeslots first (or even ones)
		int starting_tslot = tsel_odd ? 1 : 0;
		int tslot;
		for (tslot = starting_tslot; tslot < ACU_MAX_TIMESLOT_IN_INTERFACE && aculabMedia == 0; tslot += 2) {
		    AculabMedia* media = _listMedia[tslot];
		    if (media && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}

		// Then use the even timeslots (or odd ones)
		starting_tslot = 1 - starting_tslot;
		for (tslot = starting_tslot; tslot < ACU_MAX_TIMESLOT_IN_INTERFACE && aculabMedia == 0; tslot += 2) {
		    AculabMedia* media = _listMedia[tslot];
		    if (media && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}
	    }
	}
	break;

    case SS_Descending:
	{
	    if (tsel_normal) {
		for (int tslot = ACU_MAX_TIMESLOT_IN_INTERFACE-1; tslot >= 0 && aculabMedia == 0; tslot--) {
		    AculabMedia* media = _listMedia[tslot];
		    if (media && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}
	    } else {
		// Use the odd timeslots first (or even ones)
		int starting_tslot = tsel_odd ? ACU_MAX_TIMESLOT_IN_INTERFACE-1 : ACU_MAX_TIMESLOT_IN_INTERFACE-2;
		int tslot;
		for (tslot = starting_tslot; tslot >= 0 && aculabMedia == 0; tslot -= 2) {
		    AculabMedia* media = _listMedia[tslot];
		    if (media && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}

		// Then use the even timeslots (or odd ones)
		starting_tslot = (ACU_MAX_TIMESLOT_IN_INTERFACE-1) + (ACU_MAX_TIMESLOT_IN_INTERFACE-2) - starting_tslot;
		for (tslot = starting_tslot; tslot >= 0 && aculabMedia == 0; tslot -= 2) {
		    AculabMedia* media = _listMedia[tslot];
		    if (media && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}
	    }
	}
	break;

    case SS_FIFO:
	{
	    if (tsel_normal) {
		// The oldest is at the beginning of the queue
		QueueMedia::iterator it;
		for (it = _queueMedia.begin(); it != _queueMedia.end() && aculabMedia == 0; it++) {
		    AculabMedia* media = *it;
		    if (media && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}
	    } else {
		// Use the odd timeslots first (or even ones) in FIFO mode
		int last_bit = tsel_odd ? 1 : 0;
		QueueMedia::iterator it;
		for (it = _queueMedia.begin(); it != _queueMedia.end() && aculabMedia == 0; it++) {
		    AculabMedia* media = *it;
		    if (media && (media->GetTimeSlot() & 0x01) == last_bit && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}

		// Then use the even timeslots (or odd ones) in LIFO mode
		last_bit = 1 - last_bit;
		QueueMedia::reverse_iterator itr;
		for (itr = _queueMedia.rbegin(); itr != _queueMedia.rend() && aculabMedia == 0; itr++) {
		    AculabMedia* media = *itr;
		    if (media && (media->GetTimeSlot() & 0x01) == last_bit && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}
	    }
	}
	break;

    case SS_LIFO:
	{
	    if (tsel_normal) {
		// The youngest is at the end of the queue
		QueueMedia::reverse_iterator it;
		for (it = _queueMedia.rbegin(); it != _queueMedia.rend() && aculabMedia == 0; it++) {
		    AculabMedia* media = *it;
		    if (media && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}
	    } else {
		// Use the odd timeslots first (or even ones) in LIFO mode
		int last_bit = tsel_odd ? 1 : 0;
		QueueMedia::reverse_iterator itr;
		for (itr = _queueMedia.rbegin(); itr != _queueMedia.rend() && aculabMedia == 0; itr++) {
		    AculabMedia* media = *itr;
		    if (media && (media->GetTimeSlot() & 0x01) == last_bit && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}

		// Then use the even timeslots (or odd ones) in FIFO
		last_bit = 1 - last_bit;
		QueueMedia::iterator it;
		for (it = _queueMedia.begin(); it != _queueMedia.end() && aculabMedia == 0; it++) {
		    AculabMedia* media = *it;
		    if (media && (media->GetTimeSlot() & 0x01) == last_bit && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}
	    }
	}
	break;

    case SS_Uniform:
	{
	    if (tsel_normal) {
		for (int tslot = 0; tslot < ACU_MAX_TIMESLOT_IN_INTERFACE && aculabMedia == 0; tslot++) {
		    _nextTS = (_nextTS+1)%ACU_MAX_TIMESLOT_IN_INTERFACE;
		    AculabMedia* media = _listMedia[_nextTS];
		    if (media && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}
	    } else {
		// Use the odd timeslots first (or even ones)
		int last_bit = tsel_odd ? 1 : 0;
		int tslot;
		for (tslot = 0; tslot < ACU_MAX_TIMESLOT_IN_INTERFACE && aculabMedia == 0; tslot++) {
		    _nextTS = (_nextTS+1)%ACU_MAX_TIMESLOT_IN_INTERFACE;
		    if ((_nextTS & 0x01) != last_bit)
			continue;
		    AculabMedia* media = _listMedia[_nextTS];
		    if (media && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}

		// Then use the even timeslots (or odd ones)
		last_bit = 1 - last_bit;
		for (tslot = 0; tslot < ACU_MAX_TIMESLOT_IN_INTERFACE && aculabMedia == 0; tslot++) {
		    _nextTS = (_nextTS+1)%ACU_MAX_TIMESLOT_IN_INTERFACE;
		    if ((_nextTS & 0x01) != last_bit)
			continue;
		    AculabMedia* media = _listMedia[_nextTS];
		    if (media && media->GetState() == AMS_Idle && !media->IsBlocked())
			aculabMedia = media;
		}
	    }
	}
	break;
    }

    if (aculabMedia) {
	// mark it as busy
	if (aculabMedia) aculabMedia->SetState(AMS_Busy);

	CLog::Detail(THISMODULE,"%s allocated",aculabMedia->GetName());

	// temporarily increase media count if blocked
	// so that media count does not change in the end
	if (aculabMedia->IsBlocked()) SetMediaCount(_mediaCount + 1);

	DecMediaCount(aculabMedia, dir,cparam);

	// update call counters
	_cntIdle--;
	if (dir == CD_Incoming) _cntInCall++;
	else _cntOutCall++;

	if (call) call->MediaAvailableInd(aculabMedia);
	media = aculabMedia;
	return true;
    }
    return false;
}

/** After each media usage, the media is put back at the end of a list of free pool. 
    The next time a media is requested, depending on the allocation strategy a free 
    media is returned:
    - Ascending: the media with smallest tslot
    - Descending: the media with the highest tslot
    - FIFO: the head of the list
    - LIFO: the tail of the list
    - Uniform: equivalent to FIFO
    NOTE: This implementation works only with permanent medias only

    The argument call may be NULL and it's only valid on synchronous allocation or
    permanent medias.
 */
bool AculabInterface::AllocateMedia(Call* call,Media *&media,MediaId id,CallDirection dir,CallParameter* cparam)
{
    // check if we are in service
    if (!_enabled || _interfaceLocked) return false;

    media = NULL;
    if (id < 0 || id >= ACU_MAX_TIMESLOT_IN_INTERFACE) 
	return false;

    AculabMedia* aMedia = _listMedia[id];
    if (aMedia && aMedia->GetState() == AMS_Idle) {
	CLog::Detail(THISMODULE,"%s TS#%d allocated",Name().c_str(),aMedia->GetId());

	media = aMedia;

	// temporarily increase media count if blocked
	// so that media count does not change in the end
	if (aMedia->IsBlocked()) SetMediaCount(_mediaCount + 1);

	DecMediaCount(aMedia, dir,cparam);

	// mark it as busy
	aMedia->SetState(AMS_Busy);

	// update call counters
	_cntIdle--;
	if (dir == CD_Incoming) _cntInCall++;
	else _cntOutCall++;

	if (call) call->MediaAvailableInd(aMedia);
	return true;
    }

    return false;
}

void AculabInterface::ReleaseMedia(Media* media,CallDirection dir,CallParameter* cparam,CallCause cc)
{
    AculabMedia* aMedia = (AculabMedia*)media;

    CLog::Detail(THISMODULE,"%s TS#%d released",Name().c_str(),aMedia->GetId());

    // update call counters
    if (dir == CD_Incoming) _cntInCall--;
    else _cntOutCall--;

    if (_cfg_enabled) {
	_cntIdle++;
	aMedia->SetState(AMS_Idle);
	_queueMedia.remove(aMedia);
	_queueMedia.push_back(aMedia);
    } else {
	aMedia->SetState(AMS_Closed);
    }
    IncMediaCount(media,dir,cparam,cc);

    // decrease media count if blocked so that media 
    // count does not change in the end
    if (aMedia->IsBlocked()) SetMediaCount(_mediaCount - 1);

}

/** Load: Called once at initialization time
 */
bool AculabInterface::Load()
{
    // establish default values for valid timeslots
    SetLockingLocation("Acu:050");
    switch (call_line(_port)) {
    case L_E1:
	_timeslotmask = 0xfffefffe;
	break;
    case L_T1_CAS:
	_timeslotmask = 0x00ffffff;
	break;
    case L_T1_ISDN:
	_timeslotmask = 0x007fffff;
	break;
    case L_BASIC_RATE:
	_timeslotmask = 0x00000006;
	break;
    default:
	SetLockingLocation("Acu:060");
	if (call_is_download(_port) == 1) _timeslotmask = 0xfffefffe;
	else _timeslotmask = 0;
    }

    //BuildMediaList();

    // signalling driver index
    SetLockingLocation("Acu:070");
    _swdrv = call_port_2_swdrvr(_port);

    // Register configuration commands
    // timeslot
    Term_Cmd* cmd = new Term_Cmd(this, Cmd_Timeslot, "timeslot", "Specify timeslots enabled for this interface", PrivilegeLevelAdmin, TermModeConfig, false);
    cmd->Add(new Term_Cmd_Arg("arg1", CVariant(0), CVariantValidator(CVariant(0), "Timeslot mask")));
    RegisterTermCmd(cmd,_cfgTree);

    // signalling
    cmd = new Term_Cmd(this, Cmd_Signalling, "signalling", "Specify the signalling driver for this interface", PrivilegeLevelAdmin, TermModeConfig, false);
	CVariantChoiceValidator ctv("aculab","Use Aculab driver for signalling");
	ctv.Add("isup","Use ISUP signalling");
	ctv.Add("bssap-call","Use BSSAP-Call signalling");
    cmd->Add (new Term_Cmd_Arg ("@0", CVariant(""), ctv));
    cmd->AddDynamic(1, 0, "isup", new Term_Cmd_Arg("@1", CVariant(""), CVariantValidator(CVariant(""), "ISUP signalling name")));
    cmd->AddDynamic(2, 0, "isup", new Term_Cmd_Arg("@2", CVariant(0), CVariantValidator(CVariant(0), "ISUP group of this interface")));
    cmd->AddDynamic(1, 0, "bssap-call", new Term_Cmd_Arg("@1", CVariant(""), CVariantValidator(CVariant(""), "BSSAP-Call signalling name")));
    cmd->AddDynamic(2, 0, "bssap-call", new Term_Cmd_Arg("@2", CVariant(0), CVariantValidator(CVariant(0), "BSC port connected to this interface")));
    RegisterTermCmd(cmd,_cfgTree);

    // (no) description
    cmd = new Term_Cmd(this, Cmd_Description, "description", "Specify the description for this interface", PrivilegeLevelAdmin, TermModeConfig, true);
    cmd->Add(new Term_Cmd_Arg("arg1", "", CVariantValidator("", "Description for this interface"),TermArgOptional));
    RegisterTermCmd(cmd,_cfgTree);

    // (no) circuit-reference
    cmd = new Term_Cmd(this, Cmd_CircuitRef, "circuit-reference", "Specify the circuit reference for this interface", PrivilegeLevelAdmin, TermModeConfig, true);
    cmd->Add(new Term_Cmd_Arg("arg1", "", CVariantValidator("", "Circuit reference for this interface"),TermArgOptional));
    RegisterTermCmd(cmd,_cfgTree);

    // (no) signalling firmware file
    cmd = new Term_Cmd(this, Cmd_SigFwFile, "fwfile", "Specify the name of signalling firmware", PrivilegeLevelAdmin, TermModeConfig, true);
    cmd->Add(new Term_Cmd_Arg("arg1", "", CVariantValidator("", "Signalling firmware filename"),TermArgOptional));
    RegisterTermCmd(cmd,_cfgTree);

    // (no) signalling firmware file
    cmd = new Term_Cmd(this, Cmd_SigFwArgs, "fwargs", "Specify the arguments/parameters of signalling firmware", PrivilegeLevelAdmin, TermModeConfig, true);
    cmd->Add(new Term_Cmd_Arg("arg1", "", CVariantValidator("", "Signalling firmware arguments/parameters"),TermArgOptional));
    RegisterTermCmd(cmd,_cfgTree);

    // (no) dsp firmware
    cmd = new Term_Cmd(this, Cmd_DspFwFile, "fwdsp", "Specify the name of firmware file for the attached DSPs", PrivilegeLevelAdmin, TermModeConfig, true);
    cmd->Add(new Term_Cmd_Arg("arg1", "", CVariantValidator("", "DSP firmware filename"),TermArgOptional));
    RegisterTermCmd(cmd,_cfgTree);

    // (no) enable
    cmd = new Term_Cmd(this, Cmd_Enable, "enable", "Enable this interface", PrivilegeLevelAdmin, TermModeConfig, true);
    RegisterTermCmd(cmd,_cfgTree);

    cmd = new Term_Cmd(this, Cmd_Group, "group", "Assign this interface to a particular group", PrivilegeLevelAdmin, TermModeConfig, true);
    cmd->Add(new Term_Cmd_Arg("arg1", "", CVariantValidator("", "group name"),TermArgOptional));
    RegisterTermCmd(cmd,_cfgTree);

    cmd = new Term_Cmd(this, Cmd_UseRLS, "resource-locator", "Use Resource Locator", PrivilegeLevelAdmin, TermModeConfig, true);
    RegisterTermCmd(cmd,_cfgTree);

    cmd = new Term_Cmd(this, Cmd_MonitorLayer2, "monitor-layer2", "Prevent calls if Aculab signalling is down", PrivilegeLevelAdmin, TermModeConfig, true);
    RegisterTermCmd(cmd,_cfgTree);

    if (MapCallType(_port) == S_CAS) {
	cmd = new Term_Cmd(this, Cmd_AppendEOS, "auto-append-eos", "Automatically append 'F' at the end of dialled digits (CAS only)", PrivilegeLevelAdmin, TermModeConfig, true);
	RegisterTermCmd(cmd,_cfgTree);
    }

    if (MapCallType(_port) == S_ISUP) {
	cmd = new Term_Cmd(this, Cmd_NCI, "nature-of-call-indicators", "Specify NCI parameter during outgoing call", PrivilegeLevelAdmin, TermModeConfig, true);
	cmd->Add(new Term_Cmd_Arg( "arg1", CVariant(0), CVariantValidator(CVariant(0), "One octet NCI value")));
	RegisterTermCmd(cmd,_cfgTree);

	cmd = new Term_Cmd(this, Cmd_FCI, "forward-call-indicators", "Specify FCI parameter during outgoing call", PrivilegeLevelAdmin, TermModeConfig, true);
	cmd->Add(new Term_Cmd_Arg( "arg1", CVariant(0), CVariantValidator(CVariant(0), "Two octets FCI value")));
	RegisterTermCmd(cmd,_cfgTree);
    }

    /**
    // (no) start
    cmd = new Term_Cmd(this, Cmd_Start, "start", "Start this interface", PrivilegeLevelAdmin, TermModeConfig, true);
    RegisterTermCmd(cmd,_cfgTree);
    **/

    // reload
    cmd = new Term_Cmd(this, Cmd_ReloadFirmware, "reload", "Reload firmware", PrivilegeLevelAdmin, TermModeConfig, false);
    RegisterTermCmd(cmd,_cfgTree);

    // Register Aculab specific commands
    // alarm 
    cmd = new Term_Cmd(this, Cmd_Alarm, "alarm", "Send/stop a specific alarm to be sent to the network", PrivilegeLevelAdmin, TermModeConfig, true);
    CVariantChoiceValidator* vDtor = new CVariantChoiceValidator("RRA", "will cause Remote Alarm");
    vDtor->Add("AIS", "will cause AIS to be sent");
    vDtor->Add("CML", "will cause CAS Multi Frame alarm");
    cmd->Add(new Term_Cmd_Arg("alarm_type", "", *vDtor, TermArgOptional));
    RegisterTermCmd(cmd,_cfgTree);
    delete vDtor;

    // status
    cmd = new Term_Cmd(this, Cmd_ShowStatus, "status", "Show/reset the status of layer 1 and 2 of this interface", PrivilegeLevelAdmin, TermModeConfig, false);
    vDtor = new CVariantChoiceValidator("reset", "If specified, reset the counters of layer 1 and 2");
    cmd->Add(new Term_Cmd_Arg("reset", "", *vDtor, TermArgOptional));
    RegisterTermCmd(cmd,_cfgTree);
    delete vDtor;

    _cfgTree->RegisterShowHandler(this);
    return Interface::Load() && Interface::Start();
}

/** This function will be called by the service framework once the 
    configuration file has been fully read.
 */
bool AculabInterface::CommitConfiguration()
{
    _running = true;
    if (!CommitSignallingDriver()) _sigName = "(none)";
    if (_cfg_enabled) Enable();
    else Disable();

    // display system information
    // Save last serial number, to avoid repetition.
    static char lastserno[64] = "";
    static int board_num = 0;
   

    ACU_ERR		rc;
    CARD_INFO_PARMS	cCardInfo;
    
    INIT_ACU_STRUCT(&cCardInfo);
    cCardInfo.card_id = _card_id;
    
    SetLockingLocation("Acu:080");
    rc = call_get_card_info(&cCardInfo);

    if(rc)
	CLog::Error(THISMODULE, "Unable to get card information (%s)" NEWLINE,MapErrorToString(rc));
    
    std::string sernum = "";

    if (strlen(cCardInfo.serial_no))
    {
	sernum = cCardInfo.serial_no;
    }
    else
    {
	switch (cCardInfo.card_type)
	{
	    case 1:
		sernum = "REV4CARD";
		break;
	    case 2:
		sernum = "REV5CARD_E1T1";
		break;
	    case 3:
		sernum = "BR4CARD";
		break;
	    case 4:
		sernum = "BR8CARD";
		break;
	    default:
		sernum = "unknown";
		break;
	    }
	}
    
    
#ifdef XGATE_HAS_LM
    if (strcmp(sernum.c_str(), lastserno) != 0)
    {
	CLog::Detail(THISMODULE, "Registering Aculab board %d (sn=%s)", board_num, sernum.c_str());
	LicenceManager::Instance()->RegisterSerialNum(Fg_Aculab, board_num++, sernum.c_str(), sernum.size() );
	
	strcpy(lastserno, sernum.c_str());
    }
#endif
    
    return true;
}

bool AculabInterface::CommitSignallingDriver()
{
    if (_sigName == "aculab") {
	_cfg_signalling = AculabSignalling::Instance();
	return SetSignallingDriver(_cfg_signalling);
    } else if (_sigName == "isup") {
	_cfg_signalling = SignallingDriverManager::Instance()->FindDriverByName(_isupName.c_str());
	if (_cfg_signalling && (_cfg_signalling->GetType() == SigType_ISUP)) {
	    if (!SetSignallingDriver(_cfg_signalling)) {
		CLog::Warning(THISMODULE,"Unable to set signalling to %s",_isupName.c_str());
		return false;
	    }
	    return true;
	} else {
	    CLog::Warning(THISMODULE,"Unable to set signalling to %s",_isupName.c_str());
	    return false;
	}
    } else if(_sigName == "bssap-call") {
	_cfg_signalling = SignallingDriverManager::Instance()->FindDriverByName(_bssapCallName.c_str());
	if (_cfg_signalling && (_cfg_signalling->GetType() == SigType_BSSAP)) {
	    SetSignallingDriver(NULL);
	    _interfacegroup = _buf_interfacegroup;
	    if (!SetSignallingDriver(_cfg_signalling)) {
		CLog::Warning(THISMODULE,"Unable to set signalling to %s",_bssapCallName.c_str());
		return false;
	    }
	    return true;
	} else {
	    CLog::Warning(THISMODULE,"Unable to set signalling to %s",_bssapCallName.c_str());
	    return false;
	}
    } else {
	CLog::Warning(THISMODULE,"Unsupported signalling %s on %s",_sigName.c_str(),Name().c_str());
	SetSignallingDriver(NULL);
	_cfg_signalling = NULL;
	return false;
    }
}

/** Set the signalling driver for this interface.
 */
bool AculabInterface::SetSignallingDriver(SignallingDriver* signalling)
{
    // reset blocked state first
    for (int i=0;i<ACU_MAX_TIMESLOT_IN_INTERFACE;i++) {
	if (_listMedia[i]) 
	    BlockTimeslot(_listMedia[i]->GetTimeSlot(),false);
    }

    return Interface::SetSignallingDriver(signalling);
}

/** Terminal command handlers
 */
void AculabInterface::DoCmd_Timeslot(Term_Session* session,Term_Cmd* cmd)
{
    // check if this interface is disabled and that there is no active call
    if (_cfg_enabled || _cntInCall || _cntOutCall) {
	session->SendError("This parameter can only be changed when this interface is not enabled and there is no outstanding calls" NEWLINE);
	return;
    }

    CVariant v1;
    cmd->GetArg((unsigned)0)->GetValue(v1);
    _timeslotmask = v1.AsInt();

    // BuildMediaList();
}

void AculabInterface::DoCmd_Signalling(Term_Session* session,Term_Cmd* cmd)
{
    // check if this interface is disabled and that there is no active call
    if (_cfg_enabled || _cntInCall || _cntOutCall) {
	session->SendError("This parameter can only be changed when this interface is not enabled and there is no outstanding calls" NEWLINE);
	return;
    }

    CVariant v1;
    cmd->GetArg((unsigned)0)->GetValue(v1);

    if (strcmpi(v1.AsString(),"aculab") == 0) {
	_sigName = "aculab";
	if (_running) {
	    if (!CommitSignallingDriver()) {
		session->SendErrorf("Unable to set signalling to aculab" NEWLINE);
		_sigName = "(none)";
	    }
	}
    } else if (strcmpi(v1.AsString(),"isup") == 0) {
	CVariant v2;	// ISUP signalling name
	CVariant v3;	// ISUP group
	cmd->GetArg(1)->GetValue(v2);
	cmd->GetArg(2)->GetValue(v3);

	_sigName = "isup";
	_isupName = v2.AsString();
	_interfacegroup = v3.AsInt();

	if (_running) {
	    if (!CommitSignallingDriver()) {
		session->SendErrorf("Unable to set signalling to %s" NEWLINE,_isupName.c_str());
		_sigName = "(none)";
	    }
	}
    } else if (strcmpi(v1.AsString(),"bssap-call") == 0) {
	CVariant v2;	// BSSAP-Call signalling name
	CVariant v3;	// BSSAP-Call group
	cmd->GetArg(1)->GetValue(v2);
	cmd->GetArg(2)->GetValue(v3);

	_sigName = "bssap-call";
	_bssapCallName = v2.AsString();
	_buf_interfacegroup = v3.AsInt();

	if (_running) {
	    if (!CommitSignallingDriver()) {
		session->SendErrorf("Unable to set signalling to %s" NEWLINE,_bssapCallName.c_str());
		_sigName = "(none)";
	    }
	}
    } else {
	session->SendError("Unsupported signalling for this interface" NEWLINE);
    }
}

/** Terminal command callback
 */
void AculabInterface::DoTermCmd(Term_Session* session, bool no, Term_Cmd* cmd)
{
    CVariant v1;

    switch (cmd->GetId()) {
    case Cmd_Timeslot:
	ACQUIRE_SYSTEM_LOCK();
	DoCmd_Timeslot(session,cmd);
	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_Signalling:
	ACQUIRE_SYSTEM_LOCK();
	DoCmd_Signalling(session,cmd);
	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_Enable:
	ACQUIRE_SYSTEM_LOCK();
	if (_interfaceLocked) {
	    session->SendError("Interface is currently locked" NEWLINE);
	} else {
	    if (no) {
		Disable();
	    } else {
		Enable();
	    }
	}
	RELEASE_SYSTEM_LOCK();
	break;

    /*TBD: to be supported or not
    case Cmd_Start:
	if (no) {
	    //TBD: Carefull with concurrency
	    Stop();
	} else {
	    Start();
	}
	break;
    */

    case Cmd_Alarm:
	ACQUIRE_SYSTEM_LOCK();
	if (no) {
	    SetAlarm(ALARM_NONE);
	} else {
	    cmd->GetArg((unsigned)0)->GetValue(v1);
	    SetAlarm(_mapAlarm[v1.AsString()]);
	}
	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_ShowStatus:
	cmd->GetArg((unsigned)0)->GetValue(v1);
	if (strcmpi(v1.AsString(),"reset") == 0) {
	    ACQUIRE_SYSTEM_LOCK();
	    ResetStatus(session);
	    RELEASE_SYSTEM_LOCK();
	} else {
	    ShowInterface(session);
	}
	break;

    case Cmd_Description:
	ACQUIRE_SYSTEM_LOCK();
	if (no) {
	    SetDescription("");
	} else {
	    cmd->GetArg((unsigned)0)->GetValue(v1);
	    SetDescription(v1.ToString());
	}
	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_CircuitRef:
	ACQUIRE_SYSTEM_LOCK();
	if (no) {
	    SetCircuitReference("");
	} else {
	    cmd->GetArg((unsigned)0)->GetValue(v1);
	    SetCircuitReference(v1.ToString());
	}
	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_SigFwFile:
	ACQUIRE_SYSTEM_LOCK();
	if (!no) {
	    cmd->GetArg((unsigned)0)->GetValue(v1);
	    _cfg_sigfwfile = v1.ToString();
	} else {
	    _cfg_sigfwfile = "";
	}
	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_SigFwArgs:
	ACQUIRE_SYSTEM_LOCK();
	if (!no) {
	    cmd->GetArg((unsigned)0)->GetValue(v1);
	    _cfg_sigfwargs = v1.ToString();
	} else {
	    _cfg_sigfwargs = "";
	}
	RELEASE_SYSTEM_LOCK();
	break;

    // [no] resource-locator
    case Cmd_UseRLS:
	ACQUIRE_SYSTEM_LOCK();
	if (no) DisableRAP();
	else EnableRAP();
	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_MonitorLayer2:
	ACQUIRE_SYSTEM_LOCK();
	if (no) {
	    _monitorL2 = false;

	    // re-enable all timeslots
	    if (_signalling == AculabSignalling::Instance()) {
		UINT32 mask = GetTimeslotMask();
		for (UINT32 tslot=0,j=1;tslot<ACU_MAX_TIMESLOT_IN_INTERFACE;tslot++,j<<=1) {
		    if (mask & j) BlockTimeslot(tslot,false);
		}
	    }
	} else {
	    _monitorL2 = true;
	}
	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_AppendEOS:
	ACQUIRE_SYSTEM_LOCK();
	_cfg_appendEOS = !no;
	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_NCI:
	cmd->GetArg((unsigned)0)->GetValue(v1);
	ACQUIRE_SYSTEM_LOCK();
	_cfg_NCI = v1.AsInt();
	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_FCI:
	cmd->GetArg((unsigned)0)->GetValue(v1);
	ACQUIRE_SYSTEM_LOCK();
	_cfg_FCI = v1.AsInt();
	RELEASE_SYSTEM_LOCK();
	break;

    // [no] group <name>
    case Cmd_Group:
	ACQUIRE_SYSTEM_LOCK();
	if (no) {
	    SetGroupName("");
	} else {
	    if (!RAPEnabled() || !_running) {
		cmd->GetArg((unsigned)0)->GetValue(v1);
		SetGroupName(v1.AsString());
	    } else {
		session->SendError("This command may only be used without resource locator" NEWLINE);
	    }
	}
	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_DspFwFile:
	ACQUIRE_SYSTEM_LOCK();
	if (!no) {
	    cmd->GetArg((unsigned)0)->GetValue(v1);
	    _cfg_dspfwfile = v1.ToString();
	} else {
	    _cfg_dspfwfile = "";
	}
	RELEASE_SYSTEM_LOCK();
	break;

    case Cmd_ReloadFirmware:
	ReloadFirmware(session);
	break;
    }
}

/**
 Show configuration for the specified entry.
 This function will be called by the framework whenever it needs to display the
 specified entry. Previously this instance should tell the entry that it wishes to
 handle displaying the entry by calling entry->RegisterShowHandler().

 When this instance is deleted, it should call entry->UnregisterShowHandler()
 so that the handler will not be called (which will cause GPF).
 */
bool AculabInterface::OnShowConfig(const Cfg_Tree_Entry* entry,String & output, const String & indent, int indent_size) const
{
    char str[256];

    if (GetDescription().size()) output += indent + "description \"" + GetDescription() + "\"" + NEWLINE;

    if (GetCircuitReference().size()) output += indent + "circuit-reference \"" + GetCircuitReference() + "\"" + NEWLINE;

    AculabInterface* This = (AculabInterface*)this;

    if (!This->RAPEnabled() && strlen(This->GetGroupName())) 
	output += indent + "group \"" + This->GetGroupName() + "\"" + NEWLINE;

    sprintf(str,"timeslot 0x%08x",_timeslotmask);
    output += indent + str + NEWLINE;
    if (!_signalling) {
	output += indent + "signalling (none)" + NEWLINE;
    } else if (_signalling == AculabSignalling::Instance()) {
	output += indent + "signalling aculab" + NEWLINE;
    } else if (strcmpi(_sigName.c_str(),"isup") == 0) {
	char isupGrp[16];
	sprintf(isupGrp,"%d",_interfacegroup);
	output += indent + "signalling isup " + _signalling->Name() + " " + isupGrp + NEWLINE;
    } else if (_signalling->GetType() == SigType_BSSAP) { // bssap-call
	char bssapCallGrp[16];
	sprintf(bssapCallGrp,"%d",_interfacegroup);
	output += indent + "signalling bssap-call " + _signalling->Name() + " " + bssapCallGrp + NEWLINE;
    } else {
	output += indent + "signalling (none)" + NEWLINE;
    }

    if (_cfg_sigfwfile != "") output += indent + "fwfile \"" + _cfg_sigfwfile + "\"" + NEWLINE;
    if (_cfg_sigfwargs != "") output += indent + "fwargs \"" + _cfg_sigfwargs + "\"" + NEWLINE;
    if (_cfg_dspfwfile != "") output += indent + "fwdsp \""  + _cfg_dspfwfile + "\"" + NEWLINE;

    sprintf(str,"%smonitor-layer2",_monitorL2 ? "" : "no ");
    output += indent + str + NEWLINE;

    if (MapCallType(_port) == S_CAS) {
	sprintf(str,"%sauto-append-eos",_cfg_appendEOS ? "" : "no ");
	output += indent + str + NEWLINE;
    }

    if (MapCallType(_port) == S_ISUP) {
	sprintf(str,"nature-of-call-indicators 0x%02X", _cfg_NCI);
	output += indent + str + NEWLINE;
	sprintf(str,"forward-call-indicators 0x%04X", _cfg_FCI);
	output += indent + str + NEWLINE;
    }

    sprintf(str,"%sresource-locator",This->RAPEnabled() ? "" : "no ");
    output += indent + str + NEWLINE;

    if (_cfg_lastalarm != ALARM_NONE) {
	output += indent + "alarm ";
	for (MapStringInt::const_iterator it=_mapAlarm.begin();it!=_mapAlarm.end();it++) {
	    if ((*it).second == _cfg_lastalarm) {
		output += (*it).first;
		break;
	    }
	}
	output += NEWLINE;
    }

    if (_cfg_enabled) output += indent + "enable" + NEWLINE;
    else output += indent + "no enable" + NEWLINE;

    return OK;
}

/** Unload: Called once at termination time
 */
bool AculabInterface::Unload()
{
    return Interface::Unload();
}

/* Start/Stop
 */
bool AculabInterface::Start()
{
    return Interface::Start();
}

bool AculabInterface::Stop()
{
    return Interface::Stop();
}

/** Enable:
 */
void AculabInterface::Enable()
{
    if (!_running) {
	_cfg_enabled = true;
	return;
    }

    // open all closed media
    for (int i=0;i<ACU_MAX_TIMESLOT_IN_INTERFACE;i++) {
	if (_listMedia[i] && _listMedia[i]->GetState() == AMS_Closed) {
	    _listMedia[i]->SetState(AMS_Idle);
	    _cntIdle++;
	}
    }

    if (_signalling) _signalling->EnableInterface(this);
	
    BuildMediaList();
    // put the alarm to its state
    SetAlarm(_cfg_lastalarm,false);

    _cfg_enabled = true;
    Interface::Enable();
}

/** Disable:
 */
void AculabInterface::Disable()
{
    if (!_running) {
	_cfg_enabled = false;
	return;
    }

    // close all opened media
    for (int i=0;i<ACU_MAX_TIMESLOT_IN_INTERFACE;i++) {
	if (_listMedia[i] && _listMedia[i]->GetState() == AMS_Idle) {
	    _listMedia[i]->SetState(AMS_Closed);
	    _cntIdle--;
	}
    }

    if (!_signalling || !_signalling->DisableInterface(this)) {
	SetAlarm(ALARM_RRA, false);
    }
    EraseMediaList();

    _cfg_enabled = false;
    Interface::Disable();
}

/** Send a specific alarm to the network or clear alarm condition
 */
void AculabInterface::SetAlarm(int alarmType,bool changeState)
{
    // set alarm
    ALARM_XPARMS alarm_xparms;
    INIT_ACU_STRUCT(&alarm_xparms);

    alarm_xparms.net = _port;
    alarm_xparms.alarm = alarmType;
    SetLockingLocation("Acu:090");
    int rc = call_send_alarm(&alarm_xparms);
    if (rc == 0 && changeState) _cfg_lastalarm = alarmType;
    _currentAlarm = alarmType;
    if (alarmType != ALARM_NONE) {
	Interface::Disable();
    } else {
	SetLockingLocation("Acu:095");
	if (_cfg_enabled && Device_Check_Layer1()) Interface::Enable();
    }
}

/** Check through Aculab API call whether layer 1 is available
 */
bool AculabInterface::Device_Check_Layer1()
{
    if(!_cfg_enabled) return false;

    L1_XSTATS l1;
    INIT_ACU_STRUCT(&l1);

    l1.net = _port;
    int rc = call_l1_stats(&l1);
    _layer1_available = (rc == 0) && ((l1.get.nos | l1.get.ais | l1.get.los | l1.get.rra | l1.get.rma) == 0);
    return _layer1_available;
}

/** Display status of various physical condition of the port
 */
void AculabInterface::OnShowInterface(Term_Session* session)
{
    int rc;
    
    L1_XSTATS	l1;
    INIT_ACU_STRUCT(&l1);

    l1.net = _port;

    SetLockingLocation("Acu:100");
    rc = call_l1_stats(&l1);
    if (rc) {
	session->SendErrorf( INDENT 
			     "Unable to get layer 1 status (%s)" NEWLINE,MapErrorToString(rc));
	return;
    }

    session->Sendf( INDENT "Call in progress: %d" NEWLINE,_cntInCall+_cntOutCall);
    session->Sendf( INDENT "Idle channels: %d" NEWLINE,_cntIdle);

    session->Send( INDENT "Layer 1 status:" NEWLINE);
    session->Send( INDENT "  Carrier status: ");
    if (l1.get.nos || l1.get.ais || l1.get.los || l1.get.rra || l1.get.rma) {
	if (l1.get.nos == 0xff) session->Send("NOS ");
	if (l1.get.ais == 0xff) session->Send("AIS ");
	if (l1.get.los == 0xff) session->Send("LOS ");
	if (l1.get.rra == 0xff) session->Send("RRA ");
	if (l1.get.rma == 0xff) session->Send("RMA ");
    } else {
	session->Send("Up");
    }
    session->Send(NEWLINE);

    if (_currentAlarm != ALARM_NONE) {
	session->Send( INDENT "  Transmitting alarm ");
	for (MapStringInt::const_iterator it=_mapAlarm.begin();it!=_mapAlarm.end();it++) {
	    if ((*it).second == _currentAlarm) {
		session->Send((*it).first);
		break;
	    }
	}
	session->Send(NEWLINE);
    }

    session->Sendf( INDENT "  HWClock:%d  ",l1.get.clock);
    session->Sendf( "BiVios:%d  ",l1.getset.bipvios);
    session->Sendf( "AlignErrs:%d  ",l1.getset.faserrs);
    session->Sendf( "SlipErrs:%d" NEWLINE,l1.getset.sliperrs);

    if (l1.getset.linestat) {
	session->Send( INDENT "  Carrier transition: ");
	if (l1.getset.linestat & LSTAT_NOS) session->Send("NOS ");
	if (l1.getset.linestat & LSTAT_LOS) session->Send("LOS ");
	if (l1.getset.linestat & LSTAT_AIS) session->Send("AIS ");
	if (l1.getset.linestat & LSTAT_FEC) session->Send("BitError ");
	if (l1.getset.linestat & LSTAT_RRA) session->Send("RRA ");
	if (l1.getset.linestat & LSTAT_SLP) session->Send("Slip ");
	if (l1.getset.linestat & LSTAT_CVC) session->Send("BiVios ");
	if (l1.getset.linestat & LSTAT_CRC) session->Send("CRC ");
	if (l1.getset.linestat & LSTAT_FFA) session->Send("FrameAlign ");
	if (l1.getset.linestat & LSTAT_CML) session->Send("CML ");
	session->Send(NEWLINE);
    }

    if (_signalling == AculabSignalling::Instance()) {

	L2_XSTATE l2;
	INIT_ACU_STRUCT(&l2);

	l2.net = _port;
	SetLockingLocation("Acu:110");
	rc = call_l2_state(&l2);
	if (rc) {
	    session->SendErrorf( INDENT 
				 "Unable to get layer 2 status (%s)" NEWLINE,MapErrorToString(rc));
	    return;
	}
	session->Sendf( INDENT "Layer 2:" NEWLINE, l2.state ? "Up" : "Down");
	session->Sendf( INDENT "  Status: %s" NEWLINE, l2.state ? "Up" : "Down");
    }


	CARD_INFO_PARMS		cCardInfo;

	INIT_ACU_STRUCT(&cCardInfo);
	cCardInfo.card_id = _card_id;

    SetLockingLocation("Acu:120");
	rc = call_get_card_info(&cCardInfo);

	if(rc)
	{
	session->SendErrorf( INDENT 
			     "Unable to get card information (%s)" NEWLINE,MapErrorToString(rc));
	return;

    }

    session->Sendf( INDENT "System information:" NEWLINE);
    
    SetLockingLocation("Acu:130");
    int version = call_version(_port);
    
    if (version > 0) {
	int major = version>>8;
	int minor = version&0xff;
	session->Sendf( INDENT "  Driver version: %x.%x" NEWLINE,major,minor);
    }
    session->Sendf( INDENT "  ");

	PORT_INFO_PARMS		portParams;

	INIT_ACU_STRUCT(&portParams);
	portParams.port_id = _port;

	call_port_info( &portParams );

    if (portParams.physical_index) 
		session->Sendf( "Port:0x%X  ",portParams.physical_index);

    session->Sendf( "IRQ:%d  ",cCardInfo.irq_ticks);

    session->Sendf( INDENT "  S/N:%s  ",cCardInfo.serial_no);
    session->Sendf( "H/W ver:%s" NEWLINE,cCardInfo.hw_version);

    session->Sendf( INDENT "  Phys. port:%x  ",portParams.physical_index);
    session->Sendf( "#ports:%d" NEWLINE,cCardInfo.ports);

    session->Sendf( INDENT "  Build:%s  ",l1.get.buildstr);
    session->Sendf( "F/W:%s" NEWLINE,l1.get.sigstr);
}

bool AculabInterface::IsLayer1Available() const
{
    return _layer1_available;
}

bool AculabInterface::IsActive() const
{
	bool layer1 = IsLayer1Available();

    return _cfg_enabled && layer1 && _enabled;
}

/** OnShutdown: This method is called when the Kernel receives
    an instruction to shutdown. Aculab interface transmits remote 
    alarm to the network on shutdown
 */
void AculabInterface::OnShutdown(bool cancel)
{
    if (!cancel) {
	SetAlarm(ALARM_RRA,false);
	Interface::Disable(DC_Shutdown);
    } else {
	// put the alarm to its state
	SetAlarm(_cfg_lastalarm,false);
	if (_cfg_enabled) Enable();
    }
}

/** HandleEvents: It is called by the Service Provider Manager to let
    this object to poll its underlying library/driver
 */
void AculabInterface::HandleEvents()
{
    if (IsLayer1Available()) {
	if (_cfg_enabled && !_enabled && (_disableCause == DC_Layer1)) {
	    CLog::Info(THISMODULE,"%s layer 1 goes to up",Name().c_str());
	    Interface::Enable();
	}

	// if signalling == Aculab then check layer 2 as well
	if (_signalling == AculabSignalling::Instance() && _monitorL2) {
	    L2_XSTATE l2;
	    INIT_ACU_STRUCT(&l2);
	    
	    l2.net = _port;
	    SetLockingLocation("Acu:150");
	    if (call_l2_state(&l2) == 0) {
		unsigned tsmask = 1;
		for (int ts=0;ts<32;ts++,tsmask<<=1) {
		    BlockTimeslot(ts,(l2.state&tsmask) == 0);
		}
	    }
	}
    } else {
	if (_cfg_enabled && _enabled) {
	    CLog::Warning(THISMODULE,"%s layer 1 goes to down",Name().c_str());
	    Interface::Disable(DC_Layer1);
	}
    }

    if (_firstPoll) {
	ResetStatus(NULL);
	_firstPoll = false;
    }
}

/** Reset layer 1 statistics counters
 */
void AculabInterface::ResetStatus(Term_Session* session)
{
    int rc;

    L1_XSTATS l1;
    INIT_ACU_STRUCT(&l1);

    l1.net = _port;
    l1.getset.linestat = ~0;
    l1.getset.bipvios = ~0;
    l1.getset.faserrs = ~0;
    l1.getset.sliperrs = ~0;
    SetLockingLocation("Acu:160");
    rc = call_l1_stats(&l1);
    if (rc) {
	if (session) session->SendErrorf("Unable to reset layer 1 counters (%s)" NEWLINE,MapErrorToString(rc));
	return;
    }
}

void AculabInterface::OnClearCounter()
{
    ResetStatus(NULL);
    Interface::OnClearCounter();
}


/** Reload/restart firmware for the specified port
 */
void AculabInterface::ReloadFirmware(Term_Session* session)
{
    int alarm = _currentAlarm;
    bool enabled = _cfg_enabled;

    // check if we are in interactive mode
    if (session->GetStream().IsInteractive()) {

	// request confirmation from user
	if (!session->PromptYesNo(false, 
		    "It will disconnect current active calls on this interface." NEWLINE 
		    "Do you wish to proceed (y/n)?"))
	    return;

	// lock and disable the interface to prevent 
	// further calls to be made on this interface
	ACQUIRE_SYSTEM_LOCK();

	_interfaceLocked = true;
	//**Disable();

	CLog::Detail(THISMODULE,"Interface %s locked for reload",Name().c_str());

	if (session) session->Send("Disconnecting current active calls ... ");
	for (int retry=0;retry<8 && (_cntInCall+_cntOutCall);retry++) {
	    // disconnect all active calls on this interface
	    for (int i=0;i<ACU_MAX_TIMESLOT_IN_INTERFACE;i++) {
		if (_listMedia[i] && _listMedia[i]->GetState() == AMS_Busy) {
		    AculabMedia* media = _listMedia[i];
		    Call* call = media->GetCall();
		    if (call) {
			switch (call->GetState()) {
			case CS_Disconnected:
			case CS_Clearing:
			case CS_Releasing:
			case CS_Null:
			    break;

			default:
			    CLog::Detail(THISMODULE,"Force disconnect call %s",call->GetName());
			    call->DisconnectInd(CC_NormalUnspecified,CP_NoIndication);
			}
		    }
		}
	    }
	    RELEASE_SYSTEM_LOCK();
	    Sleep(1000);
	    ACQUIRE_SYSTEM_LOCK();

	    Disable();
	    
	    RELEASE_SYSTEM_LOCK();
	    Sleep(5000);
	    ACQUIRE_SYSTEM_LOCK();
	}

	// normally all outstanding calls should be disconnected
	if (_cntInCall+_cntOutCall) {
	    if (enabled) Enable();
	    SetAlarm(alarm,false);
	    _interfaceLocked = false;
	    RELEASE_SYSTEM_LOCK();
	    if (session) {
		session->Send("failed" NEWLINE);
		session->SendError("Unable to disconnect calls, action aborted" NEWLINE);
	    }
	    CLog::Warning(THISMODULE,"Unable to disconnect active calls for reload on %s",Name().c_str());
	    return;
	}
	if (session) session->Send("done" NEWLINE);
    } else {

	// check if this interface is disabled and that there is no active call
	ACQUIRE_SYSTEM_LOCK();
	if (enabled || _cntInCall || _cntOutCall) {
	    RELEASE_SYSTEM_LOCK();
	    if (session) session->SendError("This action can only be done when this interface is not enabled and there is no outstanding calls" NEWLINE);
	    return;
	}

	_interfaceLocked = true;
    }

    if (_cfg_sigfwfile == "") {
	if (enabled) Enable();
	_interfaceLocked = false;
	RELEASE_SYSTEM_LOCK();
	if (session) session->SendErrorf("Unspecified signalling firmware file" NEWLINE);
	return;
    }

    // detach from signalling
    SetSignallingDriver(NULL);

    int rc;

    RESTART_XPARMS restart;
    INIT_ACU_STRUCT(&restart);

    restart.net = _port;
    restart.filenamep = (char*)_cfg_sigfwfile.c_str();
    restart.config_stringp = (char*)_cfg_sigfwargs.c_str();

    RELEASE_SYSTEM_LOCK();

    // make sure that there is no outstanding call on this interface
    Sleep(1000);

    if (session) session->Sendf("Reloading firmware ...");
    SetLockingLocation("Acu:170");
    rc = call_restart_fmw(&restart);

    ACQUIRE_SYSTEM_LOCK();

    // reattach to the original signalling
    SetSignallingDriver(_cfg_signalling);

    if (enabled) Enable();

    SetAlarm(alarm,false);

    ResetStatus(session);

    _interfaceLocked = false;

    RELEASE_SYSTEM_LOCK();

    if (rc) {
	if (session) session->Send(" failed" NEWLINE);
	if (session) session->SendErrorf("Unable to reload firmware (%s)" NEWLINE,MapErrorToString(rc));
    } else {
	if (session) session->Send(" done" NEWLINE);
    }
}


/*----------------------------------------------------------------* 
 * Implementation of AculabSignalling
 *----------------------------------------------------------------*/
AculabSignalling::AculabSignalling() : SignallingDriver(SigType_Aculab,"AculabSig","Aculab",true,Lower_Priority)
{
    _itfAttached = 0;
    _paramMaxPollCount = 8;
    _quit = false;
    _hPollThread = NULL;
#ifndef __linux__
    InitializeCriticalSection(&_csec);
#endif
}

/** Destructor
 */
AculabSignalling::~AculabSignalling()
{
#ifndef __linux__
    DeleteCriticalSection(&_csec);
#endif
}

/** Create: Returns a pointer to this singleton instance, if necessary
    create one instance first before returning a pointer to it
 */
AculabSignalling* AculabSignalling::Create()
{
    if (!theAculabSignalling) {
	theAculabSignalling = new AculabSignalling;
    } else {
	assert(false);
    }
    return theAculabSignalling;
}

/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
 */
bool AculabSignalling::Load()
{
    DWORD tid;
    pthread_t a_thread;
#ifndef __linux__
    _hPollThread = CREATE_THREAD(NULL,0,PollThread,0,0,&tid);
    if (_hPollThread == NULL) {
	CLog::Error(THISMODULE,"Unable to start signalling poller thread, errno=%d",GetLastError());
	return false;
    }
#else
    if( pthread_create(&a_thread,NULL,&PollThread,(void *) this) != 0) {  // thread_process argument passed here is NULL, CHANGE according to the actual scenario
        CLog::Error(THISMODULE,"Unable to start signalling poller thread, errno=");
        return false;
    }
#endif

    return true;
}

/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
 */
bool AculabSignalling::Unload()
{
    // instruct poller thread to suicide
    _quit = true;
#ifndef __linux__
    if (WaitForSingleObject(_hPollThread,5000) == WAIT_TIMEOUT) {
	CLog::Warning(THISMODULE,"Unable to stop signalling poller thread");
    }
#endif

    // clean events list
    Lock();
    ListEvent::iterator end = _events.end();
    for (ListEvent::iterator it=_events.begin();it != end; it++) {
	delete *it;
    }
    _events.erase(_events.begin(),end);
    Unlock();

    return true;
}

/** Close all call handles which wait for incoming call on a specific ports
 */
void AculabSignalling::RemoveCallHandleOnPort(int port)
{
    for (ListInt::iterator it=_listIncomingHandles.begin();it!=_listIncomingHandles.end();) {
	int handle = *it++;

	SetLockingLocation("Acu:180");
	if (call_handle_2_port(handle) == port) {
	    // disconnect and release any call first
	    CAUSE_XPARMS cause;
	    INIT_ACU_STRUCT(&cause);

	    cause.handle = handle;
	    cause.cause = LC_NORMAL;
	    SetLockingLocation("Acu:190");
	    int rc = call_disconnect(&cause);

	    INIT_ACU_STRUCT(&cause);

	    cause.handle = handle;
	    cause.cause = LC_NORMAL;
	    SetLockingLocation("Acu:200");
	    rc = call_release(&cause);
	    RemoveHandleFromEvents(handle);
	    //***CLog::Detail(THISMODULE,"Closing handle %04X",handle);

	    _listIncomingHandles.remove(handle);
	}
    }
}

/** AttachInterface: A new Interface object is attached to this 
    SignallingDriver object
 */
bool AculabSignalling::AttachInterface(Interface* itf)
{
    _itfAttached++;
    _listInterface.push_back(itf);

    /** Open for incoming call on each timeslot used
     */
    AculabInterface* aitf = (AculabInterface*)itf;
    int port = aitf->GetPort();
    UINT32 mask = aitf->GetTimeslotMask();
    for (UINT32 tslot=0,j=1;tslot<ACU_MAX_TIMESLOT_IN_INTERFACE;tslot++,j<<=1) {
	if (mask & j) {
	    EnableIncomingCall(port,tslot);
	    aitf->BlockTimeslot(tslot,false);
	}
    }
    return true;
}

/** DetachInterface: Remove association between a Interface object and this 
    SignallingDriver object
 */
void AculabSignalling::DetachInterface(Interface* itf)
{
    AculabInterface* aitf = (AculabInterface*)itf;
    RemoveCallHandleOnPort(aitf->GetPort());
    _itfAttached--;
    _listInterface.remove(itf);
}

/** FindInterfaceByPort: Find an interface amongst the attached interfaces having this port
 */
Interface* AculabSignalling::FindInterfaceByPort(int port)
{
    for (ListInterface::iterator it=_listInterface.begin();it!=_listInterface.end();it++) {
	if (((AculabInterface*)(*it))->GetPort() == port) return* it;
    }
    return NULL;
}

/** DisableInterface: an interface has been disabled, the signalling
    driver should take the necessary action if it can
 */
bool AculabSignalling::DisableInterface(Interface* itf)
{
    // force the caller to send ALARM
    return false;
}

/** EnableInterface: an interface has been enabled, the signalling
    driver should take the necessary action if it can
 */
bool AculabSignalling::EnableInterface(Interface* itf)
{
    return true;
}

/** XlatFromDeviceProgress: Translate Aculab specific progress parameter into
    our representation (CallProgress enum)
 */
CallProgress AculabSignalling::XlatFromDeviceProgress(int port,union uniquex* p)
{
    CallProgress cp = CP_NoIndication;
    switch (MapCallType(port)) {
    case S_1TR6:	
	break;

    case S_DASS:	
	break;

    case S_DPNSS:
	break;

    case S_CAS:
	break;

    case S_ISUP:
	if (p->sig_isup.progress_indicator.ie[0]) {
	    cp = (CallProgress)(p->sig_isup.progress_indicator.ie[2] & 0x7f);
	}
	break;

    default:
	if (p->sig_q931.progress_indicator.ie[0]) {
	    cp = (CallProgress)(p->sig_q931.progress_indicator.ie[2] & 0x7f);
	}
    }
    return cp;
}

/** Prepare to receive call on the specified port and
    tslot. It returns call handle or 0 if error.
 */
int AculabSignalling::EnableIncomingCall(int port,int ts)
{
    IN_XPARMS	in_xparms;
    INIT_ACU_STRUCT( &in_xparms );

    in_xparms.net = port;
    in_xparms.cnf = CNF_REM_DISC;
    in_xparms.ts = ts;
    SetLockingLocation("Acu:210");
    int rc = call_openin(&in_xparms);
    if (rc == 0) {
	// Add a handle to incoming call handle list. We need to maintain
	// this list so that we can clean up later
	_listIncomingHandles.push_back(in_xparms.handle);

	//***CLog::Detail(THISMODULE,"Call handle %04X opened on port=%d ts=%d",in_xparms.handle,port,ts);
	idle_net_ts(port,ts);

	return in_xparms.handle;
    } else {
	AculabInterface* itf = (AculabInterface*)FindInterfaceByPort(port);
	CLog::Error(
	    THISMODULE,
	    "Unable to open for incoming call on %s ts=%d (%s)",
	    itf ? itf->GetName() : "(null)", ts,
	    MapErrorToString(rc));
	return 0;
    }
}

/** Called when there is a new incoming call. It creates a call object
    and notifies the Kernel
 */
void AculabSignalling::OnIncomingCall(int handle)
{
    // get the call details
    DETAIL_XPARMS detail;
    INIT_ACU_STRUCT(&detail);
	
    detail.timeout = 0;
    detail.handle = handle;
    SetLockingLocation("Acu:220");
    int rc = call_details(&detail);

    if (rc == 0) {
	// a new incoming call, remove from the list
	_listIncomingHandles.remove(handle);

	// Find the interface first
	SetLockingLocation("Acu:230");

	Interface*	itf = MediaManager::GetInstance()->ItGetFirstInterface();

	while( itf != NULL )
	{
	    if (((AculabInterface*)itf)->GetPort() == call_handle_2_port(handle))
		break;

	    itf = MediaManager::GetInstance()->ItGetNextInterface();
	}

	if (itf) {
	    // attach to a call
	    SetLockingLocation("Acu:240");
	    AculabCall* call = new AculabCall(CD_Incoming,call_handle_2_port(handle),itf);
	    call->SetCallParameterFromDetail(&detail);
	    call->SetCallHandle(handle);
	    call->SetState(CS_Initiating);

	    // add handle to mapping
	    _mapCall[handle] = call;
	    CLog::Detail(THISMODULE,"Attaching handle %04X to call %s",handle,call->GetName());

	    // Allocate the specific media
	    Media* media;
	    if (itf->AllocateMedia(call,media,detail.ts,CD_Incoming,&call->_callParam)) {

		// we must loop back TX/RX during continuity test
		if (!call->_cot_in_progress) {
		    call->SetupInd(call->_callParam);
		} else {
		    if (detail.unique_xparms.sig_isup.continuity_check_ind == CCI_REQUIRED) {
			CLog::Info(THISMODULE, "%s:%02d [%s] looped back for continuity check", itf->GetName(), detail.ts, call->GetName());
			SwitchDevice* swdev = media->GetSwitchDevice();
			if (swdev) swdev->Listen(swdev);
		    }
		}

	    } else {
		//TBD: display error
		call->DisconnectReq(CC_NoCircuitAvailable,CP_NoIndication);
	    }
	} else {
	    //TBD: no interface with such name
	    assert(false);
	}
    }
}

/** Poll and handle Aculab asynchronous events
 */
void AculabSignalling::HandleEvents()
{
    //if (_itfAttached == 0) return;

    // loop while there is any pending event
    int evcount = 0;
    Lock();
    if (_events.empty()) {
	Unlock();
	return;
    }

    ListEvent::iterator end = _events.end();
    while (!_events.empty()) {
	Event* ev = _events.front();
	_events.pop_front();
	_tmpevents.push_back(ev);
	evcount++;
    }
    Unlock();

    STATE_XPARMS event;
    INIT_ACU_STRUCT(&event);

    while (evcount--) {
	
	Event* ev = _tmpevents.front();
	_tmpevents.pop_front();

	event.handle = ev->_handle;
	event.state = ev->_state;
	delete ev;

	CLog::Detail(THISMODULE,"%s on %04X",MapEventToString(event.state),event.handle);

        // map the call handle to device
	MapIntCall::iterator it = _mapCall.find(event.handle);
	if (it != _mapCall.end()) {

	    // an event is received for a particular call
	    AculabCall* call = (AculabCall*)(*it).second;

	    // get the call details
	    DETAIL_XPARMS detail;
	    INIT_ACU_STRUCT(&detail);

	    detail.timeout = 0;
	    detail.handle = event.handle;
	    SetLockingLocation("Acu:250");
	    int rc = call_details(&detail);

	    if (rc == 0) {
		// map received event into XXXInd
		switch (event.state) {
		case EV_CALL_CONNECTED:
		    if (call->GetDirection() == CD_Incoming) {
			call->ConnectCnf();
		    } else {
			call->ConnectInd(0);
		    }
		    break;

		case EV_WAIT_FOR_OUTGOING:
		    SetLockingLocation("Acu:260");
		    if (MapCallType(call_handle_2_port(event.handle)) == S_ISUP) 
			call->SetupCnf();
		    break;

		case EV_OUTGOING_RINGING:
		    if (MapCallType(call->_port) != S_CAS) {
			call->AlertingInd(XlatFromDeviceProgress(call->_port,&detail.unique_xparms), 0);
		    } else {
			call->AlertingInd(CP_InBandInfoAvailable, 0);
		    }
		    break;

		case EV_REMOTE_DISCONNECT:
		    {
			// get cause value
			CAUSE_XPARMS cause;
			INIT_ACU_STRUCT(&cause);

			cause.handle = event.handle;
			SetLockingLocation("Acu:270");
			call_getcause(&cause);

			SetLockingLocation("Acu:280");
			int port = call_handle_2_port(event.handle);
			CallCause cc = ((MapCallType(port) == S_ETS300) && cause.raw) ? (CallCause)cause.raw : MapCauseAculabToQ931(cause.cause);
			call->DisconnectInd(cc,XlatFromDeviceProgress(call->_port,&detail.unique_xparms));
		    }
		    break;

		case EV_IDLE:
		    {
			SetLockingLocation("Acu:290");
			int direction = call_handle_2_io(event.handle);
			int port = call_handle_2_port(event.handle);
			int tslot = detail.valid ? detail.ts : 0;
			Media *media = call->GetMedia(); 
			AculabInterface* itf = media ? (AculabInterface*)media->GetInterface() : NULL;

			//***CLog::Detail(THISMODULE,"Detaching handle %04X from call %s",event.handle,call->GetName());
			_mapCall.erase(event.handle);
			call->SetCallHandle(0);

			// get cause value
			CAUSE_XPARMS cause;
			INIT_ACU_STRUCT(&cause);

			cause.handle = event.handle;
			SetLockingLocation("Acu:300");
			int rc = call_getcause(&cause);
			CallCause cc = ((MapCallType(port) == S_ETS300) && cause.raw) ? (CallCause)cause.raw : MapCauseAculabToQ931(cause.cause);
			call->RelComInd(cc);

			INIT_ACU_STRUCT(&cause);

			cause.handle = event.handle;
			cause.raw = (int)cc;
			SetLockingLocation("Acu:310");
			call_release(&cause);
			RemoveHandleFromEvents(event.handle);
			//***CLog::Detail(THISMODULE,"Closing handle %04X",event.handle);

			// If it is incoming call, re-open it again for incoming call
			if (detail.calltype == INCOMING) {
			    /**
			    AculabInterface* itf = (AculabInterface*)FindInterfaceByPort(port);
			    if (itf && itf->_cfg_enabled) {
			    }
			    **/
			    _listIncomingHandles.remove(event.handle);
			    if (!itf) {
				CLog::Error(THISMODULE,"IDLE received by handle %04X (port=%d, ts=%d) without attached media",event.handle, port, tslot);

				// find interface based on port
				itf = (AculabInterface*)FindInterfaceByPort(port);
			    }

			    if (itf) {
				if (!itf->_interfaceLocked) {
				    EnableIncomingCall(port,tslot);
				}
			    } else {
				CLog::Fatal(THISMODULE,"Can't find interface for port %d", port);
			    }
			} else {
			    idle_net_ts(port,tslot);
			}
		    }
		    break;

		case EV_WAIT_FOR_ACCEPT:
		    // do nothing
		    break;

		case EV_PROGRESS:
		    call->ProgressInd(XlatFromDeviceProgress(call->_port,&detail.unique_xparms), 0);
		    break;

		case EV_OUTGOING_PROCEEDING:
		    call->ProceedingInd(XlatFromDeviceProgress(call->_port,&detail.unique_xparms), 0);
		    break;

		case EV_EXTENDED:
		    break;

		case EV_DETAILS:
		    // check if some more digits are received
		    if (call->GetDirection() == CD_Incoming) {

			CallParameter* cparam = call->GetCallParameter();
			bool complete = detail.sending_complete ? true : false;
			if ((cparam->cnCalled.length() < strlen(detail.destination_addr)) ||
			    (cparam->complete != complete)) {
			    char additionalDigits[MAXNUM+1];
			    strcpy(additionalDigits,detail.destination_addr+cparam->cnCalled.length());

			    // Strip the eventual trailing 'F' to indicate the end
			    // of digits in CAS
			    if (MapCallType(call->_port) == S_CAS) {
				unsigned len = strlen(additionalDigits);
				if (len && (additionalDigits[len-1] == 'F')) {
				    additionalDigits[len-1] = '\0';
				    complete = true;
				}

				// update CLI
				cparam->cnCalling = detail.originating_addr;
			    }

			    // raise overlap indication
			    if (!call->_cot_in_progress) call->OverlapInd(additionalDigits,complete);
			}

			if (call->_cot_in_progress) {
			    if (detail.unique_xparms.sig_isup.continuity_check_ind != CCI_NOT_REQUIRED)
				assert(false);

			    call->_cot_in_progress = false;

			    // undo loopback requested earlier
			    Media* media = call->GetMedia();
			    SwitchDevice* swdev = NULL;
			    if (media) media->GetSwitchDevice();
			    if (swdev) swdev->Unlisten();

			    call->SetupInd(call->_callParam);
			}
		    } else {
			if ((MapCallType(call->_port) != S_CAS) && (call->GetState() == CS_Initiating))
			    call->SetupCnf();
		    }
		    break;

		case EV_NOTIFY:
		    if ((MapCallType(call->_port) == S_ETS300) && (detail.unique_xparms.sig_q931.notify_indicator.ie[0])) {
			switch (detail.unique_xparms.sig_q931.notify_indicator.ie[1] & 0x7f) {
			case 0x00:
			    call->SuspendInd();
			    break;
			case 0x01:
			    call->ResumeInd();
			    break;
			}
		    }
		    break;

		default:
		    break;
		}
	    } else {
		// Error in call_details
		CLog::Fatal(THISMODULE,"Unable to get the details of handle %04X",event.handle);
	    }

        } else {

	    switch (event.state) {
	    case EV_INCOMING_CALL_DET:
		OnIncomingCall(event.handle);
		break;

	    case EV_WAIT_FOR_INCOMING:
		break;

	    case EV_REMOTE_DISCONNECT:
		{
		    // we miss a new incoming call
		    // disconnect call
		    CAUSE_XPARMS cause;
		    INIT_ACU_STRUCT(&cause);

		    cause.handle = event.handle;
		    SetLockingLocation("Acu:320");
		    call_disconnect(&cause);
		}
		break;

	    case EV_IDLE:
		{
		    // we miss a new incoming call
		    // release the call and open a new one
		    SetLockingLocation("Acu:330");
		    int port = call_handle_2_port(event.handle);

		    // make sure we've got it in the list of incoming handles
		    ListInt::iterator it=_listIncomingHandles.begin();
		    ListInt::iterator end=_listIncomingHandles.end();
		    bool incoming = false;
		    for (;it!=end;it++) {
			if ((*it) == event.handle) {
			    _listIncomingHandles.remove(event.handle);
			    incoming = true;
			    break;
			}
		    }

		    // get the call details to find the tslot
		    if (incoming) {
			
			DETAIL_XPARMS detail;
			INIT_ACU_STRUCT(&detail);

			detail.timeout = 0;
			detail.handle = event.handle;
			SetLockingLocation("Acu:340");
			int rc = call_details(&detail);
			if ((rc == 0) && detail.valid) {
			    // we miss a new incoming call
			    // release call and open a new one
			    assert(detail.calltype == INCOMING);
			    
			    CAUSE_XPARMS cause;
			    INIT_ACU_STRUCT(&cause);
			    
			    cause.handle = event.handle;
			    SetLockingLocation("Acu:350");
			    call_release(&cause);
			    RemoveHandleFromEvents(event.handle);
			    //***CLog::Detail(THISMODULE,"Closing handle %04X",event.handle);

			    EnableIncomingCall(port,detail.ts);
			} else {
			    CLog::Fatal(THISMODULE,"Unable to retrieve timeslot value for idle call handle %04X",event.handle);
			}
		    } else {
			CLog::Warning(THISMODULE,"Received spurious EV_IDLE for call handle %04X",event.handle);
		    }
		}
		break;

	    default:
		CLog::Error(THISMODULE,"Spurious event %s on %04X",MapEventToString(event.state),event.handle);
		break;
	    }
        }
    }
}

void AculabSignalling::Lock()
{
#ifndef __linux__
    EnterCriticalSection(&_csec);
#else
    pthread_mutex_lock(&_csec);
#endif
}

void AculabSignalling::Unlock()
{
#ifndef __linux__
    LeaveCriticalSection(&_csec);
#else
    pthread_mutex_unlock(&_csec);
#endif
}

void AculabSignalling::RemoveHandleFromEvents(int handle)
{
    Lock();
    ListEvent::iterator end = _events.end();
    for (ListEvent::iterator it=_events.begin();it != end; ) {
	Event* ev = *it;
	if (ev->_handle == handle) {
	    it = _events.erase(it);
	    delete ev;
	} else {
	    it++;
	}
    }
    Unlock();
}

/** Aculab event poller. Polling is done in a separate thread since this operation
    is blocking and may affect the main thread performance
 */
#ifndef __linux__
DWORD WINAPI AculabSignalling::PollThread(void *arg)
#else
DWORD AculabSignalling::PollThread(void *arg)
#endif
{
    AculabSignalling* instance = AculabSignalling::Instance();
    STATE_XPARMS event;

    int timeout = 512;
    while (!instance->_quit) {
	if (instance->_itfAttached) {
	    
	    INIT_ACU_STRUCT(&event);

	    event.timeout = timeout;

	    DWORD now = GetTickCount();
	    int rc = call_event(&event);
	    DWORD delay = GetTickCount() - now;
	    if (delay > timeout*3/2) CLog::Error(THISMODULE, "Polling signalling took too long (%dms)", delay);

	    if ((rc == 0) && (event.handle != 0)) {
		instance->Lock();
		instance->_events.push_back(new Event(event.handle,event.state));
		instance->Unlock();
	    }
	} else {
	    Sleep(timeout);
	}
    }

    return 0;
}

/** SetupReq:
 */
Call* AculabSignalling::SetupReq(Interface* port,SelectionStrategy selection,CallParameter* cparam)
{
    AculabMedia* media;
    AculabInterface* itf = (AculabInterface*)port;
    if (itf->AllocateMedia(NULL,(Media*&)media,selection,CD_Outgoing,cparam)) {
	int port = itf->_port;
	int ts = media->GetTimeSlot();

	// copy and store call parameters into member variable
	AculabCall* call = new AculabCall(CD_Outgoing,port,itf);
	call->_callParam = *cparam;
	call->MediaAvailableInd(media);

	int handle;
	if (call->DeviceSetupReq(port,ts,itf,&handle,itf->_cfg_appendEOS && (MapCallType(port) == S_CAS))) {
	    // attach media and handle to call
	    call->SetCallHandle(handle);

	    // add handle to mapping
	    _mapCall[handle] = call;
	    CLog::Detail(THISMODULE,"Attaching handle %04X to call %s",handle,call->GetName());
	    return call;
	} else {
	    // Simulate a rejected call with cause CC_InvalidNumberFormat
	    call->SetCause(CC_InvalidNumberFormat);
	    //itf->ReleaseMedia(media,CD_Outgoing,cparam,CC_InvalidNumberFormat);
	    //delete call;
	    //return NULL;
	    return call;
	}
    } else {
	// Simulate a rejected call with cause CC_NoCircuitAvailable
	AculabCall* call = new AculabCall(CD_Outgoing,0,port);
	call->_callParam = *cparam;
	call->SetCause(CC_NoCircuitAvailable);

	//TBD: should I call call->SetupCnf()?

	return call;
    }
}

/** CreateDefaultCallParameter:
 */
CallParameter* AculabSignalling::CreateDefaultCallParameter()
{
    return new CallParameter;
}


/*----------------------------------------------------------------* 
 * Implementation of AculabCall
 *----------------------------------------------------------------*/
AculabCall::AculabCall(CallDirection dir,int port,Interface* itf) : Call(dir,itf->Name().c_str())
{
    _callHandle = 0;
    _port = port;
    _cot_in_progress = false;
}

AculabCall::~AculabCall()
{
    if (_callHandle) {
	AculabSignalling::Instance()->_mapCall.erase(_callHandle);
    }
}

void AculabCall::SetCallHandle(int callHandle)
{
    _callHandle = callHandle;
}

/** Make a call according to call parameters
 */
bool AculabCall::DeviceSetupReq(int port,int tslot,AculabInterface* itf,int* handle,bool appendEOS)
{
    int rc;
    OUT_XPARMS out_xparms;
    int nci = itf->GetNCI();
    int fci = itf->GetFCI();

    // sets the API parameters
    INIT_ACU_STRUCT(&out_xparms);

    out_xparms.net = port;
    out_xparms.ts = tslot;
    out_xparms.cnf = CNF_REM_DISC;
    safe_strcpy(out_xparms.originating_addr,_callParam.cnCalling.c_str(),sizeof(out_xparms.originating_addr));
    safe_strcpy(out_xparms.destination_addr,_callParam.cnCalled.c_str(),sizeof(out_xparms.destination_addr));
    if (_callParam.complete) out_xparms.sending_complete = 1;

    // sets the network specific parameters
    switch (MapCallType(port)) {
    case S_DASS:	
        out_xparms.unique_xparms.sig_dass.sic1 = 0x10;
        out_xparms.unique_xparms.sig_dass.sic2 = 0xff;
        break;

    case S_DPNSS:
        out_xparms.unique_xparms.sig_dpnss.sic1 = 0x10;
        out_xparms.unique_xparms.sig_dpnss.sic2 = 0xff;
        break;

    case S_CAS:
	if (appendEOS && _callParam.complete) strcat(out_xparms.destination_addr,"F");
        break;

    case S_ISUP:
        out_xparms.unique_xparms.sig_isup.dest_numbering_plan = _callParam.cnpCalled;
        out_xparms.unique_xparms.sig_isup.dest_natureof_addr = XlatToDeviceIsupNOA(_callParam.cntCalled);
        out_xparms.unique_xparms.sig_isup.orig_numbering_plan = _callParam.cnpCalling;
        out_xparms.unique_xparms.sig_isup.orig_natureof_addr = XlatToDeviceIsupNOA(_callParam.cntCalling);
	out_xparms.unique_xparms.sig_isup.orig_numbering_screening = _callParam.ccs;
	out_xparms.unique_xparms.sig_isup.orig_numbering_presentation = (_callParam.cnCalling.length()==0) ? CCP_NotAvailable : _callParam.ccp;
	out_xparms.unique_xparms.sig_isup.orig_category = _callParam.cpcCalling;
	out_xparms.unique_xparms.sig_isup.nat_inter_call_ind = (fci >> 0) & 0x01;
	out_xparms.unique_xparms.sig_isup.interworking_ind = (fci >> 3) & 0x01;
	out_xparms.unique_xparms.sig_isup.isdn_userpart_ind = (fci >> 5) & 0x01;
	out_xparms.unique_xparms.sig_isup.isdn_userpart_pref_ind = (fci >> 6) & 0x03;
	out_xparms.unique_xparms.sig_isup.isdn_access_ind = (fci >> 8) & 0x01;
	out_xparms.unique_xparms.sig_isup.continuity_check_ind = (nci >> 2) & 0x03;
	out_xparms.unique_xparms.sig_isup.satellite_ind = (nci >> 0) & 0x03;

	// sub address
	FillSubaddress(out_xparms.unique_xparms.sig_isup.dest_subaddr,_callParam.csCalled.c_str());
	FillSubaddress(out_xparms.unique_xparms.sig_isup.orig_subaddr,_callParam.csCalling.c_str());

	// bearer capabilities
	switch (_callParam.cct) {
	case CCT_Speech:
	    out_xparms.unique_xparms.sig_isup.bearer.ie[0] = 3;
	    out_xparms.unique_xparms.sig_isup.bearer.ie[1] = 0x80;
	    out_xparms.unique_xparms.sig_isup.bearer.ie[2] = 0x90;
	    out_xparms.unique_xparms.sig_isup.bearer.ie[3] = (_callParam.ccl == CCL_MuLaw) ? 0xA2 : 0xA3;
	    out_xparms.unique_xparms.sig_isup.service_octet = TELEPHONY;
	    out_xparms.unique_xparms.sig_isup.add_info_octet = ANALOGUE;
	    break;
	case CCT_3K1Audio:
	    out_xparms.unique_xparms.sig_isup.bearer.ie[0] = 3;
	    out_xparms.unique_xparms.sig_isup.bearer.ie[1] = 0x90;
	    out_xparms.unique_xparms.sig_isup.bearer.ie[2] = 0x90;
	    out_xparms.unique_xparms.sig_isup.bearer.ie[3] = (_callParam.ccl == CCL_MuLaw) ? 0xA2 : 0xA3;
	    out_xparms.unique_xparms.sig_isup.service_octet = TELEPHONY;
	    out_xparms.unique_xparms.sig_isup.add_info_octet = ISDN_3K1;
	    break;
	case CCT_64KUnrestrictred:
	    out_xparms.unique_xparms.sig_isup.bearer.ie[0] = 2;
	    out_xparms.unique_xparms.sig_isup.bearer.ie[1] = 0x88;
	    out_xparms.unique_xparms.sig_isup.bearer.ie[2] = 0x90;
	    out_xparms.unique_xparms.sig_isup.service_octet = DATA64K;
	    break;
	}
	break;

    default:
        out_xparms.unique_xparms.sig_q931.dest_numbering_plan = _callParam.cnpCalled;
        out_xparms.unique_xparms.sig_q931.dest_numbering_type = _callParam.cntCalled;
        out_xparms.unique_xparms.sig_q931.orig_numbering_plan = _callParam.cnpCalling;
        out_xparms.unique_xparms.sig_q931.orig_numbering_type = _callParam.cntCalling;
	out_xparms.unique_xparms.sig_q931.orig_numbering_screening = _callParam.ccs;
	out_xparms.unique_xparms.sig_q931.orig_numbering_presentation = (_callParam.cnCalling.length()==0) ? CCP_NotAvailable : _callParam.ccp;;

	// sub address
	FillSubaddress(out_xparms.unique_xparms.sig_q931.dest_subaddr,_callParam.csCalled.c_str());
	FillSubaddress(out_xparms.unique_xparms.sig_q931.orig_subaddr,_callParam.csCalling.c_str());

	// bearer capabilities
	switch (_callParam.cct) {
	case CCT_Speech:
	    out_xparms.unique_xparms.sig_q931.bearer.ie[0] = 3;
	    out_xparms.unique_xparms.sig_q931.bearer.ie[1] = 0x80;
	    out_xparms.unique_xparms.sig_q931.bearer.ie[2] = 0x90;
	    out_xparms.unique_xparms.sig_q931.bearer.ie[3] = (_callParam.ccl == CCL_MuLaw) ? 0xA2 : 0xA3;
	    out_xparms.unique_xparms.sig_q931.service_octet = TELEPHONY;
	    out_xparms.unique_xparms.sig_q931.add_info_octet = ANALOGUE;
	    break;
	case CCT_3K1Audio:
	    out_xparms.unique_xparms.sig_q931.bearer.ie[0] = 3;
	    out_xparms.unique_xparms.sig_q931.bearer.ie[1] = 0x90;
	    out_xparms.unique_xparms.sig_q931.bearer.ie[2] = 0x90;
	    out_xparms.unique_xparms.sig_q931.bearer.ie[3] = (_callParam.ccl == CCL_MuLaw) ? 0xA2 : 0xA3;
	    out_xparms.unique_xparms.sig_q931.service_octet = TELEPHONY;
	    out_xparms.unique_xparms.sig_q931.add_info_octet = ISDN_3K1;
	    break;
	case CCT_64KUnrestrictred:
	    out_xparms.unique_xparms.sig_q931.bearer.ie[0] = 2;
	    out_xparms.unique_xparms.sig_q931.bearer.ie[1] = 0x88;
	    out_xparms.unique_xparms.sig_q931.bearer.ie[2] = 0x90;
	    out_xparms.unique_xparms.sig_q931.service_octet = DATA64K;
	    break;
	}
	break;
    }

    SetLockingLocation("Acu:360");
    rc = call_openout(&out_xparms);
    if (rc == 0) {
	*handle = out_xparms.handle;

	CLog::Detail(THISMODULE,"Call handle %04X opened on port=%d ts=%d",out_xparms.handle,port,tslot);

        return true;
    } else {
	CLog::Error(
	    THISMODULE,
	    GetTask(),
	    "Unable to initiate an outgoing call to %s on port=%d,ts=%d (%s)",
	    _callParam.cnCalled.c_str(),
	    port,tslot,
	    MapErrorToString(rc));
    }
    return false;
}


int AculabCall::XlatToDeviceIsupNOA(CallNumberType cnt)
{
    switch (cnt) {
    case CNT_National:
	return 3;
    case CNT_International:
	return 4;
    case CNT_SubscriberNumber:
	return 1;
    default:
	return 2;
    }
}


int AculabCall::XlatToDeviceNumberPlan(CallNumberPlan cnp)
{
    switch (cnp) {
    case CNP_ISDN:
	return NP_ISDN;
    case CNP_Data:
	return NP_DATA;
    case CNP_Telex:
	return NP_TELEX;
    case CNP_NationalStandard:
	return NP_NATIONAL_STANDARD;
    case CNP_Private:
	return NP_PRIVATE;
    case CNP_Unknown:
    default:
	return NP_UNKNOWN;
    }
}

void AculabCall::FillSubaddress(unsigned char* subaddr,const char* rawstr)
{
    if (strlen(rawstr) == 0) return;

    subaddr[0] = strlen(rawstr)+2;
    subaddr[1] = 0x80;	    // NSAP format
    subaddr[2] = 0x50;	    // AFI
    safe_strcpy((char*)subaddr+3,rawstr,MAXNUM-3);
}


static const char* ExtractSubaddress(unsigned char* subaddr)
{
    static char buffer[MAXNUM+1];
    int len = subaddr[0];

    if (len == 0) return "";
    if (len>MAXNUM) len = MAXNUM;

    safe_strcpy(buffer, (char*)subaddr+3, len);
    return buffer;
}


static CallNumberType XlatFromIsupNOA(int noa)
{
    switch (noa) {
    case 1:
	return CNT_SubscriberNumber;
    case 3:
	return CNT_National;
    case 4:
	return CNT_International;
    case 2:
    default:
	return CNT_Unknown;
    }
}



/** Convert from call details in Aculab format to the standard call parameter
 */
void AculabCall::SetCallParameterFromDetail(struct detail_xparms* detail)
{
     _callHandle = detail->handle;
     _callParam.cnCalled = detail->destination_addr;
     _callParam.cnCalling = detail->originating_addr;
     _callParam.complete = detail->sending_complete != 0;

    switch (MapCallType(_port)) {
    case S_1TR6:	
        break;

    case S_DASS:	
        break;

    case S_DPNSS:
        break;

    case S_CAS:
        break;

    case S_ISUP:
	_callParam.cnpCalled = (CallNumberPlan) detail->unique_xparms.sig_isup.dest_numbering_plan;
	_callParam.cntCalled = XlatFromIsupNOA(detail->unique_xparms.sig_isup.dest_natureof_addr);
	_callParam.cnpCalling = (CallNumberPlan) detail->unique_xparms.sig_isup.orig_numbering_plan;
	_callParam.cntCalling = XlatFromIsupNOA(detail->unique_xparms.sig_isup.orig_natureof_addr);
	_callParam.ccs = (CallCallingScreening) detail->unique_xparms.sig_isup.orig_numbering_screening;
	_callParam.cpcCalling = (CallPartyCategory) detail->unique_xparms.sig_isup.orig_category;

	// sub address
	_callParam.csCalled = ExtractSubaddress(detail->unique_xparms.sig_isup.dest_subaddr);
	_callParam.csCalling = ExtractSubaddress(detail->unique_xparms.sig_isup.orig_subaddr);

	// detect continuity check request embedded in IAM
	if (detail->unique_xparms.sig_isup.continuity_check_ind)
	    _cot_in_progress = true;

	if (detail->unique_xparms.sig_isup.service_octet == TELEPHONY) {
	    switch (detail->unique_xparms.sig_isup.add_info_octet) {
	    case ANALOGUE:
		_callParam.cct = CCT_Speech;
		break;
	    case ISDN_3K1:
	    case ISDN_7K:
		_callParam.cct = CCT_3K1Audio;
		break;
	    }
	} else if (detail->unique_xparms.sig_isup.service_octet == DATA64K) {
	    _callParam.cct = CCT_64KUnrestrictred;
	} else {
	    //TBD:
	}
	break;

    default:
        _callParam.cnpCalled = (CallNumberPlan)detail->unique_xparms.sig_q931.dest_numbering_plan;
        _callParam.cntCalled = (CallNumberType)detail->unique_xparms.sig_q931.dest_numbering_type;
        _callParam.cnpCalling = (CallNumberPlan)detail->unique_xparms.sig_q931.orig_numbering_plan;
        _callParam.cntCalling = (CallNumberType)detail->unique_xparms.sig_q931.orig_numbering_type;
	_callParam.ccs = (CallCallingScreening)detail->unique_xparms.sig_q931.orig_numbering_screening;
	_callParam.ccp = (CallCallingPresentation)detail->unique_xparms.sig_q931.orig_numbering_presentation;

	// sub address
	_callParam.csCalled = ExtractSubaddress(detail->unique_xparms.sig_q931.dest_subaddr);
	_callParam.csCalling = ExtractSubaddress(detail->unique_xparms.sig_q931.orig_subaddr);

	if (detail->unique_xparms.sig_q931.service_octet == TELEPHONY) {
	    switch (detail->unique_xparms.sig_q931.add_info_octet) {
	    case ANALOGUE:
		_callParam.cct = CCT_Speech;
		break;
	    case ISDN_3K1:
	    case ISDN_7K:
		_callParam.cct = CCT_3K1Audio;
		break;
	    }
	} else if (detail->unique_xparms.sig_q931.service_octet == DATA64K) {
	    _callParam.cct = CCT_64KUnrestrictred;
	} else {
	    //TBD:
	}
	break;
    }
}

bool AculabCall::DeviceOverlapReq(const char* additionalDigits,bool complete)
{
    OVERLAP_XPARMS overlap;
    INIT_ACU_STRUCT(&overlap);

    overlap.handle = _callHandle;
    overlap.sending_complete = complete;
    safe_strcpy(overlap.destination_addr,additionalDigits,sizeof(overlap.destination_addr));
    SetLockingLocation("Acu:370");
    int rc = call_send_overlap(&overlap);
    return (rc == 0);
}

bool AculabCall::DeviceProceedingReq(CallProgress cp, CallExtraInfo* exinfo)
{
    PROCEEDING_XPARMS proceeding;
    INIT_ACU_STRUCT(&proceeding);
    
    proceeding.handle = _callHandle;
    if (cp != CP_NoIndication) {
	switch (MapCallType(_port)) {
	case S_ISUP:
	    proceeding.unique_xparms.sig_isup.progress_indicator.ie[0] = 2;
	    proceeding.unique_xparms.sig_isup.progress_indicator.ie[1] = 0x80;
	    proceeding.unique_xparms.sig_isup.progress_indicator.ie[2] = 0x80 | (unsigned char)cp;
	    break;

	case S_ETS300:
	    proceeding.unique_xparms.sig_q931.progress_indicator.ie[0] = 2;
	    proceeding.unique_xparms.sig_q931.progress_indicator.ie[1] = 0x80;
	    proceeding.unique_xparms.sig_q931.progress_indicator.ie[2] = 0x80 | (unsigned char)cp;
	    break;
	}
    }
    SetLockingLocation("Acu:380");
    int rc = call_proceeding(&proceeding);
    return (rc == 0);
}

bool AculabCall::DeviceProgressReq(CallProgress cp, CallExtraInfo* exinfo)
{
    PROGRESS_XPARMS progress;
    INIT_ACU_STRUCT(&progress);

    progress.handle = _callHandle;
    if (cp != CP_NoIndication) {
	switch (MapCallType(_port)) {
	case S_ISUP:
	    progress.unique_xparms.sig_isup.progress_indicator.ie[0] = 2;
	    progress.unique_xparms.sig_isup.progress_indicator.ie[1] = 0x80;
	    progress.unique_xparms.sig_isup.progress_indicator.ie[2] = 0x80 | (unsigned char)cp;
	    break;

	case S_ETS300:
	    progress.unique_xparms.sig_q931.progress_indicator.ie[0] = 2;
	    progress.unique_xparms.sig_q931.progress_indicator.ie[1] = 0x80;
	    progress.unique_xparms.sig_q931.progress_indicator.ie[2] = 0x80 | (unsigned char)cp;
	    break;
	}
    }
    SetLockingLocation("Acu:390");
    int rc = call_progress(&progress);
    return (rc == 0);
}

bool AculabCall::DeviceAlertingReq(CallProgress cp, CallExtraInfo* exinfo)
{
    INCOMING_RINGING_XPARMS alerting;
    INIT_ACU_STRUCT(&alerting);
    
    alerting.handle = _callHandle;
    if (cp != CP_NoIndication) {
	switch (MapCallType(_port)) {
	case S_ISUP:
	    alerting.unique_xparms.sig_isup.progress_indicator.ie[0] = 2;
	    alerting.unique_xparms.sig_isup.progress_indicator.ie[1] = 0x80;
	    alerting.unique_xparms.sig_isup.progress_indicator.ie[2] = 0x80 | (unsigned char)cp;
	    break;

	case S_ETS300:
	    alerting.unique_xparms.sig_q931.progress_indicator.ie[0] = 2;
	    alerting.unique_xparms.sig_q931.progress_indicator.ie[1] = 0x80;
	    alerting.unique_xparms.sig_q931.progress_indicator.ie[2] = 0x80 | (unsigned char)cp;
	    break;
	}
    }
    SetLockingLocation("Acu:400");
    int rc = xcall_incoming_ringing(&alerting);
    return (rc == 0);
}

bool AculabCall::DeviceSendInfo(CallExtraInfo* exinfo)
{
        return(0);
}

bool AculabCall::DeviceConnectReq(CallExtraInfo* exinfo)
{
    SetLockingLocation("Acu:410");
    int rc = call_accept(_callHandle);
    return (rc == 0);
}

bool AculabCall::DeviceSuspendReq()
{
    if (MapCallType(_port) == S_ETS300) {
	
	NOTIFY_XPARMS notify;
	INIT_ACU_STRUCT(&notify);
	
	notify.handle = _callHandle;
	notify.unique_xparms.sig_q931.notify_indicator.ie[0] = 1;
	notify.unique_xparms.sig_q931.notify_indicator.ie[0] = 0x80;
	SetLockingLocation("Acu:420");
	int rc = call_notify(&notify);
	return (rc == 0);
    }
    return true;
}

bool AculabCall::DeviceResumeReq()
{
    if (MapCallType(_port) == S_ETS300) {
	
	NOTIFY_XPARMS notify;
	INIT_ACU_STRUCT(&notify);

	notify.handle = _callHandle;
	notify.unique_xparms.sig_q931.notify_indicator.ie[0] = 1;
	notify.unique_xparms.sig_q931.notify_indicator.ie[0] = 0x81;
	SetLockingLocation("Acu:430");
	int rc = call_notify(&notify);
	return (rc == 0);
    }
    return true;
}

bool AculabCall::DeviceDisconnectReq(CallCause causeCode,CallProgress cp)
{
    DISCONNECT_XPARMS cause;
    INIT_ACU_STRUCT(&cause);

    cause.handle = _callHandle;
    cause.cause = 0;
    switch (MapCallType(_port)) {
    case S_ETS300:
	cause.unique_xparms.sig_q931.raw = causeCode;
	if (cp != CP_NoIndication) {
	    cause.unique_xparms.sig_q931.progress_indicator.ie[0] = 2;
	    cause.unique_xparms.sig_q931.progress_indicator.ie[1] = 0x80;
	    cause.unique_xparms.sig_q931.progress_indicator.ie[2] = 0x80 | (unsigned char)cp;
	}
	break;

    case S_ISUP:
	cause.unique_xparms.sig_isup.raw = causeCode;
	if (cp != CP_NoIndication) {
	    cause.unique_xparms.sig_isup.progress_indicator.ie[0] = 2;
	    cause.unique_xparms.sig_isup.progress_indicator.ie[1] = 0x80;
	    cause.unique_xparms.sig_isup.progress_indicator.ie[2] = 0x80 | (unsigned char)cp;
	}
	break;

    case S_DPNSS:
    case S_DASS:
    case S_CAS:
    default:
	cause.cause = MapCauseQ931ToAculab(causeCode);
	break;
    }
    SetLockingLocation("Acu:440");
    int rc = xcall_disconnect(&cause);
    return (rc == 0);
}

#endif
// XGATE_HAS_ACULAB
