#ifndef _XGATE_BUSINESS_SCHEDULER_H
#define _XGATE_BUSINESS_SCHEDULER_H

//local include
#include "xGateScheduler.h"
#include "xGateBusinessUtil.h"

class xGateBusinessScheduler : public xGateScheduler
{
  public:
    xGateBusinessScheduler(const char* schedulerType);
    virtual ~xGateBusinessScheduler();

    /// Called back on handle_time out
    int handle_timeout(const ACE_Time_Value&, const void *); 

  private:
    Document m_doc;
    StringBuffer m_strBuf;
    bool readTemplate();
    Value setJsonParam(string &strValue);
    Value setJsonParam(unsigned int iValue);
    //bool createSubDoc(string &jsonStr, Document &doc);
    bool createSubDoc(char *jsonStr, Document &doc);
    bool sendTimeEventResult(BusinessTimerData *data);
    bool post(const char* input, unsigned int len, unsigned int toWhom);
};

#endif
