#include "URMRCPModuleBridge.h"
#include "MrcpRedisDbConnector.h"
#include "MrcpRedisDbSubscriber.h"
#include "xglog.h"
#include "xGateUtil.h"
#include "xGateSchedulerService.h"
#include <list>

#define THISMODULE "URMRCPMODULEBRIDGE"
//#define AVAMOURL "https://ai.unifiedring.co.uk/bot_connector_webhooks/5e97e1f9-4122-4e9f-a807-4d2e232cffef/message.json"
#define AVAMOURL "https://ai.unifiedring.co.uk/bot_connector_webhooks/67f00d8a-e4b3-4faf-a082-64e28819335a/message.json"

using namespace std;

std::string getFilename(std::string path)
{
  int pos;
  string tmpStr;
  if((pos=path.find_last_of('/'))!= string::npos) {
    tmpStr=path.substr(pos+1);
    return tmpStr;
  }

}

/* void threadRoutineSpeechConversion(std::string filePath,ccaasVmsInfo* pServicemsg)
{
  XGLOG_INFO("threadRoutineSpeechConversion ");
  printf("!!!!!!! inside threadRoutineSpeechConversion !!!!!!\n");
  std::string outputString("");
  std::string filename = getFilename(filePath);
  std::string bucket_name = "gs://unifiedring-bucket/" + filename;
  try{
    UplaodFile(filePath,filename);
    internal_send_longRunning_request(bucket_name,outputString);
    if(!outputString.empty())
    {
      std::cout<<"Converted Text is "<<outputString<<std::endl;
      pServicemsg->set_data(outputString);
      URMRCPMODULEBRIDGE->postMessageToDbService(pServicemsg);
    }
      else
    {
      XGLOG_INFO("!!!!!!!!!!!!!Converted Text is Empty!!!!!!!");
      printf("!!!!!!!!!!!!!Converted Text is Empty!!!!!!!");
    }
    Deletefile(filename);
  }
  catch(...){
    std::cout<<"threadRoutineSpeechConversion ,triggered Default exception"<<std::endl;
    return;
  }
} */

URMRCPConfig::URMRCPConfig(){

}
URMRCPConfig::~URMRCPConfig(){}
void URMRCPConfig::setCallBack(IURModuleCallBack* ptCallback)
{
	moduleCallBack = ptCallback;
}
IURModuleCallBack* URMRCPConfig::getCallBack() {
	return moduleCallBack;
}

URMRCPModuleBridge::URMRCPModuleBridge()
{
	m_ptModule = NULL, m_strNameServerDetails = "";
	arrayModules.resize(IURDefines::UR_MAX_LEN);
}
URMRCPModuleBridge::~URMRCPModuleBridge()
{

}
bool URMRCPModuleBridge::init()
{		
	XGLOG_FUNCTION_EN_EX("");
	if (activate(THR_NEW_LWP , 1) == -1 )
	{
		XGLOG_ERROR("UR_MRCP_Controller::startThread ACE returned error while spawning");
		return false;
	}
	XGLOG_INFO("UR_MRCP_Controller::init Success");

	MRCP_CONFIG::instance()->init();

	IURModule *pCliAppService = NULL ;
	URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE, pCliAppService);
	if(!pCliAppService){
		XGLOG_ERROR("Initializing client app service module in module bridge failed !");
		return false;
	}

	IURModuleConfig *pmrcpConfig= new URMRCPConfig();
	if(!pmrcpConfig){
		XGLOG_ERROR( "URModuleBridge::init() Initializing configuration for MRCP service module failed !");
		return false;
	}
	pmrcpConfig->setCallBack(this);
	arrayModules[pCliAppService->getModuleID()] = pCliAppService;

	if(!pCliAppService->initModule(pmrcpConfig)==IURDefines::UR_RETURN_ERROR) {
		XGLOG_ERROR("init() failed while initializing 'cliAppService' !");
		return false;
	} else {
		XGLOG_INFO("init() successfully initialized 'cliAppService' !");
	}


	//Initialising Redis db connector service
	IURModule *pDbConnService = NULL ;
	URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_MRCP_REDIS_DB_SERVICE, pDbConnService);
	if(!pDbConnService){
		XGLOG_ERROR("Initializing Redis db service module in module bridge failed !");
		return false;
	}

/* 	IURModuleConfig *pMrcpDbConfig= new URMRCPConfig();
	if(!pMrcpDbConfig){
		XGLOG_ERROR( "URModuleBridge::init() Initializing configuration for MRCP service module failed !");
		return false;
	}
	pMrcpDbConfig->setCallBack(this); */
	arrayModules[pDbConnService->getModuleID()] = pDbConnService;

	if(!pDbConnService->initModule(pmrcpConfig)==IURDefines::UR_RETURN_ERROR) {
		XGLOG_ERROR("init() failed while initializing 'redis db service' !");
		return false;
	} else {
		XGLOG_INFO("init() successfully initialized 'redis db service' !");
	}

        //Initialising Redis db subscriber service
        IURModule *pDbSubService = NULL ;
        URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_MRCP_REDIS_DB_SUBSCRIBE, pDbSubService);
        if(!pDbSubService){
                XGLOG_ERROR("Initializing Redis db subscriber service module in module bridge failed !");
                return false;
        }

        /* IURModuleConfig *pMrcpDbSubConfig= new URMRCPConfig();
        if(!pMrcpDbSubConfig){
                XGLOG_ERROR( "URModuleBridge::init() Initializing configuration for MRCP service module failed !");
                return false;
        }
        pMrcpDbSubConfig->setCallBack(this); */
        arrayModules[pDbSubService->getModuleID()] = pDbSubService;

        if(!pDbSubService->initModule(pmrcpConfig)==IURDefines::UR_RETURN_ERROR) {
                XGLOG_ERROR("init() failed while initializing 'redis db service' !");
                return false;
        } else {
                XGLOG_INFO("init() successfully initialized 'redis db service' !");
        }

	// Scheduler module
	IURModule *pSchedulerService = NULL;
	URModuleFactory::CreateModuleInterface(IURDefines::UR_MODULE_SCHEDULER_SERVICE, pSchedulerService);
	if(!pSchedulerService)
	{
		XGLOG_ERROR("Initializing scheduler service module in ACD controller failed !");
		return false;
	}
	IURModuleConfig *pSchedulerConfig = new URMRCPConfig();
	if(!pSchedulerConfig)
	{
		XGLOG_ERROR("Initializing configuration for scheduler service module in ACD controller failed !");
		return false;
	}
	pSchedulerConfig->setCallBack(this);
	arrayModules[pSchedulerService->getModuleID()] = pSchedulerService;
	if (!pSchedulerService->initModule(pSchedulerConfig) == IURDefines::UR_RETURN_ERROR)
	{
		XGLOG_ERROR("init() failed while initializing 'SchedulerService' !");
		return false;
	}
	else {
		XGLOG_INFO("init() successfully initialized 'SchedulerService' !");
	}
	return true;
}

bool URMRCPModuleBridge::handleModuleCallbackMsg(IURModuleMsg * pctrlMsg)
{
	XGLOG_FUNCTION_EN_EX("");

	if (pctrlMsg) {
		XGLOG_INFO("handleModuleCallbackMsg Message Received!");
		if (IURDefines::UR_MODULE_SERVICE_BRIDGE == pctrlMsg->getDstModuleId())
		{
			URMRCPMODULEBRIDGE->putq( dynamic_cast<ACE_Message_Block*>(pctrlMsg));
		}
		else if (arrayModules[pctrlMsg->getDstModuleId()])
		{
			arrayModules[pctrlMsg->getDstModuleId()]->pushModuleMsg(pctrlMsg);
		}
		else
		{
			XGLOG_ERROR("handleModuleCallbackMsg failed, destination Unknown!");
		}
	}

	return true;
}

int URMRCPModuleBridge::svc(void)
{
	ACE_Message_Block *pAmb = 0;

	while(true)
	{
		ACE_Time_Value tv ((time(NULL) + 20));
		int result = getq(pAmb, &tv);
		if ( ( result == -1 ) && ( errno == EWOULDBLOCK ) )
		{
			continue;
		}
		if ((result != -1))
		{
			handle_msg(pAmb);
		}
		else
		{
			XGLOG_ERROR( "URMRCPModuleBridge::svc Error in Message Queue");
			break;
		}
		if(pAmb)
		{

			// delete pAmb;// message are passing in to the another thread.
			//pAmb = NULL;
		}
	}

	XGLOG_INFO( "URMRCPModuleBridge::svc() stopped");
	return 0;
}

bool URMRCPModuleBridge::stop(void)
{
	msg_queue()->deactivate();
	wait();
	XGLOG_INFO( "URMRCPModuleBridge::stop() stopped !");
	return true;
}

bool URMRCPModuleBridge::handle_msg(ACE_Message_Block *pAmb)
{
	XGLOG_INFO( "URMRCPModuleBridge::processing handle_msg!");

	// Added to get Http response data from Avamo -starts 		
	URMRCPBaseMsg *pMsg = dynamic_cast<URMRCPBaseMsg*>(pAmb);
		
	switch(pMsg->getSrcModuleId())
	{
		case IURDefines::UR_MODULE_MRCP_REDIS_DB_SUBSCRIBE:
		{
			UrMrcpDbServiceMsg* dbService = dynamic_cast<UrMrcpDbServiceMsg*>(pAmb);
			process_mrcp_avaamo_response(dbService);
			break;
		}
		case IURDefines::UR_MODULE_SCHEDULER_SERVICE:
		{
			xGateSchedulerServiceMsg *pMsg = dynamic_cast<xGateSchedulerServiceMsg*>(pAmb);
			process_mrcp_scheduler_msg(pMsg);
			break;
		}
		case IURDefines::UR_MODULE_MRCP_REDIS_DB_SERVICE:
		{
			UrMrcpDbServiceMsg* dbService = dynamic_cast<UrMrcpDbServiceMsg*>(pAmb);
			process_mrcp_msg_from_db_service(dbService);
			break;
		}
		default:
		{
			xGateDbConfigMgrServiceMsg *mrcpServiceMsg = dynamic_cast<xGateDbConfigMgrServiceMsg*>(pAmb);

			// added Defensive code - starts 
			if (!mrcpServiceMsg)
			{
				XGLOG_INFO("URMRCPModuleBridge::handle_msg  mrcpServiceMsg is NULL");
				return ;
			}	
			switch(mrcpServiceMsg->get_mrcp_command())
			{
				case EN_MSG_PORT_ALLOCATE_REQ:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_MSG_PORT_ALLOCATE_REQ' for callid(%s) and uid(%s)!",(char*)ur_log_string(mrcpServiceMsg->m_callId),(char*)ur_log_string(mrcpServiceMsg->m_uid));
					process_mrcp_port_allocate_req(mrcpServiceMsg);
				}
				break;
				case EN_TTS_MSG_START_REQ:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_TTS_MSG_START_REQ' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					process_mrcp_tts_start_req(mrcpServiceMsg);
				}
				break;
				case EN_MSG_DTMF_DIGITS:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_MSG_DTMF_DIGITS' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					process_mrcp_received_dtmf_digit(mrcpServiceMsg);
					break;
				}
				case EN_MSG_RELEASE:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_MRCP_MSG_RELEASE' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					XGLOG_INFO("URMRCPModuleBridge::EN_MRCP_MSG_RELEASE Initiated");
					// process_mrcp_termination_req(mrcpServiceMsg);
					handleTerminationRequest(mrcpServiceMsg);
					XGLOG_INFO("URMRCPModuleBridge::EN_MRCP_MSG_RELEASE Exited");
				}
				break;
				case EN_STT_REMOVE_PARTICIPANT:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_STT_REMOVE_PARTICIPANT' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					process_mrcp_termination_req(mrcpServiceMsg);
					remove_user_from_display_list(mrcpServiceMsg->m_uid.c_str(),mrcpServiceMsg->m_callId.c_str());
					removeFromcallDetailsMap(mrcpServiceMsg->m_uid.c_str(),mrcpServiceMsg->m_callId.c_str());
					deleteUserSessionInfo(mrcpServiceMsg->m_callId.c_str());
					m_tMrcpSessionHandler.DeleteMrcpCallUid(mrcpServiceMsg->m_callId.c_str());
					break;
				}
				case EN_MRCP_MSG_ALLOCATE_STT_TTS:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_MRCP_MSG_ALLOCATE_STT_TTS' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					process_mrcp_stt_tts_allocate_req(mrcpServiceMsg);
				}
				break;
				case EN_MRCP_MSG_SYNTHESIZE_REQ:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_MRCP_MSG_SYNTHESIZE_REQ' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					if(mrcpServiceMsg->m_displayText.empty())
					{
						mrcpServiceMsg->set_mrcp_command(EN_TTS_SPEAK_COMPLETE);
						PostMessageToMrcpMg(mrcpServiceMsg);
					}
					else
					{
						process_synth_req(mrcpServiceMsg);
					}
				}
				break;
				case EN_TTS_SPEAK_COMPLETE:
				{
					PostMessageToMrcpMg(mrcpServiceMsg);
				}
				break;
				case EN_MRCP_MSG_SYNTHESIZE_STOP:
				{
					std::cout << "!! EN_MRCP_MSG_SYNTHESIZE_STOP" << std::endl;
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_MRCP_MSG_SYNTHESIZE_STOP' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					pMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
					handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
				}
				break;
				case EN_POST_BASE64_MSG:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_POST_BASE64_MSG' for callid(%s) and uid(%s) file path(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str(),(char*)ur_log_string(mrcpServiceMsg->m_filepath));
					send_base64_filename(mrcpServiceMsg);
					break;
				}
				case EN_GET_VARAIBLE_STT:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_GET_VARAIBLE_STT' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					std::map<std::string,UrMrcpSessionDetails*>::iterator it = m_sessionDetailMap.find(mrcpServiceMsg->m_callId);
					if(it != m_sessionDetailMap.end() && it->second)
					{
						UrMrcpSessionDetails *sessionDetail = NULL;
						sessionDetail = it->second;
						sessionDetail->m_pubChannelId = xGatePubChannelId::EN_CCAAS_ACD_MG_COMM;
						sessionDetail->m_eventId = xGateMgAcdEvent::EN_CCAAS_ACD_MG_GET_VARIABLE;
					}
					break;
				}
				case EN_STT_RESUME_REQ:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_STT_RESUME_REQ' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
				}
				case EN_STT_START_REQ:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_STT_START_REQ' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					UrMrcpSessionDetails* sInfo = nullptr;
					STR msg("");
					msg = mrcpServiceMsg->get_mrcp_session_detail();
					sInfo = decode_mrcp_sdp_message(msg.c_str());
					auto it = m_sessionDetailMap.find(mrcpServiceMsg->m_callId);
					if(it != m_sessionDetailMap.end() && it->second)
					{
						it->second->m_sessionRecogState = EN_SESSION_RECOG_START;
						if(sInfo)
						{
							it->second->m_mrcpDeviceType = sInfo->m_mrcpDeviceType.c_str();
						}
						else
						{
							XGLOG_ERROR( "URMRCPModuleBridge::processing 'EN_STT_START_REQ' sessionInfo not found for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
						}
					}
					add_user_to_display_list(mrcpServiceMsg);
					break;
				}
				case EN_STT_STOP_REQ:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_STT_STOP_REQ' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
				}
				case EN_STT_PAUSE_REQ:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_STT_PAUSE_REQ' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					remove_user_from_display_list(mrcpServiceMsg->m_uid.c_str(),mrcpServiceMsg->m_callId.c_str());
					break;
				}
				case EN_STT_TTS_TRANSFER_ADD_PARTICIPANT:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_STT_TTS_TRANSFER_ADD_PARTICIPANT' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					handleSessionForTransferCall(mrcpServiceMsg->m_uid.c_str(),mrcpServiceMsg->m_callId.c_str());
					break;
				}
				case EN_MRCP_MSG_RECOGNITION_STOP:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_MRCP_MSG_RECOGNITION_STOP' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					std::map<std::string,UrMrcpSessionDetails*>::iterator it = m_sessionDetailMap.find(mrcpServiceMsg->m_callId);
					if(it != m_sessionDetailMap.end() && it->second)
					{
						UrMrcpSessionDetails *sessionDetail = NULL;
						sessionDetail = it->second;
						sessionDetail->m_bisccaasChatBotInit = 0;
					}
					break;
				}
				case EN_VOICE_BOT_DEINIT:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_VOICE_BOT_DEINIT' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					std::map<std::string,UrMrcpSessionDetails*>::iterator it = m_sessionDetailMap.find(mrcpServiceMsg->m_callId);
					if(it != m_sessionDetailMap.end() && it->second)
					{
						UrMrcpSessionDetails *sessionDetail = NULL;
						sessionDetail = it->second;
						sessionDetail->m_bisccaasChatBotInit = false;
						sessionDetail->m_bIsVoiceBotDeInit = true;
					}
					break;
				}
				case EN_STT_START:
				{
					XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_STT_START' for callid(%s) and uid(%s)!",mrcpServiceMsg->m_callId.c_str(),mrcpServiceMsg->m_uid.c_str());
					std::map<std::string,UrMrcpSessionDetails*>::iterator it = m_sessionDetailMap.find(mrcpServiceMsg->m_callId);
					if(it != m_sessionDetailMap.end() && it->second)
					{
						UrMrcpSessionDetails *sessionDetail = NULL;
						sessionDetail = it->second;
						if(sessionDetail->m_ccaasUsertype == EN_CCAAS_USER_TYPE_CUSTOMER && (sessionDetail->m_bIsCcaasVirtualAgent || sessionDetail->m_iIsCcaasAutoAttendant))
						{
							sessionDetail->m_bisccaasChatBotInit = true;
							XGLOG_INFO("! URMRCPModuleBridge::handle_msg sending 'INIT' info to DB Service");
							xGateDbConfigMgrServiceMsg *pServicemsg = new xGateDbConfigMgrServiceMsg;
							// *pServicemsg = *pMsg;
							pServicemsg->fillServiceMsg(mrcpServiceMsg);
							pServicemsg->set_ccaasCallStatus("init");
							pServicemsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
							PostMessageToMrcpMg(pServicemsg);
						}
						else if(sessionDetail->m_ccaasUsertype == EN_CCAAS_USER_TYPE_AGENT && (sessionDetail->m_bIsCcaasVirtualAgent || sessionDetail->m_iIsCcaasAutoAttendant))
						{
							xGateMRCPMsg* pMgMsg = NULL;
							pMgMsg = new xGateMRCPMsg();
							XGLOG_INFO( "URMRCPModuleBridge::processing 'EN_STT_START' Sending EN_STT_RECOGNITION_RESUME for Customer for callid(%s) and uid(%s)!",sessionDetail->m_callId.c_str(),sessionDetail->m_mgResourceId.c_str());
							pMgMsg->set_mrcp_uid(sessionDetail->m_mgResourceId);
							pMgMsg->set_mrcp_call_id(sessionDetail->m_mgResourceId);
							pMgMsg->set_mrcp_command(EN_STT_RECOGNITION_RESUME);
							pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
							postMessageToMediaService(pMgMsg);
						}
					}
					break;
				}
				default:
				{
					XGLOG_INFO("URMRCPModuleBridge::handle_msg UNKNOWN Case");
				}
			}
		}
	}
			return true;
}

