#include "xGateDBManager.h"
#include "xGateMgMsg.h"
#include "log.h"
#include "klog.h"
#include<algorithm>
#include "xGateSipSignalProcessor.h"
#include "xGatePBXBusiness.h"
#include "xGateDbmMsg.h"
//#include "xGateBusinessProcessor.h"
#include "xGateDbmDispatcher.h"
//#include "xGateBusinessUtil.h"

#define THISMODULE "DBManager"
#define UNIFIEDRING_DB_SERVER_IP_ADDRESS        "10.22.2.86"
#define UNIFIEDRING_DB_UID                      "smepbx"
#define UNIFIEDRING_DB_PASSWORD                 "smeswitch"
#define UNIFIEDRING_DB_DATABASE                 "unifiedring"

#define SELECTOR_DB_SERVER_IP_ADDRESS   "10.22.2.182"
#define SELECTOR_DB_UID                 "agent"
#define SELECTOR_DB_PASSWORD            "agentsql"
#define SELECTOR_DB_DATABASE            "selector"

#define REGISTRAR_DB_SERVER_IP_ADDRESS  "10.22.2.85"
#define REGISTRAR_DB_UID                "mgc"
#define REGISTRAR_DB_PASSWORD           "mgcsql"
#define REGISTRAR_DB_DATABASE           "xgregistar"

#define UNIFIEDRING	1
#define SELECTOR	2
#define	REGISTRAR	3

#define WAV_FILE_PATH                   "/root/wav_files/sounds/vectone/"
#define CALL_FORWARDED_ANNOUNCEMENT_FILE        "call_has_been_forwarded.wav"
#define RBT_DEFAULT_FILE                        "UK_ringback_tone-0001.wav"
#define RBT_FILE_ONE_RING                       "UK_ringback_tone_one_ring.wav"
#define SILENCE_500MS_FILE                      "500ms_silence.wav"
#define CONNECT_TO_OPERATOR_FILE                "please_wait_connect_operator.wav"
#define NO_INPUT_GOODBYE_FILE                   "no_input_goodbye.wav"

#define PBXSHARE_PATH           "/pbxshare/"
#define UG_PATH                 "/UG/"
#define CH_PATH                 "/CH/"

#define DEVICE_TYPE_URDESK              "URDESK:"
#define DEVICE_TYPE_URAPP_ANDROID       "URAPP:AND"
#define DEVICE_TYPE_URAPP_IOS           "URAPP:IOS"

#define CONTACT_ADDRESS_DEFAULT         "10.22.5.39:5060"

#define         DEFAULT_SWB_SLEEPTIMEOUT_MAINLOOP       40 // seconds
#define         SWB_IVR_WELCOME_VECTONE                 "SB_welcome_vectone.wav"
#define         MAINMENU_IVR_REPEAT_COUNTER             1
static int transId_len =5;


xGateDBManager::xGateDBManager() : xGateProcessor("DBManager"), m_uid("")
{
  CLog::Error(THISMODULE, "xGateDBManager::xGateDBManager DB - Manager Started");
  regobj= new Registrar;

  m_run = true;
  db_connection_pbx = new  DB_Connection(UNIFIEDRING_DB_SERVER_IP_ADDRESS, UNIFIEDRING_DB_UID, UNIFIEDRING_DB_PASSWORD, UNIFIEDRING_DB_DATABASE, 1);
  db_connection = db_connection_pbx;
  db_connection_selector = new  DB_Connection(SELECTOR_DB_SERVER_IP_ADDRESS, SELECTOR_DB_UID, SELECTOR_DB_PASSWORD, SELECTOR_DB_DATABASE, 2);
  db_connection_registrar = new  DB_Connection(REGISTRAR_DB_SERVER_IP_ADDRESS, REGISTRAR_DB_UID, REGISTRAR_DB_PASSWORD, REGISTRAR_DB_DATABASE, 3);
}

xGateDBManager::~xGateDBManager(){

}

int xGateDBManager::svc( void ){

  ACE_Message_Block *pAmb = 0;
  while(m_run)
  {
    ACE_Time_Value tv ((time(NULL) + 20));

    // Get message from the queue
    int result = getq(pAmb, &tv);
    if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
    {
      continue;
    }

    // check the result
    if ((result != -1))
    {
      ///Process and Dispatch Message
      process_msg(pAmb);
    }
    else
    {
      // This is the case where the message queue is de-activated.
      // Need to stop the thread
      CLog::Error(THISMODULE, "xGateDBManager::svc(%s) Error in Message Queue", "DBManager");
      m_run = false;
    }

    // delete the message received
    if(pAmb)
    {
      delete pAmb;
      pAmb = NULL;
    }
  }

  return 0;
}

xGateRetVal xGateDBManager::process_msg(ACE_Message_Block *pAmb)
{
  xGateRetVal retval = EN_XGATE_STATUS_SUCCESS;
  if(pAmb) {
    CLog::Info(THISMODULE,"xGateDBManager::process_msg(%s)", "Received Msg");
    xGateBaseMsg *pBaseMsg = NULL;
    pBaseMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
    switch(pBaseMsg->getMsgType()) {
      case EN_XGATE_MSG_SIPREG_IN:
        {
          CLog::Detail(THISMODULE, "DBManager::process_msg processing EN_XGATE_MSG_SIPREG_IN");
          xGateSipMsgBlock *pSipMsg = dynamic_cast<xGateSipMsgBlock*>(pAmb);
          if(pSipMsg){
            std::cout<< pSipMsg->m_sipregobj->domainname.c_str()<<endl;

            CLog::Detail(THISMODULE,"xGateDBManager::process_msg(%d)",pSipMsg->m_sipregobj->Regevent);
            handle_regis_process(pSipMsg->m_sipregobj);
          }
          break;
        }

      case EN_XGATE_MSG_DBCONNECT_IN:
        {
          CLog::Detail(THISMODULE, "xGateDBManager::process_msg processing EN_XGATE_MSG_DBCONNECT_IN");
          xGateSipMsgBlock *pSipMsg = dynamic_cast<xGateSipMsgBlock*>(pAmb);
          if(pSipMsg){
            handle_db_process(pSipMsg->m_dbObj);
          }
          break;
        }
#ifdef XGATE_HAS_BUSINESS_SUPPORT
      case EN_XGATE_MSG_FROM_BUSINESS:
        {
          CLog::Detail(THISMODULE, "xGateDBManager processing EN_XGATE_MSG_FROM_BUSINESS");
          xGateBusinessMsg *pBusinessMsg = dynamic_cast<xGateBusinessMsg*>(pAmb);
          if(pBusinessMsg) {
            process_business_request(pBusinessMsg);
          }
          break;
        }
#endif
      case EN_XGATE_MSG_FROM_DBM:
        {
          CLog::Detail(THISMODULE, "xGateDBManager::process_msg processing EN_XGATE_MSG_FROM_DBM");
          xGateDbmMsg *pDbmMsg = dynamic_cast<xGateDbmMsg*>(pAmb);
          if(pDbmMsg) {
#if 0
            xGateDBObj *dbObj = NULL;
            DbmReqDetail &reqDetail = pDbmMsg->get_dbm_request_detail();
            dbObj = GetDbObjInfo(reqDetail.m_transId);
            if(dbObj != NULL)
            {
              handle_regdb_process(dbObj,reqDetail.m_output);
            }
            if(dbObj != NULL)
              DeleteDbObjInfo(dbObj);
#else //TODO: Yoga, added for business layer development
            DbmReqDetail &reqDetail = pDbmMsg->get_dbm_request_detail();
            handle_db_response(reqDetail);
#endif
          }
          break;
        }


    }
  }
  return retval;
}

xGateRetVal xGateDBManager::stop(){
  m_run = false;
  msg_queue()->deactivate();
  return EN_XGATE_STATUS_SUCCESS;
}

