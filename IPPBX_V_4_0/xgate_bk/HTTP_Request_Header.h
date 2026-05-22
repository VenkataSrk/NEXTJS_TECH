#pragma once

#define MAX_FIELD_LEN	2048

/*---------------------------------------------------------*
 *
 *---------------------------------------------------------*/
class Request_Header
{
public:
    Request_Header(void);
    ~Request_Header(void);

    bool Parse(const char* raw_header);

    const char* Method() { return _method; }
    const char* Version() { return _version; }
    const char* URI() { return _uri; }
    const char* Query_String() { return _query_string; }
    const char* File_Extension() { return _file_extension; }

    const char* User_Agent() { return _user_agent; }
    const char* Host() { return _host; }
    const char* Accept() { return _accept; }
    int Content_Length() { return _content_length; }
    const char* Content_Type() { return _content_type; }
    bool Connection_Keep_Alive() { return _keep_alive; }

private:
    char _method[MAX_FIELD_LEN+1];
    char _version[MAX_FIELD_LEN+1];
    char _uri[MAX_FIELD_LEN+1];
    char* _query_string;
    char _file_extension[MAX_FIELD_LEN+1];

    char _user_agent[MAX_FIELD_LEN+1];
    char _host[MAX_FIELD_LEN+1];
    char _accept[MAX_FIELD_LEN+1];
    int _content_length;
    char _content_type[MAX_FIELD_LEN+1];
    bool _keep_alive;
};
