/*
    Author: @PraveenMS
    
    Speech synthesis session managment functions, they invoker wrapper functions. 

    Layers of function

    ------------------------------------
    | ur_mrcp_controller               | - interacts with business layer
    ------------------------------------
    | xGateMrcpCliAppHandler           | - interacts with controller later
    ------------------------------------
    | UniMrcpCliAppFw                  | - Framework of speech recognition
    ------------------------------------
    | MrcpCliAppSynthScenario          | - Store necessary infomation for the current session
    ------------------------------------
    | MrcpCliAppSynthSession           | - Responsible for session creation, modification and update
    ------------------------------------
    | WrapMrcpSession                  | - Wrapper functions for UniMrcp lib apis
    ------------------------------------
    | UniMRCP 1.7.0                    | - 3rd party library 
    ------------------------------------
    | SIP          | MRCPv2            |
    ------------------------------------

    Note: RTP is responsibility of MG. This client has RTP capability for testing purpose only. 
    To integrate TTS server with MG,  configure MG and RTP port range under profiles "RTP-Settings-1"
    and "RTP-Factory-1" in configuration file "unimrcpclient.xml"
    MRCP client will share this information to TTS server during sdp and mrcp negotition process. 
    
*/


#include "MrcpCliAppSynthSession.h"
#include "MrcpCliAppSynthScenario.h"
#include "mrcp_message.h"
#include "mrcp_generic_header.h"
#include "mrcp_synth_header.h"
#include "mrcp_synth_resource.h"
#include "apt_log.h"
#include "xglog.h"
#include "URMRCPModuleBridge.h"
#include "mrcp_client_session.h"

struct SynthChannel
{
	/** MRCP channel */
	mrcp_channel_t* m_pMrcpChannel;
	/** IN-PROGRESS SPEAK request */
	mrcp_message_t* m_pSpeakRequest;
	/** File to write audio stream to */
	FILE*           m_pAudioOut;

	SynthChannel() : m_pMrcpChannel(NULL), m_pSpeakRequest(NULL), m_pAudioOut(NULL) {}
};

MrcpCliAppSynthSession::MrcpCliAppSynthSession(const MrcpCliAppSynthScenario* pScenario) :
        WrapMrcpSession(pScenario),
        m_pSynthChannel(NULL)
{
        XGLOG_FUNCTION_EN_EX("");
}

MrcpCliAppSynthSession::~MrcpCliAppSynthSession()
{
}

/*
    Load data and start session with server
*/
bool MrcpCliAppSynthSession::Start()
{
	XGLOG_FUNCTION_EN_EX("");
	if(!GetScenario()->IsSpeakEnabled())
		return false;
	
	/* create channel and associate all the required data */
	m_pSynthChannel = CreateSynthChannel();
	if(!m_pSynthChannel) 
		return false;

	/* add channel to session (send asynchronous request) */
	if(!AddMrcpChannel(m_pSynthChannel->m_pMrcpChannel))
	{
		delete m_pSynthChannel;
		m_pSynthChannel = NULL;
		return false;
	}
	return true;
}

/*
    shutdown session gracefully with server
*/

bool MrcpCliAppSynthSession::Stop()
{
	XGLOG_FUNCTION_EN_EX("");
	if(!WrapMrcpSession::Stop())
		return false;

	if(!m_pSynthChannel)
		return false;

	mrcp_message_t* pStopMessage = CreateMrcpMessage(m_pSynthChannel->m_pMrcpChannel,SYNTHESIZER_STOP);
	if(!pStopMessage)
		return false;

	if(m_pSynthChannel->m_pSpeakRequest)
	{
		mrcp_generic_header_t* pGenericHeader;
		/* get/allocate generic header */
		pGenericHeader = (mrcp_generic_header_t*) mrcp_generic_header_prepare(pStopMessage);
		if(pGenericHeader) 
		{
			pGenericHeader->active_request_id_list.count = 1;
			pGenericHeader->active_request_id_list.ids[0] = 
				m_pSynthChannel->m_pSpeakRequest->start_line.request_id;
			mrcp_generic_header_property_add(pStopMessage,GENERIC_HEADER_ACTIVE_REQUEST_ID_LIST);
		}

		m_pSynthChannel->m_pSpeakRequest = NULL;
	}
	
	return SendMrcpRequest(m_pSynthChannel->m_pMrcpChannel,pStopMessage);
}

