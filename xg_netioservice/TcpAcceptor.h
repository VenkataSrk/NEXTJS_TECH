#ifndef _XGATE_TCP_SERVER_H
#define _XGATE_TCP_SERVER_H

//ace includes
#include <ace/Event_Handler.h>
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>

//local includes
#include "xGateUtil.h"
#include "NetIOServiceDefine.h"

#define TCPACCEPTOR XGTcpAcceptor::instance()
class TcpAcceptor : public ACE_Event_Handler
{
  public:
    typedef ACE_Acceptor<TcpAcceptor, ACE_SOCK_ACCEPTOR> Acceptor;
    TcpAcceptor();
    virtual ~TcpAcceptor(void);

    xGateRetVal init (vector<TcpLisentAddr>);
    static void run (void* arguments);
    xGateRetVal stop_thread( void );

    int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
    int handle_input (ACE_HANDLE);
    int handle_timeout(const ACE_Time_Value&, const void *arg);
    virtual int handle_connection(ACE_HANDLE aceHandle);

private:
    TcpAcceptor(const TcpAcceptor& rhs);
    TcpAcceptor& operator= (const TcpAcceptor& rhs);

protected:
    bool m_running;
    vector<ACE_SOCK_Acceptor*> m_acceptorList;
    ACE_Reactor *m_reactor;
};

typedef ACE_Unmanaged_Singleton<TcpAcceptor, ACE_Null_Mutex> XGTcpAcceptor;
#endif

