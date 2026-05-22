#include "BSLocalDBHandler.h"

/* Method Name: connectMySqlDB
 ** Description: To connect to mysql database
 ** Parameters: const char* connStr, const char* user, const char* pass
 ** return: boolean(true|false)
 **/
bool BSLocalDBHandler::connectMySqlDB(const char* connStr, const char* user, const char* pass)
{
  try 
  {
    m_driver = get_driver_instance();
    m_conn = m_driver->connect(connStr, user, pass);
    return true;
  } 
  catch (sql::SQLException &e) 
  {
    return false;
  }
}

/* Method Name: clearConnection
 ** Description: To clear database conection handle
 ** Parameters: nil
 ** return: boolean(true|false)
 **/
bool BSLocalDBHandler::clearConnection()
{
  if(m_conn!=0)
  {
    delete m_conn;
  }
  return true;
}

STR BSLocalDBHandler::getDate()
{
  STR ret("");
  time_t rawtime;
  struct tm *info;
  char buffer[20];
  time(&rawtime);
  info = localtime(&rawtime);
  strftime(buffer,20,"%Y%m%d", info);
  ret = buffer;
  return ret;
}

/* Method Name: createReportSchema
 ** Description: To create default schema
 ** Parameters: CADBMsg* dbObj
 ** return: boolean(true|false)
 **/
bool BSLocalDBHandler::createReportSchema()
{
  try
  {
    STR query("");
    m_stmt = m_conn->createStatement();
    query = "CREATE DATABASE IF NOT EXISTS BS_report";
    m_stmt->execute(query.c_str());
    delete m_stmt;
    return true;
  }
  catch(sql::SQLException &e)
  {
    return false;
  }
}

/* Method Name: insertCallInfo
 ** Description: To insert call info into table
 ** Parameters: CADBMsg* dbObj
 ** return: boolean(true|false)
 **/
bool BSLocalDBHandler::insertCallInfo(BSDBMsg* dbObj)
{
  if (!dbObj)
  {
    return false;
  }

  try
  {
    STR tableName(""), query("");
    std::ostringstream createQuery, insertQuery;
    tableName.assign("CI");
    tableName.append(getDate());
    m_conn->setSchema("test");
    m_stmt = m_conn->createStatement();
    createQuery << "CREATE TABLE IF NOT EXISTS " << tableName << "(CallId VARCHAR(150), Uid VARCHAR(150),\
                    AddrFamily INT, MgIpAddr VARCHAR(20), MgPort INT, Codec INT, CallStream INT, \
                    CallState INT, CnCalled VARCHAR(20), CnCalling VARCHAR(20), CsaCalled VARCHAR(20),\
                    CsaCalling VARCHAR(20), Cause INT, CallStartTime INT, CallConnectedTime INT,\
                    CallDisconnectedTime INT, CallDuration INT, MediaState INT, P2PMedia INT)";
    query = createQuery.str();
    m_stmt->execute(query.c_str());
    query.clear();
    insertQuery << "INSERT INTO " << tableName << " VALUES (" << "\"" << dbObj->getCallId().c_str() << \
          "\"" << ", " << "\"" << dbObj->getUid().c_str() << "\"" << ", " << dbObj->getAddrFamily() << "," << \
          "\"" << dbObj->getMgIpAddr().c_str() << "\"" << ", " << dbObj->getMgPort() << ", " << \
          dbObj->getCodec() << ", " << dbObj->getCallStream() << ", " << dbObj->getCallState() << ", " << \
          "\"" << dbObj->getCnCalled().c_str() << "\"" << ", " << "\"" << dbObj->getCnCalling().c_str() << \
          "\"" << ", " << "\"" << dbObj->getCsaCalled().c_str() << "\"" << ", " << "\"" << dbObj->getCsaCalling().c_str() << \
          "\"" << ", " << dbObj->getCause() << ", " << dbObj->getCallStartTime() << ", " << dbObj->getCallConnectedTime() << \
          ", " << dbObj->getCallDisconnectedTime() << ", " << dbObj->getCallDuration() << ", " << \
          dbObj->getMediaState() << ", " << dbObj->getP2PMedia() << ")";
    query = insertQuery.str();
    m_stmt->execute(query.c_str());
    delete m_stmt;
    return true;
  }
  catch(sql::SQLException &e) 
  {
    return false;
  }
}

