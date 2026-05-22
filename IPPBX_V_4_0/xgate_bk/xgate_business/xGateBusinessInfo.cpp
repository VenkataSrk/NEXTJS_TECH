//system includes
#include <sstream>

//local includes
#include "log.h"
#include "klog.h"

//self includes
#include "xGateBusinessInfo.h"
#include "xGateBusinessUtil.h"

#define THISMODULE "BusInfo"

xGateBusinessInfo::xGateBusinessInfo(string &uid, string &cli, string &ddi, string &domainName) :
  m_uid(uid), m_cli(cli), m_ddi(ddi), m_domainName(domainName), m_domainId(0),
  m_callFeatureType(EN_CALL_FEATURE_TYPE_INTERNAL), m_currentSigEvent(EN_BUSINESS_EVENT_UNKNOWN),
  m_isForkingEnabled(0)
{
  CLog::Warning(THISMODULE, "xGateBusinessManager::xGateBusinessManager");
  //set important cli info details
  m_cliInfo.m_number = cli;
  m_cliInfo.m_domainName = domainName;
  m_cliInfo.findNumberType();

  //set important ddi info details
  m_ddiInfo.m_number = ddi;
  m_ddiInfo.m_domainName = domainName;
  m_ddiInfo.findNumberType();

  for(unsigned int i=0; i<EN_BUSINESS_TIMER_MAX; i++) {
    m_timerList[i]=NULL;
  }
}

xGateBusinessInfo::~xGateBusinessInfo()
{
  CLog::Warning(THISMODULE, "xGateBusinessManager::~xGateBusinessManager");
}

bool xGateBusinessInfo::init()
{
  //step1: send combined/pre query request to DBManager
  m_currentSigEvent = EN_BUSINESS_EVENT_INIT; 
  if(!sendToDb(EN_DB_EVENT_PREQUERY)) {
    CLog::Error(THISMODULE, "BusinessInfo init() failed. EN_DB_EVENT_PREQUERY sendToDb for uid:%s failed!", \
        m_uid.c_str());
    return false;
  }
  return true; 
}

bool xGateBusinessInfo::get_aor(const Value &val)
{
  m_currentSigEvent = EN_BUSINESS_EVENT_FORKING; 
  string ddi(""), domainName("");
  Value::ConstMemberIterator itr = val.MemberBegin();
  Value::ConstMemberIterator itrEnd = val.MemberEnd();

  for (itr; itr != itrEnd; ++itr) {
    if(!strcmp(itr->name.GetString(), "ddi")) {
      ddi = itr->value.GetString();
    } else if(!strcmp(itr->name.GetString(), "domain_name")) {
      domainName = itr->value.GetString();
    } 
  }
  
  if(ddi.empty() || domainName.empty()) {
    CLog::Error(THISMODULE, "get_aor failed. either ddi/domainName value is empty !");
    return false;
  }

  CLog::Detail(THISMODULE, "get_aor for ddi:%d, domain_name:%s", ddi.c_str(), domainName.c_str());

  if(!sendToDb(EN_DB_EVENT_GET_AOR)) {
    CLog::Error(THISMODULE, "BusinessInfo get_aor() failed. EN_DB_EVENT_GET_AOR sendToDb for uid:%s failed!", \
        m_uid.c_str());
    return false;
  }

  return true; 
}

bool xGateBusinessInfo::sendToDb(int event)
{
  std::ostringstream oss;
  switch(event) {
    case EN_DB_EVENT_PREQUERY:
      {
        oss << jbegin << "cli" << jcolon << m_cli << jcomma \
          << "ddi" << jcolon << m_ddi << jcomma \
          << "domain_name" << jcolon << m_domainName << jend;
        break;
      } 
    case EN_DB_EVENT_GET_AOR:
      {
        oss << jbegin << "ddi" << jcolon << m_ddiInfo.m_number << jcomma \
          << "domain_name" << jcolon  << m_domainName << jend;
        break;
      }
    case EN_DB_EVENT_UNKNOWN:
    default:
      {
        return false;
      }
  }

  xGateBusinessData data(m_uid, event, oss.str(), SEND_TO_DB_PROCESSOR);
  return xGateBusinessUtil::sendBusinessData(data);
}

