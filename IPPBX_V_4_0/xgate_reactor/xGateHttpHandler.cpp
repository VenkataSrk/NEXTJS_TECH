//local includes
#include "xglog.h"
#include "xGateHttpHandler.h"
#include "xGateDbmProcessor.h"

#define THISMODULE "HttpHandler"

#define MAX_HTTP_CON 200

int xGateHttpHandler::m_sentCount=0;
int xGateHttpHandler::m_recvCount=0;

/* HttpRequestInfo class - Start
*/
HttpRequestInfo::HttpRequestInfo() : m_easyHandle(NULL), m_url(""), m_data(""),
  m_requestId(""), m_output(""), m_contentType(EN_CONTENT_TYPE_JSON),
  m_requestorType(EN_REQUESTOR_TYPE_DBM_DISPATCHER)
{
}

HttpRequestInfo::~HttpRequestInfo()
{
  if(m_easyHandle) {
    curl_easy_cleanup(m_easyHandle);
    m_easyHandle = NULL;
  }
}

HttpRequestInfo::HttpRequestInfo(const HttpRequestInfo& rhs)
{
  m_easyHandle = rhs.m_easyHandle;
  m_url = rhs.m_url;
  m_data = rhs.m_data;
}

HttpRequestInfo& HttpRequestInfo::operator=(const HttpRequestInfo& rhs)
{
  m_easyHandle = rhs.m_easyHandle;
  m_url = rhs.m_url;
  m_data = rhs.m_data;
}
/* HttpRequestInfo class - End
*/

/* xGateHttpHandler class - Start
*/
xGateHttpHandler* xGateHttpHandler::m_instance = NULL;
ACE_Mutex xGateHttpHandler::m_mutex;

xGateHttpHandler::xGateHttpHandler() : 
  m_multiHandle(NULL), m_stillRunning(0)
{
}

xGateHttpHandler::~xGateHttpHandler()
{
}

xGateHttpHandler::xGateHttpHandler(const xGateHttpHandler& rhs)
{
}

xGateHttpHandler& xGateHttpHandler::operator=(const xGateHttpHandler& rhs)
{
}

xGateHttpHandler * xGateHttpHandler::getInstance(void)
{
  if(m_instance == NULL) {
    ACE_Guard<ACE_Mutex> instantiationGuard(m_mutex);
    if(m_instance == NULL) {
      m_instance = new xGateHttpHandler();
      if(!m_instance->init()) {
        XGLOG_ERROR( "http-handler initialization failed !");
        delete m_instance;
        m_instance = NULL;
      } else {
      XGLOG_INFO( "http-handler initialized successfully !");
      }
    }
  }
  return m_instance;
}

void xGateHttpHandler::deleteInstance(void)
{
  if(m_instance != NULL) {
    ACE_Guard<ACE_Mutex> instantiationGuard(m_mutex);
    if(m_instance != NULL) {
      m_instance->deInit();
      delete m_instance;
      m_instance = NULL;
    XGLOG_INFO( "http-handler de-initialized successfully !");
    }
  }
}

bool xGateHttpHandler::init()
{
  m_multiHandle = curl_multi_init();
  if(!m_multiHandle) {
    XGLOG_ERROR( "curl_multi_init failed !");
    return false;
  }
  curl_multi_setopt(m_multiHandle, CURLMOPT_MAX_TOTAL_CONNECTIONS, MAX_HTTP_CON);
  curl_multi_setopt(m_multiHandle, CURLMOPT_MAX_HOST_CONNECTIONS, 6L);
  return true;
}

bool xGateHttpHandler::deInit()
{
  if(m_multiHandle) {
    CURLMcode mcode = curl_multi_cleanup(m_multiHandle);
    if(mcode != CURLM_OK) {
      XGLOG_ERROR( "curl_multi_cleanup failed. got %s error !", getMcodeStr(mcode));
    }
    m_multiHandle = NULL; 
  }
  return true;
}

