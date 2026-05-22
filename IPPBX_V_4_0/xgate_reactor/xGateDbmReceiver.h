#ifndef _XGATE_DBM_RECEIVER_H
#define _XGATE_DBM_RECEIVER_H

//local includes
#include "xGateProcessor.h"
#include "xGateDbmMsg.h"

class xGateDbmReceiver :  public xGateProcessor
{
  public:
    xGateDbmReceiver(const char*);
    virtual ~xGateDbmReceiver(void);

    virtual xGateRetVal process_msg(xGateBaseMsg*);
    bool check_http_response(xGateDbmMsg *pMsg);
    bool post(HttpRequestInfo *httpReqInfo);
};

#endif
