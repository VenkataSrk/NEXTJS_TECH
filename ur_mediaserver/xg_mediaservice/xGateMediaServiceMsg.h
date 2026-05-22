#ifndef _XGATE_MEDIA_SERVICE_MSG_H
#define _XGATE_MEDIA_SERVICE_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
#include "xGateBaseMsg.h"
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
    void set_netConnInfo(xGateNetConnection netInfo)
    {
      m_netConInfo = netInfo;
    }
    xGateNetConnection get_netConnInfo(void)
    {
      return m_netConInfo;	    
    }
    int get_connectionId(void)
    {
      return m_connectionId;	    
    }
    void setConnectionId(int connection_id)
    {
      m_connectionId = connection_id;	    
    }
    void set_msg(void *ptr){
      msg = ptr;  
    }
    void* get_msg(void){
       return msg;
    }
    void setMsgLen(int len){
      msg_len = len;
    }
    int getMsgLen(void){
      return msg_len;
    }
  protected:
    xGateMediaServiceMsg& operator=(const xGateMediaServiceMsg&);

  private:
    // Not Implemented Methods
    xGateMediaServiceMsg(const xGateMediaServiceMsg&);
    MgMediaDetail m_mediaDetail;
    xGateNetConnection m_netConInfo;
    int m_connectionId;
    void *msg;
    int msg_len;
};
#endif
