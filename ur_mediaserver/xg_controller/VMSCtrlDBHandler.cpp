#include "VMSCtrlDBHandler.h"
#include "VmsController.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/ostreamwrapper.h"
using namespace rapidjson;

#define THISMODULE "VMSCTRLDBHANDLER"

VMSCtrlDbHandler::VMSCtrlDbHandler() 
{
}

VMSCtrlDbHandler::~VMSCtrlDbHandler(){

}

bool VMSCtrlDbHandler::Handle_Request_srvr_info_msg(string &tIpInfo) 
{
  /*
     creat the json message 
     create the http request and add the ip and send to http module
     */

  string strSsrvURL = VMS_CONFIG::instance()->m_vmsControllerProfile.m_db_serverInfoURL;
  string strContext = "d6ed2968-a8c8-4152-ac02-65064f152c61";
  std::ostringstream oss;

  oss << "{\"server_ip\":\"" << tIpInfo.c_str() << "\"}";

  xGateHttpServiceMsg *ptHttpMSg = getHttpReuestMsg(strContext, strSsrvURL, EN_HTTP_METHOD_POST,oss.str());

  if(!ptHttpMSg) {
    XGLOG_ERROR("OOM Issue. Creating HTTP request for Server info failed");
    return false;
  }

  XGLOG_INFO("VMS Controller sending request to HTTP Module [%s]", strSsrvURL.c_str());
  VMSCONTROLLER_TASK::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(ptHttpMSg));
  return true;
}


xGateHttpServiceMsg * VMSCtrlDbHandler::getHttpReuestMsg(string &strContext, string &strURL, 
    HttpReqMethod tHttpmethod, 
    string stData)
{
  XGLOG_DEBUG("Inside VMSCtrlDbHandler::getHttpReuestMsg");
  xGateHttpServiceMsg * ptHttpMsg = new xGateHttpServiceMsg();
  if(!ptHttpMsg) {
    XGLOG_ERROR("OOM Issue. Creating HTTP request failed");
    return NULL;
  }
  ptHttpMsg->setDstModuleId(IURDefines::UR_MODULE_HTTP_SERVICE);
  ptHttpMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_CONTROLLER);
  ptHttpMsg->setHttpMsgType(xGateHttpServiceMsg::HTTP_SRV_MSG_TYPE_REQUEST);
  HttpRequestInfo &httpReqInfo		= ptHttpMsg->get_http_request_info();
  httpReqInfo.m_url					= strURL.c_str();
  httpReqInfo.m_contextId				= strContext.c_str();
  httpReqInfo.m_requestorModule		= ptHttpMsg->getSrcModuleId();
  httpReqInfo.m_contentType			= EN_CONTENT_TYPE_JSON;
  httpReqInfo.m_httpReqMethod			= tHttpmethod;
  httpReqInfo.m_data					= stData.c_str();
  return ptHttpMsg;
}

bool VMSCtrlDbHandler::Handle_Request_user_info_msg(string tcallID, string &tuserInfo) 
{
  string strSsrvURL = VMS_CONFIG::instance()->m_vmsControllerProfile.m_db_UsrInfoURL;
  std::ostringstream oss;
  oss << "{\"Vm_username\":\"" << tuserInfo.c_str() << "\"}";

  xGateHttpServiceMsg *ptHttpMSg = getHttpReuestMsg(tcallID,strSsrvURL, EN_HTTP_METHOD_POST, oss.str());

  if(!ptHttpMSg) {
    XGLOG_ERROR("OOM Issue. Creating HTTP request for validate VBox info failed");
    return false;
  }
  XGLOG_INFO("VMS Controller sending VBox details request to HTTP Module [%s]", strSsrvURL.c_str());

  string data = oss.str();
  XGLOG_DEBUG("Received http response data is [%s]", data.c_str());
  VMSCONTROLLER_TASK::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(ptHttpMSg));
  return true;
}

