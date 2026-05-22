//local includes
#include "xglog.h"
#include "xGateBusinessProcessor.h"
#include "xGateBusinessHandler.h"
#include "xGateBusinessUtil.h"

#define THISMODULE "BusiHandler"

xGateBusinessHandler::xGateBusinessHandler() : m_doc(NULL), m_strBuf(NULL) 
{
  m_doc = new Document();
  m_strBuf = new StringBuffer();
  readTemplate();
  XGLOG_INFO( "xGateBusinessHandler::xGateBusinessHandler");
}

xGateBusinessHandler::~xGateBusinessHandler(void)
{
  if(m_doc) delete m_doc;
  if(m_strBuf) delete m_strBuf;
  XGLOG_INFO( "xGateBusinessHandler::~xGateBusinessHandler");
}

bool xGateBusinessHandler::readTemplate()
{
  //read the template and make it as json document
  ParseResult ok = m_doc->Parse(json_business_tmpl.c_str());
  if(!ok) {
    XGLOG_ERROR( "xGateBusinessHandler::read template failed due to json parse error: %s !", \
        GetParseError_En(ok.Code()));
    return false;
  } else if(!m_doc->IsObject()) {
    XGLOG_ERROR( "xGateBusinessHandler::read template failed. not able to create document object from json_business template !");
    return false;
  }
  XGLOG_INFO( "parsed json_business template successfully");
  return true;
}

Value setJsonParam(string strValue,Document *doc )
{
  Value retVal;
  retVal.SetString(strValue.c_str(), strValue.length(), doc->GetAllocator());
  return retVal;
}

Value setJsonParam(unsigned int iValue, Document *doc)
{
  Value retVal;
  retVal = iValue;
  return retVal;
}

bool xGateBusinessHandler::createRequest(callInfoParameter &cp, xGateBusinessSigEvent event)
{
  m_strBuf->Clear();
  Writer<StringBuffer> writer(*m_strBuf);
  (*m_doc)["uid"] = setJsonParam(cp.uid, m_doc);
  (*m_doc)["opcode"] = setJsonParam(event, m_doc);
  if(!formInput(cp, event)) {
    return false;
  }
  m_doc->Accept(writer);

  if(!postRequest()) {
    return false;
  }
  return true;
}

bool xGateBusinessHandler::formInput(callInfoParameter &cp, xGateBusinessSigEvent event)
{
  std::ostringstream oss;

  switch(event) {
    case EN_BUSINESS_EVENT_INIT:
    {
      //TODO: instead of forming input using ostream, try to use rapidjson AddMember api
      oss << "{" << "\"cli\"" << ":" << "\"" << cp.cnCalling << "\"" << "," \
        << "\"ddi\"" << ":" << "\""  << cp.cnCalled << "\"" << "," \
        << "\"domain_name\"" << ":" << "\""  << cp.routeAddr << "\"" << "}";
      break;
    } 
    case EN_BUSINESS_EVENT_FORKING:
    {
      oss << "{" << "\"ddi\"" << ":" << "\"" << cp.cnCalled << "\"" << "," \
        << "\"domain_name\"" << ":" << "\""  << cp.routeAddr << "\"" << "}";
      break;
    }
    case EN_BUSINESS_EVENT_CDRUPDATE:
    oss << cp.cdrJsonString;
    printf("CDR json ---- %s",cp.cdrJsonString.c_str()); 
    break;
    case EN_BUSINESS_EVENT_UNKNOWN:
    default:
    {
      return false;
    } 
  }

  Document inputDoc(&m_doc->GetAllocator());
  string input = oss.str();
  if(getInputDoc(input, inputDoc)) {
    m_doc->RemoveMember("input"); //remove existing input object if any
    m_doc->AddMember("input", inputDoc, m_doc->GetAllocator());
  }
  return true;
}

bool xGateBusinessHandler::getInputDoc(string &input, Document &doc)
{
  if(input.empty()) {
    XGLOG_ERROR( "getInputDoc failed. given input vaule is empty!");
    return false;
  }

  ParseResult ok = doc.Parse(input.c_str());
  if(!ok) {
    XGLOG_ERROR( "getInputDoc failed due to json parse error: %s !", \
        GetParseError_En(ok.Code()));
    return false;
  } else if(!doc.IsObject()) {
    XGLOG_ERROR( "getInputDoc failed to create json object from given input: %s!", \
        input.c_str());
    return false;
  }
  return true;
}

bool xGateBusinessHandler::getOutputDoc(Document &doc, const Value &outputVal)
{
  if(doc.HasMember("output")) {
    outputVal = doc["output"].GetObject();
    return true;
  }
  return false;
}

