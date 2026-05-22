//local includes
#include "xGateExternalCallFeature.h"

xGateExternalCallFeature::xGateExternalCallFeature() :
  xGateCallFeature(EN_CALL_FEATURE_TYPE_EXTERNAL)
{
}

xGateExternalCallFeature::~xGateExternalCallFeature()
{
}

bool xGateExternalCallFeature::init_feature_info(const Value &val, LegInfo &cliInfo, LegInfo &ddiInfo)
{
  return true;
}