bool VMSCtrlDbHandler::Handle_Request_Validate_Extn(string tcallID, VMSUserPinInfo &tpinInfo) 
{
  string strSsrvURL = VMS_CONFIG::instance()->m_vmsControllerProfile.m_db_ValidateExtnURL;
  std::ostringstream oss;
  oss << "{\"extension\":\"" << tpinInfo.extnToValidate.c_str() << "\""<< ","
    << "\"domain_name\":\"" << tpinInfo.domainName.c_str() << "\"}";

  xGateHttpServiceMsg *ptHttpMSg = getHttpReuestMsg(tcallID,strSsrvURL, EN_HTTP_METHOD_POST, oss.str());

  if(!ptHttpMSg) {
    XGLOG_ERROR("OOM Issue. Creating HTTP request for validate User extension info failed");
    return false;
  }
  XGLOG_INFO("VMS Controller sending Extension validate details request to HTTP Module [%s]", strSsrvURL.c_str());

  string data = oss.str();
  XGLOG_DEBUG("Received http response data is [%s]", data.c_str());
  VMSCONTROLLER_TASK::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(ptHttpMSg));
  return true;
}

bool VMSCtrlDbHandler::Handle_Request_Validate_Pin(string tcallID, VMSUserPinInfo &tpinInfo) 
{
  string strSsrvURL = VMS_CONFIG::instance()->m_vmsControllerProfile.m_db_ValidatePinURL;
  std::ostringstream oss;
  oss << "{\"Vm_username\":\"" << tpinInfo.Vm_username.c_str() << "\""<< ","
    << "\"Vm_User_pin\":\"" << tpinInfo.pinToValidate.c_str() << "\"}";

  xGateHttpServiceMsg *ptHttpMSg = getHttpReuestMsg(tcallID,strSsrvURL, EN_HTTP_METHOD_POST, oss.str());

  if(!ptHttpMSg) {
    XGLOG_ERROR("OOM Issue. Creating HTTP request for validate User Pin info failed");
    return false;
  }
  XGLOG_INFO("VMS Controller sending Pin validate details request to HTTP Module [%s]", strSsrvURL.c_str());

  string data = oss.str();
  XGLOG_DEBUG("Received http response data is [%s]", data.c_str());
  VMSCONTROLLER_TASK::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(ptHttpMSg));
  return true;
}

bool VMSCtrlDbHandler::Handle_Request_save_vamil_info(xGateBaseMsg *pMsg, VMSVoiceMsgInfo &tinfo) 
{
  string strSsrvURL = VMS_CONFIG::instance()->m_vmsControllerProfile.m_db_VMSaveURL;
  string strId=pMsg->getUid(); 
  std::ostringstream oss;

  /*
     {
  //"vm_user_id": "string",
  //"vm_user_id": "int",
  "vms_server_id": "string",
  "cid_name": "string",
  "cid_number": "string",
  "vmail_server_url": "string",
  "file_path": "string",
  "NFSPath": "string",
  "message_len": "string",
  "ReadFlag": "string",
  "Archived_flag": "string",
  "deleted_flag": "string",
  "Forwarded_by": "string"
  }
  */

  /*
     << "\"created_date\"" << ":" <<"\"" << tinfo.created_date.c_str() <<"\"" << "," \
     << "\"Vm_uuid\"" << ":" <<"\"" << tinfo.Vm_uuid.c_str() <<"\"" << "," \
     */

  oss << "{"<< "\"vm_user_id\"" << ":" << tinfo.vm_user_id <<"," \
    << "\"Vm_uuid\"" << ":"  <<"\"" << tinfo.Vm_uuid.c_str() <<"\"" << "," \
    << "\"vms_server_id\"" << ":"  <<"\"" << tinfo.vms_server_id.c_str() <<"\"" << "," \
    << "\"domain_id\"" << ":" << tinfo.domain_id << "," \
    << "\"vbox_number\"" << ":" << "\""  << tinfo.vbox_number.c_str() << "\"" << "," \
    << "\"cid_name\"" << ":" << "\""  << tinfo.cid_name.c_str() << "\"" << "," \
    << "\"cid_number\"" << ":" << "\""  << tinfo.cid_number.c_str() << "\"" << "," \
    << "\"vmail_server_url\"" << ":" << "\""  << tinfo.vmail_server_url.c_str()<< "\"" << "," \
    << "\"file_path\"" << ":" << "\"" << tinfo.file_path.c_str() << "\"" << "," \
    << "\"NFSPath\"" << ":" <<"\"" << tinfo.NFSPath.c_str() <<"\"" << "," \
    << "\"message_len\"" << ":" << "\""  << tinfo.message_len.c_str()<< "\"" << "," \
    << "\"ReadFlag\"" << ":" << "\""  << tinfo.ReadFlag.c_str() << "\"" << "," \
    << "\"Archived_flag\"" << ":" << "\""  << tinfo.Archived_flag.c_str() << "\"" << "," \
    << "\"deleted_flag\"" << ":" << "\""  << tinfo.deleted_flag.c_str() << "\"" << "," \
    << "\"Forwarded_by\"" << ":" << "\""  << tinfo.Forwarded_by.c_str() << "\"" << "," \
    << "\"created_date\"" << ":" << "\""  << tinfo.created_date.c_str() << "\"" << "}";

  std::string data=oss.str();

  XGLOG_DEBUG("VMSCtrlDbHandler::Handle_Request_save_vmail_info data is %s !",data.c_str());

  xGateHttpServiceMsg *ptHttpMsg = getHttpReuestMsg(	strId,
      strSsrvURL, 
      EN_HTTP_METHOD_POST, 
      oss.str());

  if (!ptHttpMsg)
  {
    XGLOG_ERROR("OOM Issue. Creating HTTP request failed");
    return false;
  }
  XGLOG_INFO("VMS Controller sending request to HTTP Module [%s]", strSsrvURL.c_str());

  VMSCONTROLLER_TASK::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(ptHttpMsg));

  return true;
}

