#ifndef _XGATE_TCP_ACCEPTOR_H
#define _XGATE_TCP_ACCEPTOR_H

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

    xGateRetVal init();
    static void run(void* arguments);
    xGateRetVal stop_thread(void);

    int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);
    int handle_input(ACE_HANDLE);

    int handle_timeout(const ACE_Time_Value&, const void *arg);
    virtual int handle_connection(ACE_HANDLE aceHandle);
    xGateRetVal add_new_server(string,int,int);

private:
    MediaTcpAcceptor(const MediaTcpAcceptor& rhs);
    MediaTcpAcceptor& operator=(const MediaTcpAcceptor& rhs);
    xGateRetVal send_new_client(unsigned int,int,string,int);
    bool remove_entry_from_acceptor_moduleId_map(string & m_connDetails);
protected:
    bool m_running;
    ACE_Recursive_Thread_Mutex m_tMtxServerList;
    vector<ACE_SOCK_Acceptor*> m_acceptorList;
    map<string,int> m_acceptorModuleId;
    ACE_Reactor *m_reactor;
};

typedef ACE_Unmanaged_Singleton<MediaTcpAcceptor, ACE_Null_Mutex> TcpAcceptor;
#endif

