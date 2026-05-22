#include "xglog.h"
#include "xGateMrcpCliAppService.h"
#include "xGateMrcpCliAppHandler.h"
#include "URMRCPModuleBridge.h"
//#include "ReadConfigFromProxyConfFile.h"

#include <apr_getopt.h>
#include "apt_pool.h"
#include "uni_revision.h"

//const char* t_RootDirPath = "../conf";
//const char* t_DirLayoutConf = "../conf"; 

xGateMrcpCliAppHandler::xGateMrcpCliAppHandler() : m_pFramework(NULL)
{
	XGLOG_FUNCTION_EN_EX("");
	m_pFramework = new UniMrcpCliAppFw;
	m_pFrameworkRecog = new UniMrcpCliAppFwSpeechRecog;
	m_Options.m_RootDirPath = NULL; 
	m_Options.m_DirLayoutConf = NULL;
	m_Options.m_LogPriority = NULL;
	m_Options.m_LogOutput = NULL;
	m_pool = NULL;
}

xGateMrcpCliAppHandler::~xGateMrcpCliAppHandler()
{
	XGLOG_FUNCTION_EN_EX("");
	/* destroy demo framework */
	m_pFramework->Destroy();
	m_pFrameworkRecog->Destroy();
       /* destroy singleton logger */
        apt_log_instance_destroy();
        /* destroy APR pool */
        apr_pool_destroy(m_pool);
        /* APR global termination */
        apr_terminate();

	delete m_pFramework;
	delete m_pFrameworkRecog;
}

#if 0
bool xGateMrcpCliAppHandler::checkConfigPath() 
{
  char* configPath;
  configPath = getenv(ENV_BASE);

  if(configPath) {
      XGLOG_INFO("xGateMrcpCliAppHandler::checkConfigPath UniMrcpCliApp will read 'conf' folder from path '%s'", configPath);
      strcat (configPath, "/conf");	
      m_Options.m_RootDirPath = configPath;
      m_Options.m_DirLayoutConf = configPath;

  } else {
    XGLOG_ERROR("xGateMrcpCliAppHandler::checkConfigPath 'URENGINE_BASE' environment variable is missing !!");
    return false;
  }

  return true;
}
#endif

