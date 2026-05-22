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
#include "MediaTcpSocketHandler.h"

#define MEDIATCPRECEIVER TcpReceiver::instance()
class MediaTcpReceiver : public ACE_Event_Handler
{
  public:
    MediaTcpReceiver();
    virtual ~MediaTcpReceiver(void);

    xGateRetVal init(void);
    static void run(void* arguments);
    xGateRetVal stop_thread( void );

    ACE_Reactor * get_reactor(void);
    int handle_timeout(const ACE_Time_Value&, const void *arg);

    /// Map to store FD to SocketHandler Map
    void addFdToSocketHandlerMap(int ,MediaTcpSocketHandler * const);
    xGateRetVal getHandlerFromFdToSocketHandlerMap(int ,MediaTcpSocketHandler *&);
    void removeFromFdToSocketHandlerMap(int);

  private:
    MediaTcpReceiver(const MediaTcpReceiver& rhs);
    MediaTcpReceiver& operator= (const MediaTcpReceiver& rhs);

    ACE_Reactor *m_reactor;
    bool m_running;
    map<int, MediaTcpSocketHandler*>m_fdToSocketHandlerMap;
};

typedef ACE_Unmanaged_Singleton<MediaTcpReceiver, ACE_Null_Mutex> TcpReceiver;
#endif
