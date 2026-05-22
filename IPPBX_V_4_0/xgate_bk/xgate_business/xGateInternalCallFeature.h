#ifndef _XGATE_INTERNAL_CALL_FEATURE_H
#define _XGATE_INTERNAL_CALL_FEATURE_H

//local includes
#include "xGateCallFeature.h"

typedef enum {
  EN_INTERNAL_UNKNOWN_NUMBER=0,
  EN_INTERNAL_DIRECT_NUMBER=1,
  EN_INTERNAL_EXTENSION_NUMBER=2,
}InternalNumberType;

class xGateInternalCallFeature : public xGateCallFeature
{
  public:
    xGateInternalCallFeature();
    virtual ~xGateInternalCallFeature();

    bool init_feature_info(const Value &val, LegInfo &cliInfo, LegInfo &ddiInfo);

    //member variables
    InternalNumberType m_numberType;
};

#endif
