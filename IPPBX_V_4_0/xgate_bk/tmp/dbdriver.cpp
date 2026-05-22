/* $Header: /xGate/dbdriver.cpp 37    7/29/04 7:28p Bennylp $
 *   
 * XGATE PROJECT
 * (C) 2001 Switchlab, Ltd
 *
 * XGATE DATABASE SERVICE.
 *
 * 38 07122018-thread-locking-from-telnet-command
 * Done by Yoga - After supporting new ACE lib, we had thread locking problem
 * fixed at DB_Connection_Slot::Show function. why we need use ACE_GUARD in this funciton. calling funciton will do ACE_GUARD
 *
 * $Log: /xGate/dbdriver.cpp $
 * 
 * 37    7/29/04 7:28p Bennylp
 * Zombie the connection upon really fatal error
 * 
 * 36    1/22/04 10:49p Bennylp
 * Workaround to crash problems in DB_Connection_Slot::HandleEvents
 * 
 * 35    1/09/04 6:18p Bennylp
 * Added checking EOF/BOF condition before data is accessed
 * 
 * 34    4/23/03 5:34p Bennylp
 * Administrivia: tidy-up the usage of API provider ID. Now the
 * enumeration in api.h should be used instead of bare constants.
 * 
 * 33    10/22/02 5:14p Bennylp
 * Keep track of thread creation ("show threads")
 * 
 * 32    9/17/02 6:06p Bennylp
 * Fixed UMR (Uninitialized Memory Read) in pwd encryption.
 * 
 * 31    5/13/02 10:20a Bennylp
 * Fix crash if connect string is empty.
 * 
 * 30    5/09/02 9:17p Bennylp
 * More random encryption algorithm
 * 
 * 29    5/09/02 8:42p Bennylp
 * Don't put the secret in a string.
 * 
 * 28    5/09/02 7:54p Bennylp
 * Several enhancements to database:
 *   - encrypt password in connect string (RC4 + base64)
 *   - added information about currently running SQL in a connection
 *   - added lock information in connection
 *   - added command to cancel currently running SQL
 *   - added command to force reconnection of a DB connection
 * 
 * 27    5/08/02 7:30p Bennylp
 * Select connection based on the least used && show query time.
 * 
 * 26    5/08/02 6:33p Bennylp
 * Quick fix: show currently running query in one connection
 * 
 * 25    8/09/01 5:44p Bennylp
 * Reduce the number of CLog::Detail calls.
 * 
 * 24    8/06/01 7:36p Bennylp
 * Fixed bug in error message.
 * 
 * 23    8/03/01 5:14p Bennylp
 * Added feature to disable query timeout.
 * 
 * 22    7/13/01 11:41a Bennylp
 * Don't show handle zero in 'show db-handles'
 * 
 * 21    7/12/01 4:30p Bennylp
 * Added profile info in 'show db-handles'
 * 
 * 20    7/12/01 3:58p Bennylp
 * Add term command 'show db-handles'
 * 
 * 19    6/09/01 4:58p Bennylp
 * Fix bug which causes crash in dbdrv::AllocHandle.
 * 
 * 18    6/08/01 2:56p Bennylp
 * Fixed bugs in dbdriver (reload).
 * 
 * 17    6/07/01 1:10p Bennylp
 * Bug fixes in reloading profile.
 * 
 * 16    6/06/01 8:34p Bennylp
 * Fixed bug.
 * 
 * 15    6/06/01 7:09p Bennylp
 * Supports dynamic config and auto-connect when connection failed.
 * 
 * 14    5/30/01 8:50p Bennylp
 * Fixed system locking
 * 
 * 13    5/26/01 8:04p Bennylp
 * Record each call to AcquireSystemLock in DEBUG mode.
 * 
 * 12    5/25/01 9:42p Bennylp
 * Fixed database connection busy.
 * 
 * 11    5/24/01 2:07p Bennylp
 * Fix bug variable not initialized (isShutdown)
 * 
 * 10    5/22/01 11:01a Bennylp
 * Verify task's last error.
 * 
 * 9     10/05/01 13:19 Junanto
 * Added a new line between database-profile
 * 
 * 8     10/05/01 12:40 Junanto
 * Replaced "\r\n" sequence to "\n". Telnet translates
 * an orphaned "\n" into "\r\n" during output
 * 
 * 7     10/05/01 11:33 Junanto
 * 
 * 6     10/05/01 11:29 Junanto
 * Replaced all "\r\n" with a constant
 * 
 * 5     1/05/01 15:21 Junanto
 * reduced and simplified log output
 * 
 * 4     4/30/01 7:45a Bennylp
 * Integrate database service.
 * 
 * 3     4/28/01 6:33a Bennylp
 * Added service profiling and database driver (testing)
 */
#include <sqlext.h>
//#include "stdafx.h"
#include <iostream>
#include "dbdriver.h"
#include "odbc.h"
#include "kernel.h"
#include "task.h"
#include "log.h"
#include "term.h"
#include "thread_process.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
 
#include <sstream>

#include "arc4.h"
#include "../slsip/encryption/base64.h"
using namespace std;

#define DBDRIVER	    "dbdrv"
#define NEWLINE		    "\n"
#define _snprintf   snprintf    //linux porting

// Static variables
DB_Service *DB_Service::dbSvcInstance;


#define DB_DEFAULT_STMT_CACHE	    0
#define DB_DEFAULT_CONNECTION_CNT   1
#define DB_DEFAULT_TIMEOUT	    300
#define DB_DEFAULT_CURSOR_TYPE	    DB_Connection_Option::STATIC
#define DB_DEFAULT_CURSOR_SIDE	    DB_Connection_Option::SERVER_SIDE
#define DB_DEFAULT_ASYNC	    true
#define DB_DEFAULT_READ_ONLY	    false
#define DB_DEFAULT_ROWSET_SIZE	    0
#define DB_MAX_CONNECTION	    96
#define DB_PROVIDER_ID		    API_PROVIDER_ID_DB
#define DB_CONNECT_HOLDING_TIME	    30

/*
 * !!! Changing these values will make the hashed password incompatible !!!
 */
#define DB_MAX_PWD_LEN		    32
#define DB_RANDOM_CIPHER_LEN	    2
static unsigned char dbSecret[DB_MAX_PWD_LEN];

enum {
    // commands for database connection profile
    TermCmd_ConnectionCount,
    TermCmd_ConnectString,
    TermCmd_CancelQuery,
    TermCmd_CursorSide,
    TermCmd_CursorType,
    TermCmd_Driver,
    TermCmd_Password,
    TermCmd_QueryTimeout,
    TermCmd_RowsetSize,
    TermCmd_ReadOnly,
    TermCmd_StatementCacheSize,
    TermCmd_Reload,
    TermCmd_Reconnect,
    TermCmd_Show,
    TermCmd_Shutdown,
    TermCmd_KillZombies,

    // command for database service manager
    TermCmd_Database,
};

#define CMD_CANCEL_QUERY	    "cancel-query"
#define CMD_CONNECTION_COUNT	    "connection-count"
#define CMD_CONNECT_STRING	    "connect-string"
#define CMD_CURSOR_SIDE		    "cursor-side"
#define CMD_CURSOR_TYPE		    "cursor-type"
#define CMD_DRIVER		    "driver"
#define CMD_PASSWORD		    "password"
#define CMD_QUERY_TIMEOUT	    "query-timeout"
#define CMD_ROWSET_SIZE		    "rowset-size"
#define CMD_READ_ONLY		    "read-only"
#define CMD_STATEMENT_CACHE	    "statement-cache"
#define CMD_SHUTDOWN		    "shutdown"
#define CMD_DATABASE		    "database-profile"
#define CMD_RELOAD          	    "reload"
#define CMD_RECONNECT		    "reconnect"
#define CMD_SHOW		    "status"
#define CMD_KILL_ZOMBIES	    "kill-zombies"

static IdNamePair __curtype[] = {
    { DB_Connection_Option::UNKNOWN,	    "unknown"},
    { DB_Connection_Option::FORWARD_ONLY,   "forward-only" },
    { DB_Connection_Option::STATIC,	    "static" },
    { DB_Connection_Option::KEYSET,	    "keyset" },
    { DB_Connection_Option::DYNAMIC,	    "dynamic" }
};

static IdNamePair __curside[] = {
    { DB_Connection_Option::UNKNOWN_SIDE, "unknown"},
    { DB_Connection_Option::SERVER_SIDE,  "server"},
    { DB_Connection_Option::CLIENT_SIDE,  "client"}
};

///////////////////////////////////////////////////////////////////////////////

#define IS_PWD(p)   (*p==';' && tolower(*(p+1))=='p' && tolower(*(p+2))=='w' && \
		     tolower(*(p+3))=='d' && tolower(*(p+4))=='=')

/*
 * Extract the password portion from the connect string.
 * The extracted string is in the format ";PWD=xxx".
 */
const char *password_from_connect_string(const String &cs, int *length)
{
    const char *start = NULL;
    int i;

    if (cs.size() < 5) {
	*length = 0;
	return NULL;
    }

    for (i=0; i<cs.size()-5; ++i) {
	start = cs.c_str()+i;
	if (IS_PWD(start))
	    break;
    }
    if(!IS_PWD(start))
	return NULL;

    const char *end = strchr(start+1, ';');
    if (end == NULL)
	end = cs.c_str() + cs.size();

    *length = end - start;
    return start;
}

/*
 * Replace the password section in connect string with the specified password.
 * The password must be in the complete string.
 */
static String ReplaceConnectStringPassword(const String & cs, const String & password)
{
    if (cs.size() < 5)
	return cs;

    String str = cs;
    int length;
    const char *start;

    start = password_from_connect_string(str, &length);
    if (start)
	str.replace( (start-str.c_str()), length, password);
    else {
	if (*(str.c_str() + str.size() - 1) == ';')
	    str.insert( str.size()-1, password);
	else {
	    str += password;
	    str += ";";
	}
    }
    return str;
}

/*
 * Remove the password section in the connect string for safe display.
 */
static String SafeConnectString(const String & cs)
{
    if (cs.size() < 5)
	return cs;

    String str = cs;
    int length;
    const char *start;

    start = password_from_connect_string(str, &length);
    if (start) {
	str.replace( (start-str.c_str()), length, "");

	// Sometimes by removing password, the last ';' will be removed as well
	// (if the 'PWD' token doesn't contain ';'). This will give hint that
	// the connect string has a password. So, add a ';' if it is missing.
	if (*(str.c_str() + str.size() - 1) != ';')
	    str += ";";
    }
    return str;
}


#undef IS_PWD


SQLRETURN MySQLAllocStmt(SQLHDBC h_conn, SQLHSTMT *h_stmt)
{
    SQLRETURN rc = ::SQLAllocHandle(SQL_HANDLE_STMT, h_conn, h_stmt);
	/* cout << "Inside MysQLALLOC statements, hconn is " << h_conn << "and h_stmt is " << h_stmt  << endl;
	cout << " size of h_stmt is " << sizeof(SQLHSTMT) << endl;
	*/
		
    return rc;
}

SQLRETURN MySQLFreeStmt(SQLHSTMT h_stmt, SQLUSMALLINT opt)
{
    SQLRETURN rc;
    switch (opt) {
    case SQL_CLOSE:
	//*** rc = ::SQLCloseCursor(h_stmt);
	rc = ::SQLFreeStmt(h_stmt, SQL_CLOSE);
	break;
    case SQL_DROP:
	rc = ::SQLFreeHandle(SQL_HANDLE_STMT, h_stmt);
	break;
    default:
	return SQL_ERROR;
    }

    return rc;
}

///////////////////////////////////////////////////////////////////////////////
DB_Connection_Option::DB_Connection_Option()
: m_CursorType(DB_DEFAULT_CURSOR_TYPE), 
  m_QueryTimeout(DB_DEFAULT_TIMEOUT), 
  m_EnableAsync(DB_DEFAULT_ASYNC), 
  m_StmtCacheSize(DB_DEFAULT_STMT_CACHE),
  m_CursorSide(DB_DEFAULT_CURSOR_SIDE), 
  m_RowsetSize(DB_DEFAULT_ROWSET_SIZE), 
  m_ReadOnly(DB_DEFAULT_READ_ONLY) 
{}

DB_Connection_Option::CursorType DB_Connection_Option::GetCursorType(const char *type_name)
{
    for (unsigned i=0; i<sizeof(__curtype)/sizeof(__curtype[0]); ++i) {
	if (!strcasecmp( type_name, __curtype[i].Name))
	    return static_cast<CursorType>(__curtype[i].Id);
    }
    return UNKNOWN;
}

unsigned long DB_Connection_Option::GetODBCCursorType() const
{
    switch(m_CursorType) {
    case FORWARD_ONLY:
	return SQL_CURSOR_FORWARD_ONLY;
    case KEYSET:
	return SQL_CURSOR_KEYSET_DRIVEN;
    case DYNAMIC:
	return SQL_CURSOR_DYNAMIC;
    case STATIC:
	return SQL_CURSOR_STATIC;
    default:
	return SQL_CURSOR_TYPE_DEFAULT;
    }
}

DB_Connection_Option::CursorSide DB_Connection_Option::GetCursorSide(const char *side_name)
{
    for (unsigned i=0; i<sizeof(__curside)/sizeof(__curside[0]); ++i) {
	if (!strcasecmp( side_name, __curside[i].Name))
	    return static_cast<CursorSide>(__curside[i].Id);
    }
    return UNKNOWN_SIDE;
}

unsigned long DB_Connection_Option::GetODBCCursorSide() const
{
    switch(m_CursorSide) {
    case SERVER_SIDE:
	return SQL_CUR_USE_DRIVER;
    case CLIENT_SIDE:
	return SQL_CUR_USE_ODBC;
    }

    return SERVER_SIDE;
}

const char * DB_Connection_Option::GetCursorTypeString() const
{
    for (unsigned i=0; i<sizeof(__curtype)/sizeof(__curtype[0]); ++i) {
	if (__curtype[i].Id == m_CursorType)
	    return __curtype[i].Name;
    }
    return __curtype[0].Name;
}


const char * DB_Connection_Option::GetCursorSideString() const
{
    for (unsigned i=0; i<sizeof(__curside)/sizeof(__curside[0]); ++i) {
	if (__curside[i].Id == m_CursorSide)
	    return __curside[i].Name;
    }
    return __curside[0].Name;
}


