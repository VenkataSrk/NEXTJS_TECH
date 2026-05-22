#ifndef UR_MRCP_REDIS_DB_MSG
#define UR_MRCP_REDIS_DB_MSG
#include "UrMrcpDefines.h"
#include "xGateUtil.h"
#include "URMRCPBaseMsg.h"

class Display_Info
{
	public:

		Display_Info()
		{
		    ext = "";
		    domain_id = "";
		    device = "";
			remoteExtn = "";
		}
		~Display_Info()
		{
		    ext = "";
		    domain_id = "";
		    device = "";
			remoteExtn = "";
		}
		Display_Info(const Display_Info &src)=default;
		Display_Info& operator=(const Display_Info &src)=default;

		list<STR> users;
		STR domain_id;
		STR device;
		STR ext;
		STR remoteExtn;
};

typedef enum
{
	AVAAMO_RESPONSE_TYPE_NULL = -1,
	AVAAMO_RESPONSE_TYPE_MSG,
	AVAAMO_RESPONSE_TYPE_TRANS,
}AvaamoResponseType;

enum class EN_CCAAS_VMS_PUB_TYPE
{
	EN_CCAAS_VMS_PUB_TYPE_UNKNOWN = 0,
	EN_CCAAS_VMS_PUB_TYPE_TRANSCRIPT_REQ,
	EN_CCAAS_VMS_PUB_TYPE_VMS_NODE,
};

class ccaasVmsInfo
{
  public:

	ccaasVmsInfo()
	{
		m_strSessionId = "";
		m_strFileName = "";
		m_iAsrType = 0;
		m_strData = "";
		m_enPubType = EN_CCAAS_VMS_PUB_TYPE::EN_CCAAS_VMS_PUB_TYPE_UNKNOWN;
	}
	~ccaasVmsInfo()
	{
		m_strSessionId = "";
		m_strFileName = "";
		m_iAsrType = 0;
		m_strData = "";
		m_enPubType = EN_CCAAS_VMS_PUB_TYPE::EN_CCAAS_VMS_PUB_TYPE_UNKNOWN;
	}
	ccaasVmsInfo(const ccaasVmsInfo& rhs)=default;
	ccaasVmsInfo& operator=(const ccaasVmsInfo& rhs)=default;

    void set_session_id(STR sessionId)
    {
      m_strSessionId = sessionId.c_str();
    }
    STR get_session_id()
    {
      return m_strSessionId.c_str();
    }

    void set_file_name(STR fileName)
    {
      m_strFileName = fileName.c_str();
    }
    STR get_file_name()
    {
      return m_strFileName.c_str();
    }

    void set_asr_type(unsigned int asrType)
    {
      m_iAsrType = asrType;
    }
    unsigned int get_asr_type()
    {
      return m_iAsrType;
    }
    void set_data(STR data)
    {
      m_strData = data.c_str();
    }
    STR get_data()
    {
      return m_strData.c_str();
    }

	void set_vms_pub_type(EN_CCAAS_VMS_PUB_TYPE type)
    {
      m_enPubType = type;
    }
    EN_CCAAS_VMS_PUB_TYPE get_vms_pub_type()
    {
      return m_enPubType;
    }

  private:
    STR m_strSessionId;
    STR m_strFileName;
    unsigned int m_iAsrType;
    STR m_strData;
	EN_CCAAS_VMS_PUB_TYPE m_enPubType;
};

enum class EN_MESSAGE_TYPE
{
   EN_MESSAGE_TYPE_UNKNOWN = 0,
   EN_MESSAGE_TYPE_VMS_STT_REQ,
   EN_MESSAGE_TYPE_VMS_STT_RESP,
};

class  UrMrcpDbServiceMsg:public URMRCPBaseMsg
{
	public:

