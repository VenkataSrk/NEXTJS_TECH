#ifndef _XGATE_MGSFUDISPATCHER_H
#define _XGATE_MGSFUDISPATCHER_H

//local includes
#include "xGateUtil.h"
#include "xGateDispatcher.h"
#include "xGateMgMsg.h"

class xGateMgSFUDispatcher :  public xGateDispatcher
{
  public:
    xGateMgSFUDispatcher(const char*);
    virtual ~xGateMgSFUDispatcher(void);
    virtual int svc( void );
    virtual xGateRetVal process_msg(ACE_Message_Block*);
    xGateRetVal encode_msg(xGateMgMsg* pMgMsg);
    xGateRetVal dispatch_msg(const char *data, int len, xGateNetConnection &netConInfo, xGateMgMsg* pMgMsg);
    xGateRetVal send_msg_on_tcp(unsigned int fd, const char *pBuffer, int len);
    Value setJsonParam(string strValue,Document &doc);
    Value setJsonParam(unsigned int iValue,Document &doc);
    Value setJsonParam(MgMediaDetail& media, Document &doc);
    Value setURMeetUserListParam(MgMediaDetail& media, Document &doc);

 private:
    int create_media_msg(xGateMgMsg* pMgMsg, string &jsonBuff, int &gatewayId);
};

#endif
