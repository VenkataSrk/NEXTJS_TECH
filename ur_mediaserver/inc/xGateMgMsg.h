#ifndef _XGATE_MG_MSG_H
#define _XGATE_MG_MSG_H

//local includes
#include "xGateBaseMsg.h"
//#include "task.h"

class xGateMgMsg : public xGateBaseMsg {
 
 public:
    //OBJPOOL_SUPPORT;

    xGateMgMsg() : xGateBaseMsg(EN_XGATE_MSG_MGC_MG_IN) {
      m_data.clear();
      m_dataLen=0;
      m_mgMsgType = EN_XGATE_MG_TYPE_UNKNOWN;
    }

    virtual ~xGateMgMsg() {
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

    xGateMgMsgType get_mg_msg_type() {
      return m_mgMsgType;
    }

    void set_mg_msg_type(xGateMgMsgType type) {
      m_mgMsgType = type;
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

    MgMediaDetail & get_media_detail() {
      return m_media_detail;
    }
    MgMediaDetail & get_participant_list() {
      return m_media_detail;
    }

    void set_media_detail(MgMediaDetail & media_detail) {
      m_media_detail = media_detail;
      m_media_detail.audioDetail.m_dynamicCodec = media_detail.audioDetail.m_dynamicCodec;
      m_media_detail.videoDetail.m_dynamicCodec = media_detail.videoDetail.m_dynamicCodec; 
    } 

    void set_participant_list (MgMediaDetail & media_detail) {
        m_media_detail = media_detail;
        m_media_detail.Participant_list = media_detail.Participant_list;
    }

#if 0
    void set_task(CTask *pTask) {
      m_pTask = pTask;
    }

    CTask * get_task() {
      return m_pTask;
    }
#endif
    int get_sfu_id()
    {
      return m_sfuId;
    }
    void set_sfu_id(int sfu_id)
    {
      m_sfuId = sfu_id;
    }

  private:

    xGateMgMsg(const xGateMgMsg&);
    xGateMgMsg& operator=(const xGateMgMsg&);

    string m_data;
    unsigned int m_dataLen;
    xGateMgMsgType m_mgMsgType;
    //Connection Details, on which Msg is received.
    xGateNetConnection m_netConInfo;
    unsigned short m_srcPort;

    MgMediaDetail m_media_detail;
    int m_sfuId;
    //CTask *m_pTask;
};

typedef struct msgSfuId_t
{
    int sfuId;
    xGateMgMsg *pMgMsg;
}msgSfuId_t;

#endif

