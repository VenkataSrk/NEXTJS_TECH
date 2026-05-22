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

    call UniMrcpCliAppFw::ProcessSynthRequest() to start TTS session.

    Note: RTP is responsibility of MG. This client has RTP capability for testing purpose only
    To integrate STT server with MG,  configure MG and RTP port range under profiles "RTP-Settings-1"
    and "RTP-Factory-1" in configuration file "unimrcpclient.xml". MRCP client will share this information to 
    STT server during sdp and mrcp negotition process. 
    
*/


#include <apr_fnmatch.h>
#include "UniMrcpCliAppFw.h"
#include "MrcpCliAppSynthScenario.h"
#include "unimrcp_client.h"
#include "apt_log.h"
#include "xglog.h"
#include "mrcp_client_session.h"
#include "mrcp_session_descriptor.h"
#include "URMRCPModuleBridge.h"
#include "mrcp_application.h"
#include "MrcpCliAppSynthSession.h"

typedef struct
{
	char                      m_SessionId[10];
	char                      m_ScenarioName[128];
	char                      m_ProfileName[128];
	const mrcp_app_message_t* m_pAppMessage;
	WrapMrcpSession*               m_pSession;
} UmcTaskMsg;

enum UmcTaskMsgType
{
	UMC_TASK_CLIENT_MSG,
	UMC_TASK_RUN_SESSION_MSG,
	UMC_TASK_STOP_SESSION_MSG,
	UMC_TASK_KILL_SESSION_MSG,
	UMC_TASK_SHOW_SCENARIOS_MSG,
	UMC_TASK_SHOW_SESSIONS_MSG,
	UMC_TASK_EXIT_SESSION_MSG
};

apt_bool_t UmcProcessMsg(apt_task_t* pTask, apt_task_msg_t* pMsg);
void UmcOnStartComplete(apt_task_t* pTask);
void UmcOnTerminateComplete(apt_task_t* pTask);
apt_bool_t AppMessageHandler(const mrcp_app_message_t* pAppMessage);


UniMrcpCliAppFw::UniMrcpCliAppFw() :
	m_pPool(NULL),
	m_pDirLayout(NULL),
	m_pTask(NULL),
	m_pMrcpClient(NULL),
	m_pMrcpApplication(NULL),
	m_pSessionTable(NULL)
{
	XGLOG_FUNCTION_EN_EX("");
}

UniMrcpCliAppFw::~UniMrcpCliAppFw()
{
}

/*allocate process memory*/
bool UniMrcpCliAppFw::Create(apt_dir_layout_t* pDirLayout, apr_pool_t* pool)
{
	XGLOG_FUNCTION_EN_EX("");
	m_pDirLayout = pDirLayout;
	m_pPool = pool; /*pool to allocate memory from*/

    /*Create pool of hash for stroign session objects (alternative for map)*/
	m_pSessionTable = apr_hash_make(m_pPool);
    /*Create pool of hash for stroign scenarios objects (alternative for map)*/
	//m_pScenarioTable = apr_hash_make(m_pPool);
	return CreateTask();
}

void UniMrcpCliAppFw::Destroy()
{
	XGLOG_FUNCTION_EN_EX("");
	DestroyTask();

	//m_pScenarioTable = NULL;
	m_pSessionTable = NULL;
}

/*create tasks to be triggered on session initiation, processing msg and session termination */
bool UniMrcpCliAppFw::CreateTask()
{
	XGLOG_FUNCTION_EN_EX("");
	apt_task_t* pTask;
	apt_task_vtable_t* pVtable;
	apt_task_msg_pool_t* pMsgPool;

	pMsgPool = apt_task_msg_pool_create_dynamic(sizeof(UmcTaskMsg),m_pPool);
	m_pTask = apt_consumer_task_create(this,pMsgPool,m_pPool);
	if(!m_pTask)
		return false;

	pTask = apt_consumer_task_base_get(m_pTask);
	apt_task_name_set(pTask,"Framework Agent");
	pVtable = apt_consumer_task_vtable_get(m_pTask);
	if(pVtable) 
	{
		pVtable->process_msg = UmcProcessMsg;
		pVtable->on_start_complete = UmcOnStartComplete;
		pVtable->on_terminate_complete = UmcOnTerminateComplete;
	}
    /*UniMRCP lib start the task*/
	apt_task_start(pTask);
	return true;
}

