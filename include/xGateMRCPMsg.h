#ifndef _XGATE_MRCP_MSG_H
#define _XGATE_MRCP_MSG_H

//local includes
#include "xGateBaseMsg.h"

class xGateMRCPMsg : public xGateBaseMsg {
 
 public:
    //OBJPOOL_SUPPORT;
    

    xGateMRCPMsg() : xGateBaseMsg(EN_XGATE_MSG_UNKNOWN){
    }

    virtual ~xGateMRCPMsg() {
    }


    xGateMgMsgType get_mg_msg_type() {
      return m_mgMsgType;
    }

    void set_mg_msg_type(xGateMgMsgType type) {
      m_mgMsgType = type;
    }

    MRCPMediaDetail &get_mrcp_media_detail() {
      return m_mrcp_detail;
    }

    void set_mrcp_media_detail(MRCPMediaDetail &mrcp_detail){
        m_mrcp_detail = mrcp_detail;
    }


    const char * &get_mrcp_session_detail() {
      return m_mrcpSessionDetails;
    }
   
    void set_mrcp_session_detail(const char* sessionDetail){
        m_mrcpSessionDetails = sessionDetail;
    }

    void get_net_con_info(xGateNetConnection &netConInfo) {
      netConInfo = m_netConInfo;   
    }

    void set_net_con_info(xGateNetConnection &netConInfo) {
      m_netConInfo = netConInfo;  
    }

    void set_mrcp_call_id(std::string callId)
    {
      m_callId = callId;
    }

    std::string get_mrcp_call_id()
    {
      return m_callId;
    }

    void set_mrcp_uid(std::string uid)
    {
      m_uid = uid;
    }

    std::string get_mrcp_uid()
    {
      return m_uid;
    }

    void set_mrcp_dtmf_digit(std::string dtmf_digit)
    {
      m_dtmf_digit = dtmf_digit;
    }

    std::string get_mrcp_dtmf_digit()
    {
      return m_dtmf_digit;
    }


    void set_mrcp_stt_info(unsigned int mrcp_isSttEnabled)
    {
    m_mrcp_isSttEnabled =  mrcp_isSttEnabled;
    }

    unsigned int get_mrcp_stt_info()
    {
    return m_mrcp_isSttEnabled;
    } 

#if 1
    xGateMrcpCommand get_mrcp_command() {
	    return m_mrcpcommand;
    }

    void set_mrcp_command(xGateMrcpCommand mrcpCommand){
	    m_mrcpcommand = mrcpCommand;
    }
#endif

    void set_disconnection_code(std::string disconnCode)
    {
	    m_disconnCode = disconnCode;
    }

    std::string get_disconnection_code()
    {
	    return m_disconnCode;
    }

    void set_disconnection_reason(std::string disconnReason)
    {
	    m_disconnReason = disconnReason;
    }

    std::string get_disconnection_reason()
    {
	    return m_disconnReason;
    }

    void set_transfer_to_number(std::string transfer_to)
    {
	    m_transfer_to = transfer_to;
    }

    std::string get_transfer_to_number()
    {
	    return m_transfer_to;
    }


  private:

    xGateMRCPMsg(const xGateMRCPMsg&);
    xGateMRCPMsg& operator=(const xGateMRCPMsg&);
    xGateMgMsgType m_mgMsgType;
    xGateMrcpCommand m_mrcpcommand;
    MRCPMediaDetail m_mrcp_detail;
    xGateNetConnection m_netConInfo;
    MgMediaDetail m_media_detail;
    const char *m_mrcpSessionDetails;
    std::string m_callId;
    std::string m_uid;
    std::string m_resourceId;
    unsigned int m_mrcp_isSttEnabled;
    std::string m_disconnCode;
    std::string m_disconnReason;
    std::string m_dtmf_digit;
    std::string m_transfer_to;
    std::string m_filepath;
};

#endif