bool xGateBusinessInfo::use_prequery_result(const Value &val)
{

  if(val.HasMember("domain_id")) {
    m_domainId = val["domain_id"].GetInt();
  } else {
    CLog::Error(THISMODULE, "use_prequery_result failed. 'domain_id' is missing in json message !");
    return false;
  }

  if(val.HasMember("domain_name")) {
    m_domainName = val["domain_name"].GetString();
  } else {
    CLog::Error(THISMODULE, "process_msg failed. 'domain_name' is missing in json message !");
    return false;
  }

  //find and load call feature
  if(!load_call_feature(val)) {
    CLog::Error(THISMODULE, "process_msg failed due to load_call_feature failure !");
    return false;
  }

  return true;
}

bool xGateBusinessInfo::load_call_feature(const Value &val)
{
  //reset call feature list
  for(unsigned int i=0; i<EN_CALL_FEATURE_TYPE_MAX; i++) {
    m_callFeatureList[i]=NULL;
  }

  if(load_internal_call_feature(val)) {
    m_callFeatureType = EN_CALL_FEATURE_TYPE_INTERNAL; 
    CLog::Detail(THISMODULE, "load_call_feature loaded internal-call feature module successfully"); 
  }

  if(load_vms_call_feature(val)) {
    m_callFeatureType = EN_CALL_FEATURE_TYPE_VMS; 
    CLog::Detail(THISMODULE, "load_call_feature loaded voice-mail-call feature module successfully"); 
  }

  if(load_swb_call_feature(val)) {
    m_callFeatureType = EN_CALL_FEATURE_TYPE_SWB; 
    CLog::Detail(THISMODULE, "load_call_feature loaded swb-call feature module successfully"); 
  }

  if(load_conf_call_feature(val)) {
    m_callFeatureType = EN_CALL_FEATURE_TYPE_CONF; 
    CLog::Detail(THISMODULE, "load_call_feature loaded conf-call feature module successfully"); 
  }

  if(load_external_call_feature(val)) {
    m_callFeatureType = EN_CALL_FEATURE_TYPE_EXTERNAL; 
    CLog::Detail(THISMODULE, "load_call_feature loaded external-call feature module successfully"); 
  }

  return true;
}

