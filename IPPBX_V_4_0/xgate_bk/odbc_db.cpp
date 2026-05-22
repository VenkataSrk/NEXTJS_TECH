/* $Header: /xGate/odbc_db.cpp 16    7/29/04 7:29p Bennylp $
 *   
 * XGATE PROJECT
 * (C) 2001 Switchlab, Ltd
 *
 * XGATE ODBC DATABASE.
 * ODBC database connection.
 *
 * $Log: /xGate/odbc_db.cpp $
 * 
 * 16    7/29/04 7:29p Bennylp
 * Zombie the connection upon really fatal error
 * 
 * 15    7/26/04 10:16a Bennylp
 * Disconnect ODBC connection upon TDS errors
 * 
 * 14    1/09/04 6:28p Bennylp
 * Fixed minor uninitialized variable
 * 
 * 13    1/07/04 7:13p Sonny
 * Display the statement causing connection busy
 * 
 * 12    5/09/02 7:54p Bennylp
 * Several enhancements to database:
 *   - encrypt password in connect string (RC4 + base64)
 *   - added information about currently running SQL in a connection
 *   - added lock information in connection
 *   - added command to cancel currently running SQL
 *   - added command to force reconnection of a DB connection
 * 
 * 11    11/19/01 1:26p Bennylp
 * Fixed bug when xGate crashes if SQL returns no data.
 * 
 * 10    8/09/01 5:44p Bennylp
 * Reduce the number of CLog::Detail calls.
 * 
 * 9     8/03/01 5:15p Bennylp
 * Don't show error if unable to set statement to asynchronous.
 * 
 * 8     6/08/01 2:56p Bennylp
 * Fixed bugs in dbdriver (reload).
 * 
 * 7     6/06/01 7:09p Bennylp
 * Supports dynamic config and auto-connect when connection failed.
 * 
 * 6     5/25/01 9:42p Bennylp
 * Fixed database connection busy.
 * 
 * 5     5/24/01 2:08p Bennylp
 * Added assertion when statement is submitted more than once.
 * 
 * 4     4/30/01 7:45a Bennylp
 * Integrate database service.
 * 
 * 3     4/28/01 6:33a Bennylp
 * Added service profiling and database driver (testing)
 */
//#include "stdafx.h"
#include <sqlext.h>
#include "odbc.h"
#include "types.h"
#include "log.h"
#include "dbdriver.h"
#include "task.h"
#include <iostream>

using namespace std;

#define ODBC	"freeTDS"

size_t n_opened_handle;


