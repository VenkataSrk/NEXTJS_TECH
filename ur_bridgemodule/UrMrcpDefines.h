#ifndef __URMRCPDefines_H__
#define __URMRCPDefines_H__
#include <map>

#define STR std::string
#define SET(type, c, name) void set##name(type name){m_##c##name = name;}
#define GET(c, name) auto get##name(){return m_##c##name;}
#define GETSTR(c, name) auto get##name(){return m_##c##name.c_str();}

typedef enum
{
	EN_SESSION_RECOG_UNKNOWN,
	EN_SESSION_RECOG_START,
	EN_SESSION_RECOG_PAUSE,
	EN_SESSION_RECOG_RESUME,
	EN_SESSION_RECOG_STOP,
	EN_SESSION_RECOG_COMPLETED,
}session_recognize_state;

typedef enum
{
	EN_SESSION_SYNTH_UNKNOWN,
	EN_SESSION_SYNTH_START,
	EN_SESSION_SYNTH_STOP,
	EN_SESSION_SYNTH_COMPLETED,
}session_synthesize_state;

typedef enum 
{
	AVAAMO_UNKNOWN_BOT,
	AVAAMO_LAWFIRM_BOT,
	AVAAMO_RESTRARUNT_BOT,
	AVAAMO_SUBSCRIBER_BOT,
	AVAAMO_MEETING_ASSISTANT_BOT
}avaamo_bot_name;

typedef enum
{
	EN_CCAAS_USER_TYPE_UNKNOWN = 0,
	EN_CCAAS_USER_TYPE_CUSTOMER = 1,
	EN_CCAAS_USER_TYPE_AGENT = 2,
	EN_CCAAS_USER_TYPE_SUPERVISOR = 3,
  	EN_CCAAS_USER_TYPE_TRANSFER_AGENT = 4,
  	EN_CCAAS_USER_TYPE_TRANSFEROR_AGENT = 5,
}xGateCcaasUserType;

enum class EN_MEMBER_TYPE
{
	EN_MEMBER_TYPE_UNKNOWN,
	EN_MEMBER_TYPE_PREMIUM,
	EN_MEMBER_TYPE_STANDARD,
	EN_MEMBER_TYPE_FREE,
};

enum class xGatePubChannelId
{
	EN_PUB_ID_UNKNOWN = 0,
	EN_CCAAS_BOT,
	EN_CCAAS_AGENT_CONNECTED,
	EN_CCAAS_ACD_MG_COMM,
	EN_CCAAS_VMS_TRANSCRIPT_REQ,
	EN_CCAAS_VMS_TRANSCRIPT_NODE,
	EN_PUB_ID_UCAAS,
	EN_PUB_ID_PBX,
};

enum class xGateMgAcdEvent
{
	EN_CCAAS_ACD_MG_UNKNOWN = 0,
	EN_CCAAS_ACD_MG_BOT_FAILED,
	EN_CCAAS_ACD_MG_BASE64_FILE_NAME,
	EN_CCAAS_ACD_MG_VMS_TRANSCRIPT,
	EN_CCAAS_ACD_MG_GET_VARIABLE,
	EN_CCAAS_ACD_MG_BOT_INDENT,
	EN_CCAAS_ACD_MG_BOT_SHUTDOWN,
};

enum class EN_BOT_TRANSFER_EVENT
{
	EN_BOT_TRANSFER_EVENT_VOICE_BOT = 0,
	EN_BOT_TRANSFER_EVENT_CCAAS_LIVE_AGENT,
	EN_BOT_TRANSFER_EVENT_SMART_ASSIST,
};

enum class EN_BOT_SMART_ASSIST_EVENT
{
	EN_BOT_SMART_ASSIST_EVENT_NULL = 0,
	EN_BOT_SMART_ASSIST_EVENT_DIAL_USER,
};

