#ifndef _XGATE_TCP_SERVER_H
#define _XGATE_TCP_SERVER_H

//ace includes
#include <ace/Event_Handler.h>
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>

//local includes
#include "xGateUtil.h"
#include "MediaServiceDefine.h"

#define MEDIATCPACCEPTOR TcpAcceptor::instance()
class MediaTcpAcceptor : public ACE_Event_Handler
{
  public:
    typedef ACE_Acceptor<MediaTcpAcceptor, ACE_SOCK_ACCEPTOR> Acceptor;
    MediaTcpAcceptor();
    virtual ~MediaTcpAcceptor(void);

    xGateRetVal init (vector<TcpLisentAddr>);
    static void run (void* arguments);
    xGateRetVal stop_thread( void );

    int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
    int handle_input (ACE_HANDLE);
    int handle_timeout(const ACE_Time_Value&, const void *arg);
    virtual int handle_connection(ACE_HANDLE aceHandle);

private:
    MediaTcpAcceptor(const MediaTcpAcceptor& rhs);
    MediaTcpAcceptor& operator= (const MediaTcpAcceptor& rhs);

protected:
    bool m_running;
    vector<ACE_SOCK_Acceptor*> m_acceptorList;
    ACE_Reactor *m_reactor;
};

typedef ACE_Unmanaged_Singleton<MediaTcpAcceptor, ACE_Null_Mutex> TcpAcceptor;
#endif