xGateRetVal xGateDBManager::handle_regis_process(xGateSofiaSipRegobj *pSipMsg)
{
  CLog::Detail(THISMODULE,"Enter xGateDBManager::handle_registration_process");
  int result;
  char str[1024];
  char pass_str[1024];
  switch(pSipMsg->Regevent)
  {
    case EN_VALIDATE:
      {
        CLog::Detail(THISMODULE, "xGateDBManager::process_msg regevent EN_VALIDATE");
        sprintf(str,"Exec sip_authenticate_user_registration '%s','%s','%s','%s'",
            pSipMsg->callid.c_str(),
            pSipMsg->domainname.c_str(),
            pSipMsg->contactaddress.c_str(),
            pSipMsg->username.c_str());
        result= regobj->CheckAuthReq(str);
        std::cout<<str<<endl;
        if(result !=0) {
          pSipMsg->Regevent = EN_SEND401;
        } else {
          if(pSipMsg->expries > 0) {
            pSipMsg->Regevent = EN_SEND401;// This needs to be reworked
          } else {
            pSipMsg->Regevent = EN_SEND401;
          }
        }
        break;
      }
    case EN_GETUSRPWD:
      {
        sprintf(pass_str,"Exec sip_get_user_password '%s','%s','%s','%s','%s'",
            pSipMsg->mobileno.c_str(),
            pSipMsg->domainname.c_str(),
            pSipMsg->devicetype.c_str(),
            pSipMsg->deviceid.c_str(),
            pSipMsg->username.c_str());
        pSipMsg->password= regobj->getuserdetails(pass_str);
        CLog::Detail(THISMODULE,"Query %s Password %s"	 ,pass_str,pSipMsg->password.c_str());
        if(pSipMsg->expries > 0) {
          pSipMsg->Regevent = EN_REGISTER;
        } else {
          pSipMsg->Regevent = EN_DEREGISTER;
        }
        break;
      }
    case EN_AUTHCALL:
      {
        sprintf(pass_str,"Exec sip_get_user_password '%s','%s','%s','%s','%s'",
            pSipMsg->mobileno.c_str(),
            pSipMsg->domainname.c_str(),
            pSipMsg->devicetype.c_str(),
            pSipMsg->deviceid.c_str(),
            pSipMsg->username.c_str());
        pSipMsg->password= regobj->getuserdetails(pass_str);
        CLog::Detail(THISMODULE,"Query %s Password %s"  ,pass_str,pSipMsg->password.c_str());
        pSipMsg->Regevent = EN_AUTHCALL;
        break;
      }
    case EN_REGISTER:
      {
        CLog::Detail(THISMODULE, "xGateDBManager::process_msg regevent EN_REGISTER");
        sprintf(str,"Exec sip_do_user_registration '%s','%s','%s','%s','%d',%s,'%s','%d','%d','%d','%d','%s'",
            pSipMsg->callid.c_str(),
            pSipMsg->domainname.c_str(),
            pSipMsg->contactaddress.c_str(),
            pSipMsg->ipaddress.c_str(),
            pSipMsg->addresstype,
            pSipMsg->username.c_str(),
            pSipMsg->password.c_str(),
            pSipMsg->aaa,
            pSipMsg->expries,
            pSipMsg->cseq,
            1,
            pSipMsg->proxyusername.c_str());
        cout<<str<<endl;
        result=regobj->DoUserRegis(str);
        if(result !=0) {
          pSipMsg->Regevent = EN_REG_FAILED;
        }
        pSipMsg->Regevent = EN_REG_SUCESS;
        CLog::Detail(THISMODULE, "xGateDBManager::process_msg regevent (%s)",str);	
        break;
      }
    case EN_REG_SET_STATUS:
      {
        CLog::Detail(THISMODULE, "xGateDBManager::process_msg regevent EN_REG_SET_STATUS");
        pSipMsg->username.erase(std::remove(pSipMsg->username.begin(),pSipMsg->username.end(),'\"'),pSipMsg->username.end());
        sprintf(str,"Exec sip_update_registered_status  '%s','%s','%s','%d'",
            pSipMsg->callid.c_str(),
            pSipMsg->domainname.c_str(),
            pSipMsg->username.c_str(),
            pSipMsg->status);
        regobj->DoRegisStatus(str);

        CLog::Detail(THISMODULE, "xGateDBManager::process_msg regevent (%s)",str);
        break;
      }
    case EN_DEREGISTER:
      {
        CLog::Detail(THISMODULE, "xGateDBManager::process_msg regevent EN_DEREGISTER");
        sprintf(str,"Exec sip_delete_user_registration  '%s','%s','%s','%s'",
            pSipMsg->callid.c_str(),
            pSipMsg->domainname.c_str(),
            pSipMsg->contactaddress.c_str(),
            pSipMsg->username.c_str());
        regobj->DoDeleteRegis(str);
        pSipMsg->Regevent = EN_REG_SUCESS;
        break;
      }
    case EN_UNKNOWN:
    default:
      {
        CLog::Detail(THISMODULE, "xGateDBManager::process_msg regevent EN_UNKNOWN");
        break;        
      }
  }
  PostToReceiver(pSipMsg);
}

#ifdef XGATE_HAS_BUSINESS_SUPPORT
bool xGateDBManager::parseJsonMsg(const char *jsonData, Document &doc)
{
  CLog::Detail(THISMODULE, "parsing business json data: %s", jsonData);
  ParseResult ok = doc.Parse(jsonData);
  if(!ok) {
    CLog::Error(THISMODULE, "parseJsonMsg failed. Encountered with json parse error !");
    return false;
  } else if(!doc.IsObject()) {
    CLog::Error(THISMODULE, "parseJsonMsg failed. Not able create documnet object !");
    return false;
  }

  CLog::Detail(THISMODULE, "successfully parsed business json data");
  return true;
}

bool xGateDBManager::getInputDoc(Document &doc, const Value &inputVal)
{
  if(doc.HasMember("input")) {
    inputVal = doc["input"].GetObject();
    return true;
  }
  return false;
}

bool xGateDBManager::process_business_request(xGateBusinessMsg *pMsg)
{
  const char  *data = 0;
  unsigned int dataLen = 0;
  pMsg->get_data(data);
  dataLen = pMsg->get_data_len();

  if(!data && dataLen <=0) {
    CLog::Error(THISMODULE, "process_business_request failed. Invalid data in xGateBusinessMsg !");
    return false;
  }

  //step1 : parse json message
  Document doc;
  if(!parseJsonMsg(data, doc)) {
    CLog::Error(THISMODULE, "process_business_request failed to parse business message !");
    return false;
  }

  xGateBusinessDbEvent event; 
  if(doc.HasMember("uid")) {
    m_uid = doc["uid"].GetString();
  } else {
    CLog::Error(THISMODULE, "process_business_request failed. 'uid' is missing in business json message !");
    return false;
  }

  if(doc.HasMember("opcode")) {
    event = doc["opcode"].GetInt();
  } else {
    CLog::Error(THISMODULE, "process_business_request failed. 'opcode' is missing in json message !");
    return false;
  }

  const Value inputVal;
  if(!getInputDoc(doc, inputVal)) {
    CLog::Error(THISMODULE, "process_business_request failed. not able to get 'input' object from main json object !");
    return false;
  }

  switch(event) {
    case EN_DB_EVENT_PREQUERY:
      {
        CLog::Detail(THISMODULE,"process_business_request EN_DB_PREQUERY_REQ");
        if(!do_prequery_request(doc, inputVal)) {
          CLog::Error(THISMODULE, "process_business_request failed. do_prequery_request for uid: %s failed !", m_uid.c_str());
        }
        break;
      }
    case EN_DB_EVENT_GET_AOR: //get address of records
      {
        CLog::Detail(THISMODULE,"process_business_request EN_DB_EVENT_GET_AOR");
        if(!do_aor_request(doc, inputVal)) {
          CLog::Error(THISMODULE, "process_business_request failed. do_aor_request for uid: %s failed !", m_uid.c_str());
        }
        break;
      }
    default:
      {
        break;
      }
  }

  return EN_XGATE_STATUS_SUCCESS; 
}

bool xGateDBManager::do_prequery_request(Document &doc, const Value &inputVal) 
{
  string cli(""), ddi(""), domainName("");
  Value::ConstMemberIterator itr = inputVal.MemberBegin();
  Value::ConstMemberIterator itrEnd = inputVal.MemberEnd();

  for (itr; itr != itrEnd; ++itr) {
    if(!strcmp(itr->name.GetString(), "cli")) {
      cli = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "ddi")) {
      ddi = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "domain_name")) {
      domainName = itr->value.GetString();
    }
  }

  if(cli.empty() || ddi.empty()) { //Note: for PSTN call domain name will be unknown
    CLog::Error(THISMODULE, "do_prequery_request failed. either cli/ddi/domainName value is empty !");
    return false;
  }

  HSTMT hstmt;
  char queryStr[250]={0};
  sprintf(queryStr, "exec sip_pbx_new_get_extension_info_new '%s', '%s','%s'", \
      cli.c_str(), ddi.c_str(), domainName.c_str());
  if(!db_connection->execute_query(queryStr, hstmt)) {
    CLog::Error(THISMODULE, "do_prequery_request failed while executing query: '%s' !", queryStr);
    db_connection->CloseStatementHandle(hstmt);
    return false;
  }

  string output("");
  if(!get_prequery_result(hstmt, output)) {
    CLog::Error(THISMODULE, "do_prequery_request failed while reading result of query: '%s' !", queryStr);
    return false;
  }

  if(!post_result(doc, output)) {
    CLog::Error(THISMODULE, "do_prequery_request failed while posting prequery result to business layer !");
    return false;
  }

  return true;
}

