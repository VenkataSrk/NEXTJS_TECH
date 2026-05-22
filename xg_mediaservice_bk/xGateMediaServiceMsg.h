#ifndef _XGATE_MEDIA_SERVICE_MSG_H
#define _XGATE_MEDIA_SERVICE_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
#include "MediaServiceDefine.h"
class xGateMediaServiceMsg : public xGateBaseMsg
{
  public:
    xGateMediaServiceMsg(void) :
      xGateBaseMsg(IURDefines::UR_MODULE_MEDIA_PROXY_SERVICE){};
    virtual ~xGateMediaServiceMsg(){};

    MgMediaDetail& get_media_detail() {
      return m_mediaDetail;
    }

    void set_media_detail(MgMediaDetail& mediaDetail) {
      m_mediaDetail = mediaDetail;
    }

  protected:
    xGateMediaServiceMsg& operator=(const xGateMediaServiceMsg&);

  private:
    // Not Implemented Methods
    xGateMediaServiceMsg(const xGateMediaServiceMsg&);
    MgMediaDetail m_mediaDetail;
    xGateNetConnection m_netConInfo;
};
#endif
