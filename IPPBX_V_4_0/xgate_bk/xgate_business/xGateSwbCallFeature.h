#ifndef _XGATE_SWB_CALL_FEATURE_H
#define _XGATE_SWB_CALL_FEATURE_H

//local includes
#include "xGateCallFeature.h"

class xGateSwbCallFeature : public xGateCallFeature 
{
  public:
    xGateSwbCallFeature();
    virtual ~xGateSwbCallFeature();

    bool init_feature_info(const Value &val, LegInfo &cliInfo, LegInfo &ddiInfo);
};

#endif
