/* $Header: /xGate/odbc.h 17    2/02/05 9:30a Bennylp $
 *   
 * XGATE PROJECT
 * (C) 2001 Switchlab, Ltd
 *
 * XGATE ODBC DATABASE.
 *
 * $Log: /xGate/odbc.h $
 * 
 * 17    2/02/05 9:30a Bennylp
 * Added flag m_IsDeleted to assist debugging
 * 
 * 16    7/29/04 7:29p Bennylp
 * Zombie the connection upon really fatal error
 * 
 * 15    7/26/04 10:15a Bennylp
 * Disconnect ODBC connection upon TDS protocol errors
 * 
 * 14    3/19/04 12:49p Bennylp
 * Preliminary support for Unicode columns
 * 
 * 13    1/07/04 7:13p Sonny
 * Display the statement causing connection busy
 * 
 * 12    2/19/03 10:38a Bennylp
 * Changed std::string to C string in DB_Col (suspect bug)
 * 
 * 11    5/09/02 7:54p Bennylp
 * Several enhancements to database:
 *   - encrypt password in connect string (RC4 + base64)
 *   - added information about currently running SQL in a connection
 *   - added lock information in connection
 *   - added command to cancel currently running SQL
 *   - added command to force reconnection of a DB connection
 * 
 * 10    5/08/02 6:34p Bennylp
 * Quick fix: show currently running query in one connection
 * 
 * 9     11/19/01 1:26p Bennylp
 * Fixed bug when xGate crashes if SQL returns no data.
 * 
 * 8     7/11/01 19:36 Bennylp
 * Fixed bug when there are too many columns
 * 
 * 7     6/08/01 2:56p Bennylp
 * Fixed bugs in dbdriver (reload).
 * 
 * 6     6/06/01 7:09p Bennylp
 * Supports dynamic config and auto-connect when connection failed.
 * 
 * 5     5/25/01 9:42p Bennylp
 * Fixed database connection busy.
 * 
 * 4     4/30/01 7:45a Bennylp
 * Integrate database service.
 * 
 * 3     4/28/01 6:33a Bennylp
 * Added service profiling and database driver (testing)
 */
#ifndef __ODBCDRV_H__
#define __ODBCDRV_H__

#include<sqltypes.h>
#include<sql.h>
#include "log.h"
#include "dbdriver.h"



#define	DB_MAX_COL	40
#define DB_MAX_COL_NAME	64


#define DB_DEBUG	(LOG_DEBUG)
#define DB_INFO		(LOG_INFO)
#define	DB_ERROR	(LOG_ERRORS)
#define DB_WARNING	(LOG_WARNING)
#define DB_FATAL	(LOG_FATAL)


//=============================================================================
// Forward decl.
class ODBC_Database;
class ODBC_Recordset;
class CTask;


//=============================================================================
// Database column attribute
//
struct DB_Column_Desc {
    enum Type {
	DB_VOID,
	DB_BINARY,
	DB_INT,
	DB_FLOAT,
	DB_DATETIME,
	DB_STRING,
	DB_WSTRING,
    };

    char    m_Name[DB_MAX_COL_NAME+2];
    Type    m_Type;
    int	    m_Length,	    // size/length of actual column in database
	    m_DataLength;   // size/length of the corresponding C type
    int	    m_IsUpdatable;
    bool    m_Bound;

    // ctor
    DB_Column_Desc() 
    : m_Type(DB_VOID), m_Length(0), m_DataLength(0), m_IsUpdatable(0), m_Bound(false) 
    {
	m_Name[0] = '\0';
    }

    // get column name
    const char* GetName() { return m_Name; };

    // get column's data size
    int Size() { return m_Length; };
    int DataSize() { return m_DataLength; };

    // update the m_DataLength according to m_Type
    void UpdateDataLength() {
	switch(m_Type) {
	case DB_VOID:
	    m_DataLength=0;
	    break;
	case DB_INT:
	case DB_FLOAT:
	case DB_DATETIME:
	    m_DataLength=sizeof(int);
	    break;
	case DB_BINARY:
	    m_DataLength = m_Length;
	    break;
	case DB_STRING:
	    m_DataLength = MAXAPPSTRLEN;
	    break;
	case DB_WSTRING:
	    m_DataLength = m_Length;
	}
    }

