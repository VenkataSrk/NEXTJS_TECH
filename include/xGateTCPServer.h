#ifndef _XGATE_TCP_SERVER_H
#define _XGATE_TCP_SERVER_H

//ace includes
#include <ace/Event_Handler.h>
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>

//local includes
#include "xGateUtil.h"

class xGateTCPServer : public ACE_Event_Handler
{

  public:
    typedef ACE_Acceptor<xGateTCPServer, ACE_SOCK_ACCEPTOR> Acceptor;

    xGateTCPServer (const char*);

    virtual ~xGateTCPServer (void);

    xGateRetVal init (vector<xGateListenAddress>);

    static void run (void* arguments);

    xGateRetVal stop_thread( void );

    int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

    int handle_input (ACE_HANDLE);

    int handle_timeout(const ACE_Time_Value&, const void *arg);

    virtual int handle_connection(ACE_HANDLE aceHandle);

private:
    xGateTCPServer(const xGateTCPServer& rhs);

    xGateTCPServer& operator= (const xGateTCPServer& rhs);

protected:
    bool m_running;

    char m_serverType[SERVER_TYPE_SIZE];

    vector<ACE_SOCK_Acceptor*> m_acceptorList;

    ACE_Reactor *m_reactor;
};

#endif

