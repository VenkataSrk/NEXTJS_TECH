#ifndef _XGATE_MBCLINK_SERVICE_MSG_H
#define _XGATE_MBCLINK_SERVICE_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
#include "MBCLinkServiceDefine.h"
class xGateMBCLinkServiceMsg : public xGateBaseMsg
{
  public:
    xGateMBCLinkServiceMsg(void) :
      xGateBaseMsg(IURDefines::UR_MODULE_MBC_LINK_SERVICE), m_strMsg("") {};
    virtual ~xGateMBCLinkServiceMsg(){};

    MgMediaDetail& get_media_detail() {
      return m_mediaDetail;
    }

    void set_media_detail(MgMediaDetail& mediaDetail) {
      m_mediaDetail = mediaDetail;
    }

    void setMsg(std::string msg)
    {
      m_strMsg = msg;
    }

    const char* getMsg() const
    {
      return m_strMsg.c_str();
    }

  protected:
    xGateMBCLinkServiceMsg& operator=(const xGateMBCLinkServiceMsg&);

  private:
    // Not Implemented Methods
    xGateMBCLinkServiceMsg(const xGateMBCLinkServiceMsg&);
    MgMediaDetail m_mediaDetail;
    xGateNetConnection m_netConInfo;
    std::string m_strMsg;
};
#endif
