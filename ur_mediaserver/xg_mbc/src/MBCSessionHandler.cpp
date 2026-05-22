//local includes
#include "MBCSessionHandler.h"
#include "xglog.h"
#define THISMODULE "HmpProc"

MBCSessionHandler::MBCSessionHandler(const char *serverType):xGateProcessor(serverType)
{
  XGLOG_INFO( "MBCSessionHandler::~MBCSessionHandler");
}

MBCSessionHandler::~MBCSessionHandler(void)
{
  XGLOG_INFO( "MBCSessionHandler::~MBCSessionHandler");
}
xGateRetVal MBCSessionHandler::process_msg(xGateBaseMsg*)
{
  return EN_XGATE_STATUS_SUCCESS;
}
//xGateRetVal (*functionptr)(xGateMgMsg *pMgMsg);

xGateRetVal MBCSessionHandler::handle_channel_req(xGateMgMsg* pMgMsg)
{
  return EN_XGATE_STATUS_SUCCESS;
}
xGateRetVal MBCSessionHandler::post_media_detail(xGateMgMsg *pMgMsg)
{
  return EN_XGATE_STATUS_SUCCESS;
}
 