void UniMrcpCliAppFw::DestroyTask()
{
	XGLOG_FUNCTION_EN_EX("");
	if(m_pTask)
	{
		apt_task_t* pTask = apt_consumer_task_base_get(m_pTask);
		if(pTask)
		{
			apt_task_terminate(pTask,TRUE);
			apt_task_destroy(pTask);
		}
		m_pTask = NULL;
	}
}


/*creating MRCP client to send/receive from MRCP server, to be called by 'UmcOnStartComplete'*/
bool UniMrcpCliAppFw::CreateMrcpClient()
{
	XGLOG_FUNCTION_EN_EX("");
	/* create MRCP client stack first */
	m_pMrcpClient = unimrcp_client_create(m_pDirLayout);
	if(!m_pMrcpClient)
		return false;

	/* create MRCP application to send/get requests to/from MRCP client stack */
	m_pMrcpApplication = mrcp_application_create(AppMessageHandler,this,m_pPool);
	if(!m_pMrcpApplication)
	{
	    XGLOG_ERROR("UniMrcpCliAppFw::CreateMrcpClient mrcp_application_create failed");
		mrcp_client_destroy(m_pMrcpClient);
		m_pMrcpClient = NULL;
		return false;
	}

	/* register MRCP application to MRCP client */
	mrcp_client_application_register(m_pMrcpClient,m_pMrcpApplication,"UMC");
	/* start MRCP client stack processing */
	if(mrcp_client_start(m_pMrcpClient) == FALSE)
	{
	    XGLOG_ERROR("UniMrcpCliAppFw::CreateMrcpClient mrcp_client_start failed");
		mrcp_client_destroy(m_pMrcpClient);
		m_pMrcpClient = NULL;
		m_pMrcpApplication = NULL;
		return false;
	}
	return true;
}

void UniMrcpCliAppFw::DestroyMrcpClient()
{
	XGLOG_FUNCTION_EN_EX("");
	if(m_pMrcpClient)
	{
		/* shutdown MRCP client stack processing first (blocking call) */
		mrcp_client_shutdown(m_pMrcpClient);
		/* destroy MRCP client stack */
		mrcp_client_destroy(m_pMrcpClient);
		m_pMrcpClient = NULL;
		m_pMrcpApplication = NULL;
	}
}

bool UniMrcpCliAppFw::AddSession(WrapMrcpSession* pSession)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!pSession)
		return false;

//	apr_hash_set(m_pSessionTable,pSession->GetId(),APR_HASH_KEY_STRING,pSession);
	apr_hash_set(m_pSessionTable,pSession->sessionId.c_str(),APR_HASH_KEY_STRING,pSession);
	
	return true;
}

WrapMrcpSession* UniMrcpCliAppFw::GetSession(std::string sessionName)
{
        WrapMrcpSession* pSession = NULL;
        pSession = (WrapMrcpSession*)apr_hash_get(m_pSessionTable,sessionName.c_str(),APR_HASH_KEY_STRING);
        return pSession;
}

bool UniMrcpCliAppFw::RemoveSession(WrapMrcpSession* pSession)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!pSession)
		return false;

	apr_hash_set(m_pSessionTable,pSession->sessionId.c_str(),APR_HASH_KEY_STRING,NULL);
	return true;
}

/*
    call UniMrcpCliAppFw::ProcessSynthRequest() to start TTS session.

    Currently hardcoded values used to start the TTS scenarios. 
    You can optimize this function to use the values from business layer, by passing as arguments

*/

