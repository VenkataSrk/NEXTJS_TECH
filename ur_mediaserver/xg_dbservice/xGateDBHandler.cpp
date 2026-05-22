//local includes
#include "xglog.h"
#include "xGateDBHandler.h"
#include "xGateDBService.h"
#include "xGateDBServiceMsg.h"
#include "xGateBaseMsg.h"

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
	 auto itr = XG_CONFIG::instance()->m_dbModProfile.m_mapDbConnInfo.begin();
	 for(itr;itr != XG_CONFIG::instance()->m_dbModProfile.m_mapDbConnInfo.end();itr++)
	 {
	 		xGateDBConnector* dbConnector = nullptr;
			DBConnInfo* dbConnInfo = itr->second;
		  if(!dbConnInfo->m_db_server_ip.empty())	
			{
			  auto connItr = m_dbConnMap.find(dbConnInfo->m_db_server_ip);
				if(connItr == m_dbConnMap.end())
				{
					dbConnector = new xGateDBConnector(dbConnInfo);				
					m_dbConnMap.insert(DB_CONN_MAP::value_type(dbConnInfo->m_db_server_ip, dbConnector));
					XGLOG_INFO("xGateDBHandler::init DB connection success for server_ip:%s", dbConnInfo->m_db_server_ip.c_str());
				}
				else
				{
					XGLOG_INFO("xGateDBHandler::init Already DB connection exist with server_ip:%s", dbConnInfo->m_db_server_ip.c_str());	
				}
			}
	 }  
	 return true;
}

