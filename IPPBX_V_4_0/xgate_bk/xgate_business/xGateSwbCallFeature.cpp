//local includes
#include "xGateSwbCallFeature.h"

xGateSwbCallFeature::xGateSwbCallFeature() :
  xGateCallFeature(EN_CALL_FEATURE_TYPE_SWB)
{
}

xGateSwbCallFeature::~xGateSwbCallFeature()
{
}

bool xGateSwbCallFeature::init_feature_info(const Value &val, LegInfo &cliInfo, LegInfo &ddiInfo)
{
  return true;
}
