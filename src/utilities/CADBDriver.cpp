#include "CADBDriver.h"
#include "LogHandler.h"


CA::CASQLRETURN CA::DBInterface::openConnection(std::string conStr,std::string timeOut){
loginfo("","CA::DBInterface::openConnection conStr:(%s), timeOut:(%s)",conStr.c_str(),timeOut.c_str());
#if 0
    SQLRETURN retcode;   
    retcode = ::SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HENV, &m_henv);
    if(retcode != SQL_SUCCESS ){
        getExecutionError(SQL_HANDLE_ENV);
        if(retcode != SQL_SUCCESS_WITH_INFO)
            return CA::CASQLRETURN::CASQL_FAIL;
    }

    retcode = ::SQLSetEnvAttr( m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
    if(retcode != SQL_SUCCESS ){ 
        getExecutionError(SQL_HANDLE_ENV);
        if(retcode != SQL_SUCCESS_WITH_INFO){
            ::SQLFreeHandle( SQL_HANDLE_ENV, m_henv);
            return CA::CASQLRETURN::CASQL_FAIL;
        }
    }

    retcode = ::SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);
    if(retcode != SQL_SUCCESS ){
        getExecutionError(SQL_HANDLE_DBC);
        if(retcode != SQL_SUCCESS_WITH_INFO){
            ::SQLFreeHandle( SQL_HANDLE_ENV, m_henv);
            return CA::CASQLRETURN::CASQL_FAIL;
        }
    }

    retcode = ::SQLSetConnectAttr(m_hdbc,SQL_LOGIN_TIMEOUT,(SQLPOINTER)5,0);
    if(retcode != SQL_SUCCESS ){
        getExecutionError(SQL_HANDLE_DBC);
    }
    
    char connectstr[conStr.length()];
    std::size_t length = conStr.copy(connectstr,conStr.length(),0);
    connectstr[length] = '\0';

    //strncpy(connectstr,conStr.c_str(),conStr.length());
    SQLCHAR connectstring_out[1024] = "\0";
    SQLSMALLINT	len;

     retcode = ::SQLDriverConnect(m_hdbc,NULL,(SQLCHAR*)connectstr,SQL_NTS,connectstring_out,sizeof(connectstring_out),&len,SQL_DRIVER_COMPLETE);
     if(retcode != SQL_SUCCESS ){
        getExecutionError(SQL_HANDLE_DBC); 
        
        if(retcode != SQL_SUCCESS_WITH_INFO){
            ::SQLFreeHandle( SQL_HANDLE_DBC, m_hdbc);
            ::SQLFreeHandle( SQL_HANDLE_ENV, m_henv);
            return CA::CASQLRETURN::CASQL_FAIL;
        }
        

    }
    return CA::CASQLRETURN::CASQL_SUCCESS;
#endif
    setConnectionInfo(conStr,timeOut,getDbType());
    return openConnection();
}