///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
DB_Database::~DB_Database()
{
    m_Signature = 0x5678ABCD;
}


bool DB_Database::is_valid() const
{
    try {
	return m_Signature == SIGNATURE;
    } catch (...) {
	return false;
    }
}

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
// TLS CTOR
CDbTLS::CDbTLS() 
: m_Current(-1), m_Ret(NULL)
{  
    typedef DB_Async_Object *p_CDbAsyncObject;
    memset( m_Handle, 0, MAX_DB_HANDLE * sizeof(p_CDbAsyncObject));

}

//-----------------------------------------------------------------------------
// TLS DTOR
CDbTLS::~CDbTLS() 
{ 
}



///////////////////////////////////////////////////////////////////////////////
/** Constructor.
 */
DB_Connection_Slot::DB_Connection_Slot (unsigned idx, DB_Connection_Profile & profile)
: dbSignature(SIGNATURE), 
  dbProfile(profile), state(State_Null), slotIndex(idx), dbConnection(NULL),
  lastConnectTry(0), lastUsage(0), dbTransition(0)
{
}

DB_Connection_Slot::~DB_Connection_Slot()
{
    dbSignature = 0x12345678;
}

bool DB_Connection_Slot::is_valid() const
{
    try {
	return dbSignature == SIGNATURE;
    } catch (...) {
	return false;
    }
}

/** Start open connection to remote host, synchronously.
 */
bool DB_Connection_Slot::Open()
{
    assert (dbConnection == NULL);
    return Reconnect(false);
}


/** Close connection to remote host.
 */
bool DB_Connection_Slot::Destroy()
{
    // wait for pending connection to finish.
    for (;;) {
	connectionMutex.acquire();
	DB_Connect_Param *param = NULL;
	if (!connectingList.empty()) {
	    param = connectingList.front();
        cout << " ****************** Inside Destroy removing the connectingList *********************"  << endl;
	    connectingList.remove (param);
	}
	connectionMutex.release();

	if (!param)
	    break;

	// Wait until the thread finishes
	ACE_Thread::join(param->handle);

	connectionMutex.acquire();
	delete param;
	connectionMutex.release();
    }

    connectionMutex.acquire();
    CHECK_SYSTEM_LOCK();

    if (dbConnection) {
	dbConnection->Close();
	delete dbConnection;
	dbConnection = NULL;
    }
    connectionMutex.release();

    SetState (State_Null);
    return true;
}


/** Disconnect connection to remote host.
 */
bool DB_Connection_Slot::Disconnect()
{
    ConnectionGuard guard(connectionMutex);
    CHECK_SYSTEM_LOCK();

    lastConnectTry = time(NULL);

    if (dbConnection) {
	if (dbConnection->GetHandlesCount()==0) {
	    dbConnection->Close();
	    delete dbConnection;
	    dbConnection = NULL;
	} else {
	    dbDeactivateList.push_back(dbConnection);
	    dbConnection = NULL;
	}
    }
    // All other pending connection is not the one to be active.
    List <DB_Connect_Param*> ::iterator pit, pend;
    pend = connectingList.end();
    for (pit=connectingList.begin(); pit!=pend; ++pit) {
	DB_Connect_Param *p = *pit;
	p->tobeActive = false;
    }
    SetState (State_Closing);
    
    return true;
}


/** Cancel currently running query in active connection.
 */
bool DB_Connection_Slot::CancelQuery()
{
    ConnectionGuard guard(connectionMutex);
    if (dbConnection)
	dbConnection->CancelQuery();
    List <DB_Database*>::iterator it, end = dbDeactivateList.end();
    for (it=dbDeactivateList.begin(); it!=end; ++it) {
	DB_Database* db = *it;
	db->CancelQuery();
    }
    return true;
}

/** Reconnect to database server using the configuration from the parent
    database profile.
 */
bool DB_Connection_Slot::Reconnect (bool force)
{

    ConnectionGuard guard(connectionMutex);
    CHECK_SYSTEM_LOCK();

    // When 'force' is applied, disconnect active and closing databases.
    if (force) {
	CancelQuery();
    }

    if (dbConnection) {
	dbDeactivateList.push_back (dbConnection);
	dbConnection = NULL;
    }

  //  cout << "****************************** Setting Trying *******************" << endl;
    SetState (State_Connecting);

    ODBC_Database *odbc = new ODBC_Database( DB_Service::Instance(), &dbProfile, this);
    odbc->SetOption(dbProfile.GetConnectionOption()); // CBM

    // Create connection parameter.
    DB_Connect_Param *param = new DB_Connect_Param;
    param->connection = odbc;
    param->connectString = dbProfile.GetConnectString();
    param->tobeActive = true;
    //cout << "****************************** Setting Active *******************" << endl;
    param->finished = false;
    param->slot = this;

    // All other pending connection is not the one to be active.
    List <DB_Connect_Param*> ::iterator pit, pend;
    pend = connectingList.end();
    for (pit=connectingList.begin(); pit!=pend; ++pit) {
	DB_Connect_Param *p = *pit;
	p->tobeActive = false;
    }
    connectingList.push_back(param);
 //   cout << " ************** Inside  DB_Connection_Slot::Reconnect , connectingList size is " << connectingList.size() <<" and this pointer val is " << this <<  endl;

    /* int spawn( const char *filename, int line,
                unsigned stack_size,
                ACE_THR_FUNC func,
                void *arg = 0,
                int flags = THR_NEW_LWP | THR_JOINABLE,
                ACE_thread_t *t_id = 0,
                ACE_hthread_t *t_handle = 0); */

      pthread_t my_thread;
    //if (SPAWN_THREAD(&start_thread, param, THR_NEW_LWP | THR_JOINABLE, NULL, &param->handle))
    int ret = pthread_create (&my_thread, NULL, &start_thread, param) ;
    if(ret != 0)
    {
	//connectionMutex.acquire();
//	cout << " ************* 1111 removing the connectingList *******************"  << endl;
	connectingList.remove(param);
	//connectionMutex.release();

	delete odbc;
	delete param;
	SetState (State_Failed);
	return false;


     }

    return true;
}


/** Get number of database jobs associated with this slot.
 */
size_t DB_Connection_Slot::GetHandlesCount() const
{
    ConnectionGuard guard(connectionMutex);

    unsigned handles=0;
    if (dbConnection)
	handles += dbConnection->GetHandlesCount();

    List <DB_Database*>::const_iterator it, end;
    end = dbDeactivateList.end();
    for (it=dbDeactivateList.begin(); it!=end; ++it) {
	DB_Database *db = *it;
	handles += db->GetHandlesCount();
    }

    return handles;
}

/** Get number of pending jobs/statement.
 */
size_t DB_Connection_Slot::GetJobCount() const
{
    ConnectionGuard guard(connectionMutex);

    unsigned jobcount=0;
    if (dbConnection)
	jobcount += dbConnection->GetJobCount();

    List <DB_Database*>::const_iterator it, end;
    end = dbDeactivateList.end();
    for (it=dbDeactivateList.begin(); it!=end; ++it) {
	DB_Database *db = *it;
	jobcount += db->GetJobCount();
    }

    return jobcount;
}


/** Get total number of connection count in this slot.
 */
size_t DB_Connection_Slot::GetConnectionCount()
{
    ConnectionGuard guard(connectionMutex);

    unsigned cc = 0;
    if (dbConnection)
	cc += 1;
    cc += dbDeactivateList.size();
    cc += connectingList.size();

    return cc;
}

/** Detemine whether slot is ready to be destroyed.
 */
bool DB_Connection_Slot::CanBeDestroyed()
{
    ConnectionGuard guard(connectionMutex);
    if (!connectingList.empty())
	return false;

    return GetHandlesCount() == 0;
}


/** Event polling.
 */
void DB_Connection_Slot::HandleEvents()
{
    // Workaround until bug is found!!!!!!!!!!!!!!!!
  //  cout << "***********Inside  DB_Connection_Slot::HandleEvents *********************" << endl;
  //  HDBC tmp_hdbc;
    if (!is_valid()) {
	CLog::Fatal(DBDRIVER, "Caught exception: invalid DB SLOT in HandleEvents()");
	return;
    }

    ConnectionGuard guard(connectionMutex);

    if (dbConnection) {
	// Workaround until bug is found!!!!!!!!!!!!!!!!
	if (!dbConnection->is_valid()) {
	    CLog::Fatal(DBDRIVER, "Caught exception: invalid DB connection in HandleEvents()");
	    dbConnection = NULL;
	    Reconnect(false);
	    return;
	}

 //   cout << "**************** Calling dbConnection->PollEvents *********************" << endl;
//    tmp_hdbc = dbConnection->GetHDbc();
//    cout << "************** Inside  DB_Connection_Slot::HandleEvents() , connection list size is " <<  connectingList.size() << " and this pointer is "  << this << endl;
//    extract_error("datatase_connection_check",
	dbConnection->PollEvents();

   
	// dbConnection may become NULL after PollEvents

    } else if (GetState()==State_Holding && time(NULL)-lastConnectTry > DB_CONNECT_HOLDING_TIME) {
	Reconnect(false);
	lastConnectTry = time(NULL);
    }

    if (!dbDeactivateList.empty()) {
	List <DB_Database*>::iterator it, end;
	end = dbDeactivateList.end();
	for (it=dbDeactivateList.begin(); it!=end;) {
	    DB_Database *db = *it;
	    db->PollEvents();
	    if (db->GetHandlesCount() == 0) {
		db->Close();
		delete db;
		it = dbDeactivateList.erase(it);
	    } else
		++it;
	}
    }

#if 0

    if (!connectingList.empty()) {
	List <DB_Connect_Param*> ::iterator pit, pend;
	pend = connectingList.end();
	for (pit = connectingList.begin(); pit!=pend; ) {
	    DB_Connect_Param *p = *pit;
	    if (p->finished) {
		// Make sure the thread has quited before the handle is destroyed.
		// This should happen immediately.
		//WaitForSingleObject (p->handle, INFINITE);
		//pthread_mutex_lock(&p->handle);	//linux porting
		//CloseHandle (p->handle);
		//pthread_mutex_destroy(p->handle);
		delete p;
        cout << " ************** Revoving the connecting LIst *************"  << endl;
		pit = connectingList.erase (pit);
	    } else
		++pit;
	}
    }
#endif 
}


/** This will create and open a recordset object, using the next database 
    connection in round-robin fashion.
 */
DB_Recordset *DB_Connection_Slot::CreateRecordset(CTask *task, 
						  bool task_has_exclusive_lock, 
						  const char *sql)
{
    ConnectionGuard guard(connectionMutex);
    CHECK_SYSTEM_LOCK();

    if (GetState() != State_Active || !dbConnection) {
	CLog::Detail (DBDRIVER, task, "  %d: connection is NOT available", this->slotIndex);
	return NULL;
    }

    CLog::Detail (DBDRIVER, task, "  %d: connection is available", this->slotIndex);

    if (dbConnection->GetExclusiveObject () == NULL) {
	
	CLog::Detail (DBDRIVER, task, 
		      "%s: Using connection slot %u. %s", 
		      dbProfile.Name().c_str(),
		      slotIndex,
		      (task_has_exclusive_lock ? "WARNING: task has exclusive lock" : ""));
	
	lastUsage = time(NULL);
	return dbConnection->OpenQuery(task, sql);

    } else if (dbConnection->GetExclusiveOwner() != task && !task_has_exclusive_lock) {
	
	CLog::Detail (DBDRIVER, task, 
		      "%s: Using connection slot %u (currently locked by task %u)", 
		      dbProfile.Name().c_str(),
		      slotIndex, 
		      dbConnection->GetExclusiveOwner()->GetPID());
	 
	lastUsage = time(NULL);
	return dbConnection->OpenQuery(task, sql);

    } 

    
    CLog::Detail (DBDRIVER, task, 
		  "%s: Database connection %u is locked by task %u and can not be used", 
		  dbProfile.Name().c_str(),
		  slotIndex, 
		  dbConnection->GetExclusiveOwner()->GetPID());
    
    return NULL;
}


/** This will create and open an executor object, using the next database 
    connection in round-robin fashion.
 */
DB_SQL_Exec *DB_Connection_Slot::CreateSQLExec(CTask *task, 
					       bool task_has_exclusive_lock, 
					       const char *sql)
{
    ConnectionGuard guard(connectionMutex);
    CHECK_SYSTEM_LOCK();

    if (!dbConnection)
	return NULL;

    if (dbConnection->GetExclusiveObject () == NULL) {
	
	CLog::Detail (DBDRIVER, task, 
		      "%s: Using connection slot %u. %s", 
		      dbProfile.Name().c_str(),
		      slotIndex,
		      (task_has_exclusive_lock ? "WARNING: task has exclusive lock" : ""));

	lastUsage = time(NULL);
	return dbConnection->CreateExecutor(task);

    } else if (dbConnection->GetExclusiveOwner() != task && !task_has_exclusive_lock) {
	
	CLog::Detail (DBDRIVER, task, 
		      "%s: Using connection slot %u (currently locked by task %u)", 
		      dbProfile.Name().c_str(),
		      slotIndex, 
		      dbConnection->GetExclusiveOwner()->GetPID());
	
	lastUsage = time(NULL);
	return dbConnection->CreateExecutor(task);

    } 

    
    CLog::Detail (DBDRIVER, task, 
		  "%s: Database connection %u is locked by task %u and can not be used", 
		  dbProfile.Name().c_str(),
		  slotIndex, 
		  dbConnection->GetExclusiveOwner()->GetPID());
    
    return NULL;
}


/** Kill Zombies.. */
void DB_Connection_Slot::KillZombies()
{
    List <DB_Zombie*>::iterator it, end = dbZombieList.end();
    for (it = dbZombieList.begin(); it != end; ++it) {
	DB_Zombie *z = *it;
	z->db->Close();
	delete z->db;
	delete z;
    }

    dbZombieList.clear();
}

/** Show status to the specified terminal.
 */