bool xGateDBHandler::reconnect_db(xGateDBConnector *& dbConnector)
{
  std::string strDbSrvrIpAddr("");
	DBConnInfo* dbConnInfo = new DBConnInfo();
  if(dbConnector)
	{
			dbConnInfo = dbConnector->m_dbConnInfo;
			auto itr = m_dbConnMap.find(dbConnInfo->m_db_server_ip);
			if(itr != m_dbConnMap.end())
			{
				m_dbConnMap.erase(dbConnInfo->m_db_server_ip);
				XGLOG_INFO("==> reconnect_db() erasing entry from connection map for server_ip:%s", dbConnInfo->m_db_server_ip.c_str());
			}
      delete dbConnector;
			dbConnector = nullptr;
	}
	else
	{
    	XGLOG_INFO("xGateDBHandler::reconnect_db() failed"); 
			return false;
	}
	dbConnector = new xGateDBConnector(dbConnInfo);				
  if(dbConnector) {
     XGLOG_INFO("==> xGateDBHandler::reconnect_db() success for server_ip:%s", dbConnInfo->m_db_server_ip.c_str());
		 m_dbConnMap.insert(DB_CONN_MAP::value_type(dbConnInfo->m_db_server_ip, dbConnector));
     return true;
  } else {
    XGLOG_INFO("xGateDBHandler::reconnect_db() failed for server_ip:%s", dbConnInfo->m_db_server_ip.c_str()); 
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
   case IURDefines::UR_MODULE_SFU_SERVICE:
   {
      return process_msg_from_sfu_controller(pMsg);
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

bool xGateDBHandler::process_msg_from_sfu_controller(xGateBaseMsg *pBaseMsg)
{
   XGLOG_INFO("xGateDBHandler::process_msg_from_sfu_controller");
  
   xGateDBServiceMsg *pDbMsg = dynamic_cast<xGateDBServiceMsg*>(pBaseMsg);
   if(pDbMsg == NULL)
   {
      XGLOG_ERROR("process_msg_from_controller recvd invalid Message class in DB Handler");
      return false;
   }

	 xGateDBConnector* dbConnector = nullptr;
   DBRequestInfo &dbReqInfo = pDbMsg->get_db_request_info();
   std::string uId = dbReqInfo.m_contextId;
   DBServiceEvent event = dbReqInfo.m_dbEvent;
   switch(event)
   {
    case EN_DB_EVENT_STORE_VIDEO_REC_DATA_QUERY:
    {
			auto itr = XG_CONFIG::instance()->m_dbModProfile.m_mapDbConnInfo.find(EN_DB_MSG_TYPE::EN_DB_MSG_TYPE_UCAAS_CALL_RECORD);
			if(itr != XG_CONFIG::instance()->m_dbModProfile.m_mapDbConnInfo.end())
			{
				DBConnInfo* dbConnInfo = itr->second;
				if(!dbConnInfo->m_db_server_ip.empty())
				{
					auto connItr = m_dbConnMap.find(dbConnInfo->m_db_server_ip);
					if(connItr != m_dbConnMap.end())
					{
						dbConnector = connItr->second;
					}
				}
			}
			else
			{
				XGLOG_ERROR("store_video_record_data query failed. DB connection not found with server for callid(%s)",uId.c_str());
				return false;
			}
      if(!store_video_record_file_info(dbReqInfo, dbConnector)){
        XGLOG_ERROR("store_video_record_data query failed for callid(%s)",uId.c_str());
      } else {
        XGLOG_INFO("[[[store_video_record_data query success for callid(%s)]]]",uId.c_str());
      }
      break;
    }
    case EN_DB_EVENT_STORE_CCAAS_VIDEO_REC_DATA_QUERY:
    {
			auto itr = XG_CONFIG::instance()->m_dbModProfile.m_mapDbConnInfo.find(EN_DB_MSG_TYPE::EN_DB_MSG_TYPE_CCAAS_CALL_RECORD);
			if(itr != XG_CONFIG::instance()->m_dbModProfile.m_mapDbConnInfo.end())
			{
				DBConnInfo* dbConnInfo = itr->second;
				if(!dbConnInfo->m_db_server_ip.empty())
				{
					auto connItr = m_dbConnMap.find(dbConnInfo->m_db_server_ip);
					if(connItr != m_dbConnMap.end())
					{
						dbConnector = connItr->second;
					}
				}
			}
			else
			{
				XGLOG_ERROR("store_ccaas_video_record_data query failed. DB connection not found with server for callid(%s)",uId.c_str());
				return false;
			}
      if(!store_ccaas_video_record_file_info(dbReqInfo, dbConnector)){
        XGLOG_ERROR("store_ccaas_video_record_data query failed for callid(%s)",uId.c_str());
      } else {
        XGLOG_INFO("[[[store_ccaas_video_record_data query success for callid(%s)]]]",uId.c_str());
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

bool xGateDBHandler::perform_query_operation(const char *queryStr, HSTMT &hstmt, xGateDBConnector* dbConnector)
{
  std::unique_lock<std::mutex> lock(_mutex);//lock for m_txGateDBConnector to avoid concurrency 
  if(!dbConnector->execute_query(queryStr,hstmt)) {
     XGLOG_ERROR("xGateDBHandler::perform_query_operation failed to execute query(%s)",queryStr);
      dbConnector->CloseStatementHandle(hstmt);
     if(check_db_connection(dbConnector)) {
        XGLOG_INFO("xGateDBHandler::perform_query_operation DB Connection still alive ,so issue with query(%s)",queryStr);
        return false;
     } else {
        if(!retry_failed_query(queryStr,hstmt, dbConnector)){
          XGLOG_INFO("xGateDBHandler::perform_query_operation retry failed for query(%s) ,DB connection not available",queryStr);
          dbConnector->CloseStatementHandle(hstmt);
        }
     }  
  }
  return true;
}

bool xGateDBHandler::store_video_record_file_info(DBRequestInfo &dbReqInfo, xGateDBConnector* dbConnector)
{
  XGLOG_INFO("*** xGateDBHandler::store_video_record_file_info '%s' ***",dbReqInfo.m_contextId.c_str());
  std::string uid = dbReqInfo.m_contextId;
  std::string spName = dbReqInfo.m_spName;
  std::string data = dbReqInfo.m_data;
  HSTMT hstmt;
  size_t sz = sizeof(dbReqInfo.m_data);
  string QueryStr = "call " + spName + " (" + data + ");";

  if (!perform_query_operation(QueryStr.c_str(), hstmt, dbConnector)) {
    XGLOG_ERROR("store_video_record_data failed while executing query: '%s' for '%s'!", QueryStr.c_str(), uid.c_str());
    return false;
  }

  if(!get_store_video_record_query_result(hstmt, dbReqInfo.m_output, dbConnector))
  {
    XGLOG_ERROR( "store_video_record_data failed while reading result of query: '%s' !", QueryStr.c_str());
    return false;
  }

  if(!post_result(dbReqInfo)) {
    XGLOG_ERROR( "store_video_record_data failed while posting query result to controller !");
    return false;
  }

  dbConnector->CloseStatementHandle(hstmt);
  return true;
}

bool xGateDBHandler::get_store_video_record_query_result(HSTMT hstmt, string &output, xGateDBConnector* dbConnector)
{
  Document doc;
  StringBuffer strBuf;
  Writer<StringBuffer> writer(strBuf);
  Value outputVal(kObjectType);
  Document::AllocatorType& allocator = doc.GetAllocator();

  //RETCODE retCode; //TODO::code review: dead code
  SQLRETURN sqlReturn;
  SQLINTEGER iVal = 0;
  char sVal[500] = {0};
  memset(sVal, 0, sizeof(sVal));

  while(1) {
    sqlReturn = SQLFetch(hstmt);
    if(sqlReturn == SQL_ERROR || sqlReturn == SQL_SUCCESS_WITH_INFO) {
      XGLOG_INFO("got sql error while reading prequery result\n");
      dbConnector->CloseStatementHandle(hstmt);
      return false;
    }
    if(sqlReturn == SQL_SUCCESS || sqlReturn == SQL_SUCCESS_WITH_INFO) {
      SQLGetData(hstmt, EN_V_ID, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);
      outputVal.AddMember("v_id", setJsonParam(iVal,doc), allocator);
    } else {
      break;
    }
  }

  outputVal.Accept(writer);
  output = strBuf.GetString();
  XGLOG_INFO("get_store_video_record_query_result output: %s", output.c_str());
  return true;
}

bool xGateDBHandler::store_ccaas_video_record_file_info(DBRequestInfo &dbReqInfo, xGateDBConnector* dbConnector)
{
  XGLOG_INFO("*** xGateDBHandler::store_ccaas_video_record_file_info '%s' ***",dbReqInfo.m_contextId.c_str());
  std::string uid = dbReqInfo.m_contextId;
  std::string spName = dbReqInfo.m_spName;
  std::string data = dbReqInfo.m_data;
  HSTMT hstmt;
  size_t sz = sizeof(dbReqInfo.m_data);
  string QueryStr = "call " + spName + " (" + data + ");";

  if (!perform_query_operation(QueryStr.c_str(), hstmt, dbConnector)) {
    XGLOG_ERROR("store_ccaas_video_record_data failed while executing query: '%s' for '%s'!", QueryStr.c_str(), uid.c_str());
    return false;
  }

  if(!get_ccaas_video_record_query_result(hstmt, dbReqInfo.m_output, dbConnector))
  {
    XGLOG_ERROR( "store_ccaas_video_record_data failed while reading result of query: '%s' !", QueryStr.c_str());
    return false;
  }

  if(!post_result(dbReqInfo)) {
    XGLOG_ERROR( "store_ccaas_video_record_data failed while posting query result to controller !");
    return false;
  }

  dbConnector->CloseStatementHandle(hstmt);
  return true;
}

bool xGateDBHandler::get_ccaas_video_record_query_result(HSTMT hstmt, string &output, xGateDBConnector* dbConnector)
{
  Document doc;
  StringBuffer strBuf;
  Writer<StringBuffer> writer(strBuf);
  Value outputVal(kObjectType);
  Document::AllocatorType& allocator = doc.GetAllocator();

  //RETCODE retCode; //TODO::code review: dead code
  SQLRETURN sqlReturn;
  SQLINTEGER iVal = 0;
  char sVal[500] = {0};
  memset(sVal, 0, sizeof(sVal));

  while(1) {
    sqlReturn = SQLFetch(hstmt);
    if(sqlReturn == SQL_ERROR || sqlReturn == SQL_SUCCESS_WITH_INFO) {
      XGLOG_INFO("got sql error while reading prequery result\n");
      dbConnector->CloseStatementHandle(hstmt);
      return false;
    }
    if(sqlReturn == SQL_SUCCESS || sqlReturn == SQL_SUCCESS_WITH_INFO) {
      SQLGetData(hstmt, EN_ERR_CODE, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);
      outputVal.AddMember("err_code", setJsonParam(iVal,doc), allocator);
      SQLGetData(hstmt, EN_ERR_MSG, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);
      outputVal.AddMember("err_msg", setJsonParam(sVal,doc), allocator);
    } else {
      break;
    }
  }

  outputVal.Accept(writer);
  output = strBuf.GetString();
  XGLOG_INFO("get_ccaas_video_record_query_result output: %s", output.c_str());
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
	ptdbMsg->setSrcModuleId(IURDefines::UR_MODULE_DB_SERVICE);
	ptdbMsg->setDstModuleId(dbReqInfo.m_requestorModule);
	ptdbMsg->setUid(dbReqInfo.m_contextId);
	XGLOG_INFO("$$$$$$$$$$$ xGateDBHandler::post_result getSrcModuleId():%d", ptdbMsg->getSrcModuleId());
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


bool xGateDBHandler::check_db_connection(xGateDBConnector* dbConnector)
{
  HSTMT hstmt;
  char queryStr[12] = {0};
  memset(queryStr, 0, sizeof(queryStr));
  snprintf(queryStr, 15, "SELECT 1");
  if(!dbConnector->execute_query(queryStr, hstmt)) {
    //XGLOG_ERROR( "check_db_connection failed while executing query: '%s' !", (char*)ur_log_string(queryStr));
    dbConnector->CloseStatementHandle(hstmt);
    return false;
   }
   dbConnector->CloseStatementHandle(hstmt);
   XGLOG_ERROR( "check_db_connection success ,connection still alive");
  return true;
}

bool xGateDBHandler::retry_failed_query(const char *queryStr, HSTMT &hstmt, xGateDBConnector* dbConnector)
{
  XGLOG_INFO("xGateDBHandler::retry_failed_query started to reconnect the DB");
  if (reconnect_db(dbConnector)) {
    if (!dbConnector->execute_query(queryStr, hstmt)) {
      //XGLOG_ERROR("retry_failed_query query execution failed ,after DB reconnection: '%s' !", (char *)ur_log_string(queryStr));
      // m_txGateDBConnector->CloseStatementHandle(hstmt);
      return false;
    }
    return true;
  }
  return false;
}
