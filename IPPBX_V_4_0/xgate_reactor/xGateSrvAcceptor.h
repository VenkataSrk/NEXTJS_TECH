#ifndef _XGATE_MGACCEPTOR_H
#define _XGATE_MGACCEPTOR_H

//ace includes

//local includes
#include "xGateTCPServer.h"

class xGateSrvAcceptor : public xGateTCPServer
{
  public:
    xGateSrvAcceptor (void);

    virtual ~xGateSrvAcceptor (void);

    virtual int handle_connection(ACE_HANDLE aceHandle);

  private:

    xGateSrvAcceptor(const xGateSrvAcceptor& rhs);

    xGateSrvAcceptor& operator= (const xGateSrvAcceptor& rhs);

};

#endif 