bool VMSCtrlDbHandler::Handle_response_srvr_info_msg(xGateHttpServiceMsg* pthttpSrvMsg, VMSServerInfo & tserverInfo) {

  xGateHttpServiceMsg* ptHttpMsg = dynamic_cast<xGateHttpServiceMsg*>(pthttpSrvMsg);
  HttpRequestInfo &tHttpResponse = pthttpSrvMsg->get_http_request_info();

  if (EN_HTTP_STATUS_SUCCESS != tHttpResponse.m_httpRespStatus)
  {
    XGLOG_ERROR("Reading Server deatils failed. HTTP ERROR response (%s)", tHttpResponse.m_output.c_str());
    return false;
  }

  if (!readServ_infofromJson(tHttpResponse.m_output.c_str(),
        tHttpResponse.m_output.length(), tserverInfo))
  {
    XGLOG_ERROR("Reading Server deatils failed. Creating HTTP request failed");
    return false;
  }

  return true;
}
bool VMSCtrlDbHandler::Handle_response_user_info_msg(xGateHttpServiceMsg* pthttpSrvMsg, VMSBoxUserInfo &tuserInfo, string &strID) {


  HttpRequestInfo &tHttpResponse = pthttpSrvMsg->get_http_request_info();
  strID = tHttpResponse.m_contextId.c_str();
  if (EN_HTTP_STATUS_SUCCESS != tHttpResponse.m_httpRespStatus)
  {
    XGLOG_ERROR("Reading User details failed. HTTP ERROR response (%s)", tHttpResponse.m_output.c_str());
    return false;
  }

  if (!readUser_infofromJson(tHttpResponse.m_output.c_str(),
        tHttpResponse.m_output.length(), tuserInfo))
  {
    XGLOG_ERROR("Reading User details failed. Creating HTTP request failed");
    return false;
  }
  return true;
}

bool VMSCtrlDbHandler::Handle_response_Validate_Extn(xGateHttpServiceMsg* pthttpSrvMsg, VMSUserPinInfo &tpinInfo, string &strID) {


  HttpRequestInfo &tHttpResponse = pthttpSrvMsg->get_http_request_info();
  strID = tHttpResponse.m_contextId.c_str();
  if (EN_HTTP_STATUS_SUCCESS != tHttpResponse.m_httpRespStatus)
  {
    XGLOG_ERROR("Reading Extn details failed. HTTP ERROR response (%s)", tHttpResponse.m_output.c_str());
    return false;
  }

  if (!readValidateExtn_infofromJson(tHttpResponse.m_output.c_str(),\
        tHttpResponse.m_output.length(), tpinInfo))
  {
    XGLOG_ERROR("Reading Extn details failed. Creating HTTP request failed");
    return false;
  }
  return true;
}

bool VMSCtrlDbHandler::Handle_response_Validate_Pin(xGateHttpServiceMsg* pthttpSrvMsg, VMSUserPinInfo &tpinInfo, string &strID) {


  HttpRequestInfo &tHttpResponse = pthttpSrvMsg->get_http_request_info();
  strID = tHttpResponse.m_contextId.c_str();
  if (EN_HTTP_STATUS_SUCCESS != tHttpResponse.m_httpRespStatus)
  {
    XGLOG_ERROR("Reading Pin details failed. HTTP ERROR response (%s)", tHttpResponse.m_output.c_str());
    return false;
  }

  if (!readValidatePin_infofromJson(tHttpResponse.m_output.c_str(),\
        tHttpResponse.m_output.length(), tpinInfo))
  {
    XGLOG_ERROR("Reading Pin details failed. Creating HTTP request failed");
    return false;
  }
  return true;
}