bool xGateBusinessInfo::load_internal_call_feature(const Value &val)
{
  if(m_ddiInfo.m_number.length() >= 10) { //finding direct number code logics - start
    string directDomainName("");    //DomainName related to extension number that mapped with direct numaber
    unsigned int directDomainId(0); //DomainId related to extension number
    string directExtnNum(""); //extension number mapped with this direct number
    string directExtnPwd("");  //passowrd for mapped extension

    if(val.HasMember("externalnb_domain_name")) {
      directDomainName = val["externalnb_domain_name"].GetString(); 
    }
    if(val.HasMember("externalnb_domain_id")) {
      directDomainId = val["externalnb_domain_id"].GetInt(); 
    }
    if(val.HasMember("externalnb_extn")) {
      directExtnNum = val["externalnb_extn"].GetString(); 
    }
    if(val.HasMember("externalnb_extn_pwd")) {
      directExtnPwd = val["externalnb_extn_pwd"].GetString(); 
    }

    if(!directDomainName.empty() && !directExtnNum.empty() && \
        !directExtnPwd.empty() && directDomainId > 0) {
      xGateCallFeature *callFeature = CallFeatureFactory::load(EN_CALL_FEATURE_TYPE_INTERNAL);
      if(callFeature->init_info(val, m_cliInfo, m_ddiInfo)) {
        m_callFeatureList[EN_CALL_FEATURE_TYPE_INTERNAL] = callFeature;      
        CLog::Detail(THISMODULE, "load_internal_call_feature success for uid:%s !", m_uid.c_str());
        return true;
      }
    }

    CLog::Detail(THISMODULE, "load_internal_call_feature found as the ddi number: %s \
        is not a direct call number", m_ddiInfo.m_number.c_str());
  } //finding direct number code logics - end
  
  if(m_ddiInfo.m_number.length() <= 5 && \ 
      m_ddiInfo.m_numberType == EN_LEG_NUMBER_TYPE_INTERNAL) { //finding extension number logics - start
    if(val.HasMember("voice_mail")) {
      if(val["voice_mail"].GetInt()) {
        CLog::Detail(THISMODULE, "load_internal_call_feature found as the ddi number: %s \
            might be a voice mail service number", m_ddiInfo.m_number.c_str());
        return false;
      }
    }

    //load internal call feature module for this direct call number
    xGateCallFeature *callFeature = CallFeatureFactory::load(EN_CALL_FEATURE_TYPE_INTERNAL);
    if(callFeature->init_info(val, m_cliInfo, m_ddiInfo)) {
      m_callFeatureList[EN_CALL_FEATURE_TYPE_INTERNAL] = callFeature;      
      CLog::Detail(THISMODULE, "load_internal_call_feature success for uid:%s !", m_uid.c_str());
      //once internal-call feature initialized, send AOR request to dblayer
      if(!sendToDb(EN_DB_EVENT_GET_AOR)) {
        CLog::Error(THISMODULE, "load_internal_call failed. EN_DB_EVENT_AOR sendToDb for uid:%s failed!", \
            m_uid.c_str());
        return false;
      }
      return true;
    }

    return true;
  } //finding extension number logics - end

  return false;
}

bool xGateBusinessInfo::load_vms_call_feature(const Value &val)
{
  VoiceMailServiceType vmsType = EN_VMS_TYPE_UNKNOWN; 
  //we may need to load vms module only in two scenario's
  //1. load vms deposit service only if vms enabled for dailed number and call failure bcoz of (408/480/486)
  if(val.HasMember("is_voicemail_did")) {
    if(val["is_voicemail_did"].GetInt()) {
      vmsType = EN_VMS_TYPE_DEPOSITE;
    }
  }

  //2. load vms retrieval service only if dailed/ddi number is voice mail number
  if(val.HasMember("is_voice_mail")) {
    if(val["is_voice_mail"].GetInt()) {
      vmsType = EN_VMS_TYPE_RETRIEVAL;
    }
  }

  if(vmsType == EN_VMS_TYPE_UNKNOWN) {
    CLog::Detail(THISMODULE, "voice mail service not required for uid:%s !", m_uid.c_str());
    return false; 
  }

  xGateCallFeature *callFeature = CallFeatureFactory::load(EN_CALL_FEATURE_TYPE_VMS);
  if(callFeature->init_info(val, m_cliInfo, m_ddiInfo)) {
    m_callFeatureList[EN_CALL_FEATURE_TYPE_VMS] = callFeature;      
    CLog::Detail(THISMODULE, "load_vms_call_feature success for uid:%s !", m_uid.c_str());
    return true;
  }

  CLog::Error(THISMODULE, "load_vms_call_feature failed for uid: %s!", m_uid.c_str());
  return false;
}

bool xGateBusinessInfo::load_swb_call_feature(const Value &val)
{
  if(val.HasMember("is_swb")) {
    if(val["is_swb"].GetInt()) {
      xGateCallFeature *callFeature = CallFeatureFactory::load(EN_CALL_FEATURE_TYPE_SWB);
      if(callFeature->init_info(val, m_cliInfo, m_ddiInfo)) {
        m_callFeatureList[EN_CALL_FEATURE_TYPE_SWB] = callFeature;      
        CLog::Detail(THISMODULE, "load_swb_call_feature success for uid:%s !", m_uid.c_str());
        return true;
      }
    } else {
      CLog::Detail(THISMODULE, "switch board call service not required for uid:%s !", m_uid.c_str());
    }
  }
  return false;
}

