#pragma once

#include "Web_Server.h"
#include "HTTP_Request_Header.h"


#define MAX_HTTP_REQUEST_HEADER_LEN	16384
#define POST_VALUE_MAX_LEN		1024



/*---------------------------------------------------------*
 * Base class of HTTP_Request: must be customized by descendants
 *---------------------------------------------------------*/
class HTTP_Request
{
public:
    HTTP_Request(Web_Server *web_server, SOCKET socket);
    virtual ~HTTP_Request();

    // return true if socket must be closed by the caller
    bool Serve_Request();

    Request_Header* Get_Request_Header();
    const char* Get_Header();
    const char* Get_Body();
    SOCKET Get_Socket();
    int Serve_File(const char* file_name, const char* content_type);

protected:
    Web_Server* Get_Web_Server();

    /*---------------------------------------------------------*
     * All these functions must be customized by descendent class
     *---------------------------------------------------------*/
    // Return false if the default behaviour (file server) is to be
    // done. If true is returned, it means the descendant has already
    // handled this request.
    virtual bool Handle_Request() { return false; }

private:
    int Read_Header();
    int Read_Body(unsigned content_length);

    SOCKET _socket;
    Web_Server* _web_server;
    Request_Header _request_hdr;
    char _header[MAX_HTTP_REQUEST_HEADER_LEN+1];
    char *_body;
};