bool VMSCtrlDbHandler::readServ_infofromJson(const char  *data, unsigned int dataLen, VMSServerInfo & tserverInfo)
{
  if (!data && dataLen <= 0) {

    XGLOG_ERROR("Server Config data error from Database failed. data is empty!");
    return false;
  }
  /*
     {
     "status_code": 0,
     "err_code": 0,
     "affected_rows": 0,
     "message": "string",
     "output": {
     "Serverid": "string",
     "Max_call_load": "string",
     "vms_srv_local_weburl": "string",
     "Vms_srv_sip_login_number": "string",
     "vms_srv_sip_login_password": "string",
     "Vms_registrar_sip_url": "string",
     "Vms_registration_enabled": "string",
     "Vms_server_enabled": "string",
     "Vms_server_preference_order": "string",
     "Direct_ext_dial_num": "string",
     "Foreign_ext_dial_num": "string",
     "Disa_number": "string",
     "Disa_enabled": "string",
     "Vms_call_max_duration": "string",
     "Vms_max_store_duration": "string",
     "Vms_max_del_store_duration": "string"
     }
     }
     */

  Document doc;

  XGLOG_INFO( "PARSING Server info data: %s", data);

  ParseResult ok = doc.Parse(data);
  if (!ok)
  {
    XGLOG_ERROR( "Server Config data error from Database . Encountered with json parse error !");
    return false;
  }
  else if (!doc.IsObject())
  {
    XGLOG_ERROR( "Server Config data error from Database . Not able create documnet object !");
    return false;
  }

  XGLOG_INFO("successfully parsed Server Config data from Database");

  if (doc.HasMember("status_code"))
  {
    tserverInfo.m_StatusCode = doc["status_code"].GetInt();
  }
  else 
  {
    XGLOG_ERROR( "Server Config data error from Database. 'uid' is missing in business json message !");
    return false;
  }
  if (doc.HasMember("err_code"))
  {
    tserverInfo.m_errorcode = doc["err_code"].GetInt();
  }
  else
  {
    XGLOG_ERROR("Server Config data error from Database. 'err_code' is missing in business json message !");
    return false;
  }

  if (doc.HasMember("affected_rows"))
  {
    tserverInfo.affectedrows = doc["affected_rows"].GetInt();
  }
  else
  {
    XGLOG_ERROR("Server Config data error from Database. 'affected_rows' is missing in business json message !");
    return false;
  }

  if (tserverInfo.affectedrows < 1)
  {
    XGLOG_ERROR("Server Config data error from Database. 'affected_rows' is missing in business json message !");
    return false;
  }

  if (doc.HasMember("message"))
  {
    tserverInfo.m_message = doc["message"].GetString();
  }
  else
  {
    XGLOG_ERROR( "Server Config data error from Database. 'opcode' is missing in json message !");
    return false;
  }

  Value val; // results
  if (!doc.HasMember("output"))
  {
    XGLOG_ERROR("Server Config data error from Database. output is missing in json message !");
    return false;
  }
  val = doc["output"].GetObject();

  if (!val.HasMember("Serverid"))
  {
    XGLOG_ERROR("Server Config data error from Database. output is missing in json message !");
    return false;
  }
  //tserverInfo.m_ServerId = getIntValue(val["Serverid"].GetString());
  tserverInfo.m_ServerId = val["Serverid"].GetString();

  if (!val.HasMember("Max_call_load"))
  {
    XGLOG_ERROR("Server Config data error from Database. Max_call_load is missing in json message !");
    return false;
  }

  tserverInfo.m_Max_call_load = getIntValue(  val["Max_call_load"]. GetString());

  if (!val.HasMember("vms_srv_local_weburl"))
  {
    XGLOG_ERROR("Server Config data error from Database. vms_srv_local_weburl is missing in json message !");
    return false;
  }
  tserverInfo.m_vms_srv_local_weburl = val["vms_srv_local_weburl"].GetString();

  if (!val.HasMember("Vms_srv_sip_login_number"))
  {
    XGLOG_ERROR("Server Config data error from Database. Vms_srv_sip_login_number is missing in json message !");
    return false;
  }
  tserverInfo.m_Vms_srv_sip_login_number = val["Vms_srv_sip_login_number"].GetString();

  if (!val.HasMember("vms_srv_sip_login_password"))
  {
    XGLOG_ERROR("Server Config data error from Database. vms_srv_sip_login_password is missing in json message !");
    return false;
  }
  tserverInfo.m_vms_srv_sip_login_password = val["vms_srv_sip_login_password"].GetString();

  if (!val.HasMember("Vms_registrar_sip_url"))
  {
    XGLOG_ERROR("Server Config data error from Database. Vms_registrar_sip_url is missing in json message !");
    return false;
  }
  tserverInfo.m_Vms_registrar_sip_url = val["Vms_registrar_sip_url"].GetString();

  if (!val.HasMember("Vms_registration_enabled"))
  {
    XGLOG_ERROR("Server Config data error from Database. Vms_registration_enabled is missing in json message !");
    return false;
  }
  tserverInfo.m_Vms_registration_enabled = getIntValue( val["Vms_registration_enabled"].GetString());

  if (!val.HasMember("Vms_server_enabled"))
  {
    XGLOG_ERROR("Server Config data error from Database. Vms_server_enabled is missing in json message !");
    return false;
  }
  tserverInfo.m_Vms_server_enabled = val["Vms_server_enabled"].GetString();

  if (!val.HasMember("Vms_server_preference_order"))
  {
    XGLOG_ERROR("Server Config data error from Database. Vms_server_preference_order is missing in json message !");
    return false;
  }
  tserverInfo.m_Vms_server_preference_order = getIntValue(val["Vms_server_preference_order"].GetString());

  if (!val.HasMember("Direct_ext_dial_num"))
  {
    XGLOG_ERROR("Server Config data error from Database. Direct_ext_dial_num is missing in json message !");
    return false;
  }
  tserverInfo.m_Direct_ext_dial_num = val["Direct_ext_dial_num"].GetString();

  if (!val.HasMember("Foreign_ext_dial_num"))
  {
    XGLOG_ERROR("Server Config data error from Database. Foreign_ext_dial_num is missing in json message !");
    return false;
  }
  tserverInfo.m_Foreign_ext_dial_num = val["Foreign_ext_dial_num"].GetString();

  if (!val.HasMember("Disa_number"))
  {
    XGLOG_ERROR("Server Config data error from Database. Disa_number is missing in json message !");
    return false;
  }
  tserverInfo.m_Disa_number = val["Disa_number"].GetString();

  if (!val.HasMember("Disa_enabled"))
  {
    XGLOG_ERROR("Server Config data error from Database. Disa_enabled is missing in json message !");
    return false;
  }
  tserverInfo.m_Disa_enabled = getIntValue( val["Disa_enabled"].GetString());

  if (!val.HasMember("Vms_call_max_duration"))
  {
    XGLOG_ERROR("Server Config data error from Database. Vms_call_max_duration is missing in json message !");
    return false;
  }
  tserverInfo.m_Vms_call_max_duration = getIntValue(val["Vms_call_max_duration"].GetString());

  if (!val.HasMember("Vms_max_store_duration"))
  {
    XGLOG_ERROR("Server Config data error from Database. Vms_max_store_duration is missing in json message !");
    return false;
  }
  tserverInfo.m_Vms_max_store_duration = getIntValue(val["Vms_max_store_duration"].GetString());

  if (!val.HasMember("Vms_max_del_store_duration"))
  {
    XGLOG_ERROR("Server Config data error from Database. Vms_max_del_store_duration is missing in json message !");
    return false;
  }
  tserverInfo.m_Vms_max_del_store_duration = getIntValue(val["Vms_max_del_store_duration"].GetString());

  return true;
}

