/*----------------------------------------------------------------* 
 * $Archive: /xGate/callmanager.h $
 * $Date: 1/09/04 6:17p $
 * $Revision: 39 $
 *
 * $History: callmanager.h $
 * 
 * *****************  Version 39  *****************
 * User: Bennylp      Date: 1/09/04    Time: 6:17p
 * Updated in $/xGate
 * Moved some class methods to callmanager.cpp
 * 
 * *****************  Version 38  *****************
 * User: Bennylp      Date: 3/06/03    Time: 5:11p
 * Updated in $/xGate
 * Changed trace format to support per-user trace
 * 
 * *****************  Version 37  *****************
 * User: Junanto      Date: 27/05/02   Time: 10:05
 * Updated in $/xGate
 * Added CallGetMoreDigitsEx which adds max. number of digits.
 * This function will block until either one of this condition occurs:
 *   - max. number of digits collected
 *   - "number complete" received
 *   - interdigit time out
 * 
 * *****************  Version 36  *****************
 * User: Junanto      Date: 26/05/02   Time: 17:23
 * Updated in $/xGate
 * Implemented CallGetMoreDigits
 * 
 * *****************  Version 35  *****************
 * User: Adham        Date: 8/03/02    Time: 18:36
 * Updated in $/xGate
 * The class declaration of CallExpression and CallTraceCondition moved to
 * header file
 * 
 * *****************  Version 34  *****************
 * User: Sonny        Date: 31/01/02   Time: 13:12
 * Updated in $/xGate
 * add call quality monitoring
 * 
 * *****************  Version 33  *****************
 * User: Sonny        Date: 24/01/02   Time: 20:58
 * Updated in $/xGate
 * Quality monitoring support
 * 
 * *****************  Version 32  *****************
 * User: Bennylp      Date: 8/20/01    Time: 3:30p
 * Updated in $/xGate
 * Moved DestroyCall definition to cpp file.
 * 
 * *****************  Version 31  *****************
 * User: Bennylp      Date: 7/30/01    Time: 12:34p
 * Updated in $/xGate
 * Disconnect calls in OnShutdown(), not only in Unload().
 * 
 * *****************  Version 30  *****************
 * User: Junanto      Date: 23/07/01   Time: 22:13
 * Updated in $/xGate
 * Present call disconnected state to the application for early RelComInd
 * StrCallState moved to the call manager
 * The declaration of some APIs are changed to synchronous
 * 
 * *****************  Version 29  *****************
 * User: Junanto      Date: 20/07/01   Time: 18:51
 * Updated in $/xGate
 * Fixed bug when task is woken up more than once
 * 
 * *****************  Version 28  *****************
 * User: Junanto      Date: 19/07/01   Time: 19:33
 * Updated in $/xGate
 * Added information displayed in "call detail"
 * Corrected bug info "call list task"
 * 
 * *****************  Version 27  *****************
 * User: Junanto      Date: 16/07/01   Time: 22:44
 * Updated in $/xGate
 * Delayed RelComInd for failed Setup
 * 
 * *****************  Version 26  *****************
 * User: Bennylp      Date: 7/15/01    Time: 7:03p
 * Updated in $/xGate
 * RelComInd will not immediately delete call instance.
 * 
 * *****************  Version 25  *****************
 * User: Junanto      Date: 29/06/01   Time: 16:48
 * Updated in $/xGate
 * Added new application APIs for:
 * - retrieving current call parameter
 * - retrieving interface name
 * - retrieving group name
 * 
 * *****************  Version 24  *****************
 * User: Junanto      Date: 25/06/01   Time: 16:39
 * Updated in $/xGate
 * CallSetup may specify wild card (any port with a 
 * particular group name)
 * 
 * *****************  Version 23  *****************
 * User: Bennylp      Date: 6/22/01    Time: 7:40p
 * Updated in $/xGate
 * Changed 'bool' arguments to int in API functions.
 * 
 * *****************  Version 22  *****************
 * User: Junanto      Date: 8/06/01    Time: 16:51
 * Updated in $/xGate
 * Added new command to list current active calls
 * 
 * *****************  Version 21  *****************
 * User: Junanto      Date: 7/06/01    Time: 17:00
 * Updated in $/xGate
 * Added selective call trace
 * 
 * *****************  Version 20  *****************
 * User: Junanto      Date: 2/06/01    Time: 14:35
 * Updated in $/xGate
 * Changed output format of call trace
 * 
 * *****************  Version 19  *****************
 * User: Bennylp      Date: 5/29/01    Time: 4:45p
 * Updated in $/xGate
 * Remove invalid handle warning in CallGetHandle()
 * 
 * *****************  Version 18  *****************
 * User: Junanto      Date: 25/05/01   Time: 13:05
 * Updated in $/xGate
 * Corrected behaviour of SignallingConnect
 * 
 * *****************  Version 17  *****************
 * User: Junanto      Date: 11/05/01   Time: 11:30
 * Updated in $/xGate
 * Switch voice and call automatically during incoming
 * call when a task needs automatic voice allocation
 * 
 * *****************  Version 16  *****************
 * User: Junanto      Date: 3/05/01    Time: 11:14
 * Updated in $/xGate
 * 
 * *****************  Version 15  *****************
 * User: Junanto      Date: 2/05/01    Time: 18:40
 * Updated in $/xGate
 * Added support for call statistics
 * 
 * *****************  Version 14  *****************
 * User: Junanto      Date: 26/04/01   Time: 12:05
 * Updated in $/xGate
 * Changed call trace output to take
 * benefit of new facilities in KLog
 * 
 * *****************  Version 13  *****************
 * User: Junanto      Date: 25/04/01   Time: 11:57
 * Updated in $/xGate
 * added call tracing facility
 * 
 * *****************  Version 12  *****************
 * User: Bennylp      Date: 4/23/01    Time: 10:42p
 * Updated in $/xGate
 * Fixed std namespace conflicts.
 * 
 * *****************  Version 11  *****************
 * User: Junanto      Date: 22/04/01   Time: 21:26
 * Updated in $/xGate
 * Added APIs: CallExSetDisconnectMsg and Set/GetSessionId
 * 
 * *****************  Version 10  *****************
 * User: Junanto      Date: 21/04/01   Time: 23:06
 * Updated in $/xGate
 * added media API
 * 
 * *****************  Version 9  *****************
 * User: Junanto      Date: 18/04/01   Time: 11:49
 * Updated in $/ct15
 * 
 * *****************  Version 8  *****************
 * User: Junanto      Date: 15/04/01   Time: 20:48
 * Updated in $/ct15
 * 
 * *****************  Version 7  *****************
 * User: Junanto      Date: 14/04/01   Time: 20:00
 * Updated in $/ct15
 * Add more APIs
 * 
 * *****************  Version 6  *****************
 * User: Junanto      Date: 13/04/01   Time: 21:03
 * Updated in $/ct15
 * 1st version of integration with call manager
 * 
 * *****************  Version 5  *****************
 * User: Junanto      Date: 12/04/01   Time: 19:54
 * Updated in $/ct15
 * 
 * *****************  Version 3  *****************
 * User: Junanto      Date: 8/04/01    Time: 19:43
 * Updated in $/ct2
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 15/03/01   Time: 17:20
 * Updated in $/ct2
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 12/03/01   Time: 22:20
 * Created in $/ct2
 * Created
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 12/03/01   Time: 15:34
 * Created in $/ct2
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 *  
 *----------------------------------------------------------------*/

