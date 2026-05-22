#ifndef _XGATE_HTTP_HANDLER_H
#define _XGATE_HTTP_HANDLER_H
//system includes

//ace includes
#include <ace/Mutex.h>
#include <ace/Guard_T.h>

//curl includes
#include <curl/curl.h>

//local inludes
#include "xGateUtil.h"

typedef enum {
  EN_CONTENT_TYPE_UNKNOWN=0,
  EN_CONTENT_TYPE_TEXT=1,
  EN_CONTENT_TYPE_JSON=2,
  EN_CONTENT_TYPE_XML=3,
}HttpContentType;

typedef enum {
  EN_REQUESTOR_TYPE_UNKNOWN=0,
  EN_REQUESTOR_TYPE_DBM_DISPATCHER=1,
  EN_REQUESTOR_TYPE_BUSINESS_PROCESSOR=2,
}HttpRequestorType;

/* HttpRequestInfo class - Start
*/
class HttpRequestInfo
{
  public:
    HttpRequestInfo();
    ~HttpRequestInfo();
    HttpRequestInfo(const HttpRequestInfo& rhs);
    HttpRequestInfo& operator=(const HttpRequestInfo& rhs);

    CURL *m_easyHandle;
    string m_url;
    string m_data;
    string m_output;
    string m_requestId;
    HttpContentType m_contentType;
    HttpRequestorType m_requestorType;
    char m_err[CURL_ERROR_SIZE];
};
/* HttpRequestInfo class - End
*/

/* xGateHttpHandler class - Start
*/
class xGateHttpHandler
{
  public:
    static xGateHttpHandler* getInstance();
    static void deleteInstance();
    bool doHttpRequest(HttpRequestInfo &httpReqInfo);
    HttpRequestInfo * getHttpResponse();
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
    string getMcodeStr(CURLMcode code);
#if 0 //TODO: temporarily commented 
    bool post(HttpRequestInfo *httpReqInfo);
#endif
};
/* xGateHttpHandler class - End
*/
#endif
