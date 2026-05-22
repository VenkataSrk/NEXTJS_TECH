/*----------------------------------------------------------------* 
 * Description: Driver for HTTP server
 * Author:      Junanto 21/02/07
 *----------------------------------------------------------------*/

#include "stdafx.h"
#include "kernel.h"
#include "application.h"
#include "task.h"
#include "term.h"
#include "api.h"
#include "http_srv.h"
#include "Web_Server.h"
#include "HTTP_Request.h"
#include "HTTP_Request_Header.h"
#include "Critical_Section.h"


#define THISMODULE		"WSrv"
#define CRLF			"\r\n"
#ifndef NEWLINE
  #define NEWLINE		"\n"
#endif


HTTP_Service_Manager* HTTP_Service_Manager::_instance = 0;

class XG_HTTP_Request;
typedef List<XG_HTTP_Request*> List_HTTP_Request;

static List_HTTP_Request active_requests;

class HTTP_Service_Provider;

// This is sent automatically by xGate if application quits without sending a response
const char no_response_message[] =
    "HTTP/1.1 500 Internal Server Error" CRLF
    "Connection: close" CRLF
    "Server: xGate/2.0 (compatible; MSIE 4.0; )" CRLF
    "Content-Type: text/plain" CRLF
    "" CRLF
    "Application has quit before sending response";

/*---------------------------------------------------------*
 * Base class of HTTP_Request: must be customized by descendants
 *---------------------------------------------------------*/
class XG_HTTP_Request : public HTTP_Request
{
    HTTP_Service_Provider* _service;
    time_t start_time_;

public:
    XG_HTTP_Request(HTTP_Service_Provider* service, Web_Server *web_server, SOCKET socket) :
	HTTP_Request(web_server, socket)
    {
	_service = service;
	response_sent = false;
	task = 0;
	start_time_ = time(0);
    }

    ~XG_HTTP_Request()
    {
	if (task && !response_sent) {
	    CLog::Warning(THISMODULE, task, "Application quit before sending response to the request %s %s", Get_Request_Header()->Method(), Get_Request_Header()->URI());
	    send(Get_Socket(), no_response_message, strlen(no_response_message), 0);
	}
	closesocket(Get_Socket());

	active_requests.remove(this);
	task = 0;
    }

    time_t Get_Life_Time() { return time(0)-start_time_; }

    bool response_sent;
    CTask* task;

protected:
    // Return false if the default behaviour (file server) is to be
    // done. If true is returned, it means the descendant has already
    // handled this request.
    bool Handle_Request();
};

/*----------------------------------------------------------------* 
 * Class XG_Web_Server (derivation from Web_Server)
 *----------------------------------------------------------------*/
class XG_Web_Server : public Web_Server
{
    HTTP_Service_Provider* _service;

public:
    XG_Web_Server(HTTP_Service_Provider* service)
    {
	_service = service;
    }

    ~XG_Web_Server()
    {
    }

    HTTP_Request* Create_HTTP_Request(SOCKET socket)
    {
	return new XG_HTTP_Request(_service, this, socket);
    }

    void Delete_HTTP_Request(HTTP_Request* request)
    {
	// don't delete now, the Web Service will delete it
    }
};

/*----------------------------------------------------------------* 
 * Class HTTP_Service_Provider (managed by HTTP_Service_Manager)
 *----------------------------------------------------------------*/
class HTTP_Service_Provider : public ServiceProvider
{
    enum HTTP_Term_Cmd_Id
    {
	Cmd_Description,
	Cmd_Port,
	Cmd_Thread_Num,
	Cmd_Route,
	Cmd_Enable,
    };

    struct In_Route
    {
	String method;
	String path;
	String app;
	int num_args;
	String args[4];
    };

    typedef List<In_Route*> List_In_Route;

    Cfg_Tree_Group* _cfgTree;
    String _description;
    int _port;
    int _num_thread;
    List_In_Route _active_routes;
    List_HTTP_Request _pending_requests;
    bool _enabled;
    XG_Web_Server* ws;
    Critical_Section cs;

public:
    HTTP_Service_Provider(const char* name) : ServiceProvider(name, false, true, Low_Priority, "HTTP Server", IC_HTTP) 
    {
	_cfgTree = 0;
	_port = 80;
	_num_thread = 1;
	_description = "";
	_enabled = false;
	ws = new XG_Web_Server(this);

	HTTP_Service_Manager::Instance()->Add_Service_To_List(this);
    }

