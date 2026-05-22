/*
    Author: @PraveenMS
    
    Speech recognition session managment functions, they invoker wrapper functions. 

    Layers of function

    ------------------------------------
    | ur_mrcp_controller               | - interacts with business layer
    ------------------------------------
    | xGateMrcpCliAppHandler           | - interacts with controller later
    ------------------------------------
    | UniMrcpCliAppFwSpeechRecog       | - Framework of speech recognition
    ------------------------------------
    | MrcpCliAppSpeechRecognizeScenario| - Store necessary infomation for the current session
    ------------------------------------
    | MrcpCliAppSpeechRecognizeSession | - Responsible for session creation, modification and update
    ------------------------------------
    | WrapSpeechRecogMrcpSession       | - Wrapper functions for UniMrcp lib apis
    ------------------------------------
    | UniMRCP 1.7.0                    | - 3rd party library 
    ------------------------------------
    | SIP          | MRCPv2            |
    ------------------------------------

    Note: RTP is responsibility of MG. This client has RTP capability for testing purpose only
    To integrate STT server with MG,  configure MG and RTP port range under profiles "RTP-Settings-1"
    and "RTP-Factory-1" in configuration file "unimrcpclient.xml". MRCP client will share this information to 
    STT server during sdp and mrcp negotition process. 
    
*/

#include "MrcpCliAppSpeechRecognizeSession.h"
#include "MrcpCliAppSpeechRecognizeScenario.h"
#include "mrcp_message.h"
#include "mrcp_generic_header.h"
#include "mrcp_recog_header.h"
#include "mrcp_recog_resource.h"
#include "apt_nlsml_doc.h"
#include "apt_log.h"
#include "xglog.h"
#include "URMRCPModuleBridge.h"
#include "mrcp_client_session.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"


struct RecogChannel
{
	mrcp_channel_t* m_pMrcpChannel;
	mrcp_message_t* m_pRecogRequest;
	bool            m_Streaming;
	FILE*           m_pAudioIn;	
	apr_size_t      m_TimeToComplete;

	RecogChannel() :
		m_pMrcpChannel(NULL),
		m_pRecogRequest(NULL),
		m_Streaming(false),
		m_pAudioIn(NULL),
		m_TimeToComplete(0) {}
};

MrcpCliAppSpeechRecognizeSession::MrcpCliAppSpeechRecognizeSession(const MrcpCliAppSpeechRecognizeScenario* pScenario) :
	WrapSpeechRecogMrcpSession(pScenario),
	m_pRecogChannel(NULL),
	m_ContentId("request1@form-level")
{
    XGLOG_FUNCTION_EN_EX("");
}

MrcpCliAppSpeechRecognizeSession::~MrcpCliAppSpeechRecognizeSession()
{
}

/*
    Load data and start session with server
*/
bool MrcpCliAppSpeechRecognizeSession::Start()
{
    XGLOG_FUNCTION_EN_EX("");
	const MrcpCliAppSpeechRecognizeScenario* pScenario = GetSpeechRecognizeScenario();
	
	if(!pScenario) {
        XGLOG_ERROR("MrcpCliAppSpeechRecognizeSession::Start failed, GetSpeechRecognizeScenario is null");
        return false;
    }

	if(!pScenario->IsDefineGrammarEnabled() && !pScenario->IsRecognizeEnabled())
		return false;
	
	/* create channel and associate all the required data */
	m_pRecogChannel = CreateRecogChannel();
	if(!m_pRecogChannel) 
		return false;

	/* add channel to session (send asynchronous request) */
	if(!AddMrcpChannel(m_pRecogChannel->m_pMrcpChannel))
	{
		delete m_pRecogChannel;
		m_pRecogChannel = NULL;
		return false;
	}
	return true;
}

