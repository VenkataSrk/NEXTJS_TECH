#ifndef _XGATE_MG_PROCESSOR_H
#define _XGATE_MG_PROCESSOR_H

//local includes
#include "xGateProcessor.h"
#include "xGateUtil.h"
#include "xGateMgMsg.h"

class xGateMgProcessor :  public xGateProcessor
{
  public:
    xGateMgProcessor(const char*);
    virtual ~xGateMgProcessor(void);

    virtual xGateRetVal process_msg(xGateBaseMsg*);
    xGateRetVal decode_media_msg(xGateMgMsg *pMgMsg);
  private:
    xGateRetVal create_doc_object(Document &doc, const char *data, Value& dataMemObj);
    xGateRetVal handle_playback_req(Value& dataMemObj, MgPlayBackDetail& playBackDetail);
    //xGateRetVal handle_playback_req( MgPlayBackDetail& playBackDetail);
    xGateRetVal handle_media_req(Value& dataMemObj, MgMediaDetail& clientMediaDetail);
    //xGateRetVal handle_media_req( MgMediaDetail& clientMediaDetail);
    xGateRetVal post_media_detail(xGateMgMsg *pMgMsg);
};

#endif
