#ifndef _XGATE_BASE_MSG_H
#define _XGATE_BASE_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
class xGateBaseMsg : public ACE_Message_Block
{
  public:
    xGateBaseMsg(xGateMsgType  type): m_msgType(type) {
      this->msg_priority((unsigned long)EN_XGATE_MSG_PRIORITY_LOW);
    }
    virtual ~xGateBaseMsg(){};

    const xGateMsgType& getMsgType(void) const {
      return m_msgType;
    }

    void setMsgType(xGateMsgType msgType) {
      m_msgType = msgType;
    }
  protected:
    xGateBaseMsg& operator=(const xGateBaseMsg&);

  private:
    // Not Implemented Methods
    xGateBaseMsg(void);
    xGateBaseMsg(const xGateBaseMsg&);

    xGateMsgType m_msgType;
};
#endif
