/* $Header: /xGate/odbc_exec.cpp 11    7/29/04 7:29p Bennylp $
 *   
 * XGATE PROJECT
 * (C) 2001 Switchlab, Ltd
 *
 * XGATE ODBC DATABASE.
 * ODBC SQL exec implementation.
 *
 * $Log: /xGate/odbc_exec.cpp $
 * 
 * 11    7/29/04 7:29p Bennylp
 * Zombie the connection upon really fatal error
 * 
 * 10    7/26/04 10:16a Bennylp
 * Disconnect ODBC connection upon TDS errors
 * 
 * 9     1/07/04 7:13p Sonny
 * Display the statement causing connection busy
 * 
 * 8     5/09/02 7:54p Bennylp
 * Several enhancements to database:
 *   - encrypt password in connect string (RC4 + base64)
 *   - added information about currently running SQL in a connection
 *   - added lock information in connection
 *   - added command to cancel currently running SQL
 *   - added command to force reconnection of a DB connection
 * 
 * 7     5/08/02 7:30p Bennylp
 * Select connection based on the least used && show query time.
 * 
 * 6     7/13/01 11:40a Bennylp
 * Show error instead of warning on SQL_SUCCESS_WITH_INFO.
 * 
 * 5     7/12/01 5:24p Bennylp
 * Remove fatal error 'statement has not been freed'
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
#include "log.h"
#include "task.h"

#define ODBC	"odbc"

///////////////////////////////////////////////////////////////////////////////
//
// ODBC Executor
//
///////////////////////////////////////////////////////////////////////////////
//
ODBC_SQL_Exec::ODBC_SQL_Exec( CTask *task, ODBC_Database *db)
: DB_SQL_Exec( task, db), m_RowsAffected(0),
  m_Cancelling(false), CheckExecuting(&ODBC_SQL_Exec::_ODBCExecuting)
{
    m_ExecState = XSTATE_ERROR; 
}

//-----------------------------------------------------------------------------
ODBC_SQL_Exec::~ODBC_SQL_Exec()
{
    assert (m_hstmt == SQL_NULL_HSTMT);
    GetParentDb()->Remove(this);
}


//-----------------------------------------------------------------------------
// initializing handles, etc. (doing things that can be share by several
// executions)
int ODBC_SQL_Exec::Init(int options)
{

    if ( (m_hstmt = GetParentDb()->AllocStmt()) == SQL_NULL_HSTMT) {
	m_ExecState = XSTATE_ERROR;
	return -1;
    }

    CLog::Detail( ODBC, GetTask(), "[%s] Handle 0x%X is allocated for task %d", 
				   GetParentDb()->GetProfile()->Name().c_str(),
				   m_hstmt, 
				   GetTask()->GetPID());

    return 0;
}

//-----------------------------------------------------------------------------
// Main function entry: Execute SQL
int ODBC_SQL_Exec::Execute(const char *sql)
{
    // Save the SQL string
    m_SQL = sql;

    // Mark async. execution
    OnAsyncBegin();
    m_TimeBeginExecuting = 0;
    return 0;
}


// Destroy the statement handle.
void ODBC_SQL_Exec::DestroyStmt()
{
    if (m_hstmt!=SQL_NULL_HSTMT) {
	MySQLFreeStmt(m_hstmt, SQL_DROP);
	m_hstmt = SQL_NULL_HSTMT;
    }
}


//-----------------------------------------------------------------------------
// Function for running the exec. and for polling the completion status
//
void ODBC_SQL_Exec::_ODBCExecuting()
{
    /*
    CLog::Detail( ODBC, GetTask(), "[%s] SQLExecDirect: handle 0x%X", 
		  GetParentDb()->GetProfile()->Name().c_str(),
		  m_hstmt);
    */

    // Execute the statement
    if (m_TimeBeginExecuting == 0)
	m_TimeBeginExecuting = time(NULL);

    RETCODE ret = ::SQLExecDirect( m_hstmt, (UCHAR*)m_SQL.c_str(), SQL_NTS);

    if ( ret==SQL_SUCCESS || ret==SQL_NO_DATA) {
	m_ExecState = XSTATE_READY;
	GetParentDb()->SetLastSqlQuery( this, m_SQL.c_str() );

    } else if (ret==SQL_SUCCESS_WITH_INFO) {
	// ODBC driver 3.x for SQL Server will issue SQL_SUCCESS_WITH_INFO
	// on error (sometimes)
	m_ExecState = XSTATE_ERROR;

	SetError( GetParentDb(), DB_ERROR, GetTask(), m_SQL.c_str(), GetParentDb()->GetHEnv(), 
					   GetParentDb()->GetHDbc(), m_hstmt, NULL);
	CLog::Info( ODBC, GetTask(), "Last successfull execution: %s", GetParentDb()->GetLastSqlQuery());

    } else if (ret != SQL_STILL_EXECUTING ) {
	m_ExecState = XSTATE_ERROR;
	SetError( GetParentDb(), DB_ERROR, GetTask(), m_SQL.c_str(),  GetParentDb()->GetHEnv(), 
					  GetParentDb()->GetHDbc(), m_hstmt, NULL);
	CLog::Info( ODBC, GetTask(), "Last successfull execution: %s", GetParentDb()->GetLastSqlQuery());

	// check for connection level error.
	const char *sql_state = GetLastSQLState();
	if (sql_state[0]=='0' && sql_state[1]=='8') {
	    String err;
	    if (GetODBCError(GetParentDb()->GetHEnv(), GetParentDb()->GetHDbc(), m_hstmt, err)) {
		err = "[";
		err += sql_state;
		err += "] Database connection link failure";
	    }
	    GetParentDb()->SignalDbFatal(err, false);
	}

    } else
	;   // must be still executing
}

