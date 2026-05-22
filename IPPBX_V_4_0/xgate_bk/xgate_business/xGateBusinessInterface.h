#ifndef _XGATE_BUSINESS_INTERFACE_H
#define _XGATE_BUSINESS_INTERFACE_H

//local includes
#include "xGateInternalCallFeature.h"
#include "xGateExternalCallFeature.h"
#include "xGateSwbCallFeature.h"
#include "xGateVmsCallFeature.h"
#include "xGateConfCallFeature.h"

class CallFeatureFactory {
  public:
  CallFeatureFactory();
  ~CallFeatureFactory();
  static xGateCallFeature* load(CallFeatureType callFeatureType);

};

#endif
