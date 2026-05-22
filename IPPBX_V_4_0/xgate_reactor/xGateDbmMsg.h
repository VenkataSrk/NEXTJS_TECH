#ifndef _XGATE_DBM_MSG_H
#define _XGATE_DBM_MSG_H

//local includes
#include "xGateBaseMsg.h"
#include "xGateHttpHandler.h"

class xGateDbmMsg : public xGateBaseMsg {

  public:
    xGateDbmMsg() : xGateBaseMsg(EN_XGATE_MSG_FROM_DBM), m_data(""), \
                     m_dataLen(0), m_httpReqInfo(NULL)
    {};

    virtual ~xGateDbmMsg() {
      m_data.clear();
      m_dataLen=0;
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

    void get_dbm_request_detail(DbmReqDetail &reqDetail) {
      reqDetail = m_reqDetail;
    }

    DbmReqDetail& get_dbm_request_detail() {
      return m_reqDetail;
    }

    void set_dbm_request_detail(DbmReqDetail &reqDetail) {
      m_reqDetail = reqDetail;
    }

    HttpRequestInfo* get_http_request_info() {
      return m_httpReqInfo;
    }
    
    void set_http_request_info(HttpRequestInfo *httpReqInfo) {
      m_httpReqInfo = httpReqInfo;
    }
  
  private:

    xGateDbmMsg(const xGateDbmMsg&);
    xGateDbmMsg& operator=(const xGateDbmMsg&);

    string m_data;
    unsigned int m_dataLen;
    DbmReqDetail m_reqDetail;
    HttpRequestInfo *m_httpReqInfo; 
};
#endif
