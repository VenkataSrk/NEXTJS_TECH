#ifndef _XGATE_MG_MSG_H
#define _XGATE_MG_MSG_H

//local includes
#include "xGateBaseMsg.h"
#include "task.h"

class xGateIppbxMsg : public xGateBaseMsg {
 
 public:
    OBJPOOL_SUPPORT;

    xGateIppbxMsg() : xGateBaseMsg(EN_XGATE_MSG_MGC_MG_IN) {
      m_data.clear();
      m_dataLen=0;
      m_ippbxMsgType = EN_XGATE_IPPBX_TYPE_UNKNOWN;
    }

    virtual ~xGateIppbxMsg() {
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

    xGateIppbxMsgType get_Ippbx_type() {
      return m_ippbxMsgType;
    }

    void set_mg_msg_type(xGateIppbxMsgType type) {
      m_ippbxMsgType = type;
    }

    void get_net_con_info(xGateNetConnection &netConInfo) {
      netConInfo = m_netConInfo;   
    }

    void set_net_con_info(xGateNetConnection &netConInfo) {
      m_netConInfo = netConInfo;  
    } 

    /// get source port on which  this msg is received
    unsigned short get_src_port()
    {
      return m_srcPort;
    }
    void set_src_port(unsigned short port)
    {
      m_srcPort = port;
    }

	/*****************
    MgMediaDetail & get_media_detail() {
      return m_media_detail;
    }

    void set_media_detail(MgMediaDetail & media_detail) {
      m_media_detail = media_detail; 
    }
	***************/

    void set_task(CTask *pTask) {
      m_pTask = pTask;
    }

    CTask * get_task() {
      return m_pTask;
    }

  private:

    xGateIppbxMsg(const xGateIppbxMsg&);
    xGateIppbxMsg& operator=(const xGateIppbxMsg&);

    string m_data;
    unsigned int m_dataLen;
    xGateIppbxMsgType m_ippbxMsgType;
    //Connection Details, on which Msg is received.
    xGateNetConnection m_netConInfo;
    unsigned short m_srcPort;

/**********
    MgMediaDetail m_media_detail;
***********/

    CTask *m_pTask;
};

#endif

