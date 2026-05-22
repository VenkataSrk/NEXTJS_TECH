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

    Call UniMrcpCliAppFwSpeechRecog::ProcessSpeechRecogRequest() from upper layer to trigger STT scenario.

    Note: RTP is responsibility of MG. This client has RTP capability for testing purpose only
    To integrate STT server with MG,  configure MG and RTP port range under profiles "RTP-Settings-1"
    and "RTP-Factory-1" in configuration file "unimrcpclient.xml". MRCP client will share this information to 
    STT server during sdp and mrcp negotition process. 
    
*/


#include <apr_fnmatch.h>
#include "UniMrcpCliAppFwSpeechRecog.h"
#include "MrcpCliAppSpeechRecognizeScenario.h"
#include "unimrcp_client.h"
#include "apt_log.h"
#include "xglog.h"
#include "mrcp_client_session.h"
#include "mrcp_session_descriptor.h"
#include "URMRCPModuleBridge.h"
#include "mrcp_application.h"
#include "MrcpCliAppSpeechRecognizeSession.h"

typedef struct
{
	char                      m_SessionId[10];
	char                      m_ScenarioName[128];
	char                      m_ProfileName[128];
	const mrcp_app_message_t* m_pAppMessage;
	WrapSpeechRecogMrcpSession*               m_pSession;
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

apt_bool_t UmcRecogProcessMsg(apt_task_t* pTask, apt_task_msg_t* pMsg);
void UmcRecogOnStartComplete(apt_task_t* pTask);
void UmcRecogOnTerminateComplete(apt_task_t* pTask);
apt_bool_t AppRecogMessageHandler(const mrcp_app_message_t* pAppMessage);


UniMrcpCliAppFwSpeechRecog::UniMrcpCliAppFwSpeechRecog() :
	m_pPool(NULL),
	m_pDirLayout(NULL),
	m_pTask(NULL),
	m_pMrcpClient(NULL),
	m_pMrcpApplication(NULL),
	m_pSessionTable(NULL)
{
	XGLOG_FUNCTION_EN_EX("");
}

UniMrcpCliAppFwSpeechRecog::~UniMrcpCliAppFwSpeechRecog()
{
}

/*allocate process memory*/
bool UniMrcpCliAppFwSpeechRecog::Create(apt_dir_layout_t* pDirLayout, apr_pool_t* pool)
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

void UniMrcpCliAppFwSpeechRecog::Destroy()
{
	XGLOG_FUNCTION_EN_EX("");
	DestroyTask();

	//m_pScenarioTable = NULL;
	m_pSessionTable = NULL;
}

/*create tasks to be triggered on session initiation, processing msg and session termination */
bool UniMrcpCliAppFwSpeechRecog::CreateTask()
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
		pVtable->process_msg = UmcRecogProcessMsg;
		pVtable->on_start_complete = UmcRecogOnStartComplete;
		pVtable->on_terminate_complete = UmcRecogOnTerminateComplete;
	}
    /*UniMRCP lib start the task*/
	apt_task_start(pTask);
	return true;
}

void UniMrcpCliAppFwSpeechRecog::DestroyTask()
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


/*creating MRCP client to send/receive from MRCP server, to be called by 'UmcRecogOnStartComplete'*/
bool UniMrcpCliAppFwSpeechRecog::CreateMrcpClient()
{
	XGLOG_FUNCTION_EN_EX("");
	/* create MRCP client stack first */
	m_pMrcpClient = unimrcp_client_create(m_pDirLayout);
	if(!m_pMrcpClient)
		return false;

	/* create MRCP application to send/get requests to/from MRCP client stack */
	m_pMrcpApplication = mrcp_application_create(AppRecogMessageHandler,this,m_pPool);
	if(!m_pMrcpApplication)
	{
	    XGLOG_ERROR("UniMrcpCliAppFwSpeechRecog::CreateMrcpClient mrcp_application_create failed");
		mrcp_client_destroy(m_pMrcpClient);
		m_pMrcpClient = NULL;
		return false;
	}

	/* register MRCP application to MRCP client */
	mrcp_client_application_register(m_pMrcpClient,m_pMrcpApplication,"UMC");
	/* start MRCP client stack processing */
	if(mrcp_client_start(m_pMrcpClient) == FALSE)
	{
	    XGLOG_ERROR("UniMrcpCliAppFwSpeechRecog::CreateMrcpClient mrcp_client_start failed");
		mrcp_client_destroy(m_pMrcpClient);
		m_pMrcpClient = NULL;
		m_pMrcpApplication = NULL;
		return false;
	}
	return true;
}

