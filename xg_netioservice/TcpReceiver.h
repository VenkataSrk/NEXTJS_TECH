#ifndef _MEDIA_TCP_RECEIVER_H
#define _MEDIA_TCP_RECEIVER_H

//ace includes
#include <ace/Singleton.h>
#include <ace/Reactor.h>
#include <ace/Svc_Handler.h>
#include <ace/Synch.h>
#include <ace/OS.h>
#include <ace/Task.h>

//local includes
#include "xGateUtil.h"
#include "TcpSocketHandler.h"

#define TCPRECEIVER XGTcpReceiver::instance()
class TcpReceiver : public ACE_Event_Handler
{
  public:
    TcpReceiver();
    virtual ~TcpReceiver(void);

    xGateRetVal init(void);
    static void run(void* arguments);
    xGateRetVal stop_thread( void );

    ACE_Reactor * get_reactor(void);
    int handle_timeout(const ACE_Time_Value&, const void *arg);

    /// Map to store FD to SocketHandler Map
    void addFdToSocketHandlerMap(int ,TcpSocketHandler * const);
    xGateRetVal getHandlerFromFdToSocketHandlerMap(int ,TcpSocketHandler *&);
    void removeFromFdToSocketHandlerMap(int);

  private:
    TcpReceiver(const TcpReceiver& rhs);
    TcpReceiver& operator= (const TcpReceiver& rhs);

    ACE_Reactor *m_reactor;
    bool m_running;
    map<int, TcpSocketHandler*>m_fdToSocketHandlerMap;
};

typedef ACE_Unmanaged_Singleton<TcpReceiver, ACE_Null_Mutex> XGTcpReceiver;
#endif
