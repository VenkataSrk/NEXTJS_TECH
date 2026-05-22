#ifndef _XGATE_BASE_MSG_H
#define _XGATE_BASE_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateDefine.h"
#include "xGateUtil.h"
#include "IURDefines.h"
#include "IURModuleMsg.h"

class xGateBaseMsg : public ACE_Message_Block,public IURModuleMsg
{
  public:
    //xGateBaseMsg(void) { };
    xGateBaseMsg(const xGateBaseMsg&);
    //xGateBaseMsg(IURDefines::MODULE_ID srcModuleId) : m_srcModuleId(srcModuleId) {}
    
    xGateBaseMsg(IURDefines::MODULE_ID srcModuleId) :
      m_srcModuleId(srcModuleId),
      m_dstModuleId(IURDefines::UR_MODULE_UNKNOWN),
      m_uid(""),
      m_opcode(0){};
      
    //DebugVM start
    xGateBaseMsg(xGateMsgType type): m_msgType(type) {
      this->msg_priority((unsigned long)EN_XGATE_MSG_PRIORITY_LOW);
    }
    //DebugVM end
    virtual ~xGateBaseMsg(){};

    void setSrcModuleId(IURDefines::MODULE_ID moduleId)
    {
      m_srcModuleId = moduleId;
    }

    IURDefines::MODULE_ID getSrcModuleId() const
    {
      return m_srcModuleId;
    }

    void setDstModuleId(IURDefines::MODULE_ID moduleId)
    {
      m_dstModuleId = moduleId;
    }

    IURDefines::MODULE_ID getDstModuleId() const
    {
     // return UR_MODULE_IVR_SERVICE;  
      return m_dstModuleId;
    }

    void setUid(std::string uid)
    {
      m_uid = uid;
    }

    std::string getUid() const
    {
      return m_uid;
    }

    void setOpCode(unsigned int opcode)
    {
      m_opcode = opcode;
    }

    unsigned int getOpCode() const
    {
      return m_opcode;
    }
    //DebugVM start
    const xGateMsgType& getMsgType(void) const {
      return m_msgType;
    }

    void setMsgType(xGateMsgType msgType) {
      m_msgType = msgType;
    }
   //DebugVM end
  protected:
    xGateBaseMsg& operator=(const xGateBaseMsg&);

  private:
    //DebugVM start
    xGateMsgType m_msgType;
    //DebugVM end

    std::string m_uid;
    unsigned int m_opcode;
    IURDefines::MODULE_ID m_srcModuleId;
    IURDefines::MODULE_ID m_dstModuleId;
};
#endif