/*
    shutdown session gracefully with server
*/
bool MrcpCliAppSpeechRecognizeSession::Stop()
{
    XGLOG_FUNCTION_EN_EX("");
	if(!WrapSpeechRecogMrcpSession::Stop())
		return false;

	if(!m_pRecogChannel)
		return false;

	mrcp_message_t* pStopMessage = CreateMrcpMessage(m_pRecogChannel->m_pMrcpChannel,RECOGNIZER_STOP);
	if(!pStopMessage)
		return false;

	if(m_pRecogChannel->m_pRecogRequest)
	{
		mrcp_generic_header_t* pGenericHeader;
		/* get/allocate generic header */
		pGenericHeader = (mrcp_generic_header_t*) mrcp_generic_header_prepare(pStopMessage);
		if(pGenericHeader) 
		{
			pGenericHeader->active_request_id_list.count = 1;
			pGenericHeader->active_request_id_list.ids[0] = 
				m_pRecogChannel->m_pRecogRequest->start_line.request_id;
			mrcp_generic_header_property_add(pStopMessage,GENERIC_HEADER_ACTIVE_REQUEST_ID_LIST);
		}

		m_pRecogChannel->m_pRecogRequest = NULL;
	}
	
	return SendMrcpRequest(m_pRecogChannel->m_pMrcpChannel,pStopMessage);
}

/*
    use to terminate the processing session
*/
bool MrcpCliAppSpeechRecognizeSession::OnSessionTerminate(mrcp_sig_status_code_e status)
{
    XGLOG_FUNCTION_EN_EX("");
	if(m_pRecogChannel)
	{
		FILE* pAudioIn = m_pRecogChannel->m_pAudioIn;
		if(pAudioIn)
		{
			m_pRecogChannel->m_pAudioIn = NULL;
			fclose(pAudioIn);
		}
		
		delete m_pRecogChannel;
		m_pRecogChannel = NULL;
	}
	return WrapSpeechRecogMrcpSession::OnSessionTerminate(status);
}

/*
    ReadStream Used to read the audio file and play it to server. 
    Used only when RTP channel is supported between MRCP client and STT server. 
    If MG is supporting RTP channel, then this function is not used. 
*/

static apt_bool_t ReadStream(mpf_audio_stream_t* pStream, mpf_frame_t* pFrame)
{
   // XGLOG_FUNCTION_EN_EX("");
	RecogChannel* pRecogChannel = (RecogChannel*) pStream->obj;
	if(pRecogChannel && pRecogChannel->m_Streaming) 
	{
		if(pRecogChannel->m_pAudioIn) 
		{
			if(fread(pFrame->codec_frame.buffer,1,pFrame->codec_frame.size,pRecogChannel->m_pAudioIn) == pFrame->codec_frame.size) 
			{
				/* normal read */
				pFrame->type |= MEDIA_FRAME_TYPE_AUDIO;
			}
			else 
			{
				/* file is over */
				pRecogChannel->m_Streaming = false;
			}
		}
		else 
		{
			/* fill with silence in case no file available */
			if(pRecogChannel->m_TimeToComplete >= CODEC_FRAME_TIME_BASE) 
			{
				pFrame->type |= MEDIA_FRAME_TYPE_AUDIO;
				memset(pFrame->codec_frame.buffer,0,pFrame->codec_frame.size);
				pRecogChannel->m_TimeToComplete -= CODEC_FRAME_TIME_BASE;
			}
			else 
			{
				pRecogChannel->m_Streaming = false;
			}
		}
	}
	return TRUE;
}

/*
    create channel for speech recognition 
*/

