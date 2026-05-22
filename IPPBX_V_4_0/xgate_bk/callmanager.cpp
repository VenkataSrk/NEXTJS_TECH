/*----------------------------------------------------------------* 
 * $Archive: /xGate/callmanager.cpp $
 * $Date: 7/13/04 2:02p $
 * $Revision: 133 $
 *
 * $History: callmanager.cpp $
 * 
 * *****************  Version 134  *****************
 * User: okY          Date: 11/7/13    Time: 10:45a
 * Updated in $/xGate
 * Add API to set call credential for outgoing call(may required for e.g as response to 401/407 in SIP)
 *
 * *****************  Version 133  *****************
 * User: Bennylp      Date: 7/13/04    Time: 2:02p
 * Updated in $/xGate
 * Version 57.1 (ResumeTask/SuspendTask thing -awal)
 * 
 * *****************  Version 132  *****************
 * User: Bennylp      Date: 7/13/04    Time: 12:07p
 * Updated in $/xGate
 * suspend task during answer
 * 
 * *****************  Version 131  *****************
 * User: Bennylp      Date: 7/08/04    Time: 6:49p
 * Updated in $/xGate
 * Call Gapping
 * 
 * *****************  Version 130  *****************
 * User: Bennylp      Date: 3/16/04    Time: 10:45a
 * Updated in $/xGate
 * Added API CallGetAuthorization
 * 
 * *****************  Version 129  *****************
 * User: Bennylp      Date: 2/20/04    Time: 10:21a
 * Updated in $/xGate
 * Fixed bug that CallDescription is not a full class, and also it is
 * zeroed.
 * 
 * *****************  Version 128  *****************
 * User: Bennylp      Date: 1/09/04    Time: 6:27p
 * Updated in $/xGate
 * Move some implementation from callmanager.h and fixed memory leak when
 * trace condition regular expression is not deleted.
 * 
 * *****************  Version 127  *****************
 * User: Sonny        Date: 12/29/03   Time: 12:48p
 * Updated in $/xGate
 * callsetup is blocking again
 * 
 * *****************  Version 126  *****************
 * User: Junanto      Date: 19/09/03   Time: 17:37
 * Updated in $/xGate
 * - added new API SetCharging
 * - more verbose call details
 * - CallSetup does not block
 * 
 * *****************  Version 123  *****************
 * User: Bennylp      Date: 3/06/03    Time: 5:11p
 * Updated in $/xGate
 * Changed trace format to support per-user trace
 * 
 * *****************  Version 122  *****************
 * User: Bennylp      Date: 1/31/03    Time: 2:24p
 * Updated in $/xGate
 * Changed 'call' command privilege to Normal
 * 
 * *****************  Version 121  *****************
 * User: Bennylp      Date: 11/20/02   Time: 6:17p
 * Updated in $/xGate
 * Trim DDI digit for "call quality-list output"
 * 
* *****************  Version 120  *****************
* User: Bennylp      Date: 10/25/02   Time: 8:00p
* Updated in $/xGate
* Change the output of "call quality-list" to show details media name
* 
* *****************  Version 119  *****************
* User: Bennylp      Date: 9/17/02    Time: 6:07p
* Updated in $/xGate
* Fixed UMR: uninitialized CallDescription.
* 
* *****************  Version 118  *****************
* User: Junanto      Date: 1/07/02    Time: 12:39
* Updated in $/xGate
* removed unnecssary warning in pipes during shutdown
* 
* *****************  Version 117  *****************
* User: Junanto      Date: 14/06/02   Time: 21:51
* Updated in $/xGate
* Fixed incorrect call state during ISUP outgoing overlap 
* sending mode
* 
* *****************  Version 116  *****************
* User: Junanto      Date: 6/06/02    Time: 13:25
* Updated in $/xGate
* Corrected behaviour of SignallingConnect to pass
* the last progress indication IE
* 
* *****************  Version 115  *****************
* User: Junanto      Date: 28/05/02   Time: 10:19
* Updated in $/xGate
* fixed bug when call is waiting for more
* digits and detached from task
* 
* *****************  Version 114  *****************
* User: Junanto      Date: 27/05/02   Time: 17:49
* Updated in $/xGate
* Changed external naming from maxdigits to mindigits
* 
* *****************  Version 113  *****************
* User: Junanto      Date: 27/05/02   Time: 10:05
* Updated in $/xGate
* Added CallGetMoreDigitsEx which adds max. number of digits.
* This function will block until either one of this condition occurs:
*   - max. number of digits collected
*   - "number complete" received
*   - interdigit time out
* 
* *****************  Version 112  *****************
* User: Junanto      Date: 26/05/02   Time: 17:23
* Updated in $/xGate
* Implemented CallGetMoreDigits
* 
* *****************  Version 111  *****************
* User: Adham        Date: 8/03/02    Time: 18:36
* Updated in $/xGate
* The class declaration of CallExpression and CallTraceCondition moved to
* header file
* 
* *****************  Version 110  *****************
* User: Sonny        Date: 13/02/02   Time: 15:04
* Updated in $/xGate
* Quality monitoring updates
* 
* *****************  Version 109  *****************
* User: Sonny        Date: 7/02/02    Time: 12:32
* Updated in $/xGate
* 
* *****************  Version 108  *****************
* User: Sonny        Date: 6/02/02    Time: 19:01
* Updated in $/xGate
* optimizations for quality monitoring
* 
* *****************  Version 107  *****************
* User: Sonny        Date: 4/02/02    Time: 18:03
* Updated in $/xGate
* show packets instead of id and ie
* 
* *****************  Version 106  *****************
* User: Sonny        Date: 1/02/02    Time: 13:13
* Updated in $/xGate
* 
* *****************  Version 105  *****************
* User: Sonny        Date: 31/01/02   Time: 13:11
* Updated in $/xGate
* 
* *****************  Version 104  *****************
* User: Sonny        Date: 24/01/02   Time: 20:58
* Updated in $/xGate
* Quality monitoring support
* 
* *****************  Version 103  *****************
* User: Bennylp      Date: 11/15/01   Time: 11:56a
* Updated in $/xGate
* Implement API CallExSetDisconnectMsg
* 
* *****************  Version 102  *****************
* User: Junanto      Date: 31/10/01   Time: 16:42
* Updated in $/xGate
* Prevent calls from being made on disabled interfaces
* 
* *****************  Version 101  *****************
* User: Junanto      Date: 29/10/01   Time: 19:15
* Updated in $/xGate
* Added "call list" by user-info or session
* 
* *****************  Version 100  *****************
* User: Junanto      Date: 29/10/01   Time: 19:00
* Updated in $/xGate
* Shortened tag in ctrace-filter
* 
* *****************  Version 99  *****************
* User: Junanto      Date: 29/10/01   Time: 18:29
* Updated in $/xGate
* Added userinfo, session and direction in ctrace-filter command
* 
* *****************  Version 98  *****************
* User: Junanto      Date: 17/10/01   Time: 14:46
* Updated in $/xGate
* Added conditional compilation to include/exclude
* call transition logging
* 
* *****************  Version 97  *****************
* User: Bennylp      Date: 10/11/01   Time: 5:44p
* Updated in $/xGate
* Free reqular expression in call list.
* 
* *****************  Version 96  *****************
* User: Bennylp      Date: 5/10/01    Time: 19:44
* Updated in $/xGate
* In OnShutdown(), call RelComInd() if state==NULL and !_release.
* 
* *****************  Version 95  *****************
* User: Bennylp      Date: 10/05/01   Time: 7:22p
* Updated in $/xGate
* Fixed GPF related to OnShutdown() when the call has been deleted
* 
* *****************  Version 94  *****************
* User: Junanto      Date: 1/10/01    Time: 14:21
* Updated in $/xGate
* Changed warning to info for invalid call handle
* 
* *****************  Version 93  *****************
* User: Bennylp      Date: 8/28/01    Time: 7:57p
* Updated in $/xGate
* Option to include/exclide H.323, SIP, and ATM in compilation.
* 
* *****************  Version 92  *****************
* User: Adham        Date: 8/28/01    Time: 7:16p
* Updated in $/xGate
* Option to exclude/include H.323 from compilation
* 
* *****************  Version 91  *****************
* User: Adham        Date: 8/24/01    Time: 4:59p
* Updated in $/xGate
* h323 conditional compile
* 
* *****************  Version 90  *****************
* User: Adham        Date: 8/21/01    Time: 6:32p
* Updated in $/xGate
* Adding H323_Signalling::Create
* 
* *****************  Version 89  *****************
* User: Bennylp      Date: 8/20/01    Time: 4:26p
* Updated in $/xGate
* Don't call OnShutdown in Unload
* 
* *****************  Version 88  *****************
* User: Bennylp      Date: 8/08/01    Time: 1:48p
* Updated in $/xGate
* Added SIP signalling.
* 
* *****************  Version 87  *****************
* User: Sonny        Date: 3/08/01    Time: 12:15
* Updated in $/xGate
* Set state to CS_Initiating after SetupReq
* 
* *****************  Version 86  *****************
* User: Junanto      Date: 1/08/01    Time: 17:13
* Updated in $/xGate
* Corrected invalid specification format in ValidCallParameter
* 
* *****************  Version 85  *****************
* User: Bennylp      Date: 7/31/01    Time: 12:11p
* Updated in $/xGate
* Added more cause codes.
* 
* *****************  Version 84  *****************
* User: Bennylp      Date: 7/30/01    Time: 12:34p
* Updated in $/xGate
* Disconnect calls in OnShutdown(), not only in Unload().
* 
* *****************  Version 83  *****************
* User: Junanto      Date: 30/07/01   Time: 11:35
* Updated in $/xGate
* return code is set to 0 during first call to CallSetup
* 
* *****************  Version 82  *****************
* User: Junanto      Date: 25/07/01   Time: 17:01
* Updated in $/xGate
* Moved RTP signalling/interface creation from Kernel to CallManager
* 
* *****************  Version 81  *****************
* User: Junanto      Date: 23/07/01   Time: 22:13
* Updated in $/xGate
* Present call disconnected state to the application for early RelComInd
* StrCallState moved to the call manager
* The declaration of some APIs are changed to synchronous
* 
* *****************  Version 80  *****************
* User: Junanto      Date: 22/07/01   Time: 20:47
* Updated in $/xGate
* Added warning if CallGetCause is called before being assigned
* 
* *****************  Version 79  *****************
* User: Junanto      Date: 20/07/01   Time: 20:08
* Updated in $/xGate
* Changed the way CancelJob works: disconnect pending call
* rather than ResumeTask
* 
* *****************  Version 78  *****************
* User: Junanto      Date: 20/07/01   Time: 18:51
* Updated in $/xGate
* Fixed bug when task is woken up more than once
* 
* *****************  Version 77  *****************
* User: Junanto      Date: 20/07/01   Time: 17:19
* Updated in $/xGate
* Added protection in CallSignallingConnect when 
* passed invalid handle
* 
* *****************  Version 76  *****************
* User: Bennylp      Date: 7/20/01    Time: 4:55p
* Updated in $/xGate
* CancelJob will check whether task state is blocked.
* 
* *****************  Version 75  *****************
* User: Junanto      Date: 19/07/01   Time: 20:48
* Updated in $/xGate
* Changed formatting of "call detail"
* 
* *****************  Version 74  *****************
* User: Junanto      Date: 19/07/01   Time: 19:33
* Updated in $/xGate
* Added information displayed in "call detail"
* Corrected bug info "call list task"
* 
* *****************  Version 73  *****************
* User: Bennylp      Date: 7/18/01    Time: 8:44p
* Updated in $/xGate
* Call manager will destroy calls in Unload.
* 
* *****************  Version 72  *****************
* User: Junanto      Date: 18/07/01   Time: 19:00
* Updated in $/xGate
* List call by task ID
* 
* *****************  Version 71  *****************
* User: Junanto      Date: 16/07/01   Time: 22:44
* Updated in $/xGate
* Delayed RelComInd for failed Setup
* 
* *****************  Version 70  *****************
* User: Bennylp      Date: 7/16/01    Time: 4:37p
* Updated in $/xGate
* Resume task in CancelJob.
* 
* *****************  Version 69  *****************
* User: Junanto      Date: 15/07/01   Time: 22:25
* Updated in $/xGate
* Add command "call disconnect"
* 
* *****************  Version 68  *****************
* User: Junanto      Date: 15/07/01   Time: 19:23
* Updated in $/xGate
* Added new command "call detail" to see the detail of a call
* including call transition
* 
* *****************  Version 67  *****************
* User: Bennylp      Date: 7/15/01    Time: 7:03p
* Updated in $/xGate
* RelComInd will not immediately delete call instance.
* 
* *****************  Version 66  *****************
* User: Junanto      Date: 15/07/01   Time: 16:26
* Updated in $/xGate
* 
* *****************  Version 65  *****************
* User: Sonny        Date: 14/07/01   Time: 12:10
* Updated in $/xGate
* add dialogic
* 
* *****************  Version 64  *****************
* User: Junanto      Date: 10/07/01   Time: 19:06
* Updated in $/xGate
* Changed interface of Call class constructor
* 
* *****************  Version 63  *****************
* User: Junanto      Date: 2/07/01    Time: 20:59
* Updated in $/xGate
* Added trace call and call list by group and private info
* 
* *****************  Version 62  *****************
* User: Bennylp      Date: 6/30/01    Time: 11:36p
* Updated in $/xGate
* Misc. bug fixes and improvement (see rel notes in ver. 32).
* 
* *****************  Version 61  *****************
* User: Junanto      Date: 29/06/01   Time: 16:48
* Updated in $/xGate
* Added new application APIs for:
* - retrieving current call parameter
* - retrieving interface name
* - retrieving group name
* 
* *****************  Version 60  *****************
* User: Junanto      Date: 28/06/01   Time: 21:10
* Updated in $/xGate
* Fixed bug in auto-call-proceeding handling
* 
* *****************  Version 59  *****************
* User: Junanto      Date: 27/06/01   Time: 22:41
* Updated in $/xGate
* 
* *****************  Version 58  *****************
* User: Junanto      Date: 26/06/01   Time: 10:58
* Updated in $/xGate
* Added new parameter in application "call-proceeding"
* which indicates whether a PROCEEDING needs to
* be sent automatically during incoming call or not
* 
* *****************  Version 57  *****************
* User: Junanto      Date: 25/06/01   Time: 16:39
* Updated in $/xGate
* CallSetup may specify wild card (any port with a 
    * particular group name)
* 
* *****************  Version 56  *****************
* User: Bennylp      Date: 6/22/01    Time: 7:40p
* Updated in $/xGate
* Changed 'bool' arguments to int in API functions.
* 
* *****************  Version 55  *****************
* User: Junanto      Date: 19/06/01   Time: 21:27
* Updated in $/xGate
* Pending overlap digits are sent during CallSignallingConnect
* 
* *****************  Version 54  *****************
* User: Junanto      Date: 19/06/01   Time: 17:16
* Updated in $/xGate
* CallGetCallParam now uses task local storage
* 
* *****************  Version 53  *****************
* User: Junanto      Date: 19/06/01   Time: 16:04
* Updated in $/xGate
* 
* *****************  Version 52  *****************
* User: Junanto      Date: 9/06/01    Time: 19:55
* Updated in $/xGate
* Protect "call list" and "trace call" against GPF when
* call doesn't have media
* 
* *****************  Version 51  *****************
* User: Junanto      Date: 8/06/01    Time: 22:25
* Updated in $/xGate
* 
* *****************  Version 50  *****************
* User: Junanto      Date: 8/06/01    Time: 21:36
* Updated in $/xGate
* 
* *****************  Version 49  *****************
* User: Junanto      Date: 8/06/01    Time: 17:41
* Updated in $/xGate
* 
* *****************  Version 48  *****************
* User: Junanto      Date: 8/06/01    Time: 16:51
* Updated in $/xGate
* Added new command to list current active calls
* 
* *****************  Version 47  *****************
* User: Junanto      Date: 8/06/01    Time: 10:37
* Updated in $/xGate
* Removed some CLog::Detail
* 
* *****************  Version 46  *****************
* User: Junanto      Date: 7/06/01    Time: 17:00
* Updated in $/xGate
* Added selective call trace
* 
* *****************  Version 45  *****************
* User: Junanto      Date: 2/06/01    Time: 15:04
* Updated in $/xGate
* Changed trace output
* 
* *****************  Version 44  *****************
* User: Junanto      Date: 2/06/01    Time: 14:35
* Updated in $/xGate
* Changed output format of call trace
* 
* *****************  Version 43  *****************
* User: Bennylp      Date: 6/01/01    Time: 11:31a
* Updated in $/xGate
* Changed Session ID to generate unique values
* 
* *****************  Version 42  *****************
* User: Bennylp      Date: 5/29/01    Time: 4:41p
  * Updated in $/xGate
* Remove invalid handle warning in CallGetHandle()
  * 
  * *****************  Version 41  *****************
  * User: Junanto      Date: 25/05/01   Time: 19:25
  * Updated in $/xGate
  * Corrected bug in SignallingConnect
  * 
  * *****************  Version 40  *****************
  * User: Junanto      Date: 25/05/01   Time: 13:05
  * Updated in $/xGate
  * Corrected behaviour of SignallingConnect
  * 
  * *****************  Version 39  *****************
  * User: Sonny        Date: 5/24/01    Time: 11:47a
  * Updated in $/xGate
  * added some more info csa/cui in trace
  * 
  * *****************  Version 38  *****************
  * User: Junanto      Date: 23/05/01   Time: 20:52
  * Updated in $/xGate
  * Added some error messages
  * 
  * *****************  Version 37  *****************
  * User: Junanto      Date: 22/05/01   Time: 14:39
  * Updated in $/xGate
  * Changed some display
  * 
  * *****************  Version 36  *****************
  * User: Junanto      Date: 21/05/01   Time: 17:09
  * Updated in $/xGate
  * Added group name in the call-in criteria
  * 
  * *****************  Version 35  *****************
  * User: Junanto      Date: 19/05/01   Time: 15:30
  * Updated in $/xGate
  * Fixed bug in CallGetMediaName (it returned port name instead of group
      * name)
  * 
  * *****************  Version 34  *****************
  * User: Junanto      Date: 11/05/01   Time: 11:30
  * Updated in $/xGate
  * Switch voice and call automatically during incoming
  * call when a task needs automatic voice allocation
  * 
  * *****************  Version 33  *****************
  * User: Junanto      Date: 10/05/01   Time: 12:40
  * Updated in $/xGate
  * Replaced "\r\n" sequence to "\n". Telnet translates
  * an orphaned "\n" into "\r\n" during output
  * 
  * *****************  Version 32  *****************
  * User: Junanto      Date: 3/05/01    Time: 11:14
  * Updated in $/xGate
  * 
  * *****************  Version 31  *****************
  * User: Junanto      Date: 2/05/01    Time: 20:11
  * Updated in $/xGate
  * Added statistics counters
  * 
  * *****************  Version 30  *****************
  * User: Junanto      Date: 2/05/01    Time: 18:40
  * Updated in $/xGate
  * Added support for call statistics
  * 
  * *****************  Version 29  *****************
  * User: Junanto      Date: 2/05/01    Time: 15:17
  * Updated in $/xGate
  * Add new state CS_Releasing
  * 
  * *****************  Version 28  *****************
  * User: Sonny        Date: 5/01/01    Time: 10:41a
  * Updated in $/xGate
  * added warning message during incompatible
  * signalling connect
  * 
  * *****************  Version 27  *****************
  * User: Junanto      Date: 1/05/01    Time: 14:18
  * Updated in $/xGate
  * Simplified and reduced log output
  * 
  * *****************  Version 26  *****************
  * User: Junanto      Date: 30/04/01   Time: 21:21
  * Updated in $/xGate
  * Fixed bug in pipeline
  * 
  * *****************  Version 25  *****************
  * User: Junanto      Date: 30/04/01   Time: 15:31
  * Updated in $/xGate
  * Added CallSignallingConnect
  * 
  * *****************  Version 24  *****************
  * User: Junanto      Date: 30/04/01   Time: 11:30
  * Updated in $/xGate
  * add group and device name in RouteIncomingCall
  * 
  * *****************  Version 23  *****************
  * User: Junanto      Date: 30/04/01   Time: 10:57
  * Updated in $/xGate
  * Pipe.* renamed to PipeLine.* to avoid
  * conflict with similar file names in ACE
  * 
  * *****************  Version 22  *****************
  * User: Junanto      Date: 29/04/01   Time: 17:35
  * Updated in $/xGate
  * added support for pipeline signalling
  * 
  * *****************  Version 21  *****************
  * User: Junanto      Date: 26/04/01   Time: 12:05
  * Updated in $/xGate
  * Changed call trace output to take
  * benefit of new facilities in KLog
  * 
  * *****************  Version 20  *****************
  * User: Junanto      Date: 25/04/01   Time: 14:11
  * Updated in $/xGate
  * added call trace for transmission of SetupReq message
  * 
  * *****************  Version 19  *****************
  * User: Junanto      Date: 25/04/01   Time: 11:57
  * Updated in $/xGate
  * added call tracing facility
  * 
  * *****************  Version 18  *****************
  * User: Bennylp      Date: 4/23/01    Time: 10:42p
  * Updated in $/xGate
  * Fixed std namespace conflicts.
  * 
  * *****************  Version 17  *****************
  * User: Junanto      Date: 23/04/01   Time: 16:47
  * Updated in $/xGate
  * added error message in call parameters acessors if the specified handle
  * is invalid
  * 
  * *****************  Version 16  *****************
  * User: Junanto      Date: 22/04/01   Time: 21:26
  * Updated in $/xGate
  * Added APIs: CallExSetDisconnectMsg and Set/GetSessionId
  * 
  * *****************  Version 15  *****************
  * User: Junanto      Date: 21/04/01   Time: 23:05
  * Updated in $/xGate
  * 
  * *****************  Version 14  *****************
  * User: Bennylp      Date: 4/19/01    Time: 12:25p
  * Updated in $/ct15
  * Add routing incoming calls.
  * 
  * *****************  Version 13  *****************
  * User: Junanto      Date: 18/04/01   Time: 15:30
  * Updated in $/ct15
  * 
  * *****************  Version 12  *****************
  * User: Bennylp      Date: 4/18/01    Time: 3:29p
  * Updated in $/ct15
  * Resolve conflicting member names in Call.
  * 
  * *****************  Version 11  *****************
  * User: Junanto      Date: 18/04/01   Time: 11:49
  * Updated in $/ct15
  * 
  * *****************  Version 10  *****************
  * User: Junanto      Date: 15/04/01   Time: 20:48
  * Updated in $/ct15
  * 
  * *****************  Version 9  *****************
  * User: Junanto      Date: 14/04/01   Time: 20:00
  * Updated in $/ct15
  * Add more APIs
  * 
  * *****************  Version 8  *****************
  * User: Junanto      Date: 13/04/01   Time: 22:37
  * Updated in $/ct15
  * 
  * *****************  Version 7  *****************
  * User: Junanto      Date: 13/04/01   Time: 21:03
  * Updated in $/ct15
  * 1st version of integration with call manager
  * 
  * *****************  Version 6  *****************
  * User: Bennylp      Date: 4/12/01    Time: 8:59p
  * Updated in $/ct15
  * Integration and some bug fixes.
  * 
  * *****************  Version 5  *****************
  * User: Junanto      Date: 12/04/01   Time: 19:54
  * Updated in $/ct15
  * 
  * *****************  Version 4  *****************
  * User: Junanto      Date: 8/04/01    Time: 19:43
  * Updated in $/ct2
  * 
  * *****************  Version 3  *****************
  * User: Junanto      Date: 2/04/01    Time: 18:29
  * Updated in $/ct2
  * 
  * *****************  Version 2  *****************
  * User: Junanto      Date: 15/03/01   Time: 17:20
  * Updated in $/ct2
  * 
  * *****************  Version 1  *****************
  * User: Junanto      Date: 12/03/01   Time: 22:21
  * Created in $/ct2
  *----------------------------------------------------------------*/

  /*----------------------------------------------------------------* 
   * Description:
   *  CallManager is responsible for translating application requests into a 
   *  standard interface before passing the request to the Signalling Driver. 
   *  Call Manager is the only entity that has knowledge of application tasks. 
   *  The operation such as SuspendTask and ResumeTask is done at this level. 
   *  It is also at this level that an asynchronous (or blocking) operation 
   *  is implemented using state machine. Call Manager is also responsible 
   *  in interacting with the Resource Locator.
   *----------------------------------------------------------------*/

  //#include "stdafx.h"
#include "xgconfig.h"

#ifdef XGATE_HAS_H323
#include "h323sig.h"
#ifdef OK
#undef OK
#endif
#endif // XGATE_HAS_H323

#include "callmanager.h"
#include "MediaManager.h"
#include "kernel.h"
#include "SignallingDriver.h"

#ifdef XGATE_HAS_ISUP
#include "Isup.h"
#endif

#ifdef XGATE_HAS_ACULAB
#include "Aculab.h"
#endif

#ifdef XGATE_HAS_DIALOGIC
#include "Dialogic.h"
#endif

#ifdef XGATE_HAS_PROSODY
#include "prosody.h"
#endif

#ifdef XGATE_HAS_SIP // added for ippbx
#include "xGateSipSignalProcessor.h"
#endif

#ifdef XGATE_HAS_RTP_SIGNALLING
  //#include "rtpsig.h"
#endif

#ifdef XGATE_HAS_RTP_INTERFACE
#include "rtpmedia.h"
#endif

#ifdef XGATE_HAS_PIPE
#include "PipeLine.h"
#endif

#ifdef XGATE_HAS_SIP
#include "sip.h"
#endif

#ifdef XGATE_HAS_SOFIA
#include "sofia.h"
#endif

  // include file if we have Dialogic Digital Interface
#ifdef XGATE_HAS_DIALOGIC_DTI
#include "dialogic_dti.h"
#endif

#ifdef XGATE_HAS_BSSAP_CALL
#include "bssap_driver.h"
#include "bssap_call.h"
#endif

#include "task.h" 
#include "log.h"
#include "klog.h"
#include "term.h"

#define stricmp strcasecmp
#define strnicmp strncasecmp
#define strcmpi strcasecmp

  //#include "pcre-3.4/pcreposix.h"	// Regex
#include <regex.h>
#include "quality.h" //E-model, Calculate_R

  //-----------------------------------------------------------------
  static char __modname__[] =	"CallMgr";
#define THISMODULE		__modname__


  //-----------------------------------------------------------------
  const int ProvId_CallManager =	API_PROVIDER_ID_CALL;

  //-----------------------------------------------------------------
#define NEWLINE			"\n"
#define STAT_INDENT		"  "
#define STAT_INDENT2		"    "
#define MAXCTRACEFILTER		10

#ifdef XGATE_HAS_HMP
  int HmpConferenceRoom::_handle  = 0;
#endif

  map <string, int> host_info; // T 1710 - To maintain host user login status in conference

  extern map<CTask *, ip_info *> ip_list;
  pthread_mutex_t audit_mutex;

  //This function for linux porting need to move one common place
static unsigned long GetTickCount()
{
  struct timeval tv;
  if( gettimeofday(&tv, NULL) != 0 )
    return 0;

  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}


//-----------------------------------------------------------------
/** CallAPI: Methods, exposed to Kernel for call control, made on behalf of user 
  applications
  */
class CallAPI : public CAPIProvider
{
  CallManager* _manager;

  public:
  CallAPI(CallManager* manager,int providerId,const char* name);
  virtual ~CallAPI();

  protected:
  bool ValidCallParameter(CTask* task,CallParameter* cparam,const char* funcname);

  /** Call parameter related
  */
  STATUS API_CALL GetCallParam(CTask* task,CallParameter** cparam /*retval*/,int handle);
  STATUS API_CALL CreateDefaultCallParam(CTask* task,CallParameter** cparam /*retval*/,const char* portId);

