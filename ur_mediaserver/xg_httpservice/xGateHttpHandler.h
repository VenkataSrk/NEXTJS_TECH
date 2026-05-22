#ifndef _XGATE_HTTP_HANDLER_H
#define _XGATE_HTTP_HANDLER_H
//system includes
#include <vector>
//ace includes
#include <ace/Mutex.h>
#include <ace/Guard_T.h>

//curl includes
#include <curl/curl.h>

//local inludes
#include "xGateUtil.h"
#include "HttpServiceDefine.h"

/* xGateHttpHandler class - Start
*/
class xGateHttpHandler
{
  public:
    static xGateHttpHandler* getInstance();
    static void deleteInstance();
    bool doHttpRequest(HttpRequestInfo &httpReqInfo);
    bool getHttpResponse();
    static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *data);
    std::string currentTime();
  private:
    xGateHttpHandler();
    ~xGateHttpHandler();
    xGateHttpHandler(const xGateHttpHandler& rhs);
    xGateHttpHandler& operator=(const xGateHttpHandler& rhs);

    //member variables
    static xGateHttpHandler *m_instance;
    static ACE_Mutex m_mutex;
    CURLM *m_multiHandle;
    int m_stillRunning; 

    static int m_sentCount;
    static int m_recvCount;

    //member functions
    bool init();
    bool deInit();
    bool initEasyHandle(HttpRequestInfo *httpReqInfo);
    bool setHttpContentType(CURL *easyHandle, HttpRequestInfo *httpReqInfo);
    bool fillHttpStatusInfo(HttpRequestInfo *httpReqInfo);
    string getMcodeStr(CURLMcode code);
};
/* xGateHttpHandler class - End
*/
#endif