RecogChannel* MrcpCliAppSpeechRecognizeSession::CreateRecogChannel()
{
    XGLOG_FUNCTION_EN_EX("");
	mrcp_channel_t* pChannel;
	mpf_termination_t* pTermination;
	mpf_stream_capabilities_t* pCapabilities;
	apr_pool_t* pool = GetSessionPool();

	/* create channel */
	RecogChannel* pRecogChannel = new RecogChannel;

#if 0
	/* create source stream capabilities */
	pCapabilities = mpf_source_stream_capabilities_create(pool);
	GetSpeechRecognizeScenario()->InitCapabilities(pCapabilities);

	static const mpf_audio_stream_vtable_t audio_stream_vtable = 
	{
		NULL,
		NULL,
		NULL,
		ReadStream,
		NULL,
		NULL,
		NULL,
		NULL
	};

	pTermination = CreateAudioTermination(
			&audio_stream_vtable,      /* virtual methods table of audio stream */
			pCapabilities,             /* capabilities of audio stream */
			pRecogChannel);            /* object to associate */
#endif
        /* create source stream capabilities */
        mpf_rtp_termination_descriptor_t *rtp_descriptor;
        rtp_descriptor = NULL;

        rtp_descriptor = mpf_rtp_termination_descriptor_alloc(pool);
        //string mgip = "10.22.2.152";
        rtp_descriptor->audio.local = mpf_rtp_media_descriptor_alloc(pool);
        rtp_descriptor->audio.local->state = MPF_MEDIA_ENABLED;
        apt_string_assign(&rtp_descriptor->audio.local->ip,m_tSdpDescriptor->m_mrcpMgIp.c_str(),pool);
        //static int port = 5002;
        rtp_descriptor->audio.local->port = atoi(m_tSdpDescriptor->m_mrcpMgPort.c_str());
        rtp_descriptor->audio.local->direction = STREAM_DIRECTION_DUPLEX;
        rtp_descriptor->audio.local->ptime = m_tSdpDescriptor->m_ptime;
        mpf_codec_list_init(&rtp_descriptor->audio.local->codec_list, 1, pool);
        mpf_codec_descriptor_t* codec_discriptor = mpf_codec_list_add(&rtp_descriptor->audio.local->codec_list);
        codec_discriptor->payload_type = 8;
        apt_string_assign(&codec_discriptor->name,"PCMA",pool);
        codec_discriptor->sampling_rate = 8000;
        codec_discriptor->enabled = 1;

	pChannel = CreateMrcpChannel(
			MRCP_RECOGNIZER_RESOURCE,  /* MRCP resource identifier */
			pTermination,              /* media termination, used to terminate audio stream */
			rtp_descriptor,                      /* RTP descriptor, used to create RTP termination (NULL by default) */
			pRecogChannel);            /* object to associate */
	if(!pChannel)
	{
		delete pRecogChannel;
		return NULL;
	}
	
	pRecogChannel->m_pMrcpChannel = pChannel;
	return pRecogChannel;
}

/*
    Add channel for speech recognition. 
    Here it starts the speach recognition
*/
bool MrcpCliAppSpeechRecognizeSession::OnChannelAdd(mrcp_channel_t* pMrcpChannel, mrcp_sig_status_code_e status)
{
    XGLOG_FUNCTION_EN_EX("");
	if(!WrapSpeechRecogMrcpSession::OnChannelAdd(pMrcpChannel,status))
		return false;

	if(status != MRCP_SIG_STATUS_CODE_SUCCESS)
	{
		/* error case, just terminate the demo */
		return Terminate();
	}

	if(GetSpeechRecognizeScenario()->IsDefineGrammarEnabled())
	{
		mrcp_message_t* pMrcpMessage = CreateDefineGrammarRequest(pMrcpChannel);
		if(pMrcpMessage)
			SendMrcpRequest(pMrcpChannel,pMrcpMessage);
		return true;
	}

//	return StartRecognition(pMrcpChannel);
	return true;
}

