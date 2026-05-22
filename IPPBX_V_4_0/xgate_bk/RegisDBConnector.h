#ifndef __REGDBCONNECT_H__
#define __REGDBCONNECT_H__

#include <sqlext.h>
#include <iostream>
#include <string.h>
#include <sqltypes.h>
#include <sql.h>
#include <fstream>
#include <pthread.h>
#include <cstdio>
#include <vector>
#include "xGateUtil.h"

#define MYSQLSUCCESS(rc) ((rc == SQL_SUCCESS) || (rc == SQL_SUCCESS_WITH_INFO) )

#define MAX_TOTAL_NO_OF_DB_CONNECTIONS  3

class DB_Connection
{
  public:
    RETCODE ret; // ODBC return code
    HENV henv; // Environment
    HDBC hdbc; // Connection handle
    static unsigned int g_totalNoOfDBConnections;
    static HDBC hdbc_array[MAX_TOTAL_NO_OF_DB_CONNECTIONS];
    static std::vector<HDBC> hdbc_array_2;

  public:
    DB_Connection();
    DB_Connection(char* db_SERVER_ipaddress, char* UID, char* password, char* DATABASE, int DBConnectionSlot);
    ~DB_Connection();
    ////protected:
    void  CloseStatementHandle( HSTMT hstmt); // 

    std::string getFirstColumn_StringValue(char *);
    std::string get_Nth_Column_StringValue(int columnNumber,char *);
    std::string get_Nth_Column_StringValue_Recordwise(int columnNumber,char *, int DbRecordNumber);
    int get_Nth_Column_IntegerValue(int columnNumber,char *);
    int get_Nth_Column_IntegerValue_Recordwise(int columnNumber,char *, int DbRecordNumber);

#ifdef XGATE_HAS_BUSINESS_SUPPORT
    bool execute_query(const char *queryStr, HSTMT &hstmt);
#endif

    std::vector<DBQueryResultData>* db_combined_sp(char *);
    std::vector<DBQueryResultData>* db_combined_sp_TRUNKOUT_PORTOUT(char *);
    std::vector<DBQueryResultData>* db_combined_sp_SendAppNotificationsAll(char *);
    std::vector<DBQueryResultData>* db_callhandling_sp(char *);
    std::vector<DBQueryResultData>* db_combined_sp_swb(char *);
    std::vector<DBQueryResultData>* db_combined_sp_callForward(char *);
    std::vector<DBQueryResultData>* db_combined_sp_conf_info(char *);
    std::vector<DBQueryResultData>* db_combined_sp_conf_id(char *);
    std::vector<DBQueryResultData>* db_combined_sp_conf_validate_pin(char *);
    std::vector<DBQueryResultData>* db_combined_sp_conf_get_callid_mguid(char *);
    std::vector<DBQueryResultData>* db_combined_sp_direct_number(char *);
    std::vector<DBQueryResultData>* db_combined_sp_get_pstn_operator_details(char *);
    int get_totalNoOfRegisteredUsers(char *);
};

class Registrar : public DB_Connection
{
  public:
    int CheckAuthReq(char *);    //check Authentication
    std::string getuserdetails(char *);
    int DoUserRegis(char *);      //register user
    int DoDeleteRegis(char *);    // delete registration
    int DoRegisStatus(char *);    // registration status
};
#endif
