#include "log.h"
#include "klog.h"
#include "xGateVmsCallFeature.h"

#define THISMODULE "VmsCall"

xGateVmsCallFeature::xGateVmsCallFeature() :
  xGateCallFeature(EN_CALL_FEATURE_TYPE_VMS), m_vmsType(EN_VMS_TYPE_UNKNOWN) 
{
}

xGateVmsCallFeature::~xGateVmsCallFeature()
{
}

bool xGateVmsCallFeature::init_feature_info(const Value &val, LegInfo &cliInfo, LegInfo &ddiInfo)
{
  //1. load vms deposit service only if vms enabled for dailed number and call failure bcoz of (408/480/486)
  if(val.HasMember("is_voicemail_did")) {
    if(val["is_voicemail_did"].GetInt()) {
      m_vmsType = EN_VMS_TYPE_DEPOSITE;
    }
  }

  //2. load vms retrieval service only if dailed/ddi number is voice mail number
  if(val.HasMember("is_voice_mail")) {
    if(val["is_voice_mail"].GetInt()) {
      m_vmsType = EN_VMS_TYPE_RETRIEVAL;
    }
  }
  return true;
}