bool UniMrcpCliAppFw::ProcessSynthRequest()
{
	XGLOG_FUNCTION_EN_EX("");
	
    const char* pName = NULL;
    const char* pClass = NULL;
    const char* pMrcpProfile = NULL;
    const char* pProfileName = "uni2";
    
    /*Data needed for speech syntesis*/
    bool t_Speak = false;
    const char* t_SpeechLanguage = NULL;
    const char* t_ContentType = NULL;
    const char* t_Content = NULL;
    
    /*hardcoding the values for testing purpose*/
    pName = "synth";
    pClass = "Synthesizer";
    pMrcpProfile = "uni2";
     
    /*task params*/    
    t_Speak = true;
    t_SpeechLanguage = "en-UK";
    t_ContentType = "application/synthesis+ssml";
    //t_Content = "<?xml version=\"1.0\"?>\n<speak version=\"1.1\" xml:lang=\"en-US\" xmlns=\"http://www.w3.org/2001/10/synthesis\">\n  <p>\n    <s>Welcome to Bangalore.</s>\n  </p>\n</speak>";
    //t_Content = "<?xml version=\"1.1\"?>\n<speak version=\"1.1\" xml:lang=\"en-US\">\n  <p>\n    <s>Welcome to Bangalore.</s>\n  </p>\n</speak>";
    //t_Content = "<?xml version=\"1.0\"?>\n<speak version=\"1.1\" xml:lang=\"en-US\">\n  Welcome to Bangalore.\n  </speak>";
    //t_Content = "<?xml version=\"1.0\"?>\n<speak version=\"1.1\" xml:lang=\"en-US\">\n  Welcome to Bangalore.\n</speak>";
    //t_Content = "<speak version=\"1.1\" xml:lang=\"en-US\">\n  Welcome to Bangalore.\n</speak>";
    t_Content = "<?xml version=\"1.0\"?>\n<speak version=\"1.1\" xml:lang=\"en-UK\">\n    <voice name=\"Alice\">\n        <s>Welcome to Bangalore.</s>\n    </voice>\n</speak>";

  /*
    t_Content = "<?xml version=\"1.0\"?>\n<speak version=\"1.1\" xml:lang=\"en-UK\">\n    <voice name=\"Alice\">\n        <s>";
    t_Content += input_string; 
    t_Content += ".</s>\n    </voice>\n</speak>";
    */

    MrcpCliAppSynthScenario* pScenario = new MrcpCliAppSynthScenario();
    if(!pScenario) {
         XGLOG_ERROR("UniMrcpCliAppFw::ProcessSynthRequest create MrcpCliAppSynthScenario failed");
        return false;
    }
    
    pScenario->LoadSynth(t_Speak, t_SpeechLanguage, t_ContentType, t_Content, m_pPool);
    pScenario->SetDirLayout(m_pDirLayout);
	pScenario->SetName(pName);
	pScenario->SetMrcpProfile(pMrcpProfile);

	WrapMrcpSession* pSession = pScenario->CreateSession();
	if(!pSession) {
        XGLOG_ERROR("UniMrcpCliAppFw::ProcessSynthRequest create WrapMrcpSession failed");
        return false;
    }
	
    XGLOG_INFO("UniMrcpCliAppFw::ProcessSynthRequest Session id[%s]", pSession->GetId());
    
	if(pProfileName && *pProfileName != '\0')
		pSession->SetMrcpProfile(pProfileName);
    
	pSession->SetMrcpApplication(m_pMrcpApplication);
	pSession->SetMethodProvider(this);
	pSession->sessionId = m_tSessionDetails->m_callId.c_str();
	pSession->m_tSdpDescriptor = m_tSessionDetails;
	if(!pSession->Run())
	{
		delete pSession;
		return false;
	}

	AddSession(pSession);
	return true;
}

bool UniMrcpCliAppFw::ProcessSendSynthesisRequest(std::string text)
{
        XGLOG_FUNCTION_EN_EX("");
        WrapMrcpSession* session = NULL;
	UrMrcpSessionDetails *sessionInfo;
        session = GetSession(m_sessionId);
	std::map<std::string,UrMrcpSessionDetails *>::iterator it = UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.find(m_sessionId);
	if(it != UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.end() && it->second)
	{
		sessionInfo = it->second;
	}
	if(session && sessionInfo && !text.empty())
	{
		mrcp_client_session_t *pClientSession = (mrcp_client_session_t*)session->m_pMrcpSession;
		MrcpCliAppSynthSession *pSession = (MrcpCliAppSynthSession*)session;
		mrcp_channel_t *pChannel;
		if(pClientSession && pClientSession->channels)
		{
			for(int i=0; i<pClientSession->channels->nelts; i++)
			{
				pChannel = APR_ARRAY_IDX(pClientSession->channels,i,mrcp_channel_t*);
			}
			if(pChannel && session->m_pScenario)
			{
				std::string Content("");
				Content = session->FrameXml(text);
				session->m_pScenario->SetContent(Content.c_str());
				sessionInfo->m_sessionSynthState = EN_SESSION_SYNTH_START;
				pSession->SendSynthesisRequest(pChannel);
				return true;
			}
		}
	}
	return false;
}

bool UniMrcpCliAppFw::ProcessSynthTerminationReq()
{
        XGLOG_FUNCTION_EN_EX("");
        WrapMrcpSession* session = NULL;
        session = GetSession(m_sessionId);
	if(session && session->m_pMrcpSession)
	{
		session->Terminate();
	}
  else
  {
    XGLOG_ERROR("UniMrcpCliAppFw::ProcessSynthTerminationReq session not found for sessionId(%s) ", m_sessionId.c_str());
  }
	return true;
}

