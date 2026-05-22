//local includes
#include "xglog.h"
#include "xGateDBHandler.h"
#include "xGateDBService.h"
#include "xGateDBServiceMsg.h"
#include "UREngineConfig.h"


xGateDBHandler::xGateDBHandler()
{
    m_txGateDBConnector = nullptr;
}

xGateDBHandler::~xGateDBHandler()
{
    if (m_txGateDBConnector)
    {
      delete m_txGateDBConnector;
      m_txGateDBConnector = nullptr;
    }
}

bool xGateDBHandler::init()
{
   m_tConnInfo.m_sDBSrvIp       = URENGINE_CONFIG::instance()->m_dbModuleProfile.m_server_ip.c_str();
   m_tConnInfo.m_sDBSrvPort     = URENGINE_CONFIG::instance()->m_dbModuleProfile.m_server_port;
   m_tConnInfo.m_sDBSrvUname    = URENGINE_CONFIG::instance()->m_dbModuleProfile.m_username.c_str();
   m_tConnInfo.m_sDBSrvPassword = URENGINE_CONFIG::instance()->m_dbModuleProfile.m_password.c_str();
   m_tConnInfo.m_sDBName        = URENGINE_CONFIG::instance()->m_dbModuleProfile.m_database.c_str();
   m_txGateDBConnector = new xGateDBConnector(m_tConnInfo.m_sDBSrvIp, m_tConnInfo.m_sDBSrvPort, m_tConnInfo.m_sDBSrvUname,  m_tConnInfo.m_sDBSrvPassword, m_tConnInfo.m_sDBName);

   if(m_txGateDBConnector)
   {
      XGLOG_INFO("xGateDBHandler::init success");
      return true;
    }

    XGLOG_ERROR("xGateDBHandler::init failed!");
    return false;
}

bool xGateDBHandler::reconnect_db()
{
  if(m_txGateDBConnector )
      delete m_txGateDBConnector;
  m_txGateDBConnector = new xGateDBConnector(m_tConnInfo.m_sDBSrvIp, m_tConnInfo.m_sDBSrvPort, m_tConnInfo.m_sDBSrvUname, m_tConnInfo.m_sDBSrvPassword, m_tConnInfo.m_sDBName);
  if(m_txGateDBConnector) {
     XGLOG_INFO("xGateDBHandler::reconnect_db() success"); 
     return true;
  } else {
    XGLOG_INFO("xGateDBHandler::reconnect_db() failed"); 
    return false;
  }
  return false;
}