/* Method Name: getCallInfo
 ** Description: To get row from call info table
 ** Parameters: CADBMsg* dbObj
 ** return: boolean(true|false)
 **/
bool BSLocalDBHandler::getCallInfo(BSDBMsg* dbObj)
{
  if (!dbObj)
  {
    return false;
  }

  try
  {
    STR tableName(""), query("");
    std::ostringstream getQuery;
    tableName.assign("CI");
    tableName.append(getDate());
    m_conn->setSchema("test");
    m_stmt = m_conn->createStatement();
    getQuery << "SELECT * FROM " << tableName << " where CallId = " << "'" << dbObj->getCallId() << "'";
    query = getQuery.str();
    m_res = m_stmt->executeQuery(query.c_str());
    while (m_res->next())
    {
      //cout << "UID: " << m_res->getString("Uid") << endl;
    }
    delete m_res;
    delete m_stmt;
    return true;
  }
  catch(sql::SQLException &e) 
  {
    return false;
  }
}

/* Method Name: updateCallInfo
 ** Description: To update call info row based on call id
 ** Parameters: CADBMsg* dbObj
 ** return: boolean(true|false)
 **/
bool BSLocalDBHandler::updateCallInfo(BSDBMsg* dbObj)
{
  if (!dbObj)
  {
    return false;
  }

  try
  {
    STR tableName(""), query("");
    std::ostringstream updateQuery;
    tableName.assign("CI");
    tableName.append(getDate());
    m_conn->setSchema("test");
    m_stmt = m_conn->createStatement();
    updateQuery << "UPDATE SET CallState = " << dbObj->getCallState() << " FROM " << tableName << " where CallId = " << "'" << dbObj->getCallId() << "'";
    query = updateQuery.str();
    m_stmt->executeQuery(query.c_str());
    delete m_stmt;
    return true;
  }
  catch(sql::SQLException &e) 
  {
    return false;
  }
}

/* Method Name: deleteCallInfo
 ** Description: To delete call info row based on call id
 ** Parameters: CADBMsg* dbObj
 ** return: boolean(true|false)
 **/
bool BSLocalDBHandler::deleteCallInfo(BSDBMsg* dbObj)
{
  if (!dbObj)
  {
    return false;
  }

  try
  {
    STR tableName(""), query("");
    std::ostringstream deleteQuery;
    tableName.assign("CI");
    tableName.append(getDate());
    m_conn->setSchema("test");
    m_stmt = m_conn->createStatement();
    deleteQuery << "DELETE FROM " << tableName << " where CallId = " << "'" << dbObj->getCallId() << "'";
    query = deleteQuery.str();
    m_stmt->executeQuery(query.c_str());
    delete m_stmt;
    return true;
  }
  catch(sql::SQLException &e) 
  {
    return false;
  }
}


/* Method Name: dropTable
 ** Description: To drop call info table 
 ** Parameters: STR tableName
 ** return: boolean(true|false)
 **/
//bool CADBHandler::dropTable(STR tableName)
bool BSLocalDBHandler::dropTable(BSDBMsg* dbObj)
{
  if (!dbObj)
  {
    return false;
  }

  try
  {
    STR query("");
    std::ostringstream dropQuery;
    m_conn->setSchema("test");
    m_stmt = m_conn->createStatement();
    dropQuery << "DROP TABLE " << dbObj->gettableName();
    query = dropQuery.str();
    m_stmt->executeQuery(query.c_str());
    delete m_stmt;
    return true;
  }
  catch(sql::SQLException &e) 
  {
    return false;
  }
}

STR BSDBReportMsg::TmStrFormat(const char *format)
{
  tm * timeptr;
  time_t timeval = time(NULL);
  char time_string[50];
  timeptr = localtime(&timeval);
  memset(time_string, 0, sizeof(time_string));
  strftime(time_string, sizeof(time_string), format, timeptr);
  return time_string;
}