bool xGateMrcpCliAppHandler::init()
{
	XGLOG_FUNCTION_EN_EX("");
	apt_dir_layout_t* pDirLayout = NULL;
	apt_dir_layout_t* pTtsDirLayout = NULL;
	const char *logConfPath;
	const char *logPrefix = "unimrcpclient";	

	/* APR global initialization */
	if(apr_initialize() != APR_SUCCESS) 
	{
		XGLOG_ERROR("xGateMrcpCliAppHandler::init() APR global initialization failed");
		apr_terminate();
		return false;
	}

	/* create APR pool */
	m_pool = apt_pool_create();
	if(!m_pool) 
	{
		XGLOG_ERROR("xGateMrcpCliAppHandler::init() create APR pool failed");
		apr_terminate();
		return false;
	}


	if(m_Options.m_DirLayoutConf)
	{
		/* create and load directories layout from the configuration file */
		XGLOG_INFO("xGateMrcpCliAppHandler::init() create and load directories layout from the configuration file");
		pDirLayout = apt_dir_layout_create(m_pool);
		if(pDirLayout)
			apt_dir_layout_load(pDirLayout,m_Options.m_DirLayoutConf,m_pool);
	}
	else
	{
		m_Options.m_RootDirPath = MRCP_CONFIG::instance()->m_sttProfile.m_mrcp_xml_path.c_str();
		/* create default directories layout */
		XGLOG_INFO("xGateMrcpCliAppHandler::init() create default directories layout");
		pDirLayout = apt_default_dir_layout_create(m_Options.m_RootDirPath,m_pool);
    m_Options.m_RootDirPath = MRCP_CONFIG::instance()->m_ttsProfile.m_mrcp_xml_path.c_str();
    pTtsDirLayout = apt_default_dir_layout_create(m_Options.m_RootDirPath,m_pool);
	}

	if(!pDirLayout || !pTtsDirLayout)
	{
		XGLOG_ERROR("xGateMrcpCliAppHandler::init() Failed to Create Directories Layout for stt or tts");
		printf("Failed to Create Directories Layout\n");
		apr_pool_destroy(m_pool);
		apr_terminate();
		return false;
	}

		/* get path to logger configuration file */
	logConfPath = apt_confdir_filepath_get(pDirLayout,"logger.xml",m_pool);
	/* create and load singleton logger */
	apt_log_instance_load(logConfPath,m_pool);

	if(m_Options.m_LogPriority) 
	{
		/* override the log priority, if specified in command line */
		XGLOG_INFO("xGateMrcpCliAppHandler::init() override the log priority, if specified in command line");
		apt_log_priority_set((apt_log_priority_e)atoi(m_Options.m_LogPriority));
	}
	if(m_Options.m_LogOutput) 
	{
		/* override the log output mode, if specified in command line */
		XGLOG_INFO("xGateMrcpCliAppHandler::init() override the log output mode, if specified in command line");
		apt_log_output_mode_set((apt_log_output_e)atoi(m_Options.m_LogOutput));
	}

	if(apt_log_output_mode_check(APT_LOG_OUTPUT_FILE) == TRUE) 
	{
		/* open the log file */
		XGLOG_INFO("xGateMrcpCliAppHandler::init() open the log file");
		const char *logDirPath = apt_dir_layout_path_get(pDirLayout,APT_LAYOUT_LOG_DIR);
		const char *logfileConfPath = apt_confdir_filepath_get(pDirLayout,"logfile.xml",m_pool);
		apt_log_file_open_ex(logDirPath,logPrefix,logfileConfPath,m_pool);
	}

	if(apt_log_output_mode_check(APT_LOG_OUTPUT_SYSLOG) == TRUE)
	{
		/* open the syslog */
		XGLOG_INFO("xGateMrcpCliAppHandler::init() open the syslog file");
		const char *logfileConfPath = apt_confdir_filepath_get(pDirLayout,"syslog.xml",m_pool);
		apt_syslog_open(logPrefix,logfileConfPath,m_pool);
	}

	/* create MRCP client framework for URIPPBX is successs*/
	

	if(m_pFramework->Create(pTtsDirLayout,m_pool))
	{
		XGLOG_INFO("xGateMrcpCliAppHandler::init() create MRCP client STT for URIPPBX is successs");
		if(m_pFrameworkRecog->Create(pDirLayout,m_pool))
		{
			XGLOG_INFO("xGateMrcpCliAppHandler::init() create MRCP client TTS for URIPPBX is successs");
			return true;
		}
		else
		{
			XGLOG_INFO("xGateMrcpCliAppHandler::init() create MRCP client framework for URIPPBX is failed ");
			return false;
			
		}
	}
	else
	{
		XGLOG_INFO("xGateMrcpCliAppHandler::init() create MRCP client STT for URIPPBX is failed");
		return false;
	}

}