bool xGateBusinessInfo::load_conf_call_feature(const Value &val)
{
  if(val.HasMember("is_conference")) {
    if(val["is_conference"].GetInt()) {
      xGateCallFeature *callFeature = CallFeatureFactory::load(EN_CALL_FEATURE_TYPE_CONF);
      if(callFeature->init_info(val, m_cliInfo, m_ddiInfo)) {
        m_callFeatureList[EN_CALL_FEATURE_TYPE_CONF] = callFeature;      
        CLog::Detail(THISMODULE, "load_conf_call_feature success for uid:%s !", m_uid.c_str());
        return true;
      }
    } else {
      CLog::Detail(THISMODULE, "conference call service not required for uid:%s !", m_uid.c_str());
    }
  }
  return false;
}

//TODO: Yoga, need to build logic to identify on which basis we can find external number
bool xGateBusinessInfo::load_external_call_feature(const Value &val)
{
  //TODO: Yoga, pls check also domain_id is null. and then try to load this call feature.
  if(val.HasMember("is_external")) {
    if(val["is_external"].GetInt()) {
      xGateCallFeature *callFeature = CallFeatureFactory::load(EN_CALL_FEATURE_TYPE_EXTERNAL);
      if(callFeature->init_info(val, m_cliInfo, m_ddiInfo)) {
        m_callFeatureList[EN_CALL_FEATURE_TYPE_EXTERNAL] = callFeature;      
        CLog::Detail(THISMODULE, "load_external_call_feature success for uid:%s !", m_uid.c_str());
        return true;
      }
    } else {
      CLog::Detail(THISMODULE, "external call service not required for uid:%s !", m_uid.c_str());
    }
  }
  return false;
}

bool xGateBusinessInfo::use_aor_result(const Value &val)
{
  if(val.IsArray()) {
    Value::ConstValueIterator itr = val.Begin();
    for(itr; itr != val.End(); itr++) {
      const Value &aor = *itr;
      if(!load_aor(aor)) {
        CLog::Detail(THISMODULE, "use_aor_result failed due to loading aor failure for uid:%s !", m_uid.c_str());
        return false;
      }
    }
    m_isForkingEnabled = 1; //if aor is more than one, then enable the forking
  } else if(val.IsObject()) {
    if(!load_aor(val)) {
      CLog::Detail(THISMODULE, "use_aor_result failed due to loading aor failure for uid:%s !", m_uid.c_str());
      return false;
    }
  }

  //on sucessful load_aor, send complete result to sig layer
  m_callFeatureType = EN_CALL_FEATURE_TYPE_INTERNAL; //TODO: Yoga, added for 1-1 call integration testing 
  if(!send_business_init_response(val)) {
    CLog::Error(THISMODULE, "use_aor_result failed to send_business_init_response for uid:%s !", \
        m_uid.c_str());
    return false;
  }
  return true;
}

bool xGateBusinessInfo::load_aor(const Value &val)
{
  if(!val.IsObject()) {
    return false;
  }
  
  AddressOfRecord aor;
  if(val.HasMember("Contact_address")) {
    aor.m_contactAddr = val["Contact_address"].GetString();
  } else {
    CLog::Error(THISMODULE, "load_aor failed. Contact_address is empty for uid: %s", m_uid.c_str());
    return false;
  }

  if(val.HasMember("Ipaddress")) {
    aor.m_ipAddr = val["Ipaddress"].GetString();
  } else {
    CLog::Error(THISMODULE, "load_aor failed. Ipaddress is empty for uid: %s", m_uid.c_str());
    return false;
  }

  if(val.HasMember("proxy_username")) {
    aor.m_proxyUserName = val["proxy_username"].GetString();
  } else {
    CLog::Error(THISMODULE, "load_aor failed. proxy_username is empty for uid: %s", m_uid.c_str());
    return false;
  }

  if(val.HasMember("device_type")) {
    aor.m_sDeviceType = val["device_type"].GetString();
  } else {
    CLog::Error(THISMODULE, "load_aor failed. device_type is empty for uid: %s", m_uid.c_str());
    return false;
  }
  
  unsigned int deviceType = aor.getDeviceType();
  m_MappedUserList[deviceType] = aor;

  //TODO: Yoga, send push notification only if currentSigEvent is EN_BUSINESS_EVENT_INIT 
  if(m_currentSigEvent == EN_BUSINESS_EVENT_INIT) {
    check_send_app_notification(aor);
  }
  return true;
}

