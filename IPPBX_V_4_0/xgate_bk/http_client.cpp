/*----------------------------------------------------------------* 
 * $Archive: /xGate/http_client.cpp $
 * $Date: 5/08/03 11:33a $
 * $Revision: 5 $
 *
 * $History: http_client.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Bennylp      Date: 5/08/03    Time: 11:33a
 * Updated in $/xGate
 * Added option to include/exclude HTTP
 * 
 * *****************  Version 4  *****************
 * User: Bennylp      Date: 4/23/03    Time: 5:34p
 * Updated in $/xGate
 * Administrivia: tidy-up the usage of API provider ID. Now the
 * enumeration in api.h should be used instead of bare constants.
 * 
 * *****************  Version 3  *****************
 * User: Junanto      Date: 16/04/03   Time: 12:00
 * Updated in $/xGate
 * Added new API HttpGetResponseHeader
 * 
 * *****************  Version 2  *****************
 * User: Junanto      Date: 15/04/03   Time: 18:53
 * Updated in $/xGate
 * 
 * *****************  Version 1  *****************
 * User: Junanto      Date: 15/04/03   Time: 17:33
 * Created in $/xGate
 * version 1
 *----------------------------------------------------------------*/

/*----------------------------------------------------------------* 
 * Description:
 *  
 *----------------------------------------------------------------*/

//#include "stdafx.h"
#include "xgconfig.h"

#ifdef XGATE_HAS_HTTP_API

//#include <wininet.h>

#include "kernel.h"
#include "task.h"
#include "api.h"
#include "serviceprovider.h"
#include "http_client.h"

#ifndef NEWLINE
  #define NEWLINE	    "\n"
#endif

//-----------------------------------------------------------------
static char __modname__[]	= "HttpCli";
const int HttpApiProviderId	= API_PROVIDER_ID_HTTP;

//-----------------------------------------------------------------
#define THISMODULE			__modname__
#define VALID_SIGNATURE			0xC0BABACA
#define HTTP_DEFAULT_AGENT		"xGate/2.0 (compatible; MSIE 4.0; )"
#define HTTP_DEFAULT_VERSION		"HTTP/1.1"
#define READ_BUFFER_SIZE		1024


//-----------------------------------------------------------------
#define HINTERNET int		//linux porting

//-----------------------------------------------------------------
/**
  Forward declaration
 */
class HTTP_Client;
class HTTP_Connection;


//-----------------------------------------------------------------
enum Connection_State 
{
    CONN_NULL,
    CONN_SENDING_REQUEST,
    CONN_ENDING_REQUEST,
    CONN_CONNECTED,
    CONN_READING_DATA,
    CONN_DISCONNECTED,
};


//-----------------------------------------------------------------
/**
  Message structure used to communicate asynchronous notification
  from callback thread to the main thread
 */
struct Async_Notification
{
    HTTP_Connection* connection_;
    unsigned result_;
    unsigned error_;
};


//-----------------------------------------------------------------
typedef List<HTTP_Connection*>		List_Connection;
typedef List<Async_Notification*>	List_Notification;


//-----------------------------------------------------------------
/**
  HTTP_Connection
  ---------------
  This object represents an active HTTP connection
  established on behalf of task
 */
class HTTP_Connection
{
public:
    //friend HTTP_Client;
    friend class HTTP_Client;	//linux porting

    /** 
      Object constructor/destructor
     */
    HTTP_Connection(HTTP_Client* manager, HINTERNET h_session);
    virtual ~HTTP_Connection();

    /** 
      Access to various internal member variables
    */
    bool Is_Valid();
    Connection_State State() { return conn_state_; }
    bool End_Of_File() { return end_of_file_; }
    int Get_Status_Code() { return status_code_; }
    int Get_Response_Length() { return response_length_; }
    CTask* Get_Task() { return user_task_; }
    const char* Get_Initial_Url() { return init_url_.c_str(); }
    const char* Get_Connected_Url() { return conn_url_; }
    time_t Get_Life_Time() { return time(NULL)-start_time_; }

    void Task_Attach(CTask *task) { user_task_ = task; }
    void Task_Set_Return_Code(int *retcode) { user_retcode_ = retcode; }

    /**
      Initiate asynchronous HTTP connection
     */
    int Send_Request(const char *verb, const char *url, const char* req_header, const char* req_body, DWORD connect_timeout, DWORD receive_timeout);

    /**
      Return the value of a header identified by header_name
     */
    const char* Get_Header(const char *header_name, unsigned long index);

    /**
      User's entry point to read data
     */
    int Read_Response(char *buffer, unsigned length);

    /**
      Called when an asynchronous request has completed
     */
    bool Asynch_Completed(unsigned result, unsigned error);

protected:
    int			    signature_;
    HINTERNET		    h_session_;
    HINTERNET		    h_connect_;
    HINTERNET		    h_file_;
    HTTP_Client*	    manager_;
    Connection_State	    conn_state_;
    bool		    end_of_file_;
    time_t		    start_time_;

    String		    verb_;				    // verb used for HTTP request (GET, POST, ...)
    String		    init_url_;				    // initial URL
    char		    conn_url_[INTERNET_MAX_URL_LENGTH+1];   // connected URL (may be different than initial)
    char		    header_value_[1024];		    // user for Get_Header()
    DWORD		    status_code_;			    // HTTP response status code

    /**
      Asynchronous response storage and counters
    */
    INTERNET_BUFFERS	    inbuff_;
    char		    response_buffer_[READ_BUFFER_SIZE+1];
    unsigned		    response_length_;			    // total response length (if known)
    unsigned		    response_nread_;			    // response read so far