//This is wrapper function and need to move to common header
//linux porting
static unsigned long GetTickCount()
{
  struct timeval tv;
  if( gettimeofday(&tv, NULL) != 0 )
    return 0;

  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

///////////////////////////////////////////////////////////////////////////////
//
// ODBC Database
//
///////////////////////////////////////////////////////////////////////////////
//
  ODBC_Database::ODBC_Database(DB_Service *drv, DB_Connection_Profile *profile, DB_Connection_Slot *slot)
: DB_Database(drv, profile, slot), m_henv(0), m_hdbc(0), m_ExclusiveRst (NULL),
  m_HasFatal(false)
{
  m_LastSqlObject = NULL;
  m_LastSqlQuery[0] = m_LastSqlQuery[sizeof(m_LastSqlQuery)-1] = 0;
}

void extract_error(
    char *fn,
    SQLHANDLE handle,
    SQLSMALLINT type)
{
  SQLINTEGER i = 0;
  SQLINTEGER native;
  SQLCHAR state[ 7 ];
  SQLCHAR text[256];
  SQLSMALLINT len;
  SQLRETURN ret;
  fprintf(stderr,
      "\n"
      "The driver reported the following diagnostics whilst running "
      "%s\n\n",
      fn);

  do
  {
    ret = SQLGetDiagRec(type, handle, ++i, state, &native, text,
        sizeof(text), &len );
    if (SQL_SUCCEEDED(ret))
      printf("%s:%ld:%ld:%s\n", state, i, native, text);
  }
  while( ret == SQL_SUCCESS );
}

// Open a database connection
int ODBC_Database::Open(const char *connectstring)
{
  RETCODE ret;

  //CLog::Detail( ODBC, "multithreaded-ippbx : odbc_db.cpp : .....1");
  // allocate environment handle
  ret = ::SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);
  //  ret = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);
  if (ret != SQL_SUCCESS) {
    CLog::Error( ODBC, "[%s] Unable to alocate environment handle for database",
        GetProfile()->Name().c_str());
    return -1;
  }

  //CLog::Detail( ODBC, "multithreaded-ippbx : odbc_db.cpp : .....2");
  // set ODBC version
  ret = ::SQLSetEnvAttr( m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
  //	ret = ::SQLSetEnvAttr( m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER); //CBM
  if (ret != SQL_SUCCESS) {
    SetError( this, DB_ERROR, 0, "Setting ODBC version to 3", m_henv);
    goto on_error0;
  }

  //CLog::Detail( ODBC, "multithreaded-ippbx : odbc_db.cpp : .....3");
  // allocate database connection handle
  ret = ::SQLAllocHandle( SQL_HANDLE_DBC, m_henv, &m_hdbc);
  if (ret != SQL_SUCCESS) {
    SetError( this, DB_ERROR, 0, "Allocating database handle", m_henv);
    goto on_error0;
  }
  //	printf("***********Sizeof  is %ld *************\n", m_hdbc);
  //      printf("************** m_hdbc is %ld *************\n", m_hdbc);
  /*      cout << "size of connection handle is " << sizeof(m_hdbc) << " and mhdbc is " << m_hdbc << endl;
  */

#if 0
  // server/client side cursor
  ret = ::SQLSetConnectAttr( m_hdbc, SQL_ATTR_ODBC_CURSORS, (SQLPOINTER)m_Option.GetODBCCursorSide(), 0);
  if (ret != SQL_SUCCESS) {
    SetError( this, DB_ERROR, 0, "Setting cursor side", m_henv, m_hdbc);
    goto on_error0;
  }

#endif 

  SQLCHAR connectstr[1024+1],connectstring_out[1024+1];
  SQLSMALLINT	len;

  safe_strcpy((char*)connectstr,(char*)connectstring,sizeof(connectstr)-1); //CBM
  //   	strcpy(connectstr,"DSN=MSSQLTestServer;UID=sa;PWD=abbeysql;");

  //CLog::Detail( ODBC, "multithreaded-ippbx : odbc_db.cpp : .....4");
  ret = ::SQLDriverConnect(
      m_hdbc, 
      NULL, 
      connectstr,
      SQL_NTS,
      connectstring_out,
      sizeof(connectstring_out),
      &len,
      /*			NULL,
              SQL_NTS,
              NULL,*/
      SQL_DRIVER_NOPROMPT);

  if(ret == SQL_SUCCESS){
    //printf("***** can able to connect to the database ***************\n");

#if 0
    SQLCHAR   SQLStmt[1024];
    SQLINTEGER rtnPersonExtn;

    //cout << "SQLDriverConnect Passed"<< endl;
    //  create a SQL Statement variable
    SQLHSTMT StatementHandle;
    ret  = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &StatementHandle);
    CLog::Detail( ODBC, "multithreaded-ippbx : odbc_db.cpp : SQLAllocHandle() : ret = %d",ret);
    cout << "***** StatementHandle value is "<< StatementHandle << endl;

    if(ret  == SQL_SUCCESS){

      if(SQLSetStmtAttr(StatementHandle, SQL_ATTR_ASYNC_ENABLE,(SQLPOINTER) SQL_ASYNC_ENABLE_ON, 0) == SQL_SUCCESS){
        printf("******* Successfully set async attr *******\n");
      }else{
        printf("******* Cannot able to set async attr *******\n");
        extract_error("SQLSetStmtAttr for stmt", StatementHandle, SQL_HANDLE_STMT);
      } 




      if( ::SQLSetStmtOption( StatementHandle, SQL_ASYNC_ENABLE, SQL_ASYNC_ENABLE_ON)==SQL_SUCCESS) {
        //SetError( this, DB_ERROR, 0, "Setting to async", 0, 0, hstmt );
        printf("******* Successfully set async option *******\n");
        //                                //printf("******* Cannot able to set async option *******\n");
      }else{
        printf("******* Cannot able to set async option *******\n");
        //                                                            //              printf("******* Successfully set async option *******\n");
      }

      strcpy((char *) SQLStmt, "exec UMN_prakash_retereive '7878'");
      ret = SQLExecDirect(StatementHandle, SQLStmt, SQL_NTS);

      if(ret  == SQL_SUCCESS){

        //cout << "SQLExecDirect is successfull" << endl;
        SQLBindCol(StatementHandle, 1, SQL_C_LONG, &rtnPersonExtn, sizeof(rtnPersonExtn), NULL );
        ret  = SQLFetch(StatementHandle);
        if(ret  == SQL_NO_DATA_FOUND)
          cout << "No sql data found "<< endl;
        //cout << "The result of the stored procedure is "<< endl;
        //cout << rtnPersonExtn << endl;
      }
    }









#endif


  }

  //CLog::Detail( ODBC, "multithreaded-ippbx : odbc_db.cpp : ret = %d .......5",ret);
  //SetError( this, DB_ERROR, 0, "Connecting to datasource", m_henv, m_hdbc);


  if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
    SQLCHAR State[10];
    SQLCHAR SqlState[6];
    SQLINTEGER    NativeError;
    SQLCHAR Msg[SQL_MAX_MESSAGE_LENGTH];
    SQLSMALLINT   MsgLen;
    SQLRETURN sqlRet;
    sqlRet = SQLGetDiagRec(SQL_HANDLE_DBC, m_hdbc, 1, State, NULL, NULL, 0, 0);
    sqlRet = SQLGetDiagRec(SQL_HANDLE_DBC, m_hdbc, 1, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen);
    SetError( this, DB_ERROR, 0, "Connecting to datasource", m_henv, m_hdbc);
    if (GetODBCError(m_henv, m_hdbc, SQL_NULL_HSTMT, m_FatalErrMsg))
      m_FatalErrMsg = GetLastSQLState();
    printf("***** not able to connect to the database err: %s ***************\n", Msg);
    goto on_error1;
  }


  return 0;