CA::CASQLRETURN CA::DBInterface::openConnection(){
    loginfo("","CA::DBInterface::openConnection");
    SQLRETURN retcode;   

    if(m_connectionString.empty()){
        setLastKnownError("Connection string is empty");
        loginfo("","CA::DBInterface::openConnection() Error: Connection string is empty");
        return CA::CASQLRETURN::CASQL_FAIL;
    }

    retcode = ::SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HENV, &m_henv);
    if(retcode != SQL_SUCCESS ){
        getExecutionError(SQL_HANDLE_ENV);
        if(retcode != SQL_SUCCESS_WITH_INFO)
            return CA::CASQLRETURN::CASQL_FAIL;
    }

    retcode = ::SQLSetEnvAttr( m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
    if(retcode != SQL_SUCCESS ){ 
        getExecutionError(SQL_HANDLE_ENV);
        if(retcode != SQL_SUCCESS_WITH_INFO){
            ::SQLFreeHandle( SQL_HANDLE_ENV, m_henv);
            return CA::CASQLRETURN::CASQL_FAIL;
        }
    }

    retcode = ::SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);
    if(retcode != SQL_SUCCESS ){
        getExecutionError(SQL_HANDLE_DBC);
        if(retcode != SQL_SUCCESS_WITH_INFO){
            ::SQLFreeHandle( SQL_HANDLE_ENV, m_henv);
            return CA::CASQLRETURN::CASQL_FAIL;
        }
    }

    retcode = ::SQLSetConnectAttr(m_hdbc,SQL_LOGIN_TIMEOUT,(SQLPOINTER)5,0);
    if(retcode != SQL_SUCCESS ){
        getExecutionError(SQL_HANDLE_DBC);
    }
    
    
    char connectstr[m_connectionString.length()];
    std::size_t length = m_connectionString.copy(connectstr,m_connectionString.length(),0);
    connectstr[length] = '\0';

    //strncpy(connectstr,conStr.c_str(),conStr.length());
    SQLCHAR connectstring_out[1024] = "\0";
    SQLSMALLINT	len;

     retcode = ::SQLDriverConnect(m_hdbc,NULL,(SQLCHAR*)connectstr,SQL_NTS,connectstring_out,sizeof(connectstring_out),&len,SQL_DRIVER_COMPLETE);
     if(retcode != SQL_SUCCESS ){
        getExecutionError(SQL_HANDLE_DBC); 
        
        if(retcode != SQL_SUCCESS_WITH_INFO){
            ::SQLFreeHandle( SQL_HANDLE_DBC, m_hdbc);
            ::SQLFreeHandle( SQL_HANDLE_ENV, m_henv);
            return CA::CASQLRETURN::CASQL_FAIL;
        }
        

    }
    setConnectionState(true);
    return CA::CASQLRETURN::CASQL_SUCCESS;

}

