/**
 * Sample code from The ACE Programmer's Guide,
 * copyright 2003 Addison-Wesley. All Rights Reserved.
 */

#ifndef __CLIENTSERVICE_H_
#define __CLIENTSERVICE_H_

#include "ace/Synch_Traits.h"
#include "ace/Null_Condition.h"
#include "ace/Null_Mutex.h"

#include "xGateProcessor.h"
#include "xGateUtil.h"
#include "xGateMgMsg.h"
#include "xGateMBCServiceMsg.h"
#include "MediaServiceDefine.h"

// Listing 3 code/ch07
#include "ace/Message_Block.h"
#include "ace/SOCK_Stream.h"
#include "ace/Svc_Handler.h"
#include "ace/Reactor_Notification_Strategy.h"
#include "ace/Thread.h"
#include "ace/Synch.h"

int createSfu(int);

typedef enum
{
    MSG_TYPE_SFU_ID =0,
    MSG_TYPE_JSON
}MsgType_t;

typedef struct MBC_SFU_MsgSt_t
{
    int msg_type;
    int sfu_id;
    char msg[10000];
}MBC_SFU_MsgSt_t;

class ClientService : public xGateProcessor
{

public:
  ClientService(const char*);
  virtual ~ClientService(void);
  virtual xGateRetVal process_msg(xGateBaseMsg*);
  xGateRetVal decode_media_msg(xGateMBCServiceMsg *ptmediaBCastMsg);
  xGateRetVal create_doc_object(Document &doc, const char *jsonData, Value& dataMemObj);
  xGateRetVal sendSfuDisconnectionToBridgeServer(xGateMBCServiceMsg *ptmediaBCastMsg);

  void SendMsg(char *data, int len, xGateNetConnection &netConInfo, xGateMgMsg* pMgMsg);
  xGateRetVal dispatch_msg(const char *data, int len, xGateNetConnection &netConInfo);
  xGateRetVal send_msg_on_tcp(unsigned int fd, const char *pBuffer, int len);
};
// Listing 3

#endif /* __CLIENTSERVICE_H_ */
