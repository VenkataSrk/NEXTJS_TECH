//local includes
#include "xglog.h"
#include "xGateDBHandler.h"
#include "xGateDBService.h"
#include "xGateDBServiceMsg.h"
#include "xGConfig.h"

xGateDBHandler::xGateDBHandler()
{
    m_txGateDBConnector = NULL;
}

xGateDBHandler::~xGateDBHandler()
{
    if (m_txGateDBConnector)
    {
      delete m_txGateDBConnector;
      m_txGateDBConnector = NULL;
    }
}

bool xGateDBHandler::init()
{
    m_tConnInfo.m_sDBSrvIp = XGCONFIG().m_dbProfile.m_db_srv_ip.c_str();
    m_tConnInfo.m_sDBSrvPort = XGCONFIG().m_dbProfile.m_db_srv_port;
    m_tConnInfo.m_sDBSrvUname = XGCONFIG().m_dbProfile.m_db_srv_uname.c_str();
    m_tConnInfo.m_sDBSrvPassword = XGCONFIG().m_dbProfile.m_db_srv_password.c_str();
    m_tConnInfo.m_sDBName = XGCONFIG().m_dbProfile.m_db_name.c_str();

    m_tCcaasDbConnInfo.m_sDBSrvIp = XGCONFIG().m_ccaasDbProfile.m_db_srv_ip.c_str();
    m_tCcaasDbConnInfo.m_sDBSrvPort = XGCONFIG().m_ccaasDbProfile.m_db_srv_port;
    m_tCcaasDbConnInfo.m_sDBSrvUname = XGCONFIG().m_ccaasDbProfile.m_db_srv_uname.c_str();
    m_tCcaasDbConnInfo.m_sDBSrvPassword = XGCONFIG().m_ccaasDbProfile.m_db_srv_password.c_str();
    m_tCcaasDbConnInfo.m_sDBName = XGCONFIG().m_ccaasDbProfile.m_db_name.c_str();

    m_tCrmDbConnInfo.m_sDBSrvIp = XGCONFIG().m_crmDbProfile.m_db_srv_ip.c_str();
    m_tCrmDbConnInfo.m_sDBSrvPort = XGCONFIG().m_crmDbProfile.m_db_srv_port;
    m_tCrmDbConnInfo.m_sDBSrvUname = XGCONFIG().m_crmDbProfile.m_db_srv_uname.c_str();
    m_tCrmDbConnInfo.m_sDBSrvPassword = XGCONFIG().m_crmDbProfile.m_db_srv_password.c_str();
    m_tCrmDbConnInfo.m_sDBName = XGCONFIG().m_crmDbProfile.m_db_name.c_str();



      m_txGateDBConnector = new xGateDBConnector(m_tConnInfo.m_sDBSrvIp, m_tConnInfo.m_sDBSrvPort, m_tConnInfo.m_sDBSrvUname, m_tConnInfo.m_sDBSrvPassword, m_tConnInfo.m_sDBName, DBConnectionType::EN_DB_CONNECTION_TYPE_MYSQL);

    if (m_txGateDBConnector)
    {
      XGLOG_INFO("xGateDBHandler::init success for Ucaas DB");
      //return true;
    }

    m_txGateCcaasDBConnector = new xGateDBConnector(m_tCcaasDbConnInfo.m_sDBSrvIp, m_tCcaasDbConnInfo.m_sDBSrvPort, m_tCcaasDbConnInfo.m_sDBSrvUname, m_tCcaasDbConnInfo.m_sDBSrvPassword, m_tCcaasDbConnInfo.m_sDBName, DBConnectionType::EN_DB_CONNECTION_TYPE_MYSQL);

    if (m_txGateCcaasDBConnector)
    {
      XGLOG_INFO("xGateDBHandler::init success for Ccaas DB");
      //return true;
    }

    m_txGateCrmDBConnector = new xGateDBConnector(m_tCrmDbConnInfo.m_sDBSrvIp, m_tCrmDbConnInfo.m_sDBSrvPort, m_tCrmDbConnInfo.m_sDBSrvUname, m_tCrmDbConnInfo.m_sDBSrvPassword, m_tCrmDbConnInfo.m_sDBName, DBConnectionType::EN_DB_CONNECTION_TYPE_POSTGRESQL);
    if (m_txGateCrmDBConnector)
    {
      XGLOG_INFO("xGateDBHandler::init success for CRM DB");
      return true;
    }

    XGLOG_ERROR("xGateDBHandler::init failed!");
    return false;
}

