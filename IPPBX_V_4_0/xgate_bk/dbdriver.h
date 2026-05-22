/* $Header: /xGate/dbdriver.h 17    7/29/04 7:29p Bennylp $
 *   
 * XGATE PROJECT
 * (C) 2001 Switchlab, Ltd
 *
 * XGATE DATABASE SERVICE.
 *
 * $Log: /xGate/dbdriver.h $
 * 
 * 17    7/29/04 7:29p Bennylp
 * Zombie the connection upon really fatal error
 * 
 * 16    1/22/04 10:49p Bennylp
 * Workaround to crash problems in DB_Connection_Slot::HandleEvents
 * 
 * 15    1/09/04 6:18p Bennylp
 * Added checking EOF/BOF condition before data is accessed
 * 
 * 14    5/09/02 7:54p Bennylp
 * Several enhancements to database:
 *   - encrypt password in connect string (RC4 + base64)
 *   - added information about currently running SQL in a connection
 *   - added lock information in connection
 *   - added command to cancel currently running SQL
 *   - added command to force reconnection of a DB connection
 * 
 * 13    5/08/02 7:30p Bennylp
 * Select connection based on the least used && show query time.
 * 
 * 12    5/08/02 6:33p Bennylp
 * Quick fix: show currently running query in one connection
 * 
 * 11    7/12/01 3:58p Bennylp
 * Add term command 'show db-handles'
 * 
 * 10    6/09/01 4:58p Bennylp
 * Fix bug which causes crash in dbdrv::AllocHandle.
 * 
 * 9     6/08/01 2:56p Bennylp
 * Fixed bugs in dbdriver (reload).
 * 
 * 8     6/07/01 1:10p Bennylp
 * Bug fixes in reloading profile.
 * 
 * 7     6/06/01 7:09p Bennylp
 * Supports dynamic config and auto-connect when connection failed.
 * 
 * 6     5/30/01 8:50p Bennylp
 * Fixed system locking
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

#ifndef __DBDRIVER_H__
#define __DBDRIVER_H__

#include "api.h"
#include <ace/Synch.h>
#include <ace/Thread_Mutex.h>
#include <ace/Guard_T.h>

//typedef ACE_Token ACE_Thread_Mutex;	//linux porting

class DB_Async_Object;
class DB_Database;
class DB_Recordset;
class DB_SQL_Exec;
class DB_API;
class DB_Service;
class CTask;
class Cfg_Tree_Group;
class DB_Connection_Profile;
class DB_Connection_Slot;



#define MAX_DB_HANDLE	10000


//=============================================================================
// Open recordset and execute SQL options
enum DB_RST_OPTION {
  DB_EXEC_ASYNC=1,
};



//-------------------------------------------------------------------------
// Representation of task's TLS value
struct CDbTLS {

  DB_Async_Object	*m_Handle[MAX_DB_HANDLE];
  int		m_Current;
  int		*m_Ret;

  CDbTLS();
  ~CDbTLS();
};

//=============================================================================
// Parameters/options for database connection
//
class DB_Connection_Option {
  public:
    DB_Connection_Option();

    enum CursorType {
      UNKNOWN,
      FORWARD_ONLY,
      STATIC,
      KEYSET,
      DYNAMIC
    };

    enum CursorSide {
      UNKNOWN_SIDE,
      SERVER_SIDE,
      CLIENT_SIDE,
    };

    CursorType	m_CursorType;
    CursorSide	m_CursorSide;
    int		m_QueryTimeout;
    bool	m_EnableAsync;
    unsigned	m_StmtCacheSize;
    unsigned	m_RowsetSize;
    bool	m_ReadOnly;

    bool operator == (const DB_Connection_Option & rhs) const {
      return m_CursorType == rhs.m_CursorType &&
        m_CursorSide == rhs.m_CursorSide &&
        m_QueryTimeout == rhs.m_QueryTimeout &&
        m_EnableAsync == rhs.m_EnableAsync &&
        m_StmtCacheSize == rhs.m_StmtCacheSize &&
        m_RowsetSize == rhs.m_RowsetSize &&
        m_ReadOnly == rhs.m_ReadOnly;
    }

    unsigned long GetODBCCursorType() const;
    static CursorType GetCursorType(const char *type_name);
    const char * GetCursorTypeString() const;

    unsigned long GetODBCCursorSide() const;
    static CursorSide GetCursorSide(const char *side_name);
    const char * GetCursorSideString() const;
};



//=============================================================================
// Abstract asynchronous database object
class DB_Async_Object {
  public:

    // Status of current/last async. execution
    enum ExecState {
      XSTATE_READY,	    // successful
      XSTATE_EXECUTING,   // still executing
      XSTATE_ERROR,	    // error
      XSTATE_CLOSED
    };

    DB_Async_Object( CTask *task, DB_Database *parent)
      : m_ExecState(XSTATE_ERROR), m_ParentDb(parent), m_Task(task), 
      m_TimeBeginExecuting(0), m_Quitting(false) 
  {
  }

    virtual ~DB_Async_Object() {}

    DB_Database	*GetParentDb() { return m_ParentDb; }
    CTask	*GetTask() { return m_Task; }
    const CTask	*GetTask() const { return m_Task; }

    ExecState GetState() const { return m_ExecState; }
    time_t    GetTimeBeginExecuting() const { return m_TimeBeginExecuting; }

    virtual ExecState PollEvents() = 0;
    virtual int CancelExecuting() = 0;
    virtual int CancelQuery() = 0;
    virtual int Close() = 0;
    virtual bool IsExclusive() = 0;
    virtual const char *GetSQL() const = 0;

    // Destroy the statement handle.
    virtual void DestroyStmt() = 0;

  protected:
    // Async. prolog & epilogue
    void	OnAsyncBegin();
    void    	OnAsyncEnd();

    ExecState	m_ExecState;

    time_t	m_TimeBeginExecuting;

    // Quit flag
    // TODO: fix
    bool	m_Quitting;

    DB_Database *m_ParentDb;
    CTask	*m_Task;

};


//=============================================================================
// Database abstract class
class DB_Database {
  public:
    DB_Database( DB_Service *drv, DB_Connection_Profile *profile, DB_Connection_Slot *slot)
      : m_Signature(SIGNATURE), m_Driver(drv), m_Profile(profile), m_Slot(slot) {}

    virtual ~DB_Database();

    virtual int Open(const char *connectstring) = 0;
    virtual int Close() = 0;
    virtual int CancelQuery() = 0;

    virtual DB_Recordset *OpenQuery(CTask *, const char *sql, int opt=DB_EXEC_ASYNC) = 0;
    virtual DB_SQL_Exec *CreateExecutor(CTask *) = 0;
    virtual size_t GetHandlesCount() const = 0;
    virtual size_t GetJobCount() const = 0;

    virtual int	    PollEvents() = 0;

    virtual int	OnAsyncBegin( DB_Async_Object *) = 0;
    virtual int	OnAsyncEnd( DB_Async_Object *) = 0;
    virtual void AbortJob(bool dont_close) = 0;
    virtual const CTask * GetExecutingTask() const = 0;
    virtual const DB_Async_Object * GetExecutingObject() const = 0;

    virtual CTask *GetExclusiveOwner() = 0;
    virtual DB_Async_Object * GetExclusiveObject() = 0;

    virtual const String & GetLastErrMsg() const = 0;

    DB_Service *GetDriver() { return m_Driver; }
    DB_Connection_Profile *GetProfile() { return m_Profile; }
    DB_Connection_Slot *GetSlot() { return m_Slot; }

    bool is_valid() const;

  private:
    enum { SIGNATURE = 0xDEADC0DE };
    unsigned m_Signature;
    DB_Service *m_Driver;
    DB_Connection_Profile *m_Profile;
    DB_Connection_Slot *m_Slot;
};


//=============================================================================
// Execute SQL
class DB_SQL_Exec : public DB_Async_Object {
  public:
    DB_SQL_Exec ( CTask *task, DB_Database *db)
      : DB_Async_Object(task, db) {}

    // initializing handles, etc. (doing things that can be share by several
    // executions)
    virtual int Init(int options = DB_EXEC_ASYNC) = 0;

    // execute SQL. The initialization should be minimal.
    virtual int Execute(const char *sql) = 0;

    // get number of affected rows (after execution completed)
    virtual int GetRowsAffected() = 0;

    // a DB_SQL_Exec can not exclusively locking the database
    // connection during its lifetime.
    virtual bool IsExclusive() { return false; }
};


//=============================================================================
// Recordset abstract class
//
class DB_Recordset : public DB_Async_Object {
  public:
    DB_Recordset(CTask *task, DB_Database *parent)
      : DB_Async_Object( task, parent ) {}

    virtual int Open(const char *sql, int options) = 0;

    virtual bool IsUpdatable() = 0;
    virtual int AddNew() = 0;
    virtual int Edit() = 0;
    virtual int Update() = 0;
    virtual int Delete() = 0;
    virtual int Requery() = 0;

    virtual int MoveFirst() = 0;
    virtual int MovePrev() = 0;
    virtual int MoveNext() = 0;
    virtual int MoveLast() = 0;

    virtual bool IsBOF() = 0;
    virtual bool IsEOF() = 0;

    virtual int GetColumnCount() = 0;
    virtual int GetColumn(const char *name) = 0;

    virtual int SetValInt(int col, int val) = 0;
    virtual int SetValFloat(int col, float val) = 0;
    virtual int SetValString(int col, const char *val) = 0;
    virtual int SetValDate(int col, time_t val) = 0;

    virtual int IsNull(int col, int*) = 0;

    virtual int GetValInt(int col, int *val) = 0;
    virtual int GetValFloat(int col, float *val) = 0;
    virtual int GetValString(int col, char *val, int len) = 0;
    virtual int GetValDate(int col, time_t *val) = 0;

};


struct DB_Connect_Param {
  bool	  tobeActive;
  bool	  finished;
  DB_Database  *connection;
  String	  connectString;
  //ACE_hthread_t handle;
  void* handle;
  DB_Connection_Slot *slot;
};


//-----------------------------------------------------------------------------
/** This class describes a single connection slot in the database profile.
  In normal operation, each instance of this class should have exactly one
  database connection. However, in some circumstances, there may be more
  than one db connection, e.g. the first connection failed and this class
  has to re-connect another connection.
  */
