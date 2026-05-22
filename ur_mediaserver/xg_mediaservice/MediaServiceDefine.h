#ifndef _MEDIA_SERVICE_DEFINE_H
#define _MEDIA_SERVICE_DEFINE_H

#include "xGateDefine.h"
#include "IURDefines.h"
#include <string>
using namespace std;

#define XGATE_TCP_READ_SIZE  2048
#define MEDIA_MSG_TCP_PORT 32779
#define TCP_MAX_MESSAGE_SIZE 8192


class TcpConTuple
{
  public:
    int m_fd;
    string m_remoteIp;
    unsigned short m_remotePort;
    unsigned short m_localPort;
    unsigned short m_timeoutCount;
    long int m_timerId;

    TcpConTuple() :
      m_fd(-1), m_remoteIp(""), m_remotePort(0), m_localPort(0), 
      m_timerId(0), m_timeoutCount(0) {};

    ~TcpConTuple() {};

#if 0
    TcpConTuple& operator=(TcpConTuple &src)
    {
      m_fd = src.m_fd;
      m_remoteIp = src.m_remoteIp;
      m_remotePort = src.m_remotePort;
      m_localPort = src.m_localPort;
      m_timeoutCount = src.m_timeoutCount;
      m_timerId = src.m_timerId;
    }
#endif

    bool operator==(TcpConTuple &src) 
    {
      if(m_remoteIp == src.m_remoteIp && m_remotePort == src.m_remotePort \
          && m_fd == src.m_fd && m_localPort == src.m_localPort) {
        return true;
      } else {
        return false;
      }
    }
};

typedef enum 
{
 EN_XGATE_MSG_RECV=0,
 EN_XGATE_MSG_SEND,
 EN_XGATE_MSG_CONN_CLOSED,
 EN_XGATE_MSG_CONNECT_CLIENT,
 EN_XGATE_MSG_NEW_CLIENT,
 EN_XGATE_MSG_LISTEN,
 EN_XGATE_MSG_CLIENT_CONNECTED,
 EN_XGATE_MSG_REC_CLIENT_CONNECTED,
}xGateMSMsgType;
//class MediaTcpSocketHandler;
class MediaTimerData
{
  public: 
    int m_fd;
    string m_ip;
    //MediaTcpSocketHandler *m_pHandler;
    //ACE_SOCK_Stream m_newPeerStream;

    MediaTimerData() :
      m_fd(0), m_ip("") {}; 
    ~MediaTimerData() {};
    MediaTimerData(const MediaTimerData &src) 
    {
      m_fd = src.m_fd;
      m_ip = src.m_ip;
      //m_pHandler = src.m_pHandler;
      //m_newPeerStream = src.m_newPeerStream;
    };
    MediaTimerData& operator=(MediaTimerData &src)
    {
      m_fd = src.m_fd;
      m_ip = src.m_ip;
      //m_pHandler = src.m_pHandler;
      //m_newPeerStream = src.m_newPeerStream;
      return *this;
    };
};
 struct sessionDetail
{
  sessionDetail(string &uid, IURDefines::MODULE_ID srcModuleId) :
  m_uid(uid),m_srcModuleId(srcModuleId){};
  ~sessionDetail(){};
  IURDefines::MODULE_ID m_srcModuleId;
  std::string m_uid;
};




#endif
