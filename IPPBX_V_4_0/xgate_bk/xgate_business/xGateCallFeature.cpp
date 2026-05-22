//system includes
#include <string>
#include <iostream>
#include <sstream>

//local includes
#include "xGateCallFeature.h"

using namespace std;

xGateCallFeature::xGateCallFeature(CallFeatureType callFeatureType) : 
  m_callFeatureType(callFeatureType)
{
}

xGateCallFeature::~xGateCallFeature()
{
}

//this function fill the generic leg info's from prequery result
bool xGateCallFeature::init_info(const Value &val, LegInfo &cliInfo, LegInfo &ddiInfo)
{

  //fill cli info's
  if(val.HasMember("leg_a_deskphone_status")) {
    cliInfo.m_userStatus = val["leg_a_deskphone_status"].GetInt(); 
  }

  if(cliInfo.m_numberType != EN_LEG_NUMBER_TYPE_EXTERNAL && !cliInfo.m_domainName.empty()) {
    if(val.HasMember("domain_id")) {
      cliInfo.m_domainId = val["domain_id"].GetInt(); 
    }
  }

  if(val.HasMember("ondemand_rec_start_ivr")) {
    cliInfo.m_ivrFile.m_recordStart = val["ondemand_rec_start_ivr"].GetString(); 
  }

  if(val.HasMember("ondemand_rec_stop_ivr")) {
    cliInfo.m_ivrFile.m_recordStop = val["ondemand_rec_stop_ivr"].GetString(); 
  }

  if(val.HasMember("is_voicemail_cli")) {
    cliInfo.m_isVoiceMailEnabled = val["is_voicemail_cli"].GetString(); 
  }

  //fill ddi info's
  if(val.HasMember("leg_b_deskphone_status")) {
    ddiInfo.m_userStatus = val["leg_b_deskphone_status"].GetInt(); 
  }

  if(ddiInfo.m_numberType != EN_LEG_NUMBER_TYPE_EXTERNAL && !ddiInfo.m_domainName.empty()) {
    if(val.HasMember("domain_id")) {
      ddiInfo.m_domainId = val["domain_id"].GetInt(); 
    }
  }

  if(val.HasMember("user_greeting_file")) {
    ddiInfo.m_ivrFile.m_userGreeting = val["user_greeting_file"].GetString(); 
    if(ddiInfo.m_domainId > 0 && !ddiInfo.m_ivrFile.m_userGreeting.empty()) { 
      std::ostringstream oss;
      oss << "/pbxshare/" << ddiInfo.m_domainId << "/UG/" << ddiInfo.m_ivrFile.m_userGreeting;
      ddiInfo.m_ivrFile.m_userGreeting = oss.str();
    }
  }

  if(val.HasMember("connecting_ivr")) {
    ddiInfo.m_ivrFile.m_connectingMsg = val["connecting_ivr"].GetString(); 
    if(!ddiInfo.m_ivrFile.m_connectingMsg.empty()) { 
      std::ostringstream oss;
      oss << "/pbxshare/" << ddiInfo.m_domainId << "/UG/" << ddiInfo.m_ivrFile.m_connectingMsg;
      ddiInfo.m_ivrFile.m_connectingMsg = oss.str();
    }
  }

  if(val.HasMember("ondemand_rec_start_ivr")) {
    ddiInfo.m_ivrFile.m_recordStart = val["ondemand_rec_start_ivr"].GetString(); 
    if(!ddiInfo.m_ivrFile.m_recordStart.empty()) { 
      std::ostringstream oss;
      oss << "/pbxshare/" << ddiInfo.m_domainId << "/UG/" << ddiInfo.m_ivrFile.m_recordStart;
      ddiInfo.m_ivrFile.m_recordStart = oss.str();
    }
  }

  if(val.HasMember("ondemand_rec_stop_ivr")) {
    ddiInfo.m_ivrFile.m_recordStop = val["ondemand_rec_stop_ivr"].GetString(); 
    if(!ddiInfo.m_ivrFile.m_recordStop.empty()) { 
      std::ostringstream oss;
      oss << "/pbxshare/" << ddiInfo.m_domainId << "/UG/" << ddiInfo.m_ivrFile.m_recordStop;
      ddiInfo.m_ivrFile.m_recordStop = oss.str();
    }
  }

  if(val.HasMember("is_voicemail_did")) {
    ddiInfo.m_isVoiceMailEnabled = val["is_voicemail_did"].GetString(); 
  }

  return init_feature_info(val, cliInfo, ddiInfo);
}