class DB_Connection_Slot {
  public:
    /** Constructor.
    */
    DB_Connection_Slot (unsigned idx, DB_Connection_Profile & profile);

    /** Destructor.
    */
    ~DB_Connection_Slot();

    /** Start open connection to remote host, asynchronously.
    */
    bool Open();

    /** Close connection to remote host.
    */
    bool Destroy();

    /** Disconnect connection to remote host.
    */
    bool Disconnect();

    /** Reconnect to database server using the configuration from the parent
      database profile.
      */
    bool Reconnect (bool force);

    /** Cancel currently running query in active connection.
    */
    bool CancelQuery();

    /** Event polling.
    */
    void HandleEvents();

    /** This will create and open a recordset object, using the next database 
      connection in round-robin fashion.
      */
    DB_Recordset *CreateRecordset(CTask *task, 
        bool task_has_exclusive_lock, 
        const char *sql);

    /** This will create and open an executor object, using the next database 
      connection in round-robin fashion.
      */
    DB_SQL_Exec *CreateSQLExec(CTask *task, 
        bool task_has_exclusive_lock, 
        const char *sql);

    /** Show status to the specified terminal.
    */
    void Show (String & output);

    /** Notification from database connection about the status.
    */
    void NotifyConnectionStatus(int status, DB_Connect_Param *param);

