#include "RegisDBConnector.h"
#include "log.h"
#include "klog.h"
#include "dbdriver.h"
#include <vector>

#define THISMODULE "DBConnector"
using namespace std;
static unsigned int DB_Connection::g_totalNoOfDBConnections = 0;
static HDBC DB_Connection::hdbc_array[MAX_TOTAL_NO_OF_DB_CONNECTIONS] = {0, 0, 0};
static vector<HDBC> DB_Connection::hdbc_array_2 = {0, 0, 0};

DB_Connection::DB_Connection()
{
  CLog::Detail(THISMODULE,"Inside DEFAULT constructor... DB_Connection");
}

void DB_Connection::CloseStatementHandle(HSTMT hstmt)
{
  SQLFreeStmt(hstmt,SQL_CLOSE);
}

//---------- check authentication of the registration
int Registrar :: CheckAuthReq(char *str)
{
  std::cout<< " Inside authentication request function\n" <<endl;
  RETCODE ret; // ODBC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  SQLINTEGER rtnauthstat;
  UCHAR errmsg[100];

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS)
  {
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return -1;
  }
  strcpy((char *)SQLStmt,str);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);

  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in authentication\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return -1;
  }
  std::cout<< " execution succeeded"<<endl;

  SQLBindCol(hstmt, 1, SQL_C_LONG, &rtnauthstat, sizeof(rtnauthstat), NULL ); 
  ret = SQLFetch(hstmt);
  if(ret == SQL_NO_DATA_FOUND){
    std::cout << "No sql data found "<< endl;
  }

  CloseStatementHandle(hstmt);
  return rtnauthstat;
}

//--------------getuser details ------------------
std::string  Registrar :: getuserdetails(char *passstr)
{
  std::cout<< "\nInside get user details function\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  UCHAR errmsg[50]= "";

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return -1;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user details\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return -1;
  }
  std::cout<< " execution succeeded"<<endl;
  SQLBindCol(hstmt,2,SQL_CHAR , &passwd, sizeof(passwd), NULL );
  ret = SQLFetch(hstmt);
  CLog::Detail(THISMODULE,"Registrar in RegisDBConnector.cpp ret = %s", passwd);
  if(ret == SQL_NO_DATA_FOUND){
    std::cout << "No sql data found "<< endl;
  }
  CloseStatementHandle(hstmt);
  printf(" Password from DB %s\n", passwd);
  return passwd;
}
/*
 *
 */
//---------- registration--------------------------
int Registrar :: DoUserRegis(char *str)
{
  std::cout<< "\nInside do registration function\n"<<endl;
  RETCODE ret; // ODBC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  SQLINTEGER regstat;
  UCHAR errmsg[100];

  ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    std::cout<<"\nFailed SQLAllocHandle In registration\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
  }
  strcpy((char *)SQLStmt,str);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    std::cout<<"\nFailed SQLExecDirect in registration\n"<<errmsg;
    CloseStatementHandle(hstmt);
  }
  std::cout<< " execution succeeded\n "<<endl;
  SQLBindCol(hstmt, 1, SQL_C_LONG, &regstat, sizeof(regstat), NULL );
  ret  = SQLFetch(hstmt);
  if(ret == SQL_NO_DATA_FOUND){
    std::cout << "No sql data found "<< endl;
  }
  CloseStatementHandle(hstmt);	
  return regstat;
}
/*
 *
 */
//---------- check deletion of the registration--------------------------
int Registrar :: DoDeleteRegis(char *str)
{
  std::cout<<"Inside delete registration"<<endl;

  RETCODE ret; // ODBC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  SQLINTEGER delstat;
  UCHAR errmsg[100];

  ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle In deletion\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
  }
  strcpy((char *)SQLStmt,str);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in deletion\n"<<errmsg;
    CloseStatementHandle(hstmt);
  }
  std::cout<< " execution succeeded\n "<<endl;
  SQLBindCol(hstmt, 1, SQL_C_LONG, &delstat, sizeof(delstat), NULL );
  ret  = SQLFetch(hstmt);
  if(ret != SQL_NO_DATA_FOUND){
    std::cout << "No sql data found "<< endl;
  }
  CloseStatementHandle(hstmt);
  return delstat;
}
/*
 *
 */
//-----------------Registration status Update-----------------

int Registrar :: DoRegisStatus(char *str)
{
  std::cout<<"Inside registration status update"<<endl;

  RETCODE ret; // ODBC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  SQLINTEGER updatestat;
  UCHAR errmsg[100];

  ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle In Registration status update\t\t"<<errmsg;
    CLog::Detail(THISMODULE,"Registrar in RegisDBConnector.cpp ","Failed SQLAllocHandle in Registration status update");
    CloseStatementHandle(hstmt);
  }
  strcpy((char *)SQLStmt,str);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in Registration status update\n"<<errmsg;
    CLog::Detail(THISMODULE,"Registrar in RegisDBConnector.cpp ","Failed SQLExecDirect in Registration status update");
    CloseStatementHandle(hstmt);
  }
  std::cout<< " execution succeeded\n "<<endl;
  SQLBindCol(hstmt, 1, SQL_C_LONG, &updatestat, sizeof(updatestat), NULL );
  CLog::Detail(THISMODULE,"Registrar in RegisDBConnector.cpp ","execution succeeded");
  ret  = SQLFetch(hstmt);
  std::cout << ret << endl;   
  if(ret == SQL_NO_DATA_FOUND){
    std::cout << "No sql data found "<< endl;
  }
  CloseStatementHandle(hstmt);
  return updatestat;
}

