#ifndef CA_DB_DRIVER
#define CA_DB_DRIVER
#include "CAUtilites.h"
#define MAX_LEN 255
namespace CA {
    enum  class CASQLRETURN{
        CASQL_FAIL=-1,
        CASQL_SUCCESS,
        
    };
    class ColumnInfo{
        public:
        unsigned char colName[MAX_LEN] ;
        short int DataTypePtr;
    };
    union Data{
        char data[MAX_LEN];
        short int idata;
        long int lidata;
        float fdata;
        double ddata;
        
    };
class ColumnDetails{
    public:
      
    ColumnInfo m_cinfo[MAX_LEN];
    Data m_data[MAX_LEN];
    short int colCount; 
    long int len;


};

using DBCOL =  std::map<std::string,Data>;
using DBMAP = std::map<int,DBCOL*>;


class DBInterface{
    private:
    HENV    m_henv;
    HDBC    m_hdbc;
    SQLHSTMT m_hstmt;
    private:
    void getExecutionError(short int handleType);
    public:
    std::string m_strLastKnownError;
    std::string m_dbType;
    std::string m_connectionString;
    std::string m_connectionTimeout;
    std::string m_lastQuery;
    bool m_connectionState;
    std::string m_strSqlState;
    
    
    public:
    
     CASQLRETURN openConnection(std::string conStr,std::string timeOut);
     CASQLRETURN closeConnection();
     CASQLRETURN openConnection();
     CASQLRETURN executeDirectQuery(std::string sqlquery);
    
    //virtual CASQLRETURN bindSqlCol(Data*)=0;
    CASQLRETURN bindSqlCol(ColumnDetails&);
    CASQLRETURN fetchSqlData(DBMAP&,ColumnDetails*);
    CASQLRETURN executeQuery(std::string sqlquery,DBMAP&);
    CASQLRETURN prepareAndExeuteQuery(std::string sqlquery,DBMAP&);
    CASQLRETURN clearExecution();
    void setConnectionInfo(std::string constr, std::string timeout, std::string dbType);
    void setLastKnownError(std::string error){
           m_strLastKnownError = error;
    }
     std::string getLastKnownError(){
        return m_strLastKnownError;
    }
    void setLastQuery(std::string qry){
        m_lastQuery = qry;
    }
    std::string getLastQuery(){return m_lastQuery;}
    std::string getDbType() {return m_dbType;}
    void setDbType(std::string str){ m_dbType = str;}
    void setConnectionState(bool state){m_connectionState =  state;}
    bool getConnectionState(){return m_connectionState; }
    void setSqlState(std::string state){m_strSqlState=state;}
    std::string getSqlState(){return m_strSqlState;}
    

};

class DBMSSql : public DBInterface{
    public:
    DBMSSql()=default;
    DBMSSql(std::string conStr,std::string timeOut,std::string type){setConnectionInfo(conStr,timeOut,type);}
    virtual ~DBMSSql(){
        closeConnection();
    };
};
class DBMYSql : public DBInterface {
    public:
    DBMYSql()=default;
    DBMYSql(std::string conStr,std::string timeOut,std::string type){setConnectionInfo(conStr,timeOut,type);}
    virtual~DBMYSql(){
        closeConnection();
    };
};

};
#endif