    /** This function is called by underlying database object/connection to signal
      this connection slot that an unrecoverable error has occured. Note that
      the server might not be the database connection that's owned by this slot.
      */
    void SignalDbFatal(DB_Database* sender, bool really_really_fatal);

    /** Get number of database handles opened with this slot.
    */
    size_t GetHandlesCount() const;

    /** Get number of pending jobs/statement.
    */
    size_t GetJobCount() const;

    /** Get total number of connection count in this slot.
    */
    size_t GetConnectionCount();

    /** Get the last time this slot is used to create handle.
    */
    time_t GetLastUsage() const {
      return lastUsage;
    }

    /** Determine whether slot is active.
    */
    bool IsActive() const {
      return GetState() == State_Active;
    }

    /** Detemine whether slot is ready to be destroyed.
    */
    bool CanBeDestroyed();

    /** Set database state to closing.
    */
    void SetClosing() {
      SetState (State_Closing);
    }

    /** Kill zombies.. */
    void KillZombies();

  private:
    enum { SIGNATURE = 0xDEADC0DE };
    unsigned		     dbSignature;
    DB_Connection_Profile &  dbProfile;

    enum State {
      State_Null,
      State_Connecting,
      State_Holding,
      State_Failed,
      State_Active,
      State_Closing,
      State_CloseWait,
    };
    State		     state;