    /**
      User's related variabled
     */
    char*		    user_buffer_ptr_;
    unsigned		    user_buffer_len_;
    int*		    user_retcode_;
    bool		    user_suspended_;
    CTask*		    user_task_;

    /**
      Read and copy data from the network to user's buffer
     */
    int Read_File_Asynch();
    int Copy_To_User();

    /**
      If an IO request is pending, the caller is suspended until completion
     */
    void Task_Suspend();
    void Task_Resume();

    /**
      Display error message and destroy the instance
     */
    void Process_Error(const char *location, unsigned error);
};


//-----------------------------------------------------------------
/** 
  HTTP_Client 
  -----------
  This singleton is the manager of all HTTP_Connection
  It is part of xGate device driver (service provider)
 */
class HTTP_Client : public ServiceProvider
{
    friend class HTTP_API_Provider;

public:
    /** 
      Object constructor/destructor
     */
    HTTP_Client();
    virtual ~HTTP_Client();

    /** 
      Create/retrieve the singleton of HTTP_Client
     */
    static HTTP_Client* Create();
    static HTTP_Client* Instance();

    /**
      Add/remove connections to/from the active list
     */
    void Add_Connection(HTTP_Connection* connection);
    void Remove_Connection(HTTP_Connection* connection);

protected:
    static HTTP_Client*	    instance_;
    int			    tls_id_;
    HINTERNET		    h_session_;
    HINTERNET		    h_file_;
    String		    http_agent_;
    String		    http_version_;
    List_Connection	    connections_;
    List_Notification	    notifs_;
    CRITICAL_SECTION	    crit_sect_;
    HTTP_API_Provider*	    api_prov_;

    /**
      Critical section management
     */
    void Critical_Section_Lock();
    void Critical_Section_Unlock();

    /**
      Win32 Internet callback function
     */
    static void CALLBACK Http_Callback_Handler(
		HINTERNET h_session, 
		DWORD context, 
		DWORD status, 
		void* status_info, 
		DWORD status_info_len);

    /**
      ServiceProvicer base classes derived methods
     */
    virtual bool Load();
    virtual bool Unload();
    virtual bool AttachTask(CTask* task);
    virtual void DetachTask(CTask* task);
    virtual bool CancelJob(CTask* task);
    virtual void HandleEvents();
    virtual void OnShutdown(bool cancel);
    virtual bool OnShowStatus(String & output);
    virtual bool CommitConfiguration();
    virtual void OnClearCounter();

    // API entry point
    STATUS API_CALL API_Send_Request(CTask* task, HTTP_Connection** handle, const char *verb, const char *url, const char* req_header, const char* req_body, DWORD connect_timeout, DWORD receive_timeout);
    STATUS API_CALL API_Get_Status_Code(CTask* task, int* status_code, HTTP_Connection* handle);
    STATUS API_CALL API_Get_Response_Length(CTask* task, int* response_length, HTTP_Connection* handle);
    STATUS API_CALL API_Read_Response(CTask* task, int* retcode, HTTP_Connection* handle, char *user_buffer);
    STATUS API_CALL API_Get_Header(CTask* task, int *retcode, HTTP_Connection* handle, const char* hdr_name, char* hdr_value);
    STATUS API_CALL API_Close(CTask* task, HTTP_Connection* handle);
};


//--------------------------------------------------------
/**
  API Provider
 */
