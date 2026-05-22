#ifndef _UR_DB_CONNECTOR_H_
#define _UR_DB_CONNECTOR_H_

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

enum class DBConnectionType
{
  EN_DB_CONNECTION_TYPE_MYSQL = 1,
  EN_DB_CONNECTION_TYPE_POSTGRESQL,
};
class xGateDBConnector
{
  public:
    RETCODE ret; // ODBC return code
    HENV henv;   // Environment
    HDBC hdbc;   // Connection handle

  public:
    xGateDBConnector();

    xGateDBConnector(std::string &dbSrvrIpAddr, unsigned int &dbSrvrPort, std::string &userName, std::string &password, std::string &database, DBConnectionType dbConnType);

    ~xGateDBConnector();
 
    void CloseStatementHandle( HSTMT hstmt);
    bool execute_query(const char *queryStr, HSTMT &hstmt);
};
#endif