bool xGateHttpHandler::doHttpRequest(HttpRequestInfo *httpReqInfo)
{
  CURLMcode mcode;
  CURLcode res;
  //step1: create and init new curl easy handle for this http request
  if(!initEasyHandle(httpReqInfo)) {
    XGLOG_ERROR( "doHttpRequest failed due to initEasyHandle failure !");
    return false;
  }

  std::string nowTime = currentTime();
  printf("************ sending [%d] http_request for trans_id: %s @ %s start ************\n", 
      ++m_sentCount, httpReqInfo->m_requestId.c_str(),nowTime.c_str());
  /*fprintf(stderr, "xGateHttpHandler sending http_request: (%s)\n", httpReqInfo->m_data.c_str());*/
  //step2: add each handle in multi request queue
  mcode = curl_multi_add_handle(m_multiHandle, httpReqInfo->m_easyHandle);

  return true;
}

bool xGateHttpHandler::initEasyHandle(HttpRequestInfo *httpReqInfo)
{
  CURL *easyHandle = curl_easy_init();
  if(easyHandle == NULL) {
    XGLOG_ERROR( "initEasyHandle failed since curl handle is not created !");
    return false;
  }
  if(httpReqInfo->m_url.empty()) {
    XGLOG_ERROR( "initEasyHandle failed since url is empty !");
    return false;
  }
  if(httpReqInfo->m_data.empty()) {
    XGLOG_ERROR( "initEasyHandle failed since data is empty !");
    return false;
  }
  curl_easy_setopt(easyHandle, CURLOPT_URL, httpReqInfo->m_url.c_str());
  //curl_easy_setopt(easyHandle, CURLOPT_VERBOSE, 1L);
  /* set the error buffer as empty before performing a request */
  curl_easy_setopt(easyHandle, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(easyHandle, CURLOPT_TCP_NODELAY, 0);
  curl_easy_setopt(easyHandle, CURLOPT_TIMEOUT, 5L);
  curl_easy_setopt(easyHandle, CURLOPT_CONNECTTIMEOUT, 2L);
  curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, write_cb);
  curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, httpReqInfo);
  struct curl_slist *header = NULL;
  header = curl_slist_append(header, "Content-Type: application/json");
  curl_easy_setopt(easyHandle, CURLOPT_HTTPHEADER, header);
  curl_easy_setopt(easyHandle, CURLOPT_POSTFIELDS, httpReqInfo->m_data.c_str());
  httpReqInfo->m_err[0] = 0;
  curl_easy_setopt(easyHandle, CURLOPT_ERRORBUFFER, httpReqInfo->m_err);
  curl_easy_setopt(easyHandle, CURLOPT_PRIVATE, httpReqInfo);

  httpReqInfo->m_easyHandle = easyHandle;
  //CLog::Detail(THISMODULE, "initialized easy handle successfully !");
  return true; 
}

size_t xGateHttpHandler::write_cb(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t written = size * nmemb;
  HttpRequestInfo *httpReqInfo = (HttpRequestInfo *)data;
  httpReqInfo->m_output = (const char *)ptr;
#ifdef XGATE_HAS_BUSINESS_SUPPORT
  //post the result to BusinessProcessor thread
  /*if(httpReqInfo->m_requestorType == EN_REQUESTOR_TYPE_BUSINESS_PROCESSOR) {
    }*/
#endif
  /*printf("********* write_cb got http response for trans_id: %s ***********\n", httpReqInfo->m_requestId.c_str());
    fprintf(stderr, "got http response (%s)\n", httpReqInfo->m_output.c_str());
    printf("********* write_cb got http response - end ***********\n\n");*/
  //CLog::Detail(THISMODULE,"got http response (%s)", httpReqInfo->m_output.c_str());
  return written;
}