CA::CASQLRETURN CA::DBInterface::closeConnection(){
    loginfo("","CA::DBInterface::closeConnection()");
    if(m_hstmt)
        SQLFreeHandle(SQL_HANDLE_STMT,m_hstmt);
    
    if(m_hdbc){
        ::SQLDisconnect(m_hdbc);
        SQLFreeHandle(SQL_HANDLE_DBC,m_hdbc);
    }

    if(m_henv)
        SQLFreeHandle(SQL_HANDLE_ENV,m_henv);
    
    setConnectionState(false);

    return CA::CASQLRETURN::CASQL_SUCCESS;
}
CA::CASQLRETURN CA::DBInterface::executeDirectQuery(std::string sqlquery){
    loginfo("","CA::DBInterface::executeDirectQuery");
    SQLRETURN retcode;   
 
    retcode=::SQLExecDirect(m_hstmt,(SQLCHAR*)sqlquery.c_str(),SQL_NTS);
    if(retcode != SQL_SUCCESS ){
        getExecutionError(SQL_HANDLE_STMT);
        if(retcode != SQL_SUCCESS_WITH_INFO){
            clearExecution();
            return CA::CASQLRETURN::CASQL_FAIL;
        }
        
    }
    return CA::CASQLRETURN::CASQL_SUCCESS;
}
CA::CASQLRETURN CA::DBInterface::bindSqlCol(ColumnDetails &ci){
    loginfo("","CA::DBInterface::bindSqlCol");
    if(m_hstmt == SQL_NULL_HSTMT){
        setLastKnownError("statement handle is NULL");
        return CA::CASQLRETURN::CASQL_FAIL;
    }
    SQLRETURN retcode;
    ci.colCount = 0;
    SQLSMALLINT    NumCols;
    retcode = ::SQLNumResultCols(m_hstmt,&ci.colCount) ;
    if(retcode != SQL_SUCCESS){
        getExecutionError(SQL_HANDLE_STMT);
        if(retcode != SQL_SUCCESS_WITH_INFO){
            clearExecution();
            return CA::CASQLRETURN::CASQL_FAIL;
        }
    }
    

    retcode = ::SQLNumParams(m_hstmt,&NumCols); 
        if(retcode!= SQL_SUCCESS ){
        getExecutionError(SQL_HANDLE_STMT);
        if(retcode != SQL_SUCCESS_WITH_INFO){
            clearExecution();
            return CA::CASQLRETURN::CASQL_FAIL;
        }
    }
    if(NumCols > 0)
        ci.colCount =  NumCols;
    
    
    for(unsigned short int count = 0 ;count < ci.colCount;count++){
        
        short int NameLengthPtr;
        long unsigned int ColumnSizePtr;
        short int DecimalDigitsPtr;
        short int NullablePtr;

        retcode = ::SQLDescribeCol(m_hstmt,count+1,ci.m_cinfo[count].colName,MAX_LEN,&NameLengthPtr,&(ci.m_cinfo[count].DataTypePtr),&ColumnSizePtr,&DecimalDigitsPtr,&NullablePtr);
        if(retcode != SQL_SUCCESS){
            getExecutionError(SQL_HANDLE_STMT);
            continue;
        }
        
        switch (ci.m_cinfo[count].DataTypePtr)
        {
            case SQL_CHAR:
            case SQL_VARCHAR:
            case SQL_DATETIME: 
            case SQL_NUMERIC:
            case SQL_DECIMAL:
            if(::SQLBindCol(m_hstmt,count+1,SQL_C_CHAR,ci.m_data[count].data,sizeof(ci.m_data[count].data)+1,&ci.len) != SQL_SUCCESS)
                getExecutionError(SQL_HANDLE_STMT);
            break;
            case SQL_INTEGER:
            //bindType =  SQL_C_LONG;
            if(::SQLBindCol(m_hstmt,count+1,SQL_C_LONG,&ci.m_data[count].lidata,sizeof(ci.m_data[count].lidata),&ci.len) != SQL_SUCCESS)
                getExecutionError(SQL_HANDLE_STMT);
            break;
            case SQL_SMALLINT:
            //bindType =  SQL_C_SSHORT;
            if(::SQLBindCol(m_hstmt,count+1,SQL_C_SSHORT,&ci.m_data[count].idata,sizeof(ci.m_data[count].idata),&ci.len) != SQL_SUCCESS)
                getExecutionError(SQL_HANDLE_STMT);
            break;
            case SQL_FLOAT:
            case SQL_DOUBLE:
            //bindType =  SQL_C_DOUBLE;
            if(::SQLBindCol(m_hstmt,count+1, SQL_C_DOUBLE,&ci.m_data[count].ddata,sizeof(ci.m_data[count].ddata),&ci.len) != SQL_SUCCESS)
                getExecutionError(SQL_HANDLE_STMT);
            break;
            case SQL_REAL:
            //bindType =  SQL_C_FLOAT;
            if(::SQLBindCol(m_hstmt,count+1,SQL_C_FLOAT,&ci.m_data[count].fdata,sizeof(ci.m_data[count].fdata),&ci.len) != SQL_SUCCESS)
                getExecutionError(SQL_HANDLE_STMT);
            break;
            case SQL_UNKNOWN_TYPE:
            default:
            //bindType = SQL_C_CHAR;
            if(::SQLBindCol(m_hstmt,count+1,SQL_C_CHAR,ci.m_data[count].data,sizeof(ci.m_data[count].data)+1,&ci.len) != SQL_SUCCESS)
                getExecutionError(SQL_HANDLE_STMT);
                break;
        }
        

    }
    return CA::CASQLRETURN::CASQL_SUCCESS;    
}

