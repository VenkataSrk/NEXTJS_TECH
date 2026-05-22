#include <io.h>
#include <stdio.h>
#include <string.h>

#include "stdafx.h"
#include "log.h"

#include "HTTP_Request.h"
#include "HTTP_Response_Header.h"


#define CRLF			"\r\n"
#define TCP_READ_TIMEOUT	10	    // in seconds
#define THISMODULE		"WSrv"


const char* default_response_404 = "<html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested file was not found on this server.</p></body></html>";


/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
HTTP_Request::HTTP_Request(Web_Server *web_server, SOCKET socket)
{
    _socket = socket;
    _web_server = web_server;
    _body = NULL;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
HTTP_Request::~HTTP_Request()
{
    if (_body) delete _body;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
SOCKET HTTP_Request::Get_Socket()
{
    return _socket;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
Web_Server* HTTP_Request::Get_Web_Server()
{
    return _web_server;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
const char* HTTP_Request::Get_Header()
{
    return _header;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
const char* HTTP_Request::Get_Body()
{
    return _body;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
Request_Header* HTTP_Request::Get_Request_Header()
{
    return &_request_hdr;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
bool HTTP_Request::Serve_Request()
{
    struct sockaddr_in sin;
    int len = sizeof(sin);
    getpeername(_socket, (sockaddr*)&sin, &len);

    if (Read_Header() != 0) {
	CLog::Debug(THISMODULE, "*** Invalid HTTP request received from: %s\n", inet_ntoa(sin.sin_addr));
	return true;
    }

    if (!_request_hdr.Parse(_header)) {
	CLog::Debug(THISMODULE, "*** Invalid HTTP header received from: %s\n%s\n", inet_ntoa(sin.sin_addr), _header);
	return true;
    }

    // does the request contain body?
    if (_request_hdr.Content_Length()) {
	if (Read_Body(_request_hdr.Content_Length()) != 0) {
	    CLog::Debug(THISMODULE, "*** Invalid HTTP body received from: %s\n%s\n", inet_ntoa(sin.sin_addr), _body);
	    return true;
	}
    }
    
    CLog::Debug(THISMODULE, "%s: %s %s\n", inet_ntoa(sin.sin_addr), _request_hdr.Method(), _request_hdr.URI());

    if (!Handle_Request()) {
	const char* ext = _request_hdr.File_Extension();
	if (ext && strlen(ext)) {
	    // extension is specified, serve file
	    char fname[1024];
	    sprintf(fname, "%s%s", _web_server->Base_Directory(), _request_hdr.URI());
	    Serve_File(fname, _web_server->Content_Type_By_Ext(ext));
	} else {
	    // no extension is specified, serve default file (index.html)
	    char fname[1024];
	    sprintf(fname, "%s%s/index.html", _web_server->Base_Directory(), _request_hdr.URI());
	    Serve_File(fname, _web_server->Content_Type_By_Ext("html"));
	}
	return true;
    }

    return false;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
int HTTP_Request::Serve_File(const char* file_name, const char* content_type)
{
    if (!content_type) content_type = "text/plain";

    bool text_mode = strncmp(content_type, "text", 4) == 0;

    // try to open the file
    FILE* f;
    f = fopen(file_name, "rb");

    int len;
    char aux[1024];
    char buf[65536];
    HTTP_Response_Header resp_hdr;
    if (!f) {
	// not found
	resp_hdr.Set_Status_Code(HSC_404_Not_Found);
	resp_hdr.Add_Header("Content-Type: text/html");
	sprintf(aux, "Content-Length: %d", strlen(default_response_404));
	resp_hdr.Add_Header(aux);

	len = resp_hdr.Get_Header(buf, sizeof(buf)-1);
	send(_socket, buf, len, 0);
	send(_socket, default_response_404, (int)strlen(default_response_404), 0);
	return -1;
    }

    // serve the request
    sprintf(aux, "Content-Type: %s", content_type);
    resp_hdr.Add_Header(aux);

    // get the size of served file
    fseek(f, 0L, SEEK_END);
    len = ftell(f);
    sprintf(aux, "Content-Length: %d", len);
    resp_hdr.Add_Header(aux);

    // send the header first
    len = resp_hdr.Get_Header(buf, sizeof(buf)-1);
    send(_socket, buf, len, 0);

    // send the file
    fseek(f, 0L, SEEK_SET);
    while (!feof(f)) {
	len = (int)fread(buf, 1, (int)sizeof(buf), f);
	send(_socket, buf, len, 0);
    }

    fclose(f);

    return 0;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
int HTTP_Request::Read_Header()
{
    char header_terminator[] = CRLF CRLF;
    char* phdr = header_terminator;

    // read until all request header part is received (CRLF CRLF)
    // or timeout
    int i=0;
    _header[0] = 0;
    while (i<MAX_HTTP_REQUEST_HEADER_LEN) {
	struct timeval tv = { TCP_READ_TIMEOUT, 0 };

	fd_set fs;
	FD_ZERO(&fs);
	FD_SET(_socket, &fs);
	int rc = select(1, &fs, NULL, NULL, &tv);

	if (rc == 0) {
	    // time out
	    return 1;
	} else if (rc != 1) {
	    // error
	    return -1;
	}

	// data available, read it until CRLF+CRLF is found
	u_long nbio;
	if (ioctlsocket(_socket, FIONREAD, &nbio) != 0 || nbio == 0) {
	    return -1;
	}

	bool header_found = false;
	while (!header_found && nbio-- && i<MAX_HTTP_REQUEST_HEADER_LEN) {

	    // read a character
	    char c;
	    if (recv(_socket, &c, 1, 0) != 1)
		return -1;

	    _header[i++] = c;

	    // see if we have found the end of header
	    if (*phdr == c) {
		phdr++;
		if (*phdr == 0) header_found = true;
	    } else {
		phdr = header_terminator;
	    }
	}

	if (header_found) break;
    }

    _header[i] = 0;
    //*** CLog::Debug(THISMODULE, "%s", _header);

    return 0;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
int HTTP_Request::Read_Body(unsigned content_length)
{
    _body = new char[content_length+1];
    char* p = _body;

    while (content_length) {
	struct timeval tv = { TCP_READ_TIMEOUT, 0 };

	fd_set fs;
	FD_ZERO(&fs);
	FD_SET(_socket, &fs);
	int rc = select(1, &fs, NULL, NULL, &tv);

	if (rc == 0) {
	    // time out
	    return 1;
	} else if (rc != 1) {
	    // error
	    return -1;
	}

	// data available, read it until the number of bytes
	// as specified by Content-Length is reached
	u_long nbio;
	if (ioctlsocket(_socket, FIONREAD, &nbio) != 0 || nbio == 0)
	    return -1;

	int to_read = min(nbio, content_length);
	int len = recv(_socket, p, to_read, 0);
	if (len != to_read)
	    return -1;

	content_length -= len;
	p += len;
    }

    *p = 0;
    //*** CLog::Debug(THISMODULE, "%s" CRLF CRLF, _body);

    return 0;
}
