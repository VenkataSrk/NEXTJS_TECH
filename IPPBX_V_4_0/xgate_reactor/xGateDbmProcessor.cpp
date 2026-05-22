//local includes
#include "log.h"
//#include "klog.h"
#include "xGateDbmProcessor.h"
#include "xGateSipUtil.h"
#include "xGateDBManager.h"

#define THISMODULE "DbmProc"

xGateDbmProcessor::xGateDbmProcessor(const char *serverType):xGateProcessor(serverType)
{
  XGLOG_WARN("xGateDbmProcessor::xGateDbmProcessor");
}

xGateDbmProcessor::~xGateDbmProcessor(void)
{
  XGLOG_WARN("xGateDbmProcessor::~xGateDbmProcessor");
}

xGateRetVal xGateDbmProcessor::process_msg(xGateBaseMsg* pMsg)
{
  XGLOG_INFO( "Enter xGateDbmProcessor::process_msg(%s)", m_serverType); 

  switch(pMsg->getMsgType()) {
    case EN_XGATE_MSG_FROM_DBM: //read dbm response
      {
        XGLOG_INFO( "Enter process_msg processing MSG_FROM_DBM"); 
        xGateDbmMsg *pDbmMsg = dynamic_cast<xGateDbmMsg*>(pMsg);
        //decode and validate dbm message
        if(decode_message(pDbmMsg) != EN_XGATE_STATUS_SUCCESS) {
          XGLOG_ERROR( "xGateDbmProcessor::process_msg dbm_msg decode or validation failed");
        }
        break;
      }
    case EN_XGATE_MSG_TO_DBM:
      {
        XGLOG_WARN("Note: DBM Dispatcher thread directly handle EN_XGATE_MSG_TO_DBM");
        break;
      }
    case EN_XGATE_MSG_UNKNOWN:
    default:
      {
        XGLOG_ERROR( "Enter xGateDbmProcessor::process_msg is EN_XGATE_MSG_UNKNOWN");
        break;
      }      
  }
  return EN_XGATE_STATUS_SUCCESS;  
}

//This function handle response message from DBM server in json formate
xGateRetVal xGateDbmProcessor::decode_message(xGateDbmMsg *pMsg)
{
  XGLOG_INFO( "Enter xGateDbmProcessor::decode_message(%s)", m_serverType); 
  const char  *data = 0;
  unsigned int dataLen = 0;

  pMsg->get_data(data);
  dataLen = pMsg->get_data_len();

  if(!data && dataLen <=0) {
    XGLOG_ERROR( "xGateDbmProcessor::decode_message invalid data in xGateDbmMsg.");
    return EN_XGATE_STATUS_ERROR;
  }

  Document doc;
  if(!parseDbmMsg(data, doc)) {
    XGLOG_ERROR( "xGateDbmProcessor::decode_message parse dbm message failed");
    return EN_XGATE_STATUS_ERROR;
  }

  xGateDbmMsg *pDbmMsg = new xGateDbmMsg();
  if(!validateDbmMsg(doc, pMsg)) {
    XGLOG_ERROR( "xGateDbmProcessor::decode_message validate dbm message failed");
    return EN_XGATE_STATUS_ERROR;
  }

  if(!post(pMsg, pDbmMsg)) {
    XGLOG_ERROR( "xGateDbmProcessor::decode_message post dbm message failed");
    return EN_XGATE_STATUS_ERROR;
  } 

  return EN_XGATE_STATUS_SUCCESS;
}

bool xGateDbmProcessor::parseDbmMsg(const char *jsonData, Document &doc)
{
  XGLOG_INFO( "xGateDbmProcessor parsing dbm json data: %s", jsonData);
  ParseResult ok = doc.Parse(jsonData);
  if(!ok) {
    XGLOG_ERROR( "xGateDbmProcessor encountered with json parse error !");
    return false;
  } else if(!doc.IsObject()) {
    XGLOG_ERROR( "xGateDbmProcessor not able create documnet object !");
    return false;
  }
  XGLOG_INFO( "xGateDbmProcessor successfully parsed client json data");
  return true;
}

