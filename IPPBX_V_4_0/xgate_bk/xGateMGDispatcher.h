#ifndef _XGATE_DISMGPATCHER_H
#define _XGATE_DISMGPATCHER_H

//ace includes
//#include <ace/Task.h>

//local includes
#include "xGateUtil.h"
#include "xGateDispatcher.h"
#include "xGateMgMsg.h"
//#include "task.h"

class xGateMGDispatcher :  public xGateDispatcher
{
  public:
    xGateMGDispatcher(const char*);
    virtual ~xGateMGDispatcher(void);
    virtual int svc( void );
    virtual xGateRetVal process_msg(ACE_Message_Block*);
    xGateRetVal encode_msg(xGateMgMsg* pMgMsg);
    xGateRetVal dispatch_msg(const char *data, int len, xGateNetConnection &netConInfo);
    xGateRetVal send_msg_on_tcp(unsigned int fd, const char *pBuffer, int len);

 private:
    int create_media_msg(xGateMgMsg* pMgMsg, string &jsonBuff);
};

#endif
