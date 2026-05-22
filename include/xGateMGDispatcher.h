#ifndef _XGATE_DISMGPATCHER_H
#define _XGATE_DISMGPATCHER_H

//local includes
#include "../include/xGateUtil.h"
#include "../include/xGateDispatcher.h"
#include "../include/xGateMgMsg.h"

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
      Value setJsonParam(string strValue,Document &doc);
      Value setJsonParam(unsigned int iValue,Document &doc);
      Value setJsonParam(MgMediaDetail& media, Document &doc);
      Value setJsonParam(TtsDisconnectInfo& disconn_info, Document &doc);

   private:
      int create_media_msg(xGateMgMsg* pMgMsg, string &jsonBuff, int &gatewayId);
};

#endif