//std::string  Registrar::getdomainid(char *passstr)
std::string  DB_Connection::getFirstColumn_StringValue(char *passstr)
{
  std::cout<< "\nInside getFirstColumn_StringValue\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  UCHAR errmsg[50]= "";

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return passwd;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return passwd;
  }
  std::cout<< " execution succeeded"<<endl;
  SQLBindCol(hstmt,1,SQL_CHAR , &passwd, sizeof(passwd), NULL );
  ret = SQLFetch(hstmt);
  CLog::Detail(THISMODULE,"field = %s", passwd);
  if(ret == SQL_NO_DATA_FOUND){
    std::cout << "No sql data found "<< endl;
  }
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return passwd;
}

#ifdef XGATE_HAS_BUSINESS_SUPPORT
//Note: CloseStatementHandle(hstmt) should be called by caller function
bool DB_Connection::execute_query(const char *queryStr, HSTMT &hstmt)
{
  RETCODE ret; // ODC return code
  SQLCHAR SQLStmt[1024];
  UCHAR errmsg[50]= "";

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS) {
    SQLError(henv, hdbc, SQL_NULL_HSTMT, NULL, NULL, errmsg, sizeof(errmsg), NULL);
    printf("SQLAllocHandle failed with sql errmsg: %s\n", errmsg);
    return false;	
  }

  strcpy((char *)SQLStmt, queryStr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS) {
    SQLError(henv, hdbc, hstmt, NULL, NULL, errmsg, sizeof(errmsg), NULL);
    printf("SQLExecDirect failed with sql errmsg: %s\n",  errmsg);
    return false;
  }
  return true;
}
#endif

std::vector<DBQueryResultData>*  DB_Connection::db_combined_sp(char *passstr)
{
  std::cout<< "\nInside db_combined_sp\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  std::vector<DBQueryResultData>*  combined_output = new vector<DBQueryResultData>;
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return combined_output;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return combined_output;
  }
  std::cout<< " execution succeeded"<<endl;

  //DBQueryResultData* temp_DBQueryResultData = new DBQueryResultData();
  //DBQueryResultData temp_DBQueryResultData;

  /****
    for (int index = 0; index < 30; index++)
    {
    SQLBindCol(hstmt,index,SQL_CHAR, &passwd, sizeof(passwd), NULL );
    ret = SQLFetch(hstmt);
    if(ret != SQL_NO_DATA_FOUND)
    {
    CLog::Detail(THISMODULE,"field = %s", passwd);
    temp_DBQueryResultData.data_type = true;
    safe_strcpy(temp_DBQueryResultData.data.string_data,passwd, MAXAPPSTRLEN);
    combined_output.push_back(temp_DBQueryResultData); 			
    }
    }
    return combined_output;
   ***/

  SQLBindCol(hstmt,1,SQL_CHAR, &passwd, sizeof(passwd), NULL );
  ret = SQLFetch(hstmt);
  for (int index = 1; index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_INITIAL; index++)
  {
    switch (index)
    {
      case 1: // domain_id
      case 14: // direct_call_pin
      case 18: // blocked_no_ivr
      case 25: // user_greeting_file
      case 27: // ddi_user_hold_music
      case 28: // cli_user_hold_music
      case 30: // user_after_greeting_file
      case 32: // connecting_ivr
      case 34: // audio_connecting_file
      case 40: // audio_connecting_file
      case 41: // audio_connecting_file
        //CLog::Detail(THISMODULE,"index == %d", index);
        ret = SQLGetData(hstmt,index, SQL_CHAR,&passwd, sizeof(passwd), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"string_field[%d] = %s", index, passwd);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = true; // true == string exists

          if (strlen(passwd) <= 0)
          {
            temp_DBQueryResultData.data_type = false; // false == No string
          }
          temp_DBQueryResultData.data.string_data = new string(passwd);
          //safe_strcpy(temp_DBQueryResultData.data.string_data,passwd, MAXAPPSTRLEN);
          (*temp_DBQueryResultData.data.string_data).assign(passwd);
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        else
          CLog::Detail(THISMODULE,"string_field[%d] = %s",index,"Data not received!!!!!");
        break;
      case 2:	// is_swith_board
      case 4:	// swb_domainid
      case 6:	// isHuntGroupCall
      case 8:	// swb_domainid
      case 12:	// is_direct_call_status
      case 13:	// is_direct_call_pin_required
      case 15:	// legA_deskphone_Status
      case 16:	// legB_deskphone_Status
      case 17:	// block_type
      case 19: // is_blocked_number
      case 29: // company_hour
      case 33: // audio_connecting_Type
      case 35:	// ddi_user_hold_music_type
      case 36:	// cli_user_hold_music_type
        //CLog::Detail(THISMODULE,"index == %d", index);
        ret = SQLGetData(hstmt,index, SQL_C_LONG,&ret_int, sizeof(ret_int), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"integer_field[%d] = %d", index, ret_int);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = false;
          temp_DBQueryResultData.data.integer_data = ret_int;
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        break;
    }
  }
  if(ret == SQL_NO_DATA_FOUND)
    std::cout << "No sql data found "<< endl;
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return combined_output;
}

