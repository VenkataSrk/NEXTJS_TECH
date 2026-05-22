#ifndef BUSINESS_INTERFACE
#define BUSINESS_INTERFACE
#include "CAUtilityInternal.h"
#include "BusinessConfigObject.h"
#include "SignalParam.h"
#include "ChargingParam.h"
#include "GmscParam.h"
#include "RoutingParam.h"
enum class retCode{
    BS_ERROR_UNKOWN = -1,
    BS_SUCCESS,
    BS_SUCCESS_DIRECTDIAL,
    BS_NULL_OBJECT,
    BS_DB_CONNECTION_FAILED,
    BS_DB_NO_RECORD,
    BS_DB_QUERY_FAILED,
    BS_DB_QUERY_STRING_ERROR,
    BS_ACCOUNT_BLOCKED,
    BS_NO_ROUTE,
    BS_ROW_OUTOFBOUND,
    BS_XLAT_ERROR,
    BS_OUTGOING_BARRED,
    BS_NO_BUNDLE_BALANCE,
    BS_NO_MASTER_BALANCE,
    BS_NO_BUNDLE_PACKAGE,
    BS_NO_MASTER_PACKAGE,
    BS_NO_PACKAGE_SET,
    BS_NO_SIGNALIP,
    BS_TIME_EXCEED,
    BS_SUB_MULTI_NUMBER_ERROR,
    BS_SUB_ERROR,    
    BS_PORTEDOUT_NUMBER,
    BS_UNALLOCATED_NUMBER,
    BS_EXCHANGE_ROUTING_ERROR,
    BS_ABSENT_SUBSCRIBER,
    BS_HLR_REQUEST,
    BS_HLR_FAILED,

};
enum class APPLICATIONTYPE{
    TYPE_NONE,
    TYPE_CHARGING,
    TYPE_GMSC
};
class BSInterface{
    private: // member functions
    std::string formQuery(std::string DBtype, std::string spName, std::string spValue);
    std::string m_lastKnownError;
    std::string m_appID;
    APPLICATIONTYPE m_applicationType;  
    std::string m_strRequestTime;
    std::string m_strResponseTime;
    
    public: // member variables
    chargingSharedData m_charingData;
    public: // memeber functions

    retCode executeQuery(CA::DBInterface*,std::string /*query header*/ , std::string /*query values*/, CA::DBMAP& dbMap );
    retCode reconnectExecuteQuery(CA::DBInterface*,std::string /*query header*/ , CA::DBMAP& dbMap);
    retCode prepareexecuteQuery(CA::DBInterface*,std::string /*query header*/ , std::string /*query values*/, CA::DBMAP& dbMap);
    retCode getData(std::string columnName,int rowID, CA::DBMAP&, std::string& outVal );
    retCode getData(std::string columnName,int rowID, CA::DBMAP&,  int& outVal );
    retCode getData(std::string columnName,int rowID, CA::DBMAP&,  long& outVal );
    retCode getData(std::string columnName,int rowID, CA::DBMAP&,  float& outVal );
    retCode getData(std::string columnName,int rowID, CA::DBMAP&,  double& outVal );
   std::string getLastKnownError(){ return m_lastKnownError;}
   void setLastKnownError(std::string err) { m_lastKnownError =  err;}
   std::string xlat(std::string number, std::string prefix);
   std::string GetTime();
   std::string getTimeStrFormated(std::time_t);
   std::string getDataBaseName(std::string);
   int getEquivalentSipError(retCode ret);
   void setAppType(APPLICATIONTYPE it){ m_applicationType = it;}
   APPLICATIONTYPE getAppType(){return m_applicationType;}
   std::string generateAppID();
   std::string getAppID() { return m_appID;};
   void setBsRequestTime(){ m_strRequestTime =  GetTime();}
   void setBSResponseTime(){ m_strResponseTime =  GetTime();}
   std::string getBSRequestTime(){return m_strRequestTime;}
   std::string getBsResponseTime(){return m_strResponseTime;}
   bool ClearDBData(CA::DBMAP&); 

   
};
#endif
