#ifndef _XGATE_EXTERNAL_CALL_FEATURE_H
#define _XGATE_EXTERNAL_CALL_FEATURE_H

//local includes
#include "xGateCallFeature.h"

class xGateExternalCallFeature : public xGateCallFeature
{
  public:
    xGateExternalCallFeature();
    virtual ~xGateExternalCallFeature();

    bool init_feature_info(const Value &val, LegInfo &cliInfo, LegInfo &ddiInfo);
};

#endif
