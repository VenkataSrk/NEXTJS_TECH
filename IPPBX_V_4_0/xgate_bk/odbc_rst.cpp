/* $Header: /xGate/odbc_rst.cpp 23    7/29/04 7:29p Bennylp $
 *   
 * XGATE PROJECT
 * (C) 2001 Switchlab, Ltd
 *
 * XGATE ODBC DATABASE.
 * ODBC recordset manip. implementation.
 *
 * $Log: /xGate/odbc_rst.cpp $
 * 
 * 23    7/29/04 7:29p Bennylp
 * Zombie the connection upon really fatal error
 * 
 * 22    7/26/04 10:16a Bennylp
 * Disconnect ODBC connection upon TDS errors
 * 
 * 21    4/23/04 4:35p Bennylp
 * Experimental Unicode string support (returned as BASE64)
 * 
 * 20    3/19/04 12:49p Bennylp
 * Preliminary support for Unicode columns
 * 
 * 19    1/07/04 7:13p Sonny
 * Display the statement causing connection busy
 * 
 * 18    7/16/03 11:48a Bennylp
 * Add more buffer to each of database value's buffer (suspected bug with
 * DateTime as shown by BugSlayer)
 * 
 * 17    3/06/03 5:14p Bennylp
 * Changed GetValDate() to use memcpy
 * 
 * 16    2/19/03 10:38a Bennylp
 * Changed std::string to C string in DB_Col (suspect bug)
 * 
 * 15    5/09/02 7:54p Bennylp
 * Several enhancements to database:
 *   - encrypt password in connect string (RC4 + base64)
 *   - added information about currently running SQL in a connection
 *   - added lock information in connection
 *   - added command to cancel currently running SQL
 *   - added command to force reconnection of a DB connection
 * 
 * 14    5/08/02 7:30p Bennylp
 * Select connection based on the least used && show query time.
 * 
 * 13    11/19/01 1:26p Bennylp
 * Fixed bug when xGate crashes if SQL returns no data.
 * 
 * 12    7/11/01 19:36 Bennylp
 * Fixed bug when there are too many columns
 * 
 * 11    8/09/01 5:44p Bennylp
 * Reduce the number of CLog::Detail calls.
 * 
 * 10    6/13/01 8:33p Bennylp
 * Check EOF/BOF in dbGetValueXX
 * 
 * 9     6/08/01 2:56p Bennylp
 * Fixed bugs in dbdriver (reload).
 * 
 * 8     6/06/01 7:09p Bennylp
 * Supports dynamic config and auto-connect when connection failed.
 * 
 * 7     5/26/01 6:52p Bennylp
 * Fixed bug when SQLMoreResults returns SQL_NO_DATA.
 * 
 * 6     5/26/01 2:49p Bennylp
 * Don't call task->GetPID() in CancelExecuting.
 * 
 * 5     5/25/01 9:42p Bennylp
* Fixed database connection busy.
* 
* 4     5/24/01 2:08p Bennylp
* Fix bug statement not closed.
* 
* 3     4/30/01 7:45a Bennylp
* Integrate database service.
* 
	* 2     4/28/01 6:33a Bennylp
* Added service profiling and database driver (testing)
	*/
	//#include "stdafx.h"
#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"
#include "types.h"
#include "log.h"
#include "task.h"
#include "sl.h"
#include "dbdriver.h"
#include "../slsip/encryption/base64.h"
#include "string.h" 
using namespace std;


	// TODO:
	//  - check again: GMT time

#define ODBC    "odbc"

	//-----------------------------------------------------------------------------
	// DATE CONVERSION FUNCTIONS
time_t TimestampToTime_t( const TIMESTAMP_STRUCT &d )
{
	tm mytm;

	mytm.tm_sec = d.second;
	mytm.tm_min = d.minute;
	mytm.tm_hour = d.hour;

	mytm.tm_mday = d.day;
	mytm.tm_mon = d.month-1;
	mytm.tm_year = d.year-1900;
	mytm.tm_isdst = -1;

	time_t t = mktime( &mytm );
	if (t == (time_t)-1)
		return 0;

	return t;
}

