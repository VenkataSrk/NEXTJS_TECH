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
    int create_media_msg(xGateMgMsg *pMgMsg, string &jsonBuff, int &gatewayId);
    xGateRetVal dispatch_msg(const char *data, int len, int mgId);
    xGateRetVal send_msg_on_tcp(unsigned int fd, const char *pBuffer, int len);
    Value setJsonParam(string strValue,Document &doc);
    Value setJsonParam(unsigned int iValue,Document &doc);
    Value setJsonParam(MgMediaDetail& media, Document &doc);

 private:
};

#endif