void UniMrcpCliAppFwSpeechRecog::DestroyMrcpClient()
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

bool UniMrcpCliAppFwSpeechRecog::AddSession(WrapSpeechRecogMrcpSession* pSession)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!pSession)
		return false;

	//apr_hash_set(m_pSessionTable,pSession->GetId(),APR_HASH_KEY_STRING,pSession);
	apr_hash_set(m_pSessionTable,pSession->sessionId.c_str(),APR_HASH_KEY_STRING,pSession);
	return true;
}

WrapSpeechRecogMrcpSession* UniMrcpCliAppFwSpeechRecog::GetSession(std::string sessionName)
{
        WrapSpeechRecogMrcpSession* pSession = NULL;
        pSession = (WrapSpeechRecogMrcpSession*)apr_hash_get(m_pSessionTable,sessionName.c_str(),APR_HASH_KEY_STRING);
        return pSession;
}

bool UniMrcpCliAppFwSpeechRecog::RemoveSession(WrapSpeechRecogMrcpSession* pSession)
{
	XGLOG_FUNCTION_EN_EX("");
	if(!pSession)
		return false;

	apr_hash_set(m_pSessionTable,pSession->sessionId.c_str(),APR_HASH_KEY_STRING,NULL);
	return true;
}
/*
    Call UniMrcpCliAppFwSpeechRecog::ProcessSpeechRecogRequest() from upper layer to trigger STT scenario.

    Currently hardcoded values used to start the STT scenarios. 
    You can optimize this function to use the values from business layer, by passing as arguments

*/
bool UniMrcpCliAppFwSpeechRecog::ProcessSpeechRecogRequest()
{
	XGLOG_FUNCTION_EN_EX("");
	
    const char* pName = NULL;
    const char* pClass = NULL;
    const char* pMrcpProfile = NULL;
    const char* pProfileName = "uni2";
    
    /*Data needed for speech syntesis*/
    bool t_Recognize = false;
    const char* t_SpeechLanguage = NULL;
    const char* t_ContentType = NULL;
    const char* t_Content = NULL;
    const char* t_AudioSource = NULL;
    bool t_DefineGrammar = false;
    
    /*hardcoding the values for testing purpose*/
    pName = "recog";
    pClass = "Recognizer";
    pMrcpProfile = "uni2";
    t_SpeechLanguage = "en-US";
    t_Recognize = false;	
    t_DefineGrammar = false;
     
    /*Configure either grammar params or recognize params*/ 	

    /*------------------ Load Recognize ------------------*/
    t_Recognize = true;
    t_ContentType = "text/uri-list";
    //t_ContentType = "application/srgs+xml";
    //t_ContentType = "application/x-jsgf";
    //t_Content = "grammar digits;\npublic <numbers> = (one | two | three);";	
    //t_AudioSource = "one-8kHz.pcm";
    t_AudioSource = "demo-8kHz.pcm";
    //t_AudioSource = "johnsmith-8kHz.pcm";

    /*------------------ Load Grammer ------------------*/
    //Support JSGF 		
    /*
    t_DefineGrammar = true;
    t_ContentType = "application/x-jsgf";
    t_Content = "#JSGF V1.0;\ngrammar digits;\npublic <numbers> = (one | two | three);\n";
    */
            
    MrcpCliAppSpeechRecognizeScenario* pScenario = new MrcpCliAppSpeechRecognizeScenario();
    if(!pScenario) {
         XGLOG_ERROR("UniMrcpCliAppFwSpeechRecog::ProcessSpeechRecogRequest create MrcpCliAppSpeechRecognizeScenario failed");
        return false;
    }
    
    pScenario->LoadRecognize(t_Recognize, t_ContentType, t_Content, t_AudioSource, m_pPool);
    pScenario->LoadDefineGrammar(t_DefineGrammar, t_ContentType, t_Content);
   
    pScenario->SetDirLayout(m_pDirLayout);
	pScenario->SetName(pName);
	pScenario->SetMrcpProfile(pMrcpProfile);

	WrapSpeechRecogMrcpSession* pSession = pScenario->CreateSession();
	if(!pSession) {
        XGLOG_ERROR("UniMrcpCliAppFwSpeechRecog::ProcessSpeechRecogRequest create WrapSpeechRecogMrcpSession failed");
        return false;
    	}
    XGLOG_INFO("UniMrcpCliAppFwSpeechRecog::ProcessSpeechRecogRequest Session id[%s]", pSession->GetId());
    
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

bool UniMrcpCliAppFwSpeechRecog::ProcessStartRecogReq()
{
	XGLOG_FUNCTION_EN_EX("");
	WrapSpeechRecogMrcpSession* session = NULL;
	URMRCPModuleBridge *URModule = UR_MRCP_MODULE_BRIDGE::instance();
	session = GetSession(m_sessionId);
	if(session)
	{
		mrcp_client_session_t *pClientSession = (mrcp_client_session_t*)session->m_pMrcpSession;
		//	UmcRecogSessionEventHandler* pEventHandler = (UmcRecogSessionEventHandler*) mrcp_application_session_object_get(session->m_pMrcpSession);
		MrcpCliAppSpeechRecognizeSession *pSession = (MrcpCliAppSpeechRecognizeSession*)session;
		mrcp_channel_t *pChannel;
		for(int i=0; i<pClientSession->channels->nelts; i++)
		{
			pChannel = APR_ARRAY_IDX(pClientSession->channels,i,mrcp_channel_t*);
		}
		if(pSession->StartRecognition(pChannel))
		{
			std::map<std::string,UrMrcpSessionDetails *>::iterator it = URModule->m_sessionDetailMap.find(m_sessionId);
			if(it!=URModule->m_sessionDetailMap.end() && it->second)
			{
				it->second->m_sessionRecogState = EN_SESSION_RECOG_START;
			}
		}
#if 0
		if(pEventHandler->OnChannelAdd(pChannel,session->m_sessionStatus))
		{
			xGateDbConfigMgrServiceMsg *mrcpServiceMsg = new xGateDbConfigMgrServiceMsg();
			mrcpServiceMsg->m_callId = m_sessionId;
			mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_RECOGNITION_SENT);
			UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
		}
#endif

	}
	return true;
}