CA::CASQLRETURN CA::DBInterface::fetchSqlData(DBMAP& rsData,ColumnDetails* cdData){
loginfo("","CA::DBInterface::fetchSqlData");
if(cdData == nullptr){
    setLastKnownError("Column Details NULL");
    return CA::CASQLRETURN::CASQL_FAIL;

}
bool breakLoop =  false;
CA::CASQLRETURN retVal = CA::CASQLRETURN::CASQL_SUCCESS;
SQLRETURN ret;
    for(int row = 0; cdData->colCount > 0; row++){
        ret =  ::SQLFetch(m_hstmt) ;
        switch(ret){
            case SQL_NO_DATA:
                breakLoop =  true;
            break;
            case SQL_SUCCESS:
                breakLoop =  false;
            break;
            case SQL_SUCCESS_WITH_INFO:
                breakLoop =  false;
            break;
            default:
            breakLoop =  true;
            retVal =  CA::CASQLRETURN::CASQL_FAIL;
            getExecutionError(SQL_HANDLE_STMT);
            break;
        }
        //std::unique_ptr<CA::DBCOL> colData( new CA::DBCOL);
        CA::DBCOL *colData =  new CA::DBCOL;
        for(int cCnt = 0 ; cCnt < cdData->colCount ; cCnt++){
            //Data* dd = new Data;
            //*dd =  cdData->m_data[cCnt];
            std::string ss = (char*)cdData->m_cinfo[cCnt].colName;
            (*colData)[ss] = cdData->m_data[cCnt];
            
            //colData->insert(std::pair{cdData->m_cinfo[cCnt].colName,std::move(cdData->m_data[cCnt])});
        }
     
        if(breakLoop){
            delete colData;
            colData = nullptr;
            break;
          }
        
        rsData[row] =  std::move(colData);

    }
    if(retVal == CA::CASQLRETURN::CASQL_FAIL){
        clearExecution();
    }
return retVal;

}
CA::CASQLRETURN CA::DBInterface::executeQuery(std::string sqlquery,DBMAP& rsData){
loginfo("","CA::DBInterface::executeQuery");
if(sqlquery.empty()){
    setLastKnownError("Empty query");
    return CA::CASQLRETURN::CASQL_FAIL;
}
SQLRETURN retcode;
retcode = ::SQLAllocHandle(SQL_HANDLE_STMT,m_hdbc,&m_hstmt) ;
if (retcode != SQL_SUCCESS){
    getExecutionError(SQL_HANDLE_STMT);
    std::cout<<"CA::DBInterface::executeQuery != SQL_SUCCESS "<<std::endl;
    loginfo("","Error: CA::DBInterface::executeQuery != SQL_SUCCESS");
if(retcode != SQL_SUCCESS_WITH_INFO){
    std::cout<<"CA::DBInterface::executeQuery set connection state was fail"<<std::endl;
    loginfo("","Error: CA::DBInterface::executeQuery set connection state was fail");
    setConnectionState(false);
    return CA::CASQLRETURN::CASQL_FAIL;
}
}

#if 0
std::size_t found = sqlquery.find("exec");
if (found == std::string::npos || found > 0)
    sqlquery = "exec " + sqlquery;
#endif

setLastQuery(sqlquery); // this for log purpose
//SQLCHAR Statement[1024] = sqlquery.c_str();
#if 0
if(::SQLPrepare(m_hstmt,(SQLCHAR*)sqlquery.c_str(),sqlquery.length()) != SQL_SUCCESS){
    getExecutionError(SQL_HANDLE_STMT);
    return CA::CASQLRETURN::CASQL_FAIL;
}
#endif

 if(executeDirectQuery(sqlquery)!= CA::CASQLRETURN::CASQL_SUCCESS)
{
    std::cout<<"CA::DBInterface::executeQuery executeDirectQuery failed"<<std::endl;
    loginfo("","Error: CA::DBInterface::executeQuery executeDirectQuery failed %s",getLastKnownError().c_str());
     return CA::CASQLRETURN::CASQL_FAIL;
}
 
/* int retryCount(0);
while(retryCount<2)
{
 if(executeDirectQuery(sqlquery)!= CA::CASQLRETURN::CASQL_SUCCESS)
 {
    retryCount++;
    //std::cout<<"*****************executeDirectQuery failed"<<std::endl;
    if(strncmp(getSqlState().c_str(),"08S01", getSqlState().length())==0)
    {
        closeConnection();
        //std::cout<<"*****************executeDirectQuery failed openconnection"<<std::endl;
        if(openConnection()==CA::CASQLRETURN::CASQL_FAIL)
        {
            //std::cout<<"*****************executeDirectQuery failed openconnection failed"<<std::endl;
            return CA::CASQLRETURN::CASQL_FAIL;
        }
        else
        {
            SQLRETURN retcode;
            retcode = ::SQLAllocHandle(SQL_HANDLE_STMT,m_hdbc,&m_hstmt) ;
            if (retcode != SQL_SUCCESS)
            {
                getExecutionError(SQL_HANDLE_STMT);
                std::cout<<"CA::DBInterface::executeQuery != SQL_SUCCESS "<<std::endl;
                if(retcode != SQL_SUCCESS_WITH_INFO)
                {
                   // std::cout<<"CA::DBInterface::executeQuery set connection state was fail"<<std::endl;
                    setConnectionState(false);
                    return CA::CASQLRETURN::CASQL_FAIL;
                }
            }
        }
    }
 }
 else
 {
    //std::cout<<"*****************executeDirectQuery successs"<<std::endl;
    break;
 } 
} */  
ColumnDetails *cdData =  new ColumnDetails();

if(cdData){
    if(bindSqlCol(*cdData) != CA::CASQLRETURN::CASQL_SUCCESS){
        delete cdData;
        cdData = nullptr;
        return CA::CASQLRETURN::CASQL_FAIL;
    }
}



if(fetchSqlData(rsData,cdData) != CA::CASQLRETURN::CASQL_SUCCESS){
    delete cdData;
    cdData = nullptr;
    return CA::CASQLRETURN::CASQL_FAIL;
}
if(cdData){
    delete cdData;
    cdData = nullptr;
}

if(clearExecution() != CA::CASQLRETURN::CASQL_SUCCESS){
    getExecutionError(SQL_HANDLE_STMT);
    return CA::CASQLRETURN::CASQL_FAIL;
}

return CA::CASQLRETURN::CASQL_SUCCESS;    
}