#if !defined(CT2_CALLMANAGER_H__INCLUDED_)
#define CT2_CALLMANAGER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "task.h"
#include "call.h"
#include "api.h"
//#include "pcre-3.4/pcreposix.h"	// Regex
#include <regex.h>

#ifdef XGATE_HAS_RTP_INTERFACE
#include "rtpmedia.h"
#endif

#ifdef XGATE_HAS_HMP
extern "C"
{
#include <gst/gst.h>
}
#include <map>
#endif


#define RECORD_ECT_INDICATOR "tect_indicator"

typedef struct call_queue_list {
        String desk_phone[200];
        String mobile_phone[200];
} Call_Queue_List;

typedef struct Call_Queue {
        String domain_name[4];
        Call_Queue_List call_queue_list;
} call_queue_t;

/** Forward declaration
 */
class CTask;
struct TaskCallBlock;
class Log_Trace;
class Term_Session;
class CallTraceCondition;
class Cfg_Tree_Group;


/*----------------------------------------------------------------* 
 * Call expression: 
 * Used to specify the condition of call to be included in the call
 * trace. Expression is specified as tag=value[,tag=value ...]
 *----------------------------------------------------------------*/

class CallExpression
{
    typedef Map<String,String> MapTagValue;
public:
    CallExpression(const char* expr);

    bool Parse();

    const char* GetValue(const char* tag) {
	if (_tv.find(tag) == _tv.end()) return "";
	else return _tv[tag].c_str();
    }

    const char* GetExpression() {
	return _expr.c_str();
    }

private:
    MapTagValue _tv;
    String _expr;
    char _token[256];
    const char* _cursor;

    void ConsumeWhiteSpace() {
	while (*_cursor) {
	    if (*_cursor == ' ') _cursor++;
	    else break;
	}
    }

    char* GetToken();
};


/*----------------------------------------------------------------* 
 * CallTraceCondition: 
 * Used to store the compiled version of call expression
 *----------------------------------------------------------------*/