std::vector<DBQueryResultData>*  DB_Connection::db_combined_sp_swb(char *passstr)
{
  std::cout<< "\nInside db_combined_sp_swb\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  std::vector<DBQueryResultData>*  combined_output = new vector<DBQueryResultData>;
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return combined_output;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return combined_output;
  }
  std::cout<< " execution succeeded"<<endl;

  //DBQueryResultData* temp_DBQueryResultData = new DBQueryResultData();
  //DBQueryResultData temp_DBQueryResultData;

  /****
    for (int index = 0; index < 30; index++)
    {
    SQLBindCol(hstmt,index,SQL_CHAR, &passwd, sizeof(passwd), NULL );
    ret = SQLFetch(hstmt);
    if(ret != SQL_NO_DATA_FOUND)
    {
    CLog::Detail(THISMODULE,"field = %s", passwd);
    temp_DBQueryResultData.data_type = true;
    safe_strcpy(temp_DBQueryResultData.data.string_data,passwd, MAXAPPSTRLEN);
    combined_output.push_back(temp_DBQueryResultData); 			
    }
    }
    return combined_output;
   ***/

  SQLBindCol(hstmt,1,SQL_CHAR, &passwd, sizeof(passwd), NULL );
  ret = SQLFetch(hstmt);
  for (int index = 1; index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_SWB; index++)
  {
    switch (index)
    {
      case 5: // comp_hours_ivr_file
      case 7: // comp_hours_ivr_file
      case 10: // comp_hours_ivr_file
      case 11: // comp_hours_ivr_file
      case 15: // comp_hours_ivr_file
      case 16: // comp_hours_ivr_file
        //CLog::Detail(THISMODULE,"index == %d", index);
        ret = SQLGetData(hstmt,index, SQL_CHAR,&passwd, sizeof(passwd), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"string_field[%d] = %s", index, passwd);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = true; // true == string exists

          if (strlen(passwd) <= 0)
          {
            temp_DBQueryResultData.data_type = false; // false == No string
          }
          temp_DBQueryResultData.data.string_data = new string(passwd);
          //safe_strcpy(temp_DBQueryResultData.data.string_data,passwd, MAXAPPSTRLEN);
          (*temp_DBQueryResultData.data.string_data).assign(passwd);
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        else
          CLog::Detail(THISMODULE,"string_field[%d] = %s",index,"Data not received!!!!!");
        break;
      case 2:	// company_hour
      case 4:	// comp_hours_ivr_type
      case 6:	// comp_hours_ivr_type
      case 8:	// Greeting_Play_Type
      case 9:	// Greeting_Play_Type
      case 12:	// Greeting_Play_Type
      case 13:	// Greeting_Play_Type
      case 14:	// Greeting_Play_Type
        //CLog::Detail(THISMODULE,"index == %d", index);
        ret = SQLGetData(hstmt,index, SQL_C_LONG,&ret_int, sizeof(ret_int), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"integer_field[%d] = %d", index, ret_int);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = false;
          temp_DBQueryResultData.data.integer_data = ret_int;
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        break;
    }
  }
  if(ret == SQL_NO_DATA_FOUND)
    std::cout << "No sql data found "<< endl;
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return combined_output;
}

std::vector<DBQueryResultData>*  DB_Connection::db_combined_sp_callForward(char *passstr)
{
  std::cout<< "\nInside db_combined_sp_callForward\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  std::vector<DBQueryResultData>*  combined_output = new vector<DBQueryResultData>;
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return combined_output;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return combined_output;
  }
  std::cout<< " execution succeeded"<<endl;

  SQLBindCol(hstmt,1,SQL_CHAR, &passwd, sizeof(passwd), NULL );
  ret = SQLFetch(hstmt);
  for (int index = 1; index <= 9; index++)
  {
    switch (index)
    {
      case 4: // forward_to
        //CLog::Detail(THISMODULE,"index == %d", index);
        ret = SQLGetData(hstmt,index, SQL_CHAR,&passwd, sizeof(passwd), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"string_field[%d] = %s", index, passwd);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = true; // true == string exists

          if (strlen(passwd) <= 0)
          {
            temp_DBQueryResultData.data_type = false; // false == No string
          }
          temp_DBQueryResultData.data.string_data = new string(passwd);
          //safe_strcpy(temp_DBQueryResultData.data.string_data,passwd, MAXAPPSTRLEN);
          (*temp_DBQueryResultData.data.string_data).assign(passwd);
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        else
          CLog::Detail(THISMODULE,"string_field[%d] = %s",index,"Data not received!!!!!");
        break;
      case 1:	// incoming_call_type
        //CLog::Detail(THISMODULE,"index == %d", index);
        ret = SQLGetData(hstmt,index, SQL_C_LONG,&ret_int, sizeof(ret_int), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"integer_field[%d] = %d", index, ret_int);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = false;
          temp_DBQueryResultData.data.integer_data = ret_int;
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        break;
    }
  }
  if(ret == SQL_NO_DATA_FOUND)
    std::cout << "No sql data found "<< endl;
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return combined_output;
}

std::vector<DBQueryResultData>*  DB_Connection::db_combined_sp_conf_info(char *passstr)
{
  std::cout<< "\nInside db_combined_sp_conf_info\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  std::vector<DBQueryResultData>*  combined_output = new vector<DBQueryResultData>;
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return combined_output;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return combined_output;
  }
  std::cout<< " execution succeeded"<<endl;

  SQLBindCol(hstmt,1,SQL_CHAR, &passwd, sizeof(passwd), NULL );
  ret = SQLFetch(hstmt);
  for (int index = 1; index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_CONF_INFO; index++)
  {
    switch (index)
    {
      case 3: // conference_status
      case 22: // host_pin
      case 23: // participant_pin
        //CLog::Detail(THISMODULE,"index == %d", index);
        ret = SQLGetData(hstmt,index, SQL_CHAR,&passwd, sizeof(passwd), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"string_field[%d] = %s", index, passwd);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = true; // true == string exists

          if (strlen(passwd) <= 0)
          {
            temp_DBQueryResultData.data_type = false; // false == No string
          }
          temp_DBQueryResultData.data.string_data = new string(passwd);
          (*temp_DBQueryResultData.data.string_data).assign(passwd);
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        else
          CLog::Detail(THISMODULE,"string_field[%d] = %s",index,"Data not received!!!!!");
        break;
      case 7:	// status_flag
        //CLog::Detail(THISMODULE,"index == %d", index);
        ret = SQLGetData(hstmt,index, SQL_C_LONG,&ret_int, sizeof(ret_int), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"integer_field[%d] = %d", index, ret_int);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = false;
          temp_DBQueryResultData.data.integer_data = ret_int;
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        break;
    }
  }
  if(ret == SQL_NO_DATA_FOUND)
    std::cout << "No sql data found "<< endl;
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return combined_output;
}