/* Method Name: dropTable
 ** Description: To drop call info table 
 ** Parameters: STR tableName
 ** return: boolean(true|false)
 **/
bool BSLocalDBHandler::insertCDRInfo(BSDBReportMsg* dbObj)
{
  if (!dbObj)
  {
    return false;
  }
  try
  {
    STR tableName(""), query("");
    std::ostringstream createQuery, insertQuery;
    tableName.assign("CDR");
    tableName.append(getDate());
    m_conn->setSchema("BS_report");
    m_stmt = m_conn->createStatement();
    createQuery << "CREATE TABLE IF NOT EXISTS " << tableName << "(DateTime VARCHAR(150), Cli VARCHAR(150), CustCode VARCHAR(150), SiteCode VARCHAR(150), SwitchCode VARCHAR(20), Ddi VARCHAR(150), PrefixDest VARCHAR(150), AccLangset VARCHAR(150), CardId VARCHAR(150), Balance INT, CallMaxTalkTime INT, Batchcode INT, Serialcode INT, Ispackcnxcharge INT, UselimitDelay INT, PricePerUnit INT, DateTime1 VARCHAR(150), CheckDest INT, BreakageChargeMode INT, Year VARCHAR(150), Month VARCHAR(150), Date VARCHAR(150), Hour VARCHAR(150), Minute VARCHAR(150))";
    
    query = createQuery.str();
    m_stmt->execute(query.c_str());
    query.clear();
   
    insertQuery << "INSERT INTO " << tableName << " VALUES (" << "\"" \
    << dbObj->TmStrFormat("%Y-%m-%d %H:%M:%S") << "\"" << "," << "\"" \
    << dbObj->getCli() << "\"" << "," << "\"" << dbObj->getCustCode() << "\"" << "," << "\"" \
    << dbObj->getSiteCode() << "\"" << "," << "\"" << dbObj->getSwitchCode() << "\"" << "," << "\"" \
    << dbObj->getDdi() << "\"" << "," << "\"" << dbObj->getPrefixDest() << "\"" << "," << "\"" \
    << dbObj->getAccLangset() << "\"" << "," << "\"" << dbObj->getCardId() << "\"" << "," \
    << dbObj->getBalance() << "," << dbObj->getBatchcode() << "," \
    << dbObj->getSerialcode() << "," << dbObj->getIspackcnxcharge() << "," \
    << dbObj->getUselimitDelay() << "," << dbObj->getPricePerUnit() << "," << "\"" \
    << dbObj->TmStrFormat("%Y-%m-%d %H:%M:%S") << "\"" << "," << dbObj->getCheckDest() << "," \
    << dbObj->getBreakageChargeMode() << "," << "\"" << dbObj->TmStrFormat("%Y") << "\"" << "," << "\"" \
    << dbObj->TmStrFormat("%m") << "\"" << "," << "\"" << dbObj->TmStrFormat("%d") << "\"" << "," << "\"" \
    << dbObj->TmStrFormat("%H") << "\"" << "," << "\"" << dbObj->TmStrFormat("%M") << "\"" << ")";
    query = insertQuery.str();
    m_stmt->execute(query.c_str());
    delete m_stmt;
    return true;
  }
  catch(sql::SQLException &e) 
  {
    return false;
  }
}