    ~HTTP_Service_Provider()
    {
	HTTP_Service_Manager::Instance()->Remove_Service_From_List(this);

	// clean in-route list
	while (!_active_routes.empty()) {
	    In_Route* r = _active_routes.front();
	    _active_routes.pop_front();
	    delete r;
	}

	if (_enabled) {
	    if (ws->Stop())
		delete ws;
	} else {
	    delete ws;
	}

	// clean pending requests
	Lock l(cs);
	while (!_pending_requests.empty()) {
	    XG_HTTP_Request* r = _pending_requests.front();
	    _pending_requests.pop_front();
	    delete r;
	}

	// and active requests
	while (!active_requests.empty()) {
	    XG_HTTP_Request* r = active_requests.front();
	    delete r;
	}

    }

    /** GetConfigTree: Returns a pointer to the configuration tree of
	interface sub commands
     */
    Cfg_Tree_Group* GetConfigTree() 
    {
	return _cfgTree; 
    }

    /** 
     */
    void Add_Pending_Request(XG_HTTP_Request* req)
    {
	Lock l(cs);
	_pending_requests.push_back(req);
    }

    /** 
     */
    XG_HTTP_Request* Get_Pending_Request()
    {
	Lock l(cs);

	if (_pending_requests.empty())
	    return 0;

	XG_HTTP_Request* req = _pending_requests.front();
	_pending_requests.pop_front();

	return req;
    }


    /** This function will be called by the service framework after 
        instantiation of this class, to allow the instance to initialize
	itself.
     */
    bool Load()
    {
	// create a new local configuration tree
	_cfgTree = new Cfg_Tree_Group(Name(), Name(), HTTP_Service_Manager::Instance()->GetConfigTree());
	_cfgTree->RegisterShowHandler(this);

	// descrition
	Term_Cmd* cmd = new Term_Cmd(this, Cmd_Description, "description", "Specify the description for this service", PrivilegeLevelAdmin, TermModeConfig, false);
	cmd->Add(new Term_Cmd_Arg("arg1", "", CVariantValidator("", "Description for this service"), TermArgOptional));
	RegisterTermCmd(cmd,_cfgTree);

	// port
	cmd = new Term_Cmd(this, Cmd_Port, "port", "Specify port number to be used by HTTP service", PrivilegeLevelAdmin, TermModeConfig, false);
	cmd->Add(new Term_Cmd_Arg("arg1", CVariant(0), CVariantValidator(CVariant(0), "Port number")));
	RegisterTermCmd(cmd,_cfgTree);

	// num-thread
	cmd = new Term_Cmd(this, Cmd_Thread_Num, "num-thread", "Specify the number of serving threads", PrivilegeLevelAdmin, TermModeConfig, false);
	cmd->Add(new Term_Cmd_Arg("arg1", CVariant(0), CVariantValidator(CVariant(0), "Number of serving threads")));
	RegisterTermCmd(cmd,_cfgTree);

	// incoming route
	cmd = new Term_Cmd(this, Cmd_Route, "route", "Specify incoming route for this service", PrivilegeLevelAdmin, TermModeConfig, true);
	CVariantChoiceValidator ctv1("GET","GET method");
	ctv1.Add("POST","POST method");
	cmd->Add(new Term_Cmd_Arg("@0", CVariant(""), ctv1));
	cmd->Add(new Term_Cmd_Arg("@1", CVariant(""), CVariantValidator("", "Path for this route (e.g /service)")));
	CVariantChoiceValidator ctv2("spawn","keyword");
	cmd->Add(new Term_Cmd_Arg("@2", CVariant(""), ctv2));
	cmd->Add(new Term_Cmd_Arg("@3", CVariant(""), CVariantValidator("", "Application name")));
	cmd->Add(new Term_Cmd_Arg("@4", CVariant(""), CVariantValidator("", "Argument 1"), TermArgOptional));
	cmd->Add(new Term_Cmd_Arg("@5", CVariant(""), CVariantValidator("", "Argument 2"), TermArgOptional));
	cmd->Add(new Term_Cmd_Arg("@6", CVariant(""), CVariantValidator("", "Argument 3"), TermArgOptional));
	cmd->Add(new Term_Cmd_Arg("@7", CVariant(""), CVariantValidator("", "Argument 4"), TermArgOptional));
	RegisterTermCmd(cmd,_cfgTree);

	// [no] enable
	cmd = new Term_Cmd(this, Cmd_Enable, "enable", "Enable this service", PrivilegeLevelAdmin, TermModeConfig, true);
	RegisterTermCmd(cmd,_cfgTree);

	return true;
    }

