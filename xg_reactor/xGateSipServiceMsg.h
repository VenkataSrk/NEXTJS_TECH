#ifndef _XGATE_SIP_SERVICE_MSG_H
#define _XGATE_SIP_SERVICE_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
class xGateSipServiceMsg : public xGateBaseMsg
{
  public:
    xGateSipServiceMsg(void) : xGateBaseMsg(IURDefines::UR_MODULE_SIPSIGNAL_SERVICE)
    {};
    virtual ~xGateSipServiceMsg(){};

  protected:
    xGateSipServiceMsg& operator=(const xGateSipServiceMsg&);

  private:
    // Not Implemented Methods
    xGateSipServiceMsg(const xGateSipServiceMsg&);
};
#endif
