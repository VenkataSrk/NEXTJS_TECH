#ifndef _XGATE_Business_MSG_H
#define _XGATE_Business_MSG_H

//local includes
#include "xGateBaseMsg.h"

class xGateBusinessMsg : public xGateBaseMsg 
{
  public:
    xGateBusinessMsg() : xGateBaseMsg(EN_XGATE_MSG_TO_BUSINESS), m_data(""), \
                     m_dataLen(0)
    {};

    virtual ~xGateBusinessMsg() {
      m_data.clear();
      m_dataLen=0;
    }

    void set_business_msg_type(xGateBusinessMsgType businessMsgType) {
      m_businessMsgType = businessMsgType;
    }

    xGateBusinessMsgType get_business_msg_type() {
      return m_businessMsgType; 
    }

    void set_data(char *&pInData) {
      m_data=pInData;
    }

    void get_data(const char *&pOutData) {
      pOutData=m_data.c_str();
    }

    void set_data_len(unsigned int inLen) {
      m_dataLen=inLen;
    }

    unsigned int get_data_len(void) {
      return m_dataLen;
    }

  private:
    xGateBusinessMsg(const xGateBusinessMsg&);
    xGateBusinessMsg& operator=(const xGateBusinessMsg&);

    string m_data;
    unsigned int m_dataLen;
    xGateBusinessMsgType m_businessMsgType;
};
#endif