bool xGateDBManager::get_prequery_result(HSTMT hstmt, string &output) 
{
  std::ostringstream oss;
  SQLRETURN retcode;
  SQLINTEGER iVal; 
  char sVal[50] = {0};

  while(1) {  
    retcode = SQLFetch(hstmt);  
    if(retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {  
      printf("got sql error while reading prequery result\n");
      db_connection->CloseStatementHandle(hstmt);
      return false;
    }  
    if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {  
      oss << "{";
      SQLGetData(hstmt, EN_DOMAIN_ID, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"domain_id\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_IS_SWB, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_swb\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_SWB_DDI, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_swb_ddi\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_SWB_DOMAIN_ID, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_swb_domain_id\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_DIRECTORY_ENABLED, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_dir_enabled\"" << ":" << iVal << ",";
      SQLGetData(hstmt,EN_IS_HUNT_GROUP, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_hunt_group\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_FIND_FOLLOW_ME_NUM, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"find_follow_me_num\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_IS_CONFERENCE, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_conference\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_VOICE_MAIL, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_voice_mail\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_INTERNAL_FOUND, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"internal_found\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_HUND_GROUP_DOMAIN_ID, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"hunt_grp_domain_id\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_IS_DIRECT_CALL_STATUS, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_direct_call_status\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_IS_DIRECT_CALL_PREQ, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_direct_call_preq\"" << ":" <<  iVal << ",";
      SQLGetData(hstmt, EN_DIRECT_CALL_PIN, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"direct_call_pin\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_LEG_A_DESKPHONE_STATUS, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"leg_a_deskphone_status\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_LEG_B_DESKPHONE_STATUS, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"leg_b_deskphone_status\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_BLOCK_TYPE, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"block_type\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_BLOCKED_NUM_IVR, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"blocked_num_ivr\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_IS_NUM_BLOCKED, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_num_blocked\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_CALLER_ID, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"caller_id\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_CALL_REC_OUT_CLI, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"call_rec_out_cli\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_CALL_REC_OUT_DDI, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"call_rec_out_ddi\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_ONDEMAND_REC_START_IVR, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"ondemand_rec_start_ivr\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_ONDEMAND_REC_STOP_IVR, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"ondemand_rec_stop_ivr\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_USER_GREETING_FILE, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"user_greeting_file\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_USER_CALL_SCREENING_TYPE, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"user_call_screening_type\"" << ":" << iVal << ",";
      SQLGetData(hstmt,EN_DDI_USER_HOLD_MUSIC, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"ddi_user_hold_music\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_CLI_USER_HOLD_MUSIC, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"cli_user_hold_music\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_COMPANY_HOUR, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"company_hour\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_AFTER_GREETING_FILE, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"after_greeting_file\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_CALL_SCREENING_TYPE, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"call_screening_type\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_CONNECTING_IVR, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"connecting_ivr\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_AUDIO_CONNECTION_TYPE, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"audio_connection_type\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_AUDIO_CONNECTING_FILE, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"audio_connecting_file\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_DID_USER_HOLD_MUSIC_TYPE, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"did_user_hold_music_type\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_CLI_USER_HOLD_MUSIC_TYPE, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"cli_user_hold_music_type\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_DID_EXT, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"did_ext\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_CLI_USER_VIDEO_GREETING, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"cli_user_video_greeting\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_DID_USER_VIDEO_GREETING, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"did_user_video_greeting\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_HOST_CODE, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"host_code\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_PARTICIPANT_CODE, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"participant_code\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_IS_EMERGENCY, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_emergency\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_DOMAIN_NAME, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"domain_name\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_CLI_REC_FLAG, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"cli_rec_flag\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_DID_REC_FLAG, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"did_rec_flag\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_ERR_MSG, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"err_msg\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_IS_VOICEMAIL_DID, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_voicemail_did\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_IS_VOICEMAIL_CLI, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_voicemail_cli\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_VOICEMAIL_PIN, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);  
      oss << "\"voicemail_pin\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_IS_VOICEMAIN_PIN_NEED, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);  
      oss << "\"is_voicemail_pin_need\"" << ":" << iVal << ",";
      SQLGetData(hstmt, EN_EXTERNALNB_DOMAIN_NAME, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);
      oss << "\"externalnb_domain_name\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_EXTERNALNB_EXTN, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);
      oss << "\"externalnb_extn\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_EXTERNALNB_EXTN_PWD, SQL_C_CHAR, &sVal, sizeof(sVal), NULL);
      oss << "\"externalnb_extn_pwd\"" << ":" << "\""  << sVal << "\"" << ",";
      SQLGetData(hstmt, EN_EXTERNALNB_DOMAIN_ID, SQL_C_SLONG, &iVal, sizeof(iVal), NULL);
      oss << "\"externalnb_domain_id\"" << ":" << iVal << "}";
    } else {  
      break;  
    }  
  } 

  output = oss.str();
  printf("prequery result output: %s", output.c_str());
  db_connection->CloseStatementHandle(hstmt);
  return true;
}

bool xGateDBManager::do_aor_request(Document &doc, const Value &val)
{
  string ddi(""), domainName("");
  if(val.HasMember("ddi")) {
    ddi = val["ddi"].GetString();
  }
  if(val.HasMember("domain_name")) {
    domainName = val["domain_name"].GetString();
  }

  if(ddi.empty() || domainName.empty()) {
    CLog::Error(THISMODULE, "do_aor_request failed. either ddi/domain_name is empty for uid: %s", \
        m_uid.c_str());
    return false;
  }

  xGateDbmMsg *pMongoDBMDispatcher = new xGateDbmMsg();
  pMongoDBMDispatcher->setMsgType(EN_XGATE_MSG_TO_DBM);
  DbmReqDetail &reqDetail = pMongoDBMDispatcher->get_dbm_request_detail();
  reqDetail.m_transId = m_uid;
  reqDetail.m_dbType = EN_XGATE_DB_LAYER_MONGODB;
  reqDetail.m_opCode = EN_XGATE_DBM_MSG_DBEXECUTE;
  reqDetail.m_dbName = "unifiedring_pbx";
  reqDetail.m_spName = "sip_get_Registered_user_info";

  std::ostringstream oss;
  oss << jbegin << "Username" << jcolon << ddi << jcomma \
    << "domain_name" << jcolon  << domainName << jend;
  reqDetail.m_input = oss.str();

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateDbmMsg*>(pMongoDBMDispatcher);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  ACE_Task<ACE_MT_SYNCH>* taskPtr = xGateUtil::getDbmDispatcher();
  if(taskPtr->putq(pAmb, &tval)) {
    CLog::Detail(THISMODULE, "MGC-Business-layer-thread posted message to DBM-Network-component");
  } else {
    CLog::Error(THISMODULE, "MGC-Business-layer-thread failed to pose message to DBM-Network-component !");
  }
  return true;
}

bool xGateDBManager::post_result(Document &doc, string &output)
{
  if(output.empty()) {
    CLog::Error(THISMODULE, "post_result failed. given output vaule is empty!");
    return false;
  }

  Document outputDoc(&doc.GetAllocator());
  ParseResult ok = outputDoc.Parse(output.c_str());
  if(!ok) {
    CLog::Error(THISMODULE, "post_result failed due to json parse error: %s !", \
        GetParseError_En(ok.Code()));
    return false;
  } else if(!outputDoc.IsObject()) {
    CLog::Error(THISMODULE, "post_result failed to create json object from given output: %s!", \
        output.c_str());
    return false;
  }

  doc.RemoveMember("output"); //remove existing input object if any
  doc.AddMember("output", outputDoc, doc.GetAllocator());

  StringBuffer strBuf; 
  Writer<StringBuffer> writer(strBuf);
  doc.Accept(writer);

  //posting result to business layer
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = xGateUtil::getBusinessProcessor();
  if(!taskPtr) {
    CLog::Error(THISMODULE, "post failed. BusinessProccessor taksptr is NULL !");
    return false;
  }

  int len = strBuf.GetLength();
  char* data = new char[len+1];
  memset(data, 0, (len+1));
  memcpy (data, strBuf.GetString(), len);
  data[len] = '\0';

  xGateBusinessMsg *pMsg = new xGateBusinessMsg();
  pMsg->set_business_msg_type(EN_BUSINESS_DBLAYER_MSG);
  pMsg->set_data(data);
  pMsg->set_data_len(len);

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateBusinessMsg*>(pMsg);
  ACE_Time_Value tval ((time(NULL)));
  tval += ACE_Time_Value(0,1);

  if(taskPtr->putq(pAmb, &tval)) {
    CLog::Detail(THISMODULE, "successfully posted message to BusinessProcessor");
  } else {
    CLog::Error(THISMODULE, "failed to post message to BusinessProcessor !");
    return false;
  }
  return true;
}

bool xGateDBManager::handle_db_response(DbmReqDetail &reqDetail)
{
  if(reqDetail.m_transId.empty() || reqDetail.m_output.empty() \
      || reqDetail.m_spName.empty()) {
    CLog::Error(THISMODULE, "handle_db_response failed. either uid/output/spname might be empty !");
    return false;
  }

  xGateBusinessDbEvent event = EN_DB_EVENT_UNKNOWN; 
  if(reqDetail.m_spName == "sip_get_Registered_user_info") {
    event = EN_DB_EVENT_GET_AOR;
  }

  xGateBusinessData data(reqDetail.m_transId, event, reqDetail.m_output, FROM_DB_TO_BUSINESS_PROCESSOR);
  return xGateBusinessUtil::sendBusinessData(data);
}
#endif

xGateRetVal xGateDBManager::handle_db_process(xGateDBObj *dbObj)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::handle_db_process()");
  char pass_str[1024];
  std::string	ret_string;
  callInfoParameter cp = dbObj->cp;
  switch (dbObj->dbevent)
  {
    case EN_DB_PREQUERY_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::handle_db_process EN_DB_PREQUERY_REQ");

        if (!db_connection) {
          db_connection_pbx = new  DB_Connection(UNIFIEDRING_DB_SERVER_IP_ADDRESS, UNIFIEDRING_DB_UID, UNIFIEDRING_DB_PASSWORD, UNIFIEDRING_DB_DATABASE, 1);
          db_connection = db_connection_pbx;
        }
        xGatePBXBusiness* buspro;
        buspro = new xGatePBXBusiness();
        buspro->fetch_SysconfParams(dbObj);
        buspro->checkCalledNumberType(dbObj, this);

        if(dbObj->cp.isSwitchBoardCall) {
          buspro->switchBoardProcess(dbObj,this);
          break;
        } else {
          dbObj->transid = generateTransid();
          InsertDbObjlInfo(dbObj);
          buspro->GetAddressofrecords(dbObj);
          return;
        }
      }
    case EN_DB_SWBMAINMENUIVRTIMEOUT_RES:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_SWBMAINMENUIVRTIMEOUT_RES ");
        dbObj->cp.swbinfo.DigitReceived.assign("0");
      }
    case EN_DB_SWB_EVENT_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_SWB_EVENT_REQ ");
        xGatePBXBusiness* buspro_3;
        buspro_3 = new xGatePBXBusiness();
        bool is_operator_extension;
        is_operator_extension = false;
        if((!strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),"0")) || \
            (!(strcmp(dbObj->cp.swbinfo.DigitReceived.c_str(),"     $$$")))) {
          if (strlen(dbObj->cp.gm_swb_operator_extension.c_str()) <= 0) {
            dbObj->cp.cnCalled.assign("200");
          } else {
            dbObj->cp.cnCalled.assign(dbObj->cp.gm_swb_operator_extension.c_str());
          }
          is_operator_extension = true;	
        } else {
          dbObj->cp.cnCalled.assign(dbObj->cp.swbinfo.DigitReceived.c_str());
        }
        dbObj->cp.isSwitchBoardCall_duringDialledExtension = dbObj->cp.isSwitchBoardCall;
        buspro_3->fetch_featureListParams_from_DBmanager(dbObj, this);
        if (is_operator_extension) {
          is_operator_extension = false;
          dbObj->cp.callForwardingFile.assign(WAV_FILE_PATH);
          dbObj->cp.callForwardingFile.append(CONNECT_TO_OPERATOR_FILE);
          dbObj->cp.isThisCallForwarded = true;
        }
        dbObj->transid = generateTransid();
        InsertDbObjlInfo(dbObj);
        buspro_3->GetAddressofrecords(dbObj);
        break;
      }
    case EN_CONFERENCE_DTMFDIGIT_TIMEOUT_RES:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_CONFERENCE_DTMFDIGIT_TIMEOUT_RES ");
      }
    case EN_DB_CONF_EVENT_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_CONF_EVENT_REQ ");
        xGatePBXBusiness* buspro_5;
        buspro_5 = new xGatePBXBusiness();

        if(dbObj->cp.confinfo.ConfCallEvent ==  EN_CONF_DELPARTICIPANT) {
          buspro_5->deleteConferenceDBRecords(dbObj, this);
          return ;
        }

        if ((dbObj->cp.confinfo.ConfDtmfModule == EN_CONF_GETPIN) && \
            (dbObj->cp.confinfo.ConfDtmfState == EN_CONF_DTMFSTART)) {
          // only VALIDATE phase of EN_CONF_GETPIN is outsourced in GetPin()
          buspro_5->GetPin(dbObj, this, 4);
          break;
        }

        if (dbObj->cp.confinfo.ConfDtmfModule == EN_CONF_GETCONFID) {
          buspro_5->GetConfId(dbObj, this,5);
        }
        break;
      }
    case EN_DB_SWBDOMAINNAME_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_SWBDOMAINNAME_REQ ");
        dbObj->cp.routeAddr = DATABASE_QUERY_get_SwitchBoard_domainName(dbObj->cp.cnCalled.c_str());
        dbObj->dbevent = (XGATEDBEVENT) EN_DB_SWBDOMAINNAME_RES;
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_SWBDOMAINNAME_RES = %s",dbObj->cp.routeAddr.c_str());
        break;
      }
    case EN_DB_DOMAINID_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_DOMAINID_REQ ");
        dbObj->cp.gm_A_domain_id = DATABASE_QUERY_GET_DOMAIN_ID(dbObj->cp.routeAddr.c_str());
        dbObj->dbevent = (XGATEDBEVENT) EN_DB_DOMAINID_RES;
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_DOMAINID_RES = %s",dbObj->cp.callOnHoldFilenameCallingParty.c_str());
        break;
      }
    case EN_DB_COMPANYHOURUSER_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_COMPANYHOURUSER_REQ ");
        dbObj->cp.gm_user_company_hour = DATABASE_QUERY_GET_COMPANY_HOUR_USER(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalled.c_str(), 3);
        dbObj->dbevent = (XGATEDBEVENT) EN_DB_COMPANYHOURUSER_RES;
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_COMPANYHOURUSER_RES = %d",dbObj->cp.gm_user_company_hour);
        break;
      }
    case EN_DB_INCOMINGCALLTYPE_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_INCOMINGCALLTYPE_REQ ");
        dbObj->cp.gm_incoming_calling_type = DATABASE_QUERY_INCOMING_CALL_TYPE(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalled.c_str(), 1);
        dbObj->dbevent = (XGATEDBEVENT) EN_DB_INCOMINGCALLTYPE_RES;
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_INCOMINGCALLTYPE_RES = %d",dbObj->cp.gm_incoming_calling_type);

        if (1 == DATABASE_QUERY_FORWARD_STATUS_ENABLED(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalled.c_str(), "1", 2)) {
          dbObj->cp.gm_isThisCallForwarded_CompanyHours = true;
        }
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_INCOMINGCALLTYPE_RES = %d",dbObj->cp.gm_isThisCallForwarded_CompanyHours);
        break;
      }
    case EN_DB_FORWARDSTATUSENABLED_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_FORWARDSTATUSENABLED_REQ ");
        if (1 == DATABASE_QUERY_FORWARD_STATUS_ENABLED(dbObj->cp.gm_B_domain_id.c_str(), \ 
              dbObj->cp.cnCalled.c_str(), "2", 2)) {
          dbObj->cp.gm_isThisCallForwarded_AfterHours = true;
        }
        dbObj->dbevent = (XGATEDBEVENT) EN_DB_FORWARDSTATUSENABLED_RES;
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_FORWARDSTATUSENABLED_RES = %d",dbObj->cp.gm_isThisCallForwarded_AfterHours);
        break;
      }
    case EN_DB_CALLFORWARDNUMBER_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_CALLFORWARDNUMBER_REQ ");
        dbObj->cp.gm_call_forward_number = DATABASE_QUERY_GET_CALLFORWARD_NUMBER_USER(dbObj->cp.gm_B_domain_id.c_str(), dbObj->cp.cnCalled.c_str(), 4);
        dbObj->dbevent = (XGATEDBEVENT) EN_DB_CALLFORWARDNUMBER_RES;
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_CALLFORWARDNUMBER_RES = %s",dbObj->cp.gm_call_forward_number.c_str());
        break;
      }
    case EN_DB_COMBINEDSPSELECTOR_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_COMBINEDSPSELECTOR_REQ ");
        std::vector<DBQueryResultData>* ret_DBQueryResultData;
        ret_DBQueryResultData = DATABASE_QUERY_get_TRUNKOUT_PORTOUT(dbObj->cp.gm_site_code.c_str(), \
            dbObj->cp.gm_switch_code.c_str(), dbObj->cp.cnCalled.c_str(), dbObj->cp.cnCalling.c_str(), dbObj->cp.gm_trunk_in.c_str(),     dbObj->cp.gm_port_in.c_str());
        dbObj->cp.ptr_DBQueryResultData = ret_DBQueryResultData;
        dbObj->dbevent = (XGATEDBEVENT) EN_DB_COMBINEDSPSELECTOR_RES;
        break;
      }
    case EN_DB_COMBINEDSPINITIAL_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_COMBINEDSPINITIAL_REQ ");
        std::vector<DBQueryResultData>* ret_DBQueryResultData;
        ret_DBQueryResultData = DATABASE_QUERY_COMBINED(dbObj->cp.cnCalling.c_str(),dbObj->cp.cnCalled.c_str(),dbObj->cp.gm_user_B_DOMAIN_NAME.c_str());
        dbObj->cp.ptr_DBQueryResultData = ret_DBQueryResultData;
        dbObj->dbevent = (XGATEDBEVENT) EN_DB_COMBINEDSPINITIAL_RES;
        break;
      }
    case EN_DB_SWBCOMBINEDSP_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_SWBCOMBINEDSP_REQ ");
        dbObj->cp.ptr_DBQueryResultData = DATABASE_QUERY_SWB_COMBINED(dbObj->cp.gm_A_domain_id.c_str(), dbObj->cp.cnCalled.c_str());
        dbObj->dbevent = (XGATEDBEVENT) EN_DB_SWBCOMBINEDSP_RES;
        break;
      }
    case EN_DB_TOTALNOOFREGISTEREDUSERS_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_TOTALNOOFREGISTEREDUSERS_REQ ");
        dbObj->cp.gm_totalNoOfRegisteredUsers = DATABASE_QUERY_get_totalNoOfRegisteredUsers(dbObj->cp.cnCalled.c_str(), dbObj->cp.gm_user_B_DOMAIN_NAME.c_str());
        dbObj->dbevent = (XGATEDBEVENT) EN_DB_TOTALNOOFREGISTEREDUSERS_RES;
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_TOTALNOOFREGISTEREDUSERS_RES = %d",dbObj->cp.gm_totalNoOfRegisteredUsers);
        break;
      }
    case EN_DB_COMBINEDSPREGISTAR_REQ_1:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_COMBINEDSPREGISTAR_REQ_1 ");
      }
    case EN_DB_COMBINEDSPREGISTAR_REQ_2:
      {
        if (dbObj->dbevent == (XGATEDBEVENT) EN_DB_COMBINEDSPREGISTAR_REQ_2) {
          CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_COMBINEDSPREGISTAR_REQ_1 ");
        }
        std::vector<DBQueryResultData>* ret_DBQueryResultData;
        ret_DBQueryResultData = DATABASE_QUERY_combined_SendAppNotification(dbObj->cp.cnCalled.c_str(),  dbObj->cp.gm_user_B_DOMAIN_NAME.c_str(), 4, 1);
        dbObj->cp.ptr_DBQueryResultData = ret_DBQueryResultData;

        if (dbObj->dbevent == (XGATEDBEVENT) EN_DB_COMBINEDSPREGISTAR_REQ_1) {
          dbObj->dbevent = (XGATEDBEVENT) EN_DB_COMBINEDSPREGISTAR_RES_1;
        } else {
          dbObj->dbevent = (XGATEDBEVENT) EN_DB_COMBINEDSPREGISTAR_RES_2;
        }
        break;
      }
    case EN_DB_CONFERENCECALL_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : START_CONFERENCE_CALL_REQ ");
        dbObj->cp.confinfo.gm_PIN_from_database = DATABASE_QUERY_IS_VALID_CONFERENCE_BRIDGE_CONF_ID(dbObj->cp.gm_B_domain_id.c_str(),dbObj->cp.confinfo.gm_conf_dtmf_digits.c_str());
        dbObj->dbevent = (XGATEDBEVENT) EN_DB_CONFERENCECALL_RES;
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_CONFERENCECALL_RES = %d",dbObj->cp.gm_totalNoOfRegisteredUsers);
        break;
      }
    case EN_DB_CONFPIN_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_CONFPIN_REQ ");
        dbObj->cp.confinfo.gm_PIN_from_database = DATABASE_QUERY_GET_CONFERENCE_BRIDGE_PIN(dbObj->cp.gm_B_domain_id.c_str(),dbObj->cp.confinfo.gm_conf_dtmf_digits.c_str());
        dbObj->dbevent = (XGATEDBEVENT) EN_DB_CONFPIN_RES;
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : EN_DB_CONFPIN_RES = %d",dbObj->cp.gm_totalNoOfRegisteredUsers);
        break;
      }
    case EN_DB_NOTIFYE_REQ:
      {
        if(dbObj->cp.gm_totalNoOfRegisteredUsers > 0) {
          xGatePBXBusiness* buspro_4;
          buspro_4 = new xGatePBXBusiness();
          dbObj->transid = generateTransid();
          InsertDbObjlInfo(dbObj);
          buspro_4->GetAddressofrecords(dbObj);
          return;
        }
        break;
      }
    case EN_DB_GETREG_REQ:
      {
        if((dbObj->cp.gm_totalNoOfRegisteredUsers > 0)) {
          xGatePBXBusiness* buspro_4;
          buspro_4 = new xGatePBXBusiness();
          dbObj->transid = generateTransid();
          InsertDbObjlInfo(dbObj);
          buspro_4->GetAddressofrecords(dbObj);
          return;
        }
        break;
      }
    case EN_DB_REGDEATILS_REQ:
      {
        xGatePBXBusiness* buspro_4;
        buspro_4 = new xGatePBXBusiness();
        dbObj->transid = generateTransid();
        InsertDbObjlInfo(dbObj);
        buspro_4->GetAddressofrecords(dbObj);
        return;
      }
    default:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::process_msg : DEFAULT ; dbObj->dbevent= %d", dbObj->dbevent);
        break;
      }
  }
  PostMessage(dbObj,XGATEPOSTTYPE::EN_PROCESS);
}