bool UniMrcpCliAppFw::ProcessSynthStopReq(std::string sessionId)
{
    XGLOG_FUNCTION_EN_EX("");
    WrapMrcpSession* session = NULL;
	if(sessionId.empty())
	{
		std::cout<<"!! SynthStop sessionId is empty" <<endl;
		XGLOG_ERROR("UniMrcpCliAppFw::ProcessSynthStopReq sessionId is empty!");
		return false;
	}
    session = GetSession(sessionId);
	std::map<std::string,UrMrcpSessionDetails *>::iterator it = UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.find(sessionId);
	if(session && session->m_pMrcpSession)
	{
		session->Stop();
		if(it != UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.end() && it->second)
		{
			it->second->m_sessionSynthState = EN_SESSION_SYNTH_STOP;
		}
	}
	return true;
}

/*
    utility function. use if needed.
*/
void UniMrcpCliAppFw::ProcessStopRequest(const char* id)
{
	XGLOG_FUNCTION_EN_EX("");
	WrapMrcpSession* pSession;
	void* pVal;
	apr_hash_index_t* it = apr_hash_first(m_pPool,m_pSessionTable);
	for(; it; it = apr_hash_next(it)) 
	{
		apr_hash_this(it,NULL,NULL,&pVal);
		pSession = (WrapMrcpSession*) pVal;
		if(pSession && strcasecmp(pSession->GetId(),id) == 0)
		{
			/* stop in-progress request */
			pSession->Stop();
			return;
		}
	}
}

/*
    utility function. use if needed.
*/
void UniMrcpCliAppFw::ProcessKillRequest(const char* id)
{
	XGLOG_FUNCTION_EN_EX("");
	WrapMrcpSession* pSession;
	void* pVal;
	apr_hash_index_t* it = apr_hash_first(m_pPool,m_pSessionTable);
	for(; it; it = apr_hash_next(it)) 
	{
		apr_hash_this(it,NULL,NULL,&pVal);
		pSession = (WrapMrcpSession*) pVal;
		if(pSession && strcasecmp(pSession->GetId(),id) == 0)
		{
			/* terminate session */
			pSession->Terminate();
			return;
		}
	}
}

/*
    utility function. use if needed.
*/
void UniMrcpCliAppFw::ProcessSessionExit(WrapMrcpSession* pUmcSession)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!pUmcSession)
		return;

	RemoveSession(pUmcSession);
	delete pUmcSession;
}

/*
    utility function. use if needed.
*/
void UniMrcpCliAppFw::StopSession(const char* id)
{
	XGLOG_FUNCTION_EN_EX("");
	apt_task_t* pTask = apt_consumer_task_base_get(m_pTask);
	apt_task_msg_t* pTaskMsg = apt_task_msg_get(pTask);
	if(!pTaskMsg) 
		return;

	pTaskMsg->type = TASK_MSG_USER;
	pTaskMsg->sub_type = UMC_TASK_STOP_SESSION_MSG;
	
	UmcTaskMsg* pUmcMsg = (UmcTaskMsg*) pTaskMsg->data;
	strncpy(pUmcMsg->m_SessionId,id,sizeof(pUmcMsg->m_SessionId)-1);
	pUmcMsg->m_pAppMessage = NULL;
	apt_task_msg_signal(pTask,pTaskMsg);
}
/*
    utility function. use if needed.
*/

void UniMrcpCliAppFw::ExitSession(WrapMrcpSession* pUmcSession)
{
	XGLOG_FUNCTION_EN_EX("");
	apt_task_t* pTask = apt_consumer_task_base_get(m_pTask);
	apt_task_msg_t* pTaskMsg = apt_task_msg_get(pTask);
	if(!pTaskMsg) 
		return;

	pTaskMsg->type = TASK_MSG_USER;
	pTaskMsg->sub_type = UMC_TASK_EXIT_SESSION_MSG;
	
	UmcTaskMsg* pUmcMsg = (UmcTaskMsg*) pTaskMsg->data;
	pUmcMsg->m_pSession = pUmcSession;
	apt_task_msg_signal(pTask,pTaskMsg);
}