//-----------------------------------------------------------------------------
// Internal: get affected rows
void ODBC_SQL_Exec::_ODBCGetRowCount()
{
    /*
    CLog::Detail( ODBC, GetTask(), "Executor _ODBCGetRowCount for handle 0x%X", m_hstmt);
    */

    //RETCODE ret=::SQLRowCount( m_hstmt, (SQLINTEGER*)&m_RowsAffected);
#if defined(__x86_64__)
    RETCODE ret=::SQLRowCount( m_hstmt, (SQLLEN*)&m_RowsAffected);
#else
    RETCODE ret=::SQLRowCount( m_hstmt, (SQLINTEGER*)&m_RowsAffected);
#endif

    if (ret==SQL_SUCCESS_WITH_INFO) {
	SetError( GetParentDb(), DB_WARNING, GetTask(), m_SQL.c_str(), GetParentDb()->GetHEnv(), 
					GetParentDb()->GetHDbc(), m_hstmt, NULL);
    } else if (ret != SQL_SUCCESS) {
	SetError( GetParentDb(), DB_ERROR, GetTask(), m_SQL.c_str(), GetParentDb()->GetHEnv(), 
					GetParentDb()->GetHDbc(), m_hstmt, NULL);
	m_ExecState = XSTATE_ERROR;
    }
}


//-----------------------------------------------------------------------------
// cleanup. 
// If the state is error, drop the statement. Otherwise give statement handle to 
// parent database to be destroyed or put into cache
void ODBC_SQL_Exec::_ODBCFree()
{
    if (m_LastExecError) {
	/*
	CLog::Detail( ODBC, "Freeing statement handle %X", m_hstmt);
	if ( MySQLFreeStmt( m_hstmt, SQL_DROP) != SQL_SUCCESS) {
	    SetError( GetParentDb(), DB_ERROR, 0, "Freeing statement", GetParentDb()->GetHEnv(), 
		      GetParentDb()->GetHDbc(), m_hstmt);
	}
	*/
	// statement handle must have been freed before, so we don't
	// have to do anything here

    } else {

	//GetParentDb()->ReleaseStmt( m_hstmt );
	if (m_hstmt!=SQL_NULL_HSTMT) {
	    assert (false);
	    CLog::Debug( ODBC, "[%s] Statement '%s' has not been freed",
			 GetParentDb()->GetProfile()->Name().c_str(),
			 m_SQL.c_str());
	    GetParentDb()->ReleaseStmt( m_hstmt );
	    m_hstmt = SQL_NULL_HSTMT;
	}
    }

    m_ExecState = XSTATE_READY;
}

