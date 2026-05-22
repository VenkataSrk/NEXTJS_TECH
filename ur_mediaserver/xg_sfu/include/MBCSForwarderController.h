#ifndef _XGATE_HMP_CHANNEL_HANDLER_H
#define _XGATE_HMP_CHANNEL_HANDLER_H

//local includes
#include "xGateProcessor.h"
#include "xGateUtil.h"
#include "xGateMgMsg.h"

class MBCSForwarderController :  public xGateProcessor
{
  public:
    MBCSForwarderController(const char*);
    virtual ~MBCSForwarderController(void);
    virtual xGateRetVal process_msg(xGateBaseMsg*);
    xGateRetVal (*functionptr)(xGateMgMsg *pMgMsg);
  private:
    xGateRetVal handle_channel_req(xGateMgMsg* pMgMsg);
    xGateRetVal post_media_detail(xGateMgMsg *pMgMsg);
};

#endif