bool xGateBusinessInfo::check_send_app_notification(AddressOfRecord &aor)
{
  char pushFrom[20], pushTo[20] = {0}, uri[600]={0};
  sprintf(pushFrom, "%d_%s", m_ddiInfo.m_domainId, m_cli.c_str());
  sprintf(pushTo, "%d_%s", m_ddiInfo.m_domainId, m_ddi.c_str());

  switch(aor.m_iDeviceType) {
    case EN_DEVICE_TYPE_IOS:
      {
        sprintf(uri, "http://urmyaccount.mundio.com/v1/user/urpushnotification?origin=%s&target=%s&isvoip=true&media=audio&iscancel=0&isLive=true&iuid=%s_%s", pushFrom, pushTo, m_uid.c_str(), xGateUtil::getLocalIpAddr());
        CLog::Detail(THISMODULE, "uri: '%s' send push notification to the IOS device", uri);
        break;
      }
    case EN_DEVICE_TYPE_ANDROID:
      {
        sprintf(uri, "http://urmyaccount.mundio.com/v1/user/urpushnotificationFCM?origin=%s&target=%s&isvoip=false&iscancel=0&iuid=%s_%s", pushFrom, pushTo, m_uid.c_str(), xGateUtil::getLocalIpAddr());
        CLog::Detail(THISMODULE, "uri: '%s' send push notification to the ANDROID device", uri);
        break;
      }
    case EN_DEVICE_TYPE_DESKPHONE:
    case EN_DEVICE_TYPE_WEB:
    case EN_DEVICE_TYPE_PSTN:
      {
        if(m_timerList[EN_BUSINESS_TIMER_CALL_HANDLE] == 0) {
          m_timerList[EN_BUSINESS_TIMER_CALL_HANDLE] \
            = xGateBusinessUtil::setTimer(m_uid, EN_BUSINESS_TIMER_CALL_HANDLE, 30);
        } else {
          CLog::Error(THISMODULE, "call handle timer: %ld already been scheduled for uid: %s !", \
              m_timerList[EN_BUSINESS_TIMER_CALL_HANDLE], m_uid.c_str());
        }
        if(m_timerList[EN_BUSINESS_TIMER_DIAL_OUT] == 0) {
          m_timerList[EN_BUSINESS_TIMER_DIAL_OUT] \
            = xGateBusinessUtil::setTimer(m_uid, EN_BUSINESS_TIMER_DIAL_OUT, 60);
        } else {
          CLog::Error(THISMODULE, "dial out timer: %ld already been scheduled for uid: %s !", \
              m_timerList[EN_BUSINESS_TIMER_DIAL_OUT], m_uid.c_str());
        }
        CLog::Detail(THISMODULE, "app notification not required for DESKPHONE/WEB so ignored"); 
        return true;
      }
    default:
      {
        CLog::Error(THISMODULE, "check_send_app_notification failed. Unsupported device type, \
            not able to create uri string for uid: %s", m_uid.c_str());
        return false;
      }
  }
  if(xGateBusinessUtil::sendAppNotification(uri, m_uid.c_str())) {
    //schedule the push notification timer only once.
    if(m_timerList[EN_BUSINESS_TIMER_APP_NOTIFY] == 0) {
      m_timerList[EN_BUSINESS_TIMER_APP_NOTIFY] \
        = xGateBusinessUtil::setTimer(m_uid, EN_BUSINESS_TIMER_APP_NOTIFY, 10);
    } else {
      CLog::Error(THISMODULE, "App notification timer: %ld already been scheduled for uid: %s !", \
          m_timerList[EN_BUSINESS_TIMER_APP_NOTIFY], m_uid.c_str());
    }
  }
  return true;
}