std::vector<DBQueryResultData>*  DB_Connection::db_combined_sp_conf_validate_pin(char *passstr)
{
  std::cout<< "\nInside db_combined_sp_conf_validate_pin\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  std::vector<DBQueryResultData>*  combined_output = new vector<DBQueryResultData>;
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return combined_output;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return combined_output;
  }
  std::cout<< " execution succeeded"<<endl;

  SQLBindCol(hstmt,1,SQL_CHAR, &passwd, sizeof(passwd), NULL );
  ret = SQLFetch(hstmt);
  for (int index = 1; index <= 4; index++)
  {
    switch (index)
    {
      case 2: // extension
        ret = SQLGetData(hstmt,index, SQL_CHAR,&passwd, sizeof(passwd), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"string_field[%d] = %s", index, passwd);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = true; // true == string exists

          if (strlen(passwd) <= 0)
          {
            temp_DBQueryResultData.data_type = false; // false == No string
          }
          temp_DBQueryResultData.data.string_data = new string(passwd);
          (*temp_DBQueryResultData.data.string_data).assign(passwd);
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        else
          CLog::Detail(THISMODULE,"string_field[%d] = %s",index,"Data not received!!!!!");
        break;

      case 1:	// conf_user_type
        ret = SQLGetData(hstmt,index, SQL_C_LONG,&ret_int, sizeof(ret_int), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"integer_field[%d] = %d", index, ret_int);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = false;
          temp_DBQueryResultData.data.integer_data = ret_int;
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        break;
    }
  }
  if(ret == SQL_NO_DATA_FOUND)
    std::cout << "No sql data found "<< endl;
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return combined_output;
}

std::vector<DBQueryResultData>*  DB_Connection::db_combined_sp_get_pstn_operator_details(char *passstr)
{
  std::cout<< "\nInside db_combined_sp_get_pstn_operator_details\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  std::vector<DBQueryResultData>*  combined_output = new vector<DBQueryResultData>;
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return combined_output;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return combined_output;
  }
  std::cout<< " execution succeeded"<<endl;

  SQLBindCol(hstmt,1,SQL_CHAR, &passwd, sizeof(passwd), NULL );
  ret = SQLFetch(hstmt);
  for (int index = 1; index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_GETOPERATORDETAILS; index++)
  {
    CLog::Detail(THISMODULE,"string_field[1],",passwd);
    switch (index)
    {
      case 1: // operator_name
      case 2: // codec
      case 3: // vtcode
        ret = SQLGetData(hstmt,index, SQL_CHAR,&passwd, sizeof(passwd), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"string_field[%d] = %s", index, passwd);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = true; // true == string exists

          if (strlen(passwd) <= 0)
          {
            temp_DBQueryResultData.data_type = false; // false == No string
          }
          temp_DBQueryResultData.data.string_data = new string(passwd);
          (*temp_DBQueryResultData.data.string_data).assign(passwd);
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        else
          CLog::Detail(THISMODULE,"string_field[%d] = %s",index,"Data not received!!!!!");
        break;
      default:
        break;
    }
  }
  if(ret == SQL_NO_DATA_FOUND)
    std::cout << "No sql data found "<< endl;
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return combined_output;
}

std::vector<DBQueryResultData>*  DB_Connection::db_combined_sp_conf_get_callid_mguid(char *passstr)
{
  std::cout<< "\nInside db_combined_sp_conf_get_callid_mguid\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  std::vector<DBQueryResultData>*  combined_output = new vector<DBQueryResultData>;
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return combined_output;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return combined_output;
  }
  std::cout<< " execution succeeded"<<endl;

  int DbRecordNumber = 1;
  int max_records = 1;
  for ( ; (DbRecordNumber <= max_records) && (DbRecordNumber < 10); DbRecordNumber++)
  {
    ret = SQLFetch(hstmt);
    for (int index = 1; index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_CONF_REGISTERINFO; index++)
    {
      switch (index)
      {
        case 1: // call_id
        case 2: // conf_status  
        case 7: // conf_mg_uid
          ret = SQLGetData(hstmt,index, SQL_CHAR,&passwd, sizeof(passwd), NULL );
          if(ret != SQL_NO_DATA_FOUND)
          {
            CLog::Detail(THISMODULE,"string_field[%d] = %s", index, passwd);
            DBQueryResultData temp_DBQueryResultData;
            temp_DBQueryResultData.data_type = true; // true == string exists

            if (strlen(passwd) <= 0)
            {
              temp_DBQueryResultData.data_type = false; // false == No string
            }
            temp_DBQueryResultData.data.string_data = new string(passwd);
            (*temp_DBQueryResultData.data.string_data).assign(passwd);
            combined_output->push_back(temp_DBQueryResultData); 			
          }
          else
            CLog::Detail(THISMODULE,"string_field[%d] = %s",index,"Data not received!!!!!");
          break;

        case 6:	// participant_type
          ret = SQLGetData(hstmt,index, SQL_C_LONG,&ret_int, sizeof(ret_int), NULL );
          if(ret != SQL_NO_DATA_FOUND)
          {
            CLog::Detail(THISMODULE,"integer_field[%d] = %d", index, ret_int);
            DBQueryResultData temp_DBQueryResultData;
            temp_DBQueryResultData.data_type = false;
            temp_DBQueryResultData.data.integer_data = ret_int;
            combined_output->push_back(temp_DBQueryResultData); 			
          }
          break;
        case 8:	// err_code
          ret = SQLGetData(hstmt,index, SQL_C_LONG,&ret_int, sizeof(ret_int), NULL );
          if (ret_int < 2500)
            max_records = ret_int;
          if(ret != SQL_NO_DATA_FOUND)
          {
            DBQueryResultData temp_DBQueryResultData;
            temp_DBQueryResultData.data_type = false;
            temp_DBQueryResultData.data.integer_data = max_records;
            combined_output->push_back(temp_DBQueryResultData); 			
          }
          break;
      }
    }
  }
  if(ret == SQL_NO_DATA_FOUND)
    std::cout << "No sql data found "<< endl;
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return combined_output;
}