bool xGateDBHandler::reconnect_db()
{
  if(m_txGateDBConnector )
      delete m_txGateDBConnector;
  m_txGateDBConnector = new xGateDBConnector(m_tConnInfo.m_sDBSrvIp, m_tConnInfo.m_sDBSrvPort, m_tConnInfo.m_sDBSrvUname, m_tConnInfo.m_sDBSrvPassword, m_tConnInfo.m_sDBName, DBConnectionType::EN_DB_CONNECTION_TYPE_MYSQL);
  if(m_txGateDBConnector) {
     XGLOG_INFO("xGateDBHandler::connect_db() success"); 
     return true;
  } else {
    XGLOG_INFO("xGateDBHandler::connect_db() failed"); 
    return false;
  }
  return false;
}

bool xGateDBHandler::process_msg(xGBaseMsg *pMsg)
{
   XGLOG_INFO("xGateDBHandler::process_msg");
   IURDefines::MODULE_ID srcModuleId = pMsg->getSrcModuleId();
   switch (srcModuleId)
   {
   case IURDefines::UR_MODULE_SERVICE_CONTROLLER:
   {
      return process_msg_from_controller(pMsg);
   }
   case IURDefines::UR_MODULE_UNKNOWN:
   {
      break;
   }
   default:
   {
      XGLOG_ERROR("process_msg failed. Unknown source module id !");
      return false;
   }
   }
   return true;
}

bool xGateDBHandler::process_msg_from_controller(xGBaseMsg *pBaseMsg)
{
   XGLOG_INFO("xGateDBHandler::process_msg");
   xGateDBServiceMsg *pDbMsg = dynamic_cast<xGateDBServiceMsg*>(pBaseMsg);
   if(pDbMsg == NULL)
   {
      XGLOG_ERROR("process_msg_from_controller recvd invalid Message class in DB Handler");
      return false;
   }
   DBRequestInfo &dbReqInfo = pDbMsg->get_db_request_info();

  //  std::string strData = dbReqInfo.m_data;
  //  Document doc;
  //  if(!parseJsonMsg(strData.c_str(), doc)) {
  //     XGLOG_ERROR( "process_business_request failed to parse business message !");
  //     return false;
  //  }
 
  // Value inputVal;
  // if(!getInputDoc(doc, inputVal)) {
  //   XGLOG_ERROR( "process_business_request failed. not able to get 'input' object from main json object !");
  //   return false;
  // }   
  
   std::string uId = dbReqInfo.m_contextId;
   DBServiceEvent event = dbReqInfo.m_dbEvent; 

   switch(event)
   {
    case EN_DB_EVENT_CALL_FORWARDING_QUERY:
    {
        break;
    }
    case EN_DB_EVENT_STORE_CALL_REC_DATA_QUERY:
    {
      if(!store_call_record_data(dbReqInfo)){
        XGLOG_ERROR("store_call_record_data event(EN_DB_EVENT_STORE_CALL_REC_DATA_QUERY) query failed for callid(%s)",uId.c_str());
      } else {
       XGLOG_INFO("store_call_record_data query event(EN_DB_EVENT_STORE_CALL_REC_DATA_QUERY) success for callid(%s)",uId.c_str());
      }
      break;
    }
    case EN_DB_EVENT_STORE_CCAAS_CALL_REC_DATA_QUERY:
    {
      if(!store_call_record_data(dbReqInfo)){
        XGLOG_ERROR("store_call_record_data event(EN_DB_EVENT_STORE_CCAAS_CALL_REC_DATA_QUERY) query failed for callid(%s)",uId.c_str());
      } else {
       XGLOG_INFO("store_call_record_data event(EN_DB_EVENT_STORE_CCAAS_CALL_REC_DATA_QUERY) query success for callid(%s)",uId.c_str());
      }
      break;
    }
    case EN_DB_EVENT_STORE_CRM_CALL_REC_DATA_QUERY:
    {
      if(!store_call_record_data(dbReqInfo)){
        XGLOG_ERROR("store_call_record_data event(EN_DB_EVENT_STORE_CRM_CALL_REC_DATA_QUERY) query failed for callid(%s)",uId.c_str());
      } else {
       XGLOG_INFO("store_call_record_data event(EN_DB_EVENT_STORE_CRM_CALL_REC_DATA_QUERY) query success for callid(%s)",uId.c_str());
      }
      break;
    }
    default:
    {
        break;
    }
   }
   return true;
}

