#ifndef _XGATE_MBC_SERVICE_DEFINE_H
#define _XGATE_MBC_SERVICE_DEFINE_H
#include "IURDefines.h"

typedef enum
{
 EN_XGATE_MSG_XG_MBC_CLIENT_CONNECTED,
 EN_XGATE_MSG_XG_MBC_SFU_ID,
 EN_XGATE_MSG_XG_MBC_CLIENT,
 EN_XGATE_MSG_XG_MBC_SERVER,
 EN_XGATE_MSG_XG_MBC_CONN_CLOSED,
 EN_XGATE_MSG_XG_MBC_CONNECT_CLIENT,
} xGateMBCMsgType;

typedef enum
{
  EN_XG_MBC_EVENT_UNKNOWN=0,
  EN_XG_MBC_EVENT_INIT_LISTEN_REQ=1,
  EN_XG_MBC_EVENT_INIT_LISTEN_RES=2,
  EN_XG_MBC_EVENT_INIT_CONNECTION_REQ=3,
  EN_XG_MBC_EVENT_INIT_CONNECTION_RES=4,
  EN_XG_MBC_EVENT_SEND=5,
} xGateMBCServiceEvent;

typedef enum
{
  EN_XG_MBC_RES_STATUS_UNKNOWN=0,
  EN_XG_MBC_RES_STATUS_SUCCESS=1,
  EN_XG_MBC_RES_STATUS_FAILED=2,
} xGateMBCResponseStatus;

typedef enum
{
  EN_XG_MBC_CLIENT_UNKNOWN=0,
  EN_XG_MBC_CLIENT_PBX=1,
  EN_XG_MBC_CLIENT_SFU=2,
} xGateMBCClientType;

/* MBCTcpConnInfo class - Start
*/
class MBCTcpConnInfo
{
  public:
    MBCTcpConnInfo() : m_ip(""),m_port(0),m_connId(0),m_mbcClientType(EN_XG_MBC_CLIENT_UNKNOWN){}

    ~MBCTcpConnInfo(){}

    MBCTcpConnInfo(const MBCTcpConnInfo& rhs)
    {
      m_ip = rhs.m_ip;
      m_port = rhs.m_port;
      m_connId = rhs.m_connId;
      m_mbcClientType = rhs.m_mbcClientType;
    }

    MBCTcpConnInfo& operator=(const MBCTcpConnInfo& rhs)
    {
      m_ip = rhs.m_ip;
      m_port = rhs.m_port;
      m_connId = rhs.m_connId;
      m_mbcClientType = rhs.m_mbcClientType;
			return *this;
    }

    string m_ip;
    unsigned int m_port;
    unsigned int m_connId;
    xGateMBCClientType m_mbcClientType;
};
/* MBCTcpConnInfo class - End
*/

/* xGateMBCInfo class - Start
*/
class xGateMBCInfo
{
  public:
    xGateMBCInfo() : m_connectionId(0),m_mbcMediaMsg(""),
                           m_mbcResponseStatus(EN_XG_MBC_RES_STATUS_UNKNOWN){}

    ~xGateMBCInfo(){}

    xGateMBCInfo(const xGateMBCInfo& rhs)
    {
      m_connectionId = rhs.m_connectionId;
      m_mbcMediaMsg = rhs.m_mbcMediaMsg;
      m_mbcResponseStatus = rhs.m_mbcResponseStatus;    
    }	    

    xGateMBCInfo& operator=(const xGateMBCInfo& rhs)
    {
      m_connectionId = rhs.m_connectionId;
      m_mbcMediaMsg = rhs.m_mbcMediaMsg;
      m_mbcResponseStatus = rhs.m_mbcResponseStatus;    
			return *this;
    }

    int m_connectionId;
    string m_mbcMediaMsg;
    xGateMBCResponseStatus m_mbcResponseStatus;
};
/* xGateMBCInfo class - End
*/
#endif
