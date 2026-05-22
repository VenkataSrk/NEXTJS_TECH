/* $Header: /xGate/odbc_base.cpp 6     7/26/04 10:15a Bennylp $
 *   
 * XGATE PROJECT
 * (C) 2001 Switchlab, Ltd
 *
 * XGATE ODBC DATABASE.
 * Base ODBC services.
 *
 * $Log: /xGate/odbc_base.cpp $
 * 
 * 6     7/26/04 10:15a Bennylp
 * Disconnect ODBC connection upon TDS errors
 * 
 * 5     5/08/02 7:30p Bennylp
 * Select connection based on the least used && show query time.
 * 
 * 4     6/08/01 2:56p Bennylp
 * Fixed bugs in dbdriver (reload).
 * 
 * 3     6/06/01 7:09p Bennylp
 * Supports dynamic config and auto-connect when connection failed.
 * 
 * 2     4/28/01 6:33a Bennylp
 * Added service profiling and database driver (testing)
 */
#include "stdafx.h"
#include "odbc.h"
#include "types.h"
#include "task.h"
#include "dbdriver.h"
#include "log.h"

#define ODBC	"odbc"

///////////////////////////////////////////////////////////////////////////////
//
// DB_Async_Object
//
///////////////////////////////////////////////////////////////////////////////

// call this to signal beginning of async. function call
void DB_Async_Object::OnAsyncBegin()
{
    if (m_ExecState == XSTATE_EXECUTING) {
	CLog::Fatal( ODBC, "DB_Async_Object is already executing");
	return;
    }

    m_ExecState = XSTATE_EXECUTING;
    GetParentDb()->OnAsyncBegin(this);
    if (m_Task)
	m_Task->Sleep( GetParentDb()->GetDriver() );

    m_TimeBeginExecuting = time(NULL);
}

//-----------------------------------------------------------------------------
// call this to signal end of async. function call
void DB_Async_Object::OnAsyncEnd()
{
    GetParentDb()->OnAsyncEnd(this);
    if (m_ExecState == XSTATE_EXECUTING)
	m_ExecState = XSTATE_READY;
}



///////////////////////////////////////////////////////////////////////////////
//
// ODBC base class
//
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Miscelanous ways to report error
void ODBC_Base::SetError( ODBC_Database *db, LogVerbosity severity, CTask *task, const char *title, HENV henv)
{
    SetError( db, severity, task, title, henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, NULL);
}

//-----------------------------------------------------------------------------
void ODBC_Base::SetError( ODBC_Database *db, LogVerbosity severity, CTask *task, const char *title, HENV henv, 
			  HDBC hdbc)
{
    SetError( db, severity, task, title, henv, hdbc, SQL_NULL_HSTMT, NULL);
}

//-----------------------------------------------------------------------------
void ODBC_Base::SetError( ODBC_Database *db, LogVerbosity severity, CTask *task, const char *title, HENV henv, 
			  HDBC hdbc, HSTMT hstmt, String *saved_err)
{
    SDWORD lNative; 
    SWORD nOutlen = 0;  
    char errstr[SQL_MAX_MESSAGE_LENGTH]; 
    
    //::SQLError(	henv, hdbc, hstmt, (UCHAR*)sqlState, &lNative, 
		//(UCHAR*)errstr, SQL_MAX_MESSAGE_LENGTH-1, &nOutlen); 	//commented for linux porting
    
    errstr[nOutlen] = '\0'; 

    if (saved_err) {
	*saved_err = errstr;
    }

    // determine warning level based on sqlState.
    if (sqlState[0]=='0' && sqlState[1]=='0') {
	// SQL State indicates success
	severity = LOG_DEBUG;
    } else if (sqlState[0]=='0' && sqlState[1]=='1') {
	// SQL State indicates warning with accompanied by SQL_SUCCESS_WITH_INFO
	severity = LOG_DEBUG;
    } else if (sqlState[0]=='I' && sqlState[1]=='M') {
	// SQL State indicates warning/errors from ODBC implementation
	severity = LOG_WARNING;
    }

    CLog::Log( severity, ODBC, task, "[%s] %s: [%s] %s", db->GetProfile()->Name().c_str(), title, sqlState, errstr);
}

//-----------------------------------------------------------------------------
int ODBC_Base::GetODBCError(HENV henv, HDBC hdbc, HSTMT hstmt, String & err)
{
    SDWORD lNative; 
    SWORD nOutlen = 0;  
    char sql_state[6];
    char errstr[SQL_MAX_MESSAGE_LENGTH]; 
    SQLRETURN ret;

    //ret = ::SQLError(henv, hdbc, hstmt, (UCHAR*)sql_state, &lNative, 
		     //(UCHAR*)errstr, SQL_MAX_MESSAGE_LENGTH-1, &nOutlen); 
    
    if (nOutlen > SQL_MAX_MESSAGE_LENGTH)
	nOutlen = SQL_MAX_MESSAGE_LENGTH;
    errstr[nOutlen] = '\0'; 
    err = errstr;
    return ret;
}

//-----------------------------------------------------------------------------
void ODBC_Base::SetError( ODBC_Database *db, LogVerbosity severity, CTask *task, const char *title, 
			  const char *msg)
{
    CLog::Log( severity, ODBC, task, "[%s] %s: %s", db->GetProfile()->Name().c_str(), title, msg);  
}