bool VMSCtrlDbHandler::readUser_infofromJson(const char  *data, unsigned int dataLen, VMSBoxUserInfo &tuserInfo)
{

  Document doc;

  XGLOG_INFO( "PARSING User info data: %s", data);

  ParseResult ok = doc.Parse(data);
  if (!ok)
  {
    XGLOG_ERROR( "User Config data error from Database . Encountered with json parse error !");
    return false;
  }
  else if (!doc.IsObject())
  {
    XGLOG_ERROR( "User Config data error from Database . Not able create documnet object !");
    return false;
  }

  XGLOG_INFO("successfully parsed User Config data from Database");


  if (doc.HasMember("status_code"))
  {
    tuserInfo.m_StatusCode = doc["status_code"].GetInt();
  }
  else
  {
    XGLOG_ERROR( "User Config data error from Database. 'uid' is missing in business json message !");
    return false;
  }
  if (doc.HasMember("err_code"))
  {
    tuserInfo.m_errorcode = doc["err_code"].GetInt();
  }
  else
  {
    XGLOG_ERROR("User Config data error from Database. 'err_code' is missing in business json message !");
    return false;
  }

  if (doc.HasMember("affected_rows"))
  {
    tuserInfo.affectedrows = doc["affected_rows"].GetInt();
  }
  else
  {
    XGLOG_ERROR("User Config data error from Database. 'affected_rows' is missing in business json message !");
    return false;
  }

  if (tuserInfo.affectedrows < 1)
  {
    XGLOG_ERROR("user Config data error from Database. 'affected_rows' is missing in business json message !");
    return false;
  }

  if (doc.HasMember("message"))
  {
    tuserInfo.m_message = doc["message"].GetString();
  }
  else
  {
    XGLOG_ERROR( "User Config data error from Database. 'opcode' is missing in json message !");
    return false;
  }

  Value val; // results
  if (!doc.HasMember("output"))
  {
    XGLOG_ERROR("User Config data error from Database. output is missing in json message !");
    return false;
  }
  val = doc["output"].GetObject();

  if (!val.HasMember("domain_id"))
  {
    XGLOG_ERROR("User Config data error from Database. domain_id is missing in json message !");
    return false;
  }
  tuserInfo.domain_id = val["domain_id"].GetInt();


  if (!val.HasMember("vm_user_id"))
  {
    XGLOG_ERROR("User Config data error from Database. vm_user_id is missing in json message !");
    return false;
  }
  //tuserInfo.vm_user_id = getIntValue(val["vm_user_id"].GetString());
  tuserInfo.vm_user_id = val["vm_user_id"].GetInt();

  if (!val.HasMember("Vm_username"))
  {
    XGLOG_ERROR("User Config data error from Database. Vm_username is missing in json message !");
    return false;
  }
  tuserInfo.Vm_username = val["Vm_username"].GetString();

  if (!val.HasMember("Vm_User_pin"))
  {
    XGLOG_ERROR("User Config data error from Database. Vm_User_pin is missing in json message !");
    return false;
  }
  tuserInfo.Vm_User_pin = val["Vm_User_pin"].GetString();

  if (!val.HasMember("User_vmail_greeings_voice_url"))
  {
    XGLOG_ERROR("User Config data error from Database. User_vmail_greeings_voice_url is missing in json message !");
    return false;
  }
  tuserInfo.User_vmail_greeings_voice_url = val["User_vmail_greeings_voice_url"].GetString();


  /*
     {"status_code":200,"err_code":0,"affected_rows":1,"message":"ok","output":{"vm_user_id":"2","Vm_username":"303@6614.UR.mundio.com","Vm_User_pin":"875611","User_vmail_greeings_voice_url":"http://urmyaccount.mundio.com/abc.wav","enabled_vms_email":"test1@mundio.com","Enabled_vms_transcript":"1","Language_pref_id":"2"}}
     */

  if (!val.HasMember("enabled_vms_email"))
  {
    XGLOG_ERROR("User Config data error from Database. enabled_vms_email is missing in json message !");
    return false;
  }
  tuserInfo.enabled_vms_email = getIntValue(val["enabled_vms_email"].GetString());

  if (!val.HasMember("Enabled_vms_transcript"))
  {
    XGLOG_ERROR("User Config data error from Database. Enabled_vms_transcript is missing in json message !");
    return false;
  }
  tuserInfo.Enabled_vms_transcript = getIntValue(val["Enabled_vms_transcript"].GetString());

  if (!val.HasMember("Language_pref_id"))
  {
    XGLOG_ERROR("User Config data error from Database. Language_pref_id is missing in json message !");
    return false;
  }
  tuserInfo.Language_pref_id = getIntValue(val["Language_pref_id"].GetString());

  XGLOG_INFO("successfully parsed and got values for User Config data from Database");
  return true;
}