bool URMRCPModuleBridge::add_user_to_display_list(xGateDbConfigMgrServiceMsg *pServiceMsg)
{
	if(!pServiceMsg)
	{
		XGLOG_ERROR("URMRCPModuleBridge::add_user_to_display_list() is null");
		return false;
	}

	XGLOG_INFO("URMRCPModuleBridge::add_user_to_display_list uID[%s], callID[%s]",(char*)ur_log_string(pServiceMsg->m_uid),(char*)ur_log_string(pServiceMsg->m_callId));
	
	UrMrcpSessionDetails *sessionInfo = nullptr;
	if(getSessionInfo(pServiceMsg->m_callId,sessionInfo))
	{
		Display_Info displayInfo;

		displayInfo.ext = sessionInfo->m_mrcpExtNum.c_str();
		displayInfo.domain_id = sessionInfo->m_mrcpDomainId.c_str();
		displayInfo.device = sessionInfo->m_mrcpDeviceType;

		sessionInfo->m_mrcpSttEnabled = pServiceMsg->get_stt_enabled();
		
		if(sessionInfo->m_mrcpSttEnabled)
		{
			insertUserToList(pServiceMsg->m_uid,displayInfo);
		}
	}
	else
	{
		XGLOG_ERROR("URMRCPModuleBridge::add_user_to_display_list session not found uID[%s], callID[%s]",(char*)ur_log_string(pServiceMsg->m_uid),(char*)ur_log_string(pServiceMsg->m_callId));
		return false;
	}
	return true;
}

bool URMRCPModuleBridge::remove_user_from_display_list(std::string uID, std::string callID)
{
	if(uID.empty() || callID.empty())
	{
		XGLOG_ERROR("URMRCPModuleBridge::remove_user_from_display_list() either uID or callID is null");
		return false;
	}

	XGLOG_INFO("URMRCPModuleBridge::remove_user_from_display_list uID[%s], callID[%s]",(char*)ur_log_string(uID),(char*)ur_log_string(callID));

	UrMrcpSessionDetails *sessionInfo = nullptr;
	if(getSessionInfo(callID.c_str(),sessionInfo))
	{
		sessionInfo->m_mrcpSttEnabled = false;
		eraseUserFromList(uID.c_str(),sessionInfo->m_mrcpExtNum);
	}
	else
	{
		XGLOG_ERROR("URMRCPModuleBridge::remove_user_from_display_list session not found uID[%s], callID[%s]",(char*)ur_log_string(uID),(char*)ur_log_string(callID));
		return false;
	}
	return true;
}

bool URMRCPModuleBridge::send_base64_filename(xGateDbConfigMgrServiceMsg *mrcpServiceMsg)
{
	XGLOG_INFO("URMRCPModuleBridge::send_base64_filename uid(%s), callid(%s)",(char*)ur_log_string(mrcpServiceMsg->m_uid),(char*)ur_log_string(mrcpServiceMsg->m_callId));
	std::map<std::string,UrMrcpSessionDetails*>::iterator it = m_sessionDetailMap.find(mrcpServiceMsg->m_callId);
	
	if(it != m_sessionDetailMap.end())
	{
		mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
		UrMrcpSessionDetails *sessionDetail = NULL;
		sessionDetail = it->second;
		sessionDetail->m_pubChannelId = xGatePubChannelId::EN_CCAAS_ACD_MG_COMM;
		sessionDetail->m_eventId = xGateMgAcdEvent::EN_CCAAS_ACD_MG_BASE64_FILE_NAME;
		sessionDetail->m_base64Path = mrcpServiceMsg->m_filepath.c_str();
		PostMessageToMrcpMg(mrcpServiceMsg);
	}
	else
	{
		XGLOG_ERROR("URMRCPModuleBridge::send_base64_filename session details not found for uid(%s), callid(%s)",(char*)ur_log_string(mrcpServiceMsg->m_uid),(char*)ur_log_string(mrcpServiceMsg->m_callId));
		return false;
	}
	return true;
}

bool URMRCPModuleBridge::process_mrcp_stt_tts_allocate_req(xGateDbConfigMgrServiceMsg *pMsg)
{
	XGLOG_INFO("URMRCPModuleBridge::process_mrcp_stt_tts_allocate_req Initiated");
	UrMrcpSessionDetails *sessionDetails = NULL;
	std::string msg = pMsg->get_mrcp_session_detail();
	sessionDetails = decode_mrcp_sdp_message(msg.c_str());
	if(!sessionDetails)
	{
		XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_stt_tts_allocate_req sessionDetails is NULL uID[%s]",(char*)ur_log_string(pMsg->m_uid));
		return false;
	}
	pMsg->m_callId = sessionDetails->m_callId.c_str();
	sessionDetails->m_asrType = pMsg->get_asr_type();
	sessionDetails->m_ttsType = pMsg->get_tts_type();
	sessionDetails->m_memberType = EN_MEMBER_TYPE::EN_MEMBER_TYPE_STANDARD;
	sessionDetails->m_asrType = EN_XGATE_ASR_INHOUSE;
	//sessionDetails->m_memberType = EN_MEMBER_TYPE::EN_MEMBER_TYPE_PREMIUM;
	XGLOG_INFO("! URMRCPModuleBridge::process_mrcp_stt_tts_allocate_req ccaasUsertype - %d ", sessionDetails->m_ccaasUsertype);
  
	std::map<std::string,UrMrcpSessionDetails*>::iterator it = m_sessionDetailMap.find(sessionDetails->m_mgResourceId);
	if(it != m_sessionDetailMap.end())
	{
		XGLOG_INFO("! URMRCPModuleBridge::process_mrcp_stt_tts_allocate_req id - %s already found in m_sessionDetailMap",sessionDetails->m_mgResourceId.c_str());
		if(sessionDetails->m_ccaasUsertype == EN_CCAAS_USER_TYPE_AGENT || sessionDetails->m_ccaasUsertype == EN_CCAAS_USER_TYPE_SUPERVISOR || it->second->m_ccaasUsertype == EN_CCAAS_USER_TYPE_AGENT ||
			sessionDetails->m_ccaasUsertype == EN_CCAAS_USER_TYPE_TRANSFER_AGENT || sessionDetails->m_ccaasUsertype == EN_CCAAS_USER_TYPE_TRANSFEROR_AGENT)
		{
			sessionDetails->m_isCcaasAgentConnected = it->second->m_isCcaasAgentConnected = true;
			sessionDetails->m_pubChannelId = it->second->m_pubChannelId = xGatePubChannelId::EN_CCAAS_AGENT_CONNECTED;
		}

		if((sessionDetails->m_ccaasUsertype != EN_CCAAS_USER_TYPE_SUPERVISOR) && (it->second->m_ccaasUsertype == EN_CCAAS_USER_TYPE_CUSTOMER && it->second->m_mrcpSttEnabled))
		{
			XGLOG_INFO("! URMRCPModuleBridge::process_mrcp_stt_tts_allocate_req calling send_stop_start_req_to_stt_mg() ");
			UrMrcpSessionDetails *serviceMsg=it->second;
			send_stop_start_req_to_stt_mg(serviceMsg);
		}

		sessionDetails->m_bisUserInCall = true;

		if(it->second->m_ccaasUsertype == EN_CCAAS_USER_TYPE_CUSTOMER && sessionDetails->m_ccaasUsertype == EN_CCAAS_USER_TYPE_AGENT)
		{
			if(it->second->m_bisUserInCall && it->second->m_bisccaasChatBotInit)
			{
				it->second->m_bisccaasChatBotInit = false;
				it->second->m_bisReconnect = true;
				it->second->m_sessionRecogState = EN_SESSION_RECOG_START;
				xGateDbConfigMgrServiceMsg *ptrMsg = new xGateDbConfigMgrServiceMsg();
				ptrMsg->m_uid = it->second->m_mgResourceId.c_str();
				ptrMsg->m_callId = it->second->m_callId.c_str();
				it->second->m_bIsVoiceBotDeInit = false;
				XGLOG_INFO("URMRCPModuleBridge::process_mrcp_stt_tts_allocate_req sending EN_RESTART_STT_RECOGNIZATION for CUSTOMER - uID[%s],callID[%s]",(char*)ur_log_string(ptrMsg->m_uid),(char*)ur_log_string(ptrMsg->m_callId));
				ptrMsg->set_mrcp_command(EN_RESTART_STT_RECOGNIZATION);
				PostMessageToMrcpMg(ptrMsg);
			}
		}

		it->second->m_strCcaasCustomerFirstName = it->second->m_strCcaasCustomerFirstName.empty()?sessionDetails->m_strCcaasCustomerFirstName.c_str():it->second->m_strCcaasCustomerFirstName.c_str();
		it->second->m_strCcaasCustomerLastName = it->second->m_strCcaasCustomerLastName.empty()?sessionDetails->m_strCcaasCustomerLastName.c_str():it->second->m_strCcaasCustomerLastName.c_str();
	}
	else
	{		
		XGLOG_INFO("! URMRCPModuleBridge::process_mrcp_stt_tts_allocate_req id - %s not found in m_sessionDetailMap",sessionDetails->m_mgResourceId.c_str());
		sessionDetails->m_pubChannelId = xGatePubChannelId::EN_CCAAS_BOT;
		sessionDetails->m_bisUserInCall = true;
		EN_BOT_TYPE botType = sessionDetails->getIsCcaasAutoAttendant() ? EN_BOT_TYPE::EN_BOT_TYPE_CCAAS_AUTO_ATTENDANT : sessionDetails->getIsSmartAssist() ? EN_BOT_TYPE::EN_BOT_TYPE_SMART_ASSIST : EN_BOT_TYPE::EN_BOT_TYPE_CCAAS_AUTO_ATTENDANT;
  		sessionDetails->setBotType(botType);
	}

	insertSessionInfo(sessionDetails->m_callId,sessionDetails);
	insertCallSessionId(sessionDetails->m_mgResourceId,sessionDetails->m_callId);
	m_tMrcpSessionHandler.InsertMrcpCallUid(sessionDetails->m_callId,sessionDetails->m_mgResourceId);
	XGLOG_INFO("! URMRCPModuleBridge::process_mrcp_stt_tts_allocate_req id - %s inserting into m_sessionDetailMap",sessionDetails->m_callId.c_str());
	pMsg->m_tSessionDetails = sessionDetails;
	if(sessionDetails->m_ccaasPorttype != EN_XGATE_STT)
	{
		XGLOG_INFO("URMRCPModuleBridge::process_mrcp_port_allocate_request 'TTS' is enabled");
		pMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
		handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
	}
	else
	{
		XGLOG_INFO("URMRCPModuleBridge::process_mrcp_port_allocate_request 'TTS' is disabled");
	}

	if(sessionDetails->m_ccaasPorttype != EN_XGATE_TTS)
	{
		XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_stt_tts_allocate_req sending EN_CREATE_STT_SOCKET_CONNECTION uID[%s] callID[%s]",sessionDetails->m_mgResourceId.c_str(),sessionDetails->m_callId.c_str());
		xGateMRCPMsg* pMgMsg = nullptr;
		pMgMsg = new xGateMRCPMsg();
		pMgMsg->set_mrcp_uid(sessionDetails->m_mgResourceId);
		pMgMsg->set_mrcp_call_id(sessionDetails->m_callId);
		pMgMsg->set_mrcp_command(EN_CREATE_STT_SOCKET_CONNECTION);
		pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
		postMessageToMediaService(pMgMsg);
	}
	return true;
}

