#include <stdio.h>
#include <string.h>

#include "stdafx.h"
#include "log.h"
#include "thread_process.h"

#include "Web_Server.h"
#include "HTTP_Request.h"


#define THISMODULE	    "WSrv"


/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
struct Content_Type_By_Ext_Map
{
    const char* ext;
    const char* content_type;
};

Content_Type_By_Ext_Map content_type_by_ext_map[] = {
    { "htm", "text/html" },
    { "html", "text/html" },
    { "shtml", "text/html" },
    { "txt", "text/plain" },
    { "asc", "text/plain" },
    { "css", "text/css" },
    { "bin", "application/octet-stream" },
    { "zip", "application/octet-stream" },
    { "exe", "application/octet-stream" },
    { "dll", "application/octet-stream" },
    { "swf", "application/x-shockwave-flash" },
    { "doc", "application/msword" },
    { "pdf", "application/pdf" },
    { "jpeg", "image/jpeg" },
    { "jpg", "image/jpeg" },
    { "jpe", "image/jpeg" },
    { "png", "image/png" },
    { "xhtml", "application/xhtml+xml" },
    { "xht", "application/xhtml+xml" },
    { "xml", "application/xml" },
    { "xsl", "application/xml" },
    { "dtd", "application/xml-dtd" },
    { "xslt", "application/xslt+xml" },
    { "zip", "application/zip" },
    { "au", "audio/basic" },
    { "snd", "audio/basic" },
    { "mid", "audio/midi" },
    { "midi", "audio/midi" },
    { "mpga", "audio/mpeg" },
    { "mp2", "audio/mpeg" },
    { "mp3", "audio/mpeg" },
    { "aif", "audio/x-aiff" },
    { "aiff", "audio/x-aiff" },
    { "aifc", "audio/x-aiff" },
    { "ram", "audio/x-pn-realaudio" },
    { "rm", "audio/x-pn-realaudio" },
    { "rpm", "audio/x-pn-realaudio-plugin" },
    { "ra", "audio/x-realaudio" },
    { "wav", "audio/x-wav" },
    { "bmp", "image/bmp" },
    { "tiff", "image/tiff" },
    { "tif", "image/tiff" },
    { "ico", "image/x-icon" },
    { "rtf", "text/rtf" },
    { "mpeg", "video/mpeg" },
    { "mpg", "video/mpeg" },
    { "mpe", "video/mpeg" },
    { "avi", "video/x-msvideo" },
    { "qt", "video/quicktime" },
    { "mov", "video/quicktime" },
    { "ogg", "application/ogg" },
    { "ai", "application/postscript" },
    { "eps", "application/postscript" },
    { "ps", "application/postscript" },
};

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
Web_Server::Web_Server()
{
    _port = WS_DEFAULT_PORT;
    _socket = INVALID_SOCKET;

    _base_directory = new char[3];
    strcpy(_base_directory, "./");
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
Web_Server::Web_Server(short port, const char* base_directory)
{
    _socket = INVALID_SOCKET;
    _port = port;

    // make a copy of base directory and ensure that it is terminated by '\'
    size_t len = strlen(base_directory);
    _base_directory = new char[len+2];
    strcpy(_base_directory, base_directory);
    if (_base_directory[len-1] != '\\' || _base_directory[len-1] != '/') {
	_base_directory[len] = '\\';
	_base_directory[len+1] = 0;
    }
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
Web_Server::~Web_Server(void)
{
    Stop();
    delete _base_directory;
    _base_directory = NULL;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
int Web_Server::Stop()
{
    if (_socket != INVALID_SOCKET) {
	closesocket(_socket);
	_socket = INVALID_SOCKET;
    }

    return 0;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
void Web_Server::Set_Port(short port)
{
    _port = port;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
int Web_Server::Start(unsigned nb_listener)
{
    unsigned i;

    if (!nb_listener)
	return -1;

    int status = 0;

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket == INVALID_SOCKET)
	return -2;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);

    if (bind(_socket, (sockaddr*)&addr, sizeof(addr))) {
	status = -3;
	goto exit;
    }

    if (listen(_socket, 5)) {
	status = -4;
	goto exit;
    }

    for (i=0; i<nb_listener; i++) {
	SPAWN_THREAD(Web_Listener_Thread, this, 0, 0, 0);
    }

exit:
    if (status) {
	closesocket(_socket);
	_socket = INVALID_SOCKET;
    }

    return status;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
void* Web_Server::Web_Listener_Thread(void *arg)
{
    Web_Server* instance = (Web_Server*)arg;

    // wait for a client to come and serve it
    for (;;) {
	struct sockaddr_in addr;
	int len = sizeof(addr);
	SOCKET client;
	client = accept(instance->_socket, (sockaddr*)&addr, &len);
	if (client == INVALID_SOCKET)
	    return 0;

	CLog::Debug(THISMODULE, "[0x%08x] Processing HTTP request from: %s", instance, inet_ntoa(addr.sin_addr));

	// a new client is connected
	// prepare an argument for the new thread which will serve this request
	HTTP_Request *request = instance->Create_HTTP_Request(client);
	bool must_close_socket = true;
	if (request) {
	    must_close_socket = request->Serve_Request();
	    instance->Delete_HTTP_Request(request);
	}

	if (must_close_socket) closesocket(client);

	CLog::Debug(THISMODULE, "[0x%08x] Request from: %s done", instance, inet_ntoa(addr.sin_addr));
    }

    return 0;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
HTTP_Request* Web_Server::Create_HTTP_Request(SOCKET client)
{
    return new HTTP_Request(this, client);
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
void Web_Server::Delete_HTTP_Request(HTTP_Request* request)
{
    delete request;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
const char* Web_Server::Content_Type_By_Ext(const char* ext)
{
    for (size_t i=0; i<sizeof(content_type_by_ext_map)/sizeof(content_type_by_ext_map[0]); i++) {
	if (strcmpi(ext, content_type_by_ext_map[i].ext) == 0)
	    return content_type_by_ext_map[i].content_type;
    }
    return "text/plain";
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
const char* Web_Server::Base_Directory()
{
    return _base_directory;
}