bool VMSCtrlDbHandler::readValidateExtn_infofromJson(const char  *data, unsigned int dataLen, VMSUserPinInfo &textnInfo)
{

  Document doc;

  XGLOG_INFO( "PARSING Extn info data: %s", data);

  ParseResult ok = doc.Parse(data);
  if (!ok)
  {
    XGLOG_ERROR( "Extn Config data error from Database . Encountered with json parse error !");
    return false;
  }
  else if (!doc.IsObject())
  {
    XGLOG_ERROR( "Extn Config data error from Database . Not able create documnet object !");
    return false;
  }

  XGLOG_INFO("successfully parsed Pin Config data from Database");

  if (doc.HasMember("status_code"))
  {
    textnInfo.m_StatusCode = doc["status_code"].GetInt();
  }
  else
  {
    XGLOG_ERROR( "Extn Config data error from Database. 'uid' is missing in business json message !");
    return false;
  }
  if (doc.HasMember("err_code"))
  {
    textnInfo.m_errorcode = doc["err_code"].GetInt();
  }
  else
  {
    XGLOG_ERROR("Extn Config data error from Database. 'err_code' is missing in business json message !");
    return false;
  }

  if (doc.HasMember("affected_rows"))
  {
    textnInfo.affectedrows = doc["affected_rows"].GetInt();
  }
  else
  {
    XGLOG_ERROR("Extn Config data error from Database. 'affected_rows' is missing in business json message !");
    return false;
  }

  if (doc.HasMember("message"))
  {
    textnInfo.m_message = doc["message"].GetString();
  }
  else
  {
    XGLOG_ERROR( "Extn Config data error from Database. 'opcode' is missing in json message !");
    return false;
  }

  if (!doc.HasMember("Is_extension_valid"))
  {
    XGLOG_ERROR("Extn Config data error from Database. Is_pin_valid is missing in json message !");
    return false;
  }
  textnInfo.isValidExtn = doc["Is_extension_valid"].GetInt();

  XGLOG_INFO("successfully parsed and got values for Extn Config data from Database");
  return true;
}

