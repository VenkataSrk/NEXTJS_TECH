#ifndef _XG_MBC_SERVICE_MSG_H
#define _XG_MBC_SERVICE_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
#include "xGateMBCServiceDefine.h"
class xGateMBCServiceMsg : public xGateBaseMsg
{
  public:
    xGateMBCServiceMsg(void) :
      xGateBaseMsg(IURDefines::UR_MODULE_XG_MBC_SERVICE){};

    virtual ~xGateMBCServiceMsg(){};

    typedef enum _xgmbc_event_type
    {
      XG_MBC_EVENT_UNKNOWN=0,
      XG_MBC_EVENT_INIT_CLIENT_CONNECTION_REQ=1,
      XG_MBC_EVENT_INIT_CLIENT_CONNECTION_RES=2,
      XG_MBC_EVENT_INIT_TCP_LISTEN_REQ=3,
      XG_MBC_EVENT_INIT_TCP_LISTEN_RES=4,
    }XGMBC_EVENT_TYPE;

    MBCTcpConnInfo& get_mbc_tcp_conn_info() {
      return m_mbcTcpConnInfo;
    }

    void set_mbc_tcp_conn_info(MBCTcpConnInfo &mbcTcpConnInfo) {
      m_mbcTcpConnInfo = mbcTcpConnInfo;
    }

    xGateMBCServiceEvent& get_mbc_event_type() {
      return m_mbcEventType;
    }

    void set_mbc_event_type(xGateMBCServiceEvent mbcEventType) {
	    m_mbcEventType = mbcEventType;
    }

    int getConnectionId(void){
	    return m_connId;
    }

    void setConnectionId(int connId)
    {
	    m_connId = connId;
    }

    int getSfuId(void){
	    return m_sfuId;
    }

    void setSfuId(int sfuId)
    {
	    m_sfuId = sfuId;
    }

    char *getMsg(void){
	    return data;
    }
    void setMsg(char* msg){
	    data = msg;
    }
    void setMsgLen(int msgLen){
	    len = msgLen;
    }
    int getMsgLen(void){
	    return len;
    }
    
  protected:
    xGateMBCServiceMsg& operator=(const xGateMBCServiceMsg&);

  private:
    // Not Implemented Methods
    xGateMBCServiceMsg(const xGateMBCServiceMsg&);
    xGateMBCServiceEvent m_mbcEventType;
    xGateMBCInfo m_mediaBroadcastInfo;
    MBCTcpConnInfo m_mbcTcpConnInfo;
    char *data;
    int len;
    int m_connId;
    int m_sfuId;
    
};
#endif
