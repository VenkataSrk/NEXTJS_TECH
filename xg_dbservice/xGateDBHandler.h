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
    bool process_msg(xGateBaseMsg *pMsg);
    bool process_msg_from_controller(xGateBaseMsg *pBaseMsg);
    bool process_msg_from_monitor_service(xGateBaseMsg *pBaseMsg);
    bool post_result(DBRequestInfo &dbOutReqInfo);
   
    bool perform_query_operation(const char *queryStr, HSTMT &hstmt);
    bool do_trunk_list_request(DBRequestInfo &dbReqInfo);
    bool get_trunk_list_result(HSTMT hstmt,std::string &data);
    bool update_trunk_status(DBRequestInfo &dbReqInfo);
    bool check_db_connection();
    bool retry_failed_query(const char *queryStr, HSTMT &hstmt);
    bool do_ms_teams_domain_list_request(DBRequestInfo &dbReqInfo);
    bool get_ms_teams_list_result(HSTMT hstmt,std::string &data);
  private:
    Value setJsonParam(string &strValue, Document &doc);
    Value setJsonParam(int iValue, Document &doc);
    Value setJsonParam(char* chrValue, Document &doc);
    bool parseJsonMsg(const char *jsonData, Document &doc);

    xGateDBConnector *m_txGateDBConnector; 
    DBConnectionInfo m_tConnInfo;
    std::mutex _mutex;
};

typedef ACE_Singleton<xGateDBHandler, ACE_Recursive_Thread_Mutex> XGDBHNDLR;