bool VMSCtrlDbHandler::readValidatePin_infofromJson(const char  *data, unsigned int dataLen, VMSUserPinInfo &tpinInfo)
{

  Document doc;

  XGLOG_INFO( "PARSING Pin info data: %s", data);

  ParseResult ok = doc.Parse(data);
  if (!ok)
  {
    XGLOG_ERROR( "Pin Config data error from Database . Encountered with json parse error !");
    return false;
  }
  else if (!doc.IsObject())
  {
    XGLOG_ERROR( "Pin Config data error from Database . Not able create documnet object !");
    return false;
  }

  XGLOG_INFO("successfully parsed Pin Config data from Database");

  if (doc.HasMember("status_code"))
  {
    tpinInfo.m_StatusCode = doc["status_code"].GetInt();
  }
  else
  {
    XGLOG_ERROR( "Pin Config data error from Database. 'uid' is missing in business json message !");
    return false;
  }
  if (doc.HasMember("err_code"))
  {
    tpinInfo.m_errorcode = doc["err_code"].GetInt();
  }
  else
  {
    XGLOG_ERROR("Pin Config data error from Database. 'err_code' is missing in business json message !");
    return false;
  }

  if (doc.HasMember("affected_rows"))
  {
    tpinInfo.affectedrows = doc["affected_rows"].GetInt();
  }
  else
  {
    XGLOG_ERROR("Pin Config data error from Database. 'affected_rows' is missing in business json message !");
    return false;
  }

  if (doc.HasMember("message"))
  {
    tpinInfo.m_message = doc["message"].GetString();
  }
  else
  {
    XGLOG_ERROR( "Pin Config data error from Database. 'opcode' is missing in json message !");
    return false;
  }


  if (!doc.HasMember("Is_pin_valid"))
  {
    XGLOG_ERROR("Pin Config data error from Database. Is_pin_valid is missing in json message !");
    return false;
  }
  //tuserInfo.vm_user_id = getIntValue(val["vm_user_id"].GetString());
  tpinInfo.isValidPin = doc["Is_pin_valid"].GetInt();

  XGLOG_INFO("successfully parsed and got values for Pin Config data from Database");
  return true;
}

#if 0 
Value VMSCtrlDbHandler::setJsonParam(string &strValue, Document &doc)
{
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
  return retVal;
}

Value VMSCtrlDbHandler::setJsonParam(unsigned int iValue, Document &doc)
{
  Value retVal;
  retVal = iValue;
  return retVal;
}



