#ifndef _MBCLINK_TCP_RECEIVER_H
#define _MBCLINK_TCP_RECEIVER_H

//ace includes
#include <ace/Singleton.h>
#include <ace/Reactor.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch.h>
#include <ace/OS.h>
#include <ace/Task.h>

//local includes
#include "xGateUtil.h"
#include "MBCLinkTcpSocketHandler.h"

#define MBCLINKTCPRECEIVER TcpReceiver::instance()
class MBCLinkTcpReceiver : public ACE_Event_Handler
{
  public:
    MBCLinkTcpReceiver();
    virtual ~MBCLinkTcpReceiver(void);

    xGateRetVal init(void);
    static void run(void* arguments);
    xGateRetVal stop_thread( void );

    ACE_Reactor * get_reactor(void);
    int handle_timeout(const ACE_Time_Value&, const void *arg);

    /// Map to store FD to SocketHandler Map
    void addFdToSocketHandlerMap(int ,MBCLinkTcpSocketHandler * const);
    xGateRetVal getHandlerFromFdToSocketHandlerMap(int ,MBCLinkTcpSocketHandler *&);
    void removeFromFdToSocketHandlerMap(int);

  private:
    MBCLinkTcpReceiver(const MBCLinkTcpReceiver& rhs);
    MBCLinkTcpReceiver& operator= (const MBCLinkTcpReceiver& rhs);

    ACE_Reactor *m_reactor;
    bool m_running;
    map<int, MBCLinkTcpSocketHandler*>m_fdToSocketHandlerMap;
};

typedef ACE_Unmanaged_Singleton<MBCLinkTcpReceiver, ACE_Null_Mutex> TcpReceiver;
#endif