bool BSLocalDBHandler::insertCRRInfo(BSDBReportMsg* dbObj)
{
  if (!dbObj)
  {
    return false;
  }
  try
  {
    STR tableName(""), query("");
    std::ostringstream createQuery, insertQuery;
    tableName.assign("CRR");
    tableName.append(getDate());
    m_conn->setSchema("BS_report");
    m_stmt = m_conn->createStatement();
    createQuery << "CREATE TABLE IF NOT EXISTS " << tableName << "(SiteCode VARCHAR(150), SwitchCode VARCHAR(20),Time VARCHAR(150),SessionId VARCHAR(20),\
                    Cli VARCHAR(150), Ddi VARCHAR(150), PrefixDest VARCHAR(150), \
                    Choice INT, CallDomain VARCHAR(150), Time VARCHAR(150), CallCause INT, ConnectionState INT,\
                    DisconnectReqBy INT, CrrCause VARCHAR(150), CostPrice VARCHAR(150), HoldTime INT)";
    
    query = createQuery.str();
    m_stmt->execute(query.c_str());
    query.clear();
    insertQuery << "INSERT INTO " << tableName << " VALUES (" <<  "\"" << dbObj->getSiteCode() << "\"" << "," << "\"" << dbObj->getSwitchCode() << "\"" << ","  \
      << "\"" << dbObj->TmStrFormat( "%Y-%m-%d %H:%M:%S") << "\"" << "," << "\"" << dbObj->getSessionId() << "\"" << "," \
      << "\"" << dbObj->getCli() << "\"" <<"," << "\"" << dbObj->getDdi() << "\"" << "," \
      << "," << "\"" << dbObj->getPrefixDest() << "\"" << "," << "1" << "," << "\"" << dbObj->getChoice() << "\"" << "," \
      << "\"" << dbObj->getCallDomain() << "\"" << ","  \
      << "\"" <<  dbObj->TmStrFormat("%Y-%m-%d %H:%M:%S") << "\"" <<"," << "0" <<"," \
      << "0" << "," << "\"" << dbObj->getCallCause() << "\"" <<"," << "\"" << dbObj->getConnectionState() << "\"" <<"," << "\"" << dbObj->getDisconnectReqBy() << "\"" << "," \
      << "\"" << dbObj->getCrrCause() << "\"" << "," << "\"" << dbObj->getCostPrice() << "\"" << "," \
      << "3" << "," << "\"" << dbObj->getHoldTime() << "\"" << ")";
    query = insertQuery.str();
    m_stmt->execute(query.c_str());
    delete m_stmt;
    return true;
  }
  catch(sql::SQLException &e) 
  {
    return false;
  }
}

bool BSLocalDBHandler::insertCDSInfo(BSDBReportMsg* dbObj)
{
  if (!dbObj)
  {
    return false;
  }
  try
  {
    STR tableName(""), query("");
    std::ostringstream createQuery, insertQuery;
    tableName.assign("CDS");
    tableName.append(getDate());
    m_conn->setSchema("BS_report");
    m_stmt = m_conn->createStatement();
    createQuery << "CREATE TABLE IF NOT EXISTS " << tableName << "(SiteCode VARCHAR(150), SwitchCode VARCHAR(20), Time VARCHAR(150), SessionId VARCHAR(20),\
                    Cli VARCHAR(150), Ddi VARCHAR(150), HoldTime INT,Time VARCHAR(150),Time VARCHAR(150),Time VARCHAR(150),\
                    Time VARCHAR(150), PrefixDest VARCHAR(150), UserInfo VARCHAR(150),ConnectionState INT, \
                    DisconnectReqBy INT, CostPrice VARCHAR(150), TimeCls VARCHAR(150), CrOpr VARCHAR(150),\
                    RouteCls VARCHAR(150), CurrCode VARCHAR(150))";
    
    query = createQuery.str();
    m_stmt->execute(query.c_str());
    query.clear();
    insertQuery << "INSERT INTO " << tableName << " VALUES (" << "\"" <<  dbObj->getSiteCode() << "\"" <<  "," << "\"" <<  dbObj->getSwitchCode() << "\"" << ","  \
      << "\"" <<  dbObj->TmStrFormat( "%Y-%m-%d %H:%M:%S") << "\"" <<  "," << "\"" <<  dbObj->getSessionId() << "\"" <<  "," \
      << "\"" <<  dbObj->getCli() << "\"" <<  "," << "\"" <<  dbObj->getDdi() << "\"" << "," 
      << "\"" <<  dbObj->getHoldTime() << "\"" <<  "," << "0" << "," \
      << "\"" <<  dbObj->TmStrFormat("%Y") << "\"" <<  "," << "\"" <<  dbObj->TmStrFormat("%m") << "\"" <<  "," \
      << "\"" <<  dbObj->TmStrFormat("%d") << "\"" <<  "," << "\"" <<  dbObj->TmStrFormat("%H") << "\"" <<  "," \
      << "\"" <<  dbObj->TmStrFormat("%M") << "\"" <<  "," << "\"" <<  dbObj->getPrefixDest() << "\"" <<  "," << "\"" <<  dbObj->getUserInfo() << "\"" <<  "," \
      << "\"" <<  dbObj->getConnectionState() << "\"" <<  "," << "\"" <<  dbObj->getDisconnectReqBy() << "\"" <<  "," << "\"" <<  dbObj->getCostPrice() << "\"" << "," \
      << "\"" <<  dbObj->getTimeCls() << "\"" <<  ","  << "1" << "," << "\"" <<  dbObj->getCrOpr() << "\"" <<  ","  << "1" << "," << "\"" <<  dbObj->getRouteCls() << "\"" <<  "," \
      << "\"" <<  dbObj->getCurrCode() << "\"" <<  "," << "0,0,0" << ")";
    query = insertQuery.str();
    m_stmt->execute(query.c_str());
    delete m_stmt;
    return true;
  }
  catch(sql::SQLException &e) 
  {
    return false;
  }
}