on_error1:
  ::SQLFreeHandle( SQL_HANDLE_DBC, m_hdbc);
on_error0:
  ::SQLFreeHandle( SQL_HANDLE_ENV, m_henv);
  return -1;
}

//-----------------------------------------------------------------------------
// Force completion of all running objects
int ODBC_Database::CancelQuery()
{
  // Cancel currently executing statement
  if (m_ExclusiveRst && !m_HasFatal) {
    ODBC_Recordset *rst = m_ExclusiveRst;
    if (rst->GetState() == DB_Async_Object::XSTATE_EXECUTING) {
      if (rst->PollEvents() == DB_Async_Object::XSTATE_EXECUTING) {
        CLog::Info( ODBC, rst->GetTask(), "Cancelling query:' %s'", rst->GetSQL());
        rst->CancelQuery();
      }
    }

  } else {
    if (m_RunningObject.size()) {
      DB_Async_Object *current = m_RunningObject.front();
      if (current->PollEvents() == DB_Async_Object::XSTATE_EXECUTING) {
        CLog::Info( ODBC, current->GetTask(), "Cancelling SQL:' %s'", current->GetSQL());
        current->CancelQuery();
      }
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------
// Close database connection and all alive objects
int ODBC_Database::Close()
{
  List <DB_Async_Object*>::iterator it, end_ptr=m_AllObject.end();
  for (it=m_AllObject.begin(); it!=end_ptr; ) {
    DB_Async_Object *a = *it;
    a->DestroyStmt();
    if (a->GetState() == DB_Async_Object::XSTATE_CLOSED) {
      it = m_AllObject.erase(it);
      delete a;
    } else {
      ++it;
    }
  }

  if (m_henv) {
    if (m_hdbc) {
      // this should not be happen
      // Don't delete the objects anyway so that memory leaks can be detected.
      //*** why commented??? /*
      while(!m_RunningObject.empty())
        m_RunningObject.pop_front();

      // close and delete all objects
      while (!m_AllObject.empty()) {
        DB_Async_Object *obj = m_AllObject.front();
        obj->Close();
        delete obj;

        m_AllObject.pop_front();
      }
      //*** why commented??? */

      // Free statement cache
      ClearStmtCache();

      // disconnect from datasource
      ::SQLDisconnect(m_hdbc);

      // free dbc handle
      ::SQLFreeHandle( SQL_HANDLE_DBC, m_hdbc);
      m_hdbc = 0;
    }

    // free env. handle
    ::SQLFreeHandle( SQL_HANDLE_ENV, m_henv);
    m_henv = 0;
  }
  return 0;
}

//-----------------------------------------------------------------------------
// Open query
DB_Recordset *ODBC_Database::OpenQuery(CTask *task, const char *sql, int opt)
{
  ODBC_Recordset *rst = new ODBC_Recordset( task, this);

  if (rst->Open( sql, opt) == 0) {
    m_AllObject.push_back(rst);

    // rst already put into m_RunningObject in Open()
    //m_RunningObject.push_back(rst);
    return rst;
  } else {
    delete rst;
    return 0;
  }
}

//-----------------------------------------------------------------------------
void ODBC_Database::SetLastSqlQuery(const void *obj, const char *query)
{
  if (obj != m_LastSqlObject) {
    m_LastSqlObject = obj;
    size_t len = strlen(query);
    if (len < sizeof(m_LastSqlQuery)) strcpy(m_LastSqlQuery, query);
    else memcpy(m_LastSqlQuery, query, sizeof(m_LastSqlQuery)-1);
  }
}

// Get the last query successfully executed.
const char *ODBC_Database::GetLastSqlQuery() const
{
  return m_LastSqlQuery;
}

//-----------------------------------------------------------------------------
// Execute SQL
DB_SQL_Exec *ODBC_Database::CreateExecutor(CTask *task)
{
  DB_SQL_Exec *executor = new ODBC_SQL_Exec( task, this);
  printf("********* Inside CreateExecutor ***************\n");
  if (executor->Init()) {
    delete executor;
    return 0;
  }

  m_AllObject.push_back(executor);

  return executor;
}


//-----------------------------------------------------------------------------
// Lock database connection.
// When the database is locked, only the statement which locked the
// database can be processed.
void ODBC_Database::Lock (ODBC_Recordset *rst) 
{ 
  assert (m_ExclusiveRst==NULL); 
  m_ExclusiveRst=rst; 
  m_tExclusive=GetTickCount(); 
}


//-----------------------------------------------------------------------------
// Unlock database connection.
void ODBC_Database::Unlock(ODBC_Recordset *rst) 
{ 
  assert (m_ExclusiveRst==rst); 
  m_ExclusiveRst=NULL; 

  DWORD now = GetTickCount();
  CLog::Debug ( ODBC, "[%s] Connection was locked by statement '%s' for %u ms", 
      GetProfile()->Name().c_str(),
      rst->GetSQL(),
      now - m_tExclusive);
}


//-----------------------------------------------------------------------------
// Get the task that exclusively using this connection.
// This will return non-NULL if the database is being Locked.
CTask *ODBC_Database::GetExclusiveOwner()
{
  if (m_ExclusiveRst)
    return m_ExclusiveRst->GetTask();
  return NULL;
}


//-----------------------------------------------------------------------------
// Get the object that currently runs exclusively in the connection
DB_Async_Object * ODBC_Database::GetExclusiveObject() 
{
  return m_ExclusiveRst;
}


//-----------------------------------------------------------------------------
// Abort the remaining database recordset/exec
void ODBC_Database::AbortJob(bool dont_close)
{
  if (m_ExclusiveRst) {
    assert (false);

    m_AllObject.remove (m_ExclusiveRst);
    m_RunningObject.remove (m_ExclusiveRst);
    OnAsyncEnd(m_ExclusiveRst);

    if (dont_close) {
      // Put back to m_AllObject
      m_AllObject.push_back(m_ExclusiveRst);
    } else {
      m_ExclusiveRst->DestroyStmt();
      delete m_ExclusiveRst;
    }
    m_ExclusiveRst = NULL;
  }

  if(m_RunningObject.size()) {
    List <DB_Async_Object*>::iterator it, end_ptr=m_RunningObject.end();
    for (it=m_RunningObject.begin(); it!=end_ptr; ) {
      DB_Async_Object *a = *it;
      OnAsyncEnd(a);
      it = m_RunningObject.erase(it);
    }
  }

  if (m_AllObject.size()) {
    List <DB_Async_Object*>::iterator it, end_ptr=m_AllObject.end();
    for (it=m_AllObject.begin(); it!=end_ptr; ) {
      DB_Async_Object *a = *it;
      if (dont_close) {
        ++it;
      } else {
        a->DestroyStmt();
        if (a->GetState() == DB_Async_Object::XSTATE_CLOSED) {
          it = m_AllObject.erase(it);
          delete a;
        } else
          ++it;
      }
    }
  }
}


//-----------------------------------------------------------------------------
// Poll events
int ODBC_Database::PollEvents()
{
  List <DB_Async_Object*>::iterator it, end_ptr;

  if (m_ExclusiveRst && !m_HasFatal) {
    ODBC_Recordset *rst = m_ExclusiveRst;
    if (rst->GetState() == DB_Async_Object::XSTATE_EXECUTING) {
      if (rst->PollEvents() != DB_Async_Object::XSTATE_CLOSED)
        goto on_return;
    } else if (rst->GetState() != DB_Async_Object::XSTATE_CLOSED)
      goto on_return;

    m_AllObject.remove (rst);
    m_RunningObject.remove (rst);
    delete rst;

    // Continue with PollEvents
  }


  if (m_RunningObject.empty())
    return 0;

  if (m_HasFatal) {
    List <DB_Async_Object*>::iterator it, end_ptr=m_RunningObject.end();
    for (it=m_RunningObject.begin(); it!=end_ptr; ) {
      DB_Async_Object *a = *it;
      OnAsyncEnd(a);
      it = m_RunningObject.erase(it);
    }
    return 0;
  }

  end_ptr=m_RunningObject.end();
  for (it=m_RunningObject.begin(); it!=end_ptr; ) {

    DB_Async_Object *a = *it;

    // poll/execute the object
    switch(a->PollEvents()) {

      // if there's any executing object, don't poll no more
      case DB_Async_Object::XSTATE_EXECUTING:
        //cout << "Inside xstate executing***" << endl;
        return 0;

        // if the object is closed, delete it
      case DB_Async_Object::XSTATE_CLOSED:
        //cout << "Inside xstate closed***" << endl;
        m_AllObject.remove(a);
        delete a;
        m_RunningObject.erase(it++);
        if (m_HasFatal)
          goto on_return;
        break;

        // otherwise, we can continue to poll other object
      case DB_Async_Object::XSTATE_ERROR:
      case DB_Async_Object::XSTATE_READY:
        //cout << "Inside xstate ready***" << endl;
        m_RunningObject.erase(it++);
        if (m_HasFatal)
          goto on_return;
        break;

        // this should never happen
      default:
        CLog::Fatal( ODBC, (*it)->GetTask(), "Unknown object state in ODBC db PollEvents");
        it++;
        break;
    }

    end_ptr=m_RunningObject.end();
  }

on_return:
  if (m_HasFatal) {
    GetSlot()->SignalDbFatal (this, m_ReallyReallyFatal);
  }
  return 0;
}

//-----------------------------------------------------------------------------
// When an object begins its async execution, put it in our running lists
int ODBC_Database::OnAsyncBegin( DB_Async_Object *obj )
{
  if (m_ExclusiveRst != obj) {
    assert (std::find(m_RunningObject.begin(), m_RunningObject.end(), obj) == m_RunningObject.end());
    m_RunningObject.push_back(obj);
  }
  return 0;
}

//-----------------------------------------------------------------------------
// Afte finishing execution, wakeup the owning task.
// Note: object removal from ODBC_Database will be done in PollEvents
int ODBC_Database::OnAsyncEnd( DB_Async_Object *obj )
{
  GetDriver()->WakeupTask( obj->GetTask() );
  return 0;
}

//-----------------------------------------------------------------------------
// allocate statement handle (or get from cache)
HSTMT ODBC_Database::AllocStmt()
{
  //printf("******* Inside AllocStmt *************\n");

  //HSTMT hstmt = SQL_NULL_HSTMT;
  //SQLHSTMT hstmt = SQL_NULL_HSTMT;
  SQLHSTMT hstmt;

  // check the cache first
  if ( m_StmtCache.size() > 0) {
    hstmt = m_StmtCache.front();
    m_StmtCache.pop_front();

    CLog::Detail( ODBC, "[%s] odbc_cache[%u]: popped 0x%X", 
        GetProfile()->Name().c_str(),
        m_StmtCache.size(), hstmt);
    return hstmt;
  } 

  int ret;

#if 0 // CBM

  SQLCHAR connectstr[1024+1],connectstring_out[1024+1];
  SQLSMALLINT len;

  //safe_strcpy((char*)connectstr,(char*)connectstring,sizeof(connectstr)-1); //CBM
  strcpy(connectstr,"DSN=MSSQLTestServer;UID=sa;PWD=abbeysql;");

  ret = ::SQLDriverConnect(
      m_hdbc,
      NULL,
      connectstr,
      SQL_NTS,
      connectstring_out,
      sizeof(connectstring_out),
      &len,
      /*          NULL,
                  SQL_NTS,
                  NULL,*/
      SQL_DRIVER_NOPROMPT);

  if(ret == SQL_SUCCESS){
    //printf("***** can able to connect to the database ***************\n");
  }else{
    printf("***** SQLDriverConnect is a failure ***************\n");
  }
#endif


  //create the new statement handle
  /* Allocating handle */
  //printf("************m_hdbc is %ld ***************\n", m_hdbc);
  /*		if ( MySQLAllocStmt( m_hdbc, &hstmt) == SQL_ERROR) {
        SetError( this, DB_ERROR, 0, "Alloc stmt handle", 0, m_hdbc );
        return SQL_NULL_HSTMT;
        }*/

  SQLRETURN rc = ::SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt);
  /* cout << "Inside MysQLALLOC statements, hconn is " << m_hdbc << "and hstmt is " << hstmt  << endl;
     cout << " size of hstmt is " << sizeof(SQLHSTMT) << endl;
     cout << "After mysqlallocStmt hstmt is " <<  hstmt << endl;
     */

#if 0// CBM -- For Testing

  SQLCHAR   SQLStmt[1024];
  SQLINTEGER rtnPersonExtn = 0;


  /*		SQLHSTMT StatementHandle;
        ret  = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &StatementHandle);
  //cout << "StatementHandle return value is "<< ret << endl;*/

  //      if(ret  == SQL_SUCCESS){
  strcpy((char *) SQLStmt, "exec UMN_prakash_retereive '7878'");
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);

  if(ret  == SQL_SUCCESS){

    //cout << "SQLExecDirect is successfull" << endl;
    SQLBindCol(hstmt, 1, SQL_C_LONG, &rtnPersonExtn, sizeof(rtnPersonExtn), NULL );
    ret  = SQLFetch(hstmt);
    if(ret  == SQL_NO_DATA_FOUND)
      cout << "No sql data found "<< endl;
    //cout << "The result of the stored procedure is "<< endl;
    //cout << rtnPersonExtn << endl;
  }
  //     }


#endif


#if 0
  // set cursor type
  if (m_Option.m_CursorType!=DB_Connection_Option::UNKNOWN) {
    if ( ::SQLSetStmtAttr( hstmt, SQL_ATTR_CURSOR_TYPE, 
          (SQLPOINTER)m_Option.GetODBCCursorType(), 0)==SQL_ERROR) {
      SetError( this, DB_ERROR, 0, "Setting cursor type to keyset driven", 0, 0, hstmt, NULL );
      goto on_error;
    }
  }

#if 1
  // rowset size
  if ( m_Option.m_RowsetSize > 0 && 
      ::SQLSetStmtOption(hstmt, SQL_ROWSET_SIZE, m_Option.m_RowsetSize) != SQL_SUCCESS) {
    SetError( this, DB_ERROR, 0, "Setting rowset", 0, 0, hstmt, NULL );
    goto on_error;
  }


  // concurrency
  if ( m_Option.m_ReadOnly &&
      ::SQLSetStmtOption(hstmt, SQL_CONCURRENCY, SQL_CONCUR_READ_ONLY) != SQL_SUCCESS) {
    SetError( this, DB_ERROR, 0, "Setting concurrency to read-only", 0, 0, hstmt, NULL );
    goto on_error;
  }

  // set query timeout
  if (m_Option.m_QueryTimeout != -1) {
    if ( ::SQLSetStmtAttr( hstmt, SQL_ATTR_QUERY_TIMEOUT, 
          (SQLPOINTER)m_Option.m_QueryTimeout, 0)==SQL_ERROR) {
      SetError( this, DB_ERROR, 0, "Setting query timeout", 0, 0, hstmt, NULL );
      goto on_error;
    }
  }

  // enabling var. length bookmark
  if ( ::SQLSetStmtAttr( hstmt, SQL_ATTR_USE_BOOKMARKS, (SQLPOINTER)SQL_UB_VARIABLE, 
        sizeof(SQLUINTEGER)) != SQL_SUCCESS) {
    SetError( this, DB_ERROR, 0, "Enabling bookmark", 0, 0, hstmt, NULL);
    goto on_error;
  }

#endif 
  // enabling async, if possible
  if (m_Option.m_EnableAsync) {
    //if ( ::SQLSetStmtOption( hstmt, SQL_ASYNC_ENABLE, SQL_ASYNC_ENABLE_ON)==SQL_ERROR) {
    if ( ::SQLSetStmtOption( hstmt, SQL_ASYNC_ENABLE, SQL_ASYNC_ENABLE_ON)==SQL_SUCCESS) {
      //SetError( this, DB_ERROR, 0, "Setting to async", 0, 0, hstmt );
      printf("******* Successfully set async option *******\n");
      //printf("******* Cannot able to set async option *******\n");
    }else{
      printf("******* Cannot able to set async option *******\n");
      //				printf("******* Successfully set async option *******\n");
    }
  }

#endif
  /*
     CLog::Detail( ODBC, "[%s] odbc_cache[%u]: alloc 0x%X", 
     GetProfile()->Name().c_str(),
     m_StmtCache.size(), hstmt);
     */
  n_opened_handle++;
  //printf("************ hstmt is %ld ***************\n", hstmt);
  //cout << "************* hstmt is "<< hstmt  << " *********" << endl;
  return hstmt;

on_error:
  printf("************ Freeing statement handle %ld ***************", hstmt);
  ::SQLFreeHandle( SQL_HANDLE_STMT, hstmt);

  return SQL_NULL_HSTMT;
  }

  //-----------------------------------------------------------------------------
  // deallocate statement handle (or put into cache)
  STATUS ODBC_Database::ReleaseStmt( HSTMT &hstmt)
  {

    // validate
    if (hstmt==SQL_NULL_HSTMT)
      return OK;


    // check if cache is full
    if (m_StmtCache.size() >= m_Option.m_StmtCacheSize) {
      /*
         CLog::Detail( ODBC, "[%s] odbc_cache[%u]: discard 0x%X", 
         GetProfile()->Name().c_str(),
         m_StmtCache.size(), hstmt);
         */

      // free the statement
      if ( MySQLFreeStmt( hstmt, SQL_CLOSE) != SQL_SUCCESS) {
        SetError( this, DB_ERROR, 0, "CloseCursor", m_henv, m_hdbc, hstmt, NULL);
        //return NOT_OK;
      }

      if ( MySQLFreeStmt( hstmt, SQL_DROP) != SQL_SUCCESS) {
        SetError( this, DB_ERROR, 0, "Freeing statement", m_henv, m_hdbc, hstmt, NULL);
        return NOT_OK;
      }
      --n_opened_handle;

      return OK;
    }


    // close cursor
    RETCODE ret=MySQLFreeStmt( hstmt, SQL_CLOSE );
    if (ret==SQL_SUCCESS_WITH_INFO) {
      SetError( this, DB_WARNING, 0, "CloseCursor", m_henv, m_hdbc, hstmt, NULL);
    } else if (ret != SQL_SUCCESS) {
      SetError( this, DB_ERROR, 0, "CloseCursor", m_henv, m_hdbc, hstmt, NULL);
    }


    // put into cache
    m_StmtCache.push_back(hstmt);

    CLog::Detail( ODBC, "[%s] odbc_cache[%u]: pushed 0x%X", 
        GetProfile()->Name().c_str(),
        m_StmtCache.size(), hstmt);

    return OK;
  }

  //-----------------------------------------------------------------------------
  // Clear statement cache
  void ODBC_Database::ClearStmtCache()
  {
    size_t n_entry = m_StmtCache.size();

    while (!m_StmtCache.empty()) {
      HSTMT hstmt = m_StmtCache.front();
      m_StmtCache.pop_front();

      if ( MySQLFreeStmt( hstmt, SQL_DROP) != SQL_SUCCESS) {
        SetError( this, DB_ERROR, 0, "Freeing statement", m_henv, m_hdbc, hstmt, NULL);
      }
    }

    CLog::Detail( ODBC, "[%s] odbc_cache[%u]: %u entries freed", 
        GetProfile()->Name().c_str(),
        m_StmtCache.size(), n_entry);

  }