//-----------------------------------------------------------------------------
// Perform ODBC ::SQLCancel
bool ODBC_SQL_Exec::SQLCancel()
{
    assert(GetState() == XSTATE_EXECUTING);
    if (GetState() == XSTATE_EXECUTING) {
	SQLRETURN ret=::SQLCancel(m_hstmt);
	if (ret != SQL_SUCCESS) {
	    SetError( GetParentDb(), DB_ERROR, GetTask(), m_SQL.c_str(), GetParentDb()->GetHEnv(), 
		      GetParentDb()->GetHDbc(), m_hstmt, NULL);
	    return false;
	}
    }
    return true;
}


//-----------------------------------------------------------------------------
// Polling completion status, if any
DB_Async_Object::ExecState ODBC_SQL_Exec::PollEvents()
{
    // If is still executing, check again
    if (GetState() == XSTATE_EXECUTING) {
	(this->*CheckExecuting)();

	if (GetState() == XSTATE_EXECUTING)
	    return XSTATE_EXECUTING;

	else if (GetState() == XSTATE_ERROR) {
	    CLog::Debug( ODBC, "[%s] Error in executing handle 0x%X; SQL='%s'", 
			       GetParentDb()->GetProfile()->Name().c_str(),
			       m_hstmt,
			       m_SQL.c_str());
	    if ( MySQLFreeStmt( m_hstmt, SQL_DROP) != SQL_SUCCESS) {
		SetError( GetParentDb(), DB_ERROR, 0, "Freeing handle", GetParentDb()->GetHEnv(), 
			  GetParentDb()->GetHDbc(), m_hstmt, NULL);
	    }
	    m_hstmt = SQL_NULL_HSTMT;
	    --n_opened_handle;
	    m_LastExecError = true;
	}

    } else {
	CLog::Fatal( "ODBC", GetTask(), "Executor is idle, but still polled");
    }

    if (m_ExecState == XSTATE_READY || GetState() == XSTATE_ERROR) {
	_ODBCGetRowCount();
	if (GetState() == XSTATE_ERROR)
	    m_LastExecError = true;

	// immediately release the statement
	GetParentDb()->ReleaseStmt( m_hstmt );
	m_hstmt = SQL_NULL_HSTMT;

    } else {
	m_RowsAffected = 0;
    }

    

    OnAsyncEnd();

    if (m_Quitting)
	return XSTATE_CLOSED;
    else
	return GetState();
}

//-----------------------------------------------------------------------------
// Cancel current async. execution
int ODBC_SQL_Exec::CancelExecuting()
{
    /*
    CLog::Detail( ODBC, GetTask(), "[%s] Executor CancelExecuting for task %d: do nothing", 
				   GetParentDb()->GetProfile()->Name().c_str(),
				   GetTask()->GetPID());
    */
    if (GetParentDb()->m_RunningObject.front() != this) {
	OnAsyncEnd();
	GetParentDb()->m_RunningObject.remove(this);

	m_RowsAffected = -1;
    } else {
	//SQLCancel();
    }

    /*
    // SQLCancel only attempt to cancel the func.
    RETCODE ret = ::SQLCancel( m_hstmt );
    if (ret != SQL_SUCCESS) {
	SetError( GetParentDb(), DB_FATAL, GetTask(), "SQLCancel failed", GetParentDb()->GetHEnv(),
				      GetParentDb()->GetHDbc(), m_hstmt);
	OnAsyncEnd();
	GetParentDb()->m_RunningObject.remove(this);
    }

    // Check if the cancellation is successful
    _ODBCExecuting();
    if (GetState() == XSTATE_EXECUTING) {
	m_Cancelling = true;
    } else {
	OnAsyncEnd();
	GetParentDb()->m_RunningObject.remove(this);
    }
    */

    return 0;
}

//-----------------------------------------------------------------------------
int ODBC_SQL_Exec::Close()
{
    if (GetState() == XSTATE_EXECUTING) {
	//CLog::Fatal( ODBC, "Executor is still executing when closed");
	m_Quitting = true;
	this->m_Task = NULL;
	return 0;
    }

    if (m_hstmt==SQL_NULL_HSTMT) {
	/* This can happen in case of database connection failure, which 
	   all statement handles will be closed by DB parent.
	 */
	GetParentDb()->Remove(this);
	delete this;
	return 0;
    }

    CheckExecuting = &ODBC_SQL_Exec::_ODBCFree;
    m_ExecState = XSTATE_EXECUTING;
    GetParentDb()->OnAsyncBegin(this);
    m_Quitting = true;
    this->m_Task = NULL;
    return 0;
}