bool URMRCPModuleBridge::send_stop_start_req_to_stt_mg(UrMrcpSessionDetails *serviceMsg)
{
	XGLOG_ERROR( "URMRCPModuleBridge::send_stop_start_req_to_stt_mg ");
	if(serviceMsg->m_isCcaasAgentConnected)
	{
		      XGLOG_ERROR( "URMRCPModuleBridge::send_stop_start_req_to_stt_mg inside");
			      ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
          taskPtr = (ACE_Task<ACE_MT_SYNCH>*)xGateUtil::getHmpProcessor();
          if(!taskPtr) {
                  XGLOG_ERROR( "URMRCPModuleBridge::send_stop_start_req_to_stt_mg HmpProcessor task not found");
                  return EN_XGATE_STATUS_ERROR;
          }

		  #if 1
          xGateMRCPMsg* pMgMsg1 = NULL;
          pMgMsg1 = new xGateMRCPMsg();

		  if(serviceMsg->m_asrType == EN_XGATE_ASR_SPEECH_MATICS)
		  {
			pMgMsg1->set_mrcp_command(EN_STT_MSG_RECOGNITION_START_REQ);
		  }
		  pMgMsg1->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
		  pMgMsg1->set_mrcp_uid(serviceMsg->m_mgResourceId);
          pMgMsg1->set_mrcp_call_id(serviceMsg->m_callId);

		  ACE_Message_Block* pAmb1 = NULL;
          pAmb1 = static_cast<xGateMRCPMsg*> (pMgMsg1);
          ACE_Time_Value tval1 ((time(NULL) ));
          tval1 += ACE_Time_Value(0,1);

		   
		   XGLOG_INFO("URMRCPModuleBridge::send_stop_start_req_to_stt_mg Sending Start for callid - %s ",serviceMsg->m_callId.c_str());
		   if(taskPtr->putq(pAmb1, &tval1)) 
		   {
                  XGLOG_INFO( "URMRCPModuleBridge::send_stop_start_req_to_stt_mg HmpProcessor enqueue message success");
           } else 
		   {
                  XGLOG_ERROR( "URMRCPModuleBridge::send_stop_start_req_to_stt_mg HmpProcessor enqueue message failed");
                  delete pMgMsg1;
                  return true;
           }
		   #endif
	}
	else
	{
		XGLOG_INFO( "URMRCPModuleBridge::send_stop_start_req_to_stt_mg serviceMsg->m_isCcaasAgentConnected is false ");
	}
}

bool URMRCPModuleBridge::process_mrcp_port_allocate_req(xGateDbConfigMgrServiceMsg *pMsg)
{
	XGLOG_INFO("URMRCPModuleBridge::process_mrcp_port_allocate_request Initiated for callID[%s]",(char*)ur_log_string(pMsg->get_call_id()));
	UrMrcpSessionDetails *sessionDetails = NULL;
	std::string msg = pMsg->get_mrcp_session_detail();
	sessionDetails = decode_mrcp_sdp_message(msg.c_str());
	sessionDetails->set_avaamo_bot_name();
	sessionDetails->m_asrType = pMsg->get_asr_type();
	sessionDetails->m_pubChannelId = xGatePubChannelId::EN_PUB_ID_UCAAS;
	XGLOG_INFO("!!!!! URMRCPModuleBridge::process_mrcp_port_allocate_req Asr Type - %d !!!!!!!",sessionDetails->m_asrType);
	insertSessionInfo(sessionDetails->m_callId,sessionDetails);
	// m_callDetailsMap.insert({sessionDetails->m_mgResourceId,sessionDetails->m_callId});
	insertCallSessionId(sessionDetails->m_mgResourceId,sessionDetails->m_callId);
	m_tMrcpSessionHandler.InsertMrcpCallUid(sessionDetails->m_callId,sessionDetails->m_mgResourceId);

	xGateMRCPMsg* pMgMsg = nullptr;
	pMgMsg = new xGateMRCPMsg();
	pMgMsg->set_mrcp_uid(sessionDetails->m_mgResourceId);
	pMgMsg->set_mrcp_call_id(sessionDetails->m_callId);
	XGLOG_INFO("URMRCPModuleBridge::process_mrcp_port_allocate_request sending EN_CREATE_STT_SOCKET_CONNECTION for callID[%s]",sessionDetails->m_callId.c_str());
	pMgMsg->set_mrcp_command(EN_CREATE_STT_SOCKET_CONNECTION);
	pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
	postMessageToMediaService(pMgMsg);
	return true;
}

bool URMRCPModuleBridge::process_mrcp_tts_start_req(xGateDbConfigMgrServiceMsg *pMsg)
{
	UrMrcpSessionDetails *sessionDetails = NULL;
	std::map<std::string,UrMrcpSessionDetails *>::iterator it = m_sessionDetailMap.find(pMsg->m_uid);
	if(it != m_sessionDetailMap.end() && it->second)
	{
		sessionDetails = it->second;
	}
	else
	{
		XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_start_req session details not found for callid (%s) ",pMsg->m_uid.c_str());
		return false;
	}
	UrMrcpDbServiceMsg *dbServiceMsg = new UrMrcpDbServiceMsg();
	if(dbServiceMsg && !sessionDetails->m_mrcpExtNum.empty())
	{
		dbServiceMsg->set_session_uid(pMsg->m_uid.c_str());
		dbServiceMsg->set_session_speaker(sessionDetails->m_mrcpExtNum.c_str());
		dbServiceMsg->set_avaamo_bot_name(sessionDetails->m_botName);
		dbServiceMsg->set_display_text("reset");
		dbServiceMsg->set_avaamo_request_type("join");
		dbServiceMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_REDIS_DB_SERVICE);
		handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(dbServiceMsg));
	}
	return true;
}

bool URMRCPModuleBridge::process_mrcp_recognition_start_req(xGateDbConfigMgrServiceMsg *pMsg)
{
	//std::string callId = pMsg->get_call_id();
	std::string sessionId("");	
#if 0
	UrMrcpSessionDetails *sessionInfo = NULL;
	std::multimap<std::string,std::string>::iterator itr = m_callDetailsMap.find(pMsg->m_uid);
	for(itr;itr!=m_callDetailsMap.end();itr++)
	{
		std::string callId = itr->second;
		if(strncmp(callId.c_str(),pMsg->m_callId.c_str(),callId.length())==0)
		{
			std::map<std::string,UrMrcpSessionDetails *>::iterator it = m_sessionDetailMap.find(callId);
			if(it!=m_sessionDetailMap.end())
			{
				sessionInfo = it->second;
				sessionInfo->m_sessionRecogState = EN_SESSION_RECOG_START;
				pMsg->m_tSessionId = callId.c_str();
				if(sessionInfo->m_mrcpSttEnabled)
				{
					std::string displayUser = frameDisplayInfo(sessionInfo->m_mrcpExtNum,sessionInfo->m_mrcpDomainId,sessionInfo->m_mrcpDeviceType);
					//m_displayUserList.push_back(displayUser);
					std::map<std::string,std::list<std::string>>::iterator userList_itr = m_displayUserList.find(pMsg->m_uid);
					if(userList_itr != m_displayUserList.end())
					{
						userList_itr->second.push_back(displayUser);
					}
					else if(userList_itr == m_displayUserList.end())
					{
						std::list<std::string> user_list;
						user_list.push_back(displayUser);
						m_displayUserList.insert({pMsg->m_uid,user_list});
					}
				}
			}
		}

	}
	pMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
	handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
#endif
	return true;	
}

bool URMRCPModuleBridge::process_synth_req(xGateDbConfigMgrServiceMsg *pMsg)
{
  std::map<std::string,UrMrcpSessionDetails *>::iterator itr = m_sessionDetailMap.find(pMsg->m_callId);
  if(itr!= m_sessionDetailMap.end() && itr->second)
  {
    itr->second->m_tts_text_type = 1;
	itr->second->m_bisccaasChatBotInit = pMsg->m_bisccaasChatBotInit;
	itr->second->m_bisBotFirst = pMsg->m_bisBotFirst;
	itr->second->m_ccaasChannelId = pMsg->m_strCcaasChannelId.c_str();
	itr->second->m_ccaasBlockType = pMsg->m_strCcaasBlockType.c_str();
	itr->second->m_ccaasBlockId = pMsg->m_strCcaasBlockId.c_str();
	itr->second->m_ccaasAgentId = pMsg->m_strCcaasAgentId.c_str();
	itr->second->m_ccaasBotUrl = pMsg->m_strCcaasBotUrl.c_str();
	itr->second->m_iCcaasBotId = pMsg->m_iCcaasBotId;
  }

#if 1
  if(pMsg->m_bisccaasChatBotInit)
  {
	XGLOG_INFO("! URMRCPModuleBridge::process_synth_req sending 'INIT' info to DB Service");
  	xGateDbConfigMgrServiceMsg *pServicemsg = new xGateDbConfigMgrServiceMsg;
  	// *pServicemsg = *pMsg;
	pServicemsg->fillServiceMsg(pMsg);
  	pServicemsg->set_ccaasCallStatus("init");
  	pServicemsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
  	PostMessageToMrcpMg(pServicemsg);
  }
#endif

  pMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
  handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
  return true;
}

bool URMRCPModuleBridge::process_mrcp_recognition_send_req(xGateDbConfigMgrServiceMsg *pMsg)
{
	pMsg->m_tSessionId = pMsg->m_callId.c_str();
	pMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
	handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
	return true;
}

bool URMRCPModuleBridge::process_mrcp_recognition_pause_req(xGateDbConfigMgrServiceMsg *pMsg)
{
	pMsg->m_tSessionId = pMsg->m_callId.c_str();
	pMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
	handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
	return true;
}

bool URMRCPModuleBridge::process_mrcp_recognition_resume_req(xGateDbConfigMgrServiceMsg *pMsg)
{
	pMsg->m_tSessionId = pMsg->m_callId.c_str();
        std::map<std::string,UrMrcpSessionDetails *>::iterator itr = m_sessionDetailMap.find(pMsg->m_callId);
        if(itr!= m_sessionDetailMap.end() && itr->second)
        {
                itr->second->m_mrcpSttEnabled = pMsg->m_isSttEnabled;
        }
	pMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
	handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
	return true;
}

bool URMRCPModuleBridge::process_mrcp_recognition_stop_req(xGateDbConfigMgrServiceMsg *pMsg)
{
	pMsg->m_tSessionId = pMsg->m_callId.c_str();
	pMsg->set_mrcp_command(EN_MRCP_MSG_RECOGNITION_STOP);
	pMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
	handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
	return true;
}

bool URMRCPModuleBridge::process_mrcp_termination_req(xGateDbConfigMgrServiceMsg *pMsg)
{
	if(!pMsg || pMsg->m_callId.empty())
	{
		XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_termination_req either pMsg is null or callid is null");
		return false;
	}
	XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_termination_req uid(%s), callid(%s)",(char*)ur_log_string(pMsg->m_uid),(char*)ur_log_string(pMsg->m_callId));
	std::map<std::string,UrMrcpSessionDetails *>::iterator itr = m_sessionDetailMap.find(pMsg->m_callId);
	if(itr!= m_sessionDetailMap.end())
	{
		itr->second->m_bisUserInCall = false;
		UrMrcpSessionDetails *Sessiondetail = itr->second;
		// pMsg->set_ccaasCallStatus("end_call");
		XGLOG_INFO("URMRCPModuleBridge::process_mrcp_termination_req uid(%s) text - %s",(char*)ur_log_string(pMsg->m_uid),(char*)ur_log_string(pMsg->m_ccaasCallStatus));
		STR sessionExt = Sessiondetail->m_mrcpExtNum.c_str();
		STR callID = pMsg->m_callId.c_str();
#if 0
		xGateDbConfigMgrServiceMsg *msg = new xGateDbConfigMgrServiceMsg;
		//*msg = *pMsg;
		msg->fillServiceMsg(pMsg);
		msg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
		PostMessageToMrcpMg(msg);
#endif
		if(Sessiondetail->m_ccaasPorttype != EN_XGATE_STT)
		{
			xGateDbConfigMgrServiceMsg *ptrMsg = new xGateDbConfigMgrServiceMsg;
			ptrMsg->fillServiceMsg(pMsg);
			ptrMsg->set_mrcp_command(EN_MSG_RELEASE);
			ptrMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
			handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(ptrMsg));
		}

		postMessageToSchedulerService(callID.c_str(), EN_SCHEDULER_BOT_RESP_WAIT_TIMER, SCHEDULER_FUNCTION_CANCEL_TIMER);
		postMessageToSchedulerService(callID.c_str(), EN_SCHEDULER_CUSTOMER_RESP_WAIT_TIMER, SCHEDULER_FUNCTION_CANCEL_TIMER);

		Document doc;
		STR ID("");
		StringBuffer strBuf;
		Writer<StringBuffer> writer(strBuf);
		if(m_tMrcpSessionHandler.GetMrcpCallUid(callID,ID))
		{
			auto IT = m_sessionDetailMap.find(ID);
			if(IT != m_sessionDetailMap.end())
			{
				std::string docMsg = IT->second->m_strDocSocketMessage.c_str();
				if(!docMsg.empty())
				{
					ParseResult ok = doc.Parse(docMsg.c_str());
					Document::AllocatorType& allocator = doc.GetAllocator();
					if(!ok) 
					{
						XGLOG_ERROR( "parseJsonMsg encountered with json parse error !");
						return false;
					}

					if(doc.HasMember("agent"))
					{
						if(doc["agent"].IsArray() && !doc["agent"].Empty())
						{
							Value &agent = doc["agent"];
							Value::ConstValueIterator agentItr = agent.Begin();
							for(agentItr;agentItr != agent.End();agentItr++)
							{
								const Value &memItr = *agentItr;
								if(memItr.HasMember("ext"))
								{
									STR Ext = memItr["ext"].GetString();
									XGLOG_INFO("URMRCPModuleBridge::process_mrcp_termination_req removing Ext(%s) sessionExt [%s]",(char*)ur_log_string(Ext),(char*)ur_log_string(sessionExt));
									if(0==strncmp(Ext.c_str(),sessionExt.c_str(),Ext.length()))
									{
										agent.Erase(agentItr);
										break;
									}
								}
							}
						}
					}
					doc.Accept(writer);
					IT->second->m_strDocSocketMessage = strBuf.GetString();
					XGLOG_INFO("URMRCPModuleBridge::process_mrcp_termination_req message(%s)",(char*)ur_log_string(IT->second->m_strDocSocketMessage));
				}
			}
			else
			{
				XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_termination_req session not found found for uID[%s]",(char*)ur_log_string(ID));
			}
		}
		else
		{
			XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_termination_req uID not found found for callID[%s]",(char*)ur_log_string(callID));
		}

		return true;
	}
	XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_termination_req Could not find the callid in the sessionDetailMap");
	return false;
}