std::vector<DBQueryResultData>*  DB_Connection::db_combined_sp_conf_id(char *passstr)
{
  std::cout<< "\nInside db_combined_sp_conf_id\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  std::vector<DBQueryResultData>*  combined_output = new vector<DBQueryResultData>;
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return combined_output;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return combined_output;
  }
  std::cout<< " execution succeeded"<<endl;

  SQLBindCol(hstmt,1,SQL_CHAR, &passwd, sizeof(passwd), NULL );
  ret = SQLFetch(hstmt);
  for (int index = 1; index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_CONF_INFO; index++)
  {
    switch (index)
    {
      case 3: // host_pin
      case 4: // participant_pin
        //CLog::Detail(THISMODULE,"index == %d", index);
        ret = SQLGetData(hstmt,index, SQL_CHAR,&passwd, sizeof(passwd), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"string_field[%d] = %s", index, passwd);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = true; // true == string exists

          if (strlen(passwd) <= 0)
          {
            temp_DBQueryResultData.data_type = false; // false == No string
          }
          temp_DBQueryResultData.data.string_data = new string(passwd);
          (*temp_DBQueryResultData.data.string_data).assign(passwd);
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        else
          CLog::Detail(THISMODULE,"string_field[%d] = %s",index,"Data not received!!!!!");
        break;
      case 1:	// err_code
        //CLog::Detail(THISMODULE,"index == %d", index);
        ret = SQLGetData(hstmt,index, SQL_C_LONG,&ret_int, sizeof(ret_int), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"integer_field[%d] = %d", index, ret_int);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = false;
          temp_DBQueryResultData.data.integer_data = ret_int;
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        break;
    }
  }
  if(ret == SQL_NO_DATA_FOUND)
    std::cout << "No sql data found "<< endl;
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return combined_output;
}

std::vector<DBQueryResultData>*  DB_Connection::db_combined_sp_direct_number(char *passstr)
{
  std::cout<< "\nInside db_combined_sp_direct_number\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  std::vector<DBQueryResultData>*  combined_output = new vector<DBQueryResultData>;
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return combined_output;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return combined_output;
  }
  std::cout<< " execution succeeded"<<endl;

  SQLBindCol(hstmt,1,SQL_CHAR, &passwd, sizeof(passwd), NULL );
  ret = SQLFetch(hstmt);
  for (int index = 1; index <= 8; index++)
  {
    switch (index)
    {
      case 1: // mobile_no
      case 8: // domain_name
        ret = SQLGetData(hstmt,index, SQL_CHAR,&passwd, sizeof(passwd), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"string_field[%d] = %s", index, passwd);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = true; // true == string exists

          if (strlen(passwd) <= 0)
          {
            temp_DBQueryResultData.data_type = false; // false == No string
          }
          temp_DBQueryResultData.data.string_data = new string(passwd);
          (*temp_DBQueryResultData.data.string_data).assign(passwd);
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        else
          CLog::Detail(THISMODULE,"string_field[%d] = %s",index,"Data not received!!!!!");
        break;
      default:
        break;
    }
  }
  if(ret == SQL_NO_DATA_FOUND)
    std::cout << "No sql data found "<< endl;
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return combined_output;
}

std::vector<DBQueryResultData>*  DB_Connection::db_combined_sp_TRUNKOUT_PORTOUT(char *passstr)
{
  std::cout<< "\nInside db_combined_sp_TRUNKOUT_PORTOUT\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  std::vector<DBQueryResultData>*  combined_output = new vector<DBQueryResultData>;
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return combined_output;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return combined_output;
  }
  std::cout<< " execution succeeded"<<endl;

  SQLBindCol(hstmt,1,SQL_CHAR, &passwd, sizeof(passwd), NULL );
  ret = SQLFetch(hstmt);
  for (int index = 1; index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_SELECTOR; index++)
  {
    switch (index)
    {
      case 7: // user_greeting_file
      case 8: // ddi_user_hold_music
        //CLog::Detail(THISMODULE,"index == %d", index);
        ret = SQLGetData(hstmt,index, SQL_CHAR,&passwd, sizeof(passwd), NULL );
        if(ret != SQL_NO_DATA_FOUND)
        {
          CLog::Detail(THISMODULE,"string_field[%d] = %s", index, passwd);
          DBQueryResultData temp_DBQueryResultData;
          temp_DBQueryResultData.data_type = true; // true == string exists

          if (strlen(passwd) <= 0)
          {
            temp_DBQueryResultData.data_type = false; // false == No string
          }
          temp_DBQueryResultData.data.string_data = new string(passwd);
          //safe_strcpy(temp_DBQueryResultData.data.string_data,passwd, MAXAPPSTRLEN);
          (*temp_DBQueryResultData.data.string_data).assign(passwd);
          combined_output->push_back(temp_DBQueryResultData); 			
        }
        else
          CLog::Detail(THISMODULE,"string_field[%d] = %s",index,"Data not received!!!!!");
        break;
    }
  }
  if(ret == SQL_NO_DATA_FOUND)
    std::cout << "No sql data found "<< endl;
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return combined_output;
}