    unsigned		     slotIndex;
    DB_Database		   * dbConnection;
    List <DB_Database*>	     dbDeactivateList;
    List <DB_Connect_Param*> connectingList;

    mutable ACE_Thread_Mutex		connectionMutex;
    typedef ACE_Guard<ACE_Thread_Mutex> ConnectionGuard;
    time_t		     lastConnectTry;
    time_t		     lastUsage;
    String		     lastErrMsg;
    size_t		     dbTransition;

    struct DB_Zombie
    {
      time_t	    timeDead;
      DB_Database	   *db;
      String	    err;
    };
    List <DB_Zombie*>	    dbZombieList;

    void SetState(State st) {
      state = st;
    }
    State GetState() const {
      return state;
    }
    void SetLastError(const String & msg) {
      lastErrMsg = msg;
    }

    // Worker thread for opening connection.
    bool StartOpen (DB_Connect_Param *param);

    static void *start_thread(void *);
    void DBOpen ( DB_Connect_Param *param );

    bool is_valid() const;
};


//-----------------------------------------------------------------------------
/** Database connection profile describes the attributes of database
  connections, and maintains list of actual database connections that have
  been made using this profile.
  */
class DB_Connection_Profile : public ServiceProvider {
  public:
    /** Constructor.
    */
    DB_Connection_Profile(const String &name);

    /** Destructor.
    */
    ~DB_Connection_Profile();

    /** This function will be called by the service framework after 
      instantiation of this class, to allow the instance to initialize
      itself.
      */
    virtual bool Load();

    /** This function will be called by the service framework before it is
      destroyed/deleted, to allow the instance to cleanup itself.
      */
    virtual bool Unload();

    /** This function will be called by the service framework once the 
      configuration file has been fully read.
      */
    virtual bool CommitConfiguration();

    /** HandleEvents: Descendent shall implement this abstract method.
      It is called by Service Provider Manager
      */
    virtual void HandleEvents();

    /** Create all database connections, according to the connection count.
      @param reconnect  Reconnect existing connections even if connection
      parameters are not changed.
      */
    bool ReloadDatabase(bool reconnect);

    /** Disconnect and destroy all existing database connections.
    */
    void DestroyDatabases();

    /** This will create and open a recordset object, using the next database 
      connection in round-robin fashion.
      */
    DB_Recordset *CreateRecordset(CTask *task, CDbTLS *tls, const char *sql);

    /** This will create and open an executor object, using the next database 
      connection in round-robin fashion.
      */
    DB_SQL_Exec *CreateSQLExec(CTask *task, CDbTLS *tls, const char *sql);

    /** Terminal command callback
    */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