HttpRequestInfo * xGateHttpHandler::getHttpResponse()
{
  int msgs_left;
  int pending = 0;
  int complete = 0;
  int still_running = 1;
  HttpRequestInfo *httpReqInfo = NULL;

  while(still_running) {
    int numfds;
    curl_multi_wait(m_multiHandle, NULL, 0, 1000, &numfds);
    curl_multi_perform(m_multiHandle, &still_running);

    CURLMsg *m = NULL;
    while((m = curl_multi_info_read(m_multiHandle, &msgs_left))) {
      if(m->msg == CURLMSG_DONE) {
        CURL *handle = m->easy_handle;
        char *url;
        curl_easy_getinfo(handle, CURLINFO_PRIVATE, &httpReqInfo);
        curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &url);
        if(m->data.result == CURLE_OK) {
          long res_status;
          curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &res_status);
          if(res_status == 200) {
            /*fprintf(stderr, "**** recevied http [%d] response from %s ****\n", 
              res_status, httpReqInfo->m_url.c_str());*/
          } else {
            fprintf(stderr, "recevied http [%d] response with http error (%s)\n", res_status, httpReqInfo->m_err);
            fprintf(stderr, "got curl error: (%d - %s) from %s\n",
                m->data.result, curl_easy_strerror(m->data.result), url);
          }
        } else {
          //fprintf(stderr, "not able to connect %s. got curl error: %s\n", url, curl_easy_strerror(m->data.result));
        }
        curl_multi_remove_handle(m_multiHandle, handle);
        if(httpReqInfo) {
          //post to DbmProcessor thread
          std::string nowTime = currentTime();
          //fprintf(stderr, "**** recevied [%d] http response for trans_id: %s @ %s ****\n", ++m_recvCount, httpReqInfo->m_requestId.c_str(),nowTime);
          printf("************ recevied [%d] http response for trans_id: %s @ %s ****\n",++m_recvCount, httpReqInfo->m_requestId.c_str(),nowTime.c_str()); 
          if(httpReqInfo->m_requestorType == EN_REQUESTOR_TYPE_DBM_DISPATCHER) {
          post(httpReqInfo);
          }
          delete httpReqInfo;
          httpReqInfo = NULL;
        }
      }
    }
  }
  return httpReqInfo;
}
/* get CURLMcode string detail */
string xGateHttpHandler::getMcodeStr(CURLMcode code)
{
  string err("");
  switch(code) {
    case CURLM_OK:
      {
        err = "CURLM_OK";
        break;
      }
    case CURLM_CALL_MULTI_PERFORM:
      {
        err = "CURLM_CALL_MULTI_PERFORM";
        break;
      }
    case CURLM_BAD_HANDLE:
      {
        err = "CURLM_BAD_HANDLE";
        break;
      }
    case CURLM_BAD_EASY_HANDLE:
      {
        err = "CURLM_BAD_EASY_HANDLE";
        break;
      }
    case CURLM_OUT_OF_MEMORY:
      {
        err = "CURLM_OUT_OF_MEMORY";
        break;
      }
    case CURLM_INTERNAL_ERROR:
      {
        err = "CURLM_INTERNAL_ERROR";
        break;
      }
    case CURLM_UNKNOWN_OPTION:
      {
        err = "CURLM_UNKNOWN_OPTION";
        break;
      }
    case CURLM_LAST:
      {
        err = "CURLM_LAST";
        break;
      }
    case CURLM_BAD_SOCKET:
      {
        err = "CURLM_BAD_SOCKET";
        break;
      }
    default:
      {
        err = "CURLM_unknown";
        break;
      }
  }
  return err;
}

bool xGateHttpHandler::post(HttpRequestInfo *httpReqInfo)
{
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = xGateUtil::getDbmProcessor();
  if(!taskPtr) {
    XGLOG_ERROR( "post failed. DbmProccessor taksptr is NULL !");
    return false;
  }

  if(httpReqInfo->m_output.empty()) {
    XGLOG_ERROR( "post failed. http response is empty !");
    return false;
  }

  int len = httpReqInfo->m_output.length();
  char* data = new char[len+1];
  memset(data, 0, (len+1));
  if(!httpReqInfo->m_output.empty()) {
    memcpy (data, httpReqInfo->m_output.c_str(), len);
  }
  data[len] = '\0';

  xGateDbmMsg *pDbmMsg = new xGateDbmMsg();
  pDbmMsg->setMsgType(EN_XGATE_MSG_FROM_DBM);
  pDbmMsg->set_data(data);
  pDbmMsg->set_data_len(len);

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateDbmMsg*>(pDbmMsg);
  ACE_Time_Value tval ((time(NULL)));
  tval += ACE_Time_Value(0,1);

  if(taskPtr->putq(pAmb, &tval)) {
    //CLog::Detail(THISMODULE, "successfully posted message to DbmProcessor");
  } else {
    XGLOG_ERROR( "failed to post message to DbmProcessor !");
    return false;
  }
  return true;
}

std::string xGateHttpHandler::currentTime()
{
  timeval curTime;
  gettimeofday(&curTime, NULL);
  int milli = curTime.tv_usec / 1000;

  time_t rawtime;
  struct tm * timeinfo;
  char buffer [80];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

  char currentTime[84] = "";
  sprintf(currentTime, "%s:%d", buffer, milli);
  return currentTime;
}
/* xGateHttpHandler class - End
*/