std::vector<DBQueryResultData>*  DB_Connection::db_combined_sp_SendAppNotificationsAll(char *passstr)
{
  std::cout<< "\nInside db_combined_sp_SendAppNotificationsAll\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  std::vector<DBQueryResultData>*  combined_output = new vector<DBQueryResultData>;
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return combined_output;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return combined_output;
  }
  std::cout<< " execution succeeded"<<endl;

  //   	ret = SQLFetch(hstmt);
  int DbRecordNumber = 1;
  int max_records = 1;
  //for ( ; (ret = SQLFetch(hstmt)) == SQL_SUCCESS; DbRecordNumber++)
  for ( ; (DbRecordNumber <= max_records) && (DbRecordNumber < 10); DbRecordNumber++)
  {
    //SQLBindCol(hstmt,DbRecordNumber,SQL_CHAR, &passwd, sizeof(passwd), NULL );
    //ret = SQLFetch(hstmt);
    ret = SQLFetch(hstmt);
    for (int index = 1; index <= TOTAL_NOOFCOLUMNS_DB_COMBINEDSP_REGISTRAR; index++)
    {
      switch (index)
      {
        case 1: // Contact_address
        case 2: // proxy_username
        case 4: // device_type
        case 6: // prev_proxy_username
        case 7: // route_ip_address
          //CLog::Detail(THISMODULE,"index == %d", index);
          ret = SQLGetData(hstmt,index, SQL_CHAR,&passwd, sizeof(passwd), NULL );
          if(ret != SQL_NO_DATA_FOUND)
          {
            CLog::Detail(THISMODULE,"string_field[%d] = %s", index, passwd);
            DBQueryResultData temp_DBQueryResultData;
            temp_DBQueryResultData.data_type = true; // true == string exists

            if (strlen(passwd) <= 0)
            {
              temp_DBQueryResultData.data_type = false; // false == No string
            }
            temp_DBQueryResultData.data.string_data = new string(passwd);
            //safe_strcpy(temp_DBQueryResultData.data.string_data,passwd, MAXAPPSTRLEN);
            (*temp_DBQueryResultData.data.string_data).assign(passwd);
            combined_output->push_back(temp_DBQueryResultData); 			
          }
          else
          {
            CLog::Detail(THISMODULE,"string_field[%d] = %s",index,"Data not received!!!!!");

            DBQueryResultData temp_DBQueryResultData;
            temp_DBQueryResultData.data_type = true; // true == string exists

            std::string temp_string;
            temp_string = get_Nth_Column_StringValue_Recordwise(index, passstr, DbRecordNumber);

            if (strlen(temp_string.c_str()) <= 0)
            {
              temp_DBQueryResultData.data_type = false; // false == No string
            }
            temp_DBQueryResultData.data.string_data = new string(passwd);
            //safe_strcpy(temp_DBQueryResultData.data.string_data,passwd, MAXAPPSTRLEN);
            (*temp_DBQueryResultData.data.string_data).assign(temp_string.c_str());
            combined_output->push_back(temp_DBQueryResultData); 			
          }
          break;
        case 3:	// status
          //CLog::Detail(THISMODULE,"index == %d", index);
          ret = SQLGetData(hstmt,index, SQL_C_LONG,&ret_int, sizeof(ret_int), NULL );
          if(ret != SQL_NO_DATA_FOUND)
          {
            CLog::Detail(THISMODULE,"integer_field[%d] = %d", index, ret_int);
            DBQueryResultData temp_DBQueryResultData;
            temp_DBQueryResultData.data_type = false;
            temp_DBQueryResultData.data.integer_data = ret_int;
            combined_output->push_back(temp_DBQueryResultData); 			
          }
          break;
        case 5:	// rec_count
          ret = SQLGetData(hstmt,index, SQL_C_LONG,&ret_int, sizeof(ret_int), NULL );
          max_records = ret_int;
          break;
      }
    }
  }
  if(ret == SQL_NO_DATA_FOUND)
    std::cout << "No sql data found "<< endl;
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return combined_output;
}