void DB_Connection_Slot::Show (String & output)
{
    //ConnectionGuard guard(connectionMutex); //TODO: VVI - refer - 07122018-thread-locking-from-telnet-command

    const char *statestr = "";
    switch (GetState()) {
    case State_Null:	    statestr = "null"; break;
    case State_Connecting:  
//        cout << "*****************In trying state *******************" << endl;
        statestr = "trying"; break;
    case State_Holding:	 
 //       cout << "*****************In holding state *******************" << endl;
       statestr = "waiting"; break;
    case State_Failed:	 
   //     cout << "*****************In failing state *******************" << endl;
       statestr = "FAILED"; break;
    case State_Active:	  
     //   cout << "*****************In Active state *******************" << endl;
      statestr = "active"; break;
    case State_Closing:	   
       // cout << "*****************In Closing state *******************" << endl;

     statestr = "closing"; break;
    case State_CloseWait:  
       // cout << "*****************In Closingwait state *******************" << endl;
         statestr = "closewt"; break;
    }

    char temp[128];
    sprintf (temp,  "%s\tpending:%u, closing:%u, downcnt:%u, zombies:%u, handles=%u, jobs=%u %s", 
		    statestr, 
		    //(dbConnection ? 1 : 0), 
		    connectingList.size(),
		    dbDeactivateList.size(),
		    dbTransition,
		    dbZombieList.size(),
		    GetHandlesCount(),
		    GetJobCount(),
		    dbConnection && dbConnection->GetExclusiveObject() ? "(locked)" : "");
    output += temp;

    if (GetState()==State_Holding || GetState()==State_Failed) {
	_snprintf(temp, 64, "[%03ds] \"%s\"", 
			    DB_CONNECT_HOLDING_TIME - (time(NULL)-lastConnectTry), 
			    (lastErrMsg.size() < 50 ? lastErrMsg.c_str() : lastErrMsg.c_str()+(lastErrMsg.size()-50)));
	temp[64] = '\0';
	output += "\n";
	output.append(16, ' ');
	output += temp;

    } 
    
    // Print all exclusive lock owner.
    List<DB_Database*> conn_list = dbDeactivateList;
    if (dbConnection)
	conn_list.push_front(dbConnection);
    List<DB_Database*>::iterator it, end=conn_list.end();

    /* Exclusive owner is EQUAL to currently executing.
    for (it = conn_list.begin(); it!=end; ++it) {
	DB_Database *conn = *it;
	const DB_Async_Object *obj = conn->GetExclusiveObject();
	if (obj == NULL)
	    continue;

	const CTask *task = NULL;
	if (obj)
	    task = obj->GetTask();

	_snprintf(temp, 64, "%sLck.Own: [%u] \"%s\"", 
		  (conn==dbConnection ? "*" : ""),
		  task ? task->GetPID() : 0, 
		  obj ? obj->GetSQL() : "??");
	temp[64] = '\0';
	output += "\n";
	output.append(16, ' ');
	output += temp;
    }
    */

    // Print all currently executing statements in connections.
    for (it = conn_list.begin(); it!=end; ++it) {
	DB_Database *conn = *it;
	if (conn->GetJobCount() == 0)
	    continue;
	const DB_Async_Object *obj = conn->GetExecutingObject();
	const CTask *task = NULL;
	if (obj)
	    task = obj->GetTask();

	String exec_time;
	if (obj)
	    exec_time = ConvertInterval2String(obj->GetTimeBeginExecuting() ? time(NULL)-obj->GetTimeBeginExecuting() : 0);
	else
	    exec_time = "0";
	_snprintf(temp, 120, "%sCurrent: [%u] %s \"%s\"", 
		  (conn==dbConnection ? "*" : ""),
		  task ? task->GetPID() : 0, 
		  exec_time.c_str(),
		  obj ? obj->GetSQL() : "??");
	temp[120] = '\0';
	output += "\n";
	output.append(16, ' ');
	output += temp;
    }


    /***
    // Print all opened statements in connections.
    for (it = conn_list.begin(); it!=end; ++it) {
	ODBC_Database *conn = (ODBC_Database*)*it;
	if (conn->m_AllObject.size() == 0)
	    continue;

	List <DB_Async_Object*>::iterator obj = conn->m_AllObject.begin();
	while (obj != conn->m_AllObject.end()) {
	    const CTask *task = (*obj)->GetTask();

	    _snprintf(temp, 120, "%sOpened: [%u] \"%s\"", 
		      (conn==dbConnection ? "*" : ""),
		      task ? task->GetPID() : 0, 
		      (*obj)->GetSQL());
	    temp[120] = '\0';
	    output += "\n";
	    output.append(16, ' ');
	    output += temp;

	    obj++;
	}
    }
    ***/
}


/** Notification from database connection about the status.
 */
void DB_Connection_Slot::NotifyConnectionStatus(int status, DB_Connect_Param *param)
{
    ConnectionGuard guard(connectionMutex);

    if (status == 0) {
	if (param->tobeActive) {
	    if (dbConnection) {
		dbDeactivateList.push_back (dbConnection);
	    }
	    dbConnection = param->connection;
	    SetState (State_Active);

	} else {
	    param->connection->Close();
	    delete param->connection;
	    param->connection = NULL;
	}
    } else {
	if (param->tobeActive) {
	    SetState (State_Holding);
	    lastConnectTry = time(NULL);
	    SetLastError (param->connection->GetLastErrMsg());
	}
	delete param->connection;
	param->connection = NULL;
    }

    param->finished = true;
}


// Worker thread for opening connection.
void *DB_Connection_Slot::start_thread(void *p)
{
    DB_Connect_Param *param = (DB_Connect_Param *)p;
    param->slot->DBOpen(param);
    return 0;
}

void DB_Connection_Slot::DBOpen ( DB_Connect_Param *param )
{
  //  cout << "************* Inside  DB_Connection_Slot::DBOpen ******************" << endl;
    assert (param);
    int status = param->connection->Open (param->connectString.c_str());
    NotifyConnectionStatus (status, param);
}


// this driver that an unrecoverable error has occured.
void DB_Connection_Slot::SignalDbFatal(DB_Database* sender, bool really_really_fatal)
{
 //   cout << " ************* SignalDbFatal *********************"  << endl;
    String errmsg = sender->GetLastErrMsg();

    sender->AbortJob(really_really_fatal);

    ConnectionGuard guard(connectionMutex);
    if (sender==dbConnection) {

	if (really_really_fatal) {
	    DB_Zombie *z = new DB_Zombie;
	    z->timeDead = time(NULL);
	    z->db = sender;
	    z->err = errmsg;
	    dbZombieList.push_back(z);
	} else {
	    dbDeactivateList.push_back(sender);
	}

	dbConnection=NULL;
	SetState (State_Holding);
	lastConnectTry = time(NULL) - DB_CONNECT_HOLDING_TIME*2/3;
	++dbTransition;
	SetLastError (errmsg);
    }
}



///////////////////////////////////////////////////////////////////////////////
/** Database connection manager CONSTRUCTOR
 */