class CallTraceCondition
{
public:
    CallTraceCondition(const char* expr);
    ~CallTraceCondition();

    bool Parse();

    const char* GetExpression() {
	return _expr.GetExpression();
    }

    unsigned GetId() {
	return _id;
    }

    bool Match(const String& ddi, const String& cli, const String& group, 
	const String& itf, const String& pinfo, const String& uinfo,
	const String& session, const String& direction);

private:
    CallExpression _expr;
    regex_t _regex;
    unsigned _id;
    String t1,t2,t3,t4,t5,t6,t7,t8,sep;

    static unsigned nextid;
};

/** Return Code for CallManager
 */
enum CallReturnCode
{
    CRC_NoError = 0,
    CRC_InvalidInterface = -1,
    CRC_InvalidSignalling = -2,
    CRC_InvalidParameter = -3,
    CRC_InvalidCallHandle = -4,
    CRC_InvalidMedia = -5,
    CRC_InvalidCallParameter = -6,
    CRC_NoCircuit = -7,
};


#ifdef XGATE_HAS_HMP
class HmpConferenceRoom {
  private:
    std::string _room_name;
    int _room_size;
    static int _handle;
   //	int _handle;
    int _members_count;
    std::vector<Rtp_Media*> _members_list;
    CTask* _task;
    //GstElement* _hmp_conf_pipeline;
  public:
    //HmpConferenceRoom(std::string room_name, int room_size):_room_name(room_name),_room_size(room_size),_members_count(0),_task(0),_hmp_conf_pipeline(NULL)
    HmpConferenceRoom(std::string room_name, int room_size):_room_name(room_name),_room_size(room_size),_members_count(0),_task(0) 
	{
      ++_handle;
    }
    ~HmpConferenceRoom() {
      --_handle;
    }
    int GetHandle() { 
		return _handle; 
	};
 /*void InsertMember(Rtp_Media *media)
    {
      if(media)
      {
        if(_room_size > _members_count)
        {
          _members_list.push_back(media);
          ++_members_count;
        }
      }
    }
    void RemoveMember(Rtp_Media *media)
    {
      if(media)
      {
        std::vector<Rtp_Media*>::iterator it = _members_list.begin();
        std::vector<Rtp_Media*>::iterator end = _members_list.end();
        for(it; it!=end; ++it)
        {
          if(media == (Rtp_Media*) *it)
          {
            _members_list.erase(it);
            --_members_count;
            break;
          }
        }
      }
  }*/
    void increaseMembersCount() { ++_members_count;}
    void decreaseMembersCount() { --_members_count;}
    int GetMembersCount() { return _members_count; }
    /*GstElement* GetConfPipeline(){return _hmp_conf_pipeline;};
    void SetConfPipeline(GstElement* hmpConfPipeline) {_hmp_conf_pipeline = hmpConfPipeline;}
    Rtp_Media * GetMemeber()
    {
      if(!_members_list.empty())
      {
        //TODO:
        return NULL;
      }
    }*/
};

#endif





/** CallManager: This singleton entity is the interface between Kernel and
    Signalling Driver.
 */
class CallManager : public ServiceProvider
{
    friend class CallAPI;
    friend class BSSAP_ConnectionManager;

public:
    virtual ~CallManager();

    /** GetInstance: Returns a pointer to this singleton instance, if necessary
	create one instance first before returning a pointer to it
     */
    static CallManager* Create();

    /** GetInstance: Returns a pointer to this singleton instance, if necessary
	create one instance first before returning a pointer to it
     */
    static CallManager* Instance();

    /** Load: Initializes the call manager
     */
    virtual bool Load();

    /** Unload: Deinitializes the call manager
     */
    virtual bool Unload();

    /** HandleEvents: delete calls in _listCallClearing
     */
    virtual void HandleEvents();

    /** OnShutdown: disconnect current calls.
     */
    virtual void OnShutdown(bool cancel);

    /** Determine whether new calls can be created.
	CallManager can NOT create new calls when it hasn't been Load()-ed,
	and when it receives OnShutdown() notification from kernel.
     */
    bool CanCreateCall() const {
	return _canCreateCall;
    }

    /** CallCreated: This method is called by Signalling Driver during SetupReq 
	(before SetupReq returns) and on receipt of new incoming call to inform
	Call Manager about the new call object
     */
    void CallCreated(Call* call);

    /** Insert/Remove active call to/from the list
     */
    void InsertActiveCall(Call*);
    void RemoveActiveCall(Call*);

    /** Destroy a call instance. In this implementation, it will insert call 
	to the list of calls to be deleted later in HandleEvents.
     */
    void DestroyCall(Call *call);

    /** Insert a call into a task call block and return the handle
     */
    unsigned AttachCallToTask(CTask* task,Call* call);

    /** Remove a call from a task call block
     */
    void DetachCallFromTask(CTask* task,Call* call);