bool UniMrcpCliAppFwSpeechRecog::ProcessSendRecogReq()
{
  XGLOG_FUNCTION_EN_EX("");
  WrapSpeechRecogMrcpSession* session = NULL;
  session = GetSession(m_sessionId);
  if(session && session->isRecogCompleted)
  {
	  MrcpCliAppSpeechRecognizeSession *pSession = (MrcpCliAppSpeechRecognizeSession*)session;
	  mrcp_client_session_t *pClientSession = (mrcp_client_session_t*)session->m_pMrcpSession;
	  // UmcRecogSessionEventHandler* pEventHandler = (UmcRecogSessionEventHandler*) mrcp_application_session_object_get(session->m_pMrcpSession);
	  mrcp_channel_t *pChannel;
	  for(int i=0; i<pClientSession->channels->nelts; i++)
	  {
		  pChannel = APR_ARRAY_IDX(pClientSession->channels,i,mrcp_channel_t*);
	  }
	  pSession->StartRecognition(pChannel);
	  session->isRecogCompleted = false;
  }
  else if(session)
  {
	  session->isEOS = true;
  }
	  return true;
}

bool UniMrcpCliAppFwSpeechRecog::ProcessTerminationReq()
{
	XGLOG_FUNCTION_EN_EX("");
	XGLOG_INFO("processing UniMrcpCliAppFwSpeechRecog::ProcessTerminationReq for uid(%s) and sessionId(%s)",m_uid.c_str(),m_sessionId.c_str());
	WrapSpeechRecogMrcpSession* session = NULL;
#if 0
	std::string callId("");
	URMRCPModuleBridge *URModule = UR_MRCP_MODULE_BRIDGE::instance();
	std::multimap<std::string, std::string>::iterator itr = URModule->m_callDetailsMap.find(m_uid);
	for(;itr != URModule->m_callDetailsMap.end();itr = URModule->m_callDetailsMap.find(m_uid))
	{
		callId = itr->second;
		session = GetSession(callId); // get the session using callid
		if(session)
		{
			session->Terminate();
		}
		URModule->m_callDetailsMap.erase(itr);
		URModule->m_tMrcpSessionHandler.DeleteMrcpCallUid(callId);

		//ExitSession(session);
		//KillSession(m_sessionId.c_str());
	}
#endif
		session = GetSession(m_sessionId); // get the session using callid
		if(session)
		{
			session->Terminate();
		}
	return true;
}