    /** Show configuration for the specified entry.
      This function will be called by the framework whenever it needs to display the
      specified entry. Previously this instance should tell the entry that it wishes to
      handle displaying the entry by calling entry->RegisterShowHandler().
      */
    virtual bool OnShowConfig(
        const Cfg_Tree_Entry *entry,
        String & output, 
        const String & indent, 
        int indent_size
        ) const;

    /** Show database status.
    */
    virtual bool OnShowStatus ( String & output );

    /** Show configuration entry without acquiring system lock
    */
    STATUS OnShowConfig_NoLock (
        const Cfg_Tree_Entry *entry,
        String & output, 
        const String & indent, 
        int indent_size
        ) const;

    /** Get the profile's configuration entry.
    */
    Cfg_Tree_Group * GetConfig() {
      return cfgEntry;
    }

    /** Determine whether the database is ok to delete.
    */
    bool CanBeDeleted();

    /** Get database connect string.
    */
    const String & GetConnectString() const {
      return workingParam.connectString;
    }

    /** Get database connection option
    */
    const DB_Connection_Option  & GetConnectionOption() const {
      return workingParam.connectionOption;
    }

  private:
    struct DB_Parameter {
      // Driver type.
      String	dbDriver;

      // Database connect String.
      String  connectString;

      // Number of connections to be made.
      size_t  connectionCount;

      // Database connection option.
      DB_Connection_Option  connectionOption;

      bool operator == (const DB_Parameter & rhs) const {
        return dbDriver==rhs.dbDriver &&
          connectString == rhs.connectString &&
          connectionCount == rhs.connectionCount &&
          connectionOption == rhs.connectionOption;
      }
    } workingParam, currentParam;

    // list of databases
    Vector <DB_Connection_Slot*> m_DatabaseSlot;

    // Time when last database connection attempt was made.
    time_t  m_LastConnect;

    // Configuration entry.
    Cfg_Tree_Group *cfgEntry;

    // Shutdown flag.
    bool isShutdown;

    /** Get database in round-robin fashion
    */
    DB_Database *GetDatabase (CTask *task, CDbTLS *tls, const char *sql);

    DB_Async_Object *GetTaskExclusive(CDbTLS *tls);
};



//-----------------------------------------------------------------------------
/** This singleton class is the coordinator of all database activity.
*/
class DB_Service : public ServiceProvider {
  public:
    /** Destructor.
    */
    ~DB_Service();

    /** Create the singleton instance of this class.
    */
    static DB_Service * Create();

    /** Get the singleton instance of this class.
    */
    static DB_Service * Instance() {
      return dbSvcInstance;
    }

    /** This function will be called by the service framework after 
      instantiation of this class, to allow the instance to initialize
      itself.
      */
    virtual bool Load();

    /** This function will be called by the service framework before it is
      destroyed/deleted, to allow the instance to cleanup itself.
      */
    virtual bool Unload();

    /** This function will be called whenever a new task is created.
    */
    virtual bool AttachTask(CTask *);

    /** This function will be called whenever a task is about to be deleted.
    */
    virtual void DetachTask(CTask *);

    /** This function will be called if this instance has a pending job for
      the task.
      */
    virtual bool CancelJob(CTask *);

    /** This function is called by underlying database object to signal the
      task that the job has been completed.
      */
    STATUS	    WakeupTask( CTask *);

    /** Get the database root entry.
    */
    Cfg_Tree_Group *GetRootConfig() {
      return m_RootConfig;
    }

    /** Terminal command callback
    */
    virtual void DoTermCmd(Term_Session *, bool no, Term_Cmd *);

    /** Show configuration for the specified entry.
      This function will be called by the framework whenever it needs to display the
      specified entry. Previously this instance should tell the entry that it wishes to
      handle displaying the entry by calling entry->RegisterShowHandler().
      */
    virtual bool OnShowConfig(
        const Cfg_Tree_Entry *entry,
        String & output, 
        const String & indent, 
        int indent_size
        ) const;

