//local includes
#include "log.h"
//#include "klog.h"
#include "xGateDbmMsg.h"
#include "xGateDbmDispatcher.h"
#include "xGateDbmReceiver.h"
#define THISMODULE "DbmDisp"

xGateDbmDispatcher::xGateDbmDispatcher(const char* serverType) : xGateDispatcher(serverType)
{
  memset(m_serverType,0,SERVER_TYPE_SIZE);
  strcpy(m_serverType,serverType);
  m_doc = new Document();
  m_strBuf = new StringBuffer();
  readTemplate();
 XGLOG_INFO( "xGateDbmDispatcher::xGateDbmDispatcher(%s)", m_serverType);
}

xGateDbmDispatcher::~xGateDbmDispatcher(void)
{
  if(m_doc) delete m_doc;
  if(m_strBuf) delete m_strBuf;
 XGLOG_INFO( "xGateDbmDispatcher::~xGateDbmDispatcher(%s)", m_serverType);
}

bool xGateDbmDispatcher::readTemplate()
{
  //read the template and make it as json document
  ParseResult ok = m_doc->Parse(json_dbm_client_tmpl.c_str());
  if(!ok) {
   XGLOG_ERROR("xGateDbmDispatcher::read template failed due to json parse error: %s !", \
        GetParseError_En(ok.Code()));
    return false;
  } else if(!m_doc->IsObject()) {
   XGLOG_ERROR("xGateDbmDispatcher::read template failed. not able to create document object from dbm_client template !");
    return false;
  }
 XGLOG_INFO( "parsed dbm_client_json template successfully");
  return true;
}

int xGateDbmDispatcher::svc(void)
{
 XGLOG_INFO( "xGateDbmDispatcher::svc(%s)", m_serverType);

  ACE_Message_Block *pAmb = 0;

  while(m_run) {
    ACE_Time_Value tv ((time(NULL) + 20));

    // Get message from the queue
    int result = getq(pAmb, &tv);
    if((result == -1) && (errno == EWOULDBLOCK)) {
      continue;
    }

    if ((result != -1)) {
      process_msg(pAmb);
    } else {
      // This is the case where the message queue is de-activated.
      // Need to stop the thread
     XGLOG_ERROR("xGateDbmDispatcher::svc(%s) Error in Message Queue", m_serverType);
      m_run = false;
    }

    // delete the message received
    if(pAmb) {
      delete pAmb;
      pAmb = NULL;
    }
  }
 XGLOG_INFO( "xGateDbmDispatcher::svc(%s) Stopped Dispatch Task Thread", m_serverType);
  return 0;
}

//This function handle msg request from sofia.cpp(main thread) and post to MGDispatcher thread
xGateRetVal xGateDbmDispatcher::process_msg(ACE_Message_Block *pAmb)
{
 XGLOG_INFO( "xGateDbmDispatcher::process_msg(%s)", m_serverType);
  xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;

  if(pAmb != NULL) {
    xGateDbmMsg* pDbmMsg = NULL;
    pDbmMsg = dynamic_cast<xGateDbmMsg*>(pAmb);
    if(pDbmMsg  == NULL) {
     XGLOG_ERROR("xGateDbmDispatcher::process_msg recvd invalid xGateDbmMsg Message");
      return EN_XGATE_STATUS_ERROR;
    }

    if(!encode_msg(pDbmMsg)) {
     XGLOG_ERROR("xGateDbmDispatcher::process_msg not able to encode DBM message");
      return EN_XGATE_STATUS_ERROR;
    }

    if(!dispatch_msg(pDbmMsg)) {
     XGLOG_ERROR("xGateDbmDispatcher::process_msg not able to encode DBM message");
      return EN_XGATE_STATUS_ERROR;
    }
  }
  return retval;
}

Value setJsonParam(string strValue,Document *doc ){
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), doc->GetAllocator());
  return retVal;
}

Value setJsonParam(unsigned int iValue, Document *doc){
  Value retVal;
  retVal = iValue;
  return retVal;
}

bool xGateDbmDispatcher::encode_msg(xGateDbmMsg *pMsg)
{
  DbmReqDetail reqDetail = pMsg->get_dbm_request_detail();
  m_strBuf->Clear(); 
  Writer<StringBuffer> writer(*m_strBuf);

  (*m_doc)["trans_id"] = setJsonParam(reqDetail.m_transId, m_doc);
  (*m_doc)["db_type"] = setJsonParam(reqDetail.m_dbType, m_doc);
  //(*m_doc)["db_operation"] = setJsonParam(reqDetail.m_opCode, m_doc);
  (*m_doc)["db_operation"] = setJsonParam("EXECUTE", m_doc);
  (*m_doc)["db_name"] = setJsonParam(reqDetail.m_dbName, m_doc);
  (*m_doc)["table_name"] = setJsonParam(reqDetail.m_tableName, m_doc);
  (*m_doc)["sp_name"] = setJsonParam(reqDetail.m_spName, m_doc);

  Document inputDoc(&m_doc->GetAllocator());
  if(getInputDoc(reqDetail.m_input, inputDoc)) {
    m_doc->RemoveMember("input"); //remove existing input object if any
    m_doc->AddMember("input", inputDoc, m_doc->GetAllocator());
  }

  m_doc->Accept(writer);
  return true; 
}

