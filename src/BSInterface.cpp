#include "BSInterface.h"
#include "LogHandler.h"

std::string BSInterface::formQuery(std::string DBtype, std::string spName, std::string spValue){
    std::string retQry;
    if(spName.empty() || spValue.empty() || DBtype.empty())
        return retQry = "";

    if(DBtype.compare("mssql") == 0){
        retQry = "exec " + spName + " " + spValue;

    }else if(DBtype.compare("mysql") == 0){
        retQry = "call " + spName + " (" + spValue + " )";

    }else {
        retQry = "";
    }

    return retQry;
}

retCode BSInterface::executeQuery(CA::DBInterface* dbInst,std::string spName , std::string spValue,CA::DBMAP& dbMap)
{
    if(dbInst == nullptr){
        setLastKnownError("DB Instance found NULL");
        return retCode::BS_NULL_OBJECT;
    }
    
    std::string Query = formQuery(dbInst->getDbType(),spName, spValue);
    if(Query.empty()){
        setLastKnownError("query string formation error");
        return retCode::BS_DB_QUERY_STRING_ERROR;
    }
    if(dbInst->getConnectionState() == false){
	loginfo("","BSInterface::executeQuery()  DB Reconnected --> Error: %s",dbInst->getLastKnownError().c_str());
        if(dbInst->openConnection() != CA::CASQLRETURN::CASQL_FAIL){
            setLastKnownError(dbInst->getLastKnownError());
            return retCode::BS_DB_CONNECTION_FAILED;     
        }
    }
    loginfo("","BSInterface::executeQuery() DataBase: %s, call %s ( %s ) ", getDataBaseName(dbInst->m_connectionString).c_str(),spName.c_str(),spValue.c_str());
    if (dbInst->executeQuery( Query, dbMap) != CA::CASQLRETURN::CASQL_SUCCESS){
        return reconnectExecuteQuery(dbInst,Query,dbMap);
    }
    return retCode::BS_SUCCESS;
}
retCode BSInterface::reconnectExecuteQuery(CA::DBInterface* dbInst,std::string Query,CA::DBMAP& dbMap)
{
    loginfo("","BSInterface::reconnectExecuteQuery()");
    if((strncmp(dbInst->getSqlState().c_str(),"08S01", dbInst->getSqlState().length())==0)||(strstr(dbInst->getLastKnownError().c_str(),"Lost connection to MySQL server during query")))
    {
        dbInst->closeConnection();
        if(dbInst->openConnection()==CA::CASQLRETURN::CASQL_FAIL)
        {
            setLastKnownError(dbInst->getLastKnownError()); 
            return retCode::BS_DB_QUERY_FAILED;
        }
        else
        {
            if (dbInst->executeQuery( Query, dbMap) != CA::CASQLRETURN::CASQL_SUCCESS){
            setLastKnownError(dbInst->getLastKnownError());        
            return retCode::BS_DB_QUERY_FAILED;
            }
        }
    }
    return retCode::BS_SUCCESS;
}

retCode BSInterface::prepareexecuteQuery(CA::DBInterface* dbInst,std::string spName , std::string spValue,CA::DBMAP& dbMap){
    if(dbInst == nullptr){
        setLastKnownError("DB Instance found NULL");
        return retCode::BS_NULL_OBJECT;
    }
    
    std::string Query = formQuery(dbInst->getDbType(),spName, spValue);
    if(Query.empty()){
        setLastKnownError("query string formation error");
        return retCode::BS_DB_QUERY_STRING_ERROR;
    }
    //CAlog_DEBUG("BSInterface::executeQuery() %s",Query.c_str());
    if (dbInst->prepareAndExeuteQuery( Query, dbMap) != CA::CASQLRETURN::CASQL_SUCCESS){
       setLastKnownError(dbInst->getLastKnownError());
        return retCode::BS_DB_QUERY_FAILED;
    }

    return retCode::BS_SUCCESS;
}