/*
    use to terminate the processing session
*/
bool MrcpCliAppSynthSession::OnSessionTerminate(mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	if(m_pSynthChannel)
	{
		FILE* pAudioOut = m_pSynthChannel->m_pAudioOut;
		if(pAudioOut) 
		{
			m_pSynthChannel->m_pAudioOut = NULL;
			fclose(pAudioOut);
		}

		delete m_pSynthChannel;
		m_pSynthChannel = NULL;
	}
	return WrapMrcpSession::OnSessionTerminate(status);
}

/*
    WriteStream Used to record the received RTP stream from server into audio file. 
    Used only when RTP channel is supported between MRCP client and TTS server. 
    If MG is supporting RTP channel, then this function is not used. 
*/

static apt_bool_t WriteStream(mpf_audio_stream_t* pStream, const mpf_frame_t* pFrame)
{
//	XGLOG_FUNCTION_EN_EX("");
	SynthChannel* pSynthChannel = (SynthChannel*) pStream->obj;
	if(pSynthChannel && pSynthChannel->m_pAudioOut) 
	{
		fwrite(pFrame->codec_frame.buffer,1,pFrame->codec_frame.size,pSynthChannel->m_pAudioOut);
	}
	return TRUE;
}

/*
    create channel for speech synthesis 
*/

SynthChannel* MrcpCliAppSynthSession::CreateSynthChannel()
{
	XGLOG_FUNCTION_EN_EX("");
	mrcp_channel_t* pChannel;
	mpf_termination_t* pTermination;
	mpf_stream_capabilities_t* pCapabilities;
	apr_pool_t* pool = GetSessionPool();

	/* create channel */
	SynthChannel* pSynthChannel = new SynthChannel;
#if 0
	/* create sink stream capabilities */
	pCapabilities = mpf_sink_stream_capabilities_create(pool);
	GetScenario()->InitCapabilities(pCapabilities);

	static const mpf_audio_stream_vtable_t audio_stream_vtable = 
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		WriteStream,
		NULL
	};

	pTermination = CreateAudioTermination(
			&audio_stream_vtable,      /* virtual methods table of audio stream */
			pCapabilities,             /* capabilities of audio stream */
			pSynthChannel);            /* object to associate */
#endif	
        mpf_rtp_termination_descriptor_t *rtp_descriptor;
        rtp_descriptor = NULL;

        rtp_descriptor = mpf_rtp_termination_descriptor_alloc(pool);
        //string mgip = "10.22.2.152";
        rtp_descriptor->audio.local = mpf_rtp_media_descriptor_alloc(pool);
        rtp_descriptor->audio.local->state = MPF_MEDIA_ENABLED;
        apt_string_assign(&rtp_descriptor->audio.local->ip,m_tSdpDescriptor->m_mrcpMgIp.c_str(),pool);
        //static int port = 5002;
        rtp_descriptor->audio.local->port = atoi(m_tSdpDescriptor->m_mrcpMgTtsPort.c_str());
        rtp_descriptor->audio.local->direction = STREAM_DIRECTION_DUPLEX;
        rtp_descriptor->audio.local->ptime = m_tSdpDescriptor->m_ptime;
        mpf_codec_list_init(&rtp_descriptor->audio.local->codec_list, 1, pool);
        mpf_codec_descriptor_t* codec_discriptor = mpf_codec_list_add(&rtp_descriptor->audio.local->codec_list);
        codec_discriptor->payload_type = 8;
        apt_string_assign(&codec_discriptor->name,"PCMA",pool);
        codec_discriptor->sampling_rate = 8000;
        codec_discriptor->enabled = 1;
		
	pChannel = CreateMrcpChannel(
			MRCP_SYNTHESIZER_RESOURCE, /* MRCP resource identifier */
			pTermination,              /* media termination, used to terminate audio stream */
			rtp_descriptor,                      /* RTP descriptor, used to create RTP termination (NULL by default) */
			pSynthChannel);            /* object to associate */
	if(!pChannel)
	{
		delete pSynthChannel;
		return NULL;
	}

	pSynthChannel->m_pMrcpChannel = pChannel;
	return pSynthChannel;
}

/*
    Add channel for speech synthesis. 
    to synch the media. create speak request and record the audio
*/