		UrMrcpDbServiceMsg(void) : m_ccaasVmsInfo(NULL),m_enMessageType(EN_MESSAGE_TYPE::EN_MESSAGE_TYPE_UNKNOWN),
		URMRCPBaseMsg(IURDefines::UR_MODULE_MRCP_REDIS_DB_SERVICE) 
		{
			m_speaker = "";
			m_uid = "";
			m_avaamoRequestType = "";
			m_avaamoResponseType = AVAAMO_RESPONSE_TYPE_NULL;
			m_avaamoBotName = "";
			m_respMsgSeq = 0;
			m_respTotalMsg = 0;
			m_avaamoTransferNo = "";
			m_displayText = "";
			m_sessionDetail = NULL;
			m_isCcaas = false;
			m_isCcaasAgentConnected = false;
			m_ccaasUsertype = 0;
			m_ccaasBlockId = "";
			m_ccaasBlockType = "";
			m_ccaasChannelId = "";
			m_ccaasCallStatus = "";
			m_ccaasBotErrmsg = "";
			m_pubChannelId = xGatePubChannelId::EN_PUB_ID_UNKNOWN;
			m_ccaasAgentId = "";
			m_ccaasBotUrl = "";
			m_isOrignal = false;
			m_base64Path = "";
			m_eventId = xGateMgAcdEvent::EN_CCAAS_ACD_MG_UNKNOWN;
			m_enMessageType = EN_MESSAGE_TYPE::EN_MESSAGE_TYPE_UNKNOWN;
			m_iCcaasBotId = 0;
			m_strCcaasAgentName = "";
			m_strCcaasAgentEmail = "";
			m_strCcaasAgentUserId = "";
			m_strCcaasCustomerFirstName = "";
			m_strCcaasCustomerLastName = "";
			m_strDocMessage = "";
			m_strBotIndent = "";
			m_iIsCcaasAutoAttendant  = 0;
			m_bIsCcaasVirtualAgent = false;
			m_strCcaasCli = "";
			m_strCcaasDdi = "";
			m_strQid = "";
			m_enBotEvent = EN_BOT_TRANSFER_EVENT::EN_BOT_TRANSFER_EVENT_VOICE_BOT;
			m_strUserExt = "";
			m_enSmartAssistEvent = EN_BOT_SMART_ASSIST_EVENT::EN_BOT_SMART_ASSIST_EVENT_NULL;
			m_bIsSmartAssist = false;
			m_enBotType = EN_BOT_TYPE::EN_BOT_TYPE_VOICE;
		};
		virtual ~UrMrcpDbServiceMsg(){};

		UrMrcpDbServiceMsg(const UrMrcpDbServiceMsg& rhs)=default;
		UrMrcpDbServiceMsg& operator=(const UrMrcpDbServiceMsg& rhs)=default;

		void set_session_uid(STR uid)
		{
			m_uid = uid.c_str();
		}
		STR get_session_uid()
		{
			return m_uid;
		}

		void set_avaamo_request_type(STR type)
		{
			m_avaamoRequestType = type.c_str();
		}
		STR get_avaamo_request_type()
		{
			return m_avaamoRequestType.c_str();
		}
		
		void set_session_speaker(STR speaker)
		{
			m_speaker = speaker.c_str();
		}
		STR get_session_speaker()
		{
			return m_speaker;
		}

		void set_display_text(STR text)
		{
		    m_displayText = text.c_str();
		}
		STR get_display_text()
		{
		    return m_displayText;
		}

		void set_avaamo_transfer_no(STR transferNo)
		{
		    m_avaamoTransferNo = transferNo.c_str();
		}
		STR get_avaamo_transfer_no()
		{
		    return m_avaamoTransferNo;
		}

		void set_display_users(std::list<Display_Info> displayUsers)
		{
			m_ldisplayUsers = displayUsers;	
		}
		std::list<Display_Info> get_display_users()
		{
			return m_ldisplayUsers;
		}

		void set_avaamo_response_type(AvaamoResponseType avaamoResponseType)
		{
			m_avaamoResponseType = avaamoResponseType;	
		}
		AvaamoResponseType get_avaamo_response_type()
		{
			return m_avaamoResponseType;
		}
		void set_response_msg_seqence(int number)
		{
			m_respMsgSeq = number;
		}
		int get_response_msg_seqence()
		{
			return m_respMsgSeq;
		}
		void set_response_total_msg(int totalMsg)
		{
			m_respTotalMsg = totalMsg;
		}
		int get_response_total_msg()
		{
			return m_respTotalMsg;
		}

		void set_avaamo_bot_name(STR botName)
		{
			m_avaamoBotName = botName.c_str();	
		}
		STR get_avaamo_bot_name()
		{
			return m_avaamoBotName;
		}

		void set_isCcaasAgentConnected(bool isCcaasAgentConnected)
		{
			m_isCcaasAgentConnected = isCcaasAgentConnected;	
		}

		bool get_isCcaasAgentConnected()
		{
			return m_isCcaasAgentConnected;
		}

		void set_ccaasUsertype(bool ccaasUsertype)
        {
            m_ccaasUsertype = ccaasUsertype;
        }

        bool get_ccaasUsertype()
        {
            return m_ccaasUsertype;
        }

		void set_ccaasBlockId(STR blockId)
		{
			m_ccaasBlockId = blockId.c_str();
		}

		STR get_ccaasBlockId()
		{
			return m_ccaasBlockId.c_str();
		}

		void set_ccaasBlockType(STR blockType)
		{
			m_ccaasBlockType = blockType.c_str();
		}

