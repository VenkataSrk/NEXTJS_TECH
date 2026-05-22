#ifndef _XGATE_DBM_PROCESSOR_H
#define _XGATE_DBM_PROCESSOR_H

//local includes
#include "xGateProcessor.h"
#include "xGateDbmMsg.h"

class xGateDbmProcessor :  public xGateProcessor
{
  public:
    xGateDbmProcessor(const char*);
    virtual ~xGateDbmProcessor(void);

    virtual xGateRetVal process_msg(xGateBaseMsg*);
    xGateRetVal decode_message(xGateDbmMsg *pMsg);
    bool parseDbmMsg(const char *jsonData, Document &doc);
    bool validateDbmMsg(Document &doc, xGateDbmMsg *pMsg);
    bool post(xGateDbmMsg *pMsg, xGateDbmMsg *pDbmMsg); 
};

#endif