bool MrcpCliAppSynthSession::OnChannelAdd(mrcp_channel_t* pMrcpChannel, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!WrapMrcpSession::OnChannelAdd(pMrcpChannel,status))
		return false;
#if 0
	const mpf_codec_descriptor_t* pDescriptor = mrcp_application_sink_descriptor_get(pMrcpChannel);
	if(!pDescriptor) 
	{
		apt_log(APT_LOG_MARK,APT_PRIO_WARNING,"Failed to Get Media Sink Descriptor");
		return Terminate();
	}
#endif
	SynthChannel* pSynthChannel = (SynthChannel*) mrcp_application_channel_object_get(pMrcpChannel);
	if(status != MRCP_SIG_STATUS_CODE_SUCCESS)
	{
		/* error case, just terminate the demo */
		return Terminate();
	}
#if 0 
	/* create MRCP message */
	mrcp_message_t* pMrcpMessage = CreateSpeakRequest(pMrcpChannel);
	if(pMrcpMessage) 
	{
		SendMrcpRequest(pSynthChannel->m_pMrcpChannel,pMrcpMessage);
	}
#endif
    /* you dont have to call this function, when you are usng MG*/
//	pSynthChannel->m_pAudioOut = GetAudioOut(pDescriptor,GetSessionPool());
	return true;
}

/*
    Received MRCP message/response from TTS server for ongoing channel with status. 

    Note: While integrating MRCP client with Business layer like PBX, Here you can  
    add functions to pass the response back to business layer. which can take action or notify MG
    to take appropriate actions on the RTP channel and notify terminals accordingly. 

    SPEAK request is triggered by calling following function 
            UniMrcpCliAppFw::ProcessSynthRequest() function. 
            

    MRCP client will receive following MRCP messages from TTS server. 

       MRCP Client                                   TTS Server 
     (SPEAK flow )      
           |----------------SPEAK------------------------>|
           |<---------------IN-PROGRESS-------------------|
           |<---------------SPEAK-COMPLETE----------------|
    
    MRCP messages received from TTS server are handled in below funtion. 
*/

bool MrcpCliAppSynthSession::SendSynthesisRequest(mrcp_channel_t* pMrcpChannel)
{
        SynthChannel* pSynthChannel = (SynthChannel*) mrcp_application_channel_object_get(pMrcpChannel);
        mrcp_message_t* pMrcpMessage = CreateSpeakRequest(pMrcpChannel);
        if(pMrcpMessage)
        {
                SendMrcpRequest(pSynthChannel->m_pMrcpChannel,pMrcpMessage);
        }
	return true;
}