std::string xGateDBManager::DATABASE_QUERY_GET_DOMAIN_ID(char* domainName)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_GET_DOMAIN_ID()");
  char pass_str[1024];
  std::string     ret_string;
  ret_string.assign("");
  sprintf(pass_str,"exec SIP_PBX_get_domain_id '%s'",domainName);
  ret_string = db_connection->getFirstColumn_StringValue(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_GET_DOMAIN_ID = %s : USERA_DOMAINID = %s",pass_str,ret_string.c_str());
  if (strlen(ret_string.c_str()) <= 0) { //TODO: Yoga, can we do any optimization in this logics?
    // DB-connections not working! Restore all DB-connections...
    delete db_connection_pbx;
    delete db_connection_selector;
    delete db_connection_registrar;

    db_connection_pbx = new  DB_Connection(UNIFIEDRING_DB_SERVER_IP_ADDRESS, UNIFIEDRING_DB_UID, UNIFIEDRING_DB_PASSWORD, UNIFIEDRING_DB_DATABASE, 1);
    db_connection = db_connection_pbx;
    db_connection_selector = new  DB_Connection(SELECTOR_DB_SERVER_IP_ADDRESS, SELECTOR_DB_UID, SELECTOR_DB_PASSWORD, SELECTOR_DB_DATABASE, 2);
    db_connection_registrar = new  DB_Connection(REGISTRAR_DB_SERVER_IP_ADDRESS, REGISTRAR_DB_UID, REGISTRAR_DB_PASSWORD, REGISTRAR_DB_DATABASE, 3);

    ret_string = db_connection->getFirstColumn_StringValue(pass_str);
    CLog::Detail(THISMODULE,"DATABASE_QUERY_GET_DOMAIN_ID = %s : USERA_DOMAINID = %s",pass_str,ret_string.c_str());
  }
  return ret_string;
}