bool xGateDbmDispatcher::getInputDoc(string &input, Document &doc)
{
  if(input.empty()) {
   XGLOG_ERROR("getInputDoc failed. given input vaule is empty!");
    return false;
  }

  ParseResult ok = doc.Parse(input.c_str());
  if(!ok) {
   XGLOG_ERROR("getInputDoc failed due to json parse error: %s !", \
        GetParseError_En(ok.Code()));
    return false;
  } else if(!doc.IsObject()) {
   XGLOG_ERROR("getInputDoc failed to create json object from given input: %s!", \
        input.c_str());
    return false;
  }
  return true;
}

bool xGateDbmDispatcher::dispatch_msg(xGateDbmMsg *pMsg)
{
 XGLOG_INFO( "Enter xGateDbmDispatcher::dispatch_msg(%s)",m_serverType);
  xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;

  xGateHttpHandler *httpHandler = xGateHttpHandler::getInstance();
  if(!httpHandler) {
   XGLOG_ERROR("xGateDbmDispatcher::dispatch_msg failed. invalid httphandler object !");
    return false;  
  }

  DbmReqDetail reqDetail = pMsg->get_dbm_request_detail();
  HttpRequestInfo *httpReqInfo = new HttpRequestInfo(); 

#if 0 //TODO: hardcoded for testing purpose
  httpReqInfo->m_url = NODEJS_URL; 
  string data =  "{\"Call_id\":\"121d5332-408f-1238-f1b4-83897910f890\",\"domain_name\":\"vectoneapp.webrtc.mundio.com\",\"Contact_address\":\"79.11.57.202\",\"Ipaddress\":\"79.11.57.204\",\"IpAddress_type\":\"0\",\"Username\":\"sampleuser\",\"Password\":\"243536546\",\"AAA\":\"1\",\"expires\":\"3600\",\"Request_cseq\":\"8743909\",\"status\":\"1\",\"proxy_username\":\"\",\"device_type\":\"\",\"mac_address\":\"\"}";
  httpReqInfo->m_data = data;
  httpReqInfo->m_requestId = reqDetail.m_transId;
#else
if(reqDetail.m_url.empty())
  httpReqInfo->m_url = NODEJS_URL; 
else
httpReqInfo->m_url = reqDetail.m_url;
  httpReqInfo->m_requestId = reqDetail.m_transId;
  string data(m_strBuf->GetString());
CLog::Info(THISMODULE, "xGateDbmDispatcher::dispatch_msg Post Data %s", data.c_str());
  httpReqInfo->m_data = data;
  //fprintf(stderr, " xGateDbmDispatcher :: posting data (%s)\n", httpReqInfo->m_data.c_str());
#endif

  //CLog::Detail(THISMODULE, "xGateDbmDispatcher::dispatch_msg sending http request: (%s)", httpReqInfo->m_data.c_str());
  if(!httpHandler->doHttpRequest(httpReqInfo)) {
   XGLOG_ERROR("xGateDbmDispatcher::dispatch_msg failed. send http request failed !");
    delete httpReqInfo;
    return false;  
  }

  //now send the message to xGateHttpReader thread
  //so it will be looking for the response for this request
  xGateDbmMsg *pDbmMsg = new xGateDbmMsg();
  pDbmMsg->set_http_request_info(httpReqInfo);
  return post(pDbmMsg);
}

bool xGateDbmDispatcher::post(xGateDbmMsg *pMsg)
{
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  pMsg->setMsgType(EN_XGATE_MSG_FROM_DBM);

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateDbmMsg*>(pMsg);
  ACE_Time_Value tval ((time(NULL)));
  tval += ACE_Time_Value(0,1);

  taskPtr = xGateUtil::getDbmReceiver(); 
  if(taskPtr->putq(pAmb, &tval)) {
    //CLog::Detail(THISMODULE, "xGateDbmDispatcher posted message to DbmReceiver thread");
  } else {
   XGLOG_ERROR("xGateDbmDispatcher failed to post message to DbmReceiver thread !");
    return false;
  }
  return true;
}