bool MrcpCliAppSynthSession::OnMessageReceive(mrcp_channel_t* pMrcpChannel, mrcp_message_t* pMrcpMessage)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!WrapMrcpSession::OnMessageReceive(pMrcpChannel,pMrcpMessage))
	{
		XGLOG_INFO("MrcpCliAppSynthSession::OnMessageReceive returning false");
		return false;
	}

	if(pMrcpMessage->start_line.message_type == MRCP_MESSAGE_TYPE_RESPONSE) 
	{
		/* received MRCP response */
		if(pMrcpMessage->start_line.method_id == SYNTHESIZER_SPEAK) 
		{
			/* received the response to SPEAK request */
			if(pMrcpMessage->start_line.request_state == MRCP_REQUEST_STATE_INPROGRESS) 
			{
				SynthChannel* pSynthChannel = (SynthChannel*) mrcp_application_channel_object_get(pMrcpChannel);
				if(pSynthChannel)
					pSynthChannel->m_pSpeakRequest = GetMrcpMessage();
				
				/* waiting for SPEAK-COMPLETE event */
                /*You can notify to business layer about STT server response here*/
			}
			else if(pMrcpMessage->start_line.request_state == MRCP_REQUEST_STATE_COMPLETE) 
			{
				/* received unexpected response, terminate the session */
				//Terminate();
				/*You can notify to business layer about STT server response here*/

				/* Sending TTS Speak Complete msg to bridge for resuming the STT recognition */
				std::cout<<"!! received unexpected response "<<std::endl;
                UrMrcpSessionDetails *sessionInfo;
                mrcp_client_session_t* session = (mrcp_client_session_t*)pMrcpChannel->session;
                std::map<std::string,UrMrcpSessionDetails *>::iterator it = UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.find(session->base.name);
                if(it != UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.end() && it->second)
                {
                        sessionInfo = it->second;
                }
                else
                {
                        XGLOG_ERROR("MrcpCliAppSynthSession::OnMessageReceive session info not found for callid(%s)", session->base.name);
                        return false;
                }
                xGateDbConfigMgrServiceMsg *mrcpServiceMsg = new xGateDbConfigMgrServiceMsg();
                mrcpServiceMsg->m_callId = session->base.name;
                sessionInfo->m_sessionSynthState =  EN_SESSION_SYNTH_COMPLETED;
                mrcpServiceMsg->set_mrcp_command(EN_TTS_SPEAK_COMPLETE);
                UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
			}
		}
		else 
		{
			UrMrcpSessionDetails *sessionInfo;
			mrcp_client_session_t* session = (mrcp_client_session_t*)pMrcpChannel->session;
			std::map<std::string,UrMrcpSessionDetails *>::iterator it = UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.find(session->base.name);
			std::string ID = session->base.name;
			XGLOG_INFO("MrcpCliAppSynthSession::OnMessageReceive got response for TTS Stop ID[%s]",ID.c_str());
			if(it != UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.end() && it->second)
			{
				sessionInfo = it->second;
			}
			else
			{
				XGLOG_ERROR("MrcpCliAppSynthSession::OnMessageReceive session info not found for callid(%s)", session->base.name);
				return false;
			}
			sessionInfo->m_sessionSynthState =  EN_SESSION_SYNTH_COMPLETED;
			/* received unexpected response */
		}
	}
	else if(pMrcpMessage->start_line.message_type == MRCP_MESSAGE_TYPE_EVENT) 
	{
		/* Getting TTS complete response for normal scenario */
		/* received MRCP event */
		if(pMrcpMessage->start_line.method_id == SYNTHESIZER_SPEAK_COMPLETE) 
		{
			SynthChannel* pSynthChannel = (SynthChannel*) mrcp_application_channel_object_get(pMrcpChannel);
			if(pSynthChannel)
				pSynthChannel->m_pSpeakRequest = NULL;
			UrMrcpSessionDetails *sessionInfo;
			mrcp_client_session_t* session = (mrcp_client_session_t*)pMrcpChannel->session;
			std::map<std::string,UrMrcpSessionDetails *>::iterator it = UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.find(session->base.name);
			if(it != UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.end() && it->second)
			{
				sessionInfo = it->second;
			}
			else 
			{
				XGLOG_ERROR("MrcpCliAppSynthSession::OnMessageReceive session info not found for callid(%s)", session->base.name);
				return false;
			}
      if(!sessionInfo->m_avaamo_reply.empty())
      {
        std::list<bot_reply_msg>::iterator itr = sessionInfo->m_avaamo_reply.begin();
        if(itr != sessionInfo->m_avaamo_reply.end())
        {
			sessionInfo->m_playing = (*itr).m_bOrignal;
          std::string text("");
          text = (*itr).m_strMessage.c_str();
          std::string xml_content = FrameXml(text);
          GetScenario()->SetContent(xml_content.c_str());
          SendSynthesisRequest(pMrcpChannel);
          sessionInfo->m_avaamo_reply.erase(itr);
          return;
        }
      }
      xGateDbConfigMgrServiceMsg *mrcpServiceMsg = new xGateDbConfigMgrServiceMsg();
      mrcpServiceMsg->m_callId = session->base.name;
      sessionInfo->m_sessionSynthState =  EN_SESSION_SYNTH_COMPLETED;
      mrcpServiceMsg->set_mrcp_command(EN_TTS_SPEAK_COMPLETE);
      UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
#if 0
			if(!sessionInfo->m_avaamo_replys.empty())
			{
				sessionInfo->m_seqNo_playing++;
				while(sessionInfo->m_seqNo_playing <= sessionInfo->m_totalMsg)
				{
					std::map<int,std::string>::iterator it = sessionInfo->m_avaamo_replys.find(sessionInfo->m_seqNo_playing);
					if(it != sessionInfo->m_avaamo_replys.end())
					{
						if(it->second.empty())
						{
							XGLOG_INFO("MrcpCliAppSynthSession::OnMessageReceive sequense message(%d) is empty for callid(%s)",sessionInfo->m_seqNo_playing, session->base.name);
							sessionInfo->m_seqNo_playing++;
							continue;
						}
						std::list<std::string> content;
						content.push_back((it->second).c_str());
						std::string xml_content = FrameXml(content);
						//GetScenario()->SetContent((it->second).c_str());
						GetScenario()->SetContent(xml_content.c_str());
						SendSynthesisRequest(pMrcpChannel);
						sessionInfo->m_avaamo_replys.erase(it);
						return true;
					}
					sessionInfo->m_seqNo_playing++;
				}
        //Need to reset the sequence no and total message
					sessionInfo->m_seqNo_playing = 0;
					sessionInfo->m_totalMsg = 0;
					sessionInfo->m_avaamo_replys.clear();
			}
#endif
#if 0
			if(!sessionInfo->m_transferTo.empty())
			{
				XGLOG_INFO("MrcpCliAppSynthSession::OnMessageReceive sending call Tranfer request for callid(%s)", mrcpServiceMsg->m_callId.c_str());
				mrcpServiceMsg->set_transfer_to(sessionInfo->m_transferTo);
				mrcpServiceMsg->set_mrcp_command(EN_MSG_TRANSFER_REQ);
				mrcpServiceMsg->setDstModuleId(IURDefines::UR_MODULE_SERVICE_BRIDGE);
				UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
			}
			else
			{
				if(!sessionInfo->m_dtmf_digits.empty())
					return true;
				XGLOG_INFO("MrcpCliAppSynthSession::OnMessageReceive going to post Recognize Request to Client module : callid =%s ", mrcpServiceMsg->m_callId.c_str());
#if 0
				mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_RECOGNITION_START_REQ);
				mrcpServiceMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
				UR_MRCP_MODULE_BRIDGE::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(mrcpServiceMsg));
#endif
				XGLOG_INFO("!!!!!! MrcpCliAppSynthSession::OnMessageReceive response recieved from Avaamo so processing with Asr Type - %d !!!!!!!!",sessionInfo->m_asrType);
				if(sessionInfo->m_isCcaas)
				{
					XGLOG_INFO("MrcpCliAppSynthSession::OnMessageReceive posting EN_TTS_SPEAK_COMPLETE msg to urbridge service : callid =%s ", mrcpServiceMsg->m_callId.c_str());
					XGLOG_INFO("MrcpCliAppSynthSession::OnMessageReceive posting EN_TTS_SPEAK_COMPLETE msg to urbridge service : m_tts_text_type = %d, m_ccaasPorttype = %d ", sessionInfo->m_tts_text_type,sessionInfo->m_ccaasPorttype);
					#if 1
					if(sessionInfo->m_tts_text_type == 1)
					{
						mrcpServiceMsg->set_mrcp_command(EN_TTS_SPEAK_COMPLETE);
						UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
						// sessionInfo->m_tts_text_type = 0;
						// return true;
					}
					#endif
					if(sessionInfo->m_ccaasPorttype == EN_XGATE_TTS)
					{
						XGLOG_INFO("! MrcpCliAppSynthSession::OnMessageReceive 'not posting' EN_TTS_SPEAK_COMPLETE msg to urbridge service as only 'TTS' is only enabled: callid =%s ", mrcpServiceMsg->m_callId.c_str());
						return true;
					}
				}
				
				if(/*sessionInfo->m_tts_text_type == 0 ||*/ sessionInfo->m_bisccaasChatBotInit)
				{
					xGateDbConfigMgrServiceMsg *sttServiceMsg = new xGateDbConfigMgrServiceMsg();
					sttServiceMsg->m_callId = sessionInfo->m_callId.c_str();
					if(sessionInfo->m_asrType == EN_XGATE_ASR_SPEECH_MATICS)
					{
						sttServiceMsg->set_mrcp_command(EN_STT_MSG_RECOGNITION_START_REQ);
						XGLOG_INFO("MrcpCliAppSynthSession::OnMessageReceive Asr_type is Speechmatics");
						printf("!!!!!! MrcpCliAppSynthSession::OnMessageReceive response recieved from Avaamo Asr_type is Speechmatics !!!!!!");
					}
					else if(sessionInfo->m_asrType == EN_XGATE_ASR_VOICE_GAIN)
					{
						sttServiceMsg->set_mrcp_command(EN_MRCP_MSG_RECOGNITION_START_REQ);
						XGLOG_INFO("MrcpCliAppSynthSession::OnMessageReceive Asr_type is VoiceGain");
						printf("!!!!!! MrcpCliAppSynthSession::OnMessageReceive response recieved from Avaamo Asr_type is VoiceGain !!!!!!");
					}
					else
					{
						XGLOG_INFO("MrcpCliAppSynthSession::OnMessageReceive Asr_type is empty");
						delete sttServiceMsg;
						sttServiceMsg - NULL;
					}					
					UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(sttServiceMsg);
					sessionInfo->m_tts_text_type = 0;
				}
			}