bool VMSCtrlDbHandler::process_msg(ACE_Message_Block *pAmb)
{
  if(pAmb) {
    xGateBaseMsg *pBaseMsg = NULL;
    pBaseMsg = dynamic_cast<xGateBaseMsg*>(pAmb);
    switch(pBaseMsg->getOpCode()) {
      case VMSCONTROLLER_CTRL_INT_SRV_INFO:
        {
          process_vmsctrl_int_srvr_info_msg(pBaseMsg);
          break;
        }

      case VMSCONTROLLER_CTRL_INT_USR_INFO:
        {
          process_vmsctrl_int_user_info_msg(pBaseMsg);
          break;
        }
      case VMSCONTROLLER_CTRL_INT_SAV_VMAIL_INFO:
        {
          process_vmsctrl_int_save_vamil_info(pBaseMsg);
          break;
        }

    }
  }
  return true;
}


bool VMSCtrlDbHandler::process_vmsctrl_int_srvr_info_msg()
{
  std::ostringstream oss;
  oss << "{"<< "\"Server\"" << ":" <<"\""<< Server << "\""<<"," \
    << "\"Server_dns\"" << ":"  <<"\"" << Server_dns.c_str() <<"\"" << "," \
    << "\"Server_ip\"" << ":" <<"\"" <<  Server_ip.c_str()<<"\"" << "," \
    << "\"Server_mac\"" << ":" << "\""  << Server_mac.c_str() << "\"" << "," \
    << "\"present_call_load\"" << ":" << "\""  << present_call_load << "\"" << "," \
    << "\"Max_call_load\"" << ":" << "\""  <<Max_call_load << "\"" << "," \
    << "\"vms_srv_local_weburl\"" << ":" << "\""  <<vms_srv_local_weburl.c_str()<< "\"" << "," \
    << "\"Vms_srv_sip_login_number\"" << ":" << "\""  <<Vms_srv_sip_login_number.c_str() << "\"" << "," \
    << "\"vms_srv_sip_login_password\"" << ":"  <<vms_srv_sip_login_password.c_str() << "," \
    << "\"Vms_registrar_sip_url\"" << ":" <<"\"" <<  Vms_registrar_sip_url.c_str()<<"\"" << "," \
    << "\"Vms_registration_enabled\"" << ":" << "\""  << Vms_registration_enabled<< "\"" << "," \
    << "\"Vms_server_enabled\"" << ":" << "\""  << Vms_server_enabled.c_str() << "\"" << "," \
    << "\"Vms_server_preference_order\"" << ":" << "\""  <<Vms_server_preference_order << "\"" << "," \
    << "\"Direct_ext_dial_num\"" << ":" << "\""  <<Direct_ext_dial_num.c_str()<< "\"" << "," \
    << "\"Foreign_ext_dial_num\"" << ":" << "\""  <<Foreign_ext_dial_num.c_str()<< "\"" << "," \
    << "\"Disa_number\"" << ":"  <<Disa_number.c_str() << "," \
    << "\"Disa_enabled\"" << ":"  <<Disa_enabled << "," \
    << "\"Vms_call_max_duration\"" << ":" <<"\"" << Vms_call_max_duration<<"\"" << "," \
    << "\"Vms_max_store_duration\"" << ":" << "\""  << Vms_max_store_duration<< "\"" << "," \
    << "\"Vms_max_del_store_duration\"" << ":" << "\""  <<Vms_max_del_store_duration << "\"" << "}";

  return true;
}


bool VMSCtrlDbHandler::process_vmsctrl_int_user_info_msg()
{
  std::ostringstream oss;


  return true;
}
bool VMSCtrlDbHandler::process_vmsctrl_int_save_vamil_info()
{
  std::ostringstream oss;




  return true;
}

bool VMSCtrlDbHandler::stop(){
  m_run = false;
  msg_queue()->deactivate();
  return true;
}

bool VMSCtrlDbHandler::post_to_mongodb(xGateBaseMsg *pMsg){

  if(!pMsg) {
    XGLOG_ERROR("post_to_mongodb failed. BaseMsg pointer object is invalid !");
    return false;
  }
  HttpRequestInfo &httpReqInfo = pMsg->get_http_request_info();
  httpReqInfo.m_url = NODEJS_URL;
  httpReqInfo.m_requestId = m_uid;

  return true;
}
#endif