CA::CASQLRETURN CA::DBInterface::prepareAndExeuteQuery(std::string sqlquery,DBMAP& rsData){
    if(sqlquery.empty()){
    setLastKnownError("Empty query");
    return CA::CASQLRETURN::CASQL_FAIL;
    }
    if(::SQLAllocHandle(SQL_HANDLE_STMT,m_hdbc,&m_hstmt) != SQL_SUCCESS){
        getExecutionError(SQL_HANDLE_STMT);
        return CA::CASQLRETURN::CASQL_FAIL;
    }
    std::cout << "Statment Handle " << sqlquery  << " -- " << m_hstmt ;
    std::size_t found = sqlquery.find("exec");
    if (found == std::string::npos || found > 0)
        sqlquery = "exec " + sqlquery;

    setLastQuery(sqlquery); // this for log purpose

    SQLCHAR Statement[60]= {0};
    SQLSMALLINT numCols;
    snprintf((char*)Statement,sqlquery.length(),"%s",sqlquery.c_str() );
    if(::SQLPrepare(m_hstmt,Statement,strlen((char*)Statement)) != SQL_SUCCESS){
        getExecutionError(SQL_HANDLE_STMT);
        return CA::CASQLRETURN::CASQL_FAIL;
    }

    ColumnDetails *cdData =  new ColumnDetails();

    if(cdData){
        if(bindSqlCol(*cdData) != CA::CASQLRETURN::CASQL_SUCCESS){
            delete cdData;
            cdData = nullptr;
            return CA::CASQLRETURN::CASQL_FAIL;
        }
    }

    if(::SQLExecute (m_hstmt) != SQL_SUCCESS){
        getExecutionError(SQL_HANDLE_STMT);
        return CA::CASQLRETURN::CASQL_FAIL;
    }

    if(fetchSqlData(rsData,cdData) != CA::CASQLRETURN::CASQL_SUCCESS){
        delete cdData;
        cdData = nullptr;
        return CA::CASQLRETURN::CASQL_FAIL;
    }
    if(cdData){
        delete cdData;
        cdData = nullptr;
    }
    if(clearExecution() != CA::CASQLRETURN::CASQL_SUCCESS){
        getExecutionError(SQL_HANDLE_STMT);

        return CA::CASQLRETURN::CASQL_FAIL;
    }
    return CA::CASQLRETURN::CASQL_SUCCESS;  
}