class HTTP_API_Provider : public CAPIProvider
{
public:
    HTTP_API_Provider(HTTP_Client* mgr) :
      CAPIProvider(HttpApiProviderId, THISMODULE) 
    {
	// declare application APIs
	enum API_ID {
	    Id_Send_Request = 0,
	    Id_Get_Status_Code,
	    Id_Get_Response_Length,
	    Id_Read_Response,
	    Id_Get_Header,
	    Id_Close,
	    Id_Send_Request_Ex
	};

	if (API_Provider_Manager::Instance()->RegisterProvider(this) == NOT_OK) {
	    CLog::Error( THISMODULE, "Unable to register provider");
	    return;
	}

	RegisterAPI(
	    "HttpSendRequest", Id_Send_Request, true,
	    CVariant::VT_INT,
	    (CAPIProvider::APIPROC)API_Send_Request,
	    CAPIDef::CDECL_TYPE, 6,
	    CArgType("task",        BYVAL,  CVariant::VT_TASK),
	    CArgType("handle",      BYREF,  CVariant::VT_RET),
	    CArgType("verb",        BYVAL,  CVariant::VT_STRING),
	    CArgType("url",         BYVAL,  CVariant::VT_STRING),
	    CArgType("req_header",  BYVAL,  CVariant::VT_STRING),
	    CArgType("req_body",    BYVAL,  CVariant::VT_STRING)
	    );
    
	RegisterAPI(
	    "HttpSendRequestEx", Id_Send_Request_Ex, true,
	    CVariant::VT_INT,
	    (CAPIProvider::APIPROC)API_Send_RequestEx,
	    CAPIDef::CDECL_TYPE, 8,
	    CArgType("task",		BYVAL,  CVariant::VT_TASK),
	    CArgType("handle",		BYREF,  CVariant::VT_RET),
	    CArgType("max_conn_time",   BYVAL,  CVariant::VT_INT),
	    CArgType("max_read_time",   BYVAL,  CVariant::VT_INT),
	    CArgType("verb",		BYVAL,  CVariant::VT_STRING),
	    CArgType("url",		BYVAL,  CVariant::VT_STRING),
	    CArgType("req_header",	BYVAL,  CVariant::VT_STRING),
	    CArgType("req_body",	BYVAL,  CVariant::VT_STRING)
	    );
    
	RegisterAPI(
	    "HttpGetStatusCode", Id_Get_Status_Code, false,
	    CVariant::VT_INT,
	    (CAPIProvider::APIPROC)API_Get_Status_Code,
	    CAPIDef::CDECL_TYPE, 3,
	    CArgType("task",        BYVAL,  CVariant::VT_TASK),
	    CArgType("status",      BYREF,  CVariant::VT_RET),
	    CArgType("handle",      BYVAL,  CVariant::VT_INT)
	    );
    
	RegisterAPI(
	    "HttpGetResponseLength", Id_Get_Response_Length, false,
	    CVariant::VT_INT,
	    (CAPIProvider::APIPROC)API_Get_Response_Length,
	    CAPIDef::CDECL_TYPE, 3,
	    CArgType("task",        BYVAL,  CVariant::VT_TASK),
	    CArgType("length",      BYREF,  CVariant::VT_RET),
	    CArgType("handle",      BYVAL,  CVariant::VT_INT)
	    );
    
	RegisterAPI(
	    "HttpReadResponse", Id_Read_Response, true,
	    CVariant::VT_INT,
	    (CAPIProvider::APIPROC)API_Read_Response,
	    CAPIDef::CDECL_TYPE, 4,
	    CArgType("task",        BYVAL,  CVariant::VT_TASK),
	    CArgType("status",      BYREF,  CVariant::VT_RET),
	    CArgType("handle",      BYVAL,  CVariant::VT_INT),
	    CArgType("buffer",      BYREF,  CVariant::VT_STRING)
	    );
    
	RegisterAPI(
	    "HttpGetResponseHeader", Id_Get_Header, false,
	    CVariant::VT_INT,
	    (CAPIProvider::APIPROC)API_Get_Header,
	    CAPIDef::CDECL_TYPE, 5,
	    CArgType("task",        BYVAL,  CVariant::VT_TASK),
	    CArgType("status",      BYREF,  CVariant::VT_RET),
	    CArgType("handle",      BYVAL,  CVariant::VT_INT),
	    CArgType("name",	    BYVAL,  CVariant::VT_STRING),
	    CArgType("val",	    BYREF,  CVariant::VT_STRING)
	    );

	RegisterAPI(
	    "HttpClose", Id_Close, false,
	    CVariant::VT_VOID,
	    (CAPIProvider::APIPROC)API_Close,
	    CAPIDef::CDECL_TYPE, 2,
	    CArgType("task",        BYVAL,  CVariant::VT_TASK),
	    CArgType("handle",      BYVAL,  CVariant::VT_INT)
	    );

	manager_ = mgr;
    }

protected:
    HTTP_Client* manager_;

    // API entry point
    STATUS API_CALL API_Send_Request(CTask* task, HTTP_Connection** handle, const char *verb, const char *url, const char* req_header, const char* req_body)
    {
	return manager_->API_Send_Request(task, handle, verb, url, req_header, req_body, 8000, 16000);
    }

    STATUS API_CALL API_Send_RequestEx(CTask* task, HTTP_Connection** handle, int connect_timeout, int read_timeout, const char *verb, const char *url, const char* req_header, const char* req_body)
    {
	return manager_->API_Send_Request(task, handle, verb, url, req_header, req_body, connect_timeout*1000, read_timeout*1000);
    }


    STATUS API_CALL API_Get_Status_Code(CTask* task, int* status_code, HTTP_Connection* handle)
    {
	return manager_->API_Get_Status_Code(task, status_code, handle);
    }

    STATUS API_CALL API_Get_Response_Length(CTask* task, int* response_length, HTTP_Connection* handle)
    {
	return manager_->API_Get_Response_Length(task, response_length, handle);
    }

    STATUS API_CALL API_Read_Response(CTask* task, int* retcode, HTTP_Connection* handle, char *user_buffer)
    {
	return manager_->API_Read_Response(task, retcode, handle, user_buffer);
    }

    STATUS API_CALL API_Close(CTask* task, HTTP_Connection* handle)
    {
	return manager_->API_Close(task, handle);
    }

    STATUS API_CALL API_Get_Header(CTask* task, int *retcode, HTTP_Connection* handle, const char* hdr_name, char* hdr_value)
    {
	return manager_->API_Get_Header(task, retcode, handle, hdr_name, hdr_value);
    }

};


//-----------------------------------------------------------------
/**
  Convert Connection_State enum into string
 */
//-----------------------------------------------------------------
static const char* Connection_State_String(Connection_State state)
{
    if (((int)state < CONN_NULL) || ((int)state > CONN_DISCONNECTED))
	return "Unknown";

    static const char *xlat_tbl[] = {
	"Null",
	"Connecting",
	"Header",
	"Connected",
	"Receiving",
	"Disc'ed",
    };

    return xlat_tbl[state];
}


//-----------------------------------------------------------------
/**
  Convert Win32 Internet error into string
 */
static const char* Wininet_Error_Text(unsigned error)
{
    static struct Err_String {
	int errcode;
	const char* errmsg;
    } xlat_tbl[] = {
	{ ERROR_INTERNET_CONNECTION_RESET,	    "The connection with the server has been reset." },
	{ ERROR_INTERNET_SECURITY_CHANNEL_ERROR,    "Internal error loading the SSL libraries" },
	{ ERROR_INTERNET_TIMEOUT,		    "The request has timed out" },
	{ ERROR_INTERNET_INVALID_URL,		    "The URL is invalid" },
	{ ERROR_HTTP_REDIRECT_FAILED,		    "The redirection failed" },
	{ ERROR_HTTP_DOWNLEVEL_SERVER,		    "The server did not return any headers" },
	{ ERROR_HTTP_INVALID_SERVER_RESPONSE,	    "The server response could not be parsed" },
	{ ERROR_HTTP_INVALID_HEADER,		    "The supplied header is invalid" },
	{ ERROR_INTERNET_NAME_NOT_RESOLVED,	    "The server name could not be resolved" },
	{ ERROR_INTERNET_CANNOT_CONNECT,	    "The attempt to connect to the server failed." },
	{ ERROR_INTERNET_OPERATION_CANCELLED,	    "The operation was canceled" },
	{ ERROR_INTERNET_INVALID_CA,		    "Unsupported Certificate Authority"},
    };

    for (int i=0; i<sizeof(xlat_tbl)/sizeof(xlat_tbl[0]); i++) {
	if (xlat_tbl[i].errcode == error)
	    return xlat_tbl[i].errmsg;
    }

    static char buffer[128];
    sprintf(buffer, "WinInet error [%d]", error);
    return buffer;
}


