#pragma once

#include <winsock.h>


#define WS_DEFAULT_PORT	    80

class HTTP_Request;

/*---------------------------------------------------------*
 * base class of Web_Server: to be customized by descendants
 *---------------------------------------------------------*/
class Web_Server
{
public:
    Web_Server();
    Web_Server(short port, const char* base_directory);
    virtual ~Web_Server();

    void Set_Port(short port);

    int Start(unsigned nb_listener);
    int Stop();

    const char* Content_Type_By_Ext(const char* ext);
    const char* Base_Directory();

protected:
    /*---------------------------------------------------------*
     * All these functions must be customized by descendent class
     *---------------------------------------------------------*/
    // must be implemented by the descendant class
    virtual HTTP_Request* Create_HTTP_Request(SOCKET socket);
    virtual void Delete_HTTP_Request(HTTP_Request* request);

private:
    static void* Web_Listener_Thread(void* arg);

    short _port;
    char* _base_directory;
    SOCKET _socket;
};