/*
    Received MRCP message/response from STT server for ongoing channel with status. 

    Note: While integrating MRCP client with Business layer like PBX, Here you can implement 
    add functions to pass the response back to business layer. which can take action or notify MG
    to take appropriate actions on the RTP channel and notify terminals accordingly.

    RECOG requet is trigger by calling following function from framework 
    UniMrcpCliAppFwSpeechRecog::ProcessSpeechRecogRequest() function.

    MRCP client will receive following MRCP messages from STT server. 
   
       MRCP Client                                   STT Server  
     (if grammar is enabled)
           |-------------- DEFINE-GRAMMAR --------------->|
           |<----------------COMPLETE---------------------|
           |                                              |
     (RECOGNIZE flow )      
           |----------------RECOGNIZE-------------------->|
           |<---------------IN-PROGRESS-------------------|
           |<---------RECOGNITION-COMPLETE----------------|
    
    MRCP messages received from STT server are handled in below funtion. 
*/

bool MrcpCliAppSpeechRecognizeSession::OnMessageReceive(mrcp_channel_t* pMrcpChannel, mrcp_message_t* pMrcpMessage)
{
    XGLOG_FUNCTION_EN_EX("");

	if(!WrapSpeechRecogMrcpSession::OnMessageReceive(pMrcpChannel,pMrcpMessage))
		return false;

	RecogChannel* pRecogChannel = (RecogChannel*) mrcp_application_channel_object_get(pMrcpChannel);
	if(!pRecogChannel)
		return false;

	if(pMrcpMessage->start_line.message_type == MRCP_MESSAGE_TYPE_RESPONSE) 
	{
		/* received MRCP response */
		if(pMrcpMessage->start_line.method_id == RECOGNIZER_DEFINE_GRAMMAR) 
		{
			/* received the response to DEFINE-GRAMMAR request */
            /*Note, client will send grammar request only if it is enabled in 
            UniMrcpCliAppFwSpeechRecog::ProcessSpeechRecogRequest() function. 
            */
			if(pMrcpMessage->start_line.request_state == MRCP_REQUEST_STATE_COMPLETE) 
			{
			    /*udpate the grammar if shared*/
				OnDefineGrammar(pMrcpChannel);
                /*You can notify to business layer about STT server response here*/
			}
			else 
			{
				/* received unexpected response, terminate the session */
				Terminate();
                /*You can notify to business layer about STT server response here*/
			}
		}
		else if(pMrcpMessage->start_line.method_id == RECOGNIZER_RECOGNIZE)
		{
			/* received the response to RECOGNIZE request */
			if(pMrcpMessage->start_line.request_state == MRCP_REQUEST_STATE_INPROGRESS || pMrcpMessage->start_line.request_state == MRCP_REQUEST_STATE_PENDING)
			{
				pRecogChannel->m_pRecogRequest = GetMrcpMessage();
				
				/* start to stream the speech to recognize */
				//pRecogChannel->m_Streaming = true;

                /*You can notify to business layer about STT server response here*/
			}
			else 
			{
				/* received unexpected response, terminate the session */
				Terminate();

                /*You can notify to business layer about STT server response here*/
			}
		}
		else 
		{
			/* received unexpected response */
		}
	}
	else if(pMrcpMessage->start_line.message_type == MRCP_MESSAGE_TYPE_EVENT) 
	{
		if(pMrcpMessage->start_line.method_id == RECOGNIZER_RECOGNITION_COMPLETE) 
		{
			printf("******************RECOGNIZER_RECOGNITION_COMPLETE ************************");
			ParseNLSMLResult(pMrcpMessage);
			xGateDbConfigMgrServiceMsg *mrcpServiceMsg = new xGateDbConfigMgrServiceMsg();
			mrcp_client_session_t* session = (mrcp_client_session_t*)pMrcpChannel->session;
			mrcpServiceMsg->m_callId = session->base.name;
			UrMrcpSessionDetails *sessionInfo = NULL;
			URMRCPModuleBridge *URModule = UR_MRCP_MODULE_BRIDGE::instance();
			std::map<std::string,UrMrcpSessionDetails *>::iterator it = URModule->m_sessionDetailMap.find(mrcpServiceMsg->m_callId);
			if(it!=URModule->m_sessionDetailMap.end() && it->second && it->second->m_sessionRecogState != EN_SESSION_RECOG_STOP)
			{
				sessionInfo = it->second;
				sessionInfo->m_sessionRecogState = EN_SESSION_RECOG_COMPLETED;
				if(!sessionInfo->m_dtmf_digits.empty())
					return true;
			}
			else
			{
				return false;
			}
			if(pMrcpMessage)
			{
				std::string message = apt_string_buffer_get(&pMrcpMessage->body);
//                        printf("\n\n\n Converted text:  %s ************** %s \n\n\n", message.c_str(),mrcpServiceMsg->m_callId.c_str());
				std::string text("");                        
				text = decode_converted_text(message);
				if(!text.empty())
				{
					printf("******************RECOGNIZER_RECOGNITION_COMPLETE message posting to HTTP ************************");
					//mrcpServiceMsg->m_displayText = pMrcpMessage->body.buf;
				#if 0
					static int i=0;
					if(i==0)
					{
						mrcpServiceMsg->m_convertedText = "live agent";
						i++;
					}
					else if(i==1)
					{
						mrcpServiceMsg->m_convertedText = "201";
						i = 0;
					}
#else
					mrcpServiceMsg->m_convertedText = text.c_str();
#endif
					//mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_HTTP);
					mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
					XGLOG_INFO("MrcpCliAppSpeechRecognizeSession::OnMessageReceive going to post converted text to HTTP module : callid =%s and data =%s", mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_convertedText.c_str());
					UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
					return true;
#if 0
					mrcpServiceMsg->m_displayText = pMrcpMessage->body.buf;
					mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
					UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
#endif
				}
			}
			if(sessionInfo->m_sessionSynthState != EN_SESSION_SYNTH_START)
			{
				std::cout<<"Converted text is empty, So again sending Recognize Request"<<endl;
				//			StartRecognition(pMrcpChannel);

				xGateDbConfigMgrServiceMsg *pMsg = new xGateDbConfigMgrServiceMsg();
				pMsg->set_mrcp_command(EN_MRCP_MSG_SYNTHESIZE_REQ);
				pMsg->m_callId = mrcpServiceMsg->m_callId.c_str();
				pMsg->m_displayText.push_back(MRCP_CONFIG::instance()->m_no_query_from_user_ivr.c_str());
				pMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
				UR_MRCP_MODULE_BRIDGE::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
			}
#if 0
			if(isEOS)
			{
				StartRecognition(pMrcpChannel);
				isEOS = false;
			}
			else
			{
				isRecogCompleted = true;
			}
#endif
			//SendMrcpRequest(pRecogChannel->m_pMrcpChannel,pMrcpMessage);

			#if 0

                        if(pMrcpMessage->body.buf){
                        PublishDetails *var = new PublishDetails();
                        std::string data = var->encode_Json(var,pMrcpMessage->body.buf);
                        ConnectDetails detail;
                        detail.dbConnect(data.c_str());
	                        }
			#endif

			// Added to post converted text to Bridge module -starts
			#if 0
			if(pMrcpMessage->body.buf)
			{
				 mrcpServiceMsg->m_displayText = pMrcpMessage->body.buf;
				 mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_HTTP);
				 XGLOG_INFO("MrcpCliAppSpeechRecognizeSession::OnMessageReceive going to post converted text to HTTP module : callid =%s and data =%s", mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_displayText.c_str());
				UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
			}
			#endif
                        // Added to post converted text to Bridge module - Ends	
	

            /*
                This is where you will receive the translated text. 
                
                'pMrcpMessage->body->buf' will hold the response from translated text.
                Example: 
                "<?xml version=\"1.0\"?>\n<result> \n  <interpretation grammar=\"session:request1@form-level.store\" confidence=\"0.97\">\n    <instance>one</instance>\n    <input mode=\"speech\">one</input>\n  </interpretation>\n<"
    
            */

			pRecogChannel->m_Streaming = false;
			//pRecogChannel->m_pRecogRequest = NULL;


                        //pRecogChannel->m_pRecogRequest = GetMrcpMessage();
                        //StartRecognition(pMrcpChannel);

			//Terminate();

            /*You can notify to business layer about STT server response here*/
		}
		else if(pMrcpMessage->start_line.method_id == RECOGNIZER_START_OF_INPUT) 
		{
			/* received start-of-input, do whatever you need here */

            /*You can notify to business layer about STT server response here*/
		}
	}
	return true;
}

