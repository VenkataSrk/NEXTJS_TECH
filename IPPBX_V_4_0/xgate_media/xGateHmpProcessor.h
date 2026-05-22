#ifndef _XGATE_HMP_CHANNEL_HANDLER_H
#define _XGATE_HMP_CHANNEL_HANDLER_H

//local includes
#include "xGateProcessor.h"
#include "xGateUtil.h"
#include "xGateMgMsg.h"

class xGateHmpProcessor :  public xGateProcessor
//class xGateHmpProcessor
{
  public:
    xGateHmpProcessor(const char*);
    virtual ~xGateHmpProcessor(void);
    virtual xGateRetVal process_msg(xGateBaseMsg*);
    xGateRetVal (*functionptr)(xGateMgMsg *pMgMsg);
    //xGateRetVal post_media_detail(xGateMgMsg *pMgMsg);
  private:
    xGateRetVal handle_channel_req(xGateMgMsg* pMgMsg);
    xGateRetVal post_media_detail(xGateMgMsg *pMgMsg);
};

#endif