//TODO: Yoga, we have to optimize get data from query result from sql database.
std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_COMBINED(const char* user_a, const char* user_b, const char* domainName)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_COMBINED()");
  char 		pass_str[1024];
  std::vector<DBQueryResultData>*	ret_DBQueryResultData;

  sprintf(pass_str,"exec sip_pbx_new_get_extension_info_new '%s', '%s','%s'",user_a, user_b, domainName);
  ret_DBQueryResultData = db_connection->db_combined_sp(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED sp == %s",pass_str);
  return ret_DBQueryResultData;
}

std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_SWB_COMBINED(const char* domain_id, const char* switchboard_number)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_SWB_COMBINED()");
  char 		pass_str[1024];
  std::vector<DBQueryResultData>*	ret_DBQueryResultData;

  sprintf(pass_str,"exec UR_PBX_get_company_hours_setting_v2 '%s','%s'",domain_id, switchboard_number);
  ret_DBQueryResultData = db_connection->db_combined_sp_swb(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_SWB_COMBINED sp == %s",pass_str);
  return ret_DBQueryResultData;
}

std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_INCOMING_CALL_TYPE_COMBINED_GET_CALLFORWARD_NUMBER(char* domainId, char* extNum)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_INCOMING_CALL_TYPE_COMBINED_GET_CALLFORWARD_NUMBER()");
  char 		pass_str[1024];
  std::vector<DBQueryResultData>*	ret_DBQueryResultData;

  sprintf(pass_str,"exec sip_pbx_extn_callhandling_forwarding '%s', '%s','1'",domainId, extNum);
  ret_DBQueryResultData = db_connection->db_combined_sp_callForward(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_INCOMING_CALL_TYPE_COMBINED_GET_CALLFORWARD_NUMBER sp == %s",pass_str);
  return ret_DBQueryResultData;
}

std::string xGateDBManager::DATABASE_QUERY_get_SwitchBoard_domainName(char* a_did)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_get_SwitchBoard_domainName()");
  char 		pass_str[1024];
  std::string	ret_string;

  sprintf(pass_str,"exec sip_pbx_check_switchboard_no '%s',''",a_did);
  ret_string = db_connection->get_Nth_Column_StringValue(5,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_get_SwitchBoard_domainName = %s : USERA_DOMAINID = %s",pass_str,ret_string.c_str());
  return ret_string;
}