enum class EN_BOT_TYPE
{
  EN_BOT_TYPE_VOICE = 0,
  EN_BOT_TYPE_CCAAS_AUTO_ATTENDANT,
  EN_BOT_TYPE_SMART_ASSIST,
};

class bot_reply_msg
{
	public:
	bot_reply_msg()
	{
		m_strMessage = "";
		m_bOrignal = false;
	}
	bot_reply_msg(STR msg, bool original):m_strMessage(msg),m_bOrignal(original)
	{	
	}
	~bot_reply_msg()
	{
		m_strMessage = "";
		m_bOrignal = false;
	}
	bot_reply_msg(const bot_reply_msg &tIn)
	{
		m_strMessage = tIn.m_strMessage.c_str();
		m_bOrignal = tIn.m_bOrignal;
	}
	bot_reply_msg operator = (const bot_reply_msg &tIn)
	{
		m_strMessage = tIn.m_strMessage.c_str();
		m_bOrignal = tIn.m_bOrignal;
	}
	//private:
	STR m_strMessage;
	bool m_bOrignal;
};

class UrMrcpSessionDetails
{
	public:

		UrMrcpSessionDetails()
		{
			m_mgResourceId = "";
			m_callId = "";
			m_codec = 0;
			m_codecName = "";
			m_ptime = 0;
			m_mrcpCodec = 0;
			m_mrcpCodecName = "";
			m_mrcpPtime = "";
			m_mrcpMgIp = "";
			m_mrcpMgPort = "";
			m_mrcpMgTtsPort = "";
			m_mrcpServerIp = "";
			m_mrcpServerPort = "";
			m_mrcpDomainId = "";
			m_mrcpDeviceType = "";
			m_mrcpExtNum = "";
			m_mrcpSttEnabled = 0;
			m_displayText = "";
			m_sessionRecogState = EN_SESSION_RECOG_UNKNOWN;
			m_sessionSynthState = EN_SESSION_SYNTH_UNKNOWN;
			m_isTransferReqested = false;
			m_transferTo = "";
			m_botName = "";
			m_dtmf_digits = "";
			m_seqNo_playing = 0;
			m_totalMsg = 0;
			m_avaamoEmptyResp = 0;
			m_isCcaas = false;
			m_asrType = 0;
			m_ttsType = 0;
			m_memberType = EN_MEMBER_TYPE::EN_MEMBER_TYPE_UNKNOWN,
			m_tts_text_type = 0;
			m_ccaasPorttype = 0;
			m_ccaasUsertype = 0;
			m_isCcaasAgentConnected = false;
			m_remoteExtn = "";
			m_reqId = 0;
			m_resId = 0;
			m_readFd = 0;
			m_isConnClosed = false;
			m_ccaasBlockId = "";
			m_ccaasBlockType = "";
			m_ccaasChannelId = "";
			m_ttsWriteFd = 0;
			m_ccaasBotErrmsg = "";
			m_pubChannelId = xGatePubChannelId::EN_PUB_ID_UNKNOWN;
			m_bisccaasChatBotInit = false;
			m_ccaasAgentId = "";
			m_eventId = xGateMgAcdEvent::EN_CCAAS_ACD_MG_UNKNOWN;
			m_base64Path = "";
			m_ccaasBotUrl = "";
			m_bisUserInCall = false;
			m_isBotWelcomePromptSent = 0;
			m_bisBotFirst = false;
			m_iCcaasBotId = 0;
			m_strCcaasAgentName = "";
			m_strCcaasAgentEmail = "";
			botNumber = 0;
			isRecogStarted = false;
			m_playing = false;
			m_bisReconnect = false;
			m_strCcaasAgentUserId = "";
			m_strCcaasCustomerFirstName = "";
			m_strCcaasCustomerLastName = "";
			m_strDocSocketMessage = "";
			m_strBotIndent = "";
			m_bIsVoiceBotDeInit = false;
			m_iIsCcaasAutoAttendant  = 0;
			m_bIsCcaasVirtualAgent = false;
			m_bIsTransferLiveAgent = false;
			m_strCcaasCli = "";
			m_strCcaasDdi = "";
			m_strQid = "";
			m_bIsSmartAssist = false;
			m_enSmartAssistEvent = EN_BOT_SMART_ASSIST_EVENT::EN_BOT_SMART_ASSIST_EVENT_NULL;
			m_enBotType = EN_BOT_TYPE::EN_BOT_TYPE_VOICE;
			m_bIsPlayTextSent = false;
			m_bIsReadSpeakerConnected = false;
		}