		STR get_ccaasBlockType()
		{
			return m_ccaasBlockType.c_str();
		}

		void set_ccaasChannelId(STR channelId)
		{
			m_ccaasChannelId = channelId.c_str();
		}

		STR get_ccaasChannelId()
		{
			return m_ccaasChannelId.c_str();
		}

		void set_ccaasCallStatus(STR callStatus)
		{
			m_ccaasCallStatus = callStatus.c_str();
		}

		STR get_ccaasCallStatus()
		{
			return m_ccaasCallStatus.c_str();
		}

		void set_ccaasBotErrmsg(STR disconReason)
		{
			m_ccaasBotErrmsg = disconReason.c_str();
		}

		void set_orignal(bool isOriginal)
		{
			m_isOrignal = isOriginal;
		}

		bool get_orignal()
		{
			return m_isOrignal;
		}

		void set_ccaasAgentId(STR agentId)
		{
			m_ccaasAgentId = agentId.c_str();
		}

		STR get_ccaasAgentId()
		{
			return m_ccaasAgentId.c_str();
		}

		void set_base64Path(STR path)
		{
			m_base64Path = path.c_str();
		}

		STR get_base64Path()
		{
			return m_base64Path;
		}

		void set_eventId(xGateMgAcdEvent Id)
		{
			m_eventId = Id;
		}

		xGateMgAcdEvent get_eventId()
		{
			return m_eventId;
		}

		void set_ccaas_vms_info(ccaasVmsInfo *vmsInfo)
		{
		  m_ccaasVmsInfo = vmsInfo;
		}
		ccaasVmsInfo* get_ccaas_vms_info()
		{
		  return m_ccaasVmsInfo;
		}
		void set_ccaasBotUrl(STR url)
		{
			m_ccaasBotUrl = url.c_str();
		}

		void set_message_type(EN_MESSAGE_TYPE msgType)
		{
			m_enMessageType = msgType;
		}

		EN_MESSAGE_TYPE get_message_type()
		{
			return m_enMessageType;
		}

		void set_bot_id(int botId)
		{
			m_iCcaasBotId = botId;
		}

		SET(STR,str,Qid);
		GETSTR(str,Qid);
		SET(EN_BOT_TRANSFER_EVENT,en,BotEvent);
		GET(en,BotEvent);
		SET(STR,str,UserExt);
		GETSTR(str,UserExt);
		SET(EN_BOT_SMART_ASSIST_EVENT,en,SmartAssistEvent);
		GET(en,SmartAssistEvent);
		SET(bool,b,IsSmartAssist);
		GET(b,IsSmartAssist);
		SET(EN_BOT_TYPE,en,BotType);
		GET(en,BotType);

	//private:
		STR m_speaker;
		STR m_uid;
		STR m_avaamoRequestType;
		AvaamoResponseType m_avaamoResponseType;
		STR m_avaamoBotName;
		int m_respMsgSeq;
		int m_respTotalMsg;
		STR m_avaamoTransferNo;
		STR m_displayText;
		list<Display_Info> m_ldisplayUsers;
		Display_Info display;
		UrMrcpSessionDetails *m_sessionDetail;
		bool m_isCcaas;
		bool m_isCcaasAgentConnected;
		unsigned int m_ccaasUsertype;
		STR m_ccaasBlockId;
		STR m_ccaasBlockType;
		STR m_ccaasChannelId;
		STR m_ccaasCallStatus;
		STR m_ccaasBotErrmsg;
		xGatePubChannelId m_pubChannelId;
		STR m_ccaasAgentId;
		STR m_ccaasBotUrl;
		bool m_isOrignal;
		STR m_base64Path;
		xGateMgAcdEvent m_eventId;
		EN_MESSAGE_TYPE m_enMessageType;
    	ccaasVmsInfo *m_ccaasVmsInfo;
		int m_iCcaasBotId;
		STR m_strCcaasAgentName;
		STR m_strCcaasAgentEmail;
		STR m_strCcaasAgentUserId;
		STR m_strCcaasCustomerFirstName;
		STR m_strCcaasCustomerLastName;
		STR m_strDocMessage;
		STR m_strBotIndent;
		int m_iIsCcaasAutoAttendant;
		bool m_bIsCcaasVirtualAgent;
		STR m_strCcaasCli;
		STR m_strCcaasDdi;
		STR m_strQid;
		EN_BOT_TRANSFER_EVENT m_enBotEvent;
		STR m_strUserExt;
		EN_BOT_SMART_ASSIST_EVENT m_enSmartAssistEvent;
		bool m_bIsSmartAssist;
		EN_BOT_TYPE m_enBotType;
};

#endif
