#ifndef _XGATE_DBCONFIG_SERVICE_MSG_H
#define _XGATE_DBCONFIG_SERVICE_MSG_H

//ace includes
#include <ace/Message_Block.h>

//local includes
#include "xGateUtil.h"
#include "UrMrcpDefines.h"

#define STR std::string

class xGateDbConfigMgrServiceMsg : public URMRCPBaseMsg
{
	public:

		xGateDbConfigMgrServiceMsg(void) :
			URMRCPBaseMsg(IURDefines::UR_MODULE_DBCONFIG_MGR_SERVICE),m_tSessionDetails(nullptr) {};
		virtual ~xGateDbConfigMgrServiceMsg(){};

		STR &get_mrcp_session_detail() {
			return m_mrcpSessionDetails;
		}

		void set_mrcp_session_detail(STR sessionDetail){
			m_mrcpSessionDetails = sessionDetail;
		}

		xGateMrcpCommand &get_mrcp_command() {
			return m_mrcpCommand;
		}

		void set_mrcp_command(xGateMrcpCommand mrcpCommand){
			m_mrcpCommand = mrcpCommand;
		}
	
		void set_call_id(STR callid)
		{
			m_callId = callid;
		}
		STR get_call_id()
		{
			return m_callId;
		}

		void set_uid(STR uid)
		{
				m_uid = uid;
		}

		STR get_uid()
		{
				return m_uid;
		}
		void set_stt_enabled(bool isEnabled)
		{
			m_isSttEnabled = isEnabled;
		}
		bool get_stt_enabled()
		{
			return m_isSttEnabled;
		}

		void set_display_text(STR displayText)
		{
			m_displayText = displayText.c_str();
		}
		STR get_display_text()
		{
			return m_displayText;
		}

		void set_transfer_to(STR transferTo)
		{
			m_transferTo = transferTo.c_str();
		}
		STR get_transfer_to()
		{
			return m_transferTo;
		}

		void set_fileDescriptor(int fileDescriptor)
		{
			m_fileDescriptor = fileDescriptor;
		}

		int get_fileDescriptor()
		{
			return m_fileDescriptor;
		}
		void set_asr_type(int type)
		{
			m_asrType = type;
		}

		int get_asr_type()
		{
			return m_asrType;
		}

		void set_tts_type(int type)
		{
			m_ttsType = type;
		}

		int get_tts_type()
		{
			return m_ttsType;
		}

		void set_CcaasAgentConnected(bool Conn)
		{
			m_isCcaasAgentConnected = Conn;
		}

		bool get_set_CcaasAgentConnected()
		{
			return m_isCcaasAgentConnected;
		}

		void set_ccaasCallStatus(STR callStatus)
		{
			m_ccaasCallStatus = callStatus.c_str();
		}

		STR get_ccaasCallStatus()
		{
			return m_ccaasCallStatus;
		}

		void set_chatbot_init(bool ischatbotinit)
		{
			m_bisccaasChatBotInit = ischatbotinit;
		}

		void set_base64Path(STR path)
		{
			m_filepath = path.c_str();
		}

		void fillServiceMsg(xGateDbConfigMgrServiceMsg *pMsg)
		{
			m_callId = pMsg->m_callId.c_str();
			m_uid = pMsg->m_uid.c_str();
			m_tSessionId = pMsg->m_tSessionId.c_str();
			m_tSessionDetails = pMsg->m_tSessionDetails;
			m_strCcaasBlockId = pMsg->m_strCcaasBlockId.c_str();
			m_strCcaasBlockType = pMsg->m_strCcaasBlockType.c_str();
			m_strCcaasChannelId = pMsg->m_strCcaasChannelId.c_str();
			m_strCcaasAgentId = pMsg->m_strCcaasAgentId.c_str();
			m_strCcaasBotUrl = pMsg->m_strCcaasBotUrl.c_str();
			m_fileDescriptor = pMsg->m_fileDescriptor;
			m_asrType = pMsg->m_asrType;
			m_ttsType = pMsg->m_ttsType;
			m_isCcaasAgentConnected = pMsg->m_isCcaasAgentConnected;
			m_ccaasCallStatus = pMsg->m_ccaasCallStatus;
			m_bisccaasChatBotInit = pMsg->m_bisccaasChatBotInit;
			m_filepath = pMsg->m_filepath;
			m_bisBotFirst = pMsg->m_bisBotFirst;
			m_iCcaasBotId = pMsg->m_iCcaasBotId;
			m_bIsSmartAssist = pMsg->m_bIsSmartAssist;
			m_enBotType = pMsg->m_enBotType;
		}

		STR m_callId;
		STR m_tSessionId;
		STR m_uid;
		int m_iMsgType;
		STR m_strCallingNumber;
		STR m_strCalledNumber;
		STR m_strDomainName;
		STR m_strDomainId;
		STR m_strContactAddress;
		STR m_convertedText;
		STR m_displayText;
		UrMrcpSessionDetails *m_tSessionDetails;
		STR xmlInfo;
		STR m_mrcpSessionDetails;
		xGateMrcpCommand m_mrcpCommand;
		bool m_isSttEnabled;
		STR m_disconnectCode;
		STR m_disconnectReason;
		STR m_dtmf_digit;
		STR m_transferTo;
        STR m_strCcaasBlockId;
        STR m_strCcaasBlockType;
        STR m_strCcaasChannelId;
		STR m_strCcaasAgentId;
		STR m_strCcaasBotUrl;
		int m_fileDescriptor;
		int m_asrType;
		int m_ttsType;
		bool m_isCcaasAgentConnected;
		STR m_ccaasCallStatus;
		bool m_bisccaasChatBotInit;
		STR m_filepath;
		bool m_bisBotFirst;
		int m_iCcaasBotId;
		STR m_strOldCallId;
		STR m_strOldUid;
		STT_TTS_STATE m_enSttState;
		list<UrMrcpSessionDetails*> m_lDisplayUsers;
		xGateCallType m_enCallType;
		bool m_bIsSmartAssist;
		EN_BOT_TYPE m_enBotType;
};

#endif

    
    