		~UrMrcpSessionDetails()
		{
			m_mgResourceId = "";
			m_callId = "";
			m_codec = 0;
			m_codecName = "";
			m_ptime = 0;
			m_mrcpCodec = 0;
			m_mrcpCodecName = "";
			m_mrcpPtime = "";
			m_mrcpMgIp = "";
			m_mrcpMgPort = "";
			m_mrcpMgTtsPort = "";
			m_mrcpServerIp = "";
			m_mrcpServerPort = "";
			m_mrcpDomainId = "";
			m_mrcpDeviceType = "";
			m_mrcpExtNum = "";
			m_mrcpSttEnabled = 0;
			m_displayText = "";
			m_sessionRecogState = EN_SESSION_RECOG_UNKNOWN;
			m_sessionSynthState = EN_SESSION_SYNTH_UNKNOWN;
			m_isTransferReqested = false;
			m_transferTo = "";
			m_botName = "";
			m_dtmf_digits = "";
			m_seqNo_playing = 0;
			m_totalMsg = 0;
			m_avaamoEmptyResp = 0;
			m_isCcaas = false;
			m_asrType = 0;
			m_ttsType = 0;
			m_memberType = EN_MEMBER_TYPE::EN_MEMBER_TYPE_UNKNOWN,
			m_tts_text_type = 0;
			m_ccaasPorttype = 0;
			m_ccaasUsertype = 0;
			m_isCcaasAgentConnected = false;
			m_remoteExtn = "";
			m_reqId = 0;
			m_resId = 0;
			m_readFd = 0;
			m_isConnClosed = false;
			m_ccaasBlockId = "";
			m_ccaasBlockType = "";
			m_ccaasChannelId = "";
			m_ttsWriteFd = 0;
			m_ccaasBotErrmsg = "";
			m_pubChannelId = xGatePubChannelId::EN_PUB_ID_UNKNOWN;
			m_bisccaasChatBotInit = false;
			m_ccaasAgentId = "";
			m_eventId = xGateMgAcdEvent::EN_CCAAS_ACD_MG_UNKNOWN;
			m_base64Path = "";
			m_ccaasBotUrl = "";
			m_bisUserInCall = false;
			m_isBotWelcomePromptSent = 0;
			m_bisBotFirst = false;
			m_iCcaasBotId = 0;
			m_strCcaasAgentName = "";
			m_strCcaasAgentEmail = "";
			botNumber = 0;
			isRecogStarted = false;
			m_playing = false;
			m_bisReconnect = false;
			m_strCcaasAgentUserId = "";
			m_strCcaasCustomerFirstName = "";
			m_strCcaasCustomerLastName = "";
			m_strDocSocketMessage = "";
			m_strBotIndent = "";
			m_bIsVoiceBotDeInit = false;
			m_iIsCcaasAutoAttendant  = 0;
			m_bIsCcaasVirtualAgent = false;
			m_bIsTransferLiveAgent = false;
			m_strCcaasCli = "";
			m_strCcaasDdi = "";
			m_strQid = "";
			m_bIsSmartAssist = false;
			m_enSmartAssistEvent = EN_BOT_SMART_ASSIST_EVENT::EN_BOT_SMART_ASSIST_EVENT_NULL;
			m_enBotType = EN_BOT_TYPE::EN_BOT_TYPE_VOICE;
			m_bIsPlayTextSent = false;
			m_bIsReadSpeakerConnected = false;
		}