bool xGateDBHandler::perform_query_operation(const char *queryStr, HSTMT &hstmt)
{
  std::unique_lock<std::mutex> lock(_mutex);//lock for m_txGateDBConnector to avoid concurrency 
  if(!m_txGateDBConnector->execute_query(queryStr,hstmt)) {
     XGLOG_ERROR("xGateDBHandler::perform_query_operation failed to execute query(%s)",queryStr);
      m_txGateDBConnector->CloseStatementHandle(hstmt);
     if(check_db_connection()) {
        XGLOG_INFO("xGateDBHandler::perform_query_operation DB Connection still alive ,so issue with query(%s)",queryStr);
        return false;
     } else {
        if(!retry_failed_query(queryStr,hstmt)){
          XGLOG_INFO("xGateDBHandler::perform_query_operation retry failed for query(%s) ,DB connection not available",queryStr);
          m_txGateDBConnector->CloseStatementHandle(hstmt);
        }
     }  
  }
  return true;
}

bool xGateDBHandler::do_call_forward_request(Document &doc, Value &inputVal, DBRequestInfo &dbReqInfo)
{
  std::string uid  = dbReqInfo.m_contextId;
  std::string spName = dbReqInfo.m_spName;

  std::string ddi(""); 
  if(inputVal.HasMember("ddi")) {
    ddi = inputVal["ddi"].GetString();
  } else {
    XGLOG_ERROR( "do_call_forward_request failed. ddi is empty for uid: %s", (char*)ur_log_string(uid));
    return false;
  }

  std::string domainId(""); 
  if(inputVal.HasMember("domain_id")) {
    domainId = inputVal["domain_id"].GetString();
  } else {
    XGLOG_ERROR( "do_call_forward_request failed. domain_id is empty for uid: %s", (char*)ur_log_string(uid));
    return false;
  }

  if(ddi.empty() || domainId.empty()) { //Note: for PSTN call domain id will not be there
    XGLOG_ERROR( "do_call_forward_request failed. either ddi/domainId value is empty !");
    return false;
  }

  HSTMT hstmt;
  char queryStr[250]={0};
  memset(queryStr, 0, sizeof(queryStr));
  snprintf(queryStr, 250, "exec %s '%s', '%s'", (char*)ur_log_string(spName), (char*)ur_log_string(domainId),(char*)ur_log_string(ddi));
  
  //execute query and process query output
  if(!m_txGateDBConnector->execute_query(queryStr, hstmt)) {
    XGLOG_ERROR( "do_call_forward_request failed while executing query: '%s' !", (char*)ur_log_string(queryStr));
    m_txGateDBConnector->CloseStatementHandle(hstmt);
    return false;
  }

  string output("");
  if(!get_call_forward_result(hstmt, output))
  {
    XGLOG_ERROR( "do_call_forward_request failed while reading result of query: '%s' !", (char*)ur_log_string(queryStr));
    return false;
  }

  dbReqInfo.m_output = output.c_str();

  if(!post_result(dbReqInfo)) {
    XGLOG_ERROR( "do_call_forward_request failed while posting call forward query result to business layer !");
    return false;
  }

  return true;
}

bool xGateDBHandler::get_call_forward_result(HSTMT hstmt, string &output)
{
  Document doc;
  StringBuffer strBuf;
  Writer<StringBuffer> writer(strBuf);
  Value val(kObjectType);
  Document::AllocatorType& allocator = doc.GetAllocator();

  RETCODE ret;
  SQLRETURN sqlReturn;
  SQLINTEGER iVal = 0;
  char sVal[50] = {0};
  memset(sVal, 0, sizeof(sVal));

  Value outputVal(kObjectType);

  while(1) {
    sqlReturn = SQLFetch(hstmt);
    if(sqlReturn == SQL_ERROR || sqlReturn == SQL_SUCCESS_WITH_INFO) {
      XGLOG_INFO("got sql error while reading call forward query result\n");
      m_txGateDBConnector->CloseStatementHandle(hstmt);
      return false;
    }
    if(sqlReturn == SQL_SUCCESS || sqlReturn == SQL_SUCCESS_WITH_INFO) {
      ret = SQLGetData(hstmt, EN_INCOMING_CALL_TYPE, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);
      outputVal.AddMember("incoming_call_type", setJsonParam(iVal,doc), allocator);
      ret = SQLGetData(hstmt, EN_RING_TYPE, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);
      outputVal.AddMember("ring_type", setJsonParam(sVal,doc), allocator);
      ret = SQLGetData(hstmt, EN_NAME, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);
      outputVal.AddMember("name", setJsonParam(sVal,doc), allocator);
      ret = SQLGetData(hstmt, EN_FORWARD_TYPE, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);
      outputVal.AddMember("forward_type", setJsonParam(iVal,doc), allocator);
      ret = SQLGetData(hstmt, EN_FORWARD_NUMBER, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);
      outputVal.AddMember("forward_number", setJsonParam(sVal,doc), allocator);
      ret = SQLGetData(hstmt, EN_CALL_FORWARD_ORDER, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);
      outputVal.AddMember("call_forward_order", setJsonParam(iVal,doc), allocator);
    } else {
      break;
    }
  }
  
  val.AddMember("output", outputVal, allocator); 
  //outputVal.Accept(writer);
  val.Accept(writer);
  output = strBuf.GetString();
  XGLOG_INFO("call forward query result output: %s\n", output.c_str());
  m_txGateDBConnector->CloseStatementHandle(hstmt);
  return true;
}

