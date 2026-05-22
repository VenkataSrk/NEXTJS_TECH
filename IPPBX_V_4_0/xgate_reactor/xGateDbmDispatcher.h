#ifndef _XGATE_DBM_DISMGPATCHER_H
#define _XGATE_DBM_DISMGPATCHER_H

//local includes
#include "xGateUtil.h"
#include "xGateDispatcher.h"
#include "xGateDbmMsg.h"

class xGateDbmDispatcher :  public xGateDispatcher
{
  public:
    xGateDbmDispatcher(const char*);
    virtual ~xGateDbmDispatcher(void);
    virtual int svc( void );
    virtual xGateRetVal process_msg(ACE_Message_Block*);

  private:
    bool readTemplate();
    bool encode_msg(xGateDbmMsg *pMsg);
    bool dispatch_msg(xGateDbmMsg *pMsg);
    bool post(xGateDbmMsg *pMsg);
    bool getInputDoc(string &input, Document &doc);

    //member variables
    Document *m_doc;
    StringBuffer *m_strBuf;
};

#endif