//-----------------------------------------------------------------
/**
  HTTP_Client implementation
 */

/**
  Object constructor
 */
HTTP_Client::HTTP_Client() : 
    ServiceProvider("HTTP Client", true, true, Low_Priority, "HTTP client provider", IC_HTTP)
{
    http_agent_ = HTTP_DEFAULT_AGENT;
    http_version_ = HTTP_DEFAULT_VERSION;
    h_session_ = NULL;
    InitializeCriticalSection(&crit_sect_);
    api_prov_ = NULL;
}


/** 
  Object destructor
 */
HTTP_Client::~HTTP_Client()
{
    // close all active connections
    while (connections_.size()) {
	HTTP_Connection* c = connections_.front();
	delete c;
    }

    DeleteCriticalSection(&crit_sect_);
    InIternetCloseHandle(h_session_);
}


/** 
  Returns the singleton of HTTP_Client
 */
HTTP_Client* HTTP_Client::Instance()
{
    return instance_;
}


/** 
  Create an instance of HTTP_Client
 */
HTTP_Client* HTTP_Client::Create()
{
    if (!instance_) {
	instance_ = new HTTP_Client;
    }
    return Instance();
}

/**
  Insert a connection to the active list
 */
void HTTP_Client::Add_Connection(HTTP_Connection* connection)
{
    connections_.push_back(connection);
}


/**
  Remove a connection from the active list
 */
void HTTP_Client::Remove_Connection(HTTP_Connection* connection)
{
    CTask *task = connection->Get_Task();
    if (task) {
	List_Connection *list = (List_Connection *)task->GetTLSValue(tls_id_);
	list->remove(connection);
    }
    connections_.remove(connection);
}


/** 
  Acquire exclusive access to shared resource
 */
void HTTP_Client::Critical_Section_Lock()
{
    EnterCriticalSection(&crit_sect_);
}

/** 
  Release shared resource
 */
void HTTP_Client::Critical_Section_Unlock()
{
    LeaveCriticalSection(&crit_sect_);
}


/** 
  Initiate a HTTP connection using the specified verb and URL
 */
STATUS HTTP_Client::API_Send_Request(
		CTask* task, 
		HTTP_Connection** handle, 
		const char *verb, 
		const char *url, 
		const char* req_header, 
		const char* req_body,
		DWORD connect_timeout,
		DWORD receive_timeout)

{
    HTTP_Connection* connection = new HTTP_Connection(this, h_session_);
    connection->Task_Attach(task);
    int rc = connection->Send_Request(verb, url, req_header, req_body, connect_timeout, receive_timeout);
    if (rc) {
	delete connection;
	*handle = (HTTP_Connection*)rc;
    } else {
	List_Connection *list = (List_Connection *)task->GetTLSValue(tls_id_);
	list->push_back(connection);
	connection->Task_Set_Return_Code((int*)handle);
	*handle = connection;
    }
    return OK;
}


/** 
  Return HTTP status code, eg.: 200 OK
 */
STATUS HTTP_Client::API_Get_Status_Code(CTask* task, int* status_code, HTTP_Connection* handle)
{
     if (handle->Is_Valid()) *status_code = handle->Get_Status_Code();
     else *status_code = HTTP_ERR_INVALID_HANDLE;
     return OK;
}


/** 
  Return HTTP response length
 */
STATUS HTTP_Client::API_Get_Response_Length(CTask* task, int* response_length, HTTP_Connection* handle)
{
     if (handle->Is_Valid()) *response_length = handle->Get_Response_Length();
     else *response_length = HTTP_ERR_INVALID_HANDLE;
     return OK;
}


/** 
  Retrieve server's response body
 */
STATUS HTTP_Client::API_Read_Response(CTask* task, int* retcode, HTTP_Connection* handle, char *user_buffer)
{
    if (handle->Is_Valid()) {
	int rc = handle->Read_Response(user_buffer, MAXAPPSTRLEN-2);
	if (rc == 0) {
	    handle->Task_Set_Return_Code(retcode);
	    *retcode = 0;
	} else {
	    *retcode = rc;
	}
    } else {
	*retcode = HTTP_ERR_INVALID_HANDLE;
    }

    return OK;
}


/**
  Retrieve header value from response header
 */
STATUS API_CALL HTTP_Client::API_Get_Header(CTask* task, int *retcode, HTTP_Connection* handle, const char* hdr_name, char* hdr_value)
{
    if (handle->Is_Valid()) {
	const char *val;

	int index = 0;
	int total_length = 0;

	*hdr_value = 0;
	val = handle->Get_Header(hdr_name, index);

	while (val && (total_length+strlen(val) < MAXAPPSTRLEN-2)) {
	    if (index > 0) strcat(hdr_value, "\n");
	    strcat(hdr_value, val);
	    total_length += (strlen(val)+1);

	    index++;
	    val = handle->Get_Header(hdr_name, index);
	}

	*retcode = 0;
    } else {
	*retcode = HTTP_ERR_INVALID_HANDLE;
    }

    return OK;
}


