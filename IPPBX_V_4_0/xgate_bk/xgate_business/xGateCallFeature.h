#ifndef _XGATE_CALL_FEATURE_H
#define _XGATE_CALL_FEATURE_H

#include "xGateBusinessUtil.h"

class xGateCallFeature 
{
  public:
    xGateCallFeature(CallFeatureType callFeatureType);
    virtual ~xGateCallFeature();
  
    //member functions
    virtual bool init_info(const Value &val, LegInfo &cliInfo, LegInfo &ddiInfo);
    virtual bool init_feature_info(const Value &val, LegInfo &cliInfo, LegInfo &ddiInfo) = 0;

    //member vaiables
    CallFeatureType m_callFeatureType;
};

#endif