bool xGateDbmProcessor::validateDbmMsg(Document &doc, xGateDbmMsg *pMsg)
{
  DbmReqDetail &reqDetail = pMsg->get_dbm_request_detail();
  if(doc.HasMember("trans_id")) {
    reqDetail.m_transId = doc["trans_id"].GetString();
  } else {
    XGLOG_ERROR( "xGateDbmProcessor validateDbmMsg failed. 'trans_id' is missing in json message !");
    return false;
  }
  if(doc.HasMember("db_operation")) {
    //reqDetail.m_opCode = doc["db_operation"].GetInt();
    reqDetail.m_opCode = EN_XGATE_DBM_MSG_DBEXECUTE; //TODO:
  } else {
    XGLOG_ERROR( "xGateDbmProcessor validateDbmMsg failed. 'db_operation' is missing in json message !");
    return false;
  }
  if(doc.HasMember("db_name")) {
    reqDetail.m_dbName = doc["db_name"].GetString();
  } else {
    XGLOG_ERROR( "xGateDbmProcessor validateDbmMsg failed. 'db_name' is missing in json message !");
    return false;
  }
  if(doc.HasMember("table_name")) {
    reqDetail.m_tableName = doc["table_name"].GetString();
  } else {
    XGLOG_ERROR( "xGateDbmProcessor validateDbmMsg failed. 'table_name' is missing in json message !");
    return false;
  }
  if(doc.HasMember("sp_name")) {
    reqDetail.m_spName = doc["sp_name"].GetString();
  } else {
    XGLOG_ERROR( "xGateDbmProcessor validateDbmMsg failed. 'sp_name' is missing in json message !");
    return false;
  }
#if 0
  if(doc.HasMember("input")) {
    reqDetail.m_input = doc["input"].GetString();
  } else {
    XGLOG_ERROR( "xGateDbmProcessor validateDbmMsg failed. 'input' is missing in json message !");
    return false;
  }
#endif
  if(doc.HasMember("output")) {
    if(doc["output"].IsObject()) {
      const Value &outputVal = doc["output"].GetObject();
      StringBuffer sb;
      Writer<StringBuffer> writer(sb);
      outputVal.Accept(writer);
      reqDetail.m_output = sb.GetString();
    } else if(doc["output"].IsArray()) {
      const Value &outputVal = doc["output"].GetArray();
      StringBuffer sb;
      Writer<StringBuffer> writer(sb);
      outputVal.Accept(writer);
      reqDetail.m_output = sb.GetString();
    }
  }

  //XGLOG_INFO( "xGateDbmProcessor input: %s", reqDetail.m_output.c_str());
  XGLOG_INFO( "xGateDbmProcessor output: %s", reqDetail.m_output.c_str());
  if(doc.HasMember("msg")) {
    reqDetail.m_statusCode = doc["msg"]["status_code"].GetInt();
    reqDetail.m_errCode = doc["msg"]["err_code"].GetInt();
    reqDetail.m_affectedRows = doc["msg"]["affected_rows"].GetInt();
    reqDetail.m_message = doc["msg"]["message"].GetString();
  } else {
    XGLOG_ERROR( "xGateDbmProcessor validateDbmMsg failed. 'msg' object is missing in json message !");
    return false;
  }

  XGLOG_INFO( "xGateDbmProcessor validateDbmMsg success !");
  return true;
}

//TODO: post decoded dbm message to bussiness layer
bool xGateDbmProcessor::post(xGateDbmMsg *pMsg, xGateDbmMsg *pDbmMsg)
{
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;

  pDbmMsg->setMsgType(EN_XGATE_MSG_FROM_DBM);

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateDbmMsg*>(pDbmMsg);
  ACE_Time_Value tval ((time(NULL)));
  tval += ACE_Time_Value(0,1);

  DbmReqDetail &reqDetail = pMsg->get_dbm_request_detail();
  pDbmMsg->set_dbm_request_detail(reqDetail);
  taskPtr = xGateUtil::getDBManager(); 

  if(taskPtr->putq(pAmb, &tval)) {
    XGLOG_INFO( "xGateDbmProcessor posted message to business layer");
  } else {
    XGLOG_ERROR( "xGateDbmProcessor failed to pose message to business layer !");
    return false;
  }
  return true;
}
