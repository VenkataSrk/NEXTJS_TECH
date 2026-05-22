#ifndef _MBC_SESSION_HANDLER_
#define _MBC_SESSION_HANDLER_

//local includes
//#include "xGateProcessor.h"
#include "xGateUtil.h"
#include "xGateProcessor.h"
#include "xGateMgMsg.h"

class MBCSessionHandler :  public xGateProcessor 
{
public:
  MBCSessionHandler(const char*);
  virtual ~MBCSessionHandler(void);
  virtual xGateRetVal process_msg(xGateBaseMsg*);
  xGateRetVal (*functionptr)(xGateMgMsg *pMgMsg);
private:
  xGateRetVal handle_channel_req(xGateMgMsg* pMgMsg);
  xGateRetVal post_media_detail(xGateMgMsg *pMgMsg);
}; 

#endif // _MBC_SESSION_HANDLER_