    // karthi - testing
    void DetachCallFromTask_HG(CTask* task,Call* call, int handle);


    /** Methods, exposed to Kernel for user application's tasks and jobs
     */

    /** Iterate all calls owned by the specified task and disconmect the call
	if it is doing an asynchronous job
     */
    virtual bool CancelJob(CTask* task);

    /** Initialize task's TLS
     */
    virtual bool AttachTask(CTask* task);

    /** Suspend the task owner of a particular call
     */
    void SuspendOwnerTask(Call* call);

    /** Resume the task owner of a particular call
     */
    void ResumeOwnerTask(Call* call);

    /** Disconmect each call owned by the specified task that is not is Null state,
	detach the call from task, let each call to go to Null state before destroying
	the call on receipt of RelComInd.
	If call was already in Null state, detach it from task and destroy the call.
     */
    virtual void DetachTask(CTask* task);

    /** Called by a Call object to notify of a new incoming call, inform the kernel 
	and attach the call to the newly created task
      */
    void OnIncomingCall(Call* call);

    /** Return the current incoming call
      */
    Call* GetCurrentIncomingCall();

    /** Check if this call should be included in the call trace or not
     */
    bool TestTraceCondition(Call* call, unsigned *id);

    /** Called by a Call object to output call trace to the user
     */
    void TraceCall(Call* call,bool received,const char* message,const char* format, ...);

    /** TraceEnabled: Return the state of call tracing
     */
    bool TraceEnabled();

    /** Call statistics counters
     */
    void StatIncCallCounter(CallDirection cd);
    void StatDecCallCounter(CallDirection cd);
    void ShowCallStats(Term_Session* session);

    /** Some conversion to string functions
     */
    const char* StrCallNumberType(CallNumberType cnt);
    const char* StrCallPartyCategory(CallPartyCategory cpc);
    const char* StrCallConnectionType(CallConnectionType cct);
    const char* StrCallCause(CallCause cause);

    void RemoveCallFromWaitDigitsList(Call* call);
	#ifdef XGATE_HAS_HMP
    bool AddConfRoomInMap(char* room_name, HmpConferenceRoom *conf_room);
    bool RemoveConfRoomFromMap(char* room_name);
    HmpConferenceRoom *GetConfRoomFromMap(char* room_name);
	#endif

	//added for ippbx
    unsigned int GetCallHandleFromCall(CTask* task,Call* call);
    void ReadFromProcessQueue(void); // TODO: Temp solution make call into list

private:
    typedef List<Call*> ListCall;
    typedef List<CallTraceCondition*> ListCondition;

    Call		*_lastCallCreated;
    Call		*_currentInCall;
    CTask		*_lastTaskAttached;
    ListCall		_listCall, _listCallClearing, _listCallToBeReleased, _listCallWaitDigits;
    int			_tlsId;
    CAPIProvider	*_api;
    Log_Trace		*_traceCall;
    ListCondition	_listCondition;
    bool		_canCreateCall;
	#ifdef XGATE_HAS_HMP
    std::map<String, HmpConferenceRoom*> _hmp_conf_rooms_map;
	#endif
	

    /** Call statistics counters
     */
    unsigned		_statCurNbCall;			// current number of active calls
    unsigned		_statCurNbOutCall;		// current number of outgoing calls
    unsigned		_statCurNbInCall;		// current number of incoming calls
    unsigned		_statMaxNbCall;			// maximum number of active calls
    unsigned		_statMaxNbOutCall;		// maximum number of outgoing calls
    unsigned		_statMaxNbInCall;		// maximum number of incoming calls
    unsigned		_statCurCallPSec;		// current number of calls/per second
    unsigned		_statCurOutCallPSec;		// current number of outgoing calls/per second
    unsigned		_statCurInCallPSec;		// current number of incoming calls/per second
    unsigned		_statMaxCallPSec;		// maximum number of calls/per second
    unsigned		_statMaxOutCallPSec;		// maximum number of outgoing calls/per second
    unsigned		_statMaxInCallPSec;		// maximum number of incoming calls/per second
    time_t		_statLastUpdate;

    /** A pointer to the singleton
     */
    static CallManager	*theCallManager;

    /** Private constructor for the singleton
     */
    CallManager();

    /** Convert a handle as used by Kernel (and user applications)
	to a pointer to call object
     */
    Call* CallFromHandle(CTask* task,int handle,bool warning,const char* funcname);
    TaskCallBlock* GetTaskCallBlock(CTask* task);

    /** Terminal command call-back
     */
    void DoTermCmd(Term_Session*, bool, Term_Cmd*);

    /** Used to return conversion to string value
     */
    String _retstr;
};


#endif // !defined(CT2_CALLMANAGER_H__INCLUDED_)