    // set a buffer invalid according to column specification
    static void Invalidate(const DB_Column_Desc &col, void *buf) {
	switch(col.m_Type) {
	case DB_VOID:
	    break;
	case DB_INT:
	    *(int*)buf = 0;
	    break;
	case DB_FLOAT:
	    *(float*)buf = 0;
	    break;
	case DB_DATETIME:
	    *(time_t*)buf = 0;
	    break;
	case DB_STRING:
	    *(char*)buf = '\0';
	    break;
	case DB_WSTRING:
	    *(wchar_t*)buf = 0;
	}
    }
};


//=============================================================================
//
// Base class for ODBC objects. Provides several ODBC utility functions
//
class ODBC_Base {
protected:
    // Ctor
    ODBC_Base() : m_LastExecError(false), m_IsDeleted(false) { sqlState[0] = '\0'; }
    virtual ~ODBC_Base() { m_IsDeleted=true; }


    // Trigger error reporting
    void SetError( ODBC_Database *db, LogVerbosity severity, CTask *task, const char *title, const char *msg);
    void SetError( ODBC_Database *db, LogVerbosity severity, CTask *task, const char *title, HENV);
    void SetError( ODBC_Database *db, LogVerbosity severity, CTask *task, const char *title, HENV, HDBC);
    void SetError( ODBC_Database *db, LogVerbosity severity, CTask *task, const char *title, HENV, HDBC, HSTMT,
		   String *saved_err);

    // Is the function execution successful?
    bool IsExecSuccess(RETCODE ret) { return ret==SQL_SUCCESS || ret==SQL_SUCCESS_WITH_INFO; }
    bool IsExecFailed(RETCODE ret) { return !IsExecSuccess(ret) && ret!=SQL_STILL_EXECUTING; }

protected:
    // indicator whether the last execution is failed
    bool m_LastExecError;
    bool m_IsDeleted;
    char sqlState[6];

    const char *GetLastSQLState() const {
	return sqlState;
    }

    int GetODBCError(HENV env, HDBC dbc, HSTMT stmt, String & err);
};

//=============================================================================
// ODBC database object
//
class ODBC_Database : public DB_Database, public ODBC_Base {
public:
    ODBC_Database( DB_Service *drv, 
		   DB_Connection_Profile *profile,
		   DB_Connection_Slot *slot);

    // Opening/closing database
    virtual int Open(const char *connectstring);
    virtual int Close();
    virtual int CancelQuery();

    // Database main entry
    DB_Recordset *OpenQuery(CTask *, const char *sql, int opt=DB_EXEC_ASYNC);
    DB_SQL_Exec *CreateExecutor(CTask *);

    // Execution polling
    int	    PollEvents();

    // Get handles
    HENV    GetHEnv() { return m_henv; }
    HDBC    GetHDbc() { return m_hdbc; }

    // called by DB_Async_Object when the async. execution begins/stops
    int	    OnAsyncBegin( DB_Async_Object *);
    int	    OnAsyncEnd( DB_Async_Object *);

    // called by child object
    void    Remove( DB_Async_Object *obj) { m_AllObject.remove(obj); }

    // allocate statement handle (or get from cache)
    HSTMT   AllocStmt();

    // deallocate statement handle (or put into cache)
    STATUS  ReleaseStmt( HSTMT &stmt);

    // set the options
    void    SetOption(const DB_Connection_Option &opt) { m_Option=opt; }

    // Lock database connection.
    // When the database is locked, only the statement which locked the
    // database can be processed.
    void    Lock (ODBC_Recordset *rst);

    // Unlock database connection.
    void    Unlock(ODBC_Recordset *rst);

    // Get the task that exclusively using this connection.
    // This will return non-NULL if the database is being Locked.
    virtual CTask *GetExclusiveOwner();