STATUS HTTP_Client::API_Close(CTask* task, HTTP_Connection* handle)
{
    if (handle->Is_Valid()) delete handle;
    return OK;
}



/**
  Win32 Internet callback handler: called by WinInet from another thread's context
 */
void CALLBACK HTTP_Client::Http_Callback_Handler(
			HINTERNET h_session, 
			DWORD context, 
			DWORD status, 
			void* status_info, 
			DWORD status_info_len)
{
    static HTTP_Client *manager = Instance();

    switch (status)
    {
    // an asynchronous request has been completed
    case INTERNET_STATUS_REQUEST_COMPLETE:
	{
	    INTERNET_ASYNC_RESULT* async_result = (INTERNET_ASYNC_RESULT* )status_info;
	    Async_Notification *p = new Async_Notification;
	    p->connection_ = (HTTP_Connection*) context;
	    p->result_ = async_result->dwResult;
	    p->error_ = async_result->dwError;

	    manager->Critical_Section_Lock();
	    manager->notifs_.push_back(p);
	    manager->Critical_Section_Unlock();
	}
	break;

    // a connection has been closed, informational
    case INTERNET_STATUS_CONNECTION_CLOSED:
	CLog::Detail(THISMODULE, "0x%08X: Connection closed", context);
	break;

    // informational notification
    case INTERNET_STATUS_REDIRECT:
	CLog::Detail(THISMODULE, "0x%08X: Redirected to %s", context, status_info);
	break;

    // informational notification
    case INTERNET_STATUS_CONNECTING_TO_SERVER:
	CLog::Detail(THISMODULE, "0x%08X: Connecting to %s", context, status_info);
	break;

    // informational notification
    case INTERNET_STATUS_CONNECTED_TO_SERVER:
	CLog::Detail(THISMODULE, "0x%08X: Connection established", context);
	break;

    // not so interesting event, just ignore it
    case INTERNET_STATUS_RESOLVING_NAME:
    case INTERNET_STATUS_NAME_RESOLVED:
    case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
    case INTERNET_STATUS_CLOSING_CONNECTION:
    case INTERNET_STATUS_STATE_CHANGE:
    case INTERNET_STATUS_SENDING_REQUEST:
    case INTERNET_STATUS_REQUEST_SENT:
    case INTERNET_STATUS_RECEIVING_RESPONSE:
    case INTERNET_STATUS_RESPONSE_RECEIVED:
    default:
	return;
    }
}


/** 
  ServiceProvider base class method implementation
 */
bool HTTP_Client::Load()
{
    api_prov_ = new HTTP_API_Provider(this);

    // Change the connection limit of IE (5.0 or higher)
    BOOL rc;
    long max_conn = 64;
    rc = InternetSetOption(NULL, 73 /*INTERNET_OPTION_MAX_CONNS_PER_SERVER*/, &max_conn, sizeof(max_conn));
    rc = InternetSetOption(NULL, 74 /*INTERNET_OPTION_MAX_CONNS_PER_1_0_SERVER*/, &max_conn, sizeof(max_conn));

    // open an asynchronous internet session
    // all handles will derive from this session
    h_session_ = InternetOpen(
	http_agent_.c_str(), 
	PRE_CONFIG_INTERNET_ACCESS,
	NULL, NULL, 
	INTERNET_FLAG_ASYNC);
    if (h_session_ == NULL) {
	CLog::Error(THISMODULE, "Unable to initialize internet session");
	return false;
    }

    // install asynchronous call-back notification handler
    InternetSetStatusCallback(h_session_, Http_Callback_Handler);

    // allocate a TLS_ID to store the list of active HTTP_Connection
    // owned by task
    tls_id_ = Kernel::Instance()->AllocTLS();

    return true;
}


/** 
  ServiceProvider base class method implementation
 */
bool HTTP_Client::Unload()
{
    if (api_prov_) delete api_prov_;
    api_prov_ = NULL;
    InternetCloseHandle(h_session_);
    Kernel::Instance()->FreeTLS(tls_id_);
    return true;
}


/** 
  ServiceProvider base class method implementation
 */
bool HTTP_Client::AttachTask(CTask* task)
{
    // each task keeps a list of its active connection
    List_Connection *list = new List_Connection;
    task->SetTLSValue(tls_id_, (int)list);
    return true;
}


/** 
  ServiceProvider base class method implementation
 */
void HTTP_Client::DetachTask(CTask* task)
{
    // delete all attached connections
    List_Connection *list = (List_Connection *)task->GetTLSValue(tls_id_);
    while (list->size()) {
	HTTP_Connection *conn = list->front();
	list->pop_front();
	delete conn;
    }
    delete list;
}


/** 
  ServiceProvider base class method implementation
 */
bool HTTP_Client::CancelJob(CTask* task)
{
    // just resume the suspended task, the asynch operation will carry on until it is closed
    List_Connection *list = (List_Connection *)task->GetTLSValue(tls_id_);
    for (List_Connection::iterator it = list->begin(); it != list->end(); it++) {
	HTTP_Connection *conn = *it;
	if (conn->user_suspended_) {
	    conn->Task_Resume();
	    if (conn->h_file_) InternetCloseHandle(conn->h_file_);
	    if (conn->h_connect_) InternetCloseHandle(conn->h_connect_);

	    break;
	}
    }
    return true;
}


/** 
  ServiceProvider base class method implementation
 */