retCode BSInterface::getData(std::string columnName,int rowID,CA::DBMAP& dbMap, std::string& outVal ){
    outVal = "";
    if(dbMap.size() == 0 ){
        setLastKnownError("No Sql Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    if( dbMap.size() > rowID){
        CA::DBCOL *colData =  dbMap[rowID];
        auto it = colData->find(columnName);
        if(it != colData->end()){
            CA::Data *val = &it->second;
            std::string s(val->data);
            outVal =  s;
            return retCode::BS_SUCCESS;
        }else{
             setLastKnownError("columnName not found -- "+columnName);
        }
    }else{
        setLastKnownError("row ID and Map size not matching");
    }
    return retCode::BS_ERROR_UNKOWN;
}
retCode BSInterface::getData(std::string columnName,int rowID, CA::DBMAP& dbMap,  int& outVal ){
    outVal = -1;
    if(dbMap.size() == 0 ){
        setLastKnownError("No Sql Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    if(dbMap.size() > rowID){
        CA::DBCOL *colData =  dbMap[rowID];
        auto it = colData->find(columnName);
        if(it != colData->end()){
            CA::Data *val = &it->second;
            outVal =  val->idata;
            return retCode::BS_SUCCESS;
        }else{
             setLastKnownError("columnName not found -- "+columnName);
        }
    }else{
        setLastKnownError("row ID and Map size not matching or Map is Nil");
    }

return retCode::BS_ERROR_UNKOWN;
}
retCode BSInterface::getData(std::string columnName,int rowID, CA::DBMAP& dbMap,  long& outVal ){
     outVal = -1;
     if(dbMap.size() == 0 ){
        setLastKnownError("No Sql Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    if( dbMap.size() > rowID){
        CA::DBCOL *colData =  dbMap[rowID];
        auto it = colData->find(columnName);
        if(it != colData->end()){
            CA::Data *val = &it->second;
            outVal =  val->lidata;
            return retCode::BS_SUCCESS;
        }else{
              setLastKnownError("columnName not found -- "+columnName);
        }
    }else{
        setLastKnownError("row ID and Map size not matching or Map is Nil");
    }

return retCode::BS_ERROR_UNKOWN;
}
retCode BSInterface::getData(std::string columnName,int rowID, CA::DBMAP& dbMap,  float& outVal ){
    outVal = 0.0;
    if(dbMap.size() == 0 ){
        setLastKnownError("No Sql Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    if (dbMap.size() > rowID){
        CA::DBCOL *colData =  dbMap[rowID];
        auto it = colData->find(columnName);
        if(it != colData->end()){
            CA::Data *val = &it->second;
            outVal =  val->fdata;
            return retCode::BS_SUCCESS;
        }else{
              setLastKnownError("columnName not found -- "+columnName);
        }
    }else{
        setLastKnownError("row ID and Map size not matching or Map is Nil");
    }
return retCode::BS_ERROR_UNKOWN;
}
retCode BSInterface::getData(std::string columnName,int rowID, CA::DBMAP& dbMap,  double& outVal ){
    outVal = 0.0;
    if(dbMap.size() == 0 ){
        setLastKnownError("No Sql Record Found");
        return retCode::BS_DB_NO_RECORD;
    }
    if( dbMap.size() > rowID){
        CA::DBCOL *colData =  dbMap[rowID];
        auto it = colData->find(columnName);
        if(it != colData->end()){
            CA::Data *val = &it->second;
            outVal =  val->ddata;
            return retCode::BS_SUCCESS;
        }else{
            setLastKnownError("columnName not found -- "+columnName);
        }
    }else{
        setLastKnownError("row ID and Map size not matching or Map is Nil");
    }
return retCode::BS_ERROR_UNKOWN;
}
std::string BSInterface::xlat(std::string number, std::string prefix){
    std::string ret = "";

    ret = number.substr(prefix.length(),(number.length() - prefix.length()));
    return ret;
}
std::string BSInterface::GetTime() {
	#if 0
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);
	std::ostringstream ss;
	ss << (now->tm_year + 1900);
	ss << "-";
	if(10 > (now->tm_mon + 1)) {
		ss << "0";
	}
	ss << (now->tm_mon + 1);
	ss << "-";
	if(10 > now->tm_mday) {
		ss << "0";
	}
	ss << now->tm_mday;
	ss << " ";
	ss << now->tm_hour;
	ss << ":";
	ss << now->tm_min;
	ss << ":";
	ss << now->tm_sec;
	return ss.str();
    #endif
    
    std::time_t now =  std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buf[24] = {0};
    std::strftime(buf, sizeof(buf),"%Y-%0m-%0d %H:%M:%S", std::localtime(&now));
    return  std::string(buf);
}
std::string BSInterface::getTimeStrFormated(std::time_t frtTime){
     std::time_t now =  frtTime;
     char buf[24] = {0};
    std::strftime(buf, sizeof(buf),"%Y-%0m-%0d %H:%M:%S", std::localtime(&now));
    return  std::string(buf);
}
int BSInterface::getEquivalentSipError(retCode ret){
int siperr =  500;
    switch(ret){
        case retCode::BS_SUCCESS:
        siperr = 200;
        break;
        case retCode::BS_NO_SIGNALIP:
        case retCode::BS_NO_ROUTE:
            siperr =  404;
        break;
        case retCode::BS_ACCOUNT_BLOCKED:
        case retCode::BS_OUTGOING_BARRED:
        siperr =  403;
        break;
        case retCode::BS_NO_BUNDLE_BALANCE:
        case retCode::BS_NO_MASTER_BALANCE:
        siperr =  402;
        break;
        case retCode::BS_ABSENT_SUBSCRIBER:
        siperr =  480;
        break;
        case retCode::BS_TIME_EXCEED:
        siperr =  700;
        break;
        case retCode::BS_DB_CONNECTION_FAILED:
        case retCode::BS_DB_NO_RECORD:
        case retCode::BS_DB_QUERY_FAILED:
        case retCode::BS_DB_QUERY_STRING_ERROR:
        case retCode::BS_ERROR_UNKOWN:
        case retCode::BS_NULL_OBJECT:
        case retCode::BS_ROW_OUTOFBOUND:
        default:
        siperr =  500;
        break;
    }

    return siperr;
}

std::string BSInterface::generateAppID(){

   long appid = CAGSP::getCounter() ;
   std::string buf = CAGSP::getstrTimeHMSms();
   m_appID =  std::to_string(appid) +"-"+buf;
   return m_appID;
   
}

std::string BSInterface::getDataBaseName(std::string connectionString)
{
	std::string::size_type _nPos = connectionString.find("DATABASE");
	if (_nPos != std::string::npos) {

		std::string database = connectionString.substr(_nPos + 9); // 9 is the length of "DATABASE="
		std::string::size_type semicolonPos = database.find(';');

		if (semicolonPos != std::string::npos) {
			database = database.substr(0, semicolonPos);
		}
		return database;
	} else {
		return "DataBaseNotFound";
	}
}

bool BSInterface::ClearDBData(CA::DBMAP& dbMap)
{
    for (auto& itr : dbMap)
    {
        if (itr.second)
        {
            delete itr.second;
            itr.second = nullptr;
        }
    }

    dbMap.clear();
    return true;
}