bool BSLocalDBHandler::insertDATInfo(BSDBReportMsg* dbObj)
{
  if (!dbObj)
  {
    return false;
  }
  try
  {
    STR tableName(""), query("");
    std::ostringstream createQuery, insertQuery;
    tableName.assign("DAT");
    tableName.append(getDate());
    m_conn->setSchema("BS_report");
    m_stmt = m_conn->createStatement();
    createQuery << "CREATE TABLE IF NOT EXISTS " << tableName << "(Time VARCHAR(150), SessionId VARCHAR(20), SiteCode VARCHAR(150), SwitchCode VARCHAR(20),\
                    Cli VARCHAR(150),Ddi VARCHAR(150), Msisdn VARCHAR(150),\
                    IMSI VARCHAR(150), Msrn VARCHAR(150), \
                    AppDur INT, DisconnectReqBy INT, CallCause INT, FwdType VARCHAR(150),\
                    FwDdi VARCHAR(150), ConnectionState INT, Vlr VARCHAR(150), Msc VARCHAR(150))";
    
    query = createQuery.str();
    m_stmt->execute(query.c_str());
    query.clear();
    insertQuery << "INSERT INTO " << tableName << " VALUES (" << "\"" <<  dbObj->TmStrFormat("%Y-%m-%d %H:%M:%S") << "\"" <<  "," \
      << "\"" <<  dbObj->getSessionId() << "\"" <<  "," \
      << "\"" <<  dbObj->getSiteCode() << "\"" <<  "," \
      << "\"" <<  dbObj->getSwitchCode() << "\"" <<  "," \
      << "\"" <<  dbObj->getCli() << "\"" <<  "," \
      << "\"" <<  dbObj->getDdi() << "\"" <<  "," \
      << "\"" <<  dbObj->getMsisdn() << "\"" <<  "," \
      << "\"" <<  dbObj->getIMSI() << "\"" <<  "," \
      << "\"" <<  dbObj->getMsrn() << "\"" <<  "," \
      << "\"" <<  dbObj->getAppDur() << "\"" <<  "," \
      << "\"" <<  dbObj->getDisconnectReqBy() << "\"" <<  "," \
      << "\"" <<  dbObj->getCallCause() << "\"" <<  "," \
      << "\"" <<  dbObj->getFwdType() << "\"" <<  "," \
      << "\"" <<  dbObj->getFwDdi() << "\"" <<  "," \
      << "" << "," \
      << "" << "," \
      << "" << "," \
      << "\"" <<  dbObj->getConnectionState() << "\"" <<  "," \
      << "0" << "," \
      << "" << "," \
      << "\"" <<  dbObj->getVlr() << "\"" <<  "," \
      << "\"" <<  dbObj->getMsc() << "\"" <<  ")";
    query = insertQuery.str();
    m_stmt->execute(query.c_str());
    delete m_stmt;
    return true;
  }
  catch(sql::SQLException &e) 
  {
    return false;
  }
}
