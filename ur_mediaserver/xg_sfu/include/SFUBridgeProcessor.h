#ifndef _SFU_BRIDGE_PROCESSOR_
#define _SFU_BRIDGE_PROCESSOR_

//local includes
#include "xGateProcessor.h"
#include "xGateUtil.h"
#include "xGateMgMsg.h"

class SFUBridgeProcessor :  public xGateProcessor
{
public:
  SFUBridgeProcessor(const char*);
  virtual ~SFUBridgeProcessor(void);

  virtual int svc( void );
  virtual xGateRetVal process_msg(xGateBaseMsg*);
  xGateRetVal decode_media_msg(xGateMgMsg *pMgMsg);
  xGateRetVal post_media_detail_mbc_fwd_ctlr(xGateMgMsg *pMgMsg);
  xGateRetVal handle_channel_req(xGateMgMsg* pMgMsg);
private:

};

#endif

