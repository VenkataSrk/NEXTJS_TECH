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
      xGateRetVal handle_media_req(Value& dataMemObj, MgMediaDetail& clientMediaDetail);
      xGateRetVal handle_sdpinfo_process(const Value& sdpObj, MgMediaDetail& serverMediaDetail);
      xGateRetVal handle_sdpinfo_audio(const Value& sdpObj, MgMediaDetail& clientMediaDetail);
      xGateRetVal handle_audiodetail_ivrlist(const char *audioWavList, AudioDetail& audioDetail);
      xGateRetVal Get_Opus_Fmtp(std::string fmtp, AudioDetail& audioDetail);
      xGateRetVal Get_Fmtp_Values(char *fmtp_parameter, AudioDetail& audioDetail);
      xGateRetVal post_media_detail(xGateMgMsg *pMgMsg);
};

#endif