bool xGateDBHandler::process_msg(xGateBaseMsg *pMsg)
{
   XGLOG_INFO("xGateDBHandler::process_msg");
   IURDefines::MODULE_ID srcModuleId = pMsg->getSrcModuleId();
   switch (srcModuleId)
   {
   case IURDefines::UR_MODULE_SERVICE_CONTROLLER:
   {
      return process_msg_from_controller(pMsg);
   }
   case IURDefines::UR_MODULE_MONITOR_SERVICE:
   {
      return process_msg_from_monitor_service(pMsg);
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

bool xGateDBHandler::process_msg_from_controller(xGateBaseMsg *pBaseMsg)
{
   XGLOG_INFO("xGateDBHandler::process_msg");
   xGateDBServiceMsg *pDbMsg = dynamic_cast<xGateDBServiceMsg*>(pBaseMsg);
   if(pDbMsg == nullptr)
   {
      XGLOG_ERROR("process_msg_from_controller recvd invalid Message class in DB Handler");
      return false;
   }
   DBRequestInfo &dbReqInfo = pDbMsg->get_db_request_info();
  
   std::string uId = dbReqInfo.m_contextId;
   DBServiceEvent event = dbReqInfo.m_dbEvent; 

   switch(event)
   {
      case DBServiceEvent::EN_DB_EVENT_GET_TRUNK_LIST:
      {
        do_trunk_list_request(dbReqInfo);
        break;
      } 
      case DBServiceEvent::EN_DB_EVENT_UPDATE_TRUNK_STATUS:
      {
        update_trunk_status(dbReqInfo);
        break;
      }
      default:
        break;
   }
   return true;
}

bool xGateDBHandler::process_msg_from_monitor_service(xGateBaseMsg *pBaseMsg)
{
  XGLOG_INFO("xGateDBHandler::process_msg_from_monitor_service");
  xGateDBServiceMsg *pDbMsg = dynamic_cast<xGateDBServiceMsg *>(pBaseMsg);
  if (pDbMsg == nullptr)
  {
    XGLOG_ERROR("process_msg_from_monitor_service recvd invalid Message class in DB Handler");
    return false;
  }
  DBRequestInfo &dbReqInfo = pDbMsg->get_db_request_info();

  std::string uId = dbReqInfo.m_contextId;
  DBServiceEvent event = dbReqInfo.m_dbEvent;

  switch (event)
  {
  case DBServiceEvent::EN_DB_EVENT_GET_TRUNK_LIST:
  {
    XGLOG_INFO("xGateDBHandler::process_msg_from_monitor_service received EN_DB_EVENT_GET_TRUNK_LIST");
    do_trunk_list_request(dbReqInfo);
    break;
  }
  case DBServiceEvent::EN_DB_EVENT_UPDATE_TRUNK_STATUS:
  {
    XGLOG_INFO("xGateDBHandler::process_msg_from_monitor_service received EN_DB_EVENT_UPDATE_TRUNK_STATUS");
    update_trunk_status(dbReqInfo);
    break;
  }
  case DBServiceEvent::EN_DB_EVENT_GET_MSTEAMS_DOMAIN_LIST:
  {
    XGLOG_INFO("xGateDBHandler::process_msg_from_monitor_service received EN_DB_EVENT_GET_MSTEAMS_DOMAIN_LIST");
    do_ms_teams_domain_list_request(dbReqInfo);
    break;
  }
  default:
    XGLOG_ERROR("xGateDBHandler::process_msg_from_monitor_service received unknwon event !");
    break;
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

bool xGateDBHandler::do_trunk_list_request(DBRequestInfo &dbReqInfo)
{
  std::string uid = dbReqInfo.m_contextId;
  std::string spName = dbReqInfo.m_spName;

  //Lamda Expression to clear the statement
  auto statement_failed = [&](HSTMT hstmt) ->bool {
    m_txGateDBConnector->CloseStatementHandle(hstmt);
    return false;
  };

  HSTMT hstmt;
  char queryStr[250] = {0};
  memset(queryStr, 0, sizeof(queryStr));
  snprintf(queryStr, 250, "call %s()", spName.c_str());
  if (!perform_query_operation(queryStr, hstmt)) {
     XGLOG_ERROR("do_trunk_list_request failed while executing query: '%s' !", (char *)ur_log_string(queryStr));
     return statement_failed(hstmt);
  }

  std::string sResponseData{};
  if (!get_trunk_list_result(hstmt, sResponseData)) {
     XGLOG_ERROR("do_trunk_list_request failed while reading result of query: '%s' !", (char *)ur_log_string(queryStr));
     return statement_failed(hstmt);
  }

  dbReqInfo.m_data = sResponseData.c_str();
  if (!post_result(dbReqInfo)) {
     XGLOG_ERROR("do_trunk_list_request failed while posting query result to controller !");
     return statement_failed(hstmt);
  }

  m_txGateDBConnector->CloseStatementHandle(hstmt);

  return true;
}

bool xGateDBHandler::get_trunk_list_result(HSTMT hstmt,std::string &data)
{

  Document doc;
  StringBuffer strBuf;
  Writer<StringBuffer> writer(strBuf);
  Value Array(kArrayType);
  Document::AllocatorType& allocator = doc.GetAllocator();
  doc.SetArray();

  SQLRETURN sqlReturn;
  SQLINTEGER iVal = 0;
  char sVal[500] = {0};
  memset(sVal, 0, sizeof(sVal));

  while(1) 
  {
    sqlReturn = SQLFetch(hstmt);
    if(sqlReturn == SQL_ERROR || sqlReturn == SQL_SUCCESS_WITH_INFO) {
      XGLOG_INFO("got sql error while reading trunk list query result\n");
      return false;
    }
    Value outputVal(kObjectType);
    if(sqlReturn == SQL_SUCCESS || sqlReturn == SQL_SUCCESS_WITH_INFO) {  
      SQLGetData(hstmt, 2, SQL_C_CHAR, &sVal, sizeof(sVal), nullptr);
      outputVal.AddMember("domain_whitelist", setJsonParam(sVal,doc), allocator);
      memset(sVal, 0, sizeof(sVal));
      SQLGetData(hstmt, 4, SQL_C_CHAR, &sVal, sizeof(sVal), nullptr);
      outputVal.AddMember("dc_type", setJsonParam(sVal,doc), allocator);  
      memset(sVal, 0, sizeof(sVal));
      SQLGetData(hstmt, 5, SQL_C_SLONG, &iVal, sizeof(iVal), nullptr);
      outputVal.AddMember("isActive", setJsonParam(iVal,doc), allocator);
      iVal = 0;
      SQLGetData(hstmt, 6, SQL_C_SLONG, &iVal, sizeof(iVal), nullptr);
      outputVal.AddMember("trunk_id", setJsonParam(iVal,doc), allocator);
      iVal = 0;
      Array.PushBack(outputVal, allocator);
    } else {
      break;
    }
  }

  Array.Accept(writer);
  data = strBuf.GetString();

  std::cout << data << std::endl;
  return true;
}

bool xGateDBHandler::update_trunk_status(DBRequestInfo &dbReqInfo)
{

  std::string data = dbReqInfo.m_data.c_str();
  std::string spName = dbReqInfo.m_spName.c_str();
  std::string uid = dbReqInfo.m_contextId.c_str();
  std::string localIP{};
  std::string providerIP{};
  std::string dcType{};
  int trunkID{};
  int activeStatus{};
  int errorCode{};
  int responseCode{};

  Document doc;
  if (!parseJsonMsg(data.c_str(), doc))
  {
    XGLOG_ERROR("update_trunk_status failed to json data!");
    return false;
  }

  XGLOG_INFO("xGateDBHandler::update_trunk_status callid(%s) received: %s", uid.c_str(), data.c_str());
  // Lamda Expression to print the error
  auto parsing_failed = [&](std::string error) -> bool
  {
    XGLOG_ERROR("xGateDBHandler::update_trunk_status parsing failed '%s' is not available from json!", error.c_str());
    return false;
  };

  if (doc.HasMember("trunk_id"))
    trunkID = doc["trunk_id"].GetInt();
  else
    return parsing_failed("trunk_id");

  if (doc.HasMember("dc_type"))
    dcType = doc["dc_type"].GetString();
  else
    return parsing_failed("dc_type");

  if (doc.HasMember("provider_ip"))
    providerIP = doc["provider_ip"].GetString();
  else
    return parsing_failed("provider_ip");

  if (doc.HasMember("local_ip"))
    localIP = doc["local_ip"].GetString();
  else
    return parsing_failed("local_ip");

  if (doc.HasMember("isActive"))
    activeStatus = doc["isActive"].GetInt();
  else
    return parsing_failed("isActive");

  if (doc.HasMember("respons_code"))
    responseCode = doc["respons_code"].GetInt();
  else
    return parsing_failed("error_code");

  HSTMT hstmt;
  char queryStr[250] = {0};
  memset(queryStr, 0, sizeof(queryStr));

  snprintf(queryStr, 250, "call %s('%s','%s','%s','%d',%d,%d)", spName.c_str(), localIP.c_str(),providerIP.c_str(),dcType.c_str(),trunkID,activeStatus, responseCode);
  if (!perform_query_operation(queryStr, hstmt))
  {
    XGLOG_ERROR("update_trunk_status failed for callid(%s) while executing query: '%s' !", uid.c_str(), (char *)ur_log_string(queryStr));
     m_txGateDBConnector->CloseStatementHandle(hstmt);
  } else {
    XGLOG_INFO("update_trunk_status success for callid(%s) query: '%s' ", uid.c_str(), (char *)ur_log_string(queryStr));
  }
  m_txGateDBConnector->CloseStatementHandle(hstmt);

  return true;
}

bool xGateDBHandler::post_result(DBRequestInfo &dbOutReqInfo)
{
	xGateDBServiceMsg *ptdbMsg = new xGateDBServiceMsg();

  if (!ptdbMsg)
  {
    XGLOG_ERROR("xGateDBHandler::post_result failed to create xGateDBServiceMsg!");
    return false;
  }

  DBRequestInfo &dbReqInfo = ptdbMsg->get_db_request_info();
	dbReqInfo.m_contextId = dbOutReqInfo.m_contextId;
	dbReqInfo.m_data = dbOutReqInfo.m_data;
	dbReqInfo.m_output = dbOutReqInfo.m_output;
  dbReqInfo.m_dbEvent = dbOutReqInfo.m_dbEvent;
  dbReqInfo.m_data = dbReqInfo.m_data;
	dbReqInfo.m_requestorModule = dbOutReqInfo.m_requestorModule;
	ptdbMsg->setDstModuleId(dbReqInfo.m_requestorModule);
	ptdbMsg->setSrcModuleId(IURDefines::UR_MODULE_DB_SERVICE);
  ptdbMsg->setMsgType(EN_XGATE_MSG_DB_MODULE);
	ptdbMsg->setUid(dbReqInfo.m_contextId);
	DBSERVICE->getConfig()->getCallBack()->handleModuleCallbackMsg(ptdbMsg);
	return true;
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
  return retVal;
}


/*
 * Parse json message as doc
 */
bool xGateDBHandler::parseJsonMsg(const char *jsonData, Document &doc)
{
  XGLOG_INFO("parsing business json data: %s", jsonData);
  ParseResult ok = doc.Parse(jsonData);

  if (!ok)
  {
    XGLOG_ERROR("parseJsonMsg failed. Encountered with json parse error !");
    return false;
  }
  else if (!doc.IsObject())
  {
    XGLOG_ERROR("parseJsonMsg failed. Not able create documnet object !");
    return false;
  }
  return true;
}

bool xGateDBHandler::do_ms_teams_domain_list_request(DBRequestInfo &dbReqInfo)
{
  std::string uid = dbReqInfo.m_contextId;
  std::string spName = dbReqInfo.m_spName;

  //Lamda Expression to clear the statement
  auto statement_failed = [&](HSTMT hstmt) ->bool {
    m_txGateDBConnector->CloseStatementHandle(hstmt);
    return false;
  };

  HSTMT hstmt;
  char queryStr[250] = {0};
  memset(queryStr, 0, sizeof(queryStr));
  snprintf(queryStr, 250, "call %s()", spName.c_str());
  if (!perform_query_operation(queryStr, hstmt)) {
     XGLOG_ERROR("do_ms_teams_domain_list_request failed while executing query: '%s' !", (char *)ur_log_string(queryStr));
     return statement_failed(hstmt);
  }

  std::string sResponseData{};
  if (!get_ms_teams_list_result(hstmt, sResponseData)) {
     XGLOG_ERROR("do_ms_teams_domain_list_request failed while reading result of query: '%s' !", (char *)ur_log_string(queryStr));
     return statement_failed(hstmt);
  }

  dbReqInfo.m_data = sResponseData.c_str();
  if (!post_result(dbReqInfo)) {
     XGLOG_ERROR("do_ms_teams_domain_list_request failed while posting query result to controller !");
     return statement_failed(hstmt);
  }

  m_txGateDBConnector->CloseStatementHandle(hstmt);

  return true;
}

bool xGateDBHandler::get_ms_teams_list_result(HSTMT hstmt,std::string &data)
{

  Document doc;
  StringBuffer strBuf;
  Writer<StringBuffer> writer(strBuf);
  Value Array(kArrayType);
  Document::AllocatorType& allocator = doc.GetAllocator();
  doc.SetArray();

  SQLRETURN sqlReturn;
  char sVal[100] = {0};
  memset(sVal, 0, sizeof(sVal));

  while(1)
  {
    sqlReturn = SQLFetch(hstmt);
    if(sqlReturn == SQL_ERROR || sqlReturn == SQL_SUCCESS_WITH_INFO) {
      XGLOG_INFO("got sql error while reading trunk list query result\n");
      return false;
    }
    if(sqlReturn == SQL_SUCCESS || sqlReturn == SQL_SUCCESS_WITH_INFO) {
      SQLGetData(hstmt, 1, SQL_C_CHAR, &sVal, sizeof(sVal), nullptr);
      Array.PushBack(Value(sVal, allocator).Move(), allocator);
      memset(sVal, 0, sizeof(sVal));
    } else {
      break;
    }
  }

  Array.Accept(writer);
  data = strBuf.GetString();

  std::cout << data << std::endl;
  XGLOG_INFO("xGateDBHandler::get_ms_teams_list_result json data: %s",data.c_str());
  return true;
}