    // Get the object that currently runs exclusively in the connection
    virtual DB_Async_Object * GetExclusiveObject();

    // Get total number of handles opened.
    virtual size_t GetHandlesCount() const {
	return m_AllObject.size();
    }

    // Get number of jobs
    virtual size_t GetJobCount() const {
	return m_RunningObject.size();
    }

    virtual const CTask * GetExecutingTask() const {
	const DB_Async_Object *current = GetExecutingObject();
	if (current)
	    return current->GetTask();
	return NULL;
    }

    virtual const DB_Async_Object * GetExecutingObject() const {
	if (m_RunningObject.size())
	    return m_RunningObject.front();
	return NULL;
    }

    // Signal fatal database failure.
    void SignalDbFatal(const String & errmsg, bool really_really_fatal) {
	m_HasFatal = true;
	m_FatalErrMsg = errmsg;
	m_ReallyReallyFatal = really_really_fatal;
    }
    bool HasFatal() const {
	return m_HasFatal;
    }
    virtual void AbortJob(bool dont_close);

    virtual const String & GetLastErrMsg() const {
	return m_FatalErrMsg;
    }

    // Destroy the statement handle, do nothing
    virtual void DestroyStmt() {}

    // Set the last query successfully opened for this connection.
    // The SQL will be displayed when connection busy error is 
    // deteted.
    void SetLastSqlQuery(const void *obj, const char *query);

    // Get the last query successfully executed.
    const char *GetLastSqlQuery() const;

private:
    HENV    m_henv;
    HDBC    m_hdbc;

    List <DB_Async_Object*>  m_AllObject,	// all objects
			    m_RunningObject;	// currently running objects

    // Recordset that exclusively locking the database connection.
    ODBC_Recordset   *m_ExclusiveRst;

    // Time when connection was locked.
    DWORD	      m_tExclusive;

    // options
    DB_Connection_Option    m_Option;

    // statement caching
    List <HSTMT>  m_StmtCache;

    // has fatal errors?
    bool  m_HasFatal;
    String m_FatalErrMsg;
    bool m_ReallyReallyFatal;

    // Last query executed
    const void * m_LastSqlObject;
    char m_LastSqlQuery[256];

    // clear statement cache
    void ClearStmtCache();

    // TODO: fix this
    friend class ODBC_Recordset;
    friend class ODBC_SQL_Exec;
    friend class DB_Connection_Slot;
};

//=============================================================================
//
// ODBC structure for executing non-query SQL
//
class ODBC_SQL_Exec : public DB_SQL_Exec, public ODBC_Base {
public:

    // ctor
    ODBC_SQL_Exec( CTask *, ODBC_Database *);
    ~ODBC_SQL_Exec();

    // initializing handles, etc. (doing things that can be share by several
    // executions)
    int Init(int options = DB_EXEC_ASYNC);

    // Main entrty: execute non-query SQL
    int Execute(const char *sql);

    // poll exec. completion
    ExecState PollEvents();

    // cancel execution
    int CancelExecuting();

    // cleanup. After Free(), this object can not be used anymore without 
    // calling Init() again.
    int Close();

    // get number of rows affected
    int GetRowsAffected() { return m_RowsAffected; }

    // get the ODBC parent db
    ODBC_Database *GetParentDb() { return (ODBC_Database*) DB_Async_Object::GetParentDb(); }

    virtual const char *GetSQL() const { return m_SQL.c_str(); }

    // Destroy the statement handle.
    virtual void DestroyStmt();

    virtual int CancelQuery() {
	return SQLCancel() ? 0 : -1;
    }

    bool SQLCancel();

private:
    // statement handle
    HSTMT   m_hstmt;

    // error
    String  m_ErrStr;

    // saved SQL
    String  m_SQL;

    // affected rows
    int	    m_RowsAffected;

    // cancellation flag
    // TODO: fix
    bool    m_Cancelling;

    // ODBC functions
    void _ODBCExecuting();
    void _ODBCGetRowCount();
    void _ODBCFree();

    void (ODBC_SQL_Exec::*CheckExecuting)();
};