bool UniMrcpCliAppFw::OnMrcpTtsRequestResponse(const mrcp_app_message_t* pMessage)
{
	XGLOG_FUNCTION_EN_EX("");
	UniMrcpCliAppFw* pFramework = (UniMrcpCliAppFw*) mrcp_application_object_get(pMessage->application);
	mrcp_client_session_t* pclientSession = (mrcp_client_session_t*)pMessage->session;
	if(pclientSession && pclientSession->answer)
	{
		switch(pclientSession->answer->response_code)
		{
			case 200:
				{
					XGLOG_INFO("Got %d response from Read Speaker Server for callid %s",pclientSession->answer->response_code,pMessage->session->name);
					WrapMrcpSession* pSession = (WrapMrcpSession*)apr_hash_get(pFramework->m_pSessionTable,pMessage->session->name,APR_HASH_KEY_STRING);
					if(pSession && pSession->m_pMrcpSession)
					{
						STR callID("");
						callID = pSession->m_pMrcpSession->name;
						auto it = UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.find(callID);
						if(it != UR_MRCP_MODULE_BRIDGE::instance()->m_sessionDetailMap.end() && it->second)
						{
							if(false==it->second->getIsReadSpeakerConnected())
							{
								it->second->setIsReadSpeakerConnected(true);
								xGateDbConfigMgrServiceMsg *mrcpServiceMsg = new xGateDbConfigMgrServiceMsg();
								mrcpServiceMsg->m_callId = pSession->m_pMrcpSession->name;
								//mrcpServiceMsg->m_tSessionDetails = pSession->m_tSdpDescriptor;
								mrcpServiceMsg->set_mrcp_command(EN_STT_TTS_MSG_ALLOCATE_RES);
								UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
							}
							else
							{
								XGLOG_INFO("UniMrcpCliAppFw::OnMrcpTtsRequestResponse already got 200 ok from ReadSpeaker hence Omitting callID[%s]",callID.c_str());
							}
						}
						else
						{
							XGLOG_INFO("UniMrcpCliAppFw::OnMrcpTtsRequestResponse SESSION not found callID[%s]",callID.c_str());
						}
					}
#if 0
					UrMrcpDbServiceMsg *dbServiceMsg = new UrMrcpDbServiceMsg();
					if(dbServiceMsg && pSession && pSession->m_tSdpDescriptor && !pSession->m_tSdpDescriptor->m_mrcpExtNum.empty())
					{
						dbServiceMsg->set_session_uid(pMessage->session->name);
						dbServiceMsg->set_session_speaker(pSession->m_tSdpDescriptor->m_mrcpExtNum.c_str());
						dbServiceMsg->set_display_text("reset");
						dbServiceMsg->set_type("join");
						dbServiceMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_REDIS_DB_SERVICE);
						UR_MRCP_MODULE_BRIDGE::instance()->handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(dbServiceMsg));
					}
#endif
					break;
				}
			default:
				{
					XGLOG_ERROR("Got %d response from Read Speaker Server callid %s, so sending terminating request",pclientSession->answer->response_code,pMessage->session->name);
					xGateDbConfigMgrServiceMsg *mrcpServiceMsg = new xGateDbConfigMgrServiceMsg();
					mrcpServiceMsg->m_tSessionId = pMessage->session->name;
					mrcpServiceMsg->m_disconnectCode = to_string(pclientSession->answer->response_code);
					mrcpServiceMsg->m_disconnectReason = "session not established with TTS server";
					mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_TERMINATE);
					UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
					break;
				}
		}
	}
	return true;
}
/*
    Below listed functions are framework supporting functions used for activies like allocation memory, releasing memory
    terminating session, etc, you modify only if require. 
*/
apt_bool_t AppMessageHandler(const mrcp_app_message_t* pMessage)
{
	XGLOG_FUNCTION_EN_EX("");
	UniMrcpCliAppFw* pFramework = (UniMrcpCliAppFw*) mrcp_application_object_get(pMessage->application);
	if(!pFramework)
		return FALSE;

	//Added code for disconnect the user when unable to connect with the speak server
	if(pMessage->sig_message.command_id == MRCP_SIG_COMMAND_CHANNEL_ADD)
	{
		pFramework->OnMrcpTtsRequestResponse(pMessage);
	}
	apt_task_t* pTask = apt_consumer_task_base_get(pFramework->m_pTask);
	apt_task_msg_t* pTaskMsg = apt_task_msg_get(pTask);
	if(pTaskMsg) 
	{
		pTaskMsg->type = TASK_MSG_USER;
		pTaskMsg->sub_type = UMC_TASK_CLIENT_MSG;

		UmcTaskMsg* pUmcMsg = (UmcTaskMsg*) pTaskMsg->data;
		pUmcMsg->m_pAppMessage = pMessage;
		apt_task_msg_signal(pTask,pTaskMsg);
	}

	return TRUE;
}