bool UniMrcpCliAppFwSpeechRecog::ProcessRemoveParticipantReq()
{
	XGLOG_FUNCTION_EN_EX("");
	XGLOG_INFO("processing UniMrcpCliAppFwSpeechRecog::ProcessRemoveParticipantReq for uid(%s) and sessionId(%s)",m_uid.c_str(),m_sessionId.c_str());
	WrapSpeechRecogMrcpSession* session = NULL;
	std::string callId("");
	URMRCPModuleBridge *URModule = UR_MRCP_MODULE_BRIDGE::instance();
	std::multimap<std::string, std::string>::iterator itr = URModule->m_callDetailsMap.find(m_uid);
	for(;itr != URModule->m_callDetailsMap.end();itr++)
	{
		callId = itr->second;
		if(strncmp(callId.c_str(),m_sessionId.c_str(),callId.length()) == 0)
		{
			session = GetSession(callId); // get the session using callid
			if(session)
			{
				session->Terminate();
			}

			std::map<std::string,UrMrcpSessionDetails *>::iterator it = URModule->m_sessionDetailMap.find(callId);
			if(it!=URModule->m_sessionDetailMap.end())
			{
				URModule->m_sessionDetailMap.erase(callId);	
			}
			URModule->m_tMrcpSessionHandler.DeleteMrcpCallUid(callId);
			URModule->m_callDetailsMap.erase(itr);
			break;
		}
	}
	return true;
}

bool UniMrcpCliAppFwSpeechRecog::ProcessPauseRecogReq()
{
	XGLOG_FUNCTION_EN_EX("");
	UrMrcpSessionDetails *sessionInfo = NULL;
	URMRCPModuleBridge *URModule = UR_MRCP_MODULE_BRIDGE::instance();
	std::string user("");
	std::map<std::string,UrMrcpSessionDetails*>::iterator it = URModule->m_sessionDetailMap.find(m_sessionId);
	sessionInfo = it->second;
	user = sessionInfo->m_mrcpExtNum;
	std::map<std::string,std::list<std::string>>::iterator userList_itr = URModule->m_displayUserList.find(m_uid);
	if(userList_itr != URModule->m_displayUserList.end())
	{
		std::list<std::string>::iterator displayList_itr = userList_itr->second.begin();
		for(displayList_itr; displayList_itr != userList_itr->second.end(); displayList_itr++)
		{
			if(strstr((*displayList_itr).c_str(), user.c_str()))
			{
				userList_itr->second.erase(displayList_itr);
				sessionInfo->m_sessionRecogState = EN_SESSION_RECOG_PAUSE;
				sessionInfo->m_mrcpSttEnabled = 0;
				break;
			}
		}
	}
	return true;
}