    /** Show opened database handles for the specified task.
    */
    void ShowDbHandles_NoLock(String & output, CTask *task);

    //
    //APIs
    //
    STATUS  API_DbOpen( CTask *task, const char *profile, int *hnd, const char *sql);
    STATUS  API_DbClose( CTask *task, int hnd);
    STATUS  API_DbRequery( CTask*, int hnd);
    STATUS  API_DbAddNew( CTask *task, int hnd);
    STATUS  API_DbDelete( CTask *task, int hnd);
    STATUS  API_DbEdit( CTask *task, int hnd);
    STATUS  API_DbUpdate( CTask *task, int hnd);
    STATUS  API_DbIsUpdatable( CTask *task, int *ret, int hnd);
    STATUS  API_DbExecute( CTask *task, const char *profile, const char *sql);
    STATUS  API_DbGetRowsAffected( CTask *, int *);
    STATUS  API_DbMoveFirst( CTask *task, int hnd );
    STATUS  API_DbMovePrev( CTask *task, int hnd );
    STATUS  API_DbMoveNext( CTask*, int hnd);
    STATUS  API_DbIsEOF( CTask *task, int *ret, int hnd );
    STATUS  API_DbIsBOF( CTask *task, int *ret, int hnd);
    STATUS  API_DbIsNull( CTask *task, int *ret, int hnd, const char *field);
    STATUS  API_DbGetValStr( CTask*, char *sretval, int hnd, const char *field);
    STATUS  API_DbGetValInt( CTask*, int *iretval, int hnd, const char *field);
    STATUS  API_DbGetValReal( CTask*, float *fretval, int hnd, const char *field);
    STATUS  API_DbGetValDate( CTask*, int *retval, int hnd, const char *field);
    STATUS  API_DbSetValStr( CTask *task, int hnd, const char *field, const char *sval);
    STATUS  API_DbSetValInt( CTask *task, int hnd, const char *field, int ival);
    STATUS  API_DbSetValReal( CTask *task, int hnd, const char *field, float fval);
    STATUS  API_DbSetValDate( CTask *task, int hnd, const char *field, int val);

  private:
    // TLS value
    int	    m_TLS;

    // list of database connection description
    List <DB_Connection_Profile*> m_DatabaseProfiles;

    // API
    DB_API	*m_API;

    // Statistics
    int		m_OpenedHandle;	// number of opened handles

    // Singleton instance.
    static DB_Service *dbSvcInstance;

    // Database config root.
    Cfg_Tree_Group  *m_RootConfig;

    /** Constructor.
    */
    DB_Service();

    // Utility to set and display error
    void	    DBERROR( const char *title, CTask *, DB_Async_Object *rst );

    // Helper function to get object from task's handle
    int		    GetColumn( CTask *, DB_Recordset *rst, const char *name );

    bool	    IsHandleValid(CTask *task, int handle, const char *cmd);
    CDbTLS	   *GetTLS( CTask *);
    int		    AllocHandle( CTask *task, const char *sql);
    void	    ReleaseHandle(CTask *task, int handle);
    DB_Recordset   *CreateRecordset( DB_Connection_Profile *profile,
        CTask*, int *handle, const char *sql);
    DB_Async_Object *GetRstObject( CTask *task, int handle, const char *cmd);
    DB_Async_Object *GetCurrentDbObject( CTask *task);
    bool	    CheckBeforeAccessData(CTask *task, DB_Recordset *rst, 
        const char *api);

    // Profile helpers.
    DB_Connection_Profile * FindProfile(const char * name);
};



//-----------------------------------------------------------------------------
// API's
class DB_API : public CAPIProvider {

  DB_API( DB_Service *d, int provider_id);

  STATUS Init();
  STATUS	Shutdown();

  DB_Service   *m_Driver;

  STATUS API_CALL API_DbOpen( CTask *task, const char *profile, int *hnd, const char *sql) {
    return m_Driver->API_DbOpen( task, profile, hnd, sql);
  }