bool xGateMrcpCliAppHandler::process_msg(xGateDbConfigMgrServiceMsg *mrcpServiceMsg)
{
  if(!mrcpServiceMsg)
  {
    XGLOG_ERROR("xGateMrcpCliAppHandler::process_msg received invalid msg");
    return false;
  }
  XGLOG_INFO("xGateMrcpCliAppHandler::process_msg func called for uId (%s)",mrcpServiceMsg->m_callId.c_str());

  std::string callid = mrcpServiceMsg->m_callId.c_str();
  UrMrcpSessionDetails *sessionInfo = NULL;
  std::map<std::string,UrMrcpSessionDetails *>::iterator it = URMRCPMODULEBRIDGE->m_sessionDetailMap.find(callid);
  if(it != URMRCPMODULEBRIDGE->m_sessionDetailMap.end() && it->second)
  {
    sessionInfo = it->second;
  }
  else
  {
    XGLOG_ERROR("xGateMrcpCliAppHandler::process_msg session info not found for callid(%s)",(char*)ur_log_string(callid));
    return false;
  }


	switch(mrcpServiceMsg->get_mrcp_command())
	{
  #if 0
		case EN_MRCP_MSG_PORT_ALLOCATE_REQ:
		{
			m_pFrameworkRecog->m_tSessionDetails = mrcpServiceMsg->m_tSessionDetails;
			m_pFrameworkRecog->ProcessSpeechRecogRequest();
			m_pFramework->m_tSessionDetails = mrcpServiceMsg->m_tSessionDetails;
			m_pFramework->ProcessSynthRequest();
			break;
		}
		case EN_MRCP_MSG_RECOGNITION_START_REQ:
		{
			m_pFrameworkRecog->m_sessionId = mrcpServiceMsg->m_callId.c_str();
//			m_pFrameworkRecog->m_uid = mrcpServiceMsg->m_uid.c_str();
			m_pFrameworkRecog->ProcessStartRecogReq();
			break;
		}
		case EN_MRCP_MSG_EOS:
		{
			m_pFrameworkRecog->m_sessionId = mrcpServiceMsg->m_tSessionId;
			m_pFrameworkRecog->m_uid = mrcpServiceMsg->m_uid.c_str();
			m_pFrameworkRecog->ProcessSendRecogReq();
			break;
		}
		case EN_MRCP_MSG_RECOGNITION_PAUSE:
		{
			m_pFrameworkRecog->m_sessionId = mrcpServiceMsg->m_tSessionId;
			m_pFrameworkRecog->m_uid = mrcpServiceMsg->m_uid.c_str();
			m_pFrameworkRecog->ProcessPauseRecogReq();
			break;
		}
		case EN_MRCP_MSG_RECOGNITION_RESUME:
		{
			m_pFrameworkRecog->m_sessionId = mrcpServiceMsg->m_tSessionId;
			m_pFrameworkRecog->m_uid = mrcpServiceMsg->m_uid.c_str();
			m_pFrameworkRecog->ProcessResumeRecogReq();
			break;
		}
		case EN_MRCP_MSG_RECOGNITION_STOP:
		{
			m_pFrameworkRecog->m_sessionId = mrcpServiceMsg->m_tSessionId;
			m_pFrameworkRecog->m_uid = mrcpServiceMsg->m_uid.c_str();
			m_pFrameworkRecog->ProcessStopRecogReq();
			break;
		}
		case EN_MRCP_MSG_RELEASE:
                {
			m_pFrameworkRecog->m_sessionId = mrcpServiceMsg->m_tSessionId;
			m_pFrameworkRecog->m_uid = mrcpServiceMsg->m_uid.c_str();
			m_pFrameworkRecog->ProcessTerminationReq();
			m_pFramework->m_sessionId = mrcpServiceMsg->m_tSessionId;
			m_pFramework->ProcessSynthTerminationReq();
			break;
                }
		case EN_MRCP_MSG_REMOVE_PARTICIPANT_REQ:
		{
			m_pFrameworkRecog->m_sessionId = mrcpServiceMsg->m_tSessionId;
			m_pFrameworkRecog->m_uid = mrcpServiceMsg->m_uid.c_str();
			m_pFrameworkRecog->ProcessRemoveParticipantReq();
			break;
		}
  #endif
    case EN_MSG_PORT_ALLOCATE_REQ:
    case EN_MRCP_MSG_ALLOCATE_STT_TTS:
    {
      switch(sessionInfo->m_ttsType)
      {
        case EN_XGATE_TTS_READ_SPEAKER:
        {
          m_pFramework->m_tSessionDetails = mrcpServiceMsg->m_tSessionDetails;
          m_pFramework->ProcessSynthRequest();
          break;
        }
        case EN_XGATE_TTS_INHOUSE:
        {
          if(mrcpServiceMsg->m_tSessionDetails)
          {
            std::string callid = mrcpServiceMsg->m_tSessionDetails->m_callId;
            std::string uid = mrcpServiceMsg->m_tSessionDetails->m_mgResourceId;
            xGateInhouseTtsClient *clientSession = new xGateInhouseTtsClient(callid,uid,mrcpServiceMsg->m_tSessionDetails);
            if(clientSession)
            {
              m_mapClientSession.insert({callid,clientSession});
            }
          }
          break;
        }
        default:
        {
          XGLOG_ERROR("xGateMrcpCliAppHandler::process_msg unknown tts type for callid(%s)",(char*)ur_log_string(callid));
        }
      }
    }
    break;
    case EN_MRCP_MSG_SYNTHESIZE_REQ:
    {
      switch(sessionInfo->m_ttsType)
      {
        case EN_XGATE_TTS_READ_SPEAKER:
        {
          m_pFramework->m_sessionId = mrcpServiceMsg->m_callId.c_str();
          m_pFramework->ProcessSendSynthesisRequest(mrcpServiceMsg->m_displayText);
          break;
        } 
        case EN_XGATE_TTS_INHOUSE:
        {
          std::map<std::string, xGateInhouseTtsClient*>::iterator itr = m_mapClientSession.find(mrcpServiceMsg->m_callId);
          if(itr != m_mapClientSession.end() && itr->second)
          {
            if(itr->second->send_synth_request(mrcpServiceMsg->m_displayText.c_str()))
            {
              sessionInfo->m_sessionSynthState = EN_SESSION_SYNTH_START;
            }
          }
          break;
        }
        default:
        {
          XGLOG_ERROR("xGateMrcpCliAppHandler::process_msg unknown tts type for callid(%s)",(char*)ur_log_string(callid));
        }
      }
      break;
    }
    case EN_MRCP_MSG_SYNTHESIZE_STOP:
    {
      switch(sessionInfo->m_ttsType)
      {
        case EN_XGATE_TTS_READ_SPEAKER:
        {
          XGLOG_INFO("xGateMrcpCliAppHandler::process_msg 'EN_MRCP_MSG_SYNTHESIZE_STOP' for callid(%s)",(char*)ur_log_string(callid));
          m_pFramework->m_sessionId = mrcpServiceMsg->m_tSessionId.c_str();
          m_pFramework->ProcessSynthStopReq(callid);
          break;
        } 
        case EN_XGATE_TTS_INHOUSE:
        {
          std::map<std::string, xGateInhouseTtsClient*>::iterator itr = m_mapClientSession.find(mrcpServiceMsg->m_callId);
          if(itr != m_mapClientSession.end() && itr->second)
          {
            itr->second->stop_synth_request();
          }
          break;
        }
        default:
        {
          XGLOG_ERROR("xGateMrcpCliAppHandler::process_msg unknown tts type for callid(%s)",(char*)ur_log_string(callid));
        }
      }
      break;
    }
    case EN_MSG_RELEASE:
    case EN_STT_REMOVE_PARTICIPANT:
    {
      switch(sessionInfo->m_ttsType)
      {
        case EN_XGATE_TTS_READ_SPEAKER:
        {
            XGLOG_INFO("xGateMrcpCliAppHandler::process_msg 'EN_MSG_RELEASE' for TTS_READ_SPEAKER callid(%s)",
                                                 (char*)ur_log_string(callid));
            m_pFramework->m_sessionId = mrcpServiceMsg->m_tSessionId;
            m_pFramework->ProcessSynthTerminationReq();
            break;
        } 
        case EN_XGATE_TTS_INHOUSE:
        {
            XGLOG_INFO("xGateMrcpCliAppHandler::process_msg 'EN_MSG_RELEASE' for TTS_INHOUSE callid(%s)",
                                             (char*)ur_log_string(callid));
          std::map<std::string, xGateInhouseTtsClient*>::iterator itr = m_mapClientSession.find(mrcpServiceMsg->m_callId);
          if(itr != m_mapClientSession.end() && itr->second)
          {
            delete itr->second;
            itr->second = NULL;
            m_mapClientSession.erase(itr);
          }
          break;
        }
        default:
        {
          XGLOG_ERROR("xGateMrcpCliAppHandler::process_msg unknown tts type for callid(%s)",(char*)ur_log_string(callid));
        }
      }
      /* xGateDbConfigMgrServiceMsg *pServiceMsg = new xGateDbConfigMgrServiceMsg();
      pServiceMsg->m_callId = callid.c_str();
      pServiceMsg->set_mrcp_command(EN_MRCP_MSG_DELETE_SESSIONINFO);
      URMRCPMODULEBRIDGE->PostMessageToMrcpMg(pServiceMsg); */
    }
    break;
    default:
    XGLOG_INFO("xGateMrcpCliAppHandler::process_msg received unknown command for callid(%s)", (char*)ur_log_string(mrcpServiceMsg->m_callId));
    break;
  }
  XGLOG_INFO("xGateMrcpCliAppHandler Exiting process_msg ()");
  return true;
}