    /** This function will be called by the service framework before it is
        destroyed/deleted, to allow the instance to cleanup itself.
     */
    bool Unload()
    {
	return true;
    }

    /** Show configuration for the specified entry.
	This function will be called by the framework whenever it needs to display the
	specified entry. Previously this instance should tell the entry that it wishes to
	handle displaying the entry by calling entry->RegisterShowHandler().
     */
    STATUS OnShowConfig(
	const Cfg_Tree_Entry* entry,
	String & output, 
	const String & indent, 
	int indent_size) const
    {
	char str[256];

	if (_description != "") output += indent + "description \"" + _description.c_str() + "\"\n";
	output += indent + "port " + itoa(_port, str, 10) + "\n";
	output += indent + "num-thread " + itoa(_num_thread, str, 10) + "\n";

	// iterate routes
	HTTP_Service_Provider* This = (HTTP_Service_Provider*)this;
	for (List_In_Route::iterator it = This->_active_routes.begin(); it != This->_active_routes.end(); it++) {
	    In_Route* r = *it;

	    output += indent + "route " + 
		r->method + " " +
		r->path + " spawn " +
		r->app;

	    for (int i=0; i<r->num_args; i++) {
		output += " " + r->args[i];
	    }

	    output += "\n";
	}

	output += indent + (_enabled ? "" : "no ") + "enable\n";

	return OK;
    }

    /** Show service provider's status.
     */
    virtual bool OnShowStatus(String & output)
    {
	const String indent(SHOW_STATUS_INDENT);
	char buffer[1024];

	output += indent + "Incoming HTTP Connections" + NEWLINE;
	output += indent + "HTTP Server:    " + Name() + NEWLINE;
	sprintf(buffer, "%d", _port);
	output += indent + "Listening Port: " + buffer + NEWLINE;
	sprintf(buffer, "%d", _num_thread);
	output += indent + "Nb Of Threads:  " + buffer + NEWLINE;
	output += indent + "Status:         " + (_enabled ? "enabled" : "disabled") + NEWLINE;
	output += NEWLINE;

	if (active_requests.size()) {
	    output += indent + "  # From           URI                       Length  Task" NEWLINE;
	    output += indent + "---------------------------------------------------------------" NEWLINE;
	    int line=1;
	    List_HTTP_Request::iterator it = active_requests.begin();
	    List_HTTP_Request::iterator end = active_requests.end();
	    while (it != end) {
		XG_HTTP_Request *req = *it++;
		
		output += indent;

		// line number
		sprintf(buffer, "%3d ", line++);
		output += buffer;

		// from IP address
		sockaddr_in sin;
		int len = sizeof(sin);
		if (getpeername(req->Get_Socket(), (sockaddr*)&sin, &len) == 0) {
		    sprintf(buffer, "%-15s", inet_ntoa(sin.sin_addr));
		} else {
		    sprintf(buffer, "*error*        ");
		}
		output += buffer;

		sprintf(buffer, "%s %-30s", req->Get_Request_Header()->Method(), req->Get_Request_Header()->URI());
		buffer[26] = 0;
		output += buffer;

		// life duration
		time_t duration = req->Get_Life_Time();
		sprintf(buffer, "%03d:%02d  ", duration/60, duration%60);
		output += buffer;

		sprintf(buffer, "%d", req->task ? req->task->GetPID() : 0);
		output += buffer;

		output += NEWLINE;
	    }
	} else {
	    output += indent + "---- No Active Connection ----" + NEWLINE;
	}

	return true;
    }