bool xGateBusinessInfo::send_business_init_response(const Value &val)
{
  StringBuffer sb;
  Writer<StringBuffer> writer(sb);
  val.Accept(writer);
  string mappedUsers = sb.GetString();
  
  std::ostringstream oss;
  oss << jbegin \ 
    << "cli" << jcolon << m_cli << jcomma \
    << "ddi" << jcolon << m_ddi << jcomma \
    << "domain_name" << jcolon << m_domainName << jcomma \
    << "domain_id" << jcolon << m_domainId << jcomma \
    << "ivr_rbt" << jcolon << m_ddiInfo.getIvrFile(EN_IVR_RBT) << jcomma \
    << "ivr_user_greeting" << jcolon << m_ddiInfo.getIvrFile(EN_IVR_USER_GREETING) << jcomma \
    << "ivr_connecting_msg" << jcolon << m_ddiInfo.getIvrFile(EN_IVR_CONNECT_MSG) << jcomma \
    << "ivr_silence" << jcolon << m_ddiInfo.getIvrFile(EN_IVR_SILENCE) << jcomma \
    << "call_feature_type" << "\":" << m_callFeatureType << ",\"" \
    << "is_forking" << "\":" << m_isForkingEnabled << ",\"" \
    << "mapped_user" << "\":" << mappedUsers << "}";
  xGateBusinessData data(m_uid, m_currentSigEvent, oss.str(), SEND_TO_SIGNAL_PROCESSOR);
  return xGateBusinessUtil::sendBusinessData(data);
}

bool xGateBusinessInfo::handle_timeout_event(const Value &val)
{
  BusinessTimerType timerType = EN_BUSINESS_TIMER_MAX; 
  if(val.HasMember("timer_type")) {
    timerType = val["timer_type"].GetInt();
  } else {
    CLog::Error(THISMODULE, "handle_timeout_event failed. timer_type is not specified for uid: %s !", m_uid.c_str());
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
        CLog::Error(THISMODULE, "handle_timeout_event failed. Unknown/Unspported timer_type specified for uid: %s !", m_uid.c_str());
        return false;
      }
  }

  //reset timer_id value to zero
  m_timerList[timerType] = 0;
  char output[100] ={0};
  sprintf(output, "%stimer_type\":%d}", jbegin, timerType);

  xGateBusinessData data(m_uid, EN_BUSINESS_EVENT_TIMER, output, SEND_TO_SIGNAL_PROCESSOR);
  return xGateBusinessUtil::sendBusinessData(data);
}

bool xGateBusinessInfo::handle_cancel_event(const Value &val)
{
  BusinessTimerType timerType = EN_BUSINESS_TIMER_MAX; 
  if(val.HasMember("timer_type")) {
    timerType = val["timer_type"].GetInt();
  } else {
    CLog::Error(THISMODULE, "handle_cancel_event failed. timer_type is not specified for uid: %s !", m_uid.c_str());
    return false;
  }

  unsigned long timerId = m_timerList[timerType];
  if(timerId > 0) {
    xGateBusinessUtil::cancelTimer(m_uid, timerId);
    CLog::Detail(THISMODULE, "handle_cancel_event successfully canceled timer:%d for uid: %s !", timerType, m_uid.c_str());
  } else {
    CLog::Error(THISMODULE, "handle_cancel_event failed. timer not yet been scheduled for timer:%d for uid: %s !", \
        timerType, m_uid.c_str());
    return false;
  }
  return true;
}