std::vector<DBQueryResultData>*  DB_Connection::db_callhandling_sp(char *passstr)
{
  std::cout<< "\nInside db_callhandling_sp\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  std::vector<DBQueryResultData>*  combined_output = new vector<DBQueryResultData>;
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return combined_output;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return combined_output;
  }
  std::cout<< " execution succeeded"<<endl;

  //   	ret = SQLFetch(hstmt);
  int DbRecordNumber = 1;
  int max_records = 1;
  //for ( ; (ret = SQLFetch(hstmt)) == SQL_SUCCESS; DbRecordNumber++)
  for ( ; (DbRecordNumber <= max_records) && (DbRecordNumber < 10); DbRecordNumber++)
  {
    //SQLBindCol(hstmt,DbRecordNumber,SQL_CHAR, &passwd, sizeof(passwd), NULL );
    //ret = SQLFetch(hstmt);
    ret = SQLFetch(hstmt);
    for (int index = 1; index <= (TOTAL_NOOFCOLUMNS_DB_CALLHANDLING); index++)
    {
      switch (index)
      {
        case 3: // device_type
        case 5: // ring_type_info
          //CLog::Detail(THISMODULE,"index == %d", index);
          ret = SQLGetData(hstmt,index, SQL_CHAR,&passwd, sizeof(passwd), NULL );
          if(ret != SQL_NO_DATA_FOUND)
          {
            CLog::Detail(THISMODULE,"string_field[%d] = %s", index, passwd);
            DBQueryResultData temp_DBQueryResultData;
            temp_DBQueryResultData.data_type = true; // true == string exists

            if (strlen(passwd) <= 0)
            {
              temp_DBQueryResultData.data_type = false; // false == No string
            }
            temp_DBQueryResultData.data.string_data = new string(passwd);
            //safe_strcpy(temp_DBQueryResultData.data.string_data,passwd, MAXAPPSTRLEN);
            (*temp_DBQueryResultData.data.string_data).assign(passwd);
            combined_output->push_back(temp_DBQueryResultData); 			
          }
          else
          {
            CLog::Detail(THISMODULE,"string_field[%d] = %s",index,"Data not received!!!!!");

            DBQueryResultData temp_DBQueryResultData;
            temp_DBQueryResultData.data_type = true; // true == string exists

            std::string temp_string;
            temp_string = get_Nth_Column_StringValue_Recordwise(index, passstr, DbRecordNumber);

            if (strlen(temp_string.c_str()) <= 0)
            {
              temp_DBQueryResultData.data_type = false; // false == No string
            }
            temp_DBQueryResultData.data.string_data = new string(passwd);
            //safe_strcpy(temp_DBQueryResultData.data.string_data,passwd, MAXAPPSTRLEN);
            (*temp_DBQueryResultData.data.string_data).assign(temp_string.c_str());
            combined_output->push_back(temp_DBQueryResultData); 			
          }
          break;
        case 1:	// no_of_rings
        case 2:	// sec_count
        case 4:	// ring_type
        case 6:	// rec_count
          //CLog::Detail(THISMODULE,"index == %d", index);
          ret = SQLGetData(hstmt,index, SQL_C_LONG,&ret_int, sizeof(ret_int), NULL );
          if(ret != SQL_NO_DATA_FOUND)
          {
            CLog::Detail(THISMODULE,"integer_field[%d] = %d", index, ret_int);
            DBQueryResultData temp_DBQueryResultData;
            temp_DBQueryResultData.data_type = false;
            temp_DBQueryResultData.data.integer_data = ret_int;
            if (index == 6)
              max_records = ret_int;
            combined_output->push_back(temp_DBQueryResultData); 			
          }
          break;
      }
    }
  }
  if(ret == SQL_NO_DATA_FOUND)
    std::cout << "No sql data found "<< endl;
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return combined_output;
}


std::string  DB_Connection::get_Nth_Column_StringValue(int ColumnNumber, char *passstr)
{
  std::cout<< "\nInside get_Nth_Column_StringValue\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  UCHAR errmsg[50]= "";

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return passwd;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return passwd;
  }
  std::cout<< " execution succeeded"<<endl;
  SQLBindCol(hstmt,ColumnNumber,SQL_CHAR , &passwd, sizeof(passwd), NULL );
  ret = SQLFetch(hstmt);
  CLog::Detail(THISMODULE,"field = %s", passwd);
  if(ret == SQL_NO_DATA_FOUND){
    std::cout << "No sql data found "<< endl;
  }
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return passwd;
}

