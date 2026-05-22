#include "BSHttpService.h"
#include "BusinessServer.h"
#include "LogHandler.h"

bool BSHttpService::init()
{
  
  m_multiHandle = curl_multi_init();
  if (!m_multiHandle)
  {
    return false;
  }
  curl_multi_setopt(m_multiHandle, CURLMOPT_MAX_TOTAL_CONNECTIONS, MAX_HTTP_CON);
  curl_multi_setopt(m_multiHandle, CURLMOPT_MAX_HOST_CONNECTIONS, 6L);
  return true;
}

bool BSHttpService::deInit()
{
  
  if (m_multiHandle)
  {
    CURLMcode mcode = curl_multi_cleanup(m_multiHandle);
    if (mcode != CURLM_OK)
    {
      std::string mcodeStr = getMcodeStr(mcode);
      std::cout << "BSHttpService curl_multi_cleanup failed. got error - " << mcodeStr.c_str() << std::endl;
    }
    m_multiHandle = NULL;
  }
  return true;
}

bool BSHttpService::run()
{
  const std::string thread_name = "HttpService";
  int rc = pthread_setname_np(pthread_self(),thread_name.c_str());
  if(rc != 0)
  {
	  std::cerr << "Failed to set name for thread" << strerror(rc) << std::endl;
  }else{
	  std::cout << "Thread name set successfully to :" << thread_name << std::endl;
  }  
  while (getThreadStatus())
  {
    CA::CAMessageBlock *pMsg = (CA::CAMessageBlock *)RecvMessage();
    if (pMsg)
    {
      handleMsg(pMsg);
      if(pMsg){delete pMsg; pMsg = nullptr;}
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  return true;
}

bool BSHttpService::handleMsg(CA::CAMessageBlock *pMsg)
{
  
  if (!pMsg)
  {
    return false;
  }
  unsigned long msgType = pMsg->getMsgType();
  switch (msgType)
  {
  case MESSAGEEVENT::MSG_INIT:
  {
    init();
  }
  break;
  case MESSAGEEVENT::MSG_SHUTDOWN:
  {
    deInit();
  }
  break;
  case MESSAGEEVENT::MSG_GMSC2HTTP:
  case MESSAGEEVENT::MSG_ROUT2HTTP:
  {
    handle_hlr_request(pMsg);
  }
  break;
  default:
  {
  }
  break;
  }
  return true;
}

bool BSHttpService::handle_hlr_request(CA::CAMessageBlock *pMsg)
{
  
  BSOperationMessage *bsOprMsg =  static_cast<BSOperationMessage*>(pMsg);
  if(!bsOprMsg)
    return false;
    BSOperation* bsp = bsOprMsg->m_bsOperation.release();
    if(!bsp)
      return false;
  
  std::unique_ptr<BSHttpServiceMsg> pHttpMsg =  bsp->relHttpRequestMsg();
  if(!pHttpMsg)
    return false;
  sendHttpRequest(bsp,pHttpMsg.release());
  return true;
}

bool BSHttpService::sendHttpRequest(BSOperation *bsp,BSHttpServiceMsg *pHttpMsg)
{
  
  CURLMcode mcode;
  CURLcode res;
  
  pHttpMsg->setRequestId(generateRequestId());
  if (!initEasyHandle(pHttpMsg))
  {
    if(bsp){delete bsp ; bsp = nullptr;}
    return false;
  }
  mcode = curl_multi_add_handle(m_multiHandle, pHttpMsg->getHandle());
  if(getHttpResponse(bsp) == false)
  {
    if(bsp){delete bsp ; bsp = nullptr;}
	  logerror("", "BSHttpService::sendHttpRequest getHttpResponse failed");
  }	   
  return true;
}

bool BSHttpService::initEasyHandle(BSHttpServiceMsg *pHttpMsg)
{
  
  CURL *easyHandle = curl_easy_init();
  if (easyHandle == NULL)
  {
    return false;
  }
  if (pHttpMsg->getUrl().empty())
  {
    return false;
  }
  curl_easy_setopt(easyHandle, CURLOPT_URL, pHttpMsg->getUrl().c_str());
  // curl_easy_setopt(easyHandle, CURLOPT_VERBOSE, 1L);
  /* set the error buffer as empty before performing a request */
  curl_easy_setopt(easyHandle, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(easyHandle, CURLOPT_TCP_NODELAY, 0);
  curl_easy_setopt(easyHandle, CURLOPT_TIMEOUT, 5L);
  curl_easy_setopt(easyHandle, CURLOPT_CONNECTTIMEOUT, 2L);
  curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, write_cb);
  curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, pHttpMsg);
  struct curl_slist *header = NULL;
  //std::string httpHeader("");
  //httpHeader.assign("Content-Type: ");
  //httpHeader.append(pHttpMsg->getHeader());
  header = curl_slist_append(header, pHttpMsg->getHeader().c_str());
  curl_easy_setopt(easyHandle, CURLOPT_HTTPHEADER, header);
  curl_easy_setopt(easyHandle, CURLOPT_POST, 1);
  curl_easy_setopt(easyHandle, CURLOPT_POSTFIELDS, pHttpMsg->getBody());
 /* 
  switch (pHttpMsg->getReqMethod())
  {
  case HttpReqMethod::EN_HTTP_METHOD_POST:
  {
    if (pHttpMsg->getBody().empty())
    {
      return false;
    }
    curl_easy_setopt(easyHandle, CURLOPT_POST, 1L);
    curl_easy_setopt(easyHandle, CURLOPT_POSTFIELDS, pHttpMsg->getBody().c_str());
    break;
  }
  default:
  {
    return false;
  }
  }
  */
  pHttpMsg->m_err[0] = 0;
  curl_easy_setopt(easyHandle, CURLOPT_ERRORBUFFER, pHttpMsg->m_err);
  curl_easy_setopt(easyHandle, CURLOPT_PRIVATE, pHttpMsg);
  pHttpMsg->setHandle(easyHandle);
  return true;
}

size_t BSHttpService::write_cb(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t written = size * nmemb;
  BSHttpServiceMsg *httpMsg = (BSHttpServiceMsg *)data;
 // httpMsg->setOutput(httpMsg->getOutput() + (const char *)ptr);
 httpMsg->setOutput((const char*)ptr);
  return written;
}

bool BSHttpService::getHttpResponse(BSOperation *bsp)
{
  int msgs_left;
  int pending = 0;
  int complete = 0;
  int still_running = 1;
  BSHttpServiceMsg *httpMsg = NULL;
  if(!bsp)
    return false;
  while (still_running)
  {
    int numfds;
    curl_multi_wait(m_multiHandle, NULL, 0, 1000, &numfds);
    curl_multi_perform(m_multiHandle, &still_running);
    CURLMsg *m = NULL;
    while ((m = curl_multi_info_read(m_multiHandle, &msgs_left)))
    {
      if (m->msg == CURLMSG_DONE)
      {
        CURL *handle = m->easy_handle;
        char *url;
	      int res_status{-1};
        curl_easy_getinfo(handle, CURLINFO_PRIVATE, &httpMsg);
        curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &url);
        if (m->data.result == CURLE_OK)
        {
          curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &res_status);
	        if(res_status == 200) {
            fprintf(stderr, "*** recevied http [%d] response from %s ***\n", res_status, httpMsg->getUrl().c_str());
          } else {
            fprintf(stderr, "*** got curl error [%d] - [%s] from %s\n", m->data.result, curl_easy_strerror(m->data.result), url);
          }
          httpMsg->setStatusCode(res_status);
          fillHttpStatusInfo(httpMsg);
        }
        else
        {
	        //fprintf(stderr, "*** recevied http [%d] response from %s ***\n", res_status, httpMsg->getUrl().c_str());
          httpMsg->setOutput(curl_easy_strerror(m->data.result));
          httpMsg->setStatusCode(408);
          fillHttpStatusInfo(httpMsg);
        }
        curl_multi_remove_handle(m_multiHandle, handle);
        if (httpMsg)
        {
          bsp->setHttpStatusCode(httpMsg->getStatusCode());
          bsp->setHttpOutput(httpMsg->getOutput());
          delete httpMsg;
          httpMsg = nullptr;
          std::unique_ptr<BSOperationMessage> bspMsg (new BSOperationMessage);
          std::unique_ptr<BSOperation> pbsp(bsp);
          bspMsg->m_bsOperation =  std::move(pbsp);
	        bspMsg->setMsgType(MESSAGEEVENT::MSG_HTTP2BSP);
          BusinessServer::post(BUSINESS_THREADS::MVNO_BSP,std::move(bspMsg));
        }else
          return false;
      }
    }
  }
  return true;
}