bool UniMrcpCliAppFwSpeechRecog::ProcessResumeRecogReq()
{
	XGLOG_FUNCTION_EN_EX("");
	UrMrcpSessionDetails *sessionInfo = NULL;
	URMRCPModuleBridge *URModule = UR_MRCP_MODULE_BRIDGE::instance();
	std::map<std::string,UrMrcpSessionDetails*>::iterator it = URModule->m_sessionDetailMap.find(m_sessionId);
	if(it != URModule->m_sessionDetailMap.end() && it->second)
	{
		sessionInfo = it->second;
	}
	else 
	{
		XGLOG_ERROR("UniMrcpCliAppFwSpeechRecog::ProcessResumeRecogReq session details not found for sessionId(%s)",m_sessionId.c_str());
		return false;
	}
	if(sessionInfo->m_mrcpSttEnabled)
	{
		std::map<std::string,std::list<std::string>>::iterator userList_itr = URModule->m_displayUserList.find(m_uid);
		if(userList_itr != URModule->m_displayUserList.end())
		{
			std::string displayUser = URModule->frameDisplayInfo(sessionInfo->m_mrcpExtNum,sessionInfo->m_mrcpDomainId,sessionInfo->m_mrcpDeviceType);
			XGLOG_INFO("UniMrcpCliAppFwSpeechRecog::ProcessResumeRecogReq Added to display user list for sessionId(%s)",m_sessionId.c_str());
			userList_itr->second.push_back(displayUser);
		}
	}
	if(sessionInfo->m_sessionRecogState == EN_SESSION_RECOG_STOP)
	{
		WrapSpeechRecogMrcpSession* session = NULL;
		session = GetSession(m_sessionId);
		if(session)
		{
			MrcpCliAppSpeechRecognizeSession *pSession = (MrcpCliAppSpeechRecognizeSession*)session;
			mrcp_client_session_t *pClientSession = (mrcp_client_session_t*)session->m_pMrcpSession;
			mrcp_channel_t *pChannel;
			for(int i=0; i<pClientSession->channels->nelts; i++)
			{
				pChannel = APR_ARRAY_IDX(pClientSession->channels,i,mrcp_channel_t*);
			}
			XGLOG_INFO("UniMrcpCliAppFwSpeechRecog::ProcessResumeRecogReq sending Recognize request for sessionId(%s)",m_sessionId.c_str());
			pSession->StartRecognition(pChannel);
		}
	}
	sessionInfo->m_sessionRecogState = EN_SESSION_RECOG_RESUME;
	return true;
}

bool UniMrcpCliAppFwSpeechRecog::ProcessStopRecogReq()
{
	XGLOG_FUNCTION_EN_EX("");
	WrapSpeechRecogMrcpSession* session = NULL;
	URMRCPModuleBridge *URModule = UR_MRCP_MODULE_BRIDGE::instance();
	std::string user("");
	UrMrcpSessionDetails *sessionInfo = NULL;
	std::map<std::string,UrMrcpSessionDetails*>::iterator it = URModule->m_sessionDetailMap.find(m_sessionId);
	if(it != URModule->m_sessionDetailMap.end() && it->second)
	{
		it->second->m_sessionRecogState = EN_SESSION_RECOG_STOP;
	}

	session = GetSession(m_sessionId);
	if(session)
	{
		session->Stop();
	}
	else
	{
		XGLOG_ERROR("UniMrcpCliAppFwSpeechRecog::ProcessStopRecogReq Session not found for sessionId(%s)",m_sessionId.c_str());
		return false;
	}
#if 0
	std::map<std::string,UrMrcpSessionDetails*>::iterator it = URModule->m_sessionDetailMap.find(m_sessionId);
	if(it != URModule->m_sessionDetailMap.end() && it->second)
	{
		it->second->m_sessionRecogState = EN_SESSION_RECOG_STOP;
		it->second->m_mrcpSttEnabled = 0;
		user = it->second->m_mrcpExtNum;
		std::map<std::string,std::list<std::string>>::iterator userList_itr = URModule->m_displayUserList.find(m_uid);
		if(userList_itr != URModule->m_displayUserList.end())
		{
			std::list<std::string>::iterator displayList_itr = userList_itr->second.begin();
			for(displayList_itr; displayList_itr != userList_itr->second.end(); displayList_itr++)
			{
				if(strstr((*displayList_itr).c_str(), user.c_str()))
				{
					userList_itr->second.erase(displayList_itr);
					break;
				}
			}
		}
	}
#endif
	return true;
}