bool xGateDBHandler::store_call_record_data(DBRequestInfo &dbReqInfo)
{
  std::string uid = dbReqInfo.m_contextId;
  std::string spName = dbReqInfo.m_spName;
  DBServiceEvent event = dbReqInfo.m_dbEvent;
  HSTMT hstmt;
  size_t sz = sizeof(dbReqInfo.m_data);
  char queryStr[512] = {0};
  memset(queryStr, 0, sizeof(queryStr));

  switch (event)
  {
  case EN_DB_EVENT_STORE_CALL_REC_DATA_QUERY:
  {
    snprintf(queryStr, 512, "call %s (%s)",spName.c_str(), dbReqInfo.m_data.c_str());
    if (!perform_query_operation(queryStr, hstmt))
    {
      XGLOG_ERROR("store_call_record_data failed while executing query: '%s' !", (char *)ur_log_string(queryStr));
      return false;
    }
    m_txGateDBConnector->CloseStatementHandle(hstmt);
    break;
  }
  case EN_DB_EVENT_STORE_CCAAS_CALL_REC_DATA_QUERY:
  {
    snprintf(queryStr, 512, "call %s (%s)",spName.c_str(), dbReqInfo.m_data.c_str());
    if (!perform_ccaas_query_operation(queryStr, hstmt))
    {
      XGLOG_ERROR("store_call_record_data failed while executing query: '%s' !", (char *)ur_log_string(queryStr));
      return false;
    }
    m_txGateCcaasDBConnector->CloseStatementHandle(hstmt);
    break;
  }
  case EN_DB_EVENT_STORE_CRM_CALL_REC_DATA_QUERY:
  {
    snprintf(queryStr, 512, "call %s (%s,'srs_cursor')",spName.c_str(), dbReqInfo.m_data.c_str()); //cursor should be there for postgres sp call
    if (!perform_crm_query_operation(queryStr, hstmt))
    {
      XGLOG_ERROR("store_call_record_data failed while executing query: '%s' !", (char *)ur_log_string(queryStr));
      return false;
    }
    m_txGateCrmDBConnector->CloseStatementHandle(hstmt);
    break;
  }
  default:
  {
    break;
  }
  }
  return true;
}

bool xGateDBHandler::post_result(DBRequestInfo &dbOutReqInfo)
{
	xGateDBServiceMsg *ptdbMsg = new xGateDBServiceMsg();
	if(!ptdbMsg){
		XGLOG_ERROR("xGateDBHandler::post_result failed to create xGateDBServiceMsg!");
		return false;
	}
	DBRequestInfo &dbReqInfo = ptdbMsg->get_db_request_info();
	dbReqInfo.m_contextId = dbOutReqInfo.m_contextId;
	dbReqInfo.m_data = dbOutReqInfo.m_data;
	dbReqInfo.m_output = dbOutReqInfo.m_output;
	dbReqInfo.m_requestorModule = dbOutReqInfo.m_requestorModule;
	ptdbMsg->setDstModuleId(dbReqInfo.m_requestorModule);
	ptdbMsg->setSrcModuleId(IURDefines::UR_MODULE_DB_SERVICE);
	ptdbMsg->setUid(dbReqInfo.m_contextId);
	DBSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(ptdbMsg);
	return true;
}

