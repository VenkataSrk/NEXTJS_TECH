//local includes
#include "log.h"
#include "klog.h"

//self includes
#include "xGateInternalCallFeature.h"

#define THISMODULE "InternCall"

xGateInternalCallFeature::xGateInternalCallFeature() :
  xGateCallFeature(EN_CALL_FEATURE_TYPE_INTERNAL), m_numberType(EN_INTERNAL_UNKNOWN_NUMBER)
{
}

xGateInternalCallFeature::~xGateInternalCallFeature()
{
}

bool xGateInternalCallFeature::init_feature_info(const Value &val, LegInfo &cliInfo, LegInfo &ddiInfo)
{
  int len = ddiInfo.m_number.length();
  if(len <= 5) {
    m_numberType = EN_INTERNAL_EXTENSION_NUMBER; 
  } else if(len > 5 || len < 15) {
    m_numberType = EN_INTERNAL_DIRECT_NUMBER; 
  } else {
    CLog::Error(THISMODULE, "init_feature_info for internal call failed. unsupported ddi: %s received !", \
        ddiInfo.m_number.c_str());
    return false;
  }

  return true;
}