//=============================================================================
// Structure of bound column
struct ODBC_Bound_Data {
    SQLPOINTER  m_Buf;		// the buffer
    SQLINTEGER  m_LI;		// length indicator
    bool	m_IsNull;

    ODBC_Bound_Data(void *buf) { m_Buf=buf; }
};

//=============================================================================
//
// ODBC implementation of recordset
//
class ODBC_Recordset : public DB_Recordset, public ODBC_Base {

public:
    ODBC_Recordset( CTask *owner, ODBC_Database *parent);
    ~ODBC_Recordset();
    
    int Open(const char *sql, int options);
    int Close();

    bool IsUpdatable();
    int AddNew();
    int Edit();
    int Update();
    int Delete();
    int Requery();

    bool IsBOF() { 
	return m_hstmt==SQL_NULL_HSTMT ? true : m_BOF; 
    }
    bool IsEOF() { 
	return m_hstmt==SQL_NULL_HSTMT ? true : m_EOF; 
    }

    int MoveFirst();
    int MovePrev();
    int MoveNext();
    int MoveLast();

    int GetColumn(const char *name);
    int GetColumnCount() { return m_NColumn; }

    int SetValInt(int col, int val);
    int SetValFloat(int col, float val);
    int SetValString(int col, const char *val);
    int SetValDate(int col, time_t val);

    int IsNull(int col, int*);

    int GetValInt(int col, int *val);
    int GetValFloat(int col, float *val);
    int GetValString(int col, char *val, int len);
    int GetValDate(int col, time_t *val);

    virtual int CancelExecuting();
    virtual ExecState PollEvents();

    virtual bool IsExclusive() { return m_HasConnectionLock; }
    virtual const char *GetSQL() const { return m_SQL.c_str(); }

    // Destroy the statement handle.
    virtual void DestroyStmt();

    virtual int CancelQuery() {
	return SQLCancel() ? 0 : -1;
    }
    bool SQLCancel();

private:
    // handles
    HSTMT   m_hstmt;

    // columns
    int	    m_NColumn;
    DB_Column_Desc  m_Column[ DB_MAX_COL+2 ];

    // diagnostics
    bool    m_BOF, m_EOF;

    // column binding
    Vector <ODBC_Bound_Data>    m_BoundData;
    
    void    BindColumns();
    void    PrepareBinding();	// prepare buffer for column binding

    // States
    enum ODBCState {
	ST_CLOSED,	// closed
	ST_OPENING,	// opening recordset
	ST_NCOL,	// getting number of columns
	ST_COL_ATTR,	// getting column attributes
	ST_READY,	// READY for next instruction
	ST_DELETING,	// deleting current record
	ST_REQUERY,	// refreshing the whole rowset
	ST_UPDATING,	// updating current record
	ST_MOVING,	// moving
	ST_GETDATA,	// retrieving data for current record
	ST_ISNULL,	// is null?
	ST_CLOSING,	// closing the recordset
	ST_FIRST_POSITION, // positioning cursor right after open
	ST_MORE_RESULT,	// moving to next resultset
	//ST_CANCELLING,	// cancel current command
    };
    ODBCState	m_State;

    void SetState(ODBCState st);
    ODBC_Database *GetParentDb() { return (ODBC_Database*) DB_Async_Object::GetParentDb(); }

//-----------------------------------------------------------------------------
// Asynchronous execution
//
private:
    // flag whether to add a new row when updating
    bool	m_bAddRow;

    // Flag to indicate that the statement has multiple resultsets.
    // When a statement has multiple resultsets, no other statement can be active.
    bool	m_HasMoreResult;

    // Flag to indicate that this recordset is holding parent connection
    bool	m_HasConnectionLock;

    // Temporary buffer for _ODBCGetData()
    enum { TEMP_BUF_LEN = 40 };
    char	m_TempBuf[TEMP_BUF_LEN];   // temporary buffer for _ODBCGetData

