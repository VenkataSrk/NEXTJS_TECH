#pragma once

#define MAX_HTTP_RESPONSE_HEADER_LEN	16384

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
enum HTTP_Status_Code
{
    HSC_200_OK = 200,
    HSC_301_Moved_Permanently = 301,
    HSC_400_Bad_Request = 400,
    HSC_401_Unauthorized = 401,
    HSC_403_Forbidden = 403,
    HSC_404_Not_Found = 404,
    HSC_405_Method_Not_Allowed = 405,
    HSC_408_Request_Timeout = 408,
    HSC_413_Request_Entity_Too_Large = 413,
    HSC_500_Internal_Server_Error = 500,
    HSC_501_Not_Implemented = 501,
    HSC_503_Service_Unavailable = 503,
    HSC_505_HTTP_Version_Not_Supported = 505,
};

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
class HTTP_Response_Header
{
public:
    HTTP_Response_Header();
    ~HTTP_Response_Header();

    void Set_Status_Code(HTTP_Status_Code status_code);
    void Set_Redirect_Location(const char* location);
    void Add_Header(const char* header);

    int Get_Header(char* buffer, int len);

private:
    const char* Status_Code_String(HTTP_Status_Code status_code);

    HTTP_Status_Code _status_code;
    char* _redirect_location;

    char additional_headers[MAX_HTTP_RESPONSE_HEADER_LEN];
};
