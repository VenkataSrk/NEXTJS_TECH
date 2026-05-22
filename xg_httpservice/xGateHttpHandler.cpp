//local includes
#include "xglog.h"
#include "xGateHttpHandler.h"
#include "xGateHttpService.h"
#include "xGateHttpServiceMsg.h"
#define MAX_HTTP_CON 200

int xGateHttpHandler::m_sentCount=0;
int xGateHttpHandler::m_recvCount=0;

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
        XGLOG_ERROR("HttpHandler http-handler initialization failed !");
        delete m_instance;
        m_instance = NULL;
      } else {
        XGLOG_INFO("HttpHandler http-handler initialized successfully !");
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
      XGLOG_INFO("HttpHandler http-handler de-initialized successfully !");
    }
  }
}

bool xGateHttpHandler::init()
{
  XGLOG_INFO("HttpHandler http-handler init() initialized successfully!");
  m_multiHandle = curl_multi_init();
  if(!m_multiHandle) {
    XGLOG_ERROR("HttpHandler curl_multi_init failed !");
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
      string mcodeStr = getMcodeStr(mcode);
      XGLOG_ERROR("HttpHandler curl_multi_cleanup failed. got %s error !", mcodeStr.c_str());
    }
    m_multiHandle = NULL; 
  }
  return true;
}

bool xGateHttpHandler::doHttpRequest(HttpRequestInfo &httpReqInfo)
{
	CURLMcode mcode;
	CURLcode res;

	//step1: create new pointer obejct of HttpRequestInfo, 
	//that will be passed to curl callback function
	HttpRequestInfo *pHttpReqInfo = new HttpRequestInfo();
	if(!pHttpReqInfo){
	  XGLOG_ERROR("xGateHttpHandler::doHttpRequest failed to create HttpRequestInfo!");
	  return false;
	}
	pHttpReqInfo->m_url = httpReqInfo.m_url; 
	pHttpReqInfo->m_requestId = httpReqInfo.m_requestId;
	pHttpReqInfo->m_contextId = httpReqInfo.m_contextId;
	pHttpReqInfo->m_data = httpReqInfo.m_data;
	pHttpReqInfo->m_contentType = httpReqInfo.m_contentType;
	pHttpReqInfo->m_httpReqMethod = httpReqInfo.m_httpReqMethod;
	pHttpReqInfo->m_multipartFileInfo = httpReqInfo.m_multipartFileInfo;
	//pHttpReqInfo->m_requestorModule = httpReqInfo.m_requestorModule;
	pHttpReqInfo->m_requestorModule = httpReqInfo.m_requestorModule;
	pHttpReqInfo->m_httpRespStatus = httpReqInfo.m_httpRespStatus;
	pHttpReqInfo->m_statusCode = httpReqInfo.m_statusCode;

	//step1: create and init new curl easy handle for this http request
	if(!initEasyHandle(pHttpReqInfo)) {
		XGLOG_ERROR("HttpHandler doHttpRequest failed due to initEasyHandle failure !");
		return false;
	}

  std::string nowTime = currentTime();
#if 0
  printf("************ sending [%d] http_request for trans_id: %s @ %s start ************\n", 
      ++m_sentCount, pHttpReqInfo->m_requestId.c_str(),nowTime.c_str());
  fprintf(stderr, "xGateHttpHandler sending http_request: (%s)\n", pHttpReqInfo->m_data.c_str());
#else
  XGLOG_DEBUG("sending http_request to url: %s for trans_id: %s @ %s", \
      pHttpReqInfo->m_url.c_str(), pHttpReqInfo->m_requestId.c_str(), nowTime.c_str());
#endif

  //step2: add each handle in multi request queue
  mcode = curl_multi_add_handle(m_multiHandle, pHttpReqInfo->m_easyHandle);
  getHttpResponse(); 
  return true;
}

