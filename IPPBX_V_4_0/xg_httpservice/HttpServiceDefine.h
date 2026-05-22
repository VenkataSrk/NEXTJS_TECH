#ifndef _HTTP_SERVICE_DEFINE_H
#define _HTTP_SERVICE_DEFINE_H
typedef enum {
  EN_CONTENT_TYPE_UNKNOWN=0,
  EN_CONTENT_TYPE_TEXT=1,
  EN_CONTENT_TYPE_JSON=2,
  EN_CONTENT_TYPE_XML=3,
} HttpContentType;

typedef enum {
  EN_MULTIPART_CONTENT_TYPE_UNKNOWN=0,
  EN_MULTIPART_CONTENT_TYPE_DATA=1,
  EN_MULTIPART_CONTENT_TYPE_FILE=2,
} MultipartContent;

typedef enum {
  EN_MULTIPART_FIELD_UNKNOWN=0,
  EN_MULTIPART_FIELD_NAME=1,
  EN_MULTIPART_FIELD_VALUE=2,
} MultipartField;

typedef enum {
  EN_HTTP_METHOD_UNKNOWN=0,
  EN_HTTP_METHOD_GET=1,
  EN_HTTP_METHOD_POST=2,
  EN_HTTP_METHOD_PUT=3,
  EN_HTTP_METHOD_MULTIPART=4,
} HttpReqMethod;

typedef enum
{
  EN_HTTP_EVENT_UNKNOWN=0,
  EN_HTTP_EVENT_GET=1,
  EN_HTTP_EVENT_POST=2,
  EN_HTTP_EVENT_PUT=3,
  EN_HTTP_EVENT_MULTIPART=4,
} HttpServiceEvent;

typedef enum
{
  EN_HTTP_STATUS_UNKNOWN=0,
  EN_HTTP_STATUS_SUCCESS=1,
  EN_HTTP_STATUS_FAILED=2,
} HttpResponseStatus;

/* HttpRequestInfo class - Start
*/
class HttpRequestInfo
{
  public:
    HttpRequestInfo() : m_easyHandle(NULL), m_url(""), m_data(""), 
    m_requestId(""), m_contextId(""), m_output(""), m_statusCode(0),
    m_contentType(EN_CONTENT_TYPE_JSON), 
    m_httpReqMethod(EN_HTTP_METHOD_UNKNOWN),
    m_httpRespStatus(EN_HTTP_STATUS_UNKNOWN) {
      memset(m_err, '\0', CURL_ERROR_SIZE * sizeof(char)); 
      //generate and assign request id for each request
      assignRequestId();
    }

    ~HttpRequestInfo()
    {
      if(m_easyHandle) {
        curl_easy_cleanup(m_easyHandle);
        m_easyHandle = NULL;
      }
    }

    HttpRequestInfo(const HttpRequestInfo& rhs)
    {
      m_easyHandle = rhs.m_easyHandle;
      m_url = rhs.m_url;
      m_data = rhs.m_data;
      m_output = rhs.m_output;
      m_requestId = rhs.m_requestId;
      m_contextId = rhs.m_contextId;
      m_contentType = rhs.m_contentType;
      m_httpReqMethod = rhs.m_httpReqMethod;
      m_httpRespStatus = rhs.m_httpRespStatus;
	  //m_requestorModule = rhs.m_requestorModule;
      m_statusCode = rhs.m_statusCode;
      strncpy(m_err, rhs.m_err, sizeof(rhs.m_err));
    }

    HttpRequestInfo& operator=(const HttpRequestInfo& rhs)
    {
      m_easyHandle = rhs.m_easyHandle;
      m_url = rhs.m_url;
      m_data = rhs.m_data;
      m_output = rhs.m_output;
      m_requestId = rhs.m_requestId;
      m_contextId = rhs.m_contextId;
      m_contentType = rhs.m_contentType;
      m_httpReqMethod = rhs.m_httpReqMethod;
      m_httpRespStatus = rhs.m_httpRespStatus;
	  //m_requestorModule = rhs.m_requestorModule;
      m_statusCode = rhs.m_statusCode;
      strncpy(m_err, rhs.m_err, sizeof(rhs.m_err));
    }

    void assignRequestId()
    {
      static const string charList = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
      unsigned int reqId_len =5;
      unsigned int iseed = (unsigned int)time(NULL);
      srand (iseed);
      for(int i = 0; i < reqId_len; i++) {
        m_requestId += charList [rand() % charList.size()];
      }
    }

    CURL *m_easyHandle;
    string m_url;
    string m_data;
    string m_output;
    string m_requestId;
    string m_contextId;
    //IURDefines::MODULE_ID m_requestorModule;
    HttpContentType m_contentType;
    HttpReqMethod m_httpReqMethod;
    HttpResponseStatus m_httpRespStatus;
    unsigned int m_statusCode;

    vector<pair<string, string>> m_multipartFileInfo;
    vector<pair<string, string>> m_multipartDataInfo;
    char m_err[CURL_ERROR_SIZE];
};
/* HttpRequestInfo class - End
*/
#endif