    /**
     */
    In_Route* find_route(const char* method, const char* path)
    {
	for (List_In_Route::iterator it = _active_routes.begin(); it != _active_routes.end(); it++) {
	    In_Route* r = *it;
	    if (strcmpi(method, r->method.c_str()) == 0 && strcmpi(path, r->path.c_str()) == 0)
		return r;
	}
	return 0;
    }

protected:
    /** Terminal command callback
     */
    void DoTermCmd(Term_Session* session, bool no, Term_Cmd* cmd)
    {
	CVariant v1;
	String output;

	switch (cmd->GetId()) {

	case Cmd_Description:
	    cmd->GetArg((unsigned)0)->GetValue(v1);
	    ACQUIRE_SYSTEM_LOCK();
	    _description = (no) ? "" : v1.AsString();
	    RELEASE_SYSTEM_LOCK();
	    break;

	case Cmd_Port:
	    cmd->GetArg((unsigned)0)->GetValue(v1);
	    ACQUIRE_SYSTEM_LOCK();
	    _port = v1.AsInt();
	    if (_port < 1) _port = 1;
	    else if (_port > 65535) _port = 65535;
	    RELEASE_SYSTEM_LOCK();
	    break;

	case Cmd_Thread_Num:
	    cmd->GetArg((unsigned)0)->GetValue(v1);
	    ACQUIRE_SYSTEM_LOCK();
	    _num_thread = v1.AsInt();
	    if (_num_thread < 1) _num_thread = 1;
	    RELEASE_SYSTEM_LOCK();
	    break;

	case Cmd_Route:
	    ACQUIRE_SYSTEM_LOCK();
	    if (no) {
		String method, path;

		cmd->GetArg((unsigned)0)->GetValue(v1);
		method = v1.AsString();

		cmd->GetArg((unsigned)1)->GetValue(v1);
		path = v1.AsString();

		In_Route* r = find_route(method.c_str(), path.c_str());
		if (!r) {
		    if (session) session->SendError("There is no such route\n");
		} else {
		    _active_routes.remove(r);
		    delete r;
		}
	    } else {
		String method, path;

		cmd->GetArg((unsigned)0)->GetValue(v1);
		method = v1.AsString();

		cmd->GetArg((unsigned)1)->GetValue(v1);
		path = v1.AsString();

		In_Route* r = find_route(method.c_str(), path.c_str());
		if (!r) {
		    r = new In_Route;
		    r->method = method;
		    r->path = path;
		    _active_routes.push_back(r);
		}

		cmd->GetArg((unsigned)3)->GetValue(v1);
		r->app = v1.AsString();

		for (int i=0; i<4; i++) {
		    if (cmd->GetArg((unsigned)4+i)->HasValue()) {
			cmd->GetArg((unsigned)4+i)->GetValue(v1);
			r->args[i] = v1.AsString();
		    } else {
			break;
		    }
		}
		r->num_args = i;
	    }
	    RELEASE_SYSTEM_LOCK();
	    break;

	case Cmd_Enable:
	    if (!no == _enabled)
	    	break;
	    if (!no) {
		// start the service
		ws->Set_Port(_port);
		if (ws->Start(_num_thread) != 0) {
		    if (session) session->SendError("Can't start web service\n");
		    CLog::Warning(THISMODULE, "Can't start web service %s\n", Name().c_str());
		} else {
		    _enabled = true;
		}
	    } else {
		// stop the service
		ws->Stop();
		_enabled = false;
	    }
	    break;

	default:
	    break;
	}
    }

    /** HandleEvents: 
     */
    void HandleEvents() 
    {
	// scan pending request and create application
	// based on route
	XG_HTTP_Request* req;
	while ((req = Get_Pending_Request()) != 0) {

	    Request_Header* header = req->Get_Request_Header();

	    char from[32];
	    sockaddr_in sin;
	    int len = sizeof(sin);
	    if (getpeername(req->Get_Socket(), (sockaddr*)&sin, &len) == 0) {
		sprintf(from, "%s", inet_ntoa(sin.sin_addr));
	    } else {
		sprintf(from, "*unknown*");
	    }

	    char body[32];
	    _snprintf(body, sizeof(body)-1, "%s", req->Get_Body());
	    body[sizeof(body)-1];
	    CLog::Detail(THISMODULE, "Incoming HTTP request from %s: %s %s, body:%s", from, header->Method(), header->URI(), body);

	    In_Route* route = find_route(header->Method(), header->URI());

	    // no such route, send error
	    if (!route) {
		CLog::Warning(THISMODULE, "Can't find HTTP route for %s: %s %s", from, header->Method(), header->URI());

		// send an "inexistant file"
		req->Serve_File("404.html", "text/html");
		delete req;
		continue;
	    }

	    // build argument lists
	    List<String> args;
	    for (int i=0; i<route->num_args; i++)
		args.push_back(route->args[i]);

	    // the route is found, spawn application
	    CTask* task = Kernel::Instance()->SpawnTask(route->app, args, TASK_STARTUP_HTTP);
	    if (task) {
		CLog::Detail(THISMODULE, task, "Incoming HTTP assigned");

#ifdef __x86_64__
		task->SetTLSValue(HTTP_Service_Manager::Instance()->Get_TLS(), req);
#else
        task->SetTLSValue(HTTP_Service_Manager::Instance()->Get_TLS(), (int)req);
#endif
		active_requests.push_back(req);
		req->task = task;
	    } else {
		// send an "inexistant file"
		CLog::Detail(THISMODULE, "Cannot create task to serve incoming HTTP request");
		req->Serve_File("404.html", "text/html");
		delete req;
	    }
	}
    }

};