bool xGateBusinessHandler::postRequest() 
{
  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = xGateUtil::getBusinessProcessor();
  if(!taskPtr) {
    XGLOG_ERROR( "post failed. BusinessProccessor taksptr is NULL !");
    return false;
  }

#if 0 //for debugging purpose
  string data(m_strBuf->GetString());
  printf("****** business request info: %s\n", data.c_str());
#endif

  int len = m_strBuf->GetLength();
  char* data = new char[len+1];
  memset(data, 0, (len+1));
  memcpy (data, m_strBuf->GetString(), len);
  data[len] = '\0';

  xGateBusinessMsg *pMsg = new xGateBusinessMsg();
  pMsg->set_business_msg_type(EN_BUSINESS_SIGLAYER_MSG);
  pMsg->set_data(data);
  pMsg->set_data_len(len);

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateBusinessMsg*>(pMsg);
  ACE_Time_Value tval ((time(NULL)));
  tval += ACE_Time_Value(0,1);

  if(taskPtr->putq(pAmb, &tval)) {
    XGLOG_INFO( "successfully posted message to BusinessProcessor");
  } else {
    XGLOG_ERROR( "failed to post message to BusinessProcessor !");
    return false;
  }
  return true;
}

bool xGateBusinessHandler::processResponse(const char* data, xGateCallObj *callObj) 
{
  Document doc;
  if(!parseJsonMsg(data, doc)) {
    XGLOG_ERROR( "processResponse failed to parse business response message !");
    return false;
  }

  if(!fillDetail(doc, callObj)) {
    XGLOG_ERROR( "processResponse failed to fill business details in cp !");
    return false;
  }

  return true;
}

bool xGateBusinessHandler::parseJsonMsg(const char *jsonData, Document &doc)
{
  XGLOG_INFO( "parsing business json data: %s", jsonData);
  ParseResult ok = doc.Parse(jsonData);
  if(!ok) {
    XGLOG_ERROR( "parseJsonMsg encountered with json parse error !");
    return false;
  } else if(!doc.IsObject()) {
    XGLOG_ERROR( "parseJsonMsg not able create document object !");
    return false;
  }
  XGLOG_INFO( "parseJsonMsg successfully parsed business json data");
  return true;
}

bool xGateBusinessHandler::fillDetail(Document &doc, xGateCallObj *callObj)
{
  callInfoParameter &cp = callObj->cp;
  if(doc.HasMember("uid")) {
    callObj->uid = callObj->callid = cp.call_id = cp.uid = doc["uid"].GetString();
  } else {
    XGLOG_ERROR( "fillDetail failed. 'uid' is missing in business json message !");
    return false;
  }

  xGateBusinessSigEvent event = EN_BUSINESS_EVENT_UNKNOWN;
  if(doc.HasMember("opcode")) {
    event = doc["opcode"].GetInt();
  } else {
    XGLOG_ERROR( "fillDetail failed. 'opcode' is missing in json message !");
    return false;
  }
  
  unsigned int errcode = 0;
  if(doc.HasMember("errcode")) {
    errcode = doc["errcode"].GetInt();
  } else {
    XGLOG_ERROR( "fillDetail failed. 'errcode' is missing in json message !");
    return false;
  }

  const Value outputVal;
  if(!getOutputDoc(doc, outputVal)) {
    XGLOG_ERROR( "fillDetail failed. not able to get 'output' object from main json object !");
    return false;
  }

  switch(event) {
    case EN_BUSINESS_EVENT_INIT: //this reponse will contain both prequery as well aor result
      {
        fillBusinessInitDetail(outputVal, callObj);       
        break;
      } 
    case EN_BUSINESS_EVENT_FORKING:
      {
        fillForkingDetail(outputVal, callObj);
        break;
      }
    case EN_BUSINESS_EVENT_TIMER:
      {
        //handleTimeoutEvent(outputVal, callObj);
        break;
      }
    default:
      {
        XGLOG_ERROR( "fillDetail failed. EN_BUSINESS_EVENT_UNKNOWN !");
        false;
      } 
  }
  return true;
}