apt_bool_t AppOnSessionUpdate(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcSessionEventHandler* pEventHandler = (UmcSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnSessionUpdate(status);
}

apt_bool_t AppOnSessionTerminate(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcSessionEventHandler* pEventHandler = (UmcSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnSessionTerminate(status);
}

apt_bool_t AppOnChannelAdd(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_channel_t* pChannel, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcSessionEventHandler* pEventHandler = (UmcSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnChannelAdd(pChannel,status);
}

apt_bool_t AppOnChannelRemove(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_channel_t* pChannel, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcSessionEventHandler* pEventHandler = (UmcSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnChannelRemove(pChannel,status);
}

apt_bool_t AppOnMessageReceive(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_channel_t* pChannel, mrcp_message_t* pMessage)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcSessionEventHandler* pEventHandler = (UmcSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnMessageReceive(pChannel,pMessage);
}

apt_bool_t AppOnTerminateEvent(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_channel_t* pChannel)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcSessionEventHandler* pEventHandler = (UmcSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnTerminateEvent(pChannel);
}

apt_bool_t AppOnResourceDiscover(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_session_descriptor_t* pDescriptor, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcSessionEventHandler* pEventHandler = (UmcSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnResourceDiscover(pDescriptor,status);
}

void UmcOnStartComplete(apt_task_t* pTask)
{
	XGLOG_FUNCTION_EN_EX("");
	apt_consumer_task_t* pConsumerTask = (apt_consumer_task_t*) apt_task_object_get(pTask);
	UniMrcpCliAppFw* pFramework = (UniMrcpCliAppFw*) apt_consumer_task_object_get(pConsumerTask);
	
	pFramework->CreateMrcpClient();
	//pFramework->LoadScenarios();
}

void UmcOnTerminateComplete(apt_task_t* pTask)
{
	XGLOG_FUNCTION_EN_EX("");
	apt_consumer_task_t* pConsumerTask = (apt_consumer_task_t*) apt_task_object_get(pTask);
	UniMrcpCliAppFw* pFramework = (UniMrcpCliAppFw*) apt_consumer_task_object_get(pConsumerTask);

	pFramework->DestroyMrcpClient();
	//pFramework->DestroyScenarios();
}

apt_bool_t UmcProcessMsg(apt_task_t *pTask, apt_task_msg_t *pMsg)
{
	XGLOG_FUNCTION_EN_EX("");
	if(pMsg->type != TASK_MSG_USER)
		return FALSE;

	apt_consumer_task_t* pConsumerTask = (apt_consumer_task_t*) apt_task_object_get(pTask);
	UniMrcpCliAppFw* pFramework = (UniMrcpCliAppFw*) apt_consumer_task_object_get(pConsumerTask);
	UmcTaskMsg* pUmcMsg = (UmcTaskMsg*) pMsg->data;
	switch(pMsg->sub_type) 
	{
		case UMC_TASK_CLIENT_MSG:
		{
			static const mrcp_app_message_dispatcher_t applicationDispatcher = 
			{
				AppOnSessionUpdate,
				AppOnSessionTerminate,
				AppOnChannelAdd,
				AppOnChannelRemove,
				AppOnMessageReceive,
				AppOnTerminateEvent,
				AppOnResourceDiscover
			};

			mrcp_application_message_dispatch(&applicationDispatcher,pUmcMsg->m_pAppMessage);
			break;
		}
		case UMC_TASK_RUN_SESSION_MSG:
		{
			//pFramework->ProcessRunRequest();
			break;
		}
		case UMC_TASK_STOP_SESSION_MSG:
		{
			pFramework->ProcessStopRequest(pUmcMsg->m_SessionId);
			break;
		}
		case UMC_TASK_KILL_SESSION_MSG:
		{
			pFramework->ProcessKillRequest(pUmcMsg->m_SessionId);
			break;
		}
		case UMC_TASK_SHOW_SCENARIOS_MSG:
		{
			//pFramework->ProcessShowScenarios();
			break;
		}
		case UMC_TASK_SHOW_SESSIONS_MSG:
		{
		//	pFramework->ProcessShowSessions();
			break;
		}
		case UMC_TASK_EXIT_SESSION_MSG:
		{
			pFramework->ProcessSessionExit(pUmcMsg->m_pSession);
			break;
		}
	}
	return TRUE;
}
