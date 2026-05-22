#ifndef _XGATE_HMP_CHANNEL_HANDLER_H
#define _XGATE_HMP_CHANNEL_HANDLER_H

//local includes
#include "xGateProcessor.h"
#include "xGateUtil.h"
#include "xGateMgMsg.h"
#include "xGateMRCPMsg.h"
#include "URMRCPModuleBridge.h"
#ifdef XGATE_HAS_SELF_AUDIT
#include "xGateAuditMsg.h"
#include <map>
#endif


class xGateHmpProcessor :  public xGateProcessor
//class xGateHmpProcessor
{
  public:
    xGateHmpProcessor(const char*);
    virtual ~xGateHmpProcessor(void);
    virtual xGateRetVal process_msg(xGateBaseMsg*);
    xGateRetVal (*functionptr)(xGateMgMsg *pMgMsg);
    xGateRetVal initMRCPBridge();
    Value setJsonParam(string strValue,Document &doc);
    Value setJsonParam(unsigned int iValue,Document &doc);
    bool PostMessageToMrcpClient(xGateDbConfigMgrServiceMsg *);
   
    string encode_stt_tts_sdp_message(MgMediaDetail mrcp_detail);
    gboolean handle_base64_encoding_start_request(string call_id,string msg_id);
    gboolean handle_base64_encoding_stop_request(string call_id,string msg_id);
    xGateRetVal handle_pipeline_start_request(string call_id,string msg_id,STT_TTS_STATE mrcpState,xGateMrcpCommand mrcpCommand);
    gboolean handle_pipeline_stop_request(string call_id,string msg_id,STT_TTS_STATE mrcpState,xGateMrcpCommand mrcpCommand);
    xGateRetVal handle_media_event(xGateMgMsg *pMgMsg);
    xGateRetVal handle_stt_tts_msg(xGateMRCPMsg *);
    
  private:
    xGateRetVal handle_stt_tts_channel_req(xGateMgMsg* pMgMsg);
    xGateRetVal post_media_detail(xGateMgMsg *pMsg);
    xGateMgMsg* m_pmgMsg;
#ifdef XGATE_HAS_SELF_AUDIT
    xGateRetVal handle_audit_msg(xGateAuditMsg* pMsg);
#endif
};

#endif