std::string xGateDBManager::DATABASE_QUERY_COMBINED_StringValue_test(char* user_a, char* user_b, char* domainName, int DbColumnNumber)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_COMBINED_StringValue_test()");
  char 		pass_str[1024];
  std::string	ret_string;

  sprintf(pass_str,"exec sip_pbx_new_get_extension_info_test '%s', '%s','%s'",user_a, user_b, domainName);
  ret_string = db_connection->get_Nth_Column_StringValue(DbColumnNumber,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED_StringValue_test = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

std::string xGateDBManager::DATABASE_QUERY_GET_CALLER_ID(char* domainid, char* extension)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_GET_CALLER_ID()");
  char            pass_str[1024];
  std::string     ret_string;

  sprintf(pass_str,"exec sip_pbx_get_extn_callerid '%s', '%s'",domainid, extension);
  ret_string = db_connection->get_Nth_Column_StringValue(1,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_GET_CALLER_ID = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

int xGateDBManager::DATABASE_QUERY_IS_DND_ENABLED_FOR_APP(char* domainid, char* extension)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_IS_DND_ENABLED_FOR_APP()");
  char 	pass_str[1024];
  int	ret_val;

  sprintf(pass_str,"exec sip_pbx_check_dnd_number_status '%s', '%s'",domainid, extension);
  ret_val = db_connection->get_Nth_Column_IntegerValue(1,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_IS_DND_ENABLED_FOR_APP = %s : return_value = %d",pass_str,ret_val);
  return ret_val;
}

int xGateDBManager::DATABASE_QUERY_COMBINED_IntegerValue_test(const char* user_a, const char* user_b, const char* domainName, int DbColumnNumber)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_COMBINED_IntegerValue_test()");
  char 	pass_str[1024];
  int	ret_val;

  sprintf(pass_str,"exec sip_pbx_new_get_extension_info_test '%s', '%s','%s'",user_a, user_b, domainName);
  ret_val = db_connection->get_Nth_Column_IntegerValue(DbColumnNumber,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED_IntegerValue_test = %s : return_value = %d",pass_str,ret_val);
  return ret_val;
}

std::string xGateDBManager::DATABASE_QUERY_COMBINED_DirectUserInfo_get_device_type_Recordwise(char* extNum, char* domainName, int DbColumnNumber, int DbRecordNumber)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_COMBINED_DirectUserInfo_get_device_type_Recordwise()");
  char 		pass_str[1024];
  std::string	ret_string;

  sprintf(pass_str,"exec sip_get_Registered_user_info '%s', '%s',''",extNum, domainName);
  ret_string = db_connection_registrar->get_Nth_Column_StringValue_Recordwise(DbColumnNumber,pass_str, DbRecordNumber);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED_DirectUserInfo_get_device_type_Recordwise = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

int xGateDBManager::DATABASE_QUERY_COMBINED_DirectUserInfo_get_app_status_Recordwise(char* extNum, char* domainName, int DbColumnNumber, int DbRecordNumber)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_COMBINED_DirectUserInfo_get_app_status()");
  char 	pass_str[1024];
  int	ret_val;

  sprintf(pass_str,"exec sip_get_Registered_user_info '%s', '%s',''",extNum, domainName);
  ret_val = db_connection_registrar->get_Nth_Column_IntegerValue_Recordwise(DbColumnNumber,pass_str, DbRecordNumber);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED_DirectUserInfo_get_app_status = %s : return_value = %d",pass_str,ret_val);
  return ret_val;
}

std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_combined_SendAppNotification(char* extNum, char* domainName, int DbColumnNumber, int DbRecordNumber)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_combined_SendAppNotification()");
  char 		pass_str[1024];
  std::vector<DBQueryResultData>*	ret_DBQueryResultData;

  if (DbRecordNumber == EN_DB_COMBINEDSPREGISTAR_RES_1) {
    sprintf(pass_str,"exec sip_get_Registered_user_info '%s', '%s','','1'",extNum, domainName);
  } else {
    sprintf(pass_str,"exec sip_get_Registered_user_info '%s', '%s','','0'",extNum, domainName);
  }
  ret_DBQueryResultData = db_connection_registrar->db_combined_sp_SendAppNotificationsAll(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED sp == %s",pass_str);
  return ret_DBQueryResultData;
}

std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_CALL_HANDLING(char* extNum, char* domainId)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_CALL_HANDLING()");
  char 		pass_str[1024];
  std::vector<DBQueryResultData>*	ret_DBQueryResultData;

  sprintf(pass_str,"exec sip_pbx_get_call_handling_info '%s', '%s'",extNum, domainId);
  ret_DBQueryResultData = db_connection->db_callhandling_sp(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_CALL_HANDLING sp == %s",pass_str);
  return ret_DBQueryResultData;
}

std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_COMBINED_GET_PSTN_OPERATOR_DETAILS(char* ip_addr)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_COMBINED_GET_PSTN_OPERATOR_DETAILS()");
  char 		pass_str[1024];
  std::vector<DBQueryResultData>*	ret_DBQueryResultData;

  sprintf(pass_str,"exec UR_pbx_pstn_Operator_details_info '%s'",ip_addr);
  ret_DBQueryResultData = db_connection->db_combined_sp_get_pstn_operator_details(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED_GET_PSTN_OPERATOR_DETAILS sp == %s",pass_str);
  return ret_DBQueryResultData;
}

std::string xGateDBManager::DATABASE_QUERY_get_TRUNKOUT(char* site_code, char* switch_code, char* a_did, char* a_cli, char* trunk_in, char* rtp_interface)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_get_TRUNKOUT()");
  char 		pass_str[1024];
  std::string	ret_string;

  sprintf(pass_str,"exec slt_get_selector_v2 '%s','%s','%s','%s','%s','%s'",site_code, switch_code, a_did, a_cli, trunk_in, rtp_interface);
  ret_string = db_connection_selector->get_Nth_Column_StringValue(7,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_get_TRUNKOUT = %s : DATABASE_QUERY_get_TRUNKOUT = %s",pass_str,ret_string.c_str());
  return ret_string;
}

std::string xGateDBManager::DATABASE_QUERY_get_PORTOUT(char* site_code, char* switch_code, char* a_did, char* a_cli, char* trunk_in, char* rtp_interface)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_get_PORTOUT()");
  char 		pass_str[1024];
  std::string	ret_string;

  sprintf(pass_str,"exec slt_get_selector_v2 '%s','%s','%s','%s','%s','%s'",site_code, switch_code, a_did, a_cli, trunk_in, rtp_interface);
  ret_string = db_connection_selector->get_Nth_Column_StringValue(8,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_get_PORTOUT = %s : DATABASE_QUERY_get_PORTOUT = %s",pass_str,ret_string.c_str());
  return ret_string;
}

std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_get_TRUNKOUT_PORTOUT(char* site_code, char* switch_code, char* a_did, char* a_cli, char* trunk_in, char* rtp_interface)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_get_TRUNKOUT_PORTOUT()");
  char 		pass_str[1024];
  std::vector<DBQueryResultData>*	ret_DBQueryResultData;

  sprintf(pass_str,"exec slt_get_selector_v2 '%s','%s','%s','%s','%s','%s'",site_code, switch_code, a_did, a_cli, trunk_in, rtp_interface);
  ret_DBQueryResultData = db_connection_selector->db_combined_sp_TRUNKOUT_PORTOUT(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED sp == %s",pass_str);
  return ret_DBQueryResultData;
}

std::string xGateDBManager::DATABASE_QUERY_COMBINED_DirectUserInfo_get_Contact_address_Recordwise(char* extNum, char* domainName, int DbColumnNumber, int DbRecordNumber)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_COMBINED_DirectUserInfo_get_Contact_address_Recordwise()");
  char 		pass_str[1024];
  std::string	ret_string;

  sprintf(pass_str,"exec sip_get_Registered_user_info '%s', '%s',''",extNum, domainName,DbColumnNumber, DbRecordNumber);
  ret_string = db_connection_registrar->get_Nth_Column_StringValue_Recordwise(DbColumnNumber,pass_str, DbRecordNumber);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED_DirectUserInfo_get_Contact_address_Recordwise = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

std::string xGateDBManager::DATABASE_QUERY_COMBINED_DirectUserInfo_get_proxy_username_Recordwise(char* extNum, char* domainName, int DbColumnNumber, int DbRecordNumber)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_COMBINED_DirectUserInfo_get_proxy_username_Recordwise()");
  char 		pass_str[1024];
  std::string	ret_string;

  sprintf(pass_str,"exec sip_get_Registered_user_info '%s', '%s',''",extNum, domainName,DbColumnNumber, DbRecordNumber);
  ret_string = db_connection_registrar->get_Nth_Column_StringValue_Recordwise(DbColumnNumber,pass_str, DbRecordNumber);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED_DirectUserInfo_get_proxy_username_Recordwise = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

int xGateDBManager::DATABASE_QUERY_get_totalNoOfRegisteredUsers(char* extNum, char* domainName)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_get_totalNoOfRegisteredUsers()");
  char 	pass_str[1024] = "";
  int	ret_val;

  sprintf(pass_str,"exec sip_get_Registered_user_info '%s', '%s',''",extNum, domainName);
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_get_totalNoOfRegisteredUsers() = %s",pass_str);
  ret_val = db_connection_registrar->get_totalNoOfRegisteredUsers(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_get_totalNoOfRegisteredUsers = %s : return_value = %d",pass_str,ret_val);
  return ret_val;
}

int xGateDBManager::DATABASE_QUERY_GET_COMPANY_HOUR_USER(char* domainId, char* extNum, int DbColumnNumber)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_GET_COMPANY_HOUR_USER()");
  char 	pass_str[1024];
  int	ret_val;

  sprintf(pass_str,"exec UR_PBX_get_company_extension_hours_setting '%s', '%s'",domainId, extNum);
  ret_val = db_connection->get_Nth_Column_IntegerValue(DbColumnNumber,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_GET_COMPANY_HOUR_USER = %s : return_value = %d",pass_str,ret_val);
  return ret_val;
}

int xGateDBManager::DATABASE_QUERY_INCOMING_CALL_TYPE(char* domainId, char* extNum, int DbColumnNumber)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_INCOMING_CALL_TYPE()");
  char 	pass_str[1024];
  int	ret_val;

  sprintf(pass_str,"exec sip_pbx_extn_callhandling_forwarding '%s', '%s','1'",domainId, extNum);
  ret_val = db_connection->get_Nth_Column_IntegerValue(DbColumnNumber,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_INCOMING_CALL_TYPE = %s : return_value = %d",pass_str,ret_val);
  return ret_val;
}

int xGateDBManager::DATABASE_QUERY_FORWARD_STATUS_ENABLED(char* domainId, char* extNum, char* flag, int DbColumnNumber)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_FORWARD_STATUS_ENABLED()");
  char 	pass_str[1024];
  int	ret_val;

  sprintf(pass_str,"exec sip_pbx_extn_callhandling_forwarding '%s', '%s','%s'",domainId, extNum, flag);
  ret_val = db_connection->get_Nth_Column_IntegerValue(DbColumnNumber,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_FORWARD_STATUS_ENABLED = %s : return_value = %d",pass_str,ret_val);
  return ret_val;
}

std::string xGateDBManager::DATABASE_QUERY_GET_CALLFORWARD_NUMBER_USER(char* domainId, char* extNum, int DbColumnNumber)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_GET_CALLFORWARD_NUMBER_USER()");
  char 	pass_str[1024];
  std::string     ret_string;

  sprintf(pass_str,"exec sip_pbx_extn_callhandling_forwarding '%s', '%s','1'",domainId, extNum);
  ret_string = db_connection->get_Nth_Column_StringValue(DbColumnNumber,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_GET_CALLFORWARD_NUMBER_USER = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

std::string xGateDBManager::DATABASE_QUERY_GET_DOMAIN_NAME(char* domainId)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_GET_DOMAIN_NAME()");
  char    pass_str[1024];
  std::string     ret_string;

  sprintf(pass_str,"exec sip_pbx_get_domain_name '%s'",domainId);
  ret_string = db_connection->get_Nth_Column_StringValue(1,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_GET_DOMAIN_NAME = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

bool xGateDBManager::DATABASE_QUERY_IS_CALLSCREENING_ENABLED(char* extNum, char* domainId)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_IS_CALLSCREENING_ENABLED()");
  char 	pass_str[1024];
  std::string     ret_string;

  sprintf(pass_str,"exec sip_pbx_call_screening_get '%s', '%s'",extNum, domainId);
  ret_string = db_connection->get_Nth_Column_StringValue(1,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_IS_CALLSCREENING_ENABLED = %s : return_value = %s",pass_str,ret_string.c_str());

  if (!(strcmp(ret_string.c_str(),"1")))
    return true;
  return false;
}

std::string xGateDBManager::DATABASE_QUERY_GET_DIRECT_NUMBER(char* extNum, char* domainId)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_GET_DIRECT_NUMBER()");
  char 	pass_str[1024];
  std::string     ret_string;

  sprintf(pass_str,"exec SIP_PBX_get_extension_info_v2 '%s', '%s'",extNum, domainId);
  ret_string = db_connection->get_Nth_Column_StringValue(1,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_GET_DIRECT_NUMBER = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_GET_DIRECT_NUMBER_COMBINEDSP(char* extNum, char* domainId){
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_GET_DIRECT_NUMBER_COMBINEDSP()");
  char            pass_str[1024];
  std::vector<DBQueryResultData>* ret_DBQueryResultData;

  sprintf(pass_str,"exec SIP_PBX_get_extension_info_v2 '%s', '%s'",extNum, domainId);
  ret_DBQueryResultData = db_connection->db_combined_sp_direct_number(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_GET_DIRECT_NUMBER_COMBINEDSP sp == %s",pass_str);
  return ret_DBQueryResultData;
}

// Conference database queries...
bool xGateDBManager::DATABASE_QUERY_IS_VALID_CONFERENCE_BRIDGE_CONF_ID(char* domain_id, char* conference_id)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_IS_VALID_CONFERENCE_BRIDGE_CONF_ID()");
  char 	pass_str[1024] = "";
  int	ret_val;

  sprintf(pass_str,"exec SIP_PBX_check_conference_id '%s', '%s',''",domain_id, conference_id);
  ret_val = db_connection->get_Nth_Column_IntegerValue(1,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_IS_VALID_CONFERENCE_BRIDGE_CONF_ID = %s : return_value = %d",pass_str,ret_val);
  if (ret_val == 0) 
    return true;
  return false;
}

std::string xGateDBManager::DATABASE_QUERY_GET_CONFERENCE_BRIDGE_PIN(char* domain_id, char* conference_id)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_GET_CONFERENCE_BRIDGE_PIN()");
  char 	pass_str[1024] = "";
  std::string     ret_string;

  sprintf(pass_str,"exec SIP_PBX_check_conference_id '%s', '%s',''",domain_id, conference_id);
  ret_string = db_connection->get_Nth_Column_StringValue(3,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_GET_CONFERENCE_BRIDGE_PIN = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

std::string xGateDBManager::DATABASE_QUERY_GET_CONFERENCE_BRIDGE_PARTICIPANT_PIN(char* domain_id, char* conference_id)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_GET_CONFERENCE_BRIDGE_PARTICIPANT_PIN()");
  char 	pass_str[1024] = "";
  std::string     ret_string;

  sprintf(pass_str,"exec SIP_PBX_check_conference_id '%s', '%s',''",domain_id, conference_id);
  ret_string = db_connection->get_Nth_Column_StringValue(4,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_GET_CONFERENCE_BRIDGE_PARTICIPANT_PIN = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

std::string xGateDBManager::DATABASE_QUERY_INSERT_CONFERENCE_MGRESOURCEID(char* conf_id, char* call_id, char* conf_status, char* conf_mg_ip_address, char* pbx_ip_address, char* connection_time, int participant_type, char* domain_name, char* mg_uid)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_REGISTER_CONFERENCE_MGRESOURCEID()");
  char 	pass_str[1024] = "";
  std::string     ret_string;

  sprintf(pass_str,"exec sip_pbx_conference_register '%s', '%s','%s', '%s','%s', '%s','%d','%s','%s'",conf_id, call_id, conf_status, conf_mg_ip_address, pbx_ip_address, connection_time, participant_type, domain_name, mg_uid);
  ret_string = db_connection->get_Nth_Column_StringValue(1,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_REGISTER_CONFERENCE_MGRESOURCEID = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

void xGateDBManager::DATABASE_QUERY_VOICEMAIL_DEPOSIT(long long unsigned int date, const char* mboxid, const char* cid_number, const char* uuid, const char* msgfile, int msglen) 
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_VOICEMAIL_DEPOSIT()");
  char 	pass_str[1024] = "";
  std::string     ret_string;

  sprintf(pass_str,"exec voicemail_logmsg '%d', '%s','%s', '%s','%s', '%d'",date, mboxid, cid_number, uuid, msgfile, msglen);
  ret_string = db_connection->get_Nth_Column_StringValue(1,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_VOICEMAIL_DEPOSIT = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_VOICEMAIL_RETRIEVAL(const char* mboxID, unsigned int mode)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_VOICEMAIL_RETRIEVAL()");
  char 		pass_str[1024];
  std::vector<DBQueryResultData>*	ret_DBQueryResultData;

  sprintf(pass_str,"exec voicemail_get_msg '%s', '%s'",mboxID, mode);
  ret_DBQueryResultData = db_connection->db_combined_sp_conf_get_callid_mguid(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_VOICEMAIL_RETRIEVAL sp == %s",pass_str);
  return ret_DBQueryResultData;
}

void xGateDBManager::DATABASE_QUERY_CALLRECORD_INSERT(const char* clip_id, const char* clip_path, unsigned int clip_size, long long unsigned int time_epoch, const char* username, const char* extension, const char* domain) 
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_CALLRECORD_INSERT()");
  char 	pass_str[1024] = "";
  std::string     ret_string;

  sprintf(pass_str,"exec sip_pbx_new_insert_call_rec_info '%s', '%s','%u', '%llu','%s', '%s', '%s'",clip_id, clip_path, clip_size, time_epoch, username, extension, domain);
  ret_string = db_connection->get_Nth_Column_StringValue(1,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_CALLRECORD_INSERT = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

std::string xGateDBManager::DATABASE_QUERY_GET_CONFERENCE_MGRESOURCEID(char* host_extension, char* domain_name)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_GET_CONFERENCE_MGRESOURCEID()");
  char 	pass_str[1024] = "";
  std::string     ret_string;

  sprintf(pass_str,"exec sip_pbx_get_conference_register_info '%s','%s'",host_extension, domain_name);
  ret_string = db_connection->get_Nth_Column_StringValue(1,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_GET_CONFERENCE_MGRESOURCEID = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_COMBINED_CONF_GET_CALLID_MGUID(const char* host_extension, const char* domain_name)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_COMBINED_CONF_GET_CALLID_MGUID()");
  char 		pass_str[1024];
  std::vector<DBQueryResultData>*	ret_DBQueryResultData;

  sprintf(pass_str,"exec sip_pbx_get_conference_register_info '%s', '%s'",host_extension, domain_name);
  ret_DBQueryResultData = db_connection->db_combined_sp_conf_get_callid_mguid(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED_CONF_GET_CALLID_MGUID sp == %s",pass_str);
  return ret_DBQueryResultData;
}

std::string xGateDBManager::DATABASE_QUERY_CONF_DELETE_DB_RECORDS(char* conf_id, char* domain_name)
{
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_CONF_DELETE_DB_RECORDS()");
  char 	pass_str[1024] = "";
  std::string     ret_string;

  sprintf(pass_str,"exec sip_pbx_delete_conference_register '%s','%s'",conf_id, domain_name);
  ret_string = db_connection->get_Nth_Column_StringValue(2,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_CONF_DELETE_DB_RECORDS = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

std::string xGateDBManager::DATABASE_QUERY_CONF_REGISTER_MG(char* did, char* call_id, char* conf_mg_ip_address, char* pbx_ip_address, char* conf_mg_uid){
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_CONF_REGISTER_MG()");
  char 	pass_str[1024] = "";
  std::string     ret_string;

  sprintf(pass_str,"exec sip_pbx_conference_mg_register '%s','%s','%s','%s','%s'",did, call_id, conf_mg_ip_address, pbx_ip_address, conf_mg_uid);
  ret_string = db_connection->get_Nth_Column_StringValue(1,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_CONF_DELETE_DB_RECORDS = %s : return_value = %s",pass_str,ret_string.c_str());
  return ret_string;
}

int xGateDBManager::DATABASE_QUERY_CONF_VALIDATE_PIN(char* domainid, char* pin){
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_CONF_VALIDATE_PIN()");
  char 	pass_str[1024];
  int	ret_val;
  sprintf(pass_str,"exec sip_pbx_new_check_conference '%s', '%s'",domainid, pin);
  ret_val = db_connection->get_Nth_Column_IntegerValue(1,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_IS_DND_ENABLED_FOR_APP = %s : return_value = %d",pass_str,ret_val);

  return ret_val;
}

std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_COMBINED_CONF_VALIDATE_PIN(const char* domainid, const char* pin){
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_COMBINED_CONF_VALIDATE_PIN()");
  char 		pass_str[1024];
  std::vector<DBQueryResultData>*	ret_DBQueryResultData;

  sprintf(pass_str,"exec sip_pbx_new_check_conference '%s', '%s'",domainid, pin);
  ret_DBQueryResultData = db_connection->db_combined_sp_conf_validate_pin(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED_CONF_VALIDATE_PIN sp == %s",pass_str);
  return ret_DBQueryResultData;
}

bool xGateDBManager::DATABASE_QUERY_CHECK_PIN_TO_CONNECT(char* domain_id, char* extension){
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_CHECK_PIN_TO_CONNECT()");
  char 	pass_str[1024] = "";
  int	ret_val;

  sprintf(pass_str,"exec sip_pbx_extn_direct_call_status '%s', '%s',''",domain_id, extension);
  ret_val = db_connection->get_Nth_Column_IntegerValue(2,pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_CHECK_PIN_TO_CONNECT = %s : return_value = %d",pass_str,ret_val);
  if (ret_val == 1)
    return true;
  return false;
}

std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_COMBINED_CONF_INFO(const char* conf_id, const char* b_domain_id){
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_COMBINED_CONF_INFO()");
  char 		pass_str[1024];
  std::vector<DBQueryResultData>*	ret_DBQueryResultData;

  sprintf(pass_str,"exec sip_pbx_check_conference_info '%s','%s'",conf_id, b_domain_id);
  ret_DBQueryResultData = db_connection->db_combined_sp_conf_info(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED_CONF_INFO sp == %s",pass_str);
  return ret_DBQueryResultData;
}

std::vector<DBQueryResultData>* xGateDBManager::DATABASE_QUERY_COMBINED_CONF_ID(const char* b_domain_id, const char* conf_id){
  CLog::Detail(THISMODULE,"Inside xGateDBManager::DATABASE_QUERY_COMBINED_CONF_ID()");
  char 		pass_str[1024];
  std::vector<DBQueryResultData>*	ret_DBQueryResultData;

  sprintf(pass_str,"exec SIP_PBX_check_conference_id '%s','%s'",b_domain_id, conf_id);
  ret_DBQueryResultData = db_connection->db_combined_sp_conf_id(pass_str);
  CLog::Detail(THISMODULE,"DATABASE_QUERY_COMBINED_CONF_ID sp == %s",pass_str);
  return ret_DBQueryResultData;
}

bool  xGateDBManager::PostMessage(xGateDBObj *dbObj,XGATEPOSTTYPE destType)
{
  xGateSipMsgBlock *pDBMsg = new xGateSipMsgBlock(EN_XGATE_MSG_DB_IN);
  pDBMsg->m_dbObj = dbObj;
  pDBMsg->m_dbObj->cp = dbObj->cp;
  pDBMsg->m_dbObj->cp.m_mappedUserList = dbObj->cp.m_mappedUserList;
  pDBMsg->m_dbObj->cp.confinfo = dbObj->cp.confinfo;
  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateSipMsgBlock*> (pDBMsg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = (xGateSipSignalProcessor*)GetSipPostHandler(XGATEPOSTTYPE::EN_PROCESS);
  if(taskPtr){
    if(taskPtr->putq(pAmb,&tval))
      CLog::Detail(THISMODULE,"xGateDBManagaer::PostMessage(xGateDBManager) Successfully");
  }
}

void xGateDBManager::handle_regdb_process(xGateDBObj *dbObj, std::string dbOutput)
{
  xGatePBXBusiness* busObj;
  busObj = new xGatePBXBusiness();
  switch (dbObj->dbevent)
  {
    case EN_DB_PREQUERY_REQ:
      {
        busObj->SetRegisterRecordFromDB(dbObj,dbOutput);
        busObj->fetch_featureListParams_from_DBmanager(dbObj, this);
        CLog::Detail(THISMODULE, "xGatePBXBusiness::handle_regdb_process EN_DB_PREQUERY_REQ ");
        if((dbObj->cp.gm_totalNoOfRegisteredUsers ==1)&&(dbObj->cp.calledDeviceType.compare("URDESK:") != 0)) {
          busObj->sendAppNotificationToURAPPDevices(dbObj);
          busObj->updateMappedUserList(dbObj);
          busObj->AppNotification_TimerStarted(dbObj);
        }
        break;
      }
    case EN_DB_NOTIFYE_REQ:
      {
        if(dbObj->cp.gm_totalNoOfRegisteredUsers > 0) {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::handle_regdb_process EN_DB_NOTIFYE_REQ ");
          busObj->SetRegisterRecordFromDB(dbObj,dbOutput);
          busObj->sendAppNotificationToURAPPDevices(dbObj);
          busObj->updateMappedUserList(dbObj);
          busObj->AppNotification_TimerStarted(dbObj);
        }
        break;
      }
    case EN_DB_GETREG_REQ:
      {
        if((dbObj->cp.gm_totalNoOfRegisteredUsers > 0)) {
          CLog::Detail(THISMODULE, "xGatePBXBusiness::handle_regdb_process EN_DB_GETREG_REQ ");
          busObj->SetRegisterRecordFromDB(dbObj,dbOutput);
          busObj->checkForCookieChangeInURApp(dbObj);
          busObj->updateMappedUserList(dbObj);
          busObj->CheckCookieAndDeleteDeviceAndCancelAppNotification(dbObj);
          dbObj->dbevent = (XGATEDBEVENT) EN_DB_NOTIFYE_RES; // Send DialOut request thru EN_DB_NOTIFYE_RES...^M
        }
        break;
      }
    case EN_DB_REGDEATILS_REQ:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::handle_regdb_process :  EN_DB_REGDEATILS_REQ");
        busObj->SetRegisterRecordFromDB(dbObj,dbOutput);
        busObj->populateTransferData(dbObj);
        dbObj->dbevent = EN_DB_REGDEATILS_RES;
        break;
      }
    case EN_DB_SWB_EVENT_RES:
      {
        busObj->SetRegisterRecordFromDB(dbObj,dbOutput);
        busObj->populateTransferData(dbObj);
        dbObj->dbevent = (XGATEDBEVENT) EN_SWB_EVENT_REL;
        CLog::Detail(THISMODULE,"xGateDBManager::handle_regdb_process :  EN_DB_SWB_EVENT_RES");
        break;
      }
    default:
      {
        CLog::Detail(THISMODULE,"xGateDBManager::handle_regdb_process : DEFAULT ; dbObj->dbevent= %d", dbObj->dbevent);
        break;
      }
  }
  PostMessage(dbObj,XGATEPOSTTYPE::EN_PROCESS);
}

std::string xGateDBManager::generateTransid()
{
  /* Decleration and initialization of static constant string type variable */
  static const string charList = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

  /* srand() initialize random number generator */
  /* time() for get current time */
  if(transId_len > 10)
    transId_len =5;

  unsigned int iseed = (unsigned int)time(NULL);
  srand (iseed);
  string alphanumeric = "";

  for(int i = 0; i < transId_len; i++) {
    /* rand() generate random number */
    alphanumeric += charList [rand() % charList.size()];
  }
  transId_len++;
  return alphanumeric;
}

xGateDBObj * xGateDBManager::GetDbObjInfo(std::string transid)
{
  xGateDBObj *dbobj = NULL;
  HASH_CALL_MAP::ITERATOR iterator(m_dbregHashTable);
  if(m_dbregHashTable.find(transid.c_str(),dbobj) != -1){
    CLog::Info(THISMODULE,"xGateSipController::GetCallInfo(Call ID = %s Call Info Found )",transid.c_str());
  }
  return dbobj;
}

bool xGateDBManager::InsertDbObjlInfo(xGateDBObj *dbobj)
{
  HASH_CALL_MAP::ENTRY *dbEntry = 0;
  int status = m_dbregHashTable.bind(dbobj->transid.c_str(),dbobj,dbEntry);
  if(status == 0 && dbEntry) {
    CLog::Info(THISMODULE,"xGateSipController::InsertCallInfo inserting new reg object into map for trans_id:%s", \
        dbobj->transid.c_str());
    dbobj->dbEntry = (void*)dbEntry;
  } else {
    if(status == -1 && !dbEntry) {
      CLog::Error(THISMODULE,"xGateSipController::InsertCallInfo(%s) Failed Insertion",dbobj->transid.c_str());
    } else if(status == 1 && dbEntry) {
      CLog::Detail(THISMODULE,"xGateSipController::InsertCallInfo(%s) Call Already in map",dbobj->transid.c_str());
    }
    return false;
  }
  return true;
}

bool xGateDBManager::DeleteDbObjInfo(xGateDBObj *dbobj)
{
  HASH_CALL_MAP::ENTRY* dbEntry = NULL;
  if(!m_dbregHashTable.find(dbobj->transid.c_str(), dbEntry)) {
    if(dbEntry) {
      m_dbregHashTable.unbind(dbEntry);
      //  CLog::Info(THISMODULE,"xGateSipController::DeleteCallInfo(%s)","Deleted call from Map");
    }
  } else {
    // CLog::Error(THISMODULE,"xGateSipController::DeleteCallInfo(%s) Failed Deletion",dbobj->transid.c_str());
    return false;
  }
  return true;
}