void HTTP_Client::HandleEvents()
{
    // check whether there is any completed asynchronous request
    while (1) {

	// the list is shared between this thread and callback
	// so we must ensure mutual exclusion
	Critical_Section_Lock();

	if (notifs_.size() == 0) {
	    Critical_Section_Unlock();
	    return;
	}

	Async_Notification* p = notifs_.front();
	notifs_.pop_front();

	Critical_Section_Unlock();

	// notify the connection of the completion
	if (p->connection_->Is_Valid()) {
	    p->connection_->Asynch_Completed(p->result_, p->error_);
	} else {
	    if (p->error_ != ERROR_INTERNET_OPERATION_CANCELLED)
		CLog::Error(THISMODULE, "0x%08X: Asynchronous notification received for invalid connection", p->connection_);
	}

	delete p;
    }

}


/** 
  ServiceProvider base class method implementation
 */
void HTTP_Client::OnShutdown(bool cancel)
{
}


/** 
  ServiceProvider base class method implementation
 */
bool HTTP_Client::OnShowStatus(String & output)
{
    const String indent(SHOW_STATUS_INDENT);

    output += indent + "Outgoing HTTP Connections" + NEWLINE;
    output += indent + "HTTP Agent:   " + http_agent_ + NEWLINE;
    output += indent + "HTTP Version: " + http_version_ + NEWLINE;
    output += NEWLINE;

    if (connections_.size()) {
	output += indent + "  # URL                                    State      Length  Task" NEWLINE;
	output += indent + "---------------------------------------------------------------------" NEWLINE;
	int line=1;
	List_Connection::iterator it = connections_.begin();
	List_Connection::iterator end = connections_.end();
	while (it != end) {
	    HTTP_Connection *conn = *it++;
	    
	    output += indent;

	    // line number
	    char buffer[128];
	    sprintf(buffer, "%3d ", line++);
	    output += buffer;

	    // URL
	    const char *url;
	    if (conn->State() >= CONN_CONNECTED) {
		url = conn->Get_Connected_Url();
	    } else {
		url = conn->Get_Initial_Url();
	    }

	    // limit the length of displayed URL
	    if (strlen(url) > 38) {
		safe_strcpy(buffer, url, 35);
		strcat(buffer, "... ");
	    } else {
		sprintf(buffer, "%-38s ", url);
	    }
	    output += buffer;

	    // connection state
	    sprintf(buffer, "%-10s ", Connection_State_String(conn->State()));
	    output += buffer;

	    // life duration
	    time_t duration = conn->Get_Life_Time();
	    sprintf(buffer, "%03d:%02d  ", duration/60, duration%60);
	    output += buffer;

	    sprintf(buffer, "%d", conn->user_task_->GetPID());
	    output += buffer;

	    output += NEWLINE;
	}
    } else {
	output += indent + "---- No Active Connection ----" + NEWLINE;
    }

    return true;
}


/** 
  ServiceProvider base class method implementation
 */
bool HTTP_Client::CommitConfiguration()
{
    // do nothing now
    return true;
}


/** 
  ServiceProvider base class method implementation
 */
void HTTP_Client::OnClearCounter()
{
}


//-----------------------------------------------------------------
/**
  HTTP_Connection implementation
 */

/** 
  Object constructor
 */
HTTP_Connection::HTTP_Connection(HTTP_Client* manager, HINTERNET h_session)
{
    manager_ = manager;
    h_session_ = h_session;
    user_task_ = NULL;

    conn_state_ = CONN_NULL;
    user_buffer_ptr_ = NULL;
    user_retcode_ = NULL;
    h_connect_ = h_file_ = NULL;
    response_nread_ = response_length_ = 0;
    user_suspended_ = end_of_file_ = false;
    start_time_ = time(NULL);

    signature_ = VALID_SIGNATURE;

    manager_->Add_Connection(this);
}


/** 
  Object destructor
 */
HTTP_Connection::~HTTP_Connection()
{
    manager_->Remove_Connection(this);
    if (h_file_) InternetCloseHandle(h_file_);
    if (h_connect_) InternetCloseHandle(h_connect_);
    h_connect_ = h_file_ = NULL;
    user_task_ = NULL;
    signature_ = ~VALID_SIGNATURE;
}


/**
  Check validity of a pointer to this object
 */
bool HTTP_Connection::Is_Valid() 
{
    try {
	return (signature_ == VALID_SIGNATURE);
    } catch (...) {
	return false;
    }
}


/** 
  Initiate asynchronous HTTP connection
 */