bool BSHttpService::fillHttpStatusInfo(BSHttpServiceMsg *httpMsg)
{
  switch (httpMsg->getStatusCode())
  {
  case 200:
  {
    httpMsg->setRespStatus(HttpResponseStatus::EN_HTTP_STATUS_SUCCESS);
    return true;
  }
  case 400:
  {
    httpMsg->setRespStatus(HttpResponseStatus::EN_HTTP_STATUS_FAILED);
    httpMsg->setOutput("Bad Request");
    break;
  }
  case 401:
  {
    httpMsg->setRespStatus(HttpResponseStatus::EN_HTTP_STATUS_FAILED);
    httpMsg->setOutput("Unauthorised");
    break;
  }
  case 403:
  {
    httpMsg->setRespStatus(HttpResponseStatus::EN_HTTP_STATUS_FAILED);
    httpMsg->setOutput("Forbidden");
    break;
  }
  case 404:
  {
    httpMsg->setRespStatus(HttpResponseStatus::EN_HTTP_STATUS_FAILED);
    httpMsg->setOutput("Page Not Found");
    break;
  }
  case 405:
  {
    httpMsg->setRespStatus(HttpResponseStatus::EN_HTTP_STATUS_FAILED);
    httpMsg->setOutput("Request Method Not Allowed");
    break;
  }
  case 408:
  {
    httpMsg->setRespStatus(HttpResponseStatus::EN_HTTP_STATUS_FAILED);
    break;
  }
  case 500:
  {
    httpMsg->setRespStatus(HttpResponseStatus::EN_HTTP_STATUS_FAILED);
    httpMsg->setOutput("Internal Server Error");
    break;
  }
  case 503:
  {
    httpMsg->setRespStatus(HttpResponseStatus::EN_HTTP_STATUS_FAILED);
    httpMsg->setOutput("Service Unavailable Server Error");
    break;
  }
  default:
  {
    httpMsg->setRespStatus(HttpResponseStatus::EN_HTTP_STATUS_UNKNOWN);
    return true;
  }
  }
  return true;
}

/* get CURLMcode string detail */
std::string BSHttpService::getMcodeStr(CURLMcode code)
{
  std::string err("");
  switch (code)
  {
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

std::string BSHttpService::generateRequestId()
{
  char charList[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$^&*-=_+/";
  unsigned int reqId_len = 16;
  std::string newstr("");
  newstr.reserve(17);
  int pos = 0;
  while (newstr.size() < reqId_len)
  {
    pos = ((rand() % (sizeof(charList) - 1)));
    newstr += charList[pos];
  }
  return newstr;
}
