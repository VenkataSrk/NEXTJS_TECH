#pragma once

//ace includes
#include <ace/Task.h>
#include <ace/Singleton.h>
//local inludes
#include "xGateUtil.h"
#include "xglog.h"
#include "xGateBaseMsg.h"
#include "xGateDBConnector.h"
#include "xGateDBServiceDefine.h"
#include "XGConfig.h"
#include "IURModule.h"

//c++ includes
#include <vector>
#include <string>
#include <sstream>
#include <mutex>

#define DBHNDLR XGDBHNDLR::instance()

typedef std::map<std::string, xGateDBConnector*> DB_CONN_MAP;
typedef DB_CONN_MAP::iterator IT_DB_CONN_MAP;

class xGateDBHandler
{
  public:
    xGateDBHandler();
    virtual ~xGateDBHandler();

    //member functions
    bool init();
    bool reconnect_db(xGateDBConnector *& dbConnector);
   bool process_msg(xGateBaseMsg *pMsg);
   bool process_msg_from_sfu_controller(xGateBaseMsg *pBaseMsg);
    bool parseJsonMsg(const char *jsonData, Document &doc);
    bool getInputDoc(Document &doc, Value &inputVal);  
    Value setJsonParam(string &strValue, Document &doc);
    Value setJsonParam(int iValue, Document &doc);
    Value setJsonParam(char* chrValue, Document &doc);
    bool post_result(DBRequestInfo &dbOutReqInfo);
   
    bool perform_query_operation(const char *queryStr, HSTMT &hstmt, xGateDBConnector* dbConnector);
    bool perform_ccaas_query_operation(const char *queryStr, HSTMT &hstmt, xGateDBConnector* dbConnector);
    bool store_video_record_file_info(DBRequestInfo &dbReqInfo, xGateDBConnector* dbConnector);
    bool get_store_video_record_query_result(HSTMT hstmt, string &output, xGateDBConnector* dbConnector);
    bool store_ccaas_video_record_file_info(DBRequestInfo &dbReqInfo, xGateDBConnector* dbConnector);
    bool get_ccaas_video_record_query_result(HSTMT hstmt, string &output, xGateDBConnector* dbConnector);
    bool check_db_connection(xGateDBConnector* dbConnector);
    bool retry_failed_query(const char *queryStr, HSTMT &hstmt, xGateDBConnector* dbConnector);
  private:
    xGateDBConnector *m_txGateDBConnector; 
    DBConnectionInfo m_tConnInfo;
    xGateDBConnector *m_tCcaasDBConnector; 
    DBConnectionInfo m_tCcaasConnInfo;
		DB_CONN_MAP m_dbConnMap;
    std::mutex _mutex;
    std::mutex _ccaas_db_mutex;
};

typedef ACE_Singleton<xGateDBHandler, ACE_Recursive_Thread_Mutex> XGDBHNDLR;