#endif
			/* received SPEAK-COMPLETE event, terminate the session */
			//Terminate();
            /*You can notify to business layer about STT server response here*/

		}
	}
	return true;
}

/*
    creates request with MRCP headers 
*/

mrcp_message_t* MrcpCliAppSynthSession::CreateSpeakRequest(mrcp_channel_t* pMrcpChannel)
{
	XGLOG_FUNCTION_EN_EX("");
	mrcp_message_t* pMrcpMessage = CreateMrcpMessage(pMrcpChannel,SYNTHESIZER_SPEAK);
	if(!pMrcpMessage)
		return NULL;

	const MrcpCliAppSynthScenario* pScenario = GetScenario();

	mrcp_generic_header_t* pGenericHeader;
	mrcp_synth_header_t* pSynthHeader;
	/* get/allocate generic header */
	pGenericHeader = (mrcp_generic_header_t*) mrcp_generic_header_prepare(pMrcpMessage);
	if(pGenericHeader) 
	{
		/* set generic header fields */
		apt_string_assign(&pGenericHeader->content_type,pScenario->GetContentType(),pMrcpMessage->pool);
		mrcp_generic_header_property_add(pMrcpMessage,GENERIC_HEADER_CONTENT_TYPE);

		/* set message body */
		if(pScenario->GetContent())
			apt_string_assign_n(&pMrcpMessage->body,pScenario->GetContent(),pScenario->GetContentLength(),pMrcpMessage->pool);
	}
	/* get/allocate synthesizer header */
	pSynthHeader = (mrcp_synth_header_t*) mrcp_resource_header_prepare(pMrcpMessage);
	if(pSynthHeader) 
	{
#if 0
		/* set synthesizer header fields */
		pSynthHeader->voice_param.age = 28;
		mrcp_resource_header_property_add(pMrcpMessage,SYNTHESIZER_HEADER_VOICE_AGE);
#endif
		const char* pSpeechLanguage = pScenario->GetSpeechLanguage();
		if (pSpeechLanguage)
		{
			apt_string_assign(&pSynthHeader->speech_language, pSpeechLanguage, pMrcpMessage->pool);
			mrcp_resource_header_property_add(pMrcpMessage, SYNTHESIZER_HEADER_SPEECH_LANGUAGE);
		}
	}

	return pMrcpMessage;
}