bool URMRCPModuleBridge::process_mrcp_remove_participant_req(xGateDbConfigMgrServiceMsg *pMsg)
{
	pMsg->m_tSessionId = pMsg->m_callId.c_str();
	pMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
	handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
	return true;
}

bool URMRCPModuleBridge::process_mrcp_add_participant_req(xGateDbConfigMgrServiceMsg *pMsg)
{
	std::string uid("");
	std::multimap<std::string, std::string>::iterator itr = m_callDetailsMap.find(pMsg->m_callId);
	if(itr!=m_callDetailsMap.end())
	{
		std::string callid = itr->second;
		m_callDetailsMap.erase(itr);
		m_callDetailsMap.insert({pMsg->m_uid,callid});
		if(m_tMrcpSessionHandler.GetMrcpCallUid(callid,uid))
		{
			m_tMrcpSessionHandler.DeleteMrcpCallUid(callid);
			m_tMrcpSessionHandler.InsertMrcpCallUid(callid,pMsg->m_uid);
			XGLOG_INFO("URMRCPModuleBridge::process_mrcp_add_participant_req for uid(%s) callid(%s)",pMsg->m_callId.c_str(),callid.c_str());
		}
	}
	return true;
}

bool URMRCPModuleBridge::process_mrcp_avaamo_response(UrMrcpDbServiceMsg* dbService)
{
	if(!dbService)
	{
		XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_avaamo_response dbService is null");
		return false;
	}

	if(dbService && dbService->get_message_type() == EN_MESSAGE_TYPE::EN_MESSAGE_TYPE_VMS_STT_REQ)
	{
		if(dbService->get_ccaas_vms_info())
		{
			process_vms_transcript_request(dbService->get_ccaas_vms_info());
			return true;
		}
	}

	if(dbService && dbService->get_message_type() == EN_MESSAGE_TYPE::EN_MESSAGE_TYPE_VMS_STT_RESP)
	{
		XGLOG_INFO("URMRCPModuleBridge::process_mrcp_avaamo_response vms transcript response (%s)",dbService->get_display_text().c_str());
		std::list<std::string>::iterator it;
		it =  std::find(m_vmsSttlist.begin(),m_vmsSttlist.end(),dbService->get_session_uid().c_str());
		if(it != m_vmsSttlist.end())
		{
			postMessageToRedisService(dbService);
			m_vmsSttlist.erase(it);
		}
		else
		{
			XGLOG_ERROR("sessionId not found in the m_vmsSttlist for uId(%s)",dbService->get_session_uid().c_str());
			return false;
		}
		return true;
	}

	std::string callID("");
	callID = dbService->get_session_uid().c_str();
	XGLOG_INFO("URMRCPModuleBridge::process_mrcp_avaamo_response (%s)",dbService->get_session_uid().c_str());
	UrMrcpSessionDetails *sessionInfo = NULL;
	std::map<std::string,UrMrcpSessionDetails *>::iterator it = m_sessionDetailMap.find(dbService->get_session_uid());
	if(it != m_sessionDetailMap.end() && it->second)
	{
		sessionInfo = it->second;
	}
	else 
	{
		XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_avaamo_response session details not found for callid (%s) ",dbService->get_session_uid().c_str());
		return false;
	}

	if(sessionInfo)
	{
		if(sessionInfo->m_bisUserInCall == false)
		{
			XGLOG_INFO("URMRCPModuleBridge::process_mrcp_avaamo_response returning as user is not in call uID [%s]",callID.c_str());
			return false;
		}
	}

	xGateDbConfigMgrServiceMsg *ptrMsg = new xGateDbConfigMgrServiceMsg;
	ptrMsg->m_callId = dbService->get_session_uid();
	/* if(dbService->m_iIsTransferLiveAgent == 1)
	{
		sessionInfo->m_bIsTransferLiveAgent = true;
		sessionInfo->m_strBotIndent = dbService->m_strBotIndent.c_str();
		sessionInfo->setQid(dbService->getQid());
		sessionInfo->m_pubChannelId = xGatePubChannelId::EN_CCAAS_ACD_MG_COMM;
		sessionInfo->m_eventId = xGateMgAcdEvent::EN_CCAAS_ACD_MG_BOT_INDENT;
		ptrMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
		PostMessageToMrcpMg(ptrMsg);
	} */
 
	EN_BOT_TRANSFER_EVENT botEvent = dbService->getBotEvent();
	switch(botEvent)
	{
		case EN_BOT_TRANSFER_EVENT::EN_BOT_TRANSFER_EVENT_CCAAS_LIVE_AGENT:
		{
			sessionInfo->m_bIsTransferLiveAgent = true;
			sessionInfo->m_strBotIndent = dbService->m_strBotIndent.c_str();
			sessionInfo->setQid(dbService->getQid());
			sessionInfo->m_pubChannelId = xGatePubChannelId::EN_CCAAS_ACD_MG_COMM;
			sessionInfo->m_eventId = xGateMgAcdEvent::EN_CCAAS_ACD_MG_BOT_INDENT;
			ptrMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);

			xGateMRCPMsg* pMgMsg = NULL;
			pMgMsg = new xGateMRCPMsg();
			XGLOG_INFO("URMRCPModuleBridge::process_mrcp_avaamo_response EN_BOT_TRANSFER_EVENT::EN_BOT_TRANSFER_EVENT_CCAAS_LIVE_AGENT Sending EN_STT_RECOGNITION_PAUSE for uId[%s], callId[%s]",sessionInfo->m_mgResourceId.c_str(),sessionInfo->m_callId);
			pMgMsg->set_mrcp_uid(sessionInfo->m_mgResourceId);
			pMgMsg->set_mrcp_call_id(sessionInfo->m_callId);
			pMgMsg->set_mrcp_command(EN_STT_RECOGNITION_PAUSE);
			pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
			postMessageToMediaService(pMgMsg);
			PostMessageToMrcpMg(ptrMsg);
			break;
		}
		case EN_BOT_TRANSFER_EVENT::EN_BOT_TRANSFER_EVENT_SMART_ASSIST:
		{
			sessionInfo->m_mrcpExtNum = dbService->getUserExt();
			sessionInfo->m_pubChannelId = xGatePubChannelId::EN_PUB_ID_PBX;
			sessionInfo->setSmartAssistEvent(EN_BOT_SMART_ASSIST_EVENT::EN_BOT_SMART_ASSIST_EVENT_DIAL_USER);
			/* ptrMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
			PostMessageToMrcpMg(ptrMsg); */
			break;
		}
		default:
		{
			XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_avaamo_response UNKNOWN BotEvent uID[%s]",callID.c_str());
			break;
		}
		
	}

	if(sessionInfo->m_sessionSynthState == EN_SESSION_SYNTH_START)
	{
		/* if(dbService->get_orignal() && sessionInfo->m_playing == false)
		{
			sessionInfo->m_avaamo_reply.clear(); // To clear previous reply
		} */
		bot_reply_msg replyMsg(dbService->m_displayText, dbService->get_orignal());
		sessionInfo->m_avaamo_reply.clear(); // To clear previous reply
		sessionInfo->m_avaamo_reply.push_back(replyMsg);

		// sessionInfo->m_avaamo_reply.push_back(dbService->m_displayText);
	}
	else
	{
		xGateDbConfigMgrServiceMsg *pMsg = new xGateDbConfigMgrServiceMsg;
		pMsg->m_callId = dbService->get_session_uid();
		sessionInfo->m_playing = dbService->get_orignal();
		pMsg->set_mrcp_command(EN_MRCP_MSG_SYNTHESIZE_REQ);
		sessionInfo->setIsPlayTextSent(true);
		pMsg->m_displayText = dbService->m_displayText;
		pMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
		handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pMsg));
	}

	postMessageToSchedulerService(callID.c_str(), EN_SCHEDULER_BOT_RESP_WAIT_TIMER, SCHEDULER_FUNCTION_CANCEL_TIMER);

	/* if(sessionInfo->m_sessionRecogState == EN_SESSION_RECOG_START)
	{
		sessionInfo->m_sessionRecogState = EN_SESSION_RECOG_STOP;
		xGateMRCPMsg* pMgMsg = NULL;
		pMgMsg = new xGateMRCPMsg();

		XGLOG_INFO("URMRCPModuleBridge::process_mrcp_avaamo_response Sending Stop ");
		pMgMsg->set_mrcp_uid(sessionInfo->m_mgResourceId);
		pMgMsg->set_mrcp_call_id(sessionInfo->m_callId);
		pMgMsg->set_mrcp_command(EN_GRPC_MSG_STOP_CAPTURE_REQ);
		pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
		postMessageToMediaService(pMgMsg);
	} */
	deleteServiceMsg(dbService);
	return true;
}

bool URMRCPModuleBridge::encode_vms_transcript_req_msg(ccaasVmsInfo* &ccaasInfo)
{
	XGLOG_INFO("URMRCPModuleBridge::encode_vms_transcript_req_msg for session id(%s) ",(char*)ur_log_string(ccaasInfo->get_session_id()));
	Document doc;
	StringBuffer strBuf;
	Writer<StringBuffer> writer(strBuf);
	Value outputVal(kObjectType);
	std::string Id = ccaasInfo->get_session_id();
	std::string path = ccaasInfo->get_file_name();
	std::string type = "voicemail";
	Document::AllocatorType& allocator = doc.GetAllocator();
	outputVal.AddMember("session_id", setJsonParam(Id,doc),allocator);
	outputVal.AddMember("type", setJsonParam(type,doc),allocator);
	outputVal.AddMember("path", setJsonParam(path,doc),allocator);	
	outputVal.Accept(writer);
	ccaasInfo->set_data(strBuf.GetString());
	XGLOG_INFO("URMRCPModuleBridge::encode_vms_transcript_req_msg encoded text (%s)",ccaasInfo->get_data().c_str());
	return true;
}

bool URMRCPModuleBridge::process_vms_transcript_request(ccaasVmsInfo* pMsg)
{
  XGLOG_INFO("URMRCPModuleBridge:process_vms_transcript_request asr type (%d) for session id(%s) ",pMsg->get_asr_type(),(char*)ur_log_string(pMsg->get_session_id()));
  switch(pMsg->get_asr_type())
  {
    case EN_XGATE_ASR_INHOUSE:
      {
		pMsg->set_vms_pub_type(EN_CCAAS_VMS_PUB_TYPE::EN_CCAAS_VMS_PUB_TYPE_TRANSCRIPT_REQ);
		encode_vms_transcript_req_msg(pMsg);
		m_vmsSttlist.push_back(pMsg->get_session_id());
		postMessageToDbService(pMsg);
		#if 0
        URInhousesttclient *inhouseCli = new URInhousesttclient(pMsg);
        if(!inhouseCli)
        {
          XGLOG_ERROR("URMRCPModuleBridge:process_vms_transcript_request create inhouse client failed for session id(%s) ",(char*)ur_log_string(pMsg->get_session_id()));
          return false;
        }
        XGLOG_INFO("URMRCPModuleBridge:process_vms_transcript_request created inhouse client for session id(%s) ",(char*)ur_log_string(pMsg->get_session_id()));
        m_tSttInhouse.insert({pMsg->get_session_id(),inhouseCli});
		#endif
      }
      break;
    case EN_XGATE_ASR_SPEECH_MATICS:
      {
        URSpeechmaticssttclient *smaticCli = new URSpeechmaticssttclient(pMsg);
        if(!smaticCli)
        {
          XGLOG_ERROR("URMRCPModuleBridge:process_vms_transcript_request create speechmatic client failed for session id(%s) ",(char*)ur_log_string(pMsg->get_session_id()));
          return false;
        }
        m_tSttObj.insert({pMsg->get_session_id(),smaticCli});
        break;
      }
    default:
      break;
  }
  deleteServiceMsg(pMsg);
  return true;
}

bool URMRCPModuleBridge::process_mrcp_received_dtmf_digit(xGateDbConfigMgrServiceMsg *pMsg)
{
	UrMrcpSessionDetails *sessionInfo = NULL;
	XGLOG_INFO("URMRCPModuleBridge::process_mrcp_received_dtmf_digit callid - %s ",pMsg->m_callId.c_str());
	std::map<std::string,UrMrcpSessionDetails *>::iterator it = m_sessionDetailMap.find(pMsg->m_callId);
	if(it != m_sessionDetailMap.end() && it->second)
	{
		sessionInfo = it->second;
	}
	else 
	{
		XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_start_req session details not found for callid (%s) ",pMsg->m_callId.c_str());
		return false;
	}
	if(sessionInfo->m_dtmf_digits.empty())
	{
		sessionInfo->m_dtmf_digits = pMsg->m_dtmf_digit.c_str();
		postMessageToSchedulerService(pMsg->m_callId.c_str(), EN_SCHEDULER_DTMF_INTERDIGIT_TIMER, SCHEDULER_FUNCTION_SET_TIMER);
	}
	else if(!sessionInfo->m_dtmf_digits.empty())
	{
		postMessageToSchedulerService(pMsg->m_callId.c_str(), EN_SCHEDULER_DTMF_INTERDIGIT_TIMER, SCHEDULER_FUNCTION_CANCEL_TIMER);
		sessionInfo->m_dtmf_digits += pMsg->m_dtmf_digit.c_str();
		XGLOG_INFO("URMRCPModuleBridge::process_mrcp_received_dtmf_digit Setting timer for callid - %s ",pMsg->m_callId.c_str());
		postMessageToSchedulerService(pMsg->m_callId.c_str(), EN_SCHEDULER_DTMF_INTERDIGIT_TIMER, SCHEDULER_FUNCTION_SET_TIMER);
	}
	if(sessionInfo->m_sessionRecogState == EN_SESSION_RECOG_START)
	{
		sessionInfo->m_sessionRecogState = EN_SESSION_RECOG_STOP;
		//process_mrcp_recognition_stop_req(pMsg);// sending stop recognize when dtmf digit received
	}
#if 0 
	pMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
	pMsg->m_convertedText = pMsg->m_dtmf_digit.c_str();
	PostMessageToMrcpMg(pMsg); 
#endif
	return true;
}