		UrMrcpSessionDetails(const UrMrcpSessionDetails &tIn)
		{
			m_mgResourceId = tIn.m_mgResourceId.c_str();
			m_callId = tIn.m_callId.c_str();
			m_codec = tIn.m_codec;
			m_codecName = tIn.m_codecName.c_str();
			m_ptime = tIn.m_ptime;
			m_mrcpCodec = tIn.m_mrcpCodec;
			m_mrcpCodecName = tIn.m_mrcpCodecName.c_str();
			m_mrcpPtime = tIn.m_mrcpPtime;
			m_mrcpMgIp = tIn.m_mrcpMgIp.c_str();
			m_mrcpMgPort = tIn.m_mrcpMgPort.c_str();
			m_mrcpMgTtsPort = tIn.m_mrcpMgTtsPort.c_str();
			m_mrcpServerIp = tIn.m_mrcpServerIp.c_str();
			m_mrcpServerPort = tIn.m_mrcpServerPort.c_str();
			m_mrcpDomainId = tIn.m_mrcpDomainId.c_str();
			m_mrcpDeviceType = tIn.m_mrcpDeviceType.c_str();
			m_mrcpExtNum = tIn.m_mrcpExtNum.c_str();
			m_mrcpSttEnabled = tIn.m_mrcpSttEnabled;
			m_displayText = tIn.m_displayText.c_str();
			m_sessionRecogState = tIn.m_sessionRecogState;
			m_sessionSynthState = tIn.m_sessionSynthState;
			m_isTransferReqested = tIn.m_isTransferReqested;
			m_transferTo = tIn.m_transferTo.c_str();
			m_botName = tIn.m_botName.c_str();
			m_dtmf_digits = tIn.m_dtmf_digits.c_str();
			m_seqNo_playing = tIn.m_seqNo_playing;
			m_totalMsg = tIn.m_totalMsg;
			m_avaamoEmptyResp = tIn.m_avaamoEmptyResp;
			m_isCcaas = tIn.m_isCcaas;
			m_asrType = tIn.m_asrType;
			m_ttsType = tIn.m_ttsType;
			m_memberType = tIn.m_memberType;
			m_tts_text_type = tIn.m_tts_text_type;
			m_ccaasPorttype = tIn.m_ccaasPorttype;
			m_ccaasUsertype = tIn.m_ccaasUsertype;
			m_isCcaasAgentConnected = tIn.m_isCcaasAgentConnected;
			m_remoteExtn = tIn.m_remoteExtn;
			m_reqId = tIn.m_reqId;
			m_resId = tIn.m_resId;
			m_readFd = tIn.m_readFd;
			m_isConnClosed = tIn.m_isConnClosed;
			m_ccaasBlockId = tIn.m_ccaasBlockId;
			m_ccaasBlockType = tIn.m_ccaasBlockType;
			m_ccaasChannelId = tIn.m_ccaasChannelId;
			m_ttsWriteFd = tIn.m_ttsWriteFd;
			m_ccaasBotErrmsg = tIn.m_ccaasBotErrmsg;
			m_pubChannelId = tIn.m_pubChannelId;
			m_bisccaasChatBotInit = tIn.m_bisccaasChatBotInit;
			m_ccaasAgentId = tIn.m_ccaasAgentId;
			m_eventId = tIn.m_eventId;
			m_base64Path = tIn.m_base64Path;
			m_ccaasBotUrl = tIn.m_ccaasBotUrl;
			m_bisUserInCall = tIn.m_bisUserInCall;
			m_isBotWelcomePromptSent = tIn.m_isBotWelcomePromptSent;
			m_bisBotFirst = tIn.m_bisBotFirst;
			m_iCcaasBotId = tIn.m_iCcaasBotId;
			m_strCcaasAgentName = tIn.m_strCcaasAgentName;
			m_strCcaasAgentEmail = tIn.m_strCcaasAgentEmail;
			botNumber = tIn.botNumber;
			isRecogStarted = tIn.isRecogStarted;
			m_playing = tIn.m_playing;
			m_bisReconnect = tIn.m_bisReconnect;
			m_strCcaasAgentUserId = tIn.m_strCcaasAgentUserId;
			m_strCcaasCustomerFirstName = tIn.m_strCcaasCustomerFirstName;
			m_strCcaasCustomerLastName = tIn.m_strCcaasCustomerLastName;
			m_strDocSocketMessage = tIn.m_strDocSocketMessage;
			m_strBotIndent = tIn.m_strBotIndent;
			m_bIsVoiceBotDeInit = tIn.m_bIsVoiceBotDeInit;
			m_iIsCcaasAutoAttendant  = tIn.m_iIsCcaasAutoAttendant ;
			m_bIsCcaasVirtualAgent = tIn.m_bIsCcaasVirtualAgent;
			m_bIsTransferLiveAgent = tIn.m_bIsTransferLiveAgent;
			m_strCcaasCli = tIn.m_strCcaasCli;
			m_strCcaasDdi = tIn.m_strCcaasDdi;
			m_strQid = tIn.m_strQid;
			m_bIsSmartAssist = tIn.m_bIsSmartAssist;
			m_enSmartAssistEvent = tIn.m_enSmartAssistEvent;
			m_enBotType = tIn.m_enBotType;
			m_bIsPlayTextSent = tIn.m_bIsPlayTextSent;
			m_bIsReadSpeakerConnected = tIn.m_bIsReadSpeakerConnected;
		}

