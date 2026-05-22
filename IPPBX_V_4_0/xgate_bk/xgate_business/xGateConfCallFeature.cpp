//local includes
#include "xGateConfCallFeature.h"

xGateConfCallFeature::xGateConfCallFeature() :
  xGateCallFeature(EN_CALL_FEATURE_TYPE_CONF)
{
}

xGateConfCallFeature::~xGateConfCallFeature()
{
}

bool xGateConfCallFeature::init_feature_info(const Value &val, LegInfo &cliInfo, LegInfo &ddiInfo)
{
  return true;
}