// Return false if the default behaviour (file server) is to be
// done. If true is returned, it means the descendant has already
// handled this request.
bool XG_HTTP_Request::Handle_Request()
{
    _service->Add_Pending_Request(this);
    return true;
}

void url_decode(char* string);

//-----------------------------------------------------------------------------
// HTTP_Service_API
//
class HTTP_Service_API : public CAPIProvider {
public:
    HTTP_Service_API(HTTP_Service_Manager* manager) :
	CAPIProvider(API_PROVIDER_ID_HTTP_SVC, "HttpSvc")
    {
	_manager = manager;
	_tls = manager->Get_TLS();

	if (API_Provider_Manager::Instance()->RegisterProvider(this) != OK)
	    return;

	// string HttpSvcReqGetQuery(key : string)
	RegisterAPI(
	    "HttpSvcReqGetQuery",			// external name
	    0,						// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_STRING,			// return type
	    (CAPIProvider::APIPROC) &HTTP_Service_API::API_Request_GetQuery,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    3,						// number of arguments
	    CArgType("task",BYVAL,CVariant::VT_TASK),
	    CArgType("retval",BYREF,CVariant::VT_RET),
	    CArgType("key",BYVAL,CVariant::VT_STRING));

	// string HttpSvcReqGetForm(key : string)
	RegisterAPI(
	    "HttpSvcReqGetForm",			// external name
	    1,						// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_STRING,			// return type
	    (CAPIProvider::APIPROC) &HTTP_Service_API::API_Request_GetForm,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    3,						// number of arguments
	    CArgType("task",BYVAL,CVariant::VT_TASK),
	    CArgType("retval",BYREF,CVariant::VT_RET),
	    CArgType("key",BYVAL,CVariant::VT_STRING));

	// int HttpSvcRespWrite(text : string)
	RegisterAPI(
	    "HttpSvcRespWrite",				// external name
	    2,						// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_INT,				// return type
	    (CAPIProvider::APIPROC) &HTTP_Service_API::API_Response_Write,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    3,						// number of arguments
	    CArgType("task",BYVAL,CVariant::VT_TASK),
	    CArgType("retval",BYREF,CVariant::VT_RET),
	    CArgType("text",BYVAL,CVariant::VT_STRING));

	// int HttpSvcClose()
	RegisterAPI(
	    "HttpSvcClose",				// external name
	    3,						// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_INT,				// return type
	    (CAPIProvider::APIPROC) &HTTP_Service_API::API_Close,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    2,						// number of arguments
	    CArgType("task",BYVAL,CVariant::VT_TASK),
	    CArgType("retval",BYREF,CVariant::VT_RET));

	// int HttpSvcGetClientAddress()
	RegisterAPI(
	    "HttpSvcGetClientAddress",			// external name
	    4,						// ordinal
	    false,					// asynchronous flag
	    CVariant::VT_STRING,				// return type
	    (CAPIProvider::APIPROC) &HTTP_Service_API::API_GetClientAddress,
	    CAPIDef::CDECL_TYPE,			// calling convention
	    2,						// number of arguments
	    CArgType("task",BYVAL,CVariant::VT_TASK),
	    CArgType("retval",BYREF,CVariant::VT_RET));
    }

    ~HTTP_Service_API() {
    	API_Provider_Manager::Instance()->UnregisterProvider(this);
    }


private:
    HTTP_Service_Manager* _manager;
    int _tls;

