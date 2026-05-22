#pragma once

//ace includes
#include <ace/Task.h>
#include <ace/Singleton.h>
//local inludes
#include "xGateUtil.h"
#include "xglog.h"
#include "xGBaseMsg.h"
#include "xGateDBConnector.h"
#include "xGateDBServiceDefine.h"
#include "IURModule.h"

//c++ includes
#include <vector>
#include <string>
#include <sstream>
#include <mutex>

#define DBHNDLR XGDBHNDLR::instance()
class xGateDBHandler
{
  public:
    xGateDBHandler();
    virtual ~xGateDBHandler();

    //member functions
    bool init();
    bool reconnect_db();
    bool process_msg(xGBaseMsg *pMsg);
    bool process_msg_from_controller(xGBaseMsg *pBaseMsg);
    bool parseJsonMsg(const char *jsonData, Document &doc);
    bool getInputDoc(Document &doc, Value &inputVal);  
    Value setJsonParam(string &strValue, Document &doc);
    Value setJsonParam(int iValue, Document &doc);
    Value setJsonParam(char* chrValue, Document &doc);
    bool post_result(DBRequestInfo &dbOutReqInfo);
   
    bool perform_query_operation(const char *queryStr, HSTMT &hstmt);
    bool do_call_forward_request(Document &doc, Value &inputVal, DBRequestInfo &dbReqInfo);
    bool get_call_forward_result(HSTMT hstmt, string &output);
    bool store_call_record_data(DBRequestInfo &dbReqInfo);
    bool check_db_connection();
    bool retry_failed_query(const char *queryStr, HSTMT &hstmt);

    bool perform_ccaas_query_operation(const char *queryStr, HSTMT &hstmt);
    bool check_ccaas_db_connection();
    bool retry_ccaas_failed_query(const char *queryStr, HSTMT &hstmt);
    bool reconnect_ccaas_db();

    bool perform_crm_query_operation(const char *queryStr, HSTMT &hstmt);
    bool check_crm_db_connection();
    bool retry_crm_failed_query(const char *queryStr, HSTMT &hstmt);
    bool reconnect_crm_db();

  private:

    DBConnectionInfo m_tConnInfo;
    DBConnectionInfo m_tCcaasDbConnInfo;
    DBConnectionInfo m_tCrmDbConnInfo;

    xGateDBConnector *m_txGateDBConnector;
    xGateDBConnector *m_txGateCcaasDBConnector;
    xGateDBConnector *m_txGateCrmDBConnector;
    std::mutex _mutex;
};

typedef ACE_Singleton<xGateDBHandler, ACE_Recursive_Thread_Mutex> XGDBHNDLR;


