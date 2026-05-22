/* $Header: /xGate/systemapi.cpp 19    24/05/04 11:02 Sonny $
 *   
 * CTENGINE
 * (C) 2001 Switchlab, Ltd
 *
 * Provides miscelanous API to the applications.
 *
 * $Log: /xGate/systemapi.cpp $
 * 
 * 19    24/05/04 11:02 Sonny
 * added APIs: EnableException, DisableException, StrRegex, StrUpper,
 * StrLower, BitwiseAnd, BitwiseOr, BitwiseXor
 * 
 * 18    3/08/04 7:06p Bennylp
 * Added API TaskGetStartupType
 * 
 * 17    6/04/03 3:46p Bennylp
 * Added api Stoi_Ex etc for modem support.
 * 
 * 16    4/23/03 5:34p Bennylp
 * Administrivia: tidy-up the usage of API provider ID. Now the
 * enumeration in api.h should be used instead of bare constants.
 * 
 * 15    4/03/03 3:42p Bennylp
 * Fixed SpawnTask (don't include argument when string is empty)
 * 
 * 14    4/02/03 4:02p Bennylp
 * Added API TaskIsExist, and change return value of SpawnTask
 * 
 * 13    1/22/03 1:58p Sonny
 * added GetHostname API
 * 
 * 12    9/25/02 5:12p Bennylp
 * Fixed bug in RaiseException API (should be async)
 * 
 * 11    22/05/02 17:59 Junanto
 * Application logging to terminal can now be filtered out
 * (to concentrate on system messages only) by a 
 * new parameter "application" in "term logging".
 * By default, application logging is displayed to terminal.
 * To disable it: "no term logging application".
 * 
 * 10    26/03/02 16:40 Iwanj
 * bugfix API_SeqRemove
 * 
 * 9     18/10/01 18:26 Iwanj
 * add sequence API
 * 
 * 8     7/25/01 8:06p Bennylp
 * Added application tracing.
 * 
 * 7     6/15/01 1:14p Bennylp
 * Remove some warning messages (StrRight, etc).
 * 
 * 6     31/05/01 18:03 Junanto
 * SpawnTask set asynch
 * 
 * 5     5/22/01 11:01a Bennylp
 * Verify task's last error.
 * 
 * 4     1/05/01 15:44 Junanto
 * Add source name in RaiseException
 * for logging purpose
 * 
 * 3     4/18/01 5:54p Bennylp
 * Added ArgGet..() and more time APIs.
 * 
 * 2     4/13/01 11:18p Bennylp
 * Integration.
 */
//#include "stdafx.h"
#include <iostream>
using namespace std;
#include <sys/timeb.h> //linux porting
#include <arpa/inet.h>
//#include <stdlib.h>


#include "systemapi.h"
#include "task.h"
#include "sl.h"
#include "log.h"	// CLog::..
#include "kernel.h"	// spawntask
#include "sleepdrv.h"	// SleepTask() is needed by RaiseException()
#include "thread_process.h"	// CPU load
#include "api.h"
#include <math.h>	// round()
//#include "pcre-3.4/pcreposix.h"	// Regex
#include <regex.h>


#define SYSAPI   "sys"

#ifdef __linux__
char * _strlwr(char *s){

        char * t = s;
        while(*t = tolower(*t))
                t++;
        return s;

}

char * _strupr(char *s){

    char * t = s;
    while(*t = toupper(*t))
        t++;
    return s;
}
#endif


System_API_Provider *System_API_Provider::sysapiInstance;


class CSystemAPI : public CAPIProvider {

public:

    CSystemAPI();
    ~CSystemAPI();

    STATUS  Init();
    STATUS  Shutdown();

    // Safety level in string operation. Setting to UNSAFE
    enum StringSafetyLevel {
	UNSAFE_STRING,
	SAFE_STRING
    };
    StringSafetyLevel	m_StringSafetyLevel;

    // Application parameters
    STATUS  API_CALL API_ArgGet ( CTask *, char *ret, int index);
    STATUS  API_CALL API_ArgGetCount ( CTask *, int *ret);
    STATUS  API_CALL API_ParamIsExist ( CTask *task, int *ret, const char *name);
    STATUS  API_CALL API_ParamCreate ( CTask *task, const char *name, const char *value);
    STATUS  API_CALL API_ParamGet ( CTask *task, char *retstr, const char *param_name);
    STATUS  API_CALL API_ParamSet ( CTask *task, const char *param_name, const char *val);
    STATUS  API_CALL API_ParamDelete ( CTask *task, const char *param_name );

    // Task
    STATUS  API_CALL API_SpawnTask ( CTask *, int *ret, const char *appname, const char *arg0, const char *arg1, const char *arg2, const char *arg3); 
    STATUS  API_CALL API_GetTID(CTask *, int *ret);
    STATUS  API_CALL API_TaskAddr(CTask *, int *ret);
    STATUS  API_CALL API_Halt(CTask *);
    STATUS  API_CALL API_TaskIsExist(int *ret, int pid);
    STATUS  API_CALL API_TaskGetStartupType(CTask *task, int *type);
    STATUS  API_CALL API_EnableException(CTask *task);
    STATUS  API_CALL API_DisableException(CTask *task);
    STATUS  API_CALL API_TaskSetName( CTask *task, int *ret, const char *task_name);
    STATUS  API_CALL API_TaskSetIPCMessageByPID(int *ret, int pid, const char *msg, int ex_id);
    STATUS  API_CALL API_TaskSetIPCMessageByName(int *ret, const char *task_name, const char *msg, int ex_id);
    STATUS  API_CALL API_TaskGetIPCMessage( CTask *task, char *retstr);
    STATUS  API_CALL API_TaskGetName( CTask *task, char *retstr);
    STATUS  API_CALL API_TaskIsExistByName(int *ret, const char *task_name);
    STATUS  API_CALL API_TaskCountByAppName(int *ret, const char *app_name);


    //STATUS  API_CALL API_SetTaskPriority( CTask *, int prio);
    STATUS  API_CALL API_GetLastError( CTask *, int *ret);
    STATUS  API_CALL API_GetLastStatus( CTask *, int *ret);
    STATUS  API_CALL API_RaiseException( CTask *, int ex_id);
    STATUS  API_CALL API_GetExceptionReason( CTask *, int *ret);

    // Miscelanous
    STATUS  API_CALL API_Random(CTask *,int *retval,int maxval);
    STATUS  API_CALL API_LogInfo(CTask *, const char *message);
    STATUS  API_CALL API_LogWarning(CTask *, const char *message);
    STATUS  API_CALL API_LogError(CTask *, const char *message);
    STATUS  API_CALL API_Trace(CTask *, const char *trace_criteria, const char *trace_msg);

    // Time
    STATUS  API_CALL API_TmGetTime( int *ret );
    STATUS  API_CALL API_TmGetTimeEx( int *ret, int *msec);
    STATUS  API_CALL API_TmStrFormat( CTask *,char *ret, int tm, const char *format);
    STATUS  API_CALL API_TmBuildTime( int *ret, int year, int month, int day, int hour, int min, int sec);
    STATUS  API_CALL API_TmGetDayOfWeek (int *ret, int tm);
    STATUS  API_CALL API_TmGetDayOfMonth (int *ret, int tm);
    STATUS  API_CALL API_TmGetDayOfYear (int *ret, int tm);
    STATUS  API_CALL API_TmGetMonth (int *ret, int tm);
    STATUS  API_CALL API_TmGetYear (int *ret, int tm);
    STATUS  API_CALL API_TmGetHour (int *ret, int tm);
    STATUS  API_CALL API_TmGetMinute (int *ret, int tm);
    STATUS  API_CALL API_TmGetSecond (int *ret, int tm);
    STATUS  API_CALL API_TmGetUTCTime( int *ret );