std::string MrcpCliAppSpeechRecognizeSession::decode_converted_text(std::string xml_str)
{
	XGLOG_FUNCTION_EN_EX("");
	std::string text("");
	rapidxml::xml_document<> m_doc;
	m_doc.parse<0>(&xml_str[0]);
	rapidxml::xml_node<>*root = m_doc.first_node();
	if(root)
	{
		std::string node_name = "interpretation";
		rapidxml::xml_node<>*subnode = root->first_node(node_name.c_str());
		if(subnode)
		{
			node_name = "input";
			subnode = subnode->first_node(node_name.c_str());
			if(subnode)
				text = subnode->value();
		}
	}
	return text;
}
/*
    udpate gammer if received (optional)
*/
bool MrcpCliAppSpeechRecognizeSession::OnDefineGrammar(mrcp_channel_t* pMrcpChannel)
{
    XGLOG_FUNCTION_EN_EX("");
	if(GetSpeechRecognizeScenario()->IsRecognizeEnabled())
	{
		return StartRecognition(pMrcpChannel);
	}

	return Terminate();
}

/*
    This function creates & sends Recognition request to STT server 
*/

bool MrcpCliAppSpeechRecognizeSession::StartRecognition(mrcp_channel_t* pMrcpChannel)
{
    XGLOG_FUNCTION_EN_EX("");
#if 0    
	const mpf_codec_descriptor_t* pDescriptor = mrcp_application_source_descriptor_get(pMrcpChannel);
	if(!pDescriptor)
	{
		apt_log(APT_LOG_MARK,APT_PRIO_WARNING,"Failed to Get Media Source Descriptor");
		return Terminate();
	}
#endif
	RecogChannel* pRecogChannel = (RecogChannel*) mrcp_application_channel_object_get(pMrcpChannel);
	/* create and send RECOGNIZE request */
	mrcp_message_t* pMrcpMessage = CreateRecognizeRequest(pMrcpChannel);

	if(pMrcpMessage)
	{
#if 0
		xGateDbConfigMgrServiceMsg *mrcpServiceMsg = new xGateDbConfigMgrServiceMsg();
mrcp_client_session_t* session = (mrcp_client_session_t*)pMrcpChannel->session;
		mrcpServiceMsg->m_callId = session->base.name;
		mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_RECOGNITION_SENT);
		UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
		printf("!!!!!!!!!!!!!!!!!!!!!client sending 'EN_MRCP_MSG_RECOGNITION_SENT'for sessionId(%s)!!!!!!!!!!!!!!!!!!\n",mrcpServiceMsg->m_callId.c_str());
#endif
		SendMrcpRequest(pRecogChannel->m_pMrcpChannel,pMrcpMessage);
	}

    /*
        Here you can notify business layer to noify the MG to start the streaming of audio. 
        and You shall commmet below piece of code. which plays .pcm audio files to server
    */
