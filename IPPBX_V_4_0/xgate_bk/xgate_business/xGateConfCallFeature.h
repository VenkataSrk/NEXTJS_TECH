#ifndef _XGATE_CONF_CALL_FEATURE_H
#define _XGATE_CONF_CALL_FEATURE_H

//local includes
#include "xGateCallFeature.h"

class xGateConfCallFeature : public xGateCallFeature
{
  public:
    xGateConfCallFeature();
    virtual ~xGateConfCallFeature();

    bool init_feature_info(const Value &val, LegInfo &cliInfo, LegInfo &ddiInfo);
};

#endif