    int		m_BufLen;
    char	*m_Buf,		// buffer pointer for _ODBCGetData
				// (either points to m_TempBuf or m_DestBuf)
		*m_DestBuf;	// the 'real' user's buffer
    int		*m_IsNull;	// IsNull() user's data
    SQLINTEGER	m_BufLenCopied;	// bytes copied to user buffer

    // Saved parameters for async. execution
    String	m_SQL;		// save the SQL query for _ODBCOpen()
    int		m_ColAttrIdx;	// index of col. in _ODBCColAttr()
    int		m_RowOrientation,// row orientation in _ODBCMoveTo()
		m_ColToGo;	// destination col. in _ODBCGetData(),
				// _ODBCSetData()

    // flag indicating that we're cancelling the function
    bool	m_Cancelling;

    // Scroll row
    int MoveTo(int orientation);

    // Function for async. execution and CheckExecuting investigation
    void _ODBCIdle();	    // do nothing
    void _ODBCOpen();	    // open recordset
    void _ODBCNCol();	    // get numbe of columns
    void _ODBCColAttr();    // get column attributes
    void _ODBCAddNew();	    // add a new record
    void _ODBCMoveTo();	    // scroll to specified row
    void _ODBCGetData();    // get item data
    void _ODBCIsNull();	    // is null?
    void _ODBCRequery();    // refreshing the whole rowset
    void _ODBCUpdate();	    // update current record
    void _ODBCDelete();	    // delete current record
    void _ODBCGetCurRow();  // get current row number
    void _ODBCClose();	    // close and free the recordset
    void _ODBCMoreResult(); // move to next resultset

    typedef void (ODBC_Recordset::*ASYNCFUNCPTR)();
    ASYNCFUNCPTR    CheckExecuting; // pointer to one of above function

    // ODBC synchronous functions
    void _ODBCBindColumn(int); // bind a column with empty value
    void _ODBCBindValue(int);  // bind a column with values already set
    void _ODBCUnbindColumns(); // unbind all columns
    void _ODBCEnableAsync();
    void _ODBCDisableAsync();

    // Determine current status (ExecState) based on return value of currently
    // running async. exec.
    RETCODE UpdateExecState(const char *last_cmd, RETCODE ret);

    // Lock parent connection.
    void  LockConnection();
    void  UnlockConnection();

    // State handlers
    void _SH_Closed();
    void _SH_Opening();
    void _SH_NCol();
    void _SH_ColAttr();
    void _SH_Ready();
    void _SH_Requery();
    void _SH_Deleting();
    void _SH_Updating();
    void _SH_GetData();
    void _SH_IsNull();
    void _SH_Moving();
    void _SH_Closing();
    void _SH_MoreResult();

    void (ODBC_Recordset::*MoveState)();

    // Changing state
    void SetState_ST_CLOSED();	// closed
    void SetState_ST_OPENING();	// opening recordset
    void SetState_ST_NCOL();	// getting number of columns
    void SetState_ST_COL_ATTR();// getting column attributes
    void SetState_ST_READY();	// READY for next instruction
    void SetState_ST_DELETING();// deleting current record
    void SetState_ST_REQUERY();	// refreshing the whole rowset
    void SetState_ST_UPDATING();// updating current record
    void SetState_ST_MOVING();	// moving
    void SetState_ST_GETDATA();	// retrieving data for current record
    void SetState_ST_ISNULL();	// is null?
    void SetState_ST_CLOSING(); // closing recordset	
    void SetState_ST_MORE_RESULT(); // moving to next resultset.


// MISC:

    // Set error
    void SetError( LogVerbosity severity, const char *title, const char *msg, String *saved_err );

    // Type conversion
    DB_Column_Desc::Type  ODBCToOurType(int odbc_type);
    SWORD OursToODBCType( DB_Column_Desc::Type type);

};

extern size_t	n_opened_handle;

SQLRETURN  MySQLAllocStmt(SQLHDBC ConnectionHandle, SQLHSTMT *StatementHandle);
SQLRETURN  MySQLFreeStmt(SQLHSTMT StatementHandle, SQLUSMALLINT Option);


#endif