    /*---------------------------------------------------------*
     * Get the value from QueryString or Form given a key
     *---------------------------------------------------------*/
    bool get_value(const char* var, const char* str, char* buf)
    {
	if (!var || !str) return false;

	char* searched = new char[strlen(var)+4];
	sprintf(searched, "%s=", var);
	const char* p = strstr(str, searched);
	delete [] searched;
	if (!p) return false;

	if (p == str || *(p-1) == '&') {
	    p += strlen(var);
	    p += 1;

	    const char* q = strchr(p, '&');
	    if (!q) q = (p + strlen(p));
	    unsigned len = (unsigned)(q - p);
	    if (len < POST_VALUE_MAX_LEN) {
		memcpy(buf, p, len);
		buf[len] = 0;
		url_decode(buf);
		return true;
	    } else {
		return false;
	    }
	} else return false;
    }

    STATUS API_CALL API_Request_GetQuery(CTask *task, char* retval, const char* key) 
    {
	*retval = 0;

	XG_HTTP_Request* req = (XG_HTTP_Request*) task->GetTLSValue(_tls);
	if (!req) return OK;

	get_value(key, req->Get_Request_Header()->Query_String(), retval);

	return OK;
    }

    STATUS API_CALL API_Request_GetForm(CTask *task, char* retval, const char* key) 
    {
	*retval = 0;

	XG_HTTP_Request* req = (XG_HTTP_Request*) task->GetTLSValue(_tls);
	if (!req) return OK;

	get_value(key, req->Get_Body(), retval);

	return OK;
    }

    STATUS API_CALL API_Response_Write(CTask *task, int* retval, const char* data) 
    {
	*retval = -1;

	XG_HTTP_Request* req = (XG_HTTP_Request*) task->GetTLSValue(_tls);
	if (!req) return OK;

	if (strlen(data)) send(req->Get_Socket(), data, strlen(data), 0);
	req->response_sent = true;

	*retval = 0;
	
	return OK;
    }

    STATUS API_CALL API_Close(CTask *task, int* retval) 
    {
	*retval = -1;

	XG_HTTP_Request* req = (XG_HTTP_Request*) task->GetTLSValue(_tls);
	if (!req) return OK;

	delete req;
	task->SetTLSValue(_tls, 0);

	*retval = 0;

	return OK;
    }

    STATUS API_CALL API_GetClientAddress(CTask *task, char* retval) 
    {
	memset(retval,0,sizeof(retval));
	*retval = 0;

	XG_HTTP_Request* req = (XG_HTTP_Request*) task->GetTLSValue(_tls);
	if (!req) return OK;

	struct sockaddr_in addressname;
	int addrlen = sizeof(addressname);
	if(!getpeername(req->Get_Socket(), (sockaddr*)&addressname, &addrlen))
	    sprintf(retval, "%s", inet_ntoa(addressname.sin_addr));

	return OK;
    }

};




/**
 */
HTTP_Service_Manager::HTTP_Service_Manager() :
    ServiceProvider("HttpServer",  true,  false, Lowest_Priority)
{
    _api = 0;
}

/**
 */
HTTP_Service_Manager::~HTTP_Service_Manager()
{
}

/** Create a singleton instance of HTTP_Service_Manager
 */
HTTP_Service_Manager* HTTP_Service_Manager::Create()
{
    if (_instance == 0)
	_instance = new HTTP_Service_Manager;

    return _instance;
}

/** Returns the only instance of HTTP_Service_Manager. 
    The service manager must have been instantiated with Create() prior
    to calling this function.
 */
HTTP_Service_Manager* HTTP_Service_Manager::Instance()
{
    return _instance;
}

/** This function will be called whenever a new task is created.
 */
bool HTTP_Service_Manager::AttachTask(CTask *task)
{
    task->SetTLSValue(_tls, 0);
    return true;
}

/** This function will be called whenever a task is about to be deleted.
 */
void HTTP_Service_Manager::DetachTask(CTask *task)
{
    XG_HTTP_Request* req = (XG_HTTP_Request*) task->GetTLSValue(_tls);
    if (req) delete req;
    task->SetTLSValue(_tls, 0);
}

/** This function will be called by the service framework after 
    instantiation of this class, to allow the instance to initialize
    itself.
 */
