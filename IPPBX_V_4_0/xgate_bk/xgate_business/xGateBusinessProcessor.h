#ifndef _XGATE_BUSINESS_PROCESSOR_H
#define _XGATE_BUSINESS_PROCESSOR_H

//ace includes

//local includes
#include "xGateProcessor.h"
#include "xGateBusinessMsg.h"
#include "xGateBusinessManager.h"

class xGateBusinessProcessor :  public xGateProcessor
{
  public:
    xGateBusinessProcessor(const char *serverType);
    virtual ~xGateBusinessProcessor(void);

    virtual xGateRetVal process_msg(xGateBaseMsg*);
    //bool decode_msg(xGateBusinessMsg *pMsg);
    
};

#endif