/*
    Write received audio from TTS server into a file
*/
FILE* MrcpCliAppSynthSession::GetAudioOut(const mpf_codec_descriptor_t* pDescriptor, apr_pool_t* pool) const
{
	XGLOG_FUNCTION_EN_EX("");
	FILE* file;
	char* pFileName = apr_psprintf(pool,"synth-%dkHz-%s.pcm",pDescriptor->sampling_rate/1000, GetMrcpSessionId());
	apt_dir_layout_t* pDirLayout = GetScenario()->GetDirLayout();
	char* pFilePath = apt_vardir_filepath_get(pDirLayout,pFileName,pool);
	if(!pFilePath)
		return NULL;

	apt_log(APT_LOG_MARK,APT_PRIO_INFO,"Open Speech Output File [%s] for Writing",pFilePath);
	file = fopen(pFilePath,"wb");
	if(!file)
	{
		apt_log(APT_LOG_MARK,APT_PRIO_WARNING,"Failed to Open Speech Output File [%s] for Writing",pFilePath);
		return NULL;
	}
	return file;
}

/*
    Get the loaded configurations from scenario object. 
*/
const MrcpCliAppSynthScenario* MrcpCliAppSynthSession::GetScenario() const
{
	return (MrcpCliAppSynthScenario*)m_pScenario;
}