#if 0
	pRecogChannel->m_pAudioIn = GetAudioIn(pDescriptor,GetSessionPool());
	if(!pRecogChannel->m_pAudioIn)
	{
		/* no audio input availble, set some estimated time to complete instead */
		pRecogChannel->m_TimeToComplete = 5000; // 5 sec
	}
#endif
	return true;
}

/*
   This function creates & sends GRAMMAR request to STT server 
*/
mrcp_message_t* MrcpCliAppSpeechRecognizeSession::CreateDefineGrammarRequest(mrcp_channel_t* pMrcpChannel)
{
    XGLOG_FUNCTION_EN_EX("");
	mrcp_message_t* pMrcpMessage = CreateMrcpMessage(pMrcpChannel,RECOGNIZER_DEFINE_GRAMMAR);
	if(!pMrcpMessage)
		return NULL;

	const MrcpCliAppSpeechRecognizeScenario* pScenario = GetSpeechRecognizeScenario();

	mrcp_generic_header_t* pGenericHeader;
	/* get/allocate generic header */
	pGenericHeader = (mrcp_generic_header_t*) mrcp_generic_header_prepare(pMrcpMessage);
	if(pGenericHeader) 
	{
		/* set generic header fields */
		if(pScenario->GetContentType())
		{
			apt_string_assign(&pGenericHeader->content_type,pScenario->GetContentType(),pMrcpMessage->pool);
			mrcp_generic_header_property_add(pMrcpMessage,GENERIC_HEADER_CONTENT_TYPE);
		}
		apt_string_assign(&pGenericHeader->content_id,m_ContentId,pMrcpMessage->pool);
		mrcp_generic_header_property_add(pMrcpMessage,GENERIC_HEADER_CONTENT_ID);
	}

	/* set message body */
	if(pScenario->GetContent())
		apt_string_assign_n(&pMrcpMessage->body,pScenario->GetContent(),pScenario->GetContentLength(),pMrcpMessage->pool);
	return pMrcpMessage;
}

