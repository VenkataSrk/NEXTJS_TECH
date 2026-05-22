#ifndef _XGATE_TCP_SOCKET_HANDLER_H
#define _XGATE_TCP_SOCKET_HANDLER_H
//system includes
#include <map>

//ace includes
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/Task_T.h>
#include <ace/SOCK_Stream.h>

//local includes
#include "MediaServiceUtil.h"

using namespace std;

struct completedMsgBuf
{
  string msg; //one extracted complete msg
  unsigned int msgLen; //length of one extracted complete msg

  completedMsgBuf() : msg(""), msgLen(0){};
  ~completedMsgBuf() {
    msg.clear();
    msgLen = 0;
  };
  completedMsgBuf(const completedMsgBuf& rhs) {
    msg = rhs.msg;
    msgLen = rhs.msgLen;
  };
  completedMsgBuf& operator= (const completedMsgBuf& rhs) {
    msg = rhs.msg;
    msgLen = rhs.msgLen;
    return *this;
  };
};

class TcpJsonPendingMsg
{
  public:
    bool isPending; //pending msg is available or not
    string delimiter;
    string pendingMsg; //incomplete pending json msg in buffer
    unsigned int pendingMsgLen; //length of incomplete pending json msg in buffer
    completedMsgBuf completedMsg;
    TcpJsonPendingMsg();
    ~TcpJsonPendingMsg();
    xGateRetVal get_one_complete_msg();
};

class MediaTcpSocketHandler : public ACE_Event_Handler
{
  public:
    MediaTcpSocketHandler(ACE_Reactor*,int,unsigned int);
    virtual ~MediaTcpSocketHandler(void);

    xGateRetVal init(ACE_SOCK_Stream& peerStream);
    int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
    int handle_input(ACE_HANDLE handle);
    void  post(const char *,int);

    ACE_HANDLE get_handle(void) const;
    ACE_Reactor* getReactor() 
    { 
      return m_reactor;
    }
    ACE_SOCK_Stream& peer() const;
    ACE_INET_Addr& get_remote_conn_addr() const; 

  private:
    MediaTcpSocketHandler(const MediaTcpSocketHandler& rhs);
    MediaTcpSocketHandler& operator= (const MediaTcpSocketHandler& rhs);
    xGateRetVal send_connection_closed(); 

    ACE_SOCK_Stream m_peer; 
    ACE_Reactor *m_reactor;
    TcpJsonPendingMsg *m_recvdBuf; 
    unsigned char m_socketData[XGATE_TCP_READ_SIZE];
    ACE_INET_Addr m_localAddr;
    ACE_INET_Addr m_remoteAddr;
    string m_remoteIpAddr;
    string m_peerFQDN;
    int m_maxMsgSize;
    int m_moduleId;
    unsigned int m_connId;
};

#endif