bool xGateDBHandler::parseJsonMsg(const char *jsonData, Document &doc)
{
  XGLOG_INFO( "parsing business json data: %s", jsonData);
  ParseResult ok = doc.Parse(jsonData);
  if(!ok) {
    XGLOG_ERROR( "parseJsonMsg failed. Encountered with json parse error !");
    return false;
  } else if(!doc.IsObject()) {
    XGLOG_ERROR( "parseJsonMsg failed. Not able create documnet object !");
    return false;
  }

  XGLOG_INFO( "successfully parsed business json data");
  return true;
}

bool xGateDBHandler::getInputDoc(Document &doc, Value &inputVal)
{
  if(doc.HasMember("input")) {
    inputVal = doc["input"].GetObject();
    return true;
  }
  return false;
}

Value xGateDBHandler::setJsonParam(string &strValue, Document &doc)
{
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
  return retVal;
}

Value xGateDBHandler::setJsonParam(int iValue, Document &doc)
{
  Value retVal;
  retVal = iValue;
  return retVal;
}

Value xGateDBHandler::setJsonParam(char* chrValue, Document &doc)
{
  Value retVal;
  string tmp(chrValue);
  retVal.SetString(tmp.c_str(), tmp.length(), doc.GetAllocator());
  memset(chrValue, 0, sizeof(chrValue)); //resetting
  return retVal;
}


bool xGateDBHandler::check_db_connection()
{
  HSTMT hstmt;
  char queryStr[12] = {0};
  memset(queryStr, 0, sizeof(queryStr));
  snprintf(queryStr, 15, "SELECT 1");
  if(!m_txGateDBConnector->execute_query(queryStr, hstmt)) {
    XGLOG_ERROR( "check_db_connection failed while executing query: '%s' !", (char*)ur_log_string(queryStr));
    m_txGateDBConnector->CloseStatementHandle(hstmt);
    return false;
   }
   m_txGateDBConnector->CloseStatementHandle(hstmt);
   XGLOG_ERROR( "check_db_connection success ,connection still alive");
  return true;
}

bool xGateDBHandler::retry_failed_query(const char *queryStr, HSTMT &hstmt)
{
  XGLOG_INFO("xGateDBHandler::retry_failed_query started to reconnect the DB");
  if (reconnect_db()) {
    if (!m_txGateDBConnector->execute_query(queryStr, hstmt)) {
      XGLOG_ERROR("retry_failed_query query execution failed ,after DB reconnection: '%s' !", (char *)ur_log_string(queryStr));
      // m_txGateDBConnector->CloseStatementHandle(hstmt);
      return false;
    }
    return true;
  }
  return false;
}

bool xGateDBHandler::perform_ccaas_query_operation(const char *queryStr, HSTMT &hstmt)
{
  std::unique_lock<std::mutex> lock(_mutex);//lock for m_txGateDBConnector to avoid concurrency
  if(!m_txGateCcaasDBConnector->execute_query(queryStr,hstmt)) {
     XGLOG_ERROR("xGateDBHandler::perform_ccaas_query_operation failed to execute query(%s)",queryStr);
      m_txGateCcaasDBConnector->CloseStatementHandle(hstmt);
     if(check_ccaas_db_connection()) {
        XGLOG_INFO("xGateDBHandler::perform_ccaas_query_operation DB Connection still alive ,so issue with query(%s)",queryStr);
        return false;
     } else {
        if(!retry_ccaas_failed_query(queryStr,hstmt)){
          XGLOG_INFO("xGateDBHandler::perform_ccaas_query_operation retry failed for query(%s) ,DB connection not available",queryStr);
          m_txGateCcaasDBConnector->CloseStatementHandle(hstmt);
        }
     }
  }
  return true;
}

bool xGateDBHandler::check_ccaas_db_connection()
{
  HSTMT hstmt;
  char queryStr[12] = {0};
  memset(queryStr, 0, sizeof(queryStr));
  snprintf(queryStr, 15, "SELECT 1");
  if(!m_txGateCcaasDBConnector->execute_query(queryStr, hstmt)) {
    XGLOG_ERROR( "check_ccaas_db_connection failed while executing query: '%s' !", (char*)ur_log_string(queryStr));
    m_txGateCcaasDBConnector->CloseStatementHandle(hstmt);
    return false;
   }
   m_txGateCcaasDBConnector->CloseStatementHandle(hstmt);
   XGLOG_ERROR( "check_ccaas_db_connection success ,connection still alive");
  return true;
}