bool xGateHttpHandler::initEasyHandle(HttpRequestInfo *httpReqInfo)
{
  CURL *easyHandle = curl_easy_init();
  if(easyHandle == NULL) {
    XGLOG_ERROR("HttpHandler initEasyHandle failed since curl handle is not created !");
    return false;
  }
  if(httpReqInfo->m_url.empty()) {
    XGLOG_ERROR("HttpHandler initEasyHandle failed since url is empty !");
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
  if(!setHttpContentType(easyHandle,httpReqInfo)) {
    XGLOG_ERROR("HttpHandler initEasyHandle failed due to unsupported content type !");
    return false;
  }

		switch(httpReqInfo->m_httpReqMethod) {
			case EN_HTTP_METHOD_POST:
			{
				if(httpReqInfo->m_data.empty()) {
					XGLOG_ERROR("HttpHandler initEasyHandle failed since data is empty !");
					return false;
				}
				curl_easy_setopt(easyHandle, CURLOPT_POST, 1L);
				curl_easy_setopt(easyHandle, CURLOPT_POSTFIELDS, httpReqInfo->m_data.c_str());
				break;
			}
			case EN_HTTP_METHOD_GET:
			{
				curl_easy_setopt(easyHandle, CURLOPT_HTTPGET, 1L);
				break;
			}
                        case EN_HTTP_METHOD_PUT:
                        {
                                curl_easy_setopt(easyHandle, CURLOPT_PUT, 1L);
                                break;
                        }
			case EN_HTTP_METHOD_MULTIPART:
			{
				struct curl_httppost *formpost = NULL;
				struct curl_httppost *lastptr = NULL;
				struct curl_slist *headerlist = NULL;
				static const char buf[] = "Expect:";

				#if 1
				vector < pair<string,string> >::const_iterator itrFile = httpReqInfo->m_multipartFileInfo.begin();
				for(itrFile; itrFile != httpReqInfo->m_multipartFileInfo.end(); itrFile++) {
					curl_formadd(&formpost,&lastptr,
					CURLFORM_COPYNAME, itrFile->first.c_str(),
					CURLFORM_FILE, itrFile->second.c_str(),
					CURLFORM_END);
				}  

				vector < pair<string,string> >::const_iterator itrData = httpReqInfo->m_multipartDataInfo.begin();
				for(itrData; itrData != httpReqInfo->m_multipartDataInfo.end(); itrData++) {
					curl_formadd(&formpost,&lastptr,
					CURLFORM_COPYNAME, itrData->first.c_str(),
					CURLFORM_COPYCONTENTS, itrData->second.c_str(),
					CURLFORM_END);
				}

				#endif      
				headerlist = curl_slist_append(headerlist, buf);    
				curl_easy_setopt(easyHandle, CURLOPT_HTTPHEADER, headerlist);
				curl_easy_setopt(easyHandle, CURLOPT_HTTPPOST, formpost);

				break;
			}
			default:
			{
				XGLOG_ERROR("HttpHandler initEasyHandle failed. Unsupported/UNKNOWN Http request method !");
				return false;
			}
		}
		httpReqInfo->m_err[0] = 0;
		curl_easy_setopt(easyHandle, CURLOPT_ERRORBUFFER, httpReqInfo->m_err);
		curl_easy_setopt(easyHandle, CURLOPT_PRIVATE, httpReqInfo);
		
		//SMT
			curl_easy_setopt(easyHandle, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(easyHandle, CURLOPT_SSL_VERIFYHOST, 0);

  httpReqInfo->m_easyHandle = easyHandle;
  //XGLOG_INFO("HttpHandler initialized easy handle successfully !");
  return true; 
}

bool xGateHttpHandler::setHttpContentType(CURL *easyHandle,HttpRequestInfo *httpReqInfo)
{
  struct curl_slist *header = NULL;
  switch(httpReqInfo->m_contentType) {
    case EN_CONTENT_TYPE_JSON:
      {
        header = curl_slist_append(header, "Content-Type: application/json");
        curl_easy_setopt(easyHandle, CURLOPT_HTTPHEADER, header);
        break;
      }
    case EN_CONTENT_TYPE_XML:
      {
        header = curl_slist_append(header, "Content-Type: application/xml");
        curl_easy_setopt(easyHandle, CURLOPT_HTTPHEADER, header);
        break;
      }
    case EN_CONTENT_TYPE_TEXT:
      {
        header = curl_slist_append(header, "Content-Type: application/text");
        curl_easy_setopt(easyHandle, CURLOPT_HTTPHEADER, header);
        break;
      }
    default:
      {
        XGLOG_ERROR("HttpHandler setHttpContentType failed. Unsupported/UNKNOWN Http content type !");
        return false;
      }
  } 
  return true;
}

size_t xGateHttpHandler::write_cb(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t written = size * nmemb;
  HttpRequestInfo *httpReqInfo = (HttpRequestInfo *)data;
  httpReqInfo->m_output += (const char *)ptr;
  //post the result to BusinessProcessor thread
#if 0
  printf("********* write_cb got http response for trans_id: %s ***********\n", httpReqInfo->m_requestId.c_str());
  fprintf(stderr, "got http response (%s)\n", httpReqInfo->m_output.c_str());
  printf("********* write_cb got http response - end ***********\n\n");
#else
  //XGLOG_DEBUG("got http response (%s)", httpReqInfo->m_output.c_str());
#endif
  return written;
}

bool xGateHttpHandler::getHttpResponse()
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
			  curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &httpReqInfo->m_statusCode);
			  fillHttpStatusInfo(httpReqInfo);
			} else {
			  //fprintf(stderr, "not able to connect %s. got curl error: %s\n", url, curl_easy_strerror(m->data.result));
			  httpReqInfo->m_output = curl_easy_strerror(m->data.result);
			  httpReqInfo->m_statusCode = 408;//TODO: Since most of the times we get Request Timeout Error
			  fillHttpStatusInfo(httpReqInfo);
			}
			curl_multi_remove_handle(m_multiHandle, handle);
			if(httpReqInfo) {
	#if 0
			  std::string nowTime = currentTime();
			  //fprintf(stderr, "**** recevied [%d] http response for trans_id: %s @ %s ****\n", ++m_recvCount, httpReqInfo->m_requestId.c_str(),nowTime);
			  printf("************ received [%d] http response for trans_id: %s @ %s ****\n",++m_recvCount, httpReqInfo->m_requestId.c_str(),nowTime.c_str());
			  printf("************ HttpHandler got http response ==>(%s)", httpReqInfo->m_output.c_str());
	#else
			  //XGLOG_DEBUG("Received http response (%d %s) for trans_id: %s", \
				  httpReqInfo->m_statusCode, httpReqInfo->m_output.c_str(), httpReqInfo->m_requestId.c_str());
	#endif
			  xGateHttpServiceMsg *pMsg = new xGateHttpServiceMsg();
			  if(!pMsg){
			     XGLOG_ERROR("xGateHttpHandler::getHttpResponse() failed to create xGateHttpServiceMsg!");
			     return false;
			   }
			  pMsg->setDstModuleId(httpReqInfo->m_requestorModule);
			  pMsg->set_http_request_info(*httpReqInfo);
			  pMsg->setUid(httpReqInfo->m_contextId);
			  pMsg->setOpCode(httpReqInfo->m_httpReqMethod);
			  //XGUTIL->post(pMsg)
			  HTTPSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(pMsg);	
        }
      }
    }
  }
  return true;
}
 
