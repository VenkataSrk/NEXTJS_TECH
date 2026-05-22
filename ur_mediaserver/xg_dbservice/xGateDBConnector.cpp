#include "xGateDBConnector.h"
#include <vector>
#include "xglog.h"

using namespace std;

xGateDBConnector::xGateDBConnector()
{
  XGLOG_INFO("Inside DEFAULT constructor... xGateDBConnector");
	m_dbConnInfo = new DBConnInfo();
}

xGateDBConnector :: ~xGateDBConnector()
{
  std::cout<< "Inside DB Disconnection"<<endl;
  XGLOG_INFO("xGateDBConnector:: DESTRUCTOR  ~xGateDBConnector()");
  SQLDisconnect(hdbc);
  SQLFreeHandle( SQL_HANDLE_DBC, hdbc);
  SQLFreeHandle( SQL_HANDLE_ENV, henv);
}

//xGateDBConnector::xGateDBConnector(std::string &dbSrvrIpAddr, std::string &dbSrvrPort, std::string &username, std::string &password, std::string &database):m_dbSrvrIpAddr(dbSrvrIpAddr)

xGateDBConnector::xGateDBConnector(DBConnInfo* dbConnInfo):m_dbConnInfo(dbConnInfo)
{
  XGLOG_INFO("xGateDBConnector::xGateDBConnector(%s : %s : %s : %s)", dbConnInfo->m_db_server_ip.c_str(), dbConnInfo->m_db_user_name.c_str(), dbConnInfo->m_db_password.c_str(), dbConnInfo->m_db_name.c_str());

  ret = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

  ret = SQLSetEnvAttr( henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);

  ret = SQLAllocHandle( SQL_HANDLE_DBC, henv, &hdbc);

  SQLCHAR connectstr[1024+1],connectstring_out[1024+1];
  memset(connectstr, 0, sizeof(connectstr));
  memset(connectstring_out, 0, sizeof(connectstring_out));
  SQLSMALLINT len;
  //MSSQL connection string
  #if 0
    //sprintf((char*)connectstr,"DRIVER=FreeTDS;SERVER=%s;UID=%s;PWD=%s;DATABASE=%s;TDS_version=7.0;port=1433;", dbSrvrIpAddr, username, password, database);
    sprintf((char*)connectstr,"DRIVER=FreeTDS;SERVER=%s;UID=%s;PWD=%s;DATABASE=%s;TDS_version=7.0;port=%s;", dbSrvrIpAddr.c_str(), username.c_str(), password.c_str(), database.c_str(), dbSrvrPort.c_str());
  #else
  //MYSQL connection string
  sprintf((char*)connectstr,"DRIVER={MySQL ODBC 8.0 ANSI Driver};SERVER=%s;UID=%s;PWD=%s;DATABASE=%s;TDS_version=7.0;port=%s;", dbConnInfo->m_db_server_ip.c_str(), dbConnInfo->m_db_user_name.c_str(), dbConnInfo->m_db_password.c_str(), dbConnInfo->m_db_name.c_str(),dbConnInfo->m_db_serv_port.c_str());
  #endif

  XGLOG_INFO("xGateDBConnector::xGateDBConnector(): connectstr=%s", connectstr);

  ret = ::SQLDriverConnect( hdbc,
      NULL,
      connectstr,
      SQL_NTS,
      connectstring_out,
      sizeof(connectstring_out),
      &len,
      SQL_DRIVER_NOPROMPT);

  if (ret != SQL_SUCCESS)
  {
    XGLOG_INFO("xGateDBConnector::xGateDBConnector(): Failed to connect to DB. Error from SQLDriverConnect() ");
    SQLDisconnect(hdbc);
    SQLFreeHandle( SQL_HANDLE_DBC, hdbc);
    SQLFreeHandle( SQL_HANDLE_ENV, henv);
  }
  else
  {
    XGLOG_INFO("xGateDBConnector::xGateDBConnector(): DB Connection establishment Successfully ");
  }
}

bool xGateDBConnector::execute_query(const char *queryStr, HSTMT &hstmt)
{
  XGLOG_INFO("xGateDBConnector::execute_query (QueryStr --> '%s' )", queryStr);
  RETCODE ret; // ODBC return code
  UCHAR errmsg[512] = "";
  memset(errmsg, 0, sizeof(errmsg));
  
  ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if (ret != SQL_SUCCESS)
  {
    SQLError(henv, hdbc, SQL_NULL_HSTMT, NULL, NULL, errmsg, sizeof(errmsg), NULL);
    XGLOG_ERROR("SQLAllocHandle failed with sql errmsg: %s\n", errmsg);
    return false;
  }

  ret = SQLExecDirect(hstmt, (SQLCHAR*)queryStr, SQL_NTS);
  if (ret != SQL_SUCCESS)
  {
    SQLError(henv, hdbc, hstmt, NULL, NULL, errmsg, sizeof(errmsg), NULL);
    XGLOG_ERROR("SQLExecDirect failed with sql errmsg: %s\n", errmsg);
    return false;
  }else{
    XGLOG_INFO("Posted Successfully Query info: '%s'", queryStr);
  }
  return true;
}

void xGateDBConnector::CloseStatementHandle(HSTMT hstmt)
{
  SQLFreeStmt(hstmt,SQL_CLOSE);
}