		UrMrcpSessionDetails operator = (const UrMrcpSessionDetails &tIn)
		{
			m_mgResourceId = tIn.m_mgResourceId.c_str();
			m_callId = tIn.m_callId.c_str();
			m_codec = tIn.m_codec;
			m_codecName = tIn.m_codecName.c_str();
			m_ptime = tIn.m_ptime;
			m_mrcpCodec = tIn.m_mrcpCodec;
			m_mrcpCodecName = tIn.m_mrcpCodecName.c_str();
			m_mrcpPtime = tIn.m_mrcpPtime;
			m_mrcpMgIp = tIn.m_mrcpMgIp.c_str();
			m_mrcpMgPort = tIn.m_mrcpMgPort.c_str();
			m_mrcpMgTtsPort = tIn.m_mrcpMgTtsPort.c_str();
			m_mrcpServerIp = tIn.m_mrcpServerIp.c_str();
			m_mrcpServerPort = tIn.m_mrcpServerPort.c_str();
			m_mrcpDomainId = tIn.m_mrcpDomainId.c_str();
			m_mrcpDeviceType = tIn.m_mrcpDeviceType.c_str();
			m_mrcpExtNum = tIn.m_mrcpExtNum.c_str();
			m_mrcpSttEnabled = tIn.m_mrcpSttEnabled;
			m_displayText = tIn.m_displayText.c_str();
			m_sessionRecogState = tIn.m_sessionRecogState;
			m_isTransferReqested = tIn.m_isTransferReqested;
			m_transferTo = tIn.m_transferTo.c_str();
			m_botName = tIn.m_botName.c_str();
			m_dtmf_digits = tIn.m_dtmf_digits.c_str();
			m_seqNo_playing = tIn.m_seqNo_playing;
			m_totalMsg = tIn.m_totalMsg;
			m_avaamoEmptyResp = tIn.m_avaamoEmptyResp;
			m_isCcaas = tIn.m_isCcaas;
			m_asrType = tIn.m_asrType;
			m_ttsType = tIn.m_ttsType;
			m_memberType = tIn.m_memberType;
			m_tts_text_type = tIn.m_tts_text_type;
			m_ccaasPorttype = tIn.m_ccaasPorttype;
			m_ccaasUsertype = tIn.m_ccaasUsertype;
			m_isCcaasAgentConnected = tIn.m_isCcaasAgentConnected;
			m_remoteExtn = tIn.m_remoteExtn;
			m_reqId = tIn.m_reqId;
			m_resId = tIn.m_resId;
			m_readFd = tIn.m_readFd;
			m_isConnClosed = tIn.m_isConnClosed;
			m_ccaasBlockId = tIn.m_ccaasBlockId;
			m_ccaasBlockType = tIn.m_ccaasBlockType;
			m_ccaasChannelId = tIn.m_ccaasChannelId;
			m_ttsWriteFd = tIn.m_ttsWriteFd;
			m_ccaasBotErrmsg = tIn.m_ccaasBotErrmsg;
			m_pubChannelId = tIn.m_pubChannelId;
			m_bisccaasChatBotInit = tIn.m_bisccaasChatBotInit;
			m_ccaasAgentId = tIn.m_ccaasAgentId;
			m_eventId = tIn.m_eventId;
			m_base64Path = tIn.m_base64Path;
			m_ccaasBotUrl = tIn.m_ccaasBotUrl;
			m_bisUserInCall = tIn.m_bisUserInCall;
			m_isBotWelcomePromptSent = tIn.m_isBotWelcomePromptSent;
			m_bisBotFirst = tIn.m_bisBotFirst;
			m_iCcaasBotId = tIn.m_iCcaasBotId;
			m_sessionSynthState = tIn.m_sessionSynthState;
			m_strCcaasAgentName = tIn.m_strCcaasAgentName;
			m_strCcaasAgentEmail = tIn.m_strCcaasAgentEmail;
			botNumber = tIn.botNumber;
			isRecogStarted = tIn.isRecogStarted;
			m_playing = tIn.m_playing;
			m_bisReconnect = tIn.m_bisReconnect;
			m_strCcaasAgentUserId = tIn.m_strCcaasAgentUserId;
			m_strCcaasCustomerFirstName = tIn.m_strCcaasCustomerFirstName;
			m_strCcaasCustomerLastName = tIn.m_strCcaasCustomerLastName;
			m_strDocSocketMessage = tIn.m_strDocSocketMessage;
			m_strBotIndent = tIn.m_strBotIndent;
			m_bIsVoiceBotDeInit = tIn.m_bIsVoiceBotDeInit;
			m_iIsCcaasAutoAttendant  = tIn.m_iIsCcaasAutoAttendant ;
			m_bIsCcaasVirtualAgent = tIn.m_bIsCcaasVirtualAgent;
			m_bIsTransferLiveAgent = tIn.m_bIsTransferLiveAgent;
			m_strCcaasCli = tIn.m_strCcaasCli;
			m_strCcaasDdi = tIn.m_strCcaasDdi;
			m_strQid = tIn.m_strQid;
			m_bIsSmartAssist = tIn.m_bIsSmartAssist;
			m_enSmartAssistEvent = tIn.m_enSmartAssistEvent;
			m_enBotType = tIn.m_enBotType;
			m_bIsPlayTextSent = tIn.m_bIsPlayTextSent;
			m_bIsReadSpeakerConnected = tIn.m_bIsReadSpeakerConnected;
		}

