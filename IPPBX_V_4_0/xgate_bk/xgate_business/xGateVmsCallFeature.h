#ifndef _XGATE_VMS_CALL_FEATURE_H
#define _XGATE_VMS_CALL_FEATURE_H

#include "xGateCallFeature.h"

typedef enum {
  EN_VMS_TYPE_UNKNOWN=0,
  EN_VMS_TYPE_DEPOSITE=1,
  EN_VMS_TYPE_RETRIEVAL=2,
}VoiceMailServiceType;

class xGateVmsCallFeature : public xGateCallFeature 
{
  public:
    xGateVmsCallFeature();
    virtual ~xGateVmsCallFeature();
    
    //member functions
    bool init_feature_info(const Value &val, LegInfo &cliInfo, LegInfo &ddiInfo);

    //member variables
    m_vmsType;
};

#endif