#define EN_UMC_TASK_RUN_SESSION_TEXT_TO_SPEECH 10
#define EN_UMC_TASK_RUN_SESSION_SPEECH_TO_TEXT 11
#define EN_UMC_TASK_CLIENT_MSG                 12   
#define EN_UMC_TASK_STOP_SESSION_MSG           13
#define EN_UMC_TASK_KILL_SESSION_MSG           14
#define EN_UMC_TASK_EXIT_SESSION_MSG           15
#define EN_UMC_UNKNOWN                         16


bool xGateMrcpCliAppHandler::process_msg_from_controller(URMRCPBaseMsg *pBaseMsg)
{
	XGLOG_FUNCTION_EN_EX("");
   XGLOG_INFO("In xGateMrcpCliAppHandler::process_msg_from_controller");
   //xGatePbxServiceMsg *pregMsg = dynamic_cast<xGatePbxServiceMsg*>(pBaseMsg);

   if( pBaseMsg == NULL ) {
      XGLOG_ERROR("handle_sip_message recvd invalid Message class");
//      return false;
   }

   const char* pProfileName = NULL; 	
   const char* pScenarioName = NULL; 
   char* pID = NULL;

   switch(10)
   {
      case EN_UMC_TASK_RUN_SESSION_TEXT_TO_SPEECH:
         {
            XGLOG_INFO("process_msg_from_controller handling 'EN_UMC_TASK_RUN_SESSION_MSG'");	    
	      //  m_pFramework->ProcessSynthRequest();
            m_pFrameworkRecog->ProcessSpeechRecogRequest();
            break;
         }
      case EN_UMC_TASK_RUN_SESSION_SPEECH_TO_TEXT:
         {
            XGLOG_INFO("process_msg_from_controller handling 'EN_UMC_TASK_RUN_SESSION_MSG'");
	    pProfileName = ""; 	
            pScenarioName = ""; 
	            /*need to add functions*/

	   //  m_pFrameworkRecog->ProcessSpeechRecogRequest();
            break;
         }
      case EN_UMC_TASK_CLIENT_MSG:
         {
            XGLOG_INFO("process_msg_from_controller handling 'EN_UMC_TASK_CLIENT_MSG' ");
            break;
         }
      case EN_UMC_TASK_STOP_SESSION_MSG:
         {
            XGLOG_INFO("process_msg_from_controller handling 'EN_UMC_TASK_STOP_SESSION_MSG' ");
		pID = NULL;
                if(pID)
                {
                        m_pFramework->StopSession(pID);
                }

            break;
         }
      case EN_UMC_TASK_KILL_SESSION_MSG:
         {
            XGLOG_INFO("process_msg_from_controller handling 'EN_UMC_TASK_KILL_SESSION_MSG' ");
                pID = NULL;
                if(pID)
                {
                       // m_pFramework->KillSession(pID);
                }

            break;
         }
      case EN_UMC_TASK_EXIT_SESSION_MSG:
         {
            XGLOG_INFO("process_msg_from_controller handling 'EN_UMC_TASK_EXIT_SESSION_MSG' ");
            break;
         }
      case EN_UMC_UNKNOWN:
      default:
         {
            XGLOG_ERROR("handle_sip_message failed to handle Sip message!");
            break;
         }
   }

   return true;
}