		void set_avaamo_bot_name()
		{
			if(botNumber == AVAAMO_LAWFIRM_BOT)
			{
				m_botName = "lawfirm";
			}
			else if(botNumber == AVAAMO_RESTRARUNT_BOT)
			{
				m_botName = "restrarunt";
			}
			else if(botNumber == AVAAMO_SUBSCRIBER_BOT)
			{
				m_botName = "subscriber";
			}
            else if(botNumber == AVAAMO_MEETING_ASSISTANT_BOT)
            {
                m_botName = "Meeting Assistant";
            }
		}

		SET(STR,str,Qid);
		GETSTR(str,Qid);
		SET(int,i,IsCcaasAutoAttendant);
		GET(i,IsCcaasAutoAttendant);
		SET(bool,b,IsSmartAssist);
		GET(b,IsSmartAssist);
		SET(EN_BOT_SMART_ASSIST_EVENT,en,SmartAssistEvent);
		GET(en,SmartAssistEvent);
		SET(EN_BOT_TYPE,en,BotType);
		GET(en,BotType);
		SET(bool,b,IsPlayTextSent);
		GET(b,IsPlayTextSent);
		SET(bool,b,IsReadSpeakerConnected);
		GET(b,IsReadSpeakerConnected);

		STR m_mgResourceId;
		STR m_callId;
		unsigned int m_codec;
		STR m_codecName;
		unsigned int m_ptime;
		unsigned int m_mrcpCodec;
		STR m_mrcpCodecName;
		unsigned int m_mrcpPtime;
		STR m_mrcpMgIp;
		STR m_mrcpMgPort;	
		STR m_mrcpMgTtsPort;	
		STR m_mrcpServerIp;
		STR m_mrcpServerPort;
		STR m_mrcpDomainId;
		STR m_mrcpDeviceType;
		STR m_mrcpExtNum;
		unsigned int m_mrcpSttEnabled;
		STR m_displayText;
		std::list<STR>m_mrcpUsers;
		session_recognize_state m_sessionRecogState;
		session_synthesize_state m_sessionSynthState;
		std::list<bot_reply_msg> m_avaamo_reply;
		std::map<int,STR> m_avaamo_replys;
		bool m_isTransferReqested;
		STR m_transferTo;
		unsigned int botNumber;
		STR m_botName;
		STR m_dtmf_digits;
		int m_seqNo_playing;
		int m_totalMsg;
		int m_avaamoEmptyResp;
		bool m_isCcaas;
		int m_asrType;
		int m_ttsType;
		EN_MEMBER_TYPE m_memberType;
		int m_tts_text_type;
		int m_ccaasPorttype;
		int m_ccaasUsertype;
		bool m_isCcaasAgentConnected;
		STR m_remoteExtn;
		bool isRecogStarted;
		ACE_thread_t m_reqId;
		ACE_thread_t m_resId;
		int m_readFd;
		int m_ttsWriteFd;
		bool m_isConnClosed;
		STR m_ccaasBlockId;
		STR m_ccaasBlockType;
		STR m_ccaasChannelId;
		STR m_ccaasBotErrmsg;
		xGatePubChannelId m_pubChannelId;
		bool m_playing;
		bool m_bisccaasChatBotInit;
		STR m_ccaasAgentId;
		STR m_ccaasBotUrl;
		xGateMgAcdEvent m_eventId;
		STR m_base64Path;
		bool m_bisUserInCall;
		bool m_isBotWelcomePromptSent;
		bool m_bisBotFirst;
		int m_iCcaasBotId;
		STR m_strCcaasAgentName;
		STR m_strCcaasAgentEmail;
		bool m_bisReconnect;
		STR m_strCcaasAgentUserId;
		STR m_strCcaasCustomerFirstName;
		STR m_strCcaasCustomerLastName;
		STR m_strDocSocketMessage;
		STR m_strBotIndent;
		bool m_bIsVoiceBotDeInit;
		int m_iIsCcaasAutoAttendant ;
		bool m_bIsCcaasVirtualAgent;
		bool m_bIsTransferLiveAgent;
		STR m_strCcaasCli;
		STR m_strCcaasDdi;
		STR m_strQid;
		bool m_bIsSmartAssist;
		EN_BOT_SMART_ASSIST_EVENT m_enSmartAssistEvent;
		EN_BOT_TYPE m_enBotType;
		bool m_bIsPlayTextSent;
		bool m_bIsReadSpeakerConnected;
};

#endif