    // Data conversion
    STATUS  API_CALL API_Stoi( CTask *,int *ret, const char *str);
    STATUS  API_CALL API_Stor( CTask *,float *ret, const char *str);
    STATUS  API_CALL API_Itos( char *ret, int i);
    STATUS  API_CALL API_Itor( float *ret, int i);
    STATUS  API_CALL API_Rtos( char *ret, float f);
    STATUS  API_CALL API_Rtoi( int *ret, float f);
    STATUS  API_CALL API_Round( float *ret, float f);

    // String manipulation
    STATUS  API_CALL API_StrLen( int *ret, const char *str);
    STATUS  API_CALL API_StrSub( char *ret, const char *str, int from, int length);
    STATUS  API_CALL API_StrPos( int *ret, const char *str, const char *substr, int ipos);
    STATUS  API_CALL API_StrLeft( char *ret, const char *str, int len);
    STATUS  API_CALL API_StrRight( char *ret, const char *str, int len);
    STATUS  API_CALL API_StrFromChr( char *ret, int chr);
    STATUS  API_CALL API_StrGetChr( int *ret, const char *str, int ipos);
    STATUS  API_CALL API_StrTrim(char *ret,const char *str);
    STATUS  API_CALL API_StrLTrim(char *ret,const char *str);
    STATUS  API_CALL API_StrRTrim(char *ret,const char *str);
    STATUS  API_CALL API_StrLPad(char *ret,const char *str,const char *padding,int length);
    STATUS  API_CALL API_StrRPad(char *ret,const char *str,const char *padding,int length);
    STATUS  API_CALL API_StrRegex(int *ret,const char *str,const char *pattern);
    STATUS  API_CALL API_StrUpper(char *ret,const char *str);
    STATUS  API_CALL API_StrLower(char *ret,const char *str);
    STATUS  API_CALL API_StrHexFromChr( char *ret, int chr);

    // Sequence manipulation
    STATUS API_CALL API_SeqSize(int* size,const char* seq,const char* delim);
    STATUS API_CALL API_SeqGet(char* elem,const char* seq,const char* delim,int pos);
    STATUS API_CALL API_SeqRemove(char* new_seq,const char* seq,const char* delim,int pos);
    STATUS API_CALL API_SeqFind(int* pos,const char* seq,const char* delim,const char* elem);
    STATUS API_CALL API_SeqSub(char* new_seq,const char* seq,const char* delim,int pos,int len);

    // Network
    STATUS  API_CALL API_GetHostname ( CTask *task, char *retstr);
    STATUS  API_CALL API_GetIPAddress ( CTask *task, char *retstr);

    // Bitwise
    STATUS API_CALL API_BitwiseAnd(int* result, int value1, int value2);
    STATUS API_CALL API_BitwiseOr(int* result, int value1, int value2);
    STATUS API_CALL API_BitwiseXor(int* result, int value1, int value2);

    // CPU load
    STATUS  API_CALL API_CpuGetProcessLoad(int *ret, int index);
    STATUS  API_CALL API_CpuGetGlobalLoad(int *ret, int index);

};


//-----------------------------------------------------------------------------

System_API_Provider * System_API_Provider::Create()
{
    if (sysapiInstance == NULL) {
	NEW_RETURN_ (sysapiInstance, System_API_Provider);
    }
    return sysapiInstance;
}

System_API_Provider::System_API_Provider()
: ServiceProvider(SYSAPI, false, false), api(NULL)
{
}

System_API_Provider::~System_API_Provider()
{
    sysapiInstance = NULL;
}


/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
 */
bool System_API_Provider::Load()
{
    NEW_RETURN_ (api, CSystemAPI);
    if (api == NULL)
	return false;

    if (api->Init() != OK) {
	delete api;
	api = NULL;
	return false;
    }
    return true;
}

/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
 */
bool System_API_Provider::Unload()
{
    if (api) {
	api->Shutdown();
	delete api;
	api = NULL;
    }
    return true;
}

//-----------------------------------------------------------------------------
CSystemAPI::CSystemAPI()
: CAPIProvider(API_PROVIDER_ID_SYS_API, SYSAPI, 256)
{ 
    m_StringSafetyLevel = SAFE_STRING;

    // initialize random seed for API_Random
    srand((unsigned)time(NULL));
}

//-----------------------------------------------------------------------------
CSystemAPI::~CSystemAPI()
{
}