bool xGateDBHandler::retry_ccaas_failed_query(const char *queryStr, HSTMT &hstmt)
{
  XGLOG_INFO("xGateDBHandler::retry_ccaas_failed_query started to reconnect the DB");
  if (reconnect_ccaas_db()) {
    if (!m_txGateCcaasDBConnector->execute_query(queryStr, hstmt)) {
      XGLOG_ERROR("retry_ccaas_failed_query query execution failed ,after DB reconnection: '%s' !", (char *)ur_log_string(queryStr));
      return false;
    }
    return true;
  }
  return false;
}

bool xGateDBHandler::reconnect_ccaas_db()
{
  if(m_txGateCcaasDBConnector )
      delete m_txGateCcaasDBConnector;
  m_txGateCcaasDBConnector = new xGateDBConnector(m_tCcaasDbConnInfo.m_sDBSrvIp, m_tCcaasDbConnInfo.m_sDBSrvPort, m_tCcaasDbConnInfo.m_sDBSrvUname, m_tCcaasDbConnInfo.m_sDBSrvPassword, m_tCcaasDbConnInfo.m_sDBName, DBConnectionType::EN_DB_CONNECTION_TYPE_MYSQL);
  if(m_txGateCcaasDBConnector) {
     XGLOG_INFO("xGateDBHandler::reconnect_ccaas_db() success");
     return true;
  } else {
    XGLOG_INFO("xGateDBHandler::reconnect_ccaas_db() failed");
    return false;
  }
  return false;
}

bool xGateDBHandler::perform_crm_query_operation(const char *queryStr, HSTMT &hstmt)
{
  std::unique_lock<std::mutex> lock(_mutex);//lock for m_txGateCrmDBConnector to avoid concurrency
  if(!m_txGateCrmDBConnector->execute_query(queryStr,hstmt)) {
     XGLOG_ERROR("xGateDBHandler::perform_crm_query_operation failed to execute query(%s)",queryStr);
      m_txGateCrmDBConnector->CloseStatementHandle(hstmt);
     if(check_crm_db_connection()) {
        XGLOG_INFO("xGateDBHandler::perform_crm_query_operation DB Connection still alive ,so issue with query(%s)",queryStr);
        return false;
     } else {
        if(!retry_crm_failed_query(queryStr,hstmt)){
          XGLOG_INFO("xGateDBHandler::perform_crm_query_operation retry failed for query(%s) ,DB connection not available",queryStr);
          m_txGateCrmDBConnector->CloseStatementHandle(hstmt);
        }
     }
  }
  return true;
}

bool xGateDBHandler::check_crm_db_connection()
{
  HSTMT hstmt;
  char queryStr[12] = {0};
  memset(queryStr, 0, sizeof(queryStr));
  snprintf(queryStr, 15, "SELECT 1");
  if(!m_txGateCrmDBConnector->execute_query(queryStr, hstmt)) {
    XGLOG_ERROR( "check_crm_db_connection failed while executing query: '%s' !", (char*)ur_log_string(queryStr));
    m_txGateCrmDBConnector->CloseStatementHandle(hstmt);
    return false;
   }
   m_txGateCrmDBConnector->CloseStatementHandle(hstmt);
   XGLOG_ERROR( "check_crm_db_connection success ,connection still alive");
  return true;
}

bool xGateDBHandler::retry_crm_failed_query(const char *queryStr, HSTMT &hstmt)
{
  XGLOG_INFO("xGateDBHandler::retry_crm_failed_query started to reconnect the DB");
  if (reconnect_crm_db()) {
    if (!m_txGateCrmDBConnector->execute_query(queryStr, hstmt)) {
      XGLOG_ERROR("retry_crm_failed_query query execution failed ,after DB reconnection: '%s' !", (char *)ur_log_string(queryStr));
      return false;
    }
    return true;
  }
  return false;
}

bool xGateDBHandler::reconnect_crm_db()
{
  if(m_txGateCrmDBConnector )
      delete m_txGateCrmDBConnector;
  m_txGateCrmDBConnector = new xGateDBConnector(m_tCrmDbConnInfo.m_sDBSrvIp, m_tCrmDbConnInfo.m_sDBSrvPort, m_tCrmDbConnInfo.m_sDBSrvUname, m_tCrmDbConnInfo.m_sDBSrvPassword, m_tCrmDbConnInfo.m_sDBName, DBConnectionType::EN_DB_CONNECTION_TYPE_POSTGRESQL);
  if(m_txGateCrmDBConnector) {
     XGLOG_INFO("xGateDBHandler::reconnect_crm_db() success");
     return true;
  } else {
    XGLOG_INFO("xGateDBHandler::reconnect_crm_db() failed");
    return false;
  }
  return false;
}