bool xGateBusinessHandler::fillBusinessInitDetail(const Value &val, xGateCallObj *callObj)
{
  callInfoParameter &cp = callObj->cp;
  //TODO: Yoga hardcoded this for testing purpose 
#if 1
  cp.calledDeviceType = "URDESK:"; //TODO: Yoga, why sofia CreateCallParam function not filling this?
  cp.gm_user_company_hour = 1; //TODO: Yoga, actually where we are using this. is this neccessary?
  cp.isconferenceCall = false;
  cp.isSwitchBoardCall = false;
  cp.isPstnNumber = false;
#endif
  if(val.HasMember("cli")) {
    cp.cnCalling = val["cli"].GetString();
  } else {
    XGLOG_ERROR( "fillBusinessInitDetail failed. 'cli' is missing in business json message !");
    return false;
  }

  if(val.HasMember("ddi")) {
    cp.cnCalled = val["ddi"].GetString();
  } else {
    XGLOG_ERROR( "fillBusinessInitDetail failed. 'ddi' is missing in business json message !");
    return false;
  }

  if(val.HasMember("domain_name")) {
    cp.routeAddr = val["domain_name"].GetString();
    cp.csaCalling = cp.routeAddr + ":5060";
    cp.csaCalled = cp.routeAddr + ":5060";
  } else {
    XGLOG_ERROR( "fillBusinessInitDetail failed. 'domain_name' is missing in business json message !");
    return false;
  }

  if(val.HasMember("domain_id")) {
    cp.gm_A_domain_id = cp.gm_B_domain_id = val["domain_id"].GetString();
  } else {
    XGLOG_ERROR( "fillBusinessInitDetail failed. 'domain_name' is missing in business json message !");
    return false;
  }

  if(val.HasMember("ivr_rbt")) {
    cp.rbtFile = val["ivr_rbt"].GetString();
  } else {
    XGLOG_ERROR( "fillBusinessInitDetail failed. 'ivr_rbt' is missing in business json message !");
    return false;
  }
  
  if(val.HasMember("ivr_user_greeting")) {
   cp.userGreetingFile = val["ivr_user_greeting"].GetString();
  } else {
    XGLOG_ERROR( "fillBusinessInitDetail failed. 'ivr_user_greeting' is missing in business json message !");
    return false;
  }

  if(val.HasMember("ivr_connecting_msg")) {
   cp.userConnectingFile = val["ivr_connecting_msg"].GetString();
  } else {
    XGLOG_ERROR( "fillBusinessInitDetail failed. 'ivr_connecting_msg' is missing in business json message !");
    return false;
  }

  if(val.HasMember("ivr_silence")) {
   cp.gm_user_greeting_file_name_user_B_After = val["ivr_silence"].GetString();
  } else {
    XGLOG_ERROR( "fillBusinessInitDetail failed. 'ivr_silence' is missing in business json message !");
    return false;
  }

  unsigned int callFeatureType = 0;
  if(val.HasMember("call_feature_type")) {
    callFeatureType   = val["call_feature_type"].GetInt();
    getSipEvent(callFeatureType, callObj); 
  } else {
    XGLOG_ERROR( "fillBusinessInitDetail failed. 'call_feature_type' is missing in business json message !");
    return false;
  }

  if(val.HasMember("is_forking")) {
    cp.isForkingenabled = val["is_forking"].GetInt();
  } else {
    XGLOG_ERROR( "fillBusinessInitDetail failed. 'call_feature_type' is missing in business json message !");
    return false;
  }

  if(!val.HasMember("mapped_user")) {
    XGLOG_ERROR( "fillBusinessInitDetail failed. 'mapped_user' record is missing in json message !");
    return false;
  }

  const Value &mappedUserRecords = val["mapped_user"];
  if(!fillMappedUsersInfo(mappedUserRecords, cp)) {
    XGLOG_ERROR( "fillBusinessInitDetail failed while filling mapped user info in call param !");
    return false;
  }

  return true;
}

bool xGateBusinessHandler::fillForkingDetail(const Value &val, xGateCallObj *callObj)
{
  callInfoParameter &cp = callObj->cp;
  if(!val.HasMember("mapped_user")) {
    XGLOG_ERROR( "fillForkingDetail failed. 'mapped_user' record is missing in json message !");
    return false;
  }

  const Value &mappedUserRecords = val["mapped_user"];
  if(!fillMappedUsersInfo(mappedUserRecords, cp)) {
    XGLOG_ERROR( "fillForkingDetail failed while filling mapped user info in call param !");
    return false;
  }
  callObj->sipeventtype = EN_FOKINGDIAL_REL;
  return true;
}

bool xGateBusinessHandler::fillMappedUsersInfo(const Value &val, callInfoParameter &cp)
{
  MappedUserList &mappedUserList = cp.m_mappedUserList;
  if(val.IsArray()) {
    Value::ConstValueIterator itr = val.Begin();
    for(itr; itr != val.End(); itr++) {
      const Value &aor = *itr;
      multiDevice *mappedUserInfo = new multiDevice();
      if(!fillMappedUserInfo(aor, mappedUserInfo)) {
        XGLOG_INFO( "fillMappedUsersInfo failed due to fillMappedUserInfo failure !");
        delete mappedUserInfo;
        return false;
      }
      string key = cp.cnCalled + "_" + mappedUserInfo->m_userDeviceType;  
      mappedUserInfo->m_callId = cp.call_id; 
      mappedUserInfo->m_userNumber = cp.cnCalled;
      mappedUserInfo->m_domainName = cp.routeAddr;
      //mappedUserInfo->m_domainId = cp.gm_B_domain_id;
      mappedUserList[key] = mappedUserInfo;
    }
  } else if(val.IsObject()) {
    multiDevice *mappedUserInfo = new multiDevice();
    if(!fillMappedUserInfo(val, mappedUserInfo)) {
      XGLOG_INFO( "fillMappedUsersInfo failed due to fillMappedUserInfo failure !");
      delete mappedUserInfo;
      return false;
    }
    string key = cp.cnCalled + "_" + mappedUserInfo->m_userDeviceType;  
    mappedUserInfo->m_callId = cp.call_id; 
    mappedUserInfo->m_userNumber = cp.cnCalled;
    mappedUserInfo->m_domainName = cp.routeAddr;
    //mappedUserInfo->m_domainId = cp.gm_B_domain_id;
    mappedUserList[key] = mappedUserInfo;
  }

  return true;
}

