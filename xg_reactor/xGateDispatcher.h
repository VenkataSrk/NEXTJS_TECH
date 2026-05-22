#ifndef _XGATE_DISPATCHER_H
#define _XGATE_DISPATCHER_H

//ace includes
#include <ace/Task.h>

//local includes
#include "xGateUtil.h"

class xGateDispatcher : public ACE_Task<ACE_MT_SYNCH>
{
  public:
    xGateDispatcher(const char*);
    virtual ~xGateDispatcher(void);

    xGateRetVal init();
    virtual int svc( void );
    virtual xGateRetVal process_msg(ACE_Message_Block*);
    xGateRetVal stop();

    bool m_run;

  private:
    xGateRetVal sendmsg_on_tcp(unsigned int, const char *, unsigned int, xGateTCPConnectionType eConnType);

  protected:
    char m_serverType[SERVER_TYPE_SIZE];
    char m_peerAddress[SERVER_ADDRESS_LENGTH];
};

#endif