CA::CASQLRETURN CA::DBInterface::clearExecution(){
    loginfo("","CA::DBInterface::clearExecution()");
    ::SQLCloseCursor(m_hstmt);
    SQLFreeStmt(m_hstmt, SQL_CLOSE);
    SQLFreeStmt(m_hstmt, SQL_UNBIND);
    SQLFreeStmt(m_hstmt, SQL_RESET_PARAMS);
    if(SQLFreeStmt(m_hstmt, SQL_DROP) != SQL_SUCCESS)
    {
        return CA::CASQLRETURN::CASQL_FAIL;
    }
    return CA::CASQLRETURN::CASQL_SUCCESS;
#if 0
    if(::SQLCloseCursor(m_hstmt) != SQL_SUCCESS){ //ODBC3.x
        if(::SQLFreeStmt(m_hstmt,SQL_CLOSE) != SQL_SUCCESS){ // ODBC 2.x
            return CA::CASQLRETURN::CASQL_FAIL;    
        }
    }
        
    if(SQLFreeStmt(m_hstmt, SQL_UNBIND) != SQL_SUCCESS)
        return CA::CASQLRETURN::CASQL_FAIL;
        
    if(SQLFreeStmt(m_hstmt, SQL_RESET_PARAMS) != SQL_SUCCESS)
        return CA::CASQLRETURN::CASQL_FAIL;

    return CA::CASQLRETURN::CASQL_SUCCESS;    
#endif
}
void CA::DBInterface::getExecutionError(short int handleType){
    loginfo("","CA::DBInterface::getExecutionError()"); 
    SQLCHAR sqlstate[1024];
    SQLINTEGER  NativeErrorPtr;
    SQLCHAR MessageText[1024];
    SQLSMALLINT TextLengthPtr;

    switch (handleType)
    {
        case SQL_HANDLE_ENV:
            ::SQLGetDiagRec(SQL_HANDLE_ENV, m_henv,1,sqlstate,&NativeErrorPtr,MessageText,sizeof(MessageText),&TextLengthPtr);
            setLastKnownError((char*)MessageText);
            setSqlState((char*)sqlstate);
        break;
        case SQL_HANDLE_STMT:
            ::SQLGetDiagRec(SQL_HANDLE_STMT, m_hstmt,1,sqlstate,&NativeErrorPtr,MessageText,sizeof(MessageText),&TextLengthPtr);
            setLastKnownError((char*)MessageText);
            setSqlState((char*)sqlstate);
        break;
        case SQL_HANDLE_DBC:
            ::SQLGetDiagRec(SQL_HANDLE_DBC, m_hdbc,1,sqlstate,&NativeErrorPtr,MessageText,sizeof(MessageText),&TextLengthPtr);
            setLastKnownError((char*)MessageText);
            setSqlState((char*)sqlstate);
        break;    
        default:
        break;
    }

    
    

}
void CA::DBInterface::setConnectionInfo(std::string constr, std::string timeout, std::string dbType){
    m_connectionString =  constr;
    m_connectionTimeout =  timeout;
    setDbType(dbType);
}
