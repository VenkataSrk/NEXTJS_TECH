#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdafx.h"

#include "HTTP_Request_Header.h"

#define CRLF		"\r\n"

// GET / HTTP/1.1
// Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-shockwave-flash, */*
// Accept-Language: en-gb
// Accept-Encoding: gzip, deflate
// User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.0.3705)
// Host: localhost
// Connection: Keep-Alive

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
Request_Header::Request_Header(void)
{
    _method[0] = 0;
    _version[0] = 0;
    _uri[0] = 0;
    _query_string = 0;
    _file_extension[0] = 0;

    _content_length = 0;
    _content_type[0] = 0;
    _user_agent[0] = 0;
    _host[0] = 0;
    _keep_alive = false;
    _accept[0] = 0;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
Request_Header::~Request_Header(void)
{
}

/*---------------------------------------------------------*
 * URL-decode
 *---------------------------------------------------------*/
static bool is_hex(char c)
{
    if (isdigit(c)) return true;
    c = toupper(c);
    return (c >= 'A' && c <= 'F');
}

/*---------------------------------------------------------*
 * URL-decode
 *---------------------------------------------------------*/
static int hex_to_int(char c)
{
    static const char hex[] = "0123456789ABCDEF";

    const char *p = strchr(hex, toupper(c));
    if (p) return (int)(p - hex);
    else return -1;
}

/*---------------------------------------------------------*
 * URL-decode
 *---------------------------------------------------------*/
void url_decode(char* string)
{
    char* px = string;
    char* py = string;

    while (*px) {
	if (*px == '%' && is_hex(*(px+1)) && is_hex(*(px+2))) {
	    int ascii = hex_to_int(*(px+1))*16 + hex_to_int(*(px+2));
	    *py++ = ascii;
	    px += 3;
	} else if (*px == '+') {
	    *py++ = ' ';
	    px++;
	} else {
	    *py++ = *px++;
	}
    }
    *py = 0;
}

/*---------------------------------------------------------*
 * Find the value of the specified field in the HTTP header
 *---------------------------------------------------------*/
static int get_field_value(const char* header, const char* field_name, char* value, int max_value_len)
{
    *value = 0;

    const char* p = strstr(header, field_name);
    if (!p) return 0;

    // to get to the value, skip the request-header field itself
    p += strlen(field_name);

    // skip ':'
    if (*p == ':') p++;
    else return 0;

    // skip white-spaces
    while (*p == ' ' || *p == '\t') 
	p++;

    // copy the value
    int len = 0;
    while (*p != '\r') {
	*value++ = *p++;
	len++;
    }

    *value = 0;
    return len;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
bool Request_Header::Parse(const char* raw_header)
{
    const char* p = raw_header;
    const char* endp;

    // METHOD
    endp = strchr(p, ' ');
    if (!endp) return false;
    int len = (int)(endp - p);
    memcpy(_method, p, len);
    _method[len] = 0;

    // URI
    p = endp + 1;
    endp = strchr(p, ' ');
    if (!endp) return false;
    len = (int)(endp - p);
    memcpy(_uri, p, len);
    _uri[len] = 0;

    // find embedded query string in URI
    _query_string = strchr(_uri, '?');
    if (_query_string) {
	*_query_string = 0;
	_query_string++;
    }

    // url_decode URI
    url_decode(_uri);

    // find URI extension (.htm/.html/.jpg/...)
    if (strlen(_uri)) {
	for (p = _uri + strlen(_uri) - 1;
	    (p > _uri) && (*p != '.') && (*p != '/');
	    p--);
	if (*p == '.') strcpy(_file_extension, p+1);
    }

    // HTTP VERSION
    p = endp + 1;
    endp = strstr(p, CRLF);
    if (!endp) return false;
    len = (int)(endp - p);
    memcpy(_version, p, len);
    _version[len] = 0;

    // skip the 1st request line
    p = endp + 2;

    // get field-requests
    char value[2048];
    len = get_field_value(p, "Accept", value, sizeof(value)-1);
    strcpy(_accept, value);

    len = get_field_value(p, "Host", value, sizeof(value)-1);
    strcpy(_host, value);

    len = get_field_value(p, "User-Agent", value, sizeof(value)-1);
    strcpy(_user_agent, value);

    len = get_field_value(p, "Content-Length", value, sizeof(value)-1);
    _content_length = atoi(value);

    len = get_field_value(p, "Connection", value, sizeof(value)-1);
    _keep_alive = strcmpi(value, "keep-alive") == 0;

    len = get_field_value(p, "Content-Type", value, sizeof(value)-1);
    strcpy(_content_type, value);

    return true;
}