/*
    utility function. use if needed.
*/
void UniMrcpCliAppFwSpeechRecog::ProcessStopRequest(const char* id)
{
	XGLOG_FUNCTION_EN_EX("");
	WrapSpeechRecogMrcpSession* pSession;
	void* pVal;
	apr_hash_index_t* it = apr_hash_first(m_pPool,m_pSessionTable);
	for(; it; it = apr_hash_next(it)) 
	{
		apr_hash_this(it,NULL,NULL,&pVal);
		pSession = (WrapSpeechRecogMrcpSession*) pVal;
		if(pSession && (strcmp(pSession->sessionId.c_str(),id) == 0))
		{
			/* stop in-progress request */
			pSession->Stop();
			return;
		}
	}
}

void UniMrcpCliAppFwSpeechRecog::KillSession(const char* id)
{
        apt_task_t* pTask = apt_consumer_task_base_get(m_pTask);
        apt_task_msg_t* pTaskMsg = apt_task_msg_get(pTask);
        if(!pTaskMsg)
                return;

        pTaskMsg->type = TASK_MSG_USER;
        pTaskMsg->sub_type = UMC_TASK_KILL_SESSION_MSG;

        UmcTaskMsg* pUmcMsg = (UmcTaskMsg*) pTaskMsg->data;
        strncpy(pUmcMsg->m_SessionId,id,sizeof(pUmcMsg->m_SessionId)-1);
        pUmcMsg->m_pAppMessage = NULL;
        apt_task_msg_signal(pTask,pTaskMsg);
}

/*
    utility function. use if needed.
*/