  /** Set methods for CallParameter object
  */
  STATUS API_CALL SetSessionId(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetConnectionType(CTask* task,CallParameter* cparam,CallConnectionType value);
  STATUS API_CALL SetCompandingLaw(CTask* task,CallParameter* cparam,CallCompandingLaw value);
  STATUS API_CALL SetCalledPartyCategory(CTask* task,CallParameter* cparam,CallPartyCategory value);
  STATUS API_CALL SetCalledNumber(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetCalledNumberPlan(CTask* task,CallParameter* cparam,CallNumberPlan value);
  STATUS API_CALL SetCalledNumberType(CTask* task,CallParameter* cparam,CallNumberType value);
  STATUS API_CALL SetCalledSubaddress(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetCalledSignalAddress(CTask* task,CallParameter* cparam,const char* value);
#ifdef XGATE_HAS_ACME_SBC
  STATUS API_CALL SetRouteSignalAddress(CTask* task,CallParameter* cparam,const char* value);
  //TODO: Yoga, added for stateful sbc proxy to terminate deskphone calls
  STATUS API_CALL SetProxyUserName(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetCSFilename(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetRbtFile(CTask* task, CallParameter* cparam, const char* value);
  STATUS API_CALL SetCallOnHoldFilenameCallingParty(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetCallOnHoldFilenameCalledParty(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetCallForwardingFilename(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetVoicemailRecordingFilename(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetCalledDeviceType(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetCallRecording(CTask* task,CallParameter* cparam,int value);
  STATUS API_CALL SetIsForkingEnabled(CTask* task,CallParameter* cparam,int value);
  //STATUS API_CALL SetDomainName(CTask* task,CallParameter* cparam,const char* value);
#endif

  STATUS API_CALL SetMappedUserNumber(CTask* task,CallParameter* cparam, const char* mappedUserKey, const char* value);
  STATUS API_CALL SetMappedUserDeviceType(CTask* task,CallParameter* cparam, const char* mappedUserKey, const char* value);
  STATUS API_CALL SetMappedUserProxyName(CTask* task, CallParameter* cparam, const char* mappedUserKey, const char* value);
  STATUS API_CALL SetMappedUserSignalAddress(CTask* task,CallParameter* cparam, const char* mappedUserKey, const char* value);
  STATUS API_CALL SetMappedUserOutInterface(CTask* task,CallParameter* cparam, const char* mappedUserKey, const char* value);
  STATUS API_CALL SendAppNotification(CTask* task, const char* from, const char* to, const char* user_cli, const char* deviceType,const char* callType);

  STATUS API_CALL SetCalledComplete(CTask* task,CallParameter* cparam,int value);
  STATUS API_CALL SetUserInfo(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetCallingPartyCategory(CTask* task,CallParameter* cparam,CallPartyCategory value);
  STATUS API_CALL SetCallingNumber(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetCallingNumberPlan(CTask* task,CallParameter* cparam,CallNumberPlan value);
  STATUS API_CALL SetCallingNumberType(CTask* task,CallParameter* cparam,CallNumberType value);
  STATUS API_CALL SetCallingSubaddress(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetCallingSignalAddress(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetCallingPresentation(CTask* task,CallParameter* cparam,CallCallingPresentation value);
  STATUS API_CALL SetCallingScreening(CTask* task,CallParameter* cparam,CallCallingScreening value);
  STATUS API_CALL SetCallCredential(CTask* task, CallParameter* cparam, const char *username, const char*password);
  STATUS API_CALL SetOriginalCalledNumber(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetOriginalCalledNumberPlan(CTask* task,CallParameter* cparam,CallNumberPlan value);
  STATUS API_CALL SetOriginalCalledNumberType(CTask* task,CallParameter* cparam,CallNumberType value);
  STATUS API_CALL SetOriginalCalledNumberPresentation(CTask* task,CallParameter* cparam,CallCallingPresentation value);
  STATUS API_CALL SetRedirectingNumber(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetRedirectingNumberPlan(CTask* task,CallParameter* cparam,CallNumberPlan value);
  STATUS API_CALL SetRedirectingNumberType(CTask* task,CallParameter* cparam,CallNumberType value);
  STATUS API_CALL SetRedirectingNumberPresentation(CTask* task,CallParameter* cparam,CallCallingPresentation value);
  STATUS API_CALL SetLocationNumber(CTask* task,CallParameter* cparam,const char* value);
  STATUS API_CALL SetLocationNumberPlan(CTask* task,CallParameter* cparam,CallNumberPlan value);
  STATUS API_CALL SetLocationNumberType(CTask* task,CallParameter* cparam,CallNumberType value);
  STATUS API_CALL SetLocationNumberPresentation(CTask* task,CallParameter* cparam,CallCallingPresentation value);
  STATUS API_CALL SetRedirectionIndicator(CTask* task, CallParameter* cparam, int value);
  STATUS API_CALL SetRedirectionOrigReason(CTask* task, CallParameter* cparam, int value);
  STATUS API_CALL SetRedirectionCounter(CTask* task, CallParameter* cparam, int value);
  STATUS API_CALL SetRedirectionReason(CTask* task, CallParameter* cparam, int value);
  STATUS API_CALL SetPrivateInfo(CTask* task,CallParameter* cparam,const char* value);

  STATUS API_CALL SetCallingMobileIdentityType(CTask* task,CallParameter* cparam,CallMobileIdentityType value);
  STATUS API_CALL SetCallType(CTask* task,CallParameter* cparam,CallType value);
  STATUS API_CALL SetBssapDH(CTask* task,CallParameter* cparam,int value);
  STATUS API_CALL SetSuspendResumeNotification(CTask* task,CallParameter* cparam,bool value);
  STATUS API_CALL SetHuntGroupCaller(CTask* task,CallParameter* cparam,bool value);
  STATUS API_CALL SetSwitchBoardCall(CTask* task,CallParameter* cparam,bool value);
  STATUS API_CALL SetSwitchBoardCallState(CTask* task,CallParameter* cparam,int value,int mindigits, int timeout);
  STATUS API_CALL GetSwitchBoardCallState(CTask* task,CallParameter* cparam,int* value,char* dtmf_digits);
  STATUS API_CALL SwitchBoardSuspendTask(CTask* task,CallParameter* cparam, int value);
  STATUS API_CALL SetCallWaitingFlag(CTask* task,CallParameter* cparam,bool value);
  STATUS API_CALL CallSetSuspendResumeNotification(CTask* task,int handle,bool value);
  STATUS API_CALL CallSetECTNotification(CTask* task,int handle,bool value);
  STATUS API_CALL SetECTNotification(CTask* task,CallParameter* cparam,bool value);

  /** Get methods for CallParameter object
  */
  STATUS API_CALL GetSessionId(CTask* task,char* retval,CallParameter* cparam);
  STATUS API_CALL GetConnectionType(CTask* task,CallConnectionType* retval,CallParameter* cparam);
  STATUS API_CALL GetCompandingLaw(CTask* task,CallCompandingLaw* retval,CallParameter* cparam);
  STATUS API_CALL GetCalledPartyCategory(CTask* task,CallPartyCategory* retval,CallParameter* cparam);
  STATUS API_CALL GetCalledNumber(CTask* task,char* retval,CallParameter* cparam);
  STATUS API_CALL GetCalledNumberPlan(CTask* task,CallNumberPlan* retval,CallParameter* cparam);
  STATUS API_CALL GetCalledNumberType(CTask* task,CallNumberType* retval,CallParameter* cparam);
  STATUS API_CALL GetCalledSubaddress(CTask* task,char* retval,CallParameter* cparam);
  STATUS API_CALL GetCalledSignalAddress(CTask* task,char* retval,CallParameter* cparam);
#ifdef XGATE_HAS_ACME_SBC
  STATUS API_CALL GetRouteSignalAddress(CTask* task,char* retval,CallParameter* cparam);
  STATUS API_CALL GetProxyUsername(CTask* task,char* retval,CallParameter* cparam);
  STATUS API_CALL GetDomainName(CTask* task,char* retval,CallParameter* cparam);
#endif
  STATUS API_CALL GetCalledComplete(CTask* task,int* retval,CallParameter* cparam);
  STATUS API_CALL GetUserInfo(CTask* task,char* retval,CallParameter* cparam);
  STATUS API_CALL GetCallingPartyCategory(CTask* task,CallPartyCategory* retval,CallParameter* cparam);
  STATUS API_CALL GetCallingNumber(CTask* task,char* retval,CallParameter* cparam);
  STATUS API_CALL GetCallingNumberPlan(CTask* task,CallNumberPlan* retval,CallParameter* cparam);
  STATUS API_CALL GetCallingNumberType(CTask* task,CallNumberType* retval,CallParameter* cparam);
  STATUS API_CALL GetCallingSubaddress(CTask* task,char* retval,CallParameter* cparam);
  STATUS API_CALL GetCallingSignalAddress(CTask* task,char* retval,CallParameter* cparam);
  STATUS API_CALL GetCallingPresentation(CTask* task,CallCallingPresentation* retval,CallParameter* cparam);
  STATUS API_CALL GetCallingScreening(CTask* task,CallCallingScreening* retval,CallParameter* cparam);
  STATUS API_CALL GetOriginalCalledNumber(CTask* task,char* retval,CallParameter* cparam);
  STATUS API_CALL GetOriginalCalledNumberPlan(CTask* task,CallNumberPlan* retval,CallParameter* cparam);
  STATUS API_CALL GetOriginalCalledNumberType(CTask* task,CallNumberType* retval,CallParameter* cparam);
  STATUS API_CALL GetOriginalCalledNumberPresentation(CTask* task,CallCallingPresentation* retval,CallParameter* cparam);
  STATUS API_CALL GetRedirectingNumber(CTask* task,char* retval,CallParameter* cparam);
#ifdef CALL_TRANSFER
  // STATUS API_CALL GetRedirectionAddress(CTask* task,char* retval,CallParameter* cparam);
#endif
  STATUS API_CALL GetRedirectingNumberPlan(CTask* task,CallNumberPlan* retval,CallParameter* cparam);
  STATUS API_CALL GetRedirectingNumberType(CTask* task,CallNumberType* retval,CallParameter* cparam);
  STATUS API_CALL GetRedirectingNumberPresentation(CTask* task,CallCallingPresentation* retval,CallParameter* cparam);
  STATUS API_CALL GetLocationNumber(CTask* task,char* retval,CallParameter* cparam);
  STATUS API_CALL GetLocationNumberPlan(CTask* task,CallNumberPlan* retval,CallParameter* cparam);
  STATUS API_CALL GetLocationNumberType(CTask* task,CallNumberType* retval,CallParameter* cparam);
  STATUS API_CALL GetLocationNumberPresentation(CTask* task,CallCallingPresentation* retval,CallParameter* cparam);
  STATUS API_CALL GetRedirectionIndicator(CTask* task, int* retval, CallParameter* cparam);
  STATUS API_CALL GetRedirectionOrigReason(CTask* task, int* retval, CallParameter* cparam);
  STATUS API_CALL GetRedirectionCounter(CTask* task, int* retval, CallParameter* cparam);
  STATUS API_CALL GetRedirectionReason(CTask* task, int* retval, CallParameter* cparam);
  STATUS API_CALL GetPrivateInfo(CTask* task,char* value,CallParameter* cparam);

  STATUS API_CALL GetCallingMobileIdentityType(CTask* task,CallMobileIdentityType* retval,CallParameter* cparam);
  STATUS API_CALL GetCallType(CTask* task,CallType* retval,CallParameter* cparam);
  STATUS API_CALL GetBssapDH(CTask* task,int* retval,CallParameter* cparam);
  STATUS API_CALL GetSuspendResumeNotification(CTask* task,bool* retval,CallParameter* cparam);
  STATUS API_CALL GetCallWaitingFlag(CTask* task,bool* retval,CallParameter* cparam);

  /** Get methods for call object
  */
  STATUS API_CALL CallGetSessionId(CTask* task,char* retval,int handle);
  STATUS API_CALL CallGetConnectionType(CTask* task,CallConnectionType* retval,int handle);
  STATUS API_CALL CallGetCompandingLaw(CTask* task,CallCompandingLaw* retval,int handle);
  STATUS API_CALL CallGetCalledPartyCategory(CTask* task,CallPartyCategory* retval,int handle);
  STATUS API_CALL CallGetCalledNumber(CTask* task,char* retval,int handle);
  STATUS API_CALL CallGetCalledNumberPlan(CTask* task,CallNumberPlan* retval,int handle);
  STATUS API_CALL CallGetCalledNumberType(CTask* task,CallNumberType* retval,int handle);
  STATUS API_CALL CallGetCalledSubaddress(CTask* task,char* retval,int handle);
  STATUS API_CALL CallGetCalledSignalAddress(CTask* task,char* retval,int handle);
#ifdef XGATE_HAS_ACME_SBC
  STATUS API_CALL CallGetRouteSignalAddress(CTask* task,char* retval, CallParameter* cparam);
  //TODO: Yoga, added for stateful sbc proxy to terminate deskphone calls
  //////STATUS API_CALL CallGetProxyUserName(CTask* task,char* retval,int handle);
  STATUS API_CALL CallGetProxyUserName(CTask* task,char* retval,CallParameter* cparam);
  //STATUS API_CALL CallGetDomainName(CTask* task,char* retval,int handle);
#endif
  STATUS API_CALL CallGetCalledComplete(CTask* task,int* retval,int handle);
  STATUS API_CALL CallGetUserInfo(CTask* task,char* retval,int handle);
  STATUS API_CALL CallGetCallingPartyCategory(CTask* task,CallPartyCategory* retval,int handle);
  STATUS API_CALL CallGetCallingNumber(CTask* task,char* retval,int handle);
  STATUS API_CALL CallGetCallingNumberPlan(CTask* task,CallNumberPlan* retval,int handle);
  STATUS API_CALL CallGetCallingNumberType(CTask* task,CallNumberType* retval,int handle);
  STATUS API_CALL CallGetCallingSubaddress(CTask* task,char* retval,int handle);
  STATUS API_CALL CallGetCallingSignalAddress(CTask* task,char* retval,int handle);
  STATUS API_CALL CallGetCallingPresentation(CTask* task,CallCallingPresentation* retval,int handle);
  STATUS API_CALL CallGetCallingScreening(CTask* task,CallCallingScreening* retval,int handle);
  STATUS API_CALL CallGetPrivateInfo(CTask* task,char* value,int handle);
  STATUS API_CALL CallGetQuality(CTask* task, float* value, int handle, int stream, const char* name);
  STATUS API_CALL CallGetAuthorization(CTask* task, int* auth_handle, int call_handle);
  STATUS API_CALL CallGetOriginalCalledNumber(CTask* task,char* retval,int handle);
  STATUS API_CALL CallGetOriginalCalledNumberPlan(CTask* task,CallNumberPlan* retval,int handle);
  STATUS API_CALL CallGetOriginalCalledNumberType(CTask* task,CallNumberType* retval,int handle);
  STATUS API_CALL CallGetOriginalCalledNumberPresentation(CTask* task,CallCallingPresentation* retval,int handle);
  STATUS API_CALL CallGetRedirectingNumber(CTask* task,char* retval,int handle);
  STATUS API_CALL CallGetRedirectingNumberPlan(CTask* task,CallNumberPlan* retval,int handle);
  STATUS API_CALL CallGetRedirectingNumberType(CTask* task,CallNumberType* retval,int handle);
  STATUS API_CALL CallGetRedirectingNumberPresentation(CTask* task,CallCallingPresentation* retval,int handle);
  STATUS API_CALL CallGetLocationNumber(CTask* task,char* retval,int handle);
  STATUS API_CALL CallGetLocationNumberPlan(CTask* task,CallNumberPlan* retval,int handle);
  STATUS API_CALL CallGetLocationNumberType(CTask* task,CallNumberType* retval,int handle);
  STATUS API_CALL CallGetLocationNumberPresentation(CTask* task,CallCallingPresentation* retval,int handle);
  STATUS API_CALL CallGetRedirectionIndicator(CTask* task, int* retval, int handle);
  STATUS API_CALL CallGetRedirectionOrigReason(CTask* task, int* retval, int handle);
  STATUS API_CALL CallGetRedirectionCounter(CTask* task, int* retval, int handle);
  STATUS API_CALL CallGetRedirectionReason(CTask* task, int* retval, int handle);

  STATUS API_CALL CallGetCallingMobileIdentityType(CTask* task,CallMobileIdentityType* retval,int handle);
  STATUS API_CALL CallGetCallType(CTask* task,CallType* retval,int handle);
  STATUS API_CALL CallGetSuspendResumeNotification(CTask* task,bool* retval,int handle);
  STATUS API_CALL CallGetCallWaitingFlag(CTask* task,bool* retval,int handle);
  STATUS API_CALL CallGetBssapDH(CTask* task,int* retval,int handle);

  STATUS API_CALL SetCharging(CTask* task,int handle,int charging);
  STATUS API_CALL ExSetDisconnectMsg(CTask* task,int handle,const char* msg);

  /** Call control related
  */
  STATUS API_CALL Setup(CTask* task,int* retcode,const char* itfName,CallParameter* cparam,int* handle);
  STATUS API_CALL Setup_callque(CTask* task,int* retcode,const char* itfName,CallParameter* cparam,int* handle);
  STATUS API_CALL SendOverlap(CTask* task,int* retcode,int handle,const char* additionalDigits,int complete);
  STATUS API_CALL GetMoreDigitsEx(CTask* task,char* additionalDigits,int handle,int mindigits,int timeout,int* complete);
  STATUS API_CALL GetMoreDigits(CTask* task,char* additionalDigits,int handle,int timeout,int* complete);
  STATUS API_CALL Proceeding(CTask* task,int* retcode,int handle,CallProgress progress);
  STATUS API_CALL Progress(CTask* task,int* retcode,int handle,CallProgress progress);
  STATUS API_CALL Alerting(CTask* task,int* retcode,int handle);
  STATUS API_CALL Answer(CTask* task,int* retcode,int handle);
  STATUS API_CALL SendInfo(CTask* task,int* retcode,int handle);
  STATUS API_CALL Suspend(CTask* task,int* retcode,int handle);
  STATUS API_CALL Resume(CTask* task,int* retcode,int handle);
  STATUS API_CALL Disconnect(CTask* task,int* retcode,int handle,CallCause cause);
  STATUS API_CALL RegIpInfo(CTask* task,int* retcode,char *ip_usrA,char *ip_usrB, int pid);
  STATUS API_CALL RemoveFile(CTask* task, char *filename);
  STATUS API_CALL GetCause(CTask* task,CallCause* cause,int handle);
  STATUS API_CALL SignallingConnect(CTask* task,int* retcode,int handle1,int handle2);
  STATUS API_CALL SignallingDisconnect(CTask* task,int* retcode,int handle1,int handle2);
  STATUS API_CALL NotifyECT(CTask* task,int* retcode,int handle, tect_indicator *indicator);

  /** Miscellanous
  */
  STATUS API_CALL GetDirection(CTask* task,CallDirection* direction,int handle);
  STATUS API_CALL GetState(CTask* task,CallState* state,int handle);
  bool IsStateReadyForSignallingConnect(CallState state);

  /** Enumeration
  */
  STATUS API_CALL GetFirstCallHandle(CTask* task,int* handle);
  STATUS API_CALL GetNextCallHandle(CTask* task,int* handle);

  /** Media control related
  */
  STATUS API_CALL GetMedia(CTask* task,Media** media,int handle);
  STATUS API_CALL GetMediaName(CTask* task,int* retcode,int handle,char* group,char* name);
  STATUS API_CALL GetInterfaceName(CTask* task,char* retval,int handle);
  STATUS API_CALL GetGroupName(CTask* task,char* retval,int handle);
  STATUS API_CALL CallReleaseMedia(CTask* task, int* retval, int handle);

  /** CallSetup variant
  */
  STATUS API_CALL SetupInterface(CTask* task,int* retcode,const char* itfName,CallParameter* cparam,int* handle);
  STATUS API_CALL SetupGroup(CTask* task,int* retcode,const char* itfName,const char* groupName,CallParameter* cparam,int* handle);

#ifdef XGATE_HAS_HMP
  Call * GetHmpCallObject(CTask* task, int handle);

  //    STATUS API_CALL HmpGetDtmfDigits(CTask* task, int handle, int number_of_digits, int wait_time, char* dtmf_digits);
  STATUS API_CALL HmpGetDtmfDigits(CTask* task, int handle, int* retval , char* dtmf_digits, int mindigits);
  // STATUS API_CALL HmpPlayFile(CTask *task, int handle, char *filename);

  STATUS API_CALL HmpPlayFile(CTask *task, int handle, char *filename, bool autorepeat);
  ////STATUS API_CALL HmpSwbPlayFile(CTask *task, int handle, char *filename, bool autorepeat, int callstate);
  STATUS API_CALL HmpSwbPlayFile(CTask *task, CallParameter* cparam, char *filename, bool autorepeat, int callstate);
  STATUS API_CALL HmpAttachTask(CTask *task, int handle);
  STATUS API_CALL SetConferenceFlag(CTask *task, int handle);

  STATUS API_CALL HmpStopPlayFile(CTask *task, int handle, int  waitTime);
  //STATUS API_CALL HmpPlayFileBkgnd(CTask *task, int handle, char *filename);
  STATUS API_CALL HmpPlayFileBkgnd(CTask *task, int handle, char *filename, bool autorepeat);
  STATUS API_CALL HmpStopPlayFileBkgnd(CTask *task, int handle, int  waitTime);


  /*****Recording apis **************/

  STATUS API_CALL HmpRecordFile(CTask *task, int handle1, int handle2, char *filename);
  STATUS API_CALL HmpEndRecordFile(CTask *task, int handle);

  /****** Conference APIs ***********/

  STATUS API_CALL HmpConfCreateRoom(CTask* task, int* room_handle, char* room_name, int room_size, int handle);
  STATUS API_CALL HmpConfDeleteRoom(CTask* task, char* room_name, int handle);
  STATUS API_CALL HmpConfRemoveParty(CTask* task, char* room_name, int handle);
  //STATUS API_CALL HmpConfOpenMedia(CTask* task, Rtp_Media  **conf_media, const char* room_name);
  STATUS API_CALL HmpConfRecordFile(CTask* task, char* room_name, int handle, char* file_path);
  STATUS API_CALL HmpConfEndRecordFile(CTask* task, char* room_name, int handle);
  STATUS API_CALL HmpConfFindRoom(CTask* task, int *retval, char* room_name);
  STATUS API_CALL CreateFilePath(CTask *task, int *retval, char *filepath);
  STATUS API_CALL GetHostInfo(CTask *task, int *retval, char *conf_id);
  STATUS API_CALL SetHostInfo(CTask *task, char *conf_id, int status);

  // digit collection APIs

  STATUS API_CALL HmpGetDigits(CTask *task,int max_length,char *buffer);
  STATUS API_CALL HmpClearDigits(CTask *task);
  STATUS API_CALL HmpGetDigitTimeout(CTask *task,int *max_timeout);
  STATUS API_CALL HmpSetDigitTimeout(CTask *task, int handle, int max_timeout);
  STATUS API_CALL HmpGetInterDigitTimeout(CTask *task,int *max_timeout);
  STATUS API_CALL HmpSetInterDigitTimeout(CTask *task,int max_timeout);
  STATUS API_CALL HmpSetTermDigits(CTask *task,char *term_digits);
  STATUS API_CALL HmpEnableInterrupt(CTask *task);
  STATUS API_CALL HmpAlloc(CTask *task);
  STATUS API_CALL HmpFree(CTask *task);




#endif 

};


//-----------------------------------------------------------------
enum {
  Cmd_CTrace_Filter,
  Cmd_Call_Show,
};

//-----------------------------------------------------------------
extern const char * StrCallState(CallState state);
extern const char * print_fraction(double num, char *s);

//-----------------------------------------------------------------
struct CallDescription {
  CallParameter cparam;
  String itf;
  String chan;
  String grp;
  String callName;
  long holding;
  long connect;
  CallDirection dir;
  CallState state;
  String taskId;
  String appl;
  String media;

  float rx_packets;
  float rx_itot;
  float rx_rtt;
  float rx_jitter;
  float rx_loss;
  float rx_fracloss;

  float tx_packets;
  float tx_itot;
  float tx_rtt;
  float tx_jitter;
  float tx_loss;
  float tx_fracloss;

  CallDescription()
    : holding(0), connect(0), dir(CD_Null), state(CS_Null)
  {
    rx_packets = rx_itot = rx_rtt = rx_jitter = rx_loss = rx_fracloss = tx_packets = 
      tx_itot = tx_rtt = tx_jitter = tx_loss = tx_fracloss = 0;
  }

  CallDescription(const CallDescription & rhs)
  {
    assign(rhs);
  }
  CallDescription & operator=(const CallDescription & rhs)
  {
    assign(rhs);
    return *this;
  }

  void assign(const CallDescription & rhs)
  {
    cparam = rhs.cparam;
    itf = rhs.itf;
    chan = rhs.chan;
    grp = rhs.grp;
    callName = rhs.callName;
    holding = rhs.holding;
    connect = rhs.connect;
    dir = rhs.dir;
    state = rhs.state;
    taskId = rhs.taskId;
    appl = rhs.appl;
    media = rhs.media;
    rx_packets = rhs.rx_packets;
    rx_itot = rhs.rx_itot;
    rx_rtt = rhs.rx_rtt;
    rx_jitter = rhs.rx_jitter;
    rx_loss = rhs.rx_loss;
    rx_fracloss = rhs.rx_fracloss;
    tx_packets = rhs.tx_packets;
    tx_itot = rhs.tx_itot;
    tx_rtt = rhs.tx_rtt;
    tx_jitter = rhs.tx_jitter;
    tx_loss = rhs.tx_loss;
    tx_fracloss = rhs.tx_fracloss;
  }
};

/*----------------------------------------------------------------* 
 * Task Local Storage
 *----------------------------------------------------------------*/
struct TaskCallBlock
{
  typedef Vector<Call*> ArrayCall;

  ArrayCall _calls;
  CallParameter _cparam;
  Call* _suspendedOnCall;

  TaskCallBlock();
  ~TaskCallBlock();

  Call* CallFromHandle(int handle);
  unsigned InsertCall(Call* call);
  void RemoveCall(int handle);
  void RemoveCall(Call* call);
};

unsigned CallTraceCondition::nextid = 0;

/*----------------------------------------------------------------* 
 * Static variables
 *----------------------------------------------------------------*/

CallManager* CallManager::theCallManager;


/*----------------------------------------------------------------* 
 * Implementation of TaskCallBlock
 *----------------------------------------------------------------*/

/** TaskCallBlock's constructor
*/
TaskCallBlock::TaskCallBlock()
{
  _suspendedOnCall = NULL;
}

/** TaskCallBlock's destructor
*/
TaskCallBlock::~TaskCallBlock()
{
}

/** Return a pointer to Call for the specified handle
*/
Call* TaskCallBlock::CallFromHandle(int handle)
{
  if (handle < _calls.size())
    return _calls[handle];
  return NULL;
}

/** Add a new call to the list and return a new handle
*/
unsigned TaskCallBlock::InsertCall(Call* call)
{
  for (unsigned i=0;i<_calls.size();i++) {
    if (_calls[i] == NULL) {
      _calls[i] = call;
      return i;
    }
  }
  _calls.push_back(call);
  return i;
}

/** Remove the call specified by handle
*/
void TaskCallBlock::RemoveCall(int handle)
{
  if (handle < _calls.size()) {
    _calls[handle] = NULL;
    return;
  }
  assert(false);
}

/** Remove the specified call
*/
void TaskCallBlock::RemoveCall(Call* call)
{
  for (int i=0;i<_calls.size();i++) {
    if (_calls[i] == call) {
      _calls[i] = NULL;
      return;
    }
  }
  assert(false);
}

/*----------------------------------------------------------------
 * Implementation of CallExpression and CallTraceCondition
 *----------------------------------------------------------------*/
CallExpression::CallExpression(const char* expr) 
{
  _expr = expr;
  _tv["ddi"] = "";
  _tv["cli"] = "";
  _tv["group"] = "";
  _tv["interface"] = "";
  _tv["pinfo"] = "";
  _tv["uinfo"] = "";
  _tv["session"] = "";
  _tv["direction"] = "";
}

bool CallExpression::Parse() 
{
  _cursor = _expr.c_str();
  while (*_cursor) {
    ConsumeWhiteSpace();
    if (*_cursor == 0) break;

    String tag = GetToken();

    MapTagValue::iterator it=_tv.begin();
    MapTagValue::iterator end=_tv.end();
    unsigned taglen = tag.length();
    while (it != end) {
      if (strnicmp(tag.c_str(),(*it).first.c_str(),taglen) == 0) {
        tag = (*it).first;
        break;
      }
      it++;
    }

    // unsupported tag
    if (it == end)
      return false;

    ConsumeWhiteSpace();
    if (*_cursor != '=')
      return false;

    _cursor++;

    ConsumeWhiteSpace();
    String value = GetToken();
    if (value.length() == 0)
      return false;

    _tv[tag] = value;
    ConsumeWhiteSpace();
    if (*_cursor == 0)
      return true;

    if (*_cursor != ',')
      return false;

    _cursor++;
  }
  return true;
}

char* CallExpression::GetToken() 
{
  int i=0;
  bool finished = false;
  while (!finished) {
    char c = *_cursor;
    switch (c) {
      case ' ':
      case '=':
      case ',':
      case '\0':
        finished = true;
        break;
      default:
        _token[i++] = c;
        _cursor++;
        break;
    }
  }
  _token[i] = '\0';
  return _token;
}

CallTraceCondition::CallTraceCondition(const char* expr) : 
  _expr(expr),
  t1("T1="),t2("&T2="),t3("&T3="),t4("&T4="),
  t5("&T5="),t6("&T6="),t7("&T7="),t8("&T8="),
  sep("&")
{
  _id = nextid++;
  memset(&_regex, 0, sizeof(_regex));
}

CallTraceCondition::~CallTraceCondition()
{
  regfree(&_regex);
}

bool CallTraceCondition::Parse() 
{
  if (!_expr.Parse()) return false;

  String wildcard = ".*";
  String formattedExpression;
  formattedExpression = t1 + 
    (strlen(_expr.GetValue("ddi")) ? _expr.GetValue("ddi") : wildcard);

  formattedExpression += t2 +
    (strlen(_expr.GetValue("cli")) ? _expr.GetValue("cli") : wildcard);

  formattedExpression += t3 + 
    (strlen(_expr.GetValue("group")) ? _expr.GetValue("group") : wildcard);

  formattedExpression += t4 + 
    (strlen(_expr.GetValue("interface")) ? _expr.GetValue("interface") : wildcard);

  formattedExpression += t5 +
    (strlen(_expr.GetValue("direction")) ? _expr.GetValue("direction") : wildcard);

  formattedExpression += t6 +
    (strlen(_expr.GetValue("uinfo")) ? _expr.GetValue("uinfo") : wildcard);

  formattedExpression += t7 +
    (strlen(_expr.GetValue("pinfo")) ? _expr.GetValue("pinfo") : wildcard);

  formattedExpression += t8 +
    (strlen(_expr.GetValue("session")) ? _expr.GetValue("session") : wildcard);

  if (regcomp(&_regex, formattedExpression.c_str(), REG_ICASE|REG_EXTENDED))
    return false;

  return true;
}


bool CallTraceCondition::Match(
    const String& ddi, const String& cli, const String& group, 
    const String& itf, const String& pinfo, const String& uinfo,
    const String& session, const String& direction) 
{
  String condition = 
    t1 + ddi +
    t2 + cli +
    t3 + group +
    t4 + itf +
    t5 + direction +
    t6 + uinfo +
    t7 + pinfo +
    t8 + session;

  return (regexec(&_regex, condition.c_str(), 0, NULL, 0) == 0);
}



/*----------------------------------------------------------------* 
 * Implementation of Call Manager
 *----------------------------------------------------------------*/

CallManager::CallManager() : 
  ServiceProvider("CallManager",true,true,Lowest_Priority)
{
  _lastCallCreated = NULL;
  _currentInCall = NULL;
  _lastTaskAttached = NULL;
  _api = NULL;
  _traceCall = NULL;

  _statCurNbCall = 0;
  _statCurNbOutCall = 0;
  _statCurNbInCall = 0;
  _statMaxNbCall = 0;
  _statMaxNbOutCall = 0;
  _statMaxNbInCall = 0;
  _statCurCallPSec = 0;
  _statCurOutCallPSec = 0;
  _statCurInCallPSec = 0;
  _statMaxCallPSec = 0;
  _statMaxOutCallPSec = 0;
  _statMaxInCallPSec = 0;

  _canCreateCall = false;

  _statLastUpdate = time(NULL);
}


CallManager::~CallManager()
{
}


/** Create: Create this singleton instance if necessary and return a pointer to it
*/
CallManager* CallManager::Create()
{
  if (!theCallManager) {
    theCallManager = new CallManager;


#ifdef XGATE_HAS_ACULAB
    AculabSignalling::Create();
#endif

#ifdef XGATE_HAS_PIPE
    PipeSignalling::Create();
#endif

#ifdef XGATE_HAS_PROSODY
    //VoiceManager::Create();	
#endif

#ifdef XGATE_HAS_RTP_SIGNALLING
    //RTP_Signalling::Create();
#endif

#ifdef XGATE_HAS_SIP
    ////XgSipSignalling::Create(); // commented for ippbx
#endif

#ifdef XGATE_HAS_SOFIA
    //    SofiaSIPSignalling::Create();  
#endif

    // create all network interfaces
#ifdef XGATE_HAS_ACULAB
    AculabFactory::Create();	
#endif

#ifdef XGATE_HAS_RTP_INTERFACE
    RtpInterfaceManager::Create();
#endif
  } else {
    assert(false);
  }

  return theCallManager;
}


/** Instance: Return a pointer to this singleton instance, if necessary
  create one instance first before returning a pointer to it
  */
CallManager* CallManager::Instance()
{
  return theCallManager;
}


/** Convert a handle as used by Kernel (and user applications)
  to a pointer to call object
  */
Call* CallManager::CallFromHandle(CTask* task,int handle,bool warning,const char* funcname)
{
  TaskCallBlock* tcb = GetTaskCallBlock(task);
  if ((handle >= 0) && (handle < tcb->_calls.size())) {
    Call* call = tcb->_calls[handle];
    if (call && (call->_task == task)) return call;
  }

  // Invalid call handle, display warning and return NULL
  if (warning) CLog::Debug(THISMODULE, task, "Invalid call handle %d specified in %s",handle,funcname);
  return NULL;
}

/** Called by Signalling Driver each time a new call is created:
  - during SetupReq
  - on receipt of SETUP from the network
  Memorize the call instance so that it can be referenced later and assigned
  to the appropriate task by SetupReq or SetupInd
  */
void CallManager::CallCreated(Call* call)
{
  _lastCallCreated = call;
}

/** Iterate all calls owned by the specified task and disconmect the call
  if it is doing an asynchronous job, e.g. state is:
  - Initiating (SETUP sent/received)
  ? Connecting (CONNECT sent waiting for CONNECT ACK) --> let it mature
  ? Clearing --> let it finish
  */
bool CallManager::CancelJob(CTask* task)
{
  TaskCallBlock* tcb = GetTaskCallBlock(task);
  Call* call = tcb->_suspendedOnCall;
  assert(call);
  if (call) {
    if (call->_waitForMoreDigits) {
      *call->_appComplete = call->_callParam.complete;
      safe_strcpy(call->_appDigits, call->_overlapRcvd.c_str(), MAXAPPSTRLEN);
      RemoveCallFromWaitDigitsList(call);
      call->ResumeTask();
    } else {
      call->DisconnectReq(CC_NormalUnspecified,CP_NoIndication);
    }
  }
  return true;
}

/** Suspend the task owner of a particular call
*/
void CallManager::SuspendOwnerTask(Call* call)
{
  CTask* task = call->GetTask();
  if (task) {
    TaskCallBlock* tcb = GetTaskCallBlock(task);
    task->Sleep(this);
    tcb->_suspendedOnCall = call;
  }
}

/** Resume the task owner of a particular call
*/
void CallManager::ResumeOwnerTask(Call* call)
{
  CTask* task = call->GetTask();
  if (task) {
    TaskCallBlock* tcb = GetTaskCallBlock(task);
    task->Wakeup();
    tcb->_suspendedOnCall = NULL;
  }
}

/** Return the TaskControlBlock associated with the specified task
*/
TaskCallBlock* CallManager::GetTaskCallBlock(CTask* task)
{
  TaskCallBlock* tcb = (TaskCallBlock*)task->GetTLSValue(_tlsId);
  assert(tcb);
  return tcb;
}

/** Initialize task's TLS
*/
bool CallManager::AttachTask(CTask* task)
{
  TaskCallBlock* tcb = new TaskCallBlock;
  task->SetTLSValue(_tlsId,(int)tcb);

  // memorize the task instance as it will be used in
  // CallManager::OnIncomingCall
  _lastTaskAttached = task;

  return true;
}

/** Insert a call into a task call block and return the handle
*/
unsigned CallManager::AttachCallToTask(CTask* task,Call* call)
{
  TaskCallBlock* tcb = GetTaskCallBlock(task);
  return tcb->InsertCall(call);
}

/** Remove a call from a task call block
*/
void CallManager::DetachCallFromTask(CTask* task,Call* call)
{
  TaskCallBlock* tcb = GetTaskCallBlock(task);
  tcb->RemoveCall(call);
}

// karthi - testing
void CallManager::DetachCallFromTask_HG(CTask* task,Call* call, int handle)
{
  printf("Inside DetachCallFromTask_HG.. Skipping RemoveCall for handle %d \n", handle);
  /**
    TaskCallBlock* tcb = GetTaskCallBlock(task);
    tcb->RemoveCall(handle);
   **/
}

/** Disconmect each call owned by the specified task that is not is Null state,
  detach the call from task, let each call to go to Null state before destroying
  the call on receipt of RelComInd.
  If call was already in Null state, detach it from task and destroy the call.
  */
void CallManager::DetachTask(CTask* task)
{
  TaskCallBlock* tcb = GetTaskCallBlock(task);

  //TBD: Clean up all attached call
  for (int i=0;i<tcb->_calls.size();i++) {
    Call* call = tcb->_calls[i];
    if (call) {
      if (call->_waitForMoreDigits && call->GetTask() && call->GetTask()->IsValid()) 
        RemoveCallFromWaitDigitsList(call);
      call->DisconnectReq(CC_NormalCallClearing,CP_NoIndication);
    }
  }

  // delete TaskCallBlock
  task->SetTLSValue(_tlsId,0);

  if(tcb)
  {
    delete tcb;
    tcb = NULL;
  }
}

void CallManager::RemoveCallFromWaitDigitsList(Call* call)
{
  if (call->_waitForMoreDigits) {
    call->_timeLastDigit = 0;
    call->_delayInterDigit = 0;
    call->_overlapRcvd = "";
    call->_waitForMoreDigits = false;
  }
  _listCallWaitDigits.remove(call);
}

#ifdef XGATE_HAS_HMP
bool CallManager::AddConfRoomInMap(char* room_name, HmpConferenceRoom *conf_room)
{
  if(conf_room) {
    _hmp_conf_rooms_map[room_name] = conf_room;
    return true;
  }
  return false;
}
bool CallManager::RemoveConfRoomFromMap(char* room_name)
{
  if(_hmp_conf_rooms_map.erase(room_name))
    return true;
  return false;
}
HmpConferenceRoom* CallManager::GetConfRoomFromMap(char* room_name)
{
  std::map<String, HmpConferenceRoom*>::iterator it;
  it = _hmp_conf_rooms_map.find(room_name);
  if(it !=_hmp_conf_rooms_map.end()) {
    return it->second;
  }
  return NULL;
}
#endif
#if 0
xGateCallObj* ReadFromProcessQueue_lower(){
  CLog::Info(THISMODULE,"CallManager::ReadFromProcessQueue_lower() Called");
  xGateCallObj *obj = xGateDeQcallobj();
  if(obj){
    obj->CallinfoEventExecute(obj->sipeventtype);

    if (obj->sipeventtype == XGATESIPEVENT::EN_SIPINFO_REQ)
        return (obj);

    if (obj->sipeventtype == XGATESIPEVENT::EN_SWITCHBOARD_REQ)
        return (obj);

/****
    if ((obj->sipeventtype == XGATESIPEVENT::EN_MEDIA_PLAYFILE_RES) || (EN_MEDIA_PLAYFILE_REQ))
        return (obj);

    if (obj->sipeventtype == XGATESIPEVENT::EN_MEDIA_STOP_PLAY_FILE)
        return (obj);
*****/

    delete obj;
    return NULL;
  }
}
#endif
void CallManager::ReadFromProcessQueue(){
  //CLog::Info(THISMODULE,"CallManager::ReadFormProcessQueue Called");
//	ReadFromProcessQueue_lower();
#if 0
  xGateCallObj *obj = xGateDeQcallobj();
  if(obj){
    obj->CallinfoEventExecute(obj->sipeventtype);
    delete obj;
  }
#endif
}

/** HandleEvents: delete calls in _listCallClearing
*/
void CallManager::HandleEvents()
{
  ////CLog::Detail(THISMODULE,  "multithreaded-ippbx : CallManager::HandleEvents()");
  // Below segment is commet since it is not required it will be delete on the fly not on loop --yadav
/*  while (_listCallToBeReleased.size()) {
    Call* call = _listCallToBeReleased.front();
    call->RelComInd(call->GetCause());
    _listCallToBeReleased.remove(call);
  }

  while (_listCallClearing.size()) {
    xGateCallInfo* call = _listCallClearing.front();
    //delete call;
    _listCallClearing.remove(call);
    if( xGateRemoveCallObj(call->callid))
      CLog::Error (THISMODULE,"Call Object Remove From List %s",call->callid.c_str());

    //delete call;
    //    call = NULL;

  }*/

  //////////////////added for ippbx
  // New Call Add
  ReadFromProcessQueue();
  ////sleep(1);
  ////////////////////added for ippbx

#if 0
  ListCall::iterator it = _listCallWaitDigits.begin();
  ListCall::iterator end = _listCallWaitDigits.end();
  unsigned now = GetTickCount();
  while (it != end) {
    Call *call = *it;

    // time to wake-up a task suspended on GetMoreDigits
    if (call->_timeLastDigit+call->_delayInterDigit < now) {
      call->_timeLastDigit = 0;
      call->_delayInterDigit = 0;
      *call->_appComplete = call->_callParam.complete;
      safe_strcpy(call->_appDigits, call->_overlapRcvd.c_str(), MAXAPPSTRLEN);
      call->_overlapRcvd = "";
      call->_waitForMoreDigits = false;

      call->ResumeTask();

      it = _listCallWaitDigits.erase(it);
    } else {
      it++;
    }

  }
#endif 
}


/** InsertActiveCall: inserts a Call object to the list of active calls
*/
void CallManager::InsertActiveCall(Call* call)
{
  // We can not register the call if:
  //	- CallManager has not been Load()-ed
  //  - after CallManager receives OnShutdown() notification.
  //
  // If we register the call when CanCreateCall() is false, then
  // we will have trouble (i.e. GPF) in Unload(). This is because some
  // interfaces are created dynamically in config execution, so that
  // interfaces will be deleted first before CallManager::Unload() is
  // called.
  //
  if (!CanCreateCall()) {
    assert(false);
    CLog::Error (THISMODULE, "Can not register call %s in %s",
        call->GetName(),
        call->GetInterfaceName());

    // This call will not be deleted by CallManager.
    // Memory leak will happen, but hopefully no GPF will occur.
    return;
  }

  _listCall.push_back(call);
}

/** RemoveActiveCall: removes a Call object from the list of active calls
*/
void CallManager::RemoveActiveCall(Call* call)
{
  _listCall.remove(call);
  RemoveCallFromWaitDigitsList(call);
}

/** Destroy a call instance. In this implementation, it will insert call 
  to the list of calls to be deleted later in HandleEvents.
  */
void CallManager::DestroyCall(Call *call) 
{
  RemoveActiveCall(call);
  if (call->GetTask()) {
    assert(false);
    call->DetachTask();
  }
//  _listCallClearing.push_back(call);
}

/** Load: initializes the call manager
*/
bool CallManager::Load()
{
  _tlsId = Kernel::Instance()->AllocTLS();
  _api = new CallAPI(this,ProvId_CallManager,"CallManager");
  _traceCall = KLog::Instance()->CreateTrace("Call", "call", "Generic call signalling protocol trace");


  // Register configuration commands
  // ctrace-filter
  Term_Cmd* cmd = new Term_Cmd(this, Cmd_CTrace_Filter, "ctrace-filter", "Specify the call trace filter", PrivilegeLevelUser, TermModeNormal, false);
  CVariantChoiceValidator vchoice1("list", "Show call trace filter");
  vchoice1.Add ("add", "Add a new filter (valid tags: interface/ddi/cli/group/pinfo/uinfo/session/direction)");
  vchoice1.Add ("delete", "Delete an existing filter");
  cmd->Add(new Term_Cmd_Arg ("arg1", CVariant(""), vchoice1));
  cmd->Add(new Term_Cmd_Arg("arg2", "", CVariantValidator("", "argument"), TermArgOptional));
  RegisterTermCmd(cmd,Kernel::Instance()->RootConfig());

  cmd = new Term_Cmd(this, Cmd_Call_Show, "call", "Display current active calls", PrivilegeLevelUser, TermModeNormal, false);

  CVariantChoiceValidator vchoice2("list", "List active calls");
  vchoice2.Add("detail", "Show the detail of a particular call");
  vchoice2.Add("disconnect", "Disconnect a particular call");
  vchoice2.Add("quality-list", "List active calls quality");
  cmd->Add(new Term_Cmd_Arg ("@0", CVariant(""), vchoice2));

  CVariantChoiceValidator vchoice3("interface", "List active calls by interface");
  vchoice3.Add("ddi", "List active calls by ddi");
  vchoice3.Add("cli", "List active calls by cli");
  vchoice3.Add("group", "List active calls by group");
  vchoice3.Add("uinfo", "List active calls by user info");
  vchoice3.Add("pinfo", "List active calls by private info");
  vchoice3.Add("session", "List active calls by session");
  vchoice3.Add("task", "List active calls by task");
  cmd->AddDynamic(1, 0, "list", new Term_Cmd_Arg("@1", "", vchoice3));
  cmd->AddDynamic(2, 0, "list", new Term_Cmd_Arg("@2", "", CVariantValidator("", "filter")));

  cmd->AddDynamic(1, 0, "quality-list", new Term_Cmd_Arg("@1", "", vchoice3));
  cmd->AddDynamic(2, 0, "quality-list", new Term_Cmd_Arg("@2", "", CVariantValidator("", "filter")));

  cmd->AddDynamic(1, 0, "detail", new Term_Cmd_Arg("@1", "", CVariantValidator("", "call reference")));
  cmd->AddDynamic(1, 0, "disconnect", new Term_Cmd_Arg("@1", "", CVariantValidator("", "call reference")));

  RegisterTermCmd(cmd,Kernel::Instance()->RootConfig());

  _canCreateCall = true;

  return true;
}

/** OnShutdown: disconnect current calls.
  Disconnecting calls needs to be done here, since some interfaces are created
  during config execution, not during Load()-ing phase.
  */
void CallManager::OnShutdown(bool cancel)
{
  if (cancel) {
    _canCreateCall = true;
  } else {
    for (ListCall::iterator it=_listCall.begin(); it!=_listCall.end(); ) {
      Call* call = *it;

      // iterator must be incremented here because DestroyCall will
      // remove the call from the list.
      ++it;

      switch (call->GetState()) {
        case CS_Clearing:
        case CS_Releasing:
          call->RelComInd(CC_NormalUnspecified);
          break;
        case CS_Null:
          //
          // This condition happens if the call has been cleared (RelComInd() has been called)
          // but the call is kept alive so that the task can query the status.
          // In this case, don't destroy the call because the task will delete it
          // when it quits.
          //
          if (!call->GetTask()) 
            DestroyCall(call);
          else if (!call->_released)
            call->RelComInd(CC_NormalUnspecified);
          break;
        default:
          call->DisconnectReq(CC_NormalUnspecified,CP_NoIndication);
          if (call->GetState() != CS_Null) 
            call->RelComInd(CC_NormalUnspecified);
      }
    }

    //list call may not be empty.
    //assert(_listCall.empty());

    // Delete remaining calls.
    HandleEvents();

    // We don't want to create new calls.
    _canCreateCall = false;

  }
}


/** Unload: Deinitializes the call manager.
  Disconnect all active calls
  */
bool CallManager::Unload()
{
  for (ListCondition::iterator condIt = _listCondition.begin(); condIt!=_listCondition.end(); condIt++) {
    CallTraceCondition* condition = *condIt;
    delete condition;
  }
  _listCondition.erase(_listCondition.begin(),_listCondition.end());

  // Don't call OnShutdown() here.
  // OnShutdown(false);
  if (_listCall.size()) {
    CLog::Warning(THISMODULE, "%d calls are still active when CallManager is shutdown (Unload)");

    try {
      ListCall::iterator it, end;
      end = _listCall.end();
      for (it=_listCall.begin(); it!=end; ++it) {
        Call *call = *it;
        CLog::Warning(THISMODULE, "Call %s (interface %s) is not cleared", call->GetName(), call->GetInterfaceName());
      }
    } catch (...) {
    }
  }

  Kernel::Instance()->FreeTLS(_tlsId);
  delete _api;
  _api = NULL;
  KLog::Instance()->DestroyTrace(_traceCall);
  return true;
}

/** Map CallCause enum to its string representation
*/
const char* CallManager::StrCallCause(CallCause cause)
{
  static struct {
    CallCause cause;
    char *str;
  } xlat[] = {
#define MAKESTR(x)	{ x, #x }
    MAKESTR(CC_Unassigned),
    MAKESTR(CC_UnallocatedNumber),
    MAKESTR(CC_NoRouteToTransitNetwork),
    MAKESTR(CC_NoRouteToDestination),
    MAKESTR(CC_ChannelUnacceptable),
    MAKESTR(CC_NormalCallClearing),
    MAKESTR(CC_UserBusy),
    MAKESTR(CC_NoUserResponding),
    MAKESTR(CC_NoAnswerFromUser),
    MAKESTR(CC_CallRejected),
    MAKESTR(CC_NumberChanged),
    MAKESTR(CC_ExchangeRoutingError),
    MAKESTR(CC_DestinationOutOfOrder),
    MAKESTR(CC_InvalidNumberFormat),
    MAKESTR(CC_FacilityRejected),
    MAKESTR(CC_NormalUnspecified),
    MAKESTR(CC_NoCircuitAvailable),
    MAKESTR(CC_NetworkOutOfOrder),
    MAKESTR(CC_TemporaryFailure),
    MAKESTR(CC_SwitchingEquipmentCongestion),
    MAKESTR(CC_RequestedCircuitUnavailable),
    MAKESTR(CC_ResourceUnavailable),
    MAKESTR(CC_QOSUnavailable),
    MAKESTR(CC_RequestedFacilityUnsubscribed),
    MAKESTR(CC_BearerCapabilityUnauthorized),
    MAKESTR(CC_BearerCapabilityUnavailable),
    MAKESTR(CC_OptionUnavailable),
    MAKESTR(CC_BearerCapabilityUnimplemented),
    MAKESTR(CC_ChannelTypeNotImplemented),
    MAKESTR(CC_RequestedFacilityUnimplemented),
    MAKESTR(CC_ServiceOrOptionNotImplementedUnspecified),
    MAKESTR(CC_InvalidCallReference),
    MAKESTR(CC_ChannelNonExistent),
    MAKESTR(CC_CallIdentityInUse),
    MAKESTR(CC_NoCallSuspended),
    MAKESTR(CC_CallHasBeenCleared),
    MAKESTR(CC_IncompatibleDestination),
    MAKESTR(CC_InvalidTransitNetworkSelection),
    MAKESTR(CC_InvalidMessageUnspecified),
    MAKESTR(CC_MissingParameter),
    MAKESTR(CC_MessageTypeNonExistent),
    MAKESTR(CC_MessageNotCompatibleWithState),
    MAKESTR(CC_MissingInformation),
    MAKESTR(CC_InvalidParameter),
    MAKESTR(CC_RecoveryOnTimeout),
    MAKESTR(CC_ProtocolError),
    MAKESTR(CC_Interworking),
    MAKESTR(CC_CallFailed),
#undef MAKESTR
  };

  for (int i=0;i<sizeof(xlat)/sizeof(xlat[0]);i++) {
    if (cause == xlat[i].cause) return xlat[i].str+3;
  }
  char buffer[32];
  sprintf(buffer,"CAUSE(%d)",cause);
  _retstr = buffer;
  return _retstr.c_str();
}


/** Conversion from CallNumberType to string representation
*/
const char* CallManager::StrCallNumberType(CallNumberType cnt)
{
  static struct {
    CallNumberType cnt;
    char *str;
  } xlat[] = {
#define MAKESTR(x)	{ x, #x }
    MAKESTR(CNT_Unknown),
    MAKESTR(CNT_International),
    MAKESTR(CNT_National),
    MAKESTR(CNT_NetworkSpecific),
    MAKESTR(CNT_SubscriberNumber),
    MAKESTR(CNT_Abbreviated),
#undef MAKESTR
  };

  for (int i=0;i<sizeof(xlat)/sizeof(xlat[0]);i++) {
    if (cnt == xlat[i].cnt) return xlat[i].str+4;
  }
  char buffer[32];
  sprintf(buffer,"CNT(%d)",cnt);
  _retstr = buffer;
  return _retstr.c_str();
}

/** Conversion from CallPartyCategory to string representation
*/
const char* CallManager::StrCallPartyCategory(CallPartyCategory cpc)
{
  static struct {
    CallPartyCategory cpc;
    char *str;
  } xlat[] = {
#define MAKESTR(x)	{ x, #x }
    MAKESTR(CPC_Unknown),
    MAKESTR(CPC_Ordinary),
    MAKESTR(CPC_Payphone),
    MAKESTR(CPC_Data),
    MAKESTR(CPC_Test),
#undef MAKESTR
  };

  for (int i=0;i<sizeof(xlat)/sizeof(xlat[0]);i++) {
    if (cpc == xlat[i].cpc) return xlat[i].str+4;
  }
  char buffer[32];
  sprintf(buffer,"CPC(%d)",cpc);
  _retstr = buffer;
  return _retstr.c_str();
}

/** Conversion from CallConnectionType to string representation
*/
const char* CallManager::StrCallConnectionType(CallConnectionType cct)
{
  static struct {
    CallConnectionType cct;
    char *str;
  } xlat[] = {
#define MAKESTR(x)	{ x, #x }
    MAKESTR(CCT_Speech),
    MAKESTR(CCT_3K1Audio),
    MAKESTR(CCT_64KUnrestrictred),
#undef MAKESTR
  };

  for (int i=0;i<sizeof(xlat)/sizeof(xlat[0]);i++) {
    if (cct == xlat[i].cct) return xlat[i].str+4;
  }
  char buffer[32];
  sprintf(buffer,"CCT(%d)",cct);
  _retstr = buffer;
  return _retstr.c_str();
}


extern char* FormatData(char* str, float val, const char* format);

/** Terminal command callback
*/
void CallManager::DoTermCmd(Term_Session* session, bool no, Term_Cmd* cmd)
{
  CVariant v;
  cmd->GetArg((unsigned)0)->GetValue(v);

  switch (cmd->GetId()) {

    // ctrace-filter
    case Cmd_CTrace_Filter:
      if (strcmpi(v.AsString(),"list") == 0) {

        ACQUIRE_SYSTEM_LOCK();

        if (_listCondition.empty()) {
          RELEASE_SYSTEM_LOCK();
          session->Send("There is no call trace condition" NEWLINE);
          return;
        }

        session->Send("   Id  Condition" NEWLINE);
        session->Send("---------------------------------------------------" NEWLINE);
        for (ListCondition::iterator it = _listCondition.begin(); it!=_listCondition.end(); it++) {
          CallTraceCondition* condition = *it;
          session->Sendf("%5u  %s" NEWLINE, condition->GetId(), condition->GetExpression());
        }

        RELEASE_SYSTEM_LOCK();

      } else if (strcmpi(v.AsString(),"add") == 0) {

        CVariant v1;
        cmd->GetArg((unsigned)1)->GetValue(v1);
        if (strlen(v1.AsString()) == 0) {
          session->SendError("A condition must be specified" NEWLINE);
          return;
        }

        if (_listCondition.size() >= MAXCTRACEFILTER) {
          session->SendError("Too many trace filters defined, please delete older ones first" NEWLINE);
          return;
        }

        ACQUIRE_SYSTEM_LOCK();

        CallTraceCondition* condition = new CallTraceCondition(v1.AsString());
        if (condition->Parse()) {
          _listCondition.push_back(condition);
        } else {
          session->SendError("Invalid condition" NEWLINE);
          delete condition;
        }

        RELEASE_SYSTEM_LOCK();

      } else if (strcmpi(v.AsString(),"delete") == 0) {
        CVariant v1;
        cmd->GetArg((unsigned)1)->GetValue(v1);
        if (strlen(v1.AsString()) == 0) {
          session->SendError("A condition id or 'all' must be specified" NEWLINE);
          return;
        }

        bool deleteAll = (strcmpi(v1.AsString(),"all") == 0);

        ACQUIRE_SYSTEM_LOCK();
        for (ListCondition::iterator it = _listCondition.begin(); it!=_listCondition.end();) {
          CallTraceCondition* condition = *it;
          if (deleteAll || (condition->GetId() == atoi(v1.AsString()))) {
            it = _listCondition.erase(it);
            delete condition;
          } else {
            it++;
          }
        }
        RELEASE_SYSTEM_LOCK();
      } else {
        session->SendError("Unsupported command" NEWLINE);
      }
      break;

      // call list interface/ddi/cli/group

    case Cmd_Call_Show:
      bool quality_list = strcmpi(v.AsString(),"quality-list") == 0;
      if (strcmpi(v.AsString(),"list") == 0 || quality_list) {
        CVariant v1;
        cmd->GetArg((unsigned)1)->GetValue(v1);

        enum ListBy {
          LB_None,
          LB_Interface,
          LB_DDI,
          LB_CLI,
          LB_Group,
          LB_PInfo,
          LB_UInfo,
          LB_Session,
          LB_Task,
        };
        ListBy listby = LB_None;

        if (strcmpi(v1.AsString(),"interface") == 0) {
          listby = LB_Interface;
        } else if (strcmpi(v1.AsString(),"ddi") == 0) {
          listby = LB_DDI;
        } else if (strcmpi(v1.AsString(),"cli") == 0) {
          listby = LB_CLI;
        } else if (strcmpi(v1.AsString(),"group") == 0) {
          listby = LB_Group;
        } else if (strcmpi(v1.AsString(),"uinfo") == 0) {
          listby = LB_UInfo;
        } else if (strcmpi(v1.AsString(),"pinfo") == 0) {
          listby = LB_PInfo;
        } else if (strcmpi(v1.AsString(),"session") == 0) {
          listby = LB_Session;
        } else if (strcmpi(v1.AsString(),"task") == 0) {
          listby = LB_Task;
        } else {
          session->SendError("Unsupported command" NEWLINE);
          return;
        }

        // build regular expression
        CVariant v2;
        cmd->GetArg((unsigned)2)->GetValue(v2);

        regex_t r;
        if (regcomp(&r, v2.AsString(), REG_ICASE|REG_EXTENDED)) {
          session->SendError("Invalid regular expression in filter" NEWLINE);
          return;
        }

        List<CallDescription> listCallDesc;

        ACQUIRE_SYSTEM_LOCK();
        for (ListCall::iterator it=_listCall.begin();it!=_listCall.end();it++) {
          bool match = false;
          Call* call = *it;
          char taskId[32];
          if (call->GetTask()) sprintf(taskId,"%u",call->GetTask()->GetPID());
          else strcpy(taskId,"(null)");
          switch (listby) {
            case LB_Interface:
              match = (regexec(&r, call->GetInterfaceName(), 0, NULL, 0) == 0);
              break;

            case LB_Group:
              match = (regexec(&r, call->GetGroupName(), 0, NULL, 0) == 0);
              break;

            case LB_PInfo:
              match = (regexec(&r, call->GetCallParameter()->cpinfo.c_str(), 0, NULL, 0) == 0);
              break;

            case LB_UInfo:
              match = (regexec(&r, call->GetCallParameter()->cui.c_str(), 0, NULL, 0) == 0);
              break;

            case LB_Session:
              match = (regexec(&r, call->GetCallParameter()->csi.c_str(), 0, NULL, 0) == 0);
              break;

            case LB_Task:
              match = (regexec(&r, taskId, 0, NULL, 0) == 0);
              break;

            case LB_DDI:
              match = (regexec(&r, call->GetCallParameter()->cnCalled.c_str(), 0, NULL, 0) == 0);
              break;

            case LB_CLI:
              match = (regexec(&r, call->GetCallParameter()->cnCalling.c_str(), 0, NULL, 0) == 0);
              break;
          }

          // put all matched entry in a list
          if (match) {
            CallDescription cd;
            //bug:
            //memset(&cd, 0, sizeof(cd));
            cd.cparam = *call->GetCallParameter();
            cd.callName = call->GetName();
            if (call->_hasBeenConnected) cd.connect = call->_tmConnected.get_current_sec();
            else cd.connect = 0;
            cd.holding = call->_tmHolding.get_current_sec();
            cd.itf = call->GetInterfaceName();
            cd.media = call->GetMedia() ? call->GetMedia()->GetName() : "(null)";
            cd.grp = call->GetGroupName();
            cd.dir = call->GetDirection();
            cd.state = call->GetState();
            cd.taskId = taskId;

            if (quality_list) {
              cd.rx_packets = call->GetQuality(CQ_RX_PACKETS);
              cd.rx_rtt = call->GetQuality(CQ_RX_RTT);
              cd.rx_jitter = call->GetQuality(CQ_RX_JITTER);
              cd.rx_loss = call->GetQuality(CQ_RX_LOSS);
              cd.rx_itot = call->GetQuality(CQ_RX_ITOT);

              cd.tx_packets = call->GetQuality(CQ_TX_PACKETS);
              cd.tx_rtt = call->GetQuality(CQ_TX_RTT);
              cd.tx_jitter = call->GetQuality(CQ_TX_JITTER);
              cd.tx_loss = call->GetQuality(CQ_TX_LOSS);
              cd.tx_itot = call->GetQuality(CQ_TX_ITOT);
            }

            listCallDesc.push_back(cd);
          }
        }
        RELEASE_SYSTEM_LOCK();

        if (listCallDesc.empty()) {
          regfree(&r);
          session->SendError("There is no call with such criteria" NEWLINE);
          return;
        }

        if (!quality_list) {
          session->Send(NEWLINE);
          session->Sendf("  # Group    Itf.   DDI/CLI                Dir CRef TaskId     Time State" NEWLINE);
          session->Sendf("--------------------------------------------------------------------------------" NEWLINE);
          List<CallDescription>::iterator it1 = listCallDesc.begin();
          List<CallDescription>::iterator end = listCallDesc.end();
          int line = 0;
          while (it1 != end) {
            CallDescription cd = *it1++;
            char shortGroup[9];
            safe_strcpy(shortGroup,cd.grp.c_str(),sizeof(shortGroup));
            char shortItf[7];
            safe_strcpy(shortItf,cd.itf.c_str(),sizeof(shortItf));
            char time[32];
            sprintf(time,"%u:%02u",cd.holding/60,cd.holding%60);
            char ddicli[128];
            sprintf(ddicli,"%s/%s",cd.cparam.cnCalled.c_str(),cd.cparam.cnCalling.c_str());
            ddicli[22] = '\0';
            session->Sendf(
                "%3u %-8s %-6s %-22s %s %s %-8s %6s %s" NEWLINE,
                ++line,
                shortGroup,
                shortItf,
                ddicli,
                cd.dir == CD_Incoming ? "In " : "Out",
                cd.callName.c_str(),
                cd.taskId.c_str(),
                time,
                StrCallState(cd.state));
          }
          session->Send(NEWLINE);
        }
        else {
          session->Send(NEWLINE);
          session->Sendf("  # Media          DDI        Dir St  Pkts  RTT Jitt  Lost   R   Time State     " NEWLINE);
          session->Sendf("--------------------------------------------------------------------------------" NEWLINE);
          List<CallDescription>::iterator it1 = listCallDesc.begin();
          List<CallDescription>::iterator end = listCallDesc.end();
          int line = 0;
          while (it1 != end) {
            CallDescription cd = *it1++;
            char shortGroup[9];
            safe_strcpy(shortGroup,cd.grp.c_str(),sizeof(shortGroup));
            char shortMedia[15];
            safe_strcpy(shortMedia,cd.media.c_str(),sizeof(shortMedia));
            char ddi[64];
            strcpy(ddi, cd.cparam.cnCalled.c_str());
            if (cd.cparam.cnCalled.size() > 10) {
              ddi[9] = '-';	
              ddi[10] = '\0';
            }

            char stat[32], packets[10], r[10], rtt[10], jitter[10], loss[10];

            sprintf(stat, "%5s %4s %4s %5s %3s", 
                FormatData(packets, cd.rx_packets, "%4.1f"),
                FormatData(rtt, cd.rx_rtt, "%3.0f"),
                FormatData(jitter, cd.rx_jitter, "%3.0f"),
                FormatData(loss, cd.rx_loss, "%4.1f%%"),
                FormatData(r, E_Model::Instance()->Calculate_R(cd.rx_itot), "%3.0f")
                );

            char time[32];
            sprintf(time,"%u:%02u",cd.holding/60,cd.holding%60);

            session->Sendf(
                "%3u %-14s %-10s %s %s %-25s %6s %s" NEWLINE,
                ++line,
                shortMedia,
                ddi,
                cd.dir == CD_Incoming ? "In " : "Out",
                "Rx",
                stat,
                time,
                StrCallState(cd.state));


            sprintf(stat, "%5s %4s %4s %5s %3s", 
                FormatData(packets, cd.tx_packets, "%4.1f"),
                FormatData(rtt, cd.tx_rtt, "%3.0f"),
                FormatData(jitter, cd.tx_jitter, "%3.0f"),
                FormatData(loss, cd.tx_loss, "%4.1f%%"),
                FormatData(r, E_Model::Instance()->Calculate_R(cd.tx_itot), "%3.0f")
                );

            session->Sendf(
                "    %-14s %-10s %s %s %-25s %6s %s" NEWLINE,
                "",
                "",
                "   ",
                "Tx",
                stat,
                "",
                "");

          }
          session->Send(NEWLINE);
        }
        regfree(&r);

      } else if (strcmpi(v.AsString(),"detail") == 0) {
        // get call reference
        CVariant v1;
        cmd->GetArg((unsigned)1)->GetValue(v1);

        bool found = false;
        CallDescription cd;
#ifdef CALL_LOG_TRANSITION
        Call::ListTransition transition;
#endif

        ACQUIRE_SYSTEM_LOCK();
        for (ListCall::iterator it=_listCall.begin();it!=_listCall.end();it++) {
          Call* call = *it;
          char taskId[32];
          char appl[64];
          if (call->GetTask()) {
            sprintf(taskId,"%u",call->GetTask()->GetPID());
            strcpy(appl,call->GetTask()->GetApplication()->Name().c_str());
          } else {
            strcpy(taskId,"(null)");
            strcpy(appl,"null");
          }
          // put all matched entry in a list
          if (strcmpi(call->GetName(),v1.AsString()) == 0) {
            cd.cparam = *call->GetCallParameter();
            cd.callName = call->GetName();
            if (call->_hasBeenConnected) cd.connect = call->_tmConnected.get_current_sec();
            else cd.connect = 0;
            cd.holding = call->_tmHolding.get_current_sec();
            cd.media = call->GetMedia() ? call->GetMedia()->GetName() : "(null)";
            cd.itf = call->GetInterfaceName();
            cd.grp = call->GetGroupName();
            cd.dir = call->GetDirection();
            cd.state = call->GetState();
            cd.taskId = taskId;
            cd.appl = appl;
#ifdef CALL_LOG_TRANSITION
            transition = call->_transition;
#endif
            found = true;
            break;
          }
        }
        RELEASE_SYSTEM_LOCK();

        if (!found) {
          session->SendError("No call with such call reference" NEWLINE);
          return;
        }

        char holdtm[32];
        sprintf(holdtm,"%u:%02u",cd.holding/60,cd.holding%60);
        char conntm[32];
        sprintf(conntm,"%u:%02u",cd.connect/60,cd.connect%60);
        session->Sendf(" Call Ref        : [%s] %s\n",cd.callName.c_str(),cd.dir == CD_Incoming ? "Incoming" : "Outgoing");
        session->Sendf(" Call State      : %s\n",StrCallState(cd.state));
        session->Sendf(" Location        : %s at %s (group=%s)\n",cd.media.c_str(),cd.itf.c_str(),cd.grp.c_str());
        session->Sendf(" Session Id      : %s\n",cd.cparam.csi.c_str());
        session->Sendf(" Called Number   : %s (type=%s)\n",cd.cparam.cnCalled.c_str(),StrCallNumberType(cd.cparam.cntCalled));
        session->Sendf(" Sending Mode    : %s\n",cd.cparam.complete ? "Complete" : "Overlap");
        session->Sendf(" Call Type       : ");
        switch (cd.cparam.cct) {
          case CCT_Speech:
            session->Sendf("Speech\n");
            break;
          case CCT_3K1Audio:
            session->Sendf("Audio 3.1 KHz\n");
            break;
          case CCT_64KUnrestrictred:
            session->Sendf("64 Kbps\n");
            break;
          default:
            session->Sendf("(unknown %d)\n", cd.cparam.cct);
        };
        if (cd.cparam.cnCalling.length()) 
          session->Sendf(" Calling Number  : %s (type=%s, category=%s)\n",cd.cparam.cnCalling.c_str(),StrCallNumberType(cd.cparam.cntCalling),StrCallPartyCategory(cd.cparam.cpcCalling));
        session->Sendf(" Presentation    : ");
        switch (cd.cparam.ccp) {
          case CCP_Allowed:
            session->Sendf("Allowed\n");
            break;
          case CCP_Restricted:
            session->Sendf("Restricted\n");
            break;
          default:
            session->Sendf("Unavailable\n");
        };
        session->Sendf(" Screening       : ");
        switch (cd.cparam.ccs) {
          case CCS_UserNotScreened:
            session->Sendf("User not screened\n");
            break;
          case CCS_UserVerifiedPassed:
            session->Sendf("User verified passed\n");
            break;
          case CCS_UserVerifiedFailed:
            session->Sendf("User verified failed\n");
            break;
          default:
            session->Sendf("Network\n");
            break;
        };
        if (cd.cparam.cui.length())
          session->Sendf(" User Info       : %s\n",cd.cparam.cui.c_str());
        if (cd.cparam.cpinfo.length())
          session->Sendf(" Private Info    : %s\n",cd.cparam.cpinfo.c_str());
        if (cd.cparam.csaCalled.length())
          session->Sendf(" Called Address  : %s\n",cd.cparam.csaCalled.c_str());
        if (cd.cparam.csaCalling.length())
          session->Sendf(" Calling Address : %s\n",cd.cparam.csaCalling.c_str());
        session->Sendf(" Duration        : %s (holding)  %s (connected)\n",holdtm,conntm);
        session->Sendf(" Task Id         : %s (app=%s)\n",cd.taskId.c_str(),cd.appl.c_str());
#ifdef CALL_LOG_TRANSITION
        session->Sendf(" Transition      : ");
        for (Call::ListTransition::iterator iter=transition.begin();iter!=transition.end();iter++) {
          session->Sendf("%02X ",(*iter));
        }
        session->Send("\n");
#endif
      } else if (strcmpi(v.AsString(),"disconnect") == 0) {
        // get call reference
        CVariant v1;
        cmd->GetArg((unsigned)1)->GetValue(v1);

        bool found = false;
        CallDescription cd;

        Call* call = NULL;
        ACQUIRE_SYSTEM_LOCK();
        for (ListCall::iterator it=_listCall.begin();it!=_listCall.end();it++) {
          // find matched call
          if (strcmpi((*it)->GetName(),v1.AsString()) == 0) {
            call = *it;
            break;
          }
        }

        if (!call) {
          RELEASE_SYSTEM_LOCK();
          session->SendError("No call with such call reference" NEWLINE);
          return;
        }

        call->DisconnectInd(CC_CallHasBeenCleared,CP_NoIndication);
        switch (call->GetState()) {
          case CS_Null:
            break;
          case CS_Releasing:
            call->RelComInd(CC_NormalUnspecified);
            break;
          default:
            call->DisconnectReq(CC_NormalUnspecified,CP_NoIndication);
        }
        RELEASE_SYSTEM_LOCK();

      } else {
        session->SendError("Unsupported command" NEWLINE);
      }
      break;
  }
}


/** Called by a Call object to notify of a new incoming call, inform the kernel 
  and attach the call to the newly created task
  */
void CallManager::OnIncomingCall(Call* call)
{
  // reset the global variable to NULL, it will then be set
  // to the correct value in CallManager::AttachTask
  _lastTaskAttached = NULL;
  const char* ddi = call->GetCallParameter()->cnCalled.c_str();
  const char* cli = call->GetCallParameter()->cnCalling.c_str();
  CLog::Detail(THISMODULE,"CallManager::OnIncomingCall : Notify Kernel of a new incoming call(ddi=%s,cli=%s)",ddi,cli);

  CallCause cause;
  _currentInCall = call;
  STATUS rc = Kernel::Instance()->RouteIncomingCall(this,call,cause);
  _currentInCall = NULL;

  if (rc == OK && _lastTaskAttached) {
    CLog::Detail(THISMODULE,"CallManager::OnIncomingCall : call->AttachTask() ; _lastTaskAttached->->GetTaskName() = %s", _lastTaskAttached->GetTaskName());

    // added of rmultithreaded_ippbx ; to be removed later
    ///////_lastTaskAttached->SetTaskName("si0000");

    call->AttachTask(_lastTaskAttached);

    // send call proceeding to the caller if the application
    // is configured that way
    if (call->GetCallParameter()->complete && 
        _lastTaskAttached->GetApplication()->AppSendCallProceeding()) {
      call->ProceedingReq(CP_NoIndication, 0);
    }
  } else {
    call->DisconnectReq(cause, CP_NoIndication);
  }
}

/** Return the current incoming call
*/
Call* CallManager::GetCurrentIncomingCall()
{
  return _currentInCall;
}

/** Check if this call should be included in the call trace or not
*/
bool CallManager::TestTraceCondition(Call* call, unsigned *id)
{
  if (TraceEnabled()) {
    for (ListCondition::iterator it = _listCondition.begin(); it!=_listCondition.end(); it++) {
      CallTraceCondition* condition = *it;
      CallParameter* cparam = call->GetCallParameter();
      if (condition->Match(
            cparam->cnCalled,cparam->cnCalling,
            call->GetGroupName(),call->GetInterfaceName(),
            cparam->cpinfo,cparam->cui,cparam->csi,
            call->GetDirection() == CD_Incoming ? "in" : "out")) 
      {
        *id = condition->GetId();
        call->EnableTrace();
        return true;
      }
    }
  }
  return false;
}

/** TraceCall: Called by call instance to output call trace to the user
*/
void CallManager::TraceCall(Call* call,bool received,const char* message,const char* format,...)
{
  char buffer[256];
  sprintf(buffer,
      "C%02u %s [%s:%s] %s %s",
      call->GetTraceId(),
      received ? "RX" : "TX",
      call ? call->GetInterfaceName() : "(null)",
      call ? call->GetName() : "null",
      message,
      format ? format : "");

  va_list marker;
  va_start(marker,format);
  KLog::Instance()->Trace(KLog::LFmt_TimeMsec,_traceCall,buffer,marker);
  va_end(marker);
}

/** TraceEnabled: Return the state of call tracing
*/
bool CallManager::TraceEnabled()
{
  return _traceCall->traceEnabled != 0;
}

/** Call statistics counters
*/
void CallManager::StatIncCallCounter(CallDirection cd)
{
  _statCurNbCall++;
  if (cd == CD_Outgoing) _statCurNbOutCall++;
  else _statCurNbInCall++;
  if (_statMaxNbCall < _statCurNbCall) _statMaxNbCall = _statCurNbCall;
  if (_statMaxNbOutCall < _statCurNbOutCall) _statMaxNbOutCall = _statCurNbOutCall;
  if (_statMaxNbInCall < _statCurNbInCall) _statMaxNbInCall = _statCurNbInCall;

  time_t now = time(NULL);
  if (_statLastUpdate != now) {
    if (_statMaxCallPSec < _statCurCallPSec) _statMaxCallPSec = _statCurCallPSec;
    if (_statMaxOutCallPSec < _statCurOutCallPSec) _statMaxOutCallPSec = _statCurOutCallPSec;
    if (_statMaxInCallPSec < _statCurInCallPSec) _statMaxInCallPSec = _statCurInCallPSec;

    _statCurCallPSec = 0;
    _statCurOutCallPSec = 0;
    _statCurInCallPSec = 0;

    _statLastUpdate = now;
  }

  _statCurCallPSec++;
  if (cd == CD_Outgoing) _statCurOutCallPSec++;
  else _statCurInCallPSec++;

  if (_statMaxCallPSec < _statCurCallPSec) _statMaxCallPSec = _statCurCallPSec;
  if (_statMaxOutCallPSec < _statCurOutCallPSec) _statMaxOutCallPSec = _statCurOutCallPSec;
  if (_statMaxInCallPSec < _statCurInCallPSec) _statMaxInCallPSec = _statCurInCallPSec;
}

/** Call statistics counters
*/
void CallManager::StatDecCallCounter(CallDirection cd)
{
  _statCurNbCall--;
  if (cd == CD_Outgoing) _statCurNbOutCall--;
  else _statCurNbInCall--;
}

void CallManager::ShowCallStats(Term_Session* session)
{
  time_t now = time(NULL);
  if (_statLastUpdate != now) {
    if (_statMaxCallPSec < _statCurCallPSec) _statMaxCallPSec = _statCurCallPSec;
    if (_statMaxOutCallPSec < _statCurOutCallPSec) _statMaxOutCallPSec = _statCurOutCallPSec;
    if (_statMaxInCallPSec < _statCurInCallPSec) _statMaxInCallPSec = _statCurInCallPSec;

    _statCurCallPSec = 0;
    _statCurOutCallPSec = 0;
    _statCurInCallPSec = 0;

    _statLastUpdate = now;
  }

  session->Send(STAT_INDENT "Current counters:" NEWLINE);
  session->Sendf(STAT_INDENT2 "Nb of concurrent incoming calls: %u" NEWLINE,_statCurNbInCall);
  session->Sendf(STAT_INDENT2 "Nb of concurrent outgoing calls: %u" NEWLINE,_statCurNbOutCall);
  session->Sendf(STAT_INDENT2 "Total nb of concurrent calls: %u" NEWLINE,_statCurNbCall);
  session->Sendf(STAT_INDENT2 "Incoming calls/sec: %u" NEWLINE,_statCurInCallPSec);
  session->Sendf(STAT_INDENT2 "Outgoing calls/sec: %u" NEWLINE,_statCurOutCallPSec);
  session->Sendf(STAT_INDENT2 "Total calls/sec: %u" NEWLINE,_statCurCallPSec);

  session->Send(STAT_INDENT "Maximum counters:" NEWLINE);
  session->Sendf(STAT_INDENT2 "Nb of concurrent incoming calls: %u" NEWLINE,_statMaxNbInCall);
  session->Sendf(STAT_INDENT2 "Nb of concurrent outgoing calls: %u" NEWLINE,_statMaxNbOutCall);
  session->Sendf(STAT_INDENT2 "Total nb of concurrent calls: %u" NEWLINE,_statMaxNbCall);
  session->Sendf(STAT_INDENT2 "Incoming calls/sec: %u" NEWLINE,_statMaxInCallPSec);
  session->Sendf(STAT_INDENT2 "Outgoing calls/sec: %u" NEWLINE,_statMaxOutCallPSec);
  session->Sendf(STAT_INDENT2 "Total calls/sec: %u" NEWLINE,_statMaxCallPSec);
}


/*----------------------------------------------------------------* 
 * Implementation of CallAPI
 *----------------------------------------------------------------*/

CallAPI::CallAPI(CallManager* manager,int providerId,const char* name) : 
  _manager(manager),
  CAPIProvider(providerId,name,300)
{
  if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
    CLog::Error( THISMODULE, "Unable to register provider");
    return;
  }

  /** Register API entry points
  */
  // ordinal for entry points
  enum API_Id {
    Id_GetCallParam = 0,
    Id_CreateDefaultCallParam,

    Id_CParamSetSessionId = 10,
    Id_CParamSetConnectionType,
    Id_CParamSetCompandingLaw,
    Id_CParamSetCalledPartyCategory,
    Id_CParamSetCalledNumber,
    Id_CParamSetCalledNumberPlan,
    Id_CParamSetCalledNumberType,
    Id_CParamSetCalledSubaddress,
    Id_CParamSetCalledSignalAddress,
    Id_CParamSetCalledComplete,
    Id_CParamSetUserInfo,
    Id_CParamSetCallingPartyCategory,
    Id_CParamSetCallingNumber,
    Id_CParamSetCallingNumberPlan,
    Id_CParamSetCallingNumberType,
    Id_CParamSetCallingSubaddress,
    Id_CParamSetCallingSignalAddress,
    Id_CParamSetCallingPresentation,
    Id_CParamSetCallingScreening,
    Id_CParamSetPrivateInfo,
    Id_CParamSetRedirectingNumber,
    Id_CParamSetRedirectingNumberPlan,
    Id_CParamSetRedirectingNumberType,
    Id_CParamSetRedirectingNumberPresentation,
    Id_CParamSetOriginalCalledNumber,
    Id_CParamSetOriginalCalledNumberPlan,
    Id_CParamSetOriginalCalledNumberType,
    Id_CParamSetOriginalCalledNumberPresentation,

    Id_CParamSetCallingMobileIdentityType,
    Id_CParamSetCallType,

    Id_CParamGetSessionId = 40,
    Id_CParamGetConnectionType,
    Id_CParamGetCompandingLaw,
    Id_CParamGetCalledPartyCategory,
    Id_CParamGetCalledNumber,
    Id_CParamGetCalledNumberPlan,
    Id_CParamGetCalledNumberType,
    Id_CParamGetCalledSubaddress,
    Id_CParamGetCalledSignalAddress,
    Id_CParamGetCalledComplete,
    Id_CParamGetUserInfo,
    Id_CParamGetCallingPartyCategory,
    Id_CParamGetCallingNumber,
    Id_CParamGetCallingNumberPlan,
    Id_CParamGetCallingNumberType,
    Id_CParamGetCallingSubaddress,
    Id_CParamGetCallingSignalAddress,
    Id_CParamGetCallingPresentation,
    Id_CParamGetCallingScreening,
    Id_CParamGetPrivateInfo,
    Id_CParamGetRedirectingNumber,
    Id_CParamGetRedirectingNumberPlan,
    Id_CParamGetRedirectingNumberType,
    Id_CParamGetRedirectingNumberPresentation,
    Id_CParamGetOriginalCalledNumber,
    Id_CParamGetOriginalCalledNumberPlan,
    Id_CParamGetOriginalCalledNumberType,
    Id_CParamGetOriginalCalledNumberPresentation,

    Id_CParamGetCallingMobileIdentityType,
    Id_CParamGetCallType,

    Id_ExSetDisconnectMsg = 70,
    Id_SetCharging,
#ifdef CALL_TRANSFER
    //  Id_CParamGetRedirectionAddress,
#endif        

    Id_Setup = 80,
    Id_SendOverlap,
    Id_GetMoreDigits,
    Id_Proceeding,
    Id_Progress,
    Id_Alerting,
    Id_Answer,
    Id_Suspend,
    Id_Resume,
    Id_Disconnect,
    Id_GetCause,
    Id_SignallingConnect,
    Id_SignallingDisconnect,
    Id_GetDirection,
    Id_GetState,
    Id_GetMoreDigitsEx,
    Id_NotifyECT,
    Id_Setup_callque,

    Id_GetMedia = 100,
    Id_GetMediaName,
    Id_GetInterfaceName,
    Id_GetGroupName,
    Id_ReleaseMedia,

    Id_GetFirstCallHandle = 110,
    Id_GetNextCallHandle,

    Id_GetSessionId = 120,
    Id_GetConnectionType,
    Id_GetCompandingLaw,
    Id_GetCalledPartyCategory,
    Id_GetCalledNumber,
    Id_GetCalledNumberPlan,
    Id_GetCalledNumberType,
    Id_GetCalledSubaddress,
    Id_GetCalledSignalAddress,
    Id_GetCalledComplete,
    Id_GetUserInfo,
    Id_GetCallingPartyCategory,
    Id_GetCallingNumber,
    Id_GetCallingNumberPlan,
    Id_GetCallingNumberType,
    Id_GetCallingSubaddress,
    Id_GetCallingSignalAddress,
    Id_GetCallingPresentation,
    Id_GetCallingScreening,
    Id_GetPrivateInfo,
    Id_GetQuality,
    Id_CallGetAuthorization,

    Id_GetRedirectingNumber,
    Id_GetRedirectingNumberPlan,
    Id_GetRedirectingNumberType,
    Id_GetRedirectingNumberPresentation,
    Id_GetOriginalCalledNumber,
    Id_GetOriginalCalledNumberPlan,
    Id_GetOriginalCalledNumberType,
    Id_GetOriginalCalledNumberPresentation,

    Id_GetCallingMobileIdentityType,
    Id_GetCallType,
    Id_CParamSetBssapDH,
    Id_CParamGetBssapDH,
    Id_CParamSetSuspendResumeNotification,
    Id_CParamSetCallWaitingFlag,
    Id_CParamGetSuspendResumeNotification,
    Id_CParamGetCallWaitingFlag,
    Id_GetSuspendResumeNotification,
    Id_GetCallWaitingFlag,
    Id_SetSuspendResumeNotification,
    Id_GetBssapDH,
    Id_SetECTNotification,
    Id_CParamSetECTNotification,

    Id_HmpConfEndRecordFile=170,
    Id_HmpConfFindRoom=171,
    Id_HmpAttachTask=172,
    Id_SetConferenceFlag=173,

    Id_CParamSetLocationNumber = 200,
    Id_CParamSetLocationNumberPlan,
    Id_CParamSetLocationNumberType,
    Id_CParamSetLocationNumberPresentation,
    Id_CParamGetLocationNumber,
    Id_CParamGetLocationNumberPlan,
    Id_CParamGetLocationNumberType,
    Id_CParamGetLocationNumberPresentation,
    Id_GetLocationNumber,
    Id_GetLocationNumberPlan,
    Id_GetLocationNumberType,
    Id_GetLocationNumberPresentation,
    Id_CParamSetRedirectionIndicator,
    Id_CParamSetRedirectionOrigReason,
    Id_CParamSetRedirectionCounter,
    Id_CParamSetRedirectionReason,
    Id_CParamGetRedirectionIndicator,
    Id_CParamGetRedirectionOrigReason,
    Id_CParamGetRedirectionCounter,
    Id_CParamGetRedirectionReason,
    Id_GetRedirectionIndicator,
    Id_GetRedirectionOrigReason,
    Id_GetRedirectionCounter,
    Id_GetRedirectionReason,
    Id_CParamSetCredential,
    Id_CParamSetHuntGroupCaller,
    Id_CParamSetSwitchBoardCall, // index = 226
    Id_Reg_IpInfo,
    Id_RemoveFile=228,
    Id_CreateFilePath=229,
    Id_SetHostInfo=230,
    Id_GetHostInfo=231,
    Id_CallSendInfo=232,

#ifdef XGATE_HAS_HMP
    Id_HmpGetDtmfDigits=240,
    Id_HmpPlayFile=241,
    Id_HmpStopPlayFile=242,
    Id_HmpPlayFileBkgnd=243,
    Id_HmpStopPlayFileBkgnd=244,

    Id_HmpGetDigits=245,
    Id_HmpClearDigits=246,
    Id_HmpGetDigitTimeout=247,
    Id_HmpSetDigitTimeout=248,
    Id_HmpRecordFile=249,
    Id_HmpEndRecordFile,
    Id_HmpConfCreateRoom=251,
    Id_HmpConfDeleteRoom=252,
    Id_HmpConfRemoveParty=253,
    Id_HmpConfOpenMedia=254,
    Id_HmpConfRecordFile=255,
#endif
#ifdef XGATE_HAS_ACME_SBC
    Id_CParamSetRouteSignalAddress=256,
    Id_CParamGetRouteSignalAddress=257,
    Id_CParamSetProxyUserName=258, //TODO: Yoga, added for stateful sbc proxy to terminate deskphone calls
    Id_CParamGetProxyUserName=259, //TODO: Yoga, added for stateful sbc proxy to terminate deskphone calls
    Id_CParamSetCSFilename=260, 
    //Id_CParamSetDomainName=260, //TODO: Yoga, added for stateful sbc proxy to terminate deskphone calls
    //Id_CParamGetDomainName=261, //TODO: Yoga, added for stateful sbc proxy to terminate deskphone calls
#endif
    Id_CParamSetMappedUserNumber=261,
    Id_CParamGetMappedUserNumber=262,
    Id_CParamSetMappedUserDeviceType=263,
    Id_CParamGetMappedUserDeviceType=264,
    Id_CParamSetMappedUserProxyName=265,
    Id_CParamGetMappedUserProxyName=266,
    Id_CParamSetMappedUserSignalAddress=267,
    Id_CParamGetMappedUserSignalAddress=268,
    Id_CParamSetMappedUserOutInterface=269,
    Id_CParamGetMappedUserOutInterface=270,
    Id_CParamSetCalledDeviceType=271, 
    Id_CParamSetCallRecording=272, 
    Id_CParamSetCallOnHoldFilenameCallingParty=273,
    Id_CParamSetCallOnHoldFilenameCalledParty=274,
    Id_SendAppNotification=275, 
    Id_CParamSetRbtFile=276,
    Id_CParamGetSwitchBoardCallState=277, // index = 277
    Id_CParamSetSwitchBoardCallState=278, // index = 278
    Id_HmpSwbPlayFile=279,
    Id_CParamSwitchBoardSuspendTask=280, // index = 280
    Id_CParamSetIsForkingEnabled=281, // isForkingenabled
    Id_CParamSetCallForwardingFilename=282,
    Id_CParamSetVoicemailRecordingFilename=283
  };

  RegisterAPI(
      "CParamSetMappedUserNumber",                      // external name
      Id_CParamSetMappedUserNumber,                             // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetMappedUserNumber,
      CAPIDef::CDECL_TYPE,                      // calling convention
      4,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("mappedUserKey",BYVAL,CVariant::VT_STRING),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  RegisterAPI(
      "CParamSetMappedUserDeviceType",                  // external name
      Id_CParamSetMappedUserDeviceType,                         // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetMappedUserDeviceType,
      CAPIDef::CDECL_TYPE,                      // calling convention
      4,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("mappedUserKey",BYVAL,CVariant::VT_STRING),
      CArgType("value",BYVAL,CVariant::VT_STRING));
  RegisterAPI(
      "CParamSetMappedUserProxyName",                   // external name
      Id_CParamSetMappedUserProxyName,                          // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetMappedUserProxyName,
      CAPIDef::CDECL_TYPE,                      // calling convention
      4,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("mappedUserKey",BYVAL,CVariant::VT_STRING),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  RegisterAPI(
      "CParamSetMappedUserSignalAddress",                       // external name
      Id_CParamSetMappedUserSignalAddress,                              // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetMappedUserSignalAddress,
      CAPIDef::CDECL_TYPE,                      // calling convention
      4,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("mappedUserKey",BYVAL,CVariant::VT_STRING),
      CArgType("value",BYVAL,CVariant::VT_STRING));
  RegisterAPI(
      "CParamSetMappedUserOutInterface",                        // external name
      Id_CParamSetMappedUserOutInterface,                               // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetMappedUserOutInterface,
      CAPIDef::CDECL_TYPE,                      // calling convention
      4,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("mappedUserKey",BYVAL,CVariant::VT_STRING),
      CArgType("value",BYVAL,CVariant::VT_STRING));




  // GetCallParam(CTask* task,CallParameter **cparam,int handle)
  RegisterAPI(
      "CallGetCallParam",			// external name
      Id_GetCallParam,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCallParam,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CreateDefaultCallParam(CTask* task,CallParameter **cparam,const char* interface)
  RegisterAPI(
      "CallCreateDefaultCallParam",		// external name
      Id_CreateDefaultCallParam,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CreateDefaultCallParam,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("interface",BYVAL,CVariant::VT_STRING));

  // SetSessionId(CTask* task,CallParameter* cparam,const char* value)
  RegisterAPI(
      "CParamSetSessionId",			// external name
      Id_CParamSetSessionId,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetSessionId,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // SetConnectionType(CTask* task,CallParameter* cparam,CallConnectionType value);
  RegisterAPI(
      "CParamSetConnectionType",		// external name
      Id_CParamSetConnectionType,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetConnectionType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetCompandingLaw(CTask* task,CallParameter* cparam,CallCompandingLaw value);
  RegisterAPI(
      "CParamSetCompandingLaw",		// external name
      Id_CParamSetCompandingLaw,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCompandingLaw,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetCalledPartyCategory(CTask* task,CallParameter* cparam,CallPartyCategory value);
  RegisterAPI(
      "CParamSetCalledPartyCategory",		// external name
      Id_CParamSetCalledPartyCategory,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCalledPartyCategory,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetCalledNumber(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetCalledNumber",		// external name
      Id_CParamSetCalledNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCalledNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // SetCalledNumberPlan(CTask* task,CallParameter* cparam,CallNumberPlan value);
  RegisterAPI(
      "CParamSetCalledNumberPlan",		// external name
      Id_CParamSetCalledNumberPlan,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCalledNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetCalledNumberType(CTask* task,CallParameter* cparam,CallNumberType value);
  RegisterAPI(
      "CParamSetCalledNumberType",		// external name
      Id_CParamSetCalledNumberType,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCalledNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetCalledSubaddress(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetCalledSubaddress",		// external name
      Id_CParamSetCalledSubaddress,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCalledSubaddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // SetCalledSignalAddress(CTask* task,CallParameter* cparam,const char* value);
  RegisterAPI(
      "CParamSetCalledSignalAddress",		// external name
      Id_CParamSetCalledSignalAddress,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCalledSignalAddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // SetCalledComplete(CTask* task,CallParameter* cparam,CallComplete value);
  RegisterAPI(
      "CParamSetCalledComplete",		// external name
      Id_CParamSetCalledComplete,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCalledComplete,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetUserInfo(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetUserInfo",			// external name
      Id_CParamSetUserInfo,				// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetUserInfo,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // SetCallingPartyCategory(CTask* task,CallParameter* cparam,CallPartyCategory value);
  RegisterAPI(
      "CParamSetCallingPartyCategory",	// external name
      Id_CParamSetCallingPartyCategory,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallingPartyCategory,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetCallingNumber(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetCallingNumber",		// external name
      Id_CParamSetCallingNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallingNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // SetCallingNumberPlan(CTask* task,CallParameter* cparam,CallNumberPlan value);
  RegisterAPI(
      "CParamSetCallingNumberPlan",		// external name
      Id_CParamSetCallingNumberPlan,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallingNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetCallingNumberType(CTask* task,CallParameter* cparam,CallNumberType value);
  RegisterAPI(
      "CParamSetCallingNumberType",		// external name
      Id_CParamSetCallingNumberType,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallingNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetCallingSubaddress(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetCallingSubaddress",		// external name
      Id_CParamSetCallingSubaddress,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallingSubaddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // SetCallingSignalAddress(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetCallingSignalAddress",	// external name
      Id_CParamSetCallingSignalAddress,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallingSignalAddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // SetCallingPresentation(CTask* task,CallParameter* cparam,CallCallingPresentation value);
  RegisterAPI(
      "CParamSetCallingPresentation",		// external name
      Id_CParamSetCallingPresentation,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallingPresentation,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetCallingScreening(CTask* task,CallParameter* cparam,CallCallingScreening value);
  RegisterAPI(
      "CParamSetCallingScreening",		// external name
      Id_CParamSetCallingScreening,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallingScreening,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetPrivateInfo(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetPrivateInfo",			// external name
      Id_CParamSetPrivateInfo,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetPrivateInfo,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // SetOriginalCalledNumber(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetOriginalCalledNumber",		// external name
      Id_CParamSetOriginalCalledNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetOriginalCalledNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // SetOriginalCalledNumberPlan(CTask* task,CallParameter* cparam,CallNumberPlan value);
  RegisterAPI(
      "CParamSetOriginalCalledNumberPlan",		// external name
      Id_CParamSetOriginalCalledNumberPlan,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetOriginalCalledNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetOriginalCalledNumberType(CTask* task,CallParameter* cparam,CallNumberType value);
  RegisterAPI(
      "CParamSetOriginalCalledNumberType",		// external name
      Id_CParamSetOriginalCalledNumberType,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetOriginalCalledNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetOriginalCalledNumberPresentation(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetOriginalCalledNumberPresentation",		// external name
      Id_CParamSetOriginalCalledNumberPresentation,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetOriginalCalledNumberPresentation,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetRedirectingNumber(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetRedirectingNumber",		// external name
      Id_CParamSetRedirectingNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetRedirectingNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // SetRedirectingNumberPlan(CTask* task,CallParameter* cparam,CallNumberPlan value);
  RegisterAPI(
      "CParamSetRedirectingNumberPlan",		// external name
      Id_CParamSetRedirectingNumberPlan,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetRedirectingNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetRedirectingNumberType(CTask* task,CallParameter* cparam,CallNumberType value);
  RegisterAPI(
      "CParamSetRedirectingNumberType",		// external name
      Id_CParamSetRedirectingNumberType,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetRedirectingNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetRedirectingNumber(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetRedirectingNumberPresentation",		// external name
      Id_CParamSetRedirectingNumberPresentation,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetRedirectingNumberPresentation,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetLocationNumber(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetLocationNumber",		// external name
      Id_CParamSetLocationNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetLocationNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // SetLocationNumberPlan(CTask* task,CallParameter* cparam,CallNumberPlan value);
  RegisterAPI(
      "CParamSetLocationNumberPlan",		// external name
      Id_CParamSetLocationNumberPlan,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetLocationNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetLocationNumberType(CTask* task,CallParameter* cparam,CallNumberType value);
  RegisterAPI(
      "CParamSetLocationNumberType",		// external name
      Id_CParamSetLocationNumberType,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetLocationNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // CParamSetLocationNumberPresentation(CTask* task,CallParameter* cparam,int);
  RegisterAPI(
      "CParamSetLocationNumberPresentation",		// external name
      Id_CParamSetLocationNumberPresentation,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetLocationNumberPresentation,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // CParamSetRedirectionIndicator(CTask* task,CallParameter* cparam,int);
  RegisterAPI(
      "CParamSetRedirectionIndicator",		// external name
      Id_CParamSetRedirectionIndicator,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetRedirectionIndicator,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // CParamSetRedirectionOrigReason(CTask* task,CallParameter* cparam,int);
  RegisterAPI(
      "CParamSetRedirectionOrigReason",		// external name
      Id_CParamSetRedirectionOrigReason,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetRedirectionOrigReason,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // CParamSetRedirectionCounter(CTask* task,CallParameter* cparam,int);
  RegisterAPI(
      "CParamSetRedirectionCounter",		// external name
      Id_CParamSetRedirectionCounter,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetRedirectionCounter,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // CParamSetRedirectionReason(CTask* task,CallParameter* cparam,int);
  RegisterAPI(
      "CParamSetRedirectionReason",		// external name
      Id_CParamSetRedirectionReason,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetRedirectionReason,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // CParamSetCallingMobileIdentityType(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetCallingMobileIdentityType",		// external name
      Id_CParamSetCallingMobileIdentityType,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallingMobileIdentityType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // CParamSetCallType(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetCallType",		// external name
      Id_CParamSetCallType,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // CParamSetSuspendResumeNotification(CTask* task,CallParameter* cparam,bool value);
  RegisterAPI(
      "CParamSetSuspendResumeNotification",	// external name
      Id_CParamSetSuspendResumeNotification,	// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetSuspendResumeNotification,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_BOOLEAN));

  // CParamSetHuntGroupCaller(CTask* task,CallParameter* cparam,bool value);
  RegisterAPI(
      "CParamSetHuntGroupCaller",	// external name
      Id_CParamSetHuntGroupCaller,	// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetHuntGroupCaller,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_BOOLEAN));

  // CParamSetSwitchBoardCall(CTask* task,CallParameter* cparam,bool value);
  RegisterAPI(
      "CParamSetSwitchBoardCall",	// external name
      Id_CParamSetSwitchBoardCall,	// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetSwitchBoardCall,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_BOOLEAN));

  // CParamSetSwitchBoardCallState(CTask* task,CallParameter* cparam,bool value);
  RegisterAPI(
      "CParamSetSwitchBoardCallState",	// external name
      Id_CParamSetSwitchBoardCallState,	// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetSwitchBoardCallState,
      CAPIDef::CDECL_TYPE,			// calling convention
      5,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("callstate",BYVAL,CVariant::VT_INT),
      CArgType("mindigits",BYVAL,CVariant::VT_INT),
      CArgType("timeout",BYVAL,CVariant::VT_INT));

  // CParamGetSwitchBoardCallState(CTask* task,CallParameter* cparam,int* retval);
  RegisterAPI(
      "CParamGetSwitchBoardCallState",	// external name
      Id_CParamGetSwitchBoardCallState,	// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetSwitchBoardCallState,
      CAPIDef::CDECL_TYPE,			// calling convention
      4,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("dtmf_digits",BYREF,CVariant::VT_STRING));

  // CParamSwitchBoardSuspendTask(CTask* task,CallParameter* cparam,int value);
  RegisterAPI(
      "CParamSwitchBoardSuspendTask",	// external name
      Id_CParamSwitchBoardSuspendTask,	// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SwitchBoardSuspendTask,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // CParamSetECTNotification(CTask* task,CallParameter* cparam,bool value);
  RegisterAPI(
      "CParamSetECTNotification",		// external name
      Id_CParamSetECTNotification,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetECTNotification,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_BOOLEAN));

  // CParamSetCallWaitingFlag(CTask* task,CallParameter* cparam,bool value);
  RegisterAPI(
      "CParamSetCallWaitingFlag",		// external name
      Id_CParamSetCallWaitingFlag,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallWaitingFlag,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_BOOLEAN));

  // GetSessionId(CTask* task,char* retval,CallParameter* cparam)
  RegisterAPI(
      "CParamGetSessionId",			// external name
      Id_CParamGetSessionId,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetSessionId,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetConnectionType(CTask* task,CallConnectionType* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetConnectionType",		// external name
      Id_CParamGetConnectionType,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetConnectionType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCompandingLaw(CTask* task,CallCompandingLaw* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCompandingLaw",		// external name
      Id_CParamGetCompandingLaw,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCompandingLaw,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCalledPartyCategory(CTask* task,CallPartyCategory* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCalledPartyCategory",		// external name
      Id_CParamGetCalledPartyCategory,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCalledPartyCategory,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCalledNumber(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCalledNumber",		// external name
      Id_CParamGetCalledNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCalledNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCalledNumberPlan(CTask* task,CallNumberPlan* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCalledNumberPlan",		// external name
      Id_CParamGetCalledNumberPlan,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCalledNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCalledNumberType(CTask* task,CallNumberType* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCalledNumberType",		// external name
      Id_CParamGetCalledNumberType,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCalledNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCalledSubaddress(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCalledSubaddress",		// external name
      Id_CParamGetCalledSubaddress,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCalledSubaddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCalledSignalAddress(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCalledSignalAddress",		// external name
      Id_CParamGetCalledSignalAddress,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCalledSignalAddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCalledComplete(CTask* task,CallComplete* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCalledComplete",		// external name
      Id_CParamGetCalledComplete,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCalledComplete,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetUserInfo(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetUserInfo",			// external name
      Id_CParamGetUserInfo,				// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetUserInfo,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCallingPartyCategory(CTask* task,CallPartyCategory* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCallingPartyCategory",	// external name
      Id_CParamGetCallingPartyCategory,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCallingPartyCategory,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCallingNumber(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCallingNumber",		// external name
      Id_CParamGetCallingNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCallingNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCallingNumberPlan(CTask* task,CallNumberPlan* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCallingNumberPlan",		// external name
      Id_CParamGetCallingNumberPlan,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCallingNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCallingNumberType(CTask* task,CallNumberType* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCallingNumberType",		// external name
      Id_CParamGetCallingNumberType,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCallingNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCallingSubaddress(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCallingSubaddress",		// external name
      Id_CParamGetCallingSubaddress,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCallingSubaddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCallingSignalAddress(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCallingSignalAddress",	// external name
      Id_CParamGetCallingSignalAddress,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCallingSignalAddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCallingPresentation(CTask* task,CallCallingPresentation* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCallingPresentation",		// external name
      Id_CParamGetCallingPresentation,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCallingPresentation,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetCallingScreening(CTask* task,CallCallingScreening* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCallingScreening",		// external name
      Id_CParamGetCallingScreening,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCallingScreening,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetPrivateInfo(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetPrivateInfo",			// external name
      Id_CParamGetPrivateInfo,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetPrivateInfo,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetRedirectingNumber(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetRedirectingNumber",		// external name
      Id_CParamGetRedirectingNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetRedirectingNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

#ifdef CALL_TRANSFER
  /*
  // GetRedirectionAddress(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
  "CParamGetRedirectionAddress",           // external name
  Id_CParamGetRedirectionAddress,                  // ordinal
  false,                                  // asynchronous flag
  CVariant::VT_STRING,                    // return type
  (CAPIProvider::APIPROC) &CallAPI::GetRedirectionAddress,
  CAPIDef::CDECL_TYPE,                    // calling convention
  3,                                      // number of arguments
  CArgType("task",BYVAL,CVariant::VT_TASK),
  CArgType("retval",BYREF,CVariant::VT_RET),
  CArgType("cparam",BYVAL,CVariant::VT_INT)); */
#endif

  // GetRedirectingNumberPlan(CTask* task,CallNumberPlan* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetRedirectingNumberPlan",		// external name
      Id_CParamGetRedirectingNumberPlan,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetRedirectingNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetRedirectingNumberType(CTask* task,CallNumberType* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetRedirectingNumberType",		// external name
      Id_CParamGetRedirectingNumberType,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetRedirectingNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetRedirectingNumberPresentation(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetRedirectingNumberPresentation",		// external name
      Id_CParamGetRedirectingNumberPresentation,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetRedirectingNumberPresentation,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetLocationNumber(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetLocationNumber",		// external name
      Id_CParamGetLocationNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetLocationNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetLocationNumberPlan(CTask* task,CallNumberPlan* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetLocationNumberPlan",		// external name
      Id_CParamGetLocationNumberPlan,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetLocationNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetLocationNumberType(CTask* task,CallNumberType* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetLocationNumberType",		// external name
      Id_CParamGetLocationNumberType,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetLocationNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetLocationNumberPresentation(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetLocationNumberPresentation",		// external name
      Id_CParamGetLocationNumberPresentation,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetLocationNumberPresentation,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // CParamGetRedirectionIndicator(CTask* task,int* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetRedirectionIndicator",		// external name
      Id_CParamGetRedirectionIndicator,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetRedirectionIndicator,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // CParamGetRedirectionOrigReason(CTask* task,int* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetRedirectionOrigReason",		// external name
      Id_CParamGetRedirectionOrigReason,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetRedirectionOrigReason,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // CParamGetRedirectionCounter(CTask* task,int* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetRedirectionCounter",		// external name
      Id_CParamGetRedirectionCounter,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetRedirectionCounter,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // CParamGetRedirectionReason(CTask* task,int* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetRedirectionReason",		// external name
      Id_CParamGetRedirectionReason,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetRedirectionReason,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetOriginalCalledNumber(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetOriginalCalledNumber",		// external name
      Id_CParamGetOriginalCalledNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetOriginalCalledNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetOriginalCalledNumberPlan(CTask* task,CallNumberPlan* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetOriginalCalledNumberPlan",		// external name
      Id_CParamGetOriginalCalledNumberPlan,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetOriginalCalledNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetOriginalCalledNumberType(CTask* task,CallNumberType* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetOriginalCalledNumberType",		// external name
      Id_CParamGetOriginalCalledNumberType,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetOriginalCalledNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // GetOriginalCalledNumberPresentation(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetOriginalCalledNumberPresentation",		// external name
      Id_CParamGetOriginalCalledNumberPresentation,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetOriginalCalledNumberPresentation,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // CParamGetCallingMobileIdentityType(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCallingMobileIdentityType",		// external name
      Id_CParamGetCallingMobileIdentityType,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCallingMobileIdentityType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // CParamGetCallType(CTask* task,char* retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCallType",		// external name
      Id_CParamGetCallType,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCallType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // CParamGetSuspendResumeNotification(CTask* task,bool retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetSuspendResumeNotification",	// external name
      Id_CParamGetSuspendResumeNotification,	// ordinal
      false,					// asynchronous flag
      CVariant::VT_BOOLEAN,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetSuspendResumeNotification,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // CParamGetSuspendResumeNotification(CTask* task,bool retval,CallParameter* cparam);
  RegisterAPI(
      "CParamGetCallWaitingFlag",		// external name
      Id_CParamGetCallWaitingFlag,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_BOOLEAN,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCallWaitingFlag,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // ExSetDisconnectMsg(CTask* task,int handle,const char* msg);
  RegisterAPI(
      "CallExSetDisconnectMsg",		// external name
      Id_ExSetDisconnectMsg,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::ExSetDisconnectMsg,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("msg",BYVAL,CVariant::VT_STRING));

  // Setup(CTask* task,int* retcode,const char* itfName,CallParameter* cparam,int* handle)
  RegisterAPI(
      "CallSetup",				// external name
      Id_Setup,				// ordinal
      true,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::Setup,
      CAPIDef::CDECL_TYPE,			// calling convention
      5,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("interface",BYVAL,CVariant::VT_STRING),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("handle",BYREF,CVariant::VT_INT));

  // Setup_callque(CTask* task,int* retcode,const char* itfName,CallParameter* cparam,int* handle)
  RegisterAPI(
      "CallSetup_callque",				// external name
      Id_Setup_callque,				// ordinal
      true,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::Setup_callque,
      CAPIDef::CDECL_TYPE,			// calling convention
      5,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("interface",BYVAL,CVariant::VT_STRING),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("handle",BYREF,CVariant::VT_INT));

  // SendOverlap(CTask* task,int* retcode,int handle,const char* additionalDigits,bool complete);
  RegisterAPI(
      "CallSendOverlap",			// external name
      Id_SendOverlap,				// ordinal
      true,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SendOverlap,
      CAPIDef::CDECL_TYPE,			// calling convention
      5,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("digits",BYVAL,CVariant::VT_STRING),
      CArgType("complete",BYVAL,CVariant::VT_INT));

  // GetMoreDigits(CTask* task,char* additionalDigits,int handle,int timeout,bool* complete);
  RegisterAPI(
      "CallGetMoreDigits",			// external name
      Id_GetMoreDigits,			// ordinal
      true,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetMoreDigits,
      CAPIDef::CDECL_TYPE,			// calling convention
      5,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("digits",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("timeout",BYVAL,CVariant::VT_INT),
      CArgType("complete",BYREF,CVariant::VT_INT));

  // GetMoreDigitsEx(CTask* task,char* additionalDigits,int handle,int mindigits,int timeout,int* complete);
  RegisterAPI(
      "CallGetMoreDigitsEx",			// external name
      Id_GetMoreDigitsEx,			// ordinal
      true,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetMoreDigitsEx,
      CAPIDef::CDECL_TYPE,			// calling convention
      6,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("digits",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("mindigits",BYVAL,CVariant::VT_INT),
      CArgType("timeout",BYVAL,CVariant::VT_INT),
      CArgType("complete",BYREF,CVariant::VT_INT));

  // SetCharging(CTask* task,int handle,int charging)
  RegisterAPI(
      "CallSetCharging",			// external name
      Id_SetCharging,				// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCharging,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("charging",BYVAL,CVariant::VT_INT));

  // SetSuspendResumeNotification(CTask* task,int handle,bool value)
  RegisterAPI(
      "CallSetSuspendResumeNotification",	// external name
      Id_SetSuspendResumeNotification,	// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallSetSuspendResumeNotification,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_BOOLEAN));

  // SetECTNotification(CTask* task,int handle,bool value)
  RegisterAPI(
      "CallSetECTNotification",		// external name
      Id_SetECTNotification,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallSetECTNotification,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_BOOLEAN));

  // Proceeding(CTask* task,int* retcode,int handle,CallProgress progress);
  RegisterAPI(
      "CallProceeding",			// external name
      Id_Proceeding,				// ordinal
      true,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::Proceeding,
      CAPIDef::CDECL_TYPE,			// calling convention
      4,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("progress",BYVAL,CVariant::VT_INT));

  // Progress(CTask* task,int* retcode,int handle,CallProgress progress);
  RegisterAPI(
      "CallProgress",				// external name
      Id_Progress,				// ordinal
      true,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::Progress,
      CAPIDef::CDECL_TYPE,			// calling convention
      4,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("progress",BYVAL,CVariant::VT_INT));

  // Alerting(CTask* task,int* retcode,int handle);
  RegisterAPI(
      "CallAlerting",				// external name
      Id_Alerting,				// ordinal
      true,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::Alerting,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // Answer(CTask* task,int* retcode,int handle);
  RegisterAPI(
      "CallAnswer",				// external name
      Id_Answer,				// ordinal
      true,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::Answer,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // Answer(CTask* task,int* retcode,int handle);
  RegisterAPI(
      "CallSendInfo",                         // external name
      Id_CallSendInfo,                                // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_INT,                       // return type
      (CAPIProvider::APIPROC) &CallAPI::SendInfo,
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // Suspend(CTask* task,int* retcode,int handle);
  RegisterAPI(
      "CallSuspend",				// external name
      Id_Suspend,				// ordinal
      true,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::Suspend,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // Resume(CTask* task,int* retcode,int handle);
  RegisterAPI(
      "CallResume",				// external name
      Id_Resume,				// ordinal
      true,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::Resume,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // Disconnect(CTask* task,int* retcode,int handle,CallCause cause);
  RegisterAPI(
      "CallDisconnect",			// external name
      Id_Disconnect,				// ordinal
      true,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::Disconnect,
      CAPIDef::CDECL_TYPE,			// calling convention
      4,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("cause",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "Reg_IpInfo",           // external name
      Id_Reg_IpInfo,              // ordinal
      true,                   // asynchronous flag
      CVariant::VT_INT,           // return type
      (CAPIProvider::APIPROC) &CallAPI::RegIpInfo,
      CAPIDef::CDECL_TYPE,            // calling convention
      5,                  // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("ip_usrA",BYVAL,CVariant::VT_STRING),
      CArgType("ip_usrB",BYVAL,CVariant::VT_STRING),
      CArgType("pid",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "RemoveFileApi",           // external name
      Id_RemoveFile,              // ordinal
      true,                   // asynchronous flag
      CVariant::VT_VOID,           // return type
      (CAPIProvider::APIPROC) &CallAPI::RemoveFile,
      CAPIDef::CDECL_TYPE,            // calling convention
      2,                  // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("filename",BYVAL,CVariant::VT_STRING));


  // NotifyECT(CTask* task,int* retcode,int handle, tect_indicator indicator);
  RegisterAPI(
      "CallNotifyECT",			// external name
      Id_NotifyECT,				// ordinal
      true,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::NotifyECT,
      CAPIDef::CDECL_TYPE,			// calling convention
      4,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("indicator",BYREF,CVariant::VT_USRTYPE, RECORD_ECT_INDICATOR));

  // GetCause(CTask* task,CallCause* cause,int handle);
  RegisterAPI(
      "CallGetCause",				// external name
      Id_GetCause,				// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetCause,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cause",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // SignallingConnect(CTask* task,int* retcode,int handle1,int handle2);
  RegisterAPI(
      "CallSignallingConnect",		// external name
      Id_SignallingConnect,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SignallingConnect,
      CAPIDef::CDECL_TYPE,			// calling convention
      4,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retcode",BYREF,CVariant::VT_RET),
      CArgType("handle1",BYVAL,CVariant::VT_INT),
      CArgType("handle2",BYVAL,CVariant::VT_INT));

  // SignallingDisconnect(CTask* task,int* retcode,int handle1,int handle2);
  RegisterAPI(
      "CallSignallingDisconnect",		// external name
      Id_SignallingDisconnect,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SignallingDisconnect,
      CAPIDef::CDECL_TYPE,			// calling convention
      4,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retcode",BYREF,CVariant::VT_RET),
      CArgType("handle1",BYVAL,CVariant::VT_INT),
      CArgType("handle2",BYVAL,CVariant::VT_INT));

  // GetDirection(CTask* task,CallDirection* direction,int handle);
  RegisterAPI(
      "CallGetDirection",			// external name
      Id_GetDirection,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetDirection,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("direction",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // GetState(CTask* task,CallState* state,int handle);
  RegisterAPI(
      "CallGetState",				// external name
      Id_GetState,				// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetState,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("state",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // GetFirstCallHandle(CTask* task,int* handle);
  // GetNextCallHandle(CTask* task,int* handle);

  // GetMedia(CTask* task,Media **media,int handle);
  RegisterAPI(
      "CallGetMedia",				// external name
      Id_GetMedia,				// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetMedia,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("media",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // GetMediaName(CTask* task,int* retcode,int handle,char* group,char* name);
  RegisterAPI(
      "CallGetMediaName",			// external name
      Id_GetMediaName,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetMediaName,
      CAPIDef::CDECL_TYPE,			// calling convention
      5,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("group",BYREF,CVariant::VT_STRING),
      CArgType("name",BYREF,CVariant::VT_STRING));

  // GetInterfaceName(CTask* task,char* retval,int handle);
  RegisterAPI(
      "CallGetInterfaceName",			// external name
      Id_GetInterfaceName,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetInterfaceName,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // GetGroupName(CTask* task,char* retval,int handle);
  RegisterAPI(
      "CallGetGroupName",			// external name
      Id_GetGroupName,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetGroupName,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetSessionId
  RegisterAPI(
      "CallGetSessionId",			// external name
      Id_GetSessionId,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetSessionId,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetConnectionType
  RegisterAPI(
      "CallGetConnectionType",		// external name
      Id_GetConnectionType,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetConnectionType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCompandingLaw
  RegisterAPI(
      "CallGetCompandingLaw",			// external name
      Id_GetCompandingLaw,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCompandingLaw,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCalledPartyCategory
  RegisterAPI(
      "CallGetCalledPartyCategory",		// external name
      Id_GetCalledPartyCategory,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCalledPartyCategory,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCalledNumber
  RegisterAPI(
      "CallGetCalledNumber",			// external name
      Id_GetCalledNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCalledNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCalledNumberPlan
  RegisterAPI(
      "CallGetCalledNumberPlan",		// external name
      Id_GetCalledNumberPlan,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCalledNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCalledNumberType
  RegisterAPI(
      "CallGetCalledNumberType",		// external name
      Id_GetCalledNumberType,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCalledNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCalledSubaddress
  RegisterAPI(
      "CallGetCalledSubaddress",		// external name
      Id_GetCalledSubaddress,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCalledSubaddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCalledSignalAddress
  RegisterAPI(
      "CallGetCalledSignalAddress",		// external name
      Id_GetCalledSignalAddress,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCalledSignalAddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

#ifdef XGATE_HAS_ACME_SBC
  RegisterAPI(
      "CParamSetRouteSignalAddress",		// external name
      Id_CParamSetRouteSignalAddress,
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetRouteSignalAddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));
  RegisterAPI(
      "CParamGetRouteSignalAddress",		// external name
      Id_CParamGetRouteSignalAddress,
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetRouteSignalAddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // SetProxyUserName(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetProxyUserName",		// external name
      Id_CParamSetProxyUserName,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetProxyUserName,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // CallGetProxyUserName
  RegisterAPI(
      "CParamGetProxyUserName",			// external name
      Id_CParamGetProxyUserName,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetProxyUserName,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      ////CArgType("handle",BYVAL,CVariant::VT_INT));
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  //CParamSetCSFilename
  RegisterAPI(
      "CParamSetCSFilename",         // external name
      Id_CParamSetCSFilename,                        // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetCSFilename,
      CAPIDef::CDECL_TYPE,                      // calling convention
      3,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));  

  //CParamSetCallForwardingFilename
  RegisterAPI(
      "CParamSetCallForwardingFilename",         // external name
      Id_CParamSetCallForwardingFilename,                        // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallForwardingFilename,
      CAPIDef::CDECL_TYPE,                      // calling convention
      3,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  //CParamSetVoicemailRecordingFilename
  RegisterAPI(
      "CParamSetVoicemailRecordingFilename",         // external name
      Id_CParamSetVoicemailRecordingFilename,                        // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetVoicemailRecordingFilename,
      CAPIDef::CDECL_TYPE,                      // calling convention
      3,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  RegisterAPI(
      "CParamSetRbtFile",         // external name
      Id_CParamSetRbtFile,                        // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetRbtFile,
      CAPIDef::CDECL_TYPE,                      // calling convention
      3,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));  

  RegisterAPI(
      "CParamSetCalledDeviceType",         // external name
      Id_CParamSetCalledDeviceType,                        // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetCalledDeviceType,
      CAPIDef::CDECL_TYPE,                      // calling convention
      3,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  // SetCallRecording(CTask* task,CallParameter* cparam,SetCallRecording value);
  RegisterAPI(
      "CParamSetCallRecording",           // external name
      Id_CParamSetCallRecording,          // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallRecording,
      CAPIDef::CDECL_TYPE,                      // calling convention
      3,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  //CParamSetCallOnHoldFilenameCallingParty
  RegisterAPI(
      "CParamSetCallOnHoldFilenameCallingParty",         // external name
      Id_CParamSetCallOnHoldFilenameCallingParty,                        // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallOnHoldFilenameCallingParty,
      CAPIDef::CDECL_TYPE,                      // calling convention
      3,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  //CParamSetCallOnHoldFilenameCalledParty
  RegisterAPI(
      "CParamSetCallOnHoldFilenameCalledParty",         // external name
      Id_CParamSetCallOnHoldFilenameCalledParty,                        // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallOnHoldFilenameCalledParty,
      CAPIDef::CDECL_TYPE,                      // calling convention
      3,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_STRING));

  RegisterAPI(
      "SendAppNotification",         // external name
      Id_SendAppNotification,                // ordinal
      false,                                  // asynchronous flag
      CVariant::VT_VOID,                    // return type
      (CAPIProvider::APIPROC)&CallAPI::SendAppNotification,
      CAPIDef::CDECL_TYPE,                    // calling convention
      6,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("from",BYVAL,CVariant::VT_STRING),
      CArgType("to",BYVAL,CVariant::VT_STRING),
      CArgType("user_cli",BYVAL,CVariant::VT_STRING),
      CArgType("device_type",BYVAL,CVariant::VT_STRING),
      CArgType("call_type",BYVAL,CVariant::VT_STRING));

  RegisterAPI(
      "CParamSetIsForkingEnabled",         // external name
      Id_CParamSetIsForkingEnabled,                        // ordinal
      false,                                    // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetIsForkingEnabled,
      CAPIDef::CDECL_TYPE,                      // calling convention
      3,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // SetDomainName(CTask* task,CallParameter* cparam,char* value);
  /*RegisterAPI(
    "CParamSetDomainName",		// external name
    Id_CParamSetDomainName,			// ordinal
    false,					// asynchronous flag
    CVariant::VT_VOID,			// return type
    (CAPIProvider::APIPROC) &CallAPI::SetDomainName,
    CAPIDef::CDECL_TYPE,			// calling convention
    3,					// number of arguments
    CArgType("task",BYVAL,CVariant::VT_TASK),
    CArgType("cparam",BYVAL,CVariant::VT_INT),
    CArgType("value",BYVAL,CVariant::VT_STRING));

  // CallGetDomainName
  RegisterAPI(
  "CParamGetDomainName",			// external name
  Id_CParamGetDomainName,			// ordinal
  false,					// asynchronous flag
  CVariant::VT_STRING,			// return type
  (CAPIProvider::APIPROC) &CallAPI::CallGetDomainName,
  CAPIDef::CDECL_TYPE,			// calling convention
  3,					// number of arguments
  CArgType("task",BYVAL,CVariant::VT_TASK),
  CArgType("retval",BYREF,CVariant::VT_RET),
  CArgType("handle",BYVAL,CVariant::VT_INT));*/
#endif


  // CallGetCalledComplete
  RegisterAPI(
      "CallGetCalledComplete",		// external name
      Id_GetCalledComplete,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCalledComplete,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetUserInfo
  RegisterAPI(
      "CallGetUserInfo",			// external name
      Id_GetUserInfo,				// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetUserInfo,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCallingPartyCategory
  RegisterAPI(
      "CallGetCallingPartyCategory",		// external name
      Id_GetCallingPartyCategory,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCallingPartyCategory,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCallingNumber
  RegisterAPI(
      "CallGetCallingNumber",			// external name
      Id_GetCallingNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCallingNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCallingNumberPlan
  RegisterAPI(
      "CallGetCallingNumberPlan",		// external name
      Id_GetCallingNumberPlan,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCallingNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCallingNumberType
  RegisterAPI(
      "CallGetCallingNumberType",		// external name
      Id_GetCallingNumberType,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCallingNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCallingSubaddress
  RegisterAPI(
      "CallGetCallingSubaddress",		// external name
      Id_GetCallingSubaddress,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCallingSubaddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCallingSignalAddress
  RegisterAPI(
      "CallGetCallingSignalAddress",		// external name
      Id_GetCallingSignalAddress,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCallingSignalAddress,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCallingPresentation
  RegisterAPI(
      "CallGetCallingPresentation",		// external name
      Id_GetCallingPresentation,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCallingPresentation,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCallingScreening
  RegisterAPI(
      "CallGetCallingScreening",		// external name
      Id_GetCallingScreening,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCallingScreening,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetPrivateInfo
  RegisterAPI(
      "CallGetPrivateInfo",			// external name
      Id_GetPrivateInfo,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetPrivateInfo,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetPrivateInfo
  RegisterAPI(
      "CallGetQuality",			// external name
      Id_GetQuality,				// ordinal
      false,					// asynchronous flag
      CVariant::VT_FLOAT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetQuality,
      CAPIDef::CDECL_TYPE,			// calling convention
      5,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("value",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("stream",BYVAL,CVariant::VT_INT),
      CArgType("name",BYVAL,CVariant::VT_STRING)
      );

  //STATUS API_CALL CallGetAuthorization(CTask* task, int* auth_handle, int call_handle);
  RegisterAPI(
      "CallGetAuthorization",			// external name
      Id_CallGetAuthorization,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetAuthorization,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("ret",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT)
      );

  // SetCallCredential(CTask* task, CallParameter* cparam, const char *username, const char*password);
  RegisterAPI(
      "CParamSetCredential",		// external name
      Id_CParamSetCredential,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetCallCredential,
      CAPIDef::CDECL_TYPE,			// calling convention
      4,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("username",BYVAL,CVariant::VT_STRING),
      CArgType("password",BYVAL,CVariant::VT_STRING));

  // CallGetRedirectingNumber
  RegisterAPI(
      "CallGetRedirectingNumber",			// external name
      Id_GetRedirectingNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetRedirectingNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetRedirectingNumberPlan
  RegisterAPI(
      "CallGetRedirectingNumberPlan",		// external name
      Id_GetRedirectingNumberPlan,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetRedirectingNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetRedirectingNumberType
  RegisterAPI(
      "CallGetRedirectingNumberType",		// external name
      Id_GetRedirectingNumberType,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetRedirectingNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetRedirectingNumberPresentation
  RegisterAPI(
      "CallGetRedirectingNumberPresentation",			// external name
      Id_GetRedirectingNumberPresentation,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetRedirectingNumberPresentation,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetLocationNumber
  RegisterAPI(
      "CallGetLocationNumber",			// external name
      Id_GetLocationNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetLocationNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetLocationNumberPlan
  RegisterAPI(
      "CallGetLocationNumberPlan",		// external name
      Id_GetLocationNumberPlan,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetLocationNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetLocationNumberType
  RegisterAPI(
      "CallGetLocationNumberType",		// external name
      Id_GetLocationNumberType,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetLocationNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetLocationNumberPresentation
  RegisterAPI(
      "CallGetLocationNumberPresentation",			// external name
      Id_GetLocationNumberPresentation,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetLocationNumberPresentation,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetRedirectionIndicator
  RegisterAPI(
      "CallGetRedirectionIndicator",			// external name
      Id_GetRedirectionIndicator,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetRedirectionIndicator,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetRedirectionOrigReason
  RegisterAPI(
      "CallGetRedirectionOrigReason",			// external name
      Id_GetRedirectionOrigReason,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetRedirectionOrigReason,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetRedirectionCounter
  RegisterAPI(
      "CallGetRedirectionCounter",			// external name
      Id_GetRedirectionCounter,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetRedirectionCounter,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetRedirectionReason
  RegisterAPI(
      "CallGetRedirectionReason",			// external name
      Id_GetRedirectionReason,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetRedirectionReason,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetOriginalCalledNumber
  RegisterAPI(
      "CallGetOriginalCalledNumber",			// external name
      Id_GetOriginalCalledNumber,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_STRING,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetOriginalCalledNumber,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetOriginalCalledNumberPlan
  RegisterAPI(
      "CallGetOriginalCalledNumberPlan",		// external name
      Id_GetOriginalCalledNumberPlan,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetOriginalCalledNumberPlan,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetOriginalCalledNumberType
  RegisterAPI(
      "CallGetOriginalCalledNumberType",		// external name
      Id_GetOriginalCalledNumberType,		// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetOriginalCalledNumberType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetOriginalCalledNumberPresentation
  RegisterAPI(
      "CallGetOriginalCalledNumberPresentation",			// external name
      Id_GetOriginalCalledNumberPresentation,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetOriginalCalledNumberPresentation,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCallingMobileIdentityType
  RegisterAPI(
      "CallGetCallingMobileIdentityType",			// external name
      Id_GetCallingMobileIdentityType,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCallingMobileIdentityType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCallType
  RegisterAPI(
      "CallGetCallType",			// external name
      Id_GetCallType,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCallType,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetSuspendResumeNotification
  RegisterAPI(
      "CallGetSuspendResumeNotification",	// external name
      Id_GetSuspendResumeNotification,	// ordinal
      false,					// asynchronous flag
      CVariant::VT_BOOLEAN,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetSuspendResumeNotification,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetCallWaitingFlag
  RegisterAPI(
      "CallGetCallWaitingFlag",		// external name
      Id_GetCallWaitingFlag,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_BOOLEAN,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetCallWaitingFlag,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CallGetBssapDH
  RegisterAPI(
      "CallGetBssapDH",			// external name
      Id_GetBssapDH,				// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallGetBssapDH,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  // CParamSetBssapDH(CTask* task,CallParameter* cparam,char* value);
  RegisterAPI(
      "CParamSetBssapDH",		// external name
      Id_CParamSetBssapDH,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_VOID,			// return type
      (CAPIProvider::APIPROC) &CallAPI::SetBssapDH,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("value",BYVAL,CVariant::VT_INT));

  // CParamGetBssapDH(CTask* task,int* retval,CallParameter* cparam)
  RegisterAPI(
      "CParamGetBssapDH",			// external name
      Id_CParamGetBssapDH,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::GetBssapDH,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("cparam",BYVAL,CVariant::VT_INT));

  // CallReleaseMedia
  RegisterAPI(
      "CallReleaseMedia",			// external name
      Id_ReleaseMedia,			// ordinal
      false,					// asynchronous flag
      CVariant::VT_INT,			// return type
      (CAPIProvider::APIPROC) &CallAPI::CallReleaseMedia,
      CAPIDef::CDECL_TYPE,			// calling convention
      3,					// number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("handle",BYVAL,CVariant::VT_INT));

#ifdef XGATE_HAS_HMP

#if 0
  RegisterAPI(
      "HmpGetDtmfDigits",                       // external name
      Id_HmpGetDtmfDigits,                      // ordinal
      true,                                     // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpGetDtmfDigits,
      CAPIDef::CDECL_TYPE,                      // calling convention
      5,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("number_of_digits",BYVAL,CVariant::VT_INT),
      CArgType("wait_time",BYVAL,CVariant::VT_INT),
      CArgType("dtmf_digits",BYREF,CVariant::VT_STRING));

#else

  RegisterAPI(
      "HmpGetDtmfDigits",                       // external name
      Id_HmpGetDtmfDigits,                      // ordinal
      true,                                     // asynchronous flag
      CVariant::VT_INT,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpGetDtmfDigits,
      CAPIDef::CDECL_TYPE,                      // calling convention
      5,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("dtmf_digits",BYREF,CVariant::VT_STRING),
      CArgType("mindigits",BYVAL,CVariant::VT_INT));

#endif

  RegisterAPI(
      "HmpPlayFile",                            // external name
      Id_HmpPlayFile,                           // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpPlayFile,
      CAPIDef::CDECL_TYPE,                    // calling convention
      4,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("file_name",BYVAL,CVariant::VT_STRING),
      CArgType("autorepeat",BYVAL,CVariant::VT_BOOLEAN));

  RegisterAPI(
      "HmpSwbPlayFile",                            // external name
      Id_HmpSwbPlayFile,                           // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpSwbPlayFile,
      CAPIDef::CDECL_TYPE,                    // calling convention
      5,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      ////CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("cparam",BYVAL,CVariant::VT_INT),
      CArgType("file_name",BYVAL,CVariant::VT_STRING),
      CArgType("autorepeat",BYVAL,CVariant::VT_BOOLEAN),
      CArgType("callstate",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "HmpStopPlayFile",                            // external name
      Id_HmpStopPlayFile,                           // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpStopPlayFile,
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("waitTime",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "HmpPlayFileBkgnd",                            // external name
      Id_HmpPlayFileBkgnd,                           // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpPlayFileBkgnd,
      CAPIDef::CDECL_TYPE,                    // calling convention
      4,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("file_name",BYVAL,CVariant::VT_STRING),
      CArgType("autorepeat",BYVAL,CVariant::VT_BOOLEAN));

  RegisterAPI(
      "HmpStopPlayFileBkgnd",                            // external name
      Id_HmpStopPlayFileBkgnd,                           // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpStopPlayFileBkgnd,
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("waitTime",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "HmpGetDigits",                   // external name
      Id_HmpGetDigits,                  // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpGetDigits,
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("max_length",BYVAL,CVariant::VT_INT),
      CArgType("buffer",BYREF,CVariant::VT_STRING));

  RegisterAPI(
      "HmpClearDigits",                         // external name
      Id_HmpClearDigits,                        // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpClearDigits,
      CAPIDef::CDECL_TYPE,                    // calling convention
      1,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK));

  RegisterAPI(
      "HmpGetDigitTimeout",                     // external name
      Id_HmpGetDigitTimeout,                    // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_INT,                         // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpGetDigitTimeout,
      CAPIDef::CDECL_TYPE,                    // calling convention
      2,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("max_timeout",BYREF,CVariant::VT_RET));

  RegisterAPI(
      "HmpSetDigitTimeout",                     // external name
      Id_HmpSetDigitTimeout,                    // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpSetDigitTimeout,
      CAPIDef::CDECL_TYPE,                    // calling convention
      3,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("max_timeout",BYVAL,CVariant::VT_INT));



  RegisterAPI(
      "HmpRecordFile",                            // external name
      Id_HmpRecordFile,                           // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpRecordFile,
      CAPIDef::CDECL_TYPE,                    // calling convention
      4,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle1",BYVAL,CVariant::VT_INT),
      CArgType("handle2",BYVAL,CVariant::VT_INT),
      CArgType("filename",BYVAL,CVariant::VT_STRING));

  RegisterAPI(
      "HmpEndRecordFile",                            // external name
      Id_HmpEndRecordFile,                           // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpEndRecordFile,
      CAPIDef::CDECL_TYPE,                    // calling convention
      2,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "HmpConfCreateRoom",                       // external name
      Id_HmpConfCreateRoom,                      // ordinal
      true,                                     // asynchronous flag
      CVariant::VT_INT,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpConfCreateRoom,
      CAPIDef::CDECL_TYPE,                      // calling convention
      5,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("room_handle",BYREF,CVariant::VT_RET),
      CArgType("room_name",BYVAL,CVariant::VT_STRING),
      CArgType("room_size",BYVAL,CVariant::VT_INT),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "HmpConfDeleteRoom",                       // external name
      Id_HmpConfDeleteRoom,                      // ordinal
      true,                                     // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpConfDeleteRoom,
      CAPIDef::CDECL_TYPE,                      // calling convention
      3,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("room_name",BYVAL,CVariant::VT_STRING),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  //STATUS API_CALL CallAPI::HmpConfRemoveParty(CTask* task, char* room_name, int handle);
  RegisterAPI(
      "HmpConfRemoveParty",                       // external name
      Id_HmpConfRemoveParty,                      // ordinal
      true,                                     // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpConfRemoveParty,
      CAPIDef::CDECL_TYPE,                      // calling convention
      3,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("room_name",BYVAL,CVariant::VT_STRING),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "HmpConfRecordFile",                       // external name
      Id_HmpConfRecordFile,                      // ordinal
      true,                                     // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpConfRecordFile,
      CAPIDef::CDECL_TYPE,                      // calling convention
      4,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("room_name",BYVAL,CVariant::VT_STRING),
      CArgType("handle",BYVAL,CVariant::VT_INT),
      CArgType("file_path",BYVAL,CVariant::VT_STRING));

  RegisterAPI(
      "HmpConfEndRecordFile",                       // external name
      Id_HmpConfEndRecordFile,                      // ordinal
      true,                                     // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpConfEndRecordFile,
      CAPIDef::CDECL_TYPE,                      // calling convention
      3,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("room_name",BYVAL,CVariant::VT_STRING),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "HmpConfFindRoom",                       // external name
      Id_HmpConfFindRoom,                      // ordinal
      true,                                     // asynchronous flag
      CVariant::VT_INT,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpConfFindRoom,
      CAPIDef::CDECL_TYPE,                      // calling convention
      3,                                        // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_INT),
      CArgType("room_name",BYVAL,CVariant::VT_STRING));

  RegisterAPI("CreateFilePath",Id_CreateFilePath,false,
      CVariant::VT_INT,
      (CAPIProvider::APIPROC) &CallAPI::CreateFilePath,
      CAPIDef::CDECL_TYPE,
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("filepath",BYVAL,CVariant::VT_STRING));

  RegisterAPI("SetHostInfo",Id_SetHostInfo,false,
      CVariant::VT_VOID,
      (CAPIProvider::APIPROC) &CallAPI::SetHostInfo,
      CAPIDef::CDECL_TYPE,
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("conf_id",BYVAL,CVariant::VT_STRING),
      CArgType("status",BYVAL,CVariant::VT_INT));

  RegisterAPI("GetHostInfo",Id_GetHostInfo,false,
      CVariant::VT_INT,
      (CAPIProvider::APIPROC) &CallAPI::GetHostInfo,
      CAPIDef::CDECL_TYPE,
      3,
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("retval",BYREF,CVariant::VT_RET),
      CArgType("conf_id",BYVAL,CVariant::VT_STRING));




  RegisterAPI(
      "HmpAttachTask",                            // external name
      Id_HmpAttachTask,                           // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::HmpAttachTask,
      CAPIDef::CDECL_TYPE,                    // calling convention
      2,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT));

  RegisterAPI(
      "SetConferenceFlag",                            // external name
      Id_SetConferenceFlag,                           // ordinal
      true,                                   // asynchronous flag
      CVariant::VT_VOID,                        // return type
      (CAPIProvider::APIPROC) &CallAPI::SetConferenceFlag,
      CAPIDef::CDECL_TYPE,                    // calling convention
      2,                                      // number of arguments
      CArgType("task",BYVAL,CVariant::VT_TASK),
      CArgType("handle",BYVAL,CVariant::VT_INT));


#endif	


}

CallAPI::~CallAPI()
{
}

/** Interfaces exposed to Kernel and user applications
*/
STATUS CallAPI::GetCallParam(CTask* task,CallParameter **cparam,int handle)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallGetCallParam");
  if (call) {
    TaskCallBlock* tcb = _manager->GetTaskCallBlock(task);
    tcb->_cparam = *call->GetCallParameter();
    *cparam = &tcb->_cparam;
    call->_overlapRcvd = "";
  } else {
    /* *cparam = (CallParameter *)CRC_InvalidCallHandle; */
    /* Linux cannot handle negative values as address. 
     * CRC_InvalidCallHandle is -4 and translated as 0xfffffffc 
     * which results in xgate crash. 
     */

    *cparam = 0;
  }
  return OK;
}

STATUS CallAPI::CreateDefaultCallParam(CTask* task,CallParameter **cparam,const char* specs)
{
  *cparam = (CallParameter *)CRC_InvalidInterface;

  // check if interface name contains dot
  char itfName[64];
  char grpName[64];
  int count = sscanf(specs,"%[^.].%s",itfName,grpName);

  CallParameter* cp;
  TaskCallBlock *tcb = _manager->GetTaskCallBlock(task);
  switch (count) {
    case 1:
      break;

    case 2:
      cp = new CallParameter;
      tcb->_cparam = *cp;
      delete cp;
      cp = NULL;
      *cparam = &tcb->_cparam;
      return OK;

    default:
      // syntax error
      CLog::Error("Invalid specifier '%s' in CreateDefaultCallParam",specs);
      return OK;
  }

  // Find the interface instance
  Interface *itf = MediaManager::GetInstance()->FindInterface(itfName);
  if (!itf) {
    CLog::Warning(THISMODULE,task,"Unable to find interface '%s' in CreateDefaultCallParam",specs);
    //*cparam = (CallParameter *)CRC_InvalidInterface;
    return OK;
  }

  // Find the signalling driver
  SignallingDriver *sig = itf->GetSignallingDriver();
  if (sig) {
    CallParameter *cp = sig->CreateDefaultCallParameter();
    tcb->_cparam = *cp;
    delete cp;
    cp = NULL;
    *cparam = &tcb->_cparam;
  } else {
    *cparam = (CallParameter *)CRC_InvalidSignalling;
  }
  return OK;
}

bool CallAPI::ValidCallParameter(CTask* task,CallParameter* cparam,const char* funcname)
{
  if ( cparam && cparam->IsValid()) return true;
  /* Later can be reenabled above code */
  //if ( cparam->IsValid()) return true;

  CLog::Error(THISMODULE, task, "Invalid call parameter handle 0x%x passed in %s", cparam, funcname);
  return false;
}



/** API to modify or retrieve a particular call parameter field
*/
#define DEFINE_SETCALLPARAM(F,vtype,field) \
  STATUS CallAPI::F(CTask* task,CallParameter* cparam,vtype value) \
{ \
  if (!ValidCallParameter(task,cparam,#F)) return NOT_OK; \
  cparam->field = value; \
  return OK; \
}

  DEFINE_SETCALLPARAM(SetSessionId,const char*,csi)
  DEFINE_SETCALLPARAM(SetConnectionType,CallConnectionType,cct)
  DEFINE_SETCALLPARAM(SetCompandingLaw,CallCompandingLaw,ccl)
  DEFINE_SETCALLPARAM(SetCalledPartyCategory,CallPartyCategory,cpcCalled)
  DEFINE_SETCALLPARAM(SetCalledNumber,const char*,cnCalled)
  DEFINE_SETCALLPARAM(SetCalledNumberPlan,CallNumberPlan,cnpCalled)
  DEFINE_SETCALLPARAM(SetCalledNumberType,CallNumberType,cntCalled)
  DEFINE_SETCALLPARAM(SetCalledSubaddress,const char*,csCalled)
DEFINE_SETCALLPARAM(SetCalledSignalAddress,const char*,csaCalled)
#ifdef XGATE_HAS_ACME_SBC
  DEFINE_SETCALLPARAM(SetRouteSignalAddress,const char*,routeAddr)
  ////DEFINE_SETCALLPARAM(SetProxyUserName,const char*,proxyUserName) //TODO: Yoga, added for stateful sbc proxy to terminate deskphone calls
  DEFINE_SETCALLPARAM(SetCSFilename,const char*,csfilename) 
  DEFINE_SETCALLPARAM(SetRbtFile,const char*,rbtFile) 
  ////DEFINE_SETCALLPARAM(SetCallOnHoldFilenameCallingParty,const char*,callOnHoldFilenameCallingParty)
  ////DEFINE_SETCALLPARAM(SetCallOnHoldFilenameCalledParty,const char*,callOnHoldFilenameCalledParty)
  DEFINE_SETCALLPARAM(SetCallForwardingFilename,const char*,callForwardingFile)
  DEFINE_SETCALLPARAM(SetVoicemailRecordingFilename,const char*,voicemailRecordingFile)
  DEFINE_SETCALLPARAM(SetCalledDeviceType,const char*,calledDeviceType) 
  DEFINE_SETCALLPARAM(SetCallRecording,int,callRecording) 
  DEFINE_SETCALLPARAM(SetIsForkingEnabled,int,isForkingenabled) 
  //DEFINE_SETCALLPARAM(SetDomainName,const char*,domainName) //TODO: Yoga, added for stateful sbc proxy to terminate deskphone calls
#endif
  DEFINE_SETCALLPARAM(SetCalledComplete,int,complete)
  DEFINE_SETCALLPARAM(SetUserInfo,const char*,cui)
  DEFINE_SETCALLPARAM(SetCallingPartyCategory,CallPartyCategory,cpcCalling)
  DEFINE_SETCALLPARAM(SetCallingNumber,const char*,cnCalling)
  DEFINE_SETCALLPARAM(SetCallingNumberPlan,CallNumberPlan,cnpCalling)
  DEFINE_SETCALLPARAM(SetCallingNumberType,CallNumberType,cntCalling)
  DEFINE_SETCALLPARAM(SetCallingSubaddress,const char*,csCalling)DEFINE_SETCALLPARAM(SetCallingSignalAddress,const char*,csaCalling)
  DEFINE_SETCALLPARAM(SetCallingPresentation,CallCallingPresentation,ccp)
  DEFINE_SETCALLPARAM(SetCallingScreening,CallCallingScreening,ccs)
  DEFINE_SETCALLPARAM(SetPrivateInfo,const char*,cpinfo)
  DEFINE_SETCALLPARAM(SetRedirectingNumber,const char*,cnRedirectingNumber)
  DEFINE_SETCALLPARAM(SetRedirectingNumberPlan,CallNumberPlan,cnpRedirectingNumber)
  DEFINE_SETCALLPARAM(SetRedirectingNumberType,CallNumberType,cntRedirectingNumber)
  DEFINE_SETCALLPARAM(SetRedirectingNumberPresentation,CallCallingPresentation,ccpRedirectingNumber)
  DEFINE_SETCALLPARAM(SetLocationNumber,const char*,cnLocationNumber)
  DEFINE_SETCALLPARAM(SetLocationNumberPlan,CallNumberPlan,cnpLocationNumber)
  DEFINE_SETCALLPARAM(SetLocationNumberType,CallNumberType,cntLocationNumber)
  DEFINE_SETCALLPARAM(SetLocationNumberPresentation,CallCallingPresentation,ccpLocationNumber)
  DEFINE_SETCALLPARAM(SetOriginalCalledNumber,const char*,cnOriginalCalledNumber)
  DEFINE_SETCALLPARAM(SetOriginalCalledNumberPlan,CallNumberPlan,cnpOriginalCalledNumber)
  DEFINE_SETCALLPARAM(SetOriginalCalledNumberType,CallNumberType,cntOriginalCalledNumber)
  DEFINE_SETCALLPARAM(SetOriginalCalledNumberPresentation,CallCallingPresentation,ccpOriginalCalledNumber)
  DEFINE_SETCALLPARAM(SetRedirectionIndicator, int, redirectionIndicator)
  DEFINE_SETCALLPARAM(SetRedirectionOrigReason, int, redirectionOrigReason)
  DEFINE_SETCALLPARAM(SetRedirectionCounter, int, redirectionCounter)
DEFINE_SETCALLPARAM(SetRedirectionReason, int, redirectionReason)

  DEFINE_SETCALLPARAM(SetCallingMobileIdentityType,CallMobileIdentityType,cmitCalling)
  DEFINE_SETCALLPARAM(SetCallType,CallType,ct)
  DEFINE_SETCALLPARAM(SetBssapDH,int,bssapDHVal)
  DEFINE_SETCALLPARAM(SetSuspendResumeNotification,bool,suspend_resume_notification)
  DEFINE_SETCALLPARAM(SetHuntGroupCaller,bool,_isHuntGroupCaller)
  //////DEFINE_SETCALLPARAM(SetSwitchBoardCall,bool,_isSwitchBoardCall)
  DEFINE_SETCALLPARAM(SetCallWaitingFlag,bool,cw)
DEFINE_SETCALLPARAM(SetECTNotification,bool,ect_notification)



  /** Various get methods for CallParameter object
  */
#define DEFINE_GETCALLPARAM(F,vtype,field) \
    STATUS CallAPI::F(CTask* task,vtype* retval,CallParameter* cparam) \
{ \
  if (!ValidCallParameter(task,cparam,#F)) return NOT_OK; \
  *retval = cparam->field; \
  return OK; \
}

#define DEFINE_GETCALLPARAM_STR(F,field) \
    STATUS CallAPI::F(CTask* task,char* retval,CallParameter* cparam) \
{ \
  if (!ValidCallParameter(task,cparam,#F)) return NOT_OK; \
  safe_strcpy(retval,cparam->field.c_str(),MAXAPPSTRLEN); \
  return OK; \
}

  DEFINE_GETCALLPARAM_STR(GetSessionId,csi)
  DEFINE_GETCALLPARAM(GetConnectionType,CallConnectionType,cct)
  DEFINE_GETCALLPARAM(GetCompandingLaw,CallCompandingLaw,ccl)
  DEFINE_GETCALLPARAM(GetCalledPartyCategory,CallPartyCategory,cpcCalled)
  DEFINE_GETCALLPARAM_STR(GetCalledNumber,cnCalled)
  DEFINE_GETCALLPARAM(GetCalledNumberPlan,CallNumberPlan,cnpCalled)
  DEFINE_GETCALLPARAM(GetCalledNumberType,CallNumberType,cntCalled)
  DEFINE_GETCALLPARAM_STR(GetCalledSubaddress,csCalled)
DEFINE_GETCALLPARAM_STR(GetCalledSignalAddress,csaCalled)
#ifdef XGATE_HAS_ACME_SBC
  //DEFINE_GETCALLPARAM_STR(GetRouteSignalAddress,routeAddr)
  /////DEFINE_GETCALLPARAM_STR(GetProxyUsername,proxyUserName) //TODO: Yoga, added for stateful sbc proxy to terminate deskphone calls
  //DEFINE_GETCALLPARAM_STR(GetDomainName,domainName) //TODO: Yoga, added for stateful sbc proxy to terminate deskphone calls
#endif
  DEFINE_GETCALLPARAM(GetCalledComplete,int,complete)
  DEFINE_GETCALLPARAM_STR(GetUserInfo,cui)
  DEFINE_GETCALLPARAM(GetCallingPartyCategory,CallPartyCategory,cpcCalling)
  DEFINE_GETCALLPARAM_STR(GetCallingNumber,cnCalling)
  DEFINE_GETCALLPARAM(GetCallingNumberPlan,CallNumberPlan,cnpCalling)
  DEFINE_GETCALLPARAM(GetCallingNumberType,CallNumberType,cntCalling)
  DEFINE_GETCALLPARAM_STR(GetCallingSubaddress,csCalling)
  DEFINE_GETCALLPARAM_STR(GetCallingSignalAddress,csaCalling)
  DEFINE_GETCALLPARAM(GetCallingPresentation,CallCallingPresentation,ccp)
  DEFINE_GETCALLPARAM(GetCallingScreening,CallCallingScreening,ccs)
  DEFINE_GETCALLPARAM_STR(GetPrivateInfo,cpinfo)
DEFINE_GETCALLPARAM_STR(GetRedirectingNumber,cnRedirectingNumber)
#ifdef CALL_TRANSFER
//DEFINE_GETCALLPARAM_STR(GetRedirectionAddress,csaRedirected)
#endif
  DEFINE_GETCALLPARAM(GetRedirectingNumberPlan,CallNumberPlan,cnpRedirectingNumber)
  DEFINE_GETCALLPARAM(GetRedirectingNumberType,CallNumberType,cntRedirectingNumber)
  DEFINE_GETCALLPARAM(GetRedirectingNumberPresentation,CallCallingPresentation,ccpRedirectingNumber)
  DEFINE_GETCALLPARAM_STR(GetLocationNumber,cnLocationNumber)
  DEFINE_GETCALLPARAM(GetLocationNumberPlan,CallNumberPlan,cnpLocationNumber)
  DEFINE_GETCALLPARAM(GetLocationNumberType,CallNumberType,cntLocationNumber)
  DEFINE_GETCALLPARAM(GetLocationNumberPresentation,CallCallingPresentation,ccpLocationNumber)
  DEFINE_GETCALLPARAM_STR(GetOriginalCalledNumber,cnOriginalCalledNumber)
  DEFINE_GETCALLPARAM(GetOriginalCalledNumberPlan,CallNumberPlan,cnpOriginalCalledNumber)
  DEFINE_GETCALLPARAM(GetOriginalCalledNumberType,CallNumberType,cntOriginalCalledNumber)
  DEFINE_GETCALLPARAM(GetOriginalCalledNumberPresentation,CallCallingPresentation,ccpOriginalCalledNumber)
  DEFINE_GETCALLPARAM(GetRedirectionIndicator, int, redirectionIndicator)
  DEFINE_GETCALLPARAM(GetRedirectionOrigReason, int, redirectionOrigReason)
  DEFINE_GETCALLPARAM(GetRedirectionCounter, int, redirectionCounter)
DEFINE_GETCALLPARAM(GetRedirectionReason, int, redirectionReason)

  DEFINE_GETCALLPARAM(GetCallingMobileIdentityType,CallMobileIdentityType,cmitCalling)
  DEFINE_GETCALLPARAM(GetCallType,CallType,ct)
  DEFINE_GETCALLPARAM(GetBssapDH,int,bssapDHVal)
  DEFINE_GETCALLPARAM(GetSuspendResumeNotification,bool,suspend_resume_notification)
DEFINE_GETCALLPARAM(GetCallWaitingFlag,bool,cw)


  /** Various get methods for call object
  */
#define DEFINE_GETCALL(F,vtype,field) \
    STATUS CallAPI::F(CTask* task,vtype* retval,int handle) \
{ \
  Call* call = _manager->CallFromHandle(task,handle,true,#F); \
  if (call) *retval = call->GetCallParameter()->field; \
  else *retval = (vtype)CRC_InvalidCallHandle; \
  return OK; \
}

#define DEFINE_GETCALL_STR(F,field) \
    STATUS CallAPI::F(CTask* task,char* retval,int handle) \
{ \
  Call* call = _manager->CallFromHandle(task,handle,true,#F); \
  if (call) safe_strcpy(retval,call->GetCallParameter()->field.c_str(),MAXAPPSTRLEN); \
  else retval[0] = '\0'; \
  return OK; \
}

  DEFINE_GETCALL_STR(CallGetSessionId,csi)
  DEFINE_GETCALL(CallGetConnectionType,CallConnectionType,cct)
  DEFINE_GETCALL(CallGetCompandingLaw,CallCompandingLaw,ccl)
  DEFINE_GETCALL(CallGetCalledPartyCategory,CallPartyCategory,cpcCalled)
  DEFINE_GETCALL_STR(CallGetCalledNumber,cnCalled)
  DEFINE_GETCALL(CallGetCalledNumberPlan,CallNumberPlan,cnpCalled)
  DEFINE_GETCALL(CallGetCalledNumberType,CallNumberType,cntCalled)
  DEFINE_GETCALL_STR(CallGetCalledSubaddress,csCalled)
DEFINE_GETCALL_STR(CallGetCalledSignalAddress,csaCalled)
#ifdef XGATE_HAS_ACME_SBC
  //DEFINE_GETCALL_STR(CallGetRouteSignalAddress,routeAddr)
  /////DEFINE_GETCALL_STR(CallGetProxyUserName, proxyUserName) //TODO: Yoga, added for stateful sbc proxy to terminate deskphone calls
  //DEFINE_GETCALL_STR(CallGetDomainName,domainName) //TODO: Yoga, added for stateful sbc proxy to terminate deskphone calls
#endif
  DEFINE_GETCALL(CallGetCalledComplete,int,complete)
  DEFINE_GETCALL_STR(CallGetUserInfo,cui)
  DEFINE_GETCALL(CallGetCallingPartyCategory,CallPartyCategory,cpcCalling)
  DEFINE_GETCALL_STR(CallGetCallingNumber,cnCalling)
  DEFINE_GETCALL(CallGetCallingNumberPlan,CallNumberPlan,cnpCalling)
  DEFINE_GETCALL(CallGetCallingNumberType,CallNumberType,cntCalling)
  DEFINE_GETCALL_STR(CallGetCallingSubaddress,csCalling)
  DEFINE_GETCALL_STR(CallGetCallingSignalAddress,csaCalling)
  DEFINE_GETCALL(CallGetCallingPresentation,CallCallingPresentation,ccp)
  DEFINE_GETCALL(CallGetCallingScreening,CallCallingScreening,ccs)
  DEFINE_GETCALL_STR(CallGetPrivateInfo,cpinfo)
  DEFINE_GETCALL_STR(CallGetRedirectingNumber,cnRedirectingNumber)
  DEFINE_GETCALL(CallGetRedirectingNumberPlan,CallNumberPlan,cnpRedirectingNumber)
  DEFINE_GETCALL(CallGetRedirectingNumberType,CallNumberType,cntRedirectingNumber)
  DEFINE_GETCALL(CallGetRedirectingNumberPresentation,CallCallingPresentation,ccpRedirectingNumber)
  DEFINE_GETCALL_STR(CallGetLocationNumber,cnLocationNumber)
  DEFINE_GETCALL(CallGetLocationNumberPlan,CallNumberPlan,cnpLocationNumber)
  DEFINE_GETCALL(CallGetLocationNumberType,CallNumberType,cntLocationNumber)
  DEFINE_GETCALL(CallGetLocationNumberPresentation,CallCallingPresentation,ccpLocationNumber)
  DEFINE_GETCALL_STR(CallGetOriginalCalledNumber,cnOriginalCalledNumber)
  DEFINE_GETCALL(CallGetOriginalCalledNumberPlan,CallNumberPlan,cnpOriginalCalledNumber)
  DEFINE_GETCALL(CallGetOriginalCalledNumberType,CallNumberType,cntOriginalCalledNumber)
  DEFINE_GETCALL(CallGetOriginalCalledNumberPresentation,CallCallingPresentation,ccpOriginalCalledNumber)
  DEFINE_GETCALL(CallGetRedirectionIndicator, int, redirectionIndicator)
  DEFINE_GETCALL(CallGetRedirectionOrigReason, int, redirectionOrigReason)
  DEFINE_GETCALL(CallGetRedirectionCounter, int, redirectionCounter)
  DEFINE_GETCALL(CallGetRedirectionReason, int, redirectionReason)
  DEFINE_GETCALL(CallGetCallingMobileIdentityType,CallMobileIdentityType,cmitCalling)
  DEFINE_GETCALL(CallGetCallType,CallType,ct)
  DEFINE_GETCALL(CallGetSuspendResumeNotification,bool,suspend_resume_notification)
  DEFINE_GETCALL(CallGetCallWaitingFlag,bool,cw)
DEFINE_GETCALL(CallGetBssapDH,int,bssapDHVal)

  /** Get Call Quality
  */
  STATUS API_CALL CallAPI::CallGetQuality(CTask* task, float* value, int handle, int stream, const char* name) {
    Call* call = _manager->CallFromHandle(task,handle,true,"CallGetQuality");
    *value = -1;
    if (!call) 
      return OK;
    if (stream == 0) {
      if (strcmp(name, "packets") == 0) *value = call->GetQuality(CQ_RX_PACKETS);
      else if (strcmp(name, "bytes") == 0) *value = call->GetQuality(CQ_RX_BYTES);
      else if (strcmp(name, "rtt") == 0) *value = call->GetQuality(CQ_RX_RTT);
      else if (strcmp(name, "jitter") == 0) *value = call->GetQuality(CQ_RX_JITTER);
      else if (strcmp(name, "loss") == 0) *value = call->GetQuality(CQ_RX_LOSS);
      else if (strcmp(name, "fracloss") == 0) *value = call->GetQuality(CQ_RX_FRACLOSS);
      else if (strcmp(name, "is") == 0) *value = call->GetQuality(CQ_RX_IS);
      else if (strcmp(name, "id") == 0) *value = call->GetQuality(CQ_RX_ID);
      else if (strcmp(name, "ie") == 0) *value = call->GetQuality(CQ_RX_IE);
      else if (strcmp(name, "itot") == 0) *value = call->GetQuality(CQ_RX_ITOT);
    }
    else {
      if (strcmp(name, "packets") == 0) *value = call->GetQuality(CQ_TX_PACKETS);
      else if (strcmp(name, "bytes") == 0) *value = call->GetQuality(CQ_TX_BYTES);
      else if (strcmp(name, "rtt") == 0) *value = call->GetQuality(CQ_TX_RTT);
      else if (strcmp(name, "jitter") == 0) *value = call->GetQuality(CQ_TX_JITTER);
      else if (strcmp(name, "loss") == 0) *value = call->GetQuality(CQ_TX_LOSS);
      else if (strcmp(name, "fracloss") == 0) *value = call->GetQuality(CQ_TX_FRACLOSS);
      else if (strcmp(name, "is") == 0) *value = call->GetQuality(CQ_TX_IS);
      else if (strcmp(name, "id") == 0) *value = call->GetQuality(CQ_TX_ID);
      else if (strcmp(name, "ie") == 0) *value = call->GetQuality(CQ_TX_IE);
      else if (strcmp(name, "itot") == 0) *value = call->GetQuality(CQ_TX_ITOT);
    }

    return OK;
  }

/**
 * Get authorization handle.
 */
STATUS API_CALL CallAPI::CallGetAuthorization(CTask* task, int* auth_handle, int call_handle)
{
  *auth_handle = 0;

  Call* call = _manager->CallFromHandle(task,call_handle,true,"CallGetAuthorization");
  if (!call) 
    return OK;

  *auth_handle = call->GetAuthorizationHandle();
  return OK;
}

STATUS CallAPI::SetCallCredential(CTask* task, CallParameter*cparam, const char *username, const char*password)
{ 
  if (!ValidCallParameter(task,cparam,"SetCallCredential")) return NOT_OK;
  cparam->credential.crUserName=username;
  cparam->credential.crPassword=password;
  return OK;
}


/** Extension to standard call API: used in SIP and sent in DISCONNECT message
*/
STATUS CallAPI::ExSetDisconnectMsg(CTask* task,int handle,const char* msg)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallExSetDisconnectMsg");
  if (call) 
    call->SetDisconnectMsg(msg);
  return OK;
}

//#ifndef XGATE_HAS_VLINKAGENT
//extern void SetLockingLocation(const char* where);
//#else
void SetLockingLocation(const char* where)
{
  atomic_set (&Kernel::Instance()->currentService, where);
}
//#endif

STATUS CallAPI::SetSwitchBoardCall(CTask* task,CallParameter* cparam,bool switchboard_call)
{
	if (switchboard_call)
        	CLog::Detail(THISMODULE,task,"callmanager.cpp : Inside SetSwitchBoardCall() ; cparam = %d ; switchboard_call == TRUE",cparam);
	else
        	CLog::Detail(THISMODULE,task,"callmanager.cpp : Inside SetSwitchBoardCall() ; cparam = %d ; switchboard_call == FALSE",cparam);
	if(switchboard_call)
		cparam->_isSwitchBoardCall =  true;
	else
		cparam->_isSwitchBoardCall =  false;

      	return OK;
}

STATUS CallAPI::SetSwitchBoardCallState(CTask* task,CallParameter* cparam,int callstate, int mindigits, int timeout)
{
#if 0
        CLog::Detail(THISMODULE,task,"callmanager.cpp : Inside SETSwitchBoardCallState() ; cparam = %d ; SET callstate = %d",cparam, callstate);
        xGateCallInfo* call = GetHmpCallObject(task, 0); // handle == 0 for h_legA for a switchboard-call.
  	if(call){
		// CallParameter *cp = call->GetCallParameter();
                //cp  = cparam;
		cparam->swbinfo.PlayFile = "";
		cparam->swbinfo.SwbCallEvent = callstate;
    		cparam->swbinfo.maxDigits = mindigits;
		cparam->swbinfo.DigitTimeOut = timeout;
		cparam->swbinfo.DigitReceived = "";

    		call->PostMessageToReceiver(call->GetState(),cparam->call_id,*cparam,XGATESIPEVENT::EN_SWB_EVENT_REL);

		//CallManager::Instance()->SuspendOwnerTask(call);
		////task->Sleep(40);
	}
#endif
      	return OK;
}
STATUS CallAPI::GetSwitchBoardCallState(CTask* task,CallParameter* cparam,int* retval, char* dtmf_digits)
{
#if 0
        xGateCallInfo* call = GetHmpCallObject(task, 0); // handle == 0 for h_legA for a switchboard-call.
  	if(call){
        	CLog::Detail(THISMODULE,task,"callmanager.cpp : Inside GetSwitchBoardCallState() ; Get callstate = %d",call->GetCallParameter()->swbinfo.SwbCallEvent);

		*retval = call->GetCallParameter()->swbinfo.SwbCallEvent;
                //dtmf_digits = call->GetCallParameter()->swbinfo.DigitReceived.c_str();
                safe_strcpy(dtmf_digits, (const char*) call->GetCallParameter()->swbinfo.DigitReceived.c_str(), MAXAPPSTRLEN);
        	CLog::Detail(THISMODULE,task,"callmanager.cpp : GetSwitchBoardCallState() ; Get callstate = %d ; dtmf_digits = %s",call->GetCallParameter()->swbinfo.SwbCallEvent,dtmf_digits );
	}
#endif
      	return OK;
}
STATUS CallAPI::SwitchBoardSuspendTask(CTask* task,CallParameter* cparam, int value)
{
#if 0
        CLog::Detail(THISMODULE,task,"callmanager.cpp : Inside SwitchBoardSuspendTask() ; cparam = %di ; suspend_timeout = %u",cparam, value);
	if (task)
		task->Sleep(1);
        //xGateCallInfo* call = GetHmpCallObject(task, 0); // handle == 0 for h_legA for a switchboard-call.
#endif
      	return OK;
}

/** This function is called on behalf of a task that initiates a call
  A call is placed either on a specific interface or using a wildcard 
  using the provided call parameters (cparam)
  Interface name can be specified as <interface>|*[.<group>
  */
STATUS CallAPI::Setup(CTask* task,int* retcode,const char* specs,CallParameter* cparam,int* handle)
{
  *retcode = CRC_NoError;
  *handle = -1;

  CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : Inside setup() ; cparam = %02x ; cparam = %d ",cparam, cparam);
  CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : Inside setup() ; cparam->call_id.c_str() = %s ",cparam->call_id.c_str());
  CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : Inside setup() ; task = %s ",task->GetTaskName());
  //////task->SetTaskName("si0000");
  CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : Inside setup() ; task = %s ",task->GetTaskName());

  if (cparam && !cparam->IsValid()) {
    *retcode = CRC_InvalidCallParameter;
    CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : Invalid call parameter specified in CallSetup");
    return OK;
  }

  // check if interface name contains dot
  char itfName[64];
  char grpName[64];
  int count = sscanf(specs,"%[^.].%s",itfName,grpName);

  char display[256];
  sprintf(display, "Setup(%s)", specs);
  SetLockingLocation(display);

  STATUS rc;
  switch (count) {
    case 1:
      rc = SetupInterface(task,retcode,specs,cparam,handle);
      SetLockingLocation("Setup()");
      CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : switch (count) : case 1 ; return rc;");
      return rc;

    case 2:
      rc = SetupGroup(task,retcode,itfName,grpName,cparam,handle);
      SetLockingLocation("Setup()");
      CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : switch (count) : case 2 ; return rc;");
      return rc;

    default:
      // syntax error
      CLog::Detail("Invalid specifier %s in CallSetup",specs);
      *retcode = CRC_InvalidParameter;
      SetLockingLocation("Setup()");
      CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : switch (count) : case default ; return OK;");
      return OK;
  }

}

STATUS CallAPI::CallGetRouteSignalAddress(CTask* task,char* retval, CallParameter* cparam) 
{
  //TODO: Yoga 
  if (cparam && !cparam->IsValid()) {
    CLog::Warning(THISMODULE,task,"Invalid call parameter specified in CallGetRouteSignalAddress");
    return OK;
  }

  safe_strcpy(retval, cparam->routeAddr.c_str(), MAXAPPSTRLEN);

  return OK;
}

STATUS CallAPI::CallGetProxyUserName(CTask* task,char* retval, CallParameter* cparam) 
{
  //TODO: Yoga 
  if (cparam && !cparam->IsValid()) {
    CLog::Warning(THISMODULE,task,"Invalid call parameter specified in CallGetProxyUserName");
    return OK;
  }

  safe_strcpy(retval, cparam->proxyUserName.c_str(), MAXAPPSTRLEN);

  return OK;
}

STATUS CallAPI::SetProxyUserName(CTask* task,CallParameter* cparam,const char* value)
{
#if 0
        CLog::Detail(THISMODULE,task,"callmanager.cpp : Inside SetProxyUserName() ; cparam = %d ; SET ProxyUserName = %s",cparam, value);
        xGateCallInfo* call = GetHmpCallObject(task, 0); // handle == 0 for h_legA for a switchboard-call.
  	if(call){
//		CallParameter *cp = call->GetCallParameter();
//                cp  = cparam;
		//cp->proxyUserName = value; // this string-assignment is not working! -instead use safe_strcpy() as below...
  		////safe_strcpy(cp->proxyUserName.c_str(), value, MAXAPPSTRLEN);
  		cparam->proxyUserName.assign(value, strlen(value));
        	CLog::Detail(THISMODULE,task,"callmanager.cpp : Inside SetProxyUserName() ; cparam = %d ; SET ProxyUserName = %s",cparam, value);
  		cparam->proxyUserName.assign(value, strlen(value));

		// Disabled for testing; to be un-commented later...
		call->PostMessageToReceiver(call->GetState(),cparam->call_id,*cparam,XGATESIPEVENT::EN_SWB_EVENT_REL);
	}
	/******
	else
	{
  		cparam->proxyUserName.assign(value, strlen(value));
	}
	*******/
#endif
      	return OK;
}

STATUS CallAPI::SetCallOnHoldFilenameCalledParty(CTask* task,CallParameter* cparam,const char* value)
{
#if 0
        CLog::Detail(THISMODULE,task,"callmanager.cpp : Inside SetCallOnHoldFilenameCalledParty() ; cparam = %d ; SET callOnHoldFilenameCalledParty = %s",cparam, value);
        xGateCallInfo* call = GetHmpCallObject(task, 0); // handle == 0 for h_legA for a switchboard-call.
        if(call){
                CallParameter *cp = call->GetCallParameter();
                cp  = cparam;
                cp->callOnHoldFilenameCalledParty.assign(value, strlen(value));
                CLog::Detail(THISMODULE,task,"callmanager.cpp : Inside SetCallOnHoldFilenameCalledParty() ; cparam = %d ; SET callOnHoldFilenameCalledParty = %s",cparam, value);
                cparam->callOnHoldFilenameCalledParty.assign(value, strlen(value));

                ////call->PostMessageToReceiver(call->GetState(),cp->call_id,cp,XGATESIPEVENT::EN_SWB_EVENT_REL);
        }
        else
                cparam->callOnHoldFilenameCalledParty.assign(value, strlen(value));
#endif
        return OK;
}

STATUS CallAPI::SetCallOnHoldFilenameCallingParty(CTask* task,CallParameter* cparam,const char* value)
{
#if 0
        CLog::Detail(THISMODULE,task,"callmanager.cpp : Inside SetCallOnHoldFilenameCallingParty() ; cparam = %d ; SET callOnHoldFilenameCallingParty = %s",cparam, value);
        xGateCallInfo* call = GetHmpCallObject(task, 0); // handle == 0 for h_legA for a switchboard-call.
        if(call){
                CallParameter *cp = call->GetCallParameter();
                cp  = cparam;
                cp->callOnHoldFilenameCallingParty.assign(value, strlen(value));
                CLog::Detail(THISMODULE,task,"callmanager.cpp : Inside SetCallOnHoldFilenameCallingParty() ; cparam = %d ; SET callOnHoldFilenameCallingParty = %s",cparam, value);
                cparam->callOnHoldFilenameCallingParty.assign(value, strlen(value));

                ////call->PostMessageToReceiver(call->GetState(),cp->call_id,cp,XGATESIPEVENT::EN_SWB_EVENT_REL);
        }
        else
                cparam->callOnHoldFilenameCallingParty.assign(value, strlen(value));
#endif
        return OK;
}

STATUS CallAPI::Setup_callque(CTask* task,int* retcode,const char* specs,CallParameter* cparam,int* handle)
{
  printf("\n*********************inside CallSetup_callque\n");
  CLog::Warning(THISMODULE,task,"inside CallSetup_callque");
  *retcode = CRC_NoError;
  *handle = -1;

  if (cparam && !cparam->IsValid()) {
    *retcode = CRC_InvalidCallParameter;
    CLog::Warning(THISMODULE,task,"Invalid call parameter specified in CallSetup_callque");
    return OK;
  }

  // check if interface name contains dot
  char itfName[64];
  char grpName[64];
  int count = sscanf(specs,"%[^.].%s",itfName,grpName);

  char display[256];
  sprintf(display, "Setup_callque(%s)", specs);
  SetLockingLocation(display);

  STATUS rc;
  switch (count) {
    case 1:
      *handle = 999;
      rc = SetupInterface(task,retcode,specs,cparam,handle); // handle changed to 999 -to recognize this as callque case
      SetLockingLocation("Setup_callque()");
      return rc;

    case 2:
      rc = SetupGroup(task,retcode,itfName,grpName,cparam,handle);
      SetLockingLocation("Setup_callque()");
      return rc;

    default:
      // syntax error
      CLog::Error("Invalid specifier %s in CallSetup_callque",specs);
      *retcode = CRC_InvalidParameter;
      SetLockingLocation("Setup_callque()");
      return OK;
  }

}

/** This variant of CallSetup is called when application specify a specific interface
/** This variant of CallSetup is called when application specify a specific interface
*/
STATUS CallAPI::SetupInterface(CTask* task,int* retcode,const char* itfName,CallParameter* cparam,int* handle)
{
  CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : Inside SetupInterface() ; cparam = %d ; call_id =%s", cparam, cparam->call_id.c_str());
  Interface* itf = MediaManager::GetInstance()->FindInterface(itfName);

  if (!itf) {
    *retcode = CRC_InvalidInterface;
    CLog::Detail(THISMODULE,task,"Invalid interface name %s specified in CallSetup",itfName);
    CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : SetupInterface() : Invalid interface name %s specified in CallSetup",itfName);
    return OK;
  }

  if (!itf->_enabled) {
    *retcode = CRC_NoCircuit;
    CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : SetupInterface() : !itf->_enabled ; return OK;");
    return OK;
  }

  // Find the signalling driver
  SignallingDriver* sig = itf->GetSignallingDriver();
  Call* call = NULL;
#if 0
  if (!sig) {
    // TODO : Temporary Solution need have fine logic
    
    CLog::Info(THISMODULE,"\nCallAPI::SetupInterface in callmanager.cpp itfName : %s \n\n",itfName);
    if(strstr(itfName,"rtp") || strstr(itfName,"RTP")){
      xGateCallInfo* obj = GetCallProcessobj(cparam->call_id);
      if(obj)
        call = obj->SetupReq(task, itf, SS_FIFO, cparam);

    }
    else{
      *retcode = CRC_InvalidSignalling;
      CLog::Warning(THISMODULE,task,"Invalid signalling for interface %s in CallSetup",itfName);
      //printf("Invalid signalling for interface %s in CallSetup \n",itfName);
           return OK;
    }

  }
  else
#endif
{
    //TBD: follow the settings from RLS (or manually)
    if (*handle == 999)
    {
      //		printf("\n ******************* SetupInterface() , *handle = 999 ; handle = %d ******************* \n",*handle);
      CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : SetupInterface() : sig->SetupReq(task, itf,999,cparam");
      call = sig->SetupReq(task, itf,999,cparam);
    }
    else
    {
      CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : SetupInterface() : sig->SetupReq(task, itf,SS_FIFO,cparam");
      call = sig->SetupReq(task, itf,SS_FIFO,cparam);
    }
  }
  if (!call) {
    *retcode = CRC_InvalidParameter;
    CLog::Detail(THISMODULE,task,"multithreaded-ippbx : callmanager.cpp : SetupInterface() : (!call) return OK;");
    return OK;
  }

  assert(!call->_released);
  if (call->_released) {
    *retcode = CRC_InvalidCallHandle;
    return OK;
  }

  call->TraceSetup();
#ifdef CALL_LOG_TRANSITION
  call->_transition.push_front(CT_SetupReq);
#endif

  if (call->GetState() == CS_Null) 
    call->SetState(CS_Initiating);
  *handle = call->AttachTask(task);

  // call initiation has failed, simulate call hangup
//  if (call->GetCause() != CC_Unassigned)
 //   CallManager::Instance()->_listCallToBeReleased.push_back(call);

  //Narasimham - uncommented due to crashes
#ifndef __linux__
  call->SuspendTask();  //*** TBD: review the need to block
#endif

  return OK;
}

/** This variant of CallSetup is called when application specify a wildcard 
  (any interface in a group)
  */
STATUS CallAPI::SetupGroup(CTask* task,int* retcode,const char* itfName,const char* grpName,CallParameter* cparam,int* handle)
{
  int i=0,maxLoop = MediaManager::GetInstance()->GetInterfaceCount();
  for (Interface *itf = MediaManager::GetInstance()->ItGetInterfaceRoundRobin();
      itf != NULL && i<maxLoop;
      itf = MediaManager::GetInstance()->ItGetInterfaceRoundRobin(), i++) {

    // check if the interface name and group matches the specification
    if ((strcmp(itfName,"*") == 0 || strcmpi(itfName,itf->Name().c_str()) == 0) && (strcmp(grpName,"*") == 0 || strcmpi(grpName,itf->GetGroupName()) == 0) &&
        itf->_enabled) {

      // Find the signalling driver
      SignallingDriver* sig = itf->GetSignallingDriver();
      if (!sig) {
        *retcode = CRC_InvalidSignalling;
        CLog::Warning(THISMODULE,task,"Invalid signalling for interface %s in CallSetup, skipped",itfName);
        // try next interface
        continue;
      }

      //TBD: follow the settings from RLS (or manually)
      Call* call = sig->SetupReq(task, itf,SS_FIFO,cparam);
      if (!call) {
        *retcode = CRC_InvalidParameter;
        // abandon loop
        return OK;
      }

#ifdef CALL_LOG_TRANSITION
      call->_transition.push_front(CT_SetupReq);
#endif
      if (call->GetState() == CS_Null) 
        call->SetState(CS_Initiating);

      // call initiation has failed, simulate call hangup
      if (call->GetCause() != CC_Unassigned) {
        call->RelComInd(call->GetCause());
        // try next interface
        continue;
      } else {
        *handle = call->AttachTask(task);
        call->SuspendTask();  //*** TBD: review the need to block
        call->TraceSetup();
        return OK;
      }
    }
  }

  *retcode = CRC_NoCircuit;
  return OK;
}

/** Send additional digits to the network in case of overlap dialling
*/
STATUS CallAPI::SendOverlap(CTask* task,int* retcode,int handle,const char* additionalDigits,int complete)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallSendOverlap");
  *retcode = CRC_NoError;
  if (call) {
    char buf[256];
    sprintf(buf, "SendOverlap(%s)", call->GetInterfaceName());
    SetLockingLocation(buf);
    call->OverlapReq(additionalDigits,complete ? true : false);
  } else *retcode = CRC_InvalidCallHandle;
  return OK;
}

/** Get additional digits from buffered digits until:
  - max number of digits is collected
  - complete
  - timeout
  */
STATUS CallAPI::GetMoreDigitsEx(CTask* task,char* additionalDigits,int handle,int mindigits,int timeout,int* complete)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallGetMoreDigits");
  *additionalDigits = '\0';
  *complete = 0;

  if (!call) return OK;

  // Check if it is valid to get for more digits
  if ((call->GetDirection() != CD_Incoming) ||
      (call->GetState() != CS_Overlap)) {
    CLog::Warning(THISMODULE,task,"Invalid state (%s) for call %s in GetMoreDigits",
        StrCallState(call->GetState()),
        call->GetName());
    return OK;
  }

  // Call mustn't be signalling connected
  if (call->_peerCall) {
    CLog::Warning(THISMODULE,task,"GetMoreDigits can't be done on call %s ; signalling already connected",
        call->GetName());
    return OK;
  }

  // Check if we've got enough digits to return to the caller
  if (call->_overlapRcvd.length() >= mindigits || call->_callParam.complete) {
    *complete = call->_callParam.complete;
    safe_strcpy(additionalDigits, call->_overlapRcvd.c_str(), MAXAPPSTRLEN);
    call->_overlapRcvd = "";
    return OK;
  }

  // Otherwise, suspend the task until:
  //   - call becomes completed
  //	 - timeout occurs
  call->_waitForMoreDigits = true;
  call->_maxDigits = mindigits;
  call->_appComplete = complete;
  call->_appDigits = additionalDigits;
  call->_timeLastDigit = GetTickCount();
  call->_delayInterDigit = timeout*1000;

  call->SuspendTask();

  CallManager::Instance()->_listCallWaitDigits.remove(call);
  CallManager::Instance()->_listCallWaitDigits.push_back(call);

  return OK;
}

/** get additional digits from buffered digits if any, or
  activate a timer for call and suspend task
  */
STATUS CallAPI::GetMoreDigits(CTask* task,char* additionalDigits,int handle,int timeout,int* complete)
{
  return GetMoreDigitsEx(task,additionalDigits,handle,1,timeout,complete);
}

/** Set charging mode (free of charge, or charged)
*/
STATUS CallAPI::SetCharging(CTask* task,int handle,int charging)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"SetCharging");
  if (call) call->SetCharging(charging);
  return OK;
}

/** Set resume-suspend notification
*/
STATUS CallAPI::CallSetSuspendResumeNotification(CTask* task,int handle,bool value)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallSetSuspendResumeNotification");
  if (call) call->GetCallParameter()->suspend_resume_notification = value;
  return OK;
}

/** Set ect notification
*/
STATUS CallAPI::CallSetECTNotification(CTask* task,int handle,bool value)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallSetECTNotification");
  if (call) call->GetCallParameter()->ect_notification = value;
  return OK;
}

/** Send PROCEEDING message to the network
*/
STATUS CallAPI::Proceeding(CTask* task,int* retcode,int handle,CallProgress progress)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallProceeding");
  *retcode = CRC_NoError;
  if (call) {
    char buf[256];
    sprintf(buf, "Proceeding(%s)", call->GetInterfaceName());
    SetLockingLocation(buf);
    call->ProceedingReq(progress, 0);
  } else *retcode = CRC_InvalidCallHandle;
  return OK;
}

/** Send PROGRESS message to the network
*/
STATUS CallAPI::Progress(CTask* task,int* retcode,int handle,CallProgress progress)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallProgress");
  *retcode = CRC_NoError;
  if (call) {
    char buf[256];
    sprintf(buf, "Progress(%s)", call->GetInterfaceName());
    SetLockingLocation(buf);
    call->ProgressReq(progress, 0);
  } else *retcode = CRC_InvalidCallHandle;
  return OK;
}

/** Send ALERTING message to the network
*/
STATUS CallAPI::Alerting(CTask* task,int* retcode,int handle)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallAlerting");
  *retcode = CRC_NoError;
  if (call) {
    char buf[256];
    sprintf(buf, "Alerting(%s)", call->GetInterfaceName());
    SetLockingLocation(buf);
    call->AlertingReq(CP_NoIndication, 0);
  } else *retcode = CRC_InvalidCallHandle;
  return OK;
}

STATUS CallAPI::SendInfo(CTask* task,int* retcode,int handle)
{
  cout << "***** CallAPI::SendInfo ]]]]]]]]]]] ****** " << endl;
  CLog::Detail(THISMODULE, "CallAPI::SendInfo \n");

  Call* call = _manager->CallFromHandle(task,handle,true,"CallAnswer");
  *retcode = CRC_NoError;
  if (call) {
    cout << "***** Can able to take call from handle ****** " << endl;
    char buf[256];
    sprintf(buf, "Answer(%s)", call->GetInterfaceName());
    SetLockingLocation(buf);

    // connect is blocking
    //call->SuspendTask();

    call->InfoReq(0);
  }
  else *retcode = CRC_InvalidCallHandle;
  cout << "******* Answer: Going to return oK ******" << endl;
  return OK;

}


/** Send CONNECT message to the network
*/
STATUS CallAPI::Answer(CTask* task,int* retcode,int handle)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallAnswer");
  *retcode = CRC_NoError;
  if (call) {
    cout << "***** Can able to take call from handle ****** " << endl;
    char buf[256];
    sprintf(buf, "Answer(%s)", call->GetInterfaceName());
    SetLockingLocation(buf);

    // connect is blocking
    call->SuspendTask();

    call->ConnectReq(0);
  }
  else *retcode = CRC_InvalidCallHandle;
  cout << "******* Answer: Going to return oK ******" << endl;
  return OK;
}


/** Send SUSPEND message to the network
*/
STATUS CallAPI::Suspend(CTask* task,int* retcode,int handle)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallSuspend");
  *retcode = CRC_NoError;
  if (call) {
    char buf[256];
    sprintf(buf, "Suspend(%s)", call->GetInterfaceName());
    SetLockingLocation(buf);

    call->SuspendReq();
  } else *retcode = CRC_InvalidCallHandle;
  return OK;
}


/** Send RESUME message to the network
*/
STATUS CallAPI::Resume(CTask* task,int* retcode,int handle)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallResume");
  *retcode = CRC_NoError;
  if (call) {
    char buf[256];
    sprintf(buf, "Resume(%s)", call->GetInterfaceName());
    SetLockingLocation(buf);

    call->ResumeReq();
  } else *retcode = CRC_InvalidCallHandle;
  return OK;
}


/** Send DISCONNECT message to the network and detach the call
  from task
  */
STATUS CallAPI::Disconnect(CTask* task,int* retcode,int handle,CallCause cause)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallDisconnect");
  *retcode = CRC_NoError;
  if (call) {
    char buf[256];
    sprintf(buf, "Disconnect(%s)", call->GetInterfaceName());
    SetLockingLocation(buf);

    call->DisconnectReq(cause,CP_NoIndication);
  } else *retcode = CRC_InvalidCallHandle;
  return OK;
}

STATUS CallAPI::RegIpInfo(CTask* task,int* retcode,char *ip_usrA, char *ip_usrB, int pid)
{
  ip_info *ip;

#if 1
  pthread_mutex_lock(&audit_mutex);

  ip = ip_list[task];

  if(ip == NULL)
  {
    ip = (ip_info *)malloc(sizeof(*ip));
  }

  CLog::Detail(THISMODULE,"\n USER A IP = %s, USER B IP = %s -- MALLOC Addr = %02x\n", ip_usrA, ip_usrB, ip);

  strcpy(ip->ip1, ip_usrA);
  strcpy(ip->ip2, ip_usrB);
  ip->pid = pid;

  ip_list[task] = ip;

  pthread_mutex_unlock(&audit_mutex);
#endif
  return OK;
}

STATUS CallAPI::RemoveFile(CTask* task, char *filename)
{
  if (remove(filename) !=0)
  {
    CLog::Detail(THISMODULE,"RemoveFile Failed %s \n", filename);
  } else
  {
    CLog::Detail(THISMODULE,"RemoveFile Success %s \n", filename);
  }
  return OK;
}


/** Send NOTIFY-ECT message to the network
*/
STATUS CallAPI::NotifyECT(CTask* task,int* retcode,int handle, tect_indicator *indicator)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallNotifyECT");
  *retcode = CRC_NoError;
  if (call) {
    char buf[256];
    sprintf(buf, "NotifyECT(%s)", call->GetInterfaceName());
    SetLockingLocation(buf);
    call->NotifyECTReq(*indicator);
  } else *retcode = CRC_InvalidCallHandle;
  return OK;
}

STATUS CallAPI::GetCause(CTask* task,CallCause* cause,int handle)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallGetCause");
  *cause = call ? call->GetCause() : (CallCause)CRC_InvalidCallHandle;
  if (*cause == CC_Unassigned)
    CLog::Info(THISMODULE, task, "Cause code hasn't been assigned when retrieved from call %s (handle=%d)",call ? call->GetName() : "(null)",handle);
  return OK;
}

bool CallAPI::IsStateReadyForSignallingConnect(CallState state)
{
  switch (state) {
    case CS_Null:
    case CS_Disconnected:
    case CS_Clearing:
    case CS_Releasing:
      return false;

    default:
      return true;
  }
}

STATUS CallAPI::SignallingConnect(CTask* task,int* retcode,int handle1,int handle2)
{
  //printf("Entering SignallingConnect \n");
  /** May only be called if the following conditions are met:
    - one is incoming call and the other is outgoing call
    - none is already connected
    - both states are compatible
    */
  *retcode = CRC_InvalidCallHandle;
  Call* call1 = _manager->CallFromHandle(task,handle1,true,"CallSignallingConnect");
  Call* call2 = _manager->CallFromHandle(task,handle2,true,"CallSignallingConnect");

  // check call compatibility
  if (!call1 || !call2 || (call1->GetDirection() == call2->GetDirection() && 
        call1->GetCause() != CC_ECT && call2->GetCause() != CC_ECT)) {
    const char* name1 = call1 ? call1->GetName() : "(null)";
    const char* name2 = call2 ? call2->GetName() : "(null)";
    CLog::Warning(THISMODULE, task, "Unable to connect signalling, incompatible calls %s and %s",name1,name2);
    /*printf("Unable to connect signalling, incompatible calls %s and %s\n",name1,name2); */
    return OK;
  }

  // Ignore the SignallingConnect request if both calls are already connected
  // to each other.
  if (call1->_peerCall == call2 && call2->_peerCall == call1) {
    CLog::Debug (THISMODULE, task, "CallSignallingConnect %d to %d: already connected", handle1, handle2);
    return OK;
  }

  // check allowed state for the first call
  if (!IsStateReadyForSignallingConnect(call1->GetState())) {
    CLog::Warning(THISMODULE, task, "Call %s has an incompatible state (%s) for signalling connect",call1->GetName(),StrCallState(call1->GetState()));
    return OK;
  }

  // check allowed state for the second call
  if (!IsStateReadyForSignallingConnect(call2->GetState())) {
    CLog::Warning(THISMODULE, task, "Call %s has an incompatible state (%s) for signalling connect",call2->GetName(),StrCallState(call2->GetState()));
    /*printf("Call %s has an incompatible state (%s) for signalling connect\n",call2->GetName(),StrCallState(call2->GetState())); */
    return OK;
  }

  // check if the first call is already connected to another
  if (call1->_peerCall) {
    CLog::Warning(THISMODULE, task, "Call %s is already connected to %s",call1->GetName(),call1->_peerCall->GetName());
    /* printf("Call %s is already connected to %s\n",call1->GetName(),call1->_peerCall->GetName()); */
    return OK;
  }

  // check if the second call is already connected to another
  if (call2->_peerCall) {
    CLog::Warning(THISMODULE, task, "Call %s is already connected to %s",call2->GetName(),call2->_peerCall->GetName());
    /*printf("Call %s is already connected to %s\n",call2->GetName(),call2->_peerCall->GetName()); */
    return OK;
  }

  Call* incall;
  Call* outcall;
  if (call1->GetDirection() == CD_Incoming) {
    incall = call1;
    outcall = call2;
  } else {
    outcall = call1;
    incall = call2;
  }

  if ((outcall->GetState() == CS_Overlap) && incall->_overlapRcvd.length()) {
    // send remaining overlap digits
    outcall->OverlapReq(incall->_overlapRcvd.c_str(),incall->GetCallParameter()->complete);
  }

  // if incoming call has not reached the state of outgoing call, 
  // progress the incoming call state to outgoing call state
  if (incall->GetState() < outcall->GetState()) {
    switch (outcall->GetState()) {
      case CS_Proceeding:
        incall->ProceedingReq(outcall->_lastProgress, 0);
        break;

      case CS_Alerting:
        incall->AlertingReq(outcall->_lastProgress, 0);
        break;

      case CS_Connected:
        incall->ConnectReq(0);
        break;

      case CS_Suspended:
        incall->SuspendReq();
        break;
    }
  }

  call1->_peerCall = call2;
  call2->_peerCall = call1;

  *retcode = CRC_NoError;

  //TBD: transfer overlap digits collected so far
  return OK;
}


STATUS CallAPI::SignallingDisconnect(CTask* task,int* retcode,int handle1,int handle2)
{
  /** May only be called if both calls are connected
  */
  *retcode = CRC_InvalidCallHandle;
  Call* call1 = _manager->CallFromHandle(task,handle1,true,"CallSignallingDisconnect");
  Call* call2 = _manager->CallFromHandle(task,handle2,true,"CallSignallingDisconnect");
  if (call1 && call2 && 
      call1->_peerCall == call2 &&
      call2->_peerCall == call1) {

    call1->_peerCall = NULL;
    call2->_peerCall = NULL;

    *retcode = CRC_NoError;
  }
  return OK;
}


STATUS CallAPI::GetDirection(CTask* task,CallDirection* direction,int handle)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallGetDirection");
  *direction = (call) ? call->GetDirection() : (CallDirection)CRC_InvalidCallHandle;
  return OK;
}


STATUS CallAPI::GetState(CTask* task,CallState* state,int handle)
{
  Call* call = _manager->CallFromHandle(task,handle,false,"CallGetState");
  if (call) {
    if (call->_released) *state = CS_Disconnected;
    else *state = call->GetState();
  } else {
    *state = CS_Null;
  }
  return OK;
}


STATUS CallAPI::GetFirstCallHandle(CTask* task,int* handle)
{
  return OK;
}


STATUS CallAPI::GetNextCallHandle(CTask* task,int* handle)
{
  return OK;
}


STATUS CallAPI::GetMedia(CTask* task,Media **media,int handle)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"CallGetMedia");
  *media = (call) ? call->GetMedia() : (Media*)CRC_InvalidCallHandle;
  //CLog::Detail(task, "Media= %d", *media);
  return OK;
}


STATUS CallAPI::GetMediaName(CTask* task,int* retcode,int handle,char* group,char* name)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"GetMediaName");
  *group = '\0';
  *name = '\0';
  if (call) {
    Media* media = call->GetMedia();

    if (media) {
      safe_strcpy(group,call->GetGroupName(),MAXAPPSTRLEN);
      safe_strcpy(name,media->GetName(),MAXAPPSTRLEN);
      *retcode = CRC_NoError;
    } else {
      *retcode = CRC_InvalidMedia;
    }

  } else {
    *retcode = CRC_InvalidCallHandle;
  }
  return OK;
}

STATUS CallAPI::GetInterfaceName(CTask* task,char* retval,int handle)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"GetInterfaceName");
  *retval = '\0';
  if (call) safe_strcpy(retval,call->GetInterfaceName(),MAXAPPSTRLEN);
  return OK;
}

STATUS CallAPI::GetGroupName(CTask* task,char* retval,int handle)
{
  Call* call = _manager->CallFromHandle(task,handle,true,"GetGroupName");
  *retval = '\0';
  if (call) safe_strcpy(retval,call->GetGroupName(),MAXAPPSTRLEN);
  return OK;
}

STATUS CallAPI::CallReleaseMedia(CTask* task, int* retval, int handle)
{
  Call* call = _manager->CallFromHandle(task, handle, true, "CallReleaseMedia");
  *retval = -1;
  if (call) {
    if (call->SupportReleaseMediaMidCall()) {
      *retval = 0;
      call->DetachMedia(0, 0);
    } else {
      CLog::Warning(THISMODULE, task, "Call %s doesn't support release media mid-call", call->GetName());
    }
  }
  return OK;
}

STATUS API_CALL CallAPI::CreateFilePath(CTask *task, int *retval, char *filepath)
{
  printf("CallAPI::CreateFilePath ... \n");
  *retval = mkdir(filepath, 0777);
  return OK;
}

STATUS API_CALL CallAPI::SetHostInfo(CTask *task, char *conf_id, int status)
{
  printf("CallAPI::SetHostInfo... \n");
  host_info[conf_id] = status ;
}


STATUS API_CALL CallAPI::GetHostInfo(CTask *task, int *retval, char *conf_id)
{
  printf("CallAPI::GetHostInfo... \n");
  if (host_info[conf_id] == 1)
  {
    *retval = 1;
  } else {
    *retval = 0;
  }
  return OK;
}


#ifdef XGATE_HAS_HMP

Call * CallAPI::GetHmpCallObject(CTask* task, int handle)
{
  Call* call = NULL;
  call = _manager->CallFromHandle(task,handle,true,"HmpGetDtmfDigits");

  //patch for Hmp_Dtmf_Collection_From_New_Task_Spawned_For_LegB - Start
  if(!call)
  {
    //it might called from child task for
    //dtmf digit collection on legB so try below
    TaskCallBlock *current_tcb = _manager->GetTaskCallBlock(task);
    call = current_tcb->CallFromHandle(handle);
    CLog::Debug(THISMODULE, task, "HmpGetDtmfDigits: call object taken in child task for handle %d\n", handle);
  }
  //patch for Hmp_Dtmf_Collection_From_New_Task_Spawned_For_LegB - End
  return call;
}

double TimeSpecToSeconds(struct timespec* ts)
{
  return (double)ts->tv_sec + (double)ts->tv_nsec / 1000000000.0;
}

#if 0
STATUS CallAPI::HmpGetDtmfDigits(CTask* task, int handle, int number_of_digits, int wait_time, char* dtmf_digits)
{
  cout << "****** Inside HmpGetDtmfDigits  **************** " << endl;
  Call* call = _manager->CallFromHandle(task,handle,true,"HmpGetDtmfDigits");
  if(call)
  {
    Rtp_Media *rtpMedia = dynamic_cast<Rtp_Media*>(call->GetMedia());
    if(rtpMedia)
    {
      rtpMedia->GetDtmfDigit(number_of_digits, wait_time, dtmf_digits);
      CLog::Error(THISMODULE, task, "HmpGetDtmfDigits dtmf digit captured is: %s ", dtmf_digits);
      //      printf("\n HmpGetDtmfDigits dtmf digit captured is: %s ", dtmf_digits);
      return OK;
    }
  }  else {
    CLog::Error(THISMODULE, task, "\nHmpGetDtmfDigits: Invalid call object for handle");
    cout << "****** HmpGetDtmfDigits: Invalid call object for handle  **************** " << endl;
    return NOT_OK;
  }
}
#else

//STATUS CallAPI::HmpGetDtmfDigits(CTask* task, int handle, int* retval, char* dtmf_digits)
STATUS CallAPI::HmpGetDtmfDigits(CTask* task, int handle, int* retval, char* dtmf_digits, int mindigits)
{
  CLog::Detail(THISMODULE, task, "HmpGetDtmfDigits: Inside HmpGetDtmfDigits()... ; mindigits = %d\n", mindigits);
  Call* call_hmp =  GetHmpCallObject(task, handle);

/*****************
   xGateCallInfo* call =  GetHmpCallObject(task, handle);
  if(call)
  {
        // multithreaded-ippbx code added : Request gstreamer to collect dtmf digits.
        String temp_dtmf_digits = "";
        temp_dtmf_digits[0] = '\0';
        CLog::Detail(THISMODULE, task, "\nHmpGetDtmfDigits: strlen(temp_dtmf_digits.c_str() = %d", strlen(temp_dtmf_digits.c_str()));

        CallParameter *cp = call->GetCallParameter();
        CLog::Detail(THISMODULE, task, "\nHmpGetDtmfDigits: max_timeout = %d", cp->max_timeout); //cp->max_timeout: set by HmpSetDigitTimeout.

        struct timespec ts;
        int rc = clock_gettime(CLOCK_MONOTONIC, &ts);
        CLog::Detail(THISMODULE, "HmpGetDtmfDigits::dtmf_set_timeout TimeSpecToSeconds(&ts) =  [%f]\n", TimeSpecToSeconds(&ts));

        double m_dtmfTimeout = TimeSpecToSeconds(&ts) + (double)cp->max_timeout;
        CLog::Detail(THISMODULE, "xGateHmpGstPipeline::dtmf_set_timeout calculated elapsed time is: [%f]\n", m_dtmfTimeout);

        while ((strlen(temp_dtmf_digits.c_str()) < mindigits) && (TimeSpecToSeconds(&ts) < m_dtmfTimeout))
        {
                // Suspend this task until dtmf-digits are collected and sent back by sip-info / gstreamer-thread.
                CLog::Detail(THISMODULE, task, "HmpGetDtmfDigits: task has suspended...\n");

                CallManager::Instance()->SuspendOwnerTask(call);

                task->Sleep(1);

                // on resume, this task is supposed to start executing /resuming from here...
                CLog::Detail(THISMODULE, task, "HmpGetDtmfDigits: task has resumed...\n");

                xGateCallObj *obj = ReadFromProcessQueue_lower();

                if(obj){
                        CLog::Detail(THISMODULE, "HmpGetDtmfDigits: digit-collected = %s", obj->cp.dtmf_digits.c_str());
                        ////if (obj->cp.dtmf_digits[0] == '\255')
                        ////    call->PostMessageToReceiver(call->GetState(),call->GetCallParameter()->call_id,(call->GetCallParameter()),XGATESIPEVENT::EN_MEDIA_GET_DTMFDIGITS_REQ);
                        temp_dtmf_digits.push_back(obj->cp.dtmf_digits[0]);
                        delete obj;
                }
                rc = clock_gettime(CLOCK_MONOTONIC, &ts);
        }

        safe_strcpy(dtmf_digits, (const char*) temp_dtmf_digits.c_str(), MAXAPPSTRLEN);

        // *retval = 0 => desired max no. of dtmf-digits collected before wait-time elapsed.
        // *retval = 1 => desired max no. of dtmf-digits NOT collected before wait-time elapsed.
        (strlen(temp_dtmf_digits.c_str()) < mindigits) ? *retval = 1 : *retval = 0;

        CLog::Detail(THISMODULE, task, "HmpGetDtmfDigits: exiting... ; retval = %d , temp_dtmf_digits = %s", *retval, temp_dtmf_digits.c_str());
        CallManager::Instance()->ResumeOwnerTask(call);

        return OK;
        // multithreaded-ippbx code ends
  }else {
    CLog::Error(THISMODULE, task, "HmpGetDtmfDigits: Invalid call object for handle\n");
    cout << "****** HmpGetDtmfDigits: Invalid call object for handle  **************** " << endl;
    CLog::Detail(THISMODULE, "****** HmpGetDtmfDigits: Invalid call object for handle  **************** \n");
    return NOT_OK;
  }
*************/
}


#endif 


#if 0
//STATUS API_CALL CallAPI::HmpPlayFile(CTask *task, int handle, char *filename)
STATUS API_CALL CallAPI::HmpPlayFile(CTask *task, int handle, char *filename, bool autorepeat)
{
  //	cout << " ***** Inside HmpPlayFile ****** "  << endl; 
#if 1
  const char *destAddr;
  Call* call = _manager->CallFromHandle(task, handle, true, "HmpPlayFile");

  //     printf("\n CallAPI::HmpPlayFile: Playing filename=%s\n", filename);
  if(call){
    //  tmpCall = call;
    destAddr = call->GetCallParameter()->csaCalling.c_str();
    //       cout << "****************** destAddr is " << destAddr  << "***************" << endl;
    Rtp_Media *rtpMedia = dynamic_cast<Rtp_Media*>(call->GetMedia());
    if (rtpMedia){
      //         printf("\n CallAPI::HmpPlayFile: destAddr : %s , Playing filename=%s\n", destAddr, filename);
      CLog::Detail(THISMODULE,task,"\n HmpPlayFile: Playing filename=%s\n", filename);
      //        rtpMedia->PlayFileWav(filename, false);
      rtpMedia->PlayFileWav(filename, true,call);
      // rtpMedia->PlayFileWav(filename, destAddr, true);
      call->SuspendTask();        
      printf("********** After SuspendTask() ********\n");
      return OK;
    }
  }  else {
    CLog::Error(THISMODULE, task, "\n HmpPlayFile: Invalid call object for handle");
    return NOT_OK;
  }
#endif 
}

#else


STATUS API_CALL CallAPI::HmpPlayFile(CTask *task, int handle, char *filename, bool autorepeat)
{
#if 0
  xGateCallInfo* call =  GetHmpCallObject(task, handle);
  if(call){
        // multithreaded-ippbx code added : Request gstreamer to play file.
        //(call->GetCallParameter())->csfilename = filename;
        (call->GetCallParameter())->csfilename.assign(filename,strlen(filename));
        call->PostMessageToReceiver(call->GetState(),call->GetCallParameter()->call_id,*(call->GetCallParameter()),XGATESIPEVENT::EN_MEDIA_PLAYFILE_REQ);
        return OK;
        // multithreaded-ippbx code ends
  }  else {
    CLog::Error(THISMODULE, task, "HmpPlayFile: Invalid call object for handle\n");
    return NOT_OK;
  }
#endif
return OK;
}

STATUS API_CALL CallAPI::HmpSwbPlayFile(CTask *task, CallParameter* cparam, char *filename, bool autorepeat, int callstate)
{
	/************
  xGateCallInfo* call =  GetHmpCallObject(task, handle);
  if(call){
        // multithreaded-ippbx code added : Request gstreamer to play file.
        ////(call->GetCallParameter())->csfilename.assign(filename,strlen(filename));
        if(filename && strlen(filename) > 0)
        (call->GetCallParameter())->swbinfo.PlayFile = filename;
	else 
	(call->GetCallParameter())->swbinfo.PlayFile = "";
	call->GetCallParameter()->swbinfo.DigitReceived = "$$$";
        (call->GetCallParameter())->swbinfo.SwbCallEvent = callstate;
        call->PostMessageToReceiver(call->GetState(),call->GetCallParameter()->call_id,(call->GetCallParameter()),XGATESIPEVENT::EN_SWB_EVENT_REL);
	**************/
	//if (callstate == 4)
	//	CallManager::Instance()->SuspendOwnerTask(call);
		//task->Sleep(40);
#if 0
        xGateCallInfo* call = GetHmpCallObject(task, 0); // handle == 0 for h_legA for a switchboard-call.
  	if(call){
		 CallParameter *cp = call->GetCallParameter();
                cp  = cparam;
		//cp->swbinfo.PlayFile = "";
		cparam->swbinfo.SwbCallEvent = callstate;
    		//cp->swbinfo.maxDigits = mindigits; // should not be destroyed by this API.
		//cp->swbinfo.DigitTimeOut = timeout; // should not be destroyed by this API.
		cparam->swbinfo.DigitReceived = "";

        	if(filename && (strlen(filename) > 0))
        		cparam->swbinfo.PlayFile = filename;
		else 
			cparam->swbinfo.PlayFile = "";

    		call->PostMessageToReceiver(call->GetState(),cparam->call_id,*cparam,XGATESIPEVENT::EN_SWB_EVENT_REL);

        return OK;
        // multithreaded-ippbx code ends
  }  else {
    CLog::Error(THISMODULE, task, "HmpSwbPlayFile: Invalid call object for handle\n");
    return NOT_OK;
  }
#endif
return OK;
}
STATUS API_CALL CallAPI::HmpAttachTask(CTask *task, int handle)
{
  //  const char *destAddr;
  Call* call = GetHmpCallObject(task, handle);

  if(call){
    //   destAddr = call->GetCallParameter()->csaCalling.c_str();
    //  cout << "****************** destAddr is " << destAddr  << "***************" << endl;
    cout << "*********** InsideHmpAttachTask , call is not NULL ***************** " << endl;
    Rtp_Media *rtpMedia = dynamic_cast<Rtp_Media*>(call->GetMedia());
    if (rtpMedia){
      CLog::Detail(THISMODULE,task,"****  HmpAttachTask ****");
      //  rtpMedia->PlayFileWav(filename, autorepeat);
      rtpMedia->AttachTask(call);
      //      call->SuspendTask();
      return OK;
    }
  }  else {
    cout << "*********** InsideHmpAttachTask , call is NULL ***************** " << endl;
    CLog::Error(THISMODULE, task, "HmpAttachTaks: Invalid call object for handle\n");
    return NOT_OK;
  }
}

STATUS API_CALL CallAPI::SetConferenceFlag(CTask *task, int handle)
{

  Call* call = GetHmpCallObject(task, handle);
  if(call){
    call->GetCallParameter()->confFlag = true;
    cout << "*********** confFlag , ***************** " << call->GetCallParameter()->confFlag << endl;
  }else {
    cout << "*********** InsideSetConferenceFlag  , call is NULL ***************** " << endl;
    CLog::Error(THISMODULE, task, "SetConferenceFlag: Invalid call object for handle\n");
    return NOT_OK;
  }
}


#endif

STATUS API_CALL CallAPI::HmpStopPlayFile(CTask *task, int handle, int waitTime)
{
#if 0
  xGateCallInfo* call =  GetHmpCallObject(task, handle);
  if(call){
        // multithreaded-ippbx code added : Request gstreamer to stop playing file.
        //std::string callid =  call->GetCallParameter()->call_id;
        call->PostMessageToReceiver(call->GetState(),call->GetCallParameter()->call_id,*(call->GetCallParameter()),XGATESIPEVENT::EN_MEDIA_STOPPLAYFILE_REQ);

        CLog::Detail(THISMODULE, task, "HmpStopPlayFile: exiting...");
        return OK;
        // multithreaded-ippbx code ends
  }  else {
    CLog::Error(THISMODULE, task, "\n HmpStopPlayFile: Invalid call object for handle");
    return NOT_OK;
  }
#endif 
return OK;
}


STATUS API_CALL CallAPI::HmpPlayFileBkgnd(CTask *task, int handle, char *filename, bool autorepeat)
{
#if 0
  xGateCallInfo* call =  GetHmpCallObject(task, handle);
  if(call){
        // multithreaded-ippbx code added : Request gstreamer to play file.
        //(call->GetCallParameter())->csfilename = filename;
        (call->GetCallParameter())->csfilename.assign(filename,strlen(filename));
        call->PostMessageToReceiver(call->GetState(),call->GetCallParameter()->call_id,*(call->GetCallParameter()),XGATESIPEVENT::EN_MEDIA_PLAYFILEBKGND_REQ);

        CLog::Detail(THISMODULE, task, "HmpPlayFileBkgnd: exiting...");
        return OK;
        // multithreaded-ippbx code ends
  }  else {
    CLog::Error(THISMODULE, task, "\n HmpPlayFileBkgnd: Invalid call object for handle");
    return NOT_OK;
  }
#endif
return OK;
}


STATUS API_CALL CallAPI::HmpStopPlayFileBkgnd(CTask *task, int handle, int waitTime)
{
#if 0
  xGateCallInfo* call =  GetHmpCallObject(task, handle);
  if(call){
        // multithreaded-ippbx code added : Request gstreamer to stop playing file.
        std::string callid =  call->GetCallParameter()->call_id;
        call->PostMessageToReceiver(call->GetState(),callid,*(call->GetCallParameter()),XGATESIPEVENT::EN_MEDIA_STOPPLAYFILEBKGND_REQ);

        CLog::Detail(THISMODULE, task, "HmpStopPlayFileBkgnd: exiting...");
        return OK;
        // multithreaded-ippbx code ends
  }  else {
    CLog::Error(THISMODULE, task, "\n HmpStopPlayFileBkgnd: Invalid call object for handle");
    return NOT_OK;
  }
#endif
return OK;
}



STATUS API_CALL CallAPI::HmpGetDigits(CTask *task,int max_length,char *buffer)
{
  CLog::Debug(THISMODULE, task, "*** Inside HmpGetDigits API ***");
  return OK;
}

STATUS API_CALL CallAPI::HmpClearDigits(CTask *task)
{
  CLog::Debug(THISMODULE, task, "*** Inside HmpClearDigits API ***");
  return OK;
}

STATUS API_CALL CallAPI::HmpGetDigitTimeout(CTask *task,int *max_timeout)
{
  CLog::Debug(THISMODULE, task, "*** Inside HmpGetDigitTimeout API ***");
  return OK;
}

STATUS API_CALL CallAPI::HmpSetDigitTimeout(CTask *task, int handle, int max_timeout)
{
#if 0
  xGateCallInfo* call =  GetHmpCallObject(task, handle);
  if(call)
  {
    std::string callid =  call->GetCallParameter()->call_id;
    CallParameter *cp = call->GetCallParameter();
    //cp->PbState = EN_XGATE_DTMF_SETTIMEOUT;
    cp->max_timeout = max_timeout;
    CLog::Detail(THISMODULE, task, "\nHmpSetDigitTimeout: max_timeout = %d = %d = %d", max_timeout, cp->max_timeout, (call->GetCallParameter())->max_timeout);
    //call->PostMessageToReceiver(call->GetState(),callid,(call->GetCallParameter()),XGATESIPEVENT::EN_MEDIA_SETDTMF_TIMEOUT_REQ);
    return OK;
  }  else {
    CLog::Detail(THISMODULE, task, "\nHmpSetDigitTimeout: Invalid call object for handle");
    return NOT_OK;
  }
#endif
return OK;
}

STATUS API_CALL CallAPI::HmpGetInterDigitTimeout(CTask *task,int *max_timeout)
{
  CLog::Debug(THISMODULE, task, "*** Inside HmpGetInterDigitTimeout API ***");
  return OK;
}

STATUS API_CALL CallAPI::HmpSetInterDigitTimeout(CTask *task,int max_timeout)
{
  CLog::Debug(THISMODULE, task, "*** Inside HmpSetInterDigitTimeout API ***");
  return OK;
}



STATUS API_CALL CallAPI::HmpRecordFile(CTask *task, int handle1, int handle2, char *filename)
{

#if 1

#if 0
  CLog::Debug(THISMODULE, task, "*** Inside HmpRecordFile API ***");
  Call* call1 = _manager->CallFromHandle(task, handle1, true, "HmpRecordFile");
  if(!call1) {
    CLog::Error(THISMODULE, task, "HmpRecordFile: Invalid call object for handle1\n");
    return NOT_OK;
  }

  Call* call2 = _manager->CallFromHandle(task, handle2, true, "HmpRecordFile");
  if(!call2) {
    CLog::Error(THISMODULE, task, "HmpRecordFile: Invalid call object for handle2\n");
    return NOT_OK;
  }

  Rtp_Media *rtpMedia1 = dynamic_cast<Rtp_Media*>(call1->GetMedia());
  if(rtpMedia1) {
    Rtp_Media *rtpMedia2 = dynamic_cast<Rtp_Media*>(call2->GetMedia());
    if(rtpMedia2) {
      CLog::Debug(THISMODULE,task,"HmpRecordFile: recording call in filepath=%s\n", filename);
      rtpMedia1->RecordFile(filename, rtpMedia2);
      return OK;
    }
  }
  return NOT_OK;
#endif


  CLog::Debug(THISMODULE, task, "*** Inside HmpRecordFile API ***");
  cout << "************ Inside HmpRecordFile API **************" << endl;
  Call* call1 = GetHmpCallObject(task, handle1);
  if(!call1) {
    CLog::Detail(THISMODULE, task, "HmpRecordFile: Invalid call object for handle1\n");
    return NOT_OK;
  }

  Call* call2 = GetHmpCallObject(task, handle2);
  if(!call2) {
    CLog::Detail(THISMODULE, task, "HmpRecordFile: Invalid call object for handle2\n");
    return NOT_OK;
  }

  Rtp_Media *rtpMedia1 = dynamic_cast<Rtp_Media*>(call1->GetMedia());
  if(rtpMedia1) {
    Rtp_Media *rtpMedia2 = dynamic_cast<Rtp_Media*>(call2->GetMedia());
    if(rtpMedia2) {
      CLog::Detail(THISMODULE,task,"HmpRecordFile: recording call in filepath=%s\n", filename);
      rtpMedia1->RecordFile(filename, rtpMedia2);
      return OK;
    }
  }
  return NOT_OK;


#else

  cout << "************ Inside HmpRecordFile **************** " << endl;

#endif
}


STATUS API_CALL CallAPI::HmpEndRecordFile(CTask *task, int handle)
{

#if 0
  CLog::Debug(THISMODULE, task, "*** Inside HmpEndRecordFile API ***");
  Call* call = _manager->CallFromHandle(task, handle, true, "HmpRecordFile");
  if(!call) {
    CLog::Error(THISMODULE, task, "HmpRecordFile: Invalid call object for handle\n");
    return NOT_OK;
  }
  Rtp_Media *rtpMedia = dynamic_cast<Rtp_Media*>(call->GetMedia());
  if(rtpMedia) {
    rtpMedia->EndRecordFile();
    return OK;
  }
  return NOT_OK;

#else

  cout << "************ Inside HmpEndRecordFile **************** " << endl;
  CLog::Debug(THISMODULE, task, "*** Inside HmpEndRecordFile API ***");
  Call* call = GetHmpCallObject(task, handle);
  if(!call) {
    CLog::Error(THISMODULE, task, "HmpRecordFile: Invalid call object for handle\n");
    return NOT_OK;
  }

  Rtp_Media *rtpMedia = dynamic_cast<Rtp_Media*>(call->GetMedia());
  if(rtpMedia) {
    rtpMedia->EndRecordFile();
    return OK;
  }
  return NOT_OK;


#endif
}


STATUS API_CALL CallAPI::HmpConfCreateRoom(CTask* task, int* room_handle, char* room_name, int room_size, int handle)
{
  CLog::Debug(THISMODULE, task, "*** Inside HmpConfCreateRoom API ***");
  *room_handle = -1;
  if(room_name == NULL || room_name[0] == '\0') {
    CLog::Error(THISMODULE, task, "HmpConfCreateRoom: Invalid room name %s\n", room_name);
    return NOT_OK;
  }
  if(room_size <= 0) {
    CLog::Error(THISMODULE, task, "HmpConfCreateRoom: Invalid room size %d\n", room_size);
    return NOT_OK;
  }
  //get call object from handle and check
  Call* call = GetHmpCallObject(task, handle);
  if(!call) {
    CLog::Error(THISMODULE, task, "HmpConfCreateRoom: Invalid call object for handle\n");
    return NOT_OK;
  }

  Rtp_Media *rtpMedia = dynamic_cast<Rtp_Media*>(call->GetMedia());
  if(rtpMedia) {
    //find conf room already been created for given room_name
    HmpConferenceRoom *confRoom = NULL;
    confRoom = _manager->GetConfRoomFromMap(room_name);
    //if no then create new conference and do rest
    if(confRoom) {
      CLog::Debug(THISMODULE, task, "HmpConfCreateRoom: conference room %s has already been created\n", room_name);
    } else {
      confRoom = new HmpConferenceRoom(room_name, room_size);
      CLog::Debug(THISMODULE, task, "HmpConfCreateRoom: conference room %s is newly created\n", room_name);
    }

    if(rtpMedia->ConfCreateRoom(room_name)) {
      if(rtpMedia->ConfAddParty(room_name)) {
        //confRoom->InsertMember(rtpMedia);
        confRoom->increaseMembersCount();
        *room_handle = confRoom->GetHandle();
        _manager->AddConfRoomInMap(room_name, confRoom);
        return OK;
      }
    } else {
      CLog::Error(THISMODULE, task, "HmpConfCreateRoom: problem in creating pipeline for conference room: %s\n", room_name);
      if(confRoom->GetMembersCount() <= 0) {
        _manager->RemoveConfRoomFromMap(room_name);
        delete confRoom;
      }
    }
  }
  return NOT_OK;
}

STATUS API_CALL CallAPI::HmpConfDeleteRoom(CTask* task, char* room_name, int handle)
{
  CLog::Debug(THISMODULE, task, "*** Inside HmpConfDeleteRoom API ***");
  if(room_name == NULL || room_name[0] == '\0') {
    CLog::Error(THISMODULE, task, "HmpConfDeleteRoom: Invalid room name %s\n", room_name);
    return NOT_OK;
  }
  //get call object from handle and check
  Call* call = GetHmpCallObject(task, handle);
  if(!call) {
    CLog::Error(THISMODULE, task, "HmpConfDeleteRoom: Invalid call object for handle\n");
    return NOT_OK;
  }
  //get conf room from list
  HmpConferenceRoom *confRoom = NULL;
  confRoom = _manager->GetConfRoomFromMap(room_name);

  if(confRoom) {
    Rtp_Media *rtpMedia = dynamic_cast<Rtp_Media*>(call->GetMedia());
    if(rtpMedia) {
      _manager->RemoveConfRoomFromMap(room_name);
      delete confRoom;
      //Note: no need to given intimation to HMP.
      //HMP will get intimation from Sofia once user send BYE request.
      return OK;
    }
  } else {
    CLog::Debug(THISMODULE, task, "HmpConfDeleteRoom: conference room %s not available\n", room_name);
    return NOT_OK;
  }
  return OK;
}

/* E_LINE_DROP exception from SL will call this function.
   At this point we are not able to get valid room handle
   since call object has already been destroyed by Sofia call Disconnect event*/
STATUS API_CALL CallAPI::HmpConfRemoveParty(CTask* task, char* room_name, int handle)
{
  CLog::Debug(THISMODULE, task, "*** Inside HmpConfRemovePary API ***");
  if(room_name == NULL || room_name[0] == '\0') {
    CLog::Error(THISMODULE, task, "HmpConfRemoveParty: Invalid room name %s\n", room_name);
    return NOT_OK;
  }

  //get conf room from list
  HmpConferenceRoom *confRoom = NULL;
  confRoom = _manager->GetConfRoomFromMap(room_name);

  if(confRoom) {
    confRoom->decreaseMembersCount();
    if(confRoom->GetMembersCount() <= 0) {
      _manager->RemoveConfRoomFromMap(room_name);
      delete confRoom;
      //Note: no need to given intimation to HMP.
      //HMP will get intimation from Sofia once user send BYE request.
      return OK;
    }
  } else {
    CLog::Debug(THISMODULE, task, "HmpConfRemoveParty: conference room %s not available\n", room_name);
    return NOT_OK;
  }
  return OK;
}

STATUS API_CALL CallAPI::HmpConfRecordFile(CTask* task, char* room_name, int handle, char* file_path)
{
  CLog::Debug(THISMODULE, task, "*** Inside HmpConfRecordFile API ***");
  if(room_name == NULL || room_name[0] == '\0') {
    CLog::Detail(THISMODULE, task, "HmpConfRecordFile: Invalid room name %s\n", room_name);
    return NOT_OK;
  }
  //get call object from handle and check
  Call* call = GetHmpCallObject(task, handle);
  if(!call) {
    CLog::Detail(THISMODULE, task, "HmpConfRecordFile: Invalid call object for handle\n");
    return NOT_OK;
  }

  HmpConferenceRoom *confRoom = NULL;
  confRoom = _manager->GetConfRoomFromMap(room_name);
  CLog::Detail(THISMODULE, task, "HmpConfRecordFile: confRoom = %02x \n", confRoom);
  if(confRoom) {
    Rtp_Media *rtpMedia = dynamic_cast<Rtp_Media*>(call->GetMedia());
    CLog::Detail(THISMODULE, task, "HmpConfRecordFile: rtpMedia = %02x \n", rtpMedia);
    if(rtpMedia) {
      rtpMedia->ConfRecordFile(room_name, file_path);
      return OK;
    }
  }
  return NOT_OK;
}

STATUS API_CALL CallAPI::HmpConfEndRecordFile(CTask* task, char* room_name, int handle)
{
  CLog::Debug(THISMODULE, task, "*** Inside HmpConfEndRecordFile API ***");
  if(room_name == NULL || room_name[0] == '\0') {
    CLog::Detail(THISMODULE, task, "HmpConfEndRecordFile: Invalid room name %s\n", room_name);
    return NOT_OK;
  }
  Call* call = GetHmpCallObject(task, handle);
  if(!call) {
    CLog::Detail(THISMODULE, task, "HmpConfEndRecordFile: Invalid call object for handle\n");
    return NOT_OK;
  }

  HmpConferenceRoom *confRoom = NULL;
  confRoom = _manager->GetConfRoomFromMap(room_name);
  CLog::Detail(THISMODULE, task, "HmpConfEndRecordFile: confRoom = %02x \n", confRoom);
  if(confRoom) {
    Rtp_Media *rtpMedia = dynamic_cast<Rtp_Media*>(call->GetMedia());
    CLog::Detail(THISMODULE, task, "HmpConfEndRecordFile: rtpMedia = %02x \n", rtpMedia);
    if(rtpMedia) {
      rtpMedia->ConfEndRecordFile(room_name);
      return OK;
    }
  }

  return NOT_OK;
}

STATUS API_CALL CallAPI::HmpConfFindRoom(CTask* task, int *retval, char* room_name)
{
  CLog::Debug(THISMODULE, task, "*** Inside HmpConfFindRoom API ***");
  *retval = 0;
  return NOT_OK;
}

STATUS API_CALL CallAPI::SetMappedUserNumber(CTask* task, CallParameter* cparam, const char* mappedUserKey, const char* value)
{
  if (cparam && !cparam->IsValid()) {
    CLog::Warning(THISMODULE,task,"CallAPI::SetMappedUserNumber, Invalid call parameter specified");
    return OK;
  }
  //safe_strcpy(retval, cparam->routeAddr.c_str(), MAXAPPSTRLEN);

  //find out map entry already been created for this mappedUserKey
  MappedUser *mappedUser = NULL;
  std::map<std::string,  MappedUser*>::iterator it;
  it = cparam->m_mappedUserList.find(mappedUserKey);
  if(it != cparam->m_mappedUserList.end()) { //entry available for this mappedUserKey
    it->second->m_userNumber = value;
  } else { //entry not available for this mappedUserKey
    //create new entry
    mappedUser = new MappedUser();
    mappedUser->m_userNumber = value;
    cparam->m_mappedUserList[mappedUserKey] = mappedUser;
  }

  return OK;
}
STATUS API_CALL CallAPI::SetMappedUserProxyName(CTask* task, CallParameter* cparam, const char* mappedUserKey, const char* value)
{
  if (cparam && !cparam->IsValid()) {
    CLog::Warning(THISMODULE,task,"CallAPI::SetMappedUserProxyName, Invalid call parameter specified");
    return OK;
  }
  //safe_strcpy(retval, cparam->routeAddr.c_str(), MAXAPPSTRLEN);

  //find out map entry already been created for this mappedUserKey
  MappedUser *mappedUser = NULL;
  std::map<std::string,  MappedUser*>::iterator it;
  it = cparam->m_mappedUserList.find(mappedUserKey);

  if(it != cparam->m_mappedUserList.end()) {
    //entry available for this mappedUserKey
    mappedUser = (MappedUser *)it->second;
    mappedUser->m_proxyUserName = value;
  } else {
    //entry not available, create new one
    mappedUser = new MappedUser();
    mappedUser->m_proxyUserName = value;
    cparam->m_mappedUserList[mappedUserKey] = mappedUser;
  }

  return OK;
}
STATUS API_CALL CallAPI::SetMappedUserSignalAddress(CTask* task,CallParameter* cparam, const char* mappedUserKey, const char* value)
{
  if (cparam && !cparam->IsValid()) {
    CLog::Warning(THISMODULE,task,"CallAPI::SetMappedUserSignalAddress, Invalid call parameter specified");
    return OK;
  }
  //safe_strcpy(retval, cparam->routeAddr.c_str(), MAXAPPSTRLEN);

  //find out map entry already been created for this mappedUserKey
  MappedUser *mappedUser = NULL;
  std::map<std::string,  MappedUser*>::iterator it;
  it = cparam->m_mappedUserList.find(mappedUserKey);

  if(it != cparam->m_mappedUserList.end()) {
    //entry available for this mappedUserKey
    it->second->m_userSignalAddr = value;
  } else {
    //entry not available, create new one
    mappedUser = new MappedUser();
    mappedUser->m_userSignalAddr = value;
    cparam->m_mappedUserList[mappedUserKey] = mappedUser;
  }

  return OK;
}
STATUS API_CALL CallAPI::SetMappedUserOutInterface(CTask* task,CallParameter* cparam, const char* mappedUserKey, const char* value)
{
  if (cparam && !cparam->IsValid()) {
    CLog::Warning(THISMODULE,task,"CallAPI::SetMappedUserOutInterface, Invalid call parameter specified");
    return OK;
  }
  //safe_strcpy(retval, cparam->routeAddr.c_str(), MAXAPPSTRLEN);

  //find out map entry already been created for this mappedUserKey
  MappedUser *mappedUser = NULL;
  std::map<std::string,  MappedUser*>::iterator it;
  it = cparam->m_mappedUserList.find(mappedUserKey);

  if(it != cparam->m_mappedUserList.end()) {
    //entry available for this mappedUserKey
    it->second->m_outInterface = value;
  } else {
    //entry not available, create new one
    mappedUser = new MappedUser();
    mappedUser->m_outInterface = value;
    cparam->m_mappedUserList[mappedUserKey] = mappedUser;
  }

  return OK;
}
STATUS API_CALL CallAPI::SetMappedUserDeviceType(CTask* task,CallParameter* cparam, const char* mappedUserKey, const char* value)
{
  if (cparam && !cparam->IsValid()) {
    CLog::Warning(THISMODULE,task,"CallAPI::SetMappedUserDeviceType, Invalid call parameter specified");
    return OK;
  }
  //safe_strcpy(retval, cparam->routeAddr.c_str(), MAXAPPSTRLEN);

  //find out map entry already been created for this mappedUserKey
  MappedUser *mappedUser = NULL;
  std::map<std::string,  MappedUser*>::iterator it;
  it = cparam->m_mappedUserList.find(mappedUserKey);

  if(it != cparam->m_mappedUserList.end()) {
    //entry available for this mappedUserKey
    it->second->m_calledDeviceType = value;
    if(strcmp(value, "PSTN")==0) {
      it->second->m_userDeviceType = XGATE_UT_PSTN;
      it->second->m_calledcodec =  8;   
    } else if(strcmp(value, "URDESK:")==0){
      it->second->m_userDeviceType = XGATE_UT_DESKPHONE;
      it->second->m_calledcodec =  8;
    } else if((strcmp(value, "URAPP:IOS")==0) || (strcmp(value, "URAPP:AND")==0)) {
      it->second->m_userDeviceType = XGATE_UT_URAPP;
      it->second->m_calledcodec =  111;
    } else if(strcmp(value, "URWEB:WEB")==0){
      it->second->m_userDeviceType = XGATE_UT_URWEB;
      it->second->m_calledcodec =  111;
    } else {
      it->second->m_userDeviceType = XGATE_UT_DESKPHONE;
      it->second->m_calledcodec =  8;
    }
  } else {
    //entry not available, create new one
    mappedUser = new MappedUser();
    it->second->m_calledDeviceType = value;
    if(strcmp(value, "PSTN")==0) {
      it->second->m_userDeviceType = XGATE_UT_PSTN;
      it->second->m_calledcodec =  8;
    } else if(strcmp(value, "URDESK")==0){
      it->second->m_userDeviceType = XGATE_UT_DESKPHONE;
      it->second->m_calledcodec =  8;
    } else if((strcmp(value, "URAPP:IOS")==0) || (strcmp(value, "URAPP:AND")==0)) {
      it->second->m_userDeviceType = XGATE_UT_URAPP;
      it->second->m_calledcodec =  111;
    } else if(strcmp(value, "URWEB:WEB")==0){
      it->second->m_userDeviceType = XGATE_UT_URWEB;
      it->second->m_calledcodec =  111;
    } else {
      it->second->m_userDeviceType = XGATE_UT_DESKPHONE;
      it->second->m_calledcodec =  8;
    }
    cparam->m_mappedUserList[mappedUserKey] = mappedUser;
  }


  return OK;
}

//TODO: 
//1. better if we could get device type as enum instead of char
//2. instead of device and app type together, get app type in separate enum input variable
STATUS API_CALL CallAPI::SendAppNotification(CTask* task, const char* from, const char* to, const char* user_cli, const char* device_type,const char* call_type)
{
  char sys_cmd[800];
  CLog::Warning(THISMODULE,task,"SendAppNotification from: %s to: %s for call_type: %s and device_type: %s", from, to, call_type, device_type);
  if(device_type ){
  if(strstr(device_type, "DCAPP:IOS")) {
    sprintf(sys_cmd, "curl --max-time 1 'http://gocheapappapi.mundio.com/v1/user/Gcpushnotification?origin=%s&target=%s&isvoip=true&media=%s&calltype=true&callmode=ps&iscancel=0'", from, to, call_type);
  } else if(strstr(device_type, "DCAPP:AND")) {
    sprintf(sys_cmd,"curl --max-time 1 'http://im01.delightcalling.com/wakeup.php?origin=%s&target=%s&calltype=%s'", from, to, call_type);
  } else if(strstr(device_type, "URAPP:IOS")) { //TODO: RestAPI need to be developed, request given by Romald, once given change url
    //sprintf(sys_cmd, "curl --max-time 1 'http://gocheapappapi.mundio.com/v1/user/Gcpushnotification?origin=%s&target=%s&isvoip=true&media=%s&calltype=true&callmode=ps&iscancel=0'", from, to, call_type);
    sprintf(sys_cmd, "curl --max-time 1 'http://urmyaccount.mundio.com/v1/user/urpushnotification?origin=%s&target=%s&isvoip=true&media=audio&iscancel=0&isLive=true'", from, to);
  } else if(strstr(device_type, "URAPP:AND")) { //TODO: RestAPI need to be developed, request given by Romald, once given change url
    //sprintf(sys_cmd,"curl --max-time 1 'http://im01.delightcalling.com/wakeup.php?origin=%s&target=%s&calltype=%s'", from, to, call_type);
    sprintf(sys_cmd,"curl --max-time 1 'http://82.113.74.51/ejabberd/wakeup.php?target=%s&origin=%s'", to, from);
  } else if (strstr(device_type, "VAPP:IOS")) {
    sprintf(sys_cmd,"curl --max-time 1 'http://vectoneapp.mundio.com/api/v1/Vectoneapppushnotification?origin=%s&target=%s&usercli=%s&isvoip=true&media=%s&iscancel=0'", from, to, user_cli, call_type);
  } else if(strstr(device_type, "VAPP:AND")) {
    sprintf(sys_cmd,"curl --max-time 1 'http://im01.vectone.com/vectone/wakeup1.php?origin=%s&target=%s&usercli=%s'", from, to, user_cli);
  }
  system(sys_cmd);
  }
 CLog::Detail(THISMODULE,"Push Norification %s", sys_cmd);
return OK;

}

#endif 