bool xGateBusinessHandler::fillMappedUserInfo(const Value &val, multiDevice *mappedUserInfo)
{
  if(!val.IsObject()) {
    return false;
  }

  if(val.HasMember("Contact_address")) {
    mappedUserInfo->m_userSignalAddr = val["Contact_address"].GetString();
  } else {
    XGLOG_ERROR( "fillMappedUserInfo failed. Contact_address is empty !");
    return false;
  }

  if(val.HasMember("Ipaddress")) {
    mappedUserInfo->m_routeIpAddress = val["Ipaddress"].GetString();
  } else {
    XGLOG_ERROR( "fillMappedUserInfo failed. Ipaddress is empty !");
    return false;
  }

  if(val.HasMember("proxy_username")) {
    mappedUserInfo->m_proxyUserName = val["proxy_username"].GetString();
  } else {
    XGLOG_ERROR( "fillMappedUserInfo failed. proxy_username is empty !");
    return false;
  }

  if(val.HasMember("device_type")) {
    mappedUserInfo->m_calledDeviceType = mappedUserInfo->m_userDeviceType = val["device_type"].GetString();
  } else {
    XGLOG_ERROR( "fillMappedUserInfo failed. device_type is empty !");
    return false;
  }

  getCodec(mappedUserInfo->m_calledDeviceType, mappedUserInfo->m_calledcodec);

  return true;
}

bool xGateBusinessHandler::getCodec(string &deviceType, int &codec)
{
  if(strstr(deviceType.c_str(),"PSTN") || strstr(deviceType.c_str(),"URDESK")) {
    codec = 8;
  } else {
    codec = 111;
  }
  return true;
}

//bool xGateBusinessHandler::getSipEvent(unsigned int &callFeatureType, XGATESIPEVENT& sipEvent)
bool xGateBusinessHandler::getSipEvent(unsigned int &callFeatureType, xGateCallObj *callObj)
{
  callInfoParameter &cp = callObj->cp; 
  switch(callFeatureType) {
    case EN_CALL_FEATURE_TYPE_INTERNAL:
    {
      callObj->sipeventtype = EN_DIALOUT_REL; 
      break;
    }
    case EN_CALL_FEATURE_TYPE_EXTERNAL:
    {
      cp.isPstnNumber = true;
      break;
    }
    case EN_CALL_FEATURE_TYPE_SWB:
    {
      cp.isSwitchBoardCall = true;
      callObj->sipeventtype = EN_SWB_EVENT_REL;
      break;
    }
    case EN_CALL_FEATURE_TYPE_VMS:
    {
      break;
    }
    case EN_CALL_FEATURE_TYPE_CONF:
    {
      cp.isconferenceCall = true;
      callObj->sipeventtype = EN_CONF_EVENT_REL; 
      break; 
    }
    default:
    {
      return false;
    }
  }
  return true;
}

bool xGateBusinessHandler::handleTimeoutEvent(const Value &val, xGateCallObj *callObj)
{
  BusinessTimerType timerType = EN_BUSINESS_TIMER_MAX; 
  if(val.HasMember("timer_type")) {
    timerType = val["timer_type"].GetInt();
  } else {
    XGLOG_ERROR( "handleTimeoutEvent failed. timer_type is not specified for uid: %s !", \
        callObj->uid.c_str());
    return false;
  }

  switch(timerType) {
    case EN_BUSINESS_TIMER_APP_NOTIFY:
      {
        break;
      }
    case EN_BUSINESS_TIMER_CALL_HANDLE:
      {
        break;
      }
    case EN_BUSINESS_TIMER_DIAL_OUT:
      {
        break;
      }
    case EN_BUSINESS_TIMER_SWB_MAIN_IVR:
      {
        break;
      }
    case EN_BUSINESS_TIMER_CONF_DTMF_DIGIT:
      {
        break;
      }
    default:
      {
        XGLOG_ERROR( "handleTimeoutEvent failed. Unknown/Unspported timer_type specified for uid: %s !", \
            callObj->uid.c_str());
        return false;
      }
  }
  return true;
}