  STATUS API_CALL API_DbClose( CTask *task, int hnd) {
    return m_Driver->API_DbClose(task, hnd);
  }

  STATUS API_CALL API_DbRequery( CTask *task, int hnd) {
    return m_Driver->API_DbRequery( task, hnd);
  }

  STATUS API_CALL API_CALL API_DbAddNew( CTask *task, int hnd) {
    return m_Driver->API_DbAddNew( task, hnd);
  }

  STATUS API_CALL API_CALL API_DbDelete( CTask *task, int hnd) {
    return m_Driver->API_DbDelete( task, hnd);
  }

  STATUS API_CALL API_CALL API_DbEdit( CTask *task, int hnd) {
    return m_Driver->API_DbEdit( task, hnd);
  }

  STATUS API_CALL API_DbUpdate( CTask *task, int hnd) {
    return m_Driver->API_DbUpdate( task, hnd);
  }

  STATUS API_CALL API_DbIsUpdatable( CTask *task, int *ret, int hnd) {
    return m_Driver->API_DbIsUpdatable( task, ret, hnd);
  }

  STATUS API_CALL API_DbExecute( CTask *task, const char *profile, const char *sql) {
    return m_Driver->API_DbExecute( task, profile, sql);
  }

  STATUS API_CALL API_DbGetRowsAffected( CTask *task, int *ret) {
    return m_Driver->API_DbGetRowsAffected(task, ret);
  }

  STATUS API_CALL API_DbMoveFirst( CTask *task, int hnd ){
    return m_Driver->API_DbMoveFirst( task, hnd );
  }

  STATUS API_CALL API_DbMovePrev( CTask *task, int hnd ){
    return m_Driver->API_DbMovePrev( task, hnd );
  }

  STATUS API_CALL API_DbMoveNext( CTask *task, int hnd){
    return m_Driver->API_DbMoveNext( task, hnd);
  }

  STATUS API_CALL API_DbIsEOF( CTask *task, int *ret, int hnd ){
    return m_Driver->API_DbIsEOF( task, ret, hnd );
  }

  STATUS API_CALL API_DbIsBOF( CTask *task, int *ret, int hnd){
    return m_Driver->API_DbIsBOF( task, ret, hnd);
  }

  STATUS API_CALL API_DbIsNull( CTask *task, int *ret, int hnd, const char *field) {
    return m_Driver->API_DbIsNull( task, ret, hnd, field);
  }

  STATUS API_CALL API_DbGetValStr( CTask *task, char *sretval, int hnd, const char *field){
    return m_Driver->API_DbGetValStr( task, sretval, hnd, field);
  }

  STATUS API_CALL API_DbGetValInt( CTask *task, int *iretval, int hnd, const char *field){
    return m_Driver->API_DbGetValInt( task, iretval, hnd, field);
  }

  STATUS API_CALL API_DbGetValReal( CTask *task, float *fretval, int hnd, const char *field){
    return m_Driver->API_DbGetValReal( task, fretval, hnd, field);
  }

  STATUS API_CALL API_DbGetValDate( CTask* task, int *iretval, int hnd, const char *field) {
    return m_Driver->API_DbGetValDate( task, iretval, hnd, field);
  }

  STATUS API_CALL API_DbSetValStr( CTask *task, int hnd, const char *field, const char *sval){
    return m_Driver->API_DbSetValStr( task, hnd, field, sval);
  }

  STATUS API_CALL API_DbSetValInt( CTask *task, int hnd, const char *field, int ival){
    return m_Driver->API_DbSetValInt( task, hnd, field, ival);
  }

  STATUS API_CALL API_DbSetValReal( CTask *task, int hnd, const char *field, float fval){
    return m_Driver->API_DbSetValReal( task, hnd, field, fval);
  }

  STATUS API_CALL API_DbSetValDate( CTask *task, int hnd, const char *field, int ival){
    return m_Driver->API_DbSetValDate( task, hnd, field, ival);
  }

  friend class DB_Service;
};


#endif