bool URMRCPModuleBridge::process_mrcp_scheduler_msg(xGateSchedulerServiceMsg *pMsg)
{
	xGateDbConfigMgrServiceMsg* mrcpServiceMsg = new xGateDbConfigMgrServiceMsg;
	mrcpServiceMsg->m_callId = pMsg->getUid();
	UrMrcpSessionDetails *sessionInfo = NULL;
	std::map<std::string,UrMrcpSessionDetails *>::iterator it = m_sessionDetailMap.find(mrcpServiceMsg->m_callId);
	if(it != m_sessionDetailMap.end() && it->second)
	{
		sessionInfo = it->second;
	}
	else
	{
		XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_scheduler_msg session details not found for callid (%s) ",mrcpServiceMsg->m_callId.c_str());
		return false;
	}
	SchedulerInfo *schedulerInfo = &pMsg->get_scheduler_info();
	switch(pMsg->get_scheduler_info().m_schedulerType)
	{
		case EN_SCHEDULER_DTMF_INTERDIGIT_TIMER:
			{
				printf("! timeout triggered EN_SCHEDULER_DTMF_INTERDIGIT_TIMER - %s\n",mrcpServiceMsg->m_callId.c_str());
				if(sessionInfo->m_dtmf_digits.empty())
					return false;
				mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
				mrcpServiceMsg->m_convertedText = sessionInfo->m_dtmf_digits.c_str();
				sessionInfo->m_dtmf_digits = "";
				PostMessageToMrcpMg(mrcpServiceMsg);
				if(sessionInfo->m_sessionSynthState == EN_SESSION_SYNTH_START)
				{
					process_mrcp_synthesize_stop_req(mrcpServiceMsg);
				}
				if(sessionInfo->m_sessionRecogState == EN_SESSION_RECOG_START)
				{
					sessionInfo->m_sessionRecogState = EN_SESSION_RECOG_STOP;
					//process_mrcp_recognition_stop_req(mrcpServiceMsg);// sending stop recognize when dtmf digit received
				}
			}
			break;
		case EN_SCHEDULER_BOT_RESP_WAIT_TIMER:
			{
				printf("! timeout triggered EN_SCHEDULER_BOT_RESP_WAIT_TIMER - %s\n",mrcpServiceMsg->m_callId.c_str());
				//mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_RECOGNITION_START_REQ);
				//mrcpServiceMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
				//handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(mrcpServiceMsg));
				if(sessionInfo->m_sessionRecogState == EN_SESSION_RECOG_START || sessionInfo->m_sessionSynthState == EN_SESSION_SYNTH_START)
				{
					XGLOG_DEBUG("URMRCPModuleBridge::process_mrcp_scheduler_msg either recognize or synthesis is inprogress for callid(%s)",(char*)ur_log_string(mrcpServiceMsg->m_callId));
					break;
				} 

				mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
				// mrcpServiceMsg->m_disconnectReason = sessionInfo->m_ccaasBotErrmsg = "bot_failed";
				sessionInfo->m_pubChannelId = xGatePubChannelId::EN_CCAAS_ACD_MG_COMM;
				sessionInfo->m_eventId = xGateMgAcdEvent::EN_CCAAS_ACD_MG_BOT_FAILED;
				cout<<"!!! Bot failed "<<endl;
				XGLOG_INFO("URMRCPModuleBridge::process_mrcp_scheduler_msg No Response form BOT, so sending 'bot_failed' msg to ACD for uid(%s) callid(%s)",(char*)ur_log_string(mrcpServiceMsg->m_uid),(char*)ur_log_string(mrcpServiceMsg->m_callId));
#if 0
				if(sessionInfo->m_asrType == EN_XGATE_ASR_SPEECH_MATICS)
				{
					mrcpServiceMsg->set_mrcp_command(EN_STT_MSG_RECOGNITION_START_REQ);
					XGLOG_INFO("URMRCPModuleBridge::process_mrcp_scheduler_msg No Response from Avaamo so setting asr type as Speechmatics");
					printf("!!!!! URMRCPModuleBridge::process_mrcp_scheduler_msg No Response from Avaamo so setting asr type as Speechmatics !!!!!");
				}
				else if(sessionInfo->m_asrType == EN_XGATE_ASR_VOICE_GAIN)
				{
					mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_RECOGNITION_START_REQ);
					XGLOG_INFO("URMRCPModuleBridge::process_mrcp_scheduler_msg No Response from Avaamo so setting asr type as Voicegain");
					printf("!!!! URMRCPModuleBridge::process_mrcp_scheduler_msg No Response from Avaamo so setting asr type as Voicegain !!!!");
				}
				else
				{
					XGLOG_DEBUG("URMRCPModuleBridge::process_mrcp_scheduler_msg Unknown asr_type for callid(%s)",mrcpServiceMsg->m_callId.c_str());
					return false;
				}
#endif
				PostMessageToMrcpMg(mrcpServiceMsg);
			}
			break;
			case EN_SCHEDULER_CUSTOMER_RESP_WAIT_TIMER:
			{
				XGLOG_INFO("URMRCPModuleBridge::process_mrcp_scheduler_msg timeout occured 'EN_SCHEDULER_CUSTOMER_RESP_WAIT_TIMER' callID[%s]",mrcpServiceMsg->m_callId.c_str());
				mrcpServiceMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
				mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_SYNTHESIZE_REQ);
				mrcpServiceMsg->m_displayText = "Are you still there?";
				mrcpServiceMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
				handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(mrcpServiceMsg));
			}
			break;
	}
	return true;
}

bool URMRCPModuleBridge::process_mrcp_synthesize_stop_req(xGateDbConfigMgrServiceMsg *pMsg)
{
	if(!pMsg)
	{
		XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_synthesize_stop_req pMsg not found!!");
		return false;
	}
	xGateDbConfigMgrServiceMsg *mrcpServiceMsg = new xGateDbConfigMgrServiceMsg;
	mrcpServiceMsg->m_tSessionId = mrcpServiceMsg->m_callId = pMsg->m_callId.c_str();
	mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_SYNTHESIZE_STOP);
	mrcpServiceMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
	handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(mrcpServiceMsg));
	return true;
}

bool URMRCPModuleBridge::process_mrcp_msg_from_db_service(UrMrcpDbServiceMsg* dbService)
{
	xGateDbConfigMgrServiceMsg *pMsg = new xGateDbConfigMgrServiceMsg;
	pMsg->m_callId = dbService->get_session_uid();
	std::map<std::string,UrMrcpSessionDetails *>::iterator it = m_sessionDetailMap.find(pMsg->m_callId);
	if(it != m_sessionDetailMap.end() && it->second)
	{
		if(!dbService->get_isCcaasAgentConnected())
		{
			std::cout<<"!timer setting"<<std::endl;
			postMessageToSchedulerService(pMsg->m_callId.c_str(), EN_SCHEDULER_BOT_RESP_WAIT_TIMER, SCHEDULER_FUNCTION_SET_TIMER);
		}	
	}
	else
	{
		std::cout<<"!URMRCPModuleBridge::process_mrcp_msg_from_db_service session details not found"<<std::endl;
		XGLOG_ERROR("URMRCPModuleBridge::process_mrcp_msg_from_db_service session details not found for callid (%s) ",pMsg->m_callId.c_str());
		return false;
	}
	/*if(!dbService->get_isCcaasAgentConnected() && it->second->m_ccaasBotErrmsg.empty())
	{
		std::cout<<"!timer setting"<<std::endl;
		postMessageToSchedulerService(pMsg, EN_SCHEDULER_AVAAMO_RESP_WAIT_TIMER, SCHEDULER_FUNCTION_SET_TIMER);
	}*/
	deleteServiceMsg(pMsg);
	deleteServiceMsg(dbService);
	return true;
}

std::string URMRCPModuleBridge::frameDisplayInfo(string ext,string domain_id,string device)
{
	string display_info("");

	display_info = ext.c_str();
	display_info.append("-");
	display_info.append(domain_id.c_str());
	display_info.append("-");
	display_info.append(device.c_str());

	return display_info;
}

Value URMRCPModuleBridge::setJsonParam(string &strValue, Document &doc)
{
	Value retVal;
	retVal.SetString(strValue.c_str(), strValue.length(), doc.GetAllocator());
	return retVal;
}

Value URMRCPModuleBridge::setJsonParam(unsigned int iValue, Document &doc)
{
	Value retVal;
	retVal = iValue;
	return retVal;
}

