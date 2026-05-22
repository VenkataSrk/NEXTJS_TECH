#ifndef _XGATE_MBC_CONTROLLER_PROCESSOR_H
#define _XGATE_MBC_CONTROLLER_PROCESSOR_H

//local includes
#include "xGateProcessor.h"
#include "xGateUtil.h"
#include "xGateMBCServiceMsg.h"
#include "xGateMBCService.h"
#include "MediaServiceDefine.h"
class MBCControllerProcessor :  public xGateProcessor
{
public:
    MBCControllerProcessor(const char*);
    virtual ~MBCControllerProcessor(void);

    virtual xGateRetVal process_msg(xGateBaseMsg*);
    xGateRetVal decode_media_msg(xGateMBCServiceMsg *ptmediaBCastMsg);
private:
    xGateRetVal handle_media_req(Value& dataMemObj, MgMediaDetail& clientMediaDetail);
    xGateRetVal handle_sdpinfo_process(const Value& sdpObj, MgMediaDetail& serverMediaDetail);
    xGateRetVal handle_sdpinfo_audio(const Value& sdpObj, MgMediaDetail& clientMediaDetail);
    xGateRetVal handle_sdpinfo_video(const Value& sdpObj, MgMediaDetail& clientMediaDetail);
    xGateRetVal create_doc_object(Document &doc, const char *data, Value& dataMemObj);
    xGateRetVal send_media_msg_to_sfu(xGateMBCServiceMsg *ptmediaBCastMsg,MgMediaDetail& clientMediaDetail,xGateMgMsgType mgMsgType);
    xGateRetVal post_media_detail(xGateMBCServiceMsg *ptmediaBCastMsg,xGateMgMsgType mgMsgType,MgMediaDetail& clientMediaDetail);
};

#endif