DB_Connection_Profile::DB_Connection_Profile(const String &name)
: ServiceProvider(name, false, true, High_Priority, "Database Profile", IC_DB_Connection_Profile),
  cfgEntry(NULL),
  isShutdown(true)
{
    workingParam.connectionCount = 0;
    db_profile_not_reachable_count = 0;
    handle_event_count = 0;
    can_reload_db_profile = true;
    //workingParam.dbDriver = "unixODBC";
    //currentParam.dbDriver = "unixODBC";
    workingParam.dbDriver = "FreeTDS";
    currentParam.dbDriver = "FreeTDS";
    currentParam.connectionCount = DB_DEFAULT_CONNECTION_CNT;

    cfgEntry = new Cfg_Tree_Group(this->Name(), "", DB_Service::Instance()->GetRootConfig());

    Term_Cmd *cmd = NULL;

    // register 'connection-count'
    cmd = new Term_Cmd (this,
			TermCmd_ConnectionCount,
			CMD_CONNECTION_COUNT,
			"Number of database connections",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    cmd->Add (new Term_Cmd_Arg("cnt", 
			       CVariant(DB_DEFAULT_CONNECTION_CNT), 
			       CVariantRangeValidator(1, CVariant(DB_MAX_CONNECTION), "Number of database connections")) );
    RegisterTermCmd (cmd, cfgEntry);


    // register 'reload' command
    cmd = new Term_Cmd (this,
			TermCmd_Reload,
			CMD_RELOAD,
			"Commit/activate pending configuration changes",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    RegisterTermCmd (cmd, cfgEntry);

    // register 'reconnect' command
    cmd = new Term_Cmd( this,
			TermCmd_Reconnect,
			CMD_RECONNECT,
			"Force disconnection/reconnection of a connection",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    cmd->Add(new Term_Cmd_Arg( "num",
			       CVariant(1),
			       CVariantValidator(1, "Connection number")));
    RegisterTermCmd(cmd, cfgEntry);

    // register 'cancel-query' command
    cmd = new Term_Cmd( this,
			TermCmd_CancelQuery,
			CMD_CANCEL_QUERY,
			"Cancel currently running query in a connection",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    cmd->Add(new Term_Cmd_Arg( "num",
			       CVariant(1),
			       CVariantValidator(1, "Connection number")));
    RegisterTermCmd(cmd, cfgEntry);


    // registr 'kill-zombies' command
    cmd = new Term_Cmd( this,
			TermCmd_KillZombies,
			CMD_KILL_ZOMBIES,
			"Kill all zombie connections (may lock system or crash)",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    RegisterTermCmd(cmd, cfgEntry);


    // register 'show' command
    cmd = new Term_Cmd (this,
			TermCmd_Show,
			CMD_SHOW,
			"Show current database activity status",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    RegisterTermCmd (cmd, cfgEntry);

    // register 'connect-string'
    cmd = new Term_Cmd (this,
			TermCmd_ConnectString,
			CMD_CONNECT_STRING,
			"Database connect string",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    cmd->Add (new Term_Cmd_Arg ("cs", CVariant(""), CVariantValidator("", "Database connect string")));
    RegisterTermCmd (cmd, cfgEntry);

    // 'cursor-side'
    cmd = new Term_Cmd (this,
			TermCmd_CursorSide,
			CMD_CURSOR_SIDE,
			"Use server or client side cursor",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    CVariantChoiceValidator sideValidator("client", "Use client-side cursor");
    sideValidator.Add ("server", "Use server-side cursor");
    cmd->Add (new Term_Cmd_Arg("cside", CVariant(""), sideValidator));
    RegisterTermCmd (cmd, cfgEntry);

    // 'cursor type;
    cmd = new Term_Cmd (this,
			TermCmd_CursorType,
			CMD_CURSOR_TYPE,
			"The type of the resultset cursor",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    CVariantChoiceValidator ctValidator("forward-only", "Forward only cursor");
    ctValidator.Add ("dynamic", "Dynamic cursor");
    ctValidator.Add ("static",  "Static cursor");
    ctValidator.Add ("keyset",  "Keyset driven cursor");
    cmd->Add (new Term_Cmd_Arg ("ct", CVariant(""), ctValidator));
    RegisterTermCmd (cmd, cfgEntry);

    // 'driver'
    cmd = new Term_Cmd (this,
			TermCmd_Driver,
			CMD_DRIVER,
			"Database driver type",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    //CVariantChoiceValidator drvValidator("ODBC", "Connect using ODBC driver"); //CBM
    CVariantChoiceValidator drvValidator("FreeTDS", "Connect using FreeTDS driver");
    cmd->Add (new Term_Cmd_Arg ("drv", CVariant(""), drvValidator));
    RegisterTermCmd (cmd, cfgEntry);

    // 'query-timeout'
    cmd = new Term_Cmd (this,
			TermCmd_QueryTimeout,
			CMD_QUERY_TIMEOUT,
			"Query timeout (in seconds)",
			PrivilegeLevelAdmin,
			TermModeConfig,
			true);
    cmd->Add (new Term_Cmd_Arg ("timeout", CVariant(DB_DEFAULT_TIMEOUT), 
				CVariantValidator(DB_DEFAULT_TIMEOUT, "Query timeout value, in seconds"),
				TermArgOptional) );
    RegisterTermCmd (cmd, cfgEntry);

    // 'rowset-size'
    cmd = new Term_Cmd (this,
			TermCmd_RowsetSize,
			CMD_ROWSET_SIZE,
			"Number of rows to retrieve in a single database fetch",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    cmd->Add (new Term_Cmd_Arg("rs", CVariant(0), 
			       CVariantValidator(0, "Rowset size (specify 0 to use default size)")));
    RegisterTermCmd (cmd, cfgEntry);

    // 'password'
    cmd = new Term_Cmd (this,
			TermCmd_Password,
			CMD_PASSWORD,
			"Set password in connection string",
			PrivilegeLevelAdmin,
			TermModeConfig,
			false);
    cmd->Add (new Term_Cmd_Arg("@1", CVariant(""), 
			       CVariantValidator("", "Encrypted password")));
    RegisterTermCmd (cmd, cfgEntry);

    // 'read-only' flag
    cmd = new Term_Cmd (this,
			TermCmd_ReadOnly,
			CMD_READ_ONLY,
			"Set the read-only flag for the database",
			PrivilegeLevelAdmin,
			TermModeConfig,
			true);
    RegisterTermCmd (cmd, cfgEntry);

    // 'statement-cache'
    cmd = new Term_Cmd (this,
			TermCmd_StatementCacheSize,
			CMD_STATEMENT_CACHE,
			"Number of statements in the statement cache",
			PrivilegeLevelAdmin,
			TermModeConfig,
			true);
    cmd->Add (new Term_Cmd_Arg("cache", CVariant(DB_DEFAULT_STMT_CACHE),
			       CVariantRangeValidator( 0, 4096, "Cache size")));
    RegisterTermCmd (cmd, cfgEntry);

    // 'shutdown'
    cmd = new Term_Cmd (this,
			TermCmd_Shutdown,
			CMD_SHUTDOWN,
			"Disable this connection profile",
			PrivilegeLevelAdmin,
			TermModeConfig,
			true);
    RegisterTermCmd (cmd, cfgEntry);
			
    // register show.
    cfgEntry->RegisterShowHandler (this);
}


/** Database connection manager DESTRUCTOR
 */
DB_Connection_Profile::~DB_Connection_Profile()
{
}


/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
 */
bool DB_Connection_Profile::Load()
{
    workingParam = currentParam;
    return ReloadDatabase(true);
}


/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
 */
bool DB_Connection_Profile::Unload()
{
    DestroyDatabases();
    return true;
}


/** This function will be called by the service framework once the 
    configuration file has been fully read.
 */
bool DB_Connection_Profile::CommitConfiguration()
{
    return true;
}


/** Instantiate all database connection
 */
bool DB_Connection_Profile::ReloadDatabase(bool reconnect)
{
    CHECK_SYSTEM_LOCK();
    cout << "*************** Inside ReloadDatabase *********************" << endl;
    // Build ODBC options from current attributes.
    DB_Connection_Option option = workingParam.connectionOption;

    unsigned i;

    // Reconnect existing connection
    unsigned min = (workingParam.connectionCount < m_DatabaseSlot.size()) ? 
		    workingParam.connectionCount : m_DatabaseSlot.size();
    for (i=0; i<min; ++i) {
	DB_Connection_Slot *slot = m_DatabaseSlot[i];
	assert (slot);
	if (!slot) {
	    slot = new DB_Connection_Slot(i, *this);
	    if (!slot->Open()) {
		delete slot;
	    } else
		m_DatabaseSlot[i] = slot;
	} else if (reconnect || !slot->IsActive())
	    slot->Reconnect(false);
    }

    // Create new slots if connection count is larger
    if (m_DatabaseSlot.size() < workingParam.connectionCount) {
	for (i=m_DatabaseSlot.size(); i < workingParam.connectionCount; ++i) {
	    DB_Connection_Slot *slot = new DB_Connection_Slot(i, *this);
	    if (slot->Open())
		m_DatabaseSlot.push_back (slot);
	    else
		delete slot;
	}
    } else if (workingParam.connectionCount < m_DatabaseSlot.size()) {
	// Delete/disconnect slot if connection count is smaller
	for (i=workingParam.connectionCount; i < m_DatabaseSlot.size(); ++i) {
	    DB_Connection_Slot *slot = m_DatabaseSlot[i];
	    assert (slot);
	    if (slot)
		slot->Disconnect();
	}
    }

    return true;
}


/** Close all database connections
 */
void DB_Connection_Profile::DestroyDatabases()
{
    CHECK_SYSTEM_LOCK();

    unsigned i;
    for (i=0; i<m_DatabaseSlot.size(); ++i) {
	DB_Connection_Slot *slot = m_DatabaseSlot[i];
	if (slot) {
	    slot->Destroy();
	    delete slot;
	}
    }
    m_DatabaseSlot.clear();
}


DB_Async_Object *DB_Connection_Profile::GetTaskExclusive(CDbTLS *tls)
{
    DB_Async_Object *ex_obj = NULL;
    for (unsigned i=0; i<MAX_DB_HANDLE; ++i) {
	DB_Async_Object	*as = tls->m_Handle[i];
	if (as && as->IsExclusive()) {
	    ex_obj = as;
	    break;
	}
    }
    return ex_obj;
}

/*
 * Predicate to compare database slot based on the handle count.
 */
struct SlotLeastUsed {
    bool operator () (const DB_Connection_Slot * s1, const DB_Connection_Slot * s2)
    {
	return s1->GetHandlesCount() < s2->GetHandlesCount() ||
	       (s1->GetHandlesCount() == s2->GetHandlesCount() &&
	        s1->GetLastUsage() < s2->GetLastUsage());
    }

};

/** This will create and open a recordset object, using the next database 
    connection in round-robin fashion.
 */
DB_Recordset *DB_Connection_Profile::CreateRecordset(CTask *task, CDbTLS *tls, const char *sql)
{
    CHECK_SYSTEM_LOCK();

    if (isShutdown) {
	CLog::Warning ( DBDRIVER, "[%s] dbOpenRecordset '%s': db profile is shutdown", 
				  Name().c_str(), sql);
	return NULL;
    }

    if (m_DatabaseSlot.empty()) {
	CLog::Warning ( DBDRIVER, "[%s] dbOpenRecordset '%s': db profile has no connection", 
				  Name().c_str(), sql);
	return NULL;
    }

    if (!sql) {
	CLog::Fatal (DBDRIVER, task, "[%s] Null SQL statement in CreateRecordset", Name().c_str());
	return NULL;
    }

    assert(tls && task);
    if (!tls || !task)
	return NULL;

    // CLog::Detail (DBDRIVER, task, "[%s] Create recordset: GetTaskExclusive", Name().c_str());

    DB_Async_Object *ex_obj = GetTaskExclusive(tls);

    // CLog::Detail (DBDRIVER, task, "[%s] Create recordset(2)", Name().c_str());

    /* Sort the database slot based on the least used.
     */
    Vector <DB_Connection_Slot*> sorted = m_DatabaseSlot;
    std::sort(sorted.begin(), sorted.end(), SlotLeastUsed());

    /* Get database connection from connection pool
     */
    DB_Recordset *rst = NULL;
    for (unsigned i=0; i<sorted.size(); ++i) {
	// CLog::Detail (DBDRIVER, task, "[%s] Create recordset: exmining slot %d of %d", Name().c_str(), m_CurrentSlot, sorted.size());

	DB_Connection_Slot *slot = sorted[i];
	if (!slot)
	    continue;

	rst = slot->CreateRecordset(task, ex_obj!=NULL, sql);
	if (rst)
	    return rst;
    }

    CLog::Warning (DBDRIVER, task,
			     "[%s] dbOpenRecordset '%s': db connections "
			     "are not connected or locked by other tasks",
			     Name().c_str(),
			     sql);
    return NULL;
}

/** This will create and open an executor object, using the next database 
    connection in round-robin fashion.
 */
DB_SQL_Exec *DB_Connection_Profile::CreateSQLExec(CTask *task, CDbTLS *tls, const char *sql)
{
    CHECK_SYSTEM_LOCK();

    if (isShutdown) {
	CLog::Warning ( DBDRIVER, "[%s] dbExecute '%s': db profile is shutdown", 
				  sql,
				  Name().c_str());
	return NULL;
    }

    if (m_DatabaseSlot.empty()) {
	CLog::Warning ( DBDRIVER, "[%s] dbExecute '%s': db profile has no connection", 
				  sql,
				  Name().c_str());
	return NULL;
    }

    DB_Async_Object *ex_obj = GetTaskExclusive(tls);

    /* Sort the database slot based on the least used.
     */
    Vector <DB_Connection_Slot*> sorted = m_DatabaseSlot;
    std::sort(sorted.begin(), sorted.end(), SlotLeastUsed());

    /* Get database connection from connection pool
     */
    DB_SQL_Exec *exec = NULL;
    for (unsigned i=0; i<sorted.size(); ++i) {
	DB_Connection_Slot *slot = sorted[i];
	if (!slot)
	    continue;

	exec = slot->CreateSQLExec (task, ex_obj!=NULL, sql);
	if (exec)
	    return exec;
    }

    CLog::Warning (DBDRIVER, task,
			     "[%s] dbExecute '%s': db connections "
			     "are not connected or locked by other tasks",
			     Name().c_str(),
			     sql);
    return NULL;
}


int DB_Connection_Profile::Checkdatabase_connectivity(string connect_string){

    //cout << "******* connect_string is "  << connect_string << endl;
    istringstream iss(connect_string);
    string token = "";
    string db_server_ip = "";
    getline(iss, token, ';');
    //cout << token << endl;
    getline(iss, token, ';');
    //cout << token << endl;
    istringstream iss1(token);
    getline(iss1, db_server_ip, '=');
    //cout << hostname << endl;
    getline(iss1, db_server_ip, '=');
//    cout << "db_server_ip length " << db_server_ip.length() << endl;
   // cout << hostname << endl;
  //  cout << "********************* hostname is " << hostname << endl;
    this->db_server_ip = db_server_ip;
    int sockfd = -1;
    int portno = 1433;
    struct sockaddr_in serv_addr;
    int res, flags, n;


    if(sockfd == -1){
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            //  error("ERROR opening socket");
   //         cout << "******* Error opening socket *********" << endl;
            return 1;
        }
    }
    flags = fcntl(sockfd, F_GETFL, 0);
    if(fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)  == -1){
            cout << "Unable to set non blocking socket " << endl;
    }

//    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(db_server_ip.c_str());
    serv_addr.sin_port = htons(portno);

    if (n = connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
            //   printf("Port is closed");
            if (errno != EINPROGRESS){
                close(sockfd);
                return (-1);
            }else if(errno == EINPROGRESS){
              //  cout << "errno is " << errno << endl;
    //           cout << "The server may be still reachable" << endl;
                fd_set rset, wset;
                FD_ZERO(&rset);
                FD_SET(sockfd, &rset);
                wset = rset;
                timeval tv={0,5000};
              //  tval.tv_sec = 5;
              //  tval.tv_usec = 0;
                if ( (n = select(sockfd + 1, &rset, &wset, NULL,&tv)) == 0) {
                    close(sockfd);
                    errno = ETIMEDOUT;
                    //cout << "socket is not reachable or writable" << endl;
                    //cout << "db server "<<  inet_ntoa(serv_addr.sin_addr) << " is not reachable" << endl;
                    CLog::Fatal(DBDRIVER, "********* db_server %s is not reachable ********", inet_ntoa(serv_addr.sin_addr));
                    close(sockfd);
                    return (-1);
                }

                if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
                 //   cout << "socket is reachable or writable" << endl;
                    close(sockfd);
                    return 0;
                }
            }

        } else if(n == 0){
 //           cout << "The server" << server->h_name  << " and the port " << portno << " is reachable" << endl;
        //    cout << "***** db server is connecting  *********" << endl;
            close(sockfd);
            return 0;
        }

}



/** Poll databases
 */
const int db_not_reachable_max_count = 3;

void DB_Connection_Profile::HandleEvents()
{

handle_event_count++;

#if 1
    if (m_DatabaseSlot.empty())
	return;
   // cout << "***********  Inside db_connection handleevents *******" << endl;

#if 1

if((handle_event_count == 300)){
    handle_event_count = 0;
    string connect_string;
    int ret;
    connect_string = GetConnectString();

//    cout <<"************** The connect string for the database is " <<  connect_string << endl;
    ret = Checkdatabase_connectivity(connect_string);
#if 1
    if(ret == -1){
       
        db_profile_not_reachable_count++; 
        if(db_profile_not_reachable_count == db_not_reachable_max_count){  // if the database is not reachable continuoulsy for  5 times,reloading the database
            if(can_reload_db_profile){
                
                CLog::Fatal(DBDRIVER, "********* Reloading database profile as dabase server  %s is not reachable ********", db_server_ip.c_str());
                ReloadDatabase(true);
                
                can_reload_db_profile = false;
        //        return;
            }
            db_profile_not_reachable_count = 0; 
            
        }
     }else{ 
     
        if(db_profile_not_reachable_count > 0)
            db_profile_not_reachable_count--; 
         if(db_profile_not_reachable_count == 0)
                can_reload_db_profile = true;
     }
#endif 
}
#endif 
        Vector <DB_Connection_Slot*>::iterator it;
        unsigned i, connection_count;

        connection_count = isShutdown ? 0 : workingParam.connectionCount;

        for (i=0, it=m_DatabaseSlot.begin(); it != m_DatabaseSlot.end(); ++i) {
        	DB_Connection_Slot *slot = *it;
        	if (!slot) {
        	    ++it;
    	    continue;
    	}
        
    	slot->HandleEvents();
    	if (i >= connection_count) {
	        if (slot->CanBeDestroyed()) {
        		slot->Destroy();
        		delete slot;
		        *it = NULL;
		        it = m_DatabaseSlot.erase(it);
	        } else {
        		slot->SetClosing();
        		++it;
	        }
	    }else
	       ++it;
        }

    
#else
    

#endif 
}


/** Determine whether the database is ok to delete.
 */
bool DB_Connection_Profile::CanBeDeleted()
{
    if (m_DatabaseSlot.size()) {
	CLog::Info (DBDRIVER, "Can't delete profile '%s' because connection is not empty", Name().c_str());
    }
    return m_DatabaseSlot.empty();
}


bool DB_Connection_Profile::OnShowStatus (String & output)
{
    char s[128];

    sprintf (s, SHOW_STATUS_INDENT "Database profile %s, ", Name().c_str());
    output += s;
    if (isShutdown)
	output += "status is shutdown\n";
    else if (m_DatabaseSlot.size())
	output += "status is running\n";
    else
	output += "status is not running\n";

    // Connection count
    sprintf(s, SHOW_STATUS_INDENT "Connection count: %d\n", m_DatabaseSlot.size());
    output += s;

    // Connect-string, but replace password
    output += SHOW_STATUS_INDENT;
    String cs = SafeConnectString(workingParam.connectString);
    output += cs;
    output += "\n";

    output += SHOW_STATUS_INDENT;
    output += "Cursor: ";
    output += currentParam.connectionOption.GetCursorSideString();
    output += ", ";
    output += currentParam.connectionOption.GetCursorTypeString();
    output += "\n";

    if (workingParam == currentParam)
	;
    else {
	output += SHOW_STATUS_INDENT "(Some configuration changes haven't been committed)\n";
    }

    output += "\n" SHOW_STATUS_INDENT  "    Status\tConnection information\n";
    output += SHOW_STATUS_INDENT;
    output.append( 75, '=');
    output.append(1, '\n');

    CHECK_SYSTEM_LOCK();
    
    for (unsigned i=0; i<m_DatabaseSlot.size(); ++i) {
	DB_Connection_Slot *slot = m_DatabaseSlot[i];
	if (!slot) continue;
	
	sprintf( s, SHOW_STATUS_INDENT "%3u ", i);
	output += s;

	String temp;
	slot->Show(temp);
	output += temp;
	output += "\n";
    }

    return true;
}


/** Terminal command callback
 */
void DB_Connection_Profile::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant p1;
    String temp1;

    switch (cmd->GetId()) {
    case TermCmd_ConnectionCount: 
	ACQUIRE_SYSTEM_LOCK();
	cmd->GetArg((size_t)0)->GetValue(p1);
	currentParam.connectionCount = p1.AsInt();
	RELEASE_SYSTEM_LOCK();
	break;
    case TermCmd_ConnectString:
	ACQUIRE_SYSTEM_LOCK();
	cmd->GetArg((size_t)0)->GetValue(p1);
	currentParam.connectString = p1.ToString();
	RELEASE_SYSTEM_LOCK();
	break;
    case TermCmd_CursorSide:
	ACQUIRE_SYSTEM_LOCK();
	cmd->GetArg((size_t)0)->GetValue(p1);
	if (DB_Connection_Option::GetCursorSide(p1.AsString()) != DB_Connection_Option::UNKNOWN_SIDE) {
	    currentParam.connectionOption.m_CursorSide = DB_Connection_Option::GetCursorSide(p1.AsString());
	}
	RELEASE_SYSTEM_LOCK();
	break;
    case TermCmd_CursorType:
	ACQUIRE_SYSTEM_LOCK();
	cmd->GetArg((size_t)0)->GetValue(p1);
	if (DB_Connection_Option::GetCursorType(p1.AsString()) != DB_Connection_Option::UNKNOWN) {
	    currentParam.connectionOption.m_CursorType = DB_Connection_Option::GetCursorType(p1.AsString());
	}
	RELEASE_SYSTEM_LOCK();
	break;
    case TermCmd_Driver:
	ACQUIRE_SYSTEM_LOCK();
	cmd->GetArg((size_t)0)->GetValue(p1);
	currentParam.dbDriver = p1.AsString();
	RELEASE_SYSTEM_LOCK();
	break;
    case TermCmd_QueryTimeout:
	ACQUIRE_SYSTEM_LOCK();
	if (no) {
	    currentParam.connectionOption.m_QueryTimeout = -1;
	} else if (cmd->GetArg((size_t)0)->HasValue()) {
	    cmd->GetArg((size_t)0)->GetValue(p1);
	    currentParam.connectionOption.m_QueryTimeout = p1.AsInt();
	} else {
	    RELEASE_SYSTEM_LOCK();
	    session->SendErrorf("Value is required (value is in seconds)\n.");
	    return;
	}
	RELEASE_SYSTEM_LOCK();
	break;
    case TermCmd_RowsetSize:
	ACQUIRE_SYSTEM_LOCK();
	cmd->GetArg((size_t)0)->GetValue(p1);
	currentParam.connectionOption.m_RowsetSize = p1.AsInt();
	RELEASE_SYSTEM_LOCK();
	break;
    case TermCmd_ReadOnly:
	ACQUIRE_SYSTEM_LOCK();
	currentParam.connectionOption.m_ReadOnly = !no;
	RELEASE_SYSTEM_LOCK();
	break;
    case TermCmd_StatementCacheSize:
	ACQUIRE_SYSTEM_LOCK();
	cmd->GetArg((size_t)0)->GetValue(p1);
	currentParam.connectionOption.m_StmtCacheSize = p1.AsInt();
	RELEASE_SYSTEM_LOCK();
	break;
    case TermCmd_Reload:
	if (isShutdown) {
	    session->SendErrorf ("Profile is shutdown. Use 'no shutdown' command instead.\n");
	} else {
	    ACQUIRE_SYSTEM_LOCK();
	    /* Check whether existing connections needs to be reconnected.
	     */
	    bool reconnect = (workingParam.connectString != currentParam.connectString ||
			      !(workingParam.connectionOption == currentParam.connectionOption));
	    workingParam = currentParam;
	    ReloadDatabase(reconnect);
	    RELEASE_SYSTEM_LOCK();
	}
	break;
    case TermCmd_Reconnect:
	if (isShutdown) {
	    session->SendErrorf("Database profile is shutdown.\n");
	    
	} else {
	    cmd->GetArg((unsigned)0)->GetValue(p1);
	    int num = p1.AsInt();
	    if (num < 0 || num >= m_DatabaseSlot.size()) {
		session->SendErrorf("Invalid connection number.\n");
		return;
	    }
	    ACQUIRE_SYSTEM_LOCK();
	    DB_Connection_Slot *slot = m_DatabaseSlot[num];
	    if (slot)
		slot->Reconnect(true);
	    RELEASE_SYSTEM_LOCK();
	}
	break;
    case TermCmd_CancelQuery:
	if (isShutdown) {
	    session->SendErrorf("Database profile is shutdown.\n");
	    
	} else {
	    cmd->GetArg((unsigned)0)->GetValue(p1);
	    int num = p1.AsInt();
	    if (num < 0 || num >= m_DatabaseSlot.size()) {
		session->SendErrorf("Invalid connection number.\n");
		return;
	    }
	    ACQUIRE_SYSTEM_LOCK();
	    DB_Connection_Slot *slot = m_DatabaseSlot[num];
	    if (slot)
		slot->CancelQuery();
	    RELEASE_SYSTEM_LOCK();
	}
	break;
    case TermCmd_Password:
	cmd->GetArg((size_t)0)->GetValue(p1);

	if (strlen(p1.AsString()) < DB_MAX_PWD_LEN+DB_RANDOM_CIPHER_LEN) {
	    CLog::Error(DBDRIVER, "Invalid password stream. Encrypted password required");
	    session->SendErrorf("Invalid password stream. Encrypted password required\n");
	    return;
	}

	ACQUIRE_SYSTEM_LOCK();
	do {
	    //
	    // Decrypt password and put it in the connect string.
	    //
	    // Steps:
	    //	- decode from the base64 representation.
	    //	- get the first DB_RANDOM_CIPHER_LEN from the decoded stream.
	    //	  This will be the secret prefix. Concat dbSecret to this secret
	    //	  prefix to get the whole secret.
	    //	- Decrypt the remaining stream using the secret.
	    //

	    // Convert the base64 representation to binary (encrypted password)
	    //unsigned int len;
	    unsigned long len;
	    unsigned char *pwd_rc4 = base64_decode( p1.AsString(), strlen(p1.AsString()), &len);

	    // Init secret from the hash.
	    String secret;
	    for (int i=0; i<DB_RANDOM_CIPHER_LEN; ++i) {
		secret.append(1, *(pwd_rc4+i));
	    }
	    secret.append((char*)dbSecret);

	    // Decode Rc4 password.
	    char pwd[DB_MAX_PWD_LEN];
	    MARC4 pwd_encryptor((byte*)secret.c_str(), secret.size()+1, 256);
	    pwd_encryptor.ProcessString((byte*)pwd, pwd_rc4+DB_RANDOM_CIPHER_LEN, len-DB_RANDOM_CIPHER_LEN);

	    free(pwd_rc4);

	    // Replace the password in connect-string with this password.
	    currentParam.connectString = ReplaceConnectStringPassword( currentParam.connectString, pwd);

	} while (0);
	RELEASE_SYSTEM_LOCK();
	break;
    case TermCmd_Show:
	ACQUIRE_SYSTEM_LOCK();
	OnShowStatus(temp1);
	RELEASE_SYSTEM_LOCK();
	session->Send(temp1);
	break;

    case TermCmd_KillZombies:
	{
	    ACQUIRE_SYSTEM_LOCK();
	    for (int i=0; i<m_DatabaseSlot.size(); ++i) {
		DB_Connection_Slot *slot = m_DatabaseSlot[i];
		slot->KillZombies();
	    }
	    RELEASE_SYSTEM_LOCK();
	    break;
	}
	break;

    case TermCmd_Shutdown:
	if (!no) {
	    ACQUIRE_SYSTEM_LOCK();
	    isShutdown = true;
	    RELEASE_SYSTEM_LOCK();
	} else {
	    if (isShutdown) {
		ACQUIRE_SYSTEM_LOCK();
		workingParam = currentParam;
		ReloadDatabase(true);
		isShutdown = false;
		RELEASE_SYSTEM_LOCK();
	    } else
		session->SendErrorf ("Database is already running\n");
	}
	break;
    }
}

/** Show configuration for the specified entry.
    This function will be called by the framework whenever it needs to display the
    specified entry. Previously this instance should tell the entry that it wishes to
    handle displaying the entry by calling entry->RegisterShowHandler().
 */
STATUS DB_Connection_Profile::OnShowConfig_NoLock( const Cfg_Tree_Entry *entry,
						   String & output, 
						   const String & indent, 
						   int indent_size) const
{
    // Copy attributes to temporary variables while holding the system lock.
    unsigned connection_count = currentParam.connectionCount;
    String connect_string = currentParam.connectString;
    String driver = currentParam.dbDriver;
    DB_Connection_Option db_option = currentParam.connectionOption;

    char temp[256];
    const String crlf(NEWLINE);

    // connection count.
    sprintf(temp, CMD_CONNECTION_COUNT " %u" NEWLINE, connection_count);
    output += indent + temp;

    // connect-string.
    // Hide the password from the connect string, and display the password
    // separately with the 'password' command.
    output += indent + CMD_CONNECT_STRING + " \"" +
	      SafeConnectString(connect_string) + "\"" NEWLINE;

    // cursor-side.
    sprintf(temp, CMD_CURSOR_SIDE " %s" NEWLINE, db_option.GetCursorSideString());
    output += indent + temp;

    // cursor-type.
    sprintf(temp, CMD_CURSOR_TYPE " %s" NEWLINE, db_option.GetCursorTypeString());
    output += indent + temp;

    // driver.
    output += indent + CMD_DRIVER + " " + driver + crlf;

    // password.
    // Steps:
    //	- extract password from connect string.
    //	- Generate 2 bytes random character as the prefix to
    //	  the secret.
    //  - Generate the secret by adding the 2 bytes random char
    //	  with dbSecret.
    //	- encrypt with RC4 using the secret.
    //	- transform to base64 representation.
    //

    // extract password from the connect string
    //
    int pwd_len;
    const char *pwd_start;
    pwd_start = password_from_connect_string( connect_string, &pwd_len);
    if (!pwd_start) {
	pwd_start = "";
	pwd_len = 0;
    }
    //
    // Generate 2 bytes random string for secret, and append dbSecret
    // to this string.
    //
    String secret;
    for (int i=0; i<DB_RANDOM_CIPHER_LEN; ++i) {
	secret.append(1, (char)(1+rand()%254));
    }
    secret.append((char*)dbSecret);
    //
    // encrypt password with RC4:
    //
    byte pwd[DB_MAX_PWD_LEN];
    byte pwd_rc4[DB_MAX_PWD_LEN];

    memset(pwd, 0, sizeof(pwd));
    if (pwd_len) memcpy(pwd, pwd_start, pwd_len);
    pwd[pwd_len] = '\0';

    MARC4 pwd_encryptor((byte*)secret.c_str(), secret.size()+1, 256);
    pwd_encryptor.ProcessString(pwd_rc4, pwd, DB_MAX_PWD_LEN);
    //
    // Concatenate random secret with the encrypted password.
    //
    byte encrypted[DB_MAX_PWD_LEN+DB_RANDOM_CIPHER_LEN];
    memcpy(encrypted, secret.c_str(), DB_RANDOM_CIPHER_LEN);
    memcpy(encrypted+DB_RANDOM_CIPHER_LEN, pwd_rc4, DB_MAX_PWD_LEN);
    //
    // encode with base64
    //
    char *pwd_rc4_b64 = base64_encode(encrypted, DB_MAX_PWD_LEN+DB_RANDOM_CIPHER_LEN);
    // show
    output += indent + CMD_PASSWORD + " " + pwd_rc4_b64 + crlf;
    ::free(pwd_rc4_b64);

    // query timeout
    if (db_option.m_QueryTimeout > 1) {
	sprintf(temp, CMD_QUERY_TIMEOUT " %d" NEWLINE, db_option.m_QueryTimeout);
	output += indent + temp;
    } else
	output += indent + "no " CMD_QUERY_TIMEOUT "\n";

    // rowset-size
    sprintf(temp, CMD_ROWSET_SIZE " %u" NEWLINE, db_option.m_RowsetSize);
    output += indent + temp;

    // readonly.
    if (db_option.m_ReadOnly)
	output += indent + CMD_READ_ONLY NEWLINE;
    else
	output += indent + "no " CMD_READ_ONLY NEWLINE;

    // statement-cache
    sprintf(temp, CMD_STATEMENT_CACHE " %u" NEWLINE, db_option.m_StmtCacheSize);
    output += indent + temp;

    // shutdown.
    if (isShutdown)
	output += indent + CMD_SHUTDOWN + NEWLINE;
    else
	output += indent + "no " CMD_SHUTDOWN NEWLINE;

    return OK;
}


bool DB_Connection_Profile::OnShowConfig( const Cfg_Tree_Entry *entry,
					    String & output, 
					    const String & indent, 
					    int indent_size) const
{
    // Copy attributes to temporary variables while holding the system lock.
    ACQUIRE_SYSTEM_LOCK();
    OnShowConfig_NoLock (entry, output, indent, indent_size);
    RELEASE_SYSTEM_LOCK();
    return OK;
}

//-----------------------------------------------------------------------------
// CTOR
DB_Service::DB_Service()
: ServiceProvider( DBDRIVER, true, false), m_TLS(-1), m_API(NULL), m_OpenedHandle(0),
  m_RootConfig(NULL)
{
    // Initialize secret
    // !! Changing the secret will make the hashed password incompatible !!
    int secret[] = { 'd', 'b', 'D', 'r', 'i', 'v', 'e', 'r', '0', '1', 0};
    int *s = secret;
    unsigned char *p = dbSecret;
    while ((*p++ = (unsigned char)*s++) != 0)
	;

    srand(time(NULL));
}


/** Destructor.
 */
DB_Service::~DB_Service()
{
    dbSvcInstance = NULL;
}

/** Create the singleton instance of this class.
 */
DB_Service * DB_Service::Create()
{
    if (dbSvcInstance == NULL)
	dbSvcInstance = new DB_Service;
    return dbSvcInstance;
}

//-----------------------------------------------------------------------------
// Creating database objects based on configurations
bool DB_Service::Load()
{
    cout << " ***** Inside DB_Service Load ****************" << endl;
    Term_Cmd *cmd = NULL;
    /* CLog::Detail( DBDRIVER, "Initializing database service manager"); */
    //printf("Initializing database service manager");

    // create root config.
    NEW_RETURN_ (m_RootConfig, Cfg_Tree_Group("database", "Database configuration", Kernel::Instance()->RootConfig()));
    if (m_RootConfig == NULL)
	return false;

    // get connection count
    int connect_cnt = DB_DEFAULT_CONNECTION_CNT;
    
    // load each database configuration

    // Get TLS value
    m_TLS = Kernel::Instance()->AllocTLS();

    // create API
    NEW_RETURN_ (m_API, DB_API( this, DB_PROVIDER_ID));
    if (m_API == NULL)
	goto on_error;
    if (m_API->Init() == NOT_OK) {
	goto on_error;
    }

    // create command to create database.
    cmd = new Term_Cmd(this, 
			       TermCmd_Database, 
			       CMD_DATABASE,
			       "Configure/create a database connection profile",
			       PrivilegeLevelAdmin,
			       TermModeConfig,
			       true);
    cmd->Add (new Term_Cmd_Arg("name", CVariant(""), CVariantValidator("", "Database profile name")));
    RegisterTermCmd (cmd, Kernel::Instance()->RootConfig());

    // add show handler
    Kernel::Instance()->RootConfig()->RegisterShowHandler (this);

    // Done
    /* CLog::Debug( DBDRIVER, "Database service manager initialized"); */
    //printf("Database service manager initialized");
    return true;


on_error:
    delete m_API;
    Kernel::Instance()->FreeTLS(m_TLS);
    delete m_RootConfig;
    m_RootConfig = NULL;
    /* CLog::Debug( DBDRIVER, "Unable to initialize database service manager"); */
    //printf("Unable to initialize database service manager");
    return false;
}


//-----------------------------------------------------------------------------
// Shutdown: destroy all database objects
bool DB_Service::Unload()
{
    /* CLog::Detail( DBDRIVER, "Shutting down database service"); */
    //printf("Shutting down database service");

    // unregister show database handler.
    Kernel::Instance()->RootConfig()->UnregisterShowHandler (this);

    // destroy database objects
    /* CLog::Detail( DBDRIVER, "Closing database connections"); */
    //printf("Closing database connections");
    while ( !m_DatabaseProfiles.empty() ) {
	DB_Connection_Profile *profile = m_DatabaseProfiles.back();
	
	profile->DestroyDatabases();
	delete profile;

	m_DatabaseProfiles.pop_back();
    }

    // shutdown API
    m_API->Shutdown();
    delete m_API;
    m_API = NULL;

    // free TLS
    Kernel::Instance()->FreeTLS(m_TLS);

    /* CLog::Detail( DBDRIVER, "Database service has been shutdown"); */
    //printf("Database service has been shutdown\n");
    return OK;
}


// Profile helpers.
DB_Connection_Profile * DB_Service::FindProfile(const char * name)
{
    List <DB_Connection_Profile*>::iterator it, end;
    end = m_DatabaseProfiles.end();
    for (it = m_DatabaseProfiles.begin(); it!=end; ++it) {
	DB_Connection_Profile *profile = *it;
	if (!strcasecmp(profile->Name().c_str(), name))
	    return profile;
    }
    return NULL;
}

//-----------------------------------------------------------------------------
// Internal: get the TLS object
CDbTLS *DB_Service::GetTLS(CTask *task)
{
    CDbTLS *tls = (CDbTLS*) task->GetTLSValue( m_TLS );
    if (!tls) {
	/* CLog::Error( DBDRIVER, task, "Task doesn't have database connectoins");
 */
        //printf("Task doesn't have database connectoins\n");
	task->SetLastError(-1);
	return 0;
    }

    return tls;
}


/** Terminal command callback
 */
void DB_Service::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    CVariant p1;

    switch (cmd->GetId()) {
    case TermCmd_Database:
	if (!no) {
	    cmd->GetArg ((size_t)0)->GetValue(p1);
	    const char* db_name = p1.AsString();

	    DB_Connection_Profile *profile = NULL;
	    ACQUIRE_SYSTEM_LOCK();
	    profile = FindProfile(db_name);
	    RELEASE_SYSTEM_LOCK();
	    if (profile == NULL) {
		if (session->GetStream().IsInteractive()) {
		    if (session->PromptYesNo(false, "Create a new database connection profile '%s' (y/n) ? ",
					     db_name) == false) {
			return;
		    }
		}
		NEW_RETURN_ (profile, DB_Connection_Profile(db_name));
		if (profile == NULL) {
		    session->Sendf ("Unable to create a new database connection profile" NEWLINE);
		    return;
		}

		ACQUIRE_SYSTEM_LOCK();
		if (FindProfile(db_name) == NULL)
		    m_DatabaseProfiles.push_back(profile);
		else {
		    delete profile;
		    session->SendErrorf ("Duplicate profile name" NEWLINE);
		    RELEASE_SYSTEM_LOCK();
		    return;
		}

		/*
		if (profile->Load() == false) {
		    session->SendErrorf ("Error in loading the profile" NEWLINE);
		    m_DatabaseProfiles.remove(profile);
		    delete profile;
		    RELEASE_SYSTEM_LOCK();
		    return;
		}
		*/
		RELEASE_SYSTEM_LOCK();

		if (session->GetStream().IsInteractive())
		    session->Sendf ("New database profile '%s' created" NEWLINE, db_name);
	    }
	    session->SetRoot (profile->GetConfig());

	} else {
	    cmd->GetArg ((size_t)0)->GetValue(p1);
	    const char* db_name = p1.AsString();

	    ACQUIRE_SYSTEM_LOCK();
	    DB_Connection_Profile *profile = FindProfile(db_name);
	    RELEASE_SYSTEM_LOCK();

	    if (profile == NULL) {
		session->SendErrorf ("Database profile '%s' not found" NEWLINE, db_name);
		return;
	    }

	    if (session->GetStream().IsInteractive()) {
		if (session->PromptYesNo(false, "Delete database connection profile '%s' (y/n) ? ",
					     db_name) == false) {
			return;
		}
	    }

	    ACQUIRE_SYSTEM_LOCK();
	    profile = FindProfile(db_name);
	    if (profile->CanBeDeleted()) {
		m_DatabaseProfiles.remove( profile );
		profile->Unload();
		delete profile;
		profile = NULL;
	    }
	    RELEASE_SYSTEM_LOCK();
	    if (profile != NULL)
		session->SendErrorf ("Unable to delete database profile" NEWLINE);
	}
	break;
    }

}

/** Show configuration for the specified entry.
    This function will be called by the framework whenever it needs to display the
    specified entry. Previously this instance should tell the entry that it wishes to
    handle displaying the entry by calling entry->RegisterShowHandler().
 */
bool DB_Service::OnShowConfig( const Cfg_Tree_Entry *entry,
				 String & output, 
				 const String & indent, 
				 int indent_size) const
{
    String new_indent(indent);
    new_indent.append((size_t)indent_size, ' ');
    const String crlf(NEWLINE);
 //   pthread_mutex_t db_lock;
//#ifdef __linux__
    ACQUIRE_SYSTEM_LOCK();
/*#else
    pthread_mutex_lock(&db_lock);
#endif*/
    List <DB_Connection_Profile*>::const_iterator it, end;
    end = m_DatabaseProfiles.end();
    for (it = m_DatabaseProfiles.begin(); it!=end; ++it) {
	DB_Connection_Profile *profile = *it;
	output += indent + CMD_DATABASE + " " + profile->Name() + crlf;
	profile->OnShowConfig_NoLock (entry, output, new_indent, indent_size);
	output += new_indent + "top" NEWLINE;
	output += NEWLINE;
    }
//#ifdef __linux__
    RELEASE_SYSTEM_LOCK();
/*#else
    pthread_mutex_unlock(&db_lock);
#endif*/
    return OK;
}

/** Show opened database handles for the specified task.
 */
void DB_Service::ShowDbHandles_NoLock(String & output, CTask *task)
{
    CDbTLS *tls = GetTLS(task);
    if (tls==NULL) {
	output = "Task doesn't have database connection.\n";
	return;
    }

    char temp[80];
    output = "Database handles currently opened:\n"
	     "================================================================================\n";
    unsigned i, count;
    for (i=1, count=0; i<MAX_DB_HANDLE; ++i) {
	if (tls->m_Handle[i]) {
	    ++count;
	    int printed = _snprintf(temp, 78, "%2d [%s]%s", 
				    count, tls->m_Handle[i]->GetParentDb()->GetProfile()->Name().c_str(),
				    tls->m_Handle[i]->GetSQL());
	    temp[78] = '\0';
	    output += temp;
	    if (printed < 0)
		output += "..\n";
	    else
		output += "\n";
	}
    }
}


//-----------------------------------------------------------------------------
// Internal: is handle valid?
bool DB_Service::IsHandleValid(CTask *task, int handle, const char *cmd)
{
    if (handle<0 || handle >= MAX_DB_HANDLE) {
	/* CLog::Error( DBDRIVER, task, "Invalid handle %d in %s", handle, cmd); */
        //printf("Invalid handle %d in %s\n", handle, cmd);
	task->SetLastError(-1);
	return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
// Internal: Get DB_Async_Object based on the handle
DB_Async_Object *DB_Service::GetRstObject( CTask *task, int handle, const char *cmd)
{
    CDbTLS *tls = GetTLS(task);
    if (tls==NULL) {
	/* CLog::Error( DBDRIVER, task, "Task doesn't have database connection (%s)", cmd);
        */
        //printf("Task doesn't have database connection (%s)\n", cmd);
	return NULL;
    }


    if (!IsHandleValid( task, handle, cmd) )
	return 0;

    if (handle == 0 || tls->m_Handle[handle]==0) {
	/* CLog::Error( DBDRIVER, task, "Invalid handle %d in %s", handle, cmd);
         */
        //printf("Invalid handle %d in %s\n", handle, cmd);
	task->SetLastError(-1);
	return 0;
    }

    return tls->m_Handle[handle];
}

//-----------------------------------------------------------------------------
// This is internal function to check the sanity of the recordset 
// before data is accessed by task.
bool DB_Service::CheckBeforeAccessData(CTask *task, DB_Recordset *rst, 
				       const char *api)
{
    if (rst->IsBOF()) {
	/* CLog::Warning( DBDRIVER, task, "%s: attempt to access data beyond BOF (sql='%s')",
		     api, rst->GetSQL());
         */
        //printf("%s: attempt to access data beyond BOF (sql='%s')", api, rst->GetSQL());
	return false;
    }
    if (rst->IsEOF()) {
	/* CLog::Warning( DBDRIVER, task, "%s: attempt to access data beyond EOF (sql='%s')",
		     api, rst->GetSQL());
         */
         //printf("%s: attempt to access data beyond EOF (sql='%s')\n", api, rst->GetSQL());
	return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
// Attaching task
bool DB_Service::AttachTask(CTask *task)
{
    //***CLog::Detail( DBDRIVER, task, "OnTaskAttached");

    if (m_DatabaseProfiles.empty()) {
	task->SetTLSValue( m_TLS, 0);
	return true;
    }

    CDbTLS *tls;
    NEW_RETURN_ (tls, CDbTLS);
    if (tls == NULL)
	return false;

	#ifdef __i386__
    task->SetTLSValue( m_TLS, (int)tls);
	#elif __x86_64__
    task->SetTLSValue( m_TLS, tls);

	#endif

    // assign default database to the task
    /** TODO:
    if (SetDatabase(task, m_DefaultDb.c_str())!=OK || tls->m_Database == 0) {
	delete tls;
	task->SetTLSValue( m_TLS, 0);
	return true;
    }
    */

    /* Add statistics
     */
    m_OpenedHandle++;

    return true;
}


//-----------------------------------------------------------------------------
// Detaching task
//
void DB_Service::DetachTask(CTask *task)
{
    CDbTLS *tls = (CDbTLS*)task->GetTLSValue(m_TLS);
    if (!tls)
	return;

    // Wakeup task first, if necessary
    if (tls->m_Current != -1 && task->GetState()==CTask::BLOCKED)
	task->Wakeup ();


    // close all open objects in the TLS
    for (int i=0; i<MAX_DB_HANDLE; i++) {
	DB_Async_Object *obj = tls->m_Handle[i];

	if (obj) {

	    obj->Close();
	    if (i!=0) {
		/* CLog::Debug( DBDRIVER, task, "handle %d is not closed when "
					 "task is detached", i);
                 */
	    } 
	    tls->m_Handle[i] = 0;

	    // update statistics
	    --m_OpenedHandle;
	}
    }

    // destroy the TLS
    delete tls;
    task->SetTLSValue( m_TLS, 0);

}


//-----------------------------------------------------------------------------
// Cancelling task's current job
bool DB_Service::CancelJob(CTask *task)
{
    /* CLog::Detail( DBDRIVER, task, "CancelJob"); */
    //printf("CancelJob\n");

    DB_Async_Object *obj = GetCurrentDbObject(task);
    if (!obj) {
	/* CLog::Fatal( DBDRIVER, task, "OnTaskDetached: No current job"); */
        //printf("OnTaskDetached: No current job\n");
	return false;
    }

    if (obj->CancelExecuting()) {
	return false;
    }

    return true;
}


//-----------------------------------------------------------------------------
// Convenience function: set task's last error and display error to CLog
void DB_Service::DBERROR( const char *title, CTask *task, DB_Async_Object *rst )
{
    task->SetLastError(-1);
}


//-----------------------------------------------------------------------------
// Wakeup task (called by DB_Async_Object::OnAsyncEnd())
STATUS DB_Service::WakeupTask( CTask *task)
{
    if (task == NULL)
	return OK;

    // check the TLS
    CDbTLS *tls=GetTLS(task);
    if (!tls)
	return NOT_OK;

    // check current handle
    if (tls->m_Current == -1) {
	/* CLog::Fatal( DBDRIVER, task, "Task doesn't have current handle when woken up");
         */
        //printf("Task doesn't have current handle when woken up\n");
	task->Wakeup();
	return NOT_OK;
    }

    // handle error
    if (tls->m_Handle[tls->m_Current]) {

	DB_Async_Object *cur_obj = tls->m_Handle[tls->m_Current];

	if (cur_obj->GetState() == DB_Async_Object::XSTATE_ERROR) {

	    DBERROR( "Error", task, cur_obj);

	    // tls->m_Ret is the indication that this is API_DbOpen
	    if (tls->m_Ret) {
		*tls->m_Ret = -1;
		ReleaseHandle( task, tls->m_Current);
	    }

	} else {
	    task->SetLastError(0);
	}

    } else {
	task->SetLastError(0);
    }

    // wake up the task
    task->Wakeup();

    tls->m_Current = -1;
    tls->m_Ret = 0;
    return OK;
}

//-----------------------------------------------------------------------------
// Internal: allocate a new handle
int DB_Service::AllocHandle(CTask *task, const char *sql)
{
    // get the TLS object
    CDbTLS *tls = GetTLS(task);
    if (!tls)
	return -1;
    
    // find empty handle (note: handle 0 is reserved for executor).
    for (int handle=1; handle<MAX_DB_HANDLE; handle++) {
	if (tls->m_Handle[handle] == 0) {

	    // update statistics
	    m_OpenedHandle++;
	    task->SetLastError(0);
	    return handle;
	}
    }

    // ouch.. all used
    /* CLog::Error( DBDRIVER, task, "AllocHandle: out of handles (max: %d). SQL='%s'",
			   MAX_DB_HANDLE-1, sql); 
     */
    task->SetLastError(-1);
    return -1;
}


//-----------------------------------------------------------------------------
// Internal: release handle associated with a task
void DB_Service::ReleaseHandle(CTask *task, int handle)
{
    // is handle valid?
    if (!IsHandleValid( task, handle, "dbCloseRecordset"))
	return;

    // does the task have a valid tls?
    CDbTLS *tls = GetTLS(task);
    if (!tls)
	return;

    // release the TLS
    tls->m_Handle[handle] = 0;

    // update statistics
    --m_OpenedHandle;
}


//-----------------------------------------------------------------------------
// Internal: Create a new recordset object for a task
DB_Recordset *DB_Service::CreateRecordset( DB_Connection_Profile *profile, CTask *task, 
					  int *handle, const char *sql)
{
    if (!profile) {
	assert (false);
	return NULL;
    }

    // allocate new handle
    *handle = AllocHandle(task, sql);
    if (*handle == -1)
	return 0;

    // tell the database to create the recordset
    CDbTLS *tls = GetTLS(task);
    assert(tls);

    DB_Recordset *rst = profile->CreateRecordset(task, tls, sql);
    if (!rst) {
	task->SetLastError(-1);
	ReleaseHandle( task, *handle);
	*handle = -1;
	return 0;
    }

    tls->m_Handle[ *handle ] = rst;
    return rst;
}


//-----------------------------------------------------------------------------
// Internal: Get DB_Async_Object that cause the task to blocked
DB_Async_Object *DB_Service::GetCurrentDbObject( CTask *task)
{
    CDbTLS *tls = GetTLS(task);
    if (!tls)
	return 0;

    if ( !IsHandleValid( task, tls->m_Current, "GetCurrentDbObject (dbdriver.cpp)"))
	return 0;

    return tls->m_Handle[ tls->m_Current ];
}


//-----------------------------------------------------------------------------
// Internal: return the column index from the column name
int DB_Service::GetColumn( CTask *task, DB_Recordset *rst, const char *name)
{
    int col = rst->GetColumn(name);
    if (col < 1) {
	task->SetLastError(-1);
	/* CLog::Info ( DBDRIVER, task, "Column '%s' not found", name); */
	return -1;
    }

    return col;
}

//-----------------------------------------------------------------------------
// Open a new recordset, and move to first row, if any [async]
STATUS DB_Service::API_DbOpen( CTask *task, const char *profile_name, int *hnd, const char *sql)
{
    /* CLog::Debug( DBDRIVER, task, "dbRecordset: %s", sql); */
     //printf("dbRecordset: %s\n", sql);

    // Invalidate return value
    *hnd = -1;

    CDbTLS *tls = GetTLS(task);
    if (tls==NULL) {
	 CLog::Error( DBDRIVER, task, "dbRecordset '%s': Task does not have database connection",
				      sql); 
         
	task->SetLastError(-1);
	return OK;
    }

    // Find the database profile
    DB_Connection_Profile *profile = FindProfile(profile_name);
    if (profile == NULL) {
	 CLog::Error( DBDRIVER, task, "dbRecordset '%s': unable to find profile '%s'",
				      sql, profile_name); 
        
	task->SetLastError(-1);
	return OK;
    }
    
    // create recordset object, assign to task's handle
	DB_Recordset *rst = CreateRecordset( profile, task, hnd, sql);
    if (rst) {

	tls->m_Current = *hnd;
	task->SetLastError(0);
	tls->m_Ret = hnd;
    } else {
	tls->m_Ret = NULL;
	*hnd = -1;
	task->SetLastError(-1);
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Close recordset [sync]
STATUS DB_Service::API_DbClose( CTask *task, int hnd)
{
    /* CLog::Debug( DBDRIVER, task, "dbCloseRecordset for handle %d", hnd); */
    //printf("dbCloseRecordset for handle %d\n", hnd);

    DB_Recordset *rst = (DB_Recordset*) GetRstObject( task, hnd, "dbCloseRecordset");
    if (!rst)
	return OK;

    if (rst->Close() != 0) {
	DBERROR( "dbCloseRecordset", task, rst);
    } else {
	task->SetLastError(0);
    }
    //delete rst;
    ReleaseHandle( task, hnd);
    return OK;
}

//-----------------------------------------------------------------------------
// Requery recordset [async]
STATUS DB_Service::API_DbRequery( CTask *task, int hnd)
{
    /* CLog::Debug( DBDRIVER, task, "DBRequery for handle %d", hnd); */
    //printf("DBRequery for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbRequery"));
    if (!rst)
	return OK;

    if (rst->Requery() != 0) {
	DBERROR( "DbRequery", task, rst);
    } else {
	GetTLS(task)->m_Current=hnd;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Add a new record, and move to the new record [sync]
STATUS DB_Service::API_DbAddNew( CTask *task, int hnd)
{
    /* CLog::Debug( DBDRIVER, task, "DbAddNew for handle %d", hnd); */
    //printf("DbAddNew for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbAddNew"));
    if (!rst)
	return OK;

    if (rst->AddNew() != 0) {
	DBERROR( "DbAddNew", task, rst);
    } else {
	GetTLS(task)->m_Current=hnd;
	task->SetLastError(0);
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Delete current record [async]
STATUS DB_Service::API_DbDelete( CTask *task, int hnd)
{
    /* CLog::Debug( DBDRIVER, task, "DbDelete for handle %d", hnd); */
    //printf("DbDelete for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbDelete"));
    if (!rst)
	return OK;

    if (rst->Delete() != 0) {
	DBERROR( "DbDelete", task, rst);
    } else {
	GetTLS(task)->m_Current=hnd;
    }


    return OK;
}

//-----------------------------------------------------------------------------
// Start editing current record [sync]
STATUS DB_Service::API_DbEdit( CTask *task, int hnd)
{
    /* CLog::Debug( DBDRIVER, task, "DbEdit for handle %d", hnd); */
    //printf("DbEdit for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbEdit"));
    if (!rst)
	return OK;

    if (rst->Edit()) {
	DBERROR( "DbEdit", task, rst);
    } else {
	GetTLS(task)->m_Current=hnd;
	task->SetLastError(0);
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Update changes of current record [async]
STATUS DB_Service::API_DbUpdate( CTask *task, int hnd)
{
    /* CLog::Debug( DBDRIVER, task, "DbUpdate for handle %d", hnd); */
    //printf("DbUpdate for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbUpdate"));
    if (!rst)
	return OK;

    if (rst->Update()) {
	DBERROR( "DbUpdate", task, rst);
    } else {
	GetTLS(task)->m_Current=hnd;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Check recordset updatability [sync]
STATUS DB_Service::API_DbIsUpdatable( CTask *task, int *ret, int hnd)
{
    /* CLog::Debug( DBDRIVER, task, "DbIsupdateable for handle %d", hnd); */
    //printf("DbIsupdateable for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbIsUpdatable"));
    if (!rst)
	return OK;

    *ret = rst->IsUpdatable() ? APPTRUE : APPFALSE;
    task->SetLastError(0);

    return OK;
}

//-----------------------------------------------------------------------------
// Execute SQL [async]
STATUS DB_Service::API_DbExecute( CTask *task, const char *profile_name, const char *sql)
{
    /* CLog::Debug( DBDRIVER, task, "DbExecute: %s", sql); */
    //printf("DbExecute: %s\n", sql);

    CDbTLS *tls = GetTLS(task);
    if (tls==NULL) {
	/* CLog::Error( DBDRIVER, task, "dbExecSQL '%s': Task doesn't have database connection", sql);
         */
        //printf("dbExecSQL '%s': Task doesn't have database connection", sql);
	task->SetLastError(-1);
	return OK;
    }

    // close previous handle (if any)
    if (tls->m_Handle[0]) {
	DB_SQL_Exec *executor = static_cast<DB_SQL_Exec*>(tls->m_Handle[0]);
	executor->Close();
	ReleaseHandle( task, 0);
    }

    // find database profile.
    DB_Connection_Profile *profile = FindProfile(profile_name);
    if (profile == NULL) {
	/* CLog::Error( DBDRIVER, task, "dbExecSQL '%s': Unable to find database connection profile '%s'", 
				     sql, profile_name);
         */
        //printf("dbExecSQL '%s': Unable to find database connection profile '%s'\n", sql, profile_name);
	task->SetLastError(-1);
	return OK;
    }

    // create executor
    DB_SQL_Exec *executor;
    tls->m_Handle[0] = executor = profile->CreateSQLExec(task, tls, sql);
    if (!tls->m_Handle[0]) {
	return OK;
    }

    // execute
    if (executor->Execute( sql )) {
	DBERROR( "DbExecute", task, executor);
	tls->m_Handle[0] = 0;
	executor->Close();
	return NOT_OK;
    } else {
	tls->m_Current = 0;
	m_OpenedHandle++;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Get affected rows of recently executed SQL [sync]
STATUS DB_Service::API_DbGetRowsAffected( CTask *task, int *nrow)
{
    /* CLog::Debug( DBDRIVER, task, "DbGetRowsAffected"); */
    //printf("DbGetRowsAffected\n");

    CDbTLS *tls = GetTLS(task);
    if (tls==NULL) {
	/* CLog::Error( DBDRIVER, task, "dbGetRowsAffected: task doesn't have database connection");
         */
        //printf("dbGetRowsAffected: task doesn't have database connection\n");
	return OK;
    }


    if (tls->m_Handle[0] == NULL) {
	/* CLog::Error (DBDRIVER, task, "DbGetRowsAffected: invalid handle"); */
        //printf("DbGetRowsAffected: invalid handle\n");
	task->SetLastError(-1);
	return OK;
    }

    *nrow = ((DB_SQL_Exec*)tls->m_Handle[0])->GetRowsAffected();
    task->SetLastError(0);

    return OK;
}

//-----------------------------------------------------------------------------
// Move first [async]
STATUS DB_Service::API_DbMoveFirst( CTask *task, int hnd )
{
    /* CLog::Debug( DBDRIVER, task, "DbMoveFirst for handle %d", hnd); */
    //printf("DbMoveFirst for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbMoveFirst"));
    if (!rst)
	return OK;

    if (rst->MoveFirst()) {
	DBERROR( "DbMoveFirst", task, rst);
    } else {
	GetTLS(task)->m_Current=hnd;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Move previous record [async]
STATUS DB_Service::API_DbMovePrev( CTask *task, int hnd )
{
    /* CLog::Debug( DBDRIVER, task, "DbMovePrev for handle %d", hnd); */
    //printf("DbMovePrev for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbMovePrev"));
    if (!rst)
	return OK;

    if (rst->MovePrev()) {
	DBERROR( "DbMovePrev", task, rst);
    } else {
	GetTLS(task)->m_Current=hnd;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Move next record [async]
STATUS DB_Service::API_DbMoveNext( CTask *task, int hnd)
{
    /* CLog::Debug( DBDRIVER, task, "DbMoveNext for handle %d", hnd); */
    //printf("DbMoveNext for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbMoveNext"));
    if (!rst)
	return OK;

    if (rst->MoveNext()) {
	DBERROR( "DbMoveNext", task, rst);
    } else {
	GetTLS(task)->m_Current=hnd;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Check EOF [sync]
STATUS DB_Service::API_DbIsEOF( CTask *task, int *ret, int hnd )
{
    /* CLog::Debug( DBDRIVER, task, "DbIsEOF for handle %d", hnd); */
    //printf("DbIsEOF for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbIsEOF"));
    if (!rst)
	return OK;

    *ret = rst->IsEOF() ? APPTRUE : APPFALSE;
    task->SetLastError(0);

    return OK;
}

//-----------------------------------------------------------------------------
// Check BOF [sync]
STATUS DB_Service::API_DbIsBOF( CTask *task, int *ret, int hnd)
{
    /* CLog::Debug( DBDRIVER, task, "DbIsBOF for handle %d", hnd); */
    //printf("DbIsBOF for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbIsBOF"));
    if (!rst)
	return OK;

    *ret = rst->IsBOF() ? APPTRUE : APPFALSE;
    task->SetLastError(0);

    return OK;
}

//-----------------------------------------------------------------------------
// Check null value [sync]
STATUS DB_Service::API_DbIsNull( CTask *task, int *ret, int hnd, const char *field)
{
    /* CLog::Debug( DBDRIVER, task, "DbIsNull for handle %d", hnd); */
    //printf("DbIsNull for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbIsNULL"));
    if (!rst)
	return OK;

    int col=GetColumn( task, rst, field);
    if (col < 1) {
	return OK;
    }

    if (CheckBeforeAccessData(task, rst, "dbIsNull") == false)
	return OK;

    if (rst->IsNull( col, ret)) {
	DBERROR( "DbIsNull", task, rst);
    } else {
	task->SetLastError(0);
	GetTLS(task)->m_Current = hnd;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Get String value [sync]
STATUS DB_Service::API_DbGetValStr( CTask *task, char *sretval, int hnd, const char *field)
{
    /* CLog::Debug( DBDRIVER, task, "DbGetValStr for handle %d", hnd); */
    //printf("DbGetValStr for handle %d\n", hnd);

    *sretval = '\0';

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbGetValStr"));
    if (!rst)
	return OK;

    int col = GetColumn( task, rst, field);
    if (col < 1) {
	*sretval = 0;
	return OK;
    }

    if (CheckBeforeAccessData(task, rst, "dbGetValString") == false)
	return OK;


    // check whether field is NULL
    int is_null;
    rst->IsNull (col, &is_null);
    if (is_null == APPTRUE) {
	/* CLog::Info ( DBDRIVER, task, "dbGetValString: attempt to read from NULL field (column=%s)",
				      field);
         */
        //printf("dbGetValString: attempt to read from NULL field (column=%s)\n", field);
	*sretval = 0;
	return OK;
    }

    // get the value
    if (rst->GetValString( col, sretval, MAXAPPSTRLEN)) {
	DBERROR( "DbGetValString", task, rst);
    } else {
	task->SetLastError(0);
	GetTLS(task)->m_Current = hnd;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Get int value [sync]
STATUS DB_Service::API_DbGetValInt( CTask *task, int *iretval, int hnd, const char *field)
{
    /* CLog::Debug( DBDRIVER, task, "DbGetValInt for handle %d", hnd); */
    //printf("DbGetValInt for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbGetValInt"));
    if (!rst)
	return OK;

    int col = GetColumn( task, rst, field);
    if (col < 1) {
	*iretval = 0;
	return OK;
    }

    if (CheckBeforeAccessData(task, rst, "dbGetValInt") == false)
	return OK;

    // check whether field is NULL
    int is_null;
    rst->IsNull (col, &is_null);
    if (is_null == APPTRUE) {
	/* CLog::Info ( DBDRIVER, task, "dbGetValInt: attempt to read from NULL field (column=%s)",
				      field);
         */
        //printf("dbGetValInt: attempt to read from NULL field (column=%s)\n", field);
	*iretval = 0;
	return OK;
    }

    // get the value
    if (rst->GetValInt( col, iretval)) {
	DBERROR( "DbGetValInt", task, rst);
    } else {
	task->SetLastError(0);
	GetTLS(task)->m_Current = hnd;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Get float value [sync]
STATUS DB_Service::API_DbGetValReal( CTask *task, float *fretval, int hnd, const char *field)
{
    /* CLog::Debug( DBDRIVER, task, "DbGetValReal for handle %d", hnd); */
    //printf("DbGetValReal for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbGetValReal"));
    if (!rst)
	return OK;

    int col = GetColumn( task, rst, field);
    if (col < 1) {
	*fretval = 0.0;
	return OK;
    }

    if (CheckBeforeAccessData(task, rst, "dbGetValReal") == false)
	return OK;

    // check whether field is NULL
    int is_null;
    rst->IsNull (col, &is_null);
    if (is_null == APPTRUE) {
	/* CLog::Info ( DBDRIVER, task, "dbGetValReal: attempt to read from NULL field (column=%s)",
				      field);
         */
         //printf("dbGetValReal: attempt to read from NULL field (column=%s)\n", field);
	*fretval = 0.0;
	return OK;
    }

    // get the value
    if( rst->GetValFloat( col, fretval)) {
	DBERROR( "DbGetValReal", task, rst);
    } else {
	task->SetLastError(0);
	GetTLS(task)->m_Current = hnd;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Get date value [sync]
STATUS DB_Service::API_DbGetValDate( CTask *task, int *retval, int hnd, const char *field)
{
    /* CLog::Debug( DBDRIVER, task, "GetValDate for handle %d", hnd); */
    //printf("GetValDate for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbGetValDate"));
    if (!rst)
	return OK;

    int col = GetColumn( task, rst, field);
    if (col < 1) {
	*retval = 0;
	return OK;
    }

    if (CheckBeforeAccessData(task, rst, "dbGetValDate") == false)
	return OK;

    // check whether field is NULL
    int is_null;
    rst->IsNull (col, &is_null);
    if (is_null == APPTRUE) {
	CLog::Info ( DBDRIVER, task, "dbGetValDate: attempt to read from NULL field (column=%s)",
				      field);
	*retval = 0;
	return OK;
    }

    // get the value
    if (rst->GetValDate( col, (time_t*)retval)) {
	DBERROR( "GetValDate", task, rst);
    } else {
	task->SetLastError(0);
	GetTLS(task)->m_Current = hnd;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Set String value [sync]
STATUS DB_Service::API_DbSetValStr( CTask *task, int hnd, const char *field, const char *sval)
{
    /* CLog::Debug( DBDRIVER, task, "DbSetValStr for handle %d", hnd); */
    //printf("DbSetValStr for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbSetValStr"));
    if (!rst)
	return OK;

    int col = GetColumn( task, rst, field);
    if (col < 1) {
	return OK;
    }

    if (CheckBeforeAccessData(task, rst, "dbSetValStr") == false)
	return OK;

    if (rst->SetValString( col, sval)) {
	DBERROR( "DbSetValStr", task, rst);
    } else {
	task->SetLastError(0);
	GetTLS(task)->m_Current = hnd;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Set int value [sync]
STATUS DB_Service::API_DbSetValInt( CTask *task, int hnd, const char *field, int ival)
{
    /* CLog::Debug( DBDRIVER, task, "DbSetValInt for handle %d", hnd); */
    //printf("DbSetValInt for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbSetValInt"));
    if (!rst)
	return OK;

    int col = GetColumn( task, rst, field);
    if (col < 1) {
	return OK;
    }

    if (CheckBeforeAccessData(task, rst, "dbSetValInt") == false)
	return OK;

    if (rst->SetValInt( col, ival)) {
	DBERROR( "SetValInt", task, rst);
    } else {
	task->SetLastError(0);
	GetTLS(task)->m_Current = hnd;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Set float value [sync]
STATUS DB_Service::API_DbSetValReal( CTask *task, int hnd, const char *field, float fval)
{
    /* CLog::Debug( DBDRIVER, task, "DbSetValReal for handle %d", hnd); */
    //printf("DbSetValReal for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbSetValReal"));
    if (!rst)
	return OK;

    int col = GetColumn( task, rst, field);
    if (col < 1) {
	return OK;
    }

    if (CheckBeforeAccessData(task, rst, "dbSetValReal") == false)
	return OK;

    if (rst->SetValFloat( col, fval)) {
	DBERROR( "DbSetValReal", task, rst);
    } else {
	task->SetLastError(0);
	GetTLS(task)->m_Current = hnd;
    }

    return OK;
}

//-----------------------------------------------------------------------------
// Set date value [sync]
STATUS DB_Service::API_DbSetValDate( CTask *task, int hnd, const char *field, int val)
{
    /* CLog::Debug( DBDRIVER, task, "DbSetValDate for handle %d", hnd); */
    //printf("DbSetValDate for handle %d\n", hnd);

    DB_Recordset *rst = static_cast<DB_Recordset*>(GetRstObject( task, hnd, "dbSetValDate"));
    if (!rst)
	return OK;

    int col = GetColumn( task, rst, field);
    if (col < 1) {
	return OK;
    }

    if (CheckBeforeAccessData(task, rst, "dbSetValDate") == false)
	return OK;

    if (rst->SetValDate( col, (time_t)val)) {
	DBERROR( "DbSetValDate", task, rst);
    } else {
	task->SetLastError(0);
	GetTLS(task)->m_Current = hnd;
    }

    return OK;
}



///////////////////////////////////////////////////////////////////////////////
//
// API
//
///////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
DB_API::DB_API( DB_Service *d, int provider_id)
: CAPIProvider(provider_id, DBDRIVER)
{ 
    m_Driver=d; 
}


//-----------------------------------------------------------------------------
//bool DB_API::Init()
STATUS DB_API::Init()
{
    enum {
	dbRecordset,
	dbCloseRecordset,
	dbRequery,
	dbAddNew,
	dbDelete,
	dbEdit,
	dbUpdate,
	dbIsUpdatable,
	dbExecute,
	dbLock,
	dbTryLock,
	dbUnlock,
	dbMoveFirst,
	dbMoveNext,
	dbMovePrev,
	dbMoveLast,
	dbIsEOF,
	dbIsBOF,
	dbIsNull,
	dbGetValStr,
	dbGetValInt,
	dbGetValReal,
	dbSetValStr,
	dbSetValInt,
	dbSetValReal,
	dbGetValDate,
	dbSetValDate,
	dbGetRowsAffected,
	//dbGetDatabase,
	//dbSetDatabase,
    };

    if ( API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK)
	return NOT_OK;

    // STATUS API_DbGetDatabase( CTask *task, char *name );
    /*
    if (RegisterAPI( "dbGetDatabase", dbGetDatabase, false, CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &DB_API::API_DbGetDatabase,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "retval", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }
    */


    // STATUS API_DbSetDatabase( CTask *task, const char *name );
    /*
    if (RegisterAPI( "dbSetDatabase", dbSetDatabase, false, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbSetDatabase,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "db_name", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }
    */


    // STATUS  API_DbOpen( CTask *task, int *hnd, const char *sql);
    if (RegisterAPI( "dbRecordset", dbRecordset, true, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbOpen,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "db_profile", BYVAL, CVariant::VT_STRING),
		     CArgType( "rethandle", BYREF, CVariant::VT_INT),
		     CArgType( "query_sql", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbClose( CTask *task, int hnd);
    if (RegisterAPI( "dbCloseRecordset", dbCloseRecordset, true, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbClose,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbRequery( CTask*, int hnd);
    if (RegisterAPI( "dbRequery", dbRequery, true, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbRequery,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbAddNew( CTask *task, int hnd);
    if (RegisterAPI( "dbAddNew", dbAddNew, false, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbAddNew,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbDelete( CTask *task, int hnd);
    if (RegisterAPI( "dbDelete", dbDelete, true, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbDelete,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }
    
    // STATUS  API_DbEdit( CTask *task, int hnd);
    if (RegisterAPI( "dbEdit", dbEdit, false, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbEdit,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }
    
    // STATUS  API_DbUpdate( CTask *task, int hnd);
    if (RegisterAPI( "dbUpdate", dbUpdate, true, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbUpdate,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }
    
    // STATUS  API_DbIsUpdatable( CTask *task, int *ret, int hnd);
    if (RegisterAPI( "dbIsUpdatable", dbIsUpdatable, false, CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &DB_API::API_DbIsUpdatable,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "return", BYREF, CVariant::VT_RET),
		     CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }
    
    // STATUS  API_DbExecute( CTask *task, const char *sql);
    if (RegisterAPI( "dbExecSQL", dbExecute, true, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbExecute,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "db_profile", BYVAL, CVariant::VT_STRING),
		     CArgType( "sql", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }
    
    // STATUS  API_DbGetRowsAffected( CTask *task, int *ret);
    if (RegisterAPI( "dbGetRowsAffected", dbGetRowsAffected, false, CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &DB_API::API_DbGetRowsAffected,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbMoveFirst( CTask *task, int hnd );
    if (RegisterAPI( "dbMoveFirst", dbMoveFirst, true, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbMoveFirst,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbMovePrev( CTask *task, int hnd );
    if (RegisterAPI( "dbMovePrev", dbMovePrev, true, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbMovePrev,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbMoveNext( CTask*, int hnd);
    if (RegisterAPI( "dbMoveNext", dbMoveNext, true, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbMoveNext,
		     CAPIDef::CDECL_TYPE,
		     2,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbIsEOF( CTask *task, int *ret, int hnd );
    if (RegisterAPI( "dbIsEOF", dbIsEOF, false, CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &DB_API::API_DbIsEOF,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "return", BYREF, CVariant::VT_RET),
		     CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  DB_Service::API_DbIsNull( CTask *task, int *ret, int hnd, const char *field)
    if (RegisterAPI( "DbIsNull", dbIsNull, false, CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &DB_API::API_DbIsNull,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "return", BYREF, CVariant::VT_RET),
		     CArgType( "handle", BYVAL, CVariant::VT_INT),
		     CArgType( "field", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }
    
    // STATUS  API_DbIsBOF( CTask *task, int *ret, int hnd);
    if (RegisterAPI( "dbIsBOF", dbIsBOF, false, CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &DB_API::API_DbIsBOF,
		     CAPIDef::CDECL_TYPE,
		     3,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "return", BYREF, CVariant::VT_RET),
		     CArgType( "handle", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbGetValStr( CTask*, char *sretval, int hnd, const char *field);
    if (RegisterAPI( "dbGetValueStr", dbGetValStr, false, CVariant::VT_STRING,
		     (CAPIProvider::APIPROC) &DB_API::API_DbGetValStr,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "handle", BYVAL, CVariant::VT_INT),
		     CArgType( "field", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbGetValInt( CTask*, int *iretval, int hnd, const char *field);
    if (RegisterAPI( "dbGetValueInt", dbGetValInt, false, CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &DB_API::API_DbGetValInt,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "handle", BYVAL, CVariant::VT_INT),
		     CArgType( "field", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbGetValReal( CTask*, float *fretval, int hnd, const char *field);
    if (RegisterAPI( "dbGetValueReal", dbGetValReal, false, CVariant::VT_FLOAT,
		     (CAPIProvider::APIPROC) &DB_API::API_DbGetValReal,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "handle", BYVAL, CVariant::VT_INT),
		     CArgType( "field", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbGetValDate( CTask*, float *fretval, int hnd, const char *field);
    if (RegisterAPI( "dbGetValueDate", dbGetValDate, false, CVariant::VT_INT,
		     (CAPIProvider::APIPROC) &DB_API::API_DbGetValDate,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "ret_val", BYREF, CVariant::VT_RET),
		     CArgType( "handle", BYVAL, CVariant::VT_INT),
		     CArgType( "field", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbSetValStr( CTask *task, int hnd, const char *field, const char *sval);
    if (RegisterAPI( "dbSetValueStr", dbSetValStr, false, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbSetValStr,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "handle", BYVAL, CVariant::VT_INT),
		     CArgType( "field", BYVAL, CVariant::VT_STRING),
		     CArgType( "value", BYVAL, CVariant::VT_STRING)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbSetValInt( CTask *task, int hnd, const char *field, int ival);
    if (RegisterAPI( "dbSetValueInt", dbSetValInt, false, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbSetValInt,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "handle", BYVAL, CVariant::VT_INT),
		     CArgType( "field", BYVAL, CVariant::VT_STRING),
		     CArgType( "value", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbSetValReal( CTask *task, int hnd, const char *field, float fval);
    if (RegisterAPI( "dbSetValueReal", dbSetValReal, false, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbSetValReal,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "handle", BYVAL, CVariant::VT_INT),
		     CArgType( "field", BYVAL, CVariant::VT_STRING),
		     CArgType( "value", BYVAL, CVariant::VT_FLOAT)) == NOT_OK) {
	return NOT_OK;
    }

    // STATUS  API_DbSetValDate( CTask *task, int hnd, const char *field, int fval);
    if (RegisterAPI( "dbSetValueDate", dbSetValDate, false, CVariant::VT_VOID,
		     (CAPIProvider::APIPROC) &DB_API::API_DbSetValDate,
		     CAPIDef::CDECL_TYPE,
		     4,
		     CArgType( "task", BYVAL, CVariant::VT_TASK),
		     CArgType( "handle", BYVAL, CVariant::VT_INT),
		     CArgType( "field", BYVAL, CVariant::VT_STRING),
		     CArgType( "value", BYVAL, CVariant::VT_INT)) == NOT_OK) {
	return NOT_OK;
    }


    /* CLog::Detail( DBDRIVER, "API initialized"); */
    //printf("API initialized\n");
    return OK;
}


STATUS DB_API::Shutdown()
{
    /* CLog::Detail( DBDRIVER, "API shutdown"); */
    //printf("API shutdown\n");
    API_Provider_Manager::Instance()->UnregisterProvider(this);

    return OK;
}
