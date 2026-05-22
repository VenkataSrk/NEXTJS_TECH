#ifndef _XGATE_CONTROLLER_MSG_H__
#define _XGATE_CONTROLLER_MSG_H__

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
class xGateControllerMsg : public xGateBaseMsg
{
  public:
    xGateControllerMsg(void) :
      xGateBaseMsg(IURDefines::UR_MODULE_SERVICE_CONTROLLER){};
    virtual ~xGateControllerMsg(){};

  protected:
    xGateControllerMsg& operator=(const xGateControllerMsg&);

  private:
    // Not Implemented Methods
    xGateControllerMsg(const xGateControllerMsg&);
};
#endif