//-----------------------------------------------------------------------------
// DATE CONVERSION FUNCTIONS
int Time_tToTimestamp( time_t t, TIMESTAMP_STRUCT *d)
{
	// ***jnt***
	//struct tm *tmptr = gmtime(&t);
	struct tm *tmptr = localtime(&t);

	if (!tmptr)
		return -1;

	d->day = (unsigned short)tmptr->tm_mday;
	d->fraction = 0;
	d->hour = (unsigned short)tmptr->tm_hour;
	d->minute = (unsigned short)tmptr->tm_min;
	d->month = (unsigned short)(tmptr->tm_mon+1);
	d->second = (unsigned short)tmptr->tm_sec;
	d->year = (unsigned short)(tmptr->tm_year+1900);

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// ODBC recordset
//
///////////////////////////////////////////////////////////////////////////////

// INLINES

//-----------------------------------------------------------------------------
// State change
inline void ODBC_Recordset::SetState_ST_CLOSED()
{
	m_State=ST_CLOSED;
	CheckExecuting = &ODBC_Recordset::_ODBCIdle;
	MoveState = &ODBC_Recordset::_SH_Closed;
}

inline void ODBC_Recordset::SetState_ST_OPENING() 
{
	m_State=ST_OPENING;
	CheckExecuting = &ODBC_Recordset::_ODBCOpen;
	MoveState = &ODBC_Recordset::_SH_Opening;
	OnAsyncBegin();
}

inline void ODBC_Recordset::SetState_ST_NCOL() 	
{
	m_State=ST_NCOL;
	CheckExecuting = &ODBC_Recordset::_ODBCNCol;
	MoveState = &ODBC_Recordset::_SH_NCol;
}

inline void ODBC_Recordset::SetState_ST_COL_ATTR() 	
{
	m_State=ST_COL_ATTR;
	CheckExecuting = &ODBC_Recordset::_ODBCColAttr;
	MoveState = &ODBC_Recordset::_SH_ColAttr;
}
inline void ODBC_Recordset::SetState_ST_READY() 	
{
	m_State=ST_READY;
	CheckExecuting = &ODBC_Recordset::_ODBCIdle;
	MoveState = &ODBC_Recordset::_SH_Ready;
	OnAsyncEnd();
}
inline void ODBC_Recordset::SetState_ST_DELETING() 	
{
	m_State=ST_DELETING;
	OnAsyncBegin();
	CheckExecuting = &ODBC_Recordset::_ODBCDelete;
	MoveState = &ODBC_Recordset::_SH_Deleting;
}
inline void ODBC_Recordset::SetState_ST_REQUERY() 	
{
	m_State=ST_REQUERY;
	OnAsyncBegin();
	CheckExecuting = &ODBC_Recordset::_ODBCRequery;
	MoveState = &ODBC_Recordset::_SH_Requery;
}
inline void ODBC_Recordset::SetState_ST_UPDATING() 	
{
	m_State=ST_UPDATING;
	OnAsyncBegin();
	CheckExecuting = &ODBC_Recordset::_ODBCUpdate;
	MoveState = &ODBC_Recordset::_SH_Updating;
}
inline void ODBC_Recordset::SetState_ST_MOVING() 	
{
	m_State=ST_MOVING;
	OnAsyncBegin();
	CheckExecuting = &ODBC_Recordset::_ODBCMoveTo;
	MoveState = &ODBC_Recordset::_SH_Moving;
}
inline void ODBC_Recordset::SetState_ST_CLOSING()
{
	m_State=ST_CLOSING;
	CheckExecuting = &ODBC_Recordset::_ODBCClose;
	MoveState = &ODBC_Recordset::_SH_Closing;

	// put on parent's running queue
	GetParentDb()->OnAsyncBegin(this);

	m_ExecState = XSTATE_EXECUTING;
}
inline void ODBC_Recordset::SetState_ST_MORE_RESULT()
{
	m_State=ST_MORE_RESULT;
	CheckExecuting = &ODBC_Recordset::_ODBCMoreResult;
	MoveState = &ODBC_Recordset::_SH_MoreResult;
	m_ExecState = XSTATE_EXECUTING;
}

//-----------------------------------------------------------------------------
// Convenience function: set error
void ODBC_Recordset::SetError( LogVerbosity severity, 
		const char *title, const char *msg,
		String *saved_msg)
{
	if (msg) {
		ODBC_Base::SetError( GetParentDb(), severity, GetTask(), title, msg);
	} else {
		ODBC_Base::SetError( GetParentDb(), severity, GetTask(), title, GetParentDb()->GetHEnv(), 
				GetParentDb()->GetHDbc(), m_hstmt, saved_msg);
	}
}

//-----------------------------------------------------------------------------
// CTOR
	ODBC_Recordset::ODBC_Recordset(CTask *task, ODBC_Database *parent)
: DB_Recordset( task, parent ), m_NColumn(0), m_EOF(false), m_BOF(false),
	m_bAddRow(false), m_HasMoreResult(false), m_HasConnectionLock(false),
	m_Cancelling(false)
{
	SetState_ST_CLOSED();
	m_BoundData.clear();
}

//-----------------------------------------------------------------------------
// DTOR
ODBC_Recordset::~ODBC_Recordset()
{
	for (unsigned int i=0; i<m_BoundData.size(); ++i)
		//delete m_BoundData[i].m_Buf;
		free(m_BoundData[i].m_Buf);
	if (m_HasConnectionLock)
		UnlockConnection();

	assert (m_hstmt == SQL_NULL_HSTMT);
	GetParentDb()->Remove(this);
}


void ODBC_Recordset::LockConnection()
{
	// This can be called more than once.
	//assert (m_HasConnectionLock==false);
	if (m_HasConnectionLock==false) {
		/*
		   CLog::Detail ( ODBC, GetTask(), "[%s] Locking database %p", 
		   GetParentDb()->GetProfile()->Name().c_str(),
		   this);
		   */
		m_HasConnectionLock = true;
		GetParentDb()->Lock (this);
	}
}


void ODBC_Recordset::UnlockConnection()
{
	assert (m_HasConnectionLock);
	/*
	   CLog::Detail ( ODBC, GetTask(), "[%s] Unlocking database %p", 
	   GetParentDb()->GetProfile()->Name().c_str(),
	   this);
	   */
	if (m_HasConnectionLock) {
		m_HasConnectionLock = false;
		GetParentDb()->Unlock(this);
	}
}

//-----------------------------------------------------------------------------
// Update execution state based on function's return code
RETCODE ODBC_Recordset::UpdateExecState(const char *last_cmd, RETCODE ret)
{
	if ( ret == SQL_SUCCESS ) {
		m_ExecState = XSTATE_READY;
		GetParentDb()->SetLastSqlQuery(this, m_SQL.c_str());
		//CLog::Detail( ODBC, GetTask(), "SQL_SUCCESS, state is XSTATE_READY");

	} else if (ret == SQL_SUCCESS_WITH_INFO) {
		SetError( DB_INFO, last_cmd, NULL, NULL);
		//CLog::Info( ODBC, GetTask(), "Last successfull execution: %s", GetParentDb()->GetLastSqlQuery());

		m_ExecState = XSTATE_READY;

		GetParentDb()->SetLastSqlQuery(this, m_SQL.c_str());

		SQLINTEGER type; 
		SQLINTEGER read;
		::SQLGetStmtAttr( m_hstmt, SQL_ATTR_CURSOR_TYPE, &type, sizeof(type), &read);
		CLog::Debug( ODBC, GetTask(), "[%s] Cursor type is now %d", 
				GetParentDb()->GetProfile()->Name().c_str(),
				type);

	} else if (ret == SQL_STILL_EXECUTING) {
		m_ExecState = XSTATE_EXECUTING;
		//CLog::Detail( ODBC, GetTask(), "SQL_STILL_EXECUTING");

	} else if (ret == SQL_NO_DATA) {
		m_EOF = m_BOF = true;
		m_ExecState = XSTATE_READY;
		GetParentDb()->SetLastSqlQuery(this, m_SQL.c_str());

	} else {
		String db_err;
		SetError( DB_ERROR, last_cmd, NULL, &db_err);
		CLog::Info( ODBC, GetTask(), "Last successfull execution: %s", GetParentDb()->GetLastSqlQuery());
		//cout << "*** setting xstate_error ***" << endl;
		m_ExecState = XSTATE_ERROR;
		//GetParentDb()->SetLastSqlQuery("ERROR");

		OnAsyncEnd();

		// check for connection level error.
		const char *sql_state = GetLastSQLState();

		if (sql_state[0]=='0' && sql_state[1]=='8') {
			// Connection error.
			//CLog::Info( ODBC, GetTask(), "Current ODBC connection will be forcefully disconnected");
			GetParentDb()->SignalDbFatal(db_err, false);
		}
		// Handle:
		//  - Protocol error in TDS stream.
		//  - TDS buffer length too large.
		//  - Unknown token received from SQL Server.
		else if (sql_state[0]=='H' && sql_state[1]=='Y') {

			if (strstr(db_err.c_str(), "TDS") || strstr(db_err.c_str(), "token")) {
				CLog::Error( ODBC, GetTask(), "Current ODBC connection will be zombied!");
				GetParentDb()->SignalDbFatal(db_err, true);
			}
		}
	}

	return ret;
}

//-----------------------------------------------------------------------------
// Cancel currently executing ODBC func. If this stmt. is not executed yet,
// simply just remove it from the parent db. If it is already executed,
// issue ODBC SQLCancel. The cancelling procedure will be finished in the
// current CheckExecuting handler.
//
// Note:
//  - ODBC version 3 is strictly required.
//
int ODBC_Recordset::CancelExecuting()
{
	/*
	   CLog::Detail( ODBC, GetTask(), "[%s] CancelExecuting", 
	   GetParentDb()->GetProfile()->Name().c_str());
	   */
	if (GetParentDb()->m_RunningObject.front() != this) {
		OnAsyncEnd();
		GetParentDb()->m_RunningObject.remove(this);
	} else {
		//RETCODE ret = ::SQLCancel(m_hstmt);
		//SQLCancel();
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Idle handler
void ODBC_Recordset::_ODBCIdle()
{
	// should not be called in the first place
	m_ExecState = XSTATE_ERROR;
	SetError( DB_FATAL, "ODBCIdle", "Should not be called in this state", NULL);
}

#define MAX_CHAR 1024
//-----------------------------------------------------------------------------
// ODBC: Open a recordset
void ODBC_Recordset::_ODBCOpen()
{

	/*    CLog::Detail( ODBC, GetTask(), "[%s] _ODBCOpen for handle 0x%X", 
		  GetParentDb()->GetProfile()->Name().c_str(),
		  m_hstmt);*/

	SQLHSTMT tmphstmt = m_hstmt;

	if (m_TimeBeginExecuting==0)
		m_TimeBeginExecuting = time(NULL);   
	/* printf("**************** SQLExecDirect and m_SQL.c_str() is %s\n",(UCHAR*)m_SQL.c_str()); 
        */

	//if(m_hstmt == SQL_NULL_HSTMT) 
	//printf("***** Statement handle is NULLL *****\n");
	//printf("***** m_hstmt is not NULL %ld  *****\n",m_hstmt);


	SQLINTEGER rtnPersonExtn = 1;
	RETCODE ret = ::SQLExecDirect(m_hstmt, (SQLCHAR *)m_SQL.c_str(), SQL_NTS);


	if(ret == SQL_SUCCESS){
		//cout << "**** SQLExecDirect is successfull *********" << endl;
#if 0
		SQLBindCol(m_hstmt, 1, SQL_C_LONG, &rtnPersonExtn, sizeof(rtnPersonExtn), NULL );
		ret = SQLFetch(m_hstmt);
		if(ret == SQL_NO_DATA_FOUND)
			cout << "*** No sql data found *****"<< endl;
		cout << "The result of the stored procedure is "<< endl;
		cout << rtnPersonExtn << endl;

#endif


	}



	UpdateExecState( m_SQL.c_str(), ret);


	if (ret==SQL_NO_DATA) {
		cout << "**** SQL_NO_DATA *********" << endl;
		SetError( DB_DEBUG, m_SQL.c_str(), "SQL returned no data, will try next resulsets", NULL);
		//Let the execution continues
		//Error will be detected later in NCol, at least.
		//m_ExecState = XSTATE_ERROR;
	}
	
}


//-----------------------------------------------------------------------------
// ODBC: Close and free the recordset
void ODBC_Recordset::_ODBCClose()
{
	//cout << "*** Inside _ODBCClose close and free the recordset **** " << endl;
	if (m_LastExecError) {
		// statement handle must have been freed before, so we don't have
		// to do anything here
		assert (m_hstmt == SQL_NULL_HSTMT);
		if (m_hstmt != SQL_NULL_HSTMT) {
			if (MySQLFreeStmt( m_hstmt, SQL_DROP) != SQL_SUCCESS) {
				SetError( DB_ERROR, 0, "Error in freeing handle", NULL);
			}
			m_hstmt = SQL_NULL_HSTMT;
		}

	} else {
		GetParentDb()->ReleaseStmt( m_hstmt );
		m_hstmt = SQL_NULL_HSTMT;
	}

	m_ExecState = XSTATE_READY;
	if (m_HasConnectionLock)
		UnlockConnection ();
}

//-----------------------------------------------------------------------------
// ODBC: retrieve number of columns. Called right after SQLExecDirect is 
// finished executing
void ODBC_Recordset::_ODBCNCol()
{
	/*
	   CLog::Detail( ODBC, GetTask(), "[%s] _ODBCNCol for handle 0x%X", 
	   GetParentDb()->GetProfile()->Name().c_str(),
	   m_hstmt);
	   */

	//cout << "*** Inside _ODBCCol **** " << endl;
	RETCODE ret;
	//SDWORD count;
//	int count = 0 ; //linux porting
	//SQLINTEGER count = 0;	
#if defined(__x86_64__)
        SQLLEN count = 0;
#else
        SQLINTEGER count = 0;
#endif


	ret = ::SQLColAttributes( m_hstmt, 1, SQL_COLUMN_COUNT, 0, 0, 0, 
			//(SDWORD*)&count);	//linux porting
		&count);	//linux porting

	if (count >= DB_MAX_COL-1) {
		CLog::Warning (ODBC, GetTask(), "Too many columns (%d) in statement: '%s'",
				count, m_SQL.c_str());
		ret = SQL_ERROR;
	}
	UpdateExecState( m_SQL.c_str(), ret);
	if ( IsExecSuccess(ret)) {
		m_NColumn = count;
	//cout << "*** Inside _ODBCCol, column count is"<< m_NColumn << " **** " << endl;
	} else {
		m_NColumn = 0;
	}
}


//-----------------------------------------------------------------------------
// ODBC: Move to the next resultset.
void ODBC_Recordset::_ODBCMoreResult()
{
	/*
	   CLog::Detail( ODBC, GetTask(), "[%s] _ODBCMoreResult for handle 0x%X", 
	   GetParentDb()->GetProfile()->Name().c_str(),
	   m_hstmt);
	   */
	//cout << "*** Inside _ODBCMoreResult **** " << endl;
	RETCODE ret;

	ret = ::SQLMoreResults( m_hstmt );

	UpdateExecState( m_SQL.c_str(), ret);

	if (ret==SQL_NO_DATA) {
		/*
		 * No more result sets
		 */
		SetError( DB_ERROR, m_SQL.c_str(), "Stored procedure doesn't return recordset", NULL);
		m_ExecState = XSTATE_ERROR;
		OnAsyncEnd();

	} else if (ret==SQL_SUCCESS || ret==SQL_STILL_EXECUTING || SQL_SUCCESS_WITH_INFO) {
		m_HasMoreResult = true;
		LockConnection();
	}
}


//-----------------------------------------------------------------------------
// ODBC: Retrieve column attributes from recordset. Called right after _ODBCNCol
void ODBC_Recordset::_ODBCColAttr()
{
	/*
	   CLog::Detail( ODBC, GetTask(), "[%s] _ODBCColAttr for handle 0x%X", 
	   GetParentDb()->GetProfile()->Name().c_str(),
	   m_hstmt);
	   */
	//cout << "*** Inside _ODBCColAttr **** " << endl;
	if (GetColumnCount()==0) {
		CLog::Debug (ODBC, GetTask(), "[%s] Resultset has no column; SQL='%s'", 
				GetParentDb()->GetProfile()->Name().c_str(),
				m_SQL.c_str());
		return;
	}

	RETCODE ret;
	char col_name[DB_MAX_COL_NAME];
	SWORD col_len, col_type, col_scale, col_nullable;
	#if 0
	//UDWORD col_size;
	unsigned long col_size;	//linux porting
	#else
	//SQLUINTEGER col_size;
#if defined(__x86_64__)
               SQLULEN col_size;
#else
               SQLUINTEGER col_size;
#endif

        m_ColAttrIdx = 1;

	#endif	

	while( GetState() == XSTATE_READY && m_ColAttrIdx <= GetColumnCount()) {
		ret = ::SQLDescribeCol( m_hstmt, (SQLSMALLINT)m_ColAttrIdx, 
				(UCHAR*)col_name, 
				(SWORD)DB_MAX_COL_NAME-1,
				(SWORD*)&col_len, (SWORD*)&col_type, 
				//(UDWORD*)&col_size, (SWORD*)&col_scale,		//linux porting
				(SQLULEN*)&col_size, (SWORD*)&col_scale,
		//		&col_size, (SWORD*)&col_scale,
				(SWORD*)&col_nullable);

		UpdateExecState( m_SQL.c_str(), ret);
		if (IsExecSuccess(ret)) {
			col_name[DB_MAX_COL_NAME-1] = '\0';
			strcpy(m_Column[m_ColAttrIdx].m_Name, col_name);
			//m_Column[m_ColAttrIdx].m_Name = col_name;
			m_Column[m_ColAttrIdx].m_Type = ODBCToOurType(col_type);
			m_Column[m_ColAttrIdx].m_Length = col_size+1;
			m_Column[m_ColAttrIdx].m_IsUpdatable = 1;
			m_Column[m_ColAttrIdx].UpdateDataLength();

			++m_ColAttrIdx;
		}
	}
}


//-----------------------------------------------------------------------------
// Bind column col_idx. 
void ODBC_Recordset::_ODBCBindColumn(int col_idx)
{
	if (m_Column[col_idx].m_Bound)
		return;

	/*
	   CLog::Detail( ODBC, GetTask(), "[%s] _ODBCBindColumn for handle 0x%X", 
	   GetParentDb()->GetProfile()->Name().c_str(),
	   m_hstmt);
	   */
	RETCODE 
		ret = ::SQLBindCol( m_hstmt, (SQLUSMALLINT) col_idx, 
				OursToODBCType( m_Column[col_idx].m_Type ),
				m_BoundData [col_idx].m_Buf,
				m_Column[col_idx].Size(),
				(SQLLEN*) &m_BoundData [col_idx] .m_LI);

	UpdateExecState( m_SQL.c_str(), ret);
	if (IsExecSuccess(ret)) {
		m_Column[col_idx].m_Bound = true;
	}
}

//-----------------------------------------------------------------------------
// Bind column to a buffer which already holds data (actually, the difference
// with BindColumn() above is not clear)
void ODBC_Recordset::_ODBCBindValue(int col_idx)
{
	if (m_Column[col_idx].m_Bound)
		return;

	/*
	   CLog::Detail( ODBC, GetTask(), "_ODBCBindValue for handle 0x%X", m_hstmt);
	   */

	RETCODE ret;
	ODBC_Bound_Data *bound_data = &m_BoundData [col_idx];
	DB_Column_Desc *column = &m_Column[col_idx];

	// bind column to buffer
	if (m_Column[col_idx].m_Type == DB_Column_Desc::DB_STRING ||
			m_Column[col_idx].m_Type == DB_Column_Desc::DB_BINARY ||
			m_Column[col_idx].m_Type == DB_Column_Desc::DB_WSTRING) 
	{
		bound_data->m_LI = column->Size()-1;
		ret = ::SQLBindCol( m_hstmt, (SQLUSMALLINT) col_idx, 
				OursToODBCType( column->m_Type ),
				bound_data->m_Buf,
				bound_data->m_LI,
				(SQLLEN*) &bound_data->m_LI);

	} else {
		bound_data->m_LI = column->DataSize();
		ret = ::SQLBindCol( m_hstmt, (SQLUSMALLINT) col_idx, 
				OursToODBCType( column->m_Type ),
				bound_data->m_Buf,
				bound_data->m_LI,
				(SQLLEN*) &bound_data->m_LI);
	}
	UpdateExecState( m_SQL.c_str(), ret);

	if (IsExecSuccess(ret)) {
		column->m_Bound = true;
	}
}

//-----------------------------------------------------------------------------
// Unbind all columns, except column 0 (bookmark column) which always been bound
// all the time.
void ODBC_Recordset::_ODBCUnbindColumns()
{
	/*
	   CLog::Detail( ODBC, GetTask(), "_ODBCUnbindColumns for handle 0x%X", m_hstmt);
	   */

	for (int i=1; i<=GetColumnCount(); i++) {

		if (!m_Column[i].m_Bound)
			continue;

		RETCODE 
			ret = ::SQLBindCol( m_hstmt, 
					(SQLUSMALLINT)i, 
					OursToODBCType( m_Column[i].m_Type ), 
					0, m_Column [i] .Size(), 
					(SQLLEN*) &m_BoundData[i].m_LI);

		UpdateExecState( m_SQL.c_str(), ret);
		if (IsExecSuccess(ret)) {
			m_Column[i].m_Bound = false;
		}
	}

}

//-----------------------------------------------------------------------------
// Add a new recordset. This is a two step process, first _ODBCAddNew and then
// _ODBCMoveTo(last record).
// Params:
//  - none
//
void ODBC_Recordset::_ODBCAddNew()
{   
	/*
	   CLog::Detail( ODBC, GetTask(), "_ODBCAddNew for handle 0x%X", m_hstmt);
	   */

	RETCODE ret = ::SQLSetPos( m_hstmt, 1, SQL_ADD, SQL_LOCK_NO_CHANGE);
	UpdateExecState( m_SQL.c_str(), ret);
}



//-----------------------------------------------------------------------------
// Scroll to destination row
// Params:
//  - m_RowOrientation
//
void ODBC_Recordset::_ODBCMoveTo()
{
	/*
	   CLog::Detail( ODBC, GetTask(), "_ODBCMoveTo: handle=0x%X, orientation=%d", m_hstmt, m_RowOrientation);
	   */

	RETCODE
		ret = ::SQLFetchScroll( m_hstmt, (SQLSMALLINT)m_RowOrientation, 0);

	UpdateExecState( m_SQL.c_str(), ret);

	if (IsExecSuccess(ret)) {

		m_BOF = m_EOF = false;

	} else if (ret == SQL_NO_DATA) {

		if (m_RowOrientation == SQL_FETCH_PRIOR) {
			m_BOF = true; m_EOF = false;
		} else if (m_RowOrientation == SQL_FETCH_NEXT) {
			m_EOF = true; m_BOF = false;
		} else if ( m_RowOrientation==SQL_FETCH_FIRST || 
				m_RowOrientation==SQL_FETCH_LAST) {
			m_EOF = m_BOF = true;
		} else
			;	// TODO: ?
	}
	m_bAddRow = false;

}


//-----------------------------------------------------------------------------
// Update current record
void ODBC_Recordset::_ODBCUpdate()
{
	/*
	   CLog::Detail( ODBC, GetTask(), "_ODBCUpdate for handle 0x%X", m_hstmt);
	   */

	RETCODE 
		ret = ::SQLSetPos( m_hstmt, 0, SQL_UPDATE, SQL_LOCK_NO_CHANGE);
	UpdateExecState( m_SQL.c_str(), ret);
}

//-----------------------------------------------------------------------------
// Delete current record. Row position is not moved.
void ODBC_Recordset::_ODBCDelete()
{
	/*
	   CLog::Detail( ODBC, GetTask(), "_ODBCDelete for handle 0x%X", m_hstmt);
	   */

	RETCODE
		ret = ::SQLSetPos( m_hstmt, 1, SQL_DELETE, SQL_LOCK_NO_CHANGE);

	UpdateExecState( m_SQL.c_str(), ret);
}

//-----------------------------------------------------------------------------
// Requery all the rowset (refetch from database)
void ODBC_Recordset::_ODBCRequery()
{
	/*
	   CLog::Detail( ODBC, GetTask(), "_ODBCRequery for handle 0x%X", m_hstmt);
	   */

	RETCODE
		ret = ::SQLFetchScroll( m_hstmt, SQL_FETCH_RELATIVE, 0);

	UpdateExecState( m_SQL.c_str(), ret);
}



//-----------------------------------------------------------------------------
// Allocate buffer for column binding
void ODBC_Recordset::PrepareBinding()
{
	for (int i=0; i <= GetColumnCount(); i++) {
		//char *buf = new char[ m_Column[i].Size() + 32 ];
		void *buf = malloc( m_Column[i].Size() + 32 );
		m_BoundData.push_back(ODBC_Bound_Data(buf));
	}
}

//-----------------------------------------------------------------------------
// Bind all columns
void ODBC_Recordset::BindColumns()
{
	for (int i=1; i<= GetColumnCount(); i++) {
		_ODBCBindColumn(i);
		if ( GetState() == XSTATE_ERROR)
			break;
	}
}

//-----------------------------------------------------------------------------
// State handler: ST_CLOSED
void ODBC_Recordset::_SH_Closed()
{
	m_ExecState=XSTATE_ERROR;
	SetError( DB_FATAL, "_SH_Closed", "Shouldn't be called in the first place", NULL);
}

//-----------------------------------------------------------------------------
// State handler: ST_OPENING
void ODBC_Recordset::_SH_Opening()
{
	SetState_ST_NCOL();
	_ODBCNCol();
	if (m_ExecState == XSTATE_READY) {
		_SH_NCol();
	}
}


//-----------------------------------------------------------------------------
// State handler: ST_CLOSING
void ODBC_Recordset::_SH_Closing()
{
	m_ExecState = XSTATE_CLOSED;
}

//-----------------------------------------------------------------------------
// State handler: ST_NCOL
void ODBC_Recordset::_SH_NCol()
{
	if (GetColumnCount() == 0) {
		SetState_ST_MORE_RESULT();

	} else if (GetColumnCount() > 0) {
		SetState_ST_COL_ATTR();
		m_ColAttrIdx=0;	// start with bookmark column
		_ODBCColAttr();
		if (m_ExecState == XSTATE_READY)
			_SH_ColAttr();
	}
}


//-----------------------------------------------------------------------------
// State handler: ST_COL_ATTR
void ODBC_Recordset::_SH_ColAttr()
{
	RETCODE
		ret = ::SQLSetStmtAttr( m_hstmt, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)1, 
				sizeof(SQLUINTEGER));

	PrepareBinding();

	if (GetColumnCount()==0) {
		SetState_ST_READY();
		m_EOF = m_BOF = true;

	} else if (MoveTo(SQL_FETCH_NEXT)==0) {
		m_State = ST_FIRST_POSITION;
		m_ExecState = XSTATE_EXECUTING;
		CheckExecuting = &ODBC_Recordset::_ODBCMoveTo;
		MoveState = &ODBC_Recordset::_SH_Moving;
	}
}


//-----------------------------------------------------------------------------
// State handler: ST_MORE_RESULT
void ODBC_Recordset::_SH_MoreResult()
{
	/* SQLMoreResults must be cancelled before any other statement can
	   be active.
	   */
	/*
Error: function sequence error.
if (::SQLCancel ( m_hstmt ) != SQL_SUCCESS) {
SetError( DB_ERROR, 0, "Cancelling SQLMoreResult");
}
*/

SetState_ST_NCOL();
_ODBCNCol();
if (m_ExecState == XSTATE_READY) {
	_SH_NCol();
}
}


//-----------------------------------------------------------------------------
// State handler: ST_READY
void ODBC_Recordset::_SH_Ready()
{
	// This will be called in the CancelExecuting, so we comment out this
	//SetError( DB_FATAL, ODBC, "Recordset is ready but parent still polls");
}

//-----------------------------------------------------------------------------
// State handler: ST_REQUERY
void ODBC_Recordset::_SH_Requery()
{
	SetState_ST_READY();
}


//-----------------------------------------------------------------------------
// State handler: ST_DELETING
void ODBC_Recordset::_SH_Deleting()
{
	SetState_ST_READY();
}


//-----------------------------------------------------------------------------
// State handler: ST_UPDATING
void ODBC_Recordset::_SH_Updating()
{
	if (m_bAddRow) {
		MoveTo(SQL_FETCH_LAST);

		m_State = ST_MOVING;
		m_ExecState = XSTATE_EXECUTING;
		CheckExecuting = &ODBC_Recordset::_ODBCMoveTo;
		MoveState = &ODBC_Recordset::_SH_Moving;

	} else {
		SetState_ST_READY();
	}
}


//-----------------------------------------------------------------------------
// State handler: ST_MOVING
void ODBC_Recordset::_SH_Moving()
{
	SetState_ST_READY();
}



// Destroy the statement handle.
void ODBC_Recordset::DestroyStmt()
{
	if (m_hstmt!=SQL_NULL_HSTMT) {
		MySQLFreeStmt(m_hstmt, SQL_DROP);
		m_hstmt = SQL_NULL_HSTMT;
	}

	if (m_State==ST_CLOSING) {
		_ODBCClose();
		_SH_Closing();
	}
}

//-----------------------------------------------------------------------------
// Perform ODBC ::SQLCancel
bool ODBC_Recordset::SQLCancel()
{
	assert(m_ExecState == XSTATE_EXECUTING);
	if (m_ExecState == XSTATE_EXECUTING) {
		SQLRETURN ret=::SQLCancel(m_hstmt);
		if (ret != SQL_SUCCESS) {
			SetError( DB_ERROR, m_SQL.c_str(), "Error in cancelling statement", NULL);
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Move from current state to ST_READY. 
ODBC_Recordset::ExecState ODBC_Recordset::PollEvents()
{
	// if seems to be still executing, check again
	if (m_ExecState == XSTATE_EXECUTING) {
		(this->*CheckExecuting)();

		if (m_ExecState==XSTATE_ERROR) {
			if ( (!GetParentDb()->HasFatal()) && MySQLFreeStmt( m_hstmt, SQL_DROP) != SQL_SUCCESS) {
				SetError( DB_ERROR, 0, "Error in freeing handle", NULL);
			}
			--n_opened_handle;
			m_hstmt = SQL_NULL_HSTMT;
			m_LastExecError = true;

			// if we're in the middle of opening, return XSTATE_CLOSED
			switch(m_State) {
				case ST_OPENING:
				case ST_NCOL:
				case ST_COL_ATTR:
				case ST_FIRST_POSITION:
				case ST_MORE_RESULT:
					m_ExecState=XSTATE_CLOSED;
			}

			// if we're closing, set to XSTATE_CLOSED
			if (m_Quitting)
				m_ExecState=XSTATE_CLOSED;

			if (m_HasConnectionLock)
				UnlockConnection();
		}

		if (m_ExecState != XSTATE_READY)
			return m_ExecState;

	} else if (m_ExecState == XSTATE_ERROR)
		return XSTATE_ERROR;

	// if previous execution is finished, execute the state handler to move
	// to next state
	(this->*MoveState)();    

	// If statement is quitting, close it.
	if (m_ExecState==XSTATE_READY && m_Quitting) {
		m_LastExecError = false;
		_ODBCClose();
		m_ExecState=XSTATE_CLOSED;

	} else if (m_ExecState==XSTATE_ERROR) {
		// if we're in the middle of opening, return XSTATE_CLOSED
		switch(m_State) {
			case ST_OPENING:
			case ST_NCOL:
			case ST_COL_ATTR:
			case ST_FIRST_POSITION:
				m_ExecState=XSTATE_CLOSED;
				break;
		}

		if (m_hstmt != SQL_NULL_HSTMT) {
			if (MySQLFreeStmt( m_hstmt, SQL_DROP) != SQL_SUCCESS) {
				SetError( DB_ERROR, 0, "Error in freeing handle", NULL);
			}
			--n_opened_handle;
			m_hstmt = SQL_NULL_HSTMT;
		}
		m_LastExecError = true;

		if (m_HasConnectionLock)
			UnlockConnection ();
	}

	return m_ExecState;
}


//-----------------------------------------------------------------------------
// Prepare to open recordset, and open it asynchronously
int ODBC_Recordset::Open(const char *sql, int options)
{
	if ( (m_hstmt=GetParentDb()->AllocStmt()) == SQL_NULL_HSTMT) {
		m_ExecState = XSTATE_ERROR;
		return -1;
	}

	CLog::Detail( ODBC, GetTask(), "[%s] Handle 0x%X is allocated", 
			GetParentDb()->GetProfile()->Name().c_str(),
			m_hstmt);

#if 0// CBM -- For Testing

	int ret;
	SQLCHAR   SQLStmt[1024];
	SQLINTEGER rtnPersonExtn = 0;

//	SQLHDBC m_hdbc = GetParentDb()->GetHDbc();
//	printf(" ***** After Alloc ... in recordset_open m_hdbc is  %d  , m_hstmt is %d *************\n",m_hdbc,m_hstmt);

#if 1
	//      if(ret  == SQL_SUCCESS){
	strcpy((char *) SQLStmt, "exec UMN_prakash_retereive '7878'");
	ret = SQLExecDirect(m_hstmt, SQLStmt, SQL_NTS);

	if(ret  == SQL_SUCCESS){

		//cout << "SQLExecDirect is successfull" << endl;
		SQLBindCol(m_hstmt, 1, SQL_C_LONG, &rtnPersonExtn, sizeof(rtnPersonExtn), NULL );
		ret  = SQLFetch(m_hstmt);
		if(ret  == SQL_NO_DATA_FOUND)
			cout << "No sql data found "<< endl;
		//cout << "The result of the stored procedure is "<< endl;
		//cout << rtnPersonExtn << endl;
	}
	//     }
#endif

#endif


	SetState_ST_OPENING();
	m_SQL = sql;
	m_TimeBeginExecuting = 0;

	return 0;
}

//-----------------------------------------------------------------------------
// Close the recordset, abort all execution and pending updates
int ODBC_Recordset::Close()
{
	bool is_executing = (GetState() == XSTATE_EXECUTING);
	m_Quitting = true;
	m_Task = NULL;

	/* Free the handle only if statement has finished executing.
	*/
	if (!is_executing) {
		if (m_hstmt==SQL_NULL_HSTMT) {
			/* This can happen in case of database connection failure, which 
			   all statement handles will be closed by DB parent.
			   */
			GetParentDb()->Remove(this);
			delete this;
		} else {
			SetState_ST_CLOSING();
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Investigate whether the recordset is updatable by investigating updatable
// attr. of all the columns
bool ODBC_Recordset::IsUpdatable()
{
	if (GetColumnCount()==0) {
		CLog::Warning ( ODBC, GetTask(), "[%s] IsUpdatable: SQL stmt '%s' has no column",
				GetParentDb()->GetProfile()->Name().c_str(),
				m_SQL.c_str());
		return false;
	}


	for (int i=1; i<=GetColumnCount(); ++i) {
		if (!m_Column[i].m_IsUpdatable)
			return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Add a new record
int ODBC_Recordset::AddNew()
{
	if (GetColumnCount()==0) {
		CLog::Warning ( ODBC, GetTask(), "[%s] AddNew: SQL stmt '%s' has no column",
				GetParentDb()->GetProfile()->Name().c_str(),
				m_SQL.c_str());
		return -1;
	}

	// Unbind all columns
	_ODBCUnbindColumns();
	if (m_ExecState == XSTATE_ERROR)
		return -1;


	m_bAddRow = true;
	return 0;
}

//-----------------------------------------------------------------------------
// Lock a record, and start edit mode.
int ODBC_Recordset::Edit()
{
	if (GetColumnCount()==0) {
		CLog::Warning ( ODBC, GetTask(), "[%s] Edit: SQL stmt '%s' has no column",
				GetParentDb()->GetProfile()->Name().c_str(),
				m_SQL.c_str());
		return -1;
	}

	// Unbind all columns
	_ODBCUnbindColumns();
	if (m_ExecState == XSTATE_ERROR)
		return -1;

	return 0;
}


//-----------------------------------------------------------------------------
// Refresh the content of the record.
int ODBC_Recordset::Requery()
{
	if (GetColumnCount()==0) {
		CLog::Warning ( ODBC, GetTask(), "[%s] Requery: SQL stmt '%s' has no column",
				GetParentDb()->GetProfile()->Name().c_str(),
				m_SQL.c_str());
		return -1;
	}

	// bind all columns first
	BindColumns();
	if (m_ExecState == XSTATE_ERROR)
		return -1;

	// requery!
	SetState_ST_REQUERY();
	return 0;
}


//-----------------------------------------------------------------------------
// Update the changes.
int ODBC_Recordset::Update()
{
	if (GetColumnCount()==0) {
		CLog::Warning ( ODBC, GetTask(), "[%s] Update: SQL stmt '%s' has no column",
				GetParentDb()->GetProfile()->Name().c_str(),
				m_SQL.c_str());
		return -1;
	}

	SetState_ST_UPDATING();
	return 0;
}


//-----------------------------------------------------------------------------
// Delete current record.
int ODBC_Recordset::Delete()
{
	if (GetColumnCount()==0) {
		CLog::Warning ( ODBC, GetTask(), "[%s] Delete: SQL stmt '%s' has no column",
				GetParentDb()->GetProfile()->Name().c_str(),
				m_SQL.c_str());
		return -1;
	}

	SetState_ST_DELETING();
	return 0;
}


//-----------------------------------------------------------------------------
// Scroll to a row
// Note: MoveTo has special 'feature' here, is that it doesn't call SetState.
int ODBC_Recordset::MoveTo(int orientation)
{
	// Bind all bound columns first
	BindColumns();
	if (m_ExecState == XSTATE_ERROR) {
		return -1;
	}

	// move!
	/*
	   m_State = ST_MOVING;
	   m_ExecState = XSTATE_EXECUTING;
	   CheckExecuting = &ODBC_Recordset::_ODBCMoveTo;
	   MoveState = &ODBC_Recordset::_SH_Moving;
	   m_RowOrientation = orientation;
	   */
	m_RowOrientation = orientation;

	return 0;
}


//-----------------------------------------------------------------------------
int ODBC_Recordset::MoveFirst()
{
	if (GetColumnCount()==0) {
		CLog::Warning ( ODBC, GetTask(), "[%s] MoveFirst: SQL stmt '%s' has no column",
				GetParentDb()->GetProfile()->Name().c_str(),
				m_SQL.c_str());
		return -1;
	}

	if (MoveTo(SQL_FETCH_FIRST)==0) {
		//OnAsyncBegin();
		SetState_ST_MOVING();
		return 0;
	}
	return -1;
}


//-----------------------------------------------------------------------------
int ODBC_Recordset::MovePrev()
{
	if (GetColumnCount()==0) {
		CLog::Warning ( ODBC, GetTask(), "[%s] MovePrev: SQL stmt '%s' has no column",
				GetParentDb()->GetProfile()->Name().c_str(),
				m_SQL.c_str());
		return -1;
	}

	if (MoveTo(SQL_FETCH_PRIOR)==0) {
		//OnAsyncBegin();
		SetState_ST_MOVING();
		return 0;
	}
	return -1;
}


//-----------------------------------------------------------------------------
int ODBC_Recordset::MoveNext()
{
	if (GetColumnCount()==0) {
		CLog::Warning ( ODBC, GetTask(), "[%s] MoveNext: SQL stmt '%s' has no column",
				GetParentDb()->GetProfile()->Name().c_str(),
				m_SQL.c_str());
		return -1;
	}

	if (MoveTo(SQL_FETCH_NEXT)==0) {
		//OnAsyncBegin();
		SetState_ST_MOVING();
		return 0;
	}
	return -1;
}


//-----------------------------------------------------------------------------
int ODBC_Recordset::MoveLast()
{
	if (GetColumnCount()==0) {
		CLog::Warning ( ODBC, GetTask(), "[%s] MoveLast: SQL stmt '%s' has no column",
				GetParentDb()->GetProfile()->Name().c_str(),
				m_SQL.c_str());
		return -1;
	}

	if (MoveTo(SQL_FETCH_LAST)==0) {
		//OnAsyncBegin();
		SetState_ST_MOVING();
		return 0;
	}
	return -1;
}


//-----------------------------------------------------------------------------
// Convert column name into column index. If requested name has format '@number',
// then the number is the column number.
// Note that column index is started from 1 (column 0 is bookmark column)
int ODBC_Recordset::GetColumn(const char *name)
{
	if ( *name=='@' ) {
		char *err;
		int col=strtol( name+1, &err, 10);
		if (*err)
			return -1;

		if (col<1 || col>GetColumnCount())
			return -1;
		return col;

	} else {
		for (int i=1; i<=GetColumnCount(); ++i) {
			if ( !strcasecmp(m_Column[i].GetName(), name))
				return i;
		}
	}

	return -1;
}

//-----------------------------------------------------------------------------
// Convert ODBC type to our type
DB_Column_Desc::Type  ODBC_Recordset::ODBCToOurType(int odbc_type)
{
	switch (odbc_type) {

		// integer value
		case SQL_TINYINT:
		case SQL_SMALLINT:
		case SQL_INTEGER:
			return DB_Column_Desc::DB_INT;

			// float value
		case SQL_DOUBLE:
		case SQL_REAL:
		case SQL_FLOAT:
			return DB_Column_Desc::DB_FLOAT;

			// date time value
		case SQL_TIMESTAMP:
		case SQL_TYPE_TIMESTAMP:
			return DB_Column_Desc::DB_DATETIME;

			// string value
		case SQL_CHAR:
		case SQL_VARCHAR:
		case SQL_LONGVARCHAR:
			return DB_Column_Desc::DB_STRING;

		case SQL_BINARY:
		case SQL_VARBINARY:
			return DB_Column_Desc::DB_BINARY;

		case SQL_WCHAR:
		case SQL_WVARCHAR:
		case SQL_WLONGVARCHAR:
			return DB_Column_Desc::DB_WSTRING;

			// unsupported data type
			/*
			   case SQL_DECIMAL:
			   case SQL_NUMERIC:
			   case SQL_BIT:
			   case SQL_DATE:
			   case SQL_TIME:
			   case SQL_LONGVARBINARY:
			   case SQL_BIGINT:    
			   */
		default:
			SetError( DB_ERROR, m_SQL.c_str(), "Type is not supported", NULL);
			return DB_Column_Desc::DB_VOID;
	}

	/*
	// should never reach here
	return DB_Column_Desc::DB_VOID;

	//on_error:
	return DB_Column_Desc::DB_VOID;
	*/
}


//-----------------------------------------------------------------------------
// Convert our type to ODBC type
SWORD ODBC_Recordset::OursToODBCType( DB_Column_Desc::Type type)
{
	switch(type) {
		// void
		case DB_Column_Desc::DB_VOID:
			SetError( DB_ERROR, m_SQL.c_str(), "column has type DB_VOID", NULL);
			return -1;

			// integer
		case DB_Column_Desc::DB_INT:
			return SQL_C_SLONG;

			// float
		case DB_Column_Desc::DB_FLOAT:
			return SQL_C_FLOAT;

			// date-time
		case DB_Column_Desc::DB_DATETIME:
			//return SQL_C_TYPE_TIMESTAMP;	// ODBC ver 3.x
			return SQL_C_TIMESTAMP;	// ODBC ver 2.x

			// string
		case DB_Column_Desc::DB_STRING:
			return SQL_C_CHAR;

			// binary data (used by bookmark column)
		case DB_Column_Desc::DB_BINARY:
			return SQL_BINARY;

		case DB_Column_Desc::DB_WSTRING:
			return SQL_C_WCHAR;
	}

	// should never reach here
	SetError( DB_FATAL, m_SQL.c_str(), "DB_Column_Desc has unsupported type", NULL);
	return -1;
}

//-----------------------------------------------------------------------------
int ODBC_Recordset::SetValInt(int col, int val)
{
	// set the bound buffer
	//*(int*)m_BoundData[col].m_Buf = val;
	memcpy(m_BoundData[col].m_Buf, &val, sizeof(int));

	// bind/rebind the column
	_ODBCBindValue(col);
	if (GetState() == XSTATE_ERROR)
		return -1;

	return 0;
}


//-----------------------------------------------------------------------------
int ODBC_Recordset::SetValFloat(int col, float val)
{
	// set the bound buffer
	//*(float*)m_BoundData[col].m_Buf = val;
	memcpy(m_BoundData[col].m_Buf, &val, sizeof(float));

	// bind/rebind the column
	_ODBCBindValue(col);
	if (GetState() == XSTATE_ERROR)
		return -1;

	return 0;
}


//-----------------------------------------------------------------------------
int ODBC_Recordset::SetValString(int col, const char *val)
{
	// set val string
	safe_strcpy( (char*)m_BoundData[col].m_Buf, val, m_Column[col].Size()+1);

	// bind/rebind the column
	_ODBCBindValue(col);
	if (GetState() == XSTATE_ERROR)
		return -1;

	return 0;
}


//-----------------------------------------------------------------------------
int ODBC_Recordset::SetValDate(int col, time_t val)
{
	// convert to TIMESTAMP
	TIMESTAMP_STRUCT d;
	if (Time_tToTimestamp( val, &d)) {
		SetError( DB_ERROR, m_SQL.c_str(), "Date conversion error", NULL);
		return -1;
	}

	// copy to bound column buffer
	memcpy( m_BoundData[col].m_Buf, &d, sizeof(TIMESTAMP_STRUCT));

	// bind/rebind the column
	_ODBCBindValue(col);
	if (GetState() == XSTATE_ERROR)
		return -1;

	return 0;
}


//-----------------------------------------------------------------------------
// Get date value of col parameter in current row
int ODBC_Recordset::IsNull(int col, int *val)
{
	if (m_BoundData[col].m_LI == SQL_NULL_DATA)
		*val = APPTRUE;
	else
		*val = APPFALSE;
	return 0;
}

//-----------------------------------------------------------------------------
// Get integer value of col parameter in current row
int ODBC_Recordset::GetValInt(int col, int *val)
{
	if (IsEOF() || IsBOF()) {
		*val = 0;
		CLog::Debug (ODBC, "DbGetValueInt: recordset is EOF/BOF. SQL=%s", m_SQL.c_str());
		return 0;
	}

	memcpy( val, m_BoundData[col].m_Buf, sizeof(int));
	return 0;
}


//-----------------------------------------------------------------------------
// Get float value of col parameter in current row
int ODBC_Recordset::GetValFloat(int col, float *val)
{
	if (IsEOF() || IsBOF()) {
		*val = 0;
		CLog::Debug (ODBC, "DbGetValueReal: recordset is EOF/BOF. SQL=%s", m_SQL.c_str());
		return 0;
	}

	memcpy( val, m_BoundData[col].m_Buf, sizeof(float));
	return 0;
}


//-----------------------------------------------------------------------------
// Get string value of col parameter in current row
int ODBC_Recordset::GetValString(int col, char *val, int len)
{
	if (IsEOF() || IsBOF()) {
		*val = '\0';
		CLog::Debug (ODBC, "DbGetValueStr: recordset is EOF/BOF. SQL=%s", m_SQL.c_str());
		return 0;
	}

	*val = '\0';

	if (m_Column[col].m_Type == DB_Column_Desc::DB_STRING) 
	{
		safe_strcpy( val, (const char*)m_BoundData[col].m_Buf, 
				len < m_Column[col].Size()+1 ? len : m_Column[col].Size()+1);

		sl::strltrim(val);
		sl::strrtrim(val);

	} else if (m_Column[col].m_Type == DB_Column_Desc::DB_WSTRING) {

		const wchar_t *wstr = (const wchar_t*) m_BoundData[col].m_Buf;

		char *base64_str = base64_encode((const unsigned char*)wstr, (wcslen(wstr)*2)+1);
		int base64_len = strlen(base64_str);

		if (base64_len >= len) {
			CLog::Warning(ODBC, "DbGetValueStr: data in column %s truncated (length=%d, SQL='%s')",
					m_Column[col].m_Name, base64_len, m_SQL.c_str());
		}

		safe_strcpy(val, base64_str, len);
		free(base64_str);

	} else {
		CLog::Warning(ODBC, "DbGetValueStr: column %s is not string. SQL=%s", 
				m_Column[col].m_Name, m_SQL.c_str());
	}
	return 0;
}


//-----------------------------------------------------------------------------
// Get date value of col parameter in current row
int ODBC_Recordset::GetValDate(int col, time_t *val)
{
	if (IsEOF() || IsBOF()) {
		*val = 0;
		CLog::Debug (ODBC, "DbGetValueDate: recordset is EOF/BOF. SQL=%s", m_SQL.c_str());
		return 0;
	}

	TIMESTAMP_STRUCT d;

	memcpy(&d, (TIMESTAMP_STRUCT*)m_BoundData[col].m_Buf, sizeof(d));
	*val = TimestampToTime_t(d);

	return 0;
}