/*
    sub function of above StartRecognition()
    creates request with MRCP headers 
*/
mrcp_message_t* MrcpCliAppSpeechRecognizeSession::CreateRecognizeRequest(mrcp_channel_t* pMrcpChannel)
{
    XGLOG_FUNCTION_EN_EX("");
	mrcp_message_t* pMrcpMessage = CreateMrcpMessage(pMrcpChannel,RECOGNIZER_RECOGNIZE);
	if(!pMrcpMessage)
		return NULL;

	const MrcpCliAppSpeechRecognizeScenario* pScenario = GetSpeechRecognizeScenario();

	mrcp_generic_header_t* pGenericHeader;
	mrcp_recog_header_t* pRecogHeader;

	/* get/allocate generic header */
	pGenericHeader = (mrcp_generic_header_t*) mrcp_generic_header_prepare(pMrcpMessage);
	if(pGenericHeader)
	{
		/* set generic header fields */
		if(pScenario->IsDefineGrammarEnabled())
		{
			apt_string_assign(&pGenericHeader->content_type,"text/uri-list",pMrcpMessage->pool);
			/* set message body */
			const char* pContent = apr_pstrcat(pMrcpMessage->pool,"session:",m_ContentId,NULL);
			apt_string_set(&pMrcpMessage->body,pContent);
		}
		else
		{
			apt_string_assign(&pGenericHeader->content_type,pScenario->GetContentType(),pMrcpMessage->pool);
			/* set content-id */
			apt_string_assign(&pGenericHeader->content_id,m_ContentId,pMrcpMessage->pool);
			mrcp_generic_header_property_add(pMrcpMessage,GENERIC_HEADER_CONTENT_ID);
			/* set message body */
			if(pScenario->GetContent())
				apt_string_assign_n(&pMrcpMessage->body,pScenario->GetContent(),pScenario->GetContentLength(),pMrcpMessage->pool);
		}
		mrcp_generic_header_property_add(pMrcpMessage,GENERIC_HEADER_CONTENT_TYPE);
	}
	/* get/allocate recognizer header */
	pRecogHeader = (mrcp_recog_header_t*) mrcp_resource_header_prepare(pMrcpMessage);
	if(pRecogHeader)
	{
		/* set recognizer header fields */
		if(pMrcpMessage->start_line.version == MRCP_VERSION_2)
		{
			pRecogHeader->cancel_if_queue = FALSE;
			mrcp_resource_header_property_add(pMrcpMessage,RECOGNIZER_HEADER_CANCEL_IF_QUEUE);
		}
		pRecogHeader->recognition_timeout = 8000;
		mrcp_resource_header_property_add(pMrcpMessage,RECOGNIZER_HEADER_RECOGNITION_TIMEOUT);
		pRecogHeader->start_input_timers = FALSE;
		mrcp_resource_header_property_add(pMrcpMessage,RECOGNIZER_HEADER_START_INPUT_TIMERS);
		pRecogHeader->confidence_threshold = 0.9f;
		mrcp_resource_header_property_add(pMrcpMessage,RECOGNIZER_HEADER_CONFIDENCE_THRESHOLD);
		pRecogHeader->save_waveform = FALSE;
		mrcp_resource_header_property_add(pMrcpMessage,RECOGNIZER_HEADER_SAVE_WAVEFORM);
		pRecogHeader->sensitivity_level = 0.0f;
		mrcp_resource_header_property_add(pMrcpMessage,RECOGNIZER_HEADER_SENSITIVITY_LEVEL);
		pRecogHeader->speed_vs_accuracy = 1.0f;
		mrcp_resource_header_property_add(pMrcpMessage,RECOGNIZER_HEADER_SPEED_VS_ACCURACY);
		pRecogHeader->speech_complete_timeout = 1;
		mrcp_resource_header_property_add(pMrcpMessage,RECOGNIZER_HEADER_SPEECH_COMPLETE_TIMEOUT);
	}
	return pMrcpMessage;
}

