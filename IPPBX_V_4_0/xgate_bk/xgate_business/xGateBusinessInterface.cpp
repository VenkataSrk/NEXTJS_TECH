
//local includes
#include "xGateBusinessInterface.h"

xGateCallFeature * CallFeatureFactory::load(CallFeatureType callFeatureType)
{
  xGateCallFeature *callFeature = NULL;
  switch(callFeatureType) {
    case EN_CALL_FEATURE_TYPE_INTERNAL:
    {
      callFeature = new xGateInternalCallFeature(); 
      break;
    }
    case EN_CALL_FEATURE_TYPE_EXTERNAL:
    {
      callFeature = new xGateExternalCallFeature(); 
      break;
    }
    case EN_CALL_FEATURE_TYPE_SWB:
    {
      callFeature = new xGateSwbCallFeature(); 
      break;
    }
    case EN_CALL_FEATURE_TYPE_VMS:
    {
      callFeature = new xGateVmsCallFeature(); 
      break;
    }
    case EN_CALL_FEATURE_TYPE_CONF:
    {
      callFeature = new xGateConfCallFeature(); 
      break;
    }
    default:
    {
      break;
    }
  }
  return callFeature;
}