//-----------------------------------------------------------------------------
//STATUS  CSystemAPI::Init()
STATUS CSystemAPI::Init()
{
    if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK)
	return NOT_OK;

    // Note: only 128 API space reserved in this provider, so 
    // safe the ordinal
    enum {
	// System
	SpawnTask=0,
	GetHostname,
	TaskIsExist,
	EnableException,
	DisableException,
	GetIPAddress,

	// Misc
	unused2__=20,
	Random,
	LogInfo,
	LogWarning,
	LogError,
	Trace,

	// Task
	GetTID=40,
	GetExceptionReason,
	RaiseException,
	SetTaskPriority,
	GetLastError,
	Halt,
	ArgGet,
	ArgGetCount,
	ParamIsExist,
	ParamCreate,
	ParamGet,
	ParamSet,
	ParamDelete,
	TaskGetStartupType,
	TaskSetName, // 54
	TaskSetIPCMessageByPID,
	TaskSetIPCMessageByName,
	TaskGetIPCMessage,
	TaskGetName,
	TaskIsExistByName,

	// Time
	TmGetTime=60,
	TmStrFormat,
	TmBuildTime,
	TmGetTimeEx,
	TmGetDayOfWeek,
	TmGetDayOfMonth,
	TmGetDayOfYear,
	TmGetMonth,
	TmGetYear,
	TmGetHour,
	TmGetMinute,
	TmGetSecond,
	TmGetUTCTime,

	// Type conversion/manipulation
	Stoi=75,
	Stor,
	Itos,
	Itor,
	Rtos,
	Rtoi,
	Round,
	Stoi_Ex,
	Itos_Ex,

	// String
	StrLen=90,
	StrSub,
	StrPos,
	StrLeft,
	StrRight,
	StrFromChr,
	StrGetChr,
	StrTrim,
	StrLTrim,
	StrRTrim,
	StrLPad,
	StrRPad,
	StrFill,
	StrChecksum,
	StrRegex,
	StrUpper,
	StrLower,
	StrHexFromChr,

	// Sequence
	SeqSize=120,
	SeqGet,
	SeqRemove,
	SeqFind,
	SeqSub,

	// Bitwise
	BitwiseAnd=140,
	BitwiseOr,
	BitwiseXor,
    
	// CPU load
	CpuGetProcessLoad=160,
	CpuGetGlobalLoad,
    
	// task-cont'd
	TaskCountByAppName=180,
        // task address
        TaskAddr,

    
    };


    // STATUS  API_CALL API_SpawnTask ( CTask *, int *ret, const char *appname, const char *arg0, const char *arg1, const char *arg2, const char *arg3); 
    if (RegisterAPI( "SpawnTask", SpawnTask, true, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_SpawnTask,
		     CAPIDef::CDECL_TYPE,
		     7,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "appname", BYVAL, CVariant::VT_STRING),
		     CArgType( "arg0", BYVAL, CVariant::VT_STRING),
		     CArgType( "arg1", BYVAL, CVariant::VT_STRING),
		     CArgType( "arg2", BYVAL, CVariant::VT_STRING),
		     CArgType( "arg3", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_GetHostname ( CTask *task, char *retstr);
    if (RegisterAPI( "GetHostname", GetHostname, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_GetHostname,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_GetIPAddress ( CTask *task, char *retstr);
    if (RegisterAPI( "GetIPAddress", GetIPAddress, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_GetIPAddress,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TaskIsExist(int *ret, int pid);
    if (RegisterAPI( "TaskIsExist", TaskIsExist, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TaskIsExist,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "pid", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TaskIsExistByName(int *ret, const char *task_name);
    if (RegisterAPI( "TaskIsExistByName", TaskIsExistByName, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TaskIsExistByName,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "task_name", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_EnableException (CTask *task);
    if (RegisterAPI( "EnableException", EnableException, false, CVariant::VT_VOID, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_EnableException,
		     CAPIDef::CDECL_TYPE,
		     1,
		     CArgType("task", BYVAL, CVariant::VT_TASK)) == NOT_OK)
    {
	return NOT_OK;
    }

    // STATUS  API_CALL API_DisableException (CTask *task);
    if (RegisterAPI( "DisableException", DisableException, false, CVariant::VT_VOID, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_DisableException,
		     CAPIDef::CDECL_TYPE,
		     1,
		     CArgType("task", BYVAL, CVariant::VT_TASK)) == NOT_OK)
    {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TaskGetStartupType(CTask *task, int *type);
    if (RegisterAPI( "TaskGetStartupType", TaskGetStartupType, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TaskGetStartupType,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    //STATUS  API_GetTID(CTask *, int *ret);
    if (RegisterAPI( "GetTID", GetTID, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_GetTID,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    //STATUS  API_TaskAddr(CTask *, int *ret);
    if (RegisterAPI( "GetTaskAddr", TaskAddr, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TaskAddr,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_Halt(CTask *);
    if (RegisterAPI( "Halt", Halt, true, CVariant::VT_VOID, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_Halt,
		     CAPIDef::CDECL_TYPE,
		     1,
		     CArgType( "task", BYVAL, CVariant::VT_TASK)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS  API_SetTaskPriority( CTask *, int prio);
    /*
    if (RegisterAPI( "SetTaskPriority", SetTaskPriority, false, CVariant::VT_VOID, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_SetTaskPriority,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "prio", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }
    */

    //STATUS  API_TaskSetName( CTask *task, int *ret, char *task_name);
    if (RegisterAPI( "TaskSetName", TaskSetName, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TaskSetName,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "task_name", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TaskGetIPCMessage( CTask *task, char *retstr);
    if (RegisterAPI( "TaskGetName", TaskGetName, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TaskGetName,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TaskSetIPCMessageByPID(int *ret, int pid, const char *msg, int ex_id);
    if (RegisterAPI( "TaskSetIPCMessageByPID", TaskSetIPCMessageByPID, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TaskSetIPCMessageByPID,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "pid", BYVAL, CVariant::VT_INT),
		     CArgType( "msg", BYVAL, CVariant::VT_STRING),
		     CArgType( "ex_id", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TaskSetIPCMessageByName(int *ret, const char *task_name, const char *msg, int ex_id);
    if (RegisterAPI( "TaskSetIPCMessageByName", TaskSetIPCMessageByName, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TaskSetIPCMessageByName,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "task_name", BYVAL, CVariant::VT_STRING),
		     CArgType( "msg", BYVAL, CVariant::VT_STRING),
		     CArgType( "ex_id", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TaskGetIPCMessage( CTask *task, char *retstr);
    if (RegisterAPI( "TaskGetIPCMessage", TaskGetIPCMessage, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TaskGetIPCMessage,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TaskCountByAppName(int *ret, const char *app_name);
    if (RegisterAPI( "TaskCountByAppName", TaskCountByAppName, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TaskCountByAppName,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "app_name", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }



    // STATUS  API_GetLastStatus( CTask *, int *ret);
    if (RegisterAPI( "GetLastStatus", GetLastError, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_GetLastError,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "return", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_LogInfo(CTask *, const char *message);
    if (RegisterAPI( "LogInfo", LogInfo, false, CVariant::VT_VOID, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_LogInfo,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "message", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_LogWarning(CTask *, const char *message);
    if (RegisterAPI( "LogWarning", LogWarning, false, CVariant::VT_VOID, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_LogWarning,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "message", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_LogError(CTask *, const char *message);
    if (RegisterAPI( "LogError", LogError, false, CVariant::VT_VOID, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_LogError,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "message", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_Trace(CTask *, const char *trace_criteria, const char *trace_msg);
    if (RegisterAPI( "Trace", Trace, false, CVariant::VT_VOID, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_Trace,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "criteria", BYVAL, CVariant::VT_STRING),
		     CArgType( "message", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_Random(CTask *,int *retval,int maxval);
    if (RegisterAPI( "Random", Random, false, CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &CSystemAPI::API_Random,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "retval", BYREF, CVariant::VT_RET),
		     CArgType( "maxval", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS  API_CALL API_ParamIsExist ( CTask *task, int *ret, const char *name);
    if (RegisterAPI( "ParamIsExist", ParamIsExist, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_ParamIsExist,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "return", BYREF, CVariant::VT_RET),
		     CArgType( "param", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS  API_CALL API_ParamCreate ( CTask *task, const char *name, const char *value);
    if (RegisterAPI( "ParamCreate", ParamCreate, false, CVariant::VT_VOID, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_ParamCreate,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "name", BYVAL, CVariant::VT_STRING),
		     CArgType( "value", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_ParamGet ( CTask *task, char *retstr, const char *param_name);
    if (RegisterAPI( "ParamGet", ParamGet, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_ParamGet,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "return", BYREF, CVariant::VT_RET),
		     CArgType( "param", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    STATUS  API_CALL API_ParamSet ( CTask *task, const char *param_name, const char *val);
    if (RegisterAPI( "ParamSet", ParamSet, false, CVariant::VT_VOID, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_ParamSet,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "name", BYVAL, CVariant::VT_STRING),
		     CArgType( "value", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS  API_ArgGet( CTask *, char *ret, int param);
    if (RegisterAPI( "ArgGet", ArgGet, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_ArgGet,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "index", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_ArgGetCount( CTask *, int *ret);
    if (RegisterAPI( "ArgGetCount", ArgGetCount, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_ArgGetCount,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS  API_RaiseException( CTask *task, int ex_id)
    if (RegisterAPI( "RaiseException", RaiseException, true, CVariant::VT_VOID, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_RaiseException,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ex_id", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS  API_GetExceptionReason( CTask *task, int *ret)
    if (RegisterAPI( "GetExceptionReason", GetExceptionReason, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_GetExceptionReason,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_TmGetTime( int *tm )
    if (RegisterAPI( "TmGetTime", TmGetTime, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TmGetTime,
		     CAPIDef::CDECL_TYPE,
		     1,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_TmStrFormat( CTask *,char *ret, int tm, const char *format)
    if (RegisterAPI( "TmStrFormat", TmStrFormat, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TmStrFormat,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "time", BYVAL, CVariant::VT_INT),
		     CArgType( "format", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_TmBuildTime( int *tm, int year, int month, int day, int hour, int min, int sec)
    if (RegisterAPI( "TmBuildTime", TmBuildTime, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TmBuildTime,
		     CAPIDef::CDECL_TYPE,
		     7,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "year", BYVAL, CVariant::VT_INT),
		     CArgType( "month", BYVAL, CVariant::VT_INT),
		     CArgType( "day", BYVAL, CVariant::VT_INT),
		     CArgType( "hour", BYVAL, CVariant::VT_INT),
		     CArgType( "minute", BYVAL, CVariant::VT_INT),
		     CArgType( "second", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS  API_CALL API_TmGetTimeEx( int *ret, int *msec);
    if (RegisterAPI( "TmGetTimeEx", TmGetTimeEx, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TmGetTimeEx,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "msec", BYREF, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TmGetDayOfWeek (int *ret, int tm);
    if (RegisterAPI( "TmGetDayOfWeek", TmGetDayOfWeek, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TmGetDayOfWeek,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "time", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TmGetDayOfMonth (int *ret, int tm);
    if (RegisterAPI( "TmGetDayOfMonth", TmGetDayOfMonth, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TmGetDayOfMonth,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "time", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TmGetDayOfYear (int *ret, int tm);
    if (RegisterAPI( "TmGetDayOfYear", TmGetDayOfYear, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TmGetDayOfYear,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "time", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TmGetMonth (int *ret, int tm);
    if (RegisterAPI( "TmGetMonth", TmGetMonth, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TmGetMonth,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "time", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TmGetYear (int *ret, int tm);
    if (RegisterAPI( "TmGetYear", TmGetYear, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TmGetYear,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "time", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TmGetHour (int *ret, int tm);
    if (RegisterAPI( "TmGetHour", TmGetHour, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TmGetHour,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "time", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TmGetMinute (int *ret, int tm);
    if (RegisterAPI( "TmGetMinute", TmGetMinute, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TmGetMinute,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "time", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_TmGetSecond (int *ret, int tm);
    if (RegisterAPI( "TmGetSecond", TmGetSecond, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TmGetSecond,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "time", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }
    
    // STATUS  API_TmGetUTC( int *tm )
    if (RegisterAPI( "TmGetUTCTime", TmGetUTCTime, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_TmGetUTCTime,
		     CAPIDef::CDECL_TYPE,
		     1,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_Stoi( CTask *, int *ret, const char *str);
    if (RegisterAPI( "Stoi", Stoi, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_Stoi,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "str", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_Stor( CTask *, float *ret, const char *str);
    if (RegisterAPI( "Stor", Stor, false, CVariant::VT_FLOAT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_Stor,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "str", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_Itos( char *ret, int i);
    if (RegisterAPI( "Itos", Itos, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_Itos,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "ival", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_Itor( float *ret, int i);
    if (RegisterAPI( "Itor", Itor, false, CVariant::VT_FLOAT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_Itor,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "ival", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_Rtos( char *ret, float f);
    if (RegisterAPI( "Rtos", Rtos, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_Rtos,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "rval", BYVAL, CVariant::VT_FLOAT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_Rtoi( int *ret, float f);
    if (RegisterAPI( "Rtoi", Rtoi, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_Rtoi,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "rval", BYVAL, CVariant::VT_FLOAT)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS  API_Round( float *ret, float f);
    if (RegisterAPI( "Round", Round, false, CVariant::VT_FLOAT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_Round,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "rval", BYVAL, CVariant::VT_FLOAT)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS  API_StrLen( int *ret, const char *str);
    if (RegisterAPI( "Strlen", StrLen, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrLen,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "str", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_StrSub( char *ret, const char *str, int from, int length);
    if (RegisterAPI( "StrSub", StrSub, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrSub,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "str", BYVAL, CVariant::VT_STRING),
		     CArgType( "from", BYVAL, CVariant::VT_INT),
		     CArgType( "length", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_StrPos( int *ret, const char *str, const char *substr, int ipos);
    if (RegisterAPI( "StrPos", StrPos, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrPos,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "src_str", BYVAL, CVariant::VT_STRING),
		     CArgType( "sub_str", BYVAL, CVariant::VT_STRING),
		     CArgType( "start_pos", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }


    // STATUS  API_StrLeft( char *ret, const char *str, int len);
    if (RegisterAPI( "StrLeft", StrLeft, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrLeft,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "src_str", BYVAL, CVariant::VT_STRING),
		     CArgType( "length", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_StrRight( char *ret, const char *str, int len);
    if (RegisterAPI( "StrRight", StrRight, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrRight,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "src_str", BYVAL, CVariant::VT_STRING),
		     CArgType( "length", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_StrFromChr( char *ret, int chr);
    if (RegisterAPI( "StrFromChr", StrFromChr, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrFromChr,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "ascii_code", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_StrGetChr( int *ret, const char *str, int ipos);
    if (RegisterAPI( "StrGetChr", StrGetChr, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrGetChr,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "str", BYVAL, CVariant::VT_STRING),
		     CArgType( "pos", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    
    // STATUS  API_StrTrim(char *ret,const char *str);
    if (RegisterAPI( "StrTrim", StrTrim, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrTrim,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "retval", BYREF, CVariant::VT_RET),
		     CArgType( "src", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_StrLTrim(char *ret,const char *str);
    if (RegisterAPI( "StrLTrim", StrLTrim, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrLTrim,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "retval", BYREF, CVariant::VT_RET),
		     CArgType( "src", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_StrRTrim(char *ret,const char *str);
    if (RegisterAPI( "StrRTrim", StrRTrim, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrRTrim,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "retval", BYREF, CVariant::VT_RET),
		     CArgType( "src", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_StrLPad(char *ret,const char *str,const char *padding,int length);
    if (RegisterAPI( "StrLPad", StrLPad, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrLPad,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType("retval", BYREF, CVariant::VT_RET),
		     CArgType("src", BYVAL, CVariant::VT_STRING),
		     CArgType("padd", BYVAL, CVariant::VT_STRING),
		     CArgType("length", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_StrRPad(char *ret,const char *str,const char *padding,int length);
    if (RegisterAPI( "StrRPad", StrRPad, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrRPad,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType("retval", BYREF, CVariant::VT_RET),
		     CArgType("src", BYVAL, CVariant::VT_STRING),
		     CArgType("padd", BYVAL, CVariant::VT_STRING),
		     CArgType("length", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_StrRegex(int *ret,const char *pattern,const char *str);
    if (RegisterAPI( "StrRegex", StrRegex, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrRegex,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType("retval", BYREF, CVariant::VT_RET),
		     CArgType("src", BYVAL, CVariant::VT_STRING),
		     CArgType("pattern", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_StrUpper(char *ret,const char *str);
    if (RegisterAPI( "StrUpper", StrUpper, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrUpper,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "retval", BYREF, CVariant::VT_RET),
		     CArgType( "src", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_StrLower(char *ret,const char *str);
    if (RegisterAPI( "StrLower", StrLower, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrLower,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "retval", BYREF, CVariant::VT_RET),
		     CArgType( "src", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CALL API_StrHexFromChr(char *ret,int byte);
    if (RegisterAPI( "StrHexFromChr", StrHexFromChr, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_StrHexFromChr,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "ascii_code", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
	}

    //STATUS API_CALL API_SeqSize(int* size,const char* seq,const char* delim);
    if (RegisterAPI( "SeqSize", SeqSize, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_SeqSize,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType("size", BYREF, CVariant::VT_RET),
		     CArgType("seq", BYVAL, CVariant::VT_STRING),
		     CArgType("delim", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    //STATUS API_CALL API_SeqGet(char* elem,const char* seq,const char* delim,int pos);
    if (RegisterAPI( "SeqGet", SeqGet, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_SeqGet,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType("elem", BYREF, CVariant::VT_RET),
		     CArgType("seq", BYVAL, CVariant::VT_STRING),
		     CArgType("delim", BYVAL, CVariant::VT_STRING),
		     CArgType("pos", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    //STATUS API_CALL API_SeqRemove(char* new_seq,const char* seq,const char* delim,int pos);
    if (RegisterAPI( "SeqRemove", SeqRemove, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_SeqRemove,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType("result", BYREF, CVariant::VT_RET),
		     CArgType("seq", BYVAL, CVariant::VT_STRING),
		     CArgType("delim", BYVAL, CVariant::VT_STRING),
		     CArgType("pos", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    //STATUS API_CALL API_SeqFind(int* pos,const char* seq,const char* delim,const char* elem);
    if (RegisterAPI( "SeqFind", SeqFind, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_SeqFind,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType("pos", BYREF, CVariant::VT_RET),
		     CArgType("seq", BYVAL, CVariant::VT_STRING),
		     CArgType("delim", BYVAL, CVariant::VT_STRING),
		     CArgType("elem", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    //STATUS API_CALL API_SeqSub(char* new_seq,const char* seq,const char* delim,int pos,int len);
    if (RegisterAPI( "SeqSub", SeqSub, false, CVariant::VT_STRING, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_SeqSub,
		     CAPIDef::CDECL_TYPE,
		     5,
		     CArgType("result", BYREF, CVariant::VT_RET),
		     CArgType("seq", BYVAL, CVariant::VT_STRING),
		     CArgType("delim", BYVAL, CVariant::VT_STRING),
		     CArgType("pos", BYVAL, CVariant::VT_INT),
		     CArgType("length", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    //STATUS API_CALL API_BitwiseAnd(int* result, int value1, int value2);
    if (RegisterAPI( "BitwiseAnd", BitwiseAnd, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_BitwiseAnd,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType("result", BYREF, CVariant::VT_RET),
		     CArgType("value1", BYVAL, CVariant::VT_INT),
		     CArgType("value2", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    //STATUS API_CALL API_BitwiseOr(int* result, int value1, int value2);
    if (RegisterAPI( "BitwiseOr", BitwiseOr, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_BitwiseOr,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType("result", BYREF, CVariant::VT_RET),
		     CArgType("value1", BYVAL, CVariant::VT_INT),
		     CArgType("value2", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    //STATUS API_CALL API_BitwiseXor(int* result, int value1, int value2);
    if (RegisterAPI( "BitwiseXor", BitwiseXor, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_BitwiseXor,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType("result", BYREF, CVariant::VT_RET),
		     CArgType("value1", BYVAL, CVariant::VT_INT),
		     CArgType("value2", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CpuGetProcessLoad(int *ret, int index);
    if (RegisterAPI( "CpuGetProcessLoad", CpuGetProcessLoad, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_CpuGetProcessLoad,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "index", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_CpuGetGlobalLoad(int *ret, int index);
    if (RegisterAPI( "CpuGetGlobalLoad", CpuGetGlobalLoad, false, CVariant::VT_INT, 
		     (CAPIProvider::APIPROC) &CSystemAPI::API_CpuGetGlobalLoad,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "ret", BYREF, CVariant::VT_RET),
		     CArgType( "index", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }


    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::Shutdown()
{
    API_Provider_Manager::Instance()->UnregisterProvider(this);
    return OK;
}



//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_GetTID(CTask *task, int *ret)
{
    assert(ret);
    *ret = task->GetPID();
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TaskAddr(CTask *task, int *ret)
{
    assert(task);
    *ret = task;
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_Halt(CTask *task)
{
    assert(task);
    return task->Quit();
}


//-----------------------------------------------------------------------------
// Set task priority: NOT SUPPORTED
/*
STATUS  CSystemAPI::API_SetTaskPriority( CTask *task, int prio)
{
    task->SetLastError(-1);
    CLog::Warning ( SYSAPI, task, "");
    return OK;
}
*/


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_GetLastError( CTask *task, int *ret)
{
    assert(task);
    assert(ret);
    *ret = task->GetLastError();
    return OK;
}



//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_LogInfo( CTask *task, const char *message)
{
    assert(task);
    assert(message);

    CLog::InfoApp( task->GetApplication()->Name().c_str(), task, "%s", message);
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_LogWarning( CTask *task, const char *message)
{
    CLog::WarningApp( task->GetApplication()->Name().c_str(), task, "%s", message);
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_LogError( CTask *task, const char *message)
{
    CLog::ErrorApp( task->GetApplication()->Name().c_str(), task, "%s", message);
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_Trace(CTask *task, const char *trace_criteria, const char *trace_msg)
{
    // ***TBD***: add filtering in terminal display
   // cout << "********* Inside API_TRACE ************** " << endl;
    CT_App_Manager::Instance()->AppTrace(task, trace_criteria, trace_msg);
    return OK;
}


//-----------------------------------------------------------------------------
STATUS CSystemAPI::API_ParamIsExist ( CTask *task, int *ret, const char *name)
{
    assert (task);
    assert (ret);

    String value;
    if (task->GetApplication()->GetAppParam(name, value, false) == false)
	*ret = 0;
    else
	*ret = 1;
    return OK;
}

//-----------------------------------------------------------------------------
STATUS CSystemAPI::API_ParamCreate ( CTask *task, const char *name, const char *value)
{
    assert(task);
    assert(name);
    assert(value);

    task->GetApplication()->AddAppParam (name, value);

    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_ParamGet( CTask *task, char *retstr, const char *param_name)
{
    assert (task);
    assert (retstr);
    assert (param_name);

    String value;
    task->GetApplication()->GetAppParam(param_name, value);
    safe_strcpy(retstr, value.c_str(), MAXAPPSTRLEN);
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_ParamSet( CTask *task, const char *param_name, const char *val)
{
    assert (task);
    assert (param_name);
    assert (val);

    task->GetApplication()->SetAppParam (param_name, val);
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_ArgGet( CTask *task, char *ret, int index)
{
    assert (task);
    assert (ret);
    
    if (index < 1) {
	*ret = '\0';
	CLog::Warning ( "SYS", task, "Invalid index %d in API ArgGet()", index);
	return OK;
    }

    const char *cmdline = task->GetExecContext()->GetArg(index-1);
    if (cmdline)
	safe_strcpy (ret, cmdline, MAXAPPSTRLEN);
    else
	*ret = '\0';
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_ArgGetCount ( CTask *task, int *ret)
{
    assert (task);
    assert (ret);
    
    *ret = task->GetExecContext()->GetArgCount();
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_Random(CTask *,int *retval,int maxval)
{
    assert (retval);

    if (maxval > 0) *retval = rand()%(maxval+1);
    else if (maxval == 0)
	*retval = 0;
    else {
	unsigned max = 0 - maxval;
	*retval = 0 - (rand() % (max+1));
    }
    return OK;
}



//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_SpawnTask ( CTask *task, int *ret, 
				    const char *appname, 
				    const char *arg0, const char *arg1, 
				    const char *arg2, const char *arg3)
{
    List <String> cmdline_arg;
    CTask *new_task;

    if (strlen(arg0)) {
	cmdline_arg.push_back (arg0);
	if (strlen(arg1)) {
	    cmdline_arg.push_back (arg1);
	    if (strlen(arg2)) {
		cmdline_arg.push_back (arg2);
		if (strlen(arg3)) {
		    cmdline_arg.push_back (arg3);
		}
	    }
	}
    }
    new_task = Kernel::Instance()->SpawnTask (appname, cmdline_arg, TASK_STARTUP_API);

    if (new_task == NULL)
	*ret = -1;
    else
	*ret = new_task->GetPID();
    return OK;
}

//-----------------------------------------------------------------------------
STATUS API_CALL CSystemAPI::API_TaskGetStartupType(CTask *task, int *type)
{
    *type = task->GetStartupType();
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  API_CALL CSystemAPI::API_TaskIsExist(int *ret, int pid)
{
    CTask *task;
    task = Kernel::Instance()->FindTask(pid);
    *ret = task ? 1 : 0;
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  API_CALL CSystemAPI::API_TaskIsExistByName(int *ret, const char *task_name)
{
    CTask *task;
    task = Kernel::Instance()->FindTaskByName(task_name);
    *ret = task ? 1 : 0;
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  API_CALL CSystemAPI::API_TaskSetIPCMessageByPID(int *ret, int pid, const char *msg, int ex_id)
{
    CTask *task;
    task = Kernel::Instance()->FindTask(pid);
    *ret = task ? 1 : 0;
    if(task) {
	task->SetIPCMessage(msg);
	if(ex_id != EX_NO_EXCEPTION)
	    task->RaiseException((TaskExceptionId)ex_id,"Task");
    }
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  API_CALL CSystemAPI::API_TaskSetIPCMessageByName(int *ret, const char *task_name, const char *msg, int ex_id)
{
    CTask *task;
    task = Kernel::Instance()->FindTaskByName(task_name);
    *ret = task ? 1 : 0;
    if(task) {
	task->SetIPCMessage(msg);
	if(ex_id != EX_NO_EXCEPTION)
	    task->RaiseException((TaskExceptionId)ex_id,"Task");
    }
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TaskGetIPCMessage( CTask *task, char *retstr)
{
    assert (task);
    sprintf(retstr, "%s",task->GetIPCMessage());
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TaskSetName( CTask *task, int *ret, const char *task_name)
{
    assert (task);
    CTask *_task;
    _task = Kernel::Instance()->FindTaskByName(task_name);
    *ret = _task && (_task != task) ? 0 : 1;
    if(!_task) {
	task->SetTaskName(task_name);
    }
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TaskGetName( CTask *task, char *retstr)
{
    assert (task);
    sprintf(retstr, "%s",task->GetTaskName());
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  API_CALL CSystemAPI::API_TaskCountByAppName(int *ret, const char *app_name)
{
    *ret = Kernel::Instance()->CountTaskByAppName(app_name);
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_GetHostname( CTask *task, char *retstr)
{
    assert (task);
    assert (retstr);

    gethostname(retstr, MAXAPPSTRLEN-1);
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_GetIPAddress( CTask *task, char *retstr)
{
    assert (task);
    assert (retstr);
    char hostname[128];

    gethostname(hostname, MAXAPPSTRLEN-1);
    hostent *h = gethostbyname(hostname);
    safe_strcpy(retstr,inet_ntoa(*(in_addr * )h->h_addr), 64);
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_EnableException(CTask *task)
{
    assert(task);
    task->EnableException();
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_DisableException(CTask *task)
{
    assert(task);
    task->DisableException();
    return OK;
}




//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_RaiseException( CTask *task, int ex_id)
{
    assert (task);

    task->RaiseException((TaskExceptionId)ex_id,"Task");
    Sleep_Service::Instance()->API_Sleep( task, 0);
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_GetExceptionReason( CTask *task, int *ret)
{
    assert (task);
    assert (ret);

    *ret = task->GetLastException();
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TmGetUTCTime( int *ret ) 
{
    assert (ret);
    *ret = mktime(gmtime((const long *)time(NULL)));
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TmGetTime( int *ret ) 
{
    assert (ret);
    *ret = time(NULL);
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TmStrFormat( CTask *task, char *ret, int time_int, const char *format) 
{
    assert (task);
    assert (ret);
    
    time_t temp = time_int;
    time(&temp);

    //tm *mytm = localtime( (time_t*)&time_int);
    tm * tmmytm = localtime(&temp);

    printf("tinu-TmStrFormat time : '%s'", tmmytm);
    
    strftime( ret, MAXAPPSTRLEN, format, tmmytm);
    printf("tinu-retvalue: '%s'", ret);
    //*ret = tmret;
 
    if (tmmytm == 0 || strftime( ret, MAXAPPSTRLEN, format, tmmytm) == 0) {
	
	*ret = '\0';
	CLog::Warning( SYSAPI, task, "API TmStrFormat() failed: invalid time conversion. "
				     "Input format is '%s'", format);
    }
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TmBuildTime( int *ret, int year, int month, int day, 
				     int hour, int min, int sec) 
{
    assert (ret);

    tm mytm;
    
    mytm.tm_sec = sec;
    mytm.tm_min = min;
    mytm.tm_hour = hour;
    
    mytm.tm_mday = day;
    mytm.tm_mon = month-1;
    mytm.tm_year = year-1900;
    mytm.tm_isdst = -1;
    
    *ret = (int)mktime( &mytm );
    if (*ret == (time_t)-1) {
	CLog::Warning ( SYSAPI, "API TmBuildTime() failed: invalid argument(s). "
				"day/mon/year hr:min:sec = %d/%d/%d %d:%d:%d.",
				day, month, year, hour, min, sec);
	*ret = 0;
    }

    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TmGetTimeEx( int *ret, int *msec)
{
    _timeb tb;
    _ftime(&tb);
    *ret = tb.time;
    *msec = tb.millitm;
    return OK;
}

static void ConvertTime (int t1, struct tm **t2)
{
    //printf ("time: %d", t1);
    struct tm *amit;
    time_t rawtime;
    time( &rawtime );
    amit = localtime(&rawtime);
    *t2 = amit;
    printf ("Current local time and date: %s", asctime(*t2));
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TmGetDayOfWeek (int *ret, int tm)
{
    struct tm **t;
    ConvertTime(tm, t);
    *ret = (*t)->tm_wday + 1;
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TmGetDayOfMonth (int *ret, int tm)
{
    struct tm **t;
    ConvertTime(tm, t);
    *ret = (*t)->tm_mday;
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TmGetDayOfYear (int *ret, int tm)
{
    struct tm **t;
    ConvertTime(tm, t);
    *ret = (*t)->tm_year + 1;
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TmGetMonth (int *ret, int tm)
{
    struct tm **t;
    ConvertTime(tm, t);
    *ret = (*t)->tm_mon + 1;
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TmGetYear (int *ret, int tm)
{
    struct tm **t;
    ConvertTime(tm, t);
    *ret = (*t)->tm_year + 1900;
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TmGetHour (int *ret, int tm)
{
    struct tm **t;
    ConvertTime(tm, t);
    *ret = (*t)->tm_hour;
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TmGetMinute (int *ret, int tm)
{
    struct tm **t;
    ConvertTime(tm, t);
    *ret = (*t)->tm_min;
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_TmGetSecond (int *ret, int tm)
{
    struct tm **t;
    ConvertTime(tm, t);
    *ret = (*t)->tm_sec;
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_Stoi( CTask *task, int *ret, const char *str) 
{
    assert (task);
    assert (ret);
    assert (str);

    if (*str==0) {
	*ret = 0;
	return OK;
    }

    char *err;
    *ret = strtol( str, &err, 10);
    if (*err) {
	CLog::Info( SYSAPI, task, "API Stoi() failed: non numeric argument '%s'. "
				      "Conversion stopped at character '%c'.", str, *err);
	task->SetLastError(-1);
    }
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_Stor( CTask *task, float *ret, const char *str)
{
    assert (task);
    assert (ret);
    assert (str);

    if (*str==0) {
	*ret = 0;
	return OK;
    }

    char *err;
    *ret = (float) strtod( str, &err);
    if (*err) {
	CLog::Info ( SYSAPI, task, "API Stor() failed: non numeric argument '%s'. "
				      "Conversion stopped at character '%c'.", str, *err);
	task->SetLastError(-1);
	return OK;
    }

    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_Itos( char *ret, int i)
{
    assert (ret);
    //itoa(i, ret, 10);
    sprintf(ret,"%d",i);
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_Itor( float *ret, int i)
{
    assert (ret);
    *ret = (float)i;
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_Rtos( char *ret, float f)
{
    assert (ret);

    const double near_zero = 0.0000000001;
    if ( f-floor(f) < near_zero) {
	int a=(int)f;
	sprintf( ret, "%d.0", a);
    } else
	sprintf( ret, "%f", f);
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_Rtoi( int *ret, float f)
{
    assert (ret);

    *ret = (int)f;
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_Round( float *ret, float f)
{
    assert (ret);

    if ((f - floor(f)) < 0.5)
	*ret = (float)floor(f);
    else
	*ret = (float)ceil(f);

    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrLen( int *ret, const char *str)
{
    assert (str);
    assert (ret);

    *ret = strlen(str);
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrSub( char *ret, const char *str, int from, int length)
{
    if (m_StringSafetyLevel==SAFE_STRING) {
	int srclen = strlen(str);
	if (from > srclen) {
	    *ret = '\0';
	    //CLog::Info( SYSAPI, "Strsub: 'from' parameter (%d) exceeds source string length (%d)", from, srclen);
	    return OK;
	} else if (from < 1) {
	    //*ret = '\0';
	    CLog::Warning( SYSAPI, "API Strsub() failed: invalid 'from' parameter. "
				   "Source string='%s', from=%d, length=%d", 
				   str, from, length);
	    //return OK;
	    from = 1;
	} else if (length<0) {
	    *ret = '\0';
	    //CLog::Info( SYSAPI, "Strsub: Invalid 'length' parameter (%d)", length);
	    return OK;
	}
    }

    safe_strcpy( ret, str+from-1, length+1);

    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrPos( int *ret, const char *str, const char *substr, int ipos)
{
    if (m_StringSafetyLevel==SAFE_STRING) {
	int srclen = strlen(str);

	if (ipos > srclen) {
	    /*
	    CLog::Warning( SYSAPI, "API StrPos() failed: 'ipos' parameter (%d) "
				   "exceeds source string length (%d). "
				   "Source string='%s'", ipos, srclen, str);
	    */
	    *ret = '\0';
	    return OK;
	} else if (ipos < 1) {
	    /*
	    CLog::Warning( SYSAPI, "API StrPos() failed: invalid 'ipos' parameter (%d). "
				   "Source string='%s'", ipos, str);
	    */
	    *ret = '\0';
	    return OK;
	}
    }

    char *pos = strstr( str+(ipos-1), substr);
    *ret = pos? (pos - str) + 1: 0;
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrLeft( char *ret, const char *str, int len)
{
    if (len<0) {
	*ret = '\0';
	CLog::Warning( SYSAPI, "API StrLeft() failed: invalid length parameter (%d). "
			       "Source string='%s'", len, str);
	return OK;
    }

    safe_strcpy( ret, str, len+1);
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrRight( char *ret, const char *str, int len)
{
    if (len<0) {
	*ret = '\0';
	CLog::Warning( SYSAPI, "API StrRight() failed: invalid length parameter (%d). "
			       "Source string='%s'", len, str);
	return OK;
    }

    int srclen = strlen(str);

    if (srclen==0) {
	*ret = '\0';
	return OK;
    }

    if (len>srclen)
	len = srclen;

    safe_strcpy( ret, str+(srclen-len), MAXAPPSTRLEN);
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrFromChr( char *ret, int chr)
{
    if (m_StringSafetyLevel==SAFE_STRING) {
	if (chr<0 || chr>255) {
	    *ret = '\0';
	    CLog::Warning( SYSAPI, "API StrFromChr() failed: invalid ASCII code %d", chr);
	    return OK;
	}
    }

    *ret++ = (char)chr;
    *ret = '\0';
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrGetChr( int *ret, const char *str, int ipos)
{
    if (m_StringSafetyLevel==SAFE_STRING) {
	if (ipos < 1 || ipos > (int)strlen(str)) {
	    *ret = 0;
	    /*
	    CLog::Warning( SYSAPI, "API StrGetChr() failed: invalid ipos parameter (%d). "
				   "Source string='%s'", ipos, str);
	    */
	    return OK;
	}
    }

    *ret = str[ipos-1];
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrTrim(char *ret,const char *str)
{
    char buffer[MAXAPPSTRLEN+1];
    return ((API_StrLTrim(buffer,str) == OK) && (API_StrRTrim(ret,buffer) == OK)) ? OK : NOT_OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrLTrim(char *ret,const char *str)
{
    safe_strcpy(ret,str, MAXAPPSTRLEN);
    sl::strltrim (ret);

    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrRTrim(char *ret,const char *str)
{
    safe_strcpy( ret, str, MAXAPPSTRLEN);
    sl::strrtrim (ret);

    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrLPad(char *ret,const char *str,const char *padding,int length)
{
    int padlen = length-strlen(str);

    // padding must not be empty
    if (strlen(padding) && (length <= MAXAPPSTRLEN)) {
	const char *p = padding;
	while (padlen-- > 0) {
	    *ret++ = *p++;
	    if (*p == '\0') p = padding;
	}
    }
    safe_strcpy(ret,str,MAXAPPSTRLEN);
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrRPad(char *ret,const char *str,const char *padding,int length)
{
    int padlen = length-strlen(str);

    safe_strcpy(ret,str,MAXAPPSTRLEN);
    ret += strlen(ret);
    if (strlen(padding) && (length <= MAXAPPSTRLEN)) {
	const char *p = padding;
	while (padlen-- > 0) {
	    *ret++ = *p++;
	    if (*p == '\0') p = padding;
	}
    }
    *ret = '\0';
    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrRegex(int *ret,const char *str,const char *pattern)
{
    regex_t reg;
    memset(&reg, 0, sizeof(reg));
    *ret = -1;
    if (::regcomp(&reg, pattern, REG_ICASE | REG_EXTENDED)) {
	::regfree(&reg);
	return OK;
    }

    if (::regexec(&reg, str, 0, NULL, 0) == REG_NOMATCH) {
	::regfree(&reg);
	return OK;
    }

    regfree(&reg);

    *ret = 0;
    return OK;
}


//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrUpper(char *ret,const char *str)
{
    safe_strcpy( ret, str, MAXAPPSTRLEN);
    //_strupr(ret);
    _strupr(ret);	//linux porting

    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrLower(char *ret,const char *str)
{
    safe_strcpy( ret, str, MAXAPPSTRLEN);
    //_strlwr(ret);
    _strlwr(ret);	//linux porting

    return OK;
}

//-----------------------------------------------------------------------------
STATUS  CSystemAPI::API_StrHexFromChr( char *ret, int chr)
{
    if (m_StringSafetyLevel==SAFE_STRING) {
	if (chr<0 || chr>255) {
	    *ret = '\0';
	    CLog::Warning( SYSAPI, "API StrHexFromChr() failed: invalid ASCII code %d", chr);
	    return OK;
	}
    }

	sprintf( ret, "%2.2x", chr);
    return OK;
}


/**
    SEQUENCE API

    SeqSize	: return number of element in list
    SeqGet	: get n-th element of list
    SeqRemove	: remove n-th element of list
    SeqFind	: return position of elem, 0 if not found, position start from 1
    SeqSub	: return sub list with n element starting at pos 

    func SeqSize(src,delim:string):integer;
    e.g:
    SeqSize("alice:bob:charlie:dude",":") return 4

    func SeqGet(src,delim:string;pos:integer):string;
    e.g:
    SeqGet("alice:bob:charlie:dude",":",3) return "charlie"
    SeqGet("alice:bob:charlie:dude",":",0) return ""

    func SeqRemove(src,delim:string;pos:integer):string;
    e.g:
    SeqRemove("alice:bob:charlie:dude",":",1) return "bob:charlie:dude"

    func SeqFind(src,delim,elem:string):integer;
    e.g:
    SeqFind("alice:bob:charlie:dude",":","bob") return 2
    SeqFind("alice:bob:charlie:dude",":","alice") return 1
    SeqFind("alice:bob:charlie:dude",":","lucky") return 0

    func SeqSub(src,delim:string;pos,n:integer):string;
    e.g:
    SeqSub("alice:bob:charlie:dude",":",1,2) return "alice:bob"
    SeqSub("alice:bob:charlie:dude",":",3,1) return "charlie"

*/

STATUS CSystemAPI::API_SeqSize(int* size,const char* seq,const char* delim)
{
    *size = 0;
    if (*seq=='\0' || *delim=='\0') 
	return OK;

    int dlen = strlen(delim);
    for(char* p=strstr(seq,delim);p!=NULL;p=strstr(p+dlen,delim))
	++*size;

    ++*size;

    return OK;
}

STATUS CSystemAPI::API_SeqGet(char* elem,const char* seq,const char* delim,int pos)
{
    *elem = '\0';
    if (*seq=='\0' || *delim=='\0' || pos<=0) 
	return OK;

    int cur_pos = 0;
    const char* beg = seq;
    int dlen = strlen(delim);

    for(char* end=strstr(beg,delim);end!=NULL;beg=end+dlen,end=strstr(beg,delim))
    {
	if (++cur_pos==pos)
	{
	    int len = end-beg;
	    strncpy(elem,beg,len);
	    elem[len] = '\0';

	    return OK;
	}
    }

    if (++cur_pos==pos)
	strcpy(elem,beg);
    
    return OK;
}

STATUS CSystemAPI::API_SeqRemove(char* new_seq,const char* seq,const char* delim,int pos)
{
    strcpy(new_seq,seq);
    if (*seq=='\0' || *delim=='\0' || pos<=0) 
	return OK;

    int cur_pos = 0;
    const char* beg = seq;
    int dlen = strlen(delim);

    for(char* end=strstr(beg,delim);end!=NULL;beg=end+dlen,end=strstr(beg,delim))
    {
	if (++cur_pos==pos)
	{
	    char tail[MAXAPPSTRLEN];

	    int len = beg-seq;
	    strncpy(new_seq,seq,len);
	    new_seq[len] = '\0';

	    strcpy(tail,end+dlen);
	    strcat(new_seq,tail);

	    return OK;
	}
    }

    if (++cur_pos==pos)
    {
	// handle single element first
	if (beg==seq)
	{
	    strcpy(new_seq,"");
	}
	else
	{
	    int len = beg-dlen-seq;
	    strncpy(new_seq,seq,len);
	    new_seq[len] = '\0';
	}
    }

    return OK;
}

STATUS CSystemAPI::API_SeqFind(int* pos,const char* seq,const char* delim,const char* elem)
{
    *pos = 0;
    if (*seq=='\0' || *delim=='\0' || *elem=='\0') 
	return OK;

    char* p = strstr(seq,elem);
    if (p==NULL)
	return OK;

    if (p==seq)
    {
	++*pos;
	return OK;
    }

    int dlen = strlen(delim);
    for(char* q=(char*)seq;q<p && q!=NULL;q=strstr(q+dlen,delim))
	++*pos;

    return OK;
}

STATUS CSystemAPI::API_SeqSub(char* new_seq,const char* seq,const char* delim,int pos,int nelem)
{
    *new_seq = '\0';
    if (*seq=='\0' || *delim=='\0' || nelem<=0 || pos<=0) 
	return OK;

    int cur_pos = 0;
    const char* beg = seq;
    int dlen = strlen(delim);

    for(char* end=strstr(beg,delim);end!=NULL;beg=end+dlen,end=strstr(beg,delim))
    {
	if (++cur_pos==pos)
	{
	    for(int i=1;i<nelem;++i)
	    {
		end = strstr(end+dlen,delim);
		if (end==NULL)
		    break;
	    }

	    if (end==NULL)
	    {
		strcpy(new_seq,beg);
	    }
	    else
	    {
		int len = end-beg;
		strncpy(new_seq,beg,len);
		new_seq[len] = '\0';
	    }

	    return OK;
	}
    }

    if (++cur_pos==pos)
	strcpy(new_seq,beg);

    return OK;
}

STATUS CSystemAPI::API_BitwiseAnd(int* result, int value1, int value2)
{
    *result = value1 & value2;
    return OK;
}

STATUS CSystemAPI::API_BitwiseOr(int* result, int value1, int value2)
{
    *result = value1 | value2;
    return OK;
}

STATUS CSystemAPI::API_BitwiseXor(int* result, int value1, int value2)
{
    *result = value1 ^ value2;
    return OK;
}

STATUS  CSystemAPI::API_CpuGetProcessLoad(int *ret, int index)
{
    switch (index) {
    // 1 second
    case 0:
	*ret = Process_Load_Monitor::Instance()->GetPercentUserTime(Load_Monitor::OneSecond) + 
	    Process_Load_Monitor::Instance()->GetPercentKernelTime(Load_Monitor::OneSecond);
	break;

    // 5 seconds
    case 1:
	*ret = Process_Load_Monitor::Instance()->GetPercentUserTime(Load_Monitor::FiveSeconds) + 
	    Process_Load_Monitor::Instance()->GetPercentKernelTime(Load_Monitor::FiveSeconds);
	break;

    // 1 minute
    case 2:
	*ret = Process_Load_Monitor::Instance()->GetPercentUserTime(Load_Monitor::OneMinute) + 
	    Process_Load_Monitor::Instance()->GetPercentKernelTime(Load_Monitor::OneMinute);
	break;
    
    // 5 minutes
    case 3:
	*ret = Process_Load_Monitor::Instance()->GetPercentUserTime(Load_Monitor::FiveMinutes) + 
	    Process_Load_Monitor::Instance()->GetPercentKernelTime(Load_Monitor::FiveMinutes);
	break;
    
    // 1 hour
    default:
	*ret = Process_Load_Monitor::Instance()->GetPercentUserTime(Load_Monitor::OneHour) + 
	    Process_Load_Monitor::Instance()->GetPercentKernelTime(Load_Monitor::OneHour);
	break;
    }

    return OK;
}

STATUS  CSystemAPI::API_CpuGetGlobalLoad(int *ret, int index)
{
    switch (index) {
    // 1 second
    case 0:
	*ret = CPU_Load_Monitor::Instance()->GetPercentUserTime(Load_Monitor::OneSecond) + 
	    CPU_Load_Monitor::Instance()->GetPercentKernelTime(Load_Monitor::OneSecond);
	break;

    // 5 seconds
    case 1:
	*ret = CPU_Load_Monitor::Instance()->GetPercentUserTime(Load_Monitor::FiveSeconds) + 
	    CPU_Load_Monitor::Instance()->GetPercentKernelTime(Load_Monitor::FiveSeconds);
	break;

    // 1 minute
    case 2:
	*ret = CPU_Load_Monitor::Instance()->GetPercentUserTime(Load_Monitor::OneMinute) + 
	    CPU_Load_Monitor::Instance()->GetPercentKernelTime(Load_Monitor::OneMinute);
	break;
    
    // 5 minutes
    case 3:
	*ret = CPU_Load_Monitor::Instance()->GetPercentUserTime(Load_Monitor::FiveMinutes) + 
	    CPU_Load_Monitor::Instance()->GetPercentKernelTime(Load_Monitor::FiveMinutes);
	break;
    
    // 1 hour
    default:
	*ret = CPU_Load_Monitor::Instance()->GetPercentUserTime(Load_Monitor::OneHour) + 
	    CPU_Load_Monitor::Instance()->GetPercentKernelTime(Load_Monitor::OneHour);
	break;
    }

    return OK;
}