int HTTP_Connection::Send_Request(const char *verb, const char *url, const char* req_header, const char* req_body, DWORD connect_timeout, DWORD receive_timeout)
{
    char scheme[INTERNET_MAX_SCHEME_LENGTH+1];
    char host_name[INTERNET_MAX_HOST_NAME_LENGTH+1];
    char user_name[INTERNET_MAX_USER_NAME_LENGTH+1];
    char password[INTERNET_MAX_PASSWORD_LENGTH+1];
    char path[INTERNET_MAX_PATH_LENGTH+1];
    char extra_info[INTERNET_MAX_PATH_LENGTH+1];
    URL_COMPONENTS components = { 
	sizeof(components), scheme, sizeof(scheme)-1, INTERNET_SCHEME_UNKNOWN, host_name, sizeof(host_name)-1, 0, user_name, 
	sizeof(user_name)-1, password, sizeof(password)-1, path, sizeof(path)-1, extra_info, sizeof(extra_info)-1
    };

    // decompose URL into its individual components
    if (!InternetCrackUrl(url, 0, ICU_DECODE, &components)) {
	CLog::Error(THISMODULE, user_task_, "Invalid URL specified (%s)", url);
	return HTTP_ERR_INVALID_URL;
    }

    // limit ourself to HTTP and HTTPS schemes
    switch (components.nScheme) {
    case INTERNET_SCHEME_HTTP:
    case INTERNET_SCHEME_HTTPS:
	break;

    default:
	return HTTP_ERR_INVALID_SCHEME;
    }

    // declare the server to which we intend to connect to
    h_connect_ = 
	InternetConnect(
	    h_session_,
	    components.lpszHostName,
	    components.nPort,
	    components.dwUserNameLength ? components.lpszUserName : 0,
	    components.dwPasswordLength ? components.lpszPassword : 0,
	    INTERNET_SERVICE_HTTP,
	    0, (DWORD)this);
    if (h_connect_ == NULL) {
	CLog::Warning(THISMODULE, user_task_, "InternetConnect: %s", Wininet_Error_Text(GetLastError()));
	return HTTP_ERR_CANT_CONNECT;
    }

    // accepted response type
    static const char* access_types[] = {
	"text/*",
	NULL
    };

    // verb must be in capitals
    char norm_verb[64];
    safe_strcpy(norm_verb, verb, sizeof(norm_verb)-1);
    strupr(norm_verb);
    verb_ = norm_verb;
    verb_.length();
    init_url_ = url;

    // declaring the complete verb and path
    strcpy(path, components.lpszUrlPath);
    strcat(path, components.lpszExtraInfo);
    h_file_ = 
	HttpOpenRequest(
	    h_connect_,
	    norm_verb,
	    path,
	    HTTP_DEFAULT_VERSION,
	    NULL,
	    access_types,
	    INTERNET_FLAG_RELOAD |
		INTERNET_FLAG_DONT_CACHE |
		//INTERNET_FLAG_NO_COOKIES |
		INTERNET_FLAG_PRAGMA_NOCACHE |
		INTERNET_FLAG_IGNORE_CERT_CN_INVALID |
		INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
		INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
		INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |
		(components.nScheme == INTERNET_SCHEME_HTTPS ? INTERNET_FLAG_SECURE : 0) |
		0,
	    (DWORD)this);
    if (h_file_ == NULL) {
	CLog::Warning(THISMODULE, user_task_, "HttpOpenRequest: %s", Wininet_Error_Text(GetLastError()));
	return HTTP_ERR_CANT_CONNECT;
    }

    // ignore invalid security certificate
    DWORD secure_flag;
    DWORD secure_len = sizeof(secure_flag);
    InternetQueryOption(h_file_, INTERNET_OPTION_SECURITY_FLAGS, &secure_flag, &secure_len);
    secure_flag |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
    InternetSetOption(h_file_, INTERNET_OPTION_SECURITY_FLAGS, &secure_flag, secure_len);

    // add CONNECT Time Out in msec
    InternetSetOption(h_file_, INTERNET_OPTION_CONNECT_TIMEOUT, &connect_timeout, sizeof(connect_timeout));

    // add RECEIVE Time Out in msec
    InternetSetOption(h_file_, INTERNET_OPTION_RECEIVE_TIMEOUT, &receive_timeout, sizeof(receive_timeout));

    // add additional header into request
    INTERNET_BUFFERS input_buffer;
    memset(&input_buffer, 0, sizeof(input_buffer));
    input_buffer.dwStructSize= sizeof(input_buffer);

    // add body into request
    char header[1024];
    if (strlen(req_body)) {
	input_buffer.lpvBuffer = (void*)req_body;
	input_buffer.dwBufferLength = strlen(req_body);
	sprintf(header, "Content-Length: %d\r\n", input_buffer.dwBufferLength);
    } else {
	header[0] = 0;
    }

    // add user specified header
    strcat(header, req_header);
    input_buffer.lpcszHeader = header;
    input_buffer.dwHeadersLength = strlen(header);

    // now trigger the connection asynchronously
    if (!HttpSendRequestEx(h_file_, &input_buffer, NULL, 0, (DWORD)this)) {
	int error = GetLastError();
	if (error != ERROR_IO_PENDING) {
	    CLog::Warning(THISMODULE, user_task_, "HttpSendRequest: %s", Wininet_Error_Text(error));
	    return HTTP_ERR_CANT_CONNECT;
	}
    }

    // asynchronous request initiated, suspend caller
    Task_Suspend();
    conn_state_ = CONN_SENDING_REQUEST;

    return 0;
};


/**
  Return the value of a header identified by header_name
 */
const char* HTTP_Connection::Get_Header(const char *header_name, unsigned long index)
{
    safe_strcpy(header_value_, header_name, sizeof(header_value_)-1);
    DWORD size = sizeof(header_value_)-1;
    if (!HttpQueryInfo(h_file_, HTTP_QUERY_CUSTOM, header_value_, &size, &index)) {
	return NULL;
    } else {
	header_value_[size] = 0;
	return header_value_;
    }
}

/**
  Display error message, resume task if suspended and delete this instance
 */
void HTTP_Connection::Process_Error(const char *location, unsigned error)
{
    if (error != ERROR_INTERNET_OPERATION_CANCELLED) {
	CLog::Error(THISMODULE, user_task_, "%s (0x%08X): %s: %s", Get_Initial_Url(), this, location, Wininet_Error_Text(error));
    }

    if (user_retcode_) *user_retcode_ = (HTTP_ERR_WINET_BASE - (error - 12000));
    Task_Resume();

    delete this;
}


/** 
  Called by the manager when an asynchronous request completes
 */