void UniMrcpCliAppFwSpeechRecog::ProcessKillRequest(const char* id)
{
	XGLOG_FUNCTION_EN_EX("");
	WrapSpeechRecogMrcpSession* pSession;
	void* pVal;
	apr_hash_index_t* it = apr_hash_first(m_pPool,m_pSessionTable);
	for(; it; it = apr_hash_next(it)) 
	{
		apr_hash_this(it,NULL,NULL,&pVal);
		pSession = (WrapSpeechRecogMrcpSession*) pVal;
		if(pSession)
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
void UniMrcpCliAppFwSpeechRecog::ProcessSessionExit(WrapSpeechRecogMrcpSession* pUmcSession)
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

void UniMrcpCliAppFwSpeechRecog::StopSession(const char* id)
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

void UniMrcpCliAppFwSpeechRecog::ExitSession(WrapSpeechRecogMrcpSession* pUmcSession)
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

bool UniMrcpCliAppFwSpeechRecog::OnMrcpRequestResponse(const mrcp_app_message_t* pMessage)
{
	mrcp_client_session_t* pclientSession = (mrcp_client_session_t*)pMessage->session;
	UniMrcpCliAppFwSpeechRecog* pFramework = (UniMrcpCliAppFwSpeechRecog*) mrcp_application_object_get(pMessage->application);
	if(!pFramework)
		return FALSE;
	//WrapSpeechRecogMrcpSession* pSession = (WrapSpeechRecogMrcpSession*)apr_hash_get(pFramework->m_pSessionTable,pFramework->m_SessionId,APR_HASH_KEY_STRING);
	WrapSpeechRecogMrcpSession* pSession = (WrapSpeechRecogMrcpSession*)apr_hash_get(pFramework->m_pSessionTable,pMessage->session->name,APR_HASH_KEY_STRING);
	if(pclientSession && pclientSession->answer && pclientSession->answer->audio_media_arr)
	{
		apr_size_t count = pclientSession->answer->audio_media_arr->nelts;
		switch(pclientSession->answer->response_code)
		{
			case 200:
				{
					mpf_rtp_media_descriptor_t* media;
					mpf_codec_descriptor_t* codec_info;
					pSession->m_sessionStatus = pMessage->sig_message.status;
					for(int i=0; i<count; i++)
					{
						media = (mpf_rtp_media_descriptor_t*) mrcp_session_audio_media_get(pclientSession->answer, i);
						if(media)
						{
							for(int i=0;i<media->codec_list.descriptor_arr->nelts;i++)
							{
								codec_info = mpf_codec_list_descriptor_get(&media->codec_list,i);
								if(codec_info)
								{
									pSession->m_tSdpDescriptor->m_mrcpCodec = codec_info->payload_type;
									pSession->m_tSdpDescriptor->m_mrcpCodecName = apt_string_buffer_get(&codec_info->name);
									stringstream mediaPort;
									mediaPort << media->port;
									pSession->m_tSdpDescriptor->m_mrcpServerPort = mediaPort.str();
									pSession->m_tSdpDescriptor->m_mrcpServerIp = apt_string_buffer_get(&media->ip);
									pSession->m_tSdpDescriptor->m_mrcpPtime = media->ptime;
									break;
								}
							}
#if 0

							pSession->m_pMedia_info.severip = apt_string_buffer_get(&media->ip);
							pSession->m_pMedia_info.port = media->port;
							pSession->m_pMedia_info.codec_name = apt_string_buffer_get(&codec_info->name);
							pSession->m_pMedia_info.payload = codec_info->payload_type;
							pSession->m_pMedia_info.sampling_rate = codec_info->sampling_rate;
							pSession->m_pMedia_info.codec_name = apt_string_buffer_get(&media->codec_list.primary_descriptor->name);
							pSession->m_pMedia_info.payload = media->codec_list.primary_descriptor->payload_type;
							pSession->m_pMedia_info.sampling_rate = media->codec_list.primary_descriptor->sampling_rate;
#endif
						}
					}

					xGateDbConfigMgrServiceMsg *mrcpServiceMsg = new xGateDbConfigMgrServiceMsg();
					mrcpServiceMsg->m_tSessionId = pSession->m_pMrcpSession->name;
					mrcpServiceMsg->m_tSessionDetails = pSession->m_tSdpDescriptor;
					//mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_PORT_ALLOCATE_RES); //commented for code clean up - SGN
					UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
					break;
				}
			default:
				{
					XGLOG_ERROR("Got %d response from Voicegain Server callid %s",pclientSession->answer->response_code,pMessage->session->name);
					xGateDbConfigMgrServiceMsg *mrcpServiceMsg = new xGateDbConfigMgrServiceMsg();
					mrcpServiceMsg->m_tSessionId = pSession->m_pMrcpSession->name;
					mrcpServiceMsg->m_disconnectCode = to_string(pclientSession->answer->response_code);
					mrcpServiceMsg->m_disconnectReason = "session not established with STT server";
					mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_TERMINATE);
					UR_MRCP_MODULE_BRIDGE::instance()->PostMessageToMrcpMg(mrcpServiceMsg);
					break;
				}
		}
	}
return false;
}

/*
    Below listed functions are framework supporting functions used for activies like allocation memory, releasing memory
    terminating session, etc, you modify only if require. 
*/
apt_bool_t AppRecogMessageHandler(const mrcp_app_message_t* pMessage)
{
	XGLOG_FUNCTION_EN_EX("");
	UniMrcpCliAppFwSpeechRecog* pFramework = (UniMrcpCliAppFwSpeechRecog*) mrcp_application_object_get(pMessage->application);
	if(!pFramework)
		return FALSE;
        //pFramework->OnMrcpRequestResponse(pMessage);
#if 0
        if((int)pMessage->sig_message.message_type == 1)
        {
                mrcp_client_session_t* pclientSession = (mrcp_client_session_t*)pMessage->session;
                if(pclientSession->answer->response_code = 200)
                {
                        pFramework->OnMrcpRequestResponse(pMessage);
                }
                else
                {
                        return FALSE;
                }
        }
	else
		return FALSE;
#endif
  if(pMessage->sig_message.command_id == MRCP_SIG_COMMAND_CHANNEL_ADD)
  {
    pFramework->OnMrcpRequestResponse(pMessage);
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
	return FALSE;
}

apt_bool_t AppRecogOnSessionUpdate(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcRecogSessionEventHandler* pEventHandler = (UmcRecogSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnSessionUpdate(status);
}

apt_bool_t AppRecogOnSessionTerminate(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcRecogSessionEventHandler* pEventHandler = (UmcRecogSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnSessionTerminate(status);
}

apt_bool_t AppRecogOnChannelAdd(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_channel_t* pChannel, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcRecogSessionEventHandler* pEventHandler = (UmcRecogSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnChannelAdd(pChannel,status);
}

apt_bool_t AppRecogOnChannelRemove(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_channel_t* pChannel, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcRecogSessionEventHandler* pEventHandler = (UmcRecogSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnChannelRemove(pChannel,status);
}

apt_bool_t AppRecogOnMessageReceive(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_channel_t* pChannel, mrcp_message_t* pMessage)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcRecogSessionEventHandler* pEventHandler = (UmcRecogSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnMessageReceive(pChannel,pMessage);
}

apt_bool_t AppRecogOnTerminateEvent(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_channel_t* pChannel)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcRecogSessionEventHandler* pEventHandler = (UmcRecogSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnTerminateEvent(pChannel);
}

apt_bool_t AppRecogOnResourceDiscover(mrcp_application_t* pApplication, mrcp_session_t* pSession, mrcp_session_descriptor_t* pDescriptor, mrcp_sig_status_code_e status)
{
	XGLOG_FUNCTION_EN_EX("");
	UmcRecogSessionEventHandler* pEventHandler = (UmcRecogSessionEventHandler*) mrcp_application_session_object_get(pSession);
	return pEventHandler->OnResourceDiscover(pDescriptor,status);
}

void UmcRecogOnStartComplete(apt_task_t* pTask)
{
	XGLOG_FUNCTION_EN_EX("");
	apt_consumer_task_t* pConsumerTask = (apt_consumer_task_t*) apt_task_object_get(pTask);
	UniMrcpCliAppFwSpeechRecog* pFramework = (UniMrcpCliAppFwSpeechRecog*) apt_consumer_task_object_get(pConsumerTask);
	
	pFramework->CreateMrcpClient();
	//pFramework->LoadScenarios();
}

void UmcRecogOnTerminateComplete(apt_task_t* pTask)
{
	XGLOG_FUNCTION_EN_EX("");
	apt_consumer_task_t* pConsumerTask = (apt_consumer_task_t*) apt_task_object_get(pTask);
	UniMrcpCliAppFwSpeechRecog* pFramework = (UniMrcpCliAppFwSpeechRecog*) apt_consumer_task_object_get(pConsumerTask);

	pFramework->DestroyMrcpClient();
	//pFramework->DestroyScenarios();
}

apt_bool_t UmcRecogProcessMsg(apt_task_t *pTask, apt_task_msg_t *pMsg)
{
	XGLOG_FUNCTION_EN_EX("");
	if(pMsg->type != TASK_MSG_USER)
		return FALSE;

	apt_consumer_task_t* pConsumerTask = (apt_consumer_task_t*) apt_task_object_get(pTask);
	UniMrcpCliAppFwSpeechRecog* pFramework = (UniMrcpCliAppFwSpeechRecog*) apt_consumer_task_object_get(pConsumerTask);
	UmcTaskMsg* pUmcMsg = (UmcTaskMsg*) pMsg->data;
	switch(pMsg->sub_type) 
	{
		case UMC_TASK_CLIENT_MSG:
		{
			static const mrcp_app_message_dispatcher_t applicationDispatcher = 
			{
				AppRecogOnSessionUpdate,
				AppRecogOnSessionTerminate,
				AppRecogOnChannelAdd,
				AppRecogOnChannelRemove,
				AppRecogOnMessageReceive,
				AppRecogOnTerminateEvent,
				AppRecogOnResourceDiscover
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
