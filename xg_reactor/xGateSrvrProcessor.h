#ifndef _XGATE_SERVER_PROCESSOR_H
#define _XGATE_SERVER_PROCESSOR_H

//local includes
#include "xGateProcessor.h"
#include "xGateMgMsg.h"

class xGateSrvrProcessor :  public xGateProcessor
{
  public:
    xGateSrvrProcessor(const char*);
    virtual ~xGateSrvrProcessor(void);

    //virtual xGateRetVal handle_msg(ACE_Message_Block*);
    virtual xGateRetVal process_msg(xGateBaseMsg*);
    virtual xGateRetVal dispatch_msg(xGateBaseMsg*);
    xGateRetVal decode_media_msg(xGateMgMsg *pMgMsg);
    xGateRetVal create_doc_object(Document &doc, const char *jsonData, Value& dataMemObj);
    xGateRetVal handle_media_resp(Value& dataMemObj, MgMediaDetail& serverMediaDetail);
    xGateRetVal handle_gateway_reg(Value& dataMemObj, MgRegistrationDetail& gatewayRegDetail);
    xGateRetVal handle_sdpinfo_resp(const Value& sdpObj, MgMediaDetail& serverMediaDetail);
    xGateRetVal post_media_detail(xGateMgMsg *pMgMsg, MgMediaDetail& mediaDetail); 
};

#endif