std::string  DB_Connection::get_Nth_Column_StringValue_Recordwise(int ColumnNumber, char *passstr, int DbRecordNumber)
{
  std::cout<< "\nInside get_Nth_Column_StringValue_Recordwise\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  UCHAR errmsg[50]= "";

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return passwd;	
  }
  strcpy((char *)SQLStmt,passstr);
  int recordNumberIndex = 1;
  if ((ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS)) == SQL_SUCCESS)
  {
    while (recordNumberIndex <= DbRecordNumber)
    {
      SQLBindCol(hstmt,ColumnNumber,SQL_CHAR , &passwd, sizeof(passwd), NULL );
      ret = SQLFetch(hstmt);
      recordNumberIndex++;
    }
  }
  else
  {
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return passwd;
  }
  std::cout<< " execution succeeded"<<endl;
  CLog::Detail(THISMODULE,"field = %s", passwd);
  if(ret == SQL_NO_DATA_FOUND){
    std::cout << "No sql data found "<< endl;
  }
  CloseStatementHandle(hstmt);
  printf(" field from DB %s\n", passwd);
  return passwd;
}
int  DB_Connection::get_Nth_Column_IntegerValue(int ColumnNumber, char *passstr)
{
  std::cout<< "\nInside get_Nth_Column_IntegerValue\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return -1;	
  }
  strcpy((char *)SQLStmt,passstr);
  ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS);
  if(ret != SQL_SUCCESS){
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return -1;
  }
  std::cout<< " execution succeeded"<<endl;
  SQLBindCol(hstmt,ColumnNumber,SQL_C_LONG , &ret_int, sizeof(ret_int), NULL );

  ret = SQLFetch(hstmt);
  if(ret == SQL_NO_DATA_FOUND){
    std::cout << "No sql data found "<< endl;
    return ret_int;
  }
  CloseStatementHandle(hstmt);
  return ret_int;
}
int  DB_Connection::get_Nth_Column_IntegerValue_Recordwise(int ColumnNumber, char *passstr, int DbRecordNumber)
{
  std::cout<< "\nInside get_Nth_Column_IntegerValue_Recordwise\n"<<endl;
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return -1;	
  }
  strcpy((char *)SQLStmt,passstr);
  int recordNumberIndex = 1;
  if ((ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS)) == SQL_SUCCESS)
  {
    while (recordNumberIndex <= DbRecordNumber)
    {
      SQLBindCol(hstmt,ColumnNumber,SQL_C_LONG , &ret_int, sizeof(ret_int), NULL );
      ret = SQLFetch(hstmt);
      recordNumberIndex++;
    }
  }
  else
  {
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
    CloseStatementHandle(hstmt);
    return passwd;
  }
  std::cout<< " execution succeeded"<<endl;

  if(ret == SQL_NO_DATA_FOUND){
    std::cout << "No sql data found "<< endl;
    return ret_int;
  }
  CloseStatementHandle(hstmt);
  return ret_int;
}
int  DB_Connection::get_totalNoOfRegisteredUsers(char *passstr)
{
  std::cout<< "\nInside get_totalNoOfRegisteredUsers\n"<<endl;
  //CLog::Detail(THISMODULE,"DB_Connection::get_totalNoOfRegisteredUsers");
  RETCODE ret; // ODC return code
  HSTMT hstmt;
  SQLCHAR   SQLStmt[1024];
  char  passwd[50] = "";
  UCHAR errmsg[50]= "";
  SQLINTEGER ret_int;

  int total_reg_user = 0;

  ret  = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
  if(ret!= SQL_SUCCESS){
    SQLError(henv,hdbc,SQL_NULL_HSTMT,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLAllocHandle\t\t"<<errmsg;
    CloseStatementHandle(hstmt);	
    return total_reg_user;	
  }

  strcpy((char *)SQLStmt,passstr);

  //CDbTLS *tls = GetTLS(task);
  //int *nrow = ((DB_SQL_Exec*)hstmt)->GetRowsAffected();
  //CLog::Detail(THISMODULE,"No of rows affected: %d", *nrow);    

  if ((ret = SQLExecDirect(hstmt, SQLStmt, SQL_NTS)) == SQL_SUCCESS)
  {
    while ((ret = SQLFetch(hstmt)) == SQL_SUCCESS)
      total_reg_user++;
  }
  else
  {
    SQLError(henv,hdbc,hstmt,NULL,NULL,errmsg,sizeof(errmsg),NULL);
    cout<<"\nFailed SQLExecDirect in getting user domain\t\t"<<errmsg;
  }
  CloseStatementHandle(hstmt);
  return total_reg_user;
}

DB_Connection::DB_Connection(char* db_SERVER_ipaddress, char* UID, char* password, char* DATABASE, int DBConnectionSlot)
{
  CLog::Detail(THISMODULE,"Inside constructor... DB_Connection(%s ; %s ; %s ; %s)", db_SERVER_ipaddress, UID, password, DATABASE);
  if (g_totalNoOfDBConnections < MAX_TOTAL_NO_OF_DB_CONNECTIONS)
  {
    ret = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    ret = SQLSetEnvAttr( henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    ret = SQLAllocHandle( SQL_HANDLE_DBC, henv, &hdbc);
    SQLCHAR connectstr[1024+1],connectstring_out[1024+1];
    SQLSMALLINT len;
    //strcpy((char *)connectstr,(char *)"DRIVER=FreeTDS;SERVER=10.22.2.86;UID=smepbx;PWD=smeswitch;DATABASE=unifiedring;TDS_version=7.0;port=1433;");
    sprintf(connectstr,"DRIVER=FreeTDS;SERVER=%s;UID=%s;PWD=%s;DATABASE=%s;TDS_version=7.0;port=1433;",db_SERVER_ipaddress, UID, password, DATABASE);
    CLog::Detail(THISMODULE,"Inside constructor... DB_Connection(%s", connectstr);
    ret = ::SQLDriverConnect(
        hdbc,
        NULL,
        connectstr,
        SQL_NTS,
        connectstring_out,
        sizeof(connectstring_out),
        &len,
        SQL_DRIVER_NOPROMPT);

    if (ret != SQL_SUCCESS){
      //	std::cout<< " DB connection  Succeeded"<<endl;
      SQLDisconnect(hdbc);
      SQLFreeHandle( SQL_HANDLE_DBC, hdbc);
      SQLFreeHandle( SQL_HANDLE_ENV, henv);
    }
    else{
std:cout<< " Connection establishment Success" << endl;
    hdbc_array[g_totalNoOfDBConnections] = hdbc;					
    hdbc_array_2.push_back(hdbc);					
    }
    g_totalNoOfDBConnections++;
  }
  else
  { // Else need to use existing DB-connections...
    hdbc = hdbc_array[DBConnectionSlot - 1];
  }
}
/*
 *
 */
//------------------disconnection------------------------------
DB_Connection :: ~DB_Connection()
{
  std::cout<< "Inside DB Disconnection"<<endl;
  CLog::Detail(THISMODULE,"DB_Connection:: DESTRUCTOR  ~DB_Connection()");
  SQLDisconnect(hdbc);
  SQLFreeHandle( SQL_HANDLE_DBC, hdbc);
  SQLFreeHandle( SQL_HANDLE_ENV, henv);
  g_totalNoOfDBConnections--;
}