bool xGateHttpHandler::fillHttpStatusInfo(HttpRequestInfo *httpReqInfo) 
{
  std::string nowTime = currentTime();
  switch(httpReqInfo->m_statusCode) {
    case 200:
      {
        httpReqInfo->m_httpRespStatus = EN_HTTP_STATUS_SUCCESS;
        XGLOG_INFO("Recevied http success response [%d] for request_id:%s @%s",
              httpReqInfo->m_statusCode, httpReqInfo->m_requestId.c_str(), nowTime.c_str());
        return true;
      }


 	case 201:
      	{
        	httpReqInfo->m_httpRespStatus = EN_HTTP_STATUS_SUCCESS;
        	XGLOG_INFO("Recevied http success response [%d] for request_id:%s @%s",
              	httpReqInfo->m_statusCode, httpReqInfo->m_requestId.c_str(), nowTime.c_str());
        	return true;

	}	

    case 400:
      {
        httpReqInfo->m_httpRespStatus = EN_HTTP_STATUS_FAILED;
        httpReqInfo->m_output = "Bad Request";
        break;
      }
    case 401:
      {
        httpReqInfo->m_httpRespStatus = EN_HTTP_STATUS_FAILED;
        httpReqInfo->m_output = "Unauthorised";
        break;
      }
    case 403:
      {
        httpReqInfo->m_httpRespStatus = EN_HTTP_STATUS_FAILED;
        httpReqInfo->m_output = "Forbidden";
        break;
      }
    case 404:
      {
        httpReqInfo->m_httpRespStatus = EN_HTTP_STATUS_FAILED;
        httpReqInfo->m_output = "Page Not Found";
        break;
      }
    case 405:
      {
        httpReqInfo->m_httpRespStatus = EN_HTTP_STATUS_FAILED;
        httpReqInfo->m_output = "Request Method Not Allowed";
        break;
      }
    case 408:
      {
        httpReqInfo->m_httpRespStatus = EN_HTTP_STATUS_FAILED;
        break;
      }
    case 500:
      {
        httpReqInfo->m_httpRespStatus = EN_HTTP_STATUS_FAILED;
        httpReqInfo->m_output = "Internal Server Error";
        break;
      }
    case 503:
      {
        httpReqInfo->m_httpRespStatus = EN_HTTP_STATUS_FAILED;
        httpReqInfo->m_output = "Service Unavailable Server Error";
        break;
      }
    default:
      {
        httpReqInfo->m_httpRespStatus = EN_HTTP_STATUS_UNKNOWN;
        XGLOG_ERROR("Received unknown HTTP response code:%d for request_id:%s @%s",
              httpReqInfo->m_statusCode, httpReqInfo->m_requestId.c_str(), nowTime.c_str());
        return true;
      }
   }
  XGLOG_ERROR("Recevied http response (%d %s) for trans_id: %s at %s", \
      httpReqInfo->m_statusCode, httpReqInfo->m_output.c_str(), httpReqInfo->m_requestId.c_str(), nowTime.c_str());
  return true;
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