UrMrcpSessionDetails * URMRCPModuleBridge::decode_mrcp_sdp_message(const char* jsonData)
{
	UrMrcpSessionDetails *sessionInfo = new UrMrcpSessionDetails();

	XGLOG_INFO( "parsing business json data: %s", jsonData);

	Document doc;
	ParseResult ok = doc.Parse(jsonData);

	if(!ok) {
		XGLOG_ERROR( "parseJsonMsg failed. Encountered with json parse error !");
		return nullptr;
	} else if(!doc.IsObject()) {
		XGLOG_ERROR( "parseJsonMsg failed. Not able create documnet object !");
		return nullptr;
	}

	if(doc.HasMember("mg_resource_id")) {
		sessionInfo->m_mgResourceId = doc["mg_resource_id"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'mg_resource_id' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("call_id")) {
		sessionInfo->m_callId = doc["call_id"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'call_id' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("codec")) {
		sessionInfo->m_codec = doc["codec"].GetInt();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'codec' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("codec_name")) {
		sessionInfo->m_codecName = doc["codec_name"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'codec_name' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("ptime")) {
		sessionInfo->m_ptime = doc["ptime"].GetInt();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'ptime' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("mrcp_codec")) {
		sessionInfo->m_mrcpCodec = doc["mrcp_codec"].GetInt();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'mrcp_codec' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("mrcp_codec_name")) {
		sessionInfo->m_mrcpCodecName = doc["mrcp_codec_name"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'mrcp_codec_name' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("mrcp_ptime")) {
		sessionInfo->m_mrcpPtime = doc["mrcp_ptime"].GetInt();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'mrcp_ptime' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("mrcp_mg_ip")) {
		sessionInfo->m_mrcpMgIp = doc["mrcp_mg_ip"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'mrcp_mg_ip' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("mrcp_mg_port")) {
		sessionInfo->m_mrcpMgPort = doc["mrcp_mg_port"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'mrcp_mg_port' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("mrcp_mg_ttsport")) {
		sessionInfo->m_mrcpMgTtsPort = doc["mrcp_mg_ttsport"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'mrcp_mg_ttsport' detail is missing in json message !");
	//	return nullptr;
	}

	if(doc.HasMember("mrcp_server_ip")) {
		sessionInfo->m_mrcpServerIp = doc["mrcp_server_ip"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'mrcp_server_ip' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("mrcp_server_port")) {
		sessionInfo->m_mrcpServerPort = doc["mrcp_server_port"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'mrcp_server_port' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("mrcpDomainid")) {
		sessionInfo->m_mrcpDomainId = doc["mrcpDomainid"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'mrcpDomainid' detail is missing in json message !");
		return nullptr;
	}


	if(doc.HasMember("mrcp_Devicetype")) {
		sessionInfo->m_mrcpDeviceType = doc["mrcp_Devicetype"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'mrcp_Devicetype' detail is missing in json message !");
		return nullptr;
	}


	if(doc.HasMember("mrcp_Ext")) {
		sessionInfo->m_mrcpExtNum = doc["mrcp_Ext"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'mrcp_Ext' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("mrcp_isSttEnabled")) {
		sessionInfo->m_mrcpSttEnabled = doc["mrcp_isSttEnabled"].GetInt();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'mrcp_isSttEnabled' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("tts_enabled")) {
		sessionInfo->botNumber = doc["tts_enabled"].GetInt();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'tts_enabled' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("is_ccaas")) {
		sessionInfo->m_isCcaas = doc["is_ccaas"].GetInt();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'is_ccaas' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("ccaas_port_type")) {
		sessionInfo->m_ccaasPorttype = doc["ccaas_port_type"].GetInt();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'ccaas_port_type' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("ccaas_user_agent")) {
		sessionInfo->m_ccaasUsertype = doc["ccaas_user_agent"].GetInt();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'ccaas_user_agent' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("tts_fd")) {
		sessionInfo->m_ttsWriteFd = doc["tts_fd"].GetInt();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'tts_fd' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("ccaas_agent_name")) {
		sessionInfo->m_strCcaasAgentName = doc["ccaas_agent_name"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'ccaas_agent_name' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("ccaas_agent_email")) {
		sessionInfo->m_strCcaasAgentEmail = doc["ccaas_agent_email"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'ccaas_agent_email' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("ccaas_user_id")) {
		sessionInfo->m_strCcaasAgentUserId = doc["ccaas_user_id"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'ccaas_user_id' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("ccaas_cust_firstname")) {
		sessionInfo->m_strCcaasCustomerFirstName = doc["ccaas_cust_firstname"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'ccaas_cust_firstname' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("ccaas_cust_lastname")) {
		sessionInfo->m_strCcaasCustomerLastName = doc["ccaas_cust_lastname"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'ccaas_cust_lastname' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("ccaas_auto_attendant")) {
		sessionInfo->m_iIsCcaasAutoAttendant = doc["ccaas_auto_attendant"].GetInt();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'ccaas_auto_attendant' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("ccaas_virtual_agent")) {
		sessionInfo->m_bIsCcaasVirtualAgent = doc["ccaas_virtual_agent"].GetInt();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'ccaas_virtual_agent' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("ccaas_cli")) {
		sessionInfo->m_strCcaasCli = doc["ccaas_cli"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'ccaas_cli' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("ccaas_ddi")) {
		sessionInfo->m_strCcaasDdi = doc["ccaas_ddi"].GetString();
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'ccaas_ddi' detail is missing in json message !");
		return nullptr;
	}

	if(doc.HasMember("is_smart_assist")) {
		sessionInfo->setIsSmartAssist(doc["is_smart_assist"].GetInt());
	} else {
		XGLOG_ERROR("decode_mrcp_sdp_message failed. 'is_smart_assist' detail is missing in json message !");
		return nullptr;
	}

	//std::string displayUser = frameDisplayInfo(sessionInfo->m_mrcpExtNum,sessionInfo->m_mrcpDomainId,sessionInfo->m_mrcpDeviceType);

	return sessionInfo;
}

std::string URMRCPModuleBridge::encode_mrcp_sdp_message(UrMrcpSessionDetails* sessionDetails)
{
	Document doc;
	StringBuffer strBuf;
	Writer<StringBuffer> writer(strBuf);
	Value outputVal(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();

	outputVal.AddMember("mg_resource_id",setJsonParam(sessionDetails->m_mgResourceId, doc),allocator);
	outputVal.AddMember("call_id",setJsonParam(sessionDetails->m_callId, doc),allocator);
	outputVal.AddMember("codec",setJsonParam(sessionDetails->m_codec,doc),allocator);
	outputVal.AddMember("codec_name",setJsonParam(sessionDetails->m_codecName,doc),allocator);
	outputVal.AddMember("ptime",setJsonParam(sessionDetails->m_ptime,doc),allocator);
	outputVal.AddMember("mrcp_codec",setJsonParam(sessionDetails->m_mrcpCodec,doc),allocator);
	outputVal.AddMember("mrcp_codec_name",setJsonParam(sessionDetails->m_mrcpCodecName,doc),allocator);
	outputVal.AddMember("mrcp_ptime",setJsonParam(sessionDetails->m_mrcpPtime,doc),allocator);
	outputVal.AddMember("mrcp_mg_ip",setJsonParam(sessionDetails->m_mrcpMgIp,doc),allocator);
	outputVal.AddMember("mrcp_mg_port",setJsonParam(sessionDetails->m_mrcpMgPort,doc),allocator);
	outputVal.AddMember("mrcp_server_ip",setJsonParam(sessionDetails->m_mrcpServerIp,doc),allocator);
	outputVal.AddMember("mrcp_server_port",setJsonParam(sessionDetails->m_mrcpServerPort,doc),allocator);
	outputVal.Accept(writer);
	string oss("");
	oss = strBuf.GetString();
	return oss;

}

bool URMRCPModuleBridge::PostMessageToMrcpMg(xGateDbConfigMgrServiceMsg *mrcpServiceMsg)
{
	if ((!mrcpServiceMsg) || (mrcpServiceMsg->m_callId.empty())) 
	{
		XGLOG_ERROR("URMRCPModuleBridge::PostMessageToMrcpMg invaild message ");
		return false;
	}
	XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg uID[%s] callID[%s]",(char*)ur_log_string(mrcpServiceMsg->m_uid),(char*)ur_log_string(mrcpServiceMsg->m_callId));
	UrMrcpSessionDetails *sessionInfo = NULL;
	std::map<std::string,UrMrcpSessionDetails *>::iterator it = m_sessionDetailMap.find(mrcpServiceMsg->m_callId);
	if(it!=m_sessionDetailMap.end() && it->second)
	{
	  sessionInfo = it->second;
	}
	else
	{
	  XGLOG_ERROR("URMRCPModuleBridge::PostMessageToMrcpMg session info not found for callid(%s)",(char*)ur_log_string(mrcpServiceMsg->m_callId));
	  return false;
	}	
	
	if (sessionInfo->m_callId.empty() || sessionInfo->m_mgResourceId.empty())
	{
		XGLOG_ERROR("URMRCPModuleBridge::PostMessageToMrcpMg session info callId or resource id is empty callid(%s)",
				(char*)ur_log_string(mrcpServiceMsg->m_callId));
		return false;
	}

	xGateMRCPMsg* pMgMsg = NULL;
	pMgMsg = new xGateMRCPMsg();

	pMgMsg->set_mrcp_call_id(sessionInfo->m_callId);
	pMgMsg->set_mrcp_uid(sessionInfo->m_mgResourceId);

	XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg Session uID[%s] callID[%s]",(char*)ur_log_string(sessionInfo->m_mgResourceId),(char*)ur_log_string(sessionInfo->m_callId));

	if(sessionInfo->m_bIsVoiceBotDeInit)
	{
		mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
		sessionInfo->m_pubChannelId = xGatePubChannelId::EN_CCAAS_ACD_MG_COMM;
		sessionInfo->m_eventId = xGateMgAcdEvent::EN_CCAAS_ACD_MG_BOT_SHUTDOWN;
		sessionInfo->m_bIsVoiceBotDeInit = false;
	}

	switch(mrcpServiceMsg->get_mrcp_command())
	{
		case EN_MRCP_MSG_POST_MSG_TO_DB:
		{
			printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$EN_MRCP_MSG_POST_MSG_TO_DB$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
			if(mrcpServiceMsg)
			{
				UrMrcpDbServiceMsg *dbServiceMsg = new UrMrcpDbServiceMsg();
				if(dbServiceMsg)
				{
					std::map<std::string,UrMrcpSessionDetails *>::iterator it = m_sessionDetailMap.find(mrcpServiceMsg->m_callId);
					if(it!=m_sessionDetailMap.end())
					{
						if(!sessionInfo)
						{
							XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg session details not exists for SessionId(%s) ",mrcpServiceMsg->m_callId.c_str());
							return false;
						}
						if(sessionInfo->m_sessionRecogState == EN_SESSION_RECOG_START && (sessionInfo->m_isCcaas && (!sessionInfo->m_isCcaasAgentConnected && sessionInfo->m_eventId != xGateMgAcdEvent::EN_CCAAS_ACD_MG_GET_VARIABLE && sessionInfo->m_eventId != xGateMgAcdEvent::EN_CCAAS_ACD_MG_BOT_INDENT)))
						{
							sessionInfo->m_sessionRecogState = EN_SESSION_RECOG_COMPLETED;
						}
					}
					else
					{
						XGLOG_ERROR("URMRCPModuleBridge::PostMessageToMrcpMg session details not exists for SessionId(%s) ",mrcpServiceMsg->m_callId.c_str());
						return false;
					}

					dbServiceMsg->m_speaker = sessionInfo->m_mrcpExtNum;
					dbServiceMsg->m_uid = sessionInfo->m_mgResourceId.c_str();

					std::map<std::string,std::list<Display_Info>>::iterator itr = m_mapDisplay.find(sessionInfo->m_mgResourceId);
					if(itr != m_mapDisplay.end())
					{
						for(auto l : itr->second)
						{
							dbServiceMsg->m_ldisplayUsers.push_back(l);
						}
					}
					else
					{
						XGLOG_ERROR("URMRCPModuleBridge::PostMessageToMrcpMg Display_Info not found in m_mapDisplay uID[%s]",(char*)ur_log_string(sessionInfo->m_mgResourceId));
					}

					dbServiceMsg->m_displayText = mrcpServiceMsg->m_convertedText;
					dbServiceMsg->m_avaamoBotName = sessionInfo->m_botName;
					dbServiceMsg->m_isCcaas = sessionInfo->m_isCcaas;
					dbServiceMsg->m_ccaasUsertype = sessionInfo->m_ccaasUsertype;
					dbServiceMsg->set_isCcaasAgentConnected(sessionInfo->m_isCcaasAgentConnected);
					dbServiceMsg->set_avaamo_request_type("message");
					dbServiceMsg->display.ext = sessionInfo->m_mrcpExtNum.c_str();
					dbServiceMsg->display.remoteExtn = sessionInfo->m_remoteExtn.c_str();
					dbServiceMsg->display.domain_id = sessionInfo->m_mrcpDomainId.c_str();
					dbServiceMsg->set_ccaasBlockId(sessionInfo->m_ccaasBlockId.c_str());
					dbServiceMsg->set_ccaasBlockType(sessionInfo->m_ccaasBlockType.c_str());
					dbServiceMsg->set_ccaasChannelId(sessionInfo->m_ccaasChannelId.c_str());
					dbServiceMsg->set_ccaasCallStatus(mrcpServiceMsg->m_ccaasCallStatus.c_str());
					dbServiceMsg->set_ccaasBotErrmsg(mrcpServiceMsg->m_disconnectReason.c_str());
					dbServiceMsg->m_pubChannelId = sessionInfo->m_pubChannelId;
					dbServiceMsg->set_ccaasAgentId(sessionInfo->m_ccaasAgentId.c_str());
					dbServiceMsg->set_eventId(sessionInfo->m_eventId);
					dbServiceMsg->set_base64Path(sessionInfo->m_base64Path.c_str());					
					dbServiceMsg->set_ccaasBotUrl(sessionInfo->m_ccaasBotUrl.c_str());				
					dbServiceMsg->set_bot_id(sessionInfo->m_iCcaasBotId);
					dbServiceMsg->m_strCcaasAgentName = sessionInfo->m_strCcaasAgentName.c_str();
					dbServiceMsg->m_strCcaasAgentEmail = sessionInfo->m_strCcaasAgentEmail.c_str();
					dbServiceMsg->m_strCcaasAgentUserId = sessionInfo->m_strCcaasAgentUserId.c_str();
					dbServiceMsg->m_strCcaasCustomerFirstName = sessionInfo->m_strCcaasCustomerFirstName.c_str();
					dbServiceMsg->m_strCcaasCustomerLastName = sessionInfo->m_strCcaasCustomerLastName.c_str();
					dbServiceMsg->m_strBotIndent = sessionInfo->m_strBotIndent.c_str();
					dbServiceMsg->m_iIsCcaasAutoAttendant = sessionInfo->m_iIsCcaasAutoAttendant;
					dbServiceMsg->m_bIsCcaasVirtualAgent = sessionInfo->m_bIsCcaasVirtualAgent;
					dbServiceMsg->m_strCcaasCli = sessionInfo->m_strCcaasCli.c_str();
					dbServiceMsg->m_strCcaasDdi = sessionInfo->m_strCcaasDdi.c_str();
					dbServiceMsg->setQid(sessionInfo->getQid());
					dbServiceMsg->setUserExt(sessionInfo->m_mrcpExtNum.c_str());
					dbServiceMsg->setSmartAssistEvent(sessionInfo->getSmartAssistEvent());
					dbServiceMsg->setIsSmartAssist(sessionInfo->getIsSmartAssist());
					dbServiceMsg->setBotType(sessionInfo->getBotType());

					std::map<std::string,UrMrcpSessionDetails *>::iterator IT = m_sessionDetailMap.find(sessionInfo->m_mgResourceId);
					if(IT!=m_sessionDetailMap.end())
					{
						dbServiceMsg->m_strDocMessage = IT->second->m_strDocSocketMessage.c_str();
					}
					dbServiceMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_REDIS_DB_SERVICE);
					handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(dbServiceMsg));

				}
				deleteServiceMsg(mrcpServiceMsg);
				return true;
			}
		}
		case EN_MRCP_MSG_REMOVE_PARTICIPANT_RES:
		{
			std::multimap<std::string, std::string>::iterator itr = m_callDetailsMap.find(mrcpServiceMsg->m_uid);
			break;
		}
		case EN_MRCP_MSG_TERMINATE:
		{
			if(mrcpServiceMsg)
			{
				XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg EN_MRCP_MSG_TERMINATE for callid(%s)",
                         mrcpServiceMsg->m_callId.c_str());
				pMgMsg->set_mrcp_call_id(mrcpServiceMsg->m_callId);
				pMgMsg->set_mrcp_uid(mrcpServiceMsg->m_callId);
				pMgMsg->set_disconnection_code(mrcpServiceMsg->m_disconnectCode);
				pMgMsg->set_disconnection_reason(mrcpServiceMsg->m_disconnectReason);
				pMgMsg->set_mrcp_command(EN_MRCP_MSG_TERMINATE);//sending call terminate request to mg
				mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_RELEASE); // Terminate the mrcp sessions
				mrcpServiceMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
				handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(mrcpServiceMsg));
			}
			break;
		}
		case EN_MRCP_MSG_DELETE_SESSIONINFO:
		{
			UrMrcpSessionDetails  *sessionInfo = NULL;
			std::string callID = mrcpServiceMsg->m_callId.c_str();
			XGLOG_INFO("URMRCPModuleBridge:: Received EN_MRCP_MSG_DELETE_SESSIONINFO for callid(%s)",
			mrcpServiceMsg->m_callId.c_str());
			std::map<std::string,UrMrcpSessionDetails *>::iterator it = m_sessionDetailMap.find(mrcpServiceMsg->m_callId);

			if(it!=m_sessionDetailMap.end())
			{
				sessionInfo = it->second;
			}
			if (sessionInfo)
			{
				URMRCPMODULEBRIDGE->m_sessionDetailMap.erase(it);
				delete sessionInfo;
				sessionInfo = NULL;
			}
			break;
		}
		case EN_MSG_TRANSFER_REQ:
		{
			if(mrcpServiceMsg)
			{
				pMgMsg->set_mrcp_call_id(mrcpServiceMsg->m_callId);
				pMgMsg->set_mrcp_uid(mrcpServiceMsg->m_callId);
				XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg EN_MSG_TRANSFER_REQ for callid(%s)",mrcpServiceMsg->m_callId.c_str());
				pMgMsg->set_transfer_to_number(mrcpServiceMsg->get_transfer_to());

				pMgMsg->set_mrcp_command(EN_MSG_TRANSFER_REQ);
				pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
			}
			break;
		}
		case EN_STT_MSG_RECOGNITION_START_REQ:
		{
			pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
			if(mrcpServiceMsg)
			{
				std::map<std::string,UrMrcpSessionDetails *>::iterator it = m_sessionDetailMap.find(mrcpServiceMsg->m_callId);
				if(it != m_sessionDetailMap.end() && it->second)
				{
					it->second->m_sessionRecogState = EN_SESSION_RECOG_START;
				}
				pMgMsg->set_mrcp_call_id(mrcpServiceMsg->m_callId);
				pMgMsg->set_mrcp_uid(mrcpServiceMsg->m_callId);
				XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg EN_STT_MSG_RECOGNITION_START_REQ for callid(%s)",mrcpServiceMsg->m_callId.c_str());
				pMgMsg->set_mrcp_command(EN_STT_MSG_RECOGNITION_START_REQ);
			}
			break;
		}
		case EN_STT_TTS_MSG_ALLOCATE_RES:
		{
			pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
			if(mrcpServiceMsg)
			{
				XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg EN_STT_TTS_MSG_ALLOCATE_RES for callid(%s)",mrcpServiceMsg->m_callId.c_str());
				std::map<std::string,UrMrcpSessionDetails *>::iterator it = m_sessionDetailMap.find(mrcpServiceMsg->m_callId);
				if(it != m_sessionDetailMap.end() && it->second)
				{
					pMgMsg->set_mrcp_call_id(it->second->m_callId);
					pMgMsg->set_mrcp_uid(it->second->m_mgResourceId);
					pMgMsg->set_mrcp_command(EN_STT_TTS_MSG_ALLOCATE_RES);
				}
				else 
				{
					XGLOG_ERROR("URMRCPModuleBridge::PostMessageToMrcpMg EN_STT_TTS_MSG_ALLOCATE_RES session not found for callid(%s)",mrcpServiceMsg->m_callId.c_str());
					return false;
				}
			}
			else
			{
				return false;
			}
			break;
		}
		case EN_TTS_CHANNEL_START_REQ:
		{
			XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg sending EN_TTS_CHANNEL_START_REQ` for callid(%s)",sessionInfo->m_callId.c_str());
			pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
			pMgMsg->set_mrcp_command(EN_TTS_CHANNEL_START_REQ);
		}
		break;
		case EN_TTS_CHANNEL_STOP_REQ:
		{
			XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg sending 'EN_TTS_CHANNEL_STOP_REQ` for callid(%s)",sessionInfo->m_callId.c_str());
			pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
			pMgMsg->set_mrcp_command(EN_TTS_CHANNEL_STOP_REQ);
		}
		break;
		case EN_TTS_SPEAK_COMPLETE:
		{
			sessionInfo->m_sessionSynthState = EN_SESSION_SYNTH_COMPLETED;
			sessionInfo->setIsPlayTextSent(false);
			std::string callID("");
			callID = mrcpServiceMsg->m_callId.c_str();

			auto itr = URMRCPMODULEBRIDGE->m_callDetailsMap.find(sessionInfo->m_mgResourceId.c_str());
			if(itr != m_callDetailsMap.end())
			{
				if(m_callDetailsMap.count(sessionInfo->m_mgResourceId.c_str())>1)
				{
					if(0==strcmp(sessionInfo->m_callId.c_str(),itr->second.c_str()))
					{
						advance(itr,1);
					}
					std::string callId("");
					callId = itr->second.c_str();
					if(!callID.empty())
					{
						XGLOG_INFO("===== callID [%s]",callId.c_str());
						auto IT = URMRCPMODULEBRIDGE->m_sessionDetailMap.find(callId);
						if(IT!=URMRCPMODULEBRIDGE->m_sessionDetailMap.end())
						{
							IT->second->setIsPlayTextSent(false);
						}
						else
						{
							XGLOG_ERROR("URMRCPModuleBridge::PostMessageToMrcpMg sending EN_TTS_SPEAK_COMPLETE session not found callID [%s]",callId.c_str());
						}
					}
					else
					{
						XGLOG_ERROR("URMRCPModuleBridge::PostMessageToMrcpMg sending EN_TTS_SPEAK_COMPLETE ID [%s] not found in m_callDetailsMap",callId.c_str());
					}
				}
			}
			else
			{
				XGLOG_ERROR("URMRCPModuleBridge::PostMessageToMrcpMg sending EN_TTS_SPEAK_COMPLETE ID [%s] not found in m_callDetailsMap ",sessionInfo->m_mgResourceId.c_str());
			}

			if(!sessionInfo->m_avaamo_reply.empty())
			{
				std::list<bot_reply_msg>::iterator itr = sessionInfo->m_avaamo_reply.begin();
				if(itr != sessionInfo->m_avaamo_reply.end())
				{
					sessionInfo->m_playing = (*itr).m_bOrignal;
					mrcpServiceMsg->m_displayText = (*itr).m_strMessage.c_str();
					mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_SYNTHESIZE_REQ);
					mrcpServiceMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_CLI_APP_SERVICE);
					handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(mrcpServiceMsg));
					sessionInfo->m_avaamo_reply.erase(itr);
					return false;
				}
			}

			if(sessionInfo->m_bisccaasChatBotInit && sessionInfo->m_bisBotFirst)
			{
				if(!sessionInfo->m_isBotWelcomePromptSent)
				{
				XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg sending Bot Welcome Prompt tts type %d", sessionInfo->m_ttsType);
				mrcpServiceMsg->m_convertedText = "hi";
				sessionInfo->m_pubChannelId = xGatePubChannelId::EN_CCAAS_BOT;
				mrcpServiceMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
				PostMessageToMrcpMg(mrcpServiceMsg);
				sessionInfo->m_isBotWelcomePromptSent = true;
				return true;
				}
			}

			if(sessionInfo->getSmartAssistEvent() == EN_BOT_SMART_ASSIST_EVENT::EN_BOT_SMART_ASSIST_EVENT_DIAL_USER)
			{
				xGateDbConfigMgrServiceMsg *ptrMsg = new xGateDbConfigMgrServiceMsg;
				ptrMsg->m_callId = callID.c_str();
				ptrMsg->set_mrcp_command(EN_MRCP_MSG_POST_MSG_TO_DB);
				PostMessageToMrcpMg(ptrMsg);
			}

			pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
			if(!sessionInfo->m_transferTo.empty())
			{
				XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg sending EN_MSG_TRANSFER_REQ for callid(%s)",sessionInfo->m_callId.c_str());
				pMgMsg->set_transfer_to_number(sessionInfo->m_transferTo);
				pMgMsg->set_mrcp_command(EN_MSG_TRANSFER_REQ);
			}
			else
			{
				if(!sessionInfo->m_dtmf_digits.empty())
				{
					XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg dtmf_digits not empty for callid(%s)",(char*)ur_log_string(sessionInfo->m_callId));
					return true;
				}
				if(sessionInfo->m_isCcaas)
				{
					if(sessionInfo->m_tts_text_type == 1)
					{
						pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
						XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg EN_TTS_SPEAK_COMPLETE for callid(%s)",(char*)ur_log_string(sessionInfo->m_callId));
						pMgMsg->set_mrcp_command(EN_TTS_SPEAK_COMPLETE);
						postMessageToMediaService(pMgMsg);
						//return true;
					}
				}
				XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg (%d)  (%d)",sessionInfo->m_ccaasPorttype , sessionInfo->m_bisccaasChatBotInit);
				if(sessionInfo->m_ccaasPorttype == EN_XGATE_BOTH && (sessionInfo->m_bisccaasChatBotInit || sessionInfo->getIsSmartAssist()))
				{
					if(sessionInfo->m_sessionRecogState != EN_SESSION_RECOG_START)
					{
						xGateMRCPMsg* pMsg = NULL;
						pMsg = new xGateMRCPMsg();
						pMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
						pMsg->set_mrcp_call_id(sessionInfo->m_callId);
						pMsg->set_mrcp_uid(sessionInfo->m_mgResourceId);
						switch(sessionInfo->m_asrType)
						{
							case EN_XGATE_ASR_SPEECH_MATICS:
							{
								XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg EN_XGATE_ASR_SPEECH_MATICS sending EN_STT_MSG_RECOGNITION_START_REQ for callid(%s)",(char*)ur_log_string(sessionInfo->m_callId));
								sessionInfo->m_sessionRecogState = EN_SESSION_RECOG_START;
								pMsg->set_mrcp_command(EN_STT_MSG_RECOGNITION_START_REQ);
								break;
							}
							case EN_XGATE_ASR_INHOUSE:
							{
								XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg EN_XGATE_ASR_INHOUSE sending EN_STT_MSG_RECOGNITION_START_REQ for callid(%s)",(char*)ur_log_string(sessionInfo->m_callId));
								sessionInfo->m_sessionRecogState = EN_SESSION_RECOG_START;
								pMsg->set_mrcp_command(EN_STT_MSG_RECOGNITION_START_REQ);
								break;
							}
							case EN_XGATE_ASR_VOICE_GAIN:
							{
								XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg EN_XGATE_ASR_VOICE_GAIN for callid(%s)",(char*)ur_log_string(sessionInfo->m_callId));
								break;
							}
							default:
							{
								XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg asr type is unknown for callid(%s)",(char*)ur_log_string(sessionInfo->m_callId));
							}
						}
						postMessageToMediaService(pMsg);
					}
					
					if(sessionInfo->getSmartAssistEvent() == EN_BOT_SMART_ASSIST_EVENT::EN_BOT_SMART_ASSIST_EVENT_NULL)
					{
						XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg setting timer for Customer response callID[%s]",sessionInfo->m_callId.c_str());
						postMessageToSchedulerService(callID.c_str(), EN_SCHEDULER_CUSTOMER_RESP_WAIT_TIMER, SCHEDULER_FUNCTION_SET_TIMER);
					}
					else if(sessionInfo->getSmartAssistEvent() == EN_BOT_SMART_ASSIST_EVENT::EN_BOT_SMART_ASSIST_EVENT_DIAL_USER)
					{
						XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg cancelling timer for Customer response callID[%s]",sessionInfo->m_callId.c_str());
						postMessageToSchedulerService(callID.c_str(), EN_SCHEDULER_CUSTOMER_RESP_WAIT_TIMER, SCHEDULER_FUNCTION_CANCEL_TIMER);
						sessionInfo->m_pubChannelId = xGatePubChannelId::EN_PUB_ID_UNKNOWN;
					}
				}
			}
			deleteServiceMsg(mrcpServiceMsg);
			return true;
			//  break;
		}
		case EN_RESTART_STT_RECOGNIZATION:
		{
			XGLOG_INFO("URMRCPModuleBridge::PostMessageToMrcpMg EN_RESTART_STT_RECOGNIZATION for callid(%s)",(char*)ur_log_string(mrcpServiceMsg->m_callId));
			pMgMsg->setMsgType(EN_XGATE_MSG_STT_TTS_IN);
			pMgMsg->set_mrcp_command(EN_RESTART_STT_RECOGNIZATION);
			break;
		}
		default:
		{
			case EN_MRCP_MSG_UNKNOWN:
			break;
		}
	}
	postMessageToMediaService(pMgMsg);
	deleteServiceMsg(mrcpServiceMsg);
}

bool URMRCPModuleBridge::postMessageToMediaService(xGateMRCPMsg* pMgMsg)
{
  if(!pMgMsg)
  {
    XGLOG_ERROR( "URMRCPModuleBridge::postMessageToMediaService pMgMsg is invaild message");
    return false;
  }

  ACE_Task<ACE_MT_SYNCH>* taskPtr = NULL;
  taskPtr = (ACE_Task<ACE_MT_SYNCH>*)xGateUtil::getHmpProcessor();
  if(!taskPtr) {
    XGLOG_ERROR( "URMRCPModuleBridge::postMessageToMediaService HmpProcessor task not found");
    return EN_XGATE_STATUS_ERROR;
  }

  ACE_Message_Block* pAmb = NULL;
  pAmb = static_cast<xGateMRCPMsg*> (pMgMsg);
  ACE_Time_Value tval ((time(NULL) ));
  tval += ACE_Time_Value(0,1);

  if (taskPtr->putq(pAmb, &tval)) {
    XGLOG_INFO( "URMRCPModuleBridge::postMessageToMediaService HmpProcessor enqueue message success");
  } else {
    XGLOG_ERROR( "URMRCPModuleBridge::postMessageToMediaService HmpProcessor enqueue message failed");
	deleteServiceMsg(pMgMsg);
  }
}

bool URMRCPModuleBridge::postMessageToSchedulerService(std::string callID, SchedulerType timerType, TimerFunction timerFunc)
{
	XGLOG_FUNCTION_EN_EX("");

	xGateSchedulerServiceMsg *pSchedulerMsg = new xGateSchedulerServiceMsg();
	if(!pSchedulerMsg)
	{
		XGLOG_ERROR("URMRCPModuleBridge::postMessageToSchedulerService failed to create xGateSchedulerServiceMsg!");
		return;
	}

	if(callID.empty())
	{
		XGLOG_ERROR("URMRCPModuleBridge::postMessageToSchedulerService callID is null!");
		return;
	}

	SchedulerInfo schedulerInfo;;
	schedulerInfo.m_contextId = callID.c_str();
	switch(timerType)
	{
		case EN_SCHEDULER_DTMF_INTERDIGIT_TIMER:
		{
			unsigned int dtmfTimeout = MRCP_CONFIG::instance()->m_timerProfile.m_iDtmfTimeOut;
			schedulerInfo.m_timeoutSecond = (dtmfTimeout > 0) ? dtmfTimeout : 3;
			XGLOG_INFO("URMRCPModuleBridge::postMessageToSchedulerService sending EN_SCHEDULER_DTMF_INTERDIGIT_TIMER request (%d)secs for callid (%s)" ,schedulerInfo.m_timeoutSecond, callID.c_str());
			break;
		}
		case EN_SCHEDULER_BOT_RESP_WAIT_TIMER:
		{
			unsigned int botTimeValue = MRCP_CONFIG::instance()->m_timerProfile.m_iBotResponseTimeOut;
			schedulerInfo.m_timeoutSecond = (botTimeValue > 0) ? botTimeValue : 30;
			XGLOG_INFO("URMRCPModuleBridge::postMessageToSchedulerService sending EN_SCHEDULER_BOT_RESP_WAIT_TIMER request (%d)secs for callid (%s)" ,schedulerInfo.m_timeoutSecond, callID.c_str());
			break;
		}
		case EN_SCHEDULER_CUSTOMER_RESP_WAIT_TIMER:
		{
			unsigned int customerTimeValue = MRCP_CONFIG::instance()->m_timerProfile.m_iCustomerResponseTimeOut;
			schedulerInfo.m_timeoutSecond = (customerTimeValue > 0) ? customerTimeValue : 30;
			XGLOG_INFO("URMRCPModuleBridge::postMessageToSchedulerService sending EN_SCHEDULER_CUSTOMER_RESP_WAIT_TIMER request (%d)secs for callid (%s)" ,schedulerInfo.m_timeoutSecond, callID.c_str());
			break;
		}
	}
	schedulerInfo.m_schedulerType = timerType;
	schedulerInfo.m_retryCount = 0;
	schedulerInfo.m_requestorModule = IURDefines::UR_MODULE_SERVICE_BRIDGE;
	schedulerInfo.m_enumTimerFunc = timerFunc;

	pSchedulerMsg->setUid(callID.c_str());
	pSchedulerMsg->setDstModuleId(IURDefines::UR_MODULE_SCHEDULER_SERVICE);
	pSchedulerMsg->setSrcModuleId(IURDefines::UR_MODULE_SERVICE_BRIDGE);
	pSchedulerMsg->set_scheduler_info(schedulerInfo);
	handleModuleCallbackMsg(dynamic_cast<IURModuleMsg*>(pSchedulerMsg));

	return true;
}

bool URMRCPModuleBridge::postMessageToRedisService(UrMrcpDbServiceMsg* pDbServiceMsg)
{
	XGLOG_FUNCTION_EN_EX("");
  if(!pDbServiceMsg) 
  {
    XGLOG_ERROR("URMRCPModuleBridge::postMessageToRedisService pDbServiceMsg is null");
    return false;
  }

  pDbServiceMsg->m_pubChannelId = xGatePubChannelId::EN_CCAAS_VMS_TRANSCRIPT_NODE;
  pDbServiceMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_REDIS_DB_SERVICE);
  handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pDbServiceMsg));
  return true;
}

bool URMRCPModuleBridge::postMessageToDbService(ccaasVmsInfo* pMsg)
{
  XGLOG_FUNCTION_EN_EX("");
  if(!pMsg) {
    return false;
  }
  switch(pMsg->get_asr_type())
  {
    case EN_XGATE_ASR_INHOUSE:
      {
        std::map<std::string,URInhousesttclient*>::iterator itr = m_tSttInhouse.find(pMsg->get_session_id());
        if(itr != m_tSttInhouse.end() && itr->second)
        {
          XGLOG_INFO("URMRCPModuleBridge:postMessageToDbService removing the inhouse client for callid (%s)" ,(char*)ur_log_string(pMsg->get_session_id()));
          delete itr->second;
          m_tSttInhouse.erase(itr);
        }
        break;
      }
    case EN_XGATE_ASR_SPEECH_MATICS:
      {
        std::map<std::string,URSpeechmaticssttclient*>::iterator itr = m_tSttObj.find(pMsg->get_session_id());
        if(itr != m_tSttObj.end() && itr->second)
        {
          XGLOG_INFO("URMRCPModuleBridge:postMessageToDbService removing the smatic client for callid (%s)" ,(char*)ur_log_string(pMsg->get_session_id()));
          delete itr->second;
          m_tSttObj.erase(itr);
        }
        break;
      }
  }
  if(pMsg->get_data().empty())
  {
    XGLOG_INFO("URMRCPModuleBridge:postMessageToDbService received empty text for callid (%s)" ,(char*)ur_log_string(pMsg->get_session_id()));
    deleteServiceMsg(pMsg);
    return false;
  }
  UrMrcpDbServiceMsg *pDbServiceMsg = new UrMrcpDbServiceMsg();
  if(!pDbServiceMsg) {
    XGLOG_ERROR("URMRCPModuleBridge:postMessageToDbService creating pDbServiceMsg failed for callid (%s)" ,(char*)ur_log_string(pMsg->get_session_id()));
    return false;
  }
  
  if(pMsg->get_vms_pub_type() == EN_CCAAS_VMS_PUB_TYPE::EN_CCAAS_VMS_PUB_TYPE_TRANSCRIPT_REQ)
  {
	  pDbServiceMsg->m_pubChannelId = xGatePubChannelId::EN_CCAAS_VMS_TRANSCRIPT_REQ;
  }
  else if(pMsg->get_vms_pub_type() == EN_CCAAS_VMS_PUB_TYPE::EN_CCAAS_VMS_PUB_TYPE_VMS_NODE)
  {
    pDbServiceMsg->m_pubChannelId = xGatePubChannelId::EN_CCAAS_VMS_TRANSCRIPT_NODE;
    pDbServiceMsg->set_display_text(pMsg->get_data().c_str());
  }
  pDbServiceMsg->set_ccaas_vms_info(pMsg);
  pDbServiceMsg->setDstModuleId(IURDefines::UR_MODULE_MRCP_REDIS_DB_SERVICE);
  handleModuleCallbackMsg(dynamic_cast<IURModuleMsg *>(pDbServiceMsg));
  return true;
}

bool URMRCPModuleBridge::insertSessionInfo(std::string ID, UrMrcpSessionDetails *sessionDetails)
{
	XGLOG_INFO("URMRCPModuleBridge::insertSessionInfo for ID[%s]",(char*)ur_log_string(ID));
	
	if(!sessionDetails)
	{
		XGLOG_ERROR("URMRCPModuleBridge::insertSessionInfo session detail is NULL for ID[%s]",(char*)ur_log_string(ID));
		return false;
	}

	std::unique_lock<std::mutex> lock(m_lksessionDetailMap);
	m_sessionDetailMap.insert({sessionDetails->m_callId,sessionDetails});

	return true;
}

bool URMRCPModuleBridge::getSessionInfo(std::string ID,UrMrcpSessionDetails* &sessionDetails)
{
	XGLOG_INFO("URMRCPModuleBridge::getSessionInfo for ID[%s]",(char*)ur_log_string(ID));

	std::unique_lock<std::mutex> lock(m_lksessionDetailMap);
	auto it = m_sessionDetailMap.find(ID);
	if(it == m_sessionDetailMap.end())
	{
		return false;
	}
	sessionDetails = it->second;
	if(!sessionDetails)
	{
		XGLOG_ERROR("URMRCPModuleBridge::getSessionInfo found session is NULL, ID[%s]",(char*)ur_log_string(ID));
		return false;
	}
	XGLOG_INFO("URMRCPModuleBridge::getSessionInfo session found ID[%s]",(char*)ur_log_string(ID));
	return true;
}

bool URMRCPModuleBridge::insertUserToList(std::string ID,Display_Info displayInfo)
{
	XGLOG_INFO("URMRCPModuleBridge::insertUserToList for ID[%s]",(char*)ur_log_string(ID));

	std::unique_lock<std::mutex> lock(m_lkmapDisplay);
	auto itr = m_mapDisplay.find(ID);
			
	if(itr != m_mapDisplay.end())
	{
		itr->second.push_back(displayInfo);
	}
	else
	{
		std::list<Display_Info> userList;
		userList.push_back(displayInfo);
		m_mapDisplay.insert({ID,userList});
	}
	XGLOG_INFO("URMRCPModuleBridge::insertUserToList user added uID[%s] user[%s]",(char*)ur_log_string(ID),(char*)ur_log_string(displayInfo.ext));
	return true;
}

bool URMRCPModuleBridge::eraseUserFromList(std::string ID,std::string userExt)
{
	XGLOG_INFO("URMRCPModuleBridge::eraseUserFromList for ID[%s]",(char*)ur_log_string(ID));
	
	std::unique_lock<std::mutex> lock(m_lkmapDisplay);
	auto itr = m_mapDisplay.find(ID);
	if(itr != m_mapDisplay.end())
	{
		std::list<Display_Info> &userList = itr->second;
		if(userList.empty())
		{
			XGLOG_ERROR("URMRCPModuleBridge::eraseUserFromList userList is empty for uID[%s]",(char*)ur_log_string(ID));
			return false;
		}
		auto l = userList.begin();
		for(;l!=userList.end();l++)
		{
			std::string ext = (*l).ext;
			if(0==strncmp(ext.c_str(),userExt.c_str(),userExt.length()))
			{
				l = userList.erase(l);
				XGLOG_ERROR("URMRCPModuleBridge::eraseUserFromList removed user[%s] from the display list uID[%s]",(char*)ur_log_string(userExt),(char*)ur_log_string(ID));
			}
		}
	}
	return true;
}

bool URMRCPModuleBridge::deleteSessionInfo(std::string ID)
{
	if(ID.empty())
	{
		XGLOG_ERROR("URMRCPModuleBridge::deleteSessionInfo ID is NULL");
		return false;
	}

	XGLOG_INFO("URMRCPModuleBridge::deleteSessionInfo for uID[%s]",(char*)ur_log_string(ID));

	std::unique_lock<std::mutex> lock(m_lkcallDetailsMap);
	int sessionCount = m_callDetailsMap.count(ID);
	XGLOG_INFO("URMRCPModuleBridge::deleteSessionInfo sessionCount[%d]",sessionCount);
	if(sessionCount==0)
	{
		XGLOG_ERROR("URMRCPModuleBridge::deleteSessionInfo no callID found in callDetailsMap for uID[%s]",(char*)ur_log_string(ID));
		return false;
	}

	while(sessionCount)
	{
		auto it = m_callDetailsMap.find(ID);
		if(it != m_callDetailsMap.end())
		{
			remove_user_from_display_list(ID.c_str(),it->second.c_str());
			deleteUserSessionInfo(it->second);
			it->second = "";
			m_callDetailsMap.erase(it);
			sessionCount--;
		}
		else
		{
			XGLOG_ERROR("URMRCPModuleBridge::deleteSessionInfo Key not found for uID[%s]",(char*)ur_log_string(ID));
			return false;
		}
	}
	return true;
}

bool URMRCPModuleBridge::deleteUserSessionInfo(std::string ID)
{
	if(ID.empty())
	{
		XGLOG_ERROR("URMRCPModuleBridge::deleteUserSessionInfo ID is NULL");
		return false;
	}

	XGLOG_INFO("URMRCPModuleBridge::deleteUserSessionInfo for callID[%s]",(char*)ur_log_string(ID));

	std::unique_lock<std::mutex> lock(m_lksessionDetailMap);
	auto it = m_sessionDetailMap.find(ID);
	if(it != m_sessionDetailMap.end())
	{
		if(it->second)
		{
			delete it->second;
			it->second = nullptr;
		}
		m_sessionDetailMap.erase(it);
	}
	else
	{
		XGLOG_ERROR("URMRCPModuleBridge::deleteUserSessionInfo Session not found for callID[%s]",(char*)ur_log_string(ID));
		return false;
	}
	return true;
}

bool URMRCPModuleBridge::handleTerminationRequest(xGateDbConfigMgrServiceMsg *pMsg)
{
	if(!pMsg)
	{
		XGLOG_ERROR("URMRCPModuleBridge::handleTerminationRequest pMsg is NULL");
		return false;
	}

	XGLOG_INFO("URMRCPModuleBridge::handleTerminationRequest for uID[%s] callID[%s]",(char*)ur_log_string(pMsg->m_uid),(char*)ur_log_string(pMsg->m_callId));
	std::string ID = pMsg->m_uid.c_str();
	deleteSessionInfo(ID);

	return true;
}

bool URMRCPModuleBridge::insertCallSessionId(STR key, STR value)
{
	if(key.empty() || value.empty())
	{
		XGLOG_ERROR("URMRCPModuleBridge::insertCallSessionId either uID or callID is empty - uID[%s] callID[%s]",(char*)ur_log_string(key),(char*)ur_log_string(value));
		return false;
	}

	XGLOG_INFO("URMRCPModuleBridge::insertCallSessionId uID[%s] callID[%s]",(char*)ur_log_string(key),(char*)ur_log_string(value));
	std::unique_lock<std::mutex> lock(m_lkcallDetailsMap);
	m_callDetailsMap.insert({key,value});
	return true;
}

bool URMRCPModuleBridge::getDisplayList(STR ID,std::list<Display_Info> &displayList)
{
	if(ID.empty())
	{
		XGLOG_ERROR("URMRCPModuleBridge::getDisplayList ID is empty - ID[%s]",(char*)ur_log_string(ID));
		return false;
	}

	XGLOG_INFO("URMRCPModuleBridge::getDisplayList ID[%s]",(char*)ur_log_string(ID));

	std::unique_lock<std::mutex> lock(m_lkmapDisplay);
	auto itr = m_mapDisplay.find(ID);
	if(itr != m_mapDisplay.end())
	{
		for(auto it : itr->second)
		{
			displayList.push_back(it);
		}
	}
	else
	{
		XGLOG_ERROR("URMRCPModuleBridge::getDisplayList ID not found - ID[%s]",(char*)ur_log_string(ID));
		return false;
	}
	return true;
}

bool URMRCPModuleBridge::handleSessionForTransferCall(STR uID, STR callID)
{
	if(uID.empty() || callID.empty())
	{
		XGLOG_ERROR("URMRCPModuleBridge::handleSessionForTransferCall either uID or callID is empty - uID[%s] callID[%s]",(char*)ur_log_string(uID),(char*)ur_log_string(callID));
		return false;
	}

	XGLOG_INFO("URMRCPModuleBridge::handleSessionForTransferCall uID[%s] callID[%s]",(char*)ur_log_string(uID),(char*)ur_log_string(callID));

	std::list<Display_Info> displayUserList;

	if(getDisplayList(callID.c_str(),displayUserList))
	{
		for(auto it : displayUserList)
		{
			insertUserToList(uID,it);
		}
	}

	m_tMrcpSessionHandler.DeleteMrcpCallUid(callID.c_str());
	STR ID("");
	if(getCallSessionId(callID.c_str(),ID))
	{
		UrMrcpSessionDetails* sessionDetails = nullptr;
		if(getSessionInfo(ID, sessionDetails))
		{
			sessionDetails->m_mgResourceId = uID.c_str();
		}
	}
	removeFromcallDetailsMap(callID.c_str(),callID.c_str());
	insertCallSessionId(uID.c_str(),callID.c_str());

	return true;
}

bool URMRCPModuleBridge::removeFromcallDetailsMap(STR uID,STR callID)
{
	if(uID.empty() || callID.empty())
	{
		XGLOG_ERROR("URMRCPModuleBridge::removeFromcallDetailsMap either uID or callID is empty - uID[%s] callID[%s]",(char*)ur_log_string(uID),(char*)ur_log_string(callID));
		return false;
	}

	XGLOG_INFO("URMRCPModuleBridge::removeFromcallDetailsMap uID[%s] callID[%s]",(char*)ur_log_string(uID),(char*)ur_log_string(callID));

	std::unique_lock<std::mutex> lock(m_lkcallDetailsMap);

	auto range = m_callDetailsMap.equal_range(uID);

	if(range.first != range.second)
	{
		for(auto itr = range.first; itr != range.second; )
		{
			STR ID = itr->second.c_str();
			XGLOG_INFO("URMRCPModuleBridge::removeFromcallDetailsMap ID [%s] from m_callDetailsMap ",ID.c_str());
			if(0 == strncmp(ID.c_str(),callID.c_str(),callID.length()))
			{
				itr = m_callDetailsMap.erase(itr);
				XGLOG_INFO("URMRCPModuleBridge::removeFromcallDetailsMap ID [%s] removed from m_callDetailsMap ",(char*)ur_log_string(ID));
			}
			else
			{
				itr++;
			}
		}
	}
	else
	{
		XGLOG_ERROR("URMRCPModuleBridge::removeFromcallDetailsMap callID[%s] not found for uID",(char*)ur_log_string(callID),(char*)ur_log_string(uID));
		return false;
	}
	return true;
}

bool URMRCPModuleBridge::getCallSessionId(STR key, STR &value)
{
	if(key.empty())
	{
		XGLOG_ERROR("URMRCPModuleBridge::getCallSessionId uID  is empty - uID[%s]",(char*)ur_log_string(key));
		return false;
	}

	XGLOG_INFO("URMRCPModuleBridge::getCallSessionId uID[%s]",(char*)ur_log_string(key));
	std::unique_lock<std::mutex> lock(m_lkcallDetailsMap);

	auto it = m_callDetailsMap.find(key);

	if(it != m_callDetailsMap.end())
	{
		value = it->second.c_str();
	}
	else
	{
		XGLOG_ERROR("URMRCPModuleBridge::getCallSessionId uID not found in m_callDetailsMap - uID[%s]",(char*)ur_log_string(key));
		return false;
	}

	return true;
}

template <typename T> bool URMRCPModuleBridge::deleteServiceMsg(T *&pMsg)
{
    if(pMsg)
    {
        delete pMsg;
        pMsg = NULL;
    }
    return true;
}
