#ifndef _XGATE_SCHEDULER_MSG_H
#define _XGATE_SCHEDULER_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
#include "xGateBaseMsg.h"

class xGateSchedulerMsg : public xGateBaseMsg
{
  public:
    xGateSchedulerMsg() : xGateBaseMsg(EN_XGATE_MSG_SCHEDULER),
    m_uid(""), m_schedulerData(NULL) {};
    ~xGateSchedulerMsg(){};

    xGateSchedulerData* getSchedulerData() 
    { 
      return m_schedulerData; 
    };

    void setSchedulerData(xGateSchedulerData *schedulerData) 
    {
      m_schedulerData = schedulerData;
    };

    string& getUid() {
      return m_uid;
    }

    void setUid(string& uid) {
      m_uid = uid;
    }

  protected:
    xGateSchedulerMsg& operator=(const xGateSchedulerMsg&);

  private:
    // Not Implemented Methods
    xGateSchedulerMsg(const xGateSchedulerMsg&);
    xGateSchedulerData *m_schedulerData;
    string m_uid;
};
#endif