/*
    After receiving the translated text, you shall parse using NLSML(Natural Language Semantics Markup Language) 
    For future use, as your business. 
*/

bool MrcpCliAppSpeechRecognizeSession::ParseNLSMLResult(mrcp_message_t* pMrcpMessage)
{
    XGLOG_FUNCTION_EN_EX("");
	nlsml_result_t *pResult = nlsml_result_parse(pMrcpMessage->body.buf, pMrcpMessage->body.length, pMrcpMessage->pool);
	if(!pResult)
		return false;

	nlsml_result_trace(pResult, pMrcpMessage->pool);
	return true;
}

/*
    Play audio file to STT server!
    Right now we are using it for testing MRCP client application. 

    In actual implementation, MG will stream the audio 

*/
FILE* MrcpCliAppSpeechRecognizeSession::GetAudioIn(const mpf_codec_descriptor_t* pDescriptor, apr_pool_t* pool) const
{
    XGLOG_FUNCTION_EN_EX("");
	const char* pFileName = GetSpeechRecognizeScenario()->GetAudioSource();
	if(!pFileName)
	{
		pFileName = apr_psprintf(pool,"one-%dkHz.pcm",pDescriptor->sampling_rate/1000);
	}
	apt_dir_layout_t* pDirLayout = GetSpeechRecognizeScenario()->GetDirLayout();
	const char* pFilePath = apt_datadir_filepath_get(pDirLayout,pFileName,pool);
	if(!pFilePath)
		return NULL;
	
	FILE* pFile = fopen(pFilePath,"rb");
	if(!pFile)
	{
		apt_log(APT_LOG_MARK,APT_PRIO_INFO,"Cannot Find [%s]",pFilePath);
		return NULL;
	}

	apt_log(APT_LOG_MARK,APT_PRIO_INFO,"Set [%s] as Speech Source",pFilePath);
	return pFile;
}

/*
    Get the loaded configurations from scenario object. 
*/
const MrcpCliAppSpeechRecognizeScenario* MrcpCliAppSpeechRecognizeSession::GetSpeechRecognizeScenario() const
{
    XGLOG_FUNCTION_EN_EX("");
	return (MrcpCliAppSpeechRecognizeScenario*)m_pRecogScenario;
}

