#ifndef _XGATE_HTTP_SERVICE_MSG_H
#define _XGATE_HTTP_SERVICE_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateHTTPUtil.h"
#include "xGateHttpHandler.h"
class xGateHttpServiceMsg : public xGBaseMsg
{
  public:
    xGateHttpServiceMsg(void) :
      xGBaseMsg(IURDefines::UR_MODULE_HTTP_SERVICE){
        m_msgType = HTTP_SRV_MSG_TYPE_REQUEST;
    };
    virtual ~xGateHttpServiceMsg(){};

    HttpRequestInfo& get_http_request_info() {
      return m_httpReqInfo;
    }

    void set_http_request_info(HttpRequestInfo &httpReqInfo) {
      m_httpReqInfo = httpReqInfo;
    }
    typedef enum __http_srv_msg_type{
	 HTTP_SRV_MSG_TYPE_REQUEST = 0xfff1,
 	HTTP_SRV_MSG_TYPE_RESPONSE= 0xfff2
    }HTTP_SRV_MSG;

  void setHttpMsgType(HTTP_SRV_MSG msgType)
  {
        m_msgType = msgType;
  }
   HTTP_SRV_MSG getHttpmsgType() const
  {
        return m_msgType;
   }


  protected:
    xGateHttpServiceMsg& operator=(const xGateHttpServiceMsg&);

  private:
    // Not Implemented Methods
    xGateHttpServiceMsg(const xGateHttpServiceMsg&);
    HttpRequestInfo m_httpReqInfo;
    HTTP_SRV_MSG m_msgType;
};
#endif