bool HTTP_Service_Manager::Load()
{
    _tls = Kernel::Instance()->AllocTLS();

    Term_Cmd* cmd = new Term_Cmd(this, 0, "http-server", "Configure HTTP server", PrivilegeLevelAdmin, TermModeConfig, true);
    cmd->Add(new Term_Cmd_Arg("arg1", "", CVariantValidator("", "HTTP server name")));
    RegisterTermCmd(cmd, Kernel::Instance()->RootConfig());

    _cfgTree = new Cfg_Tree_Group("http-server", "HTTP server", Kernel::Instance()->RootConfig());
    
    Kernel::Instance()->RootConfig()->RegisterShowHandler(this);

    _api = new HTTP_Service_API(this);

    return true;
}

/** This function will be called by the service framework before it is
    destroyed/deleted, to allow the instance to cleanup itself.
 */
bool HTTP_Service_Manager::Unload()
{
    // Unregister show handler.
    Kernel::Instance()->RootConfig()->UnregisterShowHandler(this);

    if (_api) delete _api;
    _api = 0;

    return true;
}

/**
 */
HTTP_Service_Provider* HTTP_Service_Manager::Find_Service_By_Name(const char* name)
{
    for (HTTP_Service_List::iterator it = _list.begin(); it != _list.end(); it++) {
	HTTP_Service_Provider* svc = *it;

	if (strcmpi(name, svc->Name().c_str()) == 0)
	    return svc;
    }
    return 0;
}

/**
 */
void HTTP_Service_Manager::Add_Service_To_List(HTTP_Service_Provider* svc)
{
    _list.push_back(svc);
}

/**
 */
void HTTP_Service_Manager::Remove_Service_From_List(HTTP_Service_Provider* svc)
{
    _list.remove(svc);
}


/** Terminal command callback
 */
void HTTP_Service_Manager::DoTermCmd(Term_Session *session, bool no, Term_Cmd *cmd)
{
    ACQUIRE_SYSTEM_LOCK();

    CVariant v1;
    cmd->GetArg((unsigned)0)->GetValue(v1);
    const char* srv_name = v1.AsString();

    // find the requested signalling instance
    HTTP_Service_Provider* service = Find_Service_By_Name(srv_name);

    // This will create if necessary a new instance of IsupSignalling and move the
    // configuration tree to this instance
    if (!no) {

	if (!service) {
	    bool must_create_service = false;
	    if (session->GetStream().IsInteractive()) {
		RELEASE_SYSTEM_LOCK();
		must_create_service = session->PromptYesNo(false, "Create a new HTTP service '%s' (y/n) ?", srv_name);
		ACQUIRE_SYSTEM_LOCK();
	    } else {
		must_create_service = true;
	    }

	    if (must_create_service) {
		service = new HTTP_Service_Provider(srv_name);
		service->Load();
	    }
	}
	if (service) session->SetRoot(service->GetConfigTree());

    } else {
	// This will delete the specified driver
	if (!service) {
	    session->SendError("There is no HTTP service with such name\n");
	    return;
	}

	service->Unload();
	delete service;
    }

    RELEASE_SYSTEM_LOCK();
}

/** Show configuration for the specified entry.
    This function will be called by the framework whenever it needs to display the
    specified entry. Previously this instance should tell the entry that it wishes to
    handle displaying the entry by calling entry->RegisterShowHandler().
 */
STATUS HTTP_Service_Manager::OnShowConfig(
    const Cfg_Tree_Entry* entry,
    String & output, 
    const String & indent, 
    int indent_size) const
{
    String new_indent = indent;
    new_indent.append((size_t)indent_size, ' ');

    HTTP_Service_Manager* This = (HTTP_Service_Manager*)this;
    for (HTTP_Service_List::iterator it = This->_list.begin();
	    it != This->_list.end(); it++) {
	HTTP_Service_Provider* svc = *it;
	output += indent + "http-server " + svc->Name() + "\n";
	svc->OnShowConfig (entry, output, new_indent, indent_size);
	output += new_indent + "top\n\n";
    }

    return OK;
}

/** GetConfigTree: Returns a pointer to the configuration tree of
    interface sub commands
 */
Cfg_Tree_Group* HTTP_Service_Manager::GetConfigTree() 
{
    return _cfgTree; 
}
