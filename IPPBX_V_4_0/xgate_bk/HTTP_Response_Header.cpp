#include <stdio.h>
#include <string.h>

#include "stdafx.h"

#include "HTTP_Response_Header.h"

#define CRLF	"\r\n"

struct HTTP_Status_Code_String
{
    HTTP_Status_Code sc;
    const char* str;
} HSC_string_table[] = {
    { HSC_200_OK, "200 OK" },
    { HSC_301_Moved_Permanently, "301 Moved Permanently" },
    { HSC_400_Bad_Request, "400 Bad Request" },
    { HSC_401_Unauthorized, "401 Unauthorized" },
    { HSC_403_Forbidden, "403 Forbidden" },
    { HSC_404_Not_Found, "404 Not Found" },
    { HSC_405_Method_Not_Allowed, "405 Method Not Allowed" },
    { HSC_408_Request_Timeout, "408 Request Timeout" },
    { HSC_413_Request_Entity_Too_Large, "413 Request Entity Too Large" },
    { HSC_500_Internal_Server_Error, "500 Internal Server Error" },
    { HSC_501_Not_Implemented, "501 Not Implemented" },
    { HSC_503_Service_Unavailable, "503 Service_Unavailable" },
    { HSC_505_HTTP_Version_Not_Supported, "505 HTTP Version Not Supported" },
};

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
HTTP_Response_Header::HTTP_Response_Header()
{
    Set_Status_Code(HSC_200_OK);
    _redirect_location = NULL;
    additional_headers[0] = 0;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
HTTP_Response_Header::~HTTP_Response_Header()
{
    if (_redirect_location) delete _redirect_location;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
const char* HTTP_Response_Header::Status_Code_String(HTTP_Status_Code status_code)
{
    for (int i=0; i<sizeof(HSC_string_table)/sizeof(HSC_string_table[0]); i++) {
	if (status_code == HSC_string_table[i].sc)
	    return HSC_string_table[i].str;
    }

    return "500 Internal Server Error";
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
void HTTP_Response_Header::Set_Status_Code(HTTP_Status_Code status_code)
{
    _status_code = status_code;
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
void HTTP_Response_Header::Set_Redirect_Location(const char* location)
{
    if (_redirect_location) delete _redirect_location;
    _redirect_location = new char[strlen(location)+1];
    strcpy(_redirect_location, location);
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
void HTTP_Response_Header::Add_Header(const char* header)
{
    strcat(additional_headers, header);
    strcat(additional_headers, CRLF);
}

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
int HTTP_Response_Header::Get_Header(char* buffer, int len)
{
    size_t total_len = 0;
    char *p = buffer;
    char aux[1024];
    size_t auxlen = sprintf(aux, "HTTP/1.1 %s" CRLF, Status_Code_String(_status_code));
    strcpy(p, aux);
    p += auxlen;
    total_len += auxlen;

    strcpy(p, additional_headers);
    auxlen = strlen(additional_headers);
    p += auxlen;
    total_len += auxlen;

    // redirected?
    if (_status_code == HSC_301_Moved_Permanently && _redirect_location) {
	auxlen = sprintf(aux, "Location: %s" CRLF, _redirect_location);
	strcpy(p, aux);
	p += auxlen;
	total_len += auxlen;
    }

    strcpy(p, CRLF);
    auxlen = strlen(CRLF);
    p += auxlen;
    total_len += auxlen;

    return (int)total_len;
}