bool HTTP_Connection::Asynch_Completed(unsigned result, unsigned error)
{
    if (error) {
	Process_Error("General HTTP error", error);
	return false;
    }

    switch (conn_state_) {

    // request has been completely sent, proceed
    case CONN_SENDING_REQUEST:
	{
	    if (!HttpEndRequest(h_file_, NULL, 0, 0)) {
		int error = GetLastError();
		if (error != ERROR_IO_PENDING) {
		    Process_Error("HttpEndRequest", error);
		    return false;
		}
	    }
	    conn_state_ = CONN_ENDING_REQUEST;
	}
	break;

    // the first valid data is now available to read
    case CONN_ENDING_REQUEST:
	{
	    // store the final URL (we may be redirected)
	    DWORD url_size = sizeof(conn_url_)-1;
	    if (InternetQueryOption(h_file_, INTERNET_OPTION_URL, conn_url_, &url_size)) {
		conn_url_[url_size] = 0;
		CLog::Detail(THISMODULE, user_task_, "0x%08X: Connected to %s", this, conn_url_);
	    }

	    DWORD index = 0;
	    char header[2048];
	    DWORD hdr_size = sizeof(header);
	    if (HttpQueryInfo(h_file_, HTTP_QUERY_RAW_HEADERS_CRLF|HTTP_QUERY_FLAG_REQUEST_HEADERS, header, &hdr_size, &index)) {
		header[hdr_size] = 0;
		CLog::Detail(THISMODULE, user_task_, "0x%08X: Header sent:\n%s", this, header);
	    }

	    hdr_size = sizeof(header);
	    if (HttpQueryInfo(h_file_, HTTP_QUERY_RAW_HEADERS_CRLF, header, &hdr_size, &index)) {
		header[hdr_size] = 0;
		CLog::Detail(THISMODULE, user_task_, "0x%08X: Header received:\n%s", this, header);
	    }

	    DWORD status_size = sizeof(status_code_);
	    HttpQueryInfo(
		h_file_, 
		HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, 
		&status_code_, &status_size, &index);

	    index = 0;
	    DWORD len_size = sizeof(response_length_);
	    if (!HttpQueryInfo(
		    h_file_, 
		    HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER, 
		    &response_length_, &len_size, &index)) {

		int err = GetLastError();
		switch (err) {
		case ERROR_HTTP_HEADER_NOT_FOUND:
		    response_length_ = 0;
		    break;
		default:
		    Process_Error("QueryContentLength", err);
		    return false;
		}
	    }

	    // resume suspended task
	    Task_Resume();
	    conn_state_ = CONN_CONNECTED;
	}
	break;

    case CONN_CONNECTED:
	break;

    // asynchronous read has been completed, resume suspended task
    case CONN_READING_DATA:
	{
	    response_nread_ += inbuff_.dwBufferLength;
	    if (response_length_ && (response_nread_ >= response_length_))
		end_of_file_ = true;

	    Copy_To_User();

	    Task_Resume();
	    conn_state_ = CONN_CONNECTED;
	}
	break;
    }

    return true;
}


/**
  Read data from the network into internal buffer.
  Sometimes the operation can't be completed synchronously
 */
int HTTP_Connection::Read_File_Asynch()
{
    // read into internal buffer
    memset(&inbuff_, 0, sizeof(inbuff_));
    inbuff_.dwStructSize = sizeof(inbuff_);
    memset(&response_buffer_, 0, sizeof(response_buffer_));
    inbuff_.lpvBuffer = response_buffer_;
    inbuff_.dwBufferLength = min(user_buffer_len_, READ_BUFFER_SIZE);

    BOOL rc = InternetReadFileEx(h_file_, &inbuff_, 0, (DWORD)this);
    if (!rc) {
	// if asynchronous read is successful, we should get ERROR_IO_PENDING here
	rc = GetLastError();
	return rc;
    } else {
	// if peer closes socket, we get dwBufferLength = 0
	if (inbuff_.dwBufferLength == 0) {
	    if (response_length_ == 0)
		response_length_ = response_nread_;
	} else {
	    response_nread_ += inbuff_.dwBufferLength;
	}

	if (response_length_ && (response_nread_ >= response_length_))
	    end_of_file_ = true;

	return 0;
    }
}


/**
  Copy data from internal buffer to user's specified buffer
 */
int HTTP_Connection::Copy_To_User()
{
    // copy from data already copied to inbuff
    memcpy(user_buffer_ptr_, response_buffer_, inbuff_.dwBufferLength);
    user_buffer_ptr_[inbuff_.dwBufferLength] = 0;

    return 0;
}


/**
  User's entry point to read data from the network
 */
int HTTP_Connection::Read_Response(char *buffer, unsigned length)
{
    buffer[0] = 0;

    if (End_Of_File())
	return HTTP_ERR_EOF;

    // adjust length
    if (response_length_ && (length > response_length_-response_nread_)) {
	length = response_length_-response_nread_;
    }

    // anything to read asynchronously?
    int rc = 0;
    if (length) {
	user_buffer_len_ = length;
	user_buffer_ptr_ = buffer;

	rc = Read_File_Asynch();
	if (rc == ERROR_IO_PENDING) {
	    conn_state_ = CONN_READING_DATA;
	    Task_Suspend();
	    return 0;
	} else if (rc == 0) {
	    Copy_To_User();
	} else {
	    Process_Error("AsynchRead", rc);
	}

	return rc;
    }

    return 0;
}


/**
  Suspend caller's task
 */
void HTTP_Connection::Task_Suspend()
{
    if (user_task_ && !user_suspended_) 
	user_task_->Sleep(manager_);
    user_suspended_ = true;
}


/**
  Resume suspended task
 */
void HTTP_Connection::Task_Resume()
{
    if (user_task_ && user_suspended_) 
	user_task_->Wakeup();
    user_suspended_ = false;
    user_retcode_ = NULL;
}


//--------------------------------------------------------
HTTP_Client* HTTP_Client::instance_;


//--------------------------------------------------------

/**
  Create the singleton, called from Kernel initialization
 */
void CreateHttpClientManager()
{
    HTTP_Client::Create();
}

#endif	/* #ifdef XGATE_HAS_HTTP_API */
